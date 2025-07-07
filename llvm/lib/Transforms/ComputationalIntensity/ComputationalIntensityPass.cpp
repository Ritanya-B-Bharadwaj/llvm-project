#include "llvm/Transforms/ComputationalIntensity/ComputationalIntensityPass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Define run() method from the header
PreservedAnalyses ComputationalIntensityPass::run(Function &F, FunctionAnalysisManager &) {
    
    errs() << "\n";
    errs() << "══════════════════════════════════════════════════════════════\n";
    errs() << "Analyzing Function: " << F.getName() << "       \n";
    errs() << "══════════════════════════════════════════════════════════════\n";



    int arithmeticOps = 0;
    int memoryOps = 0;

    for (auto &BB : F) {
        for (auto &I : BB) {
            
            if (isa<BinaryOperator>(&I)) {
                arithmeticOps++;
            } else if (auto *CI = dyn_cast<CallInst>(&I)) {
                Function *called = CI->getCalledFunction();
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
        errs() << "Function '" << F.getName() << "\n' has HIGH Computational Intensity  \n";
    } else {
        errs() << "Function '" << F.getName() << "' has LOW Computational Intensity   \n";
    }

 
    errs() << "══════════════════════════════════════════════════════════════\n";
    errs() << " Arithmetic Ops : " << arithmeticOps << "       \n";
    errs() << " Memory Ops     : " << memoryOps << "                                  \n";
    errs() << " Intensity Ratio: "<< format("%.6e", ratio) << "\n";
    errs() << "══════════════════════════════════════════════════════════════\n\n";

    return PreservedAnalyses::all();
}

// Plugin registration (for opt -passes=... use)
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "ComputationalIntensityPass",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "analyze-computational-intensity") {
                        FPM.addPass(ComputationalIntensityPass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}

// Static registration (for -analyze-computational-intensity flag)
extern "C" void registerComputationalIntensityPass(llvm::PassBuilder &PB) {
    PB.registerPipelineParsingCallback(
        [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
           llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
            if (Name == "analyze-computational-intensity") {
                FPM.addPass(ComputationalIntensityPass());
                return true;
            }
            return false;
        });
}

