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
 $Revision: #1 $
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
#include <linux/mtal_ioctl.h>
#include "mtdrm_wv.h"
#include "tz_if.h"
#include "x_bim.h"


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)         \
    if (!access_ok(VERIFY_READ, (void __user *)arg,       \
                size))                                 \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}                                                     \

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)        \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                size))                             \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)      \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr, \
                size))                             \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}
#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)      \
    if (!access_ok(VERIFY_READ, (void __user *)arg,       \
                size))                                 \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}                                                     \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)  \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,  \
                (size)))                           \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}                                                     \

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)     \
    if (copy_to_user((void __user *)ArgUsr, ArgKernel,    \
                size))                             \
{                                                     \
    ret =  MTR_ERR_INV;                               \
    goto final;                                       \
}                                                     \



//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


#define  KEY_CONTROL_SIZE   16
#define  KEY_IV_SIZE        16
#define  KEY_PAD_SIZE       16
#define  KEY_SIZE           16
#define  MAC_KEY_SIZE       32


#define CC_WIDEVINE_TZ_ENABLE

#ifdef CC_WIDEVINE_TZ_ENABLE    

#define x_mem_alloc(size) (void*)VIRTUAL(BSP_AllocAlignedDmaMemory(((size)+TZ_CACHE_LINE_SIZE_MSK)&~(TZ_CACHE_LINE_SIZE_MSK), TZ_CACHE_LINE_SIZE)) 
#define x_mem_free(addr)  BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)(addr))) 

#endif

#define CHECK_ALLOC(x,size, retval,togo) \
    (x) = x_mem_alloc(size); \
    if((x) ==NULL){           \
        (retval) = MTR_BUFFER_FULL; \
        goto togo;          \
    }

#define CHECK_FREE(x) do{if((x)!=NULL) x_mem_free((x));}while(0)

#define VDEC_ALIGN 0x8000

extern void TZ_NWD_BufferStart(UINT32 u4VAddr, UINT32 u4Size);
extern void TZ_NWD_BufferEnd(UINT32 u4VAddr, UINT32 u4Size);

static inline void TZ_CACHE_STR(void* u4VAddr, size_t u4Size)
{
    if(u4VAddr == 0) return ;
    TZ_NWD_BufferStart((UINT32)u4VAddr, TZ_CACHE_ALIGN(u4Size));
}

static inline void TZ_CACHE_END(void* u4VAddr, size_t u4Size)
{
    if(u4VAddr == 0) return ;
    TZ_NWD_BufferEnd((UINT32)u4VAddr, TZ_CACHE_ALIGN(u4Size));
}

//-----------------------------------------------------------------------------
// Static Variables
//-----------------------------------------------------------------------------

static HANDLE_T _hWidevineSema;
static HANDLE_T _hSecbufSema;


//-----------------------------------------------------------------------------
// Secure Buffer Allocation 
//-----------------------------------------------------------------------------


// Must Match to TZ Definition Manually

#define SEC_BUF_ANONYMOUS_SESSION		0x0
#define SEC_BUF_ANONYMOUS_HANDLE		0x0

#define SEC_BUF_FLAG_IS_CYCLIC_BUFFER	0x1 

typedef struct
{
    UINT32 u4Session;
    UINT32 u4Size;        //Size to be alloc
    UINT32 u4Flag;        //Ctrl flag
	UINT32 *u4Handle;     //return handle of the secure buf
} TZ_DRM_SEC_BUF_ALLOC_T;

typedef struct
{
    UINT32 u4Session;
	UINT32 u4Handle;
	UINT32 *u4Fragment;	
	UINT32 u4Size;
} TZ_DRM_SEC_BUF_FRAGMENT_ALLOC_T;

typedef struct
{
    UINT32 u4Session;
	UINT32 u4Handle;
	UINT32 u4Fragment;	
	UINT32 u4Size;
} TZ_DRM_SEC_BUF_FRAGMENT_FREE_T;

typedef struct
{
    UINT32 *pu4Des;
    UINT32 u4Src;
    UINT32 *pu4Size;
    UINT32 u4RDP;        
} TZ_DRM_SEC_BUF_CPB_MOVEDATA_T;

typedef struct
{
    UINT32 u4SecureSrc;
    UINT8 *pu4NormalDes;
    UINT32 len;        
} TZ_DRM_SEC_BUF_CPB_GETDATA_T;


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

static inline bool RangeCheck(const uint8_t* message,
        uint32_t message_length,
        const uint8_t* field,
        uint32_t field_length,
        bool allow_null) {
    if (field == NULL) return allow_null;
    if (field < message) return false;
    if (field + field_length > message + message_length) return false;
    return true;
}

//===================================
// Secure Buffer Allocation 
//===================================

MT_RESULT_T _MTDRM_RPMB_LOAD_MAC_KEY(unsigned long arg)
{
    RpmbLoadKeyArg rArg, *tzArg = NULL;

    MT_RESULT_T ret     = MTR_OK;
    uint32_t *result    = NULL;
    uint8_t *key        = NULL;

    CHECK_ALLOC(result,sizeof(uint32_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(RpmbLoadKeyArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(RpmbLoadKeyArg));
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(uint32_t));

    CHECK_ALLOC(key,rArg.keySize,ret,final);    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.key,rArg.keySize);
    COPY_FROM_USER_ARG_K(rArg.key, *key, rArg.keySize);

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result  = (uint32_t* )PHYSICAL((UINT32)result);
    tzArg->key = (uint8_t* )PHYSICAL((UINT32)key);

    TZ_CACHE_STR(result  , sizeof(uint32_t));
    TZ_CACHE_STR(key , rArg.keySize);
    TZ_RPMB_LOAD_MAC_KEY(tzArg , sizeof(rArg));
    TZ_CACHE_END(result  , sizeof(uint32_t));
    TZ_CACHE_END(key , rArg.keySize);

    COPY_TO_USER_ARG_K(rArg.result,  *result ,sizeof(uint32_t));
    
final:

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(key);

    return ret;
}

MT_RESULT_T _MTDRM_RPMB_GET_WCNT_REQ(unsigned long arg)
{
    RpmGetWriteCntReqArg rArg, *tzArg = NULL;

    MT_RESULT_T ret     = MTR_OK;
    uint32_t *result    = NULL;
    RPMB_DATAFRAM *dFrm        = NULL;

    CHECK_ALLOC(result,sizeof(uint32_t),ret,final);
    CHECK_ALLOC(dFrm,sizeof(RPMB_DATAFRAM),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(RpmGetWriteCntReqArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(RpmGetWriteCntReqArg));
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(uint32_t));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrm,sizeof(RPMB_DATAFRAM));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result  = (uint32_t* )PHYSICAL((UINT32)result);
    tzArg->dFrm = (RPMB_DATAFRAM* )PHYSICAL((UINT32)dFrm);

    TZ_CACHE_STR(result  , sizeof(uint32_t));
    TZ_CACHE_STR(dFrm , sizeof(RPMB_DATAFRAM));
    TZ_RPMB_GET_WCNT_REQ(tzArg , sizeof(rArg));
    TZ_CACHE_END(result  , sizeof(uint32_t));
    TZ_CACHE_END(dFrm , sizeof(RPMB_DATAFRAM));

    COPY_TO_USER_ARG_K(rArg.result,  *result ,sizeof(uint32_t));
    COPY_TO_USER_ARG_K(rArg.dFrm,  *dFrm ,sizeof(RPMB_DATAFRAM));
    
final:

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(dFrm);

    return ret;
}

MT_RESULT_T _MTDRM_RPMB_SET_WCNT_RESP(unsigned long arg)
{
    RpmSetWriteCntRespArg rArg, *tzArg = NULL;

    MT_RESULT_T ret     = MTR_OK;
    uint32_t *result    = NULL;
    RPMB_DATAFRAM *dFrm        = NULL;

    CHECK_ALLOC(result,sizeof(uint32_t),ret,final);
    CHECK_ALLOC(dFrm,sizeof(RPMB_DATAFRAM),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(RpmSetWriteCntRespArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(RpmSetWriteCntRespArg));
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(uint32_t));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrm,sizeof(RPMB_DATAFRAM));

    COPY_FROM_USER_ARG_K(rArg.dFrm, *dFrm, sizeof(RPMB_DATAFRAM));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result  = (uint32_t* )PHYSICAL((UINT32)result);
    tzArg->dFrm = (RPMB_DATAFRAM* )PHYSICAL((UINT32)dFrm);

    TZ_CACHE_STR(result  , sizeof(uint32_t));
    TZ_CACHE_STR(dFrm , sizeof(RPMB_DATAFRAM));
    TZ_RPMB_SET_WCNT_RESP(tzArg , sizeof(rArg));
    TZ_CACHE_END(result  , sizeof(uint32_t));
    TZ_CACHE_END(dFrm , sizeof(RPMB_DATAFRAM));

    COPY_TO_USER_ARG_K(rArg.result,  *result ,sizeof(uint32_t));
    
final:

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(dFrm);

    return ret;
}

UINT32 u4SecureBufferAlloc(size_t size)
{
#if 0
    return (UINT32)BSP_AllocAlignedDmaMemory(size, VDEC_ALIGN );
#else

    TZ_DRM_SEC_BUF_ALLOC_T *arg;
    UINT32 u4Handle;
    UINT32 *pu4Handle;

    arg = x_mem_alloc(sizeof(TZ_DRM_SEC_BUF_ALLOC_T));
    pu4Handle = x_mem_alloc(sizeof(TZ_DRM_SEC_BUF_ALLOC_T));    

    arg->u4Session = 0;
    arg->u4Size    = size;
    arg->u4Flag    = 0;
    arg->u4Handle  = (UINT32*)PHYSICAL((UINT32)pu4Handle);

    TZ_CACHE_STR(pu4Handle , sizeof(UINT32));
    TZ_SEC_BUF_ALLOC(arg , sizeof(TZ_DRM_SEC_BUF_ALLOC_T));
    TZ_CACHE_END(pu4Handle , sizeof(UINT32));

    u4Handle = *pu4Handle;

    x_mem_free(arg);
    x_mem_free(pu4Handle);

    return u4Handle;

#endif    

}

