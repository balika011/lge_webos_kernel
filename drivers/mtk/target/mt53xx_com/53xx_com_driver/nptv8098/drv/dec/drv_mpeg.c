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
 * $RCSfile: drv_mpeg.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_mpeg.c
 *  Brief of file drv_mpeg.c.
 *  Details of file drv_mpeg.c (optional).
 */

#ifdef CC_SECOND_B2R_SUPPORT
#if !defined(CC_DISABLE_TWO_B2R)
#define SUPPORT_2_B2R
#endif
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


#include "vdp_drvif.h"
#include "b2r_drvif.h"
#include "fbm_drvif.h"
#include "b2r_if.h"
#include "nptv_debug.h"

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "general.h"
#include "hw_vdoin.h"
#include "video_timing.h"
#include "video_def.h"
#include "drv_mpeg.h"
#include "x_assert.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// function forward declarations
//-----------------------------------------------------------------------------

void vMpegInit(void);

void vMpegInitB2rConf(void);

void vMpegModeChg(UCHAR ucVdpId);

void vMpegModeDetDone(UCHAR ucVdpId);

void vMpegModeChg(UCHAR ucVdpId);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

// B2R_NS is 1 now. Plus 1 for B2R SD
static VDP_CONF_T* _prB2rConf[B2R_NS+1];
static B2R_PRM_T* _prB2rPrm[B2R_NS+1];


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
#define API_LOCK() \
    CRIT_STATE_T rCritState;\
    rCritState = x_crit_start()

#define API_UNLOCK() x_crit_end(rCritState)

//-----------------------------------------------------------------------------
/** Brief of vMpegInitB2rConf.
 */
//-----------------------------------------------------------------------------
void vMpegInitB2rConf(void)
{
    UCHAR ucVdpId;
    UCHAR ucB2rId;

    API_LOCK();
    for (ucB2rId = 0; ucB2rId < B2R_NS; ucB2rId++)
    {
        ucVdpId = VDP_GetVdpId(ucB2rId);  
        _prB2rConf[ucB2rId] = VDP_GetConf(ucVdpId);
        _prB2rPrm[ucB2rId] = VDP_GetVdpPrm(ucB2rId);
    }
    API_UNLOCK();

    /* Set B2R_2 to NULL */
    _prB2rConf[B2R_NS] = NULL;
}

//-----------------------------------------------------------------------------
/** Brief of vMpegInit.
 */
//-----------------------------------------------------------------------------
void vMpegInit(void)
{
    ASSERT((UINT32)SV_VDO_NOSIGNAL == (UINT32)VDP_STATUS_NOSIGNAL);
    ASSERT((UINT32)SV_VDO_NOSUPPORT == (UINT32)VDP_STATUS_NOSUPPORT);
    ASSERT((UINT32)SV_VDO_UNKNOWN == (UINT32)VDP_STATUS_UNKNOWN);
    ASSERT((UINT32)SV_VDO_STABLE == (UINT32)VDP_STATUS_STABLE);

    vMpegInitB2rConf();
}

//-----------------------------------------------------------------------------
/** Brief of vMpegSdConnect.
 */
//-----------------------------------------------------------------------------
void vMpegSdConnect(UINT8 bchannel, UINT8 fgIsOn) 
{
    if(fgIsOn == SV_ON)
    {
        if(bchannel == SV_VP_MAIN)
        {
            vClrMainFlg(MAIN_FLG_MODE_CHG | MAIN_FLG_MODE_DET_DONE);

            vVDOINIrqOn(MSK_MAIN_DET);
            _rMpegSDStat.bIsMain = SV_TRUE;
            _rMpegSDStat.bIsPip = SV_FALSE;
        }
        else
        {
            vClrPipFlg(PIP_FLG_MODE_CHG | PIP_FLG_MODE_DET_DONE);
        
            vVDOINIrqOn(MSK_PIP_DET);
            _rMpegSDStat.bIsMain = SV_FALSE;
            _rMpegSDStat.bIsPip = SV_TRUE;
        }
    }
    else
    {
        if(bchannel == SV_VP_MAIN)
        {
            vVDOINIrqOff(MSK_MAIN_DET);
            _rMpegSDStat.bIsMain = SV_FALSE;
            _rMpegSDStat.bIsPip = SV_FALSE;
        }
        else
        {
            vVDOINIrqOff(MSK_PIP_DET);
            _rMpegSDStat.bIsMain = SV_FALSE;
            _rMpegSDStat.bIsPip = SV_FALSE;
        }
    }

    LOG(9, "vMpegSdConnect\n");
}

