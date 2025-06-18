#ifndef LLVM_TRANSFORMS_OPENMPANNOTATORPASS_H
#define LLVM_TRANSFORMS_OPENMPANNOTATORPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class OpenMPAnnotatorPass : public PassInfoMixin<OpenMPAnnotatorPass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};

} // namespace llvm

#endif
