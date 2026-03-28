#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Expr.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Module.h"
#include <string>

using namespace clang;

namespace {

  // Function to write messages to a log file
  void writeToLogFile(const std::string &message, bool newLine = true) {
    std::error_code EC;
    llvm::raw_fd_ostream outFile("float_log.txt", EC, llvm::sys::fs::OF_Append); // Open the log file in append mode, truncating it if it exists

    if (EC) {
      llvm::errs() << "Error opening log file: " << EC.message() << "\n";
      return;
    }

    if (newLine) {  
      outFile << message << "\n";
    } else {
      outFile << message;
    }
  }

  // Class to handle the AST consumer for printing FP32 variable information
  class PrintFP32VarsConsumer : public ASTConsumer {
    // Reference to the necessary components
    CompilerInstance &Instance;
    std::set<std::string> ParsedTemplates;
    Rewriter TheRewriter;
    bool EnableDemotion;

    // Visitor class to traverse the AST
    struct FP32VarVisitor : public RecursiveASTVisitor<FP32VarVisitor> {
      // Reference to the AST context, rewriter, and diagnostics engine
      ASTContext &Ctx;
      Rewriter TheRewriter;
      DiagnosticsEngine &Diags;
      unsigned DiagID;
      bool EnableDemotion;
      SourceManager &SM;
      
      // Constructor to initialize the visitor with context, rewriter, and diagnostics
      FP32VarVisitor(ASTContext &Ctx, Rewriter &Rewriter, DiagnosticsEngine &Diags, bool EnableDemotion, SourceManager &SM)
          : Ctx(Ctx), TheRewriter(Rewriter), Diags(Diags), EnableDemotion(EnableDemotion), SM(SM) {
        DiagID = Diags.getCustomDiagID(DiagnosticsEngine::Warning, "Demoted float variable '%0' to __fp16");
      }

      // Function to check if a float value can be safely demoted to __fp16
      bool IsFP16Safe(float Value) {
        llvm::APFloat FP32Val(Value);
        bool LosesInfo;
        auto Status = FP32Val.convert(llvm::APFloat::IEEEhalf(),llvm::APFloat::rmNearestTiesToEven, &LosesInfo);

        if (Status != llvm::APFloat::opOK || LosesInfo) { // Check if conversion was successful and if it loses information
          return false;
        }

        llvm::APFloat BackToFP32(FP32Val);
        Status = BackToFP32.convert(llvm::APFloat::IEEEsingle(),llvm::APFloat::rmNearestTiesToEven, &LosesInfo);

        if (Status != llvm::APFloat::opOK || LosesInfo) { // Check if conversion back to FP32 was successful and if it loses information
          return false;
        }

        return BackToFP32.convertToFloat() == Value;
      }

      // Function to visit variable declarations
      bool VisitDecl(Decl *D) {

        if (const VarDecl *VD = dyn_cast<VarDecl>(D)) { // Check if the declaration is a variable declaration

          if (VD->getType()->isFloatingType() && VD->getType()->isSpecificBuiltinType(BuiltinType::Float)) { // Check if the variable is of type float
            writeToLogFile("fp32-decl: \"" + VD->getNameAsString() + "\" (kind: " + D->getDeclKindName() + ")");
            bool CanDemote = false;
            
            if (const Expr *Init = VD->getInit()) { // Check if the variable has an initializer
              llvm::APFloat Value(0.0f);

              if (Init->EvaluateAsFloat(Value, Ctx)) { // Evaluate the initializer as a float
                float Val = Value.convertToFloat();
                writeToLogFile("Evaluated float value: " + std::to_string(Val));
                CanDemote = IsFP16Safe(Val);
                writeToLogFile("Can demote: " + std::string(CanDemote ? "true" : "false"));
              }
              else {
                writeToLogFile("Failed to evaluate float value for: " + VD->getNameAsString());
              }
            } 
            else {
              writeToLogFile("No initializer for variable: " + VD->getNameAsString());
              CanDemote = false; // No initializer means we can't determine if it's safe to demote
            }

            if (EnableDemotion && CanDemote) { // If demotion is enabled and the value can be safely demoted
              Diags.Report(VD->getLocation(), DiagID) << VD->getNameAsString();
              SourceLocation TypeLoc = VD->getTypeSourceInfo()->getTypeLoc().getBeginLoc();
              

              if (TypeLoc.isValid()) { // Check if the type location is valid
                std::error_code EC;
                std::string InputFile;

                if (auto FileEntry = SM.getFileEntryRefForID(SM.getMainFileID())) { // Get the main file entry
                  InputFile = FileEntry->getName().str();
                } 
                else {
                  InputFile = "default_input";
                }
                llvm::raw_fd_ostream Out(InputFile, EC); 

                if (EC) {
                    llvm::errs() << "Error creating output file: " << EC.message() << "\n";
                    return true; 
                }
                TheRewriter.ReplaceText(TypeLoc, 5, "__fp16"); 
                writeToLogFile("Replaced float with __fp16 for: " + VD->getNameAsString());
                TheRewriter.getEditBuffer(Ctx.getSourceManager().getMainFileID()).write(Out);
              }
              else {
                writeToLogFile("Invalid type location for variable: " + VD->getNameAsString());
              }
            }
          }
        }
        return true;
      }
    };

