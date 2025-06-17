#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <regex>

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Path.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h" // Add explicit include for DILocation
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/DebugInfo/DIContext.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"

using json = nlohmann::json;

// Global variables for environment configuration
std::string GROQ_API_KEY;
std::string GROQ_MODEL = "llama3-8b-8192";
bool ENABLE_AI_SUMMARIES = false;

// HTTP response structure for curl
struct HTTPResponse {
    std::string data;
    long response_code;
};

// Callback function for curl to write response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, HTTPResponse *response) {
    size_t totalSize = size * nmemb;
    response->data.append((char*)contents, totalSize);
    return totalSize;
}

// Load environment variables from .env file
void loadEnvironmentVariables() {
    std::ifstream envFile(".env");
    if (!envFile.is_open()) {
        llvm::errs() << "Warning: .env file not found. AI summaries will be disabled.\n";
        return;
    }
    
    std::string line;
    while (std::getline(envFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "GROQ_API_KEY") {
                GROQ_API_KEY = value;
            } else if (key == "GROQ_MODEL") {
                GROQ_MODEL = value;
            } else if (key == "ENABLE_AI_SUMMARIES") {
                ENABLE_AI_SUMMARIES = (value == "true" || value == "1");
            }
        }
    }
    
    if (GROQ_API_KEY.empty() || GROQ_API_KEY == "your_groq_api_key_here" || GROQ_API_KEY == "gsk_your_actual_key_here") {
        llvm::errs() << "Warning: GROQ_API_KEY not set properly. AI summaries will be disabled.\n";
        ENABLE_AI_SUMMARIES = false;
    }
}

// Make HTTP request to Groq API
HTTPResponse makeGroqRequest(const std::string& prompt) {
    HTTPResponse response;
    response.response_code = 0;
    
    if (!ENABLE_AI_SUMMARIES || GROQ_API_KEY.empty()) {
        return response;
    }
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        llvm::errs() << "Failed to initialize curl\n";
        return response;
    }
    
    // Prepare JSON payload
    json payload = {
        {"messages", json::array({
            json::object({
                {"role", "system"},
                {"content", "You are an expert in LLVM IR and compiler optimization. Provide concise, technical explanations of LLVM IR instruction sequences in 1-2 sentences. Focus on what the code does at a high level."}
            }),
            json::object({
                {"role", "user"},
                {"content", prompt}
            })
        })},
        {"model", GROQ_MODEL},
        {"max_tokens", 250},
        {"temperature", 0.3}
    };
    
    std::string jsonString = payload.dump();
    
    // Set headers
    struct curl_slist *headers = nullptr;
    std::string authHeader = "Authorization: Bearer " + GROQ_API_KEY;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, authHeader.c_str());
    
    // Configure curl
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
    }
    
    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return response;
}

// Generate AI summary for LLVM IR instructions
std::string generateAISummary(const std::vector<std::string>& irInstructions, const std::string& sourceCode) {
    if (!ENABLE_AI_SUMMARIES || irInstructions.empty()) {
        return "";
    }
    
    std::string irCode;
    for (const auto& inst : irInstructions) {
        irCode += inst + "\n";
    }
    
    // Create varied prompts to avoid repetitive openings
    std::vector<std::string> promptTemplates = {
        "Explain each LLVM IR instruction in technical detail. Focus on register operations, memory access patterns, and instruction semantics:\n\n",
        "Provide technical analysis of this LLVM IR code. Describe what each instruction accomplishes at the IR level:\n\n", 
        "Break down these LLVM IR instructions. Explain the register usage, memory operations, and control flow:\n\n",
        "Analyze the LLVM IR instructions below. Focus on the technical details of each operation:\n\n"
    };
    
    // Use a simple hash to pick a template based on the IR content
    size_t templateIndex = std::hash<std::string>{}(irCode) % promptTemplates.size();
    std::string prompt = promptTemplates[templateIndex] + irCode;
    
    HTTPResponse response = makeGroqRequest(prompt);
    
    if (response.response_code == 200) {
        try {
            json responseJson = json::parse(response.data);
            if (responseJson.contains("choices") && !responseJson["choices"].empty()) {
                std::string summary = responseJson["choices"][0]["message"]["content"];
                
                // Clean up repetitive prefixes and standardize format
                std::vector<std::string> prefixesToRemove = {
                    "Here are the explanations:",
                    "Here are the explanations for each instruction:",
                    "Here's the analysis:",
                    "Here's the breakdown:",
                    "The explanations are:",
                    "Analysis:"
                };
                
                for (const auto& prefix : prefixesToRemove) {
                    if (summary.find(prefix) != std::string::npos) {
                        size_t pos = summary.find(prefix);
                        summary = summary.substr(pos + prefix.length());
                        break;
                    }
                }
                
                // Trim whitespace and newlines from the beginning
                summary.erase(0, summary.find_first_not_of(" \n\r\t"));
                
                return summary;
            }
        } catch (const json::exception& e) {
            llvm::errs() << "Error parsing Groq response: " << e.what() << "\n";
        }
    } else if (response.response_code != 0) {
        llvm::errs() << "Groq API request failed with code: " << response.response_code << "\n";
    }
    
    return "";
}

