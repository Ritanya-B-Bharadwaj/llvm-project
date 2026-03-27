set(LLVM_LINK_COMPONENTS
  Support
)

add_clang_executable(identify-scope
  identify-scope.cpp
)

target_link_libraries(identify-scope
  PRIVATE
  clangAST
  clangFrontend
  clangTooling
  clangBasic
  clangLex
)
