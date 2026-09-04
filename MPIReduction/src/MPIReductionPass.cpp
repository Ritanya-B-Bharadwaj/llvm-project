#include "MPIReductionPass.h"
#include "Utils.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>

using namespace llvm;

namespace MPIReductionAnalysis {

char MPIReductionPass::ID = 0;

MPIReductionPass::MPIReductionPass() : ModulePass(ID) {
    analyzer_ = std::make_unique<MPIReductionAnalyzer>();
}

MPIReductionPass::MPIReductionPass(ReductionType type, bool verbose) : ModulePass(ID) {
    analyzer_ = std::make_unique<MPIReductionAnalyzer>(type, verbose);
}

bool MPIReductionPass::runOnModule(Module &M) {
    Utils::printAnalysis("Starting MPI Reduction Pass on module: " + M.getModuleIdentifier());
    
    // Set the input filename if available
    if (!M.getSourceFileName().empty()) {
        analyzer_->setInputFileName(M.getSourceFileName());
    }
    
    // Run the analysis
    bool foundReductions = analyzer_->analyzeModule(M);
    
    // Print summary report
    analyzer_->printSummaryReport();
    
    // Analysis passes should not modify the module
    return false;
}

void MPIReductionPass::getAnalysisUsage(AnalysisUsage &AU) const {
    // This is an analysis pass, so it doesn't modify anything
    AU.setPreservesAll();
    
    // We might want to use other analysis passes in the future
    // AU.addRequired<LoopInfoWrapperPass>();
    // AU.addRequired<ScalarEvolutionWrapperPass>();
}

void MPIReductionPass::setReductionType(ReductionType type) {
    if (analyzer_) {
        analyzer_->setReductionType(type);
    }
}

void MPIReductionPass::setVerbose(bool verbose) {
    if (analyzer_) {
        analyzer_->setVerbose(verbose);
    }
}

const AnalysisStatistics& MPIReductionPass::getStatistics() const {
    return analyzer_->getStatistics();
}

const std::vector<ReductionPattern>& MPIReductionPass::getDetectedPatterns() const {
    return analyzer_->getDetectedPatterns();
}

} // namespace MPIReductionAnalysis

// LLVM pass registration
static RegisterPass<MPIReductionAnalysis::MPIReductionPass> X(
    "mpi-reduction-analyzer",
    "MPI Reduction Pattern Analyzer Pass",
    false /* Only looks at CFG */,
    true  /* Analysis Pass */
);

// Factory function for creating the pass
extern "C" llvm::ModulePass* createMPIReductionPass() {
    return new MPIReductionAnalysis::MPIReductionPass();
}