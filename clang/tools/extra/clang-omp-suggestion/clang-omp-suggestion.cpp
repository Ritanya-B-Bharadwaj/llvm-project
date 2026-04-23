#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;

// --- Command-Line Options ---
static cl::opt<std::string> Feature(
    "feature",
    cl::desc("A semantic description of the OpenMP feature"),
    cl::value_desc("feature_description"),
    cl::Required);

static cl::opt<std::string> Module(
    "module",
    cl::desc("Optional: Filter suggestions by a specific module/role"),
    cl::value_desc("module_name"),
    cl::Optional);

int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "Clang OpenMP Suggestion Tool\n");

    // Find the path to the Python executable
    auto PythonExecutable = sys::findProgramByName("python");
    if (!PythonExecutable) {
        errs() << "Error: Could not find 'python' in your PATH.\n";
        return 1;
    }

    // Use the Python path for the command
    StringRef PythonPath = *PythonExecutable;

    // --- Construct the command to run the Python script ---
    StringRef PythonScriptPath = "c:/llvm-project/clang/tools/extra/omp-suggestion-tool/omp_suggest.py";


    SmallVector<StringRef, 4> Args;
    Args.push_back(PythonPath);
    Args.push_back(PythonScriptPath);
    Args.push_back("--feature");
    Args.push_back(Feature);
    if (Module.getNumOccurrences() > 0) {
        Args.push_back("--module");
        Args.push_back(Module);
    }

    // --- Execute the command ---
    int Result = sys::ExecuteAndWait(
        Args[0], // Program
        Args,    // Arguments
        {},      // Environment (empty ArrayRef)
        {},      // Redirects
        0,       // Seconds to wait (0 = wait indefinitely)
        0        // Memory limit
    );

    if (Result != 0) {
        errs() << "Error executing Python script. Exit code: " << Result << "\n";
        return Result;
    }

    outs() << "Python script executed successfully.\n";
    return 0;
}
