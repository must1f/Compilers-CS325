; ModuleID = 'mini-c'
source_filename = "mini-c"

define i32 @foo() {
entry:
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  store i32 42, ptr %x, align 4
  ret i32 0
}

define i32 @main() {
entry:
  %calltmp = call i32 @foo()
  ret i32 %calltmp
}
