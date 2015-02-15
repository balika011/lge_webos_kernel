/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: widget_event.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget_event.c
 *  This header file declares public function prototypes of widget.
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
 
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_demux.h"
//#include "x_hal_5371.h"

#include "osd_if.h"
#include "gfx_if.h"
#include "ir_if.h"
#include "drvcust_if.h"
#include "drv_common.h"
LINT_EXT_HEADER_END
 
#include "widget.h"
#include "widget_gfx.h"
#include "widget_event.h"
#include "nav_if.h"

#include "widget_debug.h"

#include "x_drv_if.h"

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------
#define IR_REPEAT_INTERVAL          300
#define IR_EVENT_MSGQ_PRIORITY      100
#define IR_EVENT_MSGQ_CAPACITY      64

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
//static HANDLE_T             _hIRSema;
//static HANDLE_T             _hRepeatKeySema;
static BOOL                 (*_pfHandler1KeyAction)(WIDGET_OSDINFO_T[], UINT32, void*);
static BOOL                 (*_pfHandler2KeyAction)(WIDGET_OSDINFO_T[], UINT32*, void*);
static BOOL                 (*_pfHandler3KeyAction)(WIDGET_OSDINFO_T[], UINT32*, void*);
static BOOL                 (*_pfHandler)(WIDGET_OSDINFO_T[], WIDGET_EVENT_T);
//static INT32                _i4Repeat = 1;
//static WIDGET_MENULIST_T*   _prMenuList;    
static HANDLE_T             _hEventMsgq;
static HANDLE_T             _hEventDispatcherThread;
static HANDLE_T             _hRepeatTimer;
static BOOL _fgPowerBtnOnly = TRUE;
	
//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

extern NAV_ATV_PROG_INFO    rCurrATVProg;
extern NAV_ATV_TBL_PROG_T   arATVTbl[];


//---------------------------------------------------------------------------
// Public interface prototypes
//---------------------------------------------------------------------------
INT32 WIDGET_StartIRLoop(WIDGET_OSDINFO_T arOSD[]);
INT32 WIDGET_SetIRHandler1KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32, void*));
INT32 WIDGET_SetIRHandler2KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32*, void*));
INT32 WIDGET_SetIRHandler3KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32*, void*));
INT32 WIDGET_SetHandler(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], WIDGET_EVENT_T));
INT32 WIDGET_SendExternalEvent(WIDGET_EVENT_T rEvent);
INT32 WIDGET_StartPowerBtn(void);


//---------------------------------------------------------------------------
// Static function prototypes
//---------------------------------------------------------------------------
//static void _IRLoop(void* pvArg);
//static void _IRPreserveKeyThread(const void* pvTag);
static void _EventSender(INT32 i4Tag);
static void _RepeatKey(HANDLE_T hHandle, void* u4Key);
static void _EventDispatcher(void* pvArg);


//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
static void _EventSender(INT32 i4Tag)
{
    WIDGET_EVENT_T rEvent;
    UINT32 u4MsgSize = sizeof(rEvent);
    if(DRV_IR_GetKey(&(rEvent.rIrEvent), (UINT32)i4Tag) != DRV_E_OK)
    {
        return;
    }    
    if(_fgPowerBtnOnly)
    {
        if(rEvent.rIrEvent.rType == DRV_IR_EVENT_UP && rEvent.rIrEvent.u4Key == BTN_POWER)
        {
            DRV_SYS_WAKEUP_T rWakeupSetting;  
            rWakeupSetting.fgIRWakeup = TRUE;
            rWakeupSetting.fgRS232Wakeup = TRUE;
            rWakeupSetting.fgRTCWakeup = TRUE;
            rWakeupSetting.fgVGAWakeup = TRUE;
            VERIFY(DRV_SYS_SetWakeup(&rWakeupSetting) == DRV_E_OK);
            DRV_SYS_Down();
        }    
    }
    else
    {
        x_msg_q_send(_hEventMsgq, &rEvent, u4MsgSize, IR_EVENT_MSGQ_PRIORITY);
    }
}

static void _RepeatKey(HANDLE_T hHandle, void *pvArg)
{
    WIDGET_EVENT_T rEvent;
    UINT32 u4MsgSize = sizeof(rEvent);
    rEvent.rIrEvent.u4Key = (UINT32)pvArg;
    rEvent.rIrEvent.rType = DRV_IR_EVENT_REPEAT;
    x_msg_q_send(_hEventMsgq, &rEvent, u4MsgSize, IR_EVENT_MSGQ_PRIORITY);
}

