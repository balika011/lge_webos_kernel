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
 * $RCSfile: io_mtamb.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: wen han $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mtamb.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtamb.h"
#include <linux/mtal_ioctl.h>
#include "general.h"
#include "cb_low.h"
#include "io_mtamb.h"
#include "vdo_misc.h"
#include "api_notify.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define	xMAMB_MODULE_READY_F

#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtamb USR_SPACE_ACCESS_VALIDATE_ARG argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
	if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   size))			   \
	{														\
		printk("mtal_ioctl_mtamb argument error\n");	 \
		return MTR_ERR_INV; 										 \
	}

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtamb COPY_TO_USER_ARG argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
	if (copy_to_user((void __user *)ArgUsr, ArgKernel,	 \
						   size))		   \
	{														\
		printk("mtal_ioctl_mtamb argument error\n");	  \
		return MTR_ERR_INV; 										 \
	}

#define _MTAL_DEBUG_F
#ifdef _MTAL_DEBUG_F
static mamb_AmbilightMode_e g_ambi_mode = DRVAMB_AMBILIGHT_MODE_OFF;

static int g_feature_level[] = {
    50, 	// 0 DRVAMB_AMBILIGHT_FEATURE_IMMERSIONLEVEL
    50, 	// 1 DRVAMB_AMBILIGHT_FEATURE_BRIGHTNESS
    50, 	// 2 DRVAMB_AMBILIGHT_FEATURE_SATURATION
    50,		// 3 DRVAMB_AMBILIGHT_FEATURE_COLORBOOST
    50,		// 4 DRVAMB_AMBILIGHT_FEATURE_MOTION_AMBILIGHT_LEVEL
    50,		// 5 DRVAMB_AMBILIGHT_FEATURE_SHOPBOOST
    50		// 6 DRVAMB_AMBILIGHT_FEATURE_ANDROID_LIGHT_COUPLING
};

static mamb_AmbilightColor_t g_wall_color = {0,0,0};

static mamb_AmbilightColor_t g_PixelArray[10] = {	// MAX_PIXEL_ARRAY	(10)
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0}		
};
#endif
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTAMB_al_cfg_init(unsigned long arg)
	//(void)
{
		u4MtalLogMask2 |= MTAMB_LOG_MASK;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_init \n");
		
		return MTR_OK;
}

	/**
	 *	@brief Set the mode how the ambilight will behave
	 * 
	 * @param [in] ambilightmode the ambilight mode, See enum \ref MTAMB_al_cfg_AmbilightMode_e
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	  */
static MT_RESULT_T _MTAMB_al_cfg_setMode(unsigned long arg)
	//(MTAMB_al_cfg_AmbilightMode_e ambilightmode )
{
	INT32	 iArg;
	mamb_AmbilightMode_e ambilightmode;
		
	USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
	COPY_FROM_USER_1ARG(arg, iArg);
		
	ambilightmode = (mamb_AmbilightMode_e)iArg;
		
	MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setMode (%d) \n ", (int)ambilightmode);
	
#ifdef _MTAL_DEBUG_F
	g_ambi_mode = ambilightmode;
#endif

#ifdef MAMB_MODULE_READY_F	
	Mamb_setMode(ambilightmode);
#endif
	
	return MTR_OK;
}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get the mode how ambilight is set to behave
	 *	
	 * @param  [out] ambilightmode pointer containing the mode, See enum \ref MTAMB_al_cfg_AmbilightMode_e
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getMode(unsigned long arg)
	//(MTAMB_al_cfg_AmbilightMode_e *ambilightmode )
	{
		INT32	 iArg;
		MTAMB_al_cfg_AmbilightMode_e *up_ambilightmode;
		mamb_AmbilightMode_e dramb_ambilightmode;
		
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);
		
		up_ambilightmode = (MTAMB_al_cfg_AmbilightMode_e *)iArg;
		
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getMode \n ");

#ifdef _MTAL_DEBUG_F
	dramb_ambilightmode = g_ambi_mode;
	MTAMB_PRINT("[KernelSpace] -dramb_ambilightmode = %d \n ", (int)dramb_ambilightmode );
#endif

#ifdef MAMB_MODULE_READY_F	
		Mamb_getMode(&dramb_ambilightmode);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_ambilightmode, MTAMB_al_cfg_AmbilightMode_e);
		COPY_TO_USER_ARG(up_ambilightmode, dramb_ambilightmode, MTAMB_al_cfg_AmbilightMode_e);
				
		return MTR_OK;
	}
	
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Mute ambilight
	 * 
	 * @param [in]	mute the mute being set
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setMute(unsigned long arg)
	//(bool mute )
	{
		INT32	 iArg;
		BOOL dramb_mute;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);
			
		dramb_mute = (BOOL)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setMute \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_setMute(dramb_mute);
