; ModuleID = 'index11.cpp'
source_filename = "index11.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local void @_Z15addValueToArrayPiii(ptr noundef %arr, i32 noundef %size, i32 noundef %value) #0 {
entry:
  %arr.addr = alloca ptr, align 4
  %size.addr = alloca i32, align 4
  %value.addr = alloca i32, align 4
  store ptr %arr, ptr %arr.addr, align 4
  store i32 %size, ptr %size.addr, align 4
  store i32 %value, ptr %value.addr, align 4
  %0 = load i32, ptr %value.addr, align 4
  %1 = load ptr, ptr %arr.addr, align 4
  %arrayidx = getelementptr inbounds i32, ptr %1, i32 0
  %2 = load i32, ptr %arrayidx, align 4
  %add = add nsw i32 %2, %0
  store i32 %add, ptr %arrayidx, align 4
  ret void
}

attributes #0 = { mustprogress noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 62e2339300b7b3e7039c5f02b305378308a56fa8)"}
