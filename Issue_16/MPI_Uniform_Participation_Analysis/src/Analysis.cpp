#include "Analysis.h"
#include "Data.h"

// LLVM Includes
#include "llvm/IR/Module.h" // <<< THE CRITICAL FIX IS HERE
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfoMetadata.h" // For getting line numbers

#include <map>

// Helper to get source line number if available
unsigned getSourceLine(const llvm::Instruction* I) {
    if (I) {
        const llvm::DebugLoc &loc = I->getDebugLoc();
        if (loc) {
            return loc.getLine();
        }
    }
    return 0;
}

// A global context can be problematic in multithreading, but fine for this CLI tool.
static llvm::LLVMContext GlobalContext;

MPIAnalysis::MPIAnalysis(bool verbose) 
    : context(GlobalContext), verbose_mode(verbose) {}

// ADDED: Define the destructor. It MUST be in the .cpp file where Module is fully defined.
MPIAnalysis::~MPIAnalysis() = default;

bool MPIAnalysis::processFile(const std::string& filePath) {
    llvm::SMDiagnostic err;
    if (verbose_mode) llvm::errs() << "[VERBOSE] Parsing file: " << filePath << "\n";
    
    std::unique_ptr<llvm::Module> mod = llvm::parseIRFile(filePath, err, context);
    if (!mod) {
        err.print("mpi-analyser", llvm::errs());
        return false;
    }

    for (auto& F : *mod) {
        for (auto& BB : F) {
            for (auto& I : BB) {
                if (auto* callInst = llvm::dyn_cast<llvm::CallInst>(&I)) {
                    llvm::Function* calledFunc = callInst->getCalledFunction();
                    if (!calledFunc || !calledFunc->hasName()) continue;

                    llvm::StringRef funcName = calledFunc->getName();
                    if (funcName == "MPI_Send" || funcName == "MPI_Recv") {
                        MPICallInfo info;
                        info.functionName = funcName.str();
                        info.isSend = (funcName == "MPI_Send");
                        info.sourceLine = getSourceLine(&I);

                        if (callInst->arg_size() > 3)
                            if (auto* c = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(3))) info.rank = c->getSExtValue();
                        if (callInst->arg_size() > 4)
                            if (auto* c = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(4))) info.tag = c->getSExtValue();
                        if (callInst->arg_size() > 5 && callInst->getArgOperand(5)->hasName())
                            info.communicatorName = callInst->getArgOperand(5)->getName().str();
                        
                        allCalls.push_back(info);
                    }
                }
            }
        }
    }
    // Store the module to keep it alive
    modules.push_back(std::move(mod));
    return true;
}

void MPIAnalysis::runAnalysis() {
    if (verbose_mode) llvm::errs() << "[VERBOSE] Found " << allCalls.size() << " total MPI calls. Starting analysis...\n";

    // Use a map for efficient grouping: Key = <communicator, tag>, Value = list of calls
    using CommTagKey = std::pair<std::string, long long>;
    std::map<CommTagKey, std::vector<MPICallInfo>> sendsMap;
    std::map<CommTagKey, std::vector<MPICallInfo>> recvsMap;

    for (const auto& call : allCalls) {
        if (call.tag == -1) { // Skip calls with non-constant tags for now
            if (call.isSend) results.unmatchedSends.push_back(call);
            else results.unmatchedReceives.push_back(call);
            continue;
        }
        CommTagKey key = {call.communicatorName, call.tag};
        if (call.isSend) sendsMap[key].push_back(call);
        else recvsMap[key].push_back(call);
    }
    
    // Perform the matching
    for (auto const& [key, sends] : sendsMap) {
        if (recvsMap.count(key)) { // Found a matching key in the receives map
            auto& recvs = recvsMap.at(key);
            size_t match_count = std::min(sends.size(), recvs.size());

            for (size_t i = 0; i < match_count; ++i) {
                results.matchedPairs.push_back({sends[i], recvs[i]});
            }
            // Add any leftovers to unmatched
            for (size_t i = match_count; i < sends.size(); ++i) results.unmatchedSends.push_back(sends[i]);
            for (size_t i = match_count; i < recvs.size(); ++i) results.unmatchedReceives.push_back(recvs[i]);
            
            recvsMap.erase(key); // Remove from recv map to avoid re-processing
        } else { // No matching receives for this send key
            for(const auto& s : sends) results.unmatchedSends.push_back(s);
        }
    }

    // Any keys left in recvsMap are unmatched receives
    for (auto const& [key, recvs] : recvsMap) {
        for(const auto& r : recvs) results.unmatchedReceives.push_back(r);
    }
    results.totalCalls = allCalls.size();
}

const AnalysisResults& MPIAnalysis::getResults() const {
    return results;
}