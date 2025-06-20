add_llvm_component_library(LLVMComputationalIntensityPass
  ComputationalIntensityPass.cpp
  

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Core
  Support
  Analysis
  
)