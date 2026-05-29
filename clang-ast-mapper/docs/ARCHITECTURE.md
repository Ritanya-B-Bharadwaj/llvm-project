# Architecture Overview

## System Design

The Clang AST Line Mapper is designed as a modular system that transforms C++ source code into annotated output showing the relationship between source lines and AST nodes.

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   C++ Source    │───▶│   Clang AST     │───▶│   AST JSON      │
│     File        │    │   Generator     │    │     File        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Annotated     │◀───│   Source        │◀───│   AST Parser    │
│    Output       │    │   Annotator     │    │   & Mapper      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                ▲                       ▲
                                │                       │
                       ┌─────────────────┐    ┌─────────────────┐
                       │     Node        │    │   Line-to-Node  │
                       │ Explanations    │    │    Mappings     │
                       └─────────────────┘    └─────────────────┘
```

## Core Components

### 1. AST Parser (`ast_parser.py`)

**Responsibilities:**
- Generate AST JSON from C++ source using Clang
- Parse AST JSON and extract relevant information
- Create line-to-node mappings
- Handle different Clang configurations and environments

**Key Methods:**
- `generate_ast_json()`: Invokes Clang with appropriate flags
- `parse_ast_file()`: Processes JSON and creates mappings
- `_collect_nodes_by_line()`: Recursively traverses AST nodes
- `_extract_line_number()`: Extracts line information from AST nodes

**Data Flow:**
```
C++ Source → Clang Process → AST JSON → JSON Parser → Line Mappings
```

### 2. Source Annotator (`source_annotator.py`)

**Responsibilities:**
- Read source files and combine with AST information
- Generate multiple output formats (annotated, side-by-side, HTML, Markdown)
- Handle formatting and presentation logic
- Provide extensible output generation

**Key Methods:**
- `annotate_source()`: Creates inline annotations
- `side_by_side_view()`: Creates columnar output
- `generate_html_output()`: Creates styled HTML
- `generate_markdown_output()`: Creates Markdown format

**Data Flow:**
```
Source Lines + Line Mappings + Explanations → Formatted Output
```

### 3. Node Explanations (`node_explanations.py`)

**Responsibilities:**
- Provide human-readable explanations for AST node types
- Maintain comprehensive database of node descriptions
- Support categorization and organization of explanations
- Allow extension and customization

**Key Methods:**
- `get_explanation()`: Retrieves explanation for specific node
- `get_all_explanations()`: Returns complete explanation dictionary
- `get_categories()`: Groups explanations by type

**Data Structure:**
```python
explanations = {
    'FunctionDecl': 'function declaration',
    'ReturnStmt': 'return statement',
    'BinaryOperator': 'binary operator (e.g., +, -, *, /)',
    # ... hundreds of other mappings
}
```

### 4. CLI Interface (`ast_line_mapper.py`)

**Responsibilities:**
- Command-line argument parsing
- Coordinate between components
- Handle user interactions and error reporting
- Provide extensible command structure

**Key Methods:**
- `main()`: Entry point and argument parsing
- `process_file()`: Main processing pipeline
- `_output_*()`: Format-specific output handlers

## Design Patterns

### 1. Strategy Pattern
The `SourceAnnotator` uses strategy pattern for different output formats:
- `annotate_source()` - Inline annotation strategy
- `side_by_side_view()` - Columnar display strategy
- `generate_html_output()` - HTML generation strategy
- `generate_markdown_output()` - Markdown generation strategy

### 2. Builder Pattern
The AST parsing process builds line mappings incrementally:
```python
def _collect_nodes_by_line(self, node, current_file):
    # Recursively build line mappings
    if self._is_from_main_file(loc, range_info, current_file):
        self.line_to_nodes[line].append(kind)
```

### 3. Factory Pattern
The `ASTParser` tries different Clang command configurations:
```python
clang_commands = [
    f'clang++ -Xclang -ast-dump=json -fsyntax-only "{cpp_file}"',
    f'clang -Xclang -ast-dump=json -fsyntax-only "{cpp_file}"',
    # ... additional variants
]
```

## Data Flow Architecture

### 1. Input Processing
```
User Input → Argument Parser → File Validation → AST Generation
```

### 2. AST Processing
```
AST JSON → JSON Parser → Node Traversal → Line Extraction → Mapping Creation
```

### 3. Output Generation
```
Line Mappings → Format Selection → Template Processing → Output Generation
```

## Error Handling Strategy

### 1. Graceful Degradation
- If Clang is not available, provide helpful setup instructions
- If AST generation fails, suggest alternative approaches
- If parsing fails, provide detailed error information

### 2. Layered Error Handling
```python
try:
    ast_file = self.parser.generate_ast_json(cpp_file)
    if not ast_file:
        return False  # Clang error already reported
    
    line_mappings = self.parser.parse_ast_file(ast_file, cpp_file)
    if not line_mappings:
        print("❌ Failed to parse AST file")
        return False
        
