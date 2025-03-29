; ModuleID = 'index9.cpp'
source_filename = "index9.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

@total = dso_local global i32 91, align 4

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local void @_Z13callGetNumberv() #0 {
entry:
  %c = alloca i32, align 4
  %0 = load i32, ptr @total, align 4
  %mul = mul nsw i32 3, %0
  store i32 %mul, ptr %c, align 4
  ret void
}

attributes #0 = { mustprogress noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git e5faaf1c210bd30a596b13911def46ab66e4b58e)"}
