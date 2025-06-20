#ifndef MPI_REDUCTION_ANALYZER_H
#define MPI_REDUCTION_ANALYZER_H

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "ReductionDetector.h"
#include "Utils.h"

namespace MPIReductionAnalysis {

// Statistics about the analysis
struct AnalysisStatistics {
    int totalFunctions = 0;
    int analyzedFunctions = 0;
    int skippedFunctions = 0;
    int mpiCallsFound = 0;
    int reductionsDetected = 0;
    std::map<ReductionType, int> reductionsByType;
    
    void reset() {
        totalFunctions = analyzedFunctions = skippedFunctions = 0;
        mpiCallsFound = reductionsDetected = 0;
        reductionsByType.clear();
    }
    
    void print() const;
};

// Main analyzer class that orchestrates the analysis
class MPIReductionAnalyzer {
private:
    std::unique_ptr<ReductionDetector> detector_;
    AnalysisStatistics stats_;
    ReductionType targetReductionType_;
    bool verboseOutput_;
    std::string inputFileName_;
    
    // Analysis methods
    bool analyzeSingleFunction(llvm::Function& func);
    bool findMPICalls(llvm::Function& func);
    void processDetectedPatterns(llvm::Function& func);
    
    // Reporting methods
    void generateDetailedReport(const std::vector<ReductionPattern>& patterns);
    void reportReductionPattern(const ReductionPattern& pattern, llvm::Function& func);
    void printCodeContext(llvm::CallInst* mpiCall, llvm::Instruction* reductionOp);
    
public:
    // Constructor
    MPIReductionAnalyzer(ReductionType type = ReductionType::ALL, bool verbose = false);
    
    // Main analysis interface
    bool analyzeModule(llvm::Module& module);
    bool analyzeFunction(llvm::Function& func);
    
    // Configuration
    void setReductionType(ReductionType type);
    void setVerbose(bool verbose);
    void setInputFileName(const std::string& fileName) { inputFileName_ = fileName; }
    
    // Results and statistics
    const AnalysisStatistics& getStatistics() const { return stats_; }
    const std::vector<ReductionPattern>& getDetectedPatterns() const;
    
    // Report generation
    void printSummaryReport() const;
    void printDetailedReport() const;
    void exportResults(const std::string& outputFile) const;
    
    // Reset state
    void reset();
};

} // namespace MPIReductionAnalysis

#endif // MPI_REDUCTION_ANALYZER_H