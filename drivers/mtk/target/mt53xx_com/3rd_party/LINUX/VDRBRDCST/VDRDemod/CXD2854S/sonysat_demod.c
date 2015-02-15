/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#include "sonysat_integ.h"
#include "sonysat_demod.h"
#include "sonysat_stdlib.h"

#ifdef SONYSAT_DEMOD_SUPPORT_DVBT
#include "sonysat_demod_dvbt.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBC
#include "sonysat_demod_dvbc.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBT2
#include "sonysat_demod_dvbt2.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBC2
#include "sonysat_demod_dvbc2.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
#include "sonysat_demod_dvbs_s2.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_ISDBT
#include "sonysat_demod_isdbt.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_ISDBS
#include "sonysat_demod_isdbs.h"
#endif
/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Shutdown to Sleep T / C state
*/
static sonysat_result_t SDtoSLT(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from Shutdown to Sleep S state
*/
static sonysat_result_t SDtoSLS(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from any Sleep T / C state to Shutdown
*/
static sonysat_result_t SLTtoSD(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from any Sleep S state to Shutdown
*/
static sonysat_result_t SLStoSD(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from Sleep S to Sleep T / C state
*/
static sonysat_result_t SLStoSLT(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from Sleep T / C to Sleep S state
*/
static sonysat_result_t SLTtoSLS(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from any state to Sleep T / C state.
        This is used as a demodulator reset, all demodulator configuration
        settings will be lost.
*/
static sonysat_result_t XtoSLT(sonysat_demod_t * pDemod);

/**
 @brief Configure the demodulator from any state to Sleep S state.
        This is used as a demodulator reset, all demodulator configuration
        settings will be lost.
*/
static sonysat_result_t XtoSLS(sonysat_demod_t * pDemod);

/**
 @brief Clear the demodulator configuration memory table.  Use
        this API to empty the table of previous entries.  This is called 
        automatically in the sonysat_integ_InitializeT_C and 
        sonysat_integ_InitializeS API's.
*/
static sonysat_result_t clearConfigMemory (sonysat_demod_t * pDemod);

/**
 @brief Iterate through the demodulator configuration memory table and write
        each entry to the device.  This is called automatically during a 
        transition from ::SONYSAT_DEMOD_STATE_SHUTDOWN to 
        ::SONYSAT_DEMOD_STATE_SLEEP_T_C or ::SONYSAT_DEMOD_STATE_SLEEP_S or states.
*/
static sonysat_result_t loadConfigMemory (sonysat_demod_t * pDemod);

/**
 @brief Save an entry into the demodulator configuration memory table.
 
 @param pDemod The demodulator instance.
 @param slaveAddress Slave address of configuration setting
 @param bank Demodulator bank of configuration setting
 @param registerAddress Register address of configuration setting
 @param value The value being written to this register
 @param mask The bit mask used on the register
*/
static sonysat_result_t setConfigMemory (sonysat_demod_t * pDemod, 
                                      uint8_t slaveAddress, 
                                      uint8_t bank, 
                                      uint8_t registerAddress,
                                      uint8_t value,
                                      uint8_t bitMask);

/*------------------------------------------------------------------------------
 System Support Dependant Function Removal
------------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_SUPPORT_DVBC2
#define sonysat_demod_dvbc2_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_DVBT2
#define sonysat_demod_dvbt2_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_DVBC
#define sonysat_demod_dvbc_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_DVBT
#define sonysat_demod_dvbt_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_DVBS_S2
#define sonysat_demod_dvbs_s2_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_ISDBT
#define sonysat_demod_isdbt_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONYSAT_DEMOD_SUPPORT_ISDBS
#define sonysat_demod_isdbs_Sleep(pDemod) SONYSAT_RESULT_ERROR_NOSUPPORT
#endif

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_Create (sonysat_demod_t * pDemod,
                                 sonysat_demod_xtal_t xtalFreq,
                                 uint8_t i2cAddress,
                                 sonysat_i2c_t * pDemodI2c)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_demod_Create");

    if ((!pDemod) || (!pDemodI2c)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    
    sonysat_memset (pDemod, 0, sizeof (sonysat_demod_t));

    pDemod->xtalFreq = xtalFreq;
    pDemod->i2cAddressSLVT = i2cAddress;
    pDemod->i2cAddressSLVX = i2cAddress + 4;
    pDemod->pI2c = pDemodI2c;
    pDemod->state = SONYSAT_DEMOD_STATE_UNKNOWN;
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    pDemod->serialTsClkFreqTerrCable = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_SAT
    pDemod->serialTsClkFreqSat = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->satTunerIqSense = SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL;
    pDemod->isSinglecable = 0;
    pDemod->isSinglecableIqInv = 1;
    pDemod->spectrumMode = 0;
    pDemod->spectrumCount = 4;
#endif
    pDemod->serialTsClockModeContinuous = 1;
    pDemod->isTsBackwardsCompatibleMode = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_InitializeT_C (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_InitializeT_C");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    pDemod->serialTsClkFreqTerrCable = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_SAT
    pDemod->serialTsClkFreqSat = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->satTunerIqSense = SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL;
    pDemod->isSinglecable = 0;
    pDemod->isSinglecableIqInv = 1;
    pDemod->spectrumMode = 0;
    pDemod->spectrumCount = 4;
#endif
    pDemod->serialTsClockModeContinuous = 1;
    pDemod->isTsBackwardsCompatibleMode = 0;

    /* Initialize causes demodulator register reset */
    result = XtoSLT(pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_T_C;

    result = sonysat_demod_ChipID (pDemod, &(pDemod->chipId));
    if (result != SONYSAT_RESULT_OK) {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (result);
    }

    if ((pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) || (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz)) {
        if (!SONYSAT_DEMOD_CHIP_ID_2k12_GENERATION (pDemod->chipId) &&
            !SONYSAT_DEMOD_CHIP_ID_2k14_GENERATION (pDemod->chipId)) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
    }
    else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* 24MHz Xtal support is not available on 2k12 Generation Demod Chips. */
        if (!SONYSAT_DEMOD_CHIP_ID_2k14_GENERATION (pDemod->chipId)) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
    }
    else {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
    }

    /* Clear Config memory in Initialize API */
    result = clearConfigMemory (pDemod);
    if (result != SONYSAT_RESULT_OK) {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_InitializeS (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_InitializeS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    pDemod->serialTsClkFreqTerrCable = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_SAT
    pDemod->serialTsClkFreqSat = SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->satTunerIqSense = SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL;
    pDemod->isSinglecable = 0;
    pDemod->isSinglecableIqInv = 1;
    pDemod->spectrumMode = 0;
    pDemod->spectrumCount = 4;
#endif
    pDemod->serialTsClockModeContinuous = 1;
    pDemod->isTsBackwardsCompatibleMode = 0;

    /* Initialize causes demodulator register reset */
    result = XtoSLS(pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_S;

    result = sonysat_demod_ChipID (pDemod, &(pDemod->chipId));
    if (result != SONYSAT_RESULT_OK) {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (result);
    }

    if ((pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) || (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz)) {
        if (!SONYSAT_DEMOD_CHIP_ID_2k12_GENERATION (pDemod->chipId) &&
            !SONYSAT_DEMOD_CHIP_ID_2k14_GENERATION (pDemod->chipId)) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
    }
    else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* 24MHz Xtal support is not available on 2k12 Generation Demod Chips. */
        if (!SONYSAT_DEMOD_CHIP_ID_2k14_GENERATION (pDemod->chipId)) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
    }
    else {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
    }

    /* Clear Config memory in Initialize API */
    result = clearConfigMemory (pDemod);
    if (result != SONYSAT_RESULT_OK) {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

/* ---> For MTK */
sonysat_result_t sonysat_demod_ReinitializeS (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_ReinitializeS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Initialize causes demodulator register reset */
    result = XtoSLS(pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_S;

    /* Clear Config memory in Initialize API */
    result = loadConfigMemory (pDemod);
    if (result != SONYSAT_RESULT_OK) {
        pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}
/* <--- For MTK */

sonysat_result_t sonysat_demod_SleepT_C (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_Sleep_T_C");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state == SONYSAT_DEMOD_STATE_SHUTDOWN) {
        result = SDtoSLT(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_S) {
        /* First transition to SLEEP_S state */
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBS:
        case SONYSAT_DTV_SYSTEM_DVBS2:
        case SONYSAT_DTV_SYSTEM_ANY: /* Tune (S/S2 auto), TuneSRS, BlindScan */
            result = sonysat_demod_dvbs_s2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBS:
            result = sonysat_demod_isdbs_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }
        
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
        
        /* Then transfer to SLEEP_T_C state */
        result = SLStoSLT(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_T_C) {
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBT:
            result = sonysat_demod_dvbt_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBT2:
            result = sonysat_demod_dvbt2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC:
            result = sonysat_demod_dvbc_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC2:
            result = sonysat_demod_dvbc2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBT:
            result = sonysat_demod_isdbt_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_T_C) {
        /* Do nothing */
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S) {
        /* Transfer to SLEEP_T_C state */
        result = SLStoSLT(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }        

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_T_C;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_SleepS (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_SleepS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state == SONYSAT_DEMOD_STATE_SHUTDOWN) {
        result = SDtoSLS(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_T_C) {
        /* First transition to SLEEP_T_C state */
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBT:
            result = sonysat_demod_dvbt_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBT2:
            result = sonysat_demod_dvbt2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC:
            result = sonysat_demod_dvbc_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC2:
            result = sonysat_demod_dvbc2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBT:
            result = sonysat_demod_isdbt_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Then transfer to SLEEP_S state */
        result = SLTtoSLS(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_S) {
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBS:
        case SONYSAT_DTV_SYSTEM_DVBS2:
        case SONYSAT_DTV_SYSTEM_ANY: /* Tune (S/S2 auto), TuneSRS, BlindScan */
            result = sonysat_demod_dvbs_s2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBS:
            result = sonysat_demod_isdbs_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }           
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S) {
        /* Do nothing */
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_T_C) {
        /* Transfer to SLEEP_S state */
        result = SLTtoSLS(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }        

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_S;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_Shutdown (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_Shutdown");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_T_C) {
        /* First transition to SLEEP_T_C state */
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBT:
            result = sonysat_demod_dvbt_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBT2:
            result = sonysat_demod_dvbt2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC:
            result = sonysat_demod_dvbc_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_DVBC2:
            result = sonysat_demod_dvbc2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBT:
            result = sonysat_demod_isdbt_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }

        /* Then transfer to SHUTDOWN state */
        result = SLTtoSD(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_S) {
        /* First transition to SLEEP_S state */
        switch (pDemod->system) {
        case SONYSAT_DTV_SYSTEM_DVBS:
        case SONYSAT_DTV_SYSTEM_DVBS2:
        case SONYSAT_DTV_SYSTEM_ANY: /* Tune (S/S2 auto), TuneSRS, BlindScan */
            result = sonysat_demod_dvbs_s2_Sleep(pDemod);
            break;

        case SONYSAT_DTV_SYSTEM_ISDBS:
            result = sonysat_demod_isdbs_Sleep(pDemod);
            break;

        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }   

        /* Then transfer to SHUTDOWN state */
        result = SLStoSD(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_T_C) {
        /* Transfer to SHUTDOWN state */
        result = SLTtoSD(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S) {
        /* Transfer to SHUTDOWN state */
        result = SLStoSD(pDemod);
        if (result != SONYSAT_RESULT_OK) {
            pDemod->state = SONYSAT_DEMOD_STATE_INVALID;
            SONYSAT_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONYSAT_DEMOD_STATE_SHUTDOWN) {
        /* Do Nothing */
    }
    else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);    
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SHUTDOWN;

    SONYSAT_TRACE_RETURN (result);
}

/* ---> For MTK */
sonysat_result_t sonysat_demod_ForceShutdown (sonysat_demod_t * pDemod)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_ForceShutdown");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, &data, 1) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data == 0x01) {
        /* Now in shutdown state. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Disable DSQOUT */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x3F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Disable DSQIN */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    /* Disable oscillator */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_SHUTDOWN;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
/* <--- For MTK */

sonysat_result_t sonysat_demod_TuneEnd (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_TuneEnd");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
     
    if ((pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SW Reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Enable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_SoftReset (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_SoftReset");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SW Reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_SetConfig (sonysat_demod_t * pDemod, 
                                    sonysat_demod_config_id_t configId, 
                                    int32_t value)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_SetConfig");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch (configId) {
    case SONYSAT_DEMOD_CONFIG_PARALLEL_SEL:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t) (value ? 0x00 : 0x80), 0x80);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_SER_DATA_ON_MSB:       
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [3]    1'b1       OSEREXCHGB7
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t) (value ? 0x08 : 0x00), 0x08);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_OUTPUT_SEL_MSB:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [4]    1'b0       OWFMT_LSB1STON
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t) (value ? 0x00 : 0x10), 0x10);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSVALID_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [1]    1'b0       OWFMT_VALINV
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t) (value ? 0x00 : 0x02), 0x02);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSSYNC_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [2]    1'b0       OWFMT_STINV
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t) (value ? 0x00 : 0x04), 0x04);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSERR_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     CBh     [0]    1'b0       OWFMT_ERRINV
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xCB, (uint8_t) (value ? 0x00 : 0x01), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_LATCH_ON_POSEDGE:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [0]    1'b1       OWFMT_CKINV
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSCLK_CONT:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* Store the serial clock mode */
        pDemod->serialTsClockModeContinuous = (uint8_t) (value ? 0x01 : 0x00);
        break;

    case SONYSAT_DEMOD_CONFIG_TSCLK_MASK:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h    C6h     [4:0]     5'b00000    OWFMT_CKDISABLE
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00,  0xC6, (uint8_t) value, 0x1F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSVALID_MASK:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h     C8h     [4:0]    5'b00011    OWFMT_VALDISABLE
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC8, (uint8_t) value, 0x1F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSERR_MASK:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h     C9h     [4:0]    5'b00000    OWFMT_ERRDISABLE
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC9, (uint8_t) value, 0x1F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TSCLK_CURRENT_10mA:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     83h     [0]    1'b0       OREG_TSCLK_C
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x83, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_TS_CURRENT_10mA:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     83h     [1]    1'b0       OREG_TSSYNC_C
         * <SLV-T>  00h     83h     [2]    1'b0       OREG_TSVALID_C
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x83, (uint8_t) (value ? 0x06 : 0x00), 0x06);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     84h     [7:0]  8'h00      OREG_TSDATA_C
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x84, (uint8_t) (value ? 0xFF : 0x00), 0xFF);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        {
            uint8_t data;

            /* Set SLV-X Bank : 0x00 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* slave    Bank    Addr    Bit    default   Value        Name        
             * -------------------------------------------------------------------
             * <SLV-X>  00h     A5h     [3:0]   8'h00     8'h04     OREG_GPIP2_SEL  
             */
            if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xA5, &data, 1) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* Set GPIO2 Drivability, only if in TSERR mode */
            if ((data & 0x0F) == SONYSAT_DEMOD_GPIO_MODE_TS_ERROR) {
                /* slave    Bank    Addr    Bit    default    Name
                 * --------------------------------------------------------
                 * <SLV-X>  00h     85h     [2]    1'b0       OREG_GPIO2_C
                 */
                /* For MTK */
                if (sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVX, 0x00, 0x85, (uint8_t) (value ? 0x04 : 0x00), 0x04) != SONYSAT_RESULT_OK) {
                    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
                }
            }
        }
        break;

    case SONYSAT_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
        
        if ((value < 0) || (value > 0xFF)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        pDemod->parallelTsClkManualSetting = (uint8_t) value;

        break;

    case SONYSAT_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE:
        /* This register can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        pDemod->isTsBackwardsCompatibleMode = (uint8_t) (value ? 1 : 0);

        break;

    case SONYSAT_DEMOD_CONFIG_PWM_VALUE:
        /* Perform range checking on value */
        if ((value < 0) || (value > 0xFFF)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Slave    Bank    Addr    Bit      Default    Value    Name
         * -------------------------------------------------------------------
         * <SLV-T>  00h     B7h     [0]      1'b0       1'b1     OREG_RFAGCSEL
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB7, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>  00h     B2h     [3:0]    8'h00      8'h0x     OREG_GDA_VAL_RFAGC[11:8]
         * <SLV-T>  00h     B3h     [7:0]    8'h00      8'hxx     OREG_GDA_VAL_RFAGC[7:0]
         */
        {
            uint8_t data[2];
            data[0] = (uint8_t) (((uint16_t)value >> 8) & 0x0F);
            data[1] = (uint8_t) ((uint16_t)value & 0xFF);

            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB2, data[0], 0xFF);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }

            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB3, data[1], 0xFF);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }

        break;

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    case SONYSAT_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ:
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            /* This api is accepted in SLEEP state only */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if ((value < 1) || (value > 5)) {
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
        pDemod-> serialTsClkFreqTerrCable = (sonysat_demod_serial_ts_clk_t) value;
        break;

    case SONYSAT_DEMOD_CONFIG_IFAGCNEG:
        /* slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     CBh     [6]      8'h48      8'h08      OCTL_IFAGCNEG
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0xCB, (uint8_t) (value ? 0x40 : 0x00), 0x40);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_IFAGC_ADC_FS:
        {
            uint8_t data;

            if (value == 0) {
                data = 0x50; /* 1.4Vpp - Default */
            }
            else if (value == 1) {
                data = 0x39; /* 1.0Vpp */
            }
            else if (value == 2) {
                data = 0x28; /* 0.7Vpp */
            }
            else {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
            }

            /* Slave     Bank    Addr   Bit      Default    Name
             * -------------------------------------------------------------------
             * <SLV-T>   10h     CDh    [7:0]    8'h50      OCTL_IFAGC_TARGET[7:0]
             */
            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0xCD, data, 0xFF);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case SONYSAT_DEMOD_CONFIG_SPECTRUM_INV:
        /* Store the configured sense. */
        pDemod->confSense = value ? SONYSAT_DEMOD_TERR_CABLE_SPECTRUM_INV : SONYSAT_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
        break;

    case SONYSAT_DEMOD_CONFIG_RFAIN_ENABLE:
        /* This setting can change only in SLEEP state */
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
        
        pDemod->enableRfain = (uint8_t)(value ? 1 : 0);
        break;

    case SONYSAT_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST:
        /* Store the blindTune / Scan system priority setting */
        pDemod->blindTuneDvbt2First = (uint8_t)(value ? 1 : 0);
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT_BERN_PERIOD:
        /* Set the measurment period for Pre-RS BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit    default          Name
         * -----------------------------------------------------------------------
         * <SLV-T>   10h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x60, (uint8_t) (value & 0x1F), 0x1F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBC_BERN_PERIOD:
        /* Set the measurment period for Pre-RS BER (DVB-C). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit    default          Name
         * -----------------------------------------------------------------------
         * <SLV-T>   40h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x40, 0x60, (uint8_t) (value & 0x1F), 0x1F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT_VBER_PERIOD:
        /* Set the measurment period for Pre-Viterbi BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ----------------------------------------------------
        * <SLV-T>  10h     6Fh    [2:0]   OREG_VBER_PERIOD_SEL
        */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x6F, (uint8_t) (value & 0x07), 0x07);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT2C2_BBER_MES:
        /* Set the measurment period for Pre-BCH BER (DVB-T2/C2) and Post-BCH FER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-T>  20h     72h    [3:0]   OREG_BBER_MES
        */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x72, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT2C2_LBER_MES:
        /* Set the measurment period for Pre-LDPC BER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-T>  20h     6Fh    [3:0]   OREG_LBER_MES
        */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x6F, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT_PER_MES:
        /* Set the measurment period for PER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------
         * <SLV-T>   10h     5Ch     [3:0]        OREG_PER_MES[3:0]
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x5C, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBC_PER_MES:
        /* Set the measurment period for PER (DVB-C). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------
         * <SLV-T>   40h     5Ch     [3:0]        OREG_PER_MES[3:0]
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x40, 0x5C, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBT2C2_PER_MES:
        /* Set the measurment period for PER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * -----------------------------------------------
        * <SLV-T>  24h     DCh    [3:0]   OREG_SP_PER_MES
        */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x24, 0xDC, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;
        
    case SONYSAT_DEMOD_CONFIG_ISDBT_BERPER_PERIOD:
        {
            uint8_t data[2];

            data[0] = (uint8_t)((value & 0x00007F00) >> 8);
            data[1] = (uint8_t)(value & 0x000000FF);

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
             * <SLV-T>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
             */
            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x60, 0x5B, data[0], 0x7F);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN(result);
            }
            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x60, 0x5C, data[1], 0xFF);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN(result);
            }
        }
        break;

    case SONYSAT_DEMOD_CONFIG_GPIO_EWS_FLAG:
        {
            uint8_t data;

            if (value == 0) {
                data = 0x01; /* EWS (on TMCC)- Default */
            }
            else if (value == 1) {
                data = 0x80; /* AC EEW */
            }
            else if (value == 2) {
                data = 0x81; /* EWS or AC EEW */
            }
            else {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
            }

            /* Slave     Bank    Addr   Bit  Default    Name
             * -------------------------------------------------------------------
             * <SLV-T>  63h     3Bh    [7:0]  8'h01    8'h80   OPIR_COR_INTEN_B[13:6]
             */
            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x63, 0x3B, data, 0xFF);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;
        
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_SAT
    case SONYSAT_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ:
        if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)) {
            /* This api is accepted in SLEEP state only */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 5)) {
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for satellite systems */
        pDemod->serialTsClkFreqSat = (sonysat_demod_serial_ts_clk_t) value;
        break;

    case SONYSAT_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV:
        pDemod->satTunerIqSense = value? SONYSAT_DEMOD_SAT_IQ_SENSE_INV : SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL;
        break;

    case SONYSAT_DEMOD_CONFIG_SAT_IFAGCNEG:
        /* slave    Bank    Addr   Bit     Name
        * -----------------------------------------------
        * <SLV-T>   A0h     B9h    [0]     ORFAGC_AGCNEG
        */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xB9, (uint8_t) (value? 0x01 : 0x00), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_SAT_TSDATA6_DISEQC:
        {
            uint8_t tsData6Sel;
              
            if (value == 0) {
                tsData6Sel = 0x00;
            }
            else if (value == 1) {
                tsData6Sel = 0x02;
            }
            else if (value == 2) {
                tsData6Sel = 0x03;
            }
            else {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
            }

            /* Slave     Bank   Addr   Bit     Default    Name
             * ----------------------------------------------------------
             * <SLV-T>   00h    A8h    [2:0]   3'b000     OREG_TSDATA6SEL
             */
            result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xA8, tsData6Sel, 0x07);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBSS2_BER_PER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BAh    [3:0]   4'd8       OFBER_MES
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBA, (uint8_t)value, 0xFF);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBS2_BER_FER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BCh    [3:0]   4'd8       OF2BER_MES
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBC, (uint8_t)value, 0xFF);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBS_VBER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BBh    [3:0]   4'd8       OFVBER_MES
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBB, (uint8_t)value, 0xFF);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBS2_LBER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    7Ah    [3:0]   4'd8       OFLBER_MES
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xA0, 0x7A, (uint8_t)value, 0xFF);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_MODE:
        /* Verify range of value */
        if ((value < 0) || (value > 1)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        pDemod->spectrumMode = (uint8_t)value;
        break;

    case SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_COUNT:
        /* Verify range of value */
        if ((value != 0) && (value != 1) && (value != 2) && (value != 4) && (value != 8) &&
            (value != 16) && (value != 32) && (value != 64)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        pDemod->spectrumCount = (uint8_t)value;
        break;

    case SONYSAT_DEMOD_CONFIG_ISDBS_BERNUMCONF:
        /* The measurement unit can be changed by the following register.
         *      Slave   Bank    Addr    Bit    Default     Signal name  Meaning
         *    ------------------------------------------------------------------------------------------------------
         *    <SLV-T>  C0h     A6h     [0]      1'b0       OBERNUMCONF  1'b0: Measurement unit is in Super Frames.
         *                                                              1'b1: Measurement unit is in Slots.
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xA6, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_DEMOD_CONFIG_ISDBS_BER_PERIOD1:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }        
        /*      Slave   Bank    Addr    Bit    Default     Signal name
         *    ----------------------------------------------------------
         *   <SLV-T>  C0h     B1h     [3:0]    4'd1       OBERPERIOD1
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB1, (uint8_t)value, 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }        
        break;
        
    case SONYSAT_DEMOD_CONFIG_ISDBS_BER_PERIOD2:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }        
        /*      Slave   Bank    Addr    Bit    Default     Signal name
         *    ----------------------------------------------------------
         *   <SLV-T>  C0h     B2h     [3:0]    4'd1       OBERPERIOD2
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB2, (uint8_t)value, 0x0F);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }        
        break;
        
    case SONYSAT_DEMOD_CONFIG_ISDBS_BER_PERIODT:
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        /*     Slave   Bank    Addr    Bit    Default     Signal name
         *    ----------------------------------------------------------
         *    <SLV-T>  C0h     B0h     [2:0]    3'd1       OBERPERIODT
         */
        result = sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB0, (uint8_t)value, 0x07);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }        
        break;

