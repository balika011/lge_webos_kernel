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
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_dtmb_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_dtmb_glue.c
 *  DTMB glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifndef SONY_DUMMY_DEBUG
#include "u_tuner.h"
#include "x_os.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_dtmb_glue.h"
#include "tunerDebug.h"
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "drvcust_if.h"//Get Tuner/Demod I2C bus id
#include "x_gpio.h"
#include "vdr_PD_Def.h"

#else
#include "MTKdef.h"
#include "pd_dtmb_glue.h"
#endif

// for Sony CXD2840 driver
#include "sony_i2c_MTK.h"
#include "sony_cn_dtv_integ.h"
#include "sony_cn_dtv_demod.h"
#include "sony_cn_dtv_tuner.h"
#include "sony_cn_dtv_demod_monitorDtmb.h"
#include "sony_cn_dtv_demod_monitorDvbc.h"
#include "sony_math.h"
#include "cust_common_interface.h"

#define SONY_CXD2840_DRIVER_VERSION_STR "Sony CXD2840 v0.10 2014-06-13"

#ifndef SONY_DUMMY_DEBUG
enum
{
	SAW_BW_6M = 0,
	SAW_BW_7M,
	SAW_BW_8M,
    SAW_BW_5M
};
#endif // SONY_DUMMY_DEBUG

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Import extern variable
 
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Export definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define mcSEMA_LOCK_MTFE(sema)                                       \
{                                                                    \
    mcDBG_MSG_L4((" [ _Lock_%s-%d ]\r\n", __FUNCTION__, __LINE__));  \
    x_sema_lock(sema, X_SEMA_OPTION_WAIT);                           \
    mcDBG_MSG_L4((" [ _Escape_%s-%d ]\r\n", __FUNCTION__, __LINE__));\
}
#define mcSEMA_UNLOCK_MTFE(sema)                                     \
{                                                                    \
    mcDBG_MSG_L4((" [ _Unlock_%s-%d ]\r\n", __FUNCTION__, __LINE__));\
    x_sema_unlock(sema);                                             \
}

//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
static DTMB_PD_CTX_T *pPdCtx;
static TUNER_DEVICE_CTX_T*    pDtmbTunerDeviceCtx = NULL;

typedef struct _sony_demod_ctx_t // Sony CXD2840 Driver Specific
{
    // Instances to control CXD2840 driver
    sony_cn_dtv_demod_t demod;
    sony_cn_dtv_tuner_t tuner;
    sony_cn_dtv_integ_t integ;
    sony_i2c_t i2c;
    sony_i2c_t i2cTuner;

    x_break_fct breakFct; // Break function pointer to use in stopwatch function
    void* breakFctArg;    // Argument for break function
    UINT32 u4Uec;
} sony_demod_ctx_t; 

static sony_demod_ctx_t g_sony_demod_ctx;

#ifdef CC_PROJECT_FOR_SONY
// Sony Product Information
SONYPRODUCT_TYPEINFO g_sonyproduct_typeinfo;
#endif

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static BOOL sony_GetTsFmt(sony_cn_dtv_demod_t *pDemod, TS_FMT_T *pTsfmt)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    UINT8 data = 0;

    // Set Bank 0
    sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(sony_result != SONY_RESULT_OK){ return 0; }

    // OSERIALEN (Bank:00h, Addr:C1h, Bit[7])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC1, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->SerOrPar = (data & 0x80) ? SP_SERIAL : SP_PARALLEL;

    // OWFMT_CKDISBYBGAP (Bank:00h, Addr:C1h, Bit[2]
    pTsfmt->fgGatedClock = (data & 0x04) ? BOOL_TRUE : BOOL_FALSE;

    // OWFMT_CKINV (Bank:00h, Addr:C3h, Bit[0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC3, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->ClockPolarity = (data & 0x01) ? POL_HIGH_ACTIVE : POL_LOW_ACTIVE;

    // OTSDT_HIZ (Bank:00h, Addr:29h, Bit[7:0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x29, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->fgTristate = (data == 0xFF) ? BOOL_FALSE : BOOL_TRUE ;

    pTsfmt->u4TsClockInKHz = 82000;

    pTsfmt->fgExternalDemod = BOOL_TRUE;

    return 1;
}

static UINT32 sony_GetBER(void)
{
    uint32_t ber = 100000; // unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    
    /* unit is 1e5 */
    switch(pDemod->system)
    {
    case SONY_CN_DTV_SYSTEM_DTMB:
        sony_result = sony_cn_dtv_demod_MonitorDtmb_PreBchBER(pDemod, &ber); /* 1e9 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 5000) / 10000;
            mcDBG_MSG_L2(("-OK DTMB BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG DTMB BER\n"));
        }
        break;
    case SONY_CN_DTV_SYSTEM_DVBC:
        sony_result = sony_cn_dtv_demod_MonitorDvbc_PreRSBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-OK DVBC BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG DVBC BER\n"));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L2(("-NG def BER\n"));
        break;
    }

    return (UINT32)ber;
}

static UINT32 sony_GetPreBER(void)
{
    uint32_t ber = 100000; // unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    
    /* unit is 1e5 */
    switch(pDemod->system)
    {
    case SONY_CN_DTV_SYSTEM_DTMB:
        sony_result = sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-OK DTMB Pre LDPC BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG DTMB Pre LDPC BER\n"));
        }
        break;
    case SONY_CN_DTV_SYSTEM_DVBC:
        sony_result = sony_cn_dtv_demod_MonitorDvbc_PreRSBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-OK DVBC BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG DVBC BER\n"));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L2(("-NG def BER\n"));
        break;
    }

    return (UINT32)ber;
}
// This function is used in DtdCommand function
static UINT32 sony_strtoul(const char* str)
{
    const char* ptr = str;
    UINT32 value = 0;

    if(!str){
        return 0;
    }
    if((x_strlen(str) > 2) && (str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))){
        // Hex
        for(ptr = &(str[2]); *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value <<= 4; value += (UINT32)(*ptr - '0');
            }else if(('a' <= *ptr) && (*ptr <= 'f')){
                value <<= 4; value += (UINT32)(*ptr - 'a' + 10); 
            }else if(('A' <= *ptr) && (*ptr <= 'F')){
                value <<= 4; value += (UINT32)(*ptr - 'A' + 10);
            }else{
                break;
            }
        }
    }else{
        // Decimal
        for(ptr = str; *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value *= 10; value += (UINT32)(*ptr - '0');
            }else{
                break;
            }
        }
    }
    
    return value;
}

static const char* sony_FormatResult(sony_result_t result)
{
    switch(result){
    case SONY_RESULT_OK:
        return "OK";
    case SONY_RESULT_ERROR_ARG:
        return "ERROR_ARG";
    case SONY_RESULT_ERROR_I2C:
        return "ERROR_I2C";
    case SONY_RESULT_ERROR_SW_STATE:
        return "ERROR_SW_STATE";
    case SONY_RESULT_ERROR_HW_STATE:
        return "ERROR_HW_STATE";
    case SONY_RESULT_ERROR_TIMEOUT:
        return "ERROR_TIMEOUT";
    case SONY_RESULT_ERROR_UNLOCK:
        return "ERROR_UNLOCK";
    case SONY_RESULT_ERROR_RANGE:
        return "ERROR_RANGE";
    case SONY_RESULT_ERROR_NOSUPPORT:
        return "ERROR_NOSUPPORT";
    case SONY_RESULT_ERROR_CANCEL:
        return "ERROR_CANCEL";
    case SONY_RESULT_ERROR_OTHER:
        return "ERROR_OTHER";
    default:
        return "Unknown";
    }
}

