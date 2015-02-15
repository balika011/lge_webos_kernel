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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_mpeg4psr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_mpeg4psr.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "vdec_mpeg4psr.h"
#include "vdec_mpeg4dec.h"
#include "vdec_mpeg4hw.h"
#include "fbm_drvif.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
#include "x_util.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define u4Div2Slash(v1, v2) (((v1)+(v2)/2)/(v2))

//-----------------------------------------------------------------------------
// Static variables
#ifdef VDEC_TIME_PROFILE
HAL_TIME_T rTimeFbS, rTimeFbE, rTimeFbDt;
HAL_TIME_T rTimeSwS, rTimeSwDt,rTimeSwE;
static UINT32 u4SWTimeMicro = 0;
#endif
extern BOOL _bVdecLogSimPatMPEG4;

#ifdef CC_IQ_RASTER_SCAN_MODE
static UINT32 zig_zag_scan[64] =
{
  0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
  12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};
#endif

//-----------------------------------------------------------------------------

#ifdef IC_MPEG4_VERIFICATION
extern UINT32 _u4Dx311PicWidth;
extern UINT32 _u4Dx311PicHeight;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

// M4V Part
static void _PrsM4vHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId);
static void _PrsVOLHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 *pi4RetValue,UCHAR ucEsId);
static void _PrsUsrData(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 *pi4RetValue);
static void _PrsVOPHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue,UCHAR ucEsId);
static void _PrsSeqHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm,UCHAR ucEsId);
static void _SpriteTrajetory(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue);
static void _LoadM4vQMat(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue, BOOL fgIntra);
static INT32 _WarpingMvCode(UINT32* pu4BitCount);
static UINT32 _DmvCodeLength(UINT32* pBitCount);
static void _CalGmcMv(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue);
static void _PrsM4vProfileLevel(UINT32 u4Temp, UCHAR ucEsId);


// H263 Part
static void _PrsH263Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue, UCHAR ucEsId);
static void _PrsH263PicHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 * pi4RetValue, UCHAR ucEsId);

#ifdef CC_IS_MPEG4_POST_MT5363
// Flv Part
static void _PrsFlvHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue, UCHAR ucEsId);
static void _PrsFlvPicHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 * pi4RetValue, UCHAR ucEsId);
#endif

// Divx311 Part
static void _PrsDx311Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue, UCHAR ucEsId);

// Miscs
static UINT32 _GetDivXVersion(UINT8 *u1Buf);
static UINT32 _LsbMask(UINT32 u4ShiftBits);
static void _Align2Byte(UINT32* pu4BitCount);
static BOOL _SrcFmtTbl(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm);
#ifdef CC_IS_MPEG4_POST_MT5363
static BOOL _FlvSrcFmtTbl(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm);
#endif
void _CreateWorkingBuffer(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm);

#ifdef CC_SUPPORT_VDEC_PREPARSE
//pre-parse
void _PrePrsVOLHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue,UCHAR ucEsId);

void _PrePrsH263Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId);

void _PrePrsFlvHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId);

void _PrePrsDx311Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId);

void _VDEC_PrePrsVideoType(VDEC_MPEG4_DEC_PARAM_INFO_T* prVdec, UCHAR ucEsId, INT32* i4RetValue);
#endif
static void _PrsM4vHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    #ifdef IC_MPEG4_VERIFICATION
    UINT32 u4Bits = 0;
	#endif
	UINT32 u4Temp = 0;
	BOOL bUnused = TRUE;
	VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;
	VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;

	*pi4RetValue = E_VDEC_FAIL;

#ifdef IC_MPEG4_VERIFICATION
	while (_VDEC_VldMpeg4RPtr(&u4Bits) <= prDecPrm->rPesInfo.u4FifoEnd)
#endif
	{
		u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
		//LOG(5, "_PrsM4vHdr Start code = %08x\n", u4Temp);
		if ((u4Temp >> 8) == M4V_START_CODE)
		{
				bUnused = FALSE;
		    *pi4RetValue = E_VDEC_PARSE_NOT_READY;
                     if(u4Temp  == M4V_SEQ_START_CODE)
                     {
                         _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);
                        _PrsSeqHdr(prDecPrm,ucEsId);
                     }
		    if ((u4Temp <= M4V_VOL_START_CODE_MAX) &&
		        u4Temp >= M4V_VOL_START_CODE_MIN)
		    {
		        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);
      	                _PrsVOLHdr(prDecPrm, pi4RetValue,ucEsId);
		        _Align2Byte(&prDecPrm->u4BitCount);
		        if (*pi4RetValue == E_VDEC_OK)
		        {
		            *pi4RetValue = E_VDEC_PARSE_NOT_READY;
		        }
#ifdef IC_MPEG4_VERIFICATION
            continue;
#endif
		    }
	        else if (u4Temp == M4V_USER_DATA_START_CODE)
	        {
	        	bUnused = FALSE;
	            _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);
	            _PrsUsrData(prDecPrm, pi4RetValue);
	#ifdef IC_MPEG4_VERIFICATION
	            continue;
	#endif        
	        }
	        else if (u4Temp == M4V_VOP_START_CODE)
	        {
	        	bUnused = FALSE;
               _PrsVOPHdr(prDecPrm, pi4RetValue,ucEsId);
	#ifdef  IC_MPEG4_VERIFICATION
	            //*pi4RetValue = E_VDEC_OK;
	            break;
	#endif
	        }
			else
			{
				LOG(0,"[MPEG4]: Invalid Start Code %d\n",u4Temp);
				*pi4RetValue = E_VDEC_INVALID_START_CODE;
			}
		}
		if(bUnused)
		{
		    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
		    if(prVdecEsInfoKeep->fgVPush) //LG only
		    {
			    prDecPrm->fgNotDisplayDummyFrame = TRUE;
			}

			LOG(5,"Found dummy data in MPEG4 bitstream.\n");
		}
	#ifdef IC_MPEG4_VERIFICATION
		u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
	#endif
	}
}

static void _PrsSeqHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm,UCHAR ucEsId)
{
    UINT32 u4Temp = 0;   
    
    u4Temp =  _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    u4Temp = u4Temp >> 24;
    LOG(6, "ProFile and Level is :%2x\n",u4Temp);    

    _PrsM4vProfileLevel(u4Temp, ucEsId) ;    
}

static void _PrsVOLHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 *pi4RetValue,UCHAR ucEsId)
{
    UINT8 	i;
    UINT8 	u1FbgType;
    UINT8 u1ParWidth = 1;
    UINT8 u1ParHeight = 1;
    UINT32 	u4Temp;
    UINT32 u4FbWidth, u4FbHeight;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;    
    VDEC_M4V_VOL_INFO_T  *prVolInfo;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    #ifdef CC_MAPLE_CUST_DRV
    ENUM_CODEC_INFO_T eCodecType;
    #endif
    LOG(5, "Parse VOL header\n");

   
   *pi4RetValue = E_VDEC_OK;

  /* if(prDecPrm->fgFbgType)
   {
       return ;
   }
   else
   {
       prDecPrm->fgFbgType = TRUE;
   }*/
    
    prDecPrm->u4BitCount = 0;

    *pi4RetValue = E_VDEC_OK;
    prVolInfo = &prDecPrm->rM4vParams.rVol;

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 9);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if (u4Temp & 0x80000000) // is_object_layer_identifier
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->u1VideoObjectLayerVerid = (UINT8)(u4Temp >> 28);
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 7);
    }
    else
    {
        prVolInfo->u1VideoObjectLayerVerid = 0x01;
    }

    UNUSED(u1ParHeight);
    UNUSED(u1ParWidth);

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0); 
    prVolInfo->u1AspectRatioInfo = (UINT8)(u4Temp >> 28);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 4);
    if (prVolInfo->u1AspectRatioInfo == 0x0f)
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        u1ParWidth = (UINT8)(u4Temp >> 24);
        LOG(6, "Par Width = %d\n", u1ParWidth);
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        u1ParHeight = (UINT8)(u4Temp >> 24);
        LOG(6, "Par Height = %d\n", u1ParHeight);
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0) >> 31;
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if(u4Temp)  // vol control parameters
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3) >> 31;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
        if(u4Temp) // vbv_parameters
        {
            _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32); // first_half_bit_rate, marker_bit, latter_half_bit_rate, marker_bit
            _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 19); // first_half_vbv_buffer_size, marker_bit, latter_half_vbv_buffer_size
            _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 28); // first_half_vbv_occupancy, marker_bit, latter_half_vbv_occupancy, marker_bit
        }
    }

    if(((_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0) >> 30) & 0x03) != 0) // Video Object Layer Shape
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3);
    prVolInfo->u4TimeIncrementResolution = (u4Temp >> 16) & 0xffff;
    for(i = 15; i > 0; i--)
    {
        if((prVolInfo->u4TimeIncrementResolution - 1) & (0x1 << i))
        {
            break;
        }
    }
    prVolInfo->u1VopTimeIncrementResolutionBits = (UINT8)(i + 1);

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 17);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if(u4Temp & 0x80000000) // fixed_vop_rate
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->u2FixedVopTimeIncrement  = 
        	(UINT16)(u4Temp >> (32 -  prVolInfo->u1VopTimeIncrementResolutionBits));
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, prVolInfo->u1VopTimeIncrementResolutionBits);
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    prDecPrm->u2Width = (UINT16)(u4Temp >> 19);
    prDecPrm->u2Height = (UINT16)((u4Temp >> 5) & 0x1fff);

	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    if (prVdecEsInfoKeep->fgVPush && prDecPrm->u2Width == 0 && prDecPrm->u2Height == 0)
    {
        ASSERT(prVdecEsInfo->fgMMSeqInfoValid);
        prDecPrm->u2Width = prVdecEsInfo->rMMSeqInfo.u2_width;
        prDecPrm->u2Height = prVdecEsInfo->rMMSeqInfo.u2_height;
        LOG(6, "Retrieve W, H from rMMSeqInfo instead of the 0 in the fake seq. header!!!\n");
        LOG(6, "New Width = %d Height = %d\n", prDecPrm->u2Width, prDecPrm->u2Height);
    }
    LOG(6, "Video Width = 0x%08x Height = 0x%08x\n", prDecPrm->u2Width, prDecPrm->u2Height);


    prDecPrm->u4DramWidth = ((prDecPrm->u2Width % 16) != 0) ? 
    	(UINT16)(((prDecPrm->u2Width / 16) + 1) * 16) : (prDecPrm->u2Width);

    prDecPrm->u4DramHeight = ((prDecPrm->u2Height % 32) != 0) ? 
    	(UINT16)(((prDecPrm->u2Height / 32) + 1) * 32) : (prDecPrm->u2Height);


    if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RELEASE_FBG))        
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);    
        FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 20);    // block until display unlock                    
        FBM_ReleaseGroup(prDecPrm->u1FbgId);
        prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;
    }
#if defined( CC_FBM_FULL_3D_SUPPORT) || defined (CC_SUPPORT_4K2K)
    if (prDecPrm->u2Width >= 3840 || prDecPrm->u2Height > 2160)
#else
    if (prDecPrm->u2Width > 1920 || prDecPrm->u2Height > 2160)
#endif
    {
        LOG(3, "Not Support Resolution\n");
        *pi4RetValue = E_VDEC_RES_NOT_SUPPORT;
        return;
    }

    u1FbgType = FBM_SelectGroupType(prDecPrm->u4DramWidth, prDecPrm->u4DramHeight);
	#if 0
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    #endif
    if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
    {
        {
            FBM_CREATE_FBG_PAR_T rPar;
            x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
            rPar.u1VdecId = (UINT8)ucEsId;
            if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
            {
                rPar.u1AppMode = FBM_FBG_APP_OMX;
            }
            else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
            {
                rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
            }
            else
            {
                rPar.u1AppMode = FBM_FBG_APP_NORMAL;
            }

            if (prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                LOG(3, "fgCreateFromInst == TRUE\n");
                rPar.fgCreateFromInst = TRUE;
            }
            if (prVdecEsInfo->u4RenderPicCnt)
            {
                rPar.fgThumbnailMode = TRUE;
            }

            prDecPrm->u1FbgId =
                FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u4DramWidth, prDecPrm->u4DramHeight, &rPar);
        }
        FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
        if(prVdecEsInfoKeep->fgVPush)
        {
            if(prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
            }
            FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
            if(prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
            }
        }
        //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
        if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
        {
            LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
            *pi4RetValue = E_VDEC_NOT_SUPPORT;
            return;
        }
        prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
        FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
        FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
        FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
        prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
        prDecPrm->u1FbgType = u1FbgType;
        FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);           
    }
    else
    {
        if (u1FbgType != prDecPrm->u1FbgType)
        {
            LOG(3, "Fbg Type Change %d\n", u1FbgType);
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);

            FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 
            	20);    // block until display unlock            

            FBM_ReleaseGroup(prDecPrm->u1FbgId);
            prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;         

            {
                FBM_CREATE_FBG_PAR_T rPar;
                x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                rPar.u1VdecId = (UINT8)ucEsId;
                if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX;
                }
                else if(prVdecEsInfoKeep->fgVPush)
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                }
                else
                {
                    rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                }

                if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    LOG(3, "fgCreateFromInst == TRUE\n");
                    rPar.fgCreateFromInst = TRUE;
                }
                if (prVdecEsInfo->u4RenderPicCnt)
                {
                    rPar.fgThumbnailMode = TRUE;
                }

                prDecPrm->u1FbgId =
                    FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u4DramWidth, prDecPrm->u4DramHeight, &rPar);
            }
            FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                {
                    FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                }
                FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                }
            }
            //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
            if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
            {
                LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                *pi4RetValue = E_VDEC_FAIL;
                return;
            }
            prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
            FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
            FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
            prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);             
#ifdef CC_MPEG4_DEBLOCKING
                FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_DEBLOCKING);
                LOG(6, "DBK Enable\n");
#endif
        }
    }

#ifdef IC_MPEG4_VERIFICATION
	{
		UINT32 i =0, u4AddrY, u4AddrC;
		
		for (i=0; i<4; i++)
		{
			FBM_GetFrameBufferAddr(prDecPrm->u1FbgId, i, &u4AddrY, &u4AddrC);
			LOG(0,"Frame buffer %d: Y 0x%8x C 0x%8x\n",i,u4AddrY,u4AddrC);
		}
	}
#endif
    prDecPrm->u4UmvPicW = (UINT32)(((prDecPrm->u2Width + 15) / 16) * 16);
    prDecPrm->u4UmvPicH = (UINT32)(((prDecPrm->u2Height + 15) / 16) * 16);

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 28);
    prVolInfo->fgInterlaced = (BOOL)(u4Temp >> 31) & 0x1;
    prVolInfo->fgObmcDisable = (BOOL)(u4Temp >> 30) & 0x1;
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);

    if (prFbmSeqHdr != NULL)
    {
        prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
        prFbmSeqHdr->fgPixelAspectRatio = TRUE;
        
        if (prVdecEsInfo != 0)
        {
            //prFbmSeqHdr->ucAspRatInf = 1;        // 16:9
            /*switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
            {
                case VDEC_SRC_FRAME_RATE_23_976: case VDEC_SRC_FRAME_RATE_24:
                case VDEC_SRC_FRAME_RATE_25: case VDEC_SRC_FRAME_RATE_29_97:
                case VDEC_SRC_FRAME_RATE_30: case VDEC_SRC_FRAME_RATE_50:
                case VDEC_SRC_FRAME_RATE_59_94: case VDEC_SRC_FRAME_RATE_60:
                    prFbmSeqHdr->ucFrmRatCod = prVdecEsInfo->rMMSeqInfo.e_frame_rate;
                    break;
                default:
                    prFbmSeqHdr->ucFrmRatCod = VDEC_SRC_FRAME_RATE_29_97;
                    LOG(1, "Upper layer did not set the frame rate\n");
                    break;
            }*/
            switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
            {
                case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                    break;
                case VDEC_SRC_FRAME_RATE_24:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                    break;
                case VDEC_SRC_FRAME_RATE_25:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                    break;
                case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                    break;
                case VDEC_SRC_FRAME_RATE_30:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                    break;
                case VDEC_SRC_FRAME_RATE_50:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                    break;
                case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                    break;
                case VDEC_SRC_FRAME_RATE_60:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                    break;
                case VDEC_SRC_FRAME_RATE_12:
                   prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_12;
                   break;
                case VDEC_SRC_FRAME_RATE_15:
                   prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
                   break;
                 case VDEC_SRC_FRAME_RATE_20:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_20;
                    break;
                default:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                    break;
            }
            prFbmSeqHdr->fgProgressiveSeq = (prVolInfo->fgInterlaced) ? 0 : 1;
#ifdef CC_RESIZE_SMALL_IMAGE
            if(((prDecPrm->u2Width < FBM_IMG_RESIZE_LIMITATION) ||
                (prDecPrm->u2Height < FBM_IMG_RESIZE_LIMITATION)) &&
                (!((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))))
            {
                prFbmSeqHdr->fgResizeSmallPic = TRUE;

                if (prDecPrm->u2Width < FBM_IMG_RESIZE_LIMITATION)
                {
                    prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    //prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    if(prVdecEsInfo->fgMMSeqInfoValid && (prVdecEsInfo->rMMSeqInfo.u2_width > 0)) // i am not sure if this should be only for MM. xiaolei's
                    	prFbmSeqHdr->u2OrgHSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                    else
                        prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                }
                else if(prDecPrm->u2Width > FBM_IMG_RESIZE_LIMITATION_MAX)
                {
                    prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                }
                else
                {
                    prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
                    prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                }

                if (prDecPrm->u2Height < FBM_IMG_RESIZE_LIMITATION)
                {
                    prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    //prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    if(prVdecEsInfo->fgMMSeqInfoValid && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)) // i am not sure if this should be only for MM. xiaolei's
                    	prFbmSeqHdr->u2OrgVSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                    else
                        prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                  //  prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                }
                else if(prDecPrm->u2Height > FBM_IMG_RESIZE_LIMITATION_MAX)
                {
                //   prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                }
                else
                {
                    prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                    prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                }
            }
            else
            {
                if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                    ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
                {
                    prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                    prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
                    prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
                }
                else
                {
                    prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                    prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                }
                prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            }
#else
            if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
            {
                prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
                prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
            }
            else
            {
                prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
            }
            prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
#endif
        }
        else
        {
            // Did not parse the frame rate from ES so far
            //FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);   
            //prFbmSeqHdr->ucAspRatInf = 1;        // 16:9
            prFbmSeqHdr->ucFrmRatCod = 5;        // 30 fps
            prFbmSeqHdr->fgProgressiveSeq = (prVolInfo->fgInterlaced) ? 0 : 1;
            prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
            prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
            prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
            prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
            prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            //prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
            //FBM_PicStructChgNotify(0, MPEG_PIC_FRAME);
        }
	prFbmSeqHdr->u4BitRatVal = prDecPrm->rPesInfo.u4BitRate;
    /* need mentor's double check, follow Jason's mail: "About CR review on 20110316", B2R/VDEC's linesize must be 64 px. alignment */
    prFbmSeqHdr->u2LineSize = (((prFbmSeqHdr->u2LineSize + 63) >> 6) << 6);

#ifdef IC_MPEG4_VERIFICATION
    prDecPrm->u4LineSize = prDecPrm->u2Width;
#endif

        LOG(7, "prVolInfo->u1AspectRatioInfo (%X)\n", prVolInfo->u1AspectRatioInfo);
        switch (prVolInfo->u1AspectRatioInfo)
        {
            case 0x1: // 1:1 Square
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                break;
            case 0x2: // 12:11 625-type for 4:3 picture
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_12_11;
                break;
            case 0x3: // 10:11 525-type for 4:3 picture
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_10_11;
                break;                
            case 0x4: // 16:11 625-type stretched for 16:9 picture
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_11;
                break;
            case 0x5: // 40:33 525-type stretched for 16:9 picture
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_40_33;
                break;
            case 0x0: // forbidden
            case 0x6: // Reserved
            case 0x7: // Reserved
            case 0x8: // Reserved
            case 0x9: // Reserved
            case 0xA: // Reserved
            case 0xB: // Reserved
            case 0xC: // Reserved
            case 0xD: // Reserved
            case 0xE: // Reserved
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                break;
            case 0xF: // extended PAR
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                prFbmSeqHdr->u4AspectRatioWidth = u1ParWidth;
                prFbmSeqHdr->u4AspectRatioHeight = u1ParHeight;
                break;
            default:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_4_3;
                break;
        }
    }

    _CreateWorkingBuffer(prDecPrm);
	  
    if(prVolInfo->u1VideoObjectLayerVerid == 1)
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->u1SpriteEnable = (UINT8)(u4Temp >> 31) & 0x1;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    }
    else
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->u1SpriteEnable = (UINT8)(u4Temp >> 30) & 0x3;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
    }
