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
 * $RCSfile: playmgr_mpg.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/
#include "playmgr.h"
#include "feeder_if.h"
#include "playmgr_util.h"
#include "swdmx_drvif.h"
#include "playmgr_mpg.h"
#include "playmgr_debug.h"
#include "x_util.h"


static HANDLE_T _hPlaymgrFeederReqDataQ = 0;
#define PLAYMGR_FEEDER_DATA_Q_SIZE	  	10

VOID _PLAYMGR_MpgFeederCallBack(MM_NOTIFY_INFO_T *prNfyInfo, UINT32 u4Param);
BOOL _PLAYMGR_MpgReceiveFeederAck(VOID *pvData);
static VOID _PLAYMGR_MpgPsMediaInfoNotify(
                           HANDLE_T                    hMinfo,                 
                           MINFO_NFY_EVENT_TYPE_T      e_event,  
                           const VOID*                 pvData1,
                           const VOID*                 pvData2,
                           const VOID*                 pvTag);

#define READ_SIZE (235*1024)
BOOL _PLAYMGR_IsBase(VOID);
PLAYMGR_MEDIA_TYPE_T _PLAYMGR_IsWhat(VOID);
SWDMX_PROGRAM_INFO_T _rPlayerMpgInfo;

VOID _PLAYMGR_MpgFeederCallBack(MM_NOTIFY_INFO_T *prNfyInfo, UINT32 u4Param)
{
    // Send Msg to _hPlaymgrFeederReqDataQ
    INT32 i4Ret;
    FEEDER_REQ_DATA_T rFeederReqData; 
    SIZE_T zMsgSize = sizeof(FEEDER_REQ_DATA_T);

    x_memcpy(&rFeederReqData, (FEEDER_REQ_DATA_T*)u4Param, sizeof(FEEDER_REQ_DATA_T));
    i4Ret = x_msg_q_send(_hPlaymgrFeederReqDataQ, &rFeederReqData, zMsgSize, 255);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        //LOG(0, "SWDMX Queue, OSR_TOO_MANY\n");
        i4Ret = x_msg_q_send(_hPlaymgrFeederReqDataQ, &rFeederReqData, zMsgSize, 255);
    }
    
    VERIFY(i4Ret == OSR_OK);
    UNUSED(prNfyInfo);
}

BOOL _PLAYMGR_MpgReceiveFeederAck(VOID *pvData)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;
    
    zMsgSize = sizeof(FEEDER_REQ_DATA_T);
    
    i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
          &(_hPlaymgrFeederReqDataQ), 1, X_MSGQ_OPTION_WAIT);
    
    ASSERT(zMsgSize == sizeof(FEEDER_REQ_DATA_T));
    
    if (i4Ret != OSR_OK)
    {
        return FALSE;
    }
    
    return TRUE;
}


static VOID _PLAYMGR_MpgPsMediaInfoNotify(
                           HANDLE_T                    hMinfo,                 
                           MINFO_NFY_EVENT_TYPE_T      e_event,  
                           const VOID*                 pvData1,
                           const VOID*                 pvData2,
                           const VOID*                 pvTag) 
{
    SIZE_T zRecLength;
    INT32 i4Ret;
    MINFO_INFO_T rMinfoRec;
    PLAYMGR_INFO_T *prMediaInfo;    
    PLAYMGR_PS_INFO_T* prPsInfo;
    UINT32 u4Temp;

    if(pvTag == NULL)
    {
        LOG(1,"_PLAYMGR_MpgPsMediaInfoNotify pvTag NULL\n");
        return;
    }
    
    zRecLength = sizeof(MINFO_INFO_T);
    prMediaInfo = (PLAYMGR_INFO_T*)pvTag;
    prPsInfo = &prMediaInfo->FmtInfo.rPsInfo;

    LOG(5,"_PLAYMGR_MpgPsMediaInfoNotify : Event type = %d\r\n",(UINT32)e_event);
    
    switch(e_event) {
    case MINFO_NFY_EVENT_PARSING_DONE:
        
        // generic info
        i4Ret = x_minfo_get_info(hMinfo, MINFO_INFO_TYPE_GENERIC,
                    (MINFO_REC_IDX_T)1, &rMinfoRec, &zRecLength);
        if ((zRecLength != sizeof(MINFO_INFO_T)) || i4Ret)
        {
            LOG(1, "x_minfo_get_info MINFO_INFO_TYPE_GENERIC %d\n", i4Ret);
            break;
        }
        x_memcpy(&prPsInfo->rPlayerGenericInfo, &(rMinfoRec.u.t_general_info),
            sizeof(MINFO_TYPE_GENERIC_T));

        // video attribute
        i4Ret = x_minfo_get_num_recs(hMinfo, 
                    MINFO_INFO_TYPE_VIDEO_ATTR, 
                    &prMediaInfo->u2VidStmNum);
        if (i4Ret != MINFOR_OK)
        {
            LOG(1, "x_minfo_get_num_recs MINFO_INFO_TYPE_VIDEO_ATTR %d\n", i4Ret);
            break;
        }

        prPsInfo->u2VidIdx = 0;
        for (u4Temp = 0; u4Temp < prMediaInfo->u2VidStmNum; u4Temp++)
        {
            if(u4Temp >= 17)
            {
                LOG(1, "_PLAYMGR_MpgPsMediaInfoNotify u2VidStmNum >= 17\n");
                ASSERT(u4Temp < 17);
                break;
            }
            i4Ret = x_minfo_get_info(hMinfo, MINFO_INFO_TYPE_VIDEO_ATTR,
                        (MINFO_REC_IDX_T)(1+u4Temp), &rMinfoRec, &zRecLength);
            if (i4Ret != MINFOR_OK)
            {
                LOG(1, "x_minfo_get_info MINFO_INFO_TYPE_VIDEO_ATTR %d\n", i4Ret);
                break;
            }
            x_memcpy(&prPsInfo->rStmAttribute[u4Temp], 
                &(rMinfoRec.u.t_stm_attribute),
                sizeof(MINFO_TYPE_STM_ATRBT_T));
        }

        // audio attribute
        i4Ret = x_minfo_get_num_recs(hMinfo,
                    MINFO_INFO_TYPE_AUDIO_ATTR,
                    &prMediaInfo->u2AudStmNum);
        if (i4Ret != MINFOR_OK)
        {
            LOG(1, "x_minfo_get_num_recs MINFO_INFO_TYPE_AUDIO_ATTR %d\n", i4Ret);
            break;
        }

        prPsInfo->u2AudIdx = prMediaInfo->u2VidStmNum;
        for (u4Temp = 0; u4Temp < prMediaInfo->u2AudStmNum; u4Temp++)
        {
            if((UINT32)(prMediaInfo->u2VidStmNum + u4Temp) >= (UINT32)17)
            {
                LOG(1, "_PLAYMGR_MpgPsMediaInfoNotify u2VidStmNum >= 17\n");
                ASSERT(u4Temp < 17);
                break;
            }
            i4Ret = x_minfo_get_info(hMinfo, MINFO_INFO_TYPE_AUDIO_ATTR,
                        (MINFO_REC_IDX_T)(1+u4Temp), &rMinfoRec, &zRecLength);
            if (i4Ret != MINFOR_OK)
            {
                LOG(1, "x_minfo_get_info MINFO_INFO_TYPE_AUDIO_ATTR %d\n", i4Ret);
                break;
            }
            x_memcpy(&prPsInfo->rStmAttribute[prMediaInfo->u2VidStmNum + u4Temp], 
                &(rMinfoRec.u.t_stm_attribute),
                sizeof(MINFO_TYPE_STM_ATRBT_T));
        }

        // meta data info
        i4Ret = x_minfo_get_info(hMinfo, MINFO_INFO_TYPE_META_DATA,
                    (MINFO_REC_IDX_T)1, &rMinfoRec, &zRecLength);
        if ((zRecLength != sizeof(MINFO_INFO_T)) || i4Ret)
        {
            LOG(1, "x_minfo_get_info MINFO_INFO_TYPE_META_DATA %d\n", i4Ret);
            break;
        }
        x_memcpy(&prPsInfo->rMetaDat, &(rMinfoRec.u.t_meta_data),
            sizeof(MINFO_TYPE_META_DATA_T));
            
        break;
        
    case MINFO_NFY_EVENT_HANDLE_CLOSED:
        break;
    case MINFO_NFY_EVENT_ERROR:
        break;
    default:
        break;
    }

    UNUSED(_PLAYMGR_SendNotify((UINT32)e_event));
    UNUSED(pvData1);
    UNUSED(pvData2);
}

