
	.text
	.global memcmp

memcmp:
	orr	r3, r0, r1
	tst	r3, #3
	bne	.L64
.Lc:	cmp	r2, #4
	bcc	.L64
	ldr	r3, [r0], #4
	ldr	ip, [r1], #4
	sub	r2, r2, #4
	cmp	r3, ip
	beq	.Lc
	and	r0, r3, #0xff
	and	r1, ip, #0xff
	subs	r0, r0, r1
	lsleq	r0, r3, #16
	lsleq	r1, ip, #16
	lsreq	r0, r0, #16
	lsreq	r1, r1, #16
	subeqs	r0, r0, r1
	biceq	r0, r3, #0xff000000
	biceq	r1, ip, #0xff000000
	subeqs	r0, r0, r1
	bxne	lr
	lsr	r0, r3, #24
	sub	r0, r0, ip, lsr #24
	bx	lr
.L64:	cmp	r2, #0
	moveq	r0, #0
	bxeq	lr
	tst	r2, #1
	addne	r2, r2, #1
	bne	.L8c
.L7c:	ldrb	r3, [r0], #1
	ldrb	ip, [r1], #1
	subs	r3, r3, ip
	bne	.La4
.L8c:	ldrb	r3, [r0], #1
	ldrb	ip, [r1], #1
	subs	r3, r3, ip
	bne	.La4
	subs	r2, r2, #2
	bne	.L7c
.La4:	mov	r0, r3
	bx	lr

	.end
