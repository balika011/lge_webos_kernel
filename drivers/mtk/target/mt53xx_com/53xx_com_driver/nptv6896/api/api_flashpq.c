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

// === INCLUDE =============================================================================
#include "x_assert.h"
#include "sv_const.h"
#include "nptv_debug.h"
#include "pe_if.h"
#include "drv_ycproc.h"
#include "drv_pq_cust.h"    // TODO: Remove
#include "vdo_misc.h"
#include "drv_od.h"    

#include "../lzma/LzmaEnc.h"
#include "../lzma/LzmaLib.h"

#ifndef CC_COPLAT_MT82
#include "eeprom_if.h"
#include "u_drv_cust.h"
#include "nor_if.h"
#ifdef CC_NAND_ENABLE
#include "nand_if.h"
#endif
#endif

// === DEFINE =============================================================================
#define ENABLE_FLASH_PQ_DEBUG       0
//#define CC_COMPRESSED_FLASH_PQ_DATA  


#define FLASH_PQ_LOG(x)    (IO32ReadFldAlign(FLASH_PQ_00, FALSH_PQ_LOG_CFG) & x)

#define FLASH_PQ_GAMMA_CHANNEL_NUM  3
#define FLASH_PQ_GAMMA_CHANNEL_SIZE_256  256
#define FLASH_PQ_GAMMA_CHANNEL_SIZE_257  257

#define QTY_SCRIPT_SIZE ((aFlashPqFileHead.bRes & RES_SUPPORT_QTY_TIMING_DESCRIPT) ? TIMING_SCRIPT_SIZE : 0)
#define SMP_SCRIPT_SIZE ((aFlashPqFileHead.bRes & RES_SUPPORT_SMARTPIC_DESCRIPT) ? SMARTPIC_SCRIPT_SIZE : 0)
#define QTY_MAX_MIN_NUM ((aFlashPqFileHead.bRes & RES_SUPPORT_QTY_MAX_MIN)? 2 : 0)
#define QTY_MIN_INDEX   (aFlashPqFileHead.bNumOfSmartPic)       // The end of all smart pic + 1 is MIN table.
#define QTY_MAX_INDEX   (aFlashPqFileHead.bNumOfSmartPic + 1)   // The end of all smart pic + 2 is MAX table.

#define IsSupportLongName() (aFlashPqFileHead.bRes & RES_SUPPORT_LONG_PANEL_NAME)
#define PANEL_LONG_NAME_SIZE (IsSupportLongName() ? TBL_LONG_PANEL_NAME_SIZE : 0)
#define SCRIPT_SIZE ((QTY_SCRIPT_SIZE)+(SMP_SCRIPT_SIZE))

enum
{
    E_FLASH_PQ_GAMMA_LOG        = 0x01,
    E_FLASH_PQ_SCE_LOG          = 0x02,
    E_FLASH_PQ_QTY_LOG          = 0x04, 
    E_FLASH_PQ_QTY_MIN_MAX_LOG  = 0x08,
    E_FLASH_PQ_OD_LOG           = 0x10,
    E_FLASH_PQ_ELSE_LOG         = 0x20,
};

enum
{
    E_OD_SUPPORT_FB     = 0x01,
    E_OD_SUPPORT_PCID   = 0x02,
    E_OD_SUPPORT_ENABLE =0x04,
};

// === FUNCTION PROTOTYPE =================================================================

// === EXTERN FUNCTION ====================================================================

// === GLOBAL VARIABLE ====================================================================

// === EXTERN VARIABLE ====================================================================

// === STATIC VARIABLE ====================================================================
FLASH_PQ_FILE_HEAD aFlashPqFileHead;
FLASH_PQ_TABLE_HEAD aFlashPqGammaHead, aFlashPqSceHead, aFlashPqQtyHead, aFlashPqOdHead;
UINT64 u8PqBaseAddr;
UINT8 bFileHeadSize;
UINT8 bFlashPQInit = SV_FALSE;
UINT8 bPqExtesion = SV_FALSE;
UINT8 bQtyMin[CUSTOM_QTY_TBL_DIM];
UINT8 bQtyMax[CUSTOM_QTY_TBL_DIM];
UINT8* pFlashPqData = NULL;

// === BODY ===============================================================================
static void vDrvGammaTopPt(UINT8 * pu1Table)
{
    if(pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -1] >= pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -2])
    {
        pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256] =
            pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 - 1] +
            (pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -1] - pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 - 2]);
    }
    else
    {
        pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256] =
            pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -1] +
            (UINT8)((UINT16)(pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -1]+0x100) - 
            pu1Table[FLASH_PQ_GAMMA_CHANNEL_SIZE_256 -2]);
    }
}

static UINT64 u8CalcFlashPqOffset(UINT16 nType, UINT16 nModel, UINT16 nSmartPic, UINT16 nSource, UINT16 nIdx)
{
    switch (nType)
    {
        case TYPE_PQ_HEADER:
            return u8PqBaseAddr;
        case TYPE_GAMMA_HEADER:
            return u8PqBaseAddr + bFileHeadSize +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * nModel;
        case TYPE_SCE_HEADER:
            return u8PqBaseAddr + bFileHeadSize +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * nModel;
        case TYPE_QTY_HEADER:
            return u8PqBaseAddr + bFileHeadSize +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) * 
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * nModel;
        case TYPE_QTY_DESCRIPT:
            return u8PqBaseAddr + bFileHeadSize + 
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) *
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * nModel + 
                TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE  + (nSmartPic * aFlashPqFileHead.wSizeOfQtyTable *
                aFlashPqFileHead.bNumOfTiming) + nSource * aFlashPqFileHead.wSizeOfQtyTable;
			
        case TYPE_SMARTPIC_DESCRIPT:
            return u8PqBaseAddr + bFileHeadSize + 
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) *
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * nModel + 
                TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (nSmartPic * aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) +
                nSource * aFlashPqFileHead.wSizeOfQtyTable + SCRIPT_SIZE + nIdx;
			
        case TYPE_QTY_CONTENT:
            return u8PqBaseAddr + bFileHeadSize + 
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) *
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * nModel + 
                TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (nSmartPic * aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) +
                nSource * aFlashPqFileHead.wSizeOfQtyTable + SCRIPT_SIZE + nIdx;        
        case TYPE_OD_HEADER:
            return u8PqBaseAddr + bFileHeadSize +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) *
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * aFlashPqFileHead.wNumOfQty +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfOdTable) * nModel;
    	default:
        case TYPE_FILE_END:
            return u8PqBaseAddr + bFileHeadSize +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable) * aFlashPqFileHead.wNumOfGamma +
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable) * aFlashPqFileHead.wNumOfSCE +
                (TBL_HEAD_SIZE + PANEL_LONG_NAME_SIZE + (aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming) *
                (aFlashPqFileHead.bNumOfSmartPic + QTY_MAX_MIN_NUM)) * aFlashPqFileHead.wNumOfQty + 
                (TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfOdTable) * aFlashPqFileHead.wNumOfOd;
    }
}


