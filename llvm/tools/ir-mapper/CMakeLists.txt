add_llvm_tool(ir-mapper
  IRMapper.cpp
)

llvm_update_compile_flags(ir-mapper)

target_link_libraries(ir-mapper PRIVATE
  LLVMCore
  LLVMSupport
  LLVMIRReader
  LLVMOption
)
