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
	CONST	R11, #8
	ADD	R12, R12, R11
	RET
.Lfunc_end0:
	.size	_Z10callerFuncii, .Lfunc_end0-_Z10callerFuncii
                                        ; -- End function
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	CONST	R11, #16
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
	CMP R0 R0
 	BRz	_Z10callerFuncii
	CONST	R11, #16
	ADD	R12, R12, R11
	RET
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git c0de804f110d315ffffcdf76444b930775998196)"
	.section	".note.GNU-stack","",@progbits
