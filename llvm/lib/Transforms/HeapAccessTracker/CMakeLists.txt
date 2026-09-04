add_llvm_pass_plugin(LLVMHeapAccessTracker
  HeapAccessTracker.cpp

  PLUGIN_TOOL
  opt
)

