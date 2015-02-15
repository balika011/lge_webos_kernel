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
/******************************************************************************
*[File]             prot_cli.c
*[Version]          v0.1
*[Revision Date]    2008-09-01
*[Description]
*    The program will provide commands to tool authentication.
*[Copyright]
*    Copyright (C) 2004 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/

#ifndef CC_MTK_LOADER

#include "x_os.h"
#include "x_drv_cli.h"
#include "x_printf.h"
#include "x_serial.h"
#include "x_hal_io.h"
#include "x_hal_5381.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_bim.h"
#include "drvcust_if.h"

#include "c_model.h"

#define AUTH_PASS   ("Password Authentication Pass\n")
#define AUTH_FAIL   ("Password Authentication Fail\n")

static UINT32 u4CMD_IO_OFFSET = 0x00000000;
// _fgRegAccess - This variable is used for reg access control of cli. 
// Except in  UnlockRegAccess() and LockRegAccess() functions, we should not assign its value.
static UINT32 _fgRegAccess = 1;

static BOOL UnlockRegAccess(void) 
{
    BIM_WRITE32(REG_RW_MISC, BIM_READ32(REG_RW_MISC) | (MISC_RSREG));
    _fgRegAccess = 1;
    return TRUE;
}

BOOL LockRegAccess(void) 
{
    BIM_WRITE32(REG_RW_MISC, BIM_READ32(REG_RW_MISC) & ~(MISC_RSREG));
    _fgRegAccess = 0;
    return TRUE;
}


/******************************************************************************
* Function      : _CmdMemSet
* Description   : memory set
******************************************************************************/
INT32 _CmdMemSet(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 ui4DestAddr;
    UINT32 ui4Value;
    UINT32 ui4Len;
    UINT32 ui4Idx;

    if ((i4Argc < 3) || (szArgv == NULL) || (szArgv[1] == NULL) || (szArgv[2] == NULL) || (szArgv[3] == NULL))
    {
        return 0;
    }

    ui4DestAddr = StrToInt(szArgv[1]);
    ui4Value = StrToInt(szArgv[2]);
    ui4Len = StrToInt(szArgv[3]);

    // DW alignment
    ui4DestAddr &= 0xFFFFFFFC;

    for (ui4Idx = 0; ui4Idx < ui4Len; ui4Idx++)
    {
        ASSERT(ui4DestAddr != 0);
        IO_WRITE32(u4CMD_IO_OFFSET, ui4DestAddr, ui4Value);
        ui4DestAddr += 4;
    }

    return 0;
//lint -e{818}
}

/******************************************************************************
* Function      : _CmdLinuxMode
* Description   : Turn on/off linux mode
******************************************************************************/
#ifdef __KERNEL__
INT32 _CmdLinuxMode(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Value;

    if ((i4Argc < 2) || (szArgv == NULL) || (szArgv[1] == NULL))
    {
        Printf("Turn on/off linux mode. 1 (on), 0 (off)\n");
        return 0;
    }

    u4Value = StrToInt(szArgv[1]);

    Printf("Linux mode is turned %s\n", u4Value ? "On" : "Off");
    if (u4Value)
    {
        u4CMD_IO_OFFSET = 0xD0000000;
        Printf("Read/Write 0x20000000 is mapped to 0xF0000000 now!\n");
    }
    else
    {
        u4CMD_IO_OFFSET = 0x00000000;
    }

    return 0;
//lint -e{818}
}
#endif

/******************************************************************************
* Function      : _CmdMemRead
* Description   : memory read, word
******************************************************************************/
INT32 _CmdMemRead(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4SrcAddr;
    UINT32 u4Len;
    UINT32 u4Idx;

    if (_fgRegAccess==0)
    {
        return 0;
    }

    if ((i4Argc < 2) || (szArgv == NULL) || (szArgv[1] == NULL) || (szArgv[2] == NULL))
    {
        return 0;
    }

    u4SrcAddr = StrToInt(szArgv[1]);
    u4Len = StrToInt(szArgv[2]);

    if (u4Len == 0)
    {
        return 0;
    }
    if (u4Len > 0x1000)
    {
        u4Len = 0x1000;
    }

    // DW alignment
    u4SrcAddr &= 0xFFFFFFFC;

    for (u4Idx = 0;
        u4Idx < u4Len;
        u4Idx += 4)
    {
        if ((u4Idx & 0xFU) == 0)
        {
            //lint -e{613}
            Printf("\n0x%08X | %08X"
                , (u4SrcAddr + u4Idx)
                , IO_READ32(u4CMD_IO_OFFSET, u4SrcAddr + u4Idx));
        }
        else
        {
            //lint -e{613}
            Printf(" %08X"
                , IO_READ32(u4CMD_IO_OFFSET, u4SrcAddr + u4Idx));
        }
    }
    Printf("\n");

    return 0;
//lint -e{818}
}

