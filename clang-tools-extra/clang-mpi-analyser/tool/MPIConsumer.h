#pragma once
#include "MPIAnalyzer.h" 
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/raw_ostream.h" 

class MPIConsumer : public clang::ASTConsumer {
public:
    // Constructor now takes ASTContext and the new boolean option
    MPIConsumer(clang::ASTContext &Context, bool AnalyzeSG)
        : Callback(Context, AnalyzeSG), 
          ShouldAnalyzeScatterGather(AnalyzeSG) 
    {
        //This funtion matches the finder in the AST
        registerMPIMatchers(Finder, Callback);
        // llvm::outs() << "🧩 Matcher registered\n";
    }

    void HandleTranslationUnit(clang::ASTContext &Context) override {
        Finder.matchAST(Context);
    }

private:
    MPIAnalyzerCallback Callback;
    clang::ast_matchers::MatchFinder Finder;
    bool ShouldAnalyzeScatterGather; 
};