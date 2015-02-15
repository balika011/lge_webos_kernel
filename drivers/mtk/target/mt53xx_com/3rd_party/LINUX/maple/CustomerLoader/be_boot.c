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
 * $RCSfile: be_boot.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file loader_if.c
 *  loader_if.c provides loader high-level interface functions.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "c_model.h"
#ifdef CC_MAPLE_BE_LOADER_BOOT_FLOW
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define BE_UPDATE_TIMING_BIT  (1 << 23)
#define BE_OSD_BASE_MONITOR   (OSD_BASE_REG+ 0xfc)

#ifndef LOADER_BE_TOTAL_FRAME_NUMBER
#define LOADER_BE_TOTAL_FRAME_NUMBER          (9)
#endif

//#define LOADER_BE_DATA_SWAP

#ifndef LOADER_BE_DATA_SWAP
#define LOADER_BE_BOOT_HEADER_SIZE_32  (0x00000020)
#define LOADER_BE_BOOT_HEADER_SIZE_128  (0x00000080)
#else
#define LOADER_BE_BOOT_HEADER_SIZE_32  (0x20000000)
#define LOADER_BE_BOOT_HEADER_SIZE_128  (0x80000000)
#endif

#define BOOT_INIT_ERR_COUNT    (600)
#define BE_BOOT_ERROR_RETRY_COUNT (3)

//#define BE_LOADER_DBG
//#define BE_BOOT_ERROR_CHECK
//#define BE_EMMC_HS_MODE
//#define BE_SUPPORT_FRM_HDR_MISC_INFO
//#define BE_SEND_POWER_STATE

#define BE_PACKET_FLAGS_SUPPORT
#define BE_DELETE_UPGRADE_SUPPORT

#ifndef CC_MTK_LOADER
#define BE_FBM_VERIFICATION_MODE
#endif
#ifdef BE_FBM_VERIFICATION_MODE
#define BE_FBM_VERIFICATION_RISING_COUNT  (8)
#define BE_FBM_VERIFICATION_FALLING_COUNT   (8)
#endif

#define BE_BOOT_OSD_TWO_BUF_MODE //neef at least 48MB FBM
#define BE_ARGB_RE_ORDER
//#define BE_OSD_SHIFT

#ifdef BE_LOADER_DBG
#define BELDBG(fmt,args...)  Printf (fmt ,##args)
#else
#define BELDBG(fmt,args...) 
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#if LOADER_LOGO_NUMBER || LOADER_MULTIPLE_LOGO_PARTITION
#if 1
#include "ostg_if.h"
#include "pmx_drvif.h"
#include "osd_drvif.h"
#include "gfx_if.h"
#include "panel.h"
#include "drv_video.h"
#include "drv_vdoclk.h"
#include "drv_display.h"
#include "drv_lvds.h"
#include "sv_const.h"
#include "hw_scpos.h"
#include "hw_lvds.h"
#include "vdp_if.h"
#include "nptv_if.h"
#include "x_pdwnc.h"
#include "x_gpio.h"
#if defined(CC_MT5368)|| defined(CC_MT5396)|| defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860) || defined(CC_MT5881)
#include "drv_tcon.h"
#endif
#endif /* LOADER_LOGO_NUMBER */
#ifdef CC_EMMC_BOOT
#include "msdc_if.h"
#endif
#include "x_hal_926.h"
#ifdef CC_MT5395
#include "drv_vbyone.h"
#endif
#include "be_boot_if.h"
#include "eeprom_if.h"
#ifndef CC_MTK_LOADER
#include "fbm_drvif.h"
#include "x_hal_5381.h"
#ifdef LINUX_TURNKEY_SOLUTION
#ifdef __KERNEL__
#include <linux/mm.h>
#endif
#endif
#endif
//---------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------

static BE_FRAME_INFO_T rFrameHeaderInfo[LOADER_BE_TOTAL_FRAME_NUMBER];
static BE_OUTPUT_FRM_INFO_T rOutputFrm[LOADER_BE_TOTAL_FRAME_NUMBER]=
	     {{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},
	      {0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF}};
#ifndef CC_MTK_LOADER
static BE_FBM_INFO_T rBESTRFbmInfo[LOADER_BE_TOTAL_FRAME_NUMBER];
#endif

#ifdef BE_BOOT_OSD_TWO_BUF_MODE
static BOOL _fgSecondBufused=FALSE;
#endif

#ifdef BE_ARGB_RE_ORDER
static BE_OSD_ORDER_T rARGBOsdOrder={3,2,0,1};
#endif

#ifdef BE_OSD_SHIFT
static INT32 i4PixelShift=0;
static INT32 i4VPixelShift=0;
static BOOL  fgShiftUpdated=FALSE;
#else
static BOOL  fgShiftUpdated=FALSE;
//static INT32 i4PixelShift=0x46;
//static INT32 i4VPixelShift=0x1D;
//this shift was due to OSTG using post TG, if BYPASS this shift was moved to Front TG
static INT32 i4PixelShift=0;
static INT32 i4VPixelShift=0;
#endif

#ifdef CC_MTK_LOADER
#ifdef BE_PACKET_FLAGS_SUPPORT
static BE_DEL_PART_INFO_T rDeletTypeInfo[32];
static UINT8 ui1DelTypeIndex=0;
#endif
#endif

static UINT8 u1TotalDisplayFrmNo=0;
//#if LOADER_LOGO_NUMBER || LOADER_MULTIPLE_LOGO_PARTITION
#if 1
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT16 crc16tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
#if 0
static const unsigned short crc16tab[256]= {
{
    0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
    0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
    0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
    0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
    0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
    0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
    0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
    0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974, 
    0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
    0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
    0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
    0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
    0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
    0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
    0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
    0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
    0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
    0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
    0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
    0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
    0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
    0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
    0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
    0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
    0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
    0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
    0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
    0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
    0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
    0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
    0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
    0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
};
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
UINT16 crc16_ccitt(UINT8 *buf, UINT32 len)
{
	UINT32 counter;
	UINT16 crc = 0;
	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
	return crc;
}

BE_BOOT_FLASH_INFO_T LDR_BEFlashPartitionTranslate(UINT8 u1PartitionIdx, UINT32 u4Offset)
{
    UINT32 u4BeFlashIdOffset;
	BE_BOOT_FLASH_INFO_T rBeFlashInfo;
	
    //u4BeFlashId = DRVCUST_InitGet(eNANDFlashPartIdChannelDataA);
    if(u1PartitionIdx > eFlashPartID_RESV_3)
    {
        u1PartitionIdx = 0; //set to DDRI for error or not exist
        u4BeFlashIdOffset = u4Offset;//34;
        rBeFlashInfo.u4Size = DRVCUST_InitGet((QUERY_TYPE_T)(eNANDFlashPartSize0+u4BeFlashIdOffset+u1PartitionIdx));
	    rBeFlashInfo.u4PartId = u1PartitionIdx + u4BeFlashIdOffset;    
    }
    u4BeFlashIdOffset = u4Offset;//34;
    rBeFlashInfo.u4Size = DRVCUST_InitGet((QUERY_TYPE_T)(eNANDFlashPartSize0+u4BeFlashIdOffset+u1PartitionIdx));
	rBeFlashInfo.u4PartId = u1PartitionIdx + u4BeFlashIdOffset;
	
	return rBeFlashInfo;
}
#if 0
UINT8 LDR_BENxtFrameIdx(UINT8 ui1FrameId)
{
    UINT8 ui1NextFrameId;

    switch(ui1FrameId)
    {
	  case 0:
		ui1NextFrameId = 1;
		break;
      case 1:
	  	ui1NextFrameId = 2;
	  	break;
      case 2:
	  	ui1NextFrameId = 3;
	  	break;
      case 3:
	  	ui1NextFrameId = 4;
	  	break;
      case 4:
	  	ui1NextFrameId = 5;
	  	break;		
	  default:
	  	ui1NextFrameId = ui1FrameId;
	  	break;
    }
	return ui1NextFrameId;
}
#endif
INT32 BEGetFrmHdrMiscInfo(UINT8 ui1FrameIdx)
{
    UINT8 arPanelIDString[16]={0};
    UINT8 i=0,j=0,idx=0;
    DTVCFG_T rDtvCfg;

    UNUSED(EEPDTV_GetCfg(&rDtvCfg));
	
	if(rFrameHeaderInfo[ui1FrameIdx].rFrameMode == eIFrame_Header_mode)
	{
        //to be decided
	}
	else
	{
        //get boot mode info 
        rFrameHeaderInfo[ui1FrameIdx].FrameHeader[32] = 0x01;
        //get panel off boot request
        rFrameHeaderInfo[ui1FrameIdx].FrameHeader[33] = 0x00;
		//get logo on off info
		rFrameHeaderInfo[ui1FrameIdx].FrameHeader[34] = 0x01;
        if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) && (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
        {
            BELDBG("BEGetFrmHdrMiscInfo : Quiet Boot\n");
		    rFrameHeaderInfo[ui1FrameIdx].FrameHeader[34] = 0x00;
        }
        #ifdef CC_QUIET_HOT_BOOT_SUPPORT
        if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_QUIET_HOT_BOOT)
        {
            BELDBG("BEGetFrmHdrMiscInfo : Quiet Hot Boot\n");
			rFrameHeaderInfo[ui1FrameIdx].FrameHeader[34] = 0x00;
        }
        #endif    
        if (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_DISABLE_LOGO)
        {
	        BELDBG("BEGetFrmHdrMiscInfo : AP force LOGO disabled\n");
			rFrameHeaderInfo[ui1FrameIdx].FrameHeader[34] = 0x00;
        }		
		//get download phase info
		rFrameHeaderInfo[ui1FrameIdx].FrameHeader[35] = 0x01;
		//get panel ID string
		EEPROM_Read(BE_IC_PANEL_ID_ADDR, (UINT32)arPanelIDString, 16);
		for(i=0; i<16; i++)
		{
			BELDBG("BEGetFrmHdrMiscInfo: arPanelIDString[%d]=0x%x\n",i,arPanelIDString[i]);
		}		
		for(i=0; i<4; i++)
		{
		    for(j=0;j<4; j++)
		    {
		        idx = j+(i*4);
		        rFrameHeaderInfo[ui1FrameIdx].FrameHeader[48+idx] = arPanelIDString[idx];
		    }
		}
	}
	return 0;
}
#ifndef CC_MTK_LOADER
#ifdef BE_SEND_POWER_STATE
INT32 BEResumeGetPowerState(void)
{
    DTVCFG_T rDtvCfg;

    UNUSED(EEPDTV_GetCfg(&rDtvCfg));
	if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) && (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
	{
	    BELDBG("BEResumeGetPowerState : Quiet Boot\n");
		DRVCUST_PanelPowerStateToBe(FALSE);
    }
	#ifdef CC_QUIET_HOT_BOOT_SUPPORT
    else if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_QUIET_HOT_BOOT)
    {
        BELDBG("BEResumeGetPowerState : Quiet Hot Boot\n");
	    DRVCUST_PanelPowerStateToBe(FALSE);
    }
    #endif
	else
	{
	    BELDBG("BEResumeGetPowerState : Normal Boot\n");
		DRVCUST_PanelPowerStateToBe(TRUE);
	}
	return 0;
}
#endif
#endif

