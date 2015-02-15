/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sony_dtv.h

          This file provides DTV system specific definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DTV_H
#define SONY_DTV_H

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief System (DVB-T/T2/C/C2/S/S2 and ISDB-T/S) 
*/
typedef enum {
    SONY_DTV_SYSTEM_UNKNOWN,        /**< Unknown. */
    SONY_DTV_SYSTEM_DVBT,           /**< DVB-T. */
    SONY_DTV_SYSTEM_DVBT2,          /**< DVB-T2. */
    SONY_DTV_SYSTEM_DVBC,           /**< DVB-C. */
    SONY_DTV_SYSTEM_DVBC2,          /**< DVB-C2. */
    SONY_DTV_SYSTEM_DVBS,           /**< DVB-S. */
    SONY_DTV_SYSTEM_DVBS2,          /**< DVB-S2. */
    SONY_DTV_SYSTEM_ISDBT,          /**< ISDB-T. */
    SONY_DTV_SYSTEM_ISDBS,          /**< ISDB-S. */
    SONY_DTV_SYSTEM_ANY             /**< Used for multiple system scanning / blind tuning */
} sony_dtv_system_t;

/**
 @brief System bandwidth.
*/
typedef enum {
    SONY_DTV_BW_UNKNOWN = 0,          /**< Unknown bandwidth. */
    SONY_DTV_BW_1_7_MHZ = 1,          /**< 1.7MHz bandwidth. */
    SONY_DTV_BW_5_MHZ = 5,            /**< 5MHz bandwidth. */
    SONY_DTV_BW_6_MHZ = 6,            /**< 6MHz bandwidth. */
    SONY_DTV_BW_7_MHZ = 7,            /**< 7MHz bandwidth. */
    SONY_DTV_BW_8_MHZ = 8             /**< 8MHz bandwidth. */
} sony_dtv_bandwidth_t;

#endif /* SONY_DTV_H */
