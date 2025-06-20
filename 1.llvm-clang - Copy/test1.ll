; ModuleID = 'test1.cpp'
source_filename = "test1.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.ident_t = type { i32, i32, i32, i32, ptr }

@0 = private unnamed_addr constant [22 x i8] c";test1.cpp;main;6;1;;\00", align 1
@1 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 21, ptr @0 }, align 8
@2 = private unnamed_addr constant [23 x i8] c";test1.cpp;main;6;42;;\00", align 1
@3 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @2 }, align 8
@.gomp_critical_user_.reduction.var = common global [8 x i32] zeroinitializer, align 8
@4 = private unnamed_addr constant %struct.ident_t { i32 0, i32 18, i32 0, i32 22, ptr @2 }, align 8
@5 = private unnamed_addr constant %struct.ident_t { i32 0, i32 2, i32 0, i32 21, ptr @0 }, align 8
@.str = private unnamed_addr constant [19 x i8] c"harmonic sum=%.5f\0A\00", align 1, !dbg !0

; Function Attrs: mustprogress noinline norecurse optnone uwtable
define dso_local noundef i32 @main() #0 !dbg !266 {
  %1 = alloca i32, align 4
  %2 = alloca double, align 8
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !268, metadata !DIExpression()), !dbg !269
  store double 0.000000e+00, ptr %2, align 8, !dbg !269
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @5, i32 1, ptr @main.omp_outlined, ptr %2), !dbg !270
  %3 = load double, ptr %2, align 8, !dbg !271
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, double noundef %3), !dbg !272
  ret i32 0, !dbg !273
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline norecurse nounwind optnone uwtable
define internal void @main.omp_outlined_debug__(ptr noalias noundef %0, ptr noalias noundef %1, ptr noundef nonnull align 8 dereferenceable(8) %2) #2 !dbg !274 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca double, align 8
  %14 = alloca i32, align 4
  %15 = alloca [1 x ptr], align 8
  store ptr %0, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !282, metadata !DIExpression()), !dbg !283
  store ptr %1, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !284, metadata !DIExpression()), !dbg !283
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !285, metadata !DIExpression()), !dbg !286
  %16 = load ptr, ptr %6, align 8, !dbg !287
  call void @llvm.dbg.declare(metadata ptr %7, metadata !288, metadata !DIExpression()), !dbg !283
  call void @llvm.dbg.declare(metadata ptr %9, metadata !289, metadata !DIExpression()), !dbg !283
  store i32 0, ptr %9, align 4, !dbg !290
  call void @llvm.dbg.declare(metadata ptr %10, metadata !291, metadata !DIExpression()), !dbg !283
  store i32 99, ptr %10, align 4, !dbg !290
  call void @llvm.dbg.declare(metadata ptr %11, metadata !292, metadata !DIExpression()), !dbg !283
  store i32 1, ptr %11, align 4, !dbg !290
  call void @llvm.dbg.declare(metadata ptr %12, metadata !293, metadata !DIExpression()), !dbg !283
  store i32 0, ptr %12, align 4, !dbg !290
  call void @llvm.dbg.declare(metadata ptr %13, metadata !294, metadata !DIExpression()), !dbg !283
  store double 0.000000e+00, ptr %13, align 8, !dbg !295
  call void @llvm.dbg.declare(metadata ptr %14, metadata !296, metadata !DIExpression()), !dbg !283
  %17 = load ptr, ptr %4, align 8, !dbg !287
  %18 = load i32, ptr %17, align 4, !dbg !287
  call void @__kmpc_for_static_init_4(ptr @1, i32 %18, i32 34, ptr %12, ptr %9, ptr %10, ptr %11, i32 1, i32 1), !dbg !297
  %19 = load i32, ptr %10, align 4, !dbg !290
  %20 = icmp sgt i32 %19, 99, !dbg !290
  br i1 %20, label %21, label %22, !dbg !290

21:                                               ; preds = %3
  br label %24, !dbg !290

22:                                               ; preds = %3
  %23 = load i32, ptr %10, align 4, !dbg !290
  br label %24, !dbg !290

