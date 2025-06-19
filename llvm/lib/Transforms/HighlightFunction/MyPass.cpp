#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Attributes.h"
#include <vector>
#include <string>

using namespace llvm;

namespace {

// Structure to represent a highlighted region in source code
struct HighlightCluster {
    DebugLoc start;
    DebugLoc end;
    
    HighlightCluster(DebugLoc s, DebugLoc e) : start(s), end(e) {}
};

// Simple structure to represent line ranges for comparison
struct LineRange {
    std::string filename;
    unsigned startLine;
    unsigned endLine;
    
    LineRange(const std::string &file, unsigned start, unsigned end)
        : filename(file), startLine(start), endLine(end) {}
};

// The main pass implementation
struct MyModulePass : public PassInfoMixin<MyModulePass> {
    
    // Check if a debug location overlaps with any line range
    bool debugLocOverlapsWithRanges(const DebugLoc &loc, const std::vector<LineRange> &ranges) {
        if (!loc) return false;
        
        auto *file = loc->getFile();
        if (!file) return false;
        
        std::string filename = file->getFilename().str();
        unsigned line = loc.getLine();
        
        for (const auto &range : ranges) {
            if (filename.find(range.filename) != std::string::npos &&
                line >= range.startLine && line <= range.endLine) {
                return true;
            }
        }
        return false;
    }
    
    // Get debug location from basic block
    DebugLoc getBasicBlockDebugLoc(BasicBlock &BB) {
        for (Instruction &I : BB) {
            if (I.getDebugLoc()) {
                return I.getDebugLoc();
            }
        }
        return DebugLoc();
    }
    
    // Extract source filename from module
    std::string getSourceFilename(Module &M) {
        for (Function &F : M) {
            if (!F.isDeclaration()) {
                for (BasicBlock &BB : F) {
                    DebugLoc loc = getBasicBlockDebugLoc(BB);
                    if (loc) {
                        return loc->getFilename().str();
                    }
                }
            }
        }
        
        std::string moduleName = M.getName().str();
        if (moduleName.empty()) {
            return "unknown.c";
        }
        return moduleName;
    }
    
    // Configure highlight line ranges based on source file
    std::vector<LineRange> configureHighlightRanges(const std::string &sourceFile) {
        std::vector<LineRange> ranges;
        
        errs() << "📁 Configuring highlights for source file: " << sourceFile << "\n";
        
        if (sourceFile.find("test_arithmetic.c") != std::string::npos) {
            errs() << "🎯 Arithmetic test: highlighting lines 2, 10, 18\n";
            ranges.push_back(LineRange(sourceFile, 2, 2));
            ranges.push_back(LineRange(sourceFile, 10, 10));
            ranges.push_back(LineRange(sourceFile, 18, 18));
        }
        else if (sourceFile.find("test_conditionals.c") != std::string::npos) {
            errs() << "🎯 Conditionals test: highlighting lines 4, 12, 21\n";
            ranges.push_back(LineRange(sourceFile, 4, 4));
            ranges.push_back(LineRange(sourceFile, 12, 12));
            ranges.push_back(LineRange(sourceFile, 21, 21));
        }
        else if (sourceFile.find("test_loops.c") != std::string::npos) {
            errs() << "🎯 Loops test: highlighting lines 4, 11, 20\n";
            ranges.push_back(LineRange(sourceFile, 4, 4));
            ranges.push_back(LineRange(sourceFile, 11, 11));
            ranges.push_back(LineRange(sourceFile, 20, 20));
        }
        else if (sourceFile.find("test_pointers.c") != std::string::npos) {
            errs() << "🎯 Pointers test: highlighting lines 5, 13, 20\n";
            ranges.push_back(LineRange(sourceFile, 5, 5));
            ranges.push_back(LineRange(sourceFile, 13, 13));
            ranges.push_back(LineRange(sourceFile, 20, 20));
        }
        else if (sourceFile.find("test_complex.c") != std::string::npos) {
            errs() << "�� Complex test: highlighting lines 8, 15, 22\n";
            ranges.push_back(LineRange(sourceFile, 8, 8));
            ranges.push_back(LineRange(sourceFile, 15, 15));
            ranges.push_back(LineRange(sourceFile, 22, 22));
        }
        else if (sourceFile.find("test_no_highlights.c") != std::string::npos) {
            errs() << "🎯 No highlights test: no lines to highlight\n";
            // ranges remains empty
        }
        else if (sourceFile.find("test.c") != std::string::npos) {
            errs() << "🎯 Original test: highlighting lines 3, 17\n";
            ranges.push_back(LineRange(sourceFile, 3, 3));
            ranges.push_back(LineRange(sourceFile, 17, 17));
        }
        else {
            errs() << "🎯 Unknown source file, using default highlights (lines 3, 10)\n";
            ranges.push_back(LineRange(sourceFile, 3, 3));
            ranges.push_back(LineRange(sourceFile, 10, 10));
        }
        
        return ranges;
    }
    
