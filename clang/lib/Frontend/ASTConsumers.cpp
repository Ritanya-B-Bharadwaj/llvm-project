//===--- ASTConsumers.cpp - ASTConsumer implementations -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// AST Consumer Implementations.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/ASTConsumers.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaConsumer.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;

// namespace {

// class AutoTypeDumperSemaConsumer : public SemaConsumer {
//   Sema *S;

// public:
//   explicit AutoTypeDumperSemaConsumer() : S(nullptr) {}

//   void InitializeSema(Sema &TheSema) override { S = &TheSema; }

//   bool HandleTopLevelDecl(DeclGroupRef DG) override {
//     for (auto *D : DG) {
//       if (auto *VD = dyn_cast<VarDecl>(D)) {
//         HandleVarDecl(VD);
//       }
//       if (auto *FD = dyn_cast<FunctionDecl>(D)) {
//         if (FD->hasBody()) {
//           VisitStmt(FD->getBody());
//         }
//       }
//     }
//     return true;
//   }

// private:
//   void HandleVarDecl(VarDecl *VD) {
//     SourceManager &SM = S->getSourceManager();
//     SourceLocation Loc = VD->getLocation();
//     if (!SM.isInMainFile(Loc))
//       return; // Only process variables from the main file

//     // Check if the original type was auto (before deduction)
//     if (auto *AT = VD->getType()->getAs<AutoType>()) {
//       unsigned LineNum = SM.getSpellingLineNumber(Loc);
//       unsigned ColNum = SM.getSpellingColumnNumber(Loc);
//       llvm::errs() << SM.getFilename(Loc) << ":" << LineNum << ":" << ColNum
//                    << ": note: type of '" << VD->getNameAsString()
//                    << "' deduced as '";
//       VD->getType().print(llvm::errs(),
//       S->getASTContext().getPrintingPolicy()); llvm::errs() << "'\n";
//     }
//   }

//   void VisitStmt(Stmt *St) {
//     if (!St)
//       return;
//     if (auto *DS = dyn_cast<DeclStmt>(St)) {
//       for (auto *D : DS->decls()) {
//         if (auto *VD = dyn_cast<VarDecl>(D)) {
//           HandleVarDecl(VD);
//         }
//       }
//     }
//     // Recursively visit child statements
//     for (auto *Child : St->children()) {
//       VisitStmt(Child);
//     }
//   }
// };

// } // namespace

namespace {
class AutoTypeDumperSemaConsumer : public SemaConsumer {
  Sema *S;

public:
  explicit AutoTypeDumperSemaConsumer() : S(nullptr) {}
  void InitializeSema(Sema &TheSema) override { S = &TheSema; }

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (auto *D : DG) {
      if (auto *VD = dyn_cast<VarDecl>(D)) {
        HandleVarDecl(VD);
      } else if (auto *FD = dyn_cast<FunctionDecl>(D)) {
        HandleFunctionDecl(FD);
      } else if (auto *TD = dyn_cast<TemplateDecl>(D)) {
        HandleTemplateDecl(TD);
      }
    }
    return true;
  }

  // Override this method to catch auto deduction as it happens
  void HandleCXXImplicitFunctionInstantiation(FunctionDecl *D) override {
    // This catches template function instantiations
    HandleFunctionDecl(D);
  }

private:
  void HandleVarDecl(VarDecl *VD) {
    SourceManager &SM = S->getSourceManager();
    SourceLocation Loc = VD->getLocation();
    if (!SM.isInMainFile(Loc))
      return;
    if (auto *AT = VD->getType()->getAs<AutoType>()) {
      PrintDeducedType(VD->getNameAsString(), Loc, VD->getType());
    }
  }

