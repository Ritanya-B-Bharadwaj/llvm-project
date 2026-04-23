#include <iostream>
#include <string>
#include <vector>
#include "omp_parser.h"
#include "ir_annotator.h"
#include "config.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.cpp>" << std::endl;
        return 1;
    }

    std::string sourceFile = argv[1];
    Config config;

    // Load configuration
    if (!loadConfig("config/default.toml", config)) {
        std::cerr << "Failed to load configuration." << std::endl;
        return 1;
    }

    // Initialize the OpenMP parser
    OMPParser ompParser;
    if (!ompParser.parse(sourceFile)) {
        std::cerr << "Failed to parse OpenMP directives in " << sourceFile << std::endl;
        return 1;
    }

    // Initialize the IR annotator
    IRAnnotator irAnnotator;
    if (!irAnnotator.generateIR(sourceFile, ompParser.getDirectives())) {
        std::cerr << "Failed to generate LLVM IR." << std::endl;
        return 1;
    }

    // Output the annotated IR
    irAnnotator.outputAnnotatedIR(config.outputFormat);

    return 0;
}