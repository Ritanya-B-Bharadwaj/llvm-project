find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(papi_instrumention Instrumentor.cpp)

llvm_map_components_to_libnames(llvm_libs support)

target_link_libraries(papi_instrumention
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)
