#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <sstream>
#include <fstream>
#include <curl/curl.h>

using namespace clang;

std::map<unsigned, std::vector<std::string>> LineToAST;

class AnnotatorVisitor : public RecursiveASTVisitor<AnnotatorVisitor> {
public:
  explicit AnnotatorVisitor(ASTContext *Context) : Context(Context) {}

  template <typename T>
  void logNode(unsigned line, const std::string &type) {
    LineToAST[line].push_back(type);
  }

  bool VisitFunctionDecl(FunctionDecl *D) {
    if (Context->getSourceManager().isWrittenInMainFile(D->getLocation())) {
      logNode(getLine(D->getLocation()), "FunctionDecl");
    }
    return true;
  }

  bool VisitReturnStmt(ReturnStmt *S) {
    logNode(getLine(S->getBeginLoc()), "ReturnStmt");
    return true;
  }

  bool VisitBinaryOperator(BinaryOperator *B) {
    logNode(getLine(B->getOperatorLoc()), "BinaryOperator");
    return true;
  }

  bool VisitCallExpr(CallExpr *C) {
    logNode(getLine(C->getExprLoc()), "CallExpr");
    return true;
  }

private:
  ASTContext *Context;

  unsigned getLine(SourceLocation Loc) {
    return Context->getSourceManager().getSpellingLineNumber(Loc);
  }
};

class AnnotatorConsumer : public ASTConsumer {
public:
  explicit AnnotatorConsumer(ASTContext *Context) : Visitor(Context) {}
  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  AnnotatorVisitor Visitor;
};

// Utility for CURL
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
  size_t total = size * nmemb;
  output->append((char *)contents, total);
  return total;
}

// Sends AST node prompt to Grok (or OpenAI-compatible) API
std::string getExplanationsFromGrok(const std::map<unsigned, std::vector<std::string>> &LineToAST) {
  std::ostringstream prompt;
  prompt << "You are a C++ code explainer. Explain what each line does based on AST nodes.\n";
  for (const auto &entry : LineToAST) {
    prompt << "Line " << entry.first << ": [";
    for (size_t i = 0; i < entry.second.size(); ++i) {
      prompt << "'" << entry.second[i] << "'";
      if (i + 1 < entry.second.size()) prompt << ", ";
    }
    prompt << "]\n";
  }

  std::string json = R"({
    "model": "gpt-4",
    "messages": [{"role": "user", "content": ")" + prompt.str() + R"("}],
    "temperature": 0
  })";

  CURL *curl = curl_easy_init();
  std::string response;

  if (curl) {
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Authorization: Bearer xai-nb5d4poQBUcYxOSZVlVv58tLdaBYkYTnry7MMw1K7N6Ib9xb0iOoXwMTB7rLCedBIO0lwSrcdyyKphGj");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");  // Change if Grok uses different URL
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  return response;
}

class AnnotatorAction : public ASTFrontendAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
    this->SM = &CI.getSourceManager();
    return std::make_unique<AnnotatorConsumer>(&CI.getASTContext());
  }

  void EndSourceFileAction() override {
    std::ifstream file(SM->getFileEntryForID(SM->getMainFileID())->getName().str());
    std::string line;
    unsigned lineNum = 1;

    std::map<unsigned, std::string> codeLines;
    while (std::getline(file, line)) {
      codeLines[lineNum++] = line;
    }

    std::string explanationJSON = getExplanationsFromGrok(LineToAST);
    llvm::outs() << "Grok response:\n" << explanationJSON << "\n\n";

    for (const auto &entry : codeLines) {
      llvm::outs() << entry.first << ": " << entry.second << "\n";
      if (LineToAST.count(entry.first)) {
        llvm::outs() << "  AST Nodes: ";
        for (const auto &n : LineToAST[entry.first]) llvm::outs() << n << " ";
        llvm::outs() << "\n";
      }
    }
  }

private:
  SourceManager *SM;
};

int main(int argc, const char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCodeWithArgs(std::make_unique<AnnotatorAction>(), argv[1], {"-std=c++17"});
  } else {
    llvm::errs() << "Usage: clang-annotator <code>\n";
  }
  return 0;
}
