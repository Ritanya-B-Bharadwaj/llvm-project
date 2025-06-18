#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace clang;

static double threshold = 0.001;
static std::string mode = "fp16";
static std::vector<std::string> jsonEntries;

// Simulate __fp16
float simulate_fp16(float value) {
  uint32_t bits;
  memcpy(&bits, &value, sizeof(float));
  int sign = (bits >> 31) & 0x1;
  int exponent = (bits >> 23) & 0xFF;
  int mantissa = bits & 0x7FFFFF;

  int newExp = exponent - 127 + 15;
  if (newExp <= 0 || newExp >= 31)
    return 0.0f;

  int newMantissa = mantissa >> 13;
  uint16_t halfBits = (sign << 15) | (newExp << 10) | newMantissa;

  int expandedSign = (halfBits >> 15) & 0x1;
  int expandedExp = ((halfBits >> 10) & 0x1F) - 15 + 127;
  int expandedMant = (halfBits & 0x3FF) << 13;

  uint32_t floatBits =
      (expandedSign << 31) | (expandedExp << 23) | expandedMant;
  float result;
  memcpy(&result, &floatBits, sizeof(float));
  return result;
}

// Simulate __bf16
float simulate_bf16(float value) {
  uint32_t bits;
  memcpy(&bits, &value, sizeof(float));
  uint32_t bf16Bits = bits & 0xFFFF0000;
  float result;
  memcpy(&result, &bf16Bits, sizeof(float));
  return result;
}

class FloatVisitor : public RecursiveASTVisitor<FloatVisitor> {
public:
  FloatVisitor(Rewriter &R, SourceManager &SM, DiagnosticsEngine &D)
      : TheRewriter(R), SM(SM), Diag(D) {}

  bool VisitFloatingLiteral(FloatingLiteral *F) {
    double original = F->getValueAsApproximateDouble();

    float downcast =
        (mode == "bf16") ? simulate_bf16(original) : simulate_fp16(original);
    double error = fabs(original - downcast) / fabs(original);
    SourceLocation loc = F->getBeginLoc();
    PresumedLoc ploc = SM.getPresumedLoc(loc);

    std::ostringstream entry;
    entry << std::fixed << std::setprecision(6) << "  {\n"
          << "    \"value\": " << original << ",\n"
          << "    \"downcast\": " << downcast << ",\n"
          << "    \"error\": " << error << ",\n"
          << "    \"mode\": \"" << mode << "\",\n"
          << "    \"safe\": " << (error <= threshold ? "true" : "false")
          << ",\n"
          << "    \"location\": \"" << ploc.getFilename() << ":"
          << ploc.getLine() << ", col " << ploc.getColumn() << "\"\n"
          << "  }";
    jsonEntries.push_back(entry.str());

    std::ostringstream ossOrig, ossDown, ossErr, ossThresh;
    ossOrig << std::fixed << std::setprecision(6) << original;
    ossDown << std::fixed << std::setprecision(6) << downcast;
    ossErr << std::fixed << std::setprecision(6) << error;
    ossThresh << std::fixed << std::setprecision(6) << threshold;

    if (error == 0.0) {
      unsigned diagID = Diag.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "float literal ‘%0’ can be safely downcast to ‘__%1’");
      Diag.Report(loc, diagID) << ossOrig.str() << mode;
    } else if (error <= threshold) {
      unsigned diagID = Diag.getCustomDiagID(
          DiagnosticsEngine::Warning, "float literal ‘%0’ can be downcast to "
                                      "‘__%1’ within acceptable error");
      Diag.Report(loc, diagID) << ossOrig.str() << mode;

      unsigned noteID = Diag.getCustomDiagID(
          DiagnosticsEngine::Note, "relative error is %0, threshold is %1");
      Diag.Report(loc, noteID) << ossErr.str() << ossThresh.str();
    } else {
      unsigned noteID =
          Diag.getCustomDiagID(DiagnosticsEngine::Note,
                               "converting to ‘__%0’ would introduce relative "
                               "error of %1, exceeding threshold %2");
      Diag.Report(loc, noteID) << mode << ossErr.str() << ossThresh.str();
    }

    if (error <= threshold) {
      std::ostringstream replacement;
      replacement << "__" << mode << "(" << std::setprecision(8) << downcast
                  << ")";
      CharSourceRange charRange =
          CharSourceRange::getTokenRange(F->getSourceRange());
      if (charRange.isValid())
        TheRewriter.ReplaceText(charRange, replacement.str());
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
  SourceManager &SM;
  DiagnosticsEngine &Diag;
};

class FloatASTConsumer : public ASTConsumer {
public:
  FloatASTConsumer(Rewriter &R, SourceManager &SM, DiagnosticsEngine &D)
      : Visitor(R, SM, D) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FloatVisitor Visitor;
};

class FloatFrontendAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (const auto &arg : args) {
      if (arg.rfind("-threshold=", 0) == 0)
        threshold = std::stod(arg.substr(11));
      else if (arg.rfind("-mode=", 0) == 0)
        mode = arg.substr(6);
    }
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<FloatASTConsumer>(
        TheRewriter, CI.getSourceManager(), CI.getDiagnostics());
  }

  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    std::error_code EC;
    llvm::raw_fd_ostream out("modified.cpp", EC);
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(out);

    std::ofstream jsonOut("float_map.json");
    jsonOut << "[\n";
    for (size_t i = 0; i < jsonEntries.size(); ++i) {
      jsonOut << jsonEntries[i];
      if (i + 1 != jsonEntries.size())
        jsonOut << ",\n";
    }
    jsonOut << "\n]\n";
    jsonOut.close();
  }

private:
  Rewriter TheRewriter;
};

static FrontendPluginRegistry::Add<FloatFrontendAction>
    X("float-downcast", "Analyze and downcast float literals safely");