#ifdef CC_MAPLE_CUST_DRV
	if(prVdecEsInfo->fgXVID)
	{
	  eCodecType = E_VDEC_CODEC_XVID;
	}
	else if(prDecPrm->u1VideoType == VIDEO_TYPE_ISO_MPEG4)
	{
	  eCodecType = E_VDEC_CODEC_MPEG4_VISUAL;
	}
	FBM_SetCodecInfo(prDecPrm->u1FbgId,eCodecType);
#endif
    if(prVolInfo->u1SpriteEnable != SPRITE_NOT_USED && prVolInfo->u1SpriteEnable != SPRITE_GMC)
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }

    if(prVolInfo->u1SpriteEnable == SPRITE_GMC)
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->u1NoOfSpriteWarpingPoints = (UINT8)(u4Temp >> 26) & 0x3f;
        prVolInfo->u1SpriteWarpingAccuracy = (UINT8)(u4Temp >> 24) & 0x03;
		prVolInfo->fgSpriteBrightChange = (BOOL)(u4Temp >> 23) & 0x1;
        
        if(prVdecEsInfoKeep->fgVPush) //LG only
		{
			if (prVolInfo->u1NoOfSpriteWarpingPoints > 1)
        	{
           		LOG(0, "Error: The number of warping points is larger than 1.\n");
            	*pi4RetValue = E_VDEC_NOT_SUPPORT;
            	return;
        	}
        
        	if((u4Temp >> 22) & 0x01) // Sprite Brightness Change
        	{
            	*pi4RetValue = E_VDEC_FAIL;
            	return;
        	}
    	}
        else //xiaolei.li's
        { 
            if(prVolInfo->fgSpriteBrightChange)
            {
                LOG(1, "[MPEG4]: Out of Spec,Force Support GMC type\n");
                //*pi4RetValue = E_VDEC_NOT_SUPPORT;
                //return;
            }
        }
		
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 9);
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    if (u4Temp >> 31)   // not_8_bit, we just support 8-bits pixel
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    prVolInfo->u1QuantType = (UINT8)(u4Temp >> 31);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if (prVolInfo->u1QuantType)
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->fgLoadIntraQuantMat = (UINT8)(u4Temp >> 31) & 0x01;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
        if (prVolInfo->fgLoadIntraQuantMat)
        {
            _LoadM4vQMat(prDecPrm, pi4RetValue, TRUE);
            if(*pi4RetValue == E_VDEC_FAIL)
            {
            	return;
            }
        }
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
        prVolInfo->fgLoadNonIntraQuantMat = u4Temp >> 31;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
        if(prVolInfo->fgLoadNonIntraQuantMat)
        {
          	_LoadM4vQMat(prDecPrm, pi4RetValue, FALSE);
          	if(*pi4RetValue == E_VDEC_FAIL)
          	{
            	return;
          	}
        }
    }
	  
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    if (prVolInfo->u1VideoObjectLayerVerid != 0x01)
    {
        prVolInfo->fgQuarterSample = u4Temp >> 31;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if (!(u4Temp >> 31)) // Complexity Estimation Disable
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }

    prVolInfo->fgResyncMarkerDisable = (BOOL)(u4Temp >> 30) & 0x01;
    prVolInfo->fgDataPartitioned = (BOOL)(u4Temp >> 29) & 0x01;
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
    if (prVolInfo->fgDataPartitioned)
    {
        prVolInfo->fgReversibleVlc = (u4Temp >> 28) & 0x01;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    }

    u4Temp =  _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    if (prVolInfo->u1VideoObjectLayerVerid != 0x01)
    {
        if (u4Temp >> 31) // newpred_enable
        {
             *pi4RetValue = E_VDEC_FAIL;
             return;
        }

        prVolInfo->fgReducedResolutionVopEnable = (u4Temp >> 30) & 0x01;
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    }
    prDecPrm->rM4vParams.rVol.fgValidVolInfo = TRUE;
}

static void _PrsUsrData(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 *pi4RetValue)
{
	UINT8	u1aBuf[64] = {0};
	UINT32 	u4BufPos = 0, u4Temp, i;
	VDEC_M4V_VOL_INFO_T  *prVolInfo;

	prVolInfo = &prDecPrm->rM4vParams.rVol;

	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
	while((u4Temp >> 8) != M4V_START_CODE && u4BufPos < 64)
	{
		u1aBuf[u4BufPos++] = (UINT8)(u4Temp >> 24);
		_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
		u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
	}

    if(!x_strncmp((const CHAR*)u1aBuf, "XviD", 4))
	{
		prDecPrm->u4XviDCodecVersion = (u1aBuf[4] & 0xf) * 1000 + (u1aBuf[5] & 0xf) * 100 + (u1aBuf[6] & 0xf) * 10 + (u1aBuf[7] & 0xf);
	}
	// Mainly parse user data for DIVX
	if(x_strncmp((const CHAR*)u1aBuf, "DivX", 4))
	{
		//*pi4RetValue = E_VDEC_FAIL;
		return;
	}
	else 
	{
		prDecPrm->u1VideoType = VIDEO_TYPE_DIVX_MPEG4;
	}

	prVolInfo->u4DivxCodecVersion = _GetDivXVersion(&u1aBuf[4]);

	if (!x_strncmp((const CHAR*)&u1aBuf[7], "Build", 5))
	{
		prVolInfo->u4UserDataBuildNumber = _GetDivXVersion(&u1aBuf[12]);
		return;
	}

	for(i = 0; i < u4BufPos; i++)
  	{
  		if(i < 61)//add this to avoid buffer overflow
  		{
    		if(u1aBuf[i] == 'b')
    		{
      			prVolInfo->u4UserDataBuildNumber = _GetDivXVersion(&u1aBuf[i+1]);
      			break;
    		}
  		}
		else
		{
			break;
		}
  	}
}

static void _PrsM4vProfileLevel(UINT32 u4Temp, UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    switch(u4Temp)
    {
        case 0x01:
               prVdecEsInfo->eProfile= VDEC_PROFILE_SIMPLE;
               prVdecEsInfo->eLevel= VDEC_LEVEL_1;
               break;

         case 0x02:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x03:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_3;
                break;
          case 0x11:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_SCALABLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x12:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_SCALABLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x21:
                prVdecEsInfo->eProfile = VDEC_PROFILE_CORE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x22:
                prVdecEsInfo->eProfile = VDEC_PROFILE_CORE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x32:
                prVdecEsInfo->eProfile = VDEC_PROFILE_MAIN;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x33:
                prVdecEsInfo->eProfile = VDEC_PROFILE_MAIN;
                prVdecEsInfo->eLevel = VDEC_LEVEL_3;
                break;

          case 0x34:
                prVdecEsInfo->eProfile = VDEC_PROFILE_MAIN;
                prVdecEsInfo->eLevel = VDEC_LEVEL_4;
                break;

          case 0x42:
                prVdecEsInfo->eProfile = VDEC_PROFILE_N_BIT;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x51:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SCALABLE_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x61:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_FACE_ANIMATION;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x62:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_FACE_ANIMATION;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x63:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_FBA;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x64:
                prVdecEsInfo->eProfile = VDEC_PROFILE_SIMPLE_FBA;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x71:
                prVdecEsInfo->eProfile = VDEC_PROFILE_BASIC_ANIMATED_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x72:
                prVdecEsInfo->eProfile = VDEC_PROFILE_BASIC_ANIMATED_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x81:
                prVdecEsInfo->eProfile = VDEC_PROFILE_HYBRID;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x82:
                prVdecEsInfo->eProfile = VDEC_PROFILE_HYBRID;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x91:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SIMPLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0x92:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SIMPLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0x93:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SIMPLE;
                prVdecEsInfo->eLevel= VDEC_LEVEL_3;
                break;

          case 0x94:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SIMPLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_4;
                break;

          case 0xa1:
                prVdecEsInfo->eProfile = VDEC_PROFILE_CORE_SCALABLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0xa2:
                prVdecEsInfo->eProfile = VDEC_PROFILE_CORE_SCALABLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0xa3:
                prVdecEsInfo->eProfile = VDEC_PROFILE_CORE_SCALABLE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_3;
                break;

          case 0xb1:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CODING_EFFICIENCY;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0xb2:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CODING_EFFICIENCY;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0xb3:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CODING_EFFICIENCY;
                prVdecEsInfo->eLevel = VDEC_LEVEL_3;
                break;

          case 0xb4:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CODING_EFFICIENCY;
                prVdecEsInfo->eLevel = VDEC_LEVEL_4;
                break;

          case 0xc1:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CORE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0xc2:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_CORE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0xd1:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SCALABLE_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_1;
                break;

          case 0xd2:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SCALABLE_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_2;
                break;

          case 0xd3:
                prVdecEsInfo->eProfile = VDEC_PROFILE_ADVANCED_SCALABLE_TEXTURE;
                prVdecEsInfo->eLevel = VDEC_LEVEL_3;
                break;

          default:
                prVdecEsInfo->eProfile = VDEC_PROFILE_UNKNOWN;
                prVdecEsInfo->eLevel = VDEC_LEVEL_UNKNOWN;
                break;
                
            
    }
}

static void _LoadM4vQMat(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue
							, BOOL fgIntra)
{
    // Should use software to load Q Matrix
    // instead of hardware
#if 0
    UINT32	u4OldAddr, u4Bit, u4OldBitCount;
    UINT32 	u4QLength;
    UINT32	i, u4Idx = 0;
    UINT8 u1QMat[64];
    UINT32 u4QMat[16];
    UINT32 u4Val;

    while ((VLD_READ32(0xF4) & 0x20000) != 0x20000)
    {
    };


    u4OldAddr = _VDEC_VldMpeg4RPtr(&u4Bit);

    // Trigger Hardware to load Q Matrix
    if (fgIntra) 
    {
        VLD_READ32(RW_VLD_TABLIM);
    }
    else
    {
        VLD_READ32(RW_VLD_TABLNIM);
    }

    u4OldBitCount = prDecPrm->u4BitCount;

    while ((VLD_READ32(0xF4) & 0x20000) != 0x20000)
    {
    };

    /* 040329: Workaround for HW bug:
             If quantization matrix is smaller than 64 bytes,
             HW may load more than necessary bytes,
             causing successive vparser reading to be wrong.
             To cover it, after loading a matrix,
             set vld rd ptr back to the matrix,
             and shift the correct byte number by FW.
             One issue is not covered yet:
             The remaining values should be equal to the value before the 0,
             but if HW loads the values after the 0, it will use those values.
             To really solve this issue we have to prepare a 64-byte buffer with
             correct trailing values for HW to load. */
    if(!_VDEC_VLDInitMpeg4BarrelShifter(u4OldAddr, (VDEC_HDLR)prDecPrm))
    {
    	*pi4RetValue = E_VDEC_FAIL;
    	return;
    }

    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, u4Bit);

    prDecPrm->u4BitCount = u4OldBitCount;
#endif 
    UINT32 	u4QLength;
    UINT32	i, u4Idx = 0;
    UINT32 u4Val;
    UINT8* pu1QMat;
    VDEC_M4V_VOL_INFO_T  *prVolInfo;

    prVolInfo = &prDecPrm->rM4vParams.rVol;
    pu1QMat = ( fgIntra ? prVolInfo->au1IntraQuantMat : prVolInfo->au1NonIntraQuantMat );
    
    u4QLength = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    for(i = 0; i < 64; i++)
    {	
#ifdef CC_IQ_RASTER_SCAN_MODE	
		*(pu1QMat+zig_zag_scan[i]) = (UINT8)(u4QLength >> 24);
#else
    	*(pu1QMat+i) = (UINT8)(u4QLength >> 24);
#endif
    	if((*((UINT8 *)&(u4QLength) + 3)) == 0)
    	{
    		_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
    		break;
    	}
    	_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
    	u4QLength = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    }
    
    if (i < 64)
    {
        if (i >= 1)
        {
#ifdef CC_IQ_RASTER_SCAN_MODE	        
            UINT8 u1Temp = *(pu1QMat + zig_zag_scan[i] - 1);
#else
            UINT8 u1Temp = *(pu1QMat + i- 1);
#endif
            for (;i < 64; i++)
            {
#ifdef CC_IQ_RASTER_SCAN_MODE            
                *(pu1QMat+zig_zag_scan[i]) = u1Temp;
#else
                *(pu1QMat+i) = u1Temp;
#endif
            }
        }
    }
    
    if (fgIntra)
    {
        for (u4Idx = 0; u4Idx <= 15; u4Idx++)
        {
            u4Val = (UINT32)(VLD_WRITE_QMATRIX |(u4Idx << 2));
            VLD_WRITE32(RW_VLD_RISC_QMATRIX, u4Val);
            VLD_WRITE32(RW_VLD_QMATRIX_RW, (UINT32)((*(pu1QMat + u4Idx*4) << 24) |
                          (*(pu1QMat + u4Idx*4 + 1) << 16) |
                          (*(pu1QMat + u4Idx*4 + 2) << 8) |
                          (*(pu1QMat + u4Idx*4 + 3))));
        }

    }
    else
    {
        for (u4Idx = 0; u4Idx <= 15; u4Idx++)
        {
            u4Val = (UINT32)(VLD_WRITE_QMATRIX | VLD_NONINTRA |(u4Idx << 2));
            VLD_WRITE32(RW_VLD_RISC_QMATRIX, u4Val);
            VLD_WRITE32(RW_VLD_QMATRIX_RW, (UINT32)((*(pu1QMat + u4Idx*4) << 24) |
                          (*(pu1QMat + u4Idx*4 + 1) << 16) |
                          (*(pu1QMat + u4Idx*4 + 2) << 8) |
                          (*(pu1QMat + u4Idx*4 + 3))));
        }
    }

    *pi4RetValue = E_VDEC_OK;

}

