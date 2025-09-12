#include <iostream>
#include <set>
#include <string>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory ToolCategory("regular-memory-access-analyzer");
static llvm::cl::opt<bool> AnalyzeRegularMemoryAccess(
    "analyze-regular-memory-access",
    llvm::cl::desc("Enable detection of regular memory access patterns"),
    llvm::cl::init(false),
    llvm::cl::cat(ToolCategory));

class RegularMemoryAccessCallback : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        if (const FunctionDecl *Func = Result.Nodes.getNodeAs<FunctionDecl>("funcDecl")) {
            if (!Func->hasBody())
                return;

            const Stmt *Body = Func->getBody();
            std::set<std::string> accesses;
            bool hasIrregular = false;

            for (const Stmt *Child : Body->children()) {
                if (!Child) continue;
                collectAccesses(Child, Result, accesses, hasIrregular);
            }

            llvm::outs() << "Analyzing function '" << Func->getNameInfo().getName().getAsString() << "'...\n";

            if (accesses.empty()) {
                llvm::outs() << "- No memory access patterns found.\n";
                return;
            }

            if (accesses.size() > 1)
                llvm::outs() << "- Warning: multiple memory access types found in this function.\n";

            for (const auto &acc : accesses)
                llvm::outs() << acc << "\n";

            if (hasIrregular)
                llvm::outs() << "- This function may have irregular memory access.\n";

            llvm::outs() << "\n";
        }
    }

private:
    void collectAccesses(const Stmt *S, const MatchFinder::MatchResult &Result,
                         std::set<std::string> &accesses, bool &hasIrregular) {
        if (!S) return;

        if (const ArraySubscriptExpr *ASE = dyn_cast<ArraySubscriptExpr>(S)) {
            const Expr *Idx = ASE->getIdx()->IgnoreParenCasts();
            const SourceManager *SM = Result.SourceManager;
            unsigned line = SM->getSpellingLineNumber(ASE->getExprLoc());

            std::string reason;
            std::string classification;

            if (const DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(Idx)) {
                std::string varName = DRE->getDecl()->getNameAsString();
                if (varName == "i") {
                    classification = "Sequential access";
                    reason = "Index is loop variable 'i'";
                } else {
                    classification = "Irregular access";
                    reason = "Index is variable '" + varName + "', not clearly a loop variable";
                    hasIrregular = true;
                }
            } else {
                classification = "Irregular access";
                reason = "Index is a complex expression";
                hasIrregular = true;
            }

            accesses.insert("- " + classification + " at line " + std::to_string(line) +
                            "\n  Reason: " + reason);
        }

        for (const Stmt *Child : S->children()) {
            if (Child)
                collectAccesses(Child, Result, accesses, hasIrregular);
        }
    }
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    if (!AnalyzeRegularMemoryAccess)
        return Tool.run(newFrontendActionFactory<SyntaxOnlyAction>().get());

    RegularMemoryAccessCallback Callback;
    MatchFinder Finder;

    // Only match user-defined functions (skip system headers)
    Finder.addMatcher(
        functionDecl(isDefinition(), unless(isExpansionInSystemHeader())).bind("funcDecl"),
        &Callback);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
