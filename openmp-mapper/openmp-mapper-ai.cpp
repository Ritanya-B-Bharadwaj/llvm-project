#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "clang/AST/StmtOpenMP.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <unordered_map>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Command line options
static cl::OptionCategory OpenMPMapperCategory("AI-Enhanced OpenMP Source-to-IR Mapper");
static cl::opt<std::string> OutputFile("o", cl::desc("Output file for annotated IR"),
                                       cl::value_desc("filename"), cl::cat(OpenMPMapperCategory));
static cl::opt<bool> VerboseDescriptions("verbose", cl::desc("Include detailed AI-generated descriptions"),
                                        cl::init(false), cl::cat(OpenMPMapperCategory));
static cl::opt<bool> ExplainTransformations("explain", cl::desc("Explain IR transformations"),
                                           cl::init(true), cl::cat(OpenMPMapperCategory));

// Built-in AI-Generated Description Database
class OpenMPAIDatabase {
private:
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> irTransformations;
    std::unordered_map<std::string, std::vector<std::string>> runtimeCalls;
    std::unordered_map<std::string, std::string> purposes;

public:
    OpenMPAIDatabase() { initializeDatabase(); }

    std::string getDescription(const std::string& directive) const {
        auto it = descriptions.find(directive);
        return (it != descriptions.end()) ? it->second : "OpenMP directive: " + directive;
    }

    std::string getIRTransformation(const std::string& directive) const {
        auto it = irTransformations.find(directive);
        return (it != irTransformations.end()) ? it->second : "Transforms to OpenMP runtime calls";
    }

    std::vector<std::string> getRuntimeCalls(const std::string& directive) const {
        auto it = runtimeCalls.find(directive);
        return (it != runtimeCalls.end()) ? it->second : std::vector<std::string>();
    }

    std::string explainRuntimeCall(const std::string& call) const {
        auto it = purposes.find(call);
        return (it != purposes.end()) ? it->second : "OpenMP runtime function";
    }

private:
    void initializeDatabase() {
        // Parallel constructs
        descriptions["parallel"] = "Creates a team of threads that execute the enclosed code block concurrently";
        irTransformations["parallel"] = "Compiler generates outlined function and calls __kmpc_fork_call() to spawn threads";
        runtimeCalls["parallel"] = {"__kmpc_fork_call", "__kmpc_global_thread_num"};

        descriptions["parallel for"] = "Combines parallel thread creation with work-sharing loop distribution";
        irTransformations["parallel for"] = "Generates both parallel region setup and loop scheduling calls";
        runtimeCalls["parallel for"] = {"__kmpc_fork_call", "__kmpc_for_static_init", "__kmpc_barrier"};

        descriptions["for"] = "Distributes loop iterations among threads in existing parallel region";
        irTransformations["for"] = "Inserts scheduling runtime calls to divide iterations among threads";
        runtimeCalls["for"] = {"__kmpc_for_static_init", "__kmpc_for_static_fini"};

        descriptions["sections"] = "Divides work into discrete sections executed by different threads";
        irTransformations["sections"] = "Creates dispatch mechanism using __kmpc_sections_init";
        runtimeCalls["sections"] = {"__kmpc_sections_init", "__kmpc_sections_next"};

        descriptions["single"] = "Ensures code block executed by only one thread";
        irTransformations["single"] = "Generates conditional check using __kmpc_single";
        runtimeCalls["single"] = {"__kmpc_single", "__kmpc_end_single"};

        descriptions["critical"] = "Creates mutually exclusive code section for thread-safe access";
        irTransformations["critical"] = "Generates lock acquisition/release calls using __kmpc_critical";
        runtimeCalls["critical"] = {"__kmpc_critical", "__kmpc_end_critical"};

        descriptions["barrier"] = "Synchronization point where all threads must arrive before proceeding";
        irTransformations["barrier"] = "Inserts __kmpc_barrier call with barrier algorithm implementation";
        runtimeCalls["barrier"] = {"__kmpc_barrier"};

        descriptions["task"] = "Creates independent work unit for asynchronous execution";
        irTransformations["task"] = "Packages task code and calls __kmpc_omp_task_alloc";
        runtimeCalls["task"] = {"__kmpc_omp_task_alloc", "__kmpc_omp_task"};

        descriptions["taskwait"] = "Waits for completion of all child tasks";
        irTransformations["taskwait"] = "Generates __kmpc_omp_taskwait call";
        runtimeCalls["taskwait"] = {"__kmpc_omp_taskwait"};

        descriptions["atomic"] = "Provides atomic memory access operations";
        irTransformations["atomic"] = "Generates hardware atomic instructions or mutex implementations";
        runtimeCalls["atomic"] = {"__kmpc_atomic_start", "__kmpc_atomic_end"};

        // Runtime call purposes
        purposes["__kmpc_fork_call"] = "Thread team creation - spawns worker threads for parallel execution";
        purposes["__kmpc_for_static_init"] = "Static loop scheduling - divides iterations among threads";
        purposes["__kmpc_for_static_fini"] = "Loop finalization - cleanup after static loop execution";
        purposes["__kmpc_barrier"] = "Thread synchronization - ensures all threads reach this point";
        purposes["__kmpc_critical"] = "Critical section entry - ensures mutual exclusion";
        purposes["__kmpc_end_critical"] = "Critical section exit - releases mutual exclusion lock";
        purposes["__kmpc_single"] = "Single thread execution - ensures only one thread executes code";
        purposes["__kmpc_sections_init"] = "Sections initialization - sets up work distribution";
        purposes["__kmpc_omp_task"] = "Task creation - creates asynchronous work unit";
        purposes["__kmpc_omp_taskwait"] = "Task synchronization - waits for child task completion";
        purposes["__kmpc_atomic"] = "Atomic operation - ensures thread-safe memory access";
    }
};

