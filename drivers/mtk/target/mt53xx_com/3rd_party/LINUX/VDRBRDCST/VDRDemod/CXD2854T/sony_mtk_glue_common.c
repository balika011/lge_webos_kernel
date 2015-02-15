/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#include "sony_mtk_glue_common.h"

#ifndef SONY_DUMMY_DEBUG
// Please add necessary header files.
#include "x_os.h"
#include "tunerDebug.h"
#include "tuner_interface_if.h"
#include "u_tuner.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_isdbt_glue.h"
#include "i2c_api.h"
#include "drvcust_if.h"//Get Tuner/Demod I2C bus id
#include "x_gpio.h"
#include "u_drv_cust.h"
#include "pi_demod_common_def.h"

#endif

#include "sony_i2c_MTK.h"

#define SONY_DEMOD_IIC_ADDRESS_0  0xD8
#define SONY_DEMOD_IIC_ADDRESS_1  0xDA

// Sony CXD2854 driver object instance
sony_demod_ctx_t g_sony_demod_ctx[SONY_DEMOD_CTX_NUM];

#ifdef CC_PROJECT_FOR_SONY
// Sony Product Information
SONYPRODUCT_TYPEINFO g_sonyproduct_typeinfo;
#endif

static sony_result_t sony_tuner_MTK_Create (sony_tuner_terr_cable_t * pTuner);

