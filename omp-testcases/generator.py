import glob
import json
import os
import re
import requests
import subprocess
import tempfile
import ast
import hashlib
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Set, Any, Union
from collections import defaultdict, Counter
import logging
from typing import List, Dict

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

print("✅ Core imports loaded successfully")

class TestType(Enum):
    PARSE_SEMA = "messages"
    AST_PRINT = "ast-print"
    CODEGEN = "codegen"
    IRGEN = "irgen"
    RUNTIME = "runtime"

class PragmaCategory(Enum):
    WORKSHARING = "worksharing"
    SYNCHRONIZATION = "synchronization"
    DATA_ENVIRONMENT = "data_environment"
    DEVICE = "device"
    TASKING = "tasking"
    SIMD = "simd"
    META = "meta"

class CompilerStage(Enum):
    FRONTEND = "frontend"
    MIDDLE_END = "middle_end"
    BACKEND = "backend"
    RUNTIME = "runtime"

@dataclass
class DirectivePattern:
    name: str
    category: PragmaCategory
    clauses: List[str] = field(default_factory=list)
    syntax_variants: List[str] = field(default_factory=list)
    common_errors: List[str] = field(default_factory=list)
    valid_contexts: List[str] = field(default_factory=list)

@dataclass
class TestPattern:
    test_type: TestType
    file_path: Path
    pragma_usage: str
    compiler_flags: List[str]
    check_patterns: List[str]
    error_patterns: List[str]
    ast_dump_sections: List[str]
    codegen_checks: List[str] = field(default_factory=list)  # New for codegen
    kmpc_calls: List[str] = field(default_factory=list)      # New for codegen
    directive_patterns: List[DirectivePattern] = field(default_factory=list)
    semantic_contexts: List[str] = field(default_factory=list)
    complexity_score: float = 0.0
    coverage_areas: List[str] = field(default_factory=list)  # Changed to list
    dependencies: List[str] = field(default_factory=list)

@dataclass
class TestGenerationRequest:
    pragma_name: str
    test_type: str  # Changed to str to accept string input
    requirements: List[str] = field(default_factory=list)  # Changed to list
    target_stage: CompilerStage = CompilerStage.FRONTEND
    include_negative_tests: bool = True
    include_edge_cases: bool = True
    complexity_level: str = "medium"

print("Enhanced data structures defined")

