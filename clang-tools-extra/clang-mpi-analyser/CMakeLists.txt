cmake_minimum_required(VERSION 3.13)

project(mpi-analyser)

# Required to find LLVM/Clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_DIR "/mnt/d/OMG/LLVM/llvm-project/install/lib/cmake/llvm")
set(Clang_DIR "/mnt/d/OMG/LLVM/llvm-project/install/lib/cmake/clang")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${CLANG_DEFINITIONS})

# Enable RTTI (important for typeinfo errors)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

# Add your tool's source files here
add_executable(clang-mpi-analyser
  tool/main.cpp
  tool/MPIAnalyzer.cpp
  tool/MPIAction.cpp
  tool/MPIAnalysisHelperFuncs.cpp
)

# Link all required Clang/LLVM libraries
target_link_libraries(clang-mpi-analyser
  clangTooling
  clangFrontend
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangToolingCore
  LLVMCore
  LLVMSupport
)
