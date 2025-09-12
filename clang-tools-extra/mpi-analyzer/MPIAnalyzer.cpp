#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory MPIAnalyzerCategory("mpi-analyzer options");

class FunctionBodyVisitor : public RecursiveASTVisitor<FunctionBodyVisitor> {
public:
    FunctionBodyVisitor(ASTContext &Context)
        : Context(Context), usesNonCollective(false), hasRankRootConditional(false) {}

    bool VisitCallExpr(CallExpr *Call) {
        if (const FunctionDecl *FD = Call->getDirectCallee()) {
            std::string Name = FD->getNameInfo().getName().getAsString();
            if (Name == "MPI_Send" || Name == "MPI_Recv" || Name == "MPI_Sendrecv") {
                usesNonCollective = true;
            }
        }
        return true;
    }

    bool VisitIfStmt(IfStmt *If) {
        Expr *Cond = If->getCond();
        if (!Cond) return true;

        std::string CondStr;
        llvm::raw_string_ostream OS(CondStr);
        Cond->printPretty(OS, nullptr, PrintingPolicy(Context.getLangOpts()));

        // crude check for "rank == root" substring in condition
        if (CondStr.find("rank == root") != std::string::npos) {
            hasRankRootConditional = true;
        }
        return true;
    }

    bool usesNonCollective;
    bool hasRankRootConditional;

private:
    ASTContext &Context;
};

class MPIAnalyzer : public MatchFinder::MatchCallback {
public:
    MPIAnalyzer() {}

    void run(const MatchFinder::MatchResult &Result) override {
        const FunctionDecl *Func = Result.Nodes.getNodeAs<FunctionDecl>("func");
        if (!Func || !Func->hasBody()) return;

        ASTContext &Context = *Result.Context;
        SourceManager &SM = Context.getSourceManager();

        FunctionBodyVisitor Visitor(Context);
        Visitor.TraverseStmt(Func->getBody());

        if (Visitor.usesNonCollective && Visitor.hasRankRootConditional) {
            SourceLocation Loc = Func->getLocation();
            PresumedLoc PLoc = SM.getPresumedLoc(Loc);

            unsigned Line = PLoc.isValid() ? PLoc.getLine() : 0;
            llvm::errs() << "==============================\n";
            llvm::errs() << "Analysis of " << Func->getNameAsString() << " Function\n";
            llvm::errs() << "==============================\n";
            llvm::errs() << "Pattern Detected: Non-collective Data Communication\n";
            llvm::errs() << "- Issue: Data is gathered/distributed without collective calls.\n";
            llvm::errs() << "- Location: " << Func->getNameAsString() << " function, Line " << Line << "\n";

            // Print snippet: first 10 lines from function start
            printFunctionSnippet(SM, Loc);

            llvm::errs() << "==============================\n";
        }
    }

private:
    void printFunctionSnippet(SourceManager &SM, SourceLocation Loc) {
        FileID FID = SM.getFileID(Loc);
        bool Invalid = false;
        llvm::StringRef Buffer = SM.getBufferData(FID, &Invalid);
        if (Invalid) return;

        unsigned StartOffset = SM.getFileOffset(Loc);
        if (StartOffset >= Buffer.size())
            return;

        llvm::StringRef FromFuncStart = Buffer.substr(StartOffset);

        // Print first 10 lines or up to buffer end
        int lines = 0;
        for (size_t i = 0; i < FromFuncStart.size() && lines < 10; ++i) {
            char c = FromFuncStart[i];
            llvm::errs() << c;
            if (c == '\n')
                ++lines;
        }
    }
};

int main(int argc, const char **argv) {
    auto ExpectedOptionsParser = CommonOptionsParser::create(argc, argv, MPIAnalyzerCategory);
    if (!ExpectedOptionsParser) {
        llvm::errs() << "Error creating CommonOptionsParser: "
                     << llvm::toString(ExpectedOptionsParser.takeError()) << "\n";
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedOptionsParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    MPIAnalyzer Analyzer;
    MatchFinder Finder;
    Finder.addMatcher(functionDecl(isDefinition()).bind("func"), &Analyzer);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
