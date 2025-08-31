set(LLVM_LINK_COMPONENTS
  Core
  IRReader
  Support
  )

add_clang_tool(llvm-source-mapper
  llvm-source-mapper.cpp
  )

clang_target_link_libraries(llvm-source-mapper
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )
