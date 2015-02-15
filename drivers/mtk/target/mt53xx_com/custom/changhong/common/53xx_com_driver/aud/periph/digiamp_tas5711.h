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
 * $RCSfile: digiamp_others.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file digiamp_others.h
 *  Brief of file digiamp_others.h. \n
 */

#ifndef DIGIAMP_TAS5711_H
#define DIGIAMP_TAS5711_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_typedef.h"
#include "aud_if.h"

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//TAS5711


#define SIF_BUS_ID		SIF_BUS_SYSTEM_I2C  

#define SIF_CLK_DIV		0x100

#define TAS5711_ADDR	        0x36

typedef enum
{

	TYPE_CH999999,  // 0

	TYPE_3D42B4000i,  // 1
	TYPE_3D47B4000i,  // 2
	TYPE_3D50B4000i,  // 3
	TYPE_3D55B4000i,  // 4
	TYPE_3D55B6000i, //5
	TYPE_3D47B8000i, //6
	TYPE_3D55B8000i, //7
	TYPE_3D42B4200i, //8
	TYPE_3D47B4200i,  //9
	TYPE_3D55B4200i, //10
	TYPE_LED32C3070i, //11
	TYPE_LED42C3070i, //12
	TYPE_LED32B4500i, //13
	TYPE_LED42B4500i, //14
	TYPE_3D46C2000i, //15
	TYPE_3D50C2000i, //16
	TYPE_3D55C2000i, //17
	TYPE_3D42B2000iC_L62, //18
	TYPE_3D42B2000iC_L63, //19
	TYPE_3D42C3000i, //20
	TYPE_3D47C3000i,  //21
	TYPE_LED39B4500i,  //22
	TYPE_23,                   //23
	TYPE_24,                   //24
	TYPE_25,                   //25
	TYPE_26,                   //26
	TYPE_27,                   //27
	TYPE_28,                   //28
	TYPE_29,                   //29
	TYPE_30,                   //30
	TYPE_31,                   //31
	TYPE_32,                   //32
	TYPE_33,                   //33
	TYPE_34,                   //34
	TYPE_35,                   //35
	TYPE_36,                   //36
	TYPE_37,                   //37
	TYPE_38,                   //38
	TYPE_39,                   //39
	TYPE_40,                   //40
	TYPE_41,                   //41
	TYPE_42,                   //42
	TYPE_43,                   //43
	TYPE_44,                   //44
	TYPE_45,                   //45
	TYPE_46,                   //46
	TYPE_47,                   //47
	TYPE_48,                   //48
	TYPE_49,                   //49
	TYPE_50,                   //50
	TYPE_51,                   //51
	TYPE_52,                   //52
	TYPE_53,                   //53
	TYPE_54,                   //54
	TYPE_55,                   //55
	TYPE_56,                   //56
	TYPE_57,                   //5
	TYPE_3D32B4000i,  //58
	TYPE_3D39B4000i,  //59
	TYPE_3D42B4500i,  //60
	TYPE_3D47B4500i,  //61
	TYPE_3D50B4500i,  //62
	TYPE_3D55B4500i,  //63
	
	CH_LCD_PRODUCT_TYPE_NUMS,

}ENUM_CH_LCD_PRODUCT_TYPE;


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

extern AMP_T* _AMP_GetAmpTAS5711FunTbl(void);

#endif /* DIGIAMP_TAS5711_H */