static UINT32 dwDrvCalcFlashPqQtyNumber(void)
{
    int i;

    for (i=0; i<CUSTOM_QTY_TBL_DIM; i++)
    {
        if (READ_CUST_QTY_ITEM(i) == QUALITY_MAX)
        {
            return i;
        }
    }

    return 0;
}


static UINT16 wDrvGetODTableSize(UINT16 u2OdFormat)
{
    //_ODParam.u1ODTblSizeMode = u2OdFormat;
    if (u2OdFormat == 0)
    {
        LOG(0, "FlashPQ OD Format : OD_TABLE_17 \n");
        return OD_TBL_S_SIZE;
    }
    else if (u2OdFormat == 1)
    {
        LOG(0, "FlashPQ OD Format : OD_TABLE_33 \n");
        return OD_TBL_M_SIZE;
    }
    else if (u2OdFormat == 2)
    {
        LOG(0, "FlashPQ OD Format : OD_TABLE_41_MODE1 \n");
        return OD_TBL_L_SIZE;
    }
    else if (u2OdFormat == 3)
    {
        LOG(0, "FlashPQ OD Format : OD_TABLE_41_MODE2 \n");
        return OD_TBL_L_SIZE;
    }
    else
    {
        LOG(0, "FlashPQ OD Format Error \n");
        return 0;
    }
}

#if ENABLE_FLASH_PQ_DEBUG 
static void vDrvDebugFileHeader(void)
{
    LOG(1, "=== FlashPQ File Header ===\n");
    LOG(1, "PQ base address:%X %X\n", u8PqBaseAddr>>32, (UINT32)u8PqBaseAddr);
    LOG(1, "ID %s\n", aFlashPqFileHead.bID);
    
    LOG(1, "Gamma num       %d\n", aFlashPqFileHead.wNumOfGamma);
    LOG(1, "Gamma size      %d\n", aFlashPqFileHead.wSizeOfGammaTable);

    LOG(1, "SCE num         %d\n", aFlashPqFileHead.wNumOfSCE);
    LOG(1, "SCE size        %d\n", aFlashPqFileHead.wSizeOfSCETable);

    LOG(1, "Qty num         %d\n", aFlashPqFileHead.wNumOfQty);
    LOG(1, "Qty size        %d\n", aFlashPqFileHead.wSizeOfQtyTable);
    LOG(1, "timing Num      %d\n", aFlashPqFileHead.bNumOfTiming);
    LOG(1, "MinMax          %d\n", aFlashPqFileHead.bRes & RES_SUPPORT_QTY_MAX_MIN ? 1 : 0);    
    LOG(1, "timing Num      %d\n", aFlashPqFileHead.bRes);
    
    LOG(1, "Xml ver         %s\n", aFlashPqFileHead.bQtyXmlVer);

    if (x_strcmp((CHAR *) (aFlashPqFileHead.bID), (CHAR *) FLASHPQ_ID_EX) == 0)
    {
        LOG(1, "OD num          %d\n", aFlashPqFileHead.wNumOfOd);
        LOG(1, "OD size         %d\n", aFlashPqFileHead.wSizeOfOdTable);
        LOG(1, "OD Format       %d\n", aFlashPqFileHead.wConfigOfOd >> 4);
        LOG(1, "Support FB      %d\n", aFlashPqFileHead.wConfigOfOd & E_OD_SUPPORT_FB);
        LOG(1, "Support PCID    %d\n", aFlashPqFileHead.wConfigOfOd & E_OD_SUPPORT_PCID);
    }    
}

static void vDrvDebugSCE(void)
{
    UINT32 i;
    
    Printf("\nBASE_SCE_TBL = \n");
    for (i = 0; i < aFlashPqFileHead.wSizeOfSCETable; i ++)
    {
        Printf("%2x ", BASE_SCE_TBL[i]);
        if ((i+1) % 28 == 0)
        {
            Printf("\n");
        }
    }
    
    Printf("\nMIN_SCE_TBL = \n");
    for (i = 0; i < aFlashPqFileHead.wSizeOfSCETable; i ++)
    {
        Printf("%2x ", MIN_SCE_TBL[i]);
        if ((i+1) % 28 == 0)
        {
            Printf("\n");
        }
    }
}

static void vDrvDebugOD(UINT16 u2OdTblSize)
{    
    UINT32 i;
    
    Printf("\nOD_TBL = \n");
    for (i = 0; i < u2OdTblSize; i ++)
    {
        Printf("%2x ", OD_Table[i]);
        if ((i+1) % 16 == 0)
        {
            Printf("\n");
        }
    }
    Printf("\nFB_TBL = \n");
    for (i = 0; i < FB_TBL_SIZE; i ++)
    {
        Printf("%2x ", FB_Table[i]);
        if ((i+1) % FB_TBL_DIM == 0)
        {
            Printf("\n");
        }
    }

    Printf("\nPCID_TBL = \n");
    for (i = 0; i < PCID_TBL_SIZE; i ++)
    {
        Printf("%2x ", PCID_Table[i]);
        if ((i+1) % PCID_TBL_DIM == 0)
        {
            Printf("\n");
        }
    }
}

static void vDrvDebugGamma(void)
{
    UINT32 i, j;    
    
    for (i = 0; i < FLASH_PQ_GAMMA_CHANNEL_NUM; i ++)
    {        
        Printf("\nGAMMA_256 [%d]= \n", i);
        for (j = 0; j < FLASH_PQ_GAMMA_CHANNEL_SIZE_257; j ++)
        {
            Printf("%2x ", GAMMA_256[i][j]);
            if ((j+1) % 16 == 0)
            {
                Printf("\n");
            }
        }
    }
}

