#pragma once                                                       
#include "clang/Frontend/FrontendAction.h"                         
#include "clang/ASTMatchers/ASTMatchFinder.h"  
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"                   

class MPIAction : public clang::ASTFrontendAction {
    private:
    bool AnalyzeScatterGather;
public:
    MPIAction(bool AnalyzeSG = false) : AnalyzeScatterGather(AnalyzeSG) {}
    
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &CI, llvm::StringRef file) override;
};

class MPIActionFactory : public clang::tooling::FrontendActionFactory {
    bool AnalyzeSG;

public:
    explicit MPIActionFactory(bool AnalyzeSG) : AnalyzeSG(AnalyzeSG) {}

    std::unique_ptr<clang::FrontendAction> create() override {
        return std::make_unique<MPIAction>(AnalyzeSG);
    }
};
