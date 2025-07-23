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

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Command line options
static cl::OptionCategory OpenMPMapperCategory("OpenMP Source-to-IR Mapper");
static cl::opt<std::string> OutputFile("o", cl::desc("Output file for annotated IR"), 
                                       cl::value_desc("filename"), cl::cat(OpenMPMapperCategory));

// Structure to hold OpenMP directive information
struct OpenMPDirective {
  unsigned LineNumber;
  unsigned ColumnNumber;
  std::string DirectiveType;
  std::string SourceText;
  std::string FilePath;
  std::vector<std::string> Clauses;
};

// AST Visitor to find OpenMP directives
class OpenMPVisitor : public RecursiveASTVisitor<OpenMPVisitor> {
private:
  ASTContext *Context;
  SourceManager *SM;
  std::vector<OpenMPDirective> Directives;

public:
  explicit OpenMPVisitor(ASTContext *Context) : Context(Context), SM(&Context->getSourceManager()) {}

  bool VisitOMPExecutableDirective(OMPExecutableDirective *Directive) {
    SourceLocation Loc = Directive->getBeginLoc();
    if (!SM->isWrittenInMainFile(Loc)) {
      return true; // Skip directives not in main file
    }

    OpenMPDirective OMPDir;
    OMPDir.LineNumber = SM->getSpellingLineNumber(Loc);
    OMPDir.ColumnNumber = SM->getSpellingColumnNumber(Loc);
    
    // Get directive type - updated API
    OMPDir.DirectiveType = getOpenMPDirectiveName(Directive->getDirectiveKind()).str();
    OMPDir.FilePath = SM->getFilename(Loc).str();

    // Extract source text
    SourceRange Range = Directive->getSourceRange();
    OMPDir.SourceText = getSourceText(Range);

    // Extract clauses - simplified for now
    for (auto *Clause : Directive->clauses()) {
      if (Clause) {
        OMPDir.Clauses.push_back("clause"); // Simplified
      }
    }

    Directives.push_back(OMPDir);
    
    llvm::outs() << "Found OpenMP directive: " << OMPDir.DirectiveType 
                 << " at line " << OMPDir.LineNumber << "\n";
    
    return true;
  }

  const std::vector<OpenMPDirective>& getDirectives() const {
    return Directives;
  }

private:
  std::string getSourceText(SourceRange Range) {
    if (Range.isInvalid()) return "";
    
    bool Invalid = false;
    StringRef Text = Lexer::getSourceText(CharSourceRange::getTokenRange(Range), 
                                         *SM, Context->getLangOpts(), &Invalid);
    if (Invalid) return "";
    return Text.str();
  }
};

// AST Consumer
class OpenMPConsumer : public ASTConsumer {
private:
  OpenMPVisitor Visitor;
  std::string SourceFile;

public:
  explicit OpenMPConsumer(ASTContext *Context, const std::string &File) 
    : Visitor(Context), SourceFile(File) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    
    // Generate IR with debug info
    generateAnnotatedIR();
  }

