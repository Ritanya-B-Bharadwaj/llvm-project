#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>

using namespace llvm;

namespace {
class HeapAccessTrackerPass : public PassInfoMixin<HeapAccessTrackerPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    std::ofstream outFile("heap_access_report.csv", std::ios::app);

    for (auto &I : instructions(F)) {
      if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
        handleMemoryAccess(LI->getPointerOperand(), "read", LI, F, outFile);
      } else if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
        handleMemoryAccess(SI->getPointerOperand(), "write", SI, F, outFile);
      }
    }

    outFile.close();
    return PreservedAnalyses::all();
  }

private:
  void handleMemoryAccess(Value *Ptr, StringRef AccessType, Instruction *I,
                          Function &F, std::ofstream &outFile) {
    if (!Ptr->getType()->isPointerTy())
      return;

    // Attempt to get debug location
    if (DILocation *Loc = I->getDebugLoc()) {
      std::string FuncName = F.getName().str();
      std::string File = Loc->getFilename().str();
      unsigned Line = Loc->getLine();
      outFile << FuncName << "," << Ptr << "," << AccessType.str() << ","
              << File << "," << Line << "\n";
    }
  }
};
} // namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "HeapAccessTracker", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "heap-access-tracker") {
            FPM.addPass(HeapAccessTrackerPass());
            return true;
          }
          return false;
        });
    }};
}