#endif
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get mute status
	 * 
	 * @param [out] is_muted pointer to mute status
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getMute(unsigned long arg)
	//( bool * is_muted )
	{
		INT32	 iArg;
		BOOL dramb_is_muted;
		BOOL *up_is_muted;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);
			
		up_is_muted = (BOOL *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getMute \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getMute(&dramb_is_muted);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_is_muted, BOOL);
		COPY_TO_USER_ARG(up_is_muted, dramb_is_muted, BOOL);
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Set lounge mood
	 * 
	 * @param [in] ambilightloungemood , See enum \ref MTAMB_al_cfg_AmbilightLoungeMoodSettings_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setLoungeMood(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightLoungeMoodSettings_t * ambilightloungemood )
	{
		INT32	 iArg;
		mamb_AmbilightLoungeMoodSettings_t *pdramb_ambilightloungemood;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);
			
		pdramb_ambilightloungemood = (mamb_AmbilightLoungeMoodSettings_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setLoungeMood \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_setLoungeMood(pdramb_ambilightloungemood);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Set lounge mood
	 * 
	 * @param [out] ambilightloungemood pointer to the set mood , See enum \ref MTAMB_al_cfg_AmbilightLoungeMoodSettings_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getLoungeMood(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightLoungeMoodSettings_t * ambilightloungemood )
	{
		INT32	 iArg;
		MTAMB_al_cfg_AmbilightLoungeMoodSettings_t *up_ambilightloungemood;
		mamb_AmbilightLoungeMoodSettings_t dramb_ambilightloungemood;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);
			
		up_ambilightloungemood = (MTAMB_al_cfg_AmbilightLoungeMoodSettings_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getLoungeMood \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getLoungeMood(&dramb_ambilightloungemood);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_ambilightloungemood, MTAMB_al_cfg_AmbilightLoungeMoodSettings_t);
		COPY_TO_USER_ARG(up_ambilightloungemood, dramb_ambilightloungemood, MTAMB_al_cfg_AmbilightLoungeMoodSettings_t);
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Set the level of an ambilight feature, range can be requested through GetFeatureLevelRange
	 * 
	 * @param  [in] ambilightfeature @see ambilightfeature , See enum \ref MTAMB_al_cfg_AmbilightFeature_e
	 * @param [in] level For range @see GetFeatureLevelRange
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setFeatureLevel(unsigned long arg)
	//(MTAMB_al_cfg_AmbilightFeature_e ambilightfeature, int level )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		mamb_AmbilightFeature_e dramb_ambilightfeature;
		int dramb_level;
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_ambilightfeature = (mamb_AmbilightFeature_e)rArg.ai4Arg[0];
		dramb_level = (int)rArg.ai4Arg[1];
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setFeatureLevel \n ");
#ifdef _MTAL_DEBUG_F
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setFeatureLevel(%d, %d) \n ", dramb_ambilightfeature,dramb_level);

		if ((dramb_ambilightfeature > DRVAMB_AMBILIGHT_FEATURE_ANDROID_LIGHT_COUPLING) || (dramb_level < 0) || (dramb_level > 100) )
			return MTR_NOT_OK;
		
		g_feature_level[dramb_ambilightfeature] = dramb_level;
#endif
#ifdef MAMB_MODULE_READY_F	
		Mamb_setFeatureLevel(dramb_ambilightfeature, dramb_level);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get the level of an ambilight feature, range can be requested through GetFeatureLevelRange
	 * 
	 * @param [in]	ambilightfeature @see ambilightfeature, See enum \ref MTAMB_al_cfg_AmbilightFeature_e
	 * @param [out] level Pointer with result. For range @see GetFeatureLevelRange
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getFeatureLevel(unsigned long arg)
	//(MTAMB_al_cfg_AmbilightFeature_e ambilightfeature, int * level )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		mamb_AmbilightFeature_e dramb_ambilightfeature;
		int dramb_level;
		int *up_level;
		
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_ambilightfeature = (mamb_AmbilightFeature_e)rArg.ai4Arg[0];
		up_level = (int *)rArg.ai4Arg[1];
		
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFeatureLevel \n ");

#ifdef _MTAL_DEBUG_F
		if (dramb_ambilightfeature > DRVAMB_AMBILIGHT_FEATURE_ANDROID_LIGHT_COUPLING)
			return MTR_NOT_OK;
		
		dramb_level = g_feature_level[dramb_ambilightfeature];
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFeatureLevel(%d, %d) \n ", dramb_ambilightfeature, dramb_level);		
#endif
#ifdef MAMB_MODULE_READY_F	
		Mamb_getFeatureLevel(dramb_ambilightfeature, &dramb_level);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_level, int);
		COPY_TO_USER_ARG(up_level, dramb_level, int);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Set color used for wall color adaption
	 * 
	 * @param [in] wallcolor @see AmbilightColor, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setWallColor(unsigned long arg)
	//(MTAMB_al_cfg_AmbilightColor_t *wallcolor )
	{
		INT32	 iArg;
		mamb_AmbilightColor_t *dramb_wallcolor;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		dramb_wallcolor = (mamb_AmbilightColor_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setWallColor \n ");
#ifdef _MTAL_DEBUG_F

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setWallColor(R=%d, G=%d,B=%d) \n ", dramb_wallcolor->red, dramb_wallcolor->green, dramb_wallcolor->blue);

		memcpy(&g_wall_color,dramb_wallcolor,sizeof(mamb_AmbilightColor_t) );
#endif
#ifdef MAMB_MODULE_READY_F	
		Mamb_setWallColor(dramb_wallcolor);
#endif
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get color used for wall color adaption
	 * 
	 * @param [out] wallcolor pointer to color, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getWallColor(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightColor_t *wallcolor )
	{
		INT32	 iArg;
		mamb_AmbilightColor_t dramb_wallcolor;
		MTAMB_al_cfg_AmbilightColor_t *up_wallcolor;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		up_wallcolor = (MTAMB_al_cfg_AmbilightColor_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getWallColor \n ");

#ifdef _MTAL_DEBUG_F
		memcpy(&dramb_wallcolor, &g_wall_color,sizeof(mamb_AmbilightColor_t) );
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getWallColor(R=%d, G=%d,B=%d) \n ", dramb_wallcolor.red, dramb_wallcolor.green, dramb_wallcolor.blue);
#endif
#ifdef MAMB_MODULE_READY_F
		Mamb_getWallColor(&dramb_wallcolor);
#endif
		
		USR_SPACE_ACCESS_VALIDATE_ARG(up_wallcolor, MTAMB_al_cfg_AmbilightColor_t);
		COPY_TO_USER_ARG(up_wallcolor, dramb_wallcolor, MTAMB_al_cfg_AmbilightColor_t);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Set color for static mode, can be ISF or normal static mode
	 * 
	 * @param  [in] color @see AmbilightColor, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setStaticColorRGB(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightColor_t * color )
	{
		INT32	 iArg;
		mamb_AmbilightColor_t *dramb_color;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		dramb_color = (mamb_AmbilightColor_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setStaticColorRGB \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_setStaticColorRGB(dramb_color);
#endif
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get color for static mode, can be ISF or normal static mode
	 * 
	 * @param  [out]  color pointer to color, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getStaticColorRGB(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightColor_t * color )
	{
		INT32	 iArg;
		mamb_AmbilightColor_t dramb_color;
		MTAMB_al_cfg_AmbilightColor_t *up_color;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		up_color = (MTAMB_al_cfg_AmbilightColor_t *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getStaticColorRGB \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getStaticColorRGB(&dramb_color);
#endif
		
		USR_SPACE_ACCESS_VALIDATE_ARG(up_color, MTAMB_al_cfg_AmbilightColor_t);
		COPY_TO_USER_ARG(up_color, dramb_color, MTAMB_al_cfg_AmbilightColor_t);

		return MTR_OK;
	}
	
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Configure sunset, mSec == 0 means off. Setting a delay Enables or disables Sunset next time the TV is switched-off. Default=FALSE.
	 * 
	 * @param [in] msec the desired sunset duration. mSec == 0 means off
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setSunsetDuration(unsigned long arg)
	//(int msec )
	{
		INT32	 iArg;
		int *dramb_msec;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		dramb_msec = (int *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setSunsetDuration \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_setStaticColorRGB(dramb_msec);
#endif
		
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Get sunset configuration
	 * 
	 * @param [out] msec pointer the the sunset duration
	 * @retval MTR_OK
	 * @retval MTR_NOT_OK
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getSunsetDuration(unsigned long arg)
	//( int * msec )
	{
		INT32	 iArg;
		int dramb_msec;
		int *up_msec;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		up_msec = (int *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getSunsetDuration \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getStaticColorRGB(&dramb_msec);
#endif
		
		USR_SPACE_ACCESS_VALIDATE_ARG(up_msec, int);
		COPY_TO_USER_ARG(up_msec, dramb_msec, int);

		return MTR_OK;
	}
	
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Get ambilight configuration. Used for Xtv/Factory
	 * 
	 * @param [out] topology Pointer to topology, See enum \ref MTAMB_al_cfg_AmbilightTopology_e
	 * @param [out] no_of_vertpixels Pointer to number of vertical pixels
	 * @param [out] no_of_horpixels Pointer to number of horizontal pixels
	 * @param [out] no_of_boards Pointer to number of boards
	 * @param [out] version Pointer to version number
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getConfiguration(unsigned long arg)
	//( MTAMB_al_cfg_AmbilightTopology_e * topology, int * no_of_vertpixels, int * no_of_horpixels, int * no_of_boards	, int * version	)
	{	
		MTAL_IOCTL_5ARG_T	 rArg;

		mamb_AmbilightTopology_e dramb_topology;
		int dramb_no_of_vertpixels;
		int dramb_no_of_horpixels;
		int dramb_no_of_boards;
		int dramb_version;

		MTAMB_al_cfg_AmbilightTopology_e * up_topology;
		int * up_no_of_vertpixels;
		int * up_no_of_horpixels;
		int * up_no_of_boards;
		int * up_version;

		USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
		COPY_FROM_USER_5ARG(arg, rArg);
		
		up_topology = (MTAMB_al_cfg_AmbilightTopology_e *)rArg.ai4Arg[0];
		up_no_of_vertpixels = (int *)rArg.ai4Arg[1];
		up_no_of_horpixels = (int *)rArg.ai4Arg[2];
		up_no_of_boards = (int *)rArg.ai4Arg[3];
		up_version = (int *)rArg.ai4Arg[4];

#ifdef MAMB_MODULE_READY_F	
		Mamb_getConfiguration(&dramb_topology, &dramb_no_of_vertpixels, &dramb_no_of_horpixels, &dramb_no_of_boards, &dramb_version);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_topology, MTAMB_al_cfg_AmbilightTopology_e);
		COPY_TO_USER_ARG(up_topology, dramb_topology, MTAMB_al_cfg_AmbilightTopology_e);

		USR_SPACE_ACCESS_VALIDATE_ARG(up_no_of_horpixels, int);
		COPY_TO_USER_ARG(up_no_of_horpixels, dramb_no_of_vertpixels, int);

		USR_SPACE_ACCESS_VALIDATE_ARG(up_no_of_horpixels, int);
		COPY_TO_USER_ARG(up_no_of_horpixels, dramb_no_of_horpixels, int);

		USR_SPACE_ACCESS_VALIDATE_ARG(up_no_of_boards, int);
		COPY_TO_USER_ARG(up_no_of_boards, dramb_no_of_boards, int);

		USR_SPACE_ACCESS_VALIDATE_ARG(up_version, int);
		COPY_TO_USER_ARG(up_version, dramb_version, int);

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getConfiguration \n ");
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Xtv/Factory. Set individual pixel color.
	 * 
	 * @param[in] pixel id of pixel
	 * @param[in] pixelcolors Color for pixel @see AmbilightColor, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setPixel(unsigned long arg)
	//(int pixel, MTAMB_al_cfg_AmbilightColor_t * pixelcolors )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_pixel;
		mamb_AmbilightColor_t *pdramb_pixelcolors;
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_pixel = (int )rArg.ai4Arg[0];
		pdramb_pixelcolors = (mamb_AmbilightColor_t *)rArg.ai4Arg[1];
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setPixel \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_setPixel(dramb_pixel, pdramb_pixelcolors);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Xtv/Factory. Get individual pixel color.
	 * 
	 * @param[in] pixel  id of pixel
	 * @param[out]	pixelcolors Pointer to Color for pixel, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getPixel(unsigned long arg)
	//(int  pixel, MTAMB_al_cfg_AmbilightColor_t * pixelcolors )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_pixel;
		mamb_AmbilightColor_t dramb_pixelcolors;
		MTAMB_al_cfg_AmbilightColor_t *up_pixelcolors;
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_pixel = (int )rArg.ai4Arg[0];
		up_pixelcolors = (MTAMB_al_cfg_AmbilightColor_t *)rArg.ai4Arg[1];
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getPixel \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getPixel(dramb_pixel, &dramb_pixelcolors);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_pixelcolors, MTAMB_al_cfg_AmbilightColor_t);
		COPY_TO_USER_ARG(up_pixelcolors, dramb_pixelcolors, MTAMB_al_cfg_AmbilightColor_t);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Xtv/Factory. Set color for array of pixels
	 * 
	 * @param[in]  size size of colors array
	 * @param[in]  pixelcolorsarray Pointer to array of Colors, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setPixelArray(unsigned long arg)
	//(int	size, MTAMB_al_cfg_AmbilightColor_t * pixelcolorsarray )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_size;
		mamb_AmbilightColor_t *dramb_pixelcolorsarray;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setPixelArray \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_size = (int )rArg.ai4Arg[0];
		dramb_pixelcolorsarray = (mamb_AmbilightColor_t *)rArg.ai4Arg[1];
			
#ifdef _MTAL_DEBUG_F
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setPixelArray (%d) PixelArray\n ",dramb_size);		
		if (dramb_size > 10) 	//MAX_PIXEL_ARRAY
			return MTR_NOT_OK;
		
		memcpy(g_PixelArray, dramb_pixelcolorsarray,dramb_size * sizeof(mamb_AmbilightColor_t) );
#endif
#ifdef MAMB_MODULE_READY_F	
		Mamb_setPixelArray(dramb_size, dramb_pixelcolorsarray);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 * brief Xtv/Factory. Get color for array of pixels
	 * 
	 * @param[in]  size size of colors array
	 * @param[out] pixelcolorsarray Pointer to array of Colors, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */ 
	static MT_RESULT_T _MTAMB_al_cfg_getPixelArray(unsigned long arg)
	//(int	size, MTAMB_al_cfg_AmbilightColor_t * pixelcolorsarray )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_size;
		mamb_AmbilightColor_t *dramb_pixelcolorsarray;
		MTAMB_al_cfg_AmbilightColor_t *up_pixelcolorsarray;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getPixelArray \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_size = (int )rArg.ai4Arg[0];
		up_pixelcolorsarray = (MTAMB_al_cfg_AmbilightColor_t *)rArg.ai4Arg[1];
			
#ifdef _MTAL_DEBUG_F
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getPixelArray get (%d) PixelArray\n ",dramb_size);
		if (dramb_size > 10)	//MAX_PIXEL_ARRAY
			return MTR_NOT_OK;

		dramb_pixelcolorsarray = &g_PixelArray[0];			
#endif
#ifdef MAMB_MODULE_READY_F	
		Mamb_getPixelArray(dramb_size, &dramb_pixelcolorsarray);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(up_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));
		COPY_TO_USER_ARG_SIZE(up_pixelcolorsarray, dramb_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Xtv/Factory. Get measured color for array of pixels
	 * 
	 * @param[in]  size size of colors array
	 * @param[out]	pixelcolorsarray Pointer to array of Colors, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getMeasuredPixelArray(unsigned long arg)
	//( int  size, MTAMB_al_cfg_AmbilightColor_t * pixelcolorsarray )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_size;
		mamb_AmbilightColor_t *dramb_pixelcolorsarray;
		MTAMB_al_cfg_AmbilightColor_t *up_pixelcolorsarray;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getMeasuredPixelArray \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_size = (int )rArg.ai4Arg[0];
		up_pixelcolorsarray = (MTAMB_al_cfg_AmbilightColor_t *)rArg.ai4Arg[1];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getMeasuredPixelArray(dramb_size, &dramb_pixelcolorsarray);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(up_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));
		COPY_TO_USER_ARG_SIZE(up_pixelcolorsarray, dramb_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	\brief Xtv/Factory. Get processed color for array of pixels   (always linear light domain!) 
	 * 
	 * @param[in]  size size of colors array
	 * @param[in]  colors Pointer to array of Colors @see AmbilightColor, See enum \ref MTAMB_al_cfg_AmbilightColor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getProcessedPixelArray(unsigned long arg)
	//( int  size, MTAMB_al_cfg_AmbilightColor_t * pixelcolorsarray )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_size;
		mamb_AmbilightColor_t *dramb_pixelcolorsarray;
		MTAMB_al_cfg_AmbilightColor_t *up_pixelcolorsarray;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getProcessedPixelArray \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_size = (int )rArg.ai4Arg[0];
		up_pixelcolorsarray = (MTAMB_al_cfg_AmbilightColor_t *)rArg.ai4Arg[1];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getProcessedPixelArray(dramb_size, &dramb_pixelcolorsarray);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(up_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));
		COPY_TO_USER_ARG_SIZE(up_pixelcolorsarray, dramb_pixelcolorsarray, dramb_size * sizeof(MTAMB_al_cfg_AmbilightColor_t));

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Get number of pixels per board
	 * 
	 * @param[in]  board Range 0..8 ( 0 == all boards )
	 * @param[out]	no_of_pixels  Pointer to number of pixels. Range 1..8
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getNbrOfPixelsPerboard(unsigned long arg)
	//( int board, int * no_of_pixels )
	{
		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_board;
		int dramb_no_of_pixels;

		int *up_no_of_pixels;
		
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getNbrOfPixelsPerboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		dramb_no_of_pixels = (int )rArg.ai4Arg[1];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getNbrOfPixelsPerboard(dramb_board, &dramb_no_of_pixels);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_no_of_pixels, int);
		COPY_TO_USER_ARG(up_no_of_pixels, dramb_no_of_pixels, int);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Set the matrix per board.
	 * 
	 * @param[in]  board  range 0..32 ( 0 == all boards )
	 * @param [in] led_index  range 0..32 ( 0 == all leds on the boards )
	 * @param[in]  matrix Struct, all elements have range 0..255 , See enum \ref MTAMB_al_cfg_AmbilightMatrix_t
	 * @param[in] factors Struct, all elements have range 0..255, See enum \ref MTAMB_al_cfg_AmbilightFactor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setFactoryMatrixboard(unsigned long arg)
	//( int board, int led_index, MTAMB_al_cfg_AmbilightMatrix_t * matrix, MTAMB_al_cfg_AmbilightFactor_t * factors )
	{
		MTAL_IOCTL_4ARG_T	 rArg;
		int dramb_board;
		int dramb_led_index;
		mamb_AmbilightMatrix_t *pdramb_matrix;
		mamb_AmbilightFactor_t *pdramb_factors;

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setFactoryMatrixboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
		COPY_FROM_USER_4ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		dramb_led_index = (int )rArg.ai4Arg[1];
		pdramb_matrix = (mamb_AmbilightMatrix_t *)rArg.ai4Arg[2];
		pdramb_factors = (mamb_AmbilightFactor_t *)rArg.ai4Arg[3];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_setFactoryMatrixboard(dramb_board, dramb_led_index, pdramb_matrix, pdramb_factors);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 * @brief  Factory. Get the matrix per board.
	 * 
	 * @param[in]  board  range 0..32 ( 0 == all boards )
	 * @param[in]  led_index  range 0..32 ( 0 == all leds on the boards )
	 * @param[out] matrix  Pointer to struct, all elements have range 0..255  , See enum \ref MTAMB_al_cfg_AmbilightMatrix_t
	 * @param[out] factors	Pointer to struct, all elements have range 0..255  , See enum \ref MTAMB_al_cfg_AmbilightFactor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getFactoryMatrixboard(unsigned long arg)
	//( int board, int led_index, MTAMB_al_cfg_AmbilightMatrix_t * matrix, MTAMB_al_cfg_AmbilightFactor_t * factors )
	{	
		MTAL_IOCTL_4ARG_T	 rArg;
		int dramb_board;
		int dramb_led_index;
		mamb_AmbilightMatrix_t dramb_matrix;
		mamb_AmbilightFactor_t dramb_factors;

		MTAMB_al_cfg_AmbilightMatrix_t *up_matrix;
		MTAMB_al_cfg_AmbilightFactor_t *up_factors;
		
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFactoryMatrixboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		dramb_led_index = (int )rArg.ai4Arg[1];
		up_matrix = (MTAMB_al_cfg_AmbilightMatrix_t *)rArg.ai4Arg[1];
		up_factors = (MTAMB_al_cfg_AmbilightFactor_t *)rArg.ai4Arg[1];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getFactoryMatrixboard(dramb_board, dramb_led_index, &dramb_matrix, &dramb_factors);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_matrix, MTAMB_al_cfg_AmbilightMatrix_t);
		COPY_TO_USER_ARG(up_matrix, dramb_matrix, MTAMB_al_cfg_AmbilightMatrix_t);

		USR_SPACE_ACCESS_VALIDATE_ARG(up_factors, MTAMB_al_cfg_AmbilightFactor_t);
		COPY_TO_USER_ARG(up_factors, dramb_factors, MTAMB_al_cfg_AmbilightFactor_t);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 * @brief  Factory. Set the brightness per board
	 * 
	 * @param[in]  board Range 0..32 ( 0 == all boards )
	 * @param[in]  led_index Range 0..32 ( 0 == all leds on the boards )
	 * @param[in] factors struct  with red/green/bluefactors. Range 0..255	, See enum \ref MTAMB_al_cfg_AmbilightFactor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_setFactoryBrightnessboard(unsigned long arg)
	//(  int board, int led_index,  MTAMB_al_cfg_AmbilightFactor_t * factors )
	{	

		MTAL_IOCTL_2ARG_T	 rArg;
		int dramb_board;
		int dramb_led_index;
		mamb_AmbilightFactor_t *pdramb_factors;


		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_setFactoryBrightnessboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		dramb_led_index = (int )rArg.ai4Arg[1];
		pdramb_factors = (mamb_AmbilightFactor_t *)rArg.ai4Arg[2];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_setFactoryBrightnessboard(dramb_board, dramb_led_index, pdramb_factors);
#endif

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Get the brightness per board
	 * 
	 * @param[in] board  Range 0..32 ( 0 == all boards )
	 * @param[in] led_index Range 0..32 ( 0 == all leds on the boards )
	 * @param[out] factors	Pointer to struct with red/green/bluefactors. Range 0..255	, See enum \ref MTAMB_al_cfg_AmbilightFactor_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getFactoryBrightnessboard(unsigned long arg)
	//( int board, int led_index, MTAMB_al_cfg_AmbilightFactor_t * factors )
	{
		MTAL_IOCTL_3ARG_T	 rArg;
		int dramb_board;
		int dramb_led_index;
		mamb_AmbilightFactor_t dramb_factors;
		MTAMB_al_cfg_AmbilightFactor_t *up_factors;


		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFactoryBrightnessboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
		COPY_FROM_USER_3ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		dramb_led_index = (int )rArg.ai4Arg[1];
		up_factors = (MTAMB_al_cfg_AmbilightFactor_t *)rArg.ai4Arg[2];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getFactoryBrightnessboard(dramb_board, dramb_led_index, &dramb_factors);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_factors, MTAMB_al_cfg_AmbilightFactor_t);
		COPY_TO_USER_ARG(up_factors, dramb_factors, MTAMB_al_cfg_AmbilightFactor_t);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Apply the new settings. This must be called to commit the changed settings.
	 */
	static MT_RESULT_T _MTAMB_al_cfg_saveSettings(unsigned long arg)
	//( void )
	{	
#ifdef MAMB_MODULE_READY_F	
		Mamb_saveSettings();
#endif

		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_saveSettings \n ");
		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 * @brief  Factory. Check saved status
	 * 
	 * @param[out]	retval Pointer to result @see alSettingsSavedStatus_t
	 */
	static MT_RESULT_T _MTAMB_al_cfg_checkSettingsSaved(unsigned long arg)
	//( int * retval )
	{
		INT32	 iArg;
		int dramb_retval;
		int *up_retval;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		up_retval = (int *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_checkSettingsSaved \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_checkSettingsSaved(&dramb_retval);
#endif
		
		USR_SPACE_ACCESS_VALIDATE_ARG(up_retval, int);
		COPY_TO_USER_ARG(up_retval, dramb_retval, int);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Get number of leds per board
	 * 
	 * @param[in]  board Selection of board. Range 0..8 (0 == all boards )
	 * @param[out] no_of_leds  Pointer to result. Range 1..8
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getFactoryNbrOfLedsPerboard(unsigned long arg)
	//( int board, int * no_of_leds )
	{	
		MTAL_IOCTL_4ARG_T	 rArg;
		int dramb_board;
		int dramb_no_of_leds;

		int *up_no_of_leds;
		
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFactoryNbrOfLedsPerboard \n ");
			
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);

		dramb_board = (int )rArg.ai4Arg[0];
		up_no_of_leds = (int *)rArg.ai4Arg[1];
			
#ifdef MAMB_MODULE_READY_F	
		Mamb_getFactoryNbrOfLedsPerboard(dramb_board, &dramb_no_of_leds);
#endif

		USR_SPACE_ACCESS_VALIDATE_ARG(up_no_of_leds, int);
		COPY_TO_USER_ARG(up_no_of_leds, dramb_no_of_leds, int);

		return MTR_OK;
	}
	
	/** @ingroup MTAMB_al_cfg_Functions */
	/**
	 *	@brief Factory. Get LED Hw option
	 * 
	 * @param[out] led_hw_option  Pointer to option
	 */
	static MT_RESULT_T _MTAMB_al_cfg_getFactoryLedHwOption(unsigned long arg)
	//( int * led_hw_option )
	{
		INT32	 iArg;
		int dramb_led_hw_option;
		int *up_led_hw_option;
			
		USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
		COPY_FROM_USER_1ARG(arg, iArg);

		up_led_hw_option = (int *)iArg;
			
		MTAMB_PRINT("[KernelSpace] -_MTAMB_al_cfg_getFactoryLedHwOption \n ");
#ifdef MAMB_MODULE_READY_F	
		Mamb_getFactoryLedHwOption(&dramb_led_hw_option);
#endif
		
		USR_SPACE_ACCESS_VALIDATE_ARG(up_led_hw_option, int);
		COPY_TO_USER_ARG(up_led_hw_option, dramb_led_hw_option, int);

		return MTR_OK;
	}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
int mtal_ioctl_mtamb(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    INT32 i4Ret = MTR_OK;

    switch (cmd)
    {			
        case MTAL_IO_AMB_SETMODE:
		i4Ret = _MTAMB_al_cfg_setMode(arg);
		break;
			
	case MTAL_IO_AMB_GETMODE:
		i4Ret = _MTAMB_al_cfg_getMode(arg);
		break;
			
	case MTAL_IO_AMB_SETMUTE:
		i4Ret = _MTAMB_al_cfg_setMute(arg);
		break;
			
	case MTAL_IO_AMB_GETMUTE:
		i4Ret = _MTAMB_al_cfg_getMute(arg);
		break;
			
	case MTAL_IO_AMB_SETLOUNGEMOOD:
		i4Ret = _MTAMB_al_cfg_setLoungeMood(arg);
		break;
			
	case MTAL_IO_AMB_GETLOUNGEMOOD:
		i4Ret = _MTAMB_al_cfg_getLoungeMood(arg);
		break;
			
	case MTAL_IO_AMB_SETFEATURELEVEL:
		i4Ret = _MTAMB_al_cfg_setFeatureLevel(arg);
		break;
			
	case MTAL_IO_AMB_GETFEATURELEVEL:
		i4Ret = _MTAMB_al_cfg_getFeatureLevel(arg);
		break;
			
	case MTAL_IO_AMB_SETWALLCOLOR:
		i4Ret = _MTAMB_al_cfg_setWallColor(arg);
		break;
			
	case MTAL_IO_AMB_GETWALLCOLOR:
		i4Ret = _MTAMB_al_cfg_getWallColor(arg);
		break;
		
	case MTAL_IO_AMB_SETSTATICCOLORRGB:
		i4Ret = _MTAMB_al_cfg_setStaticColorRGB(arg);
		break;
			
	case MTAL_IO_AMB_GETSTATICCOLORRGB:
		i4Ret = _MTAMB_al_cfg_getStaticColorRGB(arg);
		break;
			
	case MTAL_IO_AMB_SETSUNSETDURATION:
		i4Ret = _MTAMB_al_cfg_setSunsetDuration(arg);
		break;
			
	case MTAL_IO_AMB_GETSUNSETDURATION:
		i4Ret = _MTAMB_al_cfg_getSunsetDuration(arg);
		break;
			
	case MTAL_IO_AMB_GETCONFIGURATION:
		i4Ret = _MTAMB_al_cfg_getConfiguration(arg);
		break;
			
	case MTAL_IO_AMB_SETPIXEL:
		i4Ret = _MTAMB_al_cfg_setPixel(arg);
		break;
			
	case MTAL_IO_AMB_GETPIXEL:
		i4Ret = _MTAMB_al_cfg_getPixel(arg);
		break;
			
	case MTAL_IO_AMB_SETPIXELARRAY:
		i4Ret = _MTAMB_al_cfg_setPixelArray(arg);
		break;
			
	case MTAL_IO_AMB_GETPIXELARRAY:
		i4Ret = _MTAMB_al_cfg_getPixelArray(arg);
		break;
			
	case MTAL_IO_AMB_GETMEASUREDPIXELARRAY:
		i4Ret = _MTAMB_al_cfg_getMeasuredPixelArray(arg);
		break;
			
	case MTAL_IO_AMB_GETPROCESSEDPIXELARRAY:
		i4Ret = _MTAMB_al_cfg_getProcessedPixelArray(arg);
		break;
			
	case MTAL_IO_AMB_GETNBROFPIXELSPERBOARD:
		i4Ret = _MTAMB_al_cfg_getNbrOfPixelsPerboard(arg);
		break;
			
	case MTAL_IO_AMB_SETFACTORYMATRIXBOARD:
		i4Ret = _MTAMB_al_cfg_setFactoryMatrixboard(arg);
		break;
			
	case MTAL_IO_AMB_GETFACTORYMATRIXBOARD:
		i4Ret = _MTAMB_al_cfg_getFactoryMatrixboard(arg);
		break;
			
	case MTAL_IO_AMB_SETFACTORYBRIGHTNESSBOARD:
		i4Ret = _MTAMB_al_cfg_setFactoryBrightnessboard(arg);
		break;
		
	case MTAL_IO_AMB_GETFACTORYBRIGHTNESSBOARD:
		i4Ret = _MTAMB_al_cfg_getFactoryBrightnessboard(arg);
		break;
			
	case MTAL_IO_AMB_SAVESETTINGS:
		i4Ret = _MTAMB_al_cfg_saveSettings(arg);
		break;
			
	case MTAL_IO_AMB_CHECKSETTINGSSAVED:
		i4Ret = _MTAMB_al_cfg_checkSettingsSaved(arg);
		break;

	case MTAL_IO_AMB_GETFACTORYNBROFLEDSPERBOARD:
		i4Ret = _MTAMB_al_cfg_getFactoryNbrOfLedsPerboard(arg);
		break;

	case MTAL_IO_AMB_GETFACTORYLEDHWOPTION:
		i4Ret = _MTAMB_al_cfg_getFactoryLedHwOption(arg);
		break;

	case MTAL_IO_AMB_INIT:
		i4Ret = _MTAMB_al_cfg_init(arg);
		break;
	
    default:
        i4Ret = MTR_PARAMETER_ERROR;
        break;
    }

    return i4Ret;
}
