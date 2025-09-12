#include "llvm/IR/PassManager.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugLoc.h"

using namespace llvm;

struct HighlightCluster {
    DebugLoc Start;
    DebugLoc End;
};

namespace {
struct HighlightPass : public PassInfoMixin<HighlightPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        if (F.isDeclaration()) return PreservedAnalyses::all();

        // Fake example cluster: lines 5–10 in "example.c"
        StringRef targetFile = "example.c";
        unsigned startLine = 5, endLine = 10;

        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (const DebugLoc &DL = I.getDebugLoc()) {
                    if (DL.getLine() >= startLine && DL.getLine() <= endLine &&
                        DL.getFilename().endswith(targetFile)) {
                        errs() << "Highlighting function: " << F.getName() << "\n";

                        // Add a custom attribute to the function
                        F.addFnAttr("IsHighlighted");
                        return PreservedAnalyses::all();
                    }
                }
            }
        }

        return PreservedAnalyses::all();
    }
};
} // namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "HighlightPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "highlight") {
                        FPM.addPass(HighlightPass());
                        return true;
                    }
                    return false;
                });
        }};
}
