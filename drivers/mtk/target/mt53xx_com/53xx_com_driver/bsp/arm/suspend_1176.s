;*----------------------------------------------------------------------------*
;* Copyright Statement:                                                       *
;*                                                                            *
;*   This software/firmware and related documentation ("MediaTek Software")   *
;* are protected under international and related jurisdictions'copyright laws *
;* as unpublished works. The information contained herein is confidential and *
;* proprietary to MediaTek Inc. Without the prior written permission of       *
;* MediaTek Inc., any reproduction, modification, use or disclosure of        *
;* MediaTek Software, and information contained herein, in whole or in part,  *
;* shall be strictly prohibited.                                              *
;* MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
;*                                                                            *
;*   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
;* AGREES TO THE FOLLOWING:                                                   *
;*                                                                            *
;*   1)Any and all intellectual property rights (including without            *
;* limitation, patent, copyright, and trade secrets) in and to this           *
;* Software/firmware and related documentation ("MediaTek Software") shall    *
;* remain the exclusive property of MediaTek Inc. Any and all intellectual    *
;* property rights (including without limitation, patent, copyright, and      *
;* trade secrets) in and to any modifications and derivatives to MediaTek     *
;* Software, whoever made, shall also remain the exclusive property of        *
;* MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
;* title to any intellectual property right in MediaTek Software to Receiver. *
;*                                                                            *
;*   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
;* representatives is provided to Receiver on an "AS IS" basis only.          *
;* MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
;* including but not limited to any implied warranties of merchantability,    *
;* non-infringement and fitness for a particular purpose and any warranties   *
;* arising out of course of performance, course of dealing or usage of trade. *
;* MediaTek Inc. does not provide any warranty whatsoever with respect to the *
;* software of any third party which may be used by, incorporated in, or      *
;* supplied with the MediaTek Software, and Receiver agrees to look only to   *
;* such third parties for any warranty claim relating thereto.  Receiver      *
;* expressly acknowledges that it is Receiver's sole responsibility to obtain *
;* from any third party all proper licenses contained in or delivered with    *
;* MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
;* releases made to Receiver's specifications or to conform to a particular   *
;* standard or open forum.                                                    *
;*                                                                            *
;*   3)Receiver further acknowledge that Receiver may, either presently       *
;* and/or in the future, instruct MediaTek Inc. to assist it in the           *
;* development and the implementation, in accordance with Receiver's designs, *
;* of certain softwares relating to Receiver's product(s) (the "Services").   *
;* Except as may be otherwise agreed to in writing, no warranties of any      *
;* kind, whether express or implied, are given by MediaTek Inc. with respect  *
;* to the Services provided, and the Services are provided on an "AS IS"      *
;* basis. Receiver further acknowledges that the Services may contain errors  *
;* that testing is important and it is solely responsible for fully testing   *
;* the Services and/or derivatives thereof before they are used, sublicensed  *
;* or distributed. Should there be any third party action brought against     *
;* MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
;* to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
;* mutually agree to enter into or continue a business relationship or other  *
;* arrangement, the terms and conditions set forth herein shall remain        *
;* effective and, unless explicitly stated otherwise, shall prevail in the    *
;* event of a conflict in the terms in any agreements entered into between    *
;* the parties.                                                               *
;*                                                                            *
;*   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
;* cumulative liability with respect to MediaTek Software released hereunder  *
;* will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
;* MediaTek Software at issue.                                                *
;*                                                                            *
;*   5)The transaction contemplated hereunder shall be construed in           *
;* accordance with the laws of Singapore, excluding its conflict of laws      *
;* principles.  Any disputes, controversies or claims arising thereof and     *
;* related thereto shall be settled via arbitration in Singapore, under the   *
;* then current rules of the International Chamber of Commerce (ICC).  The    *
;* arbitration shall be conducted in English. The awards of the arbitration   *
;* shall be final and binding upon both parties and shall be entered and      *
;* enforceable in any court of competent jurisdiction.                        *
;*----------------------------------------------------------------------------*
 
;** @file suspend_1176.s
;*  provides functions for ARM11/armv6 CPU suspend to ram support
;*  Used in Nucleus AP only.
;*

;/-----------------------------------------------------------------------------
;*  Save cpu state (and the return point for resume)
;* 
;*  @param pBuffer: Address to save cpu state. 
;*                  The buffer will be pass to resume function as param.
;*  @return  1 for save stats, 0 for resume from suspend
;/-----------------------------------------------------------------------------
;extern int cpu_SaveSuspendReturn(UINT32 *pBuffer);

    ; Code section declaration
    PRESERVE8
    AREA    INIT, CODE, READONLY
    CODE32

    INCLUDE ../arm_cpu.inc

    EXPORT  cpu_SaveSuspendReturn
