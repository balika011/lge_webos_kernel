//***************************************************************************

//!file     si_RegioCbus.h
//!brief    CBUS register I/O function wrappers.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_regio.h"

uint8_t SiIRegioCbusRead ( uint16_t regAddr, uint8_t channel );
void	SiIRegioCbusWrite ( uint16_t regAddr, uint8_t channel, uint8_t value );
void	SiIRegioCbusModify ( uint16_t regAddr, uint8_t channel, uint8_t mask, uint8_t value );
