//===- CLIFileNameGlobal.cpp - Insert global var based on filename -------===//
//
// This LLVM pass inserts a global const char* named using a mangled version
// of the input source filename (e.g., __cli_foo_c for foo.c).
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/CLIFileNameGlobal/CLIFileNameGlobal.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Path.h"
using namespace llvm;

PreservedAnalyses CLIFileNameGlobal::run(Module &M, ModuleAnalysisManager &) {
  StringRef FileName =
      !M.getSourceFileName().empty() ? M.getSourceFileName() : M.getName();

  SmallString<256> BaseName = sys::path::filename(FileName);

  std::string MangledName = "__cli_";
  for (char C : BaseName) {
    MangledName += isalnum(C) ? C : '_';
  }

  Constant *StrConstant =
      ConstantDataArray::getString(M.getContext(), FileName, true);

  auto *GV = new GlobalVariable(M, StrConstant->getType(),
                                true, // isConstant
                                GlobalValue::ExternalLinkage, StrConstant,
                                MangledName);

  GV->setUnnamedAddr(GlobalValue::UnnamedAddr::None);
  GV->setAlignment(Align(1));

  return PreservedAnalyses::none();
}