  // Constructor to initialize the consumer with the compiler instance and parsed templates  
  public:
    PrintFP32VarsConsumer(CompilerInstance &Instance,
                          std::set<std::string> ParsedTemplates,
                          bool EnableDemotion)
        : Instance(Instance), ParsedTemplates(ParsedTemplates),
          TheRewriter(Instance.getSourceManager(), Instance.getLangOpts()),
          EnableDemotion(EnableDemotion) {}

    void HandleTranslationUnit(ASTContext &context) override {
      FP32VarVisitor Visitor(context, TheRewriter, Instance.getDiagnostics(), EnableDemotion, Instance.getSourceManager());
      Visitor.TraverseDecl(context.getTranslationUnitDecl());

      if (!Instance.getLangOpts().DelayedTemplateParsing) //  Check if delayed template parsing is enabled
        return;

      struct TemplateVisitor : public RecursiveASTVisitor<TemplateVisitor> {
        const std::set<std::string> &ParsedTemplates;
        TemplateVisitor(const std::set<std::string> &ParsedTemplates)
            : ParsedTemplates(ParsedTemplates) {}
        bool VisitFunctionDecl(FunctionDecl *FD) {
          if (FD->isLateTemplateParsed() && 
              ParsedTemplates.count(FD->getNameAsString())) //  Check if the function is late template parsed and in the parsed templates set
            LateParsedDecls.insert(FD);
          return true;
        }

        std::set<FunctionDecl*> LateParsedDecls;
      } v(ParsedTemplates);
      v.TraverseDecl(context.getTranslationUnitDecl());
      clang::Sema &sema = Instance.getSema();
      
      for (const FunctionDecl *FD : v.LateParsedDecls) {
        clang::LateParsedTemplate &LPT =
            *sema.LateParsedTemplateMap.find(FD)->second;
        sema.LateTemplateParser(sema.OpaqueParser, LPT);
        writeToLogFile("Late template parsed for function: " + FD->getNameAsString());
      }

      if (EnableDemotion) { // If demotion is enabled, check if any modifications were made
        bool Modified = TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID()).size() > 0;

        if (Modified) { // If modifications were made, write them to the log file
          writeToLogFile("Modifications made to source code");
          TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
              .write(llvm::outs());
        } 
        else {
          writeToLogFile("No modifications made to source code");
        }
      }
    }
  };

  // Action class to register the plugin with Clang's frontend
  class PrintFP32VarsAction : public PluginASTAction {
    std::set<std::string> ParsedTemplates;
    bool EnableDemotion = false;
  protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
      return std::make_unique<PrintFP32VarsConsumer>(CI, ParsedTemplates, EnableDemotion);
    }

    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {

      for (unsigned i = 0, e = args.size(); i != e; ++i) {

      if (args[i] == "-fprecision-demote=fp16") { // Enable demotion to __fp16
          EnableDemotion = true;
        }
      }

      if (!args.empty() && args[0] == "help") // Print help message
        PrintHelp(llvm::errs());
      return true;
    }
    void PrintHelp(llvm::raw_ostream& ros) {
      ros << "Use '-plugin-arg-floatdemote -fprecision-demote=fp16' to enable demotion to __fp16\n";
    }
  };

}

static FrontendPluginRegistry::Add<PrintFP32VarsAction>
X("floatdemote", "print FP32 (float) variables with __fp16 safety check");