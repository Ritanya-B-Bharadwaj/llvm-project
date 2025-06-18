add_llvm_library(Fp16DemotionPlugin MODULE
  Fp16DemotionPlugin.cpp
  PLUGIN_TOOL clang
)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(Fp16DemotionPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    clangLex
    )
endif()

set_target_properties(Fp16DemotionPlugin PROPERTIES
  SUFFIX ".dylib"
  LINK_FLAGS "-undefined dynamic_lookup"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/my_plugins"
)