class AdvancedASTAnalyzer:
    def __init__(self, clang_path: str = "clang"):
        self.clang_path = clang_path
        self.validate_clang_installation()
        self.detected_patterns = {}
        self.ast_node_frequencies = Counter()
        self.clause_combinations = defaultdict(set)
        self.kmpc_patterns = Counter()  # New for codegen

    def validate_clang_installation(self):
        try:
            result = subprocess.run(['clang', '--version'],
                                  capture_output=True, text=True, timeout=10)
            if result.returncode != 0:
                raise ValueError(f"Clang not functional: {result.stderr}")
            logger.info(f"Clang validated: {result.stdout.split()[0]}")
        except Exception as e:
            raise ValueError(f"Invalid Clang installation: {e}")

    def analyze_test_file(self, file_path: Path) -> TestPattern:
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()

            basic_patterns = self._extract_basic_patterns(content)
            ast_dump = self._get_ast_dump(content)
            ast_sections = self._extract_ast_sections(ast_dump) if ast_dump else {
                'omp_directives': [], 'omp_clauses': [], 'errors': []
            }
            codegen_info = self._analyze_codegen_patterns(content, basic_patterns['test_type'])  # New for codegen

            directive_patterns = self._detect_directive_patterns(content)
            semantic_contexts = self._analyze_semantic_contexts(content)
            coverage_areas = self._analyze_coverage_areas(content, basic_patterns)
            complexity_score = self._calculate_complexity_score(content, {'nodes': []})
            dependencies = self._extract_dependencies(content)

            return TestPattern(
                test_type=basic_patterns['test_type'],
                file_path=file_path,
                pragma_usage=basic_patterns['pragma_usage'],
                compiler_flags=basic_patterns['compiler_flags'],
                check_patterns=basic_patterns['check_patterns'],
                error_patterns=basic_patterns['error_patterns'],
                ast_dump_sections=[
                    section for sublist in ast_sections.values() for section in sublist
                ],
                codegen_checks=codegen_info['codegen_checks'],  # New for codegen
                kmpc_calls=codegen_info['kmpc_calls'],         # New for codegen
                directive_patterns=directive_patterns,
                semantic_contexts=semantic_contexts,
                complexity_score=complexity_score,
                coverage_areas=coverage_areas,
                dependencies=dependencies
            )
        except Exception as e:
            logger.error(f"Failed to analyze {file_path}: {e}")
            raise

    def _extract_basic_patterns(self, content: str) -> Dict[str, Any]:
        try:
            run_lines = []
            run_matches = re.finditer(r'//\s*RUN:\s*(.+?)(?=\n|$)', content, re.MULTILINE)
            for match in run_matches:
                run_lines.append(match.group(1).strip())

            check_patterns = []
            check_matches = re.finditer(r'//\s*CHECK(?:-[A-Z]+)?:\s*(.+?)(?=\n|$)', content, re.MULTILINE)
            for match in check_matches:
                check_patterns.append(match.group(1).strip())

            error_patterns = []
            try:
                error_matches = re.finditer(
                    r'//\s*expected-(?:error|warning|note)\s*(?:@[^\s}]+)?\s*\{\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}\}',
                    content,
                    re.MULTILINE | re.DOTALL
                )
                for match in error_matches:
                    error_text = match.group(1).strip()
                    if error_text and len(error_text) > 5:
                        error_patterns.append(error_text)
            except re.error as e:
                logger.warning(f"Complex error regex failed: {e}. Using simpler approach.")
                simple_matches = re.finditer(
                    r'//\s*expected-(?:error|warning|note)[^{]*\{\{([^}]+)\}\}',
                    content,
                    re.MULTILINE
                )
                for match in simple_matches:
                    error_text = match.group(1).strip()
                    if error_text and len(error_text) > 5:
                        error_patterns.append(error_text)

            pragma_usage = self._extract_pragma_patterns(content)
            test_type = self._determine_test_type(run_lines, content)
            compiler_flags = self._extract_compiler_flags(run_lines)

            return {
                'compiler_flags': compiler_flags,
                'check_patterns': check_patterns,
                'error_patterns': error_patterns,
                'pragma_usage': pragma_usage,
                'test_type': test_type
            }
        except Exception as e:
            logger.error(f"Error in _extract_basic_patterns: {e}")
            return {
                'compiler_flags': ['-fopenmp', '-verify'],
                'check_patterns': [],
                'error_patterns': [],
                'pragma_usage': "",
                'test_type': TestType.PARSE_SEMA
            }

    def _analyze_codegen_patterns(self, content: str, test_type: TestType) -> Dict[str, List[str]]:
        """Analyze codegen-specific patterns including _kmpc calls"""
        codegen_checks = []
        kmpc_calls = []

        if test_type != TestType.CODEGEN:
            return {'codegen_checks': codegen_checks, 'kmpc_calls': kmpc_calls}

        # Extract CHECK lines specific to codegen
        codegen_matches = re.finditer(r'//\s*CHECK-CODEGEN:\s*(.+?)(?=\n|$)', content, re.MULTILINE)
        for match in codegen_matches:
            check = match.group(1).strip()
            if check:
                codegen_checks.append(check)

        # Look for _kmpc runtime calls in CHECK lines
        kmpc_pattern = r'_kmpc_\w+\s*\('
        for check in codegen_checks + [content]:
            kmpc_matches = re.finditer(kmpc_pattern, check)
            for match in kmpc_matches:
                call = match.group(0).rstrip('(').strip()
                if call not in kmpc_calls:
                    kmpc_calls.append(call)
                    self.kmpc_patterns[call] += 1

        # Extract IR patterns if present
        ir_matches = re.finditer(r'//\s*CHECK-IR:\s*(.+?)(?=\n|$)', content, re.MULTILINE)
        for match in ir_matches:
            check = match.group(1).strip()
            if check and check not in codegen_checks:
                codegen_checks.append(check)

        return {'codegen_checks': codegen_checks, 'kmpc_calls': kmpc_calls}

    def _perform_ast_analysis(self, content: str, file_path: Path) -> Dict[str, Any]:
        try:
            ast_dump = self._get_ast_dump(content)
            if not ast_dump or "AST dump failed" in ast_dump:
                return {'ast_sections': [], 'nodes': []}

            omp_nodes = re.findall(r'(OMP\w+(?:Directive|Clause))', ast_dump)
            self.ast_node_frequencies.update(omp_nodes)
            ast_sections = self._extract_ast_sections(ast_dump)

            return {
                'ast_sections': ast_sections,
                'nodes': omp_nodes,
                'dump': ast_dump
            }
        except Exception as e:
            logger.warning(f"AST analysis failed for {file_path}: {e}")
            return {'ast_sections': [], 'nodes': []}

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
        if re.search(r'_kmpc_', content):  # New for codegen
            contexts.append('runtime_calls')
        return contexts

    def _extract_ast_sections(self, ast_dump: str) -> Dict[str, List[str]]:
        sections = {
            'omp_directives': [],
            'omp_clauses': [],
            'errors': []
        }
        if not ast_dump:
            return sections

        # Common regex patterns for OpenMP AST constructs
        directive_pattern = re.compile(r'\bOMP\w*Directive\b')  # matches OMPParallelDirective, OMPTargetTeamsDistributeDirective, etc.
        clause_pattern = re.compile(r'\b(OMP)?\w*Clause\b')     # matches OMPClause, PrivateClause, SharedClause, etc.
        error_pattern = re.compile(r'\b(error:|warning:|note:)\b', re.IGNORECASE)

        for line in ast_dump.splitlines():
            line = line.strip()
            if not line:
                continue

            if directive_pattern.search(line):
                sections['omp_directives'].append(line)
            elif clause_pattern.search(line):
                sections['omp_clauses'].append(line)
            elif error_pattern.search(line):
                sections['errors'].append(line)

        return sections

    def _detect_directive_patterns(self, content: str) -> List[DirectivePattern]:
        patterns = []
        pragma_lines = re.findall(r'#pragma omp (.+)', content)
        for pragma in pragma_lines:
            parts = pragma.split()
            if not parts:
                continue
            directive_name = parts[0]
            clauses = parts[1:] if len(parts) > 1 else []
            category = self._categorize_directive(directive_name)
            syntax_variants = self._extract_syntax_variants(pragma, content)
            common_errors = self._detect_directive_errors(directive_name, content)

            pattern = DirectivePattern(
                name=directive_name,
                category=category,
                clauses=clauses,
                syntax_variants=syntax_variants,
                common_errors=common_errors
            )
            patterns.append(pattern)
        return patterns

    def _categorize_directive(self, directive_name: str) -> PragmaCategory:
        directive_lower = directive_name.lower()
        if any(ws in directive_lower for ws in ['parallel', 'for', 'sections', 'single']):
            return PragmaCategory.WORKSHARING
        elif any(sync in directive_lower for sync in ['barrier', 'critical', 'atomic', 'flush']):
            return PragmaCategory.SYNCHRONIZATION
        elif any(data in directive_lower for data in ['private', 'shared', 'reduction']):
            return PragmaCategory.DATA_ENVIRONMENT
        elif any(dev in directive_lower for dev in ['target', 'teams', 'distribute']):
            return PragmaCategory.DEVICE
        elif any(task in directive_lower for task in ['task', 'taskloop', 'taskwait']):
            return PragmaCategory.TASKING
        elif 'simd' in directive_lower:
            return PragmaCategory.SIMD
        else:
            return PragmaCategory.META

    def _extract_syntax_variants(self, pragma: str, content: str) -> List[str]:
        variants = []
        directive_name = pragma.split()[0]
        similar_pragmas = re.findall(rf'#pragma omp {re.escape(directive_name)}[^\n]*', content)
        for similar in similar_pragmas:
            if similar.strip() != f"#pragma omp {pragma}".strip():
                variants.append(similar.replace('#pragma omp ', ''))
        return variants

    def _detect_directive_errors(self, directive_name: str, content: str) -> List[str]:
        errors = []
        error_matches = re.finditer(
            r'//\s*expected-(?:error|warning|note)\s*\{\{((?:[^{}]|\{[^{}]*\})*?)\}\}',
            content,
            re.DOTALL
        )
        for match in error_matches:
            error_text = match.group(1).strip()
            if directive_name.lower() in error_text.lower() or 'clause' in error_text.lower():
                errors.append(error_text)
        return errors

    
    def _analyze_coverage_areas(self, content: str, basic_analysis: Dict) -> List[str]:
        coverage = []

        # === Errors and Checks ===
        if basic_analysis.get('error_patterns'):
            coverage.append('error_handling')
        if basic_analysis.get('check_patterns'):
            coverage.append('ast_validation')

        # === Compiler Flags ===
        compiler_flags = ' '.join(basic_analysis.get('compiler_flags', []))
        if 'fopenmp' in compiler_flags:
            coverage.append('openmp_enabled')

        # === OpenMP Constructs in Content ===
        omp_pragmas = {
            'data_sharing': r'#pragma omp.*\b(private|shared|firstprivate|lastprivate|threadprivate)\b',
            'reductions': r'#pragma omp.*\breduction\b',
            'loop_constructs': r'#pragma omp.*\b(for|do)\b',
            'parallel_regions': r'#pragma omp.*\bparallel\b',
            'tasking': r'#pragma omp.*\b(task|taskgroup|taskloop)\b',
            'synchronization': r'#pragma omp.*\b(barrier|atomic|critical|flush|ordered)\b',
            'simd': r'#pragma omp.*\bsimd\b',
            'target_offloading': r'#pragma omp.*\btarget\b',
            'teams_construct': r'#pragma omp.*\bteams\b',
            'distribute_construct': r'#pragma omp.*\bdistribute\b',
            'declare_simd': r'#pragma omp declare simd',
            'declare_target': r'#pragma omp declare target',
        }

        for feature, pattern in omp_pragmas.items():
            if re.search(pattern, content):
                coverage.append(feature)

        # === Runtime Calls (e.g., from LLVM lowering or Clang output) ===
        runtime_patterns = [
            r'_kmpc_',                # Generic runtime API prefix
            r'__tgt_',                # Offloading runtime
            r'omp_get_',              # OpenMP runtime queries (e.g., thread num)
        ]
        for pattern in runtime_patterns:
            if re.search(pattern, content):
                coverage.append('runtime_calls')
                break

        # === General constructs ===
        if re.search(r'\bfor\s*\(', content):
            coverage.append('loop_constructs')

        return sorted(set(coverage))

    def _calculate_complexity_score(self, content: str, ast_analysis: Dict) -> float:
        score = 0.0
        pragma_count = len(re.findall(r'#pragma omp', content))
        score += pragma_count * 0.5
        score += len(ast_analysis.get('nodes', [])) * 0.3
        error_count = len(re.findall(r'expected-(?:error|warning|note)', content))
        score += error_count * 0.4
        check_count = len(re.findall(r'// CHECK', content))
        score += check_count * 0.2
        kmpc_count = len(re.findall(r'_kmpc_', content))  # New for codegen
        score += kmpc_count * 0.3
        return min(score, 10.0)

    def _extract_dependencies(self, content: str) -> List[str]:
        deps = []
        if re.search(r'#include\s*[<"]', content):
            deps.append('headers')
        if re.search(r'std::', content):
            deps.append('cpp_stdlib')
        if re.search(r'omp_', content) or re.search(r'_kmpc_', content):
            deps.append('openmp_runtime')
        return deps

    def _extract_compiler_flags(self, run_lines: List[str]) -> List[str]:
        flags = set()
        for line in run_lines:
            if not line.strip():
                continue
            parts = line.split()
            i = 0
            while i < len(parts):
                part = parts[i]
                if part.startswith('%') or part == '-' or len(part) <= 1:
                    i += 1
                    continue
                if part.startswith('-'):
                    if part in ['-emit-llvm', '-S', '-o', '-std', '-target', '-triple']:
                        if i + 1 < len(parts) and not parts[i + 1].startswith('-') and not parts[i + 1].startswith('%'):
                            flags.add(f"{part} {parts[i + 1]}")
                            i += 2
                        else:
                            flags.add(part)
                            i += 1
                    else:
                        flags.add(part)
                        i += 1
                else:
                    i += 1
        filtered_flags = []
        for flag in sorted(flags):
            if (not flag.startswith('-D') or 'RECORD' not in flag) and \
               flag not in ['--check-prefix', '-'] and \
               len(flag) > 1:
                filtered_flags.append(flag)
        return filtered_flags

    def _determine_test_type(self, flags: List[str], content: str) -> TestType:
        flag_str = ' '.join(flags).lower()
        content_lower = content.lower()
        if 'verify' in flag_str or 'expected-' in content_lower:
            return TestType.PARSE_SEMA
        elif 'ast-print' in flag_str or 'ast-dump' in flag_str:
            return TestType.AST_PRINT
        elif any(cg in flag_str for cg in ['emit-llvm', 'emit-codegen']) or '_kmpc_' in content_lower:
            return TestType.CODEGEN
        elif 'irgen' in content_lower:
            return TestType.IRGEN
        else:
            return TestType.PARSE_SEMA

    def _extract_pragma_patterns(self, content: str) -> str:
        pragma_lines = []
        matches = re.finditer(r'^[^\n]*#pragma omp[^\n]*', content, re.MULTILINE)
        for match in matches:
            line = match.group(0).strip()
            line = re.sub(r'^[^#]*(?=#pragma)', '', line)
            if '// expected-' not in line:
                line = re.sub(r'\s*//.*$', '', line)
            if line.startswith('#pragma omp') and len(line.strip()) > 12:
                pragma_lines.append(line.strip())
        return '\n'.join(pragma_lines)

    def _get_ast_dump(self, code: str) -> Optional[str]:
        try:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.cpp', delete=False) as f:
                f.write(code)
                temp_file = f.name
            cmd = [
                'clang',                    
                '-Xclang', '-ast-dump',    
                '-fopenmp',                
                '-std=c++11',               
                '-ferror-limit=0',          
                '-Os',                      
                temp_file                   
            ]

            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30,
                env=os.environ
            )
            os.unlink(temp_file)
            logger.debug(f"AST dump stdout: {result.stdout[:1000]}...")
            logger.debug(f"AST dump stderr: {result.stderr}")
            if result.returncode == 0 or result.stdout:
                return result.stdout if result.stdout else None
            else:
                logger.warning(f"AST dump failed: {result.stderr}")
                return None
        except Exception as e:
            logger.error(f"AST dump error: {e}")
            return None

