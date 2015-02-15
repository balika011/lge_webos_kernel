
	.global	__aeabi_memmove
	.global	__rt_memmove
	.global	__memmove_lastfew


	.text

__aeabi_memmove:
__rt_memmove:
	subs	r3, r0, r1
	cmpcs	r2, r3
	bls	__aeabi_memcpy
	cmp	r2, #3
	add	r0, r0, r2
	add	r1, r1, r2
	bls	__memmove_lastfew
	tst	r0, #3
.L20:	ldrneb	r3, [r1, #-1]!
	subne	r2, r2, #1
	strneb	r3, [r0, #-1]!
	tst	r0, #3
	bne	.L20
.L34:	subs	r2, r2, #8
	bcc	.L50
	ldr	r3, [r1, #-4]!
	ldr	ip, [r1, #-4]!
	str	r3, [r0, #-4]!
	str	ip, [r0, #-4]!
	b	.L34
.L50:	adds	r2, r2, #4
	ldrpl	r3, [r1, #-4]!
	strpl	r3, [r0, #-4]!
	b	__memmove_lastfew

__memmove_lastfew:
	lsls	r2, r2, #31
	ldrcsb	r3, [r1, #-1]!
	ldrcsb	ip, [r1, #-1]!
	ldrmib	r2, [r1, #-1]
	strcsb	r3, [r0, #-1]!
	strcsb	ip, [r0, #-1]!
	strmib	r2, [r0, #-1]
	bx	lr
