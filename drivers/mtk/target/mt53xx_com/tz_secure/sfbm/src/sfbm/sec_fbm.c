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
 * $RCSfile: sec_fbm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file sec_fbm.c
 *  Brief of file sec_fbm.c.
 *  calculate the secure fbm pool size
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_printf.h"
#include "fbm_drvif.h"


int SizeOfSecureFbm()
{
    int i;
    unsigned int min=0xffffffff;
	unsigned int mjc_start = 0,mjc_size = 0;
    FBM_POOL_T* prFbmPool = NULL;

    FBM_POOL_TYPE_T aeSecure_pool[] =
    {
    	FBM_POOL_TYPE_DRM,
        FBM_POOL_TYPE_DMX,
        FBM_POOL_TYPE_MPEG,
        FBM_POOL_TYPE_MJC_1,
        FBM_POOL_TYPE_SCPOS_MAIN,
        FBM_POOL_TYPE_SCPOS_PIP,
        FBM_POOL_TYPE_MDDI_1,
        FBM_POOL_TYPE_MDDI_PIP,
        FBM_POOL_TYPE_NR_1,
        FBM_POOL_TYPE_B2R,
        FBM_POOL_TYPE_OD,
        FBM_POOL_TYPE_DMX2,
        FBM_POOL_TYPE_MPEG2,
        FBM_POOL_TYPE_MPEG3,
        FBM_POOL_TYPE_DMX3,
        FBM_POOL_TYPE_TVE,
    };

    prFbmPool = FBM_GetPoolInfoAuto(FBM_POOL_TYPE_TDC, NULL); 
    for(i=0;i<sizeof(aeSecure_pool)/sizeof(FBM_POOL_TYPE_T);i++)
    { 
        prFbmPool = FBM_GetPoolInfo((UINT8)aeSecure_pool[i]);
        if(prFbmPool->u4Size != 0 && prFbmPool->u4Addr < min)
            min = prFbmPool->u4Addr;
    }

	prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_MJC_1);
	mjc_start = prFbmPool->u4Addr;
	mjc_size = prFbmPool->u4Size;
	
    prFbmPool=FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_TOTAL);
     
    Printf("TOTAL=0x%x, minaddr=0x%x\n", prFbmPool->u4Addr + prFbmPool->u4Size, min);

	if(mjc_start<prFbmPool->u4Addr)
		mjc_size = 0;
	Printf("MJC size=0x%x\n",mjc_size);
    return prFbmPool->u4Addr + prFbmPool->u4Size - min;
}

int SizeOfSecureDrm()
{
    FBM_POOL_T* prFbmPool = NULL;

    prFbmPool = FBM_GetPoolInfoAuto(FBM_POOL_TYPE_DRM, NULL); 
     
    Printf("secure drm addr size=0x%x\n", prFbmPool->u4Addr);
    Printf("secure drm size=0x%x\n", prFbmPool->u4Size);

    return 0;
}
