#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "PassListTracer", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            auto *PIC = PB.getPassInstrumentationCallbacks();
            if (PIC) {
                PIC->registerBeforeNonSkippedPassCallback(
                    [](StringRef PassID, Any IR) {
                         if (PassID.contains("Opt") || PassID.contains("Combine") || PassID.contains("DCE") ||
            PassID.contains("Simplify") || PassID.contains("Vector") || PassID.contains("Unroll") ||
            PassID.contains("Inlining") || PassID.contains("Promote") || PassID.contains("GVN") ||
            PassID.contains("Reassociate") || PassID.contains("SROA") || PassID.contains("MemCpy") ||
            PassID.contains("BDCE") || PassID.contains("Speculative") || PassID.contains("Loop") ||
            PassID.contains("Correlated") || PassID.contains("ConstraintElimination") ||
            PassID.contains("DivRemPairs") || PassID.contains("CallSiteSplitting") ||
            PassID.contains("Float2Int") || PassID.contains("ADCE")) {
            outs() << "Optimization pass: " << PassID << "\n";
        }
                    });
            }
        }
    };
}


