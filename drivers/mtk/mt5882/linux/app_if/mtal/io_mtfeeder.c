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
 * $RCSfile: io_mtfeeder.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file mtfeeder.c
 *  This file contains implementation of exported APIs of Feeder module.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mttype.h"
#include "feeder_if.h"
#include "fbm_drvif.h"
//#include "mtfeeder.h"
#include "dmx_if.h"
#include "cb_data.h"
#include "cb_low.h"
#include "x_os.h"
#include "x_assert.h"
#include <linux/mtal_ioctl.h>
#include "x_bim.h"  //for VIRTUAL()
//#include "x_mm_common.h"
#include "mtswdmx.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define COPY_FROM_USER(ArgUsr, ArgKernel, ArgSize)                  \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,           \
                       ArgSize))                                    \
{                                                                   \
        printk("mtal_ioctl_mtfeeder argument error\n");             \
        return MTR_ERR_INV;                                         \
    }

#define COPY_TO_USER(ArgUsr, ArgKernel,ArgSize)                     \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,             \
                       ArgSize))                                    \
    {                                                               \
        printk("mtal_ioctl_mtfeeder argument error\n");             \
        return MTR_ERR_INV;                                         \
    }    

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return -1;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel, type)    \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))                               \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");       \
        return -1;                                          \
    }
    
#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtfeeder argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

typedef struct
{
	UINT8 u1AttachedSrcId;    
    FeederSourceType eSrcType;
    FEEDER_BUF_INFO_T *pQryBufInfo;
    FEEDER_REQ_DATA_T* pt_ReqData;	
}FEEDER_CALLBACK_INFO_T_QueryFeeder;

typedef struct 
{
	FeederSourceType eSrcType;
} FEEDER_CALLBACK_INFO_T_Flush;

typedef struct 
{
	UINT8 u1AttachedSrcId;
	FeederSourceType eSrcType;
	MM_FILE_OPS_T *prFileInfo;
} FEEDER_CALLBACK_INFO_T_Seek;

typedef struct 
{
	FeederSourceType eSrcType;
	FEEDER_BUF_INFO_T *pQryBufInfo;
	UINT32 u4NewRPtr;
} FEEDER_CALLBACK_INFO_T_Consume;

typedef struct 
{
	FeederSourceType eSrcType;
	FEEDER_BUF_INFO_T *pQryBufInfo;
	UINT32 u4NewRPtr;
} FEEDER_CALLBACK_INFO_T_Push_Consume;

typedef struct 
{
	FeederSourceType eSrcType;
} FEEDER_CALLBACK_INFO_T_Overflow;

typedef struct 	
{
	FeederSourceType eSrcType;
} FEEDER_CALLBACK_INFO_T_Underflow;

static DMX_VUDU_KEY_T _rDmxVuduKey[2];
static UINT8 _au1UniqueKey[2][16] = {{0}, {0}};
static UINT8 _au1Key[2][32] = {{0}, {0}};
static UINT8 _au1IV[2][16] = {{0}, {0}};
static UCHAR g_szSeqHeaderRaw[512] = {0};
static FEEDER_PDCF_CRYPT_INFO_T rPDCFEncryptInfo;
static FEEDER_AES_CTR_ENCRYPT rAesCrtCrypt;
static FEEDER_CTRL_MTK_ENCRYPT_INFO_T rMtkEncryptInfo;
static FEEDER_CTRL_IBC_DASH_SET_INFO_T rIBC_Dash_Set_Info;
static FEEDER_CTRL_HDD_DTCP_IP_INFO_T rIBC_Hdd_Dtcp_Ip_Info;
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

static MTBUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T _rIBC_Strm_Info;
#if 0
static void _MTFEEDER_DmxNfy(void* pvNfyTag, FEEDER_COND_T eNfyCond, UINT32 u4Data1, UINT32 u4Data2)
{
    MTFEEDER_DMXNFY_CB_T rCb;
    
    x_memset(&rCb, 0, sizeof(MTFEEDER_DMXNFY_CB_T));
    rCb.eNfyCond = (UINT32)eNfyCond;
    rCb.u4Data1 = u4Data1;
    rCb.u4Data2 = u4Data2;
    _CB_PutEvent(CB_MTAL_FEEDER_DMXNFY_TRIGGER, sizeof(MTFEEDER_DMXNFY_CB_T), (void*)&rCb);
}
#endif

#if 0
static MT_RESULT_T _MTFEEDER_SetDmxNfy(unsigned long arg)
{
    UCHAR ucEsId;
    FEEDER_NFY_INFO_T rNfyInfo;
    
    ucEsId = (UCHAR)arg;
    MTFEEDER_PRINT("_MTFEEDER_SetDmxNfy %d\n",(int)ucEsId);

    rNfyInfo.pfFeederNfy = _MTFEEDER_DmxNfy;
    rNfyInfo.pvTag      = 0;
    
    //if(FEEDER_SetDmxNfy(ucEsId, &rNfyInfo) != FEEDER_DRV_OK)
    //{
    //    return MTR_NOT_OK;
    //}
    
    return MTR_OK;
}
#endif


