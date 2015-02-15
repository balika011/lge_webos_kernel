//***************************************************************************
//!file     si_drv_tx_simple.h
//!brief    SiI9687 Tx Simple Driver API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRVTXSIMPLE_INTERNAL_H__
#define __SI_DRVTXSIMPLE_INTERNAL_H__
#include "si_drv_internal.h"

//------------------------------------------------------------------------------
//  Manifest Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Tx Simple Driver Instance Data
//------------------------------------------------------------------------------

typedef struct
{
    int         structVersion;
    int         lastResultCode;         // Contains the result of the last API function called
    uint16_t    statusFlags;

}   TxSimpleInstanceData_t;


void SiiDrvTxSimpleProcessInterrupts( void );

#endif  // __SI_DRVTXSIMPLE_INTERNAL_H__

