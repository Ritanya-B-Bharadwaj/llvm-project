cmake_minimum_required(VERSION 3.13)
project(OMPIRMapper)

# LLVM and Clang setup
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
find_package(CURL REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include     # For json.hpp if added manually
)

link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core irreader passes)

add_executable(omp_ir_mapper main.cpp)

target_link_libraries(omp_ir_mapper
  PRIVATE
    CURL::libcurl
    clangTooling
    clangBasic
    clangFrontend
    clangAST
    clangLex
    clangParse
    clangRewrite
    clangAnalysis
    ${llvm_libs}
)
