# LLVM Computational Intensity Analysis

This project implements an **LLVM Function Pass** that analyzes a given function to compute its **computational intensity**, defined as the ratio of arithmetic operations (e.g., `add`, `mul`, `sin`, `cos`, etc.) to memory operations (`load` and `store`).

## Overview

The pass helps developers understand whether a function is **compute-heavy** or **memory-heavy**. This is useful in performance-critical systems such as high-performance computing (HPC), embedded systems, or compiler research.

### What It Does ?

- Counts arithmetic operations:
  - LLVM `BinaryOperator` instructions (e.g., `add`, `mul`, etc.)
  - Math function calls like `sin`, `cos`, `exp`, `sqrt`
- Counts memory operations:
  - `load` and `store` instructions
- Computes ratio = `arithmeticOps / memoryOps`
- Reports if the ratio > 2.0 as **high computational intensity**

## Building the Project
### Prerequisites
- LLVM 
- C++ compiler
- clang
- ninja build 

### Step-by-Step Build
***This cmake command is configuring the LLVM build system to build Clang and its supporting components using Ninja as the build system.***
```bash
cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS="clang" \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=../install
```
- **Ninja**: Path to the LLVM source directory. CMake reads the CMakeLists.txt here. 
- **-DLLVM_ENABLE_PROJECTS="clang"**: Tells LLVM to build Clang (the C/C++/Objective-C frontend).
- **-DLLVM_TARGETS_TO_BUILD="X86"**:Only build the X86 backend (code generation for x86_64). Speeds up compilation.

***this command builds the opt binary***
```bash
   ninja -j4 opt
```


***This command compiles a C source file (test.c) into LLVM IR (.ll) using clang:***
```bash
clang -O3 -S -emit-llvm ../tests/<testFile.c> -o ../tests/<testFile.c>.ll
```
- **-S**: Emit LLVM assembly (human-readable .ll file).
- **-emit-llvm**: Generate LLVM Intermediate Representation instead of a native binary.
- **-O3**: Apply high-level optimizations.



***This command runs the custom LLVM function pass on the generated .ll file:***
```bash
./build/bin/opt -passes=analyze-computational-intensity -disable-output < tests/clear_array.ll
```
- **-passes=analyze-computational-intensity**: Use the new pass manager to run your custom pass
- **-disable-output**: Suppresses writing transformed output; only performs analysis.
- **Input**: LLVM IR file (test.ll).
- **Output**: Diagnostic messages printed to the terminal showing computational intensity per function.

## Results and Outcomes

### Example Output:
```bash
══════════════════════════════════════════════════════════════
Analyzing Function: heavy_compute       
══════════════════════════════════════════════════════════════
Function 'heavy_compute'
has HIGH Computational Intensity  
══════════════════════════════════════════════════════════════
 Arithmetic Ops : 11       
 Memory Ops     : 1                                  
 Intensity Ratio: 1.100000e+01
══════════════════════════════════════════════════════════════
```

If the ratio exceeds 2.0, it will additionally print:
```bash
Function 'compute' has high computational intensity:
```


