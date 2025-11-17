; ModuleID = 'tests/cosine/cosine.c'
source_filename = "tests/cosine/cosine.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local float @cosine(float noundef %0) #0 {
  %2 = alloca float, align 4
  %3 = alloca float, align 4
  %4 = alloca float, align 4
  %5 = alloca float, align 4
  %6 = alloca float, align 4
  %7 = alloca float, align 4
  store float %0, ptr %2, align 4
  store float 0x3EB0C6F7A0000000, ptr %6, align 4
  store float 1.000000e+00, ptr %4, align 4
  store float 1.000000e+00, ptr %3, align 4
  store float 1.000000e+00, ptr %5, align 4
  store float -1.000000e+00, ptr %7, align 4
  br label %8

8:                                                ; preds = %12, %1
  %9 = load float, ptr %5, align 4
  %10 = load float, ptr %6, align 4
  %11 = fcmp ogt float %9, %10
  br i1 %11, label %12, label %31

12:                                               ; preds = %8
  %13 = load float, ptr %5, align 4
  %14 = load float, ptr %2, align 4
  %15 = fmul float %13, %14
  %16 = load float, ptr %2, align 4
  %17 = fmul float %15, %16
  %18 = load float, ptr %4, align 4
  %19 = fdiv float %17, %18
  %20 = load float, ptr %4, align 4
  %21 = fadd float %20, 1.000000e+00
  %22 = fdiv float %19, %21
  store float %22, ptr %5, align 4
  %23 = load float, ptr %3, align 4
  %24 = load float, ptr %7, align 4
  %25 = load float, ptr %5, align 4
  %26 = call float @llvm.fmuladd.f32(float %24, float %25, float %23)
  store float %26, ptr %3, align 4
  %27 = load float, ptr %7, align 4
  %28 = fneg float %27
  store float %28, ptr %7, align 4
  %29 = load float, ptr %4, align 4
  %30 = fadd float %29, 2.000000e+00
  store float %30, ptr %4, align 4
  br label %8, !llvm.loop !6

31:                                               ; preds = %8
  %32 = load float, ptr %3, align 4
  %33 = call float @print_float(float noundef %32)
  %34 = load float, ptr %3, align 4
  ret float %34
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.fmuladd.f32(float, float, float) #1

declare float @print_float(float noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.1.0"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