INT32 sony_mtk_glue_common_Initialize (UINT8 tunerID)
{
    // Initialize Sony CXD2854 driver/demod and tuner
    // Called from XXXX_TunerOpen APIs.
    sony_demod_ctx_t *pSonyDemodCtx = &g_sony_demod_ctx[tunerID];
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &pSonyDemodCtx->integ;
    sony_demod_t *pDemod = &pSonyDemodCtx->demod;
    sony_tuner_terr_cable_t *pTuner = &pSonyDemodCtx->tuner;
    sony_i2c_t *pI2c = &pSonyDemodCtx->i2c;
    sony_i2c_t *pI2cTuner = &pSonyDemodCtx->i2cTuner;
    sony_demod_xtal_t demod_xtal = SONY_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pSonyDemodCtx->autoPLP = 1;
    pSonyDemodCtx->frequency = 0;
    pSonyDemodCtx->fgEws = FALSE; // clear EWS flag.
    pSonyDemodCtx->breakFct = NULL;
    pSonyDemodCtx->breakFctArg = NULL;

    // Sony demod configuration ----------------------------------------------

#ifdef CC_PROJECT_FOR_SONY
    {
        // For FY15 Sony Project Configuration
        uint8_t i2cAddressDemod = 0xD8;

        VDRBRDCST_GetSonyProductTypeInfo(&g_sonyproduct_typeinfo);

        mcDBG_MSG_L2(("CXD2854[%d] : Board Ver = %d, Model Variation ID = %d\n", tunerID,
            g_sonyproduct_typeinfo.eBoardVer, g_sonyproduct_typeinfo.modelVariationID));

        // Invalid Model Variation ID check
        switch((int) g_sonyproduct_typeinfo.modelVariationID){
        case 0x00: // AEP/PA-T2S2-1Tuner
        case 0x01: // AEP/PA-T2S2-2Tuner
        case 0x02: // PA/PH-T2-1Tuner
        case 0x06: // LA-1Tuner
        case 0x07: // LA-2Tuner
            // Artemis Model
            break;

        case 0x03: // TW-1Tuner
        case 0x04: // CH/HK-1Tuner
        case 0x05: // UC/MX-1Tuner
            // Non Artemis Model
            mcDBG_MSG_L2(("CXD2854[%d] : Warning : This model does not include CXD2854. (%d)\n", tunerID, g_sonyproduct_typeinfo.modelVariationID));
            break;

        default:
            // Error!?
            mcDBG_MSG_L2(("CXD2854[%d] : Fatal Error!! Invalid Model Variation ID!! (%d)\n", tunerID, g_sonyproduct_typeinfo.modelVariationID));
            break;
        }

        // Demod I2C Connection
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // Salvia
            // I2C_D is used
            sony_i2c_MTK_CreateI2c(pI2c, SONY_I2C_MTK_BUSID_D);
        }else{
            // Sakura
            // For SCT, I2C_D is used
            // For CVT or later, I2C_A is used for main tuner, and I2C_D is used for sub tuner.
            if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
                sony_i2c_MTK_CreateI2c(pI2c, SONY_I2C_MTK_BUSID_D);
            }else{
                if(tunerID == 1){
                    sony_i2c_MTK_CreateI2c(pI2c, SONY_I2C_MTK_BUSID_D);
                }else{
                    sony_i2c_MTK_CreateI2c(pI2c, SONY_I2C_MTK_BUSID_A);
                }
            }
        }

        // Tuner I2C Connection (For tuner register dump)
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // Salvia
            // I2C_D is used
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sony_i2c_MTK_CreateI2cGw(pI2cTuner, SONY_DEMOD_IIC_ADDRESS_0, 0x09, SONY_I2C_MTK_BUSID_D);
            #else
            sony_i2c_MTK_CreateI2c(pI2cTuner, SONY_I2C_MTK_BUSID_D);
            #endif
        }else{
            // Sakura
            // Single, Dual main tuner, use I2C_A and direct access.
            // Dual sub tuner, use I2C_D. Use Repeater for SCT, and direct access for CVT or later.
            if(tunerID == 1){
                // Note : I2C Repeater will be controlled by demod side.
                #ifdef CC_I2C_GATEWAY_SUPPORT
                sony_i2c_MTK_CreateI2cGw(pI2cTuner, SONY_DEMOD_IIC_ADDRESS_1, 0x09, SONY_I2C_MTK_BUSID_D);
                #else
                sony_i2c_MTK_CreateI2c(pI2cTuner, SONY_I2C_MTK_BUSID_D);
                #endif
            }else{
                #ifdef CC_I2C_GATEWAY_SUPPORT
                sony_i2c_MTK_CreateI2cGw(pI2cTuner, SONY_DEMOD_IIC_ADDRESS_0, 0x09, SONY_I2C_MTK_BUSID_A);
                #else
                sony_i2c_MTK_CreateI2c(pI2cTuner, SONY_I2C_MTK_BUSID_A);
                #endif
            }
        }

        // Tuner struct creation used in CXD2854 driver
        sony_result = sony_tuner_MTK_Create(pTuner);
        if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

        // sony_tuner_terr_cable_t member is set for tuner register dump.
        pTuner->pI2c = pI2cTuner;
        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            pTuner->i2cAddress = 0xC0;
        }else{
            if(tunerID == 1){
                pTuner->i2cAddress = 0xC2;
            }else{
                pTuner->i2cAddress = 0xC0;
            }
        }

        // Demod I2C Address
        if(tunerID == 1){
            i2cAddressDemod = SONY_DEMOD_IIC_ADDRESS_1;
        }else{
            i2cAddressDemod = SONY_DEMOD_IIC_ADDRESS_0;
        }

        // Check Demod Chip ID and Create
        {
            uint8_t data = 0x00;
            sony_result = pI2c->WriteOneRegister(pI2c, i2cAddressDemod + 4, 0x00, 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("CXD2854[%d] : Chip ID Check Error!! (%s).\n", tunerID, sony_FormatResult(sony_result)));
                return DRVAPI_TUNER_ERROR;
            }

            sony_result = pI2c->ReadRegister(pI2c, i2cAddressDemod + 4, 0xFD, &data, 1);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("CXD2854[%d] : Chip ID Check Error!! (%s).\n", tunerID, sony_FormatResult(sony_result)));
                return DRVAPI_TUNER_ERROR;
            }

            if(data == SONY_DEMOD_CHIP_ID_CXD2854){
                // Integ and demod struct creation used in CXD2854 driver
                sony_result = sony_integ_Create(pInteg, SONY_DEMOD_XTAL_24000KHz, i2cAddressDemod, pI2c, pDemod, pTuner);
                if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                demod_xtal = SONY_DEMOD_XTAL_24000KHz;
            }else{
                // Non-CXD2854 case.
                sony_result = sony_integ_Create(pInteg, SONY_DEMOD_XTAL_41000KHz, i2cAddressDemod, pI2c, pDemod, pTuner);
                if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                demod_xtal = SONY_DEMOD_XTAL_41000KHz;
            }
        }

        // I2C Repeater Setting
        // Tuner can be access directly except for SCT, Sub-Tuner case.
        pDemod->i2cRepeaterEnable = 0;
        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            if(g_sonyproduct_typeinfo.eProjectID == 0x01){ //Salvia
                #ifndef CC_I2C_GATEWAY_SUPPORT
                pDemod->i2cRepeaterEnable = 1;
                #endif
            }else{
                if(tunerID == 1){ // Sub
                    pDemod->i2cRepeaterEnable = 1;
                }
            }
        }

        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
	        is_parallel = 1; // Parallel Output Only
        }else{
		    is_parallel = 0; // Serial Output Only
        }
    }
    //ITuner_OP(ITunerGetCtx_Multi(tunerID), itSetTunerInit, (UINT8)TRUE, NULL);

