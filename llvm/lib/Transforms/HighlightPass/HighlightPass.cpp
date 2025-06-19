//===- HighlightPass.cpp - Function Highlighting Pass --------------------===//
//
// This pass reads line‑range clusters from the command line and marks any
// function whose instructions fall within those ranges with the
// IsHighlighted attribute.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/HighlightPass/HighlightPass.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <vector>
#include <string>

using namespace llvm;

//===----------------------------------------------------------------------===//
// Command‑line option: --cluster=start-end[,start2-end2,...]
//===----------------------------------------------------------------------===//
static cl::list<std::string> ClusterRanges(
    "cluster", cl::ZeroOrMore, cl::CommaSeparated,
    cl::desc("Highlight clusters as start-end line ranges, comma-separated"));

// Structure to hold a line‑range cluster
struct HighlightCluster {
  unsigned StartLine;
  unsigned EndLine;
};

// Parse the `ClusterRanges` strings into numeric clusters
static std::vector<HighlightCluster> parseClustersFromCommandLine() {
  std::vector<HighlightCluster> Parsed;
  for (auto &Entry : ClusterRanges) {
    auto DashPos = Entry.find('-');
    if (DashPos != std::string::npos) {
      unsigned S = std::stoi(Entry.substr(0, DashPos));
      unsigned E = std::stoi(Entry.substr(DashPos + 1));
      if (S <= E)
        Parsed.push_back({S, E});
      else
        errs() << "Warning: invalid cluster '" << Entry << "' (start > end)\n";
    } else {
      errs() << "Warning: invalid cluster format '" << Entry
             << "' (expected start-end)\n";
    }
  }
  return Parsed;
}

// Returns true if DL’s line is within any cluster
static bool inAnyCluster(const DebugLoc &DL,
                         ArrayRef<HighlightCluster> Clusters) {
  if (!DL) return false;
  unsigned L = DL.getLine();
  for (auto &C : Clusters)
    if (L >= C.StartLine && L <= C.EndLine)
      return true;
  return false;
}

//===----------------------------------------------------------------------===//
// HighlightPass Implementation
//===----------------------------------------------------------------------===//
PreservedAnalyses HighlightPass::run(Module &M,
                                     ModuleAnalysisManager &) {
  errs() << "[HighlightPass] Module: " << M.getName() << "\n";

  // Load clusters from the command line once
  auto Clusters = parseClustersFromCommandLine();

  // Iterate over all functions
  for (Function &F : M) {
    if (F.isDeclaration()) continue;

    bool ShouldMark = false;
    // Walk every basic block and instruction
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (inAnyCluster(I.getDebugLoc(), Clusters)) {
          ShouldMark = true;
          break;
        }
      }
      if (ShouldMark) break;
    }

    if (ShouldMark) {
      errs() << "  -> Marking function: " << F.getName() << "\n";
      F.addFnAttr("IsHighlighted");
    }
  }

  return PreservedAnalyses::all();
}

//===----------------------------------------------------------------------===//
// Plugin Registration
//===----------------------------------------------------------------------===//
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HighlightPass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
          [](StringRef Name, ModulePassManager &MPM,
             ArrayRef<PassBuilder::PipelineElement>) {
            if (Name == "highlight") {
              MPM.addPass(HighlightPass());
              return true;
            }
            return false;
          });
    }};
}
