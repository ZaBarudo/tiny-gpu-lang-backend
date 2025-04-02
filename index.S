	.file	"index8.cpp"
	.text
	.globl	_Z10callerFuncii                ; -- Begin function _Z10callerFuncii
	.type	_Z10callerFuncii,@function
_Z10callerFuncii:                       ; @_Z10callerFuncii
; %bb.0:                                ; %entry
	CONST	R0, #8
	ADD	SP, SP, R0
	STR	R2, R1
	STR	R3, R0
	CONST	R1 , #-4
	ADD	R0, SP, R1
	STR	R3, -8
	ADD	R1, SP, R1
	STR	R2, R1
	LDR	R0, R0
	LDR	R1, R1
	ADD	R0, R0, R1
	CONST	R1 , #1
	ADD	R0, R0, R1
	CONST	R0, #8
	ADD	SP, SP, R0
	RET
.Lfunc_end0:
	.size	_Z10callerFuncii, .Lfunc_end0-_Z10callerFuncii
                                        ; -- End function
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	CONST	R0, #16
	ADD	SP, SP, R0
	CONST	R1 , #-4
	ADD	R2, SP, R1
	CONST	-8, #0
	ADD	R2, SP, R1
	CONST	-12, #1
	ADD	R1, SP, R1
	CONST	R1, #2
	BR	_Z10callerFuncii
	CONST	R0, #16
	ADD	SP, SP, R0
	RET
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git c0de804f110d315ffffcdf76444b930775998196)"
	.section	".note.GNU-stack","",@progbits
