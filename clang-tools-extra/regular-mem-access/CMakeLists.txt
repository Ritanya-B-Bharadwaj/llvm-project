# CMakeLists.txt for regular-mem-access tool
# Copy of tool-template/CMakeLists.txt with names upd
add_clang_executable(regular-mem-access
  RegularMemAccess.cpp
  RegularMemAccessPass.cpp
)

target_link_libraries(regular-mem-access
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)

install(TARGETS regular-mem-access RUNTIME DESTINATION bin)