print("Advanced AST Analyzer implemented")

class PatternDiscoveryEngine:
    def __init__(self):
        self.pattern_cache = {}
        self.directive_statistics = defaultdict(dict)
        self.clause_patterns = defaultdict(set)
        self.error_patterns = defaultdict(list)
        self.kmpc_patterns = defaultdict(set)  # New for codegen

    def discover_patterns(self, test_patterns: List[TestPattern]) -> Dict[str, Any]:
        if not test_patterns:
            return {}
        discovery_results = {
            'common_flags': self._discover_common_flags(test_patterns),
            'directive_insights': self._analyze_directive_usage(test_patterns),
            'error_categories': self._categorize_errors(test_patterns),
            'complexity_distribution': self._analyze_complexity_distribution(test_patterns),
            'coverage_gaps': self._identify_coverage_gaps(test_patterns),
            'recommended_patterns': self._recommend_test_patterns(test_patterns),
            'kmpc_patterns': self._analyze_kmpc_patterns(test_patterns)  # New for codegen
        }
        return discovery_results

    def _discover_common_flags(self, patterns: List[TestPattern]) -> Dict[str, Any]:
        flag_frequency = Counter()
        flag_combinations = defaultdict(int)
        test_type_flags = defaultdict(list)
        for pattern in patterns:
            for flag in pattern.compiler_flags:
                flag_frequency[flag] += 1
                test_type_flags[pattern.test_type].append(flag)
            flag_combo = tuple(sorted(pattern.compiler_flags))
            flag_combinations[flag_combo] += 1
        threshold = max(1, len(patterns) * 0.3)
        common_flags = [flag for flag, count in flag_frequency.items() if count >= threshold]
        return {
            'most_common': common_flags,
            'frequency_distribution': dict(flag_frequency.most_common(10)),
            'by_test_type': {tt.value: Counter(flags).most_common(5)
                           for tt, flags in test_type_flags.items()},
            'common_combinations': [combo for combo, count in flag_combinations.items() if count > 1]
        }


    #we finally make use of the most common ones for the prompt

    def _analyze_directive_usage(self, patterns: List[TestPattern]) -> Dict[str, Any]:
        directive_stats = defaultdict(lambda: {
            'frequency': 0,
            'categories': set(),
            'common_clauses': Counter(),
            'contexts': Counter(),
            'complexity_scores': []
        })
        for pattern in patterns:
            for directive_pattern in pattern.directive_patterns:
                name = directive_pattern.name
                directive_stats[name]['frequency'] += 1
                directive_stats[name]['categories'].add(directive_pattern.category.value)
                directive_stats[name]['common_clauses'].update(directive_pattern.clauses)
                directive_stats[name]['complexity_scores'].append(pattern.complexity_score)
        insights = {}
        for directive, stats in directive_stats.items():
            insights[directive] = {
                'frequency': stats['frequency'],
                'categories': list(stats['categories']),
                'top_clauses': dict(stats['common_clauses'].most_common(5)),
                'avg_complexity': sum(stats['complexity_scores']) / len(stats['complexity_scores'])
                                if stats['complexity_scores'] else 0
            }
        return insights

    def _categorize_errors(self, patterns: List[TestPattern]) -> Dict[str, List[str]]:
        error_categories = defaultdict(list)
        for pattern in patterns:
            for error in pattern.error_patterns:
                error_lower = error.lower()
                if any(kw in error_lower for kw in ['clause', 'expected']):
                    error_categories['clause_errors'].append(error)
                elif any(kw in error_lower for kw in ['variable', 'identifier']):
                    error_categories['variable_errors'].append(error)
                elif any(kw in error_lower for kw in ['syntax', 'unexpected']):
                    error_categories['syntax_errors'].append(error)
                elif any(kw in error_lower for kw in ['type', 'cannot']):
                    error_categories['type_errors'].append(error)
                else:
                    error_categories['other_errors'].append(error)
        for category in error_categories:
            error_categories[category] = list(set(error_categories[category]))[:10]
        return dict(error_categories)

    def _analyze_complexity_distribution(self, patterns: List[TestPattern]) -> Dict[str, Any]:
        complexities = [p.complexity_score for p in patterns]
        if not complexities:
            return {}
        return {
            'mean': sum(complexities) / len(complexities),
            'min': min(complexities),
            'max': max(complexities),
            'distribution': {
                'simple': len([c for c in complexities if c < 3]),
                'medium': len([c for c in complexities if 3 <= c < 7]),
                'complex': len([c for c in complexities if c >= 7])
            }
        }

    def _identify_coverage_gaps(self, patterns: List[TestPattern]) -> List[str]:
        all_coverage = set()
        for pattern in patterns:
            all_coverage.update(pattern.coverage_areas)
        important_areas = {
            'error_handling', 'ast_validation', 'data_sharing', 'reductions',
            'loop_constructs', 'nested_constructs', 'template_contexts',
            'exception_handling', 'runtime_library', 'runtime_calls'
        }
        gaps = important_areas - all_coverage
        return list(gaps)

    def _analyze_kmpc_patterns(self, patterns: List[TestPattern]) -> Dict[str, Any]:
        """Analyze patterns related to _kmpc runtime calls"""
        kmpc_stats = defaultdict(lambda: {'frequency': 0, 'directives': set(), 'checks': set()})
        for pattern in patterns:
            for kmpc_call in pattern.kmpc_calls:
                kmpc_stats[kmpc_call]['frequency'] += 1
                for directive in pattern.directive_patterns:
                    kmpc_stats[kmpc_call]['directives'].add(directive.name)
                kmpc_stats[kmpc_call]['checks'].update(pattern.check_patterns)
        return {
            k: {
                'frequency': v['frequency'],
                'associated_directives': list(v['directives']),
                'common_checks': list(v['checks'])[:3]
            } for k, v in kmpc_stats.items()
        }

    def _recommend_test_patterns(self, patterns: List[TestPattern]) -> List[str]:
        recommendations = []
        test_types = set(p.test_type for p in patterns)
        all_test_types = set(TestType)
        missing_types = all_test_types - test_types
        for test_type in missing_types:
            recommendations.append({
                'type': 'missing_test_type',
                'test_type': test_type.value,
                'priority': 'high' if test_type == TestType.PARSE_SEMA else 'medium'
            })
        avg_complexity = sum(p.complexity_score for p in patterns) / len(patterns) if patterns else 0
        if avg_complexity < 3:
            recommendations.append({
                'type': 'increase_complexity',
                'suggestion': 'Add more complex test scenarios with nested constructs or runtime calls',
                'priority': 'medium'
            })
        kmpc_calls = set(sum([p.kmpc_calls for p in patterns], []))
        if not kmpc_calls and any(p.test_type == TestType.CODEGEN for p in patterns):
            recommendations.append({
                'type': 'missing_kmpc',
                'suggestion': 'Add tests verifying _kmpc runtime calls',
                'priority': 'high'
            })
        return recommendations

