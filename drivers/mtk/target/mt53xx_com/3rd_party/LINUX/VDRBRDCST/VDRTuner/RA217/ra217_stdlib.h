/*------------------------------------------------------------------------------

 Copyright 2009, 2010 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2010/03/19
 Revision: 1.0.0.0

------------------------------------------------------------------------------*/

#ifndef RA217_STDLIB_H
#define RA217_STDLIB_H

#include "ra217_tuner_common.h"

/*
 <PORTING> Please modify if ANCI C standard library is not available.
*/

//#include <string.h>
#include "x_os.h"
#ifdef RA217_FP_ENABLE
#include <math.h>
#endif

#define ra217_memcpy  x_memcpy
#define ra217_memset  x_memset

#define ra217_log     log
#define ra217_log10   log10

#endif /* RA217_STDLIB_H */
