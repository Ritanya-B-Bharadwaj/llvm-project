// llvm/tools/optviz/src/PassDriver.cpp

#include "PassDriver.h"
#include "SummaryGen.h"                 // your GenAI summary routines
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <cstdlib>
#include <string>

using namespace llvm;

//===----------------------------------------------------------------------===//
// Command‐line options
//===----------------------------------------------------------------------===//

static cl::opt<std::string>
InputFile(cl::Positional,
          cl::desc("<source.cpp|.ll>"),
          cl::Required,
          cl::value_desc("filename"));

static cl::opt<std::string>
OptMode("opt",
        cl::desc("Optimization: use -O2 for grouped levels, or pass/pipeline names\n"
                 "Examples:\n"
                 "  -opt=-O2\n"
                 "  -opt=mem2reg\n"
                 "  -opt=instcombine,loop-unroll"),
        cl::value_desc("mode"));

static cl::opt<bool>
GenAISummaries("summary",
               cl::desc("Auto-generate a natural-language summary of the diff"),
               cl::init(false));

//===----------------------------------------------------------------------===//
// runOpt: invoke `opt` to emit IR with the requested transforms
//===----------------------------------------------------------------------===//

static int runOpt(const std::string &in, const std::string &out) {
  std::string cmd = "opt ";

  // Preserve grouped levels (-O2, -O3), else use the new-PM pipeline
  if (OptMode.rfind("-O", 0) == 0) {
    cmd += OptMode + " ";
  } else {
    cmd += "-passes=" + OptMode + " ";
  }

  cmd += "-S " + in + " -o " + out;
  return std::system(cmd.c_str());
}

//===----------------------------------------------------------------------===//
// runDriver: front-end that emits unoptimized IR (via clang) then diffs it
//===----------------------------------------------------------------------===//

int runDriver(int argc, char **argv) {
  // 1) Parse options
  cl::ParseCommandLineOptions(argc, argv, "optviz - LLVM IR diff tool\n");

  // 2) Compute base filename (strip extension)
  std::string base = InputFile;
  if (auto pos = base.rfind('.'); pos != std::string::npos)
    base = base.substr(0, pos);

  // 3) Emit IR if input is .cpp, else assume .ll
  std::string srcIR = base + ".ll";
  if (InputFile.size() >= 4 &&
      InputFile.substr(InputFile.size() - 4) == ".cpp") {
    std::string clangCmd =
      "clang -S -emit-llvm " + InputFile + " -o " + srcIR;
    if (std::system(clangCmd.c_str()) != 0) {
      errs() << "Error: clang failed to emit LLVM IR for " << InputFile << "\n";
      return 1;
    }
  } else {
    srcIR = InputFile;
  }

  // 4) Run the requested opt pipeline
  std::string optIR = base + ".opt.ll";
  if (runOpt(srcIR, optIR) != 0) {
    errs() << "Error: opt failed with mode '" << OptMode << "'\n";
    return 1;
  }

  // 5) Diff the IR
  int diffRet = runIRDiff(srcIR, optIR);

  // 6) Optionally generate a GenAI summary of the diff
  if (diffRet == 0 && GenAISummaries) {
    outs() << "\n=== Auto-generated Summary ===\n\n";
    // Calls your LLM-based summarizer: pass it the IR file paths
    std::string summary = SummaryGen::summarize(srcIR, optIR);
    outs() << summary << "\n";
  }

  return diffRet;
}