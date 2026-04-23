# Enhanced OpenMP IR Mapper

## Overview
The Enhanced OpenMP IR Mapper is a CLI tool designed to automatically map OpenMP source lines to their corresponding LLVM Intermediate Representation (IR) instructions. This tool leverages Clang's capabilities to parse C++ source files with OpenMP directives and generates annotated LLVM IR that links OpenMP constructs to their respective source lines.

## Features
- Parses C++ source files to detect OpenMP directives such as `#pragma omp parallel`, `#pragma omp target`, and `#pragma omp taskloop`.
- Generates LLVM IR that includes OpenMP runtime calls and offloading markers.
- Produces annotated `.ll` files linking OpenMP directives to their corresponding IR blocks.
- Supports both host and device code for target offloading.
- Generates structured listings mapping directives to IR blocks for improved readability.
- Outputs an HTML report summarizing the OpenMP directives and their corresponding IR mappings.

## Project Structure
```
omp_ir_mapper
├── CMakeLists.txt        # CMake configuration file
├── src                   # Source files
│   ├── main.cpp          # Entry point of the application
│   ├── omp_parser.cpp     # Implementation of OMPParser
│   ├── omp_parser.h       # Header for OMPParser
│   ├── ir_annotator.cpp   # Implementation of IRAnnotator
│   ├── ir_annotator.h     # Header for IRAnnotator
│   ├── directive_descriptions.cpp # Implementation of DirectiveDescriptions
│   ├── directive_descriptions.h   # Header for DirectiveDescriptions
│   ├── html_reporter.cpp  # Implementation of HTMLReporter
│   ├── html_reporter.h    # Header for HTMLReporter
│   ├── config.cpp         # Implementation of configuration loading
│   ├── config.h           # Header for configuration structure
├── examples               # Example C++ programs
│   └── sample_omp.cpp     # Sample OpenMP program
├── config                 # Configuration files
│   └── default.toml       # Default configuration in TOML format
└── README.md              # Project documentation
```

## Build Instructions
1. Ensure you have CMake and the required dependencies (LLVM, Clang, toml11, nlohmann_json) installed.
2. Clone the repository and navigate to the project directory.
3. Create a build directory:
   ```
   mkdir build
   cd build
   ```
4. Run CMake to configure the project:
   ```
   cmake ..
   ```
5. Build the project:
   ```
   cmake --build .
   ```

## Usage
To use the Enhanced OpenMP IR Mapper, run the compiled executable with the path to a C++ source file as an argument:
```
./omp_ir_mapper <path_to_cpp_file>
```
The tool will parse the specified file, generate the corresponding LLVM IR, and produce annotated output files.

## Output Formats
- Annotated LLVM IR files (`.ll`) linking OpenMP directives to IR instructions.
- Structured listings mapping directives to IR blocks.
- HTML reports summarizing the OpenMP directives and their mappings.

## Extending the Tool
Developers can extend the functionality of the Enhanced OpenMP IR Mapper by adding new OpenMP directive support in the `omp_parser` and `directive_descriptions` modules. Additionally, new output formats can be implemented in the `html_reporter` module.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.