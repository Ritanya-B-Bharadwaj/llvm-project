#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct ComputationalIntensityPass : public PassInfoMixin<ComputationalIntensityPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        errs() << "Analyzing Function: " << F.getName() << "\n";
        int arithmeticOps = 0;
        int memoryOps = 0;

        for (auto &BB : F) {
            for (auto &I : BB) {
                if (isa<BinaryOperator>(&I)) {
                    arithmeticOps++;
                } else if (auto *CI = dyn_cast<CallInst>(&I)) {
                    Function *called = CI->getCalledFunction();
                    // if (called && called->getName().startswith("llvm.")) {
                    //     if (called->getName().contains("sin") ||
                    //         called->getName().contains("cos") ||
                    //         called->getName().contains("exp") ||
                    //         called->getName().contains("sqrt")) {
                    //         arithmeticOps++;
                    //     }
                    // }
                    if (called) {
                        StringRef name = called->getName();
                        if (name.contains("sin") || name.contains("cos") ||
                            name.contains("exp") || name.contains("sqrt")) {
                            arithmeticOps++;
                        }
                    }
                } else if (isa<LoadInst>(&I) || isa<StoreInst>(&I)) {
                    memoryOps++;
                }
            }
        }

        float ratio = memoryOps > 0 ? (float)arithmeticOps / memoryOps : arithmeticOps;

        if (ratio > 2.0) {
            errs() << "Function '" << F.getName() << "' has high computational intensity:\n";
        }

        errs() << "- Arithmetic ops: " << arithmeticOps << "\n";
        errs() << "- Memory ops: " << memoryOps << "\n";
        errs() << "- Ratio: " << ratio << "\n\n";

        return PreservedAnalyses::all();
    }
};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "ComputationalIntensityPass",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    errs() << ">>> Pipeline callback called with pass name: " << Name << "\n";
                    if (Name == "analyze-computational-intensity") {
                        errs() << ">>> Match found! Adding ComputationalIntensityPass\n";
                        FPM.addPass(ComputationalIntensityPass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}



