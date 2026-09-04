// MPIAnalysisHelperFuncs.h

#ifndef CLANG_TUTORIAL_MPIANALYSISHELPERFUNCS_H
#define CLANG_TUTORIAL_MPIANALYSISHELPERFUNCS_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ExprCXX.h" // Ensures ExprStmt is declared

namespace clang {
namespace ast_matchers {

// Forward declarations
struct LoopInfo;
struct MPICallInfo;

// NEW: Forward declaration for isExpressionIndexedByVar
bool isExpressionIndexedByVar(const Expr *E, const VarDecl *TargetVar, const std::string& RankVarName);

// Function to analyze a block (Stmt) for MPI point-to-point calls.
bool analyzeBlockForMPICalls(const Stmt *S, clang::ASTContext &Context,
                             const std::string &RankVarName,
                             const clang::ParmVarDecl *RootParameterDecl,
                             int ExpectedRootLiteral,
                             const clang::VarDecl *CurrentLoopVarDecl,
                             llvm::SmallVectorImpl<MPICallInfo> &Sends,
                             llvm::SmallVectorImpl<MPICallInfo> &Recvs,
                             llvm::SmallVectorImpl<LoopInfo> &LoopStack);

// MPICallInfo struct to store information about an MPI point-to-point call.
struct MPICallInfo {
    const CallExpr *Call;
    std::string FunctionName;
    bool IsRankArgCurrentRank;      // Is the rank argument 'rank' variable
    bool IsRankArgRoot;             // Is the rank argument the 'root' parameter
    int RankArgLiteralValue;        // If rank argument is a literal (e.g., 0)
    bool IsRankArgLoopVar;          // Is the rank argument the *current loop variable*
     const clang::Expr *RankArgExpr = nullptr;
    const Expr *BufferExpr;         // The expression for sendbuf/recvbuf argument
    bool IsBufferIndexedByLoopVar;  // Is the buffer expression indexed by the current loop variable?
    bool IsBufferIndexedByRankVar;  // Is the buffer expression indexed by the global rank variable?

    const LoopInfo *ContainingLoop; // Pointer to the innermost loop this call is in

    bool IsSend = false;               // True if this represents a send operation (MPI_Send or send part of MPI_Sendrecv)
    bool IsRecv = false;               // True if this represents a receive operation (MPI_Recv or recv part of MPI_Sendrecv)
    bool IsRankArgAnySource = false;   

    MPICallInfo(const CallExpr *C = nullptr) : Call(C), FunctionName(""),
        IsRankArgCurrentRank(false), IsRankArgRoot(false), RankArgLiteralValue(-1),
        IsRankArgLoopVar(false),
        BufferExpr(nullptr), IsBufferIndexedByLoopVar(false), IsBufferIndexedByRankVar(false),
        ContainingLoop(nullptr) {}
};

// LoopInfo struct to track information about nested loops.
struct LoopInfo {
    const Stmt *LoopStmt;        // The ForStmt, WhileStmt, or DoStmt itself
    const VarDecl *LoopVarDecl;  // The variable declared/controlled by this loop (if any)
    bool IsLoopOverRanks;        // True if the loop iterates from 0 to num_procs-1

    LoopInfo(const Stmt *S = nullptr, const VarDecl *VD = nullptr)
        : LoopStmt(S), LoopVarDecl(VD), IsLoopOverRanks(false) {}
};


} // namespace ast_matchers
} // namespace clang

#endif // CLANG_TUTORIAL_MPIANALYSISHELPERFUNCS_H   