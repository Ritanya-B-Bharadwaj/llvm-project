add_llvm_component_library(LLVMCLIFileNameGlobal
  CLIFileNameGlobal.cpp

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Core
  Support
)
