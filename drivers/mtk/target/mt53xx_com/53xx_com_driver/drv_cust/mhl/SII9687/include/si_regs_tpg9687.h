//***************************************************************************
//!file     si_regs_tpg9687.h
//!brief    SiI9687 Device Register Manifest Constants.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_9687REGS_TPG_H__
#define __SI_9687REGS_TPG_H__

#include "si_cra_cfg.h"

#define REG_VPG_CTRL_1              (PP_PAGE_B | 0xE0)
#define BIT_VPG_EXT_EN                  (BIT2)

#define REG_VPG_CTRL_2              (PP_PAGE_B | 0xE5)
#define MSK_VPG_PATTERN                 0xF0
#define VAL_VPG_PATTERN_SHIFT           4


#define REG_VPG_CTRL_3              (PP_PAGE_B | 0xFF)
#define BIT_VPG_EN                      (BIT0)
#define MSK_VPG_CLK_SEL                 (BIT2 | BIT1)
#define BIT_VPG_TMDS_BIST_EN            (BIT3)
#define MSK_VPG_FORMAT                  0xF0

#define VAL_VPG_FORMAT_SHIFT            4

#endif  // __SI_9687REGS_TPG_H__

