# LLVM Computational Intensity Analysis

This project implements an **LLVM Function Pass** that analyzes a given function to compute its **computational intensity**, defined as the ratio of arithmetic operations (e.g., `add`, `mul`, `sin`, `cos`, etc.) to memory operations (`load` and `store`).

## Overview

The pass helps developers understand whether a function is **compute-heavy** or **memory-heavy**. This is useful in performance-critical systems such as high-performance computing (HPC), embedded systems, or compiler research.

### What It Does

- Counts arithmetic operations:
  - LLVM `BinaryOperator` instructions (e.g., `add`, `mul`, etc.)
  - Math function calls like `sin`, `cos`, `exp`, `sqrt`
- Counts memory operations:
  - `load` and `store` instructions
- Computes ratio = `arithmeticOps / memoryOps`
- Reports if the ratio > 2.0 as **high computational intensity**

## Building the Project
### Prerequisites
- LLVM 15 (ensure clang-15, opt, llvm-config-15 are available)
- C++ compiler (clang++-15)

### Step-by-Step Build
***This command compiles ComputationalIntensityPass.cpp into a shared object (.so) file that can be loaded by opt:***
```bash
clang++-15 -fPIC -shared -o ComputationalIntensityPass.so ../ComputationalIntensityPass.cpp \
`llvm-config-15 --cxxflags --ldflags --system-libs --libs core passes analysis support`
```
- **-fPIC**: Generates position-independent code, required for shared libraries.
- **-shared**: Builds a shared object .so file.
- **llvm-config**-15: Supplies correct flags and libraries for LLVM 15.
- **Output**: ComputationalIntensityPass.so, which can be used with opt to run the pass.



***This command compiles a C source file (test.c) into LLVM IR (.ll) using clang:***
```bash
clang -O3 -S -emit-llvm ../tests/<testFile.c> -o ../tests/<testFile.c>.ll
```
- **-S**: Emit LLVM assembly (human-readable .ll file).
- **-emit-llvm**: Generate LLVM Intermediate Representation instead of a native binary.
- **-O3**: Apply high-level optimizations.
- **Output**: test.ll, the optimized LLVM IR file used for analysis by the custom LLVM pass


***This command runs the custom LLVM function pass on the generated .ll file:***
```bash
/usr/lib/llvm-15/bin/opt -load-pass-plugin ./ComputationalIntensityPass.so \
-passes='function(analyze-computational-intensity)' \
-disable-output ../tests/<testFile.c>.ll
```
- **-load-pass-plugin**: Loads your custom LLVM pass plugin (.so file).
- **-passes='function(comp-intensity)'**: Runs the pass named comp-intensity on each function.
- **-disable-output**: Suppresses writing transformed output; only performs analysis.
- **Input**: LLVM IR file (test.ll).
- **Output**: Diagnostic messages printed to the terminal showing computational intensity per function.

## Results and Outcomes

### Example Output:
```bash
>>> Pipeline callback called with pass name: analyze-computational-intensity
>>> Match found! Adding ComputationalIntensityPass
Analyzing Function: compute
- Arithmetic ops: 30
- Memory ops: 30
- Ratio: 1.000000e+00
```

If the ratio exceeds 2.0, it will additionally print:
```bash
Function 'compute' has high computational intensity:
```


