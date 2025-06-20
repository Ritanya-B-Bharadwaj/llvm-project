#include "GitHubFetcher.h"
#include <curl/curl.h>
#include <iostream>
#include <set>
#include <algorithm>
#include <thread>
#include <chrono>

// Add this helper function after the includes
std::string GitHubFetcher::getStringValue(const json& obj, const std::string& key, const std::string& defaultValue) {
    if (!obj.contains(key)) {
        return defaultValue;
    }
    
    const auto& value = obj[key];
    if (value.is_null()) {
        return defaultValue;
    }
    
    if (value.is_string()) {
        return value.get<std::string>();
    }
    
    return defaultValue;
}


GitHubFetcher::GitHubFetcher(const std::string& owner, const std::string& repo, const std::string& token)
    : owner(owner), repo(repo), api_token(token) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

GitHubFetcher::~GitHubFetcher() {
    curl_global_cleanup();
}

size_t GitHubFetcher::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Add this new function for URL encoding
std::string GitHubFetcher::urlEncode(const std::string& value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return value;
    }
    
    char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    if (!encoded) {
        curl_easy_cleanup(curl);
        return value;
    }
    
    std::string result(encoded);
    curl_free(encoded);
    curl_easy_cleanup(curl);
    
    return result;
}

json GitHubFetcher::makeRequest(const std::string& endpoint) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize curl");
    }
    
    std::string url = base_url + owner + "/" + repo + "/" + endpoint;
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: token " + api_token).c_str());
    headers = curl_slist_append(headers, "User-Agent: Reviewer-Suggester");
    headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }
    
    // Check HTTP response code
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    if (http_code != 200) {
        std::cerr << "HTTP request failed with code " << http_code << std::endl;
        std::cerr << "Response: " << readBuffer << std::endl;
        throw std::runtime_error("HTTP request failed with code " + std::to_string(http_code));
    }
    
    // Parse JSON response
    try {
        return json::parse(readBuffer);
    } catch (const json::parse_error& e) {
        std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
        std::cerr << "Response: " << readBuffer << std::endl;
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
    }
}

json GitHubFetcher::searchRequest(const std::string& query, int page) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize curl");
    }
    
    // Use maximum per_page (100) to reduce API calls
    std::string encoded_query = urlEncode(query);
    std::string url = "https://api.github.com/search/issues?q=" + encoded_query +
                     "&per_page=100&sort=created&order=desc&page=" + std::to_string(page);
    
    std::cout << "Search URL: " << url << std::endl;
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: token " + api_token).c_str());
    headers = curl_slist_append(headers, "User-Agent: Reviewer-Suggester");
    headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);  // Increase timeout
    
    res = curl_easy_perform(curl);
    
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    if (res != CURLE_OK) {
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }
    
    // Handle rate limiting
    if (http_code == 403) {
        std::cerr << "Rate limit hit. Response: " << readBuffer << std::endl;
        throw std::runtime_error("GitHub API rate limit exceeded");
    }
    
    if (http_code != 200) {
        std::cerr << "Search API returned HTTP " << http_code << std::endl;
        std::cerr << "Response: " << readBuffer << std::endl;
        throw std::runtime_error("GitHub Search API returned HTTP " + std::to_string(http_code));
    }
    
    try {
        return json::parse(readBuffer);
    } catch (const json::parse_error& e) {
        std::cerr << "Failed to parse search response: " << e.what() << std::endl;
        std::cerr << "Response: " << readBuffer << std::endl;
        throw std::runtime_error("Failed to parse search response");
    }
}

