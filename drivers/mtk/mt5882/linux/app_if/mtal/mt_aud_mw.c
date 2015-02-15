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
 * $RCSfile: mt_audio.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file mt_aud_mw.c
 *  MTAL kernel driver implementation: Audio decoder
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtauddec.h"
#include "x_assert.h"
#include "aud_if.h"

#include "x_util.h"         //53xx_com_driver/inc/
#include "x_rm.h"           //middleware/res_mngr
#include "x_aud_dec.h"      //middleware/res_mngr/drv
#include "u_drv_cust.h"     //middleware/res_mngr/drv
#ifdef LINUX_TURNKEY_SOLUTION
#include "u_rm_dev_types.h" //middleware/res_mngr
#endif
#include "cb_data.h"
#include "cb_low.h"
#ifdef ENABLE_MULTIMEDIA
#include "dmx_mm_if.h"
#include "swdmx_if.h"       //53xx_com_driver/swdmx_v2
#endif

#include "eepdef.h"
#include "api_eep.h"
#include "x_aud_enc.h"
// overmtal
#include "aud_debug.h"
#include "x_debug.h"
#include "x_pinmux.h"

#ifdef CC_AUD_SUPPORT_OSS_ADAPTION_LAYER
#include "oss_adap.h"
#endif

#include "aud_drv.h"
#include "aud_cfg.h"

#define COPY_FROM_USER(ArgUsr, ArgKernel, ArgSize)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       ArgSize))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER(ArgUsr, ArgKernel,ArgSize)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       ArgSize))          \
    {                                                       \
        printk("mtal_ioctl_osd argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                                             \

#define COPY_FROM_USER_z_size(type)\
       type   z_size;\
       type*  pz_size = &z_size;\
       COPY_FROM_USER(Upz_size,z_size,sizeof(type));

#define COPY_TO_USER_z_size(type)\
       COPY_TO_USER(Upz_size, z_size, sizeof(type));

#define COPY_FROM_USER_get_type(type)\
       type   v_get_type;\
       type*  pv_get_type = &v_get_type;\
       COPY_FROM_USER(Upv_get_type,v_get_type,sizeof(type));

#define COPY_TO_USER_get_type(type)\
       COPY_TO_USER(Upv_get_type,v_get_type,sizeof(type));

#define COPY_FROM_USER_set_type(type)\
       type   v_set_type;\
       type*  pv_set_type = &v_set_type;\
       COPY_FROM_USER(Upv_set_type,v_set_type,sizeof(type));

#define COPY_TO_USER_z_get_info_len(type)\
       COPY_TO_USER(Upz_get_info_len, z_get_info_len, sizeof(type));

#define COPY_FROM_USER_comp_id(type)\
       type   t_comp_id;\
       type*  pt_comp_id = &t_comp_id;\
       COPY_FROM_USER(Upt_comp_id,t_comp_id,sizeof(type));

#define COPY_FROM_USER_get_info(type)\
       type   v_get_info;\
       type*  pv_get_info = &v_get_info;\
       COPY_FROM_USER(Upv_get_info,v_get_info,sizeof(type));

#define COPY_TO_USER_get_info(type)\
       COPY_TO_USER(Upv_get_info,v_get_info,sizeof(type));

#define COPY_FROM_USER_set_info(type)\
       type   v_set_info;\
       type*  pv_set_info = &v_set_info;\
       COPY_FROM_USER(Upv_set_info,v_set_info,sizeof(type));

/*-----------------------------------------------------------------------------
                    Macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define TREBLE_MAX_VAL          100
#define TREBLE_MIN_VAL          0
#define BASS_MAX_VAL            100
#define BASS_MIN_VAL            0

#ifdef CC_AUD_PEQ_BASS_TREBLE
// -8dB ~ 8dB => -8 * 2 + 24 ~ 8 * 2 + 24 = 8 ~ 40
#define TREBLE_TABLE_MAX_VAL    40
#define TREBLE_TABLE_MIN_VAL    8
#define BASS_TABLE_MAX_VAL      40
#define BASS_TABLE_MIN_VAL      8
#else
#define TREBLE_TABLE_MAX_VAL    28
#define TREBLE_TABLE_MIN_VAL    0
#define BASS_TABLE_MAX_VAL      28
#define BASS_TABLE_MIN_VAL      0
#endif

#define OUT_PORT_NUM            (AUD_DEC_OUT_PORT_SPEAKER + 1)
#define AUD_MAX_DELAY_IN_MS     20

#ifdef CC_AUD_7BAND_EQUALIZER
#define CH_EQ_BAND_NUM          7
#else
#define CH_EQ_BAND_NUM          5
#endif

#ifdef CC_AUD_4_DECODER_SUPPORT
#define AUD_DEC_PIP_SUB         4   //decoder id "4" means audio in PIP sub mode
#elif defined (CC_MT5391_AUD_3_DECODER)
#define AUD_DEC_PIP_SUB         3   //decoder id "3" means audio in PIP sub mode
#else
#define AUD_DEC_PIP_SUB         2   //decoder id "2" means audio in PIP sub mode
#endif

#define GET_MAP_VAL(u1ValMax, u1ValMin, u1IdxMax, u1IdxMin, u1Idx)        \
          ((UINT8)((UINT16)(((UINT16)u1ValMax - (UINT16)u1ValMin) * ((UINT16)u1Idx - (UINT16)u1IdxMin)) /   \
          (UINT16)((UINT16)u1IdxMax - (UINT16)u1IdxMin)) + u1ValMin)                   \

UINT8 GET_TREBLE_MAP_VAL(UINT8 u1Val)
{
      return GET_MAP_VAL(TREBLE_TABLE_MAX_VAL, TREBLE_TABLE_MIN_VAL,
                         TREBLE_MAX_VAL, TREBLE_MIN_VAL, u1Val);
}

UINT8 GET_BASS_MAP_VAL(UINT8 u1Val)
{
      return GET_MAP_VAL(BASS_TABLE_MAX_VAL, BASS_TABLE_MIN_VAL,
                      BASS_MAX_VAL, BASS_MIN_VAL, u1Val);
}

#define M_GET_VALIDATE(TYPE)                \
    if (*pz_get_info_len < sizeof(TYPE))    \
    {                                       \
        return RMR_DRV_NOT_ENOUGH_SPACE;    \
    }                                       \
    *pz_get_info_len = sizeof(TYPE);        \

#define M_SET_VALIDATE(TYPE)                \
    if (sizeof(TYPE) != z_set_info_len)     \
    {                                       \
        return RMR_DRV_INV_SET_INFO;        \
    }                                       \

/*-----------------------------------------------------------------------------
                    Local type definitions
 ----------------------------------------------------------------------------*/

/* Channel volume info. */
typedef struct _AUD_DEC_CH_VOL_T
{
    UINT8  u1VolL;
    UINT8  u1VolR;
    UINT8  u1VolSL;
    UINT8  u1VolSR;
    UINT8  u1VolC;
    UINT8  u1VolSW;
    UINT8  u1VolDmxL;
    UINT8  u1VolDmxR;
    UINT8  u1VolBypassL;
    UINT8  u1VolBypassR;
    UINT8  u1VolMaster;
    UINT8  u1VolTone;
}   AUD_DEC_CH_VOL_T;

/*-----------------------------------------------------------------------------
                    Local Variable definitions
 ----------------------------------------------------------------------------*/
#ifdef AUD_DECODER3_SELF_TEST
extern UINT8 u1ForceDecId; //parson MM3
#endif
#ifdef  LINUX_TURNKEY_SOLUTION
VOID * pClip_buf;
static UINT8 u1Clipflag;
#endif
static AUD_DEC_SPDIF_TYPE_T _eSpdif = AUD_DEC_SPDIF_PCM_24;
static BOOL _fgSpeakerPortOn = FALSE;
static BOOL _fgHeadphonePortOn = FALSE;
static BOOL _fgOutPortMask = FALSE;
static UINT32 _u4OutPort = AUD_DEC_OUT_PORT_FLAG_NONE;
static UINT32 _u4AdOutPort = AUD_DEC_OUT_PORT_FLAG_NONE;
#ifdef CC_AUD_BBE_SUPPORT
static AUD_DEC_BBE_MODE_T  _aeBbeMode = AUD_DEC_BBE_MODE_OFF;
#endif

#ifdef AUD_SUPPORT_SRS_BYPASS
static UINT32 isSrsBypass = FALSE;
#endif // AUD_SUPPORT_SRS_BYPASS

static UINT16                  _au2MwConnectComp[AUD_DEC_NUM];
static AUD_DEC_FMT_INFO_T      _arAudDecFmt[3];
static AUD_FMT_T               _arAudDriverDecFmt[AUD_DEC_NUM];
static AUD_DEC_CTRL_T          _aeCtrlSetting[3] = {AUD_DEC_CTRL_STOP, AUD_DEC_CTRL_STOP, AUD_DEC_CTRL_STOP};
#ifdef CC_SUPPORT_STR
AUD_DEC_MUTE_TYPE_T            _aeMuteType[AUD_DEC_NUM] = {AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF};
#else
static AUD_DEC_MUTE_TYPE_T     _aeMuteType[AUD_DEC_NUM] = {AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF, AUD_DEC_MUTE_OFF};
#endif
static AUD_DEC_HDCD_FLTR_T     _aeHdcdFilter[3] = {AUD_DEC_HDCD_FLTR_OFF, AUD_DEC_HDCD_FLTR_OFF, AUD_DEC_HDCD_FLTR_OFF};
static AUD_DEC_LS_MODE_T       _aeLsMode[3][6] =
{
    {
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE
    },
    {
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE
    },
    {
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE,
        AUD_DEC_LS_MODE_LARGE_FREQ_RANGE, AUD_DEC_LS_MODE_LARGE_FREQ_RANGE
    }
};

static VOID* _arSeInfo[3][AUD_DEC_SE_NUM];
static AUD_DEC_DOWNMIX_MODE_T  _aeDownmixMode[3] = {AUD_DEC_DM_OFF, AUD_DEC_DM_OFF, AUD_DEC_DM_OFF};
static AUD_DEC_MATRIX_MODE_T _aeMatrixMode[3] = {AUD_DEC_MTR_OFF, AUD_DEC_MTR_OFF, AUD_DEC_MTR_OFF};
static AUD_DEC_DRC_T           _aeDrc[3] = {AUD_DEC_DRC_OFF, AUD_DEC_DRC_OFF, AUD_DEC_DRC_OFF};
static AUD_DEC_MEM_BUFFER_INFO_T _arMemBuf[3];
static AUD_DEC_CMPSS_MODE_T _aeCmprssMode[3] = {AUD_DEC_CMPSS_MODE_LINE, AUD_DEC_CMPSS_MODE_LINE, AUD_DEC_CMPSS_MODE_LINE};
static AUD_DEC_CMPSS_DRC_MODE_T _aeCmprssDrcMode[3] = {AUD_DEC_CMPSS_DRC_MODE_STANDARD, AUD_DEC_CMPSS_DRC_MODE_STANDARD, AUD_DEC_CMPSS_DRC_MODE_STANDARD};

static AUD_DEC_CH_VOL_T _arChVol[AUD_DEC_NUM];
static BOOL _afgAgcFlag[3] = {TRUE, TRUE, TRUE};
//static AUD_DEC_STREAM_FROM_T _aeStrSrc[3] = {AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER};
static AUD_DEC_STREAM_FROM_T _aeStrSrc[5] = {AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_STREAM_FROM_DIGITAL_TUNER};
static UINT16 _aau2MasterDelay[3][AUD_DEC_OUT_PORT_SPEAKER + 1] =
{
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

static AUD_DEC_CHNL_DELAY_IND_T _arChlDelayInd[3][OUT_PORT_NUM];
static AUD_DEC_COPY_PROTECT_T _aeCopyProtect[3] = {AUD_DEC_COPY_PROTECT_NO_RESTRICT, AUD_DEC_COPY_PROTECT_NO_RESTRICT, AUD_DEC_COPY_PROTECT_NO_RESTRICT};
//static UINT8 _au1Balance[2] = {(MAX_VOL_LEVEL/2), (MAX_VOL_LEVEL/2)};
#ifdef CC_SUPPORT_STR
BOOL _afgMute[AUD_DEC_NUM] = {FALSE, FALSE, FALSE, FALSE};
#else
static BOOL _afgMute[AUD_DEC_NUM] = {FALSE, FALSE, FALSE, FALSE};
#endif
static INT16 _i2HpVol = 0;
//static UINT32 _au4TvSysMask[2] = {0, 0};
static BOOL _fgForceToPal = 0;

static AUD_DEC_TV_AUD_SYS_T _arTvAudSysMask[AUD_DEC_NUM];

#ifdef CC_AUD_7BAND_EQUALIZER
static INT16 _ai2UserEqCfg[AUD_EQ_NUM][CH_EQ_BAND_NUM] =
{
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
}; // BBE TV EQ configuration in unit of 1/10 dB
#else
static INT16 _ai2UserEqCfg[AUD_EQ_NUM][CH_EQ_BAND_NUM] =
{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
}; // BBE TV EQ configuration in unit of 1/10 dB
#endif  //CC_AUD_7BAND_EQUALIZER

static AUD_DEC_AUD_CLIP_T _arAudClipSetType[3];
static AUD_DEC_AUD_CLIP_INFO_T _arAudClipInfo[3] = {{0, 0}, {0, 0}, {0, 0}};
static AUD_CLIP_TYPE_T _arAudClip[3];
static BOOL _afgPlayAudClip[3] = {FALSE, FALSE, FALSE};
static BOOL _afgTvSysDetecting[3] = {FALSE, FALSE, FALSE};
static AUD_DEC_PL2_CTRL_INFO_T _rPrologic2Type;
static AUD_DEC_NFY_INFO_T      _arAudMwNotifyInfo[3];
static BOOL _afgDolbyBanner[3] = {FALSE, FALSE, FALSE};
static AUD_DEC_MUTE_EX_INFO_T _arAudMuteEx[3] = {{AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS}, {0}, {0}};
static AUD_DEC_HDPH_DETECT_NFT_INFO_T _arAudHPMwNotifyInfo[3];
static AUD_DEC_UPLOAD_DATA_NFT_INFO_T _arAudYWMwNotifyInfo[3];


#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
static BOOL _afgAudDecMwIfRealCtrl[3] = {0, 0, 0}; // if MW_if issue play , set to 1
                                                                    // if MW_if issue stop , set to 0
static BOOL _afgAudDecMwIntendCtrl[3] = {0, 0, 0}; // if MW intend to issue play , set to 1
                                                                    // if MW intend to issue stop , set to 0
static AUD_DEC_FMT_INFO_T      _arAudDecFmtMwSet[3];   // for mw_if auto playing deocder usage
#endif

#ifdef CC_MPEG_LEVEL_ADJ
static UINT8 _u1MpegLevel;
#endif

static UINT8 _u1SpdifLevel = 0;

AUD_DEC_OVM_MODE_T _arAudDecOvmMode = AUD_DEC_OVM_OFF;

#ifdef CC_AUD_DSP_SUPPORT_AUDYSSEY
static AUD_DEC_ADV_MODE_T  _aeADVMode = {AUD_DEC_ADV_MODE_OFF};
static AUD_DEC_ABX_MODE_T  _aeABXMode = {AUD_DEC_ABX_MODE_OFF};
static AUD_DEC_AEQ_MODE_T  _aeAEQMode = {AUD_DEC_AEQ_MODE_OFF};
#endif

CHAR* _paStrmNfyCond[] =
{
    "ERROR",
    "CTRL_DONE",
    "AUD_CLIP_DONE",
    "MEM_BUFFER_DONE",
    "FLUSH_DONE",
    "FEED_ME_ON",
    "FEED_ME_OFF",
    "AUD_INFO_CHG",
    "TV_AUD_SYS_DETECTED",
    "AUD_DECODE_STATUS_CHG",
    "MM_EOS",
    "FM_RADIO_DET_DONE",
    "PROTECT_INFO",
    "UPLOAD_INFO_CHG",
    "BLUETOOTH_INFO_CHG",
    "AUD_CLIP_PLAY_DONE",
    "AUD_CODEC_CHG",
    "AUD_BIT_RATE_CHG"
};
#ifdef CC_S_SUBWOFFER_SUPPORT
static UINT16 au2SWCutOffMap[AQ_SW_CUTOFF_NUM] = {0, 0, 0, 0, 0, 0, 0};// {50,80,100,120,150,200,300};
#endif

#if (!defined(MW_TV_AV_BYPASS_SUPPORT) || defined(APP_TV_AV_SCART_BYPASS_CONFIG_AS_NO_TV_SCART)) //gallen 0827
static BOOL _fgPlayAudDscr = FALSE; //For Audio Description
#else
static BOOL _fgPlayAudDscr ; //For Audio Description
#endif
static BOOL _fgAdFlag = FALSE;
extern BOOL _fgCodecSCAR2Mute;
extern void AUD_DspMasterVolume(UINT8 u1DecID, UINT8 u1Volume);
extern void _ADAC_SpeakerEnable(BOOL fgEnable);
extern void ADAC_SpeakerState_Reinit(void);
extern void AUD_AVMuxSel(UINT8 u1Sel);
extern void _ADAC_MuteEnable(BOOL fgEnable);
extern AUD_TYPE_T DSP_GetMtsSrcTyp(UINT8 u1DecId);
extern AUD_TYPE_T DSP_GetAudChlInfo(UINT8 u1DecId);
extern void DSP_SetEuroCanalPlusMode(BOOL fgEnable);
extern void AUD_AoutPADMux(AUD_CH_T ePAD_SDATA0, AUD_CH_T ePAD_SDATA1, AUD_CH_T ePAD_SDATA2,
                    AUD_CH_T ePAD_SDATA3, AUD_CH_T ePAD_SDATA4, AUD_CH_T ePAD_SDATA5,
                    UINT8 PAD_MCLK0);
extern void DSP_SetDetTvSrcMode(TV_AUD_SYS_T eAudSys);
extern UINT32 AUD_AudioDescriptionChlSel(UINT8 u1DecId, AUD_CHL_DEF_T eChl);
#ifdef ENABLE_MULTIMEDIA
extern BOOL _SWDMX_SetInfo(UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3);
#endif
extern BOOL _AUD_IgnoreDecNotify(UINT8 u1DecId, BOOL fgIgnore);
extern BOOL _IsDualDecMode(void);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
extern void AUD_SetMMAudioOnly(UINT8 u1DecId, BOOL fgMMAudioOnly);
#else
extern void AUD_SetMMAudioOnly(BOOL fgMMAudioOnly);
#endif
extern void AUD_DspMixSndVolume(UINT8 u1DecId, UINT8 u1Volume);
extern void AUD_GetRWPtr(UINT8 u1DecId, UINT32 * pu4Rp, UINT32 * pu4Wp, UINT32 *pu4Size);

#ifdef ENABLE_MULTIMEDIA
static UINT8 _au1SwdmxSrcId[AUD_DEC_NUM] = {0};
extern const UINT32 _au4CookSampPerFrame[30];
#endif

#ifdef CC_SUPPORT_STR
extern BOOL fgAudPmStrMode;
#endif

#ifdef CC_PHI_DREAMCATCHER_DANGERIOUS
extern void IMGlueclrsndP_SetClrSnd(BOOL onoff);
#endif

extern BOOL _fgMMPlusAD[AUD_DEC_MAX];
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Export functions
//-----------------------------------------------------------------------------
#ifdef CC_DUAL_AUD_DEC_SUPPORT
static UINT8 _AudConvertDecId_Dual(UINT16 u2_comp_id, UINT8* u1DecId, UINT32 eType);
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
                    Local Function definitions
 ----------------------------------------------------------------------------*/

#define IS_TUNER_UNI_SYS()      TRUE
#define IS_TUNER_MULTI_SYS()    TRUE

static BOOL _IsMaskAmbiguousTvSys(UINT8 u1DecId)
{
    UNUSED(_IsMaskAmbiguousTvSys);
    UNUSED(_arTvAudSysMask);
    UNUSED(u1DecId);
    return FALSE;
}

static BOOL _IsMaskSetToMtsSys(UINT8 u1DecId)
{
    UINT8 u1TmpID;

    UNUSED(u1TmpID);
    UNUSED(_IsMaskSetToMtsSys);
#ifdef CC_DUAL_AUD_DEC_SUPPORT
    _AudConvertDecId_Dual(u1DecId, &u1TmpID, 0xFFFFFFFF);
    u1DecId = u1TmpID; //Can not change 
#endif   
    
    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    if ((!_IsMaskAmbiguousTvSys(u1DecId)) &&
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_BTSC) == AUDIO_SYS_MASK_BTSC) &&
        (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_M) == TV_SYS_MASK_M) ||
        ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_N) == TV_SYS_MASK_N)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL _IsMaskSetToA2Sys(UINT8 u1DecId)
{
    UINT8 u1TmpID;

    UNUSED(u1TmpID);
    UNUSED(_IsMaskSetToA2Sys);
#ifdef CC_DUAL_AUD_DEC_SUPPORT
    _AudConvertDecId_Dual(u1DecId, &u1TmpID, 0xFFFFFFFF);
    u1DecId = u1TmpID; //Can not change 
#endif
    
    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    if ((!_IsMaskAmbiguousTvSys(u1DecId)) &&
        (((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2) == AUDIO_SYS_MASK_FM_A2) ||
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2_DK1) == AUDIO_SYS_MASK_FM_A2_DK1) ||
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2_DK2) == AUDIO_SYS_MASK_FM_A2_DK2)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL _IsMaskSetToPalSys(UINT8 u1DecId)
{
    UINT8 u1TmpID;

    UNUSED(u1TmpID);
    UNUSED(_IsMaskSetToPalSys);
#ifdef CC_DUAL_AUD_DEC_SUPPORT
    _AudConvertDecId_Dual(u1DecId, &u1TmpID, 0xFFFFFFFF);
    u1DecId = u1TmpID; //Can not change 
#endif

    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    if ((!_IsMaskAmbiguousTvSys(u1DecId)) &&
        (((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_MONO) == AUDIO_SYS_MASK_FM_MONO) ||
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_AM) == AUDIO_SYS_MASK_AM) ||
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_NICAM) == AUDIO_SYS_MASK_NICAM)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL _IsMaskSetToFmfmSys(UINT8 u1DecId)
{
    UINT8 u1TmpID;

    UNUSED(u1TmpID);
    UNUSED(_IsMaskSetToFmfmSys);
#ifdef CC_DUAL_AUD_DEC_SUPPORT
    _AudConvertDecId_Dual(u1DecId, &u1TmpID, 0xFFFFFFFF);
    u1DecId = u1TmpID; //Can not change 
#endif   
    
    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    if ((!_IsMaskAmbiguousTvSys(u1DecId)) &&
        ((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_EIA_J) == AUDIO_SYS_MASK_FM_EIA_J))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL _IsRunDetection(UINT8 u1DecId)
{
    UNUSED(_IsRunDetection);
    return _IsMaskAmbiguousTvSys(u1DecId);
}

#ifdef ENABLE_MULTIMEDIA  //Sync code for multimedia, from Isaac 20120130
BOOL AUD_MW_SWDMX_SetInfo(UINT8 u1DecId, UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3)
{
#ifdef CC_53XX_SWDMX_V2
    UINT8 u1SrcId;
    if(u1DecId >= AUD_DEC_NUM)
    {
        LOG(3, "(u1DecId(%d) >= AUD_DEC_NUM)\n", u1DecId);
        return FALSE;
    }

    u1SrcId = _au1SwdmxSrcId[u1DecId];

#ifdef CC_FBM_TWO_FBP
    //should get swdmx id according to config
    //u1SrcId = 0;
#endif

    if (u1SrcId >= SWDMX_SOURCE_MAX)
    {
        LOG(1, "(u1SrcId(%d) >= SWDMX_SOURCE_MAX)\n", u1SrcId);
        return FALSE;
    }

    return SWDMX_SetInfo(u1SrcId, u1Type, u4Para1, u4Para2, u4Para3);

#else
    UNUSED(u1DecId);
    return SWDMX_SetInfo(u1Type, u4Para1, u4Para2, u4Para3);
#endif
}
#endif

#ifdef CC_DUAL_AUD_DEC_SUPPORT
extern UINT8 gSubDecId;
extern UINT8 gMainDecId;
static UINT8 _AudConvertDecId_Dual(UINT16 u2_comp_id, UINT8* u1DecId, UINT32 eType)
{
    *u1DecId = (UINT8)u2_comp_id;

    if ((eType == AUD_DEC_SET_TYPE_NFY_FCT)|| 
        ((eType == AUD_DEC_SET_TYPE_FMT)&&(u2_comp_id == AUD_DEC_SUB)))
    {
        // 1. AUD_DEC_SET_TYPE_NFY_FCT is called by MW when init.
        //    MW will set to all decoders (including SUB), but this doesn't means Dual mode is on.
        // 2. AUD_DEC_SET_TYPE_FMT case, Dual id is handled in AUD_SetDecType. Don't do twice.
        LOG(7, "(Dual)_AudConvertDecId_Dual eType=0x%x, DecID %d not changed\n", eType, *u1DecId);
        return 0;        
    }
    
    if (u2_comp_id == AUD_DEC_SUB)    // If MW wants to control Sub
    {
        if (gSubDecId != AUD_DEC_SUB) // If SubID has already been assigned.
        {
            *u1DecId = gSubDecId;     // Change DecID to SubID
        }
    }
    else if (u2_comp_id == AUD_DEC_MAIN)// If MW wants to control Main
    {
        if (gSubDecId != AUD_DEC_SUB) // If SubID has already been assigned.
        {
            *u1DecId = gMainDecId;
        }
    }    

    if ((eType == DUAL_AUD_DEC_DISCONNECT) && (u2_comp_id == AUD_DEC_SUB))
    {
        gSubDecId = AUD_DEC_SUB;  //Disconnect, we can reset this to off (default value)
    }
    LOG(7, "(Dual)_AudConvertDecId_Dual %d->%d\n", u2_comp_id, *u1DecId);
    return 0;
}

BOOL _AudGetMwIntendCtrl(UINT8 u1DecId)
{
    return _afgAudDecMwIntendCtrl[u1DecId];
}
#endif

#ifndef CC_DUAL_AUD_DEC_SUPPORT   
static UINT8 _AudConvertDecId(UINT16 u2_comp_id, UINT8* u1DecId)
{
    if (u2_comp_id > (AUD_DEC_NUM - 1))
    {
        *u1DecId = AUD_DEC_MAIN;
        if (u2_comp_id == AUD_DEC_PIP_SUB) //pseduo decoder id for audio in PIP sub mode
        {
            ////AUD_DrvSetPIPVideoPath(AUD_DEC_MAIN, 1); //PIP sub mode : 1 //PIP information is directly set by MW.
            return 0;
        }
        else
        {
            return 1;   //middleware set wrong decoder id
        }
    }
    ////AUD_DrvSetPIPVideoPath(AUD_DEC_MAIN, 0); //PIP main mode : 0    //PIP information is directly set by MW by AUD_DEC_SET_TYPE_PIPVIDEO_PATH_INFO.
    *u1DecId = (UINT8)u2_comp_id;
    return 0;
}
#endif

static void _AudMwNfyFunc(void *          pvNfyTag,
                          UINT8           u1DecId,
                          AUD_COND_NFY_T  eNfyCond,
                          UINT32          u4Data1,
                          UINT32          u4Data2)
{
    UINT32 u4NfyTag = 0;

    if ((INT32)eNfyCond >= (INT32)AUD_DEC_COND_MAX)
    {
        return; // Don't send the notify to mw
    }

#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)    //parson MM3
    {
        u1DecId = AUD_DEC_MAIN;
    }
#endif
#ifdef ENABLE_MULTIMEDIA
    // in multimedia playback, dmx_mm_mw needs to know the status of
    // audio driver. So here is a hook function.
    // in this function, it just assigns a value.
    // it would not affect the original behavior.
    if (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_MULTI_MEDIA ||
		(_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER && _fgMMPlusAD[u1DecId] ==TRUE))
    {
        MTAUDDEC_CB_T eAudCbType;
        eAudCbType.u4Arg1 = (UINT32)pvNfyTag;
        eAudCbType.u4Arg2 = (UINT32)u1DecId;
        eAudCbType.u4Arg3 = (UINT32)eNfyCond;
        eAudCbType.u4Arg4 = u4Data1;
        eAudCbType.u4Arg5 = u4Data2;
      #ifdef CC_53XX_SWDMX_V2
      #ifdef CC_FBM_TWO_FBP
        eAudCbType.u4Arg6 = (UINT32)_au1SwdmxSrcId[AUD_DEC_MAIN];
      #else
        eAudCbType.u4Arg6 = 0;
      #endif
      #endif
        _CB_PutEvent(CB_MTAL_AUDDEC_AUD_MW_DMX_NFY, sizeof(MTAUDDEC_CB_T), (void*)&eAudCbType);
    }
#endif

    u4NfyTag = ((UINT32)pvNfyTag & 0xF);

#ifdef CC_DUAL_AUD_DEC_SUPPORT    
    if (gSubDecId != AUD_DEC_SUB)
    {
        if (u1DecId != gSubDecId)
        {
            LOG(5, "(DUAL)Notify change id %d to AUD_DEC_MAIN for DUAL mode\n", u1DecId);                    
            u1DecId = AUD_DEC_MAIN; // MW always think MAIN is in Dec0
        }
        else if (u1DecId == gSubDecId) //If this notify is from Sub
        {
            LOG(5, "(Dual)Notify change %d to AUD_DEC_SUB\n");    
            u1DecId = AUD_DEC_SUB; // Change to SUB_ID because MW need this ID!
        }         
    }
    LOG(5,"(Dual)Notify - ID:%d, Type:0x%x, Cond:0x%x  \n",u1DecId,u4NfyTag,eNfyCond);  
#endif

    switch ((AUD_NFY_TAG_T)u4NfyTag)
    {
        case AUD_NFY_STREAM :
            // Currently use only main decoder
            if (_arAudMwNotifyInfo[u1DecId].pf_aud_dec_nfy != NULL)
            {
                MTAUDDEC_CB_T eAudCbType;
                eAudCbType.u4Arg1 = (UINT32)u1DecId;
                eAudCbType.u4Arg2 = (UINT32)_arAudMwNotifyInfo[u1DecId].pv_tag;
                eAudCbType.u4Arg3 = (UINT32)eNfyCond;
                eAudCbType.u4Arg4 = u4Data1;
                eAudCbType.u4Arg5 = u4Data2;
                if (eNfyCond == (AUD_COND_NFY_T)AUD_DEC_COND_AUD_INFO_CHG)
                {
                    eAudCbType.rAudDecInfo = *((_AUD_DEC_INFO_T*)u4Data1);
                }
                _CB_PutEvent(CB_MTAL_AUDDEC_AUD_MW_NFY, sizeof(MTAUDDEC_CB_T), (void*)&eAudCbType);
                LOG(3, "[AUD_NFY_STREAM] Dec(%d) AUD_COND_%s, data1(%d), data2(%d) done\n",
                    u1DecId, _paStrmNfyCond[eNfyCond + 1], u4Data1, u4Data2);
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //if ((_afgAudDecMwIntendCtrl[u1DecId]==0)&&(_afgAudDecMwIfRealCtrl[u1DecId]==1))
                if ((_afgAudDecMwIntendCtrl[u1DecId]==0)&&(AUD_MWCtrl_Get(u1DecId)==1))
                {
                    LOG(3, "Notification that auto-played by audio driver to MW, u1DecId = %d\n", u1DecId);
                    if ((u1DecId==AUD_DEC_AUX) && (eNfyCond==AUD_COND_CTRL_DONE) && (u4Data1==AUD_DEC_CTRL_STOP))
                    {
                        AUD_MWCtrl_Set(AUD_DEC_AUX,0);
                        _afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0;
                        LOG(3, "_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0\n");
                    }
                }
#endif
            }
            else
            {
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //if ((_afgAudDecMwIntendCtrl[u1DecId]==0)&&(_afgAudDecMwIfRealCtrl[u1DecId]==1))
                if ((_afgAudDecMwIntendCtrl[u1DecId]==0)&&(AUD_MWCtrl_Get(u1DecId)==1))
                {
                    LOG(3, "Ignore none-registered Notofication that auto-played by audio driver to MW, u1DecId = %d\n", u1DecId);
                    if ((u1DecId==AUD_DEC_AUX) && (eNfyCond==AUD_COND_CTRL_DONE) && (u4Data1==AUD_DEC_CTRL_STOP))
                    {
                        AUD_MWCtrl_Set(AUD_DEC_AUX,0);
                        _afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0;
                        LOG(3, "_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0\n");
                    }
                    if ((u1DecId==AUD_DEC_AUX) && (_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] ==0))
                    {
                        LOG(3, "u1DecId = %d, eNfyCond = %d, u4Data1 = %\n", u1DecId, eNfyCond, u4Data1);
                    }
                    return;
                }
#endif
                LOG(3, "Not Registered Notofication to MW, u1DecId = %d\n", u1DecId);
            }
            break;

        case AUD_NFY_HP :
            if (_arAudHPMwNotifyInfo[AUD_DEC_MAIN].pf_hdph_detect_nfy != NULL)
            {
                MTAUDDEC_CB_T eAudCbType;
                eAudCbType.u4Arg1 = (UINT32)AUD_DEC_MAIN;
                eAudCbType.u4Arg2 = (UINT32)_arAudHPMwNotifyInfo[AUD_DEC_MAIN].pv_nfy_tag;
                eAudCbType.u4Arg3 = (UINT32)eNfyCond;
                eAudCbType.u4Arg4 = u4Data1;
                eAudCbType.u4Arg5 = u4Data2;
                _CB_PutEvent(CB_MTAL_AUDDEC_AUD_MW_HEADPHONE_DETECT_NFY, sizeof(MTAUDDEC_CB_T), (void*)&eAudCbType);
            }
            else
            {
                LOG(5, "HP detection callback function is not ready!\n");
            }
            break;
        case AUD_NFY_YW :
            if (_arAudYWMwNotifyInfo[AUD_DEC_MAIN].pf_upload_data_nfy != NULL)
            {
                MTAUDDEC_CB_T eAudCbType;
                eAudCbType.u4Arg1 = (UINT32)AUD_DEC_MAIN;
                eAudCbType.u4Arg2 = (UINT32)_arAudYWMwNotifyInfo[AUD_DEC_MAIN].pv_nfy_tag;
                eAudCbType.u4Arg3 = (UINT32)eNfyCond;
                eAudCbType.u4Arg4 = u4Data1;
                eAudCbType.u4Arg5 = u4Data2;
                _CB_PutEvent(CB_MTAL_AUDDEC_AUD_MW_UPLOAD_DATA_NFY, sizeof(MTAUDDEC_CB_T), (void*)&eAudCbType);
            }
            else
            {
                LOG(5, "YW upload callback function is not ready!\n");
            }
            break;
        default:
            break;
    }
}

static TV_AUD_SYS_T _GetTvSysFormat(UINT8 u1DecId)
{
    TV_AUD_SYS_T eTvSys = SV_NONE_DETECTED;
    //_au4TvSysMask[u1DecId] = AUD_DspGetAtvTvSys();
    UINT8 u1TmpID;

    UNUSED(u1TmpID);
    UNUSED(_GetTvSysFormat);

#ifdef CC_DUAL_AUD_DEC_SUPPORT
    _AudConvertDecId_Dual(u1DecId, &u1TmpID, 0xFFFFFFFF);
    u1DecId = u1TmpID; //Can not change 
#endif      

    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    if (_IsMaskSetToA2Sys(u1DecId))
    {
        _fgForceToPal = 0;
        if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_B) == TV_SYS_MASK_B) &&
            ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_G) == TV_SYS_MASK_G))
        {
            eTvSys =  SV_A2_BG;
        }
        else if ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_I) == TV_SYS_MASK_I)
        {
            eTvSys =  SV_PAL_I;
            _fgForceToPal = 1;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_L) == TV_SYS_MASK_L))
        {
            eTvSys = SV_SECAM_L;
            _fgForceToPal = 1;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_L_PRIME) == TV_SYS_MASK_L_PRIME))
        {
            eTvSys = SV_SECAM_L_PLUM;
            _fgForceToPal = 1;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2) == AUDIO_SYS_MASK_FM_A2))
        {
            eTvSys = SV_A2_DK3;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2_DK1) == AUDIO_SYS_MASK_FM_A2_DK1))
        {
            eTvSys = SV_A2_DK1;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_aud_sys_mask & AUDIO_SYS_MASK_FM_A2_DK2) == AUDIO_SYS_MASK_FM_A2_DK2))
        {
            eTvSys = SV_A2_DK2;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_M) == TV_SYS_MASK_M))
        {
            eTvSys = SV_NTSC_M;
        }
    }
    else if (_IsMaskSetToPalSys(u1DecId))
    {
        if ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_I) == TV_SYS_MASK_I)
        {
            eTvSys =  SV_PAL_I;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_D) == TV_SYS_MASK_D) &&
            ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_K) == TV_SYS_MASK_K))
        {
            eTvSys = SV_PAL_DK;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_B) == TV_SYS_MASK_B) &&
                 ((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_G) == TV_SYS_MASK_G))
        {
            eTvSys = SV_PAL_BG;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_L) == TV_SYS_MASK_L))
        {
            eTvSys = SV_SECAM_L;
        }
        else if (((_arTvAudSysMask[u1DecId].ui4_tv_sys_mask & TV_SYS_MASK_L_PRIME) == TV_SYS_MASK_L_PRIME))
        {
            eTvSys = SV_SECAM_L_PLUM;
        }
    }
    else if (_IsMaskSetToMtsSys(u1DecId))
    {
        eTvSys = SV_MTS;
    }
    else if (_IsMaskSetToFmfmSys(u1DecId))
    {
        eTvSys = SV_FM_FM;
    }

    return eTvSys;
}

static void _AudSetDetTvSrcMode(UINT8 u1DecId)
{
    if ((Aud_GetTvSystemMask(u1DecId) & TV_SYS_MASK_L) == TV_SYS_MASK_L)
    {
        DSP_SetDetTvSrcMode(SV_SECAM_L);
        AUD_DspSetDetectDefaultMode(4); // SECAM-L
    }
    else if ((Aud_GetTvSystemMask(u1DecId) & TV_SYS_MASK_L_PRIME) == TV_SYS_MASK_L_PRIME)
    {
        DSP_SetDetTvSrcMode(SV_SECAM_L_PLUM);
        AUD_DspSetDetectDefaultMode(4); // SECAM-L or SECAM-L'
    }
    else
    {
        DSP_SetDetTvSrcMode(SV_SECAM_DK);
        AUD_DspSetDetectDefaultMode(0); // PAL-DK
    }
}

static void _AudGetCtrl(UINT8 u1DecId, AUD_DEC_CTRL_T * peAudCtrl)
{
    *peAudCtrl = _aeCtrlSetting[u1DecId];
}

static void _GetAudPTS(UINT8 u1DecId, UINT64 * prPTS)
{
    UINT32 u4AudPTS;

    AUD_GetAudioPts(u1DecId, &u4AudPTS);

    *prPTS = (UINT64)u4AudPTS;

}

static void _GetAudFIFO_SZ(UINT8 u1DecId, UINT32 * prAfifoSz)
{
    UINT32 u4Rp;
    UINT32 u4Wp;
    UINT32 u4DataSZ;

    AUD_GetRWPtr(u1DecId, &u4Rp, &u4Wp, &u4DataSZ);

    *prAfifoSz = (UINT32)u4DataSZ;

}

//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
static void _GetAudUploadInfo(AUD_DEC_UPLOAD_INFO_T* prUploadInfo)
{
    AUD_GetUploadInfo(&(prUploadInfo->u4Pts),&(prUploadInfo->u4Addr),&(prUploadInfo->u4Size));
}
static void _GetAudYWUploadInfo(AUD_DEC_BUFFER_INFO_T* prUploadInfo)
{
    AUD_GetYWUploadInfo(&(prUploadInfo->ui4_buffer_start_Addr),&(prUploadInfo->ui4_bufer_Size));
}
static void _GetAUDUploadPCMInfo(AUD_DEC_UPLOAD_PCM_INFO_T* prUploadPcmInfo)
{
    AUD_GetUploadPCMInfo(&(prUploadPcmInfo->ui4_pcm_bits), &(prUploadPcmInfo->ui4_pcm_channels), &(prUploadPcmInfo->ui4_pcm_sample_rate));
}
//#endif

static void _GetFormat(UINT8 u1DecId, AUD_DEC_FMT_INFO_T * prFormatInfo)
{
    prFormatInfo->e_fmt = _arAudDecFmt[u1DecId].e_fmt;
    prFormatInfo->pv_info = _arAudDecFmt[u1DecId].pv_info;
}

#ifdef CC_AUD_SETUP_MELODY
static void _GetPowerOnMusicVolume(UINT8 u1DecId, AUD_DEC_VOLUME_INFO_T * prChannelVol)
{
    AUD_DEC_ID_VALIDATE(u1DecId); //Fix Klocwork warning
    if (prChannelVol->e_vol_type == AUD_DEC_ALL_CH)
    {
        prChannelVol->u.ui1_level = AUD_GetSetupMelodyVol();
		LOG(3," AUD_DEC_GET_TYPE_POWER_ON_OFF_MUSIC_VOLUME = %d",prChannelVol->u.ui1_level);

    }
}
#endif
static void _GetVolume(UINT8 u1DecId, AUD_DEC_VOLUME_INFO_T * prChannelVol)
{
    AUD_DEC_ID_VALIDATE(u1DecId); //Fix Klocwork warning

    if (prChannelVol->e_vol_type == AUD_DEC_ALL_CH)
    {
        prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolMaster;
    }
    else if (prChannelVol->e_vol_type == AUD_DEC_TEST_TONE)
    {
		prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolTone;
    }
    else /* AUD_DEC_INDIVIDUAL_CH */
    {
        switch (prChannelVol->u.t_ch_vol.e_out_port)
        {
        case AUD_DEC_OUT_PORT_2_CH:       // This is for dmx channel
        case AUD_DEC_OUT_PORT_SPEAKER: // doing the same thing with "AUD_DEC_OUT_PORT_2_CH"
            switch (prChannelVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolDmxL;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolDmxR;
                    break;
                default:
                    break;
            }
            break;
        case AUD_DEC_OUT_PORT_5_1_CH:
            switch (prChannelVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolL;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolR;
                    break;
                case AUD_DEC_LS_REAR_LEFT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolSL;
                    break;
                case AUD_DEC_LS_REAR_RIGHT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolSR;
                    break;
                case AUD_DEC_LS_CENTER:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolC;
                    break;
                case AUD_DEC_LS_SUB_WOOFER:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolSW;
                    break;
                default:
                    break;
            }
            break;
        case AUD_DEC_OUT_PORT_SPDIF:
            // Do nothing
            break;
        case AUD_DEC_OUT_PORT_2_CH_BY_PASS:
            switch (prChannelVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolBypassL;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    prChannelVol->u.t_ch_vol.ui1_level = _arChVol[u1DecId].u1VolBypassR;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
        }
    }
}

static void _GetVolumeEx(UINT8 u1DecId, AUD_DEC_VOLUME_INFO_EX_T * prChannelVol)
{
    AUD_DEC_VOLUME_INFO_T rVolInfo;

    AUD_DEC_ID_VALIDATE(u1DecId); //Fix Klocwork warning

   if (prChannelVol->e_vol_type == AUD_DEC_ALL_CH)
    {
        switch (prChannelVol->e_out_port)
        {
        case AUD_DEC_OUT_PORT_SPEAKER:
            prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolL;
            break;
        case AUD_DEC_OUT_PORT_HEADPHONE:
            prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolDmxL;
            break;
        default:
            prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolMaster;
            break;
        }
    }
    else if (prChannelVol->e_vol_type == AUD_DEC_TEST_TONE)
    {
		prChannelVol->u.ui1_level = _arChVol[u1DecId].u1VolTone;
    }
    else if (prChannelVol->e_vol_type == AUD_DEC_INDIVIDUAL_CH)
    {
        rVolInfo.e_vol_type = AUD_DEC_INDIVIDUAL_CH;
        rVolInfo.u.t_ch_vol.e_out_port = prChannelVol->e_out_port;
        rVolInfo.u.t_ch_vol.e_ls = prChannelVol->u.t_ch_vol.e_ls;
        _GetVolume(u1DecId, &rVolInfo);
        prChannelVol->u.t_ch_vol.ui1_level = rVolInfo.u.t_ch_vol.ui1_level;
    }
}

static void _GetCustomerMusicFlag(UINT8 u1DecId, AUD_DEC_AUD_CLIP_CUSTIOM_T *peMusicFlag)
{
    UNUSED(u1DecId);

    switch (peMusicFlag->e_drv_clip_custom_setting_type)
    {
        case AUD_DEC_CLIP_CUSTOM_SETTING_TYPE_POWER_ON:
            if (bApiEepromReadByte(EEP_BOOT_MUSIC_ONOFF) & AUD_POWER_ON_MUSIC_MASK)
            {
                peMusicFlag->e_drv_clip_custom_setting_mode = AUD_DEC_CLIP_CUSTOM_SETTING_MODE_ON;
            }
            else
            {
                peMusicFlag->e_drv_clip_custom_setting_mode = AUD_DEC_CLIP_CUSTOM_SETTING_MODE_OFF;
            }
            break;
        case AUD_DEC_CLIP_CUSTOM_SETTING_TYPE_POWER_OFF:

            if (bApiEepromReadByte(EEP_BOOT_MUSIC_ONOFF) & AUD_POWER_OFF_MUSIC_MASK)
            {
                peMusicFlag->e_drv_clip_custom_setting_mode = AUD_DEC_CLIP_CUSTOM_SETTING_MODE_ON;
            }
            else
            {
                peMusicFlag->e_drv_clip_custom_setting_mode = AUD_DEC_CLIP_CUSTOM_SETTING_MODE_OFF;
            }
            break;
        default:
            return;
    }
}

static void _GetMute(UINT8 u1DecId, AUD_DEC_MUTE_TYPE_T * peMuteType)
{
    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    *peMuteType = _aeMuteType[u1DecId];
}
static void _GetSpdif(UINT8 u1DecId, AUD_DEC_SPDIF_TYPE_T * peSpdif)
{
    UNUSED(u1DecId);
    *peSpdif = _eSpdif;
}
static void _GetHdcdfilter(UINT8 u1DecId, AUD_DEC_HDCD_FLTR_T * peHdcdFilter)
{
    *peHdcdFilter = _aeHdcdFilter[u1DecId];
}
static void _GetLsMode(UINT8 u1DecId, AUD_DEC_CH_LS_MODE_T * prLsMode)
{
    AUD_DEC_LS_T eLsChannel;
    eLsChannel = prLsMode->e_ls;
    prLsMode->e_mode = _aeLsMode[u1DecId][eLsChannel];
}
static void _GetSoundEffect(UINT8 u1DecId, AUD_DEC_SE_INFO_T * prSoundEffect)
{
	ASSERT((prSoundEffect->e_se_type >= 0) && (prSoundEffect->e_se_type < AUD_DEC_SE_NUM));
    prSoundEffect->pv_info = _arSeInfo[u1DecId][prSoundEffect->e_se_type];
}
static void _GetDownMix(UINT8 u1DecId, AUD_DEC_DOWNMIX_MODE_T * peDownmixMode)
{
    *peDownmixMode = _aeDownmixMode[u1DecId];
}

static void _GetMatrix(UINT8 u1DecId, AUD_DEC_MATRIX_MODE_T * peMatrixMode)
{
    *peMatrixMode = _aeMatrixMode[u1DecId];
}

static void _GetDrc(UINT8 u1DecId, AUD_DEC_DRC_T * peDrc)
{
    *peDrc = _aeDrc[u1DecId];
}

static void _GetOutPort(UINT8 u1DecId, UINT32 * pu4OutPort)
{
    UNUSED(u1DecId);

    if (u1DecId != AUD_DEC_THIRD)
    {
        *pu4OutPort = _u4OutPort;
    }
    else
    {
        *pu4OutPort = _u4AdOutPort;
    }
}

static void _GetCompressionMode(UINT8 u1DecId, AUD_DEC_CMPSS_MODE_T * peCmprssMode)
{
    *peCmprssMode = _aeCmprssMode[u1DecId];
}

static void _GetAgcFlag(UINT8 u1DecId, BOOL * pfgAgcFlag)
{
    *pfgAgcFlag = _afgAgcFlag[u1DecId];
}

static void _GetChlDelay(UINT8 u1DecId, AUD_DEC_CHNL_DELAY_T * prChlDelay)
{
    if (prChlDelay->e_ch_delay_type == AUD_DEC_CHNL_DELAY_TYPE_ALL)
    {
        VERIFY((prChlDelay->e_out_port) < (AUD_DEC_OUT_PORT_SPEAKER + 1));
        prChlDelay->u.ui2_delay = _aau2MasterDelay[u1DecId][prChlDelay->e_out_port];
    }
    else
    {
        switch (prChlDelay->e_out_port)
        {
            case AUD_DEC_OUT_PORT_OFF:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_OFF],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_2_CH:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_2_CH],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_5_1_CH:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_5_1_CH],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_SPDIF:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_SPDIF],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_2_CH_BY_PASS:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_2_CH_BY_PASS],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_SPEAKER:
                x_memcpy((VOID *)VIRTUAL((UINT32)&(prChlDelay->u.t_ind_delay)), (const VOID *)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_SPEAKER],
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            default:
                break;
        }
    }
}

static void _GetCopyProtect(UINT8 u1DecId, AUD_DEC_COPY_PROTECT_T * peCopyProtect)
{
    *peCopyProtect = _aeCopyProtect[u1DecId];
}

static void _GetMts(UINT8 u1DecId, AUD_DEC_MTS_T * peMts)
{
    AUD_DEC_MTS_T eMtsMode = AUD_DEC_MTS_UNKNOWN;
    AUD_SOUND_MODE_T eSoundMode;

    eSoundMode = AUD_DspGetSoundMode(u1DecId);

    switch(eSoundMode)
    {
        case AUD_SOUND_MODE_UNKNOWN:
            eMtsMode = AUD_DEC_MTS_UNKNOWN;
            break;
        case AUD_SOUND_MODE_MONO:
            eMtsMode = AUD_DEC_MTS_MONO;
            break;
        case AUD_SOUND_MODE_STEREO:
            eMtsMode = AUD_DEC_MTS_STERRO;
            break;
        case AUD_SOUND_MODE_SUB_LANG:
            eMtsMode = AUD_DEC_MTS_SUB_LANG;
            break;
        case AUD_SOUND_MODE_DUAL1:
            eMtsMode = AUD_DEC_MTS_DUAL1;
            break;
        case AUD_SOUND_MODE_DUAL2:
            eMtsMode = AUD_DEC_MTS_DUAL2;
            break;
        case AUD_SOUND_MODE_NICAM_MONO:
            eMtsMode = AUD_DEC_MTS_NICAM_MONO;
            break;
        case AUD_SOUND_MODE_NICAM_STEREO:
            eMtsMode = AUD_DEC_MTS_NICAM_STEREO;
            break;
        case AUD_SOUND_MODE_NICAM_DUAL1:
            eMtsMode = AUD_DEC_MTS_NICAM_DUAL1;
            break;
        case AUD_SOUND_MODE_NICAM_DUAL2:
            eMtsMode = AUD_DEC_MTS_NICAM_DUAL2;
            break;
        default:
            eMtsMode = AUD_DEC_MTS_UNKNOWN;
            break;
    }

    *peMts = eMtsMode;
}

static void _GetMaxDelay(UINT8 u1DecId, UINT8 * pu1Delay)
{
    UNUSED(u1DecId);
    *pu1Delay = AUD_MAX_DELAY_IN_MS;
}

static void _GetUserEqBandNum(UINT8 u1DecId, UINT8 * pu1BandNum)
{
    UNUSED(u1DecId);
    *pu1BandNum = AUD_DspGetUserEqBandNum();
}

static void _GetUserEqLevel(UINT8 u1DecId, AUD_DEC_SE_QA_USER_LVL_T * prEqLevel)
{
    UNUSED(u1DecId);

    VERIFY(prEqLevel->ui1_band_idx < CH_EQ_BAND_NUM);
    VERIFY(prEqLevel->e_eq_type < (AUD_DEC_EQ_BBE_AV + 1));
    if (prEqLevel->b_is_level_ex)
    {
        prEqLevel->i2_level_ex = _ai2UserEqCfg[prEqLevel->e_eq_type][prEqLevel->ui1_band_idx];
    }
    else
    {
        prEqLevel->i1_level = _ai2UserEqCfg[prEqLevel->e_eq_type][prEqLevel->ui1_band_idx];
    }

/*
    if (!AUD_DspGetUserDefineEqCfg(prEqLevel->ui1_band_idx, &prEqLevel->i1_level))
    {
        LOG(3, "Error EQ number\n");
    }
*/
}

static void _GetUserEqInfo(UINT8 u1DecId, AUD_DEC_SE_QA_USER_INFO_BAND_T * prEqInfo)
{
    UNUSED(u1DecId);

    if (prEqInfo != NULL)
    {
        AUD_DspGetEqBandInfo((AUD_EQ_BAND_INFO_T *)prEqInfo);
    }
}

static void _GetAudInfo(UINT8 u1DecId, AUD_DEC_AUD_INFO_T * prAudInfo)
{
    AUD_DEC_STREAM_FROM_T  rAudDecStreamFrom;
    AUD_DEC_FMT_INFO_T     rAudDecFormat;
    AUD_DEC_INFO_T rAudInfo;
    UINT8  u1Acmode;
    UINT8 u1Ac3DecType;
    UINT8  u1SmpRate;
    UINT32 u4DataRate = 0;
    UINT32 u4LPCMType = 0;

#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
    // Get decoding format                            // FIXME !!
    AUD_DRVGetDecodeType(u1DecId, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
    prAudInfo->e_aud_fmt = rAudDecFormat.e_fmt;
    UNUSED(rAudDecStreamFrom);
#else
    // Get decoding format                            // FIXME !!
    if (u1DecId == AUD_DEC_MAIN)
    {
        if (_IsDualDecMode())
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DRVGetDecodeType(AUD_DSP0, AUD_DEC_AUX, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#else
            AUD_DRVGetDecodeType(AUD_DEC_AUX, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#endif
            u1DecId = AUD_DEC_AUX;
        }
        else
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DRVGetDecodeType(AUD_DSP0, u1DecId, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#else
            AUD_DRVGetDecodeType(u1DecId, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#endif
        }
    }
    else
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        AUD_DRVGetDecodeType(AUD_DSP0, u1DecId, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#else
        AUD_DRVGetDecodeType(u1DecId, &rAudDecStreamFrom, (AUD_FMT_T *)&(rAudDecFormat.e_fmt));
#endif
    }

    prAudInfo->e_aud_fmt = rAudDecFormat.e_fmt;
    UNUSED(rAudDecStreamFrom);
#endif

    UNUSED(rAudInfo);

    switch (rAudDecFormat.e_fmt)
    {
    case AUD_DEC_FMT_TV_SYS:
        rAudDecFormat.e_fmt = AUD_DEC_FMT_TV_SYS;
        prAudInfo->ui4_sample_rate = 32000;
        prAudInfo->e_aud_type = (AUD_DEC_AUD_TYPE_T)DSP_GetAudChlInfo(AUD_DEC_AUX); // FIXME ! Hardcode ID
        break;
    case AUD_DEC_FMT_MTS: //ATV
        prAudInfo->ui4_sample_rate = 32000;
        prAudInfo->e_aud_type = (AUD_DEC_AUD_TYPE_T)DSP_GetMtsSrcTyp(AUD_DEC_MAIN); // FIXME ! Hardcode ID
        break;
    case AUD_DEC_FMT_MPEG:
    case AUD_DEC_FMT_MP3:
        AUD_GetDtvAudInfo(u1DecId, &rAudInfo);
        prAudInfo->ui4_sample_rate = rAudInfo.ui4_sample_rate;
        u4DataRate = rAudInfo.ui4_data_rate;
        prAudInfo->e_dec_type = (AUD_DEC_DECODE_TYPE_T) rAudInfo.e_dec_type;

        switch (rAudInfo.e_aud_type)
        {
        case AUD_TYPE_MONO:
            prAudInfo->e_aud_type = AUD_DEC_TYPE_MONO;
            break;
        case AUD_TYPE_DUAL_MONO:
            prAudInfo->e_aud_type = AUD_DEC_TYPE_DUAL_MONO;
            break;
        default:
            prAudInfo->e_aud_type = AUD_DEC_TYPE_STEREO;
            break;
        }
        break;
    case AUD_DEC_FMT_PCM:
        prAudInfo->ui4_sample_rate = 44100;
        prAudInfo->e_aud_type = AUD_DEC_TYPE_STEREO;
        break;
    case AUD_DEC_FMT_AC3: // DTV
        // Get audio information
        AUD_GetStreamInfo_AC3(u1DecId, &u1Acmode, &u1Ac3DecType, &u1SmpRate, &u4DataRate);
        switch (u1Acmode)
        {
        case 0: // 1+1
            prAudInfo->e_aud_type = AUD_DEC_TYPE_DUAL_MONO;
            break;
        case 1: // 1/0
            prAudInfo->e_aud_type = AUD_DEC_TYPE_MONO;
            break;
        case 2: // 2/0
            prAudInfo->e_aud_type = AUD_DEC_TYPE_STEREO;
            break;
        case 3: // 3/0
            prAudInfo->e_aud_type = AUD_DEC_TYPE_3_0;
            break;
        case 4: // 2/1
            prAudInfo->e_aud_type = AUD_DEC_TYPE_SURROUND_2CH;
            break;
        case 5: // 3/1
            prAudInfo->e_aud_type = AUD_DEC_TYPE_SURROUND;
            break;
        case 6: // 2/2
            prAudInfo->e_aud_type = AUD_DEC_TYPE_4_0;
            break;
        case 7: // 3/2
        	  prAudInfo->e_aud_type = AUD_DEC_TYPE_5_0;
            break;
        case 0x11: // 1/0.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_MONO_LFE;
            break;
        case 0x12: // 2/0.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_STEREO_LFE;
            break;
        case 0x13: // 3/0.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_3_1;
            break;
        case 0x14: // 2/1.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_SURROUND_2CH_LFE;
            break;
        case 0x15: // 3/1.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_SURROUND_LFE;
            break;
        case 0x16: // 2/2.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_4_1;
            break;
        case 0x17: // 3/2.L
            prAudInfo->e_aud_type = AUD_DEC_TYPE_5_1;
            break;
        case 8: // unknown
            prAudInfo->e_aud_type = AUD_DEC_TYPE_UNKNOWN;
            break;
        default: //
            LOG(5, "%s,", "Error acmode");
            prAudInfo->e_aud_type = AUD_DEC_TYPE_OTHERS;
            break;
        }

        switch(u1Ac3DecType & 0x01)
        {
        case 0x00:
	     prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_AC3;
	     break;
	 case 0x01:
	     prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_EAC3;
	     break;
	 default:
	     prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_UNKNOWN;
	     break;
        }

        switch (u1SmpRate)
        {
        case 13: // 48 KHz
            prAudInfo->ui4_sample_rate = 48000;
          break;
        case 8: // 44.1 KHz
            prAudInfo->ui4_sample_rate = 44100;
            break;
        case 3: // 32 KHz
            prAudInfo->ui4_sample_rate = 32000;
            break;
        default: // Error sample rate
            prAudInfo->ui4_sample_rate = 48000;
            LOG(5, "Unknown sample rate");
            break;
        }
        break;

    case AUD_DEC_FMT_LPCM:

        u4LPCMType = AUD_GetLPCMType();

        if(u4LPCMType == 0x101)
        {
        	prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_LPCM_ALAW;
        }
        else if(u4LPCMType == 0x301)
        {
        	prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_LPCM_ULAW;
        }
        else
        {
        	prAudInfo->e_dec_type = AUD_DEC_DECODE_TYPE_UNKNOWN;
        }
        break;
    case AUD_DEC_FMT_AAC: // DTV
        // Get audio information
        AUD_GetDtvAudInfo(u1DecId,&rAudInfo);
        //prAudInfo->e_aud_fmt = (AUD_DEC_FMT_T)rAudInfo.e_aud_fmt;
        prAudInfo->ui4_sample_rate = rAudInfo.ui4_sample_rate;
        prAudInfo->e_aud_type = (AUD_DEC_AUD_TYPE_T) rAudInfo.e_aud_type;
        prAudInfo->e_dec_type = (AUD_DEC_DECODE_TYPE_T) rAudInfo.e_dec_type;
        break;
    default:
        u1SmpRate = 0;
        break;
    }
    //Printf("%d Kbps,",u2DataRate);
    prAudInfo->ui4_data_rate = u4DataRate ;

    // FIXME // TODO
    prAudInfo->ui1_bit_depth = 24;
}

/*
static void _GetMemBuffer(UINT8 u1DecId, AUD_DEC_MEM_BUFFER_INFO_T * prMemBuf)
{
    x_memcpy((VOID *)VIRTUAL(prMemBuf), &_arMemBuf[u1DecId], sizeof(AUD_DEC_MEM_BUFFER_INFO_T));
}
*/

static void _GetTvSysMask(UINT8 u1DecId, UINT32* pu4Mask)
{
    AUD_DEC_TV_AUD_SYS_T rAudTvSys;

    rAudTvSys.ui4_tv_sys_mask = 0; //initialization, Fix Klocwork warning

    AUD_DspGetAtvTvSys((MW_TV_AUD_SYS_T *)(&rAudTvSys));
    *pu4Mask = rAudTvSys.ui4_tv_sys_mask;
}

static void _GetTvAudSysMask(UINT8 u1DecId, AUD_DEC_TV_AUD_SYS_T* parMask)
{
    UNUSED(u1DecId);
    AUD_DspGetAtvTvSys((MW_TV_AUD_SYS_T *)parMask);
}

static INT32 _GetTvSysCapability(UINT8 u1DecId, UINT32* pu4Capability)
{
    UNUSED(u1DecId);
    // FIXME !!
    if (IS_TUNER_MULTI_SYS())
    {
        *pu4Capability = TV_SYS_MASK_A        |
                         TV_SYS_MASK_B        |
                         TV_SYS_MASK_C        |
                         TV_SYS_MASK_D        |
                         TV_SYS_MASK_E        |
                         TV_SYS_MASK_F        |
                         TV_SYS_MASK_G        |
                         TV_SYS_MASK_H        |
                         TV_SYS_MASK_I        |
                         TV_SYS_MASK_J        |
                         TV_SYS_MASK_K        |
                         TV_SYS_MASK_K_PRIME  |
                         TV_SYS_MASK_L        |
                         TV_SYS_MASK_L_PRIME  |
                         TV_SYS_MASK_N ;
    }
    else // IS_TUNER_UNI_SYS()
    {
        *pu4Capability = TV_SYS_MASK_M;
    }

    return TRUE;
}

static void _GetAudClip(UINT8 u1DecId, AUD_DEC_AUD_CLIP_INFO_T* parAudClip)
{
    *parAudClip = _arAudClipInfo[u1DecId];
}

static void _GetPrologic2(UINT8 u1DecId, AUD_DEC_PL2_CTRL_INFO_T* prPL2)
{
    x_memcpy((VOID *)VIRTUAL((UINT32)prPL2), &_rPrologic2Type, sizeof(AUD_DEC_PL2_CTRL_INFO_T));
    if ((prPL2->e_ctrl == AUD_DEC_PL2_CTRL_DIMENSION) ||
        (prPL2->e_ctrl == AUD_DEC_PL2_CTRL_C_WIDTH))
    {
        prPL2->u.t_pl2_val.ui1_min_val = 0;
        prPL2->u.t_pl2_val.ui1_max_val = 0xf;
    }
}

static INT32 _GetCapability(UINT8 u1DecId, AUD_DEC_CAPABILITY_INFO_T * prCapability)
{
    prCapability->ui4_fmt = AUD_DspGetCapability(0);        //first 32bits
    prCapability->ui4_fmt_ext = AUD_DspGetCapability(1);    //second 32bits

    if (u1DecId == AUD_DEC_MAIN)
    {

        prCapability->ui4_channel_type = (AUD_DEC_CAP_TYPE_MONO      |
                                          AUD_DEC_CAP_TYPE_DUAL_MONO |
                                          AUD_DEC_CAP_TYPE_STEREO    |
                                          AUD_DEC_CAP_TYPE_5_1 );
    }
    else
    {
        prCapability->ui4_channel_type = (AUD_DEC_CAP_TYPE_MONO      |
                                          AUD_DEC_CAP_TYPE_DUAL_MONO |
                                          AUD_DEC_CAP_TYPE_STEREO );
    }

    prCapability->ui4_sound_effect = (AUD_DEC_CAP_SE_TRIM        |
                                      AUD_DEC_CAP_SE_DELAY       |
                                      AUD_DEC_CAP_SE_SUPERBASS   |
                                      AUD_DEC_CAP_SE_EQUALIZER   |
                                      AUD_DEC_CAP_SE_REVERB      |
                                      AUD_DEC_CAP_SE_BASS        |
                                      AUD_DEC_CAP_SE_POSTDRC     |
                                      AUD_DEC_CAP_SE_VOLUME);

    prCapability->ui4_downmix_mode = (AUD_DEC_CAP_DM_OFF         |
                                      AUD_DEC_CAP_DM_LT_RT       |
                                      AUD_DEC_CAP_DM_STEREO      |
                                      AUD_DEC_CAP_DM_VIR_SURR);

#ifdef CC_AUD_LINE_OUT_ADJ
    prCapability->b_is_outport_adj =  TRUE;
#else
    prCapability->b_is_outport_adj =  FALSE;
#endif

    return RMR_OK;
}

static void _GetDolbyBanner(UINT8 u1DecId, BOOL * pfgMode)
{
    *pfgMode = _afgDolbyBanner[u1DecId];
}

static void _GetOutPortAdj(UINT8 u1DecId, AUD_DEC_OUTPORT_ADJ_T * prOutportAdj)
{
#ifdef CC_AUD_LINE_OUT_ADJ
    switch (prOutportAdj->e_out_port)
    {
        case AUD_DEC_OUT_PORT_2_CH:
        case AUD_DEC_OUT_PORT_SPDIF:
            prOutportAdj->b_is_fixed = FALSE;
            break;
        case AUD_DEC_OUT_PORT_5_1_CH:
        case AUD_DEC_OUT_PORT_SPEAKER:
            prOutportAdj->b_is_fixed = TRUE;
            break;
        default:
            prOutportAdj->b_is_fixed = TRUE;
            break;
    }
#else
    prOutportAdj->b_is_fixed = TRUE;
#endif
}

static INT32 _GetAvSyncMode(UINT8 u1DecId, AUD_DEC_AV_SYNC_INFO_T *eSyncMode)
{
    AV_SYNC_MODE_T eMode;

    eMode = AUD_GetAvSynMode(u1DecId);

    if (eMode == AV_SYNC_SLAVE)
    {
        *eSyncMode = AUD_DEC_AV_SYNC_AUD_SLAVE;
    }
    else if (eMode == AV_SYNC_FREE_RUN)
    {
        *eSyncMode = AUD_DEC_AV_SYNC_FREE_RUN;
    }
    else if (eMode == AV_SYNC_AUDIO_MASTER)
    {
        *eSyncMode = AUD_DEC_AV_SYNC_AUD_MASTER;
    }
    else
    {
        return RMR_DRV_INV_SET_INFO;
    }

    return RMR_OK;
}

//BOOL _fgMMAudioOnly = FALSE;
static void _AudSetMMAudioOnly(UINT8 u1DecId, BOOL fgMMAudioOnly)
{
    UNUSED(u1DecId);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
	AUD_SetMMAudioOnly(u1DecId, fgMMAudioOnly);
#else
	AUD_SetMMAudioOnly(fgMMAudioOnly);
#endif
	//_fgMMAudioOnly = fgMMAudioOnly;
}


#ifdef CC_AUD_KARAOKE_MODE_SUPPORT
static void _GetMultimediaMode(UINT8 u1DecId, AUD_DEC_MM_MODE_T *eMultimediaMode)
{
    AUD_MM_MODE_T eMode;

    eMode = AUD_GetMultimediaMode(UINT8 u1DecId);

    switch (eMode)
    {
    case AUD_MM_MODE_NORMAL:
        *eMultimediaMode = AUD_DEC_MM_MODE_NORMAL;
        break;
    case AUD_DEC_MM_MODE_KARAOKE:
        *eMultimediaMode = AUD_DEC_MM_MODE_KARAOKE;
        break;
    default:
        break;
    }
}
#endif

#ifdef CC_AUD_BBE_SUPPORT
static void _GetBbeMode(UINT8 u1DecId, AUD_DEC_BBE_MODE_T * eBbeMode)
{
    UNUSED(u1DecId);

    *eBbeMode = _aeBbeMode;
}
#endif

static void _GetHeadphonePCond(UINT8 u1DecId, AUD_DEC_HEADPHONE_COND_T * eHPCond)
{
    *eHPCond = (AUD_DEC_HEADPHONE_COND_T)AUD_GetHeadphoneCond(u1DecId);
    // *eHPCond = AUD_DEC_HEADPHONE_COND_UNPLUGED;  //always return UNPLUGED
}

static void _GetMuteEx(UINT8 u1DecId, AUD_DEC_MUTE_EX_INFO_T * eMuteEx)
{
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
    *eMuteEx = _arAudMuteEx[AUD_DEC_MAIN];
#else
#ifdef CC_DUAL_AUD_DEC_SUPPORT
    if (u1DecId == AUD_DEC_SUB) 
        u1DecId = AUD_DEC_MAIN; //Please think how to handle this, MW may get this after Disconnect (Sub alread Disconnet!!!!)
#endif
    AUD_DEC_ID_VALIDATE_3(u1DecId);     //parson MM3
    *eMuteEx = _arAudMuteEx[u1DecId];
#endif
}

EXTERN void VDP_TriggerAudReceive(UINT32 u4Pts);

#ifdef CC_MPEG_LEVEL_ADJ
static void _GetMpegLevelAdjust(UINT8 u1DecId, UINT8* u1Level)
{
    *u1Level = _u1MpegLevel;
}
#endif

static void _GetCompressionDrcMode(UINT8 u1DecId, AUD_DEC_CMPSS_DRC_MODE_T* peCmprssMode)
{
    *peCmprssMode = _aeCmprssDrcMode[u1DecId];
}
static INT32 _AudSetCtrl(UINT8 u1DecId, AUD_DEC_CTRL_T  eAudCtrl)
{
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
        AUD_FMT_T eDetDecFmt;
#endif
#ifdef CC_AUD_ARM_RENDER
    APROC_MW_CTRL_T rMwCtrl;
#endif

    #ifdef ENABLE_MULTIMEDIA
    // in multimedia playback, dmx_mm_mw needs to know the status of
    // audio driver. So here is a hook function.
    // in this function, it just assigns a value.
    // it would not affect the original behavior.
    if (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_MULTI_MEDIA ||
		(_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER && _fgMMPlusAD[u1DecId] ==TRUE))
    {
#if 1
        MTAUDDEC_CB_T eAudCbType;
        #ifdef CC_53XX_SWDMX_V2
#ifdef CC_FBM_TWO_FBP
        eAudCbType.u4Arg1 = _au1SwdmxSrcId[AUD_DEC_MAIN];
#else
        eAudCbType.u4Arg1 = 0;
#endif
        eAudCbType.u4Arg2 = u1DecId;
        eAudCbType.u4Arg3 = (UINT32)eAudCtrl;
        #else
        eAudCbType.u4Arg1 = u1DecId;
        eAudCbType.u4Arg2 = (UINT32)eAudCtrl;
        #endif
        _CB_PutEvent(CB_MTAL_AUDDEC_AUD_MW_DMX_AUDSETCTRL, sizeof(MTAUDDEC_CB_T), (void*)&eAudCbType);
#else
        #ifdef CC_53XX_SWDMX_V2
#ifdef CC_FBM_TWO_FBP
        _MW_DMX_MM_AudSetCtrl(
            _MW_DMX_MM_GetSwdmxSrcId(_au2UpStrmCompId[AUD_DEC_MAIN]),
            u1DecId, (UINT32)eAudCtrl);
#else
        _MW_DMX_MM_AudSetCtrl(_u1SwdmxSrcId, u1DecId, (UINT32)eAudCtrl);
#endif
        #else
        _MW_DMX_MM_AudSetCtrl(u1DecId, (UINT32)eAudCtrl);
        #endif
#endif
    }
    #endif

    if (eAudCtrl == AUD_DEC_CTRL_RESET)
    {
        _aeCtrlSetting[u1DecId] = AUD_DEC_CTRL_STOP;
    }
    else
    {
        _aeCtrlSetting[u1DecId] = eAudCtrl;
    }

#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_SetCtrlType(AUD_DSP0, u1DecId, (UINT32)eAudCtrl);
#else
    AUD_SetCtrlType(u1DecId, (UINT32)eAudCtrl);
#endif

    switch(eAudCtrl)
    {
        case AUD_DEC_CTRL_RESET:

#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_STOP);
            AUD_DSPCmdResetAsyn(AUD_DSP0, u1DecId);
#else
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_STOP);
            AUD_DSPCmdResetAsyn(u1DecId);
#endif
            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_STOP:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_STOP, u1DecId = %d\n", u1DecId);
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            LOG(3, "-->_afgAudDecMwIntendCtrl[0, 1, 2] = %d, %d, %d\n",
                  _afgAudDecMwIntendCtrl[0], _afgAudDecMwIntendCtrl[1], _afgAudDecMwIntendCtrl[2]);
            LOG(3, "-->_afgAudDecMwIfRealCtrl[0, 1, 2] = %d, %d, %d\n",
                  //_afgAudDecMwIfRealCtrl[0], _afgAudDecMwIfRealCtrl[1], _afgAudDecMwIfRealCtrl[2]);
                  AUD_MWCtrl_Get(0), AUD_MWCtrl_Get(1), AUD_MWCtrl_Get(2));
            _afgAudDecMwIntendCtrl[u1DecId] = 0;
#endif
            if (_afgPlayAudClip[u1DecId])
            {
                _afgPlayAudClip[u1DecId] = FALSE;
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
                if (_arAudClipSetType[u1DecId].e_type == AUD_DEC_AUD_CLIP_TYPE_MIXSOUND)
                {
                    LOG(5,"[MixClip] AUD_DEC_CTRL_STOP\n");
                    AUD_StopMixSndClip(0);
                    #if !defined(CC_MT5368) && !defined(CC_MT5396) && !defined(CC_MT5389)
                    AUD_DisableMixSndClip();
                    #endif
                }
                else
                {
#endif
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_StopAudClip(AUD_DSP0, u1DecId);
#else
                    AUD_StopAudClip(u1DecId);
#endif
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
                }
#endif
            }
            else if (_afgTvSysDetecting[u1DecId])
            {
                _afgTvSysDetecting[u1DecId] = FALSE;

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_STOP);
                AUD_DSPCmdStopAsyn(AUD_DSP0, u1DecId);
#else
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_STOP);
                AUD_DSPCmdStopAsyn(u1DecId);
#endif
            }
            else
            {
                ///Normal MM run at AUD_DEC_MAIN
                if(_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_MULTI_MEDIA)
                {
                    LOG(7, "Clean AUD_DrvMMAoutEnableNotify when Decoder STOP!\n");
                    VDP_TriggerAudReceive(0xffffffff);
                }
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_STOP);
                AUD_DSPCmdStopAsyn(u1DecId);
#else
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                if ((u1DecId==AUD_DEC_AUX) && ((_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_ANALOG_TUNER)||
                    (_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_DIGITAL_TUNER)))
                {   //when stop dec2 under TV/DTV src, if dec1 is still playing, then ignore this stop cmd for dec2
#ifndef CC_DUAL_AUD_DEC_SUPPORT
                    if (_afgAudDecMwIntendCtrl[AUD_DEC_MAIN] == 1)
#else
                    if ((_afgAudDecMwIntendCtrl[AUD_DEC_MAIN] == 1)&& (gSubDecId == AUD_DEC_SUB))
#endif
                    {
                        LOG(3, "-->Dec0 is playing. Ignore MW stop Dec1\n");
                        return RMR_ASYNC_NFY;
                    }
                }
#endif

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_STOP);
                AUD_DSPCmdStop(AUD_DSP0, u1DecId);
#else
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_STOP);
                AUD_DSPCmdStop(u1DecId);
#endif
                LOG(3, "MW stop dec u1DecId = %d\n", u1DecId);

#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                AUD_MWCtrl_Set(u1DecId, 0);
                _afgAudDecMwIfRealCtrl[u1DecId] = 0;

                if ((u1DecId==AUD_DEC_MAIN) && ((_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_ANALOG_TUNER)||
                    (_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_DIGITAL_TUNER)))
                {
                    //if ((_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1) && (_afgAudDecMwIntendCtrl[AUD_DEC_AUX]==0))
#ifndef CC_DUAL_AUD_DEC_SUPPORT                    
                    if ((AUD_MWCtrl_Get(AUD_DEC_AUX)==1) && (_afgAudDecMwIntendCtrl[AUD_DEC_AUX]==0))
#else
                    if (((AUD_MWCtrl_Get(AUD_DEC_AUX)==1) && (_afgAudDecMwIntendCtrl[AUD_DEC_AUX]==0))&& (gSubDecId == AUD_DEC_SUB))
#endif
                    {

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                        AUD_SetCtrlCmd(AUD_DSP0, AUD_DEC_AUX, AUD_CMD_STOP);
                        AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_AUX);
#else
                        AUD_SetCtrlCmd(AUD_DEC_AUX, AUD_CMD_STOP);
                        AUD_DSPCmdStop(AUD_DEC_AUX);
#endif
                        LOG(3, "MW_if auto stop dec2 under ATV/DTV src\n");
                        AUD_MWCtrl_Set(AUD_DEC_AUX,0);
                        _afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0;
                        LOG(3, "_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 0\n");
                    }
                }
#endif
#endif
            }

#ifdef CC_MT5391_AUD_3_DECODER  //gallen 0521
            if ((u1DecId == AUD_DEC_THIRD) )
            {
                //AUD_AudioDescriptionChlSel(AUD_DEC_AUX, AUD_CHL_L_R);
                _fgPlayAudDscr = FALSE;
            }
#endif

#ifdef CC_MT5391_AUD_3_DECODER
            // Set TV Scart out volume
            if (u1DecId == AUD_DEC_AUX)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_LEFT, TRUE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_RIGHT, TRUE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_LEFT, TRUE);
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_RIGHT, TRUE);
#endif
                LOG(3, "Mute TV Scart output\n");
            }
#ifdef  LINUX_TURNKEY_SOLUTION
            if (u1Clipflag == 1)
            {
                BSP_FreeAlignedDmaMemory((UINT32)pClip_buf);
                u1Clipflag = 0;
            }
#endif

#endif
#ifdef CC_AUD_ARM_RENDER
            rMwCtrl.eCmd = AUD_CMD_STOP;
            rMwCtrl._aeInSrc[AUD_DEC_MAIN] = _aeStrSrc[AUD_DEC_MAIN];
            rMwCtrl._aeInSrc[AUD_DEC_AUX] = _aeStrSrc[AUD_DEC_AUX];
            rMwCtrl._aeInSrc[AUD_DEC_THIRD] = _aeStrSrc[AUD_DEC_THIRD];
            AUD_Aproc_OutCtrl(u1DecId, &rMwCtrl);
#endif //CC_AUD_ARM_RENDER

            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_PLAY:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_PLAY, u1DecId = %d\n", u1DecId);
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            LOG(3, "-->_afgAudDecMwIntendCtrl[0, 1, 2] = %d, %d, %d\n",
                  _afgAudDecMwIntendCtrl[0], _afgAudDecMwIntendCtrl[1], _afgAudDecMwIntendCtrl[2]);
            LOG(3, "-->_afgAudDecMwIfRealCtrl[0, 1, 2] = %d, %d, %d\n",
                  //_afgAudDecMwIfRealCtrl[0], _afgAudDecMwIfRealCtrl[1], _afgAudDecMwIfRealCtrl[2]);
                  AUD_MWCtrl_Get(0), AUD_MWCtrl_Get(1), AUD_MWCtrl_Get(2));
            _afgAudDecMwIntendCtrl[u1DecId] = 1;
#endif
            // Workaround here to set _afgTvSysDetecting here
            _afgTvSysDetecting[u1DecId] = FALSE;

#ifdef CC_MT5391_AUD_3_DECODER  //gallen 0521
            if ((u1DecId == AUD_DEC_THIRD) )
            {
              //  AUD_AudioDescriptionChlSel(AUD_DEC_AUX, AUD_CHL_DOWNMIX);
                _fgPlayAudDscr = TRUE;
			    if(_fgAdFlag)
			    {
			        AUD_DspSetDDCAdEnable (u1DecId, TRUE);
			    }
            }
#endif
#ifdef CC_AUD_SETUP_MELODY
			AUD_SetSetupPlayStatue(TRUE);	// Reset play finish flag.
#endif

#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(u1DecId);
#else

#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            if ((u1DecId==AUD_DEC_AUX) && (_aeStrSrc[AUD_DEC_AUX] == AUD_STREAM_FROM_ANALOG_TUNER))
            {
                if (_arAudDriverDecFmt[AUD_DEC_MAIN]==AUD_FMT_DETECTOR)
                {
                    LOG(3," MW_AUD auto STOP detector_dec1 before play ATV 2nd decoder!\n");

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetCtrlCmd(AUD_DSP0, AUD_DEC_MAIN, AUD_CMD_STOP);
                    AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_MAIN);
#else
                    AUD_SetCtrlCmd(AUD_DEC_MAIN, AUD_CMD_STOP);
                    AUD_DSPCmdStop(AUD_DEC_MAIN);
#endif
                }
            }

            if ((u1DecId==AUD_DEC_MAIN) && ((_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_ANALOG_TUNER)||
                (_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_DIGITAL_TUNER)) &&
                ((_arAudDecFmt[AUD_DEC_MAIN].e_fmt !=AUD_DEC_FMT_FMRDO)&&(_arAudDecFmt[AUD_DEC_MAIN].e_fmt !=AUD_DEC_FMT_FMRDO_DET)))
            {   // when play dec1 for TV/DTV src, if the 2nd decoder is not played yet, then
                // issue play by mw_if itself. work around for MW, by gallen, 20080731
                //if (_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] != 1)
                if (AUD_MWCtrl_Get(AUD_DEC_AUX) != 1)
                {
                    //set format and issue play;
                    if (_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_DIGITAL_TUNER)
                    {
                        _aeStrSrc[AUD_DEC_AUX] = AUD_STREAM_FROM_DIGITAL_TUNER;
                        _arAudDecFmt[AUD_DEC_AUX].e_fmt = _arAudDecFmtMwSet[AUD_DEC_MAIN].e_fmt;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                        AUD_SetDecType(AUD_DSP0, AUD_DEC_AUX, _aeStrSrc[AUD_DEC_AUX], (AUD_FMT_T)_arAudDecFmtMwSet[AUD_DEC_MAIN].e_fmt);
#else
                        AUD_SetDecType(AUD_DEC_AUX, _aeStrSrc[AUD_DEC_AUX], (AUD_FMT_T)_arAudDecFmtMwSet[AUD_DEC_MAIN].e_fmt);
#endif
                    }
                    else
                    {
                        _aeStrSrc[AUD_DEC_AUX] = AUD_STREAM_FROM_ANALOG_TUNER;
#ifdef CC_AUD_OVM_FORCE_MONO_SUPPORT
                        if ((_arAudDecOvmMode == AUD_DEC_OVM_HIGH) && (_arAudDriverDecFmt[AUD_DEC_MAIN] == AUD_FMT_A2))
                        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                            AUD_SetDecType(AUD_DSP0, AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], AUD_FMT_PAL);
#else
                            AUD_SetDecType(AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], AUD_FMT_PAL);
#endif
                            LOG(0, "FORCE_MONO Auto play aux_pal!\n");
                            //Force A2 to PAL if OVM_HIGH for PAA model
                        }
                        else
                        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                            AUD_SetDecType(AUD_DSP0, AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], _arAudDriverDecFmt[AUD_DEC_MAIN]);
#else
                            AUD_SetDecType(AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], _arAudDriverDecFmt[AUD_DEC_MAIN]);
#endif
                        }
#else
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                        AUD_SetDecType(AUD_DSP0, AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], _arAudDriverDecFmt[AUD_DEC_MAIN]);
#else
                        AUD_SetDecType(AUD_DEC_AUX, _aeStrSrc[AUD_DEC_MAIN], _arAudDriverDecFmt[AUD_DEC_MAIN]);
#endif
#endif
                    }

                    AUD_MWCtrl_Set(AUD_DEC_AUX, 1);
                    _afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 1;

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetCtrlCmd(AUD_DSP0, AUD_DEC_AUX, AUD_CMD_PLAY);
                    AUD_DSPCmdPlay(AUD_DSP0, AUD_DEC_AUX);
#else
                    AUD_SetCtrlCmd(AUD_DEC_AUX, AUD_CMD_PLAY);
                    AUD_DSPCmdPlay(AUD_DEC_AUX);
#endif
                    LOG(3, "_afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 1\n");
                }
            }
#endif

#ifdef CC_AUD_SUPPORT_MS10
#ifdef AUD_DECODER3_SELF_TEST
            if ((u1DecId == AUD_DEC_THIRD) && (u1ForceDecId == AUD_DEC_NUM) && (!AUD_DspIsTriOnlyDecMode())
			)     //parson MM3
#else
            if ((u1DecId == AUD_DEC_THIRD) && (!AUD_DspIsTriOnlyDecMode()))
#endif
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, AUD_DEC_MAIN, AUD_CMD_STOP);
                AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_MAIN);
#else
                AUD_SetCtrlCmd(AUD_DEC_MAIN, AUD_CMD_STOP);
                AUD_DSPCmdStop(AUD_DEC_MAIN);
#endif
                //AUD_DSPCmdStop(AUD_DEC_AUX);
            }
#endif

#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(AUD_DSP0, u1DecId);
#else
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(u1DecId);
#endif

#ifdef CC_AUD_SUPPORT_MS10
#ifdef AUD_DECODER3_SELF_TEST
            if ((u1DecId == AUD_DEC_THIRD) && (u1ForceDecId == AUD_DEC_NUM) && (!AUD_DspIsTriOnlyDecMode()))     //parson MM3
#else
            if ((u1DecId == AUD_DEC_THIRD) && (!AUD_DspIsTriOnlyDecMode()))
#endif
            {
                //AUD_DSPCmdPlayAsyn(AUD_DEC_AUX);

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, AUD_DEC_MAIN, AUD_CMD_PLAY);
                AUD_DSPCmdPlayAsyn(AUD_DSP0, AUD_DEC_MAIN);
#else
                AUD_SetCtrlCmd(AUD_DEC_MAIN, AUD_CMD_PLAY);
                AUD_DSPCmdPlayAsyn(AUD_DEC_MAIN);
#endif
            }
#endif

#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
           AUD_MWCtrl_Set(u1DecId, 1);
            _afgAudDecMwIfRealCtrl[u1DecId] = 1;
#endif
#endif

#ifdef CC_MT5391_AUD_3_DECODER
            // Set TV Scart out volume
            if (u1DecId == AUD_DEC_AUX)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#endif
                LOG(3, "Unmute TV Scart output\n");
            }
#endif

#ifdef CC_AUD_ARM_RENDER
            rMwCtrl.eCmd = AUD_CMD_PLAY;
            rMwCtrl._aeInSrc[AUD_DEC_MAIN] = _aeStrSrc[AUD_DEC_MAIN];
            rMwCtrl._aeInSrc[AUD_DEC_AUX] = _aeStrSrc[AUD_DEC_AUX];
            rMwCtrl._aeInSrc[AUD_DEC_THIRD] = _aeStrSrc[AUD_DEC_THIRD];
            AUD_Aproc_OutCtrl(u1DecId, &rMwCtrl);
#endif //CC_AUD_ARM_RENDER


            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_PLAY_SYNC:
            // Workaround here to set _afgTvSysDetecting here
            _afgTvSysDetecting[u1DecId] = FALSE;
            {
                AV_SYNC_MODE_T eSyncMode = AV_SYNC_SLAVE;

                AUD_SetAvSynMode(u1DecId, eSyncMode);

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_PLAY);
                AUD_DSPCmdPlayAsyn(AUD_DSP0, u1DecId);
#else
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_PLAY);
                AUD_DSPCmdPlayAsyn(u1DecId);
#endif
            }

#ifdef CC_MT5391_AUD_3_DECODER
            // Set TV Scart out volume
            if (u1DecId == AUD_DEC_AUX)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#endif
                LOG(3, "Unmute TV Scart output\n");
            }
#endif

            return RMR_ASYNC_NFY;

#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
        case AUD_DEC_CTRL_PLAY_AUD_MIXSND_CLIP:
            #ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            _afgAudDecMwIntendCtrl[u1DecId] = 1;
            #endif

            _arAudClipSetType[u1DecId].e_type = AUD_DEC_AUD_CLIP_TYPE_MIXSOUND;
            //if (_arAudClip[u1DecId].u4OutputChlSelect != AUD_DEV_LS_FLAG_NONE)
            {
                LOG(5,"[MixClip] AUD_DEC_CTRL_PLAY_AUD_MIXSND_CLIP #1\n");
                _afgPlayAudClip[u1DecId] = TRUE;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                if (AUD_IsDecoderPlay(AUD_DSP0, u1DecId))
#else
                if (AUD_IsDecoderPlay(u1DecId))
#endif
                {
                    return RMR_DRV_INV_SET_INFO;
                }
                LOG(5,"[MixClip] AUD_DEC_CTRL_PLAY_AUD_MIXSND_CLIP #2\n");
                AUD_EnableMixSndClip();
                AUD_PlayMixSndClip(0);
                #ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                AUD_MWCtrl_Set(u1DecId, 1);
                _afgAudDecMwIfRealCtrl[u1DecId] = 1;
                #endif
            }
            return RMR_ASYNC_NFY;
#endif

        case AUD_DEC_CTRL_PLAY_AUD_CLIP:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_PLAY_AUD_CLIP, u1DecId = %d\n", u1DecId);
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            LOG(3, "-->_afgAudDecMwIntendCtrl[0, 1, 2] = %d, %d, %d\n",
                  _afgAudDecMwIntendCtrl[0], _afgAudDecMwIntendCtrl[1], _afgAudDecMwIntendCtrl[2]);
            LOG(3, "-->_afgAudDecMwIfRealCtrl[0, 1, 2] = %d, %d, %d\n",
                  //_afgAudDecMwIfRealCtrl[0], _afgAudDecMwIfRealCtrl[1], _afgAudDecMwIfRealCtrl[2]);
                  AUD_MWCtrl_Get(0), AUD_MWCtrl_Get(1), AUD_MWCtrl_Get(2));

            _afgAudDecMwIntendCtrl[u1DecId] = 1;
#endif


            if (_arAudClip[u1DecId].u4OutputChlSelect != AUD_DEV_LS_FLAG_NONE)
            {
                _afgPlayAudClip[u1DecId] = TRUE;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                if (AUD_IsDecoderPlay(AUD_DSP0, u1DecId))
#else
                if (AUD_IsDecoderPlay(u1DecId))
#endif
                {
                    return RMR_DRV_INV_SET_INFO;
                }
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
                if (_arAudClipSetType[u1DecId].e_type == AUD_DEC_AUD_CLIP_TYPE_MIXSOUND)
                {
                    LOG(5,"[MixClip] AUD_DEC_CTRL_PLAY_AUD_CLIP\n");
                    AUD_EnableMixSndClip();
                    AUD_PlayMixSndClip(0);
                }
                else
                {
#endif
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_PlayAudClip(AUD_DSP0, u1DecId, _arAudClip[u1DecId].u4OutputChlSelect, _arAudClip[u1DecId].fgPlayTestTone);
#else
                    AUD_PlayAudClip(u1DecId, _arAudClip[u1DecId].u4OutputChlSelect, _arAudClip[u1DecId].fgPlayTestTone);
#endif
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
                }
#endif
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                AUD_MWCtrl_Set(u1DecId, 1);
                _afgAudDecMwIfRealCtrl[u1DecId] = 1;
#endif
            }

#ifdef CC_MT5391_AUD_3_DECODER
            // Set TV Scart out volume
            if (u1DecId == AUD_DEC_AUX)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
#endif
                LOG(3, "Unmute TV Scart output\n");
            }
#endif
            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_FLUSH:

#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetCtrlCmd(AUD_DSP0,u1DecId, AUD_CMD_STOP);
            AUD_DSPCmdStopAsyn(AUD_DSP0, u1DecId);
#else
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_STOP);
            AUD_DSPCmdStopAsyn(u1DecId);
#endif
            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_PAUSE:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_PAUSE, u1DecId = %d\n", u1DecId);
            //AUD_DSPCmdPauseAsyn(u1DecId);
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
            if ((_afgPlayAudClip[u1DecId]) && (_arAudClipSetType[u1DecId].e_type == AUD_DEC_AUD_CLIP_TYPE_MIXSOUND))
            {
                LOG(5,"[MixClip] AUD_DEC_CTRL_PAUSE\n");
                AUD_PauseMixSndClip(0);
            }
            else
            {
#endif

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_PAUSE);
                AUD_DSPCmdPause(AUD_DSP0, u1DecId);
#else
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_PAUSE);
                AUD_DSPCmdPause(u1DecId);
#endif
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
            }
#endif
            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_RESUME:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_RESUME, u1DecId = %d\n", u1DecId);
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
            if ((_afgPlayAudClip[u1DecId]) && (_arAudClipSetType[u1DecId].e_type == AUD_DEC_AUD_CLIP_TYPE_MIXSOUND))
            {
                LOG(5,"[MixClip] AUD_DEC_CTRL_RESUME\n");
                AUD_ResumeMixSndClip(0);
            }
            else
            {
#endif

#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_RESUME);
                AUD_DSPCmdResume(AUD_DSP0, u1DecId);
#else
                AUD_SetCtrlCmd(u1DecId, AUD_CMD_RESUME);
                AUD_DSPCmdResume(u1DecId);
#endif
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
            }
#endif
            return RMR_ASYNC_NFY;

        case AUD_DEC_CTRL_DETECT_TV_AUD_SYS:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_DETECT_TV_AUD_SYS, u1DecId = %d\n", u1DecId);
            // Check audio decoder format at first
            //if (_arAudDriverDecFmt[u1DecId] != AUD_FMT_DETECTOR)
            _afgTvSysDetecting[u1DecId] = TRUE;
            UNUSED(_AUD_IgnoreDecNotify(AUD_DEC_AUX, TRUE));
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_AUX);
#else
            AUD_DSPCmdStop(AUD_DEC_AUX);
#endif
            UNUSED(_AUD_IgnoreDecNotify(AUD_DEC_AUX, FALSE));
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_DETECTOR);
#else
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_DETECTOR);
#endif
                _arAudDriverDecFmt[u1DecId] = AUD_FMT_DETECTOR;
                _AudSetDetTvSrcMode(u1DecId);
            }
            UNUSED(_arAudDriverDecFmt);
            // Invoke audio playback command
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetCtrlCmd(AUD_DSP0, u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(AUD_DSP0, u1DecId);
#else
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(u1DecId);
#endif
            return RMR_ASYNC_NFY;

#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
        case AUD_DEC_CTRL_DETECT_TV_AUD_SYS_N_PLAY:
            LOG(3,"Aud_Set -- _AudSetCtrl -- AUD_DEC_CTRL_DETECT_TV_AUD_SYS_N_PLAY, u1DecId = %d\n", u1DecId);
            _afgAudDecMwIntendCtrl[u1DecId] = 1;
            if (u1DecId == AUD_DEC_AUX)
            {
                _afgTvSysDetecting[u1DecId] = TRUE;
                eDetDecFmt = AudAtvFmtDetection();
                LOG(0,"#### Detect TV Sys = %d #######\n ", eDetDecFmt);
                _afgTvSysDetecting[u1DecId] = FALSE;
                _arAudDriverDecFmt[u1DecId] = eDetDecFmt;
                AUD_SetDecType(AUD_DEC_AUX, AUD_STREAM_FROM_ANALOG_TUNER, _arAudDriverDecFmt[u1DecId]);
            }
            else if ((u1DecId==AUD_DEC_MAIN) && (_aeStrSrc[AUD_DEC_MAIN] == AUD_STREAM_FROM_ANALOG_TUNER))
            {   // when play dec1 for TV/DTV src, if the 2nd decoder is not played yet, then
                // issue play by mw_if itself. work around for MW

                // if PCB don't have TV SCART, AUD_MWCtrl_Get(AUD_DEC_AUX) won't be 1
                if (AUD_MWCtrl_Get(AUD_DEC_AUX) == 1)
                {
                    AUD_SetCtrlCmd(AUD_DEC_AUX, AUD_CMD_STOP);
                    AUD_DSPCmdStop(AUD_DEC_AUX); // Fix mw enable tv bypass and play aux decoder first
                    AUD_MWCtrl_Set(AUD_DEC_AUX, 0);
                }

                _afgTvSysDetecting[u1DecId] = TRUE;
                eDetDecFmt = AudAtvFmtDetection();
                LOG(0,"#### Detect TV Sys = %d #######\n ", eDetDecFmt);
                _afgTvSysDetecting[u1DecId] = FALSE;

                //set format and issue play;
                _aeStrSrc[AUD_DEC_AUX] = AUD_STREAM_FROM_ANALOG_TUNER;
                _arAudDriverDecFmt[u1DecId] = eDetDecFmt;
                AUD_SetDecType(AUD_DEC_AUX, AUD_STREAM_FROM_ANALOG_TUNER, _arAudDriverDecFmt[u1DecId]);

                AUD_MWCtrl_Set(AUD_DEC_AUX, 1);
                _afgAudDecMwIfRealCtrl[AUD_DEC_AUX] = 1;
                AUD_SetCtrlCmd(AUD_DEC_AUX, AUD_CMD_PLAY);
                AUD_DSPCmdPlayAsyn(AUD_DEC_AUX);
            }
            AUD_SetCtrlCmd(u1DecId, AUD_CMD_PLAY);
            AUD_DSPCmdPlayAsyn(u1DecId);
            AUD_MWCtrl_Set(u1DecId, 1);
             _afgAudDecMwIfRealCtrl[u1DecId] = 1;
            return RMR_ASYNC_NFY;
#endif

        default:
            LOG( 1, "SetCtrl Error\n");
            return RMR_DRV_INV_SET_INFO;
    }
 //   return RMR_OK;
}

static void _AudSetNotifyInfo(UINT8 u1DecId, const AUD_DEC_NFY_INFO_T * prNtyInfo)
{
#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)    //parson MM3
    {
        u1DecId=AUD_DEC_MAIN;
    }
#endif
    _arAudMwNotifyInfo[u1DecId].pv_tag = prNtyInfo->pv_tag;
    _arAudMwNotifyInfo[u1DecId].pf_aud_dec_nfy = prNtyInfo->pf_aud_dec_nfy;
}

static void _AudSetHPNotifyInfo(UINT8 u1DecId, const AUD_DEC_HDPH_DETECT_NFT_INFO_T * prNtyInfo)
{
    _arAudHPMwNotifyInfo[u1DecId].pv_nfy_tag = prNtyInfo->pv_nfy_tag;
    _arAudHPMwNotifyInfo[u1DecId].pf_hdph_detect_nfy = prNtyInfo->pf_hdph_detect_nfy;
}

//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
static void _AudSetYWNotifyInfo(UINT8 u1DecId, const AUD_DEC_UPLOAD_DATA_NFT_INFO_T * prNtyInfo)
{
    _arAudYWMwNotifyInfo[u1DecId].pv_nfy_tag = prNtyInfo->pv_nfy_tag;
    _arAudYWMwNotifyInfo[u1DecId].pf_upload_data_nfy = prNtyInfo->pf_upload_data_nfy;
}
//#endif

static void _SetCustomerMusicFlag(UINT8 u1DecId, AUD_DEC_AUD_CLIP_CUSTIOM_T * peMusicFlag)
{
    UINT8 u1EEP_data;
    UNUSED(u1DecId);

    u1EEP_data = bApiEepromReadByte(EEP_BOOT_MUSIC_ONOFF);

    switch (peMusicFlag->e_drv_clip_custom_setting_type)
    {
        case AUD_DEC_CLIP_CUSTOM_SETTING_TYPE_POWER_ON:
            if(peMusicFlag ->e_drv_clip_custom_setting_mode == AUD_DEC_CLIP_CUSTOM_SETTING_MODE_ON)
            {
                u1EEP_data |= AUD_POWER_ON_MUSIC_MASK;
            }
            else
            {
                u1EEP_data &= (~(UINT8)AUD_POWER_ON_MUSIC_MASK);
            }
            break;
        case AUD_DEC_CLIP_CUSTOM_SETTING_TYPE_POWER_OFF:
            if(peMusicFlag ->e_drv_clip_custom_setting_mode == AUD_DEC_CLIP_CUSTOM_SETTING_MODE_ON)
            {
                u1EEP_data |= AUD_POWER_OFF_MUSIC_MASK;
            }
            else
            {
                u1EEP_data &= (~(UINT8)AUD_POWER_OFF_MUSIC_MASK);
            }
            break;
        default:
            return;
    }
    fgApiEepromWriteByte(EEP_BOOT_MUSIC_ONOFF,u1EEP_data);
}

static void _SetFormat(UINT8 u1DecId, const AUD_DEC_FMT_INFO_T * prFormatInfo)
{
    AUD_DEC_STREAM_FROM_T eStreamFrom;

    //_AudConvertDecId_Dual ? Please don't do convert SUB to real id in this function.
    // 1. SetDecType will do it. (If you convert sub_id to others, it will be covert again in setDecType!!
    // 2. _au2MwConnectComp[SUB] is stored in SUB_ID, not in converted id.

    LOG(3, "[MT_AUD_MW] _SetFormat(), Dec (%d), Format (%d), StrFrom (%d)\n",
        u1DecId, prFormatInfo->e_fmt, _aeStrSrc[u1DecId]);
    _arAudDecFmt[u1DecId].e_fmt = prFormatInfo->e_fmt;
    _arAudDecFmt[u1DecId].pv_info = prFormatInfo->pv_info;

#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
    _arAudDecFmtMwSet[u1DecId].e_fmt = prFormatInfo->e_fmt;
#endif


    eStreamFrom = _aeStrSrc[u1DecId];
    // Check if EURO CANAL+ mode
    if ((_au2MwConnectComp[u1DecId]  == DRVT_AVC_SCART) &&
        (prFormatInfo->e_fmt == AUD_DEC_FMT_EU_CANAL_PLUS))
    {
        DSP_SetEuroCanalPlusMode(TRUE);
    }
    else
    {
        DSP_SetEuroCanalPlusMode(FALSE);
    }

    // Setup format ..
#ifndef CC_MT5391_AUD_3_DECODER
    if ((AUD_GetTargetCountry() == COUNTRY_US) &&
        ((eStreamFrom == AUD_STREAM_FROM_ANALOG_TUNER) ||
        ((_au2MwConnectComp[u1DecId] == DRVT_AVC_SCART) && (prFormatInfo->e_fmt == AUD_DEC_FMT_TV_SYS))))
    {
        // US model, load MTS code if stream source is ATV ..
        _aeStrSrc[u1DecId] = AUD_STREAM_FROM_ANALOG_TUNER;
        eStreamFrom = AUD_STREAM_FROM_ANALOG_TUNER;
        // Modify to support korea A2
#ifdef CC_SUPPORT_KOREA_A2
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_A2);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif

    }
#else
    if (((AUD_GetTargetCountry() == COUNTRY_US) || (AUD_GetTargetCountry() == COUNTRY_TW)) &&
        ((eStreamFrom == AUD_STREAM_FROM_ANALOG_TUNER) ||
        ((_au2MwConnectComp[u1DecId] == DRVT_AVC_SCART) && (prFormatInfo->e_fmt == AUD_DEC_FMT_TV_SYS))))
    {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
        if (u1DecId==AUD_DEC_MAIN)
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
            AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
        }
        else
        {
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
            //if ((u1DecId==AUD_DEC_AUX)&&(_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1)&&
            if ((u1DecId==AUD_DEC_AUX)&&(AUD_MWCtrl_Get(AUD_DEC_AUX)==1)&&
                (_arAudDriverDecFmt[AUD_DEC_MAIN] == AUD_FMT_MTS))
            {
                LOG(3, "Ignore MW _SetFormat of dec2 to MTS when input src is ATV\n");
            }
            else
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
                AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif
            }
#else
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif
        }
#endif  //CC_MT5391_AUD_3_DECODER
        AUD_DspSetTvSrcMode(SV_MTS);
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_MTS;

    }
#endif
    else if ((AUD_GetTargetCountry() != COUNTRY_US) &&
        ((_au2MwConnectComp[u1DecId]  == DRVT_TUNER_SAT_ANA) ||
        (_au2MwConnectComp[u1DecId]  == DRVT_TUNER_CAB_ANA) ||
        (_au2MwConnectComp[u1DecId] == DRVT_TUNER_TER_ANA &&
        ((prFormatInfo->e_fmt != AUD_DEC_FMT_FMRDO) && (prFormatInfo->e_fmt != AUD_DEC_FMT_FMRDO_DET))) ||
        ((_au2MwConnectComp[u1DecId]  == DRVT_AVC_SCART) && (prFormatInfo->e_fmt == AUD_DEC_FMT_TV_SYS))))
    {   // EU model ...
        _aeStrSrc[u1DecId] = AUD_STREAM_FROM_ANALOG_TUNER;
        eStreamFrom = AUD_STREAM_FROM_ANALOG_TUNER;

        AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning

        // To invoke detection or just load proper decoding code
        if (_IsRunDetection(u1DecId))
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_DETECTOR);
#else
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_DETECTOR);
#endif
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_DETECTOR;
        }
        else if (_IsMaskSetToMtsSys(u1DecId))
        {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
            if (u1DecId==AUD_DEC_MAIN)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
            }
            else
            {
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //if (((u1DecId==AUD_DEC_AUX)&&(_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1)&&
                if (((u1DecId==AUD_DEC_AUX)&&(AUD_MWCtrl_Get(AUD_DEC_AUX)==1)&&
                    (_arAudDriverDecFmt[AUD_DEC_MAIN] == AUD_FMT_MTS)))
                {
                    LOG(3, "Ignore MW _SetFormat of dec2 to BTSC when input src is ATV\n");
                }
                else
                {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
                    AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif
                }
#else
                AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif
            }
#endif  //CC_MT5391_AUD_3_DECODER
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_MTS;
            AUD_DspSetTvSrcMode(_GetTvSysFormat(u1DecId)); /* dyin: 090122 for SBTVD*/
        }
        else if (_IsMaskSetToA2Sys(u1DecId))
        {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_A2);
#else
            if (u1DecId==AUD_DEC_MAIN)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
            }
            else
            {
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //if (((u1DecId==AUD_DEC_AUX)&&(_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1)&&
                if (((u1DecId==AUD_DEC_AUX)&&(AUD_MWCtrl_Get(AUD_DEC_AUX)==1)&&
                    (_arAudDriverDecFmt[AUD_DEC_MAIN] == AUD_FMT_A2)))
                {
                    LOG(3, "Ignore MW _SetFormat of dec2 to A2 when input src is ATV\n");
                }
                else
                {
#ifdef CC_AUD_OVM_FORCE_MONO_SUPPORT
                    LOG(0, "Incorrect MW_CTRL under FORCE_MONO_SUPPORT!!!\n\n");
#endif
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_A2);
#else
                    AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_A2);
#endif
                }
#else
                AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_A2);
#endif
            }
#endif  //CC_MT5391_AUD_3_DECODER
#ifndef CC_AUD_OVM_FORCE_MONO_SUPPORT
            AUD_DspSetTvSrcMode(_GetTvSysFormat(u1DecId));
            if (u1DecId == AUD_DEC_MAIN) //main is alway pcm, no need to load A2 code
            {
                _arAudDriverDecFmt[u1DecId] = AUD_FMT_A2;
            }
            else
            {
                if (_fgForceToPal == 1)
                {
                    _arAudDriverDecFmt[u1DecId] = AUD_FMT_PAL;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_PAL);
#else
                    AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PAL);
#endif
                    _fgForceToPal = 0;
                }
                else
                {
                    _arAudDriverDecFmt[u1DecId] = AUD_FMT_A2;
                }
            }
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_A2;
#else
            UNUSED(_fgForceToPal);
            if (_arAudDecOvmMode != AUD_DEC_OVM_HIGH)
            {
                AUD_DspSetTvSrcMode(_GetTvSysFormat(u1DecId));
                _arAudDriverDecFmt[u1DecId] = AUD_FMT_A2;
            }
            else // OVM=HIGH, need to transfrom A2 to PAL
            {
                TV_AUD_SYS_T eTvSrc;
                eTvSrc = _GetTvSysFormat(u1DecId);
                if (eTvSrc == SV_A2_BG)
                {
                    eTvSrc = SV_PAL_BG;
                }
                else if (eTvSrc == SV_A2_DK1 || eTvSrc == SV_A2_DK2 || eTvSrc == SV_A2_DK3)
                {
                    eTvSrc = SV_PAL_DK;
                }
                AUD_DspSetTvSrcMode(eTvSrc);
                _arAudDriverDecFmt[u1DecId] = AUD_FMT_PAL;
            }
#endif
        }
        else if (_IsMaskSetToPalSys(u1DecId))
        {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PAL);
#else
            if (u1DecId==AUD_DEC_MAIN)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
            }
            else
            {
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //if ((u1DecId==AUD_DEC_AUX)&&(_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1)&&
                if ((u1DecId==AUD_DEC_AUX)&&(AUD_MWCtrl_Get(AUD_DEC_AUX)==1)&&
                    (_arAudDriverDecFmt[AUD_DEC_MAIN] == AUD_FMT_PAL))
                {
                    LOG(3, "Ignore MW _SetFormat of dec2 to PAL when input src is ATV\n");
                }
                else
                {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_PAL);
#else
                    AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PAL);
#endif
                }
#else
                AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PAL);
#endif
            }
#endif  //CC_MT5391_AUD_3_DECODER
            AUD_DspSetTvSrcMode(_GetTvSysFormat(u1DecId));
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_PAL;
        }
        else if (_IsMaskSetToFmfmSys(u1DecId))
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_FMFM);
#else
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_FMFM);
#endif
            AUD_DspSetTvSrcMode(SV_FM_FM);
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_FMFM;
        }
        else
        {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
            AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
            if (u1DecId==AUD_DEC_MAIN)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
            }
            else
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_MTS);
#else
                AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_MTS);
#endif
            }
#endif  //CC_MT5391_AUD_3_DECODER
            _arAudDriverDecFmt[u1DecId] = AUD_FMT_MTS;
            LOG(1, "Improper audio decoder format\n");
        }
    }

    //new added for FM radio support
    else if ((_au2MwConnectComp[u1DecId]  == DRVT_TUNER_TER_ANA)
                 &&((prFormatInfo->e_fmt == AUD_DEC_FMT_FMRDO)||(prFormatInfo->e_fmt == AUD_DEC_FMT_FMRDO_DET)))
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
       AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[u1DecId].e_fmt);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[u1DecId].e_fmt);
#endif
    }
    else if (eStreamFrom == AUD_STREAM_FROM_LINE_IN)
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
       AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_PCM);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PCM);
#endif
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_PCM;
    }
#ifdef  CC_DUAL_AUD_DEC_SUPPORT
    else if (eStreamFrom == AUD_STREAM_FROM_LINE_IN_2)
    {
       AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_PCM);
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_PCM;
    }
#endif
    else if (eStreamFrom == AUD_STREAM_FROM_HDMI)
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_PCM);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_PCM);
#endif
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_PCM;
    }
    else if (((_au2MwConnectComp[u1DecId]  == DRVT_AVC_SCART) && (prFormatInfo->e_fmt == AUD_DEC_FMT_PCM)) ||
             ((_au2MwConnectComp[u1DecId]  == DRVT_AVC_SCART) && (prFormatInfo->e_fmt == AUD_DEC_FMT_EU_CANAL_PLUS)))
    {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
        AUD_SetDecType(u1DecId, AUD_STREAM_FROM_LINE_IN, AUD_FMT_PCM);
#else
#ifdef CC_DUAL_AUD_DEC_SUPPORT
        _aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN_2;
       AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_LINE_IN_2, AUD_FMT_PCM);
#else
        _aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN; //gallen 20081126
#ifdef CC_AUD_SUPPORT_DUAL_DSP
       AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_LINE_IN, AUD_FMT_PCM);
#else
        AUD_SetDecType(u1DecId, AUD_STREAM_FROM_LINE_IN, AUD_FMT_PCM);
#endif
#endif
#endif
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_PCM;
    }
    else if ((_au2MwConnectComp[u1DecId]  == DRVT_AVC_SPDIF))
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, AUD_FMT_AC3);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, AUD_FMT_AC3);
#endif
        _arAudDriverDecFmt[u1DecId] = AUD_FMT_AC3;
    }
    else
    {
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
        AUD_SetDecType(u1DecId, eStreamFrom, (AUD_FMT_T)prFormatInfo->e_fmt);
#else
#ifndef CC_DUAL_AUD_DEC_SUPPORT
        if ((u1DecId==AUD_DEC_MAIN) && (eStreamFrom==AUD_STREAM_FROM_DIGITAL_TUNER))
#else
        if (((u1DecId==AUD_DEC_MAIN) && (eStreamFrom==AUD_STREAM_FROM_DIGITAL_TUNER))&&(gSubDecId == AUD_DEC_SUB))
#endif
        {
            //when source is set to DTV, force dec1 = PCM
            _arAudDecFmt[u1DecId].e_fmt = AUD_DEC_FMT_PCM;
        }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        AUD_SetDecType(AUD_DSP0, u1DecId, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[u1DecId].e_fmt);
#else
        AUD_SetDecType(u1DecId, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[u1DecId].e_fmt);
#endif
        if (((u1DecId==AUD_DEC_AUX) && (eStreamFrom==AUD_STREAM_FROM_DIGITAL_TUNER))&&_fgMMPlusAD[u1DecId]==TRUE )
        {
            _arAudDecFmt[AUD_DEC_MAIN].e_fmt = AUD_DEC_FMT_PCM;

#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, AUD_DEC_MAIN, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[AUD_DEC_MAIN].e_fmt);
#else
			AUD_SetDecType(AUD_DEC_MAIN, eStreamFrom, (AUD_FMT_T)_arAudDecFmt[AUD_DEC_MAIN].e_fmt);
#endif
        }
#endif
        _arAudDriverDecFmt[u1DecId] = (AUD_FMT_T)_arAudDecFmt[u1DecId].e_fmt;
    }
#if 0
    // Set PCM pre-buffer bank number for HDMI
    if (u1DecId == AUD_DEC_MAIN)
    {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        if (eStreamFrom == AUD_STREAM_FROM_HDMI)
        {
            AUD_DspSetPcmPreBuffer(AUD_DSP0, u1DecId, (UINT16)DRVCUST_OptGet(eAudioPcmPrebufBankNum));
        }
        else
        {
            AUD_DspSetPcmPreBuffer(AUD_DSP0, u1DecId, 0);
        }
#else
        if (eStreamFrom == AUD_STREAM_FROM_HDMI)
        {
            AUD_DspSetPcmPreBuffer(u1DecId, (UINT16)DRVCUST_OptGet(eAudioPcmPrebufBankNum));
        }
        else
        {
            AUD_DspSetPcmPreBuffer(u1DecId, 0);
        }
#endif
    }
#endif
}

static void _SetVolume(UINT8 u1DecId, AUD_DEC_VOLUME_INFO_T * prChlVol)
{
    AUD_CH_T eAudCh = AUD_CH_ALL;

    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning

    LOG(3, "[MT_AUD_MW] _SetVolume(), Dec (%d), Type (%d), Vol (%d)\n",
        u1DecId, (UINT8)prChlVol->e_vol_type, prChlVol->u.ui1_level);

    if (prChlVol->e_vol_type == AUD_DEC_ALL_CH)
    {
#ifdef AUD_SUPPORT_SRS_BYPASS
        // 1. From 5395.
        // 2. Remove ADAC_SpeakerEnable operation.
        // 3. For Ch L/R :
        //     - adjust master volume when BYPASS off.
        //     - adjust trim when BYPASS on.
        // 4. For Ch 9/10 :
        //     - adjust trim always, since CH 9/10 bypass post processing.
        UINT8 u1Volume;
        _arChVol[u1DecId].u1VolMaster = prChlVol->u.ui1_level;
        u1Volume = prChlVol->u.ui1_level;
        if(isSrsBypass)
        {
            LOG(5, "isSrsBypass\n");
            //update Channel Volume
            _arChVol[u1DecId].u1VolDmxL = u1Volume;
            _arChVol[u1DecId].u1VolDmxR = u1Volume;
            if (_aeMuteType[u1DecId] != AUD_DEC_MUTE_OFF) // Mute case.
            {
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_LEFT, 0);
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_RIGHT, 0);
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, 0);
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, 0);
                //x_thread_delay(200);
                //ADAC_SpeakerEnable(FALSE);
            }
            else
            {
                LOG(5, "u1Volume = %d\n", u1Volume);
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, u1Volume);
                AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, u1Volume);
                // For Variable lineout.
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, u1Volume);
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_RIGHT, u1Volume);
                //x_thread_delay(200);
                //ADAC_SpeakerEnable(TRUE);
            }
        }
        else
        {
            eAudCh = AUD_CH_ALL;
            AUD_DspMasterVolume(u1DecId, u1Volume);
            //update Channel Volume
            _arChVol[u1DecId].u1VolDmxL = u1Volume;
            _arChVol[u1DecId].u1VolDmxR = u1Volume;
            // For Variable line out.
            // Add Variable lineout "MUTE" control for
            ///source1(TV) mute swtich to source2(HDMI) un-mute issue.
            if (_aeMuteType[u1DecId] != AUD_DEC_MUTE_OFF) // Mute case.
            {
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, 0);
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_RIGHT, 0);
            }
            else
            {
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, u1Volume);
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_DMX_RIGHT, u1Volume);
            }
        }
#else
        UINT8 u1Volume;

        _arChVol[u1DecId].u1VolMaster = prChlVol->u.ui1_level;
        u1Volume = prChlVol->u.ui1_level;
        eAudCh = AUD_CH_ALL;

#ifdef CC_AUD_SX1_FEATURE
        if(u1DecId == AUD_DEC_THIRD)
        {
            AUD_DspAdVolOffset(u1Volume);
        }
        else
        {
            AUD_DspMasterVolume(u1DecId, u1Volume);
#ifdef CC_MT5391_AUD_3_DECODER
            AUD_DspMasterVolume(AUD_DEC_THIRD, u1Volume);
#endif
        }
#else
        AUD_DspMasterVolume(u1DecId, u1Volume);
#endif
#endif // AUD_SUPPORT_SRS_BYPASS
    }
    else if(prChlVol->e_vol_type == AUD_DEC_TEST_TONE)
	{
		UINT8 u1Volume;

		_arChVol[u1DecId].u1VolTone = prChlVol->u.ui1_level;
        u1Volume = prChlVol->u.ui1_level;
        AUD_DspMixSndVolume(u1DecId, u1Volume);
	}
    else if (prChlVol->e_vol_type == AUD_DEC_INDIVIDUAL_CH)
    {
        switch (prChlVol->u.t_ch_vol.e_out_port)
        {
        case AUD_DEC_OUT_PORT_2_CH:       // This is for dmx channel
        case AUD_DEC_OUT_PORT_SPEAKER: // doing the same thing with "AUD_DEC_OUT_PORT_2_CH"
            switch (prChlVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    _arChVol[u1DecId].u1VolDmxL = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_DMX_LEFT;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    _arChVol[u1DecId].u1VolDmxR = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_DMX_RIGHT;
                    break;
                default:
                    break;
            }
            break;
        case AUD_DEC_OUT_PORT_5_1_CH:
            switch (prChlVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    _arChVol[u1DecId].u1VolL = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_FRONT_LEFT;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    _arChVol[u1DecId].u1VolR = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_FRONT_RIGHT;
                    break;
                case AUD_DEC_LS_REAR_LEFT:
                    _arChVol[u1DecId].u1VolSL = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_REAR_LEFT;
                    break;
                case AUD_DEC_LS_REAR_RIGHT:
                    _arChVol[u1DecId].u1VolSR = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_REAR_RIGHT;
                    break;
                case AUD_DEC_LS_CENTER:
                    _arChVol[u1DecId].u1VolC = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_CENTER;
                    break;
                case AUD_DEC_LS_SUB_WOOFER:
                    _arChVol[u1DecId].u1VolSW = prChlVol->u.t_ch_vol.ui1_level;
                    eAudCh = AUD_CH_SUB_WOOFER;
                    break;
                default:
                    break;
            }
            break;
        case AUD_DEC_OUT_PORT_SPDIF:
            // Do nothing
            break;
        case AUD_DEC_OUT_PORT_2_CH_BY_PASS:
            switch (prChlVol->u.t_ch_vol.e_ls)
            {
                case AUD_DEC_LS_FRONT_LEFT:
                    _arChVol[u1DecId].u1VolBypassL = prChlVol->u.t_ch_vol.ui1_level;
                        eAudCh = AUD_CH_REAR_LEFT;
                    break;
                case AUD_DEC_LS_FRONT_RIGHT:
                    _arChVol[u1DecId].u1VolBypassR = prChlVol->u.t_ch_vol.ui1_level;
                        eAudCh = AUD_CH_REAR_RIGHT;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
        }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        AUD_DspChannelVolume(AUD_DSP0, u1DecId, eAudCh, prChlVol->u.t_ch_vol.ui1_level);
#else
        AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.t_ch_vol.ui1_level);
#endif
    }
}

static void _SetVolumeEx(UINT8 u1DecId, AUD_DEC_VOLUME_INFO_EX_T * prChlVol)
{
    AUD_CH_T eAudCh = AUD_CH_ALL;
    AUD_DEC_VOLUME_INFO_T rVolInfo;

    LOG(3, "[MT_AUD_MW]_SetVolumeEx(), Dec (%d), Type (%d), Port (%d), Vol (%d)\n",
        u1DecId, (UINT8)prChlVol->e_vol_type, (UINT8)prChlVol->e_out_port, prChlVol->u.ui1_level);

    if (prChlVol->e_vol_type == AUD_DEC_ALL_CH)
    {
        rVolInfo.e_vol_type = AUD_DEC_ALL_CH;

        switch (prChlVol->e_out_port)
        {
            case AUD_DEC_OUT_PORT_SPEAKER:
#ifdef CC_SPK_USE_M_VOLUME
            {
                UINT8 u1Volume;

                _arChVol[u1DecId].u1VolMaster = prChlVol->u.ui1_level;
                u1Volume = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_ALL;
                AUD_DspMasterVolume(u1DecId, u1Volume);
             }
#else
                // L Channel
                _arChVol[u1DecId].u1VolL = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_FRONT_LEFT;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, eAudCh, prChlVol->u.ui1_level);
#else
                AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.ui1_level);
#endif
                // R Channel
                _arChVol[u1DecId].u1VolL = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_FRONT_RIGHT;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, eAudCh, prChlVol->u.ui1_level);
#else
                AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.ui1_level);
#endif
#endif
                return;
            case AUD_DEC_OUT_PORT_HEADPHONE:
                // Dmx L Channel
                _arChVol[u1DecId].u1VolDmxL = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_DMX_LEFT;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, eAudCh, prChlVol->u.ui1_level);
#else
                AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.ui1_level);
#endif
                // Dmx R Channel
                _arChVol[u1DecId].u1VolDmxR = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_DMX_RIGHT;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelVolume(AUD_DSP0, u1DecId, eAudCh, prChlVol->u.ui1_level);
#else
                AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.ui1_level);
#endif
                return;

            #ifdef CC_S_SUBWOFFER_SUPPORT
            case AUD_DEC_OUT_PORT_SUBWOOFER:
                // Subwoofer L Channel
                _arChVol[u1DecId].u1VolSW = prChlVol->u.ui1_level;
                eAudCh = AUD_CH_SUB_WOOFER;
                AUD_DspChannelVolume(u1DecId, eAudCh, prChlVol->u.ui1_level);
                return;
            #endif

            default:
                rVolInfo.u.ui1_level = prChlVol->u.ui1_level;
                break;
        }

        _SetVolume(u1DecId, &rVolInfo);
    }
    else if(prChlVol->e_vol_type == AUD_DEC_TEST_TONE)
	{
		rVolInfo.e_vol_type = AUD_DEC_TEST_TONE;
		rVolInfo.u.ui1_level = prChlVol->u.ui1_level;

		_SetVolume(u1DecId, &rVolInfo);
	}
    else if (prChlVol->e_vol_type == AUD_DEC_INDIVIDUAL_CH)
    {
        rVolInfo.e_vol_type = AUD_DEC_INDIVIDUAL_CH;
        rVolInfo.u.t_ch_vol.e_out_port = prChlVol->e_out_port;
        rVolInfo.u.t_ch_vol.e_ls = prChlVol->u.t_ch_vol.e_ls;
        rVolInfo.u.t_ch_vol.ui1_level = prChlVol->u.t_ch_vol.ui1_level;

        _SetVolume(u1DecId, &rVolInfo);
    }
}

static void _SetMute(UINT8 u1DecId, AUD_DEC_MUTE_TYPE_T eMuteType)
{
    BOOL fgSpkOff = FALSE;
    AUD_DEC_ID_VALIDATE(u1DecId);  //Fix Klocwork warning
    LOG(3, "[MT_AUD_MW] _SetMute(), Dec (%d), MuteType = %d\n", u1DecId, (UINT8)eMuteType);
    if (u1DecId < 3)
    {
        _aeMuteType[u1DecId] = eMuteType;
    }

    if (u1DecId == AUD_DEC_MAIN)
    {
        fgSpkOff = (_arAudMuteEx[u1DecId].ui4_muted_out_port_msk & AUD_DEC_OUT_PORT_FLAG_SPEAKER);
        fgSpkOff = (fgSpkOff | (!_fgSpeakerPortOn));
    }

    switch (eMuteType)
    {
        case AUD_DEC_MUTE_OFF:
            _afgMute[u1DecId] = FALSE;

            if ((u1DecId == AUD_DEC_MAIN) && (!fgSpkOff))
            {
                ADAC_SpeakerEnable(TRUE);
            }
            x_thread_delay(50);
            AUD_UnMute(u1DecId);
#ifdef CC_AUD_MIXSOUND_SUPPORT
            AUD_DspMixSndMute(FALSE);
#endif
            break;

        case AUD_DEC_MUTE_ON:
            _afgMute[u1DecId] = TRUE;
#ifdef CC_AUD_MIXSOUND_SUPPORT
            AUD_DspMixSndMute(TRUE);
#endif
            AUD_Mute(u1DecId);
            x_thread_delay(90);
            if (u1DecId == AUD_DEC_MAIN)
            {
                ADAC_SpeakerEnable(FALSE);
            }
            break;

        default:
            break;
    }
}

static void _AudSetMuteEx(UINT8 u1DecId, AUD_DEC_MUTE_EX_INFO_T * eMuteType)
{
    UINT32 u4MuteMask = 0;
    BOOL   fgSpkOff = FALSE;
    BOOL   fgHpOff = FALSE;

    u4MuteMask = eMuteType->ui4_muted_out_port_msk;
    LOG(5, "[MT_AUD_MW]_AudSetMuteEx Dec(%d), MuteMask(0x%x)\n", u1DecId, u4MuteMask);
#ifdef CC_DUAL_AUD_DEC_SUPPORT 
    if (u1DecId == gSubDecId)
    {
        LOG(5, "(Dual)_AudSetMuteEx to SubDec(%d), return directly!!\n", gSubDecId);
        return;
    }
#endif    

#ifdef CC_MT5391_AUD_3_DECODER  //gallen 0822
    // gallen added for monitor scart(scart2) mute/un-mute control
    if ((u1DecId == AUD_DEC_MAIN) && ((u4MuteMask & AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS) != (_arAudMuteEx[AUD_DEC_MAIN].ui4_muted_out_port_msk&AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS)))
    {
        if (u4MuteMask & AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS)
        {
            LOG(5, "[MT_AUD_MW] mute monitor scart output\n");
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, TRUE);
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, TRUE);
#else
            AUD_DspChannelMute(u1DecId, AUD_CH_REAR_LEFT, TRUE);
            AUD_DspChannelMute(u1DecId, AUD_CH_REAR_RIGHT, TRUE);
#endif
            _arAudMuteEx[AUD_DEC_MAIN].ui4_muted_out_port_msk |= AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS;
        }
        else
        {
            LOG(5, "[MT_AUD_MW] un-mute monitor scart output\n");
#ifndef CC_AUD_LINEOUT_VOL_MODE
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, FALSE);
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, FALSE);
#else
            AUD_DspChannelMute(u1DecId, AUD_CH_REAR_LEFT, FALSE);
            AUD_DspChannelMute(u1DecId, AUD_CH_REAR_RIGHT, FALSE);
#endif
#else
            if ((!AUD_GetLineOutVolumeMode(u1DecId)) || (!_afgMute[u1DecId]))
            {
                x_thread_delay(50);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_REAR_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_REAR_RIGHT, FALSE);
#endif
            }
#endif
            _arAudMuteEx[AUD_DEC_MAIN].ui4_muted_out_port_msk &= (~AUD_DEC_OUT_PORT_FLAG_2_CH_BY_PASS);
        }
    }
    else
    {
#endif
    _arAudMuteEx[u1DecId].ui4_muted_out_port_msk = u4MuteMask;

    if (u1DecId == AUD_DEC_MAIN)
    {
        fgSpkOff =  ((!_fgSpeakerPortOn) | _afgMute[u1DecId]);

        if (u4MuteMask & AUD_DEC_OUT_PORT_FLAG_SPEAKER)
        {
            ADAC_SpeakerEnable(FALSE);      //Mute Speaker
        }
        else
        {
            if (!fgSpkOff)
            {
                ADAC_SpeakerEnable(TRUE);       //Unmute Speaker
            }
        }

        //Headphone
        fgHpOff = !_fgHeadphonePortOn;
        if (u4MuteMask & AUD_DEC_OUT_PORT_FLAG_HEADPHONE)
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, TRUE);
            AUD_DspChannelMute(AUD_DSP0,u1DecId, AUD_CH_DMX_RIGHT, TRUE);
#else
            AUD_DspChannelMute(u1DecId, AUD_CH_DMX_LEFT, TRUE);
            AUD_DspChannelMute(u1DecId, AUD_CH_DMX_RIGHT, TRUE);
#endif
        }
        else
        {
            if (!fgHpOff)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0,u1DecId, AUD_CH_DMX_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0,u1DecId, AUD_CH_DMX_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_DMX_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_DMX_RIGHT, FALSE);
#endif
           }
        }
    }
#ifdef CC_MT5391_AUD_3_DECODER  //gallen 0822
    }
#endif
}

static void _SetSpdif(UINT8 u1DecId, AUD_DEC_SPDIF_TYPE_T eSpdif)
{
    UNUSED(u1DecId);
    _eSpdif = eSpdif;

    LOG(3, "[MT_AUD_MW]%s, type(%d)\n", __FUNCTION__, eSpdif);
    switch(eSpdif)
    {
        case AUD_DEC_SPDIF_OFF:
            AUD_DspIECConfig(AUD_IEC_CFG_PCM, FALSE);
            break;
        case AUD_DEC_SPDIF_RAW:
            AUD_DspIECConfig(AUD_IEC_CFG_RAW, TRUE);
            break;
        case AUD_DEC_SPDIF_PCM_16:
        case AUD_DEC_SPDIF_PCM_24:
            AUD_DspIECConfig(AUD_IEC_CFG_PCM, TRUE);
            break;
        default:
            break;
    }
}

static void _SetHdcdfilter(UINT8 u1DecId, AUD_DEC_HDCD_FLTR_T eHdcdFilter)
{
    _aeHdcdFilter[u1DecId] = eHdcdFilter;
}

static void _SetLsMode(UINT8 u1DecId, AUD_DEC_CH_LS_MODE_T * prChLsMode)
{
    UINT8 u1LMode;
    UINT8 u1RMode;
    UINT8 u1LsMode;
    UINT8 u1RsMode;
    UINT8 u1CMode;

    _aeLsMode[u1DecId][prChLsMode->e_ls] = prChLsMode->e_mode;

    u1LMode = (_aeLsMode[u1DecId][AUD_DEC_LS_FRONT_LEFT] == AUD_DEC_LS_MODE_SMALL_FREQ_RANGE) ? 0 : 1;
    u1RMode = (_aeLsMode[u1DecId][AUD_DEC_LS_FRONT_RIGHT] == AUD_DEC_LS_MODE_SMALL_FREQ_RANGE) ? 0 : 1;
    u1LsMode = (_aeLsMode[u1DecId][AUD_DEC_LS_REAR_LEFT] == AUD_DEC_LS_MODE_SMALL_FREQ_RANGE) ? 0 : 1;
    u1RsMode = (_aeLsMode[u1DecId][AUD_DEC_LS_REAR_RIGHT] == AUD_DEC_LS_MODE_SMALL_FREQ_RANGE) ? 0 : 1;
    u1CMode = (_aeLsMode[u1DecId][AUD_DEC_LS_CENTER] == AUD_DEC_LS_MODE_SMALL_FREQ_RANGE) ? 0 : 1;

    AUD_DspSpeakerLsConfig(u1DecId, u1LMode, u1RMode, u1LsMode, u1RsMode, u1CMode);
}

static void _SetChEqLoadPresetChgMode(UINT8 u1DecId, AUD_DEC_EQ_TYPE_T eEqType)
{
    AUD_EQ_TYPE_T eEqTypeDrv = AUD_EQ_OFF;

    if (eEqType == AUD_DEC_EQ_OFF)
    {
        eEqTypeDrv = AUD_EQ_OFF;
    }
    else if (eEqType == AUD_DEC_EQ_ROCK)
    {
        eEqTypeDrv = AUD_EQ_ROCK;
    }
    else if (eEqType == AUD_DEC_EQ_POP)
    {
        eEqTypeDrv = AUD_EQ_POP;
    }
    else if (eEqType == AUD_DEC_EQ_LIVE)
    {
        eEqTypeDrv = AUD_EQ_LIVE;
    }
    else if (eEqType == AUD_DEC_EQ_DANCE)
    {
        eEqTypeDrv = AUD_EQ_DANCE;
    }
    else if (eEqType == AUD_DEC_EQ_TECHNO)
    {
        eEqTypeDrv = AUD_EQ_TECHNO;
    }
    else if (eEqType == AUD_DEC_EQ_CLASSIC)
    {
        eEqTypeDrv = AUD_EQ_CLASSIC;
    }
    else if (eEqType == AUD_DEC_EQ_SOFT)
    {
        eEqTypeDrv = AUD_EQ_SOFT;
    }
    else if (eEqType == AUD_DEC_EQ_USER_1)
    {
        eEqTypeDrv = AUD_EQ_CUST1;
    }
    else if (eEqType == AUD_DEC_EQ_USER_2)
    {
        eEqTypeDrv = AUD_EQ_CUST2;
    }
    else if (eEqType == AUD_DEC_EQ_USER_3)
    {
        eEqTypeDrv = AUD_EQ_CUST3;
    }
    else if (eEqType == AUD_DEC_EQ_USER_4)
    {
        eEqTypeDrv = AUD_EQ_CUST4;
    }
    else if (eEqType == AUD_DEC_EQ_USER_5)
    {
        eEqTypeDrv = AUD_EQ_CUST5;
    }
    else if (eEqType == AUD_DEC_EQ_USER_6)
    {
        eEqTypeDrv = AUD_EQ_CUST6;
    }
    else if (eEqType == AUD_DEC_EQ_USER_7)
    {
        eEqTypeDrv = AUD_EQ_CUST7;
    }
    else if (eEqType == AUD_DEC_EQ_USER_8)
    {
        eEqTypeDrv = AUD_EQ_CUST8;
    }
    else if (eEqType == AUD_DEC_EQ_BBE_TV)
    {
        eEqTypeDrv = AUD_EQ_BBE_TV;
    }
    else if (eEqType == AUD_DEC_EQ_BBE_AV)
    {
        eEqTypeDrv = AUD_EQ_BBE_AV;
    }
    else if (eEqType == AUD_DEC_EQ_SBASS)
    {
        eEqTypeDrv = AUD_EQ_SBASS;
    }

    AUD_DspChEqLoadPresetChgMode(u1DecId, eEqTypeDrv);
}

#ifdef CC_AUD_LINEOUT_VOL_MODE
#ifdef CC_AUD_SUPPORT_DUAL_DSP
static void _SetLineOutVolMode(UINT8 u1DecId, AUD_DEC_LINEOUT_VOL_T* prSeInfo)
{
    UINT8 u1Level;

    LOG (3, "[MT_AUD_MW] _SetLineOutVolMode: Mode(%d) Level(%d)\n",
         (UINT8)(prSeInfo->e_mode), (UINT8)(prSeInfo->ui1_level));

    AUD_DspSetLineOutVolumeMode(u1DecId, (UINT8) prSeInfo->e_mode);

    if (prSeInfo->e_mode == AUD_DEC_LINEOUT_VOL_MODE_FIXED)
    {
        AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, FALSE);
        AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, FALSE);

        u1Level =  AUD_DspGetLineOutFixVol();

        if (u1Level == 0)
        {
            AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, (UINT8) prSeInfo->ui1_level);
            AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, (UINT8) prSeInfo->ui1_level);
        }
        else
        {
            AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, u1Level);
            AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, u1Level);
        }
    }
    else
    {
        AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, prSeInfo->b_mute);
        AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, prSeInfo->b_mute);
        AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_LEFT, (UINT8) prSeInfo->ui1_level);
        AUD_DspChannelVolume(AUD_DSP0, u1DecId, AUD_CH_REAR_RIGHT, (UINT8) prSeInfo->ui1_level);
    }
}
#else
static void _SetLineOutVolMode(UINT8 u1DecId, AUD_DEC_LINEOUT_VOL_T* prSeInfo)
{
    UINT8 u1Level;

    LOG (3, "[MT_AUD_MW] _SetLineOutVolMode: Mode(%d) Level(%d)\n",
         (UINT8)(prSeInfo->e_mode), (UINT8)(prSeInfo->ui1_level));

    AUD_DspSetLineOutVolumeMode(u1DecId, (UINT8) prSeInfo->e_mode);

    if (prSeInfo->e_mode == AUD_DEC_LINEOUT_VOL_MODE_FIXED)
    {
        AUD_DspChannelMute(u1DecId, AUD_CH_REAR_LEFT, FALSE);
        AUD_DspChannelMute(u1DecId, AUD_CH_REAR_RIGHT, FALSE);

        u1Level =  AUD_DspGetLineOutFixVol();

        if (u1Level == 0)
        {
            AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_LEFT, (UINT8) prSeInfo->ui1_level);
            AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_RIGHT, (UINT8) prSeInfo->ui1_level);
        }
        else
        {
            AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_LEFT, u1Level);
            AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_RIGHT, u1Level);
        }
    }
    else
    {
        AUD_DspChannelMute(u1DecId, AUD_CH_REAR_LEFT, prSeInfo->b_mute);
        AUD_DspChannelMute(u1DecId, AUD_CH_REAR_RIGHT, prSeInfo->b_mute);
        AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_LEFT, (UINT8) prSeInfo->ui1_level);
        AUD_DspChannelVolume(u1DecId, AUD_CH_REAR_RIGHT, (UINT8) prSeInfo->ui1_level);
    }
}
#endif
#endif
static void _SetSoundEffect(UINT8 u1DecId, AUD_DEC_SE_INFO_T * prSeInfo)
{
	ASSERT((prSeInfo->e_se_type >= 0) && (prSeInfo->e_se_type < AUD_DEC_SE_NUM));
    _arSeInfo[u1DecId][prSeInfo->e_se_type] =  prSeInfo->pv_info;

    //FIXME

    switch(prSeInfo->e_se_type)
    {
        case AUD_DEC_SE_TRIM:
            break;
        case AUD_DEC_SE_DELAY:
            break;
        case AUD_DEC_SE_SUPERBASS:
            AUD_DspSuperBassEnable(u1DecId, (BOOL)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_EQUALIZER:
            _SetChEqLoadPresetChgMode(u1DecId, (AUD_DEC_EQ_TYPE_T)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_REVERB:
            AUD_DspReverbMode(u1DecId, (AUD_DEC_REVERB_MODE_T)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_BASS:
#ifdef CC_AUD_PEQ_BASS_TREBLE
            AUD_DspPEQBassTreble(u1DecId, 0,GET_BASS_MAP_VAL((UINT8)((UINT32)prSeInfo->pv_info)));
#else
            AUD_DspBassBoostGain(u1DecId, GET_BASS_MAP_VAL((UINT8)((UINT32)prSeInfo->pv_info)));
#endif
            break;
        case AUD_DEC_SE_TREBLE:
#ifdef CC_AUD_PEQ_BASS_TREBLE
            AUD_DspPEQBassTreble(u1DecId, 1,GET_TREBLE_MAP_VAL((UINT8)((UINT32)prSeInfo->pv_info)));
#else
            AUD_DspClearBoostGain(u1DecId, GET_TREBLE_MAP_VAL((UINT8)((UINT32)prSeInfo->pv_info)));
#endif
			break;
        case AUD_DEC_SE_BALANCE:
            AUD_DspBalance(u1DecId, (UINT8)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_POSTDRC:
            break;
        case AUD_DEC_SE_VOLUME:
            break;
        case AUD_DEC_SE_SURROUND:
#ifndef CC_AUD_NEW_POST_MULTI_VS_SUPPORT
            AUD_DspVirtualSurroundEnalbe(u1DecId, (BOOL)((UINT32)prSeInfo->pv_info));
#else
            AUD_DspVirtualSurroundEnalbe(u1DecId, (AUD_DEC_SURROUND_TYPE_T)((UINT32)prSeInfo->pv_info));
#endif
            break;
        case AUD_DEC_SE_SOUND_MODE:
            AUD_DspPostSoundMode(u1DecId, (UINT8)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_TV_POSITION:
#if (defined(CC_SONY_2K14_FLASH_AQ) || defined(CC_MAPLE_CUST_DRV))
            AUD_DspSetTvPosition(u1DecId,(UINT8)((UINT32)prSeInfo->pv_info));
            //Printf("AUD_DspSetTvPosition mode = %d\n", ((UINT32)prSeInfo->pv_info));
#endif
			      break;
        case AUD_DEC_SE_SPH_MODE:
#ifdef CC_AUD_SPEAKER_HEIGHT_MODE
            AUD_DspSetSPHMode((AUD_DEC_SE_SPH_MODE_T)((UINT32)prSeInfo->pv_info));
#endif
            break;
        case AUD_DEC_SE_VOLUME_OFFSET:
#ifdef CC_AUD_VOLUME_OFFSET
            AUD_DspVolumeOffset(u1DecId,(UINT8)((UINT32)prSeInfo->pv_info));
#endif
            break;
        case AUD_DEC_SE_HP_VOLUME_OFFSET:
#ifdef CC_AUD_VOLUME_OFFSET
            AUD_DspHpVolumeOffset(u1DecId, (UINT8)((UINT32)prSeInfo->pv_info));
#endif
            break;
#if defined(CC_AUD_VOLUME_OFFSET) && defined(CC_S_SUBWOFFER_SUPPORT)
        case AUD_DEC_SE_SW_VOLUME_OFFSET:
            AUD_DspSWVolumeOffset(u1DecId, (UINT8)((UINT32)prSeInfo->pv_info));
            break;
#endif
        case AUD_DEC_SE_SOUND_MODE_START:
            AUD_DspPostSoundModeStart(u1DecId, (BOOL)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_CLEAR_SOUND:
#ifdef CC_PHI_DREAMCATCHER_DANGERIOUS
            IMGlueclrsndP_SetClrSnd((BOOL)((UINT32)prSeInfo->pv_info));
#endif
            AUD_DspClearSoundEnable(u1DecId, (BOOL)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_PEQ:
            AUD_DspSetPEQ(u1DecId, (UINT8)((UINT32)prSeInfo->pv_info));
            break;
        case AUD_DEC_SE_LR_CH_VOL_MODE:
#ifdef CC_AUD_CHL_VOL_MODE
			AUD_DspLRChannelVolMode(u1DecId, (UINT8)((UINT32)_arSeInfo[u1DecId].pv_info));
#endif
            break;
#ifdef CC_S_SUBWOFFER_SUPPORT
        case AUD_DEC_SE_SW_CUT_OFF_FREQ:
            AUD_DspSpkSizeLpfCfg(u1DecId, au2SWCutOffMap[(UINT8)((UINT32)prSeInfo->pv_info)]);
            break;
#endif
        default:
            break;
    }
}

static void _SetDownMix(UINT8 u1DecId, AUD_DEC_DOWNMIX_MODE_T eDownmixMode)
{
    _aeDownmixMode[u1DecId] = eDownmixMode;
    LOG(5, "[MT_AUD_MW] _SetDownMix Dec(%d), DownMixMode(%d)\n", u1DecId, eDownmixMode);

    switch (eDownmixMode)
    {
        case AUD_DEC_DM_OFF:
            AUD_DspSpeakerOutputConfig(u1DecId, 7); // 3/2
            break;
        case AUD_DEC_DM_LT_RT:
            AUD_DspSpeakerOutputConfig(u1DecId, 0); // 2/0 LtRt
            break;
        case AUD_DEC_DM_STEREO:
            AUD_DspSpeakerOutputConfig(u1DecId, 2); // 2/0 LoRo
            break;
        case AUD_DEC_DM_VIR_SURR:
            AUD_DspSpeakerOutputConfig(u1DecId, 0); // 2/0 LtRt
            break;
        case AUD_DEC_DM_MONO:
            AUD_DspSpeakerOutputConfig(u1DecId, 2); // 2/0 LoRo
            break;
        case AUD_DEC_DM_AUTO_LTRT:
            AUD_DspSpeakerOutputConfig(u1DecId, 0x40);    ///< Auto LtRt downmix
            break;
        case AUD_DEC_DM_AUTO_LORO:
            AUD_DspSpeakerOutputConfig(u1DecId, 0x42);     ///< Auto LoRo downmix
            break;
        default:
            break;
    }
}

static void _SetMatrix(UINT8 u1DecId, AUD_DEC_MATRIX_MODE_T eMatrixMode)
{
    AUD_MATRIX_OUT_T _eMatrixMode = AUD_MATRIX_OUT_OFF;

    _aeMatrixMode[u1DecId] = eMatrixMode;
    LOG(5, "[MT_AUD_MW] _SetMatrix Dec(%d), MatrixMode(%d)\n", u1DecId, eMatrixMode);

    switch (eMatrixMode)
    {
        case AUD_DEC_MTR_DUAL1:
            _eMatrixMode = AUD_MATRIX_OUT_L_MONO;
            break;
        case AUD_DEC_MTR_DUAL2:
            _eMatrixMode = AUD_MATRIX_OUT_R_MONO;
            break;
        case AUD_DEC_MTR_DUAL_MIX:
            _eMatrixMode = AUD_MATRIX_OUT_LR_MIX;
            break;
        default:
            break;
    }

    // Setup matrix out
    if (u1DecId == AUD_DEC_MAIN)
    {
        AUD_DspSetMatrixOutMode(AUD_DEC_MAIN, _eMatrixMode);
    }
    else
    {
        AUD_DspSetMatrixOutMode(AUD_DEC_AUX, _eMatrixMode);
        if (_aeStrSrc[AUD_DEC_MAIN] != AUD_STREAM_FROM_DIGITAL_TUNER)
        {
            AUD_DspSetMatrixOutMode(AUD_DEC_MAIN, AUD_MATRIX_OUT_OFF);
        }
    }
}

static void _SetDrc(UINT8 u1DecId, AUD_DEC_DRC_T eDrc)
{
    _aeDrc[u1DecId] = eDrc;

    LOG(5, "[MT_AUD_MW]_SetDrc(), Dec (%d), Drc(%d)\n", u1DecId, eDrc);
	
#if 0//def CC_MT5391_AUD_3_DECODER
    if ((u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER))
    {
        u1DecId = AUD_DEC_AUX;
    }
#endif
#ifdef  CC_AUD_SUPPORT_MS10
    AUD_DspDdtCompVal ((UINT8)eDrc*10, u1DecId);
#endif
    AUD_DspAC3DRCRange100((UINT8)eDrc*10, u1DecId);

#ifdef CC_MT5391_AUD_3_DECODER
    if (u1DecId == AUD_DEC_MAIN)
    {
		AUD_DspAC3DRCRange100((UINT8)eDrc*10, AUD_DEC_AUX);
#ifdef  CC_AUD_SUPPORT_MS10
		AUD_DspDdtCompVal ((UINT8)eDrc*10, AUD_DEC_AUX);
#endif
    }
#endif
}

static void _SetOutPort(UINT8 u1DecId, UINT32 u4OutPort)
{
    AUD_IEC_T eSpdifOutFormat = AUD_IEC_CFG_PCM;
    BOOL fgSpkOff = FALSE;
    BOOL fgHpOff = FALSE;

    LOG(5, "[MT_AUD_MW]SetOutPort(), Dec (%d), OutPort = 0x%08x\n", u1DecId, u4OutPort);

    if(u1DecId != AUD_DEC_THIRD)
    {
        _u4OutPort = u4OutPort;
    }
    else
    {
        _u4AdOutPort = u4OutPort;
    }

#ifdef CC_AUD_SETUP_MELODY
    if((!AUD_IsSetupMelodyFinish()) && (bApiEepromReadByte(EEP_BOOT_MUSIC_ONOFF) & AUD_POWER_ON_MUSIC_MASK))
    {
        LOG(0,"[MT_AUD_MW]!!! MISS SetOutPort, Dec (%d), OutPort = 0x%08x\n",u1DecId, u4OutPort);
        return;
    }
#endif

    //adde for AD selection
    if (u1DecId == AUD_DEC_THIRD)
    {
        //AD will be turned on/off when play/stop dec3
        if ((u4OutPort & AUD_DEC_OUT_PORT_FLAG_SPEAKER) && (u4OutPort & AUD_DEC_OUT_PORT_FLAG_HEADPHONE))
        {
            AUD_DspADModeSet(AUD_DEC_MAIN, 0x2);
        }
        else if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_SPEAKER)
        {
            AUD_DspADModeSet(AUD_DEC_MAIN, 0x1);
        }
        else if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_HEADPHONE)
        {
            AUD_DspADModeSet(AUD_DEC_MAIN, 0x0);
        }
        else
        {
            AUD_DspADModeSet(AUD_DEC_MAIN, 0x4);    //should set ad mode as default
        }
    }

    if (u1DecId == AUD_DEC_MAIN)
    {
        // Check speaker output port
        if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_SPEAKER)
        {
            fgSpkOff = ((_arAudMuteEx[u1DecId].ui4_muted_out_port_msk) & AUD_DEC_OUT_PORT_FLAG_SPEAKER);
            fgSpkOff = (fgSpkOff | _afgMute[u1DecId]);

            if ((!fgSpkOff))
            {
                ADAC_SpeakerEnable(TRUE);
            }
            _fgSpeakerPortOn = TRUE;
#ifdef CC_SUPPORT_STR
            AUD_SetUiSpkOnOff(TRUE);
#endif            
        }
        else
        {
            _fgSpeakerPortOn = FALSE;
#ifdef CC_SUPPORT_STR
            AUD_SetUiSpkOnOff(FALSE);
#endif               
            ADAC_SpeakerEnable(FALSE);
        }

        // Check SPDIF output port
        if (_eSpdif == AUD_DEC_SPDIF_RAW)
        {
            eSpdifOutFormat = AUD_IEC_CFG_RAW;
        }
        else if (_eSpdif == AUD_DEC_SPDIF_PCM_16)
        {
            eSpdifOutFormat = AUD_IEC_CFG_PCM;
        }
        else if (_eSpdif == AUD_DEC_SPDIF_PCM_24)
        {
            eSpdifOutFormat = AUD_IEC_CFG_PCM;
        }

        if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_SPDIF)
        {
            if (_eSpdif != AUD_DEC_SPDIF_OFF)
            {
                AUD_DspIECConfig(eSpdifOutFormat, TRUE);
            }
        }
        else
        {
            AUD_DspIECConfig(eSpdifOutFormat, FALSE);
        }
        if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_HEADPHONE)
        {
            fgHpOff = ((_arAudMuteEx[u1DecId].ui4_muted_out_port_msk) & AUD_DEC_OUT_PORT_FLAG_HEADPHONE);
            if (!fgHpOff)
            {
                //unmute HP
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, FALSE);
                AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_DMX_RIGHT, FALSE);
#else
                AUD_DspChannelMute(u1DecId, AUD_CH_DMX_LEFT, FALSE);
                AUD_DspChannelMute(u1DecId, AUD_CH_DMX_RIGHT, FALSE);
#endif
            }
            _fgHeadphonePortOn = TRUE;
        }
        else
        {
            //mute HP.
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_DMX_LEFT, TRUE);
            AUD_DspChannelMute(AUD_DSP0, u1DecId, AUD_CH_DMX_RIGHT, TRUE);
#else
            AUD_DspChannelMute(u1DecId, AUD_CH_DMX_LEFT, TRUE);
            AUD_DspChannelMute(u1DecId, AUD_CH_DMX_RIGHT, TRUE);
#endif
            _fgHeadphonePortOn = FALSE;
        }
    }

    // Note that for hardware limitation in MT5351/MT5371,
    // AUD_DEC_OUT_PORT_FLAG_2_CH & AUD_DEC_OUT_PORT_FLAG_5_1_CH cannot open/close individually
    if ((u4OutPort & AUD_DEC_OUT_PORT_FLAG_2_CH) ||
        (u4OutPort & AUD_DEC_OUT_PORT_FLAG_5_1_CH))
    {
        //For semi-standby-->FIX ME!!! It seems pwm dac mute/unmute has pop noise issue
        if ((u1DecId == AUD_DEC_MAIN) && (u4OutPort <= 0x3))
        {
            ADAC_GpioCodecMute(TRUE);
            ADAC_SpeakerEnable(FALSE);
            ADAC_CodecMute(TRUE);
            ADAC_GpioCodecMute(FALSE);
        }
        else
        {
            ADAC_MuteCLK(FALSE);        // Enable pwm clock
            _AUD_OutPadEnable(u1DecId, TRUE);
            ADAC_Enable(TRUE);
            ADAC_Mute(FALSE);
            ADAC_CodecMute(FALSE);
            ADAC_GpioCodecMute(FALSE);
            #ifndef CC_AUD_SOEM_POWER_SEQUENCE
            AUD_PowerOffGpioMute(FALSE);//sunman
            #endif
            AUD_DspDecPowerOffMute(AUD_DEC_MAIN, FALSE);//sunman
        }
        ADAC_SpeakerState_Reinit();
    }
    else
    {
        _AUD_OutPadEnable(u1DecId, FALSE);
        if (u1DecId == AUD_DEC_MAIN)
        {
            // For DC power off
            AUD_DspDecPowerOffMute(AUD_DEC_MAIN, TRUE);
            x_thread_delay(30);    //wait fade out done
            AUD_PowerOffGpioMute(TRUE);
        #ifdef CC_AUD_HP_DEPOP  ///For Remy, need remove "CC_DAC_DE_POP".
            _AudHpDePopTaskStatusChange(AUD_HP_DEPOP_DC_OFF);
        #endif
            ADAC_GpioCodecMute(TRUE);
            ADAC_GpioAmpEnable(FALSE);
            ///x_thread_delay(100);
            ///ADAC_CodecMute(TRUE);
            ADAC_CodecMute(TRUE);
            ADAC_MuteCLK(TRUE); ///PWM CLK Fade-out to 0V.
            x_thread_delay(100);
        }
    }

    if (u1DecId == AUD_DEC_AUX)
    {
        if (u4OutPort & AUD_DEC_OUT_PORT_FLAG_TVSCART)
        {
            AUD_DspChannelOutPort(u1DecId, AUD_CH_FRONT_LEFT, TRUE);
            AUD_DspChannelOutPort(u1DecId, AUD_CH_FRONT_RIGHT, TRUE);
        }
        else
        {
            AUD_DspChannelOutPort(u1DecId, AUD_CH_FRONT_LEFT, FALSE);
            AUD_DspChannelOutPort(u1DecId, AUD_CH_FRONT_RIGHT, FALSE);
        }
    }
}

#if defined(CC_AUD_SX1_FEATURE) || defined (CC_MAPLE_CUST_DRV)
static void _SetOutPortLink(BOOL fgEnable)
{
    ADAC_SpkHpLinkEnable(fgEnable);
}
#endif
static INT32 _SetMemBuffer(UINT8 u1DecId, AUD_DEC_MEM_BUFFER_INFO_T * prMemBuf)
{
    MEM_BUFFER_INFO_T rMemBuf;
    UINT8 u1Data;

    COPY_FROM_USER(prMemBuf->pui1_data, u1Data, sizeof(UINT8));

    rMemBuf.pData = &u1Data;
    rMemBuf.u4Length = prMemBuf->z_len;
    rMemBuf.u4LoopCount = AUD_CLIP_REPEAT_CNT_FOREVER;

    x_memcpy((VOID *)VIRTUAL((UINT32)&_arMemBuf[u1DecId]), prMemBuf, sizeof(AUD_DEC_MEM_BUFFER_INFO_T));
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_SetDecMemBuffer(AUD_DSP0, u1DecId, &rMemBuf);
#else
    AUD_SetDecMemBuffer(u1DecId, &rMemBuf);
#endif
    return RMR_ASYNC_NFY;
}

static void _SetCompressionMode(UINT8 u1DecId, AUD_DEC_CMPSS_MODE_T eCmprssMode)
{
    UINT8 u1cmprssMode = 0;

    LOG(5, "[MT_AUD_MW]_SetCompressionMode Dec(%d), Mode(%d)\n", u1DecId, eCmprssMode);
    _aeCmprssMode[u1DecId] = eCmprssMode;
    switch (eCmprssMode)
    {
        case AUD_DEC_CMPSS_MODE_LINE:
            u1cmprssMode = 2;
            break;
        case AUD_DEC_CMPSS_MODE_CUSTOM0:
            u1cmprssMode = 0;
            break;
        case AUD_DEC_CMPSS_MODE_CUSTOM1:
            u1cmprssMode = 1;
            break;
        case AUD_DEC_CMPSS_MODE_RF:
            u1cmprssMode = 3;
            break;
        default:
            break;
    }
    AUD_DspAC3CompMode(u1cmprssMode, u1DecId);
#ifdef  CC_AUD_SUPPORT_MS10
    AUD_DspDdtDmxRfMode(u1cmprssMode - 2, u1DecId, FALSE);
#endif

    //For Non-EU mode AP don't set AUX decoder becuase of no tv bypass.
    //In addition main and aux compression mode should be always the same.
    //So we set aux compression mode the same as main to avoid aux mode incorrect
#ifdef CC_MT5391_AUD_3_DECODER
    if (u1DecId == AUD_DEC_MAIN)
    {
        AUD_DspAC3CompMode(u1cmprssMode, AUD_DEC_AUX);
        AUD_DspAC3CompMode(u1cmprssMode, AUD_DEC_THIRD);// for AD output level
#ifdef  CC_AUD_SUPPORT_MS10
        AUD_DspDdtDmxRfMode(u1cmprssMode-2, AUD_DEC_AUX, FALSE);
        AUD_DspDdtDmxRfMode(u1cmprssMode-2, AUD_DEC_THIRD, FALSE); // for AD output level
#endif
    }
#endif
}

static void _SetDspSpeed(UINT8 u1DecId, AUD_DEC_DSP_SPEED_MODE_T eDspSpeedMode)
{
    UINT16 u2DspSpeedMode = 0;

    switch (eDspSpeedMode)
    {
        case AUD_DEC_DSP_SPEED_0:
            u2DspSpeedMode = 0;
            break;
        case AUD_DEC_DSP_SPEED_2:
            u2DspSpeedMode = 1<<1;
            break;
        case AUD_DEC_DSP_SPEED_4:
            u2DspSpeedMode = 1<<2;
            break;
        case AUD_DEC_DSP_SPEED_8:
            u2DspSpeedMode = 1<<3;
            break;
	 case AUD_DEC_DSP_SPEED_16:
            u2DspSpeedMode = 1<<4;
	     break;
	 case AUD_DEC_DSP_SPEED_32:
            u2DspSpeedMode = 1<<5;
	     break;
        default:
            break;
    }

    if(u2DspSpeedMode != 0)
    {
        u2DspSpeedMode  --;
    }

    AUD_DspSpeed(u1DecId, u2DspSpeedMode);
}

#ifdef DSP_WMA_MWIF
static BOOL _SetDspWmaInfo(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T* prDspWmaInfo)
{
    AUD_WMA_CFG_T rWMAInfo;
    BOOL fgSupport = TRUE;

    rWMAInfo.u4Pktsz      = prDspWmaInfo->u.t_wma_info.ui4_pkt_sz;
    rWMAInfo.u2Numch      = prDspWmaInfo->u.t_wma_info.ui2_num_ch;
    rWMAInfo.u4Sampersec  = prDspWmaInfo->u.t_wma_info.ui4_sample_per_sec;
    rWMAInfo.u4Bpersec    = prDspWmaInfo->u.t_wma_info.ui4_bits_per_sec;
    rWMAInfo.u4Blocksz    = prDspWmaInfo->u.t_wma_info.ui4_block_sz;
    rWMAInfo.u2Encoderopt = prDspWmaInfo->u.t_wma_info.ui2_encoder_opt;
    rWMAInfo.u2HdrSrc     = prDspWmaInfo->u.t_wma_info.ui2_hdr_src;

    //new for WMAPro
    rWMAInfo.u4ChannelMask= prDspWmaInfo->u.t_wma_info.ui4_channel_mask;
    rWMAInfo.u2BitsPerSample= prDspWmaInfo->u.t_wma_info.ui2_valid_bits;
    rWMAInfo.u2AdvEncoderopt= prDspWmaInfo->u.t_wma_info.ui2_adv_option;
    rWMAInfo.u4AdvEncoderopt2= prDspWmaInfo->u.t_wma_info.ui4_adv_option2;

    if (prDspWmaInfo->e_fmt == AUD_DEC_FMT_WMA)
    {
        AUD_DspWMAHeader(u1DecId,&rWMAInfo);
    }
    else if (prDspWmaInfo->e_fmt == AUD_DEC_FMT_WMAPRO)
    {
        AUD_DspWMAProHeader(u1DecId,&rWMAInfo);
        if (rWMAInfo.u4Sampersec > 48000)
        {
            LOG(0,"!! > 48KHz WMAPRO is not Supported !!!\n");
            fgSupport = FALSE;
        }
        else if (rWMAInfo.u2Numch > 6)
        {
            LOG(0,"!! > 6ch WMAPRO is not Supported !!!\n");
            fgSupport = FALSE;
        }
        else if ((rWMAInfo.u2Encoderopt  & 0x1) == 1)        //pure lossless is not defined
        {
            LOG(0,"!! Pure Lossless WMAPRO is not Supported !!!\n");
            fgSupport = FALSE;
        }
        else if (((rWMAInfo.u2AdvEncoderopt >> 13) == 1) || ((rWMAInfo.u2AdvEncoderopt >> 13) == 2) || ((rWMAInfo.u2AdvEncoderopt >> 13) == 7))      //wma10pro LBR v2, v3, not allowed
        {
            LOG(0,"!! LBR v2, v3 WMAPRO is no Supported !!!\n");
            fgSupport = FALSE;
        }
    }
    return fgSupport;
}
#endif

static BOOL _SetDspApeInfo(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T* prSpecInfo)
{
    AUD_APE_CFG_T rAPEInfo;
    BOOL fgSupport = TRUE;

    x_memset((VOID *)VIRTUAL((UINT32)&rAPEInfo), 0, sizeof(AUD_APE_CFG_T));

    rAPEInfo.u2ParseAudOnly         =   prSpecInfo->u.t_ape_info.ui2_audio_only;
    rAPEInfo.u2FileVersion             =   prSpecInfo->u.t_ape_info.ui2_file_version;
    rAPEInfo.u4CompressLevel       =   prSpecInfo->u.t_ape_info.ui4_compress_level;
    rAPEInfo.u4BlockPerFrm           =   prSpecInfo->u.t_ape_info.ui4_block_per_frame;
    rAPEInfo.u4FinalFrmBlock         =   prSpecInfo->u.t_ape_info.ui4_final_frame_block;
    rAPEInfo.u2TotalFrmNumLow    =   prSpecInfo->u.t_ape_info.ui2_total_frame_num_low;
    rAPEInfo.u2TotalFrmNumHigh   =   prSpecInfo->u.t_ape_info.ui2_total_frame_num_high;
    rAPEInfo.u2Bpersample           =   prSpecInfo->u.t_ape_info.ui2_bits_per_sample;
    rAPEInfo.u2CHNum                 =   prSpecInfo->u.t_ape_info.ui2_channel_num;
    rAPEInfo.u4SamplingRate        =   prSpecInfo->u.t_ape_info.ui4_input_sampling_rate;
    //rAPEInfo.u2MuteBankNum       =   prSpecInfo->u.t_ape_info.ui2_mute_bank_num;
    //rAPEInfo.u2InvalidBytes          =   prSpecInfo->u.t_ape_info.ui2_invalid_bytes;
    
    AUD_DspAPEHeader(u1DecId, &rAPEInfo);
    if ((rAPEInfo.u4CompressLevel == 3000) && (rAPEInfo.u4SamplingRate > 96000))
    {
        LOG(0, "Level 3 > 96KHz is not supported\n");
        fgSupport = FALSE;
    }
    else if ((rAPEInfo.u4CompressLevel == 4000) && (rAPEInfo.u4SamplingRate > 48000))
    {
        LOG(0, "Level 4 > 48KHz is not supported\n");
        fgSupport = FALSE;
    }
    else if (rAPEInfo.u4CompressLevel == 5000)
    {
        LOG(0, "Level 5 is not supported\n");
        fgSupport = FALSE;

    }
    LOG(0, "[MT_AUD_MW]_SetDspApeInfo");

    return fgSupport;

}
static void _SetRmSpecInfo(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T * prSpecInfo)
{
#ifdef ENABLE_MULTIMEDIA
    AUD_DEC_COOK_INFO_T *prInfo = &prSpecInfo->u.t_cook_info;
    UINT8 *pu1Ptr = NULL;
    UINT8 u1RalbrMajorVer, u1RalbrMinorVer;
    AUD_COOK_CFG_T rCookCfg;
    COOK_INFO_T rCookInfo = {0};

    if (prInfo)
    {
        pu1Ptr = (UINT8 *)prInfo->pv_codec_opaque_data;
    }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_MULTI_MEDIA, AUD_FMT_COOK);
#else
    AUD_SetDecType(u1DecId, AUD_STREAM_FROM_MULTI_MEDIA, AUD_FMT_COOK);
#endif
    if (prInfo && pu1Ptr)
    {
        rCookInfo.u1FlavorID = prInfo->ui2_flavor_idx;
        //ASSERT(rCookInfo.u1FlavorID <= 29);
        if (rCookInfo.u1FlavorID <= 29)
        {
            rCookInfo.u4SamplePerFrame = _au4CookSampPerFrame[rCookInfo.u1FlavorID];
        }
        rCookInfo.u1ChanNum = prInfo->ui2_num_channel;
        rCookInfo.u4CookSampRate = prInfo->ui4_sample_rate >> 16;
        rCookInfo.u2FrameSzInBytes = prInfo->ui2_codec_frm_sz;

        u1RalbrMajorVer = *pu1Ptr;
        u1RalbrMinorVer = *(pu1Ptr + 3);
        rCookInfo.u2RegnNum = (*(pu1Ptr + 6) << 8) | (*(pu1Ptr + 7));
        if (u1RalbrMajorVer >= 1 && u1RalbrMinorVer >= 3)
        {
            rCookInfo.u2StartRegn = (*(pu1Ptr + 12) << 8) | (*(pu1Ptr + 13));
            rCookInfo.u2CplQBits= (*(pu1Ptr + 14) << 8) | (*(pu1Ptr + 15));
        }
        else
        {
            rCookInfo.u2StartRegn = rCookInfo.u2CplQBits = 0;
        }

        rCookInfo.u2CookBlockPerSB = prInfo->ui2_intlv_factor;
        rCookInfo.u2CookBlockSz = prInfo->ui2_intlv_blk_sz;
        rCookInfo.u2CookFramePerBlock = rCookInfo.u2CookBlockSz / rCookInfo.u2FrameSzInBytes;
        rCookInfo.u2CookFramePerSB = rCookInfo.u2CookFramePerBlock * rCookInfo.u2CookBlockPerSB;
        rCookInfo.u2CookSBSz = rCookInfo.u2CookBlockSz * rCookInfo.u2CookBlockPerSB;

        rCookCfg.u2ChanNum = rCookInfo.u1ChanNum;
        rCookCfg.u2CplQBits = rCookInfo.u2CplQBits;
        rCookCfg.u2FrameSzInBytes = rCookInfo.u2FrameSzInBytes;
        rCookCfg.u2RegnNum = rCookInfo.u2RegnNum;
        rCookCfg.u2SampPerFrame = rCookInfo.u4SamplePerFrame;
        rCookCfg.u2SampRate = rCookInfo.u4CookSampRate;
        rCookCfg.u2StartRegn = rCookInfo.u2StartRegn;

        AUD_DspCOOKParameter(u1DecId, &rCookCfg);

        #ifdef CC_53XX_SWDMX_V2
        if(u1DecId == AUD_DEC_MAIN)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIOTYPE, ENUM_SWDMX_ADEC_RA, (UINT32)&rCookInfo, 0);
        }
        else if(u1DecId == AUD_DEC_AUX)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIO2TYPE, ENUM_SWDMX_ADEC_RA, (UINT32)&rCookInfo, 0);
        }
        else if(u1DecId == AUD_DEC_THIRD)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIO3TYPE, ENUM_SWDMX_ADEC_RA, (UINT32)&rCookInfo, 0);
        }
        #else
        SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE, ENUM_SWDMX_ADEC_RA, (UINT32)&rCookInfo, 0);
        #endif
    }
    else
    {
        ASSERT(0);
        #ifdef CC_53XX_SWDMX_V2
        if(u1DecId == AUD_DEC_MAIN)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIOTYPE,ENUM_SWDMX_ADEC_RA,0,0);
        }
        else if(u1DecId == AUD_DEC_AUX)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIO2TYPE,ENUM_SWDMX_ADEC_RA,0,0);
        }
        else if(u1DecId == AUD_DEC_THIRD)
        {
            AUD_MW_SWDMX_SetInfo(u1DecId, eSWDMX_SET_AUDIO3TYPE,ENUM_SWDMX_ADEC_RA,0,0);
        }
        #else
        SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,ENUM_SWDMX_ADEC_RA,0,0);
        #endif
    }
#else
    UNUSED(u1DecId);
    UNUSED(prSpecInfo);
#endif
}

static void _SetDspFlacInfo(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T* prDspCodecInfo)//Modified by paul
{
	AUD_FLAC_INFO_T rFLACInfo;
	rFLACInfo.u2MinBlkSz = prDspCodecInfo->u.t_flac_info.ui2_min_block_size;
	rFLACInfo.u2MaxBlkSz = prDspCodecInfo->u.t_flac_info.ui2_max_block_size;
	rFLACInfo.u2ChannelNum = (UINT16)prDspCodecInfo->u.t_flac_info.ui1_aud_channel;
	rFLACInfo.u2BPS = (UINT16)prDspCodecInfo->u.t_flac_info.ui1_aud_sample_bits;
	rFLACInfo.u4SmpRate = prDspCodecInfo->u.t_flac_info.ui4_aud_sample_rate;
	rFLACInfo.u4MinFrmSz = prDspCodecInfo->u.t_flac_info.ui4_min_frame_size;
	rFLACInfo.u4MaxFrmSz = prDspCodecInfo->u.t_flac_info.ui4_max_frame_size;

	AUD_FlacSetting(u1DecId, &rFLACInfo);
}

static void _SetDspPcmInfo(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T* prDspPcmInfo)
{
	AUD_PCM_SETTING_T rPCMInfo;
	AUD_LPCM_INFO_T rLPCMInfo;
	UINT32 u4TmpFS = 0;

	LOG(5,"[MM] PCM set sampling rate %d, BigEndia %d \n",
	    (UINT32)(prDspPcmInfo->u.t_pcm_info.ui4_sample_rate),
	    (BOOL)(prDspPcmInfo->u.t_pcm_info.b_big_endian));

    u4TmpFS = ((UINT32)(prDspPcmInfo->u.t_pcm_info.ui4_sample_rate))/1000;
    switch (u4TmpFS)
    {
    case 48:
        rPCMInfo.eSampleFreq = FS_48K;
        break;
    case 32:
        rPCMInfo.eSampleFreq = FS_32K;
        break;
    case 44:
        rPCMInfo.eSampleFreq = FS_44K;
        break;
    case 8:
        rPCMInfo.eSampleFreq = FS_8K;
        break;
    case 11:
        rPCMInfo.eSampleFreq = FS_11K;
        break;
    case 12:
        rPCMInfo.eSampleFreq = FS_12K;
        break;
   case 16:
        rPCMInfo.eSampleFreq = FS_16K;
        break;
    case 22:
        rPCMInfo.eSampleFreq = FS_22K;
        break;
    case 24:
        rPCMInfo.eSampleFreq = FS_24K;
        break;
    case 5:
        rPCMInfo.eSampleFreq = FS_5K;
        break;
    case 6:
        rPCMInfo.eSampleFreq = FS_6K;
        break;
    case 88:
        rPCMInfo.eSampleFreq = FS_88K;
        break;
	case 96:
        rPCMInfo.eSampleFreq = FS_96K;
         break;
    case 176:
        rPCMInfo.eSampleFreq = FS_176K;
        break;
    case 192:
        rPCMInfo.eSampleFreq = FS_192K;
        break;
    default:
        rPCMInfo.eSampleFreq = FS_48K;
        break;
    }

    if ((BOOL)(prDspPcmInfo->u.t_pcm_info.b_big_endian))
    {
        rPCMInfo.ePcmDataInfo = PCM_16_BITS_BIG_ENDIAN;
    }
    else
    {
        rPCMInfo.ePcmDataInfo = PCM_16_BITS_LITTLE_ENDIAN;
    }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_PcmSetting(AUD_DSP0, u1DecId, &rPCMInfo);
#else
    AUD_PcmSetting(u1DecId, &rPCMInfo);
#endif
    //==== LPCM for MultiMedia ====
    rLPCMInfo.eSampleFreq = rPCMInfo.eSampleFreq;
    rLPCMInfo.b_big_endian = (BOOL)(prDspPcmInfo->u.t_pcm_info.b_big_endian);
    rLPCMInfo.ui2_bits_per_sample = (UINT16)(prDspPcmInfo->u.t_pcm_info.ui2_bits_per_sample);
    rLPCMInfo.ui2_block_align = (UINT16)(prDspPcmInfo->u.t_pcm_info.ui2_block_align);
    rLPCMInfo.ui2_num_ch = (UINT16)(prDspPcmInfo->u.t_pcm_info.ui4_num_ch);
    rLPCMInfo.eLPcmType = (LPCM_INPUT_TYPE_T)(prDspPcmInfo->u.t_pcm_info.e_pcm_type);
    rLPCMInfo.b_info_not_specific = (BOOL)(prDspPcmInfo->u.t_pcm_info.b_info_not_specific);
    rLPCMInfo.b_signed_data = (BOOL)(prDspPcmInfo->u.t_pcm_info.b_signed);
    /*
    printf("LPCM MW Setting : FS %d, big %d, bit %d, blk %d, ch %d, type %d, notS %d sign %d\n",
        rLPCMInfo.eSampleFreq, rLPCMInfo.b_big_endian, rLPCMInfo.ui2_bits_per_sample,
        rLPCMInfo.ui2_block_align,rLPCMInfo.ui2_num_ch, rLPCMInfo.eLPcmType, rLPCMInfo.b_info_not_specific,
        rLPCMInfo.b_signed_data);
    */
    AUD_LPcmSetting(u1DecId, &rLPCMInfo);
}

#ifdef ENABLE_MULTIMEDIA
static void _SetDspAc3Info(UINT8 u1DecId, AUD_DEC_CODEC_SPEC_INFO_T* prDspCodecInfo)
{
    AUD_AC3_SETTING_T rAc3Info;

    LOG (3, "AC3 info: endian = %d\n", prDspCodecInfo->u.t_ac3_info.ui1_endian);

    rAc3Info.eEndian = (DATA_ENDIAN_T) prDspCodecInfo->u.t_ac3_info.ui1_endian;
#ifdef CC_MT5391_AUD_3_DECODER
    if ((u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER))
    {
        u1DecId = AUD_DEC_AUX;
    }
#endif
    AUD_Ac3Setting (u1DecId, &rAc3Info);
}
#endif

static void _SetAgcFlag(UINT8 u1DecId, BOOL fgAgcFlag)
{
    _afgAgcFlag[u1DecId] = fgAgcFlag;
    AUD_DspAVCEnable(u1DecId, fgAgcFlag);
}

static void _SetChlDelay(UINT8 u1DecId, const AUD_DEC_CHNL_DELAY_T * prChlDelay)
{
/*
    u2Delay: 0.05 m/unit
    uChIndex: 0:C 1:L 2:R 3:LS 4:RS 5: reserved 6: reserved 7: SUBWOOFER
*/
	LOG(3, "_SetChlDelay: DecID = %d, type = %d, port = %d, delay = %d\n", u1DecId, prChlDelay->e_ch_delay_type, prChlDelay->e_out_port, prChlDelay->u.ui2_delay);
    if (prChlDelay->e_ch_delay_type == AUD_DEC_CHNL_DELAY_TYPE_ALL)
    {
        VERIFY((prChlDelay->e_out_port) < (AUD_DEC_OUT_PORT_SPEAKER + 1));

        _aau2MasterDelay[u1DecId][prChlDelay->e_out_port]  = prChlDelay->u.ui2_delay;
        if (u1DecId == AUD_DEC_MAIN)
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.ui2_delay , AUD_CH_ALL, AUD_DEC_MAIN);   //Andrew Wen 2007/6/6 simplify all channel delay
#else
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , AUD_CH_ALL, AUD_DEC_MAIN);   //Andrew Wen 2007/6/6 simplify all channel delay
#endif
        #if 0
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 0, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 1, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 2, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 3, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 4, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 5, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 6, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 7, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 8, AUD_DEC_MAIN);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , 9, AUD_DEC_MAIN);
        #endif
        }
#ifndef CC_MT5391_AUD_3_DECODER  //gallen 0521
        else  // AUD_DEC_AUX
#else
        else if (u1DecId == AUD_DEC_AUX)
#endif
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.ui2_delay , AUD_CH_AUX_FRONT_LEFT, AUD_DEC_AUX);
            AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.ui2_delay , AUD_CH_AUX_FRONT_RIGHT, AUD_DEC_AUX);
#else
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , AUD_CH_AUX_FRONT_LEFT, AUD_DEC_AUX);
            AUD_DspChannelDelay(prChlDelay->u.ui2_delay , AUD_CH_AUX_FRONT_RIGHT, AUD_DEC_AUX);
#endif
        }
    }
    else // AUD_DEC_CHNL_DELAY_TYPE_INDIVIDUAL_CH
    {
        switch (prChlDelay->e_out_port)
        {
            case AUD_DEC_OUT_PORT_OFF: // dummy port
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_OFF]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_2_CH:
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_2_CH]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_5_1_CH:
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_5_1_CH]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_c_delay , AUD_CH_CENTER, AUD_DEC_MAIN);
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_fl_delay , AUD_CH_FRONT_LEFT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_fr_delay , AUD_CH_FRONT_RIGHT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_rl_delay , AUD_CH_REAR_LEFT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_rr_delay , AUD_CH_REAR_RIGHT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.t_ind_delay.ui2_sw_delay , AUD_CH_SUB_WOOFER, AUD_DEC_MAIN);
#else
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_c_delay , AUD_CH_CENTER, AUD_DEC_MAIN);
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_fl_delay , AUD_CH_FRONT_LEFT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_fr_delay , AUD_CH_FRONT_RIGHT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_rl_delay , AUD_CH_REAR_LEFT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_rr_delay , AUD_CH_REAR_RIGHT, AUD_DEC_MAIN);
                AUD_DspChannelDelay(prChlDelay->u.t_ind_delay.ui2_sw_delay , AUD_CH_SUB_WOOFER, AUD_DEC_MAIN);
#endif
                break;
            case AUD_DEC_OUT_PORT_SPDIF: // driver not support
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_SPDIF]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.ui2_delay, AUD_CH_BYPASS_LEFT, AUD_DEC_MAIN); //sunman for NorDig
                AUD_DspChannelDelay(AUD_DSP0, prChlDelay->u.ui2_delay, AUD_CH_BYPASS_RIGHT, AUD_DEC_MAIN);
#else
                AUD_DspChannelDelay(prChlDelay->u.ui2_delay, AUD_CH_BYPASS_LEFT, AUD_DEC_MAIN); //sunman for NorDig
                AUD_DspChannelDelay(prChlDelay->u.ui2_delay, AUD_CH_BYPASS_RIGHT, AUD_DEC_MAIN);
#endif
                break;
            case AUD_DEC_OUT_PORT_2_CH_BY_PASS: // driver not support
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_2_CH_BY_PASS]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            case AUD_DEC_OUT_PORT_SPEAKER:
                x_memcpy((VOID *)VIRTUAL((UINT32)&_arChlDelayInd[u1DecId][AUD_DEC_OUT_PORT_SPEAKER]), (const VOID *)&(prChlDelay->u.t_ind_delay),
                                 sizeof(AUD_DEC_CHNL_DELAY_IND_T));
                break;
            default:
                break;
        }
    }
}

static void _setSpdifCpLBit (AUD_DEC_COPY_PROTECT_T eCopyProtect, BOOL *bCpBit_p, BOOL *bLBit_p)
{
    BOOL bCpBit, bLBit;

    switch (eCopyProtect)
    {
    case AUD_DEC_COPY_PROTECT_NO_RESTRICT:  /* Cp-bit = 1, L-bit = 0 */
        bCpBit = 1;
        bLBit = 0;
        break;
    case AUD_DEC_COPY_NO_FURTHER_COPY:      /* Cp-bit = 0, L-bit = 1 */
        bCpBit = 0;
        bLBit = 1;
        break;
    case AUD_DEC_COPY_PROTECT_COPY_ONCE:   /* Cp-bit = 0, L-bit = 0 */
        bCpBit = 0;
        bLBit = 0;
        break;
    case AUD_DEC_COPY_PROTECT_NEVER_COPY:  /* Cp-bit = 0, L-bit = 1 */
        bCpBit = 0;
        bLBit = 1;
        break;
    default:
        bCpBit = 1;
        bLBit = 1;
        break;
    }

    LOG (3, "SPDIF MW setup: copyright = %d, category code = 0x%X\n", bCpBit, bLBit);
    *bCpBit_p = bCpBit;
    *bLBit_p = bLBit;
}

// set copy protection bits, CP bit and L bit
static void _setSpdifCopyProtect (AUD_DEC_COPY_PROTECT_INFO_T *eCopyInfo)
{
    AUD_DEC_COPY_PROTECT_T eCopyProtect;
    AUD_DEC_CP_MODE_T eMode;
    AUD_DEC_STREAM_FROM_T eStreamFrom = _aeStrSrc[AUD_DEC_MAIN]; // now, all SPDIF is controlled by main decoder

    eCopyProtect = eCopyInfo->t_protect_info;
    eMode = eCopyInfo->t_mode;

    LOG (3, "[MT_AUD_MW] _setSpdifCopyProtect: Mode = %d, info = %d\n", eMode, eCopyProtect);

    if (eMode == AUD_DEC_CP_MODE_CI)
    {
        // CI plus Mode
        //eStreamFrom = AUD_STREAM_FROM_DIGITAL_TUNER; // it must be DTV
        if (eCopyProtect != AUD_DEC_COPY_PROTECT_OFF)
        {
            BOOL bCpBit, bLBit;

            // CI setup
            LOG (3, "SPDIF MW setup: use CI plus settings\n");
            AUD_DspSetSpdifUpdateMode (eStreamFrom, IEC_CHANNEL_STATUS_BY_SETUP);
            _setSpdifCpLBit (eCopyProtect, &bCpBit, &bLBit);
            UNUSED(AUD_DspSpdifSetCopyProtect (eStreamFrom, bCpBit, bLBit));
        }
        else
        {
            LOG (3, "SPDIF MW setup: use Default settings\n");
            AUD_DspResetSpdifReg (eStreamFrom);
        }
    }
    else if (eMode == AUD_DEC_CP_MODE_USER)
    {
        BOOL bCpBit, bLBit;

        // User mode
        LOG (3, "SPDIF MW setup: use User settings\n");
        AUD_DspSetSpdifUpdateMode (eStreamFrom, IEC_CHANNEL_STATUS_BY_SETUP);
        _setSpdifCpLBit (eCopyProtect, &bCpBit, &bLBit);
        UNUSED(AUD_DspSpdifSetCopyProtect (eStreamFrom, bCpBit, bLBit));
    }
    else
    {
        // Default Mode
        LOG (3, "SPDIF MW setup: use Default settings\n");
        AUD_DspResetSpdifReg (eStreamFrom);
    }
}

// set SPDIF channel status, copy right, category code
static void _setSpdifCopyRightCategoryCode (UINT8 u1DecId, AUD_DEC_SPDIF_STATUS_INFO_T *eStatusInfo)
{
    AUD_DEC_STREAM_FROM_T eStreamFrom;
    AUD_DEC_CP_MODE_T eMode;
    UNUSED(u1DecId);

    u1DecId = AUD_DEC_MAIN; // now, all SPDIF is controlled by main decoder
    eStreamFrom = _aeStrSrc[AUD_DEC_MAIN];
    eMode = eStatusInfo->t_mode;

    if (eMode == AUD_DEC_CP_MODE_DEFAULT)
    {
        // Default Mode
        LOG (3, "SPDIF MW setup: use Default settings\n");
        AUD_DspResetSpdifReg (eStreamFrom);
    }
    else
    {
        // User mode
        LOG (3, "SPDIF MW setup: use User settings\n");
        AUD_DspSetSpdifUpdateMode (eStreamFrom, IEC_CHANNEL_STATUS_BY_SETUP);
        UNUSED(AUD_DspSpdifSetInfo (eStreamFrom, eStatusInfo->ui1_copy_right, eStatusInfo->ui1_category_code));
    }
}

static void _SetMts(UINT8 u1DecId, AUD_DEC_MTS_T eMts)
{
    UINT32 u1DecoderId = u1DecId;
    UNUSED(u1DecId);

#ifdef CC_MT5391_AUD_3_DECODER
    if (_IsDualDecMode())
    {
        u1DecoderId = AUD_DEC_AUX;
    }
#endif
    LOG(5, "[MT_AUD_MW]_SetMts Dec(%d), u1DecoderId(%d), MTS(%d)\n", u1DecId, u1DecoderId, eMts);

    switch (eMts)
    {
        case AUD_DEC_MTS_MONO:
            AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_MONO);
            break;
        case AUD_DEC_MTS_STERRO:
            AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_STEREO);
            break;
        case AUD_DEC_MTS_SUB_LANG:
            AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_SUB_LANG);
            break;
        case AUD_DEC_MTS_DUAL1:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_DUAL1);
        	break;
        case AUD_DEC_MTS_DUAL2:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_DUAL2);
        	break;
        case AUD_DEC_MTS_NICAM_MONO:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_NICAM_MONO);
        	break;
        case AUD_DEC_MTS_NICAM_STEREO:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_NICAM_STEREO);
        	break;
        case AUD_DEC_MTS_NICAM_DUAL1:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_NICAM_DUAL1);
        	break;
        case AUD_DEC_MTS_NICAM_DUAL2:
        	AUD_DspSetAtvOutputMode(u1DecoderId, AUD_SOUND_MODE_NICAM_DUAL2);
        	break;
        case AUD_DEC_MTS_FM_MONO:
        	AUD_DspSetAtvOutputMode(AUD_DEC_MAIN, AUD_SOUND_MODE_FM_MONO);
        	break;
        case AUD_DEC_MTS_FM_STEREO:
        	AUD_DspSetAtvOutputMode(AUD_DEC_MAIN, AUD_SOUND_MODE_FM_STEREO);
        	break;
        default:
            break;
    }
}

static void _SetUserEqLevel(UINT8 u1DecId, AUD_DEC_SE_QA_USER_LVL_T * prEqLvl)
{
    VERIFY(prEqLvl->ui1_band_idx < CH_EQ_BAND_NUM);
    VERIFY(prEqLvl->e_eq_type < (AUD_DEC_EQ_BBE_AV + 1));
    if (prEqLvl->b_is_level_ex)
    {
       _ai2UserEqCfg[prEqLvl->e_eq_type][prEqLvl->ui1_band_idx] = prEqLvl->i2_level_ex;
        AUD_DspSetUserDefinedEqCfg(u1DecId, (AUD_EQ_TYPE_T)prEqLvl->e_eq_type, prEqLvl->ui1_band_idx, prEqLvl->i2_level_ex);
    }
    else
    {
        _ai2UserEqCfg[prEqLvl->e_eq_type][prEqLvl->ui1_band_idx] = prEqLvl->i1_level;
        AUD_DspSetUserDefinedEqCfg(u1DecId, (AUD_EQ_TYPE_T)prEqLvl->e_eq_type, prEqLvl->ui1_band_idx, prEqLvl->i1_level);
    }

    // Since the level is in 1/10 dB unit, translate to 1/2 dB unit
}

static void _SetTvSysMask(UINT8 u1DecId, UINT32 u4Mask)
{
    LOG(5, "[MT_AUD_MW]_SetTvSysMask Dec(%d), Mask(0x%x)\n", u1DecId, u4Mask);
    Aud_SetTvSystemMask( u1DecId, u4Mask);
}

static void _SetTvAudSysMask(UINT8 u1DecId, AUD_DEC_TV_AUD_SYS_T* prMask)
{
    _arTvAudSysMask[u1DecId].ui4_tv_sys_mask = prMask->ui4_tv_sys_mask;
    _arTvAudSysMask[u1DecId].ui4_aud_sys_mask = prMask->ui4_aud_sys_mask;
    LOG(5, "[MT_AUD_MW]_SetTvAudSysMask Dec(%d), TV_Mask(0x%x), AUD_Mask(0x%x)\n", u1DecId,
        _arTvAudSysMask[u1DecId].ui4_tv_sys_mask, _arTvAudSysMask[u1DecId].ui4_aud_sys_mask );

    // FIXME !!
    Aud_SetTvSystemMask( u1DecId, prMask->ui4_tv_sys_mask);
}

#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
static void _SetTvAutoDectectSysMask(UINT8 u1DecId, AUD_DEC_AUTO_DETECT_AUD_SYSTEM_INFO_T* prMask)
{
    _arTvAudSysMask[u1DecId].ui4_tv_sys_mask = prMask->ui4_tv_sys;
    _arTvAudSysMask[u1DecId].ui4_aud_sys_mask = prMask->ui4_aud_sys;
    LOG(5, "[MT_AUD_MW]_SetTvAutoDectectSysMask Dec(%d), TV_Mask(0x%x), AUD_Mask(0x%x)\n", u1DecId,
        _arTvAudSysMask[u1DecId].ui4_tv_sys_mask, _arTvAudSysMask[u1DecId].ui4_aud_sys_mask);
    Aud_SetTvAudSystemMask( u1DecId, prMask->ui4_tv_sys, prMask->ui4_aud_sys, prMask->t_country_code);
}
#endif

static INT32 _SetAudClip(UINT8 u1DecId, AUD_DEC_AUD_CLIP_T * prClip)
{
    AUD_CLIP_TYPE_T rAudClip;
    AUD_SETUP_MELODY_TABLE_T *prAudCustomerMelody;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    if (AUD_IsDecoderPlay(AUD_DSP0, u1DecId))
#else
    if (AUD_IsDecoderPlay(u1DecId))
#endif
    {
        return (RMR_DRV_INV_SET_INFO);
    }

    if (prClip->ui4_ch_mask == AUD_DEV_LS_FLAG_NONE)
    {
        return (RMR_OK);
    }

    x_memcpy((VOID *)VIRTUAL((UINT32)&_arAudClipSetType[u1DecId]), prClip, sizeof(AUD_DEC_AUD_CLIP_T));

    x_memset((VOID *)VIRTUAL((UINT32)&rAudClip), 0, sizeof(AUD_CLIP_TYPE_T));

    rAudClip.eAudFmt = AUD_FMT_UNKNOWN;
    rAudClip.eMemBuf.pData = NULL;
    rAudClip.eMemBuf.u4Length = 0;
    rAudClip.eMemBuf.u4LoopCount = 1;
    rAudClip.u4ToneFreq = 1000;

    switch (prClip->e_type)
    {
    case AUD_DEC_AUD_CLIP_TYPE_TONE:
        rAudClip.fgPlayTestTone = TRUE;
        rAudClip.u4ToneFreq = prClip->u.ui4_tone_freq;
        break;
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
    case AUD_DEC_AUD_CLIP_TYPE_MIXSOUND:
        rAudClip.fgPlayTestTone = FALSE;
        rAudClip.eAudFmt = (AUD_FMT_T)prClip->u.t_clip_buff.e_fmt;
        break;
#endif
    case AUD_DEC_AUD_CLIP_TYPE_CLIP_BUFF:
        rAudClip.fgPlayTestTone = FALSE;
        rAudClip.eAudFmt = (AUD_FMT_T)prClip->u.t_clip_buff.e_fmt;
#ifdef  LINUX_TURNKEY_SOLUTION
        pClip_buf = (void *)BSP_AllocAlignedDmaMemory((UINT32)prClip->u.t_clip_buff.z_clip_buff_size, 16);
        u1Clipflag = 1;
        if (pClip_buf == NULL)
        {
            LOG(0, "allocate memory for AUD_CLIP_BUFF fail!\n");
            ASSERT(0);
        }

        if (copy_from_user((VOID *)VIRTUAL((UINT32)pClip_buf), prClip->u.t_clip_buff.pui1_clip_buff,
              (UINT32)prClip->u.t_clip_buff.z_clip_buff_size))
        {
            printk("mtal_ioctl_auddec argument error\n");
            return MTR_ERR_INV;
        }

        rAudClip.eMemBuf.pData = pClip_buf;
#else
        rAudClip.eMemBuf.pData = prClip->u.t_clip_buff.pui1_clip_buff;
#endif
        rAudClip.eMemBuf.u4Length = (UINT32)prClip->u.t_clip_buff.z_clip_buff_size;
        break;
    case AUD_DEC_AUD_CLIP_TYPE_CLIP_CUSTOM:
#ifdef CC_AUD_SETUP_MELODY
        //Hack for poweroff music
        AUD_DspAVCEnable(AUD_DEC_MAIN, 0);
        AUD_DspVirtualSurroundEnalbe(AUD_DEC_MAIN, (BOOL)0);
        AUD_DspBassBoostEnable(AUD_DEC_MAIN, (BOOL)0);
        AUD_DspSuperBassEnable(AUD_DEC_MAIN, (BOOL)0);
        AUD_DspClearBoostEnable(AUD_DEC_MAIN, (BOOL)0);
        AUD_DspChannelEqEnable(AUD_DEC_MAIN, (BOOL)0);
        AUD_DspSetMelodyVol(DRVCUST_OptGet(eAudioPowerOnMusicVol));
        //End
#endif
        rAudClip.fgPlayTestTone = FALSE;
        rAudClip.eAudFmt = AUD_FMT_MP3;//customer music fix mp3 format!!!

        prAudCustomerMelody = (AUD_SETUP_MELODY_TABLE_T*)DRVCUST_OptGet(eAudioCustomerMusicTable);
        if (prAudCustomerMelody == 0)
        {
            LOG(1,"No customer music is prepared !!!!\n");
            return (RMR_OK);
        }
#ifdef  LINUX_TURNKEY_SOLUTION
        pClip_buf = (void *)BSP_AllocAlignedDmaMemory((UINT32)prAudCustomerMelody[prClip->u.e_aud_dec_clip_custom_mode].u4Size, 16);
        u1Clipflag = 1;
        if (pClip_buf == NULL)
        {
            LOG(0, "allocate memory for AUD_CLIP_CUSTOM fail!\n");
            ASSERT(0);
        }
        x_memcpy((VOID *)VIRTUAL((UINT32)pClip_buf), (UINT8*)prAudCustomerMelody[prClip->u.e_aud_dec_clip_custom_mode].prAudSetupMelody,
              prAudCustomerMelody[prClip->u.e_aud_dec_clip_custom_mode].u4Size);
        rAudClip.eMemBuf.pData = pClip_buf;
#else
        rAudClip.eMemBuf.pData =
            (UINT8*)prAudCustomerMelody[prClip->u.e_aud_dec_clip_custom_mode].prAudSetupMelody;
#endif
        rAudClip.eMemBuf.u4Length =
            prAudCustomerMelody[prClip->u.e_aud_dec_clip_custom_mode].u4Size;
        break;
    default:
        return (RMR_DRV_INV_SET_INFO);
    }

    rAudClip.u4RepeatCount = prClip->ui4_repeat_cnt;
    rAudClip.u4OutputChlSelect = prClip->ui4_ch_mask;
    rAudClip.u4AudCtrlTypeAddr = (UINT32)AUD_DEC_CTRL_PLAY_AUD_CLIP;
    x_memcpy((VOID *)VIRTUAL((UINT32)&_arAudClip[u1DecId]), &rAudClip, sizeof(AUD_CLIP_TYPE_T));
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
    if (prClip->e_type == AUD_DEC_AUD_CLIP_TYPE_MIXSOUND)
    {
        LOG(5,"[MixClip] AUD_DEC_CTRL_SET: %d\n", prClip->u.t_clip_buff.ui4_PCM_SampleRate);
        AUD_EnableMixSndClip();

        if (!AUD_SetMixSndClip(0,//u1StreamID
                                (UINT32)prClip->u.t_clip_buff.pui1_clip_buff,
                                (UINT32)prClip->u.t_clip_buff.z_clip_buff_size,
                                0x10000, //TODO
                                (prClip->u.t_clip_buff.e_PCM_channel_type == AUD_DEC_CHANNEL_TYPE_STEREO) ? TRUE : FALSE,
                                prClip->u.t_clip_buff.ui4_PCM_SampleRate,
                                prClip->u.t_clip_buff.ui1_PCM_BitDepth,
                                prClip->u.t_clip_buff.ui1_PCM_Endian)
                                )
        {
            return (RMR_DRV_ERROR);
        }
    }
    else
    {
#endif
#ifdef CC_AUD_SUPPORT_DUAL_DSP
      AUD_SetAudClip(AUD_DSP0, u1DecId, &_arAudClip[u1DecId]);
#else
      AUD_SetAudClip(u1DecId, &_arAudClip[u1DecId]);
#endif
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
    }
#endif

    return (RMR_OK);
}

static void _SetPrologic2(UINT8 u1DecId, AUD_DEC_PL2_CTRL_INFO_T * prPL2)
{
    UINT8 u1Ctrl, u1Mode;

    x_memcpy((VOID *)VIRTUAL((UINT32)&_rPrologic2Type), prPL2, sizeof(AUD_DEC_PL2_CTRL_INFO_T));
    u1Ctrl = (UINT8)prPL2->e_ctrl;

    switch (prPL2->e_ctrl)
    {
    case AUD_DEC_PL2_CTRL_SWITCH:
        u1Mode = (UINT8)prPL2->u.e_pl2_switch;
        break;

    case AUD_DEC_PL2_CTRL_MODE:
        u1Mode = PL2CTRL_MODE_MANUAL;
        u1Mode |= ((UINT8)prPL2->u.e_pl2_mode) << 1;
        break;

    case AUD_DEC_PL2_CTRL_PANORAMA:
        u1Mode = (UINT8)prPL2->u.b_is_pl2_panorama_on;
        break;

    case AUD_DEC_PL2_CTRL_DIMENSION:
    case AUD_DEC_PL2_CTRL_C_WIDTH:
        u1Mode = (UINT8)prPL2->u.t_pl2_val.ui1_curr_val;
        break;

    default:
        return;
    }
    AUD_DspPL2Config(u1Ctrl, u1Mode);
}

static void _SetDolbyBanner(UINT8 u1DecId, BOOL fgMode)
{
    _afgDolbyBanner[u1DecId] = fgMode;
    AUD_DDBannerEnable(u1DecId, fgMode);
}

static void _SetVideoScramble(UINT8 u1DecId, BOOL fgMode)
{
    UNUSED(u1DecId);
    AUD_VideoScrambled(fgMode);
}


static void _SetOutPortAdj(UINT8 u1DecId, AUD_DEC_OUTPORT_ADJ_T * pOutportAdj)
{
    AUD_DEC_CAPABILITY_INFO_T rDecCap;

    UNUSED(_GetCapability(u1DecId, &rDecCap));

    if (!rDecCap.b_is_outport_adj)
    {   // Not Adjustable .. do nothing ..
        return ;
    }
    else
    {
        if ((pOutportAdj->e_out_port == AUD_DEC_OUT_PORT_2_CH) ||
            (pOutportAdj->e_out_port == AUD_DEC_OUT_PORT_2_CH_BY_PASS))
        {
            if (pOutportAdj->b_is_fixed)
            {
                AUD_AoutPADMux(AUD_CH_FRONT_LEFT, AUD_CH_REAR_LEFT, AUD_CH_CENTER, AUD_CH_FRONT_LEFT, AUD_CH_BYPASS_LEFT, AUD_CH_AUX_FRONT_LEFT, 0);
#ifdef CC_AUD_LINE_OUT_ADJ
                ADAC_DacSetChl((UINT8)DRVCUST_OptGet(eAudioLineoutDACNum), AUD_CHL_BYPASS); // Fixed mode, CH7/8 for line out
#endif
            }
            else
            {
                AUD_AoutPADMux(AUD_CH_FRONT_LEFT, AUD_CH_REAR_LEFT, AUD_CH_CENTER, AUD_CH_FRONT_LEFT, AUD_CH_FRONT_LEFT, AUD_CH_AUX_FRONT_LEFT, 0);
#ifdef CC_AUD_LINE_OUT_ADJ
                ADAC_DacSetChl((UINT8)DRVCUST_OptGet(eAudioLineoutDACNum), AUD_CHL_DOWNMIX); // Variable mode, CH9/10 for line out
#endif
            }
        }
        else if (pOutportAdj->e_out_port == AUD_DEC_OUT_PORT_SPDIF)
        {
            if (pOutportAdj->b_is_fixed)
            {
                AUD_DspIecChannel(AUD_IEC_PCM_CH_7_8);
            }
            else
            {
                AUD_DspIecChannel(AUD_IEC_PCM_CH_L_R);
            }
        }
        else if (pOutportAdj->e_out_port == AUD_DEC_OUT_PORT_HEADPHONE)
        {
            if (pOutportAdj->b_is_fixed)
            {
                LOG(5, "MW sets headphone fixed\n");
				AUD_SetOutPortMode(u1DecId, AUD_DEC_OUTPORT_MODE_LINEOUT);
#ifdef CC_AUD_LINE_OUT_ADJ
                ADAC_DacSetChl((UINT8)DRVCUST_OptGet(eAudioLineoutDACNum), AUD_CHL_LS_RS); // Fix mode, Ls/Rs for line out
#endif
            }
            else
            {
                LOG(5, "MW sets headphone variable\n");
				AUD_SetOutPortMode(u1DecId, AUD_DEC_OUTPORT_MODE_HEADPHONE);
#ifdef CC_AUD_LINE_OUT_ADJ
                ADAC_DacSetChl((UINT8)DRVCUST_OptGet(eAudioLineoutDACNum), AUD_CHL_DOWNMIX); // Variable mode, CH9/10 for headphone
#endif
            }
        }
#ifdef CC_S_SUBWOFFER_SUPPORT
        else if (pOutportAdj->e_out_port == AUD_DEC_OUT_PORT_SUBWOOFER)
        {
        	LOG(5, "MW sets Sub woofer\n");
			AUD_SetOutPortMode(u1DecId, AUD_DEC_OUTPORT_MODE_SUB_WOOFER);
#ifdef CC_AUD_LINE_OUT_ADJ
            ADAC_DacSetChl((UINT8)DRVCUST_OptGet(eAudioLineoutDACNum), AUD_CHL_C_SW);
#endif
        }
#endif
    }
}

static INT32 _SetAvSyncMode(UINT8 u1DecId, AUD_DEC_AV_SYNC_INFO_T eSyncMode)
{
    AV_SYNC_MODE_T eMode;
    BOOL fgIsWithVideo;

    LOG(5, "[MT_AUD_MW] _SetAvSyncMode Dec(%d), SyncMode(%d)\n", u1DecId, eSyncMode);

    if (eSyncMode == AUD_DEC_AV_SYNC_AUD_SLAVE)
    {
        eMode = AV_SYNC_SLAVE;
        fgIsWithVideo = TRUE;
    }
    else if (eSyncMode == AUD_DEC_AV_SYNC_FREE_RUN)
    {
        eMode = AV_SYNC_FREE_RUN;
        fgIsWithVideo = FALSE;
    }
    else if (eSyncMode == AUD_DEC_AV_SYNC_AUD_MASTER)
    {
        eMode = AV_SYNC_AUDIO_MASTER;
        fgIsWithVideo = FALSE;
    }
    else
    {
        eMode = AV_SYNC_SLAVE;
        fgIsWithVideo = TRUE;
    }
#ifdef CC_MT5391_AUD_3_DECODER
    if ( (u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER))
    {
        u1DecId = AUD_DEC_AUX;
    }
#endif

    if ( (u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER) && _fgMMPlusAD[u1DecId] ==TRUE)
    {
	   AUD_SetAvSynMode(AUD_DEC_MAIN, AV_SYNC_SLAVE);
	   AUD_SetAvSynMode(AUD_DEC_AUX, AV_SYNC_AUDIO_MASTER);
	   AUD_SetAvSynMode(AUD_DEC_THIRD, AV_SYNC_SLAVE);
    }
    else
    {
       AUD_SetAvSynMode(u1DecId, eMode);
    }
    AUD_DspSetWithVideo(u1DecId, fgIsWithVideo);
    return RMR_OK;
}

#ifdef CC_AUD_KARAOKE_MODE_SUPPORT
static void _SetMultimediaMode(UINT8 u1DecId, AUD_DEC_MM_MODE_T eMultimediaMode)
{
    switch (eMultimediaMode)
    {
    case AUD_DEC_MM_MODE_NORMAL:
        AUD_SetMultimediaMode(u1DecId,AUD_MM_MODE_NORMAL);
        break;
    case AUD_DEC_MM_MODE_KARAOKE:
        AUD_SetMultimediaMode(u1DecId,AUD_MM_MODE_KARAOKE);
        break;
    case AUD_DEC_MM_MODE_KARAOKE_MIC_PASS:
        AUD_SetMultimediaMode(u1DecId,AUD_MM_MODE_KARAOKE_MIC_PASS);
        break;
    default:
        break;
    }
}
#endif

#ifdef CC_MPEG_LEVEL_ADJ
static void _SetMpegLevelAdjust(UINT8 u1DecId, UINT8 u1Level)
{
    _u1MpegLevel = u1Level;
    u1Level = 256 - u1Level;
    AUD_MpegLevelAdjust(u1Level);
}
#endif

static void _SetDolbyCmpssDrc(UINT8 u1DecId, AUD_DEC_CMPSS_DRC_MODE_T eCmpssMode)
{
    AUD_DEC_ID_VALIDATE_3(u1DecId);  //Fix Klocwork warning

    if (_aeCmprssDrcMode[u1DecId] == eCmpssMode)
    {
        return;
    }
    _aeCmprssDrcMode[u1DecId] = eCmpssMode;
//DTV is always played on aux decoder for 3 decoder architecture
// For HDMI/USB, Main decoder also need set DRC mode
#if 0//def CC_MT5391_AUD_3_DECODER
    if ((u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER))
    {
        u1DecId = AUD_DEC_AUX;
    }
#endif

    switch (eCmpssMode)
    {
        case AUD_DEC_CMPSS_DRC_MODE_COMPRESSED:
            _SetCompressionMode(u1DecId, AUD_DEC_CMPSS_MODE_RF);
            break;
        case AUD_DEC_CMPSS_DRC_MODE_STANDARD:
            _SetCompressionMode(u1DecId, AUD_DEC_CMPSS_MODE_LINE);
            _SetDrc(u1DecId, AUD_DEC_DRC_100);
            break;
        case AUD_DEC_CMPSS_DRC_MODE_OFF:
            _SetCompressionMode(u1DecId, AUD_DEC_CMPSS_MODE_LINE);
            _SetDrc(u1DecId, AUD_DEC_DRC_OFF);
            break;
        default:
            break;
    }

}
//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
static void _SetAudUploadMode(AUD_DEC_UPLOAD_MODE_T eUploadMode)
{
    switch (eUploadMode)
    {
        case AUD_DEC_UPLOAD_OFF:
            AUD_DspSetUploadMode(0);
            break;

        case AUD_DEC_UPLOAD_PCM:
            AUD_DspSetUploadMode(1);
            break;

        case AUD_DEC_UPLOAD_MP3:
            AUD_DspSetUploadMode(2);
            break;

        case AUD_DEC_UPLOAD_SBC:
            AUD_DspSetUploadMode(3);
            break;

        default:
            break;
    }
}
//#endif

//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
static void _AudSetYWUploadEnable(AUD_DEC_UPLOAD_MODE_T eUploadMode)
{
    static UINT8 u1Mode = AUD_DEC_UPLOAD_OFF;

    if( u1Mode == AUD_DEC_UPLOAD_OFF && eUploadMode != AUD_DEC_UPLOAD_OFF)
    {
        AUD_DataUpload_Init();
        AUD_DataUpload_start();
    }
    if(u1Mode != eUploadMode)
    {
        u1Mode = eUploadMode;
    }
    else
    {
        return;
    }
    switch (eUploadMode)
    {
        case AUD_DEC_UPLOAD_OFF:
            AUD_DspSetUploadMode(0);
            AUD_DspUploadDataEnable();
            break;
        case AUD_DEC_UPLOAD_PCM:
            AUD_DspSetUploadMode(6);
            AUD_DspUploadDataEnable();
            break;
        default:
            LOG(0,"Y!W upload not support mode %d !!\n",eUploadMode);
            break;
    }
}
//#endif

static UINT32 _u1ENCMode = AUD_ENC_FMT_TYPE_UNKNOWN;
static BOOL _fgENCFlag = FALSE;

static void _AudGetENCInfo(BOOL * prENCInfo)
{
   	 *prENCInfo=_fgENCFlag;
}

static void _AudSetENCType(AUD_ENC_SETTINGS_T *info)
{
    if( _u1ENCMode != info->e_enc_fmt )
    {
    	_u1ENCMode = info->e_enc_fmt;
    }
#ifdef CC_AUD_LINEIN_USE_DEC3
    AUD_DspSetEncMode(info->e_src_type, info->e_enc_pos);   //ATV PVR, use MW setting
#else
    AUD_DspSetEncMode(AUD_ENC_SRC_TYPE_THIRD+1, AUD_ENC_POS_MODE_INPUT_BEFORE_POST_PROCESSING);
	if(AUD_ENC_FMT_TYPE_MP3 == info->e_enc_fmt)
	{
		//AUD_DspSetEncInfo((UINT32)bitrates);
	}
#endif
}
static void _AudSetENCEnable(BOOL bENCEnable)
{
      _fgENCFlag=bENCEnable;

      if( _u1ENCMode != AUD_ENC_FMT_TYPE_UNKNOWN && bENCEnable == TRUE)
	    {
	        LOG(0,"Audio encoder  enabled.\n");
	        AUD_DataUpload_Init();
	        AUD_DataUpload_start();
	        switch (_u1ENCMode)
        	    {
        	        case AUD_ENC_FMT_TYPE_UNKNOWN:
        	            AUD_DspSetUploadMode(0);
        	            AUD_DspUploadDataEnable();
                    	break;
        	        case AUD_ENC_FMT_TYPE_PCM:
        	            AUD_DspSetUploadMode(1);
        	            AUD_DspUploadDataEnable();
        	            break;
//#ifdef  MP3ENC_SUPPORT
        	       case AUD_ENC_FMT_TYPE_MP3:
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                        AUD_SetDecType(AUD_DSP0, AUD_DEC_4TH,0,25);
#else
                        AUD_SetDecType(AUD_DEC_4TH,0,25);
#endif
#ifndef CC_MT5398
						AUD_DspMixSndDec3(TRUE);
#endif
        	            AUD_DspSetUploadMode(2);
        	            AUD_DspUploadDataEnable();
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                        AUD_DSPCmdPlay(AUD_DSP0, AUD_DEC_4TH);
#else
                        AUD_DSPCmdPlay(AUD_DEC_4TH);
#endif
#ifdef CC_AUD_ARM_RENDER
						AUD_Aproc_SetMp3Enc(TRUE);
#endif

        	            break;
//#endif
#if 0//def SUPPORT_AAC_ENC
					case AUD_ENC_FMT_TYPE_AAC:

						DSP_GetEncMode(&EncDecId, &eEncMode);
						AUD_DspSetUploadMode(10+EncDecId); // AUX=11, THIRD=12
						AUD_DspUploadDataEnable();
#endif
        	        default:
        	            LOG(0,"ENC not support mode %d !!\n",_u1ENCMode);
        	            break;
	        }
       }
     if(bENCEnable == FALSE)
     {
        LOG(0,"Audio encoder  disenabled.\n");
        AUD_DspSetUploadMode(0);
        AUD_DspUploadDataEnable();
#ifndef CC_MT5398
		AUD_DspMixSndDec3(FALSE);
#endif
        if(_u1ENCMode==AUD_ENC_FMT_TYPE_MP3)
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_4TH);
#else
            AUD_DSPCmdStop(AUD_DEC_4TH);
#endif
#ifdef CC_AUD_ARM_RENDER
			AUD_Aproc_SetMp3Enc(FALSE);
#endif
		}

     }

}


#ifdef CC_AUD_SUPPORT_MS10
// DDC control
static void _SetAudDDC (UINT8 u1DecId, AUD_DEC_DDC_INFO_T *info)
{
    BOOL bEnable = FALSE;

    LOG (3, "====> _SetAudDDC(%d): idx = %d, control = 0x%x, id = %d, mixer = %d\n",
    	u1DecId, info->eIndex, info->eControl, info->eAssocId, info->eMixerMode);
#ifdef CC_MT5391_AUD_3_DECODER
    if ((u1DecId == AUD_DEC_MAIN) && (_aeStrSrc[u1DecId] == AUD_STREAM_FROM_DIGITAL_TUNER))
    {
        u1DecId = AUD_DEC_AUX;
    }
#endif

    switch (info->eIndex)
    {
        case AUD_DEC_DDC_CONTROL:
            AUD_DspSetDDCControl (info->eControl, u1DecId);
            break;

        case AUD_DEC_DDC_ID:
            AUD_DspSetDDCStrmId (info->eAssocId, u1DecId);
            if (info->eAssocId)
            {
                bEnable = TRUE;
            }
            break;

        case AUD_DEC_DDC_MIXER:
            AUD_DspSetDDCMixer (info->eMixerMode, u1DecId);
            break;

        case AUD_DEC_DDC_ALL:
            AUD_DspSetDDCControl (info->eControl, u1DecId);
            AUD_DspSetDDCStrmId (info->eAssocId, u1DecId);
            AUD_DspSetDDCMixer (info->eMixerMode, u1DecId);
            if ((info->eAssocId) && (info->eControl))
            {
                bEnable = TRUE;
            }
            break;
    }

    // setup AD
    if(!bEnable)
    {
        AUD_DspSetDDCAdEnable (u1DecId, bEnable);
    }
    _fgAdFlag = bEnable;
}
#endif

#ifdef CC_AUD_BBE_SUPPORT
static void _SetBbeMode(UINT8 u1DecId, AUD_DEC_BBE_MODE_T eBbeMode)
{
    _aeBbeMode = eBbeMode;
    AUD_DspBbeMode(u1DecId, (UINT8)eBbeMode);
}
#endif

#ifdef CC_AUD_DSP_SUPPORT_AUDYSSEY
static void _AudGetAudyssey(AUD_DEC_AUDYSSEY_INFO_T *prAudysseyInfo)
{
    switch (prAudysseyInfo->e_se_type)
    {
    case AUD_DEC_AUDYSSEY_ADV:
        prAudysseyInfo->ui4_info = (UINT32)_aeADVMode;
        break;
    case AUD_DEC_AUDYSSEY_ABX:
        prAudysseyInfo->ui4_info = (UINT32)_aeABXMode;
        break;
    case AUD_DEC_AUDYSSEY_AEQ:
        prAudysseyInfo->ui4_info = (UINT32)_aeAEQMode;
        break;
    default:
        break;
    }
}

static void _AudSetAudyssey(AUD_DEC_AUDYSSEY_INFO_T *prAudysseyInfo)
{
    switch (prAudysseyInfo->e_se_type)
    {
    case AUD_DEC_AUDYSSEY_ADV:
        _aeADVMode = (AUD_DEC_ADV_MODE_T)(prAudysseyInfo->ui4_info);
        AUD_DspADVEnable((UINT8)_aeADVMode);
        break;
    case AUD_DEC_AUDYSSEY_ABX:
        _aeABXMode = (AUD_DEC_ABX_MODE_T)(prAudysseyInfo->ui4_info);
        AUD_DspSetABXEnable((UINT8)_aeABXMode);
        break;
    case AUD_DEC_AUDYSSEY_AEQ:
        _aeAEQMode = (AUD_DEC_AEQ_MODE_T)(prAudysseyInfo->ui4_info);
        AUD_DspSetAEQEnable((BOOL)_aeAEQMode);
        break;
    default:
        break;
    }
}
#endif

static void _SetPIPVideoPathInfo(UINT8 u1DecId, BOOL u1videopath)
{
    AUD_DrvSetPIPVideoPath(u1DecId, u1videopath);
}

#ifdef CC_AUD_MIXSOUND_SUPPORT
static void _AudSetMixSnd(AUD_DEC_BTNSND_INFO_T *prMixSndInfo)
{
#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
    //AUD_DisableMixSndClip(); //FLASHLITE_CONFLICT
    //Printf("[MixClip] AUD_DISABLE_MIXSND_CLIP\n");
#endif
    switch (prMixSndInfo->e_btnsnd_type)
    {
    case AUD_DEC_BTNSND_ENABLE:
        AUD_DspMixSndEnable((BOOL)((UINT32)prMixSndInfo->pv_info));
        break;
    case AUD_DEC_BTNSND_PLAY:
        AUD_DspMixSndControl(1);
        break;
    case AUD_DEC_BTNSND_SRC:
        AUD_DspMixSndClip((UINT8)((UINT32)prMixSndInfo->pv_info));
        break;
    default:
        break;
    }
}

static void _AudGetMixSnd(AUD_DEC_BTNSND_INFO_T *prMixSndInfo)
{
    switch (prMixSndInfo->e_btnsnd_type)
    {
    case AUD_DEC_BTNSND_ENABLE:
        prMixSndInfo->pv_info = (VOID *)((UINT32)(AUD_DspGetMixSndEnable()));
        break;
    case AUD_DEC_BTNSND_SRC:
        prMixSndInfo->pv_info = (VOID *)((UINT32)(AUD_DspGetMixSndClipIdx()));
        break;
    default:
        break;
    }
}
#endif

#ifdef CC_AUD_SKYPE_SUPPORT
static void _AudSetSkypeMix(AUD_DEC_SKYPE_INFO_T *prSkypeInfo)
{
    switch (prSkypeInfo->e_skype_mix_type)
    {
    case AUD_DEC_SKYPE_MIC_MIX:
        AUD_DspAuxMixToMain((BOOL)((UINT32)prSkypeInfo->pv_info));
        break;
    case AUD_DEC_SKYPE_RING_MIX:
        AUD_DspSkypeRingMix((BOOL)((UINT32)prSkypeInfo->pv_info));
        break;
    default:
        break;
    }
}
#endif

static void _AudSetPanFadeEnable(UINT32 *ePanFadeEnable)
{
        LOG(5, "ePanFadeEnable = %d \n", (AUD_DEC_PANFADE_CTRL_T)*ePanFadeEnable);

        AUD_DspAdPanFadeEnable((AUD_DEC_PANFADE_CTRL_T)*ePanFadeEnable);
}

#ifdef CC_AUD_SUPPORT_OSS_ADAPTION_LAYER
void AUD_SetOutDeviceType (AUD_DEC_OUT_DEVICE_T outDevType)
{
	AUD_OSS_ADAP_DEV_T devId;

	LOG (0, "[AUD_SetOutDeviceType]: %d\n", outDevType);
	devId = DEVICE_OUT_USB_HEADPHONE;
	switch(outDevType)
	{
		case AUD_DEC_OUT_DEVICE_BUILD_SPEAKER:
			devId = DEVICE_OUT_MAX;
			break;

		case AUD_DEC_OUT_DEVICE_USB_HEADPHONE:
			devId = DEVICE_OUT_USB_HEADPHONE;
			break;

		case AUD_DEC_OUT_DEVICE_BLUETOOTH_EARPHONE:
			devId = DEVICE_OUT_BLUETOOTH_EARPHONE;
			break;

		default:
			break;
	}
	_AudOssAdap_Enable(devId);
}

void AUD_GetOutDeviceType (AUD_DEC_OUT_DEVICE_T * outDevType)
{
	AUD_OSS_ADAP_DEV_T devId;
	AUD_DEC_OUT_DEVICE_T devIdRet = AUD_DEC_OUT_DEVICE_BUILD_SPEAKER;

	devId = _AudOssAdap_GetDevId();
	switch(devId)
	{
		case DEVICE_OUT_USB_HEADPHONE:
			devIdRet = AUD_DEC_OUT_DEVICE_USB_HEADPHONE;
			break;
		case DEVICE_OUT_BLUETOOTH_EARPHONE:
			devIdRet = AUD_DEC_OUT_DEVICE_BLUETOOTH_EARPHONE;
			break;
		case DEVICE_OUT_MAX:
			devIdRet = AUD_DEC_OUT_DEVICE_BUILD_SPEAKER;
			break;
		default :
			break;
	}
	*outDevType = devIdRet;
}
#endif

#ifdef AUD_SUPPORT_SRS_BYPASS
extern INT8 AUD_GetFlashAQChVolExtraGain(UINT8 u1DecId, UINT8 u4ChNum);

static void _AudSetSRSBypass(AUD_DEC_SRS_BYPASS_MODE_T prSRSBypassInfo)
{
    switch (prSRSBypassInfo)
    {
    case AUD_DEC_SRS_BYPASS_OFF:
        LOG(5,"!!!AUD_DEC_SRS_BYPASS_OFF - Surround!!!");
        isSrsBypass=FALSE;
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_LEFT, _arChVol[AUD_DEC_MAIN].u1VolDmxL );
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_RIGHT, _arChVol[AUD_DEC_MAIN].u1VolDmxR );
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, _arChVol[AUD_DEC_MAIN].u1VolL );
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, _arChVol[AUD_DEC_MAIN].u1VolR );
        AUD_DspMasterVolume(AUD_DEC_MAIN, _arChVol[AUD_DEC_MAIN].u1VolMaster);
        AUD_DspBypassPostProcess(AUD_DSP0, AUD_DEC_MAIN, FALSE);
        AUD_DspChannelVolGain(AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, 0);
        AUD_DspChannelVolGain(AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, 0);
        break;
    case AUD_DEC_SRS_BYPASS_ON:
        LOG(5,"!!!AUD_DEC_SRS_BYPASS_ON - Standard!!!");
        isSrsBypass=TRUE;
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_LEFT, _arChVol[AUD_DEC_MAIN].u1VolMaster);
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_DMX_RIGHT, _arChVol[AUD_DEC_MAIN].u1VolMaster );
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, _arChVol[AUD_DEC_MAIN].u1VolMaster );
        AUD_DspChannelVolume(AUD_DSP0, AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, _arChVol[AUD_DEC_MAIN].u1VolMaster );
        AUD_DspBypassPostProcess(AUD_DSP0, AUD_DEC_MAIN, 0xB000);    //Bypass bassmag, post, adn delay. Keep Trim.
        AUD_DspChannelVolGain(AUD_DEC_MAIN, AUD_CH_FRONT_LEFT, AUD_GetFlashAQChVolExtraGain(AUD_DEC_MAIN, AUD_CH_FRONT_LEFT));
        AUD_DspChannelVolGain(AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT, AUD_GetFlashAQChVolExtraGain(AUD_DEC_MAIN, AUD_CH_FRONT_RIGHT));
        break;
    default:
        break;
    }
}
#endif // AUD_SUPPORT_SRS_BYPASS

static BOOL _IsAudFmtValid(UINT8 u1DecId, AUD_DEC_FMT_INFO_T *prFormatInfo)
{
    return AUD_DspIsFormatValid((UINT32)prFormatInfo->e_fmt);
}

INT32 _MTAUD_AudConnect(DRV_COMP_ID_T*  Upt_comp_id,
                        DRV_CONN_TYPE_T  e_conn_type,
                        const VOID*      Upv_conn_info,
                        UINT8 u1SwdmxSrcId)
{
    DRV_COMP_ID_T* prCompReg;
    UINT8 u1DecId = AUD_DEC_MAIN;
    DRV_COMP_ID_T v_conn_info;
    DRV_COMP_ID_T* pv_conn_info = &v_conn_info;
    AUD_DEC_STREAM_FROM_T eOriStrfrom = AUD_STREAM_FROM_LINE_IN;
    UINT32 u4OriConnectComp;    

    COPY_FROM_USER_comp_id(DRV_COMP_ID_T);
    COPY_FROM_USER(Upv_conn_info,v_conn_info,sizeof(DRV_COMP_ID_T));

	if(!AUD_IsAudInit())
	{
        while (1)
        {
            if (AUD_IsAudInit())
            {
                //DRV_SET_TIMESTAMP(_aszDriverInitStr[DRIVER_INIT_MW_AUD]);
                break;
            }
            x_thread_delay(10);
        }
	}

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    UNUSED(e_conn_type);
    UNUSED(u4OriConnectComp);

    // Convert Decoder ID
#ifndef CC_DUAL_AUD_DEC_SUPPORT    
    UNUSED(_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId));
#else
    LOG(5, "(Dual)_MTAUD_AudConnect, id=%d\n", pt_comp_id->ui2_id);
    u1DecId = pt_comp_id->ui2_id; //Connect is the 1st step, we can not convert ID here. AUD_SetDecType will handle it
#endif
    
#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)
    {
        u1DecId = u1ForceDecId;
    }
#endif
    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;

    // Handle connect
    _au2MwConnectComp[u1DecId] = (UINT32)prCompReg->e_type;
    u4OriConnectComp = (UINT32)prCompReg->e_type;    

    switch (prCompReg->e_type)
    {
        case DRVT_DEMUX_TS_PES_PACKET:
#ifndef CC_MT5391_AUD_3_DECODER
            if (AUD_GetTargetCountry() == COUNTRY_US)
            {
                AUD_SetDecType(u1DecId, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_FMT_AC3);
            }
#endif
#ifdef CC_DUAL_AUD_DEC_SUPPORT    
            //when source turn to hdmi but no hdmi soure input, LR/HP/Spdif channel will be mute 
            // then change source to DTV,  we need to unmute by using function below, otherwise no audio output in mentioned channels
            // when from no input hdmi source to TV, AUD_SetDecType in there is really work.
            if (!AUD_IsDecoderRealPlay(AUD_DSP0, u1DecId))
			AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_DIGITAL_TUNER, AUD_FMT_AC3);


#endif
            //_aeStrSrc[u1DecId]  = AUD_STREAM_FROM_DIGITAL_TUNER;
            eOriStrfrom = AUD_STREAM_FROM_DIGITAL_TUNER;
            break;
        case DRVT_DEMUX_CONTAINER_ES:
            AUD_MMClearEOSFlag(u1DecId);
            AUD_MMClearPesCnt(u1DecId);
            //_aeStrSrc[u1DecId]  = AUD_STREAM_FROM_MULTI_MEDIA;
            eOriStrfrom = AUD_STREAM_FROM_MULTI_MEDIA;
#ifdef ENABLE_MULTIMEDIA
            _au1SwdmxSrcId[u1DecId] = u1SwdmxSrcId;     // Sync code for multimedia, from Isaac 20120130
            AUD_MMSetSwdmxSrcId(u1DecId, u1SwdmxSrcId); // Tell driver the attched swdmx id
#endif
            break;
        case DRVT_AVC_HDMI:
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_HDMI, AUD_FMT_PCM);
#else
            AUD_SetDecType(u1DecId, AUD_STREAM_FROM_HDMI, AUD_FMT_PCM);
#endif
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_HDMI;
            eOriStrfrom = AUD_STREAM_FROM_HDMI;            
            break;

        case DRVT_AVC_AUDIO_INP:
#ifdef CC_DUAL_AUD_DEC_SUPPORT
            AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_LINE_IN_2, AUD_FMT_PCM);
            AUD_AvInputMuxSel((AUD_INPUT_ID_T)prCompReg->ui2_id);
            LOG(0, "AvInputMux = %d\n", (AUD_INPUT_ID_T)prCompReg->ui2_id);
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN_2;
            eOriStrfrom = AUD_STREAM_FROM_LINE_IN_2;            
#else
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_LINE_IN, AUD_FMT_PCM);
#else
            AUD_SetDecType(u1DecId, AUD_STREAM_FROM_LINE_IN, AUD_FMT_PCM);
#endif
            AUD_AvInputMuxSel((AUD_INPUT_ID_T)prCompReg->ui2_id);
            LOG(0, "AvInputMux = %d\n", (AUD_INPUT_ID_T)prCompReg->ui2_id);
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN;
            eOriStrfrom = AUD_STREAM_FROM_LINE_IN;            
#endif            
            break;

        case DRVT_AVC_SPDIF:
 #ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_SPDIF, AUD_FMT_AC3);
 #else
            AUD_SetDecType(u1DecId, AUD_STREAM_FROM_SPDIF, AUD_FMT_AC3);
 #endif
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_SPDIF;
            eOriStrfrom = AUD_STREAM_FROM_SPDIF;            
            break;
        case DRVT_TUNER_SAT_ANA:
        case DRVT_TUNER_CAB_ANA:
        case DRVT_TUNER_TER_ANA:
#ifndef CC_MT5391_AUD_3_DECODER
            AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_MTS);
#else
                if (u1DecId==AUD_DEC_MAIN)
                {
#ifndef CC_DUAL_AUD_DEC_SUPPORT  // Let SetFormat and Ctrl_PLAY to do this ~~                
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#else
                    AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PCM);
#endif
#endif //CC_DUAL_AUD_DEC_SUPPORT
                }
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
                //else if ((u1DecId==AUD_DEC_AUX)&&(_afgAudDecMwIfRealCtrl[AUD_DEC_AUX]==1))
                else if ((u1DecId==AUD_DEC_AUX)&&(AUD_MWCtrl_Get(AUD_DEC_AUX)==1))
                { // for ATV, if dec2 is auto-played before MW connet, then ignore the connect command
                     LOG(3, "Ignore MW connect DEC2 ATV\n");
                }
#endif
                else
                {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    AUD_SetDecType(AUD_DSP0, u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PAL);
#else
                    AUD_SetDecType(u1DecId, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PAL);
#endif
                }
#endif
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_ANALOG_TUNER;
            eOriStrfrom = AUD_STREAM_FROM_ANALOG_TUNER;            
            break;
        case DRVT_AVC_SCART:
            AUD_AvInputMuxSel((AUD_INPUT_ID_T)((prCompReg->ui2_id + AUD_INPUT_ID_SCART_0) & 0xff));
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN_2;
            eOriStrfrom = AUD_STREAM_FROM_LINE_IN_2; 
#else
            //_aeStrSrc[u1DecId] = AUD_STREAM_FROM_LINE_IN;
            eOriStrfrom = AUD_STREAM_FROM_LINE_IN;            
#endif
            break;
        default:
            LOG(1, "ERROR _AudConnect id\n");
            break;
    }

#ifdef CC_DUAL_AUD_DEC_SUPPORT
    // Now Connect is finished. Convert ID for the following steps.
    if (u1DecId == AUD_DEC_SUB)
    {
        _aeStrSrc[AUD_DEC_SUB] = eOriStrfrom;
        _au2MwConnectComp[AUD_DEC_SUB] = u4OriConnectComp;
        LOG(5, "(Dual)_MTAUD_AudConnect Done!, Src[SUB]=%d\n", eOriStrfrom);
        _AudConvertDecId_Dual(u1DecId, &u1DecId, (UINT32)DUAL_AUD_DEC_CONNECT);        
    }
    else
    {
        _AudConvertDecId_Dual(u1DecId, &u1DecId, (UINT32)DUAL_AUD_DEC_CONNECT);                    
        _aeStrSrc[u1DecId] = eOriStrfrom;
        _au2MwConnectComp[u1DecId] = u4OriConnectComp;        
        LOG(5, "(Dual)_MTAUD_AudConnect Done!, Src[%d]=%d\n", u1DecId, eOriStrfrom);        
    }        
#else 
    _aeStrSrc[u1DecId] = eOriStrfrom;       
#endif     

    //set stream from
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_DRVSetStreamFrom(AUD_DSP0, u1DecId,_aeStrSrc[u1DecId]);
#else
    AUD_DRVSetStreamFrom(u1DecId,_aeStrSrc[u1DecId]);
#endif
    return RMR_OK;
}

INT32 _MTAUD_AudDisconnect(DRV_COMP_ID_T* Upt_comp_id,
                        DRV_DISC_TYPE_T  e_disc_type,
                        const VOID*      Upv_disc_info)
{
    DRV_COMP_ID_T* prCompReg;
    UINT8 u1DecId = AUD_DEC_MAIN;
    DRV_COMP_ID_T v_disc_info;
    DRV_COMP_ID_T* pv_disc_info = &v_disc_info;

    COPY_FROM_USER_comp_id(DRV_COMP_ID_T);
    COPY_FROM_USER(Upv_disc_info,v_disc_info,sizeof(DRV_COMP_ID_T));

	if(!AUD_IsAudInit())
	{
        while (1)
        {
            if (AUD_IsAudInit())
            {
                //DRV_SET_TIMESTAMP(_aszDriverInitStr[DRIVER_INIT_MW_AUD]);
                break;
            }
            x_thread_delay(10);
        }
	}

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    UNUSED(e_disc_type);

    // Convert Decoder ID
#ifdef CC_DUAL_AUD_DEC_SUPPORT  
    LOG(5, "(Dual)_MTAUD_AudDisconnect, id=%d\n", pt_comp_id->ui2_id);
    UNUSED(_AudConvertDecId_Dual(pt_comp_id->ui2_id, &u1DecId, (UINT32)DUAL_AUD_DEC_DISCONNECT));
#else
    UNUSED(_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId));
#endif 

#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)
    {
        u1DecId = u1ForceDecId;
        LOG(5, "[MM3]_Aud DisConnect : u1DecId = %d\n", u1DecId);
    }
#endif
#ifdef CC_MT5391_AUD_3_DECODER
    _aeStrSrc[u1DecId]  = AUD_STREAM_FROM_OTHERS;
#endif

#ifdef ENABLE_MULTIMEDIA
    _au1SwdmxSrcId[u1DecId] = 0xff;     // Sync code for multimedia, from Isaac 20120130
    AUD_MMSetSwdmxSrcId(u1DecId, 0);    // (Tell driver) when disconnect, reset it to the default one
#endif

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;

    // FIXME
    // Should do something

    return RMR_OK;
}

INT32 _MTAUD_AudGet(DRV_COMP_ID_T* Upt_comp_id,
                    DRV_GET_TYPE_T  e_get_type,
                    VOID*           Upv_get_info,
                    SIZE_T*         Upz_get_info_len)
{
    UINT8 u1DecId = AUD_DEC_MAIN;
    SIZE_T z_get_info_len;
    SIZE_T* pz_get_info_len = &z_get_info_len;

    COPY_FROM_USER_comp_id(DRV_COMP_ID_T);
    COPY_FROM_USER(Upz_get_info_len,z_get_info_len,sizeof(SIZE_T));

	if(!AUD_IsAudInit())
	{
        while (1)
        {
            if (AUD_IsAudInit())
            {
                //DRV_SET_TIMESTAMP(_aszDriverInitStr[DRIVER_INIT_MW_AUD]);
                break;
            }
            x_thread_delay(10);
        }
	}

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
    {
        if(e_get_type != AUD_DEC_GET_TYPE_OUT_PORT)
        {
            return RMR_OK;  
        }
    }
#endif

    //Convert Decoder ID
#ifdef CC_DUAL_AUD_DEC_SUPPORT  
    LOG(7, "(Dual)_MTAUD_AudGet, %d, 0x%x\n", pt_comp_id->ui2_id, e_get_type);
    UNUSED(_AudConvertDecId_Dual(pt_comp_id->ui2_id, &u1DecId, (UINT32)e_get_type));
#else
    UNUSED(_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId));
#endif  

#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)
    {
        u1DecId = u1ForceDecId; //parson MM3
        LOG(5, "[MM3]_AudGGet : u1DecId = %d, e_get_type=0x%x, %d\n", u1DecId, e_get_type, (e_get_type&0x1ff)-256);
    }
#endif
    // Perform get opertion
    switch (e_get_type)
    {
        case AUD_DEC_GET_TYPE_CTRL:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CTRL_T);
            M_GET_VALIDATE(AUD_DEC_CTRL_T);
            _AudGetCtrl(u1DecId, (AUD_DEC_CTRL_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CTRL_T);
            }
            break;

        case AUD_DEC_GET_TYPE_INP_MODE:
            {
            COPY_FROM_USER_get_info(BOOL);
            M_GET_VALIDATE(BOOL);
            *(BOOL *)pv_get_info = AUD_GetOperationMode(u1DecId);
            COPY_TO_USER_get_info(BOOL);
            }
            break;

        case AUD_DEC_GET_TYPE_FMT:
            {
            COPY_FROM_USER_get_info(AUD_DEC_FMT_INFO_T);
            M_GET_VALIDATE(AUD_DEC_FMT_INFO_T);
            _GetFormat(u1DecId, (AUD_DEC_FMT_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_FMT_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_VOLUME:
            {
            COPY_FROM_USER_get_info(AUD_DEC_VOLUME_INFO_T);
            M_GET_VALIDATE(AUD_DEC_VOLUME_INFO_T);
            _GetVolume(u1DecId, (AUD_DEC_VOLUME_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_VOLUME_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_MUTE:
            {
            COPY_FROM_USER_get_info(AUD_DEC_MUTE_TYPE_T);
            M_GET_VALIDATE(AUD_DEC_MUTE_TYPE_T);
            _GetMute(u1DecId, (AUD_DEC_MUTE_TYPE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_MUTE_TYPE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_SPDIF:
            {
            COPY_FROM_USER_get_info(AUD_DEC_SPDIF_TYPE_T);
            M_GET_VALIDATE(AUD_DEC_SPDIF_TYPE_T);
            _GetSpdif(u1DecId, (AUD_DEC_SPDIF_TYPE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_SPDIF_TYPE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_HDCD_FLTR:
            {
            COPY_FROM_USER_get_info(AUD_DEC_HDCD_FLTR_T);
            M_GET_VALIDATE(AUD_DEC_HDCD_FLTR_T);
            _GetHdcdfilter(u1DecId, (AUD_DEC_HDCD_FLTR_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_HDCD_FLTR_T);
            }
            break;

        case AUD_DEC_GET_TYPE_LS_MODE:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CH_LS_MODE_T);
            M_GET_VALIDATE(AUD_DEC_CH_LS_MODE_T);
            _GetLsMode(u1DecId, (AUD_DEC_CH_LS_MODE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CH_LS_MODE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_SOUND_EFFECT:
            {
            COPY_FROM_USER_get_info(AUD_DEC_SE_INFO_T);
            M_GET_VALIDATE(AUD_DEC_SE_INFO_T);
            _GetSoundEffect(u1DecId, (AUD_DEC_SE_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_SE_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_DOWNMIX:
            {
            COPY_FROM_USER_get_info(AUD_DEC_DOWNMIX_MODE_T);
            M_GET_VALIDATE(AUD_DEC_DOWNMIX_MODE_T);
            _GetDownMix(u1DecId, (AUD_DEC_DOWNMIX_MODE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_DOWNMIX_MODE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_MATRIX:
            {
            COPY_FROM_USER_get_info(AUD_DEC_MATRIX_MODE_T);
            M_GET_VALIDATE(AUD_DEC_MATRIX_MODE_T);
            _GetMatrix(u1DecId, (AUD_DEC_MATRIX_MODE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_MATRIX_MODE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_DRC:
            {
            COPY_FROM_USER_get_info(AUD_DEC_DRC_T);
            M_GET_VALIDATE(AUD_DEC_DRC_T);
            _GetDrc(u1DecId, (AUD_DEC_DRC_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_DRC_T);
            }
            break;

        case AUD_DEC_GET_TYPE_CAPABILITY:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CAPABILITY_INFO_T);
            M_GET_VALIDATE(AUD_DEC_CAPABILITY_INFO_T);
            _GetCapability(u1DecId, (AUD_DEC_CAPABILITY_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CAPABILITY_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_OUT_PORT:
            {
            COPY_FROM_USER_get_info(UINT32);
            M_GET_VALIDATE(UINT32);
            _GetOutPort(u1DecId, (UINT32 *)pv_get_info);
            COPY_TO_USER_get_info(UINT32);
            }
            break;

        case AUD_DEC_GET_TYPE_PCR_ID:
            {
            COPY_FROM_USER_get_info(UINT16);
            M_GET_VALIDATE(UINT16);
            *(UINT16 *)pv_get_info = (UINT16)AUD_GetStcId(u1DecId);
            COPY_TO_USER_get_info(UINT16);
            }
            break;

        case AUD_DEC_GET_TYPE_AUD_INFO:
            {
            COPY_FROM_USER_get_info(AUD_DEC_AUD_INFO_T);
            M_GET_VALIDATE(AUD_DEC_AUD_INFO_T);
            _GetAudInfo(u1DecId, (AUD_DEC_AUD_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_AUD_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_CMPSS_MODE:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CMPSS_MODE_T);
            M_GET_VALIDATE(AUD_DEC_CMPSS_MODE_T);
            _GetCompressionMode(u1DecId, (AUD_DEC_CMPSS_MODE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CMPSS_MODE_T);
            }
            break;

        case AUD_DEC_GET_TYPE_AGC:
            {
            COPY_FROM_USER_get_info(BOOL);
            M_GET_VALIDATE(BOOL);
            _GetAgcFlag(u1DecId, (BOOL *)pv_get_info);
            COPY_TO_USER_get_info(BOOL);
            }
            break;

        case AUD_DEC_GET_TYPE_CHNL_DELAY:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CHNL_DELAY_T);
            M_GET_VALIDATE(AUD_DEC_CHNL_DELAY_T);
            _GetChlDelay(u1DecId, (AUD_DEC_CHNL_DELAY_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CHNL_DELAY_T);
            }
            break;

        case AUD_DEC_GET_TYPE_COPY_PROTECT:
            {
            COPY_FROM_USER_get_info(AUD_DEC_COPY_PROTECT_T);
            M_GET_VALIDATE(AUD_DEC_COPY_PROTECT_T);
            _GetCopyProtect(u1DecId, (AUD_DEC_COPY_PROTECT_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_COPY_PROTECT_T);
            }
            break;

        case AUD_DEC_GET_TYPE_MTS:
            {
            COPY_FROM_USER_get_info(AUD_DEC_MTS_T);
            M_GET_VALIDATE(AUD_DEC_MTS_T);
            _GetMts(u1DecId, (AUD_DEC_MTS_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_MTS_T);
            }
            break;

        case AUD_DEC_GET_TYPE_MAX_CHNL_DELAY:
            {
            COPY_FROM_USER_get_info(UINT8);
            M_GET_VALIDATE(UINT8);
            _GetMaxDelay(u1DecId, (UINT8 *) pv_get_info);
            COPY_TO_USER_get_info(UINT8);
            }
            break;

        case AUD_DEC_GET_TYPE_SE_EQ_USER_NUM_BAND:
            {
            COPY_FROM_USER_get_info(UINT8);
            M_GET_VALIDATE(UINT8);
            _GetUserEqBandNum(u1DecId, (UINT8 *) pv_get_info);
            COPY_TO_USER_get_info(UINT8);
            }
            break;

        case AUD_DEC_GET_TYPE_SE_EQ_USER_LVL:
            {
            COPY_FROM_USER_get_info(AUD_DEC_SE_QA_USER_LVL_T);
            M_GET_VALIDATE(AUD_DEC_SE_QA_USER_LVL_T);
            _GetUserEqLevel(u1DecId, (AUD_DEC_SE_QA_USER_LVL_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_SE_QA_USER_LVL_T);
            }
            break;

        case AUD_DEC_GET_TYPE_SE_EQ_USER_INFO_BAND:
            {
            COPY_FROM_USER_get_info(AUD_DEC_SE_QA_USER_INFO_BAND_T);
            M_GET_VALIDATE(AUD_DEC_SE_QA_USER_INFO_BAND_T);
            _GetUserEqInfo(u1DecId, (AUD_DEC_SE_QA_USER_INFO_BAND_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_SE_QA_USER_INFO_BAND_T);
            }
            break;

        case AUD_DEC_GET_TYPE_TV_SYS_CAPABILITY:
            {
            COPY_FROM_USER_get_info(UINT32);
            M_GET_VALIDATE(UINT32);
            _GetTvSysCapability(u1DecId, (UINT32 *) pv_get_info);
            COPY_TO_USER_get_info(UINT32);
            }
            break;

        case AUD_DEC_GET_TYPE_TV_SYS_MASK:
            {
            COPY_FROM_USER_get_info(UINT32);
            M_GET_VALIDATE(UINT32);
            _GetTvSysMask(u1DecId, (UINT32 *) pv_get_info);
            COPY_TO_USER_get_info(UINT32);
            }
            break;

        case AUD_DEC_GET_TYPE_TV_AUD_SYS_MASK:
            {
            COPY_FROM_USER_get_info(AUD_DEC_TV_AUD_SYS_T);
            M_GET_VALIDATE(AUD_DEC_TV_AUD_SYS_T);
            _GetTvAudSysMask(u1DecId, (AUD_DEC_TV_AUD_SYS_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_TV_AUD_SYS_T);
            }
            break;

        case AUD_DEC_GET_TYPE_AUD_CLIP:
            {
            COPY_FROM_USER_get_info(AUD_DEC_AUD_CLIP_INFO_T);
            M_GET_VALIDATE(AUD_DEC_AUD_CLIP_INFO_T);
            _GetAudClip(u1DecId, (AUD_DEC_AUD_CLIP_INFO_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_AUD_CLIP_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_PL2_CTRL:
            {
            COPY_FROM_USER_get_info(AUD_DEC_PL2_CTRL_INFO_T);
            M_GET_VALIDATE(AUD_DEC_PL2_CTRL_INFO_T);
            _GetPrologic2(u1DecId, (AUD_DEC_PL2_CTRL_INFO_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_PL2_CTRL_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_DOLBY_CERTI_MODE:
            {
            COPY_FROM_USER_get_info(BOOL);
            M_GET_VALIDATE(BOOL);
            _GetDolbyBanner(u1DecId, (BOOL *)pv_get_info);
            COPY_TO_USER_get_info(BOOL);
            }
            break;

        case AUD_DEC_GET_TYPE_OUTPORT_ADJ:
            {
            COPY_FROM_USER_get_info(AUD_DEC_OUTPORT_ADJ_T);
            M_GET_VALIDATE(AUD_DEC_OUTPORT_ADJ_T);
            _GetOutPortAdj(u1DecId, (AUD_DEC_OUTPORT_ADJ_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_OUTPORT_ADJ_T);
            }
            break;

        case AUD_DEC_GET_TYPE_BBE_MODE:
#ifdef CC_AUD_BBE_SUPPORT
            {
            COPY_FROM_USER_get_info(AUD_DEC_BBE_MODE_T);
            M_GET_VALIDATE(AUD_DEC_BBE_MODE_T);
            _GetBbeMode(u1DecId, (AUD_DEC_BBE_MODE_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_BBE_MODE_T);
            }
#else
            LOG(1, "ERROR: BBE function is not supported\n");
#endif
            break;

        case AUD_DEC_GET_TYPE_AV_SYNC:
            {
            COPY_FROM_USER_get_info(AUD_DEC_AV_SYNC_INFO_T);
            M_GET_VALIDATE(AUD_DEC_AV_SYNC_INFO_T);
            _GetAvSyncMode(u1DecId, (AUD_DEC_AV_SYNC_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_AV_SYNC_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_HEADPHONE_COND:
            {
            COPY_FROM_USER_get_info(AUD_DEC_HEADPHONE_COND_T);
            M_GET_VALIDATE(AUD_DEC_HEADPHONE_COND_T);
            _GetHeadphonePCond(u1DecId, (AUD_DEC_HEADPHONE_COND_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_HEADPHONE_COND_T);
            }
            break;

        case AUD_DEC_GET_TYPE_MUTE_EX:
            {
            COPY_FROM_USER_get_info(AUD_DEC_MUTE_EX_INFO_T);
            M_GET_VALIDATE(AUD_DEC_MUTE_EX_INFO_T);
            _GetMuteEx(u1DecId, (AUD_DEC_MUTE_EX_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_MUTE_EX_INFO_T);
            }
            break;

        case AUD_DEC_GET_TYPE_VOLUME_EX:
            {
            COPY_FROM_USER_get_info(AUD_DEC_VOLUME_INFO_EX_T);
            M_GET_VALIDATE(AUD_DEC_VOLUME_INFO_EX_T);
            _GetVolumeEx(u1DecId, (AUD_DEC_VOLUME_INFO_EX_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_VOLUME_INFO_EX_T);
            }
            break;
        case AUD_DEC_GET_TYPE_BTN_SOUND_CTRL:
#ifdef CC_AUD_MIXSOUND_SUPPORT
            {
            COPY_FROM_USER_get_info(AUD_DEC_BTNSND_INFO_T);
            M_GET_VALIDATE(AUD_DEC_BTNSND_INFO_T);
            _AudGetMixSnd((AUD_DEC_BTNSND_INFO_T *) pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_BTNSND_INFO_T);
            }
#else
            LOG(1, "ERROR: Mix sound function is not supported\n");
#endif
            break;
        case AUD_DEC_GET_TYPE_AUDYSSEY_EFFECT:
#ifdef CC_AUD_DSP_SUPPORT_AUDYSSEY
            {
            COPY_FROM_USER_get_info(AUD_DEC_AUDYSSEY_INFO_T);
            M_GET_VALIDATE(AUD_DEC_AUDYSSEY_INFO_T);
            _AudGetAudyssey((AUD_DEC_AUDYSSEY_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_AUDYSSEY_INFO_T);
            }
#else
            LOG(1, "ERROR: Audyssey function is not supported\n");
#endif
            break;
        case AUD_DEC_GET_TYPE_MM_MODE:
#ifdef CC_AUD_KARAOKE_MODE_SUPPORT
            {
            COPY_FROM_USER_get_info(AUD_DEC_MM_MODE_T);
            M_GET_VALIDATE(AUD_DEC_MM_MODE_T);
            _GetMultimediaMode(u1DecId, (AUD_DEC_MM_MODE_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_MM_MODE_T);
            }
#else
            LOG(1, "ERROR: Karaoke function is not supported\n");
#endif
            break;
        case AUD_DEC_GET_TYPE_CLIP_CUSTOM:
            {
            COPY_FROM_USER_get_info(AUD_DEC_AUD_CLIP_CUSTIOM_T);
            M_GET_VALIDATE(AUD_DEC_AUD_CLIP_CUSTIOM_T);
            LOG(3,"_AudGet: AUD_DEC_GET_TYPE_CLIP_CUSTOM");
            _GetCustomerMusicFlag(u1DecId, (AUD_DEC_AUD_CLIP_CUSTIOM_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_AUD_CLIP_CUSTIOM_T);
            }
            break;
		case AUD_DEC_GET_TYPE_POWER_ON_OFF_MUSIC_VOLUME:
#ifdef CC_AUD_SETUP_MELODY
            {
            COPY_FROM_USER_get_info(AUD_DEC_VOLUME_INFO_T);
            M_GET_VALIDATE(AUD_DEC_VOLUME_INFO_T);
            LOG(3,"_AudGet: AUD_DEC_GET_TYPE_POWER_ON_OFF_MUSIC_VOLUME");
            _GetPowerOnMusicVolume(u1DecId, (AUD_DEC_VOLUME_INFO_T *)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_VOLUME_INFO_T);
            }
#endif
            break;
        case AUD_DEC_GET_TYPE_CMPSS_DRC:
            {
            COPY_FROM_USER_get_info(AUD_DEC_CMPSS_DRC_MODE_T);
            M_GET_VALIDATE(AUD_DEC_CMPSS_DRC_MODE_T);
            LOG(3, "_AudGet: AUD_DEC_GET_TYPE_CMPSS_DRC, u1DecId = %d \n", u1DecId);
            _GetCompressionDrcMode(u1DecId, (AUD_DEC_CMPSS_DRC_MODE_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_CMPSS_DRC_MODE_T);
            }
            break;
#ifdef CC_MPEG_LEVEL_ADJ
        case AUD_DEC_GET_TYPE_MPEG_LEVEL_ADJ:
            {
            COPY_FROM_USER_get_info(UINT8);
            M_GET_VALIDATE(UINT8);
            LOG(3, "_AudGet: AUD_DEC_GET_TYPE_CC_MPEG_LEVEL_ADJ, u1DecId = %d \n", (UINT8*)pv_get_info);
            _GetMpegLevelAdjust(u1DecId, (UINT8*)pv_get_info);
            COPY_TO_USER_get_info(UINT8);
            }
            break;
#endif
        case AUD_DEC_GET_TYPE_AUDIO_PTS:
            {
            COPY_FROM_USER_get_info(UINT64);
            M_GET_VALIDATE(UINT64);
            _GetAudPTS(u1DecId, (UINT64 *)pv_get_info);
            COPY_TO_USER_get_info(UINT64);
            }
            break;
//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
        case AUD_DEC_GET_TYPE_UPLOAD_INFO:
            {
            COPY_FROM_USER_get_info(AUD_DEC_UPLOAD_INFO_T);
            M_GET_VALIDATE(AUD_DEC_UPLOAD_INFO_T);
            _GetAudUploadInfo((AUD_DEC_UPLOAD_INFO_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_UPLOAD_INFO_T);
            }
            break;
        case AUD_DEC_GET_TYPE_BUFFER_INFO:
            {
            COPY_FROM_USER_get_info(AUD_DEC_BUFFER_INFO_T);
            M_GET_VALIDATE(AUD_DEC_BUFFER_INFO_T);
            _GetAudYWUploadInfo((AUD_DEC_BUFFER_INFO_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_BUFFER_INFO_T);
            }
            break;
        case AUD_DEC_GET_TYPE_UPLOAD_PCM_INFO:
            {
            COPY_FROM_USER_get_info(AUD_DEC_UPLOAD_PCM_INFO_T);
            M_GET_VALIDATE(AUD_DEC_UPLOAD_PCM_INFO_T);
            _GetAUDUploadPCMInfo((AUD_DEC_UPLOAD_PCM_INFO_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_UPLOAD_PCM_INFO_T);
            }
            break;

		case AUD_ENC_GET_TYPE_CTRL:
			{
			COPY_FROM_USER_get_info(BOOL);
            M_GET_VALIDATE(BOOL);
            _AudGetENCInfo((BOOL *)pv_get_info);
            COPY_TO_USER_get_info(BOOL);
			}
			break;

//#endif
#ifdef CC_AUD_SUPPORT_OSS_ADAPTION_LAYER
        case AUD_DEC_SET_TYPE_OUT_DEVICE:
            {
            COPY_FROM_USER_get_info(AUD_DEC_OUT_DEVICE_T);
            M_GET_VALIDATE(AUD_DEC_OUT_DEVICE_T);
            AUD_GetOutDeviceType((AUD_DEC_OUT_DEVICE_T*)pv_get_info);
            COPY_TO_USER_get_info(AUD_DEC_OUT_DEVICE_T);
            }
            break;
#endif
        case AUD_DEC_GET_TYPE_AFIFO_AMOUNT:
        case AUD_DEC_GET_TYPE_AUD_CODEC_TYPE:
            {
            COPY_FROM_USER_get_info(UINT32);
            M_GET_VALIDATE(UINT32);
            _GetAudFIFO_SZ(u1DecId, (UINT32 *)pv_get_info);
            COPY_TO_USER_get_info(UINT32);
            }
            break;
        case AUD_DEC_GET_TYPE_SPDIF_LEVEL:
        	{
			COPY_FROM_USER_get_info(UINT8);
            M_GET_VALIDATE(UINT8);
            *((UINT8*)pv_get_info) = _u1SpdifLevel;
            COPY_TO_USER_get_info(UINT8);
        	}
        	break;
        default:
            return (RMR_DRV_INV_GET_INFO);
    }

    COPY_TO_USER_z_get_info_len(SIZE_T);

    return RMR_OK;
}

INT32 _MTAUD_AudSet(DRV_COMP_ID_T* Upt_comp_id,
                    DRV_SET_TYPE_T  e_set_type,
                    const VOID*     Upv_set_info,
                    SIZE_T          z_set_info_len)
{
    UINT8 u1DecId = AUD_DEC_MAIN;
    static UINT32 u4Handler = (UINT32)NULL;

    COPY_FROM_USER_comp_id(DRV_COMP_ID_T);

	{
        while (1)
        {
            if (AUD_IsAudInit())
            {
                //DRV_SET_TIMESTAMP(_aszDriverInitStr[DRIVER_INIT_MW_AUD]);
                break;
            }
            x_thread_delay(10);
        }
	}

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
    {
		// when fast boot on,  suspend->resume, mask MW SetOutPort control, result to no audio output
		// save arguments MW set, when setoutport is allowed, set it to driver
        if(e_set_type != AUD_DEC_SET_TYPE_OUT_PORT)
        {
            return RMR_OK;  
        }
		else
		{
		    while (1)
            {
                if (!fgAudPmStrMode)
                {
                    break;
                }
                x_thread_delay(10);
            }
		}
    }
#endif

    //Convert Decoder ID
#ifdef CC_DUAL_AUD_DEC_SUPPORT  
    LOG(7, "(Dual)_MTAUD_AudSet, %d, 0x%x\n", pt_comp_id->ui2_id, e_set_type);
    UNUSED(_AudConvertDecId_Dual(pt_comp_id->ui2_id, &u1DecId, (UINT32)e_set_type));
#else
    UNUSED(_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId));
#endif

#ifdef AUD_DECODER3_SELF_TEST
    if (u1ForceDecId != AUD_DEC_NUM)
    {
        u1DecId = u1ForceDecId; //parson MM3
        LOG(5, "[MM3]_AudSSet : u1DecId = %d, e_get_type=0x%x, %d\n", u1DecId, e_set_type, (e_set_type&0x1ff)-256);
    }
#endif
    // Perform set opertion
    switch (e_set_type)
    {
        case AUD_DEC_SET_TYPE_CTRL:
            M_SET_VALIDATE(AUD_DEC_CTRL_T);
            return _AudSetCtrl(u1DecId, (AUD_DEC_CTRL_T)(UINT32)Upv_set_info);

        case AUD_DEC_SET_TYPE_NFY_FCT:
            {
            COPY_FROM_USER_set_info(AUD_DEC_NFY_INFO_T);
            if (u4Handler == (UINT32)NULL)
            {
                AUD_RegNotifyFunc(&u4Handler, _AudMwNfyFunc);
            }
            M_SET_VALIDATE(AUD_DEC_NFY_INFO_T);
            _AudSetNotifyInfo(u1DecId, (AUD_DEC_NFY_INFO_T *) pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_INP_MODE:
            M_SET_VALIDATE(BOOL);
            AUD_SetOperationMode(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_FMT:
            {
            COPY_FROM_USER_set_info(AUD_DEC_FMT_INFO_T);
            M_SET_VALIDATE(AUD_DEC_FMT_INFO_T);
            if (!_IsAudFmtValid(u1DecId, (AUD_DEC_FMT_INFO_T *)pv_set_info))
            {
                LOG(0, "[MT_AUD_MW] AUD FMT(%d) NOT_SUPPORT: Capability\n", ((AUD_DEC_FMT_INFO_T*)pv_set_info)->e_fmt);
                return RMR_DRV_INV_SET_INFO;
            }
            _SetFormat(u1DecId, (AUD_DEC_FMT_INFO_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_VOLUME:
            {
            COPY_FROM_USER_set_info(AUD_DEC_VOLUME_INFO_T);
            M_SET_VALIDATE(AUD_DEC_VOLUME_INFO_T);
            _SetVolume(u1DecId, (AUD_DEC_VOLUME_INFO_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_MUTE:
            M_SET_VALIDATE(AUD_DEC_MUTE_TYPE_T);
            _SetMute(u1DecId, (AUD_DEC_MUTE_TYPE_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_SPDIF:
            M_SET_VALIDATE(AUD_DEC_SPDIF_TYPE_T);
            _SetSpdif(u1DecId, (AUD_DEC_SPDIF_TYPE_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_HDCD_FLTR:
            M_SET_VALIDATE(AUD_DEC_HDCD_FLTR_T);
            _SetHdcdfilter(u1DecId, (AUD_DEC_HDCD_FLTR_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_LS_MODE:
            {
            COPY_FROM_USER_set_info(AUD_DEC_CH_LS_MODE_T);
            M_SET_VALIDATE(AUD_DEC_CH_LS_MODE_T);
            _SetLsMode(u1DecId, (AUD_DEC_CH_LS_MODE_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_SOUND_EFFECT:
            {
            COPY_FROM_USER_set_info(AUD_DEC_SE_INFO_T);
            M_SET_VALIDATE(AUD_DEC_SE_INFO_T);
            _SetSoundEffect(u1DecId, (AUD_DEC_SE_INFO_T *)pv_set_info);
            }
            break;
#ifdef CC_AUD_LINEOUT_VOL_MODE
        case AUD_DEC_SET_TYPE_LINOUT_VOL_MODE:
            {
            COPY_FROM_USER_set_info(AUD_DEC_LINEOUT_VOL_T);
            M_SET_VALIDATE(AUD_DEC_LINEOUT_VOL_T);
            _SetLineOutVolMode(u1DecId, (AUD_DEC_LINEOUT_VOL_T*)pv_set_info);
            break;
            }
#endif

        case AUD_DEC_SET_TYPE_DOWNMIX:
            M_SET_VALIDATE(AUD_DEC_DOWNMIX_MODE_T);
            _SetDownMix(u1DecId, (AUD_DEC_DOWNMIX_MODE_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_MATRIX:
            M_SET_VALIDATE(AUD_DEC_MATRIX_MODE_T);
            _SetMatrix(u1DecId, (AUD_DEC_MATRIX_MODE_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_DRC:
            M_SET_VALIDATE(AUD_DEC_DRC_T);
            _SetDrc(u1DecId, (AUD_DEC_DRC_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_MEM_BUFFER:
            {
            COPY_FROM_USER_set_info(AUD_DEC_MEM_BUFFER_INFO_T);
            M_SET_VALIDATE(AUD_DEC_MEM_BUFFER_INFO_T);
            return _SetMemBuffer(u1DecId, (AUD_DEC_MEM_BUFFER_INFO_T *) pv_set_info);
            }

        case AUD_DEC_SET_TYPE_NFY_AUD_CHG:
            M_SET_VALIDATE(BOOL);
            //TODO
            break;

        case AUD_DEC_SET_TYPE_OUT_PORT:
            M_SET_VALIDATE(UINT32);
			if(_fgOutPortMask == TRUE)
			{
				 _SetOutPort(AUD_DEC_MAIN, (UINT32)Upv_set_info);
				 _fgOutPortMask = FALSE;
			}
            _SetOutPort(u1DecId, (UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_PCR_ID:
            M_SET_VALIDATE(UINT16);
            AUD_SetStcId(u1DecId, (AV_SYNC_STC_SELECT_T)(UINT32)Upv_set_info);
            LOG(3, "[MT_AUD_MW]SetStcId Dec(%d), STC_ID(%d)\n", (AV_SYNC_STC_SELECT_T)(UINT32)Upv_set_info);
            /* Assume that if you set pcr pid, then need to do AvSync */
            AUD_SetAvSynMode(u1DecId, AV_SYNC_SLAVE);
            break;

        case AUD_DEC_SET_TYPE_CMPSS_MODE:
            M_SET_VALIDATE(AUD_DEC_CMPSS_MODE_T);
            _SetCompressionMode(u1DecId, (AUD_DEC_CMPSS_MODE_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_AGC:
            M_SET_VALIDATE(BOOL);
            _SetAgcFlag(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_CHNL_DELAY:
            {
            COPY_FROM_USER_set_info(AUD_DEC_CHNL_DELAY_T);
            M_SET_VALIDATE(AUD_DEC_CHNL_DELAY_T);
            _SetChlDelay(u1DecId, (AUD_DEC_CHNL_DELAY_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_COPY_PROTECT:
            {
            COPY_FROM_USER_set_info(AUD_DEC_COPY_PROTECT_INFO_T);
            M_SET_VALIDATE(AUD_DEC_COPY_PROTECT_INFO_T);
            // new for CI+
            _setSpdifCopyProtect ((AUD_DEC_COPY_PROTECT_INFO_T *)(UINT32)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_SPDIF_STATUS:
            {
            COPY_FROM_USER_set_info(AUD_DEC_SPDIF_STATUS_INFO_T);
            M_SET_VALIDATE(AUD_DEC_SPDIF_STATUS_INFO_T);
            LOG(3,"AUD_DEC_SET_TYPE_SPDIF_STATUS, u1DecId = %d\n", u1DecId);
            // new for CI+
            _setSpdifCopyRightCategoryCode (u1DecId, (AUD_DEC_SPDIF_STATUS_INFO_T *)(UINT32)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_MTS:
            M_SET_VALIDATE(AUD_DEC_MTS_T);
            _SetMts(u1DecId, (AUD_DEC_MTS_T)(UINT32)Upv_set_info);
            break;

         case AUD_DEC_SET_TYPE_SE_EQ_USER_LVL:
            {
            COPY_FROM_USER_set_info(AUD_DEC_SE_QA_USER_LVL_T);
            M_SET_VALIDATE(AUD_DEC_SE_QA_USER_LVL_T);
            _SetUserEqLevel(u1DecId, (AUD_DEC_SE_QA_USER_LVL_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_TV_SYS_MASK:
            M_SET_VALIDATE(UINT32);
            _SetTvSysMask(u1DecId, (UINT32)Upv_set_info);
            break;
        case AUD_DEC_SET_TYPE_TV_AUD_SYS_MASK:
            {
            COPY_FROM_USER_set_info(AUD_DEC_TV_AUD_SYS_T);
            M_SET_VALIDATE(AUD_DEC_TV_AUD_SYS_T);
            _SetTvAudSysMask(u1DecId, (AUD_DEC_TV_AUD_SYS_T *)pv_set_info);
            }
            break;
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
        case AUD_DEC_SET_TYPE_AUTO_DETECT_AUD_SYS:
            {
            COPY_FROM_USER_set_info(AUD_DEC_AUTO_DETECT_AUD_SYSTEM_INFO_T);
            M_SET_VALIDATE(AUD_DEC_AUTO_DETECT_AUD_SYSTEM_INFO_T);
            LOG(3,"_AudSet: AUD_DEC_SET_TYPE_AUTO_DETECT_AUD_SYSTEM, u1DecId = %d\n", u1DecId);
            _SetTvAutoDectectSysMask(u1DecId, (AUD_DEC_AUTO_DETECT_AUD_SYSTEM_INFO_T *)pv_set_info);
            }
            break;
#endif
        case AUD_DEC_SET_TYPE_AUD_CLIP:
            {
            COPY_FROM_USER_set_info(AUD_DEC_AUD_CLIP_T);
            M_SET_VALIDATE(AUD_DEC_AUD_CLIP_T);
            return (_SetAudClip(u1DecId,
                                (AUD_DEC_AUD_CLIP_T *)pv_set_info));
            }

        case AUD_DEC_SET_TYPE_PL2_CTRL:
            {
            COPY_FROM_USER_set_info(AUD_DEC_PL2_CTRL_INFO_T);
            M_SET_VALIDATE(AUD_DEC_PL2_CTRL_INFO_T);
            _SetPrologic2(u1DecId,
                          (AUD_DEC_PL2_CTRL_INFO_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_DOLBY_CERTI_MODE:
            M_SET_VALIDATE(BOOL);
            _SetDolbyBanner(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_OUTPORT_ADJ:
            {
            COPY_FROM_USER_set_info(AUD_DEC_OUTPORT_ADJ_T);
            M_SET_VALIDATE(AUD_DEC_OUTPORT_ADJ_T);
            _SetOutPortAdj(u1DecId, (AUD_DEC_OUTPORT_ADJ_T *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_BBE_MODE:
#ifdef CC_AUD_BBE_SUPPORT
            M_SET_VALIDATE(AUD_DEC_BBE_MODE_T);
            _SetBbeMode(u1DecId, (AUD_DEC_BBE_MODE_T)(UINT32)Upv_set_info);
#else
            LOG(1, "ERROR: BBE function is not supported\n");
#endif
            break;

        case AUD_DEC_SET_TYPE_AV_SYNC:
            M_SET_VALIDATE(AUD_DEC_AV_SYNC_INFO_T);
            _SetAvSyncMode(u1DecId, (AUD_DEC_AV_SYNC_INFO_T)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_VIDEO_SCRAMBLE_STATUS:
            M_SET_VALIDATE(BOOL);
            _SetVideoScramble(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_MUTE_EX:
            {
            COPY_FROM_USER_set_info(AUD_DEC_MUTE_EX_INFO_T);
            M_SET_VALIDATE(AUD_DEC_MUTE_EX_INFO_T);
            _AudSetMuteEx(u1DecId, (AUD_DEC_MUTE_EX_INFO_T *) pv_set_info);
            }
            break;
        case AUD_DEC_SET_TYPE_PIPVIDEO_PATH_INFO:
            M_SET_VALIDATE(BOOL);
            _SetPIPVideoPathInfo(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_VOLUME_EX:
            {
            COPY_FROM_USER_set_info(AUD_DEC_VOLUME_INFO_EX_T);
            M_SET_VALIDATE(AUD_DEC_VOLUME_INFO_EX_T);
            _SetVolumeEx(u1DecId, (AUD_DEC_VOLUME_INFO_EX_T *)pv_set_info);
            }
            break;
        case AUD_DEC_SET_TYPE_DUAL_SWAP:
            M_SET_VALIDATE(UINT32);
#ifdef CC_AUD_ARM_RENDER
#ifdef CC_DUAL_AUD_DEC_SUPPORT
            AUD_Aproc_SetFocus(u1DecId, (UINT8)(UINT32)Upv_set_info);
#endif
#endif
            break;            
        case AUD_DEC_SET_TYPE_DSP_SPEED:
            M_SET_VALIDATE(AUD_DEC_DSP_SPEED_MODE_T);
            _SetDspSpeed(u1DecId, (AUD_DEC_DSP_SPEED_MODE_T)(UINT32)Upv_set_info);
            break;
#ifdef DSP_WMA_MWIF
        case AUD_DEC_SET_TYPE_CODEC_SPEC_INFO:
            {
                COPY_FROM_USER_set_info(AUD_DEC_CODEC_SPEC_INFO_T);
                LOG(5, "[MT_AUD_MW]AUD_DEC_SET_TYPE_CODEC_SPEC_INFO: ");
                if ((((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_WMA) ||
                     (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_WMAPRO))
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "WMA\n");
                    if (!_SetDspWmaInfo(u1DecId,(AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info))
                    {
                         return (RMR_DRV_INV_SET_INFO);
                    }

                    #ifdef ENABLE_MULTIMEDIA
                    #ifdef CC_53XX_SWDMX_V2
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_WMA,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_wma_info.ui4_bits_per_sec,
                                   0
                                  );
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_WMA,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_wma_info.ui4_bits_per_sec,
                                   0
                                  );
                    #endif
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_AAC)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "AAC\n");
                    #ifdef ENABLE_MULTIMEDIA
                    #ifdef CC_53XX_SWDMX_V2
                    if(AUD_DEC_MAIN == u1DecId)
                    {
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_AAC,
                                  ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                  ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                  );
                    }
                    else if(AUD_DEC_AUX == u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO2TYPE,
                                       ENUM_SWDMX_ADEC_AAC,
                                      ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                      ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                      );

                    } else if(AUD_DEC_THIRD == u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO3TYPE,
                                       ENUM_SWDMX_ADEC_AAC,
                                      ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                      ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                      );
                    }
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_AAC,
                                  ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                  ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                  );
                    #endif
                    // doesn't support > 48KHz
                    if ( ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate > 48000)
                    {
                        LOG(0,"!! > 48KHz AAC is not Supported !!!\n");
                        return (RMR_DRV_INV_SET_INFO);
                    }
                    else if ( (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_aac_profile != 2) &&
                              (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_aac_profile != 5) &&
                              (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_aac_profile != 29) &&
                              (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_aac_profile != 0))	//default value
                    {
                        //only support AAC_LC(2), AAC_SBR(5), AAC_PS(29)
                        LOG(0,"!! AAC not support profile %d!!!\n",((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_aac_profile);
                        return (RMR_DRV_INV_SET_INFO);
                    }
                    else if ( (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.b_frame_length != 0) )
                    {
                        //only support 1024
                        LOG(0,"!! 960 AAC is not Supported !!!\n");
                        return (RMR_DRV_INV_SET_INFO);
                    }
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_PCM)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "PCM/LPCM\n");
                    #ifdef ENABLE_MULTIMEDIA
                    // Currently, only PS file needs to know audio format
                    // So naming is PS. Will extend it if necessary.
            	    DMX_PS_SetAudioType(SWDMX_AUD_PID_INDEX, DMX_AUDIO_LPCM);
            	    #ifdef CC_53XX_SWDMX_V2
                    if(AUD_DEC_MAIN == u1DecId)
                    {
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_PCM,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_num_ch
                                   );
                    }
                    else if (AUD_DEC_AUX == u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO2TYPE,
                                       ENUM_SWDMX_ADEC_PCM,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_num_ch
                                       );
                    }
                    else if (AUD_DEC_THIRD== u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO3TYPE,
                                       ENUM_SWDMX_ADEC_PCM,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_num_ch
                                       );
                    }
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_PCM,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui4_num_ch
                                   );
                    #endif
                    #endif
                    _SetDspPcmInfo(u1DecId,(AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info);

                    // for IMA4 ADPCM, current can't support.
                    // Need MW set correct info, SWDMX handle quicktime chunk padding, and DSP decoder porting.
                    if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.e_pcm_type == AUD_DEC_PCM_TYPE_IMA_ADPCM)
                    {
                        if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui2_block_align == 0)
                        {
                            return (RMR_DRV_INV_SET_INFO);  //Current MW set IMA with block =0 for IMA4.
                        }
                    }
                    #if 0
                    // doesn't support 8bit pcm.
                    if ( ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_pcm_info.ui2_bits_per_sample == 8)
                    {
                        //LOG(5,"!! 8 bit PCM is not Supported !!!\n");
                        return (RMR_DRV_INV_SET_INFO);
                    }
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_AC3)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "AC3\n");
                    #ifdef ENABLE_MULTIMEDIA
                    // Currently, only PS file needs to know audio format
                    // So naming is PS. Will extend it if necessary.
            	    DMX_PS_SetAudioType(SWDMX_AUD_PID_INDEX, DMX_AUDIO_AC3);
                    #ifdef CC_53XX_SWDMX_V2
                    if(AUD_DEC_MAIN == u1DecId)
                    {
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_AC3,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    }
                    else if (AUD_DEC_AUX)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO2TYPE,
                                       ENUM_SWDMX_ADEC_AC3,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                       );
                    }
                    else if (AUD_DEC_THIRD)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                       eSWDMX_SET_AUDIO3TYPE,
                                       ENUM_SWDMX_ADEC_AC3,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                       );
                    }
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_AC3,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    #endif
                    _SetDspAc3Info (u1DecId, (AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info);
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_MPEG)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "MPEG\n");
                    #ifdef ENABLE_MULTIMEDIA
                    // Currently, only PS file needs to know audio format
                    // So naming is PS. Will extend it if necessary.
            	    DMX_PS_SetAudioType(SWDMX_AUD_PID_INDEX, DMX_AUDIO_MPEG);
                    #ifdef CC_53XX_SWDMX_V2
                    if(AUD_DEC_MAIN == u1DecId)
                    {
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_MPEG,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    }
                    else if(AUD_DEC_AUX  == u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                      eSWDMX_SET_AUDIO2TYPE,
                                       ENUM_SWDMX_ADEC_MPEG,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                       );
                    }
                    else if(AUD_DEC_THIRD  == u1DecId)
                    {
                        AUD_MW_SWDMX_SetInfo(u1DecId,
                                      eSWDMX_SET_AUDIO3TYPE,
                                       ENUM_SWDMX_ADEC_MPEG,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                       ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                       );
                    }
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_MPEG,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    #endif
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_COOK)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "COOK\n");
                    _SetRmSpecInfo(u1DecId, (AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info);

                    // doesn't support flavor > 29
                    if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_cook_info.ui2_flavor_idx > 29)
                    {
                        //LOG(5,"!! flavor > 29 COOK is not Supported !!!\n");
                        return (RMR_DRV_INV_SET_INFO);
                    }
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_VORBIS)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "VORBIS\n");
                    #ifdef ENABLE_MULTIMEDIA
                    #ifdef CC_53XX_SWDMX_V2
                    AUD_MW_SWDMX_SetInfo(u1DecId,
                                   eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_VORBIS,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    #else
                    SWDMX_SetInfo(eSWDMX_SET_AUDIOTYPE,
                                   ENUM_SWDMX_ADEC_VORBIS,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_sample_rate,
                                   ((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_aac_info.ui4_num_ch
                                   );
                    #endif
                    #endif
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_DTS)
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "DTS\n");
                    AUD_SetDTSInfo((DATA_ENDIAN_T)(UINT32)(((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_dts_info.ui1_endian));
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_FLAC)//modified by paul
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    LOG(5, "FLAC\n");
                    _SetDspFlacInfo(u1DecId, (AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info);				// doesn't support channel num > 5
                    if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->u.t_flac_info.ui1_aud_channel > 5)
                    {
                        LOG(5, "!! > 5ch FLAC is not Supported !!!\n");
                        return (RMR_DRV_INV_SET_INFO);
                    }
                }
                else if (((AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info)->e_fmt == AUD_DEC_FMT_APE)//zhengwen for APE
                {
                    M_SET_VALIDATE(AUD_DEC_CODEC_SPEC_INFO_T);
                    if (!_SetDspApeInfo(u1DecId, (AUD_DEC_CODEC_SPEC_INFO_T*)pv_set_info))
                    {
                        LOG(0, "APE not support\n");
                         return (RMR_DRV_INV_SET_INFO);
                    }
                }
                else
                {
                    LOG(0, "Invalid: Not support spec info\n");
                }
            }
       	    break;

#endif
        case AUD_DEC_SET_TYPE_HEADPHONE_DETECT_NFY:
            {
            COPY_FROM_USER_set_info(AUD_DEC_HDPH_DETECT_NFT_INFO_T);
            M_SET_VALIDATE(AUD_DEC_HDPH_DETECT_NFT_INFO_T);
            _AudSetHPNotifyInfo(u1DecId, (AUD_DEC_HDPH_DETECT_NFT_INFO_T *) pv_set_info);
            }
            break;
//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
        case AUD_DEC_SET_TYPE_UPLOAD_DATA_NFY:
            {
            COPY_FROM_USER_set_info(AUD_DEC_UPLOAD_DATA_NFT_INFO_T);
            M_SET_VALIDATE(AUD_DEC_UPLOAD_DATA_NFT_INFO_T);
            _AudSetYWNotifyInfo(u1DecId, (AUD_DEC_UPLOAD_DATA_NFT_INFO_T *) pv_set_info);
            }
            break;
//#endif
        case AUD_DEC_SET_TYPE_CLIP_CUSTOM:
            {
            COPY_FROM_USER_set_info(AUD_DEC_AUD_CLIP_CUSTIOM_T);
            M_SET_VALIDATE(AUD_DEC_AUD_CLIP_CUSTIOM_T);
            LOG(3,"_AudSet: AUD_DEC_SET_TYPE_CLIP_CUSTOM, u1DecId = %d\n", u1DecId);
            _SetCustomerMusicFlag(u1DecId, (AUD_DEC_AUD_CLIP_CUSTIOM_T *) pv_set_info);
            }
            break;
        case AUD_DEC_SET_TYPE_AUDYSSEY_EFFECT:
            {
#ifdef CC_AUD_DSP_SUPPORT_AUDYSSEY
            COPY_FROM_USER_set_info(AUD_DEC_AUDYSSEY_INFO_T);
            M_SET_VALIDATE(AUD_DEC_AUDYSSEY_INFO_T);
            _AudSetAudyssey((AUD_DEC_AUDYSSEY_INFO_T *)pv_set_info);
#else
            LOG(1, "ERROR: Audyssey function is not supported\n");
#endif
            }
            break;
        case AUD_DEC_SET_TYPE_BTN_SOUND_CTRL:
            {
#ifdef CC_AUD_MIXSOUND_SUPPORT
            COPY_FROM_USER_set_info(AUD_DEC_BTNSND_INFO_T);
            M_SET_VALIDATE(AUD_DEC_BTNSND_INFO_T);
            _AudSetMixSnd((AUD_DEC_BTNSND_INFO_T *) pv_set_info);
#else
            LOG(1, "ERROR: Mix sound function is not supported\n");
#endif
            }
            break;
        case AUD_DEC_SET_TYPE_MM_MODE:
#ifdef CC_AUD_KARAOKE_MODE_SUPPORT
            M_SET_VALIDATE(AUD_DEC_MM_MODE_T);
            _SetMultimediaMode(u1DecId, (AUD_DEC_MM_MODE_T)(UINT32)Upv_set_info);
#else
            LOG(1, "ERROR: Karaoke function is not supported\n");
#endif
//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
        case AUD_DEC_SET_TYPE_UPLOAD_MODE:
            M_SET_VALIDATE(AUD_DEC_UPLOAD_MODE_T);
            _SetAudUploadMode((AUD_DEC_UPLOAD_MODE_T)(UINT32)Upv_set_info);
            break;
//#endif
        case AUD_DEC_SET_TYPE_OVM:
            {
            M_SET_VALIDATE(UINT32);
#ifdef CC_AUD_WAFFLE_OVM_SUPPORT
            _arAudDecOvmMode = (AUD_DEC_OVM_MODE_T)(UINT32)Upv_set_info;
            LOG (1, "====>PAL NEW OVM: mode = %d\n", _arAudDecOvmMode);
            AUD_DspSetHdevMode(AUD_ATV_DECODER_PAL, (UINT8)(UINT32)Upv_set_info);
#ifdef CC_AUD_USE_FLASH_AQ
#ifdef CC_AUD_INDIA_GAIN_SUPPORT
            Aud_DspSetIndiaSpeakerGain(_arAudDecOvmMode);
#endif
#endif
#else
            _arAudDecOvmMode = (AUD_DEC_OVM_MODE_T)(UINT32)Upv_set_info;
            LOG (1, "====> NEW OVM: mode = %d\n", _arAudDecOvmMode);
            AUD_DspSetHdevMode(AUD_ATV_DECODER_MTS, (BOOL)(UINT32)Upv_set_info);
#endif
            }
            break;
        case AUD_DEC_SET_TYPE_CMPSS_DRC:
            {
            M_SET_VALIDATE(AUD_DEC_CMPSS_DRC_MODE_T);
            LOG(3, "_AudSet: AUD_DEC_SET_TYPE_CMPSS_DRC, u1DecId = %d\n", u1DecId);
            _SetDolbyCmpssDrc(u1DecId, (AUD_DEC_CMPSS_DRC_MODE_T)(UINT32)Upv_set_info);
            }
            break;
#ifdef CC_MPEG_LEVEL_ADJ
        case AUD_DEC_SET_TYPE_MPEG_LEVEL_ADJ:
            {
            M_SET_VALIDATE(UINT8);
            LOG(3, "_AudSet: AUD_DEC_SET_TYPE_CC_MPEG_LEVEL_ADJ, u1DecId = %d\n", u1DecId);
            _SetMpegLevelAdjust(u1DecId, (UINT8)(UINT32)Upv_set_info);
            }
            break;
#endif
#if defined(CC_AUD_SX1_FEATURE) || defined (CC_MAPLE_CUST_DRV)
        case AUD_DEC_SET_TYPE_OUT_PORT_LINK:
            M_SET_VALIDATE(BOOL);
            _SetOutPortLink((BOOL)(UINT32)Upv_set_info);
            break;
#endif
        case AUD_DEC_SET_TYPE_DC_POWER_OFF:
            {
            M_SET_VALIDATE(BOOL);
            AUD_DspDecPowerOffMute(AUD_DEC_MAIN, (BOOL)(UINT32)Upv_set_info);
            x_thread_delay(30);    //wait fade out done
#ifdef  CC_AUD_SOEM_POWER_SEQUENCE
#ifdef CC_MT5565
            // disable SDATA0/1 pin
            BSP_PinSet(PIN_AOSDATA0, PINMUX_FUNCTION0);
            BSP_PinSet(PIN_AOSDATA1, PINMUX_FUNCTION0);
#endif
            AUD_PowerOffGpioMute((BOOL)(UINT32)Upv_set_info);
#ifdef CC_MT5881
            //Set MCLK/BCK/LRCK/SDATA pinmux to GPIO mode
            x_thread_delay(10);    //additional delay for AMP RST pin drop faster than MCLK/BCK/LRCK for Sony request
            BSP_PinSet(PIN_AOMCLK, PINMUX_FUNCTION0);
            BSP_PinSet(PIN_AOLRCK, PINMUX_FUNCTION0);
            BSP_PinSet(PIN_AOBCK, PINMUX_FUNCTION0);
#endif
#endif
            }
            break;
        case AUD_DEC_SET_TYPE_VDO_PICTURE_OFF:
            {
            M_SET_VALIDATE(BOOL);
            AUD_SetVdoPictureOffStatus((BOOL)(UINT32)Upv_set_info);
            }
            break;
#ifdef CC_AUD_SUPPORT_MS10
        case AUD_DEC_SET_TYPE_DDC_MODE:
            {
            COPY_FROM_USER_set_info(AUD_DEC_DDC_INFO_T);
            M_SET_VALIDATE(AUD_DEC_DDC_INFO_T);
            LOG(3,"_AudSet: AUD_DEC_SET_TYPE_DDC_MODE, u1DecId = %d\n", u1DecId);
            _SetAudDDC(u1DecId, (AUD_DEC_DDC_INFO_T *) pv_set_info);
            }
            break;
#endif
#ifdef CC_AUD_SKYPE_SUPPORT
        case AUD_DEC_SET_TYPE_SKYPE_MIX_CTRL:
            {
            COPY_FROM_USER_set_info(AUD_DEC_SKYPE_INFO_T);
            M_SET_VALIDATE(AUD_DEC_SKYPE_INFO_T);
            _AudSetSkypeMix((AUD_DEC_SKYPE_INFO_T *) pv_set_info);
            }
            break;
#endif
//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
        case  AUD_DEC_SET_TYPE_UPLOAD_CTRL:
			{
            M_SET_VALIDATE(AUD_DEC_UPLOAD_MODE_T);
            _AudSetYWUploadEnable((AUD_DEC_UPLOAD_MODE_T)(UINT32)Upv_set_info);
        	}
            break;

		case AUD_ENC_SET_TYPE_CTRL:
			{
			M_SET_VALIDATE(BOOL);
			_AudSetENCEnable((BOOL)(UINT32)Upv_set_info);
			}
			break;

		case AUD_ENC_SET_TYPE_SETTINGS:
			{
			COPY_FROM_USER_set_info(AUD_ENC_SETTINGS_T);
			M_SET_VALIDATE(AUD_ENC_SETTINGS_T);
			_AudSetENCType((AUD_ENC_SETTINGS_T *) pv_set_info);
			}
			break;


//#endif
#ifdef CC_AUD_HDMI_SPDIF_CFG
        case AUD_DEC_SET_TYPE_PROTECT_INFO:     ///[Joel] 20101011
            AUD_HdmiCfgAnalogOutput((AUD_OUTPUT_STATE)(UINT32)Upv_set_info);
            break;
#endif

#ifdef KARAOKE_MIC_LINE_IN2
        case AUD_DEC_SET_TYPE_KARAOKE:
            M_SET_VALIDATE(BOOL);
            AUD_MicInput_Enable((BOOL)(UINT32)Upv_set_info);  //wait for integration test.
            break;
#endif
        case AUD_DEC_SET_TYPE_PANFADE_ENABLE:
            {
            COPY_FROM_USER_set_info(UINT32);
            _AudSetPanFadeEnable((UINT32 *)pv_set_info);
            }
            break;

        case AUD_DEC_SET_TYPE_AUDIO_ONLY_INFO:
            M_SET_VALIDATE(BOOL);
            LOG(9,"=========================AUD_DEC_SET_TYPE_AUDIO_ONLY_INF(Dec%d) = %d\n",u1DecId, (BOOL)(UINT32)Upv_set_info);
            _AudSetMMAudioOnly(u1DecId, (BOOL)(UINT32)Upv_set_info);
            break;
        case AUD_DEC_SET_TYPE_AD_MODE:
            M_SET_VALIDATE(BOOL);
    	    LOG(7,"=========================AUD_DEC_SET_TYPE_AD_MODE = %d\n",(BOOL)(UINT32)Upv_set_info);
            AUD_SetAdMode((BOOL)(UINT32)Upv_set_info);
            break;
        case AUD_DEC_SET_TYPE_ENDIAN_MODE:
            LOG(5, "[MT_AUD_MW]DTS Endian(%d)\n", (DATA_ENDIAN_T)(UINT32) Upv_set_info);
            AUD_SetDTSInfo((DATA_ENDIAN_T)(UINT32) Upv_set_info);
            break;

        case AUD_DEC_SET_TYPE_SRS_BYPASS:
#ifdef AUD_SUPPORT_SRS_BYPASS
			_AudSetSRSBypass((AUD_DEC_SRS_BYPASS_MODE_T)(UINT32)Upv_set_info);
#else
            AUD_DspSoundEffectFlag(u1DecId, !(BOOL)(UINT32)Upv_set_info);
#endif // AUD_SUPPORT_SRS_BYPASS
            break;

#ifdef CC_AUD_SUPPORT_OSS_ADAPTION_LAYER
        case AUD_DEC_SET_TYPE_OUT_DEVICE:
            AUD_SetOutDeviceType((AUD_DEC_OUT_DEVICE_T)(UINT32)(Upv_set_info));
            break;
#endif
#ifdef CC_SUPPORT_2STC //<--- Enable this when other module submit code.
        case AUD_DEC_SET_TYPE_STC_ID:
            AUD_DspSetStcId(u1DecId, (UINT16)(UINT32)(Upv_set_info));  // 2_STC_Task
            break;
#endif
#ifdef CC_AUD_LINEIN_USE_DEC3
        case AUD_DEC_SET_TYPE_DECODE_MODE:
            {
                LOG(3,"_AudSet: AUD_DEC_SET_TYPE_DECODE_MODE, mode = %d\n", (UINT32)Upv_set_info );
                if ((UINT32)Upv_set_info == AUD_DEC_DECODE_MODE_NORMAL)
                {
                    AUD_DspSetTriOnlyDecMode(FALSE);
                }
                else if ((UINT32)Upv_set_info == AUD_DEC_DECODE_MODE_TRIONLY)
                {
                    AUD_DspSetTriOnlyDecMode(TRUE);
                }
            }
            break;
#endif
        case AUD_DEC_SET_TYPE_SPDIF_LEVEL:
            M_SET_VALIDATE(UINT8);
            _u1SpdifLevel = (UINT8)(UINT32)Upv_set_info;
        #ifdef CC_AUD_SPIDF_LVL
            AUD_DspSpdifLevel(_u1SpdifLevel);
        #endif
            break;
		case AUD_DEC_SET_TYPE_SOURCE_MODE:
			if ((UINT32)Upv_set_info == AUD_DEC_SOURCE_DTV)
			{
               _aeStrSrc[u1DecId]  = AUD_STREAM_FROM_DIGITAL_TUNER;
			   _fgMMPlusAD[AUD_DEC_MAIN] = TRUE;
			   _fgMMPlusAD[AUD_DEC_AUX] = TRUE;
			   _fgMMPlusAD[AUD_DEC_THIRD] = TRUE;
			}
            break;
        default:
            return (RMR_DRV_INV_SET_INFO);
    }

    return RMR_OK;
}

INT32 _MTAUD_d_custom_aud_set (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    VOID*                     Upv_set_type,
    SIZE_T                    z_size)
{
#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    // Check input parameters
    switch (e_aud_type)
    {
    case DRV_CUSTOM_AUD_TYPE_VOL_GAIN:
        if (z_size != sizeof(AUD_TYPE_VOL_GAIN_INFO_T))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_VOL_GAIN_OFFSET:
        if (z_size != sizeof(AUD_TYPE_VOL_GAIN_INFO_T))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_HEADPHONE_VOL:
        if (z_size != sizeof(INT16))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_AVL_GAIN:
        if (z_size != sizeof(AUD_TYPE_AVL_GAIN_INFO_T))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_BASS_BOOSTER:
#ifdef CC_AUD_VBASS_SUPPORT
        if (z_size != sizeof(AUD_TYPE_BASS_BOOSTER))
        {
            return RMR_DRV_INV_SET_INFO;
        }
#endif
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_CHECK:
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_PILOT:
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_HIGH:
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_LOW:
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_SAP:
    case DRV_CUSTOM_AUD_TYPE_MTS_SAP_HIGH:
    case DRV_CUSTOM_AUD_TYPE_MTS_SAP_LOW:
    case DRV_CUSTOM_AUD_TYPE_A2_NUM_CHECK:
    case DRV_CUSTOM_AUD_TYPE_A2_NUM_DOUBLE:
    case DRV_CUSTOM_AUD_TYPE_A2_MONO_WEIGHT:
    case DRV_CUSTOM_AUD_TYPE_A2_STEREO_WEIGHT:
    case DRV_CUSTOM_AUD_TYPE_A2_DUAL_WEIGHT:
    case DRV_CUSTOM_AUD_TYPE_PAL_CORRECT:
    case DRV_CUSTOM_AUD_TYPE_PAL_SYNC_LOOP:
    case DRV_CUSTOM_AUD_TYPE_PAL_ERROR:
    case DRV_CUSTOM_AUD_TYPE_PAL_PARITY_ERROR:
    case DRV_CUSTOM_AUD_TYPE_PAL_NUM_FRAMES:
        if (z_size != sizeof(UINT16))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_HDEV:
    case DRV_CUSTOM_AUD_TYPE_A2_HDEV:
    case DRV_CUSTOM_AUD_TYPE_PAL_HDEV:
    case DRV_CUSTOM_AUD_TYPE_MTS_CARRIER_SHIFT:
    case DRV_CUSTOM_AUD_TYPE_A2_CARRIER_SHIFT:
    case DRV_CUSTOM_AUD_TYPE_PAL_CARRIER_SHIFT:
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU:
    case DRV_CUSTOM_AUD_TYPE_FM_SATU_MUTE:
    case DRV_CUSTOM_AUD_TYPE_NON_EU:
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_OFFSET:
        if (z_size != sizeof(BOOL))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE:
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_HIGH:
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_LOW:
    case DRV_CUSTOM_AUD_TYPE_MTS_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_SAP_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_HIGH:
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_LOW:
    case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_LOW:
    case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_HIGH:
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE:
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_HIGH:
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_LOW:
    case DRV_CUSTOM_AUD_TYPE_A2_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE:
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_HIGH:
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_LOW:
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE:
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_HIGH:
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_LOW:
    case DRV_CUSTOM_AUD_TYPE_PAL_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_AM_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_NICAM_FINE_VOLUME:
    case DRV_CUSTOM_AUD_TYPE_BBE_PRESCALE_LEVEL:
    case DRV_CUSTOM_AUD_TYPE_BBE_PROCESSS:
    case DRV_CUSTOM_AUD_TYPE_BBE_LO_CONTOUR:
    case DRV_CUSTOM_AUD_TYPE_BBE_3D_GAIN:
        if (z_size != sizeof(UINT8))
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    default:
        break;
    }

    // Implementation
    switch (e_aud_type)
    {
    case DRV_CUSTOM_AUD_TYPE_VOL_GAIN:
        {
        COPY_FROM_USER_set_type(AUD_TYPE_VOL_GAIN_INFO_T);

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_ATV) > 0)
        {
            AUD_DspSetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DTV) > 0)
        {
            AUD_DspSetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_DIGITAL_TUNER, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if (((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_YPBPR) > 0) ||
            ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_VGA) > 0) ||
            ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVI) > 0))
        {
            //AUD_INPUT_ID_T eDvdId;
            //INT32 i;
            //eDvdId =  AUD_DspGetAvInputSrcDvdId();
            //for (i=0 ; i<AUD_INPUT_ID_MAX; i++)
            //{
            //    if (i == (UINT32)eDvdId)
            //    {
            //        ;
            //    }
            //    else
            //    {
            //        AUD_DspSetAvSrcVol((AUD_INPUT_ID_T)i, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
            //    }
            //}
            AUD_DspSetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_LINE_IN, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVD) > 0)
        {
            AUD_INPUT_ID_T eDvdId;
            eDvdId =  AUD_DspGetAvInputSrcDvdId();
            AUD_DspSetAvSrcVol(eDvdId, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_HDMI) > 0)
        {
            AUD_DspSetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_HDMI, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

	    if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_MM) > 0)
        {
            AUD_DspSetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_MULTI_MEDIA, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_VOL_GAIN_OFFSET:
        {
#ifdef CC_AUD_INPUT_SRC_VOL_OFFSET
        COPY_FROM_USER_set_type(AUD_TYPE_VOL_GAIN_INFO_T);

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_ATV) > 0)
        {
            AUD_DspSetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DTV) > 0)
        {
            AUD_DspSetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_DIGITAL_TUNER, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if (((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_YPBPR) > 0) ||
            ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_VGA) > 0) ||
            ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVI) > 0))
        {
            //AUD_INPUT_ID_T eDvdId;
            //INT32 i;
            //eDvdId =  AUD_DspGetAvInputSrcDvdId();
            //for (i=0 ; i<AUD_INPUT_ID_MAX; i++)
            //{
            //    if (i == (UINT32)eDvdId)
            //    {
            //        ;
            //    }
            //    else
            //    {
            //        AUD_DspSetAvSrcVolOffset((AUD_INPUT_ID_T)i, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
            //    }
            //}
            AUD_DspSetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_LINE_IN, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVD) > 0)
        {
            AUD_INPUT_ID_T eDvdId;
            eDvdId =  AUD_DspGetAvInputSrcDvdId();
         //   AUD_DspSetAvSrcVolOffset(eDvdId, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

        if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_HDMI) > 0)
        {
            AUD_DspSetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_HDMI, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }

	    if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_MM) > 0)
        {
            AUD_DspSetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_MULTI_MEDIA, ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_set_type)->i2_vol_gain);
        }
#endif
        }
        break;
    case DRV_CUSTOM_AUD_TYPE_HEADPHONE_VOL:
        ADAC_HeadphoneVolCtl((INT16)((UINT32)Upv_set_type));
        _i2HpVol = (INT16)((UINT32)Upv_set_type);
        break;
    case DRV_CUSTOM_AUD_TYPE_AVL_GAIN:
        break;
    /* MTS factory mode */
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_CHECK:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_NUM_CHECK,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_PILOT:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_NUM_PILOT,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_HIGH:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_PILOT_HIGH,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_LOW:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_PILOT_LOW,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_NUM_SAP:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_NUM_SAP,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_SAP_HIGH:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_SAP_HIGH,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_SAP_LOW:
        AUD_DspSetMtsDetection (AUD_FACTORY_MTS_SAP_LOW,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_HDEV:
#ifndef CC_AUD_WAFFLE_OVM_SUPPORT
        AUD_DspSetHdevMode(AUD_ATV_DECODER_MTS, (BOOL)((UINT32)Upv_set_type));
#else
        AUD_DspSetHdevMode(AUD_ATV_DECODER_MTS, (UINT8)((UINT32)Upv_set_type));
#endif
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_CARRIER_SHIFT:
        AUD_DspSetCarrierShiftMode(AUD_ATV_DECODER_MTS, (BOOL)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU:
        AUD_DspSetFMSatuMuteMode(AUD_ATV_DECODER_MTS, (BOOL)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_HIGH:
        AUD_DspSetFMSatuMuteTh(AUD_FACTORY_SATU_MUTE_HIGHER,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_LOW:
        AUD_DspSetFMSatuMuteTh(AUD_FACTORY_SATU_MUTE_LOWER,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE:
        AUD_DspSetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_HIGH:
        AUD_DspSetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE_HIGH,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_LOW:
        AUD_DspSetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE_LOW,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_FINE_VOLUME:
        AUD_DspSetMtsMonoFineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_SAP_FINE_VOLUME:
        AUD_DspSetSAPFineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_OFFSET:
        AUD_DspSetMtsPilotDetection((BOOL)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_LOW:
        AUD_DspSetSapMute(AUD_FACTORY_SAP_MUTE_LOWER,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_HIGH:
        AUD_DspSetSapMute(AUD_FACTORY_SAP_MUTE_HIGHER,(UINT8)((UINT32)Upv_set_type));
        break;
    /* A2 factory mode */
    case DRV_CUSTOM_AUD_TYPE_A2_NUM_CHECK:
        AUD_DspSetA2Detection (AUD_FACTORY_A2_NUM_CHECK,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_NUM_DOUBLE:
        AUD_DspSetA2Detection (AUD_FACTORY_A2_NUM_DOUBLE,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_MONO_WEIGHT:
        AUD_DspSetA2Detection (AUD_FACTORY_A2_MONO_WEIGHT,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_STEREO_WEIGHT:
        AUD_DspSetA2Detection (AUD_FACTORY_A2_STEREO_WEIGHT,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_DUAL_WEIGHT:
        AUD_DspSetA2Detection (AUD_FACTORY_A2_DUAL_WEIGHT,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_HDEV:
#ifndef CC_AUD_WAFFLE_OVM_SUPPORT
        AUD_DspSetHdevMode(AUD_ATV_DECODER_A2, (BOOL)((UINT32)Upv_set_type));
#else
        AUD_DspSetHdevMode(AUD_ATV_DECODER_A2, (UINT8)((UINT32)Upv_set_type));
#endif
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_CARRIER_SHIFT:
        AUD_DspSetCarrierShiftMode(AUD_ATV_DECODER_A2, (BOOL)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE:
        AUD_DspSetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_HIGH:
        AUD_DspSetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE_HIGH,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_LOW:
        AUD_DspSetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE_LOW,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_A2_FINE_VOLUME:
        AUD_DspSetA2FineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    /* PAL factory mode */
    case DRV_CUSTOM_AUD_TYPE_PAL_CORRECT:
        AUD_DspSetPalDetection (AUD_FACTORY_PAL_CORRECT,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_SYNC_LOOP:
        AUD_DspSetPalDetection (AUD_FACTORY_PAL_SYNC_LOOP,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_ERROR:
        AUD_DspSetPalDetection (AUD_FACTORY_PAL_ERROR,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_PARITY_ERROR:
        AUD_DspSetPalDetection (AUD_FACTORY_PAL_PARITY_ERROR,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_NUM_FRAMES:
        AUD_DspSetPalDetection (AUD_FACTORY_PAL_NUM_FRAMES,(UINT16)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_HDEV:
#ifndef CC_AUD_WAFFLE_OVM_SUPPORT
        AUD_DspSetHdevMode(AUD_ATV_DECODER_PAL, (BOOL)((UINT32)Upv_set_type));
#else
        AUD_DspSetHdevMode(AUD_ATV_DECODER_PAL, (UINT8)((UINT32)Upv_set_type));
#endif
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE:
        AUD_DspSetAmMute(AUD_FACTORY_PAL_AM_MUTE,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_HIGH:
        AUD_DspSetAmMute(AUD_FACTORY_PAL_AM_MUTE_HIGH,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_LOW:
        AUD_DspSetAmMute(AUD_FACTORY_PAL_AM_MUTE_LOW,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_CARRIER_SHIFT:
        AUD_DspSetCarrierShiftMode(AUD_ATV_DECODER_PAL, (BOOL)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE:
        AUD_DspSetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_HIGH:
        AUD_DspSetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE_HIGH,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_LOW:
        AUD_DspSetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE_LOW,(UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_PAL_FINE_VOLUME:
        AUD_DspSetPALFineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_AM_FINE_VOLUME:
        AUD_DspSetAMFineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_NICAM_FINE_VOLUME:
        AUD_DspSetNicamFineVolume((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_FM_SATU_MUTE:
        AUD_DspSetFMSatuMuteMode(AUD_ATV_DECODER_PAL, (BOOL)((UINT32)Upv_set_type));
        break;
    /* non-EU factory mode */
    case DRV_CUSTOM_AUD_TYPE_NON_EU:
        AUD_DspSetNonEUMode((BOOL)((UINT32)Upv_set_type));
        break;
#ifdef CC_AUD_BBE_SUPPORT
    /* BBE factory mode */
    case DRV_CUSTOM_AUD_TYPE_BBE_PRESCALE_LEVEL:
        AUD_DspSetBbeLevel((INT8)((INT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_BBE_PROCESSS:
        AUD_DspSetBbeProcess((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_BBE_LO_CONTOUR:
        AUD_DspSetBbeLoContour((UINT8)((UINT32)Upv_set_type));
        break;
    case DRV_CUSTOM_AUD_TYPE_BBE_3D_GAIN:
        AUD_DspSetBbe3dGain((UINT8)((UINT32)Upv_set_type));
        break;
#endif
    /* Test Tone */
    case DRV_CUSTOM_AUD_TYPE_TEST_TONE:
    {
        AUD_CLIP_TYPE_T rAudClip;
        UINT8 u1Retry = 0;

        COPY_FROM_USER_set_type(AUD_TYPE_TEST_TONE);

        if ( ((AUD_TYPE_TEST_TONE *)pv_set_type)->fgPlayTestTone )
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            while ( AUD_IsDecoderPlay(AUD_DSP0, ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId) )
#else
            while ( AUD_IsDecoderPlay(((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId) )
#endif
            {
                x_thread_delay(10); //FIXME !! wait for audio driver thread into stop state.
                if (u1Retry > 10)
                {
                    return RMR_DRV_INV_SET_INFO;
                }

                u1Retry++;
            }

            {
                x_memset((VOID *)VIRTUAL((UINT32)&rAudClip), 0, sizeof(AUD_CLIP_TYPE_T));
                rAudClip.u4OutputChlSelect = ((AUD_TYPE_TEST_TONE *)pv_set_type)->u4OutputChlSelect;
                rAudClip.u4RepeatCount = 0;
                rAudClip.fgPlayTestTone = TRUE;
                rAudClip.u4ToneFreq = ((AUD_TYPE_TEST_TONE *)pv_set_type)->u4ToneFreq ;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_SetAudClip(AUD_DSP0, ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId, &rAudClip);
#else
                AUD_SetAudClip(((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId, &rAudClip);
#endif
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_PlayAudClip( AUD_DSP0, ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId,
#else
                AUD_PlayAudClip( ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId,
#endif
                    ((AUD_TYPE_TEST_TONE *)pv_set_type)->u4OutputChlSelect,
                    ((AUD_TYPE_TEST_TONE *)pv_set_type)->u4ToneFreq );
            }
        }
        else
        {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_StopAudClip(AUD_DSP0,  ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId );
#else
            AUD_StopAudClip( ((AUD_TYPE_TEST_TONE *)pv_set_type)->u1DecId );
#endif
        }
        }
        break;

    /* Cutoff Frequency */
    case DRV_CUSTOM_AUD_TYPE_CUT_OFF_FREQ:
        AUD_DspSpkSizeCfg(AUD_DEC_MAIN, (UINT32)Upv_set_type);
        break;
    case DRV_CUSTOM_AUD_VOL_CURVE:
        {
        COPY_FROM_USER_set_type(DRV_CUSTOM_AUD_VOL_CURVE_T);
        AUD_DspSetVolumeTable(((DRV_CUSTOM_AUD_VOL_CURVE_T*)pv_set_type)->ui4_vol_values );
		}
		break;
    case DRV_CUSTOM_AUD_TYPE_DTS_ENDIAN:
        AUD_SetDTSInfo((DATA_ENDIAN_T)Upv_set_type);
        break;
    case DRV_CUSTOM_AUD_TYPE_BASS_BOOSTER:
		{
#ifdef CC_AUD_VBASS_SUPPORT
        COPY_FROM_USER_set_type(AUD_TYPE_BASS_BOOSTER);
        AUD_DspSetVirBassEnable(((AUD_TYPE_BASS_BOOSTER*)pv_set_type)->fgEnable);
#endif
    	}
		break;

    default:
        break;
    }

    return RMR_OK;
}

INT32 _MTAUD_d_custom_aud_get (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    VOID*                     Upv_get_type,
    SIZE_T*                   Upz_size
)
{
#ifdef CC_AUD_USE_FLASH_AQ
    CHAR cAQVer[FLASH_AQ_VER_LENGTH] = {0};
    UINT8 u1Len;
#endif


    COPY_FROM_USER_z_size(SIZE_T);

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    // Check pz_size
    switch (e_aud_type)
    {
        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN:
            if (*pz_size < sizeof(AUD_TYPE_VOL_GAIN_INFO_T))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(AUD_TYPE_VOL_GAIN_INFO_T);
            break;
        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN_OFFSET:
            if (*pz_size < sizeof(AUD_TYPE_VOL_GAIN_INFO_T))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(AUD_TYPE_VOL_GAIN_INFO_T);
            break;
        case DRV_CUSTOM_AUD_TYPE_HEADPHONE_VOL:
            if (*pz_size < sizeof(INT16))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(INT16);
            break;
        case DRV_CUSTOM_AUD_TYPE_SPECTRUM_INFO: //Andrew Wen 07/1/25 EQ spectrum
            if (*pz_size < sizeof(AUD_TYPE_SPECTRUM_INFO_T))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(AUD_TYPE_SPECTRUM_INFO_T);
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_CHECK:
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_PILOT:
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_HIGH:
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_LOW:
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_SAP:
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_HIGH:
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_LOW:
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_CHECK:
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_DOUBLE:
        case DRV_CUSTOM_AUD_TYPE_A2_MONO_WEIGHT:
        case DRV_CUSTOM_AUD_TYPE_A2_STEREO_WEIGHT:
        case DRV_CUSTOM_AUD_TYPE_A2_DUAL_WEIGHT:
        case DRV_CUSTOM_AUD_TYPE_PAL_CORRECT:
        case DRV_CUSTOM_AUD_TYPE_PAL_SYNC_LOOP:
        case DRV_CUSTOM_AUD_TYPE_PAL_ERROR:
        case DRV_CUSTOM_AUD_TYPE_PAL_PARITY_ERROR:
        case DRV_CUSTOM_AUD_TYPE_PAL_NUM_FRAMES:
            if (*pz_size < sizeof(UINT16))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(UINT16);
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_HDEV:
        case DRV_CUSTOM_AUD_TYPE_A2_HDEV:
        case DRV_CUSTOM_AUD_TYPE_PAL_HDEV:
        case DRV_CUSTOM_AUD_TYPE_MTS_CARRIER_SHIFT:
        case DRV_CUSTOM_AUD_TYPE_A2_CARRIER_SHIFT:
        case DRV_CUSTOM_AUD_TYPE_PAL_CARRIER_SHIFT:
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU:
        case DRV_CUSTOM_AUD_TYPE_FM_SATU_MUTE:
        case DRV_CUSTOM_AUD_TYPE_NON_EU:
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_OFFSET:
            if (*pz_size < sizeof(BOOL))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(BOOL);
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE:
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_HIGH:
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_LOW:
        case DRV_CUSTOM_AUD_TYPE_MTS_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_SAP_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_HIGH:
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_LOW:
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_LOW:
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_HIGH:
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE:
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_HIGH:
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_LOW:
        case DRV_CUSTOM_AUD_TYPE_A2_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE:
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_HIGH:
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_LOW:
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE:
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_HIGH:
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_LOW:
        case DRV_CUSTOM_AUD_TYPE_PAL_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_AM_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_NICAM_FINE_VOLUME:
        case DRV_CUSTOM_AUD_TYPE_BBE_PRESCALE_LEVEL:
        case DRV_CUSTOM_AUD_TYPE_BBE_PROCESSS:
        case DRV_CUSTOM_AUD_TYPE_BBE_LO_CONTOUR:
        case DRV_CUSTOM_AUD_TYPE_BBE_3D_GAIN:
            if (*pz_size < sizeof(UINT8))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(UINT8);
            break;
        case DRV_CUSTOM_AUD_TYPE_SMART_AUDIO:
            if (*pz_size < sizeof(SMART_AUD_GROUP_INFO_T))
            {
                return RMR_DRV_INV_GET_INFO;
            }
            *pz_size = sizeof(SMART_AUD_GROUP_INFO_T);
            break;
        default:
            break;
    }

    COPY_TO_USER_z_size(SIZE_T);

    // Implementation
    switch (e_aud_type)
    {
        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN:
            {
                COPY_FROM_USER_get_type(AUD_TYPE_VOL_GAIN_INFO_T);

                if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_ATV) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DTV) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_DIGITAL_TUNER);
                }
                else if (((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_YPBPR) > 0) ||
                    ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_VGA) > 0) ||
                    ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVI) > 0))
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_LINE_IN);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_HDMI) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_HDMI);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_MM) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVol(AUD_DEC_MAIN, AUD_STREAM_FROM_MULTI_MEDIA);
                }

                COPY_TO_USER_get_type(AUD_TYPE_VOL_GAIN_INFO_T);
            }
            break;

        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN_OFFSET:
            {
#ifdef CC_AUD_INPUT_SRC_VOL_OFFSET
                COPY_FROM_USER_get_type(AUD_TYPE_VOL_GAIN_INFO_T);

                if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_ATV) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DTV) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_DIGITAL_TUNER);
                }
                else if (((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_YPBPR) > 0) ||
                    ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_VGA) > 0) ||
                    ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_DVI) > 0))
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_LINE_IN);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_HDMI) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_HDMI);
                }
                else if ((((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->ui4_input_mask & DRV_CUSTOM_AUD_INP_FLAG_MM) > 0)
                {
                    ((AUD_TYPE_VOL_GAIN_INFO_T *)pv_get_type)->i2_vol_gain = AUD_DspGetSrcVolOffset(AUD_DEC_MAIN, AUD_STREAM_FROM_MULTI_MEDIA);
                }

                COPY_TO_USER_get_type(AUD_TYPE_VOL_GAIN_INFO_T);
#endif
            }
            break;

        case DRV_CUSTOM_AUD_TYPE_HEADPHONE_VOL:
            {
            COPY_FROM_USER_get_type(INT16);
            *(INT16 *)pv_get_type = _i2HpVol;
            COPY_TO_USER_get_type(INT16);
            }
            break;

        case DRV_CUSTOM_AUD_TYPE_SPECTRUM_INFO:
            {
            INT32 i;
            COPY_FROM_USER_get_type(AUD_TYPE_SPECTRUM_INFO_T);
            for (i=0 ; i<15; i++)
            {
                ((AUD_TYPE_SPECTRUM_INFO_T *)pv_get_type)->ai4_bar[i] = AUD_DspGetSpectrumInfo(AUD_DEC_MAIN,i+1);
            }
            COPY_TO_USER_get_type(AUD_TYPE_SPECTRUM_INFO_T);
            }
            break;

        /* MTS factory mode */
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_CHECK:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_NUM_CHECK);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_PILOT:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_NUM_PILOT);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_HIGH:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_PILOT_HIGH);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_LOW:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_PILOT_LOW);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_SAP:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_NUM_SAP);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_HIGH:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_SAP_HIGH);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_LOW:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetMtsDetection (AUD_FACTORY_MTS_SAP_LOW);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_HDEV:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetHdevMode(AUD_ATV_DECODER_MTS);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_CARRIER_SHIFT:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetCarrierShiftMode(AUD_ATV_DECODER_MTS);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetFMSatuMuteMode(AUD_ATV_DECODER_MTS);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFMSatuMuteTh(AUD_FACTORY_SATU_MUTE_HIGHER);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFMSatuMuteTh(AUD_FACTORY_SATU_MUTE_LOWER);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE_HIGH);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_MTS,AUD_FACTORY_FM_MUTE_LOW);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetMtsMonoFineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_SAP_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetSapFineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_OFFSET:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL*)pv_get_type = AUD_DspGetMtsPilotDetection();
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetSapMute(AUD_FACTORY_SAP_MUTE_LOWER);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetSapMute(AUD_FACTORY_SAP_MUTE_HIGHER);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        /* A2 factory mode */
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_CHECK:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetA2Detection (AUD_FACTORY_A2_NUM_CHECK);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_DOUBLE:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetA2Detection (AUD_FACTORY_A2_NUM_DOUBLE);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_MONO_WEIGHT:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetA2Detection (AUD_FACTORY_A2_MONO_WEIGHT);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_STEREO_WEIGHT:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetA2Detection (AUD_FACTORY_A2_STEREO_WEIGHT);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_DUAL_WEIGHT:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetA2Detection (AUD_FACTORY_A2_DUAL_WEIGHT);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_HDEV:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetHdevMode(AUD_ATV_DECODER_A2);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_CARRIER_SHIFT:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetCarrierShiftMode(AUD_ATV_DECODER_A2);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE_HIGH);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_A2,AUD_FACTORY_FM_MUTE_LOW);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetA2FineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        /* PAL factory mode */
        case DRV_CUSTOM_AUD_TYPE_PAL_CORRECT:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetPalDetection (AUD_FACTORY_PAL_CORRECT);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_SYNC_LOOP:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetPalDetection (AUD_FACTORY_PAL_SYNC_LOOP);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_ERROR:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetPalDetection (AUD_FACTORY_PAL_ERROR);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_PARITY_ERROR:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetPalDetection (AUD_FACTORY_PAL_PARITY_ERROR);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_NUM_FRAMES:
            {
            COPY_FROM_USER_get_type(UINT16);
            *(UINT16 *)pv_get_type = AUD_DspGetPalDetection (AUD_FACTORY_PAL_NUM_FRAMES);
            COPY_TO_USER_get_type(UINT16);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_HDEV:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type =  AUD_DspGetHdevMode(AUD_ATV_DECODER_PAL);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetAmMute(AUD_FACTORY_PAL_AM_MUTE);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetAmMute(AUD_FACTORY_PAL_AM_MUTE_HIGH);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetAmMute(AUD_FACTORY_PAL_AM_MUTE_LOW);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_CARRIER_SHIFT:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type =  AUD_DspGetCarrierShiftMode(AUD_ATV_DECODER_PAL);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_HIGH:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE_HIGH);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_LOW:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetFmMute(AUD_ATV_DECODER_PAL,AUD_FACTORY_FM_MUTE_LOW);
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetPALFineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_AM_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetAMFineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_NICAM_FINE_VOLUME:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetNicamFineVolume();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_FM_SATU_MUTE:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type = AUD_DspGetFMSatuMuteMode(AUD_ATV_DECODER_PAL);
            COPY_TO_USER_get_type(BOOL);
            }
            break;
         /* non-EU factory mode */
        case DRV_CUSTOM_AUD_TYPE_NON_EU:
            {
            COPY_FROM_USER_get_type(BOOL);
            *(BOOL *)pv_get_type =  AUD_DspGetNonEUMode();
            COPY_TO_USER_get_type(BOOL);
            }
            break;
#ifdef CC_AUD_BBE_SUPPORT
        /* BBE factory mode */
        case DRV_CUSTOM_AUD_TYPE_BBE_PRESCALE_LEVEL:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetBbeLevel();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_PROCESSS:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetBbeProcess();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_LO_CONTOUR:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetBbeLoContour();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_3D_GAIN:
            {
            COPY_FROM_USER_get_type(UINT8);
            *(UINT8 *)pv_get_type = AUD_DspGetBbe3dGain();
            COPY_TO_USER_get_type(UINT8);
            }
            break;
#endif
#ifdef SUPPORT_FLASH_AQ
        case DRV_CUSTOM_AUD_TYPE_AQ_VER:
            {
                CHAR cAQVer[8] = {0};
                {
                COPY_FROM_USER_get_type(DRV_CUSTOM_AUD_TYPE_AQ_VER_T);
                AUD_GetFlashAQVer(cAQVer);
                x_memcpy((VOID *)VIRTUAL((UINT32)pv_get_type), cAQVer, sizeof(DRV_CUSTOM_AUD_TYPE_AQ_VER_T));
                COPY_TO_USER_get_type(DRV_CUSTOM_AUD_TYPE_AQ_VER_T);
                }
            }
            break;
#endif
#ifdef CC_AUD_USE_FLASH_AQ
        case DRV_CUSTOM_AUD_TYPE_AQ_VER:
        {
            COPY_FROM_USER_get_type(DRV_CUSTOM_AUD_TYPE_AQ_VER_T);
            Aud_FlashAQRead(FLASH_AQ_VER_OFFSET, (UINT8*)cAQVer, FLASH_AQ_VER_LENGTH);
            u1Len = sizeof(DRV_CUSTOM_AUD_TYPE_AQ_VER_T);
            if (u1Len > FLASH_AQ_VER_LENGTH)
            {
                u1Len = FLASH_AQ_VER_LENGTH;
            }
            x_memcpy((VOID*)VIRTUAL((UINT32)((DRV_CUSTOM_AUD_TYPE_AQ_VER_T*)pv_get_type)), cAQVer, u1Len);
            COPY_TO_USER_get_type(DRV_CUSTOM_AUD_TYPE_AQ_VER_T);
        }
        break;
#endif
        case DRV_CUSTOM_AUD_TYPE_DTS_ENDIAN:
            {
            COPY_FROM_USER_get_type(UINT8);
			*(UINT8 *)pv_get_type = AUD_GetDTSInfo();
			COPY_TO_USER_get_type(UINT8);
			}
			break;

        case DRV_CUSTOM_AUD_TYPE_SMART_AUDIO:
            {
#ifdef CC_AUD_USE_NVM
                COPY_FROM_USER_get_type(SMART_AUD_GROUP_INFO_T);
                if (AUD_NVM_Get_Profile((SMART_AUD_GROUP_INFO_T*)pv_get_type))
                {
                    return RMR_DRV_INV_GET_INFO;
                }
                COPY_TO_USER_get_type(SMART_AUD_GROUP_INFO_T);
#endif
            }
            break;
        default:
            break;
    }
    return RMR_OK;
}

INT32 _MTAUD_d_custom_aud_get_min_max (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    INT16*                    pui2_min,
    INT16*                    pui2_max
)
{
#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    // Implementation
    switch (e_aud_type)
    {
        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN:
            *pui2_min = -256;
            *pui2_max = 256;
            break;
        case DRV_CUSTOM_AUD_TYPE_VOL_GAIN_OFFSET:
            *pui2_min = -256;
            *pui2_max = 256;
            break;
        case DRV_CUSTOM_AUD_TYPE_HEADPHONE_VOL:
            ADAC_GetHeadphoneVolMaxMin(pui2_max,pui2_min);
            break;
        /* MTS factory mode */
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_CHECK:
            *pui2_min = 30;
            *pui2_max = 80;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_PILOT:
           *pui2_min = 0;
           *pui2_max = 50;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_HIGH:
           *pui2_min = 120;
           *pui2_max = 200;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_LOW:
           *pui2_min = 80;
           *pui2_max = 150;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_NUM_SAP:
           *pui2_min = 0;
           *pui2_max = 50;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_HIGH:
           *pui2_min =140;
           *pui2_max = 210;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_SAP_LOW:
           *pui2_min = 70;
           *pui2_max = 130;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_HDEV:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_CARRIER_SHIFT:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU:
        case DRV_CUSTOM_AUD_TYPE_FM_SATU_MUTE:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_HIGH:
           *pui2_min = 25;
           *pui2_max = 255;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_SATU_LOW:
           *pui2_min = 1;
           *pui2_max = 25;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_HIGH:
           *pui2_min = 20;
           *pui2_max = 80;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FM_MUTE_LOW:
           *pui2_min = 110;
           *pui2_max = 180;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
        case DRV_CUSTOM_AUD_TYPE_SAP_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
        case DRV_CUSTOM_AUD_TYPE_MTS_PILOT_OFFSET:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_LOW:
           *pui2_min = 0;
           *pui2_max = 110;
        case DRV_CUSTOM_AUD_TYPE_SAP_MUTE_HIGH:
           *pui2_min = 0;
           *pui2_max = 255;
            break;
        /* A2 factory mode */
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_CHECK:
           *pui2_min = 1;
           *pui2_max = 60;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_NUM_DOUBLE:
           *pui2_min = 5;
           *pui2_max = 60;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_MONO_WEIGHT:
           *pui2_min = 1;
           *pui2_max = 20;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_STEREO_WEIGHT:
           *pui2_min = 1;
           *pui2_max = 20;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_DUAL_WEIGHT:
           *pui2_min = 1;
           *pui2_max = 20;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_HDEV:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_CARRIER_SHIFT:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_HIGH:
           *pui2_min = 0;
           *pui2_max = 96;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FM_MUTE_LOW:
           *pui2_min = 0;
           *pui2_max = 96;
            break;
        case DRV_CUSTOM_AUD_TYPE_A2_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
        /* PAL factory mode */
        case DRV_CUSTOM_AUD_TYPE_PAL_CORRECT:
           *pui2_min = 1;
           *pui2_max = 7;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_SYNC_LOOP:
           *pui2_min = 1;
           *pui2_max = 1001;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_ERROR:
           *pui2_min = 2;
           *pui2_max = 40;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_PARITY_ERROR:
           *pui2_min = 32;
           *pui2_max = 2048;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_NUM_FRAMES:
           *pui2_min = 0;
           *pui2_max = 2048;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_HDEV:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_HIGH:
           *pui2_min = 0;
           *pui2_max = 255;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_AM_MUTE_LOW:
           *pui2_min = 0;
           *pui2_max = 255;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_CARRIER_SHIFT:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_HIGH:
           *pui2_min = 0;
           *pui2_max = 96;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FM_MUTE_LOW:
           *pui2_min = 0;
           *pui2_max = 96;
            break;
        case DRV_CUSTOM_AUD_TYPE_PAL_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
        case DRV_CUSTOM_AUD_TYPE_AM_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
        case DRV_CUSTOM_AUD_TYPE_NICAM_FINE_VOLUME:
           *pui2_min = 0;
           *pui2_max = 40;
            break;
         /* non-EU factory mode */
        case DRV_CUSTOM_AUD_TYPE_NON_EU:
           *pui2_min = 0;
           *pui2_max = 1;
            break;
        /* BBE factory mode */
        case DRV_CUSTOM_AUD_TYPE_BBE_PRESCALE_LEVEL:
           *pui2_min = (-BBE_LEVEL_MAX);
           *pui2_max = 0;
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_PROCESSS:
           *pui2_min = BBE_PROCESS_MIN;
           *pui2_max = BBE_PROCESS_MAX;
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_LO_CONTOUR:
           *pui2_min = BBE_LO_CONTOUR_MIN;
           *pui2_max = BBE_LO_CONTOUR_MAX;
            break;
        case DRV_CUSTOM_AUD_TYPE_BBE_3D_GAIN:
           *pui2_min = 0;
           *pui2_max = BBE_3D_GAIN_MAX;
           break;

        default:
            break;
    }
    return RMR_OK;
}

INT32 _MTAUD_MW_AUD_Init(void)
{
    UINT8 u1DecIdx, u1VolBaseIdx;

#ifdef CC_SUPPORT_STR
    if (fgAudPmStrMode)
        return RMR_OK;
#endif

    //CLI_Parser("aud.d_l 3");
    UNUSED(_fgPlayAudDscr); //warning message

    /* Init MtkAud driver */
    //AUD_Init();

    /* Register middleware notify function */
    _arAudMwNotifyInfo[AUD_DEC_MAIN].pv_tag = 0;
    _arAudMwNotifyInfo[AUD_DEC_MAIN].pf_aud_dec_nfy = NULL;
    _arAudMwNotifyInfo[AUD_DEC_AUX].pv_tag = 0;
    _arAudMwNotifyInfo[AUD_DEC_AUX].pf_aud_dec_nfy = NULL;
    _arAudHPMwNotifyInfo[AUD_DEC_MAIN].pv_nfy_tag = 0;
    _arAudHPMwNotifyInfo[AUD_DEC_MAIN].pf_hdph_detect_nfy = NULL;
    _arAudHPMwNotifyInfo[AUD_DEC_AUX].pv_nfy_tag = 0;
    _arAudHPMwNotifyInfo[AUD_DEC_AUX].pf_hdph_detect_nfy = NULL;
    _arAudYWMwNotifyInfo[AUD_DEC_MAIN].pv_nfy_tag = 0;
    _arAudYWMwNotifyInfo[AUD_DEC_MAIN].pf_upload_data_nfy = NULL;
    _arAudYWMwNotifyInfo[AUD_DEC_AUX].pv_nfy_tag = 0;
    _arAudYWMwNotifyInfo[AUD_DEC_AUX].pf_upload_data_nfy = NULL;
#if 1 //def CC_MT5391_AUD_3_DECODER  //gallen 0521
    _arAudMwNotifyInfo[AUD_DEC_THIRD].pv_tag = 0;
    _arAudMwNotifyInfo[AUD_DEC_THIRD].pf_aud_dec_nfy = NULL;
    _arAudHPMwNotifyInfo[AUD_DEC_THIRD].pv_nfy_tag = 0;
    _arAudHPMwNotifyInfo[AUD_DEC_THIRD].pf_hdph_detect_nfy = NULL;
    _arAudYWMwNotifyInfo[AUD_DEC_THIRD].pv_nfy_tag = 0;
    _arAudYWMwNotifyInfo[AUD_DEC_THIRD].pf_upload_data_nfy = NULL;
#endif

    // Initialize channel volume table
    u1VolBaseIdx = DRVCUST_OptGet(eAudioVolBaseLevel);
    for (u1DecIdx=0; u1DecIdx<2; u1DecIdx++)
    {
        _arChVol[u1DecIdx].u1VolL = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolR = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolSL = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolSR = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolC = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolSW = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolDmxL = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolDmxR = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolBypassL = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolBypassR = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolMaster = u1VolBaseIdx;
        _arChVol[u1DecIdx].u1VolTone = u1VolBaseIdx;
    }

#if 0//def CC_MT5391_AUD_3_DECODER
    // Set TV Scart out to be muted as default
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_DspChannelMute(AUD_DSP0, AUD_DEC_AUX, AUD_CH_FRONT_LEFT, TRUE);
    AUD_DspChannelMute(AUD_DSP0, AUD_DEC_AUX, AUD_CH_FRONT_RIGHT, TRUE);
#else
    AUD_DspChannelMute(AUD_DEC_AUX, AUD_CH_FRONT_LEFT, TRUE);
    AUD_DspChannelMute(AUD_DEC_AUX, AUD_CH_FRONT_RIGHT, TRUE);
#endif
#endif

    //Init PL2 switch setting  to OFF.
    _rPrologic2Type.u.e_pl2_switch = AUD_DEC_PL2_SWITCH_OFF;

    return RMR_OK;
}


