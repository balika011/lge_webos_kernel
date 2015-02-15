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
 * $RCSfile: sofac_cmd.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file sofac_cmd.c
 *  SOEM Factory command module
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_lint.h"

LINT_SAVE_AND_DISABLE

#ifdef __MW_CLI_DEF__
#include "x_mid_cli.h"
#else

#include "x_drv_cli.h"
#endif

#define DEFINE_IS_LOG      CLI_IsLog
#include "x_debug.h"
#include "x_util.h"
#include "x_hal_5381.h"

#include "x_os.h"
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_pinmux.h"
#include "x_assert.h"
#include "x_bim.h"
#include "x_serial.h"
#include "x_timer.h"
#include "c_model.h"
#include TARGET_SPEC_DRV_CUST_CUSTOMIZED_HDR_FILE
#include "u_drv_cust.h"

static UINT8 au1buf[512]={0};

static INT32 _Sofac_Cust_Spec_Set(INT32 i4Argc, const CHAR ** szArgv)
{
   UINT8 u1data1=0;
   UINT8 fgcleanbuff=0;
   UINT16 u2data2=0;
   UINT16 u2paraofst=0;
   UINT8 u1paralen=0;
   INT32 i=0,j=0;
   
   if(i4Argc<4)
   {
      Printf("Invalid argument\n");
	  Printf("%s type  bstore clean paraofst paralen data1 data2\n",__FUNCTION__);
	  return -1;
   }
   #ifndef DRV_CUSTOM_CUST_SPEC_SET
      Printf("No D-interface DRV_CUSTOM_CUST_SPEC_SET defined!!\n");
	  return -1;
   #else
     u2data2=(UINT16)StrToInt(szArgv[1]);//type
	 u1data1=(UINT8)StrToInt(szArgv[2]);//bstore
	 fgcleanbuff=(UINT8)StrToInt(szArgv[3]);//if clean data in buff
	 //ss 39 0 0 0 2 0x00 0x01 320 4 0x40 0x01 0x14 0
	 if(fgcleanbuff)
	 {
	   x_memset(au1buf,0,sizeof(au1buf));
	 }
	 
	 for(i=4;i<i4Argc;i++)
	 {
	     u2paraofst=(UINT16)StrToInt(szArgv[i]);
		 
		 if(u2paraofst>=sizeof(au1buf)/sizeof(au1buf[0]))
		 {
		     Printf("%s paraoffset exceed %d B\n",__FUNCTION__,sizeof(au1buf)/sizeof(au1buf[0]));
			 return -1; 
		 }
		 
		 if((i+1)>=i4Argc)
		 {
		    Printf("%s missing paralen\n",__FUNCTION__);
			return -1;
		 }
		 else
		 {
		    u1paralen=(UINT8)StrToInt(szArgv[i+1]);
			 if((u2paraofst+u1paralen)>sizeof(au1buf)/sizeof(au1buf[0]))
			 {
			    Printf("%s paraoffset +paralen exceed %d B\n",__FUNCTION__,sizeof(au1buf)/sizeof(au1buf[0]));
			    return -1; 
			 }
			 i+=2;
			for(j=0;j<u1paralen && i<i4Argc;j++)
			{
			    if((u2paraofst+j)<(sizeof(au1buf)/sizeof(au1buf[0])))
			    {
				  au1buf[u2paraofst+j]=(UINT8)StrToInt(szArgv[i]);
				  Printf("au1buf[%d]=%X\n",u2paraofst+j,au1buf[u2paraofst+j]);
			    }
				else
				{
				   break;
				}
				i++;
			}
			if(j!=u1paralen)
			{
			    Printf("%s paralen  mismatch with the para count\n",__FUNCTION__);
			    return -1;  
			}
			i--;
		 }
	 }

#if 0	 
     //prepare the argument
	 for(i=2;i<i4Argc-1;i++)
     {
	    au1buf[i-2]=(UINT8)StrToInt(szArgv[i]); 
     }
#endif	 
	 if((i=DRV_CUSTOM_CUST_SPEC_SET(u2data2,(void *)au1buf,u1paralen,u1data1))<0)
	 {
	    Printf("Set fail.Ret=%d\n",i);
	    return -1;
	 }
	 return 1;
   #endif      
}
static INT32 _Sofac_Cust_MISC_Set(INT32 i4Argc, const CHAR ** szArgv)
{
   UINT8 u1data1=0;
   UINT32 u4data=0;
   INT32 i=0;
   
   if(i4Argc<3)
   {
      Printf("Invalid argument\n");
	  Printf("%s type  data1 data2 data3...bstore\n",__FUNCTION__);
	  return -1;
   }

     u4data=(UINT32)StrToInt(szArgv[1]);//type
	 u1data1=(UINT8)StrToInt(szArgv[i4Argc-1]);//bstore
     //prepare the argument
	 for(i=2;i<i4Argc-1;i++)
     {
	    au1buf[i-2]=(UINT8)StrToInt(szArgv[i]); 
     }
	 if((i=d_custom_misc_set(u4data,(void *)au1buf,i4Argc-3,u1data1))!=0)
	 {
	    Printf("Set fail.Ret=%d\n",i);
	    return -1;
	 }
	 return 1;   
}

