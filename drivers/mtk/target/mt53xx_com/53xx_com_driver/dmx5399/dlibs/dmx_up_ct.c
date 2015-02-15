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
 * $RCSfile: dmx_init.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_init.c
 *  Demux driver - initialization
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx.h"
#include "dmx_mm.h"
#include "dmx_drm_drvif.h"
#include "dmx_pcr.h"
#include "x_chip_id.h"

#undef CC_DMX_USE_CT
#ifndef DMX_NT_UCODE_DISABLE
#define CC_DMX_USE_CT
#endif  // DMX_NT_UCODE_DISABLE

#ifdef CC_DMX_EMULATION
#undef CC_DMX_USE_CT
#endif

#include "dmx_imem.h"

#include "dmx_imem_test.h"
#include "dmx_debug.h"
#include "x_lint.h"

#include "fvr.h"

#include "gcpu_if.h"
#include "drvcust_if.h"
LINT_EXT_HEADER_BEGIN

#include "x_pinmux.h"
#include "x_ckgen.h"
#include "x_assert.h"
#include "x_hal_5381.h"
#include "x_util.h"
#include "x_os.h"
#include "x_timer.h"
#include "x_bim.h"

LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

// Define EXT_TS2 to select the second external TS interface. By default the
// first external TS interface is selected
//
//#define EXT_TS2


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define NUM_UP_TEST_RESULT_WORDS            16
#define DMX_MAX_DBM_WAIT_COUNT              10

#ifdef CC_DMX_USE_CT
// Prevent this PID value from being set into the hardware PID filter.
#define BPS_PID                             0x1FFB

// The flag "ENABLE_0X1FFB_FILTERING" is no longer needed on MT5387/5388.
// The uP is told to perform 0x1FFB through the Byte 3 of CT_SETTING.
#endif  // CC_DMX_USE_CT
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
UINT8 au1MtkSecretKey[32] = {0x09, 0xF8, 0x2D, 0xCF, 0x62, 0xCA, 0x2D, 0x9C, 0x0F, 0x9B, 0x01, 0x2E, 0xB3, 0x96, 0x67, 0xDE,
						0x72, 0x30, 0xBA, 0x44, 0x21, 0x84, 0x98, 0xEF, 0x66, 0x49, 0xF8, 0xCD, 0x18, 0xBE, 0x6C, 0xCD};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef CC_DMX_USE_CT
//-----------------------------------------------------------------------------
/** _Dmx_GetBypassPidRegWordOffset
 *  Get the word address of the Bypass PID Register.
 *
 *  @param  u1IsIndex       TS index
 *
 *  @retval                 the word offset from the DMUX0 (0x20017000).
 */
//-----------------------------------------------------------------------------
static UINT32 _Dmx_GetBypassPidRegWordOffset(UINT8 u1TsIndex)
{
    UINT32 u4Register = DMX_REG_DBM_BYPASS_PID;

    switch (u1TsIndex)
    {
    case 0:
        u4Register = DMX_REG_DBM_BYPASS_PID;
        break;
    case 1:
        u4Register = DMX_REG_DBM_BYPASS_PID_2;
        break;
    case 2:
        u4Register = DMX_REG_DBM_BYPASS_PID_3;
        break;
    case 3:
        u4Register = DMX_REG_DBM_BYPASS_PID_4;
        break;
    default:
        LOG(1, "Unknown TS_index (%u)!\n", u1TsIndex);
        ASSERT(0);
        break;
    }

    return u4Register;
}
#endif  // CC_DMX_USE_CT


//-----------------------------------------------------------------------------
/** _DMX_CTInit
 *  Soft-initialize CT setting
 */
