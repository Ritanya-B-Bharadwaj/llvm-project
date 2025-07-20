#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

struct PointerTrackerPass : PassInfoMixin<PointerTrackerPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    Module *M = F.getParent();
    LLVMContext &Ctx = M->getContext();
    
    // Remove OptimizeNone attribute if present
    if (F.hasFnAttribute(Attribute::OptimizeNone)) {
      F.removeFnAttr(Attribute::OptimizeNone);
    }
    
    // Declare printf if not already declared
    FunctionCallee PrintfFunc = M->getOrInsertFunction(
      "printf", 
      FunctionType::get(
        IntegerType::getInt32Ty(Ctx), 
        PointerType::get(Type::getInt8Ty(Ctx), 0), 
        true
      )
    );
    
    // Collect all pointer operations first
    std::vector<std::pair<Instruction*, Value*>> InstrumentationPoints;
    for (auto &BB : F) {
      for (auto &I : BB) {
        Value *ptrOperand = nullptr;
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
          ptrOperand = LI->getPointerOperand();
        } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
          ptrOperand = SI->getPointerOperand();
        }
        
        if (ptrOperand) {
          InstrumentationPoints.push_back(std::make_pair(&I, ptrOperand));
        }
      }
    }
    
    // For each return instruction, add all the printing logic
    for (auto &BB : F) {
      if (auto *RI = dyn_cast<ReturnInst>(BB.getTerminator())) {
        IRBuilder<> IRB(RI);
        
        // Print function name
        std::string funcStr = F.getName().str() + "(), ";
        Constant *FuncNameFmt = IRB.CreateGlobalStringPtr(funcStr);
        IRB.CreateCall(PrintfFunc, FuncNameFmt);
        
        // Print all pointers for this function
        for (size_t i = 0; i < InstrumentationPoints.size(); ++i) {
          Value *ptrOperand = InstrumentationPoints[i].second;
          
          if (i == InstrumentationPoints.size() - 1) {
            // Last pointer - add newline
            Constant *FmtStr = IRB.CreateGlobalStringPtr("%p\n");
            IRB.CreateCall(PrintfFunc, {FmtStr, ptrOperand});
          } else {
            // Not last pointer - add comma and space
            Constant *FmtStr = IRB.CreateGlobalStringPtr("%p, ");
            IRB.CreateCall(PrintfFunc, {FmtStr, ptrOperand});
          }
        }
        
        // If no pointers, just print newline
        if (InstrumentationPoints.empty()) {
          Constant *Newline = IRB.CreateGlobalStringPtr("\n");
          IRB.CreateCall(PrintfFunc, Newline);
        }
      }
    }
    
    return PreservedAnalyses::none();
  }
};

// Plugin registration
extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "PointerTracker",
    "0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "pointer-tracker") {
            FPM.addPass(PointerTrackerPass());
            return true;
          }
          return false;
        });
    }
  };
}