  void HandleFunctionDecl(FunctionDecl *FD) {
    SourceManager &SM = S->getSourceManager();
    SourceLocation Loc = FD->getLocation();
    if (!SM.isInMainFile(Loc))
      return;

    QualType RT = FD->getReturnType();

    // Check for different types of auto return types
    if (auto *AT = RT->getAs<AutoType>()) {
      // This catches undeduced auto types
      PrintDeducedType(FD->getNameAsString() + " (return type)", Loc, RT);
    } else if (auto *DTST = RT->getAs<DeducedTemplateSpecializationType>()) {
      // This catches deduced class template types
      PrintDeducedType(FD->getNameAsString() + " (return type)", Loc, RT);
    } else {
      // For already-deduced auto return types, we need to check if the function
      // was originally declared with auto. This is the tricky case.

      // One approach: check if this function has a previous declaration with
      // auto
      for (auto *PrevDecl : FD->redecls()) {
        if (PrevDecl != FD) {
          QualType PrevRT = PrevDecl->getReturnType();
          if (auto *PrevAT = PrevRT->getAs<AutoType>()) {
            PrintDeducedType(FD->getNameAsString() + " (return type)", Loc, RT);
            break;
          }
        }
      }

      // Another approach: check type source info for auto keyword
      if (auto *TSI = FD->getTypeSourceInfo()) {
        auto FTL = TSI->getTypeLoc().getAs<FunctionTypeLoc>();
        if (FTL) {
          TypeLoc ReturnTL = FTL.getReturnLoc();
          // Check if the return type location corresponds to an auto type
          if (auto AutoTL = ReturnTL.getAs<AutoTypeLoc>()) {
            PrintDeducedType(FD->getNameAsString() + " (return type)", Loc, RT);
          }
          // Check for function templates with deduced return types
          else if (auto DTSTL =
                       ReturnTL.getAs<DeducedTemplateSpecializationTypeLoc>()) {
            PrintDeducedType(FD->getNameAsString() + " (return type)", Loc, RT);
          }
        }
      }
    }

    if (FD->hasBody())
      VisitStmt(FD->getBody());
  }

  void HandleTemplateDecl(TemplateDecl *TD) {
    if (auto *TTPD = dyn_cast<TemplateTypeParmDecl>(TD->getTemplatedDecl())) {
      // Ignore template **type** parameters
      return;
    }

    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD)) {
      auto *FD = FTD->getTemplatedDecl();
      for (auto *Param : *FTD->getTemplateParameters()) {
        if (auto *NTTP = dyn_cast<NonTypeTemplateParmDecl>(Param)) {
          QualType T = NTTP->getType();
          if (auto *AT = T->getAs<AutoType>()) {
            SourceLocation Loc = NTTP->getLocation();
            if (S->getSourceManager().isInMainFile(Loc)) {
              PrintDeducedType(NTTP->getNameAsString(), Loc, T);
            }
          }
        }
      }
      // Also check function return type inside templated function
      HandleFunctionDecl(FD);
    }
  }

  void VisitStmt(Stmt *St) {
    if (!St)
      return;
    if (auto *DS = dyn_cast<DeclStmt>(St)) {
      for (auto *D : DS->decls()) {
        if (auto *VD = dyn_cast<VarDecl>(D)) {
          HandleVarDecl(VD);
        }
      }
    }
    for (auto *Child : St->children()) {
      VisitStmt(Child);
    }
  }

  void PrintDeducedType(const std::string &Name, SourceLocation Loc,
                        QualType QT) {
    SourceManager &SM = S->getSourceManager();
    unsigned LineNum = SM.getSpellingLineNumber(Loc);
    unsigned ColNum = SM.getSpellingColumnNumber(Loc);
    llvm::errs() << SM.getFilename(Loc) << ":" << LineNum << ":" << ColNum
                 << ": note: type of '" << Name << "' deduced as '";
    QT.print(llvm::errs(), S->getASTContext().getPrintingPolicy());
    llvm::errs() << "'\n";
  }
};
} // namespace

//===----------------------------------------------------------------------===//
/// ASTPrinter - Pretty-printer and dumper of ASTs

namespace {
class AutoTypeDumperVisitor
    : public RecursiveASTVisitor<AutoTypeDumperVisitor> {
  ASTContext &Context;

public:
  explicit AutoTypeDumperVisitor(ASTContext &Ctx) : Context(Ctx) {}
  // Add these debug methods to see what's being visited
  bool VisitDecl(Decl *D) { return true; }

  bool TraverseDecl(Decl *D) { return RecursiveASTVisitor::TraverseDecl(D); }

  bool VisitVarDecl(VarDecl *VD) {

    // Check if the original type was auto (before deduction)
    if (auto *DTST =
            VD->getType()->getAs<DeducedTemplateSpecializationType>()) {
      llvm::errs() << "Auto type deduced: ";
      VD->print(llvm::errs());
      llvm::errs() << " -> Type: ";
      VD->getType().print(llvm::errs(), Context.getPrintingPolicy());
      llvm::errs() << "\n";
    } else if (auto *AT = VD->getType()->getAs<AutoType>()) {
      llvm::errs() << "Auto type found: ";
      VD->print(llvm::errs());
      llvm::errs() << " -> Type: ";
      VD->getType().print(llvm::errs(), Context.getPrintingPolicy());
      llvm::errs() << "\n";
    }
    return true;
  }
};

class AutoTypeDumperConsumer : public ASTConsumer {
  ASTContext &Context;

public:
  explicit AutoTypeDumperConsumer(ASTContext &Ctx) : Context(Ctx) {}