static MT_RESULT_T _MTFEEDER_Open(unsigned long arg)
{
    INT32 i4Result;
    
    i4Result = FeederOpen((FeederSourceType)arg);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetInfo(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    FeederSetInfo(rArg.ai4Arg[0], rArg.ai4Arg[1], rArg.ai4Arg[2]);
    
    return MTR_OK;
}

static UINT32 _MTFEEDER_QueryFeeder(UINT8 u1AttachedSrcId, 
    FeederSourceType eSrcType, 
    FEEDER_BUF_INFO_T *pQryBufInfo, 
    FEEDER_REQ_DATA_T* pt_ReqData)
{
     /*  FBM_POOL_T* prFbmPool;
	
        refer to _buf_agt_QueryFeeder();	
	*/
	MTFEEDER_CALLBACK_INFO_T_QueryFeeder_T rQueryFeeder;

    x_memset((void*)&rQueryFeeder, 0, sizeof(MTFEEDER_CALLBACK_INFO_T_QueryFeeder_T));
	rQueryFeeder.u1AttachedSrcId = u1AttachedSrcId;
	rQueryFeeder.eSrcType = eSrcType;

	rQueryFeeder.u4StartAddr = pQryBufInfo->u4StartAddr;
	//rQueryFeeder.u4PhysicalVirAddr = VIRTUAL(pQryBufInfo->u4PhysicalAddr);
	
	rQueryFeeder.u4EndAddr = pQryBufInfo->u4EndAddr;
	rQueryFeeder.u4ReadAddr = pQryBufInfo->u4ReadAddr;
	rQueryFeeder.u4WriteAddr = pQryBufInfo->u4WriteAddr;
	rQueryFeeder.u4LastReadAddr = pQryBufInfo->u4LastReadAddr;
	rQueryFeeder.u4LastReadSize = pQryBufInfo->u4LastReadSize;
	rQueryFeeder.u4FreeSize = pQryBufInfo->u4FreeSize;
	rQueryFeeder.u4BytesInBuf = pQryBufInfo->u4BytesInBuf;
	rQueryFeeder.u4Size = pQryBufInfo->u4Size; 
	rQueryFeeder.fgRingBuf = pQryBufInfo->fgRingBuf;

	rQueryFeeder.eDataType = pt_ReqData->eDataType;
	rQueryFeeder.u4Id = pt_ReqData->u4Id;
	rQueryFeeder.u4ReadSize = pt_ReqData->u4ReadSize;
	rQueryFeeder.u4WriteAddr_req = pt_ReqData->u4WriteAddr;
	rQueryFeeder.u4AlignOffset = pt_ReqData->u4AlignOffset;
	rQueryFeeder.u8FilePos = pt_ReqData->u8FilePos;
	rQueryFeeder.fgPartial = pt_ReqData->fgPartial;
	rQueryFeeder.fgEof = pt_ReqData->fgEof;
	rQueryFeeder.eFeederIBC = pt_ReqData->eFeederIBC;
	rQueryFeeder.eFeederReqType = pt_ReqData->eFeederReqType;

    //printf("io mtfeeder query\n");
	/*
	prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);
	if(prFbmPool)
       {
	    rQueryFeeder.u4FeederMapAddr = VIRTUAL(prFbmPool->u4Addr);
       }
    */
    //rQueryFeeder.u4FeederMapAddr = VIRTUAL(pQryBufInfo->u4FeederMemAddr);
    rQueryFeeder.FeederDeviceId  = pQryBufInfo->FeederDeviceId;
	_CB_PutEvent(CB_MTAL_FEEDER_QUERYFEEDER_TRIGGER, sizeof(MTFEEDER_CALLBACK_INFO_T_QueryFeeder_T), &rQueryFeeder);
	
	return rQueryFeeder.u4ReadSize;
}

static INT64 _MTFEEDER_SeekFeeder(UINT8 u1AttachedSrcId, FeederSourceType eSrcType, MM_FILE_OPS_T *prFileInfo)
{
/*
	refer to _buf_agt_Seek();
*/
	MTFEEDER_CALLBACK_INFO_T_Seek_T	rSeekFeeder;	

	rSeekFeeder.u1AttachedSrcId= u1AttachedSrcId;
	rSeekFeeder.eSrcType= eSrcType;
	rSeekFeeder.u8Offset= prFileInfo->u8Offset;
	rSeekFeeder.u4QueryID= prFileInfo->u4QueryID;
	rSeekFeeder.u1Whence= prFileInfo->u1Whence;

	_CB_PutEvent(CB_MTAL_FEEDER_SEEKFEEDER_TRIGGER, sizeof(MTFEEDER_CALLBACK_INFO_T_Seek_T), &rSeekFeeder);
	
	return rSeekFeeder.u8Offset;
}

#ifdef BAGT_URI_MODE_1
static MT_RESULT_T _MTFEEDER_ConsumeFeeder(FeederSourceType eSrcType, FEEDER_BUF_INFO_T *pQryBufInfo, UINT32 u4NewRPtr)
{
	FEEDER_CALLBACK_INFO_T_Consume   rConsumFeeder;	

	rConsumFeeder.eSrcType = eSrcType;
	rConsumFeeder.pQryBufInfo = pQryBufInfo;
	rConsumFeeder.u4NewRPtr = u4NewRPtr;
	
	_CB_PutEvent(CB_MTAL_FEEDER_CONSUMEFEEDER_TRIGGER, sizeof(FEEDER_CALLBACK_INFO_T_Consume), &rConsumFeeder);
}
#endif
static void _MTFEEDER_PushConsumeFeeder(FeederSourceType eSrcType, FEEDER_BUF_INFO_T *pQryBufInfo, UINT32 u4NewRPtr)
{
       
/*
    FBM_POOL_T* prFbmPool;
	FEEDER_CALLBACK_INFO_T_Push_Consume   rPushConsumFeeder;	

	rPushConsumFeeder.eSrcType = eSrcType;
	rPushConsumFeeder.pQryBufInfo = pQryBufInfo;
	rPushConsumFeeder.u4NewRPtr = u4NewRPtr;
*/
	MTFEEDER_CALLBACK_INFO_T_Push_Consume_T   rPushConsumFeeder;	
	rPushConsumFeeder.eSrcType	= eSrcType;
	rPushConsumFeeder.u4StartAddr = pQryBufInfo->u4StartAddr;	
	rPushConsumFeeder.u4EndAddr = pQryBufInfo->u4EndAddr;			
	rPushConsumFeeder.u4ReadAddr = pQryBufInfo->u4ReadAddr;				
	rPushConsumFeeder.u4WriteAddr = pQryBufInfo->u4WriteAddr;			
	rPushConsumFeeder.u4LastReadAddr = pQryBufInfo->u4LastReadAddr;			
	rPushConsumFeeder.u4LastReadSize = pQryBufInfo->u4LastReadSize;			
	rPushConsumFeeder.u4FreeSize = pQryBufInfo->u4FreeSize;			
	rPushConsumFeeder.u4BytesInBuf = pQryBufInfo->u4BytesInBuf;			
	rPushConsumFeeder.u4Size = pQryBufInfo->u4Size;			
	rPushConsumFeeder.fgRingBuf = pQryBufInfo->fgRingBuf;			
	rPushConsumFeeder.u4NewRPtr = u4NewRPtr;			

/*
	prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);
	if(prFbmPool)
       {
	    rPushConsumFeeder.u4FeederMapAddr = VIRTUAL(prFbmPool->u4Addr);
       }
*/
    rPushConsumFeeder.FeederDeviceId  = pQryBufInfo->FeederDeviceId;
    //rPushConsumFeeder.u4FeederMapAddr = VIRTUAL(pQryBufInfo->u4FeederMemAddr);
    
	_CB_PutEvent(CB_MTAL_FEEDER_PUSHCONSUMEFEEDER_TRIGGER, sizeof(MTFEEDER_CALLBACK_INFO_T_Push_Consume_T), &rPushConsumFeeder);
}

static void _MTFEEDER_OverflowFeeder(FeederSourceType eSrcType)
{
	FEEDER_CALLBACK_INFO_T_Overflow rOverflowFeeder;	

	rOverflowFeeder.eSrcType = eSrcType;
	
	_CB_PutEvent(CB_MTAL_FEEDER_OVERFLOWFEEDER_TRIGGER, sizeof(FEEDER_CALLBACK_INFO_T_Overflow), &rOverflowFeeder);
}

static UINT32 _MTFEEDER_Flush(FeederSourceType eSrcType)
{
	FEEDER_CALLBACK_INFO_T_Flush rFlush;
    UINT32   result;
	rFlush.eSrcType = eSrcType;

	result = _CB_PutEvent(CB_MTAL_FEEDER_FLUSH_TRIGGER, sizeof(FEEDER_CALLBACK_INFO_T_Flush), &rFlush);
    
    return result;
}

static void _MTFEEDER_UnderflowFeeder(FeederSourceType eSrcType)
{
	FEEDER_CALLBACK_INFO_T_Underflow   rUnderflowFeeder;	

	rUnderflowFeeder.eSrcType = eSrcType;
	
	_CB_PutEvent(CB_MTAL_FEEDER_UNDERFLOWFEEDER_TRIGGER, sizeof(FEEDER_CALLBACK_INFO_T_Underflow), &rUnderflowFeeder);
}
static MT_RESULT_T _MTFEEDER_RegPullDataFnc(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4Result;
    FEEDER_QUERY_DATA *prSetQuery;
	FEEDER_QUERY_DATA rSetQuery;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    prSetQuery = (FEEDER_QUERY_DATA *)rArg.ai4Arg[1];
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prSetQuery, FEEDER_QUERY_DATA);
    COPY_FROM_USER_ARG(prSetQuery, rSetQuery, FEEDER_QUERY_DATA);
    
	rSetQuery.pfnSource = _MTFEEDER_QueryFeeder;
	rSetQuery.pfnSeekPos = _MTFEEDER_SeekFeeder;
	if(rSetQuery.pfnConsume!=NULL)
	{
	    rSetQuery.pfnConsume = _MTFEEDER_PushConsumeFeeder;
	}
	if(rSetQuery.pfnOverflow!=NULL)	
	{
	    rSetQuery.pfnOverflow = _MTFEEDER_OverflowFeeder;
	}
	if(rSetQuery.pfnUnderflow!=NULL)	
	{
	    rSetQuery.pfnUnderflow = _MTFEEDER_UnderflowFeeder;	
	}
	if(rSetQuery.pfFlush != NULL)
	{
		rSetQuery.pfFlush = _MTFEEDER_Flush;
	}

    i4Result = FeederRegisterPullDataFunc(rArg.ai4Arg[0], (FEEDER_QUERY_DATA *)&rSetQuery);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetFlush(unsigned long arg)
{
    FeederSetFlush((FeederSourceType)arg);
    
    return MTR_OK;
}


static MT_RESULT_T _MTFEEDER_SetAppQryMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    FeederSetAppQueryMode(rArg.ai4Arg[0], rArg.ai4Arg[1]);
    
    return MTR_OK;
}