MT_RESULT_T _MTDRM_SECURE_ALLOC(unsigned long arg)
{
    SecureAllocArg rArg;
    void *addr=NULL;
    MT_RESULT_T ret = MTR_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(SecureAllocArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(SecureAllocArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.handle,sizeof(SECURE_BUF_HANDLE));

    //addr = (void *)PHYSICAL((UINT32)x_mem_alloc(rArg.size));
    //addr = (void *)BSP_AllocAlignedDmaMemory(rArg.size, VDEC_ALIGN );
    addr = (void *)u4SecureBufferAlloc(rArg.size);

    COPY_TO_USER_ARG_K(rArg.handle, addr,sizeof(SECURE_BUF_HANDLE));

final:
    return ret;
}

BOOL bSecureBufferFree(UINT32 u4Handle)
{
#if 0
    return BSP_FreeAlignedDmaMemory(u4Handle);
#else
    UINT32 *rel;

    rel = x_mem_alloc(sizeof(UINT32));
    *rel = u4Handle;
    TZ_SEC_BUF_FREE(rel , sizeof(UINT32));
    x_mem_free(rel);
    
    return 0;
#endif
}

MT_RESULT_T _MTDRM_SECURE_FREE(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    if(arg !=(unsigned long) NULL)
    {
        //x_mem_free((void *)VIRTUAL(arg));
        //BSP_FreeAlignedDmaMemory(arg);
        bSecureBufferFree(arg);
    }
    return ret;
}

MT_RESULT_T _MTDRM_SECURE_DATAMOVE(unsigned long arg)
{
    SecureDataMoveArg rArg;
    TZ_DRM_SEC_BUF_CPB_MOVEDATA_T *tzArg;
    MT_RESULT_T ret = MTR_OK;

    UINT32 *pu4Des=NULL;
    UINT32 *pu4Size=NULL;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(rArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(rArg));

    CHECK_ALLOC(tzArg,sizeof(*tzArg),ret,final);
    CHECK_ALLOC(pu4Des,sizeof(UINT32),ret,final);
    CHECK_ALLOC(pu4Size,sizeof(UINT32),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.Des,sizeof(SECURE_BUF_HANDLE));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.Size,sizeof(size_t));

    COPY_FROM_USER_ARG_K(rArg.Des, *pu4Des, sizeof(SECURE_BUF_HANDLE));
    COPY_FROM_USER_ARG_K(rArg.Size,* pu4Size, sizeof(size_t));

    tzArg->pu4Des  = (UINT32*)PHYSICAL((UINT32)pu4Des);
    tzArg->u4Src   = rArg.Src; 
    tzArg->pu4Size = (UINT32*)PHYSICAL((UINT32)pu4Size);
    tzArg->u4RDP   = rArg.RDP;

    TZ_CACHE_STR(pu4Des , sizeof(UINT32));
    TZ_CACHE_STR(pu4Size , sizeof(UINT32));
    TZ_SEC_BUF_CPB_MOVEDATA(tzArg, sizeof(*tzArg));
    TZ_CACHE_END(pu4Des , sizeof(UINT32));
    TZ_CACHE_END(pu4Size , sizeof(UINT32));

    COPY_TO_USER_ARG_K(rArg.Des , *pu4Des ,sizeof(SECURE_BUF_HANDLE));
    COPY_TO_USER_ARG_K(rArg.Size, *pu4Size,sizeof(size_t));

final:

    CHECK_FREE(tzArg);
    CHECK_FREE(pu4Des);
    CHECK_FREE(pu4Size);

    return ret;
}

MT_RESULT_T _MTDRM_SECURE_GETDATA(unsigned long arg)
{
    SecureGetDataArg rArg;
    TZ_DRM_SEC_BUF_CPB_GETDATA_T *tzArg;
    MT_RESULT_T ret = MTR_OK;

    UINT32 *pu4NormalDes;
    UINT32 tzRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(rArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(rArg));

    CHECK_ALLOC(tzArg,sizeof(*tzArg),ret,final);
    CHECK_ALLOC(pu4NormalDes,rArg.len,ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.pu4NormalDes,rArg.len);

    tzArg->u4SecureSrc = rArg.u4SecureSrc;
    tzArg->pu4NormalDes  = (UINT8*)PHYSICAL((UINT32)pu4NormalDes);
    tzArg->len = rArg.len;

    TZ_CACHE_STR(pu4NormalDes , rArg.len);
    tzRet = TZ_SEC_BUF_CPB_GETDATA(tzArg, sizeof(*tzArg));
    TZ_CACHE_END(pu4NormalDes , rArg.len);

    if (tzRet!=true)
        ret = MTR_NOT_OK;
    COPY_TO_USER_ARG_K(rArg.pu4NormalDes, *pu4NormalDes,rArg.len);
final:

    CHECK_FREE(tzArg);
    CHECK_FREE(pu4NormalDes);

    return ret;
}

static SECURE_BUF_HANDLE u4rdPoint;
static UINT32 u4rdSize;
static char u1InitDataMove=0;

int bSecureCpbDataMoveDone(UINT32 vdecHandle ,SECURE_BUF_HANDLE rdPoint ,UINT32 rdSize)
{    
    if (!u1InitDataMove)
    {
        x_sema_create(&_hSecbufSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1InitDataMove = 1 ;
    }
    
    x_sema_lock(_hSecbufSema, X_SEMA_OPTION_WAIT) ;
    u4rdSize = rdSize;
    u4rdPoint = rdPoint;    
    x_sema_unlock(_hSecbufSema);
    
    return MTR_OK;
}

MT_RESULT_T _MTDRM_SECURE_CPB_DATAMOVE_DONE(unsigned long arg)
{
    SecureDataMoveDoneArg rArg;
    MT_RESULT_T ret = MTR_OK;
   
    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(rArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(rArg));
    bSecureCpbDataMoveDone(rArg.vdecHandle, rArg.rdPoint, rArg.rdSize);

final:    
    return ret;
}

int bSecureCpbDataMoveInfo(UINT32 vdecHandle ,SECURE_BUF_HANDLE *rdPoint , UINT32 *rdSize)
{
    if (!u1InitDataMove)
    {
        x_sema_create(&_hSecbufSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1InitDataMove = 1 ;
    }
    
    x_sema_lock(_hSecbufSema, X_SEMA_OPTION_WAIT) ;

    *rdPoint = u4rdPoint;
    *rdSize = u4rdSize;

    x_sema_unlock(_hSecbufSema);
    
    return MTR_OK;   
}

MT_RESULT_T _MTDRM_SECURE_CPB_DATAMOVE_INFO(unsigned long arg)
{
    SecureDataMoveInfoArg rArg;
    SECURE_BUF_HANDLE rdPoint;
    UINT32 rdSize;
    MT_RESULT_T ret = MTR_OK;
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(rArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(rArg));
    bSecureCpbDataMoveInfo(rArg.vdecHandle, &rdPoint, &rdSize);
    COPY_TO_USER_ARG_K(rArg.rdPoint, rdPoint,sizeof(rdPoint));
    COPY_TO_USER_ARG_K(rArg.rdSize, rdSize,sizeof(rdSize));

final:    
    return ret;

}

BOOL bSecureBufferFragmentAlloc(UINT32 u4Session , UINT32 u4Handle, UINT32 *u4Fragment, UINT32 u4Size)
{
	TZ_DRM_SEC_BUF_FRAGMENT_ALLOC_T *rArg=NULL;

    MT_RESULT_T ret         = MTR_OK;
    UINT32 *rFrg = NULL;

    CHECK_ALLOC(rArg,sizeof(rArg),ret,final);
    CHECK_ALLOC(rFrg,sizeof(UINT32),ret,final);

	rArg->u4Session = u4Session;
	rArg->u4Handle = u4Handle;
	rArg->u4Fragment = rFrg;	
	rArg->u4Size = u4Size;

	TZ_CACHE_STR(rArg->u4Fragment , sizeof(UINT32));
	ret =  TZ_SEC_BUF_FRAGMENT_ALLOC(&rArg , sizeof(TZ_DRM_SEC_BUF_FRAGMENT_ALLOC_T));
	TZ_CACHE_END(rArg->u4Fragment , sizeof(UINT32));

final:

	if(ret == MTR_OK)
	{
		*u4Fragment = *rFrg;
	}
    
    CHECK_FREE(rArg);
    CHECK_FREE(rFrg);

    return ret;
}

BOOL bSecureBufferFragmentFree(UINT32 u4Session , UINT32 u4Handle, UINT32 u4Fragment, UINT32 u4Size)
{
	TZ_DRM_SEC_BUF_FRAGMENT_FREE_T *rArg=NULL;

    MT_RESULT_T ret         = MTR_OK;
    CHECK_ALLOC(rArg,sizeof(rArg),ret,final);

	rArg->u4Session = u4Session;
	rArg->u4Handle = u4Handle;
	rArg->u4Fragment = u4Fragment;	
	rArg->u4Size = u4Size;

	ret =  TZ_SEC_BUF_FRAGMENT_FREE(&rArg , sizeof(TZ_DRM_SEC_BUF_FRAGMENT_FREE_T));
final:
    
    CHECK_FREE(rArg);

    return ret;
}

//===================================
// WV Level 1 Wrapper
//===================================

static char u1Initialized = 0;

MT_RESULT_T _MTDRM_WV_OEMCrypto_Initialize(unsigned long arg)
{
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(OEMCryptoResult));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    TZ_OEMCrypto_Initialize(result, sizeof(OEMCryptoResult));
#else
    *result = OEMCrypto_Initialize();
#endif    

    COPY_TO_USER_ARG_K(arg, *result, sizeof(OEMCryptoResult));
final:
    
    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_Terminate(unsigned long arg)
{
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);

    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
#ifdef CC_WIDEVINE_TZ_ENABLE    
    TZ_OEMCrypto_Terminate(result,sizeof(OEMCryptoResult));
#else
    *result = OEMCrypto_Terminate();
#endif    

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(arg, *result,sizeof(OEMCryptoResult));
final:

    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_OpenSession(unsigned long arg)
{
    MT_RESULT_T ret             = MTR_OK;
    OEMCryptoResult *result     = NULL;
    OEMCrypto_SESSION *session  = NULL;
    OpenSessionArg rArg, *tzArg = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(session,sizeof(OEMCrypto_SESSION),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(OpenSessionArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(OpenSessionArg));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.session,sizeof(OEMCrypto_SESSION));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result  = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->session = (OEMCrypto_SESSION* )PHYSICAL((UINT32)session);

    TZ_CACHE_STR(result  , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(session , sizeof(OEMCrypto_SESSION));
    TZ_OEMCrypto_OpenSession(tzArg , sizeof(rArg));
    TZ_CACHE_END(result  , sizeof(OEMCryptoResult));
    TZ_CACHE_END(session , sizeof(OEMCrypto_SESSION));

#else
    *result = OEMCrypto_OpenSession(session);
#endif    

    COPY_TO_USER_ARG_K(rArg.result,  *result ,sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.session, *session,sizeof(OEMCrypto_SESSION));
final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(session);

    x_sema_unlock(_hWidevineSema);

    return ret;
}


MT_RESULT_T _MTDRM_WV_OEMCrypto_CloseSession(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    OEMCryptoResult *result      = NULL;
    CloseSessionArg rArg, *tzArg = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,sizeof(CloseSessionArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(OpenSessionArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(OEMCryptoResult));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result = (OEMCryptoResult* )PHYSICAL((UINT32)result);

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_CloseSession(tzArg , sizeof(rArg));
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
#else
    *result = OEMCrypto_CloseSession(rArg.session);
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result,sizeof(OEMCryptoResult));
final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateDerivedKeys(unsigned long arg)
{
    MT_RESULT_T ret                     = MTR_OK;
    GenerateDerivedKeysArg rArg, *tzArg = NULL;
    OEMCryptoResult *result             = NULL;
    uint8_t *mac_key_context            = NULL;
    uint8_t *enc_key_context            = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenerateDerivedKeysArg));
    COPY_FROM_USER_ARG_K(arg , rArg , sizeof(GenerateDerivedKeysArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.mac_key_context, rArg.mac_key_context_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.enc_key_context, rArg.enc_key_context_length);

    CHECK_ALLOC(mac_key_context ,rArg.mac_key_context_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.mac_key_context , *mac_key_context , rArg.mac_key_context_length);

    CHECK_ALLOC(enc_key_context ,rArg.enc_key_context_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.enc_key_context , *enc_key_context , rArg.enc_key_context_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result          = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->mac_key_context = (uint8_t *)PHYSICAL((UINT32)mac_key_context);
    tzArg->enc_key_context = (uint8_t *)PHYSICAL((UINT32)enc_key_context);

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(mac_key_context, rArg.mac_key_context_length);;
    TZ_CACHE_STR(enc_key_context, rArg.enc_key_context_length);;

    TZ_OEMCrypto_GenerateDerivedKeys(tzArg , sizeof(rArg));

    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
    TZ_CACHE_END(mac_key_context, rArg.mac_key_context_length);;
    TZ_CACHE_END(enc_key_context, rArg.enc_key_context_length);;
#else
    *result = OEMCrypto_GenerateDerivedKeys(rArg.session,
            mac_key_context, rArg.mac_key_context_length,
            enc_key_context, rArg.enc_key_context_length);
#endif

    COPY_TO_USER_ARG_K(rArg.result, *result,sizeof(OEMCryptoResult));

final :

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(mac_key_context);
    CHECK_FREE(enc_key_context);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateNonce(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    GenerateNonceArg rArg, *tzArg = NULL;
    OEMCryptoResult *result       = NULL;
    uint32_t *nonce               = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
    CHECK_ALLOC(nonce,sizeof(uint32_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenerateNonceArg));
    COPY_FROM_USER_ARG_K(arg , rArg , sizeof(GenerateNonceArg));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.nonce, sizeof(uint32_t));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->nonce  = (uint32_t *)PHYSICAL((UINT32)nonce);

    TZ_CACHE_STR(nonce  , sizeof(uint32_t));
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_GenerateNonce(tzArg, sizeof(rArg));
    TZ_CACHE_END(nonce  , sizeof(uint32_t));
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
#else
    *result =  OEMCrypto_GenerateNonce(rArg.session,nonce);
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result , sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.nonce , *nonce  , sizeof(uint32_t));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(nonce);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateSignature(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GenerateSignatureArg rArg, *tzArg = NULL;
    OEMCryptoResult *result           = NULL;
    uint8_t* message                  = NULL;
    uint8_t* signature                = NULL;
    size_t *signature_length          = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenerateSignatureArg));
    COPY_FROM_USER_ARG_K(arg,rArg, sizeof(GenerateSignatureArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(signature_length,sizeof(size_t),ret,final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature_length, sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.signature_length,*signature_length, sizeof(size_t));

    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *result = OEMCrypto_ERROR_SHORT_BUFFER;
        *signature_length = SHA256_DIGEST_LENGTH;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message, rArg.message_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature, *signature_length);

    CHECK_ALLOC(message,rArg.message_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.message , *message , rArg.message_length);

    CHECK_ALLOC(signature ,*signature_length, ret, final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result           = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->message          = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature        = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->signature_length = (size_t *)PHYSICAL((UINT32)signature_length);


    invDataLen = *signature_length;

    TZ_CACHE_STR(message          , rArg.message_length);
    TZ_CACHE_STR(signature_length , sizeof(size_t));
    TZ_CACHE_STR(result           , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(signature_length , sizeof(size_t));
    TZ_CACHE_STR(signature         , *signature_length);

    TZ_OEMCrypto_GenerateSignature(tzArg, sizeof(rArg));

    TZ_CACHE_END(message          , rArg.message_length);
    TZ_CACHE_END(signature_length , sizeof(size_t));
    TZ_CACHE_END(result           , sizeof(OEMCryptoResult));
    TZ_CACHE_END(signature_length , sizeof(size_t));
    TZ_CACHE_END(signature         , invDataLen);


#else
    *result = OEMCrypto_GenerateSignature(rArg.session,
            message, rArg.message_length, 
            signature, signature_length);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.signature, *signature , *signature_length);
    }

result:

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.signature_length, *signature_length , sizeof(size_t));


final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(signature_length);
    CHECK_FREE(message);
    CHECK_FREE(signature);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_LoadKeys(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    LoadKeysArg rArg, *tzArg       = NULL;
    OEMCryptoResult *result        = NULL;
    uint8_t* message               = NULL;
    uint8_t* signature             = NULL;
    uint8_t* enc_mac_keys_iv       = NULL;
    uint8_t* enc_mac_keys          = NULL;
    OEMCrypto_KeyObject* key_array = NULL;
    uint8_t* pst                   = NULL;

    uint32_t i;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(LoadKeysArg));
    COPY_FROM_USER_ARG_K(arg,rArg , sizeof(LoadKeysArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));


    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message, rArg.message_length);
    CHECK_ALLOC(message ,rArg.message_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.message , *message, rArg.message_length);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature, rArg.signature_length);
    CHECK_ALLOC(signature ,rArg.signature_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.signature , *signature , rArg.signature_length);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.key_array, sizeof(OEMCrypto_KeyObject)*rArg.num_keys);
    CHECK_ALLOC(key_array ,sizeof(OEMCrypto_KeyObject)*rArg.num_keys, ret, final);


    // Range check
    if (!RangeCheck(rArg.message, rArg.message_length, rArg.enc_mac_keys, 2*MAC_KEY_SIZE, true) ||
        !RangeCheck(rArg.message, rArg.message_length, rArg.enc_mac_keys_iv,  KEY_IV_SIZE, true) ||
        !RangeCheck(rArg.message, rArg.message_length, rArg.pst, rArg.pst_length, true) 
        ) 
    {
        *result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto result;
    }

    for(i=0;i<rArg.num_keys;++i)
    {
        if (!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_id, rArg.key_array[i].key_id_length, false) 
          ||!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_data, rArg.key_array[i].key_data_length, false) 
          ||!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_data_iv,   KEY_IV_SIZE, false) 
          ||!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_control,   KEY_CONTROL_SIZE, false) 
          ||!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_control_iv, KEY_IV_SIZE, false)) {
            *result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            goto result;

        }
    
        key_array[i].key_id          = rArg.key_array[i].key_id             - rArg.message + message;
        key_array[i].key_id_length   = rArg.key_array[i].key_id_length ;
        key_array[i].key_data_iv     = rArg.key_array[i].key_data_iv        - rArg.message + message;
        key_array[i].key_data        = rArg.key_array[i].key_data           - rArg.message + message;
        key_array[i].key_data_length = rArg.key_array[i].key_data_length ;;
        key_array[i].key_control_iv  = rArg.key_array[i].key_control_iv     - rArg.message + message;
        key_array[i].key_control     = rArg.key_array[i].key_control        - rArg.message + message;
    }

    pst = (rArg.pst ==NULL)? NULL : rArg.pst - rArg.message + message;

    enc_mac_keys_iv  = (rArg.enc_mac_keys_iv == NULL ) ? NULL : rArg.enc_mac_keys_iv - rArg.message + message;
    enc_mac_keys     = (rArg.enc_mac_keys==NULL) ? NULL :rArg.enc_mac_keys - rArg.message + message;

