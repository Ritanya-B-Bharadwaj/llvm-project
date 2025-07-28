// runtime/runtime.h

#ifndef RUNTIME_H
#define RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

void runtime_function_entry(const char* func_name);
void runtime_function_exit(const char* func_name);

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_H