// Enhanced OpenMP directive structure
struct AIEnhancedDirective {
    unsigned LineNumber;
    std::string DirectiveType;
    std::string AIDescription;
    std::string IRExplanation;
    std::vector<std::string> ExpectedRuntimeCalls;
    std::vector<std::string> Clauses;
};

// Enhanced AST Visitor
class AIEnhancedOpenMPVisitor : public RecursiveASTVisitor<AIEnhancedOpenMPVisitor> {
private:
    ASTContext *Context;
    SourceManager *SM;
    std::vector<AIEnhancedDirective> Directives;
    OpenMPAIDatabase AIDatabase;

public:
    explicit AIEnhancedOpenMPVisitor(ASTContext *Context) : Context(Context), SM(&Context->getSourceManager()) {}

    bool VisitOMPExecutableDirective(OMPExecutableDirective *Directive) {
        SourceLocation Loc = Directive->getBeginLoc();
        if (!SM->isWrittenInMainFile(Loc)) return true;

        AIEnhancedDirective OMPDir;
        OMPDir.LineNumber = SM->getSpellingLineNumber(Loc);
        OMPDir.DirectiveType = getOpenMPDirectiveName(Directive->getDirectiveKind()).str();
        OMPDir.AIDescription = AIDatabase.getDescription(OMPDir.DirectiveType);
        OMPDir.IRExplanation = AIDatabase.getIRTransformation(OMPDir.DirectiveType);
        OMPDir.ExpectedRuntimeCalls = AIDatabase.getRuntimeCalls(OMPDir.DirectiveType);

        // Extract clauses
        for (auto *Clause : Directive->clauses()) {
            if (Clause) {
                std::string clauseType = getOpenMPClauseName(Clause->getClauseKind()).str();
                OMPDir.Clauses.push_back(clauseType);
            }
        }

        Directives.push_back(OMPDir);
        llvm::outs() << "🔍 Found OpenMP directive: " << OMPDir.DirectiveType << " at line " << OMPDir.LineNumber << "\n";
        
        if (VerboseDescriptions) {
            llvm::outs() << "   📝 " << OMPDir.AIDescription << "\n";
        }
        return true;
    }

    const std::vector<AIEnhancedDirective>& getDirectives() const { return Directives; }
};

// Enhanced AST Consumer
class AIEnhancedOpenMPConsumer : public ASTConsumer {
private:
    AIEnhancedOpenMPVisitor Visitor;
    std::string SourceFile;
    OpenMPAIDatabase AIDatabase;

public:
    explicit AIEnhancedOpenMPConsumer(ASTContext *Context, const std::string &File)
        : Visitor(Context), SourceFile(File) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        generateAIEnhancedIR();
    }

