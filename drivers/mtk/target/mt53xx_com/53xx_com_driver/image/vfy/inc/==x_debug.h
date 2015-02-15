/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
//lint -emacro(534,LOG)

//lint -save
//lint -e960
//lint -e961

#ifndef __X_DEBUG_H__
#define __X_DEBUG_H__

#include "x_typedef.h"
#include "x_common.h"
#include "x_printf.h"
//#include "section.h"

#ifdef SYSTEM_LOG
#include "x_syslog.h"
#endif


/******************************************************************************
*	To define NDEBUG will disable leveling-LOG/Print to console mechanism.
* Instead, LOG level 0, 1, 2, and 3 will print to console directly.  The other
* log level will not exist in binary while compiling with NDEBUG definition.
******************************************************************************/


// 8530Build #ifndef DEFINE_IS_LOG
// 8530Build #define DEFINE_IS_LOG   CLI_IsLog
// 8530Build #endif

#ifndef DEFINE_VLOG
#define DEFINE_VLOG     CLI_vLog
#endif

#if  1//(CONFIG_SECTION_BUILD_ARM2 || CONFIG_SECTION_BUILD || CONFIG_SECTION_BUILD_LINUX_PROG)
#define UTIL_Printf Printf
#else
EXTERN INT32 UTIL_Printf(const CHAR *ps_format, ...);
#endif


/******************************************************************************/
#ifdef NDEBUG

#define LOG(level, fmt...)		LOG_##level(fmt)
#define LOG_0(fmt...)			UTIL_Printf(fmt)
#define LOG_1(fmt...)			UTIL_Printf(fmt)
#define LOG_2(fmt...)
#define LOG_3(fmt...)
#define LOG_4(fmt...)
#define LOG_5(fmt...)
#define LOG_6(fmt...)
#define LOG_7(fmt...)
#define LOG_8(fmt...)
#define LOG_9(fmt...)
#define LOG_10(fmt...)
#define LOG_11(fmt...)

#define LOG__R(fmt...)
#define LOG__W(fmt...)
#define LOG__B(fmt...)
#define LOG__I(fmt...)
#define LOG__S(fmt...)

#define LOG_E(fmt...)               UTIL_Printf(fmt)
#define LOG_W(fmt...)
#define LOG_I(fmt...)
#define LOG_N(fmt...)
#define LOG_D(fmt...)
#define LOG_V(fmt...)

#define LOG_LOG_ERROR(fmt...)       UTIL_Printf(fmt)
#define LOG_LOG_WARN(fmt...)
#define LOG_LOG_INFO(fmt...)
#define LOG_LOG_NOTE(fmt...)
#define LOG_LOG_DEBUG(fmt...)

#else

//lint -emacro({717}, LOG)  to disable "do {...} while (0)" lint warning
#define LOG(level, fmt...)			(DEFINE_IS_LOG(level, fmt))
#define VLOG(level, fmt, valist)	(DEFINE_VLOG(level, fmt, valist))
#define LOG_E(fmt...)               LOG(1, fmt)
#define LOG_W(fmt...)               LOG(3, fmt)
#define LOG_I(fmt...)               LOG(5, fmt)
#define LOG_N(fmt...)               LOG(7, fmt)
#define LOG_D(fmt...)               LOG(9, fmt)
#define LOG_V(fmt...)               LOG(11, fmt)

#endif
/******************************************************************************/


#define LOG_ERROR	1
#define LOG_WARN	3
#define LOG_INFO	5
#define LOG_NOTE	7
#define LOG_DEBUG	9

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

/******************************************************************************
*    CLIMOD_DEBUG_CLIENTRY(mod) macro is for a CLI structure to insert its own
* debug level command, debug_on, debug_off, and debug_level.
*    CLIMOD_DEBUG_FUNCTIONS(mod) macro is for a CLI module to implement its own
* debug level setting functions which are mapping to debug_on, debug_off, and
* debug_level commands respectively.
*
*    To use these two macros, user must put CLIMOD_DEBUG_FUNCTIONS(mod) before
* the CLI item list, and then in CLI item list, user can insert CLIMOD_DEBUG_CLIENTRY(mod)
* in the structure.  For example, in file cli_foo.c, which is a CLI program to
* debug the low level module, FOO.
*
* ----------------------------
* ......
* CLIMOD_DEBUG_FUNCTIONS(FOO)
* ......
* CLI_EXEC_T arFOOCmdTbl[] = {
*     { ...... },
*     CLIMOD_DEBUG_CLIENTRY(FOO),
*     { ...... },
* }
* ......
* ----------------------------
*    To support these two macro, the low level module FOO implementation must
* provide a function calls, INT32 FOO_i4LogLvlQuerySet(INT32 *pData), to the
* log level of the low level module FOO.
******************************************************************************/

