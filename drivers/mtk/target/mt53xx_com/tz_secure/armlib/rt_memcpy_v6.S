
	.text
	.global	__aeabi_memcpy
	.global	__rt_memcpy
	.local _memcpy_lastbytes
    .local _memcpy_aligned

__aeabi_memcpy:
__rt_memcpy:
    push {r4}
    mov r4, r0
	cmp	r2, #3
	bls	_memcpy_lastbytes
	ands	ip, r0, #3
	beq	_memcpy_dest_aligned
	ldrb	r3, [r1], #1
	cmp	ip, #2
	add	r2, r2, ip
	ldrlsb	ip, [r1], #1
	strb	r3, [r0], #1
	ldrccb	r3, [r1], #1
	strlsb	ip, [r0], #1
	sub	r2, r2, #4
	strccb	r3, [r0], #1
_memcpy_dest_aligned:
	ands	r3, r1, #3
	beq	_memcpy_aligned
_memcpy_src_nonaligned:
	subs	r2, r2, #8
	bcc	_memcpy_src_nonaligned_end
	ldr	r3, [r1], #4
	ldr	ip, [r1], #4
	str	r3, [r0], #4
	str	ip, [r0], #4
	b	_memcpy_src_nonaligned
_memcpy_src_nonaligned_end:
	adds	r2, r2, #4
	ldrpl	r3, [r1], #4
	strpl	r3, [r0], #4
	b	_memcpy_lastbytes
_memcpy_lastbytes:
	lsls	r2, r2, #31
	ldrcsb	r3, [r1], #1
	ldrcsb	ip, [r1], #1
	ldrmib	r2, [r1], #1
	strcsb	r3, [r0], #1
	strcsb	ip, [r0], #1
	strmib	r2, [r0], #1
    mov r0, r4
    pop {r4}
	bx	lr

_memcpy_aligned:
    push    {r4, lr}
    subs    r2, r2, #32
    bcc     label_1 
label_2:
    ldm     r1!, {r3, r4, ip, lr}
    subs    r2, r2, #32
    stm     r0!, {r3, r4, ip, lr}
    ldm     r1!, {r3, r4, ip, lr}
    stm     r0!, {r3, r4, ip, lr}
    bcs     label_2 
label_1:
    lsls    ip, r2, #28
    ldmcs   r1!, {r3, r4, ip, lr}
    stmcs   r0!, {r3, r4, ip, lr}
    ldmmi   r1!, {r3, r4}
    stmmi   r0!, {r3, r4}
    pop     {r4, lr}
    lsls    ip, r2, #30
    ldrcs   r3, [r1], #4
    strcs   r3, [r0], #4
    b _memcpy_lastbytes
