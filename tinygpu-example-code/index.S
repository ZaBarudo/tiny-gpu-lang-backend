	.file	"index9.cpp"
	.text
	.globl	_Z9dummyFuncii                  ; -- Begin function _Z9dummyFuncii
	.type	_Z9dummyFuncii,@function
_Z9dummyFuncii:                         ; @_Z9dummyFuncii
; %bb.0:                                ; %entry
	CONST	R0, #8
	ADD	SP, SP, R0
	CONST	 R1, #-4
	ADD	R0, R1, SP
	STR	R8, -8
	ADD	R1, R1, SP
	STR	R9, R1
	LDR	R0, R0
	LDR	R1, R1
	ADD	R0, R0, R1
	CONST	 R1, #1
	ADD	R0, R0, R1
	CONST	R0, #8
	ADD	SP, SP, R0
	RET
.Lfunc_end0:
	.size	_Z9dummyFuncii, .Lfunc_end0-_Z9dummyFuncii
                                        ; -- End function
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	CONST	R0, #12
	ADD	SP, SP, R0
	CONST	 R1, #-4
	ADD	R2, R1, SP
	CONST	R2, #-8
	ADD	R2, R1, SP
	CONST	R2, #-12
	ADD	R1, R1, SP
	CONST	R1, #2
	CONST	R0, #12
	ADD	SP, SP, R0
	RET
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git a92829cf3f96796dbf49d989d3e210ed11bed91b)"
	.section	".note.GNU-stack","",@progbits
