//===- InstructionFrequencyAnalysis.h - Instruction Frequency Analysis ---===//
// This file defines the InstructionFrequencyAnalysis pass that analyzes
// instruction frequencies in LLVM IR and emits frequency tables.

#ifndef LLVM_ANALYSIS_INSTRUCTIONFREQUENCYANALYSIS_H
#define LLVM_ANALYSIS_INSTRUCTIONFREQUENCYANALYSIS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>
#include <vector>
#include "llvm/IR/Instruction.h"         

namespace llvm {

class Function;
class Module;

enum class InstructionCategory {
  Arithmetic,      // add, sub, mul, div, rem
  Logical,         // and, or, xor, shl, lshr, ashr
  Memory,          // load, store, alloca, getelementptr
  Control,         // br, switch, ret, unreachable
  FunctionCall,    // call, invoke
  Comparison,      // icmp, fcmp
  Conversion,      // trunc, zext, sext, bitcast, etc.
  Vector,          // vector operations
  Atomic,          // atomic operations
  Other            // everything else
};

struct FunctionFrequencyInfo {
  std::string FunctionName;
  std::map<InstructionCategory, unsigned> FrequencyTable;
  unsigned TotalInstructions = 0;

  void addInstruction(InstructionCategory Category) {
    FrequencyTable[Category]++;
    TotalInstructions++;
  }
};

class InstructionFrequencyAnalysis : public AnalysisInfoMixin<InstructionFrequencyAnalysis> {
  friend AnalysisInfoMixin<InstructionFrequencyAnalysis>;

public:
  using Result = std::map<std::string, FunctionFrequencyInfo>;

  Result run(Module &M, ModuleAnalysisManager &MAM);
  InstructionCategory classifyInstruction(const Instruction &I);
  std::string getCategoryName(InstructionCategory Category);

private:
  static AnalysisKey Key;
};

class InstructionFrequencyAnalysisWrapperPass : public ModulePass {
public:
  static char ID;

  InstructionFrequencyAnalysisWrapperPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  void emitFrequencyFile(const std::string &SourceFile,
                        const InstructionFrequencyAnalysis::Result &Result);
  std::string getSourceFileName(Module &M);
};

void initializeInstructionFrequencyAnalysisWrapperPassPass(PassRegistry &);

class InstructionFrequencyPrinterPass : public PassInfoMixin<InstructionFrequencyPrinterPass> {

public:
  InstructionFrequencyPrinterPass() = default;

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} 

#endif 
