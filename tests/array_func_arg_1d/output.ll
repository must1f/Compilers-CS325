; ModuleID = 'mini-c'
source_filename = "mini-c"

declare i32 @print_int(i32)

define i32 @vector_total(ptr %a, ptr %b, i32 %n) {
entry:
  %total = alloca i32, align 4
  %i = alloca i32, align 4
  %n3 = alloca i32, align 4
  %b2 = alloca ptr, align 8
  %a1 = alloca ptr, align 8
  store ptr %a, ptr %a1, align 8
  store ptr %b, ptr %b2, align 8
  store i32 %n, ptr %n3, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %total, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i4 = load i32, ptr %i, align 4
  %n5 = load i32, ptr %n3, align 4
  %lt = icmp slt i32 %i4, %n5
  br i1 %lt, label %body, label %afterloop

body:                                             ; preds = %loop
  %total6 = load i32, ptr %total, align 4
  %a_ptr = load ptr, ptr %a1, align 8
  %i7 = load i32, ptr %i, align 4
  %arrayidx = getelementptr i32, ptr %a_ptr, i32 %i7
  %arrayelem = load i32, ptr %arrayidx, align 4
  %add = add i32 %total6, %arrayelem
  %b_ptr = load ptr, ptr %b2, align 8
  %i8 = load i32, ptr %i, align 4
  %arrayidx9 = getelementptr i32, ptr %b_ptr, i32 %i8
  %arrayelem10 = load i32, ptr %arrayidx9, align 4
  %add11 = add i32 %add, %arrayelem10
  store i32 %add11, ptr %total, align 4
  %i12 = load i32, ptr %i, align 4
  %add13 = add i32 %i12, 1
  store i32 %add13, ptr %i, align 4
  br label %loop

afterloop:                                        ; preds = %loop
  %total14 = load i32, ptr %total, align 4
  ret i32 %total14
}
