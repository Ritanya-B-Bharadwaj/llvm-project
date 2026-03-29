# LLVM Source Mapper Examples

This directory contains example C++ files that demonstrate various language constructs and how they map to LLVM IR using the llvm-source-mapper tool.

## Examples

### 1. arithmetic.cpp
Basic arithmetic operations including:
- Variable declarations and initialization
- Addition, subtraction, multiplication
- Return statements

**Run with:**
```bash
../llvm-source-mapper.sh arithmetic.cpp --format=html
```

### 2. control_flow.cpp
Conditional statements and branching:
- if-else statements
- Nested conditions
- Branch instructions in LLVM IR

**Run with:**
```bash
../llvm-source-mapper.sh control_flow.cpp --ai-summaries
```

### 3. loops.cpp
Different loop constructs:
- For loops
- While loops
- Loop optimization patterns

**Run with:**
```bash
../llvm-source-mapper.sh loops.cpp --format=md
```

### 4. arrays.cpp
Array and memory operations:
- Array declarations and initialization
- Array indexing
- Pointer arithmetic
- Memory load/store operations

**Run with:**
```bash
../llvm-source-mapper.sh arrays.cpp --ai-summaries --format=html
```

## Usage Tips

1. **HTML Output**: Best for interactive viewing and sharing
   ```bash
   ../llvm-source-mapper.sh example.cpp --format=html
   ```

2. **Markdown Output**: Good for documentation and reports
   ```bash
   ../llvm-source-mapper.sh example.cpp --format=md
   ```

3. **With AI Summaries**: Adds explanations of LLVM IR instructions
   ```bash
   ../llvm-source-mapper.sh example.cpp --ai-summaries
   ```

4. **Annotated LLVM IR**: For compiler developers
   ```bash
   ../llvm-source-mapper.sh example.cpp --format=ll
   ```

## Understanding the Output

### Source Line Mapping
Each line of C++ source code is mapped to the corresponding LLVM IR instructions that are generated for that line.

### LLVM IR Instructions
Common instruction types you'll see:
- `alloca`: Stack allocation
- `store`: Store value to memory
- `load`: Load value from memory
- `add`, `sub`, `mul`: Arithmetic operations
- `icmp`: Integer comparison
- `br`: Branch (conditional or unconditional)
- `ret`: Return from function

### Debug Information
The mappings use LLVM debug metadata (`!dbg !xxx`) to establish the connection between source and IR. This is why the tool requires debug information to be present in the compiled IR.

## Compiling Examples Manually

If you want to see the raw LLVM IR for comparison:

```bash
# Generate LLVM IR with debug info
clang++ -emit-llvm -g -S -o example.ll example.cpp

# View the IR
cat example.ll
```

The llvm-source-mapper tool automates this process and creates the mapping for you.
