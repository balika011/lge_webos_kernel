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
 * $RCSfile: io_mtci.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: Peripherals, such as NOR/NAND/SIF/RTC/PWM/GPIO (and others), are centralized in
 *                   this file
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdrm.h"
#include "mthdcp2.h"
#include <linux/mtal_ioctl.h>
#include "gcpu_if.h"
#include "dmx_drm_drvif.h"
#include "tz_if.h"

#include "x_bim.h"
#include "x_hal_arm.h"

#include "mtdrm_wv.h"


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error1.1\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error1.2\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
            MTCI_PRINT("mtal_ioctl_ci argument error1.3\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \


#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(INT32)))          \
        {                                                       \
            MTCI_PRINT("mtal_ioctl_ci argument error2.0\n");      \
            return MTR_ERR_INV;                                          \
        }


#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error2.1\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ntsc argument error2.2\n");      \
        return -1;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ntsc argument error2.3\n");      \
        return -1;                                          \
    }


#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error3\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error4.1\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error4.2\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error6\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error7\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error8\n");      \
        return MTR_ERR_INV;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
		if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   size))			   \
		{														\
			printk("%s: error: USR_SPACE_ACCESS_VALIDATE_ARG_SIZE\n", __FUNCTION__);	 \
			return MTR_ERR_INV; 										 \
		}														\
	
#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
		if (copy_from_user(ArgKernel, (void __user *)ArgUsr,   \
						   (size))) 							\
		{														\
			printk("%s: error: COPY_FROM_USER_ARG_SIZE\n", __FUNCTION__);	   \
			return MTR_ERR_INV; 										 \
		}														\
		
#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
		if (copy_to_user((void __user *)ArgUsr, ArgKernel,	 \
						   size))		   \
		{														\
			printk("%s: error: COPY_TO_USER_ARG_SIZE\n", __FUNCTION__); 	 \
			return MTR_ERR_INV; 										 \
		}														\
    
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static Variables
//-----------------------------------------------------------------------------
static HANDLE_T hMtDrmSemaHandle = (HANDLE_T)NULL;


#define IO_MTDRM_DUMP	(1)




unsigned char nvs_test[1457] = {
	
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9c, 0xe2, 0x27, 0xb8, 0x96, 0x7c, 0x52, 0x29, 0xdb, 0x93, 0xed, 0x2b, 0xee, 0x93, 0x4e, 0x53,
	0x7b, 0x14, 0x3f, 0xb9, 0x08, 0x2f, 0x10, 0x62, 0x85, 0x29, 0xd7, 0xb2, 0x34, 0xb9, 0x19, 0xf5,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7f, 0x00, 0x00, 0x00, 0x01,
	0x30, 0x82, 0x01, 0x7b, 0x30, 0x82, 0x01, 0x28, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
	0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01,
	0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c,
	0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69,
	0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x36, 0x30, 0x31, 0x30, 0x31, 0x30, 0x39, 0x30, 0x30,
	0x30, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x31, 0x30, 0x31, 0x30, 0x31, 0x30, 0x39, 0x30, 0x30, 0x30,
	0x30, 0x5a, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81,
	0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69,
	0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f,
	0x6e, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00,
	0x03, 0x3a, 0x00, 0x04, 0x9f, 0xf3, 0xc2, 0x4e, 0xea, 0xcf, 0xf4, 0x84, 0x65, 0xe8, 0x58, 0xf1,
	0x31, 0xea, 0x78, 0x9c, 0x4c, 0xb0, 0xf8, 0xb9, 0xff, 0x9b, 0x4b, 0x62, 0xe5, 0x07, 0x10, 0x4a,
	0xc9, 0x0f, 0xad, 0xf8, 0x47, 0x5e, 0x6c, 0x3a, 0xd3, 0x54, 0x5a, 0xb0, 0xba, 0xce, 0x0b, 0x10,
	0x78, 0xb2, 0x20, 0x3d, 0x31, 0x1b, 0x22, 0x44, 0x86, 0xc8, 0x64, 0x33, 0xa3, 0x42, 0x30, 0x40,
	0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f,
	0x45, 0x5d, 0xe0, 0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d, 0xe6, 0x30,
	0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30,
	0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff,
	0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x41, 0x00, 0x30,
	0x3e, 0x02, 0x1d, 0x00, 0xb6, 0x2a, 0xd7, 0xd7, 0x7e, 0x9b, 0xe8, 0x68, 0xcb, 0x06, 0x05, 0x88,
	0x4f, 0x7a, 0x89, 0x0b, 0x60, 0x1f, 0x39, 0x08, 0x64, 0x5e, 0xd4, 0x42, 0xe2, 0xed, 0xe3, 0x56,
	0x02, 0x1d, 0x00, 0x8b, 0x5e, 0x61, 0xff, 0xb7, 0xbd, 0x66, 0xd2, 0xc7, 0x55, 0xa7, 0x9a, 0xbc,
	0xa4, 0x4a, 0x16, 0x9b, 0x15, 0x76, 0xaf, 0x7e, 0xc4, 0x1e, 0x13, 0x82, 0x0b, 0xc7, 0x89, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xb6, 0x00, 0x00, 0x00, 0x01, 0x30,
	0x82, 0x03, 0xb2, 0x30, 0x82, 0x02, 0x05, 0x30, 0x82, 0x01, 0xb3, 0xa0, 0x03, 0x02, 0x01, 0x02,
	0x02, 0x08, 0x29, 0xfd, 0x58, 0x8b, 0xea, 0xdd, 0xde, 0x64, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86,
	0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06,
	0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d,
	0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61,
	0x63, 0x74, 0x69, 0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30,
	0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30, 0x33,
	0x34, 0x38, 0x34, 0x33, 0x5a, 0x30, 0x53, 0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01,
	0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61,
	0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e,
	0x74, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c, 0x69,
	0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
	0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07,
	0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x00, 0x08, 0x14,
	0x07, 0x88, 0x1c, 0x25, 0x3f, 0xb0, 0x76, 0xd7, 0x40, 0xbf, 0x73, 0x3a, 0x6f, 0x96, 0x39, 0x6b,
	0xef, 0x78, 0x0e, 0x59, 0x1b, 0xaa, 0x7c, 0x2a, 0xc0, 0xa8, 0xa1, 0xe2, 0xde, 0x83, 0x96, 0x9e,
	0x76, 0xa3, 0x25, 0x79, 0x1e, 0x2c, 0xb4, 0x19, 0x0b, 0x1a, 0xf5, 0x55, 0x8d, 0x38, 0x7c, 0x55,
	0xc8, 0xe6, 0xa6, 0x6a, 0xaf, 0xa3, 0x81, 0xa6, 0x30, 0x81, 0xa3, 0x30, 0x1f, 0x06, 0x03, 0x55,
	0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f, 0x45, 0x5d, 0xe0,
	0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d, 0xe6, 0x30, 0x1d, 0x06, 0x03,
	0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5,
	0x19, 0x57, 0x58, 0x75, 0x84, 0xfb, 0xdf, 0x4f, 0xba, 0xac, 0xf4, 0x30, 0x0e, 0x06, 0x03, 0x55,
	0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x0f, 0x06, 0x03, 0x55,
	0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x40, 0x06, 0x03,
	0x55, 0x1d, 0x1f, 0x04, 0x39, 0x30, 0x37, 0x30, 0x35, 0xa0, 0x33, 0xa0, 0x31, 0x86, 0x2f, 0x68,
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x2d, 0x63, 0x72, 0x6c, 0x2e, 0x6d,
	0x61, 0x72, 0x6c, 0x69, 0x6e, 0x2d, 0x74, 0x6d, 0x6f, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x63, 0x72,
	0x6c, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x63, 0x72, 0x6c, 0x73, 0x2e, 0x63, 0x72, 0x6c, 0x30, 0x0a,
	0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x40, 0x00, 0x30, 0x3d, 0x02,
	0x1d, 0x00, 0x87, 0x1b, 0x38, 0x4a, 0x62, 0x54, 0x41, 0x00, 0xcf, 0xde, 0xa6, 0x1d, 0x64, 0x8e,
	0xa9, 0xa0, 0x63, 0x57, 0x03, 0x5e, 0xae, 0x28, 0xd0, 0xb7, 0x23, 0x0a, 0xc7, 0xaf, 0x02, 0x1c,
	0x2f, 0x32, 0x14, 0xdb, 0xe8, 0xfa, 0x5e, 0x86, 0xa3, 0x81, 0x46, 0x0b, 0xfe, 0x53, 0x69, 0x9e,
	0x3f, 0x2b, 0x3e, 0x06, 0xc6, 0x7e, 0x40, 0x69, 0x57, 0x1c, 0x9e, 0x41, 0x30, 0x82, 0x01, 0xa5,
	0x30, 0x82, 0x01, 0x54, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x31, 0x0f, 0xd5, 0xc2, 0x6c,
	0x9c, 0x5e, 0x5b, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30,
	0x53, 0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01,
	0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x63,
	0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x74, 0x72, 0x75,
	0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61,
	0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
	0x2e, 0x2e, 0x2e, 0x2e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37, 0x30, 0x32, 0x30, 0x31, 0x30, 0x32,
	0x30, 0x30, 0x34, 0x36, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x32, 0x30, 0x31, 0x30, 0x32, 0x30,
	0x30, 0x34, 0x36, 0x5a, 0x30, 0x23, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04,
	0x01, 0x81, 0xb9, 0x2f, 0x03, 0x01, 0x01, 0x0c, 0x10, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x64, 0x38, 0x63, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a,
	0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x89, 0x04, 0x62, 0x83,
	0xd6, 0x96, 0xdb, 0x95, 0x97, 0x78, 0x82, 0x90, 0x4d, 0xf6, 0x32, 0x1d, 0xe3, 0x7d, 0xe8, 0x49,
	0xdf, 0x53, 0x5b, 0x4d, 0xbb, 0xa8, 0xe4, 0x13, 0x3e, 0xc1, 0x5d, 0xcb, 0x81, 0x36, 0xd9, 0x7f,
	0x10, 0x4e, 0xb0, 0xdf, 0x4d, 0xd5, 0x4d, 0xe3, 0xf6, 0xb7, 0x25, 0xa3, 0x19, 0x66, 0x37, 0x12,
	0xfa, 0x92, 0xb2, 0x14, 0xa3, 0x52, 0x30, 0x50, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04,
	0x18, 0x30, 0x16, 0x80, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5, 0x19, 0x57,
	0x58, 0x75, 0x84, 0xfb, 0xdf, 0x4f, 0xba, 0xac, 0xf4, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
	0x04, 0x16, 0x04, 0x14, 0x0a, 0xc7, 0xa5, 0x50, 0xcd, 0x30, 0x66, 0x25, 0x2c, 0xf2, 0xe3, 0x31,
	0x54, 0x2e, 0x59, 0x3b, 0x59, 0xfe, 0x03, 0x17, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01,
	0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
	0x3d, 0x04, 0x03, 0x02, 0x03, 0x3f, 0x00, 0x30, 0x3c, 0x02, 0x1c, 0x17, 0x70, 0xb2, 0x1b, 0x99,
	0x35, 0x28, 0xfb, 0x29, 0x70, 0xf2, 0x1b, 0xc4, 0x33, 0x05, 0xc1, 0xce, 0x3d, 0x66, 0x6d, 0x57,
	0x15, 0x9f, 0x01, 0x4d, 0x1d, 0xd1, 0x4d, 0x02, 0x1c, 0x45, 0xe2, 0xd2, 0x5c, 0x18, 0xff, 0xc4,
	0x79, 0x78, 0xf9, 0x48, 0xfc, 0x6e, 0xcc, 0x3c, 0xfc, 0xe1, 0x6d, 0x20, 0x50, 0xe3, 0x2b, 0x3a,
	0x84, 0x66, 0xfb, 0xe6, 0xe0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0x00, 0x00, 0x00, 0x01, 0xbf, 0xa4, 0xd9, 0x46, 0xd8, 0xf7, 0x38, 0x40, 0x0e, 0x0b, 0x34,
	0x1d, 0x02, 0xce, 0x7f, 0x70, 0x3e, 0x56, 0xf9, 0xbf, 0x82, 0xae, 0x52, 0x4c, 0xe5, 0x7a, 0x5f,
	0x02


};

unsigned char _au1HDCP2x_2_production_key[MTDRM_HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE] =
{
	0xc6, 0x6a, 0x7c, 0x4c, 0xaa, 0xbd, 0x58, 0xcf, 0x69, 0x21, 0x52, 0xc2, 0x09, 0x83, 0x6c, 0xcc,
	0xaf, 0x69, 0xee, 0x0d, 0xf5, 0x1c, 0x12, 0x94, 0xea, 0xe8, 0xfd, 0xb2, 0xb4, 0x4f, 0xc2, 0x78,
	0xd6, 0x33, 0x00, 0x3b, 0xd8, 0x1c, 0x49, 0xb1, 0x1b, 0xf8, 0xbc, 0x0e, 0xc7, 0x0b, 0xb9, 0xe4,
	0xce, 0xa3, 0xf8, 0xed, 0x0e, 0x03, 0x3a, 0xc7, 0x55, 0x21, 0xf1, 0xb3, 0xa0, 0xed, 0x14, 0x05,
	0xf5, 0x8c, 0xc1, 0xfd, 0xc7, 0x8d, 0x06, 0x34, 0x7b, 0x03, 0x09, 0xc8, 0x85, 0x50, 0x76, 0x0b,
	0xf2, 0x4d, 0x27, 0x67, 0x8c, 0xcc, 0x20, 0x98, 0x66, 0x13, 0xc6, 0x17, 0x9d, 0x59, 0x78, 0xca,
	0xa4, 0x4e, 0xb4, 0xa0, 0x90, 0xa8, 0xfc, 0x51, 0xcf, 0xb0, 0x2e, 0x5e, 0x2f, 0x7e, 0x96, 0xe1,
	0xbc, 0x10, 0x88, 0x80, 0x98, 0x5a, 0x3b, 0x17, 0xcd, 0x69, 0x6e, 0x19, 0x30, 0x20, 0x02, 0xba,
	0x88, 0x4c, 0x4e, 0x2d, 0x69, 0x2b, 0x97, 0x4d, 0xa7, 0x13, 0x6a, 0x9e, 0x78, 0xae, 0x41, 0xba,
	0x84, 0x16, 0xae, 0x72, 0x9f, 0x8f, 0xa8, 0x75, 0xf0, 0xdf, 0xda, 0xda, 0x12, 0x15, 0x84, 0x94,
	0x77, 0xf0, 0x7c, 0x0e, 0x92, 0x02, 0x30, 0x37, 0x92, 0x02, 0xb7, 0x3f, 0x02, 0xa5, 0x0d, 0xcd,
	0xac, 0x6c, 0xee, 0xa6, 0x3c, 0x96, 0x39, 0xcc, 0x89, 0x31, 0x72, 0x0b, 0x63, 0x39, 0xf1, 0x00,
	0xde, 0xf8, 0x1c, 0xb3, 0x15, 0xc2, 0xbd, 0x87, 0xc1, 0xd3, 0x3b, 0xe8, 0x8c, 0xbb, 0xf6, 0x4f,
	0xc8, 0xfb, 0x59, 0x99, 0x0c, 0xd3, 0x09, 0x8c, 0x90, 0xd7, 0x33, 0x07, 0xa7, 0x00, 0x5a, 0xfb,
	0xee, 0xdf, 0xb0, 0x39, 0x2a, 0x55, 0x4d, 0x77, 0xfa, 0xdc, 0x23, 0xb1, 0x2c, 0x4c, 0x5e, 0x1a,
	0xfc, 0x34, 0x1d, 0xe7, 0x95, 0x4f, 0xbe, 0x47, 0x54, 0xc3, 0xcd, 0xe7, 0x76, 0x02, 0x60, 0xf4,
	0xc2, 0x3d, 0xbc, 0x72, 0xfc, 0x21, 0x8a, 0xd9, 0xd8, 0xc7, 0x5e, 0x70, 0x40, 0x93, 0xab, 0x2d,
	0x44, 0x64, 0xf6, 0xb9, 0xba, 0xfe, 0x0f, 0xea, 0xd9, 0x0b, 0xf1, 0xbb, 0xad, 0xbb, 0x62, 0x3b,
	0x6f, 0xcf, 0x6b, 0x86, 0xdb, 0xb7, 0x28, 0x4a, 0x98, 0x90, 0x5a, 0x17, 0xd1, 0xa4, 0x16, 0xd9,
	0x89, 0x34, 0xcf, 0xec, 0x40, 0xb6, 0xa3, 0x15, 0x55, 0x14, 0xfe, 0x82, 0xb8, 0xad, 0xd6, 0xde,
	0x8f, 0xba, 0x3b, 0x8d, 0x1b, 0x17, 0x3d, 0x59, 0x53, 0x31, 0x30, 0x68, 0x1d, 0xc8, 0xe0, 0xb9,
	0xe8, 0x2a, 0x39, 0x26, 0xa8, 0x20, 0x04, 0x43, 0x38, 0x66, 0x18, 0x67, 0xc6, 0x8a, 0xf1, 0xa4,
	0xbc, 0xcc, 0xe1, 0x13, 0x5b, 0xee, 0x3f, 0x29, 0x14, 0xc9, 0x38, 0xd2, 0x95, 0x92, 0xf6, 0x56,
	0x5d, 0xb9, 0x07, 0xe7, 0x61, 0x3d, 0x1c, 0x75, 0xee, 0x3d, 0x71, 0x03, 0xb3, 0x3b, 0xc6, 0x34,
	0x13, 0x42, 0xa8, 0x7f, 0xee, 0x44, 0xa1, 0x78, 0x9e, 0x14, 0xb9, 0x51, 0xa0, 0x79, 0x0f, 0x6e,
	0xb7, 0x23, 0xa4, 0x3f, 0x45, 0xa1, 0xd7, 0x90, 0x50, 0x91, 0x9c, 0xe5, 0x0e, 0xc4, 0xe5, 0x1d,
	0x4b, 0xcc, 0xce, 0x14, 0x4d, 0x62, 0xe4, 0x60, 0x7d, 0xca, 0x8b, 0xdd, 0xc8, 0xf8, 0xa9, 0xb6,
	0xe4, 0xb7, 0x3a, 0xf6, 0x5b, 0xaa, 0x10, 0x5b, 0xf9, 0x20, 0x7c, 0xff, 0xee, 0x17, 0x17, 0x87,
	0xac, 0xab, 0xb6, 0x97, 0x34, 0x3e, 0x5e, 0xfa, 0x6d, 0x05, 0x26, 0xb4, 0x2b, 0x05, 0x1a, 0xa9,
	0xe4, 0x37, 0xd0, 0x12, 0x2a, 0x88, 0x50, 0xbd, 0x6f, 0xc7, 0x23, 0xa7, 0x8f, 0x85, 0xbf, 0xd3,
	0xf1, 0x02, 0x93, 0x30, 0xe1, 0x4b, 0x01, 0x99, 0x51, 0xd5, 0xb0, 0x50, 0x2d, 0x31, 0xc2, 0xdd,
	0x2c, 0x3d, 0x86, 0x2e, 0x26, 0x85, 0x5e, 0x57, 0x00, 0xf9, 0xae, 0x7f, 0x7a, 0x2e, 0x36, 0x44,
	0x1c, 0xd6, 0x71, 0x59, 0x64, 0xd5, 0xd9, 0x5d, 0x8f, 0xaa, 0xcf, 0xe3, 0xbe, 0xc5, 0xf1, 0x71,
	0x86, 0xb0, 0xa5, 0xcf, 0xbd, 0xce, 0x50, 0xf9, 0x94, 0xe4, 0x5c, 0xcb, 0xd4, 0x67, 0xc1, 0x2c,
	0x1b, 0xa7, 0x00, 0x03, 0x96, 0xdb, 0x43, 0x97, 0xee, 0xee, 0xb9, 0x21, 0x9c, 0xe5, 0xf8, 0x13,
	0xb5, 0x2d, 0x17, 0x62, 0x36, 0x78, 0xd8, 0x0d, 0x8e, 0xdb, 0x1a, 0x08, 0x8d, 0x64, 0xdd, 0x0a,
	0x9d, 0xab, 0xfb, 0xb6, 0x8f, 0x7f, 0x84, 0x96, 0x7f, 0xde, 0x5f, 0x47, 0xdc, 0x21, 0x1f, 0xbd,
	0xdf, 0xae, 0xf2, 0xdc, 0x17, 0x51, 0x4c, 0x09, 0x90, 0x80, 0xc6, 0xc0, 0xc1, 0x7b, 0xc0, 0x1f,
	0xf1, 0x2e, 0x8c, 0xd7, 0xd5, 0xf8, 0xb2, 0x17, 0x8d, 0xa5, 0x27, 0x18, 0x0a, 0x94, 0x33, 0xcc,
	0xf4, 0xb7, 0x7d, 0x2d, 0xa1, 0x64, 0x46, 0x03, 0x56, 0x11, 0x06, 0x4f, 0x4d, 0xf1, 0x1d, 0x0d,
	0x90, 0xb0, 0x04, 0x08, 0x05, 0x69, 0x27, 0x51, 0x1f, 0x7b, 0x9a, 0x85, 0xe2, 0x71, 0x83, 0xcb,
	0xd7, 0x51, 0x88, 0x99, 0xdb, 0xf9, 0xd7, 0x51, 0x7d, 0x16, 0x65, 0xfd, 0xc3, 0x2c, 0x3b, 0x8f,
	0xc3, 0x27, 0x44, 0x2b, 0x49, 0x5d, 0xbe, 0x43, 0xe2, 0x18, 0x2b, 0x8c, 0x03, 0x28, 0x36, 0x54,
	0x21, 0x7f, 0x78, 0xaa, 0xdc, 0xbb, 0x36, 0xfd, 0x8a, 0x0c, 0xa5, 0x95, 0xf9, 0x5d, 0x11, 0xa8,
	0x82, 0x0c, 0x33, 0x3f, 0xc6, 0xc3, 0xf9, 0xaa, 0x7a, 0x8f, 0xf4, 0x8b, 0x49, 0x90, 0x48, 0xb8,
	0xf5, 0x75, 0x2f, 0x71, 0x09, 0xa7, 0xe6, 0xfe, 0xb5, 0xbb, 0x23, 0xde, 0xc8, 0xea, 0xce, 0x96,
	0xed, 0x1b, 0xa9, 0x10, 0xb6, 0x54, 0xaa, 0x3a, 0xb5, 0xc8, 0x8e, 0x84, 0xd1, 0xcb, 0x3d, 0x7f,
	0x43, 0xf3, 0x78, 0xc8, 0xb7, 0xe3, 0xbe, 0xd5, 0x66, 0x0f, 0xca, 0x4d, 0x8c, 0x25, 0x4f, 0x58,
	0xf2, 0x8c, 0x26, 0x57, 0x7c, 0x26, 0x9d, 0xe6, 0x66, 0xb6, 0xcb, 0xdb, 0x68, 0xaf, 0x9f, 0x34,
	0x61, 0xc9, 0x73, 0xeb, 0x03, 0x4d, 0xcf, 0x5c, 0x19, 0x29, 0x4a, 0xba, 0x78, 0x3f, 0x01, 0xd5,
	0x2d, 0xb4, 0xbc, 0x3c, 0x69, 0x41, 0x15, 0x4d, 0xda, 0x31, 0x48, 0xaf, 0x4d, 0xa4, 0x20, 0xbf,
	0x21, 0xa7, 0x18, 0xd3, 0x44, 0x90, 0xd4, 0x7e, 0x72, 0x47, 0xf0, 0x92, 0x8d, 0x0a, 0xed, 0xb8,
	0x2b, 0xb5, 0xcb, 0xac, 0xe6, 0x7e, 0x2a, 0x2a, 0x99, 0x4e, 0x17, 0xbb, 0x83, 0xef, 0x43, 0x3a,
	0x6a, 0xc0, 0xf8, 0xc4, 0x68, 0xc6, 0x7f, 0xcc, 0xf5, 0xa6, 0x1d, 0xcf, 0xb1, 0x1d, 0x8f, 0x26,
	0xcb, 0x65, 0xae, 0x21, 0xae, 0x8f, 0xc6, 0x09, 0x86, 0xb5, 0x49, 0x23, 0x6c, 0x74, 0xb4, 0xa0
};


static MT_RESULT_T  _MTDRM_HDCP2_Set_MTK_EncKeySet_into_TZ(unsigned char *keyset, int keyset_len);
static int hdcp_key_tz_enable = 0;

void mtdrm_hdcp_DumpHex(char *name, const unsigned char *data, int len)
{

#if IO_MTDRM_DUMP

    int pos = 0;
    
    if (!data || !len)
    {
        return;
    }
	
    printf("@@@@@ MTDRM Dumping %s ------------------------------\n", name);
	
    while(pos < len /*len*/) // only print message ID
    {
        if (pos%8 == 0)
            printf("\n[%04x]|", pos);
        printf("0x%02x, ", data[pos]); // secure data, cannot show in mtktool
        pos ++;
    }
    printf("\n@@@@@\n------------------------------\n");


#endif

}










//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

EXTERN MT_RESULT_T _MTDRM_SECURE_ALLOC(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_SECURE_FREE(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_SECURE_DATAMOVE(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_SECURE_GETDATA(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_SECURE_CPB_DATAMOVE_DONE(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_SECURE_CPB_DATAMOVE_INFO(unsigned long arg);

EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Initialize(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Terminate(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_OpenSession(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_CloseSession(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateDerivedKeys(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateNonce(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateSignature(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_LoadKeys(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_RefreshKeys(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_SelectKey(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DecryptCTR(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_InstallKeybox(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_IsKeyboxValid(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GetDeviceID(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GetKeyData(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GetRandom(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_WrapKeybox(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_RewrapDeviceRSAKey(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_LoadDeviceRSAKey(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateRSASignature(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DeriveKeysFromSessionKey(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Encrypt(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Decrypt(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Sign(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Verify(unsigned long arg);

EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_UpdateUsageTable(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DeactivateUsageEntry(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_ReportUsage(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DeleteUsageEntry(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DeleteUsageTable(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GetUsageTableWriteRequest(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_VerifyWriteResponse(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_GetUsageTableReadRequest(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_VerifyReadResponse(unsigned long arg);

EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_SetEntitlementKey(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DeriveControlWord(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DecryptVideo(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_WV_OEMCrypto_DecryptAudio(unsigned long arg);

EXTERN MT_RESULT_T _MTDRM_RPMB_LOAD_MAC_KEY(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_RPMB_GET_WCNT_REQ(unsigned long arg);
EXTERN MT_RESULT_T _MTDRM_RPMB_SET_WCNT_RESP(unsigned long arg);


static MT_RESULT_T  _MTDRM_HDCP2_SetEncLc128(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_ENCLC128_T *prHdcp2EncLc128;/*kernel data*/
    MTDRM_HDCP2_ENCLC128_T rHdcp2EncLc128;/*kernel data*/
    MTDRM_TZIF_HDCP2_SET_ENC_LC128_T rHdcp2TzIfEncLc128;
    UINT32 u4Len;
    INT32 iRet = MTR_NOT_OK;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2EncLc128 = (MTDRM_HDCP2_ENCLC128_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2EncLc128, u4Len);
    COPY_FROM_USER_ARG_SIZE(prHdcp2EncLc128, &rHdcp2EncLc128, u4Len);
    
    rHdcp2TzIfEncLc128.pEncKey = (VOID*)PHYSICAL((UINT32)rHdcp2EncLc128.au1EncKey);
    rHdcp2TzIfEncLc128.u4Length = u4Len;
    rHdcp2TzIfEncLc128.rRet = MTR_NOT_OK;

    HalFlushInvalidateDCacheMultipleLine((UINT32)&rHdcp2EncLc128, sizeof(MTDRM_HDCP2_ENCLC128_T));      
    
    //iRet = TZ_HDCP2_SetEncLc128((MTDRM_TZIF_HDCP2_SET_ENC_LC128_T *)&rHdcp2TzIfEncLc128);

    HalFlushInvalidateDCacheMultipleLine((UINT32)&rHdcp2EncLc128, sizeof(MTDRM_HDCP2_ENCLC128_T));   
    
    if(iRet != 0)
    {
        return (rHdcp2TzIfEncLc128.rRet);
    }
    else
    {
        return (MTR_NOT_OK);
    }

}


static MT_RESULT_T  _MTDRM_HDCP2_SetEncKey(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_KEYENC_T *prHdcp2EncKey;/*kernel data*/
    UINT32 u4Len;
    INT32 i4Ret = 0;
    MTDRM_HDCP2_KEYENC_T *Alligned_rHdcp2EncKey = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_SET_ENC_KEY *Alligned_rHdcp2TzIfEncKey = NULL;

    Alligned_rHdcp2EncKey = (MTDRM_HDCP2_KEYENC_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_KEYENC_T), 32));/*kernel data*/
    if (!Alligned_rHdcp2EncKey)
    {
        printf("Allocate Alligned_rHdcp2EncKey failed\n");
        return MTR_NOT_OK;
    }    
    Alligned_rHdcp2TzIfEncKey = (MTDRM_TZIF_HDCP2_SET_ENC_KEY *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY), 32));/*kernel data*/ 
    if (!Alligned_rHdcp2TzIfEncKey)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKey);
		
        printf("Allocate Alligned_rHdcp2TzIfEncKey failed\n");
        return MTR_NOT_OK;
    }
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2EncKey = (MTDRM_HDCP2_KEYENC_T *)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2EncKey, u4Len);
    COPY_FROM_USER_ARG_SIZE(prHdcp2EncKey, Alligned_rHdcp2EncKey, u4Len);

    Alligned_rHdcp2TzIfEncKey->pEncKey = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2EncKey->au1EncKey);
    Alligned_rHdcp2TzIfEncKey->u4length = u4Len;
    Alligned_rHdcp2TzIfEncKey->rRet = MTR_NOT_OK;

	
    #ifdef CC_ENABLE_HDCP2
	
	    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2EncKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYENC_T)));
	    i4Ret = TZ_HDCP2_SetEncKey(Alligned_rHdcp2TzIfEncKey, sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY));    
	    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2EncKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYENC_T)));

		printf("TZ_HDCP2_SetEncKey: i4Ret(%d) Alligned_rHdcp2TzIfEncKey->rRet(%d) \n", 
				i4Ret, Alligned_rHdcp2TzIfEncKey->rRet);	
	
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfEncKey->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
	
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKey);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEncKey);
    return i4Ret;
}

/**
Input parameter
? pReceiverID :  HDCP 2.0 RX Identifier buffer
? pPubKey : HDCP 2.0 RX public key buffer
? pRootSign : HDCP 2.0 RX Root Signature buffer

Ouput
? Status : success/ fail

Operation
? Read the certification information form secure store and write these to each buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_GetCertInfo(unsigned long arg)
{
    INT32 i4Ret = 0;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_KEYCERT_T *prHdcp2CertKey;
    UINT32 u4Len = 0;   
    
    MTDRM_HDCP2_KEYCERT_T *Alligned_rHdcp2CertKey = NULL;
    MTDRM_TZIF_HDCP2_GET_CERT_INFO *Alligned_rHdcp2TzIfGetCertInfo = NULL;
    Alligned_rHdcp2CertKey = (MTDRM_HDCP2_KEYCERT_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_KEYCERT_T), 32));/*kernel data*/
    if (!Alligned_rHdcp2CertKey)
    {
        printf("Allocate Alligned_rHdcp2CertKey failed\n");
        return MTR_NOT_OK;
    } 
    
    Alligned_rHdcp2TzIfGetCertInfo = (MTDRM_TZIF_HDCP2_GET_CERT_INFO *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_GET_CERT_INFO), 32));
    if (!Alligned_rHdcp2TzIfGetCertInfo)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2CertKey);
        printf("Allocate Alligned_rHdcp2TzIfGetCertInfo failed\n");
        return MTR_NOT_OK;
    } 

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2CertKey = (MTDRM_HDCP2_KEYCERT_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    // read built-in edid data from driver
    Alligned_rHdcp2TzIfGetCertInfo->pData = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2CertKey->au1RxID);
    Alligned_rHdcp2TzIfGetCertInfo->u4length = u4Len;
    Alligned_rHdcp2TzIfGetCertInfo->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2    
    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2CertKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYCERT_T)));
    i4Ret = TZ_HDCP2_GetCertInfo(Alligned_rHdcp2TzIfGetCertInfo, sizeof(MTDRM_TZIF_HDCP2_GET_CERT_INFO));
    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2CertKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYCERT_T)));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGetCertInfo->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif

    USR_SPACE_ACCESS_VALIDATE_ARG(prHdcp2CertKey, MTDRM_HDCP2_KEYCERT_T);
    COPY_TO_USER_ARG(prHdcp2CertKey, *Alligned_rHdcp2CertKey, MTDRM_HDCP2_KEYCERT_T);
        
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2CertKey);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGetCertInfo);

    return i4Ret;

}

/**
Input parameter
? pEkpub_km : encrypted Km value (128byte)

Output
? Status : success or fail

Operation
? DDI decrypts the km and store the decrypted km to the secure store
? All RSA decryption and Store data shall be in the Secure world
**/
static MT_RESULT_T  _MTDRM_HDCP2_DecryptRSAESOAEP(unsigned long arg)
{
    INT32 i4Ret = 1;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_EKPUBKM_T *prEkpub_km;
    UINT32 u4Len;
    MTDRM_HDCP2_EKPUBKM_T* Alligned_rEkpub_km = NULL;
    MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP* Alligned_rHdcp2TzIfDeRSAESOAEP = NULL;

    Alligned_rEkpub_km = (MTDRM_HDCP2_EKPUBKM_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_EKPUBKM_T), 32));/*kernel data*/
    if (!Alligned_rEkpub_km)
    {
        printf("Allocate Alligned_rEkpub_km failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfDeRSAESOAEP = (MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP), 32));
    if (!Alligned_rHdcp2TzIfDeRSAESOAEP)
    {
   		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rEkpub_km);
        printf("Allocate Alligned_rHdcp2TzIfDeRSAESOAEP failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prEkpub_km = (MTDRM_HDCP2_EKPUBKM_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prEkpub_km, sizeof(MTDRM_HDCP2_EKPUBKM_T));
    COPY_FROM_USER_ARG_SIZE(prEkpub_km, Alligned_rEkpub_km, sizeof(MTDRM_HDCP2_EKPUBKM_T));
    
    Alligned_rHdcp2TzIfDeRSAESOAEP->pEkpub_km = (VOID*)PHYSICAL((UINT32)Alligned_rEkpub_km->au1Ekpubkm);
    Alligned_rHdcp2TzIfDeRSAESOAEP->u4length= u4Len;
    Alligned_rHdcp2TzIfDeRSAESOAEP->rRet = MTR_NOT_OK;


    #ifdef CC_ENABLE_HDCP2  

        TZ_NWD_BufferStart((UINT32)Alligned_rEkpub_km, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_EKPUBKM_T)));

        i4Ret = TZ_HDCP2_DecryptRSAESOAEP(Alligned_rHdcp2TzIfDeRSAESOAEP, sizeof(MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP));
        
        TZ_NWD_BufferEnd((UINT32)Alligned_rEkpub_km, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_EKPUBKM_T)));   
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfDeRSAESOAEP->rRet;         
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDeRSAESOAEP);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rEkpub_km);    
    return (i4Ret);
}

/**
Input parameter
? pRtx : received 64bit random value from HDCP 2.0 TX

Output
? Status : success or fail

Operation
? DDI derived kd value in the secure world.
? This DDI call the AES-CTR DDI twice and generate dkey0 and dkey1 using Ctr 0 and 1 respectively.
    And derived kd is stored in secure storge
**/
static MT_RESULT_T  _MTDRM_HDCP2_KdKeyDerivation(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_RTX_T *prRtx;
    //MTDRM_HDCP2_RTX_T rRtx;/*kernel data*/
    UINT32 u4Len;
    //MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION rHdcp2TzIfKdDer;
    MTDRM_HDCP2_RTX_T* Alligned_rRtx = NULL;
    MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION* Alligned_rHdcp2TzIfKdDer = NULL;
    
    Alligned_rRtx = (MTDRM_HDCP2_RTX_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_RTX_T), 32));/*kernel data*/
    if (!Alligned_rRtx)
    {
        printf("Allocate Alligned_rRtx failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfKdDer = (MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION), 32));
    if (!Alligned_rHdcp2TzIfKdDer)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRtx);
        printf("Allocate Alligned_rHdcp2TzIfKdDer failed\n");
        return MTR_NOT_OK;
    }    

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prRtx = (MTDRM_HDCP2_RTX_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prRtx, u4Len);
    COPY_FROM_USER_ARG_SIZE(prRtx, Alligned_rRtx, u4Len);

    Alligned_rHdcp2TzIfKdDer->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_rRtx->au1rTx);
    Alligned_rHdcp2TzIfKdDer->u4length = u4Len;
    Alligned_rHdcp2TzIfKdDer->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rRtx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RTX_T)));
        i4Ret = TZ_HDCP2_KdKeyDerivation(Alligned_rHdcp2TzIfKdDer, sizeof(MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION));    
        TZ_NWD_BufferEnd((UINT32)Alligned_rRtx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RTX_T)));
        
        if (i4Ret != 0)
        {        
            i4Ret = Alligned_rHdcp2TzIfKdDer->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfKdDer);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRtx);
    return(i4Ret);

}

/**
Input parameter
? pH_prime : output buffer
? Repeater: repeater flag (1byte) (0x00 or 0x01)
? pRtx : received random value (64bit)

Output
? Status : success or fail

Operation
? DDI generates hash code using HMAC-SHA256. HMAC-SHA256(pRtx XOR Repeater, kd)
     - Key ? kd (from secure DRAM)
     - Key length ? 32 byte
     - Message length = 8 byte
     - Target message ? pRtx XOR Repeater
     - Output hashed value ? pH_prime(256bit)
    Write result H_prime value to pH_prime buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_ComputeHprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_CALHPRIME_T *prCalHprime;
    MTDRM_HDCP2_CALHPRIME_T *Alligned_rCalHprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME *Alligned_rHdcp2TzIfCpuHprime = NULL;
    UINT32 u4Len;

    Alligned_rCalHprime = (MTDRM_HDCP2_CALHPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_CALHPRIME_T), 32));/*kernel data*/
    
    if (!Alligned_rCalHprime)
    {
        printf("Allocate Alligned_rCalHprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuHprime = (MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME), 32));
    if (!Alligned_rHdcp2TzIfCpuHprime)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalHprime);
        printf("Allocate Alligned_rHdcp2TzIfCpuHprime failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalHprime = (MTDRM_HDCP2_CALHPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalHprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalHprime, Alligned_rCalHprime, u4Len);

    Alligned_rHdcp2TzIfCpuHprime->pH_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalHprime->au1rHprime);
    Alligned_rHdcp2TzIfCpuHprime->Repeater = Alligned_rCalHprime->u1Repeater;
    Alligned_rHdcp2TzIfCpuHprime->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_rCalHprime->au1rTx); 
    Alligned_rHdcp2TzIfCpuHprime->rRet = MTR_NOT_OK;    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalHprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALHPRIME_T)));
        i4Ret = TZ_HDCP2_ComputeHprime(Alligned_rHdcp2TzIfCpuHprime,sizeof(MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalHprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALHPRIME_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuHprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalHprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalHprime, Alligned_rCalHprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalHprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuHprime);
    return i4Ret;
}

/**
Input parameter
? pL_prime : output buffer
? pRn : received random value form HDCP 2.0 TX
? pRtx: received random value form HDCP 2.0 TX

Output
? Status : success or fail

Operation
? L? = HMAC-SHA256(pRn, kd XOR pRrx)
? Key ? kd XOR pRrx (kd from secure DRAM)
? Key length ? 32 byte
? Message length = 8 byte
? Target message ? pRn
? Output? pL_prime(256bit)
? write result L_prime value to pL_prime buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_ComputeLprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_CALLPRIME_T *prCalLprime;
    MTDRM_HDCP2_CALLPRIME_T *Alligned_rCalLprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME *Alligned_rHdcp2TzIfCpuLprime = NULL;
    UINT32 u4Len;

    Alligned_rCalLprime = (MTDRM_HDCP2_CALLPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_CALLPRIME_T), 32));/*kernel data*/
    if (!Alligned_rCalLprime)
    {
        printf("Allocate Alligned_rCalLprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuLprime = (MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME), 32));
    if (!Alligned_rHdcp2TzIfCpuLprime)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalLprime);
        printf("Allocate Alligned_rHdcp2TzIfCpuLprime failed\n");
        return MTR_NOT_OK;
    }    

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalLprime = (MTDRM_HDCP2_CALLPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalLprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalLprime, Alligned_rCalLprime, u4Len);

    Alligned_rHdcp2TzIfCpuLprime->pL_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rLprime);
    Alligned_rHdcp2TzIfCpuLprime->pRn = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rRn);
    Alligned_rHdcp2TzIfCpuLprime->pRrx = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rRx); 
    Alligned_rHdcp2TzIfCpuLprime->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalLprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALLPRIME_T)));
        i4Ret = TZ_HDCP2_ComputeLprime(Alligned_rHdcp2TzIfCpuLprime, sizeof(MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalLprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALLPRIME_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuLprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalLprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalLprime, Alligned_rCalLprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalLprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuLprime);
    return i4Ret;
}

/**
Input parameter
? void
Output
? Status : success or fail
Operation
? Compute kh = SHA256(Kprivrx)[127:0]
? Store the kh value between 0 and 127 bit position  to the secure DRAM
**/
static MT_RESULT_T  _MTDRM_HDCP2_ComputeKh(unsigned long arg)
{
    MTDRM_TZIF_HDCP2_COMPUTE_KH *Alligned_rHdcp2TzIfCpuKh = NULL;
    INT32 i4Ret = 0;
    Alligned_rHdcp2TzIfCpuKh = (MTDRM_TZIF_HDCP2_COMPUTE_KH *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_COMPUTE_KH), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfCpuKh)
    {
        printf("Allocate Alligned_rHdcp2TzIfCpuKh failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuKh->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2        
        i4Ret = TZ_HDCP2_ComputeKh(Alligned_rHdcp2TzIfCpuKh, sizeof(MTDRM_HDCP2_CALLPRIME_T));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuKh->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuKh);
    return i4Ret;
}

/**
Input parameter
? *pEkh_km : output buffer
? *pM : input message of AES crypto engine (128bit)
Output
? pEkh_km : encrypted Km value using kh key by AES mechanism
Operation
? Encrypted Km value using kh key by AES mechanism
**/
static MT_RESULT_T  _MTDRM_HDCP2_EncryptKmUsingKh(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T    rArg;
    unsigned char* pEkh_km;
    unsigned char* Alligned_Ekh_km = NULL; /* kernel data */
    unsigned char* pM;
    unsigned char* Alligned_M = NULL; /* kernel data */
    MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH *Alligned_rHdcp2TzIfEnKm = NULL;

    Alligned_rHdcp2TzIfEnKm = (MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfEnKm)
    {
        printf("Allocate Alligned_rHdcp2TzIfEnKm failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Ekh_km = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_EKH_KM, 32));/*kernel data*/
    if (!Alligned_Ekh_km)
    {
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEnKm);
	
        printf("Allocate Alligned_Ekh_km failed\n");
        return MTR_NOT_OK;
    }

    Alligned_M = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_M, 32));/*kernel data*/
    if (!Alligned_M)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEnKm);	
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Ekh_km);

		
        printf("Allocate Alligned_M failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pEkh_km = (unsigned char*)rArg.ai4Arg[0];
    pM      = (unsigned char*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEkh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
    COPY_FROM_USER_ARG_SIZE(pEkh_km, Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pM, MTDRM_HDCP2_KEYLEN_M);
    COPY_FROM_USER_ARG_SIZE(pM, Alligned_M, MTDRM_HDCP2_KEYLEN_M);
    
    Alligned_rHdcp2TzIfEnKm->pEkh_km = (VOID*)PHYSICAL((UINT32)Alligned_Ekh_km);
    Alligned_rHdcp2TzIfEnKm->pM = (VOID*)PHYSICAL((UINT32)Alligned_M);
    Alligned_rHdcp2TzIfEnKm->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
        TZ_NWD_BufferStart((UINT32)Alligned_M, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
        i4Ret = TZ_HDCP2_EncryptKmUsingKh(Alligned_rHdcp2TzIfEnKm, sizeof(MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH));
        TZ_NWD_BufferEnd((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
        TZ_NWD_BufferEnd((UINT32)Alligned_M, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
        
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfEnKm->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEkh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
        COPY_TO_USER_ARG_SIZE(pEkh_km, Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);

        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pM, MTDRM_HDCP2_KEYLEN_M);
        COPY_TO_USER_ARG_SIZE(pM, Alligned_M, MTDRM_HDCP2_KEYLEN_M);

        USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
        COPY_TO_USER_ARG(arg, rArg,MTAL_IOCTL_2ARG_T);
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
       
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEnKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Ekh_km);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_M);

    return i4Ret;    
}

/**
Input parameter
? pM : received random value from HDCP 2.0 TX
? ehk_km : encrypted value from HDCP 2.0 TX
Output
? Success/Failure
Operation
? AES Decrypt ekh_km which is received from HDCP 2.0 TX.
? M value is also received from HDCP 2.0 TX
? Decrypted  km must store to secure storage in the secure world.
**/
static MT_RESULT_T  _MTDRM_HDCP2_DecryptKmUsingKh(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T    rArg;
    unsigned char* pM;
    unsigned char* Alligned_M = NULL; /* kernel data */
    unsigned char* ekh_km;
    unsigned char* Alligned_kh_km = NULL; /* kernel data */
    MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH* Alligned_rHdcp2TzIfDecKm = NULL;

    Alligned_rHdcp2TzIfDecKm = (MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfDecKm)
    {
        printf("Allocate Alligned_rHdcp2TzIfDecKm failed\n");
        return MTR_NOT_OK;
    }

    Alligned_M = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_M, 32));/*kernel data*/
    if (!Alligned_M)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecKm);

        printf("Allocate Alligned_M failed\n");
        return MTR_NOT_OK;
    }

    Alligned_kh_km = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_EKH_KM, 32));/*kernel data*/
    if (!Alligned_kh_km)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecKm);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_M);
	
        printf("Allocate Alligned_kh_km failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pM = (unsigned char*)rArg.ai4Arg[0];
    ekh_km = (unsigned char*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
    COPY_FROM_USER_ARG_SIZE(ekh_km, Alligned_kh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pM, MTDRM_HDCP2_KEYLEN_M);
    COPY_FROM_USER_ARG_SIZE(pM, Alligned_M, MTDRM_HDCP2_KEYLEN_M);
    
    Alligned_rHdcp2TzIfDecKm->pM = (VOID*)PHYSICAL((UINT32)Alligned_M);
    Alligned_rHdcp2TzIfDecKm->ekh_km = (VOID*)PHYSICAL((UINT32)Alligned_kh_km);
    Alligned_rHdcp2TzIfDecKm->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_M, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
        TZ_NWD_BufferStart((UINT32)Alligned_kh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
        i4Ret = TZ_HDCP2_DecryptKmUsingKh(Alligned_rHdcp2TzIfDecKm, sizeof(MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH));
        TZ_NWD_BufferEnd((UINT32)Alligned_M, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
        TZ_NWD_BufferEnd((UINT32)Alligned_kh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));

        if (i4Ret != 0)
        {     
            i4Ret = Alligned_rHdcp2TzIfDecKm->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_M);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_kh_km);
    return i4Ret;
}

/**
Input parameter
? pEks : received session key from HDCP 2.0 TX.
? pRn , pRrx : random value  (64bit)
Output
? Success/Failure
Operation
? Decrypt ks using XOR operation with (dkey2 XOR rrx), Ks= eks XOR (dkey2 XOR rrx)
? Decrypted  ks must store to secure storage in the secure world.
**/
static MT_RESULT_T  _MTDRM_HDCP2_DecryptEKs(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_4ARG_T    rArg;
    unsigned char* pEks;
    unsigned char* pRtx;
    unsigned char* pRrx;
    unsigned char* pRn;
    unsigned char* Alligned_Eks = NULL; /* kernel data */
    unsigned char* Alligned_Rtx = NULL; /* kernel data */
    unsigned char* Alligned_Rrx = NULL; /* kernel data */
    unsigned char* Alligned_Rn = NULL; /* kernel data */
    MTDRM_TZIF_HDCP2_DECRYPT_EKS* Alligned_rHdcp2TzIfDecEks = NULL;
    Alligned_rHdcp2TzIfDecEks = (MTDRM_TZIF_HDCP2_DECRYPT_EKS *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_DECRYPT_EKS), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfDecEks)
    {
        printf("Allocate Alligned_rHdcp2TzIfDecEks failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Eks = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_KS, 32));/*kernel data*/
    if (!Alligned_Eks)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecEks);

        printf("Allocate Alligned_Eks failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Rtx = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RTX, 32));/*kernel data*/
    if (!Alligned_Rtx)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecEks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);

	
        printf("Allocate Alligned_Rtx failed\n");
        return MTR_NOT_OK;
    }

	
    Alligned_Rrx = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RRX, 32));/*kernel data*/
    if (!Alligned_Rrx)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecEks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rtx);    

	
        printf("Allocate Alligned_Rrx failed\n");
        return MTR_NOT_OK;
    }
    Alligned_Rn = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RRN, 32));/*kernel data*/
    if (!Alligned_Rn)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecEks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rtx);    
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rrx);
	
        printf("Allocate Alligned_Rn failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pEks = (unsigned char*)rArg.ai4Arg[0];
    pRtx = (unsigned char*)rArg.ai4Arg[1];
    pRrx = (unsigned char*)rArg.ai4Arg[2];
    pRn  = (unsigned char*)rArg.ai4Arg[3];    

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEks, MTDRM_HDCP2_KEYLEN_KS);
    COPY_FROM_USER_ARG_SIZE(pEks, Alligned_Eks, MTDRM_HDCP2_KEYLEN_KS);
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRtx, MTDRM_HDCP2_KEYLEN_RTX);
    COPY_FROM_USER_ARG_SIZE(pRtx, Alligned_Rtx, MTDRM_HDCP2_KEYLEN_RTX);   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRrx, MTDRM_HDCP2_KEYLEN_RRX);
    COPY_FROM_USER_ARG_SIZE(pRrx, Alligned_Rrx, MTDRM_HDCP2_KEYLEN_RRX);   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRn, MTDRM_HDCP2_KEYLEN_RRN);
    COPY_FROM_USER_ARG_SIZE(pRn, Alligned_Rn, MTDRM_HDCP2_KEYLEN_RRN);       
    Alligned_rHdcp2TzIfDecEks->pEks = (VOID*)PHYSICAL((UINT32)Alligned_Eks);
    Alligned_rHdcp2TzIfDecEks->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_Rtx);
    Alligned_rHdcp2TzIfDecEks->pRrx = (VOID*)PHYSICAL((UINT32)Alligned_Rrx);
    Alligned_rHdcp2TzIfDecEks->pRn = (VOID*)PHYSICAL((UINT32)Alligned_Rn);
    Alligned_rHdcp2TzIfDecEks->rRet = MTR_NOT_OK;
    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_Eks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferStart((UINT32)Alligned_Rtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
        TZ_NWD_BufferStart((UINT32)Alligned_Rrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));
        TZ_NWD_BufferStart((UINT32)Alligned_Rn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));
        i4Ret = TZ_HDCP2_DecryptEKs(Alligned_rHdcp2TzIfDecEks, sizeof(MTDRM_TZIF_HDCP2_DECRYPT_EKS));
        TZ_NWD_BufferEnd((UINT32)Alligned_Eks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfDecEks->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDecEks);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rtx);    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rrx);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rn);
    return i4Ret;
}

/**
Input parameter
? void
Output
? Succes/Fail
Operation
? Do XOR operation ks with lc128 and the results is stored in secure DRAM.
? The lc128 is stored in the secure storage.
? The ks is stored in the secure DRAM.
**/
static MT_RESULT_T  _MTDRM_HDCP2_KsXorLC128(unsigned long arg)
{
    INT32 i4Ret;
    MTDRM_TZIF_HDCP2_KS_XOR_LC128 *Alligned_rHdcp2TzKsXorLc128 = NULL;

    Alligned_rHdcp2TzKsXorLc128 = (MTDRM_TZIF_HDCP2_KS_XOR_LC128 *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_KS_XOR_LC128), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzKsXorLc128)
    {
        printf("Allocate Alligned_rHdcp2TzKsXorLc128 failed\n");
        return MTR_NOT_OK;
    }

    Alligned_rHdcp2TzKsXorLc128->rRet = MTR_NOT_OK;
    
    #ifdef CC_ENABLE_HDCP2
        i4Ret = TZ_HDCP2_KsXorLC128(Alligned_rHdcp2TzKsXorLc128, sizeof(MTDRM_TZIF_HDCP2_KS_XOR_LC128));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzKsXorLc128->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzKsXorLc128);
    return i4Ret;
}

/**
Input parameter
? pRiv : received 64-bit pseudo-random number Riv from HDCP 2.0 TX during SKE.
    Pid : The Pid of the stream that need to be decrypted
Output
? Success/Failure
Operation
? Set Riv and Pid of the encrypted stream to driver for data decryption
**/
static MT_RESULT_T  _MTDRM_HDCP2_SetRiv_Pid(unsigned long arg)
{
    int i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_RIV_T* pRiv;
    MTDRM_HDCP2_RIV_T* Alligned_Riv = NULL;/*kernel data*/
    UINT32 pid;
    MTDRM_TZIF_HDCP2_SetRiv_Pid* Alligned_rHdcp2TzIfSetRivPid = NULL;

    Alligned_rHdcp2TzIfSetRivPid = (MTDRM_TZIF_HDCP2_SetRiv_Pid *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_SetRiv_Pid), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfSetRivPid)
    {
        printf("Allocate Alligned_rHdcp2TzIfSetRivPid failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Riv = (MTDRM_HDCP2_RIV_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_RIV_T), 32));/*kernel data*/
    if (!Alligned_Riv)
    {
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfSetRivPid);

	
        printf("Allocate Alligned_Riv failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pRiv = (MTDRM_HDCP2_RIV_T*)rArg.ai4Arg[0];
    pid = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRiv, sizeof(MTDRM_HDCP2_RIV_T));
    COPY_FROM_USER_ARG_SIZE(pRiv, Alligned_Riv, sizeof(MTDRM_HDCP2_RIV_T));

    Alligned_rHdcp2TzIfSetRivPid->pRiv = (VOID*)PHYSICAL((UINT32)Alligned_Riv->au1Riv);
    Alligned_rHdcp2TzIfSetRivPid->pid = pid;
    Alligned_rHdcp2TzIfSetRivPid->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_Riv, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RIV_T)));
        i4Ret = TZ_HDCP2_SetRiv_Pid(Alligned_rHdcp2TzIfSetRivPid, sizeof(MTDRM_TZIF_HDCP2_SetRiv_Pid));
        TZ_NWD_BufferEnd((UINT32)Alligned_Riv, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RIV_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfSetRivPid->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif

    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfSetRivPid);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Riv);
    return i4Ret;
    //return DRV_HDCP2_SetRiv_Pid((unsigned char*)&Riv.au1Riv, pid);        
}

/**
Input parameter
? ks : session key
    lc128 : 128-bit secret global constant lc128
Output
? Success/Failure
Operation
? Set ks and LC128 manually for testing
**/
static MT_RESULT_T  _MTDRM_HDCP2_PresetKsLC128(unsigned long arg)
{
    int i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    unsigned char* pks;
    unsigned char* plc128;
    unsigned char* Alligned_ks = NULL;
    unsigned char* Alligned_lc128 = NULL;
    MTDRM_TZIF_HDCP2_PresetKsLC128* Alligned_rHdcp2TzIfPresetKsLC128 = NULL;

    Alligned_rHdcp2TzIfPresetKsLC128 = (MTDRM_TZIF_HDCP2_PresetKsLC128 *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_PresetKsLC128), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfPresetKsLC128)
    {
        printf("Allocate Alligned_rHdcp2TzIfPresetKsLC128 failed\n");
        return MTR_NOT_OK;
    }

    Alligned_ks = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_KS, 32));/*kernel data*/
    if (!Alligned_ks)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfPresetKsLC128);
	
        printf("Allocate Alligned_ks failed\n");
        return MTR_NOT_OK;
    }

    Alligned_lc128 = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_LC128, 32));/*kernel data*/
    if (!Alligned_lc128)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfPresetKsLC128);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_ks);
	
        printf("Allocate Alligned_lc128 failed\n");
        return MTR_NOT_OK;
    }
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pks = (unsigned char*)rArg.ai4Arg[0];
    plc128 = (unsigned char*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pks, MTDRM_HDCP2_KEYLEN_KS);
    COPY_FROM_USER_ARG_SIZE(pks, Alligned_ks, MTDRM_HDCP2_KEYLEN_KS);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(plc128, MTDRM_HDCP2_KEYLEN_LC128);
    COPY_FROM_USER_ARG_SIZE(plc128, Alligned_lc128, MTDRM_HDCP2_KEYLEN_LC128);

    Alligned_rHdcp2TzIfPresetKsLC128->pKs = (VOID*)PHYSICAL((UINT32)Alligned_ks);
    Alligned_rHdcp2TzIfPresetKsLC128->pLC128 = (VOID*)PHYSICAL((UINT32)Alligned_lc128);
    Alligned_rHdcp2TzIfPresetKsLC128->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_ks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferStart((UINT32)Alligned_lc128, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));
        i4Ret = TZ_HDCP2_PresetKsLC128(Alligned_rHdcp2TzIfPresetKsLC128, sizeof(MTDRM_TZIF_HDCP2_PresetKsLC128));
        TZ_NWD_BufferEnd((UINT32)Alligned_ks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferEnd((UINT32)Alligned_lc128, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));

        if (i4Ret != 0)
        {        
            i4Ret = Alligned_rHdcp2TzIfPresetKsLC128->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfPresetKsLC128);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_ks);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_lc128);
    return i4Ret;
    //return DRV_HDCP2_PresetKsLC128(ks, lc128);        
}

/** Generate Km
 *  @param pMaskedDB [out]99 bytes - XOR DB and DB mask, and adding 4bytes zero padding
 *  @param pDB       [in]95 bytes - Temp DB value for RSA encryption
 *  @param pDBmask   [in]95 bytes - calculated DB mask for RSA encryption
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
static MT_RESULT_T  _MTDRM_HDCP2_Generate_Km(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_3ARG_T    rArg;
    unsigned char* pMaskedDB;
    unsigned char* pDB;
    unsigned char* pDBmask;
    unsigned char* Alligned_MaskedDB = NULL; /* kernel data */
    unsigned char* Alligned_DB = NULL; /* kernel data */
    unsigned char* Alligned_DBmask = NULL; /* kernel data */

    MTDRM_TZIF_HDCP2_GENERATE_KM* Alligned_rHdcp2TzIfGenKm = NULL;
    Alligned_rHdcp2TzIfGenKm = (MTDRM_TZIF_HDCP2_GENERATE_KM *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_GENERATE_KM), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfGenKm)
    {
        printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
        return MTR_NOT_OK;
    }

    Alligned_MaskedDB = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_MASKED_DB, 32));/*kernel data*/
    if (!Alligned_MaskedDB)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKm);
	
        printf("Allocate Alligned_MaskedDB failed\n");
        return MTR_NOT_OK;
    }

    Alligned_DB = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_DB, 32));/*kernel data*/
    if (!Alligned_DB)
    {


		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKm);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_MaskedDB);

	
        printf("Allocate Alligned_DB failed\n");
        return MTR_NOT_OK;
    }
    Alligned_DBmask = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_DBMASK, 32));/*kernel data*/
    if (!Alligned_DBmask)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKm);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_MaskedDB);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_DB);	  

	
        printf("Allocate Alligned_DBmask failed\n");
        return MTR_NOT_OK;
    }


    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    pMaskedDB = (unsigned char*)rArg.ai4Arg[0];
    pDB = (unsigned char*)rArg.ai4Arg[1];
    pDBmask = (unsigned char*)rArg.ai4Arg[2];   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pMaskedDB, MTDRM_HDCP2_KEYLEN_MASKED_DB);
    COPY_FROM_USER_ARG_SIZE(pMaskedDB, Alligned_MaskedDB, MTDRM_HDCP2_KEYLEN_MASKED_DB);
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pDB, MTDRM_HDCP2_KEYLEN_DB);
    COPY_FROM_USER_ARG_SIZE(pDB, Alligned_DB, MTDRM_HDCP2_KEYLEN_DB);   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pDBmask, MTDRM_HDCP2_KEYLEN_DBMASK);
    COPY_FROM_USER_ARG_SIZE(pDBmask, Alligned_DBmask, MTDRM_HDCP2_KEYLEN_DBMASK);   

    Alligned_rHdcp2TzIfGenKm->pMaskedDB = (VOID*)PHYSICAL((UINT32)Alligned_MaskedDB);
    Alligned_rHdcp2TzIfGenKm->pDB = (VOID*)PHYSICAL((UINT32)Alligned_DB);
    Alligned_rHdcp2TzIfGenKm->pDBmask = (VOID*)PHYSICAL((UINT32)Alligned_DBmask);
    Alligned_rHdcp2TzIfGenKm->rRet = MTR_NOT_OK;
    
#ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_MaskedDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_MASKED_DB));
        TZ_NWD_BufferStart((UINT32)Alligned_DB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DB));
        TZ_NWD_BufferStart((UINT32)Alligned_DBmask, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DBMASK));
        i4Ret = TZ_HDCP2_Generate_Km(Alligned_rHdcp2TzIfGenKm, sizeof(MTDRM_TZIF_HDCP2_GENERATE_KM));
        TZ_NWD_BufferEnd((UINT32)Alligned_MaskedDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_MASKED_DB));
        TZ_NWD_BufferEnd((UINT32)Alligned_DB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DB));
        TZ_NWD_BufferEnd((UINT32)Alligned_DBmask, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DBMASK));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGenKm->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
#else
        i4Ret = MTR_NOT_OK;
#endif

    COPY_TO_USER_ARG_SIZE(pMaskedDB, Alligned_MaskedDB, MTDRM_HDCP2_KEYLEN_MASKED_DB);

    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_MaskedDB);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_DB);    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_DBmask);
    return i4Ret;

}

