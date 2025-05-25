add_clang_tool(fp16AccuracyChecker
  FloatConverter.cpp
  FloatRewriter.cpp
)

target_link_libraries(fp16AccuracyChecker
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)

