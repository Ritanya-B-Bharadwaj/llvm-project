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
    out << "th:first-child {\n  width: 8%;\n}\n";
    out << "th:nth-child(2) {\n  width: 25%;\n}\n";
    out << "th:nth-child(3) {\n  width: 45%;\n}\n";
    out << "th:last-child {\n  width: 22%;\n}\n";
    out << "</style>\n\n";
    out << "| Source Line | Source Code | LLVM IR | Summary |\n";
    out << "| ----------: | ----------- | ------- | ------- |\n";
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

void outputHTMLView(const std::map<int, SourceLineMapping>& lineMapping, std::ostream& out, const std::string& sourceFile) {
    std::string sourceBasename = llvm::sys::path::filename(sourceFile).str();
    
    out << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    out << "  <meta charset=\"UTF-8\">\n";
    out << "  <title>LLVM Source to IR Mapping with Summaries</title>\n";
    out << "  <style>\n";
    out << "    body { font-family: 'Segoe UI', Arial, sans-serif; background: #f9f9f9; margin: 0; padding: 2em; }\n";
    out << "    table { width: 100%; border-collapse: collapse; background: #fff; box-shadow: 0 2px 8px #0001; }\n";
    out << "    th, td { border: 1px solid #ddd; padding: 0.75em 1em; vertical-align: top; }\n";
    out << "    th { background: #f0f0f0; font-weight: 600; }\n";
    out << "    tr:nth-child(even) { background: #fafbfc; }\n";
    out << "    code, pre { font-family: 'Fira Mono', 'Consolas', 'Menlo', monospace; font-size: 0.98em; }\n";
    out << "    pre { background: #f6f8fa; padding: 0.5em 1em; border-radius: 4px; margin: 0; }\n";
    out << "    .src { color: #005cc5; }\n";
    out << "    .summary { color: #22863a; }\n";
    out << "    th:last-child, td:last-child { width: 22%; min-width: 200px; }\n";
    out << "  </style>\n";
    out << "</head>\n<body>\n";
    out << "  <h1>LLVM Source to IR Mapping with Summaries</h1>\n";
    out << "  <table>\n    <thead>\n      <tr>\n        <th>Source Line</th>\n        <th>Source Code</th>\n        <th>LLVM IR</th>\n        <th>Summary</th>\n      </tr>\n    </thead>\n    <tbody>\n";
    
    for (const auto& entry : lineMapping) {
        int line = entry.first;
        const SourceLineMapping& mapping = entry.second;
        
        out << "      <tr>\n";
        out << "        <td>" << line << "</td>\n";
        out << "        <td><code>" << mapping.sourceLineContent << "</code></td>\n";
        out << "        <td><pre>";
        
        for (size_t i = 0; i < mapping.irInstructions.size(); i++) {
            out << mapping.irInstructions[i];
            if (i < mapping.irInstructions.size() - 1) {
                out << "\n";
            }
        }
        
        out << "</pre></td>\n";
        out << "        <td class=\"summary\">" << mapping.summary << "</td>\n";
        out << "      </tr>\n";
    }
    
    out << "    </tbody>\n  </table>\n</body>\n</html>\n";
}

int main(int argc, const char **argv) {
    auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, SourceMapperCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();
    const auto& sourcePaths = OptionsParser.getSourcePathList();
    if (sourcePaths.empty()) {
        llvm::errs() << "Error: No source files specified.\n";
        return 1;
    }
    std::string sourceFile = sourcePaths[0];
    if (sourcePaths.size() > 1) {
        llvm::errs() << "Warning: Only processing the first source file: " << sourceFile << "\n";
    }
    std::string extension = llvm::sys::path::extension(sourceFile).str();
    if (extension != ".cpp" && extension != ".cc" && extension != ".cxx") {
        llvm::errs() << "Error: Only C++ files (.cpp, .cc, .cxx) are supported.\n";
        return 1;
    }
    TempFileManager tempManager;
    std::string irFile = generateIR(sourceFile, tempManager);
    auto lineMapping = mapSourceToIR(irFile, sourceFile);
    
    std::ofstream outputFileStream;
    std::ostream* out = &std::cout;
    if (!OutputFile.empty()) {
        outputFileStream.open(OutputFile);
        if (!outputFileStream.is_open()) {
            llvm::errs() << "Error: Could not open output file: " << OutputFile << "\n";
            return 1;
        }
        out = &outputFileStream;
    }
    
    if (OutputFormat == "html") {
        outputHTMLView(lineMapping, *out, sourceFile);
    } else if (OutputFormat == "md") {
        outputMarkdownView(lineMapping, *out);
    } else {
        outputAnnotatedIR(lineMapping, *out);
    }
    
    if (outputFileStream.is_open()) {
        outputFileStream.close();
    }
    
    return 0;
}
