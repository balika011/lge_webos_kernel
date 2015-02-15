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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp8api.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8api.c
 *  This header file declares exported APIs of vp8.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "fbm_drvif.h"
#include "vdec_drvif.h"
#include "x_timer.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
#include "x_chip_id.h"

LINT_EXT_HEADER_END
#include "vdec_vp8webp_if.h"
#include "vdec_vp8util.h"
#include "vdec_vp8psr.h"
#include "vdec_vp8api.h"


#define MAX_ES_NUM VDEC_MAX_ES


#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
VDEC_VP8_INFO_T *pVp8DecInfo[MAX_ES_NUM] = {NULL};
#else
VDEC_VP8_INFO_T rVp8DecInfo[MAX_ES_NUM]; 
#endif 


VDEC_VP8_INFO_T *pVDecGetVp8DrvInfo(UCHAR ucEsId)
{
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    return pVp8DecInfo[ucEsId];
#else
 	return (VDEC_VP8_INFO_T *)&rVp8DecInfo[ucEsId];
#endif
}



VDEC_VP8_INFO_T *VDEC_Vp8GetLockedInstance(void)
{
    UINT8 uEsId = 0;
	VDEC_VP8_INFO_T *prDecInfo  = NULL;
    for(uEsId = 0; uEsId < MAX_ES_NUM; uEsId++)
    {
    	prDecInfo = pVDecGetVp8DrvInfo(uEsId);
	
        if((prDecInfo != NULL) && VDEC_FLGSET(prDecInfo->rDecParam.u4Flag, VP8_DEC_FLG_LOCKED))
        {
            break;
        }
    }
    
    ASSERT(uEsId != MAX_ES_NUM);
    return prDecInfo;
}

INT32 _VDEC_Vp8Init(UINT8 uEsId)
{
    VDEC_VP8_INFO_T *prDecInfo;
    VP8_DEC_PARAM_T *prDecParam;  
	INT32 i4Ret=0;

#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
		if(pVp8DecInfo[uEsId] == NULL)
		{ 
			pVp8DecInfo[uEsId] = (VDEC_VP8_INFO_T*)x_mem_alloc_virtual(sizeof(VDEC_VP8_INFO_T));	
			if(pVp8DecInfo[uEsId] == NULL)
			{
				LOG(0,"Allocate vp8 driver info struct Fail\n");
				ASSERT(0);
			}
		}
#endif
	prDecInfo  = pVDecGetVp8DrvInfo(uEsId);
	prDecParam = &prDecInfo->rDecParam; 
	x_memset(prDecInfo,0,sizeof(VDEC_VP8_INFO_T));

	_VDEC_LockVld(uEsId,0);
    if(!VDEC_FLGSET(prDecParam->u4Flag,VP8_DEC_FLG_INITED))
    {
       x_memset(&prDecInfo->rPesInfo,0,sizeof(VDEC_PES_INFO_T));
       x_memset(&prDecInfo->rDecParam,0,sizeof(VP8_DEC_PARAM_T));
       x_memset(prDecInfo->rDecParam.RefTime,(UINT8)-1,VDEC_VP8_FBNUM);
#ifdef CC_VP8_EMULATION
       prDecParam->u4LogFrameIndex=(UINT32)-1;
       prDecParam->u4DumpFrameIndex=(UINT32)-1;
       prDecParam->u4DbgFrame=(UINT32)-1;
	   VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOGERRINFO);
#endif
#ifdef VP8_TIME_PROFILE
	   x_memset(&prDecInfo->rTimeProfile,0,sizeof(VDEC_VP8_TIMEPROFILE));
#endif
       prDecParam->ucFbgId     = FBM_FBG_ID_UNKNOWN;
       prDecParam->ucFbgType   = FBM_FBG_TYPE_UNKNOWN;
       prDecParam->ucCurFbId   = FBM_FB_ID_UNKNOWN;
       prDecParam->ucGoldenFbId= FBM_FB_ID_UNKNOWN;
       prDecParam->ucLastFbId  = FBM_FB_ID_UNKNOWN;
       prDecParam->ucAltFbId   = FBM_FB_ID_UNKNOWN;
       prDecInfo->uEsId        = uEsId;
       prDecInfo->eDecType     = VP8_DEC_TYPE_WEBM;

       VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
	   VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
		
       if(_VDEC_Vp8PsrInit(prDecInfo,uEsId))
       {
          VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
		  i4Ret=E_VDEC_OK;
       }
       else
       {
          VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
		  i4Ret=E_VDEC_FAIL;
       }
    }
    else
    {
        i4Ret=E_VDEC_OK;
    }
	
	VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
	_VDEC_UnlockVld(uEsId,0);
    return (INT32)i4Ret;
}


