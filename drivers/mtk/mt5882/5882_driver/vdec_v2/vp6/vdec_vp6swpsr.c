/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright(c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp6.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp6psr.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "fbm_drvif.h"
#include "vdec_drvif.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "x_timer.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END
#include "vdec_vp6com.h"
#include "vdec_vp6util.h"
#include "vdec_vp6swpsr.h"

#ifdef VP6_DEBUG
static UINT32 *_pu4DbgParam1=NULL;
static UINT32 *_pu4DbgParam2=NULL;
static UINT32 u4Vp6DbgParma3=0;
BOOL _VDEC_Vp6SwSetDebugParam(UINT32 u4Param1,UINT32 u4Param2)
{
    if(_pu4DbgParam1)
    {
      *_pu4DbgParam1=u4Param1;
    }

    if(_pu4DbgParam2)
    {
      *_pu4DbgParam2=u4Param2;
    }

    return TRUE;
}

#endif

static BOOL _VDEC_Vp6PsrSwResolutionChange(VDEC_VP6_INFO_T * pVp6DecInfo,UINT32 u4Widht,UINT32 u4Height)
{
  VP6_DEC_PARAM_T *pDecParam=NULL;
  VDEC_ES_INFO_T *prVdecEsInfo = NULL;
  VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
  //FBM_PIC_HDR_T * prFbmPicHdr = NULL;
  FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;  
  UINT32 u4Pitch=0,u4YDataOffset=0,u4CDataOffset=0;
#ifdef VP6_USE_IMGRZ
  //UINT32 u4YAddr,u4CAddr;
  RZ_JPG_SCL_PARAM_SET_T *prImgrzParam;
#endif
  BOOL fgRet=TRUE,fgRasterMode=TRUE;
  UCHAR ucFbgType=FBM_FBG_TYPE_UNKNOWN,ucFbgId=FBM_FBG_ID_UNKNOWN;
  if(u4Widht==0 || u4Height==0)
  {
    return FALSE;
  }

  if(u4Widht*u4Height>VDEC_VP6_FBM_WIDTH*VDEC_VP6_FBM_HEIGHT)
  {
    return FALSE;
  }
  
  pDecParam=&pVp6DecInfo->rDecParam;
  
  if(pDecParam->u4FrameCounter==0)
  {
     prVdecEsInfo = _VDEC_GetEsInfo(pVp6DecInfo->uEsId);
     prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(pVp6DecInfo->uEsId);
     if(FBM_ChkFrameBufferFlag(pDecParam->ucFbgId, (UINT32)FBM_FLAG_RELEASE_FBG))        
     {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, pVp6DecInfo->uEsId);    
        FBM_WaitUnlockFrameBuffer(pDecParam->ucFbgId, 20);    // block until display unlock                    
        FBM_ReleaseGroup(pDecParam->ucFbgId);
        pDecParam->ucFbgId = FBM_FBG_ID_UNKNOWN;
     }
        
     if(pDecParam->ucFbgId!=FBM_FBG_ID_UNKNOWN)
     {
        if(u4Widht>pDecParam->u2Width || u4Height>pDecParam->u2Height)
        {
          FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, pVp6DecInfo->uEsId);    
          FBM_WaitUnlockFrameBuffer(pDecParam->ucFbgId, 20);    // block until display unlock                    
          FBM_ReleaseGroup(pDecParam->ucFbgId);
          pDecParam->ucFbgId=FBM_FBG_ID_UNKNOWN;
        }
     }
      
     pDecParam->u2Width=u4Widht;
     pDecParam->u2Height=u4Height;
        
     if(pDecParam->ucFbgId==FBM_FBG_ID_UNKNOWN)
     {       
       ucFbgType= FBM_SelectGroupType((u4Widht+STRIDE_EXTRA+0xf)&(~0xf),u4Height+STRIDE_EXTRA);
       if(ucFbgType==FBM_FBG_TYPE_UNKNOWN)
       {
         LOG(3,  "prDecPrm->u1FbgId == FBM_FGB_TYPE_UNKNOWN\n");
         return FALSE;
       }

        {
            FBM_CREATE_FBG_PAR_T rPar;
            x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
            rPar.u1VdecId = (UINT8)pVp6DecInfo->uEsId;
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

            ucFbgId =
                FBM_CreateGroupExt(ucFbgType, FBM_VDEC_VP6, u4Widht, u4Height, &rPar);
        }
       if(ucFbgId==FBM_FBG_ID_UNKNOWN)
       {
         LOG(3,  "prDecPrm->u1FbgId == FBM_FBG_ID_UNKNOWN\n");
         return FALSE;
       }
          
       FBM_SetPlayMode(ucFbgId,FBM_FBG_MM_MODE);
        if(prVdecEsInfoKeep->fgVPush)
        {
            if(prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
            }
            FBM_RegFbgCbFunc(ucFbgId,
                FBM_CB_FUNC_FB_READY_EX_IND, _VPUSH_DecodeDone, pVp6DecInfo->uEsId);
            if(prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                FBM_RegFbgCbFunc(ucFbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, pVp6DecInfo->uEsId);
            }
        }

       prVdecEsInfo->ucFbgId = ucFbgId;
       FBM_SetSyncStc(ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
       //FBM_GetFrameBufferSize(ucFbgId, &u4FbWidth, &u4FbHeight);
       FBM_FbgChgNotify(ucFbgId, pVp6DecInfo->uEsId);
       prFbmSeqHdr=FBM_GetFrameBufferSeqHdr(ucFbgId);
       pDecParam->ucFbgId=ucFbgId;
#ifdef VDEC_VP6_DEBLOCK
       FBM_SetFrameBufferFlag(pDecParam->u1FbgId, FBM_FLAG_DEBLOCKING);
       LOG(6, "DBK Enable\n");
#endif
     }
     

     if(ucFbgType!=pDecParam->ucFbgType)
     {
        UINT32 u4WorkingBuffer=NULL;
        UINT32 u4BufferSize=0;
        FBM_GetWorkingBuffer(pDecParam->ucFbgId, &u4WorkingBuffer, &u4BufferSize);
        if(VDEC_VP6_SWWORKBUF_SIZE>u4BufferSize)
        {
           u4BufferSize=0;
           u4WorkingBuffer=0;
        }
        else
        {
           VP6_CLRFLG(pDecParam->u4Flag,VP6_DEC_FLG_BSPWKBUF);
           pDecParam->u4WorkBuf=u4WorkingBuffer;
        }

        if(u4BufferSize==0)
        {
           u4WorkingBuffer=(UINT32)BSP_AllocAlignedDmaMemory(VDEC_VP6_SWWORKBUF_SIZE, 16);
           ASSERT(u4WorkingBuffer);
           if(!u4WorkingBuffer)
           {
              pDecParam->u4WorkBuf=0;
              VP6_CLRFLG(pDecParam->u4Flag,VP6_DEC_FLG_BSPWKBUF);
              return FALSE;
           }
           else
           {
              VP6_SETFLG(pDecParam->u4Flag,VP6_DEC_FLG_BSPWKBUF);
              pDecParam->u4WorkBuf=u4WorkingBuffer;
           }
        }

        _VDEC_Vp6DecUpdateWorkingBuffer(pVp6DecInfo->u4DecInstance,(UCHAR *)u4WorkingBuffer);
        _VDEC_Vp6DecGetPicInfo(pVp6DecInfo->u4DecInstance,NULL,NULL,&u4Pitch,&u4YDataOffset,&u4CDataOffset);
#ifdef VP6_USE_IMGRZ
        prImgrzParam=&pDecParam->rImgrzParam;
        x_memset(prImgrzParam,0,sizeof(RZ_JPG_SCL_PARAM_SET_T));
        prImgrzParam->u4IsRsIn = 1;  // block|raster based input
        prImgrzParam->u4YSrcBufLen = u4Pitch;

        prImgrzParam->u4YSrcVOffset = 0;
        prImgrzParam->u4YSrcHOffset = 0;
        prImgrzParam->u4YSrcW = pDecParam->u2Width;//prImgrzParam->u4YSrcHOffset;
        prImgrzParam->u4YSrcH = pDecParam->u2Height;

        prImgrzParam->u4CbSrcVOffset = 0;
        prImgrzParam->u4CbSrcHOffset = 0;
        prImgrzParam->u4CbSrcW = (pDecParam->u2Width>>1);//+prImgrzParam->u4CbSrcHOffset;
        prImgrzParam->u4CbSrcH = pDecParam->u2Height>>1;

        prImgrzParam->u4CrSrcVOffset = 0;
        prImgrzParam->u4CrSrcHOffset = 0;
        prImgrzParam->u4CrSrcW = (pDecParam->u2Width>>1);//+prImgrzParam->u4CrSrcHOffset;
        prImgrzParam->u4CrSrcH = pDecParam->u2Height>>1;
        prImgrzParam->u4ColorComp = 7;

        prImgrzParam->u4IsRsOut = 1; // block|raster based output
        prImgrzParam->u4OutSwap = 0;         // 6--block out

        prImgrzParam->u4IsVdo2Osd = 0;// output in VDO format ?
        prImgrzParam->u4YTgCM = 0;
        prImgrzParam->u4OutMode = E_RZ_VDO_OUTMD_420;
        prImgrzParam->u4YTgBufLen = pDecParam->u2Width;
        prImgrzParam->u4YTgW = (pDecParam->u2Width > 4)? pDecParam->u2Width : 4;
        prImgrzParam->u4YTgH = pDecParam->u2Height;
        prImgrzParam->u4CTgW = prImgrzParam->u4YTgW >> 1;
        prImgrzParam->u4CTgH = prImgrzParam->u4YTgH >> 1;
        prImgrzParam->u4IsFstBl = 1;
        prImgrzParam->u4IsLstBl = 1;
        prImgrzParam->u4JpgVfacY = 1;
        prImgrzParam->u4JpgVfacCb = 0;
        prImgrzParam->u4JpgVfacCr = 0;
        prImgrzParam->fgBLAssign=TRUE;
        
#if 0
        pDecParam->ucCurFbId=FBM_GetEmptyRefFrameBuffer(pDecParam->ucFbgId,VDEC_VP6_WAIT_DISP_TIME);
        FBM_SetFrameBufferStatus(pDecParam->ucFbgId,pDecParam->ucCurFbId, FBM_FB_STATUS_READY);
        FBM_GetFrameBufferAddr(pDecParam->ucFbgId,pDecParam->ucCurFbId,&u4YAddr,&u4CAddr);
        _VDEC_Vp6DecUpdateFrameBuf(pVp6DecInfo->u4DecInstance,VP6_UPDATE_SET_BUFFER,u4YAddr,u4CAddr);

        pDecParam->ucCurFbId=FBM_GetEmptyRefFrameBuffer(pDecParam->ucFbgId,VDEC_VP6_WAIT_DISP_TIME);
        FBM_SetFrameBufferStatus(pDecParam->ucFbgId,pDecParam->ucCurFbId, FBM_FB_STATUS_READY);
        FBM_GetFrameBufferAddr(pDecParam->ucFbgId,pDecParam->ucCurFbId,&u4YAddr,&u4CAddr);
        _VDEC_Vp6DecUpdateFrameBuf(pVp6DecInfo->u4DecInstance,VP6_UPDATE_SET_BUFFER,u4YAddr,u4CAddr);

        pDecParam->ucCurFbId=FBM_GetEmptyRefFrameBuffer(pDecParam->ucFbgId,VDEC_VP6_WAIT_DISP_TIME);
        FBM_SetFrameBufferStatus(pDecParam->ucFbgId,pDecParam->ucCurFbId, FBM_FB_STATUS_READY);
        FBM_GetFrameBufferAddr(pDecParam->ucFbgId,pDecParam->ucCurFbId,&u4YAddr,&u4CAddr);
        _VDEC_Vp6DecUpdateFrameBuf(pVp6DecInfo->u4DecInstance,VP6_UPDATE_SET_BUFFER,u4YAddr,u4CAddr);
        _VDEC_Vp6DecUpdateFrameBuf(pVp6DecInfo->u4DecInstance,VP6_UPDATE_SET_INIT,u4YAddr,u4CAddr);
#endif
        pDecParam->fgInitedDec=TRUE;
        u4YDataOffset=0;
        u4CDataOffset=0;
        pDecParam->u4FbmLineSize=pDecParam->u2Width;
        //fgRasterMode=FALSE;
#else
       pDecParam->u4FbmLineSize=u4Pitch;
       fgRasterMode=TRUE;
       pDecParam->fgInitedDec=FALSE;
#endif
        if (prFbmSeqHdr != NULL)
        {
           if (prVdecEsInfo != 0)
           {
                 UINT32 u4fps=0;
                 prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;        // 16:9
                 switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                 {
                 case VDEC_SRC_FRAME_RATE_23_976:
                     u4fps = 23976;
                     break;
                 case VDEC_SRC_FRAME_RATE_24:
                     u4fps = 24000;
                     break;
                 case VDEC_SRC_FRAME_RATE_25:
                     u4fps = 25000;
                     break;
                 case VDEC_SRC_FRAME_RATE_29_97:
                     u4fps = 29970;
                     break;
                 case VDEC_SRC_FRAME_RATE_30:
                     u4fps = 30000;
                     break;
                 case VDEC_SRC_FRAME_RATE_50:
                     u4fps = 50000;
                     break;
                 case VDEC_SRC_FRAME_RATE_59_94:
                     u4fps = 59940;
                     break;
                 case VDEC_SRC_FRAME_RATE_60:
                     u4fps = 60000;
                     break;
                 default:
                     u4fps = 0;
                     break;
                }
               
               if (u4fps != 0)
               {
                  pDecParam->u4DeltaPTS = (90000000/u4fps);
                  u4fps /= 1000;
               }
               else
               {
                  pDecParam->u4DeltaPTS = (90000000/VDEC_VP6DEF_FRAME_RATE);
               } 
               
               switch (u4fps)
               {
                  case 22:
                  case 23:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                    break;
                  case 24:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                    break;
                  case 25:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                    break;
                  case 30:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                    break;
                  case 29:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                    break;
                  case 50:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                    break;
                  case 60:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                    break;
                  case 59:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                    break;
                  default:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                    break;
               }
               
               prFbmSeqHdr->fgProgressiveSeq = TRUE;
               prFbmSeqHdr->u2HSize = pDecParam->u2Width;
               prFbmSeqHdr->u2VSize = pDecParam->u2Height;
               prFbmSeqHdr->u2OrgHSize = pDecParam->u2Width;
               prFbmSeqHdr->u2OrgVSize = pDecParam->u2Height;
               prFbmSeqHdr->u2LineSize = (UINT16)pDecParam->u4FbmLineSize;
               prFbmSeqHdr->u4YAddrOffset=u4YDataOffset;
               prFbmSeqHdr->u4CAddrOffset=u4CDataOffset;
               prFbmSeqHdr->fgRasterOrder=fgRasterMode;
            }
            else
            {
             // Did not parse the frame rate from ES so far
             //FBM_SetFrameBufferFlag(prDecPrm->u1FbgId, FBM_FLAG_SEQ_CHG);   
               prFbmSeqHdr->ucAspRatInf = 1;        // 16:9
               prFbmSeqHdr->ucFrmRatCod = 5;        // 30 fps
               prFbmSeqHdr->fgProgressiveSeq = 1;
               prFbmSeqHdr->u2HSize = pDecParam->u2Width;
               prFbmSeqHdr->u2VSize = pDecParam->u2Height;
               prFbmSeqHdr->u2OrgHSize = pDecParam->u2Width;
               prFbmSeqHdr->u2OrgVSize = pDecParam->u2Height;
               prFbmSeqHdr->u2LineSize = (UINT16)pDecParam->u4FbmLineSize;
               prFbmSeqHdr->u4YAddrOffset=u4YDataOffset;
               prFbmSeqHdr->u4CAddrOffset=u4CDataOffset;
               prFbmSeqHdr->fgRasterOrder=fgRasterMode;
           }
        }
     }
     
  }
  return fgRet;
}

