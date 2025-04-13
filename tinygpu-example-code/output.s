	.file	"if.cl"
	.text
	.globl	simple_matrix_ifelse            ; -- Begin function simple_matrix_ifelse
	.type	simple_matrix_ifelse,@function
simple_matrix_ifelse:                   ; @simple_matrix_ifelse
	.cfi_startproc
; %bb.0:                                ; %entry
	MUL	R0, %blockIdx, %blockDim
	ADD	R0, R0, %threadIdx
	CONST	 R1, #8
	ADD	R1, R0, R1
	LDR	R1, R1
	LDR	R2, R0
	CMP	R2, R1
	BRnz	LBB0b
	CMP R0 R0
 	BRz	LBB0a
LBB0a:                                ; %if.then
	CONST	 R1, #16
	ADD	R0, R0, R1
	STR	R0, R2
	CMP R0 R0
 	BRz	LBB0c
LBB0b:                                ; %if.else
	CONST	 R2, #16
	ADD	R0, R0, R2
	STR	R0, R1
LBB0c:                                ; %if.end
	RET
.Lfunc_end0:
	.size	simple_matrix_ifelse, .Lfunc_end0-simple_matrix_ifelse
	.cfi_endproc
                                        ; -- End function
	.ident	"clang version 20.0.0git (https://github.com/ZaBarudo/tiny-gpu-lang-backend.git 41e7d1f6acf57a66afbbe24c73abd2abb16dce7b)"
	.section	".note.GNU-stack","",@progbits