//-----------------------------------------------------------------------------
/** Brief of vMpegHdConnect.
 */
//-----------------------------------------------------------------------------
void vMpegHdConnect(UINT8 bchannel, UINT8 fgIsOn) 
{
    if(fgIsOn == SV_ON)
    {
        if(bchannel == SV_VP_MAIN)
        {
            vClrMainFlg(MAIN_FLG_MODE_CHG | MAIN_FLG_MODE_DET_DONE);

            vVDOINIrqOn(MSK_MAIN_DET);
            _rMpegHDStat.bIsMain = SV_TRUE;
            _rMpegHDStat.bIsPip = SV_FALSE;
        }
        else
        {
            vClrPipFlg(PIP_FLG_MODE_CHG | PIP_FLG_MODE_DET_DONE);

            vVDOINIrqOn(MSK_PIP_DET);
            _rMpegHDStat.bIsMain = SV_FALSE;
            _rMpegHDStat.bIsPip = SV_TRUE;
        }
    }
    else
    {
        if(bchannel == SV_VP_MAIN)
        {
            vVDOINIrqOff(MSK_MAIN_DET);
            _rMpegHDStat.bIsMain = SV_FALSE;
            _rMpegHDStat.bIsPip = SV_FALSE;
        }
        else
        {
            vVDOINIrqOff(MSK_PIP_DET);
            _rMpegHDStat.bIsMain = SV_FALSE;
            _rMpegHDStat.bIsPip = SV_FALSE;
        }
    }

    LOG(9, "vMpegHdConnect\n");
}

//-----------------------------------------------------------------------------
/** Brief of vMpegModeChg.
 */
//-----------------------------------------------------------------------------
void vMpegModeChg(UCHAR ucVdpId)
{
#ifdef __MODEL_slt__
    if ((ucVdpId == VDP_1) && !_rMChannel.bIsFreeze)
#else
    if (ucVdpId == VDP_1)
#endif
    {
        vSetMainFlg(MAIN_FLG_MODE_CHG);
    }
#ifdef __MODEL_slt__
    else if ((ucVdpId == VDP_2) && !_rPChannel.bIsFreeze)
#else
    else
#endif
    {
        vSetPipFlg(PIP_FLG_MODE_CHG);
    }
}

//-----------------------------------------------------------------------------
/** Brief of vMpegModeDetDone.
 */