  void HandleTranslationUnit(ASTContext &Ctx) override {
    AutoTypeDumperVisitor(Context).TraverseDecl(Ctx.getTranslationUnitDecl());
  }
};

} // namespace
class ASTPrinter : public ASTConsumer, public RecursiveASTVisitor<ASTPrinter> {
  typedef RecursiveASTVisitor<ASTPrinter> base;

public:
  enum Kind { DumpFull, Dump, Print, None };
  ASTPrinter(std::unique_ptr<raw_ostream> Out, Kind K,
             ASTDumpOutputFormat Format, StringRef FilterString,
             bool DumpLookups = false, bool DumpDeclTypes = false)
      : Out(Out ? *Out : llvm::outs()), OwnedOut(std::move(Out)), OutputKind(K),
        OutputFormat(Format), FilterString(FilterString),
        DumpLookups(DumpLookups), DumpDeclTypes(DumpDeclTypes) {}

  ASTPrinter(raw_ostream &Out, Kind K, ASTDumpOutputFormat Format,
             StringRef FilterString, bool DumpLookups = false,
             bool DumpDeclTypes = false)
      : Out(Out), OwnedOut(nullptr), OutputKind(K), OutputFormat(Format),
        FilterString(FilterString), DumpLookups(DumpLookups),
        DumpDeclTypes(DumpDeclTypes) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    TranslationUnitDecl *D = Context.getTranslationUnitDecl();

    if (FilterString.empty())
      return print(D);

    TraverseDecl(D);
  }

  bool shouldWalkTypesOfTypeLocs() const { return false; }

  bool TraverseDecl(Decl *D) {
    if (D && filterMatches(D)) {
      bool ShowColors = Out.has_colors();
      if (ShowColors)
        Out.changeColor(raw_ostream::BLUE);

      if (OutputFormat == ADOF_Default)
        Out << (OutputKind != Print ? "Dumping " : "Printing ") << getName(D)
            << ":\n";

      if (ShowColors)
        Out.resetColor();
      print(D);
      Out << "\n";
      // Don't traverse child nodes to avoid output duplication.
      return true;
    }
    return base::TraverseDecl(D);
  }

private:
  std::string getName(Decl *D) {
    if (isa<NamedDecl>(D))
      return cast<NamedDecl>(D)->getQualifiedNameAsString();
    return "";
  }
  bool filterMatches(Decl *D) {
    return getName(D).find(FilterString) != std::string::npos;
  }
  void print(Decl *D) {
    if (DumpLookups) {
      if (DeclContext *DC = dyn_cast<DeclContext>(D)) {
        if (DC == DC->getPrimaryContext())
          DC->dumpLookups(Out, OutputKind != None, OutputKind == DumpFull);
        else
          Out << "Lookup map is in primary DeclContext "
              << DC->getPrimaryContext() << "\n";
      } else
        Out << "Not a DeclContext\n";
    } else if (OutputKind == Print) {
      PrintingPolicy Policy(D->getASTContext().getLangOpts());
      D->print(Out, Policy, /*Indentation=*/0, /*PrintInstantiation=*/true);
    } else if (OutputKind != None) {
      D->dump(Out, OutputKind == DumpFull, OutputFormat);
    }

    if (DumpDeclTypes) {
      Decl *InnerD = D;
      if (auto *TD = dyn_cast<TemplateDecl>(D))
        if (Decl *TempD = TD->getTemplatedDecl())
          InnerD = TempD;

      // FIXME: Support OutputFormat in type dumping.
      // FIXME: Support combining -ast-dump-decl-types with -ast-dump-lookups.
      if (auto *VD = dyn_cast<ValueDecl>(InnerD))
        VD->getType().dump(Out, VD->getASTContext());
      if (auto *TD = dyn_cast<TypeDecl>(InnerD))
        TD->getTypeForDecl()->dump(Out, TD->getASTContext());
    }
  }

  raw_ostream &Out;
  std::unique_ptr<raw_ostream> OwnedOut;

  /// How to output individual declarations.
  Kind OutputKind;

  /// What format should the output take?
  ASTDumpOutputFormat OutputFormat;

  /// Which declarations or DeclContexts to display.
  std::string FilterString;

  /// Whether the primary output is lookup results or declarations. Individual
  /// results will be output with a format determined by OutputKind. This is
  /// incompatible with OutputKind == Print.
  bool DumpLookups;

  /// Whether to dump the type for each declaration dumped.
  bool DumpDeclTypes;
};

