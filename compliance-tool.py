#!/usr/bin/env python3
"""
Enhanced LLVM PR Compliance Tool - Ensures pull requests comply with LLVM coding standards
Integrates clang-format, clang-tidy, custom checks, and CodeBERT analysis
Focuses only on modified code sections for efficient analysis
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple
import requests
import torch
from transformers import RobertaTokenizer, RobertaForSequenceClassification, RobertaConfig
from github import Github
import difflib
from datetime import datetime

class IssueType(Enum):
    TIDY = "clang-tidy"
    FORMATTING = "formatting"
    STYLE = "style"
    NAMING = "naming"
    COMMENT = "comment"
    HEADER = "header"
    CODEBERT = "codebert"
    PERFORMANCE = "performance"
    MAINTAINABILITY = "maintainability"

class Severity(Enum):
    ERROR = "error"
    WARNING = "warning"
    INFO = "info"

@dataclass
class Issue:
    type: IssueType
    severity: Severity
    file: str
    line: int
    column: int
    message: str
    suggestion: Optional[str] = None
    original_code: Optional[str] = None
    suggested_code: Optional[str] = None
    rule: Optional[str] = None

class LLVMCodingStandards:
    """Enhanced LLVM coding standards rules and patterns based on official documentation"""
    
    # Naming conventions (updated to match LLVM standards exactly)
    FUNCTION_NAMING = r'^[a-z][a-zA-Z0-9]*$'  # lowerCamelCase
    CLASS_NAMING = r'^[A-Z][a-zA-Z0-9]*$'     # UpperCamelCase
    VARIABLE_NAMING = r'^[A-Z][a-zA-Z0-9]*$'  # UpperCamelCase for member variables
    LOCAL_VARIABLE_NAMING = r'^[A-Z][a-zA-Z0-9]*$'  # UpperCamelCase for locals
    CONSTANT_NAMING = r'^[A-Z][A-Z0-9_]*$'    # ALL_CAPS
    MACRO_NAMING = r'^[A-Z][A-Z0-9_]*$'       # ALL_CAPS
    NAMESPACE_NAMING = r'^[a-z][a-z0-9_]*$'   # lowercase
    
    # File patterns
    HEADER_EXTENSIONS = {'.h', '.hpp', '.hxx', '.def'}
    SOURCE_EXTENSIONS = {'.cpp', '.cc', '.c', '.cxx'}
    
    # Header guard patterns
    HEADER_GUARD_PATTERN = r'^#ifndef\s+LLVM_.*_H\s*$'
    
    # Include patterns
    SYSTEM_INCLUDES = r'^#include\s*<[^>]+>$'
    LOCAL_INCLUDES = r'^#include\s*"[^"]+"$'
    
    # LLVM-specific patterns
    LLVM_UNREACHABLE = r'llvm_unreachable'
    LLVM_DEBUG = r'LLVM_DEBUG'
    
    # License header pattern
    LICENSE_HEADER = [
        r'//===----------------------------------------------------------------------===//\s*',
        r'//\s+.*',
        r'//\s*',
        r'// Part of the LLVM Project, under the Apache License v2\.0 with LLVM Exceptions\.',
        r'// See https://llvm\.org/LICENSE\.txt for license information\.',
        r'// SPDX-License-Identifier: Apache-2\.0 WITH LLVM-exception\s*',
        r'//\s*',
        r'//===----------------------------------------------------------------------===//\s*'
    ]
    
    # Code style patterns
    EARLY_RETURN_PATTERN = r'if\s*\([^)]+\)\s*{\s*return\s*[^;]*;\s*}'
    NESTED_IF_PATTERN = r'if\s*\([^)]+\)\s*{\s*if\s*\([^)]+\)'
    
    @staticmethod
    def get_codebert_analysis_prompts():
        """Get prompts for CodeBERT analysis"""
        return {
            'style': """
Analyze this C++ code for LLVM style violations:
1. Early returns vs nested conditions
2. const correctness
3. LLVM data structure usage (StringRef, SmallVector, etc.)
4. auto usage appropriateness
5. Range-based for loop opportunities
""",
            'naming': """
Check LLVM naming conventions:
1. Functions: lowerCamelCase
2. Classes/Types: UpperCamelCase  
3. Variables: UpperCamelCase
4. Constants: ALL_CAPS
5. Namespaces: lowercase
""",
            'comments': """
Evaluate comment quality for LLVM standards:
1. Doxygen-style function documentation
2. Complex algorithm explanations
3. "Why" vs "what" comments
4. Avoiding obvious comments
""",
            'maintainability': """
Assess code maintainability:
1. Function complexity and length
2. Code duplication
3. Clear variable names
4. Logical structure
"""
        }

class GitHubPRAnalyzer:
    """Enhanced GitHub PR analyzer with better diff parsing"""
    
    def __init__(self, token: str):
        self.github = Github(token)
    
    def get_pr_changes(self, repo_name: str, pr_number: int) -> Dict[str, Dict]:
        """Get detailed modified lines and context from a PR"""
        repo = self.github.get_repo(repo_name)
        pr = repo.get_pull(pr_number)
        changes = {}
        
        for file in pr.get_files():
            if not self._is_relevant_file(file.filename):
                continue
            
            file_info = {
                'modified_lines': [],
                'added_lines': [],
                'deleted_lines': [],
                'context_lines': [],
                'file_content': None,
                'patch': file.patch
            }
            
            if file.patch:
                # Parse the patch more accurately
                parsed_diff = self._parse_unified_diff(file.patch)
                file_info.update(parsed_diff)
                
                # Try to get the current file content
                try:
                    if file.status != 'removed':
                        content = repo.get_contents(file.filename, ref=pr.head.sha)
                        file_info['file_content'] = content.decoded_content.decode('utf-8')
                except:
                    print(f"Warning: Could not fetch content for {file.filename}")
            
            changes[file.filename] = file_info
        
        return changes
    
    def _is_relevant_file(self, filename: str) -> bool:
        """Check if file is relevant for LLVM analysis"""
        ext = Path(filename).suffix.lower()
        return ext in {'.cpp', '.cc', '.c', '.cxx', '.h', '.hpp', '.hxx', '.def'}
    
    def _parse_unified_diff(self, patch: str) -> Dict:
        """Parse unified diff format more accurately"""
        lines = patch.split('\n')
        result = {
            'modified_lines': [],
            'added_lines': [],
            'deleted_lines': [],
            'context_lines': []
        }
        
        current_new_line = 0
        current_old_line = 0
        
        for line in lines:
            if line.startswith('@@'):
                # Parse hunk header: @@ -old_start,old_count +new_start,new_count @@
                match = re.search(r'@@\s*-(\d+)(?:,(\d+))?\s*\+(\d+)(?:,(\d+))?\s*@@', line)
                if match:
                    old_start = int(match.group(1))
                    new_start = int(match.group(3))
                    current_old_line = old_start
                    current_new_line = new_start
            elif line.startswith('+') and not line.startswith('+++'):
                # Added line
                result['added_lines'].append((current_new_line, line[1:]))
                result['modified_lines'].append((current_new_line, line[1:], 'added'))
                current_new_line += 1
            elif line.startswith('-') and not line.startswith('---'):
                # Deleted line
                result['deleted_lines'].append((current_old_line, line[1:]))
                result['modified_lines'].append((current_old_line, line[1:], 'deleted'))
                current_old_line += 1
            elif line.startswith(' '):
                # Context line
                result['context_lines'].append((current_new_line, line[1:]))
                current_old_line += 1
                current_new_line += 1
        
        return result

class EnhancedClangFormatChecker:
    """Enhanced clang-format checker with LLVM-specific configuration"""
    
    def __init__(self, style_file: Optional[str] = None):
        self.style_file = style_file or self._create_llvm_style()
    
    def _create_llvm_style(self) -> str:
        """Create comprehensive LLVM style configuration"""
        style_config = """
