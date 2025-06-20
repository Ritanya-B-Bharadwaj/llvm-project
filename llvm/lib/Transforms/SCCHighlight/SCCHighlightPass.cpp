#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Demangle/Demangle.h"
#include <set>
#include <string>
#include <cxxabi.h>

using namespace llvm;

namespace {

struct SCCHighlightPass : PassInfoMixin<SCCHighlightPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        std::set<Function *> Highlighted;

        // Step 1: Detect IsHighlighted attribute from prior pass
        for (Function &F : M) {
            if (F.hasFnAttribute("IsHighlighted")) {
                Highlighted.insert(&F);
            }
        }

        if (Highlighted.empty()) {
            errs() << "No functions found with IsHighlighted attribute.\n";
            return PreservedAnalyses::all();
        }

        // Step 2: Display formatted table
        errs() << "+----------------------+---------------------------------------------+---------------------------+-----+\n";
        errs() << "| Name                 | Signature                                   | Defined In                | LOC |\n";
        errs() << "+----------------------+---------------------------------------------+---------------------------+-----+\n";

        for (Function *F : Highlighted) {
            if (F->isDeclaration()) continue;

            // Demangle if needed
            std::string NameStr = F->getName().str();
            bool isCPlusPlus = F->getName().starts_with("_Z");
            std::string DemangledName = NameStr;

            if (isCPlusPlus) {
                int Status;
                char *Demangled = abi::__cxa_demangle(NameStr.c_str(), nullptr, nullptr, &Status);
                if (Status == 0 && Demangled) {
                    DemangledName = Demangled;
                    free(Demangled);
                }
            }

            // Build signature
            std::string Sig;
            raw_string_ostream SS(Sig);
            F->getReturnType()->print(SS);
            SS << " " << DemangledName << "(";

            int argIndex = 0;
            bool first = true;
            for (auto &Arg : F->args()) {
                if (!first) SS << ", ";
                Arg.getType()->print(SS);
                SS << " ";
                if (Arg.hasName()) SS << Arg.getName();
                else SS << "arg" << argIndex;
                ++argIndex;
                first = false;
            }
            SS << ")";
            SS.flush();

            std::string SignatureStr = SS.str();
            if (SignatureStr.length() > 45)
                SignatureStr = SignatureStr.substr(0, 42) + "...";

            // Source location
            std::string Location = "unknown";
            if (DISubprogram *SP = F->getSubprogram()) {
                Location = (SP->getFilename() + ":" + std::to_string(SP->getLine())).str();
            }

            // LOC
            int loc = 0;
            for (auto &I : instructions(F)) ++loc;

            errs() << "| " << format("%-20s", DemangledName.c_str())
                   << " | " << format("%-45s", SignatureStr.c_str())
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
        LLVM_PLUGIN_API_VERSION, "SCCHighlightPass", "v1.0",
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
