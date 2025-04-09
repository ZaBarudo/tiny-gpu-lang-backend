; ModuleID = 'index11.cpp'
source_filename = "index11.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define dso_local void @_Z15addValueToArrayPiii(ptr nocapture noundef %arr, i32 noundef %size, i32 noundef %value) local_unnamed_addr #0 {
entry:
  %0 = load i32, ptr %arr, align 4, !tbaa !3
  %add = add nsw i32 %0, %value
  store i32 %add, ptr %arr, align 4, !tbaa !3
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git a92829cf3f96796dbf49d989d3e210ed11bed91b)"}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C++ TBAA"}
