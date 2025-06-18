#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <vector>
#include <fstream>
#include <sstream>

using namespace clang;

class AnnotatorVisitor : public RecursiveASTVisitor<AnnotatorVisitor> {
public:
  explicit AnnotatorVisitor(ASTContext *Context, std::map<unsigned, std::vector<std::string>> &LineAnnotations)
      : Context(Context), LineAnnotations(LineAnnotations) {}

  bool VisitStmt(Stmt *S) {
    annotate(S, S->getStmtClassName());
    return true;
  }

  bool VisitDecl(Decl *D) {
    annotate(D, D->getDeclKindName());
    return true;
  }

private:
  ASTContext *Context;
  std::map<unsigned, std::vector<std::string>> &LineAnnotations;

  template <typename T>
  void annotate(T *Node, const std::string &KindName) {
    SourceLocation Loc = Node->getBeginLoc();
    if (Context->getSourceManager().isWrittenInMainFile(Loc)) {
      unsigned line = Context->getSourceManager().getSpellingLineNumber(Loc);
      LineAnnotations[line].push_back(KindName);
    }
  }
};

class AnnotatorConsumer : public ASTConsumer {
public:
  explicit AnnotatorConsumer(ASTContext *Context, std::map<unsigned, std::vector<std::string>> &LineAnnotations)
      : Visitor(Context, LineAnnotations) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  AnnotatorVisitor Visitor;
};

class AnnotatorAction : public ASTFrontendAction {
public:
  AnnotatorAction() {}

  bool BeginSourceFileAction(CompilerInstance &CI) override {
    SourceMgr = &CI.getSourceManager();
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override {
    return std::make_unique<AnnotatorConsumer>(&CI.getASTContext(), LineAnnotations);
  }

  void EndSourceFileAction() override {
  auto FileEntryRef = SourceMgr->getFileEntryRefForID(SourceMgr->getMainFileID());
  if (!FileEntryRef) {
    llvm::errs() << "Could not get file name.\n";
    return;
  }
  std::string filename = std::string(FileEntryRef->getName());

  std::ifstream input(filename);
  std::string line;
  unsigned lineno = 1;

  while (std::getline(input, line)) {
    llvm::outs() << lineno << ": " << line;
    auto it = LineAnnotations.find(lineno);
    if (it != LineAnnotations.end()) {
      llvm::outs() << "  // AST Nodes: [";
      for (size_t i = 0; i < it->second.size(); ++i) {
        llvm::outs() << "'" << it->second[i] << "'";
        if (i + 1 < it->second.size())
          llvm::outs() << ", ";
      }
      llvm::outs() << "]";
    }
    llvm::outs() << "\n";
    ++lineno;
  }
}



private:
  std::map<unsigned, std::vector<std::string>> LineAnnotations;
  clang::SourceManager *SourceMgr;
};


int main(int argc, const char **argv) {
  if (argc > 1) {
    std::ifstream file(argv[1]);
    if (!file) {
      llvm::errs() << "Error: Cannot open file " << argv[1] << "\n";
      return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    std::vector<std::string> args = {"clang-annotator", "-std=c++17"};
    clang::tooling::runToolOnCodeWithArgs(std::make_unique<AnnotatorAction>(), code, args, argv[1]);
  } else {
    llvm::errs() << "Usage: clang-annotator <source_file>\n";
  }
  return 0;
}
