; ModuleID = 'test2.cpp'
source_filename = "test2.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.ident_t = type { i32, i32, i32, i32, ptr }

@0 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;20;1;;\00", align 1
@1 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @0 }, align 8
@2 = private unnamed_addr constant [24 x i8] c";test2.cpp;main;20;33;;\00", align 1
@3 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 23, ptr @2 }, align 8
@.gomp_critical_user_.reduction.var = common global [8 x i32] zeroinitializer, align 8
@4 = private unnamed_addr constant %struct.ident_t { i32 0, i32 18, i32 0, i32 23, ptr @2 }, align 8
@5 = private unnamed_addr constant %struct.ident_t { i32 0, i32 66, i32 0, i32 22, ptr @0 }, align 8
@6 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;26;1;;\00", align 1
@7 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @6 }, align 8
@8 = private unnamed_addr constant [24 x i8] c";test2.cpp;main;26;37;;\00", align 1
@9 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 23, ptr @8 }, align 8
@10 = private unnamed_addr constant %struct.ident_t { i32 0, i32 18, i32 0, i32 23, ptr @8 }, align 8
@11 = private unnamed_addr constant %struct.ident_t { i32 0, i32 66, i32 0, i32 22, ptr @6 }, align 8
@12 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;32;1;;\00", align 1
@13 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @12 }, align 8
@14 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;34;1;;\00", align 1
@15 = private unnamed_addr constant %struct.ident_t { i32 0, i32 2, i32 0, i32 22, ptr @14 }, align 8
@.gomp_critical_user_.var = common global [8 x i32] zeroinitializer, align 8
@16 = private unnamed_addr constant [24 x i8] c";test2.cpp;main;32;16;;\00", align 1
@17 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 23, ptr @16 }, align 8
@18 = private unnamed_addr constant %struct.ident_t { i32 0, i32 66, i32 0, i32 22, ptr @12 }, align 8
@19 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;41;1;;\00", align 1
@20 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @19 }, align 8
@21 = private unnamed_addr constant [24 x i8] c";test2.cpp;main;41;16;;\00", align 1
@22 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 23, ptr @21 }, align 8
@23 = private unnamed_addr constant %struct.ident_t { i32 0, i32 66, i32 0, i32 22, ptr @19 }, align 8
@24 = private unnamed_addr constant [23 x i8] c";test2.cpp;main;17;1;;\00", align 1
@25 = private unnamed_addr constant %struct.ident_t { i32 0, i32 2, i32 0, i32 22, ptr @24 }, align 8
@.str = private unnamed_addr constant [12 x i8] c"Sum = %lld\0A\00", align 1, !dbg !0
@.str.2 = private unnamed_addr constant [37 x i8] c"Product of first 20 elements = %lld\0A\00", align 1, !dbg !8
@.str.3 = private unnamed_addr constant [16 x i8] c"Max value = %d\0A\00", align 1, !dbg !13
@.str.4 = private unnamed_addr constant [17 x i8] c"Even count = %d\0A\00", align 1, !dbg !18

; Function Attrs: mustprogress noinline norecurse optnone uwtable
define dso_local noundef i32 @main() #0 !dbg !283 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !285, metadata !DIExpression()), !dbg !286
  %8 = call noalias ptr @malloc(i64 noundef 4000) #9, !dbg !287
  store ptr %8, ptr %2, align 8, !dbg !286
  call void @llvm.dbg.declare(metadata ptr %3, metadata !288, metadata !DIExpression()), !dbg !290
  store i32 0, ptr %3, align 4, !dbg !290
  br label %9, !dbg !291

9:                                                ; preds = %19, %0
  %10 = load i32, ptr %3, align 4, !dbg !292
  %11 = icmp slt i32 %10, 1000, !dbg !294
  br i1 %11, label %12, label %22, !dbg !295

12:                                               ; preds = %9
  %13 = load i32, ptr %3, align 4, !dbg !296
  %14 = add nsw i32 %13, 1, !dbg !297
  %15 = load ptr, ptr %2, align 8, !dbg !298
  %16 = load i32, ptr %3, align 4, !dbg !299
  %17 = sext i32 %16 to i64, !dbg !298
  %18 = getelementptr inbounds i32, ptr %15, i64 %17, !dbg !298
  store i32 %14, ptr %18, align 4, !dbg !300
  br label %19, !dbg !298

19:                                               ; preds = %12
  %20 = load i32, ptr %3, align 4, !dbg !301
  %21 = add nsw i32 %20, 1, !dbg !301
  store i32 %21, ptr %3, align 4, !dbg !301
  br label %9, !dbg !302, !llvm.loop !303

22:                                               ; preds = %9
  call void @llvm.dbg.declare(metadata ptr %4, metadata !306, metadata !DIExpression()), !dbg !307
  store i64 0, ptr %4, align 8, !dbg !307
  call void @llvm.dbg.declare(metadata ptr %5, metadata !308, metadata !DIExpression()), !dbg !309
  store i64 1, ptr %5, align 8, !dbg !309
  call void @llvm.dbg.declare(metadata ptr %6, metadata !310, metadata !DIExpression()), !dbg !311
  store i32 0, ptr %6, align 4, !dbg !311
  call void @llvm.dbg.declare(metadata ptr %7, metadata !312, metadata !DIExpression()), !dbg !313
  store i32 0, ptr %7, align 4, !dbg !313
; OpenMP: [Line 17] #pragma omp parallel
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @25, i32 5, ptr @main.omp_outlined, ptr %4, ptr %2, ptr %5, ptr %6, ptr %7), !dbg !314
  %23 = load i64, ptr %4, align 8, !dbg !315
  %24 = call i32 (ptr, ...) @printf(ptr noundef @.str, i64 noundef %23), !dbg !316
  %25 = load i64, ptr %5, align 8, !dbg !317
  %26 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i64 noundef %25), !dbg !318
  %27 = load i32, ptr %6, align 4, !dbg !319
  %28 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, i32 noundef %27), !dbg !320
  %29 = load i32, ptr %7, align 4, !dbg !321
  %30 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %29), !dbg !322
  %31 = load ptr, ptr %2, align 8, !dbg !323
  call void @free(ptr noundef %31) #4, !dbg !324
  ret i32 0, !dbg !325
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind allocsize(0)
declare noalias ptr @malloc(i64 noundef) #2

; Function Attrs: noinline norecurse nounwind optnone uwtable
define internal void @main.omp_outlined_debug__(ptr noalias noundef %0, ptr noalias noundef %1, ptr noundef nonnull align 8 dereferenceable(8) %2, ptr noundef nonnull align 8 dereferenceable(8) %3, ptr noundef nonnull align 8 dereferenceable(8) %4, ptr noundef nonnull align 4 dereferenceable(4) %5, ptr noundef nonnull align 4 dereferenceable(4) %6) #3 !dbg !326 {
  %8 = alloca ptr, align 8
  %9 = alloca ptr, align 8
  %10 = alloca ptr, align 8
  %11 = alloca ptr, align 8
  %12 = alloca ptr, align 8
  %13 = alloca ptr, align 8
  %14 = alloca ptr, align 8
  %15 = alloca i32, align 4
  %16 = alloca i32, align 4
  %17 = alloca i32, align 4
  %18 = alloca i32, align 4
  %19 = alloca i32, align 4
  %20 = alloca i32, align 4
  %21 = alloca i64, align 8
  %22 = alloca i32, align 4
  %23 = alloca [1 x ptr], align 8
  %24 = alloca i32, align 4
  %25 = alloca i32, align 4
  %26 = alloca i32, align 4
  %27 = alloca i32, align 4
  %28 = alloca i32, align 4
  %29 = alloca i32, align 4
  %30 = alloca i64, align 8
  %31 = alloca i32, align 4
  %32 = alloca [1 x ptr], align 8
  %33 = alloca i64, align 8
  %34 = alloca i64, align 8
  %35 = alloca i32, align 4
  %36 = alloca i32, align 4
  %37 = alloca i32, align 4
  %38 = alloca i32, align 4
  %39 = alloca i32, align 4
  %40 = alloca i32, align 4
  %41 = alloca i32, align 4
  %42 = alloca i32, align 4
  %43 = alloca i32, align 4
  %44 = alloca i32, align 4
  %45 = alloca i32, align 4
  %46 = alloca i32, align 4
  %47 = alloca i32, align 4
  %48 = alloca i32, align 4
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !336, metadata !DIExpression()), !dbg !337
  store ptr %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !338, metadata !DIExpression()), !dbg !337
  store ptr %2, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !339, metadata !DIExpression()), !dbg !340
  store ptr %3, ptr %11, align 8
  call void @llvm.dbg.declare(metadata ptr %11, metadata !341, metadata !DIExpression()), !dbg !342
  store ptr %4, ptr %12, align 8
  call void @llvm.dbg.declare(metadata ptr %12, metadata !343, metadata !DIExpression()), !dbg !344
  store ptr %5, ptr %13, align 8
  call void @llvm.dbg.declare(metadata ptr %13, metadata !345, metadata !DIExpression()), !dbg !346
  store ptr %6, ptr %14, align 8
  call void @llvm.dbg.declare(metadata ptr %14, metadata !347, metadata !DIExpression()), !dbg !348
  %49 = load ptr, ptr %10, align 8, !dbg !349
  %50 = load ptr, ptr %11, align 8, !dbg !349
  %51 = load ptr, ptr %12, align 8, !dbg !349
  %52 = load ptr, ptr %13, align 8, !dbg !349
  %53 = load ptr, ptr %14, align 8, !dbg !349
  call void @llvm.dbg.declare(metadata ptr %15, metadata !350, metadata !DIExpression()), !dbg !353
  call void @llvm.dbg.declare(metadata ptr %17, metadata !354, metadata !DIExpression()), !dbg !353
  store i32 0, ptr %17, align 4, !dbg !355
  call void @llvm.dbg.declare(metadata ptr %18, metadata !356, metadata !DIExpression()), !dbg !353
  store i32 999, ptr %18, align 4, !dbg !355
  call void @llvm.dbg.declare(metadata ptr %19, metadata !357, metadata !DIExpression()), !dbg !353
  store i32 1, ptr %19, align 4, !dbg !355
  call void @llvm.dbg.declare(metadata ptr %20, metadata !358, metadata !DIExpression()), !dbg !353
  store i32 0, ptr %20, align 4, !dbg !355
  call void @llvm.dbg.declare(metadata ptr %21, metadata !359, metadata !DIExpression()), !dbg !353
  store i64 0, ptr %21, align 8, !dbg !360
  call void @llvm.dbg.declare(metadata ptr %22, metadata !361, metadata !DIExpression()), !dbg !353
  %54 = load ptr, ptr %8, align 8, !dbg !362
  %55 = load i32, ptr %54, align 4, !dbg !362
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  call void @__kmpc_for_static_init_4(ptr @1, i32 %55, i32 34, ptr %20, ptr %17, ptr %18, ptr %19, i32 1, i32 1), !dbg !363
  %56 = load i32, ptr %18, align 4, !dbg !355
  %57 = icmp sgt i32 %56, 999, !dbg !355
  br i1 %57, label %58, label %59, !dbg !355

