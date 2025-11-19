; ModuleID = 'mini-c'
source_filename = "mini-c"

declare i32 @print_int(i32)

define i32 @arr_addition(i32 %n, i32 %m) {
entry:
  %temp = alloca i32, align 4
  %idx = alloca i32, align 4
  %result = alloca i32, align 4
  %arr_result = alloca [10 x [5 x i32]], align 4
  %m2 = alloca i32, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  store i32 %m, ptr %m2, align 4
  store i32 0, ptr %result, align 4
  store i32 0, ptr %idx, align 4
  store i32 0, ptr %temp, align 4
  %m3 = load i32, ptr %m2, align 4
  %arrayidx = getelementptr [10 x [5 x i32]], ptr %arr_result, i32 0, i32 0, i32 0
  store i32 %m3, ptr %arrayidx, align 4
  %n4 = load i32, ptr %n1, align 4
  %arrayidx5 = getelementptr [10 x [5 x i32]], ptr %arr_result, i32 0, i32 0, i32 1
  store i32 %n4, ptr %arrayidx5, align 4
  %arrayidx6 = getelementptr [10 x [5 x i32]], ptr %arr_result, i32 0, i32 0, i32 0
  %arrayelem = load i32, ptr %arrayidx6, align 4
  %arrayidx7 = getelementptr [10 x [5 x i32]], ptr %arr_result, i32 0, i32 0, i32 1
  %arrayelem8 = load i32, ptr %arrayidx7, align 4
  %add = add i32 %arrayelem, %arrayelem8
  store i32 %add, ptr %temp, align 4
  %temp9 = load i32, ptr %temp, align 4
  ret i32 %temp9
}
