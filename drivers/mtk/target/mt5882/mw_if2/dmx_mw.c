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
 * $RCSfile: dmx_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_mw.c
 *  Demux driver - middleware interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mtdmx_mwif.h"
#include "mtswdmx.h"
#include "dmx_mw.h"

#include "dmx_if.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#ifdef LINUX_TURNKEY_SOLUTION
#include "mpv_if.h"
#endif

#ifdef ENABLE_MULTIMEDIA
//#include "swdmx_if.h"
#endif
#include "aud_if.h"
//#include "aud_drvif.h"
//#include "vdec_drvif.h"
//#include "fbm_drvif.h"
#include "drv_comp_id.h"
#include "stc_drvif.h"
#include "x_assert.h"
#include "x_hal_5381.h"
#include "x_demux.h"
#include "x_rm_dev_types.h"
#include "x_os.h"
#include "u_drv_cust.h"

//#include "x_debug.h"
#define DEFINE_IS_LOG	DMX_IsLog
#include "mwif2_debug.h"

#include <pthread.h>


LINT_EXT_HEADER_END

#undef LOG
#define LOG(a, ...)

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define DMX_DEFAULT_ES_FIFO_SIZE            0x20000         // 128K
#define DMX_ES_FIFO_ALIGNMENT               32

#define DMX_CIPLUS_KEY_INDEX                (0)
#define DMX_DEFAULT_TS_INDEX                (0)

#if defined (CC_MT5890)
#define DMX_TIMESHIFT_TS_INDEX              (4)
#elif defined (CC_MT5882)
#define DMX_TIMESHIFT_TS_INDEX              (3)
#else
#define DMX_TIMESHIFT_TS_INDEX              (2)
#endif
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define DWORDSWAP(u4Tmp)        (((u4Tmp & 0xff) << 24) | ((u4Tmp & 0xff00) << 8) | ((u4Tmp & 0xff0000) >> 8) | ((u4Tmp & 0xff000000) >> 24))
#define DMX_CI_DES_KEY_SWAP_MODE    (1)
#define DMX_CI_AES_KEY_SWAP_MODE    (1)//(0)
#define DMX_CI_AES_IV_SWAP_MODE     (1)

//-----------------------------------------------------------------------------
// Import functions
//-----------------------------------------------------------------------------

/// Translate MPV component ID to channel ID
//EXTERN UCHAR MW_MPV_CompIdtoVldId(UINT16 u2CompId, UINT32 u4ChannelId); not used yet?
//#if defined(CC_VOMX_TV_COEXIST) && defined(CC_FBM_TWO_FBP)
#if defined(CC_3D_MM_DS_SUPPORT) || defined(CC_FBM_TWO_FBP)    /* all mpv */
EXTERN UCHAR MW_MPV_CompIdtoEsId(UINT16 u2CompId);
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

/// Notification callback functions
static DEMUX_NFY_INFO_T _arNotifyInfo[DMX_NUM_PID_INDEX];

/// Notification callback of scramble state
static DEMUX_NFY_INFO_T _arScrambleState[DMX_NUM_PID_INDEX];

//#ifdef ENABLE_MULTIMEDIA
static MW_DMX_PID_INFO_T _arDmxInfo[DMX_NUM_PID_INDEX];

#ifdef CC_53XX_SWDMX_V2
static UINT8 _u1SwdmxSrcId = 0;
#endif

//#endif  // ENABLE_MULTIMEDIA

static DEMUX_DEST_INFO_T _arDestInfo[DMX_NUM_PID_INDEX];

