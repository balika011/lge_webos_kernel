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
;
;  - Our FW work in SVC Mode
;

    [ :LNOT::DEF:CC_5391_LOADER
                    GBLL    CC_5391_LOADER
CC_5391_LOADER      SETL    {FALSE}
    ]

    [ :LNOT::DEF:CC_5391_PRELOADER
                    GBLL    CC_5391_PRELOADER
CC_5391_PRELOADER      SETL    {FALSE}
    ]

    [ :LNOT::DEF:CC_5392B_PRELOADER
                    GBLL    CC_5392B_PRELOADER
CC_5392B_PRELOADER      SETL    {FALSE}
    ]

    [ :LNOT::DEF:CC_NAND_BOOT
                    GBLL    CC_NAND_BOOT
CC_NAND_BOOT      SETL    {FALSE}
    ]
    
    [ CC_5391_LOADER

    INCLUDE arm_cpu.inc

;*************************************************************************
;*   Declarations
;*************************************************************************

    ; Declaration of external functions

    ; arm.s: do arm related coprocessor settings
    IMPORT  ARM_EnableICache
    [ :LNOT:CC_5391_PRELOADER
    IMPORT  ARM_EnableDCache
    ]
    IMPORT  CHIP_GetStack
    IMPORT  ARM_StackITCM

    ; chip_boot.c: do setup register only
    IMPORT  CHIP_RS232Init
    ; IMPORT  CHIP_BimInit
    ; IMPORT  CHIP_ClockSetting
    ; IMPORT  CHIP_1stBootRegInit
    IMPORT  CHIP_DisplayChar
    IMPORT  CHIP_DisplayString
    ; IMPORT  CHIP_IsRemap
    ; IMPORT  CLK_SetDdr

    [ :LNOT:CC_5391_LOADER
    IMPORT  CHIP_CheckBoard
    ]

;    IMPORT  g_pvPageTable
;    IMPORT  HalEnableCaches
;    IMPORT  HalEnableICache
;    IMPORT  HalEnableDCache
;    IMPORT  HalInitMMU
    [ :LNOT:CC_5391_PRELOADER
    IMPORT  CIrqHandler
    IMPORT  CFiqHandler
    ]
    IMPORT  BSP_Start
;    IMPORT  BSP_Main
;    IMPORT  CkgenInit
;    IMPORT  BimInit
;    IMPORT  DramInit
;    IMPORT  DramSizing

    ; Declaration of external variables

    [ :LNOT:CC_5391_PRELOADER
    IMPORT  |Image$$BSS$$ZI$$Base|
    ;IMPORT  |Image$$BSS$$ZI$$Limit|
    IMPORT  |Image$$BSS$$ZI$$Length|
    IMPORT  |Image$$RO$$Limit|
    IMPORT  |Image$$RW$$Base|
    IMPORT  |Image$$RW$$Limit|
    ]


;*************************************************************************
;*  Begin of code section                                                *
;*************************************************************************

    ; Code section declaration

    PRESERVE8
	AREA	INIT, CODE, READONLY
    CODE32


;*************************************************************************
;*   Local variables
;*************************************************************************

; Definitions of section address

    [ :LNOT:CC_5391_PRELOADER
BssStart
    DCD     |Image$$BSS$$ZI$$Base|

BssEnd
    ;DCD     |Image$$BSS$$ZI$$Limit|
    DCD     |Image$$BSS$$ZI$$Base| + |Image$$BSS$$ZI$$Length|

RomStart
    DCD     |Image$$RO$$Limit|

RamStart
    DCD     |Image$$RW$$Base|

RamEnd
    DCD     |Image$$RW$$Limit|

    EXPORT  g_u4HeapStart
g_u4HeapStart
	DCD		0
    ]

;    EXPORT  g_u4DramSize
;g_u4DramSize
;    DCD     0

;----------------------------------------------------------------------
; Debug messages while booting

    EXPORT  MsgCMain
MsgCMain
    DCB     "\n\rStart Cmain\n\r", 0

    EXPORT  MsgHalting
MsgHalting
;    DCB     "\n\rSystem halt\n\r", 0
    DCB     "\n\rHlt\n\r", 0

;    EXPORT  MsgNotEqual
;MsgNotEqual
;    DCB     "\n\rFlash is not equal to Dram\n\r", 0

;    EXPORT  MsgDQSFail
;MsgDQSFail
;    DCB     "\n\rDQS Initial failed\n\r", 0

;    DCB     0, 0            ; 0-3 bytes padding to avoid assembler warning

;MsgDummy
;    DCD     0


;*************************************************************************
;*  Exception handlers
;*************************************************************************
    EXPORT  INT_Undef_Inst
    EXPORT  INT_Software
    EXPORT  INT_Prefetch_Abort
    EXPORT  INT_Data_Abort
    EXPORT  INT_Reserved
    EXPORT  INT_IRQ
    EXPORT  INT_FIQ

INT_Undef_Inst
    B       INT_Undef_Inst

INT_Software
    B       INT_Software

INT_Prefetch_Abort
    B       INT_Prefetch_Abort

INT_Data_Abort
    B       INT_Data_Abort

INT_Reserved
    B       INT_Reserved

INT_IRQ
    [ :LNOT:CC_5391_PRELOADER
    B       CIrqHandler
    |
    B       INT_IRQ
    ]

INT_FIQ
    [ :LNOT:CC_5391_PRELOADER
    B       CFiqHandler
    |
    B       INT_FIQ
    ]

;*************************************************************************
;*  Reset handler
;*************************************************************************
    EXPORT  INT_Reset

INT_Reset

    ; Set to supervisor mode and disable interrupts
    MOV     r1, #MODE_SVC:OR:BIT_I:OR:BIT_F
    MSR     CPSR_c, r1

	; Disable caches and MMU
	MRC		p15, 0, r0, c1, c0, 0		; Read CP15 control register
	BIC		r0, r0, #CR_ICACHE
	BIC		r0, r0, #CR_DCACHE
	BIC		r0, r0, #CR_MMU
    ORR     r0, r0, #CR_BPRDCT
	MCR		p15, 0, r0, c1, c0, 0		; Write back control register

    ; Enable I-Cache
    BL      ARM_EnableICache

    ; Remap Peripheral Port
    MRC		p15, 0, r1, c0, c0, 0
    LDR		r0, =0x410FB764	; Check ARM11 ID
    CMP		r0, r1
    BNE		NoRemapPPort
    LDR     r0, =0x2000000b	; 1MB
    MCR     p15, 0, r0, c15, c2, 4

	[ CC_NAND_BOOT
    [ CC_5392B_PRELOADER
    ; Enable 4K+6K LZHS+DMX SRAM
    BL      mt5393_SRAMSetup
    ]
    ]

    LDR r0, =0x20008098
    LDR    r1, =0x5000040          ; Trust zone mode, JTAG backdog with UART passwd
    STR r1, [r0]
            
NoRemapPPort
;    B       NoRemapPPort
    ; Call CHIP_BootupInit
    BL      CHIP_RS232Init
    MOV     r1, #0
    CMP     r0, r1
    BEQ     BootMsg
    BX      r1

BootMsg
    ; Show 'B' to note that console uart is ok.
    MOV     r0, #0x42
    BL      CHIP_DisplayChar

    ; Query SRAM stack pointer address, and pass to ARM_StackITCM by r2.
    BL      CHIP_GetStack
    MOV     r2, r0

    ; Enable ITCM and Setup Stack to ITCM.
    BL      ARM_StackITCM

    ; Jump to C Code
    BL      BSP_Start

;SoftInit
    ; Reset stack
;    MOV     r13, #0xcf0000
;    BL      BSP_Main                    ; Start call BSP_Main program.

HaltSystem
    ; Done, halt system
    LDR     r0, =MsgHalting
    BL      CHIP_DisplayString

SystemDone
    B       SystemDone

;*************************************************************************
;*  5392B_SRAMSetup
;*************************************************************************
	[ CC_NAND_BOOT
    [ CC_5392B_PRELOADER
mt5393_SRAMSetup
    LDR		r0, =0x200170c0				; Enable DMX SRAM
	LDR		r1, [r0]
	LDR 	r2, =0x4000000
	ORR     r1, r1, r2
	STR		r1, [r0]
    BX      lr
    ]
    ]
;*************************************************************************

    ]

	END

