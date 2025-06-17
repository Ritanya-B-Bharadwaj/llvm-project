cmake_minimum_required(VERSION 3.13.4)
project(LLVMSourceMapper)

# Find LLVM and Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Find curl and nlohmann/json
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM and Clang headers
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(/opt/homebrew/opt/curl/include)
include_directories(/opt/homebrew/include)

add_definitions(${LLVM_DEFINITIONS})

# Set C++ standard to C++17 (needed for LLVM 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source mapper executable
add_executable(source-mapper
    src/SourceMapper.cpp
)

# Link against LLVM and Clang libraries
target_link_libraries(source-mapper
    PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
    clangAST
    clangFrontend
    clangSerialization
    ${LLVM_AVAILABLE_LIBS}
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

# Installation rules
install(TARGETS source-mapper DESTINATION bin)
