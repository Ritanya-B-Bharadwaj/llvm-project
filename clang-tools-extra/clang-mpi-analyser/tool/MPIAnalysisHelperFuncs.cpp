#include "MPIAnalysisHelperFuncs.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/StmtCXX.h" 
#include "clang/AST/ExprCXX.h" 

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

using namespace clang;
using llvm::dyn_cast;


bool ast_matchers::isExpressionIndexedByVar(const Expr *E, const VarDecl *TargetVar, const std::string& RankVarName) {
    if (!E || !TargetVar) return false;

    E = E->IgnoreImpCasts(); 

    // Case 1: Direct reference to the target variable
    if (const auto *DRE = dyn_cast<DeclRefExpr>(E)) {
        return DRE->getDecl() == TargetVar;
    }

    // Case 2: Array subscript expression (e.g., array[index])
    // The target variable could be in the index part.
    if (const auto *ASE = dyn_cast<ArraySubscriptExpr>(E)) {
        // Recursively check if the index expression uses the target variable
        return isExpressionIndexedByVar(ASE->getIdx(), TargetVar, RankVarName);
    }

    // Case 3: Binary operator (e.g., +, -, *, /, %, <<, >>, etc.)
    // The target variable could be on either side of the operator, or within a sub-expression.
    // This is crucial for pointer arithmetic (e.g., base + offset) and calculating offsets (e.g., i * count).
    if (const auto *BO = dyn_cast<BinaryOperator>(E)) {
        // Recursively check both LHS and RHS
        if (isExpressionIndexedByVar(BO->getLHS(), TargetVar, RankVarName)) return true;
        if (isExpressionIndexedByVar(BO->getRHS(), TargetVar, RankVarName)) return true;
    }

    // Case 4: Unary operator (e.g., &var, *ptr, ++var, --var, !cond)
    // Check the sub-expression of the unary operator.
    if (const auto *UO = dyn_cast<UnaryOperator>(E)) {
        return isExpressionIndexedByVar(UO->getSubExpr(), TargetVar, RankVarName);
    }

    // Case 5: Member expressions (e.g., struct_var.member, ptr->member)
    // The base expression (struct_var or ptr) could be indexed.
    if (const auto *ME = dyn_cast<MemberExpr>(E)) {
        return isExpressionIndexedByVar(ME->getBase(), TargetVar, RankVarName);
    }

    // Case 6: Call expressions (if an argument is indexed, or return value is part of index calculation)
    // This is more complex and might not be strictly necessary for basic patterns,
    // but can be added for robustness if needed.
    if (const auto *CE = dyn_cast<CallExpr>(E)) {
        for (const auto* arg : CE->arguments()) {
            if (isExpressionIndexedByVar(arg, TargetVar, RankVarName)) return true;
        }
    }

    // Case 7: ParenExpr (parenthesized expressions)
    // Simply check the inner expression.
    if (const auto *PE = dyn_cast<ParenExpr>(E)) {
        return isExpressionIndexedByVar(PE->getSubExpr(), TargetVar, RankVarName);
    }

    // Default: If none of the above matches or recursive calls return true
    return false;
}


