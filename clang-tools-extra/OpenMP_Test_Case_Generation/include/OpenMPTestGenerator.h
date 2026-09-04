#ifndef OPENMP_TEST_GENERATOR_H
#define OPENMP_TEST_GENERATOR_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
struct sqlite3;

namespace openmp_test_gen {

struct APIResponse {
    std::string data;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, APIResponse* response);
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
                       const std::string& repo = "llvm/llvm-project");
    ~OpenMPTestGenerator();

    // Core functionality
    PRInfo fetchPRInfo(int prNumber);
    std::vector<std::string> querySimilarPatterns(const std::string& stage, int limit = 5);
    std::string generatePrompt(const PRInfo& prInfo, const std::vector<std::string>& patterns, 
                              const std::string& stage, int testNumber);
    std::string callGroqAPI(const std::string& prompt);
    bool createOutputDirectory();
    bool generateMultipleTestSkeletons(int prNumber, const std::string& stage, int numTests);

private:
    // Helper methods
    std::vector<std::string> extractModifiedFiles(const std::string& diff);
    std::string extractSpecSection(const std::string& body);
};

} // namespace openmp_test_gen

#endif // OPENMP_TEST_GENERATOR_H
