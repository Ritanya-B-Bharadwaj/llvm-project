#include "html_reporter.h"
#include <fstream>
#include <sstream>
#include <iostream>

HTMLReporter::HTMLReporter(const std::string &outputFile) : outputFile(outputFile) {}

void HTMLReporter::generateReport(const std::vector<DirectiveInfo> &directives) {
    std::ofstream report(outputFile);
    if (!report.is_open()) {
        std::cerr << "Error: Could not open output file for writing: " << outputFile << std::endl;
        return;
    }

    report << "<html>\n<head>\n<title>OpenMP IR Mapping Report</title>\n";
    report << "<style>\n";
    report << "table { width: 100%; border-collapse: collapse; }\n";
    report << "th, td { border: 1px solid black; padding: 8px; text-align: left; }\n";
    report << "th { background-color: #f2f2f2; }\n";
    report << "</style>\n</head>\n<body>\n";
    report << "<h1>OpenMP IR Mapping Report</h1>\n";
    report << "<table>\n<tr><th>Directive</th><th>Source Line</th><th>Description</th><th>IR Transformation</th></tr>\n";

    for (const auto &directive : directives) {
        report << "<tr>\n";
        report << "<td>" << directive.name << "</td>\n";
        report << "<td>" << directive.sourceLine << "</td>\n";
        report << "<td>" << directive.description << "</td>\n";
        report << "<td>" << directive.irTransformation << "</td>\n";
        report << "</tr>\n";
    }

    report << "</table>\n</body>\n</html>\n";
    report.close();
}