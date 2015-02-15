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
 * $RCSfile: swdmx_mp4.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_mp4.c
 *  Software demux for mp4 - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "swdmx_mp4.h"
#include "vdec_drvif.h"
#include "u_midxbuld.h"
#include "swdmx_debug.h"
#include "aud_drvif.h"
#include "gcpu_if.h"

#if defined(CC_MT5387) || defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
#include "b2r_drvif.h"
#include "b2r_if.h"
#endif

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT) && !defined(CC_MT5890)
#include "tz_if.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define AAC_FAKE_HDR_SIZE 7
#define AAC_ALLOC_SIZE   (256)

#define MIN_READ_SIZE     (512* 1024)
#define MIN_READ_SIZE_NO_VIDEO (192*1024)
#define MP4_BUFFER_SIZE (235 * 7 * 1024)
#define MP4_KEY_TABLE_THRESHOLD (3)
#define MP4_ONE_SEC_DURATION (90000 * 2)
#define FPS_PRECISION (1000)

#ifdef LINUX_TURNKEY_SOLUTION
#define SWDMX_MP4_VIDEO_HEADER_ALLOC_SIZE (512)
#endif

#define H264_STARTCODE_LEN (3)

#define M4A_RANGE_DIV_COUNT (80)

#define MP4_SEQHEADER_FEEDER_SIZE      (512)

#define HAN_MVC_HEADER
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#undef min
#define min(x, y) ((x < y) ? x : y)
#undef max
#define max(x, y) ((x > y) ? x : y)
#undef abs
#define abs(x)                  (((x) >= 0) ? (x) : -(x))

#define SWDMX_MP4_VERIFY_NULL(ptr)                                    \
          do {                                              \
            if (ptr == NULL) { \
                LOG(3, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return; }      \
          } while (0)

#define SWDMX_MP4_VERIFY_NULL_ERROR(ptr)                                    \
          do {                                              \
            if (ptr == NULL) { \
                LOG(3, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return FALSE; }      \
          } while (0)


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgTestMarlin = FALSE;

#if 0
static SWDMX_MP4_INFO_T prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

static FEEDER_BUF_INFO_T _rFeederInfo;
static UCHAR *_pucAudBuf = 0;

static MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T _rPrevSample = {0};
#ifdef LINUX_TURNKEY_SOLUTION
static UCHAR* _pucMp4VidHeaderPtr = 0;
#endif
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4RequestData(SWDMX_INFO_T* prSwdmxInst);

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4RequestDataByTime(SWDMX_INFO_T* prSwdmxInst);

static SWDMX_HDR_PRS_STATE_T  _SWDMX_BeforeDeliverUseIdx(SWDMX_INFO_T* prSwdmxInst, UINT8 u1HdrType);

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrsUseIdx(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    SWDMX_RANGE_LIST_T* prRangeList);

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4DataUseIdx(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType);

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrsDataType(
    SWDMX_INFO_T* prSwdmxInst,
    UINT64 u8Offset,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType);

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4Audioframe(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType,
    SWDMX_RANGE_LIST_T* prRangeList);

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4Keyframe(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType,
    SWDMX_RANGE_LIST_T* prRangeList);
static BOOL _SWDMX_InitDmx(
    SWDMX_INFO_T* prSwdmxInst,
    UINT8 u1HdrType,
    VOID* pfnCbFunc,
    BOOL fgEos);

static BOOL _SWDMX_SendMp4Eos(SWDMX_INFO_T* prSwdmxInst,
                              UINT8 u1HdrType);

static UINT32 _SWDMX_Mp4Seek(SWDMX_INFO_T* prSwdmxInst,
                             UINT64 u8SeekTime,
                             SWDMX_RANGE_LIST_T* prRangeList,
                             INT32 i4PlaySpeed);
static UINT32 _SWDMX_Mp4SeekCurPos(SWDMX_INFO_T* prSwdmxInst);

static BOOL _SWDMX_Mp4GetDataFromFeeder(SWDMX_INFO_T* prSwdmxInst,
                                        FEEDER_REQ_DATA_T *prFeederData);

static BOOL _SWDMX_Mp4GetDataFromFeederNoWait(SWDMX_INFO_T* prSwdmxInst,
        FEEDER_REQ_DATA_T *prFeederData);

static BOOL _SWDMX_Mp4CalculateRepeatFrame(SWDMX_INFO_T* prSwdmxInst, INT32 i4PlaySpeed);
static BOOL _SWDMX_Mp4CalculateAudioFrame(SWDMX_INFO_T* prSwdmxInst, INT32 i4PlaySpeed);

static BOOL _SWDMX_Mp4ResetFeeder(SWDMX_INFO_T* prSwdmxInst);

static void _SWDMX_Mp4SetFeederInvalid(SWDMX_INFO_T* prSwdmxInst, BOOL fgFlag);

static void _SWDMX_Mp4VideoWakeUp(SWDMX_INFO_T* prSwdmxInst);

static BOOL _SWDMX_Mp4CheckNextKeyTable(SWDMX_INFO_T* prSwdmxInst,INT32 i4Speed,UINT32* pu4CurIdx);

BOOL _SWDMX_Mp4SetDecoderInfo(UINT8 u1SrcId,
                              UINT8 u1Type,
                              UINT32 u4Para1,
                              UINT32 u4Para2,
                              UINT32 u4Para3)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    if(u1Type == eSWDMX_SET_VIDEOTYPE)
    {
#if defined(CC_MP4_MPEG1_DISABLE) && defined(CC_MP4_MPEG2_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG1_MPEG2)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_MP4_MPEG4_DISABLE)
            if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG4)
            {
                return FALSE;
            }
            else
#endif
#if defined(CC_MP4_DX311_DISABLE)
                if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_DX311)
                {
                    return FALSE;
                }
                else
#endif
#if defined(CC_MP4_H264_DISABLE)
                    if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_H264)
                    {
                        return FALSE;
                    }
                    else
#endif
#if defined(CC_MP4_VC1_DISABLE)
                        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_VC1)
                        {
                            return FALSE;
                        }
                        else
#endif
#if defined(CC_MP4_MJPEG_DISABLE)
                            if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
                            {
                                return FALSE;
                            }
                            else
#endif
                            {
                                prSwdmxMp4Info->u4VDecType =  u4Para1;
                                return TRUE;
                            }
    }
    else if (u1Type == eSWDMX_SET_VIDSEQHEADER)
    {
#ifndef LINUX_TURNKEY_SOLUTION
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr = u4Para1;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength = u4Para2;
#else
        UINT32 ui4_uuidPtr = 0;
        UINT8* pucUuidBuf = NULL;
        UINT32 ui4_uuidLength = 0;
        UINT32 ui4_uuid_box = 0;
        UINT32 ui4_3dds_type = 0;
        UINT32 u4Align = 16;

        /*parse 3dds info*/
        ui4_uuidLength = u4Para2;
        _SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &ui4_uuidPtr, &u4Para2, &u4Align);

        x_memcpy((VOID*)VIRTUAL(ui4_uuidPtr), (VOID*)VIRTUAL(u4Para1), ui4_uuidLength);

        pucUuidBuf = (UINT8*)VIRTUAL(ui4_uuidPtr);

        if (pucUuidBuf && ui4_uuidLength>=4)
        {
            ui4_uuid_box = (pucUuidBuf[0]<<24) | (pucUuidBuf[1]<<16) | (pucUuidBuf[2]<<8) | pucUuidBuf[3];
            LOG(0, "Parse UUID 0x75756964 = 0x%08x\n", ui4_uuid_box);

            if( ui4_uuid_box == 0x75756964) //0x75756964 == "uuid"
            {
                if (ui4_uuidLength >= 24)
                {
                    ui4_3dds_type = (pucUuidBuf[4]<<24) | (pucUuidBuf[5]<<16) | (pucUuidBuf[6]<<8) | pucUuidBuf[7];
                    if (ui4_3dds_type == 0x33444453) //0x33444453 "3DDS"
                    {
                        UINT16 u23DType = ((pucUuidBuf[20] << 8) | pucUuidBuf[21]);

                        if((u23DType&0xC000) == 0x8000)
                        {
                            u23DType = (u23DType & 0x3F80) >> 7;

                            switch(u23DType)
                            {
                                case 3:
                                    prSwdmxMp4Info->u4Mp4Src3DType = MP4_SRC_3D_SBS_HALF;
                                    break;

                                case 4:
                                    prSwdmxMp4Info->u4Mp4Src3DType = MP4_SRC_3D_TAB;
                                    break;

                                case 5:
                                    prSwdmxMp4Info->u4Mp4Src3DType = MP4_SRC_3D_FRM_ALT;
                                    break;

                                default:

                                    break;
                            }
                        }
                    }
                }

                return TRUE;
            }
            else
            {
                //maybe avvc info, do nothing , go as normal flow
            }
        }

		{
			UINT32 u4Align = 16;
            UINT32 u4Size = 512;
            UINT32 i = 0;
            UINT32 u4RequestSize = MP4_SEQHEADER_FEEDER_SIZE; //same as u4Size
            
            LOG(0,"Set decoder info, eSWDMX_SET_VIDSEQHEADER\n");

            if (u4Para2 > u4Size)
            {
                LOG(0,"=====ERROR: seq header size > %u, not support\n", u4Size);
                return TRUE;
            }
			
		    while (i < MP4_MAX_SEQ_HEADER)
	        {
	            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVidSeqHeaderIsUsed[i] == FALSE)
	            {
	                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[i] == 0)
	                {
	                    _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[i], &u4Size, &u4Align);
	                    LOG(1, "==Get seq header %u buffer, size = %u\n", i, u4Size);
	                }
	                
	                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderLength[i] = u4Para2;
	                LOG(1, "==The u4VidHdrLength %u is %d==\n", i, (int)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderLength[i]);
	               
	                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[i])
	                {
	                    x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[i]), 
	                            (VOID*)VIRTUAL(u4Para1), 
	                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderLength[i]);

	                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVidSeqHeaderIsUsed[i] = TRUE;
	                }
	                else
	                {
	                    LOG(0,"Header Seq Buffer alloc fail\n");
	                }

	                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr == 0)
	                {
	                	LOG(0, "First Video Seq Header.\n");
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[i];
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderLength[i];
					}
                    if (0 == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderFdrPtr)
                    {
                        LOG(0,"===The first play set Video Sequnce Header Feeder Buffer===\n");
                        _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderFdrPtr, &u4RequestSize, &u4Align);
                    }
	                break;
	            }     

	            i++;
	       	}

			return TRUE;
       	}
	   
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr == 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength = u4Para2;
            _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr, &u4Para2, &u4Align);

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr)
            {
                x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr),
                         (VOID*)VIRTUAL(u4Para1),
                         prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength);
            }
            else
            {
                LOG(0,"Header Seq Buffer alloc fail\n");
            }
        }
#ifdef HAN_MVC_HEADER
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr == 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrSecLength = u4Para2;
            _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr, &u4Para2, &u4Align);

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr)
            {
                x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr),
                         (VOID*)VIRTUAL(u4Para1),
                         prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrSecLength);
            }
            else
            {
                LOG(0,"Header Seq Buffer alloc fail\n");
            }
        }
#endif
#endif
        return TRUE;
    }
    else if (u1Type == eSWDMX_SET_AUDIOTYPE)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4ADecType = u4Para1;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudSampleRate = u4Para2;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudChannelNs = u4Para3;
        return TRUE;
    }
    else if (u1Type == eSWDMX_SET_VIDEO_DECRYPT)
    {
        SWDMX_TRACK_CRYPT_INFO_T *pTrackInfo = (SWDMX_TRACK_CRYPT_INFO_T *)u4Para1;

        if (pTrackInfo)
        {
            prSwdmxMp4Info->eCryptMode = pTrackInfo->eCryptMode;
            prSwdmxMp4Info->eDRMType = pTrackInfo->eDRMType;
            ASSERT(pTrackInfo->u4KeyLength <= 16);
            x_memcpy(prSwdmxMp4Info->u1VideoDrmKey, (void*)pTrackInfo->pu1Key, 16);
            LOG(3, "Get DRM type %d mode %d Vkey length %d\n",pTrackInfo->eDRMType, pTrackInfo->eCryptMode, pTrackInfo->u4KeyLength);
        }
        return TRUE;
    }
    else if (u1Type == eSWDMX_SET_AUDIO_DECRYPT)
    {
        SWDMX_TRACK_CRYPT_INFO_T *pTrackInfo = (SWDMX_TRACK_CRYPT_INFO_T *)u4Para1;
        if (prSwdmxMp4Info->u1AKeyCount < MP4_MAX_AUDIO_TRACK && pTrackInfo)
        {
            UCHAR ucTrackId;
            BOOL bUpdateKey = FALSE;
            for (ucTrackId=0; ucTrackId < prSwdmxMp4Info->u1AKeyCount; ucTrackId++)
            {
                if (prSwdmxMp4Info->u1AudioKeyTrackId[ucTrackId] == pTrackInfo->u4TrackId)
                {
                    x_memcpy(prSwdmxMp4Info->u1AudioDrmKey[ucTrackId], (void*)pTrackInfo->pu1Key, 16);
                    bUpdateKey = TRUE;
                    break;
                }
            }
            if (!bUpdateKey)
            {
                x_memcpy(prSwdmxMp4Info->u1AudioDrmKey[ucTrackId], (void*)pTrackInfo->pu1Key, 16);
                prSwdmxMp4Info->u1AudioKeyTrackId[prSwdmxMp4Info->u1AKeyCount] = pTrackInfo->u4TrackId;
                prSwdmxMp4Info->u1AKeyCount++;
            }
            LOG(3, "Get DRM type %d mode %d Akey length %d TrackID %d\n",pTrackInfo->eDRMType, pTrackInfo->eCryptMode, pTrackInfo->u4KeyLength, pTrackInfo->u4TrackId);
        }
        else
        {
            ASSERT(0);
            LOG(0, "Audio key count exceeds maximum track count(%d)!\n", MP4_MAX_AUDIO_TRACK);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL _SWDMX_Mp4GetInfo(UINT8 u1SrcId,
                       UINT8 u1Type,
                       UINT32* pu4Para1,
                       UINT32* pu4Para2,
                       UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    BOOL fgRet = FALSE;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(8, "_SWDMX_Mp4GetInfo u1Type = %d source id = %d\n", u1Type, u1SrcId);

    if(u1Type == eSWDMX_GET_CURRENT_POSITION)
    {
        if(pu4Para1 == NULL)
        {
            LOG(1, "_SWDMX_Mp4GetInfo pu4Para1 = NULL\n");
            fgRet = FALSE;
            return fgRet;
        }
        pu8Tmp = (UINT64*)pu4Para1;
        *pu8Tmp = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos;
        fgRet = TRUE;
    }
    else if (u1Type == eSWDMX_GET_IS_TRICK)
    {
        if(prSwdmxInst->eCurFMT == SWDMX_FMT_M4A)
        {
             if(pu4Para1)
             {
                 *pu4Para1 = TRUE;
             }
             if(pu4Para2)
             {
                 *pu4Para2 = TRUE;
             }
        }
        else if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl != NULL)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry < MP4_KEY_TABLE_THRESHOLD)
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = FALSE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = FALSE;
                    }
                }
                else
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = TRUE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = TRUE;
                    }
                }
            }
            else
            {
                if(pu4Para1)
                {
                    *pu4Para1 = FALSE;
                }
                if(pu4Para2)
                {
                    *pu4Para2 = FALSE;
                }
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
        {
            if(pu4Para1)
            {
                *pu4Para1 = TRUE;
            }
            if(pu4Para2)
            {
                *pu4Para2 = TRUE;
            }
        }
        fgRet = TRUE;
    }
    else if(u1Type == eSWDMX_GET_AUDIO_ACTIVE_PIDX)
    {
        UINT32 u4AudFifoAddr, u4AudFifoSize,u4DmxAvailSize;
        _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);
        u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                              (DMX_PID_TYPE_T)DMX_PID_TYPE_ES_AUDIO,
                                              prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);

        *pu4Para1 = u4AudFifoSize - u4DmxAvailSize;
        fgRet = TRUE;
    }

    return fgRet;
}

BOOL _SWDMX_Mp4VideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UINT32 u4Addr;
    UINT64 u8DataSize;
    UINT64 u8Numerator = 0, u8Denominator = 0, u8Remainder = 0;

    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    //rPesInfo.u4PTS = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastVidPts;
    rPesInfo.u4PTS = prPes->u4Pts;
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgGop = prPes->fgGop;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.ucEsId = prSwdmxInst->u1VdecId;
    rPesInfo.u8Offset =
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff;

    if(prSwdmxMp4Info->u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucMpvId = VLD1; // video format all use VLD expect jpeg
    }
    else
    {
        rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    }
    // Pts is valid all the time
    rPesInfo.fgDtsValid = TRUE;
#ifdef CC_VDEC_FMT_DETECT
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;
#endif

    LOG(11, " prPes->u4Pts = %x\n",  prPes->u4Pts );

    if (rPesInfo.fgEos)
    {
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0)
        {
            rPesInfo.fgBackward = TRUE;
        }
        rPesInfo.u4FifoStart = 0;
        rPesInfo.u4FifoEnd = 0;
        rPesInfo.u4VldReadPtr = 0;
    }

    if (prSwdmxInst->eVideoType == ENUM_SWDMX_VDEC_H265)
    {
        rPesInfo.u4VideoType = (UINT32)DMX_VIDEO_H265;
        rPesInfo.ucPicType = (UCHAR)prPes->u4FrameType;

        LOG(3, "rPesInfo.ucPicType: 0x%02x\n", rPesInfo.ucPicType);

        u4Addr = prPes->u4FrameAddr + 3;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;

        switch (prPes->u4FrameType)
        {
            case H265_SLICE_BLA_W_LP:
            case H265_SLICE_BLA_W_RADL:
            case H265_SLICE_BLA_N_LP:
            case H265_SLICE_IDR_W_RADL:
            case H265_SLICE_IDR_N_LP:
            case H265_SLICE_TRAIL_CRA:
                LOG(2, "Swdmx Got Iframe\n");

                break;

            /* case H265_PIC_TYPE_P:
            u4SwdmxPicType = SWDMX_PIC_TYPE_P;
            break;*/

            /*case PIC_TYPE_B:
            break;*/

            default:
                break;
        }

    }
    else if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264) &&
            !rPesInfo.fgEos)
    {
        u4Addr = (UINT32)prPes->au1PicInfo;
        rPesInfo.ucPicType = *(UINT8*)(u4Addr);

        u4Addr = prPes->u4FrameAddr + 4;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;

        if(( rPesInfo.ucPicType & 0x1f) == 5)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = rPesInfo.u4PTS;
        }

        LOG(8, "_SWDMX_Mp4VideoCallback: ucPicType = 0x%x\n", rPesInfo.ucPicType);

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts == 0xFFFFFFFF)
        {
            VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, TRUE, prSwdmxInst->u8FileSize);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0x0;

            LOG(0, "u4LastIPts not find!!!!!!! ucPicType: 0x%x\n", rPesInfo.ucPicType);
        }
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_MPEG4) &&
             !rPesInfo.fgEos)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)prPes->au1PicInfo;
        pucBuf = (UCHAR*)(u4Addr);

        u8DataSize = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz -prSwdmxInst->rFmtInfo.rSwdmxMp4Info.ui4_data_offset;
        u8Numerator = u8DataSize*DEMUX_MP4_AV_PERIOD;
        u8Denominator = (UINT64)prSwdmxInst->u4TotalTime;
        if(u8Denominator != 0)
        {
            rPesInfo.u4BitRate = u8Div6432(u8Numerator, u8Denominator,&u8Remainder);
            rPesInfo.u4BitRate = rPesInfo.u4BitRate*8;
        }

        if (pucBuf[0] == 0xB6)
        {
            u4Addr = (UINT32)(prPes->au1PicInfo + 1);
            pucBuf = (UCHAR*)VIRTUAL(u4Addr);

            switch (pucBuf[0] >> 6)
            {
                case 0:
                    rPesInfo.ucPicType  = I_TYPE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = rPesInfo.u4PTS;
                    break;
                case 1: case 3:
                    rPesInfo.ucPicType  = P_TYPE;
                    break;
                case 2:
                    rPesInfo.ucPicType = B_TYPE;
                    break;
                default:
                    rPesInfo.ucPicType = UNKNOWN_TYPE;
                    break;
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts == 0xFFFFFFFF)
            {
                VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, TRUE, prSwdmxInst->u8FileSize);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0x0;

                LOG(0, "u4LastIPts not find!!!!!!! ucPicType: 0x%x\n", rPesInfo.ucPicType);
            }
        }
        else if ((pucBuf[0] >= 0x20) &&
                 (pucBuf[0] <= 0x2f))
        {
            rPesInfo.fgSeqHdr = TRUE;
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucPicType = I_TYPE;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = rPesInfo.u4PTS;
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == (UINT32)ENUM_SWDMX_VDEC_H263)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)(prPes->au1PicInfo + 1);
        pucBuf = (UCHAR*)(u4Addr);

        switch (pucBuf[0] & 0x02)
        {
            case 0:
                rPesInfo.ucPicType = I_TYPE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = rPesInfo.u4PTS;
                break;
            case 2:
                rPesInfo.ucPicType = P_TYPE;
                break;
            default:
                rPesInfo.ucPicType = UNKNOWN_TYPE;
                break;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts == 0xFFFFFFFF)
        {
            VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, TRUE, prSwdmxInst->u8FileSize);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0x0;

            LOG(0, "u4LastIPts not find!!!!!!! ucPicType: 0x%x\n", rPesInfo.ucPicType);
        }
    }

    rPesInfo.rExtra.fgValid=TRUE;
    rPesInfo.rExtra.u8PTSI = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts;
    rPesInfo.rExtra.u8AudPTS=prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts;
    x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
    VDEC_SendEs((void*)&rPesInfo);

    return TRUE;
}


BOOL _SWDMX_Mp4AudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet;
    DMX_AUDIO_PES_T rPes;

    SWDMX_INFO_T *prSwdmxInst = NULL;
    if(prPes != NULL)
    {
        prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
        SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);
    }
    else
    {
        LOG(1, "_SWDMX_Mp4AudioCallback prPes NULL\n");
        return FALSE;
    }

    x_memset(&rPes, 0, sizeof(rPes));

    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed > 0)
    {
        rPes.fgForward = TRUE;
    }
    else
    {
        rPes.fgForward = FALSE;
    }

    fgRet = AUD_SendAudioPes(&rPes);
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSendAudPts = rPes.u4Pts;
    return fgRet;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _SWDMX_Mp4Init(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    // Initialize feeder
    //FeederOpen(FEEDER_PROGRAM_SOURCE);
    SWDMX_INFO_T* prSwdmxInst = NULL;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);
    UNUSED(eType);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);
    //*********************************************************
    //FeederSetBufferSize(FEEDER_PROGRAM_SOURCE, 5*10*1024);
    //FeederStart(FEEDER_PROGRAM_SOURCE);
#if 0
    FeederInstallCallback(prSwdmxInst->eFeederSrc, FEEDER_NOTIFY_DATA_REQ_DONE,
                          _SWDMX_FeederCallBack, (UINT32)&prSwdmxInst->rFeederInfo);
#endif
    //*********************************************************

#if 0
    //DMX_MM_DECODER_CALLBACKS_T rCallback; // Isaac: should be DMX_DECODER_CALLBACKS_T, Jack fix me
    DMX_DECODER_CALLBACKS_T rCallback;

    rCallback.pfnSendAudioPes = AUD_SendAudioPes;
    rCallback.pfnSendVideoPes = _SWDMX_Mp4VideoCallback;
    rCallback.pfnUpdateVideoWp = VDEC_SetWptr;
    DMX_SetDecoderCallbacks(&rCallback);
#endif

    // Default some values
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4PrevAStrmID = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AStrmID = 0xFFFFFFFF;

    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudSampleRate = 48000;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudChannelNs = 2;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts <<= 32;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts += 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts <<= 32;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts += 0xFFFFFFFF;

    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid = 0xFF;

    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample, 0, sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8PrevSampleOffset = 0;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSpeedChgforABRepeat = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4Mp4Src3DType = MP4_SRC_NORM_SOURCE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsneedQueryVdecInfo = TRUE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgGotEos = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNotFirstFrame = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRecSpeedChangeFlag = FALSE;

    //DDI_BUF_ALIGNMENT == 16 and
    //for _dmx_mm_movedata  Meet the alignment and packet size (188 bytes) requirements.
    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4ADecType == ENUM_SWDMX_ADEC_AAC &&
            prSwdmxInst->pucAudBuf == 0)
    {
#if 0
        prSwdmxInst->pucAudBuf =
            (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(AAC_ALLOC_SIZE,16));
#else
        UINT32 u4RequestSize = AAC_ALLOC_SIZE;
        UINT32 u4Align = 16;
        _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, (UINT32*)prSwdmxInst->pucAudBuf, &u4RequestSize, &u4Align);
#endif
        if(prSwdmxInst->pucAudBuf == 0)
        {
            LOG(1, "Allocate AAC buffer failed!\n");
            return FALSE;
        }
    }

    DMX_SetToDecoder(TRUE);

    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudStrmDisable = FALSE;

    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest = TRUE;
#if 0
    if(_fgTestMarlin)
    {
        UINT8 V_Key[] =  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
        UINT8 A_Key[] =  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType = eSWDMX_DRM_TYPE_PDCF;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eCryptMode = eSWDMX_CRYPT_MODE_AES_CTR;
        x_memcpy(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VideoDrmKey, V_Key, sizeof(V_Key));
        x_memcpy(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudioDrmKey[0], A_Key, sizeof(A_Key));
    }
#endif
    return TRUE;
}


/**
*/
VOID _SWDMX_Mp4SetRange(UINT8 u1SrcId,
                        SWDMX_RANGE_LIST_T *prRangeList,
                        UINT32 u4VidStrmID,
                        UINT32 u4AudStrmID,
                        UINT32 u4SubTitleID)
{
    UINT8 u1ATrack;
    UINT8 u1SubTrack;
    UINT8 u1AudEncCount;
    UINT8 ui1_i = 0;
	
    SWDMX_INFO_T* prSwdmxInst = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pBuldIdxTbl = NULL;
    MM_RANGE_ELMT_ENCTBL_T *pEncTbl = NULL;
    u1ATrack = 0;
    u1SubTrack = 0;
    u1AudEncCount = 0;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MP4_VERIFY_NULL(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);
    if (prRangeList != NULL &&
            prRangeList->prDmxRangeInfo != NULL)
    {
        /*first set the tbl null*/
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = NULL;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl = NULL;
        for (ui1_i = 0; ui1_i < MP4_MAX_AUDIO_TRACK; ui1_i++)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[ui1_i] = NULL;
        }
        for (ui1_i = 0; ui1_i < MP4_MAX_SUB_TRACK; ui1_i++)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[ui1_i] = NULL;
        }

        pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)prRangeList->prDmxRangeInfo->pt_idxtbl_lst;
        pEncTbl = (MM_RANGE_ELMT_ENCTBL_T*)prRangeList->prDmxRangeInfo->pt_enctbl_lst;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo = prRangeList->prDmxRangeInfo;

        while (pBuldIdxTbl != NULL)
        {
            if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_AUDIO &&
                    u1ATrack < MP4_MAX_AUDIO_TRACK)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[u1ATrack] = pBuldIdxTbl;
                LOG(5, "_SWDMX_Mp4SetRange Audio Index table [%u] = 0x%08x\n", u1ATrack,
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[u1ATrack]));
                u1ATrack++;
            }
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_VIDEO)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = pBuldIdxTbl;
				prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurMutliDescInfoIdx = 0;
				prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NextSeqChgVidIdx = pBuldIdxTbl->at_range_desc_entry_chg[0].ui4_sample_idx;
#if 0
				for (ui1_i = 0;ui1_i < 20;ui1_i ++)
				{
					LOG(0,"The %u video talbe info,the descyption_id is %u,the sample_idx is %u\n",
						ui1_i,
						pBuldIdxTbl->at_range_desc_entry_chg[ui1_i].ui1_sample_desc_id,
						pBuldIdxTbl->at_range_desc_entry_chg[ui1_i].ui4_sample_idx);
				}