/** Generate Ks
 *  @param pEks    [out]Encrypted Ks using dkey2
 *  @param pRrx     [in]8 byte value
 *  @param pRtx     [in]8 byte value
 *  @param pRn      [in]8 byte random number
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
static MT_RESULT_T  _MTDRM_HDCP2_Generate_Ks(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_4ARG_T    rArg;
    unsigned char* pEks;
    unsigned char* pRrx;
    unsigned char* pRtx;
    unsigned char* pRn;
    unsigned char* Alligned_Eks = NULL; /* kernel data */
    unsigned char* Alligned_Rrx = NULL; /* kernel data */
    unsigned char* Alligned_Rtx = NULL; /* kernel data */
    unsigned char* Alligned_Rn = NULL; /* kernel data */
    MTDRM_TZIF_HDCP2_GENERATE_KS* Alligned_rHdcp2TzIfGenKs = NULL;
    Alligned_rHdcp2TzIfGenKs = (MTDRM_TZIF_HDCP2_GENERATE_KS *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_GENERATE_KS), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfGenKs)
    {
        printf("Allocate Alligned_rHdcp2TzIfGenKs failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Eks = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_KS, 32));/*kernel data*/
    if (!Alligned_Eks)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKs);
	
        printf("Allocate Alligned_Eks failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Rrx = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RRX, 32));/*kernel data*/
    if (!Alligned_Rrx)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKs);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
	
        printf("Allocate Alligned_Rrx failed\n");
        return MTR_NOT_OK;
    }
    Alligned_Rtx = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RTX, 32));/*kernel data*/
    if (!Alligned_Rtx)
    {
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKs);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rrx);    

	
        printf("Allocate Alligned_Rtx failed\n");
        return MTR_NOT_OK;
    }
    Alligned_Rn = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RRN, 32));/*kernel data*/
    if (!Alligned_Rn)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKs);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rrx);    
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rtx);
	
        printf("Allocate Alligned_Rn failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pEks = (unsigned char*)rArg.ai4Arg[0];
    pRrx = (unsigned char*)rArg.ai4Arg[1];
    pRtx = (unsigned char*)rArg.ai4Arg[2];
    pRn  = (unsigned char*)rArg.ai4Arg[3];    

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEks, MTDRM_HDCP2_KEYLEN_KS);
    COPY_FROM_USER_ARG_SIZE(pEks, Alligned_Eks, MTDRM_HDCP2_KEYLEN_KS);
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRrx, MTDRM_HDCP2_KEYLEN_RRX);
    COPY_FROM_USER_ARG_SIZE(pRrx, Alligned_Rrx, MTDRM_HDCP2_KEYLEN_RRX);   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRtx, MTDRM_HDCP2_KEYLEN_RTX);
    COPY_FROM_USER_ARG_SIZE(pRtx, Alligned_Rtx, MTDRM_HDCP2_KEYLEN_RTX);   

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRn, MTDRM_HDCP2_KEYLEN_RRN);
    COPY_FROM_USER_ARG_SIZE(pRn, Alligned_Rn, MTDRM_HDCP2_KEYLEN_RRN);       
    Alligned_rHdcp2TzIfGenKs->pEks = (VOID*)PHYSICAL((UINT32)Alligned_Eks);
    Alligned_rHdcp2TzIfGenKs->pRrx = (VOID*)PHYSICAL((UINT32)Alligned_Rrx);
    Alligned_rHdcp2TzIfGenKs->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_Rtx);
    Alligned_rHdcp2TzIfGenKs->pRn = (VOID*)PHYSICAL((UINT32)Alligned_Rn);
    Alligned_rHdcp2TzIfGenKs->rRet = MTR_NOT_OK;
    
#ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_Eks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferStart((UINT32)Alligned_Rrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));
        TZ_NWD_BufferStart((UINT32)Alligned_Rtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
        TZ_NWD_BufferStart((UINT32)Alligned_Rn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));
        i4Ret = TZ_HDCP2_Generate_Ks(Alligned_rHdcp2TzIfGenKs, sizeof(MTDRM_TZIF_HDCP2_GENERATE_KS));
        TZ_NWD_BufferEnd((UINT32)Alligned_Eks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
        TZ_NWD_BufferEnd((UINT32)Alligned_Rn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGenKs->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
#else
        i4Ret = MTR_NOT_OK;
#endif

    COPY_TO_USER_ARG_SIZE(pEks, Alligned_Eks, MTDRM_HDCP2_KEYLEN_KS);

    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenKs);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Eks);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rrx);    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rtx);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Rn);
    return i4Ret;

}

/**
? This operation could decrypted the PES data (16byte) in the normal world.
? It is normal AES-CTR crypto operation.
? Input
? pOutputbuf
? pInputbuf
? length
? pCounter (128bit)
? Output
? Success/fail
? Operation
? Decrypt message in input buffer using ks _XOR_lc128
? write the decrypted data to pOutputbuf
**/
static MT_RESULT_T  _MTDRM_HDCP2_DataDecrypt(unsigned long arg)
{
    static BOOL fgInited = FALSE;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 *pOutputbuf, *pInputbuf, *pCounter;
    INT32 length;
    UINT8 au1Iv[16];
    BOOL fgIommu = FALSE;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pOutputbuf = (UINT8 *)rArg.ai4Arg[0];
    pInputbuf = (UINT8 *)rArg.ai4Arg[1];
    length = (INT32)rArg.ai4Arg[2];
    pCounter = (UINT8 *)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pCounter, sizeof(au1Iv));
    COPY_FROM_USER_ARG_K(pCounter, au1Iv, sizeof(au1Iv));

    // alloc GCPU handler for HDCP
    if(!fgInited)
    {
        if(prGcpuHDCP == NULL)
        {
            prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
        }
        rParam.eMode = BLOCK_CIPHER_MODE_CTR;
        rParam.eKeyBitLen = KEY_BIT_LEN_128;
        prGcpuHDCP->u4AesKeyPtr = AES_HDCP2_SECURE_KEY_PTR;

        fgInited = TRUE;
    }

    rParam.u4SrcStartAddr = (UINT32)pInputbuf;
    rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pInputbuf);
    rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pInputbuf + length));
    rParam.u4DstStartAddr = (UINT32)pOutputbuf;
    rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pOutputbuf);
    rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pOutputbuf + length));
    rParam.u4DatLen = length - (length % 16);
    rParam.fgEncrypt = FALSE;
    memcpy(rParam.au1Iv, au1Iv, sizeof(au1Iv));

    if(GCPU_CmdEx((UINT32)prGcpuHDCP, GCPU_AES, &rParam, fgIommu) != S_GCPU_OK)
    {
        return MTR_NOT_OK;
    }

    if((length % 16) != 0)
    {
        UINT32 u4RemainingLen, u4ProcessedLen;
        u4RemainingLen = (length % 16);
        u4ProcessedLen = length - (length % 16);
        rParam.u4SrcStartAddr = (UINT32)(pInputbuf + u4ProcessedLen);
        rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pInputbuf);
        rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pInputbuf + length));
        rParam.u4DstStartAddr = (UINT32)(pOutputbuf + u4ProcessedLen);
        rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pOutputbuf);
        rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pOutputbuf + length));
        rParam.u4DatLen = u4RemainingLen;
        if(GCPU_CmdEx((UINT32)prGcpuHDCP, GCPU_AES, &rParam, fgIommu) != S_GCPU_OK)
        {
            return MTR_NOT_OK;
        }
    }
    return MTR_OK;
}

/**
    This operation could encrypt the PES data (16byte) in the normal world.
    It is normal AES-CTR crypto operation.
Input
    pOutputbuf
    pInputbuf
    length
    pCounter (128bit)
Output
    Success/fail
Operation
    Encrypt message in input buffer using ks _XOR_lc128
    write the encrypted data to pOutputbuf
**/
static MT_RESULT_T _MTDRM_HDCP2_DataEncrypt(unsigned long arg)
{
    static BOOL fgInited = FALSE;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 *pOutputbuf, *pInputbuf, *pCounter;
    INT32 length;
    UINT8 au1Iv[16];
    BOOL fgIommu = FALSE;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pOutputbuf = (UINT8 *)rArg.ai4Arg[0];
    pInputbuf = (UINT8 *)rArg.ai4Arg[1];
    length = (INT32)rArg.ai4Arg[2];
    pCounter = (UINT8 *)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pCounter, sizeof(au1Iv));
    COPY_FROM_USER_ARG_K(pCounter, au1Iv, sizeof(au1Iv));

    // alloc GCPU handler for HDCP
    if(!fgInited)
    {
        if(prGcpuHDCP == NULL)
        {
            prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
        }
        rParam.eMode = BLOCK_CIPHER_MODE_CTR;
        rParam.eKeyBitLen = KEY_BIT_LEN_128;
        prGcpuHDCP->u4AesKeyPtr = AES_HDCP2_SECURE_KEY_PTR;

        fgInited = TRUE;
    }

    rParam.u4SrcStartAddr = (UINT32)pInputbuf;
    rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pInputbuf);
    rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pInputbuf + length));
    rParam.u4DstStartAddr = (UINT32)pOutputbuf;
    rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pOutputbuf);
    rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pOutputbuf + length));
    rParam.u4DatLen = length - (length % 16);
    rParam.fgEncrypt = TRUE;
    memcpy(rParam.au1Iv, au1Iv, sizeof(au1Iv));
    
    if(GCPU_CmdEx((UINT32)prGcpuHDCP, GCPU_AES, &rParam, fgIommu) != S_GCPU_OK)
    {
        return MTR_NOT_OK;
    }

    if((length % 16) != 0)
    {
        UINT32 u4RemainingLen, u4ProcessedLen;
        u4RemainingLen = (length % 16);
        u4ProcessedLen = length - (length % 16);
        rParam.u4SrcStartAddr = (UINT32)(pInputbuf + u4ProcessedLen);
        rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pInputbuf);
        rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pInputbuf + length));
        rParam.u4DstStartAddr = (UINT32)(pOutputbuf + u4ProcessedLen);
        rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pOutputbuf);
        rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pOutputbuf + length));
        rParam.u4DatLen = u4RemainingLen;
        if(GCPU_CmdEx((UINT32)prGcpuHDCP, GCPU_AES, &rParam, fgIommu) != S_GCPU_OK)
        {
            return MTR_NOT_OK;
        }
    }
    return MTR_OK;
}

/**
? This operation could set Km with the encrypted one previously got.
? Input
? pOu1EncKm encrypted Km
? Output
? Success/fail
? Operation
? Dset the km got before
**/
static MT_RESULT_T _MTDRM_HDCP2_SetEncKm(unsigned long arg)
{
    INT32 i4Ret;
    UINT8 *pu1EncKm;
    UINT8 *pu1VirtEncKm;
    MTDRM_TZIF_HDCP2_ENC_KM *Alligned_rHdcp2TzEncKm = NULL;

    pu1EncKm = (UINT8*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1EncKm, char[MTDRM_HDCP2_KEYLEN_ENCKM]);

    Alligned_rHdcp2TzEncKm = (MTDRM_TZIF_HDCP2_ENC_KM *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_ENC_KM), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzEncKm)
    {
        printf("Allocate Alligned_rHdcp2TzEncKm failed\n");
        return MTR_NOT_OK;
    }

    Alligned_rHdcp2TzEncKm->pEncKm = (unsigned char*)BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_ENCKM, 32);
    if(!Alligned_rHdcp2TzEncKm->pEncKm) 
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);

        printf("Allocate Alligned_rHdcp2TzEncKm->pEncKm failed\n");
        BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
        return MTR_NOT_OK;
    }
	
    pu1VirtEncKm = (UINT8*)VIRTUAL((UINT32)Alligned_rHdcp2TzEncKm->pEncKm);
    Alligned_rHdcp2TzEncKm->rRet = MTR_NOT_OK;
    COPY_FROM_USER_ARG_SIZE(pu1EncKm, pu1VirtEncKm, MTDRM_HDCP2_KEYLEN_ENCKM);
    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)pu1VirtEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
        i4Ret = TZ_HDCP2_SetEncKm(Alligned_rHdcp2TzEncKm, sizeof(MTDRM_TZIF_HDCP2_ENC_KM));
        TZ_NWD_BufferEnd((UINT32)pu1VirtEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzEncKm->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)pu1VirtEncKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
    return i4Ret;
}

/**
? This operation could get the encrypted Km 
? Input
? pOu1EncKm encrypted Km will be filled after call
? Output
? Success/fail
? Operation
? Dget the km got before
**/
static MT_RESULT_T _MTDRM_HDCP2_GetEncKm(unsigned long arg)
{
    INT32 i4Ret;
    UINT8 *pu1EncKm;
    UINT8 *pu1VirtEncKm;
    MTDRM_TZIF_HDCP2_ENC_KM *Alligned_rHdcp2TzEncKm = NULL;

    Alligned_rHdcp2TzEncKm = (MTDRM_TZIF_HDCP2_ENC_KM *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_ENC_KM), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzEncKm)
    {
        printf("Allocate Alligned_rHdcp2TzEncKm failed\n");
        return MTR_NOT_OK;
    }

    Alligned_rHdcp2TzEncKm->pEncKm = (unsigned char*)BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_ENCKM, 32);
    if(!Alligned_rHdcp2TzEncKm->pEncKm) 
    {
		
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);

	
        printf("Allocate Alligned_rHdcp2TzEncKm->pEncKm failed\n");
        BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
        return MTR_NOT_OK;
    }
    pu1VirtEncKm = (UINT8*)VIRTUAL((UINT32)Alligned_rHdcp2TzEncKm->pEncKm);
    Alligned_rHdcp2TzEncKm->rRet = MTR_NOT_OK;
    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)pu1VirtEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
        i4Ret = TZ_HDCP2_GetEncKm(Alligned_rHdcp2TzEncKm, sizeof(MTDRM_TZIF_HDCP2_ENC_KM));
        TZ_NWD_BufferEnd((UINT32)pu1VirtEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzEncKm->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif

    pu1EncKm = (UINT8*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1EncKm, char[MTDRM_HDCP2_KEYLEN_ENCKM]);
    COPY_TO_USER_ARG_SIZE(pu1EncKm, pu1VirtEncKm, MTDRM_HDCP2_KEYLEN_ENCKM);

    BSP_FreeAlignedDmaMemory((UINT32)pu1VirtEncKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
    return i4Ret;
}


/**
Input parameter
? pRtx : received 64bit random value from HDCP 2.0 TX

Output
? Status : success or fail

Operation
? DDI derived kd value in the secure world.
? This DDI call the AES-CTR DDI twice and generate dkey0 and dkey1 using Ctr 0 and 1 respectively.
    And derived kd is stored in secure storge
**/
static MT_RESULT_T  _MTDRM_HDCP2_2_KdKeyDerivation(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_3ARG_T rArg;
    MTDRM_HDCP2_RTX_T *prRtx;


    UINT32 u4Len;


    MTDRM_HDCP2_RTX_T* Alligned_rRtx = NULL;
    MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION* Alligned_rHdcp2TzIfKdDer = NULL;

	MTDRM_HDCP2_RRX_T *prRrx;
	MTDRM_HDCP2_RRX_T* Alligned_rRrx = NULL;


    Alligned_rRtx = (MTDRM_HDCP2_RTX_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_RTX_T), 32));/*kernel data*/
    if (!Alligned_rRtx)
    {
        printf("Allocate Alligned_rRtx failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfKdDer = (MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION), 32));
    if (!Alligned_rHdcp2TzIfKdDer)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRtx);
	
        printf("Allocate Alligned_rHdcp2TzIfKdDer failed\n");
        return MTR_NOT_OK;
    }    


    Alligned_rRrx = (MTDRM_HDCP2_RRX_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_RRX_T), 32));/*kernel data*/
    if (!Alligned_rRrx)
    {
	    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfKdDer);
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRtx);
		
        printf("Allocate Alligned_rRrx failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    prRtx = (MTDRM_HDCP2_RTX_T*)rArg.ai4Arg[0];
	prRrx = (MTDRM_HDCP2_RRX_T*)rArg.ai4Arg[1];	
    u4Len = (UINT32)rArg.ai4Arg[2];


	mtdrm_hdcp_DumpHex("rtx:", prRtx->au1rTx, MTDRM_HDCP2_KEYLEN_RTX);
	mtdrm_hdcp_DumpHex("rrx:", prRrx->au1rRx, MTDRM_HDCP2_KEYLEN_RRX);
	
	//printf("u4len(%02x) (%d)\n", u4Len);
	//u4Len = MTDRM_HDCP2_KEYLEN_RTX;
	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prRtx, u4Len);
    COPY_FROM_USER_ARG_SIZE(prRtx, Alligned_rRtx, u4Len);
	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prRrx, u4Len);
    COPY_FROM_USER_ARG_SIZE(prRrx, Alligned_rRrx, u4Len);


    Alligned_rHdcp2TzIfKdDer->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_rRtx->au1rTx);
    Alligned_rHdcp2TzIfKdDer->pRrx = (VOID*)PHYSICAL((UINT32)Alligned_rRrx->au1rRx);	
    Alligned_rHdcp2TzIfKdDer->u4length = u4Len;
    Alligned_rHdcp2TzIfKdDer->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rRtx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RTX_T)));
        TZ_NWD_BufferStart((UINT32)Alligned_rRrx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RRX_T)));	
        i4Ret = TZ_HDCP2_2_KdKeyDerivation(Alligned_rHdcp2TzIfKdDer, sizeof(MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION));    
        TZ_NWD_BufferEnd((UINT32)Alligned_rRtx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RTX_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_rRrx, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_RRX_T)));

	
        if (i4Ret != 0)		/*TRUE */
        {        
            i4Ret = Alligned_rHdcp2TzIfKdDer->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfKdDer);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRtx);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rRrx);	
    return(i4Ret);

}

/**
Input parameter
?? pH_prime : output buffer
?? Repeater: repeater flag (1byte) (0x00 or 0x01)
?? pRtx : received random value (64bit)

Output
?? Status : success or fail

Operation
?? DDI generates hash code using HMAC-SHA256. HMAC-SHA256(pRtx XOR Repeater, kd)
     - Key ?V kd (from secure DRAM)
     - Key length ?V 32 byte
     - Message length = 8 byte
     - Target message ?V pRtx XOR Repeater
     - Output hashed value ?V pH_prime(256bit)
    Write result H_prime value to pH_prime buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_2_ComputeHprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_CALHPRIME_T *prCalHprime;
    MTDRM_HDCP2_CALHPRIME_T *Alligned_rCalHprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME *Alligned_rHdcp2TzIfCpuHprime = NULL;
    UINT32 u4Len;

    Alligned_rCalHprime = (MTDRM_HDCP2_CALHPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_CALHPRIME_T), 32));/*kernel data*/
    
    if (!Alligned_rCalHprime)
    {
        printf("Allocate Alligned_rCalHprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuHprime = (MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME), 32));
    if (!Alligned_rHdcp2TzIfCpuHprime)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalHprime);
	
        printf("Allocate Alligned_rHdcp2TzIfCpuHprime failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalHprime = (MTDRM_HDCP2_CALHPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalHprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalHprime, Alligned_rCalHprime, u4Len);

    Alligned_rHdcp2TzIfCpuHprime->pH_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalHprime->au1rHprime);
    Alligned_rHdcp2TzIfCpuHprime->Repeater = Alligned_rCalHprime->u1Repeater;
    Alligned_rHdcp2TzIfCpuHprime->pRtx = (VOID*)PHYSICAL((UINT32)Alligned_rCalHprime->au1rTx); 



	Alligned_rHdcp2TzIfCpuHprime->rx_version   = Alligned_rCalHprime->rx_version;
	Alligned_rHdcp2TzIfCpuHprime->rx_cap_mask  = *((UINT16 *)Alligned_rCalHprime->rx_cap_mask); 
	Alligned_rHdcp2TzIfCpuHprime->tx_version   = Alligned_rCalHprime->tx_version;
	Alligned_rHdcp2TzIfCpuHprime->tx_cap_mask  =  *((UINT16 *)Alligned_rCalHprime->tx_cap_mask); 


    Alligned_rHdcp2TzIfCpuHprime->rRet = MTR_NOT_OK;    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalHprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALHPRIME_T)));
        i4Ret = TZ_HDCP2_2_ComputeHprime(Alligned_rHdcp2TzIfCpuHprime,sizeof(MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalHprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALHPRIME_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuHprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalHprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalHprime, Alligned_rCalHprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalHprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuHprime);
    return i4Ret;
}

/**
Input parameter
? pL_prime : output buffer
? pRn : received random value form HDCP 2.0 TX
? pRtx: received random value form HDCP 2.0 TX

Output
? Status : success or fail

Operation
? L? = HMAC-SHA256(pRn, kd XOR pRrx)
? Key ? kd XOR pRrx (kd from secure DRAM)
? Key length ? 32 byte
? Message length = 8 byte
? Target message ? pRn
? Output? pL_prime(256bit)
? write result L_prime value to pL_prime buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_2_ComputeLprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_CALLPRIME_T *prCalLprime;
    MTDRM_HDCP2_CALLPRIME_T *Alligned_rCalLprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME *Alligned_rHdcp2TzIfCpuLprime = NULL;
    UINT32 u4Len;

    Alligned_rCalLprime = (MTDRM_HDCP2_CALLPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_CALLPRIME_T), 32));/*kernel data*/
    if (!Alligned_rCalLprime)
    {
        printf("Allocate Alligned_rCalLprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuLprime = (MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME), 32));
    if (!Alligned_rHdcp2TzIfCpuLprime)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalLprime);
	
        printf("Allocate Alligned_rHdcp2TzIfCpuLprime failed\n");
        return MTR_NOT_OK;
    }    

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalLprime = (MTDRM_HDCP2_CALLPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalLprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalLprime, Alligned_rCalLprime, u4Len);

    Alligned_rHdcp2TzIfCpuLprime->pL_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rLprime);
    Alligned_rHdcp2TzIfCpuLprime->pRn = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rRn);
    Alligned_rHdcp2TzIfCpuLprime->pRrx = (VOID*)PHYSICAL((UINT32)Alligned_rCalLprime->au1rRx); 
    Alligned_rHdcp2TzIfCpuLprime->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalLprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALLPRIME_T)));
        i4Ret = TZ_HDCP2_2_ComputeLprime(Alligned_rHdcp2TzIfCpuLprime, sizeof(MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalLprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CALLPRIME_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuLprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalLprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalLprime, Alligned_rCalLprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalLprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuLprime);
    return i4Ret;
}










/*
	for HDCP2.X align
*/

static MT_RESULT_T  _MTDRM_HDCP2_Set_EncKeySet_into_TZ(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_KEYSET_ENC_T *prHdcp2EncKeySet;/*kernel data*/
    UINT32 u4Len;
    INT32 i4Ret = 0;
    MTDRM_HDCP2_KEYSET_ENC_T *Alligned_rHdcp2EncKeySet = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET *Alligned_rHdcp2TzIfEncKeySet = NULL;
	


    Alligned_rHdcp2EncKeySet = (MTDRM_HDCP2_KEYSET_ENC_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_KEYSET_ENC_T), 32));/*kernel data*/
    if (!Alligned_rHdcp2EncKeySet)
    {
        printf("Allocate Alligned_rHdcp2EncKeySet failed\n");
        return MTR_NOT_OK;
    }    
    Alligned_rHdcp2TzIfEncKeySet = (MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET), 32));/*kernel data*/ 
    if (!Alligned_rHdcp2TzIfEncKeySet)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKeySet);

	
        printf("Allocate Alligned_rHdcp2TzIfEncKeySet failed\n");
        return MTR_NOT_OK;
    }
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2EncKeySet = (MTDRM_HDCP2_KEYSET_ENC_T *)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2EncKeySet, u4Len);
    COPY_FROM_USER_ARG_SIZE(prHdcp2EncKeySet, Alligned_rHdcp2EncKeySet, u4Len);

    Alligned_rHdcp2TzIfEncKeySet->pKeySet = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2EncKeySet->au1EncKeySet);
    Alligned_rHdcp2TzIfEncKeySet->u4length = u4Len;
    Alligned_rHdcp2TzIfEncKeySet->rRet = MTR_NOT_OK;
#ifdef CC_ENABLE_HDCP2
	printf("%s: CC_ENABLE_HDCP2 defined.\n", __FUNCTION__);
#else
	printf("%s: CC_ENABLE_HDCP2 undefined.\n", __FUNCTION__);
#endif

	
    #ifdef CC_ENABLE_HDCP2
	
	    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2EncKeySet, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYSET_ENC_T)));
	    i4Ret = TZ_HDCP2_SetEncKeySet(Alligned_rHdcp2TzIfEncKeySet, sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET));    
	    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2EncKeySet, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYSET_ENC_T)));

		printf("_MTDRM_HDCP2_Set_EncKeySet_into_TZ: i4Ret(%d) Alligned_rHdcp2TzIfEncKeySet->rRet(%d) \n", 
				i4Ret, Alligned_rHdcp2TzIfEncKeySet->rRet);	
	
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfEncKeySet->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
	
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKeySet);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEncKeySet);
    return i4Ret;
}

static MT_RESULT_T  _MTDRM_HDCP2_Set_MTK_EncKeySet_into_TZ(unsigned char *keyset, int keyset_len)
{
    //MTAL_IOCTL_2ARG_T rArg;
    //MTDRM_HDCP2_KEYSET_ENC_T *prHdcp2EncKeySet;/*kernel data*/
    //UINT32 u4Len;
    INT32 i4Ret = 0;
    MTDRM_HDCP2_KEYSET_ENC_T *Alligned_rHdcp2EncKeySet = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET *Alligned_rHdcp2TzIfEncKeySet = NULL;

	printf(">>%s:\n", __FUNCTION__);
	
	if(keyset == NULL || keyset_len> MTDRM_HDCP2_KEYSET_LEN_MAX)
	{
	    printf("invalid parameters\n");
        return MTR_NOT_OK;	
	}
	
    Alligned_rHdcp2EncKeySet = (MTDRM_HDCP2_KEYSET_ENC_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_KEYSET_ENC_T), 32));/*kernel data*/
    if (!Alligned_rHdcp2EncKeySet)
    {
        printf("Allocate Alligned_rHdcp2EncKeySet failed\n");
        return MTR_NOT_OK;
    }    
    Alligned_rHdcp2TzIfEncKeySet = (MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET), 32));/*kernel data*/ 
    if (!Alligned_rHdcp2TzIfEncKeySet)
    {

		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKeySet);

	
        printf("Allocate Alligned_rHdcp2TzIfEncKeySet failed\n");
        return MTR_NOT_OK;
    }


    //prHdcp2EncKeySet = (MTDRM_HDCP2_KEYSET_ENC_T *)rArg.ai4Arg[0];
    //u4Len = (UINT32)rArg.ai4Arg[1];
    
    //USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2EncKeySet, u4Len);
    //COPY_FROM_USER_ARG_SIZE(prHdcp2EncKeySet, Alligned_rHdcp2EncKeySet, u4Len);

    //rArg.ai4Arg[0] = (INT32)pEncKeySet;
    //rArg.ai4Arg[1] = (INT32)sizeof(MTDRM_HDCP2_KEYSET_ENC_T);

	
	Alligned_rHdcp2EncKeySet->au1KeyType = MTDRM_HDCP2_KEY_SET_TYPE_NORMAL;
	Alligned_rHdcp2EncKeySet->au1KeySize = MTDRM_HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE;
	memcpy(Alligned_rHdcp2EncKeySet->au1EncKeySet, keyset, keyset_len);



    Alligned_rHdcp2TzIfEncKeySet->pKeySet = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2EncKeySet->au1EncKeySet);
    Alligned_rHdcp2TzIfEncKeySet->u4length = sizeof(MTDRM_HDCP2_KEYSET_ENC_T);
    Alligned_rHdcp2TzIfEncKeySet->rRet = MTR_NOT_OK;
#ifdef CC_ENABLE_HDCP2
	printf("%s: CC_ENABLE_HDCP2 defined.\n", __FUNCTION__);
#else
	printf("%s: CC_ENABLE_HDCP2 undefined.\n", __FUNCTION__);
#endif

	
    #ifdef CC_ENABLE_HDCP2
	
	    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2EncKeySet, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYSET_ENC_T)));
	    i4Ret = TZ_HDCP2_SetEncKeySet(Alligned_rHdcp2TzIfEncKeySet, sizeof(MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET));    
	    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2EncKeySet, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_KEYSET_ENC_T)));

		printf("_MTDRM_HDCP2_Set_EncKeySet_into_TZ: i4Ret(%d) Alligned_rHdcp2TzIfEncKeySet->rRet(%d) \n", 
				i4Ret, Alligned_rHdcp2TzIfEncKeySet->rRet);	
	
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfEncKeySet->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
	
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2EncKeySet);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfEncKeySet);
    return i4Ret;
}



/* for hdcp tx */

/** Generate Km
 *	@param pEkh_km [out]99 bytes - XOR DB and DB mask, and adding 4bytes zero padding
 *	@param pm		[in]95 bytes - Temp DB value for RSA encryption
 *	@param pRX_ID	[in]95 bytes - calculated DB mask for RSA encryption
 *	@retval MTR_OK			Success.
 *	@retval MTR_NOT_OK		Fail.
 */