BOOL _PLAYMGR_MpgStart(VOID* pvMediaInfo)
{
    PLAYMGR_INFO_T* prMediaInfo = (PLAYMGR_INFO_T*)pvMediaInfo;
    BOOL fgRet;
    PLAYMGR_SourceType_T ePlayMgrSrcType;

    LOG(5, "_PLAYMGR_MpgStart\n");
    
    if((prMediaInfo == NULL) || (pvMediaInfo == NULL))
    {
        LOG(1, "_PLAYMGR_MpgStart prMediaInfo pvMediaInfo NULL\n");
        return FALSE;
    }

    /* move to _PLAYMGR_Set
    if (prMediaInfo->u2VidStmNum > 0)
    {        
        fgRet = SWDMX_SetStrmID(prMediaInfo->rPlayerStatus.u4PlayVidStrmId,
            0, eSWDMX_STRM_TYPE_VID); 
        if (!fgRet)
        {
            LOG(1, "_PLAYMGR_MpgStart Fail to set video stream ID\n");
            return FALSE;
        }
        
        fgRet = SWDMX_SetInfo((UINT8)eSWDMX_ENABLE_STREAM, (UINT32)eSWDMX_STRM_TYPE_VID, 0, 0);
        if (!fgRet)
        {
            LOG(1, "_PLAYMGR_MpgStart Fail to enable video stream\n");
            return FALSE;
        }
    }

    if (prMediaInfo->u2AudStmNum > 0)
    {        
        fgRet = SWDMX_SetStrmID(prMediaInfo->rPlayerStatus.u4PlayAudStrmId, 
            0, eSWDMX_STRM_TYPE_AUD); 
        if (!fgRet)
        {
            LOG(1, "_PLAYMGR_MpgStart Fail to set audio stream ID\n");
            return FALSE;
        }
        
        fgRet = SWDMX_SetInfo((UINT8)eSWDMX_ENABLE_STREAM, (UINT32)eSWDMX_STRM_TYPE_AUD, 0, 0);
        if (!fgRet)
        {
            LOG(1, "_PLAYMGR_MpgStart Fail to enable audio stream\n");
            return FALSE;
        }
    }
    */
#ifdef TIME_SHIFT_SUPPORT
    if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS_TIME_SHIFT)
    {
        x_memset(&prMediaInfo->FmtInfo.rPsInfo.rKeyIdxTBL, 0, sizeof(MM_RANGE_ELMT_IDXTBL_T));
        x_memset(&prMediaInfo->FmtInfo.rPsInfo.rIdxTsEntry, 0, sizeof(MM_RANGE_ELEMT_IDXTBL_TS_ENTRY_T));
        prMediaInfo->FmtInfo.rPsInfo.rKeyIdxTBL.u.pt_ts_idx_tbl_entry =
            &prMediaInfo->FmtInfo.rPsInfo.rIdxTsEntry;
        prMediaInfo->rRangeInfo[0].pt_idxtbl_lst = &prMediaInfo->FmtInfo.rPsInfo.rKeyIdxTBL;
        x_memcpy(&prMediaInfo->FmtInfo.rPsInfo.rIdxTsEntry.t_tick_ctrl_blk,
                    &prMediaInfo->FmtInfo.rPsInfo.rTickCtl,
                    sizeof(PVR_TICK_CTRL_BLK_T));
    }
#endif

    ePlayMgrSrcType = _PLAYMGR_GetSrcType();
    switch(ePlayMgrSrcType)
    {
        case PLAYMGR_SRC_TYPE_NETWORK_DLNA:
            prMediaInfo->rRangeInfo[0].e_mm_src_type = MM_SRC_TYPE_NETWORK_DLNA;
        break;
        case PLAYMGR_SRC_TYPE_NETWORK_VUDU:
            prMediaInfo->rRangeInfo[0].e_mm_src_type = MM_SRC_TYPE_NETWORK_VUDU;
        break;
        case PLAYMGR_SRC_TYPE_NETWORK_RHAPSODY:
            prMediaInfo->rRangeInfo[0].e_mm_src_type = MM_SRC_TYPE_NETWORK_RHAPSODY;
        break;
        case PLAYMGR_SRC_TYPE_HIGH_SPEED_STORAGE:
        default:
            prMediaInfo->rRangeInfo[0].e_mm_src_type = MM_SRC_TYPE_HIGH_SPEED_STORAGE;
        break;
    }

    fgRet = SWDMX_AddRange((void*)&prMediaInfo->rRangeInfo[0], prMediaInfo->rRangeInfo[0].ui4_range_id);
    if(!fgRet)
    {
        LOG(1, "_PLAYMGR_MpgStart SWDMX_AddRange Fail\n");
        return FALSE;
    }
    
    return TRUE;
}


BOOL _PLAYMGR_MpgPause(VOID)
{
    // Eddie
    return TRUE;
}

BOOL _PLAYMGR_MpgStop(VOID* pvMediaInfo)
{
    //BOOL fgRet;
    INT32 i4Ret = TRUE;
    PLAYMGR_INFO_T* prMediaInfo = (PLAYMGR_INFO_T*)pvMediaInfo;

    if((prMediaInfo == NULL) || (pvMediaInfo == NULL))
    {
        LOG(5, "_PLAYMGR_MpgStop prMediaInfo NULL\n");
        return FALSE;
    }
    
    LOG(5, "_PLAYMGR_MpgStop\n");
    
    i4Ret = SWDMX_DelRange((VOID*)&(prMediaInfo->rRangeInfo[0]), 0, FALSE);
    if (!i4Ret)
    {
        LOG(1, "_PLAYMGR_MpgStop Fail in _SWDMX_DelRange\n");
        return FALSE;
    }

    /* move to _PLAYMGR_Set
    fgRet = SWDMX_SetInfo((UINT8)eSWDMX_DISABLE_STREAM, (UINT32)eSWDMX_STRM_TYPE_VID, 0, 0);
    if (!fgRet)
    {
        //LOG(1, "_PLAYMGR_MpgStop Fail to disable video stream\n");
        //return FALSE;
    }
    
    fgRet = SWDMX_SetInfo((UINT8)eSWDMX_DISABLE_STREAM, (UINT32)eSWDMX_STRM_TYPE_AUD, 0, 0);
    if (!fgRet)
    {
        //LOG(1, "_PLAYMGR_MpgStop Fail to disable audio stream\n");
        //return FALSE;
    }
    
    fgRet = SWDMX_SetInfo((UINT8)eSWDMX_DISABLE_STREAM, (UINT32)eSWDMX_STRM_TYPE_SUBTITLE, 0, 0);
    if (!fgRet)
    {
        //LOG(1, "_PLAYMGR_MpgStop Fail to disable subtitle stream\n");
        //return FALSE;
    }
    */
    
    return TRUE;
}