24:                                               ; preds = %22, %21
  %25 = phi i32 [ 99, %21 ], [ %23, %22 ], !dbg !290
  store i32 %25, ptr %10, align 4, !dbg !290
  %26 = load i32, ptr %9, align 4, !dbg !290
  store i32 %26, ptr %7, align 4, !dbg !290
  br label %27, !dbg !287

27:                                               ; preds = %42, %24
  %28 = load i32, ptr %7, align 4, !dbg !290
  %29 = load i32, ptr %10, align 4, !dbg !290
  %30 = icmp sle i32 %28, %29, !dbg !287
  br i1 %30, label %31, label %45, !dbg !287

31:                                               ; preds = %27
  %32 = load i32, ptr %7, align 4, !dbg !290
  %33 = mul nsw i32 %32, 1, !dbg !298
  %34 = add nsw i32 0, %33, !dbg !298
  store i32 %34, ptr %14, align 4, !dbg !298
  %35 = load i32, ptr %14, align 4, !dbg !299
  %36 = add nsw i32 %35, 1, !dbg !301
  %37 = sitofp i32 %36 to double, !dbg !302
  %38 = fdiv double 1.000000e+00, %37, !dbg !303
  %39 = load double, ptr %13, align 8, !dbg !304
  %40 = fadd double %39, %38, !dbg !304
  store double %40, ptr %13, align 8, !dbg !304
  br label %41, !dbg !305

41:                                               ; preds = %31
  br label %42, !dbg !297

42:                                               ; preds = %41
  %43 = load i32, ptr %7, align 4, !dbg !290
  %44 = add nsw i32 %43, 1, !dbg !287
  store i32 %44, ptr %7, align 4, !dbg !287
  br label %27, !dbg !297, !llvm.loop !306

45:                                               ; preds = %27
  br label %46, !dbg !297

46:                                               ; preds = %45
  call void @__kmpc_for_static_fini(ptr @3, i32 %18), !dbg !307
  %47 = getelementptr inbounds [1 x ptr], ptr %15, i64 0, i64 0, !dbg !297
  store ptr %13, ptr %47, align 8, !dbg !297
  %48 = call i32 @__kmpc_reduce_nowait(ptr @4, i32 %18, i32 1, i64 8, ptr %15, ptr @main.omp_outlined_debug__.omp.reduction.reduction_func, ptr @.gomp_critical_user_.reduction.var), !dbg !297
  switch i32 %48, label %56 [
    i32 1, label %49
    i32 2, label %53
  ], !dbg !297

49:                                               ; preds = %46
  %50 = load double, ptr %16, align 8, !dbg !295
  %51 = load double, ptr %13, align 8, !dbg !295
  %52 = fadd double %50, %51, !dbg !308
  store double %52, ptr %16, align 8, !dbg !308
  call void @__kmpc_end_reduce_nowait(ptr @4, i32 %18, ptr @.gomp_critical_user_.reduction.var), !dbg !297
  br label %56, !dbg !297

53:                                               ; preds = %46
  %54 = load double, ptr %13, align 8, !dbg !295
  %55 = atomicrmw fadd ptr %16, double %54 monotonic, align 8, !dbg !297
  br label %56, !dbg !297

56:                                               ; preds = %53, %49, %46
  ret void, !dbg !309
}

; Function Attrs: nounwind
declare void @__kmpc_for_static_init_4(ptr, i32, i32, ptr, ptr, ptr, ptr, i32, i32) #3

; Function Attrs: nounwind
declare void @__kmpc_for_static_fini(ptr, i32) #3

; Function Attrs: noinline norecurse uwtable
define internal void @main.omp_outlined_debug__.omp.reduction.reduction_func(ptr noundef %0, ptr noundef %1) #4 !dbg !310 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  call void @llvm.dbg.declare(metadata ptr %3, metadata !312, metadata !DIExpression()), !dbg !313
  store ptr %1, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !314, metadata !DIExpression()), !dbg !313
  %5 = load ptr, ptr %3, align 8, !dbg !315
  %6 = load ptr, ptr %4, align 8, !dbg !315
  %7 = getelementptr inbounds [1 x ptr], ptr %6, i64 0, i64 0, !dbg !315
  %8 = load ptr, ptr %7, align 8, !dbg !315
  %9 = getelementptr inbounds [1 x ptr], ptr %5, i64 0, i64 0, !dbg !315
  %10 = load ptr, ptr %9, align 8, !dbg !315
  %11 = load double, ptr %10, align 8, !dbg !316
  %12 = load double, ptr %8, align 8, !dbg !316
  %13 = fadd double %11, %12, !dbg !317
  store double %13, ptr %10, align 8, !dbg !317
  ret void, !dbg !316
}