#ifdef CC_WIDEVINE_TZ_ENABLE    

    for(i=0;i<rArg.num_keys;++i)
    {
        key_array[i].key_id           = (uint8_t*)PHYSICAL((UINT32)key_array[i].key_id          );
        key_array[i].key_data_iv      = (uint8_t*)PHYSICAL((UINT32)key_array[i].key_data_iv     );
        key_array[i].key_data         = (uint8_t*)PHYSICAL((UINT32)key_array[i].key_data        );
        key_array[i].key_control_iv   = (uint8_t*)PHYSICAL((UINT32)key_array[i].key_control_iv  );
        key_array[i].key_control      = (uint8_t*)PHYSICAL((UINT32)key_array[i].key_control     );
    }

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result          = (OEMCryptoResult* )PHYSICAL((UINT32)result);;
    tzArg->message         = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature       = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->pst             = (uint8_t *)PHYSICAL((UINT32)pst);
    tzArg->enc_mac_keys_iv = (uint8_t *)PHYSICAL((UINT32)enc_mac_keys_iv);
    tzArg->enc_mac_keys    = (uint8_t *)PHYSICAL((UINT32)enc_mac_keys);
    tzArg->key_array       = (OEMCrypto_KeyObject*)PHYSICAL((UINT32)key_array);

    TZ_CACHE_STR(message   , rArg.message_length);
    TZ_CACHE_STR(signature , rArg.signature_length);
    //TZ_CACHE_STR(pst       , rArg.pst_length);
    TZ_CACHE_STR(key_array , sizeof(OEMCrypto_KeyObject)*rArg.num_keys);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_LoadKeys(tzArg, sizeof(rArg));
    TZ_CACHE_END(message   , rArg.message_length);
    TZ_CACHE_END(signature , rArg.signature_length);
    //TZ_CACHE_END(pst       , rArg.pst_length);
    TZ_CACHE_END(key_array , sizeof(OEMCrypto_KeyObject)*rArg.num_keys);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

#else
    *result =  OEMCrypto_LoadKeys(rArg.session,
            message, rArg.message_length,
            signature, rArg.signature_length,
            enc_mac_keys_iv, enc_mac_keys,
            rArg.num_keys, key_array);
#endif            

result:

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(message);
    CHECK_FREE(signature);
    CHECK_FREE(key_array);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_RefreshKeys(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    RefreshKeysArg rArg, *tzArg           = NULL;
    OEMCryptoResult *result               = NULL ;
    uint8_t* message                      = NULL;
    uint8_t* signature                    = NULL;
    OEMCrypto_KeyRefreshObject* key_array = NULL;
    uint32_t i;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(RefreshKeysArg));
    COPY_FROM_USER_ARG_K(arg,rArg ,sizeof( RefreshKeysArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message, rArg.message_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature, rArg.signature_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.key_array, sizeof(OEMCrypto_KeyRefreshObject)*rArg.num_keys);

    CHECK_ALLOC(message , rArg.message_length,ret, final);
    COPY_FROM_USER_ARG_K(rArg.message , *message, rArg.message_length);

    CHECK_ALLOC(signature ,rArg.signature_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.signature , *signature , rArg.signature_length);

    CHECK_ALLOC(key_array ,sizeof(OEMCrypto_KeyRefreshObject)*rArg.num_keys, ret, final);

    for(i=0;i<rArg.num_keys;++i)
    {

        if (!RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_id,    rArg.key_array[i].key_id_length, true) ||
            !RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_control, KEY_CONTROL_SIZE, false) ||
            !RangeCheck(rArg.message, rArg.message_length, rArg.key_array[i].key_control_iv, KEY_IV_SIZE, true)) {
            *result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            goto result;
        }
    
        key_array[i].key_id         = (rArg.key_array[i].key_id == NULL) ? 
            NULL :rArg.key_array[i].key_id - rArg.message + message;
        key_array[i].key_control_iv = rArg.key_array[i].key_control_iv == NULL ? 
            NULL : rArg.key_array[i].key_control_iv - rArg.message + message;
        key_array[i].key_control    = rArg.key_array[i].key_control == NULL ?
            NULL : rArg.key_array[i].key_control - rArg.message + message;

        key_array[i].key_id_length  = rArg.key_array[i].key_id_length;
    }

#ifdef CC_WIDEVINE_TZ_ENABLE    
    for(i=0;i<rArg.num_keys;++i)
    {
        key_array[i].key_id         = (rArg.key_array[i].key_id == NULL) ?  NULL :
            (uint8_t*)PHYSICAL((UINT32)key_array[i].key_id);
        key_array[i].key_control_iv = key_array[i].key_control_iv == NULL ?  NULL :
            (uint8_t*)PHYSICAL((UINT32)key_array[i].key_control_iv);
        key_array[i].key_control    = key_array[i].key_control == NULL ?  NULL :
            (uint8_t*)PHYSICAL((UINT32)key_array[i].key_control);
    }

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result    = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->message   = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->key_array = (OEMCrypto_KeyRefreshObject* )PHYSICAL((UINT32)key_array);

    TZ_CACHE_STR(message   , rArg.message_length);
    TZ_CACHE_STR(signature , rArg.signature_length);
    TZ_CACHE_STR(key_array , sizeof(OEMCrypto_KeyRefreshObject)*rArg.num_keys);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_RefreshKeys(tzArg, sizeof(rArg));
    TZ_CACHE_END(message   , rArg.message_length);
    TZ_CACHE_END(signature , rArg.signature_length);
    TZ_CACHE_END(key_array , sizeof(OEMCrypto_KeyRefreshObject)*rArg.num_keys);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