//-----------------------------------------------------------------------------
BOOL _DMX_CTInit(void)
{
    if (GetTargetCountry() != COUNTRY_US)
    {
#if defined(CC_DMX_USE_CT) && !defined(CC_DRIVER_BUILD)
        LOG(0, "You should define DMX_NT_UCODE_DISABLE in XXX.cfg file for Non-US model!\n");
        ASSERT(0);
#endif
        CT_TSIDX_SETTING = 0xFFFFFFFF;
        CT_SETTING = 0x0;
    }
    else
    {
        // Set the byte 3 of CT_SETTING to 0xFF which represents an invalid
        // PID Index.  Other values greater than 31 will do, but I simply use 0xFF.
        if ((BIM_READ32(0x664) & 0x4) == 0x4)
        {
            // Demod does not clear VCT.  DMX has the choice to clear VCT or not.
#ifdef CC_DMX_USE_CT
            CT_SETTING = 0xFF000002;  // DMX clears VCT.
#else
            CT_SETTING = 0xFF000000;  // DMX does not clear VCT.
#endif  // CC_DMX_USE_CT
            CT_TSIDX_SETTING = 0xFFFFFFFF;
        }
        else
        {
            // Demod clears VCT.
            CT_SETTING = 0xFF000001;   // DMX must use the CRC result from Demod.
            CT_TSIDX_SETTING = 0xFFFFFFFF;
#ifndef CC_DMX_USE_CT
            LOG(0, "Don't define DMX_NT_UCODE_DISABLE in XXX.cfg file for US model!\n");
            ASSERT(0);  // There shall be no such usage scenario.
#endif  // CC_DMX_USE_CT
        }
    }

    return TRUE;
}


BOOL _DMX_CTSetPid(UINT8 u1Pidx, UINT32 u4Flags, PID_STRUCT_T *prPidStruct, const DMX_PID_T* prPid, UINT32 *pu4PidEntry)
{
    if (u4Flags == DMX_PID_FLAG_PID)
    {
#ifdef CC_DMX_USE_CT
        // When users want to set BPS_PID as the PID value, the PID value is
        // not set into the hardware "PID Filter".  Instead, the PID value in
        // the PID Filter is set to the PID value of a null packet (0x1FFF).
        // The software records such an event.
        if (prPid->u2Pid == BPS_PID)
        {
            *pu4PidEntry = (*pu4PidEntry & 0xe000ffff) | (0x1FFF << 16);
            prPidStruct->u2Pid = prPid->u2Pid;
            return TRUE;
        }
        else
#endif  // CC_DMX_USE_CT
        {
            return FALSE;
        }
    }
    else if (u4Flags == DMX_PID_FLAG_VALID)
    {
#ifdef CC_DMX_USE_CT
        //if((rPidStruct.u2Pid == BPS_PID) &&
        //   ((rPidStruct.eSource == DMX_SOURCE_DTV0) || (rPidStruct.eSource == DMX_SOURCE_DTV1)))
        if (prPidStruct->u2Pid == BPS_PID)
        {
            // Skip only DTV TS input
            return TRUE;
        }
        else
#endif  // CC_DMX_USE_CT
        {
            return FALSE;
        }
    }

    return TRUE;
}


BOOL _DMX_CTConfig(UINT8 u1Pidx, PID_STRUCT_T *prPidStruct, UINT32 *pu4PidEntry, BOOL fgEnable)
{
#ifdef CC_DMX_USE_CT
    if (prPidStruct->u2Pid == BPS_PID)
    {
        UINT8 u1TsIndex;
        UINT32 u4Register, u4Value;
        UINT32 u4CTMask;

        u1TsIndex = prPidStruct->u1TsIndex;
        u4Register = _Dmx_GetBypassPidRegWordOffset(u1TsIndex);

        if (fgEnable)
        {
            // Always turn off the Valid bit if PID value is BPS_PID.
            *pu4PidEntry &= 0x7FFFFFFF;
            // The "To Microprocessor" flag should have been set in u4PidEntry.
            u4Value = *pu4PidEntry & 0xFFFF;
            u4Value |= u1Pidx << 16;
            DMXCMD_WRITE32(u4Register, u4Value);

            //CT_SETTING = (CT_SETTING & 0x00FFFFFF) | ((UINT32)u1Pidx << 24);
            u4CTMask = (UINT32)0xFF << (u1TsIndex * 8);
            CT_TSIDX_SETTING = (CT_TSIDX_SETTING & ~u4CTMask) |
                               ((UINT32)u1Pidx << (u1TsIndex * 8));

            // Turn on the Bypass mode after setting the appropriate DBM_BYPASS_PID register.
            u4Value = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
            u4Value |= 1 << (26 + u1TsIndex);
            DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Value);
        }
        else
        {
            u4CTMask = (UINT32)0xFF << (u1TsIndex * 8);
            //CT_SETTING = (CT_SETTING & 0x00FFFFFF) | 0xFF000000;
            CT_TSIDX_SETTING = CT_TSIDX_SETTING | u4CTMask;

            u4Value = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
            u4Value &= ~(1 << (26 + u1TsIndex));
            DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Value);

            DMXCMD_WRITE32(u4Register, 0);
        }
    }
