add_llvm_tool(omp-pr-summary
  main.cpp
  DiffFetcher.cpp
  SpecRetriever.cpp
  SummaryGenerator.cpp
)

target_link_libraries(omp-pr-summary PRIVATE LLVMSupport)
