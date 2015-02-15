/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
 
/** @file resume.c
 *  provides functions for resume from suspend to ram
 *  Used in preloader (for both Linux & Nucleus boot)
 *
 */

#ifdef CC_MTK_PRELOADER

#include "x_bim.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"
#include "x_hal_arm.h"
#include "x_suspend.h"
#ifdef CC_TRUSTZONE_SUPPORT
#include "c_model.h" // for TOTAL_MEM_SIZE, TRUSTZONE_MEM_SIZE, and TRUSTZONE_CODE_BASE
#endif
#if defined(CC_SUPPORT_STR_CORE_OFF)
#include "x_pdwnc.h"
#endif
#include "x_dram.h" 

EXTERN void RemapJumpResume(void);
EXTERN int RemapJumpResume_sz;
EXTERN UINT32 CHIP_T8032Cmd(PDWNC_T8032_CMD_T *rCmd);
EXTERN void CHIP_DisplayString(const CHAR *szString);
EXTERN void CHIP_DisplayHex(UINT32 u4Val);

#ifdef CC_TRUSTZONE_SUPPORT                                                          
void __attribute__((__noinline__)) wrapper_theKernel(unsigned int addr, int zero, int arch, unsigned int params, unsigned int kernel_entry)
{
	void	(*theKernel)(int zero, int arch, unsigned int params, unsigned int kernel_entry);
	//kernel_entry = __builtin_return_address(0);
	kernel_entry = TEMP_IRAM_AREA; // return to sram
	theKernel = (void (*)(int, int, unsigned int, unsigned int))addr; // jump to tz INT_RESET
	//printf("[%4d] Jump to kernel...ep=0x%08lx, machid=0x%x, bi=0x%lx\n", readMsTicks(), kernel_entry, arch, params);
	theKernel (zero, arch, params, kernel_entry);
}

void init_tz(unsigned int arg1)
{
        void    (*theKernel)(int zero, int arch, unsigned int params, unsigned int kernel_entry); 

#if defined(CC_TRUSTZONE_IN_CHB)
	theKernel = (void (*)(int, int, unsigned int, unsigned int))(TCMGET_CHANNELA_SIZE()*0x100000 + TCMGET_CHANNELB_SIZE()*0x100000 - TRUSTZONE_MEM_SIZE + TRUSTZONE_CODE_BASE);
#else
	theKernel = (void (*)(int, int, unsigned int, unsigned int))(TOTAL_MEM_SIZE - TRUSTZONE_MEM_SIZE + TRUSTZONE_CODE_BASE);
#endif
	CHIP_DisplayString("theKernel: ");
	CHIP_DisplayHex((UINT32)theKernel);
	CHIP_DisplayString("\n");
	wrapper_theKernel((unsigned int)theKernel, arg1, 0, 0, 0); // ignore some parameters

	CHIP_DisplayString("Return back to preloader\n");
}
#endif