INT32 _VDEC_Vp8HdrPrs(UCHAR ucVdecId, UCHAR ucEsId)
{   
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    BOOL fgRet = FALSE;
//	    VDEC_ES_INFO_T* prVdecEsInfo;
    ASSERT(ucEsId < MAX_ES_NUM);
    prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
	prDecInfo->rDecParam.i4DecRet=(INT32)E_VDEC_OK;

#ifdef CC_VP8_EMULATION
    if(prDecInfo->rPesInfo.ucPicType == 255)
    {
#ifdef VP8_EMUCOVERAGE_STATISTIC
     Printf("Frame Count=%d\n",prDecInfo->rDecParam.u4FrameCounter);
      _Vdec_VP8UpdateEmuItemInfo(&prDecInfo->rVp8FrmHdr,2);
#endif
       return E_VDEC_BS_END;
    }
#endif

    if(prDecInfo->rPesInfo.fgEos)
    {
#ifdef CC_VP8_EMULATION
       return E_VDEC_BS_END;//E_VDEC_NOT_CODED_FRAME;
#endif
    }
	
	if(prDecInfo->rDecParam.ucFbgId!=FBM_FBG_ID_UNKNOWN)
	{
	  if(_VDEC_Vp8PsrGetEmptyFrameBuffer(&prDecInfo->rDecParam)==FALSE)
	  {
		LOG(3,"VP8 _VDEC_Vp8GetEmptyFrameBuffer return false2\n");
		return E_VDEC_FAIL;
	  }
	}

	_VDEC_LockVld(prDecInfo->uEsId,0);
    VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);

    fgRet = _VDEC_Vp8PsrHeaderInfo(prDecInfo);
	if(fgRet==FALSE && prDecInfo->rPesInfo.fgEos==FALSE)
	{
	   LOG(5,"_VDEC_Vp8PsrHeaderInfo return Fail\n");
	   VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
	   _VDEC_UnlockVld(prDecInfo->uEsId,0);
	   return prDecInfo->rDecParam.i4DecRet;
	}

#if 0
    prVdecEsInfo = _VDEC_GetEsInfo(prDecInfo->uEsId);
    if (prVdecEsInfo && prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {
        LOG(0,"_VDEC_GetEsInfo return Fail\n");
		if(prDecInfo->rDecParam.ucCurFbId!=FBM_FB_ID_UNKNOWN)
	    {
            _VDEC_Vp8PsrUpdateFrame(prDecInfo,FALSE);
	    }
        return E_VDEC_PARSE_NOT_READY;
    }
#endif
	return E_VDEC_OK;
}


INT32 _VDEC_Vp8DecStart(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    BOOL fgRet = FALSE;
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    
    VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
#ifdef VP8_ENABLE_MID_PROTECT
		{
		   VDEC_DISP_PIC_INFO_T rDispInfo;
		   UINT32 u4Width,u4Height;
		   x_memset(&rDispInfo,0,sizeof(VDEC_DISP_PIC_INFO_T));
#ifdef VP8_64X32_BLK_MODE
		   u4Width= ((prDecInfo->rDecParam.u2Width+ 63) >> 6) << 6;
#else
		   u4Width= ((prDecInfo->rDecParam.u2Width+  + 15) >> 4) << 4;
#endif
		   u4Height = ((prDecInfo->rDecParam.u2Height+ 31) >> 5) << 5;
		   FBM_GetFrameBufferAddr(prDecInfo->rDecParam.ucFbgId,prDecInfo->rDecParam.ucCurFbId, \
			(UINT32 *)&rDispInfo.pvYAddr,(UINT32 *)&rDispInfo.pvCAddr);
		   rDispInfo.u4YSize=u4Height*u4Width;
		   if(!_VDEC_MidSetting(ES0, &rDispInfo))
		   {
			   LOG(0, "VdecMid Fail~\n");
		   }
	   }
#endif
    fgRet = _VDEC_Vp8PsrDecode(prDecInfo);
    prDecInfo->rDecParam.i4DecRet = fgRet ? E_VDEC_OK : E_VDEC_FAIL;

    return prDecInfo->rDecParam.i4DecRet;
}

