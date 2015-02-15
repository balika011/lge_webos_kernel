/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
/*! \file u_pbinf.h
*
* \par Project
*    MT8520
*
* \par Description
*    Playback information definition.
*
* \par Author_Name
*    Victor Lin
*
* \par Last_Changed
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#ifndef _U_PBINF_H_
#define _U_PBINF_H_

#include "u_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// use PID as component ID
#define LP_IBC_PID_SAME_FILTERID 1

//unit : second
#define AVI_AVSYNC_THRESHOLD 3

// New audio PbInfo
#define NEW_AUD_PBINFO

// RSP after parsing end
#define LP_RSP_AFTER_PRS_END 1

/// Video mpeg playback information
typedef struct _V_MPG
{
    UINT64 u8PicPckPos;           ///< picture pack position
    BOOL fgScPartial;
    INT64  i8StcOffset;           ///< cfa provide it to LPE.
} V_MPG;

/// Video avi playback information
typedef struct _V_AVI
{
    UINT32 u4TxedChunk;         ///< Txed Chunk
    UINT64 u8PrevPTS;           ///< Prev Video Chung PTS Modify Flag
} V_AVI;

/// Video asf playback information
typedef struct _V_ASF
{
    UINT32 u4PacketNum;         ///< packet num
    UINT32 u4KfPacketNum;     ///< last key frame packet num
    UINT32 u4PicPTS;            ///< pic PTS
    UINT64 u8KfPicPTS;        ///< last key frame PTS
} V_ASF;
/// Video mp4 playback information
typedef struct _V_MP4
{
    UINT32 u4Chunk;         ///< packet num
    UINT32 u41stSamp;    ///Current chunk first sample no
    UINT32 u4Samp;         ///Current sample no
    UINT32 u4PicPTS;            ///< pic PTS
    UINT64 u8Ofst;
} V_MP4;

/// Video mkv playback information
typedef struct _V_MKV
{
    UINT64 u8allPicTimeCode;     //zhenwang add for BDP00055200 
    UINT64 u8PicTimeCode;         ///< Key frame time code
    UINT64 u8VidClusterStartAdr;     ///< Cluster start offset of Key frame
    UINT32 u4VidBlockNo;
    UINT32 u4AudBlockNo;
    UINT64 u8AudTimeCode;
    UINT64 u8AudClusterStartAdr;
} V_MKV;

/// Video ogm playback information
typedef struct _V_OGM
{
    INT64  i8VidLastGranule;
    UINT64 u8VidStartOfstKF;
    UINT8  uVidPacketStartNoKF;

    INT64  i8AudLastGranule;
    UINT64 u8AudStartOfst;
    UINT8  uAudPacketStartNo;
} V_OGM;

//mtk40301 for swf 20090925
/// Video swf playback information
typedef struct _V_SWF
{
    UINT64 u8VidPTS;
    UINT64 u8VidStartOfstKF;
    UINT64 u8VidStartOfst;

   // UINT64 u8AudPTS;
   // UINT64 u8AudStartOfst;
} V_SWF;


//mtk40156 20090814 for flv
typedef struct _V_FLV
{
    UINT64 u8Pts;
    UINT64 u8Offset;
    UINT64 u8IFrmOft;
}V_FLV;

//mtk40156 20090908 for rm
typedef struct _V_RM
{
    UINT64 u8Pts;
    UINT64 u8Offset;
    UINT64 u8IFrmOft;
    UINT16 u2AStreamNum; //for idx data
    UINT16 u2VStreamNum; //for idx data
}V_RM;

//mtk40066 20100329 for mtk private format.
typedef struct _V_MPF
{
    UINT64 u8VidPTS;
    UINT64 u8VidStartOfstKF;
    UINT64 u8VidStartOfst;
}V_MPF;

/// Playback information of a video
typedef union
{
  V_MPG rMpg;       ///< mpeg type playback information
  V_AVI rAvi;         ///< AVI type playback information
  V_ASF rAsf;
  V_MP4 rMp4;
  V_MKV rMkv;
  V_OGM rOgm;
  V_FLV rFlv;
  V_SWF rSwf;
  V_RM rRm;
  V_MPF rMpf;
} PBINF_V;



/// Audio VCD playback information
typedef struct _A_VCD
{
  UINT64 u8Pts;                          ///< PTS
  UINT64 u8Offset;                     ///< Offset
}A_VCD;

/// Audio CDDA playback information
typedef struct _A_CDDA
{
  UINT8 u1TrackNo;                 ///< Track number, 0xFF means invalid
  UINT8 u1TrackIdx;                ///< Track index
  UINT8 u1RMin;                       ///< R_time minute
  UINT8 u1RSec;                       ///< R_time second
  UINT8 u1RFrm;                      ///< R_time frame
  UINT8 u1AMin;                       ///< A_time minute
  UINT8 u1ASec;                       ///< A_time second
  UINT8 u1AFrm;                      ///< A_time frame
  UINT64 u8Offset;                   ///< Offset
}A_CDDA;

typedef struct _A_MP3
{
  UINT32 u4BitRate;            ///< Bit Rate
  UINT32 u4CurPlayTime;           ///< Current play time (sec)
  UINT64 u8Offset;             ///The current play offset, unit is byte
}A_MP3;

/// Audio wma playback information
typedef struct _A_WMA
{
	UINT64 u8PlayTime;         ///Play time information, unit is millisecond(ms)
	UINT64 u8Offset;           ///The current play offset, unit is byte
	UINT64 u8AudioPts;        ///audio playload pts
} A_WMA;

/// Audio sacd playback information
typedef struct _A_SACD
{
	UINT32 u4FrameLen;         ///Frame length, unit is byte
	UINT32 u4TimeCode;         ///Time code
	UINT32 u4PaddingLen;       ///Padding length, unit is byte
	UINT64 u8Offset;           ///The current play offset, unit is byte
} A_SACD;

/// Audio ogm playback information
typedef struct _A_OGM
{
    INT64  i8AudLastGranule;
    UINT64 u8AudStartOfst;
    UINT8  uAudPacketStartNo;
    UINT64 u8PTS;
} A_OGM;

//mtk40301 090929
/// Audio swf playback information
typedef struct _A_SWF
{
    UINT64 u8AudStartOfst;
    UINT64 u8PTS;
} A_SWF;

//mtk40066 20100329 for mtk private format
typedef struct _A_MPF
{
    UINT64 u8AudStartOfst;
    UINT64 u8PTS;
} A_MPF;

/// Audio mpeg playback information
typedef struct _A_MPEG
{
    UINT64 u8AudPTS;
    UINT64 u8AudPckPos;           ///< Audio pack position
} A_MPEG;

/// Audio mkv playback information
typedef struct _A_MKV
{
	UINT64 u8AudForMp3; /* Yi Feng modify to fix BDP00112113 @2009/06/12 */
    UINT64 u8AudClusterStartAdr;
    UINT64 u8AudTimeCode;
    UINT32 u4AudBlockNo;	
} A_MKV;