static llvm::cl::OptionCategory SourceMapperCategory("Source Mapper Options");
static llvm::cl::opt<std::string> OutputFormat(
    "format",
    llvm::cl::desc("Output format: 'll' for annotated LLVM IR, 'md' for Markdown, or 'html' for beautiful HTML"),
    llvm::cl::init("ll"),
    llvm::cl::cat(SourceMapperCategory));

static llvm::cl::opt<bool> EnableGenAISummary(
    "summarize",
    llvm::cl::desc("Enable GenAI-based summary of IR instructions (experimental)"),
    llvm::cl::init(false),
    llvm::cl::cat(SourceMapperCategory));

static llvm::cl::opt<std::string> OutputFile(
    "o",
    llvm::cl::desc("Output file path"),
    llvm::cl::value_desc("filename"),
    llvm::cl::cat(SourceMapperCategory));

class TempFileManager {
private:
    std::vector<std::string> tempFiles;
public:
    ~TempFileManager() {
        for (const auto& file : tempFiles) {
            llvm::sys::fs::remove(file);
        }
    }
    std::string createTempFile(const std::string& prefix, const std::string& suffix) {
        llvm::SmallString<128> tempFilePath;
        std::error_code EC = llvm::sys::fs::createTemporaryFile(prefix, suffix, tempFilePath);
        if (EC) {
            llvm::errs() << "Error creating temporary file: " << EC.message() << "\n";
            exit(1);
        }
        tempFiles.push_back(tempFilePath.str().str());
        return tempFilePath.str().str();
    }
};

struct SourceLineMapping {
    int sourceLine;
    std::string sourceFile;
    std::string sourceLineContent;
    std::vector<std::string> irInstructions;
    std::string summary;
};

std::string executeCommand(const std::string& command) {
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        llvm::errs() << "Error executing command: " << command << "\n";
        return "";
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    return result;
}

