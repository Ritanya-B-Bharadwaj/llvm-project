add_llvm_pass_plugin(PointerTracker
  PointerTracker.cpp

  DEPENDS
    intrinsics_gen

  LINK_COMPONENTS
    Core
    Support
    IRReader
)
