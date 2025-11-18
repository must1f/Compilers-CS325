; ModuleID = 'mini-c'
source_filename = "mini-c"

declare i32 @print_int(i32)

declare float @print_float(float)

define float @unary(i32 %n, float %m) {
entry:
  %sum = alloca float, align 4
  %result = alloca float, align 4
  %m2 = alloca float, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  store float %m, ptr %m2, align 4
  store float 0.000000e+00, ptr %result, align 4
  store float 0.000000e+00, ptr %sum, align 4
  store float 0.000000e+00, ptr %sum, align 4
  %n3 = load i32, ptr %n1, align 4
  %m4 = load float, ptr %m2, align 4
  %itof = sitofp i32 %n3 to float
  %fadd = fadd float %itof, %m4
  store float %fadd, ptr %result, align 4
  %result5 = load float, ptr %result, align 4
  %calltmp = call float @print_float(float %result5)
  %sum6 = load float, ptr %sum, align 4
  %result7 = load float, ptr %result, align 4
  %fadd8 = fadd float %sum6, %result7
  store float %fadd8, ptr %sum, align 4
  %n9 = load i32, ptr %n1, align 4
  %m10 = load float, ptr %m2, align 4
  %fneg = fneg float %m10
  %itof11 = sitofp i32 %n9 to float
  %fadd12 = fadd float %itof11, %fneg
  store float %fadd12, ptr %result, align 4
  %result13 = load float, ptr %result, align 4
  %calltmp14 = call float @print_float(float %result13)
  %sum15 = load float, ptr %sum, align 4
  %result16 = load float, ptr %result, align 4
  %fadd17 = fadd float %sum15, %result16
  store float %fadd17, ptr %sum, align 4
  %n18 = load i32, ptr %n1, align 4
  %m19 = load float, ptr %m2, align 4
  %fneg20 = fneg float %m19
  %fneg21 = fneg float %fneg20
  %itof22 = sitofp i32 %n18 to float
  %fadd23 = fadd float %itof22, %fneg21
  store float %fadd23, ptr %result, align 4
  %result24 = load float, ptr %result, align 4
  %calltmp25 = call float @print_float(float %result24)
  %sum26 = load float, ptr %sum, align 4
  %result27 = load float, ptr %result, align 4
  %fadd28 = fadd float %sum26, %result27
  store float %fadd28, ptr %sum, align 4
  %n29 = load i32, ptr %n1, align 4
  %neg = sub i32 0, %n29
  %m30 = load float, ptr %m2, align 4
  %fneg31 = fneg float %m30
  %itof32 = sitofp i32 %neg to float
  %fadd33 = fadd float %itof32, %fneg31
  store float %fadd33, ptr %result, align 4
  %result34 = load float, ptr %result, align 4
  %calltmp35 = call float @print_float(float %result34)
  %sum36 = load float, ptr %sum, align 4
  %result37 = load float, ptr %result, align 4
  %fadd38 = fadd float %sum36, %result37
  store float %fadd38, ptr %sum, align 4
  %sum39 = load float, ptr %sum, align 4
  ret float %sum39
}
