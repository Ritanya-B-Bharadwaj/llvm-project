#ifndef LLVM_TRANSFORMS_COMPUTATIONALINTENSITYPASS_H
#define LLVM_TRANSFORMS_COMPUTATIONALINTENSITYPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class ComputationalIntensityPass : public PassInfoMixin<ComputationalIntensityPass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_COMPUTATIONALINTENSITYPASS_H