static void _PrsVOPHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue, UCHAR ucEsId)
{
    UCHAR ucBRefFbId;
    UCHAR ucFRefFbId;              

    UINT32 u4Temp;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;    
    FBM_PIC_HDR_T * prFbmPicHdr;
    VDEC_M4V_VOL_INFO_T 	*prVolPrm;
    VDEC_M4V_PIC_INFO_T 	*prVopPrm;
    VDEC_M4V_GMC_INFO_T 	*prGmcPrm;

    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    if ((prVdecEsInfo != NULL) &&
        (prVdecEsInfo->fgOnlyGotSeqHeader))
    {
        LOG(5, "Get Seq Header Only, Don't need to parse pic\n");
       // Printf("SeqHead Unlock %d\n",prDecPrm->rPesInfo.ucEsId );
        if (prDecPrm->u4LockCount > 0)
        {
            _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
            prDecPrm->u4LockCount--;
        }
        *pi4RetValue = E_VDEC_OK;//return OK here? will into decode ??
        return;
    }

    if (!prDecPrm->rM4vParams.rVol.fgValidVolInfo)
    {
        LOG(0, "\nWarning!!! No valid VOL is found before parsing VOP!!!\n");
    }
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
    if (!prFbmSeqHdr)
    {
        LOG(5, "FBM sequece header is null");
        if (prVdecEsInfo != NULL && prVdecEsInfo->u4FailCnt >= 500)
        {
            *pi4RetValue = E_VDEC_NOT_SUPPORT;
        }
       else
        {
            *pi4RetValue = E_VDEC_FAIL;
        }
        return;
    }

    prVolPrm = &prDecPrm->rM4vParams.rVol;
    prVopPrm = &prDecPrm->rM4vParams.rVop;
    prGmcPrm = &prVopPrm->rGmcParam;
    
    // ???0421 LOG(7, "This is the %ld frame\n", prDecPrm->u4FrameCount);
    // ???0421 prDecPrm->u4FrameCount++;

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeSwS);
#endif


    u4Temp = VLD_READ32(0xF0);
    LOG(7, "Parser Input Window: 0x%8x\n", u4Temp);
    
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3);
    prDecPrm->u1VopCodingType = (UINT8)((u4Temp >> 1) & 0x03); // Vop Coding Type
	LOG(7,"Enter PrsVOPHdr prDecPrm->u1VopCodingType:%d\n",prDecPrm->u1VopCodingType);
    *pi4RetValue = E_VDEC_OK;

    if (prDecPrm->fgDecError == TRUE && 
        prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }
    else 
    {
        prDecPrm->fgDecError = FALSE;
    }
     
    if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_B)
    {
        prVopPrm->u4RefPicTimeBase = prVopPrm->u4TimeBase;
        prVopPrm->u4CurDispTime = prVopPrm->u4TimeBase;
		prDecPrm->u1ForwardRefVopType = prDecPrm->u1BackwardRefVopType;
        prDecPrm->u1BackwardRefVopType = prDecPrm->u1VopCodingType;
    }
    else
    {
        prVopPrm->u4CurDispTime = prVopPrm->u4RefPicTimeBase;
    }

    while (u4Temp & 0x01)
    {
        if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_B)
        {
            prVopPrm->u4TimeBase++;
        }
        prVopPrm->u4CurDispTime++;
        
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    } 
    
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, (UINT32)(prVolPrm->u1VopTimeIncrementResolutionBits+1));
    prVopPrm->u2VopTimeIncrement = 
    	(UINT16)(u4Temp & _LsbMask(prVolPrm->u1VopTimeIncrementResolutionBits)); 

    LOG(6, "Video coding type = %d Time Increment = %d\n", prDecPrm->u1VopCodingType, prVopPrm->u2VopTimeIncrement);
    
    prVopPrm->u4CurDispTime = (prVopPrm->u4CurDispTime * prVolPrm->u4TimeIncrementResolution)
    						+ prVopPrm->u2VopTimeIncrement;
    
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
    if ((u4Temp & 0x01) == 0) // vop is not coded
    {
        LOG(3, "Not coded video frame\n");
        prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
        prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
        prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;

        if(prDecPrm->rPesInfo.u2SkipVidCount > 0)
        {
            if ((prDecPrm->u8PtsScale != 0) &&
                        (prDecPrm->u4Rate != 0))
            {
                UINT64 u8Numerator = 0, u8Denominator = 0;

                u8Numerator = prDecPrm->u8PtsScale + prDecPrm->u8Remainder;
                u8Denominator = (UINT64)prDecPrm->u4Rate;

                prDecPrm->u4DispPts += 
                    u8Div6432(u8Numerator, u8Denominator, 
                    &prDecPrm->u8Remainder)*
                    (prDecPrm->rPesInfo.u2SkipVidCount);     
                
                // u8Pts
                u8Numerator = prDecPrm->u8PtsScale + prDecPrm->u8Remainder64;
                u8Denominator = (UINT64)prDecPrm->u4Rate;

                prDecPrm->u8DispPts += 
                    u8Div6432(u8Numerator, u8Denominator, 
                    &prDecPrm->u8Remainder64)*
                    (prDecPrm->rPesInfo.u2SkipVidCount);
            }
        }

        if (!prDecPrm->fgEnVirtualP)
        {
            if (prDecPrm->fgCheckVP)
            {
                prDecPrm->fgCheckVP = FALSE;
            }
            else
            {
                prDecPrm->u4VopNotCoded++;
            }

            *pi4RetValue = E_VDEC_NOT_CODED_FRAME;
            return;
        }
        else
        {
            if (prDecPrm->fgCheckVP)
            {
                prDecPrm->fgCheckVP = FALSE;
                *pi4RetValue = E_VDEC_NOT_CODED_FRAME;
                return;
            }
            prDecPrm->fgVopNotCoded = TRUE;
        }
    }
    
    LOG(7, "This is the %ld frame\n", prDecPrm->u4FrameCount);
    prDecPrm->u4FrameCount++;
    

    if (!prDecPrm->fgVopNotCoded)
    {
        if (prDecPrm->u1VopCodingType == VOP_CODING_TYPE_P || 
           (prDecPrm->u1VopCodingType == VOP_CODING_TYPE_S && 
           prVolPrm->u1SpriteEnable == SPRITE_GMC))	
        {
        	prDecPrm->u1VopRoundingType =  (UINT8)_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1) & 0x01;
        	LOG(7, "Rounding Type = %ld\n", prDecPrm->u1VopRoundingType);
        }
        else
        {
        	prDecPrm->u1VopRoundingType = 0;
        	LOG(7, "Rounding Type = 0\n");
        }
        
        if (prVolPrm->fgReducedResolutionVopEnable && 
        	(prDecPrm->u1VopCodingType == VOP_CODING_TYPE_I || 
           	 prDecPrm->u1VopCodingType == VOP_CODING_TYPE_P))
        {
        	prVopPrm->fgVopReducedResolution = (BOOL)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1) & 0x01);
        }
        
        prVopPrm->u1IntraDcVlcThr = (UINT8)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3) & 0x07);
        
#ifndef CC_MT5398_NEW_MODE        
        if(prVopPrm->u1IntraDcVlcThr == 7)
        {
             LOG(2,"u1IntraDcVlcThr is 7, return vdec not support!\n");
            *pi4RetValue = E_VDEC_NOT_SUPPORT;
        }
#endif
        if (prVolPrm->fgInterlaced)
        {
            u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
            prVopPrm->fgTopFldFirst = (u4Temp >> 1) & 0x01;
            if (prVopPrm->fgTopFldFirst)
            {
                prDecPrm->u1PicStructure = MP4_TOP_FLD_PIC;
            }
            else
            {
                prDecPrm->u1PicStructure = MP4_BTM_FLD_PIC;
            }

            prVopPrm->fgAlternateVerticalScanFlag = u4Temp & 0x01;
            if ((prVolPrm->fgInterlaced) &&
                (!prVopPrm->fgAlternateVerticalScanFlag))
            {
                //prFbmSeqHdr->fgProgressiveSeq = TRUE;
                if ((prFbmSeqHdr->u2VSize % 2) != 0)
                { 
                    prFbmSeqHdr->u2VSize = prDecPrm->u2Height - 1;
                    prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
                }
            }
        }
        else
        {
        	prDecPrm->u1PicStructure = MP4_FRM_PIC;
        }
        
        if ((prVolPrm->u1SpriteEnable == SPRITE_STATIC ||prVolPrm->u1SpriteEnable == SPRITE_GMC) &&
        	prDecPrm->u1VopCodingType == VOP_CODING_TYPE_S)
        {
        	if (prVolPrm->u1NoOfSpriteWarpingPoints > 0)
        	{
                _SpriteTrajetory(prDecPrm, pi4RetValue);
                if (*pi4RetValue != E_VDEC_OK)
                {
                    prDecPrm->fgDecError = TRUE;
                    return;
                }
        	}
        }
        else
        {
        	prVopPrm->u1EffectiveWarpingPoints = 0;
        }
        
        // Process GMC Parameters
        if(prVopPrm->u1EffectiveWarpingPoints == 0)
        	{
        		prGmcPrm->i4GmcYMvX = 0;
        		prGmcPrm->i4GmcYMvY = 0;
        		prGmcPrm->i4GmcCMvX = 0;
        		prGmcPrm->i4GmcCMvY = 0;
        	}
        	else
        	{
        		_CalGmcMv(prDecPrm, pi4RetValue);
        	}  
        
        prDecPrm->u1VopQuant = (UINT8)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, prVolPrm->u1QuantPrecision) & 0x1f);
        if(prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
        {
        	prVopPrm->u1ForwardFCode = (UINT8)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3) & 0x07);
        }
        if(prDecPrm->u1VopCodingType == VOP_CODING_TYPE_B)
        {
        	prVopPrm->u1BackwardFCode = (UINT8)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 3) & 0x07);
        }
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);

        if (prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN)
        {
            // Check if reset is required
            if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET))
            {
                FBM_ResetGroup(prDecPrm->u1FbgId);
                FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET);
                FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_NOT_READY);
    //            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_CLEAR_FRC_PRM);

                /*if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC))
                {
                    _MPV_SkipPic(*pucMpvId, prVdecEsInfo->u4EsId);
                    *pucMpvId = VDEC_MAX_VLD;
                }*/                        
            }
        }
        else
        {
            LOG(3, "_PrsVOPHdr FBM group is not created\n");
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }
    }

    if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_B)
    {
        BOOL fgNotDisplay;

        VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->rPesInfo.ucEsId);

        FBM_GetRefFrameBuffer(prDecPrm->u1FbgId, &ucFRefFbId, &ucBRefFbId);

        LOG(6, "ucFRefFbId = 0x%08x ucBRefFbId = 0x%08x\n", ucFRefFbId, ucBRefFbId);

        if (ucBRefFbId != FBM_FBG_ID_UNKNOWN)
        {
        	if( prVdecEsInfo->eDecodeMode != VDEC_DECODE_MODE_NO_REORDER)
	        {
	        	prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
				if (prFbmPicHdr)
				{
					if (prDecPrm->fgEnCalPts)
					{
						if (prDecPrm->fgValidPts)
						{
                        	LOG(7, "Valid PTS\n");
                        	prFbmPicHdr->u4PTS = prDecPrm->u4RefPts;
                        	prFbmPicHdr->u8PTS = prDecPrm->u8RefPts;
                        	prDecPrm->u4DispPts = prDecPrm->u4RefPts;
							prDecPrm->u8DispPts = prDecPrm->u8RefPts;
                         	prDecPrm->u4VopNotCoded = 0;
                        	prDecPrm->fgValidPts = FALSE;
                        	prDecPrm->fgCheckVP = FALSE;
                    	}
                    	else
                    	{
                        	prFbmPicHdr->u4PTS = prDecPrm->u4DispPts;
                        	prFbmPicHdr->u8PTS = prDecPrm->u8DispPts;
                    	}

	                    #ifdef VDEC_PUSH_PTS_64_BITS
		                LOG(7, "Enable PTS calculation Cur PTS = 0x%016zx\n", prFbmPicHdr->u8PTS);
		                #else
		                LOG(7, "Enable PTS calculation Cur PTS = 0x%08x\n", prFbmPicHdr->u4PTS);
		                #endif
                    	if ((prDecPrm->u8PtsScale != 0) && (prDecPrm->u4Rate != 0))
                    	{
                        	UINT64 u8Numerator = 0, u8Denominator = 0;

                        	u8Numerator = prDecPrm->u8PtsScale + prDecPrm->u8Remainder;
                        	u8Denominator = (UINT64)prDecPrm->u4Rate;

                        	prDecPrm->u4DispPts += 
								u8Div6432(u8Numerator, u8Denominator, 
								&prDecPrm->u8Remainder)*
								(prDecPrm->rPesInfo.u2SkipVidCount + 1 +  prDecPrm->u4VopNotCoded);
                       
            	            // u8Pts
            	            u8Numerator = prDecPrm->u8PtsScale + prDecPrm->u8Remainder64;
                	        u8Denominator = (UINT64)prDecPrm->u4Rate;

                    	    prDecPrm->u8DispPts += 
                        	    u8Div6432(u8Numerator, u8Denominator, 
                            	&prDecPrm->u8Remainder64)*
                            	(prDecPrm->rPesInfo.u2SkipVidCount + 1 +  prDecPrm->u4VopNotCoded);
                       
                        	prDecPrm->u4VopNotCoded = 0;
                    	}
                	}
                	else
                	{
                    	if (prDecPrm->fgValidPts)
						{
                        	prFbmPicHdr->u4PTS = prDecPrm->u4RefPts;
                        	prFbmPicHdr->u8PTS = prDecPrm->u8RefPts;
                        	prDecPrm->u4DispPts = prDecPrm->u4RefPts;
                            prDecPrm->u8DispPts = prDecPrm->u8RefPts;                            
                        	#ifdef VDEC_PUSH_PTS_64_BITS
                        	LOG(7, "Use container's PTS = 0x%016zx\n", prFbmPicHdr->u8PTS);
                        	#else
                        	LOG(7, "Use container's PTS = 0x%08x\n", prFbmPicHdr->u4PTS);
                        	#endif
                    	}
                    	else
                    	{
                        	UINT64 u8Numerator = 0, u8Denominator = 0;

                        	if (prVopPrm->u2VopTimeIncrement >= prVopPrm->u2VopPrevTimeIncrement)
                        	{
                            	u8Numerator = 
									(prVopPrm->u2VopTimeIncrement - prVopPrm->u2VopPrevTimeIncrement) * MPEG4_TIME_SCALE;
                        	}
                        	else
                        	{
                            	u8Numerator =  
                                	(prVolPrm->u4TimeIncrementResolution - prVopPrm->u2VopPrevTimeIncrement + prVopPrm->u2VopTimeIncrement) * MPEG4_TIME_SCALE;
                        	}

                        	u8Numerator += prDecPrm->u8Remainder;
                        	u8Denominator = (UINT64)prVolPrm->u4TimeIncrementResolution;

	                        prFbmPicHdr->u4PTS = prDecPrm->u4DispPts;

    	                    prDecPrm->u4DispPts += 
         	                   u8Div6432(u8Numerator, u8Denominator, &prDecPrm->u8Remainder);
            	            LOG(7, "Use ES PTS = 0x%08x\n", prFbmPicHdr->u4PTS);
	
    	                    // u8Pts
        	                if (prVopPrm->u2VopTimeIncrement >= prVopPrm->u2VopPrevTimeIncrement)
            	            {
                	            u8Numerator = 
                    	            (prVopPrm->u2VopTimeIncrement - prVopPrm->u2VopPrevTimeIncrement) * VDEC_U8_SECOND_CLOCK;
                        	}
                        	else
                        	{
                            	u8Numerator =  
                                	(prVolPrm->u4TimeIncrementResolution - prVopPrm->u2VopPrevTimeIncrement + prVopPrm->u2VopTimeIncrement) * VDEC_U8_SECOND_CLOCK;
                        	}

	                        u8Numerator += prDecPrm->u8Remainder64;
    	                    u8Denominator = (UINT64)prVolPrm->u4TimeIncrementResolution;

        	                prFbmPicHdr->u8PTS = prDecPrm->u8DispPts;

            	            prDecPrm->u8DispPts += 
                	            u8Div6432(u8Numerator, u8Denominator, &prDecPrm->u8Remainder64);
                    	}
                	}
                
                	prFbmPicHdr->u2DecodingOrder = prVdecEsInfo->u2DecodingOrder;
                	prFbmPicHdr->u4TotlaTimeOffset = prVdecEsInfo->u4TotlaTimeOffset;

                	x_memcpy(&prFbmPicHdr->rExtra,
                    	&prDecPrm->rRecExtra,
                    	sizeof(FBM_PIC_EXTRA_INFO_T));
	                prFbmPicHdr->u8Offset = prDecPrm->u8RecOffset;
    	            prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->u8OffsetLast;
        	        prFbmPicHdr->u8OffsetI = prDecPrm->u8RecOffsetI;
            	    prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
                	prFbmPicHdr->fgTopFldFirst = prVopPrm->fgTopFldFirst;
                	prFbmPicHdr->ucTrueZeroPTS = FBM_USE_ZERO_PTS_IF_ZERO;
                	prFbmPicHdr->i4TemporalRef = prDecPrm->i4TemporalRef;

	                prDecPrm->i4TemporalRef++;

#if 1
	                prFbmPicHdr->u4PicWidth = prDecPrm->u2Width;
    	            prFbmPicHdr->u4PicHeight = prDecPrm->u2Height;
        	        prFbmPicHdr->u4PicWidthPitch = prDecPrm->u4LineSize;
#endif

            	    fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prFbmPicHdr->u4PTS,
                	    prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset);
            	    if (fgNotDisplay)
                	{
                    	/*FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, 
                        	ucBRefFbId, 
                        	FBM_MM_NOT_DISPLAY_FLAG);

                    		LOG(6, "_PrsVOPHdr fgNotDisplay ucFbId %d\n", ucBRefFbId);*/
                    	UCHAR ucFbStatus;
						ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
	                    if ((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) ||
    	                    (ucFbStatus != FBM_FB_STATUS_LOCK))
        	            {
            	            FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                	           ucBRefFbId, FBM_FB_STATUS_EMPTY);
                    	}
	                    else
    	                {
        	                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            	        }
                	}
	                else
    	            {
        	            //Printf("prFbmPicHdr->u4PTS = 0x%08x\n", prFbmPicHdr->u4PTS);
            	        //Printf("ucFRefFbId = 0x%08x\n", ucFRefFbId);
                	    //Printf("ucBRefFbId = 0x%08x\n", ucBRefFbId);
                    	UCHAR ucFbStatus;
	                    ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
    	                if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
							(ucFbStatus == FBM_FB_STATUS_READY))
                    	{
                        	LOG(6, "I/P Pic Temporal Ref = %d\n", prFbmPicHdr->i4TemporalRef);
                        	#ifdef CC_REALD_3D_SUPPORT
                        	_VDEC_Check2D3DSwitch(ucEsId, ucBRefFbId);
                        	#endif
                        	FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                        		ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);
							if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
							{
								LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
									   prDecPrm->u1VopCodingType,
									   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
									   prDecPrm->u1FbgId, ucBRefFbId,
									   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
									   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0
									   );
							}

                        	prVdecEsInfo->u4DisplayQPicCnt++;
                    	}
                    	else
                    	{
                        	LOG(6, "Frame buffer status = %x\n", ucFbStatus);
                    	}
                	}
                	VDEC_CheckStopStatus(ucEsId, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef);
            	}
            	else
            	{
                	LOG(3, "_PrsVOPHdr prFbmPicHdr == NULL\n");
                	*pi4RetValue = E_VDEC_FAIL;
                	return;
            	}
        	}
        }
        // FBM error
        else if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
        {
            LOG(3, "Wrong reference for P VOP\n");
            *pi4RetValue = E_VDEC_FAIL;
            prDecPrm->fgNotDisplayDummyFrame = TRUE;
            return;
        }
        
        prDecPrm->fgValidPts = prDecPrm->rPesInfo.fgDtsValid;
        prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
        prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
        prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;
        prDecPrm->u8RecOffsetI = prDecPrm->rPesInfo.u8OffsetI;
        prDecPrm->rRecExtra = prDecPrm->rPesInfo.rExtra;
	
        _VDEC_GetVldRPtr(&prDecPrm->u4DecReadAddr, &prDecPrm->u4DecReadBits, prDecPrm->rPesInfo.u4FifoStart,prDecPrm->rPesInfo.u4FifoEnd);
        if(prDecPrm->u4DecReadAddr< prDecPrm->rPesInfo.u4FifoStart)
        {
		    prDecPrm->u4DecReadAddr = prDecPrm->u4DecReadAddr+
    	                    (prDecPrm->rPesInfo.u4FifoEnd-prDecPrm->rPesInfo.u4FifoStart);
        }
        //Printf("VOP Unlock %d\n",prDecPrm->rPesInfo.ucEsId );
        if (prDecPrm->u4LockCount > 0)
        {
            _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
            prDecPrm->u4LockCount--;
        }
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbS);
#endif
        prDecPrm->u1FbId = FBM_GetEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#ifdef CC_MPEG4_DEBLOCKING
    prDecPrm->u1DbkFbId = FBM_GetDbkEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#endif
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbE);
        HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
        prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