#endif

    default:
        /* Unknown ID */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_I2cRepeaterEnable (sonysat_demod_t * pDemod, uint8_t enable)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_I2cRepeaterEnable");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Using I2C gateway for MTK */
    if (pDemod->i2cRepeaterEnable) {
        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-X>  ALL     08h    [0]      8'h00      8'h01     OREG_REPEN
         */
        if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x08, (uint8_t) (enable ? 0x01 : 0x00), 0x01) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_GPIOSetConfig (sonysat_demod_t * pDemod, 
                                        uint8_t id, 
                                        uint8_t enable, 
                                        sonysat_demod_gpio_mode_t mode)
{
    uint8_t gpioModeSelAddr;
    uint8_t gpioBitSel;

    SONYSAT_TRACE_ENTER ("sonysat_demod_GPIOSetConfig");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (id > 2) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* TS Error only available on GPIO2 */
    if ((mode == SONYSAT_DEMOD_GPIO_MODE_TS_ERROR) && (id != 2)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    gpioModeSelAddr = (uint8_t)(0xA3 + id);
    gpioBitSel = (uint8_t)(1 << id);

    /* For MTK ( No need to change bank here ) */

    /* Slave    Bank    Addr    Bit      Default   Name
     * -----------------------------------------------------------   
     * <SLV-X>  00h     A3h     [3:0]    4'h00     OREG_GPIO0_SEL
     * <SLV-X>  00h     A4h     [3:0]    4'h00     OREG_GPIO1_SEL
     * <SLV-X>  00h     A5h     [3:0]    4'h00     OREG_GPIO2_SEL
     */
    /* For MTK */
    if (sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, gpioModeSelAddr, (uint8_t)mode, 0x0F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    {
        uint8_t enableHiZ;

        if (mode == SONYSAT_DEMOD_GPIO_MODE_INPUT) {
            /* HiZ enabled when pin is GPI */
            enableHiZ = (uint8_t)(0x01 << id);
        }
        else {
            /* HiZ determined by enable parameter */
            enableHiZ = enable ? 0x00 : (uint8_t)(0x01 << id);
        }

        /* Set HiZ setting for selected pin */
        /* Slave    Bank    Addr    Bit      Default    Name            Meaning
         * -----------------------------------------------------------------------------------
         * <SLV-X>  00h     82h     [2:0]    3'b111     OREG_GPIO_HIZ    0: HiZ Off, 1: HiZ On
         */
        /* For MTK */
        if (sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x82, enableHiZ, gpioBitSel) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* Set drivability current for GPIO 2 */
    if (id == 2) {
        uint8_t drivability = 0x00; /* Default 8mA */
        
        /* If GPIO2 is set to TSERR, set to same drivability as TS Valid */
        if (mode == SONYSAT_DEMOD_GPIO_MODE_TS_ERROR) {
            uint8_t data;
            
            /* Set SLV-T Bank : 0x00 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }            
            /* slave    Bank    Addr    Bit    default      Name
             * -----------------------------------------------------------
             * <SLV-T>  00h     83h     [2]    1'b0         OREG_TSVALID_C
             */
            if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, &data, 1) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }        

            if (data & 0x04) {
                /* TS pins set to 10mA */
                drivability = 0x04; 
            }
        }

        /* slave    Bank    Addr    Bit    default     Name
         * --------------------------------------------------------
         * <SLV-X>  00h     85h     [2]    1'b0        OREG_GPIO2_C
         */
        /* For MTK */
        if (sonysat_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x85, drivability, 0x04) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }        
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_GPIORead (sonysat_demod_t * pDemod, 
                                   uint8_t id, 
                                   uint8_t * pValue)
{
    uint8_t rdata = 0x00;

    SONYSAT_TRACE_ENTER ("sonysat_demod_GPIORead");

    if ((!pDemod) || (!pValue)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (id > 2) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Slave    Bank    Addr    Bit      Name
    * ------------------------------------------------------------
    * <SLV-X>   00h     A0h     [2:0]    IREG_GPIO_IN
    */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xA0, &rdata, 1) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pValue = (rdata & (0x01 << id))? 1 : 0;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_GPIOWrite (sonysat_demod_t * pDemod, 
                                    uint8_t id, 
                                    uint8_t value)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_GPIOWrite");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* For MTK ( No need to change bank here ) */

    /* Slave    Bank    Addr    Bit      Default     Name
    * ------------------------------------------------------------
    * <SLV-X>   00h     A2h     [2:0]    3'b000      IREG_GPIO_OUT
    */
    /* For MTK */
    if (sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVX, 0x00, 0xA2, (uint8_t) (value ? (0x01 << id) : 0x00), (0x01 << id)) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_ChipID (sonysat_demod_t * pDemod, 
                                 sonysat_demod_chip_id_t * pChipId)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_ChipID");

    if ((!pDemod) || (!pChipId)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Chip ID is available on both banks, SLV-T register is aligned to legacy devices so check this first,
     * if this fails (due to device being in SHUTDOWN state) read from SLV-X 
     */

    /* Set SLV-T Bank : 0x00 */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);

    if (result == SONYSAT_RESULT_OK) {
        /*SLV-T OK so read SLV-T register */

        /* slave    Bank    Addr    Bit               NAME
         * -----------------------------------------------------------
         * <SLV-T>   00h     FDh     [7:0]            CHIP_ID
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFD, &data, 1) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }
    else if (result == SONYSAT_RESULT_ERROR_I2C) {
        /*SLV-T failed, so try SLV-X */
        /* Set SLV-X Bank : 0x00 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* slave    Bank    Addr    Bit               NAME
         * -----------------------------------------------------------
         * <SLV-X>   00h     FDh     [7:0]            CHIP_ID_SYS
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xFD, &data, 1) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }
    else {
        SONYSAT_TRACE_RETURN (result);
    }

    *pChipId = (sonysat_demod_chip_id_t) data;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE

sonysat_result_t sonysat_demod_terr_cable_monitor_InternalDigitalAGCOut (sonysat_demod_t * pDemod, 
                                                                   uint16_t * pDigitalAGCOut)
{
    uint8_t data[2];
    SONYSAT_TRACE_ENTER ("sonysat_demod_terr_cable_monitor_InternalDigitalAGCOut");

    if ((!pDemod) || (!pDigitalAGCOut)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE_T_C state only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /*  slave     Bank   Addr     Bit           NAME
     * -----------------------------------------------------------
     * <SLV-T>   11h     6Dh     [5:0]        ITDA_DAGC_GAIN[13:8]
     * <SLV-T>   11h     6Eh     [7:0]        ITDA_DAGC_GAIN[7:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6D, &data[0], 2) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pDigitalAGCOut = (uint16_t)(((uint32_t)(data[0] & 0x3F) << 8) | data[1]);

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_terr_cable_monitor_RFAIN (sonysat_demod_t * pDemod, 
                                                   uint16_t * pRFAIN)
{
    uint8_t data[2];
    SONYSAT_TRACE_ENTER ("sonysat_demod_terr_cable_monitor_RFAIN");

    if ((!pDemod) || (!pRFAIN)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE_T_C state only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (!pDemod->enableRfain) {
        /* RFAIN ADC is disabled */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
    }
    
    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit      NAME
     * -----------------------------------------------------------
     * <SLV-T>   00h     50h    [1:0]    IREG_RAMON_ADCDATA[9:8]
     * <SLV-T>   00h     51h    [7:0]    IREG_RAMON_ADCDATA[7:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x50, &data[0], 2) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pRFAIN = (uint16_t)(((uint32_t)(data[0] & 0x03) << 8) + data[1]);

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

#endif

sonysat_result_t sonysat_demod_SetAndSaveRegisterBits (sonysat_demod_t * pDemod, 
                                                 uint8_t slaveAddress, 
                                                 uint8_t bank, 
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_demod_SetAndSaveRegisterBits");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    
    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set the bank */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, slaveAddress, 0x00, bank);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Write the register value */
    result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, slaveAddress, registerAddress, value, bitMask);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Store the updated setting */
    result = setConfigMemory (pDemod, slaveAddress, bank, registerAddress, value, bitMask);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
sonysat_result_t sonysat_demod_terr_cable_SetScanMode (sonysat_demod_t * pDemod, sonysat_dtv_system_t system, uint8_t scanModeEnabled)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_terr_cable_SetScanMode");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C)) {
        /* This api is accepted in SLEEP_T_C and ACTIVE_T_C states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (system == SONYSAT_DTV_SYSTEM_DVBC) {
        /* Set SLV-T Bank : 0x40 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        {
            uint8_t data = scanModeEnabled? 0x20 : 0x00;

            /* slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   40h     86h     [5]      1'b0       1'b1         OREG_AR_SCANNING
             */
            if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, data, 0x20) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }
        }
    }

    pDemod->scanMode = scanModeEnabled;

    SONYSAT_TRACE_RETURN (result);
}
#endif

