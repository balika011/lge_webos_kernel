/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command.h

          This file provides definitions for DiSEqC command.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_H
#define SONYSAT_DISEQC_COMMAND_H

#include "sonysat_demod_sat_device_ctrl.h"

/**
 @brief Definition of framing byte for DiSEqC message.
*/
typedef enum sonysat_diseqc_framing_t {
    SONYSAT_DISEQC_FRAMING_MASTER_NO_REPLY_FIRST        = 0xE0, /**< Command from master, no reply required. (first transmission) */
    SONYSAT_DISEQC_FRAMING_MASTER_NO_REPLY_REPEATED     = 0xE1, /**< Command from master, no reply required. (repeated transmission) */
    SONYSAT_DISEQC_FRAMING_MASTER_REPLY_FIRST           = 0xE2, /**< Command from master, reply required. (first transmission) */
    SONYSAT_DISEQC_FRAMING_MASTER_REPLY_REPEATED        = 0xE3, /**< Command from master, reply required. (repeated transmission) */
    SONYSAT_DISEQC_FRAMING_SLAVE_REPLY_OK               = 0xE4, /**< Reply from slave, command OK, no errors detected.  */
    SONYSAT_DISEQC_FRAMING_SLAVE_REPLY_CMD_UNSUPPORTED  = 0xE5, /**< Reply from slave, command not supported. */
    SONYSAT_DISEQC_FRAMING_SLAVE_REPLY_ERR_PARITY       = 0xE6, /**< Reply from slave, parity error detected. */
    SONYSAT_DISEQC_FRAMING_SLAVE_REPLY_CMD_UNKNOWN      = 0xE7  /**< Reply from slave, command not recoginised/unknown. Request repeat. */
} sonysat_diseqc_framing_t;

/**
 @brief Definition of address byte for DiSEqC message.
*/
typedef enum sonysat_diseqc_address_t {
    SONYSAT_DISEQC_ADDRESS_ALL_DEVICES                       = 0x00, /**< Any device (Master to all devices) */
    SONYSAT_DISEQC_ADDRESS_ALL_LNB_SMATV_SWITCHER            = 0x10, /**< Any LNB, Switcher or SMATV (Master to all...) */
    SONYSAT_DISEQC_ADDRESS_LNB_DEVICE                        = 0x11, /**< LNB devices. */
    SONYSAT_DISEQC_ADDRESS_LNB_WITH_LOOP_THROUGH             = 0x12, /**< LNB with loop-through switching. */
    SONYSAT_DISEQC_ADDRESS_SWITCHER_DEVICE_DC_BLK            = 0x14, /**< Switcher (inc. DC blocking) */
    SONYSAT_DISEQC_ADDRESS_SWITCHER_DEVICE_DC_LOOP           = 0x15, /**< Switcher with DC loop through. */
    SONYSAT_DISEQC_ADDRESS_SMATV_DEVICE                      = 0x18, /**< SMATV devices. */
    SONYSAT_DISEQC_ADDRESS_ANY_POLARISER                     = 0x20, /**< Any Polariser (Master to all Polarisers) */
    SONYSAT_DISEQC_ADDRESS_LIN_POL_CONTROLLER                = 0x21, /**< Linear Polarisation Controller. */
    SONYSAT_DISEQC_ADDRESS_ANY_POSITIONER                    = 0x30, /**< Any Positioner (Master to all Positioners) */
    SONYSAT_DISEQC_ADDRESS_POLAR_POSITIONER                  = 0x31, /**< Polar/Azimuth Positioner. */
    SONYSAT_DISEQC_ADDRESS_ELEVATION_POSITIONER              = 0x32, /**< Elevation Positioner. */
    SONYSAT_DISEQC_ADDRESS_ANY_INSTALLER_AID                 = 0x40, /**< Any Installer Aid (Master to all Installer Aids). */
    SONYSAT_DISEQC_ADDRESS_SIGNAL_STRENGTH_ANALOGUE          = 0x41, /**< Signal strength 0 Analogue value. */
    SONYSAT_DISEQC_ADDRESS_ANY_INTELLI_SLAVES                = 0x70, /**< Any Intelligent Slave Interfaces (Master to all) */
    SONYSAT_DISEQC_ADDRESS_SUBSCRIBER_CONTROLLED_HEADENDS    = 0x71  /**< Interface for subscriber controlled headends. */
} sonysat_diseqc_address_t;

#endif /* SONYSAT_DISEQC_COMMAND_H */
