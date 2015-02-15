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
 * $RCSfile: io_mtcec.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: video, such as hdmi (and others), are centralized in
 *                   this file
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtcec.h"
#include <linux/mtal_ioctl.h>
#include "cec_if.h"
#include "drv_hdmi.h"

#include "cb_low.h"  // For callback function
#ifdef SYS_ANDROID_TV_TIF_SUPPORT		
#include "drvcust_if.h"
#include "eeprom_if.h"
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_cec argument error1.1\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_cec argument error1.2\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_cec argument error2.1\n");      \
        return MTR_ERR_INV;                                          \
    }     

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error2.2\n");      \
        return -1;                                          \
    }
	
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   sizeof(int)))							\
    {														\
	printk("mtal_ioctl_cec argument error 2.3\n"); 	\
	return -1;											\
    }
	
#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,	\
						   sizeof(int)))						\
    {														\
	printk("mtal_ioctl_cec argument error 2.4 \n"); 	 \
	return -1;											\
    }
		

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_cec argument error3\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_cec argument error4.1\n");      \
        return MTR_ERR_INV;                                          \
    }       

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_cec argument error4.2\n");      \
        return MTR_ERR_INV;                                          \
    } 
    
#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_cec argument error6\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_cec argument error7\n");      \
        return MTR_ERR_INV;                                          \
    }     
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_cec argument error8\n");      \
        return MTR_ERR_INV;                                          \
    } 

//-----------------------------------------------------------------------------
//typedef
//-----------------------------------------------------------------------------
/* Notify function */
typedef MT_RESULT_T (*PFN_MTCEC_RX_RESULT_NFY) (MT_CEC_FRAME_DESCRIPTION* pFrame);
typedef MT_RESULT_T (*PFN_MTCEC_TX_RESULT_NFY) (UINT8 u1Result, void* pTxtag);
#ifdef CECNFY5VSTATUS    
typedef MT_RESULT_T (*PFN_MTCEC_5V_STATUS_NFY) (UINT8 u1Switch, BOOL bIs5VOn);
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