#else  // CC_PROJECT_FOR_SONY

    // For Normal Projects

    // NOTE: Please set correct demod xtal frequency here!!
    // SONY_DEMOD_XTAL_20500KHz : 20.5MHz
    // SONY_DEMOD_XTAL_24000KHz : 24MHz (default)
    // SONY_DEMOD_XTAL_41000KHz : 41MHz

    demod_xtal = SONY_DEMOD_XTAL_24000KHz;
	
    #ifdef CC_SONY_DEMOD_TSOUTPUT_SERIAL
    is_parallel = FALSE;
    mcDBG_MSG_L2(("CXD2854[%d]: Using SERIAL TS output!!\n", tunerID));
    #else
    // NOTE: Please set TS serial or parallel here!!
    is_parallel = TRUE;
    mcDBG_MSG_L2(("CXD2854[%d]: Using PARALLEL TS output!!\n", tunerID));
    #endif

    // Sony specific initialization -------------------------------------------

    // I2C struct creation used in CXD2854 driver
    sony_result = sony_i2c_MTK_CreateI2c(pI2c, SONY_I2C_MTK_BUSID_D);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Tuner struct creation used in CXD2854 driver
    sony_result = sony_tuner_MTK_Create(pTuner);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // These settings are for Tuner register dump
#ifdef CC_SONYDEMOD_I2CGATEWAY
    sony_result = sony_i2c_MTK_CreateI2cGw(pI2cTuner,
        (tunerID == 1) ? SONY_DEMOD_IIC_ADDRESS_1 : SONY_DEMOD_IIC_ADDRESS_0,
        0x09, SONY_I2C_MTK_BUSID_D);
#else
    sony_result = sony_i2c_MTK_CreateI2c(pI2cTuner, SONY_I2C_MTK_BUSID_D);
#endif
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    if(tunerID == 1){
        pTuner->i2cAddress = 0xC2;
    }else{
        pTuner->i2cAddress = 0xC0;
    }
    pTuner->pI2c = pI2cTuner;

    // Integ and demod struct creation used in CXD2854 driver
    sony_result = sony_integ_Create(pInteg, demod_xtal,
        (tunerID == 1) ? SONY_DEMOD_IIC_ADDRESS_1 : SONY_DEMOD_IIC_ADDRESS_0,
        pI2c, pDemod, pTuner);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

#ifdef CC_SONYDEMOD_I2CGATEWAY
    pDemod->i2cRepeaterEnable = 0;
#else
    pDemod->i2cRepeaterEnable = 1;
#endif
mcDBG_MSG_L2(("CXD2854[%d] : pDemod->i2cRepeaterEnable = [%d].\n", pDemod->i2cRepeaterEnable));