static MT_RESULT_T _MTFEEDER_Start(unsigned long arg)
{
    INT32 i4Result;
    
    i4Result = FeederStart((FeederSourceType)arg);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetRing(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    FeederSetRing(rArg.ai4Arg[0], rArg.ai4Arg[1]);
    
    return MTR_OK;
}


static MT_RESULT_T _MTFEEDER_Stop(unsigned long arg)
{
    INT32 i4Result;
    
    i4Result = FeederStop((FeederSourceType)arg);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_Close(unsigned long arg)
{
    INT32 i4Result;
    
    i4Result = FeederClose((FeederSourceType)arg);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetBagtInfo(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    FeederSetBagtInfo(rArg.ai4Arg[0], rArg.ai4Arg[1], rArg.ai4Arg[2]);
    
    return MTR_OK;
}


static MT_RESULT_T _MTFEEDER_SetUriMode(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4Result;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    i4Result = FeederSetUriMode(rArg.ai4Arg[0], rArg.ai4Arg[1], rArg.ai4Arg[2], rArg.ai4Arg[3]);
    
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetSkypeSrcFlg(unsigned long arg)
{
    FeederSetSkypeSourceFlag((BOOL)arg);
    
    return MTR_OK;
}


static MT_RESULT_T _MTFEEDER_GetBufInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    FEEDER_BUF_INFO_T rBufInfo;
    FEEDER_BUF_INFO_T *prBufInfo;  // !!! megaa 20110401, tricky tmp solution
    INT32 i4Result;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    prBufInfo = (FEEDER_BUF_INFO_T *)rArg.ai4Arg[1];  // !!! megaa 20110401, tricky tmp solution

    i4Result = FeederGetBufferInfo(rArg.ai4Arg[0], (FEEDER_BUF_INFO_T *)&rBufInfo);
    
    if (i4Result == FEEDER_E_OK)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(prBufInfo, FEEDER_BUF_INFO_T);  // !!! megaa 20110401, tricky tmp solution
        COPY_TO_USER_ARG(prBufInfo, rBufInfo, FEEDER_BUF_INFO_T);  // !!! megaa 20110401, tricky tmp solution

        return MTR_OK;
    }
    else
        return MTR_NOT_OK;
}

static MT_RESULT_T _MTFEEDER_GetFeederMemInfo(unsigned long arg)
{
   FeederMemInfo meminfo;

   USR_SPACE_ACCESS_VALIDATE_ARG(arg,FeederMemInfo);
   
   x_memset(&meminfo,0,sizeof(FeederMemInfo));
   if(FeederGetMemoryInfo(&meminfo))//(FeederMemInfo *) arg
   {
     COPY_TO_USER_ARG((FeederMemInfo*)arg,meminfo,FeederMemInfo);
     return MTR_OK;
   }
   return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_GetBufFullness(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4Len;
    UINT32 u4Percent;
    UINT32 *pu4Len;
    UINT32 *pu4Percent;
    INT32 i4Result;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    pu4Len = (UINT32 *)rArg.ai4Arg[1];
    pu4Percent = (UINT32 *)rArg.ai4Arg[2];

    i4Result = FeederGetBufferFullness(rArg.ai4Arg[0], &u4Len, &u4Percent);
    
    if (i4Result == FEEDER_E_OK)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Len, UINT32);
        COPY_TO_USER_ARG(pu4Len, u4Len, UINT32);
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Percent, UINT32);
        COPY_TO_USER_ARG(pu4Percent, u4Percent, UINT32);

        return MTR_OK;
    }
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_SetPreloadDone(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4Result;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
	i4Result = FeederSetPreLoadDone(rArg.ai4Arg[0], rArg.ai4Arg[1]);  
	
    if (i4Result == FEEDER_E_OK)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}


static MT_RESULT_T _MTFEEDER_NotifyHandle(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    BOOL fgResult;
    FEEDER_REQ_DATA_T rFeederReqData;
    DMX_VUDU_KEY_T rKeyTmp;
    MTBUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T rTmpStrmInfo;
	
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    x_memset(&rFeederReqData, 0, sizeof(FEEDER_REQ_DATA_T));
    
    if (0 != rArg.ai4Arg[2])
    {
        COPY_FROM_USER( rArg.ai4Arg[2], rFeederReqData, sizeof(FEEDER_REQ_DATA_T) );
        
        if ((FEEDER_IBC_DMX_VUDU_KEY == rFeederReqData.eFeederIBC) &&
            (rFeederReqData.u4WriteAddr) )
        {
            UINT32 u4Index = 0;

            ASSERT( (rFeederReqData.u4ReadSize == sizeof(DMX_VUDU_KEY_T)) && 
                    ((UINT32)rArg.ai4Arg[0] < FEEDER_SOURCE_INVALID) );
                    
            COPY_FROM_USER( rFeederReqData.u4WriteAddr, rKeyTmp, sizeof(DMX_VUDU_KEY_T) );

            if (rKeyTmp.fgEven)
            {
                u4Index = 0;
            }
            else
            {
                u4Index = 1;
            }
			COPY_FROM_USER(rKeyTmp.pu1UniqueKey,_au1UniqueKey[u4Index][0],16);
			COPY_FROM_USER(rKeyTmp.pu1Key,_au1Key[u4Index][0],32);
			COPY_FROM_USER(rKeyTmp.pu1IV,_au1IV[u4Index][0],16);
			rKeyTmp.pu1UniqueKey = _au1UniqueKey[u4Index];
			rKeyTmp.pu1Key = _au1Key[u4Index];
			rKeyTmp.pu1IV = _au1IV[u4Index];
			x_memcpy((void *)&_rDmxVuduKey[u4Index], (void *)&rKeyTmp, sizeof(DMX_VUDU_KEY_T));
			rFeederReqData.u4WriteAddr = (UINT32)(&_rDmxVuduKey[u4Index]);
        }
        else if (FEEDER_IBC_CMPB_MTK == rFeederReqData.eFeederIBC)
        {
            COPY_FROM_USER( rFeederReqData.prMtkEncryptInfo, rMtkEncryptInfo, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T) );
            rFeederReqData.prMtkEncryptInfo = &rMtkEncryptInfo;
        }
        else if ((FEEDER_IBC_SEQHEADER == rFeederReqData.eFeederIBC) &&
            rFeederReqData.u.t_vid_seqheader_info.b_is_raw &&
            rFeederReqData.eFeederStrmType == FEEDER_CTRL_STREAM_TYPE_VIDEO &&
            rFeederReqData.u.t_vid_seqheader_info.u.t_seq_raw.pc_buff != NULL &&
            rFeederReqData.u.t_vid_seqheader_info.u.t_seq_raw.z_buff_size < 512 )
        {
            if (copy_from_user(g_szSeqHeaderRaw, (void __user *)rFeederReqData.u.t_vid_seqheader_info.u.t_seq_raw.pc_buff, rFeederReqData.u.t_vid_seqheader_info.u.t_seq_raw.z_buff_size))                                  
            {                                                                   
                printk("mtal_ioctl_mtfeeder argument error\n");            
                return MTR_NOT_OK;                                      
            }

            rFeederReqData.u.t_vid_seqheader_info.u.t_seq_raw.pc_buff = g_szSeqHeaderRaw;
        }
        else if(FEEDER_IBC_PDCF == rFeederReqData.eFeederIBC)
        {
            COPY_FROM_USER( rFeederReqData.pt_pdcf_crypt, rPDCFEncryptInfo, sizeof(FEEDER_PDCF_CRYPT_INFO_T) );
            rFeederReqData.pt_pdcf_crypt = &rPDCFEncryptInfo;
        }
        else if(FEEDER_IBC_CENC == rFeederReqData.eFeederIBC)
        {
            COPY_FROM_USER( rFeederReqData.pt_aes_crt_crypt, rAesCrtCrypt, sizeof(FEEDER_AES_CTR_ENCRYPT) );
            rFeederReqData.pt_aes_crt_crypt = &rAesCrtCrypt;
        }
        
        else if ((FEEDER_IBC_PARAM_STRM_INFO == rFeederReqData.eFeederIBC) &&
            (rFeederReqData.u4WriteAddr) )
        {
            ASSERT( (rFeederReqData.u4ReadSize == sizeof(MTBUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T)) && 
                    ((UINT32)rArg.ai4Arg[0] < FEEDER_SOURCE_INVALID) );
            COPY_FROM_USER( rFeederReqData.u4WriteAddr, rTmpStrmInfo, sizeof(MTBUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T) );
            
            x_memcpy((void *)&_rIBC_Strm_Info, (void *)&rTmpStrmInfo, sizeof(MTBUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T));
            
            rFeederReqData.u4WriteAddr = (UINT32)&_rIBC_Strm_Info;        
        }
        else if (FEEDER_IBC_DASH_SET_INFO == rFeederReqData.eFeederIBC)
		{
			COPY_FROM_USER( rFeederReqData.pt_dash_set_info, rIBC_Dash_Set_Info, sizeof(FEEDER_CTRL_IBC_DASH_SET_INFO_T));
			rFeederReqData.u4WriteAddr = (UINT32)&rIBC_Dash_Set_Info;
			rFeederReqData.pt_dash_set_info = &rIBC_Dash_Set_Info;
		}
        else if((FEEDER_IBC_HDD_CONTENT_KEY == rFeederReqData.eFeederIBC) ||
                (FEEDER_IBC_DTCP_IP_KEY == rFeederReqData.eFeederIBC))
        {
            COPY_FROM_USER( rFeederReqData.pt_hdd_dtcp_ip_info, rIBC_Hdd_Dtcp_Ip_Info, sizeof(FEEDER_CTRL_HDD_DTCP_IP_INFO_T));
			rFeederReqData.u4WriteAddr = (UINT32)&rIBC_Hdd_Dtcp_Ip_Info;
			rFeederReqData.pt_hdd_dtcp_ip_info = &rIBC_Hdd_Dtcp_Ip_Info;
        }
    }
    
    #ifdef TIME_MEASUREMENT
        if (rFeederReqData.eFeederPrintTMS == FEEDER_PRINT_TMS_PRINT)
        {
            TMS_DIFF_EX(TMS_FLAG_WFD_LATENCY,"WFD_DATA:","io_mtfeeder _MTFEEDER_NotifyHandle");
        }
	#endif

    fgResult = FeederNotifyHandle(rArg.ai4Arg[0], rArg.ai4Arg[1], (UINT32)&rFeederReqData);
    
    if (fgResult)
        return MTR_OK;
    else
        return MTR_NOT_OK;
}

