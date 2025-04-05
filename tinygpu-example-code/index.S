	.file	"index10.cpp"
	.text
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	CONST	R0, #28
	ADD	SP, SP, R0
	CONST	R0 , #-4
	ADD	R1, SP, R0
	CONST	-8, #0
	ADD	R1, SP, R0
	CONST	-12, #1
	ADD	R2, SP, R0
	CONST	R2, #2
	LDR	R1, R1
	LDR	R2, R2
	ADD	R1, R1, R2
	LDR	R2, c
	ADD	R1, -16, R2
	ADD	R2, SP, R0
	STR	R1, -20
	ADD	R1, SP, R0
	CONST	-24, #0
	ADD	R0, SP, R0
	CONST	R0, #0
	BR	.LBB0_1
.LBB0_1:                                ; %for.cond
                                        ; =>This Inner Loop Header: Depth=1
	CONST	R0 , #-24
	ADD	R0, SP, R0
	LDR	R0, R0
	CONST	R1 , #9
	CMP	R0, R1
	BRp	.LBB0_4
	BR	.LBB0_2
.LBB0_2:                                ; %for.body
                                        ;   in Loop: Header=BB0_1 Depth=1
	LDR	R0, c
	CONST	R1 , #1
	ADD	R0, R0, R1
	STR	R0, c
	BR	.LBB0_3
.LBB0_3:                                ; %for.inc
                                        ;   in Loop: Header=BB0_1 Depth=1
	CONST	R0 , #-24
	ADD	R1, SP, R0
	LDR	R0, R1
	CONST	R2 , #1
	ADD	R0, R0, R2
	STR	R0, R1
	BR	.LBB0_1
.LBB0_4:                                ; %for.end
	CONST	R1 , #-28
	ADD	R1, SP, R1
	CONST	R1, #0
	CONST	R0, #28
	ADD	SP, SP, R0
	RET
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        ; -- End function
	.type	c,@object                       ; @c
	.data
	.globl	c
	.p2align	2, 0x0
c:
	.word	11                              ; 0xb
	.size	c, 4

	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 6b85a9b73c4c141ef40330cac66f724642d33315)"
	.section	".note.GNU-stack","",@progbits