#else
    *result = OEMCrypto_RefreshKeys(rArg.session, message, rArg.message_length,
            signature, rArg.signature_length, rArg.num_keys, key_array);
#endif            

result:

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:
#ifdef CC_WIDEVINE_TZ_ENABLE    
    if(tzArg !=NULL){
        x_mem_free(tzArg);
    }
#endif    

    CHECK_FREE(result);
    CHECK_FREE(message);
    CHECK_FREE(signature);
    CHECK_FREE(key_array);

    x_sema_unlock(_hWidevineSema);
    return MTR_OK;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_SelectKey(unsigned long arg)
{
    OEMCryptoResult *result   = NULL;
    SelectKeyArg rArg, *tzArg = NULL;
    uint8_t* key_id           = NULL;
    MT_RESULT_T ret           = MTR_OK;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(SelectKeyArg));
    COPY_FROM_USER_ARG_K(arg , rArg ,sizeof(SelectKeyArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.key_id, rArg.key_id_length);

    CHECK_ALLOC(key_id ,rArg.key_id_length, ret, final);

    COPY_FROM_USER_ARG_K(rArg.key_id , *key_id ,rArg.key_id_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result = (OEMCryptoResult* )PHYSICAL((UINT32)result);;
    tzArg->key_id = (uint8_t *)PHYSICAL((UINT32)key_id);

    TZ_CACHE_STR(key_id , rArg.key_id_length);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_SelectKey(tzArg, sizeof(rArg));
    TZ_CACHE_END(key_id , rArg.key_id_length);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
#else
    *result =  OEMCrypto_SelectKey(rArg.session, key_id, rArg.key_id_length);
#endif    

    x_mem_free(key_id);

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_DecryptCTR(unsigned long arg)
{
    MT_RESULT_T ret                      = MTR_OK;
    OEMCryptoResult *result              = NULL;
    DecryptCTRArg rArg, *tzArg           = NULL;
    uint8_t *data_addr                   = NULL;
    uint8_t *iv                          = NULL;
    uint8_t *usr_out_buf_addr            = NULL;
    uint8_t *ker_out_buf_addr            = NULL;    
    OEMCrypto_DestBufferDesc *out_buffer = NULL;

    size_t max_length ;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(out_buffer,sizeof(OEMCrypto_DestBufferDesc),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DecryptCTRArg));
    COPY_FROM_USER_ARG_K(arg , rArg, sizeof(DecryptCTRArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result     , sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.data_addr  , rArg.data_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.iv         , KEY_IV_SIZE);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.out_buffer , sizeof(OEMCrypto_DestBufferDesc));

    COPY_FROM_USER_ARG_K(rArg.out_buffer , *out_buffer , sizeof(OEMCrypto_DestBufferDesc));

    switch (out_buffer->type) {
        case OEMCrypto_BufferType_Clear:
            max_length =  out_buffer->buffer.clear.max_length;
            break;
        case OEMCrypto_BufferType_Secure:
            max_length = out_buffer->buffer.secure.max_length;
            break;
        default:
            max_length = 0xFFFFFFFF;
    }

    if (max_length < rArg.data_length) {
        *result =  OEMCrypto_ERROR_SHORT_BUFFER;
        goto result;
    }

    if(out_buffer->type ==OEMCrypto_BufferType_Clear){
        usr_out_buf_addr = out_buffer->buffer.clear.address ;
        USR_SPACE_ACCESS_VALIDATE_ARG_K(usr_out_buf_addr , out_buffer->buffer.clear.max_length); 
        CHECK_ALLOC(ker_out_buf_addr , out_buffer->buffer.clear.max_length , ret , final);        
    }

    CHECK_ALLOC(data_addr ,rArg.data_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.data_addr,*data_addr , rArg.data_length);

    CHECK_ALLOC(iv,KEY_IV_SIZE, ret, final);
    COPY_FROM_USER_ARG_K(rArg.iv, *iv, KEY_IV_SIZE);

#ifdef CC_WIDEVINE_TZ_ENABLE    

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result     = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->data_addr  = (uint8_t *)PHYSICAL((UINT32)data_addr);
    tzArg->iv         = (uint8_t *)PHYSICAL((UINT32)iv);
    tzArg->out_buffer = (OEMCrypto_DestBufferDesc *)PHYSICAL((UINT32)out_buffer);

    if(out_buffer->type == OEMCrypto_BufferType_Clear)
    {
        out_buffer->buffer.clear.address = (uint8_t *)PHYSICAL((UINT32)ker_out_buf_addr);    
        TZ_CACHE_STR((void *)ker_out_buf_addr , out_buffer->buffer.clear.max_length);
    }
    
    TZ_CACHE_STR(data_addr   , rArg.data_length);
    TZ_CACHE_STR(iv          , KEY_IV_SIZE);
    TZ_CACHE_STR(out_buffer , sizeof(OEMCrypto_DestBufferDesc));
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));

    TZ_OEMCrypto_DecryptCTR(tzArg, sizeof(rArg));

    TZ_CACHE_END(data_addr   , rArg.data_length);
    TZ_CACHE_END(iv          , KEY_IV_SIZE);
    TZ_CACHE_END(out_buffer , sizeof(OEMCrypto_DestBufferDesc));
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

    if(out_buffer->type == OEMCrypto_BufferType_Clear)
    {
        //out_buffer->buffer.clear.address = (uint8_t *) VIRTUAL( (UINT32) out_buffer->buffer.clear.address);
        TZ_CACHE_END((void *)ker_out_buf_addr , out_buffer->buffer.clear.max_length);
    }

#else
    *result =   OEMCrypto_DecryptCTR(rArg.session, data_addr, rArg.data_length, rArg.is_encrypted,
            iv, rArg.block_offset, out_buffer, rArg.subsample_flags);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        if(out_buffer->type ==OEMCrypto_BufferType_Clear){
            //COPY_TO_USER_ARG_K(usr_out_buf_addr,*out_buffer->buffer.clear.address , out_buffer->buffer.clear.max_length);
            COPY_TO_USER_ARG_K(usr_out_buf_addr, *ker_out_buf_addr , rArg.data_length);
        }
    }
result:    
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(data_addr);
    CHECK_FREE(iv);

    if((out_buffer !=NULL)&&(out_buffer->type ==OEMCrypto_BufferType_Clear)){
        CHECK_FREE(ker_out_buf_addr);
    }

    CHECK_FREE(out_buffer);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_InstallKeybox(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    InstallKeyboxArg rArg, *tzArg = NULL;
    OEMCryptoResult *result       = NULL;
    uint8_t *keybox               = NULL;


    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg ,sizeof(InstallKeyboxArg));
    COPY_FROM_USER_ARG_K(arg, rArg , sizeof(InstallKeyboxArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result,sizeof(OEMCryptoResult));


    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.keybox,rArg.keyBoxLength);
    CHECK_ALLOC(keybox ,rArg.keyBoxLength, ret, final);
    COPY_FROM_USER_ARG_K(rArg.keybox, *keybox , rArg.keyBoxLength);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->keybox = (uint8_t *)PHYSICAL((UINT32)keybox);

    TZ_CACHE_STR(keybox             , rArg.keyBoxLength);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_OEMCrypto_InstallKeybox(tzArg , sizeof(rArg));
    TZ_CACHE_END(keybox             , rArg.keyBoxLength);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

#else
    *result = OEMCrypto_InstallKeybox(keybox,rArg.keyBoxLength);
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(keybox);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_IsKeyboxValid(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    OEMCryptoResult *result=NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

#ifdef CC_WIDEVINE_TZ_ENABLE    
    TZ_OEMCrypto_IsKeyboxValid(result , sizeof(OEMCryptoResult));
#else
    *result= OEMCrypto_IsKeyboxValid();
#endif    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(arg , *result , sizeof(OEMCryptoResult));
final:

    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GetDeviceID(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    GetDeviceIDArg rArg, *tzArg = NULL;
    OEMCryptoResult *result     = NULL;
    uint8_t *deviceID           = NULL;
    size_t *dlen                = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }  
    
    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;
    
    CHECK_ALLOC(result , sizeof(OEMCryptoResult) , ret , final);
    CHECK_ALLOC(dlen   , sizeof(size_t)          , ret , final);

    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg           , sizeof(GetDeviceIDArg));
    COPY_FROM_USER_ARG_K(arg                      , rArg, sizeof(GetDeviceIDArg));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result   , sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.idLength , sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.idLength            , *dlen, sizeof(size_t));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.deviceID , *dlen);

    CHECK_ALLOC(deviceID , *dlen , ret , final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg , sizeof(rArg) , ret , final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result   = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->deviceID = (uint8_t *)PHYSICAL((UINT32)deviceID);
    tzArg->idLength = (size_t  *)PHYSICAL((UINT32)dlen);

    invDataLen = *dlen;

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(deviceID, *dlen);
    TZ_CACHE_STR(dlen   , sizeof(size_t));

    TZ_OEMCrypto_GetDeviceID(tzArg, sizeof(rArg));

    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
    TZ_CACHE_END(dlen   , sizeof(size_t));
    TZ_CACHE_END(deviceID, invDataLen);

#else
    *result =   OEMCrypto_GetDeviceID(deviceID, dlen);
#endif    

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.deviceID , *deviceID, *dlen);
    }
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.idLength , *dlen , sizeof(size_t));

final:
#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(deviceID);
    CHECK_FREE(dlen);
    
    x_sema_unlock(_hWidevineSema);
    return ret;

}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GetKeyData(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GetKeyDataArg rArg, *tzArg  = NULL;
    OEMCryptoResult     *result = NULL;
    size_t *dlen                = NULL;
    uint8_t *keyData            = NULL;
    
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(dlen,sizeof(size_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg ,sizeof(GetKeyDataArg));
    COPY_FROM_USER_ARG_K(arg, rArg , sizeof(GetKeyDataArg));


    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.keyDataLength, sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.keyDataLength,*dlen , sizeof(size_t));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.keyData, *dlen );

    if(*dlen != 0 ){
        CHECK_ALLOC(keyData ,*dlen, ret, final);
    }

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result     = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->keyData     = (uint8_t * )PHYSICAL((UINT32)keyData );
    tzArg->keyDataLength = (size_t *)PHYSICAL((UINT32)dlen);

    invDataLen = *dlen;

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(keyData , *dlen);
    TZ_CACHE_STR(dlen , sizeof(size_t));

    TZ_OEMCrypto_GetKeyData(tzArg, sizeof(rArg));

    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
    TZ_CACHE_END(dlen   , sizeof(size_t));
    TZ_CACHE_END(keyData , invDataLen);
#else
    *result =   OEMCrypto_GetKeyData(keyData, dlen);
#endif    

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.keyData       , *keyData , *dlen);
    }

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.keyDataLength , *dlen , sizeof(size_t));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(dlen);
    CHECK_FREE(keyData);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GetRandom(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    GetRandomArg rArg, *tzArg = NULL;
    OEMCryptoResult *result   = NULL;
    uint8_t *randomData       = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg ,sizeof(GetRandomArg));
    COPY_FROM_USER_ARG_K(arg, rArg , sizeof(GetRandomArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult) );
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.randomData , rArg.dataLength);

    CHECK_ALLOC(randomData ,rArg.dataLength, ret, final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result     = (OEMCryptoResult* )PHYSICAL((UINT32)result);;
    tzArg->randomData = (uint8_t *)PHYSICAL((UINT32)randomData);

    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(randomData , rArg.dataLength);

    TZ_OEMCrypto_GetRandom(tzArg, sizeof(rArg));

    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(randomData , rArg.dataLength);
#else
    *result = OEMCrypto_GetRandom(randomData, rArg.dataLength);
#endif    

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.randomData,*randomData , rArg.dataLength);
    }

    COPY_TO_USER_ARG_K(rArg.result, *result , sizeof(OEMCryptoResult));

