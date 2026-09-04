#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class OMPPragmaPrinter : public MatchFinder::MatchCallback {
public:
  void run(const MatchFinder::MatchResult &Result) override {
    if (const auto *Directive = Result.Nodes.getNodeAs<OMPParallelDirective>("ompDirective")) {
      FullSourceLoc FullLocation = Result.Context->getFullLoc(Directive->getBeginLoc());
      if (FullLocation.isValid()) {
        llvm::outs() << "OpenMP Construct Found: OMPParallelDirective at "
                     << FullLocation.getFileEntry()->getName() << ":"
                     << FullLocation.getSpellingLineNumber() << ":"
                     << FullLocation.getSpellingColumnNumber() << "\n";
      }
    }
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, llvm::cl::GeneralCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  OMPPragmaPrinter Printer;
  MatchFinder Finder;

  Finder.addMatcher(ompParallelDirective().bind("ompDirective"), &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}

