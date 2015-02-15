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
#include "x_typedef.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_chip_id.h"
#include "x_gpio.h"
#include "drvcust_if.h"
#include "x_os.h"

#define INLINE_CHIP_FUNC(func, statement)                                      \
                                                                               \
    UINT32 func()                                                              \
    {                                                                          \
        return statement;                                                      \
    }


//#ifdef CC_ALL_BOND_ENABLE
//INLINE_CHIP_FUNC(GET_EFUSE_CHIPID, 0)
//#else /* CC_ALL_BOND_ENABLE */
INLINE_CHIP_FUNC(GET_EFUSE_CHIPID, (BIM_READ32(REG_EFUSE_BONDING) & BONDING_MASK))
//#endif /* CC_ALL_BOND_ENABLE */

INLINE_CHIP_FUNC(_IS_RDES, (GET_EFUSE_CHIPID() == 0))
INLINE_CHIP_FUNC(IS_SupportPlayRDY, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_PLAYRDY_BIT) == BOND_PLAYRDY_BIT))
INLINE_CHIP_FUNC(IS_SupportH264, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_H264_BIT) == BOND_H264_BIT))
INLINE_CHIP_FUNC(IS_SupportAndroid, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_ANDRIOD_BIT) == BOND_ANDRIOD_BIT))
INLINE_CHIP_FUNC(IS_Support60MJC, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_60MJC_BIT) == BOND_60MJC_BIT))
INLINE_CHIP_FUNC(IS_SupportTsIn, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_TSIN_BIT) == BOND_TSIN_BIT))
INLINE_CHIP_FUNC(IS_SupportDimming, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DIMMING_BIT) == BOND_DIMMING_BIT))
INLINE_CHIP_FUNC(IS_Support120HZ, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_120HZ_BIT) == BOND_120HZ_BIT))
INLINE_CHIP_FUNC(IS_Support3D, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_3D_BIT) == BOND_3D_BIT))
INLINE_CHIP_FUNC(IS_SupportDVB, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DVB_BIT) == BOND_DVB_BIT))
INLINE_CHIP_FUNC(IS_SupportBRAZIL, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_BRAZIL_BIT) == BOND_BRAZIL_BIT))
INLINE_CHIP_FUNC(IS_SupportATSC, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_ATSC_BIT) == BOND_ATSC_BIT))
INLINE_CHIP_FUNC(IS_SupportDMBT, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DMBT_BIT) == BOND_DMBT_BIT))
INLINE_CHIP_FUNC(IS_SupportH264EN, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_H264EN_BIT) == BOND_H264EN_BIT))
INLINE_CHIP_FUNC(IS_SupportTCON, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_TCON_BIT) == BOND_TCON_BIT))
INLINE_CHIP_FUNC(IS_SupportMHEGHD, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_MHEGHD_BIT) == BOND_MHEGHD_BIT))
INLINE_CHIP_FUNC(IS_SupportOPERA, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_OPERA_BIT) == BOND_OPERA_BIT))
INLINE_CHIP_FUNC(IS_SupportDRA, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DRA_BIT) == BOND_DRA_BIT))
INLINE_CHIP_FUNC(IS_SupportARIB, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_ARIB_BIT) == BOND_ARIB_BIT))
INLINE_CHIP_FUNC(IS_SupportGINGA, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_GINGA_BIT) == BOND_GINGA_BIT))
INLINE_CHIP_FUNC(IS_SupportMHP, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_MHP_BIT) == BOND_MHP_BIT))
INLINE_CHIP_FUNC(IS_SupportHBBTV, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_HBBTV_BIT) == BOND_HBBTV_BIT))
INLINE_CHIP_FUNC(IS_SupportRM, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_RM_BIT) == BOND_RM_BIT))
INLINE_CHIP_FUNC(IS_SupportAVS, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_AVS_BIT) == BOND_AVS_BIT))
INLINE_CHIP_FUNC(IS_SupportVC1, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_VC1_BIT) == BOND_VC1_BIT))
INLINE_CHIP_FUNC(IS_SupportDivXHD, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DIVXHD_BIT) == BOND_DIVXHD_BIT))
INLINE_CHIP_FUNC(IS_SupportLowSpeedCPU, ((GET_EFUSE_CHIPID() & BOND_LOWSPEEDCPU_BIT) == BOND_LOWSPEEDCPU_BIT))
INLINE_CHIP_FUNC(IS_Support3DGPU, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_3DGPU_BIT) == BOND_3DGPU_BIT))
INLINE_CHIP_FUNC(IS_SupportDTS, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DTS_BIT) == BOND_DTS_BIT))
INLINE_CHIP_FUNC(IS_SupportDOLBY, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_DOLBY_BIT) == BOND_DOLBY_BIT))
INLINE_CHIP_FUNC(IS_SupportIPTV, ((GET_EFUSE_CHIPID() & BOND_IPTV_BIT) == BOND_IPTV_ENABLE))
INLINE_CHIP_FUNC(IS_SupportMSS, _IS_RDES() || ((GET_EFUSE_CHIPID() & BOND_MSS_BIT) == BOND_MSS_BIT))