#endif
        prVopPrm->u4PrevDispTime = prVopPrm->u4NextDispTime;
        prVopPrm->u4NextDispTime = prVopPrm->u4CurDispTime;
        prVopPrm->rDirectMd.u4Trd = prVopPrm->u4NextDispTime - prVopPrm->u4PrevDispTime;
    }
    else
    {
        FBM_GetRefFrameBuffer(prDecPrm->u1FbgId, &ucFRefFbId, &ucBRefFbId);

        if (ucFRefFbId == FBM_FBG_ID_UNKNOWN ||
            ucBRefFbId == FBM_FBG_ID_UNKNOWN)
        {
            LOG(3, "Wrong reference for B VOP\n");
            *pi4RetValue = E_VDEC_FAIL;
		    if(prVdecEsInfoKeep->fgVPush) //LG only
		    {
			    prDecPrm->fgNotDisplayDummyFrame = TRUE;
			}

            return;
        }
		
        _VDEC_GetVldRPtr(&prDecPrm->u4DecReadAddr, &prDecPrm->u4DecReadBits, prDecPrm->rPesInfo.u4FifoStart,prDecPrm->rPesInfo.u4FifoEnd);
        if(prDecPrm->u4DecReadAddr< prDecPrm->rPesInfo.u4FifoStart)
        {
    	     prDecPrm->u4DecReadAddr = prDecPrm->u4DecReadAddr+
    	                    (prDecPrm->rPesInfo.u4FifoEnd-prDecPrm->rPesInfo.u4FifoStart);
        }
        //Printf("VOP Unlock %d\n",prDecPrm->rPesInfo.ucEsId );
        if (prDecPrm->u4LockCount > 0)
        {
            _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
            prDecPrm->u4LockCount--;
        }
        
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbS);
#endif
        prDecPrm->u1FbId = FBM_GetEmptyBFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);

#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbE);
        HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
        prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
#endif
        prVopPrm->rDirectMd.u4Trb = prVopPrm->u4CurDispTime- prVopPrm->u4PrevDispTime;
    
        if (prVolPrm->fgInterlaced)
        {
            if (prVopPrm->rDirectMd.u4TFrm == (UINT32)0xffffffff)
            {
                prVopPrm->rDirectMd.u4TFrm = prVopPrm->u4NextDispTime - prVopPrm->u4CurDispTime;
            }
        	
            u4Temp = u4Div2Slash(prVopPrm->u4PrevDispTime, prVopPrm->rDirectMd.u4TFrm);
            		prVopPrm->rDirectMd.u4Trbi = 2 * (u4Div2Slash(prVopPrm->u4CurDispTime
            									, prVopPrm->rDirectMd.u4TFrm) - u4Temp);
            		prVopPrm->rDirectMd.u4Trdi = 2 * (u4Div2Slash(prVopPrm->u4NextDispTime
            									, prVopPrm->rDirectMd.u4TFrm) - u4Temp);
        }
    }
#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeSwE);
    HAL_GetDeltaTime(&rTimeSwDt, &rTimeSwS, &rTimeSwE);
    u4SWTimeMicro += rTimeSwDt.u4Micros;
    if(prFbmSeqHdr->fgProgressiveSeq)
    {
        prDecPrm->u4CurSwMicro = u4SWTimeMicro - prDecPrm->u4CurWaitFbMicro;
        u4SWTimeMicro = 0;
        prDecPrm->u4CurWaitFbMicro = 0;
    }

#endif

	if(prVdecEsInfo != NULL) //for klocwork issue 
		prVdecEsInfo->ucFbId = prDecPrm->u1FbId; //used to get FbmPicHdr in VDEC_QueryInfo. xiaolei's

    if (prDecPrm->u1VopCodingType == VOP_CODING_TYPE_I)
    {
        prDecPrm->i4TemporalRef = 0;
    }

    prVopPrm->u2VopPrevTimeIncrement = prVopPrm->u2VopTimeIncrement;
    
    if (prDecPrm->rPesInfo.fgNoVdecCalPts)
    {
        //ASSERT(prDecPrm->fgCheckVP == FALSE);
        prDecPrm->fgCheckVP = prDecPrm->rPesInfo.fgNoVdecCalPts;
    }
}

static void _SpriteTrajetory(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue)
{
    BOOL	fgBadDivx;
    INT32	i;
    VDEC_M4V_VOL_INFO_T *prVolPrm;
    VDEC_M4V_PIC_INFO_T *prVopPrm;

    prVolPrm = &prDecPrm->rM4vParams.rVol;
    prVopPrm = &prDecPrm->rM4vParams.rVop;

    fgBadDivx = ((prVolPrm->u4DivxCodecVersion == 500) ||
                    (prVolPrm->u4DivxCodecVersion == 501)) &&
                   (prVolPrm->u4UserDataBuildNumber >= 370) &&
                   (prVolPrm->u4UserDataBuildNumber <= 413);

    for(i = 0; (i < prVolPrm->u1NoOfSpriteWarpingPoints) && (i < 4); i++)
    {
        prVopPrm->ppi4WarpingMv[i][0] = _WarpingMvCode(&prDecPrm->u4BitCount);
        if(!fgBadDivx)
        {
            _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);  // marker_bit
        }
        prVopPrm->ppi4WarpingMv[i][1] = _WarpingMvCode(&prDecPrm->u4BitCount);
        _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);  // marker_bit
    }

    prVopPrm->u1EffectiveWarpingPoints= prVolPrm->u1NoOfSpriteWarpingPoints;
    while ((prVopPrm->u1EffectiveWarpingPoints > 0) &&
         (prVopPrm->u1EffectiveWarpingPoints <= 4) &&
         (prVopPrm->ppi4WarpingMv[prVopPrm->u1EffectiveWarpingPoints - 1][0] == 0) &&
         (prVopPrm->ppi4WarpingMv[prVopPrm->u1EffectiveWarpingPoints - 1][1] == 0))
    {
        prVopPrm->u1EffectiveWarpingPoints--;
    }

    if(prVopPrm->u1EffectiveWarpingPoints > 1)
    {
        LOG(1, "[Warning!!!] The num of Warpping points out of spec, Force Support!!!!\n");
        //*pi4RetValue = E_VDEC_NOT_SUPPORT;
        //return;
    }

    *pi4RetValue = E_VDEC_OK;
}

static INT32 _WarpingMvCode(UINT32 *pu4BitCount)
{
	UINT32 	u4Length;
	INT32 	i4Code;

	u4Length = _DmvCodeLength(pu4BitCount);

	if(u4Length == 0)
	{
		return 0;
	}
	if(_VDEC_Mpeg4GetBitStream(pu4BitCount, 1) & 0x1)
	{
		i4Code = (INT32)(1 << (u4Length - 1));
	}
	else
	{
		i4Code = (INT32)(-(1 << u4Length) + 1);
	}
	
	i4Code += (INT32)(_VDEC_Mpeg4GetBitStream(pu4BitCount, u4Length - 1) 
				& _LsbMask(u4Length - 1));

	return i4Code;

}

static UINT32 _DmvCodeLength(UINT32* pi4BitCount)
{
	UINT32 u4DmvLengthMsb;
	UINT32 u4DmvCodeLength;

	u4DmvLengthMsb = _VDEC_Mpeg4GetBitStream(pi4BitCount, 2) & 0x03;
	switch(u4DmvLengthMsb)
	{
		case 0:
	  		u4DmvCodeLength = 0;
	  		break;
		case 1:
			u4DmvCodeLength = 1 + (_VDEC_Mpeg4GetBitStream(pi4BitCount, 1) & 0x01);
			break;
		case 2:
			u4DmvCodeLength = 3 + (_VDEC_Mpeg4GetBitStream(pi4BitCount, 1) & 0x01);
	  		break;
		case 3:
		default:
	  		u4DmvCodeLength = 3;
	  		while(_VDEC_Mpeg4GetBitStream(pi4BitCount, 1) & 0x1)
	  		{
	    			u4DmvCodeLength++;
	  		}
	  		u4DmvCodeLength += 2;
	  		break;
	}

	return u4DmvCodeLength;

}

static void _CalGmcMv(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue)
{
	BOOL	fgBadDivx = FALSE;
	INT32 	i4X, i4Y;
	INT32 	i4SpriteEnable;
	INT32 	i0_pla, j0_pla;

	VDEC_M4V_VOL_INFO_T *prVolPrm;
	VDEC_M4V_PIC_INFO_T *prVopPrm;
	VDEC_M4V_GMC_INFO_T *prGmcPrm;

	prVolPrm = &prDecPrm->rM4vParams.rVol;
	prVopPrm = &prDecPrm->rM4vParams.rVop;
	prGmcPrm = &prVopPrm->rGmcParam;
	
	fgBadDivx = ((prVolPrm->u4DivxCodecVersion == 500) ||
                    (prVolPrm->u4DivxCodecVersion == 501)) &&
                   (prVolPrm->u4UserDataBuildNumber >= 370) &&
                   (prVolPrm->u4UserDataBuildNumber <= 413);

	i4SpriteEnable = 2 << prDecPrm->rM4vParams.rVol.u1SpriteEnable;

	if (fgBadDivx)
	{
		i4X = prVopPrm->ppi4WarpingMv[0][0];
		i4Y = prVopPrm->ppi4WarpingMv[0][1];
	}
	else
	{
		i4X = prVopPrm->ppi4WarpingMv[0][0] * i4SpriteEnable / 2;
		i4Y = prVopPrm->ppi4WarpingMv[0][1] * i4SpriteEnable / 2;
	}

	i0_pla = i4X * 4 / i4SpriteEnable;  /* multiply by two to get q-pel precision */
	j0_pla = i4Y * 4 / i4SpriteEnable;  /* multiply by two to get q-pel precision */
	prGmcPrm->i4GmcYMvX = i0_pla;
	prGmcPrm->i4GmcYMvY = j0_pla;

	i0_pla = (i4X >> 1) | (i4X & 0x01);
	i0_pla = i0_pla * 4 / i4SpriteEnable;   /* multiply by two to get q-pel precision */
	j0_pla = (i4Y >> 1) | (i4Y & 0x01);
	j0_pla = j0_pla * 4 / i4SpriteEnable;   /* multiply by two to get q-pel precision */
	prGmcPrm->i4GmcCMvX = i0_pla;
	prGmcPrm->i4GmcCMvY = j0_pla;

}

static void _PrsH263Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue, UCHAR ucEsId)
{
    UINT32 u4Temp;
    VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;

    *pi4RetValue = E_VDEC_OK;


    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);

#if 0
    while ((u4Temp & H263_VIDEO_START_MASK) != H263_VIDEO_START_CODE)
    {
        u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
    }
#endif 	

    if ((u4Temp & H263_VIDEO_START_MASK) == H263_VIDEO_START_CODE)
    {
        _PrsH263PicHdr(prDecPrm, pi4RetValue,ucEsId);
    }
    else
    {
    	LOG(0,"[H263]: Invalid Start Code %d\n",u4Temp);
		*pi4RetValue = E_VDEC_INVALID_START_CODE;
    }
#if 1  // ???0421 !!!
    prDecPrm->u4DramWidth = ((prDecPrm->u2Width % 16) != 0) ? 
    	(UINT16)(((prDecPrm->u2Width / 16) + 1) * 16) : (prDecPrm->u2Width);

    prDecPrm->u4DramHeight = ((prDecPrm->u2Height % 32) != 0) ? 
    	(UINT16)(((prDecPrm->u2Height / 32) + 1) * 32) : (prDecPrm->u2Height);
#endif
}

// Not Finished
static void _PrsH263PicHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 * pi4RetValue,UCHAR ucEsId)
{
    BOOL fgNotDisplay;
    UCHAR ucBRefFbId;
    UCHAR ucFRefFbId;                
    UINT8   u1FbgType;
    UINT32 u4Temp;
    VDEC_M4V_VOL_INFO_T	*prVolPrm;
    VDEC_M4V_PIC_INFO_T 	*prVopPrm;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;    
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    UINT32 u4FbWidth, u4FbHeight;
    UCHAR ucFbStatus;

    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;


    prVopPrm = &prDecPrm->rM4vParams.rVop;
    prVolPrm  = &prDecPrm->rM4vParams.rVol;
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwS);
#endif
    prVolPrm->fgShortHeader = TRUE;
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 18);
    prVopPrm->u1SplitScreenIndicator = (UINT8)((u4Temp >> 17) & 0x01);
    prVopPrm->u1DocCameraIndicator = (UINT8)((u4Temp >> 16) & 0x01);
    prVopPrm->u1SourceFormat = (UINT8)((u4Temp >> 12) & 0x07);
    prDecPrm->u1VopCodingType = (UINT8)((u4Temp >> 11) & 0x01);
    prDecPrm->u1VopQuant = (UINT8)((u4Temp >> 2) &0x1F);
	LOG(7,"Enter Prsh263PicHdr prDecPrm->u1VopCodingType:%d\n",prDecPrm->u1VopCodingType);

    if (!_SrcFmtTbl(prDecPrm))
    {
    	*pi4RetValue = E_VDEC_NOT_SUPPORT;
    	return;
    }
    
    if (prDecPrm->fgDecError == TRUE && 
        prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }
    else 
    {
        prDecPrm->fgDecError = FALSE;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if((!prVdecEsInfo) || (!prVdecEsInfoKeep))
    {
        LOG(1, "prVdecEsInfo null\n");
        return;
    }

     //only for LG
    if(prVdecEsInfoKeep->fgVPush)
    {
         if (prDecPrm->u4TotalFrameErrorCount > 100)
        {
            *pi4RetValue = E_VDEC_NOT_SUPPORT;
            return;
        }  
    }
    if (prDecPrm->u4FrameCount == 0)
    {
       // prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
      //  prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RELEASE_FBG))        
        {
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);    
            FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 20);    // block until display unlock                    
            FBM_ReleaseGroup(prDecPrm->u1FbgId);
            prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;
        }

        /*
        if (prVdecEsInfo->fgMMSeqInfoValid == TRUE)
        {
            prDecPrm->u2Width = prVdecEsInfo->rMMSeqInfo.u2_width;
            prDecPrm->u2Height = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            if (prDecPrm->u2Width == 0 || prDecPrm->u2Height == 0)
            {
                *pi4RetValue =  E_VDEC_FAIL;
                return;
            }
        }
        */
        
#if defined( CC_FBM_FULL_3D_SUPPORT) || defined (CC_SUPPORT_4K2K)
        if (prDecPrm->u2Width >= 3840 || prDecPrm->u2Height > 2160)
#else
        if (prDecPrm->u2Width > 1920 || prDecPrm->u2Height > 2160)
#endif
        {
            LOG(3, "Not Support Resolution\n");
            *pi4RetValue = E_VDEC_RES_NOT_SUPPORT;
            return;
        }

        u1FbgType = FBM_SelectGroupType(prDecPrm->u2Width, prDecPrm->u2Height);
        if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
        {
            {
                FBM_CREATE_FBG_PAR_T rPar;
                x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                rPar.u1VdecId = (UINT8)ucEsId;
                if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX;
                }
                else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                }
                else
                {
                    rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                }

                if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    LOG(3, "fgCreateFromInst == TRUE\n");
                    rPar.fgCreateFromInst = TRUE;
                }
                if (prVdecEsInfo->u4RenderPicCnt)
                {
                    rPar.fgThumbnailMode = TRUE;
                }

                prDecPrm->u1FbgId =
                    FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
            }
            FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                {
                    FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                }
                FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                }
            }
            //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
            if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
            {
                LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                *pi4RetValue = E_VDEC_FAIL;
                return;
            }

            prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
            FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
            FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
            prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
            prDecPrm->u1FbgType = u1FbgType;
            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);            
        }
        else
        {
            if (u1FbgType != prDecPrm->u1FbgType)
            {
                LOG(3, "Fbg Type Change %d\n", u1FbgType);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);

                FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 
                	20);    // block until display unlock            

                FBM_ReleaseGroup(prDecPrm->u1FbgId);
                prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;         

                {
                    FBM_CREATE_FBG_PAR_T rPar;
                    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                    rPar.u1VdecId = (UINT8)ucEsId;
                    if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX;
                    }
                    else if(prVdecEsInfoKeep->fgVPush)
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                    }
                    else
                    {
                        rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                    }

                    if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        LOG(3, "fgCreateFromInst == TRUE\n");
                        rPar.fgCreateFromInst = TRUE;
                    }
                    if (prVdecEsInfo->u4RenderPicCnt)
                    {
                        rPar.fgThumbnailMode = TRUE;
                    }

                    prDecPrm->u1FbgId =
                        FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
                }
                FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
                if(prVdecEsInfoKeep->fgVPush)
                {
                    if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                    {
                        FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                    }
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                    if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                    }
                }
                //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
                if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
                {
                    LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                    *pi4RetValue = E_VDEC_FAIL;
                    return;
                }
                prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
                FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
                FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
                FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
                prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
                FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);                 
            }
        }

        if (prFbmSeqHdr != NULL)
        {
            prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
            prFbmSeqHdr->fgPixelAspectRatio = TRUE;
            prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;        
            prFbmSeqHdr->fgProgressiveSeq = 1;
            
            if (prVdecEsInfo != 0)
            {
                /*switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: case VDEC_SRC_FRAME_RATE_24:
                    case VDEC_SRC_FRAME_RATE_25: case VDEC_SRC_FRAME_RATE_29_97:
                    case VDEC_SRC_FRAME_RATE_30: case VDEC_SRC_FRAME_RATE_50:
                    case VDEC_SRC_FRAME_RATE_59_94: case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = prVdecEsInfo->rMMSeqInfo.e_frame_rate;
                        break;
                    default:
                        prFbmSeqHdr->ucFrmRatCod = VDEC_SRC_FRAME_RATE_29_97;
                        LOG(1, "Upper layer did not set the frame rate\n");
                        break;
                }*/
                switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                        break;
                    case VDEC_SRC_FRAME_RATE_24:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                        break;
                    case VDEC_SRC_FRAME_RATE_25:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                        break;
                    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                        break;
                    case VDEC_SRC_FRAME_RATE_30:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                        break;
                    case VDEC_SRC_FRAME_RATE_50:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                        break;
                    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                        break;
                    case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                        break;
                    case VDEC_SRC_FRAME_RATE_12:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_12;
                       break;
                    case VDEC_SRC_FRAME_RATE_15:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
                       break;
                     case VDEC_SRC_FRAME_RATE_20:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_20;
                        break;
                    default:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                        break;
                }
#ifdef CC_RESIZE_SMALL_IMAGE
                 if(((prDecPrm->u2Width < FBM_IMG_RESIZE_LIMITATION) ||
                     (prDecPrm->u2Height < FBM_IMG_RESIZE_LIMITATION)) &&
                    (!((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))))
                {
                    prFbmSeqHdr->fgResizeSmallPic = TRUE;

                    if (prDecPrm->u2Width < FBM_IMG_RESIZE_LIMITATION)
                    {
                        prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                        prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                        prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    }
                    else if(prDecPrm->u2Width > FBM_IMG_RESIZE_LIMITATION_MAX)
                    {
                        prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                        prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                        prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    }
                    else
                    {
                        prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
                        prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                        prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                    }

                    if (prDecPrm->u2Height < FBM_IMG_RESIZE_LIMITATION)
                    {
                        prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                        prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                     //   prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
                    }
                    else if(prDecPrm->u2Height > FBM_IMG_RESIZE_LIMITATION_MAX)
                    {
                       // prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                        prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                        prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
                    }
                    else
                    {
                        prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                        prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                    }
                }
                else
                {

                    if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                    ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
                    {
                        prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                        prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                        prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
                        prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
                    }
                    else
                    {
                        prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                        prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                        prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                        prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                    }
                    prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
                }
#else
                    prFbmSeqHdr->fgProgressiveSeq = 1;
                if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                    ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
                {
                    prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                    prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
                    prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
                }
                else
                {
                    prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                    prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                }
                    prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
#endif
            }
            else
            {
                    // Did not parse the frame rate from ES so far
            	  //FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);   
              	  prFbmSeqHdr->ucFrmRatCod = 5;        // 30 fps
            	  prFbmSeqHdr->fgProgressiveSeq = 1;
            	  prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
            	  prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
            	  prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
            	  prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
            	  prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            	  //prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
            	  //FBM_PicStructChgNotify(0, MPEG_PIC_FRAME);
            }