static BOOL _VDEC_Vp6PsrSwInstanceCb(INT32 i4DecId,UINT32 InfoType,UINT32 u4Param1,UINT32 u4Param2,UINT32 u4Param3)
{
  VDEC_VP6_INFO_T *pVp6DecInfo;
  VP6_DEC_PARAM_T *pDecParam;
  BOOL fgRet=TRUE;
  
  if(!_VDEC_Vp6GetDecInfo(i4DecId,&pVp6DecInfo))
  {
     return FALSE;
  }
  pDecParam=&pVp6DecInfo->rDecParam;
  
  switch(InfoType)
  {
    case VP6_INSTANCE_CB_RESIZE:
	{
#ifdef VP6_USE_IMGRZ
      RZ_JPG_SCL_PARAM_SET_T *prImgrzParam=&pDecParam->rImgrzParam;
      VP6_IMGRZ_PARAM *rBufInfo=(VP6_IMGRZ_PARAM *)u4Param1;
      prImgrzParam->u4YSrcBase1 = rBufInfo->u4InY;
      prImgrzParam->u4YSrcBase2 = rBufInfo->u4InY;    // y2
      prImgrzParam->u4CbSrcBase1 = rBufInfo->u4InCb;  // cb1
      prImgrzParam->u4CbSrcBase2 = rBufInfo->u4InCb;  // cb2
      prImgrzParam->u4CrSrcBase1 = rBufInfo->u4InCr;  // cr1
      prImgrzParam->u4CrSrcBase2 = rBufInfo->u4InCr;  // cr2
      prImgrzParam->u4JpgTempAddr = rBufInfo->u4RZBuf;
      prImgrzParam->u4YTgBase=rBufInfo->u4OutY;
      prImgrzParam->u4CTgBase=rBufInfo->u4OutC;
      IMGRZ_Lock();
      IMGRZ_ReInit();
      IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
      IMGRZ_Scale((void *)prImgrzParam);
      IMGRZ_Flush();
      IMGRZ_Wait();
      IMGRZ_Unlock();
#endif
      break;
    }
    case VP6_INSTANCE_CB_RESET_FRAME:
    {
       fgRet=_VDEC_Vp6PsrSwResolutionChange(pVp6DecInfo,u4Param1,u4Param2);
       break;
    }
    case VP6_INSTANCE_CB_RPT:
    {
      pDecParam->u4ReadPtr=u4Param1;
      break;
    }
    case VP6_INSTANCE_CB_MALLOC:
    {
       UINT32 u4Addr;
       u4Addr=(UINT32)BSP_AllocAlignedDmaMemory(u4Param2,u4Param3);
       ASSERT(u4Addr);
       *((UINT32 *)u4Param1)=VIRTUAL(u4Addr);
       break;
    }
    case VP6_INSTANCE_CB_FREE:
    {
        UINT32 u4Addr;        
        u4Addr=PHYSICAL(u4Param1);
        BSP_FreeAlignedDmaMemory((void *)u4Addr);
        break;
    }
#ifdef VP6_DEBUG
    case VP6_INSTANCE_CB_DBGPARAM:
    {
        _pu4DbgParam1=(UINT32 *)u4Param1;
        _pu4DbgParam2=(UINT32 *)u4Param2;
        break;
    }
#endif
    case VP6_INSTANCE_CB_FRAME_TYPE:
    {
       pDecParam->fgKeyFrame=(BOOL)u4Param1;
       break;
    }
    case VP6_INSTANCE_CB_HEADER_INFO:
    {
      pDecParam->fgRefreshGd=(BOOL)u4Param1;
      //VP6_SETFLGV(pDecParam->u4Flag,VP6_DEC_FLG_NEWGD,u4Param1);
      break;
    }
    default:
      break;
  }
  return fgRet;
}