#endif
				LOG(5, "_SWDMX_Mp4SetRange Video Index table = 0x%08x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl));
            }
            else if ((pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_SP)&&
                     (u1SubTrack < MP4_MAX_SUB_TRACK))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[u1SubTrack] = pBuldIdxTbl;
                LOG(5, "_SWDMX_Mp4SetRange subtile Index table = 0x%08x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[u1SubTrack]));
                u1SubTrack++;
            }
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY)
            {
                MM_RANGE_ELMT_IDXTBL_T *prTmpVidKeyIdxTbl = pBuldIdxTbl;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurKeyTableIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumOfKeyTable = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl = pBuldIdxTbl;
#if 0				
				for (ui1_i = 0;ui1_i < 20;ui1_i ++)
				{
					LOG(0,"The %u key table info,the descyption_id is %u,the sample_idx is %u\n",
						ui1_i,
						pBuldIdxTbl->at_range_desc_entry_chg[ui1_i].ui1_sample_desc_id,
						pBuldIdxTbl->at_range_desc_entry_chg[ui1_i].ui4_sample_idx);
				}
#endif
                while(prTmpVidKeyIdxTbl != NULL)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.parVidKeyIdxTblArray[(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumOfKeyTable)++] = prTmpVidKeyIdxTbl;
                    prTmpVidKeyIdxTbl = prTmpVidKeyIdxTbl->pt_next_partial;
                }

                LOG(5, "_SWDMX_Mp4SetRange Key Index table = 0x%08x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl));
            }

            pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)pBuldIdxTbl->pv_next_tbl;

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.ui4_data_offset = prRangeList->prDmxRangeInfo->ui8_fileoffset;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz  = prRangeList->prDmxRangeInfo->z_range_sz;
        }

        while (pEncTbl != NULL)
        {
            if (pEncTbl->e_elem_type == MM_ELEMT_TYPE_AUDIO &&
                    u1AudEncCount < MP4_MAX_AUDIO_TRACK)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudEncTbl[u1AudEncCount] = pEncTbl;
                LOG(5, "_SWDMX_Mp4SetRange Audio cenc table [%u] = 0x%08x\n", u1AudEncCount,
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudEncTbl[u1AudEncCount]));
                u1AudEncCount++;
            }
            else if (pEncTbl->e_elem_type == MM_ELEMT_TYPE_VIDEO)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidEncTbl = pEncTbl;
                LOG(5, "_SWDMX_Mp4SetRange Video cenc table = 0x%08x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidEncTbl));
            }

            pEncTbl = (MM_RANGE_ELMT_ENCTBL_T*)pEncTbl->pv_next_tbl;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
        {
            // The upper layer (mw or playmgr should prevent 0 case for rate and scale)
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_scale)
            {
                VDEC_HDR_INFO_T rHdrInfo;

                if(!VDEC_QueryInfo(prSwdmxInst->u1VdecId,&rHdrInfo))
                {
                    LOG(2, "%s %d VDEC_QueryInfo failed!!!\n",__FUNCTION__,__LINE__); 
                    rHdrInfo.fgProgressiveSeq = TRUE;
                }
                
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps =
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_rate/prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_scale);

                if (!rHdrInfo.fgProgressiveSeq)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps = ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps*10)/2)/10;
                }
                LOG (2, "%s %d, fgProgressiveSeq is %d, FPS is %u\n",
                    __FUNCTION__,__LINE__,rHdrInfo.fgProgressiveSeq,prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps);            
            }

            // Add some error handling
            if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps ||
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps > 60)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps = 30;
            }

            LOG(5, "prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps = %ld\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps);
        }

        // Set the number of total audio tracks
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack = u1ATrack;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalSubTrack = u1SubTrack;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = FALSE;
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
        if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack > 0 &&
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack &&
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack] != NULL )
        {
            if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0 )
            {
                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL)
                        && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry > 0))
                {
                    prRangeList->u4CurRangeAudIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1;
                }
                else
                {
                    LOG(0,"Index number is 0!\n");
                    prRangeList->u4CurRangeAudIdx = 0;
                }
            }
            else
            {
                prRangeList->u4CurRangeAudIdx = 0;
            }

            LOG(1, "i4PrevSpeed = %d, u4CurRangeAudIdx = %u\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed, prRangeList->u4CurRangeAudIdx );
        }

        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime = prRangeList->prDmxRangeInfo->ui8_pb_duration;
    }
    else
    {
        LOG(1, "Demuxer Range is NULL\n");
    }
}


/**
*/
SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrs(
    UINT8 u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    SWDMX_CUR_CMD_T* prCmd)
{
    INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
    UINT64 u8SeekPos = prCmd->u8SeekPos;
    BOOL fgSpeedChange = prCmd->fgChange;
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl = NULL;
    INT32  i;

    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
    
    /*If it's the beginning playback.*/
    if ((!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBeginToPlay) &&
            (i4PlaySpeed == 1000))
    {
#if 1
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl))
        {
#define _LOOP_CNT_    2000
            UINT32 u4Cnt = 0;
            while (u4Cnt < _LOOP_CNT_)
            {
                UINT32 u4Status = MPV_GetDecMode(prSwdmxInst->u1VdecId);
                if ((u4Status == VDEC_ST_STOP) || (u4Status == VDEC_ST_IDLE))
                {
                    x_thread_delay(10);
                    u4Cnt++;
                }
                else
                {
                    break;
                }
            }
        }