; Function Attrs: convergent nounwind
declare i32 @__kmpc_reduce_nowait(ptr, i32, i32, i64, ptr, ptr, ptr) #5

; Function Attrs: convergent nounwind
declare void @__kmpc_end_reduce_nowait(ptr, i32, ptr) #5

; Function Attrs: noinline norecurse nounwind optnone uwtable
define internal void @main.omp_outlined(ptr noalias noundef %0, ptr noalias noundef %1, ptr noundef nonnull align 8 dereferenceable(8) %2) #2 !dbg !318 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  store ptr %0, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !319, metadata !DIExpression()), !dbg !320
  store ptr %1, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !321, metadata !DIExpression()), !dbg !320
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !322, metadata !DIExpression()), !dbg !320
  %7 = load ptr, ptr %6, align 8, !dbg !323
  %8 = load ptr, ptr %4, align 8, !dbg !323
  %9 = load ptr, ptr %5, align 8, !dbg !323
  %10 = load ptr, ptr %6, align 8, !dbg !323
  call void @main.omp_outlined_debug__(ptr %8, ptr %9, ptr %10) #3, !dbg !323
  ret void, !dbg !323
}

; Function Attrs: nounwind
declare !callback !324 void @__kmpc_fork_call(ptr, i32, ptr, ...) #3

declare i32 @printf(ptr noundef, ...) #6

