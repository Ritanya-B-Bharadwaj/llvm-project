#ifndef CLASSEXTENTCONSUMER_H
#define CLASSEXTENTCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace clang {

class ClassExtentVisitor : public RecursiveASTVisitor<ClassExtentVisitor> {
public:
  explicit ClassExtentVisitor(ASTContext &Ctx);

  bool VisitCXXRecordDecl(CXXRecordDecl *RD);

  void PrintResults();

private:
  ASTContext &Context;

  std::vector<std::string> ClassExtents;
  std::unordered_map<std::string, std::unordered_set<std::string>> InheritanceMap;
};

class ClassExtentConsumer : public ASTConsumer {
public:
  explicit ClassExtentConsumer(ASTContext &Ctx);

  void HandleTranslationUnit(ASTContext &Ctx) override;

private:
  ClassExtentVisitor Visitor;
};

class ClassExtentAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override;

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override;
};

} // namespace clang

#endif // CLASSEXTENTCONSUMER_H