#endif
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info!=0)
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
            {
                START_POS_INFO_T *prStartPosInfo=&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos;
                fgSpeedChange=TRUE;
                //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info;
                //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                prCmd->fgSeek=TRUE;
                prCmd->u4SeekMode = SWDMX_CUR_CMD_SEEK_OFFSET;
                prCmd->rSeekOffset.ui8_i_pts_info=prStartPosInfo->ui8_pts_info;
                prCmd->rSeekOffset.ui8_pts_info=prStartPosInfo->ui8_pts_info;
                prCmd->rSeekOffset.ui8_i_frame_position=prStartPosInfo->ui8_i_frame_position;
                prCmd->rSeekOffset.ui8_frame_position=prStartPosInfo->ui8_frame_position;
                //prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                //return eSWDMX_HDR_PRS_SUCCEED;

            }
            // Video only
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl
                     && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL))
            {
                _SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);

                LOG(5, "Video Start Render Pts = %lld\n",
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info);
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info);

                // set stc for starting playback
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info - 45000);

                VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                VDEC_Pause(prSwdmxInst->u1VdecId);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
                if (1)
                {
                    UINT32 u4AudPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info;
                    AUD_SetStartPts(prSwdmxInst->u1AdecId,u4AudPts);
                    //VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts = u4AudPts;
                    prRangeList->u4CurPTS = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_i_pts_info;
                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBeginToPlay = TRUE;

                    return eSWDMX_HDR_PRS_SUCCEED;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->t_start_pos.ui8_pts_info;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                }
            }
            // Audio only
            else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL) )
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                if (prRangeList->u4CurRangeAudIdx <= (prIdxTbl->ui4_number_of_entry - 1))
                {
                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                }
            }
            else
            {
                LOG(1, "SWDMX_Mp4 Unknow state for starting playback\n");
                return eSWDMX_HDR_PRS_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBeginToPlay = TRUE;
        }
        else if (prRangeList->fgSetRenderPosition)
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
            {
                _SWDMX_BeforeDeliverUseIdx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                UNUSED(_SWDMX_Mp4Seek(prSwdmxInst, prRangeList->u8StartPts, prRangeList, i4PlaySpeed));
                LOG(5, "Video Start Render Pts = %lld\n",
                    prRangeList->u8StartPts);

                if (prRangeList->fgIsSeekTarget)
                {
                    prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prRangeList->u8StartPts);

                    // set stc for starting playback
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId,prRangeList->u8StartPts - 45000);
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)
                    {
                        prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                    }
                    if (prRangeList->u4CurRangeAudIdx <= (prIdxTbl->ui4_number_of_entry - 1))
                    {
                        AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                            prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;
                    }
                    //VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);


                    VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                    VDEC_Pause(prSwdmxInst->u1VdecId);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = prRangeList->u8StartPts;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                }
            }
            // Video only
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl
                     && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL) )
            {
                _SWDMX_BeforeDeliverUseIdx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                UNUSED(_SWDMX_Mp4Seek(prSwdmxInst, prRangeList->u8StartPts, prRangeList, i4PlaySpeed));
                if(prRangeList->fgIsSeekTarget)
                {
                    prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prRangeList->u8StartPts);

                    // set stc for starting playback
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_pts - 45000);

                    VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                    VDEC_Pause(prSwdmxInst->u1VdecId);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = prRangeList->u8StartPts;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                }
            }
            // Audio only
            else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                if (prRangeList->u4CurRangeAudIdx <= (prIdxTbl->ui4_number_of_entry - 1))
                {
                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                }
            }
            else
            {
                LOG(1, "SWDMX_Mp4 Unknow state for starting playback\n");
                return eSWDMX_HDR_PRS_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBeginToPlay = TRUE;
        }
        else
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                    && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_pts);

                // set stc for starting playback
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_pts - 45000);

                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                if (prRangeList->u4CurRangeAudIdx <= (prIdxTbl->ui4_number_of_entry - 1))
                {
                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                    //VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                        prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;
                }
                VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                VDEC_Pause(prSwdmxInst->u1VdecId);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
            }
            // Video only
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl
                     && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry != NULL))
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_pts);

                // set stc for starting playback
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_pts - 45000);

                VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                VDEC_Pause(prSwdmxInst->u1VdecId);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
            }
            // Audio only
            else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
            {
                if ( prRangeList->u4CurRangeAudIdx <= (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1))
                {
                    prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                }
            }
            else
            {
                LOG(1, "SWDMX_Mp4 Unknow state for starting playback\n");
                return eSWDMX_HDR_PRS_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBeginToPlay = TRUE;
        }

        /*Choose correct audio track to begin to play. */
        if ( (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]))
        {
            BOOL fgFindATrack = FALSE;
            UINT8 i = 0;

            fgFindATrack = FALSE;
            for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] &&
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i]->t_strm_id.u.t_stm_mp4_id == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AStrmID)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack = i;
                    LOG(3, "Begin to Play u1CurATrack = %u\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack);
                    fgFindATrack = TRUE;
                    break;
                }
            }

            if (!fgFindATrack)
            {
                LOG(3, "_SWDMX_Mp4EnableStrm didn't find audio stream\n");
                return eSWDMX_HDR_PRS_EMPTY_RANGE;
            }
        }

        if (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = 0;
        }
    }

    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange)
    {
        LOG(1, "!!!!!!!!!!!!!!!fgWaitNewRange\n");
        return eSWDMX_HDR_PRS_WAIT_NEW_RANGE;
    }

    /*Reset some flags for audio track change or speed change.*/
    /*Notice that M4A fast forward flow is the same as normal playback flow for SWDMX.*/    
    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack ||
            fgSpeedChange)
    {
        if (!(prSwdmxInst->eCurFMT == SWDMX_FMT_M4A && fgSpeedChange && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed >= 1000 && i4PlaySpeed > 1000 ))
        {
            if (( prCmd->fgSeek ||
                    (((UINT32)(prCmd->u8SeekPos >> 32) != 0xFFFFFFFF) || ((UINT32)(prCmd->u8SeekPos) != 0xFFFFFFFF))) ||
                    !( fgSpeedChange &&
                       ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed > 0 && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed <= 2000 ) &&
                       ( i4PlaySpeed > 0 && i4PlaySpeed <= 2000 ) ))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid = 0;
            }

            //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentAud = 0;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudIsReady = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
            //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = FALSE;

            if(!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
            }
        }
        else
        {
            /*For the Case as below: M4A file, Pause->Seek(here do nothing)->Fast Forward(first seek to the seekPos) */
            if(!(((UINT32)(prCmd->u8SeekPos >> 32) != 0xFFFFFFFF) || ((UINT32)(prCmd->u8SeekPos) != 0xFFFFFFFF)))
            {
                fgSpeedChange = FALSE;
            }
        }
    }
    
    /*If audio track changes.*/
    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack > 0) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack <= MP4_MAX_AUDIO_TRACK))
    {
        BOOL fgFindAudio = FALSE, fgFindATrack = FALSE;
        UINT32 u4CurPts;
        MM_RANGE_ELMT_IDXTBL_T* prIdxTbl = NULL;   //current range

        fgFindATrack = FALSE;
        for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] &&
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i]->t_strm_id.u.t_stm_mp4_id == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AStrmID)
            {
                LOG(3, "ui4_number_of_entry = %u\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i]->ui4_number_of_entry);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack = i;
                fgFindATrack = TRUE;
                break;
            }
        }
        if (!fgFindATrack)
        {
            LOG(3, "_SWDMX_Mp4EnableStrm didn't find audio stream\n");
            x_thread_delay(10);
            return eSWDMX_HDR_PRS_EMPTY_RANGE;
        }

        if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
        {
            u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
            // set audio range idx
            prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
            if(prIdxTbl)
            {
                for(i = 1; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if((prIdxTbl->u.pt_pb_idx_tbl_entry[i-1].ui4_pts <= (u4CurPts+45000)) &&
                            (prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= (u4CurPts+45000)))
                    {
                        prRangeList->u4CurRangeAudIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts;
                        LOG(5, "Cur Audio Index = %d Audio Track PTS = 0x%08x\n",
                            prRangeList->u4CurRangeAudIdx,
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                        fgFindAudio = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
                        AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                        break;
                    }
                }
                if(prIdxTbl->ui4_number_of_entry == 0)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
                }
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
            }

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange)
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

            if (!fgFindAudio)
            {
                LOG(3, "Didn't find Audio\n");
                prRangeList->u4CurPTS = u4CurPts;
                if(!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = TRUE;
                }
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        else
        {
            u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
            LOG(3, "Mp4 audio change while VDP Pts = 0x%08x\n", u4CurPts);

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)
            {
                // set audio range idx
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                if(prIdxTbl)
                {
                    for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                    {
                        if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= (u4CurPts+45000))
                        {
                            prRangeList->u4CurRangeAudIdx = i;
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts;
                            LOG(5, "Cur Audio Index = %d Audio Track PTS = 0x%08x\n",
                                prRangeList->u4CurRangeAudIdx,
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                            fgFindAudio = TRUE;
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =  prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts;
                            AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                            break;
                        }
                    }
                    if(prIdxTbl->ui4_number_of_entry == 0 || !fgFindAudio )
                    {
                        LOG(3, "Didn't find Audio, ui4_number_of_entry = %u, fgFindAudio = %u\n", prIdxTbl->ui4_number_of_entry, fgFindAudio);
                        prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
                    }
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgATrack = FALSE;
                }
            }
        }

        LOG(5, "Chang to Track %d Cur PTS = 0x%08x\n",
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack,
            u4CurPts);
        //if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
        {
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = FALSE;
        }
    }
    /*If subtitle track changes.*/
    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgSubTrack) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalSubTrack > 0) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubTitle) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalSubTrack <= MP4_MAX_SUB_TRACK))
    {

        BOOL fgFindSubTrack = FALSE;
        UINT32 u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
        for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalSubTrack) && (i < MP4_MAX_SUB_TRACK); i++)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[i] &&
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[i]->t_strm_id.u.t_stm_mp4_id == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4SubTitleID)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack = i;
                fgFindSubTrack = TRUE;
            }
        }
        if (!fgFindSubTrack)
        {
            LOG(3, "_SWDMX_Mp4EnableStrm didn't find audio stream\n");
            return eSWDMX_HDR_PRS_EMPTY_RANGE;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)
        {
            // set audio range idx
            prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack];
            for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
            {
                if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= (u4CurPts+45000))
                {
                    prRangeList->u4CurRangeSubIdx = i;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgSubTrack = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgInvalidForSub = TRUE;
                    break;
                }
            }
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgSubTrack)
            {
                prRangeList->u4CurRangeSubIdx = prIdxTbl->ui4_number_of_entry;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgChgSubTrack = FALSE;
            }
        }

        // flush subtitle buffer
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
    }

    /*If mm is MP4 and playcmd is trick or seek.*/
    if(prSwdmxInst->eCurFMT == SWDMX_FMT_MP4 && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
    {
        if(fgSpeedChange)
        {
            UINT32 u4CurPts = 0;
            if(!(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts <<= 32;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts += 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange =  FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts <<= 32;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts += 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgInvalidForSub = FALSE;
            }
            /*Start trick means we need to deliver just key frames from now on.*/
            /*We don't start trick at the speed between 0 and 2000(including 0 and 2000).*/
            if (i4PlaySpeed > 2000 || i4PlaySpeed < 0)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
            }

            if (prRangeList->eRangeStats == eSWDMX_RANGE_STATE_PENDING)
            {
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
            }

            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRecSeekFlag)&&(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSeekAfterPause = TRUE;
            }

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
            {
                _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame=0;
            u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
            LOG(5, "Mp4 Speed change while VDP Pts = 0x%08x\n", u4CurPts);

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRecSpeedChangeFlag = TRUE;

            // Reset some values for state change.
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
            {
                UINT32 u4Flag;
                DMX_MM_T rDmxMMInfo;

                rDmxMMInfo.fgEnable = TRUE;
                rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
                u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;

                switch (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType)
                {
                    case ENUM_SWDMX_VDEC_DX311: case ENUM_SWDMX_VDEC_WMV7:
                    case ENUM_SWDMX_VDEC_WMV8: case ENUM_SWDMX_VDEC_WMV9:
                    case ENUM_SWDMX_VDEC_MJPEG: case ENUM_SWDMX_VDEC_H263:
                        LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                        rDmxMMInfo.fgSearchStartCode = FALSE;
                        break;
                    default:
                        LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                        rDmxMMInfo.fgSearchStartCode = TRUE;
                        break;
                }
                if (!DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid, u4Flag, &rDmxMMInfo))
                {
                    LOG(3, "Fail to reset demuxer\n");
                }

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = FALSE;
            }
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = FALSE;
                AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            }
            if (i4PlaySpeed == 1000)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = 0;
            }

            if((prCmd->fgSeek) &&
                    (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
            {
                u8SeekPos = prCmd->rSeekOffset.ui8_i_pts_info;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSpeedChgforABRepeat = TRUE;
            }

            /*Flush buffer, set pts for seek.*/
            if(((UINT32)(u8SeekPos>>32) != 0xFFFFFFFF) || ((UINT32)(u8SeekPos) != 0xFFFFFFFF))
            {
                if ((i4PlaySpeed < 4000) && (i4PlaySpeed > 0))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRecSeekFlag = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = 0;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = 0;
                    if((prCmd->fgSeek) &&
                            (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = prCmd->rSeekOffset.ui8_pts_info;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = prCmd->rSeekOffset.ui8_pts_info;
                    }
                    else
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = u8SeekPos;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = u8SeekPos;
                    }
                    //should enable audio if parsing to audio entry larger than target PTS
                    //if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
                    }
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                    }

                    // flush subtitle buffer
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
                    MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                    VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                    
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
                }
                else
                {
                    _SWDMX_Mp4Seek(prSwdmxInst, u8SeekPos, prRangeList, i4PlaySpeed);
                }

                if(i4PlaySpeed >0)
                {
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        if((prCmd->fgSeek) &&
                                (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
                        {
                            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, (prCmd->rSeekOffset.ui8_pts_info+2000));
                        }
                        else
                        {
                            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, (u8SeekPos+2000));
                        }
                    }
                }
                else
                {
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);
                }

                if((prCmd->fgSeek) &&
                        (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
                {
                    VDEC_StartPTS(prSwdmxInst->u1VdecId,FALSE,prCmd->rSeekOffset.ui8_pts_info,NULL);
                }
                else
                {
                    VDEC_StartPTS(prSwdmxInst->u1VdecId,FALSE,u8SeekPos,NULL);
                }

                // set stc
                STC_StopStc(prSwdmxInst->u1StcId);

                LOG(3,"%s %d Set to B2r seek, B2rId is %u\n",__FUNCTION__,__LINE__,prSwdmxInst->u1B2rId);
                //Set Seek to VDP
                VDP_SetSeek(prSwdmxInst->u1B2rId,1,0);
            }
            // 2X -> 1X
            /*Flush buffer, set pts for 2->1 and 1->2.*/
            else if(prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_NORMAL
                    && prSwdmxInst->ePrePlayMode==SWDMX_PLAY_MODE_NORMAL)
            {
                LOG(5, "Mp4 Speed 2X -> 1X\n");

                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1)
                {
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                }

                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1) ||
                        ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 1000) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed > 1)))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                }

                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl==NULL
                        && i4PlaySpeed<=2000&&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed>2000)
                {
                    _SWDMX_Mp4SeekCurPos(prSwdmxInst);

                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);

                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                    }

                    MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                    VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                    //_SWDMX_FlushReqMsgQ(u1SrcId);
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                    // set stc
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                    {
                        AUD_SetStartPts(prSwdmxInst->u1AdecId,prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts=u4CurPts;
                    }
                    else if(prIdxTbl)
                    {
                        prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                    }

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = u4CurPts;
                    //LOG(5, "_rSwdmxMp4Info.i4PrevSpeed != 1000 i4PlaySpeed != 2000\n");

                }
                else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)&&
                         (i4PlaySpeed == 1000))
                {
                    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl = NULL;
                    //if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
                    }

                    //AB repeat need to do seek.
                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSpeedChgforABRepeat)
                    {
                        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                        {
                            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                        }

                        LOG(5, "e_mm_src_type = %d, Trick -> normal PTS = 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type, u4CurPts);
                        MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                        VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                        // _SWDMX_FlushReqMsgQ(u1SrcId);
                        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);

                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = u4CurPts;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = 0;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = 0;

                        // set stc
                        STC_StopStc(prSwdmxInst->u1StcId);
                        STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                    }

                    // set audio range idx
                    prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                    for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                    {
                        if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= (u4CurPts+45000))
                        {
                            prRangeList->u4CurRangeAudIdx = i;
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = TRUE;
                            break;
                        }
                        else
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = FALSE;
                        }
                    }

                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                    {
                        AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                            prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;
                    }
                    else
                    {
                        prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                    }
                }

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSpeedChgforABRepeat)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSpeedChgforABRepeat = FALSE;
                }

                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]))
                {
                    // flush subtitle buffer
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
                }
            }
            /*Flush buffer, set pts for fast backward -> 2x or 1x.*/
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0 &&
                     (i4PlaySpeed == 2000 || i4PlaySpeed == 1000))
            {
                LOG(5, "Rewind -> 2X/1X\n");

                _SWDMX_Mp4Seek(prSwdmxInst, u4CurPts, prRangeList, i4PlaySpeed);

                //if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
                {
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
                }
                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                }
                // flush subtitle buffer
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;

                MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);

                // _SWDMX_FlushReqMsgQ(u1SrcId);

                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,
                                      prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts);

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = u4CurPts;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = 0;
                // set stc
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
            }
            // else if NOT 1X -> 2X
            /*Flush buffer, set pts for fast backward and fast forward.*/
            else if (!(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1000 && (i4PlaySpeed == 2000 || i4PlaySpeed == 1)))
            {
                if (!(((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1)&&(i4PlaySpeed == 2000))||
                        ( (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 2000 ||
                           (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1) )&&
                          ((i4PlaySpeed<1000)&&(i4PlaySpeed>1)))))
                {
                    _SWDMX_Mp4Seek(prSwdmxInst, u4CurPts, prRangeList, i4PlaySpeed);

                    //if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
                    }
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                    }
                    // flush subtitle buffer
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;

                    MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                    VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);

                    //_SWDMX_FlushReqMsgQ(u1SrcId);

                    if (i4PlaySpeed > 1000)
                    {
                        if(u4CurPts < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts)
                        {
                            u4CurPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts;
                        }
                        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                    }
                    else if (i4PlaySpeed <= 1000 &&
                             i4PlaySpeed > 0)
                    {
                        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                    }
                    else
                    {
                        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,
                                              prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts);
                    }

                    // Things for 32/16/8/4/-2/-4/-8/-16/-32 -> 1X are the same
                    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed > 2000 || prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0) &&
                            ((i4PlaySpeed == 1000)||(i4PlaySpeed<=1000 && i4PlaySpeed>0)))
                    {
                        LOG(5, "Trick -> normal PTS = 0x%08x\n", u4CurPts);

                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = u4CurPts;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = 0;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = 0;
                    }

                    // set stc
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);

                    LOG(5, "_rSwdmxMp4Info.i4PrevSpeed != 1000 i4PlaySpeed != 2000\n");
                    // don't need to handle 1X->2X
                }
            }

        }
        /*Do seek.*/
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek)
        {
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl || (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo))
            {
                _SWDMX_Mp4Seek(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts, prRangeList, i4PlaySpeed);
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts=(UINT64)_SWDMX_Mp4SeekCurPos(prSwdmxInst);
            }

            if(prRangeList->fgIsSeekTarget)
            {
                LOG(5, "prRangeList->fgIsSeekTarget\n");

                prRangeList->fgIsSeekTarget = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = FALSE;

                // set stc for starting playback
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts);

                //VDEC_SetRenderFromPts(0, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8SeekPts);
                // flush audio, vido and subtitle fifo buffer
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                }
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;

                /*For the case seek done is faster than seek, only occur while doing step*/
                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1) ||
                        ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 1000) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed > 1)))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts);
                }
                
                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL ))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

                    if(((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts>>32) == 0xFFFFFFFF)&&
                            ((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts) == 0xFFFFFFFF))
                    {
                        if ( prRangeList->u4CurRangeAudIdx <= (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1))
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;

                            AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts);
                        }
                    }
                }
            }
            else
            {
                if ( (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts) >= prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime )
                {
                    LOG(1, "%s: u8VSeekPts = 0x%x, u4TotalTime = 0x%x\n",
                        __FUNCTION__,
                        (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts),
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime );

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = FALSE;

                    if ( (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos))
                    {
                        LOG(1, "u8VSeekPts >= u4TotalTime, Send Video EOS\n");

                        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
                        {
                            _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
                        }

                        if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                        }
                    }

                    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))
                    {
                        LOG(1, "u8VSeekPts >= u4TotalTime, Send Audio EOS\n");

                        if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = TRUE;
                        }
                    }

                    LOG(1, "u8VSeekPts >= u4TotalTime, Range is finished\n");

                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_FINISHED;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = NULL;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = FALSE;

                    for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] = NULL;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack = 0;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

                    return eSWDMX_HDR_PRS_FAIL;
                }
                else
                {
                    //need new range containing target PTS
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange)
                    {
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }
                    LOG(0, "Seek: Need New Range\n");
                    prRangeList->u4CurPTS = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts;
                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                    // if(!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = TRUE;
                    }
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
        }
    }
    /*If mm is M4A and playcmd is trick or seek.*/
    else if(prSwdmxInst->eCurFMT == SWDMX_FMT_M4A || !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
    {
        if(fgSpeedChange)
        {
            UINT32 u4CurPts = 0;

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange =  FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = 0xFFFFFFFF;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts <<= 32;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts += 0xFFFFFFFF;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgInvalidForSub = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSendAudPts = 0xFFFFFFFF;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastAudPts = 0xFFFFFFFF;

            if (i4PlaySpeed < 0)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick = TRUE;
            }

            if (prRangeList->eRangeStats == eSWDMX_RANGE_STATE_PENDING)
            {
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
            }

            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRecSeekFlag)&&(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed == 1))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSeekAfterPause = TRUE;
            }

            u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
            LOG(1, "Audio Mp4 Speed change while VDP Pts = 0x%08x\n", u4CurPts);

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange = TRUE;

            // Reset some values for state change.
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = FALSE;
                prSwdmxInst->fgRcvEOS = FALSE;
                AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            }

            if((prCmd->fgSeek) &&
                    (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
            {
                u8SeekPos = prCmd->rSeekOffset.ui8_i_pts_info;
            }

            //if ( i4PlaySpeed < 0 || i4PlaySpeed == 1000 )
            {
                UINT32 u4DelayTime = 0;

                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid = TRUE;

                //_SWDMX_FlushReqMsgQ(u1SrcId);

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = TRUE;
                 /*If  the cmd passed down is seek then the u8SeekPos is not invalid, we should use it as target during seeking.*/
                if(((UINT32)(u8SeekPos>>32) != 0xFFFFFFFF) || ((UINT32)(u8SeekPos) != 0xFFFFFFFF))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = u8SeekPos;
                }
                /*Otherwise the cmd passed down is trick, but we need to seek at least one time, we use u4CurPts instead.*/
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = u4CurPts;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = u4CurPts;

                // set stc
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);

                //AUD_MMAoutEnable(0, TRUE);
                //AUD_DSPCmdPlay(0);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                while ( u4DelayTime < 20 && AUD_IsDecoderStop(AUD_DSP0,  prSwdmxInst->u1AdecId ) )
#else
                while ( u4DelayTime < 20 && AUD_IsDecoderStop( prSwdmxInst->u1AdecId ) )
#endif
                {
                    x_thread_delay( 50 );
                    u4DelayTime++;
                }
                if ( u4DelayTime != 20)
                {
                    //AUD_MMQueueSyncInfo(prSwdmxInst->u1AdecId,0,0,TRUE);
                }
                else
                {
                    LOG(0,"Speed change, But audio is stop now\n");
                }
            }
        }
        
        /*Do seek*/
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek)
        {
            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl = NULL;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo = FALSE;
                _SWDMX_Mp4Seek(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts, prRangeList, i4PlaySpeed);
            }

            if(prRangeList->fgIsSeekTarget)
            {
                LOG(5, "prRangeList->fgIsSeekTarget\n");

                prRangeList->fgIsSeekTarget = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts <<= 32;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts += 0xFFFFFFFF;

                STC_StopStc(prSwdmxInst->u1StcId);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DSPCmdPlay(AUD_DSP0, 0);
#else
                AUD_DSPCmdPlay(0);
#endif

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                {
                    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL)
                            && (prRangeList->u4CurRangeAudIdx <= (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1)))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;

                        AUD_SetStartPts(prSwdmxInst->u1AdecId,
                                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);
                    }
                }
            }
            else
            {
                if ( ((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts) >= prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime) ||
                        ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry == NULL)))
                {
                    LOG(1, "%s: u8ASeekPts = 0x%x, u4TotalTime = 0x%x\n",
                        __FUNCTION__,
                        (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts),
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime );

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsSeek = FALSE;

                    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))
                    {
                        if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry)
                        {
                            LOG(0, "Audio Index Table Is NULL, Send Audio EOS\n");
                        }
                        else
                        {
                            LOG(0, "u8ASeekPts >= u4TotalTime, Send Audio EOS\n");
                        }

                        if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = TRUE;
                        }
                    }

                    LOG(1, "u8ASeekPts >= u4TotalTime, Range is finished\n");

                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_FINISHED;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = NULL;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = FALSE;

                    for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] = NULL;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack = 0;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

                    return eSWDMX_HDR_PRS_FAIL;
                }
                else
                {
                    //need new range containing target PTS
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange)
                    {
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }
                    LOG(0, "Seek: Need New Range\n");

                    prRangeList->u4CurPTS = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts;
                    if ( i4PlaySpeed < 0 )
                    {
                        prRangeList->u4CurPTS = prRangeList->u4CurPTS > 15 * 90000 ? prRangeList->u4CurPTS - 15 * 90000 : 0;
                    }
                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = NULL;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = FALSE;

                    for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] = NULL;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack = 0;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = TRUE;

                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
        }
    }
    
    /*Normal playback flow.*/   
    if((prRangeList->prDmxRangeInfo != NULL) &&
            (prRangeList->prDmxRangeInfo->pt_idxtbl_lst != NULL) &&
            (prRangeList->prDmxRangeInfo->pt_idxtbl_lst->u.pt_pb_idx_tbl_entry != NULL))
    {
        SWDMX_HDR_PRS_STATE_T eHdrPrsState;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed = i4PlaySpeed;
        eHdrPrsState = _SWDMX_Mp4HdrPrsUseIdx(prSwdmxInst, i4PlaySpeed, prRangeList);
        if( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos )
        {
            prSwdmxInst->fgRcvEOS = TRUE;
        }

        return eHdrPrsState;
    }
    else
    {

        if((prRangeList->prDmxRangeInfo != NULL) &&
                (prRangeList->prDmxRangeInfo->pt_idxtbl_lst != NULL)&&
                !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
        {
            if(prRangeList->prDmxRangeInfo->pt_idxtbl_lst->e_elem_type == MM_ELEMT_TYPE_AUDIO)
                if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                }
        }
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed = i4PlaySpeed;
        return eSWDMX_HDR_PRS_SUCCEED;
    }
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrsUseIdx(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    SWDMX_RANGE_LIST_T* prRangeList)
{
    BOOL fgRangeDone = FALSE;
    BOOL fgVidDone = TRUE;
    BOOL fgAudDone = TRUE;

    UINT64  u8VidOffset = 0xFFFFFFFF;
    UINT64  u8AudOffset = 0xFFFFFFFF;
    UINT64  u8SubOffset = 0xFFFFFFFF;
    UINT64  u8MinOffset = 0;

    UINT32 *pu4VidCurIdx  = NULL;
    UINT32 *pu4AudCurIdx = NULL;
    UINT32 *pu4SubCurIdx = NULL;

    MM_RANGE_ELMT_IDXTBL_T *pKeyIdxTbl = NULL;
	SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;

	if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
	
	u8VidOffset <<= 32;
    u8VidOffset += 0xFFFFFFFF;
    u8AudOffset <<= 32;
    u8AudOffset += 0xFFFFFFFF;
    u8SubOffset <<= 32;
    u8SubOffset += 0xFFFFFFFF;

    pKeyIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl;

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
	
    if (!prRangeList)
    {
        LOG(3, "Range list is NULL\n");
        return eSWDMX_HDR_PRS_EMPTY_RANGE;
    }
    /*M4A fast backward flow.*/
    if ((!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo || prSwdmxInst->eCurFMT == SWDMX_FMT_M4A) && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0)
    {
        LOG(8, "prSwdmxInst->eCurFMT == SWDMX_FMT_M4A && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4PrevSpeed < 0\n");
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                (prRangeList->u4CurRangeAudIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry))
        {

            LOG(6, "prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRangeAudIdx = %u\n", prRangeList->u4CurRangeAudIdx);
            pu4AudCurIdx = &prRangeList->u4CurRangeAudIdx;

            if ( _SWDMX_DeliverMp4Audioframe(prSwdmxInst,
                                             i4Speed,
                                             prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack],
                                             pu4AudCurIdx,
                                             eSWDMX_MEDIA_AUDIO,
                                             prRangeList)  == eSWDMX_HDR_PRS_FAIL )
            {
                return eSWDMX_HDR_PRS_FAIL;
            }
        }
    }
    /*MP4 fast forward and fast backward flow.*/
    else if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->eCurFMT != SWDMX_FMT_M4A && prSwdmxInst->ePlayMode != SWDMX_PLAY_MODE_NORMAL)
    {
        fgRangeDone = FALSE;
        if (pKeyIdxTbl != NULL &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey < pKeyIdxTbl->ui4_number_of_entry))
        {
            LOG(6, "pKeyIdxTbl->ui4_number_of_entry = %x\n", pKeyIdxTbl->ui4_number_of_entry);
            LOG(6, "prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey);
            pu4VidCurIdx = &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey;

			/*Unconsider the */
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid == 0)
            {
            	UINT8 u1CurSampleDescID;
        	
				prSwdmxMp4Info->u1CurMutliDescInfoIdx = 0;

				while (pKeyIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx+1].ui4_sample_idx < *pu4VidCurIdx)
				{
				   prSwdmxMp4Info->u1CurMutliDescInfoIdx ++;
				}

				u1CurSampleDescID =	pKeyIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx].ui1_sample_desc_id;
				LOG(0, "_SWDMX_Mp4HdrPrsUseIdx FF seq header change(u4NumSentVid == 0), video index %d seq header index %d\n",  *pu4VidCurIdx, u1CurSampleDescID);
				
			    if (u1CurSampleDescID<MP4_MAX_SEQ_HEADER && prSwdmxMp4Info->fgVidSeqHeaderIsUsed[u1CurSampleDescID])
				{
					prSwdmxMp4Info->u4VidHeaderPtr = prSwdmxMp4Info->u4VidSeqHeaderPtr[u1CurSampleDescID];
					prSwdmxMp4Info->u4VidHdrLength = prSwdmxMp4Info->u4VidSeqHeaderLength[u1CurSampleDescID];
					
					_SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);

					prSwdmxMp4Info->u1CurMutliDescInfoIdx++;
					prSwdmxMp4Info->u4NextSeqChgVidIdx = pKeyIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx].ui4_sample_idx;

				}
				else
				{
					LOG(0, "Seq header index error!!!!\n");
				}
				   
            }
			else if (*pu4VidCurIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NextSeqChgVidIdx)
			{
				UINT8 u1CurSampleDescID;

				u1CurSampleDescID =  pKeyIdxTbl->at_range_desc_entry_chg[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurMutliDescInfoIdx].ui1_sample_desc_id;
					LOG(0, "_SWDMX_Mp4HdrPrsUseIdx FF seq header change, video index %d seq header index %d\n",  *pu4VidCurIdx, u1CurSampleDescID);

				if (u1CurSampleDescID<MP4_MAX_SEQ_HEADER && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVidSeqHeaderIsUsed[u1CurSampleDescID])
				{
					prSwdmxMp4Info->u4VidHeaderPtr = prSwdmxMp4Info->u4VidSeqHeaderPtr[u1CurSampleDescID];
					prSwdmxMp4Info->u4VidHdrLength = prSwdmxMp4Info->u4VidSeqHeaderLength[u1CurSampleDescID];
					
					_SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
	
					prSwdmxMp4Info->u1CurMutliDescInfoIdx++;
					prSwdmxMp4Info->u4NextSeqChgVidIdx = pKeyIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx].ui4_sample_idx;
				}
				else
				{
					LOG(0, "Seq header index error!!!!\n");
				}

			}
	

            if ( _SWDMX_DeliverMp4Keyframe(prSwdmxInst,
                                           i4Speed,
                                           prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl,
                                           pu4VidCurIdx,
                                           eSWDMX_MEDIA_VIDEO,
                                           prRangeList)  == eSWDMX_HDR_PRS_FAIL )
            {
                return eSWDMX_HDR_PRS_FAIL;
            }
        }
        else if((pKeyIdxTbl != NULL) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey >= pKeyIdxTbl->ui4_number_of_entry)&&
                (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos))
        {
            if (!_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
            {
                LOG(3, "Fail to send Video Eos\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        }
    }
    /*Normal playback flow.*/
    else
    {
        /*Some error handles for index table invalid.*/
        if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) && (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl))
        {
            LOG(3, "No video index table now!\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) )
        {
            LOG(3, "No Audio index table now!\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        /*Some error handles for range done.*/
        if  ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)&&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry))
        {
            if(prRangeList->u4CurRangeAudIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry)
            {
                fgAudDone = FALSE;
                pu4AudCurIdx = &prRangeList->u4CurRangeAudIdx;
                u8AudOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset +
                              prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[*pu4AudCurIdx].ui4_relative_offset;
            }
            else
            {
                LOG(5, "audio range done\n");
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            }
        }
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)&&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry))
        {
            if(prRangeList->u4CurRangeVidIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry)
            {
                fgVidDone = FALSE;
                pu4VidCurIdx = &prRangeList->u4CurRangeVidIdx;
                u8VidOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                              prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[*pu4VidCurIdx].ui4_relative_offset;
            }
            else
            {
                LOG(5, "Video range done\n");
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
            }
        }
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubTitle)&&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]) &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->u.pt_pb_idx_tbl_entry))
        {
            if(prRangeList->u4CurRangeSubIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->ui4_number_of_entry)
            {
                pu4SubCurIdx = &prRangeList->u4CurRangeSubIdx;
                u8SubOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->ui8_base_offset +
                              prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->u.pt_pb_idx_tbl_entry[*pu4SubCurIdx].ui4_relative_offset;
            }
            else
            {
                LOG(5, "subtitle range done\n");
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
            }
        }

		/*Find the correct seq header index.*/
        if (prSwdmxMp4Info->fgEnVideo && prSwdmxMp4Info->prVidIdxTbl &&
			(prSwdmxMp4Info->u4NumSentVid == 0) && (prSwdmxInst->eCurFMT == SWDMX_FMT_MP4))
        {	        	
			UINT8 u1CurSampleDescID;
        	
			prSwdmxMp4Info->u1CurMutliDescInfoIdx = 0;
			prSwdmxMp4Info->u4NextSeqChgVidIdx = prSwdmxMp4Info->prVidIdxTbl->at_range_desc_entry_chg[0].ui4_sample_idx;

			while (prSwdmxMp4Info->prVidIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx+1].ui4_sample_idx < prRangeList->u4CurRangeVidIdx)
			{
			   prSwdmxMp4Info->u1CurMutliDescInfoIdx ++;
			}

			u1CurSampleDescID =	prSwdmxMp4Info->prVidIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx].ui1_sample_desc_id;
			prSwdmxMp4Info->u1PreVidHeaderIndex = u1CurSampleDescID;
			LOG(0, "_SWDMX_Mp4HdrPrsUseIdx normal paly seq header change, video index %d seq header index %d\n",  prRangeList->u4CurRangeVidIdx, u1CurSampleDescID);
			
		    if (u1CurSampleDescID<MP4_MAX_SEQ_HEADER && prSwdmxMp4Info->fgVidSeqHeaderIsUsed[u1CurSampleDescID])
			{
				prSwdmxMp4Info->u4VidHeaderPtr = prSwdmxMp4Info->u4VidSeqHeaderPtr[u1CurSampleDescID];
				prSwdmxMp4Info->u4VidHdrLength = prSwdmxMp4Info->u4VidSeqHeaderLength[u1CurSampleDescID];				
				_SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);				
				prSwdmxMp4Info->u1CurMutliDescInfoIdx++;
				prSwdmxMp4Info->u4NextSeqChgVidIdx = prSwdmxMp4Info->prVidIdxTbl->at_range_desc_entry_chg[prSwdmxMp4Info->u1CurMutliDescInfoIdx].ui4_sample_idx;
			}
			else
			{
				LOG(0, "Seq header index error!!!!\n");
			}
			   
		}
        	
        if(fgVidDone&&fgAudDone)
        {
            fgRangeDone = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
        }

        /*A/V Sync for not normal playback (including 2x) -> normal playback.*/
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio))
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange && i4Speed == 1000)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                {
                    fgRangeDone = FALSE;
                    LOG(5, "Pause Video for A/V Sync\n");
                    VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                    VDEC_Pause(prSwdmxInst->u1VdecId);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange = FALSE;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = FALSE;
                }
                else
                {
                    INT32 i;
                    UINT32 u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
                    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl = NULL;

                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)
                    {
                        prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];
                    }

                    if(prIdxTbl != NULL && (prIdxTbl->u.pt_pb_idx_tbl_entry != NULL))
                    {
                        for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                        {
                            if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= (u4CurPts))
                            {
                                prRangeList->u4CurRangeAudIdx = i;
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = TRUE;
                                LOG(5, "Find Audio PTS = 0x%08x\n",
                                    prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts);
                                break;
                            }
                            else
                            {
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = FALSE;
                            }
                        }
                        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                        {
                            AUD_SetStartPts(prSwdmxInst->u1AdecId, prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts);

                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts =
                                prIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_pts;
                        }
                        else
                        {
                            prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                        }
                    }
                }
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange)
        {
            fgRangeDone = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSpeedChange = FALSE;
        }

        /*timebase never block video/auduio, just move data one by one*/
        if (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
        }
        
        /*Handling feeder invalid condition.*/
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid)
        {
            FEEDER_REQ_DATA_T rFeederReqData;

            //LOG(4, "Try to consume invalid request.\n");
            while (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes > 0)
            {
                if (!_SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData))
                {
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes--;
            }
            //LOG(5, "Consume invalid request completed.\n");

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder = 0;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest = TRUE;

            _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, FALSE);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID += 1;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes = 0;

            // 2X. 1/2x ~ 1/32x, step
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo &&  prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl != NULL && i4Speed > 0 && i4Speed <= 2000 && i4Speed != 1000 )
            {
                if ( prRangeList->u4CurRangeVidIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry )
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_relative_offset;
                }
                else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry > 0 )
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry - 1].ui4_relative_offset;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos   = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff;
            }
            else if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK &&
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                {
                    if ( prRangeList->u4CurRangeAudIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset+
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_relative_offset;

                    }
                    else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry > 0 )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset+
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1].ui4_relative_offset;
                    }

                    LOG(6, "Block Video Audio offset = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff);

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff;
                }
                else
                {
                    LOG(1, "Out of Max Audio Tracks\n");
                    return eSWDMX_HDR_PRS_FAIL;
                }
            }
            else if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
            {
                if ( prRangeList->u4CurRangeVidIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry )
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_relative_offset;
                }
                else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry > 0 )
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry - 1].ui4_relative_offset;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff;
            }
            else
            {
                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                        (prSwdmxInst->i4PlaySpeed == 1000) &&
                        !fgAudDone)
                {
                    if ( prRangeList->u4CurRangeAudIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_relative_offset;
                    }
                    else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry > 0 )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1].ui4_relative_offset;
                    }
                }
                else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                         (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                         (prSwdmxInst->i4PlaySpeed > 1000))
                {
                    if ( prRangeList->u4CurRangeAudIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeAudIdx].ui4_relative_offset;
                    }
                    else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry > 0 )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry - 1].ui4_relative_offset;
                    }
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = 0xFFFFFFFF;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff <<= 32;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff += 0xFFFFFFFF;
                }

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl &&
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo &&
                        !fgVidDone)
                {
                    if ( prRangeList->u4CurRangeVidIdx < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeVidIdx].ui4_relative_offset;
                    }
                    else if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry > 0 )
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui8_base_offset +
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry - 1].ui4_relative_offset;
                    }
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = 0xFFFFFFFF;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff <<= 32;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff += 0xFFFFFFFF;
                }

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = min(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff);
            }

            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgInvalidForSub)&&
                    (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK))
            {

                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubTitle) &&
                        (prRangeList->u4CurRangeSubIdx <prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->ui4_number_of_entry))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->ui8_base_offset+
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeSubIdx].ui4_relative_offset;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8SubLatestOff = 0xFFFFFFFFFFFFFFFF;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgInvalidForSub = FALSE;
                //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample.ui4_relative_offset
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8PrevSampleOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->ui8_base_offset+
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack]->u.pt_pb_idx_tbl_entry[prRangeList->u4CurRangeSubIdx].ui4_relative_offset;
            }

            //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample.ui4_relative_offset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8PrevSampleOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample.ui4_size = 0;

            if (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgGotEos = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNotFirstFrame = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts / 90000;
                LOG(0, "Reset u8CurFilepos with u8VSeekPts: 0x%08x,  u8FirstSamepleRelativeOffset: 0x%08x\n", (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts, (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset);
            }

            _SWDMX_Mp4ResetFeeder(prSwdmxInst);
        }
        /*Bloc A/V/S for range done condition in case of U-storage playback.*/
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type ==
                MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {
            if(fgVidDone&&(!fgAudDone)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo))
            {
                if(!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone)
                {
                    LOG(5, "Video Range is done\n");
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = TRUE;
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
            else if(!fgVidDone&&(fgAudDone)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio))
            {
                if(!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone)
                {
                    LOG(5, "Audio Range is done\n");
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = TRUE;
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
        }

        /*Let's calculate some offset, then move some data.*/
        // not send subtitle
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub ||
                !pu4SubCurIdx ||
                !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubTitle )
        {
            LOG(8, "subtitle block or not enable or done\n");

            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio))
            {
                u8MinOffset = u8AudOffset;

                if(u8MinOffset == u8AudOffset)
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)&&
                            pu4AudCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8AudOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack],
                                                     pu4AudCurIdx,
                                                     eSWDMX_MEDIA_AUDIO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }
            else if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo))
            {
                u8MinOffset = u8VidOffset;

                if(u8MinOffset == u8VidOffset)
                {
                    if(pu4VidCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8VidOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl,
                                                     pu4VidCurIdx,
                                                     eSWDMX_MEDIA_VIDEO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }
            else if( (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid && (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)) ||
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud && (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)) )
            {
                LOG(1, "pause move data!\n");
                x_thread_delay(10);
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
            else
            {
                u8MinOffset = (u8AudOffset > u8VidOffset ? u8VidOffset : u8AudOffset);

                if(u8MinOffset == u8VidOffset)
                {
                    if(pu4VidCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8VidOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl,
                                                     pu4VidCurIdx,
                                                     eSWDMX_MEDIA_VIDEO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
                else if(u8MinOffset == u8AudOffset)
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)&&
                            pu4AudCurIdx&&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8AudOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack],
                                                     pu4AudCurIdx,
                                                     eSWDMX_MEDIA_AUDIO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = FALSE;
            }
        }
        else // send subtitle
        {
            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio))
            {
                u8MinOffset = (u8SubOffset > u8AudOffset ? u8AudOffset : u8SubOffset);

                if(u8MinOffset == u8AudOffset)
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)&&
                            pu4AudCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8AudOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack],
                                                     pu4AudCurIdx,
                                                     eSWDMX_MEDIA_AUDIO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
                else
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)&&
                            pu4SubCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8SubOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack],
                                                     pu4SubCurIdx,
                                                     eSWDMX_MEDIA_SUBTITLE) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"subtitle table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }
            else if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud)&&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo))
            {
                u8MinOffset = (u8SubOffset > u8VidOffset ? u8VidOffset : u8SubOffset);

                if(u8MinOffset == u8VidOffset)
                {
                    if(pu4VidCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8VidOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl,
                                                     pu4VidCurIdx,
                                                     eSWDMX_MEDIA_VIDEO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
                else
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)&&
                            pu4SubCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8SubOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack],
                                                     pu4SubCurIdx,
                                                     eSWDMX_MEDIA_SUBTITLE) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"subtitle table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }
            else
            {
                u8MinOffset = (u8AudOffset > u8VidOffset ? u8VidOffset : u8AudOffset);
                u8MinOffset = (u8MinOffset > u8SubOffset ? u8SubOffset : u8MinOffset);


                if(u8MinOffset == u8VidOffset)
                {
                    if(pu4VidCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8VidOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl,
                                                     pu4VidCurIdx,
                                                     eSWDMX_MEDIA_VIDEO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
                else if(u8MinOffset == u8AudOffset)
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK)&&
                            pu4AudCurIdx&&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8AudOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack],
                                                     pu4AudCurIdx,
                                                     eSWDMX_MEDIA_AUDIO) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"audio table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
                else
                {
                    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)&&
                            pu4SubCurIdx&&
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack])
                    {
                        if (_SWDMX_Mp4HdrPrsDataType(prSwdmxInst,
                                                     u8SubOffset,
                                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack],
                                                     pu4SubCurIdx,
                                                     eSWDMX_MEDIA_SUBTITLE) == eSWDMX_HDR_PRS_FAIL)
                        {
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                    else
                    {
                        if(!fgRangeDone)
                        {
                            LOG(1,"subtitle table is NULL\n");
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                    }
                }
            }
        }
        /*Move data finished, now let's handle some conditions.*/
        if ((!fgRangeDone) &&
                (prRangeList->eRangeStats == eSWDMX_RANGE_STATE_PENDING))
        {
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
        }

        /*If end of stream.*/
        if (prRangeList->prDmxRangeInfo->b_endofstream)
        {
            if (fgRangeDone)
            {
                //LOG(5, "Last Range is done set it as pending\n");
                fgRangeDone = FALSE;
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
            }

            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                    (prRangeList->u4CurRangeVidIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_number_of_entry) &&
                    (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos))
            {
                LOG(5, "_SWDMX_Mp4HdrPrsUseIdx Last Video Range\n");

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
                {
                    _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
                }

                if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                }

                if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                        (((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                          (prRangeList->u4CurRangeAudIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry))
                         || (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                         ||(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry == 0)))
                {
                    /*if audio too shorter than video,or the audio entry is null,when mw receive seek done,will flush the audio_eos flag.
                    so if the video is eos,send audio eos again*/
                    AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
                    if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = TRUE;

                    }
                    //AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
                }
            }

            if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                    (((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
                      (prRangeList->u4CurRangeAudIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry))
                     || (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack])
                     ||(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->ui4_number_of_entry == 0))&&
                    (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))
            {
                LOG(5, "_SWDMX_Mp4HdrPrsUseIdx Last Audio Range\n");

                if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = TRUE;
                }
                AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
            {
                UINT16 u2CurQNum, u2MaxQNum;

                VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);

                if (u2CurQNum == 0 && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudIsReady)
                {
                    LOG(0, "Trigger audio by video eos\n");
                    AUD_MMAoutEnable(0, TRUE);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudIsReady = FALSE;
                }
            }
        }

        /*Request data.*/
        if (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
        {
            if (_SWDMX_Mp4RequestDataByTime(prSwdmxInst) != eSWDMX_HDR_PRS_SUCCEED)
            {
                LOG(5, "Feeder fails to move data\n");
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
        }
        else if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid)
        {
            if (_SWDMX_Mp4RequestData(prSwdmxInst) != eSWDMX_HDR_PRS_SUCCEED)
            {
                LOG(5, "Feeder fails to move data\n");
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
        }

        /*If range done.*/
        if (fgRangeDone)
        {
            INT32 i;
            BOOL   fgPending = FALSE;

            if ( prSwdmxInst->eCurFMT != SWDMX_FMT_M4A )
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
                {
                    _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
                }

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type ==
                        MM_SRC_TYPE_HIGH_SPEED_STORAGE)
                {
                    UINT32 u4CurPts = 0;
                    UINT32 u4CurDmxPts = 0;
                    UINT16 u2MaxQNum = 0;
                    UINT16 u2CurQNum = 0;

                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                    {
                        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
                        u4CurDmxPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts;
                        u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);

                        LOG(5, "V: 0x%08x, A: 0x%08x, C: 0x%08x, D: 0x%08x, Q: %ld, MQ: %ld, Num: %d, F: %d\n",
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts,
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts,
                            u4CurPts,
                            (u4CurDmxPts - u4CurPts),
                            u2CurQNum,
                            u2MaxQNum,
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid,
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps);

                        if (((u4CurDmxPts >= u4CurPts) && ((u4CurDmxPts - u4CurPts) > MP4_ONE_SEC_DURATION)) &&
                                (u2CurQNum > 2 * prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps))
                        {
                            LOG(5, "Range is pending\n");
                            x_thread_delay(10);
                            fgPending = TRUE;
                        }
                    }
                    else
                    {
                        u4CurDmxPts = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts;
                        u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);

                        if ((u4CurDmxPts >= u4CurPts) &&
                                ((u4CurDmxPts - u4CurPts)> MP4_ONE_SEC_DURATION))
                        {
                            LOG(5, "Range is pending\n");
                            x_thread_delay(10);
                            fgPending = TRUE;
                        }
                    }
                }
                else
                {
                    //should not block video when buffer under flow
                    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo) &&
                            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
                            (prSwdmxInst->fgSendUnderflow2MW==FALSE))
                    {
                        UINT16 u2MaxQNum = 0;
                        UINT16 u2CurQNum = 0;

                        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
                        if (u2CurQNum > 2 * (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps))
                        {
                            LOG(5, "Range is pending\n");
                            fgPending = TRUE;
                        }
                    }
                }
            }

            if(fgPending)
            {
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
            }
            else
            {
                LOG(5, "Range is finished\n");
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_FINISHED;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl = NULL;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = TRUE;

                for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[i] = NULL;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1TotalATrack = 0;

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockAud = FALSE;
            }
        }
    }

    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrsDataType(
    SWDMX_INFO_T* prSwdmxInst,
    UINT64 u8Offset,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType)
{
    if (!prSwdmxInst || (prIdxTbl == NULL) || (pu4CurIdx == NULL))
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    /*Error handles.*/
    if ((u8Offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size) >
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz))
    {
        LOG(5, "Video offset or Audio offset is larger than range size\n");

        if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) && (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos))
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
            {
                _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
            }

            if (_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
            }
        }
        if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio) &&
			(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))
        {
            if(_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos = TRUE;
            }
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
        }
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if (_SWDMX_DeliverMp4DataUseIdx(prSwdmxInst,
                                    1000, prIdxTbl,
                                    pu4CurIdx,u1HdrType)== eSWDMX_HDR_PRS_FAIL)
    {
        return eSWDMX_HDR_PRS_FAIL;
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4RequestData(SWDMX_INFO_T* prSwdmxInst)
{
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;
    UINT32 u4ReadSize = 0;
    UINT32 u4MaxDataByte = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
    {
        u4ReadSize = MIN_READ_SIZE;

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {
            u4ReadSize = MIN_READ_SIZE * 8;
            u4ReadSize = u4ReadSize > prSwdmxInst->rFeederInfo.u4Size/2 ? MIN_READ_SIZE : u4ReadSize;
        }
    }
    else
    {
        u4ReadSize =  MIN_READ_SIZE_NO_VIDEO;
    }
    u4MaxDataByte  = prSwdmxInst->rFeederInfo.u4Size - u4ReadSize;

    if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder < u4MaxDataByte) &&
            ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos) < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz))
    {
        u4ReadSize = min((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz - prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos), (UINT64)u4ReadSize);
        if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes == 0) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount <=4))
        {
            if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                                 u4ReadSize,
                                 (UINT64)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos),
                                 &rFeederCond,
                                 prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID++) == FEEDER_E_BUFFER_FULL)
            {
                LOG(3, "Feeder buffer is not enough\n");
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes++;
        }


        if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough)
        {
            if (!_SWDMX_Mp4GetDataFromFeederNoWait(prSwdmxInst, &rFeederReqData))
            {
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        else
        {
            if (!_SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData))
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

        }
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes--;

        // Feeder add error code for us. u4WriteAddr = 0 is error
        if(rFeederReqData.u4WriteAddr == 0)
        {
            LOG(1, "_SWDMX_Mp4RequestData Mp4 Fd u4WriteAddr = 0.\n");

            _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount++;

            return eSWDMX_HDR_PRS_FAIL;
        }

        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = rFeederReqData.u4WriteAddr+ rFeederReqData.u4AlignOffset;

            if(rFeederReqData.u4WriteAddr < prSwdmxInst->rFeederInfo.u4StartAddr)
            {
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount++;
                return eSWDMX_HDR_PRS_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount = 0;

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr > VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr  +
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr)   - VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest = FALSE;
        }

        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos += rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder+= rFeederReqData.u4ReadSize -rFeederReqData.u4AlignOffset ;
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos) >= prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
            else
            {
                if (((!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone)||
                        (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos) ||( !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))&&
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough))
                {
                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                }
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo &&
                 (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos||prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough))
        {
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
        {
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
    }

    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4RequestDataByTime(SWDMX_INFO_T* prSwdmxInst)
{
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;
    UINT32 u4ReadSize = 0;
    UINT32 u4MaxDataByte = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
    {
        u4ReadSize = MIN_READ_SIZE;
    }
    else
    {
        u4ReadSize =  MIN_READ_SIZE_NO_VIDEO;
    }

    u4MaxDataByte  = prSwdmxInst->rFeederInfo.u4Size - prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder;

    if(u4ReadSize < u4MaxDataByte)
    {
        if ((!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgGotEos) &&
                ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos == 0xFFFFFFFFFFFFFFFF) ||
                 ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos != 0xFFFFFFFFFFFFFFFF) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime))))
        {
            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes == 0) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount <=4))
            {
                if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                                     u4ReadSize,
                                     (UINT64)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos),
                                     &rFeederCond,
                                     prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID++) == FEEDER_E_BUFFER_FULL)
                {
                    LOG(3, "Feeder buffer is not enough\n");
                    return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes++;
            }

            if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough)
            {
                if (!_SWDMX_Mp4GetDataFromFeederNoWait(prSwdmxInst, &rFeederReqData))
                {
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
            else
            {
                if (!_SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData))
                {
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }
            }

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes--;

            if (rFeederReqData.fgEof)
            {
                LOG(0, "Get EOF !!!!\n");
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgGotEos = TRUE;
            }

            // Feeder add error code for us. u4WriteAddr = 0 is error
            if(rFeederReqData.u4WriteAddr == 0)
            {
                LOG(1, "_SWDMX_Mp4RequestData Mp4 Fd u4WriteAddr = 0.\n");

                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount++;

                return eSWDMX_HDR_PRS_FAIL;
            }

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = rFeederReqData.u4WriteAddr+ rFeederReqData.u4AlignOffset;

                if(rFeederReqData.u4WriteAddr < prSwdmxInst->rFeederInfo.u4StartAddr)
                {
                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount++;
                    return eSWDMX_HDR_PRS_FAIL;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount = 0;

                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr > VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr  +
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr)   - VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest = FALSE;
            }
