/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_integ.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_integ_Create (sonysat_integ_t * pInteg,
                                 sonysat_demod_xtal_t xtalFreq,
                                 uint8_t i2cAddress,
                                 sonysat_i2c_t * pDemodI2c,
                                 sonysat_demod_t * pDemod 
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
                                 ,sonysat_tuner_terr_cable_t * pTunerTerrCable
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
                                 ,sonysat_tuner_sat_t * pTunerSat,
                                 sonysat_lnbc_t * pLnbc
#endif
                                 )
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_Create");

    if ((!pInteg) || (!pDemodI2c) || (!pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Create demodulator instance */
    result = sonysat_demod_Create (pDemod, xtalFreq, i2cAddress, pDemodI2c);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Populate the integration structure */
    pInteg->pDemod = pDemod;

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    pInteg->pTunerTerrCable = pTunerTerrCable;
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    pInteg->pTunerSat = pTunerSat;
    pInteg->pLnbc = pLnbc;
#endif

    sonysat_atomic_set (&(pInteg->cancel), 0);

    SONYSAT_TRACE_RETURN (result);
}

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
sonysat_result_t sonysat_integ_InitializeT_C (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_InitializeT_C");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    
    result = sonysat_demod_InitializeT_C (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Enable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Initialize)) {
        /* Initialize the terrestrial / cable tuner. */
        result = pInteg->pTunerTerrCable->Initialize (pInteg->pTunerTerrCable);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Initialize)) {
        /* Initialize the satellite tuner. */
        result = pInteg->pTunerSat->Initialize (pInteg->pTunerSat);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Disable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Initialize)) {
        /* Initialize the lnb controller. */
        result = pInteg->pLnbc->Initialize (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Sleep)) {
        /* Sleep the lnb controller */
        result = pInteg->pLnbc->Sleep (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

    SONYSAT_TRACE_RETURN (result);
}
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
sonysat_result_t sonysat_integ_InitializeS (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_InitializeS");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_demod_InitializeS (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Enable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Initialize)) {
        /* Initialize the terrestrial / cable tuner. */
        result = pInteg->pTunerTerrCable->Initialize (pInteg->pTunerTerrCable);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Initialize)) {
        /* Initialize the satellite tuner. */
        result = pInteg->pTunerSat->Initialize (pInteg->pTunerSat);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Disable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    if ((pInteg->pLnbc) && (pInteg->pLnbc->Initialize)) {
        /* Initialize the lnb controller. */
        result = pInteg->pLnbc->Initialize (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
sonysat_result_t sonysat_integ_SleepT_C (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_SleepT_C");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Enable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Sleep)) {
        /* Call the terrestrial / cable tuner Sleep implementation */
        result = pInteg->pTunerTerrCable->Sleep (pInteg->pTunerTerrCable);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Sleep)) {
        /* Call the satellite tuner Sleep implementation */
        result = pInteg->pTunerSat->Sleep (pInteg->pTunerSat);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Disable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Sleep)) {
        /* Sleep the lnb controller. */
        result = pInteg->pLnbc->Sleep (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

    /* Call the demodulator Sleep function for DVB-C / C2 / T / T2 */
    result = sonysat_demod_SleepT_C (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
sonysat_result_t sonysat_integ_SleepS (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_SleepS");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Enable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Sleep)) {
        /* Call the terrestrial / cable tuner Sleep implementation */
        result = pInteg->pTunerTerrCable->Sleep (pInteg->pTunerTerrCable);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Sleep)) {
        /* Call the satellite tuner Sleep implementation */
        result = pInteg->pTunerSat->Sleep (pInteg->pTunerSat);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Disable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    if ((pInteg->pLnbc) && (pInteg->pLnbc->WakeUp)) {
        /* Call the lnb controller WakeUp implementation. */
        result = pInteg->pLnbc->WakeUp (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }

    /* Call the demodulator Sleep function for DVB-S / S2 */
    result = sonysat_demod_SleepS (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}
#endif

sonysat_result_t sonysat_integ_Shutdown (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_integ_Shutdown");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in SLEEP, ACTIVE and SHUTDOWN states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Enable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Shutdown)) {
        /* Call the terrestrial / cable tuner Shutdown implementation */
        result = pInteg->pTunerTerrCable->Shutdown (pInteg->pTunerTerrCable);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Shutdown)) {
        /* Call the satellite tuner Shutdown implementation */
        result = pInteg->pTunerSat->Shutdown (pInteg->pTunerSat);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    /* Disable the I2C repeater */
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Sleep)) {
        /* Sleep the lnb controller. */
        result = pInteg->pLnbc->Sleep (pInteg->pLnbc);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
    }
#endif

    /* Call the demodulator Shutdown function */
    result = sonysat_demod_Shutdown (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
    
    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_Cancel (sonysat_integ_t * pInteg)
{
    SONYSAT_TRACE_ENTER ("sonysat_integ_Cancel");

    /* Argument verification. */
    if ((!pInteg)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set the cancellation flag. */
    sonysat_atomic_set (&(pInteg->cancel), 1);

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