static pthread_mutex_t dmxmw_mutex;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef CI_PLUS_SUPPORT
#ifndef CC_CI_PLUS_TZ_SUPPORT
static BOOL DataSwap(UINT32 *pu4Dest, UINT32 *pu4Src, UINT32 u4Size, UINT32 u4Mode)
{
    INT32 i = 0;
    UINT32 u4Tmp;

    if(u4Mode == 0)
    {
        for(i =0; i < u4Size; i++) //memcpy
        {
            *(pu4Dest + i) = *(pu4Src + i);
        }
    }
    else if(u4Mode == 1) //Endien Change
    {
        for(i =0; i < u4Size; i++)
        {
            u4Tmp = *(pu4Src + i);
            u4Tmp = DWORDSWAP(u4Tmp);
            *(pu4Dest + i) = u4Tmp;
        }
    }
    else if(u4Mode == 2) //Head Swap
    {
        for(i =0; i < u4Size; i++)
        {
            *(pu4Dest + u4Size - 1 - i) = *(pu4Src + i);
        }
    }
    else if(u4Mode == 3) //Head Swap + Endien Change
    {
        for(i =0; i < u4Size; i++)
        {
            u4Tmp = *(pu4Src + i);
            u4Tmp = DWORDSWAP(u4Tmp);
            *(pu4Dest + u4Size - 1 - i) = u4Tmp;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}
#endif
#endif

//-----------------------------------------------------------------------------
/** _DmxMwCreateSemaphore
 */
//-----------------------------------------------------------------------------
static BOOL _DmxMwCreateSemaphore(pthread_mutex_t *mutex)
{
	if (pthread_mutex_init( mutex, NULL))
    {
	    LOG(0,"Mutex NOT created.\n" );
        return FALSE;
    }
	else
	{
        return TRUE;
	}
}

//-----------------------------------------------------------------------------
/** _DmxMwLockSemaphore
 */
//-----------------------------------------------------------------------------
static BOOL _DmxMwLockSemaphore(pthread_mutex_t *mutex)
{
    if (pthread_mutex_lock (mutex))
    {
	    LOG(0,"Mutex LOCK ERROR.\n" );
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
/** _MwDmxContypeToTsIdx
 *    Map eConType to TS index for general frontend case
 *    MW_DMX_CONN_TYPE_TUNER_0 -> 0
 *    MW_DMX_CONN_TYPE_TUNER_1 -> 1
 *    MW_DMX_CONN_TYPE_TUNER_2 -> 3
 *    MW_DMX_CONN_TYPE_BUFAGENT -> 3
 */
//-----------------------------------------------------------------------------
UINT8 _MwDmxContypeToTsIdx(MW_DMX_PID_CONTYPE_T eConType)
{
    UINT8 u1TsIdx;

    switch(eConType)
    {
    case MW_DMX_CONN_TYPE_TUNER_0:
        u1TsIdx = 0;
        break;
    case MW_DMX_CONN_TYPE_TUNER_1:
        u1TsIdx = 1;
        break;
    case MW_DMX_CONN_TYPE_TUNER_2:
    default:
        u1TsIdx = 3;
        break;
    }

    return u1TsIdx;
}


//-----------------------------------------------------------------------------
/** _DmxMwUnlockSemaphore
 */
//-----------------------------------------------------------------------------
static BOOL  _DmxMwUnlockSemaphore(pthread_mutex_t *mutex)
{
    if (pthread_mutex_unlock (mutex))
    {
	    LOG(0,"Mutex UnlOCK ERROR.\n" );
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
/** _MwDmxOnPsiNotify
 *  PSI notification handler
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
#ifndef MW_CLASSIC_DMX_NFY
#define PRE_READ_HDR_LEN    16
#endif

static BOOL _MwDmxOnPsiNotify(UINT8 u1Pidx, UINT32 u4Data, const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;
    MTDMX_NOTIFY_INFO_PSI_T* prNotifyInfo;
    UINT32 u4Data1;
    UINT8 u1SerialNumber;
    BOOL fgKeepData;
#ifndef MW_CLASSIC_DMX_NFY
    UINT8 au1Buf[PRE_READ_HDR_LEN] = {0};
    UINT64 u8_ext_data_1, u8_ext_data_2;
    UINT32 u4GetAddr;
#endif

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    _DmxMwLockSemaphore(&dmxmw_mutex);
    rInfo = _arNotifyInfo[u1Pidx];
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No PSI handler registered\n", u1Pidx);
        return FALSE;
    }

    prNotifyInfo = (MTDMX_NOTIFY_INFO_PSI_T*)u4Data;
    ASSERT(prNotifyInfo != NULL);

    u4GetAddr = prNotifyInfo->u4SecAddr;
    u1SerialNumber = prNotifyInfo->u1SerialNumber;
    ASSERT(u1SerialNumber <= DMX_MAX_SERIAL_NUM);
    u4Data1 = u4GetAddr;
    LOG(8,"%s,Sec Addr=0x%08X,Sec len=0x%x.\r\n",__FUNCTION__,prNotifyInfo->u4SecAddr,prNotifyInfo->u4SecLen);
#ifndef MW_CLASSIC_DMX_NFY
    fgKeepData = (MTDMX_PSI_GetBuffer(u1Pidx,
                                   u1SerialNumber,
                                   u4GetAddr,
                                   0/*skip size*/,
                                   (prNotifyInfo->u4SecLen < PRE_READ_HDR_LEN)?
                                    prNotifyInfo->u4SecLen : PRE_READ_HDR_LEN,
                                   (UCHAR*)&(au1Buf[0])) == MTR_OK)?TRUE:FALSE;
    if (!fgKeepData)
    {
        return FALSE;
    }

    u8_ext_data_1 = ((UINT64)(au1Buf[0])  << 56) | ((UINT64)(au1Buf[1])  << 48) |
                    ((UINT64)(au1Buf[2])  << 40) | ((UINT64)(au1Buf[3])  << 32) |
                    ((UINT64)(au1Buf[4])  << 24) | ((UINT64)(au1Buf[5])  << 16) |
                    ((UINT64)(au1Buf[6])  <<  8) | ((UINT64)(au1Buf[7]));
    u8_ext_data_2 = ((UINT64)(au1Buf[8])  << 56) | ((UINT64)(au1Buf[9])  << 48) |
                    ((UINT64)(au1Buf[10]) << 40) | ((UINT64)(au1Buf[11]) << 32) |
                    ((UINT64)(au1Buf[12]) << 24) | ((UINT64)(au1Buf[13]) << 16) |
                    ((UINT64)(au1Buf[14]) <<  8) | ((UINT64)(au1Buf[15]));
#endif

    fgKeepData = rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_DATA_REC,
        u4Data1, prNotifyInfo->u4SecLen, (UINT32)u1SerialNumber
    #ifndef MW_CLASSIC_DMX_NFY
        , u8_ext_data_1, u8_ext_data_2, 0
    #endif
        );

    UNUSED(pvTag);

    return fgKeepData;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnPesNotify
 *  PES notification handler
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnPesNotify(UINT8 u1Pidx, UINT32 u4Data, const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;
    MTDMX_NOTIFY_INFO_PES_T* prNotifyInfo;
    UINT32 u4DataAddr, u4Data1, u4Pts;
    UINT8 u1SerialNumber;
    BOOL fgKeepData;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    rInfo = _arNotifyInfo[u1Pidx];
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No PES handler registered\n", u1Pidx);
        return FALSE;
    }

    prNotifyInfo = (MTDMX_NOTIFY_INFO_PES_T*)u4Data;
    ASSERT(prNotifyInfo != NULL);

    u4DataAddr = prNotifyInfo->u4DataAddr;
    u1SerialNumber = prNotifyInfo->u1SerialNumber;
    ///play some pvr need audio pts
    #ifndef MW_CLASSIC_DMX_NFY 
    u4Pts = prNotifyInfo->u4Pts;
    #endif
    ASSERT(u1SerialNumber <= DMX_MAX_SERIAL_NUM);
    u4Data1 = u4DataAddr;
    LOG(8,"%s,Pes Addr=0x%08X,Sec len=0x%x.\r\n",__FUNCTION__,prNotifyInfo->u4DataAddr,prNotifyInfo->u4DataSize);
    fgKeepData = rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_DATA_REC,
        u4Data1, prNotifyInfo->u4DataSize, (UINT32)u1SerialNumber
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, u4Pts, 0
    #endif
        );

    UNUSED(pvTag);

    return fgKeepData;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnEsNotify
 *  ES notification handler
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnEsNotify(UINT8 u1Pidx, UINT32 u4Data, const void* pvTag)
{
    UNUSED(u1Pidx);
    UNUSED(u4Data);
    UNUSED(pvTag);

    // To be defined
    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnOverflowNotify
 *  PES notification handler
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnOverflowNotify(UINT8 u1Pidx, UINT32 u4Data,
    const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    rInfo = _arNotifyInfo[u1Pidx];
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No handler registered\n", u1Pidx);
        return FALSE;
    }
    LOG(8,"%s,Demux Send OverFlow to MW.\r\n",__FUNCTION__);
    UNUSED(rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_OVERFLOW, 0, 0, 0
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        ));

    UNUSED(u4Data);
    UNUSED(pvTag);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnScrambleStateNotify
 *  Scramble state notification handler
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnScrambleStateNotify(UINT8 u1Pidx, UINT32 u4Data,
    const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;
    MTDMX_SCRAMBLE_STATE_T eState;
    DEMUX_SCRS_T eMwState;
    BOOL fgRet;

    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    rInfo = _arScrambleState[u1Pidx];
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No scrabmle state handler registered\n", u1Pidx);
        return FALSE;
    }

    eState = (MTDMX_SCRAMBLE_STATE_T)u4Data;
    switch (eState)
    {
    case MTDMX_SCRAMBLE_STATE_CLEAR:
        eMwState = DEMUX_SCRS_CLEAR;
        break;

    case MTDMX_SCRAMBLE_STATE_SCRAMBLED:
        eMwState = DEMUX_SCRS_SCRAMBLED;
        break;

    default:
        eMwState = DEMUX_SCRS_UNKNOWN;
        break;
    }
    LOG(3, "[%s]Pidx %u: Scramble Status=%d.\n",__FUNCTION__,u1Pidx,eMwState);
    fgRet = rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_SCRS, (UINT32)eMwState, 0, 0
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        );

    // The middleware guarantees that it will always return FALSE
    VERIFY(!fgRet);

    UNUSED(pvTag);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxScrambleStateHandler
 *  Scramble-state handler
 *
 *  @param  u1Pidx          PID index
 *  @param  eCode           Notification code
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MwDmxScrambleStateHandler(UINT8 u1Pidx, MTDMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvTag)
{
    MT_RESULT_T rRet = MTR_NOT_OK;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    switch (eCode)
    {
    case MTDMX_NOTIFY_CODE_SCRAMBLE_STATE:
        rRet = (_MwDmxOnScrambleStateNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    default:
        // Unknown notification code
        LOG(1, "Pidx %u: Unknown notification code %u\n", u1Pidx,
            (UINT32)eCode);
        ASSERT(0);
        break;
    }

    return rRet;
}


//-----------------------------------------------------------------------------
/** _MwDmxSetScramblerCallback
 *  Set callback function for scramble state change
 *
 *  @param  u1Pidx          PID index
 *  @param  prNfyInfo       The callback function info
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxSetScrambleCallback(UINT8 u1Pidx,
    const DEMUX_NFY_INFO_T* prNfyInfo)
{
    MTDMX_PID_T rPid = {0};

    if ((u1Pidx >= DMX_NUM_PID_INDEX) || (prNfyInfo == NULL))
    {
        return FALSE;
    }

    // Set PSI callback function to our handler
    rPid.pfnScramble = _MwDmxScrambleStateHandler;
    rPid.pvScrambleTag = NULL;
    if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_SCRAMBLE_STATE, &rPid))
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arScrambleState[u1Pidx].pf_demux_nfy = prNfyInfo->pf_demux_nfy;
    _arScrambleState[u1Pidx].pv_tag = prNfyInfo->pv_tag;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxResetScrambleCallback
 *  Reset scramble callback handler
 *
 *  @param  u1Pidx          PID index
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxResetScrambleCallback(UINT8 u1Pidx)
{
    MTDMX_PID_T rPid = {0};

    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_SCRAMBLE_STATE, &rPid))
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arScrambleState[u1Pidx].pf_demux_nfy = NULL;
    _arScrambleState[u1Pidx].pv_tag = NULL;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxHandler
 *  Demux callback handler
 *
 *  @param  u1Pidx          PID index
 *  @param  eCode           Notification code
 *  @param  u4Data          Handler-specific data
 *  @param  pvTag           Handler-specific tag
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MwDmxHandler(UINT8 u1Pidx, MTDMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvTag)
{
    MT_RESULT_T rRet = MTR_NOT_OK;

    switch (eCode)
    {
    case MTDMX_NOTIFY_CODE_PSI:
        rRet = (_MwDmxOnPsiNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    case MTDMX_NOTIFY_CODE_PES:
        rRet = (_MwDmxOnPesNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    case MTDMX_NOTIFY_CODE_ES:
        rRet = (_MwDmxOnEsNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    case MTDMX_NOTIFY_CODE_OVERFLOW:
        rRet = (_MwDmxOnOverflowNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

//    case MTDMX_NOTIFY_CODE_SCRAMBLE_STATE:
//        fgRet = _OnScrambleStateNotify(u1Pidx, u4Data, pvTag);
//        break;

    default:
        // Unknown notification code
        LOG(1, "Pidx %u: Unknown notification code %u\n", u1Pidx,
            (UINT32)eCode);
        ASSERT(0);
        break;
    }

    return rRet;
}


//-----------------------------------------------------------------------------
/** _MX_DMX_SetCallback
 *  Set callback handler
 *
 *  @param  u1Pidx          PID index
 *  @param  prNfyInfo       Callback info
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_SetCallback(UINT8 u1Pidx, const DEMUX_NFY_INFO_T* prNfyInfo)
{
    MTDMX_PID_T rPid = {0};

    if ((u1Pidx >= DMX_NUM_PID_INDEX) || (prNfyInfo == NULL))
    {
        return FALSE;
    }

    #ifdef HYBRID_DRIVER_BASE
	if(MTDMX_RegCbFunc()!= MTR_OK)
    {
        printf("RegCbFunc fail\n");
        return FALSE;
    }
	#endif
	
    // Set PSI callback function to our handler
    rPid.pfnNotify = _MwDmxHandler;
    rPid.pvNotifyTag = NULL;
    if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_CALLBACK, &rPid))
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arNotifyInfo[u1Pidx].pf_demux_nfy = prNfyInfo->pf_demux_nfy;
    _arNotifyInfo[u1Pidx].pv_tag = prNfyInfo->pv_tag;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_ResetCallback
 *  Reset callback handler
 *
 *  @param  u1Pidx          PID index
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_ResetCallback(UINT8 u1Pidx)
{
    MTDMX_PID_T rPid = {0};

    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_CALLBACK, &rPid))
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arNotifyInfo[u1Pidx].pf_demux_nfy = NULL;
    _arNotifyInfo[u1Pidx].pv_tag = NULL;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxStcRemap
 *  Remap STC ID
 *
 *  @param  u1StcId     STC ID
 *
 *  @retval The STC ID
 */
//-----------------------------------------------------------------------------
static UINT8 _MwDmxStcRemap(UINT8 u1StcId)
{
#if 0 //def CC_SUPPORT_2STC
    // demux device id 1 -> PCR component id STC_SRC_A1
    // demux device id 0 -> PCR component id STC_SRC_A2
    switch (u1StcId)
    {
    case STC_SRC_A1:
        u1StcId = STC_SRC_A2;
        break;

    case STC_SRC_A2:
        u1StcId = STC_SRC_A1;
        break;

    default:
        u1StcId = STC_SRC_NS;
        break;
    }
#endif

    return u1StcId;
}


//-----------------------------------------------------------------------------
/** _MwDmxGetStcId
 *  Get STC ID
 *
 *  @param  u1Pidx     u1Pidx
 *
 *  @retval The STC ID
 */
//-----------------------------------------------------------------------------
UINT8 _MwDmxGetStcIdByPcrCompId(UINT8 u1PcrCompId)
{
    UINT8 u1StcId;

    switch (u1PcrCompId)
    {
    case PCR_COMP_1:
        u1StcId = STC_SRC_A1;
        break;

    case PCR_COMP_2:
#ifdef CC_SUPPORT_2STC
        u1StcId = STC_SRC_A2;
#else
        /* PCR ID is mapped to STC_SRC_A1 */
        u1StcId = STC_SRC_A1;
#endif
        break;

    default:
        ///TODO: in mulit-instance how to control STC?
        u1StcId = STC_SRC_A1;
        break;
    }

    UNUSED(_MwDmxStcRemap);

    return u1StcId;
}
#ifdef CC_SUPPORT_2STC
static UINT8 _MwDmxGetStcId(UINT8 u1Pidx)
{
    UINT8 u1StcId;
    MTDMX_PID_T rPid;

    x_memset(&rPid, 0, sizeof(MTDMX_PID_T));
    // Check if PID type is none (used to detect scramble state)
    if (MTDMX_GetPid(u1Pidx, DMX_PID_FLAG_DEVICE_ID, &rPid) != MTR_OK)
    {
        LOG(5, "Fail to DMX_GetPid\n");
        return STC_SRC_A1;
    }
    u1StcId = rPid.u1DeviceId;

    if(u1StcId > STC_SRC_A2)
    {
        printf("u1StcId: %d is over..", u1StcId);
        u1StcId = STC_SRC_A1;
    }

    return u1StcId;
}
#else
static UINT8 _MwDmxGetStcId(UINT8 u1PcrCompId)
{
    return _MwDmxGetStcIdByPcrCompId(u1PcrCompId);
}
#endif


//-----------------------------------------------------------------------------
/** _MwDmxOnSetPid
 *  Event handler of setting PID
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnSetPid(const DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info, UINT8 u1Pidx)
{
    MTDMX_PID_T rPid = {0};
    INT32 i4Ret = RMR_OK;
    UINT16 u2Pid;
    UINT8 u1PidIndex;
    MW_DMX_PID_INFO_T rPidInfo;

    ASSERT(pt_comp_id != NULL);

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1PidIndex = (UINT8)pt_comp_id->ui2_id;
    u2Pid = (UINT16)(MPEG_2_PID_T)(UINT32)pv_set_info;

    LOG(6, "Pidx: %u, pid: %u\n", u1PidIndex, u2Pid);

    _DmxMwLockSemaphore(&dmxmw_mutex);
    rPidInfo = _arDmxInfo[u1Pidx];
    _DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (!rPidInfo.fgValid)
    {
        // set pid before connect, so return false
        printf("%s set pid before connect\n", __FUNCTION__);
        return RMR_DRV_INV_SET_INFO;
    }

    switch (pt_comp_id->e_type)
    {
    case DRVT_DEMUX_TS_PCR:
        // No another PES entry with the same PID
        // Append PID
        if(u1Pidx != u1PidIndex)
        {
          ASSERT(0);
        }
        
        LOG(6,"%s,Connect to DRVT_DEMUX_TS_PCR:u1Pidx=%u.\r\n",__FUNCTION__,u1Pidx);
        if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_TUNER_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_TUNER_MAX))
        {
             if (MTDMX_SetPcrPidEx(rPidInfo.u1TsIndex, u1Pidx, _MwDmxGetStcId(u1Pidx), u2Pid) != MTR_OK)
            {
                return RMR_DRV_CONN_FAILED;
            }
        }
        else if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
        {
            if (MTDMX_SetPcrPidEx(DMX_TIMESHIFT_TS_INDEX, u1Pidx, _MwDmxGetStcId(u1Pidx), u2Pid) != MTR_OK)
            {
                LOG(0,"%s,For Timeshift,Set PCR_PID[u1Pidx=%u] error.\r\n",__FUNCTION__,u1Pidx);
                return RMR_DRV_INV_SET_INFO;
            }
        }
        else
        {
            printf("%s Connect Type Error!\n", __FUNCTION__);
            return RMR_DRV_INV_SET_INFO;
        }
        break;

    case DRVT_DEMUX_TS_PES_PACKET:
        rPid.u2Pid = u2Pid;

		_DmxMwLockSemaphore(&dmxmw_mutex);
        _arDestInfo[u1Pidx].ui2_id = u2Pid;
		_DmxMwUnlockSemaphore(&dmxmw_mutex);
        LOG(6,"%s,Connect to DRVT_DEMUX_TS_PES_PACKET,Set u1Pidx=%u,pid=0x%x.\r\n",__FUNCTION__,u1Pidx,u2Pid);
        if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_PID, &rPid))
        {
            i4Ret = RMR_DRV_INV_SET_INFO;
        }
        break;

    default:
        i4Ret = RMR_DRV_INV_SET_INFO;
        break;
    }

    return i4Ret;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnEnablePid
 *  Event handler of enabling/disabling PID
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnEnablePid(const DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info, UINT8 u1Pidx)
{
    MTDMX_PID_T rPid = {0};
    INT32 i4Ret = RMR_OK;
    BOOL fgEnable;

    ASSERT(pt_comp_id != NULL);

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    fgEnable = (BOOL)(UINT32)pv_set_info;

    if (fgEnable)
    {
        if (pt_comp_id->e_type == DRVT_DEMUX_TS_PES_PACKET)
        {
            // Check if PID type is none (used to detect scramble state)
            if (MTDMX_GetPid(u1Pidx, MTDMX_PID_FLAG_BUFFER, &rPid) != MTR_OK)
            {
                return RMR_DRV_INV_SET_INFO;
            }

            if (rPid.ePidType == MTDMX_PID_TYPE_NONE)
            {
                // PID type is none, allocate a default buffer
                rPid.fgAllocateBuffer = TRUE;
                rPid.u4BufSize = DMX_DEFAULT_ES_FIFO_SIZE;
                rPid.ePidType = MTDMX_PID_TYPE_ES_OTHER;
                if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_BUFFER, &rPid))
                {
                    return RMR_DRV_INV_SET_INFO;
                }
            }
        }
//#ifdef ENABLE_MULTIMEDIA
        else if(pt_comp_id->e_type == DRVT_DEMUX_TS_PCR)
        {
            MW_DMX_PID_CONTYPE_T eType;

			_DmxMwLockSemaphore(&dmxmw_mutex);
            eType = _arDmxInfo[u1Pidx].eConType;
			_DmxMwUnlockSemaphore(&dmxmw_mutex);

            if((eType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && (eType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
            {
#ifndef CC_53XX_SWDMX_V2
                UNUSED(MTSWDMX_SetInfo((UINT8)MTSWDMX_ENABLE_STREAM, (UINT32)MTSWDMX_STRM_TYPE_PCR, 0, 0));
#else
                UNUSED(MTSWDMX_SetInfo(_u1SwdmxSrcId, (UINT8)MTSWDMX_ENABLE_STREAM, (UINT32)MTSWDMX_STRM_TYPE_PCR, 0, 0));
//#endif
            }
        }
#endif
    }
    else
    {
//#ifdef ENABLE_MULTIMEDIA
        if(pt_comp_id->e_type == DRVT_DEMUX_TS_PCR)
        {
            MW_DMX_PID_CONTYPE_T eType;

			_DmxMwLockSemaphore(&dmxmw_mutex);
            eType = _arDmxInfo[u1Pidx].eConType;
			_DmxMwUnlockSemaphore(&dmxmw_mutex);

            if((eType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && (eType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
            {
#ifndef CC_53XX_SWDMX_V2
                UNUSED(MTSWDMX_SetInfo((UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)MTSWDMX_STRM_TYPE_PCR, 0, 0));
#else
                UNUSED(MTSWDMX_SetInfo(_u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)MTSWDMX_STRM_TYPE_PCR, 0, 0));
#endif
            }
        }
//#endif
        // Flush DSP first in case of disabling an audio channel
        if (MTDMX_GetPid(u1Pidx, MTDMX_PID_FLAG_BUFFER | MTDMX_PID_FLAG_DEVICE_ID,
            &rPid) != MTR_OK)
        {
            return RMR_DRV_INV_SET_INFO;
        }

        if (rPid.ePidType == MTDMX_PID_TYPE_ES_AUDIO)
        {
            ///TODO: do we really need to call DSP flush??
            ;//AUD_WaitDspFlush(rPid.u1DeviceId);
        }
    }
    LOG(6,"%s,%s u1Pidx=%u",__FUNCTION__,((rPid.fgEnable)? "Enable":"Disalbe"),u1Pidx);
    // Enable or disable a PID
    rPid.fgEnable = fgEnable;
    if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_VALID, &rPid))
    {
        i4Ret = RMR_DRV_INV_SET_INFO;
    }

    return i4Ret;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnSetDestination
 *  Event handler of setting destination ID
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnSetDestination(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info, UINT8 u1Pidx)
{
    DEMUX_DEST_INFO_T* ptDestInfo;
    BOOL fgRet;
    MTDMX_PID_T rPid = {0};

    ptDestInfo = (DEMUX_DEST_INFO_T*)pv_set_info;

    if (ptDestInfo == NULL)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    fgRet = TRUE;
    switch (ptDestInfo->e_type)
    {
    case DRVT_VID_DEC:

		_DmxMwLockSemaphore(&dmxmw_mutex);
        _arDestInfo[u1Pidx].e_type = DRVT_VID_DEC;
        rPid.u2Pid = _arDestInfo[u1Pidx].ui2_id;
		_DmxMwUnlockSemaphore(&dmxmw_mutex);

        //rPid.fgEnable = TRUE;
        rPid.ePidType = MTDMX_PID_TYPE_ES_VIDEO;
//#if defined(CC_VOMX_TV_COEXIST) && defined(CC_FBM_TWO_FBP)
#if defined(CC_3D_MM_DS_SUPPORT) || defined(CC_FBM_TWO_FBP) || defined(CC_DUAL_TUNER_SUPPORT)   /* all mpv */
        rPid.u1ChannelId = (UINT8)MW_MPV_CompIdtoEsId(ptDestInfo->ui2_id);
#endif
        LOG(6,"%s,Connect to DRVT_VID_DEC:u1Pidx=%u.\r\n",__FUNCTION__,u1Pidx);
        fgRet = _MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_PID |
            MTDMX_PID_FLAG_BUFFER, &rPid);
        break;

    case DRVT_AUD_DEC:

		_DmxMwLockSemaphore(&dmxmw_mutex);
        _arDestInfo[u1Pidx].e_type = DRVT_AUD_DEC;
        rPid.u2Pid = _arDestInfo[u1Pidx].ui2_id;
		_DmxMwUnlockSemaphore(&dmxmw_mutex);
        LOG(6,"%s,Connect to DRVT_AUD_DEC:u1Pidx=%u.\r\n",__FUNCTION__,u1Pidx);
        //rPid.fgEnable = TRUE;
        rPid.ePidType = MTDMX_PID_TYPE_ES_AUDIO;
        rPid.u1DeviceId = (UINT8)ptDestInfo->ui2_id;
        fgRet = _MW_DMX_SetPid(u1Pidx,  MTDMX_PID_FLAG_PID |
            MTDMX_PID_FLAG_BUFFER | MTDMX_PID_FLAG_DEVICE_ID, &rPid);
        break;

    default:
        fgRet = FALSE;
        break;
    }

    if(!fgRet)
    {
        return RMR_DRV_INV_SET_INFO;
    }