static void vDrvDebugQtyTable(void)
{
    UINT32 i, j;
    
    for (j = 0; j < aFlashPqFileHead.bNumOfTiming; j++)
    {
        Printf("\nTiming %d\n", j);
        for (i = 0; i < aFlashPqFileHead.wSizeOfQtyTable; i++)
        {
            Printf("%02X ", (int)READ_CUSTOM_QTY_TBL(j, i));
            if (i % 16 == 15)
            {
                Printf("\n");
            }
        }
    }
}

static void vDrvDebugQtyMinMax(void)
{   
    int i;
    
    Printf("\nMin  Max\n");
    for (i = 0; i < aFlashPqFileHead.wSizeOfQtyTable - SCRIPT_SIZE; i++)
    {
        Printf("%02X  %02X\n", bQtyMin[i], bQtyMax[i]);
    }

    Printf("\nQty Min Max\n");
    for (i = 0; i < QUALITY_MAX; i++)
    {
        Printf("Dft[%d]  %02X  %02X\n", i, READ_DEFUALT_QTY_TBL_MIN(i), READ_DEFUALT_QTY_TBL_MAX(i));
    }
}
#endif


// Compare the hash between QtyItem.h and FlashPQ QtyHeader.
static UINT8 bDrvCheckFlashPqQtyHash(void)
{
	UINT32 u4QtyNumber = 0;
	UINT8 i;

	// Get the hash from QtyItem.h
	u4QtyNumber = dwDrvCalcFlashPqQtyNumber();
    if (u4QtyNumber == 0)
    {
        LOG(0, "QTY item error!\n");
        return SV_FALSE;
    }
	u4QtyNumber ++; // Get the next position after the QUALITY_MAX entry.

	// Compare the hashs.
	for (i=0; i < TBL_HEAD_HASH; i++)
	{
		// Array index range check.
		if (u4QtyNumber+i < CUSTOM_QTY_TBL_DIM + TBL_HEAD_HASH)
		{
			if (READ_CUST_QTY_ITEM(u4QtyNumber+i) != aFlashPqQtyHead.bHash[i])
			{
				LOG(0, "hash error at %d\n", i);
				return SV_FALSE;
			}
		}
		else
		{
			LOG(0, "Qty index range error %d\n", u4QtyNumber+i);
			return SV_FALSE;
		}
	}
	// Compare finished.
	return SV_TRUE;
}


void vApiFlashPqReadFlash(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen)
{
#ifdef CC_COMPRESSED_FLASH_PQ_DATA    
    x_memcpy((VOID*)u4MemPtr, (VOID*)(pFlashPqData+u8Offset-u8PqBaseAddr), u4MemLen);
#else
    STORG_SyncRead(DRVCUST_OptGet(eFlashPqBasePartition) ,u8Offset, (VOID*)u4MemPtr, u4MemLen);
#endif
}

UINT8 * bApiFlashPqGetVersion(UINT8 bType)
{
    switch (bType)
    {
        default:
        case FLASHPQ_XML_VER:
            return (UINT8 *)(aFlashPqFileHead.bQtyXmlVer);
        case FLASHPQ_GAMMA_VER:
            return (UINT8 *)(aFlashPqGammaHead.bScript);
        case FLASHPQ_SCE_VER:
            return (UINT8 *)(aFlashPqSceHead.bScript);
        case FLASHPQ_QTY_VER:
            return (UINT8 *)(aFlashPqQtyHead.bScript);
        case FLASHPQ_OD_VER:
            return (UINT8 *)(aFlashPqOdHead.bScript);            
    }
}

void vApiFlashPqToDram(void)
{
    UINT8* pImageAddr = NULL;
    UINT32 u4BlockSize;

    LOG(0, "====-- Flash PQ to Dram ======\n");

    if (DRVCUST_OptGet(eFlagFlashPqEnable))
    {
        // Check if flash PQ address & block size definition.
        if ((((UINT64)DRVCUST_OptGet(eFlashPqBasePartition)<<32) +
            DRVCUST_OptGet(eFlashPqBaseAddress) == 0) ||
            (DRVCUST_OptGet(eFlashPqBlockSize) == 0))
        {
            LOG(0, "FlashPQ base address is zero !\n");
            return;
        }

        if (DRVCUST_OptGet(eFlashPqUseNandFalsh))
        {
            u4BlockSize = DRVCUST_OptGet((QUERY_TYPE_T)(eNANDFlashPartSize0+DRVCUST_OptGet(eFlashPqBasePartition)));
        }
        else
        {            
            u4BlockSize = DRVCUST_OptGet((QUERY_TYPE_T)(eNorFlashPartSize0+DRVCUST_OptGet(eFlashPqBasePartition)));
        }
    
        // Get PQ base address.
        u8PqBaseAddr = ((UINT64)DRVCUST_OptGet(eFlashPqBasePartition)<<32) +
            DRVCUST_OptGet(eFlashPqBaseAddress) + DRVCUST_OptGet(eFlashPqOffset);

        // PQ base address must be 32-bit alignment.
        if ((u8PqBaseAddr % 4) != 0)
        {
            LOG(0, "FlashPQ base address is not 32-bit align !\n");
            return;
        }

        pImageAddr = (UINT8*)x_mem_alloc(u4BlockSize);

        if (pImageAddr == NULL)
        {            
            Printf("Flash PQ Allocate Memory failed\n");
            return;
        }
        
        STORG_SyncRead(DRVCUST_OptGet(eFlashPqBasePartition), u8PqBaseAddr,
            (VOID*)pImageAddr, u4BlockSize - u8PqBaseAddr);
    }
    
#ifdef CC_COMPRESSED_FLASH_PQ_DATA  

    UINT32 i;
    UINT32 u4DstSize, u4DstAddr, u4SrcAddr, u4SrcSize;    
    LZMA_FILE_HEADER* prLzmaHdr = (LZMA_FILE_HEADER*)pImageAddr;

    Printf("======  Flash PQ Read to dram Size 0x%x ======\n", u4BlockSize);

    for (i=0; i < sizeof(LZMA_FILE_HEADER); i++)
    {            
        Printf("%02x ", pImageAddr[i]);
        if ((i+1) % 16 == 0) Printf("\n");
    }
    
    pFlashPqData = (UINT8*)x_mem_alloc(prLzmaHdr->org_size);
    if (pFlashPqData == NULL)
    {
        Printf("Flash PQ Allocate Memory %x failed\n", prLzmaHdr->org_size);
        // Flash PQ data in dram
        pFlashPqData = pImageAddr;        
        return;        
    }

    u4DstSize = prLzmaHdr->org_size;
    u4DstAddr = (UINT32)pFlashPqData;
    u4SrcAddr = (UINT32)pImageAddr + sizeof(LZMA_FILE_HEADER);
    u4SrcSize = prLzmaHdr->compressed_size;

    Printf("\n==========  LZMA decode ==========\n");
    Printf("Src  addr=0x%08x   len=%d(0x%08x) \n", u4SrcAddr, u4SrcSize, u4SrcSize);    
    Printf("Dst  addr=0x%08x   len=%d(0x%08x) \n", u4DstAddr, u4DstSize, u4DstSize);

    UINT32 u4Ret = (UINT32)LzmaUncompress((unsigned char*)u4DstAddr, (size_t*)&u4DstSize,
        (unsigned char*)u4SrcAddr, (size_t*)&u4SrcSize,
        (unsigned char *)&prLzmaHdr->props, sizeof(CLzmaEncProps));
    
    if (u4Ret != SZ_OK)
    {
        Printf("LZMA fail, code=%d\n", u4Ret);
        // Flash PQ data in dram
        pFlashPqData = pImageAddr;        
        return;        
    }
    else
    {
        Printf("LZMA decode ok\n");
    }
    
#endif
    
    x_mem_free(pImageAddr);
}


