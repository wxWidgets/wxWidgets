; crc_lcc.asm, optimized CRC calculation function for Zip and UnZip, not
; copyrighted by Paul Kienitz and Christian Spieler.  Last revised 25 Mar 98.
;
; The code in this file has been copied verbatim from crc_i386.{asm|S};
; only the assembler syntax and metacommands have been adapted to
; the habits of the free LCC-Win32 C compiler package.
; This version of the code uses the "optimized for i686" variant of
; crc_i386.{asm|S}.
;
; For more information (and a revision log), look into the original
; source files.
;
	.text
	.file "crc32.c"
	.text
	.type	_crc32,function
_crc32:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ecx
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	.line	34
	.line	37
	movl	12(%ebp),%esi
	subl	%eax,%eax
	testl	%esi,%esi
	jz	_$3
	.line	39
	call	_get_crc_table
	movl	%eax,%edi
	.line	41
	movl	8(%ebp),%eax
	movl	16(%ebp),%ecx
	notl	%eax
	testl	%ecx,%ecx
	jz	_$4
_$5:
	testl	$3,%esi
	jz	_$6
	xorb    (%esi),%al
	incl	%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$5
_$6:
	movl	%ecx,%edx
	andl	$7,%edx
	shrl	$3,%ecx
	jz	_$8
_$7:
	xorl	(%esi),%eax
	addl	$4,%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	xorl	(%esi),%eax
	addl	$4,%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$7
_$8:
	movl	%edx,%ecx
	jecxz	_$4
_$9:
	xorb    (%esi),%al
	incl    %esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$9
_$4:
	xorl	$0xffffffff,%eax
_$3:
	.line	52
	popl	%edi
	popl	%esi
	popl	%ebx
	leave
	ret
_$34:
	.size	_crc32,_$34-_crc32
	.globl	_crc32
	.extern	_get_crc_table