#if 0
            //log
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos != 0xFFFFFFFFFFFFFFFF)
            {
                LOG(0, "Request postion: %d  offset: 0x%08x :", (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr);

                {
                    UINT8 i=0;
                    for (i=0; i<32; i++)
                    {
                        printf("%02x ", ((UINT8 *)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr)[i]);
                    }

                    printf("\n");
                }
            }
#endif
            //When request the 0-s data, neet to check the ack data from 0-s or 0-offset.
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos == 0)
            {
                UINT8 *pu1Buf = (UINT8 *)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr;

                if ((pu1Buf[4] == 0x66) && (pu1Buf[5] == 0x74) && (pu1Buf[6] == 0x79) && (pu1Buf[7] == 0x70))
                {
                    LOG(0, "DLNA time-base request data from 0-s,  but ack data from 0-offset\n");
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNotFirstFrame = TRUE;
                }
            }

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder += rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8CurFilepos = 0xFFFFFFFFFFFFFFFF;

            //Ack data from 0-offset not 0-s,  need to skip some data untill find the correct position.
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNotFirstFrame == TRUE)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr += prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder = 0;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset -= prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr += prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder -= prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8FirstSamepleRelativeOffset = 0;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNotFirstFrame = FALSE;

                    LOG(0, "First frame data: ");

                    {
                        UINT8 i=0;
                        for (i=0; i<32; i++)
                        {
                            printf("%02x ", ((UINT8 *)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr)[i]);
                        }

                        printf("\n");
                    }
                }
            }
        }
        else
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
                {
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }
                else
                {
                    if (((!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgPartialRangeDone)||
                            (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos) ||( !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos))&&
                            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough))
                    {
                        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                    }
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo &&
                     (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos||prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederLeftNotEnough))
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverAEos)
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
        }
    }

    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4Audioframe(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType,
    SWDMX_RANGE_LIST_T* prRangeList)
{
    UINT8 u1PidIdx, u1PidType;
    UINT32 u4DmxAvailSize = 0;
    UINT32 u4ReadSize = 0;
    UINT32 u4LastFrame = 0;
    UINT32 u4Index = 0;

    DMX_MM_DATA_T rDmxMMData;
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;
    UINT32 u4WriteAddr = 0;

    UINT64 u8WriteOffset = 0;
    MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T *prTmpSample;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    UINT64 u8TmpSampleOffset = 0;

    if (!prSwdmxInst || !prIdxTbl )
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

    if(prSwdmxMp4Info->fgFeederInvalid)
    {
        INT32 i;
        FEEDER_REQ_DATA_T rFeederReqData;

        for (i = 0; i < prSwdmxMp4Info->i4ReqReadTimes ; i++)
        {
            _SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData);
        }

        prSwdmxMp4Info->u4LeftBytesInFeeder = 0;
        prSwdmxMp4Info->fgRepeatRequest = TRUE;
        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, FALSE);
        prSwdmxMp4Info->u4FeederReqID += 1;
        prSwdmxMp4Info->i4ReqReadTimes = 0;
        prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;
        prSwdmxMp4Info->rPrevSample.ui4_size = 0;

        _SWDMX_Mp4ResetFeeder(prSwdmxInst);

        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    u1PidIdx = prSwdmxMp4Info->u1AudPid;

    u1PidType = DMX_PID_TYPE_ES_AUDIO;

    u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                          (DMX_PID_TYPE_T)u1PidType,
                                          u1PidIdx);

    LOG(6, "prSwdmxMp4Info->u4CurSentKey = %ld Key Pts = 0x%08x Dmx available size = 0x%08x\n",
        *pu4CurIdx,
        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts,
        u4DmxAvailSize);

    if ( u1HdrType == eSWDMX_MEDIA_AUDIO )
    {
        UINT32 u4Addr, u4Size;
        UINT32 u4BufSize;

        if(!_SWDMX_GetAudioFifo(0, &u4Addr, &u4Size))
        {
            LOG(3, "_SWDMX_GetAudioFifo Fail\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        u4BufSize = u4Size;
        LOG(8, "(u4BufSize - u4DmxAvailSize) = %u,u4LastAudioFifoDataSize = %u\n", (u4BufSize - u4DmxAvailSize), prSwdmxMp4Info->u4LastAudioFifoDataSize );
        if ((prSwdmxMp4Info->u4LastSendAudPts == 0xFFFFFFFF) || (prSwdmxMp4Info->u4LastAudPts == 0xFFFFFFFF))
        {

        }
        else if((prSwdmxMp4Info->u4LastSendAudPts + 2000>= prSwdmxMp4Info->u4LastAudPts) && ((u4BufSize - u4DmxAvailSize) < 100000) &&
                ((u4BufSize - u4DmxAvailSize) == prSwdmxMp4Info->u4LastAudioFifoDataSize) )
        {
            //x_thread_delay(10);
        }
        else
        {
            prSwdmxMp4Info->u4LastAudioFifoDataSize = (u4BufSize - u4DmxAvailSize);
            LOG(5,"u4LastSendAudPts = 0x%x, u4LastAudPts = 0x%x\n", prSwdmxMp4Info->u4LastSendAudPts, prSwdmxMp4Info->u4LastAudPts );
            LOG(5, "u4LastAudioFifoDataSize = %u\n", prSwdmxMp4Info->u4LastAudioFifoDataSize);
            x_thread_delay(100);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

    if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > u4DmxAvailSize)
    {
        LOG(7, "Demuxer Buffer is not enough Hdr type = %x u4DmxAvailSize = %ld  frame size = %ld.\n",
            u1HdrType,
            u4DmxAvailSize,
            prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

        x_thread_delay(1);
        prSwdmxInst->fgFifoFull = TRUE;
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    u4LastFrame = *pu4CurIdx;
    if (((*pu4CurIdx) < prIdxTbl->ui4_number_of_entry - 1)&& !prSwdmxMp4Info->fgDeliverAEos)
    {
        if (*pu4CurIdx != prSwdmxMp4Info->u4LastSentKey)
        {
            if (prSwdmxMp4Info->i4ReqReadTimes == 0)
            {
                if ( prIdxTbl->ui4_number_of_entry - *pu4CurIdx > prIdxTbl->ui4_number_of_entry / M4A_RANGE_DIV_COUNT + 1)
                {
                    u4LastFrame = *pu4CurIdx + prIdxTbl->ui4_number_of_entry / M4A_RANGE_DIV_COUNT + 1;
                }
                else
                {
                    u4LastFrame = prIdxTbl->ui4_number_of_entry - 1;
                }

                LOG(6, "start_index.. = %u, end_index = %u ui4_number_of_entry = %u\n", *pu4CurIdx, u4LastFrame, prIdxTbl->ui4_number_of_entry);

                u4ReadSize = prIdxTbl->u.pt_pb_idx_tbl_entry[u4LastFrame].ui4_relative_offset -
                             prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                             prIdxTbl->u.pt_pb_idx_tbl_entry[u4LastFrame].ui4_size;

                //u4ReadSize = max((UINT64)(100000), (UINT64)u4ReadSize);

                LOG(6,"FeederSetRequest ui4_size = %u, offset = %llu\n", u4ReadSize,
                    (UINT64)((prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset)+(prIdxTbl->ui8_base_offset)));

                if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                                     u4ReadSize,
                                     (UINT64)((prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset)+(prIdxTbl->ui8_base_offset)),
                                     &rFeederCond,
                                     prSwdmxMp4Info->u4FeederReqID) == FEEDER_E_BUFFER_FULL)
                {
                    LOG(3, "Feeder's buffer is full\n");
                    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxInst->rFeederInfo.u4WriteAddr));
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }
                else
                {
                    prSwdmxMp4Info->u4FeederReqID++;
                    prSwdmxMp4Info->i4ReqReadTimes++;
                }
            }

            if (!_SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData))
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

            prSwdmxMp4Info->i4ReqReadTimes--;

            // Feeder add error code for us. u4WriteAddr = 0 is error
            if(rFeederReqData.u4WriteAddr == 0)
            {
                LOG(1, "_SWDMX_DeliverMp4Audioframe Mp4 Fd u4WriteAddr = 0.\n");

                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                prSwdmxMp4Info->i4FeederInvalidCount++;

                return eSWDMX_HDR_PRS_FAIL;
            }

            if (rFeederReqData.u4ReadSize <= rFeederReqData.u4AlignOffset
                    ||prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > (rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset))
            {
                LOG(3, "_SWDMX_DeliverMp4Audioframe rFeederReqData.u4ReadSize <= rFeederReqData.u4AlignOffset\n");
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

            if(rFeederReqData.u4WriteAddr+rFeederReqData.u4AlignOffset>=prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                u4WriteAddr = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr+(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset-prSwdmxInst->rFeederInfo.u4EndAddr));
            }
            else
            {
                u4WriteAddr = VIRTUAL(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset);
            }
            prSwdmxMp4Info->u4LastSentKey=*pu4CurIdx;
            prSwdmxMp4Info->u4RecFeederWPtr = u4WriteAddr;
            prSwdmxMp4Info->u4CurDmxFeederRPtr = u4WriteAddr;
            prSwdmxMp4Info->fgRequestData = FALSE;
            prSwdmxMp4Info->u4LeftBytesInFeeder+= rFeederReqData.u4ReadSize -rFeederReqData.u4AlignOffset;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = prSwdmxMp4Info->u4CurDmxFeederRPtr;//prSwdmxMp4Info->u4RecFeederWPtr;
        }

        prTmpSample = &prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx];
        u8TmpSampleOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset;
        prSwdmxMp4Info->rPrevSample.ui4_size = 0;
        prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

        u4Index = *pu4CurIdx;
        while( (u4Index <= u4LastFrame) && (prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size <= prSwdmxMp4Info->u4LeftBytesInFeeder))
        {
            prTmpSample = &prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index];
            u8TmpSampleOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset;

            if (u8TmpSampleOffset >= prSwdmxMp4Info->u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size)
            {
                u8WriteOffset = u8TmpSampleOffset - (prSwdmxMp4Info->u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size);
            }
            else
            {
                prSwdmxMp4Info->u8CurFilepos = u8TmpSampleOffset;
                break;
            }

            if((u8WriteOffset + prTmpSample->ui4_size) > (prSwdmxMp4Info->u4LeftBytesInFeeder))
            {
                LOG(3, "fgFeederInvalid , left byte too small\n");
                break;
            }

            prSwdmxMp4Info->u4CurDmxFeederRPtr += u8WriteOffset;
            if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxMp4Info->u4CurDmxFeederRPtr =
                    prSwdmxMp4Info->u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }

            if (prSwdmxMp4Info->u4ADecType == ENUM_SWDMX_ADEC_AAC)
            {
                UINT8 u1SampleRateIdx = 3;
                UINT32 u4RequestSize = AAC_ALLOC_SIZE;
                UINT32 u4Align = 16;

                if(prSwdmxInst->pucAudBuf == 0)
                {
                    LOG(1, "pucAudBuf should be allocated in init function\n");

                    _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId,eSWDMX_GET_INTERNAL_BUF, (UINT32*)prSwdmxInst->pucAudBuf, &u4RequestSize, &u4Align);
                }
                if (prSwdmxInst->pucAudBuf)
                {
                    /* 8b: syncword */
                    prSwdmxInst->pucAudBuf[0] = 0xFF;
                    /* 4b: syncword */
                    /* 1b: mpeg id = 1 */
                    /* 2b: layer = 0 */
                    /* 1b: protection absent */
                    prSwdmxInst->pucAudBuf[1] = 0xF9;
                    /* 2b: profile */
                    /* 4b: sampling_frequency_index */
                    /* 1b: private = 0 */
                    /* 1b: channel_configuration */
                    switch (prSwdmxMp4Info->u4AudSampleRate)
                    {
                        case 96000:
                            u1SampleRateIdx = 0;
                            break;
                        case 88200:
                            u1SampleRateIdx = 1;
                            break;
                        case 64000:
                            u1SampleRateIdx = 2;
                            break;
                        case 48000:
                            u1SampleRateIdx = 3;
                            break;
                        case 44100:
                            u1SampleRateIdx = 4;
                            break;
                        case 32000:
                            u1SampleRateIdx = 5;
                            break;
                        case 24000:
                            u1SampleRateIdx = 6;
                            break;
                        case 22050:
                            u1SampleRateIdx = 7;
                            break;
                        case 16000:
                            u1SampleRateIdx = 8;
                            break;
                        case 12000:
                            u1SampleRateIdx = 9;
                            break;
                        case 11025:
                            u1SampleRateIdx = 10;
                            break;
                        case 8000:
                            u1SampleRateIdx = 11;
                            break;
                        default:
                            u1SampleRateIdx = 4;
                            break;
                    }

                    prSwdmxInst->pucAudBuf[2] = (1 << 6) |
                                                ((u1SampleRateIdx<< 2) & 0x3C) |
                                                ((prSwdmxMp4Info->u4AudChannelNs >> 2) & 0x1);
                    /* 2b: channel_configuration */
                    /* 1b: original */
                    /* 1b: home */
                    /* 1b: copyright_id */
                    /* 1b: copyright_id_start */
                    /* 2b: aac_frame_length */
                    prSwdmxInst->pucAudBuf[3] = ((prSwdmxMp4Info->u4AudChannelNs & 0x3) << 6) |
                                                (((prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size + AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
                    /* 8b: aac_frame_length */
                    prSwdmxInst->pucAudBuf[4] = (((prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size + AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
                    /* 3b: aac_frame_length */
                    /* 5b: adts_buffer_fullness */
                    prSwdmxInst->pucAudBuf[5] = (((prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size + AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
                                                ((0x7FF >> 6) & 0x1F);
                    /* 6b: adts_buffer_fullness */
                    /* 2b: number_of_raw_data_blocks_in_frame */
                    prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);
                    rDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucAudBuf;
                    rDmxMMData.u4BufEnd = rDmxMMData.u4BufStart + AAC_ALLOC_SIZE;
                    rDmxMMData.u4StartAddr = rDmxMMData.u4BufStart;
                    rDmxMMData.u4FrameSize = AAC_FAKE_HDR_SIZE;
                    rDmxMMData.u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;
                    rDmxMMData.u1Idx = prSwdmxMp4Info->u1AudPid;

                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                    {
                        LOG(1, "Demuxer fails to move data. SrcID = %d, Header Type = %d\n",
                            prSwdmxInst->u1SwdmxId,
                            u1HdrType);
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                }
            }

            rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            rDmxMMData.u4StartAddr = prSwdmxMp4Info->u4CurDmxFeederRPtr;
            rDmxMMData.u4FrameSize = prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
            rDmxMMData.u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;
            rDmxMMData.u1Idx = prSwdmxMp4Info->u1AudPid;

            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
            {
                LOG(1, "Demuxer fails to move data. SrcID = %d, Header Type = %d\n",
                    prSwdmxInst->u1SwdmxId,
                    u1HdrType);
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }

            prSwdmxMp4Info->u4LeftBytesInFeeder -=
                ((UINT32)u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size);

            prSwdmxMp4Info->u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
            if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxMp4Info->u4CurDmxFeederRPtr =
                    prSwdmxMp4Info->u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }

            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));

            //record the previous sample data
            x_memcpy(&(prSwdmxMp4Info->rPrevSample),
                     prTmpSample,
                     sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));

            prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

            LOG(2, "_SWDMX_DeliverMp4Audioframe, index = %u, u4LastFrame = %u, u4CurAudPts = 0x%08x\n", u4Index, u4LastFrame, prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts);

            // update stc
            if ( prSwdmxMp4Info->u4LastAudPts > prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts)
            {
                STC_SetStcValue(prSwdmxInst->u1StcId, prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts);
            }

            prSwdmxMp4Info->u4LastAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;

            u4Index++;
        }
    }

    //prSwdmxMp4Info->u4LastAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
    prSwdmxMp4Info->u4CurSentKey = *pu4CurIdx;

    while (!_SWDMX_Mp4CalculateAudioFrame(prSwdmxInst, i4Speed))
    {
        // The last key frame
        if (prSwdmxMp4Info->u4AudRepeatFrame == 0)
        {
            if ((i4Speed > 2000) &&
                    (prSwdmxMp4Info->u4CurRepeatFrameIdx == prIdxTbl->ui4_number_of_entry - 1) &&
                    !prSwdmxMp4Info->fgDeliverAEos)
            {
                LOG(5, "Fast Forward _SWDMX_DeliverMp4Audioframe Deliver Audio Eos\n");
                if (!_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                {
                    LOG(3, "Fail to send Audio Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                prSwdmxMp4Info->fgDeliverAEos = TRUE;
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if ((i4Speed < 0) &&
                     (prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts < 45000) &&
                     (prSwdmxMp4Info->u4CurRepeatFrameIdx == 0) &&
                     !prSwdmxMp4Info->fgDeliverAEos)
            {
                if (!_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
                {
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                prSwdmxMp4Info->fgDeliverAEos = TRUE;
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if ( prSwdmxMp4Info->u4CurRepeatFrameIdx == 0 )
            {
                INT32 i = 0;

                //need new range containing target PTS
                if(prSwdmxMp4Info->fgWaitNewRange)
                {
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }

                LOG(0, "_SWDMX_DeliverMp4Audioframe: Need New Range\n");
                prRangeList->u4CurPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
                if ( i4Speed < 0 )
                {
                    prRangeList->u4CurPTS = prRangeList->u4CurPTS > 20 * 90000 ? prRangeList->u4CurPTS - 20 * 90000 : 0;
                }
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                prSwdmxMp4Info->fgWaitNewRange = TRUE;
                prSwdmxMp4Info->fgPartialRangeDone = FALSE;

                for (i = 0; (i < prSwdmxMp4Info->u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                {
                    prSwdmxMp4Info->prAudIdxTbl[i] = NULL;
                }
                prSwdmxMp4Info->u1TotalATrack = 0;

                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;

                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if ( prSwdmxMp4Info->u4CurRepeatFrameIdx == prIdxTbl->ui4_number_of_entry )
            {
                INT32 i = 0;

                LOG(5, "Range is finished\n");
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_FINISHED;
                prSwdmxMp4Info->fgPartialRangeDone = FALSE;

                for (i = 0; (i < prSwdmxMp4Info->u1TotalATrack) && (i < MP4_MAX_AUDIO_TRACK); i++)
                {
                    prSwdmxMp4Info->prAudIdxTbl[i] = NULL;
                }
                prSwdmxMp4Info->u1TotalATrack = 0;

                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;

                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
    }

    *pu4CurIdx = prSwdmxMp4Info->u4CurRepeatFrameIdx;
    prSwdmxMp4Info->u4CurSentKey = *pu4CurIdx;

    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4Keyframe(
    SWDMX_INFO_T* prSwdmxInst,
    INT32 i4Speed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
    UINT32* pu4CurIdx,
    UINT8 u1HdrType,
    SWDMX_RANGE_LIST_T* prRangeList)
{
    UINT8 u1PidIdx, u1PidType;
    UINT32 u4DmxAvailSize = 0;
    UINT32 u4CurDmxFeederRPtr = 0;
    UINT32 u4LeftBytesInFeeder = 0;
    UINT64 au8WriteOffset[2] = {0};
    UINT32 aui4FrameSize[2] = {0};
    BOOL fgIsNewFrame = FALSE;
    BOOL fgHasBreak = FALSE;
    INT32 i4_i = 0;
    
    DMX_MM_DATA_T rDmxMMData;
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;

    UINT32 u4WriteAddr = 0,u4NaluSize = 0, u4FrmSize = 0;
    UINT8  i = 0;

    UCHAR *pucFdrBuf = NULL;

    UINT32 u4Index = 0xFFFFFFFF;
    MM_RANGE_ELMT_IDXTBL_T *prVidIdxTbl = NULL;
    UINT64 u8WriteOffset = 0;
    MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T *prTmpSample = NULL;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    UINT64 u8TmpSampleOffset = 0;
    UINT32 u4ReadSize = 0;
    UINT32 u4SendFrameCount = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid)
    {
        INT32 i;
        FEEDER_REQ_DATA_T rFeederReqData;

        for (i = 0; i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes ; i++)
        {
            _SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData);
        }

        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder = 0;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRepeatRequest = TRUE;
        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, FALSE);
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID += 1;
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes = 0;

        _SWDMX_Mp4ResetFeeder(prSwdmxInst);

        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    u1PidIdx = (u1HdrType == eSWDMX_MEDIA_VIDEO) ? (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid) :
               (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);

    u1PidType = (u1HdrType == eSWDMX_MEDIA_VIDEO) ? (DMX_PID_TYPE_ES_VIDEO) : (DMX_PID_TYPE_ES_AUDIO);

#if 0
    u4DmxAvailSize = DMX_MM_GetBufEmptySize(u1HdrType);
#endif
    u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                          (DMX_PID_TYPE_T)u1PidType,
                                          u1PidIdx);

    LOG(6, "prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = %ld Key Pts = 0x%08x Dmx available size = 0x%08x\n",
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey,
        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts,
        u4DmxAvailSize);

    if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > u4DmxAvailSize)
    {
        LOG(7, "Demuxer Buffer is not enough Hdr type = %x u4DmxAvailSize = %ld  frame size = %ld.\n",
            u1HdrType,
            u4DmxAvailSize,
            prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

        x_thread_delay(1);
        prSwdmxInst->fgFifoFull = TRUE;
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    // Send sequence header for decoder's initialization?
    if (((*pu4CurIdx) < prIdxTbl->ui4_number_of_entry)&& !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
    {
        if (u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
            //UCHAR *pBuf;
            UINT16 u2MaxQNum, u2CurQNum;
            UINT16 u2DmxPictureQSize,u2DmxMaxQSize;

            VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);

            // Temporally use 30 for fps
            // Should use fps in the fucture.
            if (u2CurQNum > prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps)
            {
                x_thread_delay(1);
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if (u2DmxPictureQSize > (u2DmxMaxQSize - 256))
            {
                x_thread_delay(1);
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if ((u2DmxPictureQSize + u2CurQNum) >= u2MaxQNum)
            {
                x_thread_delay(1);
                return eSWDMX_HDR_PRS_SUCCEED;
            }

            if (((UINT64)prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset + prIdxTbl->ui8_base_offset +
                    prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size)> (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8_range_sz))
            {
                LOG(5, "Video Offset is larger than range size\n");

                if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) && (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos == FALSE))
                {
                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
                    {
                        _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
                    }

                    if (_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                    }

                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
                }
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

            LOG(7, "prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid %ld  ui4_size = 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid, prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);
            LOG(7, "ui8_base_offset + ui4_relative_offset = %08x.\n", prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset);

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsneedQueryVdecInfo)
            {
                VDEC_HDR_INFO_T rHdrInfo;

                if(!VDEC_QueryInfo(prSwdmxInst->u1VdecId,&rHdrInfo))
                {
                    LOG(2, "VDEC_QueryInfo failed!!!\n");
                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                }

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq = rHdrInfo.fgProgressiveSeq;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgIsneedQueryVdecInfo = FALSE;
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
            {
                u4Index = *pu4CurIdx;
                prVidIdxTbl = prIdxTbl;
                u4SendFrameCount = 1;
            }
            else
            {
                UINT32 i = 0;

                u4Index = 0xFFFFFFFF;
                prVidIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
                for ( i = 0; i < prVidIdxTbl->ui4_number_of_entry; i++ )
                {
                    if (prVidIdxTbl->ui8_base_offset + prVidIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_relative_offset ==
                            prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset)
                    {
                        u4Index = i;
                    }
                }

                if ( u4Index == 0xFFFFFFFF || u4Index >= prVidIdxTbl->ui4_number_of_entry - 1 )
                {
                    //need new range containing target PTS
                    if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange)
                    {
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }

                    LOG(0, "Key: Need New Range\n");
                    prRangeList->u4CurPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_SKIP;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgWaitNewRange = TRUE;

                    return eSWDMX_HDR_PRS_SUCCEED;
                }

                u4SendFrameCount = 2;
            }

            if (1) /*(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264 ||
                *pu4CurIdx!=prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSentKey)*/
            {
                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes == 0 && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame == TRUE)
                {
                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
                    {
                        u4ReadSize = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
                    }
                    else
                    {
                        u4ReadSize = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index + 1].ui4_relative_offset -
                                     prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset +
                                     prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index + 1].ui4_size;
                    }

                    if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                                         u4ReadSize,
                                         (UINT64)((prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset)+(prVidIdxTbl->ui8_base_offset)),
                                         &rFeederCond,
                                         prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID) == FEEDER_E_BUFFER_FULL)
                    {
                        LOG(3, "Feeder's buffer is full\n");
                        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxInst->rFeederInfo.u4WriteAddr));
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }
                    else
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID++;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes++;
                    }
                }

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame == TRUE)
                {
                    if (!_SWDMX_Mp4GetDataFromFeeder(prSwdmxInst, &rFeederReqData))
                    {
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }
                    
                    fgIsNewFrame = TRUE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes--;

                    // Feeder add error code for us. u4WriteAddr = 0 is error
                    if(rFeederReqData.u4WriteAddr == 0)
                    {
                        LOG(1, "_SWDMX_DeliverMp4Keyframe Mp4 Fd u4WriteAddr = 0.\n");

                        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4FeederInvalidCount++;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                        return eSWDMX_HDR_PRS_FAIL;
                    }

                    if (rFeederReqData.u4ReadSize <= rFeederReqData.u4AlignOffset ||
                            prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > (rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset))
                    {
                        LOG(3, "_SWDMX_DeliverMp4Keyframe rFeederReqData.u4ReadSize <= rFeederReqData.u4AlignOffset\n");
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }

                    if(rFeederReqData.u4WriteAddr+rFeederReqData.u4AlignOffset>=prSwdmxInst->rFeederInfo.u4EndAddr)
                    {
                        u4WriteAddr = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr+(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset-prSwdmxInst->rFeederInfo.u4EndAddr));
                    }
                    else
                    {
                        u4WriteAddr = VIRTUAL(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset);
                    }

                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSentKey=*pu4CurIdx;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4RecFeederWPtr = u4WriteAddr;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = u4WriteAddr;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgRequestData = FALSE;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder+= rFeederReqData.u4ReadSize -rFeederReqData.u4AlignOffset;
                }
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4RecFeederWPtr;
            }

            prTmpSample = &prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index];
            u8TmpSampleOffset = prVidIdxTbl->ui8_base_offset + prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset;
            prSwdmxMp4Info->rPrevSample.ui4_size = 0;
            prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

            u4LeftBytesInFeeder = prSwdmxMp4Info->u4LeftBytesInFeeder;
            u4CurDmxFeederRPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr;
            
            do
            {
                prTmpSample = &prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index];
                u8TmpSampleOffset = prVidIdxTbl->ui8_base_offset + prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset;

                if (u8TmpSampleOffset >= prSwdmxMp4Info->u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size)
                {
                    u8WriteOffset = u8TmpSampleOffset - (prSwdmxMp4Info->u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size);
                    if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
                    {
                        au8WriteOffset[2-u4SendFrameCount] = u8WriteOffset;
                        aui4FrameSize[2-u4SendFrameCount] = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
                    }
                    else 
                    {
                        au8WriteOffset[0] = u8WriteOffset;
                        aui4FrameSize[0] = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
                    }
                }
                else
                {
                    prSwdmxMp4Info->u8CurFilepos = u8TmpSampleOffset;
                    fgHasBreak = TRUE;
                    break;
                }

                if((u8WriteOffset + prTmpSample->ui4_size) > (prSwdmxMp4Info->u4LeftBytesInFeeder))
                {
                    LOG(3, "fgFeederInvalid , left byte too small\n");
                    fgHasBreak = TRUE;
                    break;
                }
                u4CurDmxFeederRPtr += u8WriteOffset;
                if(u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    u4CurDmxFeederRPtr =
                        u4CurDmxFeederRPtr -
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                }

                if (fgIsNewFrame == TRUE)
                {
                    //replace length with start code
                    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264 ||
                            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H265)
                    {
                        UCHAR* pucBuf = NULL;

                        pucFdrBuf = (UCHAR*)u4CurDmxFeederRPtr;
                        pucBuf       = pucFdrBuf;
                        u4FrmSize = 0;

                        while(u4FrmSize < prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size)
                        {
                            u4NaluSize = 0;
                            //insert start code
                            for(i = 0; i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1; i++)
                            {
                                u4NaluSize += (UINT32)*pucFdrBuf;
                                u4NaluSize <<= 8;
                                *pucFdrBuf++ = 0;
                                //check ring buffer
                                if((UINT32)pucFdrBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                                {
                                    pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                                }
                            }
                            u4NaluSize += (UINT32)*pucFdrBuf;
                            if (u4NaluSize == 0)
                            {
                                (*pu4CurIdx)++;
                                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                                return eSWDMX_HDR_PRS_SUCCEED;
                            }

                            *pucFdrBuf++ = 1;
                            if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 < H264_STARTCODE_LEN) &&
                                    (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 > 0))
                            {
                                UINT32   u4Loop = 0;
                                UINT32   u4DataSize = 0;

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                                HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                                     (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
                                HalFlushInvalidateDCache();
#endif

                                for(u4Loop=0; u4Loop<(H264_STARTCODE_LEN-
                                                      prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1); u4Loop++)
                                {
                                    if((UINT32)pucBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr))
                                    {
                                        pucBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr-1);
                                    }
                                    else
                                    {
                                        pucBuf--;
                                    }
                                    *pucBuf = 0;
                                }
                                u4DataSize = u4NaluSize + (H264_STARTCODE_LEN-
                                                           prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1)+
                                             (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 + 1);

                                rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                                rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                                rDmxMMData.u4StartAddr = (UINT32)pucBuf;
                                rDmxMMData.u4FrameSize = u4DataSize;
                                rDmxMMData.u4Pts = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;
                                rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;
                                u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                                                      DMX_PID_TYPE_ES_VIDEO,
                                                                      prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                                if(u4DataSize >= u4DmxAvailSize)
                                {
                                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                                    prSwdmxInst->fgFifoFull = TRUE;
                                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                                    return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
                                }
                                //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                                if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                                {
                                    LOG(0, "Demuxer fails to move data.\n");
                                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                                }
                            }
                            //check ring buffer
                            if((UINT32)pucFdrBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                            {
                                pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                            }
                            // nalu size
                            u4FrmSize += ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 + 1) + u4NaluSize);
                            pucFdrBuf += u4NaluSize;
                            //check ring buffer
                            if((UINT32)pucFdrBuf >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                            {
                                pucFdrBuf =
                                    (UCHAR*)((UINT32)pucFdrBuf -
                                             VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                             VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                            }
                            pucBuf = pucFdrBuf;
                        }
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                        HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                             (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
                        HalFlushInvalidateDCache();
#endif
                    }
                }
                
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid++;

                if(((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts>>32) != 0xFFFFFFFF)
                        || ((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts) != 0xFFFFFFFF))
                {
                    UINT32 u4Pts, u4CurSTC;

                    u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                    if(u4Pts > (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts)
                    {
                        STC_SetStcValue(prSwdmxInst->u1StcId, u4Pts - 18000);
                        u4CurSTC = STC_GetStcValue(prSwdmxInst->u1StcId);
                        LOG(1, "Got Vid T PTS 0x%x, STC 0x%x, Seek 0x%x\n",
                            u4Pts, u4CurSTC, (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts = 0xFFFFFFFF;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts <<= 32;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VSeekPts += 0xFFFFFFFF;
                    }
                }

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastVidPts = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prVidIdxTbl->ui8_base_offset +
                        prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_relative_offset+
                        prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;

                if((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType != ENUM_SWDMX_VDEC_H264)||
                        ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264) &&
                         (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 == H264_STARTCODE_LEN)) )
                {
                    rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                    rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                    rDmxMMData.u4StartAddr = u4CurDmxFeederRPtr;
                    rDmxMMData.u4FrameSize = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
                    rDmxMMData.u4Pts = prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts;
                    rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;

                    /*To avoid calculation error.*/
                    if (rDmxMMData.u4StartAddr > rDmxMMData.u4BufEnd || rDmxMMData.u4StartAddr < rDmxMMData.u4BufStart)
                    {
                        LOG(3,"BufStart=0x%x,BufEnd=0x%x,StartAddr=0x%x,Size=0x%x,u4pts=%d, RPtr=0x%x\n",
                            rDmxMMData.u4BufStart,rDmxMMData.u4BufEnd,rDmxMMData.u4StartAddr,rDmxMMData.u4FrameSize,rDmxMMData.u4Pts,
                            rDmxMMData.u1Idx,prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr);
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                    
                    //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                    {
                        LOG(1, "Demuxer fails to move data.\n");
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                }

                if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
                {
                    u4LeftBytesInFeeder -=
                        ((UINT32)u8WriteOffset + prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size);

                    u4CurDmxFeederRPtr += prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_size;
                    if(u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        u4CurDmxFeederRPtr =
                            u4CurDmxFeederRPtr -
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                    }           
                }

                //record the previous sample data
                x_memcpy(&(prSwdmxMp4Info->rPrevSample),
                         prTmpSample,
                         sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));

                prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

                LOG(4, "%s, index = %u, u4CurVidPts = 0x%08x\n", __FUNCTION__, u4Index, prVidIdxTbl->u.pt_pb_idx_tbl_entry[u4Index].ui4_pts);

                u4SendFrameCount--;
                u4Index++; 
            }
            while( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264 && u4SendFrameCount > 0 && !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq );

            if (fgIsNewFrame == TRUE)
            {
                fgIsNewFrame = FALSE;
            }
            
            while (!_SWDMX_Mp4CalculateRepeatFrame(prSwdmxInst, i4Speed))
            {
                BOOL fgNextKeyTable = FALSE;                
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame = TRUE;

                fgNextKeyTable = _SWDMX_Mp4CheckNextKeyTable(prSwdmxInst,i4Speed,pu4CurIdx);

                if(!fgNextKeyTable)
                {
                    if (i4Speed > 2000)
                    {
                        (*pu4CurIdx)++;
                    }
                    else if ((i4Speed < 0) &&(*pu4CurIdx > 0))
                    {
                        (*pu4CurIdx)--;
                    }
                }

                // The last key frame
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame == 0)
                {
                    if ((i4Speed > 2000) &&
                            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry) &&
                            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry - 1) &&
                            !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
                    {
                        LOG(5, "Fast Forward _SWDMX_Mp4HdrPrsUseIdx Deliver Video Eos\n");
                        if (!_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                        {
                            LOG(3, "Fail to send Video Eos\n");
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                        }
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
                        return eSWDMX_HDR_PRS_SUCCEED;
                    }
                    else if ((i4Speed < 0) &&
                             (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey == 0) &&
                             (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx == 0) &&
                             !prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos)
                    {
                        if (!_SWDMX_SendMp4Eos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                        {
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                        }
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgDeliverVEos = TRUE;
                        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
                        return eSWDMX_HDR_PRS_SUCCEED;
                    }
                }
            }
            /*Update readPointer before request next key frame.*/
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgNeedToReqKeyFrame == TRUE && fgHasBreak == FALSE)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
                {
                    prSwdmxMp4Info->u4LeftBytesInFeeder -=
                        ((UINT32)u8WriteOffset + aui4FrameSize[0]);

                    prSwdmxMp4Info->u4CurDmxFeederRPtr += (UINT32)u8WriteOffset + aui4FrameSize[0];
                    if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        prSwdmxMp4Info->u4CurDmxFeederRPtr =
                            prSwdmxMp4Info->u4CurDmxFeederRPtr -
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                    }
                    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));
                }
                else 
                {
                    for (i4_i = 0; i4_i<2; i4_i ++)
                    {
                        prSwdmxMp4Info->u4LeftBytesInFeeder -=
                        ((UINT32)au8WriteOffset[i4_i] + aui4FrameSize[i4_i]);

                        prSwdmxMp4Info->u4CurDmxFeederRPtr += (UINT32)au8WriteOffset[i4_i] + aui4FrameSize[i4_i];
                        if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                        {
                            prSwdmxMp4Info->u4CurDmxFeederRPtr =
                                prSwdmxMp4Info->u4CurDmxFeederRPtr -
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        }
                    }
                    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));
                }
            }
        }
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

