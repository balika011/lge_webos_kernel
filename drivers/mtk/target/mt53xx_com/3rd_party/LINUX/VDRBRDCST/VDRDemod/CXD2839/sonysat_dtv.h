/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_dtv.h

          This file provides DTV system specific definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONYSAT_DTV_H
#define SONYSAT_DTV_H

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief System (DVB-T/T2/C/C2/S/S2) 
*/
typedef enum {
    SONYSAT_DTV_SYSTEM_UNKNOWN,        /**< Unknown. */
    SONYSAT_DTV_SYSTEM_DVBT,           /**< DVB-T. */
    SONYSAT_DTV_SYSTEM_DVBT2,          /**< DVB-T2. */
    SONYSAT_DTV_SYSTEM_DVBC,           /**< DVB-C. */
    SONYSAT_DTV_SYSTEM_DVBC2,          /**< DVB-C2. */
    SONYSAT_DTV_SYSTEM_DVBS,           /**< DVB-S. */
    SONYSAT_DTV_SYSTEM_DVBS2,          /**< DVB-S2. */
    SONYSAT_DTV_SYSTEM_ANY             /**< Used for multiple system scanning / blind tuning */
} sonysat_dtv_system_t;

#endif /* SONYSAT_DTV_H */
