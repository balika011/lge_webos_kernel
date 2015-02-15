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
 * $RCSfile: dmx_mm_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_mm_mw.c
 *  Demux driver - middleware interface of Multi-Media
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef LINUX_TURNKEY_SOLUTION
#undef LINUX_TURNKEY_SOLUTION
#endif

#ifndef CC_53XX_SWDMX_V2
#define CC_53XX_SWDMX_V2
#endif

#define SWDMX_MW_LINUX

#include "dmx_mw.h"
#include "bagt_mw.h"
//#include "dmx_if.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_data_rm.h"
#endif


#ifdef ENABLE_MULTIMEDIA
//#include "swdmx_if.h"
#endif
#include "x_lint.h"
//#include "vdec_if.h"
//#include "vdp_if.h"
#if defined(CC_MT5387) || defined(CC_MT5363) || defined(CC_MT5365)
//#include "b2r_if.h"
#endif
//#include "vdec_drvif.h"
//#include "aud_drvif.h"
//#include "srm_drvif.h"
//#include "aud_if.h"

LINT_EXT_HEADER_BEGIN

#include "x_common.h"
#include "x_mm_common.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_demux.h"
#include "x_assert.h"
#include "x_aud_dec.h"
#include "dmx_mm_if.h"
#define __DMX_MW_API__  // avoid duplicate define for PSI MW and Driver
#include "x_printf.h"
#include "drv_comp_id.h"
#include "drv_name.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_param.h"
#endif

#ifdef SWDMX_MW_LINUX
//#include "aud_if.h"
#include "mtsys.h"
#include "mtswdmx.h"
#include "mtdmx.h"
#include "mtdmx_mwif.h"
#include "mtvdec_mwif.h"
#include "mtauddec.h"
#include "mtfeeder.h"
#include "mtstc.h"
#include "mtb2r.h"
#include "mtb2r_mwif.h"
#include "mtvdo.h"
#include "mtdrm.h"
#endif

//#define DEFINE_IS_LOG    SWDMX_IsLog
#include "x_debug.h"

LINT_EXT_HEADER_END

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

EXTERN BOOL _MW_DMX_MM_PES_Init(void);
EXTERN BOOL _MW_DMX_FlushAllCachePid(void);
EXTERN void _MW_DMX_SetCacheOn(BOOL fgCacheOn);

EXTERN UCHAR MPV_MW_GetEsId(UINT16 u2CompId);


//EXTERN void MW_MPV_SET_SWDMXSrcId(UINT8 u1SwdmxSrcId);
//EXTERN void MW_VDP_SET_SWDMXSrcId(UINT8 u1SwdmxSrcId);
//EXTERN void MW_AUD_SetSwdmxSrcId(UINT8 u1SwdmxSrcId);

#ifdef LINUX_TURNKEY_SOLUTION
extern VOID rm_copy_from_user(VOID* pv_kernel,VOID *pv_user,UINT32 size);
extern INT32 _CB_PutEvent_Adpt(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);
#endif


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MAX_STREAM_TYPE 3

//#define UNDEFINE_PARAM 0x00000000

#define DMX_MM_SEARCH_BY_COMP_ID      0
#define DMX_MM_SEARCH_BY_SRC_ID         1
#define DMX_MM_SEARCH_BY_SWDMX_ID   2

#ifdef CC_SKYPE_FINE_INSTANCE
#define DMX_MM_MAX_INSTNUM 0x5
#else
#define DMX_MM_MAX_INSTNUM 0x3
#endif
#define DMX_MM_UNKNOWN_IDX 0xFF

#ifndef DMX_MAX_NUM_PID_INDEX_TABLE
#define DMX_MAX_NUM_PID_INDEX_TABLE 32
#endif

#define DMX_MM_MAX_MARLIN_TS_KEY_INDEX     (1)
#define DMX_MM_MARLIN_AES_KEY_SWAP_MODE    (1)
#define DMX_MM_MARLIN_AES_KEY_LEN          (16)

#define DWORDSWAP(u4Tmp)        (((u4Tmp & 0xff) << 24) | ((u4Tmp & 0xff00) << 8) | ((u4Tmp & 0xff0000) >> 8) | ((u4Tmp & 0xff000000) >> 24))

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define DMX_MM_VERIFY_VALID_INSTNUM(id)                                    \
          do {                                              \
            if (id >= DMX_MM_MAX_INSTNUM) { \
                LOG(1, "%s Error Can't find Dmx Info Index\n", __FUNCTION__); \
                return; }      \
          } while (0)

#define DMX_MM_VERIFY_VALID_INSTNUM_ERROR(id, ret)                                    \
          do {                                              \
            if (id >= DMX_MM_MAX_INSTNUM) { \
                LOG(1, "%s Error Can't find Dmx Info Index\n", __FUNCTION__); \
                return ret; }      \
          } while (0)

#define IS_RAW_AUDIOES_SUBTYPE(x)   ((x == MEDIA_AUD_SUBTYPE_MP3) || \
    (x == MEDIA_AUD_SUBTYPE_MP2) || \
    (x == MEDIA_AUD_SUBTYPE_LPCM) || \
    (x == MEDIA_AUD_SUBTYPE_WAVE) || \
    (x == MEDIA_AUD_SUBTYPE_AAC) || \
    (x == MEDIA_AUD_SUBTYPE_AC3) || \
    (x == MEDIA_AUD_SUBTYPE_DTS) || \
    (x == MEDIA_AUD_SUBTYPE_FLAC) || \
    (x == MEDIA_AUD_SUBTYPE_AMR) || \
    (x == MEDIA_AUD_SUBTYPE_AWB) || \
    (x == MEDIA_AUD_SUBTYPE_AIF))

#define IS_AUD_STRM_TYPE(x)         (((x) == MTSWDMX_STRM_TYPE_AUD) || \
                                     ((x) == MTSWDMX_STRM_TYPE_AUD2))

#define IS_AUD_CONNECTED_COMP(x)    (((x)->u2CompId > 0) ? IS_AUD_STRM_TYPE((x)->u2DecType) : FALSE)

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
/* Notify conditions */
typedef enum
{
    DMX_COND_ERROR = -1,
    DMX_COND_CTRL_DONE
}   DMX_DEC_COND_T;

typedef VOID (*x_dmx_nfy_fct) (
    VOID*               pv_nfy_tag,
    DMX_DEC_COND_T      e_nfy_cond,
    UINT32              ui4_data_1,
    UINT32              ui4_data_2
    );

typedef struct _DMX_NFY_INFO_T
{
    VOID*              pv_tag;
    x_dmx_nfy_fct      pf_vid_dec_nfy;
    VOID*              pv_previous_tag;          /* OUTPUT */
    x_dmx_nfy_fct      pf_previous_vid_dec_nfy;  /* OUTPUT */
}   DMX_NFY_INFO_T;

#ifdef TIME_SHIFT_SUPPORT
typedef struct
{
    BOOL fgValid;
    UINT16 u2StreamID;
    UINT8 u1KeyIdx;
    MTDMX_DESC_MODE_T eDescMode;
} DMX_MM_KEY_INFO_T;
#endif    // TIME_SHIFT_SUPPORT

typedef struct
{
    BOOL                    fgValid;
    MM_DESC_CRYPT_INFO_T    rKeyInfo;
} DMX_MM_DESC_CRYPT_INFO_T;

/*
typedef struct
{
    BOOL fgIsSet;
    ENUM_SWDMX_ADEC_T eADecType;
    UINT32 u4SampleRate;
    UINT32 u4ChanNum;
}DMX_MM_AUD_INFO_T;

typedef struct
{
    BOOL fgIsSet;
    ENUM_SWDMX_VDEC_T eVDecType;
    UINT32 u4HdrAddr;
    UINT32 u4HdrLen;
}DMX_MM_VID_INFO_T;
*/

typedef struct
{
    BOOL fgAvailable;
    BOOL fgPreviousStateIsPause;
    BOOL fgPreviousStateIsTrick;
    BOOL fgPreviousStateIsSeek;
    BOOL fgFirstRange;
    BOOL fgKeyIsExist;
    BOOL fgIsThumbnailPb;
    BOOL fgVideoExist;
    BOOL fgAudCtrlDone;
    BOOL fgAudNotSupport;
    BOOL fgAudIsConn;
    BOOL fgVidIsConn;
    BOOL fgThumbnail;
    BOOL fgAudioDescription; // 0831    
    BOOL fgResetStcVal;
    BOOL fgFreeRun;
    UINT8 u1SwdmxSrcId;
    UINT8 u1DmxId;
    UINT8 u1VdecId;
    UINT8 u1AdecId;
    UINT8 u1Adec2Id; //0831
    UINT8 u1Adec3Id; //0831    
    UINT8 u1StcId;
    UINT8 u1B2rId;
    UINT8 u1AudCnt;  // 0831
    UINT8 u1AudCtrlId; // 0831    
    MTSWDMX_PLAY_MODE_T ePlayModeType;
    MM_PRIORITY_TYPE_T ePriority;

    BAGT_COMP_ID_T u2BagtCompId;
    UINT16 u2DmxCompId;
    UINT16 u2VideoCompId;

    UINT32 u4PreviousSpeed;
    UINT32 u4StcValue;
    UINT32 u4AudCtrl;
    UINT32 u4RangeID;

    UINT32 u4SourceType;

    HANDLE_T hDmxCtrlSema;
    HANDLE_T hFlushMutex;

    MEDIA_TYPE_T rContainerType;
    MEDIA_SUBTYPE_T rContainerSubType;
    MEDIA_TS_SUBTYPE_T rTsSubType;
    MEDIA_SVC_TYPE_T rSvcType;
    MEDIA_AUD_SUBTYPE_T rAudSubType;

    x_mm_range_nfy_fct pfnSwdmxRangeCb;
    VOID* pRangeCbTag;

#ifdef TIME_SHIFT_SUPPORT
    DMX_MM_KEY_INFO_T arDmxMMKeyInfo[DMX_MAX_NUM_PID_INDEX_TABLE];
#endif  // TIME_SHIFT_SUPPORT
#ifdef CC_3D_MM_DS_SUPPORT
    BOOL fgIs3DMultitrack;
    UINT8 u1VidCnt; //normal=1, multi-track/MVC=2
    UINT16 u2SubVidComp;
#endif
    DMX_MM_DESC_CRYPT_INFO_T rDescryptInfo;
} DMX_MM_MW_INFO_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgInit = FALSE;

static DMX_MM_DEC_INFO_T _rDecInfo = {0};
static DMX_MM_DEC_INFO_T _rDecInfo1 = {0};
static DMX_MM_DEC_INFO_T _rDecInfo2 = {0};
static DMX_MM_DEC_INFO_T _rDecInfo3 = {0}; // 0831
static DMX_MM_DEC_INFO_T _rDecInfo4 = {0}; // 0831
#ifdef CC_SKYPE_FINE_INSTANCE
static DMX_MM_MW_INFO_T _arDmxMMInfo[DMX_MM_MAX_INSTNUM];
#else
static DMX_MM_MW_INFO_T _arDmxMMInfo[DMX_MM_MAX_INSTNUM];
#endif

//static DMX_MM_AUD_INFO_T _rAudDecInfo = {0};
//static DMX_MM_VID_INFO_T _rVidDecInfo = {0};

static HANDLE_T _hDmxMMInfoMutex = (HANDLE_T)NULL;
static HANDLE_T _hDmxConnectMutex = (HANDLE_T)NULL;



/// Notification callback of scramble state
static DEMUX_NFY_INFO_T _arScrambleState[DMX_NUM_PID_INDEX];

/// State word for locking interrupt
static CRIT_STATE_T _rDmxLock;

/// Is interrupt locking? For making sure Lock()/Unlock() are not nested
static BOOL _fgDmxLocking = FALSE;


#if 0
static BOOL _bPreviousStateIsPause = FALSE;
static BOOL _bIsThumbnailPb=FALSE;
static UINT32 _u4PreviousSpeed = (UINT32)0xFFFFFFFF;
static BOOL _fgPreviousStateIsTrick = FALSE;
static BOOL _fgPreviousStateIsSeek = FALSE;
static UINT32 _ui4StcValue = 0;
static BOOL _bFirstRange = TRUE;

// Can't be an array
// Because the index will be hard to control
static DMX_MM_DEC_INFO_T _rDecInfo = {0};
static DMX_MM_DEC_INFO_T _rDecInfo1 = {0};
static DMX_MM_DEC_INFO_T _rDecInfo2 = {0};
static MEDIA_TYPE_T _rContainerType;
static MEDIA_CONTNR_SUBTYPE_T _rContainerSubType;
static MEDIA_TS_SUBTYPE_T _rTsSubType;
static MEDIA_AUD_SUBTYPE_T _rAudSubType;
static BOOL _fgKeyIsExist;
static BOOL _fgVideoExist = 0;
static UINT16 _u2VideoCompId = 0;
//static DMX_NFY_INFO_T _arDmxNfyInfo = {0};
static HANDLE_T _hDmxCtrlSema = 0;
static HANDLE_T _hFlushMutex = 0;
static BOOL _fgAudCtrlDone = TRUE;
static BOOL _fgAudNotSupport = FALSE;
static UINT32 _u4AudCtrl = 0;

static x_mm_range_nfy_fct _pfnSwdmxRangeCb = NULL;
static VOID* _pRangeCbTag = NULL;
static UINT32 _u4RangeID = 0;

#ifdef CC_FLASH_TEST
static BOOL _fgEsPtsConnect = FALSE;
#endif
#endif
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static UINT8 _MwDmxFindDmxIdx(UINT16 u2CompId, UINT8 u1IdType);
static UINT8 _MwDmxFindAvailDmxIdx(void);
static void _MwDmxResetDmxInfo(UINT8 u1Idx);

static VOID _MwDmxFlushCtrl(UINT8 u1DmxMMInfoIdx);
static VOID _MwDmxWaitCtrlDone(UINT8 u1DmxMMInfoIdx);

#ifdef LINUX_TURNKEY_SOLUTION
static MWDMX_NFY_PARAM mwdmx_param;
static x_mm_range_nfy_fct mwdmx_nfy;
static BOOL _MwDmx_nfy_fct(VOID*                  pv_nfy_tag,
                         MM_RANGE_COND_T          e_nfy_cond,
                         UINT32                 ui4_data_1,
                         UINT32                 ui4_data_2);
#endif

static BOOL _MwDmxOnMMSetVidChgCallback(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info);
static BOOL _MwDmxOnMMSetAudChgCallback(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info); 
static INT32 _MwDmxOnMMUpdateVidPid(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info);
static INT32 _MwDmxOnMMUpdateAudPid(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info);
static INT32 _MwDmxOnMMUpdateVdec(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info);
static INT32 _MwDmxOnMMUpdateAdec(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info);
static void _MwDmxMMLock(void);
static void _MwDmxMMUnlock(void);

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
extern VOID MM_AUTOTEST_SET_VDP_CALLBACK(UCHAR ucVdpId,UINT32 pfAtEosNfy,VOID *pv_Tag);
extern VOID MM_AUTOTEST_SET_MPV_CALLBACK(UCHAR ucEsId, UINT32 pfAtDecNfy,VOID *pv_Tag);
#ifdef VDEC_TIME_PROFILE
extern VOID MM_AUTOTEST_SET_CUR_FILE_DIR(UCHAR ucEsId, CHAR *ucAutoPlayCurFileDir);
extern VOID MM_AUTOTEST_SET_CUR_FILE_NAME(UCHAR ucEsId, CHAR *ucAutoPlayCurFileName);
#endif
#define MM_AUTOTEST_ADAPTER_NUM 2
typedef enum
{
  MM_AT_STATUS_INIT,
  MM_AT_STATUS_IDLE,
  MM_AT_STATUS_WAITTIMER
}MM_AUTOTEST_STATUS_T;

typedef struct
{
  HANDLE_T hCaseTimer;
  UINT32 u4CaseId;
  UINT32 u4TimerDuration;
  UINT32 u4GoldenTime;
  UINT32 u4DifTime;
  UINT32 u4StartTime;
  x_mm_autotest_nfy_fct pfAutoTestNfy;
  void *pvTag;
  MM_AUTOTEST_STATUS_T eStatus;
  MM_AUTOTEST_TYPE_T eCurCmd;
  HAL_TIME_T rTimeRecord;
  UINT8 u1B2rId;
  BOOL fgGetVEos;
  BOOL fgGetAEos;
  BOOL fgCrcCkRlt;
}MM_AUTOTEST_T;

static MM_AUTOTEST_T _arAutoTest[DMX_MM_MAX_INSTNUM] = {0};

VOID MM_AutoTestCallback(void *pvTag,UINT32 u4Type,UINT32 u4Param1,UINT32 u4Param2)
{
   MM_AUTOTEST_T *prAutoTesterr=(MM_AUTOTEST_T*)pvTag;
   UINT32 u4RetValue=0;
   MM_AUTOTEST_COND_T eRetCond;
   BOOL fgNeedCallback=FALSE;
   if(prAutoTesterr->eStatus==MM_AT_STATUS_WAITTIMER)
   {
       if(u4Param1==1)// from vdp
       {
         if(u4Type==0) //Eos
         {
             if(prAutoTesterr->eCurCmd==MM_AUTOTEST_GETEOSTIME)
             {
                 HAL_TIME_T rCurTime;
                 HAL_TIME_T rDeltTime;
                 HAL_GetTime(&rCurTime);
                 HAL_GetDeltaTime(&rDeltTime, &prAutoTesterr->rTimeRecord, &rCurTime);
                 u4RetValue= (rDeltTime.u4Seconds+1)*90000;
                 eRetCond=MM_AUTOTEST_COND_GOLDEN_DONE;
                 fgNeedCallback=TRUE;
                 prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;
             }
             else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_EOSCASE)
             {
                 #if 1
                 prAutoTesterr->fgGetVEos=TRUE;
                 fgNeedCallback=FALSE;
                 #else
                 u4RetValue= 1;
                 eRetCond=MM_AUTOTEST_COND_CASE_DONE;
                 fgNeedCallback=TRUE;
                 prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;
                 #endif

                 LOG(0," EOS done test cmd : %d",prAutoTesterr->eCurCmd);
             }
             else
             {
                 LOG(0,"Notify EOS form VDP, cmd : %d",prAutoTesterr->eCurCmd);
             }
         }
       }
       if(u4Param1==2)      //call from VDEC
       {

           if(u4Type==VDEC_COND_CRC_CHECK_DONE)
           {
               if(prAutoTesterr->eCurCmd==MM_AUTOTEST_GETCRCGOLDEN)
               {

                  u4RetValue = u4Param2;
                  eRetCond=MM_AUTOTEST_COND_GOLDEN_DONE;
                  fgNeedCallback=TRUE;
                  prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;

               }
               else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_CRCCHECK)
               {
                   #if 1
                   prAutoTesterr->fgCrcCkRlt = (u4Param2 ==prAutoTesterr->u4GoldenTime) ? 1:0;
                   fgNeedCallback=FALSE;
                   #else
                   u4RetValue= (u4Param2 ==prAutoTesterr->u4GoldenTime) ? 1:0;
                   eRetCond=MM_AUTOTEST_COND_CASE_DONE;
                   fgNeedCallback=TRUE;
                   prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;
                   #endif
                   LOG(0," CRC check done Testcmd: %d",prAutoTesterr->eCurCmd);

               }
               else
               {
                   LOG(0,"Notify EOS from VDEC, tes case cmd : %d",prAutoTesterr->eCurCmd);

             }
         }
       }
       if(fgNeedCallback)
       {
#ifndef LINUX_TURNKEY_SOLUTION
          prAutoTesterr->pfAutoTestNfy(prAutoTesterr->pvTag,eRetCond,prAutoTesterr->u4CaseId,u4RetValue);
#else
          mwdmx_param.pv_nfy_tag = prAutoTesterr->pvTag;
          mwdmx_param.e_nfy_cond = eRetCond;
          mwdmx_param.ui4_data_1 = prAutoTesterr->u4CaseId;
          mwdmx_param.ui4_data_2 = u4RetValue;
          mwdmx_param.pf_nfy = prAutoTesterr->pfAutoTestNfy;
          _CB_PutEvent_Adpt(CB_DRV_MWDMX_NFY, sizeof(MWDMX_NFY_PARAM), &mwdmx_param);
#endif
      }
   }
}

static VOID _AutoTestCaseTimer(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    MM_AUTOTEST_T *prAutoTesterr=(MM_AUTOTEST_T*)pv_tag;
    UINT32 u4CurTime,u4RetValue=0, u4TimeDelta =0;
    MM_AUTOTEST_COND_T eNfyCond=MM_AUTOTEST_COND_INVALID;
    if(prAutoTesterr->eCurCmd==MM_AUTOTEST_TIMECASE)
    {
        eNfyCond=MM_AUTOTEST_COND_CASE_DONE;
        MTB2R_GetPts(prAutoTesterr->u1B2rId, &u4CurTime);

        if(u4CurTime > prAutoTesterr->u4StartTime)
        {
            u4TimeDelta = u4CurTime - prAutoTesterr->u4StartTime;
        }
        else
        {
            u4TimeDelta = prAutoTesterr->u4StartTime - u4CurTime;
        }
        LOG(0,"test id : %d get test time (%d)\n",prAutoTesterr->u4CaseId, u4TimeDelta);

        u4TimeDelta += prAutoTesterr->u4DifTime;

        if(u4TimeDelta >= prAutoTesterr->u4GoldenTime
            &&(u4TimeDelta <= (prAutoTesterr->u4GoldenTime+(prAutoTesterr->u4DifTime*2))))
        {
           u4RetValue=1;
        }
        else
        {
          u4RetValue = 0;
          UINT32 mid = 0;
          UINT32 diff = 0;
          if(u4TimeDelta < prAutoTesterr->u4GoldenTime)
          {
              mid = (u4TimeDelta + prAutoTesterr->u4GoldenTime)/2;
              diff = prAutoTesterr->u4DifTime + (prAutoTesterr->u4GoldenTime - u4TimeDelta)/2 + 1;
          }
          else if(u4TimeDelta > (prAutoTesterr->u4GoldenTime + (prAutoTesterr->u4DifTime * 2)))
          {
              mid = (u4TimeDelta + prAutoTesterr->u4GoldenTime)/2 - prAutoTesterr->u4DifTime;
              diff = (u4TimeDelta - prAutoTesterr->u4GoldenTime)/2 + 1;
          }
          LOG(0,"Fail test id(%d) get test time (%d)\n", prAutoTesterr->u4CaseId, u4TimeDelta - prAutoTesterr->u4DifTime);
          LOG(0,"test id(%d) the new golden value (%d, %d)\n", prAutoTesterr->u4CaseId, mid, diff);
        }
    }
    else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_EOSCASE)
    {
        eNfyCond=MM_AUTOTEST_COND_CASE_DONE;
        if(prAutoTesterr->fgGetVEos)
        {
            u4RetValue=1;
        }
    }
    else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_MAKEGOLDEN)
    {

        eNfyCond=MM_AUTOTEST_COND_GOLDEN_DONE;
        MTB2R_GetPts(prAutoTesterr->u1B2rId, &u4CurTime);

        if(u4CurTime > prAutoTesterr->u4StartTime)
        {
            u4RetValue = u4CurTime - prAutoTesterr->u4StartTime;
        }
        else
        {
            u4RetValue = prAutoTesterr->u4StartTime - u4CurTime;
        }
    }
    else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_PLAYSTOP)
    {
        eNfyCond=MM_AUTOTEST_COND_CASE_DONE;
        u4RetValue=1;
    }
    else if(prAutoTesterr->eCurCmd==MM_AUTOTEST_CRCCHECK)
    {
        eNfyCond=MM_AUTOTEST_COND_CASE_DONE;
        u4RetValue= prAutoTesterr->fgCrcCkRlt;

    }
    if(prAutoTesterr->pfAutoTestNfy)
    {
#ifndef LINUX_TURNKEY_SOLUTION
        prAutoTesterr->pfAutoTestNfy(prAutoTesterr->pvTag,eNfyCond,prAutoTesterr->u4CaseId,u4RetValue);
#else
        mwdmx_param.pv_nfy_tag = prAutoTesterr->pvTag;
        mwdmx_param.e_nfy_cond = eNfyCond;
        mwdmx_param.ui4_data_1 = prAutoTesterr->u4CaseId;
        mwdmx_param.ui4_data_2 = u4RetValue;
        mwdmx_param.pf_nfy = prAutoTesterr->pfAutoTestNfy;
        _CB_PutEvent_Adpt(CB_DRV_MWDMX_NFY, sizeof(MWDMX_NFY_PARAM), &mwdmx_param);
#endif
    }
    prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;
}

static INT32 _MultiMediaAutoTest(DRV_COMP_ID_T* pt_comp_id, const VOID* pv_set_info)
{
    UINT8 u1AdapterId = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    MM_AUTOTEST_INFO_T* prAutestInfo = (MM_AUTOTEST_INFO_T*)pv_set_info;
    MM_AUTOTEST_T *prAutoTesterr=NULL;
    INT32 i4Ret = RMR_OK;
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1AdapterId, RMR_DRV_INV_SET_INFO);
    prAutoTesterr=&_arAutoTest[u1AdapterId];
    prAutoTesterr->eCurCmd=prAutestInfo->e_cmd_type;
    switch (prAutestInfo->e_cmd_type)
    {
       case MM_AUTOTEST_TIMECASE:
       case MM_AUTOTEST_CRCCHECK:
       {
          prAutoTesterr->u4CaseId=prAutestInfo->u4rParam[0];
          prAutoTesterr->u4TimerDuration=prAutestInfo->u4rParam[1];
          prAutoTesterr->u4GoldenTime=prAutestInfo->u4rParam[2];
          prAutoTesterr->u4DifTime=prAutestInfo->u4rParam[3];
          ASSERT(prAutoTesterr->u4TimerDuration!=0);
          ASSERT(prAutoTesterr->eStatus==MM_AT_STATUS_IDLE);
          if(prAutestInfo->e_cmd_type == MM_AUTOTEST_CRCCHECK)
          {
              MTVDEC_SetCrcGolden(0, prAutoTesterr->u4GoldenTime); //set crc golden to vdec
          }
          if(prAutestInfo->e_cmd_type == MM_AUTOTEST_TIMECASE)
          {
             MTB2R_GetPts(prAutoTesterr->u1B2rId, &(prAutoTesterr->u4StartTime));
          }
          prAutoTesterr->eStatus=MM_AT_STATUS_WAITTIMER;
          VERIFY(x_timer_start(prAutoTesterr->hCaseTimer, prAutoTesterr->u4TimerDuration, X_TIMER_FLAG_ONCE,
           _AutoTestCaseTimer, (VOID*)prAutoTesterr) == OSR_OK);

          break;
       }
       case MM_AUTOTEST_EOSCASE:
       {
          prAutoTesterr->u4CaseId=prAutestInfo->u4rParam[0];
          //prAutoTesterr->u4TimerDuration=prAutestInfo->u4rParam[1];
          //prAutoTesterr->u4GoldenTime=prAutestInfo->u4rParam[2];
          //prAutoTesterr->u4DifTime=prAutestInfo->u4rParam[3];
          prAutoTesterr->u4TimerDuration = (prAutestInfo->u4rParam[2] + prAutestInfo->u4rParam[3])/90; // for EOS test case , the golden time is EOS receive time.
          ASSERT(prAutoTesterr->u4TimerDuration!=0);
          ASSERT(prAutoTesterr->eStatus==MM_AT_STATUS_IDLE);
          prAutoTesterr->eStatus=MM_AT_STATUS_WAITTIMER;

          MM_AUTOTEST_SET_VDP_CALLBACK(0,(UINT32)MM_AutoTestCallback,(VOID *)prAutoTesterr);
          VERIFY(x_timer_start(prAutoTesterr->hCaseTimer, prAutoTesterr->u4TimerDuration, X_TIMER_FLAG_ONCE,
           _AutoTestCaseTimer, (VOID*)prAutoTesterr) == OSR_OK);

          break;
       }
       case MM_AUTOTEST_MAKEGOLDEN:
       case MM_AUTOTEST_PLAYSTOP:
       {
          prAutoTesterr->u4CaseId=prAutestInfo->u4rParam[0];
          prAutoTesterr->u4TimerDuration=prAutestInfo->u4rParam[1];
          ASSERT(prAutoTesterr->u4TimerDuration!=0);
          ASSERT(prAutoTesterr->eStatus==MM_AT_STATUS_IDLE);
          if(prAutestInfo->e_cmd_type == MM_AUTOTEST_MAKEGOLDEN)
          {
             MTB2R_GetPts(prAutoTesterr->u1B2rId, &(prAutoTesterr->u4StartTime));
          }
          prAutoTesterr->eStatus=MM_AT_STATUS_WAITTIMER;
          VERIFY(x_timer_start(prAutoTesterr->hCaseTimer, prAutoTesterr->u4TimerDuration, X_TIMER_FLAG_ONCE,
          _AutoTestCaseTimer, (VOID*)prAutoTesterr) == OSR_OK);
          break;
       }
       case MM_AUTOTEST_GETEOSTIME:
       {
           prAutoTesterr->u4CaseId=prAutestInfo->u4rParam[0];
           prAutoTesterr->eStatus=MM_AT_STATUS_WAITTIMER;

           HAL_GetTime(&prAutoTesterr->rTimeRecord);
           MM_AUTOTEST_SET_VDP_CALLBACK(0,(UINT32)MM_AutoTestCallback,(VOID *)prAutoTesterr);
           break;
       }
       case MM_AUTOTEST_GETCRCGOLDEN:
       {

           prAutoTesterr->u4CaseId=prAutestInfo->u4rParam[0];
           prAutoTesterr->eStatus=MM_AT_STATUS_WAITTIMER;

           MM_AUTOTEST_SET_MPV_CALLBACK(0,(UINT32)MM_AutoTestCallback,(VOID *)prAutoTesterr);

       }
       case MM_AUTOTEST_RESET:
       {
          break;
       }
       case MM_AUTOTEST_INIT:
       {
          if(!prAutoTesterr->hCaseTimer)
          {
              x_memset(prAutoTesterr,0,sizeof(MM_AUTOTEST_T));
              VERIFY(x_timer_create(&prAutoTesterr->hCaseTimer) == OSR_OK);
          }
          prAutoTesterr->pfAutoTestNfy=(x_mm_autotest_nfy_fct)prAutestInfo->u4rParam[0];
          prAutoTesterr->pvTag=(void *)prAutestInfo->u4rParam[1];
          prAutoTesterr->eStatus=MM_AT_STATUS_IDLE;
          prAutoTesterr->u1B2rId=0;
          MM_AUTOTEST_SET_VDP_CALLBACK(0,(UINT32)MM_AutoTestCallback,(VOID *)prAutoTesterr);
          MM_AUTOTEST_SET_MPV_CALLBACK(0,(UINT32)MM_AutoTestCallback,(VOID *)prAutoTesterr);
#ifdef VDEC_TIME_PROFILE
            MM_AUTOTEST_SET_CUR_FILE_DIR(0, &(prAutestInfo->ucAutoPlayCurFileDir[0]));
            MM_AUTOTEST_SET_CUR_FILE_NAME(0, &(prAutestInfo->ucAutoPlayCurFileName[0]));
#endif
          MTB2R_SetThumbnailMode(0, 0);
          MTSYS_SRM_SetMmMode(SRM_MM_MODE_TYPE_VIDEO);
          break;
       }
       default:
          LOG(0,"AutoTest Unknow command %d\n",prAutestInfo->e_cmd_type);
          break;
    }

    return i4Ret;
}
#endif

