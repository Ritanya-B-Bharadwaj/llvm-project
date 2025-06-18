# OpenMP-to-IR Construct Annotator

## Overview

The **OpenMP-to-IR Construct Annotator** is a custom LLVM pass and CLI tool that detects OpenMP constructs in C/C++ source code and annotates the corresponding LLVM Intermediate Representation (IR) with metadata. This aids in compiler analysis, optimization, and tooling for parallelism-aware programs.

---

## Features

* Annotates OpenMP-related IR instructions using `!omp.annotation` metadata.
* Supports runtime calls such as `__kmpc_fork_call`, `omp_get_thread_num`, etc.
* CLI tool `omp-annotate` performs full pipeline:

  * Compiles `.cpp` to `.ll`
  * Runs the annotator pass
  * Outputs annotated IR
* Optional GenAI module to explain annotated OpenMP IR using LLMs (e.g., LLaMA3)

---

## Installation

### Prerequisites

* CMake
* Ninja
* Python 3
* LLVM with Clang and OpenMP support
* MinGW (for Windows)

### Clone and Build LLVM

```bash
git clone https://github.com/Ritanya-B-Bharadwaj/llvm-project.git
cd llvm-project
mkdir build && cd build
cmake -DLLVM_ENABLE_PROJECTS="clang;openmp" -G Ninja ../llvm
ninja clang
```

### Add the Pass

1. Create: `llvm/lib/Transforms/OpenMPAnnotator/OpenMPAnnotatorPass.cpp`
2. Create: `llvm/include/llvm/Transforms/OpenMPAnnotator/OpenMPAnnotatorPass.h`
3. Register the pass in:

   * `llvm/lib/Passes/PassBuilder.cpp`
   * `llvm/lib/Passes/CMakeLists.txt`
   * `llvm/tools/opt/CMakeLists.txt`
4. Rebuild:

```bash
cd build
ninja opt
```

---

## CLI Tool: `omp-annotate`

### Add CLI Source

1. File: `llvm/tools/omp-annotate/omp-annotate.cpp`
2. Register tool in `llvm/tools/CMakeLists.txt`:

```cmake
add_llvm_tool_subdirectory(omp-annotate)
```

3. Create `llvm/tools/omp-annotate/CMakeLists.txt`:

```cmake
add_llvm_tool(omp-annotate
  omp-annotate.cpp
)

target_link_libraries(omp-annotate PRIVATE
  LLVMCore
  LLVMSupport
  LLVMIRReader
  LLVMPasses
)
```

4. Register the pass via:

```cpp
void registerOpenMPAnnotatorPipeline(PassBuilder &PB) {
  PB.registerPipelineParsingCallback(
    [](StringRef Name, FunctionPassManager &FPM,
       ArrayRef<PassBuilder::PipelineElement>) {
      if (Name == "openmp-annotator") {
        FPM.addPass(OpenMPAnnotatorPass());
        return true;
      }
      return false;
    });
}
```

### Rebuild

```bash
cd build
ninja omp-annotate
```

---

## Usage

### Basic Command

```bash
./bin/omp-annotate <input.cpp> -o <output.ll>
```

This does the following:

1. Compiles input `.cpp` file to `.ll` IR
2. Runs `OpenMPAnnotatorPass`
3. Outputs annotated `.ll` file

### Example

```bash
./bin/omp-annotate ../test-scripts/omp_test.cpp -o ../test-scripts/omp_test.ll
```

Output includes:

This IR instruction represents a call to the OpenMP runtime function omp_get_thread_num(), which returns the ID of the current thread within a parallel region. The second part, !omp.annotation !6, attaches custom metadata to this instruction.
The metadata:

```llvm
call i32 @omp_get_thread_num(), !omp.annotation !6
!6 = !{!"omp.get_thread_num"}
```
indicates that this call has been annotated by the OpenMPAnnotatorPass with the label "omp.get_thread_num". This annotation helps downstream tools or compiler passes recognize that this instruction is part of an OpenMP runtime operation related to thread identification, which is critical for analyzing or transforming parallel regions in the IR.

---

## GenAI IR Explainer (Optional)

### Script

* Path: `genai-tools/genai_openmp_ir_explainer.py`
* Input: `.cpp`, `.ll` file
* Output: `.json` with LLM-generated explanations

### Run

```bash
python genai_openmp_ir_explainer.py <input.cpp> <annotated.ll> <output.json>
```

### CLI Integration

`omp-annotate` also invokes GenAI script after annotation:

```bash
✔ Annotated IR written to <output.ll>
✔ Explanation written to <output_explanations.json>
```

### Sample Output (JSON)

```json
[
  {
    "directive": "parallel for",
    "description": "This OpenMP directive tells the compiler to execute a loop in parallel across multiple threads.",
    "ir_translation": "Lowered to __kmpc_fork_call, which sets up parallel regions by spawning threads.",
    "notes": "Each thread executes the loop body independently. Thread ID is accessed using omp_get_thread_num.",
    "ir_snippet": "call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @6, i32 0, ptr @_Z3foov.omp_outlined), !omp.annotation !6"
  }
]
```

---

## Supported Annotations

| Runtime Call                 | Annotation              |
| ---------------------------- | ----------------------- |
| `__kmpc_fork_call`           | `omp.parallel`          |
| `__kmpc_for_static_init`     | `omp.for`               |
| `__kmpc_critical`            | `omp.critical`          |
| `__kmpc_end_critical`        | `omp.critical.end`      |
| `__kmpc_barrier`             | `omp.barrier`           |
| `__kmpc_master`              | `omp.master`            |
| `__kmpc_single`              | `omp.single`            |
| `omp_get_thread_num`         | `omp.get_thread_num`    |
| `omp_get_num_threads`        | `omp.get_num_threads`   |
| others with `__kmpc_`/`omp_` | `omp.runtime` (default) |

---

## Directory Structure (Summary)

```
llvm-project/
├── llvm/
│   ├── include/llvm/Transforms/OpenMPAnnotator/
│   │   └── OpenMPAnnotatorPass.h
│   └── lib/Transforms/OpenMPAnnotator/
│       └── OpenMPAnnotatorPass.cpp
├── tools/omp-annotate/
│   └── omp-annotate.cpp
├── genai-tools/
│   └── genai_openmp_ir_explainer.py
├── test-scripts/
│   ├── omp_test.cpp
│   ├── omp_test.ll
│   └── omp_test_explanations.json
├── build/
│   └── bin/
│        ├── omp-annotate.exe
│        ├── clnag++.exe
│        └── opt.exe
```

---

## Conclusion

The `OpenMP-to-IR Construct Annotator` tool transforms OpenMP-based C++ code into LLVM IR annotated with metadata, enabling introspection of parallel constructs for further analysis, research, or tooling.

Command:

```bash
./bin/omp-annotate <input.cpp> -o <output.ll>
```

Optional GenAI JSON explanation also generated alongside IR output.

---

## Authors  
Aneesh Sai Grandhi<br>
Bolla Sai Naga Yaswanth<br>
Manasvini Padmasali

Based on: [Ritanya-B-Bharadwaj/llvm-project](https://github.com/Ritanya-B-Bharadwaj/llvm-project)
