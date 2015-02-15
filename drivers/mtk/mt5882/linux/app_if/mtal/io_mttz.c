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
 * $RCSfile: io_mttz.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: MTAL TrustZone entries in Linux driver.
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mttype.h"
#include <linux/mtal_ioctl.h>
#include "x_bim.h"
#include "x_hal_arm.h"
#include "mttz.h"
#ifdef CC_TRUSTZONE_SUPPORT
#include "tz_if.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTTZ_PRINT(fmt...)  MTAL_LOG(1, "MTTZ", fmt)	

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error1.1\n");\
        return MTR_ERR_INV;                                 \
    }

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error1.2\n");\
        return MTR_ERR_INV;                                 \
    }

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error1.3\n");\
        return MTR_ERR_INV;                                 \
    }

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error1.4\n");\
        return MTR_ERR_INV;                                 \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                      \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error2.0\n");\
        return MTR_ERR_INV;                                 \
    }


#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error2.1\n");\
        return MTR_ERR_INV;                                 \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error2.2\n");\
        return -1;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error2.3\n");\
        return -1;                                          \
    }

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error2.4\n");\
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)             \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))                           \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error3\n");  \
        return MTR_ERR_INV;                                 \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)            \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,     \
                       sizeof(type)))                       \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error4.1\n");\
        return MTR_ERR_INV;                                 \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)          \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))                       \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error4.2\n");\
        return MTR_ERR_INV;                                 \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)           \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))                                   \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error6\n");  \
        return MTR_ERR_INV;                                 \
    }                                                       \

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)          \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,     \
                       size))                               \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error7\n");  \
        return MTR_ERR_INV;                                 \
    }
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)        \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       size))                               \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error8\n");  \
        return MTR_ERR_INV;                                 \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)        \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))                                   \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error 4\n"); \
        return MTR_ERR_INV;                                 \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)    \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,    \
                       (size)))                             \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error\n");   \
        return MTR_ERR_INV;                                 \
    }                                                       \
    
#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)       \
    if (copy_to_user((void __user *)ArgUsr, ArgKernel,      \
                       size))                               \
    {                                                       \
        MTCOMMON_PRINT("mtal_ioctl_tz argument error 5\n"); \
        return MTR_ERR_INV;                                 \
    }                                                       \
 
#define MTTZ_AES_DRCRYPT_MAX_LEN        (512 * 1024)
#define MTTZ_AES_DRCRYPT_SEG_MAX_LEN    (24 * sizeof(UINT32))

    
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static Variables
//-----------------------------------------------------------------------------
static HANDLE_T hMtTzSemaHandle = (HANDLE_T)NULL;

static MTTZ_COMMON_ARG_T *gptCommonArg_tz = NULL;
static MTTZ_COMMON_ARG_T gtCommonArg_k;
static UINT8 *pv_decrypt_mem = NULL;
static UINT8 *pv_decrypt_seg = NULL;
static BOOL b_decrypt_init = FALSE;
static BOOL b_mttz_init = FALSE;


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTTZ_COMMON_Operate(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTTZ_COMMON_ARG_T tCommonArg_u, tCommonArg_k, *ptCommonArg_tz = NULL;
    INT32 iRet = MTR_NOT_OK, i;

    /* Argument */
    memset(&tCommonArg_u, 0, sizeof(MTTZ_COMMON_ARG_T));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(MTTZ_COMMON_ARG_T));
    COPY_FROM_USER_ARG_SIZE(arg, &tCommonArg_u, sizeof(MTTZ_COMMON_ARG_T));

    /* Check size */
    if(tCommonArg_u.ui1Size > MTTZ_COMMON_ARG_LENGTH) {
        MTTZ_PRINT("mtal_ioctl_tz ui1Size should <= %d.\n", MTTZ_COMMON_ARG_LENGTH);
        return (MTR_NOT_OK);
    }

    //printk("_MTTZ_COMMON_Operate(), call TZ command %x.\n", tCommonArg_u.ui4Command);

    /* Check, allocate, and copy from user */
    memset(&tCommonArg_k, 0, sizeof(MTTZ_COMMON_ARG_T));
    tCommonArg_k.ui4Command = tCommonArg_u.ui4Command;
    tCommonArg_k.ui1Size = tCommonArg_u.ui1Size;
    for(i = 0; i < tCommonArg_u.ui1Size; i++) {
        tCommonArg_k.ui4Length[i] = tCommonArg_u.ui4Length[i];
        tCommonArg_k.abWrite[i] = tCommonArg_u.abWrite[i];
        /* check */
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_u.ui4Length[i]);
        /* allocate */
        tCommonArg_k.apvArgument[i] = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)tCommonArg_k.ui4Length[i], TZ_CACHE_LINE_SIZE));
        if(!tCommonArg_k.apvArgument[i]) {
            MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory tCommonArg_k.apvArgument[%d] fail.\n", i);
            return (MTR_NOT_OK);
        }
        memset(tCommonArg_k.apvArgument[i], 0, (size_t)tCommonArg_k.ui4Length[i]);
        /* copy from user */
        COPY_FROM_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_k.apvArgument[i], (size_t)tCommonArg_k.ui4Length[i]);
    }
    
    /* Prepare ptCommonArg_tz */
    ptCommonArg_tz = (MTTZ_COMMON_ARG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)sizeof(MTTZ_COMMON_ARG_T), TZ_CACHE_LINE_SIZE));
    memset(ptCommonArg_tz, 0, sizeof(MTTZ_COMMON_ARG_T));
    memcpy(ptCommonArg_tz, &tCommonArg_k, sizeof(MTTZ_COMMON_ARG_T));
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        ptCommonArg_tz->apvArgument[i] = (void *)PHYSICAL((UINT32)ptCommonArg_tz->apvArgument[i]);
    }

    /* Call TZ_NWD_BufferStart() before sending buffer to secure world. */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        TZ_NWD_BufferStart((UINT32)tCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[i]));
    }
    
    /* Call TZ */
    iRet = TZ_TEE_Common_Operate(tCommonArg_k.ui4Command, ptCommonArg_tz, sizeof(MTTZ_COMMON_ARG_T));
    
    /* Call TZ_NWD_BufferEnd() when secure call return */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        if(tCommonArg_k.abWrite[i]) {
            TZ_NWD_BufferEnd((UINT32)tCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[i]));
        }
    }
    
    /* Copy to User */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        if(tCommonArg_k.abWrite[i]) {
            COPY_TO_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_k.apvArgument[i], tCommonArg_k.ui4Length[i]);
        }
    }

    /* Free */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)tCommonArg_k.apvArgument[i]));
    }
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)ptCommonArg_tz));

    if(iRet != 0)
    {
        return (MTR_OK);
    }
    else
    {
        return (MTR_NOT_OK);
    }
#else
    return (MTR_NOT_OK);
#endif
}

static MT_RESULT_T  _MTTZ_DECRYPT_Init(unsigned long arg)
{
    MTTZ_COMMON_ARG_T tCommonArg_u;
    INT32 i;

    if (b_decrypt_init == TRUE)
    {
        return MTR_OK;
    }    

    if (pv_decrypt_mem == NULL || pv_decrypt_seg == NULL)
    {
        /* should not reach here because MTTZ_Init() should be called when MTAL_Init() during system bring-up.
         * However, we still do error checking here.
         */
        MTTZ_Init();
    }

    /* Argument */
    memset(&tCommonArg_u, 0, sizeof(MTTZ_COMMON_ARG_T));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(MTTZ_COMMON_ARG_T));
    COPY_FROM_USER_ARG_SIZE(arg, &tCommonArg_u, sizeof(MTTZ_COMMON_ARG_T));

    /* Check size */
    if(tCommonArg_u.ui1Size> MTTZ_COMMON_ARG_LENGTH) 
	{
        MTTZ_PRINT("mtal_ioctl_tz ui1Size should <= %d.\n", MTTZ_COMMON_ARG_LENGTH);
        return (MTR_NOT_OK);
    }

    /* Prepare ptCommonArg_tz */
    gptCommonArg_tz = (MTTZ_COMMON_ARG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)sizeof(MTTZ_COMMON_ARG_T), TZ_CACHE_LINE_SIZE));
    /* Check, allocate, and copy from user */
    if (gptCommonArg_tz == NULL)
    {
        MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gptCommonArg_tz fail.\n");
        return (MTR_NOT_OK);
    }
    else
    {
        memset(gptCommonArg_tz, 0, sizeof(MTTZ_COMMON_ARG_T));
        memset(&gtCommonArg_k, 0, sizeof(MTTZ_COMMON_ARG_T));

        do
        {
            gtCommonArg_k.ui1Size = MTTZ_AESCTR_DECRYPT_MULTI_CMD_SIZE;

            /* argument 0 (pCtrContext) handling */
            gtCommonArg_k.ui4Length[0]   = tCommonArg_u.ui4Length[0];
            gtCommonArg_k.abWrite[0]     = tCommonArg_u.abWrite[0];
            gtCommonArg_k.apvArgument[0] = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)gtCommonArg_k.ui4Length[0], TZ_CACHE_LINE_SIZE));
            if (gtCommonArg_k.apvArgument[0] == NULL)
            {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[0](size=0x%08X) fail., gtCommonArg_k.ui4Length[0]\n");
                break;
            }

            /* argument 1 (pbData) handling */
            gtCommonArg_k.abWrite[1]     = tCommonArg_u.abWrite[1];
            gtCommonArg_k.apvArgument[1] = pv_decrypt_mem;
            if (gtCommonArg_k.apvArgument[1] == NULL)
            {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[1](size=0x%08X) fail., MTTZ_AES_DRCRYPT_MAX_LEN\n");
                break;
            }

            /* argument 2 (bProtectSignature) handling */
            gtCommonArg_k.ui4Length[2]   = tCommonArg_u.ui4Length[2];
            gtCommonArg_k.abWrite[2]     = tCommonArg_u.abWrite[2];
            gtCommonArg_k.apvArgument[2] = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)gtCommonArg_k.ui4Length[2], TZ_CACHE_LINE_SIZE));
            if (gtCommonArg_k.apvArgument[2] == NULL)
            {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[2](size=0x%08X) fail., gtCommonArg_k.ui4Length[2]\n");
                break;
            }

            /* argument 3 (au4Seg) handling */
            gtCommonArg_k.abWrite[3]     = tCommonArg_u.abWrite[3];
            gtCommonArg_k.apvArgument[3] = pv_decrypt_seg;
            if (gtCommonArg_k.apvArgument[3] == NULL)
            {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[3](size=0x%08X) fail., MTTZ_AES_DRCRYPT_SEG_MAX_LEN\n");
                break;
            }

            /* argument 4 (u8ByteOffset) handling */
            gtCommonArg_k.ui4Length[4]   = sizeof(UINT64);
            gtCommonArg_k.abWrite[4]     = 0;
            gtCommonArg_k.apvArgument[4] = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory(gtCommonArg_k.ui4Length[4], TZ_CACHE_LINE_SIZE));
            if (gtCommonArg_k.apvArgument[4] == NULL)
            {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[4](size=0x%08X) fail., gtCommonArg_k.ui4Length[4]\n");
                break;
            }

            b_decrypt_init = TRUE;
            return MTR_OK;
           
        } while(0);

        /* Should not reach here. It means error occurs */
        
        /* Free */
        for(i = 0; i < MTTZ_AESCTR_DECRYPT_MULTI_CMD_SIZE; i++) 
        {
            if (gtCommonArg_k.apvArgument[i] != NULL)
            {
                BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)gtCommonArg_k.apvArgument[i]));
                gtCommonArg_k.apvArgument[i] = NULL;
            }
        }

        if (gptCommonArg_tz != NULL)
        {
            BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)gptCommonArg_tz));
            gptCommonArg_tz = NULL;
        }

        return (MTR_NOT_OK);
    }       
}

