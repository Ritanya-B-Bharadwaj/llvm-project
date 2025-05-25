#ifndef FLOAT_REWRITER_H
#define FLOAT_REWRITER_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"  
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/CompilerInstance.h"

class FloatRewriter : public clang::ASTConsumer,
                      public clang::RecursiveASTVisitor<FloatRewriter> {
public:
    FloatRewriter(clang::Rewriter &R, const std::string &TargetFloatType);

    bool VisitVarDecl(clang::VarDecl *Declaration);
    bool VisitFunctionDecl(clang::FunctionDecl *Declaration);

    void HandleTranslationUnit(clang::ASTContext &Context) override;

private:
    clang::Rewriter &TheRewriter;
    std::string TargetType;
};

#endif

