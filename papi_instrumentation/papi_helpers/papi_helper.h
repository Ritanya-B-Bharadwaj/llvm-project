// papi_helpers/papi_helper.h

#ifndef PAPI_HELPER_H
#define PAPI_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

void runtime_function_entry(const char* func_name);
void runtime_function_exit(const char* func_name);

#ifdef __cplusplus
}
#endif

#endif // PAPI_HELPER_H
