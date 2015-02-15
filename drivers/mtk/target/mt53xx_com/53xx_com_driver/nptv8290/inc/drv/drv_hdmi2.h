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
 * $Date  $
 * $RCSfile: drv_hdmi.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifndef _DRV_HDMI2_H_
#define _DRV_HDMI2_H_

#include "typedef.h"
#include "x_timer.h"

#define HDMI_RX_NEW_AVI (1<<16)
#define HDMI_RX_NEW_SP (1<<17)
#define HDMI_RX_NEW_AUD (1<<18)
#define HDMI_RX_NEW_MPEG (1<<19)
#define HDMI_RX_NEW_UNREC (1<<20)
#define HDMI_RX_NEW_CP (1<<23)


#define HDMI_RX_AAC_MUTE (1<<6)
#define HDMI_RX_NEW_ACP (1<<10)
#define HDMI_RX_NEW_VSIF (1<<22)

#define HDMI_RX_NEW_ISRC1 (1<<2)
#define HDMI_RX_NEW_ISRC2 (1<<3)
#define HDMI_RX_NEW_GCP   (1<<4)
#define HDMI_RX_NEW_HF_VSIF (1<<9)
typedef enum
{
	HDMI_RX_PORT_Init=0,
	HDMI_RX_PORT_1,
	HDMI_RX_PORT_2,
	HDMI_RX_PORT_3,
	HDMI_RX_PORT_4,
	HDMI_RX_PORT_MAX
}HdmiRxPort;

enum 
{
	HDMI_NOSIGNAL_STATE=0,
	HDMI_WAIT_CONNECT_STATE,
	HDMI_WAIT_STABLE_STATE,
	HDMI_STABLE_STATE
};
typedef enum
{
	HDMI_RX_MODE_DVI,
	HDMI_RX_MODE_HDMI1X,
	HDMI_RX_MODE_HDMI2X,
	HDMI_RX_MODE_MHL
}HdmiRxMode_t;

typedef enum
{
	HDMI_RX_CLRSPC_YC444_601,
	HDMI_RX_CLRSPC_YC422_601,
	HDMI_RX_CLRSPC_YC420_601,

	HDMI_RX_CLRSPC_YC444_709,
	HDMI_RX_CLRSPC_YC422_709,
	HDMI_RX_CLRSPC_YC420_709,

	HDMI_RX_CLRSPC_XVYC444_601,
	HDMI_RX_CLRSPC_XVYC422_601,
	HDMI_RX_CLRSPC_XVYC420_601,

	HDMI_RX_CLRSPC_XVYC444_709,
	HDMI_RX_CLRSPC_XVYC422_709,
	HDMI_RX_CLRSPC_XVYC420_709,

	HDMI_RX_CLRSPC_sYCC444_601,
	HDMI_RX_CLRSPC_sYCC422_601,
	HDMI_RX_CLRSPC_sYCC420_601,
	
	HDMI_RX_CLRSPC_Adobe_YCC444_601,
	HDMI_RX_CLRSPC_Adobe_YCC422_601,
	HDMI_RX_CLRSPC_Adobe_YCC420_601,
	
	HDMI_RX_CLRSPC_RGB,
	HDMI_RX_CLRSPC_Adobe_RGB,
	
	HDMI_RX_CLRSPC_BT_2020_RGB_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_RGB_const_luminous,
	
	HDMI_RX_CLRSPC_BT_2020_YCC444_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC422_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC420_non_const_luminous,
	
	HDMI_RX_CLRSPC_BT_2020_YCC444_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC422_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC420_const_luminous

}HdmiRxClrSpc_t;

typedef enum
{
	HDMI_RX_BIT_DEPTH_8_BIT,
	HDMI_RX_BIT_DEPTH_10_BIT,
	HDMI_RX_BIT_DEPTH_12_BIT,
	HDMI_RX_BIT_DEPTH_16_BIT
}HdmiRxBitDepth_t;

typedef enum
{
	HDMI_RX_RGB_FULL,
	HDMI_RX_RGB_LIMT,	
	HDMI_RX_YCC_FULL,
	HDMI_RX_YCC_LIMT
}HdmiRxRange_t;
typedef enum
{
	HDMI_RX_OVERSCAN=1,
	HDMI_RX_UNDERSCAN
}HdmiRxScanInfo_t;