//#define DEBUG_DDR_RESUME
#ifdef DEBUG_DDR_RESUME
UINT32 volatile _yjdbg = 0;
#endif
void CHIP_Resume(void)
{
	#if defined(CC_MT5890)
    UINT32 uDRAMCTemp[PDWNC_SUSPEND_SAVE_SIZE];
	#else
	UINT32 uDRAMCTemp[SUSP_DRAMC_PER_CHANNEL*2+1];
	#endif
    UINT32 i;
    //void (*resumePtr)(UINT32);

    // Read DRAMC register from PDWNC data SRAM.
    {
        UINT32 u4IntStatus;	
        volatile UINT8* pu1Ptr = (volatile UINT8*)uDRAMCTemp;
        
        u4IntStatus = (UINT32)IO_READ32(PDWNC_T8032_INTEN,0);
        IO_WRITE32(PDWNC_T8032_INTEN,0,0);//disable all interrupt
        
		PDWNC_T8032_CMD_T rCmd;
		rCmd.u1Cmd = PDWNC_CMD_ARMT8032_XDATA_SWITCH;
		rCmd.u1SubCmd = 0x1;
		MSET_XDATA_TO_ARM_FLAG();
		CHIP_T8032Cmd(&rCmd);
		vIO32WriteFldAlign(PDWNC_UP_CFG, 0x1, FLD_XDATA_ACC);//switch xdata to ARM	  
		vIO32WriteFldAlign(PDWNC_UP_ADDR,(XDATA_ADDR + PDWNC_SUSPEND_SAVE_OFFSET), FLD_UP_ADDR);
		for(i = 0; i < PDWNC_SUSPEND_SAVE_SIZE; i++)
		{
				*pu1Ptr = u1IO32Read1B(PDWNC_UP_DATA);
				pu1Ptr++;  
		}
		vIO32WriteFldAlign(PDWNC_UP_CFG, 0x0, FLD_XDATA_ACC);//switch xdata to T8032
		MCLR_XDATA_TO_ARM_FLAG();

        IO_WRITE32(PDWNC_INTCLR,0,0xffffffff);//clear all interrupt
        IO_WRITE32(PDWNC_T8032_INTEN,0,u4IntStatus);//restore all interrupt
    }
#if 0 // show dramc register value. // yjdbg
	Printf("restore dramc register.\n");
	for(i=0;i<PDWNC_SUSPEND_SAVE_SIZE/4;i+=4)
	{
		Printf("%d: 0x%08X  0x%08X  0x%08X  0x%08X .\n",i,uDRAMCTemp[i],uDRAMCTemp[i+1],uDRAMCTemp[i+2],uDRAMCTemp[i+3]);
	}
#endif
    // Calling DRAMC function to restore DRAMC register from DRAM
    DRAMC_str_restore_registers(uDRAMCTemp+1);
#if defined(CC_MT5399)||defined(CC_MT5882)
	vIO32WriteFldAlign(PDWNC_CONCFG1,0,FLD_RG_DMSUS);  //set DMSUS=0
    vIO32WriteFldAlign(PDWNC_CONCFG0,1,FLD_RG_DMSUS_RESERVED);
#elif defined(CC_MT5881)
    vIO32WriteFldAlign(PDWNC_CONCFG0,0,FLD_RG_DMSUS);  //set DMSUS=0
    vIO32WriteFldAlign(PDWNC_CONCFG0,0,FLD_RG_DMSUS_RESERVED);
#elif defined(CC_MT5890)
	vIO32WriteFldAlign(PDWNC_CONCFG2,0,FLD_RG_DMSUS);  //set DMSUS=0
    vIO32WriteFldAlign(PDWNC_CONCFG1,0,FLD_RG_DMSUS_RESERVED);
#endif 
    DRAMC_Exit_Suspend(uDRAMCTemp+1);//after DMSUS=0, exit self-refresh mode.

#ifdef DEBUG_DDR_RESUME
    CHIP_DisplayString("B4 DRAMC_Exit_Suspend!!\n");
    _yjdbg = 1;
    while (_yjdbg);
    CHIP_DisplayString("AT DRAMC_Exit_Suspend!!\n");
#endif
#if 0
    // Print first DRAM read value to debug
    CHIP_DisplayHex(uDRAMCTemp[0]);
    CHIP_DisplayString(":  ");
    CHIP_DisplayHex(*(UINT32*)uDRAMCTemp[0]);
    CHIP_DisplayString("\n");
#endif

    // Good, we can read/write DRAM now :)
    // Move RemapJumpResume to SRAM
    x_memcpy((void *) TEMP_IRAM_AREA, (void *)&RemapJumpResume, RemapJumpResume_sz);

    #if defined(CC_LOAD_UBOOT)
    // only for Linux preloader (not for Nucleus preloader)
    x_memcpy((void *) (TEMP_IRAM_AREA+0x800), ((void*)((*(UINT32*)(uDRAMCTemp[0])) & 0xfffff000)), 0x800); // copy cpu_Resume to sram+0x800, address of gcc_suspend_1176.S is 4K alignment
    *(UINT32*)uDRAMCTemp[0] = TEMP_IRAM_AREA+0x800+((*(UINT32*)uDRAMCTemp[0])&0xfff); // cyj.note: affect CRC
    #endif

    CHIP_DisplayString("Wakeup now!!\n");

#ifdef CC_ROM_BOOT                 
    BIM_WRITE32(4, 0xf4000000);
#endif /* CC_ROM_BOOT */           

    BIM_SetTimeLog(1);

    // invalidate for sram code
    HalInvalidateICache();

#if defined(CC_MT5399) || defined(CC_MT5890)
    // clean D cache for CA9
    HalInvalidateDCache();  // TODO: should be ok for ARM11 // NOTE: should invalidate D-cache before we enable it
#endif

#ifdef CC_TRUSTZONE_SUPPORT
    // cleanup for tz
    //HalFlushInvalidateDCache(); // assumption: preloader doesnot enable D-cache
    //HalDisableDCache();
    HalDisableICache();

    BIM_WRITE32(0x1c, 0x1); // Remap

    init_tz((unsigned int)uDRAMCTemp); // never return, jump to sram
#else
    // Use RemapJumpResume to trigger remap and jump to resume code.
    (void) ((void (*)(UINT32 *))TEMP_IRAM_AREA)(uDRAMCTemp);
#endif

    // Should not return!!! Print messge and reboot!
    CHIP_DisplayString("Wake fail, reboot!\n");
    
    // Print first DRAM read value to debug
    CHIP_DisplayHex(uDRAMCTemp[0]);
    CHIP_DisplayString(":  ");
    CHIP_DisplayHex(*(UINT32*)uDRAMCTemp[0]);
    CHIP_DisplayString("\n");

    for (i=0; i<15; i++)
        CHIP_DisplayString("!!!!!!!!!!!!!!!!\n");

    // Reboot.
    vIO32Write4B(PDWNC_WDTRST0,0x0fffffff);
    vIO32Write4B(PDWNC_WDT0,0x7fff0000);
    vIO32Write4B(PDWNC_WDTCTL, (u4IO32Read4B(PDWNC_WDTCTL) & 0x00ffffff) | 0x1);

    while (1);
}


void CHIP_ResumeFormSuspendMode()
{
    //Check for resume from suspend to dram
#if defined(CC_MT5399) || defined(CC_MT5882)
    if(IO32ReadFldAlign(PDWNC_CONCFG0,FLD_RG_DMSUS_RESERVED) == 0)//WAKEUP_FROM_SUSPEND()
#elif defined(CC_MT5881)
    if(IO32ReadFldAlign(PDWNC_CONCFG0,FLD_RG_DMSUS_RESERVED) == 1)//WAKEUP_FROM_SUSPEND()
#elif defined(CC_MT5890)
    if(IO32ReadFldAlign(PDWNC_CONCFG1,FLD_RG_DMSUS_RESERVED) == 1)
#endif
    {
        BIM_SetTimeLog(0);
        // Jump to resume function, no return.
        CHIP_Resume();

        // No return.
        while(1);
    }
}
#endif /* CC_MTK_PRELOADER */
