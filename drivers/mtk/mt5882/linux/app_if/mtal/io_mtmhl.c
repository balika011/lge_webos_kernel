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
#include "mtmhl.h"
#include <linux/mtal_ioctl.h>
#include "mhl_if.h"
#include "cb_low.h"  // For callback function

#ifdef SYS_MHL_SUPPORT
#if defined (CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890)
#include "drv_cbus_if.h"
#elif defined(CC_MT5882)
#include "drv_cbus_cpi_if.h"
#endif
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
// Public functions
//-----------------------------------------------------------------------------

/** MTCEC_Init()  Common Interface Initialization Fucntion
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
#ifdef SYS_MHL_SUPPORT
extern x_vbus_drv_nfy_ocp_status rMhlOcpCbHandler;
MTMHL_CB_T  rMhlCbT;
extern void _MTMHL_MWNfyOcp(UINT8 port,MHL_OC_STATUS result);

MT_RESULT_T  _MTMHL_Init (unsigned long arg) 
{
    MTMHL_PRINT("_MTMHL_Init\n");
#if defined (CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    vCbusInit();
#endif
    MHL_SetOCPCallback(_MTMHL_MWNfyOcp);
    return MTR_OK;
}

void _MTMHL_MWNfyOcp(UINT8 port,MHL_OC_STATUS result)
{
    //MTCEC_CB_T  rCecCbT;
	rMhlCbT.u4Arg1 =(UINT32)port;
	rMhlCbT.u4Arg2 =(UINT32)result.MhlOCStatus;
	rMhlCbT.u4Arg3 =(UINT32)result.MhlDeviceConnect;
    MTMHL_PRINT("port%d:MhlOCStatus = %d,MhlDeviceConnect = %d\n",port,rMhlCbT.u4Arg2,rMhlCbT.u4Arg3);
}
MT_RESULT_T _MTMHL_GetOCPStatus(unsigned long arg)
{
   MHL_OC_STATUS* UserStatus;
   MHL_OC_STATUS KernelStatus;

   UserStatus = (MHL_OC_STATUS *)arg;    //ai4Arg[0]
   KernelStatus.MhlOCStatus = rMhlCbT.u4Arg2;
   KernelStatus.MhlDeviceConnect = rMhlCbT.u4Arg3;
  
   USR_SPACE_ACCESS_VALIDATE_ARG(UserStatus,INT32);
   COPY_TO_USER_ARG(UserStatus, KernelStatus,INT32);

   return MTR_OK;
   
}

#ifdef SYS_ANDROID_TV_TIF_SUPPORT
static MT_RESULT_T _MTMHL_SendMscSubCommand(unsigned long arg)
{  
    CBUS_MSC_MSG_COMMAND_T msg;
	BOOL uCbusStatus;
    x_memset((void *)&msg, 0, sizeof(CBUS_MSC_MSG_COMMAND_T));
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, CBUS_MSC_MSG_COMMAND_T);
    COPY_FROM_USER_ARG(arg, msg, CBUS_MSC_MSG_COMMAND_T);    
	uCbusStatus = fgCBUSGetDiscoveryOK();
	if(!uCbusStatus)
	{
		MTMHL_PRINT("_MTMHL_SendMscSubCommand FAIL! CBUS DISCONNECTED!\n");
		return MTR_NOT_OK;
	}
	vSendMscSubMsg(&msg);
    MTMHL_PRINT("_MTMHL_SendMscSubCommand\n");
    return MTR_OK;
}

static MT_RESULT_T _MTMHL_SendScratchPadData(unsigned long arg)
{  
    CBUS_MSG_DESCRIPTION_T msg;
	BOOL uCbusStatus;
    x_memset((void *)&msg, 0, sizeof(CBUS_MSG_DESCRIPTION_T));
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, CBUS_MSG_DESCRIPTION_T);
    COPY_FROM_USER_ARG(arg, msg, CBUS_MSG_DESCRIPTION_T); 
	uCbusStatus = fgCBUSGetDiscoveryOK();
	if(!uCbusStatus)
	{
		MTMHL_PRINT("_MTMHL_SendMscSubCommand FAIL! CBUS DISCONNECTED!\n");
		return MTR_NOT_OK;
	}
	vSendScratchPadData(&msg);
    MTMHL_PRINT("_MTMHL_SendScratchPadData\n");
    return MTR_OK;
}

static MT_RESULT_T _MTMHL_GetPeerMhlVersion(unsigned long arg)
{
	int peerMhlVersion;
	USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
	u1CbusGetPeerMhlVersion(&peerMhlVersion);
	COPY_TO_USER_ARG(arg,peerMhlVersion,sizeof(peerMhlVersion));
	
    return MTR_OK;
}
static MT_RESULT_T _MTMHL_SetOption(unsigned long arg)
{
	MHL_SET_OPTION_T eMhlSetOption;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MHL_SET_OPTION_T);
	COPY_FROM_USER_ARG(arg, eMhlSetOption, CBUS_MSG_DESCRIPTION_T);
	u1MhlSetOption(eMhlSetOption);
	
    return MTR_OK;
}

static void  _MTMHL_ScratchPadDataNfy(CBUS_MSG_DESCRIPTION_T *arMsg)
{
    _CB_PutEvent(CB_MTAL_MHL_SCRATCHPAD_DATA_NFY, sizeof(CBUS_MSG_DESCRIPTION_T), (void*)arMsg);    
    MTMHL_PRINT(" -_MTMHL_ScratchPadDataNfy is done! length: %d, arg: %d\n", arMsg->Size, arMsg->arg);
}

static void _MTMHL_MscSubCommandNfy(UINT8 u1Port,UINT8 u1Opcode,UINT8 data,void *arg)
{
	MTMHL_CB_T  rMhlCbT;

    rMhlCbT.u4Arg1 = (UINT32)u1Port;
    rMhlCbT.u4Arg2 = (UINT32)u1Opcode;
	rMhlCbT.u4Arg3 = (UINT32)data;
    rMhlCbT.u4Arg4 = (UINT32)arg;
	_CB_PutEvent(CB_MTAL_MHL_MSC_SUBCOMMAND_NFY, sizeof(MTMHL_CB_T), (void*)&rMhlCbT);	
	MTMHL_PRINT(" -_MTMHL_MscSubCommandNfy is done! Port = 0x%x, opcode = 0x%x, data = 0x%x, arg = 0x%x\n", 
	rMhlCbT.u4Arg1, rMhlCbT.u4Arg2, rMhlCbT.u4Arg3,rMhlCbT.u4Arg4);
}

static void _MTMHL_HotPlugNfy(UINT8 u1Port,UINT8 connect, void *arg)
{
    MTMHL_CB_T  rMhlCbT;

    rMhlCbT.u4Arg1 = (UINT32)u1Port;
    rMhlCbT.u4Arg2 = (UINT32)connect;
    rMhlCbT.u4Arg3 = (UINT32)arg;
    
    _CB_PutEvent(CB_MTAL_MHL_HOT_PLUG_NFY, sizeof(MTMHL_CB_T), (void*)&rMhlCbT);    
    MTMHL_PRINT(" -_MTMHL_HotPlugNfy is done! u1Switch: %d, b5VStatus : %d,arg =%d\n", rMhlCbT.u4Arg1, rMhlCbT.u4Arg2,rMhlCbT.u4Arg3);
}

static void _MTCEC_CbusModeNfy(UINT8 u1Port,UINT8 mode, void *arg)
{
    MTMHL_CB_T  rMhlCbT;

    rMhlCbT.u4Arg1 = (UINT32)u1Port;
    rMhlCbT.u4Arg2 = (UINT32)mode;
    rMhlCbT.u4Arg3 = (UINT32)arg;
    _CB_PutEvent(CB_MTAL_MHL_CBUS_MODE_NFY, sizeof(MTMHL_CB_T), (void*)&rMhlCbT);    
    MTMHL_PRINT(" -_MTCEC_CbusModeNfy is done! u1Switch: %d, cbus mode : %d,arg =%d\n", rMhlCbT.u4Arg1, rMhlCbT.u4Arg2,rMhlCbT.u4Arg3);
}

static void _MTCEC_VbusOverCurrentNfy(UINT8 u1Port,UINT8 ocStatus,void *arg)
{
    MTMHL_CB_T  rMhlCbT;

    rMhlCbT.u4Arg1 = (UINT32)u1Port;
    rMhlCbT.u4Arg2 = (UINT32)ocStatus;
    rMhlCbT.u4Arg3 = (UINT32)arg;
    _CB_PutEvent(CB_MTAL_MHL_VBUS_OVERCURRENT_NFY, sizeof(MTMHL_CB_T), (void*)&rMhlCbT);    
    MTMHL_PRINT(" -_MTCEC_VbusOverCurrentNfy is done! u1Switch: %d, cbus mode : %d,arg =%d\n", rMhlCbT.u4Arg1, rMhlCbT.u4Arg2,rMhlCbT.u4Arg3);
}


/**
* @brief Register cec callback function
* @param eFuncType call back function type
* @param u4FuncPtr pointer to call back function
* @param u4Arg1
* @retval MTR_OK          Success.
* @retval MTR_NOT_OK  Fail.
*/
static MT_RESULT_T _MTMHL_RegCbFunc(unsigned long arg)
{
    //add driver implementation code here    
    MTAL_IOCTL_3ARG_T rArg;
    MTMHL_CB_FUNC_ENUM_T eFuncType;
    UINT32 u4FuncPtr;
    UINT32 u4Arg1;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    eFuncType = (MTMHL_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    u4FuncPtr = (UINT32)rArg.ai4Arg[1];
    u4Arg1 = (UINT32)rArg.ai4Arg[2];

    MTMHL_PRINT(" -_MTMHL_RegCbFunc: eFuncType = %d, u4FuncPtr = %ul, u4Arg1 = %ul\n",
        eFuncType, u4FuncPtr, u4Arg1);

    switch(eFuncType)
    {
        case MTCEC_CB_FUNC_MSC_SUBCOMMAND:
            {
                vCbus_SetMscCallback(_MTMHL_MscSubCommandNfy, (void *)u4Arg1);
            }		
            break;
			
        case MTCEC_CB_FUNC_SCRATCHPAD_DATA:
            { 
                vCbus_SetScratchPadCallback(_MTMHL_ScratchPadDataNfy,(void *)u4Arg1);
            }
            break;
			
        case MTCEC_CB_FUNC_HOT_PLUG:
            {
                vCbus_SetHotPlugCallback(_MTMHL_HotPlugNfy,(void *)u4Arg1);
             }
            break;
			
		case MTCEC_CB_FUNC_CBUS_MODE:
			{
				vCbus_SetCbusModeCallback(_MTCEC_CbusModeNfy,(void *)u4Arg1);
			}
			break;
			
		case MTCEC_CB_FUNC_VBUS_OVERCURRENT:
			{
				vCbus_SetOverCurrentCallback(_MTCEC_VbusOverCurrentNfy,(void *)u4Arg1);
			}
			break;

        default:
            return MTR_NOT_OK;
            break;
    }
    return MTR_OK;

}
#endif

int mtal_ioctl_mtmhl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTMHL_PRINT("-------mtal_ioctl_mtmhl-------\r\n");
    switch (cmd)
    {
        case MTAL_IO_MHL_INIT:
            MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_INIT-------\r\n");
            eRet = _MTMHL_Init(arg);
            break;
		case MHL_IOCTL_GET_OVER_CURRENT_STA:
            MTMHL_PRINT("-------mhl ioctl cmd: MHL_IOCTL_GET_OVER_CURRENT_STA-------\r\n");
			eRet = _MTMHL_GetOCPStatus(arg);
			break;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
        case MTAL_IO_MHL_REGCB:
            MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_REGCB-------\r\n");
            eRet = _MTMHL_RegCbFunc(arg);
            break;
		case MTAL_IO_MHL_SENDMSCCOMMAND:
			MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_SENDMSCCOMMAND-------\r\n");
			eRet = _MTMHL_SendMscSubCommand(arg);
			break;
		case MTAL_IO_MHL_SENDMSCRATCHPAD:
			MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_SENDMSCRATCHPAD-------\r\n");
			eRet = _MTMHL_SendScratchPadData(arg);
			break;
		case MTAL_IO_MHL_GET_PEER_MHL_VERSION:
			MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_GET_PEER_MHL_VERSION-------\r\n");
			eRet = _MTMHL_GetPeerMhlVersion(arg);
			break;
		case MTAL_IO_MHL_SET_OPTION:
			MTMHL_PRINT("-------mhl ioctl cmd: MTAL_IO_MHL_SET_OPTION-------\r\n");	
			eRet = _MTMHL_SetOption(arg);
			break;
#endif
        default: 
            break;
    }
    return eRet;
}
#endif

