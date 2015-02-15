/*------------------------------------------------------------------------------

 Copyright 2009-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2012/07/20
 Revision: 1.0.1.0

------------------------------------------------------------------------------*/

#ifndef SONY_STDLIB_H
#define SONY_STDLIB_H

#include "sony_common.h"

/*
 <PORTING> Please modify if ANCI C standard library is not available.
*/

/*#include <string.h>*/
#include <x_os.h> /* For MTK */

#define sony_memcpy  x_memcpy
#define sony_memset  x_memset

#endif /* SONY_STDLIB_H */
