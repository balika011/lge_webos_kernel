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
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: board.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file ddr.c
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_bim.h"
#include "x_printf.h"
#include "x_dram.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "x_timer.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

#ifdef CC_SUPPORT_STR_CORE_OFF
//-------------------------------------------------------------------------
/** DramcEnterSR
 *  DRAMC issue self refresh command to DRAM
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (U8): 0: OK, others: Fail
 */
//-------------------------------------------------------------------------
/*
U8 DramcWriteDataToDramBeforeEnterSR(void)
{
    U32 u4err_value;
	U32 test2_0 = DEFAULT_TEST2_0_CAL;
    U32 test2_1 = DEFAULT_TEST2_1_CAL;
    U32 test2_2 = DEFAULT_TEST2_2_CAL;

    //Write pre-defined data into dram (selftest2 in write mode)
    //u4err_value = DramcEngine2(TE_OP_WRITE_READ_CHECK, test2_0, test2_1, test2_2);
    u4err_value = 0;

    //Issue self-refresh command to dram
    //entry self refresh    4h [26]	
    //u4value = ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1));
    //mcSET_BIT(u4value, POS_CONF1_SELFREF);
    //ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1), u4value);

    //Wait at least 9*tREFI+tCKSRE+20*tMIOCK (or 1ms)
    HAL_Delay_us(1);

    mcSHOW_DBG_MSG("%d (2: cha, 3: chb) DRAMC enter self refresh...err_value=%8x\n", IS_DRAM_CHANNELB_ACTIVE(), u4err_value);

	UNUSED(u4err_value);
	UNUSED(test2_0);
	UNUSED(test2_1);
	UNUSED(test2_2);

    return DRAM_OK;
}
*/

