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
#define NETFLIX_OWNER             "be67c762b0a3"
#define NETFLIX_KPE               "6f6c838df998"
#define NETFLIX_KPH               "363a61c725eb"

/* Playready related */
#define PLAYREADY_OWNER1          "10d93c1ccdb3"
#define PLAYREADY_DEVCERT_TEMP    "01614c91b2b0"
#define PLAYREADY_PRIV            "bcf6d7739c8f"
#define PLAYREADY_OWNER2          "aed63bd1150f"
#define PLAYREADY_BGROUP_CERT     "c173e5f87397"
#define PLAYREADY_ZGPRIV          "82758001af66"

/* Amazon related */
#define AMAZON_OWNER              "77a458b3b04a"
#define AMAZON_CRT                "ea686dbcba15"
#define AMAZON_KEY                "430697729883"

/* Rhapsody related */
#define RHAPSODY_OWNER            "569e6fdba176"
#define RHAPSODY_CRT              "208945d8fced"
#define RHAPSODY_KEY              "9f15131c5721"

/* Widevine */
#define WIDEVINE_OWNER            "f9073100d093"
#define WIDEVINE_KEYBOX           "48be3700035b"

/* pre-shared secret for checking POTK */
#define HAVE_PRE_SHARED_SECRET    1

UINT8    g_au1_pre_shared_secret[16] = {0x63, 0x81, 0xe6, 0xb8, 0xc5, 0xd8, 0x2f, 0x1d,
                                        0xb1, 0x64, 0xa0, 0x05, 0x3f, 0x56, 0xf0, 0xa3};
