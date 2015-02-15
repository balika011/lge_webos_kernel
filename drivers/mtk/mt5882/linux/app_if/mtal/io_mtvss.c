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
 * $RCSfile: $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtvss.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "vdp_if.h"
#include "video_def.h"
#include "drv_auto.h"

#include "mtvdo.h"
#include "cb_low.h"

#if defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5396) ||defined(CC_MT5389) || defined(CC_MT5368)
#include "nptv_drvif.h"
#include "vdp_drvif.h"
#include "api_srch_if.h"
#include "drv_scaler.h"
#include "drv_tdtv_drvif.h"
#include "drv_di.h"
#endif

#if defined(CC_MT5396)
#include "drv_mjc_if.h"
#include "vdo_misc.h"
#include "b2r_drvif.h"
#include "drv_display.h"
#include "nptv_drvif.h"
#endif

#include "drv_video.h"
#include "x_hal_5381.h"
#include "x_hal_arm.h"
#include "drv_scaler.h"

#include "hw_di.h"
#include <linux/mtal_ioctl.h>
#include "fbm_drvif.h"
#include "tve_if.h"
#include "drv_scaler_gfx.h"
#include "drv_scaler_drvif.h"
#include "source_select.h"
#include "mtvss.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "vdo_misc.h"
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
#include "drv_w2dr2r.h"
#endif
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
//#define CC_INPUTSOURCE_VIDEO_ON_TEXTURE

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define VSS_INDEX 2
//#define VSS_USE_MMAP
//#define VSS_NO_CTRL_VDP
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
#define VSS_VDEC    1
#define VSS_HDMI    0
#endif
#if defined(CC_MT5399)     //mustang have a hw issue,oryx have fix it
#define FBM_VSS_ONE_FRAME
#endif
// copy from mtvss.h

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                          \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                      \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                       \


#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)           \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))                                   \
    {                                                       \
        printk("mtal_ioctl_mtvss valid argument error\n");  \
        return MTR_ERR_INV;                                 \
    }   

#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)        \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       size))                               \
    {                                                       \
        printk("mtal_ioctl_mtvss argument copy error\n");   \
        return MTR_ERR_INV;                                 \
    } 

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                           

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }  
    
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                           

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }  


#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                           

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }  

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }                                                       \
 
#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)             \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))                           \
    {                                                       \
        printk("mtal_ioctl_mtvss argument error\n");        \
        return MTR_ERR_INV;                                 \
    }    

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)            \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,     \
                       sizeof(type)))                       \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");        \
        return MTR_ERR_INV;                                 \
    }

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
UINT32 _mtvss_FBMinfo_u4Addr = 0;
UINT32 _mtvss_FBMinfo_u4Size = 0;
UINT8 _mtvss_init    = 0;
UINT8 _mtvss_connect = 0;
UINT16 _mtvss_width  = 0;
UINT16 _mtvss_height = 0;

UINT8 _VSS_Path = MTVSS_MAX;
UINT8 _VSS_Idx = VSS_INDEX;
UINT8 _VSS_Cnt = 0;
UINT8 _VSS_ConnectTrigger = 0;

UINT8 _VSS_DumpFmt = FORMAT_NONE;

static UINT8 u1DisableTVE = SV_FALSE;
static UCHAR ucTVEFmt = TVE_MAX_FMT; //TVE_MAX_FMT


typedef struct
{
    UINT16  ui2_width;
    UINT16  ui2_height;

    /* Timestamp */
    HAL_TIME_T  t_hal_time;

    /* Dump format */
    UINT8 ui1_dump_fmt;
}   VSS_CAPTURE_INFO_T;


static VSS_CAPTURE_INFO_T _mtvss_cap_info[4];

extern void (*_pfVdoVSSCb)(void);

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
/**
 * @brief 
 It initialized the part pertaining to the Video auxiliary outputs. 
 */
static MT_RESULT_T _MTVSS_Init(void)
{
    FBM_POOL_T* prPool;

//    MTVSS_PRINT(" - void\n");


    // get VSS FBM Pool info
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_VSS, NULL);

    // pool is valid, update dram info.
    if (prPool != NULL)
    {
        _mtvss_FBMinfo_u4Addr = prPool->u4Addr;
        _mtvss_FBMinfo_u4Size = prPool->u4Size;
        #ifndef FBM_VSS_ONE_FRAME
        if (_mtvss_FBMinfo_u4Size < 1920 * 1080 * 4 * 2)
        {
            printk(" fbm pool size is too small for vss\n");
            return MTR_NOT_OK;
        }
        #else
        if (_mtvss_FBMinfo_u4Size < 1280 * 720 * 4 * 1)
        {
            printk(" fbm pool size is too small for vss\n");
            return MTR_NOT_OK;
        }
        #endif
    }
    else
    {
        printk(" fail to get pool\n");
        return MTR_NOT_OK;
    }

	u1DisableTVE = SV_FALSE;
	ucTVEFmt = TVE_MAX_FMT;
    _mtvss_init = 1;

    // Get FBM pool info, Set buffer number/address, Set write mode, Set alpha
    vScpip_GFX_init();
#ifndef FBM_VSS_ONE_FRAME
    //default use two buffer
    u1Scpip_GFX_Set_Frame_Num(2);//tem solution because LG AP do no ready for argb
    u1Scpip_GFX_Set_Dram_Address(_mtvss_FBMinfo_u4Addr, _mtvss_FBMinfo_u4Size/2, 2);
#else
    //set frame num=2, but buffer1=buffer0. only use one buffer
    u1Scpip_GFX_Set_Dram_Address(_mtvss_FBMinfo_u4Addr, _mtvss_FBMinfo_u4Size, 1);
    u1Scpip_GFX_Set_Frame_Num(2);
    vScpipGfxSetAddrBase(1, (_mtvss_FBMinfo_u4Addr>>4));
#endif
    vDrvVrmSetAppFlag(VRM_APP_GRAPHIC_DUMP);
    u1Scpip_GFX_Set_Write_Mode(0);
	_VSS_DumpFmt=RGBA8888;
    u1Scpip_GFX_Set_Dump_Format(RGBA8888);
    u1Scpip_GFX_Set_Alpha(0xFF);
    
    return MTR_OK;
}

/**
 * @brief 
 It connects the VSS component with the Input Source.
 * @param eVdpId
 */
static MT_RESULT_T _MTVSS_Connect(unsigned long arg)
{
    // Color space, Set Main/Sub path, Set VSS path. 
    MTVSS_PATH_T  eVdpId;
    UINT32 index, status;
    UINT16 width, height;
    UINT8 bPath;
    UINT32 u4Ctrl = 1;
#if defined(CC_MT5396)
#ifndef VSS_NO_CTRL_VDP
    VDP_REGION_T VSSSrcRegion, VSSOutRegion;
#endif
#endif

    eVdpId = (MTVSS_PATH_T)arg;
    //eVdpId = MTVDO_MAIN;
    printk("DEBUG connect \n");

    // error handling for flow
    if (_mtvss_init == 0)
    {
        printk(" error \n");
        return MTR_NOT_OK;
    }
    
    if (_mtvss_connect == 1 && _VSS_Path != eVdpId)
    {
        printk(" wrong path...\n");
        return MTR_PARAMETER_ERROR;
    }
    else if (_mtvss_connect == 1 && _VSS_Path == eVdpId)
    {
        printk(" alrady connect...\n");
        //return MTR_OK;
    }
    
    //printk(" MTVDO path %d \n", eVdpId);
    if (eVdpId >= MTVSS_MAX) //  || _mtvss_connect == 1, must disconnect first, then connect again
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }

    _mtvss_connect = 1;
    _VSS_Path = eVdpId;
    vDrvVrmSetAppFlag(VRM_APP_GRAPHIC_DUMP);
    // connect to main/sub
    u1Scpip_GFX_Source_Sel(_VSS_Path);
    
    printk(" Vss Src %d \n", bApiVideoGetSrcType(_VSS_Path));
