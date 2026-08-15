// llvm/tools/optviz/src/IRDiffer.cpp

#include "PassDriver.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <set>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace llvm;

// CLI flags
static cl::opt<bool> UseColor(
    "diff-color",
    cl::desc("Enable ANSI-colored diff output"),
    cl::init(true)
);

static cl::opt<std::string> OutputFormat(
    "format",
    cl::desc("Output format: plain, side-by-side, or html"),
    cl::init("plain")
);

// ANSI color codes
static const char *RED   = "\x1b[31m";
static const char *GREEN = "\x1b[32m";
static const char *RESET = "\x1b[0m";

// Heuristic: skip library or intrinsic calls
static bool isLibraryCall(const std::string &inst) {
  if (inst.rfind("call", 0) != 0)
    return false;
  return inst.find("@llvm.")  != std::string::npos
      || inst.find("@__cxa")  != std::string::npos
      || inst.find("@_ZSt")   != std::string::npos;
}

// Collect instructions from a function
static std::set<std::string> collectInsts(Function &F) {
  std::set<std::string> S;
  for (auto &BB : F)
    for (auto &I : BB) {
      std::string str;
      raw_string_ostream os(str);
      I.print(os);
      S.insert(os.str());
    }
  return S;
}

// Emit HTML header
static void emitHtmlHeader() {
  outs() << "<html><head><style>";
  outs() << ".removed { background-color:#ffe6e6;}";
  outs() << ".added { background-color:#e6ffe6;}";
  outs() << "table{border-collapse:collapse;width:100%;}";
  outs() << "td{vertical-align:top;padding:4px;border:1px solid #ccc;font-family:monospace;}";
  outs() << "</style></head><body>";
}

// Emit HTML footer
static void emitHtmlFooter() {
  outs() << "</body></html>";
}

int runIRDiff(const std::string &B, const std::string &A) {
  LLVMContext Ctx;
  SMDiagnostic Err;

  auto M1 = parseIRFile(B, Err, Ctx);
  auto M2 = parseIRFile(A, Err, Ctx);
  if (!M1 || !M2) {
    errs() << "Error parsing IR files.\n";
    return 1;
  }

  // Build maps of function->instructions
  std::map<std::string, std::set<std::string>> BeforeMap, AfterMap;
  for (auto &F : *M1) if (!F.isDeclaration())
    BeforeMap[F.getName().str()] = collectInsts(F);
  for (auto &F : *M2) if (!F.isDeclaration())
    AfterMap[F.getName().str()] = collectInsts(F);

  // Collect all function names
  std::set<std::string> FuncNames;
  for (auto &p : BeforeMap) FuncNames.insert(p.first);
  for (auto &p : AfterMap)  FuncNames.insert(p.first);

  if (OutputFormat == "html")
    emitHtmlHeader();

  // Process each function
  for (auto &fname : FuncNames) {
    auto &I1 = BeforeMap[fname];
    auto I2 = AfterMap[fname];

    // Compute diffs
    std::vector<std::string> Removed, Added;
    for (auto &s : I1) {
      if (!isLibraryCall(s) && !I2.count(s)) Removed.push_back(s);
      else I2.erase(s);
    }
    for (auto &s : I2) {
      if (!isLibraryCall(s)) Added.push_back(s);
    }

    if (Removed.empty() && Added.empty()) continue;

    if (OutputFormat == "html") {
      // HTML: table per function
      outs() << "<h2>Function " << fname << "</h2>";
      outs() << "<table><tr><th>Removed</th><th>Added</th></tr>";
      size_t rows = std::max(Removed.size(), Added.size());
      for (size_t i=0; i<rows; ++i) {
        outs() << "<tr>";
        // Removed column
        outs() << "<td class='removed'>";
        if (i < Removed.size()) outs() << "- " << Removed[i];
        outs() << "</td>";
        // Added column
        outs() << "<td class='added'>";
        if (i < Added.size())   outs() << "+ " << Added[i];
        outs() << "</td></tr>";
      }
      outs() << "</table>";

    } else if (OutputFormat == "side-by-side" || OutputFormat == "plain") {
      // Header
      errs() << "--- Function " << fname << " ---\n";
      if (OutputFormat == "side-by-side") {
        // Side-by-side code as before
        size_t maxLeft = 0;
        for (auto &inst : Removed)
          maxLeft = std::max(maxLeft, inst.size()+2);
        size_t rows = std::max(Removed.size(), Added.size());
        for (size_t i=0; i<rows; ++i) {
          std::string L = (i<Removed.size()? Removed[i]:"");
          std::string R = (i<Added.size()?   Added[i]  :"");
          if (!L.empty()) errs() << (UseColor?RED:"-") << "- " << L << (UseColor?RESET:"") ;
          else            errs() << std::string(maxLeft,' ');
          size_t pad = maxLeft - ((L.empty()?0:L.size()+2));
          errs() << std::string(pad,' ') << " | ";
          if (!R.empty()) errs() << (UseColor?GREEN:"+") << "+ " << R << (UseColor?RESET:"");
          errs() << "\n";
        }
        errs() << "\n";
      } else {
        // Plain
        for (auto &inst : Removed)
          errs() << (UseColor?RED:"-") << "- " << inst << (UseColor?RESET:"") << "\n";
        for (auto &inst : Added)
          errs() << (UseColor?GREEN:"+") << "+ " << inst << (UseColor?RESET:"") << "\n";
        errs() << "\n";
      }
    } else {
      errs() << "Unknown format: " << OutputFormat << "\n";
      return 1;
    }
  }

  if (OutputFormat == "html")
    emitHtmlFooter();

  return 0;
}