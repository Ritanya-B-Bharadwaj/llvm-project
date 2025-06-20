#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: clang-review <PR_NUMBER>\n";
        return 1;
    }

    std::string pr_number = argv[1];
    std::string command = "python ..\\..\\src\\suggest_code_review.py " + pr_number;
    std::cout << "Running GenAI Reviewer on PR #" << pr_number << "...\n";
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Review script failed with code " << result << "\n";
    }

    return result;
}
