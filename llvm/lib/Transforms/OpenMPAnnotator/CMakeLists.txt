add_llvm_library(LLVMOpenMPAnnotatorPass
  OpenMPAnnotatorPass.cpp

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Core
  Support
)