attributes #0 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { noinline norecurse nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }
attributes #4 = { noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { convergent nounwind }
attributes #6 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!8}
!llvm.module.flags = !{!257, !258, !259, !260, !261, !262, !263, !264}
!llvm.ident = !{!265}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 10, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "test1.cpp", directory: "/mnt/c/llvm-clang", checksumkind: CSK_MD5, checksum: "188c71fb163ae20b4a0f7eeee8d211e4")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 152, elements: !6)
!4 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !5)
!5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!6 = !{!7}
!7 = !DISubrange(count: 19)
!8 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !2, producer: "Ubuntu clang version 18.1.3 (1ubuntu1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !9, imports: !10, splitDebugInlining: false, nameTableKind: None)
!9 = !{!0}
!10 = !{!11, !19, !23, !30, !34, !42, !47, !49, !55, !59, !63, !73, !75, !79, !83, !87, !92, !96, !100, !104, !108, !116, !120, !124, !126, !130, !134, !139, !145, !149, !153, !155, !163, !167, !175, !177, !181, !185, !189, !193, !198, !203, !208, !209, !210, !211, !213, !214, !215, !216, !217, !218, !219, !221, !222, !223, !224, !225, !226, !227, !228, !233, !234, !235, !236, !237, !238, !239, !240, !241, !242, !243, !244, !245, !246, !247, !248, !249, !250, !251, !252, !253, !254, !255, !256}
!11 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !13, file: !18, line: 52)
!12 = !DINamespace(name: "std", scope: null)
!13 = !DISubprogram(name: "abs", scope: !14, file: !14, line: 980, type: !15, flags: DIFlagPrototyped, spFlags: 0)
!14 = !DIFile(filename: "/usr/include/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "7fa2ecb2348a66f8b44ab9a15abd0b72")
!15 = !DISubroutineType(types: !16)
!16 = !{!17, !17}
!17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!18 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/std_abs.h", directory: "")
!19 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !20, file: !22, line: 131)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "div_t", file: !14, line: 63, baseType: !21)
!21 = !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 59, size: 64, flags: DIFlagFwdDecl, identifier: "_ZTS5div_t")
!22 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/cstdlib", directory: "")
!23 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !24, file: !22, line: 132)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "ldiv_t", file: !14, line: 71, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 67, size: 128, flags: DIFlagTypePassByValue, elements: !26, identifier: "_ZTS6ldiv_t")
!26 = !{!27, !29}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !25, file: !14, line: 69, baseType: !28, size: 64)
!28 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !25, file: !14, line: 70, baseType: !28, size: 64, offset: 64)
!30 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !31, file: !22, line: 134)
!31 = !DISubprogram(name: "abort", scope: !14, file: !14, line: 730, type: !32, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!32 = !DISubroutineType(types: !33)
!33 = !{null}
!34 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !35, file: !22, line: 136)
!35 = !DISubprogram(name: "aligned_alloc", scope: !14, file: !14, line: 724, type: !36, flags: DIFlagPrototyped, spFlags: 0)
!36 = !DISubroutineType(types: !37)
!37 = !{!38, !39, !39}
!38 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!39 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !40, line: 18, baseType: !41)
!40 = !DIFile(filename: "/usr/lib/llvm-18/lib/clang/18/include/__stddef_size_t.h", directory: "", checksumkind: CSK_MD5, checksum: "2c44e821a2b1951cde2eb0fb2e656867")
!41 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!42 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !43, file: !22, line: 138)
!43 = !DISubprogram(name: "atexit", scope: !14, file: !14, line: 734, type: !44, flags: DIFlagPrototyped, spFlags: 0)
!44 = !DISubroutineType(types: !45)
!45 = !{!17, !46}
!46 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !32, size: 64)
!47 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !48, file: !22, line: 141)
!48 = !DISubprogram(name: "at_quick_exit", scope: !14, file: !14, line: 739, type: !44, flags: DIFlagPrototyped, spFlags: 0)
!49 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !50, file: !22, line: 144)
!50 = !DISubprogram(name: "atof", scope: !14, file: !14, line: 102, type: !51, flags: DIFlagPrototyped, spFlags: 0)
!51 = !DISubroutineType(types: !52)
!52 = !{!53, !54}
!53 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!54 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!55 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !56, file: !22, line: 145)
!56 = !DISubprogram(name: "atoi", scope: !14, file: !14, line: 105, type: !57, flags: DIFlagPrototyped, spFlags: 0)
!57 = !DISubroutineType(types: !58)
!58 = !{!17, !54}
!59 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !60, file: !22, line: 146)
!60 = !DISubprogram(name: "atol", scope: !14, file: !14, line: 108, type: !61, flags: DIFlagPrototyped, spFlags: 0)
!61 = !DISubroutineType(types: !62)
!62 = !{!28, !54}
!63 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !64, file: !22, line: 147)
!64 = !DISubprogram(name: "bsearch", scope: !14, file: !14, line: 960, type: !65, flags: DIFlagPrototyped, spFlags: 0)
!65 = !DISubroutineType(types: !66)
!66 = !{!38, !67, !67, !39, !39, !69}
!67 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !68, size: 64)
!68 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!69 = !DIDerivedType(tag: DW_TAG_typedef, name: "__compar_fn_t", file: !14, line: 948, baseType: !70)
!70 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !71, size: 64)
!71 = !DISubroutineType(types: !72)
!72 = !{!17, !67, !67}
!73 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !74, file: !22, line: 148)
!74 = !DISubprogram(name: "calloc", scope: !14, file: !14, line: 675, type: !36, flags: DIFlagPrototyped, spFlags: 0)
!75 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !76, file: !22, line: 149)
!76 = !DISubprogram(name: "div", scope: !14, file: !14, line: 992, type: !77, flags: DIFlagPrototyped, spFlags: 0)
!77 = !DISubroutineType(types: !78)
!78 = !{!20, !17, !17}
!79 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !80, file: !22, line: 150)
!80 = !DISubprogram(name: "exit", scope: !14, file: !14, line: 756, type: !81, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!81 = !DISubroutineType(types: !82)
!82 = !{null, !17}
!83 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !84, file: !22, line: 151)
!84 = !DISubprogram(name: "free", scope: !14, file: !14, line: 687, type: !85, flags: DIFlagPrototyped, spFlags: 0)
!85 = !DISubroutineType(types: !86)
!86 = !{null, !38}
!87 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !88, file: !22, line: 152)
!88 = !DISubprogram(name: "getenv", scope: !14, file: !14, line: 773, type: !89, flags: DIFlagPrototyped, spFlags: 0)
!89 = !DISubroutineType(types: !90)
!90 = !{!91, !54}
!91 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!92 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !93, file: !22, line: 153)
!93 = !DISubprogram(name: "labs", scope: !14, file: !14, line: 981, type: !94, flags: DIFlagPrototyped, spFlags: 0)
!94 = !DISubroutineType(types: !95)
!95 = !{!28, !28}
!96 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !97, file: !22, line: 154)
!97 = !DISubprogram(name: "ldiv", scope: !14, file: !14, line: 994, type: !98, flags: DIFlagPrototyped, spFlags: 0)
!98 = !DISubroutineType(types: !99)
!99 = !{!24, !28, !28}
!100 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !101, file: !22, line: 155)
!101 = !DISubprogram(name: "malloc", scope: !14, file: !14, line: 672, type: !102, flags: DIFlagPrototyped, spFlags: 0)
!102 = !DISubroutineType(types: !103)
!103 = !{!38, !39}
!104 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !105, file: !22, line: 157)
!105 = !DISubprogram(name: "mblen", scope: !14, file: !14, line: 1062, type: !106, flags: DIFlagPrototyped, spFlags: 0)
!106 = !DISubroutineType(types: !107)
!107 = !{!17, !54, !39}
!108 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !109, file: !22, line: 158)
!109 = !DISubprogram(name: "mbstowcs", scope: !14, file: !14, line: 1073, type: !110, flags: DIFlagPrototyped, spFlags: 0)
!110 = !DISubroutineType(types: !111)
!111 = !{!39, !112, !115, !39}
!112 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !113)
!113 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !114, size: 64)
!114 = !DIBasicType(name: "wchar_t", size: 32, encoding: DW_ATE_signed)
!115 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !54)
!116 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !117, file: !22, line: 159)
!117 = !DISubprogram(name: "mbtowc", scope: !14, file: !14, line: 1065, type: !118, flags: DIFlagPrototyped, spFlags: 0)
!118 = !DISubroutineType(types: !119)
!119 = !{!17, !112, !115, !39}
!120 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !121, file: !22, line: 161)
!121 = !DISubprogram(name: "qsort", scope: !14, file: !14, line: 970, type: !122, flags: DIFlagPrototyped, spFlags: 0)
!122 = !DISubroutineType(types: !123)
!123 = !{null, !38, !39, !39, !69}
!124 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !125, file: !22, line: 164)
!125 = !DISubprogram(name: "quick_exit", scope: !14, file: !14, line: 762, type: !81, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!126 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !127, file: !22, line: 167)
!127 = !DISubprogram(name: "rand", scope: !14, file: !14, line: 573, type: !128, flags: DIFlagPrototyped, spFlags: 0)
!128 = !DISubroutineType(types: !129)
!129 = !{!17}
!130 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !131, file: !22, line: 168)
!131 = !DISubprogram(name: "realloc", scope: !14, file: !14, line: 683, type: !132, flags: DIFlagPrototyped, spFlags: 0)
!132 = !DISubroutineType(types: !133)
!133 = !{!38, !38, !39}
!134 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !135, file: !22, line: 169)
!135 = !DISubprogram(name: "srand", scope: !14, file: !14, line: 575, type: !136, flags: DIFlagPrototyped, spFlags: 0)
!136 = !DISubroutineType(types: !137)
!137 = !{null, !138}
!138 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!139 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !140, file: !22, line: 170)
!140 = !DISubprogram(name: "strtod", scope: !14, file: !14, line: 118, type: !141, flags: DIFlagPrototyped, spFlags: 0)
!141 = !DISubroutineType(types: !142)
!142 = !{!53, !115, !143}
!143 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !144)
!144 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !91, size: 64)
!145 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !146, file: !22, line: 171)
!146 = !DISubprogram(name: "strtol", linkageName: "__isoc23_strtol", scope: !14, file: !14, line: 215, type: !147, flags: DIFlagPrototyped, spFlags: 0)
!147 = !DISubroutineType(types: !148)
!148 = !{!28, !115, !143, !17}
!149 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !150, file: !22, line: 172)
!150 = !DISubprogram(name: "strtoul", linkageName: "__isoc23_strtoul", scope: !14, file: !14, line: 219, type: !151, flags: DIFlagPrototyped, spFlags: 0)
!151 = !DISubroutineType(types: !152)
!152 = !{!41, !115, !143, !17}
!153 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !154, file: !22, line: 173)
!154 = !DISubprogram(name: "system", scope: !14, file: !14, line: 923, type: !57, flags: DIFlagPrototyped, spFlags: 0)
!155 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !156, file: !22, line: 175)
!156 = !DISubprogram(name: "wcstombs", scope: !14, file: !14, line: 1077, type: !157, flags: DIFlagPrototyped, spFlags: 0)
!157 = !DISubroutineType(types: !158)
!158 = !{!39, !159, !160, !39}
!159 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !91)
!160 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !161)
!161 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !162, size: 64)
!162 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !114)
!163 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !164, file: !22, line: 176)
!164 = !DISubprogram(name: "wctomb", scope: !14, file: !14, line: 1069, type: !165, flags: DIFlagPrototyped, spFlags: 0)
!165 = !DISubroutineType(types: !166)
!166 = !{!17, !91, !114}
!167 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !169, file: !22, line: 204)
!168 = !DINamespace(name: "__gnu_cxx", scope: null)
!169 = !DIDerivedType(tag: DW_TAG_typedef, name: "lldiv_t", file: !14, line: 81, baseType: !170)
!170 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 77, size: 128, flags: DIFlagTypePassByValue, elements: !171, identifier: "_ZTS7lldiv_t")
!171 = !{!172, !174}
!172 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !170, file: !14, line: 79, baseType: !173, size: 64)
!173 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!174 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !170, file: !14, line: 80, baseType: !173, size: 64, offset: 64)
!175 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !176, file: !22, line: 210)
!176 = !DISubprogram(name: "_Exit", scope: !14, file: !14, line: 768, type: !81, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!177 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !178, file: !22, line: 214)
!178 = !DISubprogram(name: "llabs", scope: !14, file: !14, line: 984, type: !179, flags: DIFlagPrototyped, spFlags: 0)
!179 = !DISubroutineType(types: !180)
!180 = !{!173, !173}
!181 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !182, file: !22, line: 220)
!182 = !DISubprogram(name: "lldiv", scope: !14, file: !14, line: 998, type: !183, flags: DIFlagPrototyped, spFlags: 0)
!183 = !DISubroutineType(types: !184)
!184 = !{!169, !173, !173}
!185 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !186, file: !22, line: 231)
!186 = !DISubprogram(name: "atoll", scope: !14, file: !14, line: 113, type: !187, flags: DIFlagPrototyped, spFlags: 0)
!187 = !DISubroutineType(types: !188)
!188 = !{!173, !54}
!189 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !190, file: !22, line: 232)
!190 = !DISubprogram(name: "strtoll", linkageName: "__isoc23_strtoll", scope: !14, file: !14, line: 238, type: !191, flags: DIFlagPrototyped, spFlags: 0)
!191 = !DISubroutineType(types: !192)
!192 = !{!173, !115, !143, !17}
!193 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !194, file: !22, line: 233)
!194 = !DISubprogram(name: "strtoull", linkageName: "__isoc23_strtoull", scope: !14, file: !14, line: 243, type: !195, flags: DIFlagPrototyped, spFlags: 0)
!195 = !DISubroutineType(types: !196)
!196 = !{!197, !115, !143, !17}
!197 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!198 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !199, file: !22, line: 235)
!199 = !DISubprogram(name: "strtof", scope: !14, file: !14, line: 124, type: !200, flags: DIFlagPrototyped, spFlags: 0)
!200 = !DISubroutineType(types: !201)
!201 = !{!202, !115, !143}
!202 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!203 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !168, entity: !204, file: !22, line: 236)
!204 = !DISubprogram(name: "strtold", scope: !14, file: !14, line: 127, type: !205, flags: DIFlagPrototyped, spFlags: 0)
!205 = !DISubroutineType(types: !206)
!206 = !{!207, !115, !143}
!207 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!208 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !169, file: !22, line: 244)
!209 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !176, file: !22, line: 246)
!210 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !178, file: !22, line: 248)
!211 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !212, file: !22, line: 249)
!212 = !DISubprogram(name: "div", linkageName: "_ZN9__gnu_cxx3divExx", scope: !168, file: !22, line: 217, type: !183, flags: DIFlagPrototyped, spFlags: 0)
!213 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !182, file: !22, line: 250)
!214 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !186, file: !22, line: 252)
!215 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !199, file: !22, line: 253)
!216 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !190, file: !22, line: 254)
!217 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !194, file: !22, line: 255)
!218 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !204, file: !22, line: 256)
!219 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !31, file: !220, line: 38)
!220 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "3f24ff2a8eef595875da96e5466bd4aa")
!221 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !43, file: !220, line: 39)
!222 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !80, file: !220, line: 40)
!223 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !48, file: !220, line: 43)
!224 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !125, file: !220, line: 46)
!225 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !176, file: !220, line: 49)
!226 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !20, file: !220, line: 54)
!227 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !24, file: !220, line: 55)
!228 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !229, file: !220, line: 57)
!229 = !DISubprogram(name: "abs", linkageName: "_ZSt3absg", scope: !12, file: !18, line: 137, type: !230, flags: DIFlagPrototyped, spFlags: 0)
!230 = !DISubroutineType(types: !231)
!231 = !{!232, !232}
!232 = !DIBasicType(name: "__float128", size: 128, encoding: DW_ATE_float)
!233 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !50, file: !220, line: 58)
!234 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !56, file: !220, line: 59)
!235 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !60, file: !220, line: 60)
!236 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !64, file: !220, line: 61)
!237 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !74, file: !220, line: 62)
!238 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !212, file: !220, line: 63)
!239 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !84, file: !220, line: 64)
!240 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !88, file: !220, line: 65)
!241 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !93, file: !220, line: 66)
!242 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !97, file: !220, line: 67)
!243 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !101, file: !220, line: 68)
!244 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !105, file: !220, line: 70)
!245 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !109, file: !220, line: 71)
!246 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !117, file: !220, line: 72)
!247 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !121, file: !220, line: 74)
!248 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !127, file: !220, line: 75)
!249 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !131, file: !220, line: 76)
!250 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !135, file: !220, line: 77)
!251 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !140, file: !220, line: 78)
!252 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !146, file: !220, line: 79)
!253 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !150, file: !220, line: 80)
!254 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !154, file: !220, line: 81)
!255 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !156, file: !220, line: 83)
!256 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !164, file: !220, line: 84)
!257 = !{i32 7, !"Dwarf Version", i32 5}
!258 = !{i32 2, !"Debug Info Version", i32 3}
!259 = !{i32 1, !"wchar_size", i32 4}
!260 = !{i32 7, !"openmp", i32 51}
!261 = !{i32 8, !"PIC Level", i32 2}
!262 = !{i32 7, !"PIE Level", i32 2}
!263 = !{i32 7, !"uwtable", i32 2}
!264 = !{i32 7, !"frame-pointer", i32 2}
!265 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!266 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 4, type: !128, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !8, retainedNodes: !267)
!267 = !{}
!268 = !DILocalVariable(name: "sum", scope: !266, file: !2, line: 5, type: !53)
!269 = !DILocation(line: 5, column: 12, scope: !266)
!270 = !DILocation(line: 6, column: 1, scope: !266)
!271 = !DILocation(line: 10, column: 35, scope: !266)
!272 = !DILocation(line: 10, column: 5, scope: !266)
!273 = !DILocation(line: 11, column: 5, scope: !266)
!274 = distinct !DISubprogram(name: "main.omp_outlined_debug__", scope: !2, file: !2, line: 7, type: !275, scopeLine: 7, flags: DIFlagArtificial | DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !8, retainedNodes: !267)
!275 = !DISubroutineType(types: !276)
!276 = !{null, !277, !277, !281}
!277 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !278)
!278 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !279)
!279 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !280, size: 64)
!280 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !17)
!281 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !53, size: 64)
!282 = !DILocalVariable(name: ".global_tid.", arg: 1, scope: !274, type: !277, flags: DIFlagArtificial)
!283 = !DILocation(line: 0, scope: !274)
!284 = !DILocalVariable(name: ".bound_tid.", arg: 2, scope: !274, type: !277, flags: DIFlagArtificial)
!285 = !DILocalVariable(name: "sum", arg: 3, scope: !274, file: !2, line: 5, type: !281)
!286 = !DILocation(line: 5, column: 12, scope: !274)
!287 = !DILocation(line: 7, column: 5, scope: !274)
!288 = !DILocalVariable(name: ".omp.iv", scope: !274, type: !17, flags: DIFlagArtificial)
!289 = !DILocalVariable(name: ".omp.lb", scope: !274, type: !17, flags: DIFlagArtificial)
!290 = !DILocation(line: 7, column: 10, scope: !274)
!291 = !DILocalVariable(name: ".omp.ub", scope: !274, type: !17, flags: DIFlagArtificial)
!292 = !DILocalVariable(name: ".omp.stride", scope: !274, type: !17, flags: DIFlagArtificial)
!293 = !DILocalVariable(name: ".omp.is_last", scope: !274, type: !17, flags: DIFlagArtificial)
!294 = !DILocalVariable(name: "sum", scope: !274, type: !53, flags: DIFlagArtificial)
!295 = !DILocation(line: 6, column: 38, scope: !274)
!296 = !DILocalVariable(name: "i", scope: !274, type: !17, flags: DIFlagArtificial)
!297 = !DILocation(line: 6, column: 1, scope: !274)
!298 = !DILocation(line: 7, column: 30, scope: !274)
!299 = !DILocation(line: 8, column: 23, scope: !300)
!300 = distinct !DILexicalBlock(scope: !274, file: !2, line: 7, column: 35)
!301 = !DILocation(line: 8, column: 25, scope: !300)
!302 = !DILocation(line: 8, column: 22, scope: !300)
!303 = !DILocation(line: 8, column: 20, scope: !300)
!304 = !DILocation(line: 8, column: 13, scope: !300)
!305 = !DILocation(line: 9, column: 5, scope: !300)
!306 = distinct !{!306, !297, !307}
!307 = !DILocation(line: 6, column: 42, scope: !274)
!308 = !DILocation(line: 6, column: 36, scope: !274)
!309 = !DILocation(line: 9, column: 5, scope: !274)
!310 = distinct !DISubprogram(linkageName: "main.omp_outlined_debug__.omp.reduction.reduction_func", scope: !2, file: !2, line: 6, type: !311, scopeLine: 6, flags: DIFlagArtificial, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !8, retainedNodes: !267)
!311 = !DISubroutineType(types: !267)
!312 = !DILocalVariable(arg: 1, scope: !310, type: !38, flags: DIFlagArtificial)
!313 = !DILocation(line: 0, scope: !310)
!314 = !DILocalVariable(arg: 2, scope: !310, type: !38, flags: DIFlagArtificial)
!315 = !DILocation(line: 6, column: 42, scope: !310)
!316 = !DILocation(line: 6, column: 38, scope: !310)
!317 = !DILocation(line: 6, column: 36, scope: !310)
!318 = distinct !DISubprogram(name: "main.omp_outlined", scope: !2, file: !2, line: 6, type: !275, scopeLine: 6, flags: DIFlagArtificial | DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !8, retainedNodes: !267)
!319 = !DILocalVariable(name: ".global_tid.", arg: 1, scope: !318, type: !277, flags: DIFlagArtificial)
!320 = !DILocation(line: 0, scope: !318)
!321 = !DILocalVariable(name: ".bound_tid.", arg: 2, scope: !318, type: !277, flags: DIFlagArtificial)
!322 = !DILocalVariable(name: "sum", arg: 3, scope: !318, type: !281, flags: DIFlagArtificial)
!323 = !DILocation(line: 6, column: 1, scope: !318)
!324 = !{!325}
!325 = !{i64 2, i64 -1, i64 -1, i1 true}
