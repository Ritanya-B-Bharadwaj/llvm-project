OpenMP Test Generator
Core Idea
The core idea is to automate OpenMP test case creation for LLVM Clang by analyzing PR changes and generating tailored test skeletons. The system:

Fetches PR Diffs: Identifies new OpenMP pragmas from GitHub PRs.
Analyzes Existing Tests: Scans Clang's test directory to extract patterns like flags, directives, and runtime calls.
Discovers Patterns: Aggregates insights to guide test generation.
Generates Tests: Produces LLVM lit tests with valid, invalid, and edge-case scenarios.
Reduces Manual Effort: Minimizes user input and prompt complexity for efficient test creation.

This automation ensures consistent, comprehensive testing of OpenMP directives, particularly for parsing, semantic analysis, and code generation

Codebase:
This codebase, comprising generator.py and getdiff.py, automates the generation of OpenMP test cases for the LLVM Clang compiler. It analyzes GitHub pull request (PR) diffs to identify new or modified OpenMP pragmas, then generates LLVM lit test skeletons to validate these pragmas across compiler stages (parsing, semantic analysis, code generation). The system uses static analysis, pattern discovery, and optional AI-driven test generation to produce robust test suites.

generator.py: Contains the core logic for:
Analyzing Test Files: Extracts patterns like OpenMP directives, clauses, compiler flags, and runtime calls using AdvancedASTAnalyzer.
Pattern Discovery: Identifies common patterns (e.g., flags, _kmpc_ calls) with PatternDiscoveryEngine.
Test Generation: Creates test skeletons using AdvancedTestGenerator, either via templates or an AI API (e.g., Gemini).
Orchestration: The generate_openmp_test_skeleton function coordinates analysis and generation.


getdiff.py: Fetches PR diffs from GitHub, extracts OpenMP pragmas, and generates test files (e.g., test_<pragma>.cpp) using generator.py.

The system ensures new OpenMP directives are thoroughly tested, enhancing Clang's reliability for OpenMP features.

Example Walkthrough: Generating Tests for #pragma omp parallel
This walkthrough details generating a test skeleton for #pragma omp parallel, highlighting key components, including the _analyze_semantic_contexts function, AST dumping, and IR pattern matching.
Step 1: Fetch PR Diff (getdiff.py)

Input: Run:python getdiff.py 1234 --repo <repo_name> --token <github_token> --api_key <gemini_api_key> --user_requirements <compiler_stage> --base_directory <base_directory>

base_directory is the path to the test cases, ie, ~/llvm-project/clang/test/OpenMP

Action: get_pr_diff fetches the PR #1234 diff using GitHub's API.
Output: A diff, e.g.:+#pragma omp parallel
+{
+  int x = omp_get_thread_num();
+}


Processing: extract_openmp_pragmas identifies #pragma omp parallel, normalizes it to parallel, and triggers test generation. All this is done using regular expressions.

Step 2: Initialize Components (generate_openmp_test_skeleton)

Setup: Initializes AdvancedASTAnalyzer, PatternDiscoveryEngine, and AdvancedTestGenerator.
Test Type: Sets test_type="codegen" based on requirements.
File Search: Locates test files like parallel_codegen.cpp in ~/llvm-project/clang/test/OpenMP.

Step 3: Analyze Test Files (AdvancedASTAnalyzer)

File Analysis: For each test file:
Extracts Patterns: Identifies RUN lines, CHECK lines, error patterns, and pragmas.
AST Dumping: Uses clang -ast-dump to extract OpenMP directives (e.g., OMPParallelDirective) and clauses (e.g., PrivateClause). It simplifies extraction of OpenMP constructs using regex patterns (e.g., \bOMP\w*Directive\b, \bOMP\w*Clause\b), providing a structured representation of the code's syntax and semantics, which is more reliable than parsing raw source code.
Semantic Contexts: Calls _analyze_semantic_contexts to detect code contexts:
```
def _analyze_semantic_contexts(self, content: str) -> List[str]:
    contexts = []
    if re.search(r'void\s+\w+\s*\([^)]*\)\s*{', content):
        contexts.append('function_scope')
    if re.search(r'for\s*\([^)]*\)', content):
        contexts.append('loop_context')
    if re.search(r'(class|struct)\s+\w+', content):
        contexts.append('class_context')
    if re.search(r'template\s*<', content):
        contexts.append('template_context')
    pragma_count = len(re.findall(r'#pragma omp', content))
    if pragma_count > 1:
        contexts.append('nested_pragmas')
    if re.search(r'_kmpc_', content):
        contexts.append('runtime_calls')
    return contexts
```

Usage: This function is called within analyze_test_file to populate the semantic_contexts field of a TestPattern object. It identifies contexts like function_scope, loop_context, or runtime_calls, which inform test generation by indicating where pragmas are used (e.g., in loops or functions).
It helps tailor tests to specific code structures, ensuring coverage of relevant scenarios (e.g., nested pragmas, runtime calls for codegen).


