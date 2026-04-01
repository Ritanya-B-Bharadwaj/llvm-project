# Abort if plugin support is disabled.
if(NOT CLANG_PLUGIN_SUPPORT)
  return()
endif()

add_llvm_library(FpDowncastCheck MODULE
  FpDowncastCheck.cpp
  PLUGIN_TOOL clang
)

# Only link explicitly on Windows to avoid duplicate command-line flag issues.
if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(FpDowncastCheck PRIVATE
    clangAST
    clangBasic
    clangFrontend
  )
endif() 