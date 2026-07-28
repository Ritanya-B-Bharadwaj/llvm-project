# Detailed Examples

This document provides comprehensive examples of using the Clang AST Line Mapper tool.

## Table of Contents

1. [Basic Usage Examples](#basic-usage-examples)
2. [Advanced C++ Features](#advanced-c-features)
3. [Output Format Examples](#output-format-examples)
4. [API Usage Examples](#api-usage-examples)
5. [Integration Examples](#integration-examples)

## Basic Usage Examples

### Example 1: Simple Function

**Input (`examples/simple.cpp`):**
```cpp
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    return 0;
}
```

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp
```

**Output:**
```
============================================================
AST-ANNOTATED SOURCE: examples/simple.cpp
============================================================

  1: int add(int a, int b) {
     AST: FunctionDecl, ParmVarDecl

  2:     return a + b;
     AST: ReturnStmt

  3: }

  4: 
  5: int main() {
     AST: FunctionDecl, CompoundStmt

  6:     int result = add(5, 3);
     AST: DeclStmt

  7:     return 0;
     AST: ReturnStmt

  8: }
```

### Example 2: With Explanations

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp --explanations
```

**Output:**
```
============================================================
AST-ANNOTATED SOURCE: examples/simple.cpp
============================================================

  1: int add(int a, int b) {
     AST: FunctionDecl, ParmVarDecl
          → FunctionDecl: function declaration
          → ParmVarDecl: parameter variable declaration

  2:     return a + b;
     AST: ReturnStmt
          → ReturnStmt: return statement

  3: }

  4: 
  5: int main() {
     AST: FunctionDecl, CompoundStmt
          → FunctionDecl: function declaration
          → CompoundStmt: compound statement (block)

  6:     int result = add(5, 3);
     AST: DeclStmt
          → DeclStmt: declaration statement

  7:     return 0;
     AST: ReturnStmt
          → ReturnStmt: return statement

  8: }
```

### Example 3: Side-by-Side View

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp --side-by-side
```

**Output:**
```
====================================================================================================
SIDE-BY-SIDE VIEW: examples/simple.cpp
====================================================================================================
SOURCE CODE                                          AST NODES                               
------------------------------------------------------------ ----------------------------------------
  1: int add(int a, int b) {                        AST: FunctionDecl, ParmVarDecl         
  2:     return a + b;                              AST: ReturnStmt                         
  3: }                                                                                       
  4:                                                                                        
  5: int main() {                                   AST: FunctionDecl, CompoundStmt        
  6:     int result = add(5, 3);                    AST: DeclStmt                          
  7:     return 0;                                  AST: ReturnStmt                         
  8: }                                                                                       
```

## Advanced C++ Features

### Example 4: Classes and Methods

**Input (`examples/complex.cpp`):**
```cpp
class Calculator {
private:
    int value;
    
public:
    Calculator(int initial) : value(initial) {}
    
    int add(int a, int b) {
        return a + b;
    }
    
    void setValue(int newValue) {
        value = newValue;
    }
    
    int getValue() const {
        return value;
    }
};

int main() {
    Calculator calc(10);
    int result = calc.add(5, 3);
    calc.setValue(result);
    return calc.getValue();
}
```

**Command:**
```bash
python ast_line_mapper.py examples/complex.cpp --explanations
```

**Output:**
```
============================================================
AST-ANNOTATED SOURCE: examples/complex.cpp
============================================================

  1: class Calculator {
     AST: CXXRecordDecl
          → CXXRecordDecl: C++ class/struct declaration

  2: private:
     AST: AccessSpecDecl
          → AccessSpecDecl: access specifier (public/private/protected)

  3:     int value;
     AST: FieldDecl
          → FieldDecl: field declaration

  4: 
  5: public:
     AST: AccessSpecDecl
          → AccessSpecDecl: access specifier (public/private/protected)

  6:     Calculator(int initial) : value(initial) {}
     AST: CXXConstructorDecl
          → CXXConstructorDecl: C++ constructor declaration

  7: 
  8:     int add(int a, int b) {
     AST: CXXMethodDecl, ParmVarDecl
          → CXXMethodDecl: C++ method declaration
          → ParmVarDecl: parameter variable declaration

  9:         return a + b;
     AST: ReturnStmt
          → ReturnStmt: return statement

 10:     }
```

### Example 5: Templates

**Input (`examples/templates.cpp`):**
```cpp
template<typename T>
class Container {
private:
    T data;
    
public:
    Container(const T& value) : data(value) {}
    
    T getValue() const {
        return data;
    }
};

template<typename T>
T max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

int main() {
    Container<int> intContainer(42);
    int maxValue = max(10, 20);
    return maxValue;
}
```

**Command:**
```bash
python ast_line_mapper.py examples/templates.cpp --explanations
```

**Output:**
```
============================================================
AST-ANNOTATED SOURCE: examples/templates.cpp
============================================================

  1: template<typename T>
     AST: ClassTemplateDecl
          → ClassTemplateDecl: class template declaration

  2: class Container {
     AST: CXXRecordDecl
          → CXXRecordDecl: C++ class/struct declaration

  3: private:
     AST: AccessSpecDecl
          → AccessSpecDecl: access specifier (public/private/protected)

  4:     T data;
     AST: FieldDecl
          → FieldDecl: field declaration

  5: 
  6: public:
     AST: AccessSpecDecl
          → AccessSpecDecl: access specifier (public/private/protected)

  7:     Container(const T& value) : data(value) {}
     AST: CXXConstructorDecl
          → CXXConstructorDecl: C++ constructor declaration

  8: 
  9:     T getValue() const {
     AST: CXXMethodDecl, CompoundStmt
          → CXXMethodDecl: C++ method declaration
          → CompoundStmt: compound statement (block)

 10:         return data;
     AST: ReturnStmt
          → ReturnStmt: return statement

 11:     }
```

### Example 6: Control Flow

**Input (`examples/control_flow.cpp`):**
```cpp
int main() {
    int x = 10;
    int y = 20;
    
    if (x < y) {
        x = x + y;
    } else {
        x = x - y;
    }
    
    for (int i = 0; i < 5; i++) {
        x = x * 2;
    }
    
    while (x > 100) {
        x = x / 2;
    }
    
    return x;
}
```

**Command:**
```bash
python ast_line_mapper.py examples/control_flow.cpp --explanations
```

**Output:**
```
============================================================
AST-ANNOTATED SOURCE: examples/control_flow.cpp
============================================================

  1: int main() {
     AST: FunctionDecl, CompoundStmt
          → FunctionDecl: function declaration
          → CompoundStmt: compound statement (block)

  2:     int x = 10;
     AST: DeclStmt
          → DeclStmt: declaration statement

  3:     int y = 20;
     AST: DeclStmt
          → DeclStmt: declaration statement

  4: 
  5:     if (x < y) {
     AST: IfStmt, BinaryOperator
          → IfStmt: if statement
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

  6:         x = x + y;
     AST: BinaryOperator
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

  7:     } else {
     AST: CompoundStmt
          → CompoundStmt: compound statement (block)

  8:         x = x - y;
     AST: BinaryOperator
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

  9:     }
     
 10: 
 11:     for (int i = 0; i < 5; i++) {
     AST: ForStmt, DeclStmt
          → ForStmt: for loop statement
          → DeclStmt: declaration statement

 12:         x = x * 2;
     AST: BinaryOperator
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

 13:     }
     
 14: 
 15:     while (x > 100) {
     AST: WhileStmt, BinaryOperator
          → WhileStmt: while loop statement
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

 16:         x = x / 2;
     AST: BinaryOperator
          → BinaryOperator: binary operator (e.g., +, -, *, /, ==, !=)

 17:     }
     
 18: 
 19:     return x;
     AST: ReturnStmt
          → ReturnStmt: return statement

 20: }
```

## Output Format Examples

### Example 7: JSON Output

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp --format json
```

**Output:**
```json
{
  "line_mappings": {
    "1": ["FunctionDecl", "ParmVarDecl"],
    "2": ["ReturnStmt"],
    "5": ["FunctionDecl", "CompoundStmt"],
    "6": ["DeclStmt"],
    "7": ["ReturnStmt"]
  },
  "explanations": {
    "FunctionDecl": "function declaration",
    "ParmVarDecl": "parameter variable declaration",
    "ReturnStmt": "return statement",
    "CompoundStmt": "compound statement (block)",
    "DeclStmt": "declaration statement"
  }
}
```

### Example 8: Save to File

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp --explanations --output result.txt
```

**Output:**
```
🔍 Processing: examples/simple.cpp
✅ AST JSON generated: simple_ast.json
✅ Output saved to: result.txt
```

### Example 9: Generate AST Only

**Command:**
```bash
python ast_line_mapper.py examples/simple.cpp --generate-ast
```

**Output:**
```
🔍 Processing: examples/simple.cpp
✅ AST JSON generated: simple_ast.json
```

## API Usage Examples

### Example 10: Basic API Usage

```python
from ast_parser import ASTParser
from source_annotator import SourceAnnotator
from node_explanations import NodeExplanations

# Initialize components
parser = ASTParser()
annotator = SourceAnnotator()
explanations = NodeExplanations()

# Process file
cpp_file = "examples/simple.cpp"
ast_file = parser.generate_ast_json(cpp_file)

if ast_file:
    # Parse AST
    line_mappings = parser.parse_ast_file(ast_file, cpp_file)
    
    # Annotate source
    annotated = annotator.annotate_source(
        cpp_file,
        line_mappings,
        include_explanations=True,
        explanations_dict=explanations.get_all_explanations()
    )
    
    print(annotated)
```

### Example 11: HTML Generation

```python
from ast_parser import ASTParser
from source_annotator import SourceAnnotator
from node_explanations import NodeExplanations

# Initialize components
parser = ASTParser()
annotator = SourceAnnotator()
explanations = NodeExplanations()

# Process file
cpp_file = "examples/complex.cpp"
ast_file = parser.generate_ast_json(cpp_file)

if ast_file:
    line_mappings = parser.parse_ast_file(ast_file, cpp_file)
    
    # Generate HTML output
    html_output = annotator.generate_html_output(
        cpp_file,
        line_mappings,
        explanations_dict=explanations.get_all_explanations()
    )
    
    # Save to file
    with open("output.html", "w") as f:
        f.write(html_output)
    
    print("HTML output saved to output.html")
```

### Example 12: Statistics and Analysis

```python
from ast_parser import ASTParser

parser = ASTParser()
cpp_file = "examples/complex.cpp"
ast_file = parser.generate_ast_json(cpp_file)

if ast_file:
    line_mappings = parser.parse_ast_file(ast_file, cpp_file)
    stats = parser.get_statistics()
    
    print("=== AST Statistics ===")
    print(f"Total lines with AST: {stats['total_lines_with_ast']}")
    print(f"Total AST nodes: {stats['total_ast_nodes']}")
    print(f"Average nodes per line: {stats['avg_nodes_per_line']:.2f}")
    
    print("\n=== Most Common Node Types ===")
    for node_type, count in stats['most_common_nodes']:
        print(f"{node_type}: {count}")
```

### Example 13: Custom Explanations

```python
from node_explanations import NodeExplanations

explanations = NodeExplanations()

# Add custom explanations
explanations.add_explanation(
    "CustomNode", 
    "my custom AST node explanation"
)

# Get categorized explanations
categories = explanations.get_categories()
for category, nodes in categories.items():
    print(f"\n=== {category} ===")
    for node_type, explanation in nodes[:5]:  # Show first 5
        print(f"{node_type}: {explanation}")
```

## Integration Examples

### Example 14: Batch Processing

```python
import os
import glob
from ast_parser import ASTParser
from source_annotator import SourceAnnotator
from node_explanations import NodeExplanations

def process_directory(directory_path, output_dir):
    """Process all .cpp files in a directory."""
    parser = ASTParser()
    annotator = SourceAnnotator()
    explanations = NodeExplanations()
    
    # Find all .cpp files
    cpp_files = glob.glob(os.path.join(directory_path, "*.cpp"))
    
    for cpp_file in cpp_files:
        print(f"Processing: {cpp_file}")
        
        # Generate AST
        ast_file = parser.generate_ast_json(cpp_file)
        if not ast_file:
            print(f"  ❌ Failed to generate AST for {cpp_file}")
            continue
        
        # Parse and annotate
        line_mappings = parser.parse_ast_file(ast_file, cpp_file)
        if not line_mappings:
            print(f"  ❌ Failed to parse AST for {cpp_file}")
            continue
        
        # Generate HTML output
        html_output = annotator.generate_html_output(
            cpp_file,
            line_mappings,
            explanations_dict=explanations.get_all_explanations()
        )
        
        # Save output
        base_name = os.path.basename(cpp_file).replace('.cpp', '')
        output_file = os.path.join(output_dir, f"{base_name}_annotated.html")
        
        with open(output_file, 'w') as f:
            f.write(html_output)
        
        print(f"  ✅ Generated: {output_file}")

# Usage
process_directory("examples/", "output/")
```

### Example 15: Build System Integration

```python
#!/usr/bin/env python3
"""
Integration script for CMake build system.
Generates AST annotations for all source files in a project.
"""

import os
import sys
import json
import subprocess
from pathlib import Path

def find_cpp_files(build_dir):
    """Find all C++ source files from compile_commands.json."""
    compile_commands_file = os.path.join(build_dir, "compile_commands.json")
    
    if not os.path.exists(compile_commands_file):
        print("❌ compile_commands.json not found. Enable CMAKE_EXPORT_COMPILE_COMMANDS.")
        return []
    
    with open(compile_commands_file, 'r') as f:
        compile_commands = json.load(f)
    
    cpp_files = []
    for entry in compile_commands:
        if entry['file'].endswith(('.cpp', '.cc', '.cxx')):
            cpp_files.append(entry['file'])
    
    return cpp_files

def generate_project_report(build_dir, output_dir):
    """Generate AST report for entire project."""
    from ast_parser import ASTParser
    from source_annotator import SourceAnnotator
    from node_explanations import NodeExplanations
    
    # Find source files
    cpp_files = find_cpp_files(build_dir)
    if not cpp_files:
        return
    
    # Initialize components
    parser = ASTParser()
    annotator = SourceAnnotator()
    explanations = NodeExplanations()
    
    # Process each file
    results = []
    for cpp_file in cpp_files:
        print(f"Processing: {cpp_file}")
        
        ast_file = parser.generate_ast_json(cpp_file)
        if ast_file:
            line_mappings = parser.parse_ast_file(ast_file, cpp_file)
            if line_mappings:
                stats = parser.get_statistics()
                results.append({
                    'file': cpp_file,
                    'stats': stats,
                    'line_mappings': line_mappings
                })
    
    # Generate summary report
    generate_summary_report(results, output_dir)

def generate_summary_report(results, output_dir):
    """Generate a summary report of AST analysis."""
    os.makedirs(output_dir, exist_ok=True)
    
    total_files = len(results)
    total_lines = sum(r['stats']['total_lines_with_ast'] for r in results)
    total_nodes = sum(r['stats']['total_ast_nodes'] for r in results)
    
    # Generate HTML report
    html_content = f"""
    <html>
    <head><title>Project AST Analysis Report</title></head>
    <body>
        <h1>Project AST Analysis Report</h1>
        <h2>Summary</h2>
        <ul>
            <li>Total Files: {total_files}</li>
            <li>Total Lines with AST: {total_lines}</li>
            <li>Total AST Nodes: {total_nodes}</li>
            <li>Average Nodes per File: {total_nodes / total_files:.2f}</li>
        </ul>
        
        <h2>Per-File Analysis</h2>
        <table border="1">
            <tr>
                <th>File</th>
                <th>Lines with AST</th>
                <th>Total Nodes</th>
                <th>Avg Nodes/Line</th>
            </tr>
    """
    
    for result in results:
        stats = result['stats']
        html_content += f"""
            <tr>
                <td>{result['file']}</td>
                <td>{stats['total_lines_with_ast']}</td>
                <td>{stats['total_ast_nodes']}</td>
                <td>{stats['avg_nodes_per_line']:.2f}</td>
            </tr>
        """
    
    html_content += """
        </table>
    </body>
    </html>
    """
    
    with open(os.path.join(output_dir, "ast_report.html"), 'w') as f:
        f.write(html_content)
    
    print(f"✅ Summary report generated: {output_dir}/ast_report.html")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python build_integration.py <build_dir> <output_dir>")
        sys.exit(1)
    
    build_dir = sys.argv[1]
    output_dir = sys.argv[2]
    
    generate_project_report(build_dir, output_dir)
```

### Example 16: CI/CD Integration

```bash
#!/bin/bash
# ci_ast_check.sh - CI/CD script for AST analysis

set -e

echo "=== AST Analysis CI Check ==="

# Configuration
SOURCE_DIR="src"
OUTPUT_DIR="ast_analysis"
REPORT_FILE="ast_report.json"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Find all C++ files
find "$SOURCE_DIR" -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" | while read -r cpp_file; do
    echo "Analyzing: $cpp_file"
    
    # Generate AST analysis
    python ast_line_mapper.py "$cpp_file" --format json --output "$OUTPUT_DIR/$(basename "$cpp_file").json"
    
    # Check for critical issues (example: too many nodes per line)
    python -c "
import json
import sys

with open('$OUTPUT_DIR/$(basename "$cpp_file").json', 'r') as f:
    data = json.load(f)

line_mappings = data['line_mappings']
for line, nodes in line_mappings.items():
    if len(nodes) > 10:  # Threshold for complexity
        print(f'WARNING: Line {line} has {len(nodes)} AST nodes (high complexity)')
        
# Example: Check for specific patterns
problematic_patterns = ['BinaryOperator', 'UnaryOperator', 'ConditionalOperator']
for line, nodes in line_mappings.items():
    operator_count = sum(1 for node in nodes if node in problematic_patterns)
    if operator_count > 3:
        print(f'WARNING: Line {line} has {operator_count} operators (consider refactoring)')
"
done

echo "=== AST Analysis Complete ==="
```

## Troubleshooting Examples

### Example 17: Handling Missing Headers

```cpp
// problematic.cpp - file with missing headers
#include <iostream>  // May not be found
#include <vector>    // May not be found

int main() {
    std::vector<int> numbers = {1, 2, 3};
    std::cout << "Size: " << numbers.size() << std::endl;
    return 0;
}
```

**Solution 1: Use without standard library**
```cpp
// simple_version.cpp
int main() {
    int numbers[] = {1, 2, 3};
    int size = 3;
    // printf equivalent or custom output
    return 0;
}
```

**Solution 2: Specify include paths**
```bash
# For Windows with Visual Studio
python ast_line_mapper.py problematic.cpp 

# The tool will automatically try different include paths
```

### Example 18: Debugging AST Generation

```python
from ast_parser import ASTParser
import json

parser = ASTParser()
cpp_file = "examples/debug.cpp"

# Generate AST with debug info
ast_file = parser.generate_ast_json(cpp_file)

if ast_file:
    # Examine raw AST JSON
    with open(ast_file, 'r') as f:
        ast_data = json.load(f)
    
    print("=== Raw AST Structure ===")
    print(f"Root node kind: {ast_data.get('kind')}")
    print(f"Root node ID: {ast_data.get('id')}")
    
    # Show first few child nodes
    if 'inner' in ast_data:
        print(f"Child nodes: {len(ast_data['inner'])}")
        for i, child in enumerate(ast_data['inner'][:3]):
            print(f"  Child {i}: {child.get('kind')} at line {child.get('loc', {}).get('line', 'unknown')}")
```

These examples demonstrate the full capabilities of the Clang AST Line Mapper tool, from basic usage to advanced integration scenarios. The tool provides flexibility for different use cases while maintaining ease of use for simple scenarios.