cpu_SaveSuspendReturn
    stmfd   sp!, {r4-r11, lr}

    ; Resume start PC.
    ldr     r1, =cpu_Resume
    str     r1, [r0], #4

    ; Save return registers in cpu state buffer
    ; This stackfram will be destroied by following DRAMC
    ; suspend functions. So put it in cpu state buffer.
    stmia   r0!, {r4-r11}

    ; Save current CPU stats
    mrc     p15, 0, r4, c1, c0, 0   ; control register
    mrc     p15, 0, r5, c1, c0, 2   ; co-processor access control
    mrc     p15, 0, r6, c3, c0, 0   ; Domain ID
    mrc     p15, 0, r7, c2, c0, 0   ; Translation table base 0
    mrc     p15, 0, r8, c2, c0, 1   ; Translation table base 1
    mrc     p15, 0, r9, c1, c0, 1   ; auxiliary control register
    mrc     p15, 0, r10, c13, c0, 0 ; FCSE/PID
    mrc     p15, 0, r11, c13, c0, 1 ; Context ID
    stmia   r0!, {r4-r11}

    ; Save current mode....
    mrs     r4, CPSR
    str     r4, [r0], #4

    ; USER registers (using SYS mode)
    cps     #MODE_SYS
    stmia   r0!, {sp, lr}

    ; SVC registers
    cps     #MODE_SVC
    mrs     r5, SPSR
    stmia   r0!, {r5, sp, lr}

    ; ABT registers
    cps     #MODE_ABT
    mrs     r5, SPSR
    stmia   r0!, {r5, sp, lr}

    ; UNDEF registers
    cps     #MODE_UDF
    mrs     r5, SPSR
    stmia   r0!, {r5, sp, lr}

    ; IRQ registers
    cps     #MODE_IRQ
    mrs     r5, SPSR
    stmia   r0!, {r5, sp, lr}

    ; FIQ registers
    cps     #MODE_FIQ
    mrs     r5, SPSR
    stmia   r0!, {r5, r8-r14}

    ; Switch back to original mode.
    msr     CPSR_cxsf, r4

    mov     r0, #1
    ldmfd   sp!, {r4-r11, pc}
    bx      lr


cpu_Resume
    ; We're back, Restore current CPU stats
    add     r3, r0, #4              ; R3 point to resume registers
    add     r0, r0, #(32+4)         ; Skip resume registers
    ldmia   r0!, {r4-r11}

    mov     r1, #0
    mcr     p15, 0, r1, c7, c14, 0  ; clean+invalidate D cache
    mcr     p15, 0, r1, c7, c5, 0   ; invalidate I cache
    mcr     p15, 0, r1, c7, c15, 0  ; clean+invalidate cache
    mcr     p15, 0, r1, c7, c10, 4  ; drain write buffer
    mcr     p15, 0, r11, c13, c0, 1 ; Context ID
    mcr     p15, 0, r10, c13, c0, 0 ; FCSE/PID
    mcr     p15, 0, r6, c3, c0, 0   ; Domain ID
    mcr     p15, 0, r1, c8, c7, 0   ; invalidate I + D TLBs
    mcr     p15, 0, r7, c2, c0, 0   ; Translation table base 0
    mcr     p15, 0, r8, c2, c0, 1   ; Translation table base 1
    mcr     p15, 0, r9, c1, c0, 1   ; auxiliary control register
    mcr     p15, 0, r5, c1, c0, 2   ; co-processor access control
    mcr     p15, 0, r1, c2, c0, 2   ; TTB control register

    ; Now, enable Cache & MMU
    ldr     r2, =cpu_ResumeMMUOn
    mcr     p15, 0, r1, c7, c5, 4   ; ISB
    mcr     p15, 0, r4, c1, c0, 0   ; turn on MMU, I-cache, D-cache, etc
    mcr     p15, 0, r1, c7, c5, 4   ; ISB
    nop
    nop
    nop
    nop
    bx      r2

cpu_ResumeMMUOn
    ; Load current mode....
    ldr     r4, [r0], #4

    ; USER registers (using SYS mode)
    cps     #MODE_SYS
    ldmia   r0!, {sp, lr}

    ; SVC registers
    cps     #MODE_SVC
    ldmia   r0!, {r5, sp, lr}
    msr     SPSR_cxsf, r5

    ; ABT registers
    cps     #MODE_ABT
    ldmia   r0!, {r5, sp, lr}
    msr     SPSR_cxsf, r5

    ; UNDEF registers
    cps     #MODE_UDF
    ldmia   r0!, {r5, sp, lr}
    msr     SPSR_cxsf, r5

    ; IRQ registers
    cps     #MODE_IRQ
    ldmia   r0!, {r5, sp, lr}
    msr     SPSR_cxsf, r5

    ; FIQ registers
    cps     #MODE_FIQ
    ldmia   r0!, {r5, r8-r14}
    msr     SPSR_cxsf, r5

    ; Switch back to original mode.
    msr     CPSR_cxsf, r4

    ; We're done, returning
    ; Because stackframe is corrupted, just skip them.
    mov     r0, #0
    ldmia   r3, {r4-r11}
    add     sp, sp, #36
    bx      lr

    END
