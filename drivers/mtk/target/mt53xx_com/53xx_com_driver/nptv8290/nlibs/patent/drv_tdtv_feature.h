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
 * $RCSfile: drv_display.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_display.c
 *  Brief of file drv_display.c.
 *  Details of file drv_display.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drv_tdtv.h"

#if !defined(CC_MEM_TOOL)

//3D convert configuration
//#define CC_3D_CONVERT_BY_MIB_FS
//#define CC_3D_CONVERT_BY_MIB_TB
//#define CC_3D_CONVERT_BY_MIB_LI
//#define CC_3D_CONVERT_BY_SCL
//#define CC_3D_OVERSCAN_ONLY
//#if !defined(CC_3D_CONVERT_BY_MIB_TB) && !defined(CC_3D_CONVERT_BY_MIB_LI) && defined(CC_MT5399)
//#define CC_3D_SUPPORT_DUAL_FPI
//#endif


//#define CC_SUPPORT_TDNAVI_CB        
//#define SUPPORT_CUSTOMIZED_3D_CFG

#if !defined(ANDROID)
#define CC_SUPPORT_OSD_DEPTH
#endif

#define CC_3DTV_FPR_SUPPORT
#define CC_SUPPORT_3D_DEPTH_CTRL
#define CC_3D_SUPPORT_LI_DA        
#define CC_3D_SUPPORT_IMG_SAFETY
//Default Configuration
#define SUPPORT_DUAL_DTV (1)

#ifdef CC_SUPPORT_OSD_DEPTH
#define SUPPORT_OSD_DEPTH (1)
#else
#define SUPPORT_OSD_DEPTH (0)
#endif

#ifndef SUPPORT_FS_FREERUN
#define SUPPORT_FS_FREERUN (0)
#endif

#ifndef SUPPORT_3D_EXT_COMP_TRL
#define SUPPORT_3D_EXT_COMP_TRL (0)
#endif

#ifndef SUPPORT_3D_SG_CTRL
#define SUPPORT_3D_SG_CTRL (0)
#endif

#ifndef SUPPORT_3D_BL_CTRL
#define SUPPORT_3D_BL_CTRL (0)
#endif

#ifndef SUPPORT_3D_WITHOUT_MJC
#define SUPPORT_3D_WITHOUT_MJC (0)
#endif

#ifndef SUPPORT_3D_EMBEDED_OSD
#define SUPPORT_3D_EMBEDED_OSD (0)
#endif

#ifndef SUPPORT_3D_ANALOG_SRC
#define SUPPORT_3D_ANALOG_SRC (1)
#endif

#ifndef SUPPORT_PSEUDO_MVC
#define SUPPORT_PSEUDO_MVC (0)
#endif

#ifndef SUPPORT_3D_NAVI
#define SUPPORT_3D_NAVI (1)
#endif


#ifndef SUPPORT_3D_NAVI_V2
#define SUPPORT_3D_NAVI_V2 (0)
#endif


#ifndef SUPPORT_REALD_TAG
#define SUPPORT_REALD_TAG (0)
#endif

#ifndef SUPPORT_SW_NAVI
#ifdef CC_SUPPORT_SW_NAVI_PLUS
#define SUPPORT_SW_NAVI (1)
#else
#define SUPPORT_SW_NAVI (0)
#endif
#endif

#ifndef SUPPORT_3D_EPG
#define SUPPORT_3D_EPG  (0)
#endif             

#ifdef CC_SUPPORT_3D_DEPTH_CTRL
#define SUPPORT_3D_DEPTH_CONTROL  (1)
#else
#define SUPPORT_3D_DEPTH_CONTROL  (0)
#endif

#ifndef SUPPORT_120HZ_3DFPR
#define SUPPORT_120HZ_3DFPR  (0)
#endif

#else

#define SUPPORT_FS_FREERUN (0)
#define SUPPORT_3D_EXT_COMP_TRL (0)
#define SUPPORT_3D_SG_CTRL (0)
#define SUPPORT_3D_BL_CTRL (0)
#define SUPPORT_3D_WITHOUT_MJC (0)
#define SUPPORT_3D_EMBEDED_OSD (0)
#define SUPPORT_3D_ANALOG_SRC (0)
#define SUPPORT_PSEUDO_MVC (0)
#define SUPPORT_OSD_DEPTH (0)
#define SUPPORT_3D_NAVI (0)
#define SUPPORT_REALD_TAG (0)
#define SUPPORT_SW_NAVI (0)
#define SUPPORT_3D_EPG (0)
#define SUPPORT_3D_DEPTH_CONTROL (0)
#define SUPPORT_3D_NAVI_V2 (0)

#endif
