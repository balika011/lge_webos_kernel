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
 * $RCSfile: nav_dolby.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_dolby.c
 *
 */

 
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
LINT_EXT_HEADER_END

#include "nav_debug.h"

INT32 NAV_DBY_Speaker_BassManager(void* pvArg);
INT32 NAV_DBY_Speaker_Subwoofer(void* pvArg);
INT32 NAV_DBY_Spdif_PCMChannel(void* pvArg);
INT32 NAV_DBY_DolbyDigital_OutputMode(void* pvArg);
INT32 NAV_DBY_DolbyDigital_DualMono(void* pvArg);
INT32 NAV_DBY_DolbyDigital_Dynamic(void* pvArg);
INT32 NAV_DBY_DolbyDigital_Compression(void* pvArg);
INT32 NAV_DBY_DolbyDigital_Karaoke(void* pvArg);
INT32 NAV_DBY_ChannelDelay_L(void* pvArg);
INT32 NAV_DBY_ChannelDelay_R(void* pvArg);
INT32 NAV_DBY_ChannelDelay_LS(void* pvArg);
INT32 NAV_DBY_ChannelDelay_RS(void* pvArg);
INT32 NAV_DBY_ChannelDelay_C(void* pvArg);
INT32 NAV_DBY_ChannelDelay_LFE(void* pvArg);
INT32 NAV_DBY_Prologic2_Prologic2(void* pvArg);
INT32 NAV_DBY_Prologic2_Mode(void* pvArg);
INT32 NAV_DBY_Prologic2_Panorama(void* pvArg);
INT32 NAV_DBY_Prologic2_Dimension(void* pvArg);
INT32 NAV_DBY_Prologic2_CenterWidth(void* pvArg);

LINT_EXT_HEADER_BEGIN
INT32 NAV_DBY_Init(void)
{
    NAV_DBY_Speaker_BassManager("OFF");
    NAV_DBY_Speaker_Subwoofer("ON");
    NAV_DBY_Spdif_PCMChannel("L/R");
    NAV_DBY_DolbyDigital_OutputMode("3/2");
    NAV_DBY_DolbyDigital_DualMono("Stereo");
    NAV_DBY_DolbyDigital_Dynamic("OFF");
    NAV_DBY_DolbyDigital_Compression("Line");
    NAV_DBY_DolbyDigital_Karaoke("Disable");
    NAV_DBY_ChannelDelay_L("0 ms");
    NAV_DBY_ChannelDelay_R("0 ms");
    NAV_DBY_ChannelDelay_LS("0 ms");
    NAV_DBY_ChannelDelay_RS("0 ms");
    NAV_DBY_ChannelDelay_C("0 ms");
    NAV_DBY_ChannelDelay_LFE("0 ms");
    NAV_DBY_Prologic2_Prologic2("OFF");
    NAV_DBY_Prologic2_Mode("Prologic Emulation");
    NAV_DBY_Prologic2_Panorama("OFF");
    NAV_DBY_Prologic2_Dimension("0");
    NAV_DBY_Prologic2_CenterWidth("0");
}


INT32 NAV_DBY_Speaker_BassManager(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "OFF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.bs 0 0");
	}
	else if(x_strcmp(szStr, "Config 1 SSS") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.ls 0 0 0 0");
	}
	else if(x_strcmp(szStr, "Simple LSS") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.ls 0 1 0 0");
	}
	return FALSE;
}
INT32 NAV_DBY_Speaker_Subwoofer(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "ON)") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.sw 0 1");
	}
	else if(x_strcmp(szStr, "OFF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.sw 0 0");
	}
	return FALSE;
}
INT32 NAV_DBY_Spdif_PCMChannel(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "L/R") == 0)
	{
	    CLI_Parser_Arg("aud.spdif.iecc 0 0");
	}
	else if(x_strcmp(szStr, "LS/RS") == 0)
	{
	    CLI_Parser_Arg("aud.spdif.iecc 0 1");
	}
	else if(x_strcmp(szStr, "C/LFE") == 0)
	{
	    CLI_Parser_Arg("aud.spdif.iecc 0 2");
	}
	return FALSE;
}

INT32 NAV_DBY_DolbyDigital_OutputMode(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "2/0 (Lt/Rt)") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 0");
	}
	else if(x_strcmp(szStr, "2/0 (LO/RO)") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 2");
	}
	else if(x_strcmp(szStr, "1/0") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 1");
	}
	else if(x_strcmp(szStr, "3/0") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 3");
	}
	else if(x_strcmp(szStr, "2/1") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 4");
	}
	else if(x_strcmp(szStr, "3/1") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 5");
	}
	else if(x_strcmp(szStr, "2/2") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 6");
	}
	else if(x_strcmp(szStr, "3/2") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.ch 0 7");
	}
	else if(x_strcmp(szStr, "2/0 (AUTO)") == 0)
	{
	     CLI_Parser_Arg("aud.uop.spkuop.dm 0 1");
	}
	return FALSE;
}