BOOL _VDEC_Vp6PsrSwInit(VDEC_VP6_INFO_T *prVp6DecInfo,INT32 i4DecId)
{
   if(_VDEC_VP6SwCreateInstance(&prVp6DecInfo->u4DecInstance,i4DecId,_VDEC_Vp6PsrSwInstanceCb))
   {
     return _VDEC_Vp6SwSetReturnParam(prVp6DecInfo->u4DecInstance,(void *)(&prVp6DecInfo->rDecParam.fgRefreshGd),(void *)(&prVp6DecInfo->rDecParam.u4ReadPtr),(void *)(&prVp6DecInfo->rDecParam.fgKeyFrame),NULL);
   }
   return FALSE;
}

BOOL _VDEC_Vp6PsrSwHeaderInfo(VDEC_VP6_INFO_T *pDecInfo)
{
   VP6_DEC_PARAM_T *pDecParam=&pDecInfo->rDecParam;
   VDEC_PES_INFO_T *rPesInfo=&pDecInfo->rPesInfo;
#ifndef VP6_USE_IMGRZ
   UINT32 u4YAddr=0,u4CAddr=0;
#endif
   UINT32 u4DataLen=0;
   UCHAR *puFrameData=NULL;
   if(!pDecInfo)
   {
     return FALSE;
   }

   puFrameData=(UCHAR *)rPesInfo->u4VldReadPtr;
   u4DataLen=rPesInfo->u4FifoEnd-rPesInfo->u4VldReadPtr;

   if(!_Vdec_VP6SwGetHeaderInfo(pDecInfo->u4DecInstance,puFrameData,u4DataLen))
   {
     return FALSE;
   }

   if (pDecParam->ucFbgId != FBM_FBG_ID_UNKNOWN)
   {
      if(FBM_ChkFrameBufferFlag(pDecParam->ucFbgId,(UINT32)FBM_FLAG_RESET))
      {
         FBM_ResetGroup(pDecParam->ucFbgId);
         FBM_ClrFrameBufferFlag(pDecParam->ucFbgId, (UINT32)FBM_FLAG_RESET);
         FBM_ClrFrameBufferFlag(pDecParam->ucFbgId, (UINT32)FBM_FLAG_NOT_READY);                       
      }
   }
   
   pDecParam->u4DispPts= pDecInfo->rPesInfo.u4PTS;
   pDecParam->u8DispPts= pDecInfo->rPesInfo.u8PTS;
   pDecParam->ucCurFbId = FBM_GetEmptyRefFrameBuffer(pDecParam->ucFbgId, VDEC_VP6_WAIT_DISP_TIME);
   
#ifndef VP6_USE_IMGRZ
   if(pDecParam->ucGoldenFbId!=FBM_FB_ID_UNKNOWN)
   {
     if(pDecParam->ucCurFbId==pDecParam->ucGoldenFbId)
     {
        FBM_SetFrameBufferStatus(pDecParam->ucFbgId,pDecParam->ucGoldenFbId, FBM_FB_STATUS_READY);
        pDecParam->ucCurFbId = FBM_GetEmptyRefFrameBuffer(pDecParam->ucFbgId, VDEC_VP6_WAIT_DISP_TIME);
     }
   }
   
   FBM_GetFrameBufferAddr(pDecParam->ucFbgId,pDecParam->ucCurFbId,&u4YAddr,&u4CAddr);
   if(!pDecParam->fgInitedDec)
   {
      _VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_SET_INIT,u4YAddr,u4CAddr);
      pDecParam->fgInitedDec=TRUE;
   }
   if(!_VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_CUR,u4YAddr,u4CAddr))
   {
     return FALSE;
   }