/* need mentor's double check, follow Jason's mail: "About CR review on 20110316", B2R/VDEC's linesize must be 64 px. alignment */
	    prFbmSeqHdr->u2LineSize = (((prFbmSeqHdr->u2LineSize + 63) >> 6) << 6);

        }
#ifdef IC_MPEG4_VERIFICATION
        prDecPrm->u4LineSize = prDecPrm->u2Width;
#endif
    }

    if (prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN)
    {
        // Check if reset is required
        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET))
        {
            FBM_ResetGroup(prDecPrm->u1FbgId);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_NOT_READY);
    //            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_CLEAR_FRC_PRM);

            /*if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC))
            {
                _MPV_SkipPic(*pucMpvId, prVdecEsInfo->u4EsId);
                *pucMpvId = VDEC_MAX_VLD;
            }*/                        
        }
    }

	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    #if 0
    if(prVdecEsInfo && prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {
        *pi4RetValue = E_VDEC_PARSE_NOT_READY;
        return;
    }
    #endif
	
    FBM_GetRefFrameBuffer(prDecPrm->u1FbgId, &ucFRefFbId, &ucBRefFbId);
    prDecPrm->u4FrameCount ++;
    // Video's PTS
    if (ucBRefFbId != FBM_FBG_ID_UNKNOWN)
    {
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        if (!prVdecEsInfo)
        {
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }

        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
        if (prFbmPicHdr)
        {

            if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
            ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
            {
                prFbmPicHdr->u4PicWidth = prVdecEsInfo->rMMSeqInfo.u2_width;
                prFbmPicHdr->u4PicHeight = prVdecEsInfo->rMMSeqInfo.u2_height;
            }
            else
            {
                prFbmPicHdr->u4PicWidth = prDecPrm->u2Width;
                prFbmPicHdr->u4PicHeight = prDecPrm->u2Height;
            }
            prFbmPicHdr->u4PicWidthPitch = prDecPrm->u4LineSize;
            x_memcpy(&prFbmPicHdr->rExtra,
                    &prDecPrm->rRecExtra,
                    sizeof(FBM_PIC_EXTRA_INFO_T));
            prFbmPicHdr->u4PTS = prDecPrm->u4RefPts;
            prFbmPicHdr->u8PTS = prDecPrm->u8RefPts;
            prFbmPicHdr->u8Offset = prDecPrm->u8RecOffset;
            prFbmPicHdr->u8OffsetI = prDecPrm->u8RecOffsetI;
            prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
        }
        else
        {
            LOG(3, "_PrsVOPHdr prFbmPicHdr == NULL\n");
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }

        fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prDecPrm->u4RefPts, 0, 0);
        if (fgNotDisplay)
        {
            /*FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, 
                ucBRefFbId, 
                FBM_MM_NOT_DISPLAY_FLAG);

            LOG(6, "_PrsVOPHdr fgNotDisplay ucFbId %d\n", ucBRefFbId);*/
            
            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
            if ((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) ||
                (ucFbStatus != FBM_FB_STATUS_LOCK))
            {
                FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                   ucBRefFbId, FBM_FB_STATUS_EMPTY);
            }
            else
            {
                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            }
        }
        else
        {
            LOG(6, "_PrsH263Hdr prFbmPicHdr->u4PTS = %ld\n", prFbmPicHdr->u4PTS);

            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
             if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
			 (ucFbStatus == FBM_FB_STATUS_READY))
            {
                #ifdef CC_REALD_3D_SUPPORT
                _VDEC_Check2D3DSwitch(ucEsId, ucBRefFbId);
                #endif
                FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                       ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);
				if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
				{
					LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
						   prDecPrm->u1VopCodingType,
						   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
						   prDecPrm->u1FbgId, ucBRefFbId,
						   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
						   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0
						   );
				}

                prVdecEsInfo->u4DisplayQPicCnt++;
            }
            else
            {
                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            }
        }
        UNUSED(VDEC_CheckStopStatus(ucEsId, prDecPrm->u4RefPts, 0));
    }
    // FBM error
    else if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        LOG(3, "Wrong reference for P VOP\n");
        *pi4RetValue = E_VDEC_FAIL;
        
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        if(NULL != prVdecEsInfoKeep && prVdecEsInfoKeep->fgVPush) //LG only
        {
            prDecPrm->fgNotDisplayDummyFrame = TRUE;
        }

        return;
    }

    prDecPrm->u8RecOffsetI = prDecPrm->rPesInfo.u8OffsetI;
    prDecPrm->rRecExtra = prDecPrm->rPesInfo.rExtra;
   
   prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;
    prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
    prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
    prDecPrm->fgValidPts = prDecPrm->rPesInfo.fgDtsValid;

    LOG(6, "ucFRefFbId = 0x%08x\n", ucFRefFbId);
    LOG(6, "ucBRefFbId = 0x%08x\n", ucBRefFbId);

     while(u4Temp & 0x01)  // pei
    {
    	_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 9);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    }

    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);

    LOG(6, "_PrsH263PicHdr prDecPrm->u4RefPts = %ld\n", prDecPrm->u4RefPts);
    _VDEC_GetVldRPtr(&prDecPrm->u4DecReadAddr, &prDecPrm->u4DecReadBits, prDecPrm->rPesInfo.u4FifoStart,prDecPrm->rPesInfo.u4FifoEnd);
    if(prDecPrm->u4DecReadAddr< prDecPrm->rPesInfo.u4FifoStart)
    {
          prDecPrm->u4DecReadAddr = prDecPrm->u4DecReadAddr+
                              (prDecPrm->rPesInfo.u4FifoEnd-prDecPrm->rPesInfo.u4FifoStart);
    }

    if (prDecPrm->u4LockCount > 0)
    {
        _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
        prDecPrm->u4LockCount--;
    }
        
#ifdef VDEC_TIME_PROFILE
     HAL_GetTime(&rTimeFbS);
#endif
    
    prDecPrm->u1FbId = FBM_GetEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbE);
        HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
        prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
#endif

	_CreateWorkingBuffer(prDecPrm);

#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwE);
        HAL_GetDeltaTime(&rTimeSwDt, &rTimeSwS, &rTimeSwE);
        u4SWTimeMicro += rTimeSwDt.u4Micros;
        if(prFbmSeqHdr->fgProgressiveSeq)
        {
            prDecPrm->u4CurSwMicro = u4SWTimeMicro - prDecPrm->u4CurWaitFbMicro;
            u4SWTimeMicro = 0;
            prDecPrm->u4CurWaitFbMicro = 0;
        }
    
#endif


    // Set some default values for short headers
    prDecPrm->u1VopRoundingType = 0;

    prVolPrm->u1QuantType = FALSE;
    prVolPrm->fgObmcDisable = TRUE;
    prVolPrm->fgResyncMarkerDisable = TRUE;
    prVolPrm->fgDataPartitioned = FALSE;
    prVolPrm->fgReversibleVlc = FALSE;
    prVolPrm->fgInterlaced = FALSE;
    prVopPrm->fgTopFldFirst = FALSE;
    prVopPrm->u1ForwardFCode = 1;
    prDecPrm->u1PicStructure = MP4_FRM_PIC;

    /*if (prDecPrm->rPesInfo.fgEos)
    {
        FBM_SetFrameBufferPicFlag(prDecPrm->u1FbgId, prDecPrm->u1FbId, FBM_MM_EOS_FLAG);   
    }*/
}

#ifdef CC_IS_MPEG4_POST_MT5363
static void _PrsFlvHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue, UCHAR ucEsId)
{
	UINT32 u4Temp;
	VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;

	*pi4RetValue = E_VDEC_OK;

	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);

	while ((u4Temp & H263_VIDEO_START_MASK) != FLV_VIDEO_START_CODE)
	{
		u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8);
	}

	if ((u4Temp & H263_VIDEO_START_MASK) == FLV_VIDEO_START_CODE)
	{
		_PrsFlvPicHdr(prDecPrm, pi4RetValue, ucEsId);
	}
	
}

// Not Finished
static void _PrsFlvPicHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32 * pi4RetValue, UCHAR ucEsId)
{
    //BOOL fgNotDisplay;
    UCHAR ucBRefFbId;
    UCHAR ucFRefFbId;                
    UINT8   u1FbgType;
    UINT32 u4Temp;
    VDEC_M4V_VOL_INFO_T	*prVolPrm;
    VDEC_M4V_PIC_INFO_T 	*prVopPrm;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;    
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    UINT32 u4FbWidth, u4FbHeight;
    BOOL fgNotDisplay;
    UCHAR ucFbStatus;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    prVopPrm = &prDecPrm->rM4vParams.rVop;
    prVolPrm  = &prDecPrm->rM4vParams.rVol;
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwS);
#endif
    prVolPrm->fgShortHeader = TRUE;
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    prVopPrm->u1SourceFormat = (UINT8)(u4Temp & 0x07);

    if (!_FlvSrcFmtTbl(prDecPrm))
    {
        //Printf("format is not support\n");
    	*pi4RetValue = E_VDEC_NOT_SUPPORT;
    	return;
    }

    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 9);

    prDecPrm->u1VopCodingType = (UINT8)((u4Temp >> 7) & 0x03);
    prDecPrm->u1VopQuant = (UINT8)((u4Temp >> 1) &0x1F);
    
	LOG(7,"Enter PrsFlvPicHdr prDecPrm->u1VopCodingType:%d\n",prDecPrm->u1VopCodingType);
    if(prDecPrm->u1VopCodingType == 2)
    {
    	  prDecPrm->u1VopCodingType = 1;
    }

    if (prDecPrm->fgDecError == TRUE && 
        prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }
    else 
    {
        prDecPrm->fgDecError = FALSE;
    }
    
    if (prDecPrm->u4FrameCount == 0)
    {
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RELEASE_FBG))        
        {
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);    
            FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 20);    // block until display unlock                    
            FBM_ReleaseGroup(prDecPrm->u1FbgId);
            prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;
        }

/*
        if (prVdecEsInfo->fgMMSeqInfoValid == TRUE)
        {
            prDecPrm->u2Width = prVdecEsInfo->rMMSeqInfo.u2_width;
            prDecPrm->u2Height = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            if (prDecPrm->u2Width == 0 || prDecPrm->u2Height == 0)
            {
                *pi4RetValue =  E_VDEC_FAIL;
                return;
            }
        }
*/
        prDecPrm->u4DramWidth = ((prDecPrm->u2Width % 16) != 0) ? 
    	    (UINT16)(((prDecPrm->u2Width / 16) + 1) * 16) : (prDecPrm->u2Width);

        prDecPrm->u4DramHeight = ((prDecPrm->u2Height % 32) != 0) ? 
    	    (UINT16)(((prDecPrm->u2Height / 32) + 1) * 32) : (prDecPrm->u2Height);

#if defined( CC_FBM_FULL_3D_SUPPORT) || defined (CC_SUPPORT_4K2K)
        if (prDecPrm->u2Width >= 3840 || prDecPrm->u2Height > 2160)
#else
        if (prDecPrm->u2Width > 1920 || prDecPrm->u2Height > 2160)
#endif
        {
            LOG(3, "Not Support Resolution\n");
            *pi4RetValue = E_VDEC_RES_NOT_SUPPORT;
            return;
        }

        u1FbgType = FBM_SelectGroupType(prDecPrm->u2Width, prDecPrm->u2Height);
        if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
        {
            {
                FBM_CREATE_FBG_PAR_T rPar;
                x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                rPar.u1VdecId = (UINT8)ucEsId;
                if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX;
                }
                else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                }
                else
                {
                    rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                }

                if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    LOG(6, "fgCreateFromInst == TRUE\n");
                    rPar.fgCreateFromInst = TRUE;
                }
                if (prVdecEsInfo->u4RenderPicCnt)
                {
                    rPar.fgThumbnailMode = TRUE;
                }

                prDecPrm->u1FbgId =
                    FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
            }
            FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                {
                    FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                }
                FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                }
            }
            //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
            if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
            {
                LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                *pi4RetValue = E_VDEC_FAIL;
                return;
            }

            prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
            FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
            FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
            prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
            prDecPrm->u1FbgType = u1FbgType;
            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);

#ifdef CC_MPEG4_DEBLOCKING
            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_DEBLOCKING);
            LOG(6, "DBK Enable\n");
#endif
        }
        else
        {
            if (u1FbgType != prDecPrm->u1FbgType)
            {
                LOG(3, "Fbg Type Change %d\n", u1FbgType);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);

                FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 
                	20);    // block until display unlock            

                FBM_ReleaseGroup(prDecPrm->u1FbgId);
                prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;         

                {
                    FBM_CREATE_FBG_PAR_T rPar;
                    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                    rPar.u1VdecId = (UINT8)ucEsId;
                    if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX;
                    }
                    else if(prVdecEsInfoKeep->fgVPush)
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                    }
                    else
                    {
                        rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                    }

                    if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        LOG(3, "fgCreateFromInst == TRUE\n");
                        rPar.fgCreateFromInst = TRUE;
                    }
                    if (prVdecEsInfo->u4RenderPicCnt)
                    {
                        rPar.fgThumbnailMode = TRUE;
                    }

                    prDecPrm->u1FbgId =
                        FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
                }
                FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
                if(prVdecEsInfoKeep->fgVPush)
                {
                    if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                    {
                        FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                    }
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                    if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                    }
                }
                //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
                if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
                {
                    LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                    *pi4RetValue = E_VDEC_FAIL;
                    return;
                }
                prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
                FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
                FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
                FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
                prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
                FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);                 
#ifdef CC_MPEG4_DEBLOCKING
                FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_DEBLOCKING);
                LOG(6, "DBK Enable\n");
#endif
            }
        }

        if (prFbmSeqHdr != NULL)
        {
            if (prVdecEsInfo != 0)
            {
                prFbmSeqHdr->ucAspRatInf = 1;        // 16:9
                prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;        // 16:9

                /*switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: case VDEC_SRC_FRAME_RATE_24:
                    case VDEC_SRC_FRAME_RATE_25: case VDEC_SRC_FRAME_RATE_29_97:
                    case VDEC_SRC_FRAME_RATE_30: case VDEC_SRC_FRAME_RATE_50:
                    case VDEC_SRC_FRAME_RATE_59_94: case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = prVdecEsInfo->rMMSeqInfo.e_frame_rate;
                        break;
                    default:
                        prFbmSeqHdr->ucFrmRatCod = VDEC_SRC_FRAME_RATE_29_97;
                        LOG(1, "Upper layer did not set the frame rate\n");
                        break;
                }*/
                switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                        break;
                    case VDEC_SRC_FRAME_RATE_24:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                        break;
                    case VDEC_SRC_FRAME_RATE_25:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                        break;
                    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                        break;
                    case VDEC_SRC_FRAME_RATE_30:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                        break;
                    case VDEC_SRC_FRAME_RATE_50:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                        break;
                    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                        break;
                    case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                        break;
                    case VDEC_SRC_FRAME_RATE_12:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_12;
                       break;
                    case VDEC_SRC_FRAME_RATE_15:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
                       break;
                     case VDEC_SRC_FRAME_RATE_20:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_20;
                        break;
                    default:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                        break;
                }
                prFbmSeqHdr->fgProgressiveSeq = 1;
                prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            }
            else
            {
                    // Did not parse the frame rate from ES so far
            	  //FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);   
              	  prFbmSeqHdr->ucAspRatInf = 1;        // 16:9
              	  prFbmSeqHdr->ucFrmRatCod = 5;        // 30 fps
            	  prFbmSeqHdr->fgProgressiveSeq = 1;
            	  prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
            	  prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
            	  prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
            	  prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
            	  prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            	  //prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
            	  //FBM_PicStructChgNotify(0, MPEG_PIC_FRAME);
            }
/* need mentor's double check, follow Jason's mail: "About CR review on 20110316", B2R/VDEC's linesize must be 64 px. alignment */
	    prFbmSeqHdr->u2LineSize = (((prFbmSeqHdr->u2LineSize + 63) >> 6) << 6);

        }
#ifdef IC_MPEG4_VERIFICATION
               prDecPrm->u4LineSize = prDecPrm->u2Width;
#endif
    }

    if (prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN)
    {
        // Check if reset is required
        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET))
        {
            FBM_ResetGroup(prDecPrm->u1FbgId);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_NOT_READY);
    //            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_CLEAR_FRC_PRM);

            /*if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC))
            {
                _MPV_SkipPic(*pucMpvId, prVdecEsInfo->u4EsId);
                *pucMpvId = VDEC_MAX_VLD;
            }*/                        
        }
    }

	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    #if 0
    if(prVdecEsInfo && prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {
        *pi4RetValue = E_VDEC_PARSE_NOT_READY;
        return;
    }
    #endif
	
    FBM_GetRefFrameBuffer(prDecPrm->u1FbgId, &ucFRefFbId, &ucBRefFbId);

    // Video's PTS
    if (ucBRefFbId != FBM_FBG_ID_UNKNOWN)
    {
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        if (!prVdecEsInfo)
        {
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }

        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
        if (prFbmPicHdr)
        {
 #if 1
            prFbmPicHdr->u4PicWidth = prDecPrm->u2Width;
            prFbmPicHdr->u4PicHeight = prDecPrm->u2Height;
            prFbmPicHdr->u4PicWidthPitch = prDecPrm->u4LineSize;
#endif
            x_memcpy(&prFbmPicHdr->rExtra,
                    &prDecPrm->rRecExtra,
                    sizeof(FBM_PIC_EXTRA_INFO_T));

            prFbmPicHdr->u8Offset = prDecPrm->u8RecOffset;
            prFbmPicHdr->u8OffsetI = prDecPrm->u8RecOffsetI;
            prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
            prFbmPicHdr->u4PTS = prDecPrm->u4RefPts;
            prFbmPicHdr->u8PTS = prDecPrm->u8RefPts;
        }
        else
        {
            LOG(3, "_PrsVOPHdr prFbmPicHdr == NULL\n");
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }

#if 1
        fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prDecPrm->u4RefPts, 0,0);
        if (fgNotDisplay)
        {

            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
            if ((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) ||
                (ucFbStatus != FBM_FB_STATUS_LOCK))
            {
                FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                   ucBRefFbId, FBM_FB_STATUS_EMPTY);
            }
            else
            {
                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            }
        }
        else