// Definition of analyzeBlockForMPICalls
bool ast_matchers::analyzeBlockForMPICalls(const Stmt *S, ASTContext &Context,
                                           const std::string &RankVarName,
                                           const ParmVarDecl *RootParameterDecl,
                                           int ExpectedRootLiteral,
                                           const VarDecl *CurrentLoopVarDecl,
                                           llvm::SmallVectorImpl<ast_matchers::MPICallInfo> &Sends,
                                           llvm::SmallVectorImpl<ast_matchers::MPICallInfo> &Recvs,
                                           llvm::SmallVectorImpl<ast_matchers::LoopInfo> &LoopStack) {
    if (!S) return false;

    bool foundMPICall = false;

    for (const Stmt *Child : S->children()) {
        if (!Child) continue;

        if (const auto *Call = dyn_cast<CallExpr>(Child)) {
            const FunctionDecl *Callee = Call->getDirectCallee();
            if (!Callee) continue;

            std::string funcName = Callee->getNameAsString();
            MPICallInfo callInfo(Call); 
            callInfo.FunctionName = funcName;

            bool isMPICall = false; 

            if (funcName.find("MPI_Sendrecv") != std::string::npos) {
                isMPICall = true;
                // llvm::outs() << "Hey from sendrecv (corrected)\n"; // Add newline for clarity

                if (Call->getNumArgs() >= 9) {
                    const Expr *SendBuf = Call->getArg(0)->IgnoreImpCasts();
                    const Expr *DestRankExpr = Call->getArg(3)->IgnoreImpCasts();
                    const Expr *RecvBuf = Call->getArg(5)->IgnoreImpCasts();
                    const Expr *SourceRankExpr = Call->getArg(8)->IgnoreImpCasts();

                    //  Analyze the SEND part of Sendrecv 
                    MPICallInfo sendPartInfo = callInfo;
                    sendPartInfo.IsSend = true;
                    sendPartInfo.BufferExpr = SendBuf;
                    sendPartInfo.RankArgExpr = DestRankExpr;

                    if (CurrentLoopVarDecl && ast_matchers::isExpressionIndexedByVar(sendPartInfo.BufferExpr, CurrentLoopVarDecl, RankVarName)) {
                        sendPartInfo.IsBufferIndexedByLoopVar = true;
                    }
                    if (!LoopStack.empty()) {
                        sendPartInfo.ContainingLoop = &LoopStack.back();
                    }
                    if (const auto *DRE = dyn_cast<DeclRefExpr>(DestRankExpr)) {
                        const Decl *TargetDecl = DRE->getDecl();
                        if (const auto *V = dyn_cast<VarDecl>(TargetDecl)) {
                            if (V->getNameAsString() == RankVarName) sendPartInfo.IsRankArgCurrentRank = true;
                        }
                        if (RootParameterDecl && TargetDecl == RootParameterDecl) sendPartInfo.IsRankArgRoot = true;
                        if (CurrentLoopVarDecl && TargetDecl == CurrentLoopVarDecl) sendPartInfo.IsRankArgLoopVar = true;
                    } else if (const auto *IL = dyn_cast<IntegerLiteral>(DestRankExpr)) {
                        sendPartInfo.RankArgLiteralValue = static_cast<int>(IL->getValue().getZExtValue());
                        if (ExpectedRootLiteral != -1 && sendPartInfo.RankArgLiteralValue == ExpectedRootLiteral) sendPartInfo.IsRankArgRoot = true;
                    }
                    Sends.push_back(sendPartInfo);

                    // Analyze the RECV part of Sendrecv
                    MPICallInfo recvPartInfo = callInfo;
                    recvPartInfo.IsRecv = true;
                    recvPartInfo.BufferExpr = RecvBuf;
                    recvPartInfo.RankArgExpr = SourceRankExpr;

                    if (CurrentLoopVarDecl && ast_matchers::isExpressionIndexedByVar(recvPartInfo.BufferExpr, CurrentLoopVarDecl, RankVarName)) {
                        recvPartInfo.IsBufferIndexedByLoopVar = true;
                    }
                    if (!LoopStack.empty()) {
                        recvPartInfo.ContainingLoop = &LoopStack.back();
                    }
                    if (const auto *DRE = dyn_cast<DeclRefExpr>(SourceRankExpr)) {
                        const Decl *TargetDecl = DRE->getDecl();
                        if (const auto *V = dyn_cast<VarDecl>(TargetDecl)) {
                            if (V->getNameAsString() == RankVarName) recvPartInfo.IsRankArgCurrentRank = true;
                        }
                        if (CurrentLoopVarDecl && TargetDecl == CurrentLoopVarDecl) recvPartInfo.IsRankArgLoopVar = true;
                    } else if (const auto *IL = dyn_cast<IntegerLiteral>(SourceRankExpr)) {
                        recvPartInfo.RankArgLiteralValue = static_cast<int>(IL->getValue().getZExtValue());
                        if (ExpectedRootLiteral != -1 && recvPartInfo.RankArgLiteralValue == ExpectedRootLiteral) recvPartInfo.IsRankArgRoot = true;
                    } else if (const auto *DRE = dyn_cast<DeclRefExpr>(SourceRankExpr)) {
                        if (const EnumConstantDecl *ECD = dyn_cast<EnumConstantDecl>(DRE->getDecl())) {
                            if (ECD->getNameAsString() == "MPI_ANY_SOURCE") recvPartInfo.IsRankArgAnySource = true;
                        }
                    }
                    Recvs.push_back(recvPartInfo);
                }
            } else if (funcName.find("MPI_Send") != std::string::npos) { 
                isMPICall = true;
                callInfo.IsSend = true;
                if (Call->getNumArgs() > 3) {
                    callInfo.RankArgExpr = Call->getArg(3)->IgnoreImpCasts();
                    if (const auto *DRE = dyn_cast<DeclRefExpr>(callInfo.RankArgExpr)) {
                        const Decl *TargetDecl = DRE->getDecl();
                        if (const auto *V = dyn_cast<VarDecl>(TargetDecl)) {
                            if (V->getNameAsString() == RankVarName) callInfo.IsRankArgCurrentRank = true;
                        }
                        if (RootParameterDecl && TargetDecl == RootParameterDecl) callInfo.IsRankArgRoot = true;
                        if (CurrentLoopVarDecl && TargetDecl == CurrentLoopVarDecl) callInfo.IsRankArgLoopVar = true;
                    } else if (const auto *IL = dyn_cast<IntegerLiteral>(callInfo.RankArgExpr)) {
                        callInfo.RankArgLiteralValue = static_cast<int>(IL->getValue().getZExtValue());
                        if (ExpectedRootLiteral != -1 && callInfo.RankArgLiteralValue == ExpectedRootLiteral) callInfo.IsRankArgRoot = true;
                    }
                }
                if (Call->getNumArgs() > 0) {
                    callInfo.BufferExpr = Call->getArg(0)->IgnoreImpCasts();
                    if (CurrentLoopVarDecl && ast_matchers::isExpressionIndexedByVar(callInfo.BufferExpr, CurrentLoopVarDecl, RankVarName)) {
                        callInfo.IsBufferIndexedByLoopVar = true;
                    }
                }
                if (!LoopStack.empty()) {
                    callInfo.ContainingLoop = &LoopStack.back();
                }
                Sends.push_back(callInfo);

            } else if (funcName.find("MPI_Recv") != std::string::npos) { 
                isMPICall = true;
                callInfo.IsRecv = true;
                if (Call->getNumArgs() > 3) { 
                    callInfo.RankArgExpr = Call->getArg(3)->IgnoreImpCasts();
                    if (const auto *DRE = dyn_cast<DeclRefExpr>(callInfo.RankArgExpr)) {
                        const Decl *TargetDecl = DRE->getDecl();
                        if (const auto *V = dyn_cast<VarDecl>(TargetDecl)) {
                            if (V->getNameAsString() == RankVarName) callInfo.IsRankArgCurrentRank = true;
                        }
                        if (RootParameterDecl && TargetDecl == RootParameterDecl) callInfo.IsRankArgRoot = true;
                        if (CurrentLoopVarDecl && TargetDecl == CurrentLoopVarDecl) callInfo.IsRankArgLoopVar = true;
                    } else if (const auto *IL = dyn_cast<IntegerLiteral>(callInfo.RankArgExpr)) {
                        callInfo.RankArgLiteralValue = static_cast<int>(IL->getValue().getZExtValue());
                        if (ExpectedRootLiteral != -1 && callInfo.RankArgLiteralValue == ExpectedRootLiteral) callInfo.IsRankArgRoot = true;
                    } else if (const auto *DRE = dyn_cast<DeclRefExpr>(callInfo.RankArgExpr)) {
                        if (const EnumConstantDecl *ECD = dyn_cast<EnumConstantDecl>(DRE->getDecl())) {
                            if (ECD->getNameAsString() == "MPI_ANY_SOURCE") callInfo.IsRankArgAnySource = true;
                        }
                    }
                }
                if (Call->getNumArgs() > 0) {
                    callInfo.BufferExpr = Call->getArg(0)->IgnoreImpCasts();
                    if (CurrentLoopVarDecl && ast_matchers::isExpressionIndexedByVar(callInfo.BufferExpr, CurrentLoopVarDecl, RankVarName)) {
                        callInfo.IsBufferIndexedByLoopVar = true;
                    }
                }
                if (!LoopStack.empty()) {
                    callInfo.ContainingLoop = &LoopStack.back();
                }
                Recvs.push_back(callInfo);
            }

            if (isMPICall) {
                foundMPICall = true;
            }
        }
        else if (const auto *For = dyn_cast<ForStmt>(Child)) {
            const VarDecl *LoopVar = nullptr;
            const Stmt *InitStmt = For->getInit();

            if (const auto *DS = dyn_cast<DeclStmt>(InitStmt)) {
                if (DS->isSingleDecl()) {
                    LoopVar = dyn_cast<VarDecl>(DS->getSingleDecl());
                }
            }
            else if (InitStmt) {
                const Expr *InitExpr = nullptr;
                // Try this again: It MUST be `ExprStmt` as `clang::ExprStmt` is wrong with `using namespace clang;`
                InitExpr = dyn_cast<Expr>(InitStmt);


                if (InitExpr) {
                    if (const auto *BO = dyn_cast<BinaryOperator>(InitExpr->IgnoreImpCasts())) {
                        if (BO->isAssignmentOp()) {
                            if (const auto* DRE = dyn_cast<DeclRefExpr>(BO->getLHS()->IgnoreImpCasts())) {
                                LoopVar = dyn_cast<VarDecl>(DRE->getDecl());
                            }
                        }
                    }
                }
            }

            ast_matchers::LoopInfo currentLoop(For, LoopVar);
            LoopStack.push_back(currentLoop);

            bool innerFound = ast_matchers::analyzeBlockForMPICalls(For->getBody(), Context,
                                                      RankVarName,  RootParameterDecl,ExpectedRootLiteral,
                                                      LoopVar,
                                                      Sends, Recvs, LoopStack);
            if (innerFound) foundMPICall = true;

            LoopStack.pop_back();
        }
        else {
            bool innerFound = ast_matchers::analyzeBlockForMPICalls(Child, Context,
                                                      RankVarName, RootParameterDecl,  ExpectedRootLiteral,
                                                      CurrentLoopVarDecl,
                                                      Sends, Recvs, LoopStack);
            if (innerFound) foundMPICall = true;
        }
    }
    return foundMPICall;
}