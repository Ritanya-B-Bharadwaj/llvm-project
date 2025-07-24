# MPI Reduction Analyzer - Build Instructions

## Project Structure

```
mpi-reduction-analyzer/
├── include/
│   ├── MPIReductionPass.h          # LLVM pass wrapper
│   ├── MPIReductionAnalyzer.h      # Main analyzer class
│   ├── ReductionDetector.h         # Pattern detection logic
│   └── Utils.h                     # Utility functions and constants
├── src/
│   ├── MPIReductionPass.cpp        # LLVM pass implementation
│   ├── MPIReductionAnalyzer.cpp    # Main analyzer implementation
│   ├── ReductionDetector.cpp       # Pattern detection implementation
│   ├── Utils.cpp                   # Utility functions
│   └── main.cpp                    # Command-line interface
├── CMakeLists.txt                  # Build configuration
├── BUILD.md                        # This file
└── README.md                       # Usage documentation
```

## Prerequisites

- **LLVM 17**: The analyzer is built against LLVM 17
- **CMake 3.10+**: For building the project
- **C++17 compliant compiler**: GCC 7+ or Clang 5+

## Building

### 1. Clone and prepare the project

```bash
git clone <repository-url>
cd mpi-reduction-analyzer
mkdir build
cd build
```

### 2. Configure with CMake

```bash
# Basic configuration
cmake ..

# Or with specific LLVM installation
cmake -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm ..

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with shared pass library
cmake -DBUILD_SHARED_PASS=ON ..
```

### 3. Build the project

```bash
make -j$(nproc)
```

### 4. Install (optional)

```bash
make install
```

## Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build type (Debug/Release) | Debug |
| `BUILD_SHARED_PASS` | Build shared library for LLVM pass | OFF |
| `CMAKE_INSTALL_PREFIX` | Installation directory | /usr/local |

## Usage Examples

### Basic Analysis

```bash
# Analyze all reduction types
./mpi_reduction_analyzer program.bc

# Analyze only sum reductions
./mpi_reduction_analyzer -analyze-mpi-reduction=sum program.bc

# Verbose output
./mpi_reduction_analyzer -verbose program.bc
```

### Advanced Options

```bash
# Detailed report
./mpi_reduction_analyzer -detailed program.bc

# Summary only
./mpi_reduction_analyzer -summary program.bc

# Export results to file
./mpi_reduction_analyzer -output=analysis_report.txt program.bc

# Use LLVM pass manager
./mpi_reduction_analyzer -use-pass program.bc
```

### Generating LLVM Bitcode

To analyze your MPI programs, you first need to generate LLVM bitcode:

```bash
# For C programs
clang -emit-llvm -c your_mpi_program.c -o your_mpi_program.bc

# For C++ programs
clang++ -emit-llvm -c your_mpi_program.cpp -o your_mpi_program.bc

# With debug information (recommended)
clang -emit-llvm -g -c your_mpi_program.c -o your_mpi_program.bc
```

## Troubleshooting

### LLVM Not Found

If CMake cannot find LLVM:

```bash
# Set LLVM_DIR explicitly
export LLVM_DIR=/usr/lib/llvm-17/lib/cmake/llvm
cmake ..

# Or use find_package hint
cmake -DLLVM_DIR=/usr/lib/llvm-17/lib/cmake/llvm ..
```

### Compilation Errors

1. **C++ Standard Issues**: Ensure your compiler supports C++17
2. **Missing Headers**: Make sure LLVM development headers are installed
3. **Linking Errors**: Verify all required LLVM libraries are available

### Runtime Issues

1. **Cannot load bitcode**: Ensure the input file is valid LLVM bitcode
2. **No debug information**: Use `-g` flag when generating bitcode for better location reporting
3. **Permission errors**: Check file permissions for input and output files

## Development

### Adding New Reduction Patterns

1. Extend `ReductionType` enum in `Utils.h`
2. Add pattern detection logic in `ReductionDetector.cpp`
3. Update string conversion functions in `Utils.cpp`
4. Add test cases for the new pattern

### Modifying Analysis Logic

The main analysis flow:
1. `main.cpp` - Command line parsing and module loading
2. `MPIReductionAnalyzer` - Orchestrates the analysis
3. `ReductionDetector` - Performs pattern matching
4. `Utils` - Provides helper functions

### Testing

```bash
# Test the analyzer
make test_analyzer

# Run with sample programs
./mpi_reduction_analyzer samples/simple_reduction.bc
```

## Performance Considerations

- The analyzer uses a depth-limited search to avoid infinite loops
- Large modules may take significant time to analyze
- Use `-summary` flag for quick overview of large codebases
- Consider using the pass manager (`-use-pass`) for integration with LLVM optimization pipelines