static void MW_DMX_STC_SetSyncMode(UINT8 u1DmxMMInfoIdx, UINT32 u4SyncMode)
{
    if(_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist)
    {
        MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
            u4SyncMode);
            
        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription)
        {
            if (MTSTC_AV_SYNC_MODE_AUDIO_MASTER == u4SyncMode )
            {
                u4SyncMode = MTSTC_AV_SYNC_MODE_SYSTEM_MASTER; /* the second audio set video master */
            }
            #if 0
            // AD use, AUX, Third audio decoders
            MTB2R_SetSyncMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, 
            _arDmxMMInfo[u1DmxMMInfoIdx].u1Adec2Id,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1Adec3Id,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
            u4SyncMode);
            #endif
            
        }

    }
    else
    {
        MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId,
            0xFF, // invalid VDEC ES ID
            u4SyncMode);
    }
}

VOID MW_DMX_NotifyStepDone(UINT32 u4SwdmxSrcId, UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret)
{
    BOOL fgRet;
    UINT8 u1DmxMMInfoIdx;

    UNUSED(u4VdpId);
    UNUSED(u4PTS);
    UNUSED(u4Ret);
    LOG(5, "!!!! MW_DMX_NotifyStepDone\n");

    u1DmxMMInfoIdx = _MwDmxFindDmxIdx(u4SwdmxSrcId, DMX_MM_SEARCH_BY_SWDMX_ID);
    DMX_MM_VERIFY_VALID_INSTNUM(u1DmxMMInfoIdx);
    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        u4SwdmxSrcId);

    MTSTC_StopStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);

    // actually this should be done in every format
    // we have to evaluate the performance then open this for all format
    if( (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
        ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
         (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
    {
        // we have to stop the stream,
        // or we will notfiy MW other message(like no signal).
        // mw_if2 was used for mtal, Can not use this now(due to single _cbThd / sema dead lock)
        #if 0
        _MwDmxFlushCtrl(u1DmxMMInfoIdx);
        fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"MW_DMX_NotifyStepDone SWDMX_Pause fail\n");
        }
        _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
        #endif // 0
        // MTSWDMX_WaitForPause is a sync function
        fgRet = MTSWDMX_WaitForPause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 10000);
        if(fgRet != MTR_OK)
        {
            LOG(1,"MW_DMX_NotifyStepDone MTSWDMX_WaitForPause fail\n");
        }
    }


#ifndef LINUX_TURNKEY_SOLUTION
    if(_arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb)
#else
    if(mwdmx_nfy)
#endif
    {
        if(u4Ret == MTB2R_SEEK_STEP_OK)
        {
#ifndef LINUX_TURNKEY_SOLUTION
            _arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_DONE,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#else
            _MwDmx_nfy_fct(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_DONE,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#endif
            LOG(3, "MW_DMX_NotifyStepDone MM_RANGE_SEEK_DONE\n");
        }
        else // if(u4Ret == VDP_SEEK_STEP_NO_DATA)
        {
#ifndef LINUX_TURNKEY_SOLUTION
            _arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_NO_DATA,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#else
            _MwDmx_nfy_fct(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_NO_DATA,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#endif
            LOG(3, "MW_DMX_NotifyStepDone MM_RANGE_SEEK_NO_DATA\n");
        }
    }
    else
    {
        LOG(1, "MW_DMX_NotifyStepDone not sent info\n");
    }
}

static VOID _MW_DMX_NotifySeekDone(UINT32 u4SwdmxSrcId, UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret)
{
    UINT8 u1DmxMMInfoIdx;

    UNUSED(u4VdpId);
    UNUSED(u4PTS);
    UNUSED(u4Ret);

    u1DmxMMInfoIdx = _MwDmxFindDmxIdx(u4SwdmxSrcId, DMX_MM_SEARCH_BY_SWDMX_ID);
    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        u4SwdmxSrcId);

    if (u1DmxMMInfoIdx >= DMX_MM_MAX_INSTNUM)
    {
        LOG(3, "%s wrong swdmx src id\n", __FUNCTION__);
        return;
    }

    Printf("!!!! _MW_DMX_NotifySeekDone\n");

#ifndef LINUX_TURNKEY_SOLUTION
    if(_arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb)
#else
    if(mwdmx_nfy)
#endif
    {
        if(u4Ret == MTB2R_SEEK_STEP_OK)
        {
#ifndef LINUX_TURNKEY_SOLUTION
            _arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_DONE,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#else

            _MwDmx_nfy_fct(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_DONE,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#endif
            LOG(3, "_MW_DMX_NotifySeekDone MM_RANGE_SEEK_DONE\n");
        }
        else // if(u4Ret == VDP_SEEK_STEP_NO_DATA)
        {
#ifndef LINUX_TURNKEY_SOLUTION
            _arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_NO_DATA,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#else
            _MwDmx_nfy_fct(_arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag,
                MM_RANGE_SEEK_NO_DATA,
                _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID,
                0xFFFFFFFF);
#endif
            LOG(3, "_MW_DMX_NotifySeekDone MM_RANGE_SEEK_NO_DATA\n");
        }
    }
    else
    {
        LOG(1, "_MW_DMX_NotifySeekDone not sent info\n");
    }
}

static void _MwDmxInfoLock(void)
{
    if (!_hDmxConnectMutex)
    {
        VERIFY(x_sema_create(&_hDmxMMInfoMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

    VERIFY(x_sema_lock(_hDmxMMInfoMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
}

static void _MwDmxInfoUnlock(void)
{
    VERIFY(x_sema_unlock(_hDmxMMInfoMutex) == OSR_OK);
}

static void _MwDmxConnectLock(void)
{
    if (!_hDmxConnectMutex)
    {
        VERIFY(x_sema_create(&_hDmxConnectMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

    VERIFY(x_sema_lock(_hDmxConnectMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
}

static void _MwDmxConnectUnlock(void)
{
    VERIFY(x_sema_unlock(_hDmxConnectMutex) == OSR_OK);
}

static VOID _MwDmxNtyFct(VOID* pv_nfy_tag, MTSWDMX_COND_T e_nfy_cond,
    UINT32 ui4_data_1, UINT32 ui4_data_2)
{
    UINT8 i;

    for (i = 0; i < DMX_MM_MAX_INSTNUM; i++)
    {
        if (_arDmxMMInfo[i].u1SwdmxSrcId== (UINT8)ui4_data_1)
        {
            if(e_nfy_cond == MTSWDMX_COND_CTRL_DONE)
            {
                VERIFY (x_sema_unlock(_arDmxMMInfo[i].hDmxCtrlSema) == OSR_OK);
                Printf("_MwDmxNtyFct unlock sema succ\n");
            }
            else
            {
                LOG(3, "_MwDmxNtyFct unknow ctrl %d\n", (UINT32)e_nfy_cond);
                Printf("_MwDmxNtyFct unknow ctrl %d\n", (UINT32)e_nfy_cond);
            }
            return;
        }
    }

    Printf("_MwDmxNtyFct Can't find swdmx id\n");
}

static UINT8 _MwDmxFindDmxIdx(UINT16 u2CompId, UINT8 u1IdType)
{
    UINT8 i;

    _MwDmxInfoLock();

    switch (u1IdType)
    {
        case DMX_MM_SEARCH_BY_COMP_ID:
            for (i = 0; i < DMX_MM_MAX_INSTNUM; i++)
            {
                if (_arDmxMMInfo[i].u2DmxCompId == u2CompId)
                {
                    _MwDmxInfoUnlock();
                    return i;
                }
            }
            LOG(1, "%s Error Can't find DmxCompId(%d) Info Index\n", __FUNCTION__, u2CompId);
            break;
        case DMX_MM_SEARCH_BY_SRC_ID:
            for (i = 0; i < DMX_MM_MAX_INSTNUM; i++)
            {
                if (_arDmxMMInfo[i].u2BagtCompId == (BAGT_COMP_ID_T)u2CompId)
                {
                    _MwDmxInfoUnlock();
                    return i;
                }
            }
            LOG(1, "%s Error Can't find SrcCompId(%d) Info Index\n", __FUNCTION__, u2CompId);
            break;
        case DMX_MM_SEARCH_BY_SWDMX_ID:
            for (i = 0; i < DMX_MM_MAX_INSTNUM; i++)
            {
                if (_arDmxMMInfo[i].u1SwdmxSrcId == u2CompId)
                {
                    _MwDmxInfoUnlock();
                    return i;
                }
            }
            LOG(1, "%s Error Can't find SwdmxId(%d) Info Index\n", __FUNCTION__, u2CompId);
            break;
        default:
            LOG(1, "Unknown search type\n");
            break;
    }

    _MwDmxInfoUnlock();
    LOG(1, "%s Error Can't find Dmx Info Index\n", __FUNCTION__);
    return DMX_MM_UNKNOWN_IDX;
}

static UINT8 _MwDmxFindAvailDmxIdx(void)
{
    UINT8 i;

    _MwDmxInfoLock();

    for (i = 0; i < DMX_MM_MAX_INSTNUM; i++)
    {
        if (_arDmxMMInfo[i].fgAvailable)
        {
            LOG(5, "_MwDmxFindAvailDmxIdx find the available dmx index %d\n", i);
            _MwDmxInfoUnlock();
            return i;
        }
    }

    LOG(3, "_MwDmxFindAvailDmxIdx doesn't find available dmx index\n");
    _MwDmxInfoUnlock();
    return DMX_MM_UNKNOWN_IDX;
}

static void _MwDmxResetDmxInfo(UINT8 u1Idx)
{
    _MwDmxInfoLock();

    if (u1Idx >= DMX_MM_MAX_INSTNUM)
    {
        LOG(3, "Error!!! Wrong idx for _MwDmxResetDmxInfo\n");
    }

    LOG(5, "%s  u1DmxMMInfoIdx = %d\n",
        __FUNCTION__,
        u1Idx);

    _arDmxMMInfo[u1Idx].fgAvailable = TRUE;
    _arDmxMMInfo[u1Idx].fgAudioDescription= FALSE; //0831     
    _arDmxMMInfo[u1Idx].fgPreviousStateIsPause = FALSE;
    _arDmxMMInfo[u1Idx].fgPreviousStateIsTrick = FALSE;
    _arDmxMMInfo[u1Idx].fgPreviousStateIsSeek = FALSE;
    _arDmxMMInfo[u1Idx].fgFirstRange = TRUE;
    _arDmxMMInfo[u1Idx].fgKeyIsExist = FALSE;
    _arDmxMMInfo[u1Idx].fgVideoExist = FALSE;
    _arDmxMMInfo[u1Idx].fgAudCtrlDone = TRUE;
    _arDmxMMInfo[u1Idx].fgAudNotSupport = FALSE;
    _arDmxMMInfo[u1Idx].fgVidIsConn = FALSE;
    _arDmxMMInfo[u1Idx].fgAudIsConn = FALSE;
    _arDmxMMInfo[u1Idx].u1AudCnt = 0; //0831
    _arDmxMMInfo[u1Idx].u1AudCtrlId = 0;//0831
    _arDmxMMInfo[u1Idx].u1SwdmxSrcId = MTSWDMX_UNKNOWN_SOURCE;

    _arDmxMMInfo[u1Idx].u2BagtCompId = BAGT_COMP_1;
    _arDmxMMInfo[u1Idx].u2DmxCompId = 0;
    _arDmxMMInfo[u1Idx].u2VideoCompId = 0;

    _arDmxMMInfo[u1Idx].u4PreviousSpeed = (UINT32)0xFFFFFFFF;
    _arDmxMMInfo[u1Idx].ePlayModeType=MTSWDMX_PLAY_MODE_NORMAL;
    _arDmxMMInfo[u1Idx].u4StcValue = 0;
    _arDmxMMInfo[u1Idx].u4AudCtrl = 0xFFFFFFFF;
    _arDmxMMInfo[u1Idx].u4RangeID = 0;
    _arDmxMMInfo[u1Idx].u1StcId = u1Idx;
    _arDmxMMInfo[u1Idx].fgFreeRun = FALSE;
    _arDmxMMInfo[u1Idx].u1B2rId = MTB2R_B2R_INVALID;

#ifdef CC_3D_MM_DS_SUPPORT
    _arDmxMMInfo[u1Idx].u1VdecId = 0;
    _arDmxMMInfo[u1Idx].u1VidCnt = 0;
    _arDmxMMInfo[u1Idx].fgIs3DMultitrack = FALSE;
    _arDmxMMInfo[u1Idx].u2SubVidComp = 0;
#endif

    x_memset((void *)&(_arDmxMMInfo[u1Idx].rDescryptInfo), 0, sizeof(_arDmxMMInfo[u1Idx].rDescryptInfo));
    
    _MwDmxInfoUnlock();
}

static BOOL _MwDmxSetDescryptInfo(DMX_MM_MW_INFO_T *prDmxMmMwInfo, UINT16 u2DecType)
{
    UNUSED(u2DecType);

    if (NULL == prDmxMmMwInfo)
    {
        Printf ("prInstInfo is NULL... ...\n");
        return FALSE;
    }

    if ( (prDmxMmMwInfo->rDescryptInfo.fgValid) &&
         (prDmxMmMwInfo->rDescryptInfo.rKeyInfo.t_crypto_info.e_mm_crypt_type==MM_CRYPT_TYPE_RSV) )
    {
        #if 0
         UINT32 u4Pidx = 0xFFFFFFFF;
         MTSWDMX_SET_DECODER_TYPE eGetType = MTSWDMX_SET_MAX;
         
         switch (u2DecType)
         {
            // Notes: 3D type not set current
            case    MTSWDMX_STRM_TYPE_VID:
                eGetType = MTSWDMX_GET_VIDEO_PIDX;
                break;
            case    MTSWDMX_STRM_TYPE_AUD:
                eGetType = MTSWDMX_GET_AUDIO_PIDX;
                break;
            default:
                return FALSE;
         }
         MTSWDMX_GetInfo ( prDmxMmMwInfo->u1SwdmxSrcId, eGetType, &u4Pidx, 0, 0 );
       
        if ((UINT8)u4Pidx < 0xFF)
        {
            MTDRM_HDCP2_SetRiv_Pid ( prDmxMmMwInfo->rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved,
                                     (UINT16)(u4Pidx));
        }

        Printf("[Info] Set HDCP key. Pidx(%d)\n", u4Pidx);
        #endif
        MTSWDMX_EnableHdcpKey(prDmxMmMwInfo->u1SwdmxSrcId,u2DecType);
        
    }

    return TRUE;
}
#if 0
static VOID _MwDmxSetInfo2Swdmx(UINT8 u1DmxMMInfoIdx)
{
    DMX_MM_VERIFY_VALID_INSTNUM(u1DmxMMInfoIdx);

    // Set media format here
    if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER)
    {
        switch(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype)
        {
            case MEDIA_CONTNR_SUBTYPE_AVI:
              SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_AVI);
              break;
            case MEDIA_CONTNR_SUBTYPE_ASF:
              SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_VC1_WMV);
              break;
            case MEDIA_CONTNR_SUBTYPE_MP4:
              if (_arDmxMMInfo[u1DmxMMInfoIdx].rSvcType == MEDIA_SVC_TYPE_AUDIO_ONLY)
              {
                  SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_M4A);
              }
              else
              {
                  SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MP4);
              }
              break;
            case MEDIA_CONTNR_SUBTYPE_MATROSKA:
              SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MKV);
              break;
            default:
              LOG(3, "Unknown Format\n");
              return;
        }
    }
    else if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS)
    {
        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_ps_subtype
            == MEDIA_PS_SUBTYPE_DAT)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG1_DAT);
        }
        else
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_PS);
        }
    }
    else if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
    {
        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_188)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_TS);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_TS_192);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192_ZERO)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_TS_ZERO_192);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_TS_TIME_SHIFT);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_TS unknown\n");
        }

        if(!SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,(UINT8)eSWDMX_PID_INDEX_START,
            (UINT32)DMX_PES_COMP_ID_START, 0, 0))
        {
            LOG(3,"_MwDmxOnMMSetMediaFormat eSWDMX_PID_INDEX_START fail\n");
        }
    }
    else if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_AUDIO_ES)
    {
#if 0
#if defined(CC_MT5387) || defined(CC_MT5363)
        if (FeederSetMMP(FEEDER_PROGRAM_SOURCE) != (INT32)FEEDER_E_OK)
        {
            LOG(0,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_AUDIO_ES set MMP buffer failed\n");
        }
#endif
#endif
        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
            == MEDIA_AUD_SUBTYPE_WMA)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_WMA_AUDIO_ES);
        }
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_MP3)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_MP2)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_LPCM)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_WAVE)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_AAC)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_AIF)
                ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype
                 == MEDIA_AUD_SUBTYPE_AC3)
               )
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_RAW_AUDIO_ES);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_AUDIO_ES unknown\n");
        }
    }
    else if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES)
    {
        if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M1V) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M2V))
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG2_VIDEO_ES);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_H264)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_H264_VIDEO_ES);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M4V)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_MPEG4_VIDEO_ES);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_VC1)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_VC1_ES);
        }
        else if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_AVS)
        {
            SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_AVS_VIDEO_ES);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_VIDEO_ES unknown\n");
            //fgRet = FALSE;
        }
    }

#if 0
    if (_arDmxMMInfo[u1DmxMMInfoIdx].fgVidIsConn)
    {
        MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if (_rVidDecInfo.fgIsSet)
        {
            SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                eSWDMX_SET_VIDEOTYPE,
                _rVidDecInfo.eVDecType,
                0,
                0);

            if (_rVidDecInfo.u4HdrLen != 0)
            {
                SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                    eSWDMX_SET_VIDSEQHEADER,
                    _rVidDecInfo.u4HdrAddr,
                    _rVidDecInfo.u4HdrLen,
                    0);
            }
        }
    }

    if (_arDmxMMInfo[u1DmxMMInfoIdx].fgAudIsConn)
    {
        MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if (_rAudDecInfo.fgIsSet)
        {
            SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                eSWDMX_SET_AUDIOTYPE,
                _rAudDecInfo.eADecType,
                _rAudDecInfo.u4SampleRate,
                _rAudDecInfo.u4ChanNum);
        }
    }
#endif
}
#endif

#if 0
void _MwDmxLockFlushMutex(void)
{
    if(!_arDmxMMInfo[_u1DmxMMInfoIdx].hFlushMutex)
    {
        return;
    }
    VERIFY(x_sema_lock(_arDmxMMInfo[_u1DmxMMInfoIdx].hFlushMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
}

void _MwDmxUnlockFlushMutex(void)
{
    if(!_arDmxMMInfo[_u1DmxMMInfoIdx].hFlushMutex)
    {
        return;
    }
    VERIFY(x_sema_unlock(_arDmxMMInfo[_u1DmxMMInfoIdx].hFlushMutex) == OSR_OK);
}
#endif

static VOID _MwDmxFlushCtrl(UINT8 u1DmxMMInfoIdx)
{
    INT32 i4Ret;
    i4Ret = x_sema_lock(_arDmxMMInfo[u1DmxMMInfoIdx].hDmxCtrlSema, X_SEMA_OPTION_NOWAIT);
    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_WOULD_BLOCK));
}

static VOID _MwDmxWaitCtrlDone(UINT8 u1DmxMMInfoIdx)
{
    INT32 i4Ret;
    i4Ret = x_sema_lock_timeout(_arDmxMMInfo[u1DmxMMInfoIdx].hDmxCtrlSema, 10000);
    if (i4Ret != OSR_OK)
    {
        LOG(0, "_MwDmxWaitCtrlDone timeout\n");
        Printf("_MwDmxWaitCtrlDone timeout\n");
    }
}

static INT32 _MwDmxOnSetNfyFct(UINT8 u1DmxMMInfoIdx,
    const MTSWDMX_NFY_INFO_T* ptdmxNfyInfo,
    SIZE_T z_set_info_len)
{
    INT32 i4Ret = RMR_OK;

    ASSERT(ptdmxNfyInfo != NULL);

    //_arDmxNfyInfo.pv_tag = ptdmxNfyInfo->pv_tag;
    //_arDmxNfyInfo.pf_vid_dec_nfy = ptdmxNfyInfo->pf_vid_dec_nfy;

    MTSWDMX_SetDmxNfy(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (MTSWDMX_NFY_INFO_T*)ptdmxNfyInfo);

    return i4Ret;
}


VOID *_MwDmxMalloc(SIZE_T z_size)
{
    VOID *pvAddr = x_mem_alloc(z_size);
    LOG(3, "malloc: 0x%p\n", pvAddr);
    return pvAddr;
}


VOID _MwDmxFree(VOID *pv_mem)
{
    LOG(3, "free: 0x%p\n", pv_mem);
    x_mem_free(pv_mem);
}


VOID *_MwDmxCalloc(UINT32 ui4_num_unit, SIZE_T z_unit_size)
{
    VOID *pvAddr = x_mem_calloc(ui4_num_unit, z_unit_size);
    LOG(3, "calloc: 0x%p\n", pvAddr);
    return pvAddr;
}


static INT32 _MwDmxOnMMSetStreamID(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    STREAM_ID_T* pStrmID = (STREAM_ID_T*)pv_set_info;
    BOOL fgRet;
    UINT32 u4StrmId;
    UINT32 u4SubStrmId;

    if(pv_set_info == NULL)
    {
        return RMR_DRV_INV_SET_INFO;
    }
#ifdef LINUX_TURNKEY_SOLUTION
    else
    {
        pStrmID = _MwDmxMalloc(sizeof(STREAM_ID_T));
        if(pStrmID == NULL)
        {
            return RMR_NOT_ENOUGH_MEM;
        }
        x_memset(pStrmID,0,sizeof(STREAM_ID_T));
        rm_copy_from_user((VOID *)pStrmID, (VOID *)pv_set_info, sizeof(STREAM_ID_T));
    }
#endif

    fgRet = TRUE;
    u4StrmId = 0;
    u4SubStrmId = 0;

    if (pStrmID->e_type == STREAM_ID_TYPE_AVI)
    {
        u4StrmId = pStrmID->u.t_stm_avi_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_ASF)
    {
        u4StrmId = pStrmID->u.t_stm_asf_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_MP4)
    {
        u4StrmId = pStrmID->u.t_stm_mp4_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_PS)
    {
        u4StrmId = (pStrmID->u.t_stm_ps_id.ui2_stm_id>>8);
        u4SubStrmId = (pStrmID->u.t_stm_ps_id.ui2_stm_id & 0xFF);
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_TS)
    {
        u4StrmId = pStrmID->u.t_stm_ts_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_MKV)
    {
        u4StrmId = pStrmID->u.t_stm_mkv_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_RM)
    {
        u4StrmId = pStrmID->u.t_stm_rm_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_ES)
    {
        u4StrmId = pStrmID->u.t_stm_es_id;
    }
    else if(pStrmID->e_type==STREAM_ID_TYPE_FLV)
    {
        u4StrmId = pStrmID->u.t_stm_flv_id;
    }
    else if (pStrmID->e_type == STREAM_ID_TYPE_OGG)
    {
        u4StrmId = pStrmID->u.t_stm_ogg_id;
    }
    else
    {
        LOG(3, "unknown e_type %d", pStrmID->e_type);
        //fgRet = FALSE;
    }

    if(fgRet)
    {
        DMX_MM_DEC_INFO_T *prDecInfo = NULL;
    
        if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
        {
            prDecInfo = &_rDecInfo;
        }
        else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
        {
            prDecInfo = &_rDecInfo1;
        }
        else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
        {
            prDecInfo = &_rDecInfo2;
        }
        // 0831
        // here may be needed another entry point
        else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
        {
            prDecInfo = &_rDecInfo3;
        }
        else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
        {
            prDecInfo = &_rDecInfo4;
        }        
        else
        {
            LOG(3, "no available u2CompId");
            //fgRet = FALSE;
        }

        if (prDecInfo)
        {
            UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            
            prDecInfo->u4StrmId = u4StrmId;
            prDecInfo->u4SubStrmId = u4SubStrmId;

            if (IS_AUD_CONNECTED_COMP(prDecInfo))
            {
                MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                  prDecInfo->u4StrmId, prDecInfo->u4SubStrmId, 
                                  (ENUM_MTSWDMX_STRM_TYPE_T)prDecInfo->u2DecType);
            }

            Printf("%s StrmType(%u) StrmId(%u) StrmIdType(%d) SubStrmId(%u)\n", __FUNCTION__,
                   prDecInfo->u2DecType, prDecInfo->u4StrmId,pStrmID->e_type, prDecInfo->u4SubStrmId);
        }
    }

#ifdef LINUX_TURNKEY_SOLUTION
    if(pStrmID != NULL)
    {
        _MwDmxFree(pStrmID);
    }
#endif

    return ((fgRet == TRUE) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}


static INT32 _MwDmxOnMMSetMediaFormat(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    BOOL fgRet;
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    MEDIA_FORMAT_T* prMediaFmt = (MEDIA_FORMAT_T*)pv_set_info;
    //SWDMX_STREAM_OPEN_INFO_T  rOpenInfo;
    MTSWDMX_NFY_INFO_T rDmxNfyInfo;

    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType = prMediaFmt->e_media_type;
    _arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType = prMediaFmt->t_media_subtype;
    _arDmxMMInfo[u1DmxMMInfoIdx].rSvcType = prMediaFmt->e_mm_svc_type;
    _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed = MM_SPEED_TYPE_FORWARD_1X;
    _arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType=MTSWDMX_PLAY_MODE_NORMAL;

    LOG(5, "%s  u1DmxMMInfoIdx = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx);

    x_memset(&rDmxNfyInfo, 0, sizeof(rDmxNfyInfo));
    rDmxNfyInfo.pfSwdmxNfy = _MwDmxNtyFct;
    rDmxNfyInfo.pvTag = 0;
    _MwDmxOnSetNfyFct(u1DmxMMInfoIdx, &rDmxNfyInfo, sizeof(rDmxNfyInfo));

    fgRet = FALSE;
    if (prMediaFmt->e_media_type == MEDIA_TYPE_CONTAINER)
    {
        //_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType = prMediaFmt->t_media_subtype.u.e_contnr_subtype;

        switch(prMediaFmt->t_media_subtype.u.e_contnr_subtype)
        {
            case MEDIA_CONTNR_SUBTYPE_AVI:
              fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_AVI);
              break;
            case MEDIA_CONTNR_SUBTYPE_ASF:
              /*fgRet = SWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, SWDMX_FMT_VC1_WMV);
               rOpenInfo.fgStreamInfo = TRUE;
               rOpenInfo.fgVideo = TRUE;
               rOpenInfo.fgAudio = TRUE;
               rOpenInfo.u2VideoId = 2;
               rOpenInfo.u2AudioId =1;
               rOpenInfo.eVideo = ENUM_SWDMX_VDEC_WMV9;
               rOpenInfo.eAudio;

              _SWDMX_Open(SWDMX_FMT_VC1_WMV, &rOpenInfo);*/
              fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_VC1_WMV);
              break;
            case MEDIA_CONTNR_SUBTYPE_MP4:
              if (prMediaFmt->e_mm_svc_type == MEDIA_SVC_TYPE_AUDIO_ONLY)
              {
                  fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_M4A);
              }
              else
              {
                  fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MP4);
              }
              break;
            case MEDIA_CONTNR_SUBTYPE_MATROSKA:
              fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MKV);
              break;
            case MEDIA_CONTNR_SUBTYPE_RM:
                fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_RM);
                break;
            case MEDIA_CONTNR_SUBTYPE_FLV:
                fgRet= MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_FLV);
                break;
            case MEDIA_CONTNR_SUBTYPE_CMPB:
                fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_CMPB);
                break;
            case MEDIA_CONTNR_SUBTYPE_OGM:
                fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_OGM);
                break;
            default:
              return RMR_DRV_INV_SET_INFO;
        }
    }
    else if (prMediaFmt->e_media_type == MEDIA_TYPE_PS)
    {
        if(prMediaFmt->t_media_subtype.u.e_ps_subtype
            == MEDIA_PS_SUBTYPE_DAT)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG1_DAT);
        }
        else
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_PS);
        }
    }
    else if (prMediaFmt->e_media_type == MEDIA_TYPE_TS)
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType = prMediaFmt->t_media_subtype.u.e_ts_subtype;

        Printf("ts sub type %d\n",prMediaFmt->t_media_subtype.u.e_ts_subtype);
        
        if(prMediaFmt->t_media_subtype.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_188)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS);
        }
        else if((prMediaFmt->t_media_subtype.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192) ||
                (prMediaFmt->t_media_subtype.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192_SYSTEM_B))
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_192);

            if(prMediaFmt->t_media_subtype.u.e_ts_subtype ==
                MEDIA_TS_SUBTYPE_TS_192_SYSTEM_B)
            {
                if(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                    (UINT8)MTSWDMX_SET_SYSTEM_B,
                    (UINT32)TRUE, 0, 0) !=MTR_OK)
                {
                    LOG(3,"_MwDmxOnMMSetMediaFormat eSWDMX_SET_SYSTEM_B fail\n");
                }
            }
        }
        else if(prMediaFmt->t_media_subtype.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192_ZERO)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_ZERO_192);
        }
        else if (prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_130)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_130);
        }
        else if (prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_134)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_134);
        }
        else if (prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_204)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_204);
        }        
        else if (prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_206)
        {
			MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,MTSWDMX_SET_TYPE_PCP_LEN,prMediaFmt->ui4_ts_byte_length,0,0);        
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_206);			
			Printf("DLNA JP_T Fmt(206) PCP header len 0x%x\n",prMediaFmt->ui4_ts_byte_length);
			 
        }		
        
        else if(prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_TIME_SHIFT);
        }
        else if(prMediaFmt->t_media_subtype.u.e_ts_subtype == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)
        {
            #ifndef CC_TIME_SHIFT_NO_TICK_DETAIL
            LOG(0, "Please define CC_TIME_SHIFT_NO_TICK_DETAIL\n");
            LOG(0, "when using MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB\n");
            ASSERT(0);
            #endif
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_TIME_SHIFT);
        }
        else if(prMediaFmt->t_media_subtype.u.e_ts_subtype
            == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_TS_ENCRYPT_192);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_TS unknown\n");
        }

        if(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_PID_INDEX_START,
            (UINT32)DMX_PES_COMP_ID_START, 0, 0) !=MTR_OK)
        {
            LOG(3,"_MwDmxOnMMSetMediaFormat MTSWDMX_PID_INDEX_START fail\n");
        }
    }
    else if (prMediaFmt->e_media_type == MEDIA_TYPE_AUDIO_ES)
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType = prMediaFmt->t_media_subtype.u.e_aud_subtype;
        if(prMediaFmt->t_media_subtype.u.e_aud_subtype == MEDIA_AUD_SUBTYPE_WMA)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_WMA_AUDIO_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_aud_subtype == MEDIA_AUD_SUBTYPE_OGG)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_OGG_AUDIO_ES);
        }
        else if(IS_RAW_AUDIOES_SUBTYPE(prMediaFmt->t_media_subtype.u.e_aud_subtype))
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_RAW_AUDIO_ES);
        }
        else if (prMediaFmt->t_media_subtype.u.e_aud_subtype == MEDIA_AUD_SUBTYPE_APE)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_APE_AUDIO_ES);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_AUDIO_ES unknown\n");
        }
    }
    else if (prMediaFmt->e_media_type == MEDIA_TYPE_VIDEO_ES)
    {
        if((prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M1V) ||
            (prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M2V))
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG2_VIDEO_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_H264)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_H264_VIDEO_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_M4V)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_MPEG4_VIDEO_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_VC1)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_VC1_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_AVS)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_AVS_VIDEO_ES);
        }
        else if(prMediaFmt->t_media_subtype.u.e_vid_subtype
            == MEDIA_VID_SUBTYPE_H265)
        {
            fgRet = MTSWDMX_SetContainerType(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_FMT_H265_VIDEO_ES);
        }
        else
        {
            LOG(5,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_VIDEO_ES unknown\n");
            //fgRet = FALSE;
        }
    }
    else
    {
        return RMR_DRV_INV_SET_INFO;
    }

    // switch to small buffer for audio only file or audio only mode
    if ((prMediaFmt->e_mm_svc_type == MEDIA_SVC_TYPE_AUDIO_ONLY) ||
        (prMediaFmt->e_mm_svc_type == MEDIA_SVC_TYPE_UNKNOWN && prMediaFmt->e_media_type == MEDIA_TYPE_AUDIO_ES))
    {
        if (MTFeederSetMMP(FEEDER_PROGRAM_SOURCE) != (INT32)FEEDER_E_OK)
        {
            LOG(0,"_MwDmxOnMMSetMediaFormat MEDIA_TYPE_AUDIO_ES set MMP buffer failed\n");
        }
    }

    // Set Container Type to SWDMX
    return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}