static BOOL _SWDMX_Mp4ResetFeeder(SWDMX_INFO_T* prSwdmxInst)
{
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }

    LOG(5, "_SWDMX_Mp4ResetFeeder Cur Fed's WPtr = 0x%08x\n", prSwdmxInst->rFeederInfo.u4WriteAddr);
    prSwdmxMp4Info->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr ;

    // Update read ptr to feeder
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr));
    return TRUE;
}

#ifdef HAN_MVC_HEADER //MVC Handle

SWDMX_HDR_PRS_STATE_T  _SWDMX_H264HeaderHandle(SWDMX_INFO_T *prSwdmxInst, UCHAR *pucHdrBuf, UCHAR *pucFdrBuf, UINT8 u1HdrType)
{
    UINT8 u1NumOfPS = 0;
    UINT8 u1PSNum = 0;
    UINT8 i = 0;
    UINT16 u2PSLength = 0;
    DMX_MM_DATA_T rDmxMMData;
    if(pucHdrBuf == NULL || pucFdrBuf == NULL || prSwdmxInst == NULL)
    {
        LOG(0,"header pointer is NULL\n");
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    pucHdrBuf += 4; //skip unused info
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 = (*pucHdrBuf++) & 0x3;
    for(u1PSNum = 0; u1PSNum < 2; u1PSNum++)    // 0 for SPS, 1 for PPS
    {
        if(u1PSNum == 0)
        {
            u1NumOfPS = (*pucHdrBuf++) & 0x1f;
        }
        else
        {
            u1NumOfPS = *pucHdrBuf++;
        }
        for (i = 0; i < u1NumOfPS; i++)
        {
            u2PSLength = (UINT16)(*pucHdrBuf++);
            u2PSLength <<= 8;
            u2PSLength += (UINT16)(*pucHdrBuf++);

            //handle ring buffer
             //if(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) <= (((UINT32)pucFdrBuf + (UINT32)u2PSLength) + 4))
            //{
            //    pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            //}
            //set move data parameter
            rDmxMMData.u4BufStart = VIRTUAL((UINT32)pucFdrBuf);
            rDmxMMData.u4BufEnd = VIRTUAL((UINT32)pucFdrBuf + MP4_SEQHEADER_FEEDER_SIZE);
            rDmxMMData.u4StartAddr = (UINT32)pucFdrBuf;
            rDmxMMData.u4FrameSize = (UINT32)u2PSLength + 4;    // 4 for start code
            rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;
            LOG(3,"_SWDMX_H264HeaderHandle ==The u4BufStart is 0x%x,the u4BufEnd is 0x%x,The u4StartAddr is 0x%x,The u4FrameSize is %d==\n",
                (int)rDmxMMData.u4BufStart,(int)rDmxMMData.u4BufEnd,(int)rDmxMMData.u4StartAddr,(int)rDmxMMData.u4FrameSize);
           
            //insert start code
            *pucFdrBuf = 0;
            *(pucFdrBuf + 1) = 0;
            *(pucFdrBuf + 2) = 0;
            *(pucFdrBuf + 3) = 1;    
            //copy SPS or PPS
            x_memcpy((VOID*)(pucFdrBuf+4), (VOID*)pucHdrBuf, u2PSLength);
            pucHdrBuf += u2PSLength;
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
            HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                 (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
            HalFlushInvalidateDCache();
#endif

            //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
            {
                LOG(1, "Demuxer fails to move sequence header.\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

#endif

SWDMX_HDR_PRS_STATE_T  _SWDMX_H265HeaderHandle(SWDMX_INFO_T *prSwdmxInst, UCHAR *pucHdrBuf, UCHAR *pucFdrBuf, UINT8 u1HdrType)
{
    UINT8  u1NumOfArrays = 0;
    UINT8  u1ArrayIdx = 0;
    UINT16 u2NumNalus = 0;
    UINT16 u2NalusIdx = 0;
    UINT16 u2NalUnitLength = 0;

    DMX_MM_DATA_T rDmxMMData;

    if(pucHdrBuf == NULL || pucFdrBuf == NULL || prSwdmxInst == NULL)
    {
        LOG(0,"header pointer is NULL\n");
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }
    LOG(0, "----------------H265HeaderHandle parsing begin------------\n");
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    pucHdrBuf += 21; //skip unused info

    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 = 3/**pucHdrBuf*/;

    pucHdrBuf++;

    u1NumOfArrays = *pucHdrBuf++;
    LOG(0, "u1NumOfArrays: %d\n", u1NumOfArrays);

    for(u1ArrayIdx = 0; u1ArrayIdx<u1NumOfArrays; u1ArrayIdx++)
    {
        LOG(0, "u1ArrayIdx: %d\n", u1ArrayIdx);
        pucHdrBuf++; //skip 1byte for arraycompletence/reserve/nal_type

        u2NumNalus = (UINT16)(*pucHdrBuf++);
        u2NumNalus <<= 8;
        u2NumNalus += (UINT16)(*pucHdrBuf++);

        LOG(0, "u2NumNalus: %d\n", u2NumNalus);

        for (u2NalusIdx = 0; u2NalusIdx < u2NumNalus; u2NalusIdx++)
        {
            u2NalUnitLength = (UINT16)(*pucHdrBuf++);
            u2NalUnitLength <<= 8;
            u2NalUnitLength += (UINT16)(*pucHdrBuf++);

            LOG(0, "u2NalUnitLength: %d\n", u2NalUnitLength);
            //handle ring buffer
            //if(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) <= (((UINT32)pucFdrBuf + (UINT32)u2NalUnitLength) + 4))
            //{
            //    pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            //}

            //set move data parameter
            rDmxMMData.u4BufStart = VIRTUAL((UINT32)pucFdrBuf);
            rDmxMMData.u4BufEnd = VIRTUAL((UINT32)pucFdrBuf + MP4_SEQHEADER_FEEDER_SIZE);
            rDmxMMData.u4StartAddr = (UINT32)pucFdrBuf;
            rDmxMMData.u4FrameSize = (UINT32)u2NalUnitLength + 4;    // 4 for start code
            rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;
            LOG(3,"_SWDMX_H264HeaderHandle ==The u4BufStart is 0x%x,the u4BufEnd is 0x%x,The u4StartAddr is 0x%x,The u4FrameSize is %d==\n",
                (int)rDmxMMData.u4BufStart,(int)rDmxMMData.u4BufEnd,(int)rDmxMMData.u4StartAddr,(int)rDmxMMData.u4FrameSize);
           
            //insert start code
            *pucFdrBuf = 0;
            *(pucFdrBuf + 1) = 0;
            *(pucFdrBuf + 2) = 0;
            *(pucFdrBuf + 3) = 1;    

            //copy data
            x_memcpy((VOID*)(pucFdrBuf + 4), (VOID*)pucHdrBuf, u2NalUnitLength);
            pucHdrBuf += u2NalUnitLength;

            {
                UINT32 i = 0;

                CHAR *pBuf = (CHAR*)rDmxMMData.u4StartAddr;
                LOG(0, "data: ");
                for(i=0; i<rDmxMMData.u4FrameSize; i++)
                {
                    printf("%02x ", pBuf[i]);

                    if((i+1)%16 == 0)
                    {
                        printf("\n");
                    }
                }
                printf("\n");
            }

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
            HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                 (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
            HalFlushInvalidateDCache();
#endif

            //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
            {
                LOG(1, "Demuxer fails to move sequence header.\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }

    LOG(0, "----------------H265HeaderHandle parsing end--------------\n");

    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T  _SWDMX_BeforeDeliverUseIdx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1HdrType)
{
    UCHAR *pucHdrBuf = NULL;
    UCHAR *pucFdrBuf = NULL;
    UINT32 u4RequestSize = MP4_SEQHEADER_FEEDER_SIZE;
    UINT32 u4Align = 16;
#ifndef HAN_MVC_HEADER
    UINT8 u1NumOfPS = 0, u1PSNum = 0;
    UINT8 i = 0;
    UINT16 u2PSLength = 0;
#endif
    DMX_MM_DATA_T rDmxMMData;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
	LOG(0, "_SWDMX_BeforeDeliverUseIdx\n");
    
    if (0 == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderFdrPtr)
    {
        LOG(3,"Should Set Video Seq Header Feeder Buffer in Init Function!\n");
        _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderFdrPtr, &u4RequestSize, &u4Align);
    }
    pucFdrBuf = (UCHAR*) VIRTUAL (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderFdrPtr);
    
    //Currently, only need to set MP4_SRC_3D_FRM_ALT to decoder.
    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4Mp4Src3DType == MP4_SRC_3D_FRM_ALT)
    {
        VDEC_SetMMParam(prSwdmxInst->u1VdecId, VDEC_MM_3D_INFO, VDEC_3D_BIVL, 0, 0);
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H265)
    {
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr != (UINT32)NULL)
        {
            pucHdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr);
            
            if(_SWDMX_H265HeaderHandle(prSwdmxInst, pucHdrBuf, pucFdrBuf,u1HdrType)!= eSWDMX_HDR_PRS_SUCCEED)
            {
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_H264)
    {
#ifdef HAN_MVC_HEADER
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr != (UINT32)NULL)
        {
            pucHdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr);
             
            if(_SWDMX_H264HeaderHandle(prSwdmxInst, pucHdrBuf, pucFdrBuf,u1HdrType)!= eSWDMX_HDR_PRS_SUCCEED)
            {
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr != (UINT32)NULL)
        {
            LOG(0,"This would not happen now !\n");           
            pucHdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderSecPtr);
            //This would be changed!            
            pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            if(_SWDMX_H264HeaderHandle(prSwdmxInst, pucHdrBuf, pucFdrBuf,u1HdrType)!= eSWDMX_HDR_PRS_SUCCEED)
            {
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
#else
        LOG(0,"This would not happen now 2!\n");
        pucHdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr);
        //This would be changed!
        pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
        pucHdrBuf += 4; //skip unused info
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1StartCodeLenMinus1 = (*pucHdrBuf++) & 0x3;
        for(u1PSNum = 0; u1PSNum < 2; u1PSNum++)    // 0 for SPS, 1 for PPS
        {
            if(u1PSNum == 0)
            {
                u1NumOfPS = (*pucHdrBuf++) & 0x1f;
            }
            else
            {
                u1NumOfPS = *pucHdrBuf++;
            }
            for (i = 0; i < u1NumOfPS; i++)
            {
                u2PSLength = (UINT16)(*pucHdrBuf++);
                u2PSLength <<= 8;
                u2PSLength += (UINT16)(*pucHdrBuf++);

                //handle ring buffer
                if(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) <= (((UINT32)pucFdrBuf + (UINT32)u2PSLength) + 4))
                {
                    pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                }
                //set move data parameter
                rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rDmxMMData.u4StartAddr = (UINT32)pucFdrBuf;
                rDmxMMData.u4FrameSize = (UINT32)u2PSLength + 4;    // 4 for start code
                rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;

                //insert start code
                *pucFdrBuf++ = 0;
                *pucFdrBuf++ = 0;
                *pucFdrBuf++ = 0;
                *pucFdrBuf++ = 1;
                //copy SPS or PPS
                x_memcpy((VOID*)pucFdrBuf, (VOID*)pucHdrBuf, u2PSLength);
                pucFdrBuf += u2PSLength;
                pucHdrBuf += u2PSLength;
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                     (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
                HalFlushInvalidateDCache();
#endif

                //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                {
                    LOG(1, "Demuxer fails to move sequence header.\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
        }
#endif
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType == ENUM_SWDMX_VDEC_MPEG4)
    {
        x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr),
                 (VOID*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr),
                 prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength);

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                             (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
        HalFlushInvalidateDCache();
#endif

        rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
        rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
        rDmxMMData.u4StartAddr = VIRTUAL(prSwdmxInst->rFeederInfo.u4WriteAddr);
        rDmxMMData.u4FrameSize = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength;
        rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;

        //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
        {
            LOG(1, "Demuxer fails to move sequence header.\n");
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
    }

	prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NumSentVid++;

	return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverMp4DataUseIdx(SWDMX_INFO_T* prSwdmxInst,
        INT32 i4Speed,
        MM_RANGE_ELMT_IDXTBL_T *prIdxTbl,
        UINT32* pu4CurIdx,
        UINT8 u1HdrType)
{
    UINT32 u4DmxAvailSize = 0;
    UINT32 u4DmxVdecAvailSize = 0;
    UINT32 u4DmxAdecAvailSize = 0;
    UINT32 u4DmxSubAvailSize = 0;

    DMX_MM_DATA_T rDmxMMData;

    UINT32 u4NaluSize = 0, u4FrmSize = 0;

    UCHAR *pucFdrBuf = NULL;

    UINT64 u8WriteOffset = 0;

    int i = 0;

    MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T *prTmpSample;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    UINT64 u8TmpSampleOffset = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

    /*Need to record current sample.*/
    prTmpSample = &prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx];
    u8TmpSampleOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset;

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
    
    /*Get FIFO available size.*/
    u4DmxVdecAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                         DMX_PID_TYPE_ES_VIDEO,
                         prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
    u4DmxAdecAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                         DMX_PID_TYPE_ES_AUDIO,
                         prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
    u4DmxSubAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                        DMX_PID_TYPE_ES_OTHER,
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid);

    if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size == 0)
    {
        (*pu4CurIdx)++;
        return eSWDMX_HDR_PRS_FAIL;
    }

    if(u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        u4DmxAvailSize = u4DmxVdecAvailSize;
    }
    else if(u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        u4DmxAvailSize = u4DmxAdecAvailSize;

        if ((prSwdmxMp4Info->u4ADecType == ENUM_SWDMX_ADEC_AAC) &&
                (u4DmxAvailSize > AAC_FAKE_HDR_SIZE))
        {
            u4DmxAvailSize -= AAC_FAKE_HDR_SIZE;
        }
    }
    else if(u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        u4DmxAvailSize = u4DmxSubAvailSize;
    }
    else
    {
        LOG(3, "u1HdrType ERROR!\n");
        return eSWDMX_HDR_PRS_FAIL;
    }
    
    /*If sample size is larger than FIFO available size.*/
    if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > u4DmxAvailSize)
    {
        if (prSwdmxMp4Info->fgVDecIsPause)
	    {
	        _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
	        prSwdmxMp4Info->fgVDecIsPause = FALSE;
	    }			
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type ==
                MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {        
            if (u1HdrType == eSWDMX_MEDIA_AUDIO && 
                prSwdmxMp4Info->fgEnVideo && 
                !prSwdmxMp4Info->fgPartialRangeDone)
            {
                prSwdmxMp4Info->fgBlockAud = TRUE;
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockSub = FALSE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            }
            if (u1HdrType == eSWDMX_MEDIA_VIDEO &&
                    prSwdmxMp4Info->fgEnAudio &&
                    !prSwdmxMp4Info->fgPartialRangeDone)
            {
                UINT32 u4AudAvailSize = 0;
                VDEC_ES_INFO_T* prVdecEsInfo = NULL;

                prVdecEsInfo = _VDEC_GetEsInfo(prSwdmxInst->u1VdecId);
                _SWDMX_Mp4GetInfo(0, eSWDMX_GET_AUDIO_ACTIVE_PIDX, &u4AudAvailSize, 0, 0);

                if (prVdecEsInfo->u2Count > (prSwdmxMp4Info->u4VidFps >> 1) ||
                        u4AudAvailSize < 2048)
                {
                    prSwdmxMp4Info->fgBlockAud = FALSE;
                    prSwdmxMp4Info->fgBlockVid = TRUE;
                    prSwdmxMp4Info->fgBlockSub = FALSE;
                    _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                }
                else
                {
                    LOG(5, "[****] u4DmxAvailSize:0x%08x, %d, 0x%08x\n", u4DmxAvailSize, prVdecEsInfo->u2Count, u4AudAvailSize);
                    x_thread_delay(5);
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }
        }

        if (u1HdrType == eSWDMX_MEDIA_AUDIO &&
                !prSwdmxMp4Info->fgEnVideo)
        {
            LOG(9, "Trigger audio by _SWDMX_DeliverMp4DataUseIdx\n");
            AUD_MMAoutEnable(0, TRUE);

            if(prSwdmxMp4Info->fgAudIsReady)
            {
                STC_StartStc(prSwdmxInst->u1StcId);
                prSwdmxMp4Info->fgAudIsReady = FALSE;
            }
        }

        if ( u1HdrType == eSWDMX_MEDIA_SUBTITLE )
        {
            LOG(5, "Subtitle Dmx Buffer is not enough\n");
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockSub = TRUE;

            x_thread_delay(1);

            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
        }

        LOG(6, "Dmx Buffer is not enough. Header Type = %x\n", u1HdrType);
        x_thread_delay(1);
        prSwdmxInst->fgFifoFull = TRUE;
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }
    else
    {
        prSwdmxInst->fgFifoFull = FALSE;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }
    
    /*If feeder data is not enough.*/
    if(prTmpSample->ui4_size > prSwdmxMp4Info->u4LeftBytesInFeeder)
    {
        prSwdmxMp4Info->fgFeederLeftNotEnough = TRUE;
        LOG(3, "left in feeder is not enough, hdr type = %d, sample size = %d, left bytes = %d\n",
            u1HdrType, prTmpSample->ui4_size, prSwdmxMp4Info->u4LeftBytesInFeeder);
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }
    else
    {
        prSwdmxMp4Info->fgFeederLeftNotEnough = FALSE;
    }

    /*If QFIFO is full.*/
    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        UINT16 u2MaxQNum, u2CurQNum;
        UINT16 u2DmxPictureQSize,u2DmxMaxQSize;

        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
        if ((u2CurQNum > (u2MaxQNum - 256)) ||
                (u2CurQNum > (prSwdmxMp4Info->u4VidFps * 10)))
        {
            prSwdmxInst->fgFifoFull = TRUE;
            prSwdmxMp4Info->fgBlockSub = FALSE;

            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);
            //x_thread_delay(10);
            //return eSWDMX_HDR_PRS_SUCCEED;
        }
        else if (u2DmxPictureQSize > (u2DmxMaxQSize - 256))
        {
            prSwdmxInst->fgFifoFull = TRUE;

            LOG(7, "u2DmxPictureQSize = %ld, u2DmxMaxQSize = %ld .\n", u2DmxPictureQSize, u2DmxMaxQSize);
            //x_thread_delay(10);
            //return eSWDMX_HDR_PRS_SUCCEED;
        }
        else if ((u2DmxPictureQSize + u2CurQNum) >= u2MaxQNum)
        {
            prSwdmxInst->fgFifoFull = TRUE;

            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld, u2DmxPictureQSize = %ld .\n", u2CurQNum, u2MaxQNum, u2DmxPictureQSize);
            //x_thread_delay(10);
            //return eSWDMX_HDR_PRS_SUCCEED;
        }
        else
        {
            prSwdmxInst->fgFifoFull = FALSE;
        }

        if (prSwdmxInst->fgFifoFull)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo != NULL &&
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type != MM_SRC_TYPE_HIGH_SPEED_STORAGE &&
                    prSwdmxMp4Info->fgEnAudio &&
                    prSwdmxMp4Info->u4CurAudPts < prSwdmxMp4Info->u4CurVidPts)
            {
                prSwdmxMp4Info->fgBlockAud = FALSE;
                prSwdmxMp4Info->fgBlockVid = TRUE;
                prSwdmxInst->fgFifoFull = FALSE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                LOG(5, "Vfifo full, send audio in the same timeline--\n");
            }
			else
			{
				if (prSwdmxMp4Info->fgVDecIsPause)
	            {
	                _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
	                prSwdmxMp4Info->fgVDecIsPause = FALSE;
	            }
			}

            LOG(7, "Vfifo full\n");
            x_thread_delay(10);
            return eSWDMX_HDR_PRS_SUCCEED;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type ==
                MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {
            if (prSwdmxMp4Info->fgEnAudio &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurVidPts > (prSwdmxMp4Info->u4CurAudPts + MP4_ONE_SEC_DURATION)))
            {
                LOG(5, "Video is enough, send audio data in the same timeline\n");
                prSwdmxMp4Info->fgBlockVid = TRUE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
                prSwdmxMp4Info->fgBlockSub = FALSE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            }
            else if(prSwdmxMp4Info->fgEnAudio &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurVidPts > prSwdmxMp4Info->u4CurAudPts))
            {
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
                prSwdmxMp4Info->fgBlockSub = FALSE;
            }
        }
        else if(prSwdmxInst->eSeekType != eSWDMX_TYPE_TIME_SEEK)
        {
            if (prSwdmxMp4Info->fgEnAudio &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurVidPts > (prSwdmxMp4Info->u4CurAudPts + 4 * MP4_ONE_SEC_DURATION)))
            {
                LOG(5, "NETWORK Video is enough, send audio data in the same timeline\n");
                prSwdmxMp4Info->fgBlockVid = TRUE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            }
            else if(prSwdmxMp4Info->fgEnAudio &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurVidPts > prSwdmxMp4Info->u4CurAudPts))
            {
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
            }
        }
    }
    
    /*Calculate the right sample offset.*/
    if (/*prTmpSample->ui4_relative_offset*/ u8TmpSampleOffset >=
            (/*prSwdmxMp4Info->rPrevSample.ui4_relative_offset*/prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size))
    {
        u8WriteOffset =
            /*prTmpSample->ui4_relative_offset*/ u8TmpSampleOffset -
            (/*prSwdmxMp4Info->rPrevSample.ui4_relative_offset*/prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8PrevSampleOffset + prSwdmxMp4Info->rPrevSample.ui4_size);
    }
    else
    {
        prSwdmxMp4Info->u8CurFilepos = u8TmpSampleOffset;/*prTmpSample->ui4_relative_offset*/;
        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
        if(u1HdrType == eSWDMX_MEDIA_SUBTITLE)
        {
            prSwdmxMp4Info->fgInvalidForSub = TRUE;
        }
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }
    /*
        if(prTmpSample->ui4_size > prSwdmxMp4Info->u4LeftBytesInFeeder)
        {
            prSwdmxMp4Info->fgFeederLeftNotEnough = TRUE;
            LOG(3, "left in feeder is not enough, hdr type = %d, sample size = %d, left bytes = %d\n",
                u1HdrType, prTmpSample->ui4_size, prSwdmxMp4Info->u4LeftBytesInFeeder);
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
        else
        {
            prSwdmxMp4Info->fgFeederLeftNotEnough = FALSE;
        }
    */

    /*Normally, this means A/V/S is blocked in last loop.*/
    if((u8WriteOffset + prTmpSample->ui4_size) > (prSwdmxMp4Info->u4LeftBytesInFeeder))
    {
        //LOG(3, "fgFeederInvalid , left byte too small\n");
        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
        if(u1HdrType == eSWDMX_MEDIA_SUBTITLE)
        {
            prSwdmxMp4Info->fgInvalidForSub = TRUE;
        }
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }
    
    /*Update feeder's read pointer.*/
    prSwdmxMp4Info->u4CurDmxFeederRPtr += u8WriteOffset;
    if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxMp4Info->u4CurDmxFeederRPtr =
            prSwdmxMp4Info->u4CurDmxFeederRPtr -
            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
    }

    /*Handle conditions such as encrypt, H264, etc...*/
    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {

#if 1
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
        {
            AES_PARAM_T rAesParam;
            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;

            x_memcpy(rAesParam.au1Key, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VideoDrmKey, 16);

            if(u4Tmp+16 >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                UINT32 u4FirstPart = prSwdmxInst->rFeederInfo.u4EndAddr - u4Tmp;
                LOG(2, " Handle feeder ring buffer for video IV!!!\n");

                if(u4FirstPart > 0)
                {
                    x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, u4FirstPart);
                }

                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);

                x_memcpy(rAesParam.au1Iv + u4FirstPart, (void*)u4Tmp, 16-u4FirstPart);
            }
            else
            {
                x_memcpy(rAesParam.au1Iv, (void*)prSwdmxMp4Info->u4CurDmxFeederRPtr, 16);
            }

            u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 16;
            if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
            }

            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
            rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
            rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
            rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            rAesParam.u4DatLen  = (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size-16);;

            rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
            rAesParam.fgEncrypt = FALSE;

            GCPU_Cmd(0, GCPU_AES, &rAesParam);
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
        {
            AES_PARAM_T rAesParam;
            UCHAR* pucBuf = NULL;

            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;

            if (u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
            }

            pucBuf = (UCHAR*)u4Tmp;

            if ((*pucBuf) >> 7)
            {
                x_memcpy(rAesParam.au1Key, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VideoDrmKey, 16);

                u4Tmp++;
                if (u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }

                if(u4Tmp+16 >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    UINT32 u4FirstPart = prSwdmxInst->rFeederInfo.u4EndAddr - u4Tmp;
                    LOG(2, " Handle feeder ring buffer for video IV!!!\n");

                    if(u4FirstPart > 0)
                    {
                        x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, u4FirstPart);
                    }

                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);

                    x_memcpy(rAesParam.au1Iv + u4FirstPart, (void*)u4Tmp, 16-u4FirstPart);
                }
                else
                {
                    x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, 16);
                }

                u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 17;
                if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }

                rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DatLen  = (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size-17);;

                rAesParam.eMode = (prSwdmxMp4Info->eCryptMode == eSWDMX_CRYPT_MODE_AES_CBC)? BLOCK_CIPHER_MODE_CBC : BLOCK_CIPHER_MODE_CTR;
                rAesParam.fgEncrypt = FALSE;

                GCPU_Cmd(0, GCPU_AES, &rAesParam);
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidEncTbl)
        {
            AES_PARAM_T rAesParam;
            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;
            UINT32 u4SubCount = 0;
            UINT32 u4ClearLen = 0;
            UINT32 u4EncryptLen = 0;
            UINT32 u4ClearLen2 = 0;
            UINT32 u4EncryptLen2 = 0;
            UINT32 u4AllLen = 0;
            UINT32 u4Align = 16;
            UINT32 u4TempPtr = 0;

            MM_RANGE_ELMT_ENCTBL_T *prVidEncTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidEncTbl;

            x_memcpy(rAesParam.au1Key, prVidEncTbl->aui1_key, 16);

            x_memcpy(rAesParam.au1Iv, prVidEncTbl->pt_sample_entry[*pu4CurIdx].ui1_iv, 16);

            if (prVidEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt == 0)
            {
                u4ClearLen = 0;
                u4EncryptLen = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;

                rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DatLen  = u4EncryptLen;;

                rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                rAesParam.fgEncrypt = FALSE;

                LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                GCPU_Cmd(0, GCPU_AES, &rAesParam);
            }
            else
            {
                while (u4SubCount < prVidEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt && u4SubCount < 32)
                {
                    u4ClearLen = prVidEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_clear_data;
                    u4EncryptLen = prVidEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_encrypted_data;

                    if (u4EncryptLen == 0)
                    {
                        u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                        u4SubCount++;
                        continue;
                    }
                    else if ( u4EncryptLen % 16 != 0 )
                    {
                        LOG(0, "encrypt length % 16 is not 0  !!!!\n");

                        if ( u4SubCount < prVidEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt - 1 )
                        {
                            u4ClearLen2 = prVidEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_clear_data;
                            u4EncryptLen2 = prVidEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_encrypted_data;
                        }
                        else
                        {
                            LOG(0, "the last block is error!!!!!!\n");
                            break;
                        }

                        _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId, eSWDMX_GET_INTERNAL_BUF, &u4TempPtr, &u4EncryptLen, &u4Align);

                        x_memcpy((UINT8 *)u4TempPtr, (VOID *)(u4Tmp + u4ClearLen), u4EncryptLen);
                        x_memcpy((UINT8 *)(u4TempPtr + u4EncryptLen), (VOID *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), u4EncryptLen2);

                        rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                        rAesParam.u4SrcStartAddr = VIRTUAL(u4TempPtr);
                        rAesParam.u4SrcBufStart = VIRTUAL(u4TempPtr);
                        rAesParam.u4SrcBufEnd = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstStartAddr = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstBufStart = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstBufEnd = VIRTUAL(u4TempPtr);
                        rAesParam.u4DatLen  = u4EncryptLen + u4EncryptLen2;

                        rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                        rAesParam.fgEncrypt = FALSE;

                        LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                        GCPU_Cmd(0, GCPU_AES, &rAesParam);

                        x_memcpy((UINT8 *)(u4Tmp + u4ClearLen), (VOID *)u4TempPtr, u4EncryptLen);
                        x_memcpy((UINT8 *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), (VOID *)(u4TempPtr + u4EncryptLen), u4EncryptLen2);

                        u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                        u4SubCount += 2;
                    }
                    else
                    {
                        rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                        rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                        rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                        rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                        rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                        rAesParam.u4DatLen  = u4EncryptLen;

                        rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                        rAesParam.fgEncrypt = FALSE;

                        LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                        GCPU_Cmd(0, GCPU_AES, &rAesParam);

                        u4AllLen += (u4ClearLen + u4EncryptLen);
                        u4SubCount++;

                    }
                }
            }
        }