INT32 NAV_DBY_DolbyDigital_DualMono(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "Stereo") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dm 0 0");
	}
	else if(x_strcmp(szStr, "Left Mono") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dm 0 1");
	}
	else if(x_strcmp(szStr, "Right Mono") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dm 0 2");
	}
	else if(x_strcmp(szStr, "Mix Mono") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dm 0 3");
	}
	return FALSE;
}


INT32 NAV_DBY_DolbyDigital_Dynamic(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "Full") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dr 0 8");
	}
	else if(x_strcmp(szStr, "3/4") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dr 0 6");
	}
	else if(x_strcmp(szStr, "1/2") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dr 0 4");
	}
	else if(x_strcmp(szStr, "1/4") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dr 0 2");
	}
	else if(x_strcmp(szStr, "OFF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3dr 0 0");
	}
	return FALSE;
}


INT32 NAV_DBY_DolbyDigital_Compression(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "Line") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3cm 0 2");
	}
	else if(x_strcmp(szStr, "Custom 0") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3cm 0 0");
	}
	else if(x_strcmp(szStr, "Custom 1") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3cm 0 1");
	}
	else if(x_strcmp(szStr, "RF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3cm 0 3");
	}
	return FALSE;
}



INT32 NAV_DBY_DolbyDigital_Karaoke(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "Disable") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 0");
	}
	else if(x_strcmp(szStr, "Aware") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 1");
	}
	else if(x_strcmp(szStr, "None") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 2");
	}
	else if(x_strcmp(szStr, "V1 only") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 3");
	}
	else if(x_strcmp(szStr, "V2 only") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 4");
	}
	else if(x_strcmp(szStr, "V1 + V2") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 5");
	}
	else if(x_strcmp(szStr, "Lt/Rt") == 0)
	{
	    CLI_Parser_Arg("aud.uop.ac3cfg.ac3ka 0 0");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_L(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 1 103");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_R(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 2 103");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_LS(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 3 103");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_RS(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 4 103");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_C(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 0 103");
	}
	return FALSE;
}

INT32 NAV_DBY_ChannelDelay_LFE(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 0");
	}
	else if(x_strcmp(szStr, "1 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 7");
	}
	else if(x_strcmp(szStr, "2 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 14");
	}
	else if(x_strcmp(szStr, "3 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 21");
	}
	else if(x_strcmp(szStr, "4 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 28");
	}
	else if(x_strcmp(szStr, "5 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 34");
	}
	else if(x_strcmp(szStr, "10 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 69");
	}
	else if(x_strcmp(szStr, "11 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 76");
	}
	else if(x_strcmp(szStr, "15 ms") == 0)
	{
	    CLI_Parser_Arg("aud.uop.spkuop.delaycfg 0 7 103");
	}
	return FALSE;
}

INT32 NAV_DBY_Prologic2_Prologic2(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "ON") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.sw 1");
	}
	else if(x_strcmp(szStr, "OFF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.sw 0");
	}
	else if(x_strcmp(szStr, "AUTO") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.sw 2");
	}
	return FALSE;
}


INT32 NAV_DBY_Prologic2_Mode(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "Prologic Emulation") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 0");
	}
	else if(x_strcmp(szStr, "Virtual") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 1");
	}
	else if(x_strcmp(szStr, "Movie") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 2");
	}
	else if(x_strcmp(szStr, "Music") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 3");
	}
	else if(x_strcmp(szStr, "Matrix") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 4");
	}
	else if(x_strcmp(szStr, "Custom") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.mode 0 7");
	}
	return FALSE;
}

INT32 NAV_DBY_Prologic2_Panorama(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "ON") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.pano 1");
	}
	else if(x_strcmp(szStr, "OFF") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.pano 0");
	}

	return FALSE;
}


INT32 NAV_DBY_Prologic2_Dimension(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "-3") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 0");
	}
	else if(x_strcmp(szStr, "-2") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 1");
	}
	else if(x_strcmp(szStr, "-1") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 2");
	}
	else if(x_strcmp(szStr, "0") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 3");
	}
	else if(x_strcmp(szStr, "1") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 4");
	}
	else if(x_strcmp(szStr, "2") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 5");
	}
	else if(x_strcmp(szStr, "3") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.dim 6");
	}
	return FALSE;
}

INT32 NAV_DBY_Prologic2_CenterWidth(void* pvArg)
{
	CHAR* szStr = (CHAR*)pvArg;
	if(x_strcmp(szStr, "0") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 0");
	}
	else if(x_strcmp(szStr, "1") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 1");
	}
	else if(x_strcmp(szStr, "2") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 2");
	}
	else if(x_strcmp(szStr, "3") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 3");
	}
	else if(x_strcmp(szStr, "4") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 4");
	}
	else if(x_strcmp(szStr, "5") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 5");
	}
	else if(x_strcmp(szStr, "6") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 6");
	}
	else if(x_strcmp(szStr, "7") == 0)
	{
	    CLI_Parser_Arg("aud.uop.pl2cfg.cw 7");
	}
	return FALSE;
}

LINT_EXT_HEADER_END
	