static void _EventDispatcher(void* pvArg)
{
    static UINT32 u4PrevKey = BTN_NONE;    
    WIDGET_EVENT_T rEvent;
    UINT16 u2MsgIdx = 0;
    UINT32 u4MsgSize;

    //static HANDLE_T hIRThread;
    //HANDLE_T hTimer;    
    UINT32 au4RememberedKey[3];
    UINT32 u4KeyCount = 0;
    //UINT32* pu4P2KeyCount = &u4KeyCount;
    INT32 i4Ret = 0;
    DRV_IR_EVENT_T rIrEvent;

    WIDGET_OSDINFO_T* prOSD = *((WIDGET_OSDINFO_T**)pvArg);
    WIDGET_MENULIST_T* prMenuList = prOSD->prList;

    UNUSED(i4Ret);
    x_timer_create(&_hRepeatTimer);
    while(1)
    {
        u4MsgSize = sizeof(rEvent);
        if( (i4Ret = x_msg_q_receive(&u2MsgIdx, &rEvent, (SIZE_T *)&u4MsgSize, &_hEventMsgq, 1, X_MSGQ_OPTION_WAIT)) == OSR_OK)
        {
            rIrEvent = rEvent.rIrEvent;

            if(rIrEvent.rType == DRV_IR_EVENT_DOWN)
            {
                if(rIrEvent.u4Key == BTN_DRVKEY_REPEAT)
                {
                    rIrEvent.u4Key = u4PrevKey;
                }
            	
                u4PrevKey = rIrEvent.u4Key;

#if 0            
                // 1-key action has priority over menu action
                if(_pfHandler1KeyAction != NULL)
                {
                    _pfHandler1KeyAction(rEvent.u4Key, _prMenuList);
                }    
#else

                if(rIrEvent.u4Key == BTN_POWER)
                {
                    continue;
                }


        // 1-key action has priority over menu action
                if( (_pfHandler1KeyAction != NULL) && _pfHandler1KeyAction(prOSD, rIrEvent.u4Key, prMenuList))
                {
                    u4KeyCount = 0;
                    x_timer_stop(_hRepeatTimer);                    
                    x_timer_start(_hRepeatTimer, IR_REPEAT_INTERVAL, X_TIMER_FLAG_REPEAT, _RepeatKey, (VOID *)rIrEvent.u4Key);
                    continue;
                }
            	
                if(u4KeyCount == 0)
                {                    
                    if(rIrEvent.u4Key >= BTN_DIGIT_0 && rIrEvent.u4Key <= BTN_DIGIT_9)
                    {
                        au4RememberedKey[u4KeyCount++] = rIrEvent.u4Key;                                                                              
                        NAV_DrawInfoOSD_2(rIrEvent.u4Key - BTN_DIGIT_0, 0);
                    }
                    else if(rIrEvent.u4Key == BTN_DIGIT_PLUS_100)
                    {
                        au4RememberedKey[u4KeyCount++] = rIrEvent.u4Key;                                                                              
                        NAV_DrawInfoOSD_2(100, 0);
                    }
                }
                else if(u4KeyCount == 1)
                {
                    if(_pfHandler2KeyAction != NULL && rIrEvent.u4Key >= BTN_DIGIT_0 && rIrEvent.u4Key <= BTN_DIGIT_9)
                    {
                        if(au4RememberedKey[0] != BTN_DIGIT_PLUS_100)
                        {
                            UINT32 u4ChNum;
                            au4RememberedKey[u4KeyCount++] = rIrEvent.u4Key;
                            u4ChNum = (au4RememberedKey[0] - BTN_DIGIT_0) * 10 + au4RememberedKey[1] - BTN_DIGIT_0;
                            NAV_DrawInfoOSD_2(u4ChNum, 0);                        
                            if(_pfHandler2KeyAction(prOSD, au4RememberedKey, prMenuList))
                            {                            
                                u4KeyCount = 0;
                            }
                            else
                            {
                                u4KeyCount = 0;
                                u4ChNum = arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx + 1;
                                NAV_DrawInfoOSD_2(u4ChNum, 0); 
                            }
                        }
                        else
                        {
                            UINT32 u4ChNum;
                            au4RememberedKey[u4KeyCount++] = rIrEvent.u4Key;
                            u4ChNum = 100 + (au4RememberedKey[1] - BTN_DIGIT_0) * 10;
                            NAV_DrawInfoOSD_2(u4ChNum, 0);  
                        }
                    }
                }
                else if(u4KeyCount == 2)
                {
                    if(_pfHandler3KeyAction != NULL && rIrEvent.u4Key >= BTN_DIGIT_0 && rIrEvent.u4Key <= BTN_DIGIT_9)
                    {
                        UINT32 u4ChNum;
                        au4RememberedKey[u4KeyCount++] = rIrEvent.u4Key;
                        u4ChNum = 100 + (au4RememberedKey[1] - BTN_DIGIT_0) * 10 + au4RememberedKey[2];
                        NAV_DrawInfoOSD_2(u4ChNum, 0);  
                        _pfHandler3KeyAction(prOSD, au4RememberedKey, prMenuList);                                       
                    }                    
                    u4KeyCount = 0;                            
                }
#endif

                
            }
            else if(rEvent.rIrEvent.rType == DRV_IR_EVENT_EXT)
            {
                if( (_pfHandler != NULL) && _pfHandler(prOSD, rEvent))
                {
                    u4KeyCount = 0;	                    
                    continue;
                }
            }
            else if(rEvent.rIrEvent.rType == DRV_IR_EVENT_UP)
            {
                x_timer_stop(_hRepeatTimer);      
                if(rIrEvent.u4Key == BTN_POWER)
                {
                    // 1-key action has priority over menu action
                    if( (_pfHandler1KeyAction != NULL) && _pfHandler1KeyAction(prOSD, rIrEvent.u4Key, prMenuList))
                    {
                        u4KeyCount = 0;                 
                        continue;
                    }
                }
                rIrEvent.u4Key = BTN_NONE;
                u4PrevKey = BTN_NONE;
            }
            else if(rEvent.rIrEvent.rType == DRV_IR_EVENT_REPEAT)
            {
                if( (_pfHandler1KeyAction != NULL) && _pfHandler1KeyAction(prOSD, rIrEvent.u4Key, prMenuList))
                {
                               
                }            
            }
        }
    }
}
/*
static void _IRPreserveKeyThread(const void* pvTag)
{
    UINT32* u4KeyCount;
    VERIFY(pvTag != NULL);
    u4KeyCount = (UINT32*)(*(UINT32*)pvTag);
    // If after WIDGET_IR_TIMEOUT ms, we cannot collect 3 pressed keys, reset collected keys
    x_thread_delay(WIDGET_IR_TIMEOUT);
    VERIFY(x_sema_lock(_hIRSema, X_SEMA_OPTION_WAIT) == OSR_OK);
    *u4KeyCount = 0;
    VERIFY(x_sema_unlock(_hIRSema) == OSR_OK);
    x_thread_exit();
}
*/
//---------------------------------------------------------------------------
// Public interfaces
//---------------------------------------------------------------------------
INT32 WIDGET_SetHandler(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], WIDGET_EVENT_T))
{
    _pfHandler = pfHandler;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_SetIRHandler1KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32, void*))
{

    _pfHandler1KeyAction = pfHandler;
    return (INT32)E_WIDGET_MSG_OK;

}

