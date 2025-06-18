#include "llvm/Transforms/OpenMPAnnotator/OpenMPAnnotatorPass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

PreservedAnalyses OpenMPAnnotatorPass::run(Function &F, FunctionAnalysisManager &) {
    bool Modified = false;
    LLVMContext &Ctx = F.getContext();

    errs() << "Running OpenMP Annotator on function: " << F.getName() << "\n";

    for (auto &BB : F) {
        for (auto &I : BB) {
            if (auto *CI = dyn_cast<CallInst>(&I)) {
                Function *Callee = CI->getCalledFunction();
                if (!Callee) continue;

                StringRef CalleeName = Callee->getName();
                StringRef AnnotationKind;

                if (CalleeName.starts_with("__kmpc_fork_call"))
                    AnnotationKind = "omp.parallel";
                else if (CalleeName.starts_with("__kmpc_for_static_init"))
                    AnnotationKind = "omp.for";
                else if (CalleeName.starts_with("__kmpc_critical"))
                    AnnotationKind = "omp.critical";
                else if (CalleeName.starts_with("__kmpc_end_critical"))
                    AnnotationKind = "omp.critical.end";
                else if (CalleeName.starts_with("__kmpc_barrier"))
                    AnnotationKind = "omp.barrier";
                else if (CalleeName.starts_with("__kmpc_master"))
                    AnnotationKind = "omp.master";
                else if (CalleeName.starts_with("__kmpc_end_master"))
                    AnnotationKind = "omp.master.end";
                else if (CalleeName.starts_with("__kmpc_single"))
                    AnnotationKind = "omp.single";
                else if (CalleeName.starts_with("omp_get_thread_num"))
                    AnnotationKind = "omp.get_thread_num";
                else if (CalleeName.starts_with("omp_get_num_threads"))
                    AnnotationKind = "omp.get_num_threads";
                else if (CalleeName.starts_with("__kmpc_") || CalleeName.starts_with("omp_get_"))
                    AnnotationKind = "omp.runtime";
                else
                    continue;

                errs() << "  Annotated: " << CalleeName << " as [" << AnnotationKind << "]\n";

                MDNode *MD = MDNode::get(Ctx, MDString::get(Ctx, AnnotationKind));
                CI->setMetadata("omp.annotation", MD);

                Modified = true;
            }
        }
    }

    return Modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
}


namespace llvm {
void registerOpenMPAnnotatorPipeline(PassBuilder &PB) {
  PB.registerPipelineParsingCallback(
    [](StringRef Name, FunctionPassManager &FPM,
       ArrayRef<PassBuilder::PipelineElement>) {
      if (Name == "openmp-annotator") {
        FPM.addPass(OpenMPAnnotatorPass());
        return true;
      }
      return false;
    });
}
}

