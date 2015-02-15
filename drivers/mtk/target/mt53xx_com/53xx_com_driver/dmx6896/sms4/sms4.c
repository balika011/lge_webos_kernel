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
/* sms4.c
** SMS4 Encryption algorithm for wireless networks
**
** $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/53xx_com_driver/dmx6896/sms4/sms4.c#1 $">emhmily@gmail.com>$
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc.
**/

/* define SMS4DBG flag for debug these code */
#define SMS4DBG

#ifdef SMS4DBG
//#include
#endif /* SMS4DBG */

//#include <stdio.h>
#include "dmx_drm_drvif.h"
#include "sms4.h"

/* reserved for saving interface */
/*#include "sms4.h"*/

//#ifndef unlong
//typedef unsigned long unlong;
//#endif /* unlong */

//#ifndef unchar
//typedef unsigned char unchar;
//#endif /* unchar */

/* define SMS4CROL for rotating left */
#define SMS4CROL(uval, bits) ((uval << bits) | (uval >> (0x20 - bits)))

/* define MASK code for selecting expected bits from a 32 bits value */
#define SMS4MASK3 0xFF000000
#define SMS4MASK2 0x00FF0000
#define SMS4MASK1 0x0000FF00
#define SMS4MASK0 0x000000FF

/* Sbox table: 8bits input convert to 8 bits output*/
static unsigned char SboxTable[16][16] =
{
{0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05},
{0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99},
{0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62},
{0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6},
{0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8},
{0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35},
{0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87},
{0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e},
{0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1},
{0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3},
{0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f},
{0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51},
{0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8},
{0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0},
{0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84},
{0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48}
};

/* Encryption key: 128bits */
static unsigned long MK[4] = {0x01234567,0x89abcdef,0xfedcba98,0x76543210};

/* System parameter */
static unsigned long FK[4] = {0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc};

/* fixed parameter */
static unsigned long CK[32] =
{
0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};

/* buffer for round encryption key */
static unsigned long ENRK[32];
static unsigned long DERK[32];

#ifdef SMS4DBG
/* original contents for debugging */
unsigned long pData[4] =
{
0x01234567,
0x89abcdef,
0xfedcba98,
0x76543210
};

/* original contents for debugging */
unsigned long pData2[9] =
{
0x01234567,
0x89abcdef,
0xfedcba98,
0x76543210,
0x12121212,
0x34343434,
0x56565656,
0x78787878,
0x12341234
};
#endif /* SMS4DBG */

/*=============================================================================
** private function:
**   look up in SboxTable and get the related value.
** args:    [in] inch: 0x00~0xFF (8 bits unsigned value).
**============================================================================*/
static unsigned char SMS4Sbox(unsigned char inch)
{
    unsigned char *pTable = (unsigned char *)SboxTable;
    unsigned char retVal = (unsigned char)(pTable[inch]);

    return retVal;
}

/*=============================================================================
** private function:
**   "T algorithm" == "L algorithm" + "t algorithm".
** args:    [in] a: a is a 32 bits unsigned value;
** return: c: c is calculated with line algorithm "L" and nonline algorithm "t"
**============================================================================*/
static unsigned long SMS4Lt(unsigned long a)
{
    unsigned long b = 0;
    unsigned long c = 0;
    unsigned char a0 = (unsigned char)(a & SMS4MASK0);
    unsigned char a1 = (unsigned char)((a & SMS4MASK1) >> 8);
    unsigned char a2 = (unsigned char)((a & SMS4MASK2) >> 16);
    unsigned char a3 = (unsigned char)((a & SMS4MASK3) >> 24);
    unsigned char b0 = SMS4Sbox(a0);
    unsigned char b1 = SMS4Sbox(a1);
    unsigned char b2 = SMS4Sbox(a2);
    unsigned char b3 = SMS4Sbox(a3);

    b =b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    c =b^(SMS4CROL(b, 2))^(SMS4CROL(b, 10))^(SMS4CROL(b, 18))^(SMS4CROL(b, 24));

    return c;
}

