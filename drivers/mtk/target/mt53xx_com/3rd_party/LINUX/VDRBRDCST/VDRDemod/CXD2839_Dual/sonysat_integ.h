/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_integ.h

          This file provides the integration layer control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONYSAT_INTEG_H
#define SONYSAT_INTEG_H

#include "sonysat_demod.h"

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
#include "sonysat_tuner_sat.h"
#include "sonysat_lnbc.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
#include "sonysat_tuner_terr_cable.h"
#endif

#include "sonysat_common.h"

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The high level driver object.
        This is the primary interface used for controlling the demodulator and 
        connected tuner devices.

        This object is the combination of the demodulator and the tuner devices
        to represent a single front end system. It can be used wholly, partly or 
        just as a reference for developing the application software.
*/
typedef struct sonysat_integ_t {
    sonysat_demod_t * pDemod;                          /**< Instance of the demodulator. */
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    sonysat_tuner_terr_cable_t * pTunerTerrCable;      /**< The connected tuner for terrestrial and cable systems. */
#endif
#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    sonysat_tuner_sat_t * pTunerSat;                   /**< The connected tuner satellite . */
    sonysat_lnbc_t * pLnbc;                            /**< The connected lnb controller. */
#endif

    sonysat_atomic_t cancel;                           /**< Cancellation indicator variable. */

    void * user;                                    /**< User data. */
    SAT_STOPWATCH_ELAPSED_FCT pfStopwatch_elapsed;  /*MTK_Dual_Tuner: Add for multi pd glue layer*/
} sonysat_integ_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
#if defined SONYSAT_DRIVER_BUILD_OPTION_CXD2839 /* S/S2 only device */
/**
 @brief Construct the driver.

        This function is called by the application in order to setup the 
        ::sonysat_demod_t structure and provide references for the 
        ::sonysat_integ_t::pDemod, ::sonysat_integ_t::pTunerSat and 
        ::sonysat_integ_t::pLnbc members.

        This MUST be called before calling ::sonysat_integ_InitializeS.

@note Passing NULL as pTunerSat or pLnbc will disable communication
      with the device without throwing an arguement error.

        Notes on driver configuration:
        - By default, the demodulator uses an inverted AGC.  If the connected 
          tuner uses a non-inverted IFAGC, call ::sonysat_demod_SetConfig with 
          ::SONYSAT_DEMOD_CONFIG_IFAGCNEG = 0 to setup the demodulator with postive 
          IFAGC sense, after calling ::sonysat_integ_InitializeS.
        - By default, the driver disables the RFAIN ADC and monitor on the device, 
          call ::sonysat_demod_SetConfig with ::SONYSAT_DEMOD_CONFIG_RFAIN_ENABLE = 1
          to enable it, after calling ::sonysat_integ_InitializeS.

 @note  Memory is not allocated dynamically.

 @param pInteg The driver object to create. It must be a valid pointer to 
        allocated memory for a ::sonysat_integ_t structure.
 @param xtalFreq The frequency of the demod crystal.
 @param i2cAddress The demod I2C address in 8-bit form.
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.
 @param pDemod Reference to memory allocated for the demodulator instance. The 
        create function will setup this demodulator instance also.
 @param pTunerSat The tuner driver to use with this instance of the driver for 
        DVB-S / S2 systems. Note : The tuner I2C interface should have been setup 
        before this call.
 @param pLnbc The LNB control interface.
 
 @return SONYSAT_RESULT_OK if successfully created integration layer driver structure.
*/
#elif defined SONYSAT_DEMOD_SUPPORT_DVBS_S2 /* Devices supporting Terrestrial or Cable and Satellite systems */
/**
 @brief Construct the driver.

        This function is called by the application in order to setup the 
        ::sonysat_demod_t structure and provide references for the 
        ::sonysat_integ_t::pDemod, ::sonysat_integ_t::pTunerTerrCable,  
        ::sonysat_integ_t::pTunerSat and ::sonysat_integ_t::pLnbc members.

        This MUST be called before calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.

@note Passing NULL as pTunerTerrCable, pTunerSat or pLnbc will disable communication
      with the device without throwing an arguement error.

        Notes on driver configuration:
        - By default, the demodulator uses an inverted AGC.  If the connected 
          tuner uses a non-inverted IFAGC, call ::sonysat_demod_SetConfig with 
          ::SONYSAT_DEMOD_CONFIG_IFAGCNEG = 0 to setup the demodulator with postive 
          IFAGC sense, after calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.
        - By default, the driver disables the RFAIN ADC and monitor on the device, 
          call ::sonysat_demod_SetConfig with ::SONYSAT_DEMOD_CONFIG_RFAIN_ENABLE = 1
          to enable it, after calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.

 @note  Memory is not allocated dynamically.

 @param pInteg The driver object to create. It must be a valid pointer to 
        allocated memory for a ::sonysat_integ_t structure.
 @param xtalFreq The frequency of the demod crystal.
 @param i2cAddress The demod I2C address in 8-bit form.
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.
 @param pDemod Reference to memory allocated for the demodulator instance. The 
        create function will setup this demodulator instance also.
 @param pTunerTerrCable The tuner driver to use with this instance of the driver 
        for DVB-T / T2 / C / C2 systems. Note : The tuner I2C interface should 
        have been setup before this call.
 @param pTunerSat The tuner driver to use with this instance of the driver for 
        DVB-S / S2 systems. Note : The tuner I2C interface should have been setup 
        before this call.
 @param pLnbc The LNB control interface.
 
 @return SONYSAT_RESULT_OK if successfully created integration layer driver structure.
*/
#else /* Devices not supporting satellite system */
/**
 @brief Construct the driver.

        This function is called by the application in order to setup the 
        ::sonysat_demod_t structure and provide references for the 
        ::sonysat_integ_t::pDemod and ::sonysat_integ_t::pTunerTerrCable members.

        This MUST be called before calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.

@note Passing NULL as pTunerTerrCable will disable communication
      with the device without throwing an arguement error.

        Notes on driver configuration:
        - By default, the demodulator uses an inverted AGC.  If the connected 
          tuner uses a non-inverted IFAGC, call ::sonysat_demod_SetConfig with 
          ::SONYSAT_DEMOD_CONFIG_IFAGCNEG = 0 to setup the demodulator with postive 
          IFAGC sense, after calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.
        - By default, the driver disables the RFAIN ADC and monitor on the device, 
          call ::sonysat_demod_SetConfig with ::SONYSAT_DEMOD_CONFIG_RFAIN_ENABLE = 1
          to enable it, after calling ::sonysat_integ_InitializeS or 
          ::sonysat_integ_InitializeT_C.

 @note  Memory is not allocated dynamically.

 @param pInteg The driver object to create. It must be a valid pointer to 
        allocated memory for a ::sonysat_integ_t structure.
 @param xtalFreq The frequency of the demod crystal.
 @param i2cAddress The demod I2C address in 8-bit form.
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.
 @param pDemod Reference to memory allocated for the demodulator instance. The 
        create function will setup this demodulator instance also.
 @param pTunerTerrCable The tuner driver to use with this instance of the driver 
        for DVB-T / T2 / C / C2 systems. Note : The tuner I2C interface should 
        have been setup before this call.
 
 @return SONYSAT_RESULT_OK if successfully created integration layer driver structure.
*/
#endif
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
                                 );

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Initialize the demodulator and terrestrial / cable tuner.  After this 
        operation, the demodulator and tuner are in a low power state (demod 
        state = ::SONYSAT_DEMOD_STATE_SLEEP_T_C) awaiting tune or scan commands.

        Should only be used from Power On (::SONYSAT_DEMOD_STATE_UNKNOWN).  Calling 
        from Active or Shutdown states will cause a demodulator reset, clearing
        all current configuration settings.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if OK.