print("✅ Pattern Discovery Engine implemented")

class AdvancedTestGenerator:
    def __init__(self, api_key: str, model: str = "gemini-2.0-flash"):
        self.api_key = api_key
        self.model = model
        self.api_url = f"https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent?key={api_key}"
        self.generation_cache = {}

    def generate_test(self, request: TestGenerationRequest,
                     discovered_patterns: Dict[str, Any],
                     existing_patterns: List[TestPattern]) -> str:
        context = self._build_generation_context(request, discovered_patterns, existing_patterns)
        cache_key = self._generate_cache_key(context)
        if cache_key in self.generation_cache:
            logger.info("Using cached test generation")
            return self.generation_cache[cache_key]
        if self.api_key == '':
            test_code = self._generate_mock_test(request, context)
        else:
            test_code = self._generate_with_api(context, request)
        self.generation_cache[cache_key] = test_code
        return test_code

    def _build_generation_context(self, request: TestGenerationRequest,
                             discovered_patterns: Dict[str, Any],
                             existing_patterns: List[TestPattern]) -> Dict[str, Any]:
        context = {
            'request': {
                'pragma_name': request.pragma_name,
                'test_type': request.test_type,
                'requirements': request.requirements,
                'target_stage': request.target_stage.value,
                'complexity_level': request.complexity_level,
                'include_negative_tests': request.include_negative_tests,
                'include_edge_cases': request.include_edge_cases
            },
            'patterns': {
                'common_flags': discovered_patterns.get('common_flags', {}),
                'directive_insights': discovered_patterns.get('directive_insights', {}),
                'error_categories': discovered_patterns.get('error_categories', {}),
                'recommended_patterns': discovered_patterns.get('recommended_patterns', []),
                'kmpc_patterns': discovered_patterns.get('kmpc_patterns', {})  # New for codegen
            },
            'examples': self._extract_example_snippets(existing_patterns, request),
            'metadata': {
                'total_patterns': len(existing_patterns),
                'avg_complexity': sum(p.complexity_score for p in existing_patterns) / len(existing_patterns)
                                 if existing_patterns else 0
            }
        }
        return context

    def _extract_example_snippets(self, patterns: List[TestPattern],
                            request: TestGenerationRequest) -> Dict[str, List[str]]:
        examples = {
            'pragmas': [],
            'error_checks': [],
            'ast_patterns': [],
            'run_commands': [],
            'codegen_checks': [],  # New for codegen
            'kmpc_calls': []       # New for codegen
        }
        logger.info(f"Extracting examples from {len(patterns)} patterns for {request.pragma_name}")
        relevant_patterns = []
        for p in patterns:
            if p.test_type.value == request.test_type:
                if request.pragma_name.lower() in p.pragma_usage.lower():
                    relevant_patterns.append(p)
                    logger.debug(f"Found relevant pattern: {p.file_path.name}")
        if not relevant_patterns:
            logger.warning(f"No patterns found for {request.pragma_name} and {request.test_type}")
            relevant_patterns = [p for p in patterns if p.test_type.value == request.test_type][:3]
        relevant_patterns = relevant_patterns[:3]
        logger.info(f"Using {len(relevant_patterns)} relevant patterns")
        for i, pattern in enumerate(relevant_patterns):
            logger.debug(f"Processing pattern {i+1}: {pattern.file_path.name}")
            if pattern.pragma_usage:
                pragma_lines = pattern.pragma_usage.split('\n')
                for line in pragma_lines:
                    line = line.strip()
                    if line and line.startswith('#pragma omp'):
                        clean_line = re.sub(r'\s*//.*$', '', line).strip()
                        if clean_line and clean_line not in examples['pragmas']:
                            examples['pragmas'].append(clean_line)
                            logger.debug(f"Added pragma: {clean_line}")
            for error in pattern.error_patterns[:3]:
                clean_error = re.sub(r'\s+', ' ', error.strip())
                if clean_error and len(clean_error) > 10:
                    examples['error_checks'].append(clean_error)
                    logger.debug(f"Added error: {clean_error[:50]}...")
            for ast_section in pattern.ast_dump_sections[:3]:
                ast_section = ast_section.strip()
                if (ast_section and
                    len(ast_section) > 20 and
                    any(keyword in ast_section for keyword in ['OMP', 'Directive', 'Clause', 'Decl', 'Stmt'])):
                    examples['ast_patterns'].append(ast_section)
                    logger.debug(f"Added AST pattern: {ast_section[:50]}...")
            for flag in pattern.compiler_flags:
                if (len(flag) > 1 and
                    not flag in ['-', '--check-prefix'] and
                    not flag.startswith('%') and
                    flag not in examples['run_commands']):
                    examples['run_commands'].append(flag)
                    logger.debug(f"Added flag: {flag}")
            for check in pattern.codegen_checks[:3]:  # New for codegen
                if check and check not in examples['codegen_checks']:
                    examples['codegen_checks'].append(check)
                    logger.debug(f"Added codegen check: {check[:50]}...")
            for kmpc in pattern.kmpc_calls[:3]:  # New for codegen
                if kmpc and kmpc not in examples['kmpc_calls']:
                    examples['kmpc_calls'].append(kmpc)
                    logger.debug(f"Added kmpc call: {kmpc}")
        for key in examples:
            examples[key] = list(dict.fromkeys(examples[key]))
        examples['pragmas'] = examples['pragmas'][:4]
        examples['error_checks'] = examples['error_checks'][:4]
        examples['ast_patterns'] = examples['ast_patterns'][:3]
        examples['run_commands'] = examples['run_commands'][:6]
        examples['codegen_checks'] = examples['codegen_checks'][:3]
        examples['kmpc_calls'] = examples['kmpc_calls'][:3]
        logger.info(f"Final examples summary:")
        logger.info(f"  Pragmas: {len(examples['pragmas'])}")
        logger.info(f"  Error checks: {len(examples['error_checks'])}")
        logger.info(f"  AST patterns: {len(examples['ast_patterns'])}")
        logger.info(f"  Run commands: {len(examples['run_commands'])}")
        logger.info(f"  Codegen checks: {len(examples['codegen_checks'])}")
        logger.info(f"  kmpc calls: {len(examples['kmpc_calls'])}")
        if not examples['pragmas']:
            examples['pragmas'] = [f"#pragma omp {request.pragma_name}"]
            logger.info("Added fallback pragma")
        if not examples['run_commands']:
            examples['run_commands'] = ['-fopenmp', '-verify', '-std=c++11']
            logger.info("Added fallback run commands")
        if request.test_type == 'codegen' and not examples['codegen_checks']:
            examples['codegen_checks'] = ['call void @_kmpc_fork_call']
            logger.info("Added fallback codegen check")
        if request.test_type == 'codegen' and not examples['kmpc_calls']:
            examples['kmpc_calls'] = ['_kmpc_fork_call']
            logger.info("Added fallback kmpc call")
        return examples

    def debug_test_pattern(self, pattern: TestPattern):
        print(f"\n=== DEBUG: {pattern.file_path.name} ===")
        print(f"Test Type: {pattern.test_type}")
        print(f"Pragma Usage:\n{pattern.pragma_usage}")
        print(f"Compiler Flags: {pattern.compiler_flags}")
        print(f"Error Patterns: {pattern.error_patterns[:3]}")
        print(f"AST Dump Sections: {len(pattern.ast_dump_sections)}")
        print(f"Codegen Checks: {pattern.codegen_checks[:3]}")
        print(f"kmpc Calls: {pattern.kmpc_calls}")
        if pattern.ast_dump_sections:
            print(f"First AST section: {pattern.ast_dump_sections[0][:100]}...")
        print("=" * 50)

    def _generate_cache_key(self, context: Dict[str, Any]) -> str:
        context_str = json.dumps(context, sort_keys=True, default=str)
        return hashlib.md5(context_str.encode()).hexdigest()

    def _generate_mock_test(self, request: TestGenerationRequest,
                            context: Dict[str, Any]) -> str:
        pragma_name = request.pragma_name
        test_type = request.test_type
        common_flags = context['patterns'].get('common_flags', {}).get('most_common', [
            '-fopenmp', '-fopenmp-version=50', '-std=c++11', '-verify'
        ])
        if test_type == 'codegen':
            common_flags = ['-fopenmp', '-emit-llvm', '-S', '-std=c++11']
            test_skeleton = f"""// RUN: %clang_cc1 {' '.join(common_flags)} %s

// CHECK-CODEGEN: define {{.*}}void @{{.*}}test_basic
// CHECK-CODEGEN: call void @_kmpc_fork_call

#include <omp.h>

void test_basic() {{
  #pragma omp {pragma_name}
  {{
    int x = omp_get_thread_num();
  }}
}}

void test_with_private() {{
  int i;
  #pragma omp {pragma_name} private(i)
  for (i = 0; i < 100; i++) {{
    // CHECK-CODEGEN: call void @_kmpc_for_static_init
    int y = i;
  }}
}}

void test_invalid_usage() {{
  // expected-error@+1 {{'{pragma_name}' directive must be followed by a parallelizable construct}}
  #pragma omp {pragma_name}
  int x = 0;
}}

void test_nested() {{
  #pragma omp {pragma_name}
  {{
    // CHECK-CODEGEN: call void @_kmpc_fork_call
    #pragma omp {pragma_name}
    {{
      int z = omp_get_thread_num();
    }}
  }}
}}

void test_with_reduction() {{
  int sum = 0;
  #pragma omp {pragma_name} reduction(+:sum)
  for (int i = 0; i < 100; i++) {{
    // CHECK-CODEGEN: call void @_kmpc_reduce
    sum += i;
  }}
}}
"""
        else:
            run_flags = ' '.join([f for f in common_flags if not f.startswith('-o') and f != '-'])
            test_skeleton = f"""// RUN: %clang_cc1 {run_flags} %s

void test_basic() {{
  // expected-error@+1 {{'{pragma_name}' directive must be followed by a parallelizable construct}}
  #pragma omp {pragma_name}
  int x = 0;
}}

void test_with_private_clause() {{
  int i;
  #pragma omp {pragma_name} private(i)
  for (i = 0; i < 10; i++) {{
    int y = i;
  }}
}}

void test_invalid_clause() {{
  // expected-error@+1 {{unexpected OpenMP clause 'order' in directive '#pragma omp {pragma_name}'}}
  #pragma omp {pragma_name} order(concurrent)
  for (int j = 0; j < 5; j++) {{
  }}
}}

void test_duplicate_clause() {{
  int k;
  // expected-error@+1 {{duplicate 'private' clause on directive '#pragma omp {pragma_name}'}}
  #pragma omp {pragma_name} private(k) private(k)
  for (k = 0; k < 5; k++) {{
  }}
}}

void test_default_none() {{
  int m;
  // expected-error@+2 {{variable 'm' must have an explicit data-sharing attribute when 'default(none)' is specified}}
  #pragma omp {pragma_name} default(none)
  for (m = 0; m < 5; m++) {{
    m += 1;
  }}
}}
"""
        return test_skeleton

    def _generate_with_api(self, context: Dict[str, Any],
                          request: TestGenerationRequest) -> str:
        prompt = self._create_api_prompt(context, request)
        headers = {"Content-Type": "application/json"}
        data = {
            "contents": [{"parts": [{"text": prompt}]}],
            "generationConfig": {"temperature": 0.1, "maxOutputTokens": 2048}
        }
        try:
            response = requests.post(self.api_url, headers=headers, json=data, timeout=30)
            response.raise_for_status()
            result = response.json()
            with open('api_response.json', 'w', encoding='utf-8') as f:
                json.dump(result, f, indent=2)
            logger.debug("Dumped API response to api_response.json")
            if 'candidates' in result and len(result['candidates']) > 0:
                generated_text = result['candidates'][0]['content']['parts'][0]['text']
                if generated_text.strip().startswith("// RUN:"):
                    return generated_text
                else:
                    logger.warning(f"API response is not valid test code: {generated_text[:100]}... Falling back to mock.")
                    return self._generate_mock_test(request, context)
            else:
                logger.warning("No candidates in API response, falling back to mock")
                return self._generate_mock_test(request, context)
        except Exception as e:
            logger.error(f"API Error: {str(e)}")
            return self._generate_mock_test(request, context)

    def _create_api_prompt(self, context: Dict[str, Any],
                          request: TestGenerationRequest) -> str:
        examples = context['examples']
        patterns = context['patterns']
        if request.test_type == 'codegen':
            prompt = f"""
Based on analysis of OpenMP tests for "{request.pragma_name}":
**Test Type:** {request.test_type}
**Requirements:** {', '.join(request.requirements)}
**Common Flags:** {', '.join(patterns.get('common_flags', {}).get('most_common', []))}
**Example Pragmas:** {examples['pragmas'][:2]}
**Example Codegen Checks:** {examples['codegen_checks'][:2]}
**Example kmpc Calls:** {examples['kmpc_calls'][:2]}
**Complexity Level:** {request.complexity_level}

Generate an LLVM lit test that:
1. Uses RUN: lines with -fopenmp -emit-llvm -S
2. Includes CHECK-CODEGEN lines for _kmpc runtime calls
3. Contains at least five test cases including valid/invalid/edge cases
4. Verifies codegen output with appropriate _kmpc calls
5. Follows LLVM lit test conventions
6. Tests core functionality of {request.pragma_name}

Output only the C++ test code with comments, no additional text.
"""
        else:
            prompt = f"""
Based on analysis of OpenMP tests for "{request.pragma_name}":
**Test Type:** {request.test_type}
**Requirements:** {', '.join(request.requirements)}
**Common Flags:** {', '.join(patterns.get('common_flags', {}).get('most_common', []))}
**Example Pragmas:** {examples['pragmas'][:2]}
**Example Errors:** {examples['error_checks'][:2]}
**Complexity Level:** {request.complexity_level}

Generate a minimal LLVM lit test that:
1. Uses appropriate RUN: lines with common flags
2. Includes diagnostic checks for messages tests
3. Contains at least five test cases (valid, invalid, edge cases)
4. Follows LLVM lit test conventions
5. Tests core functionality of {request.pragma_name}

Output only the C++ test code with comments, no additional text.
"""
        return prompt

