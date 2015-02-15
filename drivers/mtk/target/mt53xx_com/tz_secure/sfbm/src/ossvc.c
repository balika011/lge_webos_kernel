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
 * $RCSfile: ossvc.c $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#define EMPTY_FUNC(func) int func(void) {}
#define FUNC_SHOULD_NOT_RUN(func) int func(void) { printf("%s should not be run!\n",__FUNCTION__); exit(1); }

/* just for linking */
EMPTY_FUNC(x_sema_create)
EMPTY_FUNC(SRM_Init)
EMPTY_FUNC(SRM_RegisterCB)
EMPTY_FUNC(SRM_QueryStatus)

FUNC_SHOULD_NOT_RUN(IR_IsLog)
FUNC_SHOULD_NOT_RUN(CLI_IsLog)

FUNC_SHOULD_NOT_RUN(x_sema_unlock)
FUNC_SHOULD_NOT_RUN(x_sema_lock)
FUNC_SHOULD_NOT_RUN(x_sema_lock_timeout)
FUNC_SHOULD_NOT_RUN(x_sema_delete)

FUNC_SHOULD_NOT_RUN(x_thread_create)
FUNC_SHOULD_NOT_RUN(x_thread_delay)

FUNC_SHOULD_NOT_RUN(HAL_GetTime)
FUNC_SHOULD_NOT_RUN(HAL_GetDeltaTime)
FUNC_SHOULD_NOT_RUN(HalCriticalStart)
FUNC_SHOULD_NOT_RUN(HalCriticalEnd)
FUNC_SHOULD_NOT_RUN(x_os_drv_crit_start)
FUNC_SHOULD_NOT_RUN(x_os_drv_crit_end)
FUNC_SHOULD_NOT_RUN(x_reg_isr)

FUNC_SHOULD_NOT_RUN(BIM_LockModifyReg32)
FUNC_SHOULD_NOT_RUN(BSP_GetClock)

FUNC_SHOULD_NOT_RUN(RTC_UtcToRtc)
FUNC_SHOULD_NOT_RUN(RTC_BinaryToBcd)
FUNC_SHOULD_NOT_RUN(RTC_BcdToBinary)

FUNC_SHOULD_NOT_RUN(BSP_PinSet) //(int i, int j) {}
FUNC_SHOULD_NOT_RUN(EEPROM_Read)
FUNC_SHOULD_NOT_RUN(EEPROM_Write)
FUNC_SHOULD_NOT_RUN(SIF_Init)
FUNC_SHOULD_NOT_RUN(SIF_X_Read)
FUNC_SHOULD_NOT_RUN(SIF_X_Write)
FUNC_SHOULD_NOT_RUN(SIF_Read)
FUNC_SHOULD_NOT_RUN(SIF_Write)
FUNC_SHOULD_NOT_RUN(GPIO_Input)
FUNC_SHOULD_NOT_RUN(GPIO_Output)
FUNC_SHOULD_NOT_RUN(vDrvODInit)
FUNC_SHOULD_NOT_RUN(PDWNC_T8032Cmd)
FUNC_SHOULD_NOT_RUN(PMX_SetBg)
FUNC_SHOULD_NOT_RUN(EEPDTV_GetCfg)
FUNC_SHOULD_NOT_RUN(EEPHDMIEDID_Read)

FUNC_SHOULD_NOT_RUN(SRM_GetCurrFBMMode)

FUNC_SHOULD_NOT_RUN(FBM_GetCbFuncTbl)
FUNC_SHOULD_NOT_RUN(FBM_SetFrameBufferFlag)

FUNC_SHOULD_NOT_RUN(RTC_RtcToUtc)
FUNC_SHOULD_NOT_RUN(BIM_ClearIrq)
FUNC_SHOULD_NOT_RUN(_PDWNC_ReadWakeupStatus)
FUNC_SHOULD_NOT_RUN(_PDWNC_ReadWatchDogStatus)
FUNC_SHOULD_NOT_RUN(PDWNC_IsWakeupByPowerButton)
FUNC_SHOULD_NOT_RUN(_SIF_ISR_IsPdwncMasterEnable)
FUNC_SHOULD_NOT_RUN(_SIF_ISR_IsPdwncMaster1Enable)
FUNC_SHOULD_NOT_RUN(vIO32Write1BMsk)
FUNC_SHOULD_NOT_RUN(fgIs4k2kNativeTimingOut)
FUNC_SHOULD_NOT_RUN(BSP_GetIcVersion)
#include <stdarg.h>
#include <string.h>

void *x_mem_alloc(unsigned int size) { return malloc(size); }
void x_mem_free(void*ptr) { free(ptr); }

void* x_memset(void*p,int v, int s) { return memset(p,v,s); }
void* x_memcpy(void*d,void*s,int sz) { return memcpy(d,s,sz); }

int BSP_GetChBMemSize(void) { return 0x40000000; /* 1G */ }

int Printf(char *str, ...) { int ret; va_list argsptr; va_start(argsptr,str); ret = vprintf(str,argsptr); va_end(argsptr); return ret; }

int FBM_IsLog(int g, char *str, ...) { int ret; va_list argsptr; va_start(argsptr,str); ret = vprintf(str,argsptr); va_end(argsptr); return ret; }

int Assert(int should_be_true) { if(should_be_true <= 0) { printf("ASSERT fail\n"); exit(1); } }