58:                                               ; preds = %7
  br label %61, !dbg !355

59:                                               ; preds = %7
  %60 = load i32, ptr %18, align 4, !dbg !355
  br label %61, !dbg !355

61:                                               ; preds = %59, %58
  %62 = phi i32 [ 999, %58 ], [ %60, %59 ], !dbg !355
  store i32 %62, ptr %18, align 4, !dbg !355
  %63 = load i32, ptr %17, align 4, !dbg !355
  store i32 %63, ptr %15, align 4, !dbg !355
  br label %64, !dbg !362

64:                                               ; preds = %81, %61
  %65 = load i32, ptr %15, align 4, !dbg !355
  %66 = load i32, ptr %18, align 4, !dbg !355
  %67 = icmp sle i32 %65, %66, !dbg !364
  br i1 %67, label %68, label %84, !dbg !362

68:                                               ; preds = %64
  %69 = load i32, ptr %15, align 4, !dbg !355
  %70 = mul nsw i32 %69, 1, !dbg !365
  %71 = add nsw i32 0, %70, !dbg !365
  store i32 %71, ptr %22, align 4, !dbg !365
  %72 = load ptr, ptr %50, align 8, !dbg !366
  %73 = load i32, ptr %22, align 4, !dbg !368
  %74 = sext i32 %73 to i64, !dbg !366
  %75 = getelementptr inbounds i32, ptr %72, i64 %74, !dbg !366
  %76 = load i32, ptr %75, align 4, !dbg !366
  %77 = sext i32 %76 to i64, !dbg !366
  %78 = load i64, ptr %21, align 8, !dbg !369
  %79 = add nsw i64 %78, %77, !dbg !369
  store i64 %79, ptr %21, align 8, !dbg !369
  br label %80, !dbg !370

80:                                               ; preds = %68
  br label %81, !dbg !363

81:                                               ; preds = %80
  %82 = load i32, ptr %15, align 4, !dbg !355
  %83 = add nsw i32 %82, 1, !dbg !364
  store i32 %83, ptr %15, align 4, !dbg !364
  br label %64, !dbg !363, !llvm.loop !371

84:                                               ; preds = %64
  br label %85, !dbg !363

85:                                               ; preds = %84
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  call void @__kmpc_for_static_fini(ptr @3, i32 %55), !dbg !372
  %86 = getelementptr inbounds [1 x ptr], ptr %23, i64 0, i64 0, !dbg !363
  store ptr %21, ptr %86, align 8, !dbg !363
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  %87 = call i32 @__kmpc_reduce(ptr @4, i32 %55, i32 1, i64 8, ptr %23, ptr @main.omp_outlined_debug__.omp.reduction.reduction_func, ptr @.gomp_critical_user_.reduction.var), !dbg !363
  switch i32 %87, label %95 [
    i32 1, label %88
    i32 2, label %92
  ], !dbg !363

88:                                               ; preds = %85
  %89 = load i64, ptr %49, align 8, !dbg !360
  %90 = load i64, ptr %21, align 8, !dbg !360
  %91 = add nsw i64 %89, %90, !dbg !373
  store i64 %91, ptr %49, align 8, !dbg !373
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  call void @__kmpc_end_reduce(ptr @4, i32 %55, ptr @.gomp_critical_user_.reduction.var), !dbg !363
  br label %95, !dbg !363

92:                                               ; preds = %85
  %93 = load i64, ptr %21, align 8, !dbg !360
  %94 = atomicrmw add ptr %49, i64 %93 monotonic, align 8, !dbg !363
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  call void @__kmpc_end_reduce(ptr @4, i32 %55, ptr @.gomp_critical_user_.reduction.var), !dbg !363
  br label %95, !dbg !363

95:                                               ; preds = %92, %88, %85
; OpenMP: [Line 20] #pragma omp for reduction(+:sum)
  call void @__kmpc_barrier(ptr @5, i32 %55), !dbg !372
  call void @llvm.dbg.declare(metadata ptr %24, metadata !374, metadata !DIExpression()), !dbg !376
  call void @llvm.dbg.declare(metadata ptr %26, metadata !377, metadata !DIExpression()), !dbg !376
  store i32 0, ptr %26, align 4, !dbg !378
  call void @llvm.dbg.declare(metadata ptr %27, metadata !379, metadata !DIExpression()), !dbg !376
  store i32 19, ptr %27, align 4, !dbg !378
  call void @llvm.dbg.declare(metadata ptr %28, metadata !380, metadata !DIExpression()), !dbg !376
  store i32 1, ptr %28, align 4, !dbg !378
  call void @llvm.dbg.declare(metadata ptr %29, metadata !381, metadata !DIExpression()), !dbg !376
  store i32 0, ptr %29, align 4, !dbg !378
  call void @llvm.dbg.declare(metadata ptr %30, metadata !382, metadata !DIExpression()), !dbg !376
  store i64 1, ptr %30, align 8, !dbg !383
  call void @llvm.dbg.declare(metadata ptr %31, metadata !384, metadata !DIExpression()), !dbg !376
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  call void @__kmpc_for_static_init_4(ptr @7, i32 %55, i32 34, ptr %29, ptr %26, ptr %27, ptr %28, i32 1, i32 1), !dbg !385
  %96 = load i32, ptr %27, align 4, !dbg !378
  %97 = icmp sgt i32 %96, 19, !dbg !378
  br i1 %97, label %98, label %99, !dbg !378

98:                                               ; preds = %95
  br label %101, !dbg !378

99:                                               ; preds = %95
  %100 = load i32, ptr %27, align 4, !dbg !378
  br label %101, !dbg !378

101:                                              ; preds = %99, %98
  %102 = phi i32 [ 19, %98 ], [ %100, %99 ], !dbg !378
  store i32 %102, ptr %27, align 4, !dbg !378
  %103 = load i32, ptr %26, align 4, !dbg !378
  store i32 %103, ptr %24, align 4, !dbg !378
  br label %104, !dbg !386

104:                                              ; preds = %121, %101
  %105 = load i32, ptr %24, align 4, !dbg !378
  %106 = load i32, ptr %27, align 4, !dbg !378
  %107 = icmp sle i32 %105, %106, !dbg !387
  br i1 %107, label %108, label %124, !dbg !386

