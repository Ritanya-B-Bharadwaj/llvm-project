// llvm/tools/optviz/src/SummaryGen.cpp

#include "SummaryGen.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <llvm/Support/raw_ostream.h>

using json = nlohmann::json;

// libcurl callback: append data
static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    static_cast<std::string*>(userdata)->append(ptr, total);
    return total;
}

// Read file into string
static bool readFileToString(const std::string &path, std::string &out) {
    std::ifstream in(path);
    if (!in.is_open()) return false;
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

std::string SummaryGen::summarize(const std::string &beforePath,
                                  const std::string &afterPath) {
    // Load IR
    std::string beforeIR, afterIR;
    if (!readFileToString(beforePath, beforeIR) ||
        !readFileToString(afterPath,  afterIR)) {
        return "[Error: cannot read IR files]";
    }

    // Build prompt
    std::string prompt =
        "Summarize the change between two LLVM IR snippets, focusing on intent and performance:\n\n"
        "BEFORE IR:\n" + beforeIR +
        "\nAFTER IR:\n"  + afterIR +
        "\n\nProvide a clear, concise explanation.";

    // API key
    const char *keyEnv = std::getenv("COHERE_API_KEY");
    if (!keyEnv) return "[Error: COHERE_API_KEY not set]";
    std::string apiKey(keyEnv);

    // JSON payload
    json body = {
        {"model", "command"},
        {"prompt", prompt},
        {"max_tokens", 200},
        {"temperature", 0.3}
    };
    std::string bodyStr = body.dump();

    // HTTP POST
    CURL *curl = curl_easy_init();
    if (!curl) return "[Error: curl init failed]";
    std::string response;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.cohere.ai/generate");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return "[Error: Cohere request failed]";

    // Debug raw JSON
    llvm::errs() << "=== Raw Cohere response ===\n" << response << "\n";

    // Parse JSON
    auto j = json::parse(response, nullptr, false);
    if (j.is_discarded()) return "[Error: JSON parse failed]";

    // Extract "text" field if present
    if (j.contains("text") && j["text"].is_string()) {
        std::string out = j["text"].get<std::string>();
        // Trim whitespace
        const auto ws = " \t\n\r";
        size_t start = out.find_first_not_of(ws);
        size_t end = out.find_last_not_of(ws);
        if (start != std::string::npos && end != std::string::npos)
            return out.substr(start, end - start + 1);
        return out;
    }

    return "[Error: Unexpected API schema]";
}