#include "llvm/Transforms/Utils/AnalyeMemoryAccessPatterns.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/IntrinsicInst.h" // Crucial for DbgDeclareInst
#include "llvm/IR/Module.h"        // Crucial for iterating through all functions/instructions
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"

#include <map>
#include <string>
#include <set>

#define DEBUG_TYPE "memory-access-analysis"

using namespace llvm;
namespace {

struct MemoryAccessInfo {
    Type* accessedType = nullptr;
    uint64_t sizePerAccess = 0;
    uint64_t frequency = 0;

    void addAccess(Type* type, uint64_t singleAccessSizeInBytes) {
        if (!accessedType) {
            accessedType = type;
            sizePerAccess = singleAccessSizeInBytes;
        }
        frequency++;
    }

    uint64_t getTotalBytesAccessed() const {
        return sizePerAccess * frequency;
    }
};

std::pair<Value*, std::string> getBaseValueAndPath(Value* Ptr, const DataLayout& DL) {
    SmallVector<Value*, 8> Worklist;
    Worklist.push_back(Ptr);
    std::string Path = "";

    std::set<Value*> Visited;

    while (!Worklist.empty()) {
        Value* CurrentPtr = Worklist.back();
        Worklist.pop_back();

        LLVM_DEBUG(errs() << "Tracing: " << *CurrentPtr << " (Type: " << *CurrentPtr->getType() << ")\n");

        if (Visited.count(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Skipping (visited): " << *CurrentPtr << "\n");
            continue;
        }
        Visited.insert(CurrentPtr);

        if (AllocaInst* AI = dyn_cast<AllocaInst>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Found AllocaInst: " << *AI << "\n");
            return {AI, Path};
        } else if (GlobalVariable* GV = dyn_cast<GlobalVariable>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Found GlobalVariable: " << *GV << "\n");
            return {GV, Path};
        } else if (BitCastInst* BC = dyn_cast<BitCastInst>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing BitCast: " << *BC << "\n");
            Worklist.push_back(BC->getOperand(0));
        }
        else if (GetElementPtrInst* GEP = dyn_cast<GetElementPtrInst>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing GEPInst: " << *GEP << "\n");
            Value* Base = GEP->getPointerOperand();
            Type* CurrentType = GEP->getSourceElementType();

            auto IdxBegin = GEP->idx_begin();
            if (isa<PointerType>(GEP->getPointerOperandType()) && IdxBegin != GEP->idx_end() &&
                isa<ConstantInt>(*IdxBegin) && cast<ConstantInt>(*IdxBegin)->isZero()) {
                 ++IdxBegin;
            }

            for (auto It = IdxBegin; It != GEP->idx_end(); ++It) {
                if (StructType* ST = dyn_cast<StructType>(CurrentType)) {
                    if (ConstantInt* CI = dyn_cast<ConstantInt>(*It)) {
                        unsigned Index = CI->getZExtValue();
                        if (Index < ST->getNumElements()) {
                            Path = ".field_" + std::to_string(Index) + Path;
                            CurrentType = ST->getElementType(Index);
                        } else { break; }
                    } else { break; }
                } else if (ArrayType* AT = dyn_cast<ArrayType>(CurrentType)) {
                    CurrentType = AT;
                } else { break; }
            }
            Worklist.push_back(Base);
        }
        else if (ConstantExpr* CE = dyn_cast<ConstantExpr>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing ConstantExpr: " << *CE << " Opcode: " << CE->getOpcodeName() << "\n");
            if (CE->getOpcode() == Instruction::GetElementPtr) {
                Value* Base = CE->getOperand(0);
                Type* CurrentType = CE->getOperand(0)->getType();
                if (PointerType* PT = dyn_cast<PointerType>(CurrentType)) {
                    CurrentType = PT->getElementType();
                } else {
                    Worklist.push_back(CE->getOperand(0));
                    continue;
                }

                for (unsigned i = 1, e = CE->getNumOperands(); i != e; ++i) {
                    if (StructType* ST = dyn_cast<StructType>(CurrentType)) {
                        if (ConstantInt* CI = dyn_cast<ConstantInt>(CE->getOperand(i))) {
                            unsigned Index = CI->getZExtValue();
                            if (Index < ST->getNumElements()) {
                                Path = ".field_" + std::to_string(Index) + Path;
                                CurrentType = ST->getElementType(Index);
                            } else { break; }
                        } else { break; }
                    } else if (ArrayType* AT = dyn_cast<ArrayType>(CurrentType)) {
                        CurrentType = AT;
                    } else { break; }
                }
                Worklist.push_back(Base);
            } else if (CE->getOpcode() == Instruction::BitCast) {
                Worklist.push_back(CE->getOperand(0));
            } else {
                LLVM_DEBUG(errs() << "  Unhandled ConstantExpr type in getBaseValueAndPath, stopping: " << *CE << "\n");
                return {nullptr, ""};
            }
        }
        else if (LoadInst* LI = dyn_cast<LoadInst>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing LoadInst used as pointer: " << *LI << "\n");
            Worklist.push_back(LI->getPointerOperand());
        }
        else if (PHINode* PN = dyn_cast<PHINode>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing PHINode: " << *PN << "\n");
            for (Value* IncomingVal : PN->incoming_values()) {
                std::pair<Value*, std::string> TempInfo = getBaseValueAndPath(IncomingVal, DL);
                if (TempInfo.first) {
                    Path = TempInfo.second;
                    return TempInfo;
                }
            }
            LLVM_DEBUG(errs() << "  PHINode could not trace to base, stopping: " << *PN << "\n");
            return {nullptr, ""};
        } else if (SelectInst* SI = dyn_cast<SelectInst>(CurrentPtr)) {
            LLVM_DEBUG(errs() << "  Tracing SelectInst: " << *SI << "\n");
            std::pair<Value*, std::string> TrueInfo = getBaseValueAndPath(SI->getTrueValue(), DL);
            if (TrueInfo.first) {
                Path = TrueInfo.second;
                return TrueInfo;
            }
            std::pair<Value*, std::string> FalseInfo = getBaseValueAndPath(SI->getFalseValue(), DL);
            if (FalseInfo.first) {
                Path = FalseInfo.second;
                return FalseInfo;
            }
            LLVM_DEBUG(errs() << "  SelectInst could not trace to base, stopping: " << *SI << "\n");
            return {nullptr, ""};
        } else {
            LLVM_DEBUG(errs() << "  UNHANDLED POINTER SOURCE, STOPPING TRACE for: " << *CurrentPtr << " (Type: " << *CurrentPtr->getType() << ")\n");
            return {nullptr, ""};
        }
    }
    LLVM_DEBUG(errs() << "Worklist empty, no base found. Returning nullptr.\n");
    return {nullptr, ""};
}

std::string getSourceVariableName(const Value* V) {
    if (!V) return "unknown";

    if (const AllocaInst* AI = dyn_cast<AllocaInst>(V)) {
        LLVM_DEBUG(errs() << "AllocaInst being checked: " << *AI << "\n");

        if (AI->getFunction()) { 
            const Module* M = AI->getFunction()->getParent();
            if(M){
                for (const Function &F : *M) {
                    for (const Instruction &I : instructions(F)) {
                        if (const DbgDeclareInst* DDI = dyn_cast<DbgDeclareInst>(&I)) {
                            LLVM_DEBUG(errs() << "  Found DbgDeclareInst: " << *DDI << "\n");
                            if (DDI->getAddress() == AI) { 
                                LLVM_DEBUG(errs() << "  DbgDeclareInst matches AllocaInst! \n");
                                if (DILocalVariable* DILV = DDI->getVariable()) {
                                    return DILV->getName().str();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // Try to get the variable name if it's a GlobalVariable.
    else if (const GlobalVariable* GV = dyn_cast<GlobalVariable>(V)) {
        LLVM_DEBUG(errs() << "GlobalVariable being checked: " << *GV << "\n");
        SmallVector<DIGlobalVariableExpression*, 1> GVEs;
        GV->getDebugInfo(GVEs);
        if (!GVEs.empty()) {
            return GVEs[0]->getVariable()->getName().str();
        }
        return GV->getName().str();
    }

    if (V->hasName()) {
        LLVM_DEBUG(errs() << "Falling back to LLVM IR name: " << V->getName().str() << "\n");
        return V->getName().str();
    }
    LLVM_DEBUG(errs() << "Falling back to unnamed_ address: " << reinterpret_cast<uintptr_t>(V) << "\n");
    return "unnamed_" + std::to_string(reinterpret_cast<uintptr_t>(V));
}
}


PreservedAnalyses AnalyzeMemoryAccessPatternsPass::run(Function &F, FunctionAnalysisManager &AM) {

    if (F.isDeclaration()) {
        return PreservedAnalyses::all();
    }

    const DataLayout &DL = F.getParent()->getDataLayout();
    std::map<std::string, MemoryAccessInfo> accessMap;

    for (Instruction &I : instructions(F)) {
        Value* PtrOperand = nullptr;
        Type* AccessType = nullptr;

        if (LoadInst* LI = dyn_cast<LoadInst>(&I)) {
            PtrOperand = LI->getPointerOperand();
            AccessType = LI->getType();
        } else if (StoreInst* SI = dyn_cast<StoreInst>(&I)) {
            PtrOperand = SI->getPointerOperand();
            AccessType = SI->getValueOperand()->getType();
        } else {
            continue;
        }

        if (!PtrOperand) {
            LLVM_DEBUG(errs() << "Warning: Null pointer operand for instruction: " << I << "\n");
            continue;
        }

        std::pair<Value*, std::string> BaseInfo = getBaseValueAndPath(PtrOperand, DL);
        Value* BaseValue = BaseInfo.first;
        std::string FieldPath = BaseInfo.second;

        if (!BaseValue) {
            LLVM_DEBUG(errs() << "Could not trace pointer to a known base variable for instruction: " << I << "\n");
            continue;
        }

        std::string SourceVarName = getSourceVariableName(BaseValue);
        std::string FullVarName = SourceVarName;

        uint64_t SingleAccessSize = 0;
        Type* ReportedType = AccessType; 
        if (PointerType* PT = dyn_cast<PointerType>(BaseValue->getType())) {
            Type* ElementTypeOfBasePointer = PT->getElementType();

            if (ArrayType* ArrayTy = dyn_cast<ArrayType>(ElementTypeOfBasePointer)) {
                SingleAccessSize = DL.getTypeStoreSize(ArrayTy->getElementType());
                ReportedType = ArrayTy;
                FieldPath = ""; 
            } else {
                SingleAccessSize = DL.getTypeStoreSize(AccessType);
            }
        } else {
            SingleAccessSize = DL.getTypeStoreSize(AccessType);
        }

        if (!FieldPath.empty()) {
            FullVarName += FieldPath;
        }

        // Use ReportedType for the MemoryAccessInfo entry
        accessMap[FullVarName].addAccess(ReportedType, SingleAccessSize);
    }

    errs() << "=== Memory Access Report for Function: " << F.getName() << " ===\n";
    for (const auto &entry : accessMap) {
        const std::string &var = entry.first;
        const MemoryAccessInfo &info = entry.second;

        errs() << "  Variable: " << var << "\n";
        errs() << "    Type: ";
        if (info.accessedType) {
            info.accessedType->print(errs());
        } else {
            errs() << "unknown";
        }
        errs() << "\n";
        errs() << "    Access count: " << info.frequency << "\n";
        errs() << "    Size per access: " << info.sizePerAccess << " bytes\n";
        errs() << "    Total bytes accessed: " << info.getTotalBytesAccessed() << " bytes\n";
    }
    errs() << "============================================\n\n";

    return PreservedAnalyses::all();
}