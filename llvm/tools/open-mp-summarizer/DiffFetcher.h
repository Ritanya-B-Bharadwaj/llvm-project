#pragma once
#include <string>
#include <vector>

struct PRData {
    std::string title;
    std::string body;
    std::vector<std::string> files;
};

class DiffFetcher {
public:
    PRData fetchPR(int prNumber, const std::string& repo);
};
