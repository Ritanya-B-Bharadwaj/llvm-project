#ifndef LLVM_TRANSFORMS_CLIFILENAMEGLOBAL_H
#define LLVM_TRANSFORMS_CLIFILENAMEGLOBAL_H

#include "llvm/IR/PassManager.h"

using namespace llvm;

struct CLIFileNameGlobal : public PassInfoMixin<CLIFileNameGlobal> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
};

#endif
