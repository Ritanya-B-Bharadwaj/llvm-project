#include "llvm/Support/CommandLine.h"
#include <iostream>
#include "DiffFetcher.h"
#include "SpecRetriever.h"
#include "SummaryGenerator.h"

using namespace llvm;

cl::opt<int> PRNumber("pr", cl::desc("GitHub PR number"), cl::Required);
cl::opt<std::string> Repo("repo", cl::desc("GitHub repository (e.g., llvm/llvm-project)"), cl::Required);

int main(int argc, const char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "OpenMP PR Summarizer\n");

    DiffFetcher fetcher;
    PRData pr = fetcher.fetchPR(PRNumber, Repo);
    if (pr.title.empty()) {
        std::cerr << "Failed to fetch PR data.\n";
        return 1;
    }

    std::cout << "Title: " << pr.title << "\n";

    // ✅ Use SpecRetriever
    SpecRetriever retriever("llvm/tools/omp-pr-summary/data/openmp_spec.txt");
    std::vector<std::string> candidates = {
    "parallel", "target", "task", "schedule", "teams", "atomic", "critical"
};

std::string foundSnippet;
for (const auto& keyword : candidates) {
    foundSnippet = retriever.searchSpec(keyword);
    if (foundSnippet.find("🔍") == std::string::npos) {
        std::cout << "Matched keyword: " << keyword << "\n";
        break;
    }
}
if (foundSnippet.find("🔍") != std::string::npos)
    foundSnippet = "No OpenMP-relevant keyword found in PR title/body.";
 // You can enhance this with dynamic keyword extraction

    // ✅ Generate Markdown summary
    SummaryGenerator generator;
    std::string summary = generator.generate(pr, foundSnippet);


    std::cout << "\n--- Generated Summary ---\n";
    std::cout << summary << "\n";

    return 0;
}