// Convert the speed setting from the upper layer
static INT32 _Speed_convert(MM_SPEED_TYPE_T e_mm_spd)
{
    switch (e_mm_spd)
    {
        case MM_SPEED_TYPE_STEP:          /* Forward 1X */
            return 1;
        case MM_SPEED_TYPE_FORWARD_1X:          /* Forward 1X */
            return 1000;
        case MM_SPEED_TYPE_FORWARD_2X:          /* Fast Forward 2X */
            return 2000;
        case MM_SPEED_TYPE_FORWARD_3X:          /* Fast Forward 3X */
            return 3000;
        case MM_SPEED_TYPE_FORWARD_4X:          /* Fast Forward 4X */
            return 4000;
        case MM_SPEED_TYPE_FORWARD_8X:          /* Fast Forward 8X */
            return 8000;
        case MM_SPEED_TYPE_FORWARD_16X:         /* Fast Forward 16X */
            return 16000;
        case MM_SPEED_TYPE_FORWARD_32X:        /* Fast Forward 32X */
            return 32000;
        case MM_SPEED_TYPE_FORWARD_64X:        /* Fast Forward 64X */
            return 64000;
        case MM_SPEED_TYPE_FORWARD_128X:        /* Fast Forward 128X */
            return 128000;
        case MM_SPEED_TYPE_FORWARD_256X:        /* Fast Forward 256 */
            return 256000;
        case MM_SPEED_TYPE_FORWARD_512X:        /* Fast Forward 512X */
            return 512000;
        case MM_SPEED_TYPE_FORWARD_1024X:        /* Fast Forward 1024X */
            return 1024000;
        case MM_SPEED_TYPE_REWIND_1X:          /* Rewind 1X */
            return (-1000);
        case MM_SPEED_TYPE_REWIND_2X:          /* Fast Rewind 2X */
            return (-2000);
        case MM_SPEED_TYPE_REWIND_3X:          /* Fast Rewind 3X */
            return (-3000);
        case MM_SPEED_TYPE_REWIND_4X:          /* Fast Rewind 4X */
            return (-4000);
        case MM_SPEED_TYPE_REWIND_8X:          /* Fast Rewind 8X */
            return (-8000);
        case MM_SPEED_TYPE_REWIND_16X:         /* Fast Rewind 16X */
            return (-16000);
        case MM_SPEED_TYPE_REWIND_32X:        /* Fast Rewind 32X */
            return (-32000);
        case MM_SPEED_TYPE_REWIND_64X:        /* Fast Rewind 64X */
            return -64000;
        case MM_SPEED_TYPE_REWIND_128X:        /* Fast Rewind 128X */
            return -128000;
        case MM_SPEED_TYPE_REWIND_256X:        /* Fast Rewind 256 */
            return -256000;
        case MM_SPEED_TYPE_REWIND_512X:        /* Fast Rewind 512X */
            return -512000;
        case MM_SPEED_TYPE_REWIND_1024X:        /* Fast Rewind 1024X */
            return -1024000;
        case MM_SPEED_TYPE_FORWARD_1_DIV_2X:     /* Slow Forward 1/2X */
            return (1000/2);
        case MM_SPEED_TYPE_FORWARD_1_DIV_3X:     /* Slow Forward 1/3X */
            return (1000/3);
        case MM_SPEED_TYPE_FORWARD_1_DIV_4X:     /* Slow Forward 1/4X */
            return (1000/4);
        case MM_SPEED_TYPE_FORWARD_1_DIV_8X:     /* Slow Forward 1/8X */
            return (1000/8);
        case MM_SPEED_TYPE_FORWARD_1_DIV_16X:    /* Slow Forward 1/16X */
            return (1000/16);
        case MM_SPEED_TYPE_FORWARD_1_DIV_32X:    /* Slow Forward1/32X */
            return (1000/32);
        case MM_SPEED_TYPE_REWIND_1_DIV_2X:      /* Slow Rewind 1/2X */
            return -(1000/2);
        case MM_SPEED_TYPE_REWIND_1_DIV_4X:      /* Slow Rewind 1/4X */
            return -(1000/4);
        case MM_SPEED_TYPE_REWIND_1_DIV_8X:      /* Slow Rewind 1/8X */
            return -(1000/8);
        case MM_SPEED_TYPE_REWIND_1_DIV_16X:      /* Slow Rewind 1/16X */
            return -(1000/16);
        case MM_SPEED_TYPE_REWIND_1_DIV_32X:      /* Slow Rewind 1/32X */
            return -(1000/32);
        default:
            return 1000;
    }
}


static INT32 _MwDmxOnMMSetPriority(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    //MM_PRIORITY_TYPE_T ePriority;

#if 0
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    _arDmxMMInfo[u1DmxMMInfoIdx].ePriority = *((MM_PRIORITY_TYPE_T*)pv_set_info);

    if (_arDmxMMInfo[u1DmxMMInfoIdx].ePriority == MM_PRIORITY_DEFAULT)
    {
        if (!SWDMX_LockValidSrc(SWDMX_PRIMARY_SOURCE))
        {
            return RMR_DRV_INV_SET_INFO;
        }

        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId = SWDMX_PRIMARY_SOURCE;

        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId, FEEDER_PROGRAM_SOURCE, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, eSWDMX_SET_SRC_TYPE, FEEDER_PROGRAM_SOURCE, 0, 0);
        LOG(5, "Set buffer agent to program source\n");
    }
    else if (_arDmxMMInfo[u1DmxMMInfoIdx].ePriority == MM_PRIORITY_HIGH)
    {
        if (!SWDMX_LockValidSrc(SWDMX_PRIMARY_SOURCE))
        {
            return RMR_DRV_INV_SET_INFO;
        }

        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId = SWDMX_PRIMARY_SOURCE;

        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId, FEEDER_AUDIO_SOURCE, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, eSWDMX_SET_SRC_TYPE, FEEDER_AUDIO_SOURCE, 0, 0);
        LOG(5, "Set buffer agent to audio source\n");
    }
#ifdef CC_FBM_TWO_FBP
    else
    {
        if (!SWDMX_LockValidSrc(SWDMX_SECONDARY_SOURCE))
        {
            return RMR_DRV_INV_SET_INFO;
        }

        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId = SWDMX_SECONDARY_SOURCE;

        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId, FEEDER_PROGRAM_SOURCE, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        SWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, eSWDMX_SET_SRC_TYPE, FEEDER_PROGRAM_SOURCE, 0, 0);
        LOG(5, "Set buffer agent to program source\n");
    }
#endif
#endif
    return RMR_OK;
}


static INT32 _MwDmxOnMMSetABPosition(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    MM_AB_POS_T* prPos = (MM_AB_POS_T*)pv_set_info;
    UINT8 u1DmxMMInfoIdx;
    MTB2R_AB_INFO_T rVdpPosInfo;
    MTVDEC_AB_INFO_T rVdecPosInfo;

    if((!pt_comp_id) || (!prPos))
    {
        LOG(1,"_MwDmxOnMMSetABPosition pv_set_info pt_comp_id fail\n");
        return RMR_DRV_INV_SET_INFO;
    }

    u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    x_memset(&rVdpPosInfo, 0, sizeof(MTB2R_AB_INFO_T));
    x_memset(&rVdecPosInfo, 0, sizeof(MTVDEC_AB_INFO_T));
    if(prPos->b_valid)
    {
        rVdpPosInfo.fgValid = TRUE;
        rVdecPosInfo.fgValid = TRUE;
        if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES) ||
            ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_AUDIO_ES) &&
             IS_RAW_AUDIOES_SUBTYPE(_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType))
            )
        {
            rVdpPosInfo.fgPosition = TRUE;
            rVdecPosInfo.fgPosition = TRUE;
        }
        rVdpPosInfo.rA.u8AudPts            = prPos->t_pos_a.ui8_aud_pts_info;
        rVdpPosInfo.rA.u8AudOffset         = prPos->t_pos_a.ui8_aud_frame_position;
        rVdpPosInfo.rA.u8PtsI              = prPos->t_pos_a.ui8_i_pts_info;
        rVdpPosInfo.rA.u8Pts               = prPos->t_pos_a.ui8_pts_info;
        rVdpPosInfo.rA.u8Stc               = prPos->t_pos_a.ui8_stc;
        rVdpPosInfo.rA.u8OffsetI           = prPos->t_pos_a.ui8_i_frame_position;
        rVdpPosInfo.rA.u8Offset            = prPos->t_pos_a.ui8_frame_position;
        rVdpPosInfo.rA.u8OffsetDisp        = prPos->t_pos_a.ui8_frame_position_disp;
        rVdpPosInfo.rA.i4TemporalRef       = prPos->t_pos_a.i4_temporal_reference;
        rVdpPosInfo.rA.u2DecodingOrder     = prPos->t_pos_a.ui2_decoding_order;

        rVdecPosInfo.rA.u8AudPts            = prPos->t_pos_a.ui8_aud_pts_info;
        rVdecPosInfo.rA.u8AudOffset         = prPos->t_pos_a.ui8_aud_frame_position;
        rVdecPosInfo.rA.u8PtsI              = prPos->t_pos_a.ui8_i_pts_info;
        rVdecPosInfo.rA.u8Pts               = prPos->t_pos_a.ui8_pts_info;
        rVdecPosInfo.rA.u8OffsetI           = prPos->t_pos_a.ui8_i_frame_position;
        rVdecPosInfo.rA.u8Offset            = prPos->t_pos_a.ui8_frame_position;
        rVdecPosInfo.rA.u8OffsetDisp        = prPos->t_pos_a.ui8_frame_position_disp;
        rVdecPosInfo.rA.i4TemporalRef       = prPos->t_pos_a.i4_temporal_reference;
        rVdecPosInfo.rA.u2DecodingOrder     = prPos->t_pos_a.ui2_decoding_order;

        rVdpPosInfo.rB.u8AudPts            = prPos->t_pos_b.ui8_aud_pts_info;
        rVdpPosInfo.rB.u8AudOffset         = prPos->t_pos_b.ui8_aud_frame_position;
        rVdpPosInfo.rB.u8PtsI              = prPos->t_pos_b.ui8_i_pts_info;
        rVdpPosInfo.rB.u8Pts               = prPos->t_pos_b.ui8_pts_info;
        rVdpPosInfo.rB.u8Stc               = prPos->t_pos_b.ui8_stc;
        rVdpPosInfo.rB.u8OffsetI           = prPos->t_pos_b.ui8_i_frame_position;
        rVdpPosInfo.rB.u8Offset            = prPos->t_pos_b.ui8_frame_position;
        rVdpPosInfo.rB.u8OffsetDisp        = prPos->t_pos_b.ui8_frame_position_disp;
        rVdpPosInfo.rB.i4TemporalRef       = prPos->t_pos_b.i4_temporal_reference;
        rVdpPosInfo.rB.u2DecodingOrder     = prPos->t_pos_b.ui2_decoding_order;

        rVdecPosInfo.rB.u8AudPts            = prPos->t_pos_b.ui8_aud_pts_info;
        rVdecPosInfo.rB.u8AudOffset         = prPos->t_pos_b.ui8_aud_frame_position;
        rVdecPosInfo.rB.u8PtsI              = prPos->t_pos_b.ui8_i_pts_info;
        rVdecPosInfo.rB.u8Pts               = prPos->t_pos_b.ui8_pts_info;
        rVdecPosInfo.rB.u8OffsetI           = prPos->t_pos_b.ui8_i_frame_position;
        rVdecPosInfo.rB.u8Offset            = prPos->t_pos_b.ui8_frame_position;
        rVdecPosInfo.rB.u8OffsetDisp        = prPos->t_pos_b.ui8_frame_position_disp;
        rVdecPosInfo.rB.i4TemporalRef       = prPos->t_pos_b.i4_temporal_reference;
        rVdecPosInfo.rB.u2DecodingOrder     = prPos->t_pos_b.ui2_decoding_order;
    }

    MTB2R_SetABInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &rVdpPosInfo);
    MTVDEC_SetABInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &rVdecPosInfo);

    return RMR_OK;
}

#if 0
static INT32 _MwDmxOnMMSetParseHdrCmd(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{    
    MM_PARSE_HDR_CMD parseHdrCmd = (MM_PARSE_HDR_CMD)pv_set_info;
    LOG(1, "function = %s, line = %d\n", __FUNCTION__, __LINE__);
    if(!pt_comp_id)
    {
        LOG(1,"_MwDmxOnMMSetParseHdrCmd pt_comp_id fail\n");
        return RMR_DRV_INV_SET_INFO;
    }
    
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    LOG(1, "u1SrcId = %d\n", _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_SET_AUDIOTYPE,
            (UINT32)ENUM_MTSWDMX_ADEC_PCM, 0, 0);
    return RMR_OK;
}
#endif

static INT32 _MwDmxOnMMPlayOneFrm(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
	BOOL fgRet;
	UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
	fgRet = MTSWDMX_PlayOneFrm(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
	return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}
static INT32 _MwDmxOnMMSetSpeed(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    MM_PLAY_MODE_PL_T *prPlayMode=NULL;
    BOOL fgRet;
    BOOL fgAudPlay = FALSE;
    UINT32 u4Speed;
    MTSWDMX_PLAY_MODE_T ePlayMode;
    BOOL fgResetSwDmx = TRUE;
    #ifdef LINUX_TURNKEY_SOLUTION
    MM_PLAY_MODE_PL_T rPlayMode;
    #endif
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

#ifdef LINUX_TURNKEY_SOLUTION
    rm_copy_from_user((VOID*)&rPlayMode, (VOID *)pv_set_info, sizeof(MM_PLAY_MODE_PL_T));
    prPlayMode=&rPlayMode;
#else
    prPlayMode=(MM_PLAY_MODE_PL_T*)pv_set_info;
#endif
    u4Speed = (UINT32)prPlayMode->eSpeed;

    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    if(prPlayMode->ePlayMode==MM_PLAY_MODE_TYPE_NORMAL)
    {
       ePlayMode=MTSWDMX_PLAY_MODE_NORMAL;
    }
    else if(prPlayMode->ePlayMode==MM_PLAY_MODE_TYPE_IFRAME)
    {
       ePlayMode=MTSWDMX_PLAY_MODE_IFRAME;
    }
    else
    {
        return RMR_DRV_INV_SET_INFO;
    }
    if(u4Speed == MM_SPEED_TYPE_STEP || u4Speed == MM_SPEED_TYPE_STEP_REVERSE)
    {
        if(u4Speed == MM_SPEED_TYPE_STEP_REVERSE)
        {
            return RMR_DRV_INV_SET_INFO;
        }

        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER
        &&(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_CMPB))
        {
           if(!_arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist)
           {
              return RMR_DRV_INV_SET_INFO;
           }
        }

        _MwDmxFlushCtrl(u1DmxMMInfoIdx);
        fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed SWDMX_Pause fail\n");
        }
        _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
        fgRet=MTSWDMX_SetPlayMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,ePlayMode);
        if(!fgRet)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_SetPlayMode fail\n");
        }
        fgRet = MTSWDMX_SetSpeed(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, _Speed_convert((MM_SPEED_TYPE_T)u4Speed));
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed SWDMX_SetSpeed fail\n");
        }
        fgRet = MTSWDMX_Play(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed SWDMX_Play fail\n");
        }

        MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
    }
    else if(u4Speed == MM_SPEED_TYPE_PAUSE)
    {
        LOG(5,"_MwDmxOnMMSetSpeed MM_SPEED_TYPE_PAUSE\n");
        _MwDmxFlushCtrl(u1DmxMMInfoIdx);
        fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed SWDMX_Pause fail\n");
        }
        _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);

        if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed !=MM_SPEED_TYPE_FORWARD_1X)
        {
            LOG(5,"_MwDmxOnMMSetSpeed just pause swdmx\n");
            if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES)
            {
                MTSTC_GetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, &(_arDmxMMInfo[u1DmxMMInfoIdx].u4StcValue));
                _arDmxMMInfo[u1DmxMMInfoIdx].fgResetStcVal=TRUE;
            }
            //fake pause need do flush actually
        }
        else
        {
           _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause = TRUE;
           MTSTC_GetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, &(_arDmxMMInfo[u1DmxMMInfoIdx].u4StcValue));
           //for mkv subtitle, subtitle should display at pause, so subtitle engine can't be stop
           //in this case, stc should be stop to avoid subtitle change
           if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_1X)
           {
#if 0 // for vudu pause/play issue
             MTSTC_StopStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
#endif
           }
        }
    }
    else if(u4Speed == MM_SPEED_TYPE_FORWARD_1X)
    {
        Printf("_MwDmxOnMMSetSpeed MM_SPEED_TYPE_FORWARD_1X\n");
        _MwDmxFlushCtrl(u1DmxMMInfoIdx);
        fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed SWDMX_Pause fail\n");
        }
        _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
        while(1)
        {
            if(_arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport)
            {
                break;
            }
            else if((_arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone) &&
               ((_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl == AUD_DEC_CTRL_PLAY) ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl == AUD_DEC_CTRL_PLAY_SYNC)||
                ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType  == MEDIA_TYPE_AUDIO_ES)&&
                 (_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl == AUD_DEC_CTRL_RESUME) &&
                (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed !=MM_SPEED_TYPE_PAUSE)))) //for change audio trick to normal mode. play audio
            {
                fgAudPlay = TRUE;
                break;
            }
            else if((_arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone) &&
               ((_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl != AUD_DEC_CTRL_PLAY) &&
                (_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl != AUD_DEC_CTRL_PLAY_SYNC)))
            {
                break;
            }
            else if((_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl==AUD_DEC_CTRL_RESUME) &&
                (_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType == MEDIA_AUD_SUBTYPE_WMA))
            {
                UNUSED(MTAUDDEC_AUD_IsDecoderRealPlay(_arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId, &fgAudPlay));

                if (fgAudPlay)
                {
                    break;
                }
            }
            else if(_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl==AUD_DEC_CTRL_RESUME)
            {
                break;
            }
            LOG(5,"_MwDmxOnMMSetSpeed 1X wait aud ctrl done\n");
            x_thread_delay(10);
        }
        //_MwDmxMMLockFlushMutex();


        if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER)&&
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype!=MEDIA_CONTNR_SUBTYPE_CMPB
             &&_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype!=MEDIA_CONTNR_SUBTYPE_UNKNOWN))
        {
            if(_arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType==MTSWDMX_PLAY_MODE_IFRAME || _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed>MM_SPEED_TYPE_FORWARD_2X)
            {
                UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
            }
        }
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES) ||
             (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)) //for video ES/TS/PS when <pause-2X>  to 1x  no need to do swdmx flush
        {
           if (_arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType==MTSWDMX_PLAY_MODE_IFRAME)
           {
                UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
           }
        }
#if 0
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)&&
                 ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype == MEDIA_VID_SUBTYPE_H264)||
                 (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_vid_subtype ==MEDIA_VID_SUBTYPE_M1V))) //for H264  TS file no needed do flush when <pause-2X>  to 1x
        {
           if(_arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType==MTSWDMX_PLAY_MODE_IFRAME)
           {
                UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
           }
        }
#endif
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_AUDIO_ES) &&
            (IS_RAW_AUDIOES_SUBTYPE(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype == MEDIA_AUD_SUBTYPE_OGG) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_aud_subtype == MEDIA_AUD_SUBTYPE_APE)))
        {
           if (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed < MM_SPEED_TYPE_PAUSE)
           {
                UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
           }
        }
        else
        {
            if (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed != MM_SPEED_TYPE_PAUSE ||
                (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_PAUSE  &&
                _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause == FALSE))
            {
                //do flush when >> -- |> or >> --|| --|>
                UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
            }
        }

        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgFreeRun)
        {
            MTSTC_StopStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
            if(_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist)
            {
                MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                    MTAV_SYNC_FREE_RUN, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);

                //VDP_SetB2rUpdateStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId,FALSE);
            }
            // TODO: set the stc id to audio driver
            MTAUDDEC_SetAvSynMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId, MTAV_SYNC_FREE_RUN);
        }
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
           ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
        {
            MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_SYSTEM_MASTER);
        }
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                (_arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb))     //DTV00080949, thumb nail mode should set av_sync_none, if not vdp will
        {                                                       // can't send eos to mw
            MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
        }
        else if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES)
        {
            MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_VIDEO_MASTER);
            MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
        }

        else
        {
            // always audio master
            if (_arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType == MM_SRC_TYPE_NETWORK_SKYPE)
            {
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                LOG(5, "Skype mode set AV sync to free run\n");
            }
            else if (_arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType == MM_SRC_TYPE_WIFI_DISPLAY)
            {
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_SYSTEM_MASTER);
                LOG(5, "WFD mode set AV sync to system master.2.\n");
            }
            else if ((_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist) &&
                     ((!_arDmxMMInfo[u1DmxMMInfoIdx].fgAudIsConn) || _arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport))
            {
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                LOG(5, "No audio, Set AV sync mode to none mode\n");
            }
            else
            {
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
                LOG(5, "Set AV sync mode to audio master\n");
            }
            /*
            if (_fgVideoExist)
            {
                MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
            }
            else
            {
                MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, MTSTC_AV_SYNC_MODE_NONE);
            }
            */
        }

        //_MwDmxUnlockFlushMutex();
        fgRet=MTSWDMX_SetPlayMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,ePlayMode);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_SetPlayMode fail\n");
        }

        fgRet = MTSWDMX_SetSpeed(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 1000);

        if(fgAudPlay)
        {
#ifdef CC_MT5363
            MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_AUX);
            MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_MAIN);
#else
            // 0831 trick to normal replay audio 
            if(_arDmxMMInfo[u1DmxMMInfoIdx].u1AudCtrlId == MT_AUD_DEC_AUX)
            {
                MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_AUX);
            }
            else if(_arDmxMMInfo[u1DmxMMInfoIdx].u1AudCtrlId == MT_AUD_DEC_THIRD || 
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription)
            {
                MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_AUX);
                MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_THIRD);
            }
            
            // main must be played
            MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_MAIN);           
#endif
            Printf("_MwDmxOnMMSetSpeed 1X AUD_DSPCmdPlay\n");
        }
        MTSWDMX_Play(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgResetStcVal)
        {
            MTSTC_SetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, _arDmxMMInfo[u1DmxMMInfoIdx].u4StcValue);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgResetStcVal=FALSE;
        }
        if (_arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause &&
            !_arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsSeek &&
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed != MM_SPEED_TYPE_STEP))
        {
            //LOG(0,"_MwDmxOnMMSetSpeed SetStcValue %d\n",_ui4StcValue);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause = FALSE;
            //for mkv subtitle, subtitle should display at pause, so subtitle engine can't be stop
            //in this case, stc should be stop to avoid subtitle change
            //restart stc if pause->1x, will be start also if pause-> seek at VDP
            
			//for pvr & timeshift, use old pause->play			
			if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
				   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||					 
					(_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION)))
            {
                Printf("_MwDmxOnMMSetSpeed set aout enable\n");
                MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);                
			}

        }

        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsTrick)
        {
            #if 0
            UINT32 u4CurPts = 0;
            UINT64 u8FilePos = 0;

            MTSTC_StopStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
            VDP_SetPauseMm(0, TRUE);
            u4CurPts = MTB2R_GetPts(0);

            SWDMX_SetSpeed(1000);
            SWDMX_SeekTime(u4CurPts, &u8FilePos);

            VDP_SetPauseMm(0, FALSE);
            VDP_SetSpeed(0, MTSTC_SPEED_TYPE_FORWARD_1X);

            MTSTC_SetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, u4CurPts);
            MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
            #endif
        }
        _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsSeek = FALSE;
        _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsTrick = FALSE;
    }
    else
    {
        if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER)&&
            (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype!=MEDIA_CONTNR_SUBTYPE_CMPB
             &&_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype!=MEDIA_CONTNR_SUBTYPE_UNKNOWN))
        {
            if(!_arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist)
            {
                return RMR_DRV_INV_SET_INFO;
            }
        }
        _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsTrick = TRUE;

        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause ||
           _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_STEP)
        {
            MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause = FALSE;
        }

        _MwDmxFlushCtrl(u1DmxMMInfoIdx);
        fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_Pause fail\n");
        }
        _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
        /*
        while(1)
        {
            if((_fgAudCtrlDone) &&
               ((_u4AudCtrl != AUD_DEC_CTRL_PLAY) &&
                (_u4AudCtrl != AUD_DEC_CTRL_PLAY_SYNC)))
            {
                break;
            }
            LOG(5,"_MwDmxOnMMSetSpeed !1X wait aud ctrl done\n");
            x_thread_delay(10);
        }
        */
        //_MwDmxMMLockFlushMutex();
        #if 1
        if (_arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType==MTSWDMX_PLAY_MODE_NORMAL
            && ePlayMode==MTSWDMX_PLAY_MODE_NORMAL && u4Speed!=MM_SPEED_TYPE_PAUSE)
        {
            /* Slow Forward, 1x, 2x -> Slow Forward, 1x, 2x */
            fgResetSwDmx = FALSE;
        }
        #else
        if (((_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_1X) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_PAUSE) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_STEP)) &&
            (u4Speed == MM_SPEED_TYPE_FORWARD_2X))
        {
            /* 1x -> 2x */
            fgResetSwDmx = FALSE;
        }
        else if (((_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_1X) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_PAUSE) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_STEP)) &&
            ((u4Speed >= MM_SPEED_TYPE_FORWARD_1_DIV_32X) && (u4Speed <= MM_SPEED_TYPE_FORWARD_1_DIV_2X)))
        {
            /* 1x -> Slow Forward */
            fgResetSwDmx = FALSE;
        }
        else if (((_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed >= MM_SPEED_TYPE_FORWARD_1_DIV_32X) && (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed <= MM_SPEED_TYPE_FORWARD_1_DIV_2X)) &&
            ((u4Speed >= MM_SPEED_TYPE_FORWARD_1_DIV_32X) && (u4Speed <= MM_SPEED_TYPE_FORWARD_1_DIV_2X)))
        {
            /* Slow Forward -> Slow Forward */
            fgResetSwDmx = FALSE;
        }
        #endif
        if((fgResetSwDmx)&&(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType != MEDIA_TYPE_AUDIO_ES) &&
            !((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_OGM ) &&
             (_arDmxMMInfo[u1DmxMMInfoIdx].rSvcType == MEDIA_SVC_TYPE_AUDIO_ONLY)) &&
            !((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MP4 )&&
            (_arDmxMMInfo[u1DmxMMInfoIdx].rSvcType == MEDIA_SVC_TYPE_AUDIO_ONLY)))    //for audio trick mode, no need to flush audio FIFO  OGG Vorbis trick mode not to flush swdmx
        {
            /* 1x -> 2x or Slow motion not to SWDMX ?! */
            UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
        }
        /*else
        {
            UNUSED(MTSWDMX_Flush());
        }*/
        if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType  == MEDIA_TYPE_AUDIO_ES) ||
            ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_OGM )&&
            (_arDmxMMInfo[u1DmxMMInfoIdx].rSvcType == MEDIA_SVC_TYPE_AUDIO_ONLY)) ||
            ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MP4 )&&
            (_arDmxMMInfo[u1DmxMMInfoIdx].rSvcType == MEDIA_SVC_TYPE_AUDIO_ONLY)))       //for audio trick mode
        {
            if(_arDmxMMInfo[u1DmxMMInfoIdx].fgFreeRun)
            {
                // TODO: set the stc id to audio driver
                MTAUDDEC_SetAvSynMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId, MTAV_SYNC_FREE_RUN);
            }
            else
            {
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
            }

            MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);   //DTV00083152 for some specail mp3 file , when trick mode because decoder error,
                                                                    // audio need to wait STC reach PTS, so we should start stc in audio trick mode.
        }
        else if(((u4Speed == MM_SPEED_TYPE_FORWARD_2X)
#ifdef VDEC_TIME_SHIFT_2X_I_MODE
           &&
           ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType != MEDIA_TYPE_TS) ||
            ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType != MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) &&
             (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType != MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB))
            )
#endif
           )
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_2X)
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_3X)
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_4X)
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_8X)
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_16X)
           || (u4Speed == MM_SPEED_TYPE_FORWARD_1_DIV_32X))
        {
            MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_VIDEO_MASTER);
        }
        else
        {
#if 0  // megaa tmp rollback
            MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
            // if we set none, vdec will to underflow check
            // prVdecEsInfo->u2UdfNs = MPV_UDF_PES_NS
            // so use audio master
#else
            MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
#endif
        }


        //_MwDmxMMUnlockFlushMutex();
        fgRet=MTSWDMX_SetPlayMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,ePlayMode);
        if(!fgRet)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_SetPlayMode fail\n");
        }
        fgRet = MTSWDMX_SetSpeed(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, _Speed_convert((MM_SPEED_TYPE_T)u4Speed));
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_SetSpeed fail\n");
        }
        fgRet = MTSWDMX_Play(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        if(fgRet != MTR_OK)
        {
            LOG(1,"_MwDmxOnMMSetSpeed MTSWDMX_Play fail\n");
        }
    }
    _arDmxMMInfo[u1DmxMMInfoIdx].ePlayModeType=ePlayMode;
    _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed = u4Speed;
    return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}


static INT32 _MwDmxOnMMSetSeekPos(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    BOOL fgRet = FALSE;
    MM_RANGE_POS_INFO_T* prSeekInfo;
    MTSWDMX_RANGE_INFO_T rRangeInfo;
    UINT64 u8FilePos;
    UINT32 u4SeekTime = 0;
    BOOL fgAudPlay = FALSE;

    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    prSeekInfo = (MM_RANGE_POS_INFO_T*)pv_set_info;

    if(prSeekInfo == NULL)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    _MwDmxFlushCtrl(u1DmxMMInfoIdx);
    fgRet = MTSWDMX_Pause(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
    if(fgRet != MTR_OK)
    {
        LOG(1,"_MwDmxOnMMSetSeekPos MTSWDMX_Pause fail\n");
    }
    _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
    while(1)
    {
        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport)
        {
            break;
        }
        if((_arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone) &&
           ((_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl != AUD_DEC_CTRL_PLAY) &&
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl != AUD_DEC_CTRL_PLAY_SYNC)))
        {
            break;
        }
        else if((_arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone) &&
           ((_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl == AUD_DEC_CTRL_PLAY) ||
            (_arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl == AUD_DEC_CTRL_PLAY_SYNC)))
        {
            fgAudPlay = TRUE;
            break;
        }
        LOG(5,"_MwDmxOnMMSetSeekPos wait aud ctrl done\n");
        x_thread_delay(10);
    }
    //_MwDmxMMLockFlushMutex();
     //_MwDmxMMLockFlushMutex();
    if((prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_RANGE_AB_START))
     {
         UNUSED(MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId));
     }

    //_MwDmxMMUnlockFlushMutex();
    if(fgAudPlay)
    {
#ifdef CC_MT5363
        MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_AUX);
        MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_MAIN);
