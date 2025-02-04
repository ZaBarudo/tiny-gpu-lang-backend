; ModuleID = 'index.bc'
source_filename = "index.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define dso_local noundef range(i32 -2147483647, -2147483648) i32 @_Z3addii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %add = add i32 %a, 1
  %add1 = add i32 %add, %b
  ret i32 %add1
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 801c626889eab5afdc627ac2aa03bfc429f41f75)"}
