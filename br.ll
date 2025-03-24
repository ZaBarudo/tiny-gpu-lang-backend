; ModuleID = 'example'
source_filename = "example"

define void @main() {
entry:
  ; Unconditional jump to label 'next'
  br label %next

next:
  ; This is the target of the unconditional jump
  ret void
}
