#ifndef MPI_REDUCTION_PASS_H
#define MPI_REDUCTION_PASS_H

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <memory>
#include "MPIReductionAnalyzer.h"
#include "Utils.h"

namespace MPIReductionAnalysis {

// LLVM ModulePass wrapper for the MPI reduction analyzer
class MPIReductionPass : public llvm::ModulePass {
private:
    std::unique_ptr<MPIReductionAnalyzer> analyzer_;
    
public:
    static char ID;
    
    MPIReductionPass();
    explicit MPIReductionPass(ReductionType type, bool verbose = false);
    
    // LLVM Pass interface
    bool runOnModule(llvm::Module &M) override;
    
    // Pass information
    void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    llvm::StringRef getPassName() const override { 
        return "MPI Reduction Pattern Analyzer Pass"; 
    }
    
    // Configuration methods
    void setReductionType(ReductionType type);
    void setVerbose(bool verbose);
    
    // Results access
    const AnalysisStatistics& getStatistics() const;
    const std::vector<ReductionPattern>& getDetectedPatterns() const;
};

} // namespace MPIReductionAnalysis

// LLVM pass registration
extern "C" llvm::ModulePass* createMPIReductionPass();

#endif // MPI_REDUCTION_PASS_H