#ifndef LLVM_CLANG_FRONTEND_FUNCTIONEXTENTPLUGIN_H
#define LLVM_CLANG_FRONTEND_FUNCTIONEXTENTPLUGIN_H

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include <memory>

namespace clang {

class FunctionExtentConsumer : public ASTConsumer {
    ASTContext *Context = nullptr;
public:
  void Initialize(ASTContext &Ctx) override;
  void HandleTranslationUnit(ASTContext &Ctx) override;
};

class FunctionExtentAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override;
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &) override;
};

} // namespace clang

#endif // LLVM_CLANG_FRONTEND_FUNCTIONEXTENTPLUGIN_H
