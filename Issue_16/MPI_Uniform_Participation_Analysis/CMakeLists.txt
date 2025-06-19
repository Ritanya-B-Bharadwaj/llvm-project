# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Define the project
project(MPIPolishedTool VERSION 1.0.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- THE CRITICAL FIX for Runtime Library Mismatch ---
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# --- Use llvm-config to get library info ---
set(LLVM_CONFIG_EXECUTABLE "C:/Program Files/LLVM/bin/llvm-config.exe")
execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libnames core irreader support asmparser bitreader bitstreamreader remarks binaryformat targetparser demangle
    OUTPUT_VARIABLE LLVM_LIB_NAMES_STR OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(LLVM_LIB_NAMES NATIVE_COMMAND "${LLVM_LIB_NAMES_STR}")

# --- Configure the Project ---
add_executable(mpi-analyser src/main.cpp src/Analysis.cpp src/Reporter.cpp)
target_compile_definitions(mpi-analyser PRIVATE PROJECT_VERSION="${PROJECT_VERSION}")
target_link_directories(mpi-analyser PRIVATE "C:/Program Files/LLVM/lib")
target_include_directories(mpi-analyser PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    "C:/Program Files/LLVM/include"
    "C:/Program Files/MPI/Include"
)
target_link_libraries(mpi-analyser PRIVATE
    ${LLVM_LIB_NAMES}
    psapi.lib shell32.lib ole32.lib uuid.lib advapi32.lib ws2_32.lib ntdll.lib
)
message(STATUS "Successfully configured MPI Polished Tool with static runtime")

# ===================================================================
# --- PRODUCTION INSTALLATION AND PACKAGING (CPack) ---
# ===================================================================
include(InstallRequiredSystemLibraries)
file(GLOB LLVM_DLLS "C:/Program Files/LLVM/bin/*.dll")
install(TARGETS mpi-analyser RUNTIME DESTINATION bin)
install(FILES ${LLVM_DLLS} DESTINATION bin)

# --- THE FIX IS HERE ---
# Use an absolute path to prevent the CPack bug on Windows.
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION .)

set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_PACKAGE_NAME "MPI-Analyser")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A static analyser for MPI uniform participation")
set(CPACK_PACKAGE_VENDOR "Yogish HG")
set(CPACK_NSIS_PACKAGE_NAME "MPI Analyser v${PROJECT_VERSION}")
include(CPack)