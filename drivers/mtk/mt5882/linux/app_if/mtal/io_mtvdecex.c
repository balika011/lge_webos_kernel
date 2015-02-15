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
 * $RCSfile: io_mtvdecex.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/
/** @file MTVDECEX.c
 *  APIs of external VDEC modules.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtvdecex.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>
#include "mtutil.h"

#include "nptv_if.h"
#include "sv_const.h"
#include "source_table.h"
#include "video_def.h"
#include "video_timing.h"
#include "drv_adcset.h"
#include "hw_hdmi.h"
#include "drv_hdmi.h"
#include "drv_dvi.h"
#include "drv_tvd.h"
#include "vdo_misc.h"
#include "b2r_if.h"
#include "vdec_if.h"
#include "drv_video.h"
#include "drv_auto.h"
#ifdef SYS_MHL_SUPPORT
#include "mhl_if.h"
#endif
#ifdef CC_SUPPORT_W2DR2R
#include "drv_w2dr2r.h"
#endif
#if defined(CC_MT5882)
#ifdef SYS_MHL_SUPPORT
extern UINT8  _bIsMhlDevice;//0:HDMI   1:MHL
#endif
#endif
extern UINT8 bHDMIGetAudioBitWidth(void);
extern void vHDMIAudChannelStatus(BOOL* pfgCopyright, UINT8* pu1CategoryCode, 
                                UINT8* p1WordLength, BOOL* pfgIsPcm);
 extern UINT8 bHDMI_IsPCM(void);    
 extern UINT8 bDrvVideoSignalStatus(UINT8 bPath);
extern UINT8 u1HDMI_SetEdidDataByPort(UINT8 port, UINT8 * pData);
extern UINT8 u1HDMI_SetExternalEdidDataByPort(UINT8 u1Port, UINT8 * pData);
extern UINT8 u1HDMI_GetExternalEdidDataByPort(UINT8 u1Port, UINT8 *pData, UINT16 u2Length);
extern void vStopMHL(void);
extern void vResumeMHL(void);
extern void vCbusStop(void);
extern void vCbusResume(void);
extern UINT8 vIsRcpReceive(void);
extern UINT8 vRcpReceiveCode(void);
extern void vClearRcpState(void);
extern BOOL fgHDMIIsPixelRepeate(void);
extern BOOL i4VDOOmxPlaying(UINT8 uPath,UINT8 uSrc);

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
#define MTVDECEX_PRINT_AMBIG_TIMINGT(rAmbigVgaTimingInfo)                       \
    do{\
        if(IS_MTVDECEX_LOG_ENABLE)\
        {                                        \
            printf("rAmbigVgaTimingInfo.u1RefreshRate = %d\n",(int)rAmbigVgaTimingInfo.u1RefreshRate);\
            printf("rAmbigVgaTimingInfo.u2Width = %d\n",(int)rAmbigVgaTimingInfo.u2Width);\
            printf("rAmbigVgaTimingInfo.u2Height= %d\n",(int)rAmbigVgaTimingInfo.u2Height);\
         }\
    }while(0)

#define MTVDECEX_PRINT_INFO_FRAME(rInfoFrame)                       \
    do{\
        if(IS_MTVDECEX_LOG_ENABLE)\
        {                                        \
            printf("rFrame.HDMI_FrameSize = %d\n",(int)rInfoFrame.HDMI_FrameSize);\
            for(i=0; i <10; i++)\
           {                             \
            printf("rFrame.HDMI_FrameData[%d] = %x\n",i,(int)rInfoFrame.HDMI_FrameData[i]);\
         }\
         }\
    }while(0)
#if 0 //mtutil.h
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");     \
        return MTR_ERR_INV;                                          \
    }
#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
    
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdecex argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#endif

#define COPY_TO_USER_DATA_ALLOC(ArgUsr, ArgKernel, size)            \
    if ((ArgKernel == NULL) || (copy_to_user((void __user *)ArgUsr, \
        (void *)ArgKernel, size)))                                  \
    {                                                               \
        printk("mtal_ioctl_mtvdecex argument error\n");             \
        kfree(ArgKernel);                                           \
        return MTR_ERR_INV;                                         \
    }
#define COPY_FROM_USER_DATA_ALLOC(ArgUsr, ArgKernel, size)          \
    if ((ArgKernel == NULL) || (copy_from_user((void *)ArgKernel,   \
        (void __user *)ArgUsr, size)))                              \
    {                                                               \
        printk("mtal_ioctl_mtvdecex argument error\n");             \
        kfree(ArgKernel);                                           \
        return MTR_ERR_INV;                                         \
    }


typedef VOID (*x_signal_usr_nfy_fct) (BOOL OnOffStatus);//singnal booster add by Hua 0609    


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

UINT32 _u4OversampleDependent = 1;

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


UINT8 _MTVDECEX_VdoSrcMap(MTVDECEX_SRC_T eSrc)
{
	switch(eSrc)
	{
#if 0
		case	MTVDECEX_SRC_ATV1:
#ifdef USE_ATD_IF_DEMOD
                                return SV_VS_ATD1; // not using ATD currently
#else
                                return SV_VS_ATV1;
#endif
#else
		case	MTVDECEX_SRC_ATV1:
            return SV_VS_ATV1;
            break;

		case	MTVDECEX_SRC_ATD1:
            return SV_VS_ATD1;
#endif
		case 	MTVDECEX_SRC_DT1:
				return SV_VS_DT1;
		case 	MTVDECEX_SRC_DT2:
				return SV_VS_DT2;
		case 	MTVDECEX_SRC_DTV1:
				return SV_VS_DTV1;
		case 	MTVDECEX_SRC_DTV2:
				return SV_VS_DTV2;
#ifdef CC_USE_DDI
		case 	MTVDECEX_SRC_DTV3:
				return SV_VS_DTV3;
		case 	MTVDECEX_SRC_DTV4:
				return SV_VS_DTV4;
#endif
		case	MTVDECEX_SRC_CVBS1:
				return SV_VS_CVBS1;
		case	MTVDECEX_SRC_CVBS2:
				return SV_VS_CVBS2;
		case	MTVDECEX_SRC_CVBS3:
				return SV_VS_CVBS3;
		case	MTVDECEX_SRC_CVBS4:
				return SV_VS_CVBS4;				
		case	MTVDECEX_SRC_SV1:
				return SV_VS_S1;
		case	MTVDECEX_SRC_SV2:
				return SV_VS_S2;
		case	MTVDECEX_SRC_SV3:
				return SV_VS_S3;
		case	MTVDECEX_SRC_COMP1:
				return SV_VS_YPbPr1;
		case	MTVDECEX_SRC_COMP2:
				return SV_VS_YPbPr2;
		case	MTVDECEX_SRC_COMP3:
				return SV_VS_YPbPr3;
		case	MTVDECEX_SRC_COMP4:
				return SV_VS_YPbPr4;
		case 	MTVDECEX_SRC_PC1:
				return SV_VS_VGA1;
		case	MTVDECEX_SRC_HDMI1:
				return SV_VS_HDMI1;
		case	MTVDECEX_SRC_HDMI2:
				return SV_VS_HDMI2;
		case	MTVDECEX_SRC_HDMI3:
				return SV_VS_HDMI3;
		case	MTVDECEX_SRC_HDMI4:
				return SV_VS_HDMI4;
		case	MTVDECEX_SRC_HDMI5:
				return SV_VS_HDMI5;				
		case	MTVDECEX_SRC_SCART1:
				return SV_VS_SCART1;
		case	MTVDECEX_SRC_SCART2:
				return SV_VS_SCART2;
		case	MTVDECEX_SRC_SCART3:
				return SV_VS_SCART3;
		case	MTVDECEX_SRC_SCART4:
				return SV_VS_SCART4;
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5882)
#ifdef CC_OSD_ENCODE
		case	MTVDECEX_SRC_OSDENCODE1:
		        return SV_VS_OSDENCODE1;
		case	MTVDECEX_SRC_OSTGENC:
		        return SV_VS_OSTGENC;
#endif
#endif
		default:
			return SV_VS_MAX;		
	};
};

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTVDECEX_IsPCTiming(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    BOOL *pfgPcTiming;
    BOOL fgPcTiming;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pfgPcTiming = (BOOL *)rArg.ai4Arg[1];
    
    fgPcTiming = (BOOL) fgApiVideoIsVgaTiming(ePath);

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgPcTiming,BOOL);
    COPY_TO_USER_ARG(pfgPcTiming,fgPcTiming,BOOL);

    MTVDECEX_PRINT(" - ePath = %d, fgPcTiming = %d\n",
		(int)ePath,
		(int)fgPcTiming);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_IsOversampleTiming(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    BOOL *pfgOversampleTiming;
    BOOL fgOversampleTiming;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pfgOversampleTiming = (BOOL *)rArg.ai4Arg[1];
    
    fgOversampleTiming = (BOOL) bDrvVideoGetWidthOverSample(ePath);

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgOversampleTiming,BOOL);
    COPY_TO_USER_ARG(pfgOversampleTiming,fgOversampleTiming,BOOL);

    MTVDECEX_PRINT(" - ePath = %d, fgOversampleTiming = %d\n",
		(int)ePath,
		(int)fgOversampleTiming);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetOversampleDependent(unsigned long arg)
{
    UINT32 u4Value;

    u4Value = (UINT32)arg;
    MTVDECEX_PRINT(" - u4Value = %d\n",
		(int)u4Value);

    _u4OversampleDependent = u4Value;

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetOversampleDependent(unsigned long arg)
{
    UINT32 *pu4Value;

    pu4Value = (UINT32 *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Value, UINT32);
    COPY_TO_USER_ARG(pu4Value, _u4OversampleDependent, UINT32);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetVideoSrc(unsigned long arg)
{
    UINT8 u1Ret;
    MTVDO_PATH_T ePath;
    MTVDECEX_SRC_T eSrc;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eSrc = (MTVDECEX_SRC_T)rArg.ai4Arg[1];
    
    MTVDECEX_PRINT(" - ePath = %d, eSrc = %d\n",
		(int)ePath,
		(int)eSrc);
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
	if(u1ApiW2dTxtureModeGet())
	{
		u1ApiW2dTxtureModeSet(0);
		if((eSrc >= MTVDECEX_SRC_HDMI1) && (eSrc <= MTVDECEX_SRC_HDMI4))
		{
			UINT8 hdmiPort = eSrc - MTVDECEX_SRC_HDMI1 + 1;
			vW2dSetVideoTextureControl(hdmiPort,TRUE);
		}
		return MTR_OK;
	}
#endif
/*
    if(i4VDOOmxPlaying((UINT8)ePath,(UINT8)eSrc))
    {
        return MTR_OK;
    }
    */
    if((eSrc == MTVDECEX_SRC_DTV1) ||
       (eSrc == MTVDECEX_SRC_DTV2) ||
       (eSrc == MTVDECEX_SRC_DTV3) ||
       (eSrc == MTVDECEX_SRC_DTV4))
    {   
        if(ePath==MTVDO_MAIN)
        {
            u1Ret= bApiVideoSetVideoSrc((UINT8)ePath, SV_VS_DTV1);
        }
        else if(ePath==MTVDO_SUB)
        {
            u1Ret= bApiVideoSetVideoSrc((UINT8)ePath, SV_VS_DTV2);
        }
    }
    else
    {
        u1Ret= bApiVideoSetVideoSrc((UINT8)ePath, _MTVDECEX_VdoSrcMap(eSrc));
    }
    
    if(_MTVDECEX_VdoSrcMap(eSrc) == SV_VS_DTV1)
    {
        MTVDECEX_PRINT("MTVDO(%d) VDP_SetInput EsId=0\n", ePath);
        VDP_SetInput((UCHAR)ePath, 0, 0);
#if defined(CC_B2R_RM_SUPPORT)
        VDEC_RmSetRenderVdp(0,ePath);
#endif
    }
    else if(_MTVDECEX_VdoSrcMap(eSrc) == SV_VS_DTV2)
    {
        MTVDECEX_PRINT("MTVDO(%d) VDP_SetInput EsId=1\n", ePath);
        VDP_SetInput((UCHAR)ePath, 1, 0);
#if defined(CC_B2R_RM_SUPPORT)
        VDEC_RmSetRenderVdp(1,ePath);
#endif
    }
	else
    {
        MTVDECEX_PRINT("MTVDO(%d) VDP_SetInput EsId=0xff\n", ePath);
        VDP_SetInput((UCHAR)ePath, 0xFF, 0);
    }

    if((u1Ret == SV_NO_CHANGE) ||(u1Ret == SV_SUCCESS))
    	return MTR_OK;
    else   	//if(u1Ret == SV_FAIL)
    	return MTR_NOT_OK;

}


static MT_RESULT_T _MTVDECEX_SetVFESrcPipeline(unsigned long arg)
{
	#ifdef CC_SUPPORT_PIPELINE
    UINT8 u1Ret;
    MTVDECEX_SRC_T eSrc;
	UINT32 u4Port;
	UINT32 u4Enable;
	UINT32 u4Type;

    MTAL_IOCTL_4ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	eSrc     = (MTVDECEX_SRC_T)rArg.ai4Arg[0];
	u4Port   = (UINT32)rArg.ai4Arg[1];
	u4Enable = (UINT32)rArg.ai4Arg[2];
	u4Type    = (UINT32)rArg.ai4Arg[3];
	
    
    MTVDECEX_PRINT(" - eSrc = %d, u4Port = %d u4Enable = %d u4Type = %d\n",
		(int)eSrc,
		(int)u4Port,
		(int)u4Enable,
		(int)u4Type);

    u1Ret = bApiVFEConnectVideoSrc(_MTVDECEX_VdoSrcMap(eSrc), u4Port, (UINT8)u4Enable,(UINT8)u4Type);
	return u1Ret;
	#endif

	return MTR_OK;
	
}

static MT_RESULT_T _MTVDECEX_SetVFEAVDConnect(unsigned long arg)
{
	#ifdef CC_SUPPORT_PIPELINE
    UINT8 u1Ret;
    MTVDECEX_SRC_T eSrc;
	UINT32 u4Port;
	UINT32 u4Enable;
	UINT32 u4Type;

    MTAL_IOCTL_4ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	eSrc     = (MTVDECEX_SRC_T)rArg.ai4Arg[0];
	u4Port   = (UINT32)rArg.ai4Arg[1];
	u4Enable = (UINT32)rArg.ai4Arg[2];
	u4Type    = (UINT32)rArg.ai4Arg[3];
	
    
    MTVDECEX_PRINT(" - eSrc = %d, u4Port = %d u4Enable = %d u4Type = %d\n",
		(int)eSrc,
		(int)u4Port,
		(int)u4Enable,
		(int)u4Type);

    u1Ret = bApiVFEAVDISConnect(_MTVDECEX_VdoSrcMap(eSrc), u4Port, (UINT8)u4Enable,(UINT8)u4Type);
	return u1Ret;
	#endif

	return MTR_OK;
	
}

static MT_RESULT_T _MTVDECEX_SetVSCSrcPipeline(unsigned long arg)
{
	#ifdef CC_SUPPORT_PIPELINE
    UINT8 u1Ret;
	MTVDO_PATH_T ePath;
    MTVDECEX_INPUT_TYPE_T eSrc;
	UINT32 u4Enable;
	UINT32 u4Type;

    MTAL_IOCTL_4ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	ePath    = (MTVDO_PATH_T)rArg.ai4Arg[0];
	eSrc     = (MTVDECEX_INPUT_TYPE_T)rArg.ai4Arg[1];
	u4Enable = (UINT32)rArg.ai4Arg[2];
	u4Type    = (UINT32)rArg.ai4Arg[3];
    
    MTVDECEX_PRINT(" - ePath = %d eSrc = %d, u4Enable = %d u4Type = %d \n",
		(int)ePath,
		(int)eSrc,
		(int)u4Enable,
		(int)u4Type);

	u1Ret = bApiVSCConnectVideoSrc((UINT8)ePath, eSrc,(UINT8)u4Enable,(UINT8)u4Type);

	#endif

	return MTR_OK;

}


