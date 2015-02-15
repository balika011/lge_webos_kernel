/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2004, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $RCSfile: bepqapi.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *---------------------------------------------------------------------------*/
#ifndef _BEPQAPI_H_
#define _BEPQAPI_H_

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "u_common.h"
#include "Sony/drv_MapleApi.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/
#define RET_BEPQ_OK  (0)
#define RET_BEPQ_NG  (1)

/*-----------------------------------------------------------------------------
                    function declarations
-----------------------------------------------------------------------------*/
/* Customer specific APIs */
UINT32 BEPQ_AP_Set_Pic_Setting(CUST_PIC_SETTING_T rApPictureInfo);
UINT32 BEPQ_AP_Get_Pic_Setting(CUST_PIC_SETTING_T* prApPictureInfo);
UINT32 BEPQ_AP_Set_WideMode_Setting(CUST_SCREEN_SETTING_T rApScreenModeInfo);
UINT32 BEPQ_AP_Get_WideMode_Setting(CUST_SCREEN_SETTING_T* prApScreenModeInfo);
UINT32 BEPQ_AP_Set_3D_Setting(CUST_3D_SETTING_T rAp3DInfo);
UINT32 BEPQ_AP_Get_3D_Setting(CUST_3D_SETTING_T* prAp3DInfo);
UINT32 BEPQ_AP_Set_Eco_Setting(CUST_ECO_SETTING_T rApEcoInfo);
UINT32 BEPQ_AP_Get_Eco_Setting(CUST_ECO_SETTING_T* prApEcoInfo);
UINT32 BEPQ_AP_Set_CustService_Setting(CUST_SERVICE_SETTING_T rApCustServiceInfo);
UINT32 BEPQ_AP_Get_CustService_Setting(CUST_SERVICE_SETTING_T* prApCustServiceInfo);
UINT32 BEPQ_AP_Set_Input_Type(DRV_AP_INPUT_TYPE_T rApInpSrcInfo);
D_AP_INPUT_TYPE BEPQ_AP_Get_Input_Type(UINT8 bPath);
UINT32 BESYS_AP_Set_History_Data(CUST_HISTORY_DATA_T rApHistoryInfo);
UINT32 BESYS_AP_Get_History_Data(CUST_HISTORY_DATA_T* prApHistoryInfo);
UINT32 BESYS_AP_Set_Power_State(CUST_POWER_STATE_T rApPowerInfo);
UINT32 BESYS_AP_Get_Power_State(CUST_POWER_STATE_T* prApPowerInfo);
UINT32 BESYS_AP_Set_Screen_Saver(CUST_SCREEN_SAVER_T rApScreenSaverInfo);
UINT32 BESYS_AP_Get_Screen_Saver(CUST_SCREEN_SAVER_T* prApScreenSaverInfo);
UINT32 BESYS_AP_Set_Demo_Mode(CUST_DEMO_MODE_T rApDemoModeInfo);
UINT32 BESYS_AP_Get_Demo_Mode(CUST_DEMO_MODE_T* prApDemoModeInfo);
UINT32 BESYS_AP_Set_Demo_OSD_Mask(CUST_DEMO_OSD_MASK_T* rApDemoOSDMaskInfo);
UINT32 BESYS_AP_Get_Demo_OSD_Mask(CUST_DEMO_OSD_MASK_T* prApDemoOSDMaskInfo);
UINT32 BESYS_AP_Set_Aging_Mode(CUST_AGING_MODE_T rApAgingModeInfo);
UINT32 BESYS_AP_Get_Aging_Mode(CUST_AGING_MODE_T* prApAgingModeInfo);
UINT32 BESYS_AP_Set_Service_Trans(CUST_SERVICE_TRANS_T rApServiceTransInfo);
UINT32 BESYS_AP_Get_Service_Trans(CUST_SERVICE_TRANS_T* prApServiceTransInfo);
UINT32 BESYS_AP_Set_Service_VCom(CUST_SERVICE_VCOM_T rApServiceVComInfo);
UINT32 BESYS_AP_Get_Service_VCom(CUST_SERVICE_VCOM_T* prApServiceVComInfo);

#endif /* _BEPQAPI_H_ */
