/*!
 * \file Sony_drv_cb.c
 *
 *  - Sony driver callback framework for Rafflesia Sony Library
 *
 *
 * SONY TVEG
 *
 * Copyright (c) SONY Corporation
 *
 * Author : Ramesh.Silvarajo@ap.sony.com : 23 July 2009 : Created
 *
 */

/*** Include files ******************************************************************/
#include "x_os.h"
#include "../sony_drv_inc/Sony_drv_cb.h"

/*** Constant definitions ***********************************************************/
#define ENABLE_CB_LOG

#if defined (ENABLE_CB_LOG)
#include "x_debug.h"
#define CB_LOG(fmt...)         Printf(fmt)
#else
#define CB_LOG(fmt...)            
#endif
/*** Type definitions ***************************************************************/

/*** Static variables ***************************************************************/

static SONY_DRV_CB_ST* cbdrv_func_set = NULL;

/*** Static functions ***************************************************************/

/*** Global functions ***************************************************************/

INT8 sonyDrvRegisterCallback(SONY_DRV_CB_ST* callback_funcs)
{
    INT8 ret = 0;
    
    if (cbdrv_func_set)
    {
      CB_LOG("%s: Callbacks already registered\n", __FUNCTION__);
      ret = -1;
    }
    else
    {
      cbdrv_func_set = callback_funcs;
    }

    return ret;
}

void vAutoBacklightGetDbcParam(ST_PARAM_DBC_MAIN* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_DBC_PARAM])
    {
        cbdrv_func_set->cbDrv[CB_DRV_DBC_PARAM]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vAutoBacklightGetDbc4psEnable(BOOL* dbc4psEnable)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_DBC4PS_ENABLE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_DBC4PS_ENABLE]((VOID*)dbc4psEnable);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vAutoBacklightGetDbcEnable(BOOL* dbcEnable)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_DBC_ENABLE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_DBC_ENABLE]((VOID*)dbcEnable);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vAutoBacklightGetNoSigGain(BOOL* noSigGain)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_NO_SIG_GAIN])
    {
        cbdrv_func_set->cbDrv[CB_DRV_NO_SIG_GAIN]((VOID*)noSigGain);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vAutoBacklightSetDimming(ST_PARAM_DIMMER* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_SET_DIMMING])
    {
        cbdrv_func_set->cbDrv[CB_DRV_SET_DIMMING]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

/* >>> BEGIN, cheah 31-Jul-09, callback for Sony GammaWB adj + AutoWB */
void vGammaOSDControl(BOOL* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_GAM_OSD_CONTROL])
    {
        cbdrv_func_set->cbDrv[CB_DRV_GAM_OSD_CONTROL]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vGammaSetPattern(ST_PARAM_GAM_PTG* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_GAM_PATGEN_GAM])
    {
        cbdrv_func_set->cbDrv[CB_DRV_GAM_PATGEN_GAM]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vGammaSetSCEPattern(ST_PARAM_SCE_PTG* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_GAM_PATGEN_SCE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_GAM_PATGEN_SCE]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vGammaLoadTable(VOID* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_GAM_LOADTBL])
    {
        cbdrv_func_set->cbDrv[CB_DRV_GAM_LOADTBL]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vWBSetGainOfst(ST_PARAM_WB_T* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_WB_GAINOFST])
    {
        cbdrv_func_set->cbDrv[CB_DRV_WB_GAINOFST]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

/* <<< END, cheah 31-Jul-09, callback for Sony GammaWB adj + AutoWB */


/* Ooi START - IP callback func (31 July 08) */
void vIPGetNoise (UINT8* noise_lvl)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IP_GET_NOISE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IP_GET_NOISE]((VOID*)noise_lvl);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}


void vIPGetSignalStatus (BOOL* sig_status)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IP_GET_SIGNAL_STATUS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IP_GET_SIGNAL_STATUS]((VOID*)sig_status);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vIPIsCVBS (BOOL* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IP_IS_CVBS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IP_IS_CVBS]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

