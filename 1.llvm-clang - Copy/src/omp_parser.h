// filepath: c:\openmp to _ir_mapper_llvm\src\omp_parser.h
#ifndef OMP_PARSER_H
#define OMP_PARSER_H

#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <vector>
#include <string>

struct DirectiveInfo {
    std::string directive;
    unsigned line;
    std::string additionalInfo;
};

class OMPParser : public clang::RecursiveASTVisitor<OMPParser> {
public:
    OMPParser();

    bool VisitPragma(clang::PragmaComment *pragma);
    bool VisitOMPParallelDirective(clang::OMPParallelDirective *directive);
    bool VisitOMPTaskLoopDirective(clang::OMPTaskLoopDirective *directive);
    // Add more OpenMP directive visit methods as needed

    const std::vector<DirectiveInfo>& getDirectives() const;

private:
    std::vector<DirectiveInfo> directives;
};

#endif // OMP_PARSER_H