static MT_RESULT_T	_MTDRM_HDCP2_RetrvEkh_km(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_3ARG_T	 rArg;
	unsigned char* pEkh_km;
	unsigned char* pm;
	unsigned char* pRX_ID;
	unsigned char* Alligned_Ekh_km = NULL; /* kernel data */
	unsigned char* Alligned_m = NULL;	   /* kernel data */
	unsigned char* Alligned_RX_ID = NULL; /* kernel data */

	MTDRM_TZIF_HDCP2_RETRV_EKH_KM* Alligned_rHdcp2TzIfRetrvEkhkm = NULL;
	Alligned_rHdcp2TzIfRetrvEkhkm = (MTDRM_TZIF_HDCP2_RETRV_EKH_KM *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_RETRV_EKH_KM), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfRetrvEkhkm)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}

	Alligned_Ekh_km = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_EKH_KM, 32));/*kernel data*/
	if (!Alligned_Ekh_km)
	{
		printf("Allocate Alligned_Ekh_km failed\n");
		return MTR_NOT_OK;
	}

	Alligned_m = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_M, 32));/*kernel data*/
	if (!Alligned_m)
	{
		printf("Allocate Alligned_m failed\n");
		return MTR_NOT_OK;
	}
	Alligned_RX_ID = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RXID, 32));/*kernel data*/
	if (!Alligned_RX_ID)
	{
		printf("Allocate Alligned_RX_ID failed\n");
		return MTR_NOT_OK;
	}


	USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
	COPY_FROM_USER_3ARG(arg, rArg);

	pEkh_km = (unsigned char*)rArg.ai4Arg[0];
	pm = (unsigned char*)rArg.ai4Arg[1];
	pRX_ID = (unsigned char*)rArg.ai4Arg[2];   

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEkh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
	COPY_FROM_USER_ARG_SIZE(pEkh_km, Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
	
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pm, MTDRM_HDCP2_KEYLEN_M);
	COPY_FROM_USER_ARG_SIZE(pm, Alligned_m, MTDRM_HDCP2_KEYLEN_M);	 

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRX_ID, MTDRM_HDCP2_KEYLEN_RXID);
	COPY_FROM_USER_ARG_SIZE(pRX_ID, Alligned_RX_ID, MTDRM_HDCP2_KEYLEN_RXID);	

	Alligned_rHdcp2TzIfRetrvEkhkm->pEkh_km = (VOID*)PHYSICAL((UINT32)Alligned_Ekh_km);
	Alligned_rHdcp2TzIfRetrvEkhkm->pm = (VOID*)PHYSICAL((UINT32)Alligned_m);
	Alligned_rHdcp2TzIfRetrvEkhkm->pRX_ID = (VOID*)PHYSICAL((UINT32)Alligned_RX_ID);
	Alligned_rHdcp2TzIfRetrvEkhkm->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
		TZ_NWD_BufferStart((UINT32)Alligned_m, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
		TZ_NWD_BufferStart((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));
		i4Ret = TZ_HDCP2_RetrvEkh_km(Alligned_rHdcp2TzIfRetrvEkhkm, sizeof(MTDRM_TZIF_HDCP2_RETRV_EKH_KM));  
		TZ_NWD_BufferEnd((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
		TZ_NWD_BufferEnd((UINT32)Alligned_m, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
		TZ_NWD_BufferEnd((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfRetrvEkhkm->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif

	COPY_TO_USER_ARG_SIZE(pEkh_km, Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
	COPY_TO_USER_ARG_SIZE(pm, Alligned_m, MTDRM_HDCP2_KEYLEN_M);

	mtdrm_hdcp_DumpHex("ekhkm:", Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
	mtdrm_hdcp_DumpHex("m:", Alligned_m, MTDRM_HDCP2_KEYLEN_M);	

	

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfRetrvEkhkm);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_Ekh_km);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_m);	 
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_RX_ID);
	return i4Ret;

}



/* save RX PairingInfo*/
static MT_RESULT_T  _MTDRM_HDCP2_SavePairingInfo(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_3ARG_T    rArg;
	
    unsigned char* pRX_ID;
    unsigned char* pEkh_km;
    unsigned char* pm;
	
    unsigned char* Alligned_RX_ID = NULL; 	/* kernel data */
    unsigned char* Alligned_Ekh_km = NULL;  /* kernel data */
    unsigned char* Alligned_m = NULL;       /* kernel data */
	
    MTDRM_TZIF_HDCP2_SaveRxPairingInfo* Alligned_rHdcp2TzIfSavePairingInfo = NULL;
    Alligned_rHdcp2TzIfSavePairingInfo = (MTDRM_TZIF_HDCP2_SaveRxPairingInfo *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_SaveRxPairingInfo), 32));/*kernel data*/
    if (!Alligned_rHdcp2TzIfSavePairingInfo)
    {
        printf("Allocate Alligned_rHdcp2TzIfSavePairingInfo failed\n");
        return MTR_NOT_OK;
    }

    Alligned_RX_ID = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RXID, 32));/*kernel data*/
    if (!Alligned_RX_ID)
    {
        printf("Allocate Alligned_RX_ID failed\n");
        return MTR_NOT_OK;
    }

    Alligned_Ekh_km = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_EKH_KM, 32));/*kernel data*/
    if (!Alligned_Ekh_km)
    {
        printf("Allocate Alligned_Ekh_km failed\n");
        return MTR_NOT_OK;
    }

    Alligned_m = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_M, 32));/*kernel data*/
    if (!Alligned_m)
    {
        printf("Allocate Alligned_m failed\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    pRX_ID = (unsigned char*)rArg.ai4Arg[0];
    pEkh_km = (unsigned char*)rArg.ai4Arg[1];
    pm  = (unsigned char*)rArg.ai4Arg[2];    

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRX_ID, MTDRM_HDCP2_KEYLEN_RXID);
    COPY_FROM_USER_ARG_SIZE(pRX_ID, Alligned_RX_ID, MTDRM_HDCP2_KEYLEN_RXID);
    
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pEkh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);
    COPY_FROM_USER_ARG_SIZE(pEkh_km, Alligned_Ekh_km, MTDRM_HDCP2_KEYLEN_EKH_KM);   


    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pm, MTDRM_HDCP2_KEYLEN_M);
    COPY_FROM_USER_ARG_SIZE(pm, Alligned_m, MTDRM_HDCP2_KEYLEN_M);       
    Alligned_rHdcp2TzIfSavePairingInfo->pRX_ID = (VOID*)PHYSICAL((UINT32)Alligned_RX_ID);
    Alligned_rHdcp2TzIfSavePairingInfo->pEkh_km = (VOID*)PHYSICAL((UINT32)Alligned_Ekh_km);
    Alligned_rHdcp2TzIfSavePairingInfo->pm = (VOID*)PHYSICAL((UINT32)Alligned_m);
    Alligned_rHdcp2TzIfSavePairingInfo->rRet = MTR_NOT_OK;
    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));
        TZ_NWD_BufferStart((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
        TZ_NWD_BufferStart((UINT32)Alligned_m, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));
        i4Ret = TZ_HDCP2_SavePairingInfo(Alligned_rHdcp2TzIfSavePairingInfo, sizeof(MTDRM_TZIF_HDCP2_SaveRxPairingInfo));
        TZ_NWD_BufferEnd((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));
        TZ_NWD_BufferEnd((UINT32)Alligned_Ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));
        TZ_NWD_BufferEnd((UINT32)Alligned_m, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_M));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfSavePairingInfo->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfSavePairingInfo);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_RX_ID);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_Ekh_km);    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_m);
    return i4Ret;
}















/* check if the RX ID was saved or not */
static MT_RESULT_T  _MTDRM_HDCP2_CheckRxID(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T	 rArg;

	unsigned char* pRX_ID;
	unsigned char* Alligned_RX_ID = NULL; /* kernel data */

	MTDRM_TZIF_HDCP2_CheckRxID* Alligned_rHdcp2TzIfCheckRxID = NULL;
	Alligned_rHdcp2TzIfCheckRxID = (MTDRM_TZIF_HDCP2_CheckRxID *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_CheckRxID), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfCheckRxID)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}


	Alligned_RX_ID = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RXID, 32));/*kernel data*/
	if (!Alligned_RX_ID)
	{
		printf("Allocate Alligned_RX_ID failed\n");
		return MTR_NOT_OK;
	}


	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	pRX_ID = (unsigned char*)rArg.ai4Arg[0];   

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRX_ID, MTDRM_HDCP2_KEYLEN_RXID);
	COPY_FROM_USER_ARG_SIZE(pRX_ID, Alligned_RX_ID, MTDRM_HDCP2_KEYLEN_RXID);	

	Alligned_rHdcp2TzIfCheckRxID->pRX_ID = (VOID*)PHYSICAL((UINT32)Alligned_RX_ID);
	Alligned_rHdcp2TzIfCheckRxID->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2

		TZ_NWD_BufferStart((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));
		i4Ret = TZ_HDCP2_CheckRxID(Alligned_rHdcp2TzIfCheckRxID, sizeof(MTDRM_TZIF_HDCP2_CheckRxID));  

		TZ_NWD_BufferEnd((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfCheckRxID->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif



	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCheckRxID);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_RX_ID);

	
	return i4Ret;

}



/**
Input parameter
? pH_prime : output buffer
? Repeater: repeater flag (1byte) (0x00 or 0x01)
? pRtx : received random value (64bit)

Output
? Status : success or fail

Operation
? DDI generates hash code using HMAC-SHA256. HMAC-SHA256(pRtx XOR Repeater, kd)
     - Key ? kd (from secure DRAM)
     - Key length ? 32 byte
     - Message length = 8 byte
     - Target message ? pRtx XOR Repeater
     - Output hashed value ? pH_prime(256bit)
    Write result H_prime value to pH_prime buffer
**/
static MT_RESULT_T  _MTDRM_HDCP2_ComputeMprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_2_CALMPRIME_T *prCalMprime;
    MTDRM_HDCP2_2_CALMPRIME_T *Alligned_rCalMprime = NULL;/*kernel data*/

	unsigned char *prStreamType = NULL;				/* user space data */
	unsigned char *Alligned_rStreamType = NULL;     /* kernel space date into TEE */
	int stream_type_len = 0;
	
    MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME *Alligned_rHdcp2TzIfCpuMprime = NULL;
    UINT32 u4Len;

    Alligned_rCalMprime = (MTDRM_HDCP2_2_CALMPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_2_CALMPRIME_T), 32));/*kernel data*/
    
    if (!Alligned_rCalMprime)
    {
        printf("Allocate Alligned_rCalMprime failed\n");
        return MTR_NOT_OK;
    }

	
    Alligned_rHdcp2TzIfCpuMprime = (MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME), 32));
    if (!Alligned_rHdcp2TzIfCpuMprime)
    {
        printf("Allocate Alligned_rHdcp2TzIfCpuMprime failed\n");
        return MTR_NOT_OK;
    }    



	
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalMprime = (MTDRM_HDCP2_2_CALMPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalMprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalMprime, Alligned_rCalMprime, u4Len);


	/* copy stream type from user space*/
	prStreamType = prCalMprime->STREAMID_TYPE;
	stream_type_len = prCalMprime->STREAMID_TYPE_LEN;


    Alligned_rStreamType = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(stream_type_len, 32));
    if (!Alligned_rStreamType)
    {
        printf("Allocate Alligned_rStreamType failed\n");
        return MTR_NOT_OK;
    }    


	/* copy user space id list into KERNEL */	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prStreamType, stream_type_len);
    COPY_FROM_USER_ARG_SIZE(prStreamType, Alligned_rStreamType, stream_type_len);


	/* debug */
	mtdrm_hdcp_DumpHex("stream type", Alligned_rStreamType, stream_type_len);
	printf("stream type len:%d\n", stream_type_len);


	/* convert the kernel space(aligned) to PHYSICAL add */
    Alligned_rHdcp2TzIfCpuMprime->pM_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalMprime->au1rMprime);
    Alligned_rHdcp2TzIfCpuMprime->STREAMID_TYPE = (VOID*)PHYSICAL((UINT32)Alligned_rStreamType);
    Alligned_rHdcp2TzIfCpuMprime->seq_num_M = (VOID*)PHYSICAL((UINT32)Alligned_rCalMprime->seq_num_M); 
	Alligned_rHdcp2TzIfCpuMprime->STREAMID_TYPE_LEN = Alligned_rCalMprime->STREAMID_TYPE_LEN;
	
    Alligned_rHdcp2TzIfCpuMprime->rRet = MTR_NOT_OK;  




	
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalMprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_2_CALMPRIME_T)));
        TZ_NWD_BufferStart((UINT32)Alligned_rStreamType, TZ_CACHE_ALIGN(stream_type_len));
		
        i4Ret = TZ_HDCP2_ComputeMprime(Alligned_rHdcp2TzIfCpuMprime,sizeof(MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME));
		
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalMprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_2_CALMPRIME_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_rStreamType, TZ_CACHE_ALIGN(stream_type_len));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuMprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalMprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalMprime, Alligned_rCalMprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalMprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuMprime);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rStreamType);

	
    return i4Ret;
}




static MT_RESULT_T  _MTDRM_HDCP2_ComputeVprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_0_CALVPRIME_T *prCaVprime;
    MTDRM_HDCP2_0_CALVPRIME_T *Alligned_rCalVprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_0_CALVPRIME_T *Alligned_rHdcp2TzIfCpuVprime = NULL;
    UINT32 u4Len;


	unsigned char *prIdList = NULL;				    /* user space data */
	unsigned char *Alligned_rIdList = NULL;         /* kernel space date into TEE */
	int id_list_len = 0;
	




    Alligned_rCalVprime = (MTDRM_HDCP2_0_CALVPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_0_CALVPRIME_T), 32));/*kernel data*/
    
    if (!Alligned_rCalVprime)
    {
        printf("Allocate Alligned_rCalVprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuVprime = (MTDRM_TZIF_HDCP2_0_CALVPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_0_CALVPRIME_T), 32));
    if (!Alligned_rHdcp2TzIfCpuVprime)
    {
        printf("Allocate Alligned_rHdcp2TzIfCpuVprime failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCaVprime = (MTDRM_HDCP2_0_CALVPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCaVprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCaVprime, Alligned_rCalVprime, u4Len);


	/* copy id list */
	
	prIdList = prCaVprime->id_list;									   /* id list: user space point */
	id_list_len= MTDRM_HDCP2_KEYLEN_RXID * prCaVprime->DEVICE_COUNT;   /* id list: len*/


    Alligned_rIdList = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(id_list_len, 32));
    if (!Alligned_rIdList)
    {
        printf("Allocate Alligned_rStreamType failed\n");
        return MTR_NOT_OK;
    }    


	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prIdList, id_list_len);
    COPY_FROM_USER_ARG_SIZE(prIdList, Alligned_rIdList, id_list_len);

	/* debug */
	mtdrm_hdcp_DumpHex("id_list", Alligned_rIdList, id_list_len);
	printf("id_list len:%d\n", id_list_len);



    Alligned_rHdcp2TzIfCpuVprime->pV_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalVprime->au1rVprime);
    Alligned_rHdcp2TzIfCpuVprime->id_list= (VOID*)PHYSICAL((UINT32)Alligned_rIdList); 
    Alligned_rHdcp2TzIfCpuVprime->DEPTH = Alligned_rCalVprime->DEPTH;
    Alligned_rHdcp2TzIfCpuVprime->DEVICE_COUNT = Alligned_rCalVprime->DEVICE_COUNT;
    Alligned_rHdcp2TzIfCpuVprime->MAX_DEVS_EXCEEDED = Alligned_rCalVprime->MAX_DEVS_EXCEEDED;
    Alligned_rHdcp2TzIfCpuVprime->MAX_CASCADE_EXCEEDED = Alligned_rCalVprime->MAX_CASCADE_EXCEEDED;	
    Alligned_rHdcp2TzIfCpuVprime->rRet = MTR_NOT_OK;    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalVprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_0_CALVPRIME_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_rIdList, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID*Alligned_rCalVprime->DEVICE_COUNT));
        i4Ret = TZ_HDCP2_ComputeVprime(Alligned_rHdcp2TzIfCpuVprime,sizeof(MTDRM_TZIF_HDCP2_0_CALVPRIME_T));

		
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalVprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_0_CALVPRIME_T)));
        TZ_NWD_BufferEnd((UINT32)Alligned_rIdList, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID*Alligned_rCalVprime->DEVICE_COUNT));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuVprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCaVprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCaVprime, Alligned_rCalVprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalVprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuVprime);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIdList);

	
    return i4Ret;
}










static MT_RESULT_T  _MTDRM_HDCP2_2_ComputeVprime(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_2_CALVPRIME_T *prCalVprime;
    MTDRM_HDCP2_2_CALVPRIME_T *Alligned_rCalVprime = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_2_CALVPRIME_T *Alligned_rHdcp2TzIfCpuVprime = NULL;
    UINT32 u4Len;


	unsigned char *prIdList = NULL;				    /* user space data */
	unsigned char *Alligned_rIdList = NULL;         /* kernel space date into TEE */
	int id_list_len = 0;
	




    Alligned_rCalVprime = (MTDRM_HDCP2_2_CALVPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_2_CALVPRIME_T), 32));/*kernel data*/
    
    if (!Alligned_rCalVprime)
    {
        printf("Allocate Alligned_rCalVprime failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuVprime = (MTDRM_TZIF_HDCP2_2_CALVPRIME_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_2_CALVPRIME_T), 32));
    if (!Alligned_rHdcp2TzIfCpuVprime)
    {
        printf("Allocate Alligned_rHdcp2TzIfCpuVprime failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCalVprime = (MTDRM_HDCP2_2_CALVPRIME_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalVprime, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCalVprime, Alligned_rCalVprime, u4Len);



	/* copy id list */
	
	prIdList = prCalVprime->id_list;									   /* id list: user space point */
	id_list_len= MTDRM_HDCP2_KEYLEN_RXID * prCalVprime->DEVICE_COUNT;   /* id list: len*/


    Alligned_rIdList = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(id_list_len, 32));
    if (!Alligned_rIdList)
    {
        printf("Allocate Alligned_rStreamType failed\n");
        return MTR_NOT_OK;
    }    


	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prIdList, id_list_len);
    COPY_FROM_USER_ARG_SIZE(prIdList, Alligned_rIdList, id_list_len);

	/* debug */
	mtdrm_hdcp_DumpHex("id_list", Alligned_rIdList, id_list_len);
	printf("id_list len:%d\n", id_list_len);




    Alligned_rHdcp2TzIfCpuVprime->pV_prime = (VOID*)PHYSICAL((UINT32)Alligned_rCalVprime->au1rVprime);
    Alligned_rHdcp2TzIfCpuVprime->id_list= (VOID*)PHYSICAL((UINT32)Alligned_rIdList); 
    Alligned_rHdcp2TzIfCpuVprime->DEPTH = Alligned_rCalVprime->DEPTH;
    Alligned_rHdcp2TzIfCpuVprime->DEVICE_COUNT = Alligned_rCalVprime->DEVICE_COUNT;
    Alligned_rHdcp2TzIfCpuVprime->MAX_DEVS_EXCEEDED = Alligned_rCalVprime->MAX_DEVS_EXCEEDED;
    Alligned_rHdcp2TzIfCpuVprime->MAX_CASCADE_EXCEEDED = Alligned_rCalVprime->MAX_CASCADE_EXCEEDED;	
    Alligned_rHdcp2TzIfCpuVprime->HDCP2_0_REPEATER_DOWNSTREAM = Alligned_rCalVprime->HDCP2_0_REPEATER_DOWNSTREAM;	
    Alligned_rHdcp2TzIfCpuVprime->HDCP1_DEVICE_DOWNSTREAM = Alligned_rCalVprime->HDCP1_DEVICE_DOWNSTREAM;	
    Alligned_rHdcp2TzIfCpuVprime->seq_num_V = Alligned_rCalVprime->seq_num_V;	

    Alligned_rHdcp2TzIfCpuVprime->rRet = MTR_NOT_OK;    
	

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCalVprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_2_CALVPRIME_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_rIdList, TZ_CACHE_ALIGN(id_list_len));
        i4Ret = TZ_HDCP2_2_ComputeVprime(Alligned_rHdcp2TzIfCpuVprime,sizeof(MTDRM_TZIF_HDCP2_2_CALVPRIME_T));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCalVprime, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_2_CALVPRIME_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_rIdList, TZ_CACHE_ALIGN(id_list_len));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuVprime->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCalVprime, u4Len);
        COPY_TO_USER_ARG_SIZE(prCalVprime, Alligned_rCalVprime, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCalVprime);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuVprime);

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIdList);

	
    return i4Ret;
}









static MT_RESULT_T  _MTDRM_HDCP2_VerifySignature(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T	 rArg;


	unsigned char* pKpubDcp;
	unsigned char* Alligned_KpubDcp = NULL; /* kernel data */

	

	unsigned char* pCert;
	unsigned char* Alligned_Cert = NULL;   /* kernel data */





	MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE* Alligned_rHdcp2TzIfVerifySig = NULL;
	Alligned_rHdcp2TzIfVerifySig = (MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfVerifySig)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}


	Alligned_KpubDcp = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_ROOTSIGN+1, 32));/*kernel data*/
	if (!Alligned_KpubDcp)
	{
		printf("Allocate Alligned_KpubDcp failed\n");
		return MTR_NOT_OK;
	}






	Alligned_Cert = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_CERT, 32));/*kernel data*/
	if (!Alligned_Cert)
	{
		printf("Allocate Alligned_Cert failed\n");
		return MTR_NOT_OK;
	}





	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	pKpubDcp = (unsigned char*)rArg.ai4Arg[0];
	pCert = (unsigned char*)rArg.ai4Arg[1];   

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pKpubDcp, MTDRM_HDCP2_KEYLEN_ROOTSIGN+1);
	COPY_FROM_USER_ARG_SIZE(pKpubDcp, Alligned_KpubDcp, MTDRM_HDCP2_KEYLEN_ROOTSIGN+1);	


	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pCert, MTDRM_HDCP2_KEYLEN_CERT);
	COPY_FROM_USER_ARG_SIZE(pCert, Alligned_Cert, MTDRM_HDCP2_KEYLEN_CERT);	


	Alligned_rHdcp2TzIfVerifySig->pKpub = (VOID*)PHYSICAL((UINT32)Alligned_KpubDcp);
	Alligned_rHdcp2TzIfVerifySig->pCert= (VOID*)PHYSICAL((UINT32)Alligned_Cert);
	Alligned_rHdcp2TzIfVerifySig->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2

		TZ_NWD_BufferStart((UINT32)Alligned_KpubDcp, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ROOTSIGN+1));
		TZ_NWD_BufferStart((UINT32)Alligned_Cert, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_CERT));

		i4Ret = TZ_HDCP2_VerifySignature(Alligned_rHdcp2TzIfVerifySig, sizeof(MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE));

		TZ_NWD_BufferEnd((UINT32)Alligned_KpubDcp, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ROOTSIGN+1));
		TZ_NWD_BufferEnd((UINT32)Alligned_Cert, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_CERT));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfVerifySig->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif



	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfVerifySig);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_Cert);

	
	return i4Ret;

}




#if 0
/**
? This operation could get the encrypted Km 
? Input
? pOu1EncKm encrypted Km will be filled after call
? Output
? Success/fail
? Operation
? Dget the km got before
**/
static MT_RESULT_T _MTDRM_HDCP2_GenerateKMforTx(unsigned long arg)
{
	INT32 i4Ret;
	UINT8 *pu1EncKm;
	//UINT8 *pu1VirtEncKm;

	//UINT8 *prEkm = NULL;
	UINT8 *Alligned_Ekm = NULL;
	
	MTDRM_TZIF_HDCP2_ENC_KM *Alligned_rHdcp2TzEncKm = NULL;

	Alligned_rHdcp2TzEncKm = (MTDRM_TZIF_HDCP2_ENC_KM *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_ENC_KM), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzEncKm)
	{
		printf("Allocate Alligned_rHdcp2TzEncKm failed\n");
		return MTR_NOT_OK;
	}

	
	Alligned_Ekm = (unsigned char*)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_EKPUB_KM, 32));
	if(!Alligned_Ekm) 
	{
		printf("Allocate Alligned_Ekm failed\n");
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
		return MTR_NOT_OK;
	}


	/* NO NEED COPY USER EKM TO KERNEL */
	Alligned_rHdcp2TzEncKm->pEncKm = (VOID*)PHYSICAL((UINT32)Alligned_Ekm);		
	Alligned_rHdcp2TzEncKm->rRet = MTR_NOT_OK;

	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_Ekm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKPUB_KM));
		i4Ret = TZ_HDCP2_Generate_Km_for_tx(Alligned_rHdcp2TzEncKm, sizeof(MTDRM_TZIF_HDCP2_ENC_KM));
		TZ_NWD_BufferEnd((UINT32)Alligned_Ekm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKPUB_KM));
		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzEncKm->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}	 
#else
		i4Ret = MTR_NOT_OK;
#endif




	//pu1EncKm = (UINT8*)arg;
	//USR_SPACE_ACCESS_VALIDATE_ARG(pu1EncKm, char[MTDRM_HDCP2_KEYLEN_EKPUB_KM]);
	//COPY_TO_USER_ARG_SIZE(pu1EncKm, Alligned_Ekm, MTDRM_HDCP2_KEYLEN_EKPUB_KM);

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_Ekm );
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzEncKm);
	return i4Ret;
}



#endif


/**
output parameter
? pEkpub_km : encrypted Km value (128byte)

Output
? Status : success or fail

Operation
? DDI encrypted the km and store the decrypted km to the secure store
? All RSA decryption and Store data shall be in the Secure world
**/
static MT_RESULT_T  _MTDRM_HDCP2_GenerateKMforTx(unsigned long arg)
{
    INT32 i4Ret = 1;

    unsigned char *prEkpub_km = NULL;			/* usr space ekm*/
//    UINT32 u4Len;
    MTDRM_HDCP2_EKPUBKM_T* Alligned_rEkpub_km = NULL;
    MTDRM_TZIF_HDCP2_Generate_EKm* Alligned_rHdcp2TzIfGenerateEKm = NULL;

    Alligned_rEkpub_km = (MTDRM_HDCP2_EKPUBKM_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_EKPUBKM_T), 32));/*kernel data*/
    if (!Alligned_rEkpub_km)
    {
        printf("Allocate Alligned_rEkpub_km failed\n");
        return MTR_NOT_OK;
    }
	
    Alligned_rHdcp2TzIfGenerateEKm = (MTDRM_TZIF_HDCP2_Generate_EKm *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_Generate_EKm), 32));
    if (!Alligned_rHdcp2TzIfGenerateEKm)
    {
        printf("Allocate Alligned_rHdcp2TzIfGenerateEKm failed\n");
        return MTR_NOT_OK;
    }


    //prEkpub_km = (MTDRM_HDCP2_EKPUBKM_T*)rArg.ai4Arg[0];
    //u4Len = (UINT32)rArg.ai4Arg[1];

    
    Alligned_rHdcp2TzIfGenerateEKm->pEkpub_km = (VOID*)PHYSICAL((UINT32)Alligned_rEkpub_km->au1Ekpubkm);
    Alligned_rHdcp2TzIfGenerateEKm->rRet = MTR_NOT_OK;


    #ifdef CC_ENABLE_HDCP2  

        TZ_NWD_BufferStart((UINT32)Alligned_rEkpub_km, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_EKPUBKM_T)));

        i4Ret = TZ_HDCP2_Generate_Km_for_tx(Alligned_rHdcp2TzIfGenerateEKm, sizeof(MTDRM_TZIF_HDCP2_Generate_EKm));
        
        TZ_NWD_BufferEnd((UINT32)Alligned_rEkpub_km, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_EKPUBKM_T)));   
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGenerateEKm->rRet;         
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }    
    #else
        i4Ret = MTR_NOT_OK;
    #endif

	prEkpub_km = (UINT8*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prEkpub_km, sizeof(MTDRM_HDCP2_EKPUBKM_T));
    COPY_TO_USER_ARG_SIZE(prEkpub_km, Alligned_rEkpub_km, sizeof(MTDRM_HDCP2_EKPUBKM_T));


    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGenerateEKm);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rEkpub_km);    
    return (i4Ret);
}




static MT_RESULT_T  _MTDRM_HDCP2_ResetPairingInfo(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T	 rArg;

	unsigned char* pRX_ID;
	unsigned char* Alligned_RX_ID = NULL; /* kernel data */

	MTDRM_TZIF_HDCP2_CheckRxID* Alligned_rHdcp2TzIfCheckRxID = NULL;
	Alligned_rHdcp2TzIfCheckRxID = (MTDRM_TZIF_HDCP2_CheckRxID *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_CheckRxID), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfCheckRxID)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}


	Alligned_RX_ID = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_RXID, 32));/*kernel data*/
	if (!Alligned_RX_ID)
	{
		printf("Allocate Alligned_RX_ID failed\n");
		return MTR_NOT_OK;
	}


	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	pRX_ID = (unsigned char*)rArg.ai4Arg[0];   

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pRX_ID, MTDRM_HDCP2_KEYLEN_RXID);
	COPY_FROM_USER_ARG_SIZE(pRX_ID, Alligned_RX_ID, MTDRM_HDCP2_KEYLEN_RXID);	

	Alligned_rHdcp2TzIfCheckRxID->pRX_ID = (VOID*)PHYSICAL((UINT32)Alligned_RX_ID);
	Alligned_rHdcp2TzIfCheckRxID->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2

		TZ_NWD_BufferStart((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));
		i4Ret = TZ_HDCP2_ResetPairingInfo(Alligned_rHdcp2TzIfCheckRxID, sizeof(MTDRM_TZIF_HDCP2_CheckRxID));  

		TZ_NWD_BufferEnd((UINT32)Alligned_RX_ID, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RXID));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfCheckRxID->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif



	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCheckRxID);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_RX_ID);

	
	return i4Ret;

}




MT_RESULT_T  _MTDRM_HDCP2_UseTestKey_InKernel(unsigned long key_type)
{
	INT32 i4Ret;
	int r_key_type;


	MTDRM_TZIF_HDCP2_Set_Key_Type* Alligned_rHdcp2TzIfSetKeyType = NULL;
	Alligned_rHdcp2TzIfSetKeyType = (MTDRM_TZIF_HDCP2_Set_Key_Type *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_Set_Key_Type), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfSetKeyType)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}

    if (copy_from_user(&r_key_type, (void __user *)key_type, sizeof(int)))    
    {                                                                          
        printk("%s copy_from_user error\n", __FUNCTION__);                     
        return MTR_ERR_INV;                                                    
    }


	Alligned_rHdcp2TzIfSetKeyType->key_type = r_key_type;
	Alligned_rHdcp2TzIfSetKeyType->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2

		i4Ret = TZ_HDCP2_UseTestKey_InKernel(Alligned_rHdcp2TzIfSetKeyType, sizeof(MTDRM_TZIF_HDCP2_Set_Key_Type));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfSetKeyType->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif



	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfSetKeyType);

	
	return i4Ret;

}





/* for hdcp2.2 extra */

MT_RESULT_T  _MTDRM_HDCP2_SetTxCap(unsigned long txcap)
{
	INT32 i4Ret = 0;
	int r_tx_cap;

	printk(">>> %s \n", __FUNCTION__); 


    if (copy_from_user(&r_tx_cap, (void __user *)txcap, sizeof(int)))    
    {                                                                          
        printk("%s copy_from_user error\n", __FUNCTION__);                     
        return MTR_ERR_INV;                                                    
    }
	
	printk("<< %s tx cap (%d)\n", __FUNCTION__, r_tx_cap); 

	// wait for erik to implement the dmx api.
	//i4Ret = DMX_HDCP_EnableDistCtrCheck(r_tx_cap);

	printk("<< %s ret(%d)\n", __FUNCTION__, i4Ret); 

	
	return i4Ret;

}

/* for android */

