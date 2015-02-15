//***************************************************************************
//! @file     si_scheduler.c
//! @brief    Silicon Image mini scheduler.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_common.h"
#include "si_arm.h"
#include "si_osal_timer.h"
#include "si_platform.h"
#include "si_scheduler.h"
#include "si_cra.h"
#include "hdmi_mhl_if_sii9687.h"


#define SI_SCHED_NORMAL_PRIORITY_MARKER  0x1000

//------------------------------------------------------------------------------
// Module variables (Scheduler never instantiated more than once)
//------------------------------------------------------------------------------

// Signals List
static SiiSchedSignalListEntry_t sigList[SI_SCHED_MAX_SIGNALS];
static uint_t sigNum;
static bool_t pendingSignals;

// Scheduled Tasks Lists
static SiiSchedTaskListEntry_t   prioTaskList[SI_SCHED_MAX_HIGH_PRIO_TASKS];
static uint_t prioTaskNum;
static SiiSchedTaskListEntry_t   normTaskList[SI_SCHED_MAX_NORM_PRIO_TASKS];
static uint_t normTaskNum;

// Hold Mode
static bool_t holdMode;

//------------------------------------------------------------------------------
// Local Methods
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Creates new record in the Signals List
//!
//! @param[in]  signal - signal handling descriptor
//!
//! @return     success flag
//-------------------------------------------------------------------------------------------------