#else
        // 0831 trick to normal replay audio 
        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription)
        {
            MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_AUX);
            MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_THIRD);
        }
        MTAUDDEC_AUD_DSPCmdPlay(MT_AUD_DEC_MAIN);
#endif
        Printf("_MwDmxOnMMSetSeekPos AUD_DSPCmdPlay\n");
    }

    //seek
    x_memset(&rRangeInfo,0,sizeof(rRangeInfo));
    MTSWDMX_QueryRangeInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, &rRangeInfo);
    if(prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_TIME_RANGE_CURR)
    {
        u4SeekTime = (UINT32)rRangeInfo.u8CurRangePTS + (UINT32)prSeekInfo->ui8_pos_val;
        fgRet = MTSWDMX_SeekTime(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u4SeekTime, &u8FilePos);
    }
    else if(prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_TIME_RANGE_ID)
    {
        if(rRangeInfo.u4CurRangeID == prSeekInfo->ui4_idx)
        {
            u4SeekTime = (UINT32)rRangeInfo.u8CurRangePTS + (UINT32)prSeekInfo->ui8_pos_val;
        }
        else if(rRangeInfo.u4QRangeID == prSeekInfo->ui4_idx)
        {
            u4SeekTime = (UINT32)rRangeInfo.u8QRangePTS + (UINT32)prSeekInfo->ui8_pos_val;
        }
        else
        {
            return RMR_DRV_INV_SET_INFO;
        }
        fgRet = MTSWDMX_SeekTime(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u4SeekTime, &u8FilePos);
    }
    else if (prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_TIME_MEDIA)
    {
        u4SeekTime = (UINT32)prSeekInfo->ui8_pos_val;
        fgRet = MTSWDMX_SeekTime(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u4SeekTime, &u8FilePos);
    }
    else if (prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_OFFSET_RANGE_CURR)
    {
        u8FilePos = prSeekInfo->ui8_pos_val;
        fgRet = MTSWDMX_SeekPos(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u8FilePos, u8FilePos);
    }
    else if(prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_OFFSET_RANGE_ID)
    {
        if(rRangeInfo.u4CurRangeID == prSeekInfo->ui4_idx)
        {
            u8FilePos = prSeekInfo->ui8_pos_val;
        }
        else
        {
            return RMR_DRV_INV_SET_INFO;
        }
        fgRet = MTSWDMX_SeekPos(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u8FilePos, u8FilePos);
    }
    else if(prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_OFFSET_RANGE_START)
    {
        fgRet = MTSWDMX_RenderFromPos(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (VOID*)&prSeekInfo->t_start_pos);
    }
    else if(prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_RANGE_AB_START)
    {
        fgRet = MTSWDMX_SeekOffset(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (VOID*)&prSeekInfo->t_start_pos);
        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER)
        {
           if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype==MEDIA_CONTNR_SUBTYPE_FLV)
           {
              _arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist=TRUE;
           }
        }
    }
    else if (prSeekInfo->e_pos_type == MM_RANGE_POS_TYPE_TIME_BASED_RANGE)
    {
        u4SeekTime = (UINT32)prSeekInfo->ui8_pos_val;
        Printf("seek time %d\n",u4SeekTime);        
        fgRet = MTSWDMX_SeekTime(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u4SeekTime, &u8FilePos);        
    }
    
    if(!_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist) // audio only
    {
        //MW_DMX_NotifyStepDone(0, 0, 0, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MW_DMX_NotifyStepDone(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, 0, MTB2R_SEEK_STEP_OK);
    }
    else
    {
        if(prSeekInfo->e_auto_speed != MM_SPEED_TYPE_PAUSE)
        {
            // TODO, VDP_RegCbFunc is single instance function
            //DLNA Authentication fixed send seek done msg premature ts ps wmv
			if (((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
			     (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType != MEDIA_TS_SUBTYPE_TS_TIME_SHIFT)) ||
			     (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES)||
			     (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS) ||
			     ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER) &&
			      (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_ASF)) ||
			      ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER) &&
			      (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MP4)))
			{
			    // MTB2R_SetSeek put in container inner handle to fix timing issue
			}
			else
			{
                if (_arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType == MM_SRC_TYPE_NETWORK_DASH)
                {
                    MTB2R_SetSeek(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, 1, u4SeekTime);
                }
                else if (prSeekInfo->e_auto_speed > MM_SPEED_TYPE_PAUSE)
                {
                    MTB2R_SetSeek(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, 1, 0);
                }
                else
                {
                    MTB2R_SetSeek(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, 1, 0xFFFFFFFF);
                }						    
			}				
            
            MTVDO_RegCbFunc(VDO_CB_FUNC_MM_SEEK_FIN_IND, (UINT32)_MW_DMX_NotifySeekDone, 0, 0);
        }
    }

    _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsSeek = TRUE;
    return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);

}

#ifdef LINUX_TURNKEY_SOLUTION
typedef struct _RANGE_LIST_T
{
    MM_RANGE_INFO_T *pt_range;
    struct _RANGE_LIST_T * pt_next;
    struct _RANGE_LIST_T * pt_prev;
}   RANGE_LIST_T;

#ifdef CC_SKYPE_FINE_INSTANCE
static RANGE_LIST_T* g_pt_range_list[DMX_MM_MAX_INSTNUM];
#else
static RANGE_LIST_T* g_pt_range_list[DMX_MM_MAX_INSTNUM];
#endif
static VOID _MwDmx_add_range_list(UINT32 u4DmxIdx, MM_RANGE_INFO_T *pt_range)
{
    RANGE_LIST_T* pt_temp = NULL;

    if(NULL == pt_range)
    {
        return;
    }

    pt_temp = (RANGE_LIST_T*)_MwDmxMalloc(sizeof(RANGE_LIST_T));
    if(!pt_temp)
    {
        LOG(1, "_MwDmx_add_range_list _MwDmxMalloc fail\n");
        ASSERT(0);
        return;
    }
    x_memset(pt_temp,0,sizeof(RANGE_LIST_T));

    pt_temp->pt_range = pt_range;

    pt_temp->pt_next = g_pt_range_list[u4DmxIdx];

    if(g_pt_range_list[u4DmxIdx] != NULL)
    {
        g_pt_range_list[u4DmxIdx]->pt_prev = pt_temp;
    }

    g_pt_range_list[u4DmxIdx] = pt_temp;

    return;
}

static VOID _MwDmx_recursion_free_dec_info_enc_str(VOID *pvPtr)
{
    if (pvPtr == NULL)
    {
        return;
    }

    // For now condition (pvPtr != NULL) is only possible for mkv
    //if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
    {
        MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)pvPtr;
        _MwDmx_recursion_free_dec_info_enc_str(ptCntEncoding->pt_next);
        if (ptCntEncoding->pui1_comp_settings_data)
        {
            _MwDmxFree(ptCntEncoding->pui1_comp_settings_data);
        }
    }

    _MwDmxFree(pvPtr);
}


static VOID _MwDmx_recursion_free_dec_info(MM_RANGE_DECODER_T *ptDecoderInfo)
{
    if (ptDecoderInfo == NULL)
    {
        return;
    }

    _MwDmx_recursion_free_dec_info(ptDecoderInfo->pt_next);
    _MwDmx_recursion_free_dec_info_enc_str(ptDecoderInfo->pv_encoding_string);
    if (ptDecoderInfo->pui1_codec_private)
    {
        _MwDmxFree(ptDecoderInfo->pui1_codec_private);
    }
    _MwDmxFree(ptDecoderInfo);
}

static VOID _MwDmx_recursion_free_data(MM_RANGE_ELMT_IDXTBL_T *pt_data)
{
    if (NULL != pt_data)
    {
        /* Deal pv_next_tbl data. */
        _MwDmx_recursion_free_data(pt_data->pv_next_tbl);

        /* Deal pt_next_partial data. */
        _MwDmx_recursion_free_data(pt_data->pt_next_partial);

        /* Deal pt_pb_idx_tbl_entry data. */
        if(pt_data->u.pt_pb_idx_tbl_entry != NULL)
        {
            _MwDmxFree(pt_data->u.pt_pb_idx_tbl_entry);
        }

        _MwDmxFree(pt_data);
    }

    return;
}

static VOID _MwDmx_free_range_info(MM_RANGE_INFO_T* pt_range)
{
    if(NULL == pt_range)
    {
        return;
    }

#if SUPPORT_MULTI_INFO
    if (pt_range->pt_next_scale)
    {
        _MwDmx_free_range_info(pt_range->pt_next_scale);
    }
#endif

    _MwDmx_recursion_free_dec_info(pt_range->pt_aud_decoder_info);
    _MwDmx_recursion_free_dec_info(pt_range->pt_vid_decoder_info);

    if(pt_range->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info != NULL)
    {
        _MwDmxFree(pt_range->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info);
    }

    if(pt_range->pt_track_tm_scale_lst != NULL)
    {
        _MwDmxFree(pt_range->pt_track_tm_scale_lst);
    }

    _MwDmx_recursion_free_data(pt_range->pt_idxtbl_lst);

    _MwDmxFree(pt_range);

    return;
}

static VOID _MwDmx_free_range_node(RANGE_LIST_T* pt_node)
{
    if(NULL == pt_node)
    {
        return;
    }

    _MwDmx_free_range_info(pt_node->pt_range);

    _MwDmxFree(pt_node);

    return;
}

static VOID _MwDmx_remove_range_list(UINT32 u4DmxIdx, UINT32 ui4_range_id)
{
    RANGE_LIST_T* pt_temp = NULL;
    if(u4DmxIdx>=DMX_MM_MAX_INSTNUM)
    {
       return;
    }

    pt_temp=g_pt_range_list[u4DmxIdx];
    while(pt_temp != NULL)
    {
        if(pt_temp->pt_range->ui4_range_id == ui4_range_id)
        {
            if(pt_temp->pt_prev != NULL)
            {
                pt_temp->pt_prev->pt_next = pt_temp->pt_next;
            }
            else
            {
                g_pt_range_list[u4DmxIdx] = pt_temp->pt_next;
            }

            if(pt_temp->pt_next != NULL)
            {
                pt_temp->pt_next->pt_prev = pt_temp->pt_prev;
            }

            _MwDmx_free_range_node(pt_temp);

            break;
        }

        pt_temp = pt_temp->pt_next;
    }

    return;
}

static BOOL _MwDmx_nfy_fct(VOID*                  pv_nfy_tag,
                         MM_RANGE_COND_T          e_nfy_cond,
                         UINT32                 ui4_data_1,
                         UINT32                 ui4_data_2)
{
    UINT32 u4DmxIdx;

    mwdmx_param.pv_nfy_tag = pv_nfy_tag;
    mwdmx_param.e_nfy_cond = e_nfy_cond;
    mwdmx_param.ui4_data_1 = ui4_data_1;
    mwdmx_param.ui4_data_2 = ui4_data_2;
    mwdmx_param.pf_nfy = mwdmx_nfy;
    _CB_PutEvent_Adpt(CB_DRV_MWDMX_NFY, sizeof(MWDMX_NFY_PARAM), (void *)&mwdmx_param);

    /* Free memory for range. */
    if(e_nfy_cond == MM_RANGE_DONE)
    {
        for(u4DmxIdx = 0; u4DmxIdx < DMX_MM_MAX_INSTNUM; u4DmxIdx++)
        {
            if((UINT32)pv_nfy_tag == (UINT32)_arDmxMMInfo[u4DmxIdx].pRangeCbTag)
            {
                _MwDmx_remove_range_list(u4DmxIdx, ui4_data_1);
                break;
            }
        }
    }

    return TRUE;
}

#ifdef TIME_SHIFT_SUPPORT
static BOOL _MwDmx_set_dmx_nfy(UINT32          ui4_id,
                        VOID*           pv_nfy_tag,
                        DEMUX_COND_T    e_nfy_cond,
                        UINT32          ui4_data_1,
                        UINT32          ui4_data_2
            #ifndef MW_CLASSIC_DMX_NFY
                        ,
                        UINT64          ui8_ext_data_1,
                        UINT64          ui8_ext_data_2,
                        UINT32          ui4_ext_data_3
            #endif
                        )
{
    DMX_NFY_PARAM dmx_param;
    dmx_param.pf_nfy = _arScrambleState[ui4_id].pf_demux_nfy;
    dmx_param.pv_nfy_tag = pv_nfy_tag;
    dmx_param.e_nfy_cond = e_nfy_cond;
    dmx_param.ui4_data_1 = ui4_data_1;
    dmx_param.ui4_data_2 = ui4_data_2;
#ifndef MW_CLASSIC_DMX_NFY
    dmx_param.ui8_ext_data_1 = ui8_ext_data_1;
    dmx_param.ui8_ext_data_2 = ui8_ext_data_2;
    dmx_param.ui4_ext_data_3 = ui4_ext_data_3;
#endif
    _CB_PutEvent_Adpt(CB_DRV_DMX_NFY, sizeof(DMX_NFY_PARAM), &dmx_param);
    return FALSE;
}
#endif

static INT32 _MwDmx_recursion_data(MM_RANGE_ELMT_IDXTBL_T **ppt_data, MM_RANGE_ELMT_IDXTBL_T *ppt_data_user)
{
    VOID *pv_idx_tbl_entry_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_next_partial_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_next_tbl_user = NULL;
    INT32 i4_ret = RMR_OK;

    if (ppt_data_user)
    {
        *ppt_data = _MwDmxMalloc(sizeof(MM_RANGE_ELMT_IDXTBL_T));
        if (*ppt_data == NULL)
        {
            return RMR_NOT_ENOUGH_MEM;
        }

        rm_copy_from_user(*ppt_data, ppt_data_user, sizeof(MM_RANGE_ELMT_IDXTBL_T));
        pv_idx_tbl_entry_user = (*ppt_data)->u.pv_idx_tbl_entry;
        (*ppt_data)->u.pv_idx_tbl_entry = NULL;
        pt_next_partial_user = (*ppt_data)->pt_next_partial;
        (*ppt_data)->pt_next_partial = NULL;
        pt_next_tbl_user= (MM_RANGE_ELMT_IDXTBL_T *)(*ppt_data)->pv_next_tbl;
        (*ppt_data)->pv_next_tbl = NULL;

        /* Deal pt_pb_idx_tbl_entry/pt_ts_idx_tbl_entry data. */
        if((*ppt_data)->e_tbl_type == MM_RANGE_ELEMT_IDXTBL_TYPE_TIMESHIFT)
        {
            if (pv_idx_tbl_entry_user)
            {
                (*ppt_data)->u.pt_ts_idx_tbl_entry = _MwDmxCalloc(1,sizeof(MM_RANGE_ELEMT_IDXTBL_TS_ENTRY_T));

                if ((*ppt_data)->u.pt_ts_idx_tbl_entry)
                {
                    rm_copy_from_user((*ppt_data)->u.pt_ts_idx_tbl_entry, pv_idx_tbl_entry_user,
                        sizeof(MM_RANGE_ELEMT_IDXTBL_TS_ENTRY_T));
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_recursion_free_data(*ppt_data);
                    *ppt_data = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }
        }
        else
        {
            if((*ppt_data)->ui4_number_of_entry > 0 && pv_idx_tbl_entry_user)
            {
               (*ppt_data)->u.pt_pb_idx_tbl_entry = _MwDmxCalloc((*ppt_data)->ui4_number_of_entry,sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T));

                if ((*ppt_data)->u.pt_pb_idx_tbl_entry)
                {
                    rm_copy_from_user((*ppt_data)->u.pt_pb_idx_tbl_entry, pv_idx_tbl_entry_user,
                        sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T) * (*ppt_data)->ui4_number_of_entry);
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_recursion_free_data(*ppt_data);
                    *ppt_data = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }
        }

        /* Deal pt_next_partial data. */
        if(RMR_OK != (i4_ret = _MwDmx_recursion_data(&((*ppt_data)->pt_next_partial), pt_next_partial_user)))
        {
            _MwDmx_recursion_free_data(*ppt_data);
            *ppt_data = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }

        /* Deal pv_next_tbl data. */
        if(RMR_OK != (i4_ret = _MwDmx_recursion_data((MM_RANGE_ELMT_IDXTBL_T **)(&((*ppt_data)->pv_next_tbl)), pt_next_tbl_user)))
        {
            _MwDmx_recursion_free_data(*ppt_data);
            *ppt_data = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }
    }

    return RMR_OK;

}

static INT32 _MwDmx_pad_data_from_usr_struct(MM_RANGE_INFO_T** pprDmxRange,
    const VOID* pv_set_info,
    BOOL fgCopyFromUsr)
{
#if SUPPORT_MULTI_INFO
    VOID *pt_next_scale_user = NULL;
#endif
    VOID *pt_track_tm_scale_lst_user = NULL;
    VOID *pt_divx_drm_crypto_info_user = NULL;
    VOID *pt_vid_decoder_info_user = NULL;
    VOID *pt_aud_decoder_info_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_idxtbl_lst_user = NULL;
    INT32 i4_ret = RMR_OK;

    if(pprDmxRange == NULL || pv_set_info == NULL)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    *pprDmxRange = (MM_RANGE_INFO_T*)_MwDmxMalloc(sizeof(MM_RANGE_INFO_T));

    if (*pprDmxRange == NULL)
    {
        return RMR_NOT_ENOUGH_MEM;
    }

    if (fgCopyFromUsr)
    {
        rm_copy_from_user(*pprDmxRange,(VOID *)pv_set_info,sizeof(MM_RANGE_INFO_T));
    }
    else
    {
        x_memcpy(*pprDmxRange,pv_set_info,sizeof(MM_RANGE_INFO_T));
    }

    pt_track_tm_scale_lst_user = (*pprDmxRange)->pt_track_tm_scale_lst; // TODO: it seems that pt_track_tm_scale_lst is not used by driver
    (*pprDmxRange)->pt_track_tm_scale_lst = NULL;
    pt_divx_drm_crypto_info_user = (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info;
    (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info = NULL;
    pt_vid_decoder_info_user = (*pprDmxRange)->pt_vid_decoder_info;
    (*pprDmxRange)->pt_vid_decoder_info = NULL;
    pt_aud_decoder_info_user = (*pprDmxRange)->pt_aud_decoder_info;
    (*pprDmxRange)->pt_aud_decoder_info = NULL;
    pt_idxtbl_lst_user = (*pprDmxRange)->pt_idxtbl_lst;
    (*pprDmxRange)->pt_idxtbl_lst = NULL;
#if SUPPORT_MULTI_INFO
    pt_next_scale_user = (*pprDmxRange)->pt_next_scale;
    (*pprDmxRange)->pt_next_scale = NULL;
    if (pt_next_scale_user)
    {
        i4_ret = _MwDmx_pad_data_from_usr_struct(&(*pprDmxRange)->pt_next_scale,
            pt_next_scale_user, TRUE);
        if (RMR_OK != i4_ret)
        {
            _MwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return i4_ret;
        }
    }
#endif

    i4_ret = _MwDmx_recursion_data(&((*pprDmxRange)->pt_idxtbl_lst), pt_idxtbl_lst_user);
    if (RMR_OK != i4_ret)
    {
        _MwDmx_free_range_info(*pprDmxRange);
        *pprDmxRange = NULL;
        return i4_ret;
    }

    if ((*pprDmxRange)->ui4_track_tm_scale_num > 0 && pt_track_tm_scale_lst_user)
    {
        (*pprDmxRange)->pt_track_tm_scale_lst = _MwDmxCalloc((*pprDmxRange)->ui4_track_tm_scale_num,
            sizeof(MM_RANGE_TRACK_TM_SCALE_T));
        if ((*pprDmxRange)->pt_track_tm_scale_lst)
        {
            rm_copy_from_user((*pprDmxRange)->pt_track_tm_scale_lst, pt_track_tm_scale_lst_user,
                               sizeof(MM_RANGE_TRACK_TM_SCALE_T) * ((*pprDmxRange)->ui4_track_tm_scale_num));
        }
        else /* Error handling and error logs here */
        {
            _MwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }
    }

    if (pt_divx_drm_crypto_info_user)
    {
        (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info = _MwDmxMalloc(sizeof(DIVX_DRM_CRYPTO_INFO_T));

        if ((*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info)
        {
            rm_copy_from_user((*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info,
                               pt_divx_drm_crypto_info_user, sizeof(DIVX_DRM_CRYPTO_INFO_T));
        }
        else /* Error handling and error logs here */
        {
            _MwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }
    }

    if (pt_vid_decoder_info_user)
    {
        MM_RANGE_DECODER_T *ptDecoderInfo;

        (*pprDmxRange)->pt_vid_decoder_info = _MwDmxMalloc(sizeof(MM_RANGE_DECODER_T));
        if ((*pprDmxRange)->pt_vid_decoder_info)
        {
            rm_copy_from_user((*pprDmxRange)->pt_vid_decoder_info,
                               pt_vid_decoder_info_user, sizeof(MM_RANGE_DECODER_T));
        }
        else /* Error handling and error logs here */
        {
            _MwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }

        ptDecoderInfo = (*pprDmxRange)->pt_vid_decoder_info;
        while (ptDecoderInfo)
        {
            UINT32 u4CpyLen;
            VOID *pv_encoding_string_user = ptDecoderInfo->pv_encoding_string;
            VOID *pui1_codec_private_user = ptDecoderInfo->pui1_codec_private;
            VOID *pt_next_user = ptDecoderInfo->pt_next;
            ptDecoderInfo->pv_encoding_string = NULL;
            ptDecoderInfo->pui1_codec_private = NULL;
            ptDecoderInfo->pt_next = NULL;

            // For now condition (ptDecoderInfo != NULL) is only possible for mkv
            //if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
            if (ptDecoderInfo->b_has_encoding && pv_encoding_string_user)
            {
                UINT32 u4Idx, u4Total;
                MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding;

                ptDecoderInfo->pv_encoding_string = _MwDmxMalloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                if (ptDecoderInfo->pv_encoding_string)
                {
                    rm_copy_from_user(ptDecoderInfo->pv_encoding_string,
                                       pv_encoding_string_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }

                u4Total = ptDecoderInfo->ui4_encoding_num;
                ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)ptDecoderInfo->pv_encoding_string;
                for (u4Idx = 0; ptCntEncoding && u4Idx < u4Total; u4Idx++, ptCntEncoding = ptCntEncoding->pt_next)
                {
                    VOID *pui1_comp_settings_data_user = ptCntEncoding->pui1_comp_settings_data;
                    VOID *pt_cnt_enc_next_user = ptCntEncoding->pt_next;
                    ptCntEncoding->pui1_comp_settings_data = NULL;
                    ptCntEncoding->pt_next = NULL;

                    ASSERT(0 == (ptCntEncoding->ui8_comp_settings_length >> 32));
                    u4CpyLen = ptCntEncoding->ui8_comp_settings_length;
                    if (pui1_comp_settings_data_user && u4CpyLen)
                    {
                        ptCntEncoding->pui1_comp_settings_data = _MwDmxMalloc(u4CpyLen);
                        if (ptCntEncoding->pui1_comp_settings_data)
                        {
                            rm_copy_from_user(ptCntEncoding->pui1_comp_settings_data,
                                               pui1_comp_settings_data_user, u4CpyLen);
                        }
                        else /* Error handling and error logs here */
                        {
                            _MwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return RMR_NOT_ENOUGH_MEM;
                        }
                    }

                    if (pt_cnt_enc_next_user)
                    {
                        ptCntEncoding->pt_next = _MwDmxMalloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        if (ptCntEncoding->pt_next)
                        {
                            rm_copy_from_user(ptCntEncoding->pt_next,
                                               pt_cnt_enc_next_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        }
                        else /* Error handling and error logs here */
                        {
                            _MwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return RMR_NOT_ENOUGH_MEM;
                        }
                    }
                }
            }

            ASSERT(0 == (ptDecoderInfo->ui8_codec_private_len >> 32));
            u4CpyLen = ptDecoderInfo->ui8_codec_private_len;
            if (pui1_codec_private_user && u4CpyLen)
            {
                ptDecoderInfo->pui1_codec_private = _MwDmxMalloc(u4CpyLen);
                if (ptDecoderInfo->pui1_codec_private)
                {
                    rm_copy_from_user(ptDecoderInfo->pui1_codec_private,
                                       pui1_codec_private_user, u4CpyLen);
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }

            if (pt_next_user)
            {
                ptDecoderInfo->pt_next = _MwDmxMalloc(sizeof(MM_RANGE_DECODER_T));
                if (ptDecoderInfo->pt_next)
                {
                    rm_copy_from_user(ptDecoderInfo->pt_next,
                                       pt_next_user, sizeof(MM_RANGE_DECODER_T));
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }

            ptDecoderInfo = ptDecoderInfo->pt_next;
        }
    }

    if (pt_aud_decoder_info_user)
    {
        MM_RANGE_DECODER_T *ptDecoderInfo;

        (*pprDmxRange)->pt_aud_decoder_info = _MwDmxMalloc(sizeof(MM_RANGE_DECODER_T));
        if ((*pprDmxRange)->pt_aud_decoder_info)
        {
            rm_copy_from_user((*pprDmxRange)->pt_aud_decoder_info,
                               pt_aud_decoder_info_user, sizeof(MM_RANGE_DECODER_T));
        }
        else /* Error handling and error logs here */
        {
            _MwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return RMR_NOT_ENOUGH_MEM;
        }

        ptDecoderInfo = (*pprDmxRange)->pt_aud_decoder_info;
        while (ptDecoderInfo)
        {
            UINT32 u4CpyLen;
            VOID *pv_encoding_string_user = ptDecoderInfo->pv_encoding_string;
            VOID *pui1_codec_private_user = ptDecoderInfo->pui1_codec_private;
            VOID *pt_next_user = ptDecoderInfo->pt_next;
            ptDecoderInfo->pv_encoding_string = NULL;
            ptDecoderInfo->pui1_codec_private = NULL;
            ptDecoderInfo->pt_next = NULL;

            // For now condition (ptDecoderInfo != NULL) is only possible for mkv
            //if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
            if (ptDecoderInfo->b_has_encoding && pv_encoding_string_user)
            {
                UINT32 u4Idx, u4Total;
                MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding;

                ptDecoderInfo->pv_encoding_string = _MwDmxMalloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                if (ptDecoderInfo->pv_encoding_string)
                {
                    rm_copy_from_user(ptDecoderInfo->pv_encoding_string,
                                       pv_encoding_string_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }

                u4Total = ptDecoderInfo->ui4_encoding_num;
                ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)ptDecoderInfo->pv_encoding_string;
                for (u4Idx = 0; ptCntEncoding && u4Idx < u4Total; u4Idx++, ptCntEncoding = ptCntEncoding->pt_next)
                {
                    VOID *pui1_comp_settings_data_user = ptCntEncoding->pui1_comp_settings_data;
                    VOID *pt_cnt_enc_next_user = ptCntEncoding->pt_next;
                    ptCntEncoding->pui1_comp_settings_data = NULL;
                    ptCntEncoding->pt_next = NULL;

                    ASSERT(0 == (ptCntEncoding->ui8_comp_settings_length >> 32));
                    u4CpyLen = ptCntEncoding->ui8_comp_settings_length;
                    if (pui1_comp_settings_data_user && u4CpyLen)
                    {
                        ptCntEncoding->pui1_comp_settings_data = _MwDmxMalloc(u4CpyLen);
                        if (ptCntEncoding->pui1_comp_settings_data)
                        {
                            rm_copy_from_user(ptCntEncoding->pui1_comp_settings_data,
                                               pui1_comp_settings_data_user, u4CpyLen);
                        }
                        else /* Error handling and error logs here */
                        {
                            _MwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return RMR_NOT_ENOUGH_MEM;
                        }
                    }

                    if (pt_cnt_enc_next_user)
                    {
                        ptCntEncoding->pt_next = _MwDmxMalloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        if (ptCntEncoding->pt_next)
                        {
                            rm_copy_from_user(ptCntEncoding->pt_next,
                                               pt_cnt_enc_next_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        }
                        else /* Error handling and error logs here */
                        {
                            _MwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return RMR_NOT_ENOUGH_MEM;
                        }
                    }
                }
            }

            ASSERT(0 == (ptDecoderInfo->ui8_codec_private_len >> 32));
            u4CpyLen = ptDecoderInfo->ui8_codec_private_len;
            if (pui1_codec_private_user && u4CpyLen)
            {
                ptDecoderInfo->pui1_codec_private = _MwDmxMalloc(u4CpyLen);
                if (ptDecoderInfo->pui1_codec_private)
                {
                    rm_copy_from_user(ptDecoderInfo->pui1_codec_private,
                                       pui1_codec_private_user, u4CpyLen);
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }

            if (pt_next_user)
            {
                ptDecoderInfo->pt_next = _MwDmxMalloc(sizeof(MM_RANGE_DECODER_T));
                if (ptDecoderInfo->pt_next)
                {
                    rm_copy_from_user(ptDecoderInfo->pt_next,
                                       pt_next_user, sizeof(MM_RANGE_DECODER_T));
                }
                else /* Error handling and error logs here */
                {
                    _MwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return RMR_NOT_ENOUGH_MEM;
                }
            }

            ptDecoderInfo = ptDecoderInfo->pt_next;
        }
    }

    return RMR_OK;
}
#endif
static INT32 _MwDmxOnMMAddRange(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    BOOL fgRet;
    MM_RANGE_INFO_T* prDmxRange;

    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

#ifndef LINUX_TURNKEY_SOLUTION
    prDmxRange = (MM_RANGE_INFO_T*)pv_set_info;
#else
    if(_MwDmx_pad_data_from_usr_struct(&prDmxRange,pv_set_info, FALSE) != 0)
    {
        return RMR_DRV_ERROR;
    }

    _MwDmx_add_range_list(u1DmxMMInfoIdx, prDmxRange);
#endif
    
    fgRet = MTSWDMX_AddRange(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (VOID*)prDmxRange, prDmxRange->ui4_range_id);

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_BAD_INTERLEAVED_INFO, (UINT32)prDmxRange->b_is_bad_intlevd, 0, 0);
    if (prDmxRange->b_is_bad_intlevd)
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId= 0;
    }
#endif

    if (_arDmxMMInfo[u1DmxMMInfoIdx].fgFirstRange)
    {
        MTSTC_SetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, (UINT32)prDmxRange->t_init_stc);
        _arDmxMMInfo[u1DmxMMInfoIdx].fgFirstRange = FALSE;
    }

#ifndef LINUX_TURNKEY_SOLUTION
    prDmxRange = (MM_RANGE_INFO_T*)pv_set_info;
#endif

    _arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist = prDmxRange->b_key_tbl_exist;
    _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb = prDmxRange->b_thumbnail_pb;
    _arDmxMMInfo[u1DmxMMInfoIdx].pfnSwdmxRangeCb = prDmxRange->pt_range_nfy;
    _arDmxMMInfo[u1DmxMMInfoIdx].pRangeCbTag = prDmxRange->pv_tag;
    _arDmxMMInfo[u1DmxMMInfoIdx].u4RangeID = prDmxRange->ui4_range_id;
    _arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType = prDmxRange->e_mm_src_type;

#ifndef LINUX_TURNKEY_SOLUTION
    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_RANGECALLBACKEX, (UINT32)prDmxRange->pt_range_nfy, 0, 0);
#else
    mwdmx_nfy = prDmxRange->pt_range_nfy;
    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_RANGECALLBACKEX, (UINT32)_MwDmx_nfy_fct, 0, 0);
#endif

    return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}


static INT32 _MwDmxOnMMDelRange(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    BOOL fgRet;
    MM_RANGE_INFO_T* prDmxRange;

    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    prDmxRange = (MM_RANGE_INFO_T*)pv_set_info;
    fgRet = MTSWDMX_DelRange(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (VOID*)prDmxRange, prDmxRange->ui4_range_id, FALSE);

    #ifdef LINUX_TURNKEY_SOLUTION
    _MwDmx_remove_range_list(u1DmxMMInfoIdx, prDmxRange->ui4_range_id);
    #endif

    return ((fgRet == MTR_OK) ? RMR_OK : RMR_DRV_INV_SET_INFO);
}

/*
#ifdef TIME_SHIFT_SUPPORT
static BOOL _MwDmxSetDecrypt(UINT8 u1DmxMMInfoIdx)
{
    DMX_PID_T rPid;
    UINT8 i, j;

    UNUSED(u1DmxMMInfoIdx);
    if(u1DmxMMInfoIdx >= DMX_MM_MAX_INSTNUM)
    {
        LOG(1, "(u1DmxMMInfoIdx(%d) >= DMX_MM_MAX_INSTNUM)\n", (UINT32)u1DmxMMInfoIdx);
        return FALSE;
    }
    for(i=0; i<DMX_NUM_PID_INDEX; i++)
    {
        if(!DMX_GetPid(i, DMX_PID_FLAG_VALID | DMX_PID_FLAG_PID | DMX_PID_FLAG_TS_INDEX, &rPid))
        {
            return FALSE;
        }

        //For safety, comment by DMX PIC, to prevent race condition between swdmx thread and this thread
        //if(rPid.fgEnable)
        {
            for(j=0; j<DMX_NUM_PID_INDEX; j++)
            {
                if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid &&
                   (_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID == rPid.u2Pid))
                {
                    // Fixme
                    if(!DMX_MUL_SetPidxInstance(0, i))
                    {
                        return FALSE;
                    }

                    rPid.eDescMode = DMX_DESC_MODE_AES_ECB;
                    rPid.u1KeyIndex = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                    if(!DMX_SetFilePid(i, DMX_PID_FLAG_DESC_MODE | DMX_PID_FLAG_KEY_INDEX, &rPid))
                    {
                        return FALSE;
                    }
                }
            }
        }
    }

    return TRUE;
}
#endif  // TIME_SHIFT_SUPPORT
*/

static INT32 _MwDmxOnMMSetControl(DRV_COMP_ID_T* pt_comp_id,
  const VOID* pv_set_info)
{
    UINT32 u4Enable;
    UINT8 u1DmxMMInfoIdx = 0;
    DMX_MM_DEC_INFO_T *prDecInfo = NULL;
    BOOL fgVideo = TRUE;
    u4Enable = (UINT32)pv_set_info;

    if (pt_comp_id->e_type == DRVT_DEMUX_CONTAINER_ES)
    {
        if (pt_comp_id->ui2_id == _rDecInfo.u2CompId)
        {
            prDecInfo = &_rDecInfo;
        }
        else if (pt_comp_id->ui2_id == _rDecInfo1.u2CompId)
        {
            prDecInfo = &_rDecInfo1;
        }
        else if (pt_comp_id->ui2_id == _rDecInfo2.u2CompId)
        {
            prDecInfo = &_rDecInfo2;
        }
        // 0831
        else if (pt_comp_id->ui2_id == _rDecInfo3.u2CompId)
        {
            prDecInfo = &_rDecInfo3;
        }
        else if (pt_comp_id->ui2_id == _rDecInfo4.u2CompId)
        {
            prDecInfo = &_rDecInfo4;
        }        
        else
        {
            Printf( "%s Can't find decoder component id (%d)\n", __FUNCTION__, pt_comp_id->ui2_id);
            return RMR_DRV_INV_SET_INFO;
        }
    }

    if(u4Enable)
    {
        Printf("%s enable %d\n", __FUNCTION__, pt_comp_id->ui2_id);

        if ((_rDecInfo.u2CompId > 0) &&
            (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID) &&
            (_rDecInfo.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo.u2CompId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId = MPV_MW_GetEsId(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId);
            //_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId =  _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId = MW_VDP_Es2Vdp((UCHAR)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);

            Printf("%s u1DmxMMInfoIdx = %d, swdmx(%d), vdec(%d), b2r(%d), thumb(%d)\n",
                __FUNCTION__,
                u1DmxMMInfoIdx,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb);
        }
        else if ((_rDecInfo1.u2CompId > 0) &&
            (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID) &&
            (_rDecInfo1.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo1.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo1.u2CompId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId = MPV_MW_GetEsId(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId);
            //_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId =  _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId = MW_VDP_Es2Vdp((UCHAR)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);

            Printf("%s u1DmxMMInfoIdx = %d, swdmx(%d), vdec(%d), b2r(%d), thumb(%d)\n",
                __FUNCTION__,
                u1DmxMMInfoIdx,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb);
        }
        else if ((_rDecInfo2.u2CompId > 0) &&
                (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID) &&
                (_rDecInfo2.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo2.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo2.u2CompId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId = MPV_MW_GetEsId(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId);
            //_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId =  _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId = MW_VDP_Es2Vdp((UCHAR)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);

            Printf("%s u1DmxMMInfoIdx = %d, swdmx(%d), vdec(%d), b2r(%d), thumb(%d)\n",
                __FUNCTION__,
                u1DmxMMInfoIdx,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb);
        }
        // 0831
        else if ((_rDecInfo3.u2CompId > 0) &&
                (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID) &&
                (_rDecInfo3.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo3.u2CompId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId = MPV_MW_GetEsId(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId);
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId = MW_VDP_Es2Vdp((UCHAR)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);            
            Printf("%s u1DmxMMInfoIdx = %d, swdmx(%d), vdec(%d), b2r(%d), thumb(%d)\n",
                __FUNCTION__,
                u1DmxMMInfoIdx,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb);
            
        }
        else if ((_rDecInfo4.u2CompId > 0) &&
                (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID) &&
                (_rDecInfo4.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo4.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo4.u2CompId;
            _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId = MPV_MW_GetEsId(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId);
            _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId = MW_VDP_Es2Vdp((UCHAR)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);            
            Printf("%s u1DmxMMInfoIdx = %d, swdmx(%d), vdec(%d), b2r(%d), thumb(%d)\n",
                __FUNCTION__,
                u1DmxMMInfoIdx,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                _arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                _arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb);
            
            
        }        
        else
        {
            fgVideo = FALSE;
            Printf("no video (%d)\n", pt_comp_id->ui2_id);
        }

        #ifdef CC_3D_MM_DS_SUPPORT
        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
            (pt_comp_id->ui2_id == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
            fgVideo)
        {
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                  (UINT8)MTSWDMX_SET_VDEC2_ID,
                                  (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                                  0,
                                  0);
        }
        else
        #endif
        if(fgVideo)
        {
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                  (UINT8)MTSWDMX_SET_VDEC_ID,
                                  (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                                  0,
                                  0);
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                  (UINT8)MTSWDMX_SET_B2R_ID,
                                  (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,
                                  0,
                                  0);
        }

        if ((_rDecInfo.u2CompId > 0) &&
            ((_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3)) &&
            (_rDecInfo.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        }
        else if ((_rDecInfo1.u2CompId > 0) &&
                ((_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3)) &&
                (_rDecInfo1.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo1.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        }
        else if ((_rDecInfo2.u2CompId > 0) &&
                ((_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3)) &&
                (_rDecInfo2.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo2.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        }
        else if ((_rDecInfo3.u2CompId > 0) &&
                ((_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3)) &&
                (_rDecInfo3.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        }
        else if ((_rDecInfo4.u2CompId > 0) &&
                ((_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3)) &&
                (_rDecInfo4.u2CompId == pt_comp_id->ui2_id))
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo4.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        }
        else
        {
            Printf( "no audio\n");
        }
        
        if (pt_comp_id->e_type == DRVT_DEMUX_CONTAINER_ES)
        {
            UINT32 j;
            BOOL fgVideo = FALSE;
            BOOL fgDecrypt = FALSE;
            UINT8 u1KeyIdx = 0;
            MTDMX_DESC_MODE_T eDescMode = MTDMX_DESC_MODE_NONE;
            UNUSED(fgDecrypt);
            UNUSED(u1KeyIdx);
            UNUSED(eDescMode);
            UNUSED(j);
            if ((!_rDecInfo.fgEnable) &&
                (_rDecInfo.u2CompId == pt_comp_id->ui2_id))
            {
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
                MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, _rDecInfo.u4StrmId, _rDecInfo.u4SubStrmId, (ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo.u2DecType);

#ifdef TIME_SHIFT_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION)
#ifdef TS_MARLIN_SUPPORT
                    ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188)
#endif
                    ||(_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192)
                ))
                {
                    //for(j=0; j<DMX_NUM_PID_INDEX; j++)
                    for(j=0; j<DMX_MAX_NUM_PID_INDEX_TABLE; j++)
                    {
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid && (_rDecInfo.u4StrmId == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID))
                        {
                            fgDecrypt = TRUE;
                            eDescMode = MTDMX_DESC_MODE_AES_ECB;
                            u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                        }
#ifdef  TS_MARLIN_SUPPORT
                        if((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188) ||
                            (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188))
                        {
                            if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid)
                            {
                                fgDecrypt = TRUE;
                                eDescMode = DMX_DESC_MODE_AES_CBC;
                                u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                                break;
                            }
                        }
#endif
                    }

                    if(fgDecrypt)
                    {
                        if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo.u2DecType == MTSWDMX_STRM_TYPE_VID)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_VIDEO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo.u2DecType == MTSWDMX_STRM_TYPE_AUD)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        //0831
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo.u2DecType == MTSWDMX_STRM_TYPE_AUD2)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO2_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo.u2DecType == MTSWDMX_STRM_TYPE_AUD3)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO3_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                    }
                }
#endif

                MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_ENABLE_STREAM,
                                (UINT32)_rDecInfo.u2DecType, 0, 0);
                    
                if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
                {
                    _MwDmxSetDescryptInfo(&_arDmxMMInfo[u1DmxMMInfoIdx], _rDecInfo.u2DecType);
                    
                }
                Printf("_MwDmxOnMMSetControl Info Strm Type(%d) _rDecInfo.u2DecType(%d) SubType(%d) fgDecrypt(%d)\n", _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType,
                        _rDecInfo.u2DecType,_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType,fgDecrypt);

                _rDecInfo.fgEnable = TRUE;

                if ((_rDecInfo.u2CompId > 0) &&
                    (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
                    _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo.u2CompId;
                    fgVideo = TRUE;
                }
            }
            else if ((!_rDecInfo1.fgEnable) &&
                (_rDecInfo1.u2CompId == pt_comp_id->ui2_id))
            {
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

                #ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo1.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        _rDecInfo1.u4StrmId,
                        _rDecInfo1.u4SubStrmId,
                        MTSWDMX_STRM_TYPE_VID2);
                }
                else
                #endif
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                    _rDecInfo1.u4StrmId,
                                    _rDecInfo1.u4SubStrmId,
                                    (ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo1.u2DecType);
                }
#ifdef TIME_SHIFT_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION)
#ifdef TS_MARLIN_SUPPORT
                    ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188)
#endif
                    || (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192)
                    ))
                {
                    //for(j=0; j<DMX_NUM_PID_INDEX; j++)
                    for(j=0; j<DMX_MAX_NUM_PID_INDEX_TABLE; j++)
                    {
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid && (_rDecInfo1.u4StrmId == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID))
                        {
                            fgDecrypt = TRUE;
                            eDescMode = DMX_DESC_MODE_AES_ECB;
                            u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                        }
#ifdef  TS_MARLIN_SUPPORT
                        if((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188) ||
                            (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_188))
                        {
                            if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid)
                            {
                                fgDecrypt = TRUE;
                                eDescMode = DMX_DESC_MODE_AES_CBC;
                                u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                                break;
                            }
                        }
#endif
                    }

                    if(fgDecrypt)
                    {
                        if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo1.u2DecType == MTSWDMX_STRM_TYPE_VID)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_VIDEO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo1.u2DecType == MTSWDMX_STRM_TYPE_AUD)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        //0831
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo1.u2DecType == MTSWDMX_STRM_TYPE_AUD2)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO2_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo1.u2DecType == MTSWDMX_STRM_TYPE_AUD3)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO3_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }                        
                        
                    }
                }
