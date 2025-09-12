#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/Support/raw_ostream.h>

namespace MPIReductionAnalysis {

// Reduction type enumeration
enum class ReductionType {
    SUM,
    PRODUCT,
    MIN,
    MAX,
    ALL,
    UNKNOWN
};

// Utility functions for the MPI reduction analyzer
class Utils {
public:
    // Debug and logging utilities
    static void printVerbose(const std::string& message, bool verbose = true);
    static void printAnalysis(const std::string& message);
    static void printError(const std::string& message);
    
    // Instruction analysis utilities
    static bool isMPIFunction(llvm::Function* func, const std::string& mpiCallName);
    static bool isLoopCounter(llvm::BinaryOperator* binOp);
    static void printInstructionDetails(llvm::Instruction* inst, int index, const std::string& prefix = "");
    
    // Function analysis utilities
    static bool shouldSkipFunction(llvm::Function& func);
    static std::string getFunctionSignature(llvm::Function& func);
    
    // Module utilities
    static void printModuleInfo(llvm::Module& module);
    static std::string getSourceLocation(llvm::Instruction* inst);
};

// Constants used throughout the analyzer
struct Constants {
    static const int MAX_ANALYSIS_DEPTH = 10;
    static const int MAX_CONTEXT_INSTRUCTIONS = 20;
    static const int MAX_SUCCESSOR_INSTRUCTIONS = 10;
};

// Conversion helpers
ReductionType stringToReductionType(const std::string& typeStr);
std::string reductionTypeToString(ReductionType type);

} // namespace MPIReductionAnalysis

#endif // UTILS_H
