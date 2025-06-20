#include "clang/Frontend/FunctionExtentConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

void FunctionExtentConsumer::Initialize(ASTContext &Ctx) {
  Context = &Ctx;
}

void FunctionExtentConsumer::HandleTranslationUnit(ASTContext &Ctx) {
  SourceManager &SM = Ctx.getSourceManager();

  for (Decl *D : Ctx.getTranslationUnitDecl()->decls()) {
    if (auto *FD = dyn_cast<FunctionDecl>(D)) {
      if (!FD->hasBody())
        continue;

      SourceLocation BeginLoc = FD->getSourceRange().getBegin();
      SourceLocation EndLoc = FD->getSourceRange().getEnd();

      PresumedLoc PBegin = SM.getPresumedLoc(BeginLoc);
      PresumedLoc PEnd = SM.getPresumedLoc(EndLoc);

      if (PBegin.isInvalid() || PEnd.isInvalid())
        continue;

      llvm::outs() << FD->getQualifiedNameAsString()
                   << ":" << PBegin.getFilename()
                   << ":" << PBegin.getLine()
                   << "-" << PEnd.getLine()
                   << "\n";
      // <QualifiedFunctionName>:<Filename>:<StartLine>-<EndLine>
    }
  }
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
    X("dump-function-extents", "Dump lexical extents of functions");