#endif

#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo1.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                    (UINT8)MTSWDMX_ENABLE_STREAM,
                                    (UINT32)_rDecInfo1.u2DecType, 0, 0);
                        
                    if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
                    {
                        _MwDmxSetDescryptInfo(&_arDmxMMInfo[u1DmxMMInfoIdx], _rDecInfo1.u2DecType);
                    }
                     
					Printf("_MwDmxOnMMSetControl Info Strm Type(%d) _rDecInfo1.u2DecType(%d) SubType(%d) fgDecrypt(%d)\n", _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType,
                        _rDecInfo1.u2DecType,_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType,fgDecrypt);
                }
                _rDecInfo1.fgEnable = TRUE;

                if ((_rDecInfo1.u2CompId > 0) &&
                    (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
                    _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo1.u2CompId;
                    fgVideo = TRUE;
                }
            }
            else if ((!_rDecInfo2.fgEnable) &&
                (_rDecInfo2.u2CompId == pt_comp_id->ui2_id))
            {
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

                #ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo2.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        _rDecInfo2.u4StrmId,
                        _rDecInfo2.u4SubStrmId,
                        MTSWDMX_STRM_TYPE_VID2);
                }
                else
                #endif
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        _rDecInfo2.u4StrmId,
                        _rDecInfo2.u4SubStrmId,
                        (ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo2.u2DecType);
                }
#ifdef TIME_SHIFT_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192)))
                {
                    //for(j=0; j<DMX_NUM_PID_INDEX; j++)
                    for(j=0; j<DMX_MAX_NUM_PID_INDEX_TABLE; j++)
                    {
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid && (_rDecInfo2.u4StrmId == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID))
                        {
                            fgDecrypt = TRUE;
                            eDescMode = DMX_DESC_MODE_AES_ECB;
                            u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                        }
                    }

                    if(fgDecrypt)
                    {
                        if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo2.u2DecType == MTSWDMX_STRM_TYPE_VID)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_VIDEO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo2.u2DecType == MTSWDMX_STRM_TYPE_AUD)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_AUDIO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        //0831
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo2.u2DecType == MTSWDMX_STRM_TYPE_AUD2)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO2_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo2.u2DecType == MTSWDMX_STRM_TYPE_AUD3)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO3_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }                        
                        
                    }
                }
#endif
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo2.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                    (UINT8)MTSWDMX_ENABLE_STREAM,
                                    MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        (UINT32)_rDecInfo2.u2DecType, 0, 0);
                        
                    if (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
                    {
                        _MwDmxSetDescryptInfo(&_arDmxMMInfo[u1DmxMMInfoIdx], _rDecInfo2.u2DecType);
                    }
                    
					Printf("_MwDmxOnMMSetControl Info Strm Type(%d) _rDecInfo2.u2DecType(%d) SubType(%d) fgDecrypt(%d)\n", _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType,
                        _rDecInfo2.u2DecType,_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType,fgDecrypt);
                }
                _rDecInfo2.fgEnable = TRUE;

                if ((_rDecInfo2.u2CompId > 0) &&
                    (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
                    _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo2.u2CompId;
                    fgVideo = TRUE;
                }
            }
            else if ((!_rDecInfo3.fgEnable) &&
                (_rDecInfo3.u2CompId == pt_comp_id->ui2_id))
            {
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
                
                #ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        _rDecInfo3.u4StrmId,
                        _rDecInfo3.u4SubStrmId,
                        MTSWDMX_STRM_TYPE_VID2);         
                }
                else
                #endif
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        _rDecInfo3.u4StrmId,
                        _rDecInfo3.u4SubStrmId,
                        (ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo3.u2DecType);
                }
#ifdef TIME_SHIFT_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192)))
                {
                    for(j=0; j<DMX_MAX_NUM_PID_INDEX_TABLE; j++)
                    {
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid && (_rDecInfo3.u4StrmId == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID))
                        {
                            fgDecrypt = TRUE;
                            eDescMode = DMX_DESC_MODE_AES_ECB;
                            u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                        }
                    }

                    if(fgDecrypt)
                    {
                        if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo3.u2DecType == MTSWDMX_STRM_TYPE_VID)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_VIDEO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo3.u2DecType == MTSWDMX_STRM_TYPE_AUD)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_AUDIO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        //0831
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo3.u2DecType == MTSWDMX_STRM_TYPE_AUD2)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO2_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo3.u2DecType == MTSWDMX_STRM_TYPE_AUD3)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO3_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }                        
                        
                    }                    
                }
#endif
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, _rDecInfo3.u2CompId, 0);
                }
                else
#endif
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        (UINT32)_rDecInfo3.u2DecType, _rDecInfo3.u2CompId, 0);
					Printf("_MwDmxOnMMSetControl Info Strm Type(%d) _rDecInfo3.u2DecType(%d) SubType(%d) fgDecrypt(%d)\n", _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType,
                        _rDecInfo3.u2DecType,_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType,fgDecrypt);
                }
                _rDecInfo3.fgEnable = TRUE;

                if ((_rDecInfo3.u2CompId > 0) &&
                    (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
                    _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo3.u2CompId;
                    fgVideo = TRUE;
                }
            }
            else if ((!_rDecInfo4.fgEnable) &&
                (_rDecInfo4.u2CompId == pt_comp_id->ui2_id))
            {
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
                
                #ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        _rDecInfo4.u4StrmId,
                        _rDecInfo4.u4SubStrmId,
                        MTSWDMX_STRM_TYPE_VID2);                
                }
                else
                #endif
                {
                    MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        _rDecInfo4.u4StrmId,
                        _rDecInfo4.u4SubStrmId,
                        (ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo4.u2DecType);
                }
#ifdef TIME_SHIFT_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192_ENCRYPTION) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_192)))
                {
                    for(j=0; j<DMX_MAX_NUM_PID_INDEX_TABLE; j++)
                    {
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].fgValid && (_rDecInfo4.u4StrmId == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u2StreamID))
                        {
                            fgDecrypt = TRUE;
                            eDescMode = DMX_DESC_MODE_AES_ECB;
                            u1KeyIdx = _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[j].u1KeyIdx;
                        }
                    }
                    if(fgDecrypt)
                    {
                        if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo4.u2DecType == MTSWDMX_STRM_TYPE_VID)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_VIDEO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo4.u2DecType == MTSWDMX_STRM_TYPE_AUD)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                (UINT8)MTSWDMX_SET_AUDIO_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        //0831
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo4.u2DecType == MTSWDMX_STRM_TYPE_AUD2)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO2_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }
                        else if((ENUM_MTSWDMX_STRM_TYPE_T)_rDecInfo4.u2DecType == MTSWDMX_STRM_TYPE_AUD3)
                        {
                            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_AUDIO3_DECRYPT, (UINT32)u1KeyIdx,
                                (UINT32)eDescMode, 0);
                        }                        
                    }                    
                }
#endif
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, _rDecInfo4.u2CompId, 0);
                }
                else
#endif
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_ENABLE_STREAM,
                        (UINT32)_rDecInfo4.u2DecType, _rDecInfo4.u2CompId, 0);
					Printf("_MwDmxOnMMSetControl Info Strm Type(%d) _rDecInfo4.u2DecType(%d) SubType(%d) fgDecrypt(%d)\n", _arDmxMMInfo[u1DmxMMInfoIdx].rContainerType,
                        _rDecInfo4.u2DecType,_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType,fgDecrypt);
                }
                _rDecInfo4.fgEnable = TRUE;

                if ((_rDecInfo4.u2CompId > 0) &&
                    (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = TRUE;
                    _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = _rDecInfo4.u2CompId;
                    fgVideo = TRUE;
                }
            }
            else
            {
                Printf("all enable or unknown u2CompId %d\n", pt_comp_id->ui2_id);
            }
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                (UINT8)MTSWDMX_SET_STC_ID,
                (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, 0, 0);
            if(fgVideo)
            {
                // there is a case video stream will change
                // but we want to keep the speed
                // So av sync mode would not be set again.
                // we have to set sync mode to video
                // or video drive would not know the current sync mode is.
                // refer to dmx_pcr.c
                u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
                DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

                Printf("dmx id = %d, swdmx id = %d, vdec id = %d\n",
                    u1DmxMMInfoIdx,
                    _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                    _arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId);

                MTVDEC_PlayMM(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId , TRUE);
                MTVDEC_SetSrcId(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

#ifdef VDEC_TIME_PROFILE_PLAY_FREE_RUN
                MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_FREE_RUN, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
#else
                if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_1X)
                {
                    if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                       ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                        (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
                    {
                        // system master
                        MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_SLAVE_MM, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                    }
                    else if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb)
                    {
                        MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_FREE_RUN, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                    }
                    else
                    {
                        // audio master
                        MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_SLAVE_MM, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
#ifdef CC_3D_MM_DS_SUPPORT
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack)
                        {
                            MTVDEC_SetAvSynMode2(1, MTAV_SYNC_SLAVE_MM, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                        }
#endif
                    }
                }
                else if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_2X)
                {
                    // video master
                    MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_MASTER, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                }
                else
                {
                    // audio master
                    MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId, MTAV_SYNC_SLAVE_MM, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
#ifdef CC_3D_MM_DS_SUPPORT
                    if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack)
                    {
                        MTVDEC_SetAvSynMode2(1, MTAV_SYNC_SLAVE_MM, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                    }
#endif
                }
