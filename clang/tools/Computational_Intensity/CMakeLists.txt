# cmake_minimum_required(VERSION 3.13)
# project(ComputationalIntensityPass)

# find_package(LLVM REQUIRED CONFIG)

# message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
# include(HandleLLVMOptions)
# include(AddLLVM)

# add_definitions(${LLVM_DEFINITIONS})
# include_directories(${LLVM_INCLUDE_DIRS})

# add_library(ComputationalIntensityPass MODULE ComputationalIntensityPass.cpp)

# set_target_properties(ComputationalIntensityPass PROPERTIES
#   COMPILE_FLAGS "-fno-rtti"
# )

cmake_minimum_required(VERSION 3.13)
project(ComputationalIntensityPass)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_llvm_pass_plugin(ComputationalIntensityPass ComputationalIntensityPass.cpp)