final:
#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(randomData);

    x_sema_unlock(_hWidevineSema);

    return ret;

}

MT_RESULT_T _MTDRM_WV_OEMCrypto_WrapKeybox(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    OEMCryptoResult *result     = NULL;
    WrapKeyboxArg rArg, *tzArg  = NULL;
    uint8_t *keybox             = NULL;
    uint8_t *wrappedKeybox      = NULL;
    uint8_t *transportKey       = NULL;
    size_t *wrappedKeyBoxLength = NULL;

    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg ,sizeof(WrapKeyboxArg));
    COPY_FROM_USER_ARG_K(arg, rArg , sizeof(WrapKeyboxArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result              , sizeof(int));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.wrappedKeyBoxLength , sizeof(size_t));
    CHECK_ALLOC(wrappedKeyBoxLength,sizeof(size_t),ret,final);
    COPY_FROM_USER_ARG_K(rArg.wrappedKeyBoxLength,*wrappedKeyBoxLength, sizeof(size_t));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.wrappedKeybox , *wrappedKeyBoxLength);
    CHECK_ALLOC(wrappedKeybox ,*wrappedKeyBoxLength, ret, final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.keybox        , rArg.keyBoxLength);
    CHECK_ALLOC(keybox ,rArg.keyBoxLength, ret, final);
    COPY_FROM_USER_ARG_K(rArg.keybox , *keybox , rArg.keyBoxLength);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.transportKey  , rArg.transportKeyLength);
    CHECK_ALLOC(transportKey ,rArg.transportKeyLength, ret, final);
    COPY_FROM_USER_ARG_K(rArg.transportKey , *transportKey , rArg.transportKeyLength);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result              = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->keybox              = (uint8_t *)PHYSICAL((UINT32)keybox);
    tzArg->wrappedKeybox       = (uint8_t *)PHYSICAL((UINT32)wrappedKeybox);
    tzArg->wrappedKeyBoxLength = (size_t *)PHYSICAL((UINT32)wrappedKeyBoxLength);
    tzArg->transportKey        = (uint8_t *)PHYSICAL((UINT32)transportKey);

    invDataLen = *wrappedKeyBoxLength;

    TZ_CACHE_STR(transportKey        , rArg.transportKeyLength);
    TZ_CACHE_STR(keybox              , rArg.keyBoxLength);
    TZ_CACHE_STR(wrappedKeyBoxLength , sizeof(size_t));
    TZ_CACHE_STR(result              , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(wrappedKeybox       , *wrappedKeyBoxLength);

    TZ_OEMCrypto_WrapKeybox(tzArg, sizeof(rArg));

    TZ_CACHE_END(transportKey        , rArg.transportKeyLength);
    TZ_CACHE_END(keybox              , rArg.keyBoxLength);
    TZ_CACHE_END(wrappedKeyBoxLength , sizeof(size_t));
    TZ_CACHE_END(result              , sizeof(OEMCryptoResult));
    TZ_CACHE_END(wrappedKeybox       , invDataLen);
#else
    *result =   OEMCrypto_WrapKeybox(keybox, rArg.keyBoxLength,
            wrappedKeybox,wrappedKeyBoxLength,
            transportKey,rArg.transportKeyLength);
#endif            

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.wrappedKeyBoxLength ,*wrappedKeyBoxLength , sizeof(size_t));

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.wrappedKeybox , *wrappedKeybox , *wrappedKeyBoxLength);
    }

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(wrappedKeyBoxLength);
    CHECK_FREE(keybox);
    CHECK_FREE(wrappedKeybox);
    CHECK_FREE(transportKey);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_RewrapDeviceRSAKey(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    RewrapDeviceRSAKeyArg rArg, *tzArg = NULL;
    OEMCryptoResult *result            = NULL;
    uint8_t* message                   = NULL;
    uint8_t* signature                 = NULL;
    uint32_t *nonce                    = NULL;
    uint8_t* enc_rsa_key               = NULL;
    uint8_t* enc_rsa_key_iv            = NULL;
    uint8_t* wrapped_rsa_key           = NULL;

    size_t *wrapped_rsa_key_length=NULL;
    size_t buffer_size;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;
    
    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(wrapped_rsa_key_length,sizeof(size_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg ,sizeof(RewrapDeviceRSAKeyArg));
    COPY_FROM_USER_ARG_K(arg, rArg , sizeof(RewrapDeviceRSAKeyArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result , sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.wrapped_rsa_key_length, sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.wrapped_rsa_key_length , *wrapped_rsa_key_length , sizeof(size_t));

    buffer_size = rArg.enc_rsa_key_length + sizeof(WrappedRSAKey);

    if (rArg.wrapped_rsa_key == NULL || *wrapped_rsa_key_length < buffer_size) {
        *wrapped_rsa_key_length = buffer_size;
        *result = OEMCrypto_ERROR_SHORT_BUFFER;
        goto result;
    }

    if (rArg.message == NULL || rArg.message_length == 0 || rArg.signature == NULL
            || rArg.signature_length == 0 || rArg.nonce == NULL || rArg.enc_rsa_key == NULL) {
        *result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    // Range check
    if (!RangeCheck(rArg.message, rArg.message_length, (const uint8_t*)(rArg.nonce),sizeof(uint32_t), true) 
        || !RangeCheck(rArg.message, rArg.message_length, rArg.enc_rsa_key, rArg.enc_rsa_key_length,true) 
        || !RangeCheck(rArg.message, rArg.message_length, rArg.enc_rsa_key_iv, 16 ,true)) {
        *result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message, rArg.message_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature, rArg.signature_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.wrapped_rsa_key, *wrapped_rsa_key_length);

    CHECK_ALLOC(message ,rArg.message_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.message , *message , rArg.message_length);

    CHECK_ALLOC(signature ,rArg.signature_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.signature , *signature , rArg.signature_length);

    CHECK_ALLOC(wrapped_rsa_key ,*wrapped_rsa_key_length, ret, final);

    nonce          = (uint32_t *)((const uint8_t *)rArg.nonce - rArg.message + message);
    enc_rsa_key    = (uint8_t *)(rArg.enc_rsa_key    - rArg.message + message);
    enc_rsa_key_iv = (uint8_t *)(rArg.enc_rsa_key_iv - rArg.message + message);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result                 = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->message                = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature              = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->nonce                  = (uint32_t*)PHYSICAL((UINT32)nonce);
    tzArg->enc_rsa_key            = (uint8_t *)PHYSICAL((UINT32)enc_rsa_key);
    tzArg->enc_rsa_key_iv         = (uint8_t *)PHYSICAL((UINT32)enc_rsa_key_iv);
    tzArg->wrapped_rsa_key        = (uint8_t *)PHYSICAL((UINT32)wrapped_rsa_key);
    tzArg->wrapped_rsa_key_length = (size_t *)PHYSICAL((UINT32)wrapped_rsa_key_length);


    invDataLen = *wrapped_rsa_key_length;
    TZ_CACHE_STR(message   , rArg.message_length);
    TZ_CACHE_STR(signature , rArg.signature_length);
    TZ_CACHE_STR(result                , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(wrapped_rsa_key_length , sizeof(size_t));
    TZ_CACHE_STR(wrapped_rsa_key        , *wrapped_rsa_key_length);

    TZ_OEMCrypto_RewrapDeviceRSAKey(tzArg, sizeof(rArg));

    TZ_CACHE_END(message   , rArg.message_length);
    TZ_CACHE_END(signature , rArg.signature_length);
    TZ_CACHE_END(result                , sizeof(OEMCryptoResult));
    TZ_CACHE_END(wrapped_rsa_key_length , sizeof(size_t));
    TZ_CACHE_END(wrapped_rsa_key        , invDataLen);
#else
    *result =   OEMCrypto_RewrapDeviceRSAKey(rArg.session, message, rArg.message_length,
            signature,rArg.signature_length,
            nonce, enc_rsa_key, rArg.enc_rsa_key_length,
            enc_rsa_key_iv, wrapped_rsa_key, wrapped_rsa_key_length);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.wrapped_rsa_key , *wrapped_rsa_key ,*wrapped_rsa_key_length);
    }
result:    

    COPY_TO_USER_ARG_K(rArg.wrapped_rsa_key_length , *wrapped_rsa_key_length, sizeof(size_t));
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(wrapped_rsa_key_length);
    CHECK_FREE(message);
    CHECK_FREE(signature);
    CHECK_FREE(wrapped_rsa_key);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_LoadDeviceRSAKey(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    LoadDeviceRSAKeyArg rArg, *tzArg = NULL;
    OEMCryptoResult *result          = NULL;
    uint8_t *wrapped_rsa_key         = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;
    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,sizeof(LoadDeviceRSAKeyArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(LoadDeviceRSAKeyArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result , sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.wrapped_rsa_key , rArg.wrapped_rsa_key_length);

    CHECK_ALLOC(wrapped_rsa_key ,rArg.wrapped_rsa_key_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.wrapped_rsa_key, *wrapped_rsa_key , rArg.wrapped_rsa_key_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result           = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->wrapped_rsa_key = (uint8_t *)PHYSICAL((UINT32)wrapped_rsa_key);

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(wrapped_rsa_key , rArg.wrapped_rsa_key_length);

    TZ_OEMCrypto_LoadDeviceRSAKey(tzArg, sizeof(rArg));

    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
    TZ_CACHE_END(wrapped_rsa_key , rArg.wrapped_rsa_key_length);
#else
    *result =  OEMCrypto_LoadDeviceRSAKey(rArg.session, wrapped_rsa_key,rArg.wrapped_rsa_key_length);
#endif    
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:
#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(wrapped_rsa_key);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GenerateRSASignature(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;
    OEMCryptoResult *result           = NULL;
    GenerateRSASignatureArg rArg, *tzArg = NULL;
    uint8_t *message                  = NULL;
    uint8_t *signature                = NULL;
    size_t *signature_length          = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(signature_length,sizeof(size_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(GenerateRSASignatureArg));
    COPY_FROM_USER_ARG_K(arg, rArg, sizeof(GenerateRSASignatureArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result, sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature_length, sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.signature_length , *signature_length , sizeof(size_t));


    if (*signature_length < 2048/8) {
        *signature_length = 2048/8;
        *result = OEMCrypto_ERROR_SHORT_BUFFER;
        goto result;
    }

    if (rArg.message == NULL || rArg.message_length == 0 || rArg.signature == NULL ) {
        *result =  OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message, rArg.message_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature, *signature_length);

    CHECK_ALLOC(signature,*signature_length, ret, final);
    CHECK_ALLOC(message ,rArg.message_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.message , *message , rArg.message_length);


#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result           = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->message          = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature        = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->signature_length = (size_t  *)PHYSICAL((UINT32)signature_length);

    invDataLen = *signature_length;

    TZ_CACHE_STR(result           , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(message          , rArg.message_length);
    TZ_CACHE_STR(signature_length , sizeof(size_t));
    TZ_CACHE_STR(signature        , *signature_length);
    TZ_OEMCrypto_GenerateRSASignature(tzArg , sizeof(rArg));
    TZ_CACHE_END(result           , sizeof(OEMCryptoResult));
    TZ_CACHE_END(message          , rArg.message_length);
    TZ_CACHE_END(signature_length , sizeof(size_t));
    TZ_CACHE_END(signature        , invDataLen);

#else
    *result =  OEMCrypto_GenerateRSASignature(rArg.session, message, rArg.message_length,
            signature, signature_length);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.signature , *signature , *signature_length);
    }
result:    
    COPY_TO_USER_ARG_K(rArg.signature_length , *signature_length , sizeof(size_t));
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(signature_length);
    CHECK_FREE(signature);
    CHECK_FREE(message);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_DeriveKeysFromSessionKey(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    OEMCryptoResult *result                  = NULL;
    DeriveKeysFromSessionKeyArg rArg, *tzArg = NULL;
    uint8_t *enc_session_key                 = NULL;
    uint8_t *mac_key_context                 = NULL;
    uint8_t *enc_key_context                 = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,sizeof(DeriveKeysFromSessionKeyArg));
    COPY_FROM_USER_ARG_K(arg,rArg, sizeof(DeriveKeysFromSessionKeyArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.result , sizeof(OEMCryptoResult));
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.enc_session_key , rArg.enc_session_key_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.enc_key_context , rArg.enc_key_context_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.mac_key_context , rArg.mac_key_context_length);

    CHECK_ALLOC(enc_session_key ,rArg.enc_session_key_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.enc_session_key, *enc_session_key , rArg.enc_session_key_length);

    CHECK_ALLOC(enc_key_context ,rArg.enc_key_context_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.enc_key_context, *enc_key_context , rArg.enc_key_context_length);

    CHECK_ALLOC(mac_key_context ,rArg.mac_key_context_length ,ret, final);
    COPY_FROM_USER_ARG_K(rArg.mac_key_context, *mac_key_context , rArg.mac_key_context_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result          = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->enc_session_key = (uint8_t *)PHYSICAL((UINT32)enc_session_key);
    tzArg->mac_key_context = (uint8_t *)PHYSICAL((UINT32)mac_key_context);
    tzArg->enc_key_context = (uint8_t *)PHYSICAL((UINT32)enc_key_context);

    TZ_CACHE_STR(enc_session_key , rArg.enc_session_key_length);
    TZ_CACHE_STR(mac_key_context , rArg.mac_key_context_length);
    TZ_CACHE_STR(enc_key_context , rArg.enc_key_context_length);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));

    TZ_OEMCrypto_DeriveKeysFromSessionKey(tzArg, sizeof(rArg));

    TZ_CACHE_END(enc_session_key , rArg.enc_session_key_length);
    TZ_CACHE_END(mac_key_context , rArg.mac_key_context_length);
    TZ_CACHE_END(enc_key_context , rArg.enc_key_context_length);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

#else
    *result =  OEMCrypto_DeriveKeysFromSessionKey(rArg.session,
            enc_session_key,
            rArg.enc_session_key_length,
            mac_key_context,
            rArg.mac_key_context_length,
            enc_key_context,
            rArg.enc_key_context_length);
#endif            

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(enc_session_key);
    CHECK_FREE(enc_key_context);
    CHECK_FREE(mac_key_context);


    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Encrypt(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GenericEncryptArg rArg, *tzArg = NULL;
    OEMCryptoResult *result        = NULL;
    uint8_t* in_buffer             = NULL;
    uint8_t* iv                    = NULL;
    uint8_t* out_buffer            = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenericEncryptArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GenericEncryptArg));

    if (rArg.in_buffer == NULL || rArg.buffer_length == 0 ||
            rArg.iv == NULL || rArg.out_buffer == NULL) {
        *result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.in_buffer , rArg.buffer_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.iv , KEY_IV_SIZE);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.out_buffer , rArg.buffer_length);

    CHECK_ALLOC(in_buffer,rArg.buffer_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.in_buffer , *in_buffer , rArg.buffer_length);

    CHECK_ALLOC(iv,KEY_IV_SIZE, ret, final);
    COPY_FROM_USER_ARG_K(rArg.iv , *iv , KEY_IV_SIZE);

    CHECK_ALLOC(out_buffer ,rArg.buffer_length, ret, final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result     = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->in_buffer  = (uint8_t *)PHYSICAL((UINT32)in_buffer);
    tzArg->iv         = (uint8_t *)PHYSICAL((UINT32)iv);
    tzArg->out_buffer = (uint8_t *)PHYSICAL((UINT32)out_buffer);

    TZ_CACHE_STR(in_buffer , rArg.buffer_length);
    TZ_CACHE_STR(iv        , KEY_IV_SIZE);
    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(out_buffer, rArg.buffer_length);

    TZ_OEMCrypto_Generic_Encrypt(tzArg, sizeof(rArg));

    TZ_CACHE_END(in_buffer , rArg.buffer_length);
    TZ_CACHE_END(iv        , KEY_IV_SIZE);
    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(out_buffer, rArg.buffer_length);
#else
    *result =  OEMCrypto_Generic_Encrypt(rArg.session,  in_buffer, rArg.buffer_length,
            iv, rArg.algorithm, out_buffer);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.out_buffer , *out_buffer , rArg.buffer_length);
    }

result:
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(in_buffer);
    CHECK_FREE(iv);
    CHECK_FREE(out_buffer);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Decrypt(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GenericDecryptArg rArg, *tzArg = NULL;
    OEMCryptoResult *result        = NULL;
    uint8_t* in_buffer             = NULL;
    uint8_t* iv                    = NULL;
    uint8_t* out_buffer            = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenericDecryptArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GenericDecryptArg));

    if (rArg.in_buffer == NULL || rArg.buffer_length == 0 
            || rArg.iv == NULL || rArg.out_buffer == NULL) {
        *result =  OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.in_buffer , rArg.buffer_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.iv , KEY_IV_SIZE);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.out_buffer , rArg.buffer_length);

    CHECK_ALLOC(in_buffer,rArg.buffer_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.in_buffer , *in_buffer , rArg.buffer_length);

    CHECK_ALLOC(iv ,KEY_IV_SIZE, ret, final);
    COPY_FROM_USER_ARG_K(rArg.iv , *iv , KEY_IV_SIZE);

    CHECK_ALLOC(out_buffer ,rArg.buffer_length, ret, final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result     = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->in_buffer  = (uint8_t *)PHYSICAL((UINT32)in_buffer);
    tzArg->iv         = (uint8_t *)PHYSICAL((UINT32)iv);
    tzArg->out_buffer = (uint8_t *)PHYSICAL((UINT32)out_buffer);

    TZ_CACHE_STR(in_buffer , rArg.buffer_length);
    TZ_CACHE_STR(iv        , KEY_IV_SIZE);
    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(out_buffer , rArg.buffer_length);

    TZ_OEMCrypto_Generic_Decrypt(tzArg, sizeof(rArg));

    TZ_CACHE_END(in_buffer , rArg.buffer_length);
    TZ_CACHE_END(iv        , KEY_IV_SIZE);
    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(out_buffer , rArg.buffer_length);
#else
    *result =  OEMCrypto_Generic_Decrypt(rArg.session,  in_buffer, rArg.buffer_length,
            iv, rArg.algorithm, out_buffer);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.out_buffer , *out_buffer , rArg.buffer_length);
    }