static MT_RESULT_T  _MTDRM_HDCP2_SW_DataDecrypt(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_DataDecrypt_T *prDatadecrypt;
    MTDRM_HDCP2_DataDecrypt_T *Alligned_rDatadecrypt = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_DataDecrypt_T *Alligned_rHdcp2TzIfDatadecrypt = NULL;
    UINT32 u4Len;


	unsigned char *prInbuf = NULL;				   /* user space data */
	unsigned char *Alligned_rInbuf = NULL;         /* kernel space date into TEE */

	unsigned char *prOutbuf = NULL;				    /* user space data */
	unsigned char *Alligned_rOutbuf = NULL;         /* kernel space date into TEE */

	unsigned char *prIV = NULL; 				    /* user space data */
	unsigned char *Alligned_rIV = NULL; 		    /* kernel space date into TEE */


	
	int data_len = 0;
	

	printf(">>:%s\n", __FUNCTION__);

    Alligned_rDatadecrypt = (MTDRM_HDCP2_DataDecrypt_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_DataDecrypt_T), 32));/*kernel data*/
    
    if (!Alligned_rDatadecrypt)
    {
        printf("Allocate Alligned_rDatadecrypt failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfDatadecrypt = (MTDRM_TZIF_HDCP2_DataDecrypt_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_DataDecrypt_T), 32));
    if (!Alligned_rHdcp2TzIfDatadecrypt)
    {
        printf("Allocate Alligned_rHdcp2TzIfDatadecrypt failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);


    prDatadecrypt = (MTDRM_HDCP2_DataDecrypt_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prDatadecrypt, u4Len);
    COPY_FROM_USER_ARG_SIZE(prDatadecrypt, Alligned_rDatadecrypt, u4Len);
	printf("COPY_FROM_USER_ARG_SIZE prDatadecrypt ok\n");


	/* copy id list */
	
	prInbuf  = prDatadecrypt->in_buf;									   /* user space point */
	prOutbuf = prDatadecrypt->out_buf;									   /* user space point */	
	prIV     = prDatadecrypt->iv;
	data_len = prDatadecrypt->data_len;       		



	printf("in_buf:%p\n",   prDatadecrypt->in_buf);
	printf("out_buf:%p\n",  prDatadecrypt->out_buf);
	printf("iv:%p\n",       prDatadecrypt->iv);
	printf("data_len:%d\n", prDatadecrypt->data_len);







    Alligned_rInbuf = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(data_len, 32));
    if (!Alligned_rInbuf)
    {
        printf("Allocate Alligned_rInbuf failed\n");
        return MTR_NOT_OK;
    }    


    Alligned_rOutbuf= (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(data_len, 32));
    if (!Alligned_rOutbuf)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rInbuf);
        printf("Allocate Alligned_rOutbuf failed\n");
        return MTR_NOT_OK;
    }    
	

    Alligned_rIV= (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTDRM_HDCP2_KEYLEN_LC128, 32));
    if (!Alligned_rIV)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rInbuf);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rOutbuf);
        printf("Allocate Alligned_rIV failed\n");
        return MTR_NOT_OK;
    }    
	



	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prInbuf, data_len);
    COPY_FROM_USER_ARG_SIZE(prInbuf, Alligned_rInbuf, data_len);
	printf("COPY_FROM_USER_ARG_SIZE prInbuf ok\n");

	/* no need to copy outbuf into kernel space */

	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prIV, MTDRM_HDCP2_KEYLEN_LC128);
    COPY_FROM_USER_ARG_SIZE(prIV, Alligned_rIV, MTDRM_HDCP2_KEYLEN_LC128);
	printf("COPY_FROM_USER_ARG_SIZE Alligned_rIV ok\n");




	/* debug */
	mtdrm_hdcp_DumpHex("in_data", Alligned_rInbuf, data_len);
	printf("data:%d\n", data_len);


	mtdrm_hdcp_DumpHex("iv:", Alligned_rIV, MTDRM_HDCP2_KEYLEN_LC128);



    Alligned_rHdcp2TzIfDatadecrypt->in_buf  =  (VOID*)PHYSICAL((UINT32)Alligned_rInbuf);
    Alligned_rHdcp2TzIfDatadecrypt->out_buf =  (VOID*)PHYSICAL((UINT32)Alligned_rOutbuf); 
    Alligned_rHdcp2TzIfDatadecrypt->iv      =  (VOID*)PHYSICAL((UINT32)Alligned_rIV); 	
    Alligned_rHdcp2TzIfDatadecrypt->data_len =  Alligned_rDatadecrypt->data_len;

    Alligned_rHdcp2TzIfDatadecrypt->rRet = MTR_NOT_OK;    
	

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rDatadecrypt, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_DataDecrypt_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_rInbuf, TZ_CACHE_ALIGN(data_len));
		TZ_NWD_BufferStart((UINT32)Alligned_rOutbuf, TZ_CACHE_ALIGN(data_len));
		TZ_NWD_BufferStart((UINT32)Alligned_rIV, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));

		
        i4Ret = TZ_HDCP2_SW_Decrypt(Alligned_rHdcp2TzIfDatadecrypt,sizeof(MTDRM_TZIF_HDCP2_DataDecrypt_T));
        TZ_NWD_BufferEnd((UINT32)Alligned_rDatadecrypt, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_DataDecrypt_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_rInbuf, TZ_CACHE_ALIGN(data_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_rOutbuf, TZ_CACHE_ALIGN(data_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_rIV, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfDatadecrypt->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }

		/*get output data*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prOutbuf, data_len);
        COPY_TO_USER_ARG_SIZE(prOutbuf, Alligned_rOutbuf, data_len);
		printf("COPY_FROM_USER_ARG_SIZE Alligned_rOutbuf ok\n");

		mtdrm_hdcp_DumpHex("out_data", Alligned_rOutbuf, data_len);


		
    #else
        i4Ret = MTR_NOT_OK;
    #endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rDatadecrypt);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfDatadecrypt);

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rInbuf);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rOutbuf);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIV);

	

	
    return i4Ret;
}

MT_RESULT_T  _MTDRM_HDCP2_UseTestKey_InKernel_for_sif(unsigned long key_type)
{
	INT32 i4Ret;
	int r_key_type = key_type;


	MTDRM_TZIF_HDCP2_Set_Key_Type* Alligned_rHdcp2TzIfSetKeyType = NULL;
	Alligned_rHdcp2TzIfSetKeyType = (MTDRM_TZIF_HDCP2_Set_Key_Type *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_Set_Key_Type), 32));/*kernel data*/
	if (!Alligned_rHdcp2TzIfSetKeyType)
	{
		printf("Allocate Alligned_rHdcp2TzIfDecEkm failed\n");
		return MTR_NOT_OK;
	}

                                                                          
    printk("%s set key type:%d\n", __FUNCTION__, r_key_type);                     



	Alligned_rHdcp2TzIfSetKeyType->key_type = r_key_type;
	Alligned_rHdcp2TzIfSetKeyType->rRet = MTR_NOT_OK;
	
#ifdef CC_ENABLE_HDCP2

		i4Ret = TZ_HDCP2_UseTestKey_InKernel(Alligned_rHdcp2TzIfSetKeyType, sizeof(MTDRM_TZIF_HDCP2_Set_Key_Type));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rHdcp2TzIfSetKeyType->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif



	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfSetKeyType);

	
	return i4Ret;

}



MT_RESULT_T  _MTDRM_HDCP2_IF_TZ_KEY(unsigned long flag)
{

	int *rflag = (int *)flag;
	int key_tz_enable = hdcp_key_tz_enable;
	printf(">> %s\n", __FUNCTION__);

	if (NULL == rflag)
	{
		printf("invalid arg\n");
		return MTR_NOT_OK;
	}

    USR_SPACE_ACCESS_VALIDATE_ARG(rflag, int);
                                                             
    printk("%s tz_key_enable:%d\n", __FUNCTION__, key_tz_enable);   	

	/*get output data*/
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(rflag, sizeof(int));
	COPY_TO_USER_ARG_SIZE(rflag, &key_tz_enable, sizeof(int));
	printf("COPY_FROM_USER_ARG_SIZE rflag ok\n");

	
	return 0;
}





MT_RESULT_T  _MTDRM_HDCP2_enable_tz_key(unsigned long tz_key_enable)
{
	int ret = -1;
    hdcp_key_tz_enable = tz_key_enable;                                                                      
    printk("%s tz_key_enable:%d\n", __FUNCTION__, hdcp_key_tz_enable);    

	if(1 == hdcp_key_tz_enable)
	{
		ret = _MTDRM_HDCP2_Set_MTK_EncKeySet_into_TZ(_au1HDCP2x_2_production_key, sizeof(_au1HDCP2x_2_production_key));
	}
	
	return ret;
}




/* sync from lgwebos */
static MT_RESULT_T  _MTDRM_HDCP2_HMAC_SHA256(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_CAL_HMAC_SHA256_T *prCal_HMAC_SHA256;
    MTDRM_HDCP2_CAL_HMAC_SHA256_T *Alligned_rCal_HMAC_SHA256 = NULL;/*kernel data*/
    MTDRM_TZIF_HDCP2_HMAC_SHA256 *Alligned_rHdcp2TzIfCpuHmacSha256 = NULL;
    UINT32 u4Len;



	
	unsigned char *prInput;
	unsigned char *Alligned_input;
	unsigned int input_len;

	unsigned char *prKey;
	unsigned char *Alligned_key;
	unsigned int key_len;




	

    Alligned_rCal_HMAC_SHA256 = (MTDRM_HDCP2_CAL_HMAC_SHA256_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_CAL_HMAC_SHA256_T), 32));/*kernel data*/
    if (!Alligned_rCal_HMAC_SHA256)
    {
        printf("Allocate Alligned_rCal_HMAC_SHA256 failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rHdcp2TzIfCpuHmacSha256 = (MTDRM_TZIF_HDCP2_HMAC_SHA256 *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_HMAC_SHA256), 32));
    if (!Alligned_rHdcp2TzIfCpuHmacSha256)
    {
        printf("Allocate Alligned_rHdcp2TzIfCpuHmacSha256 failed\n");
        return MTR_NOT_OK;
    }    

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prCal_HMAC_SHA256 = (MTDRM_HDCP2_CAL_HMAC_SHA256_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCal_HMAC_SHA256, u4Len);
    COPY_FROM_USER_ARG_SIZE(prCal_HMAC_SHA256, Alligned_rCal_HMAC_SHA256, u4Len);



	/* copy user space input to kernel */
	input_len = prCal_HMAC_SHA256->intputLen;
	prInput = prCal_HMAC_SHA256->input;
	Alligned_input = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(input_len, 32));/*kernel data*/
    if (!Alligned_input)
    {
        printf("Allocate Alligned_input failed\n");
        return MTR_NOT_OK;
    }
	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prInput, input_len);
    COPY_FROM_USER_ARG_SIZE(prInput, Alligned_input, input_len);



	/* copy user space key to kernel */
	key_len = prCal_HMAC_SHA256->au1rKeyLen;
	prKey= prCal_HMAC_SHA256->au1rKey;
	Alligned_key= (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_len, 32));/*kernel data*/
    if (!Alligned_key)
    {
        printf("Allocate Alligned_key failed\n");
        return MTR_NOT_OK;
    }
	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKey, key_len);
    COPY_FROM_USER_ARG_SIZE(prKey, Alligned_key, key_len);




    Alligned_rHdcp2TzIfCpuHmacSha256->output= (VOID*)PHYSICAL((UINT32)Alligned_rCal_HMAC_SHA256->output);
    Alligned_rHdcp2TzIfCpuHmacSha256->au1rKey= (VOID*)PHYSICAL((UINT32)Alligned_key);
    Alligned_rHdcp2TzIfCpuHmacSha256->au1rKeyLen= key_len;
    Alligned_rHdcp2TzIfCpuHmacSha256->input = (VOID*)PHYSICAL((UINT32)Alligned_input);
	Alligned_rHdcp2TzIfCpuHmacSha256->intputLen = input_len;

    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rCal_HMAC_SHA256, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CAL_HMAC_SHA256_T)));
        i4Ret = TZ_HDCP2_HMAC_SHA256(Alligned_rHdcp2TzIfCpuHmacSha256, sizeof(MTDRM_TZIF_HDCP2_HMAC_SHA256));
        TZ_NWD_BufferEnd((UINT32)Alligned_rCal_HMAC_SHA256, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_CAL_HMAC_SHA256_T)));

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfCpuHmacSha256->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
        
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prCal_HMAC_SHA256, u4Len);
        COPY_TO_USER_ARG_SIZE(prCal_HMAC_SHA256, Alligned_rCal_HMAC_SHA256, u4Len);
    #else
        i4Ret = MTR_NOT_OK;
    #endif    
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rCal_HMAC_SHA256);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfCpuHmacSha256);
    return i4Ret;
}


static MT_RESULT_T  _MTDRM_HDCP2_GET_PDKey(unsigned long arg)
{
    INT32 i4Ret = 0;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_ProtectedDecryptionKey *prHdcp2PDKey;		/* point to user space value*/
    UINT32 u4Len = 0;   
    
    MTDRM_HDCP2_ProtectedDecryptionKey *Alligned_rHdcp2PDKey = NULL; /*will copy the use space value to it*/
	
    MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY *Alligned_rHdcp2TzIfGetProtectedDecryptKey = NULL;	/* IF TO TEE*/

	/* alloc kernel data */	
    Alligned_rHdcp2PDKey = (MTDRM_HDCP2_ProtectedDecryptionKey *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey), 32));/*kernel data*/
    if (!Alligned_rHdcp2PDKey)
    {
        printf("Allocate Alligned_rHdcp2PDKey failed\n");
        return MTR_NOT_OK;
    } 

	/* alloc if */
    Alligned_rHdcp2TzIfGetProtectedDecryptKey = (MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY), 32));
    if (!Alligned_rHdcp2TzIfGetProtectedDecryptKey)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2PDKey);
        printf("Allocate Alligned_rHdcp2TzIfGetProtectedDecryptKey failed\n");
        return MTR_NOT_OK;
    } 

	/* get the user space data  */
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2PDKey = (MTDRM_HDCP2_ProtectedDecryptionKey*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    // read built-in edid data from driver
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->pData = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2PDKey->protected_decryption_key);
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->u4length = 0;
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->rRet = MTR_NOT_OK;

    #ifdef CC_ENABLE_HDCP2    
    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2PDKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey)));
    i4Ret = TZ_HDCP2_GetPDKey(Alligned_rHdcp2TzIfGetProtectedDecryptKey, sizeof(MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY));
    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2PDKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey)));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGetProtectedDecryptKey->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif

	
	Alligned_rHdcp2PDKey->len = Alligned_rHdcp2TzIfGetProtectedDecryptKey->u4length;
	printf("%s: size:\n",  __FUNCTION__, Alligned_rHdcp2PDKey->len);

	if(Alligned_rHdcp2PDKey->len > PROTECTED_KEY_MAX)
	{
		printf("error, key too long\n");
	}
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));
    COPY_TO_USER_ARG_SIZE(prHdcp2PDKey, Alligned_rHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));
        
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2PDKey);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGetProtectedDecryptKey);

    return i4Ret;

}

static MT_RESULT_T  _MTDRM_HDCP2_SET_PDKey(unsigned long arg)
{
    INT32 i4Ret = 0;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_HDCP2_ProtectedDecryptionKey *prHdcp2PDKey;		/* point to user space value*/
    UINT32 u4Len = 0;   
    
    MTDRM_HDCP2_ProtectedDecryptionKey *Alligned_rHdcp2PDKey = NULL; /*will copy the use space value to it*/
	
    MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY *Alligned_rHdcp2TzIfGetProtectedDecryptKey = NULL;	/* IF TO TEE*/

	/* alloc kernel data */	
    Alligned_rHdcp2PDKey = (MTDRM_HDCP2_ProtectedDecryptionKey *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey), 32));/*kernel data*/
    if (!Alligned_rHdcp2PDKey)
    {
        printf("Allocate Alligned_rHdcp2PDKey failed\n");
        return MTR_NOT_OK;
    } 

	/* alloc if */
    Alligned_rHdcp2TzIfGetProtectedDecryptKey = (MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY), 32));
    if (!Alligned_rHdcp2TzIfGetProtectedDecryptKey)
    {
    	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2PDKey);
        printf("Allocate Alligned_rHdcp2TzIfGetProtectedDecryptKey failed\n");
        return MTR_NOT_OK;
    } 

	/* get the user space data  */
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prHdcp2PDKey = (MTDRM_HDCP2_ProtectedDecryptionKey*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];


    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));
    COPY_FROM_USER_ARG_SIZE(prHdcp2PDKey, Alligned_rHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));


	if(Alligned_rHdcp2PDKey->len > PROTECTED_KEY_MAX)
	{
		printf("WARNING, key too long\n");
		Alligned_rHdcp2PDKey->len = PROTECTED_KEY_MAX;
	}
	

	
	mtdrm_hdcp_DumpHex("set pdkey:", Alligned_rHdcp2PDKey->protected_decryption_key, Alligned_rHdcp2PDKey->len);


    // read built-in edid data from driver
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->pData = (VOID*)PHYSICAL((UINT32)Alligned_rHdcp2PDKey->protected_decryption_key);
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->u4length = Alligned_rHdcp2PDKey->len;
    Alligned_rHdcp2TzIfGetProtectedDecryptKey->rRet = MTR_NOT_OK;

	printf("%s: size:\n",  __FUNCTION__, Alligned_rHdcp2PDKey->len);



    #ifdef CC_ENABLE_HDCP2    
    TZ_NWD_BufferStart((UINT32)Alligned_rHdcp2PDKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey)));
    i4Ret = TZ_HDCP2_SetPDKey(Alligned_rHdcp2TzIfGetProtectedDecryptKey, sizeof(MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY));
    TZ_NWD_BufferEnd((UINT32)Alligned_rHdcp2PDKey, TZ_CACHE_ALIGN(sizeof(MTDRM_HDCP2_ProtectedDecryptionKey)));
        if (i4Ret != 0)
        {
            i4Ret = Alligned_rHdcp2TzIfGetProtectedDecryptKey->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }
    #else
        i4Ret = MTR_NOT_OK;
    #endif


    //USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));
    //COPY_TO_USER_ARG_SIZE(prHdcp2PDKey, Alligned_rHdcp2PDKey, sizeof(MTDRM_HDCP2_ProtectedDecryptionKey));
        
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2PDKey);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rHdcp2TzIfGetProtectedDecryptKey);

    return i4Ret;

}


/* sony iptves */


/* sony iptves */
static MT_RESULT_T  _MTDRM_IPTVES_SAC_CONSTRUCT(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_IPTV_sac_Construct_T *prSAC_Construct;
    MTDRM_IPTV_sac_Construct_T *Alligned_rSAC_Construct = NULL;/*kernel data*/


	/* user space req list information */
	int req_list_num = 0;
	int req_list_size = 0;
	char *req_list = NULL;

	/* kernel space req list */
	char *Alligned_req_list = NULL;

	
    MTDRM_TZIF_IPTVES_SAC_CONSTRUCT *Alligned_rIPTVES_SAC_If_Construct = NULL;
    UINT32 u4Len;

    Alligned_rSAC_Construct = (MTDRM_IPTV_sac_Construct_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sac_Construct_T), 32));/*kernel data*/
    
    if (!Alligned_rSAC_Construct)
    {
        printf("Allocate Alligned_rSAC_Construct failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rIPTVES_SAC_If_Construct = (MTDRM_TZIF_IPTVES_SAC_CONSTRUCT *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_SAC_CONSTRUCT), 32));
    if (!Alligned_rIPTVES_SAC_If_Construct)
    {
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_Construct);
	
        printf("Allocate Alligned_rIPTVES_SAC_If_Construct failed\n");
        return MTR_NOT_OK;
    }    
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prSAC_Construct = (MTDRM_IPTV_sac_Construct_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_Construct, u4Len);
    COPY_FROM_USER_ARG_SIZE(prSAC_Construct, Alligned_rSAC_Construct, u4Len);


	/* copy the req_list from usr space to kernel */
	req_list_num = prSAC_Construct->reg_num;
	req_list_size = prSAC_Construct->req_list_size;
	req_list = prSAC_Construct->req_list;

	Alligned_req_list = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(req_list_size, 32));
    if (NULL == Alligned_req_list)
    {
        printf("Allocate Alligned_req_list failed\n");
		
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_Construct);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_Construct);
		
        return MTR_NOT_OK;
    } 


    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(req_list, req_list_size);
    COPY_FROM_USER_ARG_SIZE(req_list, Alligned_req_list, req_list_size);




	/*init tz if for sac */
	Alligned_rIPTVES_SAC_If_Construct->id = prSAC_Construct->in_thid;
	Alligned_rIPTVES_SAC_If_Construct->req_num = prSAC_Construct->reg_num;
	Alligned_rIPTVES_SAC_If_Construct->req_lst = (VOID*)PHYSICAL((UINT32)Alligned_req_list);
	Alligned_rIPTVES_SAC_If_Construct->req_list_size = req_list_size;

    Alligned_rIPTVES_SAC_If_Construct->rRet = MTR_NOT_OK;    
    #ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rSAC_Construct, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_Construct_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_req_list, TZ_CACHE_ALIGN(req_list_size));

	
        i4Ret = TZ_IPTVES_SAC_CONSTRUCT(Alligned_rIPTVES_SAC_If_Construct,sizeof(MTDRM_TZIF_IPTVES_SAC_CONSTRUCT));

		
        TZ_NWD_BufferEnd((UINT32)Alligned_rSAC_Construct, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_Construct_T)));
        TZ_NWD_BufferEnd((UINT32)Alligned_req_list, TZ_CACHE_ALIGN(req_list_size));
		

        if (i4Ret != 0)
        {
            i4Ret = Alligned_rIPTVES_SAC_If_Construct->rRet;
        }
        else
        {
            i4Ret = MTR_NOT_OK;
        }


		//ret value
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_Construct->out_handle, sizeof(prSAC_Construct->out_handle));
        COPY_TO_USER_ARG_SIZE(prSAC_Construct->out_handle, &(Alligned_rIPTVES_SAC_If_Construct->out_handle), sizeof(prSAC_Construct->out_handle));


		
    #else
        i4Ret = MTR_NOT_OK;
    #endif
	
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_Construct);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_Construct);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_req_list);



	
    return i4Ret;
}




static MT_RESULT_T  _MTDRM_IPTVES_SAC_DESTRUCT(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sac_Destruct_T *prSAC_Destruct;
	MTDRM_IPTV_sac_Destruct_T *Alligned_rSAC_Destruct = NULL;/*kernel data*/

	
	MTDRM_TZIF_IPTVES_SAC_DESTRUCT *Alligned_rIPTVES_SAC_If_Destruct = NULL;
	UINT32 u4Len;

	Alligned_rSAC_Destruct = (MTDRM_IPTV_sac_Destruct_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sac_Destruct_T), 32));/*kernel data*/
	
	if (!Alligned_rSAC_Destruct)
	{
		printf("Allocate Alligned_rSAC_Destruct failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_SAC_If_Destruct = (MTDRM_TZIF_IPTVES_SAC_DESTRUCT *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_SAC_DESTRUCT), 32));
	if (!Alligned_rIPTVES_SAC_If_Destruct)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_Destruct);
	
		printf("Allocate Alligned_rSAC_Destruct failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prSAC_Destruct = (MTDRM_IPTV_sac_Destruct_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_Destruct, u4Len);
	COPY_FROM_USER_ARG_SIZE(prSAC_Destruct, Alligned_rSAC_Destruct, u4Len);




	/*init tz if for sac */
	Alligned_rIPTVES_SAC_If_Destruct->in_handle = (int)prSAC_Destruct->handle;
	//Alligned_rIPTVES_SAC_If_Destruct->out_success_num;
	//Alligned_rIPTVES_SAC_If_Destruct->out_trans_flag_status;

	Alligned_rIPTVES_SAC_If_Destruct->rRet = MTR_NOT_OK;	 
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rSAC_Destruct, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_Destruct_T)));
	
		i4Ret = TZ_IPTVES_SAC_DESTRUCT(Alligned_rIPTVES_SAC_If_Destruct,sizeof(MTDRM_TZIF_IPTVES_SAC_DESTRUCT));
	
		TZ_NWD_BufferEnd((UINT32)Alligned_rSAC_Destruct, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_Destruct_T)));
		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_SAC_If_Destruct->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
		

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_Destruct->out_success_num, sizeof(prSAC_Destruct->out_success_num));
		COPY_TO_USER_ARG_SIZE(prSAC_Destruct->out_success_num, &(Alligned_rIPTVES_SAC_If_Destruct->out_success_num), sizeof(prSAC_Destruct->out_success_num));


		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_Destruct->out_trans_flags_status, sizeof(prSAC_Destruct->out_trans_flags_status));
		COPY_TO_USER_ARG_SIZE(prSAC_Destruct->out_trans_flags_status, &(Alligned_rIPTVES_SAC_If_Destruct->out_trans_flag_status), sizeof(prSAC_Destruct->out_trans_flags_status));



		
#else
		i4Ret = MTR_NOT_OK;
#endif
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_Destruct);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_Destruct);

	
	return i4Ret;
}



	
	












static MT_RESULT_T	_MTDRM_IPTVES_SAC_MAKEMSG(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sac_MakeMessage_T *prSAC_MKMsg;
	MTDRM_IPTV_sac_MakeMessage_T *Alligned_rSAC_MKMsg = NULL;/*kernel data*/


	/* user space msg information */
	int out_msg_len = 0;
	char *out_msg = NULL;

	/* kernel space msg */
	char *Alligned_out_msg = NULL;

	
	MTDRM_TZIF_IPTVES_SAC_MakeMSG *Alligned_rIPTVES_SAC_If_MkMsg = NULL;
	UINT32 u4Len;

	Alligned_rSAC_MKMsg = (MTDRM_IPTV_sac_MakeMessage_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sac_MakeMessage_T), 32));/*kernel data*/
	
	if (!Alligned_rSAC_MKMsg)
	{
		printf("Allocate Alligned_rSAC_MKMsg failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_SAC_If_MkMsg = (MTDRM_TZIF_IPTVES_SAC_MakeMSG *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_SAC_MakeMSG), 32));
	if (!Alligned_rIPTVES_SAC_If_MkMsg)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_MKMsg);
	
		printf("Allocate Alligned_rIPTVES_SAC_If_MkMsg failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prSAC_MKMsg = (MTDRM_IPTV_sac_MakeMessage_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_MKMsg, u4Len);
	COPY_FROM_USER_ARG_SIZE(prSAC_MKMsg, Alligned_rSAC_MKMsg, u4Len);


	/* malloc msg buffer */
	out_msg = prSAC_MKMsg->out_msg;
	out_msg_len = *(prSAC_MKMsg->io_msg_len);
	Alligned_out_msg = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_msg_len, 32));

	if (NULL == Alligned_out_msg)
	{
		printf("Allocate Alligned_out_msg failed\n");
		
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_MKMsg);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_MkMsg);
		
		return MTR_NOT_OK;
	} 


	// no need to copy user space msg  into driver
	// just check user space msg 
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_MKMsg->out_msg, out_msg_len);
	//COPY_FROM_USER_ARG_SIZE(req_list, Alligned_req_list, req_list_size);




	/*init tz if for sac */
	Alligned_rIPTVES_SAC_If_MkMsg->in_handle = (int)prSAC_MKMsg->handle;
	Alligned_rIPTVES_SAC_If_MkMsg->io_msg_len = out_msg_len;
	Alligned_rIPTVES_SAC_If_MkMsg->out_msg = (VOID*)PHYSICAL((UINT32)Alligned_out_msg);

	//Alligned_rIPTVES_SAC_If_MkMsg->out_last_req_flag


	Alligned_rIPTVES_SAC_If_MkMsg->rRet = MTR_NOT_OK;	 
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rSAC_MKMsg, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_MakeMessage_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_out_msg, TZ_CACHE_ALIGN(out_msg_len));

	
		i4Ret = TZ_IPTVES_SAC_MkMsg(Alligned_rIPTVES_SAC_If_MkMsg,sizeof(MTDRM_TZIF_IPTVES_SAC_MakeMSG));

		
		TZ_NWD_BufferEnd((UINT32)Alligned_rSAC_MKMsg, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_MakeMessage_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_msg, TZ_CACHE_ALIGN(out_msg_len));
		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_SAC_If_MkMsg->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		//ret value
		/* out msg */

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_msg, Alligned_rIPTVES_SAC_If_MkMsg->io_msg_len);
		COPY_TO_USER_ARG_SIZE(out_msg, Alligned_out_msg, Alligned_rIPTVES_SAC_If_MkMsg->io_msg_len);


		/* io msg len */
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_MKMsg->io_msg_len, sizeof(prSAC_MKMsg->io_msg_len));
		COPY_TO_USER_ARG_SIZE(prSAC_MKMsg->io_msg_len, &(Alligned_rIPTVES_SAC_If_MkMsg->io_msg_len), sizeof(prSAC_MKMsg->io_msg_len));



		/* out last req flag*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_MKMsg->out_last_req_flag, sizeof(prSAC_MKMsg->out_last_req_flag));
		COPY_TO_USER_ARG_SIZE(prSAC_MKMsg->out_last_req_flag, &(Alligned_rIPTVES_SAC_If_MkMsg->out_last_req_flag), sizeof(prSAC_MKMsg->out_last_req_flag));



		
#else
		i4Ret = MTR_NOT_OK;
#endif
	
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_MKMsg);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_MkMsg);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_msg);



	
	return i4Ret;
}








/* sony iptves */
static MT_RESULT_T  _MTDRM_IPTVES_SAC_PROCESSMSG(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sac_ProcessMessage_T *prSAC_ProcessMsg;
	MTDRM_IPTV_sac_ProcessMessage_T *Alligned_rSAC_ProcessMsg = NULL;/*kernel data*/


	/* user space msg information */
	int in_msg_len = 0;
	char *in_msg = NULL;

	/* kernel space msg */
	char *Alligned_in_msg = NULL;

	
	MTDRM_TZIF_IPTVES_SAC_ProcessMSG *Alligned_rIPTVES_SAC_If_ProceeMsg = NULL;
	UINT32 u4Len;

	Alligned_rSAC_ProcessMsg = (MTDRM_IPTV_sac_ProcessMessage_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sac_ProcessMessage_T), 32));/*kernel data*/
	
	if (!Alligned_rSAC_ProcessMsg)
	{
		printf("Allocate Alligned_rSAC_ProcessMsg failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_SAC_If_ProceeMsg = (MTDRM_TZIF_IPTVES_SAC_ProcessMSG *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_SAC_ProcessMSG), 32));
	if (!Alligned_rIPTVES_SAC_If_ProceeMsg)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_ProcessMsg);
	
		printf("Allocate Alligned_rIPTVES_SAC_If_ProceeMsg failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prSAC_ProcessMsg = (MTDRM_IPTV_sac_ProcessMessage_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_ProcessMsg, u4Len);
	COPY_FROM_USER_ARG_SIZE(prSAC_ProcessMsg, Alligned_rSAC_ProcessMsg, u4Len);


	/* malloc msg buffer */
	in_msg = prSAC_ProcessMsg->in_msg;
	in_msg_len = prSAC_ProcessMsg->in_msg_len;
	Alligned_in_msg = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_msg_len, 32));

	if (NULL == Alligned_in_msg)
	{
		printf("Allocate Alligned_in_msg failed\n");
		
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_ProcessMsg);
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_ProceeMsg);
		
		return MTR_NOT_OK;
	} 


	// copy user space msg  into driver
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_ProcessMsg->in_msg, in_msg_len);
	COPY_FROM_USER_ARG_SIZE(prSAC_ProcessMsg->in_msg, Alligned_in_msg, in_msg_len);

	/*init tz if for sac */
	Alligned_rIPTVES_SAC_If_ProceeMsg->in_handle = (int)prSAC_ProcessMsg->handle;
	Alligned_rIPTVES_SAC_If_ProceeMsg->in_msg_len = in_msg_len;
	Alligned_rIPTVES_SAC_If_ProceeMsg->in_msg = (VOID*)PHYSICAL((UINT32)Alligned_in_msg);

	//Alligned_rIPTVES_SAC_If_ProceeMsg->out_end_flag


	Alligned_rIPTVES_SAC_If_ProceeMsg->rRet = MTR_NOT_OK;	 
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rSAC_ProcessMsg, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_ProcessMessage_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_in_msg, TZ_CACHE_ALIGN(in_msg_len));

	
		i4Ret = TZ_IPTVES_SAC_ProcessMsg(Alligned_rIPTVES_SAC_If_ProceeMsg,sizeof(MTDRM_TZIF_IPTVES_SAC_ProcessMSG));

		
		TZ_NWD_BufferEnd((UINT32)Alligned_rSAC_ProcessMsg, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sac_ProcessMessage_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_msg, TZ_CACHE_ALIGN(in_msg_len));
		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_SAC_If_ProceeMsg->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		//ret value
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prSAC_ProcessMsg->out_end_flag, sizeof(prSAC_ProcessMsg->out_end_flag));
		COPY_TO_USER_ARG_SIZE(prSAC_ProcessMsg->out_end_flag, &(Alligned_rIPTVES_SAC_If_ProceeMsg->out_end_flag), sizeof(prSAC_ProcessMsg->out_end_flag));


		
#else
		i4Ret = MTR_NOT_OK;
#endif
	
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rSAC_ProcessMsg);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_SAC_If_ProceeMsg);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_msg);



	
	return i4Ret;
}



