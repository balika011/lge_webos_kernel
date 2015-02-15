/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

/* MS VC (Windows) does not have stdint.h; HCI does define it's own here */

#ifndef HCI_BASIC_TYPES_H
#define HCI_BASIC_TYPES_H

#ifndef __KERNEL__
typedef unsigned char     uint8_t;
typedef unsigned int      uint32_t;
#endif

#endif /* #ifndef HCI_BASIC_TYPES_H */
