#include "ReviewerSuggester.h"
#include <algorithm>
#include <set>        // Add this line
#include <map>
#include <iostream>
#include <cmath>

ReviewerSuggester::ReviewerSuggester() {
    // Default constructor
}

double ReviewerSuggester::cosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2) {
    if (v1.size() != v2.size() || v1.empty()) {
        return 0.0;
    }
    
    double dot_product = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;
    
    for (size_t i = 0; i < v1.size(); ++i) {
        dot_product += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    
    if (norm1 == 0.0 || norm2 == 0.0) {
        return 0.0;
    }
    
    return dot_product / (std::sqrt(norm1) * std::sqrt(norm2));
}

double ReviewerSuggester::jaccardSimilarity(const std::vector<std::string>& s1, const std::vector<std::string>& s2) {
    std::set<std::string> set1(s1.begin(), s1.end());
    std::set<std::string> set2(s2.begin(), s2.end());
    
    std::vector<std::string> intersection;
    std::set_intersection(set1.begin(), set1.end(),
                         set2.begin(), set2.end(),
                         std::back_inserter(intersection));
    
    std::vector<std::string> union_set;
    std::set_union(set1.begin(), set1.end(),
                  set2.begin(), set2.end(),
                  std::back_inserter(union_set));
    
    if (union_set.empty()) {
        return 0.0;
    }
    
    return static_cast<double>(intersection.size()) / static_cast<double>(union_set.size());
}

double ReviewerSuggester::calculateSimilarity(const PRRepresentation& pr1, const PRRepresentation& pr2) {
    double text_sim = cosineSimilarity(pr1.text_embedding, pr2.text_embedding);
    double file_sim = jaccardSimilarity(pr1.changed_files, pr2.changed_files);
    
    // Weighted average: prioritize text similarity for OpenMP PRs
    return 0.7 * text_sim + 0.3 * file_sim;
}

PRRepresentation ReviewerSuggester::createPRRepresentation(const json& prData, PRParser& parser) {
    PRRepresentation pr;
    
    try {
        pr.number = prData.value("number", 0);
        
        // Use safe string extraction
        std::string title = prData.value("title", std::string(""));
        std::string body = prData.value("body", std::string(""));
        
        // Handle null values
        if (prData["title"].is_null()) title = "No title";
        if (prData["body"].is_null()) body = "No description";
        
        pr.title = title;
        pr.body = body;
        pr.author = prData["user"].value("login", std::string("unknown"));
        pr.created_at = prData.value("created_at", std::string(""));
        
        // Generate text embedding with safe concatenation
        std::string combined_text = pr.title + " " + pr.body;
        if (combined_text.length() > 3) { // Only generate if we have meaningful text
            pr.text_embedding = parser.generateEmbedding(combined_text);
        } else {
            pr.text_embedding = std::vector<float>(384, 0.0f); // Zero embedding for empty text
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating PR representation: " << e.what() << std::endl;
        // Return a default PR representation
        pr.title = "Error loading PR";
        pr.body = "Error loading PR";
        pr.author = "unknown";
        pr.text_embedding = std::vector<float>(384, 0.0f);
    }
    
    return pr;
}


std::vector<std::string> ReviewerSuggester::extractReviewers(const json& prData) {
    std::vector<std::string> reviewers;
    
    try {
        if (prData.contains("requested_reviewers")) {
            for (const auto& reviewer : prData["requested_reviewers"]) {
                if (reviewer.contains("login")) {
                    reviewers.push_back(reviewer["login"]);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error extracting reviewers: " << e.what() << std::endl;
    }
    
    return reviewers;
}

std::vector<ReviewerSuggestion> ReviewerSuggester::suggestReviewers(
    const PRRepresentation& currentPR,
    const std::vector<PRRepresentation>& historicalPRs,
    PRParser& parser,
    int topCount) {
    
    std::map<std::string, double> reviewer_scores;
    
    // Calculate similarity with each historical PR and accumulate scores for reviewers
    for (const auto& historicalPR : historicalPRs) {
        double similarity = calculateSimilarity(currentPR, historicalPR);
        
        // Weight by recency (newer PRs get higher weight)
        double recency_weight = 1.0; // Could be enhanced with actual date parsing
        
        for (const auto& reviewer : historicalPR.reviewers) {
            if (reviewer != currentPR.author) { // Don't suggest the author as a reviewer
                reviewer_scores[reviewer] += similarity * recency_weight;
            }
        }
    }
    
    // Convert to vector and sort by score
    std::vector<ReviewerSuggestion> suggestions;
    for (const auto& pair : reviewer_scores) {
        suggestions.emplace_back(pair.first, pair.second);
    }
    
    std::sort(suggestions.begin(), suggestions.end(),
              [](const ReviewerSuggestion& a, const ReviewerSuggestion& b) {
                  return a.score > b.score;
              });
    
    // Return top suggestions
    if (suggestions.size() > static_cast<size_t>(topCount)) {
        suggestions.resize(topCount);
    }
    
    return suggestions;
}
