//===- llvm-source-mapper.cpp - LLVM Source to IR Mapping Tool ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This tool generates mappings between C++ source code and the corresponding
// LLVM IR instructions. It can output in various formats including annotated
// LLVM IR, Markdown tables, and HTML.
//
//===----------------------------------------------------------------------===//

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
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/DebugInfo/DIContext.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"

static llvm::cl::OptionCategory SourceMapperCategory("Source Mapper Options");
static llvm::cl::opt<std::string> OutputFormat(
    "format",
    llvm::cl::desc("Output format: 'll' for annotated LLVM IR, 'md' for Markdown, or 'html' for beautiful HTML"),
    llvm::cl::init("ll"),
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

std::string escapeHtml(const std::string& str) {
    std::string escaped = str;
    std::regex ampRegex("&");
    escaped = std::regex_replace(escaped, ampRegex, "&amp;");
    std::regex ltRegex("<");
    escaped = std::regex_replace(escaped, ltRegex, "&lt;");
    std::regex gtRegex(">");
    escaped = std::regex_replace(escaped, gtRegex, "&gt;");
    return escaped;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> getSourceLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<SourceLineMapping> parseIRForMappings(const std::string& irContent, const std::string& sourceFile) {
    std::vector<SourceLineMapping> mappings;
    std::map<int, SourceLineMapping> lineMap;
    std::vector<std::string> sourceLines = getSourceLines(sourceFile);
    
    std::istringstream stream(irContent);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Look for debug info metadata pattern: !dbg !<number>
        std::regex debugRegex(R"(.*!dbg !(\d+))");
        std::smatch matches;
        
        if (std::regex_search(line, matches, debugRegex)) {
            // Extract line number from debug info metadata
            std::string metadataId = matches[1].str();
            
            // Look for the metadata definition later in the IR
            std::istringstream metadataStream(irContent);
            std::string metadataLine;
            std::regex metadataRegex("!" + metadataId + R"( = !DILocation\(line: (\d+))");
            std::smatch metadataMatches;
            
            while (std::getline(metadataStream, metadataLine)) {
                if (std::regex_search(metadataLine, metadataMatches, metadataRegex)) {
                    int sourceLine = std::stoi(metadataMatches[1].str());
                    
                    // Skip line 0 (invalid debug info)
                    if (sourceLine > 0 && sourceLine <= (int)sourceLines.size()) {
                        if (lineMap.find(sourceLine) == lineMap.end()) {
                            lineMap[sourceLine] = {
                                sourceLine,
                                sourceFile,
                                sourceLines[sourceLine - 1], // 0-indexed
                                {},
                                ""
                            };
                        }
                        lineMap[sourceLine].irInstructions.push_back(trim(line));
                    }
                    break;
                }
            }
        }
    }
    
    // Convert map to vector and sort by line number
    for (auto& pair : lineMap) {
        mappings.push_back(pair.second);
    }
    
    return mappings;
}

void writeAnnotatedIR(const std::vector<SourceLineMapping>& mappings, const std::string& irContent, const std::string& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        llvm::errs() << "Error opening output file: " << outputPath << "\n";
        return;
    }
    
    output << "; Source to LLVM IR Mapping\n";
    output << "; Generated by llvm-source-mapper\n\n";
    
    for (const auto& mapping : mappings) {
        output << "; Line " << mapping.sourceLine << ": " << mapping.sourceLineContent << "\n";
        for (const auto& instruction : mapping.irInstructions) {
            output << instruction << "\n";
        }
        output << "\n";
    }
}

void writeMarkdownTable(const std::vector<SourceLineMapping>& mappings, const std::string& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        llvm::errs() << "Error opening output file: " << outputPath << "\n";
        return;
    }
    
    output << "# Source to LLVM IR Mapping\n\n";
    output << "<style>\n";
    output << "table {\n";
    output << "  width: 100%;\n";
    output << "  table-layout: fixed;\n";
    output << "  overflow-wrap: break-word;\n";
    output << "}\n";
    output << "th:first-child {\n";
    output << "  width: 6%;\n";
    output << "}\n";
    output << "th:nth-child(2) {\n";
    output << "  width: 18%;\n";
    output << "}\n";
    output << "th:nth-child(3) {\n";
    output << "  width: 36%;\n";
    output << "}\n";
    output << "th:last-child {\n";
    output << "  width: 40%;\n";
    output << "}\n";
    output << "</style>\n\n";
    
    output << "| Line | Source Code | LLVM IR | Summary |\n";
    output << "|------|-------------|---------|----------|\n";
    
    for (const auto& mapping : mappings) {
        std::string irBlock;
        for (const auto& instruction : mapping.irInstructions) {
            if (!irBlock.empty()) irBlock += "\\n";
            irBlock += instruction;
        }
        
        output << "| " << mapping.sourceLine << " | `" << mapping.sourceLineContent 
               << "` | <pre>" << irBlock << "</pre> | " << mapping.summary << "|\n";
    }
}