#if defined(CC_MT5396)
#ifndef VSS_NO_CTRL_VDP    
    if (eVdpId == MTVSS_SUB)
    {
        UCHAR ucEnable;
        
        TVE_GetEnable(TVE_1,&ucEnable); // check TVE
        if(ucEnable)
        {
            TVE_SetEnable(TVE_1, FALSE);
            VDP_SetEnable(VDP_2, FALSE);
            bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_MAX);
            vApiTVESetOutputFormat(SV_TVE_OFMT_NONE);
        }

        bApiVideoSetVideoSrc(MTVDO_SUB, bApiVideoGetSrcType(MTVDO_MAIN));
        VSSSrcRegion.u4X = 0;
        VSSSrcRegion.u4Y = 0;
        VSSSrcRegion.u4Width  = 10000;
        VSSSrcRegion.u4Height = 10000;
        VSSOutRegion.u4X = 0;
        VSSOutRegion.u4Y = 0;
        VSSOutRegion.u4Width  = 10000;
        VSSOutRegion.u4Height = 10000;
        VDP_SetEnable(VDP_2, TRUE);
        VDP_SetSrcRegion(VDP_2, 1, VSSSrcRegion); // Full screen
        VDP_SetOutRegion(VDP_2, 1, VSSOutRegion);
        
        vSetPipFlg(PIP_FLG_MODE_CHG);
        vSetPipFlg(PIP_FLG_MODE_DET_DONE);
        printk(" Sub  Src %d \n", bApiVideoGetSrcType(MTVDO_SUB));
    }
#endif // VSS_NO_CTRL_VDP
#endif

	#if defined(CC_MT5882)
	if(_VSS_Path == MTVSS_SUB)
	{
		UCHAR ucEnable;

		TVE_GetEnable(TVE_1,&ucEnable); // if vss use sub path , capri only support one case of vss and tve 
		TVE_GetFmt(TVE_1,&ucTVEFmt);
		printk("Connect disable TVE  %d \n",ucEnable);
		if(ucEnable)
		{
			TVE_SetEnable(TVE_1, FALSE);
			VDP_SetEnable(VDP_2, FALSE);
			bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_MAX);
			vApiTVESetOutputFormat(SV_TVE_OFMT_NONE);
			u1DisableTVE = SV_TRUE;
		}
	}
	#endif

	u1Scpip_GFX_Set_3x3();
    u1Scpip_GFX_Write_Enable(1);
    
    status = u1Scpip_GFX_GetWriteStatus();
    index = u1Scpip_GFX_GetCurrentFrameIdx();
    if (status != 0 || index != 0)
    {
        printk(" DEBUG init status & index != 0\n");
    }    

    // get video width/height
    bPath   = getScalerPath(_VSS_Path); // main
//    if (_VSS_Path == MTVDO_MAIN)
    {
        VDP_SCALER_PRM_T* scalerInfo;
 
        scalerInfo = getScalerInfo(bPath); 
        width  = scalerInfo->u4DramWidth;
        height = scalerInfo->u4DramHeight;

        width = ((width+3)>>2)<<2; // multiple of 4
    }
#if 0
    else // MTVDO_SUB
    {
        width  = picInfo->wSrcW;
        height = picInfo->wSrcH;
    }
#endif
    _mtvss_width  = ((vScpipGetInputWidth(_VSS_Path)+3)>>2)<<2;
    _mtvss_height = vScpipGetInputHeight(_VSS_Path);

    if (_VSS_Path == MTVDO_SUB)
    {
        DRVCUST_OptQuery(eDisplayOffCtrlByDriver, (UINT32 *)&u4Ctrl);
        if (u4Ctrl)
        {
            VDP_SetDisplayOff_Ext(MTVDO_SUB,DRV_DISP_OFF_MODULE_VSS, TRUE);
        }
    }
#if defined(CC_MT5396)         
#ifndef VSS_NO_CTRL_VDP
    if (_VSS_Path == MTVDO_SUB)
    {
        vDrvDIInOutSelect(MTVDO_SUB, 1, MTVDO_MAIN);        // DI SRC MUX: sub out from main
        vDrvDIStopCtrlFw(MTVDO_SUB, SV_OFF);                // DI stop normal FW routine
        vDrvDIOnOff(MTVDO_SUB, SV_ON);                      // DI sub path always on (for P-timing)
    }
#endif // VSS_NO_CTRL_VDP
#endif

    _VSS_ConnectTrigger = 30;
    _VSS_Idx = index;
    MTVSS_PRINT(" - _mtvss_width %d _mtvss_height %d _VSS_Path %x\n",_mtvss_width,_mtvss_height,_VSS_Path);

    return MTR_OK;
}
 
/**
 * @brief 
 It connects the VSS component with the Input Source(Main/Sub/MonOut).
 * @param eVdpId
 */
extern void _VdoVSSCb(void);
static MT_RESULT_T _MTVSS_ConnectEx(unsigned long arg)
{
    MTVSS_PATH_T eVdpId;

    eVdpId = (MTVSS_PATH_T)arg;
    printk("DEBUG connect ex \n");

    // error handling for flow
    if (_mtvss_init == 0)
    {
        printk(" error \n");
        return MTR_NOT_OK;
    }
    
    if (_mtvss_connect == 1 && _VSS_Path != eVdpId)
    {
        printk(" wrong path...\n");
        return MTR_PARAMETER_ERROR;
    }
    else if (_mtvss_connect == 1 && _VSS_Path == eVdpId)
    {
        printk(" alrady connect...\n");
        //return MTR_OK;
    }
    
    if (eVdpId >= MTVSS_MAX)
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }

	if((eVdpId == MTVSS_MAIN_PREPROC) || (eVdpId == MTVSS_MAIN_MIB) ||
		(eVdpId == MTVSS_SUB_PREPROC) || (eVdpId == MTVSS_SUB_MIB))
	{
        printk(" Not supportted VSS path in Ex mode \n");
        return MTR_PARAMETER_ERROR;
	}

    _mtvss_connect = 1;
    _VSS_Path = eVdpId;
    vDrvVrmSetAppFlag(VRM_APP_GRAPHIC_DUMP);
    u1Scpip_GFX_Source_Sel(_VSS_Path);
	u1Scpip_GFX_Set_3x3();
    u1Scpip_GFX_Write_Enable(1);
//	vDrvVrmSetAppStage();
#if defined (CC_MT5399) || defined (CC_MT5890)
    _pfVdoVSSCb = _VdoVSSCb;
#endif

    return MTR_OK;
}

/**
 * @brief 
 It connects the VSS component with the Input Source.
  (MainPreProc/MainMIB/MainScaler/SubPreProc/SubMIB/SubScaler/MonOut).
 * @param eVdpId
 */