INT32 _VDEC_Vp8DecFin(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    _VDEC_Vp8PsrFin(prDecInfo);
	if(prDecInfo->rDecParam.i4DecRet!=E_VDEC_OK)
	{
	  VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
      _VDEC_UnlockVld(prDecInfo->uEsId,0);
	}
    return prDecInfo->rDecParam.i4DecRet;
}

void _VDEC_Vp8SWRst(UCHAR ucVdecId, UCHAR ucEsId)
{
    return;
}

#ifdef VP8_TIME_PROFILE
void _VDEC_Vp8GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 *u4Param1, UINT32 *u4Param2, UINT32 *u4Param3)
{
    VDEC_VP8_INFO_T *prDecInfo = NULL;    
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);

	if(u4Type==0)
	{
        *((UINT32 *)u4Param1) = prDecInfo->rDecParam.ucFbgId;
        *((UINT32 *)u4Param2) = prDecInfo->rDecParam.ucCurFbId;
        *((UINT32 *)u4Param3) = prDecInfo->rDecParam.ucCurFbId;
	}
	else if(u4Type ==(UINT32)TIME_PROFILE)
    {
        *((UINT32 *)u4Param1) = prDecInfo->rDecParam.ucFbgId;
        *((UINT32 *)u4Param2) = prDecInfo->rTimeProfile.u4DecMs;       //hw dec
    }
    else if(u4Type == (UINT32)TIME_PROFILE2)
    {
        if(prDecInfo->rDecParam.fgKeyFrame)
        {
			*((UINT32 *)u4Param2) = 2;
        }
		else
		{
			*((UINT32 *)u4Param2) = 0;	 
		}
		*((UINT32 *)u4Param3) = 1;	 
    }
    else if(u4Type == (UINT32)TIME_PROFILE3)
    {
	   *((UINT32 *)u4Param1) = prDecInfo->rTimeProfile.u4SwMicro;
	   *((UINT32 *)u4Param2) = prDecInfo->rTimeProfile.u4CurDecCycle;
	   *((UINT32 *)u4Param3) = prDecInfo->rTimeProfile.u4CurDramCycle;
    }
}
#endif

#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL _VDEC_Vp8IsPic(UCHAR ucEsId, VDEC_PES_INFO_T *prPes)
{
   return TRUE;
}
#endif

void _VDEC_Vp8SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
#ifdef CC_VP8_EMULATION
    VDEC_VP8_INFO_T *prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    if(u4Type==0)
    {
      prDecInfo->rDecParam.u4LogFrameIndex=u4Param1;
      prDecInfo->rDecParam.u4DumpFrameIndex=u4Param2;
	  prDecInfo->rDecParam.puFileName=(UCHAR *)u4Param3;
      //prDecInfo->rDecParam.u4DbgFrame=u4Param3;
      Printf("LogIndex=%u,DumpIndex=%u,DbgIndex=%u\n",u4Param1,u4Param2,u4Param3);
    }
#endif
}

void _VDEC_Vp8Stop(UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_VP8_INFO_T* prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
	VP8_DEC_PARAM_T *prDecParam;
	
    prDecParam=&prDecInfo->rDecParam;
    if(prDecParam->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(prDecParam->ucFbgId, 20);    // block until display unlock            
        FBM_ReleaseGroup(prDecParam->ucFbgId);
        prDecParam->ucFbgId = FBM_FBG_ID_UNKNOWN;
    }

    _VDEC_LockFlushMutex(ucEsId);
    if(prEsInfo->fgFlushEsmQ)
    {
        _VDEC_FlushEs(ucEsId);
        prEsInfo->fgFlushEsmQ = FALSE;
        _VDEC_UnlockFlushSema(ucEsId);        
    }
	
    VDEC_CLRFLG(prDecParam->u4Flag,VP8_DEC_FLG_INITED);

	if(prDecParam->u4WorkBuf &&
		VDEC_FLGSET(prDecParam->u4Flag, VP8_DEC_FLG_MALLOCWKBUF))
	{
        BSP_FreeAlignedDmaMemory(prDecParam->u4WorkBuf);
		prDecParam->u4WorkBuf=0;
		VDEC_CLRFLG(prDecParam->u4Flag, VP8_DEC_FLG_MALLOCWKBUF);
	}
	
    //_VDEC_ClearEsInfo(ucEsId);
    _VDEC_UnlockFlushMutex(ucEsId);
    
}


