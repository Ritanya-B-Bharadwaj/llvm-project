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

/**
 * @class ConstructorCallback
 * @brief A callback class for Clang's AST MatchFinder to detect and report constructor invocations.
 *
 * This class inherits from MatchFinder::MatchCallback and overrides the run() method.
 * It is designed to be used with Clang's AST matchers to find variable declarations
 * that are initialized using constructors (default, copy, or parameterized).
 *
 * When a match is found, it retrieves the variable declaration and the constructor
 * expression, determines the type of constructor used, and prints a diagnostic
 * message to llvm::errs() with the source location and constructor details.
 */
class ConstructorCallback : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("varDecl");
        const CXXConstructExpr *CtorExpr = Result.Nodes.getNodeAs<CXXConstructExpr>("ctorExpr");
        if (!VD || !CtorExpr)
            return;

        const CXXConstructorDecl *Ctor = CtorExpr->getConstructor();
        if (!Ctor)
            return;

        const SourceManager &SM = *Result.SourceManager;
        SourceLocation Loc = VD->getLocation();

        llvm::errs() << SM.getFilename(Loc) << ":" << SM.getSpellingLineNumber(Loc)
                     << ":" << SM.getSpellingColumnNumber(Loc) << ": note: '"
                     << VD->getNameAsString() << "' initialized using an implicit invocation of ";

        if (Ctor->isDefaultConstructor()) {
            llvm::errs() << "Default constructor ";
        } else if (Ctor->isCopyConstructor()) {
            llvm::errs() << "Copy constructor ";
        } else if (Ctor->getNumParams() > 0) {
            llvm::errs() << "Parameterized constructor ";
        }

        llvm::errs() << "'" << Ctor->getQualifiedNameAsString() << "'\n";
    }
};

int main(int argc, const char **argv) {
    llvm::cl::OptionCategory ToolCategory("ctor-detector");

    // Use the correct number of arguments for create()
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory, llvm::cl::NumOccurrencesFlag::Optional);
    if (!ExpectedParser) {
        llvm::errs() << "Error: Failed to create CommonOptionsParser\n";
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    ConstructorCallback Callback;
    MatchFinder Finder;

Finder.addMatcher(
    varDecl(hasInitializer(cxxConstructExpr().bind("ctorExpr"))).bind("varDecl"),
    &Callback
);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}