void DramcBackupReg(UINT32 *u4DRAMCTemp)
{
    U8 ii;
    U32 u4value;
	U8 DRAM_CHA_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHA_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
	U8 DRAM_CHB_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHB_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;


	// DRAMC backup registers
	U16 gu2dramc_backup_regaddr[DRAMC_BACKUP_REG_NUM] = {DRAMC_REG_DELDLY1,DRAMC_REG_DDR2CTL,DRAMC_REG_PADCTL4,\
                                                         DRAMC_REG_DQIDLY1,DRAMC_REG_DQIDLY2,DRAMC_REG_DQIDLY3,\
		                                                 DRAMC_REG_DQIDLY4,DRAMC_REG_DQIDLY5,DRAMC_REG_DQIDLY6,\
		                                                 DRAMC_REG_DQIDLY7,DRAMC_REG_DQIDLY8};

	U16 gu2dram_phy_cha_backup_regaddr[DRAMPHY_CHA_BACKUP_REG_NUM] = {0x010,0x014,0x100,0x104,0x108, \
	                                                                  0x120,0x128,0x12c,0x200,0x204, \
	                                                                  0x208,0x220,0x228,0x22c,0x304, \
																	  0x30c,0x404,0x408,0x40c,0x410, \
																	  0xa18,0xa1c,0xb04};
	
    U16 gu2dram_phy_chb_backup_regaddr[DRAMPHY_CHB_BACKUP_REG_NUM] = {0x010,0x014,0x100,0x104,0x108, \
		                                                              0x120,0x128,0x12c,0x200,0x204, \
		                                                              0x208,0x220,0x228,0x22c,0x304, \
																	  0x30c,0x404,0xa18,0xa1c};   

    U16 gu2dram_phy_chc_backup_regaddr[DRAMPHY_CHC_BACKUP_REG_NUM] = {0x010,0x014,0x100,0x104,0x108, \
                                                                      0x120,0x128,0x12c,0x304,0x30c, \
                                                                      0xa18,0xa1c};  

	U16 gu2dram_arbiter_backup_regaddr[DRAM_ARBITER_BACKUP_REG_NUM] ={0x000};

    if (IS_DRAM_CHANNELA_ACTIVE()) 
    {
        //CHA DRAMC registers
		for (ii = 0; ii < DRAMC_BACKUP_REG_NUM; ii++)
		{
			u4value = ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(gu2dramc_backup_regaddr[ii]));
			*(u4DRAMCTemp+ii) = u4value;
			//Printf("CHA gu2dramc_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
		}

		//CHA DRAM PHY registers
        for (ii = 0; ii < DRAMPHY_CHA_BACKUP_REG_NUM; ii++)
        {
            u4value = ucDram_Register_Read(mcSET_PHY_REG_ADDR(gu2dram_phy_cha_backup_regaddr[ii]));
            *(u4DRAMCTemp+DRAMC_BACKUP_REG_NUM+ii) = u4value;
			//Printf("CHA gu2dram_phy_cha_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }

		//CHA DRAM ARBITER registers
        for (ii = 0; ii < DRAM_ARBITER_BACKUP_REG_NUM; ii++)
        {
		    u4value = ucDram_Register_Read(mcSET_ARBITER_ADDR(gu2dram_arbiter_backup_regaddr[ii]));
		    *(u4DRAMCTemp+DRAMC_BACKUP_REG_NUM+DRAMPHY_CHA_BACKUP_REG_NUM+ii) = u4value;
			//Printf("CHA gu2dram_arbiter_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }
    }
	else if(IS_DRAM_CHANNELB_ACTIVE()) 
	{
	    //CHB DRAMC registers
		for (ii = 0; ii < DRAMC_BACKUP_REG_NUM; ii++)
		{
			u4value = ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(gu2dramc_backup_regaddr[ii]));
			*(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+ii) = u4value;
			//Printf("CHB gu2dramc_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
		}
		
		//CHB DRAM PHY registers
        for (ii = 0; ii < DRAMPHY_CHB_BACKUP_REG_NUM; ii++)
        {
            u4value = ucDram_Register_Read(mcSET_PHY_REG_ADDR(gu2dram_phy_chb_backup_regaddr[ii]));
            *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAMC_BACKUP_REG_NUM+ii) = u4value;
			//Printf("CHB gu2dram_phy_chb_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }
		
		//CHA DRAM ARBITER registers
        for (ii = 0; ii < DRAM_ARBITER_BACKUP_REG_NUM; ii++)
        {
		    u4value = ucDram_Register_Read(mcSET_ARBITER_ADDR(gu2dram_arbiter_backup_regaddr[ii]));
		    *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAMC_BACKUP_REG_NUM+DRAMPHY_CHB_BACKUP_REG_NUM+ii) = u4value;
			//Printf("CHB gu2dram_arbiter_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }
	}
	else
	{
	if(!IS_IC_5861())
		{
	    //CHC DRAMC registers
	    for (ii = 0; ii < DRAMC_BACKUP_REG_NUM; ii++)
 	    {
		  u4value = ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(gu2dramc_backup_regaddr[ii]));
		  *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+ii) = u4value;
		  //Printf("CHC gu2dramc_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
     	}
		//CHC DRAM PHY registers
        for (ii = 0; ii < DRAMPHY_CHC_BACKUP_REG_NUM; ii++)
        {
          u4value = ucDram_Register_Read(mcSET_PHY_REG_ADDR(gu2dram_phy_chc_backup_regaddr[ii]));
          *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAMC_BACKUP_REG_NUM+ii) = u4value;
		  //Printf("CHC gu2dram_phy_chc_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }
		//CHC DRAM ARBITER registers
        for (ii = 0; ii < DRAM_ARBITER_BACKUP_REG_NUM; ii++)
        {
		  u4value = ucDram_Register_Read(mcSET_ARBITER_ADDR(gu2dram_arbiter_backup_regaddr[ii]));
		  *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAMC_BACKUP_REG_NUM+DRAMPHY_CHC_BACKUP_REG_NUM+ii) = u4value;
		  //Printf("CHC gu2dram_arbiter_backup_regaddr[%d]= 0x%8x.\n",ii,u4value);
        }
		}
	}

}    
void DramcBackupCommonReg(UINT32 *u4DRAMCTemp)
{
	U8 DRAM_CHA_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHA_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
	U8 DRAM_CHB_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHB_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
    U8 DRAM_CHC_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHC_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;

if(!IS_IC_5861())
{
    *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAM_CHC_BACKUP_NUM) = TCM_DRAM_SIZE;
    *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAM_CHC_BACKUP_NUM+1) = TCM_DRAM_FLAGS1;
}
else
{
    *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM) = TCM_DRAM_SIZE;
    *(u4DRAMCTemp+DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+1) = TCM_DRAM_FLAGS1;
}
}

