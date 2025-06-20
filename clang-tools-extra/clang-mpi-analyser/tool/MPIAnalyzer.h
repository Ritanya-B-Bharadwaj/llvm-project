// File: MPIAnalyzer.h
#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h" // For clang::ast_matchers::MatchFinder
#include "MPIAnalysisHelperFuncs.h" // Includes MPICallInfo, LoopInfo, and analyzeBlockForMPICalls declaration

// Forward declarations of Clang types used in the class definition.
namespace clang {
    class FunctionDecl;
    class CallExpr;
    class Stmt;
    class ASTContext;
    class ParmVarDecl;
}

// MPIAnalyzerCallback inherits from MatchFinder::MatchCallback,
// making it the recipient of matched AST nodes.
class MPIAnalyzerCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    // Constructor that accepts ASTContext and the boolean flag
    explicit MPIAnalyzerCallback(clang::ASTContext &Context, bool AnalyzeSG = false);

    // This method is invoked every time an AST node matches a registered pattern.
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;

private:
    clang::ASTContext &Context; // Reference to the ASTContext
    bool ShouldAnalyzeScatterGather; // Member to store the option state
};

// Function to register AST matchers with the MatchFinder.
void registerMPIMatchers(clang::ast_matchers::MatchFinder &Finder, MPIAnalyzerCallback &Callback);