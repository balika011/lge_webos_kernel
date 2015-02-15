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
#include "x_debug.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_printf.h"
#include "x_chip_id.h"
#include "x_gpio.h"
#include "x_timer.h"
#include "x_hal_926.h"
#include "x_os.h"
#include "x_hal_5381.h"
#include "x_ldr_env.h"

#if !defined(CC_MTK_LOADER) && defined(CC_TRUSTZONE_SUPPORT)
#include "tz_if.h"
#endif

#ifndef CC_MTK_PRELOADER

#define REG_EFUSE_SID_0     (0x678)
#define REG_EFUSE_SID_1     (0x67c)
#define REG_EFUSE_SID_2     (0x680)
#define REG_EFUSE_SID_3     (0x684)
#define SECURE_ID_LEN       (16)

#define REG_EFUSE_KEY       (0x688)

#define REG_EFUSE_DATA_0    (0x68C)
#define REG_EFUSE_DATA_1    (0x690)
#define DEVICE_ID_LEN       (8)

#define REG_EFUSE_SDATA_0   (0x694)
#define REG_EFUSE_SDATA_1   (0x698)
#define REG_EFUSE_SDATA_2   (0x69c)
#define REG_EFUSE_SDATA_3   (0x6a0)

#define DEVICE_DATA_LEN     (128)

INT32 CmdEfusePowerPinOn(INT32 i4Argc, const CHAR** aszArgv)
{
    return 0;
}

INT32 CmdEfusePowerPinOff(INT32 i4Argc, const CHAR** aszArgv)
{
    return 0;
}


INT32 CmdEfuseDump(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 au4Array[17];
    UINT32 i = 0;

    //Read 0x8660 ~ 0x8674
    for (i = 0; i < 6; i++)
    {
        au4Array[i] = BIM_READ32(0x660 + i * 4);
    }

    au4Array[10] = BIM_READ32(REG_EFUSE_KEY);
    au4Array[11] = BIM_READ32(REG_EFUSE_DATA_0);
    au4Array[12] = BIM_READ32(REG_EFUSE_DATA_1);

    BIM_WRITE32(REG_EFUSE_KEY, 0x883);
    BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

    au4Array[7] = BIM_READ32(REG_EFUSE_SID_1);
    au4Array[9] = BIM_READ32(REG_EFUSE_SID_3);
    au4Array[6] = BIM_READ32(REG_EFUSE_SID_0);
    au4Array[8] = BIM_READ32(REG_EFUSE_SID_2);

    BIM_WRITE32(REG_EFUSE_KEY, 0x883);
    BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

    au4Array[14] = BIM_READ32(REG_EFUSE_SDATA_1);
    au4Array[16] = BIM_READ32(REG_EFUSE_SDATA_3);
    au4Array[13] = BIM_READ32(REG_EFUSE_SDATA_0);
    au4Array[15] = BIM_READ32(REG_EFUSE_SDATA_2);

    for (i = 0; i < 17;)
    {
        Printf("0x%08X  ", au4Array[i++]);
        if (i % 4 == 0)
        {
            Printf("\n");
        }
    }

    Printf("IS_SupportPlayRDY():%d\n", IS_SupportPlayRDY());
    Printf("IS_SupportH264():%d\n", IS_SupportH264());
    Printf("IS_SupportAndroid:%d\n", IS_SupportAndroid());
    Printf("IS_Support60MJC:%d\n", IS_Support60MJC());
    Printf("IS_SupportTsIn:%d\n", IS_SupportTsIn());
    Printf("IS_SupportDimming:%d\n", IS_SupportDimming());
    Printf("IS_Support120HZ:%d\n", IS_Support120HZ());
    Printf("IS_Support3D:%d\n", IS_Support3D());
    Printf("IS_SupportDVB:%d\n", IS_SupportDVB());
    Printf("IS_SupportBRAZIL:%d\n", IS_SupportBRAZIL());
    Printf("IS_SupportATSC:%d\n", IS_SupportATSC());
    Printf("IS_SupportDMBT:%d\n", IS_SupportDMBT());
    Printf("IS_SupportH264EN():%d\n", IS_SupportH264EN());
    Printf("IS_SupportTCON():%d\n", IS_SupportTCON());
    Printf("IS_SupportMHEGHD():%d\n", IS_SupportMHEGHD());
    Printf("IS_SupportOPERA():%d\n", IS_SupportOPERA());
    Printf("IS_SupportDRA():%d\n", IS_SupportDRA());
    Printf("IS_SupportJVM():%d\n", IS_SupportJVM());
    Printf("IS_SupportARIB():%d\n", IS_SupportARIB());
    Printf("IS_SupportGINGA():%d\n", IS_SupportGINGA());
    Printf("IS_SupportMHP():%d\n", IS_SupportMHP());
    Printf("IS_SupportHBBTV():%d\n", IS_SupportHBBTV());
    Printf("IS_SupportRM():%d\n", IS_SupportRM());
    Printf("IS_SupportAVS():%d\n", IS_SupportAVS());
    Printf("IS_SupportVC1():%d\n", IS_SupportVC1());
    Printf("IS_SupportDivXHD():%d\n", IS_SupportDivXHD());
    Printf("IS_SupportMSS():%d\n", IS_SupportMSS());
    Printf("IS_Support3DGPU():%d\n", IS_Support3DGPU());
    Printf("IS_SupportDTS():%d\n", IS_SupportDTS());
    Printf("IS_SupportDOLBY():%d\n", IS_SupportDOLBY());

    return 0;
}


