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
#include "des_pattern.h"
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
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
static UINT8 au1KeyDES[24] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                              0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                              0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
static UINT8 au1Dat64[8] = {0xa2, 0x8e, 0x91, 0x72, 0x4c, 0x4b, 0xba, 0x31};
static UINT8 au1Res64[8] = {0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x66, 0x63};

static UINT8 au1Dat128[8] = {0x1f, 0x89, 0xd6, 0xf6, 0xe6, 0x3f, 0xb9, 0x6c};
static UINT8 au1Res128[8] = {0x08, 0x1a, 0x2b, 0x3c, 0x4d, 0x5c, 0x6d, 0x7f};

static UINT8 au1Dat192[8] = {0x03, 0x5b, 0x09, 0xd2, 0xcf, 0x29, 0xbe, 0xd2};
static UINT8 au1Res192[8] = {0x08, 0x1a, 0x2b, 0x3c, 0x4d, 0x5c, 0x6d, 0x7f};

static UINT8 au1KeyDES40[24] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x10, 0x32, 0x54};
static UINT8 au1Dat40[8] = {0x7d, 0x74, 0x92, 0x2d, 0x74, 0xb1, 0x2e, 0x13};
static UINT8 au1Res40[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

INT32 GCPU_EMU_DESD()
{
    SIM_DES_PARAM_T rParam;
    UINT8 au1Gol[8];
    INT32 i;

    for (i = 0; i < 4; i++)
    {
        x_memcpy(rParam.au1Key, au1KeyDES, sizeof(au1KeyDES));

        if(i == 0)
        {
            x_memcpy(rParam.au1Dat, au1Dat64, sizeof(au1Dat64));
            x_memcpy(au1Gol, au1Res64, sizeof(au1Res64));
            rParam.eKeyBitLen = KEY_BIT_LEN_64;
            rParam.u4SecureKeyPtr = 0;
        }
        else if(i == 1)
        {
            x_memcpy(rParam.au1Dat, au1Dat128, sizeof(au1Dat128));
            x_memcpy(au1Gol, au1Res128, sizeof(au1Res128));
            rParam.eKeyBitLen = KEY_BIT_LEN_128;
            rParam.u4SecureKeyPtr = 0;
        }
        else if(i == 2)
        {
            x_memcpy(rParam.au1Dat, au1Dat192, sizeof(au1Dat192));
            x_memcpy(au1Gol, au1Res192, sizeof(au1Res192));
            rParam.eKeyBitLen = KEY_BIT_LEN_192;
            rParam.u4SecureKeyPtr = 0;
        }
        else
        {
            x_memcpy(rParam.au1Key, au1KeyDES40, sizeof(au1KeyDES40));
            x_memcpy(rParam.au1Dat, au1Dat40, sizeof(au1Dat40));
            x_memcpy(au1Gol, au1Res40, sizeof(au1Res40));
            rParam.eKeyBitLen = KEY_BIT_LEN_40;
            rParam.u4SecureKeyPtr = 0;
        }

        GCPU_Cmd(0, GCPU_TDES_D, &rParam);

        if(x_memcmp(au1Gol, rParam.au1Res, sizeof(au1Gol)) != 0)
        {
            Printf("DES D Fail - Mode %d\n", i);
            while (1);
        }
        else
        {
            Printf("DES D Pass - Mode %d\n", i);
        }
    }
/*
    INT32 i;
    INT32 j;
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    TDES_Param_DE rDESDParam;

    HalFlushInvalidateDCache();

    for (j=0; j<4; j++)
    {
        if (j == 0)
        {
            UINT32 u464bitPatternId = 0;

            //64 bit
            for (u464bitPatternId=0; u464bitPatternId<3; u464bitPatternId++)
            {
                if (u464bitPatternId == 0)
                {
                    u4Param0[0] = 0x3798383B;
                    u4Param0[1] = 0x5EF72015;

                    u4Param1[0] = 0xE91E2E64;
                    u4Param1[1] = 0x26911E18;

                    u4Param3[0] = 0x33221100;
                    u4Param3[1] = 0x77665544;
                }
                else if (u464bitPatternId == 1)
                {
                    u4Param0[0] = 0x89674523;
                    u4Param0[1] = 0x01EFCDAB;

                    u4Param1[0] = 0x72918EA2;
                    u4Param1[1] = 0x31BA4B4C;

                    u4Param3[0] = 0xF9A72E5A;
                    u4Param3[1] = 0x3FF5A283;
                }
                else if (u464bitPatternId == 3)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xefcdab89;

                    u4Param1[0] = 0x72918EA2;
                    u4Param1[1] = 0x31BA4B4C;

                    u4Param3[0] = 0x20656854;
                    u4Param3[1] = 0x63667571;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_D, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES D Fail - Mode %d , 64bit Pattern %d \n", j, u464bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES D Pass - Mode %d, 64bit Pattern %d \n", j, u464bitPatternId);
            }
        }
        else if (j == 1)
        {
            UINT32 u4128bitPatternId = 0;

            //128 bit
            for (u4128bitPatternId=0; u4128bitPatternId<2; u4128bitPatternId++)
            {
                if (u4128bitPatternId == 0)
                {
                    u4Param0[0] = 0x3798383B;
                    u4Param0[1] = 0x5EF72015;
                    u4Param0[2] = 0x10B52F92;
                    u4Param0[3] = 0x6E431FC7;

                    u4Param1[0] = 0x87A30D56;
                    u4Param1[1] = 0x0011D16B;

                    u4Param3[0] = 0x33221100;
                    u4Param3[1] = 0x77665544;
                }
                else if (u4128bitPatternId == 1)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x33221100;
                    u4Param0[3] = 0x77665544;

                    u4Param1[0] = 0xF6D6891F;
                    u4Param1[1] = 0x6CB93FE6;

                    u4Param3[0] = 0x3C2B1A08;
                    u4Param3[1] = 0x7F6D5C4D;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_D, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES D Fail - Mode %d , 128bit Pattern %d \n", j, u4128bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES D Pass - Mode %d, 128bit Pattern %d \n", j, u4128bitPatternId);
            }
        }
        else if (j == 2)
        {
            UINT32 u4192bitPatternId = 0;

            //192 bit
            for (u4192bitPatternId=0; u4192bitPatternId<2; u4192bitPatternId++)
            {
                if (u4192bitPatternId == 0)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x89674523;
                    u4Param0[3] = 0x01EFCDAB;
                    u4Param0[4] = 0xAB896745;
                    u4Param0[5] = 0x2301EFCD;

                    u4Param1[0] = 0x8CFD26A8;
                    u4Param1[1] = 0x5F853BE5;

                    u4Param3[0] = 0x20656854;
                    u4Param3[1] = 0x63667571;
                }
                else if (u4192bitPatternId == 1)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x33221100;
                    u4Param0[3] = 0x77665544;
                    u4Param0[4] = 0xBBAA9988;
                    u4Param0[5] = 0xFFEEDDCC;

                    u4Param1[0] = 0xD2095B03;
                    u4Param1[1] = 0xD2BE29CF;

                    u4Param3[0] = 0x3C2B1A08;
                    u4Param3[1] = 0x7F6D5C4D;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_D, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES D Fail - Mode %d , 192bit Pattern %d \n", j, u4192bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES D Pass - Mode %d, 192bit Pattern %d \n", j, u4192bitPatternId);
            }
        }
        else
        {
            UINT32 u440bitPatternId = 0;

            //40 bit
            for (u440bitPatternId=0; u440bitPatternId<3; u440bitPatternId++)
            {
                if (u440bitPatternId == 0)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0x54321089;

                    u4Param1[0] = 0x2D92747D;
                    u4Param1[1] = 0x132EB174;

                    u4Param3[0] = 0x67452301;
                    u4Param3[1] = 0xEFCDAB89;
                }
                else if (u440bitPatternId == 1)
                {
                    u4Param0[0] = 0x00000000;
                    u4Param0[1] = 0x00000000;

                    u4Param1[0] = 0x2AD7B179;
                    u4Param1[1] = 0x04D277D8;

                    u4Param3[0] = 0x67452301;
                    u4Param3[1] = 0xEFCDAB89;
                }
                else if (u440bitPatternId == 2)
                {
                    u4Param0[0] = 0xFFFFFFFF;
                    u4Param0[1] = 0xFFFFFFFF;

                    u4Param1[0] = 0xE88ECC12;
                    u4Param1[1] = 0x8063683C;

                    u4Param3[0] = 0x67452301;
                    u4Param3[1] = 0xEFCDAB89;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_D, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES D Fail - Mode %d , 40bit Pattern %d \n", j, u440bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES D Pass - Mode %d, 40bit Pattern %d \n", j, u440bitPatternId);
            }
        }
    }
*/
    return 0;
}

INT32 GCPU_EMU_DESE()
{
    SIM_DES_PARAM_T rParam;
    UINT8 au1Gol[8];
    INT32 i;

    for (i = 0; i < 4; i++)
    {
        x_memcpy(rParam.au1Key, au1KeyDES, sizeof(au1KeyDES));

        if(i == 0)
        {
            x_memcpy(rParam.au1Dat, au1Res64, sizeof(au1Res64));
            x_memcpy(au1Gol, au1Dat64, sizeof(au1Dat64));
            rParam.eKeyBitLen = KEY_BIT_LEN_64;
            rParam.u4SecureKeyPtr = 0;
        }
        else if(i == 1)
        {
            x_memcpy(rParam.au1Dat, au1Res128, sizeof(au1Res128));
            x_memcpy(au1Gol, au1Dat128, sizeof(au1Dat128));
            rParam.eKeyBitLen = KEY_BIT_LEN_128;
            rParam.u4SecureKeyPtr = 0;
        }
        else if(i == 2)
        {
            x_memcpy(rParam.au1Dat, au1Res192, sizeof(au1Res192));
            x_memcpy(au1Gol, au1Dat192, sizeof(au1Dat192));
            rParam.eKeyBitLen = KEY_BIT_LEN_192;
            rParam.u4SecureKeyPtr = 0;
        }
        else
        {
            x_memcpy(rParam.au1Key, au1KeyDES40, sizeof(au1KeyDES40));
            x_memcpy(rParam.au1Dat, au1Res40, sizeof(au1Res40));
            x_memcpy(au1Gol, au1Dat40, sizeof(au1Dat40));
            rParam.eKeyBitLen = KEY_BIT_LEN_40;
            rParam.u4SecureKeyPtr = 0;
        }

        GCPU_Cmd(0, GCPU_TDES_E, &rParam);

        if(x_memcmp(au1Gol, rParam.au1Res, sizeof(au1Gol)) != 0)
        {
            Printf("DES D Fail - Mode %d\n", i);
            while (1);
        }
        else
        {
            Printf("DES D Pass - Mode %d\n", i);
        }
    }

    return 0;
/*
    INT32 i;
    INT32 j;
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    TDES_Param_DE rDESDParam;

    HalFlushInvalidateDCache();

    for (j=0; j<4; j++)
    {
        if (j == 0)
        {
            UINT32 u464bitPatternId = 0;

            //64 bit
            for (u464bitPatternId=0; u464bitPatternId<3; u464bitPatternId++)
            {
                if (u464bitPatternId == 0)
                {
                    u4Param0[0] = 0x3798383B;
                    u4Param0[1] = 0x5EF72015;
                    u4Param3[0] = 0xE91E2E64;
                    u4Param3[1] = 0x26911E18;

                    u4Param1[0] = 0x33221100;
                    u4Param1[1] = 0x77665544;
                }
                else if (u464bitPatternId == 1)
                {
                    u4Param0[0] = 0x89674523;
                    u4Param0[1] = 0x01EFCDAB;
                    u4Param3[0] = 0x72918EA2;
                    u4Param3[1] = 0x31BA4B4C;

                    u4Param1[0] = 0xF9A72E5A;
                    u4Param1[1] = 0x3FF5A283;
                }
                else if (u464bitPatternId == 2)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xefcdab89;
                    u4Param3[0] = 0x72918EA2;
                    u4Param3[1] = 0x31BA4B4C;

                    u4Param1[0] = 0x20656854;
                    u4Param1[1] = 0x63667571;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_E, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES E Fail - Mode %d, 64bit Pattern %d \n", j, u464bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES E Pass - Mode %d, 64bit Pattern %d \n", j, u464bitPatternId);
            }
        }
        else if (j == 1)
        {
            UINT32 u4128bitPatternId = 0;

            //128 bit
            for (u4128bitPatternId=0; u4128bitPatternId<2; u4128bitPatternId++)
            {
                if (u4128bitPatternId == 0)
                {
                    u4Param0[0] = 0x3798383B;
                    u4Param0[1] = 0x5EF72015;
                    u4Param0[2] = 0x10B52F92;
                    u4Param0[3] = 0x6E431FC7;

                    u4Param3[0] = 0x87A30D56;
                    u4Param3[1] = 0x0011D16B;

                    u4Param1[0] = 0x33221100;
                    u4Param1[1] = 0x77665544;
                }
                else if (u4128bitPatternId == 1)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x33221100;
                    u4Param0[3] = 0x77665544;

                    u4Param3[0] = 0xF6D6891F;
                    u4Param3[1] = 0x6CB93FE6;

                    u4Param1[0] = 0x3C2B1A08;
                    u4Param1[1] = 0x7F6D5C4D;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_E, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES E Fail - Mode %d, 128bit Pattern %d \n", j, u4128bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES E Pass - Mode %d, 128bit Pattern %d \n", j, u4128bitPatternId);
            }
        }
        else if (j == 2)
        {
            UINT32 u4192bitPatternId = 0;

            //192 bit
            for (u4192bitPatternId=0; u4192bitPatternId<2; u4192bitPatternId++)
            {
                if (u4192bitPatternId == 0)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x89674523;
                    u4Param0[3] = 0x01EFCDAB;
                    u4Param0[4] = 0xAB896745;
                    u4Param0[5] = 0x2301EFCD;

                    u4Param3[0] = 0x8CFD26A8;
                    u4Param3[1] = 0x5F853BE5;

                    u4Param1[0] = 0x20656854;
                    u4Param1[1] = 0x63667571;
                }
                else if (u4192bitPatternId == 1)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0xEFCDAB89;
                    u4Param0[2] = 0x33221100;
                    u4Param0[3] = 0x77665544;
                    u4Param0[4] = 0xBBAA9988;
                    u4Param0[5] = 0xFFEEDDCC;

                    u4Param3[0] = 0xD2095B03;
                    u4Param3[1] = 0xD2BE29CF;

                    u4Param1[0] = 0x3C2B1A08;
                    u4Param1[1] = 0x7F6D5C4D;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_E, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES E Fail - Mode %d, 192bit Pattern %d \n", j, u4192bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES E Pass - Mode %d, 192bit Pattern %d \n", j, u4192bitPatternId);
            }
        }
        else
        {
            UINT32 u440bitPatternId = 0;

            //40 bit
            for (u440bitPatternId=0; u440bitPatternId<3; u440bitPatternId++)
            {
                if (u440bitPatternId == 0)
                {
                    u4Param0[0] = 0x67452301;
                    u4Param0[1] = 0x54321089;

                    u4Param3[0] = 0x2D92747D;
                    u4Param3[1] = 0x132EB174;

                    u4Param1[0] = 0x67452301;
                    u4Param1[1] = 0xEFCDAB89;
                }
                else if (u440bitPatternId == 1)
                {
                    u4Param0[0] = 0x00000000;
                    u4Param0[1] = 0x00000000;

                    u4Param3[0] = 0x2AD7B179;
                    u4Param3[1] = 0x04D277D8;

                    u4Param1[0] = 0x67452301;
                    u4Param1[1] = 0xEFCDAB89;
                }
                else if (u440bitPatternId == 2)
                {
                    u4Param0[0] = 0xFFFFFFFF;
                    u4Param0[1] = 0xFFFFFFFF;

                    u4Param3[0] = 0xE88ECC12;
                    u4Param3[1] = 0x8063683C;

                    u4Param1[0] = 0x67452301;
                    u4Param1[1] = 0xEFCDAB89;
                }

                rDESDParam.pbKey = (BYTE*) &u4Param0;
                rDESDParam.pbData = (BYTE*) &u4Param1;
                rDESDParam.pbResult = (BYTE*) &u4Param2;
                rDESDParam.u2KeySlotHandle = 4;
                rDESDParam.u2DatSlotHandle = 10;
                rDESDParam.u2ResSlotHandle = 12;
                rDESDParam.uFlag = 0;
                rDESDParam.uKeyLen = j;

                _i4GCPU_DES_CmdHandle(0, GCPU_TDES_E, &rDESDParam);

                for (i=0; i<2; i++)
                {
                    if (u4Param2[i] != u4Param3[i])
                    {
                        UTIL_Printf("DES E Fail - Mode %d, 40bit Pattern %d \n", j, u440bitPatternId);
                        while (1);
                    }
                }
                UTIL_Printf("DES E Pass - Mode %d, 40bit Pattern %d \n", j, u440bitPatternId);
            }
        }
    }

    return 0;
*/
}

static UINT8 au1Key[24] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

INT32 GCPU_EMU_DESDMAD()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    DES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //64 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_64_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_64_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_64_raw, sizeof(au1desdma_64_raw));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_64;
        }
        else if (i == 1)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_128_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_128_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_128_raw, sizeof(au1desdma_128_raw));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 2)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_192_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_192_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_192_raw, sizeof(au1desdma_192_raw));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else
        {
            //40 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_40_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_40_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_40_raw, sizeof(au1desdma_40_raw));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_40;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_ECB;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_TDES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("DES DMAD Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("DES DMAD Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}

INT32 GCPU_EMU_DESDMAE()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    DES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //64 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_64_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_64_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_64_enc, sizeof(au1desdma_64_enc));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_64;
        }
        else if (i == 1)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_128_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_128_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_128_enc, sizeof(au1desdma_128_enc));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 2)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_192_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_192_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_192_enc, sizeof(au1desdma_192_enc));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else
        {
            //40 Bit Mode
            u4GoldenFileSize = sizeof(au1desdma_40_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1desdma_40_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1desdma_40_enc, sizeof(au1desdma_40_enc));

            x_memcpy(rAesParam.au1Key, au1Key, sizeof(au1Key));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_40;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_ECB;
        rAesParam.fgEncrypt = TRUE;

        GCPU_Cmd(0, GCPU_TDES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("DES DMAE Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("DES DMAE Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}

static UINT8 au1CbcKey[24] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                    0xf1, 0xe0, 0xd3, 0xc2, 0xb5, 0xa4, 0x97, 0x86,
                    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

static UINT8 au1CbcIv[8] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};


INT32 GCPU_EMU_DESCBCD()
{
    UINT8 i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    DES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //64 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_64_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_64_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_64_raw, sizeof(au1descbc_64_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_64;
        }
        else if (i == 1)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_128_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_128_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_128_raw, sizeof(au1descbc_128_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 2)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_192_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_192_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_192_raw, sizeof(au1descbc_192_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else
        {
            //40 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_40_raw);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_40_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_40_raw, sizeof(au1descbc_40_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_40;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_TDES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("DES CBCD Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("DES CBCD Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}

INT32 GCPU_EMU_DESCBCE()
{
    UINT8 i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    DES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //64 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_64_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_64_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_64_enc, sizeof(au1descbc_64_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_64;
        }
        else if (i == 1)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_128_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_128_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_128_enc, sizeof(au1descbc_128_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 2)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_192_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_192_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_192_enc, sizeof(au1descbc_192_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else
        {
            //40 Bit Mode
            u4GoldenFileSize = sizeof(au1descbc_40_enc);
            u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
            u4SrcOffset = rand() % u4BufSize;
            u4DstOffset = rand() % u4BufSize;
            pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
            if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
            {
                ASSERT(0);
            }
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1descbc_40_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1descbc_40_enc, sizeof(au1descbc_40_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey, sizeof(au1CbcKey));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_40;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
        rAesParam.fgEncrypt = TRUE;

        GCPU_Cmd(0, GCPU_TDES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("DES CBCE Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("DES CBCE Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}