#endif
        {
            LOG(6, "PrsFlvPicHdr prFbmPicHdr->u4PTS = %ld\n", prFbmPicHdr->u4PTS);
             ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
             LOG(6, "esid = %d ucFbStatus = %d\n",ucEsId,ucFbStatus);
             if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
                (ucFbStatus == FBM_FB_STATUS_READY))
             {

#ifdef CC_MPEG4_DEBLOCKING
                    #ifdef CC_REALD_3D_SUPPORT
                    _VDEC_Check2D3DSwitch(ucEsId, prDecPrm->u1DbkFbId);
                    #endif
                    FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                                        prDecPrm->u1DbkFbId, FBM_FB_STATUS_DISPLAYQ);
					if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
					{
						LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
							   prDecPrm?prDecPrm->u1VopCodingType:0,
							   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
							   prDecPrm->u1FbgId, prDecPrm->u1DbkFbId,
							   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
							   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0
							   );
					}

#else
                    #ifdef CC_REALD_3D_SUPPORT
                    _VDEC_Check2D3DSwitch(ucEsId, ucBRefFbId);
                    #endif
                    FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                    							ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);
					if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
					{
						LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
							   prDecPrm->u1VopCodingType,
							   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
							   prDecPrm->u1FbgId, ucBRefFbId,
							   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
							   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0
							   );
					}

#endif
                    //Printf("_PrsH263Hdr prFbmPicHdr->u4PTS = %ld\n", prFbmPicHdr->u4PTS);
                    prVdecEsInfo->u4DisplayQPicCnt++;     
            }
            LOG(3,"esid = %d,VDEC_CheckStopStatus invoked from %s line:%d\n",ucEsId,__func__,__LINE__);
            VDEC_CheckStopStatus(ucEsId, prDecPrm->u4RefPts, 0);
        }
    }
    // FBM error
    else if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        LOG(3, "Wrong reference for P VOP\n");
        *pi4RetValue = E_VDEC_FAIL;
        
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        if(NULL != prVdecEsInfoKeep && prVdecEsInfoKeep->fgVPush) //LG only
        {
            prDecPrm->fgNotDisplayDummyFrame = TRUE;
        }

        return;
    }

    prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
    prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
    prDecPrm->rRecExtra = prDecPrm->rPesInfo.rExtra;
    prDecPrm->u8RecOffsetI = prDecPrm->rPesInfo.u8OffsetI;
    prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;

    LOG(6, "ucFRefFbId = 0x%08x\n", ucFRefFbId);
    LOG(6, "ucBRefFbId = 0x%08x\n", ucBRefFbId);

    LOG(6, "PrsFlvPicHdr prDecPrm->u4RefPts = %ld\n", prDecPrm->u4RefPts);

    
    while(u4Temp & 0x01)  // pei
    {
    	_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 9);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    }

    _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 32);
    
    _VDEC_GetVldRPtr(&prDecPrm->u4DecReadAddr, &prDecPrm->u4DecReadBits, prDecPrm->rPesInfo.u4FifoStart,prDecPrm->rPesInfo.u4FifoEnd);

    if(prDecPrm->u4DecReadAddr< prDecPrm->rPesInfo.u4FifoStart)
    {
          prDecPrm->u4DecReadAddr = prDecPrm->u4DecReadAddr+
                              (prDecPrm->rPesInfo.u4FifoEnd-prDecPrm->rPesInfo.u4FifoStart);
    }
    if (prDecPrm->u4LockCount > 0)
    {
        _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
        prDecPrm->u4LockCount--;
    }
    
#ifdef VDEC_TIME_PROFILE
            HAL_GetTime(&rTimeFbS);
#endif

    prDecPrm->u1FbId = FBM_GetEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#ifdef CC_MPEG4_DEBLOCKING
    prDecPrm->u1DbkFbId = FBM_GetDbkEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#endif

#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbE);
        HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
        prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
#endif

	_CreateWorkingBuffer(prDecPrm);

#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwE);
        HAL_GetDeltaTime(&rTimeSwDt, &rTimeSwS, &rTimeSwE);
        u4SWTimeMicro += rTimeSwDt.u4Micros;
        if(prFbmSeqHdr->fgProgressiveSeq)
        {
            prDecPrm->u4CurSwMicro = u4SWTimeMicro - prDecPrm->u4CurWaitFbMicro;
            u4SWTimeMicro = 0;
            prDecPrm->u4CurWaitFbMicro = 0;
        }
    
#endif
    // Set some default values for short headers
    prDecPrm->u1VopRoundingType = 0;

    prVolPrm->u1QuantType = FALSE;
    prVolPrm->fgObmcDisable = TRUE;
    prVolPrm->fgResyncMarkerDisable = TRUE;
    prVolPrm->fgDataPartitioned = FALSE;
    prVolPrm->fgReversibleVlc = FALSE;
    prVolPrm->fgInterlaced = FALSE;
    prVopPrm->fgTopFldFirst = FALSE;
    prVopPrm->u1ForwardFCode = 1;
    prDecPrm->u1PicStructure = MP4_FRM_PIC;
    prDecPrm->fgValidPts = prDecPrm->rPesInfo.fgDtsValid;
    prDecPrm->u4FrameCount++;
    /*if (prDecPrm->rPesInfo.fgEos)
    {
        FBM_SetFrameBufferPicFlag(prDecPrm->u1FbgId, prDecPrm->u1FbId, FBM_MM_EOS_FLAG);   
    }*/
}
#endif

static void _PrsDx311Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    BOOL fgNotDisplay;
    UCHAR ucBRefFbId;
    UCHAR ucFRefFbId;                
    UINT8   u1FbgType;
    UINT32 u4Temp, u4RetValue;
    UINT32 u4FbWidth, u4FbHeight;
    VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL; 
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);    
    if((!prVdecEsInfo) || (!prVdecEsInfoKeep))
    {
        LOG(1, "prVdecEsInfo null\n");
        return;
    }
    #ifdef CC_ENABLE_CHECK_DX311_ERROR_COUNT
    if (prDecPrm->u4TotalFrameErrorCount > 500)
    {
        *pi4RetValue = E_VDEC_NOT_SUPPORT;
        return;
    }    
    #endif
    
    #ifdef IC_MPEG4_VERIFICATION
    prDecPrm->u2Width = (UINT16)_u4Dx311PicWidth;
    prDecPrm->u2Height = (UINT16)_u4Dx311PicHeight;
    #endif
    
    prDecPrm->u1PicStructure = MP4_FRM_PIC;
    prDecPrm->u4FrameCount++;

    // skip Compression and SizeImage
    //_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 64); 
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwS);
#endif
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);
    u4RetValue = u4Temp >> 30;
    prDecPrm->u1VopCodingType = (UINT8)(u4RetValue & 0x1);
	
	LOG(7,"Enter PrsDx311Hdr prDecPrm->u1VopCodingType:%d\n",prDecPrm->u1VopCodingType);
    // Temporally mark
    if(u4RetValue & 0x2)
    {
    	//prDecPrm->fgVopNotCoded = TRUE;
       prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
       prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
    	prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;
    	*pi4RetValue = E_VDEC_NOT_CODED_FRAME;
    	return;
    }

     if (prDecPrm->fgDecError == TRUE && 
        prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        *pi4RetValue = E_VDEC_FAIL;
        return;
    }
    else 
    {
        prDecPrm->fgDecError = FALSE;
    }

    prDecPrm->u4DramWidth = ((prDecPrm->u2Width % 16) != 0) ? 
      	(UINT16)(((prDecPrm->u2Width / 16) + 1) * 16) : (prDecPrm->u2Width);

    prDecPrm->u4DramHeight = ((prDecPrm->u2Height % 32) != 0) ? 
      	(UINT16)(((prDecPrm->u2Height / 32) + 1) * 32) : (prDecPrm->u2Height);

    if (prDecPrm->u4FrameCount == 1)
    {
        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RELEASE_FBG))        
        {
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);    
            FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 20);    // block until display unlock                    
            FBM_ReleaseGroup(prDecPrm->u1FbgId);
            prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;
        }

        if (prVdecEsInfo->fgMMSeqInfoValid == TRUE)
        {
            prDecPrm->u2Width = prVdecEsInfo->rMMSeqInfo.u2_width;
            prDecPrm->u2Height = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            if (prDecPrm->u2Width == 0 || prDecPrm->u2Height == 0)
            {
                *pi4RetValue =  E_VDEC_FAIL;
                return;
            }
        }

#if defined( CC_FBM_FULL_3D_SUPPORT) || defined (CC_SUPPORT_4K2K)
        if (prDecPrm->u2Width >= 3840 || prDecPrm->u2Height > 2160)
#else
        if (prDecPrm->u2Width > 1920 || prDecPrm->u2Height > 2160)
#endif
        {
            LOG(3, "Not Support Resolution\n");
            *pi4RetValue = E_VDEC_RES_NOT_SUPPORT;
            return;
        }

        u1FbgType = FBM_SelectGroupType(prDecPrm->u2Width, prDecPrm->u2Height);
        if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
        {
            {
                FBM_CREATE_FBG_PAR_T rPar;
                x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                rPar.u1VdecId = (UINT8)ucEsId;
                if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX;
                }
                else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL) )
                {
                    rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                }
                else
                {
                    rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                }

                if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    LOG(3, "fgCreateFromInst == TRUE\n");
                    rPar.fgCreateFromInst = TRUE;
                }
                if (prVdecEsInfo->u4RenderPicCnt)
                {
                    rPar.fgThumbnailMode = TRUE;
                }

                prDecPrm->u1FbgId =
                    FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
            }
            FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                {
                    FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                }
                FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                }
            }
            //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
            if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
            {
                LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                *pi4RetValue = E_VDEC_FAIL;
                return;
            }
            prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
            FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
            FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
            prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
            prDecPrm->u1FbgType = u1FbgType;
            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);             
        }
        else
        {
            if (u1FbgType != prDecPrm->u1FbgType)
            {
                LOG(3, "Fbg Type Change %d\n", u1FbgType);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);

                FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId, 
                	20);    // block until display unlock            

                FBM_ReleaseGroup(prDecPrm->u1FbgId);
                prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;         

                {
                    FBM_CREATE_FBG_PAR_T rPar;
                    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                    rPar.u1VdecId = (UINT8)ucEsId;
                    if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX;
                    }
                    else if(prVdecEsInfoKeep->fgVPush)
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                    }
                    else
                    {
                        rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                    }

                    if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        LOG(3, "fgCreateFromInst == TRUE\n");
                        rPar.fgCreateFromInst = TRUE;
                    }
                    if (prVdecEsInfo->u4RenderPicCnt)
                    {
                        rPar.fgThumbnailMode = TRUE;
                    }

                    prDecPrm->u1FbgId =
                        FBM_CreateGroupExt(u1FbgType, FBM_VDEC_MPEG4, prDecPrm->u2Width, prDecPrm->u2Height, &rPar);
                }
                FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_MM_MODE);
                if(prVdecEsInfoKeep->fgVPush)
                {
                    if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                    {
                        FBM_SetPlayMode(prDecPrm->u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                    }
                    FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                    if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        FBM_RegFbgCbFunc(prDecPrm->u1FbgId,
                        FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                    }
                }
                //ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);  
                if (prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN)
                {
                    LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
                    *pi4RetValue = E_VDEC_FAIL;
                    return;
                }
                prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
                FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
                FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
                FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
                prDecPrm->u4LineSize = ((prDecPrm->u2Width + 63)/64)*64;
                FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);                    
             }
        }

        if (prFbmSeqHdr != NULL)
        {
           prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
           prFbmSeqHdr->fgPixelAspectRatio = TRUE;
           prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;        // 16:9
           
            if (prVdecEsInfo != 0)
            {
                /*switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: case VDEC_SRC_FRAME_RATE_24:
                    case VDEC_SRC_FRAME_RATE_25: case VDEC_SRC_FRAME_RATE_29_97:
                    case VDEC_SRC_FRAME_RATE_30: case VDEC_SRC_FRAME_RATE_50:
                    case VDEC_SRC_FRAME_RATE_59_94: case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = prVdecEsInfo->rMMSeqInfo.e_frame_rate;
                        break;
                    default:
                        prFbmSeqHdr->ucFrmRatCod = VDEC_SRC_FRAME_RATE_29_97;
                        LOG(1, "Upper layer did not set the frame rate\n");
                        break;
                }*/
                switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                        break;
                    case VDEC_SRC_FRAME_RATE_24:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                        break;
                    case VDEC_SRC_FRAME_RATE_25:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                        break;
                    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                        break;
                    case VDEC_SRC_FRAME_RATE_30:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                        break;
                    case VDEC_SRC_FRAME_RATE_50:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                        break;
                    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                        break;
                    case VDEC_SRC_FRAME_RATE_60:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                        break;
                    case VDEC_SRC_FRAME_RATE_12:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_12;
                       break;
                    case VDEC_SRC_FRAME_RATE_15:
                       prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
                       break;
                     case VDEC_SRC_FRAME_RATE_20:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_20;
                        break;
                     default:
                        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                        break;
                }

                prFbmSeqHdr->fgProgressiveSeq = 1;

                if((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                    ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) && (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
                {
                    prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
                    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
                    prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
                    prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
                }
                else
                {
                    prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
                    prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
                    prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
                }
                prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
            }
            else
            {
                    // Did not parse the frame rate from ES so far
    	  	  //FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);   
    	         prFbmSeqHdr->ucFrmRatCod = 5;        // 30 fps
    	  	  prFbmSeqHdr->fgProgressiveSeq = 1;
    	  	  prFbmSeqHdr->u2HSize = prDecPrm->u2Width;
    	  	  prFbmSeqHdr->u2VSize = prDecPrm->u2Height;
    	  	  prFbmSeqHdr->u2OrgHSize = prDecPrm->u2Width;
    	  	  prFbmSeqHdr->u2OrgVSize = prDecPrm->u2Height;
    	  	  prFbmSeqHdr->u2LineSize = (UINT16)prDecPrm->u4LineSize;
    	  	  //prFbmSeqHdr->u2LineSize = prDecPrm->u4LineSize;
    	  	  //FBM_PicStructChgNotify(0, MPEG_PIC_FRAME);
            }
 /* need mentor's double check, follow Jason's mail: "About CR review on 20110316", B2R/VDEC's linesize must be 64 px. alignment */
	    prFbmSeqHdr->u2LineSize = (((prFbmSeqHdr->u2LineSize + 63) >> 6) << 6);

        }
    }

    if (prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN)
    {
        // Check if reset is required
        if (FBM_ChkFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET))
        {
            FBM_ResetGroup(prDecPrm->u1FbgId);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_RESET);
            FBM_ClrFrameBufferFlag(prDecPrm->u1FbgId, (UINT32)FBM_FLAG_NOT_READY);
    //            FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_CLEAR_FRC_PRM);

            /*if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC))
            {
                _MPV_SkipPic(*pucMpvId, prVdecEsInfo->u4EsId);
                *pucMpvId = VDEC_MAX_VLD;
            }*/                        
        }
    }

#if 0
	if(prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {
        prDecPrm->u4FrameCount = 0;
        *pi4RetValue = E_VDEC_PARSE_NOT_READY;
        return;
    }
#endif

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    FBM_GetRefFrameBuffer(prDecPrm->u1FbgId, &ucFRefFbId, &ucBRefFbId);

    // Video's PTS
    if (ucBRefFbId != FBM_FBG_ID_UNKNOWN)
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
        if (prFbmPicHdr)
        {
            prFbmPicHdr->u4PTS = prDecPrm->u4RefPts;
            prFbmPicHdr->u8PTS = prDecPrm->u8RefPts;
            x_memcpy(&prFbmPicHdr->rExtra,
                &prDecPrm->rRecExtra,
                sizeof(FBM_PIC_EXTRA_INFO_T));
            prFbmPicHdr->u8Offset = prDecPrm->u8RecOffset;
            prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->u8OffsetLast;
            prFbmPicHdr->u8OffsetI = prDecPrm->u8RecOffsetI;
            prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
#if 1
            prFbmPicHdr->u4PicWidth = prDecPrm->u2Width;
            prFbmPicHdr->u4PicHeight = prDecPrm->u2Height;
            prFbmPicHdr->u4PicWidthPitch = prDecPrm->u4LineSize;
#endif
            prFbmPicHdr->ucTrueZeroPTS = FBM_USE_ZERO_PTS_IF_ZERO;
        }
        else
        {
            LOG(3, "_PrsVOPHdr prFbmPicHdr == NULL\n");
            *pi4RetValue = E_VDEC_FAIL;
            return;
        }

        fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prDecPrm->u4RefPts, 0, 0);
        if (fgNotDisplay)
        {
            /*FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, 
                ucBRefFbId, 
                FBM_MM_NOT_DISPLAY_FLAG);

            LOG(6, "_PrsVOPHdr fgNotDisplay ucFbId %d\n", ucBRefFbId);*/
            UCHAR ucFbStatus;
            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
            if ((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) ||
                (ucFbStatus != FBM_FB_STATUS_LOCK))
            {
                FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                   ucBRefFbId, FBM_FB_STATUS_EMPTY);
            }
            else
            {
                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            }
        }
        else
        {
            UCHAR ucFbStatus;
            LOG(6, "_PrsDx311Hdr prFbmPicHdr->u4PTS = %ld\n", prFbmPicHdr->u4PTS);

            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->u1FbgId, ucBRefFbId);
            if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
			 (ucFbStatus == FBM_FB_STATUS_READY))
            {
                #ifdef CC_REALD_3D_SUPPORT
                _VDEC_Check2D3DSwitch(ucEsId, ucBRefFbId);
                #endif
                FBM_SetFrameBufferStatus(prDecPrm->u1FbgId,
                       ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);
				if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
				{
					LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
						   prDecPrm->u1VopCodingType,
						   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
						   prDecPrm->u1FbgId, ucBRefFbId,
						   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
						   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0
						   );
				}
                prVdecEsInfo->u4DisplayQPicCnt++;
            }
            else
            {
                LOG(6, "Frame buffer status = %x\n", ucFbStatus);
            }
        }
        VDEC_CheckStopStatus(ucEsId, prDecPrm->u4RefPts, 0);
    }
    // FBM error
    else if (prDecPrm->u1VopCodingType != VOP_CODING_TYPE_I)
    {
        LOG(3, "Wrong reference for P VOP\n");
        *pi4RetValue = E_VDEC_FAIL;
        if(prVdecEsInfoKeep->fgVPush) //LG only
        {
            prDecPrm->fgNotDisplayDummyFrame = TRUE;
        }

        return;
    }
    
