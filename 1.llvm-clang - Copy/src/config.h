// filepath: c:\openmp to _ir_mapper_llvm\src\config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string clangFlags; // Clang flags for compilation
    bool generateHtmlReport; // Flag to enable HTML report generation
    std::string outputDirectory; // Directory for output files
};

void loadConfig(const std::string& configFilePath, Config& config);

#endif // CONFIG_H