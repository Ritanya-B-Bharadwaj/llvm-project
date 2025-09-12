# MPI Uniform Participation Analyzer

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![Build System](https://img.shields.io/badge/Build-CMake-orange.svg)
![Framework](https://img.shields.io/badge/Framework-LLVM-green.svg)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

A professional, standalone command-line tool for performing static analysis on MPI-based C/C++ programs. It parses LLVM Intermediate Representation (IR) to detect communication patterns, identify matching `MPI_Send`/`MPI_Recv` pairs, and report on uniform participation across MPI processes.

## Features

- **Professional CLI:** Rich command-line options for input, output, and report formatting, powered by `CLI11`.
- **Multiple Output Formats:** Generate human-readable text reports or machine-readable formats like JSON and CSV.
- **Robust Analysis:** Efficiently groups and matches MPI calls by communicator and tag to identify complete pairs and locate unmatched calls.
- **Flexible Input:** Accepts one or more LLVM IR files for analysis in a single run.
- **Extensible Architecture:** Cleanly separated code for CLI parsing, analysis logic, and reporting makes it easy to add new features.
- **Modern Build System:** Uses CMake for a reliable and cross-platform build process.

## Prerequisites

Before building, you must have the following software installed on your Windows system:

1.  **Microsoft C++ (MSVC) Build Tools:** Required for the C++ standard library and linker.
    -   Download from the [Visual Studio website](https://visualstudio.microsoft.com/downloads/) (select "Build Tools for Visual Studio").
    -   Install the **"Desktop development with C++"** workload.
2.  **CMake (Version 3.15+):** The build system generator.
    -   Install via `winget install -e --id Kitware.CMake`.
3.  **Ninja:** A fast, modern build system used by CMake.
    -   Install via `winget install -e --id Ninja-build.Ninja`.
4.  **Git:** For cloning the repository.
    -   Install from the [official Git website](https://git-scm.com/downloads).

## Setup and Build Instructions

1.  **Clone the Repository:**
    ```powershell
    git clone <your-repository-url> MPI-polished-tool
    cd MPI-polished-tool
    ```

2.  **Setup Dependencies:**
    -   The build expects dependencies to be in `C:\Program Files\`. Ensure you have placed your `LLVM` and `MPI` folders there.

3.  **Generate Test IR Files:**
    ```powershell
    # Create a directory for the generated .ll files
    mkdir test-build
    # Compile a test case
    & "C:/Program Files/LLVM/bin/clang.exe" -S -emit-llvm tests/uniform_comm.c -o test-build/uniform_comm.ll -I"C:/Program Files/MPI/Include"
    ```

4.  **Configure and Build with CMake:**
    ```powershell
    # Create a build directory and configure the project
    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" -B build

    # Compile the code
    cmake --build build
    ```

## Usage

Run the tool from the project's root directory.

### Basic Analysis

```powershell
./build/mpi-analyser.exe --input tests/build/uniform_comm.ll