BOOL _PLAYMGR_IsBase(VOID)
{
    INT32 i4Ret;
    FEEDER_BUF_INFO_T rFeederInfo;
    if(_hPlaymgrFeederReqDataQ == 0)
    {
        // create data queue
        VERIFY(x_msg_q_create(&_hPlaymgrFeederReqDataQ, "PLAYMGR_FEEDER_DATAQ",
        sizeof(FEEDER_REQ_DATA_T), PLAYMGR_FEEDER_DATA_Q_SIZE) == OSR_OK);
    }
    
    if (FeederGetBufferInfo(FEEDER_PROGRAM_SOURCE, &rFeederInfo) != 
      FEEDER_E_OK)
    {
	    LOG(5, "_PLAYMGR_IsBase FeederGetBufferInfo Fail\n");
        return FALSE;
    }
        
    i4Ret = FeederInstallCallback(FEEDER_PROGRAM_SOURCE, FEEDER_NOTIFY_DATA_REQ_DONE, 
        _PLAYMGR_MpgFeederCallBack, (UINT32)&rFeederInfo);
    if(i4Ret != FEEDER_E_OK)
    {
		LOG(7, "_PLAYMGR_IsBase FeederInstallCallback Fail.\n");
		return FALSE;
    }
    return TRUE;
}

PLAYMGR_MEDIA_TYPE_T _PLAYMGR_IsWhat(VOID)
{
	FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_BUF_INFO_T rFeederInfo;
    FEEDER_REQ_DATA_T rFeederReqData;
    UINT8 ucTmp, ucTmp1, ucTmp2, ucTmp3;
    BOOL fgRet;
    INT32 i4Ret;

    fgRet = _PLAYMGR_IsBase();
    if(!fgRet)
    {
		LOG(7, "_PLAYMGR_IsWhat _PLAYMGR_IsBase Fail.\n");
		return PLAYMGR_MEDIA_TYPE_UNKNOWN;
    }
    
    i4Ret = FeederSetRequest(FEEDER_PROGRAM_SOURCE, 
            READ_SIZE,//(4), 
            (UINT64)(0),
            &rFeederCond,
            0);//_u4FeederReqID++);
    if(i4Ret != FEEDER_E_OK)
    {
		LOG(7, "_PLAYMGR_IsWhat FeederSetRequest Fail.\n");
		return PLAYMGR_MEDIA_TYPE_UNKNOWN;
    }

    fgRet = _PLAYMGR_MpgReceiveFeederAck((VOID*)&rFeederReqData);
    if(!fgRet)
	{
		LOG(7, "_PLAYMGR_IsWhat _PLAYMGR_MpgReceiveFeederAck Fail.\n");
		return PLAYMGR_MEDIA_TYPE_UNKNOWN;
	}

    if (FeederGetBufferInfo(FEEDER_PROGRAM_SOURCE, &rFeederInfo) != 
      FEEDER_E_OK)
    {
	    LOG(5, "_PLAYMGR_IsWhat FeederGetBufferInfo Fail\n");
        return PLAYMGR_MEDIA_TYPE_UNKNOWN;
    }
        
    if((rFeederReqData.u4WriteAddr + (188*3)) > rFeederInfo.u4EndAddr)
    {
		LOG(5, "_PLAYMGR_IsWhat u4WriteAddr Fail.\n");
        return PLAYMGR_MEDIA_TYPE_UNKNOWN;
    }
	
	ucTmp = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr);
	ucTmp1 = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+1);
	ucTmp2 = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+2);
	ucTmp3 = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+3);
	if((ucTmp == 0x0) && (ucTmp1 == 0x0) && (ucTmp2 == 0x01) && (ucTmp3 == 0xB3))
    {
        return PLAYMGR_MEDIA_TYPE_MPEG2_ES;
    }
	/*
	else if((ucTmp == 0x0) && (ucTmp1 == 0x0) && (ucTmp2 == 0x01) && (ucTmp3 == 0x00))
    {
        return PLAYMGR_MEDIA_TYPE_MPEG2_ES;
    }
    */
	// use utility to identify PS
	#if 0
	else if(ucTmp == 0x0 && ucTmp1 == 0x0 && ucTmp2 == 0x01 && ucTmp3 == 0xBA)
    {
        return PLAYMGR_MEDIA_TYPE_MPEG2_PS;
    }
	#endif
	else if((ucTmp == 0x0) && (ucTmp1 == 0x0) && (ucTmp2 == 0x00) && (ucTmp3 == 0x01))
    {
        return PLAYMGR_MEDIA_TYPE_H264_ES;
    }
	
	// use utility to identify TS
	#if 0
    UINT32 u4Size=0;
    UINT32 u4Cnt=0;
	ucTmp = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr);
	u4Cnt = 0;
	u4Size = 0;
	for(;(u4Cnt < 5) && (u4Size < (READ_SIZE - 188*2));u4Cnt++)
    {
        ucTmp = 0xFF;
        while(ucTmp != 0x47 && u4Size < (READ_SIZE - 188*2))
        {
            u4Size++;
            ucTmp = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+u4Size);
        }
        ucTmp1 = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+188+u4Size);
        ucTmp2 = *(UINT8*)VIRTUAL(rFeederReqData.u4WriteAddr+(188*2)+u4Size);
        if(ucTmp == 0x47 && ucTmp1 == 0x47 && ucTmp2 == 0x47)
        {
            return PLAYMGR_MEDIA_TYPE_MPEG2_TS;
        }
        u4Size+=188;
    }
	#endif
	
	return PLAYMGR_MEDIA_TYPE_UNKNOWN;	
}

