# Clang MPI Analyser

![CI Status](https://img.shields.io/badge/CI-Passing-brightgreen)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Built with Clang](https://img.shields.io/badge/Built%20with-Clang-orange.svg)](https://clang.llvm.org/)

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Motivation](#motivation)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
- [Usage](#usage)
  - [Basic Usage](#basic-usage)
  - [Scatter/Gather Analysis](#scattergather-analysis)
  - [Understanding the Output](#understanding-the-output)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

## Introduction

The `clang-mpi-analyser` is a static analysis tool built on the LLVM/Clang infrastructure. It is designed to analyze C/C++ source code that uses the Message Passing Interface (MPI) library. Its primary goal is to identify common MPI usage patterns, particularly inefficient or non-idiomatic manual implementations of collective operations (like Scatter and Gather) that could be optimized by using native MPI collective functions.

By providing actionable suggestions, this tool aims to help developers improve the performance, scalability, and readability of their MPI applications.

## Features

- **MPI Function Call Detection:** Identifies and extracts information about various MPI functions (`MPI_Send`, `MPI_Recv`, `MPI_Sendrecv`, `MPI_Init`, `MPI_Finalize`, `MPI_Comm_rank`, `MPI_Comm_size`, etc.).
- **Contextual Analysis:** Determines if MPI calls are made within root-specific (`rank == root`) or non-root branches of control flow.
- **Loop Variable and Buffer Indexing Analysis:** Identifies when MPI communication arguments (like destination/source rank or buffer offsets) are tied to loop variables.
- **Manual Collective Pattern Detection:**
    - **Manual Scatter:** Detects patterns where the root process manually sends distinct data chunks to other processes in a loop, and non-root processes receive their specific chunks from the root.
    - **Manual Gather:** Detects patterns where non-root processes send their data to the root, and the root manually receives distinct data chunks from each process in a loop.
    - _(Optional: Add other patterns if you implement them, e.g., Manual Alltoall, Manual Broadcast)_
- **Detailed Reporting:** Generates comprehensive reports for detected patterns, including:
    - Pattern type (e.g., Manual Scatter).
    - Explanation of the issue.
    - Optimization suggestion (e.g., use `MPI_Scatter`).
    - Source code location (function name and line number).
    - Representative code snippet of the detected pattern.

## Motivation

MPI applications often achieve high performance through the use of optimized collective communication routines (e.g., `MPI_Scatter`, `MPI_Gather`). However, developers sometimes implement these operations manually using point-to-point communication (`MPI_Send`, `MPI_Recv`, `MPI_Sendrecv`). While functionally correct, these manual implementations can be:

- **Less Performant:** Native MPI collectives are highly optimized for specific network topologies and hardware, often outperforming naive manual implementations.
- **Less Scalable:** Manual point-to-point communication can lead to bottlenecks as the number of processes increases.
- **Less Readable:** Complex loops and `if/else` constructs for manual collectives make the code harder to understand and maintain.

This tool aims to automatically identify such opportunities for optimization, guiding developers towards more efficient and idiomatic MPI programming practices.

## Installation

This project is built as a Clang tool within the LLVM ecosystem. The standard way to build it is as part of `clang-tools-extra`.

### Prerequisites

* **Linux/macOS:** A modern C++ compiler (GCC 10+ or Clang 10+ recommended).
* **CMake:** Version 3.15 or higher.
* **Python:** For LLVM's build scripts.
* **LLVM & Clang Source Code:** You'll need a clone of the `llvm-project` repository. It is highly recommended to use a specific release branch (e.g., `llvmorg-18.0.0-rc2`) or a recent `main` branch.
* **Open MPI Development Headers:** The MPI library headers are required for Clang to understand MPI types and functions.

    ```bash
    # For Debian/Ubuntu
    sudo apt update
    sudo apt install build-essential cmake python3 python3-pip libopenmpi-dev openmpi-bin

    # For Fedora/RHEL
    sudo dnf install @development-tools cmake python3 python3-pip openmpi-devel
    ```

### Building the Project

1.  **Clone LLVM/Clang:** If you haven't already, clone the `llvm-project` repository:

    ```bash
    git clone [https://github.com/llvm/llvm-project.git](https://github.com/llvm/llvm-project.git)
    cd llvm-project
    # Optional: Checkout a specific release branch for stability
    # git checkout llvmorg-18.1.0
    ```

2.  **Place `clang-mpi-analyser`:** Copy your `clang-mpi-analyser` project directory into `llvm-project/clang-tools-extra/`. Your directory structure should look like this:

    ```
    llvm-project/
    ├── clang/
    ├── clang-tools-extra/
    │   ├── clang-mpi-analyser/  <-- Your project here
    │   │   ├── CMakeLists.txt
    │   │   ├── MPIAnalyzer.cpp
    │   │   ├── MPIAnalysisHelperFuncs.h
    │   │   ├── MPIAnalysisHelperFuncs.cpp
    │   │   └── ... (other source files)
    │   └── ... (other clang-tools-extra projects)
    ├── compiler-rt/
    └── ...
    ```

    **Ensure your `clang-mpi-analyser/CMakeLists.txt` is correctly configured to build your tool.** A basic `CMakeLists.txt` for your tool might look like:

    ```cmake
    # clang-mpi-analyser/CMakeLists.txt
    add_clang_tool(clang-mpi-analyser
      MPIAnalyzer.cpp
      MPIAnalysisHelperFuncs.cpp
      MPIAnalysisHelperFuncs.h
    )
    ```

3.  **Create Build Directory and Configure CMake:**

    ```bash
    cd llvm-project
    mkdir build
    cd build
    cmake -G "Unix Makefiles" \
          -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
          -DCMAKE_BUILD_TYPE="Release" \
          -DLLVM_ENABLE_DUMP=ON \
          ../llvm
    ```

    * `-G "Unix Makefiles"`: Generates Makefiles (you can use `Ninja` if preferred).
    * `-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"`: Tells CMake to build Clang and its extra tools (including yours).
    * `-DCMAKE_BUILD_TYPE="Release"`: Builds in release mode for performance. Use `Debug` for development.
    * `-DLLVM_ENABLE_DUMP=ON`: Useful for debugging ASTs.
    * `../llvm`: Points to the root LLVM source directory relative to `build`.

4.  **Build the Project:**

    ```bash
    cmake --build . -j$(nproc)
    # Or for specific tool:
    # cmake --build . --target clang-mpi-analyser -j$(nproc)
    ```

    This will compile LLVM, Clang, and your `clang-mpi-analyser` tool. The executable will be located at `build/bin/clang-mpi-analyser`.

## Usage

The `clang-mpi-analyser` tool operates on C/C++ source files. You need to provide it with the necessary include paths for your compiler and MPI library, just as you would when compiling with `clang` or `g++`.

### Basic Usage

```bash
/path/to/your/build/bin/clang-mpi-analyser -analyze-mpi-scatter-gather <your_source_file.cpp> -- <compiler_include_flags>
