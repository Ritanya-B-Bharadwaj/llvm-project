#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/FileSystem.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>

using namespace llvm;

static cl::opt<std::string> SourceFile(cl::Positional, cl::desc("<input .cpp file>"), cl::Required);
static cl::opt<std::string> AnnotatedOutput("ol", cl::desc("Output annotated .ll file"), cl::value_desc("filename"));
static cl::opt<std::string> MarkdownOutput("om", cl::desc("Output markdown file"), cl::value_desc("filename"));

int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "IR Mapper: maps IR to source lines\n");

    if (!llvm::sys::fs::exists(std::string(SourceFile))) {
        errs() << "Error: Source file does not exist.\n";
        return 1;
    }

    outs() << "Source file found: " << SourceFile << "\n";
    outs() << "Compiling with clang++...\n";

    std::string IRFile = "temp.ll";
    std::string CompileCmd = "clang++ -S -emit-llvm -g \"" + SourceFile + "\" -o \"" + IRFile + "\"";
    int result = std::system(CompileCmd.c_str());
    if (result != 0) {
        errs() << "Compilation failed.\n";
        return 1;
    }

    outs() << "Compilation finished. LLVM IR file: " << IRFile << "\n";

    LLVMContext Context;
    SMDiagnostic Err;
    auto M = parseIRFile(IRFile, Err, Context);
    if (!M) {
        Err.print(argv[0], errs());
        return 1;
    }

    std::map<unsigned, std::string> sourceLines;
    {
        std::ifstream src(SourceFile.getValue());
        std::string line;
        unsigned lineNumber = 1;
        while (std::getline(src, line)) {
            sourceLines[lineNumber++] = line;
        }
    }

    // Map from line number -> vector of IR instructions
    std::map<unsigned, std::vector<std::string>> irByLine;

    std::string annotatedIR;
    llvm::raw_string_ostream rawAnnotatedIR(annotatedIR);

    // Iterate IR instructions, group by source line
    for (auto &F : *M) {
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (DILocation *Loc = I.getDebugLoc()) {
                    unsigned Line = Loc->getLine();
                    std::string File = std::string(Loc->getFilename());

                    std::string cleanIR;
                    raw_string_ostream rso(cleanIR);
                    I.print(rso);
                    rso.flush();

                    outs() << File << ":" << Line << " => " << cleanIR << "\n";

                    irByLine[Line].push_back(cleanIR);

                    rawAnnotatedIR << "; " << File << ":" << Line << " - " << sourceLines[Line] << "\n";
                    rawAnnotatedIR << cleanIR << "\n";
                }
            }
        }
    }
    rawAnnotatedIR.flush();

    // Compose markdown output
    std::ostringstream markdown;
    markdown << "### " << SourceFile.getValue() << "\n\n";

    for (const auto &entry : irByLine) {
        unsigned lineNo = entry.first;
        const auto &irLines = entry.second;

        markdown << "#### Line " << lineNo << "\n";
        markdown << "Source code: `" << sourceLines[lineNo] << "`\n";
        markdown << "\nMapped IR code:\n```llvm\n";
        for (const auto &irInst : irLines) {
            markdown << irInst << "\n";
        }
        markdown << "```\n\n";
    }

    // Write markdown output if requested
    if (!MarkdownOutput.empty()) {
        std::ofstream mdOut(MarkdownOutput.getValue());
        if (!mdOut) {
            errs() << "Error opening markdown output file: " << MarkdownOutput << "\n";
            return 1;
        }
        mdOut << markdown.str();
        outs() << "Markdown written to: " << MarkdownOutput.getValue() << "\n";
    }

    // Write annotated .ll output if requested
    if (!AnnotatedOutput.empty()) {
        std::ofstream llOut(AnnotatedOutput.getValue());
        if (!llOut) {
            errs() << "Error opening annotated .ll file: " << AnnotatedOutput << "\n";
            return 1;
        }
        llOut << annotatedIR;
        outs() << "Annotated .ll written to: " << AnnotatedOutput << "\n";
    }

    return 0;
}
