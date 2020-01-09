	.cpu arm7tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"fichA.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Hello word\000"
	.text
	.align	2
	.global	affichage_hello
	.type	affichage_hello, %function
affichage_hello:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r0, .L2
	bl	printf
	ldmfd	sp!, {fp, pc}
.L3:
	.align	2
.L2:
	.word	.LC0
	.size	affichage_hello, .-affichage_hello
	.align	2
	.global	suite_syracuse
	.type	suite_syracuse, %function
suite_syracuse:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	b	.L5
.L7:
	ldr	r0, [fp, #-8]
	bl	print
	ldr	r3, [fp, #-8]
	and	r3, r3, #1
	cmp	r3, #0
	bne	.L6
	ldr	r3, [fp, #-8]
	mov	r2, r3, lsr #31
	add	r3, r2, r3
	mov	r3, r3, asr #1
	str	r3, [fp, #-8]
	b	.L5
.L6:
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #1
	add	r3, r3, r2
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L5:
	ldr	r3, [fp, #-8]
	cmp	r3, #1
	bne	.L7
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
	.size	suite_syracuse, .-suite_syracuse
	.ident	"GCC: (GNU) 4.5.3"