static MT_RESULT_T _MTVDECEX_SetMonitorSrc(unsigned long arg)
{
    UINT8 u1Ret;
    MTVDO_PATH_T ePath;
    MTVDECEX_SRC_T eSrc;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eSrc = (MTVDECEX_SRC_T)rArg.ai4Arg[1];
    
    MTVDECEX_PRINT(" - ePath = %d, eSrc = %d\n",
		(int)ePath,
		(int)eSrc);
    
    u1Ret= bApiMonitorSetup((UINT8)ePath, _MTVDECEX_VdoSrcMap(eSrc));

    
    if(u1Ret == SV_SUCCESS)
    	return MTR_OK;
    else   	//if(u1Ret == SV_FAIL)
    	return MTR_NOT_OK;

}
static MT_RESULT_T _MTVDECEX_SetMainSubSrc(unsigned long arg)
{
    UINT8 u1Ret;
    MTVDECEX_SRC_T eMainSrc;
    MTVDECEX_SRC_T eSubSrc;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eMainSrc = (MTVDECEX_SRC_T )rArg.ai4Arg[0];
    eSubSrc = (MTVDECEX_SRC_T )rArg.ai4Arg[1];
    MTVDECEX_PRINT(" - eMainSrc = %d, eSubSrc = %d\n",
		(int)eMainSrc,
		(int)eSubSrc);

    u1Ret=bApiVideoMainSubSrc(_MTVDECEX_VdoSrcMap(eMainSrc), _MTVDECEX_VdoSrcMap(eSubSrc));

   // if(u1Ret == SV_NO_CHANGE)
   // 	return MTR_PARAMETER_NOCHANGE;
   // else if(u1Ret == SV_SUCCESS)
   if((u1Ret == SV_NO_CHANGE) ||(u1Ret == SV_SUCCESS))
    	return MTR_OK;
    else   	//if(u1Ret == SV_FAIL)
    	return MTR_NOT_OK;
}