/*=============================================================================
** private function:
**   Calculating round encryption key.
** args:    [in] a: a is a 32 bits unsigned value;
** return: ENRK[i]: i{0,1,2,3,...31}.
**============================================================================*/
static unsigned long SMS4CalciRK(unsigned long a)
{
    unsigned long b = 0;
    unsigned long rk = 0;
    unsigned char a0 = (unsigned char)(a & SMS4MASK0);
    unsigned char a1 = (unsigned char)((a & SMS4MASK1) >> 8);
    unsigned char a2 = (unsigned char)((a & SMS4MASK2) >> 16);
    unsigned char a3 = (unsigned char)((a & SMS4MASK3) >> 24);
    unsigned char b0 = SMS4Sbox(a0);
    unsigned char b1 = SMS4Sbox(a1);
    unsigned char b2 = SMS4Sbox(a2);
    unsigned char b3 = SMS4Sbox(a3);

    b = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    rk = b^(SMS4CROL(b, 13))^(SMS4CROL(b, 23));

    return rk;
}

/*=============================================================================
** private function:
**   Calculating round encryption key.
** args:    [in] ulflag: if 0: not calculate DERK , else calculate;
** return: NONE.
**============================================================================*/
static void SMS4CalcRK(unsigned long ulflag)
{
    unsigned long k[36];
    unsigned long i = 0;

    k[0] = MK[0]^FK[0];
    k[1] = MK[1]^FK[1];
    k[2] = MK[2]^FK[2];
    k[3] = MK[3]^FK[3];

    for(; i<32; i++)
    {
        k[i+4] = k[i] ^ (SMS4CalciRK(k[i+1]^k[i+2]^k[i+3]^CK[i]));
        ENRK[i] = k[i+4];
    }

    if (ulflag != 0x00)
    {
        for (i=0; i<32; i++)
        {
            DERK[i] = ENRK[31-i];
        }
    }
}

/*=============================================================================
** private function:
**   "T algorithm" == "L algorithm" + "t algorithm".
** args:    [in] a: a is a 32 bits unsigned value.
**============================================================================*/
static unsigned long SMS4T(unsigned long a)
{
    return (SMS4Lt(a));
}

/*=============================================================================
** private function:
**   Calculating and getting encryption/decryption contents.
** args:    [in] x0: original contents;
** args:    [in] x1: original contents;
** args:    [in] x2: original contents;
** args:    [in] x3: original contents;
** args:    [in] rk: encryption/decryption key;
** return the contents of encryption/decryption contents.
**============================================================================*/
static unsigned long SMS4F(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long rk)
{
    return (x0^SMS4Lt(x1^x2^x3^rk));
}

/*=============================================================================
** public function:
**   "T algorithm" == "L algorithm" + "t algorithm".
** args:    [in] ulkey: password defined by user(NULL: default encryption key);
** args:    [in] flag: if 0: not calculate DERK , else calculate;
** return ulkey: NULL for default encryption key.
**============================================================================*/
unsigned long *SMS4SetKey(unsigned long *ulkey, unsigned long flag)
{
    if (ulkey != NULL)
    {
        x_memcpy(MK, ulkey, sizeof(MK));
    }

    SMS4CalcRK(flag);

    return ulkey;
}

