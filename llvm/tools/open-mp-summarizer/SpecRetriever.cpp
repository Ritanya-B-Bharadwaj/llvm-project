#include "SpecRetriever.h"
#include <fstream>
#include <sstream>
#include <algorithm>

SpecRetriever::SpecRetriever(const std::string& specPath) {
    std::ifstream file(specPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    specText = buffer.str();
}

std::string SpecRetriever::searchSpec(const std::string& keyword) {
    std::string lowered = specText;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

    std::string keywordLower = keyword;
    std::transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);

    size_t pos = lowered.find(keywordLower);
    if (pos == std::string::npos) return "🔍 Keyword not found in spec.";

    size_t start = lowered.rfind("\n", pos > 300 ? pos - 300 : 0);
    size_t end = lowered.find("\n", pos + 500);
    return specText.substr(start, end - start);
}