#endif // CC_PROJECT_FOR_SONY

    mcDBG_MSG_L2(("CXD2854[%d] : Starting CXD2854 Demod Initialization.\n", tunerID));

    // Sony demod initialization
    sony_result = sony_integ_InitializeT_C(pInteg);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("CXD2854[%d] : Error in sony_integ_InitializeT_C (%s).\n", tunerID, sony_FormatResult(sony_result)));
        return DRVAPI_TUNER_ERROR;
    }

    // IF frequency setting for CXD2854
    // This value is optimized for Sony silicon tuner ASCOT2E/ASCOT3/HELENE.
    pDemod->iffreqConfig.configDVBT_5 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.6);
    pDemod->iffreqConfig.configDVBT_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.6);
    pDemod->iffreqConfig.configDVBT_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.2);
    pDemod->iffreqConfig.configDVBT_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.8);

    pDemod->iffreqConfig.configDVBT2_1_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.5);
    pDemod->iffreqConfig.configDVBT2_5 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.6);
    pDemod->iffreqConfig.configDVBT2_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.6);
    pDemod->iffreqConfig.configDVBT2_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.2);
    pDemod->iffreqConfig.configDVBT2_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.8);

    pDemod->iffreqConfig.configDVBC_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.7);
    pDemod->iffreqConfig.configDVBC_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.9);
    pDemod->iffreqConfig.configDVBC_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.9);
    pDemod->iffreqConfig.configDVBC2_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.7);
    pDemod->iffreqConfig.configDVBC2_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.9);

    pDemod->iffreqConfig.configISDBT_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 3.55);
    pDemod->iffreqConfig.configISDBT_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.15);
    pDemod->iffreqConfig.configISDBT_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(demod_xtal, 4.75);

    // Tuner optimization setting (For Sony ASCOT2E/ASCOT3/HELENE silicon tuner)
    pDemod->tunerOptimize = SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3;
    // pDemod->tunerOptimize = SONY_DEMOD_TUNER_OPTIMIZE_UNKNOWN; // For not Sony tuner

    // NOTE: Following setting is necessary only for serial TS output.
    if(is_parallel){
        // It's hardware default value so additional setting is unnecessary.
    }else{
        // TS output format (Serial)
        sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 0);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("CXD2854[%d] : Error in sony_demod_SetConfig (%s).\n", tunerID, sony_FormatResult(sony_result)));
            return DRVAPI_TUNER_ERROR;
        }

        // TS output from TSDATA[0] (For Serial)
        sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("CXD2854[%d] : Error in sony_demod_SetConfig (%s).\n", tunerID, sony_FormatResult(sony_result)));
            return DRVAPI_TUNER_ERROR;
        }
    }

#ifdef CC_PROJECT_FOR_SONY
    // EWS output enable from GPIO0 pin
    sony_result = sony_demod_GPIOSetConfig(pDemod, 0, 1, SONY_DEMOD_GPIO_MODE_EWS_FLAG);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("CXD2854[%d] : Error in sony_demod_GPIOSetConfig (%s).\n", tunerID, sony_FormatResult(sony_result)));
        return DRVAPI_TUNER_ERROR;
    }
    
    // GPIO1 should be used for PWM output (RFAGC Defeat)

#else // CC_PROJECT_FOR_SONY
    // TSERR output enable from GPIO2 pin
    sony_result = sony_demod_GPIOSetConfig(pDemod, 2, 1, SONY_DEMOD_GPIO_MODE_TS_ERROR);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("CXD2854[%d] : Error in sony_demod_GPIOSetConfig (%s).\n", tunerID, sony_FormatResult(sony_result)));
        return DRVAPI_TUNER_ERROR;
    }
#endif // CC_PROJECT_FOR_SONY

    // Sony specific initialization (end) -------------------------------------

    // NOTE: Tuner initialization (ITuner_Init) is done in sony_integ_Initialize

    return (DRVAPI_TUNER_OK);
}