typedef enum
{
	HDMI_RX_ASPECT_RATIO_4_3=1,
	HDMI_RX_ASPECT_RATIO_16_9
}HdmiRxAspectRatio_t;

typedef enum
{
	HDMI_RX_AFD_SAME=8,
	HDMI_RX_AFD_4_3,
	HDMI_RX_AFD_16_9,
	HDMI_RX_AFD_14_9
}HdmiRxAFDInfo_t;

typedef enum
{
	HDMI_RX_ITC_GRAPHICS=0,
	HDMI_RX_ITC_PHOTO,
	HDMI_RX_ITC_CINEMA,
	HDMI_RX_ITC_GAME
}HdmiRxITCInfo_t;

typedef enum
{
	HDMI_RX_HDCP_LINK_VER_NONE,
	HDMI_RX_HDCP_LINK_VER_HDCP1X,	
	HDMI_RX_HDCP_LINK_VER_HDCP22,
}HdmiRxHdcpLinkVer_t;

typedef enum
{
	HDMI_RX_HDCP2_STATUS_OFF=0x0,
	HDMI_RX_HDCP2_STATUS_AUTH_DONE =0x1,	
	HDMI_RX_HDCP2_STATUS_AUTH_FAIL=0x2,
	HDMI_RX_HDCP2_STATUS_CCHK_DONE=0x4,
	HDMI_RX_HDCP2_STATUS_CCHK_FAIL=0x8,
	HDMI_RX_HDCP2_STATUS_AKE_SENT_RCVD=0x10,
	HDMI_RX_HDCP2_STATUS_SKE_SENT_RCVD=0x20,
	HDMI_RX_HDCP2_STATUS_CERT_SENT_RCVD=0x40
}HdmiRxHdcp2Status_t;

typedef enum
{
	HDMI_RX_HDCP1_STATUS_OFF =0x0,
	HDMI_RX_HDCP1_STATUS_AUTH_DONE =0x1,	
	HDMI_RX_HDCP1_STATUS_AUTH_FAIL =0x2,
	HDMI_RX_HDCP1_STATUS_DECRYPT_SUCCESS=0x4,
	HDMI_RX_HDCP1_STATUS_DECRYPT_FAIL=0x8
}HdmiRxHdcp1Status_t;

typedef enum
{
	HDMI_RX_AUDIO_FS_24KHZ,
	HDMI_RX_AUDIO_FS_32KHZ,
	HDMI_RX_AUDIO_FS_44_1KHZ,
	HDMI_RX_AUDIO_FS_48KHZ,
	HDMI_RX_AUDIO_FS_88_2KHZ,
	HDMI_RX_AUDIO_FS_96KHZ,
	HDMI_RX_AUDIO_FS_176_4KHZ,
	HDMI_RX_AUDIO_FS_192KHZ,
	HDMI_RX_AUDIO_FS_768KHZ
}HdmiRxAudioFs_t;

typedef enum
{
	HDMI_TMDS_CLOCK_UNKNOW,
	HDMI_TMDS_CLOCK_25,
	HDMI_TMDS_CLOCK_25_35,
	HDMI_TMDS_CLOCK_35_40,
	HDMI_TMDS_CLOCK_40_50,
	HDMI_TMDS_CLOCK_50_80,
	HDMI_TMDS_CLOCK_80_100,
	HDMI_TMDS_CLOCK_100_120,
	HDMI_TMDS_CLOCK_120_140,
	HDMI_TMDS_CLOCK_140_200,
	HDMI_TMDS_CLOCK_200_300
}HdmiTmdsClkRate_t;

typedef enum
{
	HDMI_AUDIO_FIFO_NORMAL= 0x1,
	HDMI_AUDIO_FIFO_UNDERRUN= 0x2,
	HDMI_AUDIO_FIFO_OVERRUN= 0x4
}HdmiAudFifoStatus_t;

typedef enum
{
    HDMI_TMDS_DATA_CHANNEL_0,
    HDMI_TMDS_DATA_CHANNEL_1,
    HDMI_TMDS_DATA_CHANNEL_2
}HdmiTmdsDataChannel_t;