static MT_RESULT_T _MTVSS_ConnectAll(unsigned long arg)
{
    MTVSS_PATH_T eVdpId;

    eVdpId = (MTVSS_PATH_T)arg;
    printk("DEBUG connect all \n");

    // error handling for flow
    if (_mtvss_init == 0)
    {
        printk(" error \n");
        return MTR_NOT_OK;
    }
    
    if (_mtvss_connect == 1 && _VSS_Path != eVdpId)
    {
        printk(" wrong path...\n");
        return MTR_PARAMETER_ERROR;
    }
    else if (_mtvss_connect == 1 && _VSS_Path == eVdpId)
    {
        printk(" alrady connect...\n");
    }
    
    if (eVdpId >= MTVSS_MAX)
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }

    _mtvss_connect = 1;
    _VSS_Path = eVdpId;
    vDrvVrmSetAppFlag(VRM_APP_GRAPHIC_DUMP);
    u1Scpip_GFX_Source_Sel(_VSS_Path);
	u1Scpip_GFX_Set_3x3();
    u1Scpip_GFX_Write_Enable(1);
	
	#if defined(CC_MT5882)
	if(_VSS_Path == MTVSS_SUB)
	{
		UCHAR ucEnable;

		TVE_GetEnable(TVE_1,&ucEnable); // if vss use sub path , capri only support one case of vss and tve 
		TVE_GetFmt(TVE_1,&ucTVEFmt);
		printk("ConnectAll disable TVE  %d \n",ucEnable);
		if(ucEnable)
		{
			TVE_SetEnable(TVE_1, FALSE);
			VDP_SetEnable(VDP_2, FALSE);
			bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_MAX);
			vApiTVESetOutputFormat(SV_TVE_OFMT_NONE);
			u1DisableTVE = SV_TRUE;
		}
	}
	#endif
	
#if defined (CC_MT5399) || defined (CC_MT5890) || defined (CC_MT5882)
    _pfVdoVSSCb = _VdoVSSCb;
#endif

    return MTR_OK;
}

/**
 * @brief 
 It releases the VSS connection settings.
 * @param eVdpId
 */
static MT_RESULT_T _MTVSS_Disconnect(unsigned long arg)
{
    // Set Sub path
    
    MTVSS_PATH_T eVdpId;
    UINT32 u4Ctrl;
    eVdpId = (MTVSS_PATH_T)arg;
    //eVdpId = MTVDO_MAIN;
    printk("DEBUG disconnect \n");

 //   MTVSS_PRINT(" - eVdpId = %d\n", (int)eVdpId);
    //printk(" MTVDO path %d \n", eVdpId);
    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    if (eVdpId != _VSS_Path)
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }
    if (_VSS_Path == MTVDO_SUB)
    {
        DRVCUST_OptQuery(eDisplayOffCtrlByDriver, (UINT32 *)&u4Ctrl);
        if (u4Ctrl)
        {
            VDP_SetDisplayOff_Ext(MTVDO_SUB, DRV_DISP_OFF_MODULE_VSS,FALSE);
        } 
    }
    vDrvVrmClrAppFlag(VRM_APP_GRAPHIC_DUMP);
#if defined(CC_MT5396)     
#ifndef VSS_NO_CTRL_VDP    
    if (_VSS_Path == MTVSS_SUB) // the default value
    {
        vDrvDIInOutSelect(MTVDO_SUB, 1, MTVDO_SUB);        // DI SRC MUX: sub out
        vDrvDIStopCtrlFw(MTVDO_SUB, SV_ON);                // DI normal FW routine
        VDP_SetEnable(VDP_2, FALSE);
        vSetPipFlg(PIP_FLG_MODE_CHG);
        vSetPipFlg(PIP_FLG_MODE_DET_DONE);
    }
#endif // VSS_NO_CTRL_VDP
#endif

	#if defined(CC_MT5882)
	if(u1DisableTVE == SV_TRUE)
	{
		TVE_SetEnable(TVE_1, TRUE);
		VDP_SetEnable(VDP_2, TRUE);
		bApiVideoSetVideoSrc(MTVDO_SUB, bApiVideoGetSrcType(MTVDO_MAIN));
		if(ucTVEFmt == TVE_FMT_480I)
		{
			vApiTVESetOutputFormat(SV_TVE_OFMT_NTSC);			
		}
		else if(ucTVEFmt == TVE_FMT_576I)
		{
			vApiTVESetOutputFormat(SV_TVE_OFMT_PAL);			
		}
		ucTVEFmt = TVE_MAX_FMT;
		u1DisableTVE = SV_FALSE;
		printk("Disconnect Enable TVE \n");
	}
	#endif

    u1Scpip_GFX_Write_Enable(0);
    
    _mtvss_connect = 0;
    _VSS_Path  = MTVSS_MAX;

    return MTR_OK;
}

/**
 * @brief 
 It releases the VSS connection settings.
 * @param eVdpId
 */
static MT_RESULT_T _MTVSS_DisconnectEx(unsigned long arg)
{
    MTVSS_PATH_T eVdpId;
    eVdpId = (MTVSS_PATH_T)arg;
	
	if((eVdpId == MTVSS_MAIN_PREPROC) || (eVdpId == MTVSS_MAIN_MIB) ||
		(eVdpId == MTVSS_SUB_PREPROC) || (eVdpId == MTVSS_SUB_MIB))
	{
        printk(" Not supportted VSS path in Ex mode \n");
        return MTR_PARAMETER_ERROR;
	}

    printk("DEBUG disconnect \n");

    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    if (eVdpId != _VSS_Path)
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }
	
    vDrvVrmClrAppFlag(VRM_APP_GRAPHIC_DUMP);
    u1Scpip_GFX_Write_Enable(0);
    
    _mtvss_connect = 0;
    _VSS_Path  = MTVSS_MAX;

    return MTR_OK;
}

/**
 * @brief 
 It releases the VSS connection settings.
 * @param eVdpId
 */
static MT_RESULT_T _MTVSS_DisconnectAll(unsigned long arg)
{
    MTVSS_PATH_T eVdpId;
    eVdpId = (MTVSS_PATH_T)arg;
	
    printk("DEBUG disconnect \n");

    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    if (eVdpId != _VSS_Path)
    {
        printk(" error \n");
        return MTR_PARAMETER_ERROR;
    }
	
    vDrvVrmClrAppFlag(VRM_APP_GRAPHIC_DUMP);
    u1Scpip_GFX_Write_Enable(0);

	#if defined(CC_MT5882)
	if(u1DisableTVE == SV_TRUE)
	{
		TVE_SetEnable(TVE_1, TRUE);
		VDP_SetEnable(VDP_2, TRUE);
		bApiVideoSetVideoSrc(MTVDO_SUB, bApiVideoGetSrcType(MTVDO_MAIN));
		if(ucTVEFmt == TVE_FMT_480I)
		{
			vApiTVESetOutputFormat(SV_TVE_OFMT_NTSC);			
		}
		else if(ucTVEFmt == TVE_FMT_576I)
		{
			vApiTVESetOutputFormat(SV_TVE_OFMT_PAL);			
		}
		ucTVEFmt = TVE_MAX_FMT;
		u1DisableTVE = SV_FALSE;
		printk("DisconnectAll Enable TVE \n");
	}
	#endif
    
    _mtvss_connect = 0;
    _VSS_Path  = MTVSS_MAX;

    return MTR_OK;
}

/**
 * @brief 
 It stops the Video auxiliary outputs (CVBS output).
 * @param eVssId
 */
