/*-----------------------------------------------------------------------------
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2013 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *---------------------------------------------------------------------------*/

/** @file		hal_dtcp.h
 *
 *  Hardware Abstract Layer for DTCP - declaration
 *
 *  @author		Wongi, Hong (wongi.hong@lge.com)
 *  @version	1.0
 *  @date		2013.02.19
 *  @note
 *  @see
 */

#ifndef _HAL_DTCP_H_
#define _HAL_DTCP_H_

#include <stdio.h>

/*------------------------------------------------------------------------
 * global variables and function prototypes declarations
 *-----------------------------------------------------------------------*/
const char* HAL_DTCP_GetDeviceSetPath(void);
const char* HAL_DTCP_GetCryptoConstPath(void);

#endif /* _HAL_DTCP_H_ */