typedef enum
{
	HDMI_INFO_FRAME_ID_AVI,
	HDMI_INFO_FRAME_ID_SPD,
	HDMI_INFO_FRAME_ID_AUD,
	HDMI_INFO_FRAME_ID_MPEG,
	HDMI_INFO_FRAME_ID_UNREC,
	HDMI_INFO_FRAME_ID_ACP,
	HDMI_INFO_FRAME_ID_VSI,
	HDMI_INFO_FRAME_ID_ISRC0,
	HDMI_INFO_FRAME_ID_ISRC1,
	HDMI_INFO_FRAME_ID_GCP,
	HDMI_INFO_FRAME_ID_HFVSI
}HdmiInfoFrameID_t;
typedef struct
{
	HdmiRxClrSpc_t clrSpc;
	HdmiRxRange_t range;
}HdmiVideoInfo_t;

typedef struct
{
	BOOL fgItc;
	HdmiRxITCInfo_t ItcContent;
}HdmiItcInfo_t;

typedef struct
{
	UINT8 HDMI_3D_Enable;
	UINT8 HDMI_3D_Video_Format;
	UINT8 HDMI_3D_Structure;
	UINT8 HDMI_3D_EXTDATA;
}HdmiVideo3DInfo_t;

typedef struct
{
	UINT8 layout;
	UINT8 dsd;
	UINT8 hbrA;
	HdmiRxAudioFs_t audioFs;
}HdmiRxAudioFormat_t;

typedef struct
{
	
	UINT8 u1CategoryCode;
	UINT8 u1WordLength;
	UINT8 b1;
	UINT8 b2;
	UINT8 b3;
	BOOL fgCopyRight;
	BOOL fgIsPCM;
}HdmiRxAudioChannelStatus_t;

typedef struct
{
	HdmiRxAudioFormat_t audFormat;
	HdmiRxAudioChannelStatus_t chlStatus;
}HdmiRxAudioInfomation_t;

typedef struct
{
	HdmiInfoFrameID_t u1InfoID;
	UINT8 u1InfoData[32];
}HdmiRxInfo_t;

typedef struct
{
	UINT32 _u4HTotal;
	UINT32 _u4VTotal;
	UINT32 _u4Width;
	UINT32 _u4Height;
}HdmiRxRes_t;

typedef struct
{
	UINT8 u1ScramblingOn;
	UINT8 u1ClkRatio;
}HdmiScdcStatus_t;

typedef struct
{
  UINT8 _bHdmiCmd;
  UINT8 _bHDMIState;
  UINT8 _bHDMIStateOld;
  UINT8 _bHDMIPower5vStatus;
  UINT8 _bHdmiPlug;
  UINT8 _bUnplugFlag;
  UINT8 _bHDMIAvMute;
  UINT16 _bHDMIRxBaseAddr;
  UINT16 _bHDMIAnaBaseAddr;
  UINT8 _bHDMICkdt;
  UINT8 _bHDMIScdt;
  UINT8 _bHDMIScdtLose;
  UINT8 _bHDMIHpdTimerCnt;
  UINT8 _bHDMIHdcp1xGood;
  UINT8 _bHDMIHdcp2xGood;
  UINT8 _bHDMIResStableCnt;
  UINT8 _bHDMIResUnStableCnt;
  UINT8 _bHDMIDviModeCnt;
  UINT8 _bIs422;
  UINT8 _bResv;
  UINT8 _bResv1;
  
  HdmiRxMode_t _HdmiMode;

  HdmiRxClrSpc_t _HdmiClrSpc;
  HdmiRxBitDepth_t _HdmiDepth;
  HdmiRxRange_t _HdmiRange;
  HdmiRxScanInfo_t _HdmiScan;
  HdmiRxAspectRatio_t _HdmiAspect;
  HdmiRxAFDInfo_t	_HdmiAFD;
  HdmiItcInfo_t     _HdmiITC;
  HdmiVideo3DInfo_t _Hdmi3DInfo;
  HdmiRxHdcpLinkVer_t _HdcpLinkVer;
  //HdmiRxHdcpStatus_t _HdcpState;
  HdmiTmdsClkRate_t _TmdsClkRate;
 
  HdmiRxRes_t	_ResInfo;
  HdmiScdcStatus_t _ScdcInfo;
  HdmiRxInfo_t _AviInfo;
  HdmiRxInfo_t _SpdInfo;
  HdmiRxInfo_t _AudInfo;
  HdmiRxInfo_t _MpegInfo;
  HdmiRxInfo_t _UnrecInfo;
  HdmiRxInfo_t _AcpInfo;
  HdmiRxInfo_t _VsiInfo;
  HdmiRxInfo_t _Isrc0Info;
  HdmiRxInfo_t _Isrc1Info;
  HdmiRxInfo_t _GcpInfo;
  HdmiRxInfo_t _HfVsiInfo;
  HdmiRxAudioInfomation_t _AudStatus;
  BOOL _fgNoScdtWithCkdt;
  UINT8 _bNoScdtWithCkdtCount;
  UINT8 _bHDMILastAudFreq;

} HdmiRxObject_t;

