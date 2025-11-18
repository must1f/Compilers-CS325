	.file	"mini-c"
	.text
	.globl	While                           # -- Begin function While
	.p2align	4
	.type	While,@function
While:                                  # @While
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movl	$0, (%rsp)
	movq	test@GOTPCREL(%rip), %rax
	movl	$12, (%rax)
	movl	$12, %edi
	callq	print_int@PLT
	cmpl	$9, (%rsp)
	jg	.LBB0_3
	.p2align	4
.LBB0_2:                                # %body
                                        # =>This Inner Loop Header: Depth=1
	incl	(%rsp)
	cmpl	$9, (%rsp)
	jle	.LBB0_2
.LBB0_3:                                # %afterloop
	movl	(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	While, .Lfunc_end0-While
	.cfi_endproc
                                        # -- End function
	.type	test,@object                    # @test
	.comm	test,4,4
	.type	f,@object                       # @f
	.comm	f,4,4
	.type	b,@object                       # @b
	.comm	b,1,1
	.section	".note.GNU-stack","",@progbits