static MT_RESULT_T  _MTTZ_DECRYPT_Operate(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT

    MTTZ_COMMON_ARG_T tCommonArg_u;
    INT32 iRet = MTR_NOT_OK, i;
    UINT32 u4_leftLen = 0;
    UINT64 u8_offset = 0;
	
    if (b_decrypt_init == FALSE)
    {
        /* 
         In case _MTTZ_DECRYPT_Init() is not called before, we do automatic initial progress instead of returning error.
         This is because in some customer projects, their playready code won't trigger _MTTZ_DECRYPT_Init/_MTTZ_DECRYPT_Deinit().
         However, in this case, the memory allocated in _MTTZ_DECRYPT_Init() will not be freed even when playback is end. 
         */
         
         MTTZ_PRINT("_MTTZ_DECRYPT_Init() is not called before using _MTTZ_DECRYPT_Operate(), do automatically init\n");
         
        _MTTZ_DECRYPT_Init(arg);
    }        

    /* Argument */
    memset(&tCommonArg_u, 0, sizeof(MTTZ_COMMON_ARG_T));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(MTTZ_COMMON_ARG_T));
    COPY_FROM_USER_ARG_SIZE(arg, &tCommonArg_u, sizeof(MTTZ_COMMON_ARG_T));

    if (tCommonArg_u.ui4Length[3] > MTTZ_AES_DRCRYPT_SEG_MAX_LEN)
    {
        MTTZ_PRINT("%s error : payload length is too large(%d), the max size is %d\n", 
                    __FUNCTION__, 
                    tCommonArg_u.ui4Length[3], 
                    MTTZ_AES_DRCRYPT_SEG_MAX_LEN);

        return MTR_NOT_OK;
    }
    else
    {
        /* fill payload length in this stage */
        gtCommonArg_k.ui4Length[3] = tCommonArg_u.ui4Length[3];
    }

    /* Copy command value & size */
    gtCommonArg_k.ui4Command = tCommonArg_u.ui4Command;
        
    for(i = 0; i < tCommonArg_u.ui1Size; i++) 
    {
        /* check */
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_u.ui4Length[i]);

        if (i == MTTZ_AESCTR_DECRYPT_DATA_IDX) /* Skip argument 1 copying operation */
        {
            continue;
        }

        memset(gtCommonArg_k.apvArgument[i], 0, (size_t)gtCommonArg_k.ui4Length[i]);
        /* copy from user */
        COPY_FROM_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], gtCommonArg_k.apvArgument[i], (size_t)gtCommonArg_k.ui4Length[i]);
    }

    u8_offset = 0;
    u4_leftLen = tCommonArg_u.ui4Length[1] ;

    do 
    {
        /* Setup offset from u8_offset, and copy payload data from argument 1 */
        memcpy(gtCommonArg_k.apvArgument[4], &u8_offset, sizeof(UINT64));
        gtCommonArg_k.ui4Length[1] = (u4_leftLen < MTTZ_AES_DRCRYPT_MAX_LEN) ? u4_leftLen : MTTZ_AES_DRCRYPT_MAX_LEN;  
        COPY_FROM_USER_ARG_SIZE((tCommonArg_u.apvArgument[1] + u8_offset), gtCommonArg_k.apvArgument[1], (size_t)gtCommonArg_k.ui4Length[1]);
        
        /* Prepare gptCommonArg_tz */
        memset(gptCommonArg_tz, 0, sizeof(MTTZ_COMMON_ARG_T));
        memcpy(gptCommonArg_tz, &gtCommonArg_k, sizeof(MTTZ_COMMON_ARG_T));
        for(i = 0; i < gtCommonArg_k.ui1Size; i++) 
        {
            gptCommonArg_tz->apvArgument[i] = (void *)PHYSICAL((UINT32)gptCommonArg_tz->apvArgument[i]);           
        }

        /* Call TZ_NWD_BufferStart() before sending buffer to secure world. */
        for(i = 0; i < gtCommonArg_k.ui1Size; i++) 
        {
            TZ_NWD_BufferStart((UINT32)gtCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(gtCommonArg_k.ui4Length[i]));
        }

        /* Call TZ */
        iRet = TZ_TEE_Common_Operate(gtCommonArg_k.ui4Command, gptCommonArg_tz, sizeof(MTTZ_COMMON_ARG_T));
        
        /* Call TZ_NWD_BufferEnd() when secure call return */
        for(i = 0; i < gtCommonArg_k.ui1Size; i++) 
        {
            if(gtCommonArg_k.abWrite[i]) 
            {
                TZ_NWD_BufferEnd((UINT32)gtCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(gtCommonArg_k.ui4Length[i]));
            }
        }

        /* copy protected data to user space */
        COPY_TO_USER_ARG_SIZE((tCommonArg_u.apvArgument[1] + u8_offset), gtCommonArg_k.apvArgument[1], gtCommonArg_k.ui4Length[1]);
        u8_offset += gtCommonArg_k.ui4Length[1];
        u4_leftLen -= gtCommonArg_k.ui4Length[1];
    } while(u4_leftLen);
   	
    /* Copy to User */
    for(i = 0; i < gtCommonArg_k.ui1Size; i++) 
    {
        if (i == MTTZ_AESCTR_DECRYPT_DATA_IDX) /* Skip argument 1 copying operation */
        { 
            continue;
        }
		
        if(gtCommonArg_k.abWrite[i]) 
        {
            COPY_TO_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], gtCommonArg_k.apvArgument[i], gtCommonArg_k.ui4Length[i]);
        }
    }

    if(iRet != 0)
    {
        return (MTR_OK);
    }
    else
    {
        return (MTR_NOT_OK);
    }
