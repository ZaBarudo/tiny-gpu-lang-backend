; ModuleID = 'matmul.cl'
source_filename = "matmul.cl"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

; Function Attrs: convergent mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite)
define dso_local spir_kernel void @add_vectors(ptr nocapture noundef readonly align 4 %a, ptr nocapture noundef readonly align 4 %b, ptr nocapture noundef writeonly align 4 %result, i32 noundef %count) local_unnamed_addr #0 !kernel_arg_addr_space !4 !kernel_arg_access_qual !5 !kernel_arg_type !6 !kernel_arg_base_type !6 !kernel_arg_type_qual !7 {
entry:
  %call = tail call i32 @_Z13get_global_idj(i32 noundef 0) #2
  %cmp = icmp ult i32 %call, %count
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %arrayidx = getelementptr inbounds float, ptr %a, i32 %call
  %0 = load float, ptr %arrayidx, align 4, !tbaa !8
  %arrayidx1 = getelementptr inbounds float, ptr %b, i32 %call
  %1 = load float, ptr %arrayidx1, align 4, !tbaa !8
  %add = fadd float %0, %1
  %arrayidx2 = getelementptr inbounds float, ptr %result, i32 %call
  store float %add, ptr %arrayidx2, align 4, !tbaa !8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
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
!3 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git c5bd2c31f7faf766d93a64cf43ed704c90fddd36)"}
!4 = !{i32 1, i32 1, i32 1, i32 0}
!5 = !{!"none", !"none", !"none", !"none"}
!6 = !{!"float*", !"float*", !"float*", !"uint"}
!7 = !{!"const", !"const", !"", !""}
!8 = !{!9, !9, i64 0}
!9 = !{!"float", !10, i64 0}
!10 = !{!"omnipotent char", !11, i64 0}
!11 = !{!"Simple C/C++ TBAA"}
