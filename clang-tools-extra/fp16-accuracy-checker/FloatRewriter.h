#ifndef FLOAT_REWRITER_H
#define FLOAT_REWRITER_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"

using namespace clang;

class FloatRewriter : public ASTConsumer, public RecursiveASTVisitor<FloatRewriter> {
public:
    FloatRewriter(Rewriter &R, std::string TargetType)
        : TheRewriter(R), TargetFloatType(std::move(TargetType)) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        this->Context = &Context;
        TraverseDecl(Context.getTranslationUnitDecl());
    }

    bool VisitTypeLoc(TypeLoc TL) {
        QualType QT = TL.getType();
        if (QT.getAsString() == "float") {
            SourceLocation Begin = TL.getBeginLoc();
            SourceLocation End = TL.getEndLoc();
            CharSourceRange CharRange = CharSourceRange::getTokenRange(Begin, End);

            StringRef OriginalText = Lexer::getSourceText(CharRange, TheRewriter.getSourceMgr(), Context->getLangOpts());
            if (OriginalText.trim() == "float") {
                TheRewriter.ReplaceText(CharRange, TargetFloatType);
            }
        }
        return true;
    }

    bool VisitBinaryOperator(BinaryOperator *BO) {
    BinaryOperatorKind OpCode = BO->getOpcode();
    if (OpCode != BO_Add && OpCode != BO_Sub && OpCode != BO_Mul && OpCode != BO_Div)
        return true;

    QualType QT = BO->getType();
    if (!QT->isFloatingType()) return true;

    std::string OpStr = BO->getOpcodeStr().str();
    std::string TypeLabel = TargetFloatType.empty() ? "float" : TargetFloatType;
    std::string PrintStmt = buildPrintStatement(BO, OpStr, TypeLabel);

    if (!Context) return true;

    const auto &Parents = Context->getParents(*BO);
    if (!Parents.empty()) {
        const DynTypedNode &ParentNode = Parents[0];

        // Case 1: Variable Declaration
        if (const VarDecl *VD = ParentNode.get<VarDecl>()) {
            const auto &GrandParents = Context->getParents(*VD);
            if (!GrandParents.empty()) {
                if (const DeclStmt *DS = GrandParents[0].get<DeclStmt>()) {
                    SourceLocation InsertLoc = DS->getEndLoc().getLocWithOffset(1);
                    TheRewriter.InsertTextAfterToken(InsertLoc, "\n" + PrintStmt);
                    return true;
                }
            }
        }

        // Case 2: Return statement
        if (const Stmt *S = ParentNode.get<Stmt>()) {
            if (isa<ReturnStmt>(S)) {
                SourceLocation InsertLoc = S->getBeginLoc();
                TheRewriter.InsertTextBefore(InsertLoc, PrintStmt + "\n");
                return true;
            }

            // Case 3: Any other statement 
            SourceLocation InsertLoc = S->getEndLoc().getLocWithOffset(1);
            TheRewriter.InsertTextAfterToken(InsertLoc, "\n" + PrintStmt);
            return true;
        }
    }

    // Fallback
    const Stmt *EnclosingStmt = BO;
    while (true) {
        auto Parents = Context->getParents(*EnclosingStmt);
        if (Parents.empty()) break;

        const Stmt *Next = Parents[0].get<Stmt>();
        if (!Next) break;

        EnclosingStmt = Next;
    }

    SourceLocation SafeInsertLoc = EnclosingStmt->getEndLoc().getLocWithOffset(1);
    TheRewriter.InsertTextAfterToken(SafeInsertLoc, "\n" + PrintStmt);
    return true;
}

bool VisitUnaryOperator(UnaryOperator *UO) {
    UnaryOperatorKind OpCode = UO->getOpcode();

    // track arithmetic related unary ops
    if (OpCode != UO_Minus && OpCode != UO_Plus &&
        OpCode != UO_PreInc && OpCode != UO_PostInc &&
        OpCode != UO_PreDec && OpCode != UO_PostDec)
        return true;

    QualType QT = UO->getType();
    if (!QT->isFloatingType()) return true;

    std::string OpStr = UnaryOperator::getOpcodeStr(OpCode).str();
    std::string TypeLabel = TargetFloatType.empty() ? "float" : TargetFloatType;
    std::string PrintStmt = buildUnaryPrintStatement(UO, OpStr, TypeLabel);

    if (!Context) return true;

    const auto &Parents = Context->getParents(*UO);
    if (!Parents.empty()) {
        const DynTypedNode &Parent = Parents[0];

        // Case 1: variable declaration 
        if (const VarDecl *VD = Parent.get<VarDecl>()) {
            const auto &GrandParents = Context->getParents(*VD);
            if (!GrandParents.empty()) {
                if (const DeclStmt *DS = GrandParents[0].get<DeclStmt>()) {     
                    SourceLocation SemiInsertLoc = DS->getEndLoc();
                    TheRewriter.InsertTextAfterToken(SemiInsertLoc, ";");

                    SourceLocation PrintInsertLoc = SemiInsertLoc.getLocWithOffset(1);
                    TheRewriter.InsertTextAfterToken(PrintInsertLoc, "\n" + PrintStmt);
                    return true;
                }
            }
        }

        // Case 2: Standalone expression like ++c;
        if (const Stmt *S = Parent.get<Stmt>()) {
            SourceLocation InsertLoc = S->getEndLoc().getLocWithOffset(1);
            TheRewriter.InsertTextAfterToken(InsertLoc, "\n" + PrintStmt);
            return true;
        }
    }

    // Absolute fallback
    SourceLocation FallbackLoc = UO->getEndLoc().getLocWithOffset(1);
    TheRewriter.InsertTextAfterToken(FallbackLoc, ";\n" + PrintStmt); 
    return true;
}


private:
    Rewriter &TheRewriter;
    ASTContext *Context;
    std::string TargetFloatType;

    std::string buildPrintStatement(BinaryOperator *BO, const std::string &op, const std::string &type) {
        std::string exprStr;
        llvm::raw_string_ostream ExprOut(exprStr);
        BO->printPretty(ExprOut, nullptr, PrintingPolicy(Context->getLangOpts()));

        std::string stmt = "\nprintf(\"[" + op + "] " + type + ": %f\\n\", (double)(" + ExprOut.str() + "));";
        return stmt;
    }
    
    std::string buildUnaryPrintStatement(UnaryOperator *UO, const std::string &op, const std::string &type) {
    std::string exprStr;
    llvm::raw_string_ostream ExprOut(exprStr);
    UO->printPretty(ExprOut, nullptr, PrintingPolicy(Context->getLangOpts()));

    std::string stmt = "\nprintf(\"[" + op + "] " + type + ": %f\\n\", (double)(" + ExprOut.str() + "));";
    return stmt;
}

};

#endif // FLOAT_REWRITER_H

