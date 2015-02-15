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
 * $RCSfile: bepqapi.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *---------------------------------------------------------------------------*/
#include "bepqapi.h"
#include "ostg_if.h"
//#include "drv_MapleApi.h"
//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Imported funtions 
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Static/Global variables
//-----------------------------------------------------------------------------
CUST_PIC_SETTING_T      rApPicSetting={0};
CUST_SCREEN_SETTING_T   rApScreenSetting={0};
CUST_3D_SETTING_T       rAp3DSetting={0};
CUST_ECO_SETTING_T      rApEcoSetting={0};
CUST_SERVICE_SETTING_T  rApCustServiceSetting={0};
CUST_HISTORY_DATA_T     rApHistoryData={0};
CUST_POWER_STATE_T      rApPowerState={0};
CUST_INPUT_TYPE_T       rApInputType={E_CUST_INPUT_TYPE_UNKNOWN,E_CUST_INPUT_TYPE_UNKNOWN};
CUST_SCREEN_SAVER_T     rApScreenSaverData={0};
CUST_DEMO_MODE_T     	rApDemoModeData={0};
CUST_DEMO_OSD_MASK_T    rApDemoOSDMaskData={0};
CUST_AGING_MODE_T     	rApAgingModeData={0};
CUST_SERVICE_TRANS_T    rApServiceTransData={0};
CUST_SERVICE_VCOM_T    	rApServiceVComData={0};


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
UINT32 BEPQ_AP_Set_Pic_Setting(CUST_PIC_SETTING_T rApPictureInfo)
{
    rApPicSetting.ePicMode = rApPictureInfo.ePicMode;
	rApPicSetting.u1Backlight = rApPictureInfo.u1Backlight;
	rApPicSetting.u1Contrast = rApPictureInfo.u1Contrast;
	rApPicSetting.u1Brightness = rApPictureInfo.u1Brightness;
	rApPicSetting.u1Color = rApPictureInfo.u1Color;
	rApPicSetting.u1Hue = rApPictureInfo.u1Hue;
	rApPicSetting.u1Sharpness = rApPictureInfo.u1Sharpness;
	rApPicSetting.eColTmp = rApPictureInfo.eColTmp;
	rApPicSetting.eNoiseReduction = rApPictureInfo.eNoiseReduction;
	rApPicSetting.eMpegNoiseReduction = rApPictureInfo.eMpegNoiseReduction;
	rApPicSetting.eRealityCreation = rApPictureInfo.eRealityCreation;
	rApPicSetting.eMasteredIn4K = rApPictureInfo.eMasteredIn4K;
	rApPicSetting.i2Resolution = rApPictureInfo.i2Resolution;
	rApPicSetting.i2NoiseFiltering = rApPictureInfo.i2NoiseFiltering;
	rApPicSetting.eMotionFlow = rApPictureInfo.eMotionFlow;
	rApPicSetting.i2MotionFlowSmoothness = rApPictureInfo.i2MotionFlowSmoothness;
	rApPicSetting.i2MotionFlowClearness = rApPictureInfo.i2MotionFlowClearness;
	rApPicSetting.eFilmMode = rApPictureInfo.eFilmMode;
	rApPicSetting.eAdvContrastEnhancer = rApPictureInfo.eAdvContrastEnhancer;
	rApPicSetting.eBlackCorrector = rApPictureInfo.eBlackCorrector;
	rApPicSetting.i2Gamma = rApPictureInfo.i2Gamma;
	rApPicSetting.eLedDynamicCtrl = rApPictureInfo.eLedDynamicCtrl;
	rApPicSetting.eLiveColor = rApPictureInfo.eLiveColor;
	rApPicSetting.i2RGain = rApPictureInfo.i2RGain;
	rApPicSetting.i2GGain = rApPictureInfo.i2GGain;
	rApPicSetting.i2BGain = rApPictureInfo.i2BGain;
	rApPicSetting.i2RBias = rApPictureInfo.i2RBias;
	rApPicSetting.i2GBias = rApPictureInfo.i2GBias;
	rApPicSetting.i2BBias = rApPictureInfo.i2BBias;
	rApPicSetting.eSpeedPreference = rApPictureInfo.eSpeedPreference;
	rApPicSetting.eLedMotionMode = rApPictureInfo.eLedMotionMode;
	rApPicSetting.eXdr = rApPictureInfo.eXdr;
	rApPicSetting.eDrange = rApPictureInfo.eDrange;
	rApPicSetting.eColorSpace = rApPictureInfo.eColorSpace;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Get_Pic_Setting(CUST_PIC_SETTING_T* prApPictureInfo)
{
    prApPictureInfo->ePicMode = rApPicSetting.ePicMode;
	prApPictureInfo->u1Backlight = rApPicSetting.u1Backlight;
	prApPictureInfo->u1Contrast = rApPicSetting.u1Contrast;
	prApPictureInfo->u1Brightness = rApPicSetting.u1Brightness;
	prApPictureInfo->u1Color = rApPicSetting.u1Color;
	prApPictureInfo->u1Hue = rApPicSetting.u1Hue;
	prApPictureInfo->u1Sharpness = rApPicSetting.u1Sharpness;
	prApPictureInfo->eColTmp = rApPicSetting.eColTmp;
	prApPictureInfo->eNoiseReduction = rApPicSetting.eNoiseReduction;
	prApPictureInfo->eMpegNoiseReduction = rApPicSetting.eMpegNoiseReduction;
	prApPictureInfo->eRealityCreation = rApPicSetting.eRealityCreation;
	prApPictureInfo->eMasteredIn4K = rApPicSetting.eMasteredIn4K;
	prApPictureInfo->i2Resolution = rApPicSetting.i2Resolution;
	prApPictureInfo->i2NoiseFiltering = rApPicSetting.i2NoiseFiltering;
	prApPictureInfo->eMotionFlow = rApPicSetting.eMotionFlow;
	prApPictureInfo->i2MotionFlowSmoothness = rApPicSetting.i2MotionFlowSmoothness;
	prApPictureInfo->i2MotionFlowClearness = rApPicSetting.i2MotionFlowClearness;
	prApPictureInfo->eFilmMode = rApPicSetting.eFilmMode;
	prApPictureInfo->eAdvContrastEnhancer = rApPicSetting.eAdvContrastEnhancer;
	prApPictureInfo->eBlackCorrector = rApPicSetting.eBlackCorrector;
	prApPictureInfo->i2Gamma = rApPicSetting.i2Gamma;
	prApPictureInfo->eLedDynamicCtrl = rApPicSetting.eLedDynamicCtrl;
	prApPictureInfo->eLiveColor = rApPicSetting.eLiveColor;
	prApPictureInfo->i2RGain = rApPicSetting.i2RGain;
	prApPictureInfo->i2GGain = rApPicSetting.i2GGain;
	prApPictureInfo->i2BGain = rApPicSetting.i2BGain;
	prApPictureInfo->i2RBias = rApPicSetting.i2RBias;
	prApPictureInfo->i2GBias = rApPicSetting.i2GBias;
	prApPictureInfo->i2BBias = rApPicSetting.i2BBias;
	prApPictureInfo->eSpeedPreference = rApPicSetting.eSpeedPreference;
	prApPictureInfo->eLedMotionMode = rApPicSetting.eLedMotionMode;
	prApPictureInfo->eXdr = rApPicSetting.eXdr;
	prApPictureInfo->eDrange = rApPicSetting.eDrange;
	prApPictureInfo->eColorSpace = rApPicSetting.eColorSpace;

	return RET_BEPQ_OK;
}


UINT32 BEPQ_AP_Set_WideMode_Setting(CUST_SCREEN_SETTING_T rApScreenModeInfo)
{
    rApScreenSetting.eWideMode = rApScreenModeInfo.eWideMode;
    rApScreenSetting.eDisplayArea = rApScreenModeInfo.eDisplayArea;
	rApScreenSetting.i2HCenter = rApScreenModeInfo.i2HCenter;
	rApScreenSetting.i2VCenter = rApScreenModeInfo.i2VCenter;
	rApScreenSetting.i2VerticalSize = rApScreenModeInfo.i2VerticalSize;
    rApScreenSetting.eZoom = rApScreenModeInfo.eZoom;
    rApScreenSetting.ePcWIdeMode = rApScreenModeInfo.ePcWIdeMode;
	rApScreenSetting.i2PcPhase = rApScreenModeInfo.i2PcPhase;
	rApScreenSetting.i2PcPitch = rApScreenModeInfo.i2PcPitch;
	rApScreenSetting.i2PcHCenter = rApScreenModeInfo.i2PcHCenter;
	rApScreenSetting.i2PcVCenter = rApScreenModeInfo.i2PcVCenter;
	
	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Get_WideMode_Setting(CUST_SCREEN_SETTING_T* prApScreenModeInfo)
{
    prApScreenModeInfo->eWideMode = rApScreenSetting.eWideMode;
    prApScreenModeInfo->eDisplayArea = rApScreenSetting.eDisplayArea;
	prApScreenModeInfo->i2HCenter = rApScreenSetting.i2HCenter;
	prApScreenModeInfo->i2VCenter = rApScreenSetting.i2VCenter;
	prApScreenModeInfo->i2VerticalSize = rApScreenSetting.i2VerticalSize;
    prApScreenModeInfo->eZoom = rApScreenSetting.eZoom;
    prApScreenModeInfo->ePcWIdeMode = rApScreenSetting.ePcWIdeMode;
	prApScreenModeInfo->i2PcPhase = rApScreenSetting.i2PcPhase;
	prApScreenModeInfo->i2PcPitch = rApScreenSetting.i2PcPitch;
	prApScreenModeInfo->i2PcHCenter = rApScreenSetting.i2PcHCenter;
	prApScreenModeInfo->i2PcVCenter = rApScreenSetting.i2PcVCenter;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Set_3D_Setting(CUST_3D_SETTING_T rAp3DInfo)
{
    rAp3DSetting.e3dDisplay = rAp3DInfo.e3dDisplay;
	rAp3DSetting.i23dDepthAdjustment = rAp3DInfo.i23dDepthAdjustment;
	rAp3DSetting.e3dGlassBrightness = rAp3DInfo.e3dGlassBrightness;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Get_3D_Setting(CUST_3D_SETTING_T* prAp3DInfo)
{
    prAp3DInfo->e3dDisplay = rAp3DSetting.e3dDisplay;
	prAp3DInfo->i23dDepthAdjustment = rAp3DSetting.i23dDepthAdjustment;
	prAp3DInfo->e3dGlassBrightness = rAp3DSetting.e3dGlassBrightness;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Set_Eco_Setting(CUST_ECO_SETTING_T rApEcoInfo)
{
    rApEcoSetting.ePowerSaving = rApEcoInfo.ePowerSaving;
	rApEcoSetting.eLightSensor = rApEcoInfo.eLightSensor;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Get_Eco_Setting(CUST_ECO_SETTING_T* prApEcoInfo)
{
    prApEcoInfo->ePowerSaving = rApEcoSetting.ePowerSaving;
	prApEcoInfo->eLightSensor = rApEcoSetting.eLightSensor;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Set_CustService_Setting(CUST_SERVICE_SETTING_T rApCustServiceInfo)
{
    rApCustServiceSetting.u2RGain = rApCustServiceInfo.u2RGain;
	rApCustServiceSetting.u2ROffset = rApCustServiceInfo.u2ROffset;
    rApCustServiceSetting.u2GGain = rApCustServiceInfo.u2GGain;
	rApCustServiceSetting.u2GOffset = rApCustServiceInfo.u2GOffset;
    rApCustServiceSetting.u2BGain = rApCustServiceInfo.u2BGain;
	rApCustServiceSetting.u2BOffset = rApCustServiceInfo.u2BOffset;

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Get_CustService_Setting(CUST_SERVICE_SETTING_T* prApCustServiceInfo)
{
    prApCustServiceInfo->u2RGain = rApCustServiceSetting.u2RGain;
	prApCustServiceInfo->u2ROffset = rApCustServiceSetting.u2ROffset;
    prApCustServiceInfo->u2GGain = rApCustServiceSetting.u2GGain;
	prApCustServiceInfo->u2GOffset = rApCustServiceSetting.u2GOffset;
    prApCustServiceInfo->u2BGain = rApCustServiceSetting.u2BGain;
	prApCustServiceInfo->u2BOffset = rApCustServiceSetting.u2BOffset;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_History_Data(CUST_HISTORY_DATA_T rApHistoryInfo)
{
    rApHistoryData.u4TotalOpTime = rApHistoryInfo.u4TotalOpTime;
	rApHistoryData.u4PanelOpTime = rApHistoryInfo.u4PanelOpTime;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_History_Data(CUST_HISTORY_DATA_T* prApHistoryInfo)
{
    prApHistoryInfo->u4TotalOpTime = rApHistoryData.u4TotalOpTime;
	prApHistoryInfo->u4PanelOpTime = rApHistoryData.u4PanelOpTime;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Power_State(CUST_POWER_STATE_T rApPowerInfo)
{
    rApPowerState.ePanelState = rApPowerInfo.ePanelState;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Power_State(CUST_POWER_STATE_T* prApPowerInfo)
{
    UINT8 fgPanelStatus;
    //prApPowerInfo->ePanelState = rApPowerState.ePanelState;
	fgPanelStatus = bIsPanelOn();
	if(fgPanelStatus)
	{
	  prApPowerInfo->ePanelState = E_CUST_PANEL_STATE_ON;
	}
	else
	{
	  prApPowerInfo->ePanelState = E_CUST_PANEL_STATE_OFF;
	}

	return RET_BEPQ_OK;
}

UINT32 BEPQ_AP_Set_Input_Type(DRV_AP_INPUT_TYPE_T rApInpSrcInfo)
{
    rApInputType.eMainInputType = rApInpSrcInfo.eMainInputType;
	rApInputType.eSubInputType  = rApInpSrcInfo.eSubInputType;

	return RET_BEPQ_OK;
}

D_AP_INPUT_TYPE BEPQ_AP_Get_Input_Type(UINT8 bPath)
{
    if (bPath == SV_VP_MAIN)
        return rApInputType.eMainInputType;
	else
	    return rApInputType.eSubInputType;
}

UINT32 BESYS_AP_Set_Screen_Saver(CUST_SCREEN_SAVER_T rApScreenSaverInfo)
{
    rApScreenSaverData.eScreenSaverMode = rApScreenSaverInfo.eScreenSaverMode;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Screen_Saver(CUST_SCREEN_SAVER_T* prApScreenSaverInfo)
{
   	prApScreenSaverInfo->eScreenSaverMode = rApScreenSaverData.eScreenSaverMode;
	
	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Demo_Mode(CUST_DEMO_MODE_T rApDemoModeInfo)
{
    rApDemoModeData.eDemoMode = rApDemoModeInfo.eDemoMode;
	rApDemoModeData.eDemoPattern = rApDemoModeInfo.eDemoPattern;
	rApDemoModeData.u4DemoOption = rApDemoModeInfo.u4DemoOption;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Demo_Mode(CUST_DEMO_MODE_T* prApDemoModeInfo)
{
   	prApDemoModeInfo->eDemoMode = rApDemoModeData.eDemoMode;
	prApDemoModeInfo->eDemoPattern = rApDemoModeData.eDemoPattern;
	prApDemoModeInfo->u4DemoOption = rApDemoModeData.u4DemoOption;
	
	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Demo_OSD_Mask(CUST_DEMO_OSD_MASK_T* rApDemoOSDMaskInfo)
{
    x_memcpy(&rApDemoOSDMaskData, rApDemoOSDMaskInfo, sizeof(CUST_DEMO_OSD_MASK_T));

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Demo_OSD_Mask(CUST_DEMO_OSD_MASK_T* prApDemoOSDMaskInfo)
{
    x_memcpy(prApDemoOSDMaskInfo, &rApDemoOSDMaskData, sizeof(CUST_DEMO_OSD_MASK_T));
	
	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Aging_Mode(CUST_AGING_MODE_T rApAgingModeInfo)
{
    rApAgingModeData.bAgingMode = rApAgingModeInfo.bAgingMode;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Aging_Mode(CUST_AGING_MODE_T* prApAgingModeInfo)
{
   	prApAgingModeInfo->bAgingMode = rApAgingModeData.bAgingMode;
	
	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Service_Trans(CUST_SERVICE_TRANS_T rApServiceTransInfo)
{
    rApServiceTransData.eWBTransferMode = rApServiceTransInfo.eWBTransferMode;
	rApServiceTransData.eMuraTransferMode = rApServiceTransInfo.eMuraTransferMode;
	rApServiceTransData.eCUCTransferMode = rApServiceTransInfo.eCUCTransferMode;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Service_Trans(CUST_SERVICE_TRANS_T* prApServiceTransInfo)
{
   	prApServiceTransInfo->eWBTransferMode = rApServiceTransData.eWBTransferMode;
	prApServiceTransInfo->eMuraTransferMode = rApServiceTransData.eMuraTransferMode;
	prApServiceTransInfo->eCUCTransferMode = rApServiceTransData.eCUCTransferMode;
	
	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Set_Service_VCom(CUST_SERVICE_VCOM_T rApServiceVComInfo)
{
    rApServiceVComData.bMode = rApServiceVComInfo.bMode;
	rApServiceVComData.u2Data = rApServiceVComInfo.u2Data;

	return RET_BEPQ_OK;
}

UINT32 BESYS_AP_Get_Service_VCom(CUST_SERVICE_VCOM_T* prApServiceVComInfo)
{
   	prApServiceVComInfo->bMode = rApServiceVComData.bMode;
	prApServiceVComInfo->u2Data = rApServiceVComData.u2Data;
	
	return RET_BEPQ_OK;
}

