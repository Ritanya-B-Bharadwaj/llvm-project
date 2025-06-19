#pragma once
#include "Data.h"
#include <string>
#include <iostream>

// The "Interface" for all reporters
class Reporter {
public:
    virtual ~Reporter() = default;
    virtual void generateReport(const AnalysisResults& results, std::ostream& out) = 0;
};

// --- DECLARATIONS ONLY ---

// Declares the TextReporter class
class TextReporter : public Reporter {
public:
    void generateReport(const AnalysisResults& results, std::ostream& out) override;
};

// Declares the JsonReporter class
class JsonReporter : public Reporter {
public:
    void generateReport(const AnalysisResults& results, std::ostream& out) override;
};

// Declares the CsvReporter class
class CsvReporter : public Reporter {
public:
    void generateReport(const AnalysisResults& results, std::ostream& out) override;
};