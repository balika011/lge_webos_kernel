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
#ifndef MWIF2_DEBUG_H
#define MWIF2_DEBUG_H

#include "x_typedef.h"
#include "x_printf.h"
#include "x_common.h"

#ifdef SYSTEM_LOG
#include "x_syslog.h"
#endif

/******************************************************************************
*	To define NDEBUG will disable leveling-LOG/Print to console mechanism.
* Instead, LOG level 0, 1, 2, and 3 will print to console directly.  The other
* log level will not exist in binary while compiling with NDEBUG definition.
******************************************************************************/
#ifndef DEFINE_IS_LOG
#define DEFINE_IS_LOG  
#endif

#ifndef DEFINE_VLOG
#define DEFINE_VLOG     
#endif

#define VA_LIST  va_list

/******************************************************************************/
#define _R			((INT32) 1 << 8)
#define _W			((INT32) 1 << 9)
#define _B			((INT32) 1 << 10)
#define _I			((INT32) 1 << 11)
#define _S			((INT32) 1 << 12)
#define LOG_BIT_MASK		(_R | _W | _B | _I | _S)

#define _M			((INT32) 1 << 16)		/* Enable Thread module name display. */
#define _T			((INT32) 1 << 17)		/* Enable Thread id display. */
#define _L			((INT32) 1 << 18)		/* Enable the log data to log thread. */
#define _X			((INT32) 1 << 19)		/* Enable Time display, 1 unit is 1ms. */
#define LOG_HEADER_MASK		(_M | _T | _X)
#ifdef NDEBUG

#ifndef CC_DRV_LOG_LVL
#define CC_DRV_LOG_LVL 2
#endif

#define LOG(level, fmt...)		LOG_##level(fmt)

#if (CC_DRV_LOG_LVL>=0)
#define LOG_0(fmt...)			(DEFINE_IS_LOG(0, fmt))
#else
#define LOG_0(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=1)
#define LOG_1(fmt...)			(DEFINE_IS_LOG(1, fmt))
#else
#define LOG_1(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=2)
#define LOG_2(fmt...)			(DEFINE_IS_LOG(2, fmt))
#else
#define LOG_2(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=3)
#define LOG_3(fmt...)			(DEFINE_IS_LOG(3, fmt))
#else
#define LOG_3(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=4)
#define LOG_4(fmt...)			(DEFINE_IS_LOG(4, fmt))
#else
#define LOG_4(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=5)
#define LOG_5(fmt...)			(DEFINE_IS_LOG(5, fmt))
#else
#define LOG_5(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=6)
#define LOG_6(fmt...)			(DEFINE_IS_LOG(6, fmt))
#else
#define LOG_6(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=7)
#define LOG_7(fmt...)			(DEFINE_IS_LOG(7, fmt))
#else
#define LOG_7(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=8)
#define LOG_8(fmt...)			(DEFINE_IS_LOG(8, fmt))
#else
#define LOG_8(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=9)
#define LOG_9(fmt...)			(DEFINE_IS_LOG(9, fmt))
#else
#define LOG_9(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=10)
#define LOG_10(fmt...)			(DEFINE_IS_LOG(10, fmt))
#else
#define LOG_10(fmt...)
#endif

#if (CC_DRV_LOG_LVL>=11)
#define LOG_11(fmt...)			(DEFINE_IS_LOG(11, fmt))
#else
#define LOG_11(fmt...)
#endif

#else

//lint -emacro({717}, LOG)  to disable "do {...} while (0)" lint warning
#define LOG(level, fmt...)			(DEFINE_IS_LOG(level, fmt))
#define VLOG(level, fmt, valist)	(DEFINE_VLOG(level, fmt, valist))
#endif

/**********/
#define CLIMOD_DEBUG_CLIENTRY(mod)																		\
	{ "debug_on", "d_on", _##mod##CLI_DebugOn, NULL, #mod ".d_on", CLI_GUEST },					\
	{ "debug_off", "d_off", _##mod##CLI_DebugOff, NULL, #mod ".d_off", CLI_GUEST },				\
	{ "debug_level", "d_l", _##mod##CLI_DebugLevel, NULL, #mod ".d_l", CLI_GUEST }

/**********/
#define CLIMOD_DEBUG_FUNCTIONS(mod)																		\
																										\
static INT32 _i4##mod##DebugLevel = -1;																	\
																										\
static INT32 _##mod##CLI_DebugOn(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
	INT32 i4DbgLvl;																						\
																										\
	i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);															\
	if (i4DbgLvl == 0) {																				\
		if (_i4##mod##DebugLevel <= 0) {																\
			i4DbgLvl = 1;																				\
			mod##_i4LogLvlQuerySet(&i4DbgLvl);															\
		} else {																						\
			/* set previous log level. */																\
			mod##_i4LogLvlQuerySet(&_i4##mod##DebugLevel);												\
		}																								\
	} /* else the current status is on. do nothing. */													\
																										\
	Printf(#mod " module debug level is turn on. \n"); 											        \
	                                                                                                    \
    return 0;					                                                              		    \
}																										\
																										\
static INT32 _##mod##CLI_DebugOff(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
	INT32 i4DbgLvl = 0;																					\
																										\
	_i4##mod##DebugLevel = mod##_i4LogLvlQuerySet(NULL);												\
	mod##_i4LogLvlQuerySet(&i4DbgLvl);																	\
	Printf(#mod " module debug level is turn off. \n");												    \
    return 0;																							\
}																										\
																										\
static INT32 _##mod##CLI_DebugLevel(INT32 i4Argc, const CHAR ** szArgv)									\
{																																																				\
	UTILCLI_DebugLevel(mod##_i4LogLvlQuerySet, i4Argc, szArgv);											\
																\
	Printf(#mod " module debug level is %d now.\n", mod##_i4LogLvlQuerySet(NULL));										\
																										\
    return 0;																							\
}

/******************************************************************************
* Below are the declarations of Log level access functions of all modules.
******************************************************************************/
typedef INT32 (* FN_LQS_T)(const INT32 *pi4Data);

EXTERN void UTILCLI_DebugLevel(FN_LQS_T i4LogLvlQuerySet, INT32 i4Argc, const CHAR ** szArgv);

#define LOG_MOD_DECLARE(mod)											\
	EXTERN INT32 mod##_i4LogLvlQuerySet(INT32 const *pi4Data);			\
	EXTERN INT32 mod##_IsLog(INT32 i4Level, const CHAR *szFmt, ...);	\
	EXTERN INT32 mod##_vLog(INT32 i4Level, const CHAR *szFmt, VA_LIST t_ap)		\

LOG_MOD_DECLARE(AUD);
LOG_MOD_DECLARE(BAGT);
LOG_MOD_DECLARE(CEC);
LOG_MOD_DECLARE(DMX);
LOG_MOD_DECLARE(SWDMX);
LOG_MOD_DECLARE(VDEC);
LOG_MOD_DECLARE(TUNER);
LOG_MOD_DECLARE(TVE);
LOG_MOD_DECLARE(VDP);
LOG_MOD_DECLARE(VENC);
LOG_MOD_DECLARE(PMX);
#endif /* __MWIF2_DEBUG_H__ */

