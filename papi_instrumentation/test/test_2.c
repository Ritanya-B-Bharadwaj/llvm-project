#include "../papi_helpers/papi_helper.h"

int foo(int n) {
    

    int sum;
    if(n>2)
     return 0;
    for(int i=0;i<2000;i++)
     sum+=0;
    return sum;


    
     
}

int bar(int n) {
    

    int sum;
    if(n>2)
     return 0;
    for(int i=0;i<4000;i++)
     sum+=0;
    return sum;


    
     
}

int main() {
    int r = foo(5);
    int s = bar(10);
    return 0;
}