#else
    return (MTR_NOT_OK);
#endif
}

static MT_RESULT_T  _MTTZ_MSL_Operate(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTTZ_COMMON_ARG_T tCommonArg_u, tCommonArg_k, *ptCommonArg_tz = NULL;
    INT32 iRet = MTR_NOT_OK, i;

    if (b_mttz_init == FALSE) {
        /* In case MTTZ_Init() is not called before, we do automatic initial progress instead of returning error. */
        MTTZ_PRINT("MTTZ_Init() is not called before using _MTTZ_MSL_Operate(), do automatically init\n");
        MTTZ_Init();
    }

    /* Argument */
    memset(&tCommonArg_u, 0, sizeof(MTTZ_COMMON_ARG_T));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(MTTZ_COMMON_ARG_T));
    COPY_FROM_USER_ARG_SIZE(arg, &tCommonArg_u, sizeof(MTTZ_COMMON_ARG_T));

    /* Check size */
    if(tCommonArg_u.ui1Size > 2) {
        MTTZ_PRINT("mtal_ioctl_tz ui1Size should <= %d.\n", 2);
        return (MTR_NOT_OK);
    }

    /* Check req size */
    if (tCommonArg_u.ui4Length[0] > MTTZ_AES_DRCRYPT_MAX_LEN / 2) {
        MTTZ_PRINT("%s error : Req payload length is too large(%d), the max size is %d\n", 
                    __FUNCTION__, 
                    tCommonArg_u.ui4Length[0], 
                    MTTZ_AES_DRCRYPT_MAX_LEN / 2);
        return MTR_NOT_OK;
    }

    /* Check rsp size */
    if (tCommonArg_u.ui4Length[1] > MTTZ_AES_DRCRYPT_MAX_LEN / 2) {
        MTTZ_PRINT("%s error : Rsp payload length is too large(%d), the max size is %d\n", 
                    __FUNCTION__, 
                    tCommonArg_u.ui4Length[1], 
                    MTTZ_AES_DRCRYPT_MAX_LEN / 2);
        return MTR_NOT_OK;
    }

    //printk("_MTTZ_COMMON_Operate(), call TZ command %x.\n", tCommonArg_u.ui4Command);

    /* Prepare common part of tCommonArg_k */
    memset(&tCommonArg_k, 0, sizeof(MTTZ_COMMON_ARG_T));
    tCommonArg_k.ui4Command = tCommonArg_u.ui4Command;
    tCommonArg_k.ui1Size = tCommonArg_u.ui1Size;

    /* Prepare req part of tCommonArg_k */
    tCommonArg_k.ui4Length[0] = tCommonArg_u.ui4Length[0];
    tCommonArg_k.abWrite[0] = tCommonArg_u.abWrite[0];
    tCommonArg_k.apvArgument[0] = pv_decrypt_mem;
    memset(tCommonArg_k.apvArgument[0], 0, (size_t)tCommonArg_k.ui4Length[0]);
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(tCommonArg_u.apvArgument[0], tCommonArg_u.ui4Length[0]);
    COPY_FROM_USER_ARG_SIZE(tCommonArg_u.apvArgument[0], tCommonArg_k.apvArgument[0], (size_t)tCommonArg_k.ui4Length[0]);

    /* Prepare rsp part of tCommonArg_k */
    tCommonArg_k.ui4Length[1] = tCommonArg_u.ui4Length[1];
    tCommonArg_k.abWrite[1] = tCommonArg_u.abWrite[1];
    tCommonArg_k.apvArgument[1] = pv_decrypt_mem + MTTZ_AES_DRCRYPT_MAX_LEN / 2;
    memset(tCommonArg_k.apvArgument[1], 0, (size_t)tCommonArg_k.ui4Length[1]);
    
    /* Prepare ptCommonArg_tz */
    ptCommonArg_tz = (MTTZ_COMMON_ARG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)sizeof(MTTZ_COMMON_ARG_T), TZ_CACHE_LINE_SIZE));
    memset(ptCommonArg_tz, 0, sizeof(MTTZ_COMMON_ARG_T));
    memcpy(ptCommonArg_tz, &tCommonArg_k, sizeof(MTTZ_COMMON_ARG_T));
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        ptCommonArg_tz->apvArgument[i] = (void *)PHYSICAL((UINT32)ptCommonArg_tz->apvArgument[i]);
    }

    /* Call TZ_NWD_BufferStart() before sending buffer to secure world. */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        TZ_NWD_BufferStart((UINT32)tCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[i]));
    }
    
    /* Call TZ */
    iRet = TZ_TEE_Common_Operate(tCommonArg_k.ui4Command, ptCommonArg_tz, sizeof(MTTZ_COMMON_ARG_T));
    
    /* Call TZ_NWD_BufferEnd() for rsp when secure call return */
    TZ_NWD_BufferEnd((UINT32)tCommonArg_k.apvArgument[1], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[1]));
    
    /* Copy rsp part to User */
    COPY_TO_USER_ARG_SIZE(tCommonArg_u.apvArgument[1], tCommonArg_k.apvArgument[1], tCommonArg_k.ui4Length[1]);
    
    /* Free */
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)ptCommonArg_tz));

    if(iRet != 0)
    {
        return (MTR_OK);
    }
    else
    {
        return (MTR_NOT_OK);
    }