void _VDEC_Vp8GetPesStruct(UCHAR ucEsId, void **prPesInfo)
{
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    ASSERT(prPesInfo != NULL);    
    ASSERT(ucEsId < MAX_ES_NUM);
	if(prPesInfo==NULL)
	{
	   ASSERT(0);
       return;
	}
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    *prPesInfo = (void*)(&prDecInfo->rPesInfo);
}


void _VDEC_Vp8DropFrame(UCHAR ucEsId)
{
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    UINT32 u4CurRptr;
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    u4CurRptr = prDecInfo->rPesInfo.u4VldReadPtr;
    if(!VDEC_SetRptr(ucEsId, u4CurRptr, u4CurRptr))                
    {   
        LOG(1, "Vp8 ES(%d) VDEC_SetRptr Fail\n", prDecInfo->rPesInfo.ucEsId);                
    }
	
}


void _VDEC_Vp8GetDispPic(UCHAR ucEsId, void* pvInfo)
{
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    UINT32 u4YAddr = 0,u4CAddr = 0;
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
	prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;
    
    _VDEC_Vp8PsrGetPicInfo(prDecInfo, &u4YAddr, &u4CAddr);

#if 0//def CC_VP8_EMULATION
    if(prDecInfo->rPesInfo.u4VldReadPtr == prDecInfo->rPesInfo.u4FifoStart + 0x2c)
    {
        x_memset(prDispInfo, 0, sizeof(VDEC_DISP_PIC_INFO_T));
    }
#endif
    
    prDispInfo->pvYAddr     = (void *)u4YAddr;
    prDispInfo->pvCAddr     = (void*)u4CAddr;
    prDispInfo->u4YSize     = prDecInfo->rDecParam.u2Width * prDecInfo->rDecParam.u2Height;
    prDispInfo->u4CSize     = prDispInfo->u4YSize >> 1;
    prDispInfo->u1PicStruct = VP8_FRAME;
    prDispInfo->u4W         = prDecInfo->rDecParam.u2Width;
    prDispInfo->u4H         = prDecInfo->rDecParam.u2Height;
    prDispInfo->u4H_Cmp     = prDecInfo->rDecParam.u2Height;
    prDispInfo->u4W_Cmp     = prDecInfo->rDecParam.u2Width;
    prDispInfo->u4CurrentPicNo=prDecInfo->rDecParam.u4FrameCounter-1;
    
#ifdef VP8_ENABLE_CRC
    prDispInfo->u4Ycrc[0]=prDecInfo->rDecParam.pu4CrcVal[0][0];
    prDispInfo->u4Ycrc[1]=prDecInfo->rDecParam.pu4CrcVal[0][1];
    prDispInfo->u4Ycrc[2]=prDecInfo->rDecParam.pu4CrcVal[0][2];
    prDispInfo->u4Ycrc[3]=prDecInfo->rDecParam.pu4CrcVal[0][3];
    
    prDispInfo->u4Ccrc[0]=prDecInfo->rDecParam.pu4CrcVal[1][0];
    prDispInfo->u4Ccrc[1]=prDecInfo->rDecParam.pu4CrcVal[1][1];
    prDispInfo->u4Ccrc[2]=prDecInfo->rDecParam.pu4CrcVal[1][2];
    prDispInfo->u4Ccrc[3]=prDecInfo->rDecParam.pu4CrcVal[1][3];
    prDispInfo->u1PicStruct=prDecInfo->rDecParam.u4DbkType;
    /*
       Printf("Frame=%d,DbkType=%d\n",prDecInfo->rDecParam.u4FrameCounter-1,prDecInfo->rDecParam.u4DbkType);
       for(UINT32 u4Index=0;u4Index<8;u4Index++)
       {
           Printf("%s[%d]=0x%x\n", u4Index>3 ? "C" : "Y",u4Index%4,prDecInfo->rDecParam.pu4CrcVal[u4Index/4][u4Index%4]);
       }
       */
#endif

}

void _VDEC_Vp8UpdDmxRptr(UCHAR ucEsId)
{
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    ASSERT(ucEsId < MAX_ES_NUM);
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
#ifndef CC_VP8_EMULATION
    if (!VDEC_SetRptr(prDecInfo->rPesInfo.ucEsId, prDecInfo->rPesInfo.u4VldReadPtr, prDecInfo->rPesInfo.u4VldReadPtr))                
    {                    
        LOG(0, "Vp8 ES(%d) VDEC_SetRptr Fail\n", prDecInfo->rPesInfo.ucEsId);                
    }
#endif

    if(VDEC_FLGSET(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED))
    {
		VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
		_VDEC_UnlockVld(prDecInfo->uEsId,0);
    }
}

