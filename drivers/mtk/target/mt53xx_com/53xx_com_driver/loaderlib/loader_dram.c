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
 * Description:
 *         The main routine of boot loader
 *
 *---------------------------------------------------------------------------*/

#ifdef CC_DRAM_TUNING
//---------------------------------------------------------------------
// include files
//---------------------------------------------------------------------
#include "loader_if.h"
#include "x_serial.h"
#include "x_drv_cli.h"
#include "x_ldr_env.h"
#include "x_dram.h"
#include "x_timer.h"
#include "pcie_if.h"

//---------------------------------------------------------------------
// Config
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------
#define PRELOADER_SIZE      (40 * 1024)     // 40K

#if defined(CC_NAND_BOOT)
#define CHIP_FlashCopyToDRAMLZHS(u4PartId, u4Dest, u4Offset, u4Len)  Loader_ReadNandFlash(u4PartId, u4Offset, (void *)(u4Dest), u4Len)
#elif defined(CC_EMMC_BOOT)
#define CHIP_FlashCopyToDRAMLZHS(u4PartId, u4Dest, u4Offset, u4Len)  Loader_ReadMsdc(u4PartId, u4Offset, (void *)(u4Dest), u4Len)
#else
#define CHIP_FlashCopyToDRAMLZHS(u4PartId, u4Dest, u4Offset, u4Len)
#endif

//---------------------------------------------------------------------
// External declarations
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------
static UINT32 _LdrBlockSize()
{
    UINT32 u4BlkSize;

    #if defined(CC_NAND_BOOT) // nand boot
    u4BlkSize = Loader_GetNandFlashBlockSize();
    if(u4BlkSize <= 0)
    {
        u4BlkSize = 16*1024;
    }
    Printf("NAND Erase Unit:0x%x\n", u4BlkSize);
    #elif defined(CC_EMMC_BOOT) // emmc boot
    u4BlkSize = Loader_GetMsdcEraseSize();
    if(u4BlkSize <= 0)
    {
        u4BlkSize = 128*1024;
    }
    Printf("Msdc Erase Unit:0x%x\n", u4BlkSize);
    #else // nor boot
    u4BlkSize = 64*1024;
    Printf("Nor Erase Unit:0x%x\n", u4BlkSize);
    #endif

    return u4BlkSize;
}

static UINT32 Loader_WriteNorFlash(UINT8 u1PartId, UINT32 u4Offset, void *pvMemPtr, UINT32 u4Size)
{
    UINT32 u4BlkSize = 0;
    UINT32 u4WroteSize = 0;
    UINT32 u4WritePoint = 0;
    UINT32 u4MemPtr = 0, i;
    UINT32 *pu4Buf = 0;
    UINT32 *pu4Cmp;

    u4BlkSize = _LdrBlockSize();
    u4MemPtr = (UINT32)pvMemPtr;
    pu4Buf = x_mem_alloc(u4BlkSize);
    if(pu4Buf == NULL)
    {
        return 0;
    }

    do
    {
        if (NOR_EraseBlk(u4WritePoint))
        {
            Printf("Nor flash erase fail: Offset:0x%08x\n", u4WritePoint);
        }

        if (NOR_Write((UINT64)u4WritePoint, u4MemPtr, u4BlkSize) )
        {
            Printf("Nor flash write fail: Offset:0x%08x, BlkSize:0x%x\n", u4WritePoint, u4BlkSize);
        }

        if (NOR_Read((UINT64)u4WritePoint, (UINT32)pu4Buf, u4BlkSize) )
        {
            Printf("Nor flash read fail: Offset:0x%08x, BlkSize:0x%x\n", u4WritePoint, u4BlkSize);
        }

        pu4Cmp = (UINT32 *)u4MemPtr;
        for (i = 0; i < u4BlkSize/4; i++)
        {
            if ((pu4Buf != NULL)&&(pu4Cmp[i] != pu4Buf[i]))
            {
                Printf("compare fail\n");
                return u4WroteSize;
            }
        }

        Printf("Nor flash write complete: Offset:0x%08x, BlkSize:0x%x\n", u4WritePoint, u4BlkSize);
        u4MemPtr += u4BlkSize;
        u4WritePoint += u4BlkSize;
        u4WroteSize += u4BlkSize;
    } while(u4WroteSize < u4Size);

    x_mem_free(pu4Buf);

    return u4WroteSize;
}