108:                                              ; preds = %104
  %109 = load i32, ptr %24, align 4, !dbg !378
  %110 = mul nsw i32 %109, 1, !dbg !388
  %111 = add nsw i32 0, %110, !dbg !388
  store i32 %111, ptr %31, align 4, !dbg !388
  %112 = load ptr, ptr %50, align 8, !dbg !389
  %113 = load i32, ptr %31, align 4, !dbg !391
  %114 = sext i32 %113 to i64, !dbg !389
  %115 = getelementptr inbounds i32, ptr %112, i64 %114, !dbg !389
  %116 = load i32, ptr %115, align 4, !dbg !389
  %117 = sext i32 %116 to i64, !dbg !389
  %118 = load i64, ptr %30, align 8, !dbg !392
  %119 = mul nsw i64 %118, %117, !dbg !392
  store i64 %119, ptr %30, align 8, !dbg !392
  br label %120, !dbg !393

120:                                              ; preds = %108
  br label %121, !dbg !385

121:                                              ; preds = %120
  %122 = load i32, ptr %24, align 4, !dbg !378
  %123 = add nsw i32 %122, 1, !dbg !387
  store i32 %123, ptr %24, align 4, !dbg !387
  br label %104, !dbg !385, !llvm.loop !394

124:                                              ; preds = %104
  br label %125, !dbg !385

125:                                              ; preds = %124
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  call void @__kmpc_for_static_fini(ptr @9, i32 %55), !dbg !395
  %126 = getelementptr inbounds [1 x ptr], ptr %32, i64 0, i64 0, !dbg !385
  store ptr %30, ptr %126, align 8, !dbg !385
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  %127 = call i32 @__kmpc_reduce(ptr @10, i32 %55, i32 1, i64 8, ptr %32, ptr @main.omp_outlined_debug__.omp.reduction.reduction_func.1, ptr @.gomp_critical_user_.reduction.var), !dbg !385
  switch i32 %127, label %145 [
    i32 1, label %128
    i32 2, label %132
  ], !dbg !385

128:                                              ; preds = %125
  %129 = load i64, ptr %51, align 8, !dbg !383
  %130 = load i64, ptr %30, align 8, !dbg !383
  %131 = mul nsw i64 %129, %130, !dbg !396
  store i64 %131, ptr %51, align 8, !dbg !396
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  call void @__kmpc_end_reduce(ptr @10, i32 %55, ptr @.gomp_critical_user_.reduction.var), !dbg !385
  br label %145, !dbg !385

132:                                              ; preds = %125
  %133 = load i64, ptr %30, align 8, !dbg !383
  %134 = load atomic i64, ptr %51 monotonic, align 8, !dbg !385
  br label %135, !dbg !385

135:                                              ; preds = %135, %132
  %136 = phi i64 [ %134, %132 ], [ %142, %135 ], !dbg !385
  store i64 %136, ptr %34, align 8, !dbg !385
  %137 = load i64, ptr %34, align 8, !dbg !383
  %138 = load i64, ptr %30, align 8, !dbg !383
  %139 = mul nsw i64 %137, %138, !dbg !396
  store i64 %139, ptr %33, align 8, !dbg !385
  %140 = load i64, ptr %33, align 8, !dbg !385
  %141 = cmpxchg ptr %51, i64 %136, i64 %140 monotonic monotonic, align 8, !dbg !385
  %142 = extractvalue { i64, i1 } %141, 0, !dbg !385
  %143 = extractvalue { i64, i1 } %141, 1, !dbg !385
  br i1 %143, label %144, label %135, !dbg !385

144:                                              ; preds = %135
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  call void @__kmpc_end_reduce(ptr @10, i32 %55, ptr @.gomp_critical_user_.reduction.var), !dbg !385
  br label %145, !dbg !385

145:                                              ; preds = %144, %128, %125
; OpenMP: [Line 26] #pragma omp for reduction(*:product)
  call void @__kmpc_barrier(ptr @11, i32 %55), !dbg !395
  call void @llvm.dbg.declare(metadata ptr %35, metadata !397, metadata !DIExpression()), !dbg !399
  call void @llvm.dbg.declare(metadata ptr %37, metadata !400, metadata !DIExpression()), !dbg !399
  store i32 0, ptr %37, align 4, !dbg !401
  call void @llvm.dbg.declare(metadata ptr %38, metadata !402, metadata !DIExpression()), !dbg !399
  store i32 999, ptr %38, align 4, !dbg !401
  call void @llvm.dbg.declare(metadata ptr %39, metadata !403, metadata !DIExpression()), !dbg !399
  store i32 1, ptr %39, align 4, !dbg !401
  call void @llvm.dbg.declare(metadata ptr %40, metadata !404, metadata !DIExpression()), !dbg !399
  store i32 0, ptr %40, align 4, !dbg !401
  call void @llvm.dbg.declare(metadata ptr %41, metadata !405, metadata !DIExpression()), !dbg !399
; OpenMP: [Line 32] #pragma omp for
  call void @__kmpc_for_static_init_4(ptr @13, i32 %55, i32 34, ptr %40, ptr %37, ptr %38, ptr %39, i32 1, i32 1), !dbg !406
  %146 = load i32, ptr %38, align 4, !dbg !401
  %147 = icmp sgt i32 %146, 999, !dbg !401
  br i1 %147, label %148, label %149, !dbg !401

148:                                              ; preds = %145
  br label %151, !dbg !401

149:                                              ; preds = %145
  %150 = load i32, ptr %38, align 4, !dbg !401
  br label %151, !dbg !401

151:                                              ; preds = %149, %148
  %152 = phi i32 [ 999, %148 ], [ %150, %149 ], !dbg !401
  store i32 %152, ptr %38, align 4, !dbg !401
  %153 = load i32, ptr %37, align 4, !dbg !401
  store i32 %153, ptr %35, align 4, !dbg !401
  br label %154, !dbg !407

154:                                              ; preds = %177, %151
  %155 = load i32, ptr %35, align 4, !dbg !401
  %156 = load i32, ptr %38, align 4, !dbg !401
  %157 = icmp sle i32 %155, %156, !dbg !408
  br i1 %157, label %158, label %180, !dbg !407

158:                                              ; preds = %154
  %159 = load i32, ptr %35, align 4, !dbg !401
  %160 = mul nsw i32 %159, 1, !dbg !409
  %161 = add nsw i32 0, %160, !dbg !409
  store i32 %161, ptr %41, align 4, !dbg !409
; OpenMP: [Line 34] #pragma omp critical
  call void @__kmpc_critical(ptr @15, i32 %55, ptr @.gomp_critical_user_.var), !dbg !410
  %162 = load ptr, ptr %50, align 8, !dbg !413
  %163 = load i32, ptr %41, align 4, !dbg !416
  %164 = sext i32 %163 to i64, !dbg !413
  %165 = getelementptr inbounds i32, ptr %162, i64 %164, !dbg !413
  %166 = load i32, ptr %165, align 4, !dbg !413
  %167 = load i32, ptr %52, align 4, !dbg !417
  %168 = icmp sgt i32 %166, %167, !dbg !418
  br i1 %168, label %169, label %175, !dbg !419

169:                                              ; preds = %158
  %170 = load ptr, ptr %50, align 8, !dbg !420
  %171 = load i32, ptr %41, align 4, !dbg !421
  %172 = sext i32 %171 to i64, !dbg !420
  %173 = getelementptr inbounds i32, ptr %170, i64 %172, !dbg !420
  %174 = load i32, ptr %173, align 4, !dbg !420
  store i32 %174, ptr %52, align 4, !dbg !422
  br label %175, !dbg !423

175:                                              ; preds = %169, %158
; OpenMP: OpenMP runtime call
  call void @__kmpc_end_critical(ptr @15, i32 %55, ptr @.gomp_critical_user_.var), !dbg !424
  br label %176, !dbg !425

176:                                              ; preds = %175
  br label %177, !dbg !406

177:                                              ; preds = %176
  %178 = load i32, ptr %35, align 4, !dbg !401
  %179 = add nsw i32 %178, 1, !dbg !408
  store i32 %179, ptr %35, align 4, !dbg !408
  br label %154, !dbg !406, !llvm.loop !426

180:                                              ; preds = %154
  br label %181, !dbg !406

181:                                              ; preds = %180
; OpenMP: [Line 32] #pragma omp for
  call void @__kmpc_for_static_fini(ptr @17, i32 %55), !dbg !427
; OpenMP: [Line 32] #pragma omp for
  call void @__kmpc_barrier(ptr @18, i32 %55), !dbg !427
  call void @llvm.dbg.declare(metadata ptr %42, metadata !428, metadata !DIExpression()), !dbg !430
  call void @llvm.dbg.declare(metadata ptr %44, metadata !431, metadata !DIExpression()), !dbg !430
  store i32 0, ptr %44, align 4, !dbg !432
  call void @llvm.dbg.declare(metadata ptr %45, metadata !433, metadata !DIExpression()), !dbg !430
  store i32 999, ptr %45, align 4, !dbg !432
  call void @llvm.dbg.declare(metadata ptr %46, metadata !434, metadata !DIExpression()), !dbg !430
  store i32 1, ptr %46, align 4, !dbg !432
  call void @llvm.dbg.declare(metadata ptr %47, metadata !435, metadata !DIExpression()), !dbg !430
  store i32 0, ptr %47, align 4, !dbg !432
  call void @llvm.dbg.declare(metadata ptr %48, metadata !436, metadata !DIExpression()), !dbg !430
