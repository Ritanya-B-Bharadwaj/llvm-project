#include "Reporter.h"
#include <vector>
#include <algorithm> // For std::min

// --- NEW: Color Definitions using ANSI Escape Codes ---
// We define them as constants to make the code readable.
const char* const RESET_COLOR = "\033[0m";
const char* const YELLOW_COLOR = "\033[1;33m"; // Bold Yellow
const char* const RED_COLOR = "\033[1;31m";    // Bold Red
const char* const GREEN_COLOR = "\033[1;32m";  // Bold Green
const char* const BLUE_COLOR = "\033[1;34m";   // Bold Blue
const char* const CYAN_COLOR = "\033[1;36m";   // Bold Cyan

// --- NEW: Helper function to wrap text in color codes ---
std::string colorize(const std::string& text, const char* color) {
    // Note: This only works on terminals that support ANSI codes.
    return std::string(color) + text + std::string(RESET_COLOR);
}


// --- ENHANCED: TextReporter with Colors and ASCII Art ---
void TextReporter::generateReport(const AnalysisResults& results, std::ostream& out) {
    // A cool banner for the tool
    out << BLUE_COLOR;
   out << "  __  __ ______ _____   ___   ____ _______ ______ \n";
out << " |  \\/  |  __ \\_   _| |__ \\ / __ \\__   __|___  / \n";
out << " | \\  / | |__) || |      ) | |  | | | |     / /  \n";
out << " | |\\/| |  ___/ | |     / /| |  | | | |    / /   \n";
out << " | |  | | |    _| |_   / /_| |__| | | |   / /__  \n";
out << " |_|  |_|_|   |_____| |____|\\____/  |_|  /_____| \n";
    out << RESET_COLOR << "\n";
    out << "                  Version 2.0\n";
    out << "--- MPI Uniform Participation Analysis Report ---\n\n";

    out << colorize("Summary:", CYAN_COLOR) << "\n";
    out << "  - Total MPI Calls Detected: " << results.totalCalls << "\n";
    out << "  - Matched Send/Recv Pairs:  " << colorize(std::to_string(results.matchedPairs.size()), GREEN_COLOR) << "\n";
    out << "  - Unmatched Sends:          " << colorize(std::to_string(results.unmatchedSends.size()), YELLOW_COLOR) << "\n";
    out << "  - Unmatched Receives:       " << colorize(std::to_string(results.unmatchedReceives.size()), RED_COLOR) << "\n\n";

    if (!results.unmatchedSends.empty()) {
        out << colorize("[WARNING]", YELLOW_COLOR) << " Found " << results.unmatchedSends.size() << " unmatched Send calls. These may lead to deadlocks.\n";
    }
    if (!results.unmatchedReceives.empty()) {
        out << colorize("[WARNING]", RED_COLOR) << " Found " << results.unmatchedReceives.size() << " unmatched Receive calls. These will likely deadlock.\n\n";
    }

    if (!results.matchedPairs.empty()) {
        out << colorize("--- Matched Pairs ---", GREEN_COLOR) << "\n";
        for (const auto& pair : results.matchedPairs) {
            out << "  - Group (Comm: " << pair.send.communicatorName << ", Tag: " << pair.send.tag << ")\n";
            out << "    - Send: dest_rank=" << pair.send.rank << " (line " << pair.send.sourceLine << ")\n";
            out << "    - Recv: source_rank=" << pair.recv.rank << " (line " << pair.recv.sourceLine << ")\n";
        }
        out << "\n";
    }

    if (!results.unmatchedSends.empty()) {
        out << colorize("--- Unmatched Sends (Details) ---", YELLOW_COLOR) << "\n";
        for (const auto& call : results.unmatchedSends) {
            out << "  - Send to rank " << call.rank << " with tag " << call.tag << " on comm '" << call.communicatorName << "' (line " << call.sourceLine << ")\n";
        }
        out << "\n";
    }

    if (!results.unmatchedReceives.empty()) {
        out << colorize("--- Unmatched Receives (Details) ---", RED_COLOR) << "\n";
        for (const auto& call : results.unmatchedReceives) {
            out << "  - Recv from rank " << call.rank << " with tag " << call.tag << " on comm '" << call.communicatorName << "' (line " << call.sourceLine << ")\n";
        }
    }
    out << "--- End of Report ---\n";
}


// --- JSON and CSV Reporters (Unchanged) ---

void printJsonCall(const MPICallInfo& call, std::ostream& out) {
    out << "{";
    out << "\"function\":\"" << call.functionName << "\",";
    out << "\"communicator\":\"" << call.communicatorName << "\",";
    out << "\"tag\":" << call.tag << ",";
    out << "\"rank\":" << call.rank << ",";
    out << "\"sourceLine\":" << call.sourceLine;
    out << "}";
}

void JsonReporter::generateReport(const AnalysisResults& results, std::ostream& out) {
    out << "{\n";
    out << "  \"summary\": {\n";
    out << "    \"totalCalls\": " << results.totalCalls << ",\n";
    out << "    \"matchedPairs\": " << results.matchedPairs.size() << ",\n";
    out << "    \"unmatchedSends\": " << results.unmatchedSends.size() << ",\n";
    out << "    \"unmatchedReceives\": " << results.unmatchedReceives.size() << "\n";
    out << "  },\n";
    out << "  \"details\": {\n";
    
    out << "    \"matchedPairs\": [\n";
    for (size_t i = 0; i < results.matchedPairs.size(); ++i) {
        out << "      {\n";
        out << "        \"send\": "; printJsonCall(results.matchedPairs[i].send, out); out << ",\n";
        out << "        \"receive\": "; printJsonCall(results.matchedPairs[i].recv, out); out << "\n";
        out << "      }" << (i == results.matchedPairs.size() - 1 ? "" : ",") << "\n";
    }
    out << "    ],\n";

    out << "    \"unmatchedSends\": [\n";
    for (size_t i = 0; i < results.unmatchedSends.size(); ++i) {
        out << "      "; printJsonCall(results.unmatchedSends[i], out);
        out << (i == results.unmatchedSends.size() - 1 ? "" : ",") << "\n";
    }
    out << "    ],\n";

    out << "    \"unmatchedReceives\": [\n";
    for (size_t i = 0; i < results.unmatchedReceives.size(); ++i) {
        out << "      "; printJsonCall(results.unmatchedReceives[i], out);
        out << (i == results.unmatchedReceives.size() - 1 ? "" : ",") << "\n";
    }
    out << "    ]\n";
    
    out << "  }\n";
    out << "}\n";
}

void CsvReporter::generateReport(const AnalysisResults& results, std::ostream& out) {
    out << "status,type,communicator,tag,rank,source_line\n";
    
    for(const auto& pair : results.matchedPairs) {
        out << "MATCHED,SEND," << pair.send.communicatorName << "," << pair.send.tag << "," << pair.send.rank << "," << pair.send.sourceLine << "\n";
        out << "MATCHED,RECV," << pair.recv.communicatorName << "," << pair.recv.tag << "," << pair.recv.rank << "," << pair.recv.sourceLine << "\n";
    }
    for (const auto& call : results.unmatchedSends) {
        out << "UNMATCHED,SEND," << call.communicatorName << "," << call.tag << "," << call.rank << "," << call.sourceLine << "\n";
    }
    for (const auto& call : results.unmatchedReceives) {
        out << "UNMATCHED,RECV," << call.communicatorName << "," << call.tag << "," << call.rank << "," << call.sourceLine << "\n";
    }
}