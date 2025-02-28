	.file	"index6.cpp"
	.text
	.globl	_Z6branchii                     ; -- Begin function _Z6branchii
	.type	_Z6branchii,@function
_Z6branchii:                            ; @_Z6branchii
; %bb.0:                                ; %entry
	CONST	R0, #12
	ADD	SP, SP, R0
	STR	R2, R1
	STR	R3, R0
	CONST	R1 , #-8
	ADD	R0, SP, R1
	STR	R3, -12
	ADD	R1, SP, R1
	STR	R2, R1
	LDR	R0, R0
	LDR	R1, R1
	CMP	R0, R1
	BRz	.LBB0_2
	BR	.LBB0_1
.LBB0_1:                                ; %if.then
	CONST	R1 , #-8
	ADD	R0, SP, R1
	LDR	R0, -4
	ADD	R1, SP, R1
	STR	R0, R1
	BR	.LBB0_3
.LBB0_2:                                ; %if.else
	CONST	R1 , #-12
	ADD	R0, SP, R1
	LDR	R0, -4
	ADD	R1, SP, R1
	STR	R0, R1
	BR	.LBB0_3
.LBB0_3:                                ; %return
	CONST	R0 , #-4
	ADD	R0, SP, R0
	LDR	R0, R0
	CONST	R0, #12
	ADD	SP, SP, R0
	RET
.Lfunc_end0:
	.size	_Z6branchii, .Lfunc_end0-_Z6branchii
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git b7bf111ba91f3be838ebaa3bcbce0d49b15cbce9)"
	.section	".note.GNU-stack","",@progbits
