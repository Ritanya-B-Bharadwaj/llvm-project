// FloatRewriter.cpp
#include "FloatRewriter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/ASTConsumer.h"


using namespace clang;

FloatRewriter::FloatRewriter(Rewriter &R, const std::string &TargetFloatType) : TheRewriter(R), TargetType(TargetFloatType) {}

void FloatRewriter::HandleTranslationUnit(ASTContext &Context) {
    TraverseDecl(Context.getTranslationUnitDecl());
}

bool FloatRewriter::VisitVarDecl(VarDecl *VD) {
    if (VD->getType().getAsString() == "float") {
        SourceLocation StartLoc = VD->getTypeSpecStartLoc();
        TheRewriter.ReplaceText(StartLoc, 5, TargetType); // replace 'float' (5 chars)
    }
    return true;
}

bool FloatRewriter::VisitFunctionDecl(FunctionDecl *FD) {
    if (FD->getReturnType().getAsString() == "float") {
        SourceLocation StartLoc = FD->getReturnTypeSourceRange().getBegin();
        TheRewriter.ReplaceText(StartLoc, 5, TargetType);
    }
    for (ParmVarDecl *Param : FD->parameters()) {
        if (Param->getType().getAsString() == "float") {
            SourceLocation ParamLoc = Param->getTypeSpecStartLoc();
            TheRewriter.ReplaceText(ParamLoc, 5, TargetType);
        }
    }
    return true;
}
