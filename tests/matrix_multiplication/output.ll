; ModuleID = 'mini-c'
source_filename = "mini-c"

declare float @print_float(float)

define i32 @matrix_mul(ptr %a, ptr %b, ptr %c, i32 %n) {
entry:
  %k = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %n4 = alloca i32, align 4
  %c3 = alloca ptr, align 8
  %b2 = alloca ptr, align 8
  %a1 = alloca ptr, align 8
  store ptr %a, ptr %a1, align 8
  store ptr %b, ptr %b2, align 8
  store ptr %c, ptr %c3, align 8
  store i32 %n, ptr %n4, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  store i32 0, ptr %k, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %afterloop37, %entry
  %i5 = load i32, ptr %i, align 4
  %n6 = load i32, ptr %n4, align 4
  %lt = icmp slt i32 %i5, %n6
  br i1 %lt, label %body, label %afterloop40

body:                                             ; preds = %loop
  store i32 0, ptr %j, align 4
  br label %loop7

loop7:                                            ; preds = %afterloop, %body
  %j8 = load i32, ptr %j, align 4
  %n9 = load i32, ptr %n4, align 4
  %lt10 = icmp slt i32 %j8, %n9
  br i1 %lt10, label %body11, label %afterloop37

body11:                                           ; preds = %loop7
  store i32 0, ptr %k, align 4
  br label %loop12

loop12:                                           ; preds = %body16, %body11
  %k13 = load i32, ptr %k, align 4
  %n14 = load i32, ptr %n4, align 4
  %lt15 = icmp slt i32 %k13, %n14
  br i1 %lt15, label %body16, label %afterloop

body16:                                           ; preds = %loop12
  %c_ptr = load ptr, ptr %c3, align 8
  %i17 = load i32, ptr %i, align 4
  %j18 = load i32, ptr %j, align 4
  %arrayidx0 = getelementptr [10 x float], ptr %c_ptr, i32 %i17
  %arrayidx1 = getelementptr float, ptr %arrayidx0, i32 %j18
  %arrayelem = load float, ptr %arrayidx1, align 4
  %a_ptr = load ptr, ptr %a1, align 8
  %i19 = load i32, ptr %i, align 4
  %k20 = load i32, ptr %k, align 4
  %arrayidx021 = getelementptr [10 x float], ptr %a_ptr, i32 %i19
  %arrayidx122 = getelementptr float, ptr %arrayidx021, i32 %k20
  %arrayelem23 = load float, ptr %arrayidx122, align 4
  %b_ptr = load ptr, ptr %b2, align 8
  %k24 = load i32, ptr %k, align 4
  %j25 = load i32, ptr %j, align 4
  %arrayidx026 = getelementptr [10 x float], ptr %b_ptr, i32 %k24
  %arrayidx127 = getelementptr float, ptr %arrayidx026, i32 %j25
  %arrayelem28 = load float, ptr %arrayidx127, align 4
  %fmul = fmul float %arrayelem23, %arrayelem28
  %fadd = fadd float %arrayelem, %fmul
  %c_ptr29 = load ptr, ptr %c3, align 8
  %i30 = load i32, ptr %i, align 4
  %j31 = load i32, ptr %j, align 4
  %arrayidx032 = getelementptr [10 x float], ptr %c_ptr29, i32 %i30
  %arrayidx133 = getelementptr float, ptr %arrayidx032, i32 %j31
  store float %fadd, ptr %arrayidx133, align 4
  %k34 = load i32, ptr %k, align 4
  %add = add i32 %k34, 1
  store i32 %add, ptr %k, align 4
  br label %loop12

afterloop:                                        ; preds = %loop12
  %j35 = load i32, ptr %j, align 4
  %add36 = add i32 %j35, 1
  store i32 %add36, ptr %j, align 4
  br label %loop7

afterloop37:                                      ; preds = %loop7
  %i38 = load i32, ptr %i, align 4
  %add39 = add i32 %i38, 1
  store i32 %add39, ptr %i, align 4
  br label %loop

afterloop40:                                      ; preds = %loop
  ret i32 0
}
