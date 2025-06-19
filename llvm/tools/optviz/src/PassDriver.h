// llvm/tools/optviz/include/PassDriver.h
#pragma once

#include <string>

// Entry point called from main()
int runDriver(int argc, char **argv);

// IR-diff engine
int runIRDiff(const std::string &before, const std::string &after);