UINT8 bApiFlashPqVerifyHeader(void)
{
    
#ifdef CC_COMPRESSED_FLASH_PQ_DATA 
    vApiFlashPqToDram();
#endif
    
    if (DRVCUST_OptGet(eFlagFlashPqEnable))
    {
        // Check if flash PQ address & block size definition.
        if ((((UINT64)DRVCUST_OptGet(eFlashPqBasePartition)<<32) +
            DRVCUST_OptGet(eFlashPqBaseAddress) == 0) ||
            (DRVCUST_OptGet(eFlashPqBlockSize) == 0))
        {
            LOG(0, "FlashPQ base address is zero !\n");
            return (SV_FALSE);
        }

        // Get PQ base address.
        u8PqBaseAddr = ((UINT64)DRVCUST_OptGet(eFlashPqBasePartition)<<32) +
            DRVCUST_OptGet(eFlashPqBaseAddress) + DRVCUST_OptGet(eFlashPqOffset);

        // PQ base address must be 32-bit alignment.
        if ((u8PqBaseAddr % 4) != 0)
        {
            LOG(0, "FlashPQ base address is not 32-bit align !\n");
            ASSERT(0);
        }

        vApiFlashPqReadFlash(u8PqBaseAddr, (UINT32)&aFlashPqFileHead, (UINT32)(sizeof(struct tagFileHead)));

        if ((x_strcmp((CHAR *) (aFlashPqFileHead.bID), (CHAR *) FLASHPQ_ID) == 0) ||
            (x_strcmp((CHAR *) (aFlashPqFileHead.bID), (CHAR *) FLASHPQ_ID_EX) == 0))
        {            
            if (x_strcmp((CHAR *) (aFlashPqFileHead.bID), (CHAR *) FLASHPQ_ID_EX) == 0)
            {
                bFileHeadSize = sizeof(struct tagFileHead);
                bPqExtesion = SV_TRUE;
            }
            else
            {  
                bFileHeadSize = sizeof(struct tagFileHead) - 32;                   
                bPqExtesion = SV_FALSE;
                LOG(0, "Old version Flash PQ binary \n");
            }
            
			if ((aFlashPqFileHead.wNumOfGamma>>8) == 48)
			{
				aFlashPqFileHead.wNumOfGamma = aFlashPqFileHead.wNumOfGamma & 0xFF;
			}
			if ((aFlashPqFileHead.wNumOfSCE>>8) == 48)
			{
				aFlashPqFileHead.wNumOfSCE= aFlashPqFileHead.wNumOfSCE & 0xFF;
			}
			if ((aFlashPqFileHead.wNumOfQty>>8) == 48)
			{
				aFlashPqFileHead.wNumOfQty = aFlashPqFileHead.wNumOfQty & 0xFF;
			}

            LOG(0, "FlashPQ ID OK\n");
            bFlashPQInit = SV_TRUE;
            
            #if ENABLE_FLASH_PQ_DEBUG
            vDrvDebugFileHeader();
            #endif

            return (SV_TRUE);
        }
        else
        {
            LOG(0, "FlashPQ ID NG\n");
            return (SV_FALSE);
        }
    }
    else
    {
        LOG(0, "Not Support FlashPQ\n");
        return (SV_FALSE);
    }
}

