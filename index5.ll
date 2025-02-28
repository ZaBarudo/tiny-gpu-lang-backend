; ModuleID = 'index5.bc'
source_filename = "index5.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

@__const._Z3addv.c = private unnamed_addr constant [5 x i32] [i32 0, i32 1, i32 2, i32 3, i32 4], align 4

; Function Attrs: mustprogress noinline nounwind optnone
define dso_local noundef i32 @_Z3addv() #0 {
entry:
  %c = alloca [5 x i32], align 4
  call void @llvm.memcpy.p0.p0.i32(ptr align 4 %c, ptr align 4 @__const._Z3addv.c, i32 20, i1 false)
  %arrayidx = getelementptr inbounds [5 x i32], ptr %c, i32 0, i32 0
  %0 = load i32, ptr %arrayidx, align 4
  %arrayidx1 = getelementptr inbounds [5 x i32], ptr %c, i32 0, i32 1
  %1 = load i32, ptr %arrayidx1, align 4
  %add = add nsw i32 %0, %1
  %arrayidx2 = getelementptr inbounds [5 x i32], ptr %c, i32 0, i32 2
  %2 = load i32, ptr %arrayidx2, align 4
  %add3 = add nsw i32 %add, %2
  %arrayidx4 = getelementptr inbounds [5 x i32], ptr %c, i32 0, i32 3
  %3 = load i32, ptr %arrayidx4, align 4
  %add5 = add nsw i32 %add3, %3
  %arrayidx6 = getelementptr inbounds [5 x i32], ptr %c, i32 0, i32 4
  %4 = load i32, ptr %arrayidx6, align 4
  %add7 = add nsw i32 %add5, %4
  ret i32 %add7
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i32(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i32, i1 immarg) #1

attributes #0 = { mustprogress noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git c5bd2c31f7faf766d93a64cf43ed704c90fddd36)"}
