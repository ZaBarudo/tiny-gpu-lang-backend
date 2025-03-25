; ModuleID = 'index8.ll'
source_filename = "index8.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local noundef i32 @_Z9getNumberii(i32 noundef %x, i32 noundef %y) #0 {
entry:
  %x.addr = alloca i32, align 4
  %y.addr = alloca i32, align 4
  store i32 %x, ptr %x.addr, align 4
  store i32 %y, ptr %y.addr, align 4
  %0 = load i32, ptr %x.addr, align 4
  %1 = load i32, ptr %y.addr, align 4
  %add = add nsw i32 %0, %1
  ret i32 %add
}

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local noundef i32 @_Z13callGetNumberv() #0 {
entry:
  %x = alloca i32, align 4
  %c = alloca i32, align 4
  %call = call noundef i32 @_Z9getNumberii(i32 noundef 32, i32 noundef 42)
  store i32 %call, ptr %x, align 4
  %0 = load i32, ptr %x, align 4
  %mul = mul nsw i32 2, %0
  store i32 %mul, ptr %c, align 4
  call void @llvm.trap()
  unreachable
}

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #1

attributes #0 = { mustprogress noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { cold noreturn nounwind memory(inaccessiblemem: write) }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 58fea6382f88cfeed94a0c285bebdb2e9c247b34)"}