void vIPIsChannelChange(BOOL* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IP_IS_CHANCHG])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IP_IS_CHANCHG]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}
/*Ooi:  END */

void vIsRgbSensor (UINT32* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IS_RGB_SENSOR])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IS_RGB_SENSOR]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

UINT16 vSIFWrite(UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddr,
                        UINT8 *pu1Buf, UINT16 u2ByteCnt)
{
    ST_PARAM_SIF_T param;
    
    param.u2ClkDiv = u2ClkDiv;
    param.u1DevAddr = u1DevAddr;
    param.u1WordAddr = u1WordAddr;
    param.pu1Buf = pu1Buf;
    param.u2ByteCnt = u2ByteCnt;
    param.u2Ret = 0;

    if (cbdrv_func_set->cbDrv[CB_DRV_SIF_WRITE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_SIF_WRITE]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	

    return param.u2Ret;
}

UINT16 vSIFRead(UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddr,
                        UINT8 *pu1Buf, UINT16 u2ByteCnt)
{
    ST_PARAM_SIF_T param;
    
    param.u2ClkDiv = u2ClkDiv;
    param.u1DevAddr = u1DevAddr;
    param.u1WordAddr = u1WordAddr;
    param.pu1Buf = pu1Buf;
    param.u2ByteCnt = u2ByteCnt;
    param.u2Ret = 0;
    
    if (cbdrv_func_set->cbDrv[CB_DRV_SIF_READ])
    {
        cbdrv_func_set->cbDrv[CB_DRV_SIF_READ]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
    
    return param.u2Ret;
}

void vAutoBacklightPwmSwitch(INT32 u1_switch)
{
    INT32 param = u1_switch;
    if (cbdrv_func_set->cbDrv[CB_DRV_BL_PWM_SWITCH])
    {
        cbdrv_func_set->cbDrv[CB_DRV_BL_PWM_SWITCH]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

void vAutoBacklightGet24P(BOOL* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_BL_24P_STATUS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_BL_24P_STATUS]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}
/* Ooi END */

UINT32 vSigBstrGetNoiseLevel(void)
{
    UINT32 noise_lvl=0;
    if (cbdrv_func_set->cbDrv[CB_DRV_LNA_GET_NOISE_LVL])
    {
        cbdrv_func_set->cbDrv[CB_DRV_LNA_GET_NOISE_LVL](&noise_lvl);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
    return noise_lvl;
}
UINT16 vSigBstrGetIFAGC(void)
{
    UINT16 ifagc=0;
    if (cbdrv_func_set->cbDrv[CB_DRV_LNA_GET_IFAGC])
    {
        cbdrv_func_set->cbDrv[CB_DRV_LNA_GET_IFAGC](&ifagc);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
    return ifagc;
}
BOOL vSigBstrSetLNA(BOOL setLNA)
{
    ST_PARAM_SetLNA param;
    
    param.fgLNA= setLNA;
    param.SetSuccess = 0;

    if (cbdrv_func_set->cbDrv[CB_DRV_LNA_SET_LNA])
    {
        cbdrv_func_set->cbDrv[CB_DRV_LNA_SET_LNA](&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
    return param.SetSuccess;
}

VOID* sony_x_mem_alloc(SIZE_T  z_size)
{
    PARAM_X_MEM_MGMT param;
    
    param.pv_mem_addr      = NULL;
    param.pv_mem_block     = NULL;
    param.z_size           = z_size;
    param.ui4_num_element  = 0;
    
    if (cbdrv_func_set->cbDrv[CB_DRV_X_MEM_ALLOC])
    {
        cbdrv_func_set->cbDrv[CB_DRV_X_MEM_ALLOC]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
    
    return param.pv_mem_addr;
}

VOID* sony_x_mem_calloc(UINT32  ui4_num_element, SIZE_T  z_size_element)
{
    PARAM_X_MEM_MGMT param;
    
    param.pv_mem_addr      = NULL;
    param.pv_mem_block     = NULL;
    param.z_size           = z_size_element;
    param.ui4_num_element  = ui4_num_element;
    
    if (cbdrv_func_set->cbDrv[CB_DRV_X_MEM_CALLOC])
    {
        cbdrv_func_set->cbDrv[CB_DRV_X_MEM_CALLOC]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
    
    return param.pv_mem_addr;
}

VOID* sony_x_mem_realloc(VOID*  pv_mem_block, SIZE_T z_new_size)
{
    PARAM_X_MEM_MGMT param;
    
    param.pv_mem_addr      = NULL;
    param.pv_mem_block     = pv_mem_block;
    param.z_size           = z_new_size;
    param.ui4_num_element  = 0;
    
    if (cbdrv_func_set->cbDrv[CB_DRV_X_MEM_REALLOC])
    {
        cbdrv_func_set->cbDrv[CB_DRV_X_MEM_REALLOC]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
    
    return param.pv_mem_addr;
}

VOID sony_x_mem_free(VOID*  pv_mem_block)
{
    PARAM_X_MEM_MGMT param;
    
    param.pv_mem_addr      = NULL;
    param.pv_mem_block     = pv_mem_block;
    param.z_size           = 0;
    param.ui4_num_element  = 0;
    
    if (cbdrv_func_set->cbDrv[CB_DRV_X_MEM_FREE])
    {
        cbdrv_func_set->cbDrv[CB_DRV_X_MEM_FREE]((VOID*)&param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

void vAutoBacklightGetDblParam(ST_PARAM_DBL_CTRL* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_DBL_PARAM])
    {
        cbdrv_func_set->cbDrv[CB_DRV_DBL_PARAM]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}
void vSonyTunerGetReg(ST_PARAM_ATD_GETSETREG* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_ATD_GETREG])
    {
        cbdrv_func_set->cbDrv[CB_DRV_ATD_GETREG]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}
void vSonyTunerSetAGCReg(ST_PARAM_ATD_GETSETREG* param)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_ATD_SETAGCPARA])
    {
        cbdrv_func_set->cbDrv[CB_DRV_ATD_SETAGCPARA]((VOID*)param);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}
void vSonyTunerGetDevData(void* pvArg)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_TUNER_DEVDATA_R])
    {
        cbdrv_func_set->cbDrv[CB_DRV_TUNER_DEVDATA_R]((VOID*)pvArg);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}
void vSonyTunerSetDevData(void* pvArg)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_TUNER_DEVDATA_W])
    {
        cbdrv_func_set->cbDrv[CB_DRV_TUNER_DEVDATA_W]((VOID*)pvArg);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}
void vSonyTunerGetIPStatus(void* pvArg)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_TUNER_GET_IP_STATUS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_TUNER_GET_IP_STATUS]((VOID*)pvArg);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vSonyIsPcHdmiPc(BOOL* pvArg)
{
    if (cbdrv_func_set->cbDrv[CB_DRV_IS_PC_HDMI_PC])
    {
        cbdrv_func_set->cbDrv[CB_DRV_IS_PC_HDMI_PC]((VOID*)pvArg);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }
}

void vSetBacklightGpioOffOn(INT32 u1_switch)
{   
    if (cbdrv_func_set->cbDrv[CB_DRV_BL_GPIO_SWITCH])
    {
        cbdrv_func_set->cbDrv[CB_DRV_BL_GPIO_SWITCH]((VOID*)&u1_switch);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

void vGetBacklightGetGpioStatus(BOOL* blGpioStatus)
{   
    if (cbdrv_func_set->cbDrv[CB_DRV_BL_ONOFF_STATUS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_BL_ONOFF_STATUS]((VOID*)blGpioStatus);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}

void vGetBacklightOsdStatus(BOOL* osdOn)
{   
    if (cbdrv_func_set->cbDrv[CB_DRV_BL_OSD_STATUS])
    {
        cbdrv_func_set->cbDrv[CB_DRV_BL_OSD_STATUS]((VOID*)osdOn);
    }
    else
    {
        CB_LOG("%s: NULL Callback provided - API abuse\n", __FUNCTION__);
    }	
}


