
#include "../open/inc/tz_macro.h"

    .arch_extension sec
    .section	.tzmon.text

#if __LINUX_ARM_ARCH__ == 6
#define dsb mcr p15, 0, r0, c7, c10, 4
#define dmb mcr p15, 0, r0, c7, c10, 5
#endif

/* get_cpu_tbl */
.macro get_cpu_tbl a
#if __LINUX_ARM_ARCH__ > 6
    mrc     p15, 0, \a, c0, c0, 5               @ armv6 mov sp,#0x0
#else
    mov     \a, #0x0
#endif
    and     \a, \a, #0x0F
    lsl     \a, #7                              @ 128*CPUID
    add     \a, \a, #(ns_reg_svc - .)
    add     \a, \a, pc
    sub     \a, \a, #0xc                        @ sp => ns_reg_svc
.endm


/* start_helper */
    .global	start_helper
start_helper:
    adr     sp, thread_map_lock
    mov     r4, #0x0
    str     r4, [sp]                            @ init thread_map_lock

    @ Save tz tag("TZ30") and swd entry in driver_map
    adr     sp, driver_map
    ldr     r4, =0x545A3330
    str     r4, [sp]
    ldr     r4, =SvcMgr_Main
    str     r4, [sp, #4]
    
    get_cpu_tbl sp
    stmia   sp, {r0 - r2}                       @ Save nwd entry parameters
    
    @ Set pc/cpsr for rfeia of N2S
    adr     r4, tz_svc
    str     r4, [sp, #72]
    ldr     r4, =0x1D3
    str     r4, [sp, #76]

    @ Set the checkpoint of ns_reg_svc
    ldr     r4, =TZ_CKPT1
    str     r4, [sp, #80]                       @ make a mark in 80~84 (0x60~0x54)
    str     r4, [sp, #84]
    ldr     r4, =TZ_CKPT2
    str     r4, [sp, #120]                      @ make a mark in 120~127 (0x78~0x7c)
    str     r4, [sp, #124]

    add     sp, sp, #60
    mrs     r4, cpsr
    stmia   sp, {r3 - r4}                       @ Save nwd entry and cpsr
    dsb
    smc     0


    .global not_used
not_used:
    /*cmp	    r0, #0
    moveq   r0, r1
    beq	    TzSecureMemoryAllocate
    
    cmp	    r0, #1
    moveq   r0, r1
    beq	    TzSecureMemoryRelease
    
    cmp     r0, #2
    moveq   r0, r1
    beq     TzYield
    
    cmp     r0, #3
    moveq   r0, r1
    beq     TzInstall_Lock*/
    
    cmp     r0, #4
    beq     TzGetNsRegSvc

    bx      lr


    .global	irq
irq:
    sub     lr, lr, #4
    srsdb   #MODE_SVC!
    cps     #MODE_SVC
    stmdb   sp!, {r0 - r12, lr}

    get_cpu_tbl r0
    add     r0, r0, #60                         @ r0 => ns_reg_svc:pc
    ldmia   r0, {r1 - r3}
    
    @ Save swd sp in the free slot of thread_map
    str	    sp, [r3, #4]

    cps	    #MODE_IRQ
    mov	    lr, r1
    msr	    spsr_cxsf, r2
    ldr	    r1, =0xFFFF0018
    mrs	    r2, cpsr
    cps	    #MODE_SVC
    
    stmia   r0, {r1 - r2}                       @ r0 => ns_reg_svc:pc
    dsb
    smc	    0


/* software_interrupt */
    .global software_interrupt
software_interrupt:
    mrc	    p15, 0, sp, c1, c1, 0 
    tst	    sp, #1
    bne	    N2S

S2N:
    orr	    sp, sp, #1

    get_cpu_tbl r0
    add     r0, r0, #52                         @ r0 => ns_reg_svc:sp
    cps	    #MODE_SVC
    ldmia   r0, {sp, lr}
    cps	    #MODE_MON
    sub     r0, r0, #52                         @ r0 => ns_reg_svc

    mov     lr, r0                              @ lr => ns_reg_svc
    ldmia   lr, {r0 - r12}
    add     lr, lr, #60                         @ lr => ns_reg_svc:pc
    
    mcr	    p15, 0, sp, c1, c1, 0
    rfeia   lr

N2S:
    bic	    sp, sp, #1
    mcr	    p15, 0, sp, c1, c1, 0

    get_cpu_tbl sp
    add     sp, sp, #60
    srsia	#MODE_MON
    sub     sp, sp, #60

    @ Save workable registers of nwd.
    stmia   sp, {r0 - r12}

    mov     r9, sp                              @ r9=> ns_reg_svc
    add     r9, r9, #52                         @ r9=> ns_reg_svc:sp
    cps	    #MODE_SVC
    stmia   r9, {sp, lr}
    cps	    #MODE_MON
    sub     r9, r9, #52                         @ r9=> ns_reg_svc
    
    add     sp, sp, #72
    rfeia   sp


/* tz_svc */
tz_svc:
    @ check mmu enable or not
    @mrc     p15, 0, r0, c1, c0, 0
    @tst     r0, #1
    @beq     0f

    @ Check Lock free
    adr     r2, thread_map_lock                 @ Lock free : 0x00  Lock taken : 0xFF
    mov     r1, #0xFF                           @ load the 'lock taken' value
try_lock_0:
    ldrex   r0, [r2]                            @ load the lock value
    cmp     r0, #0                              @ is the lock free?
    strexeq r0, r1, [r2]                        @ try and claim the lock
    cmpeq   r0, #0                              @ did this succeed?
    bne     try_lock_0                          @ no -> try again . . . .                           
    dmb

0:
    @ Find a slot (r11) in thread_map to save nwd sp (r0)
    ldr     r0, [r9, #52]
    adr	    r2, thread_map
    add	    r3, r2, #8*N_THREADS
    mov	    r11, #0

1:
    ldr	    r1, [r2], #8
    cmp	    r0, r1
    bne	    2f
    sub	    r11, r2, #8
    
    @ Restore swd sp from irq
    ldr	    sp, [r2, #-4]
    
    @ Restore thread_map entry from irq
    str	    r11, [r9, #68]

    @ check mmu
    @mrc     p15, 0, r6, c1, c0, 0
    @tst     r6, #1
    @beq     unlock_end

    @ unlock the thread_map_lock
    mov     r6, #0x0
    adr     r8,	thread_map_lock
    str     r6, [r8]
    dsb
    sev

unlock_end:
    @ Restore workable registers from irq, and jump to the interrupted point.
    ldmia   sp!, {r0 - r12, lr}
    rfeia   sp!

2:
    cmp	    r11, #0
    cmpeq   r1, #0
    subeq   r11, r2, #8
    cmp	    r2, r3
    bne	    1b

    cmp	    r11, #0
    beq	    4f
    mov	    r5, r0

    @ldr     r0, [r9]
    @cmp     r0, #0
    @ldreq   r12, =TzServiceCall
    @beq     6f

    @ Check TZ tag
    ldr	    r0, [r9]
    adr	    r2, driver_map
    add	    r3, r2, #8*N_DRIVERS
3:
    ldr	    r1, [r2], #8
    cmp	    r0, r1
    beq	    5f
    cmp	    r2, r3
    bne	    3b

4:
    mvn	    r0, #0
    str	    r0, [r9]

    @ check mmu
    @mrc     p15, 0, r6, c1, c0, 0
    @tst     r6, #1
    @beq     41f

    @ unlock the thread_map_lock
    mov     r6, #0x0
    adr     r8,	thread_map_lock
    str     r6, [r8]
    dsb
    sev

41:
    smc	    0

5:
    @ Save swd entry in driver_map to r12
    ldr	    r12, [r2, #-4]

6:
    @mov     sp, r9
    @add     sp, sp, #120
    @mov     r0, #STACK_SIZE
    @bl      TzSecureMemoryAllocate
    @cmp     r0, #0
    @beq     4b

    @mov     sl, r0
    @add     sp, r0, #STACK_SIZE
    
    @ Get the swd sp for current thread
    ldr     sp, =__stack_start
    ldr	    r8, =(. + 12)
    sub	    r8, pc, r8
    add     sp, sp, r8
    adr	    r8, thread_map
    sub     r8, r11, r8
    lsr     r8, #3
    lsl     r8, #STACK_SIZE_SHIFT
    sub     sp, sp, r8

    @ Save nwd sp in the free slot (r11) of thread_map, 
    @ currently, cpu is in critical section.
    str	    r5, [r11]

    @ check mmu
    @mrc     p15, 0, r6, c1, c0, 0               @ check mmu enable or not
    @tst     r6, #1
    @beq     7f

    @ unlock the thread_map_lock
    mov     r6, #0x0
    adr     r8,	thread_map_lock
    str     r6, [r8]
    dsb
    sev

7:
    @ Save allocated slot of thread_map to ns_reg_svc:[68]
    str     r11, [r9, #68]

    @ Restore workable registers of nwd.
    ldmia   r9, {r0 - r8}

    @ Push registers onto the stack to create an TZ_SMC_ARG_T structure.
    push    {r1-r3}
    mov     r0, sp

    @ Restore svc cpsr (enable irq)
    ldr     r8, [r9, #64]
    msr     cpsr_cxsf, r8
    
    @ Branch to swd entry
    @mov    pc, r12
    bl      SvcMgr_Main
    
    @ Disable interrupts in monitor code
    cpsid   if

    @ Restore the sp to reset value.
    add     sp, sp, #12

    @ Return from swd
    b       tz_ret


/* tz_ret */
tz_ret:
    @ Save return value(r0-r3) of swd
    get_cpu_tbl r9
    mov     sp, r9
    stmia   sp, {r0 - r3}

    @ check mmu & try_lock
    @mrc     p15, 0, r0, c1, c0, 0
    @tst     r0, #1
    @moveq   r0, #0                              @ clear this thread in threap map if mmu disable
    @streq   r0, [r11]
    @beq     3f                                  @ no need clean if mmu is disable

    @ Check Lock free
    adr     r2, thread_map_lock                 @ Lock free : 0x00  Lock taken : 0xFF
    mov     r1, #0xFF                           @ load the 'lock taken' value
try_lock_1:
    ldrex   r0, [r2]                            @ load the lock value
    cmp     r0, #0                              @ is the lock free?
    strexeq r0, r1, [r2]                        @ try and claim the lock
    cmpeq   r0, #0                              @ did this succeed?
    bne     try_lock_1                          @ no -> try again . . . .                           
    dmb

    @ Clear this thread in threap_map
    mov     r0, #0
    str     r0, [r11]
    str     r0, [r11, #4]

    @ Unlock the thread_map_lock
    mov     r0, #0x0
    adr     r2,	thread_map_lock
    str     r0, [r2]
    dsb
    sev

3:
    @mov     sp, r9
    @add     sp, sp, #120
    @mov     r0, sl
    @bl     TzSecureMemoryRelease
    smc     0


TzYield:
    @ don't support in TZ 3.0
    b       .

TzGetNsRegSvc:
    push {lr}
    get_cpu_tbl r0
    pop  {pc}


    .global thread_map_lock
thread_map_lock:
    .space  4

    .global	driver_map
driver_map:
    .space	8*N_DRIVERS

    .global	thread_map
thread_map:
    .space	8*N_THREADS

    .global	ns_reg_svc
ns_reg_svc:
    .space	128*CPU_NUMS
