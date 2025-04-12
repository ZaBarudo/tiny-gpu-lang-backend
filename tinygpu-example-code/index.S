	.file	"index8.cpp"
	.text
	.globl	_Z10callerFuncii                ; -- Begin function _Z10callerFuncii
	.type	_Z10callerFuncii,@function
_Z10callerFuncii:                       ; @_Z10callerFuncii
; %bb.0:                                ; %entry
	CONST	R11, #8
	ADD	R12, R12, R11
	CONST	 R1, #-4
	ADD	R0, R1, R12
	STR	R8, -8
	ADD	R1, R1, R12
	STR	R9, R1
	LDR	R0, R0
	LDR	R1, R1
	ADD	R0, R0, R1
	CONST	 R1, #1
	ADD	R0, R0, R1
	BR R11	
.Lfunc_end0:
	.size	_Z10callerFuncii, .Lfunc_end0-_Z10callerFuncii
                                        ; -- End function
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	CONST	R11, #28
	ADD	R12, R12, R11
	CONST	 R1, #-4
	ADD	R2, R1, R12
	CONST	R2, #-8
	ADD	R2, R1, R12
	CONST	R2, #-12
	ADD	R1, R1, R12
	CONST	R1, #2
	LDR	R2, R2
	LDR	R1, R1
	CONST	R11, #.LBB1_1
	CMP R0 R0
 	BRz	_Z10callerFuncii
	CMP R0 R0
 	BRz	.LBB1_1
.LBB1_1:                                ; %entry.afterCall
	CONST	 R1, #-16
	ADD	R3, R1, R12
	STR	R2, -20
	ADD	R2, R1, R12
	CONST	R2, #-24
	ADD	R1, R1, R12
	CONST	R1, #2
	BR R11	
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 5b593a4baab3a75f5e6b102b4e4a07f6249d75c8)"
	.section	".note.GNU-stack","",@progbits
