cmake_minimum_required(VERSION 3.10)
project(ImplicitCtorDetector)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(ImplicitCtorDetector ImplicitCtorDetector.cpp)

llvm_map_components_to_libnames(llvm_libs support)
target_link_libraries(ImplicitCtorDetector
  PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
    ${llvm_libs}
)

