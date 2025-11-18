; ModuleID = 'mini-c'
source_filename = "mini-c"

declare i32 @print_int(i32)

define i32 @addition(i32 %n, i32 %m) {
entry:
  %result = alloca i32, align 4
  %m2 = alloca i32, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  store i32 %m, ptr %m2, align 4
  store i32 0, ptr %result, align 4
  %n3 = load i32, ptr %n1, align 4
  %m4 = load i32, ptr %m2, align 4
  %add = add i32 %n3, %m4
  store i32 %add, ptr %result, align 4
  %n5 = load i32, ptr %n1, align 4
  %eq = icmp eq i32 %n5, 4
  br i1 %eq, label %then, label %else

then:                                             ; preds = %entry
  %n6 = load i32, ptr %n1, align 4
  %m7 = load i32, ptr %m2, align 4
  %add8 = add i32 %n6, %m7
  %calltmp = call i32 @print_int(i32 %add8)
  br label %ifcont

else:                                             ; preds = %entry
  %n9 = load i32, ptr %n1, align 4
  %m10 = load i32, ptr %m2, align 4
  %mul = mul i32 %n9, %m10
  %calltmp11 = call i32 @print_int(i32 %mul)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %result12 = load i32, ptr %result, align 4
  ret i32 %result12
}