static bool_t SignalListEntryAdd(SiiSchedSignalListEntry_t signal)
{
    int_t  i;
    bool_t isFound;

    if (sigNum < SI_SCHED_MAX_SIGNALS)
    {
        // Check for identical entries
        for (i = 0; i < SI_SCHED_MAX_SIGNALS; i++)
        {
            if ((sigList[i].id == signal.id) && (sigList[i].srcInst == signal.srcInst) &&
                (sigList[i].pFunc == signal.pFunc) && (sigList[i].destInst == signal.destInst))
            {
                DEBUG_PRINT( MSG_ERR, "SL: Identical record already exist!\n" );
                return false;
            }
        }

        // Find first vacant place
        isFound = false;
        for (i = 0; i < SI_SCHED_MAX_SIGNALS; i++)
        {

            if (!sigList[i].id)
            {
                // Fill up vacant place
                sigList[i].id = signal.id;
                sigList[i].srcInst = signal.srcInst;
                sigList[i].isRaised = signal.isRaised;
                sigList[i].pFunc = signal.pFunc;
                sigList[i].destInst = signal.destInst;

                sigNum++;
                isFound = true;
                break;
            }
        }

        if (isFound == false)
        {
            DEBUG_PRINT( MSG_ERR, "SL: All records are in use!\n" );
            return false;
        }
    }
    else
    {
        DEBUG_PRINT( MSG_ERR, "SL: Maximum number of signals exceeded!\n" );
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Removes record from the Signals List
//!
//! @param[in]  signal - signal handling descriptor
//!
//! @return     success flag
//-------------------------------------------------------------------------------------------------

static bool_t SignalListEntryRemove(SiiSchedSignalListEntry_t signal)
{
    int_t i;
    bool_t isFound = false;

    if (sigNum > 0)
    {
        // Find entry by ID, instance IDs and method
        for (i = 0; i < SI_SCHED_MAX_SIGNALS; i++)
        {
            if ((sigList[i].id == signal.id) && (sigList[i].pFunc == signal.pFunc) &&
                (sigList[i].pFunc == signal.pFunc) && (sigList[i].destInst == signal.destInst))
            {
                // Clear record
                sigList[i].id = 0;
                sigList[i].srcInst = 0;
                sigList[i].isRaised = false;
                sigList[i].pFunc = NULL;
                sigList[i].destInst = 0;

                sigNum--;
                isFound = true;
                break;
            }
        }
    }

    if (isFound == false)
    {
        DEBUG_PRINT( MSG_ERR, "SL: Record not found!\n" );
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Creates new record in Task List
//!
//! @param[in]  pTaskList - pointer to task list
//! @param[in]  maxTasks  - maximum number of entries in task list
//! @param[in]  pTaskNum  - pointer to task number counter of the list
//! @param[in]  time      - delay time in ms
//! @param[in]  pTask     - pointer to task function
//! @param[in]  inst      - task instance ID
//! @param[in]  repeat    - timer repeat flag
//!
//! @return     success flag
//-------------------------------------------------------------------------------------------------

static uint_t TaskListEntryAdd(SiiSchedTaskListEntry_t *pTaskList, uint_t maxTasks, uint_t *pTaskNum,
                               clock_time_t time, void (*pTask)(uint_t), uint_t inst, bool_t repeat)
{
    int_t  i;
    uint_t newEntryId = 0;
    bool_t isFound;

    if (*pTaskNum < maxTasks)
    {
        // Find first vacant place
        isFound = false;
        for (i = 0; i < maxTasks; i++)
        {
            if (!pTaskList[i].pFunc)
            {
                // Fill up vacant place
                pTaskList[i].pFunc = pTask;
                pTaskList[i].inst = inst;
                pTaskList[i].period = repeat ? time : 0;

                // Set OS timer for a task
                SiiOsTimerSet(&(pTaskList[i].timer), time);

                (*pTaskNum)++;
                newEntryId = i + 1; // ID is a position in array + 1
                isFound = true;
                break;
            }
        }

        if (isFound == false)
        {
            DEBUG_PRINT( MSG_ERR, "TL: All records are in use!\n" );
            return 0;
        }
    }
    else
    {
        DEBUG_PRINT( MSG_ERR, "TL: Maximum number of tasks exceeded!\n" );
        return 0;
    }

    return newEntryId;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Removes a record from Task List
//!
//! @param[in]  pTaskList - pointer to task list
//! @param[in]  maxTasks  - maximum number of entries in task list
//! @param[in]  pTaskNum  - pointer to task number counter of the list
//! @param[in]  timerId   - timer ID associated with a task
//!
//! @return     success flag
//-------------------------------------------------------------------------------------------------

static bool_t TaskListEntryRemove(SiiSchedTaskListEntry_t *pTaskList, uint_t maxTasks, uint_t *pTaskNum, uint_t timerId)
{
    int_t i;
    bool_t isFound = false;

    if ((*pTaskNum > 0) && (timerId > 0) && (timerId <= maxTasks))
    {
        i = timerId - 1;
        if (pTaskList[i].pFunc)
        {
            // Clear record
            pTaskList[i].pFunc = NULL;
            pTaskList[i].inst = 0;
            pTaskList[i].period = 0;
            pTaskList[i].timer.msDelay = 0;
            pTaskList[i].timer.msStart = 0;

            (*pTaskNum)--;
            isFound = true;
        }
    }
    else
    {
        DEBUG_PRINT( MSG_ERR, "TL: Bad timer ID!\n" );
        return false;
    }

    if (isFound == false)
    {
        DEBUG_PRINT( MSG_ERR, "TL: Record not found!\n" );
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Call all handlers having pending events.
//-------------------------------------------------------------------------------------------------

static void ProcessSignalsList(void)
{
    int_t i;
    int_t risedCount = 1; // set to non-zero to enter first time


    while (risedCount)
    {
        risedCount = 0;
        for (i = 0; i < SI_SCHED_MAX_SIGNALS; i++)
        {
            if (sigList[i].id && sigList[i].isRaised)
            {
                // Clear signal
                sigList[i].isRaised = false;
                // Call handler if its signal is raised with argument of instance ID
              //  Printf("\r\n9687 sigList[%d].pFunc\n",i);
                (*(sigList[i].pFunc))(sigList[i].destInst);
                // The handler is permitted to rise this signal again
                // to force its own invocation
                risedCount++;
            }			
        }		
    }
    // All signals must be processed at this point
    pendingSignals = 0;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Call all tasks having expired timers.
//!
//!             Function calculates shortest time until one of the timers expires.
//!
//! @return     shortest time until the next timer event.
//-------------------------------------------------------------------------------------------------

static clock_time_t ProcessTasksList(SiiSchedTaskListEntry_t *pTaskList, uint_t maxTasks, uint_t *pTaskNum)
{
    int_t i;
    clock_time_t minTaskTimeMs = CLOCK_TIME_MAX;
    clock_time_t timeLeftMs;

    for (i = 0; (i < maxTasks) && *pTaskNum; i++)
    {
        if (pendingSignals)
        {
            // Signals have highest priority and must be processed immediately
            minTaskTimeMs = 0;
            break;
        }

        if (pTaskList[i].pFunc)
        {
           
            if (SiiOsTimerExpired(pTaskList[i].timer))
            {
                // Call nonempty task if timer has expired
              //   Printf("\r\n tpTaskList[%d].pFunc  ",i);
                (*(pTaskList[i].pFunc))(pTaskList[i].inst);
                // Set new timer if requested
                if (pTaskList[i].period)
                {
                    SiiOsTimerSet(&(pTaskList[i].timer), pTaskList[i].period);
                }
                else
                {
                    // Clear task
                    pTaskList[i].pFunc = NULL;
                    (*pTaskNum)--;
                }
            }
            else
            {
                // Get the time left for the task to take off
                timeLeftMs = pTaskList[i].timer.msDelay - SiiOsTimerElapsed(pTaskList[i].timer);
                // Calculate minimum time
                if (timeLeftMs < minTaskTimeMs)
                {
                    minTaskTimeMs = timeLeftMs;
                }
            }
        }
    }

    return minTaskTimeMs;
}


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of scheduler.
//!
//!             Must be called once before calling scheduler main function.
//-------------------------------------------------------------------------------------------------

void SiiSchedInitialize(void)
{
    // Erase all data and mark all entries as vacant
    memset(sigList, 0, sizeof(sigList));
    memset(prioTaskList, 0, sizeof(prioTaskList));
    memset(normTaskList, 0, sizeof(normTaskList));
    // Reset entry counters
    sigNum      = 0;
    prioTaskNum = 0;
    normTaskNum = 0;
    // Normal Mode
    holdMode = false;
    // Reset pending signals flag
    pendingSignals = 0;

    // Enable peripherals to operate when CPU is in sleep.
    //MAP_SysCtlPeripheralClockGating(true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Establish link between an event (signal) and task to be called .
//!
//!             Binding mechanism decouples signal sender and related task replacing
//!             otherwise necessary callback.
//!
//! @param[in]  eventId    - a unique ID
//! @param[in]  srcInst    - signal source instance ID
//! @param[in]  pHandler   - pointer to an event handling function
//! @param[in]  destInst   - signal destination instance ID
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiSchedEventHandlerBind(uint_t eventId, uint_t srcInst, void (*pHandler)(uint_t), uint_t destInst)
{
    SiiSchedSignalListEntry_t sig;
    bool_t success;

    // Make sure signal registration is an atomic operation
#ifdef CC_MHL_HDMI_SWITCH
    EnterCriticalSection();
#endif

    sig.id = eventId;
    sig.srcInst = srcInst;
    sig.isRaised = false;
    sig.pFunc = pHandler;
    sig.destInst = destInst;

    success = SignalListEntryAdd(sig);
	
#ifdef CC_MHL_HDMI_SWITCH
    LeaveCriticalSection();
#endif
    return success;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Break a link between an event (signal) and task to be called .
//!
//!             Link implements mechanism of decoupling signals and related task.
//!
//! @param[in]  eventId    - a unique ID
//! @param[in]  pHandler   - pointer to an event handling function
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiSchedEventHandlerUnbind(uint_t eventId, uint_t srcInst, void (*pHandler)(uint_t), uint_t destInst)
{
    SiiSchedSignalListEntry_t sig;
    bool_t success;

    // Make sure signal registration is an atomic operation
    #ifdef CC_MHL_HDMI_SWITCH
    EnterCriticalSection();
	#endif

    sig.id = eventId;
    sig.srcInst = srcInst;
    sig.isRaised = false;
    sig.pFunc = pHandler;
    sig.destInst = destInst;

    success = SignalListEntryRemove(sig);

	#ifdef CC_MHL_HDMI_SWITCH
    LeaveCriticalSection();
	#endif
    return success;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Delivers signals to recipients mailboxes.
//!             This function is thread safe (doesn't depend on globals that it changes).
//!
//! @param[in]  eventId - registered signal ID
//! @param[in]  srcInst - signal source instance ID
//!
//! @return     Number of recipients receiving the signal.
//-------------------------------------------------------------------------------------------------

uint_t SiiSchedSendSignal(uint_t eventId, uint_t srcInst)
{
    int_t i;
    int_t numOfRecipients;

    numOfRecipients = 0;
    // Find and activate (make event pending) all method registered with the signal ID
    for (i = 0; i < SI_SCHED_MAX_SIGNALS; i++)
    {
        if ((sigList[i].id == eventId) && (sigList[i].srcInst == srcInst))
        {
            sigList[i].isRaised = true;
            numOfRecipients++;
            pendingSignals = true;
        }
    }

    return numOfRecipients;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Register a task function to be called upon timer expiration.
//!
//! @param[in]  time      - time in milliseconds
//! @param[in]  pTask     - pointer to a task function
//! @param[in]  inst      - task instance ID
//! @param[in]  repeat    - repetition flag; if true, the task will be called periodically
//!                                          if false, the task will only be called once
//! @param[in]  priority  - 0 - high priority, non-zero value - normal priority
//!
//! @return     timer ID, 0 - in case of a error.
//-------------------------------------------------------------------------------------------------

uint_t SiiSchedTimerSet(clock_time_t time, void (*pTask)(uint_t), uint_t inst, bool_t repeat, SiiSchedPriority_t priority)
{
    uint_t maxTasks;
    uint_t *pTaskNum;
    uint_t timerId = 0;
    SiiSchedTaskListEntry_t *pTaskList;
    if (priority == SI_SCHED_PRIORITY_HIGH)
    {
        pTaskList = prioTaskList;
        pTaskNum  = &prioTaskNum;
        maxTasks = SI_SCHED_MAX_HIGH_PRIO_TASKS;
    }
    else
    {
        pTaskList = normTaskList;
        pTaskNum  = &normTaskNum;
        maxTasks = SI_SCHED_MAX_NORM_PRIO_TASKS;
    }

    timerId = TaskListEntryAdd(pTaskList, maxTasks, pTaskNum, time, pTask, inst, repeat);

    if (priority == SI_SCHED_PRIORITY_NORMAL)
    {
        timerId |= SI_SCHED_NORMAL_PRIORITY_MARKER;
    }

    return timerId;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Remove task timer and corresponding entry from the tasks list.
//!
//! @param[in]  timerId   - ID uniquely identifying timer
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiSchedTimerClear(uint_t timerId)
{
    uint_t maxTasks;
    uint_t *pTaskNum;
    SiiSchedTaskListEntry_t *pTaskList;

    if (timerId & SI_SCHED_NORMAL_PRIORITY_MARKER)
    {
        // clear priority marker
        timerId &= ~SI_SCHED_NORMAL_PRIORITY_MARKER;

        pTaskList = normTaskList;
        pTaskNum  = &normTaskNum;
        maxTasks = SI_SCHED_MAX_NORM_PRIO_TASKS;
    }
    else
    {
        pTaskList = prioTaskList;
        pTaskNum  = &prioTaskNum;
        maxTasks = SI_SCHED_MAX_HIGH_PRIO_TASKS;
    }

    return TaskListEntryRemove(pTaskList, maxTasks, pTaskNum, timerId);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable and Disable Hold mode.
//!
//!             When Hold Mode is enabled, only high priority tasks are running.
//!             Normal priority tasks will be on hold.
//!
//! @param[in]  isEnabled - enable flag
//-------------------------------------------------------------------------------------------------

void SiiSchedHoldMode(bool_t isEnabled)
{
    holdMode = isEnabled;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Main Loop of Scheduler.
//!
//!             This function is supposed to never return.
//-------------------------------------------------------------------------------------------------

void SiiSchedMainLoop(void)
{
    clock_time_t minTaskTimeMs;
    clock_time_t timeMs;
    uint8_t     intStatus;

    //for(;;)
    {
    	//Printf("\r\n9687  SiiSchedMainLoop\n");
        minTaskTimeMs = CLOCK_TIME_MAX;
    //	Printf("\r\n9687  SiiSchedMainLoop11111111\n");
      intStatus = SiiRegRead( 0x70 );    // Get the register interrupt info
      
      if ( intStatus & 0x01)
      	{
        Printf("\r\nSiiSchedMainLoop intStatus=%02x ...\n",intStatus);
      	SiiSchedSendSignal(SI_EVENT_ID_PLATFORM_DEV_INT, 0);		
	  }

    

        // Call all handlers having pending events
        ProcessSignalsList();

        // Call high priority tasks having expired timers
        minTaskTimeMs = ProcessTasksList(prioTaskList, SI_SCHED_MAX_HIGH_PRIO_TASKS, &prioTaskNum);
        // Call normal priority tasks having expired timers
        if (!holdMode)
        {
            //	Printf("\r\n9687  SiiSchedMainLoop2222222222222\n");

            timeMs = ProcessTasksList(normTaskList, SI_SCHED_MAX_NORM_PRIO_TASKS, &normTaskNum);

            if (timeMs < minTaskTimeMs)
            {
                minTaskTimeMs = timeMs;
            }
        }
		//    	Printf("\r\n9687  SiiSchedMainLoop333333333\n");


        // Sleep for a time that is minimum of all task timers
        // or until h/w interrupt arrives.
        // minTaskTimeMs == 0 indicates pending signals
        if (minTaskTimeMs > 0)
        {
#ifdef CC_MHL_HDMI_SWITCH

            OsSleep(minTaskTimeMs);
#endif
        }

	//usleep(30*1000);//Jane add 0305
	//SiiDrvDeviceManageInterrupts(0);
    }

}