#endif  // CC_DMX_USE_CT

    return TRUE;
}


void _DMX_CTSetIndex(UINT8 u1Pidx, UINT8 u1TsIdx, DMX_PID_T* prPid)
{
#ifdef CC_DMX_USE_CT
    if (prPid->u2Pid == BPS_PID)
    {
        UINT32 u4CTMask;

        u4CTMask = (UINT32)0xFF << (prPid->u1TsIndex * 8);
        CT_TSIDX_SETTING |= u4CTMask;
        u4CTMask = (UINT32)0xFF << (u1TsIdx * 8);
        CT_TSIDX_SETTING = (CT_TSIDX_SETTING & ~u4CTMask) | ((UINT32)u1Pidx << (u1TsIdx * 8));
    }
#endif // CC_DMX_USE_CT
}


//-----------------------------------------------------------------------------
/** _DMX_uCodeHasAW
 *  Return TRUE if the TS uCode supports AW.  Otherwise, return FALSE.
*/
//-----------------------------------------------------------------------------
BOOL _DMX_uCodeHasAW(void)
{
#ifdef CC_DMX_USE_CT
    UINT16 u2CodeVersion;
    UINT8 u1Major, u1Minor;

    u2CodeVersion = (UINT16)(FTI_IMEM[0] & 0xFFFF);
    u1Major = (UINT8)((u2CodeVersion >> 8) & 0x0F);
    u1Minor = (UINT8)(u2CodeVersion & 0xFF);

    // On 5387/5388/5363, CT can be configured for TS uCode 3.24 and above.
    // Although 3.21 does support CT configuration, it does not support the
    // case where (CT_SETTING == 2).  Thus, 3.24 is a better option.
    // The 5388 uCode has been renamed to 4.25 since 3.25.
    if (((u1Major == 3) && (u1Minor == 24)) ||
            ((u1Major == 4) && (u1Minor >= 25)) ||
            ((u1Major == 5) && (u1Minor >= 40)) ||
            ((u1Major == 6) && (u1Minor >= 9))
       )
    {
        return TRUE;
    }
#endif  // CC_DMX_USE_CT

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _DMX_DriverHasAW
 *  Return TRUE if the driver supports AW.  Otherwise, return FALSE.
*/
//-----------------------------------------------------------------------------
BOOL _DMX_DriverHasAW(void)
{
#ifdef CC_DMX_USE_CT
    return TRUE;
#else
    return FALSE;
#endif  // CC_DMX_USE_CT
}


//-----------------------------------------------------------------------------
/** _DMX_LoaduCode
 *  Load default uCode
*/
//-----------------------------------------------------------------------------
BOOL _DMX_LoaduCode(void)
{
    if (!_DMX_LoadIMem(TRUE, FTI_IMEM, FTI_IMEM_LEN))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _DMX_LoaduTestCode
 *  Load test uCode
*/
//-----------------------------------------------------------------------------
BOOL _DMX_LoaduTestCode(void)
{
    if (!_DMX_LoadIMem(TRUE, FTI_IMEM_TEST_LATCH, FTI_IMEM_LEN_TEST_LATCH))
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DmxTestFTuPLatch
 *  Test uP latch-up
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_TestFTuPLatch(void)
{
    UINT32 i;
    BOOL fgRet;

    if (!_DMX_InitDiag())
    {
        return FALSE;
    }

    // Wait for a while
    x_thread_delay(100);

    // Check result
    fgRet = TRUE;
    for (i = 0; i < NUM_UP_TEST_RESULT_WORDS; i++)
    {
        if (PID_W(i) != _au4TestResult[i])
        {
            LOG(5, "uP latch test failed at word %u: expect = 0x%08x,"
                " read = 0x%08x\n", i, _au4TestResult[i], PID_W(i));
            fgRet = FALSE;
            break;
        }
    }

    // Restore normal uCode
    if (!_DMX_Init())
    {
        return FALSE;
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_Support_Tsin
 */
//-----------------------------------------------------------------------------
BOOL _DMX_Support_Tsin(void)
{
    return (BOOL)IS_SupportTsIn();
}

//-----------------------------------------------------------------------------
/** _DMX_GetMtdSecureKey
 */
//-----------------------------------------------------------------------------
BOOL _DMX_GetMtdSecureKey(UINT8 *pu1Key)
{
#ifndef CC_DMX_A1
			INT32 i;
#endif


    if (!pu1Key)
    {
        return FALSE;
    }
    
    x_memset((void*)pu1Key, 0, 16);

#ifdef CC_DMX_A1
    {
        #define REG_EFUSE_SID_0     (0x678)
        #define REG_EFUSE_SID_1     (0x67c)
        #define REG_EFUSE_SID_2     (0x680)
        #define REG_EFUSE_SID_3     (0x684)
        #define REG_EFUSE_KEY       (0x688)

        UINT32 au4SID[4] = {0};

        BIM_WRITE32(REG_EFUSE_KEY, 0x883);
        BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

        au4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
        au4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
        au4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
        au4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

        x_memcpy((void*)pu1Key, (void*)au4SID, 16);
    }
#else /*!CC_DMX_A1*/
    {
        x_memcpy(pu1Key, (void *)BIM_MTK_KEY_ADDR, 16);
        for(i = 0; i < 16; i++)
        {
            pu1Key[i] ^= 0x55;
        }
    }
#endif /*!CC_DMX_A1*/

    return TRUE;
}

typedef struct
{
    UINT16 u2KeyLen;
    UINT8 au1Key[32];
    UINT8 au1InitVector[16];
    UINT8 au1SampleId[8];
    BOOL fgCbc;
    DMX_DRM_MODE_T eMode;
} DRM_PARAM_T;

BOOL _MtkAesDecryption(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16], au1Save[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;
	DRM_PARAM_T rParam;
    UINT32 i;

	// setup parameter
	rParam.u2KeyLen = 256;
	rParam.fgCbc = TRUE;
	rParam.eMode = DMX_DRM_MODE_AES_SCTE52;

	for(i = 0; i < 32; i++)
	{
		rParam.au1Key[i] = au1MtkSecretKey[i];
	}

	for(i = 0; i < 16; i++)
	{
		rParam.au1InitVector[i] = i;
	}

    // Setup key scheduling
    aes_setkey_dec(&rCtx, rParam.au1Key, rParam.u2KeyLen);

    if (rParam.fgCbc)
    {
        memcpy(au1InitVector, rParam.au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text
		memcpy(au1Save, pu1SrcBuf + u4Offset, 16);

        // descrypt
        if (rParam.fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_DECRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_DECRYPT, au1Text, au1Text);
        }

        memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

	if((rParam.eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (rParam.fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = (UINT8)(u4Size - u4Offset);
        memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
		aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1Save, au1Text, au1Texto);
        memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
        memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    return TRUE;
}


BOOL _MtkAesEncryption(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16], au1Save[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;
	DRM_PARAM_T rParam;
    UINT32 i;

	// setup parameter
	rParam.u2KeyLen = 256;
	rParam.fgCbc = TRUE;
	rParam.eMode = DMX_DRM_MODE_AES_SCTE52;

	for(i = 0; i < 32; i++)
	{
		rParam.au1Key[i] = au1MtkSecretKey[i];
	}

	for(i = 0; i < 16; i++)
	{
		rParam.au1InitVector[i] = i;
	}

    // Setup key scheduling
    aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);

    if (rParam.fgCbc)
    {
        memcpy(au1InitVector, rParam.au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text
		memcpy(au1Save, pu1SrcBuf + u4Offset, 16);

        // descrypt
        if (rParam.fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_ENCRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        }

        memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

	if((rParam.eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (rParam.fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = (UINT8)(u4Size - u4Offset);
        memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
		aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1Save, au1Text, au1Texto);
        memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
        memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    return TRUE;
}