#ifdef IC_MPEG4_VERIFICATION
        prDecPrm->u4LineSize = prDecPrm->u2Width;
#endif

    prDecPrm->fgValidPts = prDecPrm->rPesInfo.fgDtsValid;
    prDecPrm->u4RefPts = prDecPrm->rPesInfo.u4PTS;
    prDecPrm->u8RefPts = prDecPrm->rPesInfo.u8PTS;
    prDecPrm->u8RecOffset = prDecPrm->rPesInfo.u8Offset;

    prDecPrm->u8RecOffsetI = prDecPrm->rPesInfo.u8OffsetI;
    prDecPrm->rRecExtra = prDecPrm->rPesInfo.rExtra;
  
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
    prDecPrm->u1VopQuant = (UINT8)((u4Temp >> 27) & 0x1f);
    u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 5);
    if(prDecPrm->u1VopCodingType == VOP_CODING_TYPE_I)
    {
    	prDecPrm->u1VopRoundingType = 1;
    	prDecPrm->rDx3Params.u1FrameMode = (UINT8)((u4Temp >> 27) & 0x1f);

    	if(prVdecEsInfoKeep->fgVPush) //LG only
    	{
        	if (!(prDecPrm->rDx3Params.u1FrameMode == 23 || prDecPrm->rDx3Params.u1FrameMode == 24 || prDecPrm->rDx3Params.u1FrameMode == 26))
        	{
                LOG(3, "Wrong frame mode for I frame\n");
                *pi4RetValue = E_VDEC_FAIL;
                return; 	    	
        	}
        }
        
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 5);
    	prDecPrm->rDx3Params.u1AltIAcChromDct = (UINT8)((u4Temp >> 31) & 0x1);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	if(prDecPrm->rDx3Params.u1AltIAcChromDct)
    	{
    	  	prDecPrm->rDx3Params.u1AltIAcChromDctIdx = (UINT8)((u4Temp >> 31) & 0x1);
    	  	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	}
    	else
    	{
    	 	prDecPrm->rDx3Params.u1AltIAcChromDctIdx = 0;
    	}
    	prDecPrm->rDx3Params.u1AltIAcLumDct = (UINT8)((u4Temp >> 31) & 0x1);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	if(prDecPrm->rDx3Params.u1AltIAcLumDct)
    	{
    		 prDecPrm->rDx3Params.u1AltIAcLumDctIdx = (UINT8)((u4Temp >> 31) & 0x1);
    		 u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	}
    	else
    	{
    	 	 prDecPrm->rDx3Params.u1AltIAcLumDctIdx = 0;
    	}
    	prDecPrm->rDx3Params.u1AltIDcDct = (UINT8)((u4Temp >> 31) & 0x1);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	// 050131: has_skip and alternative_P_AC_DCT_index should be 0 in I-VOP,
    	//         or the I will be wrongly decoded.
    	prDecPrm->rDx3Params.u1HasSkip = 0;
    	prDecPrm->rDx3Params.u1AltPAcDct = 0;
    	prDecPrm->rDx3Params.u1AltPAcDctIdx = 0;
    	prDecPrm->rDx3Params.u1AltPDcDct = 0;
    	prDecPrm->rDx3Params.u1AltMv = 0;
    }
    else
    {
    	if(prDecPrm->rDx3Params.fgSwitchRounding)
    	{
    		prDecPrm->u1VopRoundingType = (UINT8)(1 - prDecPrm->u1VopRoundingType);
    	}
    	else
    	{
    	   	prDecPrm->u1VopRoundingType = 0;
    	}

    	prDecPrm->rDx3Params.u1HasSkip = (UINT8)((u4Temp >> 31) & 0x1);
    	u4Temp =  _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	prDecPrm->rDx3Params.u1AltPAcDct = (UINT8)((u4Temp >> 31) & 0x1);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	if(prDecPrm->rDx3Params.u1AltPAcDct)
    	{
    		prDecPrm->rDx3Params.u1AltPAcDctIdx = (UINT8)((u4Temp >> 31) & 0x1);
    		u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 1);
    	}
    	else
    	{
    		prDecPrm->rDx3Params.u1AltPAcDctIdx = 0;
    	}
    	prDecPrm->rDx3Params.u1AltPDcDct = (UINT8)((u4Temp >> 31) & 0x1);
    	prDecPrm->rDx3Params.u1AltMv = (UINT8)((u4Temp >> 30) & 0x1);
    	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 2);
    }

    LOG(6, "ucFRefFbId = 0x%08x\n", ucFRefFbId);
    LOG(6, "ucBRefFbId = 0x%08x\n", ucBRefFbId);
    
     _VDEC_GetVldRPtr(&prDecPrm->u4DecReadAddr, &prDecPrm->u4DecReadBits, prDecPrm->rPesInfo.u4FifoStart,prDecPrm->rPesInfo.u4FifoEnd);
    if(prDecPrm->u4DecReadAddr< prDecPrm->rPesInfo.u4FifoStart)
    {
    	  prDecPrm->u4DecReadAddr = prDecPrm->u4DecReadAddr+
    	                    (prDecPrm->rPesInfo.u4FifoEnd-prDecPrm->rPesInfo.u4FifoStart)-
    	                    prDecPrm->rPesInfo.u4FifoStart;
    }
   // Printf("unlock 311\n");
    if (prDecPrm->u4LockCount > 0)
    {
        _VDEC_UnlockVld(prDecPrm->rPesInfo.ucEsId, prDecPrm->rPesInfo.ucMpvId);
        prDecPrm->u4LockCount--;
    }

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeFbS);
#endif
    prDecPrm->u1FbId = FBM_GetEmptyRefFrameBuffer(prDecPrm->u1FbgId, MPEG4_WAIT_DISP_TIME);
#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeFbE);
        HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
        prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
#endif

	_CreateWorkingBuffer(prDecPrm);

#ifdef VDEC_TIME_PROFILE
        HAL_GetTime(&rTimeSwE);
        HAL_GetDeltaTime(&rTimeSwDt, &rTimeSwS, &rTimeSwE);
        u4SWTimeMicro += rTimeSwDt.u4Micros;
        if(prFbmSeqHdr->fgProgressiveSeq)
        {
            prDecPrm->u4CurSwMicro = u4SWTimeMicro - prDecPrm->u4CurWaitFbMicro;
            u4SWTimeMicro = 0;
            prDecPrm->u4CurWaitFbMicro = 0;
        }
    
#endif
    /*prFbmPicHdr->u4PTS = prDecPrm->rPesInfo.u4PTS;
    if (prDecPrm->rPesInfo.fgEos)
    {
        FBM_SetFrameBufferPicFlag(prDecPrm->u1FbgId, prDecPrm->u1FbId, FBM_MM_EOS_FLAG);   
    }*/

    *pi4RetValue = E_VDEC_OK;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

void _VDEC_PrsVideoType(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm, UCHAR ucEsId, INT32* pi4RetValue)
{
	UINT32 u4Temp;

   	VDEC_ES_INFO_T* prVdecEsInfo;
	VDEC_M4V_VOL_INFO_T* prVolPrm;
	*pi4RetValue = E_VDEC_OK;

	u4Temp = _VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 0);

	VLD_WRITE32(RW_VLD_MP4_FLG, 1);
	
	if ((u4Temp >> 8) == M4V_START_CODE)
	{
		prDecPrm->pfnPrsHdr = _PrsM4vHdr;
		prDecPrm->pfnDecInit = _VMpeg4DEC_DecProc;

		prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        if(prVdecEsInfo->fgSetDivX4)
        {
            prDecPrm->u1VideoType = VIDEO_TYPE_DIVX_MPEG4;
	        prVolPrm  = &prDecPrm->rM4vParams.rVol;
            prVolPrm->u4DivxCodecVersion = 400;
            LOG(6, "MPEG4 DivX4 Signal Received!\n");
        }
		else
        {
            prDecPrm->u1VideoType = VIDEO_TYPE_ISO_MPEG4;
        }		
	}
	else if ((u4Temp  & H263_VIDEO_START_MASK) == H263_VIDEO_START_CODE)
	{
		prDecPrm->pfnPrsHdr = _PrsH263Hdr;
		prDecPrm->pfnDecInit = _VMpeg4DEC_DecProc;
		prDecPrm->u1VideoType = VIDEO_TYPE_H263;
	}
#ifdef CC_IS_MPEG4_POST_MT5363
	else if ((u4Temp  & H263_VIDEO_START_MASK) == FLV_VIDEO_START_CODE)
	{
		prDecPrm->pfnPrsHdr = _PrsFlvHdr;
		prDecPrm->pfnDecInit = _VMpeg4DEC_DecProc;
		prDecPrm->u1VideoType = VIDEO_TYPE_FLV;
	}
#endif
	// Because the biCompression is not fixed for DivX311
	// we can't use any start code to specify the video type for DivX311.
	else //if (u4Temp == DX311_VIDEO_START_CODE)
	{
		LOG(7,"[MPEG4]: pfnPrsHdr is setted as DIVX3!\n");
		prDecPrm->pfnPrsHdr = _PrsDx311Hdr;
		prDecPrm->pfnDecInit = _VDEC_DecDx311Proc;
		prDecPrm->u1VideoType = VIDEO_TYPE_DX311;
		prDecPrm->rM4vParams.rVol.u4DivxCodecVersion = 311;
	}
	/*else
	{
		prDecPrm->u1VideoType = VIDEO_TYPE_UNKNOWN;
		*pi4RetValue = E_VDEC_FAIL;
	}*/
	
}

#if CC_MPEG4_UNUSED_FUNCTION
UINT32 _VDEC_Mpeg4GetStartCode(UINT32* pu4BitCount)
{
	UINT32 u4Temp;

	u4Temp = _VDEC_Mpeg4GetBitStream(pu4BitCount, 0);
	
	while(((u4Temp >> 8) != M4V_START_CODE))
	{
		u4Temp = _VDEC_Mpeg4GetBitStream(pu4BitCount, 8);
	}

	return (UINT8)(u4Temp & 0xff);

}
#endif

// Miscs
static UINT32 _GetDivXVersion(UINT8 *u1Buf)
{
	UINT32	u4RetVal;

  	u4RetVal = (UINT32)((u1Buf[0] & 0xf) * 100) +
             		   ((u1Buf[1] & 0xf) * 10) +
             		   (u1Buf[2] & 0xf);
  	return u4RetVal;
}

static UINT32 _LsbMask(UINT32 u4ShiftBits)
{
	UINT32	i;
	UINT32	u4Mask = 0;

	for(i = 0; i < u4ShiftBits; i++)
	{
		u4Mask |= (0x1 << i);
	}
	return u4Mask;
}

static void _Align2Byte(UINT32* pu4BitCount)
{
	if ((*pu4BitCount % 8))
	{
		_VDEC_Mpeg4GetBitStream(pu4BitCount, 8 - (*pu4BitCount % 8));
	}
}

static BOOL _SrcFmtTbl(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm)
{

	UINT32 u4WidthTbl[5] = {128, 176, 352, 704, 1408};
  	UINT32 u4HeightTbl[5] = {96, 144, 288, 576, 1152};
	VDEC_M4V_PIC_INFO_T 	*prVopPrm;

       prVopPrm = &prDecPrm->rM4vParams.rVop;
	if((prVopPrm->u1SourceFormat >= 1) && (prVopPrm->u1SourceFormat <= 5))
	{
		prDecPrm->u2Width = (UINT16)u4WidthTbl[prVopPrm->u1SourceFormat - 1];
		prDecPrm->u2Height = (UINT16)u4HeightTbl[prVopPrm->u1SourceFormat - 1];
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

#ifdef CC_IS_MPEG4_POST_MT5363
static BOOL _FlvSrcFmtTbl(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm)
{

	UINT32 u4WidthTbl[5] = {352, 176, 128, 320,160};
  	UINT32 u4HeightTbl[5] = {288, 144, 96, 240,120};
	VDEC_M4V_PIC_INFO_T 	*prVopPrm;

    prVopPrm = &prDecPrm->rM4vParams.rVop;

    if (prVopPrm->u1SourceFormat == 0)
    {
        prDecPrm->u2Width = (UINT16)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8) & 0xFF);
        prDecPrm->u2Height = (UINT16)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 8) & 0xFF);
    }
    else if (prVopPrm->u1SourceFormat == 1)
    {
        prDecPrm->u2Width = (UINT16)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 16) & 0xFFFF);
        prDecPrm->u2Height = (UINT16)(_VDEC_Mpeg4GetBitStream(&prDecPrm->u4BitCount, 16) & 0xFFFF);
    }
    else if((prVopPrm->u1SourceFormat >= 2) && (prVopPrm->u1SourceFormat <= 5))
	{
		prDecPrm->u2Width = (UINT16)u4WidthTbl[prVopPrm->u1SourceFormat - 2];
		prDecPrm->u2Height = (UINT16)u4HeightTbl[prVopPrm->u1SourceFormat - 2];
	}
    else if(prVopPrm->u1SourceFormat == 6)
    {
        prDecPrm->u2Width = (UINT16)u4WidthTbl[prVopPrm->u1SourceFormat - 2];
		prDecPrm->u2Height = (UINT16)u4HeightTbl[prVopPrm->u1SourceFormat - 2];
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
#endif

void _CreateWorkingBuffer(VDEC_MPEG4_DEC_PARAM_INFO_T* prDecPrm)
{
    UINT32 u4WorkingSize;
    UINT32 u4WorkingBuffer;
	
#ifdef USE_FBM_BUF
		FBM_GetWorkingBuffer(prDecPrm->u1FbgId, &u4WorkingBuffer, &u4WorkingSize);	  
	
		prDecPrm->u4DCACBufAddr = u4WorkingBuffer;
		prDecPrm->u4MVECBufAddr = u4WorkingBuffer + MP4_DCAC_SZ;
		prDecPrm->u4BMB1BufAddr = prDecPrm->u4MVECBufAddr + MP4_MVEC_SZ;
		prDecPrm->u4BMB2BufAddr = prDecPrm->u4MVECBufAddr + MP4_MVEC_SZ + MP4_BMB1_SZ;
		prDecPrm->u4BCODEBufAddr = prDecPrm->u4MVECBufAddr + MP4_MVEC_SZ + MP4_BMB1_SZ + MP4_BMB2_SZ; 
        
        #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
        FBM_WorkBufPartMap(prDecPrm->u1FbgId, prDecPrm->u4BCODEBufAddr, MP4_BCODE_SZ);
        #endif
#else 
		if (prDecPrm->u4DCACBufAddr == 0)
		{
			prDecPrm->u4DCACBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(MP4_DCAC_SZ, 16);
		}
		
		if (prDecPrm->u4MVECBufAddr == 0)
		{
			prDecPrm->u4MVECBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(MP4_MVEC_SZ, 16);
		}
		
		if (prDecPrm->u4BMB1BufAddr == 0)
		{
			prDecPrm->u4BMB1BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(MP4_BMB1_SZ, 16);
		}

		#ifndef CC_MT5398_NEW_MODE
		if (prDecPrm->u4BMB2BufAddr == 0)
		{
			prDecPrm->u4BMB2BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(MP4_BMB2_SZ, 16);
		}
		#endif
		
		if (prDecPrm->u4BCODEBufAddr == 0)
		{
			prDecPrm->u4BCODEBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(MP4_BCODE_SZ, 16);
		}
#endif

}

#ifdef CC_SUPPORT_VDEC_PREPARSE
typedef struct ES_DATA_S
{
    UCHAR *prStart;
    UCHAR *prReadPtr;
    UCHAR *prEnd;
    int    Left;    /* i_count number of available bits */
} ES_DATA_T;

void _PrePrsM4vHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    UINT32 u4Temp;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;
    *pi4RetValue = E_VDEC_OK;
    LOG(2, "_PrePrsM4vHdr\n");
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
#else
    u4Addr = prDecPrm->rPesInfo.u4VldReadPtr;
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);
    u4Temp = (pucBuf[0] << 24) | (pucBuf[1] << 16) |(pucBuf[2]<<8) | (pucBuf[3]);
    //LOG(5, "_PrsM4vHdr Start code = %08x\n", u4Temp);
    if ((u4Temp >> 8) == M4V_START_CODE)
    {
        *pi4RetValue = E_VDEC_PARSE_NOT_READY;
        if ((u4Temp <= M4V_VOL_START_CODE_MAX) &&
            u4Temp >= M4V_VOL_START_CODE_MIN)
        {
            LOG(2, "VOL start code!\n");
            _PrePrsVOLHdr(prDecPrm, pi4RetValue,ucEsId);
        }
        else
		{
		   LOG(0,"[MPEG4]: Pre-Parse not vol header\n");/*do not need to parse user data and vop data*/
		   *pi4RetValue = E_VDEC_PARSE_NOT_READY;
		}
    }
	else
	{
		LOG(0,"[MPEG4]: Invalid Start Code %d\n",u4Temp);
		*pi4RetValue = E_VDEC_INVALID_START_CODE;
	}
}

void _EsDataInit( ES_DATA_T *EsData, const void *pvData, size_t Data )
{
    EsData->prStart = (void *)pvData;
    EsData->prReadPtr = EsData->prStart;
    EsData->prEnd   = EsData->prStart + Data;
    EsData->Left  = 8;
}

