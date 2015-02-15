/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef HCI_CONFIG_H
#define HCI_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Enable/disable parts of the HCI API based on
     *        needed Nautilus component functionality.
     *
     * Depending on the needs, enable the functionality by setting to 1.
     * This activates the supporting HCI API functions and
     * their underlying HCI implementation.
     *
     * The reason for minimizing functionality is to limit the porting efforts
     * of HCI functionality that's necessary to support the required functionality.
     * The HCI functionality is fine grained up to their API functions.
     *
     * Include this file right before other other HCI related headers
     */

    /* Main components needing HCI support */

#define HCI_L_SERV_DRM      1  /**< The license processing*/
#define HCI_L_SERV_NEMO     1  /**< Nemo*/
#define HCI_L_SERV_PPPROC   1  /**< Personalization Protocol service*/
#define HCI_L_SERV_PERS     1  /**< Personalization Protocol service*/
#define HCI_L_SERV_BBTS     1  /**< The BBTS service*/
#define HCI_L_SERV_IMPORT   1  /**< Marlin Import*/
#define HCI_L_SERV_MS3      1  /**< The MS3 (Marlin Simple Secure Streaming)*/
#define HCI_L_FF_BBTS_SKL   1  /**< File Format Broadband Transport Stream - Single Key Layer*/
#define HCI_L_FF_IPMP       1  /**< File Format IPMP*/

#include "hci_config_options.h"

#ifdef __cplusplus
}
#endif

#endif /* #ifndef HCI_CONFIG_H */
