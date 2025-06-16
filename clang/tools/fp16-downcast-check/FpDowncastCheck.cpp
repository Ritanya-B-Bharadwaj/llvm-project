//===- FpDowncastCheck.cpp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Clang plugin that analyses floating-point literals (float/double) and
// checks whether they can be losslessly or near-losslessly represented in
// reduced-precision binary16 (__fp16) format.
//
//  • If the literal converts to binary16 exactly → emit a remark suggesting
//    downcast.
//  • Otherwise compute relative error:
//        err = |orig − downcast| / |orig|
//    If err ≤ user-supplied threshold (default 0.001) → emit warning + note.
//  • If err > threshold → just note that it exceeds threshold.
//
// Usage example:
//   clang -Xclang -load -Xclang FpDowncastCheck.dylib \
//         -Xclang -plugin -Xclang fp16-downcast-check \
//         -Xclang -plugin-arg-fp16-downcast-check -Xclang -threshold=0.001 \
//         -fsyntax-only test.c
//
//===----------------------------------------------------------------------===//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

using namespace clang;

namespace {

class FpVisitor : public RecursiveASTVisitor<FpVisitor> {
  [[maybe_unused]] ASTContext &Ctx;
  DiagnosticsEngine &Diags;
  double Threshold;
public:
  FpVisitor(ASTContext &Ctx, DiagnosticsEngine &Diags, double Threshold)
      : Ctx(Ctx), Diags(Diags), Threshold(Threshold) {}

  bool VisitFloatingLiteral(FloatingLiteral *FL) {
    // Obtain the literal's APFloat value.
    const llvm::APFloat &OrigVal = FL->getValue();

    // Copy then convert to IEEE half precision.
    llvm::APFloat DownVal = OrigVal;
    bool LosesInfo = false;
    DownVal.convert(llvm::APFloat::IEEEhalf(), llvm::APFloat::rmNearestTiesToEven,
                    &LosesInfo);

    // Compute relative error using double for convenience.
    double OrigD = OrigVal.convertToDouble();
    double DownD = DownVal.convertToDouble();
    double RelErr = 0.0;
    if (OrigD != 0.0)
      RelErr = std::abs(OrigD - DownD) / std::abs(OrigD);

    // Build pretty string for the literal.
    llvm::SmallString<32> Buf;
    OrigVal.toString(Buf, 0, 0);
    std::string LiteralStr = Buf.str().str();

    SourceLocation Loc = FL->getBeginLoc();

    if (!LosesInfo) {
      unsigned ID = Diags.getCustomDiagID(DiagnosticsEngine::Warning,
                                          "float literal '%0' can be safely downcast to '__fp16'");
      Diags.Report(Loc, ID) << LiteralStr;
    } else if (RelErr <= Threshold) {
      unsigned ID = Diags.getCustomDiagID(DiagnosticsEngine::Warning,
                                          "float literal '%0' can be downcast to '__fp16' within acceptable error");
      Diags.Report(Loc, ID) << LiteralStr;

      unsigned NoteID = Diags.getCustomDiagID(DiagnosticsEngine::Note,
                                              "relative error is %0, threshold is %1");
      llvm::SmallString<32> ErrStr, ThStr;
      llvm::raw_svector_ostream ErrOS(ErrStr), ThOS(ThStr);
      ErrOS << RelErr; ThOS << Threshold;
      Diags.Report(Loc, NoteID) << ErrOS.str() << ThOS.str();
    } else {
      unsigned NoteID = Diags.getCustomDiagID(DiagnosticsEngine::Note,
                                              "converting to '__fp16' would introduce relative error of %0, exceeding threshold %1");
      llvm::SmallString<32> ErrStr2, ThStr2;
      llvm::raw_svector_ostream ErrOS2(ErrStr2), ThOS2(ThStr2);
      ErrOS2 << RelErr; ThOS2 << Threshold;
      Diags.Report(Loc, NoteID) << ErrOS2.str() << ThOS2.str();
    }

    return true; // continue traversal
  }
};

class FpConsumer : public ASTConsumer {
  CompilerInstance &CI;
  double Threshold;
public:
  FpConsumer(CompilerInstance &CI, double Threshold)
      : CI(CI), Threshold(Threshold) {}

  void HandleTranslationUnit(ASTContext &Ctx) override {
    FpVisitor V(Ctx, CI.getDiagnostics(), Threshold);
    V.TraverseDecl(Ctx.getTranslationUnitDecl());
  }
};

class FpDowncastCheckAction : public PluginASTAction {
  double Threshold = 0.001; // default

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                llvm::StringRef) override {
    return std::make_unique<FpConsumer>(CI, Threshold);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    DiagnosticsEngine &D = CI.getDiagnostics();

    for (size_t I = 0, E = Args.size(); I != E; ++I) {
      llvm::StringRef Arg = Args[I];
      if (Arg.starts_with("-threshold=")) {
        Arg = Arg.drop_front(strlen("-threshold="));
      } else if (Arg == "-threshold") {
        if (I + 1 >= E) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing value for -threshold"));
          return false;
        }
        Arg = Args[++I];
      } else if (Arg == "help") {
        PrintHelp(llvm::errs());
        continue;
      } else {
        unsigned ID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "unknown argument '%0'");
        D.Report(ID) << Arg;
        return false;
      }

      double Val;
      if (Arg.getAsDouble(Val)) {
        unsigned ID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "invalid floating-point threshold: '%0'");
        D.Report(ID) << Arg;
        return false;
      }
      if (Val < 0) {
        unsigned ID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "threshold must be non-negative");
        D.Report(ID);
        return false;
      }
      Threshold = Val;
    }

    return true;
  }

  void PrintHelp(llvm::raw_ostream &OS) {
    OS << "fp16-downcast-check plugin help:\n"
          "  -threshold=<N>  Relative-error threshold (default: 0.001).\n";
  }
};

} // namespace

static FrontendPluginRegistry::Add<FpDowncastCheckAction>
    X("fp16-downcast-check", "Suggest downcasting float literals to __fp16"); 