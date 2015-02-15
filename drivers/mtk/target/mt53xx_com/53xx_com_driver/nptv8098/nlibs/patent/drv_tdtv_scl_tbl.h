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
#include "drv_tdtv_feature.h"

//===========================================================================
//   CFG for PR Panel
//===========================================================================

//MT5396 Default Configuration Table for Standard PR Panel
UINT64 u8VdoPathProcCfg[NUM_OF_PACKING_TYPE]=
{
    //2D
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //TTD   
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_3D_LI ),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_3D_LI ),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //FS 
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FS_I,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FS_P,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //FP
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FP_I,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FP_P,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //TB  
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_TB_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_TB_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_TB_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_TB_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_TB_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_TB_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_TB_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_TB_P ,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //SBS
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SBS_I,E_TDTV_DECODER_OUTPUT_NATIVE),DQC(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SBS_P,E_TDTV_DECODER_OUTPUT_NATIVE),DQC(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //RD
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_RD_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_RD_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_RD_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_RD_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_RD_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_RD_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Sensio
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SS_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_SS_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SS_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_SS_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Line Interleave
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_LI_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_LI_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_LI_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_LI_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Checker Board
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_CB_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_CB_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_CB_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_CB_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Dot Alternative
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_DA_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_DA_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_DA_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_DA_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_LI ),TTD(E_TD_IN_LI_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),   
};

//===========================================================================
//   CFG for SG Panel
//===========================================================================
UINT64 u8VdoPathProcCfgShutter[NUM_OF_PACKING_TYPE]=
{
    //2D
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //TTD   
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_3D_FS ),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_2D_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_2D_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_2D_P,E_TD_OUT_3D_FS ),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //FS 
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FS_I,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FS_P,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //FP
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FP_I,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_FP_P,E_TDTV_DECODER_OUTPUT_3D_FS  ),DQC(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_FS_P ,E_TD_OUT_NATIVE),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //TB  
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_TB_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_TB_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_TB_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_TB_P ,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_TB_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_TB_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_TB_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_TB_P ,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //SBS
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SBS_I,E_TDTV_DECODER_OUTPUT_NATIVE),DQC(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SBS_P,E_TDTV_DECODER_OUTPUT_NATIVE),DQC(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //RD
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_RD_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_RD_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_RD_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_RD_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_RD_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_RD_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Sensio
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SS_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_SS_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_SS_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_SS_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Line Interleave
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_LI_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_LI_I ,E_TD_OUT_NATIVE),TTT(E_TD_IN_LI_I ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_LI_I ,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_LI_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),TTT(E_TD_IN_LI_P ,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_LI_I ,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Checker Board
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_CB_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_CB_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_CB_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_CB_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    //Dot Alternative
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_DA_I,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_DA_I ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_I,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_I,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),
    TD_TBL_PACK(DEC(E_TDTV_DECODER_INPUT_DA_P,E_TDTV_DECODER_OUTPUT_NATIVE ),DQC(E_TD_IN_DA_P ,E_TD_OUT_3D_SBS),TTT(E_TD_IN_SBS_P,E_TD_OUT_NATIVE),MIB(E_TD_IN_2D_P,E_TD_OUT_NATIVE),SCL(E_TD_IN_SBS_P,E_TD_OUT_3D_FS ),TTD(E_TD_IN_FS_P,E_TD_OUT_NATIVE),MJC(E_TD_IN_2D_P,E_TD_OUT_NATIVE)),   
};
