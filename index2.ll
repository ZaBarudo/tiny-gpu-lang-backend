; Store a value into memory
define void @example(i32* %ptr, i32 %value) {
  ; Store the value in register %value into memory location pointed to by %ptr
  store i32 %value, i32* %ptr, align 4
  ret void
}