//---------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** LDR_DramParamsStore(): Try to store DRAM parameters if any
 *
 *  @retval 0               Success.
 *  @retval otherwise       Failed.
 */
//-----------------------------------------------------------------------------
UINT32 LDR_DramParamsStore()
{
    UINT32 u4UpgradeSize;

    // check DRAM tag first
    if( (*(UINT32 *)(DRAM_PARAMS_ADDR + 0)!= DRAMTAG_0) ||
        (*(UINT32 *)(DRAM_PARAMS_ADDR + 4) != DRAMTAG_1) ||
        (*(UINT32 *)(DRAM_PARAMS_ADDR + 8) != DRAMTAG_2) ||
        (*(UINT32 *)(DRAM_PARAMS_ADDR + 12) != DRAMTAG_3) )
    {
        Printf("\nNo valid DRAM tag in DRAM.\n");
        return 1;
    }

    // Determine the upgrade size
    u4UpgradeSize = _LdrBlockSize();
    if(u4UpgradeSize < PRELOADER_SIZE)
    {
        UINT32 u4Times = 0;
        u4Times = PRELOADER_SIZE / u4UpgradeSize;
        u4UpgradeSize = u4UpgradeSize * (u4Times + 1);
    }
    Printf("Upgrade size for DRAM parameters: 0x%x\n", u4UpgradeSize);

    // Copy the upgrade size to DRAM
    #if defined(CC_NAND_BOOT) || defined(CC_EMMC_BOOT) // NAND or EMMC
    CHIP_FlashCopyToDRAMLZHS(0, LOADER_DRAM_ADDR, 0, u4UpgradeSize);
    #else // NOR
    x_memcpy(LOADER_DRAM_ADDR, (const void *)(PBI_A_BASE), u4UpgradeSize);
    #endif

    // check if there is a DRAM tag
    if( (*(UINT32 *)(LOADER_DRAM_ADDR + DRAM_PARAMS_OFFSET + 0)!= DRAMTAG_0) ||
        (*(UINT32 *)(LOADER_DRAM_ADDR + DRAM_PARAMS_OFFSET + 4) != DRAMTAG_1) ||
        (*(UINT32 *)(LOADER_DRAM_ADDR + DRAM_PARAMS_OFFSET + 8) != DRAMTAG_2) ||
        (*(UINT32 *)(LOADER_DRAM_ADDR + DRAM_PARAMS_OFFSET + 12) != DRAMTAG_3) )
    {
        Printf("\nNo valid DRAM tag in flash.\n");
        return 1;
    }

    // replace the new DRAM parameters
    x_memcpy(LOADER_DRAM_ADDR + DRAM_PARAMS_OFFSET, DRAM_PARAMS_ADDR, DRAM_PARAMS_SIZE * 4);

    // write the new parameters in flash
    #if defined(CC_NAND_BOOT)
    if (NAND_BDM_WriteBlocks(0, 0, LOADER_DRAM_ADDR, u4UpgradeSize) != 0)
    #elif defined(CC_EMMC_BOOT)
    if (Loader_WriteMsdc(0, 0, LOADER_DRAM_ADDR, u4UpgradeSize) != u4UpgradeSize)
    #else
    if (Loader_WriteNorFlash(0, 0, LOADER_DRAM_ADDR, u4UpgradeSize) != u4UpgradeSize)
    #endif
    {
        Printf("Fail to write DRAM parameters!\n");
        while(1);
    }

    Printf("\n. Saving DRAM parameters - OK!\n");

    return 0;
}
#endif // CC_DRAM_TUNING
