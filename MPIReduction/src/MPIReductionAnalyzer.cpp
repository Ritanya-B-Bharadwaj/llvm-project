#include "MPIReductionAnalyzer.h"
#include "Utils.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/CFG.h>
#include <fstream>
#include <sstream>

using namespace llvm;

namespace MPIReductionAnalysis {

void AnalysisStatistics::print() const {
    errs() << "\n=== ANALYSIS STATISTICS ===\n";
    errs() << "Total functions: " << totalFunctions << "\n";
    errs() << "Analyzed functions: " << analyzedFunctions << "\n";
    errs() << "Skipped functions: " << skippedFunctions << "\n";
    errs() << "MPI calls found: " << mpiCallsFound << "\n";
    errs() << "Reductions detected: " << reductionsDetected << "\n";
    
    if (!reductionsByType.empty()) {
        errs() << "Reductions by type:\n";
        for (const auto& pair : reductionsByType) {
            errs() << "  " << reductionTypeToString(pair.first) << ": " << pair.second << "\n";
        }
    }
    errs() << "===========================\n";
}

MPIReductionAnalyzer::MPIReductionAnalyzer(ReductionType type, bool verbose)
    : targetReductionType_(type), verboseOutput_(verbose) {
    detector_ = std::make_unique<ReductionDetector>(type, verbose);
}

bool MPIReductionAnalyzer::analyzeModule(Module& module) {
    Utils::printAnalysis("Starting analysis of module: " + module.getModuleIdentifier());
    
    if (verboseOutput_) {
        Utils::printModuleInfo(module);
    }
    
    stats_.reset();
    detector_->clearPatterns();
    
    bool foundReductions = false;
    
    // Process each function in the module
    for (Function& F : module) {
        stats_.totalFunctions++;
        
        if (Utils::shouldSkipFunction(F)) {
            stats_.skippedFunctions++;
            Utils::printVerbose("Skipping function: " + F.getName().str(), verboseOutput_);
            continue;
        }
        
        Utils::printVerbose("Analyzing function: " + F.getName().str(), verboseOutput_);
        
        if (analyzeSingleFunction(F)) {
            foundReductions = true;
        }
        
        stats_.analyzedFunctions++;
    }
    
    // Process all detected patterns
    if (module.begin() != module.end()) {
    processDetectedPatterns(*module.begin());
}

    
    Utils::printAnalysis("Analysis complete. Found reductions: " + 
                        std::string(foundReductions ? "Yes" : "No"));
    
    if (verboseOutput_) {
        stats_.print();
    }
    
    return foundReductions;
}

bool MPIReductionAnalyzer::analyzeFunction(Function& func) {
    if (Utils::shouldSkipFunction(func)) {
        return false;
    }
    
    return analyzeSingleFunction(func);
}

bool MPIReductionAnalyzer::analyzeSingleFunction(Function& func) {
    Utils::printVerbose("Analyzing function: " + func.getName().str(), verboseOutput_);
    
    bool foundReduction = false;
    
    // Look for MPI calls in this function
    for (BasicBlock& BB : func) {
        for (Instruction& I : BB) {
            if (CallInst* CI = dyn_cast<CallInst>(&I)) {
                if (Function* Callee = CI->getCalledFunction()) {
                    StringRef funcName = Callee->getName();
                    Utils::printVerbose("Found call to: " + funcName.str(), verboseOutput_);
                    
                    // Check for MPI_Recv
                    if (Utils::isMPIFunction(Callee, "MPI_Recv")) {
                        Utils::printAnalysis("Found MPI_Recv call in function: " + func.getName().str());
                        stats_.mpiCallsFound++;
                        
                        // Use detector to find reduction patterns
                        if (detector_->detectReductionAfterMPI(CI)) {
                            foundReduction = true;
                            stats_.reductionsDetected++;
                            
                            // Update statistics by type
                            for (const auto& pattern : detector_->getDetectedPatterns()) {
                                stats_.reductionsByType[pattern.type]++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return foundReduction;
}

void MPIReductionAnalyzer::processDetectedPatterns(Function& func) {
    const auto& patterns = detector_->getDetectedPatterns();
    
    if (!patterns.empty()) {
        Utils::printAnalysis("Processing " + std::to_string(patterns.size()) + " detected patterns");
        
        for (const auto& pattern : patterns) {
            reportReductionPattern(pattern, func);
        }
    }
}

void MPIReductionAnalyzer::reportReductionPattern(const ReductionPattern& pattern, Function& func) {
    errs() << "\n========================================\n";
    errs() << "REDUCTION FOUND\n";
    errs() << "========================================\n";
    
    // Get module source filename if available
    Module* M = func.getParent();
    if (!M->getSourceFileName().empty()) {
        errs() << "File: " << M->getSourceFileName() << "\n";
    }
    
    errs() << "Function: " << func.getName() << "\n";
    errs() << "Reduction Type: " << reductionTypeToString(pattern.type) << "\n";
    errs() << "Description: " << pattern.description << "\n";
    
    // Print location information
    errs() << "Location: " << Utils::getSourceLocation(pattern.mpiCall) << "\n";
    
    errs() << "\nCode Pattern:\n";
    errs() << "----------------------------------------\n";
    
    printCodeContext(pattern.mpiCall, pattern.reductionOp);
    
    errs() << "----------------------------------------\n";
    errs() << "Analysis Details:\n";
    errs() << "- Found MPI_Recv followed by " << reductionTypeToString(pattern.type) << " reduction pattern\n";
    errs() << "- This suggests manual reduction implementation\n";
    errs() << "- Consider using MPI_Reduce or MPI_Allreduce for better performance\n";
    errs() << "========================================\n\n";
}

void MPIReductionAnalyzer::printCodeContext(CallInst* mpiCall, Instruction* reductionOp) {
    BasicBlock* BB = mpiCall->getParent();
    bool printNext = false;
    int contextLines = 0;
    const int maxContext = 15;
    
    for (auto& I : *BB) {
        if (&I == mpiCall) {
            errs() << ">>> " << I << " <<<\n";  // Highlight the MPI call
            printNext = true;
            continue;
        }
        if (printNext && contextLines < maxContext) {
            errs() << "    " << I << "\n";
            contextLines++;
            
            // Stop after finding the reduction operation
            if (&I == reductionOp) {
                errs() << "    ^^^ REDUCTION OPERATION ^^^\n";
                break;
            }
        }
    }
}

void MPIReductionAnalyzer::setReductionType(ReductionType type) {
    targetReductionType_ = type;
    if (detector_) {
        detector_->setReductionType(type);
    }
}

void MPIReductionAnalyzer::setVerbose(bool verbose) {
    verboseOutput_ = verbose;
    if (detector_) {
        detector_->setVerbose(verbose);
    }
}

const std::vector<ReductionPattern>& MPIReductionAnalyzer::getDetectedPatterns() const {
    return detector_->getDetectedPatterns();
}

void MPIReductionAnalyzer::printSummaryReport() const {
    errs() << "\n=== SUMMARY REPORT ===\n";
    errs() << "Input file: " << inputFileName_ << "\n";
    errs() << "Target reduction type: " << reductionTypeToString(targetReductionType_) << "\n";
    stats_.print();
    
    if (stats_.reductionsDetected > 0) {
        errs() << "\nRecommendations:\n";
        errs() << "- Consider replacing manual reductions with MPI collective operations\n";
        errs() << "- MPI_Reduce/MPI_Allreduce can provide better performance and scalability\n";
        errs() << "- Review the detected patterns for optimization opportunities\n";
    }
    errs() << "======================\n";
}

void MPIReductionAnalyzer::printDetailedReport() const {
    printSummaryReport();
    
    const auto& patterns = getDetectedPatterns();
    if (!patterns.empty()) {
        errs() << "\n=== DETAILED PATTERN ANALYSIS ===\n";
        int patternNum = 1;
        for (const auto& pattern : patterns) {
            errs() << "\nPattern #" << patternNum++ << ":\n";
            errs() << "  Type: " << reductionTypeToString(pattern.type) << "\n";
            errs() << "  Description: " << pattern.description << "\n";
            errs() << "  Location: " << Utils::getSourceLocation(pattern.mpiCall) << "\n";
            errs() << "  Block: " << pattern.block->getName() << "\n";
        }
        errs() << "=================================\n";
    }
}

void MPIReductionAnalyzer::exportResults(const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        Utils::printError("Cannot open output file: " + outputFile);
        return;
    }
    
    file << "MPI Reduction Analysis Report\n";
    file << "============================\n\n";
    file << "Input file: " << inputFileName_ << "\n";
    file << "Analysis date: " << __DATE__ << " " << __TIME__ << "\n\n";
    
    file << "Statistics:\n";
    file << "-----------\n";
    file << "Total functions: " << stats_.totalFunctions << "\n";
    file << "Analyzed functions: " << stats_.analyzedFunctions << "\n";
    file << "MPI calls found: " << stats_.mpiCallsFound << "\n";
    file << "Reductions detected: " << stats_.reductionsDetected << "\n\n";
    
    const auto& patterns = getDetectedPatterns();
    if (!patterns.empty()) {
        file << "Detected Patterns:\n";
        file << "==================\n";
        int patternNum = 1;
        for (const auto& pattern : patterns) {
            file << "\nPattern #" << patternNum++ << ":\n";
            file << "  Type: " << reductionTypeToString(pattern.type) << "\n";
            file << "  Description: " << pattern.description << "\n";
            file << "  Location: " << Utils::getSourceLocation(pattern.mpiCall) << "\n";
        }
    }
    
    file.close();
    Utils::printAnalysis("Results exported to: " + outputFile);
}

void MPIReductionAnalyzer::reset() {
    stats_.reset();
    if (detector_) {
        detector_->clearPatterns();
    }
}

} // namespace MPIReductionAnalysis