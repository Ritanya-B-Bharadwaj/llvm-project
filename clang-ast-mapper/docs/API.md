# API Documentation

## Overview

The Clang AST Line Mapper provides a Python API for mapping C++ source lines to their corresponding AST nodes. The API consists of three main modules:

## Modules

### ast_parser.py

The core module for parsing Clang AST JSON output.

#### ASTParser Class

```python
class ASTParser:
    def __init__(self)
    def generate_ast_json(self, cpp_file) -> str
    def parse_ast_file(self, ast_file, cpp_file) -> dict
    def get_statistics(self) -> dict
```

**Methods:**

- `generate_ast_json(cpp_file)`: Generates AST JSON using Clang
  - **Parameters:** `cpp_file` (str) - Path to C++ source file
  - **Returns:** Path to generated AST JSON file or None if failed

- `parse_ast_file(ast_file, cpp_file)`: Parses AST JSON and creates line mappings
  - **Parameters:** 
    - `ast_file` (str) - Path to AST JSON file
    - `cpp_file` (str) - Path to original C++ file
  - **Returns:** Dictionary mapping line numbers to AST node types

- `get_statistics()`: Gets statistics about parsed AST
  - **Returns:** Dictionary with parsing statistics

**Example:**

```python
from ast_parser import ASTParser

parser = ASTParser()
ast_file = parser.generate_ast_json("example.cpp")
if ast_file:
    line_mappings = parser.parse_ast_file(ast_file, "example.cpp")
    stats = parser.get_statistics()
```

### source_annotator.py

Module for annotating source code with AST information.

#### SourceAnnotator Class

```python
class SourceAnnotator:
    def __init__(self)
    def annotate_source(self, cpp_file, line_mappings, include_explanations=False, explanations_dict=None) -> str
    def side_by_side_view(self, cpp_file, line_mappings, explanations_dict=None) -> str
    def generate_html_output(self, cpp_file, line_mappings, explanations_dict=None) -> str
    def generate_markdown_output(self, cpp_file, line_mappings, explanations_dict=None) -> str
```

**Methods:**

- `annotate_source(cpp_file, line_mappings, include_explanations=False, explanations_dict=None)`: Annotates source with AST info
  - **Parameters:**
    - `cpp_file` (str) - Path to C++ source file
    - `line_mappings` (dict) - Line number to AST nodes mapping
    - `include_explanations` (bool) - Whether to include explanations
    - `explanations_dict` (dict) - Dictionary of node explanations
  - **Returns:** Annotated source as string

- `side_by_side_view(cpp_file, line_mappings, explanations_dict=None)`: Generates side-by-side view
  - **Parameters:** Same as annotate_source
  - **Returns:** Side-by-side view as string

- `generate_html_output(cpp_file, line_mappings, explanations_dict=None)`: Generates HTML output
  - **Parameters:** Same as annotate_source
  - **Returns:** HTML output as string

- `generate_markdown_output(cpp_file, line_mappings, explanations_dict=None)`: Generates Markdown output
  - **Parameters:** Same as annotate_source
  - **Returns:** Markdown output as string

**Example:**

```python
from source_annotator import SourceAnnotator

annotator = SourceAnnotator()
annotated = annotator.annotate_source(
    "example.cpp", 
    line_mappings, 
    include_explanations=True,
    explanations_dict=explanations
)
```

### node_explanations.py

Module for providing human-readable explanations of AST nodes.

#### NodeExplanations Class

```python
class NodeExplanations:
    def __init__(self)
    def get_explanation(self, node_kind) -> str
    def get_all_explanations(self) -> dict
    def add_explanation(self, node_kind, explanation)
    def get_categories(self) -> dict
```

**Methods:**

- `get_explanation(node_kind)`: Gets explanation for a specific node type
  - **Parameters:** `node_kind` (str) - AST node type
  - **Returns:** Human-readable explanation

- `get_all_explanations()`: Gets all explanations
  - **Returns:** Dictionary of all node explanations

- `add_explanation(node_kind, explanation)`: Adds or updates explanation
  - **Parameters:**
    - `node_kind` (str) - AST node type
    - `explanation` (str) - Human-readable explanation

- `get_categories()`: Gets explanations grouped by categories
  - **Returns:** Dictionary with categorized explanations

**Example:**

```python
from node_explanations import NodeExplanations

explanations = NodeExplanations()
explanation = explanations.get_explanation("FunctionDecl")
all_explanations = explanations.get_all_explanations()
```

## Data Structures

### Line Mappings

Line mappings are dictionaries that map line numbers (int) to lists of AST node types (list of str):

```python
line_mappings = {
    1: ['FunctionDecl', 'ParmVarDecl'],
    2: ['ReturnStmt'],
    5: ['FunctionDecl'],
    6: ['DeclStmt', 'CallExpr'],
    7: ['ReturnStmt']
}
```

### Statistics

Statistics are dictionaries containing parsing information:

```python
statistics = {
    'total_lines_with_ast': 10,
    'total_ast_nodes': 25,
    'avg_nodes_per_line': 2.5,
    'node_type_counts': {
        'FunctionDecl': 3,
        'ReturnStmt': 2,
        'DeclStmt': 5
    },
    'most_common_nodes': [
        ('DeclStmt', 5),
        ('FunctionDecl', 3),
        ('ReturnStmt', 2)
    ]
}
```

## Error Handling

All methods handle errors gracefully:

- File not found errors return appropriate error messages
- Invalid JSON returns None or error strings
- Missing Clang installation is detected and reported
- Timeout errors are handled for long-running operations

## Integration Example

Complete example of using the API:

```python
from ast_parser import ASTParser
from source_annotator import SourceAnnotator
from node_explanations import NodeExplanations

# Initialize components
parser = ASTParser()
annotator = SourceAnnotator()
explanations = NodeExplanations()

# Process file
cpp_file = "example.cpp"
ast_file = parser.generate_ast_json(cpp_file)

if ast_file:
    # Parse AST
    line_mappings = parser.parse_ast_file(ast_file, cpp_file)
    
    # Get statistics
    stats = parser.get_statistics()
    print(f"Processed {stats['total_lines_with_ast']} lines with AST")
    
    # Annotate source
    annotated = annotator.annotate_source(
        cpp_file,
        line_mappings,
        include_explanations=True,
        explanations_dict=explanations.get_all_explanations()
    )
    
    print(annotated)
    
    # Generate HTML output
    html_output = annotator.generate_html_output(
        cpp_file,
        line_mappings,
        explanations_dict=explanations.get_all_explanations()
    )
    
    with open("output.html", "w") as f:
        f.write(html_output)
```

## Thread Safety

The API is designed to be thread-safe for read operations. However, for write operations (like generating AST files), it's recommended to use appropriate locking mechanisms in multi-threaded environments.

## Performance Considerations

- AST generation is the slowest operation (depends on Clang compilation)
- JSON parsing is relatively fast
- Line mapping creation is O(n) where n is the number of AST nodes
- Source annotation is O(m) where m is the number of source lines

## Extension Points

The API can be extended by:

1. Adding new output formats in `SourceAnnotator`
2. Adding new node explanations in `NodeExplanations`
3. Adding new parsing strategies in `ASTParser`
4. Creating new analysis tools using the line mappings
