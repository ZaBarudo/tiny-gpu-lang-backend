; ModuleID = 'index.bc'
source_filename = "index.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

define i32 @main(i32 %a_input, i32 %b_input, i32 %c_input, i32 %d_input) {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 %a_input, ptr %a, align 4
  ret i32 %a_input
}

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git b0a905b576772ca82e31b5f3c193622e171e3ae2)"}
