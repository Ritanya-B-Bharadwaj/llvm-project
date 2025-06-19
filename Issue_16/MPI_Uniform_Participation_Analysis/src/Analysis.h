#pragma once
#include "Data.h"
#include <string>
#include <vector>
#include <memory>

namespace llvm {
    class Module; // Forward declaration is fine here
    class LLVMContext;
}

class MPIAnalysis {
public:
    MPIAnalysis(bool verbose);
    ~MPIAnalysis(); // <-- ADDED: Explicit destructor declaration
    bool processFile(const std::string& filePath);
    void runAnalysis();
    const AnalysisResults& getResults() const;

private:
    std::vector<std::unique_ptr<llvm::Module>> modules;
    llvm::LLVMContext& context;
    std::vector<MPICallInfo> allCalls;
    AnalysisResults results;
    bool verbose_mode;
};