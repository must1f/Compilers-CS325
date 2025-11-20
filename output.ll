; ModuleID = 'mini-c'
source_filename = "mini-c"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  store i32 5, ptr %x, align 4
  br i1 true, label %then, label %ifcont

then:                                             ; preds = %entry
  %x1 = load i32, ptr %x, align 4
  ret i32 %x1
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  ret i32 0
}