static MT_RESULT_T _MTVSS_GetVideoInfo(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT16 *pwidth;
    UINT16 *pheight;
    UINT16 *pcolor;
    UINT16 *psource;
    UINT16 width, height, color, source, signal;
    UINT8 bPath = SV_VP_MAIN;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    pwidth  = (UINT16 *)rArg.ai4Arg[0];
    pheight = (UINT16 *)rArg.ai4Arg[1];
    pcolor  = (UINT16 *)rArg.ai4Arg[2];
    psource = (UINT16 *)rArg.ai4Arg[3];

    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    if((_VSS_Path == MTVSS_MAIN) || (_VSS_Path == MTVSS_MAIN_MIB) || (_VSS_Path == MTVSS_MAIN_PREPROC))
    {
        bPath = SV_VP_MAIN;
    }
    else if((_VSS_Path == MTVSS_SUB) || (_VSS_Path == MTVSS_SUB_MIB) || (_VSS_Path == MTVSS_SUB_PREPROC))
    {
        bPath = SV_VP_PIP;
    }
   
#if defined (CC_MT5399) || defined (CC_MT5890) || defined (CC_MT5882)  
    _mtvss_width  = ((u4Scpip_GFX_GetWidth()+3)>>2)<<2;
    _mtvss_height = u4Scpip_GFX_GetHeight();
    width  = u4Scpip_GFX_GetWidth();
#else
    _mtvss_width  = ((vScpipGetInputWidth(_VSS_Path)+3)>>2)<<2;
    _mtvss_height = vScpipGetInputHeight(_VSS_Path);
    width  = _mtvss_width;
#endif
    height = _mtvss_height;
    color  = bHDMIClocrimetry();
    signal = (UINT16)bGetSignalType(bPath);
    switch(signal)
    {
        case SV_ST_TV:      source = MTVSS_INPUT_TYPE_COAXIAL;
                            break;
        case SV_ST_AV:      source = MTVSS_INPUT_TYPE_COMPOSITE;
                            break;
        case SV_ST_SV:      source = MTVSS_INPUT_TYPE_SVIDEO;
                            break;
        case SV_ST_YP:      source = MTVSS_INPUT_TYPE_COMPONENT;
                            break;
        case SV_ST_VGA:     source = MTVSS_INPUT_TYPE_VGA;
                            break;
        case SV_ST_DVI:     source = MTVSS_INPUT_TYPE_HDMI;
                            break;
        case SV_ST_MPEG:    source = MTVSS_INPUT_TYPE_COAXIAL;
                            break;
        default:            source = MTVSS_INPUT_TYPE_MAX;
                            break;
    }

    if(_VSS_Path != MTVSS_MON_OUT && bDrvVideoSignalStatus(bPath) != SV_VDO_STABLE)
    {
        width = 0;
        height = 0;   
    }
    
    //printk(" DEBUG _MTVSS_GetVideoInfo %d %d %d %d\n", width, height, color, source);
    USR_SPACE_ACCESS_VALIDATE_ARG(pwidth,  INT16);
    COPY_TO_USER_ARG(pwidth,  width,  INT16);
    USR_SPACE_ACCESS_VALIDATE_ARG(pheight, INT16);
    COPY_TO_USER_ARG(pheight, height, INT16);
    USR_SPACE_ACCESS_VALIDATE_ARG(pcolor,  INT16);
    COPY_TO_USER_ARG(pcolor,  color,  INT16);
    USR_SPACE_ACCESS_VALIDATE_ARG(psource, INT16);
    COPY_TO_USER_ARG(psource, source, INT16);
    
 //   MTVSS_PRINT(" Width = %d, Height = %d, Color = %d, Source = %d\n",
//	              width, height, color, source);

    return MTR_OK;
}

static MT_RESULT_T _MTVSS_GetAddrBase(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 *pBase0;
    UINT32 *pBase1;
    UINT32 AddrBase0;
    UINT32 AddrBase1;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    pBase0 = (UINT32 *)rArg.ai4Arg[0];
    pBase1 = (UINT32 *)rArg.ai4Arg[1];

    AddrBase0=(vScpipGfxGetAddrBase(0)<<4);
    AddrBase1=(vScpipGfxGetAddrBase(1)<<4);

    USR_SPACE_ACCESS_VALIDATE_ARG(pBase0,  INT32);
    COPY_TO_USER_ARG(pBase0,  AddrBase0,  INT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pBase1, INT32);
    COPY_TO_USER_ARG(pBase1, AddrBase1, INT32);

    return MTR_OK;
}

/** * @brief Video 
 It makes the Video auxiliary outputs mute On/Off. 
 * @param eVssId 
 * @param bMuteOn TRUE : Mute On, FALSE : Mute Off 
 */
static MT_RESULT_T _MTVSS_GetPlatformInfo(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

//    USR_SPACE_ACCESS_VALIDATE_ARG(prSrcRegion, MTVDO_REGION_T);
//    COPY_TO_USER_ARG(prSrcRegion, rSrcRegion, MTVDO_REGION_T);
    
    return MTR_OK;
}

/**
 * @brief Get Frame Buffer from Scaler DRAM dump.
 Copy to user (previous frame)
 Check status, if index changed, call back
 After call back, trigger next frame, update current index
 * @param roi_x
 * @param roi_y
 * @param roi_width
 * @param roi_height
 * @param *buffer
 */
static MT_RESULT_T _MTVSS_GetFrameBuffer(unsigned long arg)
{
#ifndef VSS_USE_MMAP
    MTAL_IOCTL_5ARG_T rArg;
    UINT16 roi_x;
    UINT16 roi_y;
    UINT16 roi_width;
    UINT16 roi_height;
    UINT8  *buffer;
    UINT8  *buffer_index;
    UINT16 row, dram_w, dram_h;
    UINT16 bytesPerPixel;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);
    roi_x      = rArg.ai4Arg[0];
    roi_y      = rArg.ai4Arg[1];
    roi_width  = rArg.ai4Arg[2];
    roi_height = rArg.ai4Arg[3];
    buffer     = (UINT8  *)rArg.ai4Arg[4];

    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    dram_w = _mtvss_width;
    dram_h = _mtvss_height;
    
    // error handling, real total WIDTH, HEIGHT
    if (roi_width * roi_height > dram_w * dram_h)
    {
        printk(" error roi %d %d > dram %d %d\n", roi_width, roi_height, dram_w, dram_h);
        return MTR_PARAMETER_ERROR;
    }
    
    if ((roi_x + roi_width > dram_w) || (roi_y + roi_height > dram_h))
    {
        printk(" error roi (%d %d %d %d) > dram %d %d\n", roi_x, roi_y, roi_width, roi_height, dram_w, dram_h);
        return MTR_PARAMETER_ERROR;
    }

    //MTVSS_PRINT(" - roi_x  %d roi_y  %d roi_width %d roi_height %d\n",roi_x,roi_y,roi_width,roi_height);
    //MTVSS_PRINT(" - dram_w %d dram_h %d buffer 0x%x\n",dram_w,dram_h,buffer);

   _VSS_Idx = u1Scpip_GFX_GetCurrentFrameIdx() & (VSS_INDEX - 1 ); 
    // COPY from (x, y) to (x+width, y+height)
#ifdef FBM_VSS_ONE_FRAME
    buffer_index  = (UINT8  *)_mtvss_FBMinfo_u4Addr;// + (_mtvss_FBMinfo_u4Size/2)*_VSS_Idx;
#else
    buffer_index  = (UINT8  *)_mtvss_FBMinfo_u4Addr + (_mtvss_FBMinfo_u4Size/2)*_VSS_Idx;
