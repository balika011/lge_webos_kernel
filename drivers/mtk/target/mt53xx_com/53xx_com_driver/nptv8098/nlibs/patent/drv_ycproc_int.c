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
 */
 /*------------------------------------------------------------------------------------------------ 
 * Owner: cd.lee
 * 
 * Description: 
 * This file contains Luma/Chroma processing internal functions. 
 *-----------------------------------------------------------------------------------------------
 */

// === INCLUDE =============================================================================
#include "x_assert.h"
#include "x_os.h"	
#include "sv_const.h"
#include "hw_vdoin.h"
#include "nptv_debug.h"
#include "drv_pq_cust.h"

// === HW INCLUDE =============================================================================
#include "hw_ycproc.h"
#include "hw_sw.h"
#include "hw_ospe.h"

    #define SCE_CAP_EN Fld(1,2,AC_MSKB0)//[2:2]
#define POS_MAIN (IO_POSTCOLOR_BASE + 0x03C)
    #define SCE_POS_Y Fld(16,16,AC_FULLW32)//[31:16]
    #define SCE_POS_X Fld(16,0,AC_FULLW10)//[15:0]
#define CAP_IN_DATA_MAIN (IO_POSTCOLOR_BASE + 0x048)
    #define CAP_IN_CB Fld(10,16,AC_MSKW32)//[25:16]
    #define CAP_IN_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_IN_DATA_MAIN_CR (IO_POSTCOLOR_BASE + 0x04C)
    #define CAP_IN_CR Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN (IO_POSTCOLOR_BASE + 0x050)
    #define CAP_OUT_CB Fld(14,16,AC_MSKW32)//[29:16]
    #define CAP_OUT_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN_CR (IO_POSTCOLOR_BASE + 0x054)
    #define CAP_OUT_CR Fld(14,0,AC_MSKW10)//[13:0]

#define LVDS_DUMP_00 (IO_POSTPROC_BASE + 0x9A4)
    #define DUMP_EN Fld(1,0,AC_MSKB0)//[0:0]
#define LVDS_DUMP_01 (IO_POSTPROC_BASE + 0x9A8)
    #define POS_Y Fld(16,16,AC_FULLW32)//[31:16]
    #define POS_X Fld(16,0,AC_FULLW10)//[15:0]
#define LVDS_DUMP_02 (IO_POSTPROC_BASE + 0x9AC)
    #define POS_OUT_G Fld(12,16,AC_MSKW32)//[27:16]
    #define POS_OUT_R Fld(12,0,AC_MSKW10)//[11:0]
#define LVDS_DUMP_03 (IO_POSTPROC_BASE + 0x9B0)
    #define POS_OUT_B Fld(12,0,AC_MSKW10)//[11:0]


void vDrvOsDbgInfo(UINT16 u2PosX, UINT16 u2PosY, UINT16 u2RGB[3])
{
    vIO32WriteFldMulti(LVDS_DUMP_01, P_Fld(u2PosX, POS_X)|P_Fld(u2PosY, POS_Y));

    // enable capture
    vIO32WriteFldAlign(LVDS_DUMP_00, 1, DUMP_EN);

    x_thread_delay(20);

    if (u2RGB != NULL)
    {
        u2RGB[0] = IO32ReadFldAlign(LVDS_DUMP_02, POS_OUT_R);
        u2RGB[1] = IO32ReadFldAlign(LVDS_DUMP_02, POS_OUT_G);
        u2RGB[2] = IO32ReadFldAlign(LVDS_DUMP_03, POS_OUT_B);
    }

    vIO32WriteFldAlign(LVDS_DUMP_00, 0, DUMP_EN);
}

void vDrvSceDbgInfo(UINT16 u2PosX, UINT16 u2PosY, UINT16 u2YCbCrIn[3], UINT16 u2YCbCrOut[3])
{
    //ink position
    vIO32WriteFldMulti(POS_MAIN, P_Fld(u2PosX, SCE_POS_X)|P_Fld(u2PosY, SCE_POS_Y));
    
    //enable capture
    vIO32WriteFldAlign(DBG_CFG_MAIN, 1, SCE_CAP_EN);

    x_thread_delay(20);

    // capture input data
    if (u2YCbCrIn)
    {
        u2YCbCrIn[0] = IO32ReadFldAlign(CAP_IN_DATA_MAIN, CAP_IN_Y);
        u2YCbCrIn[1] = IO32ReadFldAlign(CAP_IN_DATA_MAIN, CAP_IN_CB);
        u2YCbCrIn[2] = IO32ReadFldAlign(CAP_IN_DATA_MAIN_CR, CAP_IN_CR);
    }

    // capture output data
    if (u2YCbCrOut)
    {
        u2YCbCrOut[0] = IO32ReadFldAlign(CAP_OUT_DATA_MAIN, CAP_OUT_Y);
        u2YCbCrOut[1] = IO32ReadFldAlign(CAP_OUT_DATA_MAIN, CAP_OUT_CB);
        u2YCbCrOut[2] = IO32ReadFldAlign(CAP_OUT_DATA_MAIN_CR, CAP_OUT_CR);
    }

    vIO32WriteFldAlign(DBG_CFG_MAIN, 0, SCE_CAP_EN);
}