#endif
        //replace length with start code
        if (prSwdmxMp4Info->u4VDecType == ENUM_SWDMX_VDEC_H264 ||
                prSwdmxMp4Info->u4VDecType == ENUM_SWDMX_VDEC_H265)
        {
            UCHAR* pucBuf = NULL;
            UINT32 u4MarlinOffset = 0;

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
            {
                UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 16;
                u4MarlinOffset = 16;

                if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }
                pucFdrBuf = (UCHAR*)u4Tmp;
            }
            else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
            {
                UINT32 u4Tmp;
                if((*((UCHAR*)prSwdmxMp4Info->u4CurDmxFeederRPtr))>>7)
                {
                    u4MarlinOffset = 17;
                }
                u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + u4MarlinOffset;
                if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }
                pucFdrBuf = (UCHAR*)u4Tmp;
            }
            else
            {
                pucFdrBuf = (UCHAR*)prSwdmxMp4Info->u4CurDmxFeederRPtr ;
            }

            pucBuf = pucFdrBuf;
            u4FrmSize = 0;

            if (prSwdmxMp4Info->u1StartCodeLenMinus1 <= H264_STARTCODE_LEN)
            {
                while(u4FrmSize < prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size - u4MarlinOffset)
                {
                    if(u4FrmSize >= prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size - u4MarlinOffset -4)
                    {
                        LOG(3, "Skip abnormal nalu size!\n");
                        break;
                    }
                    u4NaluSize = 0;
                    //insert start code
                    for(i = 0; i < prSwdmxMp4Info->u1StartCodeLenMinus1; i++)
                    {
                        u4NaluSize += (UINT32)*pucFdrBuf;
                        u4NaluSize <<= 8;
                        *pucFdrBuf++ = 0;
                        //check ring buffer
                        if((UINT32)pucFdrBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                        {
                            pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        }
                    }
                    u4NaluSize += (UINT32)*pucFdrBuf;
                    if (u4NaluSize == 0)
                    {
                        prSwdmxMp4Info->u8VidLatestOff = prIdxTbl->ui8_base_offset +
                                                         prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                                                         prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                        prSwdmxMp4Info->u4NumSentVid++;
                        prSwdmxMp4Info->u4CurVidPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                        prSwdmxMp4Info->u4LeftBytesInFeeder -=
                            (u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);
                        prSwdmxMp4Info->u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                        if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                        {
                            prSwdmxMp4Info->u4CurDmxFeederRPtr =
                                prSwdmxMp4Info->u4CurDmxFeederRPtr -
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        }

                        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc,
                                                VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));

                        x_memcpy(&prSwdmxMp4Info->rPrevSample,
                                 prTmpSample, sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));
                        prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

                        (*pu4CurIdx)++;
                        return eSWDMX_HDR_PRS_SUCCEED;
                    }

                    *pucFdrBuf++ = 1;

                    if((prSwdmxMp4Info->u1StartCodeLenMinus1 < H264_STARTCODE_LEN) &&
                            (prSwdmxMp4Info->u1StartCodeLenMinus1 > 0))
                    {
                        UINT32   u4Loop = 0;
                        UINT32   u4DataSize = 0;

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                        HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                             (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
                        HalFlushInvalidateDCache();
#endif
                        for(u4Loop=0; u4Loop<(H264_STARTCODE_LEN - prSwdmxMp4Info->u1StartCodeLenMinus1); u4Loop++)
                        {
                            if((UINT32)pucBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr))
                            {
                                pucBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr - 1);
                            }
                            else
                            {
                                pucBuf--;
                            }

                            *pucBuf = 0;
                        }

                        u4DataSize = u4NaluSize + (H264_STARTCODE_LEN-
                                                   prSwdmxMp4Info->u1StartCodeLenMinus1)+
                                     (prSwdmxMp4Info->u1StartCodeLenMinus1 + 1);

                        rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                        rDmxMMData.u4StartAddr = (UINT32)pucBuf;
                        rDmxMMData.u4FrameSize = u4DataSize;
                        rDmxMMData.u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                        rDmxMMData.u1Idx = prSwdmxMp4Info->u1VidPid;

                        u4DmxVdecAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                             DMX_PID_TYPE_ES_VIDEO,
                                             prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
                        if(u4DataSize >= u4DmxVdecAvailSize)
                        {
                            _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
                            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
                        }

                        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                        {
                            LOG(1, "Demuxer fails to move data.\n");
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                        }
                    }

                    //check ring buffer
                    if((UINT32)pucFdrBuf == VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                    }
                    // nalu size
                    u4FrmSize += ((prSwdmxMp4Info->u1StartCodeLenMinus1 + 1) + u4NaluSize);
                    pucFdrBuf += u4NaluSize;
                    //check ring buffer
                    if((UINT32)pucFdrBuf >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        pucFdrBuf =
                            (UCHAR*)((UINT32)pucFdrBuf -
                                     VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                     VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                    }
                    pucBuf = pucFdrBuf;
                }
            }
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
            HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr,
                                                 (UINT32)(prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr));
#else
            HalFlushInvalidateDCache();
#endif
        }

        if(((UINT32)(prSwdmxMp4Info->u8SeekPts>>32) != 0xFFFFFFFF)
                || ((UINT32)(prSwdmxMp4Info->u8SeekPts) != 0xFFFFFFFF))
        {
            UINT32 u4Pts, u4CurSTC;

            u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
            if(u4Pts > (UINT32)prSwdmxMp4Info->u8SeekPts)
            {
                STC_SetStcValue(prSwdmxInst->u1StcId, u4Pts - 18000);
                u4CurSTC = STC_GetStcValue(prSwdmxInst->u1StcId);
                LOG(1, "Got Vid T PTS 0x%x, STC 0x%x, Seek 0x%x\n",
                    u4Pts, u4CurSTC, (UINT32)prSwdmxMp4Info->u8SeekPts);
                prSwdmxMp4Info->u8SeekPts = 0xFFFFFFFF;
                prSwdmxMp4Info->u8SeekPts <<= 32;
                prSwdmxMp4Info->u8SeekPts += 0xFFFFFFFF;
            }
        }
    }

    if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
#if 1
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
        {
            AES_PARAM_T rAesParam;
            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;

            if(_fgTestMarlin)
            {
                x_memcpy(rAesParam.au1Key, &(prSwdmxMp4Info->u1AudioDrmKey[0]), 16);
            }
            else
            {
                if (prSwdmxMp4Info->u1CurAKeyIndex < MP4_MAX_AUDIO_TRACK)
                {
                    x_memcpy(rAesParam.au1Key, &(prSwdmxMp4Info->u1AudioDrmKey[prSwdmxMp4Info->u1CurAKeyIndex]), 16);
                }
            }

            if(u4Tmp+16 >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                UINT32 u4FirstPart = prSwdmxInst->rFeederInfo.u4EndAddr - u4Tmp;
                LOG(2, " Handle feeder ring buffer for video IV!!!\n");

                if(u4FirstPart > 0)
                {
                    x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, u4FirstPart);
                }

                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);

                x_memcpy(rAesParam.au1Iv + u4FirstPart, (void*)u4Tmp, 16-u4FirstPart);
            }
            else
            {
                x_memcpy(rAesParam.au1Iv, (void*)prSwdmxMp4Info->u4CurDmxFeederRPtr, 16);
            }

            u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 16;
            if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
            }

            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
            rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
            rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
            rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            rAesParam.u4DatLen  = (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size-16);;

            rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
            rAesParam.fgEncrypt = FALSE;

            GCPU_Cmd(0, GCPU_AES, &rAesParam);
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
        {
            AES_PARAM_T rAesParam;
            UCHAR* pucBuf = NULL;

            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;

            if (u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
            {
                u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
            }

            pucBuf = (UCHAR*)u4Tmp;

            if ((*pucBuf) >> 7)
            {

                if(_fgTestMarlin)
                {
                    x_memcpy(rAesParam.au1Key, &(prSwdmxMp4Info->u1AudioDrmKey[0]), 16);
                }
                else
                {
                    if (prSwdmxMp4Info->u1CurAKeyIndex < MP4_MAX_AUDIO_TRACK)
                    {
                        x_memcpy(rAesParam.au1Key, &(prSwdmxMp4Info->u1AudioDrmKey[prSwdmxMp4Info->u1CurAKeyIndex]), 16);
                    }
                }

                u4Tmp++;
                if (u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }

                if(u4Tmp+16 >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    UINT32 u4FirstPart = prSwdmxInst->rFeederInfo.u4EndAddr - u4Tmp;
                    LOG(2, " Handle feeder ring buffer for video IV!!!\n");

                    if(u4FirstPart > 0)
                    {
                        x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, u4FirstPart);
                    }

                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);

                    x_memcpy(rAesParam.au1Iv + u4FirstPart, (void*)u4Tmp, 16-u4FirstPart);
                }
                else
                {
                    x_memcpy(rAesParam.au1Iv, (void*)u4Tmp, 16);
                }

                u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 17;
                if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                {
                    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                }

                rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DatLen  = (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size-17);;

                rAesParam.eMode = (prSwdmxMp4Info->eCryptMode == eSWDMX_CRYPT_MODE_AES_CBC)? BLOCK_CIPHER_MODE_CBC : BLOCK_CIPHER_MODE_CTR;
                rAesParam.fgEncrypt = FALSE;

                GCPU_Cmd(0, GCPU_AES, &rAesParam);
            }
        }
        else if ((prSwdmxMp4Info->u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
                 (prSwdmxMp4Info->prAudEncTbl[prSwdmxMp4Info->u1CurATrack]))
        {
            AES_PARAM_T rAesParam;
            UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;
            UINT8  u1CurAtrack = prSwdmxMp4Info->u1CurATrack;
            UINT32 u4SubCount = 0;
            UINT32 u4ClearLen = 0;
            UINT32 u4EncryptLen = 0;
            UINT32 u4ClearLen2 = 0;
            UINT32 u4EncryptLen2 = 0;
            UINT32 u4AllLen = 0;
            UINT32 u4Align = 16;
            UINT32 u4TempPtr = 0;

            MM_RANGE_ELMT_ENCTBL_T *prAudEncTbl = prSwdmxMp4Info->prAudEncTbl[u1CurAtrack];

            x_memcpy(rAesParam.au1Key, prAudEncTbl->aui1_key, 16);

            x_memcpy(rAesParam.au1Iv, prAudEncTbl->pt_sample_entry[*pu4CurIdx].ui1_iv, 16);

            if (prAudEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt == 0)
            {
                u4ClearLen = 0;
                u4EncryptLen = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;

                rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                rAesParam.u4DatLen  = u4EncryptLen;;

                rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                rAesParam.fgEncrypt = FALSE;

                LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                GCPU_Cmd(0, GCPU_AES, &rAesParam);
            }
            else
            {
                while (u4SubCount < prAudEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt && u4SubCount < 32)
                {
                    u4ClearLen = prAudEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_clear_data;
                    u4EncryptLen = prAudEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_encrypted_data;

                    if (u4EncryptLen == 0)
                    {
                        u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                        u4SubCount++;
                        continue;
                    }
                    else if ( u4EncryptLen % 16 != 0 )
                    {
                        LOG(0, "encrypt length % 16 is not 0  !!!!\n");

                        if ( u4SubCount < prAudEncTbl->pt_sample_entry[*pu4CurIdx].ui4_subsample_cnt - 1 )
                        {
                            u4ClearLen2 = prAudEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_clear_data;
                            u4EncryptLen2 = prAudEncTbl->pt_sample_entry[*pu4CurIdx].pt_subsample_info[u4SubCount].ui4_bytes_encrypted_data;
                        }
                        else
                        {
                            LOG(0, "the last block is error!!!!!!\n");
                            break;
                        }

                        _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId, eSWDMX_GET_INTERNAL_BUF, &u4TempPtr, &u4EncryptLen, &u4Align);

                        x_memcpy((UINT8 *)u4TempPtr, (VOID *)(u4Tmp + u4ClearLen), u4EncryptLen);
                        x_memcpy((UINT8 *)(u4TempPtr + u4EncryptLen), (VOID *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), u4EncryptLen2);

                        rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                        rAesParam.u4SrcStartAddr = VIRTUAL(u4TempPtr);
                        rAesParam.u4SrcBufStart = VIRTUAL(u4TempPtr);
                        rAesParam.u4SrcBufEnd = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstStartAddr = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstBufStart = VIRTUAL(u4TempPtr);
                        rAesParam.u4DstBufEnd = VIRTUAL(u4TempPtr);
                        rAesParam.u4DatLen  = u4EncryptLen + u4EncryptLen2;

                        rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                        rAesParam.fgEncrypt = FALSE;

                        LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                        GCPU_Cmd(0, GCPU_AES, &rAesParam);

                        x_memcpy((UINT8 *)(u4Tmp + u4ClearLen), (VOID *)u4TempPtr, u4EncryptLen);
                        x_memcpy((UINT8 *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), (VOID *)(u4TempPtr + u4EncryptLen), u4EncryptLen2);

                        u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                        u4SubCount += 2;
                    }
                    else
                    {
                        rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                        rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                        rAesParam.u4SrcBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        rAesParam.u4SrcBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                        rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                        rAesParam.u4DstBufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                        rAesParam.u4DstBufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
                        rAesParam.u4DatLen  = u4EncryptLen;

                        rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                        rAesParam.fgEncrypt = FALSE;

                        LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                        GCPU_Cmd(0, GCPU_AES, &rAesParam);

                        u4AllLen += (u4ClearLen + u4EncryptLen);
                        u4SubCount++;

                    }
                }
            }
        }