#endif
    if (_VSS_DumpFmt == RGBA8888)
    {
        bytesPerPixel = 4;
    }
    else if (_VSS_DumpFmt == YUV422 || _VSS_DumpFmt == RGB565)
    {
        bytesPerPixel = 2;
    }
    else
    {
        return MTR_NOT_SUPPORTED;
    }

    buffer_index += bytesPerPixel*(roi_y * dram_w + roi_x);
    for (row = 0; row < roi_height; row++)
    {
        // copy roi_width only
        USR_SPACE_ACCESS_VALIDATE_ARG(buffer, UINT8);
        if (copy_to_user((void __user *)buffer, (void *)(VIRTUAL((UINT32)buffer_index)), 4*(sizeof(UINT8)*roi_width)))
        {                                                                          
            printk("%s copy_to_user error\n", __FUNCTION__);                       
            return MTR_DONT_CARE;                                                    
        }
        buffer       += bytesPerPixel*roi_width;
        buffer_index += bytesPerPixel*dram_w;
    }

    /* Save the captured info */
    _mtvss_cap_info[_VSS_Idx].ui2_width  = _mtvss_width;
    _mtvss_cap_info[_VSS_Idx].ui2_height = _mtvss_height;
    _mtvss_cap_info[_VSS_Idx].ui1_dump_fmt = _VSS_DumpFmt;
    HAL_GetTime(&_mtvss_cap_info[_VSS_Idx].t_hal_time);

    // trigger next frame
    //_VSS_Idx = (_VSS_Idx == 1)?0:1;
   // u1Scpip_GFX_Write_Trigger(); // trigger WRITE
#endif // VSS_USE_MMAP    

    return MTR_OK;
}


/**
 * @brief Get Frame Buffer from Scaler DRAM dump.
 Copy to user (previous frame)
 Check status, if index changed, call back
 After call back, trigger next frame, update current index
 * @param roi_x
 * @param roi_y
 * @param roi_width
 * @param roi_height
 * @param *buffer
 */
static MT_RESULT_T _MTVSS_GetFrameBuffer_Ex(unsigned long arg)
{
#ifndef VSS_USE_MMAP
    MTAL_IOCTL_5ARG_T rArg;
    UINT16 roi_x;
    UINT16 roi_y;
    UINT16 roi_width;
    UINT16 roi_height;
    UINT8  *buffer;
    UINT8  *buffer_index;
    UINT16 row, dram_w, dram_h;

	UINT32 u4MtVssCapWidth,u4MtVssCapHeight;
	UINT32 u4CopyWidth,u4RsetWidth;
	UINT32 u4CopyHeight,u4RsetHeight;


    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);
    roi_x      = rArg.ai4Arg[0];
    roi_y      = rArg.ai4Arg[1];
    roi_width  = rArg.ai4Arg[2];
    roi_height = rArg.ai4Arg[3];
    buffer     = (UINT8  *)rArg.ai4Arg[4];

    if (_mtvss_connect == 0)
    {
        printk(" not connected \n");
        return MTR_NOT_OK;
    }

    dram_w = _mtvss_width;
    dram_h = _mtvss_height;
    //printk(" debug index %d w %d h %d\n", _VSS_Idx, dram_w, dram_h);

	   
    //if ((roi_x + roi_width > dram_w) || (roi_y + roi_height > dram_h))
    //{
    //    printk(" error roi (%d %d %d %d) > dram %d %d\n", roi_x, roi_y, roi_width, roi_height, dram_w, dram_h);
    //    return MTR_PARAMETER_ERROR;
    //}

    // COPY from (x, y) to (x+width, y+height)
    
    #ifndef FBM_VSS_ONE_FRAME
       buffer_index  = (UINT8  *)_mtvss_FBMinfo_u4Addr + (_mtvss_FBMinfo_u4Size/2)*_VSS_Idx;
	#else
	   buffer_index  = (UINT8  *)_mtvss_FBMinfo_u4Addr; // + (_mtvss_FBMinfo_u4Size/2)*_VSS_Idx;
	#endif


#if defined (CC_MT5399) || defined (CC_MT5890) || defined (CC_MT5882)  
		u4MtVssCapWidth  = u4Scpip_GFX_GetWidth();
		u4MtVssCapHeight = u4Scpip_GFX_GetHeight();
#else
		u4MtVssCapWidth  = vScpipGetInputWidth(_VSS_Path);
		u4MtVssCapHeight = vScpipGetInputHeight(_VSS_Path);
#endif
	//printk(" VSS w= %d; h= %d\n", u4MtVssCapWidth, u4MtVssCapHeight);


    if (( roi_x > u4MtVssCapWidth ) || ( roi_y > u4MtVssCapHeight) )
    {
       printk(" error roi (%d , %d ) > capture size(%d , %d ) \n", roi_x, roi_y, u4MtVssCapWidth, u4MtVssCapHeight);
       return MTR_PARAMETER_ERROR;
    }
   
    if ( roi_x + roi_width > u4MtVssCapWidth )
    {
       u4CopyWidth = u4MtVssCapWidth - roi_x;
	   u4RsetWidth = ( roi_x + roi_width ) - u4MtVssCapWidth;
    }
	else
	{
	  u4CopyWidth = roi_width;
	  u4RsetWidth = 0;
	}

    if ( roi_y + roi_height > u4MtVssCapHeight )
    {
       u4CopyHeight = u4MtVssCapHeight - roi_y;
	   u4RsetHeight = ( roi_y + roi_height ) - u4MtVssCapHeight;
    }
	else
	{
	  u4CopyHeight = roi_height;
	  u4RsetHeight = 0;
	}



	
    buffer_index += 4*(roi_y * dram_w + roi_x);
    for (row = 0; row < u4CopyHeight; row++)
    {
        // copy roi_width only
        USR_SPACE_ACCESS_VALIDATE_ARG(buffer, UINT8);
        if (copy_to_user((void __user *)buffer, (void *)(VIRTUAL((UINT32)buffer_index)), 4*(sizeof(UINT8)*u4CopyWidth)))
        {                                                                          
            printk("%s copy_to_user error\n", __FUNCTION__);                       
            return MTR_DONT_CARE;                                                    
        }
        buffer       += 4*roi_width;
        buffer_index += 4*dram_w;
    }
#endif	
    return MTR_OK;
}

