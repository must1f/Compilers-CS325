; ModuleID = 'mini-c'
source_filename = "mini-c"

define i32 @factorial(i32 %n) {
entry:
  %factorial = alloca i32, align 4
  %i = alloca i32, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %factorial, align 4
  store i32 1, ptr %factorial, align 4
  store i32 1, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i2 = load i32, ptr %i, align 4
  %n3 = load i32, ptr %n1, align 4
  %le = icmp sle i32 %i2, %n3
  br i1 %le, label %body, label %afterloop

body:                                             ; preds = %loop
  %factorial4 = load i32, ptr %factorial, align 4
  %i5 = load i32, ptr %i, align 4
  %mul = mul i32 %factorial4, %i5
  store i32 %mul, ptr %factorial, align 4
  %i6 = load i32, ptr %i, align 4
  %add = add i32 %i6, 1
  store i32 %add, ptr %i, align 4
  br label %loop

afterloop:                                        ; preds = %loop
  %factorial7 = load i32, ptr %factorial, align 4
  ret i32 %factorial7
}