void _VDEC_Vp8Flush(UCHAR ucEsId,BOOL fgIsDisp)
{
    ASSERT(ucEsId < MAX_ES_NUM);
    _VDEC_Vp8PsrFlush(pVDecGetVp8DrvInfo(ucEsId));
    return;
}

void _VDEC_VP8SetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    VDEC_VP8_INFO_T* prDecInfo;

    ASSERT(ucEsId < MAX_ES_NUM);
    prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    prDecInfo->rDecParam.u4WPtr = u4Wptr;
    //_VDEC_SetVP8Wptr(u4Wptr);
}

//skip ES if return FALSE
BOOL _VDEC_Vp8EsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    VDEC_ES_INFO_T *prEsInfo;
    VDEC_VP8_INFO_T* prDecInfo = NULL;

    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pucVldId != NULL);
    if(ucEsId>=MAX_ES_NUM)
    {
       return FALSE;
    }
	prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    prEsInfo = _VDEC_GetEsInfo(ucEsId);

    if (prEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    //LOG(9, "Es flw control\n");
    //LOG(9, "Skip mode = %ld\n", ucSkipMode);

    if (IS_DECFLAG_SET(prEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        if(prDecInfo->rPesInfo.ucPicType!=I_TYPE)
        {
           return TRUE;
        }

		_VDEC_Vp8Flush(ucEsId,0);
        FBM_InvalidateRefFrameBuffer(prDecInfo->rDecParam.ucFbgId);
        CLR_DECFLAG(prEsInfo, DEC_FLG_WAIT_NEXT_I);
    }  
    
    if(ucSkipMode == IPMode)
    {
       //
    }
    else if (ucSkipMode == IMode)
    {
        if((prDecInfo->rPesInfo.ucPicType != I_TYPE)) 
        {
            return TRUE;
        }
    }

    return FALSE;
}

VOID _VDEC_Vp8SetIsr(UCHAR ucEsId)
{
    VDEC_VP8_INFO_T *prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
    _VDEC_Vp8PsrIsrInit(prDecInfo);
}

BOOL _VDEC_Vp8IsSupport(void)
{
#ifdef CC_VP8_EMULATION
    return TRUE;
#else
    return IS_SupportVP8();
#endif
}

#if 0
void _VDEC_Vp8DumpReg(void)
{
    return _VDEC_Vp8PsrDumpReg(pVDecGetVp8DrvInfo(0));
}
#endif

BOOL _VDEC_Vp8WebPInit(UINT32 u4DecId,PFN_EWBP_EVENT_NOTIFY pfnCb,
	UINT32 u4BufStart,UINT32 u4BufSize)
{

	VDEC_VP8_INFO_T *prDecInfo;
	WEBP_DEC_INFO_T *prWebPInfo;
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
	if(pVp8DecInfo[u4DecId] == NULL)
	{ 
		pVp8DecInfo[u4DecId] = (VDEC_VP8_INFO_T*)x_mem_alloc_virtual(sizeof(VDEC_VP8_INFO_T));	
		if(pVp8DecInfo[u4DecId] == NULL)
		{
			LOG(0,"Allocate vp8 driver info struct Fail\n");
			ASSERT(0);
		}
	}
#endif
    prDecInfo  = pVDecGetVp8DrvInfo(u4DecId);
	prWebPInfo=&prDecInfo->rWebPInfo;
    x_memset(&prDecInfo->rDecParam,0,sizeof(VP8_DEC_PARAM_T));
    x_memset(&prDecInfo->rWebPInfo,0,sizeof(WEBP_DEC_INFO_T));
	prDecInfo->eDecType=VP8_DEC_TYPE_WEBP;
	prWebPInfo->pfnWebpCb=pfnCb;
	prWebPInfo->u4BufStartAddr=u4BufStart;
	prWebPInfo->u4BufSize=u4BufSize;
	prWebPInfo->fgWrapperOff=FALSE;
	VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
	return TRUE;
}

BOOL _VDEC_Vp8WebPDecStart(UINT32 u4DecId,WEBP_DEC_MODE eDecMode,
    WEBP_DECIN_PARAM_T *prDecInParam,WEBP_DECOUT_PARAM_T *prDecOutParam)
{
   VDEC_VP8_INFO_T *prDecInfo  = pVDecGetVp8DrvInfo(u4DecId);
   WEBP_DEC_INFO_T *prWebPInfo=&prDecInfo->rWebPInfo;
   BOOL fgDecRet=TRUE;
   prWebPInfo->prDecInParam=prDecInParam;
   prWebPInfo->prDecOutParam=prDecOutParam;
   prWebPInfo->u4FileOffset=prDecInParam->u4FileOffset;
   prWebPInfo->u4Tag=prDecInParam->u4Tag;
   prDecInfo->rDecParam.u4Flag=0;
   VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
   VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
   fgDecRet=_VDEC_Vp8PsrWebPDecode(prDecInfo,eDecMode);
   VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
   VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_INITED);
#ifdef WEBP_ENABLE_CRC
   prDecOutParam->u4Ycrc[0]=prDecInfo->rDecParam.pu4CrcVal[0][0];
   prDecOutParam->u4Ycrc[1]=prDecInfo->rDecParam.pu4CrcVal[0][1];
   prDecOutParam->u4Ycrc[2]=prDecInfo->rDecParam.pu4CrcVal[0][2];
   prDecOutParam->u4Ycrc[3]=prDecInfo->rDecParam.pu4CrcVal[0][3];
   prDecOutParam->u4Ccrc[0]=prDecInfo->rDecParam.pu4CrcVal[1][0];
   prDecOutParam->u4Ccrc[1]=prDecInfo->rDecParam.pu4CrcVal[1][1];
   prDecOutParam->u4Ccrc[2]=prDecInfo->rDecParam.pu4CrcVal[1][2];
   prDecOutParam->u4Ccrc[3]=prDecInfo->rDecParam.pu4CrcVal[1][3];
   prDecOutParam->u4PPType=prDecInfo->rDecParam.u4DbkType;
   prDecOutParam->u4SrcHeight=prDecInfo->rDecParam.u2Height;
   prDecOutParam->u4SrcWidht=prDecInfo->rDecParam.u2Width;
   prDecOutParam->u4RzCrc=prDecInfo->rDecParam.u4RzCrc;
#endif
   return fgDecRet;
}