result:    
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(in_buffer);
    CHECK_FREE(iv);
    CHECK_FREE(out_buffer);

    x_sema_unlock(_hWidevineSema);
    return ret;
}       

MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Sign(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GenericSignArg rArg, *tzArg = NULL;
    OEMCryptoResult *result     = NULL;
    uint8_t* in_buffer          = NULL;
    uint8_t* signature          = NULL;
    size_t *signature_length    = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    CHECK_ALLOC(signature_length,sizeof(size_t),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenericSignArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GenericSignArg));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature_length , sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.signature_length , *signature_length , sizeof(size_t));

    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *signature_length = SHA256_DIGEST_LENGTH;
        *result = OEMCrypto_ERROR_SHORT_BUFFER;
        goto result;
    }
    if (rArg.in_buffer == NULL || rArg.buffer_length == 0 || rArg.signature == NULL) {
        *result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.in_buffer , rArg.buffer_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature , *signature_length);

    CHECK_ALLOC(in_buffer,rArg.buffer_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.in_buffer , *in_buffer , rArg.buffer_length);

    CHECK_ALLOC(signature ,*signature_length, ret, final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result           = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->in_buffer        = (uint8_t *)PHYSICAL((UINT32)in_buffer);
    tzArg->signature        = (uint8_t *)PHYSICAL((UINT32)signature);
    tzArg->signature_length = (size_t  *)PHYSICAL((UINT32)signature_length);


    invDataLen = *signature_length;

    TZ_CACHE_STR(in_buffer , rArg.buffer_length);
    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(signature_length , sizeof(size_t));
    TZ_CACHE_STR(signature , *signature_length);

    TZ_OEMCrypto_Generic_Sign(tzArg, sizeof(rArg));

    TZ_CACHE_END(in_buffer , rArg.buffer_length);
    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(signature_length , sizeof(size_t));
    TZ_CACHE_END(signature , invDataLen);
#else
    *result =   OEMCrypto_Generic_Sign(rArg.session,in_buffer,rArg.buffer_length,
            rArg.algorithm,signature, signature_length);
#endif            

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.signature , *signature ,*signature_length);
    }

    ret = MTR_OK;

