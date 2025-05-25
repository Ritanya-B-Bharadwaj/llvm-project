#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/CommandLine.h"
#include "FloatRewriter.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Command-line option to choose target float type
cl::opt<std::string> FloatTypeOpt("float-type", cl::desc("Specify the float type to convert to (e.g., __fp16 or __bf16)"), cl::value_desc("type"));

static cl::OptionCategory ToolCategory("fp16-accuracy-checker options");

class FloatConversionFrontendAction : public ASTFrontendAction {
public:
    FloatConversionFrontendAction() {}

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override {
        RewriterForAction.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<FloatRewriter>(RewriterForAction, FloatTypeOpt);
    }

    void EndSourceFileAction() override {
        Rewriter &R = RewriterForAction;
        FileID MainFileID = R.getSourceMgr().getMainFileID();
        const RewriteBuffer *RewriteBuf = R.getRewriteBufferFor(MainFileID);
        if (RewriteBuf)
            llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());
    }

private:
    Rewriter RewriterForAction;
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    return Tool.run(newFrontendActionFactory<FloatConversionFrontendAction>().get());
}

