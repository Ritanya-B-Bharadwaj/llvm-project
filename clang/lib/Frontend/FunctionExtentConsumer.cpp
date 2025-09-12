#include "clang/Frontend/FunctionExtentConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Expr.h"

#include <set>
#include <vector>

using namespace clang;

class FunctionExtentVisitor : public RecursiveASTVisitor<FunctionExtentVisitor> {
public:
  explicit FunctionExtentVisitor(ASTContext &Ctx)
      : Context(Ctx) {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    if (!FD->hasBody())
      return true;

    SourceManager &SM = Context.getSourceManager();
    SourceLocation BeginLoc = FD->getSourceRange().getBegin();
    SourceLocation EndLoc = FD->getSourceRange().getEnd();

    PresumedLoc PBegin = SM.getPresumedLoc(BeginLoc);
    PresumedLoc PEnd = SM.getPresumedLoc(EndLoc);

    if (PBegin.isInvalid() || PEnd.isInvalid())
      return true;

    std::string FuncName = FD->getQualifiedNameAsString();
    std::string Location = std::string(PBegin.getFilename()) + ":" +
                           std::to_string(PBegin.getLine()) + "-" +
                           std::to_string(PEnd.getLine());

    FunctionExtents.emplace_back(FuncName + ":" + Location);
    CurrentFunction = FuncName;

    TraverseStmt(FD->getBody());

    return true;
  }

  bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *Callee = CE->getDirectCallee()) {
      std::string CalleeName = Callee->getQualifiedNameAsString();
      if (!CurrentFunction.empty()) {
        CallGraph.emplace(CurrentFunction, CalleeName);
      }
    }
    return true;
  }

  void PrintResults() {
    llvm::outs() << "Function Extents:\n";
    if (FunctionExtents.empty()) {
      llvm::outs() << "None\n";
    } else {
      for (const auto &Entry : FunctionExtents) {
        llvm::outs() << Entry << "\n";
      }
    }

    llvm::outs() << "\nCall Graphs:\n";
    if (CallGraph.empty()) {
      llvm::outs() << "None\n";
    } else {
      for (const auto &Edge : CallGraph) {
        llvm::outs() << Edge.first << " -> " << Edge.second << "\n";
      }
    }
  }

private:
  ASTContext &Context;
  std::string CurrentFunction;
  std::vector<std::string> FunctionExtents;
  std::set<std::pair<std::string, std::string>> CallGraph;
};

void FunctionExtentConsumer::Initialize(ASTContext &Ctx) {
  Context = &Ctx;
}

void FunctionExtentConsumer::HandleTranslationUnit(ASTContext &Ctx) {
  FunctionExtentVisitor Visitor(Ctx);
  Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  Visitor.PrintResults();
}

std::unique_ptr<ASTConsumer>
FunctionExtentAction::CreateASTConsumer(CompilerInstance &CI,
                                        llvm::StringRef) {
  return std::make_unique<FunctionExtentConsumer>();
}

bool FunctionExtentAction::ParseArgs(const CompilerInstance &CI,
                                     const std::vector<std::string> &) {
  return true;
}

// Register the plugin
static FrontendPluginRegistry::Add<FunctionExtentAction>
    X("dump-function-extents", "Dump lexical extents of functions and call graph");
