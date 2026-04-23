#pragma once
#include "DiffFetcher.h"
#include <string>

class SummaryGenerator {
public:
    std::string generate(const PRData& pr, const std::string& specSnippet);
};
