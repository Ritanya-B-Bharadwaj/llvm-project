add_llvm_component_library(LLVMPasses
  CodeGenPassBuilder.cpp
  OptimizationLevel.cpp
  PassBuilder.cpp
  PassBuilderBindings.cpp
  PassBuilderPipelines.cpp
  PassPlugin.cpp
  StandardInstrumentations.cpp

  ../../lib/Transforms/CLIFileNameGlobal/CLIFileNameGlobal.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Passes

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  AggressiveInstCombine
  Analysis
  CFGuard
  CodeGen
  GlobalISel
  Core
  Coroutines
  HipStdPar
  IPO
  InstCombine
  IRPrinter
  ObjCARC
  Scalar
  Support
  Target
  TransformUtils
  Vectorize
  Instrumentation
)

# ✅ Add include path for CLIFileNameGlobal.h
include_directories(${LLVM_MAIN_SRC_DIR}/llvm/lib/Transforms)

# ✅ Link to your custom pass (if needed)
# NOTE: Usually not needed unless you split into multiple libraries.
# target_link_libraries(LLVMPasses PRIVATE LLVMCLIFileNameGlobal)
