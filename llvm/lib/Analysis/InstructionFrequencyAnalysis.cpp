//===- InstructionFrequencyAnalysis.cpp - Instruction Frequency Analysis -===//

#include "llvm/Analysis/InstructionFrequencyAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Format.h"
#include <fstream>

#define DEBUG_TYPE "instruction-frequency"

using namespace llvm;

AnalysisKey InstructionFrequencyAnalysis::Key;

char InstructionFrequencyAnalysisWrapperPass::ID = 0;

INITIALIZE_PASS(InstructionFrequencyAnalysisWrapperPass, "instr-freq",
                "Instruction Frequency Analysis", false, true)

InstructionFrequencyAnalysis::Result
InstructionFrequencyAnalysis::run(Module &M, ModuleAnalysisManager &MAM) {
  Result FunctionResults;

  for (Function &F : M) {
    if (F.isDeclaration())
      continue;

    FunctionFrequencyInfo Info;
    Info.FunctionName = F.getName().str();

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        InstructionCategory Category = classifyInstruction(I);
        Info.addInstruction(Category);
      }
    }

    FunctionResults[F.getName().str()] = std::move(Info);
  }

  return FunctionResults;
}

InstructionCategory
InstructionFrequencyAnalysis::classifyInstruction(const Instruction &I) {
  switch (I.getOpcode()) {
    case Instruction::Add:
    case Instruction::FAdd:
    case Instruction::Sub:
    case Instruction::FSub:
    case Instruction::Mul:
    case Instruction::FMul:
    case Instruction::UDiv:
    case Instruction::SDiv:
    case Instruction::FDiv:
    case Instruction::URem:
    case Instruction::SRem:
    case Instruction::FRem:
      return InstructionCategory::Arithmetic;

    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr:
      return InstructionCategory::Logical;

    case Instruction::Load:
    case Instruction::Store:
    case Instruction::Alloca:
    case Instruction::GetElementPtr:
    case Instruction::Fence:
      return InstructionCategory::Memory;

    case Instruction::Br:
    case Instruction::Switch:
    case Instruction::IndirectBr:
    case Instruction::Ret:
    case Instruction::Unreachable:
      return InstructionCategory::Control;

    case Instruction::Call:
    case Instruction::Invoke:
      return InstructionCategory::FunctionCall;

    case Instruction::ICmp:
    case Instruction::FCmp:
      return InstructionCategory::Comparison;

    case Instruction::Trunc:
    case Instruction::ZExt:
    case Instruction::SExt:
    case Instruction::FPToUI:
    case Instruction::FPToSI:
    case Instruction::UIToFP:
    case Instruction::SIToFP:
    case Instruction::FPTrunc:
    case Instruction::FPExt:
    case Instruction::PtrToInt:
    case Instruction::IntToPtr:
    case Instruction::BitCast:
    case Instruction::AddrSpaceCast:
      return InstructionCategory::Conversion;

    case Instruction::ExtractElement:
    case Instruction::InsertElement:
    case Instruction::ShuffleVector:
    case Instruction::ExtractValue:
    case Instruction::InsertValue:
      return InstructionCategory::Vector;

    case Instruction::AtomicCmpXchg:
    case Instruction::AtomicRMW:
      return InstructionCategory::Atomic;

    case Instruction::Resume:
      return InstructionCategory::Other;

    default:
      return InstructionCategory::Other;
  }
}

std::string
InstructionFrequencyAnalysis::getCategoryName(InstructionCategory Category) {
  switch (Category) {
    case InstructionCategory::Arithmetic: return "Arithmetic";
    case InstructionCategory::Logical: return "Logical";
    case InstructionCategory::Memory: return "Memory";
    case InstructionCategory::Control: return "Control Flow";
    case InstructionCategory::FunctionCall: return "Function Call";
    case InstructionCategory::Comparison: return "Comparison";
    case InstructionCategory::Conversion: return "Conversion";
    case InstructionCategory::Vector: return "Vector";
    case InstructionCategory::Atomic: return "Atomic";
    case InstructionCategory::Other: return "Other";
  }
  return "Unknown";
}

