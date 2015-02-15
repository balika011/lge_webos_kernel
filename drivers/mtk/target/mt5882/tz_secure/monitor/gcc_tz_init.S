
#include "../open/inc/tz_macro.h"

    .section	.tzinit.text


    .global  INT_Reset
INT_Reset:

    @ Setup sp for trustzone init code
    ldr     sp, =__stack_start
    ldr     r10, =(. + 12)
    sub     r10, pc, r10
    add     sp, sp, r10
    
#ifndef CC_MT5881 /* ARM11 single core, CPUID is not working */
	@ Read Multiprocessor Affinity Register(MPIDR)
	mrc 	p15, 0, r10, c0, c0, 5
	and 	r10, r10, #0x0F
	lsl		r10,#12
	sub		sp,sp,r10
#endif
    
    @ Save nwd entry register
    stmdb   sp!, {r0-r3}
    
    @r0 from u-boot is for resume
    @r0 == 0,cold boot
    @r0 != 0,resume
    stmdb   sp!, {r0}
    cmp r0,#0
    bne 2f

#ifndef CC_MT5881 /* ARM11 single core, CPUID is not working */
    @ Read Multiprocessor Affinity Register(MPIDR)
    mrc     p15, 0, r0, c0, c0, 5
    and     r0, r0, #0x0F
    cmp     r0, #0
    bne     2f
#endif
    
    @ Set the checkpoint of stack area, only core 0 needed
    ldr     r6, =__stack_end
    ldr     r10, =(. + 12)
    sub     r10, pc, r10
    add     r6, r6, r10
    mov     r4, #N_THREADS
    ldr     r5, =TZ_CKPT1
1:
    str     r5, [r6]
    add     r6, r6, #STACK_SIZE
    sub     r4, r4, #1
    cmp     r4, #0
    bne     1b
    
    @ Relocate global data address for PIC, only core 0 needed
    bl      Tz_Reloc

2:
    @ Configure Non-Secure Access control register(SCR)
    mrc     p15, 0, r0, c1, c1, 0
    ldr     r1, =0x30
    orr     r0, r0, r1
    mcr     p15, 0, r0, c1, c1, 0
    
    @ SDER Register
    mrc     p15, 0, r0, c1, c1, 1
    ldr     r1, =0x3
    orr     r0, r0, r1
    mcr     p15, 0, r0, c1, c1, 1
    
    /*
    ;[18] NS_SMP Determines if the SMP bit of the Auxiliary Control Register is writable in Non-secure state
    ;[17] TL Determines if lockable TLB entries can be allocated in Non-secure state
    ;[16] PLE Controls NS accesses to the Preload Engine resources
    ;[15] NSASEDIS Disable Non-secure Advanced SIMD Extension functionality:
    ;[14] NSD32DIS Disable the Non-secure use of D16-D31 of the VFP register file:
    ;[11] CP11 Determines permission to access coprocessor 11 in the Non-secure state:
    ;[10] CP10 Determines permission to access coprocessor 10 in the Non-secure state
    */
#ifndef CC_MT5881 /* 5881 is ARM11 without VFP */
    MRC     p15, 0, r0, c1, c0, 2       @ Read CP15 control register
    ORR     r0, r0, #0xf00000           @ Enable CP10 ACCESS
    MCR     p15, 0, r0, c1, c0, 2       @ Write back control register
    MOV     r0, #0
    MCR     p15, 0, r0, c7, c5, 4       @ISB
    MOV     r0,#0x40000000              @ VFP enable bit
    FMXR    FPEXC,r0
    fmrx    r1, c7             @ mrc p10, 7, r1, c7, c0, 0   @ Media and VFP Feature Register 0(MVFR0)
    and     r1, r1, #0xf       @ A_SIMD field                                                        
    cmp     r1, #2             @ 32 x 64bit registers?  
#endif
    
    @ NSACR Register
    mrc     p15, 0, r0, c1, c1, 2
#if defined(CC_MT5890) || defined(CC_MT5882)
    ldreq   r1, =(0x70C00)
    ldrne   r1, =(0x7CC00)
#else
#ifndef CC_MT5881
    ldreq   r1, =(0x60C00)
    ldrne   r1, =(0x6CC00)
#else
    ldr     r1, =(0x70C00) /* [16]CL Set cache lockdown for Non-secure state or entire i/dcache operation will be undef */
#endif
#endif
    orr     r0, r0, r1
    mcr     p15, 0, r0, c1, c1, 2
    
#if __LINUX_ARM_ARCH__ > 6
    @ set FW bit, Cache and TLB maintenance broadcast
    mrc     p15, 0, r0, c1, c0, 1
    ldr     r1, =0x1
    orr     r0, r0, r1
    mcr     p15, 0, r0, c1, c0, 1
#endif
    
    @ Program Secure World vector table.
    cps     #MODE_MON
    ldr     r0, =__tzvect_start
    ldr     r10, =(. + 12)
    sub     r10, pc, r10
    add     r0, r0, r10
    mcr     p15, 0, r0, c12, c0, 0
    mcr     p15, 0, r0, c12, c0, 1
    cps     #MODE_SVC

    @r0 from u-boot is for resume 
    ldmia   sp!, {r0}

    @ Branch to MMU enabling code.
    bl      TZ_Main
	
    @ Restore nwd entry register
    ldmia   sp!, {r0-r3}

    @ Switch to Normal World to allow the NWd OS to boot. This call will not return.
    b       start_helper
