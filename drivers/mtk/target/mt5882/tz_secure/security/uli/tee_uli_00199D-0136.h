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
#define NETFLIX_OWNER             "dad74430c9d4"
#define NETFLIX_KPE               "f1ae83cfa950"
#define NETFLIX_KPH               "e2af34acd1c1"

/* Playready related */
#define PLAYREADY_OWNER1          "7395b79d7e09"
#define PLAYREADY_DEVCERT_TEMP    "c55a8ced79cc"
#define PLAYREADY_PRIV            "948c144d50b7"
#define PLAYREADY_OWNER2          "db9272b069c3"
#define PLAYREADY_BGROUP_CERT     "ff6bc1ddc838"
#define PLAYREADY_ZGPRIV          "9ad9faea0d15"

/* Amazon related */
#define AMAZON_OWNER              "8acdfd2bdead"
#define AMAZON_CRT                "acf3d06abece"
#define AMAZON_KEY                "6c21d7b745f0"

/* Rhapsody related */
#define RHAPSODY_OWNER            "0cad03ab15e1"
#define RHAPSODY_CRT              "f962834c92b9"
#define RHAPSODY_KEY              "3beafe583242"

/* Widevine */
#define WIDEVINE_OWNER            "330d587713e8"
#define WIDEVINE_KEYBOX           "a5c1934b5c05"

/* pre-shared secret for checking POTK */
#define HAVE_PRE_SHARED_SECRET    1

UINT8    g_au1_pre_shared_secret[16] = {0x63, 0x81, 0xe6, 0xb8, 0xc5, 0xd8, 0x2f, 0x1d,
                                        0xb1, 0x64, 0xa0, 0x05, 0x3f, 0x56, 0xf0, 0xa3};