INT32 LDR_BEIFrameHeaderFooterGen(BE_I_FRAME_T rInputFrame, UINT8 *puFrameHeaderArray , UINT8 HeaderSize, UINT8 *puFrameFooterArray, UINT8 FooterSize)
{
	UINT8 i,shift_idx;
	UINT32 u4BeTmpData;

    if(HeaderSize < 32 || FooterSize <16)
    {
        return -1;
    }

	u4BeTmpData = rInputFrame.rIFrameHeaderInfo.u4Key;
    for(i=0;i<4;i++)
    {
        shift_idx = (3-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rIFrameHeaderInfo.u4HeaderSize;
    for(i=4;i<8;i++)
    {
        shift_idx = (7-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rIFrameHeaderInfo.u4FrameNo;
    for(i=16;i<20;i++)
    {
        shift_idx = (19-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rIFrameHeaderInfo.u4TotalFrameNo;
    for(i=20;i<24;i++)
    {
        shift_idx = (23-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rIFrameHeaderInfo.u4TotalPacketNo;
    for(i=24;i<28;i++)
    {
        shift_idx = (27-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rIFrameFooterInfo.u4FootKey;
    for(i=12;i<16;i++)
    {
        shift_idx = (15-i)*8;
        *(puFrameFooterArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }

	return 0;
}

INT32 LDR_BEFFrameHeaderFooterGen(BE_F_FRAME_T rInputFrame, UINT8 *puFrameHeaderArray , UINT8 HeaderSize, UINT8 *puFrameFooterArray, UINT8 FooterSize)
{
	UINT8 i,shift_idx;
	UINT32 u4BeTmpData;

    if(HeaderSize < 32 || FooterSize <16)
    {
        return -1;
    }

	u4BeTmpData = rInputFrame.rFFrameHeaderInfo.u4Key;
    for(i=0;i<4;i++)
    {
        shift_idx = (3-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rFFrameHeaderInfo.u4HeaderSize;
    for(i=4;i<8;i++)
    {
        shift_idx = (7-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rFFrameHeaderInfo.u4FrameNo;
    for(i=16;i<20;i++)
    {
        shift_idx = (19-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rFFrameHeaderInfo.u4TotalFrameNo;
    for(i=20;i<24;i++)
    {
        shift_idx = (23-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rFFrameHeaderInfo.u4TotalPacketNo;
    for(i=24;i<28;i++)
    {
        shift_idx = (27-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rFFrameFooterInfo.u4FootKey;
    for(i=12;i<16;i++)
    {
        shift_idx = (15-i)*8;
        *(puFrameFooterArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	
	return 0;
}

INT32 LDR_BEDFrameHeaderFooterGen(BE_D_FRAME_T rInputFrame, UINT8 *puFrameHeaderArray , UINT8 HeaderSize, UINT8 *puFrameFooterArray, UINT8 FooterSize)
{
	UINT8 i,shift_idx;
	UINT32 u4BeTmpData;

    if(HeaderSize < 32 || FooterSize <16)
    {
        return -1;
    }

	u4BeTmpData = rInputFrame.rDFrameHeaderInfo.u4Key;
    for(i=0;i<4;i++)
    {
        shift_idx = (3-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rDFrameHeaderInfo.u4HeaderSize;
    for(i=4;i<8;i++)
    {
        shift_idx = (7-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rDFrameHeaderInfo.u4FrameNo;
    for(i=16;i<20;i++)
    {
        shift_idx = (19-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rDFrameHeaderInfo.u4TotalFrameNo;
    for(i=20;i<24;i++)
    {
        shift_idx = (23-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rDFrameHeaderInfo.u4TotalPacketNo;
    for(i=24;i<28;i++)
    {
        shift_idx = (27-i)*8;
        *(puFrameHeaderArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }
	u4BeTmpData = rInputFrame.rDFrameFooterInfo.u4FootKey;
    for(i=12;i<16;i++)
    {
        shift_idx = (15-i)*8;
        *(puFrameFooterArray+i) = (UINT8)((u4BeTmpData>>shift_idx)&0x000000FF);
    }

	return 0;
}

#ifdef CC_MTK_LOADER
VOID LDR_BE_eMMC_HS_Mode(void)
{
    BELDBG("============BE_eMMC_HS_Mode_Set=============\n");
	#ifdef BE_EMMC_HS_MODE
    MsdcSetDataMode(3);
	MsdcSetBusClock(48);
	MsdcSetSpeedMode(1);
	MsdcSetDataBusWidth(8);
	#endif
}

VOID LDR_BE_eMMC_Recovery(void)
{
    BELDBG("============BE_eMMC_HS_Mode_Recovery=============\n");
	#ifdef BE_EMMC_HS_MODE
    MsdcSetDataMode(1);
	MsdcSetBusClock(24);
	MsdcSetSpeedMode(0);
	MsdcSetDataBusWidth(4);
	#endif
}
#endif

#ifdef CC_MTK_LOADER
VOID LDR_BE_Reset(void)
{
	INT32 i4BootPwrPin,i4BootPwrPolarity;

    BELDBG("============LDR_BE_Reset=============\n");
	i4BootPwrPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnGpio));
	i4BootPwrPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnPolarity));
	GPIO_SetOut(i4BootPwrPin,(1-i4BootPwrPolarity));
	x_thread_delay(20);
	GPIO_SetOut(i4BootPwrPin,i4BootPwrPolarity);
}
BOOL LDR_BE_Support(void)
{
    UINT32 u4Val;
	
	#ifdef CC_COMBINE_4K_2K
	UNUSED(DRVCUST_InitQuery(eGet2KModelSupport, &u4Val));
	//BIT0: (0/1) (2K/4K)
	//BIT1: (0/1) (wo BE or FRC / w BE or FRC)
	//BIT5: (0/1) (BE or FRC normal / BE or FRC stand_along)	
	if((u4Val&0x00000001) == 0x00000000)
	{
	  return FALSE;
	}
	else
	{
	  return TRUE;
	}
	#else
    EEPROM_Read(BE_IC_SUPPORT_ADDR, (UINT32)&u4Val, 1);
	if(((UINT8)u4Val) != 0x79)
	{
	  Printf("LDR_BE_Support Model not support BE!!");
      return FALSE;
	}
	return TRUE;
    #endif
}

#endif

//-----------------------------------------------------------------------------
/** LDR_BE_Init(): Do necessary setup settings for BEIC data transferring
 *  This function is going to setup boot settings, such as GPIO_Init() and register call back
 *
 *  @param  prLdrData       The loader global data structure pointer.
 *  @retval 0               Success.
 *  @retval otherwise       Failed.
 */
//-----------------------------------------------------------------------------
//void BEVx1Request(INT32 i4Gpio, BOOL fgStatus)
//{
//    UNUSED(i4Gpio);
//    UNUSED(fgStatus);
//}
UINT32 LDR_BE_Init(LDR_DATA_T *prLdrData)
{
    INT32 i4Ret;
    UINT8 au1Header[128]={0};
	UINT8 au1Footer[16]={0};
	UINT8 u1StateFrmInfo;
	UINT32 i,j;
	BE_FRAME_CONFIG_T* prBeFrmCfgTable;
	UINT32 u4BeFrmCfgTblSize;
    BE_I_FRAME_T rIFrame00;
	BE_F_FRAME_T rFFrame00;
	BE_D_FRAME_T rDFrame00;

    GPIO_Init();
	
    #ifdef CC_MTK_LOADER
    LDR_BE_Reset();
    #endif

	i4Ret = DRVCUST_InitQuery(eBELFrameCfgTbl, (UINT32 *)(void *)&prBeFrmCfgTable);
    if(i4Ret != 0)
    {
        Printf("LDR_BE_Init Get Parameters Fail!!\n");
		return 0;
    }
	i4Ret = DRVCUST_InitQuery(eBELFrameCfgTblSize, &u4BeFrmCfgTblSize);
    if(i4Ret != 0)
    {
        Printf("LDR_BE_Init Get Parameters Fail!!\n");
		return 0;
    }
	u1TotalDisplayFrmNo = (UINT8)u4BeFrmCfgTblSize;
	if(u4BeFrmCfgTblSize > LOADER_BE_TOTAL_FRAME_NUMBER)
	{
	    Printf("LDR_BE_Init Frame size exceeds!!\n");
		return 0;
	}
	for(i=0;i<u4BeFrmCfgTblSize;i++)
	{
	    switch(prBeFrmCfgTable[i].u1FrameType)
	    {
	        case 0:
				rFrameHeaderInfo[i].rFrameMode = eIFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;			
				rIFrame00.rIFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rIFrame00.rIFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_32;
				rIFrame00.rIFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rIFrame00.rIFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rIFrame00.rIFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rIFrame00.rIFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEIFrameHeaderFooterGen(rIFrame00,au1Header,128,au1Footer,16);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 32);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }
				break;
	        case 1:
				rFrameHeaderInfo[i].rFrameMode = eFFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;
				rFFrame00.rFFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rFFrame00.rFFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_128;
				rFFrame00.rFFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rFFrame00.rFFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rFFrame00.rFFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rFFrame00.rFFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEFFrameHeaderFooterGen(rFFrame00,au1Header,128,au1Footer,16);	
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 128);
	            x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }				
				break;
			case 2:
				rFrameHeaderInfo[i].rFrameMode = eDFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;
				rDFrame00.rDFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rDFrame00.rDFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_128;
				rDFrame00.rDFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rDFrame00.rDFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rDFrame00.rDFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rDFrame00.rDFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEDFrameHeaderFooterGen(rDFrame00,au1Header,128,au1Footer,16);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 128);
	            x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }				
				break;
			default:
				Printf("LDR_BE_Init Frame type error!!\n");
				break;
	    }
		rOutputFrm[i].u1FrameNum = prBeFrmCfgTable[i].u1FrmNo;
		rOutputFrm[i].u1State = prBeFrmCfgTable[i].u1BootState;
	    //u1StateFrmInfo = (((rOutputFrm[i].u1State)&0x0F)<<4)+((rOutputFrm[i].u1FrameNum)&0x0F);
		u1StateFrmInfo = ((rOutputFrm[i].u1State)&0x0F);
		rFrameHeaderInfo[i].FrameHeader[15] = u1StateFrmInfo;
		u1StateFrmInfo = 0x00;
	}
	#ifdef BE_LOADER_DBG
	BELDBG("============LDR_BE_Init=============\n");
	for(i=0;i<u4BeFrmCfgTblSize;i++)
	{
	  BELDBG("rFrameHeaderInfo[%d] mode = %d\n",i,rFrameHeaderInfo[i].rFrameMode);
	  BELDBG("rFrameHeaderInfo[%d] header =\n",i);
	  for(j=0;j<16;j++)
	  {
	    BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].FrameHeader[j*8],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+1],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+2],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+3],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+4],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+5],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+6],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+7]);
	  }
	  BELDBG("rFrameHeaderInfo[%d] FrmContainer =\n",i);
	  BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].rFrameContainer[0],
	  	     rFrameHeaderInfo[i].rFrameContainer[1],
	  	     rFrameHeaderInfo[i].rFrameContainer[2],
	  	     rFrameHeaderInfo[i].rFrameContainer[3],
	  	     rFrameHeaderInfo[i].rFrameContainer[4],
	  	     rFrameHeaderInfo[i].rFrameContainer[5],
	  	     rFrameHeaderInfo[i].rFrameContainer[6]);
	  BELDBG("rFrameHeaderInfo[%d] footer =\n",i);
	  for(j=0;j<2;j++)
	  {
	    BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].FrameFooter[j*8],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+1],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+2],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+3],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+4],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+5],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+6],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+7]);
	  }
	  BELDBG("outputframe (%d) = 0x%x 0x%x\n",i,rOutputFrm[i].u1State, rOutputFrm[i].u1FrameNum);
	}	
	#endif
    #ifdef CC_MTK_LOADER
    #ifdef BE_PACKET_FLAGS_SUPPORT
	for(i=0;i<32;i++)
	{
      rDeletTypeInfo[i].ui1PartitionId = 0xFF;
	  rDeletTypeInfo[i].ui4PacketFlag = 0xFFFFFFFF;
	}
    #endif
    #endif
    return 0;
}
#ifndef CC_MTK_LOADER
UINT32 BE_Suspend_Init(void)
{
    INT32 i4Ret;
    UINT8 au1Header[128]={0};
	UINT8 au1Footer[16]={0};
	UINT8 u1StateFrmInfo;	
	UINT32 i,j;
	BE_FRAME_CONFIG_T* prBeFrmCfgTable;
	UINT32 u4BeFrmCfgTblSize;
    BE_I_FRAME_T rIFrame00;
	BE_F_FRAME_T rFFrame00;
	BE_D_FRAME_T rDFrame00;

	i4Ret = DRVCUST_InitQuery(eBELFrameCfgTbl, (UINT32 *)(void *)&prBeFrmCfgTable);
    if(i4Ret != 0)
    {
        Printf("LDR_BE_Init Get Parameters Fail!!\n");
		return 0;
    }
	i4Ret = DRVCUST_InitQuery(eBELFrameCfgTblSize, &u4BeFrmCfgTblSize);
    if(i4Ret != 0)
    {
        Printf("LDR_BE_Init Get Parameters Fail!!\n");
		return 0;
    }
	u1TotalDisplayFrmNo = (UINT8)u4BeFrmCfgTblSize;
	if(u4BeFrmCfgTblSize > LOADER_BE_TOTAL_FRAME_NUMBER)
	{
	    Printf("LDR_BE_Init Frame size exceeds!!\n");
		return 0;
	}
	for(i=0;i<u4BeFrmCfgTblSize;i++)
	{
	    switch(prBeFrmCfgTable[i].u1FrameType)
	    {
	        case 0:
				rFrameHeaderInfo[i].rFrameMode = eIFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;			
				rIFrame00.rIFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rIFrame00.rIFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_32;
				rIFrame00.rIFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rIFrame00.rIFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rIFrame00.rIFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rIFrame00.rIFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEIFrameHeaderFooterGen(rIFrame00,au1Header,128,au1Footer,16);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 32);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }
				break;
	        case 1:
				rFrameHeaderInfo[i].rFrameMode = eFFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;
				rFFrame00.rFFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rFFrame00.rFFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_128;
				rFFrame00.rFFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rFFrame00.rFFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rFFrame00.rFFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rFFrame00.rFFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEFFrameHeaderFooterGen(rFFrame00,au1Header,128,au1Footer,16);	
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 128);
	            x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }				
				break;
			case 2:
				rFrameHeaderInfo[i].rFrameMode = eDFrame_Header_mode;
				rFrameHeaderInfo[i].u1State = prBeFrmCfgTable[i].u1BootState;
				rDFrame00.rDFrameHeaderInfo.u4Key = prBeFrmCfgTable[i].u4HKey;
				rDFrame00.rDFrameHeaderInfo.u4HeaderSize = LOADER_BE_BOOT_HEADER_SIZE_128;
				rDFrame00.rDFrameHeaderInfo.u4FrameNo = (UINT32)prBeFrmCfgTable[i].u1FrmNo;
				rDFrame00.rDFrameHeaderInfo.u4TotalFrameNo = (UINT32)prBeFrmCfgTable[i].u1TotalFrmNo;
				rDFrame00.rDFrameHeaderInfo.u4TotalPacketNo = (UINT32)prBeFrmCfgTable[i].u1TotalPktNo;
				rDFrame00.rDFrameFooterInfo.u4FootKey = prBeFrmCfgTable[i].u4FKey;
				LDR_BEDFrameHeaderFooterGen(rDFrame00,au1Header,128,au1Footer,16);
				x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameHeader[0])), (void*)au1Header, 128);
	            x_memcpy((void*)(&(rFrameHeaderInfo[i].FrameFooter[0])), (void*)au1Footer, 16);
				for(j=0;j<8;j++)
				{
	                rFrameHeaderInfo[i].rFrameContainer[j] = prBeFrmCfgTable[i].u1PktContainer[j];
                }				
				break;
			default:
				Printf("LDR_BE_Init Frame type error!!\n");
				break;
	    }
		rOutputFrm[i].u1FrameNum = prBeFrmCfgTable[i].u1FrmNo;
		rOutputFrm[i].u1State = prBeFrmCfgTable[i].u1BootState;
	    u1StateFrmInfo = (((rOutputFrm[i].u1State)&0x0F)<<4)+((rOutputFrm[i].u1FrameNum)&0x0F);
		rFrameHeaderInfo[i].FrameHeader[15] = u1StateFrmInfo;
		u1StateFrmInfo = 0x00;		
	}
	#ifdef BE_LOADER_DBG
	BELDBG("============LDR_BE_Init=============\n");
	for(i=0;i<u4BeFrmCfgTblSize;i++)
	{
	  BELDBG("rFrameHeaderInfo[%d] mode = %d\n",i,rFrameHeaderInfo[i].rFrameMode);
	  BELDBG("rFrameHeaderInfo[%d] header =\n",i);
	  for(j=0;j<16;j++)
	  {
	    BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].FrameHeader[j*8],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+1],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+2],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+3],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+4],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+5],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+6],
	  	     rFrameHeaderInfo[i].FrameHeader[(j*8)+7]);
	  }
	  BELDBG("rFrameHeaderInfo[%d] FrmContainer =\n",i);
	  BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].rFrameContainer[0],
	  	     rFrameHeaderInfo[i].rFrameContainer[1],
	  	     rFrameHeaderInfo[i].rFrameContainer[2],
	  	     rFrameHeaderInfo[i].rFrameContainer[3],
	  	     rFrameHeaderInfo[i].rFrameContainer[4],
	  	     rFrameHeaderInfo[i].rFrameContainer[5],
	  	     rFrameHeaderInfo[i].rFrameContainer[6]);
	  BELDBG("rFrameHeaderInfo[%d] footer =\n",i);
	  for(j=0;j<2;j++)
	  {
	    BELDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	  	     rFrameHeaderInfo[i].FrameFooter[j*8],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+1],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+2],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+3],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+4],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+5],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+6],
	  	     rFrameHeaderInfo[i].FrameFooter[(j*8)+7]);
	  }
	  BELDBG("outputframe (%d) = 0x%x 0x%x\n",i,rOutputFrm[i].u1State, rOutputFrm[i].u1FrameNum);
	}	
	#endif
    return 0;
}
#endif