//-----------------------------------------------------------------------------
void vMpegModeDetDone(UCHAR ucVdpId)
{
    if (ucVdpId == VDP_1)
    {
        vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
    }
    else
    {
        vSetPipFlg(PIP_FLG_MODE_DET_DONE);
    }
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdSigStatus.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdSigStatus(UINT8 bPath)
{
    UINT8 ret =  (UINT8) SV_VDO_NOSIGNAL;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        VDP_CONF_T* prVdp = VDP_GetConf(bPath);
		if (prVdp && prVdp->ucB2rId == B2R_1)
		{
			ret =  prVdp->ucStatus;
		}
    }
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdSigStatus.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdSigStatus(UINT8 bPath)
{
    UINT8 ret =  (UINT8) SV_VDO_NOSIGNAL;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        VDP_CONF_T* prVdp = VDP_GetConf(bPath);
		if (prVdp && prVdp->ucB2rId == B2R_2)
		{
			ret =  prVdp->ucStatus;
		}
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegHdInputWidth.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegHdInputWidth(void) 
{
    UINT16 ret =  0;
    API_LOCK();

    if (_prB2rConf[B2R_1])
    {
        // B2R width issue
        // Work-Around # 1, B2R/PSW Width 64 byte alignment
        ret =  _prB2rConf[B2R_1]->rOutInfo.u4Width;
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegSdInputWidth.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegSdInputWidth(void) 
{
    UINT16 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        // B2R width issue
        // Work-Around # 1, B2R/PSW Width 64 byte alignment
        ret =  _prB2rConf[B2R_2]->rOutInfo.u4Width;
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegHdInputValidDataWidth.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegHdInputValidDataWidth(void) 
{
    UINT16 ret =  0;
    UINT32 u4ResizeWidth = 0;
    UINT32 u4ResizeHeight = 0;
    UINT32 u4ValidB2R = 0;
    
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  _prB2rConf[B2R_1]->u4SrcWidth;
        u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R && VDP_GetSeamlessResizeSize(B2R_1, &u4ResizeWidth, &u4ResizeHeight))
    {
        ret =  (UINT16)u4ResizeWidth;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegSdInputValidDataWidth.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegSdInputValidDataWidth(void) 
{
    UINT16 ret =  0;

#ifdef SUPPORT_2_B2R
    UINT32 u4ResizeWidth = 0;
    UINT32 u4ResizeHeight = 0;
    UINT32 u4ValidB2R = 0;

    API_LOCK();
    if (_prB2rConf[B2R_2])
    {
      ret =  _prB2rConf[B2R_2]->u4SrcWidth;
      u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R && VDP_GetSeamlessResizeSize(B2R_2, &u4ResizeWidth, &u4ResizeHeight))
    {
        ret =  (UINT16)u4ResizeWidth;
    }
#endif


    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegHdInputHeight.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegHdInputHeight(void) 
{
    UINT16 ret =  0;
    UINT32 u4ResizeWidth = 0;
    UINT32 u4ResizeHeight = 0;
    UINT32 u4ValidB2R = 0;

    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  _prB2rConf[B2R_1]->u4SrcHeight;
        u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R && VDP_GetSeamlessResizeSize(B2R_1, &u4ResizeWidth, &u4ResizeHeight))
    {
        ret =  (UINT16)u4ResizeHeight;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegSdInputHeight.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegSdInputHeight(void) 
{
    UINT16 ret =  0;
#ifdef SUPPORT_2_B2R
    UINT32 u4ResizeWidth = 0;
    UINT32 u4ResizeHeight = 0;
    UINT32 u4ValidB2R = 0;

    API_LOCK();
    if (_prB2rConf[B2R_2])
    {
        ret =  _prB2rConf[B2R_2]->u4SrcHeight;
        u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R && VDP_GetSeamlessResizeSize(B2R_2, &u4ResizeWidth, &u4ResizeHeight))
    {
        ret =  (UINT16)u4ResizeHeight;
    }
    
#endif

    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdRefreshRate.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdRefreshRate(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  _prB2rConf[B2R_1]->rOutInfo.ucFrameRate;
    }
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdRefreshRate.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdRefreshRate(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        // B2R width issue
        ret =  _prB2rConf[B2R_2]->rOutInfo.ucFrameRate;
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdRefreshRate.
 */
//-----------------------------------------------------------------------------
UINT32 u4MpegHdPreciseRefreshRate(void)
{
    UINT32 ret =  0;
    UINT32 u4FrameRate = 0;
    UCHAR ucInputFrameRate = 0;
    UINT32 u4ValidB2R = 0;

    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ucInputFrameRate = _prB2rConf[B2R_1]->rOutInfo.ucInputFrameRate;
        u4FrameRate = (UINT32)(_prB2rConf[B2R_1]->rOutInfo.ucFrameRate);
        u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R)
    {
        //ret =  _prB2rConf[B2R_1]->rOutInfo.ucFrameRate;
        if( (ucInputFrameRate == MPEG_FRAME_RATE_24)
            || (ucInputFrameRate == MPEG_FRAME_RATE_25)
            || (ucInputFrameRate == MPEG_FRAME_RATE_30)
            || (ucInputFrameRate == MPEG_FRAME_RATE_50)
            || (ucInputFrameRate == MPEG_FRAME_RATE_60)
            || (ucInputFrameRate == MPEG_FRAME_RATE_UNKNOWN))
        {
            u4FrameRate = (u4FrameRate * 1000) ;
        }
        else
        {
            u4FrameRate = (u4FrameRate * 1000000) / 1001 ;
        }
        ret =  u4FrameRate;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdRefreshRate.
 */
//-----------------------------------------------------------------------------
UINT32 u4MpegSdPreciseRefreshRate(void)
{
    UINT32 ret =  0;
#ifdef SUPPORT_2_B2R
    UINT32 u4FrameRate = 0;
    UCHAR ucInputFrameRate = 0;
    UINT32 u4ValidB2R = 0;

    API_LOCK();
    if (_prB2rConf[B2R_2])
    {
        ucInputFrameRate = _prB2rConf[B2R_2]->rOutInfo.ucInputFrameRate;
        u4FrameRate = (UINT32)(_prB2rConf[B2R_2]->rOutInfo.ucFrameRate);
        u4ValidB2R = 1;
    }
    API_UNLOCK();

    if (u4ValidB2R)
    {
        // B2R width issue
        //ret =  _prB2rConf[B2R_2]->rOutInfo.ucFrameRate;
        //ret =  _prB2rConf[B2R_1]->rOutInfo.ucFrameRate;
        
        if( (ucInputFrameRate == MPEG_FRAME_RATE_24)
            || (ucInputFrameRate == MPEG_FRAME_RATE_25)
            || (ucInputFrameRate == MPEG_FRAME_RATE_30)
            || (ucInputFrameRate == MPEG_FRAME_RATE_50)
            || (ucInputFrameRate == MPEG_FRAME_RATE_60)
            || (ucInputFrameRate == MPEG_FRAME_RATE_UNKNOWN))
        {
            u4FrameRate = ((UINT32)u4FrameRate * 1000) ;
        }
        else
        {
            u4FrameRate = (u4FrameRate * 1000000) / 1001 ;
        }        
        ret =  u4FrameRate;
    }
#endif
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdInterlace.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdInterlace(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        if(_prB2rPrm[B2R_1])
        {
#ifdef CC_B2R_3D_MVC_INTERLACE_SUPPORT        
            if(_prB2rPrm[B2R_1].fgB2R3DEnable && !_prB2rConf[B2R_1]->rOutInfo.ucPrg)
            {
                ret =  SV_FALSE;
            }
            else
#endif            
            {
                ret =  (_prB2rConf[B2R_1]->rOutInfo.ucPrg ? SV_FALSE : SV_TRUE);
            }
        }     
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdInterlace.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdInterlace(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
#ifdef CC_B2R_3D_MVC_INTERLACE_SUPPORT        
        if(_prB2rPrm[B2R_1].fgB2R3DEnable && !_prB2rConf[B2R_1]->rOutInfo.ucPrg)
        {
            ret =  SV_FALSE;
        }
        else
#endif
        {
            ret =  (_prB2rConf[B2R_2]->rOutInfo.ucPrg ? SV_FALSE : SV_TRUE);
        }            
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegHdHTotal.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegHdHTotal(void)
{
    UINT16 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  _prB2rConf[B2R_1]->rOutInfo.u4TotalWidth;
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegSdHTotal.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegSdHTotal(void)
{
    UINT16 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  _prB2rConf[B2R_2]->rOutInfo.u4TotalWidth;
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegHdVTotal.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegHdVTotal(void)
{
    UINT16 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  _prB2rConf[B2R_1]->rOutInfo.u4TotalHeight;
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u2MpegSdVTotal.
 */
//-----------------------------------------------------------------------------
UINT16 u2MpegSdVTotal(void)
{
    UINT16 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  _prB2rConf[B2R_2]->rOutInfo.u4TotalHeight;
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdTiming.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdTiming(void)
{
    UINT8 ret =  MODE_525I;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
    #if 0 /* Consider DTV SCART out, ucPmxMode will be reset */
        switch (_prB2rConf[B2R_1]->rOutInfo.ucPmxMode)
        {
            case VDP_B2R_MODE_480I:
                ret =  MODE_525I;
            
            case VDP_B2R_MODE_576I:
                ret =  MODE_625I;

            case VDP_B2R_MODE_480P_24:
                ret =  MODE_480P_24;

            case VDP_B2R_MODE_480P_30:
                ret =  MODE_480P_30;

            case VDP_B2R_MODE_480P:
                ret =  MODE_480P;

            case VDP_B2R_MODE_576P_25:
                ret =  MODE_576P_25;

            case VDP_B2R_MODE_576P:
                ret =  MODE_576P;

            case VDP_B2R_MODE_720P_24:
                ret =  MODE_720p_24;

            case VDP_B2R_MODE_720P_25:
                ret =  MODE_720p_25;

            case VDP_B2R_MODE_720P_30:
                ret =  MODE_720p_30;

            case VDP_B2R_MODE_720P_50:
                ret =  MODE_720p_50;

            case VDP_B2R_MODE_720P:
                ret =  MODE_720p_60;

            case VDP_B2R_MODE_1080P_24:
                ret =  MODE_1080p_24;

            case VDP_B2R_MODE_1080P_25:
                ret =  MODE_1080p_25;

            case VDP_B2R_MODE_1080P_30:
                ret =  MODE_1080p_30;

            case VDP_B2R_MODE_1080P_50:
                ret =  MODE_1080p_50;

            case VDP_B2R_MODE_1080P:
                ret =  MODE_1080p_60;

            case VDP_B2R_MODE_1080I_50:
                ret =  MODE_1080i_50;

            case VDP_B2R_MODE_1080I:
            default:
                ret =  MODE_1080i;
        }
    #else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 480) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0))
            {
                ret =  MODE_525I;
            }
            else 
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 576) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0))
            {
                ret =  MODE_625I;
            }
            else 
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 480) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 24))
            {
                ret =  MODE_480P_24;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 480) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 30))
            {
                ret =  MODE_480P_30;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 480) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 60))
            {
                ret =  MODE_480P;
            }

            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 576) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 25))
            {
                ret =  MODE_576P_25;
            }

            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 576) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 50))
            {
                ret =  MODE_576P;
            }

            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 720) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 24))
            {
                ret =  MODE_720p_24;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 720) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 25))
            {
                ret =  MODE_720p_25;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 720) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 30))
            {
                ret =  MODE_720p_30;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 720) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 50))
            {
                ret =  MODE_720p_50;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 720) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 60))
            {
                ret =  MODE_720p_60;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 24))
            {
                ret =  MODE_1080p_24;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 25))
            {
                ret =  MODE_1080p_25;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 30))
            {
                ret =  MODE_1080p_30;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 50))
            {
                ret =  MODE_1080p_50;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 60))
            {
                ret =  MODE_1080p_60;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 48))
            {
                ret =  MODE_3D_1080p_24_FP;
            }            
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 50))
            {
                ret =  MODE_1080i_50;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 60))
            {
                ret =  MODE_1080i;
            }
            else
            if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 48))
            {
                ret =  MODE_1080i_48;
            }
            else if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 100))
            {
                ret =  MODE_3D_1080I_50_FP;
            }            
            else if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 1080) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 0) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 120))
            {
                ret =  MODE_3D_1080I_60_FP;
            }
