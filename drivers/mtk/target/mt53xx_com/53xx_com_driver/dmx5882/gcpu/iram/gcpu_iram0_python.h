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

/** @file gcpu_iram.h
 *  gcpu driver - instruction ram uCode inclusion
 */

static const unsigned long GCPU_IRAMP_TRAPPING[] = {
0x82362027,     // fix AES CTR command when the length is not multiples of 16 bytes
0x8cfc202b,     // support the bypass function and TS Scramble schemes 1
0x8c95208b,     // support the bypass function and TS Scramble schemes 2
0x8cb2209c,     // support the bypass function and TS Scramble schemes 3
0x8cd420a8,     // support the bypass function and TS Scramble schemes 4
0x82000202,     // bypass AES security check, this trapping should always in array last item index
};

static const unsigned long GCPU_IRAMP_CODE[] = {
0x001c8bd2, 0x001d8965, 0x001ed9a4, 0x0010cb64, 0x000fa957,
0x001ed9e5, 0x001c0a54, 0x000909fa, 0x001d8955, 0x001c8be8,
0x000909fa, 0x001ee975, 0x001de946, 0x00128b35, 0x001ee914,
0x001ee927, 0x001bc147, 0x000809fa, 0x000c0957, 0x0008825d,
0x001b8975, 0x001b8dd4, 0x001ac167, 0x000fa947, 0x000909fa,
0x001ee974, 0x001ee915, 0x001de946, 0x00128b35, 0x001ee927,
0x001bc147, 0x000809fa, 0x0008824d, 0x000c0115, 0x001b8975,
0x001b8dd4, 0x001ac167, 0x000fa975, 0x000909fa, 0x001b94bd,
0x0018a9dd, 0x0019a9dd, 0x00090b62, 0x001d88b7, 0x0010a917,
0x00082926, 0x001a8115, 0x0008296d, 0x001a8135, 0x0008296d,
0x001d89c7, 0x001a8095, 0x00082914, 0x001a80b5, 0x0008296a,
0x00092926, 0x0021c8b5, 0x000c0956, 0x001bad7c, 0x00234945,
0x00198975, 0x001b89c7, 0x00092917, 0x001db9d6, 0x00092917,
0x001d8b56, 0x001aad5c, 0x000f2907, 0x001bad45, 0x001dc844,
0x00088999, 0x0019aad5, 0x00188ad5, 0x001a88a4, 0x000fa91a,
0x001dc864, 0x001d89c7, 0x00088999, 0x00092907, 0x001dc804,
0x001d89c7, 0x00088999, 0x001d8957, 0x0019aad5, 0x00188ad5,
0x001a88b5, 0x0008290f, 0x00118d15, 0x001a8b55, 0x000f2921,
0x001dc864, 0x0019aad5, 0x00188ad5, 0x001a88b5, 0x000fa935,
0x001dc804, 0x0012e105, 0x001d89c7, 0x0008855b, 0x001c935e,
0x00088847, 0x001dc865, 0x0019aad5, 0x00188ad5, 0x001a88b5,
0x000fa93e, 0x001dc805, 0x0012e104, 0x001d89c7, 0x0008854a,
0x001b8357, 0x00118d15, 0x000f2926, 0x00118d15, 0x000fa906,
0x000905a8, 0x00218965, 0x001a8895, 0x000fa926, 0x001ec815,
0x001c8b5c, 0x001ec815, 0x001c8b5c, 0x001ec815, 0x001c8b5c,
0x001ec815, 0x001c8b5c, 0x0012e105, 0x001dc804, 0x001d89c7,
0x0008855b, 0x001da9d5, 0x001c8b5e, 0x00088847, 0x001dc805,
0x0012e104, 0x001d89c7, 0x0008854a, 0x00092926, 0x00199515,
0x0011a55e, 0x001aa5de, 0x001b89d4, 0x001de95a, 0x001ee80d,
0x0012e8ad, 0x001ee875, 0x002388b5, 0x0011b956, 0x000829c2,
0x0019b916, 0x001b8576, 0x0012e936, 0x001b8959, 0x000805e7,
0x000905f5, 0x001ee875, 0x001b8955, 0x000f29f1, 0x001ee8f5,
0x00198975, 0x00188975, 0x001c8a58, 0x0009058c, 0x002388b9,
0x0019b915, 0x0012e956, 0x000905e9, 0x0019b11b, 0x0018b11b,
0x00218967, 0x001a8115, 0x000829e5, 0x001a8135, 0x000829e5,
0x00090583, 0x00198297, 0x0017b11b, 0x00090583
};

#define GCPU_IRAMP_LEN   0x2CC

// built on  2012/04/19 13:52