//-----------------------------------------------------------------------------
// Tuner function implementation necessary for integ code (Sony specific)
// Just call ITuner_XXXX APIs
//-----------------------------------------------------------------------------
static sony_result_t sony_tuner_MTK_Initialize (sony_tuner_terr_cable_t * pTuner)
{
    UINT8 tunerID = 0;
    ITUNER_CTX_T *pTunerCtx = NULL;

    SONY_TRACE_ENTER ("sony_tuner_MTK_Initialize");

    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if(pTuner == &g_sony_demod_ctx[1].tuner){
        tunerID = 1;
    }else{
        tunerID = 0;
    }

    pTunerCtx = ITunerGetCtx_Multi(tunerID);
    if(pTunerCtx != NULL)
    {
        pTunerCtx->u1TunerCellID = tunerID;
        ITuner_Init(pTunerCtx);
        if(ITuner_OP(pTunerCtx, itSetTunerInit, (UINT8)TRUE, NULL) != ITUNER_OK){
            mcDBG_MSG_L1(("Error in ITuner_OP(itSetTunerInit).\n"));
        }
    }
    else
    {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t sony_tuner_MTK_Tune (sony_tuner_terr_cable_t * pTuner,
                                      uint32_t frequencyKHz,
                                      sony_dtv_system_t system,
                                      sony_dtv_bandwidth_t bandwidth)
{
    UINT8 tunerID = 0;

    SONY_TRACE_ENTER ("sony_tuner_MTK_Tune");
    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if(pTuner == &g_sony_demod_ctx[1].tuner){
        tunerID = 1;
    }else{
        tunerID = 0;
    }

    pTuner->system = system;
    pTuner->bandwidth = bandwidth;
    pTuner->frequencyKHz = frequencyKHz;

    {
        PARAM_SETFREQ_T param;
        UINT8 sawbw = 0;

        param.Freq = frequencyKHz;
        switch(system)
        {
        case SONY_DTV_SYSTEM_DVBT:
        default:
            param.Modulation = MOD_DVBT;
            break;
        case SONY_DTV_SYSTEM_DVBT2:
            param.Modulation = MOD_DVBT2;
            break;
        case SONY_DTV_SYSTEM_DVBC:
            param.Modulation = MOD_DVBC;
            break;
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
        case SONY_DTV_SYSTEM_DVBC2:
            param.Modulation = MOD_DVBC2;
            break;
        #endif
        case SONY_DTV_SYSTEM_ISDBT:
            param.Modulation = MOD_ISDBT;
            break;
        }

        // Set Bandwidth
        switch(bandwidth){
        case SONY_DTV_BW_1_7_MHZ:
            sawbw = SAW_BW_1_7M; break;
        case SONY_DTV_BW_6_MHZ:
            sawbw = SAW_BW_6M; break;
        case SONY_DTV_BW_7_MHZ:
            sawbw = SAW_BW_7M; break;
        case SONY_DTV_BW_8_MHZ:
        default:
            sawbw = SAW_BW_8M; break;
        }

        // Call ITuner_OP to set BW
        ITuner_OP(ITunerGetCtx_Multi(tunerID), itSetSawBw, sawbw, NULL);

        // Set Tuner PLL
        if(ITuner_SetFreq(ITunerGetCtx_Multi(tunerID), &param))
        {
            mcDBG_MSG_ERR(("CXD2854[%d] : ITuner_SetFreq failed\n", tunerID));
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
        }

        mcDBG_MSG_L2(("CXD2854[%d] : RF tuner control finished.\n", tunerID));
        mcDBG_MSG_L2(("CXD2854[%d] : Frequency = %d\n", tunerID, param.Freq));
        mcDBG_MSG_L2(("CXD2854[%d] : Modulation = %d.\n", tunerID, param.Modulation));
        mcDBG_MSG_L2(("CXD2854[%d] : Bandwidth = %d.\n", tunerID, sawbw));
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

// This function should return dB * 1000 value.
static sony_result_t sony_tuner_MTK_ReadRFLevel (sony_tuner_terr_cable_t * pTuner, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;
    INT32 rssi = 0;
    INT16 getRssiResult = 0;
    sony_integ_t * pInteg;
    UINT8 tunerID = 0;

    SONY_TRACE_ENTER ("sony_tuner_MTK_ReadRFLevel");

    if ((!pTuner) || (!pRFLeveldB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if(pTuner == &g_sony_demod_ctx[1].tuner){
        pInteg = &g_sony_demod_ctx[1].integ;
        tunerID = 1;
    }else{
        pInteg = &g_sony_demod_ctx[0].integ;
        tunerID = 0;
    }

    // Call ITuner_OP to get RSSI
    // NOTE: The RSSI value is dB * 100 integer.
    getRssiResult = ITuner_OP(ITunerGetCtx_Multi(tunerID), itGetRSSI, 0, &rssi);

    if(getRssiResult == ITUNER_OK){
        // Compensation using Internal Digital AGC value
        uint16_t agcreg = 0;
        int32_t agcgain = 0;

        *pRFLeveldB = 10 * rssi; // Calculate in dB * 1000 value.

        result = sony_demod_terr_cable_monitor_InternalDigitalAGCOut(pInteg->pDemod, &agcreg);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        switch (pInteg->pDemod->system) {
        case SONY_DTV_SYSTEM_DVBT:
        case SONY_DTV_SYSTEM_DVBT2:
        case SONY_DTV_SYSTEM_DVBC2:
        case SONY_DTV_SYSTEM_ISDBT:
            /* AGCGain = -0.0184 * (AGCReg - 528) */
            agcgain = -184 * (agcreg - 528);
            break;
        case SONY_DTV_SYSTEM_DVBC:
            /* AGCGain = -0.0184 * (AGCReg - 506) */
            agcgain = -184 * (agcreg - 506);
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* Get dB x 1000 value */
        if (agcgain >= 0) {
            agcgain = (agcgain + 5) / 10;
        } else {
            agcgain = (agcgain - 5) / 10;
        }

        mcDBG_MSG_L2(("CXD2854[%d] : Internal Digital AGC Reg = 0x%04X\n", tunerID, agcreg));
        mcDBG_MSG_L2(("CXD2854[%d] : Internal Digital AGC Gain (dB x 1000) = %d\n", tunerID, agcgain));

        *pRFLeveldB += agcgain;
    }else{
        *pRFLeveldB = -100 * 1000;
        result = SONY_RESULT_ERROR_OTHER;
    }

    mcDBG_MSG_L2(("CXD2854[%d] : RFLevel (dB x 1000) = %d\n", tunerID, *pRFLeveldB));

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_MTK_Create (sony_tuner_terr_cable_t * pTuner)
{
    SONY_TRACE_ENTER ("sony_tuner_MTK_Create");

    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pTuner->Initialize = sony_tuner_MTK_Initialize;
    pTuner->Shutdown = NULL;
    pTuner->Sleep = NULL;
    pTuner->Tune = sony_tuner_MTK_Tune;
    pTuner->ReadRFLevel = sony_tuner_MTK_ReadRFLevel;
    pTuner->CalcRFLevelFromAGC = NULL;
    pTuner->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->i2cAddress = 0; // Dummy
    pTuner->pI2c = NULL; // Dummy
    pTuner->flags = 0;
    pTuner->user = NULL;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

//-----------------------------------------------------------------------------
// Stopwatch function implementation necessary for integ code (Sony specific)
// NOTE: sony_stopwatch_elapsed is implemented in each glue files.
//-----------------------------------------------------------------------------
sony_result_t sony_stopwatch_start (sony_stopwatch_t * pStopwatch)
{
    SONY_TRACE_ENTER("sony_stopwatch_start");

    if (!pStopwatch) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = x_os_get_sys_tick() * x_os_get_sys_tick_period();

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_stopwatch_sleep (sony_stopwatch_t * pStopwatch, uint32_t ms)
{
    SONY_TRACE_ENTER("sony_stopwatch_sleep");
    if (!pStopwatch) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    SONY_ARG_UNUSED(*pStopwatch);
    SONY_SLEEP (ms);

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_stopwatch_elapsed (sony_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
    SONY_TRACE_ENTER("sony_stopwatch_elapsed");

    if (!pStopwatch || !pElapsed) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    *pElapsed = x_os_get_sys_tick() * x_os_get_sys_tick_period() - pStopwatch->startTime;

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

//-----------------------------------------------------------------------------
// Check cancelation API (Sony specific)
// NOTE: This API will check break too
//-----------------------------------------------------------------------------
sony_result_t sony_integ_CheckCancellation (sony_integ_t * pInteg)
{
    UINT8 tunerID = 0;
    sony_demod_ctx_t *pSonyDemodCtx;

    SONY_TRACE_ENTER ("sony_integ_CheckCancellation");

    /* Argument verification. */
    if (!pInteg) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if(pInteg == &g_sony_demod_ctx[1].integ){
        tunerID = 1;
    }else{
        tunerID = 0;
    }

    pSonyDemodCtx = &g_sony_demod_ctx[tunerID];

    /* Check break here!! */
    if(pSonyDemodCtx->breakFct){
        if(pSonyDemodCtx->breakFct(pSonyDemodCtx->breakFctArg)){
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_CANCEL);
        }
    }

    /* Check the cancellation flag. */
    if (sony_atomic_read (&(pInteg->cancel)) != 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_CANCEL);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

//-----------------------------------------------------------------------------
// Sony specific utils
//-----------------------------------------------------------------------------
// This function is used in DtdCommand function
UINT32 sony_strtoul(const char* str)
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

const char* sony_FormatResult(sony_result_t result)
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
    case SONY_RESULT_ERROR_OVERFLOW:
        return "ERROR_OVERFLOW";
    case SONY_RESULT_OK_CONFIRM:
        return "OK_CONFIRM";
    default:
        return "Unknown";
    }
}

// Function trace (For Debug)
void sony_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONY_ENTER : %s (%d)\n", funcname, linenum));
}

void sony_trace_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONY_RESULT_OK){
        mcDBG_MSG_L4(("SONY_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONY_RETURN : (line %d)\n", linenum));
    }
}

// Function trace for I2C functions (For Debug)
void sony_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sony_trace_log_enter(funcname, filename, linenum);
}
void sony_trace_i2c_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    sony_trace_log_return(result, filename, linenum);
}
