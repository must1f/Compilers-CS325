	.file	"mini-c"
	.text
	.globl	addition                        # -- Begin function addition
	.p2align	4
	.type	addition,@function
addition:                               # @addition
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, 12(%rsp)
	movl	%esi, 16(%rsp)
	addl	%edi, %esi
	movl	%esi, 20(%rsp)
	cmpl	$4, %edi
	jne	.LBB0_2
# %bb.1:                                # %then
	movl	12(%rsp), %edi
	addl	16(%rsp), %edi
	jmp	.LBB0_3
.LBB0_2:                                # %else
	movl	12(%rsp), %edi
	imull	16(%rsp), %edi
.LBB0_3:                                # %ifcont
	callq	print_int@PLT
	movl	20(%rsp), %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	addition, .Lfunc_end0-addition
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
