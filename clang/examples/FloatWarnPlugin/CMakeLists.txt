# If we don't need RTTI or EH, there's no reason to export anything
# from the plugin.
if(NOT MSVC)
  if(NOT LLVM_REQUIRES_RTTI)
    if(NOT LLVM_REQUIRES_EH)
      # Commenting out exports to avoid potential initialization issues
      set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/FloatWarnPlugin.exports)
    endif()
  endif()
endif()

# Define the plugin library
add_llvm_library(FloatWarnPlugin MODULE FloatWarnPlugin.cpp PLUGIN_TOOL clang)

# Platform-specific configurations
if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(FloatWarnPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    clangTooling
    LLVMSupport
  )
  set_target_properties(FloatWarnPlugin PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
  )
endif()

