#ifndef LLVM_TRANSFORMS_UTILS_ANALYZEMEMORYACCESSPATTERNS_H
#define LLVM_TRANSFORMS_UTILS_ANALYZEMEMORYACCESSPATTERNS_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"

namespace llvm {

class AnalyzeMemoryAccessPatternsPass : public PassInfoMixin<AnalyzeMemoryAccessPatternsPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif
