cmake_minimum_required(VERSION 3.13)
project(RegularMemoryAccessAnalyzer)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(analyze_regular_memory_access RegularMemoryAccessAnalyzer.cpp)

target_link_libraries(analyze_regular_memory_access
  clangTooling
  clangBasic
  clangASTMatchers
  clangFrontend
  clangAST
  clangLex
  clangParse
  clangSema
  clangEdit
  clangAnalysis
)
