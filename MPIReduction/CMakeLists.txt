cmake_minimum_required(VERSION 3.10)
project(MPIReductionAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 17 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM headers and libraries
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-openmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Add debug flags for development
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Source files
set(SOURCES
    src/main.cpp
    src/Utils.cpp
    src/ReductionDetector.cpp
    src/MPIReductionAnalyzer.cpp
    src/MPIReductionPass.cpp
)

# Header files (for IDE support)
set(HEADERS
    include/Utils.h
    include/ReductionDetector.h
    include/MPIReductionAnalyzer.h
    include/MPIReductionPass.h
)

# Add the analyzer executable
add_executable(mpi_reduction_analyzer ${SOURCES} ${HEADERS})

# Link against specific LLVM libraries
target_link_libraries(mpi_reduction_analyzer
    LLVMCore
    LLVMSupport
    LLVMAnalysis
    LLVMTransformUtils
    LLVMIRReader
    LLVMAsmParser
    LLVMBitReader
    LLVMBitWriter
    LLVMTarget
    LLVMPasses
)

# Optional: Create a shared library for the pass
option(BUILD_SHARED_PASS "Build shared library for LLVM pass" OFF)

if(BUILD_SHARED_PASS)
    add_library(MPIReductionPass SHARED
        src/MPIReductionPass.cpp
        src/MPIReductionAnalyzer.cpp
        src/ReductionDetector.cpp
        src/Utils.cpp
    )
    
    target_link_libraries(MPIReductionPass
        LLVMCore
        LLVMSupport
        LLVMAnalysis
        LLVMTransformUtils
    )
    
    # Don't prefix the library with 'lib'
    set_target_properties(MPIReductionPass PROPERTIES PREFIX "")
endif()

# Install targets
install(TARGETS mpi_reduction_analyzer
    RUNTIME DESTINATION bin
)

if(BUILD_SHARED_PASS)
    install(TARGETS MPIReductionPass
        LIBRARY DESTINATION lib
    )
endif()

# Add custom target for testing
add_custom_target(test_analyzer
    COMMAND ${CMAKE_BINARY_DIR}/mpi_reduction_analyzer --help
    DEPENDS mpi_reduction_analyzer
    COMMENT "Testing MPI Reduction Analyzer"
)

# Print configuration summary
message(STATUS "")
message(STATUS "=== MPI Reduction Analyzer Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Build shared pass: ${BUILD_SHARED_PASS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==========================================")
message(STATUS "")