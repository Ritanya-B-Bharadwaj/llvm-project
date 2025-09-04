# Abort if plugin support is disabled.
if(NOT CLANG_PLUGIN_SUPPORT)
  return()
endif()

add_llvm_library(FpDowncastCheck MODULE
  FpDowncastCheck.cpp
  PLUGIN_TOOL clang
)
