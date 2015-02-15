/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    mps_mp8126.h

          This file provides the functions to control MP8126(MPS).
*/
/*----------------------------------------------------------------------------*/
#ifndef MPS_MP8126_H
#define MPS_MP8126_H

#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "sonysat_demod.h"

/**
 @brief Use 13V for low level output voltage.

 If you would like to use 13V as "Low level voltage",
 please set this parameter by sonysat_lnbc_t::SetConfig(SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE, value).
*/
#define MPS_MP8126_CONFIG_VOLTAGE_LOW_13V       0

/**
 @brief Use 14V for low level output voltage.

 If you would like to use 14V as "Low level voltage",
 please set this parameter by sonysat_lnbc_t::SetConfig(SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE, value).
*/
#define MPS_MP8126_CONFIG_VOLTAGE_LOW_14V       1

/**
 @brief Use 18V for high level output voltage.

 If you would like to use 18V as "High level voltage",
 please set this parameter by sonysat_lnbc_t::SetConfig(SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE, value).
*/
#define MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V      10

/**
 @brief Use 19V for high level output voltage.

 If you would like to use 19V as "High level voltage",
 please set this parameter by sonysat_lnbc_t::SetConfig(SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE, value).
*/
#define MPS_MP8126_CONFIG_VOLTAGE_HIGH_19V      11

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief structure for MPS MP8126 LNBC.
*/
typedef struct mps_mp8126_t {
    sonysat_demod_t * pDemod; /**< Instance of the demodulator. */
    uint8_t voltagePinId;  /**< Demodulator GPIO ID connected to LNBC 13V/18V pin. */
    uint8_t linedropPinId; /**< Demodulator GPIO ID connected to LNBC LINEDROP pin. */
    uint8_t enablePinId;   /**< Demodulator GPIO ID connected to LNBC EN pin. */
} mps_mp8126_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create a instance for MP8126(MPS).

 @param pLnbc LNB controller driver instance.
 @param pDemod Demodulator driver instance.
 @param voltagePinId Demodulator GPIO ID to control LNBC 13V/18V pin.
 @param linedropPinId Demodulator GPIO ID to control LNBC LINEDROP pin.
 @param enablePinId Demodulator GPIO ID to control LNBC EN pin.
 @param pMP8126 MPS MP8126 LNBC (mps_mp8126_t) instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t mps_mp8126_Create (sonysat_lnbc_t * pLnbc,
                                 sonysat_demod_t * pDemod,
                                 uint8_t voltagePinId,
                                 uint8_t linedropPinId,
                                 uint8_t enablePinId,
                                 mps_mp8126_t * pMP8126);

#endif /* MPS_MP8126_H */