; OpenMP: [Line 41] #pragma omp for
  call void @__kmpc_for_static_init_4(ptr @20, i32 %55, i32 34, ptr %47, ptr %44, ptr %45, ptr %46, i32 1, i32 1), !dbg !437
  %182 = load i32, ptr %45, align 4, !dbg !432
  %183 = icmp sgt i32 %182, 999, !dbg !432
  br i1 %183, label %184, label %185, !dbg !432

184:                                              ; preds = %181
  br label %187, !dbg !432

185:                                              ; preds = %181
  %186 = load i32, ptr %45, align 4, !dbg !432
  br label %187, !dbg !432

187:                                              ; preds = %185, %184
  %188 = phi i32 [ 999, %184 ], [ %186, %185 ], !dbg !432
  store i32 %188, ptr %45, align 4, !dbg !432
  %189 = load i32, ptr %44, align 4, !dbg !432
  store i32 %189, ptr %42, align 4, !dbg !432
  br label %190, !dbg !438

190:                                              ; preds = %209, %187
  %191 = load i32, ptr %42, align 4, !dbg !432
  %192 = load i32, ptr %45, align 4, !dbg !432
  %193 = icmp sle i32 %191, %192, !dbg !439
  br i1 %193, label %194, label %212, !dbg !438

194:                                              ; preds = %190
  %195 = load i32, ptr %42, align 4, !dbg !432
  %196 = mul nsw i32 %195, 1, !dbg !440
  %197 = add nsw i32 0, %196, !dbg !440
  store i32 %197, ptr %48, align 4, !dbg !440
  %198 = load ptr, ptr %50, align 8, !dbg !441
  %199 = load i32, ptr %48, align 4, !dbg !444
  %200 = sext i32 %199 to i64, !dbg !441
  %201 = getelementptr inbounds i32, ptr %198, i64 %200, !dbg !441
  %202 = load i32, ptr %201, align 4, !dbg !441
  %203 = srem i32 %202, 2, !dbg !445
  %204 = icmp eq i32 %203, 0, !dbg !446
  br i1 %204, label %205, label %207, !dbg !447

205:                                              ; preds = %194
  %206 = atomicrmw add ptr %53, i32 1 monotonic, align 4, !dbg !448
  br label %207, !dbg !451

207:                                              ; preds = %205, %194
  br label %208, !dbg !452

208:                                              ; preds = %207
  br label %209, !dbg !437

209:                                              ; preds = %208
  %210 = load i32, ptr %42, align 4, !dbg !432
  %211 = add nsw i32 %210, 1, !dbg !439
  store i32 %211, ptr %42, align 4, !dbg !439
  br label %190, !dbg !437, !llvm.loop !453

212:                                              ; preds = %190
  br label %213, !dbg !437

213:                                              ; preds = %212
; OpenMP: [Line 41] #pragma omp for
  call void @__kmpc_for_static_fini(ptr @22, i32 %55), !dbg !454
; OpenMP: [Line 41] #pragma omp for
  call void @__kmpc_barrier(ptr @23, i32 %55), !dbg !454
  ret void, !dbg !455
}

; Function Attrs: nounwind
declare void @__kmpc_for_static_init_4(ptr, i32, i32, ptr, ptr, ptr, ptr, i32, i32) #4

; Function Attrs: nounwind
declare void @__kmpc_for_static_fini(ptr, i32) #4

; Function Attrs: noinline norecurse uwtable
define internal void @main.omp_outlined_debug__.omp.reduction.reduction_func(ptr noundef %0, ptr noundef %1) #5 !dbg !456 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  call void @llvm.dbg.declare(metadata ptr %3, metadata !458, metadata !DIExpression()), !dbg !459
  store ptr %1, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !460, metadata !DIExpression()), !dbg !459
  %5 = load ptr, ptr %3, align 8, !dbg !461
  %6 = load ptr, ptr %4, align 8, !dbg !461
  %7 = getelementptr inbounds [1 x ptr], ptr %6, i64 0, i64 0, !dbg !461
  %8 = load ptr, ptr %7, align 8, !dbg !461
  %9 = getelementptr inbounds [1 x ptr], ptr %5, i64 0, i64 0, !dbg !461
  %10 = load ptr, ptr %9, align 8, !dbg !461
  %11 = load i64, ptr %10, align 8, !dbg !462
  %12 = load i64, ptr %8, align 8, !dbg !462
  %13 = add nsw i64 %11, %12, !dbg !463
  store i64 %13, ptr %10, align 8, !dbg !463
  ret void, !dbg !462
}

; Function Attrs: convergent nounwind
declare i32 @__kmpc_reduce(ptr, i32, i32, i64, ptr, ptr, ptr) #6

; Function Attrs: convergent nounwind
declare void @__kmpc_end_reduce(ptr, i32, ptr) #6

; Function Attrs: convergent nounwind
declare void @__kmpc_barrier(ptr, i32) #6

; Function Attrs: noinline norecurse uwtable
define internal void @main.omp_outlined_debug__.omp.reduction.reduction_func.1(ptr noundef %0, ptr noundef %1) #5 !dbg !464 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  call void @llvm.dbg.declare(metadata ptr %3, metadata !465, metadata !DIExpression()), !dbg !466
  store ptr %1, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !467, metadata !DIExpression()), !dbg !466
  %5 = load ptr, ptr %3, align 8, !dbg !468
  %6 = load ptr, ptr %4, align 8, !dbg !468
  %7 = getelementptr inbounds [1 x ptr], ptr %6, i64 0, i64 0, !dbg !468
  %8 = load ptr, ptr %7, align 8, !dbg !468
  %9 = getelementptr inbounds [1 x ptr], ptr %5, i64 0, i64 0, !dbg !468
  %10 = load ptr, ptr %9, align 8, !dbg !468
  %11 = load i64, ptr %10, align 8, !dbg !469
  %12 = load i64, ptr %8, align 8, !dbg !469
  %13 = mul nsw i64 %11, %12, !dbg !470
  store i64 %13, ptr %10, align 8, !dbg !470
  ret void, !dbg !469
}

; Function Attrs: convergent nounwind
declare void @__kmpc_critical(ptr, i32, ptr) #6

; Function Attrs: convergent nounwind
declare void @__kmpc_end_critical(ptr, i32, ptr) #6

; Function Attrs: noinline norecurse nounwind optnone uwtable
define internal void @main.omp_outlined(ptr noalias noundef %0, ptr noalias noundef %1, ptr noundef nonnull align 8 dereferenceable(8) %2, ptr noundef nonnull align 8 dereferenceable(8) %3, ptr noundef nonnull align 8 dereferenceable(8) %4, ptr noundef nonnull align 4 dereferenceable(4) %5, ptr noundef nonnull align 4 dereferenceable(4) %6) #3 !dbg !471 {
  %8 = alloca ptr, align 8
  %9 = alloca ptr, align 8
  %10 = alloca ptr, align 8
  %11 = alloca ptr, align 8
  %12 = alloca ptr, align 8
  %13 = alloca ptr, align 8
  %14 = alloca ptr, align 8
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !472, metadata !DIExpression()), !dbg !473
  store ptr %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !474, metadata !DIExpression()), !dbg !473
  store ptr %2, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !475, metadata !DIExpression()), !dbg !473
  store ptr %3, ptr %11, align 8
  call void @llvm.dbg.declare(metadata ptr %11, metadata !476, metadata !DIExpression()), !dbg !473
  store ptr %4, ptr %12, align 8
  call void @llvm.dbg.declare(metadata ptr %12, metadata !477, metadata !DIExpression()), !dbg !473
  store ptr %5, ptr %13, align 8
  call void @llvm.dbg.declare(metadata ptr %13, metadata !478, metadata !DIExpression()), !dbg !473
  store ptr %6, ptr %14, align 8
  call void @llvm.dbg.declare(metadata ptr %14, metadata !479, metadata !DIExpression()), !dbg !473
  %15 = load ptr, ptr %10, align 8, !dbg !480
  %16 = load ptr, ptr %11, align 8, !dbg !480
  %17 = load ptr, ptr %12, align 8, !dbg !480
  %18 = load ptr, ptr %13, align 8, !dbg !480
  %19 = load ptr, ptr %14, align 8, !dbg !480
  %20 = load ptr, ptr %8, align 8, !dbg !480
  %21 = load ptr, ptr %9, align 8, !dbg !480
  %22 = load ptr, ptr %10, align 8, !dbg !480
  %23 = load ptr, ptr %11, align 8, !dbg !480
  %24 = load ptr, ptr %12, align 8, !dbg !480
  %25 = load ptr, ptr %13, align 8, !dbg !480
  %26 = load ptr, ptr %14, align 8, !dbg !480
  call void @main.omp_outlined_debug__(ptr %20, ptr %21, ptr %22, ptr %23, ptr %24, ptr %25, ptr %26) #4, !dbg !480
  ret void, !dbg !480
}