// Rest of the methods remain the same...
PRRepresentation GitHubFetcher::getPR(const std::string& prNumber) {
    std::string endpoint = "pulls/" + prNumber;
    json prData = makeRequest(endpoint);
    
    PRRepresentation pr;
    pr.number = std::stoi(prNumber);
    pr.title = getStringValue(prData, "title", "No title");
    pr.body = getStringValue(prData, "body", "No description");
    
    // Handle nested user object safely
    if (prData.contains("user") && !prData["user"].is_null()) {
        pr.author = getStringValue(prData["user"], "login", "unknown");
    } else {
        pr.author = "unknown";
    }
    
    pr.created_at = getStringValue(prData, "created_at", "");
    
    // Get files
    try {
        json filesData = makeRequest("pulls/" + prNumber + "/files");
        for (const auto& file : filesData) {
            std::string filename = getStringValue(file, "filename", "");
            if (!filename.empty()) {
                pr.changed_files.push_back(filename);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error fetching PR files: " << e.what() << std::endl;
    }
    
    // Get reviewers
    try {
        json reviewsData = makeRequest("pulls/" + prNumber + "/reviews");
        std::set<std::string> unique_reviewers;
        for (const auto& review : reviewsData) {
            if (review.contains("user") && !review["user"].is_null()) {
                std::string reviewer = getStringValue(review["user"], "login", "");
                if (!reviewer.empty()) {
                    unique_reviewers.insert(reviewer);
                }
            }
        }
        pr.reviewers.assign(unique_reviewers.begin(), unique_reviewers.end());
    } catch (const std::exception& e) {
        std::cerr << "Error fetching PR reviews: " << e.what() << std::endl;
    }
    
    return pr;
}


std::vector<PRRepresentation> GitHubFetcher::searchPRs(const std::string& query, int maxPages) {
    std::vector<PRRepresentation> prs;
    int totalApiCalls = 0;
    
    for (int page = 1; page <= maxPages; page++) {
        std::cout << "Fetching page " << page << " (up to 100 PRs per page)..." << std::endl;
        
        try {
            json searchResults = searchRequest(query, page);
            totalApiCalls++;
            
            if (!searchResults.contains("items")) {
                std::cout << "No 'items' field in search results" << std::endl;
                break;
            }
            
            auto items = searchResults["items"];
            std::cout << "Found " << items.size() << " items on page " << page << std::endl;
            
            // Check if we've hit GitHub's search limit
            if (searchResults.contains("total_count")) {
                int totalCount = searchResults["total_count"];
                std::cout << "Total available results: " << totalCount << " (GitHub limit: 1000)" << std::endl;
            }
            
            for (const auto& item : items) {
                if (item.contains("pull_request")) {
                    PRRepresentation pr;
                    pr.number = item.value("number", 0);
                    pr.title = getStringValue(item, "title", "No title");
                    pr.body = getStringValue(item, "body", "No description");
                    
                    if (item.contains("user") && !item["user"].is_null()) {
                        pr.author = getStringValue(item["user"], "login", "unknown");
                    } else {
                        pr.author = "unknown";
                    }
                    
                    pr.created_at = getStringValue(item, "created_at", "");
                    
                    std::cout << "Processing PR #" << pr.number << ": " << pr.title.substr(0, 50) << "..." << std::endl;
                    
                    // Get additional details (this will make more API calls)
                    try {
                        std::string prNumberStr = std::to_string(pr.number);
                        json filesData = makeRequest("pulls/" + prNumberStr + "/files");
                        totalApiCalls++;
                        
                        for (const auto& file : filesData) {
                            std::string filename = getStringValue(file, "filename", "");
                            if (!filename.empty()) {
                                pr.changed_files.push_back(filename);
                            }
                        }
                        
                        json reviewsData = makeRequest("pulls/" + prNumberStr + "/reviews");
                        totalApiCalls++;
                        
                        std::set<std::string> unique_reviewers;
                        for (const auto& review : reviewsData) {
                            if (review.contains("user") && !review["user"].is_null()) {
                                std::string reviewer = getStringValue(review["user"], "login", "");
                                if (!reviewer.empty()) {
                                    unique_reviewers.insert(reviewer);
                                }
                            }
                        }
                        pr.reviewers.assign(unique_reviewers.begin(), unique_reviewers.end());
                        
                        // Add rate limiting protection
                        if (totalApiCalls % 100 == 0) {
                            std::cout << "Made " << totalApiCalls << " API calls. Pausing 1 second to avoid rate limiting..." << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                        
                    } catch (const std::exception& e) {
                        std::cerr << "Error fetching details for PR #" << pr.number << ": " << e.what() << std::endl;
                        continue;
                    }
                    
                    prs.push_back(pr);
                }
            }
            
            // Check if we've reached the end (less than 100 items means last page)
            if (items.size() < 100) {
                std::cout << "Reached end of results at page " << page << std::endl;
                break;
            }
            
            // Check if we're approaching GitHub's 1000 result limit
            if (prs.size() >= 900) {
                std::cout << "Approaching GitHub's 1000 result limit. Stopping at " << prs.size() << " PRs." << std::endl;
                break;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error fetching page " << page << ": " << e.what() << std::endl;
            if (e.what() == std::string("GitHub API rate limit exceeded")) {
                std::cout << "Rate limit hit. Stopping at " << prs.size() << " PRs." << std::endl;
                break;
            }
            continue;
        }
    }
    
    std::cout << "Total API calls made: " << totalApiCalls << std::endl;
    return prs;
}


// Keep the original methods for backward compatibility
json GitHubFetcher::getOpenMPPRs(int page) {
    std::string endpoint = "pulls?state=all&labels=openmp&per_page=100&page=" + std::to_string(page);
    return makeRequest(endpoint);
}

json GitHubFetcher::getPRDetails(int pr_number) {
    std::string endpoint = "pulls/" + std::to_string(pr_number);
    return makeRequest(endpoint);
}

json GitHubFetcher::getPRReviews(int pr_number) {
    std::string endpoint = "pulls/" + std::to_string(pr_number) + "/reviews";
    return makeRequest(endpoint);
}

json GitHubFetcher::getPRFiles(int pr_number) {
    std::string endpoint = "pulls/" + std::to_string(pr_number) + "/files";
    return makeRequest(endpoint);
}
