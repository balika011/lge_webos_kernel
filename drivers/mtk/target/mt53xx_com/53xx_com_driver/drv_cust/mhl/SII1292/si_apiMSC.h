//***************************************************************************
//!file     si_apiMSC.h
//!brief    SiI1292 MSC functions head file.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_APIMSC_H__
#define __SI_APIMSC_H__

#include "si_cp1292.h"
#include "si_hal.h"
#include "si_apiCbus.h"

typedef struct
{
    bool_t LR_SUPP;
    bool_t TB_SUPP;
    bool_t FS_SUPP;
} _3D_VDI_T;
typedef struct
{
    //For 3D_DTD
    uint8_t   _3D_DTD_TOT_ENT;
    _3D_VDI_T _3D_DTD_VDI[10];
    uint8_t   _3D_DTD_Index;

    //For 3D_DTD
    uint8_t   _3D_VIC_TOT_ENT;
    _3D_VDI_T _3D_VIC_VDI[32];
    uint8_t   _3D_VIC_Index;
} _3D_SUPPORT_DTD_VIC_LIST_T;

bool_t			SI_MscClrHpd(uint8_t channel, uint8_t uData);
bool_t			SI_MscSetHpd(uint8_t channel, uint8_t uData);
bool_t			SI_MscGrtWrt(uint8_t channel, uint8_t uData);
bool_t			SI_MscPathEnable(uint8_t channel, uint8_t uData);
bool_t			SI_MscPathDisable(uint8_t channel, uint8_t uData);
bool_t			SI_MscConnectReady(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadBandwidth(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadDeviceID_H(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadDeviceID_L(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadVideoType(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadAudioLinkMode(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadVideoLinkMode(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadLogDevMap(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadDevCat(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadAdopterID_H(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadAdopterID_L(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadMhlVersion(uint8_t channel, uint8_t uData);
bool_t			SI_MscReadFeatureFlag(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadDeviceState(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadScratchpadSize(uint8_t channel, uint8_t uData);
bool_t 			SI_MscReadIntStatSize(uint8_t channel, uint8_t uData);

void 	SI_MscStartGetDevInfo(uint8_t channel);
void 			SI_MscDevCapAPIStateUpdate(void);
bool_t  			SI_WriteBurstDataDone(uint8_t channel, uint8_t dummy);


bool_t			SI_MscWriteBurst3D_DTD_REQ_WRT_Done(uint8_t channel, uint8_t uData);
bool_t			SI_MscWriteBurst3D_3D_DTD_Done(uint8_t channel, uint8_t uData);
bool_t			SI_MscWriteBurst3D_DTD_DSCR_CHG_Done(uint8_t channel, uint8_t uData);
bool_t			SI_MscWriteBurst3D_3D_VIC_Done(uint8_t channel, uint8_t uData);
bool_t			SI_MscWriteBurst3D_VIC_REQ_WRT_Done(uint8_t channel, uint8_t uData);
bool_t 			SI_MscWriteBurst_Done(uint8_t channel);
#endif  // __SI_APIMSC_H__
