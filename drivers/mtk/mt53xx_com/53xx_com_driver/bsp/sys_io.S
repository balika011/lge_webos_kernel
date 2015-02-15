        .text

        .global _sys_open
_sys_open:
        push	{r1, r2, r3, lr}
        stm	sp, {r0, r1}
        bl	strlen
        add	r1, sp, #0
        str	r0, [sp, #8]
        mov	r0, #1
        svc	0x00123456
        pop	{r1, r2, r3, pc}

        .global _sys_close
_sys_close:
        push	{r3, lr}
        add	r1, sp, #0
        str	r0, [sp]
        mov	r0, #2
        svc	0x00123456
        pop	{r3, pc}

        .global _sys_read
_sys_read:
        push	{r1, r2, r3, r4, lr}
        push	{r0}
        stm	sp, {r0, r1, r2, r3}
        mov	r0, #6
        add	r1, sp, #0
        svc	0x00123456
        add	sp, sp, #16
        pop	{r4, pc}

        .global _sys_write
_sys_write:
        push	{r1, r2, r3, r4, lr}
        push	{r0}
        stm	sp, {r0, r1, r2}
        mov	r0, #5
        add	r1, sp, #0
        svc	0x00123456
        add	sp, sp, #16
        pop	{r4, pc}

        .global _sys_seek
_sys_seek:
        push	{r2, r3, r4, lr}
        stm	sp, {r0, r1}
        mov	r0, #10
        add	r1, sp, #0
        svc	0x00123456
        pop	{r2, r3, r4, pc}

        .global _sys_flen
_sys_flen:
        push    {r3, lr}
        add     r1, sp, #0
        str     r0, [sp]
        mov     r0, #12
        svc     0x00123456
        pop     {r3, pc}
        .end
