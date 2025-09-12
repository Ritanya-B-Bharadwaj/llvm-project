#include "MPIAnalyzer.h"
#include "MPIAnalysisHelperFuncs.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"

using namespace clang;
using namespace clang::ast_matchers;

// This has the value of the bool which determines whether to run the the analyser or not
MPIAnalyzerCallback::MPIAnalyzerCallback(ASTContext &Context, bool AnalyzeSG)
    : Context(Context), 
      ShouldAnalyzeScatterGather(AnalyzeSG)
{
    // You can add any initialization logic here if needed
    // llvm::outs() << "MPIAnalyzerCallback initialized. Scatter/Gather analysis "
                //  << (ShouldAnalyzeScatterGather ? "ENABLED" : "DISABLED") << ".\n";
}

void MPIAnalyzerCallback::run(const MatchFinder::MatchResult &Result) {
    if (!ShouldAnalyzeScatterGather) {
        return;
    }

    const auto *Call = Result.Nodes.getNodeAs<CallExpr>("mpiCall");
    if (!Call || !Result.Context) return;

    // llvm::outs() << "📌 Matcher callback triggered\n";
    // llvm::outs() << "😈CheckPoint 1\n";

    ASTContext &Context = *Result.Context;
    SourceManager &SM = Context.getSourceManager();

    const FunctionDecl *FD = nullptr;
    DynTypedNode Node = DynTypedNode::create(*Call);
    while (true) {
        auto Parents = Context.getParents(Node);
        if (Parents.empty()) break;
        Node = Parents[0];
        if (const auto *CandidateFD = Node.get<FunctionDecl>()) {
            FD = CandidateFD;
            break;
        }
    }

    if (!FD || !FD->hasBody()) {
        // llvm::outs() << "  Function Decl not found or has no body. Skipping.\n";
        return;
    }
    // llvm::outs() << "😈CheckPoint 2\n";
    // llvm::outs() << "  Analyzing function: " << FD->getNameAsString() << "\n";
    //Not really necessary added this in earlier stages
    const Stmt *Body = FD->getBody();
    std::string RankVarName;
    std::string NumProcsVarName;

    const ParmVarDecl *ParamRecvBuf = nullptr;
    const ParmVarDecl *ParamSendBuf = nullptr;
    for (const ParmVarDecl *PVD : FD->parameters()) {
        if (PVD->getNameAsString() == "recvbuf") {
            ParamRecvBuf = PVD;
        } else if (PVD->getNameAsString() == "sendbuf") {
            ParamSendBuf = PVD;
        }
    }
    // if (ParamRecvBuf) llvm::outs() << "  Function has 'recvbuf' parameter.\n";
    // if (ParamSendBuf) llvm::outs() << "  Function has 'sendbuf' parameter.\n";


    // Find the variable used to store the rank and num_procs
    // by looking for MPI_Comm_rank and MPI_Comm_size calls within the function body.
    for (const Stmt *S : Body->children()) {
        if (const auto *CurrentCall = dyn_cast<CallExpr>(S)) {
            const FunctionDecl *Callee = CurrentCall->getDirectCallee();
            if (!Callee) continue;

            if (Callee->getNameAsString() == "MPI_Comm_rank" && CurrentCall->getNumArgs() >= 2) {
                const Expr *Arg = CurrentCall->getArg(1)->IgnoreImpCasts();
                if (const auto *Unary = dyn_cast<UnaryOperator>(Arg)) {
                    if (Unary->getOpcode() == UO_AddrOf) {
                        if (const auto *DRE = dyn_cast<DeclRefExpr>(Unary->getSubExpr())) {
                            RankVarName = DRE->getDecl()->getNameAsString();
                            // llvm::outs() << "  Found MPI_Comm_rank. Rank variable: " << RankVarName << "\n";
                        }
                    }
                }
            } else if (Callee->getNameAsString() == "MPI_Comm_size" && CurrentCall->getNumArgs() >= 2) {
                const Expr *Arg = CurrentCall->getArg(1)->IgnoreImpCasts();
                if (const auto *Unary = dyn_cast<UnaryOperator>(Arg)) {
                    if (Unary->getOpcode() == UO_AddrOf) {
                        if (const auto *DRE = dyn_cast<DeclRefExpr>(Unary->getSubExpr())) {
                            NumProcsVarName = DRE->getDecl()->getNameAsString();
                            // llvm::outs() << "  Found MPI_Comm_size. NumProcs variable: " << NumProcsVarName << "\n";
                        }
                    }
                }
            }
        }
    }

    const CompoundStmt *Comp = dyn_cast<CompoundStmt>(Body);
    if (!Comp || RankVarName.empty()) {
        // llvm::outs() << "  Function body not a CompoundStmt or RankVar not found. Skipping.\n";
        return;
    }
    // llvm::outs() << "😈CheckPoint 3\n";

    SourceLocation ReportLoc = FD->getBeginLoc(); 

    //  Part 1: Existing Gather/Scatter detection 
    //Basically itrate trhough the AST to find the if statement
    for (const Stmt *S : Comp->body()) {
        const IfStmt *If = dyn_cast<IfStmt>(S);
        if (!If) continue;
        // If->getCond()->dump();
        const BinaryOperator *Cond = dyn_cast<BinaryOperator>(If->getCond());
        if (!Cond || !Cond->isEqualityOp()) continue;

        const Expr *LHS = Cond->getLHS()->IgnoreImpCasts();
        const Expr *RHS = Cond->getRHS()->IgnoreImpCasts();

        const ParmVarDecl *CurrentIfRootParameterDecl = nullptr;
        int CurrentIfRootLiteral = -1;

        bool IsRankVsRootCond = false;
        const Decl* LHS_Decl = nullptr; int LHS_Literal = -1;
        const Decl* RHS_Decl = nullptr; int RHS_Literal = -1;

        auto getDeclOrLiteral = [&](const Expr* E_clean, const Decl** OutDecl, int* OutLiteral) {
            *OutDecl = nullptr;
            *OutLiteral = -1;
            if (!E_clean) return;

            if (const auto *DRE = dyn_cast<DeclRefExpr>(E_clean)) {
                *OutDecl = DRE->getDecl();
            } else if (const auto *IL = dyn_cast<IntegerLiteral>(E_clean)) {
                *OutLiteral = static_cast<int>(IL->getValue().getZExtValue());
            }
        };
        getDeclOrLiteral(LHS, &LHS_Decl, &LHS_Literal);
        getDeclOrLiteral(RHS, &RHS_Decl, &RHS_Literal);

        if (LHS_Decl && cast<NamedDecl>(LHS_Decl)->getNameAsString() == RankVarName) {
            if (RHS_Decl) {
                if (RHS_Decl->getKind() == Decl::ParmVar) {
                    const auto* PVD = dyn_cast<ParmVarDecl>(RHS_Decl);
                    if (PVD) {
                        IsRankVsRootCond = true;
                        CurrentIfRootParameterDecl = PVD;
                    }
                } else if (RHS_Decl->getKind() == Decl::Var) {
                    llvm::outs() << "    RHS_Decl is a local variable (VarDecl): " << cast<NamedDecl>(RHS_Decl)->getNameAsString() << ". Not treated as root parameter for this analysis.\n";
                } else {
                    llvm::outs() << "    RHS_Decl is a Decl* of unexpected kind: " << RHS_Decl->getDeclKindName() << "\n";
                }
            } else if (RHS_Literal != -1) {
                IsRankVsRootCond = true;
                CurrentIfRootLiteral = RHS_Literal;
            } else {
                llvm::outs() << "    RHS is neither a valid DeclRefExpr for a ParmVarDecl/VarDecl nor an IntegerLiteral, or RHS_Decl was null.\n";
            }
        } else if (RHS_Decl && cast<NamedDecl>(RHS_Decl)->getNameAsString() == RankVarName) {
            if (LHS_Decl) {
                if (LHS_Decl->getKind() == Decl::ParmVar) {
                    const auto* PVD = dyn_cast<ParmVarDecl>(LHS_Decl);
                    if (PVD) {
                        IsRankVsRootCond = true;
                        CurrentIfRootParameterDecl = PVD;
                    }
                } else if (LHS_Decl->getKind() == Decl::Var) {
                    llvm::outs() << "    LHS_Decl is a local variable (VarDecl): " << cast<NamedDecl>(LHS_Decl)->getNameAsString() << ". Not treated as root parameter for this analysis.\n";
                } else {
                    llvm::outs() << "    LHS_Decl is a Decl* of unexpected kind: " << LHS_Decl->getDeclKindName() << "\n";
                }
            } else if (LHS_Literal != -1) {
                IsRankVsRootCond = true;
                CurrentIfRootLiteral = LHS_Literal;
            } else {
                llvm::outs() << "    LHS is neither a valid DeclRefExpr for a ParmVarDecl/VarDecl nor an IntegerLiteral, or LHS_Decl was null.\n";
            }
        }

        if (!IsRankVsRootCond) {
            continue;
        }

        // llvm::outs() << "  Found IfStmt with rank vs root condition.\n";
        ReportLoc = If->getBeginLoc();

        llvm::SmallVector<LoopInfo, 2> RootLoopStack;
        llvm::SmallVector<LoopInfo, 2> NonRootLoopStack;
        llvm::SmallVector<MPICallInfo, 4> RootSends;
        llvm::SmallVector<MPICallInfo, 4> RootRecvs;
        llvm::SmallVector<MPICallInfo, 4> NonRootSends;
        llvm::SmallVector<MPICallInfo, 4> NonRootRecvs;

        // Note: For root-based patterns, CurrentLoopVarDecl is passed as nullptr
        // because the primary detection logic isn't based on an iterating loop variable for 'root'
        bool rootBlockAnalyzed = analyzeBlockForMPICalls(If->getThen(), Context,
                                                       RankVarName, CurrentIfRootParameterDecl, CurrentIfRootLiteral,
                                                       nullptr, // No specific loop variable for root branch analysis
                                                       RootSends, RootRecvs, RootLoopStack);

        bool nonRootBlockAnalyzed = false;
        if (If->getElse()) {
            nonRootBlockAnalyzed = analyzeBlockForMPICalls(If->getElse(), Context,
                                                         RankVarName, CurrentIfRootParameterDecl, CurrentIfRootLiteral,
                                                         nullptr, // No specific loop variable for non-root branch analysis
                                                         NonRootSends, NonRootRecvs, NonRootLoopStack);
        }
        // llvm::outs() << rootBlockAnalyzed << " " << nonRootBlockAnalyzed << "\n";
        bool IsGatherCandidate = false;
        bool IsScatterCandidate = false;
        bool IsSendRecvPattern = false;
        std::string PatternSnippet; 

        //Checking the Sendrecv function validity
        if(rootBlockAnalyzed && nonRootBlockAnalyzed){
            bool rootUsesSendrecvLoop = false;
            for (const auto& send : RootSends) {
                if (send.FunctionName.find("Sendrecv") != std::string::npos &&
                    !send.IsRankArgCurrentRank &&                        // not sending to self
                    (send.ContainingLoop || send.IsRankArgLoopVar)) {    // part of loop or uses loop var
                    rootUsesSendrecvLoop = true;
                    if (PatternSnippet.empty()) {
                        PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(send.Call->getSourceRange()), SM, LangOptions()).str();
                    }
                }
            }

            bool nonRootUsesSendrecvToRoot = false;
            for (const auto& recv : NonRootRecvs) {
                if (recv.FunctionName.find("Sendrecv") != std::string::npos) {
                    if ((recv.IsRankArgRoot && CurrentIfRootParameterDecl) ||
                        (recv.RankArgLiteralValue == CurrentIfRootLiteral)) {
                        nonRootUsesSendrecvToRoot = true;
                        if (PatternSnippet.empty()) {
                            PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(recv.Call->getSourceRange()), SM, LangOptions()).str();
                        }
                    }
                }
            }

            if (rootUsesSendrecvLoop && nonRootUsesSendrecvToRoot) {
                // llvm::outs() << "JAI";
                IsSendRecvPattern = true;
            }

        }

        // Checking the recv function validity
        if (rootBlockAnalyzed && nonRootBlockAnalyzed) {
            bool rootRecvFromVaryingOrNonSelf = false;
            for (const auto& rcv : RootRecvs) {
                if (rcv.FunctionName.find("Recv") != std::string::npos && !rcv.IsRankArgCurrentRank && (rcv.ContainingLoop || rcv.IsRankArgLoopVar)) {
                    rootRecvFromVaryingOrNonSelf = true;
                    if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(rcv.Call->getSourceRange()), SM, LangOptions()).str();
                }
            }

            bool nonRootSendsToRoot = false;
            for (const auto& snd : NonRootSends) {
                if (snd.FunctionName.find("Send") != std::string::npos) {
                    if (CurrentIfRootParameterDecl && snd.IsRankArgRoot) {
                        nonRootSendsToRoot = true;
                        if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(snd.Call->getSourceRange()), SM, LangOptions()).str();
                    } else if (CurrentIfRootLiteral != -1 && snd.RankArgLiteralValue == CurrentIfRootLiteral) {
                        nonRootSendsToRoot = true;
                        if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(snd.Call->getSourceRange()), SM, LangOptions()).str();
                    }
                }
            }
            if (rootRecvFromVaryingOrNonSelf && nonRootSendsToRoot) {
                IsGatherCandidate = true;
            }
        }

        // Ckecking the Send function validity
        if (rootBlockAnalyzed && nonRootBlockAnalyzed && !IsGatherCandidate) {
            bool rootSendsToVarying = false;
            for (const auto& snd : RootSends) {
                if (snd.FunctionName.find("Send") != std::string::npos && snd.IsRankArgLoopVar && snd.ContainingLoop) {
                    rootSendsToVarying = true;
                    if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(snd.Call->getSourceRange()), SM, LangOptions()).str();
                }
            }

            bool nonRootRecvFromRoot = false;
            for (const auto& rcv : NonRootRecvs) {
                if (rcv.FunctionName.find("Recv") != std::string::npos) {
                    if (CurrentIfRootParameterDecl && rcv.IsRankArgRoot) {
                        nonRootRecvFromRoot = true;
                        if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(rcv.Call->getSourceRange()), SM, LangOptions()).str();
                    } else if (CurrentIfRootLiteral != -1 && rcv.RankArgLiteralValue == CurrentIfRootLiteral) {
                        nonRootRecvFromRoot = true;
                        if (PatternSnippet.empty()) PatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(rcv.Call->getSourceRange()), SM, LangOptions()).str();
                    }
                }
            }
            if (rootSendsToVarying && nonRootRecvFromRoot) {
                IsScatterCandidate = true;
            }
        }

        if(IsSendRecvPattern){
             // llvm::outs() << "😈CheckPoint 5 (Reporting)\n";
            llvm::outs() << "=============================================================\n";
            llvm::outs() << "Analysis of " << FD->getNameAsString() << " Function\n";
            llvm::outs() << "=============================================================\n";


            llvm::outs() << "Pattern Detected: Manual Data Exchange using MPI_Sendrecv\n";
            llvm::outs() << "- Issue: This function performs a manual data gathering or scattering pattern using MPI_Sendrecv.\n";
            llvm::outs() << "    - Typically, the root process communicates with all other ranks using MPI_Sendrecv to either collect data (manual gather) or distribute data (manual scatter).\n";
            llvm::outs() << "    - Each rank participates in sendrecv operations, potentially with rank-based logic (e.g., `if (rank == root)`), leading to verbose and error-prone code.\n";
            llvm::outs() << "- Suggestion: Consider replacing this pattern with a collective operation:\n";
            llvm::outs() << "    • Use MPI_Gather if the root is collecting data from all processes into a single buffer.\n";
            llvm::outs() << "    • Use MPI_Scatter if the root is distributing portions of a buffer to all processes.\n";
            llvm::outs() << "- Benefit: MPI_Gather and MPI_Scatter are optimized for performance, reduce code complexity, and ensure correctness across diverse architectures.\n";


            llvm::outs() << "- Location: " << FD->getNameAsString() << " function, Line "
                         << SM.getPresumedLoc(ReportLoc).getLine() << "\n";
            llvm::outs() << "Details:\n- Representative code snippet:\n" << PatternSnippet << "\n";
            llvm::outs() << "=============================================================\n\n";
        }

        // Report findings for Gather/Scatter
        if (IsGatherCandidate || IsScatterCandidate) {
            // llvm::outs() << "😈CheckPoint 5 (Reporting)\n";
            llvm::outs() << "=============================================================\n";
            llvm::outs() << "Analysis of " << FD->getNameAsString() << " Function\n";
            llvm::outs() << "=============================================================\n";

            std::string root_id_str;
            if (CurrentIfRootParameterDecl) {
                root_id_str = CurrentIfRootParameterDecl->getNameAsString();
            } else if (CurrentIfRootLiteral != -1) {
                root_id_str = std::to_string(CurrentIfRootLiteral);
            } else {
                root_id_str = "an identified root";
            }

            if (IsGatherCandidate) {
                llvm::outs() << "Pattern Detected: Manual Data Gathering\n";
                llvm::outs() << "- Issue: This function implements a manual Gather operation. Data from all processes is being collected by the root process using point-to-point communication.\n";
                llvm::outs() << "    - Specifically, non-root processes send their local data to the root, and the root process iteratively receives data from all other processes.\n";
                llvm::outs() << "- Suggestion: Consider using MPI_Gather for better performance and scalability.\n";
            } else {
                llvm::outs() << "Pattern Detected: Manual Data Distribution (Scatter)\n";
                llvm::outs() << "- Issue: This function implements a manual Scatter operation. Data from the root process is being distributed to all processes using point-to-point communication.\n";
                llvm::outs() << "    - Specifically, the root process iteratively sends distinct data chunks to each rank, and non-root processes receive their respective chunks from the root.\n";
                llvm::outs() << "- Suggestion: Consider using MPI_Scatter for better performance and scalability.\n";
            }
            llvm::outs() << "- Location: " << FD->getNameAsString() << " function, Line "
                         << SM.getPresumedLoc(ReportLoc).getLine() << "\n";
            llvm::outs() << "Details:\n- Representative code snippet:\n" << PatternSnippet << "\n";
            llvm::outs() << "=============================================================\n\n";
        }
    } 

    // --- Part 2: New Allgather/Alltoall detection (loop-based, non-root) ---

