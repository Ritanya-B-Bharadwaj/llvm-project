#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <regex>
using json = nlohmann::json;

namespace fs = std::filesystem;
using namespace llvm;

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string compileToLLVMIR(const std::string& cppFile, const std::string& clangPath) {
    std::string llFile = cppFile.substr(0, cppFile.find_last_of(".")) + ".ll";

    std::string cmd = clangPath + " -fopenmp -g -S -emit-llvm " + cppFile + " -o " + llFile;
    std::cout << "🏗️ Compiling to LLVM IR with: " << cmd << "\n";
    int result = std::system(cmd.c_str());

    if (result != 0) {
        std::cerr << "❌ Failed to compile " << cppFile << " to LLVM IR\n";
        exit(1);
    }

    return llFile;
}

std::string queryGenAI(const std::string& prompt, const std::string& api_key) {
    CURL* curl = curl_easy_init();
    if (!curl) return "CURL init failed";

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    json payload = {
        {"model", "llama3-8b-8192"},
        {"messages", {
            {{"role", "system"}, {"content", "You are a compiler assistant. Explain OpenMP directives."}},
            {{"role", "user"}, {"content", prompt}}
        }},
        {"temperature", 0.7}
    };

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    std::string payloadStr = payload.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, void* userdata) {
        ((std::string*)userdata)->append(ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "CURL error";

    try {
        // Parse and print the raw response
        auto result_json = json::parse(response);

        // Extract explanation
        if (result_json.contains("choices") &&
            result_json["choices"].is_array() &&
            result_json["choices"][0].contains("message") &&
            result_json["choices"][0]["message"].contains("content")) {

            return result_json["choices"][0]["message"]["content"];
        } else {
            return "⚠️ GenAI response malformed or incomplete.";
        }
    } catch (const std::exception& e) {
        return std::string("⚠️ JSON parse error: ") + e.what();
    }
}


void parseIR(const std::string& llFile, const std::map<int, std::string>& ompDirectives)
{
    LLVMContext context;
    SMDiagnostic err;
    std::unique_ptr<Module> module = parseIRFile(llFile, err, context);

    if (!module) {
        err.print("omp_ir_mapper", errs());
        exit(1);
    }

    std::string annotatedLL = llFile + ".annotated.ll";
    std::error_code EC;
    raw_fd_ostream out(annotatedLL, EC);

    // Map of directive → list of IR call names
    std::map<std::string, std::vector<std::string>> directiveToIR;

    for (Function &F : *module) {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *call = dyn_cast<CallBase>(&I)) {
                    if (Function *callee = call->getCalledFunction()) {
                        std::string name = callee->getName().str();
                        if (name.find("__kmpc") != std::string::npos || name.find("omp") != std::string::npos) {
                            errs() << "🔧 OpenMP IR: " << name << "\n";

                            if (DILocation *loc = I.getDebugLoc()) {
                                unsigned line = loc->getLine();
                                StringRef file = loc->getFilename();

                                // Write annotation to .ll
                                if (ompDirectives.count(line)) {
                                    std::string directive = ompDirectives.at(line);
                                    out << "; From: " << file << ":" << line << "  →  " << directive << "\n";

                                    // Store for structured map
                                    directiveToIR[directive].push_back(name);
                                } else {
                                    out << "; From: " << file << ":" << line << "\n";
                                }

                                errs() << "   ↳ Source: " << file << ":" << line << "\n";
                            }
                        }
                    }
                }

                I.print(out);
                out << "\n";
            }
        }
    }

    // 🧭 Print structured map: directive → IR instructions
    std::cout << "\n📚 Structured Mapping (Directive → IR Calls):\n";
    for (const auto& [directive, irCalls] : directiveToIR) {
        std::cout << "\n➡️  " << directive << "\n";
        for (const auto& ir : irCalls) {
            std::cout << "   → " << ir << "\n";
        }
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source.cpp | ir.ll>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string clangPath = "~/llvm-install/bin/clang++";

    // Expand tilde (~)
    if (clangPath.find("~") == 0) {
        const char* home = getenv("HOME");
        clangPath.replace(0, 1, home);
    }

    std::string irFile;

    if (endsWith(inputFile, ".cpp")) {
        irFile = compileToLLVMIR(inputFile, clangPath);
    } else if (endsWith(inputFile, ".ll")) {
        irFile = inputFile;
    } else {
        std::cerr << "❌ Unsupported input. Use .cpp or .ll file.\n";
        return 1;
    }

    // 🧠 Collect directives first
    std::map<int, std::string> ompDirectives;
    std::ifstream src(inputFile);
    std::string line;
    int lineno = 1;

    while (std::getline(src, line)) {
        if (line.find("#pragma omp") != std::string::npos) {
            std::string directive = std::regex_replace(line, std::regex("^\\s+"), ""); // Clean up leading spaces
            ompDirectives[lineno] = directive;
        }
        lineno++;
    }

    parseIR(irFile, ompDirectives);  // ✅ Call IR parser with directive map

    // 🔑 GenAI explanations
    std::string api_key = "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";
    if (api_key.empty()) {
        std::cerr << "⚠️  No GROQ_API_KEY found. Skipping GenAI explanations.\n";
    } else {
        std::cout << "\n🔍 OpenMP Directive Explanations:\n";
        for (const auto& [lineNo, directive] : ompDirectives) {
            std::string prompt = "Briefly explain the OpenMP directive: " + directive +
                ". Give a short summary of its behavior and how it maps to LLVM IR in 2-3 sentences.";

            std::string explanation = queryGenAI(prompt, api_key);

            std::cout << "\n➡️  Directive: " << directive << "\n";
            std::cout << "📘 Explanation: " << explanation << "\n";
        }
    }

    return 0;
}
