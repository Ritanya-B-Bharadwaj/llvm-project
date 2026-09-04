#ifndef HTML_REPORTER_H
#define HTML_REPORTER_H

#include <string>
#include <vector>

class HTMLReporter {
public:
    HTMLReporter(const std::string &outputFile);
    void generateReport(const std::vector<std::string> &directiveMappings);
    void addDirectiveMapping(const std::string &directive, const std::string &sourceLine,
                             const std::string &description, const std::string &irTransformation);
    void finalizeReport();

private:
    std::string outputFile;
    std::vector<std::string> reportContent;
};

#endif // HTML_REPORTER_H