; ModuleID = 'matadd2.cl'
source_filename = "matadd2.cl"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: convergent mustprogress nofree norecurse nounwind willreturn memory(readwrite, inaccessiblemem: none)
define dso_local spir_kernel void @vector_add() local_unnamed_addr #0 !kernel_arg_addr_space !4 !kernel_arg_access_qual !4 !kernel_arg_type !4 !kernel_arg_base_type !4 !kernel_arg_type_qual !4 {
entry:
  %call = tail call i32 @_Z13get_global_idj(i32 noundef 0) #2
  %0 = inttoptr i32 %call to ptr
  %1 = load i32, ptr %0, align 4, !tbaa !5
  %add1 = add nsw i32 %call, 8
  %2 = inttoptr i32 %add1 to ptr
  %3 = load i32, ptr %2, align 4, !tbaa !5
  %add2 = add nsw i32 %3, %1
  %add3 = add nsw i32 %call, 16
  %4 = inttoptr i32 %add3 to ptr
  store i32 %add2, ptr %4, align 4, !tbaa !5
  ret void
}

; Function Attrs: convergent mustprogress nofree nounwind willreturn memory(none)
declare dso_local i32 @_Z13get_global_idj(i32 noundef) local_unnamed_addr #1

attributes #0 = { convergent mustprogress nofree norecurse nounwind willreturn memory(readwrite, inaccessiblemem: none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "uniform-work-group-size"="true" }
attributes #1 = { convergent mustprogress nofree nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #2 = { convergent nounwind willreturn memory(none) }

!llvm.module.flags = !{!0, !1}
!opencl.ocl.version = !{!2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{i32 1, i32 2}
!3 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 41e7d1f6acf57a66afbbe24c73abd2abb16dce7b)"}
!4 = !{}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
