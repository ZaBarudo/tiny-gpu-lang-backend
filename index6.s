	.file	"index6.cpp"
	.text
	.globl	_Z6branchii                     ; -- Begin function _Z6branchii
	.type	_Z6branchii,@function
_Z6branchii:                            ; @_Z6branchii
; %bb.0:                                ; %entry
	addi	R0, R0, -12
	addi	R2, R0, -8
	STR	R0, R2
	addi	R0, R0, -12
	STR	R1, R0
	LDR	R1, R2
	LDR	R0, R0
	CMP	R1, R0
	BRz	.LBB0_2
	BR	.LBB0_1
.LBB0_1:                                ; %if.then
	addi	R0, R0, -8
	LDR	R0, R0
	addi	R1, R0, -4
	STR	R0, R1
	BR	.LBB0_3
.LBB0_2:                                ; %if.else
	addi	R0, R0, -12
	LDR	R0, R0
	addi	R1, R0, -4
	STR	R0, R1
	BR	.LBB0_3
.LBB0_3:                                ; %return
	addi	R0, R0, -4
	LDR	R0, R0
	addi	R0, R0, 12
	RET
.Lfunc_end0:
	.size	_Z6branchii, .Lfunc_end0-_Z6branchii
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git b7bf111ba91f3be838ebaa3bcbce0d49b15cbce9)"
	.section	".note.GNU-stack","",@progbits
