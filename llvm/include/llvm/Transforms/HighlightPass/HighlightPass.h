#ifndef LLVM_TRANSFORMS_HIGHLIGHTPASS_H
#define LLVM_TRANSFORMS_HIGHLIGHTPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

/// HighlightPass:
/// - accepts one or more `-cluster=start-end` options
/// - marks any function whose basic-block instruction debug line
///   overlaps any [start,end] cluster with the "IsHighlighted" attribute
class HighlightPass : public PassInfoMixin<HighlightPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_HIGHLIGHTPASS_H