BOOL _PLAYMGR_IsMPG(PLAYMGR_INFO_T* prMediaInfo, CHAR *szFileName)
{
    PLAYMGR_MEDIA_TYPE_T eType = _PLAYMGR_IsWhat();    
    
    if(prMediaInfo == NULL)
    {
        LOG(5, "_PLAYMGR_IsMPG prMediaInfo NULL\n");
        return FALSE;
    }
    
    if(eType == PLAYMGR_MEDIA_TYPE_MPEG2_ES)
    {
        LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_MPEG2_ES\n");
        prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_VIDEO_ES;
        prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_MPEG2_ES;
        prMediaInfo->rMediaFmt.t_media_subtype.u.e_vid_subtype = MEDIA_VID_SUBTYPE_M2V;
        return TRUE;
    }
	// use utility to identify PS
	#if 0
    else if(eType == PLAYMGR_MEDIA_TYPE_MPEG2_PS)
    {
        LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_MPEG2_PS\n");
        prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_PS;
        prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_MPEG2_PS;
        return TRUE;
    }
    else if(eType == PLAYMGR_MEDIA_TYPE_MPEG2_TS)
    {
        
        LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_MPEG2_TS\n");
        prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_TS;
        prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_MPEG2_TS;
        return TRUE;
    }
	#endif
    else if(eType == PLAYMGR_MEDIA_TYPE_H264_ES)
    {
        
        LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_H264_ES\n");
        prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_VIDEO_ES;
        prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_H264_ES;
        return TRUE;
    }
    else
    {        
        CHAR* szExt;
        if((szFileName == NULL) || (prMediaInfo == NULL))
        {
            return FALSE;
        }
        szExt = (x_strrchr(szFileName, '.') + 1);
        if(szExt == NULL)
        {
            prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_UNKNOWN;
            return FALSE;
        }
        if((x_strcmp(szExt, "m4v") == 0) ||
           (x_strcmp(szExt, "M4V") == 0))
        {
            LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_MPEG4_ES\n");
            prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_VIDEO_ES;
            prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_MPEG4_ES;
            return TRUE;
        }
        else if((x_strcmp(szExt, "vc1") == 0) ||
           (x_strcmp(szExt, "VC1") == 0))
        {
            LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_VC1_ES\n");
            prMediaInfo->rMediaFmt.e_media_type = MEDIA_TYPE_VIDEO_ES;
            prMediaInfo->rPlayerStatus.eMediaType = PLAYMGR_MEDIA_TYPE_VC1_ES;
            return TRUE;
        }
        LOG(5, "_PLAYMGR_IsMPG PLAYMGR_MEDIA_TYPE_UNKNOWN\n");
        return FALSE;
    }
}