#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER            
            else if((_prB2rConf[B2R_1]->rOutInfo.u4Height == 2160) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucPrg == 1) &&
               (_prB2rConf[B2R_1]->rOutInfo.ucFrameRate == 24))
            {
                return MODE_2160p_24;
            }   
#endif 
            else
            {
                ret =  MODE_1080i;
            }
    #endif
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdTiming.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdTiming(void)
{
    UINT8 ret =  MODE_525I;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
#ifndef CC_SECOND_B2R_SUPPORT /* Consider DTV SCART out, ucPmxMode will be reset */
        switch (_prB2rConf[B2R_2]->rOutInfo.ucPmxMode)
        {
            case VDP_B2R_MODE_480I:
                ret =  MODE_525I;
            
            case VDP_B2R_MODE_576I:
                ret =  MODE_625I;

            case VDP_B2R_MODE_480P_24:
                ret =  MODE_480P_24;

            case VDP_B2R_MODE_480P_30:
                ret =  MODE_480P_30;

            case VDP_B2R_MODE_480P:
                ret =  MODE_480P;

            case VDP_B2R_MODE_576P_25:
                ret =  MODE_576P_25;

            case VDP_B2R_MODE_576P:
                ret =  MODE_576P;

            case VDP_B2R_MODE_720P_24:
                ret =  MODE_720p_24;

            case VDP_B2R_MODE_720P_25:
                ret =  MODE_720p_25;

            case VDP_B2R_MODE_720P_30:
                ret =  MODE_720p_30;

            case VDP_B2R_MODE_720P_50:
                ret =  MODE_720p_50;

            case VDP_B2R_MODE_720P:
                ret =  MODE_720p_60;

            case VDP_B2R_MODE_1080P_24:
                ret =  MODE_1080p_24;

            case VDP_B2R_MODE_1080P_25:
                ret =  MODE_1080p_25;

            case VDP_B2R_MODE_1080P_30:
                ret =  MODE_1080p_30;

            case VDP_B2R_MODE_1080P_50:
                ret =  MODE_1080p_50;

            case VDP_B2R_MODE_1080P:
                ret =  MODE_1080p_60;

            case VDP_B2R_MODE_1080I_50:
                ret =  MODE_1080i_50;

            case VDP_B2R_MODE_1080I:
            default:
                ret =  MODE_1080i;
        }
#else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 480) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 0))
        {
            ret =  MODE_525I;
        }
        else 
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 576) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 0))
        {
            ret =  MODE_625I;
        }
        else 
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 480) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 24))
        {
            ret =  MODE_480P_24;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 480) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 30))
        {
            ret =  MODE_480P_30;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 480) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 60))
        {
            ret =  MODE_480P;
        }

        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 576) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 25))
        {
            ret =  MODE_576P_25;
        }

        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 576) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 50))
        {
            ret =  MODE_576P;
        }

        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 720) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 24))
        {
            ret =  MODE_720p_24;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 720) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 25))
        {
            ret =  MODE_720p_25;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 720) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 30))
        {
            ret =  MODE_720p_30;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 720) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 50))
        {
            ret =  MODE_720p_50;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 720) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 60))
        {
            ret =  MODE_720p_60;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 24))
        {
            ret =  MODE_1080p_24;
    }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 25))
        {
            ret =  MODE_1080p_25;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 30))
        {
            ret =  MODE_1080p_30;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 50))
        {
            ret =  MODE_1080p_50;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 1) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 60))
        {
            ret =  MODE_1080p_60;
        }
        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 0) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 50))
        {
            ret =  MODE_1080i_50;
        }

        else
        if((_prB2rConf[B2R_2]->rOutInfo.u4Height == 1080) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucPrg == 0) &&
           (_prB2rConf[B2R_2]->rOutInfo.ucFrameRate == 60))
        {
            ret =  MODE_1080i;
        }
        else
        {
            ret =  MODE_1080i;
        }