static MT_RESULT_T _MTVSS_GetFrameBufferIndex(unsigned long arg)
{
    UINT8 *pindex;
    UINT8 index;

    pindex  = (UINT8 *)arg;
    index   = u1Scpip_GFX_GetCurrentFrameIdx();

    USR_SPACE_ACCESS_VALIDATE_ARG(pindex,  UINT8);
    COPY_TO_USER_ARG(pindex,  index,  UINT8);

    _VSS_Idx = index;

    //MTVSS_PRINT(" - VSS Get Frame Buffer Index _VSS_Idx %d\n",_VSS_Idx);

    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_GetWriteEndInt(unsigned long arg)
{
    UINT8 *penable;
    UINT8 enable;

    penable  = (UINT8 *)arg;
	
	#if defined(CC_MT5399) || defined (CC_MT5890) || defined (CC_MT5882)  
    enable   = u4ScpipGetISRstatus(ISR_DRAM_GFX_WRITE);
	#else
	enable = 0;
	#endif

    USR_SPACE_ACCESS_VALIDATE_ARG(penable,  UINT8);
    COPY_TO_USER_ARG(penable,  enable,  UINT8);

    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_ClrWriteEndInt(unsigned long arg)
{
	#if defined(CC_MT5399) || defined (CC_MT5890) || defined (CC_MT5882)  
    vScpipClearISRstatus(ISR_DRAM_GFX_WRITE);
	#endif
	
    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_GetScreenCaptureMode(unsigned long arg)
{
	MTVSS_SCREEN_CAPTURE_MODE_T *peMode;
	MTVSS_SCREEN_CAPTURE_MODE_T eMode;


	peMode = (MTVSS_SCREEN_CAPTURE_MODE_T *)arg;
#if defined(CC_MT5399) || defined (CC_MT5890) 
	//eMode  = (MTVSS_SCREEN_CAPTURE_MODE_T)u4ScpipGetCaptureMode();
	eMode  = SCALER_SCREEN_CAPTURE_FROM_OSTG;
#else
	eMode  = MTVSS_SCREEN_CAPTURE_NOT_SUPPORTED;
#endif
    USR_SPACE_ACCESS_VALIDATE_ARG(peMode,  MTVSS_SCREEN_CAPTURE_MODE_T);
    COPY_TO_USER_ARG(peMode,  eMode,  MTVSS_SCREEN_CAPTURE_MODE_T);

    return MTR_OK;    
}

/**
 * @brief 
 * @param eVssId
 * @param pData Data buffer pointer
 * @param length Data Length
 * @endcode
 */
static MT_RESULT_T _MTVSS_SetFrameCallback(unsigned long arg)
{    
    // Kernel space, user space

    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_SetColorFormat(unsigned long arg)
{    
    // Set ARGB8888 format
    UINT32 ColorFmt;
    UINT8 channelA, channelR, channelG, channelB;
    
    ColorFmt = (UINT32)arg;

    channelA = (ColorFmt >> 24) & 0xFF;
    channelR = (ColorFmt >> 16) & 0xFF;
    channelG = (ColorFmt >>  8) & 0xFF;
    channelB =  ColorFmt & 0xFF;

    vScpipGfxAlphaPosSel(channelA);
	vScpipGfxSetMux(channelR, channelG, channelB);

    MTVSS_PRINT(" - A = %d, R = %d, G = %d, B = %d\n", channelA , channelR, channelG, channelB);
    
    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_SetTrigger(unsigned long arg)
{    
    u1Scpip_GFX_Write_Trigger();

    //MTVSS_PRINT(" - VSS Ap set Trigger\n");
     
    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_SetFrameDrop(unsigned long arg)
{ 
	MTAL_IOCTL_4ARG_T rArg;
	
	UINT8 u1VdpId; 
	UINT8 u1Enable;
	UINT8 u1DropM; 
	UINT8 u1DropN;
	
	USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u1VdpId  = (UINT8)rArg.ai4Arg[0];
    u1Enable = (UINT8)rArg.ai4Arg[1];
    u1DropM  = (UINT8) rArg.ai4Arg[2];
    u1DropN  = (UINT8)rArg.ai4Arg[3];
	
    u1Scpip_GFX_Set_Frame_Drop(u1VdpId, u1Enable, u1DropM, u1DropN);
  
    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_SetWriteEnable(unsigned long arg)
{ 
	UINT8  enable = (UINT8)arg;

    u1Scpip_GFX_Write_Enable(enable);
 
    return MTR_OK;    
}

static MT_RESULT_T _MTVSS_SetVSSFreeze(unsigned long arg)
{ 
	UINT8  u1freeze = (UINT8)arg;
	
	u1Scpip_GFX_Write_Freeze(u1freeze);
	
    return MTR_OK;    
}


static MT_RESULT_T _MTVSS_SetDumpFormat(unsigned long arg)
{    
    // Set RGBA8888, RGB565, YUV422 format

    _VSS_DumpFmt = arg;
    u1Scpip_GFX_Set_Dump_Format(_VSS_DumpFmt);
    printk("DEBUG %d \n", _VSS_DumpFmt);

#ifdef CC_SUPPORT_VSS
    return MTR_OK;    
#else
    return MTR_NOT_SUPPORTED;
#endif
}


/**
 * @brief Get Frame Buffer Attribute
 */
static MT_RESULT_T _MTVSS_GetFrameBufferCapInfo(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;    
    UINT16* pui2_width     = NULL;
    UINT16* pui2_height    = NULL;
    UINT32* pui4_seconds   = NULL;
    UINT32* pui4_micros    = NULL;
    UINT8*  pui1_fmt       = NULL;
    UINT16  ui2_cap_width  = 0;
    UINT16  ui2_cap_height = 0;
    UINT32  ui4_seconds    = 0;
    UINT32  ui4_micros     = 0;
    UINT8   ui1_fmt        = FORMAT_NONE;
       
    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    pui2_width    = (UINT16*) rArg.ai4Arg[0];
    pui2_height   = (UINT16*) rArg.ai4Arg[1];
    pui4_seconds  = (UINT32*) rArg.ai4Arg[2];
    pui4_micros   = (UINT32*) rArg.ai4Arg[3];
    pui1_fmt      = (UINT8*)  rArg.ai4Arg[4];

    /* The captured data */
    if (_VSS_Idx < VSS_INDEX)
    {
        ui2_cap_width  = _mtvss_cap_info[_VSS_Idx].ui2_width;
        ui2_cap_height = _mtvss_cap_info[_VSS_Idx].ui2_height;
        ui4_seconds    = _mtvss_cap_info[_VSS_Idx].t_hal_time.u4Seconds;
        ui4_micros     = _mtvss_cap_info[_VSS_Idx].t_hal_time.u4Micros;
        ui1_fmt        = _mtvss_cap_info[_VSS_Idx].ui1_dump_fmt;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pui2_width,  UINT16);
    COPY_TO_USER_ARG(pui2_width, ui2_cap_width,  UINT16);

    USR_SPACE_ACCESS_VALIDATE_ARG(pui2_height,  UINT16);
    COPY_TO_USER_ARG(pui2_height, ui2_cap_height,  UINT16);

    USR_SPACE_ACCESS_VALIDATE_ARG(pui4_seconds,  UINT32);
    COPY_TO_USER_ARG(pui4_seconds, ui4_seconds,  UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pui4_micros,  UINT32);
    COPY_TO_USER_ARG(pui4_micros, ui4_micros,  UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pui1_fmt,  UINT8);
    COPY_TO_USER_ARG(pui1_fmt, ui1_fmt,  UINT8);
    
#ifdef CC_SUPPORT_VSS  
    return MTR_OK;
#else
    return MTR_NOT_SUPPORTED;
#endif
}
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
static VOID _ColorConvertYCbCr2ARGB8888(UINT32 u4Which, UINT32 u4YAddr, UINT32 u4CbAddr, UINT32 u4CrAddr, 
    UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch, UINT32 u4Rgb, UINT32 u4DstAddr)
{
    //from Ycbcr444 -> ayuv8888 ->argb8888
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;

    x_memset((void *)&rJpgParam, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T));

    rJpgParam.u4IsRsIn      = 1; //block/raster in
    rJpgParam.u4YSrcBase1   = (u4YAddr);
    rJpgParam.u4YSrcBase2   = (u4YAddr);

    rJpgParam.u4YSrcBufLen  = u4Pitch;
    rJpgParam.u4CSrcBufLen  = u4Pitch; 

    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW       = u4Width;
    rJpgParam.u4YSrcH       = u4Height;

    rJpgParam.u4CbSrcBase1  = (u4CbAddr);
    rJpgParam.u4CbSrcBase2  = (u4CbAddr);
    rJpgParam.u4CbSrcHOffset    = 0;
    rJpgParam.u4CbSrcVOffset    = 0;
    rJpgParam.u4CbSrcW      = u4Width;
    rJpgParam.u4CbSrcH      = u4Height;

    rJpgParam.u4CrSrcBase1  = (u4CrAddr);
    rJpgParam.u4CrSrcBase2  = (u4CrAddr);
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = u4Width;
    rJpgParam.u4CrSrcH = u4Height;

    rJpgParam.u4IsRsOut = 1; //block/raster out
    rJpgParam.u4OutSwap = 0;
    rJpgParam.u4ColorComp = 7; //y,cb,cr exists

    rJpgParam.u4YTgBase   = u4DstAddr;
    rJpgParam.u4CTgBase   = u4DstAddr;

    rJpgParam.u4IsJpgSameaddr = 0;
    rJpgParam.u4YTgHOffset = 0;
    rJpgParam.u4YTgVOffset = 0;
    rJpgParam.u4YTgBufLen = u4Pitch << 2;
    rJpgParam.u4CTgBufLen = u4Pitch << 2;

    rJpgParam.u4IsVdo2Osd = 1; // output in OSD format ?
    rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
    rJpgParam.u4YTgW = u4Width;
    rJpgParam.u4YTgH = u4Height;
    rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
    rJpgParam.u4CTgW = u4Width;
    rJpgParam.u4CTgH = u4Height;

    rJpgParam.u4IsFstBl = 1;
    rJpgParam.u4IsLstBl = 1;
    rJpgParam.u4JpgVfacY = 1; 
    rJpgParam.u4JpgVfacCb = 0;
    rJpgParam.u4JpgVfacCr = 0;
    rJpgParam.fgBlassign = TRUE; //indicates to directly assign input/output address

    IMGRZ_Lock_Ex(u4Which);
    IMGRZ_ReInit_Ex(u4Which);
    IMGRZ_SetScaleOpt_Ex(u4Which, E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale_Ex(u4Which, &rJpgParam);
    IMGRZ_Flush_Ex(u4Which);
    IMGRZ_Wait_Ex(u4Which);
    IMGRZ_Unlock_Ex(u4Which);

    //convert AYUV to ARGB
    if(!u4Rgb)
    {
        IMGRZ_AYUV_ARGB_Conv_Ex(u4Which, rJpgParam.u4YTgBase, rJpgParam.u4YTgBufLen, rJpgParam.u4YTgW, rJpgParam.u4YTgH,
            rJpgParam.u4YTgHOffset,rJpgParam.u4YTgVOffset, 0, 1);
    }
    
    return ;
}

static VOID _ColorConvertYC2ARGB8888(UINT32 u4Which, VDP_CAPTURE_INTO_T rCapInfo, UINT32 u4DstAddr)
{
    RZ_VDO_SCL_PARAM_SET_T rSclParam;
    //UINT32 u4YAddr, u4CAddr, u4Width, u4Height, u4Pitch;

    //yc420blk -> ayuv8888->argb8888
    x_memset((void *)&rSclParam, 0, sizeof(RZ_VDO_SCL_PARAM_SET_T));

    rSclParam.u4InMode      = E_RZ_VDO_OUTMD_420;
    rSclParam.u4IsRsIn      = rCapInfo.u4IsRsIn;
    rSclParam.u4YSrcBase    = rCapInfo.u4AddrY;
    rSclParam.u4YSrcBufLen  = rCapInfo.u4MemHSize;
    rSclParam.u4YSrcHOffset = 0;
    rSclParam.u4YSrcVOffset = 0;
    rSclParam.u4YSrcW       = rCapInfo.u4HSize;
    rSclParam.u4YSrcH       = rCapInfo.u4VSize;
    rSclParam.u4CSrcBase    = rCapInfo.u4AddrC;
    rSclParam.u4CSrcBufLen  = rCapInfo.u4MemHSize;
    rSclParam.u4CSrcW       = rCapInfo.u4HSize >> 1;
    rSclParam.u4CSrcH       = rCapInfo.u4VSize >> 1;
    rSclParam.u4CSrcHOffset = 0;
    rSclParam.u4CSrcVOffset = 0;
    rSclParam.u4IsRsOut     = 1;

    //output aycbcr8888
    rSclParam.u4OutMode     = (UINT32) E_RZ_VDO_OUTMD_444;
    rSclParam.u4IsVdo2Osd   = 1; //OSD format ?
    rSclParam.u4YTgCM       = (UINT32) E_RZ_OSD_DIR_CM_AYCbCr8888;

    rSclParam.u4YTgBase     = u4DstAddr;
    rSclParam.u4YTgBufLen   = rCapInfo.u4MemHSize << 2;
    rSclParam.u4YTgW        = rCapInfo.u4HSize;
    rSclParam.u4YTgH        = rCapInfo.u4VSize;
    rSclParam.u4YTgHOffset  = 0;
    rSclParam.u4YTgVOffset  = 0;

    rSclParam.u4CTgBase     = u4DstAddr;
    rSclParam.u4CTgBufLen   = rCapInfo.u4MemHSize << 2;
    rSclParam.u4CTgW        = rCapInfo.u4HSize;
    rSclParam.u4CTgH        = rCapInfo.u4VSize;

    rSclParam.u4SrcSwap     = rCapInfo.u4SrcSwap;
    rSclParam.u4OutSwap     = 0;
    rSclParam.u4ABlend      = 0x80;

    IMGRZ_Lock_Ex(u4Which);
    IMGRZ_ReInit_Ex(u4Which);
    IMGRZ_SetScaleOpt_Ex(u4Which, E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale_Ex(u4Which, &rSclParam);
    IMGRZ_Flush_Ex(u4Which);
    IMGRZ_Wait_Ex(u4Which);
    IMGRZ_Unlock_Ex(u4Which);

    //convert AYUV to ARGB
    IMGRZ_AYUV_ARGB_Conv_Ex(u4Which, rSclParam.u4YTgBase, rSclParam.u4YTgBufLen, rSclParam.u4YTgW, rSclParam.u4YTgH,
          rSclParam.u4YTgHOffset,rSclParam.u4YTgVOffset, 0, 1);

    return ;
}

static MT_RESULT_T _MTVSS_GetCaptureBuffer(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    UINT8 u1SrcType, u1HdmiPort, u1EsId;
    UINT32 u4Width, u4Height, u4Pitch, u4Which;
    UINT32 u4DstAddr;
    VDP_CAPTURE_INTO_T    rCapInfo;
    W2D_VDO_TXTURE_TYPE rW2DHdmiInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u4Which     = (UINT8)rArg.ai4Arg[0];
    u1SrcType   = (UINT8)rArg.ai4Arg[1];
    u4Width     = (UINT32)rArg.ai4Arg[2];
    u4Height    = (UINT32)rArg.ai4Arg[3];
    u4Pitch     = (UINT32)rArg.ai4Arg[4];

    //to get the capture frame and do color convert to argb8888
    //get dst buffer
    u4DstAddr = FBM_GetCaptureDstAddr();
    if(!u4DstAddr)
    {
        printk("- Get FBM capture dst addr Err...\n");
        return MTR_NOT_OK;
    }
    else
    {
        //different rzid, different dst address, for multi case
        u4DstAddr = (u4DstAddr + 0x7f8000 * u4Which);
    }

    if(u1SrcType == VSS_VDEC)
    {
        //use imgrzid to get es id
        u1EsId = VDEC_RmGetEsByImgrz(u4Which);
        //get buffer frame info from vdec
        if (!VDEC_GetDisplayFrameBufferAddr(u1EsId, &rCapInfo))
        {
            printk("- Get CapInfo Err... ...\n");
            return MTR_NOT_OK;
        }
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u1EsId);
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MJPEG)
        {
        #if  defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)// Force raster out for VENC
            rCapInfo.u4IsRsIn = 1; //MJPEG for 5395 encoder input
            rCapInfo.u4SrcSwap = 0;
        #else
            rCapInfo.u4IsRsIn = 0;
            rCapInfo.u4SrcSwap = 6;//block swap mode;
        #endif
        }
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
        {
            rCapInfo.u4IsRsIn = 0;
            rCapInfo.u4SrcSwap = 0;//block swap mode;
        }
        else
        {
            rCapInfo.u4IsRsIn = 0;
            rCapInfo.u4SrcSwap = 6;//block swap mode;
        }

        MTVSS_PRINT("- %s-%d-[OUT] [%d][0x%x][0x%x][%d][%d][%d][%d][%d][0x%x]\n", __FUNCTION__, __LINE__,
            u1EsId, rCapInfo.u4AddrY, rCapInfo.u4AddrC, rCapInfo.u4HSize, rCapInfo.u4VSize, rCapInfo.u4MemHSize, 
            rCapInfo.u4IsRsIn, rCapInfo.u4SrcSwap, u4DstAddr);

        rArg.ai4Arg[0] = rArg.ai4Arg[0]; //no use 
        rArg.ai4Arg[1] = rArg.ai4Arg[1]; //no use
        //to get width, height, pitch
        rArg.ai4Arg[2] = rCapInfo.u4HSize;
        rArg.ai4Arg[3] = rCapInfo.u4VSize;
        rArg.ai4Arg[4] = rCapInfo.u4MemHSize << 2;

        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_5ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_5ARG_T);

        _ColorConvertYC2ARGB8888(u4Which, rCapInfo, u4DstAddr);
    }
    else if(u1SrcType == VSS_HDMI)
    {     
        //use imgrz id to get hdmi port
        u1HdmiPort = u4GetVdoTxtureHdmiPort(u4Which);
        //get frame buffer from hdmi
        if(!u1R2rGetDumpFrameAddr(&rW2DHdmiInfo))
        {
            printk("- HDMI capture get frame address Err...\n");
            return MTR_NOT_OK;
        }
        //for hdmi, pitch is equal to  width 16 align
        rW2DHdmiInfo.pitch = (rW2DHdmiInfo.width + 0xf) & (~0xf);
        MTVSS_PRINT("- %s-%d-[IN] [0x%x][0x%x][0x%x][%d][%d][%d][0x%x]\n", __FUNCTION__, __LINE__,
            rW2DHdmiInfo.Yaddr, rW2DHdmiInfo.Uaddr, rW2DHdmiInfo.Vaddr, rW2DHdmiInfo.width, rW2DHdmiInfo.height, 
            rW2DHdmiInfo.pitch, u4DstAddr);

        rArg.ai4Arg[0] = rArg.ai4Arg[0]; //no use 
        rArg.ai4Arg[1] = rArg.ai4Arg[1]; //no use
        rArg.ai4Arg[2] = rW2DHdmiInfo.width;
        rArg.ai4Arg[3] = rW2DHdmiInfo.height;
        rArg.ai4Arg[4] = rW2DHdmiInfo.pitch;

        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_5ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_5ARG_T);

        _ColorConvertYCbCr2ARGB8888(u4Which, rW2DHdmiInfo.Yaddr, rW2DHdmiInfo.Uaddr, rW2DHdmiInfo.Vaddr, rW2DHdmiInfo.width, 
            rW2DHdmiInfo.height, rW2DHdmiInfo.pitch, rW2DHdmiInfo.isRgb, u4DstAddr);
    }
    else
    {
        printk("- %s-%d fail, error src type.\n", __FUNCTION__, __LINE__);
    }

    return MTR_OK;
}
#else
static MT_RESULT_T _MTVSS_GetCaptureBuffer(unsigned long arg)
{
    printk("- %s-%d Dummy function.\n", __FUNCTION__, __LINE__);
    return MTR_NOT_OK;
}
#endif


int mtal_ioctl_mtvss(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int i4Ret = MTR_OK;
	   
    switch (cmd) 
    {				
        case MTAL_IO_VSS_INIT:
            return _MTVSS_Init();
        case MTAL_IO_VSS_CONNECT:
            return _MTVSS_Connect(arg);
        case MTAL_IO_VSS_DISCONNECT:
             return _MTVSS_Disconnect(arg);
        case MTAL_IO_VSS_GET_VIDEO_INFO:
            return _MTVSS_GetVideoInfo(arg);
        case MTAL_IO_VSS_GET_PLATFORM_INFO:
            return _MTVSS_GetPlatformInfo(arg);
        case MTAL_IO_VSS_GET_FRAME_BUFFER:
            return _MTVSS_GetFrameBuffer(arg);
		case MTAL_IO_VSS_GET_FRAME_BUFFER_EX:
			return _MTVSS_GetFrameBuffer_Ex(arg);			
        case MTAL_IO_VSS_CALLBACK:
            return _MTVSS_SetFrameCallback(arg);
        case MTAL_IO_VSS_COLOR_FORMAT:
            return _MTVSS_SetColorFormat(arg);
        case MTAL_IO_VSS_TRIGGER:
            return _MTVSS_SetTrigger(arg);
        case MTAL_IO_VSS_GET_FRAME_IDX:
            return _MTVSS_GetFrameBufferIndex(arg);
        case MTAL_IO_VSS_GET_ADDR_BASE:
            return _MTVSS_GetAddrBase(arg);
        case MTAL_IO_VSS_CONNECT_EX:
            return _MTVSS_ConnectEx(arg);
        case MTAL_IO_VSS_CONNECT_ALL:
            return _MTVSS_ConnectAll(arg);
        case MTAL_IO_VSS_DISCONNECT_EX:
            return _MTVSS_DisconnectEx(arg);
        case MTAL_IO_VSS_DISCONNECT_ALL:
            return _MTVSS_DisconnectAll(arg);
        case MTAL_IO_VSS_GET_CAPTURE_MODE:
            return _MTVSS_GetScreenCaptureMode(arg);
        case MTAL_IO_VSS_DUMP_FORMAT:
            return _MTVSS_SetDumpFormat(arg);
		case MTAL_IO_VSS_GET_FRAME_BUFFER_CAP_INFO:
            return _MTVSS_GetFrameBufferCapInfo(arg);
        case MTAL_IO_VSS_GET_CAPTURE_BUFFER:
            return _MTVSS_GetCaptureBuffer(arg);
		case MTAL_IO_VSS_GET_WRITE_END_INT:
            return _MTVSS_GetWriteEndInt(arg);
	    case MTAL_IO_VSS_CLR_WRITE_END_INT:
            return _MTVSS_ClrWriteEndInt(arg);
		case MTAL_IO_VSS_SET_FRAME_DROP:
            return _MTVSS_SetFrameDrop(arg);
	    case MTAL_IO_VSS_SET_WRITE_ENABLE:
            return _MTVSS_SetWriteEnable(arg);
		case MTAL_IO_VSS_SET_WRITE_FREEZE:
            return _MTVSS_SetVSSFreeze(arg);
        default:
            return MTR_PARAMETER_ERROR;
    }
    return i4Ret;
}