static void DTMB_TunerGetModuInfor(void* pzAttribute)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    TUNER_TER_DIG_TUNE_STATUS_T *pAttribute = (TUNER_TER_DIG_TUNE_STATUS_T *) pzAttribute;

    if(pAttribute==NULL)
    {
        return;
    }

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    //add driver implementation code here
    x_memset(pAttribute, 0, sizeof(TUNER_TER_DIG_TUNE_STATUS_T));
    
    /* System Dependent Information */
    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB)
    {
        /* Carrier Mode */
        {
            sony_cn_dtv_dtmb_carrier_mode_t carrierMode = SONY_CN_DTV_DTMB_CARRIER_MODE_C1;
            sony_result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &carrierMode);
            if(sony_result == SONY_RESULT_OK){
                switch(carrierMode){
                case SONY_CN_DTV_DTMB_CARRIER_MODE_C1:
                    pAttribute->eDTMBarrierMode = CARRIER_MODE_C1;
                    mcDBG_MSG_L1(("CarrierMode : C1\n"));
                    break;
                case SONY_CN_DTV_DTMB_CARRIER_MODE_C3780:
                    pAttribute->eDTMBarrierMode = CARRIER_MODE_C3780;
                    mcDBG_MSG_L1(("CarrierMode : C3780\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("CarrierMode : Unknown (%d)\n", carrierMode));
                    break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_CarrierMode (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Pseudo Noise */
        {
            sony_cn_dtv_dtmb_pseudo_noise_t pnInfo = SONY_CN_DTV_DTMB_PN_420_VARIOUS;
            sony_result = sony_cn_dtv_demod_MonitorDtmb_PNInfo(pDemod, &pnInfo);
            if(sony_result == SONY_RESULT_OK){
                switch(pnInfo){
                case SONY_CN_DTV_DTMB_PN_420_VARIOUS:
                    pAttribute->eDTMBPnInfo = PN_420_VARIOUS;
                    mcDBG_MSG_L1(("Pseudo Noise : PN420-various\n"));
                    break;
                case SONY_CN_DTV_DTMB_PN_595_CONST:
                    pAttribute->eDTMBPnInfo = PN_595_CONST;
                    mcDBG_MSG_L1(("Pseudo Noise : PN595-constant\n"));
                    break;
                case SONY_CN_DTV_DTMB_PN_945_VARIOUS:
                    pAttribute->eDTMBPnInfo = PN_945_VARIOUS;
                    mcDBG_MSG_L1(("Pseudo Noise : PN945-various\n"));
                    break;
                case SONY_CN_DTV_DTMB_PN_420_CONST:
                    pAttribute->eDTMBPnInfo = PN_420_CONST;
                    mcDBG_MSG_L1(("Pseudo Noise : PN420-constant\n"));
                    break;
                case SONY_CN_DTV_DTMB_PN_945_CONST:
                    pAttribute->eDTMBPnInfo = PN_945_CONST;
                    mcDBG_MSG_L1(("Pseudo Noise : PN945-constant\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("Pseudo Noise : Unknown (%d)\n", pnInfo));
                    break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PNInfo (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* System Info */
        {
            sony_cn_dtv_dtmb_system_info_t systemInfo;
            sony_result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo(pDemod, &systemInfo);
            if(sony_result == SONY_RESULT_OK){
                switch(systemInfo.cnst){
                case SONY_CN_DTV_DTMB_CNST_4QAM:
                    mcDBG_MSG_L1(("Constellation : 4QAM\n"));
                    break;
                case SONY_CN_DTV_DTMB_CNST_4QAM_NR:
                    mcDBG_MSG_L1(("Constellation : 4QAM-NR\n"));
                    break;
                case SONY_CN_DTV_DTMB_CNST_16QAM:
                    mcDBG_MSG_L1(("Constellation : 16QAM\n"));
                    break;
                case SONY_CN_DTV_DTMB_CNST_32QAM:
                    mcDBG_MSG_L1(("Constellation : 32QAM\n"));
                    break;
                case SONY_CN_DTV_DTMB_CNST_64QAM:
                    mcDBG_MSG_L1(("Constellation : 64QAM\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("Constellation : Unknown (%d)\n", systemInfo.cnst));
                    break;
                }
                    
                switch(systemInfo.rate){
                case SONY_CN_DTV_DTMB_LDPC_RATE_0_4:
                    pAttribute->eDTMBCodeRate = LDPC_RATE_0_4;
                    mcDBG_MSG_L1(("LDPC Rate : 0.4\n"));
                    break;
                case SONY_CN_DTV_DTMB_LDPC_RATE_0_6:
                    pAttribute->eDTMBCodeRate = LDPC_RATE_0_6;
                    mcDBG_MSG_L1(("LDPC Rate : 0.6\n"));
                    break;
                case SONY_CN_DTV_DTMB_LDPC_RATE_0_8:
                    pAttribute->eDTMBCodeRate = LDPC_RATE_0_8;
                    mcDBG_MSG_L1(("LDPC Rate : 0.8\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("LDPC Rate : Unknown (%d)\n", systemInfo.rate));
                    break;
                }
                    
                switch(systemInfo.il){
                case SONY_CN_DTV_DTMB_FEC_IL_M_240:
                    mcDBG_MSG_L1(("Interleaver Mode : M_240\n"));
                    break;
                case SONY_CN_DTV_DTMB_FEC_IL_M_720:
                    mcDBG_MSG_L1(("Interleaver Mode : M_720\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("Interleaver Mode : Unknown (%d)\n", systemInfo.il));
                    break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_SystemInfo (%s)\n", sony_FormatResult(sony_result)));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Tuner function implementation necessary for integ code (Sony specific)
// Just call ITuner_XXXX APIs
//-----------------------------------------------------------------------------
static sony_result_t sony_tuner_MTK_Tune (sony_cn_dtv_tuner_t * pTuner, 
                                          uint32_t frequencyKHz, 
                                          sony_cn_dtv_system_t system,uint8_t autosearch)
{
    SONY_TRACE_ENTER ("sony_tuner_MTK_Tune");
    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    
    pTuner->system = system;
    pTuner->frequency = frequencyKHz;

    {
        PARAM_SETFREQ_T param;
        UINT8 sawbw = 0;
        
        param.Freq = frequencyKHz;
        param.fgAutoSearch=autosearch;
        switch(system)
        {
        case SONY_CN_DTV_SYSTEM_DTMB:
        default:
            param.Modulation = MOD_DTMB;
            break;
        case SONY_CN_DTV_SYSTEM_DVBC:
            param.Modulation = MOD_DVBC;
            break;
        }

        // Set Bandwidth
        sawbw = SAW_BW_8M;

        // Call ITuner_OP to set BW
        ITuner_OP(ITunerGetCtx(), itSetSawBw, sawbw, NULL);

        // Set Tuner PLL
        if(ITuner_SetFreq(ITunerGetCtx(), &param))
        {
            mcDBG_MSG_ERR(("ITuner_SetFreq failed\n"));
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
        }
        
        mcDBG_MSG_L2(("RF tuner control finished.\n"));
        mcDBG_MSG_L2(("Frequency = %d\n", param.Freq));
        mcDBG_MSG_L2(("Modulation = %d.\n", param.Modulation));
        mcDBG_MSG_L2(("Bandwidth = %d.\n", sawbw));
        mcDBG_MSG_L2(("fgAutoSearch = %d.\n", param.fgAutoSearch));
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t sony_tuner_MTK_Create (sony_cn_dtv_tuner_t * pTuner)
{
    SONY_TRACE_ENTER ("sony_tuner_MTK_Create");
    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pTuner->Initialize = NULL;
    pTuner->Finalize = NULL;
    pTuner->Sleep = NULL;
    pTuner->Tune = sony_tuner_MTK_Tune;
    pTuner->system = SONY_CN_DTV_SYSTEM_UNKNOWN;
    pTuner->frequency = 0;
    pTuner->i2cAddress = 0; // Dummy
    pTuner->pI2c = NULL; // Dummy
    pTuner->flags = 0;
    pTuner->user = NULL;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/* This API is called from integ code to check break */
sony_result_t sony_integ_MTK_CheckBreak (void)
{
    SONY_TRACE_ENTER ("sony_integ_MTK_CheckBreak");

    if(g_sony_demod_ctx.breakFct){
        if(g_sony_demod_ctx.breakFct(g_sony_demod_ctx.breakFctArg)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_CANCEL);
        }
    }

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

// Get RSSI value for SSI monitors.
// This function should return dB * 1000 value.
sony_result_t sony_integ_monitor_RFLevel_MTK_DTMB (sony_cn_dtv_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;
    INT32 rssi = 0;

    SONY_TRACE_ENTER ("sony_integ_monitor_RFLevel_MTK");

    if (!pRFLeveldB) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    // Call ITuner_OP to get RSSI
    // NOTE: The RSSI value is dB * 100 integer.
    if(ITuner_OP(ITunerGetCtx(), itGetRSSI, 0, &rssi) == ITUNER_OK){
        *pRFLeveldB = 10 * rssi;
    }else{
        *pRFLeveldB = -100 * 1000;
        result = SONY_RESULT_ERROR_OTHER;
    }

    SONY_TRACE_RETURN (result);
}

//-----------------------------------------------------------------------------
// DTMB SQI/SSI (NOT FORMAL)
//-----------------------------------------------------------------------------
/* The following Quality monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation. */
static sony_result_t sony_demod_dtmb_monitor_Quality (sony_cn_dtv_demod_t * pDemod, uint8_t * pQuality)
{
    sony_result_t result = SONY_RESULT_OK;
    int32_t snr = 0;
    int32_t snrRel = 0;
    uint32_t ber = 0;
    uint32_t berSQI = 0;
    int32_t tmp = 0;
    sony_cn_dtv_dtmb_carrier_mode_t carrierMode;
    sony_cn_dtv_dtmb_system_info_t systemInfo;

    /* The list of DTMB SNR receiver performance requirement values in dBx1000. 
     * Entries equalling 0 indicate invalid code rate and constellation combination.
     */

    static const int32_t snrNordigP1dB1000_C1[5][3] = {
    /*   0.4,   0.6,    0.8     */
        {3400,  5200,   7300},  /* 4QAM    */
        {0,     0,      4400},  /* 4QAM-NR */
        {8700,  11000,  13700}, /* 16-QAM  */
        {0,     0,      16700}, /* 32-QAM  */
        {13100, 16000,  19400}  /* 64-QAM  */
    };

    static const int32_t snrNordigP1dB1000_C3780[5][3] = {
    /*   0.4,   0.6,    0.8     */
        {3400,  5200,   7400},  /* 4QAM    */
        {0,     0,      4400},  /* 4QAM-NR */
        {9300,  11200,  13800}, /* 16-QAM  */
        {0,     0,      17100}, /* 32-QAM  */
        {13900, 16400,  19700}  /* 64-QAM  */
    };

    SONY_TRACE_ENTER ("sony_demod_dtmb_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CN_DTV_SYSTEM_DTMB) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre BCH BER. */
    result = sony_cn_dtv_demod_MonitorDtmb_PreBchBER (pDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

     /* Get SNR */
    result = sony_cn_dtv_demod_MonitorDtmb_MER (pDemod, &snr);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get carrier mode */
    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode (pDemod, &carrierMode);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get system information. */
    result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo (pDemod, &systemInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct info. */
    if ((systemInfo.rate > SONY_CN_DTV_DTMB_LDPC_RATE_0_8) || (systemInfo.cnst > SONY_CN_DTV_DTMB_CNST_64QAM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /*
     * SQI (Signal Quality Indicator) given by:
     * SQI = (((C/Nrel - 3) / 10) + 1) * BER_SQI 
     * 
     * 
     * BER = received pre BCH ber
     */
     
    /* BER_SQI is calculated from:
     * if (BER > 10^-3)          : 0
     * if (10^-7 < BER <= 10^-3) : BER_SQI = 20*log10(1/BER) - 40
     * if (BER <= 10^-7)         : BER_SQI = 100
      */
    if (ber > 10000) {
        berSQI = 0;
    }  else if (ber > 1) {
        /* BER_SQI = 20 * log10(1/BER) - 40
         * BER_SQI = 20 * (log10(1) - log10(BER)) - 40
         * 
         * If BER in units of 1e-7
         * BER_SQI = 20 * (log10(1) - (log10(BER) - log10(1e7)) - 40
         * 
         * BER_SQI = 20 * (log10(1e7) - log10(BER)) - 40
         * BER_SQI = 20 * (7 - log10 (BER)) - 40
         */
        berSQI = (int32_t) (10 * sony_math_log10_DTMB (ber));
        berSQI = 20 * (7 * 1000 - (berSQI)) - 40 * 1000;
    }
    else {
        berSQI = 100 * 1000;
    }
  
    if (carrierMode == SONY_CN_DTV_DTMB_CARRIER_MODE_C1) {
        snrRel = snr - snrNordigP1dB1000_C1[systemInfo.cnst][systemInfo.rate];
    } else {
        snrRel = snr - snrNordigP1dB1000_C3780[systemInfo.cnst][systemInfo.rate];
    }

    if ( snrRel < -7000 ) {
        *pQuality = 0;
        SONY_TRACE_RETURN (result);
    } 
    else if ( snrRel >= 3000 ) {
        *pQuality = (uint8_t) ((berSQI + 500) / 1000);
    } 
    else {
        tmp = (((snrRel-3000) / 10) + 1000);
        *pQuality = (uint8_t) ((((uint32_t)tmp * berSQI) + (1000 * 1000 / 2)) / (1000 * 1000));
    }
 
    if (*pQuality > 100) {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

/* The following SSI monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation. 
 *
 * NOTE : This function calls the integration layer RF Level monitor which is 
 * HW implementation dependant, therefore the SSI level may be incorrect RF Level 
 * is correctly characterised. */
static sony_result_t sony_integ_dtmb_monitor_SSI (sony_cn_dtv_integ_t * pInteg, uint8_t * pSSI)
{
    int32_t rfLevel;
    int32_t prel;
    int32_t tempSSI = 0;
    sony_result_t result = SONY_RESULT_OK;
    sony_cn_dtv_dtmb_carrier_mode_t carrierMode;
    sony_cn_dtv_dtmb_system_info_t systemInfo;

    static const int32_t pRefdBm1000_C1[5][3] = {
    /*    0.4,    0.6,    0.8     */
        {-96400, -94600, -92500}, /* 4QAM    */
        {0,      0,      -95400}, /* 4QAM-NR */
        {-91100, -88800, -86100}, /* 16-QAM  */
        {0,      0,      -83100}, /* 32-QAM  */
        {-86700, -83800, -80400}  /* 64-QAM  */
    };

    static const int32_t pRefdBm1000_C3780[5][3] = {
    /*    0.4,    0.6,    0.8     */
        {-96400, -94600, -92400}, /* 4QAM    */
        {0,      0,      -95400}, /* 4QAM-NR */
        {-90500, -88600, -86000}, /* 16-QAM  */
        {0,      0,      -82700}, /* 32-QAM  */
        {-85900, -83400, -80100}  /* 64-QAM  */
    };

    SONY_TRACE_ENTER ("sony_integ_dtmb_monitor_SSI");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_CN_DTV_SYSTEM_DTMB) {
        /* Not DTMB */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level (for MTK) */
    result = sony_integ_monitor_RFLevel_MTK_DTMB (pInteg, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get carrier mode */
    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode (pInteg->pDemod, &carrierMode);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get system information. */
    result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo (pInteg->pDemod, &systemInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct info. */
    if ((systemInfo.rate > SONY_CN_DTV_DTMB_LDPC_RATE_0_8) || (systemInfo.cnst > SONY_CN_DTV_DTMB_CNST_64QAM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* prel = prec - pref */
    if (carrierMode == SONY_CN_DTV_DTMB_CARRIER_MODE_C1) {
        prel = rfLevel - pRefdBm1000_C1[systemInfo.cnst][systemInfo.rate];
    } else {
        prel = rfLevel - pRefdBm1000_C3780[systemInfo.cnst][systemInfo.rate];
    }

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)             SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = (4 * prel) + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    SONY_TRACE_RETURN (result);
}


//-----------------------------------------------------------------------------
// DVB-C SQI/SSI (NOT FORMAL)
//-----------------------------------------------------------------------------
/* The following Quality monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation. */
static sony_result_t sony_demod_dvbc_monitor_Quality (sony_cn_dtv_demod_t * pDemod, uint8_t * pQuality)
{
    uint32_t ber = 0;
    int32_t snr = 0;
    int32_t snRel = 0;
    int32_t berSQI = 0;
    sony_cn_dtv_dvbc_cnst_t constellation;

    /* Nordig spec C/N (Es/No) minimum performance 
     * Note: 32QAM isn't provided in the Nordig unified specification, so has been
     * Implemented based on interpolation and measurements. 
     */
    static const int32_t cnrNordigdB1000[] = {
    /*  16QAM   32QAM   64QAM   128QAM  256QAM */
        20000,  23000,  26000,  29000,  32000  };

    sony_result_t result = SONY_RESULT_OK;
    
    SONY_TRACE_ENTER ("sony_demod_dvbc_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CN_DTV_SYSTEM_DVBC) {
        /* Not DVB-C*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre-RS (Post-Viterbi) BER. */
    result = sony_cn_dtv_demod_MonitorDvbc_PreRSBER (pDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR value. */
    result = sony_cn_dtv_demod_MonitorDvbc_SNR (pDemod, &snr);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    /* Get Code rate */
    result = sony_cn_dtv_demod_MonitorDvbc_QAM (pDemod, &constellation);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }
       
    /* Calculate. */
    snRel = snr - cnrNordigdB1000[(uint8_t)constellation];
   
    /* BER_SQI is calculated from:
     * if (BER > 10^-3)          : 0
     * if (10^-7 < BER <= 10^-3) : BER_SQI = 20*log10(1/BER) - 40
     * if (BER <= 10^-7)         : BER_SQI = 100
      */
    if (ber > 10000) {
        berSQI = 0;
    }
    else if (ber > 1) {
        /* BER_SQI = 20 * log10(1/BER) - 40
         * BER_SQI = 20 * (log10(1) - log10(BER)) - 40
         * 
         * If BER in units of 1e-7
         * BER_SQI = 20 * (log10(1) - (log10(BER) - log10(1e7)) - 40
         * 
         * BER_SQI = 20 * (log10(1e7) - log10(BER)) - 40
         * BER_SQI = 20 * (7 - log10 (BER)) - 40
         */
        berSQI = (int32_t) (10 * sony_math_log10_DTMB (ber));
        berSQI = 20 * (7 * 1000 - (berSQI)) - 40 * 1000;
    }
    else {
        berSQI = 100 * 1000;
    }

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -7dB)         : SQI = 0
     * if (-7dB <= C/Nrel < +3dB) : SQI = (((C/Nrel - 3) / 10) + 1) * BER_SQI
     * if (C/Nrel >= +3dB)        : SQI = BER_SQI
     */
    if (snRel < -7 * 1000) {
        *pQuality = 0;
    }
    else if (snRel < 3 * 1000) {
        int32_t tmpSQI = (((snRel - (3 * 1000)) / 10) + 1000);
        *pQuality = (uint8_t) (((tmpSQI * berSQI) + (1000000/2)) / (1000000)) & 0xFF;
    }
    else {
        *pQuality = (uint8_t) ((berSQI + 500) / 1000);
    }

    /* Clip to 100% */
    if (*pQuality > 100) {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

/* The following SSI monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation. 
 *
 * NOTE : This function calls the integration layer RF Level monitor which is 
 * HW implementation dependant, therefore the SSI level may be incorrect RF Level 
 * is correctly characterised. */
static sony_result_t sony_integ_dvbc_monitor_SSI (sony_cn_dtv_integ_t * pInteg, uint8_t * pSSI)
{
    sony_cn_dtv_dvbc_cnst_t constellation;
    uint32_t symbolRate;
    int32_t prec;
    int32_t prel;
    int32_t pref=0;
    int32_t tempSSI = 0;  
    int32_t noiseFigureDB1000;
    sony_result_t result = SONY_RESULT_OK;

    /* Nordig spec C/N (Es/No) minimum performance 
     * Note: 32QAM isn't provided in the Nordig unified specification, so has been
     * Implemented based on interpolation and measurements. 
     */
    static const int32_t cnrNordigdB1000[] = {
    /*  16QAM   32QAM   64QAM   128QAM  256QAM */
        20000,  23000,  26000,  29000,  32000  };

    SONY_TRACE_ENTER ("sony_integ_dvbc_monitor_SSI");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_CN_DTV_SYSTEM_DVBC) {
        /* Not DVB-C*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level (for MTK) */
    result = sony_integ_monitor_RFLevel_MTK_DTMB (pInteg, &prec);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Monitor constellation */
    result = sony_cn_dtv_demod_MonitorDvbc_QAM (pInteg->pDemod, &constellation);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Monitor symbol rate */
    result = sony_cn_dtv_demod_MonitorDvbc_SymbolRate(pInteg->pDemod, &symbolRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }    
    
    /* Modify this to suit the tuner noise figure specification value in dB * 1000 */
   noiseFigureDB1000 = 9000; 

    /* Reference sensitivity limit is calcualted from:
     * RefLevel (dB) = (10 * Log(1.38*10^-23 * 290)) + 30 + NoiseFigure + (10 * Log(symbolRate)) + C/N_Offset
     *  - sony_math_log10(x)     = 100 * Log(x)
     *  - Log(1.38*10^-23 * 290) = -20.3977
     *
     * Therefore:
     * RefLevel (dB * 1000) = -203977 + 30000 + (1000 * NoiseFigure) + (100 * Log(symbolRate)) + (1000 * C/N_Offset)
     */
    pref = (int32_t)(-203977 + 30000 + noiseFigureDB1000 + (int32_t)(100 * sony_math_log10_DTMB(symbolRate* 1000)) + cnrNordigdB1000[(uint8_t)constellation]);

    /* prel = prec - pref */
    prel = prec - pref;

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)              SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = 4 * prel + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    SONY_TRACE_RETURN (result);
}

/* ========================================================================== */

static BOOL SetBreakMonitor(DTMB_PD_CTX_T *psPdCtx)
{
    UINT8 u1Cnt=0;
    
    psPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psPdCtx->fgPIMoniStatus){  //PI really doing PI Monitor
        while(psPdCtx->fgPIMoniStatus){ 
            //wait PI Monitor really finish break 
            if(u1Cnt>250){  //in case of deadlock
                break;   
            }
            u1Cnt++;
            mcDBG_MSG_L2(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
            //  mcDELAY_MS(10); 
            x_thread_delay((UINT32)(10));
        }
    }
    
    return TRUE;
}
//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------

static VOID vResetDemod(UINT16 u2DelayTime)
{
    UINT32 u4RstPinNum;

    // Config GPIO to reset demod
    mcDBG_MSG_L2(("vResetDemod\n"));
    if (DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum) == 0)
    {
        GPIO_SetOut(u4RstPinNum, 0);
        x_thread_delay((UINT32)u2DelayTime);
        GPIO_SetOut(u4RstPinNum, 1);
        x_thread_delay((UINT32)20); // Delay 20 ms for CR[DTV00030459]. Or it will init fail in rel image
        mcDBG_MSG_L2(("Reset CXD2840 Demod GPIO=0x%x, 0->1, %dms\r\n",u4RstPinNum, u2DelayTime));
    }
}

static void ResumeMonitor(DTMB_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgBreakMonitor = FALSE;
   mcSEMA_UNLOCK_MTFE(psPdCtx->t_escape_mon_suspend);
}
/************************************************************************
*  MonitorProcess
*  Monitor thread process function.
*  @param  void.
*  @retval  void.
************************************************************************/
static void MonitorProcess(VOID *pvArg)
{
    UINT8 uNcount = 0;
    DTMB_PD_CTX_T *psPdCtx = *(DTMB_PD_CTX_T **) pvArg;
    
    mcDBG_MSG_INFO(("MonitorProcess start!\n"));
        
    while (psPdCtx->fgMonitorThreadRunning)
    {

        if (psPdCtx->fgBreakMonitor)
        {
            mcDBG_MSG_L2(("Entering DTMB mon_suspend\n"));
            mcSEMA_LOCK_MTFE(psPdCtx->t_escape_mon_suspend);
            mcDBG_MSG_L2(("Escape DTMB mon_suspend\n"));
        }
        else
        {
            mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
            psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor

            if(uNcount == 1){
                UINT32 packetErrorNum = 0;
                UINT64 temp = 0;
                sony_result_t sony_result = SONY_RESULT_OK;
                sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
                uNcount = 0;
                switch(pDemod->system){
                case SONY_CN_DTV_SYSTEM_DTMB:
                    sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum(pDemod, &packetErrorNum); break;
                case SONY_CN_DTV_SYSTEM_DVBC:
                    sony_result = sony_cn_dtv_demod_MonitorDvbc_RSErrorNum(pDemod, &packetErrorNum); break;
                default:
                    sony_result = SONY_RESULT_ERROR_SW_STATE; break;
                }
                if(sony_result == SONY_RESULT_OK){
                    temp = (UINT64)(g_sony_demod_ctx.u4Uec) + (UINT64)packetErrorNum;
                    if(temp > 0xffffffff){
                        temp = 0xffffffff;
                        g_sony_demod_ctx.u4Uec = (UINT32)temp;
                        mcDBG_MSG_L2(("UEC(max) : %lu\n", g_sony_demod_ctx.u4Uec));
                    }else{
                        g_sony_demod_ctx.u4Uec = (UINT32)temp;
                        mcDBG_MSG_L2(("UEC : %lu\n", g_sony_demod_ctx.u4Uec));
                    }
                }else{
                    mcDBG_MSG_L2(("Error in Packet Error Number monitor (%s)\n", sony_FormatResult(sony_result)));
                }
            }else{
                uNcount ++;
            }

            psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
            mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock);
            x_thread_delay((UINT32)(500)); 
        }
    }
    
    psPdCtx->t_monitor_thread = (THREAD_T)NULL;
    mcDBG_MSG_INFO(("MonitorProcess exit!\n"));
   x_thread_exit();
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static INT32 PD_StartMonitorEngine(DTMB_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgMonitorThreadRunning = TRUE;
    if(x_thread_create(&psPdCtx->t_monitor_thread,
              "DTMB_MonitorThread",
              cTHREAD_STACK_SIZE,cTHREAD_PRIORITY,
              MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
    {
        mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcDBG_MSG_INFO(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  PD_StopMonitorEngine
*  Function which stops monitor thread engine.
*  @param  void.
*  @retval  DRVAPI_TUNER_OK : Stop monitor thread successfully.
************************************************************************/
static INT32 PD_StopMonitorEngine(void)
{
    while (pPdCtx->t_monitor_thread){
        x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_L1(("DTMB PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}
//-------------------------------------------------------------------------
/**
 *  This API implement close tuner. It should performs
 *  1. free all allocated resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
static INT32 DTMB_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
     
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtx->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
     mcSEMA_UNLOCK_MTFE(pPdCtx->t_escape_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    x_sema_delete(pPdCtx->t_escape_mon_suspend); 

    //add driver implementation code here
    x_sema_delete(pPdCtx->hHalLock);
    x_mem_free(pPdCtx);
    //x_mem_free(psDemodCtx);

    //To Do If you have allocated any resource in TunerOpen(), release them here.

    return (DRVAPI_TUNER_OK);
}
//-------------------------------------------------------------------------
/**
 *  This API do channel acuqusition.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @param   pv_conn_info       middleware tuner info structure, refer u_tuner.h
 *  @param   z_conn_info_len    length of pv_conn_info
 *  @param   _BreakFct          TunerHAL callback API for Vendor_TunerAcq to check whether need to break TunerAcq
 *  @param   *pvArg             pass this parameter to _BreakFct when calling _BreakFct
 *  @retval  1                  TPS LOCK
 *  @retval  0                  TPS UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DTMB_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    UINT32 u4Freq_KHz = 0;
    BOOL fgChannelScan = FALSE;   
    BOOL fgLock = FALSE;

    // CXD2840 driver specific
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;

    // Set break function pointer to the member to call it from integ APIs.
    g_sony_demod_ctx.breakFct = _BreakFct;
    g_sony_demod_ctx.breakFctArg = pvArg;

    mcDBG_MSG_L2((SONY_CXD2840_DRIVER_VERSION_STR "\n"));

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Pause monitor thread
    if(pDtmbTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDtmbTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        SetBreakMonitor(pPdCtx);
    }
    //Parse parameter from upper layer
    
    //IF AGC Switch Control
    if(g_sonyproduct_typeinfo.eProjectID == 0x00){ //Sakura 
        UINT32 u4GpioforRFAGCSwitch;

        if( 0 == DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4GpioforRFAGCSwitch))
        {
	        GPIO_SetOut(u4GpioforRFAGCSwitch,0);//d20140628_Xiaohong:Gpio set Low for DTV
	        mcDBG_MSG_L2(("GPIO %d set low to switch IF AGC control\n",u4GpioforRFAGCSwitch));
        }
    }
    
    pPdCtx->isBreak = _BreakFct;
    
    if(e_conn_type == TUNER_CONN_TYPE_CAB_DIG){
        TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_CAB_DIG_TUNE_INFO_T *) pv_conn_info;
        if(CONN_MODE_SCAN == pt_tuner_info->e_conn_mode)
        {
            fgChannelScan = TRUE;
        }

        u4Freq_KHz = pt_tuner_info->ui4_freq/1000;
        pInteg->frequency = u4Freq_KHz;
        pInteg->system = SONY_CN_DTV_SYSTEM_DVBC;
        pInteg->autoscan = (uint8_t)fgChannelScan;
        mcDBG_MSG_L1(("System = DVB-C\n"));
    }else if(e_conn_type == TUNER_CONN_TYPE_TER_DIG){
        TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *) pv_conn_info;
        if(MOD_UNKNOWN == pt_tuner_info->e_mod)
        {
            fgChannelScan = TRUE;
        }

        u4Freq_KHz = pt_tuner_info->ui4_freq/1000;
        pInteg->frequency = u4Freq_KHz;
        pInteg->system = SONY_CN_DTV_SYSTEM_DTMB;
        pInteg->autoscan = (uint8_t)fgChannelScan;
        mcDBG_MSG_L1(("System = DTMB\n"));
    }else{
        mcDBG_MSG_L1(("Error TUNER CONN TYPE = %d!\n", e_conn_type));
        goto ErrorExit;
    }

    mcDBG_MSG_L1(("Freq = %d\n", pInteg->frequency));

    pInteg->isCancel = 0;  // Clear cancel flag
    pInteg->presetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_NONE;  // Preset OFF

    mcDBG_MSG_L2(("Acq freq=%dKHz,fgChannelScan=%d\n",u4Freq_KHz,fgChannelScan));

    // Set scan mode
    sony_result = sony_cn_dtv_demod_SetConfig(pInteg->pDemod, SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE, fgChannelScan ? 1 : 0);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s)\n", sony_FormatResult(sony_result)));
        fgLock = 0;
        goto ErrorExit;
    }

    sony_result = sony_cn_dtv_integ_Tune(pInteg);
    switch(sony_result){
    case SONY_RESULT_OK:
        mcDBG_MSG_L1(("TS Locked.\n"));
        fgLock = 1;
        break;
    case SONY_RESULT_ERROR_UNLOCK:
        mcDBG_MSG_L1(("TS Unlocked.\n"));
        fgLock = 0;
        break;
    case SONY_RESULT_ERROR_TIMEOUT:
        mcDBG_MSG_L1(("Wait TS Lock but Timeout.\n"));
        fgLock = 0;
        break;
    default:
        mcDBG_MSG_L1(("Error in sony_cn_dtv_integ_Tune (%s)\n", sony_FormatResult(sony_result)));
        fgLock = 0;
        break;
    }

ErrorExit:
    // Clear break function pointer
    g_sony_demod_ctx.breakFct = NULL;
    g_sony_demod_ctx.breakFctArg = NULL;
    g_sony_demod_ctx.u4Uec = 0;
    
    if(pDtmbTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDtmbTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg) && (!fgChannelScan))
        {
            ResumeMonitor(pPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
            mcDBG_MSG_L2(("Enable monitor process!!\n"));
        }
    }

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    // TODO return correct lock status
    return (fgLock);    //if lock return 1, unlock return 0
}
//-------------------------------------------------------------------------
/**
 *  This API check lock status
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  1                  LOCK
 *  @retval  0                  UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DTMB_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    BOOL fgLock = FALSE;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Get demod lock status from demod driver and update it as variable fgLock

    {
        uint8_t dmdLock = 0;
        uint8_t tsLock = 0;
        uint8_t unlock = 0;

        switch(pDemod->system){
        case SONY_CN_DTV_SYSTEM_DTMB:
            sony_result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
            break;
        case SONY_CN_DTV_SYSTEM_DVBC:
            sony_result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &dmdLock, &tsLock);
            break;
        default:
            sony_result = SONY_RESULT_ERROR_SW_STATE; // temporaly
            break;
        }

        if((sony_result == SONY_RESULT_OK) && tsLock){
            fgLock = TRUE;
        }else{
            fgLock = FALSE;
        }
    }

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return fgLock;
}

//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  void
 */
//-------------------------------------------------------------------------
static VOID  DTMB_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));  
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered

    sony_cn_dtv_demod_Sleep(pDemod);

    //END
    SetBreakMonitor(pPdCtx);
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get modulation information.
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  Outputs:
 *  @param   *_pSignal          Modulation info. structure to be filled.
 *  @retval  void
 */
//-------------------------------------------------------------------------
static void DTMB_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    //add driver implementation code here
    x_memset(_pSignal, 0, sizeof(SIGNAL));

    // Frequency -----------------------------------------------------
    _pSignal->Frequency  =  g_sony_demod_ctx.integ.frequency;

    // BandWidth -----------------------------------------------------
    _pSignal->e_bandwidth = BW_8_MHz;

    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
        sony_cn_dtv_dtmb_system_info_t systemInfo;
        sony_result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo(pDemod, &systemInfo);
        if(sony_result == SONY_RESULT_OK){
            switch(systemInfo.cnst){
            case SONY_CN_DTV_DTMB_CNST_4QAM:
                _pSignal->QAMSize = 4;
                break;
            case SONY_CN_DTV_DTMB_CNST_4QAM_NR:
				_pSignal->QAMSize = 5;
                break;
            case SONY_CN_DTV_DTMB_CNST_16QAM:
                _pSignal->QAMSize = 16;
                break;
            case SONY_CN_DTV_DTMB_CNST_32QAM:
                _pSignal->QAMSize = 32;
                break;
            case SONY_CN_DTV_DTMB_CNST_64QAM:
                _pSignal->QAMSize = 64;
                break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
                break;
            }
        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        }
    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
        sony_cn_dtv_dvbc_cnst_t constellation = SONY_CN_DTV_DVBC_CNST_16QAM;
        uint32_t symRate = 0;

        sony_result = sony_cn_dtv_demod_MonitorDvbc_QAM(pDemod, &constellation);
        if(sony_result == SONY_RESULT_OK){
            switch(constellation){
            case SONY_CN_DTV_DVBC_CNST_16QAM:
                _pSignal->QAMSize = 16; break;
            case SONY_CN_DTV_DVBC_CNST_32QAM:
                _pSignal->QAMSize = 32; break;
            case SONY_CN_DTV_DVBC_CNST_64QAM:
                _pSignal->QAMSize = 64; break;
            case SONY_CN_DTV_DVBC_CNST_128QAM:
                _pSignal->QAMSize = 128; break;
            case SONY_CN_DTV_DVBC_CNST_256QAM:
                _pSignal->QAMSize = 256; break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
            }
        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        }
        
        sony_result = sony_cn_dtv_demod_MonitorDvbc_SymbolRate(pDemod, &symRate);
        if(sony_result == SONY_RESULT_OK){
            _pSignal->SymbolRate = symRate * 1000;
        }else{
            _pSignal->SymbolRate = 0;
        }
    }

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in terms of percentage.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 DTMB_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;
    uint8_t ssi = 0;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    // Get SSI
    switch(pInteg->pDemod->system){
    case SONY_CN_DTV_SYSTEM_DTMB:
        sony_result = sony_integ_dtmb_monitor_SSI(pInteg, &ssi);
        break;
    case SONY_CN_DTV_SYSTEM_DVBC:
        sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi);
        break;
    default:
        sony_result = SONY_RESULT_ERROR_NOSUPPORT;
        break;
    }

    if(sony_result != SONY_RESULT_OK){
        ssi = 0;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    //example of return
    return ((UINT8)ssi);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in multiples of (0.1dBm).
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0 ~ -900           signal level
 */
//-------------------------------------------------------------------------
static INT16 DTMB_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT16 i2Ret = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;
    int32_t rfLevel = 0;

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    sony_result = sony_integ_monitor_RFLevel_MTK_DTMB(pInteg, &rfLevel);
    if(sony_result == SONY_RESULT_OK)
    {
        if(rfLevel >= 0)
        {
            i2Ret = (INT16)((rfLevel + 50) / 100);
        }
        else
        {
            i2Ret = (INT16)((rfLevel - 50) / 100); 
        }
    }
    else
    {
        i2Ret = 0;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);    
    return i2Ret;
}

static UINT32 DTMB_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret = 100000;//unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    uint32_t per = 0;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    //mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); //DO NOT LOCK MUTEX HERE!! (This function is called from GetAttribute)
    //To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-5
    switch(pDemod->system)
    {
    case SONY_CN_DTV_SYSTEM_DTMB:
        sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchPER(pDemod, &per);
        break;
    case SONY_CN_DTV_SYSTEM_DVBC:
        sony_result = sony_cn_dtv_demod_MonitorDvbc_PER(pDemod, &per);
        break;
    default:
        sony_result = SONY_RESULT_ERROR_SW_STATE;
        break;
    }

    if(sony_result != SONY_RESULT_OK){
        u4_ret = 100000; // Error Rate = 100%
    }else{
        // NOTE: sony_demod_dvbx_monitor_PER returns 1e6 unit value.
        u4_ret = (UINT32)((per + 5) / 10);
    }

    //END
   // mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u4_ret;//modify return value
}

static UINT16 DTMB_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret = 0;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_result_t sony_result = SONY_RESULT_OK;
    int32_t snr = 0;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));   
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Get SNR(signal-noise ratio) from demod driver

    // NOTE: monitor functions return dB * 1000.
    switch(pDemod->system){
    case SONY_CN_DTV_SYSTEM_DTMB:
        sony_result = sony_cn_dtv_demod_MonitorDtmb_MER(pDemod, &snr);
        break;
    case SONY_CN_DTV_SYSTEM_DVBC:
        sony_result = sony_cn_dtv_demod_MonitorDvbc_SNR(pDemod, &snr);
        break;
    default:
        snr = 0;
        break;
    }
    
    if(sony_result != SONY_RESULT_OK){
        snr = 0;
    }

    u2_ret = (UINT16)((snr + 500)/1000); // rounding

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u2_ret;//modify return value
}

static void DTMB_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static char *DTMB_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    static char * version = SONY_CXD2840_DRIVER_VERSION_STR;

    return version;
}

static INT32 DTMB_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return 0;
}
static void DTMB_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void DTMB_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void DTMB_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
static void DTMB_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void DTMB_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;

    //TODO you can add command for debug use here
   
    //END
    UCHAR   ucCmdId = 0;               
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t s                                 Show signal, TS, DTMB/DVBC system status\n"));
        mcDBG_MSG_L1(("\t d [t|x] [Bank]                    Demod register page dump\n")); 
        mcDBG_MSG_L1(("\t r [t|x] [Bank] [RegAddr] [Num]    Read demod register\n"));
        mcDBG_MSG_L1(("\t w [t|x] [Bank] [RegAddr] [Value]  Write demod register\n"));
        mcDBG_MSG_L1(("\t ad                                Tuner register page dump\n")); 
        mcDBG_MSG_L1(("\t ar [RegAddr] [Num]                Read tuner register\n"));
        mcDBG_MSG_L1(("\t aw [RegAddr] [Value]              Write tuner register\n"));
        mcDBG_MSG_L1(("\t p [d|a] [freqKHz]                 Change trap threshold\n"));
        mcDBG_MSG_L1(("\t ts                                To show demod TS lock status\n"));
        mcDBG_MSG_L1(("\t be                                To show pre RS or pre BCH BER\n"));
        mcDBG_MSG_L1(("\t ue                                To show UEC value\n"));
    }   
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);
        
    switch (ucCmdId)
    {
    case 's':
        mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

        /* Demod Lock Status */
        if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
            UINT8 dmdLock = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Demod Lock : %d\n", dmdLock));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det : %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
            UINT8 arLock = 0;
            UINT8 tsLock = 0;
            sony_result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("AR Lock    : %d\n", arLock));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else{
            mcDBG_MSG_L1(("Invalid System\n"));
        }

        /* IFAGC */
        {
            uint32_t ifAgc = 0;
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_IfAgcout(pDemod, &ifAgc); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_IfAgcout(pDemod, &ifAgc); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("IFAGC (%%) : %d\n", ifAgc * 100 / 4095));
                mcDBG_MSG_L1(("IFAGC value : %d\n", ifAgc));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorXXXX_IfAgcout (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Carrier Offset */
        {
            int32_t offset = 0;
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_CarrierOffset(pDemod, &offset); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_CarrierOffset(pDemod, &offset); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Carrier Offset (kHz) : %d\n", offset));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorXXXX_CarrierOffset (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Spectrum Sense */
        {
            uint8_t isInvert = 0;
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_SpectrumSense(pDemod, &isInvert); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_SpectrumSense(pDemod, &isInvert); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Spectrum Sense : %s\n", isInvert ? "Invert" : "Normal"));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorXXXX_SpectrumSense (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* BER */
        if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
            uint32_t ber = 0;
            sony_result = sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreLDPC BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_cn_dtv_demod_MonitorDtmb_PreBchBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreBCH BER (1e9) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PreBchBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
            uint32_t ber = 0;
            sony_result = sony_cn_dtv_demod_MonitorDvbc_PreRSBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreRS BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else{
            mcDBG_MSG_L1(("Invalid System\n"));
        }

        /* PER */
        {
            uint32_t per = 0;
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchPER(pDemod, &per); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_PER(pDemod, &per); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (1e6) : %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorXXXX_PER (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* FER */
        if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
            uint32_t fer = 0;
            
            sony_result = sony_cn_dtv_demod_MonitorDtmb_PostLdpcFER(pDemod, &fer);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Post LDPC FER (1e6) : %d\n", fer));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PostLdpcFER (%s)\n", sony_FormatResult(sony_result)));
            }

            sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchFER(pDemod, &fer);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Post BCH FER (1e6) : %d\n", fer));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PostBchFER (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Packet Error Number */
        {
            uint32_t packetErrorNum = 0;
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum(pDemod, &packetErrorNum); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_RSErrorNum(pDemod, &packetErrorNum); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (1sec) : %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in Packet Error Number monitor (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* MER/SNR */
        {
            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                {
                    int32_t mer = 0;
                    sony_result = sony_cn_dtv_demod_MonitorDtmb_MER(pDemod, &mer);
                    if(sony_result == SONY_RESULT_OK){
                        mcDBG_MSG_L1(("MER (dB) : %d.%03d\n", mer/1000U, mer%1000U));
                    }else{
                        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_MER (%s)\n", sony_FormatResult(sony_result)));
                    }
                }
                break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                {
                    int32_t snr = 0;
                    sony_result = sony_cn_dtv_demod_MonitorDvbc_SNR(pDemod, &snr);
                    if(sony_result == SONY_RESULT_OK){
                        mcDBG_MSG_L1(("SNR (dB) : %d.%03d\n", snr/1000U, snr%1000U));
                    }else{
                        mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
                    }
                }
                break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
        }

        /* SQI */
        {
            uint8_t sqi = 0;

            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_demod_dtmb_monitor_Quality(pDemod, &sqi); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_Quality(pDemod, &sqi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SQI (%%) : %d\n", sqi));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_xxxx_monitor_Quality (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* SSI */
        {
            uint8_t ssi = 0;

            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_integ_dtmb_monitor_SSI(pInteg, &ssi); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SSI (%%) : %d\n", ssi));
            }else{
                mcDBG_MSG_L1(("Error in sony_integ_xxxx_monitor_SSI (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* System Dependent Information */
        if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
            /* Carrier Mode */
            {
                sony_cn_dtv_dtmb_carrier_mode_t carrierMode = SONY_CN_DTV_DTMB_CARRIER_MODE_C1;
                sony_result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &carrierMode);
                if(sony_result == SONY_RESULT_OK){
                    switch(carrierMode){
                    case SONY_CN_DTV_DTMB_CARRIER_MODE_C1:
                        mcDBG_MSG_L1(("CarrierMode : C1\n"));
                        break;
                    case SONY_CN_DTV_DTMB_CARRIER_MODE_C3780:
                        mcDBG_MSG_L1(("CarrierMode : C3780\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("CarrierMode : Unknown (%d)\n", carrierMode));
                        break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_CarrierMode (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* Pseudo Noise */
            {
                sony_cn_dtv_dtmb_pseudo_noise_t pnInfo = SONY_CN_DTV_DTMB_PN_420_VARIOUS;
                sony_result = sony_cn_dtv_demod_MonitorDtmb_PNInfo(pDemod, &pnInfo);
                if(sony_result == SONY_RESULT_OK){
                    switch(pnInfo){
                    case SONY_CN_DTV_DTMB_PN_420_VARIOUS:
                        mcDBG_MSG_L1(("Pseudo Noise : PN420-various\n"));
                        break;
                    case SONY_CN_DTV_DTMB_PN_595_CONST:
                        mcDBG_MSG_L1(("Pseudo Noise : PN595-constant\n"));
                        break;
                    case SONY_CN_DTV_DTMB_PN_945_VARIOUS:
                        mcDBG_MSG_L1(("Pseudo Noise : PN945-various\n"));
                        break;
                    case SONY_CN_DTV_DTMB_PN_420_CONST:
                        mcDBG_MSG_L1(("Pseudo Noise : PN420-constant\n"));
                        break;
                    case SONY_CN_DTV_DTMB_PN_945_CONST:
                        mcDBG_MSG_L1(("Pseudo Noise : PN945-constant\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("Pseudo Noise : Unknown (%d)\n", pnInfo));
                        break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PNInfo (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* System Info */
            {
                sony_cn_dtv_dtmb_system_info_t systemInfo;
                sony_result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo(pDemod, &systemInfo);
                if(sony_result == SONY_RESULT_OK){
                    switch(systemInfo.cnst){
                    case SONY_CN_DTV_DTMB_CNST_4QAM:
                        mcDBG_MSG_L1(("Constellation : 4QAM\n"));
                        break;
                    case SONY_CN_DTV_DTMB_CNST_4QAM_NR:
                        mcDBG_MSG_L1(("Constellation : 4QAM-NR\n"));
                        break;
                    case SONY_CN_DTV_DTMB_CNST_16QAM:
                        mcDBG_MSG_L1(("Constellation : 16QAM\n"));
                        break;
                    case SONY_CN_DTV_DTMB_CNST_32QAM:
                        mcDBG_MSG_L1(("Constellation : 32QAM\n"));
                        break;
                    case SONY_CN_DTV_DTMB_CNST_64QAM:
                        mcDBG_MSG_L1(("Constellation : 64QAM\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("Constellation : Unknown (%d)\n", systemInfo.cnst));
                        break;
                    }
                    
                    switch(systemInfo.rate){
                    case SONY_CN_DTV_DTMB_LDPC_RATE_0_4:
                        mcDBG_MSG_L1(("LDPC Rate : 0.4\n"));
                        break;
                    case SONY_CN_DTV_DTMB_LDPC_RATE_0_6:
                        mcDBG_MSG_L1(("LDPC Rate : 0.6\n"));
                        break;
                    case SONY_CN_DTV_DTMB_LDPC_RATE_0_8:
                        mcDBG_MSG_L1(("LDPC Rate : 0.8\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("LDPC Rate : Unknown (%d)\n", systemInfo.rate));
                        break;
                    }
                    
                    switch(systemInfo.il){
                    case SONY_CN_DTV_DTMB_FEC_IL_M_240:
                        mcDBG_MSG_L1(("Interleaver Mode : M_240\n"));
                        break;
                    case SONY_CN_DTV_DTMB_FEC_IL_M_720:
                        mcDBG_MSG_L1(("Interleaver Mode : M_720\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("Interleaver Mode : Unknown (%d)\n", systemInfo.il));
                        break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_SystemInfo (%s)\n", sony_FormatResult(sony_result)));
                }
            }
        }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
            sony_cn_dtv_dvbc_cnst_t qam = SONY_CN_DTV_DVBC_CNST_16QAM;
            uint32_t symRate = 0;

            sony_result = sony_cn_dtv_demod_MonitorDvbc_QAM(pDemod, &qam);
            if(sony_result == SONY_RESULT_OK){
                switch(qam){
                case SONY_CN_DTV_DVBC_CNST_16QAM:
                    mcDBG_MSG_L1(("Constellation : 16 QAM\n")); break;
                case SONY_CN_DTV_DVBC_CNST_32QAM:
                    mcDBG_MSG_L1(("Constellation : 32 QAM\n")); break;
                case SONY_CN_DTV_DVBC_CNST_64QAM:
                    mcDBG_MSG_L1(("Constellation : 64 QAM\n")); break;
                case SONY_CN_DTV_DVBC_CNST_128QAM:
                    mcDBG_MSG_L1(("Constellation : 128 QAM\n")); break;
                case SONY_CN_DTV_DVBC_CNST_256QAM:
                    mcDBG_MSG_L1(("Constellation : 256 QAM\n")); break;
                default:
                    mcDBG_MSG_L1(("Constellation : Unknown (%d)\n", qam)); break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_QAM (%s)\n", sony_FormatResult(sony_result)));
            }
            
            sony_result = sony_cn_dtv_demod_MonitorDvbc_SymbolRate(pDemod, &symRate);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SymbolRate : %d kSymbol/sec\n", symRate));
            }else{
                mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_SymbolRate (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
        break;
    case 't':
        {
            switch(aszArgv[0][1])
            {
                case 's':
                default:
                {
                    /* Demod Lock Status */
                    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
                        UINT8 dmdLock = 0;
                        UINT8 tsLock = 0;
                        UINT8 unlock = 0;
                        sony_result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
                        if(sony_result == SONY_RESULT_OK){
                            mcDBG_MSG_L4(("DTMB Demod Lock : %d\n", dmdLock));
                            mcDBG_MSG_L1(("DTMB TS Lock    : %d\n", tsLock));
                            mcDBG_MSG_L4(("DTMB Unlock Det : %d\n", unlock));
                        }else{
                            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                        }
                    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
                        UINT8 arLock = 0;
                        UINT8 tsLock = 0;
                        sony_result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
                        if(sony_result == SONY_RESULT_OK){
                            mcDBG_MSG_L4(("DVBC AR Lock    : %d\n", arLock));
                            mcDBG_MSG_L1(("DVBC TS Lock    : %d\n", tsLock));
                        }else{
                            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                        }
                    }else{
                        mcDBG_MSG_L1(("Invalid System\n"));
                    }
                }
                break;
            }
        }
        break;
    /* Tuner Debug Command ----------------------------------------------------- */
    case 'a':
        {
            // Tuner Dump/Read/Write
            // Using sony_demod_ctx_t::tuner for I2C dump/write/read
            // NOTE: sony_demod_ctx_t::tuner should be initialized correctly in TunerOpen.
            sony_cn_dtv_tuner_t *pTuner = &g_sony_demod_ctx.tuner;

            if((pTuner->i2cAddress == 0x00) || (pTuner->pI2c == NULL)){
                mcDBG_MSG_L1(("Tuner struct instance is not correctly configured!\n"));
                return;
            }

            switch(aszArgv[0][1]){
            case 'd':
            default:
                {
                    UINT8 data[256];
                    
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); // Enter Critical Section
                    sony_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x00, data, 256);
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock); // Leave Critical Section
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Register Dump Result. (Tuner)\n"));
                    mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
                    {
                        int i = 0;
                        int j = 0;
                        char dumpline[53];
                        char* ptr = dumpline;
                        const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                        dumpline[52] = '\0';
                        for(i=0; i<16; i++){
                            ptr = dumpline;
                            for(j=0; j<16; j++){
                                if(j == 0){
                                    *ptr++ = hexchar[(i*16) >> 4];
                                    *ptr++ = hexchar[(i*16) & 0x0F];
                                    *ptr++ = ' ';
                                    *ptr++ = '|';
                                }
                                *ptr++ = ' ';
                                *ptr++ = hexchar[data[(i*16)+j] >> 4];
                                *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                            }
                            mcDBG_MSG_L1(("%s\n", dumpline));
                        }
                    }
                    mcDBG_MSG_L1(("\n"));
                }
                break;
            case 'r':
                {
                    UINT8 data[256];
                    UINT8 subAddress = 0;
                    UINT32 length = 0;
                    UINT32 i = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No read register and length.\n"));
                        return;
                    }
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    length = (UINT32)sony_strtoul(aszArgv[2]);
                    if(length > 256){
                        length = 256;
                    }
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
                    sony_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, subAddress, data, length);
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    for(i=0; i<length; i++){
                        mcDBG_MSG_L1(("Tuner Register 0x%02X = 0x%02X\n", i + subAddress, data[i]));
                    }
                }
                break;
            case 'w':
                {
                    UINT8 subAddress = 0;
                    UINT8 value = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No write register and data.\n"));
                        return;
                    }
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    value = (UINT8)sony_strtoul(aszArgv[2]);
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
                    sony_result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, subAddress, value);
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Wrote Tuner Register 0x%02X = 0x%02X\n", subAddress, value));
                }
                break;
            }
            break;
        }
    case 'b':
        {
            switch(aszArgv[0][1])
            {
                case 'e':
                default:
                {
                    /* BER */
                    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
                        uint32_t ber = 0;
                        sony_result = sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER(pDemod, &ber);
                        if(sony_result == SONY_RESULT_OK){
                            mcDBG_MSG_L4(("DTMB PreLDPC BER (1e7) : %d\n", ber));
                        }else{
                            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER (%s)\n", sony_FormatResult(sony_result)));
                        }
                        sony_result = sony_cn_dtv_demod_MonitorDtmb_PreBchBER(pDemod, &ber);
                        if(sony_result == SONY_RESULT_OK){
                            mcDBG_MSG_L1(("DTMB PreBCH BER (1e9) : %d\n", ber));
                        }else{
                            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDtmb_PreBchBER (%s)\n", sony_FormatResult(sony_result)));
                        }
                    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
                        uint32_t ber = 0;
                        sony_result = sony_cn_dtv_demod_MonitorDvbc_PreRSBER(pDemod, &ber);
                        if(sony_result == SONY_RESULT_OK){
                            mcDBG_MSG_L1(("DVBC PreRS BER (1e7) : %d\n", ber));
                        }else{
                            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_MonitorDvbc_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
                        }
                    }else{
                        mcDBG_MSG_L1(("Invalid System\n"));
                    }
                }
                break;
            }
        }
        break;
    case 'd':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            
            if(i4Argc < 3){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and bank are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddress + 4;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddress;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); // Enter Critical Section
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                return;
            }
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, 0x00, data, 256);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock); // Leave Critical Section
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Register Dump Result. (Slave:0x%02X, Bank:0x%02X)\n", i2cAddress, bank));
            mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
            {
                int i = 0;
                int j = 0;
                char dumpline[53];
                char* ptr = dumpline;
                const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                dumpline[52] = '\0';
                for(i=0; i<16; i++){
                    ptr = dumpline;
                    for(j=0; j<16; j++){
                        if(j == 0){
                            *ptr++ = hexchar[(i*16) >> 4];
                            *ptr++ = hexchar[(i*16) & 0x0F];
                            *ptr++ = ' ';
                            *ptr++ = '|';
                        }
                        *ptr++ = ' ';
                        *ptr++ = hexchar[data[(i*16)+j] >> 4];
                        *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                    }
                    mcDBG_MSG_L1(("%s\n", dumpline));
                }
            }
            mcDBG_MSG_L1(("\n"));
        }
        break;
    case 'r':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT32 length = 0;
            UINT32 i = 0;
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, length are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddress + 4;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddress;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            length = (UINT32)sony_strtoul(aszArgv[4]);
            if(length > 256){
                length = 256;
            }
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, subAddress, data, length);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            for(i=0; i<length; i++){
                mcDBG_MSG_L1(("Read Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, i + subAddress, data[i]));
            }
        }
        break;
    case 'w':
        {
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT8 value = 0;
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, value are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddress + 4;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddress;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            value = (UINT8)sony_strtoul(aszArgv[4]);
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, subAddress, value);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Wrote Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, subAddress, value));
        }
        break;
    case 'p':
        {
            UINT32 frequencyKHz = 0;
            UINT8 isAnalog = 0;

            switch(aszArgv[1][0]){
            case 'd':
            case 'D':
                isAnalog = 0;
                break;
            case 'a':
            case 'A':
                isAnalog = 1;
                break;
            default:
                mcDBG_MSG_L1(("Error: Digital/Analog ('d' or 'a') is necessary.\n"));
                return;
            }
            frequencyKHz = (UINT32)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            if(ITuner_OP(ITunerGetCtx(), itSetTrapThresold, isAnalog, &frequencyKHz) == ITUNER_OK){
                if(isAnalog){
                    mcDBG_MSG_L1(("Analog trap threshold : %d kHz\n", frequencyKHz));
                }else{
                    mcDBG_MSG_L1(("Digital trap threshold : %d kHz\n", frequencyKHz));
                }
            }else{
                mcDBG_MSG_L1(("ITuner_OP failed.\n"));
            }
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
        }
        break;
    case 'u':
        {
            UINT32 Uec = g_sony_demod_ctx.u4Uec;
            switch(aszArgv[0][1])
            {
                case 'e':
                default:
                {
                    mcDBG_MSG_L1(("DTMB : UEC = %lu\n", Uec));
                }
                break;
                case 'c':
                {
                    g_sony_demod_ctx.u4Uec = 0;
                    mcDBG_MSG_L1(("DTMB : Clean UEC\n"));
                }
                break;                    
            }
        }
        break;
    default:
        break;
    }    
}

static void DTMB_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_DTMB
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_DTMB_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 DTMB_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    INT32 i4Temp = 0;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;
	sony_result_t sony_result = SONY_RESULT_OK;
	//uint32_t packetErrorNum = 0;
	uint32_t ifAgc=0;
	int32_t snr = 0;
	TUNER_DIG_ATTRIBUTE_T *pAttri;
	SPECIFIC_MEMBER_EU_CTX* pEUCtx = &(ITunerGetCtx()->specific_member.eu_ctx);
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    mcDBG_MSG_L4((" %s ,e_get_type=%d\n",__FUNCTION__,e_get_type));

    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER: 
        //TODO Get BER from demod driver.Note the unit is 1e-5
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
        {
            UINT32 ber = sony_GetBER();
            i4Temp = (INT32)ber;
        }
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        //END
        break;
    case TUNER_GET_TYPE_PRE_VBER:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_PRE_BER\n"));
        {
            UINT32 u4Preber = sony_GetPreBER();
            i4Temp = (INT32)u4Preber;
        }
		mcDBG_MSG_L2(("CXD2840 PRE_VBER value=%d\n",i4Temp));
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_TS_FMT:
        //TODO Get TS format such as TS output via serial or parallel,data active edge,etc.please refer the definition of TS_FMT_T
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_TS_FMT\r\n"));
        {
            TS_FMT_T tsFmt;
            *pzAttributeLen = sizeof(TS_FMT_T);
            sony_GetTsFmt(pDemod, &tsFmt);
            x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);
        }
        //END
        break;
	case TUNER_GET_TYPE_DIG_ATTRIBUTE:
	    mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
	    pAttri = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
	    //Tuner frequency boundary
	    pAttri->ui4_lower_bound_freq = pEUCtx->m_s4FreqBoundLower;
	    pAttri->ui4_upper_bound_freq = pEUCtx->m_s4FreqBoundUpper;
	    // freq offset tolerence range in Hz (1M mapping to 100K)
	    pAttri->b_auto_qam_capability = TRUE;
	    pAttri->b_auto_sr_capability = TRUE;
	    pAttri->ui4_fine_tune_delta_freq = 400000;
		break;
	case TUNER_GET_TYPE_UEC:
        /*
		 *pzAttributeLen = sizeof(UINT16);		 
		 switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum(pDemod, &packetErrorNum); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_RSErrorNum(pDemod, &packetErrorNum); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
		  if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("PacketErrorNumber (1sec) : %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L2(("Error in Packet Error Number monitor (%s)\n", sony_FormatResult(sony_result)));
            }
		 i4Temp=(UINT16)packetErrorNum;
		 mcDBG_MSG_L2(("TUNER_GET_TYPE_UEC =%d\n",packetErrorNum));
		  x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
		  */
        mcDBG_MSG_L2(("e_get_type =TUNER_GET_TYPE_UEC\r\n"));
        *pzAttributeLen = sizeof(UINT32);
        x_memcpy(pzAttribute, &g_sony_demod_ctx.u4Uec, *pzAttributeLen);
		break;	
	case TUNER_GET_TYPE_AGC:
		 *pzAttributeLen = sizeof(INT32);
		 switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_cn_dtv_demod_MonitorDtmb_IfAgcout(pDemod, &ifAgc); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_IfAgcout(pDemod, &ifAgc); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
		 //ifAgc=ifAgc * 100 / 4095;
         ifAgc=255-(ifAgc>>4);
		 if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("IFAGC : %d\n",ifAgc));
            }else{
                mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorXXXX_IfAgcout (%s)\n", sony_FormatResult(sony_result)));
            }
		 i4Temp=(INT32)ifAgc;
		 x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
		break;
    case TUNER_GET_TYPE_KHZ_CFO:
        {
            int32_t offset = 0;
            switch(pDemod->system){
                case SONY_CN_DTV_SYSTEM_DTMB:
                    sony_result = sony_cn_dtv_demod_MonitorDtmb_CarrierOffset(pDemod, &offset); break;
                case SONY_CN_DTV_SYSTEM_DVBC:
                    sony_result = sony_cn_dtv_demod_MonitorDvbc_CarrierOffset(pDemod, &offset); break;
                default:
                    sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("Carrier Offset (kHz) : %d\n", offset));
            }else{
                mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorXXXX_CarrierOffset (%s)\n", sony_FormatResult(sony_result)));
            }
            *pzAttributeLen = sizeof(INT32);
            x_memcpy(pzAttribute, &offset, *pzAttributeLen);
        }
        break;
	case TUNER_GET_TYPE_DBM_SNR:
		*pzAttributeLen = sizeof(INT32);
		switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:                   
                sony_result = sony_cn_dtv_demod_MonitorDtmb_MER(pDemod, &snr);
                break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_cn_dtv_demod_MonitorDvbc_SNR(pDemod, &snr);
				 break;			               
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; 
                break;
        }
		if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("SNR (dB) : %d.%03d\n", snr/1000U, snr%1000U));
        }else{
            mcDBG_MSG_L2(("Error in sony_demod_dvbx_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
        }
		snr=snr/100;
		x_memcpy(pzAttribute, &snr, *pzAttributeLen);
		break;	
    case TUNER_GET_TYPE_TS_LOCK_STATUS:
        {
            UINT8 tsLock = 0;
            UINT8 dmdLock = 0;
            UINT8 unlock = 0;
            UINT8 arLock = 0;
            if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
                sony_result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L2(("Demod Lock : %d\n", dmdLock));
                    mcDBG_MSG_L2(("TS Lock    : %d\n", tsLock));
                    mcDBG_MSG_L2(("Unlock Det : %d\n", unlock));
                }else{
                    mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorDtmb_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                }
            }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
                sony_result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L2(("AR Lock    : %d\n", arLock));
                    mcDBG_MSG_L2(("TS Lock    : %d\n", tsLock));
                }else{
                    mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorDvbc_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                }
            }else{
                mcDBG_MSG_L2(("Invalid System\n"));
            }
            *pzAttributeLen = sizeof(UINT8);
            x_memcpy(pzAttribute, &tsLock, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_DEMOD_LOCK_STATUS:
        {
            UINT8 tsLock = 0;
            UINT8 dmdLock = 0;
            UINT8 unlock = 0;
            UINT8 arLock = 0;
            if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
                sony_result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L2(("Demod Lock : %d\n", dmdLock));
                }else{
                    mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorDtmb_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                }
            }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
                sony_result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L2(("AR Lock    : %d\n", arLock));
                    if(arLock){
                        dmdLock = 1;
                    }else{
                        dmdLock = 0;
                    }
                }else{
                    mcDBG_MSG_L2(("Error in sony_cn_dtv_demod_MonitorDvbc_SyncStat (%s)\n", sony_FormatResult(sony_result)));
                }
            }else{
                mcDBG_MSG_L2(("Invalid System\n"));
            }
            *pzAttributeLen = sizeof(UINT8);
            x_memcpy(pzAttribute, &dmdLock, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_TUNER_STATUS:
        {
            BOOL pllLock = FALSE;
            UINT8 temp = 0;
            ITuner_OP(ITunerGetCtx(), itGetStatus, 0, &pllLock);
            mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_TUNER_STATUS\n"));
            mcDBG_MSG_L2(("pllLock = %d\n", pllLock));
            temp = (UINT8)pllLock;
            *pzAttributeLen = sizeof(UINT8);
            x_memcpy(pzAttribute, &temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SQI:
        *pzAttributeLen = sizeof(INT32);
        {
            UINT8 sqi = 0;
            sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_demod_dtmb_monitor_Quality(pDemod, &sqi); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_Quality(pDemod, &sqi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result != SONY_RESULT_OK){
                sqi = 0;
            }
            i4Temp = (INT32)sqi;
            mcDBG_MSG_L2(("SQI = %d\r\n", sqi));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SSI:
        *pzAttributeLen = sizeof(INT32);
        {
            UINT8 ssi = 0;
            sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_CN_DTV_SYSTEM_DTMB:
                sony_result = sony_integ_dtmb_monitor_SSI(pInteg, &ssi); break;
            case SONY_CN_DTV_SYSTEM_DVBC:
                sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result != SONY_RESULT_OK){
                ssi = 0;
            }
            i4Temp = (INT32)ssi;
            mcDBG_MSG_L2(("SSI = %d\r\n", ssi));     
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);    
        }
        break;
    case TUNER_GET_TYPE_BER_UPPER:
		{
		 UINT16 berUpper;			 
		 UINT32 ber=sony_GetBER();
		 berUpper = (UINT16)((ber>>16)&0xffff);
         *pzAttributeLen = sizeof(UINT16);
         x_memcpy(pzAttribute, &berUpper, *pzAttributeLen);		
		}
		break;
    case TUNER_GET_TYPE_BER_LOWER:
		{
		 UINT16 berLower;		 
		 UINT32 ber=sony_GetBER();
		 berLower = (UINT16)(ber&0xffff);
         *pzAttributeLen = sizeof(UINT16);
         x_memcpy(pzAttribute, &berLower, *pzAttributeLen);		
		}
		break;
    case TUNER_GET_TYPE_PER_UPPER:
		{
		 UINT16 perUpper;
		 UINT32 per=DTMB_TunerGetSignalPER(ptTDSpecificCtx, 0);
		 perUpper = (UINT16)((per>>16)&0xffff);
         *pzAttributeLen = sizeof(UINT16);
         x_memcpy(pzAttribute, &perUpper, *pzAttributeLen);		
		}
		break;
	case TUNER_GET_TYPE_PER_LOWER:
		{	
		 UINT16 perLower;	 
		 UINT32 per=DTMB_TunerGetSignalPER(ptTDSpecificCtx, 0);
		 perLower = (UINT16)(per&0xffff);
         *pzAttributeLen = sizeof(UINT16);
         x_memcpy(pzAttribute, &perLower, *pzAttributeLen);		
		}
		break;
    case TUNER_GET_TYPE_DIG_DEMOD_STATUS:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_DEMOD_STATUS\r\n"));
        {
            TUNER_TER_DIG_TUNE_STATUS_T  tuneStatus;
            *pzAttributeLen = sizeof(TUNER_TER_DIG_TUNE_STATUS_T);
            DTMB_TunerGetModuInfor(&tuneStatus);
            x_memcpy(pzAttribute, &tuneStatus, *pzAttributeLen);
        }
        break;    
    default:
        //mcDBG_MSG_ERR(("unknown: get Attribute %d\r \n",e_get_type));
        break;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return DRVAPI_TUNER_OK;
}
//-------------------------------------------------------------------------
/**
 *  This API set attribute to tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_set_type         tuner set type, check x_tuner.h set operation TUNER_SET_TYPE_DTMBX
 *  @param   *pvAttribute       set value.
 *  @param   zAttributeLen      length of *pvAttribute.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT32 DTMB_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return DRVAPI_TUNER_OK;
}
#ifdef fcADD_DIAG_INFO
extern void ASCOT3HELENE_SetTerrLNA(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, UINT8 setVal);
static INT32 DTMB_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    INT32   retSts = RMR_OK;
    
    if(pv_set_info == NULL)
    {
        mcDBG_MSG_L1(("[DTMB_TunerSetDiagCmd] pv_set_info is NULL & set type can not support...\n"));
        return DRVAPI_TUNER_OK;
    }
    
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    switch(e_tuner_set_type)
    {
        case DRV_CUSTOM_TUNER_SET_TYPE_SET_LNA:
        {   
            UINT8 *pSetVal = (UINT8*)pv_set_info;             
            ITUNER_CTX_T *pTCtx;
            pTCtx = ITunerGetCtx();
            ASCOT3HELENE_SetTerrLNA(0,pTCtx,*pSetVal);
        }
        break;
        default:
            mcDBG_MSG_L1(("unknown: set type = %d\r\n",e_tuner_set_type));
        break;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return retSts;
}
static INT32 DTMB_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return 0;
}
#endif
//-------------------------------------------------------------------------
/**
*  This API  control  the i2c bypass to tuner. When zap from digital to ananlog,we need this api to bypass i2c to tuner. 
* Inputs:
* @param   ptTDSpecificCtx  Pointer to tuner driver context, there is no use for third party tuner.  
* @param    bSwitchOn        TRUE(bypass i2c to tuner on).
                             FALSE(bypass i2c to tuner off) 
* @Retval                    void
*/
//-------------------------------------------------------------------------
static void DTMB_TunerBypassI2C (PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO  Bypass i2c to tuner connected(bSwitchOn is TRUE) or disconnect(bSwitchOn is FALSE)
    
    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
void PD_DTMB_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose   = DTMB_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq     = DTMB_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = DTMB_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = DTMB_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DTMB_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DTMB_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DTMB_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DTMB_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = DTMB_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer  = DTMB_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = DTMB_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DTMB_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DTMB_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = DTMB_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = DTMB_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = DTMB_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = DTMB_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc    = DTMB_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DTMB_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DTMB_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= DTMB_TunerBypassI2C;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = DTMB_TunerSetDiagCmd;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DTMB_TunerGetDiagInfo;
#endif
}
//-------------------------------------------------------------------------
/**
 *  This API is driver entry point. It performs
 *  1. allocate and initialize resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDCtx            TunerHAL Context use for LTDIS API mapping
 *  @param   ptTunerCfgInfo     Tuner configuration information.
 *  @param   pptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
INT32 DTMB_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //create pdctx used in this file, you also can add any declaration you want in pdctx
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_cn_dtv_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_cn_dtv_tuner_t *pTuner = &g_sony_demod_ctx.tuner;
    sony_cn_dtv_integ_t *pInteg = &g_sony_demod_ctx.integ;
    sony_i2c_t *pI2c = &g_sony_demod_ctx.i2c;
    sony_i2c_t *pI2cTuner = &g_sony_demod_ctx.i2cTuner;
    sony_cn_dtv_demod_xtal_freq_t demod_xtal;
    BOOL is_parallel = FALSE;
  
    ITUNER_CTX_T *psTunerCtx ;
    psTunerCtx=ITunerGetCtx();
    psTunerCtx->u1DemodI2cAddress=0xD8;
  
    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

    pPdCtx = (DTMB_PD_CTX_T *) x_mem_calloc(1, sizeof(DTMB_PD_CTX_T));
    if (pPdCtx == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtx;
    pDtmbTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    mcDBG_MSG_L1(("pDtmbTunerDeviceCtx->tunerType=0x%02X\n",pDtmbTunerDeviceCtx->tunerType));

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtx->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtx->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    //register LTDIS APIs to HAL
    PD_DTMB_Register_LTDIS_Fct(ptTDCtx);
	vResetDemod(100); // 100 ms
    //Regisiter tuner driver
    ITunerRegistration(pDtmbTunerDeviceCtx->tunerType);
    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR

    // Sony demod configuration ----------------------------------------------

#ifdef CC_PROJECT_FOR_SONY
    {
        // For FY15 Sony Project Configuration
        VDRBRDCST_GetSonyProductTypeInfo(&g_sonyproduct_typeinfo);

        mcDBG_MSG_L2(("Board Ver = %d, Model Variation ID = %d\n",
            g_sonyproduct_typeinfo.eBoardVer, g_sonyproduct_typeinfo.modelVariationID));

        // Invalid Model Variation ID check
        switch((int) g_sonyproduct_typeinfo.modelVariationID){
        case 0x00: // AEP/PA-T2S2-1Tuner
        case 0x01: // AEP/PA-T2S2-2Tuner
        case 0x02: // PA/PH-T2-1Tuner
        case 0x06: // LA-1Tuner
        case 0x07: // LA-2Tuner
        case 0x03: // TW-1Tuner
        case 0x05: // UC/MX-1Tuner
            // Non CXD2840 Model
            mcDBG_MSG_L2(("Warning : This model does not include CXD2840. (%d)\n", g_sonyproduct_typeinfo.modelVariationID));
            break;

        case 0x04: // CH/HK-1Tuner
            break;

        default:
            // Error!?
            mcDBG_MSG_L2(("Fatal Error!! Invalid Model Variation ID!! (%d)\n", g_sonyproduct_typeinfo.modelVariationID));
            break;
        }

        // Demod I2C Connection
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // For Salvia I2C_D is used
            sony_i2c_MTK_CreateI2c_DTMB(pI2c, SONY_I2C_MTK_BUSID_D);
        }else{
            // For SCT, I2C_D is used
            // For CVT or later, I2C_A is used for main tuner, and I2C_D is used for sub tuner.
            if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
                sony_i2c_MTK_CreateI2c_DTMB(pI2c, SONY_I2C_MTK_BUSID_D);
            }else{
                sony_i2c_MTK_CreateI2c_DTMB(pI2c, SONY_I2C_MTK_BUSID_A);
            }
        }
        // Tuner I2C Connection (For tuner register dump)
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // For Salvia, I2C_D is uesed
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sony_i2c_MTK_CreateI2cGw_DTMB(pI2cTuner, 0xD8, 0x09, SONY_I2C_MTK_BUSID_D);
            #else
            sony_i2c_MTK_CreateI2c_DTMB(pI2cTuner, SONY_I2C_MTK_BUSID_D);
            #endif
        }else{
            // Single, Dual main tuner, use I2C_A and direct access.
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sony_i2c_MTK_CreateI2cGw_DTMB(pI2cTuner, 0xD8, 0x09, SONY_I2C_MTK_BUSID_A);
            #else
            sony_i2c_MTK_CreateI2c_DTMB(pI2cTuner, SONY_I2C_MTK_BUSID_A);
            #endif
        }
        // Tuner struct creation used in CXD2854 driver
        sony_result = sony_tuner_MTK_Create(pTuner);
        if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

        // sony_tuner_terr_cable_t member is set for tuner register dump.
        pTuner->pI2c = pI2cTuner;
        pTuner->i2cAddress = 0xC0;

        demod_xtal = SONY_CN_DTV_DEMOD_41000KHZ;

        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
	        is_parallel = 1; // Parallel Output Only
        }else{
		    is_parallel = 0; // Serial Output Only
        }
    }

#else  // CC_PROJECT_FOR_SONY

    // NOTE: Please set correct demod xtal frequency here!!
    // SONY_CN_DTV_DEMOD_20500KHZ : 20.5MHz
    // SONY_CN_DTV_DEMOD_41000KHZ : 41MHz

    demod_xtal = SONY_CN_DTV_DEMOD_41000KHZ;

    // NOTE: Please set TS serial or parallel here!!
	is_parallel = FALSE;

    // Sony specific initialization -------------------------------------------

    // I2C struct creation used in CXD2840 driver
    sony_i2c_MTK_CreateI2c_DTMB(pI2c, SONY_I2C_MTK_BUSID_A);

    // Tuner I2C Connection (For tuner register dump)
    sony_i2c_MTK_CreateI2cGw_DTMB(pI2cTuner, 0xD8, 0x09, SONY_I2C_MTK_BUSID_A);

    // Tuner struct creation used in CXD2840 driver
    sony_result = sony_tuner_MTK_Create(pTuner);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // sony_tuner_terr_cable_t member is set for tuner register dump.
    pTuner->pI2c = pI2cTuner;
    pTuner->i2cAddress = 0xC0;

#endif // CC_PROJECT_FOR_SONY

    {
        sony_cn_dtv_demod_iffreq_config_t iffreq_config;

        // IF frequency setting for Sony silicon tuner
        iffreq_config.config_DTMB_U3B = CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DTMB_U(5.1);
        iffreq_config.config_DTMB_L32B = CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DTMB_L(5.1);
        iffreq_config.config_DVBC = CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DVBC(4.9);

        sony_cn_dtv_demod_Create(pDemod, pI2c, 0xD8, demod_xtal, &iffreq_config);
    }

    // integ struct creation
    x_memset(pInteg, 0, sizeof(sony_cn_dtv_integ_t));
    
    pInteg->pDemod = pDemod;
    pInteg->pTuner = pTuner;
    pInteg->frequency = 0;
    pInteg->system = SONY_CN_DTV_SYSTEM_UNKNOWN;
    pInteg->isCancel = 0;
    pInteg->lockInfo = SONY_CN_DTV_INTEG_LOCK_ID_UNKNOWN;
    pInteg->presetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_NONE;
    pInteg->user = 0;

    // Demod initialization
    sony_result = sony_cn_dtv_demod_Initialize(pDemod);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_Initialize (%s).\n", sony_FormatResult(sony_result)));
       // return DRVAPI_TUNER_ERROR;
    }

    // Use ASCOT3
    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_IS_ASCOT, SONY_CN_DTV_DEMOD_TUNER_ASCOT3);
    if(sony_result != SONY_RESULT_OK){// return DRVAPI_TUNER_ERROR; 
	}

    // Enable Shared IF
    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_SHARED_IF, 1);
    if(sony_result != SONY_RESULT_OK){// return DRVAPI_TUNER_ERROR; 
	}

    if(is_parallel){
        /* TS Parallel Output, LSB is output to TSDATA0(default) */
        sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_TS_OUTPUT_PARALLEL_SERIAL,
            SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_LSB);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
          //  return DRVAPI_TUNER_ERROR;
        }
    }else{
		/* TS Serial Output from TSDATA7 pin */
		//sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_TS_OUTPUT_PARALLEL_SERIAL,
        //  SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_CONTINUOUS);
        /* TS Serial Output from TSDATA0 pin */
        sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_TS_OUTPUT_PARALLEL_SERIAL,
            SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_CONTINUOUS);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
          //  return DRVAPI_TUNER_ERROR;
        }
    }

#ifdef CC_PROJECT_FOR_SONY
    /* PWM from GPIO1 (RFAGC) pin pullup disable */
    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_GPIO_1_DISABLE_PULLUP, 1);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
      //  return DRVAPI_TUNER_ERROR;
    }

    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_PWM_ENABLE, 0);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
      //  return DRVAPI_TUNER_ERROR;
    }

    // NOTE: PWM (RFAGC) output should be enable when it's necessary.
#else // CC_PROJECT_FOR_SONY
    /* TS ERR (from GPIO2) enable */
    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_GPIO_2_DISABLE_PULLUP, 1);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
      //  return DRVAPI_TUNER_ERROR;
    }

    sony_result = sony_cn_dtv_demod_SetConfig(pDemod, SONY_CN_DTV_DEMOD_CONFIG_TSERR_ENABLE, 1);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_cn_dtv_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
      //  return DRVAPI_TUNER_ERROR;
    }
#endif // CC_PROJECT_FOR_SONY

    // Tuner initialization (After demod initialization)
    ITuner_Init(ITunerGetCtx());
    if(ITuner_OP(ITunerGetCtx(), itSetTunerInit, (UINT8)TRUE, NULL) != ITUNER_OK){
        mcDBG_MSG_L1(("Error in ITuner_OP(itSetTunerInit).\n"));
    }

    //END
    if(pDtmbTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDtmbTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtx->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcDBG_MSG_ERR(("EN_DTMB_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        pPdCtx->fgBreakMonitor = TRUE;//init
        pPdCtx->fgPIMoniStatus = FALSE; //init
        PD_StartMonitorEngine(pPdCtx);  //start monitor engine
    }
    return (DRVAPI_TUNER_OK);
}

// For Debugging
void sony_trace_log_enter_DTMB(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONY_ENTER : %s (%d)\n", funcname, linenum));
}
void sony_trace_log_return_DTMB(sony_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONY_RESULT_OK){
        mcDBG_MSG_L4(("SONY_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONY_RETURN : (line %d))\n", linenum));
    }
}

void sony_trace_i2c_log_enter_DTMB(const char* funcname, const char* filename, unsigned int linenum)
{
    sony_trace_log_enter_DTMB(funcname, filename, linenum);
}
void sony_trace_i2c_log_return_DTMB(sony_result_t result, const char* filename, unsigned int linenum)
{
    sony_trace_log_return_DTMB(result, filename, linenum);
}
