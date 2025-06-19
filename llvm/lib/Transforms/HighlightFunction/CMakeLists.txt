cmake_minimum_required(VERSION 3.13)
project(MyPass LANGUAGES C CXX)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set C++ standard and flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Define the plugin library
add_library(MyPass MODULE MyPass.cpp)

# Set proper compilation flags
target_compile_features(MyPass PRIVATE cxx_std_17)
target_compile_options(MyPass PRIVATE -fno-rtti)

# Don't link against LLVM libraries for plugins
# target_link_libraries(MyPass PRIVATE ${llvm_libs})