#endif
            }
            /*
#ifdef TIME_SHIFT_SUPPORT
            if(!_MwDmxSetDecrypt(u1DmxMMInfoIdx))
            {
                LOG(3, "%s _MwDmxSetDecrypt fail\n", __FUNCTION__);
            }
#endif  // TIME_SHIFT_SUPPORT
            */
        }
        else
        {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

            // STC ID should be ready in control ES stage
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                (UINT8)MTSWDMX_SET_STC_ID,
                (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, 0, 0);

            if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_1X)
            {
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
                {
                    MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_SYSTEM_MASTER);
                }
                else if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIsThumbnailPb)
                {
                    MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                }
                else
                {
                    if(_arDmxMMInfo[u1DmxMMInfoIdx].fgFreeRun)
                    {
                        MTSTC_StopStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist)
                        {
                            MTVDEC_SetAvSynMode2(_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                                MTAV_SYNC_FREE_RUN, _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                            //VDP_SetB2rUpdateStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId,FALSE);
                        }
                        // TODO: set the stc id to audio driver
                        MTAUDDEC_SetAvSynMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1AdecId, MTAV_SYNC_FREE_RUN);
                    }
                    else if (_arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType == MM_SRC_TYPE_WIFI_DISPLAY)
                    {
                        MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_SYSTEM_MASTER);
                        Printf("WFD mode set AV sync to system master\n");
                    }
                    else if (_arDmxMMInfo[u1DmxMMInfoIdx].u4SourceType == MM_SRC_TYPE_NETWORK_SKYPE)
                    {
                        MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                        Printf("Skype mode set AV sync to free run\n");
                    }
                    else if ((_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist) &&
                             ((!_arDmxMMInfo[u1DmxMMInfoIdx].fgAudIsConn) || _arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport))
                    {
                        MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                        MTSTC_StartStc(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId);
                        Printf("No audio, Set AV sync mode to none mode\n");
                    }
                    else
                    {
                        MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
                        Printf("Set AV sync mode to audio master\n");
                    }
                }
            }
            else if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_FORWARD_2X)
            {
#ifdef VDEC_TIME_SHIFT_2X_I_MODE
                if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
                {
                    MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
                }
                else
#endif
                {
                    MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_VIDEO_MASTER);
                }
            }
            else
            {
            #if 0
                MTSTC_SetSyncMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, MTSTC_AV_SYNC_MODE_AUDIO_MASTER);
                // if we set none, vdec will to underflow check
                // prVdecEsInfo->u2UdfNs = MPV_UDF_PES_NS
                // so use audio master
            #endif
                MW_DMX_STC_SetSyncMode(u1DmxMMInfoIdx, MTSTC_AV_SYNC_MODE_NONE);
            }

            #if 0 // conainer ES needs it too.
            MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                                  (UINT8)MTSWDMX_SET_VDEC_ID,
                                  (UINT32)_arDmxMMInfo[u1DmxMMInfoIdx].u1VdecId,
                                  0,
                                  0);
            #endif

            _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed = MM_SPEED_TYPE_FORWARD_1X;
            //SRM_SetMmMode(SRM_MM_MODE_TYPE_VIDEO);
            if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
            {
                _MW_DMX_SetCacheOn(FALSE);
                if(!_MW_DMX_FlushAllCachePid())
                {
                    Printf("_MwDmxOnMMSetControl _MW_DMX_FlushAllCachePid fail\n");
                }
            }
            MTSWDMX_Play(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        }
     }
   else
   {
       if(pt_comp_id->e_type == DRVT_DEMUX_CONTAINER_ES)
       {
            BOOL fgVideo = FALSE;
            // TS file with program change
            // We have to stop vdp to let MPV_FlushEsm finish
            if ((_rDecInfo.u2CompId > 0) &&
                (_rDecInfo.u2CompId == pt_comp_id->ui2_id) &&
                (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
            {
                fgVideo = TRUE;
            }
            else if ((_rDecInfo1.u2CompId > 0) &&
                (_rDecInfo1.u2CompId == pt_comp_id->ui2_id) &&
                (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
            {
                fgVideo = TRUE;
            }
            else if ((_rDecInfo2.u2CompId > 0) &&
                (_rDecInfo2.u2CompId == pt_comp_id->ui2_id) &&
                (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
            {
                fgVideo = TRUE;
            }
            // 0831
             else if ((_rDecInfo3.u2CompId > 0) &&
                (_rDecInfo3.u2CompId == pt_comp_id->ui2_id) &&
                (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
            {
                fgVideo = TRUE;
            }
            else if ((_rDecInfo4.u2CompId > 0) &&
                 (_rDecInfo4.u2CompId == pt_comp_id->ui2_id) &&
                 (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
            {
                 fgVideo = TRUE;
            }            

            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

            if((_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist)
               &&
               (fgVideo)
               &&
               ((_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed == MM_SPEED_TYPE_STEP)
                ||
                ((_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed >= MM_SPEED_TYPE_FORWARD_1_DIV_32X)
                 &&
                 (_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed <= MM_SPEED_TYPE_FORWARD_1_DIV_2X))
               )
              )
            {
                UINT8 u1Val = 0;

                MTB2R_SetPauseMM(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, FALSE);

                // Reset Step Forward status
                MTB2R_GetStepForward(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &u1Val);
                if(u1Val != 0)
                {
                    MTB2R_SetFBNoTimeout(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, FALSE);
                    MTB2R_SetStepForward(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, MTB2R_PLAY_NORMAL);
                }
                // Reset Slow Forward status
                MTB2R_GetSlowForward(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &u1Val);
                if(u1Val != 0)
                {
                    MTB2R_SetFBNoTimeout(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, FALSE);
                    MTB2R_SetSlowForward(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, MTB2R_PLAY_NORMAL);
                }
                MTB2R_SetSpeed(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, MTSTC_SPEED_TYPE_FORWARD_1X);

                // Reset trick mode status
                MTB2R_SetTrickMode(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId,MTB2R_PLAY_NORMAL);
            }

            if (_rDecInfo.fgEnable &&
                (_rDecInfo.u2CompId == pt_comp_id->ui2_id))
            {
#ifdef CC_3D_MM_DS_SUPPORT
                if((_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt) && (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    (_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt)--;
                }
#endif
                if((_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))
                {                   
                    _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = FALSE;             
                }
                MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)_rDecInfo.u2DecType, 0, 0);
                _rDecInfo.fgEnable = FALSE;
            }
            else if (_rDecInfo1.fgEnable &&
                (_rDecInfo1.u2CompId == pt_comp_id->ui2_id))
            {
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo1.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        (UINT8)MTSWDMX_DISABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {
#ifdef CC_3D_MM_DS_SUPPORT
                    if((_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt) && (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                    {
                        (_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt)--;
                    }
#endif
                    if((_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))
                    {                   
                        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = FALSE;             
                    }                    
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)_rDecInfo1.u2DecType, 0, 0);
                }
                _rDecInfo1.fgEnable = FALSE;
            }
            else if (_rDecInfo2.fgEnable &&
                (_rDecInfo2.u2CompId == pt_comp_id->ui2_id))
            {
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo2.u2CompId == _arDmxMMInfo[u1DmxMMInfoIdx].u2SubVidComp) &&
                    (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                        (UINT8)MTSWDMX_DISABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {
#ifdef CC_3D_MM_DS_SUPPORT
                    if((_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt) && (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                    {
                        (_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt)--;
                    }
#endif
                    if((_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))
                    {                   
                        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = FALSE;             
                    }
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)_rDecInfo2.u2DecType, 0, 0);
                }
                _rDecInfo2.fgEnable = FALSE;
            }
            else if (_rDecInfo3.fgEnable &&
                (_rDecInfo3.u2CompId == pt_comp_id->ui2_id))
            {
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_DISABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {   
#ifdef CC_3D_MM_DS_SUPPORT
                    if((_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt) && (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                    {
                        (_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt)--;
                    }
#endif                
                    if((_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))
                    {                   
                         _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = FALSE;                
                    }
                
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)_rDecInfo3.u2DecType, 0, 0);
                }
                _rDecInfo3.fgEnable = FALSE;
            }
            else if (_rDecInfo4.fgEnable &&
                (_rDecInfo4.u2CompId == pt_comp_id->ui2_id))
            {
#ifdef CC_3D_MM_DS_SUPPORT
                if(_arDmxMMInfo[u1DmxMMInfoIdx].fgIs3DMultitrack &&
                    (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                {
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
                        (UINT8)MTSWDMX_DISABLE_STREAM,
                        MTSWDMX_STRM_TYPE_VID2, 0, 0);
                }
                else
#endif
                {
#ifdef CC_3D_MM_DS_SUPPORT
                    if((_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt) && (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID))
                    {
                        (_arDmxMMInfo[u1DmxMMInfoIdx].u1VidCnt)--;
                    }
#endif                
                    if((_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))
                    {                   
                         _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = FALSE;                
                    }
                    
                    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_DISABLE_STREAM, (UINT32)_rDecInfo4.u2DecType, 0, 0);
                }
                _rDecInfo4.fgEnable = FALSE;
            }            
            else
            {
                Printf("disable or unknown u2CompId %d\n", pt_comp_id->ui2_id);
            }
       }
       else
       {
            u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

            _MwDmxFlushCtrl(u1DmxMMInfoIdx);
            Printf("[SWDMX]before stop\n");
            MTSWDMX_Stop(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
            Printf("[SWDMX]after stop\n");
            _MwDmxWaitCtrlDone(u1DmxMMInfoIdx);
            Printf("[SWDMX]before flush\n");
            MTSWDMX_Flush(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
            Printf("[SWDMX]after  flush\n");
            if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
            {
                _MW_DMX_SetCacheOn(TRUE);
            }

            //if(!_fgThumbnailMode)
            {
            //    SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
            }

            _arDmxMMInfo[u1DmxMMInfoIdx].fgFirstRange = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed = (UINT32)0xFFFFFFFF;

            _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsPause = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsTrick = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].fgPreviousStateIsSeek = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].fgResetStcVal=FALSE;
            /*
            _rDecInfo.fgEnable = FALSE;
            _rDecInfo1.fgEnable = FALSE;
            _rDecInfo2.fgEnable = FALSE;
            */
       }
   }

    Printf("%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    return RMR_OK;
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
static INT32 _MwDmxOnMMSetDestination(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxIdx;
    DEMUX_DEST_INFO_T* ptDestInfo;
    INT32 i4Ret = RMR_OK;
    UINT32 eTypeAud = MTSWDMX_STRM_TYPE_UNKNOWN;

    ptDestInfo = (DEMUX_DEST_INFO_T*)pv_set_info;

    if (ptDestInfo == NULL)
    {
        return i4Ret;
    }

    switch (ptDestInfo->e_type)
    {
    case DRVT_VID_DEC:
        if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo.u2DecType = (UINT16)MTSWDMX_STRM_TYPE_VID;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            LOG(5, "%s  MTSWDMX_STRM_TYPE_VID u1DmxMMInfoIdx = %d\n",
                __FUNCTION__,
                u1DmxIdx);
            _arDmxMMInfo[u1DmxIdx].fgVidIsConn = TRUE;
#ifdef CC_3D_MM_DS_SUPPORT
            _arDmxMMInfo[u1DmxIdx].u1VidCnt++;
#endif
            /* Todo
            MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            */
        }
        else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo1.u2DecType = (UINT16)MTSWDMX_STRM_TYPE_VID;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo1.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            LOG(5, "%s  MTSWDMX_STRM_TYPE_VID u1DmxMMInfoIdx = %d\n",
                __FUNCTION__,
                u1DmxIdx);
            _arDmxMMInfo[u1DmxIdx].fgVidIsConn = TRUE;
#ifdef CC_3D_MM_DS_SUPPORT
            _arDmxMMInfo[u1DmxIdx].u1VidCnt++;
            if(_arDmxMMInfo[u1DmxIdx].u1VidCnt > 1)
            {
                _arDmxMMInfo[u1DmxIdx].fgIs3DMultitrack = TRUE;
                _arDmxMMInfo[u1DmxIdx].u2SubVidComp = _rDecInfo1.u2CompId;
            }
#endif
            /* Todo
            MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            */
        }
        else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo2.u2DecType = (UINT16)MTSWDMX_STRM_TYPE_VID;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo2.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
           Printf( "%s  MTSWDMX_STRM_TYPE_VID u1DmxMMInfoIdx = %d\n",
                __FUNCTION__,
                u1DmxIdx);
            _arDmxMMInfo[u1DmxIdx].fgVidIsConn = TRUE;
#ifdef CC_3D_MM_DS_SUPPORT
            _arDmxMMInfo[u1DmxIdx].u1VidCnt++;
            if(_arDmxMMInfo[u1DmxIdx].u1VidCnt > 1)
            {
                _arDmxMMInfo[u1DmxIdx].fgIs3DMultitrack = TRUE;
                _arDmxMMInfo[u1DmxIdx].u2SubVidComp = _rDecInfo2.u2CompId;
            }
#endif
            /* Todo
            MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            */
        }
        else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo3.u2DecType = (UINT16)MTSWDMX_STRM_TYPE_VID;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
           Printf( "%s  MTSWDMX_STRM_TYPE_VID u1DmxMMInfoIdx = %d\n", 
                __FUNCTION__, 
                u1DmxIdx);
            _arDmxMMInfo[u1DmxIdx].fgVidIsConn = TRUE;
#ifdef CC_3D_MM_DS_SUPPORT
            _arDmxMMInfo[u1DmxIdx].u1VidCnt++;
            if(_arDmxMMInfo[u1DmxIdx].u1VidCnt > 1)
            {
                _arDmxMMInfo[u1DmxIdx].fgIs3DMultitrack = TRUE;
                _arDmxMMInfo[u1DmxIdx].u2SubVidComp = _rDecInfo2.u2CompId;                
            }
#endif
            /* Todo
            MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            */
        }
        else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo4.u2DecType = (UINT16)MTSWDMX_STRM_TYPE_VID;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
           Printf( "%s  MTSWDMX_STRM_TYPE_VID u1DmxMMInfoIdx = %d\n", 
                __FUNCTION__, 
                u1DmxIdx);
            _arDmxMMInfo[u1DmxIdx].fgVidIsConn = TRUE;
#ifdef CC_3D_MM_DS_SUPPORT
            _arDmxMMInfo[u1DmxIdx].u1VidCnt++;
            if(_arDmxMMInfo[u1DmxIdx].u1VidCnt > 1)
            {
                _arDmxMMInfo[u1DmxIdx].fgIs3DMultitrack = TRUE;
                _arDmxMMInfo[u1DmxIdx].u2SubVidComp = _rDecInfo2.u2CompId;
            }
#endif
            /* Todo
            MW_MPV_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            MW_VDP_SET_SWDMXSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            */
        }
        else
        {
            LOG(3, "unknown u2CompId %d\n", pt_comp_id->ui2_id);
        }
        break;
    case DRVT_AUD_DEC:
        if(ptDestInfo->ui2_id == MT_AUD_DEC_MAIN)
        {
            eTypeAud = MTSWDMX_STRM_TYPE_AUD;
        }
        else if(ptDestInfo->ui2_id == MT_AUD_DEC_AUX)
        {
            eTypeAud = MTSWDMX_STRM_TYPE_AUD2;
        }
        else if(ptDestInfo->ui2_id == MT_AUD_DEC_THIRD)
        {
            eTypeAud = MTSWDMX_STRM_TYPE_AUD3;
        }
        if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo.u2DecType = (UINT16)eTypeAud;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            Printf( "%s  MTSWDMX_STRM_TYPE_AUD u1DmxMMInfoIdx = %d, swdmx id = %d\n",
                __FUNCTION__,
                u1DmxIdx,
                _arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            _arDmxMMInfo[u1DmxIdx].fgAudIsConn = TRUE;
            _arDmxMMInfo[u1DmxIdx].fgAudNotSupport = FALSE;
            // Todo MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
        }
        else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
        {
            _rDecInfo1.u2DecType = (UINT16)eTypeAud;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo1.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            Printf( "%s  MTSWDMX_STRM_TYPE_AUD u1DmxMMInfoIdx = %d, swdmx id = %d\n",
                __FUNCTION__,
                u1DmxIdx,
                _arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            _arDmxMMInfo[u1DmxIdx].fgAudIsConn = TRUE;
            _arDmxMMInfo[u1DmxIdx].fgAudNotSupport = FALSE;
            // Todo MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
        }
        else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
        {
           _rDecInfo2.u2DecType = (UINT16)eTypeAud;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo2.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            Printf( "%s  MTSWDMX_STRM_TYPE_AUD u1DmxMMInfoIdx = %d, swdmx id = %d\n",
                __FUNCTION__,
                u1DmxIdx,
                _arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            _arDmxMMInfo[u1DmxIdx].fgAudIsConn = TRUE;
            _arDmxMMInfo[u1DmxIdx].fgAudNotSupport = FALSE;
            // Todo MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
        }
        else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
        {
           _rDecInfo3.u2DecType = (UINT16)eTypeAud;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            Printf( "%s  MTSWDMX_STRM_TYPE_AUD u1DmxMMInfoIdx = %d, swdmx id = %d\n", 
                __FUNCTION__, 
                u1DmxIdx,
                _arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            _arDmxMMInfo[u1DmxIdx].fgAudIsConn = TRUE;
            _arDmxMMInfo[u1DmxIdx].fgAudNotSupport = FALSE;
            // Todo MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
        }
        else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
        {
           _rDecInfo4.u2DecType = (UINT16)eTypeAud;
            u1DmxIdx = _MwDmxFindDmxIdx(_rDecInfo4.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
            DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxIdx, RMR_DRV_INV_SET_INFO);
            Printf( "%s  MTSWDMX_STRM_TYPE_AUD u1DmxMMInfoIdx = %d, swdmx id = %d\n", 
                __FUNCTION__, 
                u1DmxIdx,
                _arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
            _arDmxMMInfo[u1DmxIdx].fgAudIsConn = TRUE;
            _arDmxMMInfo[u1DmxIdx].fgAudNotSupport = FALSE;
            // Todo MW_AUD_SetSwdmxSrcId(_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
        }
        else
        {
            LOG(3, "unknown u2CompId %d\n", pt_comp_id->ui2_id);
        }
        break;
    default:
        LOG(3, "unknown e_type %d", ptDestInfo->e_type);
        i4Ret = RMR_DRV_INV_SET_INFO;
        break;
    }

    UNUSED(pt_comp_id);

    return i4Ret;
}

//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetPSIBlockMode
 *  Event handler of setting TS blocking mode with PSI buffer check
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetPSIBlockMode(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT32 u4Enable=0;
    UINT8 u1DmxMMInfoIdx = 0;
    u4Enable = (UINT32)pv_set_info;
    u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    if(u4Enable)
    {
        // to check PSI buffer size, only send data if PSI buffer is not full
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_CHK_PSI_BUF_SIZE, (UINT32)TRUE, 0, 0);
    }
    else
    {
        // not to check PSI buffer size, send data only blocked by A/V buffer
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_CHK_PSI_BUF_SIZE, (UINT32)FALSE, 0, 0);
    }
    return RMR_OK;
}

#ifdef TIME_SHIFT_SUPPORT
//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetRecBufRange
 *  Event handler of setting TS time shift record buffer range
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetRecBufRange(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_REC_BUF_RANGE, (UINT32)pv_set_info, 0, 0);
    UNUSED(pt_comp_id);
    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetMonitorLBA
 *  Event handler of setting TS time shift monitor LBA
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *  @param  u1Pidx          PID index
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetMonitorLBA(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_MONITOF_LBA, (UINT32)pv_set_info, 0, 0);
    UNUSED(pt_comp_id);
    return RMR_OK;
}

//-----------------------------------------------------------------------------
/** _MwDmxMMResetScrambleCallback
 *  Reset scramble callback handler
 *
 *  @param  u1Pidx          PID index
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMResetScrambleCallback(UINT8 u1Pidx)
{
    Printf("_MwDmxMMResetScrambleCallback u1Pidx %d \n",u1Pidx);
    #if 0
    MTDMX_PID_T rPid = {0};
    #endif

    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    // dmx_freepid will clean registered scramble notify function
    // remove it later
    #if 0
    // Fixme
    if(MTDMX_MUL_SetPidxInstance(0, u1Pidx) != MTR_OK)
    {
        return FALSE;
    }

    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    if (MTDMX_SetFilePid(u1Pidx, MTDMX_PID_FLAG_SCRAMBLE_STATE, &rPid) != MTR_OK)
    {
        return FALSE;
    }
    #endif

    _MwDmxMMLock();
    _arScrambleState[u1Pidx].pf_demux_nfy = NULL;
    _arScrambleState[u1Pidx].pv_tag = NULL;
    _MwDmxMMUnlock();

    return TRUE;
}
#endif

static INT32 _MwDmxOnMMSetFileSizeUpdate(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_FILE_SZ_UPDATE, (UINT32)pv_set_info, 0, 0);
    UNUSED(pt_comp_id);
    return RMR_OK;
}

//-----------------------------------------------------------------------------
/** _MwDmxMMLock
 *  Enter demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MwDmxMMLock(void)
{
    _rDmxLock = x_crit_start();

    ASSERT(!_fgDmxLocking);
    _fgDmxLocking = TRUE;

    UNUSED(_fgDmxLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** _MwDmxMMUnlock
 *  Leave demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MwDmxMMUnlock(void)
{
    ASSERT(_fgDmxLocking);
    _fgDmxLocking = FALSE;

    x_crit_end(_rDmxLock);

    UNUSED(_fgDmxLocking);          // Make Lint happy
}

//-----------------------------------------------------------------------------
/** _MwDmxOnMMScrambleStateNotify
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
static BOOL _MwDmxOnMMScrambleStateNotify(UINT8 u1Pidx, UINT32 u4Data,
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

    _MwDmxMMLock();
    rInfo = _arScrambleState[u1Pidx];
    _MwDmxMMUnlock();

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
         Printf(" _MwDmxOnMMScrambleStateNotify notify scamble states clear\n");
        break;

    case MTDMX_SCRAMBLE_STATE_SCRAMBLED:
         eMwState = DEMUX_SCRS_SCRAMBLED;
         Printf(" _MwDmxOnMMScrambleStateNotify notify scamble states scramble\n");
        break;

    default:
        eMwState = DEMUX_SCRS_UNKNOWN;
        break;
    }

#ifndef LINUX_TURNKEY_SOLUTION
    fgRet = rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_SCRS, (UINT32)eMwState, 0, 0
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        );
#else
    fgRet = _MwDmx_set_dmx_nfy(u1Pidx,
        rInfo.pv_tag, DEMUX_COND_SCRS, (UINT32)eMwState, 0
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        );
#endif

    // The middleware guarantees that it will always return FALSE
    VERIFY(!fgRet);

    UNUSED(pvTag);

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MwDmxOnMMScrambleStateHandler
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
static BOOL _MwDmxOnMMScrambleStateHandler(UINT8 u1Pidx, MTDMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvTag)
{
    BOOL fgRet = FALSE;

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    switch (eCode)
    {
    case MTDMX_NOTIFY_CODE_SCRAMBLE_STATE:
        fgRet = _MwDmxOnMMScrambleStateNotify(u1Pidx, u4Data, pvTag);
        break;

    default:
        // Unknown notification code
        LOG(1, "Pidx %u: Unknown notification code %u\n", u1Pidx,
            (UINT32)eCode);
        ASSERT(0);
        break;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetScrambleCallback
 *  Set scramble notify callback
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetScrambleCallback(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    INT8 u1Pidx;
    UINT16 u2DecType;
    //DMX_PID_T rPid;
    DEMUX_NFY_INFO_T* prNfyInfo;
    UINT8 u1DmxMMInfoIdx =0;

    //UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);

    //DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);


    if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
    {
        u2DecType = _rDecInfo.u2DecType;
    }
    else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
    {
        u2DecType = _rDecInfo1.u2DecType;
    }
    else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
    {
        u2DecType = _rDecInfo2.u2DecType;
    }
    // 0831
    else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
    {
        u2DecType = _rDecInfo3.u2DecType;
    }
    else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
    {
        u2DecType = _rDecInfo4.u2DecType;
    }    
    else
    {
        LOG(3, "unknown u2CompId %d\n", pt_comp_id->ui2_id);
        return RMR_DRV_INV_SET_INFO;
    }
    if ((_rDecInfo.u2CompId > 0) &&
        ((_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID)||
         (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD)||
         (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD2) ||
         (_rDecInfo.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))&&
        (_rDecInfo.u2CompId == pt_comp_id->ui2_id))
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
        DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
    }
    else if ((_rDecInfo1.u2CompId > 0) &&
        ((_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID)||
         (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD)||
         (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD2)||
         (_rDecInfo1.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))&&
        (_rDecInfo1.u2CompId == pt_comp_id->ui2_id))
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo1.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
        DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
    }
    else if ((_rDecInfo2.u2CompId > 0) &&
        ((_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID)||
         (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD)||
         (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD2)||
         (_rDecInfo2.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))&&
        (_rDecInfo2.u2CompId == pt_comp_id->ui2_id))
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo2.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
        DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
    }
    else if ((_rDecInfo3.u2CompId > 0) &&
        ((_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID)||
         (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD)||
         (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD2)||
         (_rDecInfo3.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))&&
        (_rDecInfo3.u2CompId == pt_comp_id->ui2_id))
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo3.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
        DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
    }    
    else if ((_rDecInfo4.u2CompId > 0) &&
        ((_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_VID)||
         (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD)||
         (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD2)||
         (_rDecInfo4.u2DecType == (UINT16)MTSWDMX_STRM_TYPE_AUD3))&&
        (_rDecInfo4.u2CompId == pt_comp_id->ui2_id))
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(_rDecInfo4.u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
        DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
    }
    else
    {
        LOG(3, "unknown u2CompId %d\n", pt_comp_id->ui2_id);
        return RMR_DRV_INV_SET_INFO;

    }
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    if ((UINT16)MTSWDMX_STRM_TYPE_VID == u2DecType)
    {
        u1Pidx = (UINT8)(DMX_PES_COMP_ID_START);
        // try to get the active pidx in swdmx.
        // however, MW is not open stream yet.
        // the pidx in swdmx is not active.
        // we have to refine it later.
        #if 0
        if(MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_GET_VIDEO_PIDX, &u4Tmp, 0, 0))
        {
            if(u4Tmp < DMX_NUM_PID_INDEX)
            {
                u1Pidx = (UINT8)u4Tmp;
            }
        }
        #endif
        if(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_SET_VID_SCRAMBLE_NFY,
            (UINT32)_MwDmxOnMMScrambleStateHandler, u1Pidx, 0) !=MTR_OK)
        {
            LOG(3,"_MwDmxOnMMSetMediaFormat eSWDMX_PID_INDEX_START fail\n");
        }
    }
    else if ((UINT16)MTSWDMX_STRM_TYPE_AUD == u2DecType)
    {
        u1Pidx = (UINT8)(DMX_PES_COMP_ID_START + 1);
        #if 0
        if(MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_GET_AUDIO_PIDX, &u4Tmp, 0, 0))
        {
            if(u4Tmp < DMX_NUM_PID_INDEX)
            {
                u1Pidx = (UINT8)u4Tmp;
            }
        }
        #endif
        if(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_SET_AUD_SCRAMBLE_NFY,
            (UINT32)_MwDmxOnMMScrambleStateHandler, u1Pidx, 0) !=MTR_OK)
        {
            LOG(3,"_MwDmxOnMMSetMediaFormat eSWDMX_PID_INDEX_START fail\n");
        }
    }
    else if ((UINT16)MTSWDMX_STRM_TYPE_AUD2 == u2DecType)
    {
        u1Pidx = (UINT8)(DMX_PES_COMP_ID_START + 2);
        if(!MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            (UINT8)MTSWDMX_SET_AUD_SCRAMBLE_NFY,
            (UINT32)_MwDmxOnMMScrambleStateHandler, 0, 0))
        {
            LOG(3,"_MwDmxOnMMSetMediaFormat eSWDMX_PID_INDEX_START fail\n");
        }        
    }
    else if ((UINT16)MTSWDMX_STRM_TYPE_AUD3 == u2DecType)
    {
        u1Pidx = (UINT8)(DMX_PES_COMP_ID_START + 3);
    }    
    else
    {
        LOG(3, "unknown u2DecType %d\n", (UINT32)u2DecType);
        return RMR_DRV_INV_SET_INFO;
    }

    // Set scramble state callback function

    if ((u1Pidx >= DMX_NUM_PID_INDEX) || (pv_set_info == NULL))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    prNfyInfo = (DEMUX_NFY_INFO_T*)pv_set_info;

 /*
    // Fixme
    if(!DMX_MUL_SetPidxInstance(0, u1Pidx))
    {
        return FALSE;
    }

    // Set PSI callback function to our handler
    x_memset(&rPid, 0, sizeof(rPid));
    rPid.pfnScramble = _MwDmxOnMMScrambleStateHandler;
    rPid.pvScrambleTag = NULL;
    if (!DMX_SetFilePid(u1Pidx, DMX_PID_FLAG_SCRAMBLE_STATE, &rPid))
    {
        return RMR_DRV_INV_SET_INFO;
    }
*/
    _MwDmxMMLock();
    _arScrambleState[u1Pidx].pf_demux_nfy = prNfyInfo->pf_demux_nfy;
    _arScrambleState[u1Pidx].pv_tag = prNfyInfo->pv_tag;
    _MwDmxMMUnlock();

    return RMR_OK;
}


 /*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Registration
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
// For subtitle usage
BOOL _MwDmxMM_SetSubtitleStrmID(UINT32 u4StrmID, UINT16 u2ConnectSrcCompId)
{
    UINT8 u1DmxMMInfoIdx = 0;//_MwDmxFindDmxIdx(u2ConnectSrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, FALSE);

    if(MTSWDMX_SetStrmID(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, u4StrmID, 0, MTSWDMX_STRM_TYPE_SUBTITLE) == MTR_OK)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}


 /*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Registration
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
BOOL _MwDmxMM_EnableSubtitle(UINT16 u2ConnectSrcCompId)
{
    UINT8 u1DmxMMInfoIdx = 0;//_MwDmxFindDmxIdx(u2ConnectSrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, FALSE);

   if( MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_ENABLE_STREAM, MTSWDMX_STRM_TYPE_SUBTITLE, 0, 0) == MTR_OK)
   {
       return TRUE;
   }
   else
   {
       return FALSE;
   }

   
}


 /*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Registration
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
BOOL _MwDmxMM_DisableSubtitle(UINT16 u2ConnectSrcCompId)
{
    UINT8 u1DmxMMInfoIdx = 0;//_MwDmxFindDmxIdx(u2ConnectSrcCompId, DMX_MM_SEARCH_BY_SRC_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, FALSE);

    if(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_DISABLE_STREAM, MTSWDMX_STRM_TYPE_SUBTITLE, 0, 0) == MTR_OK)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#ifdef  TS_MARLIN_SUPPORT
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

static INT32 _MwDmxOnMMSetDescryptInfoMarlinTS(DRV_COMP_ID_T* pt_comp_id, const VOID* pv_set_info)
{
    MM_DESC_CRYPT_INFO_T*       prDesInfo;
    MM_CRYPT_INFO_T*            ptEncryptInfo;
    MTDMX_DESC_MODE_T           eMode;
    MW_DMX_CRYPT_KEY_UNION_T    uCryptCurrentKey;
    MW_DMX_CRYPT_KEY_UNION_T    uCryptCurrentIv;
    MW_DMX_CRYPT_KEY_UNION_T    uCryptNextKey;
    MW_DMX_CRYPT_KEY_UNION_T    uCryptNextIv;
    MW_DMX_CRYPT_KEY_INFO_T     rKeyInfoCurrent;
    MW_DMX_CRYPT_KEY_INFO_T     rIvInfo;
    UINT8                       u1_i           = 0;
    UINT8                       u1KeyIdxCur    = 0;
    UINT8                       u1DmxMMInfoIdx = 0;
    BOOL                        b_even;

    prDesInfo = (MM_DESC_CRYPT_INFO_T*)pv_set_info;
    if(prDesInfo == NULL)
    {
        return RMR_INV_ARG;
    }

    ptEncryptInfo = (MM_CRYPT_INFO_T*)&(prDesInfo->t_crypto_info);
    if(ptEncryptInfo == NULL)
    {
        return RMR_INV_ARG;
    }
    if(ptEncryptInfo->u.t_ml_ecm_crypt.b_is_even == TRUE)
    {
        b_even = FALSE;
    }
    else
    {
        b_even = TRUE;
    }

    //set current key and IV , encryption algorithm : 128 bit AES CBC , residual termination block process : SCTE52
    x_memset((void*)&uCryptCurrentKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    x_memset((void*)&uCryptCurrentIv, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    DataSwap((UINT32*)uCryptCurrentKey.rAESKeySet.au4Key,(UINT32*)ptEncryptInfo->u.t_ml_ecm_crypt.aui1_current_key,
        DMX_MM_MARLIN_AES_KEY_LEN/4 ,DMX_MM_MARLIN_AES_KEY_SWAP_MODE);
    DataSwap((UINT32*)uCryptCurrentIv.rAESKeySet.au4Iv,(UINT32*)ptEncryptInfo->u.t_ml_ecm_crypt.aui1_current_iv,
        DMX_MM_MARLIN_AES_KEY_LEN/4 ,DMX_MM_MARLIN_AES_KEY_SWAP_MODE);
    rKeyInfoCurrent.fgEven = ptEncryptInfo->u.t_ml_ecm_crypt.b_is_even;
    rKeyInfoCurrent.eDescAlg = DESC_ALG_AES_128_CBC;
    rKeyInfoCurrent.puCryptKey = &uCryptCurrentKey;
    u1KeyIdxCur = MW_DMX_CRYPT_INVALID_KEYX;

    rIvInfo.fgEven = ptEncryptInfo->u.t_ml_ecm_crypt.b_is_even;
    rIvInfo.eDescAlg = DESC_ALG_AES_128_CBC;
    rIvInfo.puCryptKey = &uCryptCurrentIv;
    if (!_MW_DMX_CRYPT_AllocateMarlinTSKeySlot(MWDMX_CRYPTKEY_FLAG_MM_PLAY,pt_comp_id->ui2_id,&u1KeyIdxCur))
    {
        return RMR_INV_ARG;
    }
    MTDMX_SetAesEvenOddIVEx(u1KeyIdxCur, rIvInfo.fgEven, rIvInfo.puCryptKey->rAESKeySet.au4Iv);
    MTDMX_SetAesKeyLenEx(u1KeyIdxCur, 128);
    if(MTDMX_SetAesKey(u1KeyIdxCur, rKeyInfoCurrent.fgEven, rKeyInfoCurrent.puCryptKey->rAESKeySet.au4Key) != MTR_OK)
    {
        return RMR_INV_ARG;
    }
    MTDMX_SetRtbMode(u1KeyIdxCur,MTDMX_DESC_RTB_MODE_SCTE52);

    //set current key and IV , encryption algorithm : 128 bit AES CBC , residual termination block process : SCTE52
    x_memset((void*)&uCryptNextKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    x_memset((void*)&uCryptNextIv, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    DataSwap((UINT32*)uCryptNextKey.rAESKeySet.au4Key,(UINT32*)ptEncryptInfo->u.t_ml_ecm_crypt.aui1_next_key,
        DMX_MM_MARLIN_AES_KEY_LEN/4 ,DMX_MM_MARLIN_AES_KEY_SWAP_MODE);
    DataSwap((UINT32*)uCryptNextIv.rAESKeySet.au4Iv,(UINT32*)ptEncryptInfo->u.t_ml_ecm_crypt.aui1_next_iv,
        DMX_MM_MARLIN_AES_KEY_LEN/4 ,DMX_MM_MARLIN_AES_KEY_SWAP_MODE);
    rKeyInfoCurrent.fgEven = b_even;
    rKeyInfoCurrent.eDescAlg = DESC_ALG_AES_128_CBC;
    rKeyInfoCurrent.puCryptKey = &uCryptNextKey;
    u1KeyIdxCur = MW_DMX_CRYPT_INVALID_KEYX;

    if (!_MW_DMX_CRYPT_AllocateMarlinTSKeySlot(MWDMX_CRYPTKEY_FLAG_MM_PLAY,pt_comp_id->ui2_id,&u1KeyIdxCur))
    {
        return RMR_INV_ARG;
    }
    rIvInfo.fgEven = b_even;
    rIvInfo.eDescAlg = DESC_ALG_AES_128_CBC;
    rIvInfo.puCryptKey = &uCryptNextIv;

    MTDMX_SetAesEvenOddIVEx(u1KeyIdxCur, rIvInfo.fgEven, rIvInfo.puCryptKey->rAESKeySet.au4Iv);
    MTDMX_SetAesKeyLenEx(u1KeyIdxCur, 128);
    if(MTDMX_SetAesKey(u1KeyIdxCur, rKeyInfoCurrent.fgEven, rKeyInfoCurrent.puCryptKey->rAESKeySet.au4Key) != MTR_OK)
    {
        return RMR_INV_ARG;
    }
    MTDMX_SetRtbMode(u1KeyIdxCur,MTDMX_DESC_RTB_MODE_SCTE52);
    eMode = DMX_DESC_MODE_AES_CBC;
    for(u1_i=0; u1_i<DMX_MM_MAX_MARLIN_TS_KEY_INDEX; u1_i++)
    {
        if(!_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[u1_i].fgValid)
        {
             _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[u1_i].fgValid = TRUE;
             _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[u1_i].u1KeyIdx = u1KeyIdxCur;
             _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[u1_i].eDescMode = eMode;
        }
        else
        {

        }
    }
    //UNUSED(MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,(UINT8)MTSWDMX_SET_EVEN_ODD_KEY, (UINT32)ptEncryptInfo->u.t_ml_ecm_crypt.b_is_even, 0, 0));
    return RMR_OK;
}
#endif

#ifdef TIME_SHIFT_SUPPORT
/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Set description infomation
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxOnMMSetDescryptInfo(DRV_COMP_ID_T* pt_comp_id, const VOID* pv_set_info)
{
    MM_DESC_CRYPT_INFO_T *prDesInfo;
    MM_CRYPT_INFO_T *ptEncryptInfo;
    MTDMX_DESC_MODE_T eMode;
    MW_DMX_CRYPT_KEY_UNION_T uCryptKey;
    MW_DMX_CRYPT_KEY_INFO_T rKeyInfo;
    UINT32 au4EvenKey[6], au4OddKey[6];
    UINT8 i, u1KeyIdx;
    MTDMX_CRYPTO_KEY_TYPE_T eKeyType;
    // Jack, fixme
    UINT8 u1DmxMMInfoIdx = 0; //_MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    UINT32 eDescAlg = DESC_ALG_AES_128_ECB;
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    prDesInfo = (MM_DESC_CRYPT_INFO_T*)pv_set_info;
    if(prDesInfo == NULL)
    {
        return RMR_INV_ARG;
    }

    ptEncryptInfo = (MM_CRYPT_INFO_T*)&(prDesInfo->t_crypto_info);
    if(ptEncryptInfo == NULL)
    {
        return RMR_INV_ARG;
    }

	Printf("_MwDmxOnMMSetDescryptInfo e_mm_crypt_type %d\n",ptEncryptInfo->e_mm_crypt_type);
    
    if (ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_RSV)
    {
        if (RESEVED_KEY_LENGTH_MAX < MTDRM_HDCP2_KEYLEN_RIV)
        {
            Printf ("[Err]: Set HDCP key Error.\n");
            return RMR_INV_ARG;
        }

        _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.fgValid = TRUE;
        x_memcpy ( &_arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo,
                   prDesInfo, sizeof(MM_DESC_CRYPT_INFO_T) );
                   
        MTSWDMX_SetHdcpKey(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                           (UINT32)&_arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved);
                           
        Printf ("[Info]: Get HDCP Key 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[0],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[1],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[2],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[3],         
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[4],            
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[5],      
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[6],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[7]      
            );        
        return RMR_OK;
    }
#ifdef  TS_MARLIN_SUPPORT
    if(ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_TSMLN)
    {
        _MwDmxOnMMSetDescryptInfoMarlinTS(pt_comp_id,pv_set_info);
        return RMR_OK;
    }
#endif
    if(ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_MLN || 
           ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_PDCF ||
           ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_CENC)
    {
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_MM_DECRYPT, (UINT32)ptEncryptInfo, 0, 0);
        return RMR_OK;
    }

    switch(ptEncryptInfo->u.t_pvr_crypt.e_mode)
    {
       case MM_CRYPT_MODE_AES_ECB:
            eDescAlg = DESC_ALG_AES_128_ECB;
            break;
       case MM_CRYPT_MODE_AES_CBC:
            eDescAlg = DESC_ALG_AES_128_CBC;
            break;
       case MM_CRYPT_MODE_3_DES_EBC:
            eDescAlg = DESC_ALG_3DES_ECB;
            break;
       case MM_CRYPT_MODE_3_DES_CBC:
            eDescAlg = DESC_ALG_3DES_CBC;
            break;
       case MM_CRYPT_MODE_DES_EBC:      
            eDescAlg = DESC_ALG_DES_ECB;
            break;
       case MM_CRYPT_MODE_DES_CBC:      
            eDescAlg = DESC_ALG_DES_CBC;
            break;
       default:
            break;
    }    

    x_memset((void*)au4OddKey, 0, sizeof(au4OddKey));
    x_memset((void*)au4EvenKey, 0, sizeof(au4EvenKey));

    au4OddKey[0] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_odd_key_1[0];
    au4OddKey[1] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_odd_key_1[1];
    au4EvenKey[0] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_even_key_1[0];
    au4EvenKey[1] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_even_key_1[1];

    if (ptEncryptInfo->u.t_pvr_crypt.e_mode == MM_CRYPT_MODE_AES_ECB ||ptEncryptInfo->u.t_pvr_crypt.e_mode ==  MM_CRYPT_MODE_AES_CBC)
    {
        au4OddKey[2] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_odd_key_2[0];
        au4OddKey[3] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_odd_key_2[1];
        au4EvenKey[2] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_even_key_2[0];
        au4EvenKey[3] = prDesInfo->t_crypto_info.u.t_pvr_crypt.aui4_even_key_2[1];
    }
      
    //backup key type
    switch(ptEncryptInfo->e_mm_crypt_type)
    {
        case MM_CRYPT_TYPE_PVR:
            default:
            eKeyType = MTDMX_CRYPTO_KEY_SECURE_PVR_AES_0;
        break;
    }

    x_memset((void*)&uCryptKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    x_memcpy(uCryptKey.rAESKeySet.au4Key, au4OddKey, 16);
        
    rKeyInfo.fgEven = FALSE;
    rKeyInfo.puCryptKey = &uCryptKey;
    u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX;
    rKeyInfo.eDescAlg = eDescAlg;

    if (!_MW_DMX_CRYPT_AllocateCAKeySlot(&rKeyInfo, MWDMX_CRYPTKEY_FLAG_MM_PLAY, 0, &u1KeyIdx))
    {
        return RMR_INV_ARG;
    }

    if(prDesInfo->t_crypto_info.u.t_pvr_crypt.b_is_encrypt)
    {
        if(MTR_OK != MTDMX_SetCryptoKey(u1KeyIdx, FALSE, eKeyType, (const UINT8 *)au4OddKey, sizeof(au4OddKey)))
        {
            return RMR_INV_ARG;
        }
    }
    else
    {
        if(MTDMX_SetAesKey(u1KeyIdx, FALSE, au4OddKey) != MTR_OK)
        {
            return RMR_INV_ARG;
        }
    }
    
    x_memset((void*)&uCryptKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));
    x_memcpy(uCryptKey.rAESKeySet.au4Key, au4EvenKey, 16);
    
    rKeyInfo.fgEven = TRUE;
    rKeyInfo.puCryptKey = &uCryptKey;
    u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX;
    rKeyInfo.eDescAlg = eDescAlg;

    if (!_MW_DMX_CRYPT_AllocateCAKeySlot(&rKeyInfo, MWDMX_CRYPTKEY_FLAG_MM_PLAY, 0, &u1KeyIdx))
    {
        return RMR_INV_ARG;
    }

    if(prDesInfo->t_crypto_info.u.t_pvr_crypt.b_is_encrypt)
    {
        if(MTR_OK != MTDMX_SetCryptoKey(u1KeyIdx, TRUE, eKeyType, (const UINT8 *)au4EvenKey, sizeof(au4EvenKey)))
        {
            return RMR_INV_ARG;
        }
    }
    else
    {
        if(MTDMX_SetAesKey(u1KeyIdx, TRUE, au4EvenKey) != MTR_OK)
        {
           return RMR_INV_ARG;
        }
    }

    // Backup key
    switch(prDesInfo->t_crypto_info.u.t_pvr_crypt.e_mode)
    {
    case MM_CRYPT_MODE_DES_EBC:
        eMode = MTDMX_DESC_MODE_DES_ECB;
        break;

    case MM_CRYPT_MODE_DES_CBC:
        eMode = MTDMX_DESC_MODE_DES_CBC;
        break;

    case MM_CRYPT_MODE_3_DES_EBC:
        eMode = MTDMX_DESC_MODE_3DES_ECB;
        break;

    case MM_CRYPT_MODE_3_DES_CBC:
        eMode = MTDMX_DESC_MODE_3DES_CBC;
        break;

    case MM_CRYPT_MODE_AES_ECB:
        eMode = MTDMX_DESC_MODE_AES_ECB;
        break;

    case MM_CRYPT_MODE_AES_CBC:
        eMode = MTDMX_DESC_MODE_AES_CBC;
        break;

    default:
        return RMR_INV_ARG;
    }

    // Backward compatible for key storage
    //for(i=0; i<DMX_NUM_PID_INDEX; i++)
    for(i=0; i<DMX_MAX_NUM_PID_INDEX_TABLE; i++)
    {
        if(_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].fgValid &&
          (prDesInfo->t_stream_setting.u.t_stm_ts_id == _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u2StreamID))
        {
            if((_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u1KeyIdx == u1KeyIdx) && (_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].eDescMode == eMode))
            {
                return RMR_OK;
            }
            else
            {
                LOG(1, "%s, Key index not match, %d, %d\n", __FUNCTION__,
                                               _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u1KeyIdx, u1KeyIdx);
                return RMR_INV_ARG;
            }
        }
    }

    //for(i=0; i<DMX_NUM_PID_INDEX; i++)
    for(i=0; i<DMX_MAX_NUM_PID_INDEX_TABLE; i++)
    {
        if(!_arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].fgValid)
        {
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].fgValid = TRUE;
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u1KeyIdx = u1KeyIdx;
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u2StreamID = prDesInfo->t_stream_setting.u.t_stm_ts_id;
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].eDescMode = eMode;
            break;
        }
    }

    if(i == DMX_NUM_PID_INDEX)
    {
        LOG(3, "%s, No valid info array\n", __FUNCTION__);
        return RMR_INV_ARG;
    }

    return RMR_OK;
}


/*-----------------------------------------------------------------------------
 * Name:
 * Description: Run crypt command
 * Inputs:
 * Outputs:
 * Returns:
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxOnMMSetCryptCmd(DRV_COMP_ID_T* pt_comp_id, const VOID* pv_set_info)
{
    MM_CRYPT_CMD_T *prCryptCmd;
    INT32 i4Ret = RMR_OK;
    UINT8 i;
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    prCryptCmd = (MM_CRYPT_CMD_T*)pv_set_info;
    if(prCryptCmd == NULL)
    {
        return RMR_INV_ARG;
    }

    switch(prCryptCmd->t_crypt_ctrl_cmd)
    {
     case MM_CRYPT_CTRL_CMD_RESET_ALL:
        if(!_MW_DMX_CRYPT_ResetAllCAKeySlots(TRUE))
        {
            i4Ret = RMR_INV_ARG;
        }

        //for(i=0; i<DMX_NUM_PID_INDEX; i++)
        for(i=0; i<DMX_MAX_NUM_PID_INDEX_TABLE; i++)
        {
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].fgValid = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].arDmxMMKeyInfo[i].u1KeyIdx = 0xFF;
        }
        break;
     default:
         break;
    }

    return i4Ret;
}
#else

#ifdef ENABLE_MULTIMEDIA
/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Set description infomation
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxOnMMSetDescryptInfo(DRV_COMP_ID_T* pt_comp_id, const VOID* pv_set_info)
{
    MM_DESC_CRYPT_INFO_T *prDesInfo;
    MM_CRYPT_INFO_T *ptEncryptInfo;
    UINT8 u1DmxMMInfoIdx = 0;

    //u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    prDesInfo = (MM_DESC_CRYPT_INFO_T*)pv_set_info;
    if(prDesInfo == NULL)
    {
        return RMR_INV_ARG;
    }

    ptEncryptInfo = (MM_CRYPT_INFO_T*)&(prDesInfo->t_crypto_info);
    if(ptEncryptInfo == NULL)
    {
        return RMR_INV_ARG;
    }

#ifdef  TS_MARLIN_SUPPORT
    if(ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_TSMLN)
    {
        _MwDmxOnMMSetDescryptInfoMarlinTS(pt_comp_id,pv_set_info);
        return RMR_OK;
    }
#endif

    if(ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_MLN || 
       ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_PDCF ||
       ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_CENC)
    {
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_MM_DECRYPT, (UINT32)ptEncryptInfo, 0, 0);
        return RMR_OK;
    }

    if (ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_RSV)
    {
        if (RESEVED_KEY_LENGTH_MAX < MTDRM_HDCP2_KEYLEN_RIV)
        {
            Printf ("[Err]: Set HDCP key Error.\n");
            return RMR_INV_ARG;
        }

        _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.fgValid = TRUE;
        x_memcpy ( &_arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo,
                   prDesInfo, sizeof(MM_DESC_CRYPT_INFO_T) );

        MTSWDMX_SetHdcpKey(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
                           (UINT32)&_arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved);
        
        Printf ("[Info]: Get HDCP Key 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[0],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[1],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[2],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[3],         
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[4],            
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[5],      
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[6],
            _arDmxMMInfo[u1DmxMMInfoIdx].rDescryptInfo.rKeyInfo.t_crypto_info.u.aui1Reserved[7]      
            );
        
        return RMR_OK;
    }
    else
    {
        LOG(0, "[Info]: Not used yet!\n");
        
        return RMR_INV_ARG;        
    }
    
    return RMR_INV_ARG;
}
#endif //ENABLE_MULTIMEDIA

#endif  // TIME_SHIFT_SUPPORT

//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetStcId
 *  Set Stc Id to Swdmx/Vdec/Adec/Vdp
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Stc Id
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetStcId(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    _arDmxMMInfo[u1DmxMMInfoIdx].u1StcId = (UINT32)pv_set_info;
    UNUSED(pt_comp_id);
    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetFreeRun
 *  Set Stc Id to Swdmx/Vdec/Adec/Vdp
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Bool for free run or not
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetFreeRun(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    _arDmxMMInfo[u1DmxMMInfoIdx].fgFreeRun = (BOOL)(UINT32)pv_set_info;
    UNUSED(pt_comp_id);
    return RMR_OK;
}

static INT32 _MwDmxOnMMSetFlushFeederInfo(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    u1DmxMMInfoIdx=_MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    if (NULL != pv_set_info)
    {
        //Printf("_MwDmxOnMMSetFlushFeederInfo ApSeekPts is %d ms\n", *((UINT32*)pv_set_info));
        MTSWDMX_SetFlushFeederInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 0, (UINT32)pv_set_info, 0, 0);
    }
    else 
    {
        //Printf("_MwDmxOnMMSetFlushFeederInfo pv_set_info is 0\n");
        MTSWDMX_SetFlushFeederInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 0, 0, 0, 0);
    }

    return RMR_OK;
}

static INT32 _MwDmxOnMMSetGnrcStreamInfo(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    u1DmxMMInfoIdx=_MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetGnrcStreamInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT32)pv_set_info);

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetHdMvcInfo
 *  Set HdMvc Info to Swdmx
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Bool for free run or not
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMSetHdMvcInfo(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    BOOL fgEnable = 0;
    UINT8 u1DmxMMInfoIdx = 0;
    fgEnable = (pv_set_info)? TRUE : FALSE;
    u1DmxMMInfoIdx=_MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
        (UINT8)MTSWDMX_SET_HDMVC_INFO, (UINT32)fgEnable, 0, 0);

    return RMR_OK;
}

static INT32 _MwDmxOnMMSetDuration(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT64 ui8_duration = *(UINT64 *)pv_set_info;
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);

    MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
        (UINT8)MTSWDMX_SET_DURATION, (UINT32)ui8_duration, 0, 0);

    return RMR_OK;
}

static INT32 _MwDmxOnMMSetPushChangeAudio(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
     UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
     DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_SET_INFO);
     MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, 
        (UINT8)MTSWDMX_SET_TYPE_PUSH_CHANGE_AUDIO, 0, 0, 0);
     return RMR_OK;
}

/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Notes: record and set something on hardware(src, dest, type...)
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxMM_Connect(
              const DRV_COMP_ID_T*    pt_comp_id,
              DRV_CONN_TYPE_T         e_conn_type,
              const VOID*             pv_conn_info,
              SIZE_T                  z_conn_info_len,
              VOID*                   pv_tag,
              x_rm_nfy_fct            pf_nfy)
{
    DRV_COMP_ID_T* prCompReg;
    UINT8 u1DmxMMInfoIdx;

    _MwDmxConnectLock();

    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
        goto _ErrExit;
    }

    // 1. Check para valid
    if(pt_comp_id->e_type == DRVT_DEMUX_CONTAINER_ES)
    {
        if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(0, "pt_comp_id %d already connected 0\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }
        else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(0, "pt_comp_id %d already connected 1\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }
        else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(0, "pt_comp_id %d already connected 2\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }
        // 0831
        // here may needed add new entry points
        else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(0, "pt_comp_id %d already connected 2\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }
        else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(0, "pt_comp_id %d already connected 2\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }        
        else
        {
            if (_rDecInfo.u2CompId == 0)
            {
                prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
                // record its own component id
                _rDecInfo.u2CompId = pt_comp_id->ui2_id;

                // record the buffer agent id and we can use it
                // to recognize which instance this es control belongs
                _rDecInfo.u2SrcCompId = prCompReg->ui2_id;
                Printf( "_rDecInfo: pt_comp_id %d connected\n", (UINT32)pt_comp_id->ui2_id);
            }
            else if (_rDecInfo1.u2CompId == 0)
            {
                prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
                // record its own component id
                _rDecInfo1.u2CompId = pt_comp_id->ui2_id;

                // record the buffer agent id and we can use it
                // to recognize which instance this es control belongs
                _rDecInfo1.u2SrcCompId = prCompReg->ui2_id;
                Printf( "_rDecInfo1: pt_comp_id %d connected\n", (UINT32)pt_comp_id->ui2_id);
            }
            else if (_rDecInfo2.u2CompId == 0)
            {
                prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
                // record its own component id
                _rDecInfo2.u2CompId = pt_comp_id->ui2_id;

                // record the buffer agent id and we can use it
                // to recognize which instance this es control belongs
                _rDecInfo2.u2SrcCompId = prCompReg->ui2_id;
                Printf( "_rDecInfo2: pt_comp_id %d connected\n", (UINT32)pt_comp_id->ui2_id);
            }
            else if (_rDecInfo3.u2CompId == 0)
            {
                prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
                // record its own component id
                _rDecInfo3.u2CompId = pt_comp_id->ui2_id;

                // record the buffer agent id and we can use it
                // to recognize which instance this es control belongs
                _rDecInfo3.u2SrcCompId = prCompReg->ui2_id;
                Printf( "_rDecInfo3: pt_comp_id %d connected\n", (UINT32)pt_comp_id->ui2_id);
            }     
            else if (_rDecInfo4.u2CompId == 0)
            {
                prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
                // record its own component id
                _rDecInfo4.u2CompId = pt_comp_id->ui2_id;

                // record the buffer agent id and we can use it
                // to recognize which instance this es control belongs
                _rDecInfo4.u2SrcCompId = prCompReg->ui2_id;
                Printf( "_rDecInfo4: pt_comp_id %d connected\n", (UINT32)pt_comp_id->ui2_id);
            }             

            _MwDmxConnectUnlock();
            return RMR_OK;
        }
    }

    // 1.2 check id range
    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
        goto _ErrExit;
    }


    u1DmxMMInfoIdx = _MwDmxFindAvailDmxIdx();
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_CONN_INFO);
    _arDmxMMInfo[u1DmxMMInfoIdx].fgAvailable = FALSE;
    _arDmxMMInfo[u1DmxMMInfoIdx].u2DmxCompId = pt_comp_id->ui2_id;

    if (MTSWDMX_LockValidSrc(u1DmxMMInfoIdx) != MTR_OK)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId = u1DmxMMInfoIdx;
    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
    buf_agt_dmx_connect((BAGT_COMP_ID_T)prCompReg->ui2_id);
    _arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId = (BAGT_COMP_ID_T)prCompReg->ui2_id;

    #ifdef CC_DMX_CONN_TYPE_SUPPORT
    if((UINT32)((UINT32)DMX_CONN_TYPE_BUFAGENT_0 + _arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId)
        < (UINT32)(DMX_CONN_TYPE_BUFAGENT_MAX))
    {
        MTDMX_MUL_GetAvailableInst(
            (DMX_CONN_TYPE_T)((UINT32)DMX_CONN_TYPE_BUFAGENT_0 +
            _arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId), &_arDmxMMInfo[u1DmxMMInfoIdx].u1DmxId);
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_DMX_ID,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1DmxId, 0, 0);
    }
    else
    {
        LOG(1, "%s u2BagtCompId = %d >= %d !!!!!!\n",
            __FUNCTION__,
            _arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            BAGT_COMP_NS);
    }
    #endif

    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    if (u1DmxMMInfoIdx == MTSWDMX_PRIMARY_SOURCE)
    {
        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId, FEEDER_PROGRAM_SOURCE, _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_SRC_TYPE, FEEDER_PROGRAM_SOURCE, 0, 0);
        LOG(5, "Set buffer agent to program source\n");

        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
    }
    else if (u1DmxMMInfoIdx == MTSWDMX_SECONDARY_SOURCE)
    {
        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            FEEDER_AUDIO_SOURCE,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_SRC_TYPE,
            FEEDER_AUDIO_SOURCE, 0, 0);
        LOG(5, "Set buffer agent to audio source\n");
    }
#ifndef CC_FBM_FOUR_INST_SUPPORT    
#ifdef CC_SKYPE_FINE_INSTANCE
    else if (u1DmxMMInfoIdx == MTSWDMX_THIRD_SOURCE)
    {
        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            FEEDER_PROGRAM_SOURCE_2,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_SRC_TYPE,
            FEEDER_PROGRAM_SOURCE_2, 0, 0);
        LOG(5, "Set buffer agent to the third source\n");
    }
    #endif
#else
    else if (u1DmxMMInfoIdx == MTSWDMX_THIRD_SOURCE)
    {
        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            FEEDER_SOURCE_3,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_SRC_TYPE,
            FEEDER_SOURCE_3, 0, 0);
        LOG(5, "Set buffer agent to the third source\n");
    }
    else if (u1DmxMMInfoIdx == MTSWDMX_4th_SOURCE)
    {
        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            FEEDER_SOURCE_4,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_SRC_TYPE,
            FEEDER_SOURCE_4, 0, 0);
        LOG(5, "Set buffer agent to the 4th source\n");
    }
    #ifdef CC_SKYPE_FINE_INSTANCE
    else if (u1DmxMMInfoIdx == MTSWDMX_5th_SOURCE)
    {
        LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
            __FUNCTION__,
            u1DmxMMInfoIdx,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        buf_agt_set_feedersrc(_arDmxMMInfo[u1DmxMMInfoIdx].u2BagtCompId,
            FEEDER_AUDIO_SOURCE_3,
            _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
        MTSWDMX_SetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId,
            MTSWDMX_SET_SRC_TYPE,
            FEEDER_AUDIO_SOURCE_3, 0, 0);
        LOG(5, "Set buffer agent to the 5th source\n");
    }
    #endif
#endif

    UNUSED(z_conn_info_len);
    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    _MwDmxConnectUnlock();
    return RMR_OK;

_ErrExit:
    _MwDmxConnectUnlock();
    return (RMR_DRV_INV_CONN_INFO);
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  just record, since it's hardwire
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxMM_Disconnect(
              const DRV_COMP_ID_T*    pt_comp_id,
              DRV_DISC_TYPE_T         e_disc_type,
              const VOID*             pv_disc_info,
              SIZE_T                  z_disc_info_len)
{
    INT8 u1Pidx;
    UINT16 u2DecType;
    UINT8 u1DmxMMInfoIdx;
    UINT16 u2BagtCompId;
    DRV_COMP_ID_T* prCompReg;

    _MwDmxConnectLock();

    // 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
        goto _ErrExit;
    }

    Printf("%s disconnect %d\n", __FUNCTION__, (UINT32)pt_comp_id->ui2_id);
    if(pt_comp_id->e_type == DRVT_DEMUX_CONTAINER_ES)
    {

        if (_rDecInfo.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(2, "_rDecInfo: %d disconnected\n", (UINT32)pt_comp_id->ui2_id);
            _rDecInfo.u2CompId = 0;
            u2BagtCompId = _rDecInfo.u2SrcCompId;
            u2DecType = _rDecInfo.u2DecType;
        }
        else if (_rDecInfo1.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(2, "_rDecInfo1: %d disconnected\n", (UINT32)pt_comp_id->ui2_id);
            _rDecInfo1.u2CompId = 0;
            u2BagtCompId = _rDecInfo1.u2SrcCompId;
            u2DecType = _rDecInfo1.u2DecType;
        }
        else if (_rDecInfo2.u2CompId == pt_comp_id->ui2_id)
        {
            LOG(2, "_rDecInfo2: %d disconnected\n", (UINT32)pt_comp_id->ui2_id);
            _rDecInfo2.u2CompId = 0;
            u2BagtCompId = _rDecInfo2.u2SrcCompId;
            u2DecType = _rDecInfo2.u2DecType;
        }
        else if (_rDecInfo3.u2CompId == pt_comp_id->ui2_id)
        {
            Printf( "_rDecInfo3: %d disconnected\n", (UINT32)pt_comp_id->ui2_id);
            _rDecInfo3.u2CompId = 0;
            u2BagtCompId = _rDecInfo3.u2SrcCompId;
            u2DecType = _rDecInfo3.u2DecType;
        }    
        else if (_rDecInfo4.u2CompId == pt_comp_id->ui2_id)
        {
            Printf( "_rDecInfo4: %d disconnected\n", (UINT32)pt_comp_id->ui2_id);
            _rDecInfo4.u2CompId = 0;
            u2BagtCompId = _rDecInfo4.u2SrcCompId;
            u2DecType = _rDecInfo4.u2DecType;
        }        
        else
        {
            LOG(0, "pt_comp_id %d doesn't connected\n", (UINT32)pt_comp_id->ui2_id);
            goto _ErrExit;
        }

        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(u2BagtCompId, DMX_MM_SEARCH_BY_SRC_ID);
        if(u1DmxMMInfoIdx >= DMX_MM_MAX_INSTNUM)
        {
            LOG(0, "u1DmxMMInfoIdx(%d) >= DMX_MM_MAX_INSTNUM\n", (UINT32)u1DmxMMInfoIdx);
            goto _ErrExit;
        }
        if(_arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId == pt_comp_id->ui2_id)
        {
            _arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist = FALSE;
            _arDmxMMInfo[u1DmxMMInfoIdx].u2VideoCompId = 0;
        }

        if((!IS_AUD_CONNECTED_COMP((DMX_MM_DEC_INFO_T *)&_rDecInfo)) &&
           (!IS_AUD_CONNECTED_COMP((DMX_MM_DEC_INFO_T *)&_rDecInfo1)) &&
           (!IS_AUD_CONNECTED_COMP((DMX_MM_DEC_INFO_T *)&_rDecInfo2)))
        {
            _arDmxMMInfo[u1DmxMMInfoIdx].fgAudIsConn = FALSE;
        } 

#ifdef TIME_SHIFT_SUPPORT
        // workaround for non time-shift format
        // the correct way is to get the correct pidx.
        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS)
        {
            u1Pidx = 0xFF;
            if ((UINT16)MTSWDMX_STRM_TYPE_VID == u2DecType)
            {
                u1Pidx = (UINT8)(DMX_PES_COMP_ID_START);
            }
            else if ((UINT16)MTSWDMX_STRM_TYPE_AUD == u2DecType)
            {
                u1Pidx = (UINT8)(DMX_PES_COMP_ID_START + 1);
            }
            else if ((UINT16)MTSWDMX_STRM_TYPE_AUD2 == u2DecType)
            {
                u1Pidx = (UINT8)(DMX_PES_COMP_ID_START + 2);
            }

            if(u1Pidx != 0xFF)
            {
                // Reset scramble state callback
                if (!_MwDmxMMResetScrambleCallback(u1Pidx))
                {
                    LOG(0, "_MwDmxMMResetScrambleCallback fail u1Pidx(%d)\n", (UINT32)u1Pidx);
                    _MwDmxConnectUnlock();
                    return RMR_DRV_DISC_FAILED;
                }
            }
        }
#endif

        UNUSED(u1Pidx);
        UNUSED(u2DecType);
        _MwDmxConnectUnlock();
        return RMR_OK;
    }
    else
    {
        u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
        if(u1DmxMMInfoIdx >= DMX_MM_MAX_INSTNUM)
        {
            LOG(0, "u1DmxMMInfoIdx(%d) >= DMX_MM_MAX_INSTNUM\n", (UINT32)u1DmxMMInfoIdx);
            goto _ErrExit;
        }
        else
        {
          MTSWDMX_DelRange(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, NULL, 0, TRUE);     //DTV00148327 before thumb nail stop, we should delete all the range info
          #ifdef LINUX_TURNKEY_SOLUTION
          _MwDmx_remove_range_list(u1DmxMMInfoIdx, 0);
          #endif
        }

    }

    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_CONN_INFO);

    LOG(5, "%s  u1DmxMMInfoIdx = %d, swdmx id = %d\n",
        __FUNCTION__,
        u1DmxMMInfoIdx,
        _arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);

    #ifdef CC_DMX_CONN_TYPE_SUPPORT
    MTDMX_MUL_FreeInst(_arDmxMMInfo[u1DmxMMInfoIdx].u1DmxId);
    _arDmxMMInfo[u1DmxMMInfoIdx].u1DmxId = u1DmxMMInfoIdx;
    #endif
    
#ifndef LINUX_TURNKEY_SOLUTION
    MTSWDMX_DelRangeList(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
#else
    while (g_pt_range_list[u1DmxMMInfoIdx])
    {
        if (g_pt_range_list[u1DmxMMInfoIdx]->pt_range)
        {
            _MwDmx_remove_range_list(u1DmxMMInfoIdx, g_pt_range_list[u1DmxMMInfoIdx]->pt_range->ui4_range_id);
        }
        else
        {
            break;
        }
    }
#endif

    MTSWDMX_UnlockSrc(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId);
    _MwDmxResetDmxInfo(u1DmxMMInfoIdx);

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;
    buf_agt_dmx_disconnect((BAGT_COMP_ID_T)prCompReg->ui2_id);

#ifdef CC_FLASH_TEST
    _fgEsPtsConnect = FALSE;
#endif

    UNUSED(e_disc_type);
    UNUSED(z_disc_info_len);

    _MwDmxConnectUnlock();
    return RMR_OK;

_ErrExit:
    _MwDmxConnectUnlock();
    return (RMR_DRV_INV_CONN_INFO);
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxMM_Set(
          const DRV_COMP_ID_T*    pt_comp_id,
          DRV_SET_TYPE_T        e_set_type,
          const VOID*         pv_set_info,
          SIZE_T            z_set_info_len)
{
    INT32 i4Ret = RMR_OK;

    // Check parameters
    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    // Check component type
    // ...


    switch (e_set_type)
    {
        case DEMUX_SET_TYPE_DEST_ID:
          i4Ret = _MwDmxOnMMSetDestination((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_CTRL:
          i4Ret = _MwDmxOnMMSetControl((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_STREAM_ID:
          i4Ret = _MwDmxOnMMSetStreamID((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_MEDIA_FORMAT:
          i4Ret = _MwDmxOnMMSetMediaFormat((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_ADD_RANGE:
          i4Ret = _MwDmxOnMMAddRange((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_DEL_RANGE:
          i4Ret = _MwDmxOnMMDelRange((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_SPEED:
          i4Ret = _MwDmxOnMMSetSpeed((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
		case DEMUX_SET_TYPE_PLAY_ONE_FRM:
          i4Ret = _MwDmxOnMMPlayOneFrm((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_SEEK_POSTION:
          i4Ret = _MwDmxOnMMSetSeekPos((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_BLOCK_PSI:
          i4Ret = _MwDmxOnMMSetPSIBlockMode((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_PRIORITY:
          i4Ret = _MwDmxOnMMSetPriority((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_AB_POSITION:
          i4Ret = _MwDmxOnMMSetABPosition((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
#if 0
        case DEMUX_SET_TYPE_PARSE_HDR_CMD:
          LOG(1, "mw->swdmx, DEMUX_SET_TYPE_PARSE_HDR_CMD\n");
          i4Ret = _MwDmxOnMMSetParseHdrCmd((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
#endif
        case DEMUX_SET_TYPE_SCRS_NFY_FCT:
          i4Ret = _MwDmxOnMMSetScrambleCallback((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;

#ifdef TIME_SHIFT_SUPPORT
        case DEMUX_SET_TYPE_DECRYPT_INFO:
          i4Ret = _MwDmxOnMMSetDescryptInfo((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_CRYPT_CMD:
          i4Ret = _MwDmxOnMMSetCryptCmd((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_MONITOR_LBA:
          i4Ret = _MwDmxOnMMSetMonitorLBA((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_REC_BUF_RANGE:
          i4Ret = _MwDmxOnMMSetRecBufRange((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
#else
#ifdef ENABLE_MULTIMEDIA
        case DEMUX_SET_TYPE_DECRYPT_INFO:
          i4Ret = _MwDmxOnMMSetDescryptInfo((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
#endif // ENABLE_MULTIMEDIA          
#endif
        case DEMUX_SET_TYPE_FILE_SZ_UPDATE:
            i4Ret = _MwDmxOnMMSetFileSizeUpdate((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
        break;
#ifdef  MULTIMEDIA_AUTOTEST_SUPPORT
        case DEMUX_SET_TYPE_MM_AUTOTEST:
          i4Ret=_MultiMediaAutoTest((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
#endif
        case DEMUX_SET_TYPE_STC_ID:
          i4Ret = _MwDmxOnMMSetStcId((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;

        case DEMUX_SET_TYPE_FREE_RUN:
          i4Ret = _MwDmxOnMMSetFreeRun((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_HDMVC_INFO:
          i4Ret = _MwDmxOnMMSetHdMvcInfo((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_VID_CHG_PID_NFY_FCT:
          i4Ret = _MwDmxOnMMSetVidChgCallback((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_AUD_CHG_PID_NFY_FCT:
          i4Ret = _MwDmxOnMMSetAudChgCallback((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;          
        case DEMUX_SET_TYPE_VID_PID_CHG_DONE:
          i4Ret = _MwDmxOnMMUpdateVidPid((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;
        case DEMUX_SET_TYPE_AUD_PID_CHG_DONE:
          i4Ret = _MwDmxOnMMUpdateAudPid((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
          break;         
        case DEMUX_SET_TYPE_VID_CODEC_CHG_DONE:
            i4Ret = _MwDmxOnMMUpdateVdec((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break; 
        case DEMUX_SET_TYPE_AUD_CODEC_CHG_DONE:
            i4Ret = _MwDmxOnMMUpdateAdec((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break;  
        case DEMUX_SET_TYPE_FLUSH_FEEDER:
            i4Ret = _MwDmxOnMMSetFlushFeederInfo((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break;
        case DEMUX_SET_TYPE_ALL_GNRC_STREAM_ID:
            i4Ret = _MwDmxOnMMSetGnrcStreamInfo((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break;
        case DEMUX_SET_TYPE_TS_DURATION:
            i4Ret = _MwDmxOnMMSetDuration((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break;
        case DEMUX_SET_TYPE_PUSH_CHANGE_AUDIO:
            i4Ret = _MwDmxOnMMSetPushChangeAudio((DRV_COMP_ID_T*)pt_comp_id, pv_set_info);
            break;
        default:
          break;
    }

    return i4Ret;
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:
 ----------------------------------------------------------------------------*/
static INT32 _MwDmxMM_Get(
            const DRV_COMP_ID_T*    pt_comp_id,
            DRV_GET_TYPE_T        e_get_type,
            VOID*           pv_get_info,
            SIZE_T*           pz_get_info_len)
{
    BOOL fgRet;
    UINT8 u1DmxMMInfoIdx;
    UINT32 u4Tmp = 0;
    UINT32* pu4Info;
    UINT64 u8Tmp;
    DEMUX_POST_INFO_T* prDmxPosInfo;
    DEMUX_UOP_CAP_INFO_T* prDemuxUopCapInfo;

    // 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) ||
        (NULL == pz_get_info_len))
    {
        return RMR_DRV_INV_GET_INFO;
    }

    u1DmxMMInfoIdx = _MwDmxFindDmxIdx(pt_comp_id->ui2_id, DMX_MM_SEARCH_BY_COMP_ID);
    DMX_MM_VERIFY_VALID_INSTNUM_ERROR(u1DmxMMInfoIdx, RMR_DRV_INV_GET_INFO);

    prDmxPosInfo = (DEMUX_POST_INFO_T*)pv_get_info;

    //get
    switch (e_get_type)
    {
        case DEMUX_GET_TYPE_CTRL:
          break;

        case DEMUX_GET_TYPE_BLOCK_PSI:
          fgRet = MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_SET_CHK_PSI_BUF_SIZE, &u4Tmp, 0, 0);
          if(fgRet == MTR_OK)
          {
              pu4Info = pv_get_info;
              *pu4Info = u4Tmp;
              return RMR_OK;
          }
          break;

        case DEMUX_GET_TYPE_CURR_LBA:
          fgRet = MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_GET_CURRENT_LBA, &u4Tmp, 0, 0);
          if(fgRet == MTR_OK)
          {
              pu4Info = pv_get_info;
              *pu4Info = u4Tmp;
              return RMR_OK;
          }
          break;

        case DEMUX_GET_TYPE_CURR_TICK_NUM:
          fgRet = MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_GET_CURRENT_TICK, &u4Tmp, 0, 0);
          if(fgRet == MTR_OK)
          {
              pu4Info = pv_get_info;
              *pu4Info = u4Tmp;
              return RMR_OK;
          }
          break;

        case DEMUX_GET_TYPE_UOP_CAPABILITY:

            prDemuxUopCapInfo = (DEMUX_UOP_CAP_INFO_T*)(pv_get_info);
            prDemuxUopCapInfo->b_is_allowed = TRUE;

            if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_CONTAINER)
            {
                if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype!=MEDIA_CONTNR_SUBTYPE_CMPB)
                {
                    if(prDemuxUopCapInfo->e_uop == DEMUX_UOP_SEEK)
                    {

                        UINT32 fgSeek = 0;
                        MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&fgSeek,0,0);
                        _arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist = (BOOL)fgSeek;
                        prDemuxUopCapInfo->b_is_allowed =_arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist;
                        if((!_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist) &&
                           (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_FLV) &&
                           (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_RM) &&
                           (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_AVI) &&
                           (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_ASF) &&
                           (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype != MEDIA_CONTNR_SUBTYPE_OGM))
                        {
                           prDemuxUopCapInfo->b_is_allowed = TRUE;
                        }

                    }
                    else if(prDemuxUopCapInfo->e_uop == DEMUX_UOP_TRICK)
                    {
                        if(prDemuxUopCapInfo->u.t_speed == MM_SPEED_TYPE_PAUSE ||
                            prDemuxUopCapInfo->u.t_speed == MM_SPEED_TYPE_FORWARD_1X )
                        {
                            prDemuxUopCapInfo->b_is_allowed = TRUE;
                        }
                        else
                        {

                            UINT32 fgTrick = 0;
                            MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&fgTrick,0,0);
                            if(!fgTrick)
                            {
                                if(prDemuxUopCapInfo->u.t_speed>MM_SPEED_TYPE_PAUSE
                                 &&_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist
                                 && !(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MP4 && prDemuxUopCapInfo->u.t_speed > MM_SPEED_TYPE_FORWARD_2X))
                               {
                                    if (prDemuxUopCapInfo->u.t_speed <= MM_TRICK_NOINDEX_MAXSPEED)
                                    {
                                        fgTrick=TRUE;
                                    }
                               }
                            }
                            _arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist = (BOOL)fgTrick;
                            prDemuxUopCapInfo->b_is_allowed = _arDmxMMInfo[u1DmxMMInfoIdx].fgKeyIsExist;
                        }

                        //Printf("******>>GET_TRICK_CAPABILITY speed=0x%x,allowed=%d\n",prDemuxUopCapInfo->u.t_speed,prDemuxUopCapInfo->b_is_allowed);
                    }
                    else
                    {
                        return RMR_DRV_INV_GET_INFO;
                    }
                }
                else
                {
                    prDemuxUopCapInfo->b_is_allowed = TRUE;
                }
            }
            else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES) ||
                    (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_AUDIO_ES))
            {

              UINT32 u4Trick = 0;
              UINT32 u4Forward = 0;
              if(_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType != MEDIA_AUD_SUBTYPE_WMA)
              {
                  if(prDemuxUopCapInfo->e_uop == DEMUX_UOP_TRICK)
                  {
                  if(prDemuxUopCapInfo->u.t_speed >= MM_SPEED_TYPE_FORWARD_1X)
                  {
                      u4Forward = 1;
                  }
                      MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&u4Trick,&u4Forward,0);
                        if(_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType != MEDIA_TYPE_AUDIO_ES)
                        {
                            if(prDemuxUopCapInfo->u.t_speed == MM_SPEED_TYPE_PAUSE)
                            {
                                u4Trick = TRUE;
                            }
                        }
                        else
                        {
                            if(prDemuxUopCapInfo->u.t_speed == MM_SPEED_TYPE_PAUSE ||
                               prDemuxUopCapInfo->u.t_speed == MM_SPEED_TYPE_FORWARD_1X)
                            {
                                u4Trick = TRUE;
                            }
                        }
                        prDemuxUopCapInfo->b_is_allowed = (BOOL)u4Trick;
                    }
                    else
                    {
	                  MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&u4Trick,&u4Forward,0);
	                  if (u4Trick)
	                  {
	                      if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed==MM_SPEED_TYPE_PAUSE || _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed==MM_SPEED_TYPE_FORWARD_1X)
	                      {
	                          prDemuxUopCapInfo->b_is_allowed=TRUE;
	                      }
	                      else
	                      {
	                          prDemuxUopCapInfo->b_is_allowed=FALSE;
	                      }                          
	                  }
	                  else
	                  {
	                      prDemuxUopCapInfo->b_is_allowed=FALSE;
	                  }

				      if (IS_RAW_AUDIOES_SUBTYPE(_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType) ||
                        _arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType == MEDIA_AUD_SUBTYPE_APE)
				      {
                          prDemuxUopCapInfo->b_is_allowed = TRUE;
				      }
					  else if((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) &&
                   		 ((_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT) ||
                    	 (_arDmxMMInfo[u1DmxMMInfoIdx].rTsSubType == MEDIA_TS_SUBTYPE_TS_TIME_SHIFT_CMPB)))
				  	  {
					      prDemuxUopCapInfo->b_is_allowed = TRUE;
					  }
                    }
                }
                else
                {
                    if(prDemuxUopCapInfo->e_uop == DEMUX_UOP_TRICK)
                    {
                        if(prDemuxUopCapInfo->u.t_speed==MM_SPEED_TYPE_PAUSE ||prDemuxUopCapInfo->u.t_speed==MM_SPEED_TYPE_FORWARD_1X)
                        {
                            prDemuxUopCapInfo->b_is_allowed = TRUE;
                        }
                        else
                        {
                            MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&u4Trick,&u4Forward,0);
                            prDemuxUopCapInfo->b_is_allowed = (BOOL)u4Trick;
                        }
                    }
                    else
                    {

                      MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_GET_IS_TRICK,&u4Trick,&u4Forward,0);
                      if (u4Trick)
                      {
                          prDemuxUopCapInfo->b_is_allowed=TRUE;
                          #if 0
                          if(_arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed==MM_SPEED_TYPE_PAUSE || _arDmxMMInfo[u1DmxMMInfoIdx].u4PreviousSpeed==MM_SPEED_TYPE_FORWARD_1X)
                          {
                              prDemuxUopCapInfo->b_is_allowed=TRUE;
                          }
                          else
                          {
                              prDemuxUopCapInfo->b_is_allowed=FALSE;
                          }
                          #endif
                      }
                      else
                      {
                          prDemuxUopCapInfo->b_is_allowed=FALSE;
                      }
                  }
              }

            }
            return RMR_OK;

        case DEMUX_GET_TYPE_POST_INFO:
            u8Tmp = 0;
            ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info = 0;
            fgRet = MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_GET_CURRENT_POSITION, (UINT32*)&u8Tmp, 0, 0);
            if(fgRet == MTR_OK)
            {
                ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info = u8Tmp;
            }
            if (((DEMUX_POST_INFO_T*)pv_get_info)->e_type == DEMUX_POST_INFO_MEDIA_RENDER_OFST)
            {
                if (_arDmxMMInfo[u1DmxMMInfoIdx].fgVideoExist)
                {
                    UINT32 u4Pts = 0;

                    MTB2R_GetPts(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &u4Pts);
                    prDmxPosInfo->u.render_info.ui8_pts_info = (UINT64)u4Pts;

                    //if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS) ||
                    //    (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) ||
                    //    (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_VIDEO_ES))
                    {
                        #if 1
                        MTB2R_POS_INFO_T rVdpPosInfo;
                        x_memset(&rVdpPosInfo, 0, sizeof(rVdpPosInfo));
                        if(MTB2R_GetPosInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1B2rId, &rVdpPosInfo) == MTR_OK)
                        {
                            LOG(5, "Get RENDER_OFST u8Offset(0x%llx) u8OffsetI(0x%llx) DecOrd(%d) TmpRef(%d)\n",
                                rVdpPosInfo.u8Offset,
                                rVdpPosInfo.u8OffsetI,
                                rVdpPosInfo.u2DecodingOrder,
                                rVdpPosInfo.i4TemporalRef);

                            prDmxPosInfo->u.render_info.ui8_aud_pts_info = rVdpPosInfo.u8AudPts;
                            prDmxPosInfo->u.render_info.ui8_aud_frame_position = rVdpPosInfo.u8AudOffset;
                            prDmxPosInfo->u.render_info.ui8_i_pts_info = rVdpPosInfo.u8PtsI;
                            prDmxPosInfo->u.render_info.ui8_pts_info = rVdpPosInfo.u8Pts;
                            prDmxPosInfo->u.render_info.ui2_decoding_order = rVdpPosInfo.u2DecodingOrder;
                            prDmxPosInfo->u.render_info.i4_temporal_reference = rVdpPosInfo.i4TemporalRef;
                            prDmxPosInfo->u.render_info.ui8_i_frame_position = rVdpPosInfo.u8OffsetI;
                            prDmxPosInfo->u.render_info.ui8_frame_position = rVdpPosInfo.u8Offset;
                            prDmxPosInfo->u.render_info.ui8_frame_position_disp = rVdpPosInfo.u8OffsetDisp;
                            prDmxPosInfo->u.render_info.ui8_stc = rVdpPosInfo.u8Stc;
                            prDmxPosInfo->u.render_info.ui4_timestap = rVdpPosInfo.u4Timestap;
                        }
                        else        //for mw A-B repeat error handle
                        {
                            prDmxPosInfo->u.render_info.ui8_aud_pts_info = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui8_aud_frame_position = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui8_i_pts_info = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui8_pts_info = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui2_decoding_order = 0xffff;
                            prDmxPosInfo->u.render_info.i4_temporal_reference = -1;
                            prDmxPosInfo->u.render_info.ui8_i_frame_position = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui8_frame_position = 0xffffffffffffffff;
                            prDmxPosInfo->u.render_info.ui8_frame_position_disp = 0xffffffffffffffff;

                            LOG(3, "Fail to Get DEMUX_POST_INFO_MEDIA_RENDER_OFST\n");
                            return RMR_DRV_INV_GET_INFO;

                        }
                        #else
                        UINT64 u8Offset;
                        UINT64 u8OffsetI;
                        UINT16 u2DecodingOrder;
                        INT32 i4TempRef;

                        u8Offset = 0;
                        u8OffsetI = 0;
                        u2DecodingOrder = 0;
                        i4TempRef = 0;

                        if(VDP_GetFilePosition(B2R_1, &i4TempRef, &u2DecodingOrder, &u8Offset, &u8OffsetI))
                        {
                            LOG(5, "Get DEMUX_POST_INFO_MEDIA_RENDER_OFST Fileoffset = %lld, IFileoffset = %lld\n",
                                u8Offset,
                                u8OffsetI);
                            LOG(5, "Get DEMUX_POST_INFO_MEDIA_RENDER_OFST u2DecodingOrder = %d, i4TempRef = %d\n",
                                u2DecodingOrder,
                                i4TempRef);

                            prDmxPosInfo->u.render_info.ui2_decoding_order = u2DecodingOrder;
                            prDmxPosInfo->u.render_info.i4_temporal_reference = i4TempRef;
                            prDmxPosInfo->u.render_info.ui8_i_frame_position = u8OffsetI;
                            prDmxPosInfo->u.render_info.ui8_frame_position = u8Offset;
                        }
                        else
                        {
                            LOG(3, "Fail to Get DEMUX_POST_INFO_MEDIA_RENDER_OFST\n");
                            return RMR_DRV_INV_GET_INFO;
                        }
                        #endif
                    }
                }
                else // audio only
                {
                    UINT32 u4Stc = 0;
                    MTSTC_GetStcValue(_arDmxMMInfo[u1DmxMMInfoIdx].u1StcId, &u4Stc);
                    prDmxPosInfo->u.render_info.ui8_pts_info = (UINT64)u4Stc;

                    if ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_PS) ||
                        (_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_TS) ||
                        ((_arDmxMMInfo[u1DmxMMInfoIdx].rContainerType == MEDIA_TYPE_AUDIO_ES) &&
                         (IS_RAW_AUDIOES_SUBTYPE(_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType) ||
                         (_arDmxMMInfo[u1DmxMMInfoIdx].rAudSubType == MEDIA_AUD_SUBTYPE_APE)))
                       )
                    {
                        prDmxPosInfo->u.render_info.ui2_decoding_order = 0;
                        prDmxPosInfo->u.render_info.i4_temporal_reference = 0;
                        prDmxPosInfo->u.render_info.ui8_i_frame_position = ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info;
                        prDmxPosInfo->u.render_info.ui8_frame_position = ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info;
                        prDmxPosInfo->u.render_info.ui8_frame_position_disp =
                            ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info;
                        LOG(5, "Get DEMUX_POST_INFO_MEDIA_RENDER_OFST Fileoffset = %lld, IFileoffset = %lld\n",
                            ((DEMUX_POST_INFO_T*)pv_get_info)->ui8_pos_info);
                    }
                }
                LOG(6, "Get RENDER_OFST Render Pts = %lld\n", prDmxPosInfo->ui8_pos_info);
            }
            return RMR_OK;
        case DEMUX_GET_TYPE_STC_OFFSET:
            fgRet = MTSWDMX_GetInfo(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, (UINT8)MTSWDMX_GET_STC_OFFSET, &u4Tmp, 0, 0);
            if(fgRet == MTR_OK)
            {
                pu4Info = pv_get_info;
                *pu4Info = u4Tmp;
                return RMR_OK;
            }
        default:
            break;
    }

    return RMR_DRV_INV_GET_INFO;
}


 /*-----------------------------------------------------------------------------
 * Name: _MW_DMX_MM_GetSwdmxSrcId
 *
 * Description: The down-stream module can use the component id to query swdmx src id
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  #define DMX_SECTION_MEM_COMP_NUM, DMX_SECTION_MEM_COMP_ID_START
 ----------------------------------------------------------------------------*/