INT32 WIDGET_SetIRHandler2KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32*, void*))
{		
    _pfHandler2KeyAction = pfHandler;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_SetIRHandler3KeyAction(BOOL (*pfHandler)(WIDGET_OSDINFO_T[], UINT32*, void*))
{		
    _pfHandler3KeyAction = pfHandler;
    return (INT32)E_WIDGET_MSG_OK;
}
	
INT32 WIDGET_StartIRLoop(WIDGET_OSDINFO_T arOSD[])
{
    INT32 i4Ret;
    //DRV_IR_PARAM_T rParam;

    //rParam.rIrType      = DRV_IR_TYPE_AUTO;
    //rParam.rRepeatFreq  = DRV_IR_REPEAT_FREQ_NONE;
    i4Ret               = x_msg_q_create(&_hEventMsgq, "M_IREVENT", sizeof(WIDGET_EVENT_T), IR_EVENT_MSGQ_CAPACITY);
    //rParam.pfnBtnEvent  = _EventSender;
    i4Ret               = x_thread_create(&_hEventDispatcherThread, "WIDGET", 15000, NAV_IR_PRIORITY, _EventDispatcher, sizeof(void*), (void*)&arOSD);    

#if 0 // Start norml event processing
    DRV_IR_SetParameter(&rParam);
    DRV_IR_Start();
#else
    _fgPowerBtnOnly = FALSE;
#endif
  
    if (i4Ret != DRV_E_OK)
    {
        Printf("Create thread to poll MTK-IR failed.\n");
        return (INT32)E_WIDGET_MSG_ERROR;
    }
    Printf("Create thread to poll MTK-IR successfully.\n");
    
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_SendExternalEvent(WIDGET_EVENT_T rEvent)
{    
    x_msg_q_send(_hEventMsgq, &rEvent, sizeof(rEvent), 100);
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_StartPowerBtn(void)
{
    DRV_IR_PARAM_T rParam;  
    rParam.rIrType      = DRV_IR_TYPE_AUTO;
    rParam.rRepeatFreq  = DRV_IR_REPEAT_FREQ_NONE;
    rParam.pfnBtnEvent  = _EventSender;
    DRV_IR_SetParameter(&rParam);
    DRV_IR_Start();
    
    return (INT32)E_WIDGET_MSG_OK;    
}

