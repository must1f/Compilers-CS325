; ModuleID = 'mini-c'
source_filename = "mini-c"

declare i32 @print_int(i32)

define i32 @test_init() {
entry:
  %i = alloca i32, align 4
  %arr = alloca [5 x i32], align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i1 = load i32, ptr %i, align 4
  %lt = icmp slt i32 %i1, 5
  br i1 %lt, label %body, label %afterloop

body:                                             ; preds = %loop
  %i2 = load i32, ptr %i, align 4
  %mul = mul i32 %i2, 10
  %i3 = load i32, ptr %i, align 4
  %arrayidx = getelementptr [5 x i32], ptr %arr, i32 0, i32 %i3
  store i32 %mul, ptr %arrayidx, align 4
  %i4 = load i32, ptr %i, align 4
  %add = add i32 %i4, 1
  store i32 %add, ptr %i, align 4
  br label %loop

afterloop:                                        ; preds = %loop
  %arrayidx5 = getelementptr [5 x i32], ptr %arr, i32 0, i32 2
  %arrayelem = load i32, ptr %arrayidx5, align 4
  ret i32 %arrayelem
}
