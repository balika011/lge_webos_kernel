
	.text
	.global	strcmp

strcmp:
	push	{r4, lr}
	ldr	r2, [r0], #4
	ldr	lr, =0x01010101
.Lc:	ldr	r3, [r1], #4
	ldr	r4, [r0], #4
	uqsub8	ip, lr, r2
	cmp	r2, r3
	cmpeq	ip, #0
	bne	.L40
	ldr	r3, [r1], #4
	ldr	r2, [r0], #4
	uqsub8	ip, lr, r4
	cmp	r4, r3
	cmpeq	ip, #0
	beq	.Lc
	mov	r2, r4
.L40:	lsl	r0, r3, #24
	subs	r0, r0, r2, lsl #24
	lsleqs	r1, ip, #31
	bne	.L70
	lsl	r0, r3, #16
	subs	r0, r0, r2, lsl #16
	tsteq	ip, #0xff00
	bne	.L70
	lsl	r0, r3, #8
	subs	r0, r0, r2, lsl #8
	tsteq	ip, #0xff0000
	subeqs	r0, r3, r2
.L70:	movne	r0, r0, rrx
	pop	{r4, pc}
