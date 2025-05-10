define void @example() {
entry:
  br label %next



next:
  br label %next2

next2:
  ret void
}
