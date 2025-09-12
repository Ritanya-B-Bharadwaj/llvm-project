#include "DiffFetcher.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>  // ✅ Needed for std::cerr

PRData DiffFetcher::fetchPR(int prNumber, const std::string& repo) {
    std::string command = "gh pr view " + std::to_string(prNumber) + " --repo " + repo +
                          " --json title,body,files --jq '.title + \"\\n\" + .body + \"\\n\" + (.files | map(.path) | join(\"\\n\"))' > pr_data.txt";

    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to fetch PR data using GitHub CLI." << std::endl;
        return {};
    }

    std::ifstream file("pr_data.txt");
    PRData data;
    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        if (lineCount == 0) data.title = line;
        else if (lineCount == 1) data.body = line;
        else data.files.push_back(line);
        lineCount++;
    }

    return data;
}