UINT8 bApiFlashPqUpdateGamma(void)
{
    UINT8 bi;
    UINT8* u1GammaBufPtr;
    UINT64 u8GammaAddr;
    UINT16 u2GammaMdlSize = TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfGammaTable;
    UINT16 u2GammaChlSize;
    UINT16 u2GammaIndex = vApiFlashPqGetGammaId();

    if (DRVCUST_OptGet(eFlagFlashPqEnable) == 0)
    {
        return (SV_FALSE);
    }

    if (bFlashPQInit != SV_TRUE)
    {
        LOG(0, "Update Gamma Check init failed\n");
        return (SV_FALSE);
    }

    if (DRVCUST_OptGet(eFlagFlashPqEnableGamma) == 0)
    {
        LOG(0, "FlashPQ not support Gamma\n");
        return (SV_FALSE);
    }

    if (u2GammaIndex >= aFlashPqFileHead.wNumOfGamma)
    {
        LOG(0, "Gamma Index Error! %d > %d \n", u2GammaIndex, aFlashPqFileHead.wNumOfGamma);
        return (SV_FALSE);
    }

    // Calculate start address of Gamma table.
    u8GammaAddr = u8CalcFlashPqOffset(TYPE_GAMMA_HEADER, u2GammaIndex, 0, 0, 0);

    // Gamma head base address must be 32-bit alignment.
    if ((u8GammaAddr % 4) != 0)
    {
        LOG(0, "FlashPQ gamma address error !(%d)\n", u8GammaAddr);
        return (SV_FALSE);
    }

    // Check Gamma table size in FlashPQ bin file: 256 or 257
    if (aFlashPqFileHead.wSizeOfGammaTable == 
        FLASH_PQ_GAMMA_CHANNEL_SIZE_256 * FLASH_PQ_GAMMA_CHANNEL_NUM)
    {
        u2GammaChlSize = FLASH_PQ_GAMMA_CHANNEL_SIZE_256;
    }
    else if (aFlashPqFileHead.wSizeOfGammaTable == 
        FLASH_PQ_GAMMA_CHANNEL_SIZE_256 * FLASH_PQ_GAMMA_CHANNEL_NUM + 16)
    {
        u2GammaChlSize = FLASH_PQ_GAMMA_CHANNEL_SIZE_257;
    }
    else
    {
        LOG(0, "FlashPQ Gamma size error !\n");
        return (SV_FALSE);
    }

    if ((u1GammaBufPtr = (UINT8*)x_mem_alloc(u2GammaMdlSize)) == NULL)
    {
        LOG(0, "FlashPQ Gamma memory request fail !\n");
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8GammaAddr, (UINT32)u1GammaBufPtr, (UINT32)u2GammaMdlSize);

    x_memcpy(&aFlashPqGammaHead, u1GammaBufPtr, TBL_HEAD_SIZE);

    LOG(1, "\nGamma ID        %s\n", aFlashPqGammaHead.bID);
    LOG(1, "PANEL NO        %d\n", u2GammaIndex);
    LOG(1, "DATE            %s\n", aFlashPqGammaHead.bDate);
    LOG(1, "SCRIPT          %s\n", aFlashPqGammaHead.bScript);

    // Check Gamma ID
    if (x_strcmp((CHAR *) (aFlashPqGammaHead.bID), (CHAR *) GAMMA_ID) != 0)
    {
        LOG(0, "Gamma ID [%d]  Error  %s\n", u2GammaIndex, aFlashPqGammaHead.bID);
        x_mem_free(u1GammaBufPtr);
        return (SV_FALSE);
    }
    
    for (bi = 0; bi < FLASH_PQ_GAMMA_CHANNEL_NUM; bi ++)
    {
        x_memcpy(&GAMMA_256[bi], 
            u1GammaBufPtr + TBL_HEAD_SIZE + u2GammaChlSize * bi, 
            u2GammaChlSize);

        // Calculate point 257 if bin file only store 256 points
        if (u2GammaChlSize == FLASH_PQ_GAMMA_CHANNEL_SIZE_256)
        {
            vDrvGammaTopPt(GAMMA_256[bi]);
        }
    }

    #if ENABLE_FLASH_PQ_DEBUG
    if (FLASH_PQ_LOG(E_FLASH_PQ_GAMMA_LOG))
    {
        vDrvDebugGamma();
    }
    #endif
    
    x_mem_free(u1GammaBufPtr);
    LOG(0, "FlashPQ Gamma OK\n");
    return (SV_TRUE);
}

UINT8 bApiFlashPqUpdateSCE(void)
{
    UINT8* u1SceBufPtr;
    UINT64 u8SceAddr;
    UINT16 u2SceMdlSize = TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfSCETable;
    UINT16 u2SceIndex = vApiFlashPqGetSceId();

    if (DRVCUST_OptGet(eFlagFlashPqEnable) == 0)
    {
        return (SV_FALSE);
    }

    if (bFlashPQInit != SV_TRUE)
    {
        LOG(0, "Update SCE Check init failed\n");
        return (SV_FALSE);
    }
    
    if (DRVCUST_OptGet(eFlagFlashPqEnableSCE) == 0)
    {
        LOG(0, "FlashPQ not support SCE\n");
        return (SV_FALSE);
    }

    if (u2SceIndex >= aFlashPqFileHead.wNumOfSCE)
    {
        LOG(0, "SCE Index Error! %d >= %d \n", u2SceIndex, aFlashPqFileHead.wNumOfSCE);
        return (SV_FALSE);
    }

    // Calcuate start address of SCE table.
    u8SceAddr = u8CalcFlashPqOffset(TYPE_SCE_HEADER, u2SceIndex, 0, 0, 0);

    // SCE head base address must be 32-bit alignment.
    if ((u8SceAddr % 4) != 0)
    {
        LOG(0, "FlashPQ SCE address error!!!(%d)\n", u8SceAddr);
        return (SV_FALSE);
    }

    if ((u1SceBufPtr = (UINT8*)x_mem_alloc(u2SceMdlSize)) == NULL)
    {
        LOG(0, "FlashPQ SCE memory request fail\n");
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8SceAddr, (UINT32)u1SceBufPtr, (UINT32)u2SceMdlSize);

    x_memcpy(&aFlashPqSceHead, u1SceBufPtr, TBL_HEAD_SIZE);

    LOG(1, "\nDef SCE ID      %s\n", aFlashPqSceHead.bID);
    LOG(1, "Def SCE NO      %d\n", u2SceIndex);
    LOG(1, "DATE            %s\n", aFlashPqSceHead.bDate);
    LOG(1, "SCRIPT          %s\n", aFlashPqSceHead.bScript);

    // Check SCE ID
    if (x_strcmp((CHAR *) (aFlashPqSceHead.bID), (CHAR *) SCE_ID) != 0)
    {
        LOG(0, "Def SCE ID [%d]  Error  %s\n", u2SceIndex, aFlashPqSceHead.bID);
        x_mem_free(u1SceBufPtr);
        return (SV_FALSE);
    }

    if (aFlashPqFileHead.wSizeOfSCETable > 960)
    {
        LOG(0, "SCE table size %d is too big\n", aFlashPqFileHead.wSizeOfSCETable);
        x_mem_free(u1SceBufPtr);
        return (SV_FALSE);
    }
    
    x_memcpy(&DEFAULT_SCE_TBL, (u1SceBufPtr + TBL_HEAD_SIZE), aFlashPqFileHead.wSizeOfSCETable);
    x_memcpy(&BASE_SCE_TBL, &DEFAULT_SCE_TBL, aFlashPqFileHead.wSizeOfSCETable);

    if (DRVCUST_OptGet(eFlagFlashPqSCEMin) == SV_TRUE) // Normanl Mode
    {        
        u2SceIndex = vApiFlashPqGetMinSceId();
        
        if (u2SceIndex >= aFlashPqFileHead.wNumOfSCE)
        {
            LOG(0, "SCE Index Error! %d >= %d \n", u2SceIndex, aFlashPqFileHead.wNumOfSCE);
            x_mem_free(u1SceBufPtr);
            return (SV_FALSE);
        }
        
        u8SceAddr = u8CalcFlashPqOffset(TYPE_SCE_HEADER, u2SceIndex, 0, 0, 0);

        if ((u8SceAddr % 4) != 0)
        {
            LOG(0, "FlashPQ sce address error!!!(%d)\n", u8SceAddr);
            x_mem_free(u1SceBufPtr);
            return (SV_FALSE);
        }
        
        vApiFlashPqReadFlash(u8SceAddr, (UINT32)u1SceBufPtr, (UINT32)u2SceMdlSize);
        
        x_memcpy(&aFlashPqSceHead, u1SceBufPtr, TBL_HEAD_SIZE);
        
        LOG(1, "\nMin SCE ID      %s\n", aFlashPqSceHead.bID);
        LOG(1, "Min SCE NO      %d\n", u2SceIndex);
        LOG(1, "DATE            %s\n", aFlashPqSceHead.bDate);
        LOG(1, "SCRIPT          %s\n", aFlashPqSceHead.bScript);

        if (x_strcmp((CHAR *) (aFlashPqSceHead.bID), (CHAR *) SCE_ID) != 0)
        {
            LOG(0, "Min SCE ID [%d]  Error  %s\n", u2SceIndex, aFlashPqSceHead.bID);
            x_mem_free(u1SceBufPtr);
            return (SV_FALSE);
        }
        
        x_memcpy(&MIN_SCE_TBL, (u1SceBufPtr + TBL_HEAD_SIZE), aFlashPqFileHead.wSizeOfSCETable);
    }
    
    #if ENABLE_FLASH_PQ_DEBUG
    if (FLASH_PQ_LOG(E_FLASH_PQ_SCE_LOG))
    {
        vDrvDebugSCE();
    }
    #endif
    
    x_mem_free(u1SceBufPtr);    
    LOG(0, "FlashPQ SCE OK\n");
    return (SV_TRUE);
}