#else
    return (MTR_NOT_OK);
#endif
}

static MT_RESULT_T  _MTTZ_DTCP_Operate(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTTZ_COMMON_ARG_T tCommonArg_u, tCommonArg_k, *ptCommonArg_tz = NULL;
    INT32 iRet = MTR_NOT_OK, i;

    if (pv_decrypt_mem == NULL) {
        /* should not reach here because MTTZ_Init() should be called when MTAL_Init() during system bring-up.
         * However, we still do error checking here.
         */
        MTTZ_Init();
    }

    /* Argument */
    memset(&tCommonArg_u, 0, sizeof(MTTZ_COMMON_ARG_T));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, sizeof(MTTZ_COMMON_ARG_T));
    COPY_FROM_USER_ARG_SIZE(arg, &tCommonArg_u, sizeof(MTTZ_COMMON_ARG_T));

    /* Check size */
    if(tCommonArg_u.ui1Size > MTTZ_COMMON_ARG_LENGTH) {
        MTTZ_PRINT("mtal_ioctl_tz ui1Size should <= %d.\n", MTTZ_COMMON_ARG_LENGTH);
        return (MTR_NOT_OK);
    }

    //printk("_MTTZ_COMMON_Operate(), call TZ command %x.\n", tCommonArg_u.ui4Command);

    /* Check, allocate, and copy from user */
    memset(&tCommonArg_k, 0, sizeof(MTTZ_COMMON_ARG_T));
    tCommonArg_k.ui4Command = tCommonArg_u.ui4Command;
    tCommonArg_k.ui1Size = tCommonArg_u.ui1Size;
    for(i = 0; i < tCommonArg_u.ui1Size; i++) {
        tCommonArg_k.ui4Length[i] = tCommonArg_u.ui4Length[i];
        tCommonArg_k.abWrite[i] = tCommonArg_u.abWrite[i];
        /* check */
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_u.ui4Length[i]);
        /* allocate */
        if (i == 0) {
            /* input */
            if (tCommonArg_k.ui4Length[i] > MTTZ_AES_DRCRYPT_MAX_LEN) {
                MTTZ_PRINT("mtal_ioctl_tz _MTTZ_DTCP_Operate BufSize=%u exceeds the limit.\n", tCommonArg_k.ui4Length[i]);
                return (MTR_NOT_OK);
            }

            tCommonArg_k.apvArgument[i] = pv_decrypt_mem;
        }
        else if (i == 2) {
            /* output */
            if (tCommonArg_k.ui4Length[i] > MTTZ_AES_DRCRYPT_MAX_LEN) {
                MTTZ_PRINT("mtal_ioctl_tz _MTTZ_DTCP_Operate BufSize=%u exceeds the limit.\n", tCommonArg_k.ui4Length[i]);
                return (MTR_NOT_OK);
            }

            tCommonArg_k.apvArgument[i] = pv_decrypt_mem;
            continue;
        }
        else {
            tCommonArg_k.apvArgument[i] = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)tCommonArg_k.ui4Length[i], TZ_CACHE_LINE_SIZE));
            if(!tCommonArg_k.apvArgument[i]) {
                MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory tCommonArg_k.apvArgument[%d] fail.\n", i);
                return (MTR_NOT_OK);
            }
            
            memset(tCommonArg_k.apvArgument[i], 0, (size_t)tCommonArg_k.ui4Length[i]);
        }
            /* copy from user */
        COPY_FROM_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_k.apvArgument[i], (size_t)tCommonArg_k.ui4Length[i]);
    }
    
    /* Prepare ptCommonArg_tz */
    ptCommonArg_tz = (MTTZ_COMMON_ARG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory((UINT32)sizeof(MTTZ_COMMON_ARG_T), TZ_CACHE_LINE_SIZE));
    memset(ptCommonArg_tz, 0, sizeof(MTTZ_COMMON_ARG_T));
    memcpy(ptCommonArg_tz, &tCommonArg_k, sizeof(MTTZ_COMMON_ARG_T));
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        ptCommonArg_tz->apvArgument[i] = (void *)PHYSICAL((UINT32)ptCommonArg_tz->apvArgument[i]);
    }

    /* Call TZ_NWD_BufferStart() before sending buffer to secure world. */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        TZ_NWD_BufferStart((UINT32)tCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[i]));
    }
    
    /* Call TZ */
    iRet = TZ_TEE_Common_Operate(tCommonArg_k.ui4Command, ptCommonArg_tz, sizeof(MTTZ_COMMON_ARG_T));
    
    /* Call TZ_NWD_BufferEnd() when secure call return */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        if(tCommonArg_k.abWrite[i]) {
            TZ_NWD_BufferEnd((UINT32)tCommonArg_k.apvArgument[i], TZ_CACHE_ALIGN(tCommonArg_k.ui4Length[i]));
        }
    }
    
    /* Copy to User */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        if(tCommonArg_k.abWrite[i]) {
            COPY_TO_USER_ARG_SIZE(tCommonArg_u.apvArgument[i], tCommonArg_k.apvArgument[i], tCommonArg_k.ui4Length[i]);
        }
    }

    /* Free */
    for(i = 0; i < tCommonArg_k.ui1Size; i++) {
        if (i == 0 || i == 2)
            continue;
        
        BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)tCommonArg_k.apvArgument[i]));
    }
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)ptCommonArg_tz));

    if(iRet != 0)
    {
        return (MTR_OK);
    }
    else
    {
        return (MTR_NOT_OK);
    }
