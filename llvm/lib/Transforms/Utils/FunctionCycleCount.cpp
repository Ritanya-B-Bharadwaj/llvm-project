#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {
struct FunctionCycleCount : public FunctionPass {
  static char ID;
  FunctionCycleCount() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    unsigned CycleCount = 0;
    for (auto &BB : F) {
      for (auto &I : BB) {
        CycleCount++; // Simple: count instructions
      }
    }
    errs() << "Function " << F.getName() << ": " << CycleCount << " cycles\n";
    return false;
  }
};
}

char FunctionCycleCount::ID = 0;
static RegisterPass<FunctionCycleCount> X("function-cycle-count", "Function Cycle Count Pass");

namespace llvm {
  FunctionPass *createFunctionCycleCountPass() { return new FunctionCycleCount(); }
}