/**********/
#define CLIMOD_DEBUG_CLIENTRY(mod)																		\
	{ "debug_on", "d_on", _##mod##CLI_DebugOn, NULL, #mod ".d_on", CLI_SUPERVISOR },					\
	{ "debug_off", "d_off", _##mod##CLI_DebugOff, NULL, #mod ".d_off", CLI_SUPERVISOR },				\
	{ "debug_level", "d_l", _##mod##CLI_DebugLevel, NULL, #mod ".d_l", CLI_SUPERVISOR }

/**********/
#define CLIMOD_DEBUG_CLIENTRY_EX(mod, AccessRight)																		\
	{ "debug_on", "d_on", _##mod##CLI_DebugOn, NULL, #mod ".d_on", AccessRight },					\
	{ "debug_off", "d_off", _##mod##CLI_DebugOff, NULL, #mod ".d_off", AccessRight },				\
	{ "debug_level", "d_l", _##mod##CLI_DebugLevel, NULL, #mod ".d_l", AccessRight }

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
			/* no value reference, set log level 1. */													\
			i4DbgLvl = 1;																				\
			mod##_i4LogLvlQuerySet(&i4DbgLvl);															\
		} else {																						\
			/* set previous log level. */																\
			mod##_i4LogLvlQuerySet(&_i4##mod##DebugLevel);												\
		}																								\
	} /* else the current status is on. do nothing. */													\
																										\
	UTIL_Printf(#mod " module debug level is turn on as %s.\n", 												\
			UTILCLI_LogLevelText(mod##_i4LogLvlQuerySet(NULL)));										\
    return 0;																							\
}																										\
																										\
static INT32 _##mod##CLI_DebugOff(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
	INT32 i4DbgLvl = 0;																					\
																										\
	_i4##mod##DebugLevel = mod##_i4LogLvlQuerySet(NULL);												\
	mod##_i4LogLvlQuerySet(&i4DbgLvl);																	\
	UTIL_Printf(#mod " module debug level is turn off as 0.\n");												\
    return 0;																							\
}																										\
																										\
static INT32 _##mod##CLI_DebugLevel(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
/*	INT32 i, i4DbgLvl; */																				\
																										\
	UTILCLI_DebugLevel(mod##_i4LogLvlQuerySet, i4Argc, szArgv);											\
/*	for (i=1; i<i4Argc; i++) {																			\
		if ((szArgv[i][0] == '+') || (szArgv[i][0] == '-')) {											\
			switch(szArgv[i][1]) {																		\
			case 'R': case 'r':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _R) == 0) { i4DbgLvl += _R; }									\
				} else {																				\
						if ((i4DbgLvl & _R) != 0) { i4DbgLvl -= _R; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'W': case 'w':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _W) == 0) { i4DbgLvl += _W; }									\
				} else {																				\
						if ((i4DbgLvl & _W) != 0) { i4DbgLvl -= _W; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'B': case 'b':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _B) == 0) { i4DbgLvl += _B; }									\
				} else {																				\
						if ((i4DbgLvl & _B) != 0) { i4DbgLvl -= _B; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'I': case 'i':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _I) == 0) { i4DbgLvl += _I; }									\
				} else {																				\
						if ((i4DbgLvl & _I) != 0) { i4DbgLvl -= _I; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'M': case 'm':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _M) == 0) { i4DbgLvl += _M; }									\
				} else {																				\
						if ((i4DbgLvl & _M) != 0) { i4DbgLvl -= _M; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'T': case 't':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _T) == 0) { i4DbgLvl += _T; }									\
				} else {																				\
						if ((i4DbgLvl & _T) != 0) { i4DbgLvl -= _T; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'L': case 'l':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _L) == 0) { i4DbgLvl += _L; }									\
				} else {																				\
						if ((i4DbgLvl & _L) != 0) { i4DbgLvl -= _L; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			default:																					\
				UTIL_Printf(#mod " %s is not support.\n", szArgv[i]);										\
				break;																					\
			}																							\
		} else {																						\
			if (szArgv[i][0] >= '0' && szArgv[i][0] <='9') {											\
				i4DbgLvl = (StrToInt(szArgv[i]) & 0x0ff) | (mod##_i4LogLvlQuerySet(NULL) & 0xffff00);	\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
			}																							\
		}																								\
	} */																								\
	UTIL_Printf(#mod " module debug level is %s now.\n", 													\
			UTILCLI_LogLevelText(mod##_i4LogLvlQuerySet(NULL)));										\
																										\
    return 0;																							\
}


/******************************************************************************
*      To support these two macro, this file of FOO module needs to define
* FOO_BASE, FOO_REG_LENGTH, and arFOORgtList.
******************************************************************************/

/**********/
#define CLIMOD_REGTST_CLIENTRY(mod)																		\
    { "reg_dft_chk", "regc", _##mod##CLI_RegDftChk, NULL, #mod ".regc", CLI_SUPERVISOR},				\
    { "regrw_test", "regt", _##mod##CLI_RegRWTest, NULL, #mod ".regt", CLI_SUPERVISOR},					\
    { "reg_space_test", "spt", _##mod##CLI_SpaceRWTest, NULL, #mod ".spt", CLI_SUPERVISOR}

/**********/
#define CLIMOD_REGTST_FUNCTIONS(mod)																	\
																										\
static INT32 _##mod##CLI_RegDftChk(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_RegDefChk(mod##_BASE, ar##mod##RgtList);												\
	UTIL_Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}																										\
																										\
static INT32 _##mod##CLI_SpaceRWTest(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_AllSpaceRWTest(mod##_BASE, mod##_REG_LENGTH);										\
	UTIL_Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}																										\
																										\
static INT32 _##mod##CLI_RegRWTest(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_RegRWTest(mod##_BASE, ar##mod##RgtList);												\
	UTIL_Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}

/**********/
#define DECLARE_G_SUBLIST(module, cmd, ab, msg)    \
     {                                             \
        #cmd,                    /* cmd name */    \
        #ab,                     /* cmd abname */  \
        NULL,                    /* function */    \
        ar##module##CmdTbl,      /* sublist */     \
        msg,                     /* msg */         \
        CLI_GUEST                /* permission */  \
    }

/**********/
#define DECLARE_G_CMD(func, cmd, ab, msg)   \
    {                                       \
        #cmd,           /* cmd name */      \
        #ab,            /* cmd abname */    \
        func,           /* function */      \
        NULL,           /* sublist */       \
        msg,            /* msg */           \
        CLI_GUEST       /* permission */    \
    }

/**********/
#define DECLARE_SUBLIST(module, cmd, ab, msg)      \
     {                                             \
        #cmd,                    /* cmd name */    \
        #ab,                     /* cmd abname */  \
        NULL,                    /* function */    \
        ar##module##CmdTbl,      /* sublist */     \
        msg,                     /* msg */         \
        CLI_SUPERVISOR           /* permission */  \
    }

/**********/
#define DECLARE_CMD(func, cmd, ab, msg)     \
    {                                       \
        #cmd,           /* cmd name */      \
        #ab,            /* cmd abname */    \
        func,           /* function */      \
        NULL,           /* sublist */       \
        msg,            /* msg */           \
        CLI_SUPERVISOR  /* permission */    \
    }

/**********/
#define DECLARE_END_ITEM()                              \
    {                                                   \
        NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR    \
    }


/******************************************************************************
* Below are the declarations of Log level access functions of all modules.
******************************************************************************/
typedef INT32 (* FN_LQS_T)(const INT32 *pi4Data);

EXTERN UINT8 *UTILCLI_LogLevelText(INT32 i4LogLevel);
EXTERN void UTILCLI_DebugLevel(FN_LQS_T i4LogLvlQuerySet, INT32 i4Argc, const CHAR ** szArgv);
EXTERN BOOL UTIL_SetLogFlag(BOOL fgSet);
EXTERN INT32 UTIL_Log(const CHAR *szFmt, ...);
EXTERN INT32 UTIL_vLog(const CHAR *szFmt, VA_LIST t_ap);

#ifndef CC_MINI_DRIVER
EXTERN void UTIL_FlushSLog(void);
#endif /* CC_MINI_DRIVER */

#define LOG_MOD_DECLARE(mod)											\
	EXTERN INT32 mod##_i4LogLvlQuerySet(INT32 const *pi4Data);			\
	EXTERN INT32 mod##_IsLog(INT32 i4Level, const CHAR *szFmt, ...);	\
	EXTERN INT32 mod##_vLog(INT32 i4Level, const CHAR *szFmt, VA_LIST t_ap)		\

LOG_MOD_DECLARE(GFX);
LOG_MOD_DECLARE(GFXSC);
LOG_MOD_DECLARE(OSD);
LOG_MOD_DECLARE(PMX);
LOG_MOD_DECLARE(VDP);
LOG_MOD_DECLARE(TVE);
LOG_MOD_DECLARE(TVD);
LOG_MOD_DECLARE(MPV);
LOG_MOD_DECLARE(AUD);
LOG_MOD_DECLARE(SMC);
LOG_MOD_DECLARE(IDE);
LOG_MOD_DECLARE(DMX);
LOG_MOD_DECLARE(PSR);
//LOG_MOD_DECLARE(SAMPLE);
LOG_MOD_DECLARE(BD);
LOG_MOD_DECLARE(UART);
LOG_MOD_DECLARE(FCI);
LOG_MOD_DECLARE(MSDC);
LOG_MOD_DECLARE(IR);
LOG_MOD_DECLARE(NAND);
LOG_MOD_DECLARE(NOR);
LOG_MOD_DECLARE(FBM);
LOG_MOD_DECLARE(VIN);
LOG_MOD_DECLARE(SIF);
LOG_MOD_DECLARE(RTC);
LOG_MOD_DECLARE(CLI);
LOG_MOD_DECLARE(FAT);
LOG_MOD_DECLARE(MID);
LOG_MOD_DECLARE(PVR);
LOG_MOD_DECLARE(POD);
LOG_MOD_DECLARE(DV);
LOG_MOD_DECLARE(DVMGR);
LOG_MOD_DECLARE(I1394);
LOG_MOD_DECLARE(NPTV);
LOG_MOD_DECLARE(MD);
LOG_MOD_DECLARE(BWT);
LOG_MOD_DECLARE(HDMI);
LOG_MOD_DECLARE(AUDINMULTI);
LOG_MOD_DECLARE(HDMIRXAUD); 
LOG_MOD_DECLARE(NET);
LOG_MOD_DECLARE(LPDMX);
LOG_MOD_DECLARE(AVICFA);
LOG_MOD_DECLARE(ASFCFA);
LOG_MOD_DECLARE(MP4CFA);
LOG_MOD_DECLARE(AUDINCFA);
LOG_MOD_DECLARE(FLVCFA);
LOG_MOD_DECLARE(RMCFA);
LOG_MOD_DECLARE(STORG);
LOG_MOD_DECLARE(MUSB);
LOG_MOD_DECLARE(SYNCCTRL);
LOG_MOD_DECLARE(PBBUF);
LOG_MOD_DECLARE(VFD);
LOG_MOD_DECLARE(JDEC);
LOG_MOD_DECLARE(PNG);
LOG_MOD_DECLARE(IMGDRV);
LOG_MOD_DECLARE(INFLATE);
LOG_MOD_DECLARE(POSTP);
LOG_MOD_DECLARE(GIF);
LOG_MOD_DECLARE(GPIO);
LOG_MOD_DECLARE(AUDIOIN);
LOG_MOD_DECLARE(VDec_OMX);
LOG_MOD_DECLARE(PWMDAC);
LOG_MOD_DECLARE(VDOIN);
#endif /* __X_DEBUG_H__ */

//lint -restore