def generate_openmp_test_skeleton(pragma_name: str, api_key: str, user_requirements: str, base_directory: str = "~/llvm-project/clang/test/OpenMP"):
    """Generate an OpenMP test skeleton based on the given pragma name, API key, and requirements."""
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    logger = logging.getLogger(__name__)

   # base_directory = r"C:\Users\Eshaan Mathur\Downloads\llvm llvm-project main clang-test_OpenMP"
#    clang_path = r"C:\Program Files\LLVM\bin\clang.exe"

    # Parse requirements as a list
    requirements_list = [req.strip() for req in user_requirements.split(',') if req.strip()]

    # Determine test type from requirements
    test_type = 'messages'  # Default
    for req in requirements_list:
        req_lower = req.lower()
        if 'codegen' in req_lower or 'kmpc' in req_lower:
            test_type = 'codegen'
            break
        elif 'semantics' in req_lower or 'ast' in req_lower:
            test_type = 'ast-print'
        elif 'runtime' in req_lower:
            test_type = 'runtime'

    

    logger.info(f"Initializing with directory: {base_directory}, pragma: {pragma_name}, test type: {test_type}")
    analyzer = AdvancedASTAnalyzer(clang_path=clang_path)
    discovery_engine = PatternDiscoveryEngine()
    generator = AdvancedTestGenerator(api_key=api_key)

    logger.info(f"Finding test files for pragma '{pragma_name}' and test type '{test_type}'...")
    test_files = []
    file_pattern = pragma_name.replace(" ", "_")
    patterns = [
        f"**/{file_pattern}_{test_type}.cpp",
        f"**/{file_pattern}_*.cpp"
    ]
    for pattern in patterns:
        matches = list(Path(base_directory).glob(pattern))
        test_files.extend(matches)
        if matches:
            logger.info(f"Found {len(matches)} files for pattern '{pattern}': {[f.name for f in matches]}")
    test_files = list(dict.fromkeys(test_files))
    logger.info(f"Total unique test files found: {len(test_files)} - {[f.name for f in test_files]}")

    logger.info(f"Analyzing up to 3 test files...")
    test_patterns = []
    for file_path in test_files[:3]:
        try:
            pattern = analyzer.analyze_test_file(file_path)
            if not isinstance(pattern, TestPattern):
                logger.warning(f"Invalid pattern type for {file_path.name}: {type(pattern)}")
                continue
            if not pattern.compiler_flags or not pattern.test_type:
                logger.warning(f"Incomplete pattern for {file_path.name}: {pattern}")
                continue
            test_patterns.append(pattern)
            logger.info(f"Analyzed {file_path.name}")
            if test_type == 'codegen':
                generator.debug_test_pattern(pattern)
        except Exception as e:
            logger.error(f"Failed to analyze {file_path.name}: {e}")
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read(500)
                logger.debug(f"Snippet of {file_path.name}: {content}")
            except Exception as ex:
                logger.debug(f"Could not read {file_path.name}: {ex}")
            continue

    if not test_patterns:
        logger.error("No valid test patterns found to analyze")
        return None

    logger.info(f"Successfully analyzed {len(test_patterns)} test patterns")

    logger.info("Discovering patterns across test files...")
    discovered_patterns = discovery_engine.discover_patterns(test_patterns)
    logger.info(f"Discovered patterns: {discovered_patterns.get('common_flags', {}).get('most_common', [])}")
    if test_type == 'codegen':
        logger.info(f"kmpc patterns: {discovered_patterns.get('kmpc_patterns', {})}")

    logger.info("Generating test skeleton...")
    request = TestGenerationRequest(
        pragma_name=pragma_name,
        test_type=test_type,
        requirements=requirements_list,
        target_stage=CompilerStage.BACKEND if test_type == 'codegen' else CompilerStage.FRONTEND,
        include_negative_tests=True,
        include_edge_cases=True,
        complexity_level="medium"
    )
    skeleton = generator.generate_test(request, discovered_patterns, test_patterns)

    return skeleton
