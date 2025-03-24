; ModuleID = 'temp.cl'
source_filename = "temp.cl"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: convergent mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite)
define dso_local spir_kernel void @vector_add(ptr nocapture noundef readonly align 4 %A, ptr nocapture noundef readonly align 4 %B, ptr nocapture noundef writeonly align 4 initializes((16, 20)) %C, i32 noundef %N) local_unnamed_addr #0 !kernel_arg_addr_space !4 !kernel_arg_access_qual !5 !kernel_arg_type !6 !kernel_arg_base_type !6 !kernel_arg_type_qual !7 {
entry:
  %call = tail call i32 @_Z13get_global_idj(i32 noundef 0) #2

  ret void
}

; Function Attrs: convergent mustprogress nofree nounwind willreturn memory(none)
declare dso_local i32 @_Z13get_global_idj(i32 noundef) local_unnamed_addr #1

attributes #0 = { convergent mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "uniform-work-group-size"="true" }
attributes #1 = { convergent mustprogress nofree nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #2 = { convergent nounwind willreturn memory(none) }

!llvm.module.flags = !{!0, !1}
!opencl.ocl.version = !{!2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{i32 1, i32 2}
!3 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 58fea6382f88cfeed94a0c285bebdb2e9c247b34)"}
!4 = !{i32 1, i32 1, i32 1, i32 0}
!5 = !{!"none", !"none", !"none", !"none"}
!6 = !{!"int*", !"int*", !"int*", !"int"}
!7 = !{!"const", !"const", !"", !""}
!8 = !{!9, !9, i64 0}
!9 = !{!"int", !10, i64 0}
!10 = !{!"omnipotent char", !11, i64 0}
!11 = !{!"Simple C/C++ TBAA"}
