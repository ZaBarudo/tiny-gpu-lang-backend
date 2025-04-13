; ModuleID = 'if.ll'
source_filename = "if.cl"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

define dso_local spir_kernel void @simple_matrix_ifelse() !kernel_arg_addr_space !4 !kernel_arg_access_qual !4 !kernel_arg_type !4 !kernel_arg_base_type !4 !kernel_arg_type_qual !4 {
entry:
  %call = call i32 @_Z13get_global_idj(i32 noundef 0) #1
  %add = add nsw i32 0, %call
  %0 = inttoptr i32 %add to ptr
  %1 = load i32, ptr %0, align 4
  %add1 = add nsw i32 8, %call
  %2 = inttoptr i32 %add1 to ptr
  %3 = load i32, ptr %2, align 4
  %cmp = icmp sgt i32 %1, %3
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %add2 = add nsw i32 16, %call
  %4 = inttoptr i32 %add2 to ptr
  store i32 %1, ptr %4, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %add3 = add nsw i32 16, %call
  %5 = inttoptr i32 %add3 to ptr
  store i32 %3, ptr %5, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: convergent nounwind willreturn memory(none)
declare dso_local i32 @_Z13get_global_idj(i32 noundef) #0

attributes #0 = { convergent nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { convergent nounwind willreturn memory(none) }

!llvm.module.flags = !{!0, !1}
!opencl.ocl.version = !{!2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{i32 1, i32 2}
!3 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 41e7d1f6acf57a66afbbe24c73abd2abb16dce7b)"}
!4 = !{}