static MT_RESULT_T _MTFEEDER_SetMMP(unsigned long arg)
{
    if (FeederSetMMP((FeederSourceType)arg) == FEEDER_E_OK)
    {
        return MTR_OK;
    }
    return MTR_NOT_OK;
}

static MT_RESULT_T _MTFeeder_InitPreLoad(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    BOOL fgResult;
    UINT32	*pu4Addr;
    UINT32	u4Addr;    
    UINT32	*pu4Size;
    UINT32	u4Size;      
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

	pu4Addr = (UINT32 *)rArg.ai4Arg[1];
	pu4Size = (UINT32 *)rArg.ai4Arg[2];
	
    fgResult = FeederInitPreLoad(rArg.ai4Arg[0], &u4Addr, &u4Size);

    if (fgResult == FEEDER_E_OK)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
        COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
        COPY_TO_USER_ARG(pu4Size, u4Size, UINT32);

        return MTR_OK;
    }
    else
        return MTR_NOT_OK;
}

int mtal_ioctl_mtfeeder(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

	switch (cmd)
	{
        case MTAL_IO_FEEDER_INIT:
            break;
#if 0
        case MTAL_IO_FEEDER_SETDMXNFY:
            _MTFEEDER_SetDmxNfy(arg);
            break;
#endif
        case MTAL_IO_FEEDER_OPEN:
            i4Ret = _MTFEEDER_Open(arg);
            break;
        case MTAL_IO_FEEDER_SETINFO:
            i4Ret = _MTFEEDER_SetInfo(arg);
            break;
        case MTAL_IO_FEEDER_REGPULLDATAFNC:
            i4Ret = _MTFEEDER_RegPullDataFnc(arg);
            break;
        case MTAL_IO_FEEDER_SETFLUSH:
            i4Ret = _MTFEEDER_SetFlush(arg);
            break;
        case MTAL_IO_FEEDER_SETAPPQRYMODE:
            i4Ret = _MTFEEDER_SetAppQryMode(arg);
            break;
        case MTAL_IO_FEEDER_START:
            i4Ret = _MTFEEDER_Start(arg);
            break;
        case MTAL_IO_FEEDER_SETRING:
            i4Ret = _MTFEEDER_SetRing(arg);
            break;
        case MTAL_IO_FEEDER_STOP:
            i4Ret = _MTFEEDER_Stop(arg);
            break;
        case MTAL_IO_FEEDER_CLOSE:
            i4Ret = _MTFEEDER_Close(arg);
            break;
        case MTAL_IO_FEEDER_SETBAGTINFO:
            i4Ret = _MTFEEDER_SetBagtInfo(arg);
            break;
        case MTAL_IO_FEEDER_SETURIMODE:
            i4Ret = _MTFEEDER_SetUriMode(arg);
            break;
        case MTAL_IO_FEEDER_SETSKYPESRCFLG:
            i4Ret = _MTFEEDER_SetSkypeSrcFlg(arg);
            break;
        case MTAL_IO_FEEDER_GETBUFINFO:
            i4Ret = _MTFEEDER_GetBufInfo(arg);
            break;
        case MTAL_IO_FEEDER_GETBUFFULLNESS:
            i4Ret = _MTFEEDER_GetBufFullness(arg);
            break;
        case MTAL_IO_FEEDER_SETPRELOADDONE:
            i4Ret = _MTFEEDER_SetPreloadDone(arg);
            break;
        case MTAL_IO_FEEDER_NOTIFYHANDLE:
            i4Ret = _MTFEEDER_NotifyHandle(arg);
            break;
        case MTAL_IO_FEEDER_SETMMP:
            i4Ret = _MTFEEDER_SetMMP(arg);
            break;
        case MTAL_IO_FEEDER_INITPRELOAD:
            i4Ret = _MTFeeder_InitPreLoad(arg);
            break;  
        case MTAL_IO_FEEDER_GETMEMINFO:
            i4Ret = _MTFEEDER_GetFeederMemInfo(arg);
            break;
        default:
            i4Ret = MTR_NOT_OK;
            break;
	}
	return i4Ret;
}

