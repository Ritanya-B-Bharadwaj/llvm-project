#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <vector>
#include <string>

using namespace llvm;

namespace {

class PointerTracerPass : public PassInfoMixin<PointerTracerPass> {
private:
    // Function declarations for runtime printing
    FunctionCallee PrintFunctionName;
    FunctionCallee PrintPointer;
    FunctionCallee FlushOutput;

    void initializeRuntimeFunctions(Module &M) {
        LLVMContext &Context = M.getContext();
        
        // In LLVM 21, use PointerType::getUnqual() for opaque pointers
        Type *PtrTy = PointerType::getUnqual(Context);
        Type *Int32Ty = Type::getInt32Ty(Context);
        Type *Int64Ty = Type::getInt64Ty(Context);
        
        // printf function: int printf(const char* format, ...)
        FunctionType *PrintfTy = FunctionType::get(Int32Ty, {PtrTy}, true);
        PrintFunctionName = M.getOrInsertFunction("printf", PrintfTy);
        
        // We'll use printf for both function names and pointers
        PrintPointer = PrintFunctionName;
        
        // fflush function: int fflush(FILE* stream)
        FunctionType *FflushTy = FunctionType::get(Int32Ty, {PtrTy}, false);
        FlushOutput = M.getOrInsertFunction("fflush", FflushTy);
    }

    Value* createGlobalString(Module &M, const std::string &str, const std::string &name) {
        LLVMContext &Context = M.getContext();
        
        // Create global string constant
        Constant *StrConstant = ConstantDataArray::getString(Context, str, true);
        GlobalVariable *GV = new GlobalVariable(
            M, StrConstant->getType(), true, GlobalValue::PrivateLinkage,
            StrConstant, name);
        
        // Return pointer to first character using GEP
        Type *Int32Ty = Type::getInt32Ty(Context);
        
        return ConstantExpr::getGetElementPtr(
            StrConstant->getType(), GV,
            ArrayRef<Constant*>{ConstantInt::get(Int32Ty, 0), ConstantInt::get(Int32Ty, 0)});
    }

public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        Module &M = *F.getParent();
        LLVMContext &Context = F.getContext();
        
        // Initialize runtime functions
        initializeRuntimeFunctions(M);
        
        // Skip declaration-only functions
        if (F.isDeclaration()) {
            return PreservedAnalyses::all();
        }
        
        // Skip our own runtime functions to avoid infinite recursion
        if (F.getName() == "printf" || F.getName() == "fflush") {
            return PreservedAnalyses::all();
        }
        
        IRBuilder<> Builder(Context);
        
        // SOLUTION 1: Include newline at the beginning of function name format
        Value *FuncNameFormat = createGlobalString(M, "\n" + F.getName().str() + "(),", "func_name_fmt");
        Value *PointerFormat = createGlobalString(M, " 0x%lx,", "ptr_fmt");
        Value *NewlineFormat = createGlobalString(M, "\n", "newline_fmt");
        
        // Get entry basic block
        BasicBlock &EntryBB = F.getEntryBlock();
        Builder.SetInsertPoint(&EntryBB, EntryBB.getFirstInsertionPt());
        
        // Print function name at the beginning (now includes leading newline)
        Builder.CreateCall(PrintFunctionName, {FuncNameFormat});
        
        // Collect all pointer access instructions
        std::vector<Instruction*> PointerInsts;
        
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *LI = dyn_cast<LoadInst>(&I)) {
                    // Load instruction - accessing memory through pointer
                    PointerInsts.push_back(LI);
                } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
                    // Store instruction - accessing memory through pointer
                    PointerInsts.push_back(SI);
                } else if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    // GetElementPtr - pointer arithmetic
                    PointerInsts.push_back(GEP);
                }
            }
        }
        
        // Instrument pointer accesses
        for (Instruction *Inst : PointerInsts) {
            Value *PtrValue = nullptr;
            
            if (auto *LI = dyn_cast<LoadInst>(Inst)) {
                PtrValue = LI->getPointerOperand();
            } else if (auto *SI = dyn_cast<StoreInst>(Inst)) {
                PtrValue = SI->getPointerOperand();
            } else if (auto *GEP = dyn_cast<GetElementPtrInst>(Inst)) {
                PtrValue = GEP;  // GEP itself produces a pointer
            }
            
            if (PtrValue) {
                // Insert instrumentation after the instruction
                Builder.SetInsertPoint(Inst->getNextNode());
                
                // Cast pointer to i64 for printing
                Type *Int64Ty = Type::getInt64Ty(Context);
                Value *PtrAsInt = Builder.CreatePtrToInt(PtrValue, Int64Ty);
                
                // Print the pointer value
                Builder.CreateCall(PrintPointer, {PointerFormat, PtrAsInt});
            }
        }
        
        // Add flush at function exit points (newline already printed at function entry)
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *RI = dyn_cast<ReturnInst>(&I)) {
                    Builder.SetInsertPoint(RI);
                    
                    // Flush output to ensure immediate printing
                    Value *NullPtr = ConstantPointerNull::get(PointerType::getUnqual(Context));
                    Builder.CreateCall(FlushOutput, {NullPtr});
                }
            }
        }
        
        return PreservedAnalyses::none();
    }
    
    static bool isRequired() { return true; }
};

} // end anonymous namespace

// Pass registration for new pass manager
llvm::PassPluginLibraryInfo getPointerTracerPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "PointerTracerPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "pointer-tracer") {
                        FPM.addPass(PointerTracerPass());
                        return true;
                    }
                    return false;
                });
        }};
}

// This is the core interface for pass plugins
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getPointerTracerPassPluginInfo();
}