// llvm::outs() << "😈CheckPoint 6: Starting Allgather/Alltoall analysis.\n";

llvm::SmallVector<MPICallInfo, 8> AllP2PSends; 
llvm::SmallVector<MPICallInfo, 8> AllP2PRecvs; 
llvm::SmallVector<LoopInfo, 2> GlobalLoopStack; 

bool foundAnyP2PInLoops = analyzeBlockForMPICalls(Comp, Context,
                                                    RankVarName, nullptr, -1 , nullptr,
                                                    AllP2PSends, AllP2PRecvs, GlobalLoopStack);

if (foundAnyP2PInLoops) {
    // llvm::outs() << "  Found P2P calls in loops. Analyzing for Allgather/Alltoall...\n";

    // Group calls by their containing loop, as a single loop should represent one collective
    std::map<const LoopInfo*, llvm::SmallVector<MPICallInfo, 4>> SendsByLoop;
    std::map<const LoopInfo*, llvm::SmallVector<MPICallInfo, 4>> RecvsByLoop;

    for (const auto& callInfo : AllP2PSends) {
        if (callInfo.ContainingLoop) {
            SendsByLoop[callInfo.ContainingLoop].push_back(callInfo);
        }
    }
    for (const auto& callInfo : AllP2PRecvs) {
        if (callInfo.ContainingLoop) {
            RecvsByLoop[callInfo.ContainingLoop].push_back(callInfo);
        }
    }

    // llvm::outs() << "  Debug: Number of loops with sends: " << SendsByLoop.size() << "\n";

    for (auto const& [loopPtr, sendsInLoop] : SendsByLoop) {
        const LoopInfo* loopInfo = loopPtr;
        const auto& recvsInLoop = RecvsByLoop[loopInfo]; // Empty if no receives in this loop

        // llvm::outs() << "  Debug: Analyzing loop at line " << SM.getPresumedLoc(loopInfo->LoopStmt->getBeginLoc()).getLine() << " in function " << FD->getNameAsString() << "\n";
        // llvm::outs() << "  Debug:  Loop has " << sendsInLoop.size() << " sends and " << recvsInLoop.size() << " receives.\n";


        if (!loopInfo->LoopVarDecl) {
            // llvm::outs() << "  Skipping loop without identifiable loop variable.\n";
            continue;
        }
        // llvm::outs() << "  Debug: Loop variable: " << loopInfo->LoopVarDecl->getNameAsString() << "\n";


        // Heuristic for Allgather/Alltoall
        bool isAllgatherCandidate = false;
        bool isAlltoallCandidate = false;
        std::string currentPatternSnippet;

        bool foundSendToLoopVar = false;
        bool foundRecvFromLoopVar = false;
        bool recvBufIndexedByLoopVar = false;
        bool sendBufIndexedByLoopVar = false; // To distinguish Alltoall

        for (const auto& snd : sendsInLoop) {
            // llvm::outs() << "  Debug:  Processing Send call: " << Lexer::getSourceText(CharSourceRange::getTokenRange(snd.Call->getSourceRange()), SM, LangOptions()).str() << "\n";
            // llvm::outs() << "  Debug:   IsRankArgLoopVar: " << (snd.IsRankArgLoopVar ? "true" : "false") << "\n";
            // llvm::outs() << "  Debug:   IsBufferIndexedByLoopVar (Send): " << (snd.IsBufferIndexedByLoopVar ? "true" : "false") << "\n";

            if (snd.IsRankArgLoopVar) {
                foundSendToLoopVar = true;
                if (snd.BufferExpr && snd.IsBufferIndexedByLoopVar) {
                    sendBufIndexedByLoopVar = true;
                }
                // Only take the first snippet as representative
                if (currentPatternSnippet.empty()) currentPatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(snd.Call->getSourceRange()), SM, LangOptions()).str();
            }
        }
        // llvm::outs() << "  Debug: After processing sends: foundSendToLoopVar = " << (foundSendToLoopVar ? "true" : "false") << ", sendBufIndexedByLoopVar = " << (sendBufIndexedByLoopVar ? "true" : "false") << "\n";


        for (const auto& rcv : recvsInLoop) {
            // llvm::outs() << "  Debug:  Processing Recv call: " << Lexer::getSourceText(CharSourceRange::getTokenRange(rcv.Call->getSourceRange()), SM, LangOptions()).str() << "\n";
            // llvm::outs() << "  Debug:   IsRankArgLoopVar: " << (rcv.IsRankArgLoopVar ? "true" : "false") << "\n";
            // llvm::outs() << "  Debug:   IsBufferIndexedByLoopVar (Recv): " << (rcv.IsBufferIndexedByLoopVar ? "true" : "false") << "\n";

            if (rcv.IsRankArgLoopVar) {
                foundRecvFromLoopVar = true;
                if (rcv.BufferExpr && rcv.IsBufferIndexedByLoopVar) {
                    recvBufIndexedByLoopVar = true;
                }
                if (currentPatternSnippet.empty()) currentPatternSnippet = Lexer::getSourceText(CharSourceRange::getTokenRange(rcv.Call->getSourceRange()), SM, LangOptions()).str();
            }
        }
        // llvm::outs() << "  Debug: After processing receives: foundRecvFromLoopVar = " << (foundRecvFromLoopVar ? "true" : "false") << ", recvBufIndexedByLoopVar = " << (recvBufIndexedByLoopVar ? "true" : "false") << "\n";


        // llvm::outs() << "  Debug: Final checks for candidate pattern:\n";
        // llvm::outs() << "  Debug:  foundSendToLoopVar = " << (foundSendToLoopVar ? "true" : "false") << "\n";
        // llvm::outs() << "  Debug:  foundRecvFromLoopVar = " << (foundRecvFromLoopVar ? "true" : "false") << "\n";
        // llvm::outs() << "  Debug:  sendBufIndexedByLoopVar = " << (sendBufIndexedByLoopVar ? "true" : "false") << "\n";
        // llvm::outs() << "  Debug:  recvBufIndexedByLoopVar = " << (recvBufIndexedByLoopVar ? "true" : "false") << "\n";


        if (foundSendToLoopVar && foundRecvFromLoopVar) {
            // llvm::outs() << "  Debug:  Both Send to Loop Var and Recv from Loop Var found.\n";
            if (sendBufIndexedByLoopVar && recvBufIndexedByLoopVar) {
                isAlltoallCandidate = true;
                // llvm::outs() << "  Debug:   --> Identified as Alltoall Candidate.\n";
            } else if (recvBufIndexedByLoopVar) { // Send buffer not indexed by loop var, but recv buffer is
                isAllgatherCandidate = true;
                // llvm::outs() << "  Debug:   --> Identified as Allgather Candidate.\n";
            } else {
                // llvm::outs() << "  Debug:   --> Not a clear Allgather/Alltoall pattern based on indexing.\n";
            }
        } else {
            // llvm::outs() << "  Debug:  Did not find both Send to Loop Var AND Recv from Loop Var. (foundSendToLoopVar=" << foundSendToLoopVar << ", foundRecvFromLoopVar=" << foundRecvFromLoopVar << ")\n";
        }


        // Report findings for Allgather/Alltoall
        if (isAllgatherCandidate || isAlltoallCandidate) {
            llvm::outs() << "=============================================================\n";
            llvm::outs() << "Analysis of " << FD->getNameAsString() << " Function\n";
            llvm::outs() << "=============================================================\n";

            if (isAllgatherCandidate) {
                llvm::outs() << "Pattern Detected: Manual All-to-All Data Gathering (Allgather)\n";
                llvm::outs() << "- Issue: This function implements a manual Allgather operation. Data from all processes is being gathered by all other processes using point-to-point communication within a loop.\n";
                llvm::outs() << "    - Specifically, each process sends its local data to every other process, and receives data from every other process into a collective buffer indexed by the iterating rank.\n";
                llvm::outs() << "- Suggestion: Consider using MPI_Allgather for better performance and scalability.\n";
                llvm::outs() << "- Note : This may also be a case of manual data gathering at a root process using MPI_Sendrecv so use MPI_Gather if the data is being gathered at root process else use MPI_Allgather.\n";
            } else { 
                llvm::outs() << "Pattern Detected: Manual All-to-All Data Exchange (Alltoall)\n";
                llvm::outs() << "- Issue: This function implements a manual Alltoall operation. Data is being exchanged between all processes using point-to-point communication within a loop.\n";
                llvm::outs() << "    - Specifically, each process sends a distinct chunk of its data to every other process (indexed by iterating rank) and receives a distinct chunk from every other process (indexed by iterating rank).\n";
                llvm::outs() << "- Suggestion: Consider using MPI_Alltoall for better performance and scalability.\n";
            }
            llvm::outs() << "- Location: " << FD->getNameAsString() << " function, Loop starting at Line "
                         << SM.getPresumedLoc(loopInfo->LoopStmt->getBeginLoc()).getLine() << "\n";
            llvm::outs() << "Details:\n- Representative code snippet:\n" << currentPatternSnippet << "\n";
            llvm::outs() << "=============================================================\n\n";
        }
    }
} else {
    // llvm::outs() << "  No P2P calls found in loops for Allgather/Alltoall analysis.\n";
}
}

void registerMPIMatchers(MatchFinder &Finder, MPIAnalyzerCallback &Callback) {
    Finder.addMatcher(
        callExpr(
            callee(functionDecl(hasName("MPI_Comm_rank")))
        ).bind("mpiCall"),
        &Callback
    );
}