BOOL _VDEC_Vp8WebPDecStop(UINT32 u4DecId)
{
	VDEC_VP8_INFO_T *prDecInfo	= pVDecGetVp8DrvInfo(u4DecId);
   VDEC_SETFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_FORCESTOP);
   //VDEC_CLRFLG(prDecInfo->rDecParam.u4Flag,VP8_DEC_FLG_LOCKED);
   return TRUE;
}

UINT32 _VDEC_Vp8WebPGetProgress(UINT32 u4DecId)
{
	VDEC_VP8_INFO_T *prDecInfo	= pVDecGetVp8DrvInfo(u4DecId);
	return _VDEC_Vp8PsrWebPProgress(prDecInfo);
}

#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 _VDEC_Vp8PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    UCHAR *pucBuf;
    UINT32 u4ReadPos;
    UINT32 u4ReadValue;
    INT32   i4RetValue = 0;
    VDEC_VP8_INFO_T* prDecInfo = NULL;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prDecInfo  = pVDecGetVp8DrvInfo(ucEsId);
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    pucBuf = (UCHAR*)(prVdecEsInfoKeep->au1SeqInfo);
#else
    pucBuf = (UCHAR*)VIRTUAL(prDecInfo->rPesInfo.u4VldReadPtr);
#endif

  if((pucBuf[0]&0x1)==0) // Key frame
  {
     u4ReadPos = 3;
     u4ReadValue=(pucBuf[u4ReadPos]<<16)+ (pucBuf[u4ReadPos+1]<<8)+pucBuf[u4ReadPos+2];
     if(u4ReadValue!=0x9d012a)
     {
         LOG(0,"_PrePrsVp8Hdr Error I frame header, Tag=0x%x\n",u4ReadValue);
         i4RetValue = E_VDEC_SKIPFRAME;
     }
     else
     {
         prVdecEsInfo->u2OrgHSize=(pucBuf[u4ReadPos+3]+(pucBuf[u4ReadPos+4]<<8))&0x3fff;
         prVdecEsInfo->u2OrgVSize=(pucBuf[u4ReadPos+5]+(pucBuf[u4ReadPos+6]<<8))&0x3fff;
         prVdecEsInfo->fgProgressive = TRUE;
     }
  }
  else
  {
     LOG(0,"_PrePrsVp8Hdr Error Not I frame header\n");
     i4RetValue = E_VDEC_SKIPFRAME;
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
  return i4RetValue;
}
#endif
