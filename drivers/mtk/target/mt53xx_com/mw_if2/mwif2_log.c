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
#ifndef CC_MTK_LOADER
#include "x_typedef.h"
//#define DEFINE_IS_LOG 
#include "x_lint.h"
#include "x_os.h"
#include "x_stl_lib.h"


#include <stdio.h>
#include <stdlib.h>
#include "cli/x_cli.h"
#include "mwif2_debug.h"

//lint -save
//lint -e960 cannot use #/## in maro many times.
//lint -e950 use __align(16)
//lint -e818 bother to ask ass const in function parameter.
//lint -e718 va_list...
//lint -e645 variable might not initialize, but it's stupid.
//lint -e564 a variable appears too many time in expression.
//lint -e530 va_list not initial.
//lint -e522 I don't know why cannot use function pointer.

#ifndef CC_DRIVER_PROGRAM
#define DEFAULT_LEVEL   0
#else
#ifdef __KERNEL__
#define DEFAULT_LEVEL   0
#else                     // defined(CC_DRIVER_PROGRAM) && !defined(__KERNEL__)
#define DEFAULT_LEVEL   5 /* raise default debug level in drv_build */
#endif
#endif

/***********************************************************************/

#define LOG_LEVEL_FUNCTIONS(mod, level)                                 \
                                                                        \
static INT32 _i4##mod##LogLvl = (level);                                \
                                                                        \
INT32 mod##_i4LogLvlQuerySet(INT32 const *pi4Data)                      \
{                                                                       \
    if (pi4Data) {                                                      \
        _i4##mod##LogLvl = *pi4Data;                                    \
    }                                                                   \
    return _i4##mod##LogLvl;                                            \
}                                                                       \
                                                                        \