sonysat_result_t sonysat_demod_SetTsClockModeAndFreq (sonysat_demod_t * pDemod, sonysat_dtv_system_t system)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t serialTs = 0;
    uint8_t backwardsCompatible = 0;
    sonysat_demod_ts_clk_configuration_t tsClkConfiguration;

    const sonysat_demod_ts_clk_configuration_t serialTsClkSettings [2][6] = 
    {{ /* Gated Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSIF                            */
        {      3,          1,            8,             0        }, /* High Freq, full rate */
        {      3,          1,            8,             1        }, /* Mid Freq,  full rate */
        {      3,          1,            8,             2        }, /* Low Freq,  full rate */
        {      0,          2,            16,            0        }, /* High Freq, half rate */
        {      0,          2,            16,            1        }, /* Mid Freq,  half rate */
        {      0,          2,            16,            2        }  /* Low Freq,  half rate */
    },
    {  /* Continuous Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSIF                            */
        {      1,          1,            8,             0        }, /* High Freq, full rate */
        {      1,          1,            8,             1        }, /* Mid Freq,  full rate */
        {      1,          1,            8,             2        }, /* Low Freq,  full rate */
        {      2,          2,            16,            0        }, /* High Freq, half rate */
        {      2,          2,            16,            1        }, /* Mid Freq,  half rate */
        {      2,          2,            16,            2        }  /* Low Freq,  half rate */
    }};

    const sonysat_demod_ts_clk_configuration_t parallelTsClkSetting = 
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSIF */
               0,          0,            8,             0        
    }; 

    const sonysat_demod_ts_clk_configuration_t backwardsCompatibleSerialTsClkSetting [2] = 
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSIF                            */
        {      3,          1,            8,             1        }, /* Gated Clock          */
        {      1,          1,            8,             1        }  /* Continuous Clock     */
    }; 

    const sonysat_demod_ts_clk_configuration_t backwardsCompatibleParallelTsClkSetting = 
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSIF */
               0,          0,            8,             1        
    }; 

    SONYSAT_TRACE_ENTER ("sonysat_demod_SetTsClockModeAndFreq");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &serialTs, 1);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     D3h     [0]      1'b0       OTSRATECTRLOFF
     * <SLV-T>  00h     DEh     [0]      1'b0       OTSIN_OFF
     *
     * (0, 0): Packet gap insertion On  (DVB-T/T2/C/C2/S/S2)
     * (1, 0): Packet gap insertion Off (ISDB-T/S)
     * (1, 1): Packet gap insertion Off (for backwards compatibility)
     *
     *
     * Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     DAh     [0]      1'b0       OTSRC_TSCKMANUALON
     *
     * 0 : Parallel TS clock manual setting off.
     * 1 : Parallel TS clock manual setting on.
     *     (Note: OTSRATECTRLOFF should be 0.)
     */
    {
        uint8_t tsRateCtrlOff = 0;
        uint8_t tsInOff = 0;
        uint8_t tsClkManaulOn = 0;

        if ((system == SONYSAT_DTV_SYSTEM_ISDBT) || (system == SONYSAT_DTV_SYSTEM_ISDBS)) {
            backwardsCompatible = 0;
            tsRateCtrlOff = 1;
            tsInOff = 0;
        } else if (pDemod->isTsBackwardsCompatibleMode) {
            /* DVB only */
            backwardsCompatible = 1;
            tsRateCtrlOff = 1;
            tsInOff = 1;
        } else {
            backwardsCompatible = 0;
            tsRateCtrlOff = 0;
            tsInOff = 0;
        }

        if (!(serialTs & 0x80) && pDemod->parallelTsClkManualSetting) {
            tsClkManaulOn = 1;
            tsRateCtrlOff = 0;
        }

        result = sonysat_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD3, tsRateCtrlOff, 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, tsInOff, 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDA, tsClkManaulOn, 0x01);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

    if (backwardsCompatible) {
        /* Backwards compatible mode */
        if (serialTs & 0x80) {
            /* Serial TS */
            tsClkConfiguration = backwardsCompatibleSerialTsClkSetting[pDemod->serialTsClockModeContinuous];
        }
        else {
            /* Parallel TS */
            tsClkConfiguration = backwardsCompatibleParallelTsClkSetting;

            tsClkConfiguration.tsClkPeriod = (uint8_t)(pDemod->parallelTsClkManualSetting ? pDemod->parallelTsClkManualSetting : 0x08);
        }
    }
    else if (serialTs & 0x80) {
        /* Serial TS */
        switch (system) {
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
            /* Intentional fall through */
            case SONYSAT_DTV_SYSTEM_DVBT:
            case SONYSAT_DTV_SYSTEM_DVBT2:
            case SONYSAT_DTV_SYSTEM_DVBC:
            case SONYSAT_DTV_SYSTEM_DVBC2:
            case SONYSAT_DTV_SYSTEM_ISDBT:
                /* Terrestrial or Cable */
                tsClkConfiguration = serialTsClkSettings[pDemod->serialTsClockModeContinuous][(uint8_t)pDemod->serialTsClkFreqTerrCable];
                break;
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_SAT
            /* Intentional fall through */
            case SONYSAT_DTV_SYSTEM_DVBS:
            case SONYSAT_DTV_SYSTEM_DVBS2:
            case SONYSAT_DTV_SYSTEM_ISDBS:
                /* Satellite */
                tsClkConfiguration = serialTsClkSettings[pDemod->serialTsClockModeContinuous][(uint8_t)pDemod->serialTsClkFreqSat];
                break;
#endif
            default:
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }
    }
    else {
        /* Parallel TS */
        tsClkConfiguration = parallelTsClkSetting;

        tsClkConfiguration.tsClkPeriod = (uint8_t)(pDemod->parallelTsClkManualSetting ? pDemod->parallelTsClkManualSetting : 0x08);
    }

    if (serialTs & 0x80) {
        /* Serial TS, so set serial TS specific registers */        
                
        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------
         * <SLV-T>  00h     C4h     [1:0]  2'b01      OSERCKMODE
         */
        result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, tsClkConfiguration.serialClkMode, 0x03);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -------------------------------------------------------
         * <SLV-T>  00h     D1h     [1:0]  2'b01      OSERDUTYMODE
         */
        result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD1, tsClkConfiguration.serialDutyMode, 0x03);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ------------------------------------------------------
     * <SLV-T>  00h     D9h     [7:0]  8'h08      OTSCKPERIOD
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, tsClkConfiguration.tsClkPeriod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Disable TS IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSIF_EN
     */
    result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x00, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     33h     [1:0]  2'b01      OREG_CKSEL_TSIF
     */
    result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, tsClkConfiguration.clkSelTsIf, 0x03);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Enable TS IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSIF_EN
     */
    result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x01, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    {
        /* Set parity period enable / disable based on backwards compatible TS configuration */
        uint8_t data = backwardsCompatible? 0x00 : 0x01;

        if (system == SONYSAT_DTV_SYSTEM_DVBT) {
            /* Enable parity period for DVB-T */
            /* Set SLV-T Bank : 0x10 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* slave    Bank    Addr    Bit    default    Name
             * ---------------------------------------------------------------
             * <SLV-T>  10h     66h     [0]    1'b0       OREG_TSIF_PCK_LENGTH (For 2k12 Generation Demod Chips)
             * <SLV-T>  10h     66h     [0]    1'b1       OREG_TSIF_PCK_LENGTH (For 2k14 Generation Demod Chips)
             */
            result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x66, data, 0x01);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }

        if (system == SONYSAT_DTV_SYSTEM_DVBC) {
            /* Enable parity period for DVB-C */
            /* Set SLV-T Bank : 0x40 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* slave    Bank    Addr    Bit    default    Name
             * ---------------------------------------------------------------
             * <SLV-T>  40h     66h     [0]    1'b0       OREG_TSIF_PCK_LENGTH (For 2k12 Generation Demod Chips)
             * <SLV-T>  40h     66h     [0]    1'b1       OREG_TSIF_PCK_LENGTH (For 2k14 Generation Demod Chips)
             */
            result = sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x66, data, 0x01);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_DisableTsDataPinHiZ (sonysat_demod_t * pDemod)
{
    uint8_t data = 0;
    uint8_t tsDataHiZ = 0;

    SONYSAT_TRACE_ENTER ("sonysat_demod_DisableTsDataPinHiZ");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
     * <SLV-T>  00h     C4h     [3]    1'b1       OSEREXCHGB7
     */

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &data, 1) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    switch (data & 0x88) {
    case 0x80:
        /* Serial TS, output from TSDATA0 */
        tsDataHiZ = 0xFE;
        break;
    case 0x88:
        /* Serial TS, output from TSDATA7 */
        tsDataHiZ = 0x7F;
        break;
    case 0x08:
    case 0x00:
    default:
        /* Parallel TS */
        tsDataHiZ = 0x00;
        break;
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>   00h    81h    [7:0]    8'hFF   OREG_TSDATA_HIZ
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, tsDataHiZ) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t SDtoSLT(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SDtoSLT");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Enable oscillator */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (3);

    /* Select ADC clock mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x13, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* Select demod frequency */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x12, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    {
        uint8_t data = 0x00;

        /* Select X'tal clock mode*/
        if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) {
            data = 0x00;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
            data = 0x03;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz) {
            data = 0x01;
        }
        else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
        
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, data) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (SONYSAT_DEMOD_CHIP_ID_SUPPORT_SAT (pDemod->chipId)) {
        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t SDtoSLS(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SDtoSLS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Enable oscillator */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (3);

    if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* Select demod frequency */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x12, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    {
        uint8_t data = 0x00;

        /* Select X'tal clock mode*/
        if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) {
            data = 0x00;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
            data = 0x03;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz) {
            data = 0x01;
        }
        else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
       
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, data) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQOUT */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQIN */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x40) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (SONYSAT_DEMOD_CHIP_ID_SUPPORT_TERR_OR_CABLE (pDemod->chipId)) {
        /* TADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* TADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t SLTtoSD(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SLTtoSD");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    /* Disable oscillator */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t SLStoSD(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SLStoSD");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Disable DSQOUT */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x3F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Disable DSQIN */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    /* Disable oscillator */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t SLStoSLT(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SLStoSLT");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    /* Select ADC clock mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x13, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t SLTtoSLS(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("SLTtoSLS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQOUT */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQIN */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x40) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t XtoSLT(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("XtoSLT");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Clear all demodulator registers */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x02, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (3);

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Select ADC clock mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x13, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* Select demod frequency */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x12, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    {
        uint8_t data = 0x00;

        /* Select X'tal clock mode*/
        if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) {
            data = 0x00;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
            data = 0x03;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz) {
            data = 0x01;
        }
        else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
        
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, data) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* TADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (SONYSAT_DEMOD_CHIP_ID_SUPPORT_SAT (pDemod->chipId)) {
        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* SADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t XtoSLS(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("XtoSLS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Clear all demodulator registers */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x02, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    SONYSAT_SLEEP (3);

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
        /* Select demod frequency */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x12, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    {
        uint8_t data = 0x00;

        /* Select X'tal clock mode*/
        if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_20500KHz) {
            data = 0x00;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_24000KHz) {
            data = 0x03;
        }
        else if (pDemod->xtalFreq == SONYSAT_DEMOD_XTAL_41000KHz) {
            data = 0x01;
        }
        else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        }
        
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, data) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x0A) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Clear demod SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_SLEEP (1);

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQOUT */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* enable DSQIN */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x40) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (SONYSAT_DEMOD_CHIP_ID_SUPPORT_TERR_OR_CABLE (pDemod->chipId)) {
        /* TADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* TADC Bias On */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* SADC Bias On */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t clearConfigMemory (sonysat_demod_t * pDemod)
{
    SONYSAT_TRACE_ENTER ("clearConfigMemory");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pDemod->configMemoryLastEntry = 0;
    sonysat_memset(pDemod->configMemory, 0x00, sizeof(pDemod->configMemory));

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t loadConfigMemory (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t i;

    SONYSAT_TRACE_ENTER ("loadConfigMemory");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for (i = 0; i < pDemod->configMemoryLastEntry; i++) {
        /* Set the bank */
        result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c,
                                                 pDemod->configMemory[i].slaveAddress,
                                                 0x00,
                                                 pDemod->configMemory[i].bank);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        /* Write the register value */
        result = sonysat_i2c_SetRegisterBits (pDemod->pI2c,
                                           pDemod->configMemory[i].slaveAddress,
                                           pDemod->configMemory[i].registerAddress,
                                           pDemod->configMemory[i].value,
                                           pDemod->configMemory[i].bitMask);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t setConfigMemory (sonysat_demod_t * pDemod, 
                                                 uint8_t slaveAddress, 
                                                 uint8_t bank, 
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask)
{
    uint8_t i;
    uint8_t valueStored = 0;

    SONYSAT_TRACE_ENTER ("setConfigMemory");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Search for matching address entry already in table */
    for (i = 0; i < pDemod->configMemoryLastEntry; i++){
        if ((valueStored == 0) &&
            (pDemod->configMemory[i].slaveAddress == slaveAddress) &&
            (pDemod->configMemory[i].bank == bank) &&
            (pDemod->configMemory[i].registerAddress == registerAddress)) {
                /* Clear bits to overwrite / set  and then store the new value */
                pDemod->configMemory[i].value &= ~bitMask;
                pDemod->configMemory[i].value |= (value & bitMask);

                /* Add new bits to the bit mask */
                pDemod->configMemory[i].bitMask |= bitMask;

                valueStored = 1;
        }
    }
    
    /* If current register does not exist in the table, add a new entry to the end */
    if (valueStored == 0) {
        if (pDemod->configMemoryLastEntry < SONYSAT_DEMOD_MAX_CONFIG_MEMORY_COUNT) {
            pDemod->configMemory[pDemod->configMemoryLastEntry].slaveAddress = slaveAddress;
            pDemod->configMemory[pDemod->configMemoryLastEntry].bank = bank;
            pDemod->configMemory[pDemod->configMemoryLastEntry].registerAddress = registerAddress;
            pDemod->configMemory[pDemod->configMemoryLastEntry].value = (value & bitMask);
            pDemod->configMemory[pDemod->configMemoryLastEntry].bitMask = bitMask;
            pDemod->configMemoryLastEntry++;
        }
        else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_OVERFLOW);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
