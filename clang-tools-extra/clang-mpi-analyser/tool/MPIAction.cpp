#include "MPIAction.h"
#include "MPIConsumer.h"
#include "llvm/Support/raw_ostream.h"

std::unique_ptr<clang::ASTConsumer> MPIAction::CreateASTConsumer(
    clang::CompilerInstance &CI, llvm::StringRef) {
    // llvm::outs() << "🔧 Creating ASTConsumer\n";
    return std::make_unique<MPIConsumer>(CI.getASTContext(), AnalyzeScatterGather);
}