static MT_RESULT_T	_MTDRM_IPTVES_SDI_INIT(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sdi_Init_T *prsdi_Init;
	MTDRM_IPTV_sdi_Init_T *Alligned_rsdi_Init = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_sdi_Init_T *Alligned_rIPTVES_TZIf_sdi_Init = NULL;
	UINT32 u4Len;


	//printf(">> %s", __FUNCTION__);

	Alligned_rsdi_Init = (MTDRM_IPTV_sdi_Init_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_Init_T), 32));/*kernel data*/
	
	if (!Alligned_rsdi_Init)
	{
		printf("Allocate Alligned_rsdi_Init failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_sdi_Init = (MTDRM_TZIF_IPTVES_sdi_Init_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_Init_T), 32));
	if (!Alligned_rIPTVES_TZIf_sdi_Init)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_sdi_Init failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prsdi_Init = (MTDRM_IPTV_sdi_Init_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_Init, u4Len);
	COPY_FROM_USER_ARG_SIZE(prsdi_Init, Alligned_rsdi_Init, u4Len);

	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_sdi_Init->current_time_secs = Alligned_rsdi_Init->current_time_secs;
	Alligned_rIPTVES_TZIf_sdi_Init->gmtoff = Alligned_rsdi_Init->gmtoff;

	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_sdi_Init->rRet = MTR_NOT_OK;	

	printf("\n iptves %s 1401 init\n", __FUNCTION__);

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rsdi_Init, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_Init_T)));
		i4Ret = TZ_IPTVES_SDI_INIT(Alligned_rIPTVES_TZIf_sdi_Init,sizeof(MTDRM_TZIF_IPTVES_sdi_Init_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_Init, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_Init_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_sdi_Init->rRet;
			
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}

#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rsdi_Init)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_Init);
	}

	if(NULL != Alligned_rIPTVES_TZIf_sdi_Init)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_sdi_Init);
	}

	
	return i4Ret;


}


static MT_RESULT_T  _MTDRM_IPTVES_SDI_FIN(unsigned long arg)
{
	MTDRM_TZIF_IPTVES_sdi_Fin_T *Alligned_rIPTVES_sdi_Fin = NULL;
	INT32 i4Ret = 0;
	Alligned_rIPTVES_sdi_Fin = (MTDRM_TZIF_IPTVES_sdi_Fin_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_Fin_T), 32));/*kernel data*/
	if (!Alligned_rIPTVES_sdi_Fin)
	{
		printf("Allocate Alligned_rIPTVES_sdi_Fin failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_sdi_Fin->rRet = MTR_NOT_OK;

	//printf("\n>>%s\n", __FUNCTION__);

#ifdef CC_ENABLE_HDCP2	


		TZ_NWD_BufferStart((UINT32)Alligned_rIPTVES_sdi_Fin, TZ_CACHE_ALIGN(sizeof(Alligned_rIPTVES_sdi_Fin)));

		i4Ret = TZ_IPTVES_SDI_FIN(Alligned_rIPTVES_sdi_Fin, sizeof(MTDRM_TZIF_IPTVES_sdi_Fin_T));

		TZ_NWD_BufferEnd((UINT32)Alligned_rIPTVES_sdi_Fin, TZ_CACHE_ALIGN(sizeof(Alligned_rIPTVES_sdi_Fin)));
		
		if (i4Ret != 0)
		{
			//printf("\n>>%s ret11: %d\n", __FUNCTION__, Alligned_rIPTVES_sdi_Fin->rRet);
			i4Ret = Alligned_rIPTVES_sdi_Fin->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
			//printf("\n>>%s ret22: %d\n", __FUNCTION__, i4Ret);
		}
#else
		i4Ret = MTR_NOT_OK;
#endif

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_sdi_Fin);
	return i4Ret;

}



static MT_RESULT_T  _MTDRM_IPTVES_SDI_CleanupNVS(unsigned long arg)
{
	MTDRM_TZIF_IPTVES_sdi_CleanupNVS_T *Alligned_rIPTVES_sdi_CleanupNVS = NULL;
	INT32 i4Ret = 0;
	Alligned_rIPTVES_sdi_CleanupNVS = (MTDRM_TZIF_IPTVES_sdi_CleanupNVS_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_CleanupNVS_T), 32));/*kernel data*/
	if (!Alligned_rIPTVES_sdi_CleanupNVS)
	{
		printf("Allocate Alligned_rIPTVES_sdi_CleanupNVS failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_sdi_CleanupNVS->rRet = MTR_NOT_OK;

#ifdef CC_ENABLE_HDCP2		  
		i4Ret = TZ_IPTVES_SDI_CleanupNVS(Alligned_rIPTVES_sdi_CleanupNVS, 0);
		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_sdi_CleanupNVS->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
#else
		i4Ret = MTR_NOT_OK;
#endif

	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_sdi_CleanupNVS);
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_SDI_GetDeviceID(unsigned long arg)
{
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTDRM_IPTV_sdi_GetDeviceID_T *prsdi_GetDeviceID;
    MTDRM_IPTV_sdi_GetDeviceID_T *Alligned_rsdi_GetDeviceID = NULL;/*kernel data*/
    MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T *Alligned_rIPTVES_TZIf_GetDeviceID = NULL;
    UINT32 u4Len;

	
	

	printf("\n>>%s:\n", __FUNCTION__);

    Alligned_rsdi_GetDeviceID = (MTDRM_IPTV_sdi_GetDeviceID_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_GetDeviceID_T), 32));/*kernel data*/
    
    if (!Alligned_rsdi_GetDeviceID)
    {
        printf("Allocate Alligned_rsdi_GetDeviceID failed\n");
        return MTR_NOT_OK;
    }
    Alligned_rIPTVES_TZIf_GetDeviceID = (MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T), 32));
    if (!Alligned_rIPTVES_TZIf_GetDeviceID)
    {
        printf("Allocate Alligned_rIPTVES_TZIf_GetDeviceID failed\n");
        return MTR_NOT_OK;
    }    



	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);



    prsdi_GetDeviceID = (MTDRM_IPTV_sdi_GetDeviceID_T*)rArg.ai4Arg[0];
    u4Len = (UINT32)rArg.ai4Arg[1];



    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_GetDeviceID, u4Len);
    COPY_FROM_USER_ARG_SIZE(prsdi_GetDeviceID, Alligned_rsdi_GetDeviceID, u4Len);



	Alligned_rIPTVES_TZIf_GetDeviceID->in_thid = Alligned_rsdi_GetDeviceID->in_thid;
	Alligned_rIPTVES_TZIf_GetDeviceID->out_devid = (VOID*)PHYSICAL((UINT32)Alligned_rsdi_GetDeviceID->out_devid); 
	Alligned_rIPTVES_TZIf_GetDeviceID->out_devid_len = MTAL_PO_DEVICE_ID_LEN;

	Alligned_rIPTVES_TZIf_GetDeviceID->rRet = MTR_NOT_OK;   



	
#ifdef CC_ENABLE_HDCP2
        TZ_NWD_BufferStart((UINT32)Alligned_rsdi_GetDeviceID, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_GetDeviceID_T)));


        i4Ret = TZ_IPTVES_SDI_GetDeviceID(Alligned_rIPTVES_TZIf_GetDeviceID, sizeof(MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T));

	
		
        TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_GetDeviceID, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_GetDeviceID_T)));



        if (i4Ret != 0)
        {
            i4Ret = Alligned_rIPTVES_TZIf_GetDeviceID->rRet;

        }
        else
        {

            i4Ret = MTR_NOT_OK;
        }
		

		
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_GetDeviceID, u4Len);

        COPY_TO_USER_ARG_SIZE(prsdi_GetDeviceID, Alligned_rsdi_GetDeviceID, u4Len);

#else
        i4Ret = MTR_NOT_OK;
#endif
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_GetDeviceID);
    BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_GetDeviceID);
    return i4Ret;

}


static MT_RESULT_T  _MTDRM_IPTVES_SDI_VerifyServiceMetaData(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sdi_VerifyServiceMetaData_T *prsdi_VerifyServiceMetaData;
	MTDRM_IPTV_sdi_VerifyServiceMetaData_T *Alligned_rsdi_VerifyServiceMetaData = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T *Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_msg     = NULL;
	int in_msg_len = 0;
	const unsigned char       *in_pkipath = NULL;
	int in_pkipath_len = 0;	
	const unsigned char       *in_sig     = NULL;	
	int in_sig_len = 0;


	unsigned char 	  *Alligned_in_msg = NULL;
	unsigned char       *Alligned_in_pkipath = NULL;
	unsigned char       *Alligned_in_sig = NULL;	


	//printf(">>%s:\n", __FUNCTION__);

	Alligned_rsdi_VerifyServiceMetaData = (MTDRM_IPTV_sdi_VerifyServiceMetaData_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_VerifyServiceMetaData_T), 32));/*kernel data*/
	
	if (!Alligned_rsdi_VerifyServiceMetaData)
	{
		printf("Allocate Alligned_rsdi_VerifyServiceMetaData failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData = (MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T), 32));
	if (!Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prsdi_VerifyServiceMetaData = (MTDRM_IPTV_sdi_VerifyServiceMetaData_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_VerifyServiceMetaData, u4Len);
	COPY_FROM_USER_ARG_SIZE(prsdi_VerifyServiceMetaData, Alligned_rsdi_VerifyServiceMetaData, u4Len);
	//printf("%s copy prsdi_VerifyServiceMetaData ok\n", __FUNCTION__);


	/* get user space data: */
	in_msg     = Alligned_rsdi_VerifyServiceMetaData->in_msg;
	in_msg_len = Alligned_rsdi_VerifyServiceMetaData->in_msg_len;
	
	in_pkipath     =  Alligned_rsdi_VerifyServiceMetaData->in_pkipath;
	in_pkipath_len =  Alligned_rsdi_VerifyServiceMetaData->in_pkipath_len;	
	
	in_sig     = Alligned_rsdi_VerifyServiceMetaData->in_sig;	
	in_sig_len = MTAL_PO_SIG_LEN;/* to -do fix, get from certmng.c*/;
	

	/* malloc aligned memory */
    Alligned_in_msg =     (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_msg_len, 32));/*kernel data*/
    Alligned_in_pkipath = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_pkipath_len, 32));/*kernel data*/
    Alligned_in_sig =     (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_sig_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_msg || NULL == Alligned_in_pkipath || NULL == Alligned_in_sig)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_msg, in_msg_len);
    COPY_FROM_USER_ARG_SIZE(in_msg, Alligned_in_msg, in_msg_len);
	//printf("%s copy Alligned_in_msg ok\n", __FUNCTION__);




    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_pkipath, in_pkipath_len);
    COPY_FROM_USER_ARG_SIZE(in_pkipath, Alligned_in_pkipath, in_pkipath_len);
	//printf("%s copy Alligned_in_pkipath ok\n", __FUNCTION__);




	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_sig, in_sig_len);
    COPY_FROM_USER_ARG_SIZE(in_sig, Alligned_in_sig, in_sig_len);
	//printf("%s copy Alligned_in_sig ok\n", __FUNCTION__);


	
	
	/* set TZ IF parameters */
	
	printf("\n%s: in_thid:%d\n", __FUNCTION__, Alligned_rsdi_VerifyServiceMetaData->in_thid);
	
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_thid = Alligned_rsdi_VerifyServiceMetaData->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_msg= (VOID*)PHYSICAL((UINT32)Alligned_in_msg); 
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_pkipath= (VOID*)PHYSICAL((UINT32)Alligned_in_pkipath); 
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_sig = (VOID*)PHYSICAL((UINT32)Alligned_in_sig); 

	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_msg_len     = in_msg_len;
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_pkipath_len = in_pkipath_len;
	Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->in_sig_len     = in_sig_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rsdi_VerifyServiceMetaData, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_VerifyServiceMetaData_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_msg, TZ_CACHE_ALIGN(in_msg_len));
		TZ_NWD_BufferStart((UINT32)Alligned_in_pkipath, TZ_CACHE_ALIGN(in_pkipath_len));
		TZ_NWD_BufferStart((UINT32)Alligned_in_sig, TZ_CACHE_ALIGN(in_sig_len));



		i4Ret = TZ_IPTVES_SDI_VerifyServiceMetaData(Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData,sizeof(MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_VerifyServiceMetaData, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_VerifyServiceMetaData_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_in_msg, TZ_CACHE_ALIGN(in_msg_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_pkipath, TZ_CACHE_ALIGN(in_pkipath_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_sig, TZ_CACHE_ALIGN(in_sig_len));
		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		
		//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_VerifyServiceMetaData, u4Len);
		//COPY_TO_USER_ARG_SIZE(prsdi_VerifyServiceMetaData, Alligned_rsdi_VerifyServiceMetaData, u4Len);
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rsdi_VerifyServiceMetaData)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_VerifyServiceMetaData);
	}

	if(NULL != Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_sdi_VerifyServiceMetaData);
	}

//   free malloced aligned buffer

	if(NULL != Alligned_in_msg)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_msg);
	}


	if(NULL != Alligned_in_pkipath)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_pkipath);
	}

	if(NULL != Alligned_in_sig)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_sig);
	}	


	
	return i4Ret;


}

static MT_RESULT_T  _MTDRM_IPTVES_SDI_JudgeCRLUpdate(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sdi_JudgeCRLUpdate_T *prsdi_JudgeCRLUpdate;
	MTDRM_IPTV_sdi_JudgeCRLUpdate_T *Alligned_rsdi_JudgeCRLUpdate = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T *Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate = NULL;
	UINT32 u4Len;


	/* value needed to return from tee */
	int out_is_requested = 0; 


	Alligned_rsdi_JudgeCRLUpdate = (MTDRM_IPTV_sdi_JudgeCRLUpdate_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_JudgeCRLUpdate_T), 32));/*kernel data*/
	
	if (!Alligned_rsdi_JudgeCRLUpdate)
	{
		printf("Allocate Alligned_rsdi_JudgeCRLUpdate failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate = (MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T), 32));
	if (!Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prsdi_JudgeCRLUpdate = (MTDRM_IPTV_sdi_JudgeCRLUpdate_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_JudgeCRLUpdate, u4Len);
	COPY_FROM_USER_ARG_SIZE(prsdi_JudgeCRLUpdate, Alligned_rsdi_JudgeCRLUpdate, u4Len);



	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate->in_thid = Alligned_rsdi_JudgeCRLUpdate->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate->rRet = MTR_NOT_OK;  



#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rsdi_JudgeCRLUpdate, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_JudgeCRLUpdate_T)));
		i4Ret = TZ_IPTVES_SDI_JudgeCRLUpdate(Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate,sizeof(MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_JudgeCRLUpdate, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_JudgeCRLUpdate_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate->rRet;

			/* value needed to return from tee */			
			out_is_requested = Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate->out_is_requested;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_JudgeCRLUpdate->out_is_requested, sizeof(prsdi_JudgeCRLUpdate->out_is_requested));
		COPY_TO_USER_ARG_SIZE(prsdi_JudgeCRLUpdate->out_is_requested, &out_is_requested, sizeof(prsdi_JudgeCRLUpdate->out_is_requested));
#else
		out_is_requested = 0;
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rsdi_JudgeCRLUpdate)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_JudgeCRLUpdate);
	}

	if(NULL != Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_sdi_JudgeCRLUpdate);
	}

	
	return i4Ret;


}




static MT_RESULT_T  _MTDRM_IPTVES_SDI_GetCRLDstPoint(unsigned long arg)
{
	INT32 i4Ret;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sdi_GetCRLDstPoint_T *prsdi_GetCRLDstPoint;
	MTDRM_IPTV_sdi_GetCRLDstPoint_T *Alligned_rsdi_GetCRLDstPoint = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T *Alligned_rIPTVES_TZIf_GetCRLDstPoint = NULL;
	UINT32 u4Len;

	Alligned_rsdi_GetCRLDstPoint = (MTDRM_IPTV_sdi_GetCRLDstPoint_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_GetCRLDstPoint_T), 32));/*kernel data*/
	
	if (!Alligned_rsdi_GetCRLDstPoint)
	{
		printf("Allocate Alligned_rsdi_GetCRLDstPoint failed\n");
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_GetCRLDstPoint = (MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T), 32));
	if (!Alligned_rIPTVES_TZIf_GetCRLDstPoint)
	{
		printf("Allocate Alligned_rIPTVES_TZIf_GetCRLDstPoint failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prsdi_GetCRLDstPoint = (MTDRM_IPTV_sdi_GetCRLDstPoint_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_GetCRLDstPoint, u4Len);
	COPY_FROM_USER_ARG_SIZE(prsdi_GetCRLDstPoint, Alligned_rsdi_GetCRLDstPoint, u4Len);

	Alligned_rIPTVES_TZIf_GetCRLDstPoint->in_thid = Alligned_rsdi_GetCRLDstPoint->in_thid;
	Alligned_rIPTVES_TZIf_GetCRLDstPoint->out_crl_dp = (VOID*)PHYSICAL((UINT32)Alligned_rsdi_GetCRLDstPoint->out_crl_dp); 
	Alligned_rIPTVES_TZIf_GetCRLDstPoint->out_crl_dp_len = MTAL_PO_MAX_CRL_DP_LEN;

	Alligned_rIPTVES_TZIf_GetCRLDstPoint->rRet = MTR_NOT_OK;   
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rsdi_GetCRLDstPoint, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_GetCRLDstPoint_T)));
		i4Ret = TZ_IPTVES_SDI_GetCRLDstPoint(Alligned_rIPTVES_TZIf_GetCRLDstPoint,sizeof(MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_GetCRLDstPoint, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_GetCRLDstPoint_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_GetCRLDstPoint->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_GetCRLDstPoint, u4Len);
		COPY_TO_USER_ARG_SIZE(prsdi_GetCRLDstPoint, Alligned_rsdi_GetCRLDstPoint, u4Len);
#else
		i4Ret = MTR_NOT_OK;
#endif
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_GetCRLDstPoint);
	BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_GetCRLDstPoint);
	return i4Ret;

}



static MT_RESULT_T  _MTDRM_IPTVES_SDI_SetCRL(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_sdi_SetCRL_T *prsdi_SetCRL;
	MTDRM_IPTV_sdi_SetCRL_T *Alligned_rsdi_SetCRL = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_sdi_SetCRL_T *Alligned_rIPTVES_TZIf_sdi_SetCRL = NULL;
	UINT32 u4Len;

	/* value needed to return from tee */
	int out_is_updated = 0;		


	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_crl	  = NULL;
	int in_crl_len = 0;



	unsigned char	  *Alligned_in_crl = NULL;



	Alligned_rsdi_SetCRL = (MTDRM_IPTV_sdi_SetCRL_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_SetCRL_T), 32));/*kernel data*/
	
	if (NULL == Alligned_rsdi_SetCRL)
	{
		printf("Allocate Alligned_rsdi_SetCRL failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_sdi_SetCRL = (MTDRM_TZIF_IPTVES_sdi_SetCRL_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_SetCRL_T), 32));
	if (NULL == Alligned_rIPTVES_TZIf_sdi_SetCRL)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_sdi_SetCRL failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prsdi_SetCRL = (MTDRM_IPTV_sdi_SetCRL_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_SetCRL, u4Len);
	COPY_FROM_USER_ARG_SIZE(prsdi_SetCRL, Alligned_rsdi_SetCRL, u4Len);


	/* get user space data: */
	in_crl	   = Alligned_rsdi_SetCRL->in_crl;
	in_crl_len = Alligned_rsdi_SetCRL->in_crl_len;
	
	//printf("thid:%d\n", in_thid);
	printf("in_crl:%s", in_crl);
	printf("in_crl_len:%d", in_crl_len);


	/* malloc aligned memory */
	Alligned_in_crl =	  (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_crl_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_crl)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_crl, in_crl_len);
	COPY_FROM_USER_ARG_SIZE(in_crl, Alligned_in_crl, in_crl_len);
	printf("\n%s: Alligned_in_crl COPY OK\n", __FUNCTION__);

	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_sdi_SetCRL->in_thid = Alligned_rsdi_SetCRL->in_thid;

	printf("\n%s: in_thid:%d\n", __FUNCTION__, Alligned_rsdi_SetCRL->in_thid);

	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_sdi_SetCRL->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_sdi_SetCRL->in_crl= (VOID*)PHYSICAL((UINT32)Alligned_in_crl); 
	Alligned_rIPTVES_TZIf_sdi_SetCRL->in_crl_len = in_crl_len;

 
	
#ifdef CC_ENABLE_HDCP2
		printf("\n%s: aaaaa\n", __FUNCTION__);

		TZ_NWD_BufferStart((UINT32)Alligned_rsdi_SetCRL, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_SetCRL_T)));
		printf("\n%s: bbbbb\n", __FUNCTION__);

		TZ_NWD_BufferStart((UINT32)Alligned_in_crl, TZ_CACHE_ALIGN(in_crl_len));
		printf("\n%s: ccccc\n", __FUNCTION__);

		i4Ret = TZ_IPTVES_SDI_SetCRL(Alligned_rIPTVES_TZIf_sdi_SetCRL,sizeof(MTDRM_TZIF_IPTVES_sdi_SetCRL_T));
			printf("\n%s: ddddd\n", __FUNCTION__);
		TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_SetCRL, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_SetCRL_T)));
		printf("\n%s: eeeee\n", __FUNCTION__);

		TZ_NWD_BufferEnd((UINT32)Alligned_in_crl, TZ_CACHE_ALIGN(in_crl_len));

		printf("\n%s: ffffff\n", __FUNCTION__);

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_sdi_SetCRL->rRet;

			/* value needed to return from tee */			
			out_is_updated = Alligned_rIPTVES_TZIf_sdi_SetCRL->out_is_updated;
			
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_SetCRL->out_is_updated, sizeof(prsdi_SetCRL->out_is_updated));
		COPY_TO_USER_ARG_SIZE(prsdi_SetCRL->out_is_updated, &out_is_updated, sizeof(prsdi_SetCRL->out_is_updated));

#else
		out_is_updated = 0;
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rsdi_SetCRL)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_SetCRL);
	}

	if(NULL != Alligned_rIPTVES_TZIf_sdi_SetCRL)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_sdi_SetCRL);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_crl)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_crl);
	}


	
	return i4Ret;


}


static MT_RESULT_T  _MTDRM_IPTVES_SDI_CheckTrustedTime(unsigned long arg)
{
		INT32 i4Ret = 0;
		MTAL_IOCTL_2ARG_T rArg;
		MTDRM_IPTV_sdi_CheckTrustedTime_T *prsdi_CheckTrustedTime;
		MTDRM_IPTV_sdi_CheckTrustedTime_T *Alligned_rsdi_CheckTrustedTime = NULL;/*kernel data*/
		MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T *Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime = NULL;
		UINT32 u4Len;
	
		/* value needed to return from tee */
		int out_is_valid = 0;
	
	
	
	
		Alligned_rsdi_CheckTrustedTime = (MTDRM_IPTV_sdi_CheckTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_sdi_CheckTrustedTime_T), 32));/*kernel data*/
		
		if (!Alligned_rsdi_CheckTrustedTime)
		{
			printf("Allocate Alligned_rsdi_CheckTrustedTime failed\n");
			goto out;		
			return MTR_NOT_OK;
		}
		Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime = (MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T), 32));
		if (!Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime)
		{
			goto out;			
			printf("Allocate Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime failed\n");
			return MTR_NOT_OK;
		}	 
		
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);
	
		prsdi_CheckTrustedTime = (MTDRM_IPTV_sdi_CheckTrustedTime_T*)rArg.ai4Arg[0];
		u4Len = (UINT32)rArg.ai4Arg[1];
	
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_CheckTrustedTime, u4Len);
		COPY_FROM_USER_ARG_SIZE(prsdi_CheckTrustedTime, Alligned_rsdi_CheckTrustedTime, u4Len);
		
		
		/* set TZ IF parameters */
	
		//Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime->in_thid = Alligned_rsdi_CheckTrustedTime->in_thid;
		
		/* ret value: default NOT_OK*/
		Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime->rRet = MTR_NOT_OK;	
	
	

	 
		
#ifdef CC_ENABLE_HDCP2
			TZ_NWD_BufferStart((UINT32)Alligned_rsdi_CheckTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_CheckTrustedTime_T)));
			i4Ret = TZ_IPTVES_SDI_CheckTrustedTime(Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime,sizeof(MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T));
			TZ_NWD_BufferEnd((UINT32)Alligned_rsdi_CheckTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_sdi_CheckTrustedTime_T)));
	
			if (i4Ret != 0)
			{
				i4Ret = Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime->rRet;
	
				/* value needed to return from tee */			
				out_is_valid = Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime->out_is_valid;
				
			}
			else
			{
				i4Ret = MTR_NOT_OK;
			}
	
	
			/* need data out to user space ?*/
			USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prsdi_CheckTrustedTime->out_is_valid, sizeof(prsdi_CheckTrustedTime->out_is_valid));
			COPY_TO_USER_ARG_SIZE(prsdi_CheckTrustedTime->out_is_valid, &out_is_valid, sizeof(prsdi_CheckTrustedTime->out_is_valid));
	
#else
			out_is_valid = 0;
			i4Ret = MTR_NOT_OK;
#endif
	
	out:
	
		if(NULL != Alligned_rsdi_CheckTrustedTime)
		{
			BSP_FreeAlignedDmaMemory((UINT32)Alligned_rsdi_CheckTrustedTime);
		}
	
		if(NULL != Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime)
		{
			BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_sdi_CheckTrustedTime);
		}
	

		
		return i4Ret;
	


}

static MT_RESULT_T  _MTDRM_IPTVES_ECM_ExtractScrambleKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_ecm_ExtractScrambleKey_T *precm_ExtractScrambleKey;
	MTDRM_IPTV_ecm_ExtractScrambleKey_T *Alligned_recm_ExtractScrambleKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T *Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey = NULL;
	UINT32 u4Len;

	/* value needed to return from tee */
	//int out_is_updated = 0;


	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_ecm	  = NULL;
	int in_ecm_len = 0;



	unsigned char	  *Alligned_in_ecm = NULL;



	Alligned_recm_ExtractScrambleKey = (MTDRM_IPTV_ecm_ExtractScrambleKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_ecm_ExtractScrambleKey_T), 32));/*kernel data*/
	
	if (!Alligned_recm_ExtractScrambleKey)
	{
		printf("Allocate Alligned_recm_ExtractScrambleKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey = (MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	precm_ExtractScrambleKey = (MTDRM_IPTV_ecm_ExtractScrambleKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(precm_ExtractScrambleKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(precm_ExtractScrambleKey, Alligned_recm_ExtractScrambleKey, u4Len);


	/* get user space data: */
	in_ecm	   = Alligned_recm_ExtractScrambleKey->in_ecm;
	in_ecm_len = Alligned_recm_ExtractScrambleKey->in_ecm_len;	// should be 98
	


	/* malloc aligned memory */
	Alligned_in_ecm =	  (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_ecm_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_ecm)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_ecm, in_ecm_len);
	COPY_FROM_USER_ARG_SIZE(in_ecm, Alligned_in_ecm, in_ecm_len);

	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_thid    = Alligned_recm_ExtractScrambleKey->in_thid;
	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_rend_id = Alligned_recm_ExtractScrambleKey->in_rend_id;

	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_ecm = (VOID*)PHYSICAL((UINT32)Alligned_in_ecm); 
	Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_ecm_len = in_ecm_len;	


 
	printf("%s thid:%d rend_id:%d ecm_len:%d\n", __FUNCTION__, 
		 Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_thid, 
		 Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_rend_id,
		 Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->in_ecm_len);
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_recm_ExtractScrambleKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_ecm_ExtractScrambleKey_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_ecm, TZ_CACHE_ALIGN(in_ecm_len));


		i4Ret = TZ_IPTVES_ECM_ExtractScrambleKey(Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey,sizeof(MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_recm_ExtractScrambleKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_ecm_ExtractScrambleKey_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_in_ecm, TZ_CACHE_ALIGN(in_ecm_len));


		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->rRet;

			/* value needed to return from tee */			
			//out_is_updated = Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey->out_is_updated;
			
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(precm_ExtractScrambleKey->out_is_updated, sizeof(precm_ExtractScrambleKey->out_is_updated));
		//COPY_TO_USER_ARG_SIZE(precm_ExtractScrambleKey->out_is_updated, &out_is_updated, sizeof(precm_ExtractScrambleKey->out_is_updated));

#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_recm_ExtractScrambleKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_recm_ExtractScrambleKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_ecm_ExtractScrambleKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_ecm)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_ecm);
	}


	
	return i4Ret;


}

