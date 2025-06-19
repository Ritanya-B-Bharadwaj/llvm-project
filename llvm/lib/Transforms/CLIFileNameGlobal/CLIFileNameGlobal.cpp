#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Constants.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "CLIFileNameGlobal.h"

using namespace llvm;

PreservedAnalyses CLIFileNameGlobal::run(Module &M, ModuleAnalysisManager &) {
  errs() << "=== Pass executed on: " << M.getName() << " ===\n";

  StringRef FileName = !M.getSourceFileName().empty() 
                      ? M.getSourceFileName() 
                      : M.getName();

  SmallString<256> BaseName = sys::path::filename(FileName);

  std::string MangledName = "__cli_";
  for (char C : BaseName) {
    MangledName += (isalnum(C) ? C : '_');
  }

  Constant *StrConstant = ConstantDataArray::getString(M.getContext(), FileName, true);
  auto *GV = new GlobalVariable(M,
                              StrConstant->getType(),
                              true,
                              GlobalValue::ExternalLinkage,
                              StrConstant,
                              MangledName);

  GV->setUnnamedAddr(GlobalValue::UnnamedAddr::None);
  GV->setAlignment(Align(1));

  errs() << "Created global: " << MangledName << " = \"" << FileName << "\"\n";
  return PreservedAnalyses::none();
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "CLIFileNameGlobal",
    "v1.0",
    [](PassBuilder &PB) {
      PB.registerPipelineStartEPCallback(
        [](ModulePassManager &MPM, OptimizationLevel) {
          MPM.addPass(CLIFileNameGlobal());
        });
      PB.registerPipelineParsingCallback(
        [](StringRef Name, ModulePassManager &MPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "cli-filename-global") {
            MPM.addPass(CLIFileNameGlobal());
            return true;
          }
          return false;
        });
    }
  };
}
