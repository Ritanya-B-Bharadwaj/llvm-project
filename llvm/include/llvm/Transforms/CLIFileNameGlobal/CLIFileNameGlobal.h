//===- CLIFileNameGlobal.h - Insert Global Variable Based on Filename ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_CLIFILENAMEGLOBAL_H
#define LLVM_TRANSFORMS_CLIFILENAMEGLOBAL_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class CLIFileNameGlobal : public PassInfoMixin<CLIFileNameGlobal> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

  static StringRef name() { return "CLIFileNameGlobal"; }
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_CLIFILENAMEGLOBAL_H
