#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <string>

using namespace llvm;

static cl::opt<std::string> InputFile(cl::Positional, cl::desc("<input.cpp>"), cl::Required);
static cl::opt<std::string> OutputFile("o", cl::desc("Output file"), cl::value_desc("filename"), cl::init("annotated.ll"));

int main(int argc, char **argv) {
  InitLLVM X(argc, argv);
  cl::ParseCommandLineOptions(argc, argv, "OpenMP Annotator CLI\n");

  std::string IRFile = InputFile.substr(0, InputFile.find_last_of('.')) + ".ll";

  // Step 1: Compile input.cpp to IR
  std::string ClangCmd = ".\\bin\\clang++.exe -fopenmp -S -emit-llvm -I .\\include " + InputFile + " -o " + IRFile;
  outs() << "[1] Running Clang: " << ClangCmd << "\n";
  if (std::system(ClangCmd.c_str()) != 0) {
    errs() << "✖ Failed to compile " << InputFile << " to IR.\n";
    return 1;
  }

  // Step 2: Load IR
  LLVMContext Context;
  SMDiagnostic Err;
  std::unique_ptr<Module> M = parseIRFile(IRFile, Err, Context);
  if (!M) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Step 3: Set up the Module pass pipeline
  PassBuilder PB;

  ModuleAnalysisManager MAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  LoopAnalysisManager LAM;

  PB.registerModuleAnalyses(MAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  ModulePassManager MPM;
  if (auto Err = PB.parsePassPipeline(MPM, "openmp-annotator")) {
    errs() << "✖ Failed to parse pipeline: openmp-annotator\n";
    return 1;
  }

  // Step 4: Run the pass
  MPM.run(*M, MAM);

  // Step 5: Save output
  std::error_code EC;
  raw_fd_ostream OS(OutputFile, EC, sys::fs::OF_None);
  if (EC) {
    errs() << "✖ Failed to open output file: " << EC.message() << "\n";
    return 1;
  }

  M->print(OS, nullptr);
  outs() << "✔ Annotated IR written to " << OutputFile << "\n";

  // Step 6: Run GenAI explanation
  std::string ExplanationFile = OutputFile.substr(0, OutputFile.find_last_of('.')) + "_explanations.json";
  std::string GenAIScript = "..\\genai-tools\\genai_openmp_ir_explainer.py";

  std::string PythonPath = "\"C:\\Users\\manas\\AppData\\Local\\Programs\\Python\\Python313\\python.exe\"";
  std::string GenAICommand = PythonPath + " " + GenAIScript + " " + InputFile + " " + OutputFile + " " + ExplanationFile;
  outs() << "[6] Running GenAI explainer:\n  " << GenAICommand << "\n";


  int retCode = std::system(GenAICommand.c_str());
  if (retCode != 0) {
    errs() << "⚠️  GenAI explanation step failed.\n";
  } else {
    outs() << "✔ Explanation written to: " << ExplanationFile << "\n";
  }



  return 0;
}