private:
  void generateAnnotatedIR() {
    // Use our built clang with proper system includes
    std::string ClangPath = getenv("HOME");
    ClangPath += "/Documents/openmp-project/llvm-project/build/bin/clang++";
    
    std::string OutputIR = SourceFile + ".ll";
    
    // Build clang command with proper includes - Fixed version
    std::string CmdLine = ClangPath + " -fopenmp -S -emit-llvm -g -O0";
    
    // CRITICAL: Add the built-in clang includes FIRST
    std::string HomeDir = getenv("HOME");
    std::string ClangBuiltins = HomeDir + "/Documents/openmp-project/llvm-project/build/lib/clang/21/include";
    CmdLine += " -I" + ClangBuiltins;
    
    // Add system C++ headers
    CmdLine += " -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1";
    CmdLine += " -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include";
    
    // Set the system root
    CmdLine += " -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk";
    
    // Try to find libomp if installed
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
    
    // Add the source file and output
    CmdLine += " -o " + OutputIR + " " + SourceFile;
    
    llvm::outs() << "Executing: " << CmdLine << "\n";
    int Result = std::system(CmdLine.c_str());
    
    if (Result == 0) {
      llvm::outs() << "Generated IR: " << OutputIR << "\n";
      analyzeAndAnnotateIR(OutputIR);
    } else {
      llvm::errs() << "Failed to generate IR (exit code: " << Result << ")\n";
    }
  }
  
  void analyzeAndAnnotateIR(const std::string &IRFile) {
    std::ifstream IRStream(IRFile);
    if (!IRStream) {
      llvm::errs() << "Cannot open IR file: " << IRFile << "\n";
      return;
    }
    
    std::string OutputFileName = OutputFile.empty() ? 
      (SourceFile + ".annotated.ll") : OutputFile;
    std::ofstream OutStream(OutputFileName);
    
    OutStream << "; OpenMP Source-to-IR Mapping\n";
    OutStream << "; Generated from: " << SourceFile << "\n\n";
    
    // Add directive summary
    OutStream << "; OpenMP Directives Found:\n";
    for (const auto &Dir : Visitor.getDirectives()) {
      OutStream << "; Line " << Dir.LineNumber << ": " << Dir.DirectiveType << "\n";
    }
    OutStream << "\n";
    
    // Process IR line by line and annotate
    std::string Line;
    while (std::getline(IRStream, Line)) {
      // Annotate OpenMP runtime calls
      if (Line.find("__kmpc_") != std::string::npos || 
          Line.find("omp_") != std::string::npos) {
        OutStream << "; >>> OpenMP Runtime Call <<<\n";
        
        // Try to match with source directives
        for (const auto &Dir : Visitor.getDirectives()) {
          if (isRelatedToDirective(Line, Dir)) {
            OutStream << "; Source: Line " << Dir.LineNumber 
                     << " - " << Dir.DirectiveType << "\n";
            break;
          }
        }
      }
      
      // Output the IR line
      OutStream << Line << "\n";
    }
    
    llvm::outs() << "Annotated IR written to: " << OutputFileName << "\n";
  }
  
  bool isRelatedToDirective(const std::string &IRLine, const OpenMPDirective &Dir) {
    // Simple heuristic to match IR calls with directives
    if (Dir.DirectiveType == "parallel" && IRLine.find("__kmpc_fork_call") != std::string::npos) {
      return true;
    }
    if (Dir.DirectiveType == "for" && IRLine.find("__kmpc_for_static_init") != std::string::npos) {
      return true;
    }
    if (Dir.DirectiveType == "parallel for" && 
        (IRLine.find("__kmpc_fork_call") != std::string::npos || 
         IRLine.find("__kmpc_for_static_init") != std::string::npos)) {
      return true;
    }
    if (Dir.DirectiveType == "target" && IRLine.find("__tgt_") != std::string::npos) {
      return true;
    }
    return false;
  }
};

// Frontend Action
class OpenMPAction : public ASTFrontendAction {
public:
  OpenMPAction() = default;

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,
                                                 StringRef InFile) override {
    return std::make_unique<OpenMPConsumer>(&Compiler.getASTContext(), InFile.str());
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

  // Add compiler flags for OpenMP and system includes
  // CRITICAL: Put the built-in clang includes FIRST
  std::string HomeDir = getenv("HOME");
  std::string ClangBuiltins = HomeDir + "/Documents/openmp-project/llvm-project/build/lib/clang/21/include";
  
  std::vector<std::string> ExtraArgs = {
    "-fopenmp", 
    "-Wno-unknown-pragmas",
    "-I" + ClangBuiltins,  // Built-in clang headers FIRST
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

  // Get source file
  auto SourceFiles = OptionsParser.getSourcePathList();
  if (SourceFiles.empty()) {
    llvm::errs() << "No source files provided\n";
    return 1;
  }

  std::string SourceFile = SourceFiles[0];
  
  llvm::outs() << "Processing OpenMP source file: " << SourceFile << "\n";
  
  // Run the tool
  return Tool.run(newFrontendActionFactory<OpenMPAction>().get());
}
