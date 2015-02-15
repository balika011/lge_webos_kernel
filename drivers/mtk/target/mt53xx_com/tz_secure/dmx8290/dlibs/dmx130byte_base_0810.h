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
 * $RCSfile: $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_imem.h
 *  Demux driver - uCode inclusion
 */

#ifndef DMX_IMEM_H
#define DMX_IMEM_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

static const UINT32 FTI_IMEM[] = {
0x240100fa, 0x4427f80a, 0x24150001, 0x24160002, 0x240affff, 
0x4405a00a, 0x4541400a, 0xd8010000, 0x24000000, 0x24000000, 
0x24000000, 0x24000000, 0x24000000, 0x24000000, 0x46816009, 
0x01814a70, 0x74200002, 0x08000007, 0x24010008, 0x4427f80a, 
0x4407f80a, 0x4407300a, 0x4661a009, 0x08000009, 0x24010008, 
0x4427f80a, 0x4407f80a, 0x4407300a, 0x01814230, 0x6820001a, 
0x44216009, 0x4401a009, 0x4401180a, 0x4420d809, 0x337b00ff, 
0x081f0016, 0x24190000, 0x27610000, 0x081f0032, 0x46c3700a, 
0xdc030000, 0x45e31009, 0x30420003, 0x68400004, 0x4584080a, 
0x4444000a, 0x08000009, 0x45230809, 0x45431009, 0xa6810020, 
0xa6820022, 0x4424200a, 0x4444280a, 0x4584180a, 0x46a4100a, 
0x4401200a, 0x0800ffcf, 0x86880014, 0x86890016, 0x4503180a, 
0x4523200a, 0x86880018, 0x8689001a, 0x4503280a, 0x4523300a, 
0x081e0006, 0x86880020, 0x86890022, 0x4503480a, 0x4523500a, 
0x1fe00000, 0x4420580a, 0x4440600a, 0x4460680a, 0x4480700a, 
0x86810024, 0x86820026, 0x86830024, 0x86840026, 0x7423fffc, 
0x7444fffb, 0x4423380a, 0x4443400a, 0x45600809, 0x45801009, 
0x45a01809, 0x45c02009, 0x1fc00000, 0x68200008, 0x00200a00, 
0x00390825, 0x4421300a, 0x46a1280a, 0x081effea, 0x44a10809, 
0x7420fffe, 0x1fe00000};

#define FTI_IMEM_LEN   97

LINT_EXT_HEADER_END

#endif  // DMX_IMEM_H

