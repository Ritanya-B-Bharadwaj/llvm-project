# 🧮 MPI Reduction Analyzer

This tool statically analyzes LLVM IR code to identify **opportunities for MPI reduction operations** like `MPI_Reduce`, `MPI_Allreduce`, etc. It detects manual reduction patterns in C/C++ programs and suggests optimization potential for collective communication.

## 📁 Project Structure

MPIReduction/
├── build/ # Build directory (CMake output + binary)
│ └── mpi_reduction_analyzer
├── include/ # Header files
│ ├── MPIReductionAnalyzer.h
│ ├── MPIReductionPass.h
│ ├── ReductionDetector.h
│ └── Utils.h
├── src/ # Source files (main logic)
│ ├── main.cpp
│ ├── MPIReductionAnalyzer.cpp
│ ├── MPIReductionPass.cpp
│ ├── ReductionDetector.cpp
│ └── Utils.cpp
├── test/ # Test files
│ ├── mpi.h
│ ├── test_reduction.ll # LLVM IR for testing
│ ├── testit.c # Optional C file to generate IR
│ └── testit.ll
├── CMakeLists.txt # CMake configuration
├── build.md # (Optional) Build documentation
└── README.md # 📄 You're reading it!

## ⚙️ Prerequisites

- LLVM (v10 or higher) with `clang`, `opt`, and `llvm-config`
- CMake ≥ 3.10
- C++ compiler (`g++` or `clang++`)
- Unix-like environment (Linux, WSL, macOS)

## 🛠️ LLVM + Clang + Build Tools Setup on Ubuntu

# Update package list
sudo apt update

# Install compiler toolchain, CMake, and Ninja
sudo apt install -y build-essential cmake ninja-build

# Install LLVM core tools and Clang
sudo apt install -y clang llvm lld

# Install development libraries for writing LLVM passes
sudo apt install -y llvm-dev libclang-dev clang-tools

# Install OpenMPI and mpicc
sudo apt install -y openmpi-bin libopenmpi-dev

# Check Installed Versions
clang --version
llvm-as --version
cmake --version
ninja --version

## 🔨 Build Instructions

# 1. Clone the repo or navigate into the folder
cd MPIReduction

# 2. Create a build directory
mkdir build && cd build

# 3. Generate Makefiles using CMake
cmake ..

# 4. Compile the analyzer
make

This produces the binary: ./build/mpi_reduction_analyzer

## 🔨 Generate Intermediate Code 
clang -S -emit-llvm test/testit.c -o test/testit.ll

## 🚀 Usage
/path/to/mpi_reduction_analyzer -analyze-mpi-reduction=<type> /path/to/input.ll