static MT_RESULT_T  _MTDRM_IPTVES_ECM_GetScrambleKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_ecm_GetScrambleKey_T *precm_GetScrambleKey;
	MTDRM_IPTV_ecm_GetScrambleKey_T *Alligned_recm_GetScrambleKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T *Alligned_rIPTVES_TZIf_ecm_GetScrambleKey = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_ecm	  = NULL;
	int in_ecm_len = 0;
	const unsigned char 	  *out_skey_even = NULL;
	int out_skey_even_len = 0;	
	const unsigned char 	  *out_skey_odd 	= NULL; 
	int out_skey_odd_len = 0;


	unsigned char	  *Alligned_in_ecm = NULL;
	unsigned char	  *Alligned_out_skey_even = NULL;
	unsigned char	  *Alligned_out_skey_odd = NULL;	



	Alligned_recm_GetScrambleKey = (MTDRM_IPTV_ecm_GetScrambleKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_ecm_GetScrambleKey_T), 32));/*kernel data*/
	
	if (!Alligned_recm_GetScrambleKey)
	{
		printf("Allocate Alligned_recm_GetScrambleKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey = (MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_ecm_GetScrambleKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_ecm_GetScrambleKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	precm_GetScrambleKey = (MTDRM_IPTV_ecm_GetScrambleKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(precm_GetScrambleKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(precm_GetScrambleKey, Alligned_recm_GetScrambleKey, u4Len);


	/* get user space data: */
	in_ecm	   = Alligned_recm_GetScrambleKey->in_ecm;
	in_ecm_len = Alligned_recm_GetScrambleKey->in_ecm_len;
	
	out_skey_even	  =  Alligned_recm_GetScrambleKey->out_skey_even;
	out_skey_even_len =  Alligned_recm_GetScrambleKey->out_skey_even_len;	
	
	out_skey_odd	 = Alligned_recm_GetScrambleKey->out_skey_odd;	
	out_skey_odd_len =  Alligned_recm_GetScrambleKey->out_skey_odd_Len;
	

	/* malloc aligned memory */
	Alligned_in_ecm =	     (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_ecm_len, 32));/*kernel data*/
	Alligned_out_skey_even = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_skey_even_len, 32));/*kernel data*/
	Alligned_out_skey_odd =  (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_skey_odd_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_ecm || NULL == Alligned_out_skey_even || NULL == Alligned_out_skey_odd)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_ecm, in_ecm_len);
	COPY_FROM_USER_ARG_SIZE(in_ecm, Alligned_in_ecm, in_ecm_len);

	//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_skey_even, out_skey_even_len);
	//COPY_FROM_USER_ARG_SIZE(out_skey_even, Alligned_out_skey_even, out_skey_even_len);
	
	//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_skey_odd, out_skey_odd_len);
	//COPY_FROM_USER_ARG_SIZE(out_skey_odd, Alligned_out_skey_odd, out_skey_odd_len);

	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->in_thid = Alligned_recm_GetScrambleKey->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->in_ecm= (VOID*)PHYSICAL((UINT32)Alligned_in_ecm); 
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->out_skey_even= (VOID*)PHYSICAL((UINT32)Alligned_out_skey_even); 
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->out_skey_odd = (VOID*)PHYSICAL((UINT32)Alligned_out_skey_odd); 
		

	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->in_ecm_len = in_ecm_len;
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->out_skey_even_len = out_skey_even_len;
	Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->out_skey_odd_len = out_skey_odd_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_recm_GetScrambleKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_ecm_GetScrambleKey_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_ecm, TZ_CACHE_ALIGN(in_ecm_len));
		TZ_NWD_BufferStart((UINT32)Alligned_out_skey_even, TZ_CACHE_ALIGN(out_skey_even_len));
		TZ_NWD_BufferStart((UINT32)Alligned_out_skey_odd, TZ_CACHE_ALIGN(out_skey_odd_len));




		i4Ret = TZ_IPTVES_ECM_GetScrambleKey(Alligned_rIPTVES_TZIf_ecm_GetScrambleKey,sizeof(MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_recm_GetScrambleKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_ecm_GetScrambleKey_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_in_ecm, TZ_CACHE_ALIGN(in_ecm_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_skey_even, TZ_CACHE_ALIGN(out_skey_even_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_skey_odd, TZ_CACHE_ALIGN(out_skey_odd_len));

		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_ecm_GetScrambleKey->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		
		//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(precm_GetScrambleKey, u4Len);
		//COPY_TO_USER_ARG_SIZE(precm_GetScrambleKey, Alligned_recm_GetScrambleKey, u4Len);

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_skey_even, out_skey_even_len);
		COPY_TO_USER_ARG_SIZE(out_skey_even, Alligned_out_skey_even, out_skey_even_len);
		
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_skey_odd, out_skey_odd_len);
		COPY_TO_USER_ARG_SIZE(out_skey_odd, Alligned_out_skey_odd, out_skey_odd_len);

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_recm_GetScrambleKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_recm_GetScrambleKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_ecm_GetScrambleKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_ecm_GetScrambleKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_ecm)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_ecm);
	}


	if(NULL != Alligned_out_skey_even)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_skey_even);
	}

	if(NULL != Alligned_out_skey_odd)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_skey_odd);
	}	


	
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_ExtractContentKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_ExtractContentKey_T *prdrm_ExtractContentKey;
	MTDRM_IPTV_drm_ExtractContentKey_T *Alligned_rdrm_ExtractContentKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T *Alligned_rIPTVES_TZIf_drm_ExtractContentKey = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_key_ref	  = NULL;
	int in_key_ref_len = 0;


	unsigned char	  *Alligned_in_key_ref = NULL;


	Alligned_rdrm_ExtractContentKey = (MTDRM_IPTV_drm_ExtractContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_ExtractContentKey_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_ExtractContentKey)
	{
		printf("Allocate Alligned_rdrm_ExtractContentKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_ExtractContentKey = (MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_ExtractContentKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_ExtractContentKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_ExtractContentKey = (MTDRM_IPTV_drm_ExtractContentKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_ExtractContentKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_ExtractContentKey, Alligned_rdrm_ExtractContentKey, u4Len);


	/* get user space data: */
	in_key_ref	   = Alligned_rdrm_ExtractContentKey->in_key_ref;
	in_key_ref_len = Alligned_rdrm_ExtractContentKey->in_key_ref_len;
	
	/* malloc aligned memory */
	Alligned_in_key_ref =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_key_ref_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_key_ref)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_key_ref, in_key_ref_len);
	COPY_FROM_USER_ARG_SIZE(in_key_ref, Alligned_in_key_ref, in_key_ref_len);
	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_ExtractContentKey->in_thid = Alligned_rdrm_ExtractContentKey->in_thid;
	Alligned_rIPTVES_TZIf_drm_ExtractContentKey->in_rend_id = Alligned_rdrm_ExtractContentKey->in_rend_id;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_ExtractContentKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_ExtractContentKey->in_key_ref= (VOID*)PHYSICAL((UINT32)Alligned_in_key_ref); 
	Alligned_rIPTVES_TZIf_drm_ExtractContentKey->in_key_ref_len = in_key_ref_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_ExtractContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_ExtractContentKey_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));

		i4Ret = TZ_IPTVES_DRM_ExtractContentKey(Alligned_rIPTVES_TZIf_drm_ExtractContentKey,sizeof(MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T));

		
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_ExtractContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_ExtractContentKey_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_ExtractContentKey->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_ExtractContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_ExtractContentKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_ExtractContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_ExtractContentKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_key_ref)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_key_ref);
	}
		
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_DelContentKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_DelContentKey_T *prdrm_DelContentKey;
	MTDRM_IPTV_drm_DelContentKey_T *Alligned_rdrm_DelContentKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_DelContentKey_T *Alligned_rIPTVES_TZIf_drm_DelContentKey = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_key_ref	  = NULL;
	int in_key_ref_len = 0;


	unsigned char	  *Alligned_in_key_ref = NULL;


	Alligned_rdrm_DelContentKey = (MTDRM_IPTV_drm_DelContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_DelContentKey_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_DelContentKey)
	{
		printf("Allocate Alligned_rdrm_DelContentKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_DelContentKey = (MTDRM_TZIF_IPTVES_drm_DelContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_DelContentKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_DelContentKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_DelContentKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_DelContentKey = (MTDRM_IPTV_drm_DelContentKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_DelContentKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_DelContentKey, Alligned_rdrm_DelContentKey, u4Len);


	/* get user space data: */
	in_key_ref	   = Alligned_rdrm_DelContentKey->in_key_ref;
	in_key_ref_len = Alligned_rdrm_DelContentKey->in_key_ref_len;
	
	/* malloc aligned memory */
	Alligned_in_key_ref =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_key_ref_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_key_ref)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_key_ref, in_key_ref_len);
	COPY_FROM_USER_ARG_SIZE(in_key_ref, Alligned_in_key_ref, in_key_ref_len);
	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_DelContentKey->in_thid = Alligned_rdrm_DelContentKey->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_DelContentKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_DelContentKey->in_key_ref= (VOID*)PHYSICAL((UINT32)Alligned_in_key_ref); 
		
	Alligned_rIPTVES_TZIf_drm_DelContentKey->in_key_ref_len = in_key_ref_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_DelContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_DelContentKey_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));

		i4Ret = TZ_IPTVES_DRM_DelContentKey(Alligned_rIPTVES_TZIf_drm_DelContentKey,sizeof(MTDRM_TZIF_IPTVES_drm_DelContentKey_T));

		
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_DelContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_DelContentKey_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_DelContentKey->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_DelContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_DelContentKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_DelContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_DelContentKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_key_ref)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_key_ref);
	}
		
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_DelUncommittedKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_DelUncommittedKey_T *prdrm_DelUncommittedKey;
	MTDRM_IPTV_drm_DelUncommittedKey_T *Alligned_rdrm_DelUncommittedKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T *Alligned_rIPTVES_TZIf_drm_DelUncommittedKey = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_key_ref	  = NULL;
	int in_key_ref_len = 0;


	unsigned char	  *Alligned_in_key_ref = NULL;


	Alligned_rdrm_DelUncommittedKey = (MTDRM_IPTV_drm_DelUncommittedKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_DelUncommittedKey_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_DelUncommittedKey)
	{
		printf("Allocate Alligned_rdrm_DelUncommittedKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_DelUncommittedKey = (MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_DelUncommittedKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_DelUncommittedKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_DelUncommittedKey = (MTDRM_IPTV_drm_DelUncommittedKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_DelUncommittedKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_DelUncommittedKey, Alligned_rdrm_DelUncommittedKey, u4Len);


	/* get user space data: */
	in_key_ref	   = Alligned_rdrm_DelUncommittedKey->in_key_ref;
	in_key_ref_len = Alligned_rdrm_DelUncommittedKey->in_key_ref_len;
	
	/* malloc aligned memory */
	Alligned_in_key_ref =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_key_ref_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_key_ref)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_key_ref, in_key_ref_len);
	COPY_FROM_USER_ARG_SIZE(in_key_ref, Alligned_in_key_ref, in_key_ref_len);
	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_DelUncommittedKey->in_thid = Alligned_rdrm_DelUncommittedKey->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_DelUncommittedKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_DelUncommittedKey->in_key_ref= (VOID*)PHYSICAL((UINT32)Alligned_in_key_ref); 
	Alligned_rIPTVES_TZIf_drm_DelUncommittedKey->in_key_ref_len = in_key_ref_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_DelUncommittedKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_DelUncommittedKey_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));

		i4Ret = TZ_IPTVES_DRM_DelUncommittedKey(Alligned_rIPTVES_TZIf_drm_DelUncommittedKey,sizeof(MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T));

		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_DelUncommittedKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_DelUncommittedKey_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_DelUncommittedKey->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_DelUncommittedKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_DelUncommittedKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_DelUncommittedKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_DelUncommittedKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_key_ref)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_key_ref);
	}
		
	return i4Ret;

}



static MT_RESULT_T  _MTDRM_IPTVES_DRM_GetKeyInfo(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_GetKeyInfo_T *prdrm_GetKeyInfo;
	MTDRM_IPTV_drm_GetKeyInfo_T *Alligned_rdrm_GetKeyInfo = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T *Alligned_rIPTVES_TZIf_drm_GetKeyInfo = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_key_ref	  = NULL;
	int in_key_ref_len = 0;

	const unsigned char 	  *out_key_info	  = NULL;
	int out_key_info_len = 0;

	
	unsigned char	  *Alligned_in_key_ref = NULL;
	unsigned char	  *Alligned_out_key_info = NULL;




	Alligned_rdrm_GetKeyInfo = (MTDRM_IPTV_drm_GetKeyInfo_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_GetKeyInfo_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_GetKeyInfo)
	{
		printf("Allocate Alligned_rdrm_GetKeyInfo failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo = (MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_GetKeyInfo)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_GetKeyInfo failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_GetKeyInfo = (MTDRM_IPTV_drm_GetKeyInfo_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetKeyInfo, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_GetKeyInfo, Alligned_rdrm_GetKeyInfo, u4Len);


	/* get user space data: */
	in_key_ref	   = Alligned_rdrm_GetKeyInfo->in_key_ref;
	in_key_ref_len = Alligned_rdrm_GetKeyInfo->in_key_ref_len;


	out_key_info	   = Alligned_rdrm_GetKeyInfo->out_key_info;
	out_key_info_len   = Alligned_rdrm_GetKeyInfo->out_key_info_len;

	
	printf("%s in_key_ref:%08x len:%d\n", __FUNCTION__, in_key_ref, in_key_ref_len);
	printf("%s out_key_info:%08x len:%d\n", __FUNCTION__, out_key_info, out_key_info_len);
	
	/* malloc aligned memory */
	Alligned_in_key_ref =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_key_ref_len, 32));/*kernel data*/
	Alligned_out_key_info =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_key_info_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_key_ref || NULL == Alligned_out_key_info)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_key_ref, in_key_ref_len);
	COPY_FROM_USER_ARG_SIZE(in_key_ref, Alligned_in_key_ref, in_key_ref_len);
	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->in_thid = Alligned_rdrm_GetKeyInfo->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->in_key_ref= (VOID*)PHYSICAL((UINT32)Alligned_in_key_ref); 
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->in_key_ref_len = in_key_ref_len;

		
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->out_key_info= (VOID*)PHYSICAL((UINT32)Alligned_out_key_info); 
	Alligned_rIPTVES_TZIf_drm_GetKeyInfo->out_key_info_len = out_key_info_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_GetKeyInfo, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyInfo_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));
		TZ_NWD_BufferStart((UINT32)Alligned_out_key_info, TZ_CACHE_ALIGN(out_key_info_len));

		

		i4Ret = TZ_IPTVES_DRM_GetKeyInfo(Alligned_rIPTVES_TZIf_drm_GetKeyInfo,sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T));
		
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_GetKeyInfo, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyInfo_T)));
		
		TZ_NWD_BufferEnd((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_key_info, TZ_CACHE_ALIGN(out_key_info_len));


		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_GetKeyInfo->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_key_info, out_key_info_len);
		COPY_TO_USER_ARG_SIZE(out_key_info, Alligned_out_key_info, out_key_info_len);

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_GetKeyInfo)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_GetKeyInfo);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_GetKeyInfo)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_GetKeyInfo);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_key_ref)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_key_ref);
	}

	if(NULL != Alligned_out_key_info)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_key_info);
	}
		

		
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_GetKeyNum(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_GetKeyNum_T *prdrm_GetKeyNum;
	MTDRM_IPTV_drm_GetKeyNum_T *Alligned_rdrm_GetKeyNum = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_GetKeyNum_T *Alligned_rIPTVES_TZIf_drm_GetKeyNum = NULL;
	UINT32 u4Len;

	/* value needed to return from tee */
	int out_num = 0;




	Alligned_rdrm_GetKeyNum = (MTDRM_IPTV_drm_GetKeyNum_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_GetKeyNum_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_GetKeyNum)
	{
		printf("Allocate Alligned_rdrm_GetKeyNum failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_GetKeyNum = (MTDRM_TZIF_IPTVES_drm_GetKeyNum_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyNum_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_GetKeyNum)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_GetKeyNum failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_GetKeyNum = (MTDRM_IPTV_drm_GetKeyNum_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetKeyNum, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_GetKeyNum, Alligned_rdrm_GetKeyNum, u4Len);
	
	
	/* set TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_GetKeyNum->in_thid  =  Alligned_rdrm_GetKeyNum->in_thid;

	Alligned_rIPTVES_TZIf_drm_GetKeyNum->in_action_id = Alligned_rdrm_GetKeyNum->in_action_id;

	Alligned_rIPTVES_TZIf_drm_GetKeyNum->out_num = 0;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_GetKeyNum->rRet = MTR_NOT_OK; 

	printf("\n %s: thid:%d  action_id:%d\n", __FUNCTION__, Alligned_rIPTVES_TZIf_drm_GetKeyNum->in_thid,  Alligned_rIPTVES_TZIf_drm_GetKeyNum->in_action_id);


 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_GetKeyNum, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyNum_T)));
		i4Ret = TZ_IPTVES_DRM_GetKeyNum(Alligned_rIPTVES_TZIf_drm_GetKeyNum,sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyNum_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_GetKeyNum, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyNum_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_GetKeyNum->rRet;

			/* value needed to return from tee */			
			out_num = Alligned_rIPTVES_TZIf_drm_GetKeyNum->out_num;

			printf("\n %s: out_num %d\n", __FUNCTION__, out_num);
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetKeyNum->out_num, sizeof(prdrm_GetKeyNum->out_num));
		COPY_TO_USER_ARG_SIZE(prdrm_GetKeyNum->out_num, &out_num, sizeof(prdrm_GetKeyNum->out_num));

#else
		out_num = 0;
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_GetKeyNum)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_GetKeyNum);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_GetKeyNum)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_GetKeyNum);
	}


	
	return i4Ret;
		
	

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_GetKeyRefList(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_GetKeyRefList_T *prdrm_GetKeyRefList;
	MTDRM_IPTV_drm_GetKeyRefList_T *Alligned_rdrm_GetKeyRefList = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T *Alligned_rIPTVES_TZIf_drm_GetKeyRefList = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *out_key_ref_lst	  = NULL;
	int out_key_ref_lst_len = 0;

	unsigned int  *io_num	  = NULL;
	int io_num_len = 0;

	
	unsigned char	  *Alligned_out_key_ref_lst = NULL;
	unsigned char	  *Alligned_io_num = NULL;




	Alligned_rdrm_GetKeyRefList = (MTDRM_IPTV_drm_GetKeyRefList_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_GetKeyRefList_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_GetKeyRefList)
	{
		printf("Allocate Alligned_rdrm_GetKeyRefList failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList = (MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_GetKeyRefList)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_GetKeyRefList failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_GetKeyRefList = (MTDRM_IPTV_drm_GetKeyRefList_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetKeyRefList, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_GetKeyRefList, Alligned_rdrm_GetKeyRefList, u4Len);


	/* get user space data: */
	out_key_ref_lst    = Alligned_rdrm_GetKeyRefList->out_key_ref_lst;
	out_key_ref_lst_len = Alligned_rdrm_GetKeyRefList->out_key_ref_lst_len;


	io_num	   = Alligned_rdrm_GetKeyRefList->io_num;
	io_num_len = sizeof(unsigned int);// Alligned_rdrm_GetKeyRefList->io_num_len;

	

	
	
	/* malloc aligned memory */
	Alligned_out_key_ref_lst =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_key_ref_lst_len, 32));/*kernel data*/
	Alligned_io_num          =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(io_num_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_out_key_ref_lst || NULL == Alligned_io_num)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_key_ref_lst, out_key_ref_lst_len);
	//COPY_FROM_USER_ARG_SIZE(out_key_ref_lst, Alligned_out_key_ref_lst, out_key_ref_lst_len);
	
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(io_num, io_num_len);
	COPY_FROM_USER_ARG_SIZE(io_num, Alligned_io_num, io_num_len);
		


	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->in_thid = Alligned_rdrm_GetKeyRefList->in_thid;
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->in_action_id= Alligned_rdrm_GetKeyRefList->in_action_id;


	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->out_key_ref_lst= (VOID*)PHYSICAL((UINT32)Alligned_out_key_ref_lst); 
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->out_key_ref_lst_len = out_key_ref_lst_len;

		
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->io_num= (VOID*)PHYSICAL((UINT32)Alligned_io_num); 
	Alligned_rIPTVES_TZIf_drm_GetKeyRefList->io_num_len = io_num_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_GetKeyRefList, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyRefList_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_out_key_ref_lst, TZ_CACHE_ALIGN(out_key_ref_lst_len));
		TZ_NWD_BufferStart((UINT32)Alligned_io_num, TZ_CACHE_ALIGN(io_num_len));

		i4Ret = TZ_IPTVES_DRM_GetKeyRefList(Alligned_rIPTVES_TZIf_drm_GetKeyRefList,sizeof(MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T));

		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_GetKeyRefList, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetKeyRefList_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_out_key_ref_lst, TZ_CACHE_ALIGN(out_key_ref_lst_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_io_num, TZ_CACHE_ALIGN(io_num_len));
		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_GetKeyRefList->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(io_num, io_num_len);
		COPY_TO_USER_ARG_SIZE(io_num, Alligned_io_num, io_num_len);

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_key_ref_lst, out_key_ref_lst_len);
		COPY_TO_USER_ARG_SIZE(out_key_ref_lst, Alligned_out_key_ref_lst, out_key_ref_lst_len);


		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_GetKeyRefList)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_GetKeyRefList);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_GetKeyRefList)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_GetKeyRefList);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_out_key_ref_lst)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_key_ref_lst);
	}
	if(NULL != Alligned_io_num)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_io_num);
	}


		
	return i4Ret;

}

static MT_RESULT_T  _MTDRM_IPTVES_DRM_GetContentKey(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_drm_GetContentKey_T *prdrm_GetContentKey;
	MTDRM_IPTV_drm_GetContentKey_T *Alligned_rdrm_GetContentKey = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_drm_GetContentKey_T *Alligned_rIPTVES_TZIf_drm_GetContentKey = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_key_ref	  = NULL;
	int in_key_ref_len = 0;

	const unsigned char 	  *out_ckey   = NULL;
	int out_ckey_len = 0;

	
	unsigned char	  *Alligned_in_key_ref = NULL;
	unsigned char	  *Alligned_out_ckey = NULL;




	Alligned_rdrm_GetContentKey = (MTDRM_IPTV_drm_GetContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_drm_GetContentKey_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_GetContentKey)
	{
		printf("Allocate Alligned_rdrm_GetContentKey failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_GetContentKey = (MTDRM_TZIF_IPTVES_drm_GetContentKey_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_drm_GetContentKey_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_GetContentKey)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_GetContentKey failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_GetContentKey = (MTDRM_IPTV_drm_GetContentKey_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetContentKey, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_GetContentKey, Alligned_rdrm_GetContentKey, u4Len);


	/* get user space data: */
	in_key_ref	   = Alligned_rdrm_GetContentKey->in_key_ref;
	in_key_ref_len = Alligned_rdrm_GetContentKey->in_key_ref_len;


	out_ckey	   = Alligned_rdrm_GetContentKey->out_ckey;
	out_ckey_len = Alligned_rdrm_GetContentKey->out_ckey_len;

	

	
	
	/* malloc aligned memory */
	Alligned_in_key_ref =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_key_ref_len, 32));/*kernel data*/
	Alligned_out_ckey =		 (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_ckey_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_in_key_ref || NULL == Alligned_out_ckey)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_key_ref, in_key_ref_len);
	COPY_FROM_USER_ARG_SIZE(in_key_ref, Alligned_in_key_ref, in_key_ref_len);
	
	
	/* set TZ IF parameters */

	Alligned_rIPTVES_TZIf_drm_GetContentKey->in_thid = Alligned_rdrm_GetContentKey->in_thid;
	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_GetContentKey->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_GetContentKey->in_key_ref= (VOID*)PHYSICAL((UINT32)Alligned_in_key_ref); 
	Alligned_rIPTVES_TZIf_drm_GetContentKey->in_key_ref_len = in_key_ref_len;

		
	Alligned_rIPTVES_TZIf_drm_GetContentKey->out_ckey= (VOID*)PHYSICAL((UINT32)Alligned_out_ckey); 
	Alligned_rIPTVES_TZIf_drm_GetContentKey->out_ckey_len = out_ckey_len;

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_GetContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetContentKey_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));
		TZ_NWD_BufferStart((UINT32)Alligned_out_ckey, TZ_CACHE_ALIGN(out_ckey_len));

		i4Ret = TZ_IPTVES_DRM_GetContentKey(Alligned_rIPTVES_TZIf_drm_GetContentKey,sizeof(MTDRM_TZIF_IPTVES_drm_GetContentKey_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_GetContentKey, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_drm_GetContentKey_T)));


		TZ_NWD_BufferEnd((UINT32)Alligned_in_key_ref, TZ_CACHE_ALIGN(in_key_ref_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_ckey, TZ_CACHE_ALIGN(out_ckey_len));		

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_GetContentKey->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_ckey, out_ckey_len);
		COPY_TO_USER_ARG_SIZE(out_ckey, Alligned_out_ckey, out_ckey_len);

		
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_GetContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_GetContentKey);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_GetContentKey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_GetContentKey);
	}

//	 free malloced aligned buffer

	if(NULL != Alligned_in_key_ref)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_key_ref);
	}

	if(NULL != Alligned_out_ckey)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_ckey);
	}
		

		
	return i4Ret;

}




/* nvs */


static MT_RESULT_T  _MTDRM_IPTVES_NVS_Init(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_nvs_Init_T *prnvs_Init;
	MTDRM_IPTV_nvs_Init_T *Alligned_rnvs_Init = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_nvs_Init_T *Alligned_rIPTVES_TZIf_nvs_Init = NULL;
	UINT32 u4Len;



	/* need to malloc aligned memory into TEE */
	const unsigned char 	  *in_ro     = NULL;
	int in_ro_len = 0;
	const unsigned char       *in_rw = NULL;
	int in_rw_len = 0;	


	unsigned char 	  *Alligned_in_ro = NULL;
	unsigned char     *Alligned_in_rw = NULL;




	Alligned_rnvs_Init = (MTDRM_IPTV_nvs_Init_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_nvs_Init_T), 32));/*kernel data*/
	
	if (!Alligned_rnvs_Init)
	{
		printf("Allocate Alligned_rnvs_Init failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_nvs_Init = (MTDRM_TZIF_IPTVES_nvs_Init_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_nvs_Init_T), 32));
	if (!Alligned_rIPTVES_TZIf_nvs_Init)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_nvs_Init failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prnvs_Init = (MTDRM_IPTV_nvs_Init_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prnvs_Init, u4Len);
	COPY_FROM_USER_ARG_SIZE(prnvs_Init, Alligned_rnvs_Init, u4Len);


	/* get user space data: */
	in_ro     = Alligned_rnvs_Init->in_ro;
	in_ro_len = Alligned_rnvs_Init->in_ro_len;
	
	in_rw     = Alligned_rnvs_Init->in_rw;
	in_rw_len = Alligned_rnvs_Init->in_rw_len;	
	

	

	/* malloc aligned memory */
    Alligned_in_ro = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_ro_len, 32));/*kernel data*/
    Alligned_in_rw = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(in_rw_len, 32));/*kernel data*/



	/* malloc error ?*/
	if(NULL == Alligned_in_ro || NULL == Alligned_in_rw)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_ro, in_ro_len);
    COPY_FROM_USER_ARG_SIZE(in_ro, Alligned_in_ro, in_ro_len);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_rw, in_rw_len);
    COPY_FROM_USER_ARG_SIZE(in_rw, Alligned_in_rw, in_rw_len);
	


	//printf("%s kknvs_ro_len [%d]\n", __FUNCTION__, in_ro_len);
	//printf("%s kknvs_rw_len [%d]\n", __FUNCTION__, in_rw_len);

#if 0

	if(0!=memcmp(Alligned_in_ro, nvs_test, in_ro_len))
	{
		printf("\t\n\n!!!!!! kk_nvs_ro init not ok \n");
		/* dump */
		
		mtdrm_hdcp_DumpHex("kknvs_ro", Alligned_in_ro, in_ro_len);

		i4Ret = -55;

		
		goto out;
	}
	else
	{
		//printf("\tkk_nvs_ro init  ok \n");
	}


	if(0!=memcmp(Alligned_in_rw, nvs_test, in_rw_len))
	{
		printf("\t\n\n!!!!!! kk_nvs_rw init not ok \n");
		
		
		mtdrm_hdcp_DumpHex("kknvs_rw", Alligned_in_rw, in_rw_len);

		i4Ret = -66;
				
		goto out;
	}
	else
	{
		//printf("\tkk_nvs_rw init  ok \n");
		
	}
#endif
	
	/* set TZ IF parameters */


	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_nvs_Init->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_nvs_Init->in_ro = (VOID*)PHYSICAL((UINT32)Alligned_in_ro); 
	Alligned_rIPTVES_TZIf_nvs_Init->in_rw = (VOID*)PHYSICAL((UINT32)Alligned_in_rw); 


	Alligned_rIPTVES_TZIf_nvs_Init->in_ro_len = in_ro_len;
	Alligned_rIPTVES_TZIf_nvs_Init->in_rw_len = in_rw_len;


 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rnvs_Init, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_nvs_Init_T)));

		TZ_NWD_BufferStart((UINT32)Alligned_in_ro, TZ_CACHE_ALIGN(in_ro_len));
		TZ_NWD_BufferStart((UINT32)Alligned_in_rw, TZ_CACHE_ALIGN(in_rw_len));






		i4Ret = TZ_IPTVES_NVS_MEM_Init(Alligned_rIPTVES_TZIf_nvs_Init,sizeof(MTDRM_TZIF_IPTVES_nvs_Init_T));
		
		TZ_NWD_BufferEnd((UINT32)Alligned_rnvs_Init, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_nvs_Init_T)));

		TZ_NWD_BufferEnd((UINT32)Alligned_in_ro, TZ_CACHE_ALIGN(in_ro_len));
		TZ_NWD_BufferEnd((UINT32)Alligned_in_rw, TZ_CACHE_ALIGN(in_rw_len));



		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_nvs_Init->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}

		//printf("\n\n%s i4ret[%d]\n\n", __FUNCTION__, i4Ret);

		/* need data out to user space ?*/
		
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prnvs_Init, u4Len);
		COPY_TO_USER_ARG_SIZE(prnvs_Init, Alligned_rnvs_Init, u4Len);
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rnvs_Init)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rnvs_Init);
	}

	if(NULL != Alligned_rIPTVES_TZIf_nvs_Init)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_nvs_Init);
	}

//   free malloced aligned buffer

	if(NULL != Alligned_in_ro)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_ro);
	}


	if(NULL != Alligned_in_rw)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_in_rw);
	}


	
	return i4Ret;


}






static MT_RESULT_T  _MTDRM_IPTVES_NVS_Fin(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_nvs_Fin_T *prnvs_Fin;
	MTDRM_IPTV_nvs_Fin_T *Alligned_rnvs_Fin = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_nvs_Fin_T *Alligned_rIPTVES_TZIf_nvs_Fin = NULL;
	UINT32 u4Len;


	/* need to malloc aligned memory into TEE */
	const unsigned char       *out_rw = NULL;
	int out_rw_len = 0;	
	
	unsigned char       *Alligned_out_rw = NULL;


	//printf("\n>>%s\n", __FUNCTION__);


	Alligned_rnvs_Fin = (MTDRM_IPTV_nvs_Fin_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_nvs_Fin_T), 32));/*kernel data*/
	
	if (!Alligned_rnvs_Fin)
	{
		printf("Allocate Alligned_rnvs_Fin failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_nvs_Fin = (MTDRM_TZIF_IPTVES_nvs_Fin_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_nvs_Fin_T), 32));
	if (!Alligned_rIPTVES_TZIf_nvs_Fin)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_nvs_Fin failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prnvs_Fin = (MTDRM_IPTV_nvs_Fin_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prnvs_Fin, u4Len);
	COPY_FROM_USER_ARG_SIZE(prnvs_Fin, Alligned_rnvs_Fin, u4Len);


	/* get user space data: */
	out_rw     =  Alligned_rnvs_Fin->out_rw;
	out_rw_len =  Alligned_rnvs_Fin->out_rw_len;	
	

	//printf("%s out_rw_len:%d\n", __FUNCTION__, out_rw_len);

	/* malloc aligned memory */
    Alligned_out_rw = (unsigned char *)VIRTUAL(BSP_AllocAlignedDmaMemory(out_rw_len, 32));/*kernel data*/


	/* malloc error ?*/
	if(NULL == Alligned_out_rw)
	{
		printf("%s malloc alligned buffer error!\n", __FUNCTION__);
		goto out;
	}


	/* need copy user buffer into kernel ?*/
    //USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_rw, out_rw_len);
    //COPY_FROM_USER_ARG_SIZE(out_rw, Alligned_out_rw, out_rw_len);
	
	
	/* set TZ IF parameters */


	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_nvs_Fin->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */
	Alligned_rIPTVES_TZIf_nvs_Fin->out_rw= (VOID*)PHYSICAL((UINT32)Alligned_out_rw); 

	Alligned_rIPTVES_TZIf_nvs_Fin->out_rw_len = out_rw_len;


 	//printf("\n%s out_rw_len(%d)\n", __FUNCTION__, out_rw_len);
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rnvs_Fin, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_nvs_Fin_T)));
		TZ_NWD_BufferStart((UINT32)Alligned_out_rw, TZ_CACHE_ALIGN(out_rw_len));


		i4Ret = TZ_IPTVES_NVS_MEM_Fin(Alligned_rIPTVES_TZIf_nvs_Fin,sizeof(MTDRM_TZIF_IPTVES_nvs_Fin_T));


		
		TZ_NWD_BufferEnd((UINT32)Alligned_rnvs_Fin, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_nvs_Fin_T)));
		TZ_NWD_BufferEnd((UINT32)Alligned_out_rw, TZ_CACHE_ALIGN(out_rw_len));


		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_nvs_Fin->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(Alligned_rnvs_Fin->out_rw, out_rw_len);
		COPY_TO_USER_ARG_SIZE(Alligned_rnvs_Fin->out_rw, Alligned_out_rw, out_rw_len);
#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rnvs_Fin)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rnvs_Fin);
	}

	if(NULL != Alligned_rIPTVES_TZIf_nvs_Fin)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_nvs_Fin);
	}


	if(NULL != Alligned_out_rw)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_out_rw);
	}

	
	return i4Ret;


}