std::string readFileContent(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        llvm::errs() << "Error opening file: " << filePath << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

std::string getLineFromFile(const std::string& filePath, int lineNum) {
    std::ifstream file(filePath);
    std::string line;
    int currentLine = 1;
    while (std::getline(file, line)) {
        if (currentLine == lineNum) {
            return line;
        }
        currentLine++;
    }
    return "";
}

std::string generateIR(const std::string& sourceFile, TempFileManager& tempManager) {
    std::string irFile = tempManager.createTempFile("ir", ".ll");
    std::string command = "clang++ -S -emit-llvm -g -O0 -o " + irFile + " " + sourceFile;
    std::string output = executeCommand(command);
    if (!llvm::sys::fs::exists(irFile)) {
        llvm::errs() << "Failed to generate IR file\n";
        llvm::errs() << "Command output: " << output << "\n";
        exit(1);
    }
    return irFile;
}

std::map<int, SourceLineMapping> mapSourceToIR(const std::string& irFile, const std::string& sourceFile) {
    std::map<int, SourceLineMapping> lineMapping;
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;
    std::unique_ptr<llvm::Module> module = llvm::parseIRFile(irFile, err, context);
    if (!module) {
        llvm::errs() << "Error parsing IR file: " << err.getMessage() << "\n";
        exit(1);
    }
    std::string sourceBasename = llvm::sys::path::filename(sourceFile).str();
    for (auto &F : *module) {
        if (F.isDeclaration()) continue;
        for (auto &BB : F) {
            for (auto &I : BB) {
                const llvm::DebugLoc &debugLoc = I.getDebugLoc();
                if (debugLoc) {
                    unsigned line = debugLoc.getLine();
                    const llvm::DILocation *DILoc = debugLoc.get();
                    std::string filename = "";
                    if (DILoc) {
                        filename = DILoc->getFilename().str();
                    }
                    if (filename == sourceBasename) {
                        std::string instStr;
                        llvm::raw_string_ostream rso(instStr);
                        I.print(rso);
                        if (lineMapping.find(line) == lineMapping.end()) {
                            SourceLineMapping mapping;
                            mapping.sourceLine = line;
                            mapping.sourceFile = filename;
                            mapping.sourceLineContent = getLineFromFile(sourceFile, line);
                            lineMapping[line] = mapping;
                        }
                        lineMapping[line].irInstructions.push_back(instStr);
                    }
                }
            }
        }
    }
    return lineMapping;
}

void outputAnnotatedIR(const std::map<int, SourceLineMapping>& lineMapping, std::ostream& out) {
    out << "; LLVM IR with source mapping\n\n";
    for (const auto& entry : lineMapping) {
        int line = entry.first;
        const SourceLineMapping& mapping = entry.second;
        out << "\n; Source line " << line << ": " << mapping.sourceLineContent << "\n";
        for (const auto& inst : mapping.irInstructions) {
            out << inst << "\n";
        }
        if (!mapping.summary.empty()) {
            out << "; Summary: " << mapping.summary << "\n";
        }
    }
}

void outputMarkdownView(const std::map<int, SourceLineMapping>& lineMapping, std::ostream& out) {
    out << "# Source to LLVM IR Mapping\n\n";
    out << "<style>\n";
    out << "table {\n  width: 100%;\n  table-layout: fixed;\n  overflow-wrap: break-word;\n}\n";
    out << "th:first-child {\n  width: 6%;\n}\n";
    out << "th:nth-child(2) {\n  width: 18%;\n}\n";
    out << "th:nth-child(3) {\n  width: 36%;\n}\n";
    out << "th:last-child {\n  width: 40%;\n}\n";
    out << "</style>\n\n";
    out << "| Line | Source Code | LLVM IR | LLVM Analysis |\n";
    out << "| ---: | ----------- | ------- | -------------- |\n";
    for (const auto& entry : lineMapping) {
        int line = entry.first;
        const SourceLineMapping& mapping = entry.second;
        std::string escapedSource = mapping.sourceLineContent;
        std::string::size_type pos = 0;
        while ((pos = escapedSource.find("|", pos)) != std::string::npos) {
            escapedSource.replace(pos, 1, "\\|");
            pos += 2;
        }
        out << "| " << line << " | `" << escapedSource << "` | ";
        if (!mapping.irInstructions.empty()) {
            out << "<pre>";
            for (size_t i = 0; i < mapping.irInstructions.size(); i++) {
                std::string inst = mapping.irInstructions[i];
                std::string::size_type irPos = 0;
                while ((irPos = inst.find("|", irPos)) != std::string::npos) {
                    inst.replace(irPos, 1, "\\|");
                    irPos += 2;
                }
                out << inst;
                if (i < mapping.irInstructions.size() - 1) {
                    out << "\n";
                }
            }
            out << "</pre>";
        } else {
            out << " ";
        }
        out << " | ";
        if (!mapping.summary.empty()) {
            out << mapping.summary;
        }
        out << " |\n";
    }
}

// HTML escape function
std::string htmlEscape(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '&': escaped += "&amp;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Output HTML format matching the exact style from the repository
void outputHTMLView(const std::map<int, SourceLineMapping>& lineMapping, std::ostream& out, const std::string& sourceFile) {
    std::string sourceBasename = llvm::sys::path::filename(sourceFile).str();
    
    // Start HTML document with exact same styling
    out << R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>LLVM Source to IR Mapping with Summaries</title>
  <style>
    body { font-family: 'Segoe UI', Arial, sans-serif; background: #f9f9f9; margin: 0; padding: 2em; }
    table { width: 100%; border-collapse: collapse; background: #fff; box-shadow: 0 2px 8px #0001; }
    th, td { border: 1px solid #ddd; padding: 0.75em 1em; vertical-align: top; }
    th { background: #f0f0f0; font-weight: 600; }
    tr:nth-child(even) { background: #fafbfc; }
    code, pre { font-family: 'Fira Mono', 'Consolas', 'Menlo', monospace; font-size: 0.98em; }
    pre { background: #f6f8fa; padding: 0.5em 1em; border-radius: 4px; margin: 0; }
    .src { color: #005cc5; }
    .summary { color: #22863a; }
    th:last-child, td:last-child { width: 22%; min-width: 200px; }
  </style>
</head>
<body>
  <h1>LLVM Source to IR Mapping with Summaries</h1>
  <table>
    <thead>
      <tr>
        <th>Source Line</th>
        <th>Source Code</th>
        <th>LLVM IR</th>
        <th>Summary</th>
      </tr>
    </thead>
    <tbody>)";
    
    // Generate table rows
    for (const auto& entry : lineMapping) {
        int line = entry.first;
        const SourceLineMapping& mapping = entry.second;
        
        out << "      <tr>\n";
        out << "        <td>" << line << "</td>\n";
        out << "        <td><code>" << htmlEscape(mapping.sourceLineContent) << "</code></td>\n";
        out << "        <td><pre>";
        
        for (size_t i = 0; i < mapping.irInstructions.size(); i++) {
            out << htmlEscape(mapping.irInstructions[i]);
            if (i < mapping.irInstructions.size() - 1) {
                out << "\n";
            }
        }
        
        out << "</pre></td>\n";
        out << "        <td class=\"summary\">";
        if (!mapping.summary.empty()) {
            out << htmlEscape(mapping.summary);
        }
        out << "</td>\n";
        out << "      </tr>\n";
    }
    
    // Close HTML document
    out << R"(    </tbody>
  </table>
</body>
</html>)";
}

int main(int argc, const char **argv) {
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Load environment variables
    loadEnvironmentVariables();
    
    auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, SourceMapperCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        curl_global_cleanup();
        return 1;
    }
    clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();
    const auto& sourcePaths = OptionsParser.getSourcePathList();
    if (sourcePaths.empty()) {
        llvm::errs() << "Error: No source files specified.\n";
        curl_global_cleanup();
        return 1;
    }
    std::string sourceFile = sourcePaths[0];
    if (sourcePaths.size() > 1) {
        llvm::errs() << "Warning: Only processing the first source file: " << sourceFile << "\n";
    }
    std::string extension = llvm::sys::path::extension(sourceFile).str();
    if (extension != ".cpp" && extension != ".cc" && extension != ".cxx") {
        llvm::errs() << "Error: Only C++ files (.cpp, .cc, .cxx) are supported.\n";
        curl_global_cleanup();
        return 1;
    }
    TempFileManager tempManager;
    std::string irFile = generateIR(sourceFile, tempManager);
    auto lineMapping = mapSourceToIR(irFile, sourceFile);
    
    // Generate AI summaries if enabled
    if (EnableGenAISummary || ENABLE_AI_SUMMARIES) {
        llvm::errs() << "Generating AI summaries...\n";
        for (auto& entry : lineMapping) {
            SourceLineMapping& mapping = entry.second;
            if (!mapping.irInstructions.empty()) {
                mapping.summary = generateAISummary(mapping.irInstructions, mapping.sourceLineContent);
                if (!mapping.summary.empty() && mapping.summary != "Summary generation failed") {
                    // Trim the summary to keep it concise
                    if (mapping.summary.length() > 200) {
                        mapping.summary = mapping.summary.substr(0, 197) + "...";
                    }
                }
            }
        }
    }
    
    std::ofstream outputFileStream;
    std::ostream* out = &std::cout;
    if (!OutputFile.empty()) {
        outputFileStream.open(OutputFile);
        if (!outputFileStream.is_open()) {
            llvm::errs() << "Error: Could not open output file: " << OutputFile << "\n";
            curl_global_cleanup();
            return 1;
        }
        out = &outputFileStream;
    }
    
    // Generate output in the specified format
    if (OutputFormat == "html") {
        outputHTMLView(lineMapping, *out, sourceFile);
    } else if (OutputFormat == "md") {
        outputMarkdownView(lineMapping, *out);
    } else {
        // Default to 'll' format
        outputAnnotatedIR(lineMapping, *out);
    }
    
    if (outputFileStream.is_open()) {
        outputFileStream.close();
    }
    
    curl_global_cleanup();
    return 0;
}
