
	.text
	.global	__aeabi_idiv
	.global	__aeabi_idivmod

__aeabi_idiv:
__aeabi_idivmod:
	orrs	r3, r0, r1
	bmi	.L140
	movs	r2, #0
	rsbs	r3, r1, r0, lsr #1
	bcc	.L124
	rsbs	r3, r1, r0, lsr #4
	bcc	__arm_div4
	rsbs	r3, r1, r0, lsr #8
	bcc	__arm_div8
	rsbs	r3, r1, r0, lsr #12
	bcc	__arm_div12
	rsbs	r3, r1, r0, lsr #16
	bcc	__arm_div16

__arm_div_large_positive:
	lsl	r1, r1, #8
	rsbs	r3, r1, r0, lsr #16
	orr	r2, r2, #0xff000000
	orrcs	r2, r2, #0xff0000
	lslcs	r1, r1, #8
	rsbs	r3, r1, r0, lsr #12
	bcc	__arm_div12
	rsbs	r3, r1, #0
	bcs	.L21c
.L58:	lsrcs	r1, r1, #8

__arm_div16:
	rsbs	r3, r1, r0, lsr #15
	subcs	r0, r0, r1, lsl #15
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #14
	subcs	r0, r0, r1, lsl #14
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #13
	subcs	r0, r0, r1, lsl #13
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #12
	subcs	r0, r0, r1, lsl #12
	adcs	r2, r2, r2

__arm_div12:
	rsbs	r3, r1, r0, lsr #11
	subcs	r0, r0, r1, lsl #11
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #10
	subcs	r0, r0, r1, lsl #10
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #9
	subcs	r0, r0, r1, lsl #9
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #8
	subcs	r0, r0, r1, lsl #8
	adcs	r2, r2, r2
	bcs	.L58

__arm_div8:
	rsbs	r3, r1, r0, lsr #7
	subcs	r0, r0, r1, lsl #7
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #6
	subcs	r0, r0, r1, lsl #6
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #5
	subcs	r0, r0, r1, lsl #5
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #4
	subcs	r0, r0, r1, lsl #4
	adcs	r2, r2, r2

__arm_div4:
	rsbs	r3, r1, r0, lsr #3
	subcs	r0, r0, r1, lsl #3
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #2
	subcs	r0, r0, r1, lsl #2
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #1
	subcs	r0, r0, r1, lsl #1
	adcs	r2, r2, r2
	subs	r1, r0, r1
	movcc	r1, r0
	adc	r0, r2, r2
	bx	lr
.L124:	subs	r1, r0, r1
	bcc	.L134
	movs	r0, #1
	bx	lr
.L134:	mov	r1, r0
	movs	r0, #0
	bx	lr
.L140:	ands	r2, r1, #0x80000000
	rsbmi	r1, r1, #0
	eors	ip, r2, r0, asr #32
	rsbcs	r0, r0, #0
	rsbs	r3, r1, r0, lsr #4
	bcc	.L1d0
	rsbs	r3, r1, r0, lsr #8
	bcc	.L1a0
	lsl	r1, r1, #6
	rsbs	r3, r1, r0, lsr #8
	orr	r2, r2, #0xfc000000
	bcc	.L1a0
	lsl	r1, r1, #6
	rsbs	r3, r1, r0, lsr #8
	orr	r2, r2, #0x3f00000
	bcc	.L1a0
	lsl	r1, r1, #6
	rsbs	r3, r1, r0, lsr #8
	orr	r2, r2, #0xfc000
	lslcs	r1, r1, #6
	orrcs	r2, r2, #0x3f00
	rsbs	r3, r1, #0
	bcs	.L214
.L19c:	lsrcs	r1, r1, #6
.L1a0:	rsbs	r3, r1, r0, lsr #7
	subcs	r0, r0, r1, lsl #7
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #6
	subcs	r0, r0, r1, lsl #6
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #5
	subcs	r0, r0, r1, lsl #5
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #4
	subcs	r0, r0, r1, lsl #4
	adcs	r2, r2, r2
.L1d0:	rsbs	r3, r1, r0, lsr #3
	subcs	r0, r0, r1, lsl #3
	adcs	r2, r2, r2
	rsbs	r3, r1, r0, lsr #2
	subcs	r0, r0, r1, lsl #2
	adcs	r2, r2, r2
	bcs	.L19c
	rsbs	r3, r1, r0, lsr #1
	subcs	r0, r0, r1, lsl #1
	adcs	r2, r2, r2
	subs	r1, r0, r1
	movcc	r1, r0
	adc	r0, r2, r2
	asrs	ip, ip, #31
	rsbmi	r0, r0, #0
	rsbcs	r1, r1, #0
	bx	lr
.L214:	asrs	ip, ip, #31
	rsbmi	r0, r0, #0
.L21c:	push	{r0, lr}
	movs	r0, #0
	bl	__aeabi_idiv0
	pop	{r1, pc}

	.text
	.global	__aeabi_uidiv
	.global	__aeabi_uidivmod

__aeabi_uidiv:
__aeabi_uidivmod:
	movs	r2, #0
	rsbs	r3, r1, r0, lsr #4
	bcc	__arm_div4
	rsbs	r3, r1, r0, lsr #8
	bcc	__arm_div8
	rsbs	r3, r1, r0, lsr #12
	bcc	__arm_div12
	rsbs	r3, r1, r0, lsr #16
	bcc	__arm_div16
	b	__arm_div_large_positive

	.end
