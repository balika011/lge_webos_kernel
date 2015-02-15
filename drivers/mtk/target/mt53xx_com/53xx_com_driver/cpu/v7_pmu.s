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
; v7-A/R PMU Code
;
; This example code is provided "as is", with out warranty
; or support entitlement.  No liability is accepted.
; ------------------------------------------------------------

  PRESERVE8

  AREA  v7PMU,CODE,READONLY
  
  ARM

; ------------------------------------------------------------
; Performance Monitor Block
; ------------------------------------------------------------

  EXPORT getPMN
  ; Returns the number of progammable counters
  ; uint32_t getPMN(void)
getPMN PROC
  MRC     p15, 0, r0, c9, c12, 0 ; Read PMNC Register
  MOV     r0, r0, LSR #11        ; Shift N field down to bit 0
  AND     r0, r0, #0x1F          ; Mask to leave just the 5 N bits
  BX      lr
  ENDP
  

  EXPORT  pmn_config
  ; Sets the event for a programmable counter to record
  ; void pmn_config(unsigned counter, uint32_t event)
  ; counter = r0 = Which counter to program  (e.g. 0 for PMN0, 1 for PMN1)
  ; event   = r1 = The event code
pmn_config PROC
  AND     r0, r0, #0x1F          ; Mask to leave only bits 4:0
  MCR     p15, 0, r0, c9, c12, 5 ; Write PMNXSEL Register
  MCR     p15, 0, r1, c9, c13, 1 ; Write EVTSELx Register
  BX      lr
  ENDP
  

  EXPORT ccnt_divider
  ; Enables/disables the divider (1/64) on CCNT
  ; void ccnt_divider(int divider)
  ; divider = r0 = If 0 disable divider, else enable dvider
ccnt_divider PROC
  MRC     p15, 0, r1, c9, c12, 0  ; Read PMNC

  CMP     r0, #0x0                ; IF (r0 == 0)
  BICEQ   r1, r1, #0x08           ; THEN: Clear the D bit (disables the divisor)
  ORRNE   r1, r1, #0x08           ; ELSE: Set the D bit (enables the divisor)

  MCR     p15, 0, r1, c9, c12, 0  ; Write PMNC
  BX      lr
  ENDP

  ; ---------------------------------------------------------------
  ; Enable/Disable
  ; ---------------------------------------------------------------

  EXPORT enable_pmu
  ; Global PMU enable
  ; void enable_pmu(void)
enable_pmu  PROC
  MRC     p15, 0, r0, c9, c12, 0  ; Read PMNC
  ORR     r0, r0, #0x01           ; Set E bit
  MCR     p15, 0, r0, c9, c12, 0  ; Write PMNC
  BX      lr
  ENDP


  EXPORT disable_pmu
  ; Global PMU disable
  ; void disable_pmu(void)
disable_pmu  PROC
  MRC     p15, 0, r0, c9, c12, 0  ; Read PMNC
  BIC     r0, r0, #0x01           ; Clear E bit
  MCR     p15, 0, r0, c9, c12, 0  ; Write PMNC
  BX      lr
  ENDP
  

  EXPORT enable_ccnt
  ; Enable the CCNT
  ; void enable_ccnt(void)
enable_ccnt PROC
  MOV     r0, #0x80000000         ; Set C bit
  MCR     p15, 0, r0, c9, c12, 1  ; Write CNTENS Register
  BX      lr
  ENDP
  

  EXPORT disable_ccnt
  ; Disable the CCNT
  ; void disable_ccnt(void)
disable_ccnt PROC
  MOV     r0, #0x80000000         ; Clear C bit
  MCR     p15, 0, r0, c9, c12, 2  ; Write CNTENC Register
  BX      lr
  ENDP
  

  EXPORT enable_pmn
  ; Enable PMN{n}
  ; void enable_pmn(uint32_t counter)
  ; counter = r0 = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
enable_pmn PROC
  MOV     r1, #0x1                ; Use arg (r0) to set which counter to disable
  MOV     r1, r1, LSL r0

  MCR     p15, 0, r1, c9, c12, 1  ; Write CNTENS Register
  BX      lr
  ENDP
  

  EXPORT disable_pmn
  ; Enable PMN{n}
  ; void disable_pmn(uint32_t counter)
  ; counter = r0 = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
disable_pmn PROC
  MOV     r1, #0x1                ; Use arg (r0) to set which counter to disable
  MOV     r1, r1, LSL r0

  MCR     p15, 0, r1, c9, c12, 1  ; Write CNTENS Register
  BX      lr
  ENDP
  
  
  EXPORT  enable_pmu_user_access
  ; Enables User mode access to the PMU (must be called in a priviledged mode)
  ; void enable_pmu_user_access(void)