extern void vHDMI2Init(void);
extern void vHDMI2HwInit(HdmiRxPort ePort);
extern void vHDMI2MainLoop(void);
extern void vHDMI2TMDSCTRL(HdmiRxPort ePort,BOOL bOnOff);
extern void vHDMI2DDCBusCTRL(HdmiRxPort ePort,BOOL bOnOff);
extern UINT16 u2HDMI2GetDigBaseAddr(HdmiRxPort ePort);
extern UINT8 u1GetMinHDMI20Port(void);
extern UINT16 u2HDMI2GetAnaBaseAddr(HdmiRxPort ePort);
extern void vHDMI2PortSel(HdmiRxPort ePort);
extern UINT32 u4GetHdmi2RegBase(HdmiRxPort ePort);
extern UINT32 u4GetHdmi2AnalogRegBase(HdmiRxPort ePort);
extern void vHDMI2SetStateMachine(HdmiRxPort ePort,UINT8 u1HdmiSta);
extern UINT8 bHDMI2GetStateMachine(HdmiRxPort ePort);
extern void vHDMI2StopFw(HdmiRxPort ePort);
extern void vHDMI2ResumeFw(HdmiRxPort ePort);
extern UINT8 u1HDMI2IsUnplug(HdmiRxPort ePort);
extern void vHDMI2ClrUnplug(HdmiRxPort ePort);
extern void vHDMI2SetUnplug(HdmiRxPort ePort);
extern UINT8 u1HDMI2IsHpdPulse(HdmiRxPort ePort);
extern void vHDMI2SetHpdPulse(HdmiRxPort ePort,UINT8 u1Val);
extern UINT8 u1HDMI2IsSyncLose(HdmiRxPort ePort);
extern void vHDMI2SetSyncLose(HdmiRxPort ePort,UINT8 u1Val);
extern UINT8 u1HDMI2GetDviModeCnt(HdmiRxPort ePort);
extern BOOL fgHDMI2GetDuleMode(HdmiRxPort ePort);
extern BOOL fgHDMI2Get420Mode(HdmiRxPort ePort);
extern BOOL fgHDMI2IsPixelRepeate(HdmiRxPort ePort);
extern UINT8 bHDMI2AudStatus(void);
extern UINT8 bHDMI2InputType(HdmiRxPort ePort);
extern UINT8 bHDMI2Clocrimetry(HdmiRxPort ePort);
extern UINT8 bHDMI2RgbRange(HdmiRxPort ePort);
extern void vHDMI2AudStatusSet(HdmiRxPort ePort,UINT8 u1On);
extern UINT8 u1HDMI2GetAudioSamplingFreq(void);
extern UINT8 bHDMI2ScanInfo(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2AspectRatio(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2AFD(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2422Input(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2GetColorDepth(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2ITCFlag(HdmiRxPort eHdmiPort);
extern UINT8 bHDMI2ITCContent(HdmiRxPort eHdmiPort);
extern UINT8 bHDMIHDCP1StatusGet(HdmiRxPort eHdmiPort);
extern UINT8 bHDMIHDCP2StatusGet(HdmiRxPort eHdmiPort);
extern void vHDMI2PrintHdmiStatus(HdmiRxPort eHdmiPort);
extern UINT8 u1IsHDMI2Stable(HdmiRxPort ePort);
extern void vHDMI2VideoMute(HdmiRxPort ePort,UINT8 u1En);
extern void HDMI2_pm_suspend(void);
extern void HDMI2_pm_resume(void);
#endif