BOOL _PLAYMGR_OpenMpgMediaInfo(PLAYMGR_STATUS_T* prMediaStatus, VOID* pvMediaInfo)
{
    ENUM_SWDMX_FMT_T eFMT;
    PLAYMGR_INFO_T* prMediaInfo;
    MEDIA_FORMAT_T* prformat;
    PLAYMGR_PS_INFO_T* prPsInfo;
    BOOL fgRet;
    UINT64 u8Remainder;
    UINT64 u8Tmp;
    UINT32 u4Msg;
    INT32 i4Ret;
    UINT32 i;
    UINT32 j;
    
    LOG(5, "_PLAYMGR_OpenMpgMediaInfo\n");

    if(prMediaStatus == NULL)
    {
        LOG(1, "_PLAYMGR_OpenMpgMediaInfo prMediaStatus = NULL\n");
        return FALSE;
    }
    
    x_memset(&_rPlayerMpgInfo, 0, sizeof(_rPlayerMpgInfo));
    eFMT = SWDMX_FMT_UNKNOWN;
    // parsing media data
    prMediaInfo = (PLAYMGR_INFO_T*)pvMediaInfo;
    if(prMediaInfo == NULL)
    {
        LOG(1, "_PLAYMGR_OpenMpgMediaInfo prMediaInfo = NULL\n");
        return FALSE;
    }
    prformat = (&prMediaInfo->rMediaFmt);
    
    prPsInfo = (PLAYMGR_PS_INFO_T*)&(prMediaInfo->FmtInfo.rPsInfo);
    
    if((prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_VCD) ||
        (prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_PS))
    {        
        // Open for media info
        i4Ret = x_minfo_mopen((VOID *)&(prMediaInfo->rMediaFeeder), 0, _PLAYMGR_MpgPsMediaInfoNotify, prMediaInfo,
                		       prformat, &(prPsInfo->hMediaInfoHandle));
        if(i4Ret != MINFOR_OK)
        {
            LOG(1, "_PLAYMGR_OpenMpgMediaInfo x_minfo_mopen %d\n", i4Ret);
            return FALSE;
        }

        fgRet = _PLAYMGR_ReceiveNotify((VOID*)&u4Msg);
        if ((!fgRet) && (u4Msg != (UINT32)MINFO_NFY_EVENT_PARSING_DONE))
        {
            LOG(1, "_PLAYMGR_OpenMpgMediaInfo _PLAYMGR_ReceiveNotify %d %d\n",
                (UINT32)fgRet, (UINT32)u4Msg);
            return FALSE;
        }

        if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_VCD)
        {
            eFMT = SWDMX_FMT_MPEG1_DAT;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_PS)
        {
            eFMT = SWDMX_FMT_MPEG2_PS;
        }

        _rPlayerMpgInfo.u4PgmNum = 1;
        prMediaInfo->rPlayerStatus.u2TotalPrmNum = 1;
        if (prMediaInfo->u2VidStmNum > 0)
        {
            if (prMediaInfo->u2VidStmNum < PLAYMGR_MAX_VID_TRACK)
            {
                //prMediaStatus->u2VideoTracks = prMediaInfo->u2VidStmNum;
                prMediaInfo->rPlayerStatus.u2VideoTracks = prMediaInfo->u2VidStmNum;
                _rPlayerMpgInfo.rPgm[0].u2VideoNum = prMediaInfo->u2VidStmNum;
            }
            else
            {
                //prMediaStatus->u2VideoTracks = PLAYMGR_MAX_VID_TRACK;
                prMediaInfo->rPlayerStatus.u2VideoTracks = (UINT16)(PLAYMGR_MAX_VID_TRACK-1);
                _rPlayerMpgInfo.rPgm[0].u2VideoNum = (UINT16)(PLAYMGR_MAX_VID_TRACK-1);
            }
        
            for(i=0;i<prMediaInfo->rPlayerStatus.u2VideoTracks;i++)
            {
                if (i >= PLAYMGR_MAX_VID_TRACK)
                {
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_VID_TRACK\n");
                    break;
                }
                if(i >= 17)
                {
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo u2VidStmNum >= 17\n");
                    break;
                }
                
                if((prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_MPEG_1) ||
                    (prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_MPEG_2))
                {
                    _rPlayerMpgInfo.rPgm[0].aeVideo[i] = ENUM_SWDMX_VDEC_MPEG1_MPEG2;
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG1_MPEG2;
                }
                else if(prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_H264)
                {
                    _rPlayerMpgInfo.rPgm[0].aeVideo[i] = ENUM_SWDMX_VDEC_H264;
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_H264;
                }
                else if(prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_WVC1)
                {
                    _rPlayerMpgInfo.rPgm[0].aeVideo[i] = ENUM_SWDMX_VDEC_VC1;
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_VC1;
                }
                else if(prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_MPEG_4)
                {
                    _rPlayerMpgInfo.rPgm[0].aeVideo[i] = ENUM_SWDMX_VDEC_MPEG4;
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG4;
                }
                else
                {
                    _rPlayerMpgInfo.rPgm[0].aeVideo[i] = ENUM_SWDMX_VDEC_UNKNOWN;
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] =PLAYMGR_VDEC_UNKNOWN;
                    LOG(3, "Unknown Video %d\n", (prPsInfo->rStmAttribute[i].u.t_ps_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_H264));
                }
                prMediaInfo->rPlayerStatus.au4VideoId[i] = 
                    (UINT32)prPsInfo->rStmAttribute[i].t_stm_id.u.t_ps_stm_id.ui1_stm_id;
                _rPlayerMpgInfo.rPgm[0].au2VideoId[i] =
                    (UINT16)prMediaInfo->rPlayerStatus.au4VideoId[i];
                LOG(5, "Video %d, Id %d\n",
                    (UINT32)prMediaInfo->rPlayerStatus.aeVidFormat[i],
                    prMediaInfo->rPlayerStatus.au4VideoId[i]);
            }
        }
        
        if (prMediaInfo->u2AudStmNum > 0)
        {
            if (prMediaInfo->u2AudStmNum < PLAYMGR_MAX_AUD_TRACK)
            {
                //prMediaStatus->u2AudioTracks = prMediaInfo->u2AudStmNum;            
                prMediaInfo->rPlayerStatus.u2AudioTracks = prMediaInfo->u2AudStmNum;
                _rPlayerMpgInfo.rPgm[0].u2AudioNum = prMediaInfo->u2AudStmNum;
            }
            else
            {
                //prMediaStatus->u2AudioTracks = PLAYMGR_MAX_AUD_TRACK;            
                prMediaInfo->rPlayerStatus.u2AudioTracks = (UINT16)(PLAYMGR_MAX_AUD_TRACK-1);
                _rPlayerMpgInfo.rPgm[0].u2AudioNum = (UINT16)(PLAYMGR_MAX_AUD_TRACK-1);
            }
            
            for(i=0;i<prMediaInfo->rPlayerStatus.u2AudioTracks;i++)
            {
                if (i >= PLAYMGR_MAX_AUD_TRACK)
                {
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_AUD_TRACK\n");
                    break;
                }
                if((UINT32)(i+prMediaInfo->u2VidStmNum) >= (UINT32)17)
                {     
                    prMediaInfo->rPlayerStatus.u2AudioTracks = (UINT16)i;
                    _rPlayerMpgInfo.rPgm[0].u2AudioNum = (UINT16)i;
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_AUD_TRACK >= 17\n");
                    break;
                }
                
                if(prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_enc == MINFO_INFO_AUD_ENC_AC3)
                {
                    _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_AC3;
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_AC3;
                }
                else if(prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_enc == MINFO_INFO_AUD_ENC_MPEG_1)
                {
                    if(prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_layer == 3)
                    {
                        _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_MP3;
                        //prMediaStatus->aeAudFormat[i] = 
                        prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_MP3;
                    }
                    else
                    {
                        _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_MPEG;
                        //prMediaStatus->aeAudFormat[i] = 
                        prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_MPEG;
                    }
                }
                else if(prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_enc == MINFO_INFO_AUD_ENC_MPEG_2)
                {
                    _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_MPEG;
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_MPEG;
                }
                else if(prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_enc == MINFO_INFO_AUD_ENC_LPCM)
                {
                    _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_PCM;
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_PCM;
                }
                else
                {
                    _rPlayerMpgInfo.rPgm[0].aeAudio[i] = ENUM_SWDMX_ADEC_UNKNOWN;
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_UNKNOWN;
                    LOG(3, "Unknown Audio %d\n", (prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].u.t_ps_audio_stm_attr.e_enc == MINFO_INFO_AUD_ENC_AC3));
                }
                prMediaInfo->rPlayerStatus.au4AudioId[i] = 
                    (UINT32)prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].t_stm_id.u.t_ps_stm_id.ui1_stm_id;
                _rPlayerMpgInfo.rPgm[0].au2AudioId[i] =
                    (UINT16)prMediaInfo->rPlayerStatus.au4AudioId[i];

                prMediaInfo->rPlayerStatus.au4AudioSubId[i] = 
                    (UINT32)prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum].t_stm_id.u.t_ps_stm_id.ui1_sub_stm_id;
                _rPlayerMpgInfo.rPgm[0].au2AudioSubId[i] =
                    (UINT16)prMediaInfo->rPlayerStatus.au4AudioSubId[i];
                
                LOG(5, "Audio %d, Id %d, SubId %d\n",
                    (UINT32)prMediaInfo->rPlayerStatus.aeAudFormat[i],
                    prMediaInfo->rPlayerStatus.au4AudioId[i],
                    prMediaInfo->rPlayerStatus.au4AudioSubId[i]);
            }
        }
        
        if (prMediaInfo->u2SubStmNum > 0)
        {
            if (prMediaInfo->u2SubStmNum < PLAYMGR_MAX_SUB_TRACK)
            {
                //prMediaStatus->u2CaptionTracks = prMediaInfo->u2SubStmNum;            
                prMediaInfo->rPlayerStatus.u2CaptionTracks = prMediaInfo->u2SubStmNum;
                _rPlayerMpgInfo.rPgm[0].u2SubpicNum = prMediaInfo->u2SubStmNum;
            }
            else
            {
                //prMediaStatus->u2CaptionTracks = PLAYMGR_MAX_SUB_TRACK;            
                prMediaInfo->rPlayerStatus.u2CaptionTracks = (UINT16)(PLAYMGR_MAX_SUB_TRACK-1);
                _rPlayerMpgInfo.rPgm[0].u2SubpicNum = (UINT16)(PLAYMGR_MAX_SUB_TRACK-1);
            }
            
            for(i=0;i<prMediaInfo->rPlayerStatus.u2CaptionTracks;i++)
            {
                if (i >= PLAYMGR_MAX_SUB_TRACK)
                {
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_SUB_TRACK\n");
                    break;
                }
                if ((UINT16)(i+prMediaInfo->u2VidStmNum+prMediaInfo->u2AudStmNum) >= (UINT16)17)
                {
                    prMediaInfo->rPlayerStatus.u2CaptionTracks = (UINT16)(i);
                    _rPlayerMpgInfo.rPgm[0].u2SubpicNum = (UINT16)(i);
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_SUB_TRACK >= 17\n");
                    break;
                }
                
                //prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum+prMediaInfo->u2AudStmNum].u.t_ps_sp_stm_attr                
                _rPlayerMpgInfo.rPgm[0].aeSubpic[i] = ENUM_SWDMX_SUBPIC_UNKNOWN;
                //prMediaStatus->aeCapFormat[i] = 
                prMediaInfo->rPlayerStatus.aeCapFormat[i] = PLAYMGR_CAPTION_MPEG2;
                prMediaInfo->rPlayerStatus.au4CapTrackId[i] = 
                    (UINT32)prPsInfo->rStmAttribute[i+prMediaInfo->u2VidStmNum+prMediaInfo->u2AudStmNum].t_stm_id.u.t_ps_stm_id.ui1_stm_id;
                _rPlayerMpgInfo.rPgm[0].au2SubpicId[i] =
                    (UINT16)prMediaInfo->rPlayerStatus.au4CapTrackId[i];
                
                LOG(5, "Caption %d, Id %d\n",
                    (UINT32)prMediaInfo->rPlayerStatus.aeCapFormat[i],
                    prMediaInfo->rPlayerStatus.au4CapTrackId[i]);
            }
        }        
        
        prMediaInfo->rRangeInfo[0].ui8_fileoffset = 0;
        prMediaInfo->rRangeInfo[0].z_range_sz = (SIZE_T)prPsInfo->rPlayerGenericInfo.ui8_media_sz;
        prMediaInfo->rRangeInfo[0].ui8_pb_duration = prPsInfo->rPlayerGenericInfo.ui8_pb_duration;
        prMediaInfo->rRangeInfo[0].t_pts_offset = prPsInfo->rPlayerGenericInfo.ui8_preroll;
        prMediaInfo->rPlayerStatus.u4TotalDuration = (UINT32)prPsInfo->rPlayerGenericInfo.ui8_pb_duration;
        //prMediaInfo->rPlayerStatus.u4TotalDuration = (UINT32)prPsInfo->rPlayerGenericInfo.ui8_pb_duration;
        //prMediaStatus->u4TotalDuration = (UINT32)prPsInfo->rPlayerGenericInfo.ui8_pb_duration;

    }
    else //if(1)
    {
        prMediaInfo->rRangeInfo[0].ui8_fileoffset = 0;
        prMediaInfo->rRangeInfo[0].z_range_sz = (SIZE_T)prMediaInfo->u8FileSize;
        
        eFMT = SWDMX_FMT_UNKNOWN;
        if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_VCD)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG1_DAT\n");
            eFMT = SWDMX_FMT_MPEG1_DAT;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_PS)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_PS\n");
            eFMT = SWDMX_FMT_MPEG2_PS;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_TS\n");
            eFMT = SWDMX_FMT_MPEG2_TS;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS_192)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_TS_192\n");
            eFMT = SWDMX_FMT_MPEG2_TS_192;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS_ZERO_192)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_TS_ZERO_192\n");
            eFMT = SWDMX_FMT_MPEG2_TS_ZERO_192;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS_TIME_SHIFT)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_TS_TIME_SHIFT\n");
            eFMT = SWDMX_FMT_MPEG2_TS_TIME_SHIFT;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_ES)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MEDIA_TYPE_MPEG2_ES\n");
            eFMT = SWDMX_FMT_MPEG2_VIDEO_ES;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_H264_ES)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MEDIA_TYPE_MPEG2_ES\n");
            eFMT = SWDMX_FMT_H264_VIDEO_ES;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_VC1_ES)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MEDIA_TYPE_VC1_ES\n");
            eFMT = SWDMX_FMT_VC1_ES;
        }
        else if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG4_ES)
        {
            LOG(5, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MEDIA_TYPE_MPEG4_ES\n");
            eFMT = SWDMX_FMT_MPEG4_VIDEO_ES;
        }
        else
        {
            LOG(1, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_UNKNOWN Fail\n");
            return FALSE;
        }

        if((eFMT == SWDMX_FMT_MPEG2_TS_TIME_SHIFT) ||
            (_PLAYMGR_GetSrcType() == PLAYMGR_SRC_TYPE_NETWORK_VUDU))
        {
            LOG(3, "_PLAYMGR_OpenMpgMediaInfo SWDMX_FMT_MPEG2_TS_TIME_SHIFT OK\n");
            
            prMediaInfo->rRangeInfo[0].t_pts_offset = 0;
            prMediaInfo->rRangeInfo[0].ui8_fileoffset = 0;
            prMediaInfo->rRangeInfo[0].z_range_sz = (UINT64)(-1);
            prMediaInfo->rRangeInfo[0].ui8_pb_duration = 0;
            prMediaInfo->rPlayerStatus.u4TotalDuration = 0;
            
            return TRUE;
        }

        fgRet = SWDMX_GetStreamInfo(eFMT, SWDMX_STREAM_INFO_MASK_TRACK,
                    prMediaInfo->u8FileSize, &_rPlayerMpgInfo);
        if (!fgRet)
        {
            LOG(1, "_PLAYMGR_OpenMpgMediaInfo SWDMX_GetStreamInfo Fail\n");
            return FALSE;
        }

        prMediaInfo->rRangeInfo[0].t_pts_offset = _rPlayerMpgInfo.rPgm[0].u4FirstPts;
        prMediaInfo->rRangeInfo[0].ui8_fileoffset = 0;
        prMediaInfo->rRangeInfo[0].z_range_sz = (SIZE_T)prMediaInfo->u8FileSize;
        prMediaInfo->rRangeInfo[0].ui8_pb_duration = _rPlayerMpgInfo.rPgm[0].u4TotalTime;
        prMediaInfo->rPlayerStatus.u4TotalDuration = (UINT32)_rPlayerMpgInfo.rPgm[0].u4TotalTime;
        
        x_memset((void*)&(prPsInfo->rMetaDat), 0, sizeof(MINFO_TYPE_META_DATA_T));
        prPsInfo->rMetaDat.ui8_pb_duration = _rPlayerMpgInfo.rPgm[0].u4TotalTime;
        if(_rPlayerMpgInfo.rPgm[0].u4TotalTime > 0)
        {
            u8Tmp = (u8Div6432((prMediaInfo->u8FileSize * 8 * 90000),
                (UINT64)(_rPlayerMpgInfo.rPgm[0].u4TotalTime), &u8Remainder));
            prPsInfo->rMetaDat.ui4_bitrate = (UINT32)u8Tmp;
        }
        prMediaInfo->rPlayerStatus.u2TotalPrmNum = (UINT16)_rPlayerMpgInfo.u4PgmNum;
        if(_rPlayerMpgInfo.rPgm[0].u2VideoNum > 0)
        {
            prMediaInfo->u2VidStmNum = _rPlayerMpgInfo.rPgm[0].u2VideoNum;
            prMediaInfo->rPlayerStatus.u2VideoTracks = _rPlayerMpgInfo.rPgm[0].u2VideoNum;
            
            for(i=0;i<_rPlayerMpgInfo.rPgm[0].u2VideoNum;i++)
            {
                if(i >= PLAYMGR_MAX_VID_TRACK)
                {
                    prMediaInfo->u2VidStmNum = (UINT16)i;
                    prMediaInfo->rPlayerStatus.u2VideoTracks = (UINT16)i;
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_VID_TRACK\n");
                    break;
                }
                LOG(5, "_rPlayerMpgInfo.rPgm[0].aeVideo %d, Id %d\n", _rPlayerMpgInfo.rPgm[0].aeVideo[i], _rPlayerMpgInfo.rPgm[0].au2VideoId[i]);
                prMediaInfo->rPlayerStatus.au4VideoId[i] = _rPlayerMpgInfo.rPgm[0].au2VideoId[i];
                if(_rPlayerMpgInfo.rPgm[0].aeVideo[i] == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
                {
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG1_MPEG2;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeVideo[i] == ENUM_SWDMX_VDEC_H264)
                {
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_H264;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeVideo[i] == ENUM_SWDMX_VDEC_VC1)
                {
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_VC1;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeVideo[i] == ENUM_SWDMX_VDEC_MPEG4)
                {
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG4;
                }
                else
                {
                    //prMediaStatus->aeVidFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_UNKNOWN;
                    //prMediaInfo->rPlayerStatus.u4VideoId = 0;
                    LOG(3, "Unknown Video %d\n", (_rPlayerMpgInfo.rPgm[0].aeVideo[i]));
                }
            }
        }
        if(_rPlayerMpgInfo.rPgm[0].u2AudioNum > 0)
        {
            prMediaInfo->u2AudStmNum = _rPlayerMpgInfo.rPgm[0].u2AudioNum;
            prMediaInfo->rPlayerStatus.u2AudioTracks = _rPlayerMpgInfo.rPgm[0].u2AudioNum;
            
            for(i=0;i<_rPlayerMpgInfo.rPgm[0].u2AudioNum;i++)
            {
                if(i >= PLAYMGR_MAX_AUD_TRACK)
                {
                    prMediaInfo->u2AudStmNum = (UINT16)i;
                    prMediaInfo->rPlayerStatus.u2AudioTracks = (UINT16)i;
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_AUD_TRACK\n");
                    break;
                }
                
                LOG(5, "rInfo.aeAudio %d, Id %d\n", _rPlayerMpgInfo.rPgm[0].aeAudio[i], _rPlayerMpgInfo.rPgm[0].au2AudioId[i]);
                prMediaInfo->rPlayerStatus.au4AudioId[i] = _rPlayerMpgInfo.rPgm[0].au2AudioId[i];
                prMediaInfo->rPlayerStatus.au4AudioSubId[i] = _rPlayerMpgInfo.rPgm[0].au2AudioSubId[i];
                if(_rPlayerMpgInfo.rPgm[0].aeAudio[i] == ENUM_SWDMX_ADEC_AC3)
                {
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_AC3;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeAudio[i] == ENUM_SWDMX_ADEC_MPEG)
                {
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_MPEG;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeAudio[i] == ENUM_SWDMX_ADEC_PCM)
                {
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_PCM;
                }
                else if(_rPlayerMpgInfo.rPgm[0].aeAudio[i] == ENUM_SWDMX_ADEC_AAC)
                {
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_AAC;
                }
                else
                {
                    //prMediaStatus->aeAudFormat[i] = 
                    prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_UNKNOWN;
                    LOG(3, "Unknown Audio %d\n", (_rPlayerMpgInfo.rPgm[0].aeAudio[i]));
                }
            }
        }
        if(_rPlayerMpgInfo.rPgm[0].u2SubpicNum > 0)
        {
            prMediaInfo->u2SubStmNum = _rPlayerMpgInfo.rPgm[0].u2SubpicNum;
            prMediaInfo->rPlayerStatus.u2CaptionTracks = _rPlayerMpgInfo.rPgm[0].u2SubpicNum;
            
            for(i=0;i<_rPlayerMpgInfo.rPgm[0].u2SubpicNum;i++)
            {
                if(i >= PLAYMGR_MAX_SUB_TRACK)
                {
                    prMediaInfo->u2SubStmNum = (UINT16)i;
                    prMediaInfo->rPlayerStatus.u2CaptionTracks = (UINT16)i;
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_SUB_TRACK\n");
                    break;
                }
                
                LOG(5, "rInfo.eSubpic %d\n", _rPlayerMpgInfo.rPgm[0].aeSubpic[i]);
                prMediaInfo->rPlayerStatus.au4CapTrackId[i] = _rPlayerMpgInfo.rPgm[0].au2SubpicId[i];
                //prMediaStatus->aeCapFormat[i] = 
                prMediaInfo->rPlayerStatus.aeCapFormat[i] = PLAYMGR_CAPTION_MPEG2;
                LOG(3, "Unknown Subpic\n");
            }
        }
        
        if (_rPlayerMpgInfo.rPgm[0].u2PcrNum > 0)
        {
            
            prMediaInfo->u2PcrStmNum = _rPlayerMpgInfo.rPgm[0].u2PcrNum;
            prMediaInfo->rPlayerStatus.u2PcrTracks = _rPlayerMpgInfo.rPgm[0].u2PcrNum;
            
            for(i=0;i<_rPlayerMpgInfo.rPgm[0].u2PcrNum;i++)
            {
                if(i >= PLAYMGR_MAX_PCR_TRACK)
                {
                    prMediaInfo->u2PcrStmNum = (UINT16)i;
                    prMediaInfo->rPlayerStatus.u2PcrTracks = (UINT16)i;
                    LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_PCR_TRACK\n");
                    break;
                }
                prMediaInfo->rPlayerStatus.au4PcrId[i] = _rPlayerMpgInfo.rPgm[0].au2PcrId[i];
            }            
        }
    }
    /*
    else // test
    {
        #if 1
        prMediaInfo->u2VidStmNum = 1;
        prMediaInfo->rPlayerStatus.u2VideoTracks = 1;
        prMediaInfo->rPlayerStatus.u2AudioId = 49;
        prMediaInfo->u2VideoId = 49;
        prMediaInfo->rPlayerStatus.aeVidFormat = PLAYMGR_VDEC_MPEG1_MPEG2;
        #else
        prMediaInfo->u2VidStmNum = 1;
        prMediaInfo->rPlayerStatus.u2VideoTracks = 1;
        prMediaInfo->u2AudStmNum = 1;
        prMediaInfo->rPlayerStatus.u2AudioTracks = 1;
        prMediaInfo->rPlayerStatus.u2VideoId = 65;
        prMediaInfo->rPlayerStatus.u2AudioId = 68;
        prMediaInfo->u2VideoId = 65;
        prMediaInfo->u2AudioId = 68;
        prMediaInfo->rPlayerStatus.aeVidFormat = PLAYMGR_VDEC_MPEG1_MPEG2;
        prMediaInfo->rPlayerStatus.aeAudFormat = PLAYMGR_ADEC_AC3;
        #endif        
  
    }
    */

#if 0
    if(prMediaInfo->rPlayerStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS_TIME_SHIFT)
    {
        // ToDo: now PVR can't seek position, wait for this part ready.
        // ilii
    }
    else
#endif
    {
        for(j=0;j<_rPlayerMpgInfo.u4PgmNum;j++)
        {
            if(_rPlayerMpgInfo.rPgm[j].u2VideoNum > 0) // to get video h/w
            {
                // get width/height    
                fgRet = SWDMX_GetStreamInfo(eFMT, SWDMX_STREAM_INFO_MASK_RESOLUTION,
                            prMediaInfo->u8FileSize, &_rPlayerMpgInfo);
                if (!fgRet)
                {
                    LOG(1, "_PLAYMGR_OpenMpgMediaInfo SWDMX_GetStreamInfo sequence Fail\n");
                    //return FALSE;
                }
                for(i=0;i<_rPlayerMpgInfo.rPgm[j].u2VideoNum;i++)
                {
                    if(i >= PLAYMGR_MAX_VID_TRACK)
                    {
                        LOG(3, "_PLAYMGR_OpenMpgMediaInfo PLAYMGR_MAX_VID_TRACK\n");
                        break;
                    }
                    prMediaInfo->rPlayerStatus.au4VideoWidth[i] = _rPlayerMpgInfo.rPgm[j].u4Width[i];
                    prMediaInfo->rPlayerStatus.au4VideoHeight[i] = _rPlayerMpgInfo.rPgm[j].u4Height[i];
                }
                break; // base on
            }
        }
    }

    if((!prMediaInfo->u2VidStmNum) && (!prMediaInfo->u2AudStmNum))
    {
        LOG(1, "_PLAYMGR_MpgOpen no A/V\n");
        return FALSE;
    }

    UNUSED(prMediaStatus);
    return TRUE;
}


BOOL _PLAYMGR_GetMpgMediaInfo(VOID* pvMediaInfo, UINT32 u4Pgm)
{
    PLAYMGR_INFO_T* prMediaInfo;
    UINT32 i;
    
    LOG(5, "_PLAYMGR_GetMpgMediaInfo %d\n", u4Pgm);
    
    prMediaInfo = (PLAYMGR_INFO_T*)pvMediaInfo;
    if(prMediaInfo == NULL)
    {
        LOG(1, "_PLAYMGR_GetMpgMediaInfo prMediaInfo = NULL\n");
        return FALSE;
    }

    if((u4Pgm >= _rPlayerMpgInfo.u4PgmNum) || 
       (u4Pgm >= SWDMX_MAX_PROGRAM))
    {
        LOG(1, "_PLAYMGR_GetMpgMediaInfo u4Pgm %d > u4PgmNum %d\n",
            u4Pgm,
            _rPlayerMpgInfo.u4PgmNum);
        return FALSE;
    }
    prMediaInfo->rPlayerStatus.u2EnPgmNum = (UINT16)u4Pgm;
    
    prMediaInfo->rPlayerStatus.u2VideoTracks = 0;
    prMediaInfo->u2VidStmNum = 0;
    if (_rPlayerMpgInfo.rPgm[u4Pgm].u2VideoNum > 0)
    {
        prMediaInfo->u2VidStmNum =
            _rPlayerMpgInfo.rPgm[u4Pgm].u2VideoNum;
        prMediaInfo->rPlayerStatus.u2VideoTracks =
            prMediaInfo->u2VidStmNum;
    
        for(i=0;i<prMediaInfo->rPlayerStatus.u2VideoTracks;i++)
        {
            if(i >= SWDMX_MAX_VID_TRACK)
            {
                LOG(3, "_PLAYMGR_GetMpgMediaInfo SWDMX_MAX_VID_TRACK\n");
                break;
            }
            if(_rPlayerMpgInfo.rPgm[u4Pgm].aeVideo[i] == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
            {
                prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG1_MPEG2;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeVideo[i] == ENUM_SWDMX_VDEC_H264)
            {
                prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_H264;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeVideo[i] == ENUM_SWDMX_VDEC_VC1)
            {
                prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_VC1;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeVideo[i] == ENUM_SWDMX_VDEC_MPEG4)
            {
                prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_MPEG4;
            }
            else
            {
                prMediaInfo->rPlayerStatus.aeVidFormat[i] = PLAYMGR_VDEC_UNKNOWN;
                LOG(3, "Unknown Video\n");
            }
            prMediaInfo->rPlayerStatus.au4VideoId[i] = 
                (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].au2VideoId[i];
        }
    }
    
    prMediaInfo->rPlayerStatus.u2AudioTracks = 0;
    prMediaInfo->u2AudStmNum = 0;
    if (_rPlayerMpgInfo.rPgm[u4Pgm].u2AudioNum > 0)
    {
        prMediaInfo->u2AudStmNum =
            _rPlayerMpgInfo.rPgm[u4Pgm].u2AudioNum;
        prMediaInfo->rPlayerStatus.u2AudioTracks =
            prMediaInfo->u2AudStmNum;
        
        for(i=0;i<prMediaInfo->rPlayerStatus.u2AudioTracks;i++)
        {
            if(i >= SWDMX_MAX_AUD_TRACK)
            {
                LOG(3, "_PLAYMGR_GetMpgMediaInfo SWDMX_MAX_AUD_TRACK\n");
                break;
            }
            if(_rPlayerMpgInfo.rPgm[u4Pgm].aeAudio[i] == ENUM_SWDMX_ADEC_AC3)
            {
                prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_AC3;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeAudio[i] == ENUM_SWDMX_ADEC_MPEG)
            {
                prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_MPEG;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeAudio[i] == ENUM_SWDMX_ADEC_PCM)
            {
                prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_PCM;
            }
            else if(_rPlayerMpgInfo.rPgm[u4Pgm].aeAudio[i] == ENUM_SWDMX_ADEC_AAC)
            {
                prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_AAC;
            }
            else
            {
                prMediaInfo->rPlayerStatus.aeAudFormat[i] = PLAYMGR_ADEC_UNKNOWN;
                LOG(3, "Unknown Audio\n");
            }
            prMediaInfo->rPlayerStatus.au4AudioId[i] = 
                (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].au2AudioId[i];
            prMediaInfo->rPlayerStatus.au4AudioSubId[i] = 
                (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].au2AudioSubId[i];
        }
    }
    
    prMediaInfo->u2SubStmNum = 0;
    prMediaInfo->rPlayerStatus.u2CaptionTracks = 0;
    if (_rPlayerMpgInfo.rPgm[u4Pgm].u2SubpicNum > 0)
    {
        prMediaInfo->u2SubStmNum =
            _rPlayerMpgInfo.rPgm[u4Pgm].u2SubpicNum;
        prMediaInfo->rPlayerStatus.u2CaptionTracks =
            prMediaInfo->u2SubStmNum;
        
        for(i=0;i<prMediaInfo->rPlayerStatus.u2CaptionTracks;i++)
        {
            if(i >= SWDMX_MAX_SUB_TRACK)
            {
                LOG(3, "_PLAYMGR_GetMpgMediaInfo SWDMX_MAX_SUB_TRACK\n");
                break;
            }
            //if(_rPlayerMpgInfo.rPgm[u4Pgm].aeSubpic[i] == ENUM_SWDMX_SUBPIC_UNKNOWN)
            prMediaInfo->rPlayerStatus.aeCapFormat[i] = PLAYMGR_CAPTION_MPEG2;
            prMediaInfo->rPlayerStatus.au4CapTrackId[i] = 
                (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].au2SubpicId[i];
        }
    }

    prMediaInfo->u2PcrStmNum = 0;
    prMediaInfo->rPlayerStatus.u2PcrTracks = 0;
    if (_rPlayerMpgInfo.rPgm[u4Pgm].u2PcrNum > 0)
    {
        prMediaInfo->u2PcrStmNum =
            _rPlayerMpgInfo.rPgm[u4Pgm].u2PcrNum;
        prMediaInfo->rPlayerStatus.u2PcrTracks =
            prMediaInfo->u2PcrStmNum;
        
        for(i=0;i<prMediaInfo->rPlayerStatus.u2PcrTracks;i++)
        {
            if(i >= SWDMX_MAX_PCR_TRACK)
            {
                LOG(3, "_PLAYMGR_GetMpgMediaInfo SWDMX_MAX_PCR_TRACK\n");
                break;
            }
            prMediaInfo->rPlayerStatus.au4PcrId[i] = 
                (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].au2PcrId[i];
        }
    }
    
    prMediaInfo->rRangeInfo[0].t_pts_offset = _rPlayerMpgInfo.rPgm[u4Pgm].u4FirstPts;
    prMediaInfo->rRangeInfo[0].ui8_fileoffset = 0;
    prMediaInfo->rRangeInfo[0].z_range_sz = (SIZE_T)prMediaInfo->u8FileSize;
    prMediaInfo->rRangeInfo[0].ui8_pb_duration = _rPlayerMpgInfo.rPgm[u4Pgm].u4TotalTime;
    prMediaInfo->rPlayerStatus.u4TotalDuration = (UINT32)_rPlayerMpgInfo.rPgm[u4Pgm].u4TotalTime;    
    if(!SWDMX_SetInfo((UINT8)eSWDMX_TOTAL_DURATION,
        _rPlayerMpgInfo.rPgm[u4Pgm].u4TotalTime, 0, 0))
    {
        LOG(1, "SWDMX_SetInfo eSWDMX_TOTAL_DURATION fail\n");
    }
        
    if((!prMediaInfo->u2VidStmNum) && (!prMediaInfo->u2AudStmNum))
    {
        LOG(1, "_PLAYMGR_GetMpgMediaInfo no A/V\n");
        return FALSE;
    }
    
    return TRUE;
}