UINT8 bApiFlashPqSystemInfo(void)
{
    // support flashpq info only when flashpq enabled
    if(DRVCUST_OptGet(eFlagFlashPqEnable))
    {
        Printf("[FLASHPQ] SUPPORT_FLASH_PQ\n");            
        Printf("[FLASHPQ] FLASH_PQ_BASE_PARTITION = %d\n", DRVCUST_OptGet(eFlashPqBasePartition));

        if(DRVCUST_OptGet(eFlashPqUseNandFalsh))
        {
            Printf("[FLASHPQ] FLASH_PQ_USE_NAND_FLASH\n");            
            Printf("[FLASHPQ] NAND_PART_SIZE = 0x%x\n", DRVCUST_OptGet(eNANDFlashPartSize0+DRVCUST_OptGet(eFlashPqBasePartition)));                
            Printf("[FLASHPQ] FLASH_PQ_BASE_ADDRESS = 0x%x\n", DRVCUST_OptGet(eFlashPqBaseAddress));                    
            Printf("[FLASHPQ] FLASH_PQ_BASE_OFFSET = 0x%x\n", DRVCUST_OptGet(eFlashPqOffset));                                           
        }
        else
        {

            Printf("[FLASHPQ] FLASH_PQ_USE_NOR_FLASH\n");     
            Printf("[FLASHPQ] NOR_PART_SIZE = 0x%x\n", DRVCUST_OptGet(eNorFlashPartSize0+DRVCUST_OptGet(eFlashPqBasePartition)));                    
            Printf("[FLASHPQ] FLASH_PQ_BASE_ADDRESS = 0x%x\n", DRVCUST_OptGet(eFlashPqBaseAddress));                    
            Printf("[FLASHPQ] FLASH_PQ_BASE_OFFSET = 0x%x\n", DRVCUST_OptGet(eFlashPqOffset));                                                       
        }
    }
    else
    {
        Printf("[FLASHPQ] NOT_SUPPORT_FLASH_PQ\n");            
    }
    return 0;
}

