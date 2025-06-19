#include "llvm/IR/Attributes.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

class HighlightPass : public PassInfoMixin<HighlightPass> {
public:
  struct HighlightCluster {
    DebugLoc start;
    DebugLoc end;
  };

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    std::vector<DebugLoc> TempMarkers;
    std::vector<HighlightCluster> Clusters;

    for (Function &F : M) {
      if (F.getName().starts_with("__highlight_marker")) {
        for (auto &BB : F) {
          for (auto &I : BB) {
            if (auto DL = I.getDebugLoc()) {
              TempMarkers.push_back(DL);
              outs() << "Collected marker: " << DL->getFilename() << ":"
                     << DL->getLine() << "\n";
              break;
            }
          }
          break;
        }
      }
    }

    for (size_t i = 0; i + 1 < TempMarkers.size(); i += 2) {
      DebugLoc A = TempMarkers[i];
      DebugLoc B = TempMarkers[i + 1];
      HighlightCluster cluster;

      if (A.getLine() <= B.getLine()) {
        cluster.start = A;
        cluster.end = B;
      } else {
        cluster.start = B;
        cluster.end = A;
      }

      Clusters.push_back(cluster);
      outs() << "Created cluster from lines " << cluster.start.getLine()
             << " to " << cluster.end.getLine() << "\n";
    }

    for (Function &F : M) {
      if (F.getName().starts_with("__highlight_marker"))
        continue;

      const DISubprogram *SP = F.getSubprogram();
      if (!SP) {
        outs() << "Function " << F.getName() << " has no debug info\n";
        continue;
      }

      unsigned FuncStart = SP->getLine();
      unsigned FuncEnd = FuncStart;
      StringRef Filename = SP->getFile()->getFilename();

      for (auto &BB : F) {
        for (auto &I : BB) {
          if (DebugLoc DL = I.getDebugLoc()) {
            unsigned Line = DL.getLine();
            if (Line > 0)
              FuncEnd = std::max(FuncEnd, Line);
          }
        }
      }

      outs() << "Function: " << F.getName() << " spans lines " << FuncStart
             << " to " << FuncEnd << " in file: " << Filename << "\n";

      for (const auto &cluster : Clusters) {
        if (Filename == cluster.start->getFilename()) {
          unsigned ClusterStart = cluster.start.getLine();
          unsigned ClusterEnd = cluster.end.getLine();
          outs() << "Cluster: " << ClusterStart << " to " << ClusterEnd << "\n";
          if (FuncEnd >= ClusterStart && FuncStart <= ClusterEnd) {

            F.addFnAttr("IsHighlighted", "");

            outs() << " Function " << F.getName()
                   << " marked as IsHighlighted (lines " << FuncStart << " to "
                   << FuncEnd << ")\n";

            if (F.hasFnAttribute("IsHighlighted")) {
              outs() << " VERIFIED: " << F.getName() << " has IsHighlighted\n";
            }
            break;
          }
        }
      }
    }

    return PreservedAnalyses::none();
  }
};

} // namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HighlightPass", LLVM_VERSION_STRING,
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
