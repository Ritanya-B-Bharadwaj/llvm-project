#ifndef REDUCTION_DETECTOR_H
#define REDUCTION_DETECTOR_H

#include <llvm/IR/Instructions.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <set>
#include <vector>
#include "Utils.h"

namespace MPIReductionAnalysis {

// Structure to hold information about a detected reduction pattern
struct ReductionPattern {
    llvm::CallInst* mpiCall;           // The MPI_Recv call
    llvm::Instruction* reductionOp;    // The reduction operation (add, mul, select, etc.)
    ReductionType type;                // Type of reduction detected
    llvm::BasicBlock* block;           // Block where reduction was found
    std::string description;           // Human-readable description
    
    ReductionPattern(llvm::CallInst* call, llvm::Instruction* op, ReductionType t, 
                    llvm::BasicBlock* bb, const std::string& desc)
        : mpiCall(call), reductionOp(op), type(t), block(bb), description(desc) {}
};

// Main class responsible for detecting reduction patterns
// Main class responsible for detecting reduction patterns
class ReductionDetector {
private:
    ReductionType targetType_;         // What type of reduction to look for
    bool verboseOutput_;               // Enable verbose debugging
    std::vector<ReductionPattern> detectedPatterns_;  // Found patterns
    
    // Internal analysis methods
    bool analyzePostMPIPattern(llvm::CallInst* mpiCall);
    bool searchForReductionInBlocks(llvm::CallInst* mpiCall, 
                                   std::set<llvm::BasicBlock*>& visited,
                                   std::vector<llvm::BasicBlock*>& toVisit);
    
    // Pattern recognition methods
    bool isReductionBinaryOp(llvm::BinaryOperator* binOp);
    bool isMinMaxSelect(llvm::SelectInst* selectInst);
    bool isReductionCall(llvm::CallInst* callInst);
    
    // Analysis helper methods
    void analyzeInstructionPattern(llvm::Instruction* inst, int index);
    void analyzeSuccessorBlocks(llvm::BasicBlock* block);

public:
    ReductionDetector(ReductionType type = ReductionType::ALL, bool verbose = false);
    
    // Main detection interface
    bool detectReductionAfterMPI(llvm::CallInst* mpiCall);
    
    // Pattern analysis
    void performDetailedAnalysis(llvm::CallInst* mpiCall);
    
    // ✅ ADD THIS LINE FOR GENERAL FUNCTION-WIDE REDUCTION DETECTION
    void analyzeFunctionForGeneralReductions(llvm::Function* F);

    // Getters
    const std::vector<ReductionPattern>& getDetectedPatterns() const { return detectedPatterns_; }
    size_t getPatternCount() const { return detectedPatterns_.size(); }
    
    // Configuration
    void setReductionType(ReductionType type) { targetType_ = type; }
    void setVerbose(bool verbose) { verboseOutput_ = verbose; }
    
    // Clear detected patterns
    void clearPatterns() { detectedPatterns_.clear(); }
};


} // namespace MPIReductionAnalysis

#endif // REDUCTION_DETECTOR_H