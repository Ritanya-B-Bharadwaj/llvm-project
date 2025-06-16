add_llvm_library(LLVMHighlight MODULE
  Highlight.cpp

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Core
  Support
  IRReader
)
