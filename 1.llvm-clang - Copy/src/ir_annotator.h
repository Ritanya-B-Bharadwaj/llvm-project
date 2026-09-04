#ifndef IR_ANNOTATOR_H
#define IR_ANNOTATOR_H

#include "omp_parser.h"
#include <llvm/IR/Module.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

class IRAnnotator {
public:
    IRAnnotator(const std::string& sourceFile, const std::vector<DirectiveInfo>& directives, const std::string& clangFlags);
    void generateIR();
    void annotateIR();
    void saveOutput(const std::string& llOutput, const std::string& jsonOutput);
    std::string getIRForDirective(const std::string& directive) const;

private:
    std::string sourceFile;
    std::vector<DirectiveInfo> directives;
    std::string irContent;
    std::string clangFlags;
    std::unique_ptr<llvm::Module> module;
    std::map<unsigned, std::vector<std::string>> lineToIRMap;

    void runClang();
    void parseIR();
    void mapDirectivesToIR();
};

#endif