/*=============================================================================
** public function:
**   sms4 encryption algorithm.
** args:   [in/out] psrc: a pointer point to original contents;
** args:   [in] lgsrc: the length of original contents;
** args:   [in] derk: a pointer point to encryption/decryption key;
** return: pRet: a pointer point to encrypted contents.
**============================================================================*/
unsigned long *SMS4Encrypt(unsigned long *psrc, unsigned long lgsrc, unsigned long rk[])
{
    unsigned long *pRet = NULL;
    unsigned long i = 0;
   
    unsigned long ulbuf[36];

    unsigned long ulCnter = 0;
    unsigned long ulTotal = (lgsrc >> 4);

   
    if(psrc != NULL)
    {
        pRet = psrc;
       
        /* !!!It's a temporary scheme: start!!! */
        /*========================================
        ** 16 bytes(128 bits) is deemed as an unit.
        **======================================*/
        while (ulCnter<ulTotal)
        {
            /* reset number counter */
            i = 0;

            /* filled up with 0*/
            x_memset(ulbuf, 0, sizeof(ulbuf));
            x_memcpy(ulbuf, psrc, 16);
           
            while(i<32)
            {
                ulbuf[i+4] = SMS4F(ulbuf[i], ulbuf[i+1],
                                   ulbuf[i+2], ulbuf[i+3], rk[i]);
                i++;
            }

            /* save encrypted contents to original area */
            psrc[0] = ulbuf[35];
            psrc[1] = ulbuf[34];
            psrc[2] = ulbuf[33];
            psrc[3] = ulbuf[32];

            ulCnter++;
            psrc += 4;
        }
        /* !!!It's a temporary scheme: end!!! */
    }

    return pRet;
}

/*=============================================================================
** public function:
**   sms4 decryption algorithm.
** args:   [in/out] psrc: a pointer point to encrypted contents;
** args:   [in] lgsrc: the length of encrypted contents;
** args:   [in] derk: a pointer point to decryption key;
** return: pRet: a pointer point to decrypted contents.
**============================================================================*/
unsigned long *SMS4Decrypt(unsigned long *psrc, unsigned long lgsrc, unsigned long derk[])
{
    unsigned long *pRet = NULL;
    unsigned long i = 0;

    if(psrc != NULL)
    {
        pRet = psrc;

        /* the same arithmetic, different encryption key sequence. */
        SMS4Encrypt(psrc, lgsrc, derk);
    }
   
    return pRet;
}


void SMS4Encrypt_ecb(unsigned long *psrc)
{
    SMS4Encrypt(psrc, 16, ENRK);
}

void SMS4Encrypt_cbc(unsigned long *psrc, unsigned long *piv)
{
    UINT8 i;

    for(i=0; i<4; i++)
    {
        psrc[i] = psrc[i] ^ piv[i];
    }
    
    SMS4Encrypt(psrc, 16, ENRK);

    x_memcpy(piv, psrc, 16);
}

void SMS4Encrypt_ofb(unsigned long *psrc, unsigned long *piv)
{
    UINT8 i;

    SMS4Encrypt(piv, 16, ENRK);

    for(i=0; i<4; i++)
    {
        psrc[i] = psrc[i] ^ piv[i];
    }
}

void SMS4Encrypt_cfb(unsigned long *psrc, unsigned long *piv)
{
    UINT8 i;

    SMS4Encrypt(piv, 16, ENRK);

    for(i=0; i<4; i++)
    {
        psrc[i] = psrc[i] ^ piv[i];
    }

    x_memcpy(piv, psrc, 16);
}


void SMS4Encrypt_scte52(unsigned long *psrc, unsigned long *piv)
{
    INT32 i;
    
    SMS4Encrypt(piv, 16, ENRK);
    for( i = 0; i < 4; i++ )
    {
        psrc[i] = psrc[i] ^ piv[i];
    }
}


unsigned long *SMS4DoDecrypt(unsigned long *psrc, unsigned long lgsrc)
{
    return SMS4Encrypt(psrc, lgsrc, DERK);
}


#ifdef SMS4DBG

/* entry-point for debugging */
#if 0
int SMSTest(void)
{
    SMS4SetKey(NULL, 1);

    /* cycle1: common test */
    SMS4Encrypt(pData, sizeof(pData), ENRK);
    SMS4Decrypt(pData, sizeof(pData), DERK);

    /* cycle3: longer contents */
    SMS4Encrypt(pData2, sizeof(pData2), ENRK);
    SMS4Decrypt(pData2, sizeof(pData2), DERK);

    return 0;
}
#endif
#endif /* SMS4DBG */
