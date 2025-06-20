#include "Utils.h"
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Module.h>
#include <sstream>

using namespace llvm;

namespace MPIReductionAnalysis {

void Utils::printVerbose(const std::string& message, bool verbose) {
    if (verbose) {
        errs() << "[VERBOSE] " << message << "\n";
    }
}

void Utils::printAnalysis(const std::string& message) {
    errs() << "[ANALYSIS] " << message << "\n";
}

void Utils::printError(const std::string& message) {
    errs() << "[ERROR] " << message << "\n";
}

bool Utils::isMPIFunction(Function* func, const std::string& mpiCallName) {
    if (!func) return false;
    StringRef funcName = func->getName();
    return funcName == mpiCallName;
}

bool Utils::isLoopCounter(BinaryOperator* binOp) {
    if (!binOp || binOp->getOpcode() != Instruction::Add) return false;

    if (ConstantInt* CI = dyn_cast<ConstantInt>(binOp->getOperand(1))) {
        return CI->isOne();
    }

    return false;
}

void Utils::printInstructionDetails(Instruction* inst, int index, const std::string& prefix) {
    if (!inst) return;

    errs() << prefix << "[" << index << "] " << *inst << "\n";

    if (BinaryOperator* BO = dyn_cast<BinaryOperator>(inst)) {
        errs() << prefix << "    -> Binary op: " << BO->getOpcodeName();
        if (isLoopCounter(BO)) {
            errs() << " (likely loop counter)";
        }
        errs() << "\n";
    } else if (SelectInst* SI = dyn_cast<SelectInst>(inst)) {
        errs() << prefix << "    -> Select instruction (potential min/max)\n";
    } else if (ICmpInst* CI = dyn_cast<ICmpInst>(inst)) {
        errs() << prefix << "    -> Integer comparison: " << CI->getPredicateName(CI->getPredicate()) << "\n";
    } else if (FCmpInst* FC = dyn_cast<FCmpInst>(inst)) {
        errs() << prefix << "    -> Float comparison: " << FC->getPredicateName(FC->getPredicate()) << "\n";
    } else if (LoadInst* LI = dyn_cast<LoadInst>(inst)) {
        errs() << prefix << "    -> Load from memory\n";
    } else if (StoreInst* SI = dyn_cast<StoreInst>(inst)) {
        errs() << prefix << "    -> Store to memory\n";
    } else if (CallInst* CI = dyn_cast<CallInst>(inst)) {
        if (Function* F = CI->getCalledFunction()) {
            errs() << prefix << "    -> Function call: " << F->getName() << "\n";
        } else {
            errs() << prefix << "    -> Indirect function call\n";
        }
    }
}

bool Utils::shouldSkipFunction(Function& func) {
    return func.isDeclaration() || func.empty();
}

std::string Utils::getFunctionSignature(Function& func) {
    std::string signature = func.getName().str() + "(";
    bool first = true;
    for (auto& arg : func.args()) {
        if (!first) signature += ", ";
        Type* type = arg.getType();
        if (auto* ptrType = dyn_cast<PointerType>(type)) {
            Type* elemType =ptrType->getNonOpaquePointerElementType();
            if (elemType->isStructTy()) {
                signature += elemType->getStructName().str();
            } else {
                signature += elemType->isPointerTy() ? "ptr" : "other";
            }
        } else {
            signature += "non-pointer";
        }
        first = false;
    }
    signature += ")";
    return signature;
}

void Utils::printModuleInfo(Module& module) {
    errs() << "=== MODULE INFORMATION ===\n";
    errs() << "Module identifier: " << module.getModuleIdentifier() << "\n";
    errs() << "Source filename: " << module.getSourceFileName() << "\n";
    errs() << "Number of functions: " << module.getFunctionList().size() << "\n";
    errs() << "Number of global variables: " << module.global_size() << "\n";
    errs() << "==========================\n";
}

std::string Utils::getSourceLocation(Instruction* inst) {
    if (!inst) return "(unknown)";

    if (const DebugLoc& DL = inst->getDebugLoc()) {
        if (const DILocation* Loc = dyn_cast<DILocation>(DL.get())) {
            std::stringstream ss;
            ss << Loc->getFilename().str() << ":" << Loc->getLine();
            if (Loc->getColumn() != 0) {
                ss << ":" << Loc->getColumn();
            }
            return ss.str();
        }
    }

    return "(no debug info)";
}

ReductionType stringToReductionType(const std::string& typeStr) {
    if (typeStr == "sum") return ReductionType::SUM;
    if (typeStr == "product") return ReductionType::PRODUCT;
    if (typeStr == "min") return ReductionType::MIN;
    if (typeStr == "max") return ReductionType::MAX;
    if (typeStr == "all") return ReductionType::ALL;
    return ReductionType::UNKNOWN;
}

std::string reductionTypeToString(ReductionType type) {
    switch (type) {
        case ReductionType::SUM: return "sum";
        case ReductionType::PRODUCT: return "product";
        case ReductionType::MIN: return "min";
        case ReductionType::MAX: return "max";
        case ReductionType::ALL: return "all";
        default: return "unknown";
    }
}

} // namespace MPIReductionAnalysis
