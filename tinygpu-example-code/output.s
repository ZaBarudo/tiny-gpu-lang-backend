	.file	"matmul.cl"
	.text
	.globl	vector_add                      ; -- Begin function vector_add
	.type	vector_add,@function
vector_add:                             ; @vector_add
; %bb.0:                                ; %entry
	MUL	R0, R13, R14
	ADD	R0, R0, R15
	CONST	 R1, #4
	MUL	R0, R0, R1
	ADD	R1, R8, R0
	LDR	R1, R1
	ADD	R2, R9, R0
	LDR	R2, R2
	ADD	R1, R2, R1
	ADD	R0, R10, R0
	STR	R1, R0
	RET
.Lfunc_end0:
	.size	vector_add, .Lfunc_end0-vector_add
                                        ; -- End function
	.ident	"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 36e18b1ced12584d21c46a2adb86db8d9f871b34)"
	.section	".note.GNU-stack","",@progbits
