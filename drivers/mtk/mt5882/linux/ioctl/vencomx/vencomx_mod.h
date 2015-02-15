/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
/*
*
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#ifndef _VENCOMX_MOD_H_
#define _VENCOMX_MOD_H_

#include "x_typedef.h"
#include "vencomx_drv_if.h"

#ifdef CC_SUPPORT_VENC
extern VENCOMX_RESULT_T _VENCOMX_CopyData(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Init(void);
extern VENCOMX_RESULT_T _VENCOMX_Termint(void);
extern VENCOMX_RESULT_T _VENCOMX_Reset(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Set(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Get(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Open(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_SetParam(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_SetCtrl(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_DrvInit(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Run(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Stop(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_Close(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_EncFrameSync(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_EncFrameAsync(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_CreateBuffMgr(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_DeleteBuffMgr(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_BUFMGR_OBJ_Get(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_BUFMGR_OBJ_Free(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_FlushFb(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_InsertKeyframe(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_RegCb(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_UnlockBuf(unsigned long arg);
extern VENCOMX_RESULT_T _VENCOMX_EncSeqHdrSync(unsigned long arg);
#endif

#endif //#ifndef _VENCOMX_MOD_H_