//-------------------------------------------------------------------------
/** DramcSuspend
 *  Suspend DRAMC and DDRPHY
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL 
 */
//-------------------------------------------------------------------------
UINT32 DRAMC_str_save_registers(UINT32 *u4DRAMCTemp, UINT32 u4Size)
{
	U8 DRAM_CHA_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHA_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
	U8 DRAM_CHB_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHB_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
    U8 DRAM_CHC_BACKUP_NUM = DRAMC_BACKUP_REG_NUM+DRAMPHY_CHC_BACKUP_REG_NUM+DRAM_ARBITER_BACKUP_REG_NUM;
	
	// enter self refresh
    TCMSET_CHANNELA_ACTIVE();
    //DramcWriteDataToDramBeforeEnterSR();
	DramcBackupReg(u4DRAMCTemp);
	
    TCMSET_CHANNELB_ACTIVE();
    //DramcWriteDataToDramBeforeEnterSR();
    DramcBackupReg(u4DRAMCTemp);
	
	if(!IS_IC_5861())
		{
		    TCMSET_CHANNELC_ACTIVE();
		    DramcBackupReg(u4DRAMCTemp);
		}

	DramcBackupCommonReg(u4DRAMCTemp);

    // no suspend S1 state in MT5399
    TCMSET_DRAM_SUSPEND();

    // Set DMSUS33=1, for MT5399, it is GPIO control
 	
    // RSTB33=0, for MT5399, it is GPIO control
	
    // Shut down VCCK, for MT5399, it is GPIO control
	
    // Shut down AVDD33, for MT5399, it is GPIO control

	TCMSET_CHANNELA_ACTIVE();

if(!IS_IC_5861())
{
    return (DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAM_CHC_BACKUP_NUM+DRAM_COMMON_BACKUP_REG_NUM)*sizeof(UINT32);
}
else
{
    return (DRAM_CHA_BACKUP_NUM+DRAM_CHB_BACKUP_NUM+DRAM_COMMON_BACKUP_REG_NUM)*sizeof(UINT32);
}
   
}


static UINT32* pm_save_tcm_msg= NULL;

void DMARC_TCM_suspend(void)
{
	pm_save_tcm_msg = x_mem_alloc(8 * sizeof(UINT32));//
	Printf("DMARC_TCM_suspend addr : 0x%x .\n",pm_save_tcm_msg);
	if (pm_save_tcm_msg!=NULL)
	{
		pm_save_tcm_msg[0] = TCM_DRAM_SIZE;
		pm_save_tcm_msg[1] = TCM_DRAM_FLAGS;
		pm_save_tcm_msg[2] = TCM_DRAM_FLAGS1;
		pm_save_tcm_msg[3] = TCM_DRAM_FLAGS2;
		pm_save_tcm_msg[4] = TCM_DRAM_FLAGS3;
		pm_save_tcm_msg[5] = TCM_DRAM_DLYCELL_PERT;
		pm_save_tcm_msg[6] = _global_DRAMC_base;
		pm_save_tcm_msg[7] = _global_DRAMPHY_base;	
	}
}

void DMARC_TCM_resume(void)
{
	if (pm_save_tcm_msg!=NULL)
	{
		TCM_DRAM_SIZE 			= pm_save_tcm_msg[0];
		TCM_DRAM_FLAGS 			= pm_save_tcm_msg[1];
		TCM_DRAM_FLAGS1 		= pm_save_tcm_msg[2];
		TCM_DRAM_FLAGS2 		= pm_save_tcm_msg[3];
		TCM_DRAM_FLAGS3			= pm_save_tcm_msg[4];
		TCM_DRAM_DLYCELL_PERT	= pm_save_tcm_msg[5];
		_global_DRAMC_base      = pm_save_tcm_msg[6];
		_global_DRAMPHY_base    = pm_save_tcm_msg[7];
		x_mem_free(pm_save_tcm_msg);
	}
}

void DMARC_TCM_partial_resume(void)
{
	if (pm_save_tcm_msg!=NULL)
	{
		TCM_DRAM_SIZE 			= pm_save_tcm_msg[0];
	}
}

#endif
