#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h" 
#include "MPIAnalysisHelperFuncs.h" 

// Forward declarations of Clang types used in the class definition.
namespace clang {
    class FunctionDecl;
    class CallExpr;
    class Stmt;
    class ASTContext;
    class ParmVarDecl;
}

class MPIAnalyzerCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit MPIAnalyzerCallback(clang::ASTContext &Context, bool AnalyzeSG = false);

    // This method is invoked every time an AST node matches a registered pattern.
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;

private:
    clang::ASTContext &Context; 
    bool ShouldAnalyzeScatterGather; 
};

// Function to register AST matchers with the MatchFinder.
void registerMPIMatchers(clang::ast_matchers::MatchFinder &Finder, MPIAnalyzerCallback &Callback);