result:    
    COPY_TO_USER_ARG_K(rArg.signature_length , *signature_length ,sizeof(size_t));
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    
#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    

    CHECK_FREE(result);
    CHECK_FREE(signature_length);
    CHECK_FREE(in_buffer);
    CHECK_FREE(signature);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_Generic_Verify(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    GenericVerifyArg rArg, *tzArg = NULL;
    OEMCryptoResult *result       = NULL;
    uint8_t* in_buffer            = NULL;
    uint8_t* signature            = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GenericVerifyArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GenericVerifyArg));

    if (rArg.signature_length != SHA256_DIGEST_LENGTH) {
        *result =  OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto result;
    }

    if (rArg.in_buffer == NULL || rArg.buffer_length == 0 || rArg.signature == NULL) {
        *result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.in_buffer , rArg.buffer_length);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature , rArg.signature_length);

    CHECK_ALLOC(in_buffer,rArg.buffer_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.in_buffer , *in_buffer , rArg.buffer_length);

    CHECK_ALLOC(signature ,rArg.signature_length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.signature , *signature , rArg.signature_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result    = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->in_buffer = (uint8_t *)PHYSICAL((UINT32)in_buffer);
    tzArg->signature = (uint8_t *)PHYSICAL((UINT32)signature);

    TZ_CACHE_STR(in_buffer , rArg.buffer_length);
    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(signature , rArg.signature_length);

    TZ_OEMCrypto_Generic_Verify(tzArg, sizeof(rArg));

    TZ_CACHE_END(in_buffer , rArg.buffer_length);
    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(signature , rArg.signature_length);

#else
    *result =   OEMCrypto_Generic_Verify(rArg.session,in_buffer,rArg.buffer_length,
            rArg.algorithm,signature, rArg.signature_length);
#endif            
result:

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_FREE(tzArg);
#endif    
    CHECK_FREE(result);
    CHECK_FREE(in_buffer);
    CHECK_FREE(signature);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_UpdateUsageTable(unsigned long arg)
{
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(OEMCryptoResult));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    TZ_OEMCrypto_UpdateUsageTable(result, sizeof(OEMCryptoResult));
#endif    

    COPY_TO_USER_ARG_K(arg, *result, sizeof(OEMCryptoResult));
final:
    
    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_DeactivateUsageEntry(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    DeactivateUsageEntryArg rArg, *tzArg = NULL;
    OEMCryptoResult *result       = NULL;
    uint8_t* pst            = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DeactivateUsageEntryArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(DeactivateUsageEntryArg));

    CHECK_ALLOC(pst ,rArg.pst_length, ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.pst , rArg.pst_length);
    COPY_FROM_USER_ARG_K(rArg.pst , *pst , rArg.pst_length);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->pst    = (uint8_t *)PHYSICAL((UINT32)pst);

    TZ_CACHE_STR(result               , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(pst                  , rArg.pst_length);

    TZ_OEMCrypto_DeactivateUsageEntry(tzArg , sizeof(rArg));

    TZ_CACHE_END(pst                  , rArg.pst_length);
    TZ_CACHE_END(result               , sizeof(OEMCryptoResult));

#endif            

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(pst);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_ReportUsage(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    ReportUsageArg rArg, * tzArg         = NULL;
    OEMCryptoResult        * result        = NULL;
    uint8_t                * pst           = NULL;
    OEMCrypto_PST_Report   * buffer        = NULL;
    size_t                 * buffer_length = NULL;
    size_t                 dataLength;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(ReportUsageArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(ReportUsageArg));

    CHECK_ALLOC(pst ,rArg.pst_length, ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.pst , rArg.pst_length);
    COPY_FROM_USER_ARG_K(rArg.pst , *pst , rArg.pst_length);

    CHECK_ALLOC(buffer_length ,sizeof(size_t), ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.buffer_length , sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.buffer_length , *buffer_length ,sizeof(size_t));

    CHECK_ALLOC(buffer                          , *buffer_length   , ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.buffer , *buffer_length);
    COPY_FROM_USER_ARG_K(rArg.buffer            , *buffer               , *buffer_length);

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result        = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->pst           = (uint8_t *)PHYSICAL((UINT32)pst);
    tzArg->buffer        = (OEMCrypto_PST_Report *)PHYSICAL((UINT32)buffer);
    tzArg->buffer_length = (size_t *)PHYSICAL((UINT32)buffer_length);
    dataLength = *buffer_length;

    TZ_CACHE_STR(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(pst           , rArg.pst_length);
    TZ_CACHE_STR(buffer        , dataLength);
    TZ_CACHE_STR(buffer_length , sizeof(size_t));

    TZ_OEMCrypto_ReportUsage(tzArg , sizeof(rArg));

    TZ_CACHE_END(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_END(pst           , rArg.pst_length);
    TZ_CACHE_END(buffer_length , sizeof(size_t));
    TZ_CACHE_END(buffer        , dataLength);

#endif            

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.buffer_length , *buffer_length ,sizeof(size_t));

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.buffer , *buffer ,*buffer_length);
    }

final:    

    CHECK_FREE(result);
    CHECK_FREE(pst);
    CHECK_FREE(buffer_length);
    CHECK_FREE(buffer);
    CHECK_FREE(tzArg);

    x_sema_unlock(_hWidevineSema);

    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_DeleteUsageEntry(unsigned long arg)
{
    MT_RESULT_T ret = MTR_OK;

    DeleteUsageEntryArg rArg, * tzArg= NULL;
    OEMCryptoResult      * result    = NULL;
    uint8_t              * pst       = NULL;
    uint8_t              * message   = NULL;
    uint8_t              * signature = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DeleteUsageEntryArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(DeleteUsageEntryArg));

    CHECK_ALLOC(message ,rArg.message_length, ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.message , rArg.message_length);
    COPY_FROM_USER_ARG_K(rArg.message , *message , rArg.message_length);

    CHECK_ALLOC(signature ,rArg.signature_length, ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.signature , rArg.signature_length);
    COPY_FROM_USER_ARG_K(rArg.signature , *signature , rArg.signature_length);

    if (!RangeCheck(rArg.message, rArg.message_length, rArg.pst, rArg.pst_length, false)) {
        *result =  OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto result;                
    }

    pst = rArg.pst - rArg.message + message;

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result    = (OEMCryptoResult* )PHYSICAL((UINT32)result);
    tzArg->pst       = (uint8_t *)PHYSICAL((UINT32)pst);
    tzArg->message   = (uint8_t *)PHYSICAL((UINT32)message);
    tzArg->signature = (uint8_t *)PHYSICAL((UINT32)signature);

    TZ_CACHE_STR(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(message   , rArg.message_length);
    TZ_CACHE_STR(signature , rArg.signature_length);

    TZ_OEMCrypto_DeleteUsageEntry(tzArg , sizeof(rArg));

    TZ_CACHE_END(result    , sizeof(OEMCryptoResult));
    TZ_CACHE_END(message   , rArg.message_length);
    TZ_CACHE_END(signature , rArg.signature_length);
#endif 

result:
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

    CHECK_FREE(result);
    CHECK_FREE(message);
    CHECK_FREE(signature);
    CHECK_FREE(tzArg);

    x_sema_unlock(_hWidevineSema);

    return ret;
}
MT_RESULT_T _MTDRM_WV_OEMCrypto_DeleteUsageTable(unsigned long arg)
{
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg , sizeof(OEMCryptoResult));

#ifdef CC_WIDEVINE_TZ_ENABLE    
    TZ_OEMCrypto_DeleteUsageTable(result, sizeof(OEMCryptoResult));
#endif    

    COPY_TO_USER_ARG_K(arg, *result, sizeof(OEMCryptoResult));
final:
    
    CHECK_FREE(result);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GetUsageTableWriteRequest(unsigned long arg)
{
    GetUsageTableWriteRequestArg rArg, * tzArg= NULL;
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;
    size_t *buffer_length   = NULL;
    uint8_t * buffer        = NULL;
    RPMB_DATAFRAM *dFrame   = NULL;
    
    size_t dataLength;
    
    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GetUsageTableWriteRequestArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GetUsageTableWriteRequestArg));

    CHECK_ALLOC(buffer_length ,sizeof(size_t), ret, final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.buffer_length , sizeof(size_t));
    COPY_FROM_USER_ARG_K(rArg.buffer_length , *buffer_length ,sizeof(size_t));

    CHECK_ALLOC(buffer                          , *buffer_length   , ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.buffer , *buffer_length);
    //COPY_FROM_USER_ARG_K(rArg.buffer            , *buffer               , *buffer_length);

    CHECK_ALLOC(dFrame                          , sizeof(RPMB_DATAFRAM), ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrame , sizeof(RPMB_DATAFRAM));
    //COPY_FROM_USER_ARG_K(rArg.dFrame            , *dFrame, sizeof(RPMB_DATAFRAM));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result        = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->buffer        = (uint8_t *)PHYSICAL((UINT32)buffer);
    tzArg->buffer_length = (size_t *)PHYSICAL((UINT32)buffer_length);
    tzArg->dFrame        = (RPMB_DATAFRAM *)PHYSICAL((UINT32)dFrame);
    
    dataLength = *buffer_length;

    TZ_CACHE_STR(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(buffer        , dataLength);
    TZ_CACHE_STR(buffer_length , sizeof(size_t));
    TZ_CACHE_STR(dFrame        , sizeof(RPMB_DATAFRAM));

    TZ_OEMCrypto_GetUsageTableWriteRequest(tzArg, sizeof(GetUsageTableWriteRequestArg));

    TZ_CACHE_END(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_END(buffer        , dataLength);
    TZ_CACHE_END(buffer_length , sizeof(size_t));
    TZ_CACHE_END(dFrame        , sizeof(RPMB_DATAFRAM));
    
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));
    COPY_TO_USER_ARG_K(rArg.buffer_length , *buffer_length ,sizeof(size_t));

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.buffer , *buffer ,*buffer_length);
        COPY_TO_USER_ARG_K(rArg.dFrame , *dFrame ,sizeof(RPMB_DATAFRAM));        
    }

final:    

    CHECK_FREE(result);
    CHECK_FREE(buffer_length);
    CHECK_FREE(buffer);
    CHECK_FREE(dFrame);    
    CHECK_FREE(tzArg);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_VerifyWriteResponse(unsigned long arg)
{
    VerifyWriteResponseArg rArg, * tzArg= NULL;
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;
    RPMB_DATAFRAM *dFrame   = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(VerifyWriteResponseArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(VerifyWriteResponseArg));

    CHECK_ALLOC(dFrame                          , sizeof(RPMB_DATAFRAM), ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrame , sizeof(RPMB_DATAFRAM));
    COPY_FROM_USER_ARG_K(rArg.dFrame            , *dFrame, sizeof(RPMB_DATAFRAM));
    
    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result        = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->dFrame        = (RPMB_DATAFRAM *)PHYSICAL((UINT32)dFrame);

    TZ_CACHE_STR(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(dFrame        , sizeof(RPMB_DATAFRAM));
    TZ_OEMCrypto_VerifyWriteResponse(tzArg, sizeof(VerifyWriteResponseArg));
    TZ_CACHE_END(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_END(dFrame        , sizeof(RPMB_DATAFRAM));
    
#endif    

    COPY_TO_USER_ARG_K(arg, *result, sizeof(OEMCryptoResult));
final:

    CHECK_FREE(result);
    CHECK_FREE(dFrame);    
    CHECK_FREE(tzArg);

    x_sema_unlock(_hWidevineSema);
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_GetUsageTableReadRequest(unsigned long arg)
{
    GetUsageTableReadRequesArg rArg, * tzArg= NULL;
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;
    RPMB_DATAFRAM *dFrame   = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT);

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(GetUsageTableReadRequesArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(GetUsageTableReadRequesArg));

    CHECK_ALLOC(dFrame                          , sizeof(RPMB_DATAFRAM), ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrame , sizeof(RPMB_DATAFRAM));
    //COPY_FROM_USER_ARG_K(rArg.dFrame            , *dFrame, sizeof(RPMB_DATAFRAM));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result        = (OEMCryptoResult*)PHYSICAL((UINT32)result);
    tzArg->dFrame        = (RPMB_DATAFRAM *)PHYSICAL((UINT32)dFrame);
    
    TZ_CACHE_STR(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(dFrame        , sizeof(RPMB_DATAFRAM));

    TZ_OEMCrypto_GetUsageTableReadRequest(tzArg, sizeof(GetUsageTableReadRequesArg));

    TZ_CACHE_END(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_END(dFrame        , sizeof(RPMB_DATAFRAM));
    
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.dFrame , *dFrame ,sizeof(RPMB_DATAFRAM));        
    }

final:    

    CHECK_FREE(result);
    CHECK_FREE(dFrame);
    CHECK_FREE(tzArg);

    x_sema_unlock(_hWidevineSema);
    
    return ret;
}

MT_RESULT_T _MTDRM_WV_OEMCrypto_VerifyReadResponse(unsigned long arg)
{
    VerifyReadResponseArg rArg, * tzArg= NULL;
    MT_RESULT_T ret         = MTR_OK;
    OEMCryptoResult *result = NULL;
    uint8_t* buffer         = NULL;
    RPMB_DATAFRAM *dFrame   = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }
    
    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);
    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(VerifyReadResponseArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(VerifyReadResponseArg));

    CHECK_ALLOC(buffer                          , rArg.buffer_length, ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.buffer , rArg.buffer_length);
    COPY_FROM_USER_ARG_K(rArg.buffer            , *buffer, rArg.buffer_length);

    CHECK_ALLOC(dFrame                          , sizeof(RPMB_DATAFRAM), ret              , final);
    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.dFrame , sizeof(RPMB_DATAFRAM));
    COPY_FROM_USER_ARG_K(rArg.dFrame            , *dFrame, sizeof(RPMB_DATAFRAM));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);