; Function Attrs: nounwind
declare !callback !481 void @__kmpc_fork_call(ptr, i32, ptr, ...) #4

declare i32 @printf(ptr noundef, ...) #7

; Function Attrs: nounwind
declare void @free(ptr noundef) #8

attributes #0 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nounwind allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { noinline norecurse nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }
attributes #5 = { noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { convergent nounwind }
attributes #7 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { nounwind allocsize(0) }

!llvm.dbg.cu = !{!23}
!llvm.module.flags = !{!274, !275, !276, !277, !278, !279, !280, !281}
!llvm.ident = !{!282}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 50, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "test2.cpp", directory: "/mnt/c/llvm-clang", checksumkind: CSK_MD5, checksum: "18f77da42c342c472bf139abb02a780a")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 96, elements: !6)
!4 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !5)
!5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!6 = !{!7}
!7 = !DISubrange(count: 12)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(scope: null, file: !2, line: 51, type: !10, isLocal: true, isDefinition: true)
!10 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 296, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 37)
!13 = !DIGlobalVariableExpression(var: !14, expr: !DIExpression())
!14 = distinct !DIGlobalVariable(scope: null, file: !2, line: 52, type: !15, isLocal: true, isDefinition: true)
!15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 128, elements: !16)
!16 = !{!17}
!17 = !DISubrange(count: 16)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(scope: null, file: !2, line: 53, type: !20, isLocal: true, isDefinition: true)
!20 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 136, elements: !21)
!21 = !{!22}
!22 = !DISubrange(count: 17)
!23 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !2, producer: "Ubuntu clang version 18.1.3 (1ubuntu1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !24, globals: !27, imports: !28, splitDebugInlining: false, nameTableKind: None)
!24 = !{!25}
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64)
!26 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!27 = !{!0, !8, !13, !18}
!28 = !{!29, !36, !40, !47, !51, !59, !64, !66, !72, !76, !80, !90, !92, !96, !100, !104, !109, !113, !117, !121, !125, !133, !137, !141, !143, !147, !151, !156, !162, !166, !170, !172, !180, !184, !192, !194, !198, !202, !206, !210, !215, !220, !225, !226, !227, !228, !230, !231, !232, !233, !234, !235, !236, !238, !239, !240, !241, !242, !243, !244, !245, !250, !251, !252, !253, !254, !255, !256, !257, !258, !259, !260, !261, !262, !263, !264, !265, !266, !267, !268, !269, !270, !271, !272, !273}
!29 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !31, file: !35, line: 52)
!30 = !DINamespace(name: "std", scope: null)
!31 = !DISubprogram(name: "abs", scope: !32, file: !32, line: 980, type: !33, flags: DIFlagPrototyped, spFlags: 0)
!32 = !DIFile(filename: "/usr/include/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "7fa2ecb2348a66f8b44ab9a15abd0b72")
!33 = !DISubroutineType(types: !34)
!34 = !{!26, !26}
!35 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/std_abs.h", directory: "")
!36 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !37, file: !39, line: 131)
!37 = !DIDerivedType(tag: DW_TAG_typedef, name: "div_t", file: !32, line: 63, baseType: !38)
!38 = !DICompositeType(tag: DW_TAG_structure_type, file: !32, line: 59, size: 64, flags: DIFlagFwdDecl, identifier: "_ZTS5div_t")
!39 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/cstdlib", directory: "")
!40 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !41, file: !39, line: 132)
!41 = !DIDerivedType(tag: DW_TAG_typedef, name: "ldiv_t", file: !32, line: 71, baseType: !42)
!42 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !32, line: 67, size: 128, flags: DIFlagTypePassByValue, elements: !43, identifier: "_ZTS6ldiv_t")
!43 = !{!44, !46}
!44 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !42, file: !32, line: 69, baseType: !45, size: 64)
!45 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!46 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !42, file: !32, line: 70, baseType: !45, size: 64, offset: 64)
!47 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !48, file: !39, line: 134)
!48 = !DISubprogram(name: "abort", scope: !32, file: !32, line: 730, type: !49, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!49 = !DISubroutineType(types: !50)
!50 = !{null}
!51 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !52, file: !39, line: 136)
!52 = !DISubprogram(name: "aligned_alloc", scope: !32, file: !32, line: 724, type: !53, flags: DIFlagPrototyped, spFlags: 0)
!53 = !DISubroutineType(types: !54)
!54 = !{!55, !56, !56}
!55 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!56 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !57, line: 18, baseType: !58)
!57 = !DIFile(filename: "/usr/lib/llvm-18/lib/clang/18/include/__stddef_size_t.h", directory: "", checksumkind: CSK_MD5, checksum: "2c44e821a2b1951cde2eb0fb2e656867")
!58 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!59 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !60, file: !39, line: 138)
!60 = !DISubprogram(name: "atexit", scope: !32, file: !32, line: 734, type: !61, flags: DIFlagPrototyped, spFlags: 0)
!61 = !DISubroutineType(types: !62)
!62 = !{!26, !63}
!63 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !49, size: 64)
!64 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !65, file: !39, line: 141)
!65 = !DISubprogram(name: "at_quick_exit", scope: !32, file: !32, line: 739, type: !61, flags: DIFlagPrototyped, spFlags: 0)
!66 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !67, file: !39, line: 144)
!67 = !DISubprogram(name: "atof", scope: !32, file: !32, line: 102, type: !68, flags: DIFlagPrototyped, spFlags: 0)
!68 = !DISubroutineType(types: !69)
!69 = !{!70, !71}
!70 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!71 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!72 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !73, file: !39, line: 145)
!73 = !DISubprogram(name: "atoi", scope: !32, file: !32, line: 105, type: !74, flags: DIFlagPrototyped, spFlags: 0)
!74 = !DISubroutineType(types: !75)
!75 = !{!26, !71}
!76 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !77, file: !39, line: 146)
!77 = !DISubprogram(name: "atol", scope: !32, file: !32, line: 108, type: !78, flags: DIFlagPrototyped, spFlags: 0)
!78 = !DISubroutineType(types: !79)
!79 = !{!45, !71}
!80 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !81, file: !39, line: 147)
!81 = !DISubprogram(name: "bsearch", scope: !32, file: !32, line: 960, type: !82, flags: DIFlagPrototyped, spFlags: 0)
!82 = !DISubroutineType(types: !83)
!83 = !{!55, !84, !84, !56, !56, !86}
!84 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !85, size: 64)
!85 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!86 = !DIDerivedType(tag: DW_TAG_typedef, name: "__compar_fn_t", file: !32, line: 948, baseType: !87)
!87 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !88, size: 64)
!88 = !DISubroutineType(types: !89)
!89 = !{!26, !84, !84}
!90 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !91, file: !39, line: 148)
!91 = !DISubprogram(name: "calloc", scope: !32, file: !32, line: 675, type: !53, flags: DIFlagPrototyped, spFlags: 0)
!92 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !93, file: !39, line: 149)
!93 = !DISubprogram(name: "div", scope: !32, file: !32, line: 992, type: !94, flags: DIFlagPrototyped, spFlags: 0)
!94 = !DISubroutineType(types: !95)
!95 = !{!37, !26, !26}
!96 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !97, file: !39, line: 150)
!97 = !DISubprogram(name: "exit", scope: !32, file: !32, line: 756, type: !98, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!98 = !DISubroutineType(types: !99)
!99 = !{null, !26}
!100 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !101, file: !39, line: 151)
!101 = !DISubprogram(name: "free", scope: !32, file: !32, line: 687, type: !102, flags: DIFlagPrototyped, spFlags: 0)
!102 = !DISubroutineType(types: !103)
!103 = !{null, !55}
!104 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !105, file: !39, line: 152)
!105 = !DISubprogram(name: "getenv", scope: !32, file: !32, line: 773, type: !106, flags: DIFlagPrototyped, spFlags: 0)
!106 = !DISubroutineType(types: !107)
!107 = !{!108, !71}
!108 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!109 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !110, file: !39, line: 153)
!110 = !DISubprogram(name: "labs", scope: !32, file: !32, line: 981, type: !111, flags: DIFlagPrototyped, spFlags: 0)
!111 = !DISubroutineType(types: !112)
!112 = !{!45, !45}
!113 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !114, file: !39, line: 154)
!114 = !DISubprogram(name: "ldiv", scope: !32, file: !32, line: 994, type: !115, flags: DIFlagPrototyped, spFlags: 0)
!115 = !DISubroutineType(types: !116)
!116 = !{!41, !45, !45}
!117 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !118, file: !39, line: 155)
!118 = !DISubprogram(name: "malloc", scope: !32, file: !32, line: 672, type: !119, flags: DIFlagPrototyped, spFlags: 0)
!119 = !DISubroutineType(types: !120)
!120 = !{!55, !56}
!121 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !122, file: !39, line: 157)
!122 = !DISubprogram(name: "mblen", scope: !32, file: !32, line: 1062, type: !123, flags: DIFlagPrototyped, spFlags: 0)
!123 = !DISubroutineType(types: !124)
!124 = !{!26, !71, !56}
!125 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !126, file: !39, line: 158)
!126 = !DISubprogram(name: "mbstowcs", scope: !32, file: !32, line: 1073, type: !127, flags: DIFlagPrototyped, spFlags: 0)
!127 = !DISubroutineType(types: !128)
!128 = !{!56, !129, !132, !56}
!129 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !130)
!130 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !131, size: 64)
!131 = !DIBasicType(name: "wchar_t", size: 32, encoding: DW_ATE_signed)
!132 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !71)
!133 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !134, file: !39, line: 159)
!134 = !DISubprogram(name: "mbtowc", scope: !32, file: !32, line: 1065, type: !135, flags: DIFlagPrototyped, spFlags: 0)
!135 = !DISubroutineType(types: !136)
!136 = !{!26, !129, !132, !56}
!137 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !138, file: !39, line: 161)
!138 = !DISubprogram(name: "qsort", scope: !32, file: !32, line: 970, type: !139, flags: DIFlagPrototyped, spFlags: 0)
!139 = !DISubroutineType(types: !140)
!140 = !{null, !55, !56, !56, !86}
!141 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !142, file: !39, line: 164)
!142 = !DISubprogram(name: "quick_exit", scope: !32, file: !32, line: 762, type: !98, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!143 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !144, file: !39, line: 167)
!144 = !DISubprogram(name: "rand", scope: !32, file: !32, line: 573, type: !145, flags: DIFlagPrototyped, spFlags: 0)
!145 = !DISubroutineType(types: !146)
!146 = !{!26}
!147 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !148, file: !39, line: 168)
!148 = !DISubprogram(name: "realloc", scope: !32, file: !32, line: 683, type: !149, flags: DIFlagPrototyped, spFlags: 0)
!149 = !DISubroutineType(types: !150)
!150 = !{!55, !55, !56}
!151 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !152, file: !39, line: 169)
!152 = !DISubprogram(name: "srand", scope: !32, file: !32, line: 575, type: !153, flags: DIFlagPrototyped, spFlags: 0)
!153 = !DISubroutineType(types: !154)
!154 = !{null, !155}
!155 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!156 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !157, file: !39, line: 170)
!157 = !DISubprogram(name: "strtod", scope: !32, file: !32, line: 118, type: !158, flags: DIFlagPrototyped, spFlags: 0)
!158 = !DISubroutineType(types: !159)
!159 = !{!70, !132, !160}
!160 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !161)
!161 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !108, size: 64)
!162 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !163, file: !39, line: 171)
!163 = !DISubprogram(name: "strtol", linkageName: "__isoc23_strtol", scope: !32, file: !32, line: 215, type: !164, flags: DIFlagPrototyped, spFlags: 0)
!164 = !DISubroutineType(types: !165)
!165 = !{!45, !132, !160, !26}
!166 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !167, file: !39, line: 172)
!167 = !DISubprogram(name: "strtoul", linkageName: "__isoc23_strtoul", scope: !32, file: !32, line: 219, type: !168, flags: DIFlagPrototyped, spFlags: 0)
!168 = !DISubroutineType(types: !169)
!169 = !{!58, !132, !160, !26}
!170 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !171, file: !39, line: 173)
!171 = !DISubprogram(name: "system", scope: !32, file: !32, line: 923, type: !74, flags: DIFlagPrototyped, spFlags: 0)
!172 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !173, file: !39, line: 175)
!173 = !DISubprogram(name: "wcstombs", scope: !32, file: !32, line: 1077, type: !174, flags: DIFlagPrototyped, spFlags: 0)
!174 = !DISubroutineType(types: !175)
!175 = !{!56, !176, !177, !56}
!176 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !108)
!177 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !178)
!178 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !179, size: 64)
!179 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !131)
!180 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !181, file: !39, line: 176)
!181 = !DISubprogram(name: "wctomb", scope: !32, file: !32, line: 1069, type: !182, flags: DIFlagPrototyped, spFlags: 0)
!182 = !DISubroutineType(types: !183)
!183 = !{!26, !108, !131}
!184 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !186, file: !39, line: 204)
!185 = !DINamespace(name: "__gnu_cxx", scope: null)
!186 = !DIDerivedType(tag: DW_TAG_typedef, name: "lldiv_t", file: !32, line: 81, baseType: !187)
!187 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !32, line: 77, size: 128, flags: DIFlagTypePassByValue, elements: !188, identifier: "_ZTS7lldiv_t")
!188 = !{!189, !191}
!189 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !187, file: !32, line: 79, baseType: !190, size: 64)
!190 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!191 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !187, file: !32, line: 80, baseType: !190, size: 64, offset: 64)
!192 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !193, file: !39, line: 210)
!193 = !DISubprogram(name: "_Exit", scope: !32, file: !32, line: 768, type: !98, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!194 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !195, file: !39, line: 214)
!195 = !DISubprogram(name: "llabs", scope: !32, file: !32, line: 984, type: !196, flags: DIFlagPrototyped, spFlags: 0)
!196 = !DISubroutineType(types: !197)
!197 = !{!190, !190}
!198 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !199, file: !39, line: 220)
!199 = !DISubprogram(name: "lldiv", scope: !32, file: !32, line: 998, type: !200, flags: DIFlagPrototyped, spFlags: 0)
!200 = !DISubroutineType(types: !201)
!201 = !{!186, !190, !190}
!202 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !203, file: !39, line: 231)
!203 = !DISubprogram(name: "atoll", scope: !32, file: !32, line: 113, type: !204, flags: DIFlagPrototyped, spFlags: 0)
!204 = !DISubroutineType(types: !205)
!205 = !{!190, !71}
!206 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !207, file: !39, line: 232)
!207 = !DISubprogram(name: "strtoll", linkageName: "__isoc23_strtoll", scope: !32, file: !32, line: 238, type: !208, flags: DIFlagPrototyped, spFlags: 0)
!208 = !DISubroutineType(types: !209)
!209 = !{!190, !132, !160, !26}
!210 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !211, file: !39, line: 233)
!211 = !DISubprogram(name: "strtoull", linkageName: "__isoc23_strtoull", scope: !32, file: !32, line: 243, type: !212, flags: DIFlagPrototyped, spFlags: 0)
!212 = !DISubroutineType(types: !213)
!213 = !{!214, !132, !160, !26}
!214 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!215 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !216, file: !39, line: 235)
!216 = !DISubprogram(name: "strtof", scope: !32, file: !32, line: 124, type: !217, flags: DIFlagPrototyped, spFlags: 0)
!217 = !DISubroutineType(types: !218)
!218 = !{!219, !132, !160}
!219 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!220 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !185, entity: !221, file: !39, line: 236)
!221 = !DISubprogram(name: "strtold", scope: !32, file: !32, line: 127, type: !222, flags: DIFlagPrototyped, spFlags: 0)
!222 = !DISubroutineType(types: !223)
!223 = !{!224, !132, !160}
!224 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!225 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !186, file: !39, line: 244)
!226 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !193, file: !39, line: 246)
!227 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !195, file: !39, line: 248)
!228 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !229, file: !39, line: 249)
!229 = !DISubprogram(name: "div", linkageName: "_ZN9__gnu_cxx3divExx", scope: !185, file: !39, line: 217, type: !200, flags: DIFlagPrototyped, spFlags: 0)
!230 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !199, file: !39, line: 250)
!231 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !203, file: !39, line: 252)
!232 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !216, file: !39, line: 253)
!233 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !207, file: !39, line: 254)
!234 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !211, file: !39, line: 255)
!235 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !30, entity: !221, file: !39, line: 256)
!236 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !48, file: !237, line: 38)
!237 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "3f24ff2a8eef595875da96e5466bd4aa")
!238 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !60, file: !237, line: 39)
!239 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !97, file: !237, line: 40)
!240 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !65, file: !237, line: 43)
!241 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !142, file: !237, line: 46)
!242 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !193, file: !237, line: 49)
!243 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !37, file: !237, line: 54)
!244 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !41, file: !237, line: 55)
!245 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !246, file: !237, line: 57)
!246 = !DISubprogram(name: "abs", linkageName: "_ZSt3absg", scope: !30, file: !35, line: 137, type: !247, flags: DIFlagPrototyped, spFlags: 0)
!247 = !DISubroutineType(types: !248)
!248 = !{!249, !249}
!249 = !DIBasicType(name: "__float128", size: 128, encoding: DW_ATE_float)
!250 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !67, file: !237, line: 58)
!251 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !73, file: !237, line: 59)
!252 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !77, file: !237, line: 60)
!253 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !81, file: !237, line: 61)
!254 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !91, file: !237, line: 62)
!255 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !229, file: !237, line: 63)
!256 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !101, file: !237, line: 64)
!257 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !105, file: !237, line: 65)
!258 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !110, file: !237, line: 66)
!259 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !114, file: !237, line: 67)
!260 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !118, file: !237, line: 68)
!261 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !122, file: !237, line: 70)
!262 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !126, file: !237, line: 71)
!263 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !134, file: !237, line: 72)
!264 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !138, file: !237, line: 74)
!265 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !144, file: !237, line: 75)
!266 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !148, file: !237, line: 76)
!267 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !152, file: !237, line: 77)
!268 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !157, file: !237, line: 78)
!269 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !163, file: !237, line: 79)
!270 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !167, file: !237, line: 80)
!271 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !171, file: !237, line: 81)
!272 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !173, file: !237, line: 83)
!273 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !23, entity: !181, file: !237, line: 84)
!274 = !{i32 7, !"Dwarf Version", i32 5}
!275 = !{i32 2, !"Debug Info Version", i32 3}
!276 = !{i32 1, !"wchar_size", i32 4}
!277 = !{i32 7, !"openmp", i32 51}
!278 = !{i32 8, !"PIC Level", i32 2}
!279 = !{i32 7, !"PIE Level", i32 2}
!280 = !{i32 7, !"uwtable", i32 2}
!281 = !{i32 7, !"frame-pointer", i32 2}
!282 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!283 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 7, type: !145, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !23, retainedNodes: !284)
!284 = !{}
!285 = !DILocalVariable(name: "arr", scope: !283, file: !2, line: 8, type: !25)
!286 = !DILocation(line: 8, column: 10, scope: !283)
!287 = !DILocation(line: 8, column: 22, scope: !283)
!288 = !DILocalVariable(name: "i", scope: !289, file: !2, line: 9, type: !26)
!289 = distinct !DILexicalBlock(scope: !283, file: !2, line: 9, column: 5)
!290 = !DILocation(line: 9, column: 14, scope: !289)
!291 = !DILocation(line: 9, column: 10, scope: !289)
!292 = !DILocation(line: 9, column: 21, scope: !293)
!293 = distinct !DILexicalBlock(scope: !289, file: !2, line: 9, column: 5)
!294 = !DILocation(line: 9, column: 23, scope: !293)
!295 = !DILocation(line: 9, column: 5, scope: !289)
!296 = !DILocation(line: 9, column: 42, scope: !293)
!297 = !DILocation(line: 9, column: 44, scope: !293)
!298 = !DILocation(line: 9, column: 33, scope: !293)
!299 = !DILocation(line: 9, column: 37, scope: !293)
!300 = !DILocation(line: 9, column: 40, scope: !293)
!301 = !DILocation(line: 9, column: 28, scope: !293)
!302 = !DILocation(line: 9, column: 5, scope: !293)
!303 = distinct !{!303, !295, !304, !305}
!304 = !DILocation(line: 9, column: 46, scope: !289)
!305 = !{!"llvm.loop.mustprogress"}
!306 = !DILocalVariable(name: "sum", scope: !283, file: !2, line: 11, type: !190)
!307 = !DILocation(line: 11, column: 15, scope: !283)
!308 = !DILocalVariable(name: "product", scope: !283, file: !2, line: 12, type: !190)
!309 = !DILocation(line: 12, column: 15, scope: !283)
!310 = !DILocalVariable(name: "max_val", scope: !283, file: !2, line: 13, type: !26)
!311 = !DILocation(line: 13, column: 9, scope: !283)
!312 = !DILocalVariable(name: "even_count", scope: !283, file: !2, line: 14, type: !26)
!313 = !DILocation(line: 14, column: 9, scope: !283)
!314 = !DILocation(line: 17, column: 1, scope: !283)
!315 = !DILocation(line: 50, column: 28, scope: !283)
!316 = !DILocation(line: 50, column: 5, scope: !283)
!317 = !DILocation(line: 51, column: 53, scope: !283)
!318 = !DILocation(line: 51, column: 5, scope: !283)
!319 = !DILocation(line: 52, column: 32, scope: !283)
!320 = !DILocation(line: 52, column: 5, scope: !283)
!321 = !DILocation(line: 53, column: 33, scope: !283)
!322 = !DILocation(line: 53, column: 5, scope: !283)
!323 = !DILocation(line: 55, column: 10, scope: !283)
!324 = !DILocation(line: 55, column: 5, scope: !283)
!325 = !DILocation(line: 56, column: 5, scope: !283)
!326 = distinct !DISubprogram(name: "main.omp_outlined_debug__", scope: !2, file: !2, line: 18, type: !327, scopeLine: 18, flags: DIFlagArtificial | DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !23, retainedNodes: !284)
!327 = !DISubroutineType(types: !328)
!328 = !{null, !329, !329, !333, !334, !333, !335, !335}
!329 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !330)
!330 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !331)
!331 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !332, size: 64)
!332 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !26)
!333 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !190, size: 64)
!334 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !25, size: 64)
!335 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !26, size: 64)
!336 = !DILocalVariable(name: ".global_tid.", arg: 1, scope: !326, type: !329, flags: DIFlagArtificial)
!337 = !DILocation(line: 0, scope: !326)
!338 = !DILocalVariable(name: ".bound_tid.", arg: 2, scope: !326, type: !329, flags: DIFlagArtificial)
!339 = !DILocalVariable(name: "sum", arg: 3, scope: !326, file: !2, line: 11, type: !333)
!340 = !DILocation(line: 11, column: 15, scope: !326)
!341 = !DILocalVariable(name: "arr", arg: 4, scope: !326, file: !2, line: 8, type: !334)
!342 = !DILocation(line: 8, column: 10, scope: !326)
!343 = !DILocalVariable(name: "product", arg: 5, scope: !326, file: !2, line: 12, type: !333)
!344 = !DILocation(line: 12, column: 15, scope: !326)
!345 = !DILocalVariable(name: "max_val", arg: 6, scope: !326, file: !2, line: 13, type: !335)
!346 = !DILocation(line: 13, column: 9, scope: !326)
!347 = !DILocalVariable(name: "even_count", arg: 7, scope: !326, file: !2, line: 14, type: !335)
!348 = !DILocation(line: 14, column: 9, scope: !326)
!349 = !DILocation(line: 18, column: 5, scope: !326)
!350 = !DILocalVariable(name: ".omp.iv", scope: !351, type: !26, flags: DIFlagArtificial)
!351 = distinct !DILexicalBlock(scope: !352, file: !2, line: 20, column: 1)
!352 = distinct !DILexicalBlock(scope: !326, file: !2, line: 18, column: 5)
!353 = !DILocation(line: 0, scope: !351)
!354 = !DILocalVariable(name: ".omp.lb", scope: !351, type: !26, flags: DIFlagArtificial)
!355 = !DILocation(line: 21, column: 14, scope: !351)
!356 = !DILocalVariable(name: ".omp.ub", scope: !351, type: !26, flags: DIFlagArtificial)
!357 = !DILocalVariable(name: ".omp.stride", scope: !351, type: !26, flags: DIFlagArtificial)
!358 = !DILocalVariable(name: ".omp.is_last", scope: !351, type: !26, flags: DIFlagArtificial)
!359 = !DILocalVariable(name: "sum", scope: !351, type: !190, flags: DIFlagArtificial)
!360 = !DILocation(line: 20, column: 29, scope: !351)
!361 = !DILocalVariable(name: "i", scope: !351, type: !26, flags: DIFlagArtificial)
!362 = !DILocation(line: 20, column: 1, scope: !352)
!363 = !DILocation(line: 20, column: 1, scope: !351)
!364 = !DILocation(line: 21, column: 9, scope: !351)
!365 = !DILocation(line: 21, column: 32, scope: !351)
!366 = !DILocation(line: 22, column: 20, scope: !367)
!367 = distinct !DILexicalBlock(scope: !351, file: !2, line: 21, column: 37)
!368 = !DILocation(line: 22, column: 24, scope: !367)
!369 = !DILocation(line: 22, column: 17, scope: !367)
!370 = !DILocation(line: 23, column: 9, scope: !367)
!371 = distinct !{!371, !363, !372}
!372 = !DILocation(line: 20, column: 33, scope: !351)
!373 = !DILocation(line: 20, column: 27, scope: !351)
!374 = !DILocalVariable(name: ".omp.iv", scope: !375, type: !26, flags: DIFlagArtificial)
!375 = distinct !DILexicalBlock(scope: !352, file: !2, line: 26, column: 1)
!376 = !DILocation(line: 0, scope: !375)
!377 = !DILocalVariable(name: ".omp.lb", scope: !375, type: !26, flags: DIFlagArtificial)
!378 = !DILocation(line: 27, column: 14, scope: !375)
!379 = !DILocalVariable(name: ".omp.ub", scope: !375, type: !26, flags: DIFlagArtificial)
!380 = !DILocalVariable(name: ".omp.stride", scope: !375, type: !26, flags: DIFlagArtificial)
!381 = !DILocalVariable(name: ".omp.is_last", scope: !375, type: !26, flags: DIFlagArtificial)
!382 = !DILocalVariable(name: "product", scope: !375, type: !190, flags: DIFlagArtificial)
!383 = !DILocation(line: 26, column: 29, scope: !375)
!384 = !DILocalVariable(name: "i", scope: !375, type: !26, flags: DIFlagArtificial)
!385 = !DILocation(line: 26, column: 1, scope: !375)
!386 = !DILocation(line: 26, column: 1, scope: !352)
!387 = !DILocation(line: 27, column: 9, scope: !375)
!388 = !DILocation(line: 27, column: 33, scope: !375)
!389 = !DILocation(line: 28, column: 24, scope: !390)
!390 = distinct !DILexicalBlock(scope: !375, file: !2, line: 27, column: 38)
!391 = !DILocation(line: 28, column: 28, scope: !390)
!392 = !DILocation(line: 28, column: 21, scope: !390)
!393 = !DILocation(line: 29, column: 9, scope: !390)
!394 = distinct !{!394, !385, !395}
!395 = !DILocation(line: 26, column: 37, scope: !375)
!396 = !DILocation(line: 26, column: 27, scope: !375)
!397 = !DILocalVariable(name: ".omp.iv", scope: !398, type: !26, flags: DIFlagArtificial)
!398 = distinct !DILexicalBlock(scope: !352, file: !2, line: 32, column: 1)
!399 = !DILocation(line: 0, scope: !398)
!400 = !DILocalVariable(name: ".omp.lb", scope: !398, type: !26, flags: DIFlagArtificial)
!401 = !DILocation(line: 33, column: 14, scope: !398)
!402 = !DILocalVariable(name: ".omp.ub", scope: !398, type: !26, flags: DIFlagArtificial)
!403 = !DILocalVariable(name: ".omp.stride", scope: !398, type: !26, flags: DIFlagArtificial)
!404 = !DILocalVariable(name: ".omp.is_last", scope: !398, type: !26, flags: DIFlagArtificial)
!405 = !DILocalVariable(name: "i", scope: !398, type: !26, flags: DIFlagArtificial)
!406 = !DILocation(line: 32, column: 1, scope: !398)
!407 = !DILocation(line: 32, column: 1, scope: !352)
!408 = !DILocation(line: 33, column: 9, scope: !398)
!409 = !DILocation(line: 33, column: 32, scope: !398)
!410 = !DILocation(line: 34, column: 1, scope: !411)
!411 = distinct !DILexicalBlock(scope: !412, file: !2, line: 34, column: 1)
!412 = distinct !DILexicalBlock(scope: !398, file: !2, line: 33, column: 37)
!413 = !DILocation(line: 36, column: 21, scope: !414)
!414 = distinct !DILexicalBlock(scope: !415, file: !2, line: 36, column: 21)
!415 = distinct !DILexicalBlock(scope: !411, file: !2, line: 35, column: 13)
!416 = !DILocation(line: 36, column: 25, scope: !414)
!417 = !DILocation(line: 36, column: 30, scope: !414)
!418 = !DILocation(line: 36, column: 28, scope: !414)
!419 = !DILocation(line: 36, column: 21, scope: !415)
!420 = !DILocation(line: 36, column: 49, scope: !414)
!421 = !DILocation(line: 36, column: 53, scope: !414)
!422 = !DILocation(line: 36, column: 47, scope: !414)
!423 = !DILocation(line: 36, column: 39, scope: !414)
!424 = !DILocation(line: 37, column: 13, scope: !415)
!425 = !DILocation(line: 38, column: 9, scope: !412)
!426 = distinct !{!426, !406, !427}
!427 = !DILocation(line: 32, column: 16, scope: !398)
!428 = !DILocalVariable(name: ".omp.iv", scope: !429, type: !26, flags: DIFlagArtificial)
!429 = distinct !DILexicalBlock(scope: !352, file: !2, line: 41, column: 1)
!430 = !DILocation(line: 0, scope: !429)
!431 = !DILocalVariable(name: ".omp.lb", scope: !429, type: !26, flags: DIFlagArtificial)
!432 = !DILocation(line: 42, column: 14, scope: !429)
!433 = !DILocalVariable(name: ".omp.ub", scope: !429, type: !26, flags: DIFlagArtificial)
!434 = !DILocalVariable(name: ".omp.stride", scope: !429, type: !26, flags: DIFlagArtificial)
!435 = !DILocalVariable(name: ".omp.is_last", scope: !429, type: !26, flags: DIFlagArtificial)
!436 = !DILocalVariable(name: "i", scope: !429, type: !26, flags: DIFlagArtificial)
!437 = !DILocation(line: 41, column: 1, scope: !429)
!438 = !DILocation(line: 41, column: 1, scope: !352)
!439 = !DILocation(line: 42, column: 9, scope: !429)
!440 = !DILocation(line: 42, column: 32, scope: !429)
!441 = !DILocation(line: 43, column: 17, scope: !442)
!442 = distinct !DILexicalBlock(scope: !443, file: !2, line: 43, column: 17)
!443 = distinct !DILexicalBlock(scope: !429, file: !2, line: 42, column: 37)
!444 = !DILocation(line: 43, column: 21, scope: !442)
!445 = !DILocation(line: 43, column: 24, scope: !442)
!446 = !DILocation(line: 43, column: 28, scope: !442)
!447 = !DILocation(line: 43, column: 17, scope: !443)
!448 = !DILocation(line: 45, column: 17, scope: !449)
!449 = distinct !DILexicalBlock(scope: !450, file: !2, line: 44, column: 1)
!450 = distinct !DILexicalBlock(scope: !442, file: !2, line: 43, column: 34)
!451 = !DILocation(line: 46, column: 13, scope: !450)
!452 = !DILocation(line: 47, column: 9, scope: !443)
!453 = distinct !{!453, !437, !454}
!454 = !DILocation(line: 41, column: 16, scope: !429)
!455 = !DILocation(line: 48, column: 5, scope: !326)
!456 = distinct !DISubprogram(linkageName: "main.omp_outlined_debug__.omp.reduction.reduction_func", scope: !2, file: !2, line: 20, type: !457, scopeLine: 20, flags: DIFlagArtificial, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !23, retainedNodes: !284)
!457 = !DISubroutineType(types: !284)
!458 = !DILocalVariable(arg: 1, scope: !456, type: !55, flags: DIFlagArtificial)
!459 = !DILocation(line: 0, scope: !456)
!460 = !DILocalVariable(arg: 2, scope: !456, type: !55, flags: DIFlagArtificial)
!461 = !DILocation(line: 20, column: 33, scope: !456)
!462 = !DILocation(line: 20, column: 29, scope: !456)
!463 = !DILocation(line: 20, column: 27, scope: !456)
!464 = distinct !DISubprogram(linkageName: "main.omp_outlined_debug__.omp.reduction.reduction_func.1", scope: !2, file: !2, line: 26, type: !457, scopeLine: 26, flags: DIFlagArtificial, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !23, retainedNodes: !284)
!465 = !DILocalVariable(arg: 1, scope: !464, type: !55, flags: DIFlagArtificial)
!466 = !DILocation(line: 0, scope: !464)
!467 = !DILocalVariable(arg: 2, scope: !464, type: !55, flags: DIFlagArtificial)
!468 = !DILocation(line: 26, column: 37, scope: !464)
!469 = !DILocation(line: 26, column: 29, scope: !464)
!470 = !DILocation(line: 26, column: 27, scope: !464)
!471 = distinct !DISubprogram(name: "main.omp_outlined", scope: !2, file: !2, line: 17, type: !327, scopeLine: 17, flags: DIFlagArtificial | DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !23, retainedNodes: !284)
!472 = !DILocalVariable(name: ".global_tid.", arg: 1, scope: !471, type: !329, flags: DIFlagArtificial)
!473 = !DILocation(line: 0, scope: !471)
!474 = !DILocalVariable(name: ".bound_tid.", arg: 2, scope: !471, type: !329, flags: DIFlagArtificial)
!475 = !DILocalVariable(name: "sum", arg: 3, scope: !471, type: !333, flags: DIFlagArtificial)
!476 = !DILocalVariable(name: "arr", arg: 4, scope: !471, type: !334, flags: DIFlagArtificial)
!477 = !DILocalVariable(name: "product", arg: 5, scope: !471, type: !333, flags: DIFlagArtificial)
!478 = !DILocalVariable(name: "max_val", arg: 6, scope: !471, type: !335, flags: DIFlagArtificial)
!479 = !DILocalVariable(name: "even_count", arg: 7, scope: !471, type: !335, flags: DIFlagArtificial)
!480 = !DILocation(line: 17, column: 1, scope: !471)
!481 = !{!482}
!482 = !{i64 2, i64 -1, i64 -1, i1 true}
