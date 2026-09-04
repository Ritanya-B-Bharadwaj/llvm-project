#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

#include "MPIReductionAnalyzer.h"
#include "MPIReductionPass.h"
#include "Utils.h"

using namespace llvm;
using namespace MPIReductionAnalysis;

// Command line options
static cl::opt<std::string> InputFilename(
    cl::Positional, 
    cl::desc("<input bitcode file>"), 
    cl::Required
);

static cl::opt<std::string> ReductionTypeStr(
    "analyze-mpi-reduction",
    cl::desc("Specify the type of reduction to analyze"),
    cl::value_desc("type"),
    cl::init("all"),
    cl::Optional
);

static cl::opt<bool> VerboseOutput(
    "verbose",
    cl::desc("Enable verbose debugging output"),
    cl::init(false)
);

static cl::opt<std::string> OutputFile(
    "output",
    cl::desc("Output file for analysis results"),
    cl::value_desc("filename"),
    cl::init(""),
    cl::Optional
);

static cl::opt<bool> UsePassManager(
    "use-pass",
    cl::desc("Use LLVM pass manager instead of direct analysis"),
    cl::init(false)
);

static cl::opt<bool> DetailedReport(
    "detailed",
    cl::desc("Generate detailed analysis report"),
    cl::init(false)
);

static cl::opt<bool> SummaryOnly(
    "summary",
    cl::desc("Show only summary statistics"),
    cl::init(false)
);

void printUsage(const char* progName) {
    errs() << "Usage: " << progName << " [options] <input.bc>\n\n";
    errs() << "MPI Reduction Analyzer - Detects manual MPI reduction patterns\n\n";
    errs() << "Options:\n";
    errs() << "  -analyze-mpi-reduction=<type>  Type of reduction (sum|product|min|max|all)\n";
    errs() << "  -verbose                       Enable verbose output\n";
    errs() << "  -output=<file>                 Save results to file\n";
    errs() << "  -use-pass                      Use LLVM pass manager\n";
    errs() << "  -detailed                      Generate detailed report\n";
    errs() << "  -summary                       Show only summary\n";
    errs() << "  -help                          Show this help\n\n";
    errs() << "Examples:\n";
    errs() << "  " << progName << " program.bc\n";
    errs() << "  " << progName << " -analyze-mpi-reduction=sum -verbose program.bc\n";
    errs() << "  " << progName << " -output=report.txt -detailed program.bc\n";
}

bool validateInputs(const std::string& reductionType) {
    // Validate reduction type
    ReductionType type = stringToReductionType(reductionType);
    if (type == ReductionType::UNKNOWN) {
        Utils::printError("Invalid reduction type: " + reductionType);
        Utils::printError("Valid types: sum, product, min, max, all");
        return false;
    }
    
    return true;
}

int runDirectAnalysis(Module& module, ReductionType reductionType, bool verbose) {
    Utils::printAnalysis("Running direct analysis");
    
    // Create analyzer
    MPIReductionAnalyzer analyzer(reductionType, verbose);
    analyzer.setInputFileName(InputFilename);
    
    // Run analysis
    bool foundReductions = analyzer.analyzeModule(module);
    
    // Generate reports
    if (SummaryOnly) {
        analyzer.printSummaryReport();
    } else if (DetailedReport) {
        analyzer.printDetailedReport();
    } else {
        analyzer.printSummaryReport();
    }
    
    // Export results if requested
    if (!OutputFile.empty()) {
        analyzer.exportResults(OutputFile);
    }
    
    return foundReductions ? 0 : 1;
}

int runPassAnalysis(Module& module, ReductionType reductionType, bool verbose) {
    Utils::printAnalysis("Running pass-based analysis");
    
    // Create and configure pass
    MPIReductionPass pass(reductionType, verbose);
    
    // Run the pass
    bool foundReductions = pass.runOnModule(module);
    
    // Get results
    const auto& stats = pass.getStatistics();
    
    if (DetailedReport) {
        stats.print();
        
        const auto& patterns = pass.getDetectedPatterns();
        if (!patterns.empty()) {
            errs() << "\nDetected Patterns:\n";
            int i = 1;
            for (const auto& pattern : patterns) {
                errs() << i++ << ". " << pattern.description 
                       << " (" << reductionTypeToString(pattern.type) << ")\n";
                errs() << "   Location: " << Utils::getSourceLocation(pattern.mpiCall) << "\n";
            }
        }
    } else if (!SummaryOnly) {
        stats.print();
    }
    
    return foundReductions ? 0 : 1;
}

int main(int argc, char **argv) {
    InitLLVM X(argc, argv);
    
    // Parse command line options
    cl::ParseCommandLineOptions(argc, argv, "MPI Reduction Pattern Analyzer\n");
    
    // Show help if requested
    if (argc == 1) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Validate inputs
    if (!validateInputs(ReductionTypeStr)) {
        return 1;
    }
    
    // Parse reduction type
    ReductionType reductionType = stringToReductionType(ReductionTypeStr);
    
    // Load LLVM module
    LLVMContext Context;
    SMDiagnostic Err;
    std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, Context);
    
    if (!M) {
        Utils::printError("Failed to load input file: " + InputFilename);
        Err.print(argv[0], errs());
        return 1;
    }
    
    Utils::printAnalysis("Successfully loaded module: " + M->getModuleIdentifier());
    Utils::printAnalysis("Number of functions: " + std::to_string(M->getFunctionList().size()));
    
    if (VerboseOutput) {
        Utils::printModuleInfo(*M);
    }
    
    // Run analysis
    int result;
    if (UsePassManager) {
        result = runPassAnalysis(*M, reductionType, VerboseOutput);
    } else {
        result = runDirectAnalysis(*M, reductionType, VerboseOutput);
    }
    
    if (result == 0) {
        Utils::printAnalysis("Analysis completed successfully - reductions found");
    } else {
        Utils::printAnalysis("Analysis completed - no reductions detected");
    }
    
    return result;
}