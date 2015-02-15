//***************************************************************************
//!file     si_drv_ddc_pt.h
//!brief    DDC driver for service and flush mode
//          
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRVDDC_H__
#define __SI_DRVDDC_H__
#include "si_datatypes.h"


typedef enum
{
    DDC_PT_SERVICE_MODE,
    DDC_PT_FLUSH_MODE,
}DDC_PT_TYPE;

typedef enum
{
    DDC_PT_DISABLE,
    DDC_PT_ENABLE,
}DDC_PT_CONTROL;

typedef enum
{
    DDC_PT_PORT_0,
    DDC_PT_PORT_1,
    DDC_PT_PORT_2,
    DDC_PT_PORT_3,
    DDC_PT_PORT_4,

}DDC_PT_PORT;

#define DDC_PT_TYPE_SHIFT           0
#define DDC_PT_CONTROL_SHIFT        1
#define DDC_PT_PORT_SHIFT           5

void  SiiDrvDdcPassThrough(uint8_t ddcType, uint8_t ddcPort);

#endif      // __SI_DRVDDC_H__
