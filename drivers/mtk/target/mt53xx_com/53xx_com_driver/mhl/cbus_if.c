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
 * $RCSfile: cbus_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file cec_if.c
 *  This C file implements the cbus hardware protocol functions.  
 */
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "cec_drv_if.h"
#ifdef SYS_MHL_SUPPORT
#include "cbus_if.h"
#include "mhl_if.h"
#ifndef CC_MHL_HDMI_SWITCH
#ifdef CC_MT5882
#include "drv_cbus_cpi_if.h"
#else
#include "drv_cbus_if.h"
#endif
#endif
#endif
#include "x_os.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------
#if defined(SYS_MHL_SUPPORT)
UINT16 _Cbus_Status;
#endif

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
#ifdef SYS_MHL_SUPPORT
static CEC_FRAME_DESCRIPTION *prCbusActiveTxFrame;
static CEC_FRAME_DESCRIPTION *prCbusActiveRxFrame;
static UINT8 _Cbus_VerifyFrame(CEC_FRAME_DESCRIPTION *frame);

static UINT32 u4CbusMsg = OPCODE_NONE;
static UINT32 u4CbusMsg1 = OPCODE_NONE;
UINT8 u1CbusLa = 13; // RESERVED 12, 13
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------
#ifdef SYS_MHL_SUPPORT
void CBUS_PrintFrame(const CEC_FRAME_DESCRIPTION *pFrame, BOOL fgTxFrame)
{
    UINT8 i;

    if (!pFrame)
    {
        return;
    }
    
    LOG(0,"%s CBUS Frame start ========>.\n", ((fgTxFrame) ? "Tx": "Rx"));

    LOG(0,"CBUS:%s Size=%d, sendidx=%d, reTXcnt=%d, txtag=0x%08X.\n",
        ((fgTxFrame) ? "Tx": "Rx"),
        pFrame->size, pFrame->sendidx, pFrame->reTXcnt, (UINT32)pFrame->txtag);

    LOG(0,"CBUS:%s Initiator=0x%X, Destination=0x%X.\n",
        ((fgTxFrame) ? "Tx": "Rx"),
        pFrame->blocks.header.initiator, pFrame->blocks.header.destination);

    if (pFrame->size > 1)
    {
        LOG(0,"CBUS:%s Opcode=0x%X.\n", 
            ((fgTxFrame) ? "Tx": "Rx"), pFrame->blocks.opcode);
    }

    if ((pFrame->size > 2) && (pFrame->size <= 16))
    {
        for (i = 0; i < (pFrame->size - 2); i++)
        {
            LOG(0,"CBUS:%s Operand[%d]=0x%02X.\n", 
                ((fgTxFrame) ? "Tx": "Rx"), i, pFrame->blocks.operand[i]);
        }
    }

    LOG(0,"CBUS:%s CBUS Frame end  <========.\n", ((fgTxFrame) ? "Tx": "Rx"));
}
#endif

//---------------------------------------------------------------------------
// Locial functions
//---------------------------------------------------------------------------
#ifdef SYS_MHL_SUPPORT
void vCbusPrintFrame(CBUS_MSG_DESCRIPTION_T *pMsg)
{
    UINT8 i = 0;
    
    if (!pMsg)
    {
        return;
    }
    
    LOG(0, "CBUS Message\n");
    LOG(0, "msc: 0x%2x\n", pMsg->MscMsg);
    LOG(0, "Opcode: 0x%2x\n", pMsg->Opcode);

    for (i = 0 ; i < pMsg->Size ; i++) {
        LOG(0, "Data: 0x%2x\n", pMsg->Data[i]);
    }
}
#endif

