#ifndef PRDATA_H
#define PRDATA_H

#include <string>
#include <vector>

struct PRRepresentation {
    std::string title;
    std::string body;
    std::string author;
    std::string created_at;
    int number;
    std::vector<std::string> changed_files;
    std::vector<std::string> reviewers;
    std::vector<float> text_embedding;
    
    PRRepresentation() : number(0) {}
};

struct ReviewerSuggestion {
    std::string reviewer;
    double score;
    
    // Add default constructor
    ReviewerSuggestion() : reviewer(""), score(0.0) {}
    
    ReviewerSuggestion(const std::string& r, double s) : reviewer(r), score(s) {}
};

#endif // PRDATA_H