#ifdef BE_ARGB_RE_ORDER
//-----------------------------------------------------------------------------
/** LDR_BESetOsdOrder(): Setup BE OSD ARGB order.
 *  This function is going to setup BE IC and osd driver with VBx1 data order.
 *
 */
//--------------------------------------------------------------------------------
UINT32 LDR_BESetOsdOrder(UINT32 AOrder, UINT32 ROrder, UINT32 GOrder, UINT32 BOrder)
{
    UINT32 u4ChkSum;

    u4ChkSum = AOrder+ROrder+GOrder+BOrder;
    if(u4ChkSum != 6)
    {
        Printf("LDR_BESetOsdOrder Fail(%d/%d/%d/%d)\n",AOrder,ROrder,GOrder,BOrder);
		return 1;
    }
	else
	{
        rARGBOsdOrder.ui4Alpha = AOrder;
	    rARGBOsdOrder.ui4Red = ROrder;
	    rARGBOsdOrder.ui4Green = GOrder;
	    rARGBOsdOrder.ui4Blue = BOrder;
	}
    return 0;
}

//-----------------------------------------------------------------------------
/** LDR_BEGetOsdOrder(): Get BE OSD ARGB order.
 *  This function is going to setup BE IC and osd driver with VBx1 data order.
 *
 */
//--------------------------------------------------------------------------------
UINT32 LDR_BEGetOsdOrder(BE_OSD_ORDER_T* prBeOsdOder)
{
    UINT32 u4ChkSum;

    u4ChkSum = rARGBOsdOrder.ui4Alpha + rARGBOsdOrder.ui4Red + rARGBOsdOrder.ui4Green + rARGBOsdOrder.ui4Blue;
    if(u4ChkSum != 6)
    {
        Printf("LDR_BEGetOsdOrder Fail(%d/%d/%d/%d)\n",rARGBOsdOrder.ui4Alpha,rARGBOsdOrder.ui4Red,
			                                rARGBOsdOrder.ui4Green,rARGBOsdOrder.ui4Blue);
		return 1;
    }
	else
	{
        prBeOsdOder->ui4Alpha = rARGBOsdOrder.ui4Alpha;
	    prBeOsdOder->ui4Red = rARGBOsdOrder.ui4Red;
	    prBeOsdOder->ui4Green = rARGBOsdOrder.ui4Green;
	    prBeOsdOder->ui4Blue = rARGBOsdOrder.ui4Blue;
	}
    return 0;
}
#endif

//-----------------------------------------------------------------------------
/** LDR_BEOsdDisplay(): Setup BE via OSD driver.
 *  This function is going to setup BE IC and osd driver with VBx1 data.
 *
 *  @param  u4VBx1Addr      The VBx1 data pointer.
 *  @param  u4VBx1Size      The VBx1 actual data size.
 *  @param  u4Width         The width info of the VBx1 data.
 *  @param  u4Height        The height info of the VBx1 data.
 *  @param  u1OpMode        The operation mode of transmission.
 *  @retval 0               Success.
 *  @retval otherwise       Failed.
 */
//-----------------------------------------------------------------------------
UINT32 LDR_BEOsdDisplay(UINT32 u4ColorMode, UINT32 u4VBx1Addr, UINT32 u4VBx1Size, UINT32 u4Width, UINT32 u4Height,UINT8 u1OpMode)
{
    UINT32 u4RegionList0, u4RegionList1, u4Region0, u4Region1, u4BmpPitch;
    UINT32 u4PanelWidth, u4PanelHeight;
	UINT32 u4VBx1AuxAddr=0;
    UINT32 u4MainOsdPlane = OSD_PLANE_2;
	UINT32 u4AuxOsdPlane = OSD_PLANE_3;
	UINT32 u4ScalerMain = OSD_SCALER_2;
	UINT32 u4ScalerAux = OSD_SCALER_3;	
    UINT32 ui4OSDMonitorReg = 0;
    INT32 ret;
	#ifdef BE_ARGB_RE_ORDER
	UINT32 u4OrderRet;
	BE_OSD_ORDER_T rArgbOrder;
    #endif
	
    BELDBG("Color:%d BmpAddr:0x%08x Width:%d Height:%d\n", u4ColorMode, u4VBx1Addr, u4Width, u4Height);
    OSD_Init();
    ui4OSDMonitorReg = IO_READ32MSK(BE_OSD_BASE_MONITOR, 0, BE_UPDATE_TIMING_BIT);
    IO_WRITE32MSK(BE_OSD_BASE_MONITOR, 0, BE_UPDATE_TIMING_BIT, BE_UPDATE_TIMING_BIT);
    OSD_UpdateReg();
    u4PanelWidth = PANEL_GetPanelWidth();
    u4PanelHeight = PANEL_GetPanelHeight();

	if(u1OpMode == 1)
	{
	  u4VBx1AuxAddr = u4VBx1Addr + u4VBx1Size;
	  BELDBG("Main VBx1 addr = 0x%08x, Aux VBx1 addr = 0x%08x\n", u4VBx1Addr, u4VBx1AuxAddr);
	}

    BELDBG("Panel %d x %d \n", u4PanelWidth, u4PanelHeight);
    ret = OSD_RGN_LIST_Create(&u4RegionList0);
    if (ret != OSD_RET_OK) return 1;
	if(u1OpMode == 1)
	{
      ret = OSD_RGN_LIST_Create(&u4RegionList1);
      if (ret != OSD_RET_OK) return 1;	
	}
    u4BmpPitch = 0;

	if(u4Width == 1366)
	{
		OSD_GET_PITCH_SIZE(u4ColorMode, u4Width+2, u4BmpPitch); // to set u4BmpPitch by u4ColorMode and u4Width.
	}
	else
	{
		OSD_GET_PITCH_SIZE(u4ColorMode, u4Width, u4BmpPitch);
	}
			
    BELDBG("\n*************u4BmpPitch=%d******************\n",u4BmpPitch);

    OSD_GET_PITCH_SIZE(u4ColorMode, u4Width, u4BmpPitch); // to set u4BmpPitch by u4ColorMode and u4Width.
    u4BmpPitch = (u4BmpPitch +0xF) & (~0xF);
    ret = OSD_RGN_Create(&u4Region0, u4Width, u4Height, (void *)PHYSICAL(u4VBx1Addr),
                            u4ColorMode, u4BmpPitch, 0, 0, u4Width, u4Height);
	OSD_RGN_Set(u4Region0, OSD_RGN_MIX_SEL, 1);
    if (ret != OSD_RET_OK) return 2;
	if(u1OpMode == 1)
	{
	  if(u4PanelWidth <= u4Width)
	  {
	    ret = OSD_RET_OUT_OF_REGION;
	  }
	  else
	  {
        ret = OSD_RGN_Create(&u4Region1, (u4PanelWidth-u4Width), u4Height, (void *)PHYSICAL(u4VBx1AuxAddr),
                               u4ColorMode, u4BmpPitch, u4Width, 0, (u4PanelWidth-u4Width), u4Height);
		OSD_RGN_Set(u4Region1, OSD_RGN_MIX_SEL, 1);

	  }
      if (ret != OSD_RET_OK) return 2;	
	}
    #ifdef BE_ARGB_RE_ORDER
    u4OrderRet = LDR_BEGetOsdOrder(&rArgbOrder);
    if(!u4OrderRet)
    {
	  if(u1OpMode == 1)
	  {
	    OSD_RGN_SetColorOrder(u4Region0,rArgbOrder.ui4Alpha,rArgbOrder.ui4Red,rArgbOrder.ui4Green,rArgbOrder.ui4Blue);
        OSD_RGN_SetColorOrder(u4Region1,rArgbOrder.ui4Alpha,rArgbOrder.ui4Red,rArgbOrder.ui4Green,rArgbOrder.ui4Blue);		
	  }
	  else
	  {
		OSD_RGN_SetColorOrder(u4Region0,rArgbOrder.ui4Alpha,rArgbOrder.ui4Red,rArgbOrder.ui4Green,rArgbOrder.ui4Blue);
	  }
    }
	#endif
	if(u1OpMode == 1)
	{
      ret = OSD_RGN_Insert(u4Region0, u4RegionList0);
      if (ret != OSD_RET_OK) return 3;
      ret = OSD_RGN_Insert(u4Region1, u4RegionList1);
      if (ret != OSD_RET_OK) return 3;	  
      ret = OSD_PLA_FlipTo(OSD_PLANE_3, u4RegionList0);
      if (ret != OSD_RET_OK) return 4;
      ret = OSD_PLA_FlipTo(OSD_PLANE_2, u4RegionList1);
      if (ret != OSD_RET_OK) return 4;	

	}
	else
	{
      ret = OSD_RGN_Insert(u4Region0, u4RegionList0);
      if (ret != OSD_RET_OK) return 3;
      ret = OSD_PLA_FlipTo(OSD_PLANE_2, u4RegionList0);
      if (ret != OSD_RET_OK) return 4;
	}
    ret = OSD_SC_Scale(u4ScalerMain, 1, u4PanelWidth, u4PanelHeight, u4PanelWidth,
            u4PanelHeight);
    if (ret != OSD_RET_OK) return 5;
	if(u1OpMode == 1)
	{
      ret = OSD_SC_Scale(u4ScalerAux, 1, u4PanelWidth, u4PanelHeight, u4PanelWidth,
              u4PanelHeight);
      if (ret != OSD_RET_OK) return 5;	
	}
	#if 0
    ret = OSD_RGN_Set(u4Region0, OSD_RGN_POS_X,0);
    if (ret != OSD_RET_OK)
        return 6;
    ret = OSD_RGN_Set(u4Region0, OSD_RGN_POS_Y,0);
    if (ret != OSD_RET_OK)
        return 7;
	#endif
    HalFlushDCache();
	#ifdef BE_OSD_SHIFT
	if(!fgShiftUpdated)
	{
	  OSD_Base_AdjustOSDPosition(i4PixelShift);
	  OSD_Base_AdjustOSDVPosition(i4VPixelShift);
	}
    #else
	if(!fgShiftUpdated)
	{
	  OSD_Base_AdjustOSDPosition(i4PixelShift);
	  OSD_Base_AdjustOSDVPosition(i4VPixelShift);
	  fgShiftUpdated = TRUE;
	}
	#endif
	OSD_PLA_SetPremulEn(u4MainOsdPlane, 1);
    ret = OSD_PLA_FlipTo(u4MainOsdPlane, u4RegionList0);
    if (ret != OSD_RET_OK) return 4; 
    ret = OSD_PLA_Enable(u4MainOsdPlane, TRUE);
    if (ret != OSD_RET_OK) return 8;
	if(u1OpMode == 1)
	{
      ret = OSD_PLA_FlipTo(u4AuxOsdPlane, u4RegionList1);
      if (ret != OSD_RET_OK) return 4;
	  OSD_PLA_SetPremulEn(u4AuxOsdPlane, 1);
      ret = OSD_PLA_Enable(u4AuxOsdPlane, TRUE);
      if (ret != OSD_RET_OK) return 8;
	}
    IO_WRITE32MSK(OSD_BASE_MONITOR, 0, ui4OSDMonitorReg, UPDATE_TIMING_BIT);
    OSD_UpdateReg();
    return 0;
}

#endif /* LOADER_LOGO_NUMBER */
//-----------------------------------------------------------------------------
/** LDR_BEDisplay(): Setup BE via OSD driver.
 *  This function is going to setup BE IC and osd driver with VBx1 data.
 *
 */
//--------------------------------------------------------------------------------
UINT32 LDR_BEDisplay(BE_FBM_INFO_T rBEFbmInfo)
{
    INT32 i4Ret;
	#ifdef BE_OSD_SHIFT
	UINT32 u4Polarity,u4Val;
    #endif

    //BELDBG("LDR_BEDisplay: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    if((rBEFbmInfo.ui4DisplayFBMSize == 0)||(rBEFbmInfo.ui4DisplayFBMAddr == 0))
    {
        Printf("LDR_BEDisplay: Fail due to FBM data corruption!!\n");
        return 0xFFFFFFFF;
    }
    //LDR_PmxDisplay(DRVCUST_InitGet(eLoaderLogoBackground));
	#ifdef BE_LOADER_DBG
	vGetPostScalerStatus();
	vDrvVOPLLStatus();
	vErrorHandleStatus();
	DumpPanelAttribute(0);
	#endif

	#ifdef BE_OSD_SHIFT
    if((EEPROM_Read(BE_IC_OSD_PIXEL_SHIFT_POLARITY, (UINT32)&u4Polarity, 1) != 0) || 
	    (EEPROM_Read(BE_IC_OSD_PIXEL_SHIFT, (UINT32)&u4Val, 1) != 0))
	{
	  Printf("LDR_BEDisplay: OSD shift read fail set to 0!!\n");
	  i4PixelShift = 0;
	}
	else
	{
	  u4Val = (u4Val&0x000000FF);
      if(u4Polarity != 0)
      {
        i4Ret = (INT32)u4Val; //right side
      }
	  else
	  {
	    i4Ret = (INT32)(0-u4Val); //left side
	  }
      if (i4PixelShift == i4Ret)
      {
        fgShiftUpdated = TRUE;
      }
	  else
	  {
	    fgShiftUpdated = FALSE;
		i4PixelShift = i4Ret;
	  }	  
	}
    if((EEPROM_Read(BE_IC_OSD_PIXEL_SHIFT_POLARITY+2, (UINT32)&u4Polarity, 1) != 0) || 
	    (EEPROM_Read(BE_IC_OSD_PIXEL_SHIFT+2, (UINT32)&u4Val, 1) != 0))
	{
	  Printf("LDR_BEDisplay: OSD V shift read fail set to 0!!\n");
	  i4VPixelShift = 0;
	}
	else
	{
	  u4Val = (u4Val&0x000000FF);
      if(u4Polarity != 0)
      {
        i4Ret = (INT32)u4Val; //right side
      }
	  else
	  {
	    i4Ret = (INT32)(0-u4Val); //left side
	  }
      if (i4VPixelShift == i4Ret)
      {
        fgShiftUpdated = TRUE;
      }
	  else
	  {
	    fgShiftUpdated = FALSE;
		i4VPixelShift = i4Ret;
	  }	  
	}	
	#endif
	i4Ret = (INT32)LDR_BEOsdDisplay(OSD_CM_ARGB8888_DIRECT32, rBEFbmInfo.ui4DisplayFBMAddr, 
		                            rBEFbmInfo.ui4DisplayFBMSize, rBEFbmInfo.ui4DisplayAreaWidth, 
		                            rBEFbmInfo.ui4DisplayAreaHeight,rBEFbmInfo.ui1DisplayMode);
    //BELDBG("LDR_BEDisplay: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    return (UINT32)i4Ret;
}


