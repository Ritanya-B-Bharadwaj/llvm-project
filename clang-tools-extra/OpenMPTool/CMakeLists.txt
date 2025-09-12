set(LLVM_OPTIONAL_SOURCES
  OpenMPTool.cpp
  DetectOMP.cpp
)

# Main AI tool
add_clang_executable(OpenMPTool
  OpenMPTool.cpp
  PARTIAL_SOURCES_INTENDED
)

target_link_libraries(OpenMPTool
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)

# AST-based DetectOMP tool
add_clang_tool(DetectOMP
  DetectOMP.cpp
  PARTIAL_SOURCES_INTENDED
)

target_link_libraries(DetectOMP
  PRIVATE
  clangTooling
  clangFrontend
  clangASTMatchers
  clangAST
  clangBasic
)