UINT8 bApiFlashPqUpdateQtyDft(void)
{
    UINT8* u1QtyPtr;
    UINT8* u1QtyBufPtr;
    UINT8 bSmartPicIndex;
    UINT8 bQtyScriptSize = SCRIPT_SIZE;
    UINT16 u2QtyMdlSize, i;
    UINT16 u2QtyIndex = vApiFlashPqGetQtyId();
    UINT64 u8QtyAddr;

    if (DRVCUST_OptGet(eFlagFlashPqEnable) == 0)
    {
        return (SV_FALSE);
    }

    if (bFlashPQInit != SV_TRUE)
    {
        LOG(0, "Update Qty Check init failed\n");
        return (SV_FALSE);
    }

    if (DRVCUST_OptGet(eFlagFlashPqEnableQty) == 0)
    {
        LOG(0, "FlashPQ not support QtyTable\n");
        return (SV_FALSE);
    }
 
    if (u2QtyIndex >= aFlashPqFileHead.wNumOfQty)
    {
        LOG(0, "Qty Index Error %d >= %d!\n", u2QtyIndex, aFlashPqFileHead.wNumOfQty);
        return (SV_FALSE);
    }

    // Check SmartPic ID
    bSmartPicIndex = DRVCUST_OptGet(eFlagFlashPqEnableQtySmartPic) ? vApiFlashPqGetSmartPicId() : 0;
    
    if (bSmartPicIndex >= aFlashPqFileHead.bNumOfSmartPic)
    {
        LOG(0, "SmartPic Index Error!\n");
        return (SV_FALSE);
    }

    // Read Qty header only
    // Calculate start address of Qty table header.
    u8QtyAddr = u8CalcFlashPqOffset(TYPE_QTY_HEADER, u2QtyIndex, 0, 0, 0);
    
    // Qty head base address must be 32-bit alignment.
    if ((u8QtyAddr % 4) != 0)
    {
        LOG(0, "FlashPQ qty address error!!!(%d)\n", u8QtyAddr);
        return SV_FALSE;
    }
    
    if ((u1QtyBufPtr = (UINT8*)x_mem_alloc(TBL_HEAD_SIZE)) == NULL)
    {
        LOG(0, "FlashPQ Qty memory request fail\n");
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8QtyAddr, (UINT32)u1QtyBufPtr, (UINT32)TBL_HEAD_SIZE);

    x_memcpy(&aFlashPqQtyHead, u1QtyBufPtr, TBL_HEAD_SIZE);
    x_mem_free(u1QtyBufPtr);

    LOG(1, "\nQTY NO      %d\n", u2QtyIndex);
    LOG(1, "DATE        %s\n", aFlashPqQtyHead.bDate);
    LOG(1, "PIC ID      %d\n", bSmartPicIndex);
    LOG(1, "SCRIPT      %s\n", aFlashPqQtyHead.bScript);

    // Read Qty table content
    u2QtyMdlSize = aFlashPqFileHead.wSizeOfQtyTable * aFlashPqFileHead.bNumOfTiming;

    // Calculate start address of Qty table content.
    u8QtyAddr = u8CalcFlashPqOffset(TYPE_QTY_DESCRIPT, u2QtyIndex, bSmartPicIndex, 0, 0);
    
    // Qty head base address must be 32-bit alignment.
    if ((u8QtyAddr % 4) != 0)
    {
        LOG(0, "FlashPQ qty address error!!!(%d)\n", u8QtyAddr);
        return SV_FALSE;
    }
    
    if ((u1QtyBufPtr = (UINT8*)x_mem_alloc(u2QtyMdlSize)) == NULL)
    {
        LOG(0, "FlashPQ Qty memory request fail\n");
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8QtyAddr, (UINT32)u1QtyBufPtr, (UINT32)u2QtyMdlSize);

    // Check Qty ID
    if (x_strcmp((CHAR *) (aFlashPqQtyHead.bID), (CHAR *) QTY_ID) != 0)
    {
        LOG(0, "QTY ID NG\n");
        x_mem_free(u1QtyBufPtr);
        return (SV_FALSE);
    }

    // Check the hash info inside the Qty header.
    if (aFlashPqFileHead.bRes & RES_SUPPORT_QTY_HASH)
    {
        if (!bDrvCheckFlashPqQtyHash())
        {
            LOG(0, "QTY hash error!\n");
            x_mem_free(u1QtyBufPtr);
            return (SV_FALSE);
        }
    }

    // Check QTY table size.
    if (aFlashPqFileHead.wSizeOfQtyTable != CUSTOM_QTY_TBL_DIM)
    {
        LOG(0, "FlashPQ qty table size error!!!(%d!=%d)\n",
            aFlashPqFileHead.wSizeOfQtyTable, CUSTOM_QTY_TBL_DIM);
            x_mem_free(u1QtyBufPtr);
        return (SV_FALSE);
    }

    u1QtyPtr = u1QtyBufPtr;

    for (i = 0; i < aFlashPqFileHead.bNumOfTiming; i ++)
    {           
        x_memcpy(&(bCustomQtyTbl[i]), u1QtyPtr + bQtyScriptSize, 
            aFlashPqFileHead.wSizeOfQtyTable - bQtyScriptSize);

        u1QtyPtr += aFlashPqFileHead.wSizeOfQtyTable;
    }

    #if ENABLE_FLASH_PQ_DEBUG
    if (FLASH_PQ_LOG(E_FLASH_PQ_QTY_LOG))
    {
        vDrvDebugQtyTable();
    }
    #endif
    
    x_mem_free(u1QtyBufPtr);
    LOG(0, "FlashPQ Qty OK\n");
    return (SV_TRUE);
}

UINT8 bApiFlashPqUpdateQtyMinMax(UINT8 u1VidPath)
{
    UINT64 u8QtyAddr;
    UINT16 i;
    UINT8 u1SrcTypTmg;
    UINT8 bQtyScriptSize = SCRIPT_SIZE;
    UINT16 u2QtyIndex = vApiFlashPqGetQtyId();

    if (DRVCUST_OptGet(eFlagFlashPqEnable) == 0)
    {
        return (SV_FALSE);
    }

    if (_fgAutoSearch == TRUE)
    {
        LOG(6, "Search Chaneel, Do Not Update PQ Setting.\n");
        return SV_FALSE;
    }

    if (bFlashPQInit != SV_TRUE)
    {
        LOG(0, "Update MinMax Check init failed\n");
        return (SV_FALSE);
    }

    // Check QtyTable Enable
    if (((aFlashPqFileHead.bRes & RES_SUPPORT_QTY_MAX_MIN) == 0) ||
        (DRVCUST_OptGet(eFlagFlashPqEnableQty) == 0) ||
        (DRVCUST_OptGet(eFlagFlashPqEnableQtyMaxMin) == 0))
    {
        LOG(0, "FlashPQ not support Qty Min/Max\n");
        return (SV_FALSE);
    }

    // Check QTY table size.
    if (aFlashPqFileHead.wSizeOfQtyTable != CUSTOM_QTY_TBL_DIM)
    {
        LOG(0, "FlashPQ qty table size error!!!(%d!=%d)\n",
            aFlashPqFileHead.wSizeOfQtyTable, CUSTOM_QTY_TBL_DIM);
        return (SV_FALSE);
    }

    for (i=0; i< CUSTOM_QTY_TBL_DIM; i++)
    {
        bQtyMin[i] = 0;
        bQtyMax[i] = 0;
    }

    u1SrcTypTmg = bDrvVideoGetSourceTypeTiming(u1VidPath);

    // Transfrom the QtyIndex.
    // Find a match SourceTypeTiming in CustomQtyIdx[].
    u1SrcTypTmg = bApiGetCustomSourceTypeTiming(u1SrcTypTmg);

    if (u1SrcTypTmg == SOURCE_TYPE_TIMING_MAX)
    {
        return (SV_FALSE);
    }

    // Calculate start address of Qty table.
    u8QtyAddr = u8CalcFlashPqOffset(TYPE_QTY_CONTENT, u2QtyIndex, QTY_MIN_INDEX, u1SrcTypTmg, 0);

    // Qty head base address must be 32-bit alignment.
    if ((u8QtyAddr % 4) != 0)
    {
        LOG(0, "FlashPQ qty address error!!!(%d)\n", u8QtyAddr);
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8QtyAddr, (UINT32)&bQtyMin,
        (UINT32)aFlashPqFileHead.wSizeOfQtyTable - bQtyScriptSize);
    
    // Calculate start address of Qty table.
    u8QtyAddr = u8CalcFlashPqOffset(TYPE_QTY_CONTENT, u2QtyIndex, QTY_MAX_INDEX, u1SrcTypTmg, 0);

    // Qty head base address must be 32-bit alignment.
    if ((u8QtyAddr % 4) != 0)
    {
        LOG(0, "FlashPQ qty table address error!!!(%d)\n", u8QtyAddr);
        return (SV_FALSE);
    }
    
    vApiFlashPqReadFlash(u8QtyAddr, (UINT32)&bQtyMax,
        (UINT32)aFlashPqFileHead.wSizeOfQtyTable - bQtyScriptSize);

    for (i=0; i < QUALITY_MAX; i++)
    {
        if ((READ_DEFUALT_QTY_TBL_MIN(i) == 0xFF) &&
            (READ_DEFUALT_QTY_TBL_MAX(i) == 0xFF) &&
            (READ_DEFUALT_QTY_TBL_DFT(i) == 0xFF))
        {
            break;
        }

        if ((READ_DEFUALT_QTY_TBL_REF(i) != FROM_DFT) &&
            (READ_DEFUALT_QTY_TBL_REF(i) < CUSTOM_QTY_TBL_DIM))
        {
            WRITE_DEFUALT_QTY_TBL_MIN(i, bQtyMin[READ_DEFUALT_QTY_TBL_REF(i)]);
            WRITE_DEFUALT_QTY_TBL_MAX(i, bQtyMax[READ_DEFUALT_QTY_TBL_REF(i)]);
        }
    }

    #if ENABLE_FLASH_PQ_DEBUG
    if (FLASH_PQ_LOG(E_FLASH_PQ_QTY_MIN_MAX_LOG))
    {
        vDrvDebugQtyMinMax();
    }
    #endif

    LOG(0, "FlashPQ Qty Min/Max OK\n");
    return (SV_TRUE);
}