bool InstructionFrequencyAnalysisWrapperPass::runOnModule(Module &M) {
  InstructionFrequencyAnalysis Analysis;
  ModuleAnalysisManager DummyMAM;

  auto Result = Analysis.run(M, DummyMAM);
  std::string SourceFile = getSourceFileName(M);

  if (!SourceFile.empty()) {
    emitFrequencyFile(SourceFile, Result);
  }

  return false;
}

void InstructionFrequencyAnalysisWrapperPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

std::string InstructionFrequencyAnalysisWrapperPass::getSourceFileName(Module &M) {
  if (auto *SourceFileMD = M.getModuleFlag("source_filename")) {
    if (auto *MDStr = dyn_cast<MDString>(SourceFileMD)) {
      return MDStr->getString().str();
    }
  }

  StringRef ModuleID = M.getModuleIdentifier();
  if (!ModuleID.empty()) {
    return ModuleID.str();
  }

  return "unknown";
}

void InstructionFrequencyAnalysisWrapperPass::emitFrequencyFile(
    const std::string &SourceFile,
    const InstructionFrequencyAnalysis::Result &Result) {

  SmallString<256> OutputPath(SourceFile);
  sys::path::replace_extension(OutputPath, ".ic");

  std::error_code EC;
  raw_fd_ostream OS(OutputPath, EC, sys::fs::OF_Text);

  if (EC) {
    errs() << "Error opening output file " << OutputPath << ": " << EC.message() << "\n";
    return;
  }

  OS << "Function,Arithmetic,Logical,Comparison,Memory,Control Flow,Function Call\n";

  std::vector<InstructionCategory> CategoryOrder = {
    InstructionCategory::Arithmetic,
    InstructionCategory::Logical,
    InstructionCategory::Comparison,
    InstructionCategory::Memory,
    InstructionCategory::Control,
    InstructionCategory::FunctionCall
  };

  for (const auto &FuncPair : Result) {
    const auto &Info = FuncPair.second;
    
    OS << Info.FunctionName;
    
    for (InstructionCategory Category : CategoryOrder) {
      auto it = Info.FrequencyTable.find(Category);
      unsigned Count = (it != Info.FrequencyTable.end()) ? it->second : 0;
      OS << "," << Count;
    }
    
    OS << "\n";
  }

  OS.close();
  outs() << "Instruction frequency analysis written to: " << OutputPath << "\n";
}

PreservedAnalyses
InstructionFrequencyPrinterPass::run(Module &M, ModuleAnalysisManager &MAM) {
  auto &Result = MAM.getResult<InstructionFrequencyAnalysis>(M);

  std::string SourceFile = M.getSourceFileName();
  if (SourceFile.empty())
    SourceFile = M.getModuleIdentifier();

  llvm::SmallString<256> OutputPath(SourceFile);
  llvm::sys::path::replace_extension(OutputPath, ".ic");

  std::error_code EC;
  llvm::raw_fd_ostream OS(OutputPath, EC, llvm::sys::fs::OF_Text);
  if (EC) {
    llvm::errs() << "Error opening output file " << OutputPath << ": " << EC.message() << "\n";
    return PreservedAnalyses::all();
  }

  OS << "Function,Arithmetic,Logical,Comparison,Memory,Control Flow,Function Call\n";

  std::vector<InstructionCategory> CategoryOrder = {
    InstructionCategory::Arithmetic,
    InstructionCategory::Logical,
    InstructionCategory::Comparison,
    InstructionCategory::Memory,
    InstructionCategory::Control,
    InstructionCategory::FunctionCall
  };

  for (const auto &FuncPair : Result) {
    const auto &Info = FuncPair.second;
    
    OS << Info.FunctionName;
    
    for (InstructionCategory Category : CategoryOrder) {
      auto it = Info.FrequencyTable.find(Category);
      unsigned Count = (it != Info.FrequencyTable.end()) ? it->second : 0;
      OS << "," << Count;
    }
    
    OS << "\n";
  }

  OS.close();
  llvm::outs() << "Instruction frequency analysis written to: " << OutputPath << "\n";

  return PreservedAnalyses::all();
}
