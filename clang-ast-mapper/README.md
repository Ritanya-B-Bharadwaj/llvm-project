# Clang AST Line Mapper

A powerful CLI tool that maps C++ source code lines to their corresponding AST (Abstract Syntax Tree) nodes using Clang's JSON AST dump functionality.

## 🚀 Features

- ✅ **AST Generation**: Automatically generates AST JSON from C++ source files
- ✅ **Line Mapping**: Maps each source line to its corresponding AST nodes
- ✅ **Human-Readable Explanations**: Converts technical AST nodes to understandable descriptions
- ✅ **Multiple Output Formats**: Annotated source, side-by-side view, table, CSV, JSON export
- ✅ **AI Interpretation**: Uses AI to analyze and explain your code's AST structure
- ✅ **Complex C++ Support**: Handles classes, methods, templates, loops, conditionals
- ✅ **CLI Interface**: Easy-to-use command-line interface with multiple options
- ✅ **Extensible**: Modular design for easy extension and customization

## 📋 Requirements

- Python 3.6+
- Clang compiler in PATH
- For Windows: Run from Developer Command Prompt or Visual Studio Developer PowerShell

## 🛠️ Installation

1. Clone or download the project
2. Navigate to the project directory:
   ```bash
   cd clang-ast-mapper
   ```
3. Install dependencies (if any):
   ```bash
   pip install -r requirements.txt
   ```

## 🎯 Quick Start

### Basic Usage
```bash
python src/ast_line_mapper.py examples/simple.cpp
```

### With Explanations
```bash
python src/ast_line_mapper.py examples/simple.cpp --explanations
```

### Side-by-Side View
```bash
python src/ast_line_mapper.py examples/complex.cpp --side-by-side
```

### Generate AST JSON Only
```bash
python src/ast_line_mapper.py examples/simple.cpp --generate-ast
```

### Output as CSV with AI Interpretation
```bash
python src/ast_line_mapper.py examples/simple.cpp --format csv --output result.csv --interpret
```

For more details on AI interpretation, see [AI Interpretation Documentation](docs/ai_interpretation.md).

## 📖 Usage Examples

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

**Output:**
```
 1: int add(int a, int b) {
    AST: FunctionDecl, ParmVarDecl
         → FunctionDecl: function declaration
         → ParmVarDecl: parameter variable declaration
 2:     return a + b;
    AST: ReturnStmt
         → ReturnStmt: return statement
```

### Example 2: Class with Methods
**Input (`examples/class.cpp`):**
```cpp
class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }
};
```

**Output:**
```
 1: class Calculator {
    AST: CXXRecordDecl
         → CXXRecordDecl: C++ class/struct declaration
 3:     int add(int a, int b) {
    AST: CXXMethodDecl, ParmVarDecl
         → CXXMethodDecl: C++ method declaration
         → ParmVarDecl: parameter variable declaration
```

## 🔧 Command Line Options

| Option | Description |
|--------|-------------|
| `--explanations` | Include human-readable explanations for AST nodes |
| `--side-by-side` | Display source and AST information side by side |
| `--generate-ast` | Generate AST JSON file only (don't annotate) |
| `--output FILE` | Save output to specified file |
| `--format FORMAT` | Output format: `annotated`, `side-by-side`, `json` |
| `--help` | Show help message |

## 📁 Project Structure

```
clang-ast-mapper/
├── src/
│   ├── ast_line_mapper.py      # Main CLI tool
│   ├── ast_parser.py           # AST parsing logic
│   ├── source_annotator.py     # Source code annotation
│   └── node_explanations.py    # AST node explanations
├── examples/
│   ├── simple.cpp              # Simple function example
│   ├── class.cpp               # Class example
│   ├── complex.cpp             # Complex C++ features
│   ├── templates.cpp           # Template examples
│   └── control_flow.cpp        # Control flow examples
├── tests/
│   ├── test_ast_parser.py      # Unit tests for AST parser
│   ├── test_annotator.py       # Unit tests for annotator
│   └── test_integration.py     # Integration tests
├── docs/
│   ├── API.md                  # API documentation
│   ├── ARCHITECTURE.md         # Architecture overview
│   └── EXAMPLES.md             # Detailed examples
├── README.md                   # This file
├── requirements.txt            # Python dependencies
├── setup.py                    # Installation script
└── LICENSE                     # License file
```

## 🔍 AST Node Types

The tool recognizes and explains various AST node types:

| AST Node Type | Explanation |
|---------------|-------------|
| `FunctionDecl` | Function declaration |
| `CXXMethodDecl` | C++ method declaration |
| `CXXRecordDecl` | C++ class/struct declaration |
| `CXXConstructorDecl` | C++ constructor declaration |
| `CXXDestructorDecl` | C++ destructor declaration |
| `ParmVarDecl` | Parameter variable declaration |
| `VarDecl` | Variable declaration |
| `FieldDecl` | Field declaration |
| `CompoundStmt` | Compound statement (block) |
| `ReturnStmt` | Return statement |
| `IfStmt` | If statement |
| `ForStmt` | For loop statement |
| `WhileStmt` | While loop statement |
| `BinaryOperator` | Binary operator (+, -, *, /) |
| `UnaryOperator` | Unary operator (++, --, !) |
| `CallExpr` | Function call expression |
| `DeclRefExpr` | Reference to a declaration |
| `IntegerLiteral` | Integer literal |
| `StringLiteral` | String literal |

## 🧪 Testing

Run the test suite:
```bash
python -m pytest tests/
```

Run specific tests:
```bash
python -m pytest tests/test_ast_parser.py
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built on top of LLVM/Clang's AST dumping capabilities
- Inspired by the need for better C++ code analysis tools
- Thanks to the LLVM community for their excellent documentation

## 📞 Support

If you encounter any issues or have questions:
1. Check the [documentation](docs/)
2. Look at the [examples](examples/)
3. Create an issue on the project repository

---

**Happy AST Mapping! 🎉**
