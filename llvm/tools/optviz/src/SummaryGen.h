// llvm/tools/optviz/src/SummaryGen.h

#pragma once
#include <string>

namespace SummaryGen {
  // Sends the before/after IR snippet to an LLM and returns the summary.
  std::string summarize(const std::string &beforeIR,
                        const std::string &afterIR);
}