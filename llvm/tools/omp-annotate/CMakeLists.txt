add_llvm_tool(omp-annotate
  omp-annotate.cpp
)

target_link_libraries(omp-annotate PRIVATE
  LLVMCore
  LLVMSupport
  LLVMIRReader
  LLVMPasses
)
