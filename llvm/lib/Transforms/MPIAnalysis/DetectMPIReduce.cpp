#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassPlugin.h"
#include <map>

using namespace llvm;

namespace {
class DetectMPIReducePass : public PassInfoMixin<DetectMPIReducePass> {
private:
  std::map<std::string, int> reductionOps = {
    {"MPI_Reduce", 1}, {"MPI_Allreduce", 1}, {"MPI_Scan", 1},
    {"MPI_Sum", 1}, {"MPI_Max", 1}, {"MPI_Min", 1}
  };

  bool isReductionOperation(Instruction &I) {
    if (auto *BI = dyn_cast<BinaryOperator>(&I)) {
      switch(BI->getOpcode()) {
        case Instruction::FAdd:
        case Instruction::Add:
        case Instruction::FMul:
        case Instruction::Mul:
          return true;
        default:
          return false;
      }
    }
    return false;
  }

  void checkNearbyReductions(Instruction &I, int &manualPatterns) {
    constexpr unsigned LOOKAHEAD = 15;
    BasicBlock::iterator it(&I);
    
    for (unsigned i = 0; i < LOOKAHEAD && ++it != I.getParent()->end(); ++i) {
      if (isReductionOperation(*it)) {
        errs() << "🔍 CONFIRMED Manual Reduction Pattern:\n";
        errs() << "   Communication: ";
        I.print(errs());
        errs() << "\n   Reduction Op:  ";
        it->print(errs());
        errs() << "\n";
        manualPatterns++;
        break;
      }
    }
  }

public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    errs() << "\n========================================\n";
    errs() << " Running MPI Reduction Detection Pass\n";
    errs() << "========================================\n\n";

    int explicitReductions = 0;
    int manualPatterns = 0;

    // Detect explicit MPI reductions
    for (Function &F : M) {
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (auto *CB = dyn_cast<CallBase>(&I)) {
            if (Function *calledFunc = CB->getCalledFunction()) {
              StringRef funcName = calledFunc->getName();
              
              // Check for explicit reductions
              for (auto &op : reductionOps) {
                if (funcName.contains(op.first)) {
                  errs() << "Explicit " << funcName << " found at: ";
                  I.print(errs());
                  errs() << "\n";
                  explicitReductions++;
                  break;
                }
              }

              // Check for manual reduction patterns
              if (funcName.contains("MPI_Send") || funcName.contains("MPI_Recv")) {
                checkNearbyReductions(I, manualPatterns);
              }
            }
          }
        }
      }
    }

    // Summary
    errs() << "\n========================================\n";
    errs() << " Analysis Summary:\n";
    errs() << " - Explicit MPI Reductions: " << explicitReductions << "\n";
    errs() << " - Manual Reduction Patterns: " << manualPatterns << "\n";
    errs() << "========================================\n";

    return PreservedAnalyses::all();
  }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "DetectMPIReduce", "v2.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, ModulePassManager &MPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "detect-mpi-reduce") {
            MPM.addPass(DetectMPIReducePass());
            return true;
          }
          return false;
        });
    }
  };
}
