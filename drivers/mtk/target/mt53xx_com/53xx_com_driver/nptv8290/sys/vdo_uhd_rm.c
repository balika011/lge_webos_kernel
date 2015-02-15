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

#include "x_assert.h"

#include "vdo_rm.h"
#include "x_os.h"
#include "drv_predown.h"
#include "drv_scaler.h"
#include "drv_scaler_psc.h"
#include "drv_tdtv.h"
#include "drv_tdtv_drvif.h"
#include "vdo_misc.h"
#include "drv_regen.h"
#include "vdp_display.h"
#include "drv_display.h"
#include "hw_sw.h"
#include "b2r_if.h"
#include "scpos_debug.h"
#include "drv_di.h"
#if defined(CC_MT5890) 
#include "drv_mjc.h"
#endif
#include "vdo_rm.h"
#include "drv_uhd.h"




//MON_OUT porting
//---start-----
//UHD_INFO_T   _rUhdInfo;
//UHD_INFO_T   _vrm_Uhd_info;


static UINT32     _u4VRMUhdEventFlag = 0;
//---end-----

//extern UINT32 IS_Support4KDD(void);

//============================================================
//MON_OUT porting
//
//============================================================
//---start----
UINT32 vVRMGetUhdEventFlg( void )
{
    return _u4VRMUhdEventFlag;
}

void vVRMSetUhdEventFlg( UINT32 u4UhdFlag )
{
    _u4VRMUhdEventFlag |= u4UhdFlag;
}



VOID vVRMClrUhdEventFlg( void )
{
    CRIT_STATE_T csState;
    csState = x_crit_start();
    _u4VRMUhdEventFlag = 0;
    x_crit_end(csState);
    return;
}
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void vVrmUhdSwInit( void )
{

}

//----------------------------------------------------------------------------
void vVRMUpdateUhdMUXin(void)
{

}

void vVRMUhdUpdateResolution( void )
{
        
    LOG(2, "VRM UHD Update resolution start.....\n");
    vVRMUpdateUhdMUXin();
    LOG(1, "VRM Update monitor out resolution done......\n");
    return;
}



void vVRM_UhdLoop( void )
{
	//if (0 ==((BSP_GetIcVersion() >= IC_VER_5890_AC) && IS_Support4KDD()&& PANEL_IsSupport4K2KDD()))
	{
       return;
	}   

#if 0
    CRIT_STATE_T csState;

     if(VRM_UHD_EVENT_NONE !=vVRMGetUhdEventFlg())
      {
          LOG(1,"vVRM_UhdLoop\n");      
          do
          {
              csState = x_crit_start();
          
              LOG(3, "========UHD Loop Event flag (0x%X) ========\n",vVRMGetUhdEventFlg());
      
              vVRMClrUhdEventFlg(); 
              x_crit_end(csState);
              
              vVRMUhdUpdateResolution();
      
          }while(VRM_MON_OUT_EVENT_NONE !=vVRMGetMonOutEventFlg());
      
          //x_memcpy((void*)&_vrm_mon_out_info,(void*)&_rMonitorOutInfo,sizeof(_rMonitorOutInfo));
      }
#endif	 
}
//============================================================
//
//============================================================






