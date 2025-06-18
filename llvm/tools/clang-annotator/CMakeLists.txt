add_llvm_tool(clang-annotator
  ClangAnnotator.cpp
)

# Add the ACTUAL Clang source include dir
target_include_directories(clang-annotator PRIVATE
  ${LLVM_SOURCE_DIR}/tools/clang/include
  ${LLVM_BINARY_DIR}/tools/clang/include
  ${LLVM_SOURCE_DIR}/include
  ${LLVM_BINARY_DIR}/include

 
  ${LLVM_SOURCE_DIR}/../clang/include
)

target_link_libraries(clang-annotator
  PRIVATE
  clangTooling
  clangFrontend
  clangAST
  clangBasic
  clangASTMatchers
)