/// Audio mpeg playback information
typedef struct _A_MP4
{
    UINT64 u8AudDur;
    UINT64 u8AudPts;           ///< Audio pack position
    UINT64 u8Ofst;
} A_MP4;

/// Audio general type playback information
typedef struct _A_GENERAL
{
  UINT64 u8Pts;               // PTS
  UINT64 u8LocPbBufSlotOfst;  // located pb buffer slot offset 
}A_GENERAL;

#if 1//CONFIG_SUPPORT_AUDIO_IN
typedef struct _A_AUDIN
{
  UINT32 u4BitRate;            ///< Bit Rate
  UINT32 u4CurPlayTime;        ///< Current play time (sec)
  UINT64 u8Offset;             ///The current play offset, unit is byte
}A_AUDIN;
#endif

#define CFA_MPG_DVD_AUDIO_DE_SUPPORT            1
#if CFA_MPG_DVD_AUDIO_DE_SUPPORT
 /// DVD audio playback information
typedef struct _A_DVDAUDIO
{
    UINT64 u8AudPTS;
    UINT64 u8AudPckPos;           ///< Audio pack position
} A_DVDAUDIO;
#endif

typedef struct _A_APE  //mtk70169 for time show
{
  UINT32 u4TimeInfoFrameNumLo;
  UINT32 u4TimeInfoFrameNumHi;
  UINT32 u4ApeErrType;
  UINT32 u4ApeBanknumInFrame;    
}A_APE;

