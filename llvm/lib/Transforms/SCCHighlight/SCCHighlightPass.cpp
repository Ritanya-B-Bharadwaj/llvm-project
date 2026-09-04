#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/Support/Format.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/InstIterator.h"
#include <unordered_set>
#include <set>
#include <string>
#include <cxxabi.h>

using namespace llvm;

namespace {

struct SCCHighlightPass : PassInfoMixin<SCCHighlightPass> {
    std::set<Function*> Highlighted;

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        CallGraph CG(M);

        // Traverse SCCs
        for (scc_iterator<CallGraph*> SCCI = scc_begin(&CG); !SCCI.isAtEnd(); ++SCCI) {
            const std::vector<CallGraphNode*> &SCC = *SCCI;
            bool anyHighlighted = false;

            // Check if any function in the SCC is highlighted
            for (CallGraphNode *Node : SCC) {
                Function *F = Node->getFunction();
                if (!F || F->isDeclaration()) continue;
                if (F->hasFnAttribute("IsHighlighted"))
                    anyHighlighted = true;
            }

            // Propagate highlight
            if (anyHighlighted) {
                for (CallGraphNode *Node : SCC) {
                    Function *F = Node->getFunction();
                    if (!F || F->isDeclaration()) continue;
                    F->addFnAttr("IsHighlighted");
                    Highlighted.insert(F);
                }
            }
        }

        // Print results
        if (Highlighted.empty()) {
            errs() << "No functions with IsHighlighted.\n";
            return PreservedAnalyses::all();
        }

        errs() << "+----------------------+---------------------------------------------+---------------------------+-----+\n";
        errs() << "| Name                 | Signature                                   | Defined In                | LOC |\n";
        errs() << "+----------------------+---------------------------------------------+---------------------------+-----+\n";

        for (Function *F : Highlighted) {
            if (F->isDeclaration()) continue;

            std::string NameStr = F->getName().str();
            std::string DemangledName = NameStr;
            if (NameStr.size() >= 2 && NameStr[0] == '_' && NameStr[1] == 'Z') {
                int Status;
                char *Demangled = abi::__cxa_demangle(NameStr.c_str(), nullptr, nullptr, &Status);
                if (Status == 0 && Demangled) {
                    DemangledName = Demangled;
                    free(Demangled);
                }
            }

            std::string SigStr;
            raw_string_ostream SS(SigStr);
            F->getReturnType()->print(SS);
            SS << " " << DemangledName << "(";
            int i = 0;
            for (auto &Arg : F->args()) {
                if (i++) SS << ", ";
                Arg.getType()->print(SS);
            }
            SS << ")";
            SS.flush();
            if (SigStr.length() > 45) SigStr = SigStr.substr(0, 42) + "...";

            std::string Location = "unknown";
            if (DISubprogram *SP = F->getSubprogram())
                Location = (SP->getFilename() + ":" + std::to_string(SP->getLine())).str();

            int loc = 0;
            for (auto &I : instructions(*F)) ++loc;

            errs() << "| " << format("%-20s", DemangledName.c_str())
                   << " | " << format("%-45s", SigStr.c_str())
                   << " | " << format("%-25s", Location.c_str())
                   << " | " << format("%3d", loc) << " |\n";
        }

        errs() << "+----------------------+---------------------------------------------+---------------------------+-----+\n";

        return PreservedAnalyses::all();
    }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "SCCHighlightPass", "v3.0",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "highlight-scc") {
                        MPM.addPass(SCCHighlightPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}
