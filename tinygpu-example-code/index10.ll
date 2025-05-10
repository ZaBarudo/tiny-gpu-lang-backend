; ModuleID = 'index10.cpp'
source_filename = "index10.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

@c = dso_local global i32 11, align 4

; Function Attrs: mustprogress noinline norecurse nounwind optnone
define dso_local noundef i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %d = alloca i32, align 4
  %i = alloca i32, align 4
  %i2 = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 1, ptr %a, align 4
  store i32 2, ptr %b, align 4
  %0 = load i32, ptr %a, align 4
  %1 = load i32, ptr %b, align 4
  %add = add nsw i32 %0, %1
  %2 = load i32, ptr @c, align 4
  %add1 = add nsw i32 %add, %2
  store i32 %add1, ptr %d, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %i2, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %3 = load i32, ptr %i2, align 4
  %cmp = icmp slt i32 %3, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %4 = load i32, ptr @c, align 4
  %add3 = add nsw i32 %4, 1
  store i32 %add3, ptr @c, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %5 = load i32, ptr %i2, align 4
  %inc = add nsw i32 %5, 1
  store i32 %inc, ptr %i2, align 4
  br label %for.cond, !llvm.loop !3

for.end:                                          ; preds = %for.cond
  store i32 0, ptr %j, align 4
  ret i32 0
}

attributes #0 = { mustprogress noinline norecurse nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 6b85a9b73c4c141ef40330cac66f724642d33315)"}
!3 = distinct !{!3, !4}
!4 = !{!"llvm.loop.mustprogress"}
