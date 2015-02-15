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

#ifndef __SI_APIRAP_H__
#define __SI_APIRAP_H__


#include "si_cp1292.h"
#include "si_hal.h"

//------------------------------------------------------------------------------
// Module variables
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t cecCommand;         //!< rc protocol command code
    uint8_t rapKeyCode;          //!< RCP CBUS Key Code
    char    rapName[15];
} SI_CecRapConversion_t;

#define CATEGORY_C_CMD_NUM 31
#define CECNOMATCH 0xFF
#define RAPNOMATCH 0xFF

#define RAP_NOERROR 0x00
#define RAP_UNRECOGNIZED 0x01
#define RAP_UNSUPPORTED 0x02
#define RAP_BUSY 0x03

#define MHL_RAP_CMD_POLL 0x00
#define MHL_RAP_CMD_CONTENT_ON 0x10
#define MHL_RAP_CMD_CONTENT_OFF 0x11

#define RAP_IN 0
#define RAP_OUT 1

#define MHL_RAP_CMD_CONTENT_ON_IREG 0x6B
#define MHL_RAP_CMD_CONTENT_OFF_IREG 0x6C

bool_t	IsThisRapMsgInCategoryC(uint8_t rapStatusCode);
bool_t	SI_RapHandler(uint8_t channel);
bool_t	SI_RapCecTranslater(uint8_t channel, uint8_t rapKeyCode);
bool_t	SI_RapTimeoutHandler(uint8_t channel, uint8_t direction);
bool_t	SI_RapKeyIDCheck(uint8_t channel, uint8_t keyID, uint8_t direction);
bool_t SI_RapSendMsg(uint8_t channel, uint8_t rapKeyCode);
bool_t	SI_RapSendAck(uint8_t channel, uint8_t rapStatusCode);
bool_t	SI_RapAckRcvd(uint8_t channel, uint8_t rapKeyCode);
bool_t	SI_RapErrProcess(uint8_t channel, uint8_t rapStatusCode);

uint8_t SI_RapTransferDoneHandler(uint8_t channel);

void SI_RapCecRetHandler(uint8_t channel, bool_t result);
#endif  // __SI_APIRCP_H__
