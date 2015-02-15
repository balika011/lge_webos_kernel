//***************************************************************************
//! @file     si_sched_config.h
//! @brief    Silicon Image mini scheduler configuration file.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#ifndef SI_SCHED_CONFIG_H_
#define SI_SCHED_CONFIG_H_

// Maximum number of event-handler entries that can be registered
#define SI_SCHED_MAX_SIGNALS 15
// Maximum number of high priority timer driven tasks
#define SI_SCHED_MAX_HIGH_PRIO_TASKS   5
// Maximum number of normal priority timer driven tasks
#define SI_SCHED_MAX_NORM_PRIO_TASKS   20

// Event IDs
#define SI_EVENT_ID_PLATFORM_DEV_INT    0x01
#define SI_EVENT_ID_PLATFORM_IR_INT     0x02
#define SI_EVENT_ID_PLATFORM_JSTK_INT   0x03
#define SI_EVENT_ID_IPV_INT             0x12
#define SI_EVENT_ID_SWITCH_INT          0x13
#define SI_EVENT_ID_CBUS_INT            0x14
#define SI_EVENT_ID_GPIO_INT            0x15
#define SI_EVENT_ID_TX_INT              0x16


#endif /* SI_SCHED_CONFIG_H_ */
