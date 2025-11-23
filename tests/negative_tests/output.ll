; ModuleID = 'mini-c'
source_filename = "mini-c"

define i32 @foo(i32 %x) {
entry:
  %x2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 0, ptr %x2, align 4
  store i32 10, ptr %x2, align 4
  %x3 = load i32, ptr %x2, align 4
  ret i32 %x3
}
