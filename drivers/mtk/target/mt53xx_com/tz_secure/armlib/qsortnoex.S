
	.global qsort


	.text

_partition_sort:
	push	{r8, r9, r10, r11, lr}
	mov	r10, r3
	mov	r8, #1
	push	{r4, r5, r6, r7}
	mov	r7, r2
	sub	sp, sp, #260
	str	r0, [sp, #132]
	str	r1, [sp, #4]
.L20:	sub	r8, r8, #1
	add	r0, sp, #132
	ldr	r6, [r0, r8, lsl #2]
	add	r0, sp, #4
	ldr	r9, [r0, r8, lsl #2]
.L34:	add	lr, pc, #32
	tst	r9, #1
	asr	r0, r9, #1
	mul	r0, r7, r0
	add	r4, r6, r0
	subeq	r0, r0, r7
	mov	r1, r4
	add	r5, r4, r0
	mov	r0, r6
	bx	r10
	cmp	r0, #0
	ble	.Lb0
	mov	r0, r7
	orr	r1, r7, r6
	tst	r1, #3
	bne	.L90
.L74:	ldr	r2, [r4]
	ldr	r1, [r6]
	subs	r0, r0, #4
	str	r2, [r6], #4
	str	r1, [r4], #4
	bne	.L74
	b	.La8
.L90:	ldrb	r2, [r4]
	ldrb	r1, [r6]
	subs	r0, r0, #1
	strb	r2, [r6], #1
	strb	r1, [r4], #1
	bne	.L90
.La8:	sub	r6, r6, r7
	sub	r4, r4, r7
.Lb0:	mov	r1, r5
	add	lr, pc, #4
	mov	r0, r4
	bx	r10
	cmp	r0, #0
	ble	.L178
	mov	r1, r5
	mov	r0, r6
	mov	lr, pc
	bx	r10
	cmp	r0, #0
	ble	.L12c
	mov	r0, r7
	orr	r1, r7, r6
	tst	r1, #3
	bne	.L10c
.Lf0:	ldr	r2, [r5]
	ldr	r1, [r6]
	subs	r0, r0, #4
	str	r2, [r6], #4
	str	r1, [r5], #4
	bne	.Lf0
	b	.L124
.L10c:	ldrb	r2, [r5]
	ldrb	r1, [r6]
	subs	r0, r0, #1
	strb	r2, [r6], #1
	strb	r1, [r5], #1
	bne	.L10c
.L124:	sub	r6, r6, r7
	sub	r5, r5, r7
.L12c:	mov	r0, r7
	orr	r1, r7, r4
	tst	r1, #3
	bne	.L158
.L13c:	ldr	r2, [r5]
	ldr	r1, [r4]
	subs	r0, r0, #4
	str	r2, [r4], #4
	str	r1, [r5], #4
	bne	.L13c
	b	.L170
.L158:	ldrb	r2, [r5]
	ldrb	r1, [r4]
	subs	r0, r0, #1
	strb	r2, [r4], #1
	strb	r1, [r5], #1
	bne	.L158
.L170:	sub	r4, r4, r7
	sub	r5, r5, r7
.L178:	mov	r1, r7
	orr	r2, r7, r4
	tst	r2, #3
	sub	r0, r5, r7
	bne	.L1a8
.L18c:	ldr	r3, [r0]
	ldr	r2, [r4]
	subs	r1, r1, #4
	str	r3, [r4], #4
	str	r2, [r0], #4
	bne	.L18c
	b	.L1c0
.L1a8:	ldrb	r3, [r0]
	ldrb	r2, [r4]
	subs	r1, r1, #1
	strb	r3, [r4], #1
	strb	r2, [r0], #1
	bne	.L1a8
.L1c0:	mov	r5, r6
	sub	r4, r0, r7
	mov	r11, r4
.L1cc:	add	r5, r5, r7
	add	lr, pc, #8
	mov	r1, r11
	mov	r0, r5
	bx	r10
	cmp	r0, #0
	blt	.L1cc
.L1e8:	mov	r1, r11
	sub	r4, r4, r7
	add	lr, pc, #4
	mov	r0, r4
	bx	r10
	cmp	r0, #0
	bgt	.L1e8
	mov	r0, r7
	orr	r1, r7, r5
	cmp	r4, r5
	and	r1, r1, #3
	bls	.L260
	cmp	r1, #0
	bne	.L23c
.L220:	ldr	r2, [r4]
	ldr	r1, [r5]
	subs	r0, r0, #4
	str	r2, [r5], #4
	str	r1, [r4], #4
	bne	.L220
	b	.L254
.L23c:	ldrb	r2, [r4]
	ldrb	r1, [r5]
	subs	r0, r0, #1
	strb	r2, [r5], #1
	strb	r1, [r4], #1
	bne	.L23c
.L254:	sub	r5, r5, r7
	sub	r4, r4, r7
	b	.L1cc
.L260:	cmp	r1, #0
	bne	.L284
.L268:	ldr	r2, [r11]
	ldr	r1, [r5]
	subs	r0, r0, #4
	str	r2, [r5], #4
	str	r1, [r11], #4
	bne	.L268
	b	.L29c
.L284:	ldrb	r2, [r11]
	ldrb	r1, [r5]
	subs	r0, r0, #1
	strb	r2, [r5], #1
	strb	r1, [r11], #1
	bne	.L284
.L29c:	mov	r1, r7
	sub	r4, r5, r7
	sub	r0, r4, r6
	bl	__aeabi_uidivmod
	sub	r0, r0, #1
	sub	r1, r9, r0
	sub	r9, r1, #1
	cmp	r0, r9
	ble	.L2ec
	cmp	r9, #10
	addgt	r1, sp, #132
	strgt	r6, [r1, r8, lsl #2]
	addgt	r1, sp, #4
	strgt	r0, [r1, r8, lsl #2]
	addgt	r8, r8, #1
	bgt	.L310
	cmp	r0, #10
	ble	.L318
.L2e4:	mov	r9, r0
	b	.L34
.L2ec:	cmp	r0, #10
	addgt	r1, sp, #132
	strgt	r4, [r1, r8, lsl #2]
	addgt	r1, sp, #4
	strgt	r9, [r1, r8, lsl #2]
	addgt	r8, r8, #1
	bgt	.L2e4
	cmp	r9, #10
	ble	.L318
.L310:	mov	r6, r4
	b	.L34
.L318:	cmp	r8, #0
	bne	.L20
	add	sp, sp, #260
	pop	{r4, r5, r6, r7, r8}
	pop	{r9, r10, r11, pc}

qsort:
	push	{r7, r8, r9, r10, lr}
	mov	r8, r1
	mov	r7, r3
	push	{r4, r5, r6}
	mov	r6, r0
	movs	r4, r2
	cmpne	r8, #0
	beq	.L428
	cmp	r8, #10
	movhi	r3, r7
	movhi	r2, r4
	movhi	r1, r8
	movhi	r0, r6
	blhi	_partition_sort
	mov	r5, r6
	sub	r0, r8, #1
	mla	r8, r0, r4, r6
	b	.L420
.L374:	mov	r9, r5
	add	r5, r5, r4
	b	.L384
.L380:	sub	r9, r9, r4
.L384:	cmp	r9, r6
	bcc	.L3a4
	mov	r1, r5
	mov	r0, r9
	mov	lr, pc
	bx	r7
	cmp	r0, #0
	bgt	.L380
.L3a4:	mov	r1, #0
	orr	r0, r4, r5
	tst	r0, #3
	add	r2, r9, r4
	bne	.L418
	b	.L3e4
.L3bc:	add	r0, r5, r1
	ldr	r3, [r0]
	b	.L3d0
.L3c8:	ldr	ip, [r0]
	str	ip, [r0, r4]
.L3d0:	sub	r0, r0, r4
	cmp	r0, r2
	bcs	.L3c8
	add	r1, r1, #4
	str	r3, [r0, r4]
.L3e4:	cmp	r1, r4
	bcc	.L3bc
	b	.L420
.L3f0:	add	r0, r5, r1
	ldrb	r3, [r0]
	b	.L404
.L3fc:	ldrb	ip, [r0]
	strb	ip, [r0, r4]
.L404:	sub	r0, r0, r4
	cmp	r0, r2
	bcs	.L3fc
	add	r1, r1, #1
	strb	r3, [r0, r4]
.L418:	cmp	r1, r4
	bcc	.L3f0
.L420:	cmp	r5, r8
	bcc	.L374
.L428:	pop	{r4, r5, r6, r7, r8}
	pop	{r9, r10, pc}
