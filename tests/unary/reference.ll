; ModuleID = 'tests/unary/unary.c'
source_filename = "tests/unary/unary.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local float @unary(i32 noundef %0, float noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  %5 = alloca float, align 4
  %6 = alloca float, align 4
  store i32 %0, ptr %3, align 4
  store float %1, ptr %4, align 4
  store float 0.000000e+00, ptr %6, align 4
  %7 = load i32, ptr %3, align 4
  %8 = sitofp i32 %7 to float
  %9 = load float, ptr %4, align 4
  %10 = fadd float %8, %9
  store float %10, ptr %5, align 4
  %11 = load float, ptr %5, align 4
  %12 = call float @print_float(float noundef %11)
  %13 = load float, ptr %6, align 4
  %14 = load float, ptr %5, align 4
  %15 = fadd float %13, %14
  store float %15, ptr %6, align 4
  %16 = load i32, ptr %3, align 4
  %17 = sitofp i32 %16 to float
  %18 = load float, ptr %4, align 4
  %19 = fneg float %18
  %20 = fadd float %17, %19
  store float %20, ptr %5, align 4
  %21 = load float, ptr %5, align 4
  %22 = call float @print_float(float noundef %21)
  %23 = load float, ptr %6, align 4
  %24 = load float, ptr %5, align 4
  %25 = fadd float %23, %24
  store float %25, ptr %6, align 4
  %26 = load i32, ptr %3, align 4
  %27 = sitofp i32 %26 to float
  %28 = load float, ptr %4, align 4
  %29 = fadd float %28, -1.000000e+00
  store float %29, ptr %4, align 4
  %30 = fadd float %27, %29
  store float %30, ptr %5, align 4
  %31 = load float, ptr %5, align 4
  %32 = call float @print_float(float noundef %31)
  %33 = load float, ptr %6, align 4
  %34 = load float, ptr %5, align 4
  %35 = fadd float %33, %34
  store float %35, ptr %6, align 4
  %36 = load i32, ptr %3, align 4
  %37 = sub nsw i32 0, %36
  %38 = sitofp i32 %37 to float
  %39 = load float, ptr %4, align 4
  %40 = fneg float %39
  %41 = fadd float %38, %40
  store float %41, ptr %5, align 4
  %42 = load float, ptr %5, align 4
  %43 = call float @print_float(float noundef %42)
  %44 = load float, ptr %6, align 4
  %45 = load float, ptr %5, align 4
  %46 = fadd float %44, %45
  store float %46, ptr %6, align 4
  %47 = load float, ptr %6, align 4
  ret float %47
}

declare float @print_float(float noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.1.0"}
