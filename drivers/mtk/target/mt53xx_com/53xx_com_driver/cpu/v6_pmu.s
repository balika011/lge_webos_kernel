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
; ------------------------------------------------------------
; PMU access functions for ARM11 (v6)
;
; This example code is provided "as is", with out warranty
; or support entitlement.  No liability is accepted.
; ------------------------------------------------------------

  PRESERVE8

  AREA  v6_pmu, CODE,READONLY
  
  ARM

; ------------------------------------------------------------
; Performance Monitor Block
; ------------------------------------------------------------

  EXPORT getPMN
  ; unsigned int getPMN(void)
getPMN PROC
  MOV     r0, #2                ;  They all implement 2 currently
  BX      lr
  ENDP
  

  EXPORT  pmn_config
  ; void pmn_config(unsigned counter, unsigned int event)
pmn_config PROC
  CMP     r0, #0
  MRC     p15, 0, r2, c15, c12, 0 ; Read Performance Monitor Control Register

  ; Clear current event selection
  MOVEQ   r0, #0x0FF00000
  MOVNE   r0, #0x000FF000
  BIC     r2, r2, r0

  ; Shift event selection to correct position
  AND     r1, r1, #0xFF           ; Evt field is 8 bits, clear off the rest
  MOVEQ   r1, r1, LSL #20         ; If PMN0, shift to fill bits 27:20
  MOVNE   r1, r1, LSL #12         ; If PMN1, shift to fill bits 19:12

  ; Conbine current PMCR with event
  ORR     r2, r2, r1
  MCR     p15, 0, r2, c15, c12, 0 ; Write Performance Monitor Control Register

  BX      lr
  ENDP
  
  
  EXPORT ccnt_divider
  ; Enables/disables the divider (1/64) on CCNT
  ; void ccnt_divider(int divider)
  ; divider = r0 = If 0 disable divider, else enable dvider
ccnt_divider PROC
  MRC     p15, 0, r1, c15, c12, 0 ; Read Performance Monitor Control Register

  CMP     r0, #0x0                ; IF (r0 == 0)
  BICEQ   r1, r1, #0x08           ; THEN: Clear the D bit (disables the divisor)
  ORRNE   r1, r1, #0x08           ; ELSE: Set the D bit (enables the divisor)

  MCR     p15, 0, r1, c15, c12, 0 ; Write PMNC
  BX      lr
  ENDP


  ; ---------------------------------------------------------------
  ; Enable/Disable
  ; ---------------------------------------------------------------

  EXPORT enable_pmu
  ; void enable_pmu(void)
enable_pmu  PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  ORR     r0, r0, #0x01           ; Set E bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP


  EXPORT disable_pmu
  ; void disable_pmu(void)
disable_pmu  PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  BIC     r0, r0, #0x01           ; Clear E bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP

  ; ---------------------------------------------------------------
  ; Counter read registers
  ; ---------------------------------------------------------------

  EXPORT read_ccnt
  ; unsigned int read_ccnt(void)
read_ccnt   PROC
  MRC     p15, 0, r0, c15, c12, 1 ; Read Cycle Counter Register
  BX      lr
  ENDP


  EXPORT  read_pmn
  ; unsigned int read_pmn(unsigned int counter)
read_pmn PROC
  CMP     r0, #0
  MRCEQ   p15, 0, r0, c15, c12, 2 ; Read PMN0 Count Register
  MRCNE   p15, 0, r0, c15, c12, 3 ; Read PMN1 Count Register
  BX      lr
  ENDP
  
  ; ---------------------------------------------------------------
  ; Overflow & Interrupt Generation
  ; ---------------------------------------------------------------

  EXPORT  read_flags
  ; unsigned int read_flags(void)
read_flags PROC
  ;
  ;
  ;
  ;
  ;
  BX      lr
  ENDP


  EXPORT  write_flags
  ; void write_flags(unsigned int flags)
write_flags PROC
	;
	;
	;
	;
	;
  BX      lr
  ENDP
  

  EXPORT  enable_ccnt_irq
  ; void enable_ccnt_irq(void)
enable_ccnt_irq PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  ORR     r0, r0, #0x40           ; Set ECC bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP
  

  EXPORT  disable_ccnt_irq
  ; void enable_ccnt_irq(void)
disable_ccnt_irq PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  BIC     r0, r0, #0x40           ; Set ECC bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP
  

  EXPORT  enable_pmn_irq
  ; void enable_pmn_irq(unsigned int counter)
enable_pmn_irq PROC
  CMP     r0, #0                  ; Check which counter we're programming
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  ORREQ   r0, r0, #0x10           ; Set EC0 bit
  ORRNE   r0, r0, #0x20           ; Set EC1 bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP


  EXPORT  disable_pmn_irq
  ; void disable_pmn_irq(unsigned int counter)
disable_pmn_irq PROC
  CMP     r0, #0                  ; Check which counter we're programming
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  BICEQ   r0, r0, #0x10           ; Clear EC0 bit
  BICNE   r0, r0, #0x20           ; Clear EC1 bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP

  ; ---------------------------------------------------------------
  ; Reset Functions
  ; ---------------------------------------------------------------

  EXPORT  reset_pmn
  ; void reset_pmn(void)
reset_pmn PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  ORR     r0, r0, #0x02           ; Set P bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP


  EXPORT  reset_ccnt
  ; void reset_ccnt(void)
reset_ccnt PROC
  MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
  ORR     r0, r0, #0x04           ; Set C bit
  MCR     p15, 0, r0, c15, c12, 0 ; Write Performance Monitor Control Register
  BX      lr
  ENDP

  END

; ------------------------------------------------------------
; End of v6_pmu.s
; ------------------------------------------------------------
