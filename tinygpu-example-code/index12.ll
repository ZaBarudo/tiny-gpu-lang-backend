; ModuleID = 'index12.cpp'
source_filename = "index12.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define dso_local noundef i32 @_Z6branchii(i32 noundef returned %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  ret i32 %a
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 41e7d1f6acf57a66afbbe24c73abd2abb16dce7b)"}