#endif

        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prDmxRangeInfo->e_mm_src_type ==
                MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {
            if (prSwdmxMp4Info->fgEnVideo &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurAudPts > (prSwdmxMp4Info->u4CurVidPts + MP4_ONE_SEC_DURATION)))
            {
                LOG(5, "Audio is enough, send video data in the same timeline\n");
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = TRUE;
                prSwdmxMp4Info->fgBlockSub = FALSE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            }
            else if (prSwdmxMp4Info->fgEnVideo &&
                     (!prSwdmxMp4Info->fgPartialRangeDone) &&
                     (prSwdmxMp4Info->u4CurAudPts > prSwdmxMp4Info->u4CurVidPts))
            {
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
                prSwdmxMp4Info->fgBlockSub = FALSE;
            }
        }
        else if(prSwdmxInst->eSeekType != eSWDMX_TYPE_TIME_SEEK)
        {
            if (prSwdmxMp4Info->fgEnVideo &&
                    (!prSwdmxMp4Info->fgPartialRangeDone) &&
                    (prSwdmxMp4Info->u4CurAudPts > (prSwdmxMp4Info->u4CurVidPts + 4 * MP4_ONE_SEC_DURATION)))
            {
                LOG(5, "NETWORK Audio is enough, send video data in the same timeline\n");
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = TRUE;
                _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
            }
            else if (prSwdmxMp4Info->fgEnVideo &&
                     (!prSwdmxMp4Info->fgPartialRangeDone) &&
                     (prSwdmxMp4Info->u4CurAudPts > prSwdmxMp4Info->u4CurVidPts))
            {
                prSwdmxMp4Info->fgBlockVid = FALSE;
                prSwdmxMp4Info->fgBlockAud = FALSE;
            }
        }

        if(((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts>>32) != 0xFFFFFFFF)
                || ((UINT32)(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts) != 0xFFFFFFFF))
        {
            UINT32 u4Pts;

            u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
            if(u4Pts > (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts)
            {
                AUD_SetStartPts(0, u4Pts);

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts = (UINT32)u4Pts;
                }

                LOG(7, "Got Aud T PTS 0x%x, Seek 0x%x\n",
                    u4Pts, (UINT32)prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts = 0xFFFFFFFF;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts <<= 32;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8ASeekPts += 0xFFFFFFFF;
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prIdxTbl->ui8_base_offset +
                        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder -=
                    (u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr =
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr -
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                }
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr));
                //record the previous sample data
                x_memcpy(&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample, prTmpSample, sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));
                prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

                (*pu4CurIdx)++;

                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSeekAfterPause)
        {
            UINT32 u4Pts = 0;
            UINT32 u4PlayPts = VDP_GetPts(prSwdmxInst->u1B2rId);

            u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
            if(u4Pts > u4PlayPts)
            {
                AUD_SetStartPts(0, u4Pts);

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts = (UINT32)u4Pts;
                }
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgSeekAfterPause = FALSE;
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8AudLatestOff = prIdxTbl->ui8_base_offset +
                        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                        prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LeftBytesInFeeder -=
                    (u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
                if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr =
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr -
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                }
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurDmxFeederRPtr));
                //record the previous sample data
                x_memcpy(&prSwdmxInst->rFmtInfo.rSwdmxMp4Info.rPrevSample, prTmpSample, sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));
                prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

                (*pu4CurIdx)++;

                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        if (prSwdmxMp4Info->u4ADecType == ENUM_SWDMX_ADEC_AAC)
        {
            UINT8 u1SampleRateIdx = 3;
            UINT32 u4IVOffset = 0;
            UINT32 u4RequestSize = AAC_ALLOC_SIZE;
            UINT32 u4Align = 16;
            if(prSwdmxInst->pucAudBuf == 0)
            {
                LOG(1, "pucAudBuf should be allocated in init function");
#if 0
                prSwdmxInst->pucAudBuf =
                    (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(AAC_ALLOC_SIZE,16));
#else
                _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId,eSWDMX_GET_INTERNAL_BUF, (UINT32*)prSwdmxInst->pucAudBuf, &u4RequestSize, &u4Align);
#endif
            }
            if (prSwdmxInst->pucAudBuf)
            {
                /* 8b: syncword */
                prSwdmxInst->pucAudBuf[0] = 0xFF;
                /* 4b: syncword */
                /* 1b: mpeg id = 1 */
                /* 2b: layer = 0 */
                /* 1b: protection absent */
                prSwdmxInst->pucAudBuf[1] = 0xF9;
                /* 2b: profile */
                /* 4b: sampling_frequency_index */
                /* 1b: private = 0 */
                /* 1b: channel_configuration */
                switch (prSwdmxMp4Info->u4AudSampleRate)
                {
                    case 96000:
                        u1SampleRateIdx = 0;
                        break;
                    case 88200:
                        u1SampleRateIdx = 1;
                        break;
                    case 64000:
                        u1SampleRateIdx = 2;
                        break;
                    case 48000:
                        u1SampleRateIdx = 3;
                        break;
                    case 44100:
                        u1SampleRateIdx = 4;
                        break;
                    case 32000:
                        u1SampleRateIdx = 5;
                        break;
                    case 24000:
                        u1SampleRateIdx = 6;
                        break;
                    case 22050:
                        u1SampleRateIdx = 7;
                        break;
                    case 16000:
                        u1SampleRateIdx = 8;
                        break;
                    case 12000:
                        u1SampleRateIdx = 9;
                        break;
                    case 11025:
                        u1SampleRateIdx = 10;
                        break;
                    case 8000:
                        u1SampleRateIdx = 11;
                        break;
                    default:
                        u1SampleRateIdx = 4;
                        break;
                }

                prSwdmxInst->pucAudBuf[2] = (1 << 6) |
                                            ((u1SampleRateIdx<< 2) & 0x3C) |
                                            ((prSwdmxMp4Info->u4AudChannelNs >> 2) & 0x1);
                /* 2b: channel_configuration */
                /* 1b: original */
                /* 1b: home */
                /* 1b: copyright_id */
                /* 1b: copyright_id_start */
                /* 2b: aac_frame_length */

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
                {
                    u4IVOffset = 16;
                }
                else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
                {
                    if((*((UCHAR*)prSwdmxMp4Info->u4CurDmxFeederRPtr))>>7)
                    {
                        u4IVOffset = 17;
                    }
                    else
                    {
                        u4IVOffset = 0;
                    }
                }
                prSwdmxInst->pucAudBuf[3] = ((prSwdmxMp4Info->u4AudChannelNs & 0x3) << 6) |
                                            (((prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size - u4IVOffset + AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
                /* 8b: aac_frame_length */
                prSwdmxInst->pucAudBuf[4] = (((prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size - u4IVOffset + AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
                /* 3b: aac_frame_length */
                /* 5b: adts_buffer_fullness */
                prSwdmxInst->pucAudBuf[5] = (((prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size - u4IVOffset + AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
                                            ((0x7FF >> 6) & 0x1F);
                /* 6b: adts_buffer_fullness */
                /* 2b: number_of_raw_data_blocks_in_frame */
                prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);
                rDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucAudBuf;
                rDmxMMData.u4BufEnd = rDmxMMData.u4BufStart + AAC_ALLOC_SIZE;
                rDmxMMData.u4StartAddr = rDmxMMData.u4BufStart;
                rDmxMMData.u4FrameSize = AAC_FAKE_HDR_SIZE;
                rDmxMMData.u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
                rDmxMMData.u1Idx =prSwdmxMp4Info->u1AudPid;
                //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                {
                    LOG(1, "Demuxer fails to move data. SrcID = %d, Header Type = %d\n",
                        prSwdmxInst->u1SwdmxId,
                        u1HdrType);
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
        }
    }
    /*Move data for subtitle.*/
    if(u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        UINT32 u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

        if(prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts < u4CurPts)
        {
            prSwdmxMp4Info->u4LeftBytesInFeeder -=
                (u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

            prSwdmxMp4Info->u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
            if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxMp4Info->u4CurDmxFeederRPtr =
                    prSwdmxMp4Info->u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }

            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));

            //record the previous sample data
            x_memcpy(&prSwdmxMp4Info->rPrevSample,
                     prTmpSample,
                     sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));

            prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

            (*pu4CurIdx)++;

            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

    if(u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        prSwdmxMp4Info->u4LastVidPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
    }
    
    /*Move data for audio/video.*/
    if(!((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
            (prSwdmxMp4Info->u1StartCodeLenMinus1 < H264_STARTCODE_LEN) &&
            (prSwdmxMp4Info->u1StartCodeLenMinus1 > 0)))
    {
        UINT32 u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr;
        UINT32 u4IVOffset = 0;

        switch (u1HdrType)
        {
            case eSWDMX_MEDIA_VIDEO:

				rDmxMMData.u1Idx = prSwdmxMp4Info->u1VidPid;

				//check seq header changing
				if (*pu4CurIdx == prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NextSeqChgVidIdx)
				{		
					UINT8 u1CurSampleDescID;

					u1CurSampleDescID =  prIdxTbl->at_range_desc_entry_chg[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurMutliDescInfoIdx].ui1_sample_desc_id;
						LOG(0, "seq header change, video index %d seq header index %d\n",  *pu4CurIdx, u1CurSampleDescID);

					if (u1CurSampleDescID<MP4_MAX_SEQ_HEADER && 
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVidSeqHeaderIsUsed[u1CurSampleDescID] &&
						prSwdmxMp4Info->u1PreVidHeaderIndex != u1CurSampleDescID)
					{
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHeaderPtr = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderPtr[u1CurSampleDescID];
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidHdrLength = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidSeqHeaderLength[u1CurSampleDescID];	
						_SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurMutliDescInfoIdx++;
						prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4NextSeqChgVidIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->at_range_desc_entry_chg[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurMutliDescInfoIdx].ui4_sample_idx;
						prSwdmxMp4Info->u1PreVidHeaderIndex = u1CurSampleDescID;
					}
					else
					{
						LOG(0, "seq header index error.\n");
					}

				}

				if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
				{
					u4IVOffset = 16;
					u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 16;
					if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
					{
					    u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
					}
				}
				else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
				{
					if((*((UCHAR*)prSwdmxMp4Info->u4CurDmxFeederRPtr))>>7)
					{
					    u4IVOffset = 17;
					    u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 17;
					    if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
					    {
					        u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
					    }
					}
				}
                break;
            case eSWDMX_MEDIA_AUDIO:
                rDmxMMData.u1Idx = prSwdmxMp4Info->u1AudPid;
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_MARLIN)
                {
                    u4IVOffset = 16;
                    u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 16;
                    if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                    {
                        u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                    }
                }
                else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.eDRMType == eSWDMX_DRM_TYPE_PDCF)
                {
                    if((*((UCHAR*)prSwdmxMp4Info->u4CurDmxFeederRPtr))>>7)
                    {
                        u4IVOffset = 17;
                        u4Tmp = prSwdmxMp4Info->u4CurDmxFeederRPtr + 17;
                        if(u4Tmp >= prSwdmxInst->rFeederInfo.u4EndAddr)
                        {
                            u4Tmp = prSwdmxInst->rFeederInfo.u4StartAddr + (u4Tmp-prSwdmxInst->rFeederInfo.u4EndAddr);
                        }
                    }
                }
                break;
            // To Do
            case eSWDMX_MEDIA_SUBTITLE:
                rDmxMMData.u1Idx = prSwdmxMp4Info->u1SubPid;
                break;
            default:
                LOG(3, "Wrong Pid\n");
                return eSWDMX_HDR_PRS_FAIL;
        }
        rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
        rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
        rDmxMMData.u4StartAddr = (u4Tmp);
        rDmxMMData.u4FrameSize = (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size-u4IVOffset);
        rDmxMMData.u4Pts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;

        //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
        {
            LOG(1, "Demuxer fails to move data. SrcID = %d, Header Type = %d\n",
                prSwdmxInst->u1SwdmxId,
                u1HdrType);
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
    }

    if((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
            (prSwdmxMp4Info->u8VidLatestOff <= prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset))
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u8VidLatestOff = prIdxTbl->ui8_base_offset +
                prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
        prSwdmxMp4Info->u4NumSentVid++;

        if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts >
                prSwdmxMp4Info->u4CurVidPts)
        {
            prSwdmxMp4Info->u4CurVidPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
        }
        LOG(6, "prSwdmxMp4Info->u4CurVidPts = 0x%08x\n", prSwdmxMp4Info->u4CurVidPts);
    }
    else if(u1HdrType == eSWDMX_MEDIA_AUDIO &&
            (prSwdmxMp4Info->u8AudLatestOff  <= prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset) )
    {
        prSwdmxMp4Info->u8AudLatestOff = prIdxTbl->ui8_base_offset +
                                         prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset +
                                         prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
        prSwdmxMp4Info->u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
        //prSwdmxMp4Info->u4NumSentAud++;
        LOG(7, "prSwdmxMp4Info->u4CurAudPts = 0x%08x\n", prSwdmxMp4Info->u4CurAudPts);
    }

    prSwdmxMp4Info->u4LeftBytesInFeeder -=
        (u8WriteOffset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size);

    prSwdmxMp4Info->u4CurDmxFeederRPtr += prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size;
    if(prSwdmxMp4Info->u4CurDmxFeederRPtr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxMp4Info->u4CurDmxFeederRPtr =
            prSwdmxMp4Info->u4CurDmxFeederRPtr -
            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
    }

    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxMp4Info->u4CurDmxFeederRPtr));

    //record the previous sample data
    x_memcpy(&prSwdmxMp4Info->rPrevSample,
             prTmpSample,
             sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));

    prSwdmxMp4Info->u8PrevSampleOffset = u8TmpSampleOffset;

    (*pu4CurIdx)++;

    return eSWDMX_HDR_PRS_SUCCEED;
}

BOOL _SWDMX_Mp4SetStrmID(
    UINT8 u1SrcId,
    UINT8 u1HdrType,
    UINT32 u4StrmID,
    UINT32 u4StrmSubID)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    UNUSED(u4StrmSubID);
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    switch(u1HdrType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxMp4Info->u4VStrmID = u4StrmID;
            break;
        case eSWDMX_STRM_TYPE_AUD:

            if (prSwdmxMp4Info->u4AStrmID != u4StrmID)
            {
                prSwdmxMp4Info->u4PrevAStrmID = prSwdmxMp4Info->u4AStrmID;
                prSwdmxMp4Info->u4AStrmID = u4StrmID;
            }

            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxMp4Info->u4SubTitleID = u4StrmID;
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_Mp4EnableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType,
    VOID* pfnCbFunc)
{
    //BOOL fgFindATrack;
    //INT32 i;

    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxMp4Info->fgEnVideo = TRUE;
            if (prSwdmxMp4Info->fgEnVDmx == FALSE)
            {
                _SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,NULL, FALSE);
                prSwdmxMp4Info->fgEnVDmx = TRUE;
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            if (prSwdmxMp4Info->fgEnADmx == FALSE)
            {
                _SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_AUDIO, NULL, FALSE);
                prSwdmxMp4Info->fgEnADmx = TRUE;
            }
            /*fgFindATrack = FALSE;
            for (i = 0; i < prSwdmxMp4Info->u1TotalATrack; i++)
            {
                if (prSwdmxMp4Info->prAudIdxTbl[i] &&
                    prSwdmxMp4Info->prAudIdxTbl[i]->t_strm_id.u.t_stm_mp4_id == prSwdmxMp4Info->u4AStrmID)
                {
                    prSwdmxMp4Info->u1CurATrack = i;
                    fgFindATrack = TRUE;
                }
            }
            if (!fgFindATrack)
            {
                LOG(3, "_SWDMX_Mp4EnableStrm didn't find audio stream\n");
                return FALSE;
            }
            if (prSwdmxMp4Info->u4NumSentAud != 0)
            {
                prSwdmxMp4Info->fgChgATrack = TRUE;
            }*/
            if (/*(prSwdmxMp4Info->u4PrevAStrmID != 0xFFFFFFFF)&&*/
                (prSwdmxMp4Info->u4PrevAStrmID != prSwdmxMp4Info->u4AStrmID)&&
                prSwdmxMp4Info->fgBeginToPlay)
            {
                prSwdmxMp4Info->fgChgATrack = TRUE;
            }
            else if (prSwdmxInst->eCurFMT != SWDMX_FMT_M4A && prSwdmxMp4Info->fgAudStrmDisable && !prSwdmxMp4Info->fgRecSeekFlag && !prSwdmxMp4Info->fgRecSpeedChangeFlag &&
                     prSwdmxMp4Info->fgBeginToPlay)
            {
                LOG(5, "Audio stream is disabled and seek/trick is not performed\n");
                prSwdmxMp4Info->fgChgATrack = TRUE;
                prSwdmxMp4Info->fgAudStrmDisable = FALSE;
            }

            if(!prSwdmxMp4Info->fgRecSeekFlag)
            {
                prSwdmxMp4Info->fgWaitNewRange =  FALSE;
            }

            prSwdmxMp4Info->fgRecSpeedChangeFlag = FALSE;
            prSwdmxMp4Info->fgRecSeekFlag = FALSE;
            prSwdmxMp4Info->u4PrevAStrmID = prSwdmxMp4Info->u4AStrmID;
            prSwdmxMp4Info->fgEnAudio = TRUE;

            if (prSwdmxMp4Info->eDRMType != eSWDMX_DRM_TYPE_NONE)
            {
                UCHAR ucAKeyId;
                for (ucAKeyId=0; ucAKeyId < prSwdmxMp4Info->u1AKeyCount; ucAKeyId++)
                {
                    if (prSwdmxMp4Info->u1AudioKeyTrackId[ucAKeyId] == prSwdmxMp4Info->u4AStrmID)
                    {
                        prSwdmxMp4Info->u1CurAKeyIndex = ucAKeyId;
                    }
                }
            }

            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:

            if (prSwdmxMp4Info->fgEnSubDmx == FALSE)
            {
                UNUSED(_SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_SUBTITLE, pfnCbFunc, FALSE));
                prSwdmxMp4Info->fgEnSubDmx = TRUE;
            }
            prSwdmxMp4Info->fgChgSubTrack = TRUE;
            prSwdmxMp4Info->fgEnSubTitle = TRUE;
            break;

        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_Mp4DisableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo = FALSE;
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
            {
                _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnAudio = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudStrmDisable = TRUE;
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubTitle = FALSE;
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

static BOOL _SWDMX_InitDmx(
    SWDMX_INFO_T* prSwdmxInst,
    UINT8 u1HdrType,
    VOID* pfnCbFunc,
    BOOL fgEos)
{
    BOOL fgRet = FALSE;
    UINT32 u4Flag = 0;
    DMX_MM_T rDmxMMInfo;

    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, prSwdmxInst->u1SwdmxId);

    x_memset((void*)&rDmxMMInfo, 0, sizeof(DMX_MM_T));

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVDmx)
        {
            UINT32 u4VidFifoAddr, u4VidFifoSize;

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

#if 0
            // Allocate video buffer from FBM
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
            if((prFbmPool == NULL) || (prFbmPool->u4Addr == 0))
            {
                LOG(1, "_SWDMX_SetVideoStreamID (prFbmPool == NULL) || (prFbmPool->u4Addr == NULL)\n");
                return FALSE;
            }
#endif
            _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &u4VidFifoAddr, &u4VidFifoSize);

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID |
                      DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG |
                      DMX_MM_FLAG_DEVICE_ID);

            rDmxMMInfo.fgAllocBuf = FALSE;
            rDmxMMInfo.u4BufAddr = u4VidFifoAddr;
            rDmxMMInfo.u4BufSize = u4VidFifoSize;
            rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType ==
                    (UINT32)ENUM_SWDMX_VDEC_MJPEG)
            {
                rDmxMMInfo.u1DeviceId = VLD1; // video format all use VLD expect jpeg
            }
            else
            {
                rDmxMMInfo.u1DeviceId = VLD0; // video format all use VLD expect jpeg
            }

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT) && !defined(CC_MT5890)
#if defined(CC_VOMX_TV_COEXIST) || defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)
            {
                prSwdmxInst->pVFifo2Info = FBM_Alloc(FBM_TYPE_TZ_DMX);
                if (prSwdmxInst->pVFifo2Info)
                {
                    rDmxMMInfo.u4BufSize = u4VidFifoSize + ((FBM_POOL_T *)prSwdmxInst->pVFifo2Info)->u4Size;
                    rDmxMMInfo.u4BufAddr = u4VidFifoAddr;
                }
                LOG(0,"Mp4 don not use second vfifo, Now VFIFO Size is 0x%08x\n", rDmxMMInfo.u4BufSize);
            }
#endif
#endif
        }

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;

        u4Flag |= DMX_MM_FLAG_SEARCH_START_CODE;

        switch (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VDecType)
        {
            case ENUM_SWDMX_VDEC_DX311: case ENUM_SWDMX_VDEC_WMV7:
            case ENUM_SWDMX_VDEC_WMV8: case ENUM_SWDMX_VDEC_WMV9:
            case ENUM_SWDMX_VDEC_MJPEG: case ENUM_SWDMX_VDEC_H263:
                LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                rDmxMMInfo.fgSearchStartCode = FALSE;
                break;
            default:
                LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                rDmxMMInfo.fgSearchStartCode = TRUE;
                break;
        }

        if (fgEos)
        {
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
        }

        fgRet = DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid, u4Flag, &rDmxMMInfo);
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        //UINT32 au4BufStart[2], au4BufEnd[2];
        UINT32 u4AudFifoAddr, u4AudFifoSize;

        u4Flag = (DMX_MM_FLAG_TYPE |
                  DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF |
                  DMX_MM_FLAG_INSTANCE_TAG |
                  DMX_MM_FLAG_DEVICE_ID);

        if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnADmx)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
        }

        _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;
        rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
        rDmxMMInfo.u1ChannelId = prSwdmxInst->u1AdecId;
        rDmxMMInfo.u1DeviceId = prSwdmxInst->u1AdecId;

#if 0
#ifdef AUD_OLDFIFO_INTF
        if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                           &au4BufEnd[1]) != AUD_OK)
#else
        if (AUD_GetAudFifo(prSwdmxInst->u1AdecId, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK)
#endif
        {
            LOG(1, "_SWDMX_SetAudioStreamID Can't get audio buffers!\n");
            return FALSE;
        }
#endif

        rDmxMMInfo.fgAllocBuf = FALSE;

        rDmxMMInfo.u4BufAddr = u4AudFifoAddr;
        rDmxMMInfo.u4BufSize = u4AudFifoSize;

        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudBufSize = rDmxMMInfo.u4BufSize;

        if (fgEos)
        {
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
        }

        fgRet = DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid, u4Flag, &rDmxMMInfo);
    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_PES;

        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid =
            DMX_MUL_GetSubtitlePidx(prSwdmxInst->u1DmxId);
        if (pfnCbFunc)
        {
            u4Flag = (DMX_MM_FLAG_TYPE |
                      DMX_MM_FLAG_SEARCH_START_CODE |
                      DMX_MM_FLAG_BUF |
                      DMX_MM_FLAG_VALID);

            u4Flag |= DMX_MM_FLAG_NOTIFY;
            rDmxMMInfo.pfnCallback = (PFN_DMX_NOTIFY)pfnCbFunc;

            rDmxMMInfo.fgSearchStartCode = FALSE;
            rDmxMMInfo.fgAllocBuf = TRUE;
            rDmxMMInfo.u4BufSize = MP4_SUBTITLE_BUFSIZE;

            fgRet = DMX_MM_Set( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid , u4Flag, &rDmxMMInfo);
        }
    }

    return fgRet;
}


static BOOL _SWDMX_SendMp4Eos(
    SWDMX_INFO_T* prSwdmxInst,
    UINT8 u1HdrType)
{
    UINT8 u1PidIdx;
    UINT16 u2MaxQNum, u2CurQNum;
    //UINT32 u4DummyData[4];
    DMX_MM_DATA_T rDmxMMData;

    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, prSwdmxInst->u1SwdmxId);
    LOG(5, "SWDMX Send Mp4 Eos Hdr Type = %d\n", u1HdrType);

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        VDEC_SetTotalTime(prSwdmxInst->u1VdecId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TotalTime);
        LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);
        //if (u2CurQNum > (_rSwdmxMp4Info.u4VidFps/2))
        if (u2CurQNum > (u2MaxQNum - 10))
        {
            x_thread_delay(1);
            return FALSE;
        }
        u1PidIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid;
    }
    else
    {
        u1PidIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    if (_SWDMX_InitDmx(prSwdmxInst, u1HdrType, NULL, TRUE) == FALSE)
    {
        return FALSE;
    }

    rDmxMMData.u4BufStart = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4BufEnd = prSwdmxInst->rFeederInfo.u4EndAddr;
    rDmxMMData.u4StartAddr = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4FrameSize = 4;
    rDmxMMData.fgEOS = TRUE;
    rDmxMMData.u1Idx = u1PidIdx;
    rDmxMMData.fgIgnoreData = TRUE;

    //return DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500);
    return _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData);
}


/**
*/
BOOL _SWDMX_Mp4Stop(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    DMX_MM_T rPid;
    rPid.fgEnable = FALSE;
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVDmx)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVDmx = FALSE;

        VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
        DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1VidPid);
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnADmx)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnADmx = FALSE;
        VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid);
        DMX_MUL_FreePidx( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1AudPid );
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubDmx)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnSubDmx = FALSE;
        VERIFY(DMX_MM_Set( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid , DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid );
        DMX_MUL_FreePidx( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1SubPid );
    }
    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxMp4Info, 0, sizeof(SWDMX_MP4_INFO_T));

    return TRUE;
}

static UINT32 _SWDMX_Mp4SeekCurPos(SWDMX_INFO_T* prSwdmxInst)
{
    VDP_POS_INTO_T rVdpPosInfo;
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    MM_RANGE_ELMT_IDXTBL_T* prIdxTbl = NULL;   //current range
    UINT32 u4StartPTS=0xffffffff,u4EndPTS,u4TargetPTS=0xffffffff,i=0;

    prSwdmxInst->rCurRangeReq.fgIsSeekTarget = FALSE;
    if(!prSwdmxMp4Info->fgEnVDmx)
    {
        return FALSE;
    }

    if(VDP_GetPosInfo(prSwdmxInst->u1B2rId,&rVdpPosInfo))
    {
        //u4TargetPTS=rVdpPosInfo.u8PtsI;
        prIdxTbl = prSwdmxMp4Info->prVidIdxTbl;
        prSwdmxMp4Info->u4TriggerAudPts=rVdpPosInfo.u8AudPts;
        prSwdmxMp4Info->u4LastVidPts = rVdpPosInfo.u8Pts;
        if(prIdxTbl)
        {
            u4StartPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
            u4EndPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_pts;
            if(rVdpPosInfo.u8PtsI<u4StartPTS || rVdpPosInfo.u8PtsI>u4EndPTS)
            {
                return (UINT32)rVdpPosInfo.u8PtsI;
            }
            else
            {
                u4TargetPTS=(UINT32)rVdpPosInfo.u8PtsI;
            }

            prSwdmxMp4Info->u8CurFilepos=rVdpPosInfo.u8Offset;
            prSwdmxInst->rCurRangeReq.fgIsSeekTarget=TRUE;
            //set video range idx to the entry with target PTS
            for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
            {
                if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts == u4TargetPTS)
                {
                    prSwdmxInst->rCurRangeReq.u4CurRangeVidIdx = i;
                    prSwdmxMp4Info->u4CurVidPts = u4TargetPTS;
                    break;
                }
            }
        }
        else
        {
            prSwdmxMp4Info->u4CurVidPts=(UINT32)rVdpPosInfo.u8Pts;
        }

        if(prSwdmxMp4Info->u1CurATrack<MP4_MAX_AUDIO_TRACK)
        {
            prIdxTbl=prSwdmxMp4Info->prAudIdxTbl[prSwdmxMp4Info->u1CurATrack];
        }
        if(prIdxTbl&&(prIdxTbl->ui4_number_of_entry != 0))
        {
            for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
            {
                if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >=u4TargetPTS)
                {
                    prSwdmxInst->rCurRangeReq.u4CurRangeAudIdx = i;
                    prSwdmxMp4Info->fgHasAud = TRUE;
                    prSwdmxMp4Info->u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts;
                    LOG(5, "Audio PTS = 0x%08x indx = %x\n",prSwdmxMp4Info->u4CurAudPts,prSwdmxInst->rCurRangeReq.u4CurRangeAudIdx);
                    break;
                }
            }
            if (!prSwdmxMp4Info->fgHasAud)
            {
                prSwdmxInst->rCurRangeReq.u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
            }
        }
        else
        {
            prSwdmxMp4Info->fgHasAud=FALSE;
        }

        if(prSwdmxMp4Info->u1CurSubTrack < MP4_MAX_SUB_TRACK)
        {
            prIdxTbl = prSwdmxMp4Info->prSubIdxTbl[prSwdmxMp4Info->u1CurSubTrack];
        }

        if(prIdxTbl&&(prIdxTbl->ui4_number_of_entry != 0))
        {
            for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
            {
                if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= u4TargetPTS)
                {
                    prSwdmxInst->rCurRangeReq.u4CurRangeSubIdx = i;
                    LOG(5, "Subtitle PTS = 0x%08x\n", prSwdmxMp4Info->u4CurAudPts);
                    break;
                }
            }

            if (i >= prIdxTbl->ui4_number_of_entry)
            {
                prSwdmxInst->rCurRangeReq.u4CurRangeSubIdx = prIdxTbl->ui4_number_of_entry;
            }

            LOG(2, "_SWDMX_Mp4Seek Key = %ld, Key PTS = %ld\n", prSwdmxMp4Info->u4CurSentKey, u4TargetPTS);
        }
        return TRUE;
    }
    return FALSE;
}

