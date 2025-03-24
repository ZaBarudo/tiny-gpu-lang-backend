; ModuleID = 'test.cl'
source_filename = "test.cl"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: convergent mustprogress nofree norecurse nounwind willreturn memory(argmem: write) uwtable
define dso_local spir_kernel void @test_kernel(ptr nocapture noundef writeonly align 4 %data) local_unnamed_addr #0 !kernel_arg_addr_space !7 !kernel_arg_access_qual !8 !kernel_arg_type !9 !kernel_arg_base_type !9 !kernel_arg_type_qual !10 {
entry:
  %call = tail call i64 @_Z13get_global_idj(i32 noundef 0) #2
  %conv = trunc i64 %call to i32
  %sext = shl i64 %call, 32
  %0 = ashr exact i64 %sext, 30
  %arrayidx = getelementptr inbounds i8, ptr %data, i64 %0
  store i32 %conv, ptr %arrayidx, align 4, !tbaa !11
  ret void
}

; Function Attrs: convergent mustprogress nofree nounwind willreturn memory(none)
declare i64 @_Z13get_global_idj(i32 noundef) local_unnamed_addr #1

attributes #0 = { convergent mustprogress nofree norecurse nounwind willreturn memory(argmem: write) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "uniform-work-group-size"="false" }
attributes #1 = { convergent mustprogress nofree nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { convergent nounwind willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!opencl.ocl.version = !{!5}
!llvm.ident = !{!6}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{i32 2, i32 0}
!6 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 4b20d6a489fd0b17ddcaba2039a98e02dce8b2ab)"}
!7 = !{i32 1}
!8 = !{!"none"}
!9 = !{!"int*"}
!10 = !{!""}
!11 = !{!12, !12, i64 0}
!12 = !{!"int", !13, i64 0}
!13 = !{!"omnipotent char", !14, i64 0}
!14 = !{!"Simple C/C++ TBAA"}
