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
 * AGREES TOsets THE FOLLOWING:                                                   *
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
 * $RCSfile: io_MTFE_ATD.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_MTFE_ATD.c
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "io_mtfe_pd_def.h"
#include "mtfe_common.h"

#include "tuner_if.h"
// Guocheng add, for the TunerATD module, 2010/02/24
#include "pi_atd_if.h"         
#include "pd_atd_if.h"         
#include "pi_demod_atd.h"

#include "cb_low.h"  // For callback function

#include <linux/mtal_ioctl.h>

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
	  printk("usr_space_access_validate fail\n");     \
        printk("mtal_ioctl_ntsc argument error\n");     \
        return MTR_ERR_INV;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG_BY_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   (size)))              \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");     \
        return MTR_ERR_INV;                                          \
    }
	
#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
       printk("COPY_TO_USER_ARG fail\n");     \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_TO_USER_ARG_BY_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
       printk("COPY_TO_USER_ARG fail\n");     \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
	 printk("COPY_TO_USER_ARG fail\n");     \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG_BY_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_ntsc argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
	
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgATDInit=FALSE;
/*all variable used in this file should put into pMTPdCtx exclusive of Exclusive_DigiAnaSema and u1ExclSemaInit*/
static HANDLE_T    Exclusive_DigiAnaSema=0;
static UINT8       u1ExclSemaInit=0;
static ATD_PD_CTX_T      *pAtvPdCtx = NULL;
static ATD_CTX_T *pDemodCtx;
static BOOL fgIsA2MMode=FALSE;
#ifdef CC_SUPPORT_EU_US_COMPATIABLE
extern UINT8 DynamicSwitch_DTD_DVBT_Mode;
static BOOL fgIsUSMode=FALSE;
#endif
#ifdef CC_SUPPORT_SIF_BYPASS
static BOOL fgSifBypassInited = FALSE;
extern UINT8 fgSifBypassEnabled;
static UINT8 Pre_u1SubSysIdx = MOD_TYPE_UNKNOWN;
#endif
//-----------------------------------------------------------------------------
// extern function
//-----------------------------------------------------------------------------
#ifdef CC_FESBTVD//SBTVD
extern void Get_MTFE_SBTVD_Sema_Ex_DiAna(HANDLE_T *sema);
#define MTAL_GET_DTD_SEMA_EX_DIANA  Get_MTFE_SBTVD_Sema_Ex_DiAna
#elif defined(CC_FEISDB)   // Japan ARIB ISDB-T
extern void Get_MTFE_ISDBT_Sema_Ex_DiAna(HANDLE_T *sema);
#define MTAL_GET_DTD_SEMA_EX_DIANA  Get_MTFE_ISDBT_Sema_Ex_DiAna
#elif defined(CC_FEDTD) // DTD
extern void Get_MTFE_ATSC_Sema_Ex_DiAna(HANDLE_T *sema);
#define MTAL_GET_DTD_SEMA_EX_DIANA  Get_MTFE_ATSC_Sema_Ex_DiAna
#else
#define MTAL_GET_DTD_SEMA_EX_DIANA(x)
#endif
#ifdef CC_SUPPORT_INT_EXT_ATD_COMPATIABLE//d120522_Haibo:select internal and external analog demod dynamically
extern void vADCInit(void);
static UINT8 fgUseInternalATD=FALSE;
#endif
//extern UCHAR u1DbgLevel;
//-----------------------------------------------------------------------------
// Static function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** _MTFE_ATD_Open
 *  Initialize demod device
 *  @param  void
 *  @retval MTR_NOT_OK     demod initalize fail
 *  @retval MTR_OK              demod initalize successfully
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_Open(VOID)
{
    INT32 i4Status;

	//u1DbgLevel=3;
	
    MTFE_NTSC_PRINT("In kernel %s\n", __FUNCTION__);
#ifdef CC_SUPPORT_INT_EXT_ATD_COMPATIABLE
    if(FALSE == fgSifBypassEnabled)//If ATD is used to do SIF bypass, don't tell other module  ATD is in use.
    {
    	fgUseInternalATD=TRUE;//To indicate now it is internal ATD 
    	
    	vADCInit();//d120514_Haibo:Invoke audio SIF input to set correct input
    }
#endif	
    if(_fgATDInit==TRUE)//d110831_Haibo:To avoid this called more than one time
	{
        MTFE_NTSC_PRINT("_fgATDInit==TRUE, do _MTFE_ATD_Close() first\r\n");
        return (MTR_OK);
    }

   TunerSetMwIntf(cMTAL_IF);//Set MW interface type
   
   i4Status = TunerOpen();
	
    if (i4Status < DRVAPI_TUNER_OK)
    {   
        MTFE_NTSC_PRINT("TunerOpen failed (%d)\n", i4Status);
        return (MTR_NOT_OK);
    }	
    //get Analog pd Ctx 
    pAtvPdCtx = (ATD_PD_CTX_T*)TunerGetCtx(ANALOG_FE);
    if(NULL==pAtvPdCtx)
	{
	    MTFE_NTSC_PRINT("TunerGetCtx failed\n");
        return (MTR_NOT_OK);
	}
	pDemodCtx=(ATD_CTX_T *)(pAtvPdCtx->sAtdPiCtx);
	
    MTAL_GET_DTD_SEMA_EX_DIANA(&Exclusive_DigiAnaSema);
    if(Exclusive_DigiAnaSema!=0){
        u1ExclSemaInit = 1;
    }
    _fgATDInit = TRUE;
    return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_Connect
 *  resume analogue demod since it will be sleep mode when connect to DTV or other source  
 *  @retval  MTR_NOT_OK  demod connect fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to resume demod
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_Connect(VOID)
{
   //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
  MTFE_NTSC_PRINT("In kernel %s\n", __FUNCTION__);
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
  if(ATD_ChipInit(pDemodCtx))
  {
    mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
    MTFE_NTSC_PRINT("Connect fail\n");
	return MTR_NOT_OK;
  }
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** MTFE_ATD_SetA2MMode
 *  To notify driver if now it is A2M mode(A2 Korea) 
 *  @param  fgIsA2MMode   to indicate if now is A2M mode        
 *  @retval  MTR_NOT_OK  fail
 *  @retval  MTR_OK           successul to set A2 mode
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T  _MTFE_ATD_SetA2MMode(unsigned long arg)
{
      fgIsA2MMode=(BOOL)arg;
      MTFE_NTSC_PRINT("In kernel %s fgIsA2MMode=%d\n", __FUNCTION__,fgIsA2MMode);
	  return MTR_OK;
}
#ifdef CC_SUPPORT_EU_US_COMPATIABLE
//-----------------------------------------------------------------------------
/** MTFE_ATD_SetUSMode
 *  To notify driver if now it is US mode(EU/US Co-Image) 
 *  @param  fgIsUSMode   to indicate if now is US mode        
 *  @retval  MTR_NOT_OK  fail
 *  @retval  MTR_OK           successul to select US mode
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T  _MTFE_ATD_SetUSMode(unsigned long arg)
{
      fgIsUSMode=(BOOL)arg;
      MTFE_NTSC_PRINT("In kernel %s fgIsUSMode=%d\n", __FUNCTION__,fgIsUSMode);
	  if (TRUE == fgIsUSMode)
	  	{
	  	DynamicSwitch_DTD_DVBT_Mode = 0;
	  	}
	  return MTR_OK;
}
#endif
/** _MTFE_ATD_SetTunerIF
 *  Indicate the tuner IF frequency(unit is KHz)  to config demod 
 *  @param  arg        the  IF frequency(KHz) tuner output
 *  @retval  MTR_NOT_OK  set tuner IF fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to config tuner IF frequency
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_SetTunerIF(unsigned long arg)
{
  UINT16 u2TunerIFInKHz=(UINT16)arg;
  #if defined(CC_MT5398)||defined(CC_MT5399) ||defined(CC_MT5890)  ||defined(CC_MT5882) 
  UINT8 u1SubSysIdx;
  #endif
    //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	  return MTR_NOT_OK;
  }
   MTFE_NTSC_PRINT("In kernel %s\n", __FUNCTION__);
  //d110831_Haibo:Check if the address in user space can access
 // USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT16);
  //COPY_FROM_USER_ARG(arg,u2TunerIFInKHz,UINT16);
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
  pDemodCtx->pTCtx->u2IF_Freq_A=u2TunerIFInKHz;
   #if defined(CC_MT5398)||defined(CC_MT5399) ||defined(CC_MT5890) ||defined(CC_MT5882) 
   u1SubSysIdx=pDemodCtx->u1SubSysIdx;
  if(MOD_NTSC_M==u1SubSysIdx)
  {
    if(fgIsA2MMode)//A2 Korea will be 7M to avoid sound carrier was cut off by CHF 
    {
	   ATD_SetCHF(pDemodCtx,7);//set channel filter BW as 7 when country is Korea
	   MTFE_NTSC_PRINT("Set CHF 7 MHz\n");
    }
	else
	{
	   ATD_SetCHF(pDemodCtx,6);//set channel filter BW when country is US
	   MTFE_NTSC_PRINT("Set CHF 6 MHz\n");
	}
  }
  else
  {
     ATD_SetCHF(pDemodCtx,8);
  }
  #endif
  ATD_SetIF(pDemodCtx);//d120410_Haibo:It must be set after channel filter reset
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_SetSystem
 *  To load the ATD default register setting with specified sub system
 *  @param  arg        the  sub system to set
 *  @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to config tuner IF frequency
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_SetSystem(unsigned long arg)
{
  UINT8 u1SubSysIdx=(UINT8)arg;//d110831_Haibo:For US ATV,just set NTSC M
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	  return MTR_NOT_OK;
  }
  
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  ATD_SetSystem(pDemodCtx,u1SubSysIdx);
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_LoadEQScript
 *  To load the atd eq script with specified CVBS response
 *  @param  arg        the  pointer to eq script array
 *  @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to load the eq script to demod register 
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_LoadEQScript(unsigned long arg)
{
  UINT8 u1EqScriptArr[206]={0};
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
    //d110831_Haibo:Check if the address in user space can access
  USR_SPACE_ACCESS_VALIDATE_ARG_BY_SIZE(arg,sizeof(u1EqScriptArr));
  COPY_FROM_USER_ARG_BY_SIZE(arg,u1EqScriptArr[0],sizeof(u1EqScriptArr));
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  ATD_LoadEQCoef(pDemodCtx,u1EqScriptArr);
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_SetRegister
 *  To set the tuner relative registers such as PRA parameters to ATD register 
 *  @param  arg.ai4Arg[0]         The register address
 *  @param  arg.ai4Arg[1]          The data buffer to set
 *  @param  arg.ai4Arg[2]         The data count to set
 *  @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to set the register
 *  @retval  MTR_PARAMETER_ERROR     The parameter from MW/AP is wrong
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_SetRegister(unsigned long arg)
{
 MTAL_IOCTL_3ARG_T rArg;
 UINT16 u2RegAddr;
 UINT8 *pu1DataBuff=NULL;
 UINT8 *pu1KerBuff=NULL;
 UINT8 u1DataCnt=0;
 UINT8 u1Index=0;
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
     MTFE_NTSC_PRINT("In kernel %s\n", __FUNCTION__);
    //d110831_Haibo:Check if the address in user space can access
  USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
  COPY_FROM_USER_3ARG(arg,rArg);
  
  u2RegAddr=(UINT16)(rArg.ai4Arg[0]);
  pu1DataBuff=(UINT8 *)(rArg.ai4Arg[1]);
  u1DataCnt=(UINT8)(rArg.ai4Arg[2]);
  
  if((NULL==pu1DataBuff) || (0==u1DataCnt))
  {
  MTFE_NTSC_PRINT("pu1DataBuff is NULL or data cnt is 0\n");
  return MTR_PARAMETER_ERROR;
  }
  else
  {
	USR_SPACE_ACCESS_VALIDATE_ARG_BY_SIZE(pu1DataBuff,u1DataCnt*sizeof(u1DataCnt));
	pu1KerBuff=(UINT8 *)x_mem_calloc(1,u1DataCnt*sizeof(UINT8));
	if(NULL==pu1KerBuff)
	{
	 MTFE_NTSC_PRINT("memory allocation fail\n");
	 return MTR_NOT_OK;//d110831_Haibo:memory allocate fail
	}
    if (copy_from_user((void *) pu1KerBuff, (const void __user *) pu1DataBuff, u1DataCnt*sizeof(u1DataCnt)))
   {
      MTFE_NTSC_PRINT("copy data from user space fail\n");
      x_mem_free(pu1KerBuff);
      return MTR_PARAMETER_ERROR; 	  
   }
  }
  MTFE_NTSC_PRINT("Arg: u2RegAddr 0x%03X,Len %d\n",u2RegAddr,u1DataCnt);
  MTFE_NTSC_PRINT("Value:\n");
  for(u1Index=0;u1Index<u1DataCnt;u1Index++)
  {
    MTFE_NTSC_PRINT("Arg: 0x%02X\n",*(pu1KerBuff+u1Index));
  }
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  if(ATD_SetReg(pDemodCtx,u2RegAddr,pu1KerBuff,u1DataCnt))
  {
    mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
	x_mem_free(pu1KerBuff);
    MTFE_NTSC_PRINT("write register fail\n");   
    return MTR_NOT_OK;
  }
  x_mem_free(pu1KerBuff);
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_Acquire
 *  Set demod to start to acquire signal
 *  @param  arg      To indicate now if it is channel scan(TRUE),if it is channel change(FALSE)
*   @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL).
                                         or demod acquire signal fail(demod work abnormal)
 *  @retval  MTR_OK          successul to acquire signal
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_Acquire(unsigned long arg)
{
 UINT8 u1ChScan=(UINT8)arg;
  
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
      MTFE_NTSC_PRINT("In kernel %s\n", __FUNCTION__);
    //d110831_Haibo:Check if the address in user space can access
  //USR_SPACE_ACCESS_VALIDATE_ARG(&arg,UINT8);
  //COPY_FROM_USER_ARG(&arg,u1ChScan,UINT8);
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  //u1ChScan=FALSE;//for NTSC,no need to set scan mode
  if(ATD_StartAcq(pDemodCtx,u1ChScan))
  {
      MTFE_NTSC_PRINT("demod acquire fail\n");
      mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
	  return MTR_NOT_OK;
  }
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_GetAFTWinSts
 *  Check if there is  norminal VIF is in the range of AFT window(+-1.5MHz)
 *  @param  u1AFTWSts      The AFT window lock status to return to user space(0:No VIF in +-1.5MHz;1:norminal VIF in +-1.5MHz)
*   @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL).
                                         or user space data access fail
 *  @retval  MTR_OK          successul to get AFT window lock status
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_GetAFTWinSts(unsigned long arg)
{
  UINT8 u1AFTWinLockSts=FALSE;
  
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  if(ATD_GetVIFLock(pDemodCtx,TRUE)>0)
  {
  u1AFTWinLockSts=TRUE;//d110831_Haibo:Now PIF is in the +-1MHz AFT window
  }
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  
      //d110831_Haibo:Check if the address in user space can access
  USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT8);
  COPY_TO_USER_ARG(arg,u1AFTWinLockSts,UINT8);
  
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_GetCFOKHz
 *  Get the picture carrier offset(KHz) between current tuner LO with norminal picture carrier.
   It should be called after _MTFE_ATD_GetAFTWinSts is TRUE,otherwise,the estimated CFO value will be invalid 
 *  @param  arg      The CFO to return to user space(unit is KHz)
*   @retval  MTR_NOT_OK  set operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL).
                                         or user space data access fail
 *  @retval  MTR_OK          successul to get current CFO
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_GetCFOKHz(unsigned long arg)
{
  INT32 i4CFOKHz=0;
  
  //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);//d110831_Haibo:to protect multi-access
  i4CFOKHz=pDemodCtx->s4Cfo;
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  
  //d110831_Haibo:Check if the address in user space can access
  USR_SPACE_ACCESS_VALIDATE_ARG(arg,INT32);
  COPY_TO_USER_ARG(arg,i4CFOKHz,INT32);
  
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_DisConnect
 *  Set demod as sleep mode.It shoud be invoked when switch from ATV to other source(except AV) 
 *  @retval  MTR_NOT_OK  demod connect fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK          successul to sleep demod
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_DisConnect(VOID)
{
   //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
#ifdef CC_SUPPORT_SIF_BYPASS
  fgSifBypassInited = FALSE;
  fgSifBypassEnabled = FALSE;
#endif  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
  ATD_DisConnect(pDemodCtx);
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_Patch
 *  To monitor current signal status and load corresponding register setting into demod.It should be invoked continuously after signal is stable
 *  @param  VOID        
 *  @retval  MTR_NOT_OK  operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK           successul to Patch demod 
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_Patch(VOID)
{
    //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
  
  mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
  ATD_Patch(pDemodCtx);
  mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
  return MTR_OK;
}
//-----------------------------------------------------------------------------
/** _MTFE_ATD_GetVer
 *  Get the ATD driver version string(the string length is 8)
 *  @param  VOID        
 *  @retval  MTR_NOT_OK   fail to get driver version
 *  @retval  MTR_OK           successul to get driver version
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_GetVer(unsigned long arg)
{
    CHAR * pStrVer=NULL; 
	CHAR * pStrVerInUsr=(CHAR *)arg;
    //d110831_Haibo:Check if the pointer is NULL to avoid kernel crash caused by dereference NULL pointer 
  if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
  {
    MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	return MTR_NOT_OK;
  }
  pStrVer=TunerGetVer(ANALOG_FE);
  if(NULL==pStrVer)
  {
    MTFE_NTSC_PRINT("The verion string is NULL\n");
	return MTR_NOT_OK;
  }
  MTFE_NTSC_PRINT("StrVer:%s\n",pStrVer);
  USR_SPACE_ACCESS_VALIDATE_ARG_BY_SIZE(pStrVerInUsr,8*sizeof(CHAR));//the version string length is 8(include \0)
  COPY_TO_USER_ARG_BY_SIZE(pStrVerInUsr,pStrVer[0],8*sizeof(CHAR));
  return MTR_OK;
}
#ifdef CC_SUPPORT_INT_EXT_ATD_COMPATIABLE
//-----------------------------------------------------------------------------
/** _MTFE_ATD_IsAlive
 *  Check if ATD is being used now 
 *  @param  arg      The ATD runing status return to use space (TRUE:internal ATD is alive.FALSE:internal ATD is not used)
*   @retval  MTR_NOT_OK   operation fail
 *  @retval  MTR_OK          successul to get ATD runing status
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_IsAlive(unsigned long arg)
{
     USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT8);
     COPY_TO_USER_ARG(arg,fgUseInternalATD,UINT8);
	 
	 return MTR_OK;
}

BOOL IsATDAlive(void)
{
   return fgUseInternalATD;
}
#endif
//-----------------------------------------------------------------------------
/** _MTFE_ATD_SetSnowMode
 *  To set  TVD snow mode when signal is not exist
 *  @param  arg           if open TVD snow mode,0 close TVD snow mode, 1 open TVD snow mode        
 *  @retval  MTR_NOT_OK  operation request  fail due to demod device is not initialized or initialized fail(then the pointer will be NULL)
 *  @retval  MTR_OK           successul to set snow mode
 */
 //----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_ATD_SetSnowMode(unsigned long arg )
{
    UINT8 fgSnowOn =(UINT8)arg;
	MTFE_NTSC_PRINT("MTFE_ATD_SetSnowMode = %d",fgSnowOn);
	if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
	 {
	   MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	   return MTR_NOT_OK;
	 }
	mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
	ATD_SetSnowMode(pDemodCtx, fgSnowOn);
	mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
    return MTR_OK;
}
#ifdef CC_SUPPORT_SIF_BYPASS
static MT_RESULT_T _MTFE_ATD_SifBypass(unsigned long arg)
{
    UINT8 u1SubSysIdx = (UINT8)arg;
    if((!fgSifBypassInited) || (Pre_u1SubSysIdx != u1SubSysIdx))
    {
        fgSifBypassEnabled = TRUE;
        _MTFE_ATD_Open();
		MTFE_NTSC_PRINT("[_MTFE_ATD_SifBypass]u1SubSysIdx is %d!\n",u1SubSysIdx);
		if(NULL==pDemodCtx)
	    {
	        MTFE_NTSC_PRINT("pDemodCtx is NULL\n");
	        return MTR_NOT_OK;
	    }
		pDemodCtx->u1SubSysIdx = u1SubSysIdx;
		ATD_ChipInit(pDemodCtx);
		ATD_SetSystem(pDemodCtx,pDemodCtx->u1SubSysIdx);
		ATD_StartAcq(pDemodCtx,FALSE);
		ATD_BypassSIF2Aud(pDemodCtx,pDemodCtx->u1SubSysIdx);
        fgSifBypassInited = TRUE;
    }
	Pre_u1SubSysIdx = u1SubSysIdx;
	return MTR_OK;

}
#endif

static MT_RESULT_T _MTFE_ATD_GetSNR(unsigned long arg)
{
	UINT32 u4SNR=0;
	if((NULL==pAtvPdCtx) || (NULL==pDemodCtx))
	 {
	   MTFE_NTSC_PRINT("pAtvPdCtx is NULL\n");
	   return MTR_NOT_OK;
	 }
	
	mcSEMA_LOCK_MTFE(pAtvPdCtx->hHalLock);
	u4SNR = ATDPI_GetTVDNoiseLevel(pDemodCtx);
	mcSEMA_UNLOCK_MTFE(pAtvPdCtx->hHalLock);
	 //d110831_Haibo:Check if the address in user space can access
    USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT32);
    COPY_TO_USER_ARG(arg,u4SNR,UINT32);
	return MTR_OK;
}

int mtal_ioctl_mtfe_atd(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTFE_NTSC_PRINT("-------atd ioctl cmd-------\r\n");
    if(u1ExclSemaInit==1){
        mcSEMA_LOCK_MTFE_S(Exclusive_DigiAnaSema,"Ex_DiAna");
    }

	switch (cmd)
	{
		case MTAL_IO_ATD_OPEN:
			eRet= _MTFE_ATD_Open();
            break;
		case MTAL_IO_ATD_ACQUIRE:
			eRet= _MTFE_ATD_Acquire(arg);
            break;
		case MTAL_IO_ATD_CONNECT:
			eRet= _MTFE_ATD_Connect();
            break;
		case MTAL_IO_ATD_DISCONNECT:
		    eRet= _MTFE_ATD_DisConnect();
            break;
		case MTAL_IO_ATD_GETAFTWINSTS:
		    eRet= _MTFE_ATD_GetAFTWinSts(arg);
            break;
		case MTAL_IO_ATD_GETCFOKHZ:
		    eRet= _MTFE_ATD_GetCFOKHz(arg);
            break;
		case MTAL_IO_ATD_LOADEQ:
		    eRet= _MTFE_ATD_LoadEQScript(arg);
            break;
		case MTAL_IO_ATD_SETSYSTEM:
			eRet= _MTFE_ATD_SetSystem(arg);
            break;
		case MTAL_IO_ATD_SETIF:
			eRet= _MTFE_ATD_SetTunerIF(arg);
            break;
		case MTAL_IO_ATD_SETREG:
		    eRet= _MTFE_ATD_SetRegister(arg);
            break;
		case MTAL_IO_ATD_PATCH:
 		    eRet= _MTFE_ATD_Patch();
            break;
#ifdef CC_SUPPORT_INT_EXT_ATD_COMPATIABLE			
		case MTAL_IO_ATD_CHECK_ALIVE:
			eRet=_MTFE_ATD_IsAlive(arg);
			break;
#endif	
		case MTAL_IO_ATD_SETSNOWMODE:
			eRet= _MTFE_ATD_SetSnowMode(arg);
			break;
        case MTAL_IO_ATD_GET_VER:
            eRet=_MTFE_ATD_GetVer(arg);
            break;		
		case MTAL_IO_ATD_SETA2MMODE:
   		    eRet= _MTFE_ATD_SetA2MMode(arg);
            break;
#ifdef CC_SUPPORT_EU_US_COMPATIABLE
		case MTAL_IO_ATD_SETUSMODE:
   		    eRet= _MTFE_ATD_SetUSMode(arg);
            break;
#endif
#ifdef CC_SUPPORT_SIF_BYPASS
        case MTAL_IO_ATD_BYPASSSIF:
            eRet = _MTFE_ATD_SifBypass(arg);
            break;
#endif
		case MTAL_IO_ATD_Get_SNR:
            eRet=_MTFE_ATD_GetSNR(arg);
            break;
		default:
		    break;
	}

    if(u1ExclSemaInit==1){
        mcSEMA_UNLOCK_MTFE_S(Exclusive_DigiAnaSema, "Ex_DiAna");
    }
	return eRet;
}
    

