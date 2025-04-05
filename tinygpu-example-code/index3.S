	.file	"index3.ll"
	.text
	.globl	example                         ; -- Begin function example
	.type	example,@function
example:                                ; @example
	.cfi_startproc
; %bb.0:
	CONST	r0, #12
	RET
.Lfunc_end0:
	.size	example, .Lfunc_end0-example
	.cfi_endproc
                                        ; -- End function
	.section	".note.GNU-stack","",@progbits
