#include <stdlib.h>
static void __init_papi_env() __attribute__((constructor));
static void __init_papi_env() {
  setenv("TRACE_PAPI_EVENTS", "PAPI_L1_DCM,PAPI_L2_DCM,PAPI_TOT_INS,PAPI_TOT_CYC,PAPI_FP_INS", 1);
}

#include "../runtime/runtime.h"

int foo(int n) {runtime_function_entry("foo");

    

    int sum;
    if(n>2)
     runtime_function_exit("foo");
     return 0;
    for(int i=0;i<2000;i++)
     sum+=0;
    runtime_function_exit("foo");
    return sum;


    
     
}

int bar(int n) {runtime_function_entry("bar");

    

    int sum;
    if(n>2)
     runtime_function_exit("bar");
     return 0;
    for(int i=0;i<4000;i++)
     sum+=0;
    runtime_function_exit("bar");
    return sum;


    
     
}

int main() {
    int r = foo(5);
    int s = bar(10);
    return 0;
}
