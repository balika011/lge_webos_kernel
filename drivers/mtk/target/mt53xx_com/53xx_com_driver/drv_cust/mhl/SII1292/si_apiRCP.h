//***************************************************************************

//!file     si_apiRCP.h
//!brief    SiI1292 RCP functions head file.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_APIRCP_H__
#define __SI_APIRCP_H__

#include "si_cp1292.h"
#include "si_hal.h"

//------------------------------------------------------------------------------
// Module variables
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t cecCommand;         //!< rc protocol command code
    uint8_t rcpKeyCode;          //!< RCP CBUS Key Code
    char    rcpName[15];
} SI_CecRcpConversion_t;

#define RCPE_INEFFECTIVE_KEY_CODE 0x01
#define RCPE_RESPONDER_BUSY 0x02


#define RCP_KEYCODE_NUM 0x80

bool_t             SI_IrRcpKeys(uint8_t key, uint8_t eventType);
uint8_t            SI_RcpAckRcvd(uint8_t channel, uint8_t KeyID);
uint8_t            SI_RcpErrRcvd(uint8_t channel, uint8_t statusCode);
uint8_t            SI_RcpTimeoutHandler(uint8_t channel, uint8_t direction);
uint8_t            SI_RcpTransferDoneHandler(uint8_t channel);

void 		SI_RcpCecRetHandler(uint8_t channel, bool_t result);
uint8_t            SI_RcpHandler(uint8_t channel);

#endif  // __SI_APIRCP_H__
