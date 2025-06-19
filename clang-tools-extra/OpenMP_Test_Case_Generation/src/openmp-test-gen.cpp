#include <iostream>
#include <sstream>
#include <regex>
#include <getopt.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct APIResponse {
    std::string data;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, APIResponse* response) {
        size_t totalSize = size * nmemb;
        response->data.append((char*)contents, totalSize);
        return totalSize;
    }
};

struct PRInfo {
    int number;
    std::string title;
    std::string body;
    std::string diff;
    std::vector<std::string> modifiedFiles;
    std::string specSection;
};

class OpenMPTestGenerator {
private:
    sqlite3* db;
    std::string groqAPIKey;
    std::string repoName;

public:
    OpenMPTestGenerator(const std::string& dbPath, const std::string& apiKey, 
                       const std::string& repo = "llvm/llvm-project")
        : groqAPIKey(apiKey), repoName(repo), db(nullptr) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            db = nullptr;
        }
    }

    ~OpenMPTestGenerator() {
        if (db) sqlite3_close(db);
    }

    PRInfo fetchPRInfo(int prNumber) {
        PRInfo info;
        info.number = prNumber;
        
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize CURL" << std::endl;
            return info;
        }
        
        // Fetch PR details
        std::string url = "https://api.github.com/repos/" + repoName + "/pulls/" + std::to_string(prNumber);
        APIResponse response;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, APIResponse::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "OpenMP-Test-Generator/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        // Add GitHub token if available
        struct curl_slist* headers = nullptr;
        const char* token = std::getenv("GITHUB_TOKEN");
        if (token) {
            std::string auth = "Authorization: token " + std::string(token);
            headers = curl_slist_append(headers, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            try {
                auto jsonData = json::parse(response.data);
                info.title = jsonData.value("title", "");
                info.body = jsonData.value("body", "");
                info.specSection = extractSpecSection(info.body);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing PR JSON: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "CURL error fetching PR: " << curl_easy_strerror(res) << std::endl;
        }
        
        // Fetch diff
        response.data.clear();
        curl_slist_free_all(headers);
        headers = curl_slist_append(nullptr, "Accept: application/vnd.github.v3.diff");
        if (token) {
            std::string auth = "Authorization: token " + std::string(token);
            headers = curl_slist_append(headers, auth.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            info.diff = response.data;
            info.modifiedFiles = extractModifiedFiles(info.diff);
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return info;
    }

    std::vector<std::string> extractModifiedFiles(const std::string& diff) {
        std::vector<std::string> files;
        std::istringstream stream(diff);
        std::string line;
        
        std::regex diffPattern(R"(diff --git a/(.+) b/.+)");
        std::smatch match;
        
        while (std::getline(stream, line)) {
            if (std::regex_search(line, match, diffPattern)) {
                files.push_back(match[1].str());
            }
        }
        return files;
    }

    std::string extractSpecSection(const std::string& body) {
        std::vector<std::regex> patterns = {
            std::regex(R"((?:spec(?:ification)?|summary|description)\s*[:\-]?\s*(.*?)(?=\n\n|\n#|$))", 
                      std::regex_constants::icase),
            std::regex(R"(## Summary\s*(.*?)(?=\n##|$))", std::regex_constants::icase),
            std::regex(R"(## Description\s*(.*?)(?=\n##|$))", std::regex_constants::icase)
        };
        
        for (const auto& pattern : patterns) {
            std::smatch match;
            if (std::regex_search(body, match, pattern)) {
                return match[1].str();
            }
        }
        
        size_t pos = body.find("\n\n");
        return pos != std::string::npos ? body.substr(0, pos) : body;
    }

    std::vector<std::string> querySimilarPatterns(const std::string& stage, int limit = 5) {
        std::vector<std::string> patterns;
        if (!db) return patterns;
        
        const char* sql = "SELECT pattern_data FROM test_patterns WHERE compiler_stage = ? ORDER BY complexity_score ASC LIMIT ?";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, stage.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, limit);
            
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const char* data = (const char*)sqlite3_column_text(stmt, 0);
                if (data) patterns.push_back(std::string(data));
            }
        }
        sqlite3_finalize(stmt);
        
        return patterns;
    }

    std::string generatePrompt(const PRInfo& prInfo, const std::vector<std::string>& patterns, 
                              const std::string& stage, int testNumber) {
        std::ostringstream prompt;
        prompt << "You are an expert OpenMP compiler test generator for LLVM/Clang.\n\n";
        prompt << "TASK: Generate a minimal test skeleton for a new OpenMP feature.\n\n";
        prompt << "PR INFORMATION:\n";
        prompt << "- Title: " << prInfo.title << "\n";
        prompt << "- Number: " << prInfo.number << "\n";
        prompt << "- Test Variant: " << testNumber << "\n";
        
        if (!prInfo.specSection.empty()) {
            prompt << "- Specification: " << prInfo.specSection << "\n";
        }
        
        if (!prInfo.modifiedFiles.empty()) {
            prompt << "- Modified Files: ";
            for (size_t i = 0; i < std::min(prInfo.modifiedFiles.size(), size_t(5)); ++i) {
                if (i > 0) prompt << ", ";
                prompt << prInfo.modifiedFiles[i];
            }
            prompt << "\n";
        }
        
        prompt << "\nTARGET STAGE: " << stage << "\n\n";
        
        if (!patterns.empty()) {
            prompt << "SIMILAR TEST PATTERNS:\n";
            for (size_t i = 0; i < std::min(patterns.size(), size_t(3)); ++i) {
                prompt << "Pattern " << (i + 1) << ":\n";
                try {
                    auto jsonData = json::parse(patterns[i]);
                    if (jsonData.contains("file_name")) {
                        prompt << "File: " << jsonData["file_name"] << "\n";
                    }
                    if (jsonData.contains("test_category")) {
                        prompt << "Category: " << jsonData["test_category"] << "\n";
                    }
                } catch (...) {
                    // Ignore JSON parsing errors
                }
                prompt << "\n";
            }
        }
        
        // Add variation instruction for multiple tests
        if (testNumber > 1) {
            prompt << "VARIATION REQUIREMENT:\n";
            prompt << "- Generate a different test variant from previous ones\n";
            prompt << "- Use different variable names, loop structures, or clause combinations\n";
            prompt << "- Focus on different aspects of the feature\n\n";
        }
        
        // Add stage-specific requirements
        if (stage == "sema") {
            prompt << "REQUIREMENTS:\n";
            prompt << "- Generate a semantic analysis test (Parse/Sema stage)\n";
            prompt << "- Include RUN line with: %clang_cc1 -fopenmp -fsyntax-only -verify %s\n";
            prompt << "- Add expected-error comments for invalid usage\n";
            prompt << "- Focus on clause validation and semantic correctness\n";
        } else if (stage == "codegen") {
            prompt << "REQUIREMENTS:\n";
            prompt << "- Generate a code generation test (CodeGen stage)\n";
            prompt << "- Include RUN line with: %clang_cc1 -fopenmp -emit-llvm %s -o - | FileCheck %s\n";
            prompt << "- Add CHECK patterns to verify LLVM IR output\n";
            prompt << "- Focus on runtime function calls and IR structure\n";
            prompt << "- Check for __kmpc_ function calls\n";
        } else {
            prompt << "REQUIREMENTS:\n";
            prompt << "- Generate a parsing test (Parse stage)\n";
            prompt << "- Include RUN line with: %clang_cc1 -fopenmp -fsyntax-only %s\n";
            prompt << "- Focus on syntax validation and basic parsing\n";
        }
        
        prompt << "\nOUTPUT: Provide ONLY the complete test file content. No explanations.\n";
        return prompt.str();
    }

    std::string callGroqAPI(const std::string& prompt) {
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        
        APIResponse response;
        std::string url = "https://api.groq.com/openai/v1/chat/completions";
        
        json payload = {
            {"model", "llama3-70b-8192"},
            {"max_tokens", 1024},
            {"temperature", 0.3}, // Slightly higher for variation
            {"messages", {
                {{"role", "system"}, {"content", "You are an expert OpenMP compiler test generator. Generate only test code, no explanations."}},
                {{"role", "user"}, {"content", prompt}}
            }}
        };
        
        std::string jsonStr = payload.dump();
        
        struct curl_slist* headers = nullptr;
        std::string auth = "Authorization: Bearer " + groqAPIKey;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, APIResponse::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
        
        CURLcode res = curl_easy_perform(curl);
        std::string result;
        
        if (res == CURLE_OK) {
            try {
                auto jsonData = json::parse(response.data);
                if (jsonData.contains("choices") && !jsonData["choices"].empty()) {
                    result = jsonData["choices"][0]["message"]["content"];
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing Groq API response: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "CURL error calling Groq API: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return result;
    }

    bool createOutputDirectory() {
        try {
            if (!fs::exists("outputs")) {
                fs::create_directory("outputs");
                std::cout << "Created 'outputs' directory" << std::endl;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error creating outputs directory: " << e.what() << std::endl;
            return false;
        }
    }

    bool generateMultipleTestSkeletons(int prNumber, const std::string& stage, int numTests) {
        if (!createOutputDirectory()) {
            return false;
        }

        std::cout << "Fetching PR #" << prNumber << " information..." << std::endl;
        
        PRInfo prInfo = fetchPRInfo(prNumber);
        if (prInfo.title.empty()) {
            std::cerr << "Failed to fetch PR information" << std::endl;
            return false;
        }
        
        std::cout << "PR Title: " << prInfo.title << std::endl;
        std::cout << "Modified files: " << prInfo.modifiedFiles.size() << std::endl;
        
        std::cout << "Querying similar patterns for " << stage << " stage..." << std::endl;
        auto patterns = querySimilarPatterns(stage);
        std::cout << "Found " << patterns.size() << " similar patterns" << std::endl;
        
        std::vector<std::string> generatedTests;
        bool allSuccessful = true;

        for (int i = 1; i <= numTests; ++i) {
            std::cout << "\nGenerating test skeleton " << i << "/" << numTests << " with Groq API..." << std::endl;
            
            std::string prompt = generatePrompt(prInfo, patterns, stage, i);
            std::string skeleton = callGroqAPI(prompt);
            
            if (skeleton.empty()) {
                std::cerr << "Failed to generate test skeleton " << i << std::endl;
                allSuccessful = false;
                continue;
            }

            // Create unique filename
            std::string filename = "outputs/pr_" + std::to_string(prNumber) + "_" + stage + "_test_" + std::to_string(i) + ".cpp";
            
            // Save to file
            std::ofstream file(filename);
            if (file.is_open()) {
                file << skeleton;
                file.close();
                std::cout << "✓ Test " << i << " saved to: " << filename << std::endl;
                generatedTests.push_back(filename);
            } else {
                std::cerr << "✗ Error saving test " << i << " to file: " << filename << std::endl;
                allSuccessful = false;
            }

            // Print to console as well
            std::cout << "\n" << std::string(60, '=') << std::endl;
            std::cout << "GENERATED TEST SKELETON " << i << " (" << stage << ")" << std::endl;
            std::cout << std::string(60, '=') << std::endl;
            std::cout << skeleton << std::endl;
            std::cout << std::string(60, '=') << std::endl;
        }

        // Summary
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "GENERATION SUMMARY" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total tests requested: " << numTests << std::endl;
        std::cout << "Tests successfully generated: " << generatedTests.size() << std::endl;
        std::cout << "All tests saved in: outputs/" << std::endl;
        
        if (!generatedTests.empty()) {
            std::cout << "\nGenerated files:" << std::endl;
            for (const auto& filename : generatedTests) {
                std::cout << "  - " << filename << std::endl;
            }
        }
        
        return allSuccessful;
    }
};

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --pr NUMBER          GitHub PR number (required)\n";
    std::cout << "  --stage STAGE        Test stage: parse, sema, codegen (default: sema)\n";
    std::cout << "  --db PATH            Path to pattern database (default: openmp_patterns.db)\n";
    std::cout << "  --groq-key KEY       Groq API key (or set GROQ_API_KEY env var)\n";
    std::cout << "  --repo REPO          GitHub repository (default: llvm/llvm-project)\n";
    std::cout << "  --num-tests N        Number of tests to generate (default: 1, max: 4)\n";
    std::cout << "  -h, --help           Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << progName << " --pr 67890 --stage codegen\n";
    std::cout << "  " << progName << " --pr 12345 --stage sema --num-tests 3\n";
    std::cout << "  GROQ_API_KEY=your_key " << progName << " --pr 67890 --num-tests 2\n\n";
    std::cout << "Environment Variables:\n";
    std::cout << "  GROQ_API_KEY         Groq API key (preferred method)\n";
    std::cout << "  GITHUB_TOKEN         GitHub token for API access (optional)\n\n";
    std::cout << "Output:\n";
    std::cout << "  All generated tests are saved in the 'outputs/' directory\n";
    std::cout << "  Files are named: pr_<NUMBER>_<STAGE>_test_<N>.cpp\n";
}

int main(int argc, char* argv[]) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    int prNumber = 0;
    std::string stage = "sema";
    std::string dbPath = "openmp_patterns.db";
    std::string groqKey;
    std::string repoName = "llvm/llvm-project";
    int numTests = 1; // Default number of tests
    
    static struct option long_options[] = {
        {"pr", required_argument, 0, 'p'},
        {"stage", required_argument, 0, 's'},
        {"db", required_argument, 0, 'd'},
        {"groq-key", required_argument, 0, 'g'},
        {"repo", required_argument, 0, 'r'},
        {"num-tests", required_argument, 0, 'n'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "p:s:d:g:r:n:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'p': prNumber = std::atoi(optarg); break;
            case 's': stage = optarg; break;
            case 'd': dbPath = optarg; break;
            case 'g': groqKey = optarg; break;
            case 'r': repoName = optarg; break;
            case 'n': 
                numTests = std::atoi(optarg);
                if (numTests < 1) numTests = 1;
                if (numTests > 4) numTests = 4;
                break;
            case 'h': printUsage(argv[0]); curl_global_cleanup(); return 0;
            case '?': printUsage(argv[0]); curl_global_cleanup(); return 1;
        }
    }
    
    if (prNumber == 0) {
        std::cerr << "Error: PR number is required (--pr)" << std::endl;
        printUsage(argv[0]);
        curl_global_cleanup();
        return 1;
    }
    
    // Get Groq API key from environment if not provided
    if (groqKey.empty()) {
        const char* envKey = std::getenv("GROQ_API_KEY");
        if (envKey) {
            groqKey = envKey;
            std::cout << "Using GROQ_API_KEY from environment variable" << std::endl;
        } else {
            std::cerr << "Error: Groq API key is required (--groq-key or GROQ_API_KEY env var)" << std::endl;
            printUsage(argv[0]);
            curl_global_cleanup();
            return 1;
        }
    }
    
    if (stage != "parse" && stage != "sema" && stage != "codegen") {
        std::cerr << "Error: Invalid test stage. Use: parse, sema, or codegen" << std::endl;
        printUsage(argv[0]);
        curl_global_cleanup();
        return 1;
    }
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  PR Number: " << prNumber << std::endl;
    std::cout << "  Stage: " << stage << std::endl;
    std::cout << "  Number of tests: " << numTests << std::endl;
    std::cout << "  Database: " << dbPath << std::endl;
    std::cout << "  Repository: " << repoName << std::endl;
    std::cout << std::endl;
    
    OpenMPTestGenerator generator(dbPath, groqKey, repoName);
    bool success = generator.generateMultipleTestSkeletons(prNumber, stage, numTests);
    
    curl_global_cleanup();
    return success ? 0 : 1;
}