#else
   if(!_VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_CUR,0,0))
   {
     return FALSE;
   }
#endif
   FBM_SetFrameBufferStatus(pDecParam->ucFbgId,pDecParam->ucCurFbId, FBM_FB_STATUS_DECODE);

#ifdef VDEC_VP6_DEBLOCK
   pDecParam->ucDbkFbId=FBM_GetDbkEmptyRefFrameBuffer(pDecParam->ucFbgId, VDEC_VP6_WAIT_DISP_TIME);
#endif
   pDecInfo->rPesInfo.fgDtsValid ? VP6_SETFLG(pDecParam->u4Flag,VP6_DEC_FLG_VALIDPTS) : VP6_CLRFLG(pDecParam->u4Flag,VP6_DEC_FLG_VALIDPTS);
   pDecParam->u4FrameCounter++;
   if(u4Vp6DbgParma3 && pDecParam->u4FrameCounter==u4Vp6DbgParma3)
   {
      u4Vp6DbgParma3++;
   }
   return TRUE;
}

BOOL _VDEC_Vp6PsrSwGetPicInfo(VDEC_VP6_INFO_T *pDecInfo,UINT32 *pu4YAddr,UINT32 *pu4CAddr)
{
   return _VDEC_Vp6DecGetPicInfo(pDecInfo->u4DecInstance,pu4YAddr,pu4CAddr,NULL,NULL,NULL);
}

