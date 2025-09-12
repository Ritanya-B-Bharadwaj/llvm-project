#include <stdlib.h>
static void __init_papi_env() __attribute__((constructor));
static void __init_papi_env() {
  setenv("TRACE_PAPI_EVENTS", "PAPI_L1_DCM,PAPI_L1_ICM,PAPI_L2_DCM,PAPI_L2_ICM,PAPI_L3_TCM,PAPI_CA_SHR,PAPI_CA_CLN,PAPI_CA_INV,PAPI_TLB_DM,PAPI_TLB_IM,PAPI_BR_MSP,PAPI_BR_PRC,PAPI_TOT_INS,PAPI_TOT_CYC,PAPI_FP_INS,PAPI_FP_OPS", 1);
}

#include "../papi_helpers/papi_helper.h"

int square(int n) {runtime_function_entry("square");

    

    int sum;
    if(n>2)
     runtime_function_exit("square");
     return 0;
    for(int i=0;i<2000;i++)
     sum+=0;
    runtime_function_exit("square");
    return sum;


    
     
}

int main() {
    int r = square(5);
    return 0;
}