except Exception as e:
    print(f"❌ Unexpected error: {e}")
    return False
```

### 3. User-Friendly Messages
- Use emojis and clear formatting for status messages
- Provide actionable error messages with solutions
- Include setup help for common configuration issues

## Performance Considerations

### 1. Bottlenecks
- **AST Generation**: Clang compilation is the slowest operation
- **JSON Parsing**: Large AST files can be memory-intensive
- **Line Mapping**: O(n) traversal where n = number of AST nodes

### 2. Optimizations
- **Caching**: Generated AST files can be reused
- **Filtering**: Only process nodes from main file (not includes)
- **Streaming**: Process large files in chunks if needed

### 3. Memory Management
- Clear intermediate data structures after processing
- Use generators for large file processing
- Implement file cleanup for temporary files

## Extensibility Points

### 1. New Output Formats
Add new methods to `SourceAnnotator`:
```python
def generate_xml_output(self, cpp_file, line_mappings, explanations_dict=None):
    # Implementation for XML format
    pass
```

### 2. New AST Processors
Extend `ASTParser` for specialized processing:
```python
def analyze_complexity(self, line_mappings):
    # Analyze code complexity based on AST nodes
    pass
```

### 3. New Explanation Categories
Extend `NodeExplanations` with domain-specific explanations:
```python
def add_domain_explanations(self, domain):
    # Add explanations for specific domains (embedded, web, etc.)
    pass
```

### 4. New CLI Commands
Extend main CLI with subcommands:
```python
parser.add_subcommand('analyze', help='Analyze code complexity')
parser.add_subcommand('compare', help='Compare AST between files')
```

## Configuration Management

### 1. Environment Detection
```python
# Detect Windows vs. Linux/macOS
if os.name == 'nt':
    # Windows-specific Clang paths
    pass
else:
    # Unix-specific Clang paths
    pass
```

### 2. Clang Configuration
```python
# Try multiple Clang configurations
clang_configs = [
    {'cmd': 'clang++', 'flags': ['-std=c++17']},
    {'cmd': 'clang', 'flags': ['-std=c11']},
    # ... more configurations
]
```

### 3. User Preferences
Future extension could include configuration file support:
```python
# ~/.clang-ast-mapper.conf
[clang]
executable = /usr/local/bin/clang++
flags = -std=c++20 -Wall

[output]
default_format = side-by-side
include_explanations = true
```

## Testing Strategy

### 1. Unit Tests
- Test individual components in isolation
- Mock external dependencies (Clang, file system)
- Test error conditions and edge cases

### 2. Integration Tests
- Test complete workflows end-to-end
- Test with real Clang installation
- Test different file types and configurations

### 3. Performance Tests
- Measure processing time for different file sizes
- Test memory usage with large AST files
- Benchmark different output formats

## Security Considerations

### 1. Input Validation
- Validate file paths to prevent directory traversal
- Sanitize user input before passing to shell commands
- Limit file sizes to prevent resource exhaustion

### 2. Command Injection Prevention
```python
# Use subprocess with argument lists, not shell strings
subprocess.run(['clang++', '-Xclang', '-ast-dump=json', cpp_file])
```

### 3. Temporary File Handling
- Use secure temporary file creation
- Clean up temporary files after processing
- Handle permissions appropriately

## Future Enhancements

### 1. Language Support
- Support for C files (not just C++)
- Support for Objective-C and Objective-C++
- Support for other Clang-supported languages

### 2. Advanced Analysis
- Control flow analysis
- Data flow analysis
- Dependency analysis
- Code complexity metrics

### 3. Integration Features
- IDE plugins (VS Code, CLion, etc.)
- Build system integration (CMake, Make, etc.)
- Continuous integration support

### 4. Visualization
- Interactive HTML output with collapsible sections
- Graphical AST visualization
- Syntax highlighting in output
- Export to documentation formats
