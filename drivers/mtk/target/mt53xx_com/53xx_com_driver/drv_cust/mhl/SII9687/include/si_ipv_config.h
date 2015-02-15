//***************************************************************************
//!file     si_ipv_config.h
//!brief    System-specific configuration information for IPV
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#if !defined __SI_CONFIG_IPV_H__
#define __SI_CONFIG_IPV_H__

//------------------------------------------------------------------------------
//  IPV Component SiI9687-specific
//------------------------------------------------------------------------------

#define SII_NUM_IPV     1

#define SII_IPV_PENTA_THEME_CC_INIT  0, -8660, 8660      // 0, -10^4 * cos(pi/6), +10^4 * cos(pi/6)
#define SII_IPV_PENTA_THEME_CS_INIT -10000, 5000, 5000   // 1, -10^4 * sin(pi/6), -10^4 * sin(pi/6)
#define SII_IPV_PENTA_THEME_RD_INIT  2, 3 // Relative radius {numerator, denominator} (has to be < 1)

// InstaPrevue windows sizes for select MP resolutions
//                                     wid16x9 wid4x3  height hspacer yspacer
#define SII_IPV_WIN_SIZE_640x480          64,     64,     48,    14,     32
#define SII_IPV_WIN_SIZE_720x480          72,     64,     48,    14,     32
#define SII_IPV_WIN_SIZE_1280x720        128,     96,     72,    26,     32
#define SII_IPV_WIN_SIZE_1920x1080       192,    144,    108,    40,     48

// PIP window sizes for select MP resolutions
//                                       wid16x9 wid4x3  height hspacer yspacer
#define SII_IPV_PIP_WIN_SIZE_640x480     128,     96,     72,     0,      0
#define SII_IPV_PIP_WIN_SIZE_720x480     144,    108,     82,     0,      0
#define SII_IPV_PIP_WIN_SIZE_1280x720    256,    192,    144,     0,      0
#define SII_IPV_PIP_WIN_SIZE_1920x1080   256,    192,    144,     0,      0

// PIP window coordinates for select MP resolutions
// For best performance, Y coord should be a multiple of 8
//                                       X                Y
#define SII_IPV_PIP_WIN_COORDS_640x480    640 - 128 - 24,  480 -  72 - 32
#define SII_IPV_PIP_WIN_COORDS_720x480    720 - 144 - 32,  480 -  80 - 32
#define SII_IPV_PIP_WIN_COORDS_1280x720  1280 - 256 - 48,  720 - 144 - 40
#define SII_IPV_PIP_WIN_COORDS_1920x1080 1920 - 256 - 72, 1080 - 144 - 80

// Window background color (Cr, Cb, Y)
#define SII_IPV_WIN_BG_COLOR        0x7F, 0x7F, 0x40    //  Dark Gray
// Window border color (Cr, Cb, Y)
#define SII_IPV_WIN_BORDER_COLOR    0x7F, 0x7F, 0x82    // Light Gray

// Bouncing effect amplitude (pixels)
#define SII_IPV_BOUNCE_AMPL     32

#endif  //__SI_CONFIG_IPV_H__