typedef struct _A_FLAC
{
  UINT32 u4TimeInfoFrameNumL24;
  UINT32 u4TimeInfoFrameNumH12;
  UINT32 u4FlacErrType;
}A_FLAC;

typedef struct _A_AVI
{
    UINT64 u8AudOfst;
    UINT64 u8PTS;
} A_AVI;

typedef struct _A_RM
{
    UINT64 u8AudOfst;
    UINT64 u8PTS;
} A_RM;
#ifdef NEW_AUD_PBINFO
typedef union
{
  A_GENERAL  rGeneral;         ///< General audio type playback information
  A_VCD      rVCD;             ///< VCD playback information
  A_CDDA     rCDDA;            ///< CDDA playback information
  A_MP3      rMP3;             ///< MP3 playback information
  A_WMA      rWMA;             ///< WMA playback information
  A_SACD     rSACD;            ///< SACD playback information
  A_OGM      rOGM;
  A_SWF      rSWF;
  A_MPEG     rMPEG;           ///< MPEG playback information
  A_MKV      rMKV;
  #if CFA_MPG_DVD_AUDIO_DE_SUPPORT
  A_DVDAUDIO      rDVDAUDIO;           ///< DVD AUDIO playback information
  #endif
  A_MP4      rMP4; 
#if 1//CONFIG_SUPPORT_AUDIO_IN
  A_AUDIN   rAudIn;
#endif
  A_MPF     rMpf;
  A_APE     rAPE;    //mtk70169 for time show
  A_FLAC   rFLAC;
  A_AVI     rAvi;
  A_RM      rRm;
}PBINF_AUD;

typedef struct _PBINF_A
{
    UINT32 u8DspPlayBackTime;      ///< DSP playback time
    PBINF_AUD rPbInf;             ///<Audio Playback information
} PBINF_A;

#else
/// access unit information of Audio
typedef union
{
  A_GENERAL  rGeneral;         ///< General audio type playback information
  A_VCD      rVCD;             ///< VCD playback information
  A_CDDA     rCDDA;            ///< CDDA playback information
  A_MP3      rMP3;             ///< MP3 playback information
  A_WMA      rWMA;             ///< WMA playback information
  A_SACD     rSACD;            ///< SACD playback information
#if 1//CONFIG_SUPPORT_AUDIO_IN
  A_AUDIN   rAudIn;
#endif
  A_MPF     rMpf;
  A_APE     rAPE;    //mtk70169 for time show
  A_FLAC   rFLAC;  
}PBINF_A;
#endif

/// SubPic playback information
typedef struct _PBINF_SP
{
  // TODO: should change to a knowed name
  UINT64 u8CusInf;                         ///< custom information
  UINT32 u4HLI_S_PTM;                      ///< High light information, 0xffffffff means invalid
  UINT32 u4HLI_E_PTM;                      ///< High light information
  #if  CFA_MPG_DVD_AUDIO_DE_SUPPORT
  UINT16 u2SpStreamId;    ///< sub picture stream id
  UINT16 u2SpUnitIndexId;///< sub picture unit index
#endif
}PBINF_SP;

/// NV playback information
typedef struct _PBINF_NV
{
  // TODO: should change to a knowed name
  UINT64 u8CusInf;                         ///< custom information
  UINT64 u8Pts;                            ///< PTS
}PBINF_NV;

/// Graphic playback information
typedef struct _PBINF_G
{
  // TODO: should change to a knowed name
  UINT64 u8CusInf;                         ///< custom information
}PBINF_G;

#ifdef __cplusplus
}
#endif

#endif //#ifndef _U_PBINF_H_