IR Pattern Matching: For codegen tests, extracts patterns like _kmpc_fork_call from CHECK-CODEGEN lines. Significance: IR (Intermediate Representation) pattern matching verifies that OpenMP directives are correctly lowered to LLVM IR, ensuring proper code generation (e.g., _kmpc_ runtime calls for parallel regions).
```
Output: A TestPattern object, e.g.:TestPattern(
    test_type=TestType.CODEGEN,
    file_path=Path("parallel_codegen.cpp"),
    pragma_usage="#pragma omp parallel",
    compiler_flags=["-fopenmp", "-emit-llvm", "-S"],
    codegen_checks=["call void @_kmpc_fork_call"],
    kmpc_calls=["_kmpc_fork_call"],
    semantic_contexts=["function_scope", "runtime_calls"],
    complexity_score=3.5,
    coverage_areas=["parallel_regions", "runtime_calls"]
)
```


Step 4: Discover Patterns (PatternDiscoveryEngine)

Input: List of TestPattern objects.
Action: Aggregates:
Common Flags: E.g., -fopenmp, -emit-llvm.
Directive Insights: Frequency and clauses of #pragma omp parallel.
kmpc Patterns: Tracks _kmpc_fork_call usage.
Coverage Gaps: Identifies missing areas like nested_constructs.

```
Output: Insights like:{
    'common_flags': {'most_common': ['-fopenmp', '-emit-llvm', '-S']},
    'kmpc_patterns': {'_kmpc_fork_call': {'frequency': 2, 'associated_directives': ['parallel']}},
    'coverage_gaps': ['nested_constructs']
}
```


Step 5: Generate Test Skeleton (AdvancedTestGenerator)

Input: TestGenerationRequest for pragma_name="parallel", test_type="codegen".
Context: Includes semantic_contexts (e.g., function_scope, runtime_calls) to ensure tests reflect relevant code structures.
Generation: Produces a test skeleton, either via mock template or Gemini API, e.g.:
```
// RUN: %clang_cc1 - -Wuninitialized -ferror-limit -fopenmp -fopenmp-simd -std=c++11 -verify -DOMP51 -fopenmp-version=30 -fopenmp-version=31 -fopenmp-version=40 -fopenmp-version=45 -fopenmp-version=50 -verify=expected,ge40 %s

// Test basic parallel usage
void test_basic() {
  // expected-error@+1 {expected 'for' loop after '#pragma omp parallel'}
  #pragma omp parallel
  int x = 0;
}

// Test parallel with valid private clause
void test_with_private_clause() {
  int i;
  // No error expected for valid usage
  #pragma omp parallel private(i)
  for (i = 0; i < 10; i++) {
    int y = 0;
  }
}

// Test invalid clause
void test_invalid_clause() {
  // expected-error@+1 {unexpected OpenMP clause 'order' in directive '#pragma omp parallel'}
  #pragma omp parallel order(concurrent)
  for (int j = 0; j < 5; j++) {
  }
}

// Test duplicate clause
void test_duplicate_clause() {
  int k;
  // expected-error@+1 {duplicate 'private' clause on directive '#pragma omp parallel'}
  #pragma omp parallel private(k) private(k)
  for (k = 0; k < 5; k++) {
  }
}

// Test default(none) missing clause
void test_default_none() {
  int m;
  // expected-error@+2 {variable 'm' must appear in a data-sharing clause when 'default(none)' is specified}
  #pragma omp parallel default(none)
  for (m = 0; m < 5; m++) {
  }
}
```


Step 6: Save Test File (getdiff.py)

Action: Saves the test as test_parallel.cpp.

Why Are We Doing This: Stress on Reducing the Prompt
The codebase automates OpenMP test generation to reduce manual effort and ensure robust testing of Clang's OpenMP support. A key focus is reducing prompt complexity to make the system efficient:

Minimal User Input: Requires only PR number, repository, tokens, and high-level requirements (e.g., "codegen"). The system infers test types and contexts using _analyze_semantic_contexts and pattern discovery.
Pattern-Driven Generation: Extracts patterns (e.g., flags, _kmpc_ calls) from existing tests, reducing the need for detailed AI prompts or user specifications.
AST Dumping: Simplifies directive and clause extraction with regex, avoiding complex source code parsing and enabling precise test generation.
IR Pattern Matching: Ensures correct codegen by verifying _kmpc_ calls, critical for backend validation, without requiring users to specify IR details.
Mock Fallback: Uses templates when no API key is provided, eliminating external dependencies.
Concise AI Prompts: When using Gemini, constructs focused prompts with relevant context (e.g., semantic_contexts, kmpc patterns), minimizing token usage.

This approach reduces development time, ensures consistency, and scales to handle new OpenMP features.
Conclusion
The OpenMP Test Generator streamlines LLVM Clang test creation by automating pragma detection, pattern analysis, and test generation. The _analyze_semantic_contexts function ensures tests reflect relevant code contexts, while AST dumping and IR pattern matching enhance precision in validating OpenMP directives. By minimizing prompt complexity, the system enables efficient, comprehensive testing, making it a valuable tool for LLVM developers enhancing OpenMP support.
