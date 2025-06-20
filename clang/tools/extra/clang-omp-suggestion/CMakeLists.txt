# Define the project name
project(clang-omp-suggestion)

# Add the executable
add_clang_tool(clang-omp-suggestion
  clang-omp-suggestion.cpp
)

# Link necessary libraries
target_link_libraries(clang-omp-suggestion
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  LLVMSupport
)