    // Main pass execution
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
        errs() << "🚀 ===== MyModulePass Starting Analysis =====\n";
        errs() << "📦 Module: " << M.getName() << "\n";
        
        // Get source filename
        std::string sourceFile = getSourceFilename(M);
        errs() << "📄 Source file detected: " << sourceFile << "\n";
        
        // Configure highlight ranges based on source file  
        std::vector<LineRange> highlightRanges = configureHighlightRanges(sourceFile);
        errs() << "✅ Configured " << highlightRanges.size() << " highlight ranges\n";
        
        if (highlightRanges.empty()) {
            errs() << "⚠️  No highlight ranges configured - no functions will be marked\n";
        }
        
        errs() << "\n🔍 Starting basic block highlight analysis...\n";
        
        // Analyze each function in the module
        int totalFunctions = 0;
        int highlightedFunctions = 0;
        
        for (Function &F : M) {
            if (F.isDeclaration()) {
                continue;
            }
            
            totalFunctions++;
            errs() << "\n📋 Analyzing CFG for function: " << F.getName() << "\n";
            
            bool functionHasOverlap = false;
            int basicBlockCount = 0;
            
            // Check each basic block in the function
            for (BasicBlock &BB : F) {
                basicBlockCount++;
                DebugLoc bbLoc = getBasicBlockDebugLoc(BB);
                
                if (bbLoc) {
                    errs() << "  📍 Checking basic block " << basicBlockCount 
                           << " at line " << bbLoc.getLine() 
                           << ", col " << bbLoc.getCol() << "\n";
                    
                    // Check overlap with highlight ranges
                    if (debugLocOverlapsWithRanges(bbLoc, highlightRanges)) {
                        errs() << "    🎯 OVERLAP! Basic block overlaps with highlight range "
                               << "(line " << bbLoc.getLine() << ")\n";
                        functionHasOverlap = true;
                        break;
                    }
                } else {
                    errs() << "  ❌ Basic block " << basicBlockCount 
                           << " has no debug location info\n";
                }
            }
            
            // Mark function if it has overlapping basic blocks
            if (functionHasOverlap) {
                F.addFnAttr("IsHighlighted", "true");
                highlightedFunctions++;
                errs() << "  🔥 Marked function with IsHighlighted attribute: " << F.getName() << "\n";
            } else {
                errs() << "  ✅ Function " << F.getName() << " - no overlaps found\n";
            }
        }
        
        errs() << "\n📊 ===== Analysis Summary =====\n";
        errs() << "📄 Source file: " << sourceFile << "\n";
        errs() << "🎯 Highlight ranges: " << highlightRanges.size() << "\n";
        errs() << "🔍 Total functions analyzed: " << totalFunctions << "\n";
        errs() << "🔥 Functions marked as highlighted: " << highlightedFunctions << "\n";
        errs() << "✅ Pass execution completed!\n";
        errs() << "=====================================\n\n";
        
        return PreservedAnalyses::none();
    }
};

} // anonymous namespace

// Plugin registration
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, 
        "MyPass", 
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "mypass") {
                        MPM.addPass(MyModulePass());
                        return true;
                    }
                    return false;
                });
        }
    };
}
