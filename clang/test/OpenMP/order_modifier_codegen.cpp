// Check code generation
// RUN: %clang_cc1 -verify -fopenmp -fopenmp-version=51 -triple x86_64-unknown-linux-gnu -emit-llvm %s -o - | FileCheck %s --check-prefix=CHECK 
// expected-no-diagnostics

#ifndef HEADER
#define HEADER

int main() {
  int N = 5, M = 5;
  int A[N], B[M], C[N], D[M], E[N], F[M];

#pragma omp parallel
  {
    #pragma omp for schedule(dynamic) nowait order(reproducible:concurrent)
    for (int I = 0; I < N; I++) {
      A[I] = I;
    }

    #pragma omp for order(reproducible:concurrent)
    for (int J = 0; J < M; J++) {
      B[J] = A[J] * J;
    }

    #pragma omp loop order(reproducible:concurrent)
    for (int I = 0; I < N; I++) {
      C[I] = I;
    }

    #pragma omp loop order(reproducible:concurrent)
    for (int J = 0; J < M; J++) {
      D[J] = C[J] * J;
    }

    #pragma omp simd order(reproducible:concurrent)
    for (int I = 0; I < N; I++) {
      E[I] = I;
    }

    #pragma omp simd order(reproducible:concurrent)
    for (int J = 0; J < M; J++) {
      F[J] = E[J] * J;
    }
  }

  /*for (int J = 0; J < N; J++) {
    if (B[J] != A[J] * A[J] || D[J] != C[J] * C[J] || F[J] != E[J] * E[J]) {
      printf("\nDidn't execute in consistent schedules\n");
      return 0;
    }
  }*/
  return 0;
}
#endif

// CHECK: call void @__kmpc_for_static_init_4{{.*}}
// CHECK: call void @__kmpc_for_static_fini{{.*}}
// CHECK: call void @__kmpc_barrier{{.*}}

// CHECK: call void @__kmpc_for_static_init_4{{.*}}
// CHECK: call void @__kmpc_for_static_fini{{.*}}
// CHECK: call void @__kmpc_barrier{{.*}}

// CHECK: call void @__kmpc_for_static_init_4{{.*}}
// CHECK: call void @__kmpc_for_static_fini{{.*}}
// CHECK: call void @__kmpc_barrier{{.*}}

// CHECK: call void @__kmpc_for_static_init_4{{.*}}
// CHECK: call void @__kmpc_for_static_fini{{.*}}
// CHECK: call void @__kmpc_barrier{{.*}}

// CHECK: call void @__kmpc_barrier{{.*}}
// CHECK: call void @__kmpc_barrier{{.*}}