#endif
    }

#endif
    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegHdClock
 */
//-----------------------------------------------------------------------------

UINT8 u1MpegHdClock(void)
{
    UINT8 ret =  B2R_CLK_MODE_27;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        switch (_prB2rConf[B2R_1]->rOutInfo.u4ClkMode)
        {            
            case VDP_B2R_CLK_74_250:
            case VDP_B2R_CLK_74_176:            
                ret =  B2R_CLK_MODE_74;

            case VDP_B2R_CLK_148_500:
            case VDP_B2R_CLK_148_352:
                ret =  B2R_CLK_MODE_148;

            case VDP_B2R_CLK_27_000:
            case VDP_B2R_CLK_27_027:            
            default:            
                ret =  B2R_CLK_MODE_27;
        }
    }

    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegHdClock
 */
//-----------------------------------------------------------------------------

UINT8 u1MpegSdClock(void)
{
    UINT8 ret =  B2R_CLK_MODE_27;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        switch (_prB2rConf[B2R_2]->rOutInfo.u4ClkMode)
        {            
            case VDP_B2R_CLK_74_250:
            case VDP_B2R_CLK_74_176:            
                ret =  B2R_CLK_MODE_74;

            case VDP_B2R_CLK_148_500:
            case VDP_B2R_CLK_148_352:
                ret =  B2R_CLK_MODE_148;

            case VDP_B2R_CLK_27_000:
            case VDP_B2R_CLK_27_027:            
            default:            
                ret =  B2R_CLK_MODE_27;
        }
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u4MpegHdQuality.
 */
//-----------------------------------------------------------------------------
UINT32 u4MpegHdQuality(void)
{
    UINT32 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  (_prB2rConf[B2R_1]->u4QpAvg);
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u4MpegSdQuality.
 */
//-----------------------------------------------------------------------------
UINT32 u4MpegSdQuality(void)
{
    UINT32 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  (_prB2rConf[B2R_2]->u4QpAvg);
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdColorSpace709.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdColorSpace709(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        if (_prB2rConf[B2R_1]->ucColourPrimary == MPEG_COLOR_PRIMARY_709)
        {
            ret =  1;
        }
    }
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdColorSpace709.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdColorSpace709(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        if (_prB2rConf[B2R_2]->ucColourPrimary == MPEG_COLOR_PRIMARY_709)
        {
            ret =  1;
        }
    }
#endif
    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegHdColourPrimary.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdColourPrimary(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  (_prB2rConf[B2R_1]->ucColourPrimary);
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdColourPrimary.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdColourPrimary(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  (_prB2rConf[B2R_2]->ucColourPrimary);
    }
#endif
    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegSdTransCharacter.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdTransCharacter(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  (_prB2rConf[B2R_1]->ucTransCharacter);
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdTransCharacter.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdTransCharacter(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  (_prB2rConf[B2R_2]->ucTransCharacter);
    }
#endif
    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegHdMatrixCoeff.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdMatrixCoeff(void)
{
    UINT8 ret =  0;
    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  (_prB2rConf[B2R_1]->ucMatrixCoeff);
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdMatrixCoeff.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdMatrixCoeff(void)
{
    UINT8 ret =  0;

    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  (_prB2rConf[B2R_2]->ucMatrixCoeff);
    }
#endif
    API_UNLOCK();
    return ret;
}


//-----------------------------------------------------------------------------
/** Brief of u1MpegHdColourPrimary.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegHdColorDescrip(void)
{
    UINT8 ret =  0;

    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        ret =  (_prB2rConf[B2R_1]->ucColorDescrip);
    }

    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegSdColorDescrip.
 */
//-----------------------------------------------------------------------------
UINT8 u1MpegSdColorDescrip(void)
{
    UINT8 ret =  0;
    API_LOCK();
#ifdef SUPPORT_2_B2R
    if (_prB2rConf[B2R_2])
    {
        ret =  (_prB2rConf[B2R_2]->ucColorDescrip);
    }
#endif
    API_UNLOCK();
    return ret;
}

//-----------------------------------------------------------------------------
/** Brief of u1MpegHdHdeRatio.
 * 10 bit base assumption
 */
//-----------------------------------------------------------------------------
UINT32 u4MpegHdHdeRatio(void)
{
    UINT32 ret =  1024;

    API_LOCK();
    if (_prB2rConf[B2R_1])
    {
        if(_prB2rConf[B2R_1]->rOutInfo.ucDoubleClk == 0)
        {
            ret =  1024;
        }
        else if(_prB2rConf[B2R_1]->rOutInfo.ucDoubleClk == 1)
        {
            ret =  512;
        }
        else if(_prB2rConf[B2R_1]->rOutInfo.ucDoubleClk == 2)
        {
            ret =  410;
        }
    }

    API_UNLOCK();
    return ret;
}

VOID vMpegGet3DInfo(UINT8 u1B2RId,B2R_3D_INFO* prB2R3DInfo)
{
    API_LOCK();
    if (_prB2rPrm[u1B2RId])
    {
        prB2R3DInfo->fgB2R3DEnable = (_prB2rPrm[u1B2RId]->fgB2R3DEnable && (!_prB2rPrm[u1B2RId]->fgB2RForce2D));
        prB2R3DInfo->u1B2R3DType = _prB2rPrm[u1B2RId]->u1B2R3DType;
    }
    API_UNLOCK();
}

#ifdef CC_SUPPORT_4K2K
#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER
extern UINT8 fgIs4k2kVideohoto  ;
BOOL fgMpegHdIs4k2kVdoPhoto(void)
{
	if (_prB2rConf[B2R_1])
	{
		return fgIs4k2kVideohoto ;
	}
		return FALSE;

}
#endif
#endif