void writeHTMLTable(const std::vector<SourceLineMapping>& mappings, const std::string& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        llvm::errs() << "Error opening output file: " << outputPath << "\n";
        return;
    }
    
    output << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    output << "  <meta charset=\"UTF-8\">\n";
    output << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    output << "  <title>LLVM Source to IR Mapping</title>\n";
    output << "  <style>\n";
    output << "    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif; line-height: 1.5; color: #1f2328; background-color: #ffffff; margin: 0; padding: 24px; }\n";
    output << "    .container { max-width: 1200px; margin: 0 auto; }\n";
    output << "    h1 { font-size: 32px; font-weight: 600; margin-bottom: 24px; border-bottom: 1px solid #d1d9e0; padding-bottom: 10px; }\n";
    output << "    table { width: 100%; border-collapse: collapse; border-spacing: 0; margin-top: 16px; font-size: 14px; }\n";
    output << "    th, td { padding: 6px 13px; border: 1px solid #d1d9e0; text-align: left; vertical-align: top; }\n";
    output << "    th { font-weight: 600; background-color: #f6f8fa; }\n";
    output << "    tr:nth-child(2n) { background-color: #f6f8fa; }\n";
    output << "    code { padding: 0.2em 0.4em; margin: 0; font-size: 85%; background-color: rgba(175,184,193,0.2); border-radius: 6px; font-family: ui-monospace, SFMono-Regular, \"SF Mono\", Menlo, Consolas, \"Liberation Mono\", monospace; }\n";
    output << "    pre { padding: 16px; overflow: auto; font-size: 85%; line-height: 1.45; background-color: #f6f8fa; border-radius: 6px; margin: 0; font-family: ui-monospace, SFMono-Regular, \"SF Mono\", Menlo, Consolas, \"Liberation Mono\", monospace; white-space: pre; }\n";
    output << "    .line-number { width: 80px; text-align: center; font-weight: 600; }\n";
    output << "    .source-code { width: 25%; min-width: 200px; }\n";
    output << "    .llvm-ir { width: 40%; min-width: 300px; }\n";
    output << "    .summary { width: 25%; min-width: 200px; color: #656d76; font-style: italic; }\n";
    output << "  </style>\n";
    output << "</head>\n<body>\n";
    output << "  <div class=\"container\">\n";
    output << "    <h1>LLVM Source to IR Mapping</h1>\n";
    output << "    <table>\n      <thead>\n        <tr>\n";
    output << "          <th class=\"line-number\">Line</th>\n";
    output << "          <th class=\"source-code\">Source Code</th>\n";
    output << "          <th class=\"llvm-ir\">LLVM IR</th>\n";
    output << "          <th class=\"summary\">Summary</th>\n";
    output << "        </tr>\n      </thead>\n      <tbody>\n";
    
    for (const auto& mapping : mappings) {
        std::string irBlock;
        for (const auto& instruction : mapping.irInstructions) {
            if (!irBlock.empty()) irBlock += "\n";
            irBlock += instruction;
        }
        
        output << "        <tr>\n";
        output << "          <td class=\"line-number\">" << mapping.sourceLine << "</td>\n";
        output << "          <td class=\"source-code\"><code>" << escapeHtml(mapping.sourceLineContent) << "</code></td>\n";
        output << "          <td class=\"llvm-ir\"><pre>" << escapeHtml(irBlock) << "</pre></td>\n";
        output << "          <td class=\"summary\">" << escapeHtml(mapping.summary) << "</td>\n";
        output << "        </tr>\n";
    }
    
    output << "      </tbody>\n    </table>\n  </div>\n</body>\n</html>\n";
}

int main(int argc, const char **argv) {
    auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, SourceMapperCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();
    
    auto SourcePaths = OptionsParser.getSourcePathList();
    if (SourcePaths.empty()) {
        llvm::errs() << "No source files specified\n";
        return 1;
    }
    
    std::string sourceFile = SourcePaths[0];
    
    // Generate output filename if not provided
    std::string outputPath = OutputFile;
    if (outputPath.empty()) {
        llvm::SmallString<128> baseName(sourceFile);
        llvm::sys::path::replace_extension(baseName, "");
        outputPath = baseName.str().str() + "_mapping";
        if (OutputFormat == "md") {
            outputPath += ".md";
        } else if (OutputFormat == "html") {
            outputPath += ".html";
        } else {
            outputPath += ".ll";
        }
    }
    
    // Create temporary file manager
    TempFileManager tempManager;
    
    // Generate LLVM IR with debug info
    std::string tempIRFile = tempManager.createTempFile("source_mapper", ".ll");
    
    // Compile to LLVM IR with debug information
    std::string compileCommand = "clang++ -emit-llvm -g -S -o " + tempIRFile + " " + sourceFile;
    int result = system(compileCommand.c_str());
    if (result != 0) {
        llvm::errs() << "Error compiling source file to LLVM IR\n";
        return 1;
    }
    
    // Read the generated IR
    std::string irContent = readFileContent(tempIRFile);
    if (irContent.empty()) {
        llvm::errs() << "Error reading generated LLVM IR\n";
        return 1;
    }
    
    // Parse IR and create mappings
    std::vector<SourceLineMapping> mappings = parseIRForMappings(irContent, sourceFile);
    
    if (mappings.empty()) {
        llvm::errs() << "No source-to-IR mappings found. Make sure the source file compiles correctly.\n";
        return 1;
    }
    
    // Generate output based on format
    if (OutputFormat == "md") {
        writeMarkdownTable(mappings, outputPath);
        llvm::outs() << "Markdown mapping written to: " << outputPath << "\n";
    } else if (OutputFormat == "html") {
        writeHTMLTable(mappings, outputPath);
        llvm::outs() << "HTML mapping written to: " << outputPath << "\n";
    } else {
        writeAnnotatedIR(mappings, irContent, outputPath);
        llvm::outs() << "Annotated LLVM IR written to: " << outputPath << "\n";
    }
    
    return 0;
}
