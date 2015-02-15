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
#define NETFLIX_OWNER             "906bb3641270"
#define NETFLIX_KPE               "ae2b3b48f9b7"
#define NETFLIX_KPH               "245480055045"

/* Playready related */
#define PLAYREADY_OWNER1          "727427f513a6"
#define PLAYREADY_DEVCERT_TEMP    "b1c540d9c0bc"
#define PLAYREADY_PRIV            "19ab8ab016c7"
#define PLAYREADY_OWNER2          "39e1f1b41e2a"
#define PLAYREADY_BGROUP_CERT     "1577186508ff"
#define PLAYREADY_ZGPRIV          "3aa6c5c06074"

/* Amazon related */
#define AMAZON_OWNER              "25ef86bc5538"
#define AMAZON_CRT                "53ce27f7ff2e"
#define AMAZON_KEY                "c9d719d16a07"

/* Rhapsody related */
#define RHAPSODY_OWNER            "7aace176eaf7"
#define RHAPSODY_CRT              "551025a2a827"
#define RHAPSODY_KEY              "1dbd95c7c5cc"

/* Widevine */
#define WIDEVINE_OWNER            "6df67d850bed"
#define WIDEVINE_KEYBOX           "f28a5a830745"

/* pre-shared secret for checking POTK */
#define HAVE_PRE_SHARED_SECRET    1

UINT8    g_au1_pre_shared_secret[16] = {0x63, 0x81, 0xe6, 0xb8, 0xc5, 0xd8, 0x2f, 0x1d,
                                        0xb1, 0x64, 0xa0, 0x05, 0x3f, 0x56, 0xf0, 0xa3};
