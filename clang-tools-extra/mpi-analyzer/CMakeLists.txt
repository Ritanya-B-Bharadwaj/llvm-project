cmake_minimum_required(VERSION 3.13)

project(mpi-analyzer LANGUAGES C CXX)

# Locate LLVM config (provided by your main build)
find_package(LLVM REQUIRED CONFIG)

# Locate Clang config explicitly (optional but recommended)
find_package(Clang REQUIRED CONFIG)

# Locate MPI package
find_package(MPI REQUIRED)

# Add LLVM CMake modules path (helps with LLVM utilities)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

# Set C++ standard and other compile options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Add your source file(s) here
set(SOURCES MPIAnalyzer.cpp)

# Create the executable
add_executable(mpi-analyzer ${SOURCES})

# Include LLVM and MPI headers and definitions properly
target_include_directories(mpi-analyzer PRIVATE
  ${LLVM_INCLUDE_DIRS}
  ${MPI_INCLUDE_PATH}
)

target_compile_definitions(mpi-analyzer PRIVATE ${LLVM_DEFINITIONS})

# Link to required LLVM, Clang, and MPI libraries
target_link_libraries(mpi-analyzer
  PRIVATE
  LLVM
  clangASTMatchers
  clangTooling
  clangBasic
  clangFrontend
  ${MPI_CXX_LIBRARIES}
)

# Optional messages for debugging config
message(STATUS "Using LLVMConfig.cmake from: ${LLVM_DIR}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "MPI include dirs: ${MPI_INCLUDE_PATH}")
message(STATUS "MPI libraries: ${MPI_CXX_LIBRARIES}")
