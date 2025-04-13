; ModuleID = 'index6.ll'
source_filename = "index6.cpp"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tinygpu"

define dso_local noundef i32 @_Z5mainnii(i32 noundef %a, i32 noundef %b) {
entry:
  %add = add nsw i32 %a, 1
  %add1 = add nsw i32 %add, 1
  %cmp = icmp ne i32 %add1, %b
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %return

if.else:                                          ; preds = %entry
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ %add, %if.then ], [ %b, %if.else ]
  ret i32 %retval.0
}

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 41e7d1f6acf57a66afbbe24c73abd2abb16dce7b)"}
