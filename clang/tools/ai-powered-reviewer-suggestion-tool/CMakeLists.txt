cmake_minimum_required(VERSION 3.15)
project(reviewer-suggester C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(LLVM REQUIRED CONFIG)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Find ONNX Runtime
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h PATHS /opt/homebrew/include/onnxruntime /usr/local/include/onnxruntime)
find_library(ONNXRUNTIME_LIB onnxruntime PATHS /opt/homebrew/lib /usr/local/lib)

if(NOT ONNXRUNTIME_INCLUDE_DIR OR NOT ONNXRUNTIME_LIB)
    message(FATAL_ERROR "ONNX Runtime not found. Please install it via 'brew install onnxruntime'")
endif()

message(STATUS "ONNX Runtime include: ${ONNXRUNTIME_INCLUDE_DIR}")
message(STATUS "ONNX Runtime library: ${ONNXRUNTIME_LIB}")

# Include directories - THIS IS THE KEY FIX
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${ONNXRUNTIME_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)  # Add this line for your headers

# Add definitions
add_definitions(${LLVM_DEFINITIONS})

# Source files
set(SOURCES
    src/main.cpp
    src/GitHubFetcher.cpp
    src/PRParser.cpp
    src/ReviewerSuggester.cpp
    src/BertTokenizer.cpp
)

# Header files (optional, for IDE support)
set(HEADERS
    include/GitHubFetcher.h
    include/PRParser.h
    include/ReviewerSuggester.h
    include/BertTokenizer.h
    include/PRData.h  # Add this line
)

# Create executable
add_executable(reviewer-suggester ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(reviewer-suggester 
    ${LLVM_LIBRARIES}
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
    ${ONNXRUNTIME_LIB}
)

# Set output directory
set_target_properties(reviewer-suggester PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# macOS specific settings
if(APPLE)
    set_target_properties(reviewer-suggester PROPERTIES
        MACOSX_RPATH TRUE
        BUILD_RPATH "${ONNXRUNTIME_LIB}/.."
    )
endif()