#ifdef SYS_MHL_SUPPORT
extern UINT32 _u4DevieAdapterID;
void vCbusDrvNfyDataArrival(CBUS_MSG_DESCRIPTION_T *frame)
{
    CEC_FRAME_DESCRIPTION rMhlframe;
    CEC_FRAME_DESCRIPTION *prMhlframe = NULL;
	UINT8 i=0;

    x_memset(&rMhlframe, 0, sizeof(CEC_FRAME_DESCRIPTION));
    
   vCbusPrintFrame(frame);
   
	if (frame->MscMsg == 0x68) 
	{
		if (frame->Opcode == 0x20) 
		{
			if (frame->Size == 1) 
			{   	
				switch (frame->Data[0]) 
				{
					case 0x10:
					    rMhlframe.size = 4;
					    rMhlframe.sendidx = 0;
					    rMhlframe.reTXcnt = 0;
					    rMhlframe.txtag = NULL;
					    rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
					    rMhlframe.blocks.header.initiator = u1CbusLa;
					    rMhlframe.blocks.opcode = OPCODE_MHL_CONTENT_ON;
					    rMhlframe.blocks.operand[0] = u1MhlGetPortId(1) ? (u1MhlGetPortId(1)<<4) : 0x20;
					    rMhlframe.blocks.operand[1] = 0x0;

					    prMhlframe = &rMhlframe;                
						break; 

					case 0x11:
					    rMhlframe.size = 4;
					    rMhlframe.sendidx = 0;
					    rMhlframe.reTXcnt = 0;
					    rMhlframe.txtag = NULL;
					    rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
					    rMhlframe.blocks.header.initiator = u1CbusLa;
					    rMhlframe.blocks.opcode = OPCODE_MHL_CONTENT_OFF;
					    rMhlframe.blocks.operand[0] = u1MhlGetPortId(1) ? (u1MhlGetPortId(1)<<4) : 0x20;
					    rMhlframe.blocks.operand[1] = 0x0;

					    prMhlframe = &rMhlframe;                
					    break; 

					default:
						break;
				};
			}
		}
	}
	else if(frame->MscMsg == 0x6c)
	{
		if(frame->Opcode == 0x00)
		{
			rMhlframe.size = 4 + frame->Size;
		    rMhlframe.sendidx = 0;
		    rMhlframe.reTXcnt = 0;
		    rMhlframe.txtag = NULL;
		    rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
		    rMhlframe.blocks.header.initiator = u1CbusLa;
		    rMhlframe.blocks.opcode = OPCODE_MHL_SCRATCHPAD_DATA;
		    rMhlframe.blocks.operand[0] = _u4DevieAdapterID >>8;
		    rMhlframe.blocks.operand[1] = _u4DevieAdapterID & 0xff;
			for(i=0; i< frame->Size; i++)
			{
				rMhlframe.blocks.operand[2+i] = frame->Data[i];
			}

		    prMhlframe = &rMhlframe;             
   	}
   }
  
  
   if (prMhlframe != NULL)
   {

       //_Cbus_NotifyRxResult(prMhlframe);
       if(_Cbus_VerifyFrame(prMhlframe))
       {
       		_Cbus_EnqueueRx(prMhlframe);
			CBUS_PrintFrame(prMhlframe, CEC_RX_FRAME);
       }
   }
   
}
#endif

#ifdef SYS_MHL_SUPPORT
void vCbusDrvNfyTxResult(UINT8 result, void *txtag)
{
    LOG(0, "CBUS Tx result\n");
    LOG(0, "result: 0x%2x\n", result);
    LOG(0, "txtag: 0x%2x\n", txtag);
}
#endif

#ifdef SYS_MHL_SUPPORT
extern void vCbus_SetRxCallback(x_cbus_drv_nfy_data_arrival nfyfp);
extern void vCbus_SetTxCallback(x_cbus_drv_nfy_tx_result nfyfp);
#endif

#ifdef SYS_MHL_SUPPORT
UINT8 u1KeyPressed = 0;
static UINT8 _Cbus_SendFrame(CEC_FRAME_DESCRIPTION *frame)
{
    if (frame->blocks.header.initiator == CEC_LOG_TV && frame->blocks.header.destination == u1CbusLa)
    {
        if (frame->size == 3)
        {
            // CEC RCP to MHL RCP mapping
            if (frame->blocks.opcode == OPCODE_USER_CONTROL_PRESSED) {
                u1KeyPressed = 1;
                bSendRcpKeys(frame->blocks.operand[0], 0);
            } else if (frame->blocks.opcode == OPCODE_USER_CONTROL_RELEASED) {
                // sometimes no press event, but release event
                if (u1KeyPressed == 0) {
                    bSendRcpKeys(frame->blocks.operand[0], 0);
                }
                u1KeyPressed = 0;
            }
			
            printf("send rcp key 0x%x,%d\n",frame->blocks.operand[0],frame->blocks.opcode);
        }
		else if(frame->size == 2)
		{
			if (frame->blocks.opcode == OPCODE_USER_CONTROL_RELEASED) 
			{
                bSendRcpKeys((frame->blocks.operand[0] | 0x80), 0);
				printf("send rcp release key 0x%x,%d\n",frame->blocks.operand[0],frame->blocks.opcode);
			}
		}
    }

    return(0);
}
#endif