INLINE_CHIP_FUNC(IS_SupportEAC3, 1)
INLINE_CHIP_FUNC(IS_SupportDivX, 1)
INLINE_CHIP_FUNC(IS_SupportAC3, 1)
INLINE_CHIP_FUNC(IS_SupportDDCO, 1)
INLINE_CHIP_FUNC(IS_SupportMS10, 1)
INLINE_CHIP_FUNC(IS_SupportCIPlus, 1)
INLINE_CHIP_FUNC(IS_MultiMediaSupport, 1)
INLINE_CHIP_FUNC(IS_MHEG5Support, 1)
INLINE_CHIP_FUNC(IS_SupportCHINATV, 1)
INLINE_CHIP_FUNC(IS_SupportQJY, 1)
INLINE_CHIP_FUNC(IS_SupportCINEMA, 1)
//INLINE_CHIP_FUNC(IS_SupportNETWORK, 1)
INLINE_CHIP_FUNC(IS_SupportVP8, 1)
INLINE_CHIP_FUNC(IS_SupportNETWORK, 1)
INLINE_CHIP_FUNC(IS_SupportCPU1G, 0)
INLINE_CHIP_FUNC(IS_SupportJVM, 0)

VOID CHIP_CheckBoard(VOID)
{
}

#ifdef SUPPORT_EFUSE_CLI
static void vWriteEfuseBit(UINT32 bit)
{
	#ifndef CC_MTK_PRELOADER
    //printf("bit:%d\n", bit);
    IO_WRITE32(0, IO_VIRT + 0x86b4,0x80cd9000 + bit);
    
    IO_WRITE32(0, IO_VIRT + 0x86b0, 0x1);
    x_thread_delay(1);
    IO_WRITE32(0, IO_VIRT + 0x86b0, 0x11);
    x_thread_delay(1);
    while((IO_READ32(0, IO_VIRT + 0x86b0) & 0x10) == 0x10);
    x_thread_delay(1);
	#endif
}

void vWriteEfuseBits(UINT32 u4PassKey, UINT32 u4_8670, UINT32 u4_8664)
{
	#ifndef CC_MTK_PRELOADER
    UINT32 bit;
    
    IO_WRITE32(0, IO_VIRT + 0x8688, u4PassKey);

    for (bit=0; bit<32; bit++)
    {
        if (u4_8670 & (1L << bit))
        {
            vWriteEfuseBit(bit + 0x80);
            x_thread_delay(1);
        }

    }

    for (bit=0; bit<32; bit++)
    {
        if (u4_8664 & (1L << bit))
        {
            vWriteEfuseBit(bit + 0x20);
            x_thread_delay(1);
        }
    }
    
    IO_WRITE32(0, IO_VIRT + 0x8688, 0x0);
	#endif
}
#endif