UINT8 bApiFlashPqUpdateOD(void)
{
    UINT8* u1OdBufPtr;
    UINT64 u8OdAddr;
    UINT16 u2OdMdlSize = TBL_HEAD_SIZE + aFlashPqFileHead.wSizeOfOdTable;
    UINT16 u2OdFormat = aFlashPqOdHead.bConfig[0];
    UINT16 u2OdIndex = vApiFlashPqGetOdId();
    UINT16 u2OdTblSize = 0;
    
    if (DRVCUST_OptGet(eFlagFlashPqEnable) == 0)
    {
        return (SV_FALSE);
    }
    
    if (bFlashPQInit != SV_TRUE)
    {
        LOG(0, "Update OD Check init failed\n");
        return (SV_FALSE);
    }  
    
    if (bPqExtesion == SV_FALSE)
    {        
        LOG(0, "Old version Flash PQ binary Not support OD\n");
        return (SV_FALSE);
    }

    if (u2OdIndex >= aFlashPqFileHead.wNumOfOd)
    {
        LOG(0, "OD Index Error %d >= %d !\n", u2OdIndex, aFlashPqFileHead.wNumOfOd);
        return (SV_FALSE);
    }

    u8OdAddr = u8CalcFlashPqOffset(TYPE_OD_HEADER, u2OdIndex, 0, 0, 0);

    if ((u8OdAddr % 4) != 0)
    {
        LOG(0, "FlashPQ OD address error!!!(%d)\n", u8OdAddr);
        return (SV_FALSE);
    }

    if ((u1OdBufPtr = (UINT8*)x_mem_alloc(u2OdMdlSize)) == NULL)
    {
        LOG(0, "FlashPQ OD memory request fail\n");
        return (SV_FALSE);
    }

    vApiFlashPqReadFlash(u8OdAddr, (UINT32)u1OdBufPtr, (UINT32)u2OdMdlSize);

    x_memcpy(&aFlashPqOdHead, u1OdBufPtr, TBL_HEAD_SIZE);

    LOG(1, "\nOD ID       %s\n", aFlashPqOdHead.bID);
    LOG(1, "OD NO           %d\n", u2OdIndex);
    LOG(1, "DATE            %s\n", aFlashPqOdHead.bDate);
    LOG(1, "SCRIPT          %s\n", aFlashPqOdHead.bScript);
    LOG(1, "Format          %d\n", aFlashPqOdHead.bConfig[0]>4);
    LOG(1, "Support OD Enable      %d\n", (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_ENABLE) ? 1 : 0);
    LOG(1, "Support FB      %d\n", (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_FB) ? 1 : 0);
    LOG(1, "Support PCID    %d\n", (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_PCID) ? 1 : 0);
    
    if (x_memcmp((void *) (aFlashPqOdHead.bID), (void *) OD_ID, TBL_HEAD_ID_SIZE) != 0)
    {
        LOG(0, "OD ID [%d] Error  %s\n", u2OdIndex, aFlashPqOdHead.bID);
        x_mem_free(u1OdBufPtr);
        return (SV_FALSE);
    }

    _ODParam.u1ODTblSizeMode = aFlashPqOdHead.bConfig[0] >>4;
    u2OdFormat = aFlashPqFileHead.wConfigOfOd>>4;
    u2OdTblSize = wDrvGetODTableSize(u2OdFormat);
    
    x_memcpy(OD_Table, (u1OdBufPtr + TBL_HEAD_SIZE), u2OdTblSize);

     _ODParam.u1FB_ON =0;
     _ODParam.u1PCID_ON =0;
     _ODParam.u1OD_ON =0;
    if (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_FB)
    {
        _ODParam.u1FB_ON =1;
        x_memcpy(FB_Table, (u1OdBufPtr + TBL_HEAD_SIZE + u2OdTblSize + 15), FB_TBL_SIZE);
    }
    
    if (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_PCID)
    {
        _ODParam.u1PCID_ON =1;
        x_memcpy(PCID_Table, (u1OdBufPtr + TBL_HEAD_SIZE + u2OdTblSize + 15 + PCID_TBL_SIZE + 15), 
            PCID_TBL_SIZE);
    }
    
    if (aFlashPqOdHead.bConfig[0] & E_OD_SUPPORT_ENABLE)
    {
        _ODParam.u1OD_ON =1;
    }
    
    #if ENABLE_FLASH_PQ_DEBUG
    if (FLASH_PQ_LOG(E_FLASH_PQ_OD_LOG))
    {
        vDrvDebugOD(u2OdTblSize);
    }
    #endif
    
    x_mem_free(u1OdBufPtr);    
    LOG(0, "FlashPQ OD OK\n");
    return (SV_TRUE);
}