#ifdef CC_WIDEVINE_TZ_ENABLE    
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result        = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->dFrame        = (RPMB_DATAFRAM *)PHYSICAL((UINT32)dFrame);
    tzArg->buffer        = (uint8_t *)PHYSICAL((UINT32)buffer);

    TZ_CACHE_STR(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(dFrame        , sizeof(RPMB_DATAFRAM));
    TZ_CACHE_STR(buffer        , rArg.buffer_length);
    
    TZ_OEMCrypto_VerifyReadResponse(tzArg, sizeof(VerifyReadResponseArg));

    TZ_CACHE_END(result        , sizeof(OEMCryptoResult));
    TZ_CACHE_END(dFrame        , sizeof(RPMB_DATAFRAM));
    TZ_CACHE_END(buffer        , rArg.buffer_length);
        
#endif    

    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

    CHECK_FREE(result);
    CHECK_FREE(dFrame);
    CHECK_FREE(buffer);    
    CHECK_FREE(tzArg);
    
    x_sema_unlock(_hWidevineSema);
    
    return ret;
}



MT_RESULT_T  _MTDRM_WV_OEMCrypto_SetEntitlementKey(unsigned long arg)
{
#ifdef CC_WIDEVINE_TZ_ENABLE    
    MT_RESULT_T ret = MTR_OK;
    SetEntitlementKeyArg rArg, *tzArg  = NULL;
    OEMCryptoResult            *result = NULL;
    uint8_t                    *emmKey = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(SetEntitlementKeyArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(SetEntitlementKeyArg));

    if (rArg.emmKeyLength != KEY_SIZE) {
        *result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.emmKey , rArg.emmKeyLength);
    CHECK_ALLOC(emmKey,rArg.emmKeyLength, ret, final);
    COPY_FROM_USER_ARG_K(rArg.emmKey , *emmKey , rArg.emmKeyLength);

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));
    tzArg->result    = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->emmKey = (uint8_t *)PHYSICAL((UINT32)emmKey);

    TZ_CACHE_STR(emmKey , rArg.emmKeyLength);
    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));

    TZ_OEMCrypto_SetEntitlementKey(tzArg, sizeof(rArg));

    TZ_CACHE_END(emmKey , rArg.emmKeyLength);
    TZ_CACHE_END(result , sizeof(OEMCryptoResult));

result:
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(emmKey);

    x_sema_unlock(_hWidevineSema);
    return ret;
#else
    return MTR_ERR_INV;
#endif            
}

MT_RESULT_T  _MTDRM_WV_OEMCrypto_DeriveControlWord(unsigned long arg)
{
#ifdef CC_WIDEVINE_TZ_ENABLE    
    MT_RESULT_T ret = MTR_OK;

    DeriveControlWordArg rArg, *tzArg  = NULL;
    OEMCryptoResult            *result = NULL;
    uint8_t                    *ecm    = NULL;
    uint32_t                   *flags  = NULL;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);

    *result = OEMCrypto_ERROR_UNKNOWN_FAILURE;

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DeriveControlWordArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(DeriveControlWordArg));

    if (rArg.length != CTRL_WORD_SIZE) {
        *result = OEMCrypto_ERROR_NO_CW;
        goto result;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ecm , rArg.length);
    CHECK_ALLOC(ecm,rArg.length, ret, final);
    COPY_FROM_USER_ARG_K(rArg.ecm , *ecm , rArg.length);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.flags , FLAGS_SIZE);
    CHECK_ALLOC(flags,FLAGS_SIZE, ret, final);

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result    = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->ecm       = (uint8_t *)PHYSICAL((UINT32)ecm);
    tzArg->flags     = (uint32_t *)PHYSICAL((UINT32)flags);

    TZ_CACHE_STR(result , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(ecm    , rArg.length);
    TZ_CACHE_STR(flags  , FLAGS_SIZE);

    TZ_OEMCrypto_DeriveControlWord(tzArg, sizeof(rArg));

    TZ_CACHE_END(result , sizeof(OEMCryptoResult));
    TZ_CACHE_END(ecm    , rArg.length);
    TZ_CACHE_END(flags  , FLAGS_SIZE);

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.flags , *flags , FLAGS_SIZE);
    }

result:
    COPY_TO_USER_ARG_K(rArg.result, *result, sizeof(OEMCryptoResult));

final:    

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(ecm);
    CHECK_FREE(flags);

    x_sema_unlock(_hWidevineSema);
    return ret;
#else
    return MTR_ERR_INV;
#endif            
}

MT_RESULT_T  _MTDRM_WV_OEMCrypto_DecryptVideo(unsigned long arg)
{
#ifdef CC_WIDEVINE_TZ_ENABLE    
    MT_RESULT_T ret = MTR_OK;

    DecryptVideoArg rArg, *tzArg        = NULL;
    OEMCryptoResult       *result       = NULL;
    uint8_t               *iv           = NULL;
    uint8_t               *input        = NULL;
    uint32_t              *outputLength = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DecryptVideoArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(DecryptVideoArg));


    if(NULL != rArg.iv)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.iv , KEY_IV_SIZE);
        CHECK_ALLOC(iv , KEY_IV_SIZE , ret , final);
        COPY_FROM_USER_ARG_K(rArg.iv , *iv , KEY_IV_SIZE );
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.input , rArg.inputLength);
    CHECK_ALLOC(input ,rArg.inputLength , ret , final);
    COPY_FROM_USER_ARG_K(rArg.input , *input , rArg.inputLength);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.outputLength , sizeof(uint32_t));
    CHECK_ALLOC(outputLength,sizeof(uint32_t), ret, final);
    COPY_FROM_USER_ARG_K(rArg.outputLength , *outputLength , sizeof(uint32_t));

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));


    invDataLen = *outputLength;

    tzArg->result       = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->iv           = (NULL==iv)? NULL : (uint8_t *)PHYSICAL((UINT32)iv);
    tzArg->input        = (uint8_t *)PHYSICAL((UINT32)input);
    tzArg->outputLength = (uint32_t *)PHYSICAL((UINT32)outputLength);

    TZ_CACHE_STR(result       , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(iv           , KEY_IV_SIZE);
    TZ_CACHE_STR(input        , rArg.inputLength);
    TZ_CACHE_STR(outputLength , sizeof(uint32_t));
    TZ_CACHE_STR((void*)VIRTUAL((tzArg->output_handle)), invDataLen);

    TZ_OEMCrypto_DecryptVideo(tzArg, sizeof(rArg));

    TZ_CACHE_END(result       , sizeof(OEMCryptoResult));
    TZ_CACHE_END(iv           , KEY_IV_SIZE);
    TZ_CACHE_END(input        , rArg.inputLength);
    TZ_CACHE_END(outputLength , sizeof(uint32_t));
    TZ_CACHE_END((void*)VIRTUAL((tzArg->output_handle)), invDataLen);

    if(*result == OEMCrypto_SUCCESS)
    {
        if(iv !=NULL)
            COPY_TO_USER_ARG_K(rArg.iv           , *iv , KEY_IV_SIZE);
        COPY_TO_USER_ARG_K(rArg.outputLength , *outputLength , sizeof(uint32_t));
    }

    COPY_TO_USER_ARG_K(rArg.result , *result , sizeof(OEMCryptoResult));
final:    

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(iv);
    CHECK_FREE(input);
    CHECK_FREE(outputLength);

    x_sema_unlock(_hWidevineSema);
    return ret;
#else
    return MTR_ERR_INV;
#endif            
}

MT_RESULT_T  _MTDRM_WV_OEMCrypto_DecryptAudio(unsigned long arg)
{
#ifdef CC_WIDEVINE_TZ_ENABLE    
    MT_RESULT_T ret = MTR_OK;

    DecryptAudioArg rArg, *tzArg        = NULL;
    OEMCryptoResult       *result       = NULL;
    uint8_t               *iv           = NULL;
    uint8_t               *input        = NULL;
    uint8_t               *output       = NULL;
    uint32_t              *outputLength = NULL;
    size_t invDataLen;

    if (!u1Initialized)
    {
        x_sema_create(&_hWidevineSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        u1Initialized = 1 ;
    }

    x_sema_lock(_hWidevineSema, X_SEMA_OPTION_WAIT) ;

    CHECK_ALLOC(result,sizeof(OEMCryptoResult),ret,final);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(arg, sizeof(DecryptAudioArg));
    COPY_FROM_USER_ARG_K(arg , rArg,sizeof(DecryptAudioArg));

    if(NULL != rArg.iv)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.iv , KEY_IV_SIZE);
        CHECK_ALLOC(iv , KEY_IV_SIZE , ret , final);
        COPY_FROM_USER_ARG_K(rArg.iv , *iv , KEY_IV_SIZE );
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.input , rArg.inputLength);
    CHECK_ALLOC(input ,rArg.inputLength , ret , final);
    COPY_FROM_USER_ARG_K(rArg.input , *input , rArg.inputLength);

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.outputLength , sizeof(uint32_t));
    CHECK_ALLOC(outputLength,sizeof(uint32_t), ret, final);
    COPY_FROM_USER_ARG_K(rArg.outputLength , *outputLength , sizeof(uint32_t));

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.output , *rArg.outputLength);
    CHECK_ALLOC(output , *rArg.outputLength, ret, final);

    CHECK_ALLOC(tzArg,sizeof(rArg),ret,final);
    x_memcpy(tzArg,&rArg,sizeof(rArg));

    tzArg->result       = (OEMCryptoResult* )PHYSICAL((UINT32)result );
    tzArg->iv           = (NULL==iv)? NULL : (uint8_t *)PHYSICAL((UINT32)iv);
    tzArg->input        = (uint8_t *)PHYSICAL((UINT32)input);
    tzArg->output       = (uint8_t *)PHYSICAL((UINT32)output);
    tzArg->outputLength = (uint32_t *)PHYSICAL((UINT32)outputLength);

    invDataLen = *outputLength;

    TZ_CACHE_STR(result       , sizeof(OEMCryptoResult));
    TZ_CACHE_STR(iv           , KEY_IV_SIZE);
    TZ_CACHE_STR(input        , rArg.inputLength);
    TZ_CACHE_STR(output       , invDataLen);
    TZ_CACHE_STR(outputLength , sizeof(uint32_t));

    TZ_OEMCrypto_DecryptAudio(tzArg, sizeof(rArg));

    TZ_CACHE_END(result       , sizeof(OEMCryptoResult));
    TZ_CACHE_END(iv           , KEY_IV_SIZE);
    TZ_CACHE_END(input        , rArg.inputLength);
    TZ_CACHE_END(output       , invDataLen);
    TZ_CACHE_END(outputLength , sizeof(uint32_t));

    if(*result == OEMCrypto_SUCCESS)
    {
        COPY_TO_USER_ARG_K(rArg.outputLength , *outputLength , sizeof(uint32_t));
        if(iv !=NULL)
            COPY_TO_USER_ARG_K(rArg.iv           , *iv , KEY_IV_SIZE);        
        COPY_TO_USER_ARG_K(rArg.output       , *output       , *outputLength);
    }

    COPY_TO_USER_ARG_K(rArg.result , *result , sizeof(OEMCryptoResult));
final:    

    CHECK_FREE(tzArg);
    CHECK_FREE(result);
    CHECK_FREE(iv);
    CHECK_FREE(input);
    CHECK_FREE(output);
    CHECK_FREE(outputLength);

    x_sema_unlock(_hWidevineSema);
    return ret;
#else
    return MTR_ERR_INV;
#endif            
}