static UINT32 _SWDMX_Mp4Seek(SWDMX_INFO_T* prSwdmxInst,
                             UINT64 u8SeekTime,
                             SWDMX_RANGE_LIST_T* prRangeList,
                             INT32 i4PlaySpeed)
{
    //find previous key's PTS
    //if within cur range,
    // 1. prRangeList->fgIsSeekTarget will be TRUE,
    // 2. range idx will set to the entry with target PTS
    //else, update new range according to target PTS
    INT32 i;
    UINT32 u4TargetPTS = 0xFFFFFFFF, u4StartPTS = 0xFFFFFFFF, u4EndPTS = 0xFFFFFFFF;
    UINT32 u4TargetOffset = 0xFFFFFFFF, u4StartOffset = 0xFFFFFFFF, u4EndOffset = 0xFFFFFFFF;

    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    MM_RANGE_ELMT_IDXTBL_T* prIdxTbl = NULL;   //current range
    MM_RANGE_ELMT_IDXTBL_T* prVidKeyIdxTbl = NULL;
    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    prVidKeyIdxTbl = prSwdmxMp4Info->prVidKeyIdxTbl;

    prRangeList->fgIsSeekTarget = FALSE;
    if (!prSwdmxInst)
    {
        return 0;
    }

    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
            (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]))
    {
        BOOL fgTarget = FALSE;
        BOOL fgNextKeyTable = FALSE;
        BOOL fgFind = FALSE;

        do
        {
            fgTarget = FALSE;
            fgNextKeyTable = FALSE;

            for (i = 0; i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry; i++)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts > u8SeekTime)
                {
                    fgTarget = TRUE;
                    fgFind = TRUE;
                    break;
                }
            }
			
            if(prVidKeyIdxTbl->pt_next_partial != NULL)
            {
                fgNextKeyTable = TRUE;
                if(!fgTarget)
                {
                    prVidKeyIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)(prVidKeyIdxTbl->pt_next_partial);
                    prSwdmxMp4Info->prVidKeyIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)(prSwdmxMp4Info->prVidKeyIdxTbl->pt_next_partial);
                }
            }
        }
        while(!fgTarget && fgNextKeyTable);

        //get current idx of key table array
        _SWDMX_Mp4SetKeyTableIdx(prSwdmxInst,prVidKeyIdxTbl);

        // Error Handling
        // i = 0 is impossible
        if (i > 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = i - 1;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey= 0;
            fgFind = FALSE;
        }

        if (i4PlaySpeed > 2000&&(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey+1<prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry))
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey + 1;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey++;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey;
        }
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSentKey=prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey+1;

        // check if target key frame PTS is in current range
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastVidPts = u8SeekTime;
        if ( fgFind )
        {
            u4TargetPTS = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts;
        }
        else
        {
            u4TargetPTS = u8SeekTime;
        }
        prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;

        u4TargetOffset =  prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui8_base_offset + prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_relative_offset;
        u4StartOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_relative_offset;
        u4StartPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
        u4EndOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_relative_offset;
        u4EndPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_pts;

        LOG(5, "u4StartOffset = 0x%08x u4EndOffset = 0x%08x u4TargetOffset = 0x%08x\n",
            u4StartOffset,
            u4EndOffset,
            u4TargetOffset);

        LOG(5, "u4StartPTS = 0x%08x u4EndPTS = 0x%08x u4TargetPTS = 0x%08x\n",
            u4StartPTS,
            u4EndPTS,
            u4TargetPTS);

        if((u4TargetOffset >= u4StartOffset) && (u4TargetOffset <= u4EndOffset))
        {
            prRangeList->fgIsSeekTarget = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0xFFFFFFFF;
            if(u4TargetOffset <= u4StartOffset)
            {
                prRangeList->u4CurRangeVidIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
            }
            else
            {
                //set video range idx to the entry with target PTS
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_relative_offset == u4TargetOffset)
                    {
                        prRangeList->u4CurRangeVidIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;

                        LOG(1, "Video PTS = 0x%08x indx = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts,  prRangeList->u4CurRangeVidIdx);
                        break;
                    }
                }
            }

        }
        else if((u4TargetPTS + MP4_MAX_CTTS_PTS >= u4StartPTS) && (u4TargetPTS <= u4EndPTS))
        {
            prRangeList->fgIsSeekTarget = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0xFFFFFFFF;
            if(u4TargetPTS <= u4StartPTS)
            {
                prRangeList->u4CurRangeVidIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
            }
            else
            {
                //set video range idx to the entry with target PTS
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts == u4TargetPTS)
                    {
                        prRangeList->u4CurRangeVidIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;

                        LOG(1, "Video PTS = 0x%08x indx = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts,  prRangeList->u4CurRangeVidIdx);
                        break;
                    }
                }
            }

        }
        else
        {
            // to handle piff or seamless playback.
            u4TargetPTS = u8SeekTime;
            prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;
            u4StartPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
            u4EndPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_pts;

            if ((u4TargetPTS + MP4_MAX_CTTS_PTS >= u4StartPTS) && (u4TargetPTS <= u4EndPTS))
            {
                LOG(0, "SEEK: Use first entry of range as key frame!!!\n");
                LOG(5, "u4StartPTS = 0x%08x u4EndPts = 0x%08x u4TargetPTS = 0x%08x\n",
                    u4StartPTS,
                    u4EndPTS,
                    u4TargetPTS);

                prRangeList->fgIsSeekTarget = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0xFFFFFFFF;
                //set video range idx to the first entry, which supposed to be key frame.
                prRangeList->u4CurRangeVidIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
            }
        }

        if( prRangeList->fgIsSeekTarget)
        {
            // set audio range idx
            prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];

            if(prIdxTbl&&
                    (prIdxTbl->ui4_number_of_entry != 0))
            {
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= u8SeekTime)
                    {
                        prRangeList->u4CurRangeAudIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts;
                        LOG(5, "Audio PTS = 0x%08x indx = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts,  prRangeList->u4CurRangeAudIdx);
                        break;
                    }
                }

                if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                {
                    prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                }

                LOG(2, "_SWDMX_Mp4Seek Key = %ld, Key PTS = %ld\n",
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey, u4TargetPTS);
            }

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack];
            }

            if(prIdxTbl&&
                    (prIdxTbl->ui4_number_of_entry != 0))
            {
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= u8SeekTime)
                    {
                        prRangeList->u4CurRangeSubIdx = i;
                        LOG(5, "Subtitle PTS = 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                        break;
                    }
                }

                if (i >= prIdxTbl->ui4_number_of_entry)
                {
                    prRangeList->u4CurRangeSubIdx = prIdxTbl->ui4_number_of_entry;
                }

                LOG(2, "_SWDMX_Mp4Seek Key = %ld, Key PTS = %ld\n",
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey, u4TargetPTS);
            }
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl &&
             prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl)
    {
        BOOL fgFind = FALSE;

        for (i = 0; i < prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui4_number_of_entry; i++)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts > u8SeekTime)
            {
                fgFind = TRUE;
                break;
            }
        }

        // Error Handling
        // i = 0 is impossible
        if (i > 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = i - 1;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey= 0;
            fgFind = FALSE;
        }

        if (i4PlaySpeed > 2000)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey + 1;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey++;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey;
        }

        // check if target key frame PTS is in current range
        if ( fgFind )
        {
            u4TargetPTS = prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts;
        }
        else
        {
            u4TargetPTS = u8SeekTime;
        }

        prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl;

        u4TargetOffset = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->ui8_base_offset + prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_relative_offset;
        u4StartOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_relative_offset;
        u4StartPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
        u4EndOffset = prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_relative_offset;
        u4EndPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_pts;

        if((u4TargetOffset >= u4StartOffset) && (u4TargetOffset <= u4EndOffset))
        {
            LOG(5, "u4StartOffset = 0x%08x u4EndOffset = 0x%08x u4TargetOffset = 0x%08x\n",
                u4StartOffset,
                u4EndOffset,
                u4TargetOffset);

            prRangeList->fgIsSeekTarget = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0xFFFFFFFF;
            if(u4TargetOffset <= u4StartOffset)
            {
                prRangeList->u4CurRangeVidIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
            }
            else
            {
                //set video range idx to the entry with target PTS
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_relative_offset == u4TargetOffset)
                    {
                        prRangeList->u4CurRangeVidIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
                        break;
                    }
                }
            }

            LOG(2, "_SWDMX_Mp4Seek Key = %ld, Key PTS = %ld\n",
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey, u4TargetPTS);
        }
        else if((u4TargetPTS + MP4_MAX_CTTS_PTS >= u4StartPTS) && (u4TargetPTS <= u4EndPTS))
        {
            LOG(5, "u4StartPTS = 0x%08x u4EndPTS = 0x%08x u4TargetPTS = 0x%08x\n",
                u4StartPTS,
                u4EndPTS,
                u4TargetPTS);

            prRangeList->fgIsSeekTarget = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastIPts = 0xFFFFFFFF;
            if(u4TargetPTS <= u4StartPTS)
            {
                prRangeList->u4CurRangeVidIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;
            }
            else
            {
                //set video range idx to the entry with target PTS
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts == u4TargetPTS)
                    {
                        prRangeList->u4CurRangeVidIdx = i;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts = u4TargetPTS;

                        LOG(1, "Video PTS = 0x%08x indx = %x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurVidPts,  prRangeList->u4CurRangeVidIdx);
                        break;
                    }
                }
            }

        }

        if (prRangeList->fgIsSeekTarget)
        {
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack < MP4_MAX_SUB_TRACK)
            {
                prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prSubIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurSubTrack];
            }
            if(prIdxTbl&&
                    (prIdxTbl->ui4_number_of_entry != 0))
            {
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= u8SeekTime)
                    {
                        prRangeList->u4CurRangeAudIdx = i;
                        LOG(5, "Subtitle PTS = 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts);
                        break;
                    }
                }

                if (i >= prIdxTbl->ui4_number_of_entry)
                {
                    prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                }

                LOG(2, "_SWDMX_Mp4Seek Key = %ld, Key PTS = %ld\n",
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey, u4TargetPTS);
            }
        }

    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack < MP4_MAX_AUDIO_TRACK) &&
             (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]) &&
             (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack]->u.pt_pb_idx_tbl_entry != NULL))
    {
        BOOL fgFind = FALSE;

        LOG(1, "_SWDMX_Mp4Seek  Audio Seek, u8SeekTime = 0x%x\n", u8SeekTime);
        // set audio range idx
        prIdxTbl = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];

        do
        {
            for ( i = 0; i < prIdxTbl->ui4_number_of_entry; i++ )
            {
                if ( prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts >= u8SeekTime )
                {
                    fgFind = TRUE;
                    break;
                }
            }
        }
        while(0);

        // Error Handling
        // i = 0 is impossible
        if (i > 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = i - 1;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = 0;
            fgFind = FALSE;
        }

        if ( i4PlaySpeed > 2000 && ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey+1 < prIdxTbl->ui4_number_of_entry ) )
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey + 1;
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey++;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey;
        }
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastSentKey=prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey + 1;

        // check if target key frame PTS is in current range
        //prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastAudPts = u8SeekTime;
        if ( fgFind )
        {
            u4TargetPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey].ui4_pts;
        }
        else
        {
            u4TargetPTS = u8SeekTime;
        }

        u4StartPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts;
        u4EndPTS = prIdxTbl->u.pt_pb_idx_tbl_entry[prIdxTbl->ui4_number_of_entry - 1].ui4_pts;

        LOG(5, "u4StartPTS = 0x%08x u4EndPts = 0x%08x u4TargetPTS = 0x%08x\n",
            u4StartPTS,
            u4EndPTS,
            u4TargetPTS);

        if((u4TargetPTS + MP4_MAX_CTTS_PTS >= u4StartPTS) && (u4TargetPTS <= u4EndPTS))
        {
            prRangeList->fgIsSeekTarget = TRUE;
            if(u4TargetPTS <= u4StartPTS)
            {
                prRangeList->u4CurRangeAudIdx = 0;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = u4TargetPTS;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = TRUE;
            }
            else
            {
                //set video range idx to the entry with target PTS
                for(i = 0; i < prIdxTbl->ui4_number_of_entry; i++)
                {
                    if(prIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts == u4TargetPTS)
                    {
                        prRangeList->u4CurRangeAudIdx = i;
                        LOG(1, "%s, %d, u4CurRangeAudIdx = %u\n", __FUNCTION__, __LINE__, prRangeList->u4CurRangeAudIdx );
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts = u4TargetPTS;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud = TRUE;
                        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = i;

                        LOG(1, "Audio PTS = 0x%08x indx = %u\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurAudPts,  prRangeList->u4CurRangeAudIdx);
                        break;
                    }
                }

                if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgHasAud)
                {
                    prRangeList->u4CurRangeAudIdx = prIdxTbl->ui4_number_of_entry;
                    LOG(1, "%s, %d, u4CurRangeAudIdx = %u\n", __FUNCTION__, __LINE__, prRangeList->u4CurRangeAudIdx );
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurSentKey = i;
                }
            }
        }
    }

    return u4TargetPTS;
}


static BOOL _SWDMX_Mp4GetDataFromFeeder(SWDMX_INFO_T* prSwdmxInst,
                                        FEEDER_REQ_DATA_T *prFeederData)
{
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    if (!_SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData))
    {
        LOG(3, "_SWDMX_Mp4GetDataFromFeeder FeederTimeOut!!!\n");
        return FALSE;
    }

    if (prFeederData->eDataType == FEEDER_SOURCE_INVALID)
    {
        LOG(3, "_SWDMX_Mp4GetDataFromFeeder invalid. Current req times: %d\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes);
        return FALSE;
    }

    //if ((prFeederData->u4Id + 1) != prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID)
    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes == 0) ||
            (prFeederData->u4Id != (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID - prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes)))
    {
        LOG(0, "Feeder ID mismatch!!! feeder id = %ld\n", prFeederData->u4Id);
        _SWDMX_Mp4SetFeederInvalid(prSwdmxInst, TRUE);
        return FALSE;
    }


    return TRUE;
}


static BOOL _SWDMX_Mp4GetDataFromFeederNoWait(SWDMX_INFO_T* prSwdmxInst,
        FEEDER_REQ_DATA_T *prFeederData)
{

    INT32 i4Ret = 0;
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);
    i4Ret = _SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData);

    if (i4Ret != OSR_OK)
    {
        //LOG(3, "_SWDMX_Mp4GetDataFromFeederNoWait No Msg!!!\n");
        //_rSwdmxMp4Info.fgFeederInvalid = TRUE;
        return FALSE;
    }

    if (prFeederData->eDataType == FEEDER_SOURCE_INVALID)
    {
        LOG(3, "_SWDMX_Mp4GetDataFromFeederNoWait invalid\n");
        //_rSwdmxMp4Info.fgFeederInvalid = TRUE;
        return FALSE;
    }

    //if ((prFeederData->u4Id + 1) != prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID)

    if ((prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes == 0) ||
            (prFeederData->u4Id != (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4FeederReqID - prSwdmxInst->rFmtInfo.rSwdmxMp4Info.i4ReqReadTimes)))
    {
        LOG(0, "_SWDMX_Mp4GetDataFromFeederNoWait Feeder ID mismatch!!! feeder id = %ld\n", prFeederData->u4Id);
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid = TRUE;
        return FALSE;
    }


    return TRUE;
}

static BOOL _SWDMX_Mp4CalculateAudioFrame(SWDMX_INFO_T* prSwdmxInst,
        INT32 i4PlaySpeed)
{
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl = NULL;
    UINT32 u4CurKey = 0;

    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    u4CurKey = prSwdmxMp4Info->u4CurSentKey;
    prIdxTbl = prSwdmxMp4Info->prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u1CurATrack];

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame > 0)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame--;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame == 0)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx < prIdxTbl->ui4_number_of_entry)
        {
            if (i4PlaySpeed < 0)
            {
                if (u4CurKey > ((abs(i4PlaySpeed / 1000) * 2 ) * (prIdxTbl->ui4_number_of_entry / M4A_RANGE_DIV_COUNT + 1)))
                {
                    u4CurKey = u4CurKey - ((abs(i4PlaySpeed / 1000) * 2) * (prIdxTbl->ui4_number_of_entry / M4A_RANGE_DIV_COUNT + 1));
                }
                else
                {
                    u4CurKey = 0;
                }
            }

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame = 1;

            if ((u4CurKey == prIdxTbl->ui4_number_of_entry - 1) || (prSwdmxMp4Info->u4CurSentKey/*u4CurKey*/ == 0))
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame = 0;
            }

            LOG(5,"Current Idx = %ld Repeat frame = %ld u4LastAudPts = 0x%x, ui4_pts = 0x%x\n", u4CurKey, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastAudPts, prIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts);
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = u4CurKey;

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame == 0)
            {
                return FALSE;
            }

            if( i4PlaySpeed < 0 && prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastAudPts < prIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts )
            {
                LOG(1,"u4LastAudPts = 0x%x, ui4_pts = 0x%x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4LastAudPts, prIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame = 0;

                if ( prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx > 0 )
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx--;
                }

                if ( prSwdmxMp4Info->u4CurSentKey > 0 )
                {
                    prSwdmxMp4Info->u4CurSentKey--;
                }

                return FALSE;
            }

            return TRUE;
        }
        else
        {
            // Force to play the first key frame
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4AudRepeatFrame = 1;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = u4CurKey;
                return TRUE;
            }

            LOG(1, "u4CurRepeatFrameIdx: 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx);

            return FALSE;
        }
    }

    return TRUE;
}

static BOOL _SWDMX_Mp4CalculateRepeatFrame(SWDMX_INFO_T* prSwdmxInst,
        INT32 i4PlaySpeed)
{
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    MM_RANGE_ELMT_IDXTBL_T *prKeyIdxTbl = NULL;
    UINT32 u4CurKey = 0;
    UINT32 u4FpsMuiltTen = 0;
    UINT32 u4PtsDif = 0;

    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;
    u4CurKey = prSwdmxMp4Info->u4CurSentKey;
    prKeyIdxTbl = prSwdmxMp4Info->prVidKeyIdxTbl;
    u4FpsMuiltTen = (UINT32)(10 * prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_rate/prSwdmxInst->rFmtInfo.rSwdmxMp4Info.prVidIdxTbl->ui4_scale);

    if (!prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgProgressiveSeq)
    {
        u4FpsMuiltTen /= 2;
    }
    
    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame > 0)
    {
        prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame--;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame == 0)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx != u4CurKey &&
                (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx < prKeyIdxTbl->ui4_number_of_entry))
        {
            UINT32 u4MaxRepeatFrame;

            u4MaxRepeatFrame = (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps * 1000 * 3)/abs(i4PlaySpeed);
            if(u4MaxRepeatFrame == 0)
            {
                u4MaxRepeatFrame = 1;
            }

            if (i4PlaySpeed > 2000)
            {
                if ((u4CurKey < (prKeyIdxTbl->ui4_number_of_entry - 1)) &&
                        (prKeyIdxTbl->ui4_number_of_entry > MP4_MINIMUM_KEY_NUM))
                {
                    u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey+1].ui4_pts -
                        prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame =
                        (UINT32)(((u4PtsDif * u4FpsMuiltTen)/(90 * abs(i4PlaySpeed)))/10);
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps * 1000)/abs(i4PlaySpeed);
                }
            }
            else if (i4PlaySpeed < 0)
            {
                if (u4CurKey > 0 &&
                        (prKeyIdxTbl->ui4_number_of_entry > MP4_MINIMUM_KEY_NUM))
                {
                    u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts -
                        prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey-1].ui4_pts;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame =
                        (UINT32)(((u4PtsDif * u4FpsMuiltTen)/(90 * abs(i4PlaySpeed)))/10);
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps * 1000)/abs(i4PlaySpeed);
                } 
            }

            if (i4PlaySpeed <= -2000 || i4PlaySpeed > 2000)
            {
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = (u4MaxRepeatFrame > prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame) ?
                        (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame) : u4MaxRepeatFrame;
            }

            if ((u4CurKey == prKeyIdxTbl->ui4_number_of_entry - 1) ||
                    (u4CurKey == 0))
            {
                /*If abs(i4PlaySpped)<1000, the divisor will be 0.*/
                if (abs(i4PlaySpeed) > 1000)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame =
                        (MP4_FIX_REPEAT_FRAME/(abs(i4PlaySpeed)/1000));
                }
                else 
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = MP4_MAX_REPEAT_FRAME;
                }

                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame > MP4_MAX_REPEAT_FRAME)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = MP4_MAX_REPEAT_FRAME;
                }
                else if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame < MP4_MIN_REPEAT_FRAME)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = MP4_MIN_REPEAT_FRAME;
                }
            }

             LOG(5, "%s %d u4PtsDi =%ld Fps=%u EntryNum=%u CurrentIdx=%ld Repeatframe=%ld\n",
                __FUNCTION__,__LINE__,u4PtsDif, u4FpsMuiltTen/10, prKeyIdxTbl->ui4_number_of_entry,
                u4CurKey, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame);

            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame == 0)
            {
                /*Means EOS*/
                if (u4CurKey > prKeyIdxTbl->ui4_number_of_entry - 1 || u4CurKey == 0)
                {
                    LOG(5,"%s %d Meet Eos!\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
                /*For some case, the value is 0.xxxxxx*/
                else 
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = 1;
                }
            }

            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = u4CurKey;
            LOG(5, "Repeat Idx = %ld\n", prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx);

            return TRUE;
        }
        else
        {
            // Force to play the first key frame
            if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick)
            {  
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick = FALSE;
                if (i4PlaySpeed < 0)
                {
                    u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts -
                        prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey-1].ui4_pts;
                }
                else
                {
                    u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey+1].ui4_pts -
                        prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts;
                }
                
                if (i4PlaySpeed <= -2000 || i4PlaySpeed > 2000)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame = (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidFps * 1000)/abs(i4PlaySpeed);
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame =
                        (UINT32)(((u4PtsDif * u4FpsMuiltTen)/(90 * abs(i4PlaySpeed)))/10);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = u4CurKey;
                }
                
                //add log
                LOG(5,"Start Trick, RepeatFrame is %d, ptsDif = %d \n",prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4VidRepeatFrame,u4PtsDif);
                return TRUE;
            }

            LOG(1, "_SWDMX_Mp4CalculateRepeatFrame FALSE, fgStartTrick = %d !!!!!!!!!!!!!!\n",
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgStartTrick);

            return FALSE;
        }
    }

    return TRUE;
}


BOOL _SWDMX_Mp4AudioCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MP4_VERIFY_NULL_ERROR(prSwdmxInst);

    if(eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
        {
            LOG(5, "SWDMX_Mp4AudioCb Audio is ready\n");
            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts != 0xFFFFFFFF)
            {
                VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts);
                LOG(5, "SWDMX_Mp4AudioCb VDP Trigger PTS = 0x%08x\n",
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4TriggerAudPts = 0xFFFFFFFF;
            }

            if(prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause)
            {	
                _SWDMX_Mp4VideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgVDecIsPause = FALSE;
                prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgBlockVid = FALSE;
            }
            prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgAudIsReady = TRUE;
        }
        else
        {
            // Audio Only case
            LOG(5, "Trigger audio by _SWDMX_Mp4AudioCb\n");
            AUD_MMAoutEnable(0, TRUE);
            STC_StartStc(prSwdmxInst->u1StcId);
        }
    }
    else
    {
        // impossible
        LOG(11, "_SWDMX_Mp4AudioCb eAudioNotifyType != ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE\n");
    }

    return TRUE;
}

static void _SWDMX_Mp4VideoWakeUp(SWDMX_INFO_T* prSwdmxInst)
{
    SWDMX_MP4_INFO_T *prSwdmxMp4Info = NULL;
    SWDMX_MP4_VERIFY_NULL(prSwdmxInst);
	prSwdmxMp4Info = (SWDMX_MP4_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

	if(prSwdmxInst->rCurRangeReq.prDmxRangeInfo &&
             prSwdmxInst->fgCheckUnderFlow &&
             IS_NETWORK_SRC(prSwdmxInst->rCurRangeReq.prDmxRangeInfo->e_mm_src_type)
             && prSwdmxInst->fgSendUnderflow2MW)
 	{
 		LOG(0,"_SWDMX_Mp4VideoWakeUp, Underflow no need wakeup video!\n");
 		return;
 	}
	
	VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);

	LOG(5, "_SWDMX_Mp4VideoWakeUp\n");

	switch(prSwdmxMp4Info->u4VDecType)
    {
        case ENUM_SWDMX_VDEC_VC1:
        case ENUM_SWDMX_VDEC_WMV7:
        case ENUM_SWDMX_VDEC_WMV8:
        case ENUM_SWDMX_VDEC_WMV9:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_WMV);
            break;
        case ENUM_SWDMX_VDEC_DX311:
        case ENUM_SWDMX_VDEC_MPEG4:
        case ENUM_SWDMX_VDEC_H263:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
            break;
        case ENUM_SWDMX_VDEC_MJPEG:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MJPEG);
            break;
        case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MPV);
            break;
        case ENUM_SWDMX_VDEC_H264:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H264);
            break;
        case ENUM_SWDMX_VDEC_H265:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H265);
            break;
        default:
            LOG(1, "SWDMX_Mp4AudioCb Unknown Video Type\n");
            break;
    }
}

static void _SWDMX_Mp4SetFeederInvalid(SWDMX_INFO_T* prSwdmxInst, BOOL fgFlag)
{
    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgFeederInvalid = fgFlag;
}

static BOOL _SWDMX_Mp4CheckNextKeyTable(SWDMX_INFO_T* prSwdmxInst,INT32 i4Speed,UINT32* pu4CurIdx)
{
    SWDMX_MP4_INFO_T* prMp4Info = &prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

    if(prMp4Info->u4NumOfKeyTable >= 2)
    {
        if((*pu4CurIdx) == prMp4Info->parVidKeyIdxTblArray[prMp4Info->u4CurKeyTableIdx]->ui4_number_of_entry -2)
        {
            if(i4Speed > 2000)
            {
                if((prMp4Info->u4CurKeyTableIdx+1) < prMp4Info->u4NumOfKeyTable)
                {
                    prMp4Info->u4CurKeyTableIdx++;
                    prMp4Info->prVidKeyIdxTbl = prMp4Info->parVidKeyIdxTblArray[prMp4Info->u4CurKeyTableIdx];
                    *pu4CurIdx = 0;
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = *pu4CurIdx;
                    return TRUE;
                }
            }
            return FALSE;
        }
        else if((*pu4CurIdx) ==1)
        {
            if(i4Speed < 0)
            {
                if(prMp4Info->u4CurKeyTableIdx >= 1)
                {
                    prMp4Info->u4CurKeyTableIdx--;
                    prMp4Info->prVidKeyIdxTbl = prMp4Info->parVidKeyIdxTblArray[prMp4Info->u4CurKeyTableIdx];
                    *pu4CurIdx = ((prMp4Info->prVidKeyIdxTbl)->ui4_number_of_entry -2);
                    prSwdmxInst->rFmtInfo.rSwdmxMp4Info.u4CurRepeatFrameIdx = *pu4CurIdx;
                    return TRUE;
                }
            }
            return FALSE;
        }
        return FALSE;
    }
    else
    {
        return FALSE;
    }
}

void _SWDMX_Mp4SetKeyTableIdx(SWDMX_INFO_T* prSwdmxInst,MM_RANGE_ELMT_IDXTBL_T* prTargetKeyTable)
{
    UINT32 i=0;
    SWDMX_MP4_INFO_T* prMp4Info = &prSwdmxInst->rFmtInfo.rSwdmxMp4Info;

    for(i=0 ; i< prMp4Info->u4NumOfKeyTable ; i++)
    {
        if(prMp4Info->parVidKeyIdxTblArray[i] == prTargetKeyTable)
        {
            prMp4Info->u4CurKeyTableIdx = i;
            break;
        }
    }
}


