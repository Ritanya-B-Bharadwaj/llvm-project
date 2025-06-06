;RUN: llc < %s -mtriple=r600 -mcpu=redwood | FileCheck %s

;CHECK: MIN * T{{[0-9]+\.[XYZW], T[0-9]+\.[XYZW], T[0-9]+\.[XYZW]}}

define amdgpu_ps void @test(<4 x float> inreg %reg0) {
   %r0 = extractelement <4 x float> %reg0, i32 0
   %r1 = extractelement <4 x float> %reg0, i32 1
   %r2 = fcmp uge float %r0, %r1
   %r3 = select i1 %r2, float %r1, float %r0
   %vec = insertelement <4 x float> poison, float %r3, i32 0
   call void @llvm.r600.store.swizzle(<4 x float> %vec, i32 0, i32 0)
   ret void
}

declare void @llvm.r600.store.swizzle(<4 x float>, i32, i32)
