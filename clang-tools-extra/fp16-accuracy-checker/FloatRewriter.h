#ifndef FLOAT_REWRITER_H
#define FLOAT_REWRITER_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;

class FloatRewriter : public ASTConsumer, public RecursiveASTVisitor<FloatRewriter> {
public:
    FloatRewriter(Rewriter &R, std::string TargetType)
        : TheRewriter(R), TargetFloatType(std::move(TargetType)) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        this->Context = &Context;
        TraverseDecl(Context.getTranslationUnitDecl());
    }

    bool VisitVarDecl(VarDecl *VD) {
        QualType QT = VD->getType();
        if (QT.getAsString() == "float") {
            SourceLocation SL = VD->getTypeSpecStartLoc();
            TheRewriter.ReplaceText(SL, 5, TargetFloatType);
        }
        return true;
    }

    bool VisitFieldDecl(FieldDecl *FD) {
        QualType QT = FD->getType();
        if (QT.getAsString() == "float") {
            SourceLocation SL = FD->getTypeSpecStartLoc();
            TheRewriter.ReplaceText(SL, 5, TargetFloatType);
        }
        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl *PVD) {
        QualType QT = PVD->getType();
        if (QT.getAsString() == "float") {
            SourceLocation SL = PVD->getTypeSpecStartLoc();
            TheRewriter.ReplaceText(SL, 5, TargetFloatType);
        }
        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *FD) {
        QualType RT = FD->getReturnType();
        if (RT.getAsString() == "float") {
            SourceLocation SL = FD->getReturnTypeSourceRange().getBegin();
            TheRewriter.ReplaceText(SL, 5, TargetFloatType);
        }
        return true;
    }

private:
    Rewriter &TheRewriter;
    ASTContext *Context;
    std::string TargetFloatType;
};

#endif // FLOAT_REWRITER_H