static MT_RESULT_T  _MTDRM_IPTVES_NVS_GetRWLen(unsigned long arg)
{
	//arg: int *

	INT32 i4Ret = 0;
	
	int *usr_outRwlen = NULL;

	MTDRM_TZIF_IPTVES_nvs_GetRWLen_T *Alligned_rIPTVES_TZIf_nvs_GetRWLen = NULL;
	

	Alligned_rIPTVES_TZIf_nvs_GetRWLen = (MTDRM_TZIF_IPTVES_nvs_GetRWLen_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_nvs_GetRWLen_T), 32));
	if (!Alligned_rIPTVES_TZIf_nvs_GetRWLen)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_nvs_GetRWLen failed\n");
		return MTR_NOT_OK;
	}	 
	


    if (!access_ok(VERIFY_READ, (void __user *)arg,         
                   sizeof(int)))                            
    {                                                       
        printk("mtal_ioctl_atsc argument error\n");     
        return -1;                                          
    }



	
	COPY_FROM_USER_1ARG(arg, usr_outRwlen);

	
	/* set TZ IF parameters */

	
	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_nvs_GetRWLen->rRet = MTR_NOT_OK;  


	/* set the aligned buffer as the TZ IF parameters */


	Alligned_rIPTVES_TZIf_nvs_GetRWLen->out_rw_len = 0;


 
	
#ifdef CC_ENABLE_HDCP2
		//TZ_NWD_BufferStart((UINT32)Alligned_rnvs_Fin, TZ_CACHE_ALIGN(sizeof(int)));
		//i4Ret = TZ_IPTVES_SDI_VerifyServiceMetaData(Alligned_rIPTVES_TZIf_nvs_GetRWLen,sizeof(MTDRM_TZIF_IPTVES_nvs_GetRWLen_T));
		//TZ_NWD_BufferEnd((UINT32)Alligned_rnvs_Fin, TZ_CACHE_ALIGN(sizeof(int)));


		i4Ret = TZ_IPTVES_NVS_MEM_GetRWLen(Alligned_rIPTVES_TZIf_nvs_GetRWLen, sizeof(MTDRM_TZIF_IPTVES_nvs_GetRWLen_T));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_nvs_GetRWLen->rRet;
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
	
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(int));
		COPY_TO_USER_ARG_SIZE(arg, &(Alligned_rIPTVES_TZIf_nvs_GetRWLen->out_rw_len), sizeof(int));
#else
		i4Ret = MTR_NOT_OK;
#endif

out:


	if(NULL != Alligned_rIPTVES_TZIf_nvs_GetRWLen)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_nvs_GetRWLen);
	}

	
	return i4Ret;


}





static MT_RESULT_T  _MTDRM_IPTVES_time_SetTrustedTime(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_time_SetTrustedTime_T *prdrm_SetTrustedTime;
	MTDRM_IPTV_time_SetTrustedTime_T *Alligned_rdrm_SetTrustedTime = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_time_SetTrustedTime_T *Alligned_rIPTVES_TZIf_drm_SetTrustedTime = NULL;
	UINT32 u4Len;



	Alligned_rdrm_SetTrustedTime = (MTDRM_IPTV_time_SetTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_time_SetTrustedTime_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_SetTrustedTime)
	{
		printf("Allocate Alligned_rdrm_SetTrustedTime failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_SetTrustedTime = (MTDRM_TZIF_IPTVES_time_SetTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_time_SetTrustedTime_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_SetTrustedTime)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_SetTrustedTime failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_SetTrustedTime = (MTDRM_IPTV_time_SetTrustedTime_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_SetTrustedTime, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_SetTrustedTime, Alligned_rdrm_SetTrustedTime, u4Len);
	
	
	/* set TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_SetTrustedTime->in_tt =  Alligned_rdrm_SetTrustedTime->in_tt;

	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_SetTrustedTime->rRet = MTR_NOT_OK; 

	printf("\n %s: in_tt:%d\n", __FUNCTION__, Alligned_rIPTVES_TZIf_drm_SetTrustedTime->in_tt);


 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_SetTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_SetTrustedTime_T)));
		i4Ret = TZ_IPTVES_TIME_SetTrustedTime(Alligned_rIPTVES_TZIf_drm_SetTrustedTime,sizeof(MTDRM_TZIF_IPTVES_time_SetTrustedTime_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_SetTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_SetTrustedTime_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_SetTrustedTime->rRet;

			/* value needed to return from tee */			
			//out_num = Alligned_rIPTVES_TZIf_drm_SetTrustedTime->out_num;

			//printf("\n %s: out_num %d\n", __FUNCTION__, out_num);
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


		/* need data out to user space ?*/
		//USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_SetTrustedTime->out_num, sizeof(prdrm_SetTrustedTime->out_num));
		//COPY_TO_USER_ARG_SIZE(prdrm_SetTrustedTime->out_num, &out_num, sizeof(prdrm_SetTrustedTime->out_num));

#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_SetTrustedTime)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_SetTrustedTime);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_SetTrustedTime)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_SetTrustedTime);
	}


	
	return i4Ret;
		
	

}








static MT_RESULT_T  _MTDRM_IPTVES_time_GetTrustedTime(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_time_GetTrustedTime_T *prdrm_GetTrustedTime;
	MTDRM_IPTV_time_GetTrustedTime_T *Alligned_rdrm_GetTrustedTime = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_time_GetTrustedTime_T *Alligned_rIPTVES_TZIf_drm_GetTrustedTime = NULL;
	UINT32 u4Len;



	Alligned_rdrm_GetTrustedTime = (MTDRM_IPTV_time_GetTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_time_GetTrustedTime_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_GetTrustedTime)
	{
		printf("Allocate Alligned_rdrm_GetTrustedTime failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_GetTrustedTime = (MTDRM_TZIF_IPTVES_time_GetTrustedTime_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_time_GetTrustedTime_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_GetTrustedTime)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_GetTrustedTime failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_GetTrustedTime = (MTDRM_IPTV_time_GetTrustedTime_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetTrustedTime, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_GetTrustedTime, Alligned_rdrm_GetTrustedTime, u4Len);
	
	
	/* set TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_GetTrustedTime->out_tt =  0;

	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_GetTrustedTime->rRet = MTR_NOT_OK; 

	

 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_GetTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_GetTrustedTime_T)));
		i4Ret = TZ_IPTVES_TIME_GetTrustedTime(Alligned_rIPTVES_TZIf_drm_GetTrustedTime,sizeof(MTDRM_TZIF_IPTVES_time_GetTrustedTime_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_GetTrustedTime, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_GetTrustedTime_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_GetTrustedTime->rRet;

			/* value needed to return from tee */			
			//out_num = Alligned_rIPTVES_TZIf_drm_GetTrustedTime->out_num;

			//printf("\n %s: out_num %d\n", __FUNCTION__, out_num);
		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}
		printf("\n %s: out_tt:%d\n", __FUNCTION__, Alligned_rIPTVES_TZIf_drm_GetTrustedTime->out_tt);


		/* need data out to user space ?*/
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_GetTrustedTime->out_tt, sizeof(unsigned int));
		COPY_TO_USER_ARG_SIZE(prdrm_GetTrustedTime->out_tt, &(Alligned_rIPTVES_TZIf_drm_GetTrustedTime->out_tt), sizeof(unsigned int));

#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_GetTrustedTime)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_GetTrustedTime);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_GetTrustedTime)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_GetTrustedTime);
	}


	
	return i4Ret;
		
	

}






static MT_RESULT_T  _MTDRM_IPTVES_time_Cleanup(unsigned long arg)
{
	INT32 i4Ret = 0;
	MTAL_IOCTL_2ARG_T rArg;
	MTDRM_IPTV_time_Cleanup_T *prdrm_time_Cleanup;
	MTDRM_IPTV_time_Cleanup_T *Alligned_rdrm_time_Cleanup = NULL;/*kernel data*/
	MTDRM_TZIF_IPTVES_time_Cleanup_T *Alligned_rIPTVES_TZIf_drm_time_Cleanup = NULL;
	UINT32 u4Len;



	Alligned_rdrm_time_Cleanup = (MTDRM_IPTV_time_Cleanup_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_IPTV_time_Cleanup_T), 32));/*kernel data*/
	
	if (!Alligned_rdrm_time_Cleanup)
	{
		printf("Allocate Alligned_rdrm_time_Cleanup failed\n");
		goto out;		
		return MTR_NOT_OK;
	}
	Alligned_rIPTVES_TZIf_drm_time_Cleanup = (MTDRM_TZIF_IPTVES_time_Cleanup_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(MTDRM_TZIF_IPTVES_time_Cleanup_T), 32));
	if (!Alligned_rIPTVES_TZIf_drm_time_Cleanup)
	{
		goto out;			
		printf("Allocate Alligned_rIPTVES_TZIf_drm_time_Cleanup failed\n");
		return MTR_NOT_OK;
	}	 
	
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	prdrm_time_Cleanup = (MTDRM_IPTV_time_Cleanup_T*)rArg.ai4Arg[0];
	u4Len = (UINT32)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prdrm_time_Cleanup, u4Len);
	COPY_FROM_USER_ARG_SIZE(prdrm_time_Cleanup, Alligned_rdrm_time_Cleanup, u4Len);
	
	
	/* set TZ IF parameters */
	Alligned_rIPTVES_TZIf_drm_time_Cleanup->in_type =  Alligned_rdrm_time_Cleanup->in_type;

	/* ret value: default NOT_OK*/
	Alligned_rIPTVES_TZIf_drm_time_Cleanup->rRet = MTR_NOT_OK; 

	printf("\n %s: in_type:%d\n", __FUNCTION__, Alligned_rIPTVES_TZIf_drm_time_Cleanup->in_type);


 
	
#ifdef CC_ENABLE_HDCP2
		TZ_NWD_BufferStart((UINT32)Alligned_rdrm_time_Cleanup, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_Cleanup_T)));
		i4Ret = TZ_IPTVES_TIME_Cleanup(Alligned_rIPTVES_TZIf_drm_time_Cleanup,sizeof(MTDRM_TZIF_IPTVES_time_Cleanup_T));
		TZ_NWD_BufferEnd((UINT32)Alligned_rdrm_time_Cleanup, TZ_CACHE_ALIGN(sizeof(MTDRM_IPTV_time_Cleanup_T)));

		if (i4Ret != 0)
		{
			i4Ret = Alligned_rIPTVES_TZIf_drm_time_Cleanup->rRet;

		}
		else
		{
			i4Ret = MTR_NOT_OK;
		}


#else
		i4Ret = MTR_NOT_OK;
#endif

out:

	if(NULL != Alligned_rdrm_time_Cleanup)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rdrm_time_Cleanup);
	}

	if(NULL != Alligned_rIPTVES_TZIf_drm_time_Cleanup)
	{
		BSP_FreeAlignedDmaMemory((UINT32)Alligned_rIPTVES_TZIf_drm_time_Cleanup);
	}


	
	return i4Ret;
		
	

}






extern int sign_snapshot(char* blkdev, int size, unsigned int snapoffset,unsigned int frag_unit_size,unsigned int num_of_frag);
static MT_RESULT_T  _MTDRM_SIGN_SNAPSHOT_IMAGE(unsigned long arg)
{
    MTDRM_SNAPSHOT_SIGN_T rSign;
    MTDRM_SNAPSHOT_SIGN_T* prSign;

    prSign = (MTDRM_SNAPSHOT_SIGN_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prSign,MTDRM_SNAPSHOT_SIGN_T);
    COPY_FROM_USER_ARG(prSign,rSign,MTDRM_SNAPSHOT_SIGN_T);

    prSign = &rSign;

    printf("(yjdbg) snapshot partition name: %s\n", rSign.block_dev);
    printf("(yjdbg) snapshot image size: 0x%x\n", rSign.img_size);
    printf("(yjdbg) snapshot signature offset: 0x%x\n", rSign.sig_offet);
    printf("(yjdbg) snapshot frag_unit_size: 0x%x\n", rSign.frag_unit_size);
    printf("(yjdbg) snapshot num_of_frag: 0x%x\n", rSign.num_of_frag);

    sign_snapshot(rSign.block_dev, rSign.img_size, rSign.sig_offet,rSign.frag_unit_size,rSign.num_of_frag);

    return MTR_OK;
}

int mtal_ioctl_mtdrm(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int result = MTR_OK;

    if ( hMtDrmSemaHandle == (HANDLE_T)NULL )
    {
        if ( x_sema_create(&hMtDrmSemaHandle, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) != 0 )
        {
            return MTR_NOT_OK;
        }
    }

    x_sema_lock(hMtDrmSemaHandle, X_SEMA_OPTION_WAIT);



    switch (cmd)
    {
        case MTAL_IO_DRM_HDCP2_SET_ENC_LC128:

        	 result = _MTDRM_HDCP2_SetEncLc128(arg);
            break;
            
        case MTAL_IO_DRM_HDCP2_SET_ENC_KEY:

        	 result = _MTDRM_HDCP2_SetEncKey(arg);
            break;

        case MTAL_IO_DRM_HDCP2_GETCERTINFO:

        	 result = _MTDRM_HDCP2_GetCertInfo(arg);
            break;

        case MTAL_IO_DRM_HDCP2_DECRYPT_RSAESOAEP:

        	 result = _MTDRM_HDCP2_DecryptRSAESOAEP(arg);
            break;

        case MTAL_IO_DRM_HDCP2_KDKEYDERIVATION:

        	 result = _MTDRM_HDCP2_KdKeyDerivation(arg);
            break;

        case MTAL_IO_DRM_HDCP2_COMPUTE_HPRIME:

        	 result = _MTDRM_HDCP2_ComputeHprime(arg);
            break;

        case MTAL_IO_DRM_HDCP2_COMPUTE_LPRIME:


        	 result = _MTDRM_HDCP2_ComputeLprime(arg);
            break;

        case MTAL_IO_DRM_HDCP2_COMPUTE_KH:

        	 result = _MTDRM_HDCP2_ComputeKh(arg);
            break;

        case MTAL_IO_DRM_HDCP2_ENCRYPT_KM:


        	 result = _MTDRM_HDCP2_EncryptKmUsingKh(arg);
            break;

        case MTAL_IO_DRM_HDCP2_DECRYPT_KM:

        	 result = _MTDRM_HDCP2_DecryptKmUsingKh(arg);
            break;

        case MTAL_IO_DRM_HDCP2_DECRYPT_EKS:

        	 result = _MTDRM_HDCP2_DecryptEKs(arg);
            break;

        case MTAL_IO_DRM_HDCP2_KSLC128XOR:

        	 result = _MTDRM_HDCP2_KsXorLC128(arg);
            break;

        case MTAL_IO_DRM_HDCP2_ENCRYPT_PES:

        	 result = _MTDRM_HDCP2_DataEncrypt(arg);
            break;

        case MTAL_IO_DRM_HDCP2_DECRYPT_PES:

        	 result = _MTDRM_HDCP2_DataDecrypt(arg);
            break;
        case MTAL_IO_DRM_HDCP2_SET_RIV_PID:

             result = _MTDRM_HDCP2_SetRiv_Pid(arg);
            break;
        case MTAL_IO_DRM_HDCP2_PRESET_KS_LC128:

             result = _MTDRM_HDCP2_PresetKsLC128(arg);
            break; 
        case MTAL_IO_DRM_HDCP2_GENERATE_KM:

             result = _MTDRM_HDCP2_Generate_Km(arg);
            break;            
        case MTAL_IO_DRM_HDCP2_GENERATE_KS:

             result = _MTDRM_HDCP2_Generate_Ks(arg);
            break; 
        case MTAL_IO_DRM_HDCP2_SET_ENCRYPTEDKM:

             result = _MTDRM_HDCP2_SetEncKm(arg);
            break; 
        case MTAL_IO_DRM_HDCP2_GET_ENCRYPTEDKM:

             result = _MTDRM_HDCP2_GetEncKm(arg);
            break; 
        case MTAL_IO_DRM_SIGN_SNAPSHOT_IMG:

        	 result = _MTDRM_SIGN_SNAPSHOT_IMAGE(arg);
            break;

/* for HDCP 2.2*/

        case MTAL_IO_DRM_HDCP2_2_KDKEYDERIVATION:

        	 result = _MTDRM_HDCP2_2_KdKeyDerivation(arg);
            break;

        case MTAL_IO_DRM_HDCP2_2_COMPUTE_HPRIME:

        	 result = _MTDRM_HDCP2_2_ComputeHprime(arg);
            break;

        case MTAL_IO_DRM_HDCP2_2_COMPUTE_LPRIME:

        	 result = _MTDRM_HDCP2_2_ComputeLprime(arg);
            break;


/* for HDCP ALIGN */
        case MTAL_IO_DRM_HDCP2_SET_ENC_KEY_SET_INTO_TZ:
			printf("MTAL_IO_DRM_HDCP2_SET_ENC_KEY_SET_INTO_TZ \n");
	       	result = _MTDRM_HDCP2_Set_EncKeySet_into_TZ(arg);
            break;

/* for HDCP TX */
		case MTAL_IO_DRM_HDCP2_RetrvEkh_km:
			printf("MTAL_IO_DRM_HDCP2_RetrvEkh_km \n");
	       	result = _MTDRM_HDCP2_RetrvEkh_km(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_EncryptRSAESOAEP:
			printf("MTAL_IO_DRM_HDCP2_EncryptRSAESOAEP \n");
	       	//result = _MTDRM_HDCP2_EncryptRSAESOAEP(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_PairingInfo:
			printf("MTAL_IO_DRM_HDCP2_PairingInfo \n");
	       	result = _MTDRM_HDCP2_SavePairingInfo(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_EncryptKs:
			printf("MTAL_IO_DRM_HDCP2_EncryptKs \n");
	       	//result = _MTDRM_HDCP2_EncryptKs(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_CheckRxID:
			printf("MTAL_IO_DRM_HDCP2_CheckRxID \n");
	       	result = _MTDRM_HDCP2_CheckRxID(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_ComputeMprime:
			printf("MTAL_IO_DRM_HDCP2_ComputeMprime \n");
	       	result = _MTDRM_HDCP2_ComputeMprime(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_ComputeVprime:
			printf("MTAL_IO_DRM_HDCP2_ComputeVprime \n");
	       	result = _MTDRM_HDCP2_ComputeVprime(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_2_ComputeVprime:
			printf("MTAL_IO_DRM_HDCP2_2_ComputeVprime \n");
	       	result = _MTDRM_HDCP2_2_ComputeVprime(arg);			
			break;
			
		case MTAL_IO_DRM_HDCP2_VerifySignature:
			printf("MTAL_IO_DRM_HDCP2_VerifySignature \n");
	       	result = _MTDRM_HDCP2_VerifySignature(arg);			
			break;

        case MTAL_IO_DRM_HDCP2_GET_GENERATE_KM_FOR_TX:
			printf("MTAL_IO_DRM_HDCP2_GET_GENERATE_KM_FOR_TX \n");
            result = _MTDRM_HDCP2_GenerateKMforTx(arg);
            break; 
		
		case MTAL_IO_DRM_HDCP2_ResetPairingInfo:
			printf("MTAL_IO_DRM_HDCP2_ResetPairingInfo \n");
			result = _MTDRM_HDCP2_ResetPairingInfo(arg);
			break; 


		case MTAL_IO_DRM_HDCP2_Set_Test_Key:
			printf("MTAL_IO_DRM_HDCP2_Set_Test_Key \n");
            result = _MTDRM_HDCP2_UseTestKey_InKernel(arg);
            break; 

		/* for hdcp2.2 extra */
		case MTAL_IO_DRM_HDCP2_Set_TX_CAP:
			printf("MTAL_IO_DRM_HDCP2_Set_TX_CAP \n");
			result = _MTDRM_HDCP2_SetTxCap(arg);
			break; 
		case MTAL_IO_DRM_HDCP2_HMAC_SHA256:
			  result = _MTDRM_HDCP2_HMAC_SHA256(arg);
			 break;

		/* for android hdcp */
		
		case MTAL_IO_DRM_HDCP2_DataDecrypt:
			result = _MTDRM_HDCP2_SW_DataDecrypt(arg);
		break;


		case MTAL_IO_DRM_HDCP2_If_TZ_KEY:
			printf("MTAL_IO_DRM_HDCP2_If_TZ_KEY \n");
			result = _MTDRM_HDCP2_IF_TZ_KEY(arg);
			break; 

		case MTAL_IO_DRM_HDCP2_GETPDKey:

		 result = _MTDRM_HDCP2_GET_PDKey(arg);
		break;

		case MTAL_IO_DRM_HDCP2_SETPDKey:

		 result = _MTDRM_HDCP2_SET_PDKey(arg);
		break;


//Secure Buffer Alloc		 
		case  MTAL_IO_DRM_SECURE_ALLOC:
			result =  _MTDRM_SECURE_ALLOC(arg);
			break;
		case  MTAL_IO_DRM_SECURE_FREE:
			result =  _MTDRM_SECURE_FREE(arg);
			break;
		case  MTAL_IO_DRM_SECURE_DATAMOVE:
			result =  _MTDRM_SECURE_DATAMOVE(arg);
			break;
		case  MTAL_IO_DRM_SECURE_GETDATA:
			result =  _MTDRM_SECURE_GETDATA(arg);
			break;      
        case  MTAL_IO_DRM_SECURE_DATAMOVE_DONE:
            result =  _MTDRM_SECURE_CPB_DATAMOVE_DONE(arg);
            break;
        case  MTAL_IO_DRM_SECURE_GETDATA_INFO:
            result =  _MTDRM_SECURE_CPB_DATAMOVE_INFO(arg);
            break;                 
//RPMB            
        case MTAL_IO_DRM_RPMB_LOAD_MAC_KEY:
            result = _MTDRM_RPMB_LOAD_MAC_KEY(arg);
            break;
        case MTAL_IO_DRM_RPMB_GET_WCNT_REQ:
            result = _MTDRM_RPMB_GET_WCNT_REQ(arg);
            break;
        case MTAL_IO_DRM_RPMB_SET_WCNT_RESP:
            result = _MTDRM_RPMB_SET_WCNT_RESP(arg);
            break;               
//Widevine 
		case  MTAL_IO_DRM_WV_OEMCRYPTO_INITIALIZE:
			result = _MTDRM_WV_OEMCrypto_Initialize(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_TERMINATE:
			result = _MTDRM_WV_OEMCrypto_Terminate(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_OPENSESSION:
			result = _MTDRM_WV_OEMCrypto_OpenSession(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_CLOSESESSION:
			result = _MTDRM_WV_OEMCrypto_CloseSession(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERATEDERIVEDKEYS:
			result = _MTDRM_WV_OEMCrypto_GenerateDerivedKeys(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERATENONCE:
			result = _MTDRM_WV_OEMCrypto_GenerateNonce(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERATESIGNATURE:
			result = _MTDRM_WV_OEMCrypto_GenerateSignature(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_LOADKEYS:
			result = _MTDRM_WV_OEMCrypto_LoadKeys(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_REFRESHKEYS:
			result = _MTDRM_WV_OEMCrypto_RefreshKeys(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_SELECTKEY:
			result = _MTDRM_WV_OEMCrypto_SelectKey(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTCTR:
			result = _MTDRM_WV_OEMCrypto_DecryptCTR(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_INSTALLKEYBOX:
			result = _MTDRM_WV_OEMCrypto_InstallKeybox(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_ISKEYBOXVALID:
			result = _MTDRM_WV_OEMCrypto_IsKeyboxValid(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GETDEVICEID:
			result = _MTDRM_WV_OEMCrypto_GetDeviceID(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GETKEYDATA:
			result = _MTDRM_WV_OEMCrypto_GetKeyData(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GETRANDOM:
			result = _MTDRM_WV_OEMCrypto_GetRandom(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_WRAPKEYBOX:
			result = _MTDRM_WV_OEMCrypto_WrapKeybox(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_REWRAPDEVICERSAKEY:
			result = _MTDRM_WV_OEMCrypto_RewrapDeviceRSAKey(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_LOADDEVICERSAKEY:
			result = _MTDRM_WV_OEMCrypto_LoadDeviceRSAKey(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERATERSASIGNATURE:
			result = _MTDRM_WV_OEMCrypto_GenerateRSASignature(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_DERIVEKEYSFROMSESSIONKEY:
			result = _MTDRM_WV_OEMCrypto_DeriveKeysFromSessionKey(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_ENCRYPT:
			result = _MTDRM_WV_OEMCrypto_Generic_Encrypt(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_DECRYPT:
			result = _MTDRM_WV_OEMCrypto_Generic_Decrypt(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_SIGN:
			result = _MTDRM_WV_OEMCrypto_Generic_Sign(arg);
			break;
		case  MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_VERIFY:
			result = _MTDRM_WV_OEMCrypto_Generic_Verify(arg);
			break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_UPDATEUSAGETABLE:
            result = _MTDRM_WV_OEMCrypto_UpdateUsageTable(arg);
            break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_DEACTIVATEUSAGEENTRY:
            result = _MTDRM_WV_OEMCrypto_DeactivateUsageEntry(arg);
            break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_REPORTUSAGE:
            result = _MTDRM_WV_OEMCrypto_ReportUsage(arg);
            break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_DELETEUSAGEENTRY:
            result = _MTDRM_WV_OEMCrypto_DeleteUsageEntry(arg);
            break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_DELETEUSAGETABLE:
            result = _MTDRM_WV_OEMCrypto_DeleteUsageTable(arg);
            break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_SETENTITLEMENTKEY:
			result = _MTDRM_WV_OEMCrypto_SetEntitlementKey(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_DERIVECONTROLWORD:
			result = _MTDRM_WV_OEMCrypto_DeriveControlWord(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTVIDEO:
			result = _MTDRM_WV_OEMCrypto_DecryptVideo(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTAUDIO:
			result = _MTDRM_WV_OEMCrypto_DecryptAudio(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_GETUSAGETABLEWRITEREQUEST:
			result = _MTDRM_WV_OEMCrypto_GetUsageTableWriteRequest(arg);
			break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_VERIFYWRITERESPONSE:
			result = _MTDRM_WV_OEMCrypto_VerifyWriteResponse(arg);
			break;
        case MTAL_IO_DRM_WV_OEMCRYPTO_GETUSAGETABLEREADREQUEST:
            result = _MTDRM_WV_OEMCrypto_GetUsageTableReadRequest(arg);
            break;
		case MTAL_IO_DRM_WV_OEMCRYPTO_VERIFYREADRESPONSE:
			result = _MTDRM_WV_OEMCrypto_VerifyReadResponse(arg);
			break;                           
		/* for sony iptves */
		
		/* sac */
		case MTAL_IO_DRM_IPTVES_SAC_CONSTRUCT:
			 result = _MTDRM_IPTVES_SAC_CONSTRUCT(arg);
			break;

		case MTAL_IO_DRM_IPTVES_SAC_DESTRUCT:
			 result = _MTDRM_IPTVES_SAC_DESTRUCT(arg);
			break;

		case MTAL_IO_DRM_IPTVES_SAC_MAKEMSG:
			 result = _MTDRM_IPTVES_SAC_MAKEMSG(arg);
			break;
			
		case MTAL_IO_DRM_IPTVES_SAC_PROCESSMSG:
			 result = _MTDRM_IPTVES_SAC_PROCESSMSG(arg);
			break;			
			

		/* sdi */
		case  MTAL_IO_DRM_IPTVES_SDI_INIT:
			result = _MTDRM_IPTVES_SDI_INIT(arg);
			break;	
			
								   
		case  MTAL_IO_DRM_IPTVES_SDI_FIN:
			result = _MTDRM_IPTVES_SDI_FIN(arg);
			break;	 
			
													   
		case  MTAL_IO_DRM_IPTVES_SDI_CleanupNVS:
			result = _MTDRM_IPTVES_SDI_CleanupNVS(arg);
			break;	 
			
											 
		case  MTAL_IO_DRM_IPTVES_SDI_GetDeviceID:
			result = _MTDRM_IPTVES_SDI_GetDeviceID(arg);
			break;	  
			
											 
		case  MTAL_IO_DRM_IPTVES_SDI_VerifyServiceMetaData:
			result = _MTDRM_IPTVES_SDI_VerifyServiceMetaData(arg);
			break;	 
			
								
		case  MTAL_IO_DRM_IPTVES_SDI_JudgeCRLUpdate:
			result = _MTDRM_IPTVES_SDI_JudgeCRLUpdate(arg);
			break;	

			
		case  MTAL_IO_DRM_IPTVES_SDI_GetCRLDstPoint:
			result = _MTDRM_IPTVES_SDI_GetCRLDstPoint(arg);
			break;	 
			
									   
		case  MTAL_IO_DRM_IPTVES_SDI_SetCRL:
			result = _MTDRM_IPTVES_SDI_SetCRL(arg);
			break;	 
			
											   
		case  MTAL_IO_DRM_IPTVES_SDI_CheckTrustedTime:
			result = _MTDRM_IPTVES_SDI_CheckTrustedTime(arg);
			break;								 
		
		
		/* ecm */
		case  MTAL_IO_DRM_IPTVES_ECM_ExtractScrambleKey:
			result = _MTDRM_IPTVES_ECM_ExtractScrambleKey(arg);
			break;	
			
									   
		case  MTAL_IO_DRM_IPTVES_ECM_GetScrambleKey:
			result = _MTDRM_IPTVES_ECM_GetScrambleKey(arg);
			break;	 
			
										
															
		/* drm */
		case  MTAL_IO_DRM_IPTVES_DRM_ExtractContentKey:
			result = _MTDRM_IPTVES_DRM_ExtractContentKey(arg);
			break;	 
			
									
		case  MTAL_IO_DRM_IPTVES_DRM_DelContentKey:
			result = _MTDRM_IPTVES_DRM_DelContentKey(arg);
			break;	
			
											
		case  MTAL_IO_DRM_IPTVES_DRM_DelUncommittedKey:
			result = _MTDRM_IPTVES_DRM_DelUncommittedKey(arg);
			break;	
			
										
		case  MTAL_IO_DRM_IPTVES_DRM_GetKeyInfo:
			result = _MTDRM_IPTVES_DRM_GetKeyInfo(arg);
			break;	 
			
											 
		case  MTAL_IO_DRM_IPTVES_DRM_GetKeyNum:
			result = _MTDRM_IPTVES_DRM_GetKeyNum(arg);
			break;	
			
													
		case  MTAL_IO_DRM_IPTVES_DRM_GetKeyRefList:
			result = _MTDRM_IPTVES_DRM_GetKeyRefList(arg);
			break;	
			
												
		case  MTAL_IO_DRM_IPTVES_DRM_GetContentKey:
			result = _MTDRM_IPTVES_DRM_GetContentKey(arg);
			break;									

		
		 
		/* nvs */
		case MTAL_IO_DRM_IPTVES_NVS_Init:
			result = _MTDRM_IPTVES_NVS_Init(arg);
			break;



		case MTAL_IO_DRM_IPTVES_NVS_Fin:
			result = _MTDRM_IPTVES_NVS_Fin(arg);
			break;



		case MTAL_IO_DRM_IPTVES_NVS_GetRWLen:
			result = _MTDRM_IPTVES_NVS_GetRWLen(arg);
			break;


		case  MTAL_IO_DRM_IPTVES_time_SetTrustedTime:
			result = _MTDRM_IPTVES_time_SetTrustedTime(arg);
					 
			break;	

		case  MTAL_IO_DRM_IPTVES_time_GetTrustedTime:
			result = _MTDRM_IPTVES_time_GetTrustedTime(arg);
			break;	


		case  MTAL_IO_DRM_IPTVES_time_Cleanup:
			result = _MTDRM_IPTVES_time_Cleanup(arg);
					 
			break;	



        default:
			printf("mtal_ioctl_mtdrm cmd: unknown \n");
            result = MTR_NOT_OK;
            break;
    }

    x_sema_unlock(hMtDrmSemaHandle);

    return result;
}


