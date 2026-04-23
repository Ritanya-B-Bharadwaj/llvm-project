cmake_minimum_required(VERSION 3.10)
project(OMPIRMapper LANGUAGES NONE)

# Find Python interpreter
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# User configurable variables
set(SOURCE_FILE "" CACHE STRING "C/C++ source file to process")
set(ARGS "" CACHE STRING "Additional arguments for ompir_map.py (semicolon-separated)")

add_custom_target(run
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ompir_map.py ${SOURCE_FILE} ${ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running ompir_map.py"
)
