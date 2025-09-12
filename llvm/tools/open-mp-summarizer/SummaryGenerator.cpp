#include "SummaryGenerator.h"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <fstream>

std::string readFile(const std::string& filename) {
    std::ifstream f(filename);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::string escapeForJSON(const std::string& text) {
    std::ostringstream out;
    for (char c : text) {
        if (c == '\"') out << "\\\"";
        else if (c == '\n') out << "\\n";
        else out << c;
    }
    return out.str();
}

std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "❌ Failed to open pipe.";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        result += buffer.data();
    pclose(pipe);
    return result;
}

std::string SummaryGenerator::generate(const PRData& pr, const std::string& specSnippet) {
    // 1. Write prompt to file
    std::ofstream promptFile("prompt.txt");
    promptFile << "Summarize this GitHub PR in clean Markdown format.\n\n";
    promptFile << "### Title:\n" << pr.title << "\n\n";
    promptFile << "### Body:\n" << pr.body << "\n\n";
    promptFile << "### Changed Files:\n";
    for (const auto& f : pr.files)
        promptFile << "- " << f << "\n";
    promptFile << "\n### Relevant OpenMP Spec Snippet:\n" << specSnippet << "\n";
    promptFile.close();

    // 2. Escape prompt for JSON
    std::string prompt = escapeForJSON(readFile("prompt.txt"));

    // 3. Build curl command for Ollama (local model)
    std::string cmd = "curl -s http://localhost:11434/api/generate "
                  "-H \"Content-Type: application/json\" "
                  "-d \"{"
                  "\\\"model\\\": \\\"tinyllama\\\", "
                  "\\\"prompt\\\": \\\"" + escapeForJSON(prompt) + "\\\", "
                  "\\\"stream\\\": false"
                  "}\"";



    // 4. Run it and return response
    std::string raw = exec(cmd);
std::size_t start = raw.find("\"response\":\"");
if (start == std::string::npos) return "❌ Failed to parse response.";
start += std::string("\"response\":\"").length();
std::size_t end = raw.find("\"", start);
std::string clean = raw.substr(start, end - start);

// Unescape newlines and quotes
for (size_t pos; (pos = clean.find("\\n")) != std::string::npos;)
    clean.replace(pos, 2, "\n");
for (size_t pos; (pos = clean.find("\\\"")) != std::string::npos;)
    clean.replace(pos, 2, "\"");

// ✅ Save to summary.md
std::ofstream out("summary.md");
out << clean;
out.close();

return clean;


}
