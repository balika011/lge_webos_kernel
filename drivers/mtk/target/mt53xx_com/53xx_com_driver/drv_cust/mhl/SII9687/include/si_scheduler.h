//***************************************************************************
//! @file     si_scheduler.h
//! @brief    Silicon Image mini scheduler.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#ifndef SI_SCHEDULER_H_
#define SI_SCHEDULER_H_

#include "si_osal_timer.h"
#include "si_sched_config.h"

//------------------------------------------------------------------------------
//  Type definitions
//------------------------------------------------------------------------------

typedef enum _SiiSchedPriority_t
{
    SI_SCHED_PRIORITY_HIGH,
    SI_SCHED_PRIORITY_NORMAL,

} SiiSchedPriority_t;

typedef struct _SiiSchedSignalListEntry_t
{
    uint_t      id;                 // Signal ID
    uint_t      srcInst;            // Signal source Instance ID
    void      (*pFunc)(uint_t);     // Signal recipient method
    uint_t      destInst;           // Signal recipient instance ID
    bool_t      isRaised;           // Signal state

} SiiSchedSignalListEntry_t;

typedef struct _SiiSchedTaskListEntry_t
{
    void          (*pFunc)(uint_t); // Scheduled task
    uint_t          inst;           // Instance ID
    SiiTimer_t      timer;          // timer structure
    clock_time_t    period;         // 0 - one shot timer, non-zero - time period in ms

} SiiSchedTaskListEntry_t;


//------------------------------------------------------------------------------
//  API functions
//------------------------------------------------------------------------------

void    SiiSchedInitialize(void);
void    SiiSchedMainLoop(void);

bool_t  SiiSchedEventHandlerBind(uint_t eventId, uint_t srcInst, void (*pHandler)(uint_t), uint_t destInst);
bool_t  SiiSchedEventHandlerUnbind(uint_t eventId, uint_t srcInst, void (*pHandler)(uint_t), uint_t destInst);
uint_t  SiiSchedSendSignal(uint_t eventId, uint_t srcInst);

uint_t  SiiSchedTimerSet(clock_time_t time, void (*pTask)(uint_t), uint_t inst, bool_t repeat, SiiSchedPriority_t priority);
bool_t  SiiSchedTimerClear(uint_t timerId);
void    SiiSchedHoldMode(bool_t isEnabled);

#endif /* SI_SCHEDULER_H_ */
