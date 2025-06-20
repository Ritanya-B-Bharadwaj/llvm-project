#include "ReductionDetector.h"
#include "Utils.h"
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace MPIReductionAnalysis {

ReductionDetector::ReductionDetector(ReductionType type, bool verbose)
    : targetType_(type), verboseOutput_(verbose) {
}

bool ReductionDetector::detectReductionAfterMPI(CallInst* mpiCall) {
    if (!mpiCall) {
        Utils::printError("Invalid MPI call provided to detector");
        return false;
    }
    
    Utils::printVerbose("Starting reduction detection after MPI call", verboseOutput_);
    
    // Perform detailed analysis if verbose mode is enabled
    if (verboseOutput_) {
        performDetailedAnalysis(mpiCall);
    }
    
    // Look for reduction patterns
    return analyzePostMPIPattern(mpiCall);
}

void ReductionDetector::performDetailedAnalysis(CallInst* mpiCall) {
    Utils::printAnalysis("=== DETAILED ANALYSIS AFTER MPI_Recv ===");
    
    BasicBlock* BB = mpiCall->getParent();
    bool afterRecv = false;
    int instCount = 0;
    
    // Analyze current basic block
    Utils::printAnalysis("Instructions in current basic block:");
    for (Instruction& I : *BB) {
        if (&I == mpiCall) {
            errs() << "[MPI_RECV] " << I << "\n";
            afterRecv = true;
            continue;
        }
        if (afterRecv && instCount < Constants::MAX_CONTEXT_INSTRUCTIONS) {
            analyzeInstructionPattern(&I, instCount);
            instCount++;
        }
    }
    
    // Analyze successor blocks
    analyzeSuccessorBlocks(BB);
    Utils::printAnalysis("=== END DETAILED ANALYSIS ===");
}

void ReductionDetector::analyzeInstructionPattern(Instruction* inst, int index) {
    Utils::printInstructionDetails(inst, index, "");
    
    // Check for potential reduction patterns
    if (BinaryOperator* BO = dyn_cast<BinaryOperator>(inst)) {
        if (isReductionBinaryOp(BO)) {
            Utils::printAnalysis("    -> POTENTIAL REDUCTION OPERATION!");
        }
    } else if (SelectInst* SI = dyn_cast<SelectInst>(inst)) {
        if (isMinMaxSelect(SI)) {
            Utils::printAnalysis("    -> POTENTIAL MIN/MAX OPERATION!");
        }
    } else if (CallInst* CI = dyn_cast<CallInst>(inst)) {
        if (isReductionCall(CI)) {
            Utils::printAnalysis("    -> POTENTIAL REDUCTION CALL!");
        }
    }
}
void ReductionDetector::analyzeFunctionForGeneralReductions(Function* F) {
    if (!F) {
        Utils::printError("Invalid function provided for reduction scan.");
        return;
    }

    Utils::printVerbose("Starting full-function reduction scan", verboseOutput_);

    for (BasicBlock& BB : *F) {
        for (Instruction& I : BB) {
            if (BinaryOperator* BO = dyn_cast<BinaryOperator>(&I)) {
                if (isReductionBinaryOp(BO)) {
                    Utils::printAnalysis("FOUND GENERAL REDUCTION: " + std::string(BO->getOpcodeName()));
                    detectedPatterns_.emplace_back(nullptr, BO,
                        (BO->getOpcode() == Instruction::Add || BO->getOpcode() == Instruction::FAdd) ? ReductionType::SUM :
                        (BO->getOpcode() == Instruction::Sub || BO->getOpcode() == Instruction::FSub) ? ReductionType::ALL :
                        ReductionType::PRODUCT,
                        &BB, "General binary reduction");
                }
            } else if (SelectInst* SI = dyn_cast<SelectInst>(&I)) {
                if (isMinMaxSelect(SI)) {
                    Utils::printAnalysis("FOUND GENERAL MIN/MAX SELECT");
                    detectedPatterns_.emplace_back(nullptr, SI, ReductionType::MIN, &BB, "General min/max select");
                }
            } else if (CallInst* CI = dyn_cast<CallInst>(&I)) {
                if (isReductionCall(CI)) {
                    Utils::printAnalysis("FOUND GENERAL REDUCTION CALL");
                    detectedPatterns_.emplace_back(nullptr, CI, ReductionType::ALL, &BB, "General reduction call");
                }
            }
        }
    }

    Utils::printVerbose("Finished full-function reduction scan", verboseOutput_);
}

void ReductionDetector::analyzeSuccessorBlocks(BasicBlock* block) {
    Utils::printAnalysis("Successor basic blocks:");
    
    for (BasicBlock* Succ : successors(block)) {
        Utils::printAnalysis("Successor block instructions:");
        int succInstCount = 0;
        
        for (Instruction& I : *Succ) {
            if (succInstCount < Constants::MAX_SUCCESSOR_INSTRUCTIONS) {
                Utils::printInstructionDetails(&I, succInstCount, "S");
                succInstCount++;
            }
        }
        
        if (succInstCount >= Constants::MAX_SUCCESSOR_INSTRUCTIONS) {
            Utils::printAnalysis("... (truncated)");
        }
    }
}

bool ReductionDetector::analyzePostMPIPattern(CallInst* mpiCall) {
    BasicBlock* BB = mpiCall->getParent();
    std::set<BasicBlock*> visited;
    std::vector<BasicBlock*> toVisit;
    
    toVisit.push_back(BB);
    
    return searchForReductionInBlocks(mpiCall, visited, toVisit);
}

bool ReductionDetector::searchForReductionInBlocks(CallInst* mpiCall,
                                                   std::set<BasicBlock*>& visited,
                                                   std::vector<BasicBlock*>& toVisit) {
    bool foundReduction = false;
    bool afterRecv = false;
    
    while (!toVisit.empty() && !foundReduction) {
        BasicBlock* currentBB = toVisit.back();
        toVisit.pop_back();
        
        if (visited.count(currentBB)) continue;
        visited.insert(currentBB);
        
        for (Instruction& I : *currentBB) {
            // Skip instructions before MPI call in the original block
            if (currentBB == mpiCall->getParent()) {
                if (&I == mpiCall) {
                    afterRecv = true;
                    continue;
                }
                if (!afterRecv) continue;
            }
            
            // Check for binary operations
            if (BinaryOperator* BO = dyn_cast<BinaryOperator>(&I)) {
                if (isReductionBinaryOp(BO)) {
                    Utils::printAnalysis("FOUND REDUCTION: " + std::string(BO->getOpcodeName()));
                    detectedPatterns_.emplace_back(mpiCall, BO, 
                        (BO->getOpcode() == Instruction::Add || BO->getOpcode() == Instruction::FAdd) ? 
                        ReductionType::SUM : ReductionType::PRODUCT,
                        currentBB, "Binary reduction operation");
                    foundReduction = true;
                    break;
                }
            }
            // Check for select instructions (min/max)
            else if (SelectInst* SI = dyn_cast<SelectInst>(&I)) {
                if (isMinMaxSelect(SI)) {
                    Utils::printAnalysis("FOUND MIN/MAX SELECT");
                    detectedPatterns_.emplace_back(mpiCall, SI, ReductionType::MIN,
                        currentBB, "Min/Max select operation");
                    foundReduction = true;
                    break;
                }
            }
            // Check for function calls
            else if (CallInst* CI = dyn_cast<CallInst>(&I)) {
                if (isReductionCall(CI)) {
                    Utils::printAnalysis("FOUND POTENTIAL REDUCTION CALL");
                    detectedPatterns_.emplace_back(mpiCall, CI, ReductionType::ALL,
                        currentBB, "Reduction function call");
                    foundReduction = true;
                    break;
                }
            }
        }
        
        // Add successor blocks for analysis (limited depth)
        if (!foundReduction && visited.size() < Constants::MAX_ANALYSIS_DEPTH) {
            for (BasicBlock* Succ : successors(currentBB)) {
                if (!visited.count(Succ)) {
                    toVisit.push_back(Succ);
                }
            }
        }
    }
    
    return foundReduction;
}

bool ReductionDetector::isReductionBinaryOp(BinaryOperator* BO) {
    if (!BO) return false;
    
    // Filter out loop counters
    if (Utils::isLoopCounter(BO)) {
        return false;
    }
    
    switch (BO->getOpcode()) {
        case Instruction::Add:
        case Instruction::FAdd:
            return targetType_ == ReductionType::SUM || targetType_ == ReductionType::ALL;
        case Instruction::Mul:
        case Instruction::FMul:
            return targetType_ == ReductionType::PRODUCT || targetType_ == ReductionType::ALL;
        case Instruction::Sub:
        case Instruction::FSub:
            return targetType_ == ReductionType::ALL; // Sometimes used in reduction patterns
        default:
            return false;
    }
}

bool ReductionDetector::isMinMaxSelect(SelectInst* SI) {
    if (!SI) return false;
    
    if (targetType_ != ReductionType::MIN && 
        targetType_ != ReductionType::MAX && 
        targetType_ != ReductionType::ALL) {
        return false;
    }
    
    // Check if condition is a comparison
    if (ICmpInst* Cmp = dyn_cast<ICmpInst>(SI->getCondition())) {
        ICmpInst::Predicate pred = Cmp->getPredicate();
        return (pred == ICmpInst::ICMP_SLT || pred == ICmpInst::ICMP_ULT ||
                pred == ICmpInst::ICMP_SGT || pred == ICmpInst::ICMP_UGT ||
                pred == ICmpInst::ICMP_SLE || pred == ICmpInst::ICMP_ULE ||
                pred == ICmpInst::ICMP_SGE || pred == ICmpInst::ICMP_UGE);
    }
    
    if (FCmpInst* Cmp = dyn_cast<FCmpInst>(SI->getCondition())) {
        FCmpInst::Predicate pred = Cmp->getPredicate();
        return (pred == FCmpInst::FCMP_OLT || pred == FCmpInst::FCMP_OGT ||
                pred == FCmpInst::FCMP_OLE || pred == FCmpInst::FCMP_OGE);
    }
    
    return false;
}

bool ReductionDetector::isReductionCall(CallInst* CI) {
    if (!CI) return false;
    
    Function* F = CI->getCalledFunction();
    if (!F) return false;
    
    StringRef name = F->getName();
    return (name.contains("min") || name.contains("max") || 
            name.contains("sum") || name.contains("add") ||
            name.contains("reduce"));
}

} // namespace MPIReductionAnalysis