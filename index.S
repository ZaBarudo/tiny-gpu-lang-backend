	.file	"index7.ll"
	.text
	.globl	example                         ; -- Begin function example
	.type	example,@function
example:                                ; @example
	.cfi_startproc
; %bb.0:                                ; %entry
	BR	.LBB0_1
.LBB0_1:                                ; %next
	BR	.LBB0_2
.LBB0_2:                                ; %next2
	RET
.Lfunc_end0:
	.size	example, .Lfunc_end0-example
	.cfi_endproc
                                        ; -- End function
	.section	".note.GNU-stack","",@progbits
