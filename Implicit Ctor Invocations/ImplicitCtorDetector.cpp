#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class ImplicitCtorCallback : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("varDecl");

        if (!VD || !VD->hasInit())
            return;

        const Expr *InitExpr = VD->getInit();
        const auto *CtorExpr = dyn_cast<CXXConstructExpr>(InitExpr);

        // Removed: if (!CtorExpr || !CtorExpr->isImplicit())  ← doesn't exist in LLVM 14
        if (!CtorExpr)
            return;

        const CXXConstructorDecl *CtorDecl = CtorExpr->getConstructor();
        if (!CtorDecl)
            return;

        const SourceManager &SM = *Result.SourceManager;
        SourceLocation Loc = VD->getLocation();

        llvm::errs() << SM.getFilename(Loc) << ":" << SM.getSpellingLineNumber(Loc)
                     << ":" << SM.getSpellingColumnNumber(Loc) << ": note: '"
                     << VD->getNameAsString() << "' initialized using an implicit invocation of ";

        if (CtorDecl->isCopyConstructor())
            llvm::errs() << "copy ctor ";
        else if (CtorDecl->getNumParams() == 0)
            llvm::errs() << "default ctor ";
        else
            llvm::errs() << "ctor ";

        llvm::errs() << "'" << CtorDecl->getQualifiedNameAsString() << "'\n";
    }
};

int main(int argc, const char **argv) {
    llvm::cl::OptionCategory ToolCategory("implicit-ctor-detector");

    // Fixed: Use create() instead of direct constructor
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << "Error: Failed to create CommonOptionsParser\n";
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    ImplicitCtorCallback Callback;
    MatchFinder Finder;

    Finder.addMatcher(
        varDecl(hasInitializer(cxxConstructExpr().bind("ctorExpr"))).bind("varDecl"),
        &Callback
    );

    return Tool.run(newFrontendActionFactory(&Finder).get());
}

