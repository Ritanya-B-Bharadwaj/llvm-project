cmake_minimum_required(VERSION 3.13.4)
project(OpenMPTestGenerator)

# Set C++ standard to 17 for filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required libraries using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Find nlohmann/json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(include)
include_directories(${SQLITE3_INCLUDE_DIRS})

# Add executable
add_executable(openmp-test-gen src/openmp-test-gen.cpp)

# Link libraries (including filesystem for C++17)
target_link_libraries(openmp-test-gen
  ${CURL_LIBRARIES}
  ${SQLITE3_LIBRARIES}
  nlohmann_json::nlohmann_json
  stdc++fs  # For filesystem support on older compilers
)

# Include directories and compile flags
target_include_directories(openmp-test-gen PRIVATE 
  ${CURL_INCLUDE_DIRS}
  ${SQLITE3_INCLUDE_DIRS}
)

target_compile_options(openmp-test-gen PRIVATE 
  ${CURL_CFLAGS_OTHER}
  ${SQLITE3_CFLAGS_OTHER}
)