enable_pmu_user_access PROC
  MRC     p15, 0, r0, c9, c14, 0  ; Read PMUSERENR Register
  ORR     r0, r0, #0x01           ; Set EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  ; Write PMUSERENR Register
  BX      lr
  ENDP
  
  
  EXPORT  disable_pmu_user_access
  ; Disables User mode access to the PMU (must be called in a priviledged mode)
  ; void disable_pmu_user_access(void)
disable_pmu_user_access PROC
  MRC     p15, 0, r0, c9, c14, 0  ; Read PMUSERENR Register
  BIC     r0, r0, #0x01           ; Clear EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  ; Write PMUSERENR Register
  BX      lr
  ENDP


  ; ---------------------------------------------------------------
  ; Counter read registers
  ; ---------------------------------------------------------------

  EXPORT read_ccnt
  ; Returns the value of CCNT
  ; uint32_t read_ccnt(void)
read_ccnt   PROC
  MRC     p15, 0, r0, c9, c13, 0 ; Read CCNT Register
  BX      lr
  ENDP


  EXPORT  read_pmn
  ; Returns the value of PMN{n}
  ; uint32_t read_pmn(uint32_t counter)
  ; counter = r0 =  The counter to read (e.g. 0 for PMN0, 1 for PMN1)
read_pmn PROC
  AND     r0, r0, #0x1F          ; Mask to leave only bits 4:0
  MCR     p15, 0, r0, c9, c12, 5 ; Write PMNXSEL Register
  MRC     p15, 0, r0, c9, c13, 2 ; Read current PMNx Register
  BX      lr
  ENDP
  
  
  ; ---------------------------------------------------------------
  ; Software Increment
  ; ---------------------------------------------------------------

	EXPORT pmu_software_increment
	; Writes to software increment register
	; void pmu_software_increment(uint32_t counter)
	; counter = r0 =  The counter to increment (e.g. 0 for PMN0, 1 for PMN1)
pmu_software_increment PROC
  MOV     r1, #0x01
  MOV			r1, r1, LSL r0
  MCR     p15, 0, r1, c9, c12, 4 ; Write SWINCR Register
  BX      lr
  ENDP

  ; ---------------------------------------------------------------
  ; Overflow & Interrupt Generation
  ; ---------------------------------------------------------------

  EXPORT read_flags
  ; Returns the value of the overflow flags
  ; uint32_t read_flags(void)
read_flags PROC
  MRC     p15, 0, r0, c9, c12, 3 ; Read FLAG Register
  BX      lr
  ENDP
  

  EXPORT  write_flags
  ; Writes the overflow flags
  ; void write_flags(uint32_t flags)
write_flags PROC
  MCR     p15, 0, r0, c9, c12, 3 ; Write FLAG Register
  BX      lr
  ENDP
  

  EXPORT  enable_ccnt_irq
  ; Enables interrupt generation on overflow of the CCNT
  ; void enable_ccnt_irq(void)
enable_ccnt_irq PROC
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 1  ; Write INTENS Register
  BX      lr
  ENDP


  EXPORT  disable_ccnt_irq
  ; Disables interrupt generation on overflow of the CCNT
  ; void disable_ccnt_irq(void)
disable_ccnt_irq PROC
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 2   ; Write INTENC Register
  BX      lr
  ENDP
  

  EXPORT enable_pmn_irq
  ; Enables interrupt generation on overflow of PMN{x}
  ; void enable_pmn_irq(uint32_t counter)
  ; counter = r0 =  The counter to enable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
enable_pmn_irq PROC
  MOV     r1, #0x1                 ; Use arg (r0) to set which counter to disable
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 1   ; Write INTENS Register
  BX      lr
  ENDP


  EXPORT disable_pmn_irq
  ; Disables interrupt generation on overflow of PMN{x}
  ; void disable_pmn_irq(uint32_t counter)
  ; counter = r0 =  The counter to disable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
disable_pmn_irq PROC
  MOV     r1, #0x1                ; Use arg (r0) to set which counter to disable
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 2  ; Write INTENC Register
  BX      lr
  ENDP

  ; ---------------------------------------------------------------
  ; Reset Functions
  ; ---------------------------------------------------------------

  EXPORT reset_pmn
  ; Resets the programmable counters
  ; void reset_pmn(void)
reset_pmn PROC
  MRC     p15, 0, r0, c9, c12, 0  ; Read PMNC
  ORR     r0, r0, #0x02           ; Set P bit (Event Counter Reset)
  MCR     p15, 0, r0, c9, c12, 0  ; Write PMNC
  BX      lr
  ENDP


  EXPORT  reset_ccnt
  ; Resets the CCNT
  ; void reset_ccnt(void)
reset_ccnt PROC
  MRC     p15, 0, r0, c9, c12, 0  ; Read PMNC
  ORR     r0, r0, #0x04           ; Set C bit (Event Counter Reset)
  MCR     p15, 0, r0, c9, c12, 0  ; Write PMNC
  BX      lr
  ENDP

  END

; ------------------------------------------------------------
; End of v7_pmu.s
; ------------------------------------------------------------