#ifdef SYS_MHL_SUPPORT
static UINT8 _Cbus_VerifyFrame(CEC_FRAME_DESCRIPTION *frame)
{

    if (frame == NULL) return 0;

    if (((frame->blocks.header.initiator == CEC_LOG_TV) && \
    	((frame->blocks.header.destination == u1CbusLa) || (frame->blocks.header.destination == CEC_LOG_BROADCAST)))\
    	||((frame->blocks.header.initiator == u1CbusLa )&& (frame->blocks.header.destination == CEC_LOG_BROADCAST)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif

#ifdef SYS_MHL_SUPPORT
UINT8 u1DeviceName[16] = {0};
static UINT8 _Cbus_VirtualTxDeviceHandler(CEC_FRAME_DESCRIPTION *frame)
{
    UINT8 ret = 0;
    UINT32 u4Msg = OPCODE_NONE;
    UINT32 u4Msg1 = OPCODE_NONE;
    UINT8 u1CbusPA[2] = {0x20, 0x00};
    
    u1CbusPA[0] = u1MhlGetPortId(1) ? (u1MhlGetPortId(1)<<4) : 0x20;
    u1CbusPA[1] = 0x0;
    
    if (frame == NULL) return 0;

    if (frame->blocks.header.initiator == CEC_LOG_TV && frame->blocks.header.destination == u1CbusLa)
    {
        if (frame->size == 1)
        {
            // it is a ping message
            ret = 1;
        }
        else
        {
             switch(frame->blocks.opcode)
             {
                 case OPCODE_GIVE_PHYSICAL_ADDRESS:
                          u4Msg = OPCODE_REPORT_PHYSICAL_ADDRESS;
       	       break;
       	       
                 case OPCODE_GIVE_OSD_NAME:
                 	     x_memset(u1DeviceName, 0, sizeof(u1DeviceName));
                       u4Msg = OPCODE_SET_OSD_NAME; // if no device name reported
       	       break;

                 case OPCODE_GIVE_DEVICE_POWER_STATUS:
                          u4Msg = OPCODE_REPORT_POWER_STATUS;
       	       break;

                 case OPCODE_GIVE_DEVICE_VENDOR_ID:
                          u4Msg = OPCODE_DEVICE_VENDOR_ID;
       	       		  break; 
				 case OPCODE_MHL_SCRATCHPAD_DATA:
						ret = 1;
						Mhl_SendWriteBurstRequest();
       	       break;    
       	       
             	   default:             	   	
             	   	break;
             };

        }

    }
    else if (frame->blocks.header.initiator == CEC_LOG_TV && frame->blocks.header.destination == CEC_LOG_BROADCAST)
    {
         switch(frame->blocks.opcode)
         {

             case OPCODE_ROUTING_CHANGE:
     	            //u4Msg = OPCODE_ACTIVE_SOURCE;
      	       break;

             case OPCODE_SET_STREAM_PATH:
             	     if (frame->blocks.operand[0] == u1CbusPA[0] && frame->blocks.operand[1] == u1CbusPA[1])
             	     {
   	                u4Msg = OPCODE_TEXT_VIEW_ON;
   	                u4Msg1 = OPCODE_ACTIVE_SOURCE;
             	     }
   	       break;      
		   	case OPCODE_STANDBY:
				ret=2;
				break;

        	   default:
             	   	
         	   	break;
          };
    }


    if (u4Msg != OPCODE_NONE) {
        ret = 1;
        u4CbusMsg = u4Msg;

        if (u4Msg1 != OPCODE_NONE) {
            u4CbusMsg1 = u4Msg1;
        }
    }
    
    
    return (ret);
}
#endif

#ifdef SYS_MHL_SUPPORT
//extern UINT8 buf1[16];
static UINT8 _Cbus_VirtualRxDeviceHandler(void)
{
    CEC_FRAME_DESCRIPTION rMhlframe;
    CEC_FRAME_DESCRIPTION *prMhlframe = NULL;

    x_memset(&rMhlframe, 0, sizeof(CEC_FRAME_DESCRIPTION));

    if (u4CbusMsg == OPCODE_NONE && u4CbusMsg1 != OPCODE_NONE)
    {
        u4CbusMsg = u4CbusMsg1;
        u4CbusMsg1 = OPCODE_NONE;
    }

   
   switch (u4CbusMsg)
   {
       case OPCODE_REPORT_PHYSICAL_ADDRESS:
				{
					rMhlframe.size = 6;
	                rMhlframe.sendidx = 0;
	                rMhlframe.reTXcnt = 0;
	                rMhlframe.txtag = NULL;
	                rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
	                rMhlframe.blocks.header.initiator = u1CbusLa;
	                rMhlframe.blocks.opcode = OPCODE_REPORT_PHYSICAL_ADDRESS;
	                rMhlframe.blocks.operand[0] = u1MhlGetPortId(1) ? (u1MhlGetPortId(1)<<4) : 0x30;
	                rMhlframe.blocks.operand[1] = 0x0;
	                rMhlframe.blocks.operand[2] = u1CbusLa;
	                rMhlframe.blocks.operand[3] = 1;
				}
	   	       
                prMhlframe = &rMhlframe;
       	break;

       case OPCODE_SET_OSD_NAME:
                rMhlframe.size = 8;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = CEC_LOG_TV;
                rMhlframe.blocks.header.initiator = u1CbusLa;
                rMhlframe.blocks.opcode = OPCODE_SET_OSD_NAME;
				rMhlframe.blocks.operand[0] = 'M';//'X';
                rMhlframe.blocks.operand[1] = 'H';
                rMhlframe.blocks.operand[2] = 'L';
                prMhlframe = &rMhlframe;
       	break;

       case OPCODE_REPORT_POWER_STATUS:
                rMhlframe.size = 3;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = CEC_LOG_TV;
                rMhlframe.blocks.header.initiator = u1CbusLa;
                rMhlframe.blocks.opcode = OPCODE_REPORT_POWER_STATUS;
                rMhlframe.blocks.operand[0] = 0x0;

                prMhlframe = &rMhlframe;                
       	break;    
       	
       case OPCODE_DEVICE_VENDOR_ID:
                rMhlframe.size = 5;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
                rMhlframe.blocks.header.initiator = u1CbusLa;
                rMhlframe.blocks.opcode = OPCODE_DEVICE_VENDOR_ID;
                rMhlframe.blocks.operand[0] = 0x8;
                rMhlframe.blocks.operand[1] = 0x0;
                rMhlframe.blocks.operand[2] = 0x46;

                prMhlframe = &rMhlframe;                
       	break;

       case OPCODE_ACTIVE_SOURCE:
                rMhlframe.size = 4;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = CEC_LOG_BROADCAST;
                rMhlframe.blocks.header.initiator = u1CbusLa;
                rMhlframe.blocks.opcode = OPCODE_ACTIVE_SOURCE;
                rMhlframe.blocks.operand[0] = u1MhlGetPortId(1) ? (u1MhlGetPortId(1)<<4) : 0x20;
                rMhlframe.blocks.operand[1] = 0x0;

                prMhlframe = &rMhlframe;                
       	break; 
       	
       case OPCODE_TEXT_VIEW_ON:
                rMhlframe.size = 2;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = CEC_LOG_TV;
                rMhlframe.blocks.header.initiator = u1CbusLa;
                rMhlframe.blocks.opcode = OPCODE_TEXT_VIEW_ON;

                prMhlframe = &rMhlframe;                
       	break;

       case OPCODE_FEATURE_ABORT:
                rMhlframe.size = 3;
                rMhlframe.sendidx = 0;
                rMhlframe.reTXcnt = 0;
                rMhlframe.txtag = NULL;
                rMhlframe.blocks.header.destination = u1CbusLa;
                rMhlframe.blocks.header.initiator = CEC_LOG_TV;
                rMhlframe.blocks.opcode = OPCODE_FEATURE_ABORT;
                rMhlframe.blocks.operand[0] = 0x8;
                
                prMhlframe = &rMhlframe;                
       	break;
       	
       default:
       	break;
   };
  
   if (prMhlframe != NULL)
   {
       CBUS_PrintFrame(prMhlframe, CEC_RX_FRAME);

       _Cbus_NotifyRxResult(prMhlframe);
   }
    
    u4CbusMsg = OPCODE_NONE;

   return(0);
}
#endif

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
#ifdef SYS_MHL_SUPPORT
void Cbus_Init(void)
{
  vMhlInit();
  vCbus_SetRxCallback(vCbusDrvNfyDataArrival);
  vCbus_SetTxCallback(vCbusDrvNfyTxResult);
  UNUSED(prCbusActiveRxFrame);
  UNUSED(prCbusActiveTxFrame);
}
#endif

#ifdef SYS_MHL_SUPPORT
UINT32 u4CbusStatus = 0;
extern UINT8 uIsHaveAckFlag;
extern UINT8 u1SourceStatus;
void _Cbus_HandleTxTask(void)
{
    CEC_FRAME_DESCRIPTION *frame;
	UINT8 i=0;
    UNUSED(frame);
    UNUSED(prCbusActiveTxFrame);
    
    /* if the tx message queue is empty */
    if (u1IsTX_Q_Empty())
    {
        return;
    }

    if (IsCBUSStatus(STATE_TX_SEND_FRAME))
    {  
        ClrCBUSStatus(STATE_TX_SEND_FRAME);
        //LOG(5,"clear Tx send frame \n");
    }
    else
    {
        // check if it is mhl frame
        frame = _Cbus_GetTxReadFrame();
        if (frame == NULL) return;   // avoid null poniter
        if (_Cbus_VerifyFrame(frame) == 1)
        {
            prCbusActiveTxFrame = frame;
                       
            if (_Cbus_VirtualTxDeviceHandler(frame) == 0)
            {
                // send mhl frame, RCP keys
                
                 SetCBUSStatus(STATE_TX_SEND_FRAME);
			
#ifdef SYS_MHL_SUPPORT
                 _Cbus_SendFrame(frame);
#endif
                 ClrCBUSStatus(STATE_TX_SEND_FRAME);
            }else if(_Cbus_VirtualTxDeviceHandler(frame) == 2)
            {
               if(vIsSonyMHLDevice())
               {
                	for(i=0;i<4;i++)
					{
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
                        Mhl_RapKeys(0x11);
#else
			#if defined(CC_MHL_HDMI_SWITCH)
					   fgSil_RapKeys(0x11);//content off
			#endif
#endif					   
					   printf("[mhl]enter stabdy:send content off \n");
					   uIsHaveAckFlag=0;
					   x_thread_delay(500);  //wait 1s
					   if(uIsHaveAckFlag >= 1)//receive rapk
					   {
						   uIsHaveAckFlag--;
						   printf("[mhl]enter stabdy:send content off success and receive ack from phone\n");
						   break;
					   }
					   else
					   {
					   	   printf("[mhl]enter stabdy:send content off and no receive ack from phone\n");
					   }
					}
               }
            }

#ifndef SYS_ANDROID_TV_TIF_SUPPORT
            // TODO: enqueue to MHL small queue ?
#if defined (CC_MT5399) || defined (CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
            if(vCbusGetState() == MHL_STATE_CONNECTED)
            {
             _Cbus_NotifyTxResult(CEC_SUCCESS,prCbusActiveTxFrame, prCbusActiveTxFrame->txtag);
            }
            else
#endif
            {
				_Cbus_NotifyTxResult(CEC_ERROR,prCbusActiveTxFrame, prCbusActiveTxFrame->txtag);
            }
#endif
            // de queue
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
			if ((frame->blocks.header.destination == u1CbusLa)
				&& (_Cbus_VirtualTxDeviceHandler(frame) != 1))// only rcp, then Dequeue for sony R7.
#else
			if (frame->blocks.header.destination == u1CbusLa)
#endif
            {
                _Cbus_DequeueTx();
                //LOG(5,"CBUS _Cec_DequeueTx \n");
            }
        }
    }
}
#endif

#ifdef SYS_MHL_SUPPORT
void _Cbus_HandleRxTask(void)
{
    CEC_FRAME_DESCRIPTION *frame;

    //UNUSED(frame);

    _Cbus_VirtualRxDeviceHandler();
	frame = _Cbus_GetRxReadFrame();
	if(frame == NULL)
	{
		return;
	}
  	if(_Cbus_VerifyFrame(frame))
  	{
		_Cbus_NotifyRxResult(frame);
		CBUS_PrintFrame(frame,CEC_RX_FRAME);
	}
	if((frame->blocks.header.initiator == u1CbusLa))
	{
		_Cbus_DequeueRx();
	}
  
}
#endif