UINT8 _MW_DMX_MM_GetSwdmxSrcId(UINT16 u2CompId)
{
    UINT8 u1DmxIdx = 0xFF;
    DMX_MM_DEC_INFO_T *prDecInfo = NULL;

    UNUSED(u1DmxIdx);

    if (_rDecInfo.u2CompId == u2CompId)
    {
        prDecInfo = &_rDecInfo;
    }
    else if (_rDecInfo1.u2CompId == u2CompId)
    {
        prDecInfo = &_rDecInfo1;
    }
    else if (_rDecInfo2.u2CompId == u2CompId)
    {
        prDecInfo = &_rDecInfo2;
    }
    else
    {
        return MTSWDMX_UNKNOWN_SOURCE;
    }

    u1DmxIdx = _MwDmxFindDmxIdx(prDecInfo->u2SrcCompId, DMX_MM_SEARCH_BY_SRC_ID);

    if (u1DmxIdx != DMX_MM_UNKNOWN_IDX)
    {
        return (_arDmxMMInfo[u1DmxIdx].u1SwdmxSrcId);
    }
    else
    {
        return MTSWDMX_UNKNOWN_SOURCE;
    }
}


 /*-----------------------------------------------------------------------------
 * Name: _MW_DMX_MM_AudSetCtrl
 *
 * Description: Demux Multimedia part, hook _AudSetCtrl
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  #define DMX_SECTION_MEM_COMP_NUM, DMX_SECTION_MEM_COMP_ID_START
 ----------------------------------------------------------------------------*/