INT32 mod##_IsLog(INT32 i4Level, const CHAR *szFmt, ...)                \
{                                                                       \
    if (_fgLog && ((i4Level <= (_i4##mod##LogLvl & 0x0ff))              \
        || (i4Level & 0x0ff00 & _i4##mod##LogLvl) || (i4Level & _S))) { \
        VA_LIST v_arg;                                                  \
        VA_START(v_arg,szFmt);                                          \
        printf("[%s]",#mod);                                            \
        vprintf(szFmt,v_arg);                                           \
        VA_END(v_arg);                                                  \
        return i4Level;                                                 \
    }                                                                   \
    return 0;                                                           \
}                                                                       \
                                                                        \
INT32 mod##_vLog(INT32 i4Level, const CHAR *szFmt, VA_LIST t_ap)        \
{                                                                       \
    if (_fgLog && ((i4Level <= (_i4##mod##LogLvl & 0x0ff))              \
        || (i4Level & 0x0ff00 & _i4##mod##LogLvl) || (i4Level & _S))) { \
        i4Level = _UTIL_SetLog(_i4##mod##LogLvl, #mod, szFmt, t_ap,     \
        i4Level);                                                       \
        return i4Level;                                                 \
    }                                                                   \
    return 0;                                                           \
}                                                                       \

static BOOL _fgLog = TRUE;

UINT32 StrToHex(const CHAR* pszStr, UINT32 u4Len)
{
	UINT32 u4Idx;
	UINT32 u4ReturnValue = 0;

	if ((pszStr == NULL) || (u4Len == 0))
	{
		return 0;
	}

	u4Len = (u4Len > 8) ? 8 : u4Len;

	for (u4Idx = 0;
		u4Idx < u4Len;
		u4Idx++)
	{
		if ((pszStr[u4Idx] >= '0') && (pszStr[u4Idx] <= '9'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (UINT32)(UINT8)(pszStr[u4Idx] - '0');
		}
		else
		if ((pszStr[u4Idx] >= 'A') && (pszStr[u4Idx] <= 'F'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (UINT32)(UINT8)(pszStr[u4Idx] - 'A' ) + 10;
		}
		else
		if ((pszStr[u4Idx] >= 'a') && (pszStr[u4Idx] <= 'f'))
		{
			u4ReturnValue = u4ReturnValue << 4;
			u4ReturnValue += (UINT32)(UINT8)(pszStr[u4Idx] - 'a') + 10;
		}
		else
		{
			return 0;
		}
	}

	return u4ReturnValue;
}

UINT32 StrToDec(const CHAR* pszStr, UINT32 u4Len)
{
	UINT32 u4Idx;
	UINT32 u4ReturnValue = 0;

	if ((pszStr == NULL) || (u4Len == 0))
	{
		return 0;
	}

	// 0xFFFFFFFF = 4294967295
	u4Len = (u4Len > 10) ? 10 : u4Len;

	for (u4Idx = 0;
		u4Idx < u4Len;
		u4Idx++)
	{
		if ((pszStr[u4Idx] >= '0') && (pszStr[u4Idx] <= '9'))
		{
			u4ReturnValue *= 10;
			u4ReturnValue += (UINT32)(UINT8)(pszStr[u4Idx] - '0');
		}
		else
		{
			return 0;
		}
	}

	return u4ReturnValue;
}

UINT32 StrToInt(const CHAR* pszStr)
{
	UINT32 u4Len;

	if (pszStr == NULL)
	{
		return 0;
	}

	u4Len = x_strlen(pszStr);

	if (u4Len > 2)
	{
		if ((pszStr[0] == '0') && (pszStr[1] == 'x'))
		{
			return StrToHex(&pszStr[2], u4Len - 2);
		}
	}

	return StrToDec(pszStr, u4Len);
}

void UTILCLI_DebugLevel(FN_LQS_T i4LogLvlQuerySet, INT32 i4Argc, const CHAR ** szArgv)
{
    INT32 i, i4DbgLvl;

    if(szArgv == NULL)
    {
        Printf("The argument is NULL\n");
		return;
    }
	
    if(i4LogLvlQuerySet == NULL)
    {
        Printf("The Log Lvl function is NULL\n");
		return;
    }
	
    for (i=1; i<i4Argc; i++) {
        if ((szArgv[i][0] == '+') || (szArgv[i][0] == '-')) {
            switch(szArgv[i][1]) {
            case 'R': case 'r':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _R) == 0) { i4DbgLvl += _R; }
                } else {
                        if ((i4DbgLvl & _R) != 0) { i4DbgLvl -= _R; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'W': case 'w':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _W) == 0) { i4DbgLvl += _W; }
                } else {
                        if ((i4DbgLvl & _W) != 0) { i4DbgLvl -= _W; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'B': case 'b':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _B) == 0) { i4DbgLvl += _B; }
                } else {
                        if ((i4DbgLvl & _B) != 0) { i4DbgLvl -= _B; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'I': case 'i':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _I) == 0) { i4DbgLvl += _I; }
                } else {
                        if ((i4DbgLvl & _I) != 0) { i4DbgLvl -= _I; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'M': case 'm':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _M) == 0) { i4DbgLvl += _M; }
                } else {
                        if ((i4DbgLvl & _M) != 0) { i4DbgLvl -= _M; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'T': case 't':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _T) == 0) { i4DbgLvl += _T; }
                } else {
                        if ((i4DbgLvl & _T) != 0) { i4DbgLvl -= _T; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'L': case 'l':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _L) == 0) { i4DbgLvl += _L; }
                } else {
                        if ((i4DbgLvl & _L) != 0) { i4DbgLvl -= _L; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            case 'X': case 'x':
                i4DbgLvl = i4LogLvlQuerySet(NULL);
                if (szArgv[i][0] == '+') {
                        if ((i4DbgLvl & _X) == 0) { i4DbgLvl += _X; }
                } else {
                        if ((i4DbgLvl & _X) != 0) { i4DbgLvl -= _X; }
                }
                i4LogLvlQuerySet(&i4DbgLvl);
                break;
            default:
                Printf("%s is not support.\n", szArgv[i]);
                break;
            }
        } else {
            if ((szArgv[i][0] >= '0') && (szArgv[i][0] <='9')) {
                i4DbgLvl = (StrToInt(szArgv[i]) & 0x0ff) ;
                i4LogLvlQuerySet(&i4DbgLvl);
            }
        }
    }
}

static INT32 _UTIL_SetLog(INT32 i4ModLvl, CHAR *szMod, const CHAR *szFmt, VA_LIST t_ap, INT32 i4Level)
{
    return TRUE;
}


LOG_LEVEL_FUNCTIONS(CLI,    (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(AUD,    (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(BAGT,	(DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(CEC,    (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(DMX,    (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(SWDMX,  (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(VDEC,   (DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(TUNER,	(DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(TVE,	(DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(VDP,	(DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(VENC,	(DEFAULT_LEVEL | _M | _L))
LOG_LEVEL_FUNCTIONS(PMX,	(DEFAULT_LEVEL | _M | _L))


CLIMOD_DEBUG_FUNCTIONS(AUD)
CLIMOD_DEBUG_FUNCTIONS(BAGT)
CLIMOD_DEBUG_FUNCTIONS(CEC)
CLIMOD_DEBUG_FUNCTIONS(DMX)
CLIMOD_DEBUG_FUNCTIONS(SWDMX)
CLIMOD_DEBUG_FUNCTIONS(VDEC)
CLIMOD_DEBUG_FUNCTIONS(TUNER)
CLIMOD_DEBUG_FUNCTIONS(TVE)
CLIMOD_DEBUG_FUNCTIONS(VDP)
CLIMOD_DEBUG_FUNCTIONS(VENC)
CLIMOD_DEBUG_FUNCTIONS(PMX)


//lint -restore
#ifdef CLI_SUPPORT 
extern UINT32 _u4MwIfDbgFlags ;


static INT32 _mwif_cli_dbg(INT32 i4Argc, const CHAR **szArgv)
{
	if(i4Argc==2)
    {
       _u4MwIfDbgFlags = strtol(szArgv[1], NULL, 16) ;
    }
    else
    {
        Printf("VDEC : 0x8\n");
		Printf("VDP : 0x10\n");
		Printf("SRCR : 0x20\n");
		Printf("OUTR : 0x40\n");
		Printf("vdp set: 0x80\n");
		Printf("lifa : 0x100\n");
    }
	Printf("\nCurrent Val:0x%x\n", _u4MwIfDbgFlags);
    
	return 0 ;
};


static CLI_EXEC_T at_mwif_aud_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(AUD),
	END_OF_CLI_CMD_TBL,
	
};

static CLI_EXEC_T at_mwif_bagt_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(BAGT),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_cec_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(CEC),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_dmx_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(DMX),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_swdmx_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(SWDMX),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_vdec_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(VDEC),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_tuner_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(TUNER),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_tve_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(TVE),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_vdp_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(VDP),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_venc_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(VENC),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_pmx_tbl[] = 
{
    CLIMOD_DEBUG_CLIENTRY(PMX),
	END_OF_CLI_CMD_TBL,

};

static CLI_EXEC_T at_mwif_cli_tbl[] = 
{
    {"dbg",          "dbg"         ,_mwif_cli_dbg,           NULL,    "_mwif_cli_dbg",              CLI_GUEST},
    {"aud",          "aud"         ,NULL,           at_mwif_aud_tbl,    "mwif aud commands",              CLI_GUEST},
    {"bagt",       "bagt"      ,NULL,           at_mwif_bagt_tbl,    "mwif bagt commands",           CLI_GUEST},
    {"cec",          "cec"         ,NULL,           at_mwif_cec_tbl,    "mwif cec commands",              CLI_GUEST},
    {"dmx",          "dmx"         ,NULL,           at_mwif_dmx_tbl,    "mwif dmx commands",              CLI_GUEST},
    {"swdmx",        "swdmx"       ,NULL,           at_mwif_swdmx_tbl,    "mwif swdmx commands",            CLI_GUEST},
    {"vdec",         "vdec"        ,NULL,           at_mwif_vdec_tbl,    "mwif vdec commands",             CLI_GUEST},
    {"tuner",        "tuner"       ,NULL,           at_mwif_tuner_tbl,    "mwif tuner commands",            CLI_GUEST},
    {"tve",          "tve"         ,NULL,           at_mwif_tve_tbl,    "mwif tve commands",              CLI_GUEST},
    {"vdp",          "vdp"         ,NULL,           at_mwif_vdp_tbl,    "mwif vdp commands",              CLI_GUEST},
    {"venc",         "venc"        ,NULL,           at_mwif_venc_tbl,    "mwif venc commands",              CLI_GUEST},
    {"pmx",          "pmx"         ,NULL,           at_mwif_pmx_tbl,     "mwif pmx commands",              CLI_GUEST},
    /* Last entry is used as an ending indicator */
	END_OF_CLI_CMD_TBL,
    END_OF_CLI_CMD_TBL
};


static CLI_EXEC_T at_mwif_root_cmd_tbl[] =
{
    {"mwif",     NULL,        NULL,       at_mwif_cli_tbl,      "mwif commands",     CLI_GUEST},
    END_OF_CLI_CMD_TBL
};

#endif

INT32 mw_if_cli_init(VOID)
{
#ifdef CLI_SUPPORT 
    INT32   i4_return;

    /* Attach mwif CLI command table to CLI */
    i4_return = x_cli_attach_cmd_tbl(at_mwif_root_cmd_tbl, CLI_CAT_MW, CLI_GRP_NONE);


   // test_cli_reg_sub_table(at_mwif_test_root_cmd_tbl);
    if (i4_return != CLIR_OK)
    {
        return -1;
    }
    return 0;
#else
    return 0;
#endif  /* CLI_SUPPORT */
}

#endif // CC_MTK_LOADER

