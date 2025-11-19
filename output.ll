; ModuleID = 'mini-c'
source_filename = "mini-c"

@a = common global [10 x i32] zeroinitializer
@b = common global [10 x [10 x i32]] zeroinitializer

declare i32 @print_int(i32)

define i32 @init_arrays() {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  br label %loop

loop:                                             ; preds = %afterloop, %entry
  %i1 = load i32, ptr %i, align 4
  %lt = icmp slt i32 %i1, 10
  br i1 %lt, label %body, label %afterloop19

body:                                             ; preds = %loop
  store i32 0, ptr %j, align 4
  br label %loop2

loop2:                                            ; preds = %body5, %body
  %j3 = load i32, ptr %j, align 4
  %lt4 = icmp slt i32 %j3, 10
  br i1 %lt4, label %body5, label %afterloop

body5:                                            ; preds = %loop2
  %i6 = load i32, ptr %i, align 4
  %add = add i32 %i6, 1
  %j7 = load i32, ptr %j, align 4
  %add8 = add i32 %j7, 1
  %mul = mul i32 %add, %add8
  %i9 = load i32, ptr %i, align 4
  %j10 = load i32, ptr %j, align 4
  %arrayidx = getelementptr [10 x [10 x i32]], ptr @b, i32 0, i32 %i9, i32 %j10
  store i32 %mul, ptr %arrayidx, align 4
  %j11 = load i32, ptr %j, align 4
  %add12 = add i32 %j11, 1
  store i32 %add12, ptr %j, align 4
  br label %loop2

afterloop:                                        ; preds = %loop2
  %i13 = load i32, ptr %i, align 4
  %add14 = add i32 %i13, 1
  %i15 = load i32, ptr %i, align 4
  %arrayidx16 = getelementptr [10 x i32], ptr @a, i32 0, i32 %i15
  store i32 %add14, ptr %arrayidx16, align 4
  %i17 = load i32, ptr %i, align 4
  %add18 = add i32 %i17, 1
  store i32 %add18, ptr %i, align 4
  br label %loop

afterloop19:                                      ; preds = %loop
  ret i32 0
}

define i32 @vector_weighted_total() {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %total = alloca i32, align 4
  store i32 0, ptr %total, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  %calltmp = call i32 @init_arrays()
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  store i32 0, ptr %total, align 4
  br label %loop

loop:                                             ; preds = %afterloop, %entry
  %i1 = load i32, ptr %i, align 4
  %lt = icmp slt i32 %i1, 10
  br i1 %lt, label %body, label %afterloop16

body:                                             ; preds = %loop
  store i32 0, ptr %j, align 4
  br label %loop2

loop2:                                            ; preds = %body5, %body
  %j3 = load i32, ptr %j, align 4
  %lt4 = icmp slt i32 %j3, 10
  br i1 %lt4, label %body5, label %afterloop

body5:                                            ; preds = %loop2
  %total6 = load i32, ptr %total, align 4
  %i7 = load i32, ptr %i, align 4
  %arrayidx = getelementptr [10 x i32], ptr @a, i32 0, i32 %i7
  %arrayelem = load i32, ptr %arrayidx, align 4
  %i8 = load i32, ptr %i, align 4
  %j9 = load i32, ptr %j, align 4
  %arrayidx10 = getelementptr [10 x [10 x i32]], ptr @b, i32 0, i32 %i8, i32 %j9
  %arrayelem11 = load i32, ptr %arrayidx10, align 4
  %mul = mul i32 %arrayelem, %arrayelem11
  %add = add i32 %total6, %mul
  store i32 %add, ptr %total, align 4
  %j12 = load i32, ptr %j, align 4
  %add13 = add i32 %j12, 1
  store i32 %add13, ptr %j, align 4
  br label %loop2

afterloop:                                        ; preds = %loop2
  %i14 = load i32, ptr %i, align 4
  %add15 = add i32 %i14, 1
  store i32 %add15, ptr %i, align 4
  br label %loop

afterloop16:                                      ; preds = %loop
  %total17 = load i32, ptr %total, align 4
  ret i32 %total17
}
