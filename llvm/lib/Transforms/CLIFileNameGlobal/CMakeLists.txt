add_llvm_component_library(LLVMCLIFileNameGlobal
  CLIFileNameGlobal.cpp

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Core
  Support

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/CLIFileNameGlobal
)
