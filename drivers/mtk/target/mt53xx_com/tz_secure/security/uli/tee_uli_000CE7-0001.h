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
#define NETFLIX_OWNER             "b59bf8199378"
#define NETFLIX_KPE               "53e10d1a8b4d"
#define NETFLIX_KPH               "b1d0712ce0ef"

/* Playready related */
#define PLAYREADY_OWNER1          "112fc165ac04"
#define PLAYREADY_DEVCERT_TEMP    "2a867aeb1e9e"
#define PLAYREADY_PRIV            "bef043f37783"
#define PLAYREADY_OWNER2          "1e3cd24e2dd3"
#define PLAYREADY_BGROUP_CERT     "dfee38130a64"
#define PLAYREADY_ZGPRIV          "56b886fb3c3f"

/* Amazon related */
#define AMAZON_OWNER              "26e5d3e41970"
#define AMAZON_CRT                "8882dd8e02d1"
#define AMAZON_KEY                "5b1fee3f6757"

/* Rhapsody related */
#define RHAPSODY_OWNER            "292c2e26819e"
#define RHAPSODY_CRT              "0233574e3f88"
#define RHAPSODY_KEY              "d54ab1d78dd3"

/* Widevine */
#define WIDEVINE_OWNER            "acbd89fcbe23"
#define WIDEVINE_KEYBOX           "202ca73a5fb1"

