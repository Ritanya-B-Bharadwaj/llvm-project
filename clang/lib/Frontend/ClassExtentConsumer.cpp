#include "clang/Frontend/ClassExtentConsumer.h"
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

ClassExtentVisitor::ClassExtentVisitor(ASTContext &Ctx) : Context(Ctx) {}

bool ClassExtentVisitor::VisitCXXRecordDecl(CXXRecordDecl *RD) {
  if (RD->isThisDeclarationADefinition()) {
    SourceManager &SM = Context.getSourceManager();
    SourceLocation BeginLoc = RD->getSourceRange().getBegin();
    SourceLocation EndLoc = RD->getSourceRange().getEnd();

    PresumedLoc PBegin = SM.getPresumedLoc(BeginLoc);
    PresumedLoc PEnd = SM.getPresumedLoc(EndLoc);

    if (PBegin.isInvalid() || PEnd.isInvalid())
      return true;

    std::string ClassName = RD->getQualifiedNameAsString();

    ClassExtents.push_back(ClassName + ":" + std::string(PBegin.getFilename()) +
                           ":" + std::to_string(PBegin.getLine()) + "-" +
                           std::to_string(PEnd.getLine()));

    for (const auto &Base : RD->bases()) {
      QualType BaseType = Base.getType();
      if (const CXXRecordDecl *BaseDecl = BaseType->getAsCXXRecordDecl()) {
        InheritanceMap[BaseDecl->getQualifiedNameAsString()].insert(ClassName);
      }
    }
  }

  return true;
}

void ClassExtentVisitor::PrintResults() {
  llvm::outs() << "Class Extents:\n";
  if (ClassExtents.empty()) {
    llvm::outs() << "None\n";
  } else {
    for (const auto &Entry : ClassExtents) {
      llvm::outs() << Entry << "\n";
    }
  }

  llvm::outs() << "\nInheritance Tree:\n";
  if (InheritanceMap.empty()) {
    llvm::outs() << "None\n";
  } else {
    for (const auto &Pair : InheritanceMap) {
      for (const auto &Derived : Pair.second) {
        llvm::outs() << Pair.first << " <- " << Derived << "\n";
      }
    }
  }
}

ClassExtentConsumer::ClassExtentConsumer(ASTContext &Ctx) : Visitor(Ctx) {}

void ClassExtentConsumer::HandleTranslationUnit(ASTContext &Ctx) {
  Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  Visitor.PrintResults();
}

std::unique_ptr<ASTConsumer>
ClassExtentAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) {
  return std::make_unique<ClassExtentConsumer>(CI.getASTContext());
}

bool ClassExtentAction::ParseArgs(const CompilerInstance &CI,
                                  const std::vector<std::string> &) {
  return true;
}

static FrontendPluginRegistry::Add<ClassExtentAction>
    X("dump-class-extents", "Dump class extents and inheritance tree");