BasedOnStyle: LLVM
Language: Cpp
IndentWidth: 2
UseTab: Never
ColumnLimit: 80
BreakBeforeBraces: Attach
SpacesBeforeTrailingComments: 1
AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false
AlignOperands: true
AlignTrailingComments: true
AllowShortBlocksOnASingleLine: false
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
BinPackArguments: true
BinPackParameters: true
BreakBeforeBinaryOperators: None
BreakBeforeTernaryOperators: true
BreakConstructorInitializersBeforeComma: false
BreakAfterJavaFieldAnnotations: false
BreakStringLiterals: true
ConstructorInitializerAllOnOneLineOrOnePerLine: false
ConstructorInitializerIndentWidth: 4
ContinuationIndentWidth: 4
Cpp11BracedListStyle: true
DerivePointerAlignment: false
DisableFormat: false
ExperimentalAutoDetectBinPacking: false
ForEachMacros: [ foreach, Q_FOREACH, BOOST_FOREACH ]
IncludeCategories:
  - Regex: '^"(llvm|clang|lldb|lld)/'
    Priority: 1
  - Regex: '^(<|"(gtest|gmock|isl|json)/)'
    Priority: 2
  - Regex: '.*'
    Priority: 3
IndentCaseLabels: false
IndentWrappedFunctionNames: false
KeepEmptyLinesAtTheStartOfBlocks: true
MacroBlockBegin: ''
MacroBlockEnd: ''
MaxEmptyLinesToKeep: 1
NamespaceIndentation: None
PenaltyBreakBeforeFirstCallParameter: 19
PenaltyBreakComment: 300
PenaltyBreakFirstLessLess: 120
PenaltyBreakString: 1000
PenaltyExcessCharacter: 1000000
PenaltyReturnTypeOnItsOwnLine: 60
PointerAlignment: Right
ReflowComments: true
SortIncludes: true
SpaceAfterCStyleCast: false
SpaceBeforeAssignmentOperators: true
SpaceBeforeParens: ControlStatements
SpaceInEmptyParentheses: false
SpacesInAngles: false
SpacesInContainerLiterals: true
SpacesInCStyleCastParentheses: false
SpacesInParentheses: false
SpacesInSquareBrackets: false
Standard: Cpp11
TabWidth: 8
"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.clang-format', delete=False) as f:
            f.write(style_config)
            return f.name
    
    def check_modified_lines(self, file_path: str, modified_lines: List[Tuple]) -> List[Issue]:
        """Check formatting for specific modified lines only"""
        issues = []
        
        if not os.path.exists(file_path):
            return issues
        
        try:
            # Run clang-format on the entire file
            result = subprocess.run([
                'clang-format', 
                f'-style=file:{self.style_file}',
                file_path
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode != 0:
                return issues
            
            # Compare original and formatted content
            with open(file_path, 'r') as f:
                original_lines = f.readlines()
            
            formatted_lines = result.stdout.splitlines(keepends=True)
            
            # Check only modified lines
            for line_info in modified_lines:
                if len(line_info) >= 2:
                    line_num = line_info[0]
                    if (line_num > 0 and line_num <= len(original_lines) and 
                        line_num <= len(formatted_lines)):
                        
                        orig = original_lines[line_num - 1].rstrip()
                        formatted = formatted_lines[line_num - 1].rstrip()
                        
                        if orig != formatted:
                            issues.append(Issue(
                                type=IssueType.FORMATTING,
                                severity=Severity.ERROR,
                                file=file_path,
                                line=line_num,
                                column=1,
                                message="Code formatting does not match LLVM style",
                                original_code=orig,
                                suggested_code=formatted,
                                rule="clang-format"
                            ))
        
        except (subprocess.CalledProcessError, subprocess.TimeoutExpired) as e:
            print(f"clang-format error for {file_path}: {e}")
        
        return issues

class FilteredClangTidyChecker:
    """Enhanced clang-tidy checker that filters results to modified lines"""

    def __init__(self):
        self.checks = [
            'readability-*',
            'modernize-*',
            'performance-*',
            'bugprone-*',
            'clang-analyzer-*',
            'llvm-*',
            'google-readability-*',
            'cppcoreguidelines-*'
        ]

        self.disabled_checks = [
            '-readability-magic-numbers',
            '-readability-convert-member-functions-to-static',
            '-modernize-use-trailing-return-type'
        ]

    def check_modified_lines(self, file_path: str, modified_lines: Set[int]) -> List[Issue]:
        """Run clang-tidy and filter results to modified lines only"""
        issues = []

        if not os.path.exists(file_path) or not modified_lines:
            return issues

        try:
            all_checks = ','.join(self.checks + self.disabled_checks)

            compile_commands = self._create_compile_commands(file_path)

            cmd = [
                'clang-tidy',
                f'-checks={all_checks}',
                '-extra-arg=-fno-color-diagnostics',
                '-extra-arg=-Wno-unknown-warning-option',
                '-extra-arg=-std=c++17',
                '-extra-arg=-I/home/ganeshnaik/Desktop/cd_proj/llvm-project/llvm/include',
                '-extra-arg=-I/home/ganeshnaik/Desktop/cd_proj/llvm-project/build/include',
                '-extra-arg=-I/home/ganeshnaik/Desktop/cd_proj/llvm-project/clang/include',
                '-extra-arg=-I/home/ganeshnaik/Desktop/cd_proj/llvm-project/build/tools/clang/include',
                '--quiet',
                '--format-style=file',
                file_path
            ]

            if compile_commands:
                cmd.append(f'-p={os.path.dirname(compile_commands)}')
            else:
                cmd.append('--')

            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)

            for line in result.stdout.split('\n'):
                if ':' in line and ('warning:' in line or 'error:' in line):
                    issue = self._parse_clang_tidy_line(line, file_path, modified_lines)
                    if issue:
                        issues.append(issue)

        except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
            pass  # silently fail to avoid flooding console

        return issues

    def _create_compile_commands(self, file_path: str) -> Optional[str]:
        """
        Return path to real compile_commands.json from CMake build dir,
        assuming it's at llvm-project/build/compile_commands.json
        """
        try:
            curr = os.path.abspath(file_path)
            while curr != os.path.dirname(curr):
                candidate = os.path.join(curr, 'build', 'compile_commands.json')
                if os.path.exists(candidate):
                    return candidate
                curr = os.path.dirname(curr)
            return None
        except:
            return None

    def _parse_clang_tidy_line(self, line: str, file_path: str, modified_lines: Set[int]) -> Optional[Issue]:
        try:
            # Example: path/to/file.cpp:123:45: warning: message [check-name]
            match = re.match(r"^(.*?):(\d+):(\d+): (\w+): (.*?)(?: \[(.*?)\])?$", line)
            if not match:
                return None

            parsed_file, line_str, col_str, severity_str, message, rule = match.groups()
            line_num = int(line_str)
            column = int(col_str) if col_str else 1

            if line_num not in modified_lines:
                return None

            severity = {
                "error": Severity.ERROR,
                "warning": Severity.WARNING,
                "info": Severity.INFO
            }.get(severity_str.lower(), Severity.INFO)

            return Issue(
                type=IssueType.TIDY,
                severity=severity,
                file=file_path,
                line=line_num,
                column=column,
                message=message.strip(),
                rule=rule or "clang-tidy"
            )

        except:
            return None



class CustomLLVMChecker:
    """Enhanced custom checks for LLVM-specific patterns"""
    
    def __init__(self):
        self.standards = LLVMCodingStandards()
    
    def check_modified_lines(self, file_path: str, file_info: Dict) -> List[Issue]:
        """Run custom LLVM checks on modified lines"""
        issues = []
        
        modified_lines = file_info.get('modified_lines', [])
        file_content = file_info.get('file_content')
        
        if not modified_lines:
            return issues
        
        # Get all file lines for context
        if file_content:
            all_lines = file_content.splitlines()
        elif os.path.exists(file_path):
            with open(file_path, 'r') as f:
                all_lines = f.readlines()
        else:
            return issues
        
        for line_info in modified_lines:
            if len(line_info) >= 2:
                line_num = line_info[0]
                line_content = line_info[1]
                
                # Skip if line was deleted
                if len(line_info) > 2 and line_info[2] == 'deleted':
                    continue
                
                # Run various checks
                issues.extend(self._check_naming_conventions(file_path, line_num, line_content))
                issues.extend(self._check_header_compliance(file_path, line_num, line_content, all_lines))
                issues.extend(self._check_comment_quality(file_path, line_num, line_content, all_lines))
                issues.extend(self._check_llvm_patterns(file_path, line_num, line_content))
                issues.extend(self._check_include_order(file_path, line_num, line_content, all_lines))
        
        return issues
    
    def _check_naming_conventions(self, file_path: str, line_num: int, line: str) -> List[Issue]:
        """Check LLVM naming conventions"""
        issues = []
        
        # Function definitions
        func_match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*[{;]', line)
        if func_match and not line.strip().startswith('//'):
            func_name = func_match.group(1)
            # Skip constructors, destructors, operators
            if not (func_name[0].isupper() and func_name == func_name or 
                   func_name.startswith('operator') or func_name.startswith('~')):
                if not re.match(self.standards.FUNCTION_NAMING, func_name):
                    issues.append(Issue(
                        type=IssueType.NAMING,
                        severity=Severity.WARNING,
                        file=file_path,
                        line=line_num,
                        column=func_match.start(1) + 1,
                        message=f"Function '{func_name}' should use lowerCamelCase naming",
                        suggestion=f"Rename to follow lowerCamelCase (e.g., {self._to_camel_case(func_name)})",
                        rule="llvm-naming-function"
                    ))
        
        # Class/struct definitions
        class_match = re.search(r'\b(?:class|struct)\s+([a-zA-Z_][a-zA-Z0-9_]*)', line)
        if class_match:
            class_name = class_match.group(1)
            if not re.match(self.standards.CLASS_NAMING, class_name):
                issues.append(Issue(
                    type=IssueType.NAMING,
                    severity=Severity.WARNING,
                    file=file_path,
                    line=line_num,
                    column=class_match.start(1) + 1,
                    message=f"Class '{class_name}' should use UpperCamelCase naming",
                    suggestion=f"Rename to follow UpperCamelCase convention",
                    rule="llvm-naming-class"
                ))
        
        # Variable declarations
        var_match = re.search(r'\b(?:const\s+)?(?:auto|int|bool|char|float|double|size_t|[A-Z][a-zA-Z0-9]*)\s+([a-z_][a-zA-Z0-9_]*)\s*[=;,)]', line)
        if var_match:
            var_name = var_match.group(1)
            if not re.match(self.standards.VARIABLE_NAMING, var_name):
                issues.append(Issue(
                    type=IssueType.NAMING,
                    severity=Severity.INFO,
                    file=file_path,
                    line=line_num,
                    column=var_match.start(1) + 1,
                    message=f"Variable '{var_name}' should use UpperCamelCase naming",
                    suggestion=f"Consider renaming to UpperCamelCase",
                    rule="llvm-naming-variable"
                ))
        
        return issues
    
    def _check_header_compliance(self, file_path: str, line_num: int, line: str, all_lines: List[str]) -> List[Issue]:
        """Check header file compliance"""
        issues = []
        
        if file_path.endswith(('.h', '.hpp')):
            # Check for header guards
            if line_num <= 3 and line.strip().startswith('#ifndef'):
                if not re.match(self.standards.HEADER_GUARD_PATTERN, line.strip()):
                    issues.append(Issue(
                        type=IssueType.HEADER,
                        severity=Severity.WARNING,
                        file=file_path,
                        line=line_num,
                        column=1,
                        message="Header guard should follow LLVM_*_H pattern",
                        rule="llvm-header-guard"
                    ))
            
            # Check for license header in first 10 lines
            if line_num <= 10:
                header_text = '\n'.join(all_lines[:10])
                if 'LLVM Project' not in header_text and 'Apache License' not in header_text:
                    if line_num == 1:
                        issues.append(Issue(
                            type=IssueType.HEADER,
                            severity=Severity.INFO,
                            file=file_path,
                            line=1,
                            column=1,
                            message="Consider adding LLVM license header",
                            rule="llvm-license-header"
                        ))
        
        return issues
    
    def _check_comment_quality(self, file_path: str, line_num: int, line: str, all_lines: List[str]) -> List[Issue]:
        """Check comment quality and documentation"""
        issues = []
        
        # Check for function documentation
        if re.search(r'\b[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*{', line):
            # Look for preceding Doxygen comment
            comment_found = False
            for i in range(max(0, line_num - 5), line_num):
                if i < len(all_lines):
                    prev_line = all_lines[i].strip()
                    if prev_line.startswith('///') or prev_line.startswith('/**'):
                        comment_found = True
                        break
            
            if not comment_found:
                issues.append(Issue(
                    type=IssueType.COMMENT,
                    severity=Severity.INFO,
                    file=file_path,
                    line=line_num,
                    column=1,
                    message="Public function should have Doxygen documentation",
                    suggestion="Add /// or /** */ comment describing the function",
                    rule="llvm-function-docs"
                ))
        
        # Check for obvious comments
        comment_match = re.search(r'//\s*(.+)', line)
        if comment_match:
            comment_text = comment_match.group(1).strip().lower()
            code_part = line[:comment_match.start()].strip()
            
            # Simple heuristic for obvious comments
            if code_part and any(word in comment_text for word in ['increment', 'decrement', 'return', 'set', 'get']):
                if any(word in code_part.lower() for word in comment_text.split()):
                    issues.append(Issue(
                        type=IssueType.COMMENT,
                        severity=Severity.INFO,
                        file=file_path,
                        line=line_num,
                        column=comment_match.start() + 1,
                        message="Comment may be stating the obvious",
                        suggestion="Consider explaining 'why' instead of 'what'",
                        rule="llvm-obvious-comment"
                    ))
        
        return issues
    
    def _check_llvm_patterns(self, file_path: str, line_num: int, line: str) -> List[Issue]:
        """Check for LLVM-specific patterns and best practices"""
        issues = []
        
        # Check for proper use of LLVM_UNREACHABLE
        if 'assert(false' in line or 'assert(0' in line:
            issues.append(Issue(
                type=IssueType.STYLE,
                severity=Severity.INFO,
                file=file_path,
                line=line_num,
                column=1,
                message="Consider using llvm_unreachable() instead of assert(false)",
                suggestion="Replace with llvm_unreachable(\"message\")",
                rule="llvm-unreachable"
            ))
        
        # Check for range-based for loop opportunities
        if re.search(r'for\s*\(\s*\w+\s+\w+\s*=.*\.begin\(\)', line):
            issues.append(Issue(
                type=IssueType.STYLE,
                severity=Severity.INFO,
                file=file_path,
                line=line_num,
                column=1,
                message="Consider using range-based for loop",
                suggestion="Use 'for (auto &Item : Container)' style",
                rule="llvm-range-for"
            ))
        
        # Check for auto usage
        if re.search(r'\b(?:std::)?(?:vector|map|set|unordered_map)<.*>\s+\w+', line):
            issues.append(Issue(
                type=IssueType.STYLE,
                severity=Severity.INFO,
                file=file_path,
                line=line_num,
                column=1,
                message="Consider using 'auto' for complex type names",
                rule="llvm-auto-type"
            ))
        
        return issues
    
    def _check_include_order(self, file_path: str, line_num: int, line: str, all_lines: List[str]) -> List[Issue]:
        """Check include ordering according to LLVM standards"""
        issues = []
        
        if line.strip().startswith('#include'):
            # This is a simplified check - full implementation would analyze all includes
            if '"llvm/' in line and line_num > 1:
                # Check if previous line was a system include
                prev_line = all_lines[line_num - 2].strip() if line_num - 2 < len(all_lines) else ""
                if prev_line.startswith('#include <'):
                                            issues.append(Issue(
                        type=IssueType.STYLE,
                        severity=Severity.INFO,
                        file=file_path,
                        line=line_num,
                        column=1,
                        message="LLVM includes should be grouped after system includes",
                        suggestion="Group includes: system, LLVM, local",
                        rule="llvm-include-order"
                    ))
        
        return issues
    
    def _to_camel_case(self, snake_str: str) -> str:
        """Convert snake_case to camelCase"""
        components = snake_str.split('_')
        return components[0].lower() + ''.join(x.capitalize() for x in components[1:])

class CodeBERTAnalyzer:
    """CodeBERT-based advanced code analysis for LLVM compliance"""
    
    def __init__(self, model_name: str = "microsoft/codebert-base"):
        """Initialize CodeBERT analyzer"""
        try:
            self.device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
            self.tokenizer = RobertaTokenizer.from_pretrained(model_name)
            self.model = RobertaForSequenceClassification.from_pretrained(
                model_name,
                num_labels=4  # style, naming, comments, maintainability
            )
            self.model.to(self.device)
            self.model.eval()
            self.standards = LLVMCodingStandards()
        except Exception as e:
            print(f"Warning: Could not initialize CodeBERT model: {e}")
            self.model = None
            self.tokenizer = None
    
    def analyze_code_segment(self, file_path: str, file_info: Dict) -> List[Issue]:
        """Analyze code using CodeBERT for advanced pattern detection"""
        if not self.model or not self.tokenizer:
            return []
        
        issues = []
        modified_lines = file_info.get('modified_lines', [])
        file_content = file_info.get('file_content')
        
        if not modified_lines or not file_content:
            return issues
        
        all_lines = file_content.splitlines()
        
        # Group consecutive modified lines into segments for analysis
        segments = self._group_modified_lines(modified_lines, all_lines)
        
        for segment in segments:
            segment_issues = self._analyze_segment(file_path, segment, all_lines)
            issues.extend(segment_issues)
        
        return issues
    
    def _group_modified_lines(self, modified_lines: List[Tuple], all_lines: List[str]) -> List[Dict]:
        """Group consecutive modified lines into analysis segments"""
        segments = []
        current_segment = None
        
        for line_info in modified_lines:
            if len(line_info) < 2:
                continue
                
            line_num = line_info[0]
            
            # Skip deleted lines
            if len(line_info) > 2 and line_info[2] == 'deleted':
                continue
            
            if current_segment is None:
                current_segment = {
                    'start_line': line_num,
                    'end_line': line_num,
                    'lines': [line_num]
                }
            elif line_num == current_segment['end_line'] + 1:
                # Consecutive line
                current_segment['end_line'] = line_num
                current_segment['lines'].append(line_num)
            else:
                # Gap found, finalize current segment
                segments.append(current_segment)
                current_segment = {
                    'start_line': line_num,
                    'end_line': line_num,
                    'lines': [line_num]
                }
        
        if current_segment:
            segments.append(current_segment)
        
        return segments
    
    def _analyze_segment(self, file_path: str, segment: Dict, all_lines: List[str]) -> List[Issue]:
        """Analyze a code segment using CodeBERT"""
        issues = []
        
        # Extract code segment with context
        start_line = max(0, segment['start_line'] - 5)
        end_line = min(len(all_lines), segment['end_line'] + 5)
        
        code_context = '\n'.join(all_lines[start_line:end_line])
        
        # Analyze different aspects
        analyses = [
            ('style', self._analyze_style_issues),
            ('naming', self._analyze_naming_issues),
            ('comments', self._analyze_comment_issues),
            ('maintainability', self._analyze_maintainability_issues)
        ]
        
        for analysis_type, analysis_func in analyses:
            try:
                segment_issues = analysis_func(file_path, segment, code_context, all_lines)
                issues.extend(segment_issues)
            except Exception as e:
                print(f"CodeBERT analysis error ({analysis_type}): {e}")
        
        return issues
    
    def _analyze_style_issues(self, file_path: str, segment: Dict, code_context: str, all_lines: List[str]) -> List[Issue]:
        """Analyze style issues using CodeBERT"""
        issues = []
        
        # Prepare input for CodeBERT
        prompt = self.standards.get_codebert_analysis_prompts()['style']
        input_text = f"{prompt}\n\nCode:\n{code_context}"
        
        # Tokenize and predict
        try:
            inputs = self.tokenizer(
                input_text,
                return_tensors="pt",
                truncation=True,
                max_length=512,
                padding=True
            ).to(self.device)
            
            with torch.no_grad():
                outputs = self.model(**inputs)
                predictions = torch.nn.functional.softmax(outputs.logits, dim=-1)
            
            # Interpret predictions
            confidence_threshold = 0.6
            if predictions[0][0] > confidence_threshold:  # Style issues detected
                # Rule-based detection for specific issues
                issues.extend(self._detect_specific_style_issues(file_path, segment, code_context, all_lines))
        
        except Exception as e:
            print(f"CodeBERT style analysis error: {e}")
        
        return issues
    
    def _detect_specific_style_issues(self, file_path: str, segment: Dict, code_context: str, all_lines: List[str]) -> List[Issue]:
        """Detect specific style issues in the code segment"""
        issues = []
        
        lines = code_context.split('\n')
        
        for i, line in enumerate(lines):
            actual_line_num = segment['start_line'] - 5 + i + 1
            
            if actual_line_num not in segment['lines']:
                continue
            
            # Check for nested if statements instead of early returns
            if 'if' in line and '{' in line:
                indent_level = len(line) - len(line.lstrip())
                # Look for nested conditions
                for j in range(i + 1, min(i + 10, len(lines))):
                    next_line = lines[j]
                    next_indent = len(next_line) - len(next_line.lstrip())
                    if 'if' in next_line and next_indent > indent_level:
                        issues.append(Issue(
                            type=IssueType.CODEBERT,
                            severity=Severity.INFO,
                            file=file_path,
                            line=actual_line_num,
                            column=1,
                            message="Consider using early return instead of nested conditions",
                            suggestion="Restructure with early return for better readability",
                            rule="codebert-early-return"
                        ))
                        break
            
            # Check for const correctness opportunities
            if re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*[^=]', line):
                if 'const' not in line and not line.strip().endswith('++') and not line.strip().endswith('--'):
                    # Simple heuristic: if variable is not modified later
                    var_match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*=', line)
                    if var_match:
                        var_name = var_match.group(1)
                        # Check if variable is modified in subsequent lines
                        is_modified = False
                        for j in range(i + 1, min(i + 20, len(lines))):
                            if re.search(rf'\b{var_name}\s*[=+\-*/]', lines[j]):
                                is_modified = True
                                break
                        
                        if not is_modified:
                            issues.append(Issue(
                                type=IssueType.CODEBERT,
                                severity=Severity.INFO,
                                file=file_path,
                                line=actual_line_num,
                                column=1,
                                message=f"Variable '{var_name}' could be const",
                                suggestion="Add const qualifier for immutable variables",
                                rule="codebert-const-correctness"
                            ))
            
            # Check for LLVM data structure usage
            if re.search(r'\bstd::(vector|string|map)', line):
                issues.append(Issue(
                    type=IssueType.CODEBERT,
                    severity=Severity.INFO,
                    file=file_path,
                    line=actual_line_num,
                    column=1,
                    message="Consider using LLVM equivalents (SmallVector, StringRef, DenseMap)",
                    suggestion="Use LLVM data structures for better performance",
                    rule="codebert-llvm-datastructures"
                ))
        
        return issues
    
    def _analyze_naming_issues(self, file_path: str, segment: Dict, code_context: str, all_lines: List[str]) -> List[Issue]:
        """Analyze naming convention issues using CodeBERT"""
        issues = []
        
        lines = code_context.split('\n')
        
        for i, line in enumerate(lines):
            actual_line_num = segment['start_line'] - 5 + i + 1
            
            if actual_line_num not in segment['lines']:
                continue
            
            # Advanced naming pattern detection
            # Check for inconsistent naming within the same scope
            identifiers = re.findall(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', line)
            
            naming_styles = {
                'snake_case': 0,
                'camelCase': 0,
                'PascalCase': 0
            }
            
            for identifier in identifiers:
                if '_' in identifier and identifier.islower():
                    naming_styles['snake_case'] += 1
                elif identifier[0].islower() and any(c.isupper() for c in identifier[1:]):
                    naming_styles['camelCase'] += 1
                elif identifier[0].isupper():
                    naming_styles['PascalCase'] += 1
            
            # If mixed naming styles detected
            used_styles = sum(1 for count in naming_styles.values() if count > 0)
            if used_styles > 1:
                issues.append(Issue(
                    type=IssueType.CODEBERT,
                    severity=Severity.INFO,
                    file=file_path,
                    line=actual_line_num,
                    column=1,
                    message="Inconsistent naming styles detected in line",
                    suggestion="Use consistent LLVM naming conventions",
                    rule="codebert-naming-consistency"
                ))
        
        return issues
    
    def _analyze_comment_issues(self, file_path: str, segment: Dict, code_context: str, all_lines: List[str]) -> List[Issue]:
        """Analyze comment quality using CodeBERT"""
        issues = []
        
        lines = code_context.split('\n')
        
        for i, line in enumerate(lines):
            actual_line_num = segment['start_line'] - 5 + i + 1
            
            if actual_line_num not in segment['lines']:
                continue
            
            # Check comment-to-code ratio
            if '//' in line:
                comment_part = line[line.index('//'):]
                code_part = line[:line.index('//')]
                
                # If comment is much longer than code, might be over-commenting
                if len(comment_part) > len(code_part) * 2 and len(code_part.strip()) > 0:
                    issues.append(Issue(
                        type=IssueType.CODEBERT,
                        severity=Severity.INFO,
                        file=file_path,
                        line=actual_line_num,
                        column=line.index('//') + 1,
                        message="Comment may be overly verbose",
                        suggestion="Consider more concise documentation",
                        rule="codebert-verbose-comment"
                    ))
            
            # Check for missing comments on complex lines
            complexity_indicators = ['?', '&&', '||', 'static_cast', 'dynamic_cast', 'reinterpret_cast']
            if any(indicator in line for indicator in complexity_indicators):
                if '//' not in line and '/*' not in line:
                    issues.append(Issue(
                        type=IssueType.CODEBERT,
                        severity=Severity.INFO,
                        file=file_path,
                        line=actual_line_num,
                        column=1,
                        message="Complex expression might benefit from a comment",
                        suggestion="Add comment explaining the logic",
                        rule="codebert-complex-uncommented"
                    ))
        
        return issues
    
    def _analyze_maintainability_issues(self, file_path: str, segment: Dict, code_context: str, all_lines: List[str]) -> List[Issue]:
        """Analyze maintainability issues using CodeBERT"""
        issues = []
        
        lines = code_context.split('\n')
        
        # Function length analysis
        function_start = None
        brace_count = 0
        
        for i, line in enumerate(lines):
            actual_line_num = segment['start_line'] - 5 + i + 1
            
            if re.search(r'\b[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*{', line):
                function_start = i
                brace_count = line.count('{') - line.count('}')
            elif function_start is not None:
                brace_count += line.count('{') - line.count('}')
                if brace_count == 0:  # Function ended
                    function_length = i - function_start + 1
                    if function_length > 50:  # Arbitrary threshold
                        issues.append(Issue(
                            type=IssueType.CODEBERT,
                            severity=Severity.INFO,
                            file=file_path,
                            line=segment['start_line'] - 5 + function_start + 1,
                            column=1,
                            message=f"Function is quite long ({function_length} lines)",
                            suggestion="Consider breaking into smaller functions",
                            rule="codebert-long-function"
                        ))
                    function_start = None
            
            # Check for code duplication patterns
            if actual_line_num in segment['lines']:
                stripped_line = line.strip()
                if len(stripped_line) > 20:  # Only check substantial lines
                    # Simple duplication check within the segment
                    similar_lines = [j for j, other_line in enumerate(lines) 
                                   if j != i and other_line.strip() == stripped_line]
                    
                    if similar_lines:
                        issues.append(Issue(
                            type=IssueType.CODEBERT,
                            severity=Severity.INFO,
                            file=file_path,
                            line=actual_line_num,
                            column=1,
                            message="Potential code duplication detected",
                            suggestion="Consider extracting common code into a function",
                            rule="codebert-duplication"
                        ))
        
        return issues

class EnhancedLLVMPRComplianceTool:
    """Enhanced main tool orchestrating all checkers with better integration"""
    
    def __init__(self, github_token: str, use_codebert: bool = True):
        self.pr_analyzer = GitHubPRAnalyzer(github_token)
        self.clang_format = EnhancedClangFormatChecker()
        self.clang_tidy = FilteredClangTidyChecker()
        self.custom_checker = CustomLLVMChecker()
        
        if use_codebert:
            self.codebert_analyzer = CodeBERTAnalyzer()
        else:
            self.codebert_analyzer = None
        
        # Statistics tracking
        self.stats = {
            'files_analyzed': 0,
            'total_issues': 0,
            'issues_by_type': {},
            'issues_by_severity': {}
        }
    
    def analyze_pr(self, repo_name: str, pr_number: int, local_repo_path: str = None) -> Dict[str, List[Issue]]:
        """Enhanced PR analysis with better error handling and progress tracking"""
        print(f"🔍 Analyzing PR #{pr_number} in {repo_name}...")
        
        try:
            # Get PR changes
            pr_changes = self.pr_analyzer.get_pr_changes(repo_name, pr_number)
            
            if not pr_changes:
                print("ℹ️  No C/C++ files modified in this PR")
                return {}
            
            print(f"📁 Found {len(pr_changes)} modified files")
            
            all_issues = {}
            
            for file_path, file_info in pr_changes.items():
                self.stats['files_analyzed'] += 1
                print(f"  Checking {file_path}...")
                
                file_issues = self._analyze_single_file(file_path, file_info, local_repo_path)
                
                if file_issues:
                    all_issues[file_path] = file_issues
                    self._update_stats(file_issues)
                
                print(f"    Found {len(file_issues)} issues")
            
            self._print_analysis_summary()
            return all_issues
        
        except Exception as e:
            print(f"❌ Error analyzing PR: {e}")
            return {}
    
    def _analyze_single_file(self, file_path: str, file_info: Dict, local_repo_path: str = None) -> List[Issue]:
        """Analyze a single file with all checkers"""
        file_issues = []
        
        # Determine actual file path
        actual_path = file_path
        if local_repo_path:
            actual_path = os.path.join(local_repo_path, file_path)
        
        # Check if file exists or use content from PR
        if not os.path.exists(actual_path) and file_info.get('file_content'):
            # Create temporary file
            with tempfile.NamedTemporaryFile(mode='w', suffix=Path(file_path).suffix, delete=False) as tmp_file:
                tmp_file.write(file_info['file_content'])
                actual_path = tmp_file.name
        
        if not os.path.exists(actual_path):
            print(f"    ⚠️  Warning: {file_path} not accessible, skipping tool-based checks")
            if self.codebert_analyzer:
                # Still run CodeBERT if we have content
                file_issues.extend(self.codebert_analyzer.analyze_code_segment(file_path, file_info))
            return file_issues
        
        try:
            # Extract modified line numbers for filtering
            modified_line_nums = [line_info[0] for line_info in file_info.get('modified_lines', []) 
                                if len(line_info) >= 2 and (len(line_info) < 3 or line_info[2] != 'deleted')]
            modified_line_set = set(modified_line_nums)
            
            if not modified_line_nums:
                return file_issues
            
            # Run clang-format check
            try:
                format_issues = self.clang_format.check_modified_lines(actual_path, file_info.get('modified_lines', []))
                file_issues.extend(format_issues)
            except Exception as e:
                print(f"      ⚠️  clang-format check failed: {e}")
            
            # Run clang-tidy check
            try:
                tidy_issues = self.clang_tidy.check_modified_lines(actual_path, modified_line_set)
                file_issues.extend(tidy_issues)
            except Exception as e:
                print(f"      ⚠️  clang-tidy check failed: {e}")
            
            # Run custom LLVM checks
            try:
                custom_issues = self.custom_checker.check_modified_lines(actual_path, file_info)
                file_issues.extend(custom_issues)
            except Exception as e:
                print(f"      ⚠️  Custom checks failed: {e}")
            
            # Run CodeBERT analysis
            if self.codebert_analyzer:
                try:
                    codebert_issues = self.codebert_analyzer.analyze_code_segment(actual_path, file_info)
                    file_issues.extend(codebert_issues)
                except Exception as e:
                    print(f"      ⚠️  CodeBERT analysis failed: {e}")
            
            # Clean up temporary file
            if actual_path != file_path and actual_path.startswith('/tmp'):
                try:
                    os.unlink(actual_path)
                except:
                    pass
        
        except Exception as e:
            print(f"      ❌ Error analyzing {file_path}: {e}")
        
        return file_issues
    
    def _update_stats(self, issues: List[Issue]):
        """Update analysis statistics"""
        self.stats['total_issues'] += len(issues)
        
        for issue in issues:
            # Update type stats
            issue_type = issue.type.value
            if issue_type not in self.stats['issues_by_type']:
                self.stats['issues_by_type'][issue_type] = 0
            self.stats['issues_by_type'][issue_type] += 1
            
            # Update severity stats
            severity = issue.severity.value
            if severity not in self.stats['issues_by_severity']:
                self.stats['issues_by_severity'][severity] = 0
            self.stats['issues_by_severity'][severity] += 1
    
    def _print_analysis_summary(self):
        """Print analysis summary statistics"""
        print(f"\n📊 Analysis Summary:")
        print(f"   Files analyzed: {self.stats['files_analyzed']}")
        print(f"   Total issues: {self.stats['total_issues']}")
        
        if self.stats['issues_by_severity']:
            print(f"   By severity: {dict(self.stats['issues_by_severity'])}")
        
        if self.stats['issues_by_type']:
            print(f"   By type: {dict(self.stats['issues_by_type'])}")
    
    def print_detailed_report(self, issues: Dict[str, List[Issue]]):
        """Print comprehensive compliance report"""
        total_issues = sum(len(file_issues) for file_issues in issues.values())
        
        print("\n" + "="*100)
        print(f"🔍 LLVM PR COMPLIANCE REPORT")
        print("="*100)
        
        if total_issues == 0:
            print("✅ Excellent! All checks passed - the PR complies with LLVM coding standards.")
            print("🎉 Ready for review and merge!")
            return
        
        print(f"📊 Summary: {total_issues} issues found across {len(issues)} files")
        print(f"[DEBUG] Detected issue types: {[issue.type for f in issues.values() for issue in f]}")

        
        # Print severity summary
        severity_counts = {}
        for file_issues in issues.values():
            for issue in file_issues:
                severity_counts[issue.severity.value] = severity_counts.get(issue.severity.value, 0) + 1
        
        if severity_counts:
            print("📈 Severity breakdown:")
            for severity, count in sorted(severity_counts.items()):
                emoji = {"error": "🔴", "warning": "🟡", "info": "🔵"}.get(severity, "⚪")
                print(f"   {emoji} {severity.upper()}: {count}")
        
        print("\n" + "-"*100)
        
        # Print detailed issues by file
        for file_path, file_issues in issues.items():
            if not file_issues:
                continue
            
            print(f"\n📁 {file_path}")
            print("   " + "="*80)
            
            # Group by issue type
            by_type = {}
            for issue in file_issues:
                if issue.type not in by_type:
                    by_type[issue.type] = []
                by_type[issue.type].append(issue)
            
            for issue_type, type_issues in sorted(by_type.items(), key=lambda x: x[0].value):
                type_name = issue_type.value.upper().replace('_', ' ')
                print(f"\n   🔧 {type_name} ({len(type_issues)} issues):")
                
                for issue in sorted(type_issues, key=lambda x: x.line):
                    severity_emoji = {"error": "🔴", "warning": "🟡", "info": "🔵"}[issue.severity.value]
                    
                    print(f"      {severity_emoji} Line {issue.line}:{issue.column} - {issue.message}")
                    
                    if issue.rule:
                        print(f"         Rule: {issue.rule}")
                    
                    if issue.original_code and issue.suggested_code:
                        print(f"         Original:  {issue.original_code}")
                        print(f"         Suggested: {issue.suggested_code}")
                    elif issue.suggestion:
                        print(f"         💡 Suggestion: {issue.suggestion}")
                    
                    print()
        
        print("="*100)
        
        # Print actionable summary
        error_count = severity_counts.get('error', 0)
        warning_count = severity_counts.get('warning', 0)
        
        if error_count > 0:
            print(f"🚨 {error_count} errors must be fixed before merging")
        if warning_count > 0:
            print(f"⚠️  {warning_count} warnings should be addressed")
        
        print("💡 Review the suggestions above to improve code quality and LLVM compliance")
        print("📚 Reference: https://llvm.org/docs/CodingStandards.html")
        # print("📝 Diffs with suggested corrections have been generated (see diffs.txt)")
    
    def export_json_report(self, issues: Dict[str, List[Issue]], output_file: str, include_diffs: bool = True):
        """Export detailed JSON report, including original and suggested code for all issues"""
        json_report = {
            'summary': {
                'total_files': len(issues),
                'total_issues': sum(len(file_issues) for file_issues in issues.values()),
                'stats': self.stats,
                'timestamp': datetime.now().isoformat(),
                'version': '1.0'
            },
            'files': {}
        }

        for file_path, file_issues in issues.items():
            json_report['files'][file_path] = []
            
            # Read the file to extract original lines if needed
            original_lines = []
            try:
                if os.path.exists(file_path):
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        original_lines = f.readlines()
            except Exception as e:
                print(f"Warning: Could not read {file_path} for line extraction: {e}")
            
            for issue in file_issues:
                entry = {
                    'type': issue.type.value,
                    'severity': issue.severity.value,
                    'line': issue.line,
                    'column': issue.column,
                    'message': issue.message,
                    'rule': issue.rule,
                    'suggestion': issue.suggestion
                }
                
                # Always try to include original and suggested code
                if include_diffs:
                    # Use existing original_code and suggested_code if available
                    if hasattr(issue, 'original_code') and issue.original_code:
                        entry['original_code'] = issue.original_code
                    elif original_lines and 1 <= issue.line <= len(original_lines):
                        # Extract original line from file
                        entry['original_code'] = original_lines[issue.line - 1].rstrip('\n\r')
                    else:
                        entry['original_code'] = None
                    
                    # Use existing suggested_code if available
                    if hasattr(issue, 'suggested_code') and issue.suggested_code:
                        entry['suggested_code'] = issue.suggested_code
                    elif issue.suggestion and entry['original_code']:
                        # Try to generate suggested code from suggestion and original
                        entry['suggested_code'] = self._generate_suggested_code(
                            entry['original_code'], issue.suggestion, issue.type, None
                        )
                    else:
                        # Try to generate suggestion even without explicit suggestion text (for clang-tidy)
                        entry['suggested_code'] = self._generate_suggested_code(
                            entry['original_code'], issue.suggestion, issue.type, None
                        )
                else:
                    entry['suggested_code'] = None
                    
                    # Add diff information if both codes are available
                    if entry['original_code'] and entry['suggested_code']:
                        entry['has_code_diff'] = entry['original_code'] != entry['suggested_code']
                    else:
                        entry['has_code_diff'] = False
                
                json_report['files'][file_path].append(entry)

        # Write the JSON report
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                json.dump(json_report, f, indent=2, ensure_ascii=False)
            print(f"📄 Detailed report with code diffs exported to {output_file}")
        except Exception as e:
            print(f"❌ Error writing JSON report: {e}")

    def _generate_suggested_code(self, original_code: str, suggestion: str, issue_type, context_lines=None) -> str:
        """Generate suggested code based on original code and suggestion"""
        if not original_code:
            return None
        
        # If no suggestion message, try to infer from common clang-tidy patterns
        if not suggestion:
            return self._generate_clang_tidy_suggestion(original_code, issue_type, context_lines)
        
        suggestion_lower = suggestion.lower()
        original_stripped = original_code.strip()
        
        # === CLANG-TIDY SPECIFIC FIXES ===
        
        # Braces around statements
        if ('braces' in suggestion_lower or 'statement should be inside braces' in suggestion_lower):
            # Find the next non-empty line from context
            if context_lines:
                current_line_num = None
                next_line_content = None
                
                # Find current line and get next line
                for i, ctx in enumerate(context_lines):
                    if ctx.get('is_issue_line', False):
                        if i + 1 < len(context_lines):
                            next_line_content = context_lines[i + 1]['content'].strip()
                        break
                
                if next_line_content:
                    # Extract indentation
                    indent = len(original_code) - len(original_code.lstrip())
                    spaces = ' ' * indent
                    return f"{original_code} {{\n{spaces}  {next_line_content}\n{spaces}}}"
            
            return f"{original_code} {{\n  // next statement here\n}}"
        
        # Variable naming issues
        if 'variable name' in suggestion_lower or 'parameter name' in suggestion_lower:
            if 'should be' in suggestion_lower:
                # Try to extract suggested name from message
                parts = suggestion_lower.split('should be')
                if len(parts) > 1:
                    suggested_name = parts[1].strip().strip("'\"")
                    # Replace variable name in the line
                    import re
                    # Match variable declarations or assignments
                    var_match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\b', original_stripped)
                    if var_match:
                        old_name = var_match.group(1)
                        return original_code.replace(old_name, suggested_name, 1)
        
        # Function naming
        if 'function name' in suggestion_lower:
            if 'should be' in suggestion_lower:
                parts = suggestion_lower.split('should be')
                if len(parts) > 1:
                    suggested_name = parts[1].strip().strip("'\"")
                    import re
                    func_match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', original_stripped)
                    if func_match:
                        old_name = func_match.group(1)
                        return original_code.replace(old_name, suggested_name, 1)
        
        # Const correctness
        if 'const' in suggestion_lower:
            if 'should be const' in suggestion_lower or 'make const' in suggestion_lower:
                if 'auto ' in original_code:
                    return original_code.replace('auto ', 'const auto ', 1)
                elif not original_code.strip().startswith('const'):
                    return f"const {original_code.lstrip()}"
        
        # Prefer nullptr over NULL
        if 'null' in suggestion_lower and 'nullptr' in suggestion_lower:
            return original_code.replace('NULL', 'nullptr').replace('0', 'nullptr')
        
        # Use auto
        if 'use auto' in suggestion_lower or 'auto instead' in suggestion_lower:
            import re
            # Replace explicit type with auto
            type_match = re.search(r'\b(int|double|float|char|bool|std::\w+|[A-Z]\w*)\s+', original_stripped)
            if type_match:
                return original_code.replace(type_match.group(0), 'auto ', 1)
        
        # Range-based for loops
        if 'range-based' in suggestion_lower or 'range based' in suggestion_lower:
            import re
            # Convert traditional for to range-based
            for_match = re.search(r'for\s*\([^)]+\)', original_stripped)
            if for_match:
                return original_code.replace(for_match.group(0), 'for (const auto& item : container)')
        
        # Redundant string init
        if 'redundant' in suggestion_lower and 'string' in suggestion_lower:
            return original_code.replace('= ""', '').replace('("")', '()')
        
        # Unnecessary parentheses
        if 'unnecessary parentheses' in suggestion_lower or 'redundant parentheses' in suggestion_lower:
            import re
            # Remove outer parentheses if they exist
            stripped = original_stripped.strip()
            if stripped.startswith('(') and stripped.endswith(')'):
                # Check if parentheses are actually redundant
                inner = stripped[1:-1]
                if inner.count('(') == inner.count(')'):
                    return original_code.replace('(' + inner + ')', inner, 1)
        
        # Static cast instead of C-style cast
        if 'static_cast' in suggestion_lower or 'c-style cast' in suggestion_lower:
            import re
            cast_match = re.search(r'\(([^)]+)\)\s*([^;,)]+)', original_stripped)
            if cast_match:
                cast_type = cast_match.group(1)
                expression = cast_match.group(2)
                return original_code.replace(cast_match.group(0), f'static_cast<{cast_type}>({expression})', 1)
        
        # === GENERAL FORMATTING FIXES ===
        
        # Spacing issues
        if 'spacing' in suggestion_lower or 'space' in suggestion_lower:
            if 'after comma' in suggestion_lower:
                return original_code.replace(',', ', ')
            elif 'around operator' in suggestion_lower:
                import re
                # Add spaces around operators
                result = original_code
                for op in ['=', '+', '-', '*', '/', '%', '==', '!=', '<', '>', '<=', '>=']:
                    result = re.sub(f'(\\w){op}(\\w)', f'\\1 {op} \\2', result)
                return result
            elif 'before paren' in suggestion_lower:
                return original_code.replace('(', ' (')
        
        # Indentation
        if 'indent' in suggestion_lower:
            lines = original_code.split('\n')
            if 'increase' in suggestion_lower:
                return '\n'.join('  ' + line for line in lines)
            elif 'decrease' in suggestion_lower:
                return '\n'.join(line[2:] if line.startswith('  ') else line for line in lines)
        
        # If we can't generate a specific fix, return the original with a comment
        return f"{original_code}  // TODO: {suggestion}"

    def _generate_clang_tidy_suggestion(self, original_code: str, issue_type, context_lines=None) -> str:
        """Generate suggestions for clang-tidy issues without explicit suggestion text"""
        if not issue_type or issue_type.value != 'clang-tidy':
            return None
        
        original_stripped = original_code.strip()
        
        # Common clang-tidy fixes based on code patterns
        
        # If statement without braces (common pattern)
        if original_stripped.startswith(('if ', 'for ', 'while ')) and not original_stripped.endswith('{'):
            if context_lines:
                # Get next line for brace wrapping
                for i, ctx in enumerate(context_lines):
                    if ctx.get('is_issue_line', False) and i + 1 < len(context_lines):
                        next_line = context_lines[i + 1]['content']
                        indent = len(original_code) - len(original_code.lstrip())
                        spaces = ' ' * indent
                        return f"{original_code} {{\n{next_line}\n{spaces}}}"
            return f"{original_code} {{\n  // statement here\n}}"
        
        # Variable declarations that might need const
        if re.search(r'\b(auto|int|double|float|char|bool)\s+\w+\s*=', original_stripped):
            if not original_stripped.startswith('const'):
                return f"const {original_code.lstrip()}"
        
        # NULL to nullptr
        if 'NULL' in original_code:
            return original_code.replace('NULL', 'nullptr')
        
        # C-style casts
        cast_match = re.search(r'\(([^)]+)\)\s*([^;,)]+)', original_stripped)
        if cast_match:
            cast_type = cast_match.group(1)
            expression = cast_match.group(2)
            return original_code.replace(cast_match.group(0), f'static_cast<{cast_type}>({expression})', 1)
        
        return None

    def export_enhanced_json_report(self, issues: Dict[str, List[Issue]], output_file: str):
        """Export comprehensive JSON report with enhanced code diff information"""
        json_report = {
            'metadata': {
                'tool_version': 'EnhancedLLVMPRComplianceTool v2.0',
                'analysis_timestamp': datetime.now().isoformat(),
                'total_files_analyzed': self.stats['files_analyzed'],
                'total_issues_found': self.stats['total_issues']
            },
            'summary': {
                'files_with_issues': len(issues),
                'issues_by_severity': self.stats.get('issues_by_severity', {}),
                'issues_by_type': self.stats.get('issues_by_type', {})
            },
            'detailed_results': {}
        }

        for file_path, file_issues in issues.items():
            file_result = {
                'file_info': {
                    'path': file_path,
                    'total_issues': len(file_issues),
                    'issues_by_severity': {},
                    'issues_by_type': {}
                },
                'issues': []
            }
            
            # Calculate file-level statistics
            for issue in file_issues:
                severity = issue.severity.value
                issue_type = issue.type.value
                
                file_result['file_info']['issues_by_severity'][severity] = \
                    file_result['file_info']['issues_by_severity'].get(severity, 0) + 1
                file_result['file_info']['issues_by_type'][issue_type] = \
                    file_result['file_info']['issues_by_type'].get(issue_type, 0) + 1
            
            # Read original file content for line extraction
            original_lines = []
            try:
                if os.path.exists(file_path):
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        original_lines = f.readlines()
            except Exception:
                pass
            
            # Process each issue
            for issue in file_issues:
                issue_entry = {
                    'issue_id': f"{file_path}:{issue.line}:{issue.column}",
                    'type': issue.type.value,
                    'severity': issue.severity.value,
                    'location': {
                        'line': issue.line,
                        'column': issue.column
                    },
                    'description': {
                        'message': issue.message,
                        'rule': issue.rule,
                        'suggestion': issue.suggestion
                    },
                    'code_context': {
                        'original_line': None,
                        'suggested_line': None,
                        'context_lines': []
                    }
                }
                
                # Extract original line
                if original_lines and 1 <= issue.line <= len(original_lines):
                    issue_entry['code_context']['original_line'] = original_lines[issue.line - 1].rstrip('\n\r')
                    
                    # Add context lines (2 before and 2 after)
                    start_line = max(0, issue.line - 3)
                    end_line = min(len(original_lines), issue.line + 2)
                    
                    for i in range(start_line, end_line):
                        context_entry = {
                            'line_number': i + 1,
                            'content': original_lines[i].rstrip('\n\r'),
                            'is_issue_line': (i + 1) == issue.line
                        }
                        issue_entry['code_context']['context_lines'].append(context_entry)
                
                # Use existing suggested code or generate it
                if hasattr(issue, 'suggested_code') and issue.suggested_code:
                    issue_entry['code_context']['suggested_line'] = issue.suggested_code
                elif hasattr(issue, 'original_code') and issue.original_code:
                    issue_entry['code_context']['original_line'] = issue.original_code
                    if hasattr(issue, 'suggested_code') and issue.suggested_code:
                        issue_entry['code_context']['suggested_line'] = issue.suggested_code
                elif issue_entry['code_context']['original_line'] and issue.suggestion:
                    issue_entry['code_context']['suggested_line'] = self._generate_suggested_code(
                        issue_entry['code_context']['original_line'], 
                        issue.suggestion, 
                        issue.type
                    )
                
                # Add diff information
                if (issue_entry['code_context']['original_line'] and 
                    issue_entry['code_context']['suggested_line']):
                    issue_entry['code_context']['has_suggested_fix'] = True
                    issue_entry['code_context']['diff_available'] = True
                else:
                    issue_entry['code_context']['has_suggested_fix'] = False
                    issue_entry['code_context']['diff_available'] = False
                
                file_result['issues'].append(issue_entry)
            
            json_report['detailed_results'][file_path] = file_result

        # Write the enhanced JSON report
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                json.dump(json_report, f, indent=2, ensure_ascii=False)
            print(f"📄 Enhanced JSON report with comprehensive code diffs exported to {output_file}")
            print(f"📊 Report includes {json_report['summary']['files_with_issues']} files with issues")
        except Exception as e:
            print(f"❌ Error writing enhanced JSON report: {e}")

def write_diffs(issues_by_file: Dict[str, List[Issue]], output_path="diffs.txt"):
    with open(output_path, 'w') as f:
        for file_path, issues in issues_by_file.items():
            for issue in issues:
                if issue.original_code and issue.suggested_code:
                    diff = difflib.unified_diff(
                        issue.original_code.splitlines(),
                        issue.suggested_code.splitlines(),
                        fromfile='original',
                        tofile='suggested',
                        lineterm=''
                    )
                    f.write(f"--- {file_path} (Line {issue.line}) ---\n")
                    f.writelines('\n'.join(diff) + '\n\n')

def main():
    parser = argparse.ArgumentParser(
        description='Enhanced LLVM PR Compliance Tool with CodeBERT Integration',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s --repo llvm/llvm-project --pr 12345 --github-token TOKEN
  %(prog)s --repo llvm/llvm-project --pr 12345 --github-token TOKEN --local-path /path/to/llvm
  %(prog)s --repo llvm/llvm-project --pr 12345 --github-token TOKEN --output report.json --no-codebert
        """
    )
    
    parser.add_argument('--repo', required=True, 
                       help='Repository name (e.g., llvm/llvm-project)')
    parser.add_argument('--pr', type=int, required=True, 
                       help='Pull request number')
    parser.add_argument('--github-token', required=True, 
                       help='GitHub personal access token')
    parser.add_argument('--local-path', 
                       help='Local path to LLVM repository (optional)')
    parser.add_argument('--output', 
                       help='Output file for JSON report')
    parser.add_argument('--no-codebert', action='store_true',
                       help='Disable CodeBERT analysis')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    
    args = parser.parse_args()
    
    if args.verbose:
        print("🚀 Starting LLVM PR Compliance Analysis...")
        print(f"   Repository: {args.repo}")
        print(f"   PR Number: {args.pr}")
        print(f"   CodeBERT: {'Disabled' if args.no_codebert else 'Enabled'}")
        if args.local_path:
            print(f"   Local Path: {args.local_path}")
    
    # Initialize tool
    tool = EnhancedLLVMPRComplianceTool(
        github_token=args.github_token,
        use_codebert=not args.no_codebert
    )
    
    # Analyze PR
    try:
        issues = tool.analyze_pr(args.repo, args.pr, args.local_path)
        
        # Print detailed report
        tool.print_detailed_report(issues)

        # if any(issue.original_code and issue.suggested_code for file_issues in issues.values() for issue in file_issues):
        #     write_diffs(issues)

        
        # Export JSON report if requested
        if args.output:
            tool.export_enhanced_json_report(issues, args.output)
        
        # Exit with appropriate code
        total_issues = sum(len(file_issues) for file_issues in issues.values())
        error_count = sum(1 for file_issues in issues.values() 
                         for issue in file_issues 
                         if issue.severity == Severity.ERROR)
        
        if error_count > 0:
            print(f"\n❌ Exiting with error code due to {error_count} errors")
            sys.exit(1)
        elif total_issues > 0:
            print(f"\n⚠️  Exiting with warning code due to {total_issues} issues")
            sys.exit(0)
        else:
            print("\n✅ All checks passed")
            sys.exit(0)
    except KeyboardInterrupt:
        print("\n❗ Analysis interrupted by user.")
        sys.exit(130)
    except Exception as e:
        print(f"\n❌ Unexpected error: {e}")
        sys.exit(2)

if __name__ == "__main__":
    main()