static INT32 _Sofac_Cust_Spec_Get(INT32 i4Argc, const CHAR ** szArgv)
{
   UINT16 u2data1=0;
   UINT16 u2paraofst=0;
   UINT8 u1paralen=0;
   INT32 i=0,j=0;
   SIZE_T s_getsize=0;
   
   if(i4Argc<2)
   {
      Printf("Invalid argument\n");
	  Printf("%s type size paraofst para_len data1 data2 data3...\n",__FUNCTION__);
	  return -1;
   }
   #ifndef DRV_CUSTOM_CUST_SPEC_GET
      Printf("No D-interface DRV_CUSTOM_CUST_SPEC_GET defined!!\n");
	  return -1;
   #else
     u2data1=(UINT16)StrToInt(szArgv[1]);//type
	 s_getsize=(SIZE_T)StrToInt(szArgv[2]);//size
	 
	 for(i=3;i<i4Argc;i++)
	 {
	     u2paraofst=(UINT16)StrToInt(szArgv[i]);
		 
		 if(u2paraofst>=sizeof(au1buf)/sizeof(au1buf[0]))
		 {
		     Printf("%s paraoffset exceed %d B\n",__FUNCTION__,sizeof(au1buf)/sizeof(au1buf[0]));
			 return -1; 
		 }
		 
		 if((i+1)>=i4Argc)
		 {
		    Printf("%s missing paralen\n",__FUNCTION__);
			return -1;
		 }
		 else
		 {
		    u1paralen=(UINT8)StrToInt(szArgv[i+1]);
			 if((u2paraofst+u1paralen)>sizeof(au1buf)/sizeof(au1buf[0]))
			 {
			    Printf("%s paraoffset +paralen exceed %d B\n",__FUNCTION__,sizeof(au1buf)/sizeof(au1buf[0]));
			    return -1; 
			 }
			 i+=2;
			for(j=0;j<u1paralen && i<i4Argc;j++)
			{
			   if((u2paraofst+j)<(sizeof(au1buf)/sizeof(au1buf[0])))
			   {
			   	au1buf[u2paraofst+j]=(UINT8)StrToInt(szArgv[i]);
				Printf("au1buf[%d]=%X\n",u2paraofst+j,au1buf[u2paraofst+j]);
			   }
			   else
			   {
			      break;
			   }
				i++;
			}
			if(j!=u1paralen)
			{
			    Printf("%s paralen  mismatch with the para count\n",__FUNCTION__);
			    return -1;  
			}
			i--;
		 }
	 }
#if 0
	 for(i=4;i<i4Argc;i++)
     {
	    au1buf[i-4]=(UINT8)StrToInt(szArgv[i]); 
     }
#endif	 
     if((i=DRV_CUSTOM_CUST_SPEC_GET(u2data1,(void *)au1buf,&s_getsize))<0)
	 {
	      Printf("Get fail.Ret=%d\n",i);
	      return -1;
	 }
     //prepare the argument
	 Printf("\nReturn Result:\n");
	 for(i=0;i<s_getsize && i<sizeof(au1buf)/sizeof(au1buf[0]);i++)
     {
	    Printf("%02X ",au1buf[i]);
		if(i && (0==i%16))
		{
	      Printf("\n");  
		}
     }
     Printf("\n");
	 return 1;
   #endif      
}
static INT32 _Sofac_Cust_MISC_Get(INT32 i4Argc, const CHAR ** szArgv)
{
   UINT32 u4data=0;
   INT32 i=0;
   SIZE_T s_getsize=0;
   
   if(i4Argc<2)
   {
      Printf("Invalid argument\n");
	  Printf("%s type size data1 data2 data3...\n",__FUNCTION__);
	  return -1;
   }

     u4data=(UINT32)StrToInt(szArgv[1]);//type
	 s_getsize=(SIZE_T)StrToInt(szArgv[2]);//size
	 
	 for(i=3;i<i4Argc;i++)
     {
	    au1buf[i-3]=(UINT8)StrToInt(szArgv[i]); 
     }
	 
     if((i=d_custom_misc_get(u4data,(void *)au1buf,&s_getsize))!=0)
	 {
	      Printf("Get fail.Ret=%d\n",i);
	      return -1;
	 }
     //prepare the argument
	 for(i=0;i<s_getsize && i<sizeof(au1buf)/sizeof(au1buf[0]);i++)
     {
	    Printf("%02X ",au1buf[i]);
		if(i && (0==i%16))
		{
	      Printf("\n");  
		}
     }
     Printf("\n");
	 return 1;  
}
static INT32 _Sofac_Cust_Spec_Help(INT32 i4Argc, const CHAR ** szArgv)
{

   UNUSED(i4Argc);
   UNUSED(szArgv);
{
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_BEGIN:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_BEGIN);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PATH:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PATH);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PICTURE_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PICTURE_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_CINE_MOTION_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_CINE_MOTION_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GRAPHIC_AREA:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GRAPHIC_AREA);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DCC:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DCC);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MASK_AREA:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MASK_AREA);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_INPUT_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_INPUT_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MICRO_P_NVM_UPGRADE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MICRO_P_NVM_UPGRADE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_READ_REGISTER:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_READ_REGISTER);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_WRITE_REGISTER:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_WRITE_REGISTER);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_EXC_COMMAND:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_EXC_COMMAND);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_NUM:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_NUM);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LED_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LED_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_BRIGHTNESS_PLUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_BRIGHTNESS_PLUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_STANDBY_LED_DISABLE_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_STANDBY_LED_DISABLE_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_SELF_DIAG_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_SELF_DIAG_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_BOOT_COUNT_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_BOOT_COUNT_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LOADER_UPGRADE_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LOADER_UPGRADE_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_STATE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_STATE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_DISPLAY:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_DISPLAY);
Printf("DRV_CUSTOM_CUST_SPEC_GET_TAG_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_GET_TAG_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_SYNC_MENU_FLAG:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_SYNC_MENU_FLAG);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_REFRESH:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_REFRESH);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DEEPCOLOR:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DEEPCOLOR);
Printf("DRV_CUSTOM_CUST_SPEC_GET_ADC_RGB_GAIN_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_ADC_RGB_GAIN_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_LIGHT_SENSOR_STATS:%d\n",DRV_CUSTOM_CUST_SPEC_GET_LIGHT_SENSOR_STATS);
Printf("DRV_CUSTOM_CUST_SPEC_GET_IRRX:%d\n",DRV_CUSTOM_CUST_SPEC_GET_IRRX);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_GAMMA_ORIGINAL:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_GAMMA_ORIGINAL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_POWER5V_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_POWER5V_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_INPUT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_INPUT_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_OUTPUT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_OUTPUT_STATUS);
Printf("DRV_CUSTOM_SPEC_TYPE_DBC_DEBUG:%d\n",DRV_CUSTOM_SPEC_TYPE_DBC_DEBUG);
Printf("DRV_CUSTOM_SPEC_TYPE_YAPL:%d\n",DRV_CUSTOM_SPEC_TYPE_YAPL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_EDID:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_EDID);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_HDCP:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_HDCP);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MONITOR_OUT:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MONITOR_OUT);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_STARTUP_SOUND_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_STARTUP_SOUND_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_48:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_48);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_50:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_50);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_60:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_60);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_SS_PERMILLAGE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_SS_PERMILLAGE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_CIS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_CIS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PCB_TYPE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PCB_TYPE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MMP_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MMP_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MMP_OSD_STATE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MMP_OSD_STATE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MMP_PHOTO_FRAME_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MMP_PHOTO_FRAME_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_OSD_TYPE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_OSD_TYPE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_FM_RADIO_STATE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_FM_RADIO_STATE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_AUTOMAN_SCAN_STATE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_AUTOMAN_SCAN_STATE);
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TELETEXT_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TELETEXT_MODE);
#endif
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_SCENE_SELECT_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_SCENE_SELECT_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_PATTERN:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_PATTERN);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_TIME:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_TIME);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_CFG:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_CFG);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_READ:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_READ);
Printf("DRV_CUSTOM_CUST_SPEC_GET_SONY_LIB_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_GET_SONY_LIB_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_GET_TEMP_SENSOR_VALUE:%d\n",DRV_CUSTOM_CUST_SPEC_GET_TEMP_SENSOR_VALUE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_INTERNAL_SIG_PTN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_INTERNAL_SIG_PTN);
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY
Printf("DRV_CUSTOM_CUST_SPEC_GET_SIGNAL_LEVEL:%d\n",DRV_CUSTOM_CUST_SPEC_GET_SIGNAL_LEVEL);
#endif
Printf("DRV_CUSTOM_CUST_SPEC_GET_A_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_A_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_D_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_D_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_R_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_R_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_G_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_G_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_B_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_B_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_R_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_R_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_G_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_G_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_B_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_B_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_CVBS_TARGET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_CVBS_TARGET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL1_TARGET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL1_TARGET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL2_TARGET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL2_TARGET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL3_TARGET:%d\n",DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL3_TARGET);
Printf("DRV_CUSTOM_CUST_SPEC_GET_EFUSE_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_EFUSE_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_MAX_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_MAX_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_BLANK_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_BLANK_LVL);
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY
Printf("DRV_CUSTOM_CUST_SPEC_SET_IP_NOISE_TH:%d\n",DRV_CUSTOM_CUST_SPEC_SET_IP_NOISE_TH);
Printf("DRV_CUSTOM_CUST_SPEC_GET_IP_NOISE_TH:%d\n",DRV_CUSTOM_CUST_SPEC_GET_IP_NOISE_TH);
#endif
Printf("DRV_CUSTOM_CUST_SPEC_SET_SOURCE_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_SOURCE_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_SOURCE_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_SOURCE_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_SET_SOURCE_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_SET_SOURCE_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_GET_SOURCE_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_GET_SOURCE_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_MAIN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_MAIN);
Printf("DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_SUB:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_SUB);
Printf("DRV_CUSTOM_CUST_SPEC_SET_NONLINEAR_PARAM:%d\n",DRV_CUSTOM_CUST_SPEC_SET_NONLINEAR_PARAM);
Printf("DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_SLOPE:%d\n",DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_SLOPE);
Printf("DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_ENDFACTOR:%d\n",DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_ENDFACTOR);
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY
Printf("DRV_CUSTOM_CUST_SPEC_GET_TUNER_REG:%d\n",DRV_CUSTOM_CUST_SPEC_GET_TUNER_REG);
Printf("DRV_CUSTOM_CUST_SPEC_SET_TUNER_AGC:%d\n",DRV_CUSTOM_CUST_SPEC_SET_TUNER_AGC);
Printf("DRV_CUSTOM_CUST_SPEC_SET_CIS_AGC:%d\n",DRV_CUSTOM_CUST_SPEC_SET_CIS_AGC);
Printf("DRV_CUSTOM_CUST_SPEC_SET_INT_PIC_SIG_BSTR:%d\n",DRV_CUSTOM_CUST_SPEC_SET_INT_PIC_SIG_BSTR);
#endif
Printf("DRV_CUSTOM_CUST_SPEC_GET_DRV_SREG:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DRV_SREG);
Printf("DRV_CUSTOM_CUST_SPEC_SET_DRV_SREG:%d\n",DRV_CUSTOM_CUST_SPEC_SET_DRV_SREG);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SETUP:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SETUP);
Printf("DRV_CUSTOM_CUST_SPEC_GET_GAMMA_SETUP:%d\n",DRV_CUSTOM_CUST_SPEC_GET_GAMMA_SETUP);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAVE:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAVE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_QUIT:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_QUIT);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_EXIT:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_EXIT);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_RESET:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_RESET);
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAMPLE:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAMPLE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_SETUP:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_SETUP);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_SAVE:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_SAVE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_QUIT:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_QUIT);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_EXIT:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_EXIT);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_RESET:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_RESET);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_SETLVL:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_SETLVL);
Printf("DRV_CUSTOM_CUST_SPEC_SET_WB_SETCLR:%d\n",DRV_CUSTOM_CUST_SPEC_SET_WB_SETCLR);
#ifndef THIRD_PARTY_LIB_EMCS_ATV_ONLY
Printf("DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA_EN:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA_EN);
Printf("DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA_EN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA_EN);
#endif
Printf("DRV_CUSTOM_CUST_SPEC_SET_GAMMA_PAT_GEN:%d\n",DRV_CUSTOM_CUST_SPEC_SET_GAMMA_PAT_GEN);
Printf("DRV_CUSTOM_CUST_SPEC_GET_COLOR_LEVEL:%d\n",DRV_CUSTOM_CUST_SPEC_GET_COLOR_LEVEL);
Printf("DRV_CUSTOM_CUST_SPEC_GET_PANEL_INCH_SIZE:%d\n",DRV_CUSTOM_CUST_SPEC_GET_PANEL_INCH_SIZE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_AUDIO_INTERDAC_MUTE:%d\n",DRV_CUSTOM_CUST_SPEC_SET_AUDIO_INTERDAC_MUTE);
Printf("DRV_CUSTOM_CUST_SPEC_SET_HTL_IR_PASS_THROUGH:%d\n",DRV_CUSTOM_CUST_SPEC_SET_HTL_IR_PASS_THROUGH);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_INITVOL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_INITVOL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_STEPTIMER:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_STEPTIMER);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_VOLLVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_VOLLVL);
Printf("DRV_CUSTOM_CUST_SPEC_SET_RGB_GAISOU:%d\n",DRV_CUSTOM_CUST_SPEC_SET_RGB_GAISOU);
Printf("DRV_CUSTOM_CUST_SPEC_GET_RGB_GAISOU:%d\n",DRV_CUSTOM_CUST_SPEC_GET_RGB_GAISOU);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE_USB_VIDEO_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE_USB_VIDEO_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HTLSI_ONLINEMODE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HTLSI_ONLINEMODE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DITHER:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DITHER);
Printf("DRV_CUSTOM_CUST_SPEC_RESET_IRIS_MUTE_PIN:%d\n",DRV_CUSTOM_CUST_SPEC_RESET_IRIS_MUTE_PIN);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_APP_COUNTRY:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_APP_COUNTRY);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_OSD_STATUS_CHANGE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_OSD_STATUS_CHANGE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_VGA_DETECT_WAKEUP:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_VGA_DETECT_WAKEUP);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_VGA_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_VGA_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PANEL_ID:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PANEL_ID);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DVD_ENABLE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DVD_ENABLE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_PCB_REGION:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_PCB_REGION);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_ATV_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_ATV_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_SET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_SET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_PRESET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_PRESET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_BY_ELEMENT:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_BY_ELEMENT);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_ADJ_METHOD:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_ADJ_METHOD);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_EN:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_EN);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_POS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_POS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_MEAS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_MEAS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_POS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_POS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_Y:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_Y);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CB:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CB);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CR:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CR);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_ON:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_ON);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX_CHANGE_STS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX_CHANGE_STS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_R:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_R);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_G:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_G);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_B:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_B);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R00:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R00);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R01:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R01);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R02:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R02);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R03:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R03);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R04:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R04);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R05:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R05);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R06:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R06);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R07:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R07);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R08:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R08);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R09:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R09);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R10:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R10);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R11:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R11);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R12:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R12);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R13:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R13);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R14:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R14);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R15:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R15);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R16:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R16);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R17:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R17);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R18:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R18);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R19:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R19);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R20:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R20);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R21:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R21);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R22:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R22);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R23:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R23);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R24:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R24);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R25:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R25);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R26:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R26);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R27:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R27);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R28:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R28);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R29:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R29);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R30:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R30);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R31:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R31);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R32:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R32);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G00:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G00);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G01:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G01);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G02:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G02);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G03:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G03);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G04:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G04);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G05:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G05);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G06:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G06);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G07:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G07);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G08:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G08);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G09:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G09);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G10:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G10);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G11:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G11);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G12:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G12);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G13:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G13);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G14:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G14);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G15:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G15);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G16:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G16);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G17:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G17);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G18:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G18);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G19:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G19);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G20:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G20);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G21:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G21);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G22:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G22);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G23:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G23);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G24:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G24);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G25:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G25);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G26:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G26);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G27:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G27);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G28:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G28);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G29:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G29);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G30:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G30);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G31:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G31);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G32:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G32);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B00:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B00);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B01:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B01);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B02:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B02);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B03:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B03);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B04:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B04);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B05:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B05);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B06:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B06);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B07:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B07);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B08:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B08);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B09:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B09);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B10:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B10);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B11:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B11);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B12:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B12);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B13:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B13);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B14:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B14);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B15:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B15);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B16:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B16);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B17:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B17);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B18:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B18);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B19:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B19);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B20:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B20);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B21:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B21);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B22:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B22);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B23:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B23);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B24:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B24);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B25:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B25);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B26:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B26);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B27:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B27);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B28:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B28);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B29:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B29);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B30:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B30);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B31:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B31);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B32:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B32);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_AGING_LAST_POWER_CONTROL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_AGING_LAST_POWER_CONTROL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DMPLL_SPECTRUM_PERMILLAGE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DMPLL_SPECTRUM_PERMILLAGE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_DVB_T2_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_DVB_T2_STATUS);
Printf("DRV_CUSTOM_VID_TYPE_LED_MOTION:%d\n",DRV_CUSTOM_VID_TYPE_LED_MOTION);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LED_BLINK_CONTROL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LED_BLINK_CONTROL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TEST_PTN_RAMP:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TEST_PTN_RAMP);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_MOVE_OS_OPTION_POS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_MOVE_OS_OPTION_POS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_PRESET_BEFORE_GET_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_PRESET_BEFORE_GET_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_SET_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_SET_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SET_SELECT_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SET_SELECT_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_GET_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_GET_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_GET_SELECT_LVL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_GET_SELECT_LVL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SETUP_STATUS_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SETUP_STATUS_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_REFRESH:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_REFRESH);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_VID_PRESET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_VID_PRESET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_PRESET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_PRESET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_SET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_SET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLRIDX_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLRIDX_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBLVL_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBLVL_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_SETUP_STATUS_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_SETUP_STATUS_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_GAIN_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_GAIN_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_SET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_SET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_SET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_SET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_PARM_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_PARM_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_ADJ_CLR_IDX_GET:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_ADJ_CLR_IDX_GET);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_AGING_PTN:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_AGING_PTN);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_CONTROL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_CONTROL);
Printf("DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA:%d\n",DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA);
Printf("DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA:%d\n",DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA);
Printf("DRV_CUSTOM_VID_TYPE_GAMMA_REFRESH:%d\n",DRV_CUSTOM_VID_TYPE_GAMMA_REFRESH);
Printf("DRV_CUSTOM_VID_TYPE_YAPL:%d\n",DRV_CUSTOM_VID_TYPE_YAPL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_POP_CVBS_COMP_NOTIFY:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_POP_CVBS_COMP_NOTIFY);
Printf("DRV_CUSTOM_CUST_SPEC_EPG_FLAG:%d\n",DRV_CUSTOM_CUST_SPEC_EPG_FLAG);
Printf("DRV_CUSTOM_CUST_SPEC_DBC_DEBUG:%d\n",DRV_CUSTOM_CUST_SPEC_DBC_DEBUG);
Printf("DRV_CUSTOM_CUST_SPEC_GAMMA_BUF:%d\n",DRV_CUSTOM_CUST_SPEC_GAMMA_BUF);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_ALL_TOUCH_KEY_ENA:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_ALL_TOUCH_KEY_ENA);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_RECALIBRATION:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_RECALIBRATION);
Printf("DRV_CUSTOM_CUST_SPEC_V_COM_DRV:%d\n",DRV_CUSTOM_CUST_SPEC_V_COM_DRV);
Printf("DRV_CUSTOM_CUST_SPEC_SCART_DRV:%d\n",DRV_CUSTOM_CUST_SPEC_SCART_DRV);
Printf("DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE:%d\n",DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE);
Printf("DRV_CUSTOM_CUST_SPEC_MICRO_OPTION_RMVB:%d\n",DRV_CUSTOM_CUST_SPEC_MICRO_OPTION_RMVB);
Printf("DRV_CUSTOM_CUST_SPEC_EEP_UPDATE:%d\n",DRV_CUSTOM_CUST_SPEC_EEP_UPDATE);
Printf("DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_AGING_EEP_UPDATE:%d\n",DRV_CUSTOM_CUST_SPEC_AGING_EEP_UPDATE);
Printf("DRV_CUSTOM_CUST_SPEC_BATTERY_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_BATTERY_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_ID:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_ID);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_NAME:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_NAME);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_HI:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_HI);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_LOW:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_LOW);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_SELECT:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_SELECT);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_TOTAL_NUM:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_TOTAL_NUM);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_MAGIC:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_MAGIC);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_OFFSET:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_OFFSET);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV0:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV0);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV1:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV1);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV2:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV2);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV3:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV3);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV4:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV4);
Printf("DRV_CUSTOM_CUST_SPEC_USBUART_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_USBUART_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_BLUE_STRETCH:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_BLUE_STRETCH);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_MAGIC_NUM:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_MAGIC_NUM);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_RELOAD:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_RELOAD);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_REMAP:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_REMAP);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_LINEAR:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_LINEAR);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_SCE_ON_OFF:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_SCE_ON_OFF);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_DCR_ON_OFF:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_DCR_ON_OFF);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_WR_VGA_EDID:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_WR_VGA_EDID);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_CVBS_AUTO_COLOR:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_CVBS_AUTO_COLOR);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_CVBS_D_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_CVBS_D_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_CVBS_A_GAIN:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_CVBS_A_GAIN);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_Y_LEVEL:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_Y_LEVEL);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PQ_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PQ_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PQ_GAMMA_VER:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PQ_GAMMA_VER);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PQ_SCE_VER:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PQ_SCE_VER);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PQ_QTY_VER:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PQ_QTY_VER);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_AQ_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_AQ_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_VERSION:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_VERSION);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_NUMBER:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_NUMBER);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_ID_LIST:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_ID_LIST);
Printf("DRV_CUSTOM_CUST_SPEC_SERVO_ADC:%d\n",DRV_CUSTOM_CUST_SPEC_SERVO_ADC);
Printf("DRV_CUSTOM_CUST_SPEC_DISP_PANEL_LUT_SEL:%d\n",DRV_CUSTOM_CUST_SPEC_DISP_PANEL_LUT_SEL);
Printf("DRV_CUSTOM_CUST_SPEC_VGA_EDID_DATA:%d\n",DRV_CUSTOM_CUST_SPEC_VGA_EDID_DATA);
Printf("DRV_CUSTOM_CUST_SPEC_HDMI_EDID_DATA:%d\n",DRV_CUSTOM_CUST_SPEC_HDMI_EDID_DATA);
Printf("DRV_CUSTOM_CUST_SPEC_TUNER_REG:%d\n",DRV_CUSTOM_CUST_SPEC_TUNER_REG);
Printf("DRV_CUSTOM_CUST_SPEC_SIF:%d\n",DRV_CUSTOM_CUST_SPEC_SIF);
Printf("DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_DEF:%d\n",DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_DEF);
Printf("DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_PQ:%d\n",DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_PQ);
Printf("DRV_CUSTOM_CUST_SPEC_EEP_RW:%d\n",DRV_CUSTOM_CUST_SPEC_EEP_RW);
Printf("DRV_CUSTOM_CUST_SPEC_LED_MODE:%d\n",DRV_CUSTOM_CUST_SPEC_LED_MODE);
Printf("DRV_CUSTOM_CUST_SPEC_UPDATE_LED_FIX_PATTERN:%d\n",DRV_CUSTOM_CUST_SPEC_UPDATE_LED_FIX_PATTERN);
Printf("DRV_CUSTOM_CUST_SPEC_CI_PLUG_IN:%d\n",DRV_CUSTOM_CUST_SPEC_CI_PLUG_IN);
Printf("DRV_CUSTOM_CUST_SPEC_CI_DESCRAMBLE:%d\n",DRV_CUSTOM_CUST_SPEC_CI_DESCRAMBLE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA_CTRL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA_CTRL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DTV_PACKET_RX_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DTV_PACKET_RX_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_HDCP2X_RW:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_HDCP2X_RW);
Printf("DRV_CUSTOM_CUST_SPEC_READ_ECC_UNCRRCT:%d\n",DRV_CUSTOM_CUST_SPEC_READ_ECC_UNCRRCT);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_WRITE:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_WRITE);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_READ:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_READ);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_V_COM_DRV:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_V_COM_DRV);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_SCART_OUT:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_SCART_OUT);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_VOL_CTRL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_VOL_CTRL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_TONE_CTRL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_TONE_CTRL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_SHORT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_SHORT_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_DEMOD_CLK_CTRL:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_DEMOD_CLK_CTRL);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_DEMOD_MOUNT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_DEMOD_MOUNT_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_LNB_MOUNT_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_LNB_MOUNT_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_UPDATE_REC_STATUS:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_UPDATE_REC_STATUS);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_START_LED_BLINK:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_START_LED_BLINK);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_STOP_LED_BLINK:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_STOP_LED_BLINK);
Printf("DRV_CUSTOM_CUST_SPEC_TYPE_END:%d\n",DRV_CUSTOM_CUST_SPEC_TYPE_END);
}
   return 1;
}
static INT32 _SoFac_Cust_MISC_Help(INT32 i4Argc, const CHAR ** szArgv)
{
   UNUSED(i4Argc);
   UNUSED(szArgv);
   
Printf("DRV_CUSTOM_MISC_TYPE_BEGIN:%d\n",DRV_CUSTOM_MISC_TYPE_BEGIN);
Printf("DRV_CUSTOM_MISC_TYPE_ALIVE_CHECK_CTRL:%d\n",DRV_CUSTOM_MISC_TYPE_ALIVE_CHECK_CTRL);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_MENU_LANG_SET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_MENU_LANG_SET);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_ENABLE_SET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_ENABLE_SET);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_ONE_TOUCH_PLAY_EN_SET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_ONE_TOUCH_PLAY_EN_SET);
Printf("DRV_CUSTOM_MISC_TYPE_MHL_STB_CHARGING_SET:%d\n",DRV_CUSTOM_MISC_TYPE_MHL_STB_CHARGING_SET);
Printf("DRV_CUSTOM_MISC_TOTAL_PANEL_NUM:%d\n",DRV_CUSTOM_MISC_TOTAL_PANEL_NUM);
Printf("DRV_CUSTOM_MISC_TYPE_MHL_BGM_PWR_INTERRUPT_SET:%d\n",DRV_CUSTOM_MISC_TYPE_MHL_BGM_PWR_INTERRUPT_SET);
Printf("DRV_CUSTOM_MISC_TYPE_PANEL_NAME:%d\n",DRV_CUSTOM_MISC_TYPE_PANEL_NAME);
Printf("DRV_CUSTOM_MISC_TYPE_TUNER_SIF:%d\n",DRV_CUSTOM_MISC_TYPE_TUNER_SIF);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_EDID:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_EDID);
Printf("DRV_CUSTOM_MISC_TYPE_SIF_ADVANCED:%d\n",DRV_CUSTOM_MISC_TYPE_SIF_ADVANCED);
Printf("DRV_CUSTOM_MISC_TYPE_TVD_MCDONE_CNT:%d\n",DRV_CUSTOM_MISC_TYPE_TVD_MCDONE_CNT);
Printf("DRV_CUSTOM_MISC_TYPE_PALNEL_MODE:%d\n",DRV_CUSTOM_MISC_TYPE_PALNEL_MODE);
Printf("DRV_CUSTOM_MISC_TYPE_PANEL_INFO:%d\n",DRV_CUSTOM_MISC_TYPE_PANEL_INFO);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_STATUS:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_STATUS);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_HPD_LOW:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_HPD_LOW);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_TOGGLE_HPD:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_TOGGLE_HPD);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_BYPASS_AVIINFO:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_BYPASS_AVIINFO);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_KSV_VALUE:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_KSV_VALUE);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_DEEPCOLOR_STATUS:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_DEEPCOLOR_STATUS);
Printf("DRV_CUSTOM_MISC_TYPE_AUTO_SEARCH:%d\n",DRV_CUSTOM_MISC_TYPE_AUTO_SEARCH);
Printf("DRV_CUSTOM_GET_NTPV_EEPROM_OFFSET:%d\n",DRV_CUSTOM_GET_NTPV_EEPROM_OFFSET);
Printf("DRV_CUSTOM_GET_COUNTRY:%d\n",DRV_CUSTOM_GET_COUNTRY);
Printf("DRV_CUSTOM_SET_COUNTRY:%d\n",DRV_CUSTOM_SET_COUNTRY);
Printf("DRV_CUSTOM_MISC_SET_IMAGE_BANK:%d\n",DRV_CUSTOM_MISC_SET_IMAGE_BANK);
Printf("DRV_CUSTOM_MISC_QUERY_IMAGE_BANK:%d\n",DRV_CUSTOM_MISC_QUERY_IMAGE_BANK);
Printf("DRV_CUSTOM_MISC_SET_8032UP_BANK:%d\n",DRV_CUSTOM_MISC_SET_8032UP_BANK);
Printf("DRV_CUSTOM_MISC_QUERY_8032UP_BANK:%d\n",DRV_CUSTOM_MISC_QUERY_8032UP_BANK);
Printf("DRV_CUSTOM_MISC_TYPE_STARTUP_SOUND_STATUS:%d\n",DRV_CUSTOM_MISC_TYPE_STARTUP_SOUND_STATUS);
Printf("DRV_CUSTOM_MISC_TYPE_STARTUP_LOGO_STATUS:%d\n",DRV_CUSTOM_MISC_TYPE_STARTUP_LOGO_STATUS);
Printf("DRV_CUSTOM_MISC_TYPE_GET_T8032_VER:%d\n",DRV_CUSTOM_MISC_TYPE_GET_T8032_VER);
Printf("DRV_CUSTOM_MISC_TYPE_OSD_STATUS_CHG:%d\n",DRV_CUSTOM_MISC_TYPE_OSD_STATUS_CHG);
Printf("DRV_CUSTOM_DRAM_CFG_SET:%d\n",DRV_CUSTOM_DRAM_CFG_SET);
Printf("DRV_CUSTOM_DRAM_CFG_GET:%d\n",DRV_CUSTOM_DRAM_CFG_GET);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_MENU_LANG_GET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_MENU_LANG_GET);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_ENABLE_GET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_ENABLE_GET);
Printf("DRV_CUSTOM_MISC_TYPE_CEC_ONE_TOUCH_PLAY_EN_GET:%d\n",DRV_CUSTOM_MISC_TYPE_CEC_ONE_TOUCH_PLAY_EN_GET);
Printf("DRV_CUSTOM_MISC_TYPE_MHL_STB_CHARGING_GET:%d\n",DRV_CUSTOM_MISC_TYPE_MHL_STB_CHARGING_GET);
Printf("DRV_CUSTOM_SET_CRNT_INP_SRC_INFO:%d\n",DRV_CUSTOM_SET_CRNT_INP_SRC_INFO);
Printf("DRV_CUSTOM_MISC_TYPE_VDP_CB_ENABLE_SET:%d\n",DRV_CUSTOM_MISC_TYPE_VDP_CB_ENABLE_SET);
Printf("DRV_CUSTOM_MISC_TYPE_HDMI_HDCP:%d\n",DRV_CUSTOM_MISC_TYPE_HDMI_HDCP);
Printf("DRV_CUSTOM_MISC_TYPE_VGA_EDID:%d\n",DRV_CUSTOM_MISC_TYPE_VGA_EDID);
Printf("DRV_CUSTOM_MISC_TYPE_LOADER_INFO:%d\n",DRV_CUSTOM_MISC_TYPE_LOADER_INFO);
Printf("DRV_CUSTOM_MISC_TYPE_LED_MODE:%d\n",DRV_CUSTOM_MISC_TYPE_LED_MODE);
Printf("DRV_CUSTOM_MISC_TYPE_LED_NOTIFICATION:%d\n",DRV_CUSTOM_MISC_TYPE_LED_NOTIFICATION);
Printf("DRV_CUSTOM_MISC_TYPE_NAND_ERASE:%d\n",DRV_CUSTOM_MISC_TYPE_NAND_ERASE);
Printf("DRV_CUSTOM_MISC_TYPE_NAND_WRITE:%d\n",DRV_CUSTOM_MISC_TYPE_NAND_WRITE);
Printf("DRV_CUSTOM_MISC_TYPE_NAND_BULK_WRITE:%d\n",DRV_CUSTOM_MISC_TYPE_NAND_BULK_WRITE);
Printf("DRV_CUSTOM_MISC_TYPE_NOR_BULK_WRITE:%d\n",DRV_CUSTOM_MISC_TYPE_NOR_BULK_WRITE);
Printf("DRV_CUSTOM_MISC_TYPE_TV_STATUS:%d\n",DRV_CUSTOM_MISC_TYPE_TV_STATUS);
Printf("DRV_CUSTOM_MISC_TYPE_FBM_SET_EDB_SIZE:%d\n",DRV_CUSTOM_MISC_TYPE_FBM_SET_EDB_SIZE);
Printf("DRV_CUSTOM_MISC_TYPE_FBM_GET_EDB_ADDR:%d\n",DRV_CUSTOM_MISC_TYPE_FBM_GET_EDB_ADDR);
Printf("DRV_CUSTOM_MISC_TYPE_BIM_GET_BOOT_TIME:%d\n",DRV_CUSTOM_MISC_TYPE_BIM_GET_BOOT_TIME);
Printf("DRV_CUSTOM_MISC_TYPE_CURRENT_NAND_NAME:%d\n",DRV_CUSTOM_MISC_TYPE_CURRENT_NAND_NAME);
Printf("DRV_CUSTOM_MISC_TYPE_END:%d\n",DRV_CUSTOM_MISC_TYPE_END);
return 1;
}
 
