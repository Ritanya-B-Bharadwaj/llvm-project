# LLVM Source Mapper

A tool for generating mappings between C++ source code and the corresponding LLVM IR instructions. This tool helps developers understand how their C++ code translates to LLVM IR, making it easier to optimize code and understand compiler behavior.

## Features

- **Source-to-IR Mapping**: Maps each line of C++ source code to its corresponding LLVM IR instructions
- **Multiple Output Formats**: Supports annotated LLVM IR, Markdown tables, and styled HTML
- **Debug Information Integration**: Uses LLVM debug metadata to create accurate mappings
- **AI-Powered Summaries**: Optional AI-generated explanations of LLVM IR instructions (requires API key)
- **Command-line Interface**: Easy-to-use CLI with flexible options

## Building

This tool is part of the LLVM project and is built automatically when LLVM is built with tools enabled.

```bash
# From LLVM build directory
cmake -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_BUILD_TOOLS=ON /path/to/llvm-project/llvm
make llvm-source-mapper
```

## Usage

### Basic Usage

```bash
# Generate HTML mapping (default)
llvm-source-mapper example.cpp

# Generate Markdown table
llvm-source-mapper example.cpp --format=md

# Generate annotated LLVM IR
llvm-source-mapper example.cpp --format=ll

# Specify output file
llvm-source-mapper example.cpp --format=html -o my_mapping.html
```

### Using the Wrapper Script

The included shell script provides additional features like AI summaries:

```bash
# Basic mapping
./llvm-source-mapper.sh example.cpp

# With AI summaries (requires GROQ_API_KEY)
./llvm-source-mapper.sh example.cpp --ai-summaries

# Custom format and output
./llvm-source-mapper.sh example.cpp --format=md --output=mapping.md
```

### AI Summaries

To enable AI-generated summaries of LLVM IR instructions:

1. Get a free API key from [Groq Cloud](https://console.groq.com/)
2. Set the environment variable:
   ```bash
   export GROQ_API_KEY="your_api_key_here"
   ```
3. Install Python dependencies:
   ```bash
   pip install requests python-dotenv
   ```
4. Use the AI feature:
   ```bash
   ./llvm-source-mapper.sh example.cpp --ai-summaries
   ```

## Output Formats

### Annotated LLVM IR (`--format=ll`)
```llvm
; Line 5: int x = 42;
  store i32 42, ptr %x, align 4, !dbg !123

; Line 6: return x + 1;
  %1 = load i32, ptr %x, align 4, !dbg !124
  %2 = add nsw i32 %1, 1, !dbg !125
  ret i32 %2, !dbg !126
```

### Markdown Table (`--format=md`)
| Line | Source Code | LLVM IR | Summary |
|------|-------------|---------|----------|
| 5 | `int x = 42;` | `store i32 42, ptr %x, align 4` | Store immediate value |
| 6 | `return x + 1;` | `%1 = load i32, ptr %x, align 4`<br>`%2 = add nsw i32 %1, 1` | Load and add operation |

### HTML (`--format=html`)
Generates a beautiful, responsive HTML table with syntax highlighting and modern styling.

## Command Line Options

- `--format=FORMAT`: Output format (`ll`, `md`, `html`)
- `-o FILE`: Output file path
- `--help`: Show help message

## Dependencies

- **LLVM/Clang**: Core functionality
- **Python 3**: For AI summaries (optional)
- **requests**: Python HTTP library (optional, for AI)
- **python-dotenv**: Environment variable loading (optional)

## Environment Variables

- `GROQ_API_KEY`: API key for AI summaries
- `GROQ_MODEL`: AI model to use (default: `llama-3.3-70b-versatile`)

## Examples

### Simple Function
```cpp
// example.cpp
int add(int a, int b) {
    return a + b;
}
```

```bash
llvm-source-mapper example.cpp --format=html
```

### Complex Example with Loops
```cpp
// fibonacci.cpp
int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}
```

```bash
./llvm-source-mapper.sh fibonacci.cpp --ai-summaries --format=html
```

## Troubleshooting

### Common Issues

1. **No mappings found**: Ensure your C++ code compiles correctly and has debug information
2. **AI summaries not working**: Check that `GROQ_API_KEY` is set and Python dependencies are installed
3. **Tool not found**: Make sure LLVM is built with tools enabled and the binary is in your PATH

### Debug Information

The tool relies on debug information in the LLVM IR. If mappings are missing:
- Ensure your code compiles without errors
- Check that debug information is being generated (the tool automatically adds `-g`)
- Verify that the source file path is correct

## Contributing

This tool is part of the LLVM project. Contributions should follow LLVM contribution guidelines:

1. Follow LLVM coding standards
2. Add tests for new features
3. Update documentation
4. Submit patches via Phabricator or GitHub

## License

This tool is part of the LLVM Project and is licensed under the Apache License v2.0 with LLVM Exceptions. See LICENSE.txt for details.

## Related Tools

- `llvm-dis`: Disassemble LLVM bitcode to human-readable IR
- `clang`: C/C++ compiler that generates LLVM IR
- `opt`: LLVM optimizer and analysis tool
- `llc`: LLVM static compiler
