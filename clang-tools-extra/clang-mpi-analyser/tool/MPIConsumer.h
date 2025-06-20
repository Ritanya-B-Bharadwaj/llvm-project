#pragma once
#include "MPIAnalyzer.h" // Your MatchCallback
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/raw_ostream.h" // For llvm::outs()

class MPIConsumer : public clang::ASTConsumer {
public:
    // Constructor now takes ASTContext and the new boolean option
    MPIConsumer(clang::ASTContext &Context, bool AnalyzeSG)
        : Callback(Context, AnalyzeSG), // Initialize Callback with Context and AnalyzeSG
          ShouldAnalyzeScatterGather(AnalyzeSG) // Store the option here if needed in MPIConsumer directly
    {
        // The registerMPIMatchers function (which needs to be defined somewhere,
        // typically in MPIAnalyzer.h or a helper file)
        // should likely also take the Context and potentially the AnalyzeSG flag
        // if the registration of matchers depends on it.
        // For now, assuming registerMPIMatchers only needs the finder and callback.
        registerMPIMatchers(Finder, Callback);
        // llvm::outs() << "🧩 Matcher registered\n";
    }

    void HandleTranslationUnit(clang::ASTContext &Context) override {
        // Trigger the matching process
        Finder.matchAST(Context);
    }

private:
    MPIAnalyzerCallback Callback;
    clang::ast_matchers::MatchFinder Finder;
    bool ShouldAnalyzeScatterGather; // Storing the option within MPIConsumer (optional, but good for clarity)
};