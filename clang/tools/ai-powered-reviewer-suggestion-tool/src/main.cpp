#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "GitHubFetcher.h"
#include "PRParser.h"
#include "ReviewerSuggester.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " --pr <PR_NUMBER> --token <GITHUB_TOKEN> [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --pr <number>        PR number to analyze" << std::endl;
    std::cout << "  --token <token>      GitHub API token" << std::endl;
    std::cout << "  --max-pages <num>    Maximum pages of historical PRs to fetch (default: unlimited, GitHub max ~33)" << std::endl;
    std::cout << "  --top-reviewers <num> Number of top reviewers to suggest (default: 5)" << std::endl;
    std::cout << "  --model-path <path>  Path to ONNX model file (default: models/all-MiniLM-L6-v2.onnx)" << std::endl;
    std::cout << "  --vocab-path <path>  Path to vocabulary file (default: models/vocab.txt)" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== AI-Powered Reviewer Suggestion Tool for OpenMP PRs ===" << std::endl;
    
    // Default values - REMOVE CAPS
    std::string prNumber;
    std::string githubToken;
    int maxPages = 2;  // -1 means unlimited (up to GitHub's limit)
    int topReviewers = 5;
    std::string modelPath = "models/all-MiniLM-L6-v2.onnx";
    std::string vocabPath = "models/vocab.txt";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--pr" && i + 1 < argc) {
            prNumber = argv[++i];
        } else if (arg == "--token" && i + 1 < argc) {
            githubToken = argv[++i];
        } else if (arg == "--max-pages" && i + 1 < argc) {
            maxPages = std::stoi(argv[++i]);
        } else if (arg == "--top-reviewers" && i + 1 < argc) {
            topReviewers = std::stoi(argv[++i]);
        } else if (arg == "--model-path" && i + 1 < argc) {
            modelPath = argv[++i];
        } else if (arg == "--vocab-path" && i + 1 < argc) {
            vocabPath = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Validate required arguments
    if (prNumber.empty()) {
        std::cerr << "Error: PR number is required" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    if (githubToken.empty()) {
        const char* envToken = std::getenv("GITHUB_TOKEN");
        if (envToken) {
            githubToken = envToken;
        } else {
            std::cerr << "Error: GitHub token is required (use --token or set GITHUB_TOKEN environment variable)" << std::endl;
            return 1;
        }
    }
    
    // Warn about unlimited pages
    if (maxPages == -1) {
        std::cout << "WARNING: Unlimited pages requested. This may take a long time and hit API rate limits." << std::endl;
        std::cout << "GitHub Search API is limited to ~1000 results maximum (~33 pages at 30 per page)." << std::endl;
        maxPages = 1000; // Set to a very high number
    }
    
    try {
        // Initialize components
        std::cout << "Initializing GitHub fetcher..." << std::endl;
        GitHubFetcher fetcher("llvm", "llvm-project", githubToken);
        
        std::cout << "Initializing PR parser with model: " << modelPath << std::endl;
        PRParser parser(modelPath, vocabPath);
        
        std::cout << "Initializing reviewer suggester..." << std::endl;
        ReviewerSuggester suggester;
        
        // Test embedding generation
        std::cout << "Testing embedding generation..." << std::endl;
        std::string testText = "This is a test sentence for OpenMP parallel processing.";
        auto testEmbedding = parser.generateEmbedding(testText);
        std::cout << "Test embedding size: " << testEmbedding.size() << std::endl;
        
        if (testEmbedding.size() == 0 || std::all_of(testEmbedding.begin(), testEmbedding.end(), [](float f){ return f == 0.0f; })) {
            std::cerr << "Warning: Test embedding generation failed or returned zero vector" << std::endl;
        } else {
            std::cout << "Test embedding generated successfully!" << std::endl;
        }
        
        // Get current PR
        std::cout << "Fetching PR #" << prNumber << "..." << std::endl;
        auto currentPR = fetcher.getPR(prNumber);
        if (currentPR.title.empty()) {
            std::cerr << "Error: Could not fetch PR #" << prNumber << std::endl;
            return 1;
        }
        
        std::cout << "Current PR: " << currentPR.title << std::endl;
        
        // Fetch historical PRs with unlimited pages
        std::cout << "Fetching historical OpenMP PRs (unlimited pages, up to GitHub limit)..." << std::endl;
        std::string query = "repo:llvm/llvm-project is:pr  label:clang:openmp in:title,body";
        
        auto historicalPRs = fetcher.searchPRs(query, maxPages);
        
        std::cout << "Found " << historicalPRs.size() << " historical PRs" << std::endl;
        
        if (historicalPRs.empty()) {
            std::cerr << "Warning: No historical PRs found" << std::endl;
            return 1;
        }
        
        // Generate suggestions
        std::cout << "Generating reviewer suggestions..." << std::endl;
        auto suggestions = suggester.suggestReviewers(currentPR, historicalPRs, parser, topReviewers);
        
        // Display results
        std::cout << "\n=== Suggested Reviewers for PR #" << prNumber << " ===" << std::endl;
        std::cout << "PR Title: " << currentPR.title << std::endl;
        std::cout << "PR Author: " << currentPR.author << std::endl;
        std::cout << "\nTop " << suggestions.size() << " Reviewer Suggestions:" << std::endl;
        
        for (size_t i = 0; i < suggestions.size(); i++) {
            std::cout << (i + 1) << ". " << suggestions[i].reviewer
                      << " (Score: " << std::fixed << std::setprecision(3) << suggestions[i].score << ")" << std::endl;
        }
        
        std::cout << "\nAnalysis complete!" << std::endl;
        std::cout << "Total API requests made: ~" << (historicalPRs.size() * 3 + 3) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
