#pragma once
#include <string>

class SpecRetriever {
public:
    explicit SpecRetriever(const std::string& specPath);
    std::string searchSpec(const std::string& keyword);

private:
    std::string specText;
};
