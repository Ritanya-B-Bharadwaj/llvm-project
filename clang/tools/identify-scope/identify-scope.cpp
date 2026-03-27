#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory IdentifyScopeCategory("identify-scope options");

static cl::opt<std::string> LineRangeOpt(
    "identify-scope-range",
    cl::desc("Comma-separated line ranges, e.g., 12-17,19-34"),
    cl::value_desc("line-ranges"),
    cl::cat(IdentifyScopeCategory)
);


class FunctionLocator : public RecursiveASTVisitor<FunctionLocator> {
    SourceManager *SM;
    std::vector<std::pair<unsigned, unsigned>> Ranges;

public:
    // To hold results: map<range, vector<function details>>
    std::map<std::pair<unsigned, unsigned>, std::vector<std::tuple<std::string, unsigned, unsigned>>> RangeToFunctions;

    void setSourceManager(SourceManager &sm) { SM = &sm; }

    void setRanges(const std::string &str) {
        std::stringstream ss(str);
        std::string part;
        while (std::getline(ss, part, ',')) {
            size_t dash = part.find('-');
            if (dash != std::string::npos) {
                unsigned start = std::stoi(part.substr(0, dash));
                unsigned end = std::stoi(part.substr(dash + 1));
                Ranges.emplace_back(start, end);
            }
        }
    }

    bool VisitFunctionDecl(FunctionDecl *FD) {
        if (!FD->hasBody()) return true;

        SourceLocation begin = FD->getBeginLoc();
        SourceLocation end = FD->getEndLoc();
        unsigned startLine = SM->getSpellingLineNumber(begin);
        unsigned endLine = SM->getSpellingLineNumber(end);

        for (const auto &[queryStart, queryEnd] : Ranges) {
            if (!(queryEnd < startLine || queryStart > endLine)) {
                RangeToFunctions[{queryStart, queryEnd}].emplace_back(
                    FD->getNameAsString(), startLine, endLine
                );
                break;
            }
        }

        return true;
    }
};


class FunctionAnalyzerConsumer : public ASTConsumer {
    FunctionLocator Locator;

public:
    void HandleTranslationUnit(ASTContext &Context) override {
        Locator.setSourceManager(Context.getSourceManager());
        Locator.setRanges(LineRangeOpt);
        Locator.TraverseDecl(Context.getTranslationUnitDecl());

        // Print results grouped by range
        for (const auto &entry : Locator.RangeToFunctions) {
            unsigned queryStart = entry.first.first;
            unsigned queryEnd = entry.first.second;

            llvm::outs() << "Range " << queryStart << "-" << queryEnd << ":\n";
            for (const auto &funcInfo : entry.second) {
                llvm::outs() << "Function: " << std::get<0>(funcInfo) << "\n";
                llvm::outs() << "Start Line: " << std::get<1>(funcInfo) << "\n";
                llvm::outs() << "End Line: " << std::get<2>(funcInfo) << "\n\n";
            }
        }
    }
};


class FunctionAnalyzerAction : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
        return std::make_unique<FunctionAnalyzerConsumer>();
    }
};


int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, IdentifyScopeCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<FunctionAnalyzerAction>().get());
}
