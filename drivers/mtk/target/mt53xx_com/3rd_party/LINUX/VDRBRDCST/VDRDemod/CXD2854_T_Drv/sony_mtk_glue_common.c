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

//#define SONY_DEMOD_IIC_ADDRESS 0xD8

// Sony CXD2854 driver object instance
sony_demod_ctx_t g_sony_demod_ctx;

static sony_result_t sony_tuner_MTK_Create (sony_tuner_terr_cable_t * pTuner);

INT32 sony_mtk_glue_common_Initialize (void)
{
    // Initialize Sony CXD2854 driver/demod and tuner
    // Called from XXXX_TunerOpen APIs.
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_tuner_terr_cable_t *pTuner = &g_sony_demod_ctx.tuner;
    sony_i2c_t *pI2c = &g_sony_demod_ctx.i2c;
    sony_demod_xtal_t demod_xtal;
    BOOL is_parallel = FALSE;
	
    ITUNER_CTX_T *psTunerCtx ;
    psTunerCtx=ITunerGetCtx();
	
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set correct demod xtal frequency here!!
    // SONY_DEMOD_XTAL_20500KHz : 20.5MHz
    // SONY_DEMOD_XTAL_24000KHz : 24MHz (default)
    // SONY_DEMOD_XTAL_41000KHz : 41MHz
   /* #if defined SONY_DEMOD_CRYSTAL_41M
    demod_xtal = SONY_DEMOD_XTAL_41000KHz;
    mcDBG_MSG_L2(("CXD2854: Using 41MHz crystal!!\n"));
    #elif defined SONY_DEMOD_CRYSTAL_20_5M
    demod_xtal = SONY_DEMOD_XTAL_20500KHz;
    mcDBG_MSG_L2(("CXD2854: Using 20.5MHz crystal!!\n"));
    #else // 24MHz
    demod_xtal = SONY_DEMOD_XTAL_24000KHz;
    mcDBG_MSG_L2(("CXD2854: Using 24MHz crystal!!\n"));
    #endif
*/
    demod_xtal = SONY_DEMOD_XTAL_24000KHz;
	
    #ifdef CC_SONY_DEMOD_TSOUTPUT_SERIAL
    is_parallel = FALSE;
    mcDBG_MSG_L2(("CXD2854: Using SERIAL TS output!!\n"));
    #else
    // NOTE: Please set TS serial or parallel here!!
    is_parallel = TRUE;
    mcDBG_MSG_L2(("CXD2854: Using PARALLEL TS output!!\n"));
    #endif

    // Sony specific initialization -------------------------------------------
    g_sony_demod_ctx.autoPLP = 1;
    g_sony_demod_ctx.frequency = 0;
    g_sony_demod_ctx.fgEws = FALSE; // clear EWS flag.
    g_sony_demod_ctx.breakFct = NULL;
    g_sony_demod_ctx.breakFctArg = NULL;

    // I2C struct creation used in CXD2854 driver
    sony_result = sony_i2c_MTK_CreateI2c(pI2c);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Tuner struct creation used in CXD2854 driver
    sony_result = sony_tuner_MTK_Create(pTuner);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Integ and demod struct creation used in CXD2854 driver 
    
    sony_result = sony_integ_Create(pInteg, demod_xtal, psTunerCtx->u1DemodI2cAddress_Drv, pI2c, pDemod, pTuner);
	
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2854 Demod Initialization.\n"));

    // Sony demod initialization
    sony_result = sony_integ_InitializeT_C(pInteg);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_integ_InitializeT_C (%s).\n", sony_FormatResult(sony_result)));
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
            mcDBG_MSG_L1(("Error in sony_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
            return DRVAPI_TUNER_ERROR;
        }

        // TS output from TSDATA[0] (For Serial)
        sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
            return DRVAPI_TUNER_ERROR;
        }
    }

    // TSERR output enable from GPIO2 pin
    sony_result = sony_demod_GPIOSetConfig(pDemod, 2, 1, SONY_DEMOD_GPIO_MODE_TS_ERROR);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_demod_GPIOSetConfig (%s).\n", sony_FormatResult(sony_result)));
        return DRVAPI_TUNER_ERROR;
    }

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
   
   ITUNER_CTX_T *psTunerCtx ;
   psTunerCtx=ITunerGetCtx();
   SONY_TRACE_ENTER ("sony_tuner_MTK_Initialize");
    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    ITuner_Init(ITunerGetCtx());
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t sony_tuner_MTK_Tune (sony_tuner_terr_cable_t * pTuner,
                                      uint32_t frequencyKHz,
                                      sony_dtv_system_t system,
                                      sony_dtv_bandwidth_t bandwidth)
{
    SONY_TRACE_ENTER ("sony_tuner_MTK_Tune");
    if (!pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
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
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
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
    pTuner->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->i2cAddress = 0; // Dummy
    pTuner->pI2c = NULL; // Unnecessary
    pTuner->flags = 0;
    pTuner->user = NULL;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

// This function should return dB * 1000 value.
sony_result_t sony_integ_monitor_RFLevel_MTK (sony_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;
    INT32 rssi = 0;
    INT16 getRssiResult = 0;

    SONY_TRACE_ENTER ("sony_integ_monitor_RFLevel_MTK");

    if (!pRFLeveldB) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_demod_I2cRepeaterEnable(pInteg->pDemod, 1);
    if(result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    // Call ITuner_OP to get RSSI
    // NOTE: The RSSI value is dB * 100 integer.
    getRssiResult = ITuner_OP(ITunerGetCtx(), itGetRSSI, 0, &rssi);

    result = sony_demod_I2cRepeaterEnable(pInteg->pDemod, 0);
    if(result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

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

        mcDBG_MSG_L2(("Internal Digital AGC Reg = 0x%04X\n", agcreg));
        mcDBG_MSG_L2(("Internal Digital AGC Gain (dB x 1000) = %d\n", agcgain));

        *pRFLeveldB += agcgain;
    }else{
        *pRFLeveldB = -100 * 1000;
        result = SONY_RESULT_ERROR_OTHER;
    }

    mcDBG_MSG_L2(("RFLevel (dB x 1000) = %d\n", *pRFLeveldB));

    SONY_TRACE_RETURN (result);
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

    /* Check break here!! */
    if(g_sony_demod_ctx.breakFct){
        if(g_sony_demod_ctx.breakFct(g_sony_demod_ctx.breakFctArg)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_CANCEL);
        }
    }

    SONY_TRACE_RETURN(SONY_RESULT_OK);
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