VOID _MW_DMX_MM_AudSetCtrl(UINT8 u1SwdmxSrcId, UINT8 u1DecId, UINT32 u4AudCtrl)
{
    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx(u1SwdmxSrcId, DMX_MM_SEARCH_BY_SWDMX_ID);
    DMX_MM_VERIFY_VALID_INSTNUM(u1DmxMMInfoIdx);

    if ((u1DecId == MT_AUD_DEC_MAIN) &&
        //(u4AudCtrl >= (UINT32)AUD_DEC_CTRL_RESET) &&
        (u4AudCtrl <= (UINT32)AUD_DEC_CTRL_DETECT_TV_AUD_SYS))
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone = FALSE;
        _arDmxMMInfo[u1DmxMMInfoIdx].u1AudCtrlId = MT_AUD_DEC_MAIN;        
        _arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl = u4AudCtrl;
        Printf("_MW_DMX_MM_AudSetCtrl u1DecId %d u4AudCtrl %d\n", u1DecId,u4AudCtrl);
    }
    else if ((u1DecId == MT_AUD_DEC_AUX) &&
        //(u4AudCtrl >= (UINT32)AUD_DEC_CTRL_RESET) &&
        (u4AudCtrl <= (UINT32)AUD_DEC_CTRL_DETECT_TV_AUD_SYS))
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone = FALSE;
        _arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl = u4AudCtrl;
        _arDmxMMInfo[u1DmxMMInfoIdx].u1AudCtrlId = MT_AUD_DEC_AUX;
        Printf("_MW_DMX_MM_AudSetCtrl u1DecId %d u4AudCtrl %d\n", u1DecId,u4AudCtrl);
    }
    else if ((u1DecId == MT_AUD_DEC_THIRD) &&
        //(u4AudCtrl >= (UINT32)AUD_DEC_CTRL_RESET) &&
        (u4AudCtrl <= (UINT32)AUD_DEC_CTRL_DETECT_TV_AUD_SYS))
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone = FALSE;
        _arDmxMMInfo[u1DmxMMInfoIdx].u4AudCtrl = u4AudCtrl;
        _arDmxMMInfo[u1DmxMMInfoIdx].u1AudCtrlId = MT_AUD_DEC_THIRD;
        // if thrid layed, must be AD
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudioDescription = TRUE;
        Printf("_MW_DMX_MM_AudSetCtrl u1DecId %d u4AudCtrl %d\n", u1DecId,u4AudCtrl);
    }    
    else
    {
        Printf("_MW_DMX_MM_AudSetCtrl unknown %d\n", u4AudCtrl);
    }
}

 /*-----------------------------------------------------------------------------
 * Name: _MW_DMX_MM_AudMwNfyFunc
 *
 * Description: Demux Multimedia part, hook _AudMwNfyFunc
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  #define DMX_SECTION_MEM_COMP_NUM, DMX_SECTION_MEM_COMP_ID_START
 ----------------------------------------------------------------------------*/
VOID _MW_DMX_MM_AudMwNfyFunc(void *pvNfyTag, UINT8 u1DecId,
                UINT32 u4NfyCond, UINT32 u4Data1, UINT32 u4Data2, UINT32 u4Data3)
{
    UINT32 u4NfyTag = 0;

    UINT8 u1DmxMMInfoIdx = _MwDmxFindDmxIdx((UINT16)u4Data3, DMX_MM_SEARCH_BY_SWDMX_ID);
    DMX_MM_VERIFY_VALID_INSTNUM(u1DmxMMInfoIdx);

    u4NfyTag = (((UINT32)pvNfyTag) & 0xF);

    if((u4NfyTag == (UINT32)MTAUDDEC_NFY_STREAM)
        && (u4NfyCond== (UINT32)MTAUDDEC_COND_CTRL_DONE)
        && (!_arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone))
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone = TRUE;
        Printf( "_MW_DMX_MM_AudMwNfyFunc u4NfyCond done\n");
    }
    else if((u4NfyCond== (UINT32)MTAUDDEC_COND_AUD_DECODE_STATUS_CHG) && u4Data1 == MTAUDDEC_DECODE_NOT_SUPPORT)
    {
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudCtrlDone = TRUE;
        _arDmxMMInfo[u1DmxMMInfoIdx].fgAudNotSupport = TRUE;
        Printf( "_MW_DMX_MM_AudMwNfyFunc AUD_COND_AUD_DECODE_STATUS_CHG AUD_DECODE_NOT_SUPPORT\n");
    }
    else if((u4NfyCond== (UINT32)MTAUDDEC_COND_AUD_DECODE_STATUS_CHG) && u4Data1 == MTAUDDEC_DECODE_NO_DATA)
    {
        //SWDMX_AudioNotify(ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE);
        Printf( "_MW_DMX_MM_AudMwNfyFunc AUD_COND_AUD_DECODE_STATUS_CHG AUD_DECODE_NO_DATA\n");
    }
    else
    {
        LOG(3, "_MW_DMX_MM_AudMwNfyFunc unknown u4NfyCond %d %d\n", u4NfyCond, u4Data1);
    }
}

 /*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Registration
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  #define DMX_SECTION_MEM_COMP_NUM, DMX_SECTION_MEM_COMP_ID_START
 ----------------------------------------------------------------------------*/
BOOL _MW_DMX_MM_Init(void)
{
    UINT32 u4i = 0, u4Flags;
#ifdef TIME_SHIFT_SUPPORT
    UINT32 u4j = 0;
#endif
    INT32                ret;
    DRV_COMP_REG_T       t_comp_id;
    DRV_COMP_FCT_TBL_T   t_comp_fct_tbl;
    ID_IND_T             at_id_ind_sec[DMX_MM_CONTAINER_ES_COMP_NUM];
    //ID_IND_T             at_id_ind_fil[DMX_MM_CONTAINER_ES_COMP_NUM];

    /* register Demux Container ES component */
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmxMM_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmxMM_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmxMM_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmxMM_Set;

    t_comp_id.e_type = DRVT_DEMUX_CONTAINER_ES;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_MM_CONTAINER_ES_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind_sec;

    for ( u4i = 0; u4i < DMX_MM_CONTAINER_ES_COMP_NUM; u4i++ )
    {
        at_id_ind_sec[u4i].ui2_id = (UINT16)(DMX_MM_CONTAINER_ES_COMP_ID_START + u4i);
        at_id_ind_sec[u4i].pv_tag = NULL;
        at_id_ind_sec[u4i].ui1_port = ALL_PORTS;
    }

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    ret = x_rm_reg_comp(&t_comp_id,
                        1,                              /* num in port */
                        1,                              /* num out port */
                        NULL, /* name */
                        u4Flags,                        /* flag */
                        &t_comp_fct_tbl,
                        NULL,
                        0);
    if ( ret != RMR_OK )
    {
      return FALSE;
    }

    /* register Demux Control component */
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmxMM_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmxMM_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmxMM_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmxMM_Set;

    t_comp_id.e_type = DRVT_DEMUX_CONTROL;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_MM_CONTROL_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind_sec;

    for ( u4i = 0; u4i < DMX_MM_CONTROL_COMP_NUM; u4i++ )
    {
        at_id_ind_sec[u4i].ui2_id = (UINT16)(DMX_MM_CONTROL_COMP_ID_START+ u4i);
        at_id_ind_sec[u4i].pv_tag = NULL;
        at_id_ind_sec[u4i].ui1_port = ALL_PORTS;
    }

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    ret = x_rm_reg_comp(&t_comp_id,
                        1,                              /* num in port */
                        1,                              /* num out port */
                        NULL, /* name */
                        u4Flags,                        /* flag */
                        &t_comp_fct_tbl,
                        NULL,
                        0);
    if ( ret != RMR_OK )
    {
      return FALSE;
    }

    if(!_MW_DMX_MM_PES_Init())
    {
        return FALSE;
    }


    MTSWDMX_Init();

    if (!_fgInit)
    {
        UINT32 u4Addr,u4Size;
        u4Addr=(UINT32)_arDmxMMInfo;
        u4Size=DMX_MM_MAX_INSTNUM*sizeof(DMX_MM_MW_INFO_T);
        x_memset((void *)u4Addr,0,u4Size);
        UNUSED(u4Addr);
        UNUSED(u4Size);
        for (u4i = 0; u4i < DMX_MM_CONTROL_COMP_NUM; u4i++ )
        {
            VERIFY(x_sema_create(&_arDmxMMInfo[u4i].hDmxCtrlSema,
                X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

            VERIFY(x_sema_create(&_arDmxMMInfo[u4i].hFlushMutex,
                X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

            _arDmxMMInfo[u4i].fgAvailable = TRUE;
            _arDmxMMInfo[u4i].fgFirstRange = TRUE;
            _arDmxMMInfo[u4i].u1SwdmxSrcId = MTSWDMX_UNKNOWN_SOURCE;
            _arDmxMMInfo[u4i].u1VdecId = u4i;
            _arDmxMMInfo[u4i].u1DmxId = u4i;
            _arDmxMMInfo[u4i].u1StcId = u4i;
            
            _arDmxMMInfo[u4i].u1AdecId= MT_AUD_DEC_MAIN; // 0831
            _arDmxMMInfo[u4i].u1Adec2Id= MT_AUD_DEC_AUX;
            _arDmxMMInfo[u4i].u1Adec3Id= MT_AUD_DEC_THIRD;
            _MwDmxResetDmxInfo(u4i);

    #ifdef TIME_SHIFT_SUPPORT
            for(u4j=0; u4j<DMX_MAX_NUM_PID_INDEX_TABLE; u4j++)
            {
                _arDmxMMInfo[u4i].arDmxMMKeyInfo[u4j].fgValid = 0;
                _arDmxMMInfo[u4i].arDmxMMKeyInfo[u4j].u1KeyIdx = 0xFF;
                _arDmxMMInfo[u4i].arDmxMMKeyInfo[u4j].u2StreamID = 0xFFFF;
                _arDmxMMInfo[u4i].arDmxMMKeyInfo[u4j].eDescMode = MTDMX_DESC_MODE_NONE;
            }
    #endif  // TIME_SHIFT_SUPPORT
        }

        VERIFY(x_sema_create(&_hDmxMMInfoMutex,
            X_SEMA_TYPE_MUTEX,
            X_SEMA_STATE_UNLOCK) == OSR_OK);

        VERIFY(x_sema_create(&_hDmxConnectMutex,
            X_SEMA_TYPE_MUTEX,
            X_SEMA_STATE_UNLOCK) == OSR_OK);
                #ifdef LINUX_TURNKEY_SOLUTION
        for(u4i = 0; u4i < DMX_MM_MAX_INSTNUM; u4i++ )
        {
            g_pt_range_list[u4i] = NULL;
        }
                #endif
        _fgInit = TRUE;
    }


    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetVidChgCallback
 *  Set video change notify callback
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnMMSetVidChgCallback(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if (pv_set_info == NULL || pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return FALSE;
    }
    Printf("%s,%d pv_set_info %p entry\n",__FUNCTION__,__LINE__,pv_set_info);
    
    MTSWDMX_RegPidChgCallback(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_VID_CHG_CALLBACK, (UINT32)pv_set_info,0,0);

    return TRUE;       
}
//-----------------------------------------------------------------------------
/** _MwDmxOnMMSetAudChgCallback
 *  Set video change notify callback
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxOnMMSetAudChgCallback(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if (pv_set_info == NULL || pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return FALSE;
    }
    Printf("%s,%d pv_set_info %p entry\n",__FUNCTION__,__LINE__,pv_set_info);
    
    MTSWDMX_RegPidChgCallback(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_AUD_CHG_CALLBACK, (UINT32)pv_set_info,0,0);
    return TRUE;       
}
//-----------------------------------------------------------------------------
/** _MwDmxOnMMUpdateVidPid
 *  Set video change notify callback
 *
 *  @param  pt_comp_id      Component ID
 *  @param  pv_set_info     Set info
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwDmxOnMMUpdateVidPid(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if (pv_set_info == NULL || pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return RMR_INV_ARG;
    }
    Printf("%s,%d entry\n",__FUNCTION__,__LINE__);
       
    MTSWDMX_PidChgDone(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_TYPE_VID_PID_CHG_DONE, (UINT32)pv_set_info, 0, 0);
    return RMR_OK;
}

static INT32 _MwDmxOnMMUpdateAudPid(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if (pv_set_info == NULL || pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return RMR_INV_ARG;
    }
    Printf("%s,%d entry\n",__FUNCTION__,__LINE__);
       
    MTSWDMX_PidChgDone(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_TYPE_AUD_PID_CHG_DONE, (UINT32)pv_set_info, 0, 0);
    return RMR_OK;
}

static INT32 _MwDmxOnMMUpdateVdec(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if ( pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return RMR_INV_ARG;
    }
    Printf("%s,%d entry\n",__FUNCTION__,__LINE__);
    MTSWDMX_CodecChgDone(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_TYPE_VID_CODEC_CHG_DONE, 0, 0, 0);
    
    return RMR_OK;
}

static INT32 _MwDmxOnMMUpdateAdec(DRV_COMP_ID_T* pt_comp_id,const VOID* pv_set_info)
{
    UINT8 u1DmxMMInfoIdx = 0;
    
    if (pv_set_info == NULL || pt_comp_id == NULL)
    {
        LOG(0,"%s,%d para is null \n",__FUNCTION__,__LINE__);
        return RMR_INV_ARG;
    }
    Printf("%s,%d entry\n",__FUNCTION__,__LINE__);
    
    MTSWDMX_CodecChgDone(_arDmxMMInfo[u1DmxMMInfoIdx].u1SwdmxSrcId, MTSWDMX_SET_TYPE_AUD_CODEC_CHG_DONE, 0, 0, 0);
    
    return RMR_OK;
}