static MT_RESULT_T _MTVDECEX_SetColorSystem(unsigned long arg)
{
    //UINT8 u1Ret;
    MTVDECEX_CS_T eColSys;
    eColSys = (MTVDECEX_CS_T)arg;

    MTVDECEX_PRINT(" - eColSys = %d\n",(int)eColSys);

    if( fgApiVideoColorSystem(eColSys) != SV_TRUE)
    {
    	return MTR_NOT_OK;
    }	
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoSrc(unsigned long arg)
{
/*
	MTVDO_PATH_T ePath;
    MTVDECEX_SRC_T eSrc;
    MTAL_IOCTL_2ARG_T    rArg;
    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    MTVDECEX_PRINT(" - GetSrcType ePath = %d\n",(int)ePath);
	eSrc=(MTVDECEX_SRC_T)bApiVideoGetSrcType((UINT8)ePath);
    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],MTVDECEX_SRC_T);
    COPY_TO_USER_ARG(rArg.ai4Arg[1],eSrc,MTVDECEX_SRC_T );
    return MTR_OK;
    */

   UINT8 u1SrcType,*pu1SrcType;
   
   MTAL_IOCTL_2ARG_T	 rArg;
   MTVDO_PATH_T ePath;
   
   USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
   COPY_FROM_USER_2ARG(arg, rArg);
   
   ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
   pu1SrcType = (UINT8 *)rArg.ai4Arg[1];
   
   u1SrcType = bApiVideoGetSrcType(ePath);
   
   MTVDECEX_PRINT(" - ePath = %d, u1SrcType = %d\n",
   	(int)ePath,
   	(int)u1SrcType);
   
   USR_SPACE_ACCESS_VALIDATE_ARG(pu1SrcType,UINT8);
   COPY_TO_USER_ARG(pu1SrcType,u1SrcType,UINT8);
   
   return MTR_OK;
   
}

static MT_RESULT_T _MTVDECEX_GetColorSystem(unsigned long arg)
{
    //MTVDO_PATH_T ePath;
    MTVDECEX_CS_T *peColSys;
    MTVDECEX_CS_T eColSys;

    peColSys =  (MTVDECEX_CS_T *)arg;
    
    eColSys = bApiVideoGetColorSystem();
    
    USR_SPACE_ACCESS_VALIDATE_ARG(peColSys,MTVDECEX_CS_T );
    COPY_TO_USER_ARG(peColSys,eColSys,MTVDECEX_CS_T );

    MTVDECEX_PRINT(" - eColSys = %d\n",(int)eColSys);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetPorch(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDECEX_PORCH_TYPE_T ePorchType;
    UINT16 u2Value;

    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    ePorchType = (MTVDECEX_PORCH_TYPE_T)rArg.ai4Arg[1];
    u2Value = (UINT16)rArg.ai4Arg[2];
    MTVDECEX_PRINT(" - ePath = %d, ePorchType = %d, u2Value = %d\n",
		(int)ePath,
		(int)ePorchType,
		(int)u2Value);

    if (_u4OversampleDependent == 0)
    {
        if (bDrvVideoGetWidthOverSample(ePath) && (ePorchType == MTVDECEX_HPORCH_CURRENT))
        {
            u2Value = u2Value<<1;
        }
    }

    wDrvVideoSetPorch(ePath, ePorchType,u2Value);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetPorch(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDECEX_PORCH_TYPE_T ePorchType;
    UINT16 *pu2Value;
    UINT16  u2Value;

    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    ePorchType = (MTVDECEX_PORCH_TYPE_T)rArg.ai4Arg[1];
    pu2Value = (UINT16 *)rArg.ai4Arg[2];

    u2Value = wDrvVideoGetPorch(ePath, ePorchType);

    MTVDECEX_PRINT(" - ePath = %d, ePorchType = %d, u2Value = %d\n",
		(int)ePath,
		(int)ePorchType,
		(int)u2Value);

    if (_u4OversampleDependent == 0)
    {
        if (bDrvVideoGetWidthOverSample(ePath) && 
        	((ePorchType == MTVDECEX_HPORCH_CURRENT) || (ePorchType == MTVDECEX_HPORCH_DEFAULT) || (ePorchType == MTVDECEX_HPORCH_MIN) || (ePorchType == MTVDECEX_HPORCH_MAX)))
        {
            u2Value = u2Value >> 1;
        }
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetPorchTune(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDECEX_PORCHTUNE_TYPE_T  eTuneType;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eTuneType = (MTVDECEX_PORCHTUNE_TYPE_T )rArg.ai4Arg[1];

    vDrvSetPorchTune(ePath, (UINT8)eTuneType);

    MTVDECEX_PRINT(" - ePath = %d, eTuneType= %d\n",
		(int)ePath,
		(int)eTuneType);
		
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetPorchTune(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDECEX_PORCHTUNE_TYPE_T *peTuneType;
    MTVDECEX_PORCHTUNE_TYPE_T  eTuneType;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peTuneType = (MTVDECEX_PORCHTUNE_TYPE_T *)rArg.ai4Arg[1];

    eTuneType = (MTVDECEX_PORCHTUNE_TYPE_T) bDrvGetPorchTune(ePath);

    MTVDECEX_PRINT(" - ePath = %d, eTuneType= %d\n",
		(int)ePath,
		(int)eTuneType);

    USR_SPACE_ACCESS_VALIDATE_ARG(peTuneType,MTVDECEX_PORCHTUNE_TYPE_T);
    COPY_TO_USER_ARG(peTuneType,eTuneType,MTVDECEX_PORCHTUNE_TYPE_T);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_StartVgaAuto(VOID)
{
       MTVDECEX_PRINT(" - void\n");
	if(fgApiVideoVgaAuto()==SV_FALSE)
	{
		return MTR_NOT_OK;
	}
	return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_StopVgaAuto(VOID)
{
       MTVDECEX_PRINT(" - void\n");
	vDrvVgaAutoStop();
	return MTR_OK;
}


static MT_RESULT_T _MTVDECEX_StartAutoColor(VOID)
{
       MTVDECEX_PRINT(" - void\n");
	if(fgApiVideoAutoColor()==SV_FALSE)
	{
		return MTR_NOT_OK;
	}
	return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_StartYPbPrAuto(VOID)
{
       MTVDECEX_PRINT(" - void\n");
	if(fgApiVideoYPbPrAuto()==SV_FALSE)
	{
		return MTR_NOT_OK;
	}
	return MTR_OK;
}


static MT_RESULT_T _MTVDECEX_SetPhase(unsigned long arg)
{
    UINT8 u1Value;
    u1Value = (UINT8)arg;
    MTVDECEX_PRINT(" - u1Value = %d\n",u1Value);

    if(fgApiVideoVgaSetPhase(u1Value)==SV_FALSE)
    {
    	return MTR_NOT_OK;
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetPhase(unsigned long arg)
{
    MTVDECEX_PHASE_TYPE_T ePhaseType;
    UINT8 *pu1Value;
    UINT8  u1Value;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePhaseType = (MTVDECEX_PHASE_TYPE_T )rArg.ai4Arg[0];
    pu1Value = (UINT8 *)rArg.ai4Arg[1];

    u1Value =u1DrvVGAGetPhase();//bDrvVGAGetPhase();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    MTVDECEX_PRINT(" - ePhaseType = %d, u1Value = %d\n",
		(int)ePhaseType,
		(int)u1Value);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetClock(unsigned long arg)
{
    UINT16 u2Value;
    u2Value = (UINT16)arg;
    MTVDECEX_PRINT(" - u2Value = %d\n",(int)u2Value);

    if(fgApiVideoVgaSetClock(u2Value)==SV_FALSE)
    {
    	return MTR_NOT_OK;
    }

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetClock(unsigned long arg)
{
    MTVDECEX_CLOCK_TYPE_T eClockType;
    UINT16 *pu2Value;
    UINT16  u2Value;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eClockType = (MTVDECEX_CLOCK_TYPE_T)rArg.ai4Arg[0];
    pu2Value = (UINT16 *)rArg.ai4Arg[1];

    if(eClockType == MTVDECEX_VGA_CLOCK_CURRENT)
    {
    	u2Value= wDrvVGAGetClock();
    }
    else if(eClockType == MTVDECEX_VGA_CLOCK_MAX)
    {
        //Tadd recommand
    	u2Value =wDrvVgaGetClockMax() + 127;
    }
    else //if(eClockType == MTVDECEX_VGA_CLOCK_MIN)
    {
    	u2Value =0;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    MTVDECEX_PRINT(" - eClockType = %d, u2Value = %d\n",
		(int)eClockType,
		(int)u2Value);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_SetAmbiguousUserSel(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT
	BOOL fgUserSel;

	fgUserSel = (BOOL)arg;

	MTVDECEX_PRINT(" - fgUserSel = %d\n",(int)fgUserSel);
	vVgaSetAmbiguousUserSel(fgUserSel);
#endif	

       return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_GetAmbiguousTimingNum(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT
    UINT8 *pu1AmbiguousTimingNum;
    UINT8  u1AmbiguousTimingNum;
    pu1AmbiguousTimingNum = (UINT8 *)arg;

    //add driver implementation code here
    u1AmbiguousTimingNum = bVgaGetAmbiguousTimingNum();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1AmbiguousTimingNum,UINT8);
    COPY_TO_USER_ARG(pu1AmbiguousTimingNum,u1AmbiguousTimingNum,UINT8);

    MTVDECEX_PRINT(" - u1AmbiguousTimingNum = %d\n",(int)u1AmbiguousTimingNum);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_QueryAmbiguousTiming(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT    
    UINT8 i, u1OptionIdx, bTimingNum;
    MTVDECEX_VGA_AMBIG_TIMING_T* prAmbigVgaTimingInfo;
    MTVDECEX_VGA_AMBIG_TIMING_T  rAmbigVgaTimingInfo;
    VGAAmbTimingInfo_t rTmpVgaTimingInfo;
    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1OptionIdx = (UINT8)rArg.ai4Arg[0];
    prAmbigVgaTimingInfo = (MTVDECEX_VGA_AMBIG_TIMING_T *)rArg.ai4Arg[1];
    bTimingNum = (UINT8)rArg.ai4Arg[2];

    //add driver implementation code here    
    MTVDECEX_PRINT(" - u1OptionIdx = %d, bTimingNum = %d\n",(int)u1OptionIdx, (int)bTimingNum);

    for(i=0; i<bTimingNum; i++)
    {
            vVgaQueryAmbiguousTiming(u1OptionIdx+i, &rTmpVgaTimingInfo, 1);
	    rAmbigVgaTimingInfo.u2Width =rTmpVgaTimingInfo.wHor_res;
	    rAmbigVgaTimingInfo.u2Height=rTmpVgaTimingInfo.wVer_res;
	    rAmbigVgaTimingInfo.u1RefreshRate= rTmpVgaTimingInfo.bVer_freq; 
	    USR_SPACE_ACCESS_VALIDATE_ARG(prAmbigVgaTimingInfo+i*sizeof(MTVDECEX_VGA_AMBIG_TIMING_T), MTVDECEX_VGA_AMBIG_TIMING_T);
    	    COPY_TO_USER_ARG(prAmbigVgaTimingInfo+i*sizeof(MTVDECEX_VGA_AMBIG_TIMING_T), rAmbigVgaTimingInfo, MTVDECEX_VGA_AMBIG_TIMING_T);
	    MTVDECEX_PRINT_AMBIG_TIMINGT(rAmbigVgaTimingInfo);
    }
#endif    


    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_SetAmbiguousIndex(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT    
    UINT8 u1OptionIdx;
    BOOL fgSetAsDefault;

    MTAL_IOCTL_2ARG_T    rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OptionIdx = rArg.ai4Arg[0];
    fgSetAsDefault = (BOOL) rArg.ai4Arg[1];
    
    MTVDECEX_PRINT(" - u1OptionIdx = %d, fgSetAsDefault = %d\n",(int)u1OptionIdx, (int)fgSetAsDefault);
    //add driver implementation code here
    vVgaSetAmbiguousTiming(u1OptionIdx, fgSetAsDefault);
#endif
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_GetCurrentAmbiguousIndex(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT    
    UINT8 *pu1CurrentAmbiguousIndex;
    UINT8  u1CurrentAmbiguousIndex;
    VGAAmbTimingInfo_t VGAInfo;
    pu1CurrentAmbiguousIndex= (UINT8 *)arg;

    //add driver implementation code here
    vVgaQueryAmbiguousTiming(QUERY_CUR_AMBIGUOUS_TIMING, &VGAInfo, 1);
    u1CurrentAmbiguousIndex = VGAInfo.bCandiIdx;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1CurrentAmbiguousIndex,UINT8);
    COPY_TO_USER_ARG(pu1CurrentAmbiguousIndex,u1CurrentAmbiguousIndex,UINT8);
    MTVDECEX_PRINT(" - u1CurrentAmbiguousIndex = %d\n",(int)u1CurrentAmbiguousIndex);
#endif    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_RegSigChgNfy( MTVDO_PATH_T ePath, MTVDECEX_SIG_STATUS eSigStatus, UINT8 u1SigSkip)
{
    static  UINT16 u2SigStatus[2][5] = {{0, 0xffff, 0xffff, 0xffff, 0xffff},//main
                                                                 {0, 0xffff, 0xffff, 0xffff, 0xffff}};    //sub
    static UINT8 u1SigSrc[2]={0xff, 0xff};

    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = eSigStatus;
    rVdoCbT.u4Arg3 = wApiVideoGetInputWidth(ePath);    
    rVdoCbT.u4Arg4 = wApiVideoGetInputHeight(ePath);
    rVdoCbT.u4Arg5 = (UINT16)bApiVideoGetRefreshRate(ePath);
    rVdoCbT.u4Arg6 = (UINT16)bDrvVideoIsSrcInterlace(ePath);

   // if the input signal status is no change, don't call back.
   if((u2SigStatus[ePath][0] != (UINT16)rVdoCbT.u4Arg2) ||
   	(u2SigStatus[ePath][1] != (UINT16)rVdoCbT.u4Arg3) ||
   	(u2SigStatus[ePath][2] != (UINT16)rVdoCbT.u4Arg4) ||
   	(u2SigStatus[ePath][3] != (UINT16)rVdoCbT.u4Arg5) ||
   	(u2SigStatus[ePath][4] != (UINT16)rVdoCbT.u4Arg6) ||
   	(u1SigSrc[ePath] != bGetVdoSrcNew(ePath)))
   {
    _CB_PutEvent(CB_MTAL_VDECEX_SIG_CHG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
   	 u2SigStatus[ePath][0] = (UINT16)rVdoCbT.u4Arg2;
   	 u2SigStatus[ePath][1] = (UINT16)rVdoCbT.u4Arg3;
   	 u2SigStatus[ePath][2] = (UINT16)rVdoCbT.u4Arg4;
   	 u2SigStatus[ePath][3] = (UINT16)rVdoCbT.u4Arg5;
   	 u2SigStatus[ePath][4] = (UINT16)rVdoCbT.u4Arg6;
   	 u1SigSrc[ePath] = bGetVdoSrcNew(ePath);   	 	
   }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGAAutoNfyPrg( MTVDO_PATH_T ePath, UINT8 u1Progress)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = u1Progress;
    
    _CB_PutEvent(CB_MTAL_VDECEX_VGA_AUTO_PRG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGAAutoNfy( MTVDO_PATH_T ePath, BOOL fgSuccess)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = fgSuccess;
    
    _CB_PutEvent(CB_MTAL_VDECEX_VGA_AUTO, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_AutoColorNfy( MTVDO_PATH_T ePath, UINT8 u1Success)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = u1Success;
    
    _CB_PutEvent(CB_MTAL_VDECEX_AUTO_COLOR, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_ResolutionChgNfy(MTVDO_PATH_T ePath, UINT8 u1SigState)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = u1SigState;
    
    _CB_PutEvent(CB_MTAL_VDECEX_RESOLUTION_CHG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_CompSigChgNfy(MTVDO_PATH_T ePath , UINT8 ui4_drv_status)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = ui4_drv_status;
    
    _CB_PutEvent(CB_MTAL_VDECEX_COMP_SIG_CHG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_ColorSystemNfy(MTVDO_PATH_T ePath, UINT8 u1SigState)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = u1SigState;
    
    _CB_PutEvent(CB_MTAL_VDECEX_COLOR_SYSTEM, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_MVChgNfy(MTVDO_PATH_T ePath, BOOL fgStatus)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = ePath;
    rVdoCbT.u4Arg2 = fgStatus;
    
    _CB_PutEvent(CB_MTAL_VDECEX_MV_CHG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

//Tuner analog tvd notify
static MT_RESULT_T _MTVDECEX_RegTunerAnaSigChgNfy(UINT8 u1SrcIndex, UINT8 u1SrcStatus)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u1SrcIndex;
    rVdoCbT.u4Arg2 = u1SrcStatus;
    
    _CB_PutEvent(CB_MTAL_VDECEX_ANA_SIG_CHG, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_AutoDetectSrcNfy(UINT8 u1SrcIndex, UINT8 u1SrcStatus)
{
    MTVDECEX_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u1SrcIndex;
    rVdoCbT.u4Arg2 = u1SrcStatus;
    
    _CB_PutEvent(CB_MTAL_VDECEX_AUTO_DETECT_SRC, sizeof(MTVDECEX_CB_T), (void*)&rVdoCbT);
    return MTR_OK;
}


static MT_RESULT_T _MTVDECEX_RegCbFunc(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDECEX_CB_FUNC_ENUM_T eFuncType;
    UINT32 u4FuncPtr;
    UINT32 u4Arg1;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eFuncType = (MTVDECEX_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    u4FuncPtr = (UINT32)rArg.ai4Arg[1];
    u4Arg1 = (UINT32)rArg.ai4Arg[2];

    MTVDECEX_PRINT(" -IO: eFuncType = %d, u4FuncPtr = %ul, u4Arg1 = %ul\n",
        eFuncType, u4FuncPtr, u4Arg1);

    switch (eFuncType)
    {
        //Tuner analog tvd notify
        case MTVDECEX_CB_FUNC_ANA_SIG_CHG:
            NPTV_SetTunerAnaSigChgNfyFct( (NPTV_PFN_TUNER_ANA_TVD_NFY)_MTVDECEX_RegTunerAnaSigChgNfy);
            break;
        case MTVDECEX_CB_FUNC_SIG_CHG:
            NPTV_SetSigChgNfyFct( (NPTV_PFN_SIG_NFY_FCT)_MTVDECEX_RegSigChgNfy);
            break;
        case MTVDECEX_CB_FUNC_VGA_AUTO_PRG:
             NPTV_SetVgaAutoNfyProgress( (NPTV_PFN_VGA_AUTO_NFY)_MTVDECEX_VGAAutoNfyPrg);
            break;
        case MTVDECEX_CB_FUNC_VGA_AUTO:
            NPTV_SetVgaAutoNfy( (NPTV_PFN_VGA_AUTO_NFY)_MTVDECEX_VGAAutoNfy);
            break;
        case MTVDECEX_CB_FUNC_AUTO_COLOR:
            NPTV_SetVgaAutocolorNfy( (NPTV_PFN_VGA_AUTOCOLOR_NFY)_MTVDECEX_AutoColorNfy);
            break;
        case MTVDECEX_CB_FUNC_RESOLUTION_CHG:
            NPTV_SetResolutionChgNfy( (NPTV_PFN_RESOLUTION_CHG_NFY)_MTVDECEX_ResolutionChgNfy);
            break;
        case MTVDECEX_CB_FUNC_COMP_SIG_CHG:
            NPTV_SetCompSigChgNfy( (NPTV_PFN_COMP_SIG_CHG_NFY)_MTVDECEX_CompSigChgNfy);
            break;
        case MTVDECEX_CB_FUNC_COLOR_SYSTEM:
            NPTV_SetTvdColorSystemNfy( (NPTV_PFN_COLOR_SYSTEM_NFY)_MTVDECEX_ColorSystemNfy);
            break;
        case MTVDECEX_CB_FUNC_MV_CHG:
            NPTV_SetTvdMVChgNfy( (NPTV_PFN_TVD_MVCHG_NFY)_MTVDECEX_MVChgNfy);
            break;
        case MTVDECEX_CB_FUNC_AUTO_DETECT_SRC:
            NPTV_SetAutoDetectSrcNfy( (NPTV_PFN_AUTO_DETECT_SRC_NFY)_MTVDECEX_AutoDetectSrcNfy);
			break;
        default:
            return MTR_NOT_OK;
            break;
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_SetHdcpKey(unsigned long arg)
{
    MTVDECEX_HDCP_KEY_T *prHdcpKey;
    MTVDECEX_HDCP_KEY_T rHdcpKey;/*kernel data*/
    UINT32 u4i = 0;
	UINT8 au1Buf[MTVDECEX_HDMI_HDCP_KEY_LEN] = {0};
    prHdcpKey = (MTVDECEX_HDCP_KEY_T *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prHdcpKey, MTVDECEX_HDCP_KEY_T);
    COPY_FROM_USER_ARG(prHdcpKey, rHdcpKey, MTVDECEX_HDCP_KEY_T);

    // write Hdcp key to driver
   for (u4i = 0 ; u4i < MTVDECEX_HDMI_HDCP_KEY_LEN ; u4i++)
   {
       if (u4i % 8 == 0) {Printf("\n");}
       Printf(" 0x%x", rHdcpKey.au1HdcpKey[u4i]);

       au1Buf[u4i] = (UINT8) rHdcpKey.au1HdcpKey[u4i];       
   }
    u1HDMI_SetHdcpKey((UINT8*)&au1Buf[0],(sizeof(au1Buf)/sizeof(UINT8)));
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_SetEdidData(unsigned long arg)
{
    MTVDECEX_HDMI_EDID_T *prHdmiEdidData;
    MTVDECEX_HDMI_EDID_T rHdmiEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 au1Buf[MTVDECEX_HDMI_EDID_LEN] = {0};
    prHdmiEdidData = (MTVDECEX_HDMI_EDID_T *)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prHdmiEdidData, MTVDECEX_HDMI_EDID_T);
    COPY_FROM_USER_ARG(prHdmiEdidData, rHdmiEdidData, MTVDECEX_HDMI_EDID_T);

    // write HDMI edid data to driver
    for (u4i = 0 ; u4i < MTVDECEX_HDMI_EDID_LEN ; u4i++)
   {
       if (u4i % 8 == 0) {Printf("\n");}
       Printf(" 0x%x", rHdmiEdidData.au1HdmiEdid[u4i]);
       
       au1Buf[u4i] = (UINT8) rHdmiEdidData.au1HdmiEdid[u4i];
   }
    
    u1HDMI_SetEdidData((UINT8*)&au1Buf[0], (sizeof(au1Buf)/sizeof(UINT8)));
    
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_SetEdidDataByPort(unsigned long arg)
{
#if defined(CC_MT5890)||defined(CC_MT5882)
	MTVDECEX_HDMI_EDID_T *prHdmiEdidData;
    MTVDECEX_HDMI_EDID_T rHdmiEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 u1Port = 1;
    UINT8 au1Buf[MTVDECEX_HDMI_EDID_LEN] = {0};
	
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Port = (UINT8)rArg.ai4Arg[0];
    prHdmiEdidData = (MTVDECEX_HDMI_EDID_T *)rArg.ai4Arg[1];

	
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prHdmiEdidData, MTVDECEX_HDMI_EDID_T);
    COPY_FROM_USER_ARG(prHdmiEdidData, rHdmiEdidData, MTVDECEX_HDMI_EDID_T);

    // write HDMI edid data to driver
    for (u4i = 0 ; u4i < MTVDECEX_HDMI_EDID_LEN ; u4i++)
   {
       //if (u4i % 8 == 0) {Printf("\n");}
       //Printf(" 0x%x", rHdmiEdidData.au1HdmiEdid[u4i]);
       
       au1Buf[u4i] = (UINT8) rHdmiEdidData.au1HdmiEdid[u4i];
   }	
   u1HDMI_SetEdidDataByPort(u1Port,(UINT8*)&au1Buf[0]);
 #endif   
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_SetEdidData(unsigned long arg)
{
    MTVDECEX_VGA_EDID_T *prVgaEdidData;
    MTVDECEX_VGA_EDID_T rVgaEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 au1Buf[MTVDECEX_VGA_EDID_LEN] = {0};    
    prVgaEdidData = (MTVDECEX_VGA_EDID_T *)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prVgaEdidData, MTVDECEX_VGA_EDID_T);
    COPY_FROM_USER_ARG(prVgaEdidData, rVgaEdidData, MTVDECEX_VGA_EDID_T);

    // write VGA edid data to driver
    for (u4i = 0 ; u4i < MTVDECEX_VGA_EDID_LEN ; u4i++)
   {
       if (u4i % 8 == 0) {Printf("\n");}
       Printf(" 0x%x", rVgaEdidData.au1VgaEdid[u4i]);

       au1Buf[u4i] = (UINT8) rVgaEdidData.au1VgaEdid[u4i];
   }
    vVGA_SETEDID((UINT8*)&au1Buf[0],(sizeof(au1Buf)/sizeof(UINT8)));
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetHdcpKey(unsigned long arg)
{
    MTVDECEX_HDCP_KEY_T *prHdcpKey;
    MTVDECEX_HDCP_KEY_T rHdcpKey;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 au1Buf[MTVDECEX_HDMI_HDCP_KEY_LEN] = {0};

   prHdcpKey = (MTVDECEX_HDCP_KEY_T *)arg;
   
    // read built-in edid data from driver
    u1HDMI_GetHdcpKey((UINT8*)&au1Buf[0], MTVDECEX_HDMI_HDCP_KEY_LEN);
    
    for (u4i = 0 ; u4i < MTVDECEX_HDMI_HDCP_KEY_LEN ; u4i++)
    {
        rHdcpKey.au1HdcpKey[u4i] = au1Buf[u4i];
       
        Printf("0x%2x", rHdcpKey.au1HdcpKey[u4i]);
        if (u4i % 16 == 0) {Printf("\n");}
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prHdcpKey, MTVDECEX_HDCP_KEY_T);
    COPY_TO_USER_ARG(prHdcpKey, rHdcpKey, MTVDECEX_HDCP_KEY_T);
    
    return MTR_OK;
}
    
static MT_RESULT_T _MTVDECEX_HDMI_GetEdidData(unsigned long arg)
{
    MTVDECEX_HDMI_EDID_T *prHdmiEdidData;
    MTVDECEX_HDMI_EDID_T rHdmiEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 u1Port = 1;
    UINT8 au1Buf[256] = {0};

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Port = (UINT8)rArg.ai4Arg[0];
    prHdmiEdidData = (MTVDECEX_HDMI_EDID_T *)rArg.ai4Arg[1];

    // read built-in edid data from driver
    u1HDMI_GetEdidData(u1Port, (UINT8*)&au1Buf[0], 256);
    
    for (u4i = 0 ; u4i < 256 ; u4i++)
    {
        rHdmiEdidData.au1HdmiEdid[u4i] = au1Buf[u4i];
       
        //Printf("0x%2x", rHdmiEdidData.au1HdmiEdid[u4i]);
        //if (u4i % 16 == 0) {Printf("\n");}
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prHdmiEdidData, MTVDECEX_HDMI_EDID_T);
    COPY_TO_USER_ARG(prHdmiEdidData, rHdmiEdidData, MTVDECEX_HDMI_EDID_T);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_GetEdidData(unsigned long arg)
{
    MTVDECEX_VGA_EDID_T *prVgaEdidData;
    MTVDECEX_VGA_EDID_T rVgaEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 au1Buf[256] = {0};

    prVgaEdidData = (MTVDECEX_VGA_EDID_T *)arg;

    // read built-in edid data from driver
    u1VGA_GetEdidData((UINT8*)&au1Buf[0], 256);
    
    for (u4i = 0 ; u4i < 256 ; u4i++)
    {
        rVgaEdidData.au1VgaEdid[u4i] = au1Buf[u4i];
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prVgaEdidData, MTVDECEX_VGA_EDID_T);
    COPY_TO_USER_ARG(prVgaEdidData, rVgaEdidData, MTVDECEX_VGA_EDID_T);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_EdidOnOffCtrl(unsigned long arg)
{

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    vHDMI_EdidOnOffCtrl((UINT8) rArg.ai4Arg[0], (UINT8) rArg.ai4Arg[1]);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_EdidOnOffCtrl(unsigned long arg)
{

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    vVga_EdidOnOffCtrl((UINT8) rArg.ai4Arg[0], (UINT8) rArg.ai4Arg[1]);
    
    return MTR_OK;
}


static MT_RESULT_T _MTVDECEX_HDMI_GetAspectRatio(unsigned long arg)
{
    MTVDECEX_HDMI_ARATIO_T* peRatio;
    MTVDECEX_HDMI_ARATIO_T  eRatio;
    peRatio = (MTVDECEX_HDMI_ARATIO_T* )arg;

    //add driver implementation code here
   if( bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_NOSIGNAL) {
       eRatio = MTVDECEX_HDMI_ARATIO_NODATA;
   }
   else {
    	 if(_bHdmiMode){
  	      switch (bHDMIAspectRatio()) {
  	          case 0: 
  	          	    eRatio = MTVDECEX_HDMI_ARATIO_NODATA;
  	                 break;
  	          case 1: 
  	          	    eRatio = MTVDECEX_HDMI_ARATIO_4_3;
  	                 break;
  	          case 2: 
  	          	    eRatio = MTVDECEX_HDMI_ARATIO_16_9;
  	                 break;
  	          case 3: 
  	          	    eRatio = MTVDECEX_HDMI_ARATIO_RESERVED;
  	                 break;
  	          default:
  	          	    eRatio = MTVDECEX_HDMI_ARATIO_NODATA;
  	                 break;    
  	      }
  	 }
    	 else {
   	     eRatio = MTVDECEX_HDMI_ARATIO_NODATA;
   	 }
   }	
    
    USR_SPACE_ACCESS_VALIDATE_ARG(peRatio, MTVDECEX_HDMI_ARATIO_T);
    COPY_TO_USER_ARG(peRatio, eRatio, MTVDECEX_HDMI_ARATIO_T);

    MTVDECEX_PRINT(" - eRatio = %d\n",(int)eRatio);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_SetHPDAdjust(unsigned long arg)
{
    UINT32 u4Value;
    u4Value = (UINT32)arg;
    MTVDECEX_PRINT(" - u4Value = %d\n",(int)u4Value);
    vHDMIHPDAdjust(u4Value);
    //add driver implementation code here
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetInfoFrame(unsigned long arg)
{
    MTVDECEX_HDMI_INFOFRAME* prFrame ;
    MTVDECEX_HDMI_INFOFRAME  rFrame ;
    MTVDECEX_HDMI_INFOFRAME* frame;
    UINT8 u1FrameType;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prFrame = (MTVDECEX_HDMI_INFOFRAME*)rArg.ai4Arg[0];
    u1FrameType = (UINT8)rArg.ai4Arg[1];

   frame=(MTVDECEX_HDMI_INFOFRAME *) API_HDMI_Get_InfoFrame(u1FrameType);
   rFrame=*frame;

    USR_SPACE_ACCESS_VALIDATE_ARG(prFrame, MTVDECEX_HDMI_INFOFRAME);
    COPY_TO_USER_ARG(prFrame, rFrame,MTVDECEX_HDMI_INFOFRAME);

//    MTVDECEX_PRINT(" - u1FrameType = %x\n",(int)u1FrameType);
//    MTVDECEX_PRINT_INFO_FRAME(rFrame);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetMode(unsigned long arg)
{
    MTVDECEX_HDMI_MODE_T* peMode;
    MTVDECEX_HDMI_MODE_T  eMode = MTVDECEX_HDMI_NO_SIGNAL;
    peMode = (MTVDECEX_HDMI_MODE_T* )arg;

    //add driver implementation code here
	if(bDviSigStatus() == SV_VDO_STABLE)
	{
		   if(_bHdmiMode)
   		   {
   				eMode =MTVDECEX_HDMI_HDMI;
   		   }
   		   else
   		   {
   				eMode =MTVDECEX_HDMI_DVI;
   		   }
    }
	else
	{
		eMode=MTVDECEX_HDMI_NO_SIGNAL;
	}
		

    USR_SPACE_ACCESS_VALIDATE_ARG(peMode,MTVDECEX_HDMI_MODE_T);
    COPY_TO_USER_ARG(peMode,eMode,MTVDECEX_HDMI_MODE_T);

    MTVDECEX_PRINT(" - eMode = %d\n",(int)eMode);

    return MTR_OK;
}

#ifdef CC_SOURCE_AUTO_DETECT
extern VOID vDrvGetSrcDetectStatus(_TV_DEC_SOURCE_STATUS_T* pv_get_info);
#endif

static MT_RESULT_T _MTVDECEX_GetSrcDetectStatus(unsigned long arg)
{
	MTVDECEX_TV_DEC_SOURCE_STATUS_T rStatus;
	MTVDECEX_TV_DEC_SOURCE_STATUS_T * prStatus;
	prStatus = (MTVDECEX_TV_DEC_SOURCE_STATUS_T *)arg;

#ifdef CC_SOURCE_AUTO_DETECT
    vDrvGetSrcDetectStatus((_TV_DEC_SOURCE_STATUS_T *)&rStatus);
#endif
	

	USR_SPACE_ACCESS_VALIDATE_ARG(prStatus, MTVDECEX_TV_DEC_SOURCE_STATUS_T);
    COPY_TO_USER_ARG(prStatus, rStatus, MTVDECEX_TV_DEC_SOURCE_STATUS_T);

	return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetDeepColorMode(unsigned long arg)
{
    MTVDECEX_HDMI_DEEPCOLOR_MODE_T* peDeepColorMode;
    MTVDECEX_HDMI_DEEPCOLOR_MODE_T  eDeepColorMode;
    peDeepColorMode = (MTVDECEX_HDMI_DEEPCOLOR_MODE_T* )arg;

    if(bHDMIDeepColorStatus()==0x1)
    {
    	eDeepColorMode=MTVDECEX_HDMI_DEEPCOLOR_10BIT;
    }
    else  if(bHDMIDeepColorStatus()==0x2)
    {
    	eDeepColorMode=MTVDECEX_HDMI_DEEPCOLOR_12BIT;
    }
    else  if(bHDMIDeepColorStatus()==0x3)
    {
    	eDeepColorMode=MTVDECEX_HDMI_DEEPCOLOR_16BIT;
    }
    else
    {
    	eDeepColorMode=MTVDECEX_HDMI_DEEPCOLOR_8BIT;
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(peDeepColorMode,MTVDECEX_HDMI_DEEPCOLOR_MODE_T);
    COPY_TO_USER_ARG(peDeepColorMode,eDeepColorMode,MTVDECEX_HDMI_DEEPCOLOR_MODE_T);

 //   MTVDECEX_PRINT(" - eDeepColorMode = %d\n",(int)eDeepColorMode);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_IsPCM(unsigned long arg)
{
    MTVDECEX_HDMI_AUDIOTYPE_T* pePCM;
    MTVDECEX_HDMI_AUDIOTYPE_T  ePCM;
    pePCM = (MTVDECEX_HDMI_AUDIOTYPE_T* )arg;

    if(bHDMI_IsPCM()==0x0)
    {
    	ePCM=MTVDECEX_HDMI_AUDIOTYPE_PCM;
    }
    else  
    {
    	ePCM=MTVDECEX_HDMI_AUDIOTYPE_NONEPCM;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pePCM,MTVDECEX_HDMI_AUDIOTYPE_T);
    COPY_TO_USER_ARG(pePCM,ePCM,MTVDECEX_HDMI_AUDIOTYPE_T);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetAudioFrequency(unsigned long arg)
{
    UINT8 AUDIOFreq;
    MTVDECEX_HDMI_AUDIOFREQUENCY_T* peAUDIOFreq;
    MTVDECEX_HDMI_AUDIOFREQUENCY_T  eAUDIOFreq;
    peAUDIOFreq = (MTVDECEX_HDMI_AUDIOFREQUENCY_T* )arg;
    //AUDIOFreq=bHDMIAUDIOSampleRateCal();
    //AUDIOFreq=RegReadFld(CHST1, AUD_SAMPLE_F);
    AUDIOFreq=u1HDMIGetAudioSamplingFreq();
    switch (AUDIOFreq)
    {
    	case AUD_FS_32K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_32K;
    	break;
    	case AUD_FS_44K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_44K;
    	break;
    	case AUD_FS_48K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_48K;
    	break;
    	case AUD_FS_88K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_88K;
    	break;
    	case AUD_FS_96K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_96K;
    	break;
    	case AUD_FS_176K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_176K;
    	break;
    	case AUD_FS_192K:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_192K;
    	break;
    	case AUD_FS_UNKNOWN:
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_NOTINDICATED;
    	break;
    	default :
    		eAUDIOFreq=	MTVDECEX_HDMI_AUDIOFREQUENCY_UNKNOWN;
    	break;
    }


    USR_SPACE_ACCESS_VALIDATE_ARG(peAUDIOFreq,MTVDECEX_HDMI_AUDIOFREQUENCY_T);
    COPY_TO_USER_ARG(peAUDIOFreq,eAUDIOFreq,MTVDECEX_HDMI_AUDIOFREQUENCY_T);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetAudioBitWidth(unsigned long arg)
{
    UINT8 BitWidth;
    MTVDECEX_HDMI_AUDIOLENGTH_T* peBitWidth;
    MTVDECEX_HDMI_AUDIOLENGTH_T  eBitWidth;
    peBitWidth = (MTVDECEX_HDMI_AUDIOLENGTH_T* )arg;
    BitWidth=bHDMIGetAudioBitWidth();
    switch (BitWidth)
    {
    	case HDMI_AUD_Length_16bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_16BITS;
    	break;
    	case HDMI_AUD_Length_17bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_17BITS;
    	break;
       case HDMI_AUD_Length_18bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_18BITS;
    	break;   
    	case HDMI_AUD_Length_19bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_19BITS;
    	break;
    	case HDMI_AUD_Length_20bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_20BITS;
    	break;
    	case HDMI_AUD_Length_21bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_21BITS;
    	break;
    	case HDMI_AUD_Length_22bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_22BITS;
    	break;
    	case HDMI_AUD_Length_23bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_23BITS;
    	break;
    	case HDMI_AUD_Length_24bits:
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_24BITS;
    	break;
    	default :
    		eBitWidth=MTVDECEX_HDMI_AUDIOLENGTH_UNKNOWN;
    	break;
    }


    USR_SPACE_ACCESS_VALIDATE_ARG(peBitWidth,MTVDECEX_HDMI_AUDIOLENGTH_T);
    COPY_TO_USER_ARG(peBitWidth,eBitWidth,MTVDECEX_HDMI_AUDIOLENGTH_T);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetAudioCategoryCode(unsigned long arg)
{
    UINT8 u1CategoryCode;
    BOOL  fgCopyright;
    UINT8 u1WordLength;
    BOOL  fgIsPcm;
    
    UINT8* pu1CategoryCode;
   
    pu1CategoryCode = (UINT8* )arg;

    vHDMIAudChannelStatus(&fgCopyright, &u1CategoryCode, 
                                &u1WordLength, &fgIsPcm);
                                
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1CategoryCode,UINT8);
    COPY_TO_USER_ARG(pu1CategoryCode,u1CategoryCode,UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_Init(unsigned long arg)
{
    UNUSED(arg);
	
    vHDMIInit();
    vDviInitial();   
	MTVDECEX_PRINT("HDMI_Init()\n");
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_FastInit(unsigned long arg)
{
    UNUSED(arg);

    vHDMIInit();
    vDviInitial();   
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_UnInit(unsigned long arg)
{
    UNUSED(arg);
	MTVDECEX_PRINT("HDMI_UnInit()\n");
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_Open(unsigned long arg)
{
    UNUSED(arg);
	MTVDECEX_PRINT("HDMI_Open()\n");
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_Close(unsigned long arg)
{
    UNUSED(arg);
	MTVDECEX_PRINT("HDMI_Close()\n");
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_Connect(unsigned long arg)
{
    UINT32 u4Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, u4Value);
	
    MTVDECEX_PRINT(" -HDMI_Connect port = %d\n",(UINT8)u4Value);
	vHDMISetSwitch((E_HDMI_SWITCH_NUM)u4Value);
	
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_DisConn(unsigned long arg)
{
    UNUSED(arg);
	//to do
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_EnableHPD(unsigned long arg)
{
    UINT32 eport;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, eport);

	
    MTVDECEX_PRINT(" ENABLE Port%d HPD\n= %d\n",(UINT8)eport);
    vHDMIHPDHigh((UINT8)eport);
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_DisableHPD(unsigned long arg)
{
    UINT32 eport;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, eport);

    MTVDECEX_PRINT(" DisABLE Port%d HPD\n= %d\n",(UINT8)eport);
    vHDMIHPDLow((UINT8)eport);
	
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_Get_SW5v(unsigned long arg)
{
	MTAL_IOCTL_2ARG_T rArg;
	UINT8 eport, u1value;
	UINT8 *pu1data;
	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	eport = (UINT8)rArg.ai4Arg[0];
	pu1data = (UINT8 *)rArg.ai4Arg[1];
	
	u1value = bHDMIPort5VStatus(eport);
	//u1value = 1;
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1data,UINT8);
    COPY_TO_USER_ARG(pu1data,u1value,UINT8);
    MTVDECEX_PRINT(" - eport = %d, u1Value = %d\n",
		(int)eport,
		(int)u1value);
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_Source_KSV(unsigned long arg)
{
#if defined(CC_MT5890)||defined(CC_MT5882)
	MTVDECEX_SOURCE_KSV_T *prKSV;
	MTVDECEX_SOURCE_KSV_T rKSV;
	MTAL_IOCTL_2ARG_T rArg;	
	UINT8 eport, u4i;
	UINT8 au1Buf[MTVDECEX_HDMI_SOURCE_KSV_LEN] = {0};
	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	eport = (UINT8)rArg.ai4Arg[0];
	prKSV = (MTVDECEX_SOURCE_KSV_T *)rArg.ai4Arg[1];
	
	u1HDMI_GetSource_KSV_Data(eport, (UINT8*)&au1Buf[0]);
	
	for (u4i = 0; u4i < MTVDECEX_HDMI_SOURCE_KSV_LEN ; u4i++)
	{
		 rKSV.au1HDMI_KSV[u4i] = au1Buf[u4i];		 
	}
	
    USR_SPACE_ACCESS_VALIDATE_ARG(prKSV,MTVDECEX_SOURCE_KSV_T);
    COPY_TO_USER_ARG(prKSV,rKSV,MTVDECEX_SOURCE_KSV_T);
#endif
    return MTR_OK;	
}
static MT_RESULT_T _MTVDEC_HDMI_MHL_IsMode(unsigned long arg)
{
#if defined(CC_MT5890)
#ifdef SYS_MHL_SUPPORT
	MTVDECEX_HDMI_MHL_MODE_T u1MhlDeviceConnected;
	MTVDECEX_HDMI_MHL_MODE_T *pu1MhlDeviceConnected;
	
	pu1MhlDeviceConnected = (MTVDECEX_HDMI_MHL_MODE_T*)arg;
	if(_IsMhlDevice())
		{
			u1MhlDeviceConnected = MTVDECEX_HDMI_MHL_DEVICE_CONNECTED;
			MTVDECEX_PRINT(" - MhlDeviceConnected -\n");
		}
	else
		{
			u1MhlDeviceConnected = MTVDECEX_HDMI_MHL_DEVICE_NOT_EXIST;
			MTVDECEX_PRINT(" - NO MhlDeviceConnected -\n");
		}
	USR_SPACE_ACCESS_VALIDATE_ARG(pu1MhlDeviceConnected,MTVDECEX_HDMI_MHL_MODE_T);
	COPY_TO_USER_ARG(pu1MhlDeviceConnected,u1MhlDeviceConnected,MTVDECEX_HDMI_MHL_MODE_T);
#endif
#endif
#if defined(CC_MT5882)
#ifdef SYS_MHL_SUPPORT
			MTVDECEX_HDMI_MHL_MODE_T u1MhlDeviceConnected;
			MTVDECEX_HDMI_MHL_MODE_T *pu1MhlDeviceConnected;
			
			pu1MhlDeviceConnected = (MTVDECEX_HDMI_MHL_MODE_T*)arg;
			if(_bIsMhlDevice)
				{
					u1MhlDeviceConnected = MTVDECEX_HDMI_MHL_DEVICE_CONNECTED;
					MTVDECEX_PRINT(" - MhlDeviceConnected -\n");
				}
			else
				{
					u1MhlDeviceConnected = MTVDECEX_HDMI_MHL_DEVICE_NOT_EXIST;
					MTVDECEX_PRINT(" - NO MhlDeviceConnected -\n");
				}
			USR_SPACE_ACCESS_VALIDATE_ARG(pu1MhlDeviceConnected,MTVDECEX_HDMI_MHL_MODE_T);
			COPY_TO_USER_ARG(pu1MhlDeviceConnected,u1MhlDeviceConnected,MTVDECEX_HDMI_MHL_MODE_T);
			
#endif
#endif
    return MTR_OK;	
}
static MT_RESULT_T _MTVDEC_HDMI_MHL_SendMSCMsg(unsigned long arg)
{
#if defined(CC_MT5890)||defined(CC_MT5882)
	UINT32 rcpKey;
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rcpKey);

    MTVDECEX_PRINT(" Send MHL Key\n= %d\n",(UINT8)rcpKey);
#ifdef SYS_MHL_SUPPORT
	bSendRcpKeys((UINT8)rcpKey,0);
#endif
#endif
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_HDMI_MHL_ReadWriteBurst(unsigned long arg)
{
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_MHL_ChkRcpRecive(unsigned long arg)
{

#if defined(CC_MT5882)
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = vIsRcpReceive();

    MTVDECEX_PRINT(" - Rcp Receive = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);
#endif
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_MHL_GetMhlRcpKey(unsigned long arg)
{
#if defined(CC_MT5882)
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = vRcpReceiveCode();

    MTVDECEX_PRINT(" - Rcp code value = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);
#endif
    return MTR_OK;

}	
static MT_RESULT_T _MTVDECEX_HDMI_W2d_SetVideoMode(unsigned long arg)
{
#ifdef CC_SUPPORT_W2DR2R
	MTVDECEX_HDMI_VIDEO_MODE_T rArg;
	//UINT8 hdmiPort;
	UINT8 enable;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg,MTVDECEX_HDMI_VIDEO_MODE_T);
    COPY_FROM_USER_ARG(arg, rArg,MTVDECEX_HDMI_VIDEO_MODE_T);
	//hdmiPort = rArg.u4HdmiPort;
	enable = rArg.enable;
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
	u1ApiW2dTxtureModeSet(enable);
#endif
	//vW2dSetVideoTextureControl(hdmiPort,enable);
#endif		
	return MTR_OK;	
}

static MT_RESULT_T _MTVDECEX_TVD_SetMAGC(unsigned long arg)
{
    UINT8 u1OnOff;
    u1OnOff = (UINT8)arg;
    MTVDECEX_PRINT(" - u1OnOff = 0x%x\n",u1OnOff);

    vTvd3dSetFixAgc(u1OnOff);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetAGCAnalogGain(unsigned long arg)
{
    MTVDEC_TVD_CHANNEL_T* eChannelSel;
    UINT8 u1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eChannelSel = (MTVDEC_TVD_CHANNEL_T*)rArg.ai4Arg[0];
    u1Value = (UINT8)rArg.ai4Arg[1];

    MTVDECEX_PRINT(" - eChannelSel = 0x%x\n"
    " - u1Value = 0x%x\n",
    eChannelSel,
    u1Value);

    u1Value =(u1Value>0x3f)?0x3f:u1Value; //limit 6 bits to meet PGA's spec
    if(eChannelSel == TVDADC_CHANNEL_A)
    {
        vDrvSetTVDADCGain(u1Value<<1);
    }
    else
    {
        vDrvSetTVDCHBGain(u1Value<<1);
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetAGCDigitalGain(unsigned long arg)
{
    UINT16 u2Value;
    u2Value = (UINT16)arg;
    MTVDECEX_PRINT(" - u2Value = 0x%x\n",u2Value);

    vDrvSetTVDADCDGain(u2Value);
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_TVD_SetClampLevel(unsigned long arg)
{
    MTVDEC_TVD_CHANNEL_T* eChannelSel;
    UINT16 u1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eChannelSel = (MTVDEC_TVD_CHANNEL_T*)rArg.ai4Arg[0];
    u1Value = (UINT16)rArg.ai4Arg[1];

    MTVDECEX_PRINT(" - eChannelSel = 0x%x\n"
    " - u1Value = 0x%x\n",
    eChannelSel,
    u1Value);

    if(eChannelSel == TVDADC_CHANNEL_A)
    {
    	if((u1Value>(256+12))||(u1Value<(256-12))) //range:10bit 256 +- 12 code
    		return  MTR_PARAMETER_ERROR; 
    	else
		vDrvDFE(u1Value);
    }
    else
    {
        return MTR_NOT_SUPPORTED;
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetAGCMode(unsigned long arg)
{
    MTVDEC_TVD_CHANNEL_T* eChannelSel;
    UINT8 u1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eChannelSel = (MTVDEC_TVD_CHANNEL_T*)rArg.ai4Arg[0];
    u1Value = (UINT8)rArg.ai4Arg[1];

    MTVDECEX_PRINT(" - eChannelSel = 0x%x\n"
    " - u1Value = 0x%x\n",
    eChannelSel,
    u1Value);

    if(eChannelSel == TVDADC_CHANNEL_A)
    {
    	vTvd3dSetFixAgc(u1Value==0);
    }
    else
    {
        	vDrvSetTVDCHBSrcMux((u1Value==0)?2:0);
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetCAGC(unsigned long arg)
{
    MTVDECEX_CAGC_PRM_T *prCAGCParam;
    MTVDECEX_CAGC_PRM_T rCAGCParam;/*kernel data*/
    prCAGCParam = (MTVDECEX_CAGC_PRM_T *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prCAGCParam, MTVDECEX_CAGC_PRM_T);
    COPY_FROM_USER_ARG(prCAGCParam, rCAGCParam, MTVDECEX_CAGC_PRM_T);

    MTVDO_PRINT(" rCAGCParam.u1MCAGC_EN = 0x%x\n"
        "rCAGCParam.u1MCAG = 0x%x\n"
        "rCAGCParam.u4Bound = 0x%lx\n"
        "rCAGCParam.u1L_Ste = 0x%x\n"
        "rCAGCParam.u1S_Step = 0x%x\n",
        rCAGCParam.u1MCAGC_EN,
        rCAGCParam.u1MCAGC,
        rCAGCParam.u4Bound,
        rCAGCParam.u1L_Step,
        rCAGCParam.u1S_Step);

    _rTvd3dCAGCInfo.MCAGC_EN = (UINT8) rCAGCParam.u1MCAGC_EN;
    _rTvd3dCAGCInfo.MCAGC = (UINT8) rCAGCParam.u1MCAGC;
    _rTvd3dCAGCInfo.Bound = (UINT32)rCAGCParam.u4Bound;
    _rTvd3dCAGCInfo.L_Step = (UINT8) rCAGCParam.u1L_Step;
    _rTvd3dCAGCInfo.S_Step = (UINT8) rCAGCParam.u1S_Step;
	
	vTvd3dSetCAGCbyAP();   
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetScanadjInfo(unsigned long arg)
{
	MTVDECEX_SCANADJ_INFO_T *prScanInfo;
	MTVDECEX_SCANADJ_INFO_T rScanInfo;
	prScanInfo = (MTVDECEX_SCANADJ_INFO_T *)arg;

	USR_SPACE_ACCESS_VALIDATE_ARG(prScanInfo, MTVDECEX_SCANADJ_INFO_T);
    COPY_FROM_USER_ARG(prScanInfo, rScanInfo, MTVDECEX_SCANADJ_INFO_T);

	MTVDECEX_PRINT("- vpres ongain(%d), offgain(%d)\n"
		"- vlock ratio(%d)\n",
		(rScanInfo.u1VpresGain>>4 & 0x0f),rScanInfo.u1VpresGain & 0x0f,rScanInfo.u1VlockRatio);
#if TVD_MANUAL_TUNER_SCAN
    vDrvTvdSetScanadjInfo(prScanInfo->u1VpresGain,prScanInfo->u1VlockRatio);
#endif
	return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_GetScanadjInfo(unsigned long arg)
{
	MTVDECEX_SCANADJ_INFO_T rScanInfo;
	MTVDECEX_SCANADJ_INFO_T * prScanInfo;
	prScanInfo = (MTVDECEX_SCANADJ_INFO_T *)arg;

    rScanInfo.u1VpresGain =0;
	rScanInfo.u1VlockRatio =0;
#if TVD_MANUAL_TUNER_SCAN
	vDrvTvdGetScanadjInfo(& rScanInfo.u1VpresGain,& rScanInfo.u1VlockRatio);
#endif

	USR_SPACE_ACCESS_VALIDATE_ARG(prScanInfo, MTVDECEX_SCANADJ_INFO_T);
    COPY_TO_USER_ARG(prScanInfo, rScanInfo, MTVDECEX_SCANADJ_INFO_T);

	MTVDECEX_PRINT("- vpres ongain(%d), offgain(%d)\n"
		"- vlock ratio(%d)\n",
		(rScanInfo.u1VpresGain>>4 & 0x0f),rScanInfo.u1VpresGain & 0x0f,rScanInfo.u1VlockRatio);

	return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetUVDelay(unsigned long arg)
{
    UINT8 u1YDelay;
    UINT8 u1UVDelay;

    MTAL_IOCTL_2ARG_T    rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1YDelay = (UINT8)rArg.ai4Arg[0];
    u1UVDelay = (UINT8) rArg.ai4Arg[1];
    if((u1YDelay > 3) || (u1UVDelay > 3))
    {
        MTVDECEX_PRINT(" u1YDelay or u1UVDelay error input value (> 3).\n");
        return MTR_NOT_OK;
    }

    MTVDECEX_PRINT(" - u1YDelay = 0x%x, u1UVDelay = 0x%x\n",(int)u1YDelay, (int)u1UVDelay);
    vTvd3dSetYCDelaybyAP(u1YDelay, u1UVDelay);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_GetStatus(unsigned long arg)
{
    MTVDECEX_TVD_STATUS_T eType;
    UINT8 *pu1Value;
    UINT8  u1Value;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eType = (MTVDECEX_TVD_STATUS_T )rArg.ai4Arg[0];
    pu1Value = (UINT8 *)rArg.ai4Arg[1];

    switch(eType)
    {
        case MTVDECEX_TVD_VPRES:
            u1Value = fgHwTvdVPres();
        break;
        case MTVDECEX_TVD_VLOCK:
            u1Value = fgHwTvdVLock();
        break;
        case MTVDECEX_TVD_HLOCK: 
            u1Value = fgHwTvdHLock();
        break;
        case MTVDECEX_TVD_LLOCK: 
            u1Value = fgHwTvdLLock();
        break;
        case MTVDECEX_TVD_NRLVL: 
            u1Value = bHwTvdNRLevel();
        break;
        case MTVDECEX_TVD_NASTA: 
            u1Value = bHwTvdNAState();
        break;
        case MTVDECEX_TVD_CHSCAN_LOCK:
            u1Value = fgDrvTvdCheckVPresOn();
        break;	
        case MTVDECEX_TVD_VIDEO_STABLE:
            u1Value = fgDrvTvdCheckVideoStable();
        break;	
		case MTVDECEX_TVD_VIDEO_MODEDONE:
			u1Value = fgDrvTvdCheckTVDModeDone();
        break;
        default:
            u1Value = 0;
        break;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    MTVDECEX_PRINT(" - eType = %d, u1Value = %d\n",
		(int)eType,
		(int)u1Value);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetAAF(unsigned long arg)
{
    UINT8 u1OnOff, u1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OnOff = (UINT8)rArg.ai4Arg[0];
    u1Value = (UINT8)rArg.ai4Arg[1];

    MTVDECEX_PRINT(" - u1OnOff = 0x%x\n"
    " - u1Value = 0x%x\n",
    u1OnOff,
    u1Value);
    if(u1Value > 7)
    {
        MTVDECEX_PRINT(" u1Value error input value (> 7).\n");
        return MTR_NOT_OK;
    }
    vTvd3dSetAAFbyAP(u1OnOff, u1Value);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetSFSetting(unsigned long arg)
{
    MTVDECEX_TVD_SECAM_T *prSecamParam;
    MTVDECEX_TVD_SECAM_T rSecamParam;/*kernel data*/
    prSecamParam = (MTVDECEX_TVD_SECAM_T *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prSecamParam, MTVDECEX_TVD_SECAM_T);
    COPY_FROM_USER_ARG(prSecamParam, rSecamParam, MTVDECEX_TVD_SECAM_T);

    vTvdSetSFSetting(rSecamParam.uSecam01.u4Secam01);
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_SetAutoSearch(unsigned long arg)
{
    UINT8 u1Value;
    u1Value = (UINT8)arg;
    MTVDECEX_PRINT(" - u1Value = %d\n",u1Value);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_StartCVBSAutoColor(VOID)
{
       MTVDECEX_PRINT(" - void\n");
	if(fgApiVideoCVBSAutoColor()==SV_FALSE)
	{
		return MTR_NOT_OK;
	}
	return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_HoldMode(unsigned long arg)
{
    UINT8 u1OnOff;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OnOff = (UINT8)rArg.ai4Arg[0];

    MTVDECEX_PRINT(" - u1OnOff = 0x%x\n", u1OnOff);
    
    vTvdHoldMode(u1OnOff);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_AGCPedEn(unsigned long arg)
{
    UINT8 u1OnOff;

    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OnOff = (UINT8)rArg.ai4Arg[0];

    MTVDECEX_PRINT(" - u1OnOff = 0x%x\n", u1OnOff);
    
    vTVD3dAGCPedEn(u1OnOff);
    return MTR_OK;
}

/*static MT_RESULT_T _MTVDECEX_HDMI_GetColorDomain(unsigned long arg)
{
	MTPQL_HDMI_COLOR_DOMAIN_T *pColorDomain;
	MTPQL_HDMI_COLOR_DOMAIN_T ColorDomain;
	pColorDomain=(MTPQL_HDMI_COLOR_DOMAIN_T*)arg;
	ColorDomain=bGetHDMIColorDomain(); 
	USR_SPACE_ACCESS_VALIDATE_ARG(pColorDomain,MTPQL_HDMI_COLOR_DOMAIN_T);
	COPY_TO_USER_ARG(pColorDomain,ColorDomain,MTPQL_HDMI_COLOR_DOMAIN_T);
	return MTR_OK;
}*/

static MT_RESULT_T _MTVDECEX_GetVideoDecType(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDO_DEC_TYPE_T *peType;
    MTVDO_DEC_TYPE_T  eType;
    UINT32 eDecType = SV_VD_NA;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peType = (MTVDO_DEC_TYPE_T *)rArg.ai4Arg[1];

    if(ePath == MTVDO_MAIN)
    {
    	eDecType = bGetVideoDecType(SV_VP_MAIN);
    }
    else if(ePath == MTVDO_SUB)
    {
    	eDecType = bGetVideoDecType(SV_VP_PIP);
    }
    else
    {
        return MTR_NOT_OK;
    }
    switch(eDecType)
    {
        case SV_VD_NA:
            eType = MTVDO_DEC_NA;
            break;
        case SV_VD_TVD3D:
            eType = MTVDO_DEC_TVD3D;
            break;
        case SV_VD_YPBPR:
            eType = MTVDO_DEC_YPBPR;
            break;
        case SV_VD_VGA:
            eType = MTVDO_DEC_VGA;
            break;
        case SV_VD_DVI:
            eType = MTVDO_DEC_DVI;
            break;
        case SV_VD_MPEGHD:
            eType = MTVDO_DEC_MPEGHD;
            break;
        case SV_VD_MPEGSD:
            eType = MTVDO_DEC_MPEGSD;
            break;
        default:
            eType = MTVDO_DEC_MAX;
            break;
    }


    USR_SPACE_ACCESS_VALIDATE_ARG(peType,MTVDO_DEC_TYPE_T);
    COPY_TO_USER_ARG(peType,eType,MTVDO_DEC_TYPE_T);

    MTVDECEX_PRINT(" - ePath = %d, eType = %d\n",
		(int)ePath,
		(int)eType);

    return MTR_OK;

}

static MT_RESULT_T _MTVDECEX_TVD_GetDefaultPorch(unsigned long arg)
{
    MTVDO_PATH_T ePath;
    MTVDECEX_PORCH_TYPE_T ePorchType;
    UINT16 *pu2Value;
    UINT16  u2Value;

    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    ePorchType = (MTVDECEX_PORCH_TYPE_T)rArg.ai4Arg[1];
    pu2Value = (UINT16 *)rArg.ai4Arg[2];

    u2Value = wTvdGetDefaultPorch(ePath, ePorchType);

    MTVDECEX_PRINT(" - ePath = %d, ePorchType = %d, u2Value = %d\n",
		(int)ePath,
		(int)ePorchType,
		(int)u2Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetChannelChange(unsigned long arg)
{
    UINT8 u1Value;
    u1Value = (UINT8)arg;
    MTVDECEX_PRINT(" - u1Value = %d\n",u1Value);
    vDrvTvdSetChannelChange(TRUE);
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetEnabledColorSystem(unsigned long arg)
{
    UINT32 u4DrvEnabledColSys=0,u4EnabledColSys=0;
    u4EnabledColSys = (MTVDECEX_CS_T)arg;

    MTVDECEX_PRINT(" - u4EnabledColSys = %d\n",(int)u4EnabledColSys);

    
    if(u4EnabledColSys & (1<<MTVDECEX_CS_PAL_N))
        u4DrvEnabledColSys |= (1<<SV_CS_PAL_N);
    
    if(u4EnabledColSys & (1<<MTVDECEX_CS_PAL))
        u4DrvEnabledColSys |= (1<<SV_CS_PAL);

    if(u4EnabledColSys & (1<<MTVDECEX_CS_PAL_M))
        u4DrvEnabledColSys |= (1<<SV_CS_PAL_M);
    
    if(u4EnabledColSys & (1<<MTVDECEX_CS_NTSC358))
        u4DrvEnabledColSys |= (1<<SV_CS_NTSC358);

    if(u4EnabledColSys & (1<<MTVDECEX_CS_SECAM))
        u4DrvEnabledColSys |= (1<<SV_CS_SECAM);

    if(u4EnabledColSys & (1<<MTVDECEX_CS_PAL_60))
        u4DrvEnabledColSys |= (1<<SV_CS_PAL_60); //PAL with pedestal

    if(u4EnabledColSys & (1<<MTVDECEX_CS_NTSC443))
        u4DrvEnabledColSys |= (1<<SV_CS_NTSC443); //PAL with pedestal

    if( vDrvTvd3dSetEnabledColorSystem(u4DrvEnabledColSys) != SV_TRUE)
    {
    	return MTR_NOT_OK;
    }	
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetVPresOnOffGain(unsigned long arg)
{
    UINT8 u1OffGain;
    UINT8 u1OnGain;
    //UINT8 u1OnOffGain;

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OffGain = (UINT8)rArg.ai4Arg[0];
    u1OnGain = (UINT8)rArg.ai4Arg[1];
    //u1OnOffGain = ((u1OnGain & 0x0F)<<4) | (u1OffGain & 0x0F);
    
    vDrvTvd3dSetVPresOnOffGain(u1OffGain, u1OnGain, DFE_NORMAL_ONOFF_THRD);

    MTVDECEX_PRINT(" - u1OffGain = %d, u1OnGain = %d\n",
		(int)u1OffGain,
		(int)u1OnGain);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetSyncDetForTuning(unsigned long arg)
{
    BOOL fgLowSens;
    fgLowSens = (BOOL)arg;
    
    vDrvTvd3dSetSyncDetForTuning(fgLowSens);
    MTVDECEX_PRINT("- fgLowSens = %d\n",fgLowSens);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_SetHPLL(unsigned long arg)
{
    UINT8 u1KPGain;
    UINT8 u1KIGain;
    UINT32 u4ColorSys;

    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1KPGain = (UINT8)rArg.ai4Arg[0];
    u1KIGain = (UINT8)rArg.ai4Arg[1];
    u4ColorSys = (UINT32)rArg.ai4Arg[2];

    vDrvTvd3dSetHPLL(u1KPGain, u1KIGain, u4ColorSys);

    MTVDECEX_PRINT(" - u1KPGain = %d, u1KIGain = %d, u4ColorSys = %d\n",
		(int)u1KPGain,
		(int)u1KIGain,
		(int)u4ColorSys);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoStatus(unsigned long arg)
{
    MTVDECEX_SIG_STATUS eSigStatus,*peSigStatus;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peSigStatus = (MTVDECEX_SIG_STATUS *)rArg.ai4Arg[1];

    eSigStatus = bDrvVideoSignalStatus(ePath);


    MTVDECEX_PRINT(" - ePath = %d, eSigStatus = %d\n",
		(int)ePath,
		(int)eSigStatus);

    USR_SPACE_ACCESS_VALIDATE_ARG(peSigStatus,MTVDECEX_SIG_STATUS);
    COPY_TO_USER_ARG(peSigStatus,eSigStatus,MTVDECEX_SIG_STATUS);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoSrcType(unsigned long arg)
{
    UINT8 u1SrcType,*pu1SrcType;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1SrcType = (UINT8 *)rArg.ai4Arg[1];

    u1SrcType = bApiVideoGetSrcType(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u1SrcType = %d\n",
		(int)ePath,
		(int)u1SrcType);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1SrcType,UINT8);
    COPY_TO_USER_ARG(pu1SrcType,u1SrcType,UINT8);

    return MTR_OK;
}

/**
 * Get video signal status
 *
 * @param eType query type
 * @param fgValue TRUE/FALSE
 * @retval MTR_OK                    Success.
 * @retval MTR_NOT_OK                Fail.
 */
MT_RESULT_T _MTVDECEX_GetVideoDefStatus(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_DEF_TYPE_T eType;
    UINT8* pfgValue;
    UINT8 fgValue;


    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eType = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pfgValue = (BOOL *)rArg.ai4Arg[1];

    switch(eType)
    {
        case MTVDO_IS_MAIN_DEC:
            fgValue = fgIsMainDec();
            break;    
        case  MTVDO_IS_PIP_DEC:
            fgValue = fgIsPipDec();
            break;
        case MTVDO_IS_MAIN_TVD:
            fgValue = fgIsMainTvd3d();
            break;
        case MTVDO_IS_PIP_TVD:
            fgValue = fgIsPipTvd3d();
            break;
        case MTVDO_IS_MAIN_VGA:
            fgValue = fgIsMainVga();
            break;
        case MTVDO_IS_PIP_VGA:
            fgValue = fgIsPipVga();
            break;
        case MTVDO_IS_MAIN_YPBPR:
            fgValue = fgIsMainYPbPr();
            break;
        case MTVDO_IS_PIP_YPBPR:
            fgValue = fgIsPipYPbPr();
            break;
        case MTVDO_IS_MAIN_DVI:
            fgValue = fgIsMainDVI();
            break;
        case MTVDO_IS_PIP_DVI:
            fgValue = fgIsPipDVI();
            break;
        case MTVDO_IS_MAIN_DTV:
            fgValue = fgIsMainDTV();
            break;
        case MTVDO_IS_PIP_DTV:
            fgValue = fgIsPipDTV();
            break;
        case MTVDO_IS_MAIN_SCART:
            fgValue = fgIsMainScart();
            break;
        case MTVDO_IS_PIP_SCART:
            fgValue = fgIsPipScart();
            break;        
        default:
            fgValue = FALSE;
            break;;
    }

    MTVDECEX_PRINT(" - eType = %d, fgValue = %d\n",
		(int)eType,
		(int)fgValue);

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgValue, UINT8);
    COPY_TO_USER_ARG(pfgValue, fgValue, UINT8);

    return MTR_OK;

}
#if 0
/**
 * Get video decoder type
 *
 * @param ePath MTVDO_MAIN/MTVDO_SUB
 * @param eDecType
 * @retval MTR_OK                    Success.
 * @retval MTR_NOT_OK                Fail.
 */
static MT_RESULT_T _MTVDECEX_GetVideoDecType(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;
    MTVDO_DEC_TYPE_T* peDecType;
    MTVDO_DEC_TYPE_T eDecType;


    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peDecType = (UINT32 *)rArg.ai4Arg[1];

    eDecType = bGetVideoDecType(ePath);

    MTVDECEX_PRINT(" - ePath = %d, eDecType = %d\n",
		(int)ePath,
		(int)eDecType);

    USR_SPACE_ACCESS_VALIDATE_ARG(peDecType,MTVDO_DEC_TYPE_T);
    COPY_TO_USER_ARG(peDecType,eDecType,MTVDO_DEC_TYPE_T);

    return MTR_OK;

}



static MT_RESULT_T _MTVDECEX_VGA_MWIFQueryAmbiguousTiming(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT    
    UINT8 i, u1OptionIdx, bTimingNum;
    MTVDECEX_VGA_AMBIG_TIMING_T* prAmbigVgaTimingInfo;
    MTVDECEX_VGA_AMBIG_TIMING_T  rAmbigVgaTimingInfo[3];
    VGAAmbTimingInfo_t rTmpVgaTimingInfo[3];
    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1OptionIdx = (UINT8)rArg.ai4Arg[0];
    prAmbigVgaTimingInfo = (MTVDECEX_VGA_AMBIG_TIMING_T *)rArg.ai4Arg[1];
    bTimingNum = (UINT8)rArg.ai4Arg[2];

    //add driver implementation code here
    vVgaQueryAmbiguousTiming(u1OptionIdx, rTmpVgaTimingInfo, bTimingNum);
    MTVDECEX_PRINT(" - u1OptionIdx = %d, bTimingNum = %d\n",(int)u1OptionIdx, (int)bTimingNum);

    for(i=0; i<bTimingNum; i++)
    {
           rAmbigVgaTimingInfo[i].u1CandiIdx=rTmpVgaTimingInfo[i].bCandiIdx;
	    rAmbigVgaTimingInfo[i].u2Width =rTmpVgaTimingInfo[i].wHor_res;
	    rAmbigVgaTimingInfo[i].u2Height=rTmpVgaTimingInfo[i].wVer_res;
	    rAmbigVgaTimingInfo[i].u1RefreshRate= rTmpVgaTimingInfo[i].bVer_freq; 
	    USR_SPACE_ACCESS_VALIDATE_ARG(prAmbigVgaTimingInfo+i*sizeof(MTVDECEX_VGA_AMBIG_TIMING_T), MTVDECEX_VGA_AMBIG_TIMING_T);
    	    COPY_TO_USER_ARG(prAmbigVgaTimingInfo+i*sizeof(MTVDECEX_VGA_AMBIG_TIMING_T), rAmbigVgaTimingInfo[i], MTVDECEX_VGA_AMBIG_TIMING_T);
	    MTVDECEX_PRINT_AMBIG_TIMINGT(rAmbigVgaTimingInfo[i]);
    }
#endif    

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_VGA_MWIFSetAmbiguousIndex(unsigned long arg)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT    
    UINT8 u1OptionIdx;
    BOOL fgSetAsDefault;

    MTAL_IOCTL_2ARG_T    rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OptionIdx = rArg.ai4Arg[0];
    fgSetAsDefault = (BOOL) rArg.ai4Arg[1];
    
    MTVDECEX_PRINT(" - u1OptionIdx = %d, fgSetAsDefault = %d\n",(int)u1OptionIdx, (int)fgSetAsDefault);
    //add driver implementation code here
    vVgaSetAmbiguousTiming(u1OptionIdx, fgSetAsDefault);
#endif
    
    return MTR_OK;
}
#endif

EXTERN MT_RESULT_T _MTVDECEX_VGA_MWIFResetAmbiguousTimingEEPROM(VOID)
{
#ifdef SUPPORT_VGA_UI_AMBIGUOUS_TIMING_SELECT  
       MTVDECEX_PRINT(" - void\n");

	if(vVgaResetAmbiguousTimingEEPROM()==SV_FALSE)
	{
		return MTR_NOT_OK;
	}
#endif
	return MTR_OK;
}


/**
 * Get video signal strength
 *
 * @param eType 
 * @param pu1Value
 * @retval MTR_OK                    Success.
 * @retval MTR_NOT_OK                Fail.
 */
MT_RESULT_T _MTVDECEX_TVD_GetSignalStrengthLevel(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Type;
    UINT8* pu1Level;
    UINT8 u1Level;


    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Type = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1Level = (UINT8 *)rArg.ai4Arg[1];

    u1Level = bTvdSignalStrengthLevel(u1Type);

    MTVDECEX_PRINT(" - u1Type = %d, u1Level = %d\n",
		(int)u1Type,
		(int)u1Level);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Level,UINT8);
    COPY_TO_USER_ARG(pu1Level,u1Level,UINT8);

    return MTR_OK;

}

static MT_RESULT_T _MTVDECEX_GetVideoRefreshRate(unsigned long arg)
{
    UINT16 u2Value,*pu2Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu2Value = (UINT16 *)rArg.ai4Arg[1];

    u2Value = bDrvVideoGetRefreshRate(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u2Value = %d\n",
		(int)ePath,
		(int)u2Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoInputWidth(unsigned long arg)
{
    UINT16 u2Value,*pu2Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu2Value = (UINT16 *)rArg.ai4Arg[1];

    u2Value = wApiVideoGetInputWidth(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u2Value = %d\n",
		(int)ePath,
		(int)u2Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoInputHeight(unsigned long arg)
{
    UINT16 u2Value,*pu2Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu2Value = (UINT16 *)rArg.ai4Arg[1];

    u2Value = wDrvVideoInputHeight(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u2Value = %d\n",
		(int)ePath,
		(int)u2Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value,UINT16);
    COPY_TO_USER_ARG(pu2Value,u2Value,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_IsVideoSrcInterlace(unsigned long arg)
{
    UINT8 u1Value,*pu1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1Value = (UINT8 *)rArg.ai4Arg[1];

    u1Value = bDrvVideoIsSrcInterlace(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u1Value = %d\n",
		(int)ePath,
		(int)u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    return MTR_OK;
}

#if 0
static MT_RESULT_T _MTVDECEX_TDTV_GetTag3D(unsigned long arg)
{
    MTTDTV_TAG3D_TYPE_T eTag3d,*peTag3d;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peTag3d = (UINT32 *)rArg.ai4Arg[1];

    eTag3d = u4DrvTDTVGetTag3D(ePath);

    MTVDECEX_PRINT(" - ePath = %d, eTag3d = %d\n",
		(int)ePath,
		(int)eTag3d);

    USR_SPACE_ACCESS_VALIDATE_ARG(peTag3d,MTTDTV_TAG3D_TYPE_T);
    COPY_TO_USER_ARG(peTag3d,eTag3d,MTTDTV_TAG3D_TYPE_T);

    return MTR_OK;
}
#endif
static MT_RESULT_T _MTVDECEX_IsMhlSignal(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value = 0;
    UINT8 *pu1Value;


    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);
    
    pu1Value = (UINT8 *)iArg;
#ifdef SYS_MHL_SUPPORT
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    u1Value = u1IsMhlSignal();
#endif
#endif

    MTVDECEX_PRINT(" - GetMHLSignalStatus = %d\n",u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_GetVideoInputPolarity(unsigned long arg)
{
    UINT8 u1Value,*pu1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1Value = (UINT8 *)rArg.ai4Arg[1];

    u1Value = bApiVideoGetInputPolarity(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u1Value = %d\n",
		(int)ePath,
		(int)u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    return MTR_OK;

}

static MT_RESULT_T _MTVDECEX_GetVideoInputTimingID(unsigned long arg)
{
    UINT8 u1Value,*pu1Value;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1Value = (UINT8 *)rArg.ai4Arg[1];

    u1Value = bApiVideoGetInputTimingID(ePath);

    MTVDECEX_PRINT(" - ePath = %d, u1Value = %d\n",
		(int)ePath,
		(int)u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value,u1Value,UINT8);

    return MTR_OK;

}

static MT_RESULT_T _MTVDECEX_GetVideoInternalDec(unsigned long arg)
{
    MTVDO_DEC_TYPE_T eDecType,*peDecType;

    MTAL_IOCTL_2ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    peDecType = (MTVDO_DEC_TYPE_T *)rArg.ai4Arg[1];

    eDecType = bGetInternalDec(ePath);

    MTVDECEX_PRINT(" - ePath = %d, eDecType = %d\n",
		(int)ePath,
		(int)eDecType);

    USR_SPACE_ACCESS_VALIDATE_ARG(peDecType,MTVDO_DEC_TYPE_T);
    COPY_TO_USER_ARG(peDecType,eDecType,MTVDO_DEC_TYPE_T);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_GetNRLevel(unsigned long arg)
{
    INT32  iArg;
    UINT16 u2Value;
    UINT16 *pu2Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu2Value = (UINT16 *)iArg;

    u2Value = bTvdNRLevel();

    MTVDECEX_PRINT(" - u2Value = %d\n",
		(int)u2Value);


    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Value, UINT16);
    COPY_TO_USER_ARG(pu2Value, u2Value, UINT16);

    MTVDECEX_PRINT(" - TVD_GetNRLevel = %d\n", u2Value);

    return MTR_OK;

}

static MT_RESULT_T _MTVDECEX_HDMI_GetHDCPStatus(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = bHDMIHDCPStatusGet();

    MTVDECEX_PRINT(" - GetHDCPStatus = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetScanInfo(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = bHDMIScanInfo();

    MTVDECEX_PRINT(" - GetScanInfo = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
static MT_RESULT_T _MTVDECEX_HDMI_GetUpSta(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = u1HdmiGetUPStatus();

    MTVDECEX_PRINT(" - GetUpStatus = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetUpStaChg(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = u1HdmiGetUPStaChg();

    MTVDECEX_PRINT(" - GetUpStaChg = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetSvSta(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = u1HdmiGetSVStatus();

    MTVDECEX_PRINT(" - GetSvStatus = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetSvStaChg(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = u1HdmiGetSVStaChg();

    MTVDECEX_PRINT(" - GetSvStaChg = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
#endif
#if defined(CC_MT5890)
static MT_RESULT_T _MTVDECEX_HDMI_Get4KpsSta(unsigned long arg)
{
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);

    pu1Value = (UINT8 *)arg;

    u1Value = u1HDMI_Get4KPStatus();

    MTVDECEX_PRINT(" - Get4KpsSta  = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
#endif
static MT_RESULT_T _MTVDECEX_HDMI_GetAFD(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = bHDMIAFD();

    MTVDECEX_PRINT(" - GetAFD = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetInputType(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = bHDMIInputType();

    MTVDECEX_PRINT(" - GetInputType = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_IsSupportAudio(unsigned long arg)
{
    INT32  iArg;
    UINT8 u1Value;
    UINT8 *pu1Value;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, iArg);

    pu1Value = (UINT8 *)iArg;

    u1Value = fgHDMISupportAudio();

    MTVDECEX_PRINT(" - GetInputType = %d\n", u1Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value, UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    return MTR_OK;
}

static INT32 _MTVDECEX_SetHdmiColorMode(UCHAR ucVdpId, const MTVDECEX_HDMI_COLOR_MODE_T * prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(MTVDECEX_HDMI_COLOR_MODE_T))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
    MTVDECEX_PRINT(" - set hdmi colormode u4VdpId %d= 0x%x\n",ucVdpId, *prData);
    vSetHDMIColorMode(ucVdpId, *prData);        
     return  MTR_OK;
}

static INT32 _MTVDECEX_GetHdmiColorMode(UCHAR ucVdpId,  MTVDECEX_HDMI_COLOR_MODE_T * prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(MTVDECEX_HDMI_COLOR_MODE_T))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
    *prData = u1GetHDMIColorMode();            
    MTVDECEX_PRINT(" - get hdmi colormode u4VdpId %d= 0x%x\n",ucVdpId, *prData);

     return  MTR_OK;
}
static INT32 _MTVDECEX_SetScposPorch(UCHAR ucVdpId, MTAL_IOCTL_2ARG_T * prData, UINT32 u4Size)
{
    if (u4Size != sizeof(MTAL_IOCTL_2ARG_T))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
    MTVDECEX_PRINT(" - _MTVDO_SetScposPorch 0x%x\n",(prData->ai4Arg[0]));  
    VDP_SetScposPorch(ucVdpId, prData->ai4Arg[0], prData->ai4Arg[1]);
     return  MTR_OK;
}

static MT_RESULT_T  _MTVDECEX_SetCmd(UCHAR ucVdpId, UINT32 u4VdoType, const void* pkvVdoSetInfo, UINT32 u4Size)
{
    MT_RESULT_T i4Ret = MTR_NOT_SUPPORTED;
    
    switch ((MTVDO_VDECEX_TYPE_T) u4VdoType)
    {
        case VDECEX_GET_SET_HDMI_COLOR_MODE:
        {
            i4Ret = _MTVDECEX_SetHdmiColorMode( ucVdpId, (MTVDECEX_HDMI_COLOR_MODE_T *) pkvVdoSetInfo,  u4Size);
        }    
            break;           
        case VDECEX_SET_SCPOS_PORCH:
        {
            i4Ret = _MTVDECEX_SetScposPorch(ucVdpId, (MTAL_IOCTL_2ARG_T *) pkvVdoSetInfo,  u4Size);
        }              
            break;              
        default:
            i4Ret = MTR_NOT_SUPPORTED;
            break;
    }
    
    return i4Ret;
}

static MT_RESULT_T  _MTVDECEX_GetCmd(UCHAR ucVdpId, UINT32 u4VdoType, void* pkvVdoGetInfo, UINT32 u4Size)
{
    MT_RESULT_T i4Ret = MTR_NOT_SUPPORTED;
    
    switch ((MTVDO_VDECEX_TYPE_T) u4VdoType)
    {
        case VDECEX_GET_SET_HDMI_COLOR_MODE:
        {
            i4Ret = _MTVDECEX_GetHdmiColorMode( ucVdpId, (MTVDECEX_HDMI_COLOR_MODE_T *) pkvVdoGetInfo,  u4Size);
        }         
            break;        
        default:
            i4Ret = MTR_NOT_SUPPORTED;
            break;
    }
    
    return i4Ret;
}

static MT_RESULT_T _MTVDECEX_Set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_VDECEX_TYPE_T eVdecexType;
    void* pkvVdecexSetInfo = NULL;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;
        
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    eVdecexType = (MTVDO_VDECEX_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvVdecexSetInfo = kmalloc(u4Size, GFP_KERNEL);

    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdecexSetInfo, u4Size);

    eRet = (MT_RESULT_T) _MTVDECEX_SetCmd(eVdpId, eVdecexType, pkvVdecexSetInfo, u4Size);

    kfree(pkvVdecexSetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDECEX_Get(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_VDECEX_TYPE_T eVdoType;
    void* pkvVdecexGetInfo;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    eVdoType = (MTVDO_VDECEX_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvVdecexGetInfo = kmalloc(u4Size, GFP_KERNEL);

    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdecexGetInfo, u4Size);

    eRet = _MTVDECEX_GetCmd(eVdpId, eVdoType, pkvVdecexGetInfo, u4Size);

    COPY_TO_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdecexGetInfo, u4Size);    

    kfree(pkvVdecexGetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDECEX_SetSignalBooster(unsigned long arg)
{
    UINT8 u1OnOff;

    MTAL_IOCTL_2ARG_T    rArg;
    x_signal_usr_nfy_fct      pf_signal_usr_nfy_fct = NULL;//singnal booster add by Hua 0609
        
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1OnOff = (UINT8)rArg.ai4Arg[0];

    MTVDECEX_PRINT(" - u1OnOff = 0x%x\n", u1OnOff);
    
    if (DRVCUST_InitQuery(eSetSignalUsrNotifyFunc, (UINT32 *)(void *)&(pf_signal_usr_nfy_fct))==0)
    {
        if (pf_signal_usr_nfy_fct != NULL)
        {
            pf_signal_usr_nfy_fct(u1OnOff);
        }//pf_irrc_usr_nfy_fct
        return MTR_OK;
    }
    else
    {
        return MTR_OK;
    }
}

static MT_RESULT_T _MTVDECEX_TVD_GetAnalogCopyProtect(unsigned long arg)
{
	UINT8 u1Psp =0, u1CStripe =0;
	UINT8 * pu1Psp, *pu1CStripe;

    MTAL_IOCTL_3ARG_T    rArg;
    MTVDO_PATH_T ePath;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ePath = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1Psp = (UINT8 *)rArg.ai4Arg[1];
	pu1CStripe =(UINT8 *)rArg.ai4Arg[2];

    // do it
    bApiVideoGetAnalogCopyProtect(ePath, & u1Psp, & u1CStripe);

    MTVDECEX_PRINT(" - ePath = %d, PSP = 0x%X, CStripe = 0x%X\n",
		(int)ePath,
		(int)u1Psp,
		(int)u1CStripe);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Psp,UINT8);
    COPY_TO_USER_ARG(pu1Psp,u1Psp,UINT8);
	USR_SPACE_ACCESS_VALIDATE_ARG(pu1CStripe,UINT8);
    COPY_TO_USER_ARG(pu1CStripe,u1CStripe,UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_TVD_GetPreWidthHeight(unsigned long arg)
{
    UINT16 u2Width, u2Height;
    UINT16 *pu2Width, *pu2Height;
    MTAL_IOCTL_2ARG_T rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    pu2Width = (UINT16 *)rArg.ai4Arg[0];
    pu2Height = (UINT16 *)rArg.ai4Arg[1];
    
    vPreTvdWidthHeight(&u2Width,&u2Height);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Width,UINT16);
    COPY_TO_USER_ARG(pu2Width,u2Width,UINT16);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu2Height,UINT16);
    COPY_TO_USER_ARG(pu2Height,u2Height,UINT16);

    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetMhlPortId(unsigned long arg)
{
    UINT8 u1Port = 0;
    vGetMhlPortId(&u1Port) ;	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_TO_USER_ARG(arg, u1Port, INT32);
    MTVDECEX_PRINT("mtal: get mhl port id: %d\n", u1Port);
    return MTR_OK;	
}
static MT_RESULT_T _MTVDECEX_HDMI_SetExternalEdidDataByPort(unsigned long arg)
{
	    MT_RESULT_T ret=MTR_OK;
#if defined(CC_MT5882)	
		MTVDECEX_HDMI_EDID_T *prHdmiEdidData;
		MTVDECEX_HDMI_EDID_T rHdmiEdidData;/*kernel data*/
		UINT32 u4i = 0;
		UINT8 u1Port = 1;
		UINT8 au1Buf[MTVDECEX_HDMI_EDID_LEN] = {0};
		
		MTAL_IOCTL_2ARG_T	 rArg;
	
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);
	
		u1Port = (UINT8)rArg.ai4Arg[0];
		prHdmiEdidData = (MTVDECEX_HDMI_EDID_T *)rArg.ai4Arg[1];
	
		
		
		USR_SPACE_ACCESS_VALIDATE_ARG(prHdmiEdidData, MTVDECEX_HDMI_EDID_T);
		COPY_FROM_USER_ARG(prHdmiEdidData, rHdmiEdidData, MTVDECEX_HDMI_EDID_T);
	
		// write HDMI edid data to driver
		for (u4i = 0 ; u4i < MTVDECEX_HDMI_EDID_LEN ; u4i++)
	   {
		   //if (u4i % 8 == 0) {Printf("\n");}
		   //Printf(" 0x%x", rHdmiEdidData.au1HdmiEdid[u4i]);
		   
		   au1Buf[u4i] = (UINT8) rHdmiEdidData.au1HdmiEdid[u4i];
	   }	
	   if(u1HDMI_SetExternalEdidDataByPort(u1Port,(UINT8*)&au1Buf[0]))
	   	  ret=MTR_OK;
	   else
	   	  ret=MTR_NOT_OK;
	   	  
 #endif   
		return ret;

}

static MT_RESULT_T _MTVDECEX_HDMI_GetExternalEdidDataByPort(unsigned long arg)
{
    MT_RESULT_T ret=MTR_OK;
#if defined(CC_MT5882)
    MTVDECEX_HDMI_EDID_T *prHdmiEdidData;
    MTVDECEX_HDMI_EDID_T rHdmiEdidData;/*kernel data*/
    UINT32 u4i = 0;
    UINT8 u1Port = 0;
    UINT8 au1Buf[256] = {0};

    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Port = (UINT8)rArg.ai4Arg[0];
    prHdmiEdidData = (MTVDECEX_HDMI_EDID_T *)rArg.ai4Arg[1];

    // read built-in edid data from driver
    if(u1HDMI_GetExternalEdidDataByPort(u1Port, (UINT8*)&au1Buf[0], 256))
	   ret=MTR_OK;
	else
	   ret=MTR_NOT_OK;
    
    for (u4i = 0 ; u4i < 256 ; u4i++)
    {
        rHdmiEdidData.au1HdmiEdid[u4i] = au1Buf[u4i];
       
        //Printf("0x%2x", rHdmiEdidData.au1HdmiEdid[u4i]);
        //if (u4i % 16 == 0) {Printf("\n");}
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prHdmiEdidData, MTVDECEX_HDMI_EDID_T);
    COPY_TO_USER_ARG(prHdmiEdidData, rHdmiEdidData, MTVDECEX_HDMI_EDID_T);
 #endif  
    return ret;

}

static MT_RESULT_T _MTVDECEX_HDMI_GetInfoFrameByPort(unsigned long arg)
{
#if defined(CC_MT5882)
    MTVDECEX_HDMI_INFOFRAME* prFrame ;
    MTVDECEX_HDMI_INFOFRAME  rFrame ;
    MTVDECEX_HDMI_INFOFRAME* frame;
    UINT8 u1FrameType;
    UINT8 u1PortNum;
	
    MTAL_IOCTL_3ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

	u1PortNum = (UINT8)rArg.ai4Arg[0];
    prFrame = (MTVDECEX_HDMI_INFOFRAME*)rArg.ai4Arg[1];
    u1FrameType = (UINT8)rArg.ai4Arg[2];

    frame=(MTVDECEX_HDMI_INFOFRAME *) API_HDMI_Get_InfoFrameByPort(u1PortNum,u1FrameType);
    rFrame=*frame;

    USR_SPACE_ACCESS_VALIDATE_ARG(prFrame, MTVDECEX_HDMI_INFOFRAME);
    COPY_TO_USER_ARG(prFrame, rFrame,MTVDECEX_HDMI_INFOFRAME);

//    MTVDECEX_PRINT(" - u1FrameType = %x\n",(int)u1FrameType);
//    MTVDECEX_PRINT_INFO_FRAME(rFrame);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetModeByPort(unsigned long arg)
{
#if defined(CC_MT5882)
	UINT8 u1Port = 0;
    MTVDECEX_HDMI_MODE_T* peMode;
    MTVDECEX_HDMI_MODE_T  eMode = MTVDECEX_HDMI_NO_SIGNAL;
	MTAL_IOCTL_2ARG_T	 rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Port = (UINT8)rArg.ai4Arg[0];
    peMode = (MTVDECEX_HDMI_MODE_T *)rArg.ai4Arg[1];

    //add driver implementation code here
	eMode = u1HDMI_GetModeByPort(u1Port);

    USR_SPACE_ACCESS_VALIDATE_ARG(peMode,MTVDECEX_HDMI_MODE_T);
    COPY_TO_USER_ARG(peMode,eMode,MTVDECEX_HDMI_MODE_T);

//  MTVDECEX_PRINT(" - eMode = %d\n",(int)eMode);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDECEX_HDMI_GetTimingInfoByPort(unsigned long arg)
{
#if defined(CC_MT5882)
	MTAL_IOCTL_2ARG_T rArg;
	UINT8 u1Port = 0;
	
	HDMI_TIMING_INFO_T *prTiming;
	HDMI_TIMING_INFO_T rTiming;
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);
	u1Port = (UINT8)rArg.ai4Arg[0];
	prTiming = (HDMI_TIMING_INFO_T*)rArg.ai4Arg[1];
	
	rTiming.hTotal= wHDMIHTotalByPort(u1Port);
	rTiming.vTotal = wHDMIVTotalByPort(u1Port);
	rTiming.vFreq = bHDMIRefreshRateByPort(u1Port)*10;
	rTiming.hPorch = 0;
	rTiming.vPorch = 0;
	rTiming.wActive= wHDMIResoWidthByPort(u1Port);
	rTiming.hActive= wHDMIResoHeightByPort(u1Port);
    //prTiming.u4Interlace = fgHDMIinterlacedByPort(u1Port);
	rTiming.scanType = (UINT16)bHDMIScanInfoByPort(u1Port);
	rTiming.full_3d_timing = (HDMI_3D_FORMAT_TYPE_T)HDMI_3D_Get_InfoFrameByPort(u1Port);
    USR_SPACE_ACCESS_VALIDATE_ARG(prTiming, HDMI_TIMING_INFO_T);
    COPY_TO_USER_ARG(prTiming, rTiming, HDMI_TIMING_INFO_T);
#endif	
	return MTR_OK;

}
static MT_RESULT_T _MTVDECEX_HDMI_EnableMHL(unsigned long arg)
{

#if defined(CC_MT5882)
    UINT32 enmhl;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, enmhl);

	if(enmhl)
	{
      vResumeMHL();
	}
	else
	{
      vStopMHL();
	}
#endif
	return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_MHL_ClearRcpState(unsigned long arg)
{
#if defined(CC_MT5882)
    UNUSED(arg);
	
    vClearRcpState();
	MTVDECEX_PRINT("Clear Rcp State\n");
#endif
    return MTR_OK;
}
static MT_RESULT_T _MTVDECEX_HDMI_GetHWActiveInfo(unsigned long arg)
{
#if defined(CC_MT5882)

	 HDMI_HWACTIVE_T *prHWActive;
	 HDMI_HWACTIVE_T rHWActive;
	 prHWActive =(HDMI_HWACTIVE_T *)arg;
	 rHWActive.u4Height = wHDMIResoHeight();
	 rHWActive.u4Width = wHDMIResoWidth();
	 rHWActive.u4Interlace = fgHDMIinterlaced();
	 if(rHWActive.u4Interlace)
	 {
        rHWActive.u4Height = rHWActive.u4Height*2;
	 }
	 if(fgHDMIIsPixelRepeate())
	 {
		  rHWActive.u4Width = rHWActive.u4Width/2;
	 }
	 USR_SPACE_ACCESS_VALIDATE_ARG(prHWActive, HDMI_HWACTIVE_T);
	 COPY_TO_USER_ARG(prHWActive, rHWActive, HDMI_HWACTIVE_T);

#endif	
	return MTR_OK;

}

int mtal_ioctl_mtvdecex(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

	switch (cmd)
	{
        case MTAL_IO_VDECEX_ISPCTIMING:
        	i4Ret = _MTVDECEX_IsPCTiming(arg);
            break;
        case MTAL_IO_VDECEX_ISOVERSAMPLETIMING:
        	i4Ret = _MTVDECEX_IsOversampleTiming(arg);
            break;
        case MTAL_IO_VDECEX_SETOVERSAMPLEDEPENDENT:
        	i4Ret = _MTVDECEX_SetOversampleDependent(arg);
            break;
        case MTAL_IO_VDECEX_GETOVERSAMPLEDEPENDENT:
        	i4Ret = _MTVDECEX_GetOversampleDependent(arg);
            break;
        case MTAL_IO_VDECEX_SETVIDEOSRC:
        	i4Ret = _MTVDECEX_SetVideoSrc(arg);
            break;
	    case MTAL_IO_VDECEX_SETVFESRC_PIPELINE:
        	i4Ret = _MTVDECEX_SetVFESrcPipeline(arg);
            break;
		case MTAL_IO_VDECEX_VFEAVDConnect:
			i4Ret = _MTVDECEX_SetVFEAVDConnect(arg);
			break;
	    case MTAL_IO_VDECEX_SETVSCSRC_PIPELINE:
        	i4Ret = _MTVDECEX_SetVSCSrcPipeline(arg);
            break;
		case MTAL_IO_VDECEX_SETMONITORSRC:
        	i4Ret = _MTVDECEX_SetMonitorSrc(arg);
            break;

        case MTAL_IO_VDECEX_SETMAINSUBSRC:
        	i4Ret = _MTVDECEX_SetMainSubSrc(arg);
            break;
        case MTAL_IO_VDECEX_SETCOLORSYSTEM:
        	i4Ret = _MTVDECEX_SetColorSystem(arg);
            break;
        case MTAL_IO_VDECEX_GETCOLORSYSTEM:
        	i4Ret = _MTVDECEX_GetColorSystem(arg);
            break;
        case MTAL_IO_VDECEX_SETPORCH:
        	i4Ret = _MTVDECEX_SetPorch(arg);
            break;
        case MTAL_IO_VDECEX_GETPORCH:
        	i4Ret = _MTVDECEX_GetPorch(arg);
            break;
        case MTAL_IO_VDECEX_GETPORCHTUNE:
        	i4Ret = _MTVDECEX_GetPorchTune(arg);
            break;
        case MTAL_IO_VDECEX_STARTVGAAUTO:
        	i4Ret = _MTVDECEX_StartVgaAuto();
            break;
        case MTAL_IO_VDECEX_STOPVGAAUTO:
        	i4Ret = _MTVDECEX_StopVgaAuto();
            break;

        case MTAL_IO_VDECEX_STARTAUTOCOLOR:
        	i4Ret = _MTVDECEX_StartAutoColor();
            break;
        case MTAL_IO_VDECEX_SETPHASE:
        	i4Ret = _MTVDECEX_SetPhase(arg);
            break;
        case MTAL_IO_VDECEX_GETPHASE:
        	i4Ret = _MTVDECEX_GetPhase(arg);
            break;
        case MTAL_IO_VDECEX_SETCLOCK:
        	i4Ret = _MTVDECEX_SetClock(arg);
            break;
        case MTAL_IO_VDECEX_GETCLOCK:
        	i4Ret = _MTVDECEX_GetClock(arg);
            break;
        //case MTAL_IO_VDECEX_VGA_MWIFSETAMBIGUOUSUSRSEL:
        case MTAL_IO_VDECEX_VGA_SETAMBIGUOUSUSRSEL:
        	i4Ret = _MTVDECEX_VGA_SetAmbiguousUserSel(arg);
            break;
        //case MTAL_IO_VDECEX_VGA_MWIFGETAMBIGUOUSTIMINGNUM:
        case MTAL_IO_VDECEX_VGA_GETAMBIGUOUSTIMINGNUM:
        	i4Ret = _MTVDECEX_VGA_GetAmbiguousTimingNum(arg);
            break;
        case MTAL_IO_VDECEX_VGA_QUERYAMBIGUOUSTIMING:
        	i4Ret = _MTVDECEX_VGA_QueryAmbiguousTiming(arg);
            break;
        case MTAL_IO_VDECEX_VGA_SETAMBIGUOUSINDEX:
        	i4Ret = _MTVDECEX_VGA_SetAmbiguousIndex(arg);
            break;
        case MTAL_IO_VDECEX_VGA_GETCURRENTAMBIGUOUSINDEX:
        	i4Ret = _MTVDECEX_VGA_GetCurrentAmbiguousIndex(arg);
            break;
        case MTAL_IO_VDECEX_REGCB:
        	i4Ret = _MTVDECEX_RegCbFunc(arg);
            break;
		case MTAL_IO_VDECEX_HDMI_SET_HDCP_KEY:
			i4Ret = _MTVDECEX_HDMI_SetHdcpKey(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_SET_EDID_DATA:
			i4Ret = _MTVDECEX_HDMI_SetEdidData(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_SET_EDID_DATA_BY_PORT:
			i4Ret = _MTVDECEX_HDMI_SetEdidDataByPort(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_EDID_ONOFF_CTRL:
			i4Ret = _MTVDECEX_HDMI_EdidOnOffCtrl(arg);
			break;
		case MTAL_IO_VDECEX_VGA_SET_EDID_DATA:
			i4Ret = _MTVDECEX_VGA_SetEdidData(arg);
			break;			  
		case MTAL_IO_VDECEX_VGA_EDID_ONOFF_CTRL:
			i4Ret = _MTVDECEX_VGA_EdidOnOffCtrl(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_GET_HDCP_KEY:
			i4Ret = _MTVDECEX_HDMI_GetHdcpKey(arg); 
			break;
		case MTAL_IO_VDECEX_HDMI_GET_EDID_DATA:
			i4Ret = _MTVDECEX_HDMI_GetEdidData(arg); 
			break;
		case MTAL_IO_VDECEX_VGA_GET_EDID_DATA:
			i4Ret = _MTVDECEX_VGA_GetEdidData(arg); 
			break;
        case MTAL_IO_VDECEX_HDMI_GETASPECTRATIO:
        	i4Ret = _MTVDECEX_HDMI_GetAspectRatio(arg);
            break;            
        case MTAL_IO_VDECEX_HDMI_SETHPDADJUST:
        	i4Ret = _MTVDECEX_HDMI_SetHPDAdjust(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GETINFOFRAME:
        	i4Ret = _MTVDECEX_HDMI_GetInfoFrame(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GETMODE:
        	i4Ret = _MTVDECEX_HDMI_GetMode(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GETDEEPCOLORMODE:
        	 i4Ret = _MTVDECEX_HDMI_GetDeepColorMode(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_ISPCM:
        	 i4Ret = _MTVDECEX_HDMI_IsPCM(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GETAUDIOFREQUENCY:
        	 i4Ret = _MTVDECEX_HDMI_GetAudioFrequency(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GETDAUDIOBITWIDTH:
        	 i4Ret = _MTVDECEX_HDMI_GetAudioBitWidth(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_INIT:
            i4Ret = _MTVDECEX_HDMI_Init(arg);
			break;
        case MTAL_IO_VDECEX_HDMI_FASTINIT:
            i4Ret = _MTVDECEX_HDMI_FastInit(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_UnINIT:
            i4Ret = _MTVDECEX_HDMI_UnInit(arg);			
			break;
		case MTAL_IO_VDECEX_HDMI_Open:
            i4Ret = _MTVDECEX_HDMI_Open(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_Close:
            i4Ret = _MTVDECEX_HDMI_Close(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_Connect:
            i4Ret = _MTVDECEX_HDMI_Connect(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_DisConn:
            i4Ret = _MTVDECEX_HDMI_DisConn(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_EnableHPD:
            i4Ret = _MTVDECEX_HDMI_EnableHPD(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_DisableHPD:
            i4Ret = _MTVDECEX_HDMI_DisableHPD(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_Get_SW5v:
            i4Ret = _MTVDECEX_HDMI_Get_SW5v(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_Get_Source_KSV:
            i4Ret = _MTVDECEX_HDMI_Source_KSV(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_MHL_ISMODE:
			i4Ret = _MTVDEC_HDMI_MHL_IsMode(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_MHL_SENDMSCMSG:
			i4Ret = _MTVDEC_HDMI_MHL_SendMSCMsg(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_MHL_READWRITEBURST:
			i4Ret = _MTVDEC_HDMI_MHL_ReadWriteBurst(arg);
		case MTAL_IO_VDECEX_HDMI_MHL_CHKRCPRECEIVE:
			i4Ret = _MTVDECEX_HDMI_MHL_ChkRcpRecive(arg);
			break;
		case MTAL_IO_VDECEX_HDMI_MHL_GETMHLRCPKEY:
			i4Ret = _MTVDECEX_HDMI_MHL_GetMhlRcpKey(arg);
            break; 
		case MTAL_IO_VDECEX_HDMI_W2D_SETVIDEOMODE:
			i4Ret = _MTVDECEX_HDMI_W2d_SetVideoMode(arg);
			break;
        case MTAL_IO_VDECEX_TVD_SETMAGC:
        	 i4Ret = _MTVDECEX_TVD_SetMAGC(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SET2CHPGAGAIN:
        	 i4Ret = _MTVDECEX_TVD_SetAGCAnalogGain(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETAGCDIGITALGAIN:
        	 i4Ret = _MTVDECEX_TVD_SetAGCDigitalGain(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETCAGC:
        	 i4Ret = _MTVDECEX_TVD_SetCAGC(arg);
			break;
	    case MTAL_IO_VDECEX_TVD_SET_SCANADJ_INFO:
			 i4Ret = _MTVDECEX_TVD_SetScanadjInfo(arg);
            break;
		case MTAL_IO_VDECEX_TVD_GET_SCANADJ_INFO:
			 i4Ret = _MTVDECEX_TVD_GetScanadjInfo(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETUVDELAY:
        	 i4Ret = _MTVDECEX_TVD_SetUVDelay(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SET2CHCLAMPLEVEL:
             i4Ret = _MTVDECEX_TVD_SetClampLevel(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SET2CHAGCMODE:
             i4Ret = _MTVDECEX_TVD_SetAGCMode(arg);
            break;
         case MTAL_IO_VDECEX_TVD_GETSTATUS:
             i4Ret = _MTVDECEX_TVD_GetStatus(arg);
            break;
         // 2012.07.27 huiyu yang
		 case MTAL_IO_VDECEX_TVD_GET_ANALOG_COPY_PROTECT:
		    i4Ret = _MTVDECEX_TVD_GetAnalogCopyProtect(arg);
		    break;
         case MTAL_IO_VDECEX_TVD_GET_PRE_WIDTH_HEIGHT:
            i4Ret = _MTVDECEX_TVD_GetPreWidthHeight(arg);
            break;
         case MTAL_IO_VDECEX_TVD_SETAAF:
             i4Ret = _MTVDECEX_TVD_SetAAF(arg);
            break;
         case MTAL_IO_VDECEX_TVD_SETSECAMSETTING:
             i4Ret = _MTVDECEX_TVD_SetSFSetting(arg);
            break;     
        case MTAL_IO_VDECEX_SETAUTOSEARCH:
             i4Ret = _MTVDECEX_SetAutoSearch(arg);
            break;            
        case MTAL_IO_VDECEX_STARTCVBSAUTOCOLOR:
        	i4Ret = _MTVDECEX_StartCVBSAutoColor();
            break;
        case MTAL_IO_VDECEX_TVD_HOLDMODE:
             i4Ret = _MTVDECEX_TVD_HoldMode(arg);
             break;
        case MTAL_IO_VDECEX_HDMI_GETDAUDIOCATEGORYCODE:
        	 i4Ret = _MTVDECEX_HDMI_GetAudioCategoryCode(arg);
            break;

        case MTAL_IO_VDECEX_TVD_PEDESTAL_EN:
            i4Ret = _MTVDECEX_TVD_AGCPedEn(arg);
            break;
        case MTAL_IO_VDECEX_GETVIDEODECODERTYPE:
            i4Ret = _MTVDECEX_GetVideoDecType(arg);
            break;
        case MTAL_IO_VDECEX_TVD_GETDEFAULTPORCH:
        	i4Ret = _MTVDECEX_TVD_GetDefaultPorch(arg);
        	break;
        case MTAL_IO_VDECEX_TVD_SETCHANNELCHANGE:
            i4Ret = _MTVDECEX_TVD_SetChannelChange(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETENABLEDCOLORSYSTEM:
            i4Ret = _MTVDECEX_TVD_SetEnabledColorSystem(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETVPRESONOFFGAIN:
            i4Ret = _MTVDECEX_TVD_SetVPresOnOffGain(arg); 
            break;
        case MTAL_IO_VDECEX_TVD_SET_SYNCDET_FOR_TUNING:
            i4Ret = _MTVDECEX_TVD_SetSyncDetForTuning(arg);
            break;
        case MTAL_IO_VDECEX_TVD_SETHPLL:
            i4Ret = _MTVDECEX_TVD_SetHPLL(arg); 
            break;
        case MTAL_IO_VDECEX_GET_VIDEOSTATUS:
        	 i4Ret = _MTVDECEX_GetVideoStatus(arg);
            break;
        case MTAL_IO_VDECEX_GET_VIDEOSRCTYPE:
        	 i4Ret = _MTVDECEX_GetVideoSrcType(arg);
            break;
        case MTAL_IO_VDECEX_GET_VIDEODEFSTATUS:
        	 i4Ret = _MTVDECEX_GetVideoDefStatus(arg);
            break;
#if 0            
        case MTAL_IO_VDECEX_GET_VIDEODECTYPE:
    	        i4Ret = _MTVDECEX_GetVideoDecType(arg);
    	        break;
        case MTAL_IO_VDECEX_VGA_MWIFQUERYAMBIGUOUSTIMING:
        	i4Ret = _MTVDECEX_VGA_MWIFQueryAmbiguousTiming(arg);
        	break;
        case MTAL_IO_VDECEX_VGA_MWIFSETAMBIGUOUSINDEX:
              i4Ret = _MTVDECEX_VGA_MWIFSetAmbiguousIndex(arg);
              break;
#endif                 
        case MTAL_IO_VDECEX_VGA_MWIFRESETAMBIGUOUSTIMINGEEPROM:
              i4Ret = _MTVDECEX_VGA_MWIFResetAmbiguousTimingEEPROM();
              break;
           
        case MTAL_IO_VDECEX_TVD_GET_SIGNALSTRENGTHLEVEL:
        	i4Ret = _MTVDECEX_TVD_GetSignalStrengthLevel(arg);
        	break;
        case MTAL_IO_VDECEX_SETPORCHTUNE:
        	i4Ret = _MTVDECEX_SetPorchTune(arg);
        	break;
        case MTAL_IO_VDECEX_STARTYPBPRAUTO:
        	i4Ret = _MTVDECEX_StartYPbPrAuto();
        	break;
        case MTAL_IO_VDECEX_GET_VIDEOREFRESHRATE:
              i4Ret = _MTVDECEX_GetVideoRefreshRate(arg);
              break;

        case MTAL_IO_VDECEX_GET_VIDEOINPUTWIDTH:
            i4Ret = _MTVDECEX_GetVideoInputWidth(arg);
            break;
        case MTAL_IO_VDECEX_GET_VIDEOINPUTHEIGHT:
            i4Ret = _MTVDECEX_GetVideoInputHeight(arg);
            break;
        case MTAL_IO_VDECEX_IS_VIDEOSRCINTERLACE:
            i4Ret = _MTVDECEX_IsVideoSrcInterlace(arg);
            break;
//        case MTAL_IO_VDECEX_TDTV_GETTAG3D:
//           i4Ret = _MTVDECEX_TDTV_GetTag3D(arg);
//            break;
		case MTAL_IO_VDECEX_IS_MHLSIGNAL:
			i4Ret = _MTVDECEX_IsMhlSignal(arg);
			 break;

        case MTAL_IO_VDECEX_GET_VIDEOINPUTPOLARITY:
            i4Ret = _MTVDECEX_GetVideoInputPolarity(arg);
            break;
        case MTAL_IO_VDECEX_GET_VIDEOINPUTTIMINGID:
            i4Ret = _MTVDECEX_GetVideoInputTimingID(arg);
            break;
        case MTAL_IO_VDECEX_GET_VIDEOINTERNALDEC:
            i4Ret = _MTVDECEX_GetVideoInternalDec(arg);
            break;
        case MTAL_IO_VDECEX_TVD_GET_NRLEVEL:
            i4Ret = _MTVDECEX_TVD_GetNRLevel(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GET_HDCPSTATUS:
            i4Ret = _MTVDECEX_HDMI_GetHDCPStatus(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GET_SCANINFO:
            i4Ret = _MTVDECEX_HDMI_GetScanInfo(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GET_AFD:
            i4Ret = _MTVDECEX_HDMI_GetAFD(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_GET_INPUTTYPE:
            i4Ret = _MTVDECEX_HDMI_GetInputType(arg);
            break;
        case MTAL_IO_VDECEX_HDMI_SUPPORT_AUDIO:
            i4Ret = _MTVDECEX_HDMI_IsSupportAudio(arg);
            break;
        case MTAL_IO_VDECEX_SET_SIGNALBOOSTER:
            i4Ret = _MTVDECEX_SetSignalBooster(arg);
            break;
            
         case MTAL_IO_VDECEX_SET:
            i4Ret = _MTVDECEX_Set(arg);
            break;
            
        case MTAL_IO_VDECEX_GET:
            i4Ret = _MTVDECEX_Get(arg);
            break;  
        case MTAL_IO_VDECEX_GETVIDEOSRC:
            i4Ret =_MTVDECEX_GetVideoSrc(arg);
            break;			
        case MTAL_IO_VDECEX_VBI_GET_SUPPORT_MHL_PORT_ID:
            i4Ret = _MTVDECEX_HDMI_GetMhlPortId(arg);
            break;
        case MTAL_IO_VDECEX_GET_SRC_DETECT_STATUS:
			i4Ret = _MTVDECEX_GetSrcDetectStatus(arg);
			break;    
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
		case MTAL_IO_VDECEX_GETHDMISPDUPSTA:
			i4Ret = _MTVDECEX_HDMI_GetUpSta(arg);
			break;
		case MTAL_IO_VDECEX_GETHDMISPDUPSTACHG:
			i4Ret = _MTVDECEX_HDMI_GetUpStaChg(arg);
			break;
		case MTAL_IO_VDECEX_GETHDMISPDSVSTA:
			i4Ret = _MTVDECEX_HDMI_GetSvSta(arg);
			break;
		case MTAL_IO_VDECEX_GETHDMISPDSVSTACHG:
			i4Ret = _MTVDECEX_HDMI_GetSvStaChg(arg);
			break;
#endif
#if defined(CC_MT5890)
		case MTAL_IO_VDECEX_GETHDMISPD4kPSSTA:
			i4Ret = _MTVDECEX_HDMI_Get4KpsSta(arg);
			break;
#endif
  case MTAL_IO_VDECEX_HDMI_SET_EXTERNAL_EDID_DATA_BY_PORT:
	  i4Ret = _MTVDECEX_HDMI_SetExternalEdidDataByPort(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_GET_EXTERNAL_EDID_DATA_BY_PORT:
	  i4Ret = _MTVDECEX_HDMI_GetExternalEdidDataByPort(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_GETPORTINFOFRAME:
	  i4Ret = _MTVDECEX_HDMI_GetInfoFrameByPort(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_GETPORTMODE:
	  i4Ret = _MTVDECEX_HDMI_GetModeByPort(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_GETPORTTIMINGINFO:
	  i4Ret = _MTVDECEX_HDMI_GetTimingInfoByPort(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_ENABLEMHL:
	  i4Ret = _MTVDECEX_HDMI_EnableMHL(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_MHL_CLEARRCPSTATE:
	  i4Ret = _MTVDECEX_HDMI_MHL_ClearRcpState(arg);
	  break;
  case MTAL_IO_VDECEX_HDMI_GETHWACTIVEINFO:
  	   i4Ret = _MTVDECEX_HDMI_GetHWActiveInfo(arg);
	   break;
  default:
       i4Ret = MTR_NOT_OK;
       break;

	}
	return i4Ret;
}