//#ifdef ENABLE_MULTIMEDIA
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(0,"%s Set pid index Error.\r\n",__FUNCTION__);
        return RMR_DRV_INV_SET_INFO;
    }

//#endif

    UNUSED(pt_comp_id);

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnSetStcId
 *  Set STC ID
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnSetStcId(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info, UINT8 u1Pidx)
{
    UINT8 u1PidIndex, u1StcId;
    MW_DMX_PID_INFO_T rPidInfo;

    ASSERT(pt_comp_id != NULL);

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1PidIndex = (UINT8)pt_comp_id->ui2_id;
    u1StcId = (UINT8)(UINT32)pv_set_info;

    _DmxMwLockSemaphore(&dmxmw_mutex);
    rPidInfo = _arDmxInfo[u1Pidx];
    _DmxMwUnlockSemaphore(&dmxmw_mutex);

    switch (pt_comp_id->e_type)
    {
    case DRVT_DEMUX_TS_PCR:
        // No another PES entry with the same PID
        // Append PID
        if(u1Pidx != u1PidIndex)
        {
          ASSERT(0);
        }

        LOG(6,"%s,Connect to DRVT_DEMUX_TS_PCR:u1Pidx=%u.\r\n",__FUNCTION__,u1Pidx);
        if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_TUNER_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_TUNER_MAX))
        {
            if(MTDMX_SetPcrPidEx(rPidInfo.u1TsIndex, u1Pidx, _MwDmxStcRemap(u1StcId), 0) != MTR_OK)
            {
                return RMR_DRV_INV_SET_INFO;
            }
        }
        else if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
        {
            if(MTDMX_SetPcrPidEx(DMX_TIMESHIFT_TS_INDEX, u1Pidx, _MwDmxStcRemap(u1StcId), 0) != MTR_OK)
        {
            LOG(0,"%s,For Timeshift ,Set PCR_PID[u1Pidx=%u] error.\r\n",__FUNCTION__,u1Pidx);
            return RMR_DRV_INV_SET_INFO;
        }
        }
        else
        {
            printf("%s Connect Type Error!\n", __FUNCTION__);
            return RMR_DRV_INV_SET_INFO;
        }
        break;

    default:
        return RMR_DRV_INV_SET_INFO;
        break;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmx_Connect
 *  Middleware interface - connect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmx_Connect(
                            const DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    DRV_COMP_ID_T* prConnectComp = 0;
    MTDMX_PID_T rPid = {0};
    //UINT32 u4Flags;
    UINT8 u1Pidx;
    MW_DMX_PID_INFO_T rPidInfo;

    // 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(0,"Set Pid Index Error.\r\n");
        return RMR_DRV_INV_CONN_INFO;
    }

    // Check connection type
    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
       LOG(0,"Set Connect Type error(%d).\r\n",e_conn_type);
       return RMR_DRV_INV_CONN_INFO;
    }

    // Check frontend component
    //
    // Only support normal tuner currently
    // ...

    // Check component type
     LOG(6,">>>>>>>>>>>>>>>>>>%s<<<<<<<<<<<<<<<<.\r\n",__FUNCTION__);
	prConnectComp = (DRV_COMP_ID_T*)pv_conn_info;
    if (!_MW_DMX_ConnectInfo(u1Pidx, (UINT32)pt_comp_id->e_type, prConnectComp->e_type, prConnectComp->ui2_id))
    { 
        Printf("%s,pidx=%u Connection Error.\r\n",__FUNCTION__,u1Pidx);
		return RMR_DRV_CONN_FAILED;
	}

    _DmxMwLockSemaphore(&dmxmw_mutex);
    rPidInfo = _arDmxInfo[u1Pidx];
    _DmxMwUnlockSemaphore(&dmxmw_mutex);

    if (pt_comp_id->e_type == DRVT_DEMUX_TS_PCR)
    {
        UINT8 u1StcId;

        // Get mapping of PID index and STC id
        u1StcId = _MwDmxGetStcId(u1Pidx);

        if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_TUNER_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_TUNER_MAX))
        {
        if (MTDMX_SetPcrPidEx(rPidInfo.u1TsIndex, u1Pidx, u1StcId, 0) != MTR_OK)
        {
            Printf("%s,For DTV,pidx=%u  Set PCR Pid Error.\r\n",__FUNCTION__,u1Pidx);
            return RMR_DRV_CONN_FAILED;
        }
    }
        else if (((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
        {
            if (MTDMX_SetPcrPidEx(DMX_TIMESHIFT_TS_INDEX, u1Pidx, u1StcId, 0) != MTR_OK)
            {
                Printf("%s,For Timeshift,pidx=%u  Set PCR Pid Error.\r\n",__FUNCTION__,u1Pidx);
                return RMR_DRV_CONN_FAILED;
            }
        }
        else
        {
            printf("%s Connect Type Error!\n", __FUNCTION__);
            return RMR_DRV_INV_SET_INFO;
        }

    }
    else
    {
        x_memset(&rPid, 0, sizeof(rPid));
		LOG(6,"%s ,initializing pidx %u.\r\n ",__FUNCTION__,u1Pidx);
        if (!_MW_DMX_SetPid(u1Pidx, MTDMX_PID_FLAG_ALL, &rPid))
        {
            return RMR_DRV_CONN_FAILED;
        }
    }

    UNUSED(z_conn_info_len);
    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmx_Disconnect
 *  Middleware interface - disconnect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmx_Disconnect(
                            const DRV_COMP_ID_T*    pt_comp_id,
                            DRV_DISC_TYPE_T         e_disc_type,
                            const VOID*             pv_disc_info,
                            SIZE_T                  z_disc_info_len)
{
    UINT8 u1Pidx;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    // Check component type
    // ...

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    // Check frontend component
    // ...
    LOG(6,">>>>>>>>>>>>>>>>>>%s<<<<<<<<<<<<<<<<<<<<<.\r\n",__FUNCTION__);
    if (pt_comp_id->e_type == DRVT_DEMUX_TS_PCR)
    {
        // Reset mapping of PID index and STC id by setting an invalid STC id
        UNUSED(MTDMX_SetPcr(u1Pidx, FALSE));
    }

	// Reset scramble state callback
	if (!_MwDmxResetScrambleCallback(u1Pidx))
	{
		return RMR_DRV_DISC_FAILED;
	}

    if (MTDMX_FreePid(u1Pidx) != MTR_OK)
    {
        // Release a PID
        return RMR_DRV_DISC_FAILED;
    }

    if (!_MW_DMX_DisConnectInfo(u1Pidx))
    {
		return RMR_DRV_DISC_FAILED;
	}

	_DmxMwLockSemaphore(&dmxmw_mutex);
    x_memset(&_arDestInfo[u1Pidx], 0, sizeof(_arDestInfo[u1Pidx]));
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    UNUSED(e_disc_type);
    UNUSED(z_disc_info_len);

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmx_Get
 *  Middleware interface - get
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmx_Get(
                        const DRV_COMP_ID_T*    pt_comp_id,
                        DRV_GET_TYPE_T          e_get_type,
                        VOID*                   pv_get_info,
                        SIZE_T*                 pz_get_info_len)
{
    MTDMX_PID_T rPid = {0};
    DEMUX_SCRS_T* prScrs;
    DEMUX_STC_INFO_T* prStc;
    STC_CLOCK rStcClk;
    UINT8 u1Pidx;
    UINT8 u1StcId;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    // Check component type
    // ...

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_GET_INFO;
    }

//#ifdef ENABLE_MULTIMEDIA
    if(!_MW_DMX_IsPidConnedted(u1Pidx))
    {
        return RMR_DRV_INV_GET_INFO;
    }
//#endif  // ENABLE_MULTIMEDIA

    //get
    switch (e_get_type)
    {
    case DEMUX_GET_TYPE_PID:
        // Get PID value
        if (MTDMX_GetPid(u1Pidx, MTDMX_PID_FLAG_PID, &rPid) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        ASSERT(pz_get_info_len != NULL);
        *(MPEG_2_PID_T*)pv_get_info = rPid.u2Pid;
        *pz_get_info_len = sizeof(MPEG_2_PID_T);
        break;

    case DEMUX_GET_TYPE_CTRL:
        // Query if a PID is enabled or not
        if (MTDMX_GetPid(u1Pidx, MTDMX_PID_FLAG_VALID, &rPid) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        ASSERT(pz_get_info_len != NULL);
        *(BOOL*)pv_get_info = rPid.fgEnable;
        *pz_get_info_len = sizeof(BOOL);
        break;

    case DEMUX_GET_TYPE_SCRS:
        prScrs = (DEMUX_SCRS_T*)pv_get_info;
        *prScrs = DEMUX_SCRS_UNKNOWN;
        if (MTDMX_GetPid(u1Pidx, MTDMX_PID_FLAG_VALID, &rPid) != MTR_OK)
        {
            if (rPid.fgEnable)
            {
                MTDMX_SCRAMBLE_STATE_T eState;

                MTDMX_GetScrambleState(u1Pidx, &eState);
                switch (eState)
                {
                case MTDMX_SCRAMBLE_STATE_CLEAR:
                    *prScrs = DEMUX_SCRS_CLEAR;
                    break;

                case MTDMX_SCRAMBLE_STATE_SCRAMBLED:
                    *prScrs = DEMUX_SCRS_SCRAMBLED;
                    break;

                case MTDMX_SCRAMBLE_STATE_UNKNOWN:
                default:
                    *prScrs = DEMUX_SCRS_UNKNOWN;
                    break;
                }
            }
        }
        break;

    case DEMUX_GET_TYPE_STC:
        {
            UINT32 u4StcHi = 0, u4StcLo = 0;

            prStc = (DEMUX_STC_INFO_T*)pv_get_info;
            u1Pidx = (UINT8)pt_comp_id->ui2_id;     // Bug Fixed
            u1StcId = _MwDmxGetStcId(u1Pidx);

            x_memset((void*)&rStcClk, 0, sizeof(rStcClk));

            if(MTDMX_GetCurSTCEx(_MwDmxStcRemap(u1StcId), &u4StcHi, &u4StcLo) == MTR_OK)
            {
                prStc->ui1_flags = DEMUX_STC_FLAG_STABLE;
            }
            else
            {
                prStc->ui1_flags = 0;
            }

            prStc->t_stc = (STC_T) (u4StcLo);
        }
        break;

    default:
        break;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmx_Set
 *  Middleware interface - set
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmx_Set(
                    DRV_COMP_ID_T*      pt_comp_id,
                    DRV_SET_TYPE_T      e_set_type,
                    const VOID*         pv_set_info,
                    SIZE_T              z_set_info_len)
{
    INT32 i4Ret = RMR_OK;
    UINT8 u1Pidx;
    BOOL fgRet;

    // Check parameters
    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    // Check component type
    // ...

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return RMR_DRV_INV_SET_INFO;
    }

//#ifdef ENABLE_MULTIMEDIA
    if(!_MW_DMX_IsPidConnedted(u1Pidx))
    {
        return RMR_DRV_INV_SET_INFO;
    }
//#endif  // ENABLE_MULTIMEDIA

    switch (e_set_type)
    {
    case DEMUX_SET_TYPE_PID:
        // Set PID value
        i4Ret = _MwDmxOnSetPid(pt_comp_id, pv_set_info, u1Pidx);
        break;

    case DEMUX_SET_TYPE_CTRL:
        // Enable/disable PID
        i4Ret = _MwDmxOnEnablePid(pt_comp_id, pv_set_info, u1Pidx);
        break;

    case DEMUX_SET_TYPE_DEST_ID:
        // Set destination ID
        i4Ret = _MwDmxOnSetDestination(pt_comp_id, pv_set_info, u1Pidx);
        break;

    case DEMUX_SET_TYPE_SCRS_NFY_FCT:
        // Set scramble state callback function
        fgRet = _MwDmxSetScrambleCallback(u1Pidx,
            (DEMUX_NFY_INFO_T*)pv_set_info);
        i4Ret = (fgRet ? RMR_OK : RMR_DRV_INV_SET_INFO);
        break;

    case DEMUX_SET_TYPE_STC_ID:
        #ifdef  CC_SUPPORT_2STC
        i4Ret = _MwDmxOnSetStcId(pt_comp_id, pv_set_info, u1Pidx);
        #else
        UNUSED(_MwDmxOnSetStcId);
        #endif
        break;

    default:
        break;
    }

    UNUSED(z_set_info_len);

    return i4Ret;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _MW_DMX_CISetFrontEnd
 * This function store CI setting, then used for MM -> DMX for re-initialize
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CISetFrontEnd(DMX_FRONTEND_T eFEType)
{
    UNUSED(eFEType);
    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_ConnectInfo
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_ConnectInfo(UINT8 u1Pidx, UINT32 u4RMType, DRV_TYPE_T eType , UINT16 u2Id)
{
//#ifdef ENABLE_MULTIMEDIA
    MW_DMX_PID_CONTYPE_T eConType;
	MTDMX_PID_INFO_T  rPidInfo;


    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
    	Printf("Set the wrong PID index!\n");
        return FALSE;
    }

	//check pidx favalid
	_DmxMwLockSemaphore(&dmxmw_mutex);
	ASSERT(!_arDmxInfo[u1Pidx].fgValid);
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

	//Assign connect type
	if (eType == DRVT_BUF_AGENT)
    {
        if(u2Id >= MW_DMX_CONN_TYPE_BUFAGENT_MAX - MW_DMX_CONN_TYPE_BUFAGENT_0)
        {
            Printf("Connet Id is over the max number.\n");
			return FALSE;
        }

        eConType = (MW_DMX_PID_CONTYPE_T)(MW_DMX_CONN_TYPE_BUFAGENT_0 + u2Id);
    }
    else
    {
        if(u2Id >= MW_DMX_CONN_TYPE_TUNER_MAX - MW_DMX_CONN_TYPE_TUNER_0)
        {
            Printf("Connet Id is over the max number.\n");
           return FALSE;
    	}

  	   //TODO
        //eConType = (MW_DMX_PID_CONTYPE_T)(MW_DMX_CONN_TYPE_TUNER_0 + u2Id);
        eConType = (MW_DMX_PID_CONTYPE_T)(MW_DMX_CONN_TYPE_TUNER_0 + (u2Id-1));
    }


	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arDmxInfo[u1Pidx].fgValid = TRUE;
    _arDmxInfo[u1Pidx].u4RMType = u4RMType;
    _arDmxInfo[u1Pidx].eConType = eConType;
	_arDmxInfo[u1Pidx].u1TsIndex = _MwDmxContypeToTsIdx(eConType);
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

	//Printf("[DMX_MW.c] ConnectInfo, VIRPidx: %d, Type: %d.\n",u1Pidx, eConType);

	rPidInfo.fgValid = TRUE;
	rPidInfo.eConType = eConType;

	if (MTDMX_ConnectInfo(u1Pidx, &rPidInfo) != MTR_OK)
    {
        // Release a PID
        return FALSE;
    }

/*
    if (eType == DRVT_BUF_AGENT)
    {
        if(MTDMX_MUL_SetPidxInstance(0, u1Pidx) != MTR_OK)
        {
            return;
        }
    }
*/
//#else
//    UNUSED(u1Pidx);
 //   UNUSED(u4RMType);
//    UNUSED(eType);
//#endif  // ENABLE_MULTIMEDIA

	return TRUE;
}


//#ifdef ENABLE_MULTIMEDIA
//-----------------------------------------------------------------------------
/** _MW_DMX_IsPidConnedted
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_IsPidConnedted(UINT8 u1Pidx)
{
    BOOL fgValid;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    fgValid = _arDmxInfo[u1Pidx].fgValid;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return fgValid;
}
//#endif  // ENABLE_MULTIMEDIA


//-----------------------------------------------------------------------------
/** _MW_DMX_DisConnectInfo
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_DisConnectInfo(UINT8 u1Pidx)
{
//#ifdef ENABLE_MULTIMEDIA
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    _arDmxInfo[u1Pidx].fgValid = FALSE;
    _arDmxInfo[u1Pidx].fgCached = FALSE;
    _arDmxInfo[u1Pidx].u4RMType = 0;
    _arDmxInfo[u1Pidx].eConType = MW_DMX_CONN_TYPE_NONE;
	_arDmxInfo[u1Pidx].u1TsIndex = 0;  /*set default to 0*/
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

	if (MTDMX_DisconnectInfo(u1Pidx) != MTR_OK)
    {
        return  FALSE;
    }
//#else
 //   UNUSED(u1Pidx);
//#endif  // ENABLE_MULTIMEDIA

	return TRUE;
}


//#ifdef ENABLE_MULTIMEDIA
//-----------------------------------------------------------------------------
/** _MW_DMX_FlushAllCachePid
 * Tricky for MM TS file
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_FlushAllCachePid(void)
{
    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_SetCacheOn
 */
//-----------------------------------------------------------------------------
void _MW_DMX_SetCacheOn(BOOL fgCacheOn)
{
    UNUSED(fgCacheOn);
}


//-----------------------------------------------------------------------------
/** _MW_DMX_GetPidConType
 */
//-----------------------------------------------------------------------------
MW_DMX_PID_CONTYPE_T _MW_DMX_GetPidConType(UINT8 u1Pidx)
{
    MW_DMX_PID_CONTYPE_T eConType;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return MW_DMX_CONN_TYPE_NONE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    eConType = _arDmxInfo[u1Pidx].eConType;
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    return eConType;
}
//#endif  // ENABLE_MULTIMEDIA


//-----------------------------------------------------------------------------
/** _MW_DMX_SetPid
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_SetPid(UINT8 u1Pidx, UINT32 u4Flags, const MTDMX_PID_T* prPid)
{
    MTDMX_PID_T rPid = {0};

    MW_DMX_PID_INFO_T rPidInfo;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    rPidInfo = _arDmxInfo[u1Pidx];
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

    if(!rPidInfo.fgValid)
    {
        // set pid before connect, so return false
        printf("%s set pid before connect\n", __FUNCTION__);
        return FALSE;
    }


    rPid = *prPid;


    if(((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_TUNER_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_TUNER_MAX))
    {
        UINT8 i;
        MTDMX_PID_T rPidx = {0};

        // check if this is the first SetPid. If yes, set the video type to MPEG2 before SetPid
        for(i =0; i < DMX_NUM_PID_INDEX; i++)
        {
             if(MTDMX_GetPid(i, (DMX_PID_FLAG_VALID  | DMX_PID_FLAG_TS_INDEX), &rPidx) == MTR_OK)
            {
                if((rPidx.fgEnable == TRUE) && (rPidx.u1TsIndex == rPidInfo.u1TsIndex))
                {
                    break;
                }
            }
        }

        if(i >= DMX_NUM_PID_INDEX)
        {
		  //Set default video type
		  MTDMX_SetVideoTypeEx(rPidInfo.u1TsIndex,MTDMX_VIDEO_MPEG);

	    //Set framer packet size
		  MTDMX_SetFramerPacketSize(rPidInfo.u1TsIndex,188);

        }
        rPid.u1TsIndex = rPidInfo.u1TsIndex;
        return (MTDMX_SetPid(u1Pidx, u4Flags, &rPid) == MTR_OK)?TRUE:FALSE;
    }
    else if(((INT32)rPidInfo.eConType >= MW_DMX_CONN_TYPE_BUFAGENT_0) && ((INT32)rPidInfo.eConType < MW_DMX_CONN_TYPE_BUFAGENT_MAX))
    {
        //printf("%s set pid with DMX_SOURCE_TSFILE0\n", __FUNCTION__);
        return (MTDMX_SetFilePid(u1Pidx, u4Flags, &rPid) == MTR_OK)?TRUE:FALSE;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_DESC_Init
 *  Initialize demux record driver for middleware interface
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
#ifdef CI_PLUS_SUPPORT

static DMX_MW_DESC_INFO_T _arDmxDesc[DMX_DESC_COMP_NUM];
static UINT8 _u1DmxKeyIdx = MW_DMX_CRYPT_INVALID_KEYX;

static BOOL _IsOtherDescHasSamePidValue(UINT8 u1CompId)
{
    UINT8 i = 0;

    //Search all desc has the same pid value
    for(i = 0; i < DMX_DESC_COMP_NUM; i++)
    {
        if((i != u1CompId) && (_arDmxDesc[i].fgEnable == TRUE))
        {
            if((_arDmxDesc[i].u2PidNum == _arDmxDesc[u1CompId].u2PidNum) && (_arDmxDesc[i].rDescSource == _arDmxDesc[u1CompId].rDescSource))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

#ifndef CC_CI_PLUS_TZ_SUPPORT
static BOOL _MwDmxOnEnableDesc(UINT8 u1CompId, const VOID* pv_set_info)
{
    MTDMX_PID_T rPid = {0};
    MTFVR_PID_T rFvrPid = {0};

    MTDMX_DESC_MODE_T eDescMode = MTDMX_DESC_MODE_NONE;
    BOOL fgEnable = (BOOL)(UINT32)pv_set_info;
    INT32 i = 0;
    BOOL fgEnable_Key = FALSE;
	
    if (_u1DmxKeyIdx == MW_DMX_CRYPT_INVALID_KEYX)
    {
        if (!_MW_DMX_CRYPT_AllocateCAKeySlot(NULL, MWDMX_CRYPTKEY_FLAG_STATIC_IDX, 0, &_u1DmxKeyIdx))
        {
            printf("%s - failed to allocate static keyIdx\n", __FUNCTION__);
            return FALSE;
        }
    }

    x_memset((void*)&rPid, 0, sizeof(rPid));
    x_memset((void*)&rFvrPid, 0, sizeof(rFvrPid));

    if(fgEnable)
    {
        //handle Playback Case
        if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_DTV)
        {
            //Search all playback Pidx that match pid number
            for(i = 0; i < DMX_NUM_PID_INDEX; i++)
            {
                if(MTDMX_GetPid(i, MTDMX_PID_FLAG_PID | MTDMX_PID_FLAG_VALID| MTDMX_PID_FLAG_BUFFER, &rPid) == MTR_OK)
                {
                    if(rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
                    {
                        fgEnable_Key = (rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum);
                    }
    				else
    				{
    				    fgEnable_Key = ((rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rPid.fgEnable));
    				}
                    if(fgEnable_Key)
                    {
                        //Setup Descrambler
                        if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DES_ECB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_DES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DES_CBC)
                        {
                        	LOG(1,"MTDMX_DESC_MODE_DES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1]);
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DES_CBC;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_3DES_ECB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_3DES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_3DES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_3DES_CBC)
                        {
                            LOG(1,"MTDMX_DESC_MODE_3DES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1]);
                        
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_3DES_CBC;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DVB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_DVB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1]);
                        
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DVB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_AES_128_ECB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_AES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                            MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven,  _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_AES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_AES_128_CBC)
                        {
                            ///TODO: extend AES IV and length setup
                           	LOG(1,"MTDMX_DESC_MODE_AES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1],
    							                                     _arDmxDesc[u1CompId].au4Iv[2],_arDmxDesc[u1CompId].au4Iv[3]);
                            MTDMX_SetAesEvenOddIVEx(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Iv);
                            MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                            MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven,  (UINT32*)_arDmxDesc[u1CompId].au4Key);

                            eDescMode = MTDMX_DESC_MODE_AES_CBC;
                        }
                        else
                        {
                            eDescMode = MTDMX_DESC_MODE_NONE;
                            LOG(1,"[dmxmw]MW SetAlgorithm None.u1CompId : %d.\n",u1CompId);
                        }

                        MTDMX_SetDescramblPid(i, eDescMode, _u1DmxKeyIdx);
                        LOG(1, "%s,[pidx=%u]set key idx:%d,mode=%d.\n", __FUNCTION__,i,_u1DmxKeyIdx,eDescMode);
                    }
                }
            }
        }
        else if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_FVR)
        {
            //Search all record Pidx that match pid number
            for(i = 0; i < FVR_NUM_PID_INDEX; i++)
            {
                if(MTFVR_GetPid(i, MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE, &rFvrPid) == MTR_OK)
                {
                    if((rFvrPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rFvrPid.fgEnable))
                    {
                        rFvrPid.fgDesc = TRUE;
                        //Setup Descrambler
                        if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DES_ECB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_DES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DES_CBC)
                        {
                        	LOG(1,"MTDMX_DESC_MODE_DES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1]);
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DES_CBC;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_3DES_ECB)
                        {
                           	LOG(1,"MTDMX_DESC_MODE_3DES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_3DES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_3DES_CBC)
                        {
                         	LOG(1,"MTDMX_DESC_MODE_3DES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1])
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_3DES_CBC;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_DVB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_DVB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1]);
                            MTDMX_SetDesIV(_arDmxDesc[u1CompId].au4Iv[1], _arDmxDesc[u1CompId].au4Iv[0]);
                            MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_DVB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_AES_128_ECB)
                        {
                            LOG(1,"MTDMX_DESC_MODE_AES_ECB:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
                            MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                            MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven,  _arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_AES_ECB;
                        }
                        else if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_AES_128_CBC)
                        {
                            ///TODO: extend AES IV and length setup
                            LOG(1,"MTDMX_DESC_MODE_AES_CBC:KeyIdx=%d,%s:\n",_u1DmxKeyIdx,((_arDmxDesc[u1CompId].fgEven)? "EVEN":
    							"ODD"));
    					    LOG(1,"KEY(0-3):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[0],_arDmxDesc[u1CompId].au4Key[1],
    							                                           _arDmxDesc[u1CompId].au4Key[2],_arDmxDesc[u1CompId].au4Key[3]);
    					    LOG(1,"KEY(4-7):0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Key[4],_arDmxDesc[u1CompId].au4Key[5],
    							                                           _arDmxDesc[u1CompId].au4Key[6],_arDmxDesc[u1CompId].au4Key[7]);
    						LOG(1,"IV:0x%08x,0x%08x,0x%08x,0x%08x\n",_arDmxDesc[u1CompId].au4Iv[0],_arDmxDesc[u1CompId].au4Iv[1],
    							                                     _arDmxDesc[u1CompId].au4Iv[2],_arDmxDesc[u1CompId].au4Iv[3]);
                            MTDMX_SetAesEvenOddIVEx(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven, _arDmxDesc[u1CompId].au4Iv);
                            MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                            MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[u1CompId].fgEven,  (UINT32*)_arDmxDesc[u1CompId].au4Key);
                            eDescMode = MTDMX_DESC_MODE_AES_CBC;
                            LOG(6, "%s, CBC, even:%d, val:0x%x\n", __FUNCTION__, (UINT32)_arDmxDesc[u1CompId].fgEven,
                                        _arDmxDesc[u1CompId].au4Key[0]);
                        }
                        else
                        {
                            eDescMode = MTDMX_DESC_MODE_NONE;
                            rFvrPid.fgDesc = FALSE;
    					    LOG(1,"[dmxmw]For Record,MW SetAlgorithm None,Ftup Decrambler  Disalbe.\n");
                        }

                        rFvrPid.u1DescKeyidx = _u1DmxKeyIdx;
                        rFvrPid.eDescMode = eDescMode;
                        MTFVR_SetPid(i, MTFVR_PID_FLAG_DESC, &rFvrPid);
                        LOG(1, "%s,[pidx=%u]set fvr key idx:%d,mode=%d.\n", __FUNCTION__,i,_u1DmxKeyIdx,eDescMode);
                    }
                }
            }
        }
        else
        {
            printf("%s - Please Set Descramble Source Before Enable!\n", __FUNCTION__);
            return FALSE;
        }

        _arDmxDesc[u1CompId].fgEnable = TRUE;
    }
    else
    {
        //handle Playback Case
        if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_DTV)
        {
            //Search all Pidex that match pid number
            for(i = 0; i < DMX_NUM_PID_INDEX; i++)
            {
                if(MTDMX_GetPid(i, MTDMX_PID_FLAG_PID | MTDMX_PID_FLAG_VALID, &rPid) == MTR_OK)
                {
                    if((rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rPid.fgEnable))
                    {
                        // Only set descramble mode to none when this is the last descrambler
                        // map to this pid value
                        if(!_IsOtherDescHasSamePidValue(u1CompId))
                        {
                            //Stop playback descrambler
                            MTDMX_SetDescramblPid(i, eDescMode, _u1DmxKeyIdx);
                        }
                    }
                }
            }
        }
        else if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_FVR)
        {
            //Search all record Pidx that match pid number
            for(i = 0; i < FVR_NUM_PID_INDEX; i++)
            {
                if(MTFVR_GetPid(i, MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE, &rFvrPid) == MTR_OK)
                {
                    if((rFvrPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rFvrPid.fgEnable))
                    {
                        // Only set descramble mode to none when this is the last descrambler
                        // map to this pid value
                        if(!_IsOtherDescHasSamePidValue(u1CompId))
                        {
                            //Stop record descrambler
                            rFvrPid.fgDesc = FALSE;
                            rFvrPid.eDescMode = MTDMX_DESC_MODE_NONE;
                            MTFVR_SetPid(i, MTFVR_PID_FLAG_DESC, &rFvrPid);
                        }
                    }
                }
            }
        }
        else
        {
            printf("%s - Please Set Descramble Source Before Enable!\n", __FUNCTION__);
            return FALSE;
        }

        _arDmxDesc[u1CompId].fgEnable = FALSE;
    }

    return TRUE;
}

BOOL _MwDmxTraverseFvrDesc(UINT8 u1TsIdx)
{
    MTDMX_DESC_MODE_T eDescMode = MTDMX_DESC_MODE_NONE;
    MTFVR_PID_T rFvrPid = {0};
    INT32 i = 0, j = 0;

    x_memset((void*)&rFvrPid, 0, sizeof(rFvrPid));

     ///TODO: Need handel two pvr case and two CI+ case
    for(i = 0; i < DMX_DESC_COMP_NUM; i++)
    {
        if(!_arDmxDesc[i].fgEnable)
        {
            continue;
        }

        //Search all record Pidx that match pid number
        for(j = 0; j < FVR_NUM_PID_INDEX; j++)
        {
            if(MTFVR_GetPid(j, (MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE | MTFVR_PID_FLAG_TSINDEX), &rFvrPid) == MTR_OK)
            {
                if((rFvrPid.u2Pid == _arDmxDesc[i].u2PidNum) && (rFvrPid.fgEnable) && (rFvrPid.u1TsIndex == u1TsIdx))
                {
                    rFvrPid.fgDesc = TRUE;

                    if (_u1DmxKeyIdx == MW_DMX_CRYPT_INVALID_KEYX)
                    {
                        return FALSE;
                    }

                    //Setup Descrambler
                    if(_arDmxDesc[i].rDescAlg == DESC_ALG_DES_ECB)
                    {
                        MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_DES_ECB;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_DES_CBC)
                    {
                        MTDMX_SetDesIV(_arDmxDesc[i].au4Iv[1], _arDmxDesc[i].au4Iv[0]);
                        MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_DES_CBC;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_3DES_ECB)
                    {
                        MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_3DES_ECB;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_3DES_CBC)
                    {
                        MTDMX_SetDesIV(_arDmxDesc[i].au4Iv[1], _arDmxDesc[i].au4Iv[0]);
                        MTDMX_SetDesKey(_u1DmxKeyIdx, TRUE, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_3DES_CBC;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_DVB)
                    {
                        MTDMX_SetDesIV(_arDmxDesc[i].au4Iv[1], _arDmxDesc[i].au4Iv[0]);
                        MTDMX_SetDesKey(_u1DmxKeyIdx, FALSE, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_DVB;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_AES_128_ECB)
                    {
                        MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                        MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[i].fgEven,  _arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_AES_ECB;
                    }
                    else if(_arDmxDesc[i].rDescAlg == DESC_ALG_AES_128_CBC)
                    {
                        ///TODO: extend AES IV and length setup
                        MTDMX_SetAesEvenOddIVEx(_u1DmxKeyIdx, _arDmxDesc[i].fgEven, _arDmxDesc[i].au4Iv);
                        MTDMX_SetAesKeyLenEx(_u1DmxKeyIdx, 128);
                        MTDMX_SetAesKey(_u1DmxKeyIdx, _arDmxDesc[i].fgEven,  (UINT32*)_arDmxDesc[i].au4Key);
                        eDescMode = MTDMX_DESC_MODE_AES_CBC;
                        LOG(6, "%s, CBC, even:%d, val:0x%x\n", __FUNCTION__, (UINT32)_arDmxDesc[i].fgEven,
                                    _arDmxDesc[i].au4Key[0]);
                    }
                    else
                    {
                        eDescMode = MTDMX_DESC_MODE_NONE;
                        rFvrPid.fgDesc = FALSE;
                    }

                    rFvrPid.u1DescKeyidx = _u1DmxKeyIdx;
                    rFvrPid.eDescMode = eDescMode;
                    MTFVR_SetPid(j, MTFVR_PID_FLAG_DESC, &rFvrPid);
                    LOG(6, "%s, set fvr key idx:%d\n", __FUNCTION__, _u1DmxKeyIdx);
                }
            }
        }
    }

    return TRUE;
}
#else
static BOOL _MwDmxDescSetKeyIndex(UINT8 u1CompId, const VOID* pv_set_info)
{
    UINT8 u1SecureKeyIdx;
    if (_u1DmxKeyIdx == MW_DMX_CRYPT_INVALID_KEYX)
    {
        if (!_MW_DMX_CRYPT_AllocateCAKeySlot(NULL, MWDMX_CRYPTKEY_FLAG_STATIC_IDX, 0, &_u1DmxKeyIdx))
        {
            printf("%s - failed to allocate static keyIdx\n", __FUNCTION__);
            return FALSE;
        }
    }
    u1SecureKeyIdx = (UINT8)(UINT32)pv_set_info;
    MTDMX_ReloadSecureKey(_u1DmxKeyIdx, u1SecureKeyIdx, MTDMX_CRYPTO_KEY_TZ_DTV_CIPLUS);
    return TRUE;
}

static BOOL _MwDmxOnEnableDesc(UINT8 u1CompId, const VOID* pv_set_info)
{
    MTDMX_PID_T rPid = {0};
    MTFVR_PID_T rFvrPid = {0};

    MTDMX_DESC_MODE_T eDescMode = MTDMX_DESC_MODE_NONE;
    BOOL fgEnable = (BOOL)(UINT32)pv_set_info;
    INT32 i = 0;
    BOOL fgEnable_Key = FALSE;
	
    x_memset((void*)&rPid, 0, sizeof(rPid));
    x_memset((void*)&rFvrPid, 0, sizeof(rFvrPid));

    if (_u1DmxKeyIdx == MW_DMX_CRYPT_INVALID_KEYX)
    {
        if (!_MW_DMX_CRYPT_AllocateCAKeySlot(NULL, MWDMX_CRYPTKEY_FLAG_STATIC_IDX, 0, &_u1DmxKeyIdx))
        {
            printf("%s - failed to allocate static keyIdx\n", __FUNCTION__);
            return FALSE;
        }
    }
    
    if(fgEnable)
    {
        //handle Playback Case
        if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_DTV)
        {
            //Search all playback Pidx that match pid number
            for(i = 0; i < DMX_NUM_PID_INDEX; i++)
            {
                if(MTDMX_GetPid(i, MTDMX_PID_FLAG_PID | MTDMX_PID_FLAG_VALID| MTDMX_PID_FLAG_BUFFER, &rPid) == MTR_OK)
                {
                    if(rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
                    {
                        fgEnable_Key = (rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum);
                    }
    				else
    				{
    				    fgEnable_Key = ((rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rPid.fgEnable));
    				}
                    if(fgEnable_Key)
                    {   
                        if(_arDmxDesc[u1CompId].rDescAlg == DESC_ALG_NONE)
                        {
                            eDescMode = MTDMX_DESC_MODE_NONE;
                        }
                        else
                        {
                            eDescMode = MTDMX_DESC_MODE_CA_SECURE;
                        }
                        MTDMX_SetDescramblPid(i, eDescMode, _u1DmxKeyIdx);
                        LOG(1, "%s,[pidx=%u]set key idx:%d,mode=%d.\n", __FUNCTION__,i,_u1DmxKeyIdx,eDescMode);
                    }
                }
            }
        }
        else if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_FVR)
        {
            //Search all record Pidx that match pid number
            for(i = 0; i < FVR_NUM_PID_INDEX; i++)
            {
                if(MTFVR_GetPid(i, MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE, &rFvrPid) == MTR_OK)
                {
                    if((rFvrPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rFvrPid.fgEnable))
                    {
                        rFvrPid.fgDesc = TRUE;
                        //Setup Descrambler                        
                        if(_arDmxDesc[u1CompId].rDescAlg != DESC_ALG_NONE)                        
                        {
                            eDescMode = MTDMX_DESC_MODE_CA_SECURE;
                        }
                        else
                        {
                            eDescMode = MTDMX_DESC_MODE_NONE;
                            rFvrPid.fgDesc = FALSE;
    					    LOG(1,"[dmxmw]For Record,MW SetAlgorithm None,Ftup Decrambler  Disalbe.\n");
                        }
                        
                        rFvrPid.u1DescKeyidx = _u1DmxKeyIdx;
                        rFvrPid.eDescMode = eDescMode;
                        MTFVR_SetPid(i, MTFVR_PID_FLAG_DESC, &rFvrPid);
                        LOG(1, "%s,[pidx=%u]set fvr key idx:%d,mode=%d.\n", __FUNCTION__,i,_u1DmxKeyIdx,eDescMode);
                    }
                }
            }
        }
        else
        {
            printf("%s - Please Set Descramble Source Before Enable!\n", __FUNCTION__);
            return FALSE;
        }

        _arDmxDesc[u1CompId].fgEnable = TRUE;
    }
    else
    {
        //handle Playback Case
        if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_DTV)
        {
            //Search all Pidex that match pid number
            for(i = 0; i < DMX_NUM_PID_INDEX; i++)
            {
                if(MTDMX_GetPid(i, MTDMX_PID_FLAG_PID | MTDMX_PID_FLAG_VALID, &rPid) == MTR_OK)
                {
                    if((rPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rPid.fgEnable))
                    {
                        // Only set descramble mode to none when this is the last descrambler
                        // map to this pid value
                        if(!_IsOtherDescHasSamePidValue(u1CompId))
                        {
                            //Stop playback descrambler
                            MTDMX_SetDescramblPid(i, eDescMode, _u1DmxKeyIdx);
                        }
                    }
                }
            }
        }
        else if (_arDmxDesc[u1CompId].rDescSource == DESC_SOURCE_FVR)
        {
            //Search all record Pidx that match pid number
            for(i = 0; i < FVR_NUM_PID_INDEX; i++)
            {
                if(MTFVR_GetPid(i, MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE, &rFvrPid) == MTR_OK)
                {
                    if((rFvrPid.u2Pid == _arDmxDesc[u1CompId].u2PidNum) && (rFvrPid.fgEnable))
                    {
                        // Only set descramble mode to none when this is the last descrambler
                        // map to this pid value
                        if(!_IsOtherDescHasSamePidValue(u1CompId))
                        {
                            //Stop record descrambler
                            rFvrPid.fgDesc = FALSE;
                            rFvrPid.eDescMode = MTDMX_DESC_MODE_NONE;
                            MTFVR_SetPid(i, MTFVR_PID_FLAG_DESC, &rFvrPid);
                        }
                    }
                }
            }
        }
        else
        {
            printf("%s - Please Set Descramble Source Before Enable!\n", __FUNCTION__);
            return FALSE;
        }

        _arDmxDesc[u1CompId].fgEnable = FALSE;
    }

    return TRUE;
}

BOOL _MwDmxTraverseFvrDesc(UINT8 u1TsIdx)
{
    MTDMX_DESC_MODE_T eDescMode = MTDMX_DESC_MODE_NONE;
    MTFVR_PID_T rFvrPid = {0};
    INT32 i = 0, j = 0;

    x_memset((void*)&rFvrPid, 0, sizeof(rFvrPid));

     ///TODO: Need handel two pvr case and two CI+ case
    for(i = 0; i < DMX_DESC_COMP_NUM; i++)
    {
        if(!_arDmxDesc[i].fgEnable)
        {
            continue;
        }

        //Search all record Pidx that match pid number
        for(j = 0; j < FVR_NUM_PID_INDEX; j++)
        {
            if(MTFVR_GetPid(j, (MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_ENABLE | MTFVR_PID_FLAG_TSINDEX), &rFvrPid) == MTR_OK)
            {
                if((rFvrPid.u2Pid == _arDmxDesc[i].u2PidNum) && (rFvrPid.fgEnable) && (rFvrPid.u1TsIndex == u1TsIdx))
                {
                    rFvrPid.fgDesc = TRUE;

                    if (_u1DmxKeyIdx == MW_DMX_CRYPT_INVALID_KEYX)
                    {
                        return FALSE;
                    }

                    //Setup Descrambler                        
                    if(_arDmxDesc[i].rDescAlg != DESC_ALG_NONE)                        
                    {
                        eDescMode = MTDMX_DESC_MODE_CA_SECURE;
                    }
                    else
                    {
                        eDescMode = MTDMX_DESC_MODE_NONE;
                        rFvrPid.fgDesc = FALSE;
    				    LOG(1,"[dmxmw]For Record,MW SetAlgorithm None,Ftup Decrambler  Disalbe.\n");
                    }

                    rFvrPid.u1DescKeyidx = _u1DmxKeyIdx;
                    rFvrPid.eDescMode = eDescMode;
                    MTFVR_SetPid(j, MTFVR_PID_FLAG_DESC, &rFvrPid);
                    LOG(6, "%s, set fvr key idx:%d\n", __FUNCTION__, _u1DmxKeyIdx);
                }
            }
        }
    }

    return TRUE;
}

#endif

//-----------------------------------------------------------------------------
/** _MwDmxDesc_Connect
 *  Middleware interface - connect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxDesc_Connect(
                            const DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    UINT8 u1CompId;

    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    // Check index range
    u1CompId = (UINT8)pt_comp_id->ui2_id;
    if (u1CompId >= DMX_DESC_COMP_NUM)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    _arDmxDesc[u1CompId].fgConnented = TRUE;

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxDesc_Disconnect
 *  Middleware interface - disconnect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxDesc_Disconnect(
                            const DRV_COMP_ID_T*    pt_comp_id,
                            DRV_DISC_TYPE_T         e_disc_type,
                            const VOID*             pv_disc_info,
                            SIZE_T                  z_disc_info_len)
{
    UINT8 u1CompId;

    // Check parameters
    if ((NULL == pt_comp_id))
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    // Check index range
    u1CompId = (UINT8)pt_comp_id->ui2_id;
    if (u1CompId >= DMX_DESC_COMP_NUM)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    _arDmxDesc[u1CompId].fgConnented = FALSE;
    _arDmxDesc[u1CompId].u2PidNum = 0x1FFF;
    _arDmxDesc[u1CompId].rDescSource = DESC_SOURCE_NONE;
    _arDmxDesc[u1CompId].rDescAlg = DESC_ALG_NONE;

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxDesc_Get
 *  Middleware interface - get
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxDesc_Get(
                        const DRV_COMP_ID_T*    pt_comp_id,
                        DRV_GET_TYPE_T          e_get_type,
                        VOID*                   pv_get_info,
                        SIZE_T*                 pz_get_info_len)
{
    UINT8 u1CompId = 0;
    BOOL fgRet = FALSE;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) ||
        (NULL == pz_get_info_len))
    {
       	return RMR_DRV_INV_GET_INFO;
    }

    // Check index range
    u1CompId = (UINT8)pt_comp_id->ui2_id;
    if (u1CompId >= DMX_DESC_COMP_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    if(!_arDmxDesc[u1CompId].fgConnented)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    switch (e_get_type)
    {
    case DESC_GET_TYPE_CTRL:
        *(BOOL*)pv_get_info = _arDmxDesc[u1CompId].fgEnable;
        *pz_get_info_len = sizeof(BOOL);
        fgRet = TRUE;
        break;

    case DESC_GET_TYPE_ALGORITHM:
        // Get descrambler mode value
        // Notice: We must make sure DESC_ALG_T
        // and DMX_DESC_MODE_T have the same enum order
        *(DESC_ALG_T*)pv_get_info = _arDmxDesc[u1CompId].rDescAlg;
        *pz_get_info_len = sizeof(DESC_ALG_T);
        fgRet = TRUE;
        break;

    case DESC_GET_TYPE_PID:
        // Get PID value
        *(MPEG_2_PID_T*)pv_get_info = _arDmxDesc[u1CompId].u2PidNum;
        *pz_get_info_len = sizeof(MPEG_2_PID_T);
        fgRet = TRUE;
        break;

    default:
        break;
    }

    return (fgRet ? RMR_OK : RMR_DRV_INV_GET_INFO);
}


//-----------------------------------------------------------------------------
/** _MwDmxDesc_Set
 *  Middleware interface - set
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxDesc_Set(
                    DRV_COMP_ID_T*      pt_comp_id,
                    DRV_SET_TYPE_T      e_set_type,
                    const VOID*         pv_set_info,
                    SIZE_T              z_set_info_len)
{
#ifndef CC_CI_PLUS_TZ_SUPPORT
    DESC_KEY_INFO_T *prKeyInfo;
#endif
    UINT8 u1CompId = 0;
    BOOL fgRet = FALSE;


    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1CompId = (UINT8)pt_comp_id->ui2_id;
    if (u1CompId >= DMX_DESC_COMP_NUM)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    if(!_arDmxDesc[u1CompId].fgConnented)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    switch (e_set_type)
    {
    case DESC_SET_TYPE_CTRL:
        //Set all descrambler config in one time
        fgRet = _MwDmxOnEnableDesc(u1CompId, pv_set_info);
        break;

    case DESC_SET_TYPE_ALGORITHM:
        _arDmxDesc[u1CompId].rDescAlg = *(DESC_ALG_T*)pv_set_info;
        fgRet = TRUE;
        break;

    case DESC_SET_TYPE_PID:
        _arDmxDesc[u1CompId].u2PidNum = (UINT16)(MPEG_2_PID_T)(UINT32)pv_set_info;
        fgRet = TRUE;
        break;

    case DESC_SET_TYPE_SOURCE:
        _arDmxDesc[u1CompId].rDescSource = *(DESC_SOURCE_T*)pv_set_info;
		LOG(1,"%s set DescSource=%d.\r\n",__FUNCTION__,_arDmxDesc[u1CompId].rDescSource);
        fgRet = TRUE;
        break;
#ifndef CC_CI_PLUS_TZ_SUPPORT
    case DESC_SET_TYPE_ODD_KEY:
        _arDmxDesc[u1CompId].fgEven = FALSE;
        prKeyInfo = (DESC_KEY_INFO_T*)pv_set_info;
        if(prKeyInfo->e_descrambler_alg == DESC_ALG_DES_ECB)
        {
            _arDmxDesc[u1CompId].u4KeyLen = (UINT32)prKeyInfo->u_key_info.t_des_ecb_key.z_key_len;
            ASSERT(_arDmxDesc[u1CompId].u4KeyLen <= 24);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Key, (UINT32 *)prKeyInfo->u_key_info.t_des_ecb_key.pui1_key, _arDmxDesc[u1CompId].u4KeyLen/4, DMX_CI_DES_KEY_SWAP_MODE);
        }
        else if(prKeyInfo->e_descrambler_alg == DESC_ALG_AES_128_CBC)
        {
            UINT32 au4Key[4];
            UINT32 au4Iv[4];
            x_memcpy(au4Key, prKeyInfo->u_key_info.t_aes_128_cbc_key.pui1_key, 16);
            x_memcpy(au4Iv, prKeyInfo->u_key_info.t_aes_128_cbc_key.pui1_iv, 16);

            _arDmxDesc[u1CompId].u4KeyLen = (UINT32)prKeyInfo->u_key_info.t_aes_128_cbc_key.z_key_len;
            ASSERT(_arDmxDesc[u1CompId].u4KeyLen <= 32);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Key, (UINT32 *)au4Key, _arDmxDesc[u1CompId].u4KeyLen/4, DMX_CI_AES_KEY_SWAP_MODE);

            _arDmxDesc[u1CompId].u4IvLen = (UINT32)prKeyInfo->u_key_info.t_aes_128_cbc_key.z_iv_len;
            ASSERT((UINT32)_arDmxDesc[u1CompId].u4IvLen <= 16);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Iv, (UINT32 *)au4Iv, prKeyInfo->u_key_info.t_aes_128_cbc_key.z_iv_len/4, DMX_CI_AES_IV_SWAP_MODE);
        }
        else
        {
            fgRet = FALSE;  //Not implemented yet
            break;
        }

        fgRet = TRUE;
        break;

    case DESC_SET_TYPE_EVEN_KEY:
        _arDmxDesc[u1CompId].fgEven = TRUE;
        prKeyInfo = (DESC_KEY_INFO_T*)pv_set_info;
        if(prKeyInfo->e_descrambler_alg == DESC_ALG_DES_ECB)
        {
            _arDmxDesc[u1CompId].u4KeyLen = (UINT32)prKeyInfo->u_key_info.t_des_ecb_key.z_key_len;
            ASSERT(_arDmxDesc[u1CompId].u4KeyLen <= 24);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Key, (UINT32 *)prKeyInfo->u_key_info.t_des_ecb_key.pui1_key, _arDmxDesc[u1CompId].u4KeyLen, DMX_CI_DES_KEY_SWAP_MODE);
        }
        else if(prKeyInfo->e_descrambler_alg == DESC_ALG_AES_128_CBC)
        {
            UINT32 au4Key[4];
            UINT32 au4Iv[4];
            x_memcpy(au4Key, prKeyInfo->u_key_info.t_aes_128_cbc_key.pui1_key, 16);
            x_memcpy(au4Iv, prKeyInfo->u_key_info.t_aes_128_cbc_key.pui1_iv, 16);

            _arDmxDesc[u1CompId].u4KeyLen = (UINT32)prKeyInfo->u_key_info.t_aes_128_cbc_key.z_key_len;
            ASSERT(_arDmxDesc[u1CompId].u4KeyLen <= 32);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Key, (UINT32 *)au4Key, _arDmxDesc[u1CompId].u4KeyLen/4, DMX_CI_AES_KEY_SWAP_MODE);

            _arDmxDesc[u1CompId].u4IvLen = (UINT32)prKeyInfo->u_key_info.t_aes_128_cbc_key.z_iv_len;
            ASSERT((UINT32)_arDmxDesc[u1CompId].u4IvLen <= 16);
            DataSwap((UINT32 *)_arDmxDesc[u1CompId].au4Iv, (UINT32 *)au4Iv, prKeyInfo->u_key_info.t_aes_128_cbc_key.z_iv_len/4, DMX_CI_AES_IV_SWAP_MODE);
        }
        else
        {
            fgRet = FALSE;  //Not implemented yet
            break;
        }

        fgRet = TRUE;
        break;
#else
    case DESC_SET_TYPE_KEY_INDEX:
        fgRet = _MwDmxDescSetKeyIndex(u1CompId, pv_set_info);
        break;
#endif//CC_CI_PLUS_TZ_SUPPORT
    default:
        break;
    }

    return (fgRet ? RMR_OK : RMR_DRV_INV_SET_INFO);
}

BOOL _MW_DMX_DESC_Init(void)
{
    ID_IND_T at_id_ind[DMX_DESC_COMP_NUM];
    DRV_COMP_REG_T t_comp_id;
    DRV_COMP_FCT_TBL_T t_comp_fct_tbl;
    UINT32 i, u4Flags;
    INT32 i4Ret;

    for(i=0; i<DMX_DESC_COMP_NUM; i++)
    {
        _arDmxDesc[i].fgConnented = FALSE;
        _arDmxDesc[i].u2PidNum = 0x1FFF;
        _arDmxDesc[i].fgEnable = FALSE;
        _arDmxDesc[i].rDescSource = DESC_SOURCE_NONE;
        _arDmxDesc[i].rDescAlg = DESC_ALG_NONE;

    }

    //---------------------------------------------------------
    // Register Descrambler components
    //---------------------------------------------------------
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmxDesc_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmxDesc_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmxDesc_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmxDesc_Set;
    t_comp_id.e_type = DRVT_DESCRAMBLER;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_DESC_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind;

    for (i = 0; i < DMX_DESC_COMP_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = i;
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    i4Ret = x_rm_reg_comp(&t_comp_id,
                         1,                     // in port num
                         1,                     // out port num
                         NULL,                  // component name
                         u4Flags,               // flags
                         &t_comp_fct_tbl,
                         NULL,
                         0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

    return TRUE;
}
#endif  // CI_PLUS_SUPPORT

//-----------------------------------------------------------------------------
/** MW_DMX_Init
 *  Initialize demux driver for middleware interface
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL MW_DMX_Init(void)
{
    ID_IND_T at_id_ind[DMX_PES_COMP_NUM];
    DRV_COMP_REG_T t_comp_id;
    DRV_COMP_FCT_TBL_T t_comp_fct_tbl;
    UINT32 i, u4Flags;
    INT32 i4Ret;
	static BOOL _fgInit = FALSE;

    // Initlialize driver
    if (MTDMX_Init() != MTR_OK)
    {
        return FALSE;
    }

    if (MTDMX_MWIF_Init() != MTR_OK)
    {
        return FALSE;
    }

	if(!_fgInit)
    {
        if(!_DmxMwCreateSemaphore(&dmxmw_mutex))
        {
            return FALSE;
        }
        _fgInit = TRUE;
    }

	_DmxMwLockSemaphore(&dmxmw_mutex);
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        _arNotifyInfo[i].pf_demux_nfy = NULL;
        _arNotifyInfo[i].pv_tag = NULL;
        _arDmxInfo[i].eConType = MW_DMX_CONN_TYPE_NONE;
        _arDmxInfo[i].u4RMType = 0;
        _arDmxInfo[i].fgValid = FALSE;
		_arDmxInfo[i].u1TsIndex = 0; /*set	inavailble value*/
        x_memset(&_arDestInfo[i], 0, sizeof(_arDestInfo[i]));
    }
	_DmxMwUnlockSemaphore(&dmxmw_mutex);

//#ifdef ENABLE_MULTIMEDIA
    _MW_DMX_SetCacheOn(TRUE);
//#endif  // ENABLE_MULTIMEDIA

    // Register PES components
    //
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmx_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmx_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmx_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmx_Set;
    t_comp_id.e_type = DRVT_DEMUX_TS_PES_PACKET;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_PES_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind;

    for (i = 0; i < DMX_PES_COMP_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = (UINT16)(DMX_PES_COMP_ID_START + i);
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }

    //u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;
    u4Flags = 0;

    i4Ret = x_rm_reg_comp(&t_comp_id,
                         1,                     // in port num
                         1,                     // out port num
                         NULL,                  // component name
                         u4Flags,               // flags
                         &t_comp_fct_tbl,
                         NULL,
                         0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

#if 0
    // Register PES connection list
    t_prcd_comp_id.e_type = MW_REG_TYPE_DMX_TS1;
    t_prcd_comp_id.e_id_type = ID_TYPE_IND;
    t_prcd_comp_id.u.t_ind.ui2_id = MW_REG_ID_DMX_TS1;
    t_prcd_comp_id.u.t_ind.ui1_port = ALL_PORTS;
    t_prcd_comp_id.u.t_ind.pv_tag = NULL;

    i4Ret = x_rm_reg_conn_list(&t_prcd_comp_id,
                               &t_comp_id,
                               1,
                               CONN_DIR_OUT_TO_INP,
                               FALSE );
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }
#endif  // 0

    // Register PCR components
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmx_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmx_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmx_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmx_Set;

    t_comp_id.e_type = DRVT_DEMUX_TS_PCR;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_PCR_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind;

    for (i = 0; i < DMX_PCR_COMP_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = (UINT16)(DMX_PCR_COMP_ID_START + i);
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }

    i4Ret = x_rm_reg_comp(&t_comp_id,
                          1,                    // in port num
                          1,                    // out port num
                          NULL,                 // component name
                          u4Flags,              // flags
                          &t_comp_fct_tbl,
                          NULL,
                          0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

#if 0
    // Register PCR connection list
    t_prcd_comp_id.e_type =  MW_REG_TYPE_DMX_TS1;
    t_prcd_comp_id.e_id_type = ID_TYPE_IND;
    t_prcd_comp_id.u.t_ind.ui2_id = MW_REG_ID_DMX_TS1;
    t_prcd_comp_id.u.t_ind.ui1_port = ALL_PORTS;
    t_prcd_comp_id.u.t_ind.pv_tag = NULL;

    i4Ret = x_rm_reg_conn_list(&t_prcd_comp_id,
                               &t_comp_id,
                               1,
                               CONN_DIR_OUT_TO_INP,
                               FALSE);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }
#endif  // 0

#if 1
    // Register PSI components
    if (!_MW_DMX_PSI_Init())
    {
        return FALSE;
    }
#endif  // 1

#if 1
    // Register PES components
    if (!_MW_DMX_PES_Init())
    {
        return FALSE;
    }
#endif  // 1

#ifdef ENABLE_MULTIMEDIA
    // Register MM components
    if (!_MW_DMX_MM_Init())
    {
        return FALSE;
    }
#endif  // ENABLE_MULTIMEDIA

#ifdef CI_PLUS_SUPPORT
    // Initialize descrambler
    if(!_MW_DMX_DESC_Init())
    {
        return FALSE;
    }
#endif  // CI_PLUS_SUPPORT

    if (!_MW_DMX_CRYPT_Init())
    {
        return FALSE;
    }

    return TRUE;
}

//Demux D-Interface
static INT32 d_custom_demux_set_stop(VOID * pv_set_info, SIZE_T z_size)
{
    MT_RESULT_T eRet;
    BOOL fgStop;
    
    UNUSED(z_size);
    fgStop = *((BOOL *)pv_set_info);
    if(fgStop)
    {
        eRet = MTDMX_Stop();
    }
    else
    {
        eRet = MTDMX_Start();
    }

    if(eRet != MTR_OK)
    {
        return -1;
    }
    return 0;
}

INT32 d_custom_demux_set (
    DRV_CUSTOM_DEMUX_SET_TYPE_T  e_demux_set_type,
    VOID*                        pv_set_info,
    SIZE_T                       z_size)
{
    INT32 i4Ret;
    switch(e_demux_set_type)
    {
        case DRV_CUSTOM_DEMUX_TYPE_STOP:
            i4Ret = d_custom_demux_set_stop(pv_set_info,z_size);
            break;
        default:
            i4Ret = -1;
            break;
    }
    return i4Ret;
}