#else
    return (MTR_NOT_OK);
#endif
}

static MT_RESULT_T _MTTZ_DECRYPT_Deinit(void)
{
#if 0
    INT32 i;
#endif
    if (!b_decrypt_init)
    {
        return MTR_OK;
    }
#if 0
    /* Free */
    for(i = 0; i < gtCommonArg_k.ui1Size; i++) 
    {
        if (gtCommonArg_k.apvArgument[i] != NULL)
        {
            BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)gtCommonArg_k.apvArgument[i]));
        }
    }
    memset(&gtCommonArg_k, 0, sizeof(MTTZ_COMMON_ARG_T));
    
    if (gptCommonArg_tz != NULL)
    {
        BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)gptCommonArg_tz));
        gptCommonArg_tz = NULL;
    }

    b_decrypt_init = FALSE;
#endif
    return MTR_OK;
}


int mtal_ioctl_mttz(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int result = MTR_OK;

    if ( hMtTzSemaHandle == (HANDLE_T)NULL )
    {
        if ( x_sema_create(&hMtTzSemaHandle, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) != 0 )
        {
            return MTR_NOT_OK;
        }
    }

    x_sema_lock(hMtTzSemaHandle, X_SEMA_OPTION_WAIT);

    switch (cmd)
    {
        case MTAL_IO_TZ_COMMON_OPERATE:
            result = _MTTZ_COMMON_Operate(arg);
            break;

        case MTAL_IO_TZ_DECRYPT_INIT:
            result = _MTTZ_DECRYPT_Init(arg);
            break;

        case MTAL_IO_TZ_DECRYPT_OPERATE:
            result = _MTTZ_DECRYPT_Operate(arg);
            break;

        case MTAL_IO_TZ_DECRYPT_DEINIT:
            result = _MTTZ_DECRYPT_Deinit();
            break;

        case MTAL_IO_TZ_MSL_OPERATE:
            result = _MTTZ_MSL_Operate(arg);
            break;

        case MTAL_IO_TZ_DTCP_OPERATE:
            result = _MTTZ_DTCP_Operate(arg);
            break;
                
        default:
            result = MTR_NOT_OK;
            break;
    }

    x_sema_unlock(hMtTzSemaHandle);

    return result;
}

MT_RESULT_T  MTTZ_Init(void)
{
    if (pv_decrypt_mem == NULL)
    {
        pv_decrypt_mem = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTTZ_AES_DRCRYPT_MAX_LEN, TZ_CACHE_LINE_SIZE));
    }

    if (pv_decrypt_seg == NULL)
    {
        pv_decrypt_seg = (UINT8 *)VIRTUAL(BSP_AllocAlignedDmaMemory(MTTZ_AES_DRCRYPT_SEG_MAX_LEN, TZ_CACHE_LINE_SIZE));
    }

    if (pv_decrypt_mem == NULL || pv_decrypt_seg == NULL)
    {
        MTTZ_PRINT("mtal_ioctl_tz BSP_AllocAlignedDmaMemory gtCommonArg_k.apvArgument[1](size=0x%08X) fail., MTTZ_AES_DRCRYPT_MAX_LEN\n");
        return (MTR_NOT_OK);
    }
    else
    {
	    b_mttz_init = TRUE;
        return (MTR_OK);
    }
}


