// RegularMemAccessPass.cpp
// LLVM IR pass to detect regular (sequential) memory access patterns

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfoMetadata.h"

using namespace llvm;

namespace {
struct RegularMemAccessPass : public FunctionPass {
  static char ID;
  RegularMemAccessPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    bool found = false;
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *gep = dyn_cast<GetElementPtrInst>(&I)) {
          // Check if GEP uses a loop induction variable (simple heuristic: variable index)
          if (gep->getNumIndices() == 1) {
            if (auto *idx = dyn_cast<Instruction>(gep->getOperand(1))) {
              // Heuristic: if index is incremented in a loop, likely sequential
              if (idx->getOpcode() == Instruction::PHI) {
                found = true;
                // Try to get debug location for line number
                if (DILocation *Loc = gep->getDebugLoc()) {
                  errs() << "Function '" << F.getName() << "' has regular memory access patterns (LLVM IR):\n";
                  errs() << "- Sequential access detected at line " << Loc->getLine() << "\n";
                } else {
                  errs() << "Function '" << F.getName() << "' has regular memory access patterns (LLVM IR):\n";
                  errs() << "- Sequential access detected at instruction: ";
                  gep->print(errs());
                  errs() << "\n";
                }
              }
            }
          }
        }
      }
    }
    return false;
  }
};
} // namespace

char RegularMemAccessPass::ID = 0;

static RegisterPass<RegularMemAccessPass> X("analyze-regular-memory-access-ir", "Detect Regular Memory Access Patterns (IR)");
