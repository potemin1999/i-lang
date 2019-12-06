@hw_const = private unnamed_addr constant [20 x i8] c"hello world! %f %d\0A\00", align 1

define i32 @main() {
entrypoint:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @hw_const, i32 0, i32 0), double 4.200000e+01, i32 42)
  ret i32 42
}

declare i32 @printf(i8*, ...)