/******************************************************************************
* Function      : _CmdMemWrite
* Description   : memory write, word
******************************************************************************/
INT32 _CmdMemWrite(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4DestAddr;
    UINT32 u4Value;
    UINT32 u4Mask;

    if (_fgRegAccess==0)
    {
        return 0;
    }

    if ((i4Argc < 2) || (szArgv == NULL) || (szArgv[1] == NULL) || (szArgv[2] == NULL))
    {
        return 0;
    }

    u4DestAddr = StrToInt(szArgv[1]);
    u4Value = StrToInt(szArgv[2]);

    // DW alignment
    u4DestAddr &= 0xFFFFFFFC;

    ASSERT(u4DestAddr != 0);
    if( i4Argc  == 4)
    {
          //Write mask
        UINT32 u4Tmp ;

        u4Mask = StrToInt(szArgv[3]);
        u4Tmp = IO_READ32(u4CMD_IO_OFFSET, u4DestAddr);
        u4Value = (u4Tmp &(~(u4Mask))) | ((u4Value) & (u4Mask)) ;

        IO_WRITE32(u4CMD_IO_OFFSET, u4DestAddr, u4Value);
    }
    else
    {
        IO_WRITE32(u4CMD_IO_OFFSET, u4DestAddr, u4Value);
    }    

    return 0;
//lint -e{818}
}


//-----------------------------------------------------------------------------
/** _CmdCustomerPassword
 *  Command: custpass
 *
 *  @param  i4Argc          Number of arguments
 *  @param  aszArgv         Argument array
 *
 *  @retval 0               Succeed
 *  @retval Others          Error code
 */
//-----------------------------------------------------------------------------
INT32 _CmdCustomerPassword(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT8 *pu1CustPass;
    INT32 i, fgFailed = 0;
    static UINT8 szMtkPass[] = { "03:46><@" };
    
    if(_fgRegAccess == 1) 
    {
        Printf("Already unlock reg access\n");
        return 0;
    }
    
    if (i4Argc != 2)
    {
        Printf("%s [password]\n", szArgv[0]);
        return 0;
    }

    //Check super password
    for (i=0; szMtkPass[i]!=0; i++)
    {
        if (szArgv[1][i] != (CHAR)(szMtkPass[i] - i))
        {
            fgFailed=1;
        }
    }
    
    //Check if there is redundancy character in the end of szArgv[1]
    if( szArgv[1][i] != 0)
    {
        fgFailed=1;        
    }
    
    if(fgFailed == 1) 
    {
        if (DRVCUST_OptQuery(eCustomerPassword, (UINT32 *)&pu1CustPass)!=0)
        {
            // get password failed, always success.
            goto Auth_Pass;
        }
    
        for (i=0; pu1CustPass[i]!=0; i++)
        {
            if (szArgv[1][i] != (CHAR)pu1CustPass[i])
            {
                goto Auth_Fail;
            }
        }

        //Check if there is redundancy character in the end of szArgv[1]        
        if( szArgv[1][i] != 0)
        {
            goto Auth_Fail; 
        }        
    }

Auth_Pass:
    UnlockRegAccess();
    Printf(AUTH_PASS);
    return 0;

Auth_Fail:
    Printf(AUTH_FAIL);
    return 0;       
}


INT32 _CmdCustomer(INT32 i4Argc, const CHAR ** szArgv)
{
    CHAR szBuf[64];
    INT32 i;
    UINT8 *pu1CustName;
    
    UNUSED(i4Argc);
    UNUSED(szArgv);

    if (DRVCUST_OptQuery(eCustomerName, (UINT32 *)&pu1CustName)!=0)
    {
        x_memcpy (szBuf,"[cmtk / mtk]\n",64);
    }
    else 
    {
        x_memcpy (szBuf, pu1CustName,64);
    }    

    for (i=0; i<64; i++)
    {
        if (szBuf[i]!=0)
        {
            SerPollPutChar((UINT8)szBuf[i]);
        }
        else
        {
            i = 64;
        }
    }

    return 0;
}

#endif /* CC_MTK_LOADER */