*/
sonysat_result_t sonysat_integ_InitializeT_C (sonysat_integ_t * pInteg);
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
/**
 @brief Initialize the demodulator and satellite tuner.  After this 
        operation, the demodulator and tuner are in a low power state (demod 
        state = ::SONYSAT_DEMOD_STATE_SLEEP_S) awaiting tune or scan commands.

        Should only be used from Power On (::SONYSAT_DEMOD_STATE_UNKNOWN).  Calling 
        from Active or Shutdown states will cause a demodulator reset, clearing
        all current configuration settings.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if OK.
*/
sonysat_result_t sonysat_integ_InitializeS (sonysat_integ_t * pInteg);
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Put the demodulator and tuner devices into a low power state for DVB-T, T2
        C and C2 systems.  This function can be called from SHUTDOWN or ACTIVE states.
        In ::SONYSAT_DEMOD_STATE_SLEEP_T_C GPIO and demodulator configuration options are 
        available.  Calling this function from ::SONYSAT_DEMOD_STATE_SLEEP_S will 
        reconfigure the demodulator for use with DVB terrestrial or cable systems.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if OK.
*/
sonysat_result_t sonysat_integ_SleepT_C (sonysat_integ_t * pInteg);
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
/**
 @brief Put the demodulator and tuner devices into a low power state for DVB-S and 
        S2 systems.  This function can be called from SHUTDOWN or ACTIVE states.
        In ::SONYSAT_DEMOD_STATE_SLEEP_S GPIO, demodulator configuration options and
        DiSEqC contol are available.  Calling this function from 
        ::SONYSAT_DEMOD_STATE_SLEEP_T_C will reconfigure the demodulator for use with
        DVB satellite systems.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if OK.
*/
sonysat_result_t sonysat_integ_SleepS (sonysat_integ_t * pInteg);
#endif

/**
 @brief Shutdown the demodulator and tuner parts into a low power disabled state.
        This state can be used to manually transition between DVB-S / S2 and 
        DVB-T /T2 / C / C2 operating modes.  Call ::sonysat_integ_SleepS (DVB-S / S2)
        or ::sonysat_integ_SleepT_C (DVB-T /T2 / C / C2) dependant on the system 
        requirements of the applicaiton.  ::sonysat_integ_Shutdown can be directly 
        called from SLEEP or ACTIVE states.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if OK.
*/
sonysat_result_t sonysat_integ_Shutdown (sonysat_integ_t * pInteg);

/**
 @brief Cancels current Tune or Scan operation in the demod and tuner parts.
        This function is thread safe, calling thread will get the result
        SONYSAT_RESULT_ERROR_CANCEL.

 @param pInteg The driver instance.

 @return SONYSAT_RESULT_OK if able to cancel the pending operation.
*/
sonysat_result_t sonysat_integ_Cancel (sonysat_integ_t * pInteg);

#endif /* SONYSAT_INTEG_H */
