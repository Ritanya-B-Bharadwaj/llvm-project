#ifndef GITHUB_FETCHER_H
#define GITHUB_FETCHER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "PRData.h"

using json = nlohmann::json;

class GitHubFetcher {
public:
    GitHubFetcher(const std::string& owner, const std::string& repo, const std::string& token);
    ~GitHubFetcher();
    
    // New interface methods to match main.cpp usage
    PRRepresentation getPR(const std::string& prNumber);
    std::vector<PRRepresentation> searchPRs(const std::string& query, int maxPages = 5);
    
    // Original methods
    json getOpenMPPRs(int page = 1);
    json getPRDetails(int pr_number);
    json getPRReviews(int pr_number);
    json getPRFiles(int pr_number);

private:
    std::string owner;
    std::string repo;
    std::string api_token;
    const std::string base_url = "https://api.github.com/repos/";
    // Add this helper function
    std::string getStringValue(const json& obj, const std::string& key, const std::string& defaultValue = "");
    
    // Helper functions
    json makeRequest(const std::string& endpoint);
    json searchRequest(const std::string& query, int page);
    std::string urlEncode(const std::string& value);  // Add this line
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif // GITHUB_FETCHER_H
