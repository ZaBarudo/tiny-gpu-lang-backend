	.file	"index.cpp"
	.text
	.globl	_Z3addii                        ; -- Begin function _Z3addii
	.type	_Z3addii,@function
_Z3addii:                               ; @_Z3addii
; %bb.0:                                ; %entry
	ADD	r0, r0, r1
	CONST	r1 , #1
	ADD	r0, r0, r1
	RET
.Lfunc_end0:
	.size	_Z3addii, .Lfunc_end0-_Z3addii
                                        ; -- End function
	.ident	"clang version 20.0.0git (git@github.com:ZaBarudo/tiny-gpu-lang-backend.git 801c626889eab5afdc627ac2aa03bfc429f41f75)"
	.section	".note.GNU-stack","",@progbits
