; ModuleID = 'mini-c'
source_filename = "mini-c"

define float @pi() {
entry:
  %i = alloca i32, align 4
  %PI = alloca float, align 4
  %flag = alloca i1, align 1
  store i1 false, ptr %flag, align 1
  store float 0.000000e+00, ptr %PI, align 4
  store i32 0, ptr %i, align 4
  store i1 true, ptr %flag, align 1
  store float 3.000000e+00, ptr %PI, align 4
  store i32 2, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %ifcont, %entry
  %i1 = load i32, ptr %i, align 4
  %lt = icmp slt i32 %i1, 100
  br i1 %lt, label %body, label %afterloop

body:                                             ; preds = %loop
  %flag2 = load i1, ptr %flag, align 1
  br i1 %flag2, label %then, label %else

then:                                             ; preds = %body
  %PI3 = load float, ptr %PI, align 4
  %i4 = load i32, ptr %i, align 4
  %i5 = load i32, ptr %i, align 4
  %add = add i32 %i5, 1
  %mul = mul i32 %i4, %add
  %i6 = load i32, ptr %i, align 4
  %add7 = add i32 %i6, 2
  %mul8 = mul i32 %mul, %add7
  %itof = sitofp i32 %mul8 to double
  %fdiv = fdiv double 4.000000e+00, %itof
  %fadd = fadd float %PI3, double %fdiv
  store float %fadd, ptr %PI, align 4
  br label %ifcont

else:                                             ; preds = %body
  %PI9 = load float, ptr %PI, align 4
  %i10 = load i32, ptr %i, align 4
  %i11 = load i32, ptr %i, align 4
  %add12 = add i32 %i11, 1
  %mul13 = mul i32 %i10, %add12
  %i14 = load i32, ptr %i, align 4
  %add15 = add i32 %i14, 2
  %mul16 = mul i32 %mul13, %add15
  %itof17 = sitofp i32 %mul16 to double
  %fdiv18 = fdiv double 4.000000e+00, %itof17
  %fsub = fsub float %PI9, double %fdiv18
  store float %fsub, ptr %PI, align 4
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %flag19 = load i1, ptr %flag, align 1
  %not = xor i1 %flag19, true
  store i1 %not, ptr %flag, align 1
  %i20 = load i32, ptr %i, align 4
  %add21 = add i32 %i20, 2
  store i32 %add21, ptr %i, align 4
  br label %loop

afterloop:                                        ; preds = %loop
  %PI22 = load float, ptr %PI, align 4
  ret float %PI22
}