class ASTDeclNodeLister : public ASTConsumer,
                          public RecursiveASTVisitor<ASTDeclNodeLister> {
public:
  ASTDeclNodeLister(raw_ostream *Out = nullptr)
      : Out(Out ? *Out : llvm::outs()) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    TraverseDecl(Context.getTranslationUnitDecl());
  }

  bool shouldWalkTypesOfTypeLocs() const { return false; }

  bool VisitNamedDecl(NamedDecl *D) {
    D->printQualifiedName(Out);
    Out << '\n';
    return true;
  }

private:
  raw_ostream &Out;
};

std::unique_ptr<ASTConsumer>
clang::CreateASTPrinter(std::unique_ptr<raw_ostream> Out,
                        StringRef FilterString) {
  return std::make_unique<ASTPrinter>(std::move(Out), ASTPrinter::Print,
                                      ADOF_Default, FilterString);
}

std::unique_ptr<ASTConsumer>
clang::CreateASTDumper(std::unique_ptr<raw_ostream> Out, StringRef FilterString,
                       bool DumpDecls, bool Deserialize, bool DumpLookups,
                       bool DumpDeclTypes, ASTDumpOutputFormat Format) {
  assert((DumpDecls || Deserialize || DumpLookups) && "nothing to dump");
  return std::make_unique<ASTPrinter>(std::move(Out),
                                      Deserialize ? ASTPrinter::DumpFull
                                      : DumpDecls ? ASTPrinter::Dump
                                                  : ASTPrinter::None,
                                      Format, FilterString, DumpLookups,
                                      DumpDeclTypes);
}

std::unique_ptr<ASTConsumer>
clang::CreateASTDumper(raw_ostream &Out, StringRef FilterString, bool DumpDecls,
                       bool Deserialize, bool DumpLookups, bool DumpDeclTypes,
                       ASTDumpOutputFormat Format) {
  assert((DumpDecls || Deserialize || DumpLookups) && "nothing to dump");
  return std::make_unique<ASTPrinter>(Out,
                                      Deserialize ? ASTPrinter::DumpFull
                                      : DumpDecls ? ASTPrinter::Dump
                                                  : ASTPrinter::None,
                                      Format, FilterString, DumpLookups,
                                      DumpDeclTypes);
}

std::unique_ptr<ASTConsumer> clang::CreateASTDeclNodeLister() {
  return std::make_unique<ASTDeclNodeLister>(nullptr);
}

//===----------------------------------------------------------------------===//
/// ASTViewer - AST Visualization

namespace {
class ASTViewer : public ASTConsumer {
  ASTContext *Context = nullptr;

public:
  void Initialize(ASTContext &Context) override { this->Context = &Context; }

  bool HandleTopLevelDecl(DeclGroupRef D) override {
    for (DeclGroupRef::iterator I = D.begin(), E = D.end(); I != E; ++I)
      HandleTopLevelSingleDecl(*I);
    return true;
  }

  void HandleTopLevelSingleDecl(Decl *D);
};

} // namespace

void ASTViewer::HandleTopLevelSingleDecl(Decl *D) {
  if (isa<FunctionDecl>(D) || isa<ObjCMethodDecl>(D)) {
    D->print(llvm::errs());

    if (Stmt *Body = D->getBody()) {
      llvm::errs() << '\n';
      Body->viewAST();
      llvm::errs() << '\n';
    }
  }
}

std::unique_ptr<ASTConsumer> clang::CreateASTViewer() {
  return std::make_unique<ASTViewer>();
}

namespace clang {

std::unique_ptr<ASTConsumer> CreateAutoTypeDumper(ASTContext &Ctx) {

  return std::make_unique<AutoTypeDumperSemaConsumer>();
}

} // namespace clang