private:
    void generateAIEnhancedIR() {
        std::string ClangPath = getenv("HOME");
        ClangPath += "/Documents/openmp-project/llvm-project/build/bin/clang++";
        
        std::string OutputIR = SourceFile + ".ll";
        std::string CmdLine = ClangPath + " -fopenmp -S -emit-llvm -g -O0";
        std::string HomeDir = getenv("HOME");
        std::string ClangBuiltins = HomeDir + "/Documents/openmp-project/llvm-project/build/lib/clang/21/include";
        CmdLine += " -I" + ClangBuiltins;
        CmdLine += " -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1";
        CmdLine += " -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include";
        CmdLine += " -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk";
        
        // Add OpenMP includes if available
        FILE* brew_fp = popen("brew --prefix libomp 2>/dev/null", "r");
        if (brew_fp != nullptr) {
            char brew_path[1024];
            if (fgets(brew_path, sizeof(brew_path), brew_fp) != nullptr) {
                std::string ompPath = std::string(brew_path);
                if (!ompPath.empty() && ompPath.back() == '\n') {
                    ompPath.pop_back();
                }
                CmdLine += " -I" + ompPath + "/include";
                CmdLine += " -L" + ompPath + "/lib";
            }
            pclose(brew_fp);
        }
        
        CmdLine += " -o " + OutputIR + " " + SourceFile;
        
        llvm::outs() << "🔧 Executing: " << CmdLine << "\n";
        int Result = std::system(CmdLine.c_str());
        
        if (Result == 0) {
            llvm::outs() << "✅ Generated IR: " << OutputIR << "\n";
            analyzeAndCreateAIEnhancedIR(OutputIR);
        } else {
            llvm::errs() << "❌ Failed to generate IR\n";
        }
    }
    
    void analyzeAndCreateAIEnhancedIR(const std::string &IRFile) {
        std::ifstream IRStream(IRFile);
        if (!IRStream) return;
        
        std::string OutputFileName = OutputFile.empty() ? (SourceFile + ".ai-enhanced.ll") : OutputFile;
        std::ofstream OutStream(OutputFileName);
        
        // AI-Enhanced Header
        OutStream << "; ================================================================\n";
        OutStream << ";          🤖 AI-ENHANCED OPENMP SOURCE-TO-IR MAPPER\n";
        OutStream << "; ================================================================\n";
        OutStream << "; Generated from: " << SourceFile << "\n";
        OutStream << "; Analysis includes AI-generated descriptions and explanations\n";
        OutStream << "; ================================================================\n\n";
        
        // AI-Generated Directive Analysis
        OutStream << "; === 🧠 AI-GENERATED OPENMP ANALYSIS ===\n";
        for (const auto &Dir : Visitor.getDirectives()) {
            OutStream << "; ────────────────────────────────────────────────────────────────\n";
            OutStream << "; 📍 DIRECTIVE: #pragma omp " << Dir.DirectiveType << " (Line " << Dir.LineNumber << ")\n";
            OutStream << "; 📝 AI Description: " << Dir.AIDescription << "\n";
            if (ExplainTransformations) {
                OutStream << "; 🔄 IR Transformation: " << Dir.IRExplanation << "\n";
                if (!Dir.ExpectedRuntimeCalls.empty()) {
                    OutStream << "; ⚙️  Expected Runtime Calls:\n";
                    for (const auto& call : Dir.ExpectedRuntimeCalls) {
                        OutStream << ";    • " << call << " - " << AIDatabase.explainRuntimeCall(call) << "\n";
                    }
                }
            }
            if (!Dir.Clauses.empty()) {
                OutStream << "; 🏷️  Clauses: ";
                for (size_t i = 0; i < Dir.Clauses.size(); ++i) {
                    OutStream << Dir.Clauses[i];
                    if (i < Dir.Clauses.size() - 1) OutStream << ", ";
                }
                OutStream << "\n";
            }
            OutStream << ";\n";
        }
        
        OutStream << "; ================================================================\n";
        OutStream << "; === 🔍 LLVM IR WITH AI-ENHANCED RUNTIME ANALYSIS ===\n";
        OutStream << "; ================================================================\n\n";
        
        // Process IR with AI enhancement
        std::string Line;
        while (std::getline(IRStream, Line)) {
            if (Line.find("__kmpc_") != std::string::npos || Line.find("__tgt_") != std::string::npos) {
                OutStream << "; ╔════════════════════════════════════════════════════════════════╗\n";
                OutStream << "; ║                    🎯 OPENMP RUNTIME CALL DETECTED            ║\n";
                OutStream << "; ╚════════════════════════════════════════════════════════════════╝\n";
                
                std::regex runtimeCallRegex("(__[a-zA-Z0-9_]+)");
                std::smatch match;
                if (std::regex_search(Line, match, runtimeCallRegex)) {
                    std::string runtimeCall = match.str();
                    OutStream << "; 🔧 Runtime Call: " << runtimeCall << "\n";
                    OutStream << "; 💡 AI Explanation: " << AIDatabase.explainRuntimeCall(runtimeCall) << "\n";
                    
                    // Try to map to source directive
                    for (const auto &Dir : Visitor.getDirectives()) {
                        if (isRuntimeCallRelated(runtimeCall, Dir)) {
                            OutStream << "; 📍 Source Mapping: Line " << Dir.LineNumber
                                     << " - #pragma omp " << Dir.DirectiveType << "\n";
                            break;
                        }
                    }
                }
                OutStream << "; ────────────────────────────────────────────────────────────────\n";
            }
            OutStream << Line << "\n";
        }
        
        // AI Summary
        OutStream << "\n; ================================================================\n";
        OutStream << "; === 📊 AI-GENERATED ANALYSIS SUMMARY ===\n";
        OutStream << "; Total OpenMP directives analyzed: " << Visitor.getDirectives().size() << "\n";
        OutStream << "; AI-Enhanced Features:\n";
        OutStream << ";    ✅ AI-generated descriptions for each OpenMP construct\n";
        OutStream << ";    ✅ Runtime call identification with purpose explanation\n";
        OutStream << ";    ✅ Source-to-IR mapping with AI insights\n";
        OutStream << "; ================================================================\n";
        
        llvm::outs() << "🎉 AI-enhanced IR written to: " << OutputFileName << "\n";
    }
    
    bool isRuntimeCallRelated(const std::string &runtimeCall, const AIEnhancedDirective &Dir) {
        for (const auto &expectedCall : Dir.ExpectedRuntimeCalls) {
            if (runtimeCall.find(expectedCall) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

// Frontend Action
class AIEnhancedOpenMPAction : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) override {
        return std::make_unique<AIEnhancedOpenMPConsumer>(&Compiler.getASTContext(), InFile.str());
    }
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, OpenMPMapperCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    std::string HomeDir = getenv("HOME");
    std::string ClangBuiltins = HomeDir + "/Documents/openmp-project/llvm-project/build/lib/clang/21/include";
    
    std::vector<std::string> ExtraArgs = {
        "-fopenmp", "-Wno-unknown-pragmas", "-I" + ClangBuiltins,
        "-isysroot", "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk",
        "-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1",
        "-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include"
    };
    
    // Add OpenMP includes if available
    FILE* brew_fp = popen("brew --prefix libomp 2>/dev/null", "r");
    if (brew_fp != nullptr) {
        char brew_path[1024];
        if (fgets(brew_path, sizeof(brew_path), brew_fp) != nullptr) {
            std::string ompPath = std::string(brew_path);
            if (!ompPath.empty() && ompPath.back() == '\n') {
                ompPath.pop_back();
            }
            ExtraArgs.push_back("-I" + ompPath + "/include");
        }
        pclose(brew_fp);
    }
    
    ArgumentsAdjuster ArgsAdjuster = getInsertArgumentAdjuster(ExtraArgs, ArgumentInsertPosition::BEGIN);
    Tool.appendArgumentsAdjuster(ArgsAdjuster);

    auto SourceFiles = OptionsParser.getSourcePathList();
    if (SourceFiles.empty()) {
        llvm::errs() << "No source files provided\n";
        return 1;
    }

    llvm::outs() << "🤖====================================================\n";
    llvm::outs() << "    AI-ENHANCED OPENMP SOURCE-TO-IR MAPPER\n";
    llvm::outs() << "====================================================🤖\n";
    llvm::outs() << "🔍 Processing: " << SourceFiles[0] << "\n";
    
    if (VerboseDescriptions) {
        llvm::outs() << "🗣️  Verbose mode: ON\n";
    }
    if (ExplainTransformations) {
        llvm::outs() << "🔄 Explanations: ON\n";
    }
    
    llvm::outs() << "====================================================\n";
    
    int result = Tool.run(newFrontendActionFactory<AIEnhancedOpenMPAction>().get());
    
    if (result == 0) {
        llvm::outs() << "\n🎉=== AI-ENHANCED ANALYSIS COMPLETE ===🎉\n";
        llvm::outs() << "✅ AI descriptions and runtime analysis ready\n";
    }
    
    return result;
}