BOOL _VDEC_Vp6PsrSwDecode(VDEC_VP6_INFO_T *pDecInfo)
{
   UINT8 *puFrameData;
   VP6_DEC_PARAM_T *prDecParam=&pDecInfo->rDecParam;
   UINT32 u4DataLen;   
   
   puFrameData=(UINT8 *)pDecInfo->rPesInfo.u4VldReadPtr;
   u4DataLen=pDecInfo->rPesInfo.u4FifoEnd-pDecInfo->rPesInfo.u4VldReadPtr;
   if(_VDEC_Vp6DecDecodeFrame(pDecInfo->u4DecInstance,puFrameData,u4DataLen)>=0)
   {
      FBM_SetFrameBufferStatus(prDecParam->ucFbgId,prDecParam->ucCurFbId, FBM_FB_STATUS_READY);
      if(pDecInfo->rPesInfo.fgEos)
      {
        prDecParam->u4LastReadPtr=prDecParam->u4ReadPtr;
        prDecParam->u4RefPts=prDecParam->u4DispPts;
        prDecParam->u8RefPts=prDecParam->u8DispPts;
        prDecParam->fgLastKeyFrame=prDecParam->fgKeyFrame;
      }
      
      return TRUE;
    }
   else
   {
     ASSERT(0);
     return FALSE;
   }
}


