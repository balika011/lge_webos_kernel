/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2007, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: dmx_emu_drm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_des.c
 *  Demux driver - DES/3DES emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"

#include "gcpu.h"
//#include "des_pattern.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** GCPU_EMU_EFUSE
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_EFUSE()
{
    INT32 i;
    INT32 i4Ret;
    GCPU_IF_PARAM_T rParam;

    UINT8 au1Data[64];
    UINT8 au1Golden[64];
    UINT8 au1Addr[4] = {};

    for(i = 0; i < 64; i++)
    {
        au1Data[i] = i;
    }
    x_memcpy(au1Golden, au1Data, 64);

    // DMEM test
    rParam.u4ADR = 0x0; //DMX DMEM starts
    rParam.u1DataLen = 64;
    rParam.pu1Data = au1Data;

    i4Ret = GCPU_Cmd(0, GCPU_IF_WRITE, &rParam);

    i4Ret = GCPU_Cmd(0, GCPU_IF_READ, &rParam);

    if(x_memcmp(au1Data, au1Golden, 64) == 0)
    {
        Printf("EFUSE DMEM access Pass\n");
    }
    else
    {
        Printf("EFUSE DMEM access Fail\n");
        while(1);
    }

    // EFUSE test
    rParam.u4ADR = 0x4660; //DMX DMEM starts
    rParam.u1DataLen = 64;
    rParam.pu1Data = au1Data;

    i4Ret = GCPU_Cmd(0, GCPU_IF_READ, &rParam);

    Printf("EFUSE data:");

    for(i = 0; i < 64; i++)
    {
        if(i % 8 == 0)
        {
            Printf("\n  ");
        }

        Printf("0x%02x ", au1Data[i]);
    }
    Printf("\n  ");

    //HDCP test
    for(i = 0; i < 64; i++)
    {
        au1Data[i] = i;
    }
    x_memcpy(au1Golden, au1Data, 64);

    for (i = 0; i < 16; i++)
    {
        ///1. wirte HDCP (0xc4<<2), addr
        ///2. wite HDCP (0xc5<<2), data
        au1Addr[0] = i;
        rParam.u4ADR = 0x8000 + (0xc4<<2); //DMX DMEM starts
        rParam.u1DataLen = 4;
        rParam.pu1Data = au1Addr;

        i4Ret = GCPU_Cmd(0, GCPU_IF_WRITE, &rParam);

        rParam.u4ADR = 0x8000 + (0xc5<<2); //DMX DMEM starts
        rParam.u1DataLen = 4;
        rParam.pu1Data = au1Data + i * 4;

        i4Ret = GCPU_Cmd(0, GCPU_IF_WRITE, &rParam);
    }

    for (i = 0; i < 16; i++)
    {
        ///1. wirte HDCP (0xc4<<2), addr
        ///2. read HDCP (0xc5<<2)
        au1Addr[0] = i;
        rParam.u4ADR = 0x8000 + (0xc4<<2); //DMX DMEM starts
        rParam.u1DataLen = 4;
        rParam.pu1Data = au1Addr;

        i4Ret = GCPU_Cmd(0, GCPU_IF_WRITE, &rParam);

        rParam.u4ADR = 0x8000 + (0xc5<<2);
        rParam.u1DataLen = 4;
        rParam.pu1Data = au1Data + i * 4;

        i4Ret = GCPU_Cmd(0, GCPU_IF_READ, &rParam);
    }

    //Check Golden
    if(x_memcmp(au1Data, au1Golden, 64) == 0)
    {
        Printf("HDCP test pass\n");
    }
    else
    {
        Printf("HDCP test fail\n");
        while(1);
    }

    return 0;
}