/** MTCEC_Init()  Common Interface Initialization Fucntion
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_Init (unsigned long arg) 
{
    MTCEC_PRINT("_MTCEC_Init\n");
    
    CEC_Init();
    
    return MTR_OK;
}

static void  _MTCEC_TxResultNfy(UINT8 u1Result,  CEC_FRAME_DESCRIPTION *frame,void *arg)
{
    MTCEC_CB_T  rCecCbT;

    rCecCbT.u4Arg1 = (UINT32)u1Result;
    rCecCbT.u4Arg2 = (UINT32)frame->blocks.opcode;
    rCecCbT.u4Arg3 = (UINT32)arg;
    
    _CB_PutEvent(CB_MTAL_CEC_TX_RESULT_NFY, sizeof(MTCEC_CB_T), (void*)&rCecCbT);    
    MTCEC_PRINT(" -IO_CB_MTAL_CEC_TX_RESULT_NFY is done!Initiator(%d)->Destination(%d), u1Result: %d, pTag: %d, arg: %d\n", \
		frame->blocks.header.initiator,frame->blocks.header.destination,rCecCbT.u4Arg1, rCecCbT.u4Arg2, rCecCbT.u4Arg3);
}

static void _MTCEC_RxResultNfy(CEC_FRAME_DESCRIPTION *pFrame)
{
_CB_PutEvent(CB_MTAL_CEC_RX_RESULT_NFY, sizeof(CEC_FRAME_DESCRIPTION), (void*)pFrame);	
MTCEC_PRINT(" -IO_CB_MTAL_CEC_RX_RESULT_NFY is done! initiator = 0x%x, destination = 0x%x, opcode = 0x%x, size = 0x%x\n", 
	pFrame->blocks.header.initiator, 
	pFrame->blocks.header.destination,
	pFrame->blocks.opcode,
                pFrame->size,
                pFrame->txtag);
}

#ifdef CECNFY5VSTATUS    
static void _MTCEC_5VResultNfy(UINT8 u1Switch, BOOL b5VStatus,void *arg)
{
    MTCEC_CB_T  rCecCbT;

    rCecCbT.u4Arg1 = (UINT32)u1Switch;
    rCecCbT.u4Arg2 = (UINT32)b5VStatus;
    rCecCbT.u4Arg3 = (UINT32)arg;
    
    _CB_PutEvent(CB_MTAT_CEC_5V_STATUS_NFY, sizeof(MTCEC_CB_T), (void*)&rCecCbT);    
    MTCEC_PRINT(" -IO_CB_CB_MTAT_CEC_5V_STATUS_NFY is done! u1Switch: %d, b5VStatus : %d\n", rCecCbT.u4Arg1, rCecCbT.u4Arg2);
}
#endif
#ifdef CC_FOR_POC_DEMO
static void _MTCEC_Philips_5VResultNfy(UINT8 u1HdmiChangeType,UINT8 u1Switch, BOOL b5VStatus)
{
    MTCEC_CB_T  rCecCbT;

    rCecCbT.u4Arg1 = (UINT32)u1Switch;
    rCecCbT.u4Arg2 = (UINT32)b5VStatus;
    rCecCbT.u4Arg3 = u1HdmiChangeType;
    _CB_PutEvent(CB_MTAT_CEC_5V_STATUS_NFY, sizeof(MTCEC_CB_T), (void*)&rCecCbT);    
    MTCEC_PRINT(" -IO_CB_CB_MTAT_CEC_5V_STATUS_NFY is done! u1Switch: %d, b5VStatus : %d\n", rCecCbT.u4Arg1, rCecCbT.u4Arg2);
}
#endif
#ifdef SYS_ANDROID_TV_TIF_SUPPORT			
static void  _MTCEC_TxHeaderResultNfy(UINT8 u1Result, CEC_FRAME_DESCRIPTION *frame,void *arg)
{
    MTCEC_CB_T  rCecCbT;

    rCecCbT.u4Arg1 = (UINT32)u1Result;
    rCecCbT.u4Arg2 = (UINT32)frame->blocks.opcode;
    rCecCbT.u4Arg3 = (UINT32)arg;
    
    _CB_PutEvent(CB_MTAT_CEC_TX_HEADER_RESULT_NFY, sizeof(MTCEC_CB_T), (void*)&rCecCbT);    
    MTCEC_PRINT(" -IO_CB_MTAL_CEC_TX_HEADER_RESULT_NFY is done! Initiator(%d)->Destination(%d),u1Result: %d, pTag: %d, arg: %d\n",\
		frame->blocks.header.initiator,frame->blocks.header.destination,rCecCbT.u4Arg1, rCecCbT.u4Arg2, rCecCbT.u4Arg3);
}
#endif
/**
* @brief Register cec callback function
* @param eFuncType call back function type
* @param u4FuncPtr pointer to call back function
* @param u4Arg1
* @retval MTR_OK          Success.
* @retval MTR_NOT_OK  Fail.
*/
static MT_RESULT_T _MTCEC_RegCbFunc(unsigned long arg)
{
    //add driver implementation code here    
    MTAL_IOCTL_3ARG_T rArg;
    MTCEC_CB_FUNC_ENUM_T eFuncType;
    UINT32 u4FuncPtr;
    UINT32 u4Arg1;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    eFuncType = (MTCEC_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    u4FuncPtr = (UINT32)rArg.ai4Arg[1];
    u4Arg1 = (UINT32)rArg.ai4Arg[2];

    MTCEC_PRINT(" -IO_MTCEC_RegCb: eFuncType = %d, u4FuncPtr = %ul, u4Arg1 = %ul\n",
        eFuncType, u4FuncPtr, u4Arg1);

    switch(eFuncType)
    {
        case MTCEC_CB_FUNC_RX_RESULT:
            {
				
#ifdef SYS_ANDROID_TV_TIF_SUPPORT			
				CEC_SetRxCallback(_MTCEC_RxResultNfy, (void *)u4Arg1);
#else
                //pfnCecRxResultNfy = _MTCEC_TxResultNfy;
                CEC_SetRxCallback(_MTCEC_RxResultNfy,(void*)0);
#endif
            }		
            break;
        case MTCEC_CB_FUNC_TX_RESULT:
            { 
#ifdef SYS_ANDROID_TV_TIF_SUPPORT			
				CEC_SetTxCallback(_MTCEC_TxResultNfy, (void *)u4Arg1);
#else
                //pfnCecTxResultNfy = _MTCEC_RxResultNfy;
                CEC_SetTxCallback(_MTCEC_TxResultNfy,(void*)0);
#endif
            }
            break;
#ifdef CECNFY5VSTATUS
        case MTCEC_CB_FUNC_5V_STATUS:
            {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT			
				CEC_Set5VCallback(_MTCEC_5VResultNfy, (void *)u4Arg1);
#else			
                //pfnCec5VStatusNfy = _MTCEC_5VResultNfy;
                CEC_Set5VCallback(_MTCEC_5VResultNfy,(void*)0);
#endif
             }
            break;
#endif
		case MTCEC_CB_FUNC_PHILIPS_5V_STATUS:
			{
			
#ifdef CC_FOR_POC_DEMO			 
				CEC_SetPhilips5VCallback(_MTCEC_Philips_5VResultNfy);
#endif
			}
		break;

		case MTCEC_CB_FUNC_TX_HEADER_RESULT:
			{			
			#ifdef SYS_ANDROID_TV_TIF_SUPPORT			
				CEC_SetTxHeaderCallback(_MTCEC_TxHeaderResultNfy,(void *)u4Arg1);	
			#endif
			}
			break;
        default:
            return MTR_NOT_OK;
            break;
    }
    return MTR_OK;

}

//-----------------------------------------------------------------------------
/** MTCEC_SetMWReady()  Set fgCecMwReady when cec middleware is ready
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_SetMWReady(unsigned long arg)
{
#if defined(SYS_ANDROID_TV_TIF_SUPPORT)
    INT32 i4Arg;
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
	CEC_HALSetMWReady((UINT8)i4Arg);
#else
    CEC_SetMWReady();
#endif    
    MTCEC_PRINT("_MTCEC_SetMWReady\n");
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCEC_SetLogicAddress()  Set logical address for TV
 *  @param u1LogicalAddr, logical address to be set for TV
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_SetLogicAddress(unsigned long arg)
{
    INT32 i4Arg;
	
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SetLogicAddress((UINT8)i4Arg);
    MTCEC_PRINT("_MTCEC_SetLogicAddress\n");
    return MTR_OK;
}

static MT_RESULT_T  _MTCEC_ClrLogicAddress(unsigned long arg)
{
//    INT32 i4Arg;
	
   // USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    //COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_ClrLogicAddress();
    MTCEC_PRINT("_MTCEC_ClearLogicAddress\n");
    
    return MTR_OK;
}

static MT_RESULT_T  _MTCEC_SendStandby(unsigned long arg)
{
    INT32 i4Arg;
	
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SendStandby((UINT8)i4Arg);
    MTCEC_PRINT("_MTCEC_SendStandby\n");
    
    return MTR_OK;
}

static MT_RESULT_T  _MTCEC_SetWakeUp(unsigned long arg)
{
    INT32 i4Arg;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT		
	UINT32 u4Addr;
#endif
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SetWakeUp((UINT8)i4Arg);
    MTCEC_PRINT("_MTCEC_SendStandby\n");
#ifdef SYS_ANDROID_TV_TIF_SUPPORT // only for Android-L	
	if ((u4Addr = DRVCUST_InitGet(eEepromCECOneTouchPlayEnableOffset)) != 0x00)
	{
		EEPROM_Write(u4Addr, (UINT32)(&i4Arg), 1);
		if (EEPROM_Write(u4Addr, (UINT32)(&i4Arg), 1) == 0)
		{
		   return MTR_OK;
		}
	}
#endif
    return MTR_OK;
}

static MT_RESULT_T  _MTCEC_SendStandardCmd(unsigned long arg)
{
    INT32 i4Arg;
	
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SendStandardCmd((UINT8)i4Arg);
    MTCEC_PRINT("_MTCEC_SendStandby\n");
    return MTR_OK;
}
static MT_RESULT_T  _MTCEC_SetSysCtrl(unsigned long arg)
{
	INT32 i4Arg;
	
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SetSysVtrl((UINT8)i4Arg);
    MTCEC_PRINT("_MTCEC_SetSysCtrl\n");
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCEC_SetEnable()  Enable&disable cec function
 *  @param bEnable, enable | disable cec function
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_SetEnable(unsigned long arg)
{
    INT32 i4Arg;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT		
	UINT32 u4Addr;
#endif
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
    CEC_SetEnable((BOOL)i4Arg);
    MTCEC_PRINT("_MTCEC_SetEnable\n");
#ifdef SYS_ANDROID_TV_TIF_SUPPORT	//only for Android-L	
	if ((u4Addr = DRVCUST_InitGet(eEepromCECEnableOffset)) != 0x00)
	{
		EEPROM_Write(u4Addr, (UINT32)(&i4Arg), 1);
		if (EEPROM_Write(u4Addr, (UINT32)(&i4Arg), 1) == 0)
		{
		   return MTR_OK;
		}
	}
#endif
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCEC_GetLogicalAddr()  Get logical address for TV
 *  @param pu1LogicalAddr, pointer to cec logical address
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_GetLogicalAddr(unsigned long arg)
{
    UINT8 u1LogicalAddr = 0;
    u1LogicalAddr = CEC_GetLogicalAddr() ;	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_TO_USER_ARG(arg, u1LogicalAddr, INT32);
    MTCEC_PRINT("_MTCEC_GetLogicalAddr\n");
		 
    return MTR_OK;		 
}

//-----------------------------------------------------------------------------
/** MTCEC_CheckActiveSource()  Check if active source op code has been received
 *  @param pActiveSrc : pointer to active source information
 *  @retval TRUE DA=1 (data avaliable), FALSE DA=0 (Not avaliable)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCEC_CheckActiveSource(unsigned long arg)
{
    CEC_FRAME_ACTSRCREVED rActiveSrc;
    CEC_FRAME_ACTSRCREVED* prActiveSrc = (CEC_FRAME_ACTSRCREVED*)arg;
    x_memset((void *)&rActiveSrc, 0, sizeof(CEC_FRAME_ACTSRCREVED));
    CEC_CheckActiveSource(&rActiveSrc);

    USR_SPACE_ACCESS_VALIDATE_ARG(prActiveSrc, CEC_FRAME_ACTSRCREVED);
    COPY_TO_USER_ARG(prActiveSrc, rActiveSrc, CEC_FRAME_ACTSRCREVED);

    MTCEC_PRINT("_MTCEC_CheckActiveSource\n");
	 
    return MTR_OK;		 	
}

//-----------------------------------------------------------------------------
/** MTCEC_EnqueueTx()  put tx frame to be sent in tx queue 
 *  @param pFrame, pointer to tx frame 
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------

static MT_RESULT_T _MTCEC_EnqueueTx(unsigned long arg)
{  
    CEC_FRAME_DESCRIPTION  frame;
    x_memset((void *)&frame, 0, sizeof(CEC_FRAME_DESCRIPTION));
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, CEC_FRAME_DESCRIPTION);
    COPY_FROM_USER_ARG(arg, frame, CEC_FRAME_DESCRIPTION);    
    CEC_EnqueueTx(& frame);
    MTCEC_PRINT("_MTCEC_EnqueueTx\n");
    return MTR_OK;
}
//eggy for LG_CEC
#ifdef LG_CEC
static MT_RESULT_T _MTCEC_ReadRxCommand(unsigned long arg)
{    
    CEC_FRAME_DESCRIPTION  frame;
    CEC_FRAME_DESCRIPTION *pFrame = (CEC_FRAME_DESCRIPTION*)arg;
    x_memset((void *)&frame, 0, sizeof(CEC_FRAME_DESCRIPTION));
    CEC_ReadRxCommand(&frame);
    USR_SPACE_ACCESS_VALIDATE_ARG(pFrame, CEC_FRAME_DESCRIPTION);
    COPY_TO_USER_ARG(pFrame, frame, CEC_FRAME_DESCRIPTION);
    MTCEC_PRINT("_MTCEC_ReadRxCommand\n");
    return MTR_OK;
}

static MT_RESULT_T _MTCEC_ReadTxResult(unsigned long arg)
{    
    CEC_TX_RESULT t_txResult;
    CEC_TX_RESULT *pt_txResult = (CEC_TX_RESULT*)arg;
    x_memset((void *)&t_txResult, 0, sizeof(CEC_TX_RESULT));
    CEC_ReadTxResult(&t_txResult);
    USR_SPACE_ACCESS_VALIDATE_ARG(pt_txResult, CEC_TX_RESULT);
    COPY_TO_USER_ARG(pt_txResult, t_txResult, CEC_TX_RESULT);
    MTCEC_PRINT("_MTCEC_ReadTxResult\n");
    return MTR_OK;
}

static MT_RESULT_T _MTCEC_ReadResponse(unsigned long arg)
{    
    UINT16 ui2_cecResponse = 0;
    ui2_cecResponse = CEC_ReadResponse() ;	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_TO_USER_ARG(arg, ui2_cecResponse, INT32);
    MTCEC_PRINT("_MTCEC_ReadReponse\n");	 
    return MTR_OK;	
}
#endif
//-----------------------------------------------------------------------------
/** MTCEC_SetArcEnable()  Enable&disable cec arc out
 *  @param bEnable, enable | disable cec arc out
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
#if defined (CC_HDMI_ARC_GPIO_CTRL) || defined(CC_HDMI_ARC_SPDIF_CTRL)
static MT_RESULT_T  _MTCEC_SetArcEnable(unsigned long arg)
{
    INT32 i4Arg;
	
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, i4Arg);
#ifdef CC_HDMI_ARC_SPDIF_CTRL
    CEC_SetArcEnable((BOOL)i4Arg);
#else
    vHdmiSetARCOnOff((UINT8)i4Arg);
#endif
    MTCEC_PRINT("_MTCEC_SetArcEnable\n");
    
    return MTR_OK;
}
#endif

static MT_RESULT_T _MTCEC_GetConnected(unsigned long arg)
{
	UINT8 ePort;
	UINT8 *peConnect;
	UINT8 eConnect;
	
	MTAL_IOCTL_2ARG_T	 rArg;

	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	ePort = (UINT8)rArg.ai4Arg[0];
	peConnect = (UINT8 *)rArg.ai4Arg[1];

	eConnect = u1HDMI_Get5VStatus(ePort);
	
	MTCEC_PRINT("_MTCEC_GetConnected,port=%d,connect=%d  \n",ePort,eConnect);

	USR_SPACE_ACCESS_VALIDATE_ARG(peConnect,UINT8);
	COPY_TO_USER_ARG(peConnect,eConnect,UINT8);
	
    return MTR_OK;
}

int mtal_ioctl_mtcec(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTCEC_PRINT("-------cec ioctl cmd-------\r\n");
    switch (cmd)
    {
        case MTAL_IO_CEC_INIT:
            MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_INIT-------\r\n");
            _MTCEC_Init(arg);
            break;
        case MTAL_IO_CEC_REGCB:
            MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_REGCB-------\r\n");
            _MTCEC_RegCbFunc(arg);
            break;
        case MTAL_IO_CEC_SETMWREADY:	
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_SETMWREADY-------\r\n");
            _MTCEC_SetMWReady(arg);
            break;
        case MTAL_IO_CEC_SETLOGICADDRESS:
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_SETLOGICADDRESS-------\r\n");			
            _MTCEC_SetLogicAddress(arg);
            break;
		case MTAL_IO_CEC_ClearLOGICADDRESS:
			 MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_CLRLOGICADDRESS-------\r\n");			
            _MTCEC_ClrLogicAddress(arg);
			break;
		case MTAL_IO_CEC_SENDINGSTANDBY:
			MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_SENDINGSTANDBY-------\r\n");			
            _MTCEC_SendStandby(arg);
			break;
		case MTAL_IO_CEC_WAKEUP:
			MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_WAKEUP-------\r\n");			
            _MTCEC_SetWakeUp(arg);
			break;
		case MTAL_IO_CEC_STANDARDCOMMAND:
			MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_STANDARDCOMMAND-------\r\n");			
            _MTCEC_SendStandardCmd(arg);
			break;
		case MTAL_IO_CEC_SYSCTRL:
			MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_SYSCTRL-------\r\n");			
            _MTCEC_SetSysCtrl(arg);
			break;
        case MTAL_IO_CEC_SETENABLE:
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_SETENABLE-------\r\n");		 		
            _MTCEC_SetEnable(arg);
            break;
        case MTAL_IO_CEC_GETLOGICALADDRESS:
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_GETLOGICALADDRESS-------\r\n");			 		
            _MTCEC_GetLogicalAddr(arg);
            break;
        case MTAL_IO_CEC_CHECKACTIVESOURCE:
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_CHECKACTIVESOURCE-------\r\n");					 		
            _MTCEC_CheckActiveSource(arg);
            break;
        case MTAL_IO_CEC_ENQUEUETX:
	   MTCEC_PRINT("-------cec ioctl cmd: MTAL_IO_CEC_ENQUEUETX-------\r\n");							 
            _MTCEC_EnqueueTx(arg);
            break;
#ifdef LG_CEC
        case MTAL_IO_CEC_READ_RX_COMMAND:
            MTCEC_PRINT("--------cec ioctl cmd:MTAL_IO_CEC_READ_COMMAND-----------\r\n");
            _MTCEC_ReadRxCommand(arg);
            break;
        case MTAL_IO_CEC_READ_RESPONSE:
            MTCEC_PRINT("--------cec ioctl cmd:MTAL_IO_CEC_READ_REPONSE-----------\r\n");
            _MTCEC_ReadResponse(arg);
            break;
         case MTAL_IO_CEC_READ_TX_RESULT:
             MTCEC_PRINT("--------cec ioctl cmd:MTAL_IO_CEC_READ_TX_RESULT-----------\r\n");
             _MTCEC_ReadTxResult(arg);
            break;
#endif
#if defined (CC_HDMI_ARC_GPIO_CTRL) || defined(CC_HDMI_ARC_SPDIF_CTRL)
         case MTAL_IO_CEC_SETARCENABLE:		 	
		 	MTCEC_PRINT("--------cec ioctl cmd:MTAL_IO_CEC_SETARCENANLE-----------\r\n");
			_MTCEC_SetArcEnable(arg);
		 	break;
#endif
		case MTAL_IO_CEC_ISCONNECTES:
			MTCEC_PRINT("--------cec ioctl cmd:MTAL_IO_CEC_ISCONNECTES-----------\r\n");
			_MTCEC_GetConnected(arg);
			break;
        default: 
            break;
    }
    return eRet;
}