BOOL _VDEC_Vp6PsrSwFin(VDEC_VP6_INFO_T *pDecInfo)
{
  VP6_DEC_PARAM_T *prDecParam=&pDecInfo->rDecParam;
  FBM_PIC_HDR_T *prFbmPicHdr;
  VDEC_ES_INFO_T *prVdecEsInfo = NULL;
#ifdef VP6_USE_IMGRZ
  UINT32 u4YAddr,u4CAddr;
#endif

  BOOL fgNotDisplay=FALSE;
  UCHAR ucFbgId,ucLastFbId;
  ucFbgId=prDecParam->ucFbgId;
  ucLastFbId=prDecParam->ucLastFbId;
  prVdecEsInfo=_VDEC_GetEsInfo((UCHAR)pDecInfo->uEsId);
  if(prDecParam->ucLastFbId!=FBM_FB_ID_UNKNOWN)
  {
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId,ucLastFbId);
    if (prFbmPicHdr)
    {
      prFbmPicHdr->u4PTS = prDecParam->u4RefPts;
      prFbmPicHdr->u8PTS = prDecParam->u8RefPts;
    }
    else
    {
      LOG(3, "_PrsVOPHdr prFbmPicHdr == NULL\n");
      return FALSE;
    }

    fgNotDisplay = VDEC_IsNotDisplay(pDecInfo->uEsId, prDecParam->u4RefPts, 0, 0);
    if(fgNotDisplay)
    {
       UCHAR ucFbStatus;
       ucFbStatus = FBM_GetFrameBufferStatus(ucFbgId,ucLastFbId);
       if((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) ||(ucFbStatus != FBM_FB_STATUS_LOCK))
       {
          FBM_SetFrameBufferStatus(ucFbgId,ucLastFbId, FBM_FB_STATUS_EMPTY);
       }
       else
       {
          LOG(6, "Frame buffer status = %x\n", ucFbStatus);
       }
    }
    else
    {
       LOG(6, "_PrsVp6Hdr prFbmPicHdr->u4PTS = %ld\n", prFbmPicHdr->u4PTS);
       UCHAR ucFbStatus;
       ucFbStatus = FBM_GetFrameBufferStatus(ucFbgId,ucLastFbId);
       if((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||(ucFbStatus == FBM_FB_STATUS_READY))
       {
          prFbmPicHdr->ucPicCdTp = prDecParam->fgLastKeyFrame ? (UCHAR)I_TYPE : (UCHAR)P_TYPE;
          prFbmPicHdr->ucPicStruct = (UCHAR)3;
          prFbmPicHdr->fgProgressiveFrm = TRUE; 
          prFbmPicHdr->fgRepFirstFld = FALSE;
          prFbmPicHdr->fgTopFldFirst = FALSE;
          
          FBM_SetFrameBufferStatus(ucFbgId,ucLastFbId, FBM_FB_STATUS_DISPLAYQ);
          prVdecEsInfo->u4DisplayQPicCnt++;
       }
       else
       {
          LOG(6, "Frame buffer status = %x\n", ucFbStatus);
       }
     }
     UNUSED(VDEC_CheckStopStatus(pDecInfo->uEsId, prDecParam->u4RefPts, 0));    
  }
  
  prDecParam->fgLastKeyFrame=prDecParam->fgKeyFrame;
  prDecParam->u4RefPts=prDecParam->u4DispPts;
  prDecParam->u8RefPts=prDecParam->u8DispPts;
  prDecParam->ucLastFbId=prDecParam->ucCurFbId;
  prDecParam->u4LastReadPtr=prDecParam->u4ReadPtr;

#ifdef VP6_USE_IMGRZ
  FBM_GetFrameBufferAddr(prDecParam->ucFbgId,prDecParam->ucLastFbId,&u4YAddr,&u4CAddr);
  _VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_LAST,u4YAddr,u4CAddr);
  if(prDecParam->fgRefreshGd || prDecParam->fgKeyFrame)
  {
     _VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_GOLDEN,0,0);
  }
#else
  _VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_LAST,0,0);
  if(prDecParam->fgRefreshGd || prDecParam->fgKeyFrame)
  {
     if(prDecParam->ucGoldenFbId!=FBM_FB_ID_UNKNOWN)
     {
        FBM_SetFrameBufferStatus(ucFbgId,prDecParam->ucGoldenFbId, FBM_FB_STATUS_EMPTY);
     }
     _VDEC_Vp6DecUpdateFrameBuf(pDecInfo->u4DecInstance,VP6_UPDATE_FRAME_GOLDEN,0,0);
     prDecParam->ucGoldenFbId=prDecParam->ucCurFbId;
  }
#endif
  return TRUE;
}


BOOL _VDEC_Vp6PsrSwRelease(VDEC_VP6_INFO_T *pDecInfo)
{
  _VDEC_Vp6DecReleaseInstance(pDecInfo->u4DecInstance);
  return TRUE;
}