//-----------------------------------------------------------------------------
/** LDR_BEFrameBuffer(): Setup BE Frame via DRAM buffer.
 *
 */
//--------------------------------------------------------------------------------
BE_FBM_INFO_T LDR_BEFrameBufferGet(UINT8 ui1FrameIdx, UINT32 u4Offset)
{
#ifdef CC_MTK_LOADER
	INT32 i4Ret;
    #ifdef BE_BOOT_OSD_TWO_BUF_MODE
    UINT8 *puVBx1FirstBuf = (UINT8 *)0x0D000000;
	UINT8 *puVBx1SecondBuf = (UINT8 *)0x0D800000;
	UINT8 *puVBx1BufTmp = (UINT8 *)0x0E000000;
	UINT8 *puVx1Buffer = (UINT8 *)0x0E000000;
	#else
	UINT8 *puVBx1Buf = (UINT8 *)0x0D000000;
	UINT8 *puVBx1BufTmp = (UINT8 *)0x0D800000;
	UINT8 *puVx1Buffer = (UINT8 *)0x0D000000;
	#endif
	UINT32 u4VBx1BufferSize,u4VBx1PartId;
	UINT32 w, h;
	BE_BOOT_FLASH_INFO_T rBeFlashInfo;
	UINT32 ui4HeaderSize,ui4FrameSize,ui4PacketNo,ui4PacketSize[7],i,ui4PacketHeaderSize[7];
        UINT32 ui4BlankPacketNo;
	UINT8  uiTmp0,uiTmp1,uiTmp2,uiTmp3;
	UINT16 ui2CRCchecksum;
	UINT32 ui4BootTimeInfo=0;
	#ifdef BE_PACKET_FLAGS_SUPPORT
	UINT32 ui4PacketFlags=0;
	#endif
#endif
	BE_FBM_INFO_T rBEFbmInfo={0};
	
#ifdef CC_MTK_LOADER	
    BELDBG("LDR_BEFrameBuffer: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	w = PANEL_GetPanelWidth();
    h = PANEL_GetPanelHeight();
	#ifdef BE_BOOT_OSD_TWO_BUF_MODE
	if(!_fgSecondBufused)
	{
		x_memset((void*)puVBx1FirstBuf,0,0x800000);
		x_memset((void*)puVBx1BufTmp,0,0x800000);
	}
	else
	{
	    x_memset((void*)puVBx1SecondBuf,0,0x800000);
	    x_memset((void*)puVBx1BufTmp,0,0x800000);	
	}
	#else
	x_memset((void*)puVBx1Buf,0,0x800000);
	x_memset((void*)puVBx1BufTmp,0,0x800000);
    #endif
	
    if(ui1FrameIdx>u1TotalDisplayFrmNo-1)
    {
      return rBEFbmInfo;
    }

    uiTmp0 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[4];
    uiTmp1 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[5];		
    uiTmp2 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[6];
    uiTmp3 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[7];
    ui4HeaderSize = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
	//BELDBG("LDR_BEFrameBuffer: ui4HeaderSize=0x%08x\n",ui4HeaderSize);
    uiTmp0 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[24];
    uiTmp1 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[25];		
    uiTmp2 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[26];
    uiTmp3 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[27];
	ui4PacketNo = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
	//BELDBG("LDR_BEFrameBuffer: ui4PacketNo=0x%08x\n",ui4PacketNo);
    ui4BlankPacketNo = 0;
	ui4FrameSize = 0;
	#ifdef BE_BOOT_OSD_TWO_BUF_MODE
	if(!_fgSecondBufused)
	{	
	  puVx1Buffer = puVBx1FirstBuf + ui4HeaderSize;  
	}
	else
	{
	  puVx1Buffer = puVBx1SecondBuf + ui4HeaderSize;
	}
	#else
	puVx1Buffer = puVBx1Buf+ui4HeaderSize;
	#endif
    for(i=0;i<ui4PacketNo;i++)
    {
        BELDBG("LDR_BEFrameBuffer: parsing loop start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
        rBeFlashInfo = LDR_BEFlashPartitionTranslate(rFrameHeaderInfo[ui1FrameIdx].rFrameContainer[i],u4Offset);
		u4VBx1BufferSize = rBeFlashInfo.u4Size;
		u4VBx1PartId = rBeFlashInfo.u4PartId;	
        #if defined(CC_EMMC_BOOT)
		i4Ret = Loader_ReadMsdc(u4VBx1PartId, 0, puVBx1BufTmp, u4VBx1BufferSize);
        #elif defined(CC_NAND_BOOT) 
	    i4Ret = Loader_ReadNandFlash(u4VBx1PartId, 0, puVBx1BufTmp, u4VBx1BufferSize);
        #endif
		BELDBG("LDR_BEFrameBuffer: parsing loop flash read end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		uiTmp0 = *(puVBx1BufTmp+16);
		uiTmp1 = *(puVBx1BufTmp+17);
		uiTmp2 = *(puVBx1BufTmp+18);
		uiTmp3 = *(puVBx1BufTmp+19);
		ui4PacketSize[i] = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
		//BELDBG("LDR_BEFrameBuffer: ui4PacketSize=0x%08x\n",ui4PacketSize[i]);
		uiTmp0 = *(puVBx1BufTmp+4);
		uiTmp1 = *(puVBx1BufTmp+5);
		uiTmp2 = *(puVBx1BufTmp+6);
		uiTmp3 = *(puVBx1BufTmp+7);
		ui4PacketHeaderSize[i] = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
		//BELDBG("LDR_BEFrameBuffer: ui4PacketHeaderSize=0x%08x\n",ui4PacketHeaderSize[i]);
		uiTmp0 = *(puVBx1BufTmp+12);
		uiTmp1 = *(puVBx1BufTmp+13);
		uiTmp2 = *(puVBx1BufTmp+14);
		uiTmp3 = *(puVBx1BufTmp+15);
		ui4BootTimeInfo = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
        #ifdef BE_PACKET_FLAGS_SUPPORT
		uiTmp0 = *(puVBx1BufTmp+8);
		uiTmp1 = *(puVBx1BufTmp+9);
		uiTmp2 = *(puVBx1BufTmp+10);
		uiTmp3 = *(puVBx1BufTmp+11);
		ui4PacketFlags = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
		rDeletTypeInfo[ui1DelTypeIndex].ui1PartitionId = rFrameHeaderInfo[ui1FrameIdx].rFrameContainer[i];
		rDeletTypeInfo[ui1DelTypeIndex].ui4PacketFlag = ui4PacketFlags;
		ui1DelTypeIndex++;
		#endif
		#ifdef BE_DELETE_UPGRADE_SUPPORT
		if((ui4PacketHeaderSize[i] != 0) && (ui4PacketSize[i] != 0))
		{
	        x_memcpy((void*)puVx1Buffer,(void*)puVBx1BufTmp,(ui4PacketSize[i]+ui4PacketHeaderSize[i]));
		    BELDBG("LDR_BEFrameBuffer: parsing loop memcpy end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		    puVx1Buffer = puVx1Buffer + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		    ui4FrameSize = ui4FrameSize + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		    x_memset((void*)puVBx1BufTmp,0,(w*h*4));		
		    BELDBG("LDR_BEFrameBuffer: parsing loop end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));		
		}
		else
		{
		    ui4BlankPacketNo++;
		    Printf("LDR_BEFrameBuffer: packet (%d) is delete type\n",u4VBx1PartId);
		}
		#else
	    x_memcpy((void*)puVx1Buffer,(void*)puVBx1BufTmp,(ui4PacketSize[i]+ui4PacketHeaderSize[i]));
		BELDBG("LDR_BEFrameBuffer: parsing loop memcpy end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		puVx1Buffer = puVx1Buffer + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		ui4FrameSize = ui4FrameSize + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		x_memset((void*)puVBx1BufTmp,0,(w*h*4));		
		BELDBG("LDR_BEFrameBuffer: parsing loop end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		#endif
    }
    BELDBG("LDR_BEFrameBuffer: endpoint1=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    uiTmp0 = (UINT8)((ui4FrameSize&0xFF000000)>>24);
	uiTmp1 = (UINT8)((ui4FrameSize&0x00FF0000)>>16); 
	uiTmp2 = (UINT8)((ui4FrameSize&0x0000FF00)>>8); 
	uiTmp3 = (UINT8)(ui4FrameSize&0x000000FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[11] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[10] = uiTmp2;
    rFrameHeaderInfo[ui1FrameIdx].FrameHeader[9] = uiTmp1;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[8] = uiTmp0;
    #ifdef BE_SUPPORT_FRM_HDR_MISC_INFO
    BEGetFrmHdrMiscInfo(ui1FrameIdx);
    #endif
    ui2CRCchecksum = crc16_ccitt(rFrameHeaderInfo[ui1FrameIdx].FrameHeader,(ui4HeaderSize-2));
	uiTmp2 = (UINT8)((ui2CRCchecksum&0xFF00)>>8); 
	uiTmp3 = (UINT8)(ui2CRCchecksum&0x00FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[ui4HeaderSize-1] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[ui4HeaderSize-2] = uiTmp2;
	BELDBG("LDR_BEFrameBuffer: CRC=0x%04x\n",ui2CRCchecksum);
	BELDBG("LDR_BEFrameBuffer: endpoint2=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    #ifdef BE_PACKET_FLAGS_SUPPORT
	Printf("LDR_BEFrameBuffer: PacketFlags=0x%08x\n",ui4PacketFlags);
	#endif
	#ifdef BE_DELETE_UPGRADE_SUPPORT
	//update total packet no in frame header
    ui4PacketNo=ui4PacketNo-ui4BlankPacketNo;
    uiTmp0 = (UINT8)((ui4PacketNo&0xFF000000)>>24);
	uiTmp1 = (UINT8)((ui4PacketNo&0x00FF0000)>>16); 
	uiTmp2 = (UINT8)((ui4PacketNo&0x0000FF00)>>8); 
	uiTmp3 = (UINT8)(ui4PacketNo&0x000000FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[27] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[26] = uiTmp2;
    rFrameHeaderInfo[ui1FrameIdx].FrameHeader[25] = uiTmp1;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[24] = uiTmp0;
	#endif
    for(i=0;i<ui4HeaderSize;i++)
    {
        #ifdef BE_BOOT_OSD_TWO_BUF_MODE
		if(!_fgSecondBufused)
		{	
		  *(puVBx1FirstBuf+i)=rFrameHeaderInfo[ui1FrameIdx].FrameHeader[i]; 
		}
		else
		{
		  *(puVBx1SecondBuf+i)=rFrameHeaderInfo[ui1FrameIdx].FrameHeader[i];
		}
        #else
		*(puVBx1Buf+i)=rFrameHeaderInfo[ui1FrameIdx].FrameHeader[i];
        #endif
    }
	
    #ifdef BE_BOOT_OSD_TWO_BUF_MODE
	if(!_fgSecondBufused)
	{
	    if(puVx1Buffer > puVBx1FirstBuf + 0x800000)
	    {
	        Printf("LDR_BEFrameBufferGet: Fail due to (1st buf)frame size overflow!!\n");
			return rBEFbmInfo;
	    }
	}
	else
	{
	    if(puVx1Buffer > puVBx1SecondBuf + 0x800000)
	    {
	        Printf("LDR_BEFrameBufferGet: Fail due to (2nd buf)frame size overflow!!\n");
			return rBEFbmInfo;	    
	    }
	}
    #else
	if(puVx1Buffer > puVBx1Buf + 0x800000)
	{
	    Printf("LDR_BEFrameBufferGet: Fail due to frame size overflow!!\n");
	    return rBEFbmInfo;	
	}
    #endif

    for(i=0;i<16;i++)
    {
        *(puVx1Buffer+i)=rFrameHeaderInfo[ui1FrameIdx].FrameFooter[i];
    }
    puVx1Buffer = puVx1Buffer + 16;

    #ifdef BE_BOOT_OSD_TWO_BUF_MODE
	if(!_fgSecondBufused)
	{
	    rBEFbmInfo.ui4DisplayFBMAddr = (UINT32) puVBx1FirstBuf;
		_fgSecondBufused = TRUE;
	}
	else
	{
	    rBEFbmInfo.ui4DisplayFBMAddr = (UINT32) puVBx1SecondBuf;
		_fgSecondBufused = FALSE;
	}
	#else
	rBEFbmInfo.ui4DisplayFBMAddr = (UINT32) puVBx1Buf;
	#endif
	rBEFbmInfo.ui4DisplayFBMSize = (UINT32)(ui4FrameSize+ui4HeaderSize+16);
	rBEFbmInfo.ui1DisplayMode = 0;
	rBEFbmInfo.ui4DisplayAreaHeight = 1080;
    rBEFbmInfo.ui4MaxBootTime = ui4BootTimeInfo;
	
	if(rFrameHeaderInfo[ui1FrameIdx].rFrameMode == eIFrame_Header_mode)
	{
	    rBEFbmInfo.ui4DisplayAreaWidth = 64;
	}
	else
	{
	    rBEFbmInfo.ui4DisplayAreaWidth = 1920;
	}
    BELDBG("LDR_BEFrameBuffer: FBM info :\n");
	BELDBG("LDR_BEFrameBuffer: ui4DisplayFBMAddr=0x%08x\n",rBEFbmInfo.ui4DisplayFBMAddr);
	BELDBG("LDR_BEFrameBuffer: ui4DisplayFBMSize=0x%08x\n",rBEFbmInfo.ui4DisplayFBMSize);
	BELDBG("LDR_BEFrameBuffer: ui4ui4MaxBootTime=0x%08x\n",rBEFbmInfo.ui4MaxBootTime);
	BELDBG("LDR_BEFrameBuffer: mode=%d, display w/h = %d/ %d\n",rBEFbmInfo.ui1DisplayMode,
		                   rBEFbmInfo.ui4DisplayAreaWidth,rBEFbmInfo.ui4DisplayAreaHeight);
	BELDBG("LDR_BEFrameBuffer: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
#endif	
    return rBEFbmInfo;
}
#ifndef CC_MTK_LOADER
INT32 OSD_BEFbmDisplay(UINT32 u4ColorMode, UINT32 u4VBx1Addr, UINT32 u4Width, UINT32 u4Height, BE_FBM_REGION_INFO_T * prFBMRegionInfo)
{
	UINT32 u4Plane = OSD_PLANE_3;
	UINT32 u4Scaler = OSD_SCALER_3;
	UINT32 u4RegionId = 0xFFFFFFFF;
	UINT32 u4RegionList = 0xFFFFFFFF;
	UINT32 u4RegionPitch = 0;
	UINT32 u4PanelWidth, u4PanelHeight;
	#ifdef BE_ARGB_RE_ORDER
	UINT32 u4OrderRet;
	BE_OSD_ORDER_T rArgbOrder;
    #endif
	
    u4PanelWidth = PANEL_GetPanelWidth();
    u4PanelHeight = PANEL_GetPanelHeight();	
	OSD_GET_PITCH_SIZE(u4ColorMode,u4Width,u4RegionPitch);
	u4RegionPitch = (u4RegionPitch + 0xF)&(~0xF);
	OSD_RGN_LIST_Create(&u4RegionList);
	if(u4RegionList == 0xFFFFFFFF)
	{
		Printf("%s:RegionList create fail.\n",__FUNCTION__);
		return OSD_RET_UNDEF_ERR;
	}
	OSD_RGN_Create(&u4RegionId,u4Width,u4Height,(void *) u4VBx1Addr,u4ColorMode,u4RegionPitch,0,0,u4Width,u4Height);
	if(u4RegionId == 0xFFFFFFFF)
	{
		Printf("%s:Region create fail.\n",__FUNCTION__);
		return OSD_RET_UNDEF_ERR;
	}
    #ifdef BE_ARGB_RE_ORDER
    u4OrderRet = LDR_BEGetOsdOrder(&rArgbOrder);
    if(!u4OrderRet)
    {
      OSD_RGN_SetColorOrder(u4RegionId,rArgbOrder.ui4Alpha,rArgbOrder.ui4Red,rArgbOrder.ui4Green,rArgbOrder.ui4Blue);
    }
	#endif	
	OSD_RGN_Insert(u4RegionId,u4RegionList);
	OSD_PLA_FlipTo(u4Plane,u4RegionList);
	OSD_SC_Scale(u4Scaler, 1, u4PanelWidth, u4PanelHeight, u4PanelWidth,u4PanelHeight);
	OSD_PLA_FlipTo(u4Plane,u4RegionList);
	OSD_PLA_Enable(u4Plane,TRUE);
	prFBMRegionInfo->u4Pitch = u4RegionPitch;
	prFBMRegionInfo->u4RegionIdx = u4RegionId;
	prFBMRegionInfo->u4RegionListId = u4RegionList;
    return (INT32) OSD_RET_OK;
}

INT32 OSD_BEFreeResource(BE_FBM_REGION_INFO_T rFBMRegionInfo)
{
    OSD_RGN_Delete(rFBMRegionInfo.u4RegionIdx);
	OSD_RGN_LIST_Delete(rFBMRegionInfo.u4RegionListId);
	
    return (INT32) OSD_RET_OK;
}

//-----------------------------------------------------------------------------
/** BEDisplay(): Setup BE via OSD driver.
 *  This function is going to setup BE IC and osd driver with VBx1 data.
 *
 */
//--------------------------------------------------------------------------------
BE_FBM_REGION_INFO_T BEDisplay(BE_FBM_INFO_T rBEFbmInfo)
{
    INT32 i4Ret;
	BE_FBM_REGION_INFO_T rOSDRegionInfo;
	#ifdef BE_BOOT_OSD_TWO_BUF_MODE
	FBM_POOL_T *prFbmPool=NULL;
	UINT8 *puVBx1FirstBuf;
	UINT8 *puVBx1SecondBuf;
	UINT8 *puVBx1TmpBuf;
	UINT32 u4DisplayBuffAddr,u4DisplayBuffSize;
	UINT32 w,h;

    w = rBEFbmInfo.ui4DisplayAreaWidth;
	h = rBEFbmInfo.ui4DisplayAreaHeight; 
	prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_BEIC);
	puVBx1FirstBuf = (UINT8 *)VIRTUAL(prFbmPool->u4Addr + prFbmPool->u4Size - 0x1000000);
	puVBx1SecondBuf = (UINT8 *)VIRTUAL(prFbmPool->u4Addr + prFbmPool->u4Size - 0x800000);
	u4DisplayBuffSize = rBEFbmInfo.ui4DisplayFBMSize;
	puVBx1TmpBuf = (UINT8 *)VIRTUAL(rBEFbmInfo.ui4DisplayFBMAddr);
	
	if(!_fgSecondBufused)
	{
		x_memset((void*)puVBx1FirstBuf,0,0x800000);
		x_memcpy((void*)puVBx1FirstBuf,(void*)puVBx1TmpBuf,u4DisplayBuffSize);
		u4DisplayBuffAddr = PHYSICAL((UINT32)puVBx1FirstBuf);
		_fgSecondBufused = TRUE;
	}
	else
	{
	    x_memset((void*)puVBx1SecondBuf,0,0x800000);
		x_memcpy((void*)puVBx1SecondBuf,(void*)puVBx1TmpBuf,u4DisplayBuffSize);
		u4DisplayBuffAddr = PHYSICAL((UINT32)puVBx1SecondBuf);
		_fgSecondBufused = FALSE;
	}
    #endif

    //BELDBG("LDR_BEDisplay: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    if((rBEFbmInfo.ui4DisplayFBMSize == 0)||(rBEFbmInfo.ui4DisplayFBMAddr == 0))
    {
        Printf("LDR_BEDisplay: Fail due to FBM data corruption!!\n");
        rOSDRegionInfo.u4Pitch = 0;
	    rOSDRegionInfo.u4RegionIdx = 0xFFFFFFFF;
	    rOSDRegionInfo.u4RegionListId = 0xFFFFFFFF;		
        return rOSDRegionInfo;
    }
    //LDR_PmxDisplay(DRVCUST_InitGet(eLoaderLogoBackground));
	#ifdef BE_LOADER_DBG
	vGetPostScalerStatus();
	vDrvVOPLLStatus();
	vErrorHandleStatus();
	DumpPanelAttribute(0);
	#endif
	
	#if 0
	//i4Ret = (INT32)LDR_BEOsdDisplay(OSD_CM_ARGB8888_DIRECT32, rBEFbmInfo.ui4DisplayFBMAddr, 
	//	                            rBEFbmInfo.ui4DisplayFBMSize, rBEFbmInfo.ui4DisplayAreaWidth, 
	//	                            rBEFbmInfo.ui4DisplayAreaHeight,rBEFbmInfo.ui1DisplayMode);
	i4Ret = (INT32)LDR_BEOsdDisplay(OSD_CM_ARGB8888_DIRECT32,  rBEFbmInfo.ui4DisplayFBMAddr, 
		                            rBEFbmInfo.ui4DisplayFBMSize, rBEFbmInfo.ui4DisplayAreaWidth, 
		                            rBEFbmInfo.ui4DisplayAreaHeight,rBEFbmInfo.ui1DisplayMode);
	#else
	#ifdef BE_BOOT_OSD_TWO_BUF_MODE
	i4Ret = (INT32)OSD_BEFbmDisplay(OSD_CM_ARGB8888_DIRECT32, u4DisplayBuffAddr, w, h, &rOSDRegionInfo);	
	#else
	i4Ret = (INT32)OSD_BEFbmDisplay(OSD_CM_ARGB8888_DIRECT32, rBEFbmInfo.ui4DisplayFBMAddr, 
		                         rBEFbmInfo.ui4DisplayAreaWidth, rBEFbmInfo.ui4DisplayAreaHeight, &rOSDRegionInfo);
	#endif
	#endif
    if(i4Ret != 0)
    {
       Printf("LDR_BEDisplay: Fail due to get OSD region!!\n");
       rOSDRegionInfo.u4Pitch = 0;
	   rOSDRegionInfo.u4RegionIdx = 0xFFFFFFFF;
	   rOSDRegionInfo.u4RegionListId = 0xFFFFFFFF;
    }
    //BELDBG("LDR_BEDisplay: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    return rOSDRegionInfo;
}

//-----------------------------------------------------------------------------
/** BEFrameBufferGet(): Setup BE Frame via DRAM buffer.
 *
 */
//--------------------------------------------------------------------------------
BE_FBM_INFO_T BEFrameBufferGet(UINT8 ui1FrameIdx, UINT32 u4Offset)
{
	INT32 i4Ret;
	FBM_POOL_T *prFbmPool=NULL;
	UINT8 *puVBx1Buf;
	UINT8 *puVx1Buffer;
	//UINT8 *puVBx1BufTmp = (UINT8 *)x_mem_alloc(0x800000);
	UINT8 *puVBx1BufTmp;
	UINT32 u4VBx1BufferSize,u4VBx1PartId;
	UINT32 w, h;
	BE_BOOT_FLASH_INFO_T rBeFlashInfo;
	UINT32 ui4HeaderSize,ui4FrameSize,ui4PacketNo,ui4PacketSize[7],i,ui4PacketHeaderSize[7];
	UINT32 ui4BlankPacketNo;
	UINT8  uiTmp0,uiTmp1,uiTmp2,uiTmp3;
	UINT16 ui2CRCchecksum;
	UINT32 ui4BootTimeInfo=0;
    #ifdef BE_PACKET_FLAGS_SUPPORT
	UINT32 ui4PacketFlags=0;
    #endif	
	BE_FBM_INFO_T rBEFbmInfo={0};
	
    BELDBG("BEFrameBufferGet: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	w = PANEL_GetPanelWidth();
    h = PANEL_GetPanelHeight();
    #if 0
	puVBx1BufTmp = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory(w*h*4, 16));
	if (puVBx1BufTmp == 0)
	{
		Printf("puVBx1BufTmp null\n");
		return rBEFbmInfo;
	}
    #endif
    if(ui1FrameIdx == 0)
    {
	    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_BEIC);
	    puVBx1Buf = (UINT8 *)VIRTUAL(prFbmPool->u4Addr);
	    puVx1Buffer = (UINT8 *)VIRTUAL(prFbmPool->u4Addr);
		puVBx1BufTmp = (UINT8 *)VIRTUAL(prFbmPool->u4Addr + prFbmPool->u4Size - 0x800000);
    }
	else
	{
	    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_BEIC);
        if((rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMAddr + rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMSize) >
			(prFbmPool->u4Addr + prFbmPool->u4Size))
        {
            Printf("BEFrameBufferGet buffer overflow!!\n"); 
			return rBEFbmInfo;
        }
	    puVBx1Buf = (UINT8 *)VIRTUAL(rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMAddr + rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMSize);
	    puVx1Buffer = (UINT8 *)VIRTUAL(rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMAddr + rBESTRFbmInfo[ui1FrameIdx-1].ui4DisplayFBMSize);
		puVBx1BufTmp = (UINT8 *)VIRTUAL(prFbmPool->u4Addr + prFbmPool->u4Size - 0x800000);
	}
	x_memset((void*)puVBx1BufTmp,0,0x800000);

    if(ui1FrameIdx>u1TotalDisplayFrmNo-1)
    {
      return rBEFbmInfo;
    }

    uiTmp0 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[4];
    uiTmp1 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[5];		
    uiTmp2 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[6];
    uiTmp3 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[7];
    ui4HeaderSize = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
	//BELDBG("LDR_BEFrameBuffer: ui4HeaderSize=0x%08x\n",ui4HeaderSize);
    uiTmp0 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[24];
    uiTmp1 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[25];		
    uiTmp2 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[26];
    uiTmp3 = rFrameHeaderInfo[ui1FrameIdx].FrameHeader[27];
	ui4PacketNo = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
	//BELDBG("LDR_BEFrameBuffer: ui4PacketNo=0x%08x\n",ui4PacketNo);

	ui4BlankPacketNo = 0;
	ui4FrameSize = 0;

	puVx1Buffer = puVBx1Buf+ui4HeaderSize;

    for(i=0;i<ui4PacketNo;i++)
    {       
        BELDBG("BEFrameBufferGet: parsing loop start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
        rBeFlashInfo = LDR_BEFlashPartitionTranslate(rFrameHeaderInfo[ui1FrameIdx].rFrameContainer[i],u4Offset);
		u4VBx1BufferSize = rBeFlashInfo.u4Size;
		u4VBx1PartId = rBeFlashInfo.u4PartId;	
        #if defined(CC_EMMC_BOOT)
		i4Ret = Loader_ReadMsdc(u4VBx1PartId, 0, puVBx1BufTmp, u4VBx1BufferSize);
        #elif defined(CC_NAND_BOOT) 
	    i4Ret = Loader_ReadNandFlash(u4VBx1PartId, 0, puVBx1BufTmp, u4VBx1BufferSize);
        #endif
		BELDBG("BEFrameBufferGet: parsing loop flash read end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		uiTmp0 = *(puVBx1BufTmp+16);
		uiTmp1 = *(puVBx1BufTmp+17);
		uiTmp2 = *(puVBx1BufTmp+18);
		uiTmp3 = *(puVBx1BufTmp+19);
		ui4PacketSize[i] = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
		//BELDBG("LDR_BEFrameBuffer: ui4PacketSize=0x%08x\n",ui4PacketSize[i]);
		uiTmp0 = *(puVBx1BufTmp+4);
		uiTmp1 = *(puVBx1BufTmp+5);
		uiTmp2 = *(puVBx1BufTmp+6);
		uiTmp3 = *(puVBx1BufTmp+7);
		ui4PacketHeaderSize[i] = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
		//BELDBG("LDR_BEFrameBuffer: ui4PacketHeaderSize=0x%08x\n",ui4PacketHeaderSize[i]);
		uiTmp0 = *(puVBx1BufTmp+12);
		uiTmp1 = *(puVBx1BufTmp+13);
		uiTmp2 = *(puVBx1BufTmp+14);
		uiTmp3 = *(puVBx1BufTmp+15);
		ui4BootTimeInfo = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
        #ifdef BE_PACKET_FLAGS_SUPPORT
		uiTmp0 = *(puVBx1BufTmp+8);
		uiTmp1 = *(puVBx1BufTmp+9);
		uiTmp2 = *(puVBx1BufTmp+10);
		uiTmp3 = *(puVBx1BufTmp+11);
		ui4PacketFlags = ((UINT32)(uiTmp0<<24)+(UINT32)(uiTmp1<<16)+(UINT32)(uiTmp2<<8)+(UINT32)uiTmp3);
        #endif
		#ifdef BE_DELETE_UPGRADE_SUPPORT
		if((ui4PacketHeaderSize[i] != 0) && (ui4PacketSize[i] != 0))
		{
		    ui4FrameSize = ui4FrameSize + ui4PacketSize[i] + ui4PacketHeaderSize[i];
            if(ui4FrameSize > (prFbmPool->u4Size - 0x800000))
            {
			    break;
            } 		
		    x_memcpy((void*)puVx1Buffer,(void*)puVBx1BufTmp,(ui4PacketSize[i]+ui4PacketHeaderSize[i]));
		    BELDBG("BEFrameBufferGet: parsing loop memcpy end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		    puVx1Buffer = puVx1Buffer + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		    x_memset((void*)puVBx1BufTmp,0,(w*h*4));		
		    BELDBG("BEFrameBufferGet: parsing loop end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));	
		}
		else
		{
		    ui4BlankPacketNo++;
		    Printf("BEFrameBufferGet: packet (%d) is delete type\n",u4VBx1PartId);
		}
		#else	
		ui4FrameSize = ui4FrameSize + ui4PacketSize[i] + ui4PacketHeaderSize[i];
        if(ui4FrameSize > (prFbmPool->u4Size - 0x800000))
        {
			break;
        } 		
		x_memcpy((void*)puVx1Buffer,(void*)puVBx1BufTmp,(ui4PacketSize[i]+ui4PacketHeaderSize[i]));
		BELDBG("BEFrameBufferGet: parsing loop memcpy end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		puVx1Buffer = puVx1Buffer + ui4PacketSize[i] + ui4PacketHeaderSize[i];
		x_memset((void*)puVBx1BufTmp,0,(w*h*4));		
		BELDBG("BEFrameBufferGet: parsing loop end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		#endif
    }
    BELDBG("BEFrameBufferGet: endpoint1=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    if(ui4FrameSize > (prFbmPool->u4Size - 0x800000))
    {
        Printf("BEFrameBufferGet buffer size too large!!\n");
		return rBEFbmInfo;
    }	
    uiTmp0 = (UINT8)((ui4FrameSize&0xFF000000)>>24);
	uiTmp1 = (UINT8)((ui4FrameSize&0x00FF0000)>>16); 
	uiTmp2 = (UINT8)((ui4FrameSize&0x0000FF00)>>8); 
	uiTmp3 = (UINT8)(ui4FrameSize&0x000000FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[11] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[10] = uiTmp2;
    rFrameHeaderInfo[ui1FrameIdx].FrameHeader[9] = uiTmp1;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[8] = uiTmp0;
    #ifdef BE_SUPPORT_FRM_HDR_MISC_INFO
    BEGetFrmHdrMiscInfo(ui1FrameIdx);
    #endif
    ui2CRCchecksum = crc16_ccitt(rFrameHeaderInfo[ui1FrameIdx].FrameHeader,(ui4HeaderSize-2));
	uiTmp2 = (UINT8)((ui2CRCchecksum&0xFF00)>>8); 
	uiTmp3 = (UINT8)(ui2CRCchecksum&0x00FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[ui4HeaderSize-1] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[ui4HeaderSize-2] = uiTmp2;
	BELDBG("BEFrameBufferGet: CRC=0x%04x\n",ui2CRCchecksum);
	BELDBG("BEFrameBufferGet: endpoint2=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    #ifdef BE_PACKET_FLAGS_SUPPORT
	Printf("LDR_BEFrameBuffer: PacketFlags=0x%08x\n",ui4PacketFlags);
	#endif
	//update total packet no in frame header
	#ifdef BE_DELETE_UPGRADE_SUPPORT
    ui4PacketNo=ui4PacketNo-ui4BlankPacketNo;
    uiTmp0 = (UINT8)((ui4PacketNo&0xFF000000)>>24);
	uiTmp1 = (UINT8)((ui4PacketNo&0x00FF0000)>>16); 
	uiTmp2 = (UINT8)((ui4PacketNo&0x0000FF00)>>8); 
	uiTmp3 = (UINT8)(ui4PacketNo&0x000000FF);
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[27] = uiTmp3;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[26] = uiTmp2;
    rFrameHeaderInfo[ui1FrameIdx].FrameHeader[25] = uiTmp1;
	rFrameHeaderInfo[ui1FrameIdx].FrameHeader[24] = uiTmp0;
	#endif
    for(i=0;i<ui4HeaderSize;i++)
    {
		*(puVBx1Buf+i)=rFrameHeaderInfo[ui1FrameIdx].FrameHeader[i];
    }
	
    for(i=0;i<16;i++)
    {
        *(puVx1Buffer+i)=rFrameHeaderInfo[ui1FrameIdx].FrameFooter[i];
    }
    puVx1Buffer = puVx1Buffer + 16;
	rBEFbmInfo.ui4DisplayFBMAddr = PHYSICAL((UINT32)puVBx1Buf);
	rBEFbmInfo.ui4DisplayFBMSize = (UINT32)(ui4FrameSize+ui4HeaderSize+16);
	rBEFbmInfo.ui1DisplayMode = 0;
	rBEFbmInfo.ui4DisplayAreaHeight = 1080;
    rBEFbmInfo.ui4MaxBootTime = ui4BootTimeInfo;

	if(rFrameHeaderInfo[ui1FrameIdx].rFrameMode == eIFrame_Header_mode)
	{
	    rBEFbmInfo.ui4DisplayAreaWidth = 64;
	}
	else
	{
	    rBEFbmInfo.ui4DisplayAreaWidth = 1920;
	}
    BELDBG("BEFrameBufferGet: FBM info :\n");
	BELDBG("BEFrameBufferGet: ui4DisplayFBMAddr=0x%08x\n",rBEFbmInfo.ui4DisplayFBMAddr);
	BELDBG("BEFrameBufferGet: ui4DisplayFBMSize=0x%08x\n",rBEFbmInfo.ui4DisplayFBMSize);
	BELDBG("BEFrameBufferGet: ui4ui4MaxBootTime=0x%08x\n",rBEFbmInfo.ui4MaxBootTime);
	BELDBG("BEFrameBufferGet: mode=%d, display w/h = %d/ %d\n",rBEFbmInfo.ui1DisplayMode,
		                   rBEFbmInfo.ui4DisplayAreaWidth,rBEFbmInfo.ui4DisplayAreaHeight);
	BELDBG("BEFrameBufferGet: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    return rBEFbmInfo;
}
#endif
//-----------------------------------------------------------------------------
/** LDR_BEFrameDisplay(): Setup BE via OSD driver.
 *  This function is going to setup BE IC and osd driver with VBx1 data.
 *
 */
//--------------------------------------------------------------------------------
UINT32 LDR_BEFrameDisplay(UINT32 u4FrameIdx, UINT32 u4Offset)
{
    UINT32 u4Ret;
    BE_FBM_INFO_T rBEFbmInfo={0};

    BELDBG("LDR_BEFrameDisplay: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	rBEFbmInfo = LDR_BEFrameBufferGet((UINT8)u4FrameIdx,u4Offset);
	u4Ret = LDR_BEDisplay(rBEFbmInfo);
    BELDBG("LDR_BEFrameDisplay: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
    return u4Ret;
}

VOID LDR_BEBootSeqWaitUartReady(LDR_DATA_T *prLdrData)
{
	INT32 i4BootUpNotifyPin, i4BootUpNotifyPolarity;
	
	i4BootUpNotifyPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpio));
	i4BootUpNotifyPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpioPolarity));
    while(GPIO_GetIn(i4BootUpNotifyPin) == i4BootUpNotifyPolarity)
    {
        x_thread_delay(500);
    }
}

//-----------------------------------------------------------------------------
/** LDR_BEBootSeq(): To display logo with panel index.
 *  This function is going to display logo at boot loader.
 *
 *  @param  u4BmpAddr       The bmp data pointer.
 *  @param  u4Width         The width info of the bmp data.
 *  @param  u4Height        The height info of the bmp data.
 *  @retval 0               Success.
 *  @retval otherwise       Failed.
 */
//-----------------------------------------------------------------------------
UINT32 LDR_BEBootSeq(LDR_DATA_T *prLdrData)
{
#ifdef CC_MTK_LOADER
#if 1//LOADER_SONY_BE_BOOTUP
	BE_BOOT_FLASH_INFO_T rBeFlashInfo;
	BOOL  fgBootUpDone=FALSE;
	BE_BOOT_STATUS_T u1BootState=eState_BEBootInit,u1PreBootState=eState_BEBootInit;
    UINT8 u1FrameCount=0;
	BE_OUTPUT_FRM_INFO_T u1OutputFrm;
	UINT32 u4ErrorChkCount=0,u4ErrorHandleCount=0;
	INT32 i4BootPwrPin,i4BootUpNotifyPin,i4BootUpVx1ReqPin;
	INT32 i4BootPwrPolarity,i4BootUpNotifyPolarity,i4BootUpVx1ReqPolarity;
	INT32 i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity;
	UINT32 u4BEPartitionBase,u4FileTransferTime=1000;
	BE_FBM_INFO_T rBEFbmInfo={0};
	UINT32 u4Val;
	#ifdef BE_FBM_VERIFICATION_MODE
	UINT32 u4VerificationCounter=0;
        #endif

    //BELDBG("LDR_BEBootSeq: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
#endif

	#ifdef CC_COMBINE_4K_2K
	UNUSED(DRVCUST_InitQuery(eGet2KModelSupport, &u4Val));
	if(((u4Val&0x00000001) == 0x00000000)||((u4Val&0x00000040) == 0x00000040))
	{
	  Printf("LDR_BEBootSeq Model not support BE!!");
	  return 0;
	}
	#else	    
    EEPROM_Read(BE_IC_SUPPORT_ADDR, (UINT32)&u4Val, 1);
	if(((UINT8)u4Val) == 0x79)
	{
	  Printf("LDR_BEBootSeq Model Skip BE boot step!!");
      return 0;
	}
    #endif
	#ifdef BE_PACKET_FLAGS_SUPPORT
	ui1DelTypeIndex = 0;
	#endif
    i4BootPwrPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnGpio));
	i4BootUpNotifyPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpio));
	i4BootUpVx1ReqPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootVx1ReqGpio));
	i4BootPnlPwrNotifyPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPnlPwrNotifyGpio));
    i4BootPwrPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnPolarity));
	i4BootUpNotifyPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpioPolarity));
	i4BootUpVx1ReqPolarity= (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootVx1ReqGpioPolarity));
	i4BootPnlPwrNotifyPolarity= (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPnlPwrNotifyGpioPolarity));
	u4BEPartitionBase = DRVCUST_InitGet((QUERY_TYPE_T)(eBELPartitionId));

    //GPIO_SetOut(i4BootPwrPin,i4BootPwrPolarity);
	LDR_PmxDisplay(DRVCUST_InitGet(eLoaderLogoBackground));
	//set OSD lane to 4K1K 60Hz
	#if defined(CC_MT5890) && defined(SUPPORT_PANEL_GRAPH_MODE)
	if (IS_SUPPORT_VB1_GRAPH_MODE)
	{
		Printf("LDR_BEBootSeq Set to 4K1K60Hz!!\n");
		GraphMode_SetPanelType(PANEL_TYPE_4K1K_60);
		vDrvSetGFXTiming(SV_DCLK_60HZ);
	}
	#endif
	//
	LDR_BE_eMMC_HS_Mode();
    while(!fgBootUpDone)
	{
      switch(u1BootState)
      {
        case eState_BEBootInit:
			if(GPIO_GetIn(i4BootUpNotifyPin) == i4BootUpNotifyPolarity)
			{
			  u1PreBootState = u1BootState;
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootInit->eState_BEBootWaitOutput\n");
			  u1BootState = eState_BEBootWaitOutput;
			  u4ErrorChkCount=0;
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u4VerificationCounter = 0;
			  #endif
			}
			if(u4ErrorChkCount > BOOT_INIT_ERR_COUNT)
			{
			  u1PreBootState = u1BootState;
		      BELDBG("LDR_BEBootSeq: state=eState_BEBootInit->eState_BEBootErrorHandle\n");
			  u1BootState = eState_BEBootErrorHandle;			      
			}			
			break;
		case eState_BEBootWaitOutput:
			if(GPIO_GetIn(i4BootUpVx1ReqPin) == i4BootUpVx1ReqPolarity)
			{
			  if((u1PreBootState == eState_BEBootInit) || (u1PreBootState == eState_BEBootFallingVx1Output))
			  {
			    if(u1PreBootState == eState_BEBootInit)
			    {
			      u4ErrorChkCount=0;
			      u1PreBootState = u1BootState;
			      BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootRisingVx1Output\n");				  
			      u1BootState = eState_BEBootRisingVx1Output;				  
			    }
				else
				{
				  if(u4ErrorChkCount > u4FileTransferTime)
				  {			      
				    u1PreBootState = u1BootState;
					BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootErrorHandle\n");
			        u1BootState = eState_BEBootErrorHandle;	
				  }
				  else
				  {
				    u4ErrorChkCount=0;
			        u1PreBootState = u1BootState;
					BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootRisingVx1Output\n");
			        u1BootState = eState_BEBootRisingVx1Output;					  
				  }
				}
			  }
			}
			else
			{
			  if((u1PreBootState != eState_BEBootInit) && (u1PreBootState != eState_BEBootFallingVx1Output))
			  {
			    if(u4ErrorChkCount > u4FileTransferTime)
			    {
			      u1PreBootState = u1BootState;
				  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootWaitOutput->eState_BEBootErrorHandle\n");
			      u1BootState = eState_BEBootErrorHandle;			      
			    }
				else
				{
				  u4ErrorChkCount=0;				
			      u1PreBootState = u1BootState;
				  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootWaitOutput->eState_BEBootFallingVx1Output\n");
			      u1BootState = eState_BEBootFallingVx1Output;
				}
			  }
			}
			break;
		case eState_BEBootRisingVx1Output:
			#ifdef BE_FBM_VERIFICATION_MODE
			if(u4VerificationCounter)
			{
			  u4VerificationCounter--;
			}
			else
			#endif
			{
			//BELDBG("LDR_BEBootSeq: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
			u1OutputFrm = rOutputFrm[u1FrameCount];
			if((u1OutputFrm.u1State != 0xFF)&&(u1OutputFrm.u1FrameNum != 0xFF))
			{
			    rBEFbmInfo = LDR_BEFrameBufferGet(u1FrameCount,u4BEPartitionBase);
	            LDR_BEDisplay(rBEFbmInfo);			
		        //BELDBG("LDR_BEBootSeq: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		        if((u1OutputFrm.u1State == 3)&&(u1OutputFrm.u1FrameNum == 1))
		        {
		            GPIO_SetOut(i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity);
		        }
				u4FileTransferTime = rBEFbmInfo.ui4MaxBootTime;
				#ifdef BE_FBM_VERIFICATION_MODE
				u4VerificationCounter=BE_FBM_VERIFICATION_RISING_COUNT;
				#endif
			}
			else
			{	
			    u4FileTransferTime = 0;
			    Printf("LDR_BEBootSeq output frame data corruption!!\n");
			}
			u1FrameCount++;
			u1PreBootState = u1BootState;
			if(u1FrameCount > u1TotalDisplayFrmNo-1)
			{
			  BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootRisingVx1Output->eState_BEBootWaitUartRdy\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  //OSD_UnLockLogoDisplaying();
			  u1BootState = eState_BEBootFinish;
			  #else
			  u1BootState = eState_BEBootWaitUartRdy;
			  #endif
			  u4ErrorChkCount=0;
			}
			else
			{
			  BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootRisingVx1Output->eState_BEBootWaitOutput\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u1BootState = eState_BEBootFallingVx1Output;
			  #else
			  u1BootState = eState_BEBootWaitOutput;			
			  #endif			
			}
			}
			break;		
		case eState_BEBootFallingVx1Output:
			#ifdef BE_FBM_VERIFICATION_MODE
			if(u4VerificationCounter)
			{
			  u4VerificationCounter--;
			}
			else
			#endif
			{
			//BELDBG("LDR_BEBootSeq: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
			u1OutputFrm = rOutputFrm[u1FrameCount];
			if((u1OutputFrm.u1State != 0xFF)&&(u1OutputFrm.u1FrameNum != 0xFF))
			{
			    rBEFbmInfo = LDR_BEFrameBufferGet(u1FrameCount,u4BEPartitionBase);
	            LDR_BEDisplay(rBEFbmInfo);			
		        //BELDBG("LDR_BEBootSeq: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
		        if((u1OutputFrm.u1State == 3)&&(u1OutputFrm.u1FrameNum == 1))
		        {
		            GPIO_SetOut(i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity);
		        }
				u4FileTransferTime = rBEFbmInfo.ui4MaxBootTime;
				#ifdef BE_FBM_VERIFICATION_MODE
				u4VerificationCounter=BE_FBM_VERIFICATION_FALLING_COUNT;
				#endif
			}
			else
			{
			    u4FileTransferTime = 0;
			    Printf("LDR_BEBootSeq output frame data corruption!!\n");
			}		    
			u1FrameCount++;
			u1PreBootState = u1BootState;
			if(u1FrameCount > u1TotalDisplayFrmNo-1)
			{
			  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootFallingVx1Output->eState_BEBootWaitUartRdy\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  //OSD_UnLockLogoDisplaying();
			  u1BootState = eState_BEBootFinish;
			  #else
			  u1BootState = eState_BEBootWaitUartRdy;
			  #endif
			  u4ErrorChkCount=0;
			}
			else
			{
			  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootFallingVx1Output->eState_BEBootWaitOutput\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u1BootState = eState_BEBootRisingVx1Output;
			  #else
			  u1BootState = eState_BEBootWaitOutput;			
			  #endif			
			}		
			}		
			break;
			
		case eState_BEBootErrorHandle:
            if(u4ErrorHandleCount == BE_BOOT_ERROR_RETRY_COUNT)
            {
              BELDBG("LDR_BEBootSeq: state=eState_BEBootErrorHandle->eState_BEBootFail\n");
              u1BootState=eState_BEBootFail;
            }
			else
			{
			  GPIO_SetOut(i4BootPwrPin,(1-i4BootPwrPolarity));
			  x_thread_delay(300);
			  GPIO_SetOut(i4BootPwrPin,i4BootPwrPolarity);
			  u1BootState=eState_BEBootInit;
			  u1PreBootState=eState_BEBootInit;
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootErrorHandle->eState_BEBootInit\n");
			  u1FrameCount=0;
			  u4ErrorChkCount=0;
			  u4ErrorHandleCount++;
			}
			break;
			
		case eState_BEBootWaitUartRdy:
			if(GPIO_GetIn(i4BootUpNotifyPin) == i4BootUpNotifyPolarity)
			{
			  u1PreBootState = u1BootState;
			  u1BootState = eState_BEBootWaitUartRdy;
			  if(u4ErrorChkCount > u4FileTransferTime)
			  {
		        BELDBG("LDR_BEBootSeq: state=eState_BEBootWaitUartRdy->eState_BEBootErrorHandle\n");
			    u1BootState = eState_BEBootErrorHandle;			      
			  }              			  
			}
			else
			{
			  u1PreBootState = u1BootState;
			  u1BootState = eState_BEBootFinish;			
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootWaitUartRdy->eState_BEBootFinish\n");
			}
			break;

		case eState_BEBootFail:
			LDR_BE_eMMC_Recovery();
			fgBootUpDone=TRUE;
			//add flush OSD here
			BELDBG("LDR_BEBootSeq : eState_BEBootFail\n");
			prLdrData->u4LdrFlags = (prLdrData->u4LdrFlags)| LDRFLAG_BEBOOT_FAIL;
			BELDBG("LDR_BEBootSeq : set LDR data flag = 0x%08x\n",prLdrData->u4LdrFlags);
			//GPIO_SetOut(i4BootUpNotifyPin,(1-i4BootUpNotifyPolarity));
			break;
			
		case eState_BEBootFinish:
			LDR_BE_eMMC_Recovery();
			fgBootUpDone=TRUE;
			BELDBG("LDR_BEBootSeq: state=eState_BEBootFinish\n");
			//add flush OSD here
			#if defined(CC_MT5890) && defined(SUPPORT_PANEL_GRAPH_MODE)
			if (IS_SUPPORT_VB1_GRAPH_MODE)
			{
				GraphMode_SetPanelType(PANEL_TYPE_4K2K_30);
		        vDrvSetGFXTiming(SV_DCLK_60HZ);
			}
            #endif
			break;
		default:
			break;
      }
	  x_thread_delay(10);
	  #ifdef BE_BOOT_ERROR_CHECK
	  u4ErrorChkCount++;
	  #endif
	}
#endif	
	return 0;
}

#ifndef CC_MTK_LOADER
UINT32 BEBoot_pm_suspend(void)
{
	BOOL  fgSuspendDone=FALSE;
    UINT8 u1FrameCount=0;
	BE_OUTPUT_FRM_INFO_T u1OutputFrm;
	UINT32 u4BEPartitionBase;
	BE_FBM_INFO_T rBEFbmInfo={0};
	UINT32 u4Val;

    BELDBG("BEBoot_pm_suspend: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	#ifdef CC_COMBINE_4K_2K
	UNUSED(DRVCUST_InitQuery(eGet2KModelSupport, &u4Val));
	if(((u4Val&0x00000001) == 0x00000000)||((u4Val&0x00000040) == 0x00000040))
	{
	  Printf("Model not support BE!!\n");
	  return 0;
	}
	#else
    EEPROM_Read(BE_IC_SUPPORT_ADDR, (UINT32)&u4Val, 1);
	if(((UINT8)u4Val) != 0x79)
	{
	  Printf("Model not support BE!!\n");
      return 0;
	}
	#endif
	BE_Suspend_Init();
	u4BEPartitionBase = DRVCUST_InitGet((QUERY_TYPE_T)(eBELPartitionId));

    while(!fgSuspendDone)
	{
	    BELDBG("BEBootSuspend: Frame(0x%x)\n",u1FrameCount);
		u1OutputFrm = rOutputFrm[u1FrameCount];
		if((u1OutputFrm.u1State != 0xFF)&&(u1OutputFrm.u1FrameNum != 0xFF))
		{
			rBEFbmInfo = BEFrameBufferGet(u1FrameCount,u4BEPartitionBase);
		    rBESTRFbmInfo[u1FrameCount].ui4DisplayFBMAddr = rBEFbmInfo.ui4DisplayFBMAddr;
	        rBESTRFbmInfo[u1FrameCount].ui4DisplayFBMSize = rBEFbmInfo.ui4DisplayFBMSize;
	        rBESTRFbmInfo[u1FrameCount].ui4DisplayAreaWidth = rBEFbmInfo.ui4DisplayAreaWidth;
	        rBESTRFbmInfo[u1FrameCount].ui4DisplayAreaHeight = rBEFbmInfo.ui4DisplayAreaHeight;
	        rBESTRFbmInfo[u1FrameCount].ui4MaxBootTime = rBEFbmInfo.ui4MaxBootTime;
	        rBESTRFbmInfo[u1FrameCount].ui1DisplayMode = rBEFbmInfo.ui1DisplayMode;
		}
		else
		{
			Printf("BEBootSuspend output frame data corruption!!\n");
		}			
		u1FrameCount++;
		if(u1FrameCount > u1TotalDisplayFrmNo-1)
		{
		  fgSuspendDone = TRUE;
		  BELDBG("BEBootSuspend: Done\n");
		}	
	}
	BELDBG("BEBoot_pm_suspend: end=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	return 0;
}
UINT32 BEBoot_pm_resume(void)
{
	BOOL  fgBootUpDone=FALSE;
	BE_BOOT_STATUS_T u1BootState=eState_BEBootInit,u1PreBootState=eState_BEBootInit;
    UINT8 u1FrameCount=0;
	BE_OUTPUT_FRM_INFO_T u1OutputFrm;
	UINT32 u4ErrorChkCount=0,u4ErrorHandleCount=0;
	INT32 i4BootPwrPin,i4BootUpNotifyPin,i4BootUpVx1ReqPin;
	INT32 i4BootPwrPolarity,i4BootUpNotifyPolarity,i4BootUpVx1ReqPolarity;
	INT32 i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity;
	UINT32 u4BEPartitionBase,u4FileTransferTime=1000;
	BE_FBM_INFO_T rBEFbmInfo={0};
	BE_FBM_REGION_INFO_T rFBMRegionInfo[LOADER_BE_TOTAL_FRAME_NUMBER];
	UINT32 u4Val=0;
	UINT32 u4OsdPlane = OSD_PLANE_3;
	UINT32 u4Scaler = OSD_SCALER_3;
    INT32 i4OriRegionList = (-1);
    UINT8 u1OriPLA_Enable;
    UINT32 u4OriSC_Enable = 0;
    UINT32 u4OriSrcW = 0;
    UINT32 u4OriSrcH = 0;
    UINT32 u4OriDstW = 0;
    UINT32 u4OriDstH = 0;
    UINT32 u4OriIs16Bpp = 0;
	#ifdef BE_FBM_VERIFICATION_MODE
	UINT32 u4VerificationCounter=0;
    #endif
	
    BELDBG("BEBoot_pm_resume: start=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	#ifdef CC_COMBINE_4K_2K
	UNUSED(DRVCUST_InitQuery(eGet2KModelSupport, &u4Val));
	if(((u4Val&0x00000001) == 0x00000000)||((u4Val&0x00000040) == 0x00000040))
	{
	  Printf("LDR_BEBootSeq Model not support BE!!");
	  return 0;
	}
	#else	
    EEPROM_Read(BE_IC_SUPPORT_ADDR, (UINT32)&u4Val, 1);
	if(((UINT8)u4Val) != 0x79)
	{
	  Printf("LDR_BEBootSeq Model not support BE!!");
	  return 0;
    }
    #endif
    i4BootPwrPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnGpio));
	i4BootUpNotifyPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpio));
	i4BootUpVx1ReqPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootVx1ReqGpio));
	i4BootPnlPwrNotifyPin = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPnlPwrNotifyGpio));
    i4BootPwrPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPowerOnPolarity));
	i4BootUpNotifyPolarity = (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootUpNotifyGpioPolarity));
	i4BootUpVx1ReqPolarity= (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootVx1ReqGpioPolarity));
	i4BootPnlPwrNotifyPolarity= (INT32)DRVCUST_InitGet64((QUERY_TYPE_T)(eBELBootPnlPwrNotifyGpioPolarity));
	u4BEPartitionBase = DRVCUST_InitGet((QUERY_TYPE_T)(eBELPartitionId));

    //get original setting
    i4OriRegionList = OSD_PLA_GetFirstRegionList(u4OsdPlane);
    if (OSD_PLA_GetEnable(u4OsdPlane, &u1OriPLA_Enable) < 0)
    {
        Printf("OSD_PLA_GetEnable fail\n");
    }
    UNUSED(OSD_SC_GetScalerInfo(u4Scaler, &u4OriSC_Enable, &u4OriSrcW,
                    &u4OriSrcH, &u4OriDstW, &u4OriDstH, &u4OriIs16Bpp));
    UNUSED(u4OriIs16Bpp);

    GPIO_SetOut(i4BootPwrPin,i4BootPwrPolarity);
	PMX_SetBg(eLoaderLogoBackground, FALSE);
	//set OSD lane to 4K1K 60Hz
	#if defined(CC_MT5890) && defined(SUPPORT_PANEL_GRAPH_MODE)
	if (IS_SUPPORT_VB1_GRAPH_MODE)
	{
		Printf("BEBoot_pm_resume Set GFX to 4K1K60Hz!!\n");
		//u4PanelType = GraphMode_GetPanelType();
		GraphMode_SetPanelType(PANEL_TYPE_4K1K_60);
		vDrvSetGFXTiming(SV_DCLK_60HZ);
	}
	#endif
    while(!fgBootUpDone)
	{
      switch(u1BootState)
      {
        case eState_BEBootInit:
			if(GPIO_GetIn(i4BootUpNotifyPin) == i4BootUpNotifyPolarity)
			{
			  u1PreBootState = u1BootState;
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootInit->eState_BEBootWaitOutput\n");
			  u1BootState = eState_BEBootWaitOutput;
			  u4ErrorChkCount=0;
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u4VerificationCounter = 0;
			  #endif
			}
			if(u4ErrorChkCount > BOOT_INIT_ERR_COUNT)
			{
			  u1PreBootState = u1BootState;
		      BELDBG("LDR_BEBootSeq: state=eState_BEBootInit->eState_BEBootErrorHandle\n");
			  u1BootState = eState_BEBootErrorHandle;			      
			}			
			break;
		case eState_BEBootWaitOutput:
			if(GPIO_GetIn(i4BootUpVx1ReqPin) == i4BootUpVx1ReqPolarity)
			{
			  if((u1PreBootState == eState_BEBootInit) || (u1PreBootState == eState_BEBootFallingVx1Output))
			  {
			    if(u1PreBootState == eState_BEBootInit)
			    {
			      u4ErrorChkCount=0;
			      u1PreBootState = u1BootState;
			      BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootRisingVx1Output\n");				  
			      u1BootState = eState_BEBootRisingVx1Output;				  
			    }
				else
				{
				  if(u4ErrorChkCount > u4FileTransferTime)
				  {			      
				    u1PreBootState = u1BootState;
					BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootErrorHandle\n");
					//OSD_UnLockLogoDisplaying();
			        u1BootState = eState_BEBootErrorHandle;	
				  }
				  else
				  {
				    u4ErrorChkCount=0;
			        u1PreBootState = u1BootState;
					BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootWaitOutput->eState_BEBootRisingVx1Output\n");
					//OSD_UnLockLogoDisplaying();
			        u1BootState = eState_BEBootRisingVx1Output;					  
				  }
				}
			  }
			}
			else
			{
			  if((u1PreBootState != eState_BEBootInit) && (u1PreBootState != eState_BEBootFallingVx1Output))
			  {
			    if(u4ErrorChkCount > u4FileTransferTime)
			    {
			      u1PreBootState = u1BootState;
				  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootWaitOutput->eState_BEBootErrorHandle\n");
				  //OSD_UnLockLogoDisplaying();
			      u1BootState = eState_BEBootErrorHandle;			      
			    }
				else
				{
				  u4ErrorChkCount=0;				
			      u1PreBootState = u1BootState;
				  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootWaitOutput->eState_BEBootFallingVx1Output\n");
				  //OSD_UnLockLogoDisplaying();
			      u1BootState = eState_BEBootFallingVx1Output;
				}
			  }
			}
			break;
		case eState_BEBootRisingVx1Output:
			#ifdef BE_FBM_VERIFICATION_MODE
			if(u4VerificationCounter)
			{
			  u4VerificationCounter--;
			}
			else
			#endif
			{
			u1OutputFrm = rOutputFrm[u1FrameCount];
			Printf("BEBoot_pm_resume: (R)display (%x) start=0x%08x\n", u1FrameCount, BIM_READ32(REG_RW_TIMER2_LOW));
			if((u1OutputFrm.u1State != 0xFF)&&(u1OutputFrm.u1FrameNum != 0xFF))
			{
			    rFBMRegionInfo[u1FrameCount].u4RegionIdx = 0xFFFFFFFF;
				rFBMRegionInfo[u1FrameCount].u4RegionListId = 0xFFFFFFFF;
				rFBMRegionInfo[u1FrameCount].u4Pitch = 0;			    
			    rBEFbmInfo = rBESTRFbmInfo[u1FrameCount];
	            rFBMRegionInfo[u1FrameCount] = BEDisplay(rBEFbmInfo);
				//OSD_LockLogoDisplaying();
		        if((u1OutputFrm.u1State == 3)&&(u1OutputFrm.u1FrameNum == 1))
		        {
		            GPIO_SetOut(i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity);
		        }
				u4FileTransferTime = rBEFbmInfo.ui4MaxBootTime;
				#ifdef BE_FBM_VERIFICATION_MODE
				u4VerificationCounter=BE_FBM_VERIFICATION_RISING_COUNT;
				#endif
			}
			else
			{	
			    u4FileTransferTime = 0;
			    Printf("LDR_BEBootSeq output frame data corruption!!\n");
			}
			u1FrameCount++;
			u1PreBootState = u1BootState;
			Printf("BEBoot_pm_resume: (R)display (%x) end=0x%08x\n", u1FrameCount, BIM_READ32(REG_RW_TIMER2_LOW));
			if(u1FrameCount > u1TotalDisplayFrmNo-1)
			{
			  BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootRisingVx1Output->eState_BEBootWaitUartRdy\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  //OSD_UnLockLogoDisplaying();
			  u1BootState = eState_BEBootFinish;
			  #else
			  u1BootState = eState_BEBootWaitUartRdy;
			  #endif
			  u4ErrorChkCount=0;
			}
			else
			{
			  BELDBG("LDR_BEBootSeq: state(R)=eState_BEBootRisingVx1Output->eState_BEBootWaitOutput\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u1BootState = eState_BEBootFallingVx1Output;
			  #else
			  u1BootState = eState_BEBootWaitOutput;			
			  #endif
			}
			}
			break;		
		case eState_BEBootFallingVx1Output:
			#ifdef BE_FBM_VERIFICATION_MODE
			if(u4VerificationCounter)
			{
			  u4VerificationCounter--;
			}
			else
			#endif
			{
			u1OutputFrm = rOutputFrm[u1FrameCount];
			Printf("BEBoot_pm_resume: (F)display (%x) start=0x%08x\n", u1FrameCount, BIM_READ32(REG_RW_TIMER2_LOW));
			if((u1OutputFrm.u1State != 0xFF)&&(u1OutputFrm.u1FrameNum != 0xFF))
			{
			    rFBMRegionInfo[u1FrameCount].u4RegionIdx = 0xFFFFFFFF;
				rFBMRegionInfo[u1FrameCount].u4RegionListId = 0xFFFFFFFF;
				rFBMRegionInfo[u1FrameCount].u4Pitch = 0;
			    rBEFbmInfo = rBESTRFbmInfo[u1FrameCount];
	            rFBMRegionInfo[u1FrameCount] = BEDisplay(rBEFbmInfo);
				//OSD_LockLogoDisplaying();
		        if((u1OutputFrm.u1State == 3)&&(u1OutputFrm.u1FrameNum == 1))
		        {
		            GPIO_SetOut(i4BootPnlPwrNotifyPin,i4BootPnlPwrNotifyPolarity);
		        }
				u4FileTransferTime = rBEFbmInfo.ui4MaxBootTime;
				#ifdef BE_FBM_VERIFICATION_MODE
				u4VerificationCounter=BE_FBM_VERIFICATION_FALLING_COUNT;
				#endif
			}
			else
			{
			    u4FileTransferTime = 0;
			    Printf("LDR_BEBootSeq output frame data corruption!!\n");
			}		    
			u1FrameCount++;
			u1PreBootState = u1BootState;
			Printf("BEBoot_pm_resume: (F)display (%x) end=0x%08x\n", u1FrameCount, BIM_READ32(REG_RW_TIMER2_LOW));
			if(u1FrameCount > u1TotalDisplayFrmNo-1)
			{
			  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootFallingVx1Output->eState_BEBootWaitUartRdy\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  //OSD_UnLockLogoDisplaying();
			  u1BootState = eState_BEBootFinish;
			  #else
			  u1BootState = eState_BEBootWaitUartRdy;
			  #endif
			  u4ErrorChkCount=0;
			}
			else
			{
			  BELDBG("LDR_BEBootSeq: state(F)=eState_BEBootFallingVx1Output->eState_BEBootWaitOutput\n");
			  #ifdef BE_FBM_VERIFICATION_MODE
			  u1BootState = eState_BEBootRisingVx1Output;
			  #else
			  u1BootState = eState_BEBootWaitOutput;			
			  #endif
			}
			}		
			break;
			
		case eState_BEBootErrorHandle:
            if(u4ErrorHandleCount == BE_BOOT_ERROR_RETRY_COUNT)
            {
              BELDBG("LDR_BEBootSeq: state=eState_BEBootErrorHandle->eState_BEBootFail\n");
              u1BootState=eState_BEBootFail;
            }
			else
			{
			  GPIO_SetOut(i4BootPwrPin,(1-i4BootPwrPolarity));
			  x_thread_delay(300);
			  GPIO_SetOut(i4BootPwrPin,i4BootPwrPolarity);
			  u1BootState=eState_BEBootInit;
			  u1PreBootState=eState_BEBootInit;
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootErrorHandle->eState_BEBootInit\n");
			  u1FrameCount=0;
			  u4ErrorChkCount=0;
			  u4ErrorHandleCount++;
			}
			break;
			
		case eState_BEBootWaitUartRdy:
			//OSD_UnLockLogoDisplaying();
			if(GPIO_GetIn(i4BootUpNotifyPin) == i4BootUpNotifyPolarity)
			{
			  u1PreBootState = u1BootState;
			  u1BootState = eState_BEBootWaitUartRdy;
			  if(u4ErrorChkCount > u4FileTransferTime)
			  {
		        BELDBG("LDR_BEBootSeq: state=eState_BEBootWaitUartRdy->eState_BEBootErrorHandle\n");
			    u1BootState = eState_BEBootErrorHandle;			      
			  }              			  
			}
			else
			{
			  u1PreBootState = u1BootState;
			  u1BootState = eState_BEBootFinish;			
			  BELDBG("LDR_BEBootSeq: state=eState_BEBootWaitUartRdy->eState_BEBootFinish\n");
			}
			break;

		case eState_BEBootFail:
			fgBootUpDone=TRUE;
			OSD_PLA_Enable(u4OsdPlane, FALSE);
			for(u4Val=0; u4Val < u1TotalDisplayFrmNo; u4Val++)
			{
			    if((rFBMRegionInfo[u4Val].u4RegionListId != 0xFFFFFFFF) && 
					(rFBMRegionInfo[u4Val].u4RegionIdx != 0xFFFFFFFF))
			    {
			        OSD_BEFreeResource(rFBMRegionInfo[u4Val]);
			    }
			}
			//add flush OSD (back to original setting)here
			if (u4OriSC_Enable != 0)
			{
			    UNUSED(OSD_SC_Scale(u4Scaler, u4OriSC_Enable, u4OriSrcW, u4OriSrcH,u4OriDstW, u4OriDstH));
			}
			if (i4OriRegionList >= 0)
			{
			    OSD_PLA_FlipTo(u4OsdPlane, (UINT32) i4OriRegionList);
			}
			else
			{
			    OSD_PLA_FlipToNone(u4OsdPlane);
			}
			if (u1OriPLA_Enable)
			{
			    OSD_PLA_Enable(u4OsdPlane, TRUE);   
			}
			Printf("BEBootResume eState_BEBootFail recover OSD(%d %d %d)!!\n",
			                    u4OriSC_Enable,u4OriSC_Enable,i4OriRegionList);			
			BELDBG("LDR_BEBootSeq : eState_BEBootFail\n");
			//GPIO_SetOut(i4BootUpNotifyPin,(1-i4BootUpNotifyPolarity));
			break;
			
		case eState_BEBootFinish:
			fgBootUpDone=TRUE;
			OSD_PLA_Enable(u4OsdPlane, FALSE);
			for(u4Val=0; u4Val < u1TotalDisplayFrmNo; u4Val++)
			{
			    if((rFBMRegionInfo[u4Val].u4RegionListId != 0xFFFFFFFF) && 
					(rFBMRegionInfo[u4Val].u4RegionIdx != 0xFFFFFFFF))
			    {
			        OSD_BEFreeResource(rFBMRegionInfo[u4Val]);
			    }
			}			
			if (u4OriSC_Enable != 0)
			{
			    UNUSED(OSD_SC_Scale(u4Scaler, u4OriSC_Enable, u4OriSrcW, u4OriSrcH,u4OriDstW, u4OriDstH));
			}
			if (i4OriRegionList >= 0)
			{
			    OSD_PLA_FlipTo(u4OsdPlane, (UINT32) i4OriRegionList);
			}
			else
			{
			    OSD_PLA_FlipToNone(u4OsdPlane);
			}
			if (u1OriPLA_Enable)
			{
			    OSD_PLA_Enable(u4OsdPlane, TRUE);   
			}			
			Printf("BEBootResume eState_BEBootFinish recover OSD(%d %d %d)!!\n",
			                    u4OriSC_Enable,u4OriSC_Enable,i4OriRegionList);			
			BELDBG("LDR_BEBootSeq: state=eState_BEBootFinish\n");
			//add flush OSD (back to original setting)here
			#if defined(CC_MT5890) && defined(SUPPORT_PANEL_GRAPH_MODE)
			if (IS_SUPPORT_VB1_GRAPH_MODE)
			{
				GraphMode_SetPanelType(PANEL_TYPE_4K2K_30);
		        vDrvSetGFXTiming(SV_DCLK_60HZ);
			}
            #endif			
			break;
		default:
			break;
      }
	  x_thread_delay(10);
	  #ifdef BE_BOOT_ERROR_CHECK
	  u4ErrorChkCount++;
	  #endif
	}
	BELDBG("BEBoot_pm_resume: nd=0x%08x\n", BIM_READ32(REG_RW_TIMER2_LOW));
	#ifdef BE_SEND_POWER_STATE
	BEResumeGetPowerState();
	#endif
	return 0;
}
#endif
VOID LDR_BE_Erase_Del_Partition(void)
{
#ifdef CC_MTK_LOADER
#ifdef BE_PACKET_FLAGS_SUPPORT
    UINT8  u1PartId;
    UINT32 i=0;
	UINT32 ui4PacketFlag=0;
	UINT32 u4BEPartitionBase=0;
	UINT64 u8Offset,u8Len;

#ifdef BE_LOADER_DBG
	Printf("============LDR_BE_Erase_Del_Partition=============\n");
	Printf("rDeletTypeInfo : ui1PartitionId ui4PacketFlag\n");
	for(i=0;i<32;i++)
	{
	  Printf("================0x%x / 0x%04x===================\n",
	  	     rDeletTypeInfo[i].ui1PartitionId,rDeletTypeInfo[i].ui4PacketFlag);
	}	
#endif
    u4BEPartitionBase = DRVCUST_InitGet((QUERY_TYPE_T)(eBELPartitionId));
    for(i=0;i<32;i++)
    {
        u1PartId = (UINT8)(u4BEPartitionBase&0x000000FF);
        if(rDeletTypeInfo[i].ui1PartitionId != 0xFF)
        {
            ui4PacketFlag = rDeletTypeInfo[i].ui4PacketFlag;
			ui4PacketFlag = ui4PacketFlag & 0x80000000;
			u1PartId = u1PartId + rDeletTypeInfo[i].ui1PartitionId;
            if(ui4PacketFlag)
            {
		        u8Offset = DRVCUST_InitGet64((QUERY_TYPE_T)(eNANDFlashPartOffset0+u1PartId));
                u8Len = DRVCUST_InitGet64((QUERY_TYPE_T)(eNANDFlashPartSize0+u1PartId));			
			    UNUSED(MsdcEraseCard(u8Offset, u8Len, 0));	
			    Printf("LDR_BE_Erase_Del_Partition: erease partition(%d)(from: 0x%08x / size: 0x%08x)\n",
					   u1PartId, u8Offset, u8Len);
			}
        }
		else
		{
		    BELDBG("LDR_BE_Erase_Del_Partition: index(%d) not init\n",i);
			break;
		}
    }
#endif
#endif
}
#endif

