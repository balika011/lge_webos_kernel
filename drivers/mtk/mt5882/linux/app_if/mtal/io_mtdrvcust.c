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
 * $RCSfile: io_mtdrvcust.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtdrvcust.c
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drvcust_if.h"
#include "x_chip_id.h"

#include "u_common.h"
#include "x_rm.h"
#include "u_rm_dev_types.h"
#include "u_drv_cust.h"
#include "x_util.h"
#include "c_model.h"

#include "mtdrvcust.h"
#include <linux/mtal_ioctl.h>


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                           

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }  
    
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                           

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }  


#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                           

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }  

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust valid argument error\n");     \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument copy error\n");      \
        return MTR_ERR_INV;                                          \
    } 

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel, size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG_KX(ArgUsr, ArgKernel, size)              \
    if (copy_to_user((void __user *)ArgUsr, ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdrvcust argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/* MTDRVCUST_InitQuery
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_InitQuery(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    QUERY_TYPE_T eQryType;
    UINT32 u4Data, *pu4Data;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eQryType = (QUERY_TYPE_T)rArg.ai4Arg[0];
    pu4Data = (UINT32*)rArg.ai4Arg[1];
    
    if(DRVCUST_InitQuery(eQryType, &u4Data) != 0)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Data, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Data, u4Data, sizeof(UINT32));
        
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/* MTDRVCUST_InitGet
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_InitGet(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    QUERY_TYPE_T eQryType;
    UINT32 u4Data, *pu4Data;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eQryType = (QUERY_TYPE_T)rArg.ai4Arg[0];
    pu4Data = (UINT32*)rArg.ai4Arg[1];
    
    u4Data = DRVCUST_InitGet(eQryType);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Data, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Data, u4Data, sizeof(UINT32));
        
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/* MTDRVCUST_OptQuery
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_OptQuery(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    QUERY_TYPE_T eQryType;
    UINT32 u4Data, *pu4Data;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eQryType = (QUERY_TYPE_T)rArg.ai4Arg[0];
    pu4Data = (UINT32*)rArg.ai4Arg[1];
    
    if(DRVCUST_OptQuery(eQryType, &u4Data) != 0)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Data, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Data, u4Data, sizeof(UINT32));
        
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/* MTDRVCUST_OptGet
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_OptGet(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    QUERY_TYPE_T eQryType;
    UINT32 u4Data, *pu4Data;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eQryType = (QUERY_TYPE_T)rArg.ai4Arg[0];
    pu4Data = (UINT32*)rArg.ai4Arg[1];
    
    u4Data = DRVCUST_OptGet(eQryType);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Data, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Data, u4Data, sizeof(UINT32));
        
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/* _MTDRVCUST_IsSupport
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_IsSupport(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTCHIP_SPEC_T eSpec;
    BOOL fgSupport, *pfgSupport;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eSpec = (MTCHIP_SPEC_T)rArg.ai4Arg[0];
    pfgSupport = (BOOL*)rArg.ai4Arg[1];
    
    switch(eSpec)
    {
    case MTCHIP_SPEC_MHEG5:
        fgSupport = IS_MHEG5Support();
        break;
    case MTCHIP_SPEC_CIPLUS:
        fgSupport = IS_SupportCIPlus();
        break;
    case MTCHIP_SPEC_TCON:
        fgSupport = IS_SupportTCON();
        break;
    case MTCHIP_SPEC_CHINATV:
        fgSupport = IS_SupportCHINATV();
        break;
    case MTCHIP_SPEC_MHEGHD:
        fgSupport = IS_SupportMHEGHD();
        break;
    case MTCHIP_SPEC_OPERA:
        fgSupport = IS_SupportOPERA();
        break;
    case MTCHIP_SPEC_JVM:
        fgSupport = IS_SupportJVM();
        break;
    case MTCHIP_SPEC_ARIB:
        fgSupport = IS_SupportARIB();
        break;
    case MTCHIP_SPEC_GINGA:
        fgSupport = IS_SupportGINGA();
        break;
    case MTCHIP_SPEC_MHP:
        fgSupport = IS_SupportMHP();
        break;
    case MTCHIP_SPEC_DMBT:
        fgSupport = IS_SupportDMBT();
        break;
    case MTCHIP_SPEC_ATSC:
        fgSupport = IS_SupportATSC();
        break;
    case MTCHIP_SPEC_BRAZIL:
        fgSupport = IS_SupportBRAZIL();
        break;
    case MTCHIP_SPEC_DVB:
        fgSupport = IS_SupportDVB();
        break;
    case MTCHIP_SPEC_ANDROID:
        fgSupport = IS_SupportAndroid();
        break;
    case MTCHIP_SPEC_IPTV:
        fgSupport = IS_SupportIPTV();
        break;
    case MTCHIP_SPEC_PLAYRDY:
        fgSupport = IS_SupportPlayRDY();
        break;                
    case MTCHIP_SPEC_MSS:
        fgSupport = IS_SupportMSS();
        break;                
    case MTCHIP_SPEC_HBBTV:
        fgSupport = IS_SupportHBBTV();
        break;
	case MTCHIP_SPEC_IPTV_DLNA:
#ifdef CC_MT5565
        fgSupport = IS_SupportIPTV_DLNA();
#else
        fgSupport = FALSE;
#endif
        break;	
    default:
        fgSupport = FALSE;
        break;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pfgSupport, sizeof(BOOL));
    COPY_TO_USER_ARG_K(pfgSupport, fgSupport, sizeof(BOOL));

    return MTR_OK;
}
//-----------------------------------------------------------------------------
/* MTDRVCUST_GetCountry
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_GetCountry(unsigned long arg)
{
    INT32 rArg;
	UINT32 u4Country;
	UINT32 *pu4Country;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

    pu4Country = (UINT32 *)rArg;

    u4Country =  GetTargetCountry(); 

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Country, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Country, u4Country, sizeof(UINT32));
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/* _MTDRVCUST_GetCountryMask
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_GetCountryMask(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	UINT32 u4Country, u4Mask;
	UINT32 *pu4Country;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4Mask = (UINT32)rArg.ai4Arg[0];
    pu4Country = (UINT32 *)rArg.ai4Arg[1];

    u4Country =  GetTargetCountryMask(u4Mask); 

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu4Country, sizeof(UINT32));
    COPY_TO_USER_ARG_K(pu4Country, u4Country, sizeof(UINT32));

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/* MTDRVCUST_SetCountry
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_SetCountry(unsigned long arg)
{
    INT32 rArg;
	UINT32 u4Country;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

    u4Country = (UINT32)rArg;

	switch(u4Country)
	{
		case 0://SDEC_COUNTRY_KR
			SetTargetCountry(COUNTRY_US);
		break;
		case 1://SDEC_COUNTRY_US
			SetTargetCountry(COUNTRY_US);
		break;			
		case 2://SDEC_COUNTRY_EU
			SetTargetCountry(COUNTRY_EU);
		break;
		case 3://SDEC_COUNTRY_CN
			SetTargetCountry(COUNTRY_EU);
		break;
		case 4://SDEC_COUNTRY_MA
			SetTargetCountry(COUNTRY_EU);
		break;
		case 5://SDEC_COUNTRY_JP
			SetTargetCountry(COUNTRY_JP);
		break;
		case 6://SDEC_COUNTRY_COLUMBIA
			SetTargetCountry(COUNTRY_TW);
		break;
		default:
			SetTargetCountry(COUNTRY_EU);
		break;
	}
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_tuner_set
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_tuner_set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    DRV_CUSTOM_TUNER_TYPE_T         e_tuner_type;
    DRV_CUSTOM_TUNER_SET_TYPE_T     e_tuner_set_type;
    VOID*                           pv_set_info;
    SIZE_T                          z_size;
    VOID*                           pv_kernel_set_info = NULL;
    INT32 i4_ret;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_tuner_type = (DRV_CUSTOM_TUNER_TYPE_T)rArg.ai4Arg[0];
    e_tuner_set_type = (DRV_CUSTOM_TUNER_SET_TYPE_T)rArg.ai4Arg[1];
    pv_set_info = (VOID*)rArg.ai4Arg[2];
    z_size = (SIZE_T)rArg.ai4Arg[3];

    /* only do rm_copy_from_user() when the argument is a pointer! */
    if (DRV_CUSTOM_ARG_USE_REF_OFFSET <= e_tuner_type)
    {
        pv_kernel_set_info = (VOID*)x_mem_alloc(z_size);
        if(pv_kernel_set_info == NULL)
        {
            return MTR_NOT_OK;
        }

        if (!access_ok(VERIFY_READ, (void __user *)pv_set_info, z_size))
        {
        printk("mtal_ioctl_mtdrvcust valid argument error\n");
        x_mem_free(pv_kernel_set_info);
        return MTR_ERR_INV;
        }   
        COPY_FROM_USER_ARG_K(pv_set_info, pv_kernel_set_info, z_size);
    
        i4_ret = d_custom_tuner_set(e_tuner_type, e_tuner_set_type, pv_kernel_set_info, z_size);
    
        x_mem_free(pv_kernel_set_info);
    }
    else
    {
        i4_ret = d_custom_tuner_set(
                                e_tuner_type,
                                e_tuner_set_type,
                                pv_set_info,
                                z_size);
    }

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_tuner_get
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_tuner_get (unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_TUNER_TYPE_T e_tuner_type;
    DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type;
    VOID*                 pv_get_info;
    VOID*                 pv_kernel_get_info = NULL;
    SIZE_T                *pz_size;
    SIZE_T                z_size;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_tuner_type = (DRV_CUSTOM_TUNER_TYPE_T)rArg.ai4Arg[0];
    e_tuner_get_type = (DRV_CUSTOM_TUNER_GET_TYPE_T)rArg.ai4Arg[1];
    pv_get_info = (VOID*)rArg.ai4Arg[2];
    pz_size = (SIZE_T*)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_FROM_USER_ARG_K(pz_size, &z_size, sizeof(SIZE_T));

    pv_kernel_get_info = (VOID*)x_mem_alloc(z_size);
    if(pv_kernel_get_info == NULL)
    {
        return MTR_NOT_OK;
    }    

    // Maybe not necessary
    if (!access_ok(VERIFY_READ, (void __user *)pv_get_info, z_size))
    {
        printk("mtal_ioctl_mtdrvcust valid argument error\n");
        x_mem_free(pv_kernel_get_info);
        return MTR_ERR_INV;
    }       
    COPY_FROM_USER_ARG_K(pv_get_info, pv_kernel_get_info, z_size);

    i4_ret = d_custom_tuner_get(e_tuner_type, e_tuner_get_type, pv_kernel_get_info, &z_size);   

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_TO_USER_ARG_K(pz_size, z_size, sizeof(SIZE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_TO_USER_ARG_KX(pv_get_info, pv_kernel_get_info, z_size);

    x_mem_free(pv_kernel_get_info);

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_vid_set
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_vid_set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_VID_TYPE_T e_vid_type;
    VOID*                 pv_set_info;
    VOID*                 pv_kernel_set_info = NULL;
    SIZE_T                z_size;
    BOOL b_store;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_vid_type = (DRV_CUSTOM_VID_TYPE_T)rArg.ai4Arg[0];
    pv_set_info = (VOID*)rArg.ai4Arg[1];
    z_size = (SIZE_T)rArg.ai4Arg[2];
    b_store = (BOOL)rArg.ai4Arg[3];

	
    /* only do rm_copy_from_user() when the argument is a pointer! */
    if (DRV_CUSTOM_ARG_USE_REF_OFFSET <= e_vid_type)
    {
        pv_kernel_set_info = (VOID*)x_mem_alloc(z_size);
        if(pv_kernel_set_info == NULL)
        {
            return MTR_NOT_OK;
        }    

        if (!access_ok(VERIFY_READ, (void __user *)pv_set_info, z_size))
        {
            printk("mtal_ioctl_mtdrvcust valid argument error\n");
            x_mem_free(pv_kernel_set_info);
            return MTR_ERR_INV;
        }       
        COPY_FROM_USER_ARG_K(pv_set_info, pv_kernel_set_info, z_size);

        i4_ret = d_custom_vid_set(e_vid_type, pv_kernel_set_info, z_size, b_store);
    
        x_mem_free(pv_kernel_set_info);
    }
    else
    {
        i4_ret = d_custom_vid_set(
                                e_vid_type,
                                pv_set_info,
                                z_size,
                                b_store);
    }

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_vid_get
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_vid_get(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_VID_TYPE_T e_vid_type;
    DRV_CUSTOM_VID_INP_T e_vid_inp;
    VOID*                 pv_get_info;
    VOID*                 pv_kernel_get_info = NULL;
    SIZE_T                *pz_size;
    SIZE_T                z_size;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_vid_type = (DRV_CUSTOM_VID_TYPE_T)rArg.ai4Arg[0];
    e_vid_inp = (DRV_CUSTOM_VID_INP_T)rArg.ai4Arg[1];
    pv_get_info = (VOID*)rArg.ai4Arg[2];
    pz_size = (SIZE_T*)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_FROM_USER_ARG_K(pz_size, &z_size, sizeof(SIZE_T));

    pv_kernel_get_info = (VOID*)x_mem_alloc(z_size);
    if(pv_kernel_get_info == NULL)
    {
        return MTR_NOT_OK;
    }
    
    // Maybe not necessary
    if (!access_ok(VERIFY_READ, (void __user *)pv_get_info, z_size))
    {
        printk("mtal_ioctl_mtdrvcust valid argument error\n");
        x_mem_free(pv_kernel_get_info);
        return MTR_ERR_INV;
    }
    COPY_FROM_USER_ARG_K(pv_get_info, pv_kernel_get_info, z_size);

    i4_ret = d_custom_vid_get(e_vid_type, e_vid_inp, pv_kernel_get_info, &z_size);   

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_TO_USER_ARG_K(pz_size, z_size, sizeof(SIZE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_TO_USER_ARG_KX(pv_get_info, pv_kernel_get_info, z_size);

    x_mem_free(pv_kernel_get_info);

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_extmjc_set
 */
//-----------------------------------------------------------------------------
#ifdef DRV_SUPPORT_EXTMJC
static MT_RESULT_T _MTDRVCUST_dcustom_extmjc_set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_EXTMJC_TYPE_T e_extmjc_type;
    VOID*                 pv_set_info;
    VOID*                 pv_kernel_set_info = NULL;
    SIZE_T                z_size;
    BOOL b_store;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_extmjc_type = (DRV_CUSTOM_EXTMJC_TYPE_T)rArg.ai4Arg[0];
    pv_set_info = (VOID*)rArg.ai4Arg[1];
    z_size = (SIZE_T)rArg.ai4Arg[2];
    b_store = (BOOL)rArg.ai4Arg[3];

    /* only do rm_copy_from_user() when the argument is a pointer! */
    if (DRV_CUSTOM_ARG_USE_REF_OFFSET <= e_extmjc_type)
    {
        pv_kernel_set_info = (VOID*)x_mem_alloc(z_size);
        if(pv_kernel_set_info == NULL)
        {
            return MTR_NOT_OK;
        }   

        USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_set_info, z_size);
        COPY_FROM_USER_ARG_K(pv_set_info, pv_kernel_set_info, z_size);

        i4_ret = d_custom_extmjc_set(e_extmjc_type, pv_kernel_set_info, z_size, b_store);
    
        x_mem_free(pv_kernel_set_info);
    }
    else
    {
        i4_ret = d_custom_extmjc_set(
                                e_extmjc_type,
                                pv_set_info,
                                z_size,
                                b_store);
    }

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_extmjc_get
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_extmjc_get (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_EXTMJC_TYPE_T e_extmjc_type;
    VOID*                 pv_get_info;
    VOID*                 pv_kernel_get_info = NULL;
    SIZE_T                *pz_size;
    SIZE_T                z_size;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    e_extmjc_type = (DRV_CUSTOM_EXTMJC_TYPE_T)rArg.ai4Arg[0];
    pv_get_info = (VOID*)rArg.ai4Arg[1];
    pz_size = (SIZE_T*)rArg.ai4Arg[2];

    // Maybe not necessary
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_FROM_USER_ARG_K(pz_size, &z_size, sizeof(SIZE_T));

    pv_kernel_get_info = (VOID*)x_mem_alloc(z_size);
    if(pv_kernel_get_info == NULL)
    {
        return MTR_NOT_OK;
    }
    
    // Maybe not necessary
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_FROM_USER_ARG_K(pv_get_info, pv_kernel_get_info, z_size);

    i4_ret = d_custom_extmjc_get(e_extmjc_type, pv_kernel_get_info, &z_size);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_TO_USER_ARG_K(pz_size, z_size, sizeof(SIZE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_TO_USER_ARG_KX(pv_get_info, pv_kernel_get_info, z_size);

    x_mem_free(pv_kernel_get_info);

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}
#endif // DRV_SUPPORT_EXTMJC

//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_vid_get_gamma
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_vid_get_gamma(unsigned long arg)
{
    INT32 i4_ret;    
    INT32 rArg;
    GAMMA_INFO_T rGammaTbl, *prGammaTbl;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

    prGammaTbl = (GAMMA_INFO_T*)rArg;

    i4_ret = d_custom_vid_get_gamma(&rGammaTbl);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(prGammaTbl, sizeof(GAMMA_INFO_T));
    COPY_TO_USER_ARG_K(prGammaTbl, rGammaTbl, sizeof(GAMMA_INFO_T));

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_vid_get_min_max
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_vid_get_min_max(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_VID_TYPE_T e_vid_type;
    DRV_CUSTOM_VID_INP_T e_vid_inp;
    UINT16 u2Min, *pu2Min;
    UINT16 u2Max, *pu2Max;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_vid_type = (DRV_CUSTOM_VID_TYPE_T)rArg.ai4Arg[0];
    e_vid_inp = (DRV_CUSTOM_VID_INP_T)rArg.ai4Arg[1];
    pu2Min = (UINT16*)rArg.ai4Arg[2];
    pu2Max = (UINT16*)rArg.ai4Arg[3];

    i4_ret = d_custom_vid_get_min_max(e_vid_type, e_vid_inp, &u2Min, &u2Max);   
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu2Min, sizeof(UINT16));
    COPY_TO_USER_ARG_K(pu2Min, u2Min, sizeof(UINT16));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu2Max, sizeof(UINT16));
    COPY_TO_USER_ARG_K(pu2Max, u2Max, sizeof(UINT16));

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_SetDrvInit
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_SetDrvInit(unsigned long arg)
{
    INT32 rArg;
    UINT32 u4Flag;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

    u4Flag = (UINT32)rArg;

    DRVCUST_SetDrvInit(u4Flag);
    
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_misc_set
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_misc_set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    DRV_CUSTOM_MISC_TYPE_T         e_misc_type;
    VOID*                           pv_set_info;
    SIZE_T                          z_size;
    VOID*                           pv_kernel_set_info = NULL;
    BOOL b_store;
    INT32 i4_ret;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_misc_type = (DRV_CUSTOM_MISC_TYPE_T)rArg.ai4Arg[0];
    pv_set_info = (VOID*)rArg.ai4Arg[1];
    z_size = (SIZE_T)rArg.ai4Arg[2];
    b_store = (BOOL)rArg.ai4Arg[3];

	
    /* only do rm_copy_from_user() when the argument is a pointer! */
    if (DRV_CUSTOM_ARG_USE_REF_OFFSET <= e_misc_type)
    {
        pv_kernel_set_info = (VOID*)x_mem_alloc(z_size);
        if(pv_kernel_set_info == NULL)
        {
            return MTR_NOT_OK;
        }

        if (!access_ok(VERIFY_READ, (void __user *)pv_set_info, z_size))
        {
            printk("mtal_ioctl_mtdrvcust valid argument error\n");
            x_mem_free(pv_kernel_set_info);
            return MTR_ERR_INV;
        }
        COPY_FROM_USER_ARG_K(pv_set_info, pv_kernel_set_info, z_size);
    
        i4_ret = d_custom_misc_set(e_misc_type, pv_kernel_set_info, z_size, b_store);
    
        x_mem_free(pv_kernel_set_info);
    }
    else
    {
        i4_ret = d_custom_misc_set(
                                e_misc_type,
                                pv_set_info,
                                z_size,
                                b_store);
    }

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }
    
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_misc_get
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_misc_get(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_MISC_TYPE_T e_misc_type;
    VOID*                 pv_get_info;
    VOID*                 pv_kernel_get_info = NULL;
    SIZE_T                *pz_size;
    SIZE_T                z_size;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    e_misc_type = (DRV_CUSTOM_MISC_TYPE_T)rArg.ai4Arg[0];
    pv_get_info = (VOID*)rArg.ai4Arg[1];
    pz_size = (SIZE_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_FROM_USER_ARG_K(pz_size, &z_size, sizeof(SIZE_T));

    pv_kernel_get_info = (VOID*)x_mem_alloc(z_size);
    if(pv_kernel_get_info == NULL)
    {
        return MTR_NOT_OK;
    }    

    // Maybe not necessary
    if (!access_ok(VERIFY_READ, (void __user *)pv_get_info, z_size))
    {
        printk("mtal_ioctl_mtdrvcust valid argument error\n");
        x_mem_free(pv_kernel_get_info);
        return MTR_ERR_INV;
    }
    COPY_FROM_USER_ARG_K(pv_get_info, pv_kernel_get_info, z_size);

    i4_ret = d_custom_misc_get(e_misc_type, pv_kernel_get_info, &z_size);   

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_TO_USER_ARG_K(pz_size, z_size, sizeof(SIZE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_TO_USER_ARG_KX(pv_get_info, pv_kernel_get_info, z_size);

    x_mem_free(pv_kernel_get_info);

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_misc_get_min_max
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_misc_get_min_max(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_MISC_TYPE_T e_misc_type;
    UINT16 u2Min, *pu2Min;
    UINT16 u2Max, *pu2Max;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    e_misc_type = (DRV_CUSTOM_VID_TYPE_T)rArg.ai4Arg[0];
    pu2Min = (UINT16*)rArg.ai4Arg[1];
    pu2Max = (UINT16*)rArg.ai4Arg[2];

    i4_ret = d_custom_misc_get_min_max(e_misc_type, &u2Min, &u2Max);   
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu2Min, sizeof(UINT16));
    COPY_TO_USER_ARG_K(pu2Min, u2Min, sizeof(UINT16));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu2Max, sizeof(UINT16));
    COPY_TO_USER_ARG_K(pu2Max, u2Max, sizeof(UINT16));

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_cust_spec_set
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_cust_spec_set(unsigned long arg)
{
#ifdef DRV_CUSTOM_CUST_SPEC_SET
    MTAL_IOCTL_4ARG_T rArg;
    DRV_CUSTOM_CUST_SPEC_TYPE_T  e_cust_spec_type;
    VOID*                        pv_set_info;
    SIZE_T                       z_size;
    VOID*                        pv_kernel_set_info = NULL;
    BOOL b_store;
    INT32 i4_ret;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    e_cust_spec_type = (DRV_CUSTOM_CUST_SPEC_TYPE_T)rArg.ai4Arg[0];
    pv_set_info = (VOID*)rArg.ai4Arg[1];
    z_size = (SIZE_T)rArg.ai4Arg[2];
    b_store = (BOOL)rArg.ai4Arg[3];

	
    /* only do rm_copy_from_user() when the argument is a pointer! */
    if (DRV_CUSTOM_ARG_USE_REF_OFFSET <= e_cust_spec_type)
    {
        pv_kernel_set_info = (VOID*)x_mem_alloc(z_size);
        if(pv_kernel_set_info == NULL)
        {
            return MTR_NOT_OK;
        }

        USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_set_info, z_size);
        COPY_FROM_USER_ARG_K(pv_set_info, pv_kernel_set_info, z_size);
    
        i4_ret = DRV_CUSTOM_CUST_SPEC_SET(e_cust_spec_type, pv_kernel_set_info, z_size, b_store);
    
        x_mem_free(pv_kernel_set_info);
    }
    else
    {
        i4_ret = DRV_CUSTOM_CUST_SPEC_SET(
                                e_cust_spec_type,
                                pv_set_info,
                                z_size,
                                b_store);
    }

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
#else
    return MTR_OK;
#endif
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_dcustom_cust_spec_get
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_dcustom_cust_spec_get(unsigned long arg)
{
#ifdef DRV_CUSTOM_CUST_SPEC_GET
    MTAL_IOCTL_3ARG_T rArg;
    INT32 i4_ret;    
    DRV_CUSTOM_CUST_SPEC_TYPE_T e_cust_spec_type;
    VOID*                 pv_get_info;
    VOID*                 pv_kernel_get_info = NULL;
    SIZE_T                *pz_size;
    SIZE_T                z_size;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    e_cust_spec_type = (DRV_CUSTOM_CUST_SPEC_TYPE_T)rArg.ai4Arg[0];
    pv_get_info = (VOID*)rArg.ai4Arg[1];
    pz_size = (SIZE_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_FROM_USER_ARG_K(pz_size, &z_size, sizeof(SIZE_T));

    pv_kernel_get_info = (VOID*)x_mem_alloc(z_size);
    if(pv_kernel_get_info == NULL)
    {
        return MTR_NOT_OK;
    }    

    // Maybe not necessary
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_FROM_USER_ARG_K(pv_get_info, pv_kernel_get_info, z_size);

    i4_ret = DRV_CUSTOM_CUST_SPEC_GET(e_cust_spec_type, pv_kernel_get_info, &z_size);   

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pz_size, sizeof(SIZE_T));
    COPY_TO_USER_ARG_K(pz_size, z_size, sizeof(SIZE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, z_size);
    COPY_TO_USER_ARG_KX(pv_get_info, pv_kernel_get_info, z_size);

    x_mem_free(pv_kernel_get_info);

    if(i4_ret != 0)
    {
        return MTR_NOT_OK;
    }  

    return MTR_OK;
#else
    return MTR_OK;
#endif
}


//-----------------------------------------------------------------------------
/** _MTDRVCUST_BootupCheck
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRVCUST_BootupCheck(unsigned long arg)
{
    UNUSED(arg);

    DRVCUST_BootupCheck();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

int mtal_ioctl_mtdrvcust(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;
	   
	switch (cmd) 
	{
	case MTAL_IO_DRVCUST_INITQUERY:
		i4Ret = _MTDRVCUST_InitQuery(arg);
		break;

    case MTAL_IO_DRVCUST_INITGET:
        i4Ret = _MTDRVCUST_InitGet(arg);
        break;

    case MTAL_IO_DRVCUST_OPTQUERY:
        i4Ret = _MTDRVCUST_OptQuery(arg);
        break;

    case MTAL_IO_DRVCUST_OPTGET:
        i4Ret = _MTDRVCUST_OptGet(arg);
        break;        

    case MTAL_IO_DRVCUST_CHIPSUPPORT:
        i4Ret = _MTDRVCUST_IsSupport(arg);
        break;
	
	case MTAL_IO_DRVCUST_GET_COUNTRY:
        i4Ret = _MTDRVCUST_GetCountry(arg);
        break;

	case MTAL_IO_DRVCUST_GET_COUNTRY_MASK:
        i4Ret = _MTDRVCUST_GetCountryMask(arg);
        break;

	case MTAL_IO_DRVCUST_SET_COUNTRY:
        i4Ret = _MTDRVCUST_SetCountry(arg);
        break;

    case MTAL_IO_DRVCUST_TUNER_SET:
        i4Ret = _MTDRVCUST_dcustom_tuner_set(arg);
        break;

    case MTAL_IO_DRVCUST_TUNER_GET:
        i4Ret = _MTDRVCUST_dcustom_tuner_get(arg);
        break;

    case MTAL_IO_DRVCUST_VID_SET:
        i4Ret = _MTDRVCUST_dcustom_vid_set(arg);
        break;

    case MTAL_IO_DRVCUST_VID_GET:
        i4Ret = _MTDRVCUST_dcustom_vid_get(arg);
        break;

#ifdef DRV_SUPPORT_EXTMJC
    case MTAL_IO_DRVCUST_EXTMJC_SET:
        i4Ret = _MTDRVCUST_dcustom_extmjc_set(arg);
        break;

    case MTAL_IO_DRVCUST_EXTMJC_GET:
        i4Ret = _MTDRVCUST_dcustom_extmjc_get(arg);
        break;
#endif

    case MTAL_IO_DRVCUST_VID_GET_GAMMA:
        i4Ret = _MTDRVCUST_dcustom_vid_get_gamma(arg);
        break;

    case MTAL_IO_DRVCUST_VID_GET_MIN_MAX:
        i4Ret = _MTDRVCUST_dcustom_vid_get_min_max(arg);
        break;

    case MTAL_IO_DRVCUST_SETDRVINIT:
        i4Ret = _MTDRVCUST_SetDrvInit(arg);
        break;

    case MTAL_IO_DRVCUST_MISC_SET:
        i4Ret = _MTDRVCUST_dcustom_misc_set(arg);
        break;

    case MTAL_IO_DRVCUST_MISC_GET:
        i4Ret = _MTDRVCUST_dcustom_misc_get(arg);
        break;

    case MTAL_IO_DRVCUST_SPEC_SET:
        i4Ret = _MTDRVCUST_dcustom_cust_spec_set(arg);
        break;

    case MTAL_IO_DRVCUST_SPEC_GET:
        i4Ret = _MTDRVCUST_dcustom_cust_spec_get(arg);
        break;

    case MTAL_IO_DRVCUST_MISC_GET_MIN_MAX:
        i4Ret = _MTDRVCUST_dcustom_misc_get_min_max(arg);
        break;
        
    case MTAL_IO_DRVCUST_BOOTUPCHECK:
        i4Ret = _MTDRVCUST_BootupCheck(arg);
        break;

    default:
		i4Ret = MTR_NOT_SUPPORTED;
		break;
	}
	return i4Ret;
}

