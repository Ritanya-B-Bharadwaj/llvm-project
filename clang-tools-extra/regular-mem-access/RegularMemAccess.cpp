// RegularMemAccess.cpp
// Copy of tool-template/ToolTemplate.cpp with names updated

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory RegularMemAccessCategory("regular-mem-access options");
static cl::opt<bool> AnalyzeRegularMemoryAccess(
    "analyze-regular-memory-access",
    cl::desc("Enable detection of regular memory access patterns."),
    cl::cat(RegularMemAccessCategory));

class RegularMemAccessCallback : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    const auto *For = Result.Nodes.getNodeAs<ForStmt>("forLoop");
    const auto *ArraySub = Result.Nodes.getNodeAs<ArraySubscriptExpr>("arrayAccess");
    const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("funcDecl");
    if (!For || !ArraySub || !Func)
      return;

    // Check if the array index is the loop variable
    const auto *Idx = dyn_cast<DeclRefExpr>(ArraySub->getIdx());
    if (!Idx)
      return;
    const VarDecl *LoopVar = nullptr;
    if (const auto *Init = For->getInit()) {
      if (const auto *DS = dyn_cast<DeclStmt>(Init)) {
        if (DS->isSingleDecl()) {
          if (const auto *VD = dyn_cast<VarDecl>(DS->getSingleDecl())) {
            LoopVar = VD;
          }
        }
      }
    }
    if (!LoopVar)
      return;
    if (Idx->getDecl() != LoopVar)
      return;

    // Emit diagnostic: regular (sequential) access detected
    FullSourceLoc FullLocation = Result.Context->getFullLoc(ArraySub->getBeginLoc());
    if (FullLocation.isValid()) {
      llvm::outs() << "Function '" << Func->getNameAsString() << "' has regular memory access patterns:\n";
      llvm::outs() << "- Sequential access detected at line " << FullLocation.getSpellingLineNumber() << "\n";
    }
  }
};

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, RegularMemAccessCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

  if (!AnalyzeRegularMemoryAccess) {
    llvm::errs() << "Use -analyze-regular-memory-access to enable analysis.\n";
    return 1;
  }

  RegularMemAccessCallback Callback;
  MatchFinder Finder;
  // Match for-loops with array accesses using the loop variable as index
  Finder.addMatcher(
    functionDecl(
      hasDescendant(
        forStmt(
          hasDescendant(
            arraySubscriptExpr(
              hasIndex(declRefExpr().bind("loopVar"))
            ).bind("arrayAccess")
          )
        ).bind("forLoop")
      )
    ).bind("funcDecl"),
    &Callback);

  return Tool.run(newFrontendActionFactory(&Finder).release());
}
