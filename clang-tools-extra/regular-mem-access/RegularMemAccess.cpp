//===--- RegularMemAccess.cpp - Regular Memory Access Analyzer -----------===//
//
//  Detects functions with regular (sequential) memory access patterns.
//
//  Usage:
//    regular-mem-access -analyze-regular-memory-access <source files>
//    regular-mem-access -analyze-regular-memory-access-llvm-ir <source files>
//
//===----------------------------------------------------------------------===//

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Module.h"
#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory RegularMemAccessCategory("regular-mem-access options");
static cl::opt<bool> AnalyzeRegularMemoryAccess(
    "analyze-regular-memory-access",
    cl::desc("Enable detection of regular memory access patterns."),
    cl::cat(RegularMemAccessCategory));

static cl::opt<bool> AnalyzeRegularMemoryAccessLLVMIR(
    "analyze-regular-memory-access-llvm-ir",
    cl::desc("Enable detection of regular memory access patterns in LLVM IR."),
    cl::cat(RegularMemAccessCategory));

namespace {
class RegularMemAccessCallback : public MatchFinder::MatchCallback {
public:
  void run(const MatchFinder::MatchResult &Result) override {
    const auto *Access = Result.Nodes.getNodeAs<ArraySubscriptExpr>("arrayAccess");
    const auto *Loop = Result.Nodes.getNodeAs<ForStmt>("forLoop");
    const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("func");
    if (!Access || !Loop || !Func) return;

    // Check if the index is a DeclRefExpr to the loop variable, or a linear function of it
    const Expr *Idx = Access->getIdx()->IgnoreParenImpCasts();
    const VarDecl *LoopVar = nullptr;
    if (const auto *Init = Loop->getInit()) {
      if (const auto *DS = dyn_cast<DeclStmt>(Init)) {
        if (const auto *VD = dyn_cast<VarDecl>(DS->getSingleDecl())) {
          LoopVar = VD;
        }
      }
    }
    bool isRegular = false;
    // Case 1: arr[i]
    if (const auto *IdxRef = dyn_cast<DeclRefExpr>(Idx)) {
      if (LoopVar && IdxRef->getDecl() == LoopVar) {
        isRegular = true;
      }
    }
    // Case 2: arr[i + c] or arr[i - c]
    else if (const auto *BO = clang::dyn_cast<clang::BinaryOperator>(Idx)) {
      if ((BO->getOpcode() == BO_Add || BO->getOpcode() == BO_Sub)) {
        const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();
        const Expr *RHS = BO->getRHS()->IgnoreParenImpCasts();
        if ((isa<DeclRefExpr>(LHS) && cast<DeclRefExpr>(LHS)->getDecl() == LoopVar && isa<IntegerLiteral>(RHS)) ||
            (isa<DeclRefExpr>(RHS) && cast<DeclRefExpr>(RHS)->getDecl() == LoopVar && isa<IntegerLiteral>(LHS))) {
          isRegular = true;
        }
      }
    }
    // Case 3: arr[i * stride + c] or arr[stride * i + c]
    else if (const auto *BO = clang::dyn_cast<clang::BinaryOperator>(Idx)) {
      if (BO->getOpcode() == BO_Add || BO->getOpcode() == BO_Sub) {
        const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();
        const Expr *RHS = BO->getRHS()->IgnoreParenImpCasts();
        // Check for (i * stride) + c or c + (i * stride)
        const clang::BinaryOperator *InnerBO = nullptr;
        const Expr *ConstExpr = nullptr;
        if ((InnerBO = clang::dyn_cast<clang::BinaryOperator>(LHS)) && (InnerBO->getOpcode() == BO_Mul) && isa<IntegerLiteral>(RHS))
          ConstExpr = RHS;
        else if ((InnerBO = clang::dyn_cast<clang::BinaryOperator>(RHS)) && (InnerBO->getOpcode() == BO_Mul) && isa<IntegerLiteral>(LHS))
          ConstExpr = LHS;
        if (InnerBO && ConstExpr) {
          const Expr *MulLHS = InnerBO->getLHS()->IgnoreParenImpCasts();
          const Expr *MulRHS = InnerBO->getRHS()->IgnoreParenImpCasts();
          if ((isa<DeclRefExpr>(MulLHS) && cast<DeclRefExpr>(MulLHS)->getDecl() == LoopVar && isa<IntegerLiteral>(MulRHS)) ||
              (isa<DeclRefExpr>(MulRHS) && cast<DeclRefExpr>(MulRHS)->getDecl() == LoopVar && isa<IntegerLiteral>(MulLHS))) {
            isRegular = true;
          }
        }
      }
    }
    if (isRegular) {
      FullSourceLoc FullLoc(Access->getBeginLoc(), *Result.SourceManager);
      llvm::outs() << "Function '" << Func->getNameAsString() << "' has regular memory access patterns:\n";
      llvm::outs() << "- Sequential/linear access detected at line " << FullLoc.getSpellingLineNumber() << "\n\n";
    }
    // Otherwise, do not emit diagnostic (irregular or unknown pattern)
  }
};

// LLVM IR analysis
class RegularMemAccessIRAction : public clang::EmitLLVMOnlyAction {
public:
  void EndSourceFileAction() override {
    std::unique_ptr<llvm::Module> ModulePtr = takeModule();
    llvm::Module *Module = ModulePtr.get();
    if (!Module) return;
    for (auto &F : *Module) {
      if (F.isDeclaration()) continue;
      for (auto &BB : F) {
        for (auto &I : BB) {
          if (auto *GEP = llvm::dyn_cast<llvm::GetElementPtrInst>(&I)) {
            if (GEP->getNumIndices() == 1) {
              llvm::Value *Idx = GEP->getOperand(GEP->getNumOperands() - 1);
              if (llvm::isa<llvm::Argument>(Idx) || llvm::isa<llvm::PHINode>(Idx)) {
                llvm::errs() << "[LLVM IR] Function '" << F.getName() << "' has regular memory access patterns (sequential/linear GEP)\n";
              }
            }
          }
        }
      }
    }
  }
};
} // namespace

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, RegularMemAccessCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

  if (AnalyzeRegularMemoryAccessLLVMIR) {
    return Tool.run(newFrontendActionFactory<RegularMemAccessIRAction>().get());
  }
  if (!AnalyzeRegularMemoryAccess)
    return Tool.run(newFrontendActionFactory<SyntaxOnlyAction>().get());

  RegularMemAccessCallback Callback;
  MatchFinder Finder;
  // Match for-loops with array accesses indexed by the loop variable
  Finder.addMatcher(
    functionDecl(
      isDefinition(),
      hasDescendant(
        forStmt(
          hasBody(
            hasDescendant(
              arraySubscriptExpr(
                hasIndex(ignoringParenImpCasts(declRefExpr().bind("idx")))
              ).bind("arrayAccess")
            )
          )
        ).bind("forLoop")
      )
    ).bind("func"),
    &Callback);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
