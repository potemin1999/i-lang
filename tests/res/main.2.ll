
%record.HW = type { i32, float, double }

@hw_const = private unnamed_addr constant [17 x i8] c"hello world! %f\0A\00", align 1

declare i32 @printf(i8*, ...)

define void @print(%record.HW* noalias sret %_ret) {
entrypoint:
  %HW.fp_ptr = getelementptr %record.HW, %record.HW* %_ret, i32 1
  %HW.fp = load %record.HW, %record.HW* %HW.fp_ptr
  %printf.ret = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @hw_const, i32 0, i32 0), %record.HW %HW.fp)
  ret void
}

define i32 @main() {
entrypoint:
  %0 = alloca %record.HW
  call void @print(%record.HW* %0)
  ret i32 0
}