	.file	"index9.cpp"
	.text
	.globl	_Z9dummyFuncii                  ; -- Begin function _Z9dummyFuncii
	.type	_Z9dummyFuncii,@function
_Z9dummyFuncii:                         ; @_Z9dummyFuncii
; %bb.0:                                ; %entry
	CONST	R0, #8
	ADD	SP, SP, R0
	CONST	R1 , #-4
	ADD	R0, SP, R1
	STR	R8, -8
	ADD	R1, SP, R1
	STR	R9, R1
	LDR	R0, R0
	LDR	R1, R1
	ADD	R0, R0, R1
	CONST	R1 , #1
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
	CONST	R1 , #-4
	ADD	R2, SP, R1
	CONST	-8, #0
	ADD	R2, SP, R1
	CONST	-12, #1
	ADD	R1, SP, R1
	CONST	R1, #2
	CONST	R0, #12
	ADD	SP, SP, R0
	RET
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 58fea6382f88cfeed94a0c285bebdb2e9c247b34)"
	.section	".note.GNU-stack","",@progbits
