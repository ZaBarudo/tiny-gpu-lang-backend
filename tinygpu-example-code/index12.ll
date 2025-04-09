; ModuleID = 'index12.cpp'
source_filename = "index12.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local noundef i32 @_Z6branchii(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %cmp = icmp ne i32 %0, %1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, ptr %a.addr, align 4
  store i32 %2, ptr %retval, align 4
  br label %return

if.else:                                          ; preds = %entry
  %3 = load i32, ptr %a.addr, align 4
  %4 = load i32, ptr %b.addr, align 4
  %cmp1 = icmp sgt i32 %3, %4
  br i1 %cmp1, label %if.then2, label %if.else3

if.then2:                                         ; preds = %if.else
  %5 = load i32, ptr %b.addr, align 4
  store i32 %5, ptr %retval, align 4
  br label %return

if.else3:                                         ; preds = %if.else
  %6 = load i32, ptr %b.addr, align 4
  %7 = load i32, ptr %a.addr, align 4
  %cmp4 = icmp slt i32 %6, %7
  br i1 %cmp4, label %if.then5, label %if.else6

if.then5:                                         ; preds = %if.else3
  %8 = load i32, ptr %a.addr, align 4
  store i32 %8, ptr %retval, align 4
  br label %return

if.else6:                                         ; preds = %if.else3
  %9 = load i32, ptr %b.addr, align 4
  store i32 %9, ptr %retval, align 4
  br label %return

return:                                           ; preds = %if.else6, %if.then5, %if.then2, %if.then
  %10 = load i32, ptr %retval, align 4
  ret i32 %10
}

attributes #0 = { mustprogress noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git a92829cf3f96796dbf49d989d3e210ed11bed91b)"}