UINT32 _EsDataRead( ES_DATA_T *EsData, UINT8 u1Count )
{
     UINT32 u4Mask[33] =
     {  0x00,
        0x01,      0x03,      0x07,      0x0f,
        0x1f,      0x3f,      0x7f,      0xff,
        0x1ff,     0x3ff,     0x7ff,     0xfff,
        0x1fff,    0x3fff,    0x7fff,    0xffff,
        0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
        0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
        0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
        0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
    int  u1Shr;
    UINT32 u4Result = 0;
    Printf("_EsDataRead!\n");
    Printf("EsData->prReadPtr:%d,EsData->prEnd:%d!\n", EsData->prReadPtr,EsData->prEnd);

    while( u1Count > 0 )
    {
        if( EsData->prReadPtr >= EsData->prEnd )
        {
            break;
        }

        if( ( u1Shr = EsData->Left - u1Count ) >= 0 )
        {
            /* more in the buffer than requested */
            Printf("more in the buffer than requested !\n");
            u4Result |= ( *(EsData->prReadPtr) >> u1Shr )& u4Mask[u1Count];
            Printf("u4Result:%d!\n", u4Result);
            EsData->Left -= u1Count;
            Printf("EsData->Left:%d!\n", EsData->Left);
            if( EsData->Left == 0 )
            {
                EsData->prReadPtr++;
                EsData->Left = 8;
                Printf("prReadPtr:%d,Left:%d!\n",EsData->prReadPtr, EsData->Left);
            }
            return( u4Result );
        }
        else
        {
            /* less in the buffer than requested */
            Printf("less in the buffer than requested !\n");
           u4Result |= (*(EsData->prReadPtr) & u4Mask[EsData->Left]) << -u1Shr;
           Printf("u4Result:%d!\n", u4Result);
           u1Count  -= EsData->Left;
           EsData->prReadPtr++;
           EsData->Left = 8;
           Printf("prReadPtr:%d!\n",EsData->prReadPtr);
        }
    }

    return( u4Result );
}

UINT32 _EsDataRead1( ES_DATA_T *EsData )
{
    Printf("_EsDataRead1!\n");
    if( EsData->prReadPtr < EsData->prEnd )
    {
        unsigned int i_result;

        EsData->Left--;
        i_result = ( *(EsData->prReadPtr) >> EsData->Left )&0x01;
        Printf("i_result:%d,EsData->LeftL%d\n",i_result,EsData->Left);
        if( EsData->Left == 0 )
        {
            EsData->prReadPtr++;
            EsData->Left = 8;
        }
        return i_result;
    }

    return 0;
}

void _EsDataSkip( ES_DATA_T *EsData, size_t Count )
{
    LOG(2, "_EsDataSkip!\n");
    EsData->Left -= Count;

    if( EsData->Left <= 0 )
    {
        int i_bytes = ( -EsData->Left + 8 ) / 8;

        EsData->prReadPtr += i_bytes;
        EsData->Left += 8 * i_bytes;
    }
}

int _GetTimeIncrementBits( unsigned int v )
{
    int n = 0;
    int vlc_log2_table[16] =
    {
        0,0,1,1,2,2,2,2, 3,3,3,3,3,3,3,3
    };

    if( v&0xffff0000 )
    {
        v >>= 16;
        n += 16;
    }
    if( v&0xff00 )
    {
        v >>= 8;
        n += 8;
    }
    if( v&0xf0 )
    {
        v >>= 4;
        n += 4;
    }
    n += vlc_log2_table[v];

    return n;
}

void _PrePrsVOLHdr(VDEC_MPEG4_DEC_PARAM_INFO_T * prDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_M4V_VOL_INFO_T  *prVolInfo;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    UINT8 u1ParWidth = 1;
    UINT8 u1ParHeight = 1;
    UINT32 u4Vol;
	UINT8 u1ObjectLayerShape;
	ES_DATA_T EsData;
	VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
 
	*pi4RetValue = E_VDEC_OK;
	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	prVolInfo = &prDecPrm->rM4vParams.rVol;
	LOG(2, "_PrePrsVOLHdr!\n");
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
    u4Addr += 4;
#else
    u4Addr = prDecPrm->rPesInfo.u4VldReadPtr + 4;/*skip start code and random_accessible_vol and video_object_type_indication*/
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);
    LOG(2, "Get seq addr!\n");

	//u4Vol = prDecPrm->rPesInfo.u4FifoEnd - prDecPrm->rPesInfo.u4VldReadPtr;/*availe data size*/
	u4Vol = VDEC_SEQINFO_SIZE;
	_EsDataInit( &EsData, &pucBuf[0], u4Vol - 4 );
    LOG(2, "_EsDataInit!\n");

	_EsDataSkip( &EsData, 1 );   /* random access */
	
	prVolInfo->u1VideoTypeIndication = _EsDataRead( &EsData, 8 );
	
	if( _EsDataRead1( &EsData ) )
	{
	    prVolInfo->u1VideoObjectLayerVerid= _EsDataRead( &EsData, 4 );
	    _EsDataSkip( &EsData, 3 );
	}

	prVolInfo->u1AspectRatioInfo = _EsDataRead( &EsData, 4 );
	if( prVolInfo->u1AspectRatioInfo == 0xf )
	{
	    u1ParWidth = _EsDataRead( &EsData, 8 );
	    u1ParHeight = _EsDataRead( &EsData, 8 );
	}
	if( _EsDataRead1( &EsData ) )
	{
	    UINT8 u1ChromaFormat;
	    UINT8 u1LowDelay;

	    /* vol control parameter */
	    u1ChromaFormat = _EsDataRead( &EsData, 2 );
	    u1LowDelay = _EsDataRead1( &EsData );

	    if( _EsDataRead1( &EsData ) )
	    {
	        _EsDataSkip( &EsData, 16 );
	        _EsDataSkip( &EsData, 16 );
	        _EsDataSkip( &EsData, 16 );
	        _EsDataSkip( &EsData, 3 );
	        _EsDataSkip( &EsData, 11 );
	        _EsDataSkip( &EsData, 1 );
	        _EsDataSkip( &EsData, 16 );
	    }
	}
	/* shape 0->RECT, 1->BIN, 2->BIN_ONLY, 3->GRAY */
	u1ObjectLayerShape = _EsDataRead( &EsData, 2 );
	if( u1ObjectLayerShape == 3 && prVolInfo->u1VideoObjectLayerVerid != 1 )
	{
	    _EsDataSkip( &EsData, 4 );
	}

	if( !_EsDataRead1( &EsData ) ) /* Marker */
	{
	    *pi4RetValue = E_VDEC_FAIL; 
    }
	prVolInfo->u4TimeIncrementResolution = _EsDataRead( &EsData, 16 ); /* Time increment resolution*/
	if( !prVolInfo->u4TimeIncrementResolution ) 
	{
		prVolInfo->u4TimeIncrementResolution = 1;
    }
	if( !_EsDataRead1( &EsData ) ) /* Marker */
	{
        *pi4RetValue = E_VDEC_FAIL;
	}

	if( _EsDataRead1( &EsData ) )
	{
	    prVolInfo->u1VopTimeIncrementResolutionBits = _GetTimeIncrementBits( prVolInfo->u4TimeIncrementResolution - 1 ) + 1;
        LOG(2, "_GetTimeIncrementBits!\n");

	    if( prVolInfo->u1VopTimeIncrementResolutionBits < 1 )
	    {
	    	prVolInfo->u1VopTimeIncrementResolutionBits = 1;
        }
	    _EsDataSkip( &EsData, prVolInfo->u1VopTimeIncrementResolutionBits );
	}
	if( u1ObjectLayerShape == 0 )
	{
	    _EsDataSkip( &EsData, 1 );
	    prVdecEsInfo->u2OrgHSize = _EsDataRead( &EsData, 13 );
	    _EsDataSkip( &EsData, 1 );
	    prVdecEsInfo->u2OrgVSize= _EsDataRead( &EsData, 13 );
	    _EsDataSkip( &EsData, 1 );
	}

	if(_EsDataRead1( &EsData ))
	{
        prVdecEsInfo->fgProgressive = FALSE;
	}
	else
	{
	    prVdecEsInfo->fgProgressive = TRUE;
    }

    switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
    {
        case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
            prVdecEsInfo->u4FrameRate = 2397;
            break;
        case VDEC_SRC_FRAME_RATE_24:
            prVdecEsInfo->u4FrameRate = 2400;
            break;
        case VDEC_SRC_FRAME_RATE_25:
            prVdecEsInfo->u4FrameRate = 2500;
            break;
        case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
            prVdecEsInfo->u4FrameRate = 2997;
            break;
        case VDEC_SRC_FRAME_RATE_30:
            prVdecEsInfo->u4FrameRate = 3000;
            break;
        case VDEC_SRC_FRAME_RATE_50:
            prVdecEsInfo->u4FrameRate = 5000;
            break;
        case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
            prVdecEsInfo->u4FrameRate = 5994;
            break;
        case VDEC_SRC_FRAME_RATE_60:
            prVdecEsInfo->u4FrameRate = 6000;
            break;
#if defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER)
        case VDEC_SRC_FRAME_RATE_12:
            prVdecEsInfo->u4FrameRate = 1200;
            break;
        case VDEC_SRC_FRAME_RATE_15:
            prVdecEsInfo->u4FrameRate = 1500;
            break;
        case VDEC_SRC_FRAME_RATE_20:
            prVdecEsInfo->u4FrameRate = 2000;
            break;
#endif
        default:
            prVdecEsInfo->u4FrameRate = 0;
            break;
    }

}

void _PrePrsH263Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    UINT32 u4Temp;
    VDEC_ES_INFO_T* prVdecEsInfo;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    UINT8 u1SourceFormat;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;
    UINT32 u4WidthTbl[5] = {128, 176, 352, 704, 1408};
    UINT32 u4HeightTbl[5] = {96, 144, 288, 576, 1152};
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    *pi4RetValue = E_VDEC_OK;
    LOG(2,"_PrePrsH263Hdr\n");
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
    UNUSED(prDecPrm);
#else
    u4Addr = prDecPrm->rPesInfo.u4VldReadPtr;
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);
    u4Temp = (pucBuf[0] << 24) | (pucBuf[1] << 16) |(pucBuf[2]<<8) | (pucBuf[3]);
    if ((u4Temp & H263_VIDEO_START_MASK) == H263_VIDEO_START_CODE)
    {
        u1SourceFormat = (pucBuf[4] & 0x1c) >> 3;
        if((u1SourceFormat >= 1) && (u1SourceFormat <= 5))
		{
			prVdecEsInfo->u2OrgHSize = (UINT16)u4WidthTbl[u1SourceFormat - 1];
			prVdecEsInfo->u2OrgVSize = (UINT16)u4HeightTbl[u1SourceFormat - 1];
			prVdecEsInfo->fgProgressive = TRUE;
		}
		else
		{
			*pi4RetValue = E_VDEC_NOT_SUPPORT;
		}

        switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
	    {
	        case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
	            prVdecEsInfo->u4FrameRate = 2397;
	            break;
	        case VDEC_SRC_FRAME_RATE_24:
	            prVdecEsInfo->u4FrameRate = 2400;
	            break;
	        case VDEC_SRC_FRAME_RATE_25:
	            prVdecEsInfo->u4FrameRate = 2500;
	            break;
	        case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
	            prVdecEsInfo->u4FrameRate = 2997;
	            break;
	        case VDEC_SRC_FRAME_RATE_30:
	            prVdecEsInfo->u4FrameRate = 3000;
	            break;
	        case VDEC_SRC_FRAME_RATE_50:
	            prVdecEsInfo->u4FrameRate = 5000;
	            break;
	        case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
	            prVdecEsInfo->u4FrameRate = 5994;
	            break;
	        case VDEC_SRC_FRAME_RATE_60:
	            prVdecEsInfo->u4FrameRate = 6000;
	            break;
#if defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER)
	        case VDEC_SRC_FRAME_RATE_12:
	           prVdecEsInfo->u4FrameRate = 1200;
	           break;
	        case VDEC_SRC_FRAME_RATE_15:
	           prVdecEsInfo->u4FrameRate = 1500;
	           break;
	         case VDEC_SRC_FRAME_RATE_20:
	            prVdecEsInfo->u4FrameRate = 2000;
	            break;
#endif
	        default:
	            prVdecEsInfo->u4FrameRate = 0;
	            break;
	    }
    }
    else
    {
    	LOG(0,"[H263]: Invalid Start Code %d\n",u4Temp);
		*pi4RetValue = E_VDEC_INVALID_START_CODE;
    }

}

void _PrePrsFlvHdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    UINT32 u4Temp;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    UINT8 u1SourceFormat;
	UINT32 u4WidthTbl[5] = {352, 176, 128, 320,160};
  	UINT32 u4HeightTbl[5] = {288, 144, 96, 240,120};
  	*pi4RetValue = E_VDEC_OK;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    LOG(2,"_PrePrsFlvHdr\n");
    
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
#else
    u4Addr = ((VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm)->rPesInfo.u4VldReadPtr;
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);
    u4Temp = (pucBuf[0] << 24)|(pucBuf[1] << 16)|(pucBuf[2]<<8)|(pucBuf[3]);
    if ((u4Temp & H263_VIDEO_START_MASK) == FLV_VIDEO_START_CODE)
      {
          u1SourceFormat = ((pucBuf[3] & 0x03) << 1)|(pucBuf[4] >> 7);
          prVdecEsInfo->fgProgressive = TRUE;
          if (u1SourceFormat == 0)
          {
              prVdecEsInfo->u2OrgHSize = ((pucBuf[4] & 0x7F) << 1)|(pucBuf[5] >> 7);
              prVdecEsInfo->u2OrgVSize = ((pucBuf[5] & 0x7F) << 1)|(pucBuf[6] >> 7);
          }
          else if (u1SourceFormat == 1)
          {
              prVdecEsInfo->u2OrgHSize = ((pucBuf[4] & 0x7F) << 9)|(pucBuf[5] << 1)|(pucBuf[6] >> 7);
              prVdecEsInfo->u2OrgVSize = ((pucBuf[6] & 0x7F) << 9)|(pucBuf[7] << 1)|(pucBuf[8] >> 7); 
          }
          else if((u1SourceFormat >= 2) && (u1SourceFormat <= 5))
          {
              prVdecEsInfo->u2OrgHSize = (UINT16)u4WidthTbl[u1SourceFormat - 2];
              prVdecEsInfo->u2OrgVSize = (UINT16)u4HeightTbl[u1SourceFormat - 2];
          }
          else if(u1SourceFormat == 6)
          {
              prVdecEsInfo->u2OrgHSize = (UINT16)u4WidthTbl[u1SourceFormat - 2];
              prVdecEsInfo->u2OrgVSize = (UINT16)u4HeightTbl[u1SourceFormat - 2];
          }
          else
          {
              *pi4RetValue = E_VDEC_NOT_SUPPORT;
          }

          switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
		  {
		    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
		        prVdecEsInfo->u4FrameRate = 2397;
		        break;
		    case VDEC_SRC_FRAME_RATE_24:
		        prVdecEsInfo->u4FrameRate = 2400;
		        break;
		    case VDEC_SRC_FRAME_RATE_25:
		        prVdecEsInfo->u4FrameRate = 2500;
		        break;
		    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
		        prVdecEsInfo->u4FrameRate = 2997;
		        break;
		    case VDEC_SRC_FRAME_RATE_30:
		        prVdecEsInfo->u4FrameRate = 3000;
		        break;
		    case VDEC_SRC_FRAME_RATE_50:
		        prVdecEsInfo->u4FrameRate = 5000;
		        break;
		    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
		        prVdecEsInfo->u4FrameRate = 5994;
		        break;
		    case VDEC_SRC_FRAME_RATE_60:
		        prVdecEsInfo->u4FrameRate = 6000;
		        break;
#if defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER)
		    case VDEC_SRC_FRAME_RATE_12:
		       prVdecEsInfo->u4FrameRate = 1200;
		       break;
		    case VDEC_SRC_FRAME_RATE_15:
		       prVdecEsInfo->u4FrameRate = 1500;
		       break;
		     case VDEC_SRC_FRAME_RATE_20:
		        prVdecEsInfo->u4FrameRate = 2000;
		        break;
#endif
		    default:
		        prVdecEsInfo->u4FrameRate = 0;
		        break;
		}
      }
      else
      {
          LOG(0,"[H263]: Invalid Start Code %d\n",u4Temp);
          *pi4RetValue = E_VDEC_INVALID_START_CODE;
      }

}

void _PrePrsDx311Hdr(VDEC_HDLR hDecPrm, INT32* pi4RetValue,UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_MPEG4_DEC_PARAM_INFO_T *prDecPrm = (VDEC_MPEG4_DEC_PARAM_INFO_T*)hDecPrm;
    *pi4RetValue = E_VDEC_OK;
    LOG(2,"_PrePrsDx311Hdr\n");
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if (prVdecEsInfo->fgMMSeqInfoValid == TRUE)
    {
        prVdecEsInfo->u2OrgHSize= prVdecEsInfo->rMMSeqInfo.u2_width;
        prVdecEsInfo->u2OrgVSize= prVdecEsInfo->rMMSeqInfo.u2_height;
        prVdecEsInfo->fgProgressive = TRUE;
    }
    else
    {
        if (prDecPrm->u2Width == 0 || prDecPrm->u2Height == 0)
        {
            *pi4RetValue =  E_VDEC_FAIL;
            return;
        }
    }
    switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
    {
        case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
            prVdecEsInfo->u4FrameRate = 2397;
            break;
        case VDEC_SRC_FRAME_RATE_24:
            prVdecEsInfo->u4FrameRate = 2400;
            break;
        case VDEC_SRC_FRAME_RATE_25:
            prVdecEsInfo->u4FrameRate = 2500;
            break;
        case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
            prVdecEsInfo->u4FrameRate = 2997;
            break;
        case VDEC_SRC_FRAME_RATE_30:
            prVdecEsInfo->u4FrameRate = 3000;
            break;
        case VDEC_SRC_FRAME_RATE_50:
            prVdecEsInfo->u4FrameRate = 5000;
            break;
        case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
            prVdecEsInfo->u4FrameRate = 5994;
            break;
        case VDEC_SRC_FRAME_RATE_60:
            prVdecEsInfo->u4FrameRate = 6000;
            break;
#if defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER)
        case VDEC_SRC_FRAME_RATE_12:
           prVdecEsInfo->u4FrameRate = 1200;
           break;
        case VDEC_SRC_FRAME_RATE_15:
           prVdecEsInfo->u4FrameRate = 1500;
           break;
         case VDEC_SRC_FRAME_RATE_20:
            prVdecEsInfo->u4FrameRate = 2000;
            break;
#endif
         default:
            prVdecEsInfo->u4FrameRate = 0;
            break;
    }

}

void _VDEC_PrePrsVideoType(VDEC_MPEG4_DEC_PARAM_INFO_T* prVdec, UCHAR ucEsId, INT32* i4RetValue)
{
    UINT32 u4Temp;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    *i4RetValue = E_VDEC_OK;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
#else
    u4Addr = prVdecEsInfoKeep->rPesInfo.u4VldReadPtr;
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);
    u4Temp = (pucBuf[0] << 24) | (pucBuf[1] << 16) |(pucBuf[2]<<8) | (pucBuf[3]);
    if ((u4Temp >> 8) == M4V_START_CODE)
    {
        prVdec->pfnPrePrsHdr = _PrePrsM4vHdr;
    }
    else if ((u4Temp  & H263_VIDEO_START_MASK) == H263_VIDEO_START_CODE)
    {
        prVdec->pfnPrePrsHdr = _PrePrsH263Hdr;
    }
#ifdef CC_IS_MPEG4_POST_MT5363
    else if ((u4Temp  & H263_VIDEO_START_MASK) == FLV_VIDEO_START_CODE)
    {
        prVdec->pfnPrePrsHdr = _PrePrsFlvHdr;
    }
#endif
    // Because the biCompression is not fixed for DivX311
    // we can't use any start code to specify the video type for DivX311.
    else //if (u4Temp == DX311_VIDEO_START_CODE)
    {
        LOG(7,"[MPEG4]: pfnPrsHdr is setted as DIVX3!\n");
        prVdec->pfnPrePrsHdr = _PrePrsDx311Hdr;
    }
}
#endif
