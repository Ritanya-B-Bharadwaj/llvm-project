cmake_minimum_required(VERSION 3.13)

project(clang-mpi-analyser)

# Declare the tool using LLVM's macro
add_llvm_executable(clang-mpi-analyser
  tool/main.cpp
  tool/MPIAnalyzer.cpp
  tool/MPIAction.cpp
  tool/MPIAnalysisHelperFuncs.cpp
)

# Link against required Clang and LLVM libraries
target_link_libraries(clang-mpi-analyser
  PRIVATE
  clangTooling
  clangFrontend
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangToolingCore
)
