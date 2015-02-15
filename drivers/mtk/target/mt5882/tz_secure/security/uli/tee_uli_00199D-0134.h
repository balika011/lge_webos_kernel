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
 * Copyright (c) 2012, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tee_securestorage.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

///-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

/* Netflix related */
#define NETFLIX_OWNER             "9042ed013afe"
#define NETFLIX_KPE               "b945ca44cd4d"
#define NETFLIX_KPH               "69ff7a603114"

/* Playready related */
#define PLAYREADY_OWNER1          "56b2fd061b5f"
#define PLAYREADY_DEVCERT_TEMP    "ac34b4fbe435"
#define PLAYREADY_PRIV            "92ad1b03d73d"
#define PLAYREADY_OWNER2          "da2978b16635"
#define PLAYREADY_BGROUP_CERT     "bee41a898f5d"
#define PLAYREADY_ZGPRIV          "01163cc979c9"

/* Amazon related */
#define AMAZON_OWNER              "89a0a9a7a5ba"
#define AMAZON_CRT                "a47c9df8fbbd"
#define AMAZON_KEY                "4dfb171855d2"

/* Rhapsody related */
#define RHAPSODY_OWNER            "62af4feee221"
#define RHAPSODY_CRT              "7057ad062ba8"
#define RHAPSODY_KEY              "c4d0fa24ee47"

/* Widevine */
#define WIDEVINE_OWNER            "e540c709723c"
#define WIDEVINE_KEYBOX           "2f177ced414a"

/* pre-shared secret for checking POTK */
#define HAVE_PRE_SHARED_SECRET    1

UINT8    g_au1_pre_shared_secret[16] = {0x63, 0x81, 0xe6, 0xb8, 0xc5, 0xd8, 0x2f, 0x1d,
                                        0xb1, 0x64, 0xa0, 0x05, 0x3f, 0x56, 0xf0, 0xa3};
