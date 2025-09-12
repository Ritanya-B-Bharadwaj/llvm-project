# fp16AccuracyChecker

This Clang-based tool analyzes C++ programs using `float` operations and generates three transformed versions:

- A version using `float` (original)
- A version using `__fp16`
- A version using `__bf16` (or emulated if native support is unavailable)

It then compiles and executes all three versions, capturing their outputs and computing accuracy differences between `__fp16`/`__bf16` and the original `float`.

---

## Build Instructions

From the root of your LLVM repo:

```bash
cd llvm-project
mkdir build && cd build
cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release
ninja fp16AccuracyChecker

## Usage 
./clang-tools-extra/fp16AccuracyChecker/run_fp_accuracy.sh path/to/test.cpp