INT32 BIM_IsSecureBoot(void)
{
    CKGEN_WRITE32(0x10, 0xff00);

    if ((CKGEN_READ32(0x0) & 0x800) == 0x800)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#endif //#ifndef CC_MTK_PRELOADER

#ifdef CC_SECURE_BOOT

#define REG_SDATA1          (0x664)
#define KEY_INDEX_EFUSE     ((BIM_READ32(REG_SDATA1) >> 17) & 0x3)
#define MTK_EFUSE_PUBLIC_KEY   (UINT32 *)(0xf4003c00 + KEY_INDEX_EFUSE * 256)

extern UINT32 modMultTemp(UINT32 a[], const UINT32 x[], const UINT32 y[], UINT32 m[], UINT32 ndigits, UINT32 temp[], UINT32 tqq[], UINT32 trr[]);
typedef UINT32 (*PFN_MODMULTTEMP)(UINT32 a[], const UINT32 x[], const UINT32 y[], UINT32 m[], UINT32 ndigits, UINT32 temp[], UINT32 tqq[], UINT32 trr[]);

INT32 RSADecryption(UINT32 *pu1Signature, UINT32 *pu4PublicKey, UINT32 *pu4CheckSum, UINT32 e)
{
    // m = s ^ 5 mod n
    UINT32 y_2048[64];
    UINT32 t1_4096[128];
    UINT32 t2_4096[128];
    UINT32 t3_4096[128];

#if defined(CC_ROM_BOOT) && defined(__GCC__)
    // The RSA function is from romcode.
    // Address can get from \project_x\drv_build\mtxxxx\rom_code\mtxxxx_rom_code.sym
    // 0x00001844 A modMultTemp -> 0xf4001844, 0xf4000000 is a ROM code base address
    PFN_MODMULTTEMP pfn_modMultTemp;

    if (BSP_GetIcVersion() == IC_VER_5399_AA)
    {
        pfn_modMultTemp = (PFN_MODMULTTEMP)0xf4001604;
    }
    else
    {
        // fix me when at-speed ROM code done
        pfn_modMultTemp = (PFN_MODMULTTEMP)0xf40011cb;
    }
#endif

    //y = s * s mod n
#if defined(CC_ROM_BOOT) && defined(__GCC__)
    pfn_modMultTemp(y_2048, pu1Signature, pu1Signature, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
#else
    modMultTemp(y_2048, pu1Signature, pu1Signature, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
#endif

	// y = y * y mode n
	if(e == 5)
	{
		#if defined(CC_ROM_BOOT) && defined(__GCC__)
		pfn_modMultTemp(y_2048, y_2048, y_2048, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
		#else
		modMultTemp(y_2048, y_2048, y_2048, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
		#endif
	}

    //y = y * s mode n
#if defined(CC_ROM_BOOT) && defined(__GCC__)
    pfn_modMultTemp(y_2048, y_2048, pu1Signature, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
#else
    modMultTemp(y_2048, y_2048, pu1Signature, pu4PublicKey, 64, t1_4096, t2_4096, t3_4096);
#endif

    x_memcpy( pu4CheckSum, y_2048, 256 );
    return 0;
}

#ifndef NDEBUG //secure boot must define NDEBUG
static INT32 CmdEfuseUnlockJtag(INT32 i4Argc, const CHAR** aszArgv)
{
    IO_WRITE32MSK(BIM_BASE, 0x2c, 0x42000000, 0x42000000);

    return 0;
}
#endif

BOOL BIM_VerifySignature(UINT32* pu4Signature, UINT32* pu4CheckSum)
{
    UINT32 au4CalulatedCheckSum[64]; // 2048bit key
    UINT32 au4PublicKey[64]; // 2048bit key
    UINT32 au4VendorKey[64]; // 2048bit key
    LDR_ENV_T* prLdrEnv = (LDR_ENV_T*)CC_LDR_ENV_OFFSET;
	UINT32 u4e = 3;

#ifndef NDEBUG //secure boot must define NDEBUG
    CmdEfuseUnlockJtag(0, NULL);
#endif

    x_memcpy((void*)au4PublicKey, (void *)MTK_EFUSE_PUBLIC_KEY, 256);


    if((KEY_INDEX_EFUSE == 0) || (KEY_INDEX_EFUSE == 1))
    {
    	u4e = 3;
    }
    else
    {
   		u4e = 5;
    }

#ifdef CC_MTK_PRELOADER
    RSADecryption(pu4Signature+64, au4PublicKey, au4VendorKey, u4e);
    x_memcpy((void*)prLdrEnv->au4CustKey, (void*)au4VendorKey, sizeof(prLdrEnv->au4CustKey));
#else
    x_memcpy((void*)au4VendorKey, (void*)prLdrEnv->au4CustKey, sizeof(prLdrEnv->au4CustKey));
    x_memcpy((void*)SRAM_DMX_BASE, (void*)CC_LDR_ENV_OFFSET, 0x1000);
#endif //CC_MTK_PRELOADER

    RSADecryption(pu4Signature, au4VendorKey, au4CalulatedCheckSum, u4e);

    // use 255 to ignore the last byte(value=0)
    return x_memcmp((void*)au4CalulatedCheckSum, (void*)pu4CheckSum, 255);
}
#endif
