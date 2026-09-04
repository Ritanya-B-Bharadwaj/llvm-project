#include "cli11/CLI.hpp"
#include "Analysis.h"
#include "Reporter.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string> // Required for std::string

// These helper macros turn the version definition from CMake into a string literal
#define MACRO_TO_STRING_HELPER(x) #x
#define MACRO_TO_STRING(x) MACRO_TO_STRING_HELPER(x)

int main(int argc, char** argv) {
    CLI::App app{"MPI Uniform Participation Analyzer"};

    // --- Define CLI options ---
    std::vector<std::string> input_files;
    std::string output_file;
    bool json_output = false;
    bool csv_output = false;
    bool verbose = false;
    std::string logfile;
    bool strict = false;
    
    app.add_option("-i,--input", input_files, "Input LLVM IR file(s)")->required()->check(CLI::ExistingFile);
    app.add_option("-o,--output", output_file, "Write report to output file");
    app.add_flag("--json", json_output, "Export report in JSON format");
    app.add_flag("--csv", csv_output, "Export report in CSV format");
    app.add_flag("-v,--verbose", verbose, "Enable detailed processing logs");
    app.add_flag("--strict", strict, "Treat warnings (e.g., unmatched calls) as errors");
    app.add_option("--logfile", logfile, "Redirect verbose logs to a file");
    
    // Version flag now reads the version dynamically from the build system
    app.add_flag_callback("--version", [](){ 
        std::cout << "mpi-analyser version " << MACRO_TO_STRING(PROJECT_VERSION) << std::endl; 
        throw CLI::Success{}; 
    }, "Print tool version");

    CLI11_PARSE(app, argc, argv);

    // --- Main Logic ---
    if (verbose) std::cerr << "[VERBOSE] Verbose mode enabled." << std::endl;
    
    MPIAnalysis engine(verbose);
    for (const auto& file : input_files) {
        if (!engine.processFile(file)) {
            std::cerr << "ERROR: Failed to process file: " << file << std::endl;
            return 1;
        }
    }

    engine.runAnalysis();
    const auto& results = engine.getResults();

    // --- Select Reporter ---
    std::unique_ptr<Reporter> reporter;
    if (json_output) {
        reporter = std::make_unique<JsonReporter>();
    } else if (csv_output) {
        reporter = std::make_unique<CsvReporter>();
    } else {
        reporter = std::make_unique<TextReporter>();
    }

    // --- Output Generation ---
    if (!output_file.empty()) {
        std::ofstream outfile(output_file);
        if (!outfile) {
            std::cerr << "ERROR: Could not open output file: " << output_file << std::endl;
            return 1;
        }
        reporter->generateReport(results, outfile);
        std::cout << "Report successfully written to " << output_file << std::endl;
    } else {
        reporter->generateReport(results, std::cout);
    }
    
    // --- Strict Mode ---
    if (strict && (!results.unmatchedSends.empty() || !results.unmatchedReceives.empty())) {
        std::cerr << "STRICT MODE: Unmatched MPI calls detected, exiting with error." << std::endl;
        return 1;
    }

    return 0;
}