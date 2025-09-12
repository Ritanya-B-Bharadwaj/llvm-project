# CMakeLists.txt for regular-mem-access tool
add_clang_executable(regular-mem-access
  RegularMemAccess.cpp
)

# Do NOT add test.c to the build target! It is a test input, not a source file for the tool.

# Required libraries for codegen and IR analysis

target_link_libraries(regular-mem-access
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangTooling
  clangCodeGen
  LLVMCore
)

install(TARGETS regular-mem-access RUNTIME DESTINATION bin)
