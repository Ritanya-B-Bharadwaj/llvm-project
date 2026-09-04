cmake_minimum_required(VERSION 3.10)
project(DetectConstructors)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(DetectConstructors DetectConstructors.cpp)

target_link_libraries(DetectConstructors
  PRIVATE
    clangTooling
    clangASTMatchers
    clangFrontend
    ${LLVM_LIBS}
)