static CLI_EXEC_T _ardcustCmdTbl[] = {
    {"Spec_Set",            "ss",     _Sofac_Cust_Spec_Set,        NULL, "custom_sepc set command",             CLI_GUEST},
    {"Spec Get",         "sg",       _Sofac_Cust_Spec_Get,  NULL, "custom spec get command",          CLI_GUEST},
    {"Spec Help ",         "sh",      _Sofac_Cust_Spec_Help,  NULL, "custom spec help",          CLI_GUEST},
	{"MISC Get/Set Help ",   "mh",     _SoFac_Cust_MISC_Help,  NULL, "custom misc get/set  help",          CLI_GUEST},
	{"Misc Set",            "ms",     _Sofac_Cust_MISC_Set,        NULL, "custom misc set command",             CLI_GUEST},
    {"Misc Get",         "mg",       _Sofac_Cust_MISC_Get,  NULL, "custom misc get command",          CLI_GUEST},	
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};
extern CLI_EXEC_T _arSelfDiagCmdTbl[];	
static CLI_EXEC_T _arCustCmdTbl[] =
{
    {"dcust",            "dcust",     NULL,_ardcustCmdTbl, "customer d-interface command",CLI_GUEST},
	{"self-diag cmd",      "seld",      NULL, _arSelfDiagCmdTbl,"self-diag command",CLI_GUEST},
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};
CLI_MAIN_COMMAND_ITEM(Cust)
{
    "customer_cli",
    "cust_cli",
    NULL,
    _arCustCmdTbl,
    "customer_cli",
    CLI_GUEST
};
