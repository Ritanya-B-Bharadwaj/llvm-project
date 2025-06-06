// RUN: %clang_cc1 %s -emit-llvm -o - -triple -amdgcn-amd-amdhsa \
// RUN:  -target-cpu gfx906 -fcuda-is-device -x hip \
// RUN:  | FileCheck --check-prefixes=COMMON,CRDIV %s
// RUN: %clang_cc1 %s -emit-llvm -o - -triple -amdgcn-amd-amdhsa \
// RUN:  -target-cpu gfx906 -fcuda-is-device -x hip \
// RUN:  -fno-hip-fp32-correctly-rounded-divide-sqrt \
// RUN:  | FileCheck --check-prefixes=COMMON,NCRDIV %s

#include "Inputs/cuda.h"

typedef __attribute__(( ext_vector_type(4) )) float floatvec4;

// COMMON-LABEL: @_Z11spscalardiv
// COMMON: fdiv{{.*}},
// NCRDIV: !fpmath ![[MD:[0-9]+]]
// CRDIV-NOT: !fpmath
__device__ float spscalardiv(float a, float b) {
  return a / b;
}

// COMMON-LABEL: @_Z11spvectordiv
// COMMON: fdiv{{.*}},
// NCRDIV: !fpmath ![[MD]]
// CRDIV-NOT: !fpmath
__device__ floatvec4 spvectordiv(floatvec4 a, floatvec4 b) {
  return a / b;
}

// COMMON-LABEL: @_Z11dpscalardiv
// COMMON-NOT: !fpmath
__device__ double dpscalardiv(double a, double b) {
  return a / b;
}

// COMMON-LABEL: @_Z12spscalarsqrt
// NCRDIV: call contract float @llvm.sqrt.f32(float %{{.+}}), !fpmath ![[MD:[0-9]+]]
// CRDIV: call contract float @llvm.sqrt.f32(float %{{.+}}){{$}}
__device__ float spscalarsqrt(float a) {
  return __builtin_sqrtf(a);
}

// COMMON-LABEL: @_Z12dpscalarsqrt
// COMMON: call contract double @llvm.sqrt.f64(double %{{.+}}){{$}}
// COMMON-NOT: !fpmath
__device__ double dpscalarsqrt(double a) {
  return __builtin_sqrt(a);
}

// COMMON-LABEL: @_Z28test_builtin_elementwise_f32f
// NCRDIV: call contract float @llvm.sqrt.f32(float %{{.+}}), !fpmath ![[MD:[0-9]+]]
// CRDIV: call contract float @llvm.sqrt.f32(float %{{.+}}){{$}}
__device__ float test_builtin_elementwise_f32(float a) {
  return __builtin_elementwise_sqrt(a);
}

// COMMON-LABEL: @_Z28test_builtin_elementwise_f64d
// COMMON: call contract double @llvm.sqrt.f64(double %{{.+}}){{$}}
// COMMON-NOT: !fpmath
__device__ double test_builtin_elementwise_f64(double a) {
  return __builtin_elementwise_sqrt(a);
}

// NCRSQRT: ![[MD]] = !{float 2.500000e+00}
