#include "MPIAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;

static llvm::cl::OptionCategory MPIAnalyserCategory("mpi-analyser options");

static llvm::cl::opt<bool> AnalyzeMPIScatterGather(
    "analyze-mpi-scatter-gather", 
    llvm::cl::desc("Enable identification of MPI scatter/gather patterns."), 
    llvm::cl::cat(MPIAnalyserCategory), 
    llvm::cl::init(false) 
);

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MPIAnalyserCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    ClangTool Tool(ExpectedParser->getCompilations(), ExpectedParser->getSourcePathList());

    MPIActionFactory Factory(AnalyzeMPIScatterGather);
    int Result = Tool.run(&Factory);

    return Result;
}
