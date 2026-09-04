#pragma once
#include <string>
#include <vector>

// Information extracted from a single MPI call instruction
struct MPICallInfo {
    std::string functionName;
    long long rank = -1;
    long long tag = -1;
    std::string communicatorName = "UNKNOWN";
    bool isSend = false;
    // For error reporting
    unsigned sourceLine = 0; 
};

// Represents a matched Send/Recv pair
struct MatchedPair {
    MPICallInfo send;
    MPICallInfo recv;
};

// The final results of the analysis engine
struct AnalysisResults {
    std::vector<MatchedPair> matchedPairs;
    std::vector<MPICallInfo> unmatchedSends;
    std::vector<MPICallInfo> unmatchedReceives;
    unsigned int totalCalls = 0;
};