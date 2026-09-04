#ifndef REVIEWER_SUGGESTER_H
#define REVIEWER_SUGGESTER_H

#include "PRData.h"
#include "PRParser.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ReviewerSuggester {
public:
    ReviewerSuggester();
    
    std::vector<ReviewerSuggestion> suggestReviewers(
        const PRRepresentation& currentPR,
        const std::vector<PRRepresentation>& historicalPRs,
        PRParser& parser,
        int topCount = 5
    );

private:
    double calculateSimilarity(const PRRepresentation& pr1, const PRRepresentation& pr2);
    double cosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2);
    double jaccardSimilarity(const std::vector<std::string>& s1, const std::vector<std::string>& s2);
    
    PRRepresentation createPRRepresentation(const json& prData, PRParser& parser);
    std::vector<std::string> extractReviewers(const json& prData);
};

#endif // REVIEWER_SUGGESTER_H
