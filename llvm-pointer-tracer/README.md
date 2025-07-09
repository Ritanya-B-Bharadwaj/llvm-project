# LLVM Pointer Tracer Pass

## Overview

This project implements an LLVM Function pass that instruments C/C++ code to track and log pointer accesses at runtime. The pass identifies all pointer-related operations (loads, stores, GEP instructions) and adds instrumentation to print function names and pointer addresses in CSV format during program execution.

## Issue Reference

Solves issue #26: https://github.com/Ritanya-B-Bharadwaj/llvm-project/issues/26

## Features

- **Function-level instrumentation**: Tracks pointer accesses per function
- **Comprehensive pointer detection**: Identifies LoadInst, StoreInst, and GetElementPtrInst operations
- **CSV output format**: Runtime output in clean CSV format for easy analysis
- **Runtime pointer tracking**: Captures actual memory addresses during program execution
- **LLVM 21 compatibility**: Built and tested with LLVM 21.0.0git

## How It Works

The pass performs the following operations:

1. **Function Entry Instrumentation**: At the beginning of each function, adds code to print the function name
2. **Pointer Access Detection**: Traverses the IR to identify instructions that involve pointer operations:
   - `LoadInst`: Loading values from memory addresses
   - `StoreInst`: Storing values to memory addresses  
   - `GetElementPtrInst`: Computing pointer addresses for array/struct access
3. **Runtime Instrumentation**: Inserts `printf` calls to log pointer addresses as they are accessed

## Expected Output Format

When the instrumented program runs, it produces output like:
```
foo(), 0x7ffd9523a4f0, 0x7ffd9523a4f8
bar(), 0x7ffd9523a4f0, 0x7ffd9523a4a0, 0x7ffd9523aaa4
main(), 0x7ffe83ffea8c
```

Each line represents:
- Function name followed by parentheses and comma
- All pointer addresses accessed within that function, separated by commas

## Prerequisites

- LLVM 21.0.0 or compatible version
- CMake 3.13 or higher
- C++ compiler with C++17 support
- Linux/Unix environment (tested on Ubuntu/Debian) WSL

## Directory Structure

```
llvm-pointer-tracer/
├── pass/
│   └── PointerTracer.cpp      # Main pass implementation
├── test/
│   └── test.c                 # Sample test program           
├── build_pass_test.sh         # Build and test script
├── test.sh                    # Alternative run script
├── README.md                  # This file
```

## Steps to Build and Run

### 1. Build the Pass

Run the build script to compile the pass and test it:

```bash
bash build_pass_test.sh
```

This script will:
- Build the `pass/PointerTracer.cpp` file into a shared library
- Create the function pass plugin
- Generate LLVM IR from the test program in `test/test.c`
- Apply the pass to instrument the code
- Compile and run the instrumented program

### 2. Alternative Run Script

You can also use the alternative run script once the PointerTracerPass.so is built:

```bash
bash test.sh
```

## Sample Test Program

The repository includes a sample `test/test.c` file that demonstrates various pointer operations:

```c
#include <stdio.h>

void foo() {
    int x = 10;
    int y = 20;
    int *ptr1 = &x;
    int *ptr2 = &y;
    *ptr1 = *ptr2;  // Pointer load and store operations
}

void bar() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;
    for (int i = 0; i < 5; i++) {
        printf("%d ", *(ptr + i));  // Array access via pointer arithmetic
    }
}

int main() {
    int local_var = 42;
    int *main_ptr = &local_var;
    foo();
    bar();
    return 0;
}
```

## Understanding the Output

The instrumented program will output something like:
```
main(), 0x7ffe83ffea8c,
foo(), 0x7ffe83ffea6c, 0x7ffe83ffea68, 0x7ffe83ffea60, 0x7ffe83ffea58, 0x7ffe83ffea60, 0x7ffe83ffea6c,
bar(), 0x7ffe83ffea50, 0x7ffe83ffea48, 0x7ffe83ffea44, 0x7ffe83ffea44, 0x7ffe83ffea48,
```

This shows:
- `main()` accessed 1 pointer address
- `foo()` accessed 6 pointer addresses (including repeated accesses)
- `bar()` accessed 5 pointer addresses during array iteration

## Implementation Details

The pass is implemented as a modern LLVM pass using the new Pass Manager:

- **Pass Type**: `FunctionPass` - operates on individual functions
- **Instrumentation Strategy**: Inserts `printf` calls before pointer operations
- **Pointer Detection**: Uses LLVM's instruction visitor pattern to identify relevant instructions
- **Format Strings**: Creates global constant strings for printf formatting

### Key Components

1. **PointerTracerPass Class**: Main pass implementation in `pass/PointerTracer.cpp`
2. **Function Entry Instrumentation**: Adds function name logging
3. **Instruction Visitor**: Traverses IR to find pointer operations
4. **Printf Injection**: Inserts runtime logging calls

## Debugging and Troubleshooting

### Common Issues

1. **No Output**: Check if instrumentation was properly added:
   ```bash
   grep 'printf' test_instrumented.ll
   ```

2. **Missing Libraries**: Verify all dependencies are linked:
   ```bash
   ldd test_instrumented
   ```

3. **LLVM Version Mismatch**: Ensure you're using LLVM 21 or compatible version:
   ```bash
   llvm-config --version
   ```

### Debugging Steps

1. **Check IR Generation**: Verify LLVM IR is generated correctly
2. **Verify Pass Loading**: Ensure the pass plugin loads without errors
3. **Inspect Instrumented IR**: Look for printf calls in the output IR

## Customization

### Modifying Output Format

To change the output format, modify the format strings in `pass/PointerTracer.cpp`:

```cpp
// Current format: "function_name(), "
// Change to custom format as needed
```

### Adding More Instruction Types

To track additional instruction types, extend the visitor methods:

```cpp
// Add handlers for other instruction types
void visitCallInst(CallInst &CI) { /* Handle function calls */ }
void visitAllocaInst(AllocaInst &AI) { /* Handle stack allocations */ }
```

## Performance Considerations

- The instrumentation adds runtime overhead due to printf calls
- Each pointer access generates a system call for logging
- For performance-critical analysis, consider buffering output or using lighter logging mechanisms
- The pass preserves program semantics but increases execution time
