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
 * $RCSfile: nandhw_reg.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file ldm.c
 *  ldm.c provides local dimming control functions\n
 *  device.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_typedef.h"
#include "sv_const.h"
#include "nptv_debug.h"
#include "panel.h"
#include "hw_ldmspi.h"
#include "drv_ldmspi.h"
#if defined(CC_MT5398)
#include "drv_lcdim_v2.h"
#else
#include "drv_lcdim.h"
#endif
#include "drv_display.h"
#include "x_printf.h"
#include "hw_sw.h"
#include "drv_pwm.h"
#include "pe_if.h"

#define CLIP(val, min, max) ((val>=max) ? max : ((val<=min) ? min : val)) 
#define DIM_PROTECT 75

UINT16 u2FaultState[16] =          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
UINT16 u2BadBlock = 0x0;            // use 16bit to record bad block status
UINT32 _u4FaultBlkIdx = 0;

UINT8 u1AMSDelayOrder[16] = {0, 0, 1, 2, 2, 3, 4, 4, 4, 4, 3, 2, 2, 1, 0, 0};
 
static UINT32 u4SPILogCnt = 0;
static UINT8 u1DimProtect[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

extern UINT8 u1SpiCmdArray[MAX_SPI_CMD_SIZE];
extern UINT8 u1ReadFlg;
extern UINT8 u1DriverOrder[200];
extern UINT8 u1BlockDimGain[256];
extern UINT16 u2DimmingLowBound;

extern PANEL_LOCAL_DIMMING_ATTRIBUTE_T *pParam;

//====================================================================================
//                        Function Prototype For Command Set
//====================================================================================
sLdmSpiCommandSet sAmsSpiFunc = 
{
	vDrvAmsBatchReadCmd,            //  void (*vSpiBatchReadCmd)(void);
	vDrvAmsBatchWriteCmd,           //  void (*vSpiBatchWriteCmd)(void);
    vDrvAmsLightUpCmd,              //  void (*vSpiLightUpCmd)(void);
	vDrvAmsLogOffCmd,               //	void (*vSpiLogOffCmd)(void);
	vDrvAmsRepairCmd,	            //	void (*vSpiRepairCmd)(void);	
	vDrvAmsRead1BCmd,	            //	void (*vSpiRead1BCmd)(UINT8);
	vDrvAmsWrite1BCmd,               //	void (*vSpiWrite1BCmd)(UINT8, UINT8);	
	vDrvAmsSetScanningCmd,           //    void (*vSpiSetScanningCmd)(UINT8);		
    SPI_NON_DEF_CMD_2_PARAM,         //  void (*vSpiMultiDevRead1BCmd)(UINT8, UINT8);
    SPI_NON_DEF_CMD_3_PARAM          // void (*vSpiMultiDevWrite1BCmd)(UINT8, UINT8, UINT8);
};

//====================================================================================
//                     AMS Spec Implementation (General Behavior)
//====================================================================================

void vDrvAmsRead1BCmd(UINT8 u1Addr)
{
    UINT8 u1ReadBack;

    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);  
    vIO32WriteFldAlign(LED_NOB2, 2, RBYTE_LEN);     
    vIO32WriteFldAlign(LED_NOB2, 1, RBACK_LEN);         

    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)(((u1Addr) << 1) | LDM_AS3693_READ_IDX);

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 2);    

    if(vDrvLcdimSpiWaitReady(10000000)==SV_FAIL)
    {
        Printf("[SPI] Reading Address %d Fail\n", u1Addr);

        vIO32WriteFldAlign(LED_CON, 1, OP_MODE);          

        return;
    }
    
    u1ReadBack = IO32ReadFldAlign(LED_RBUF, READ_BUF0);    
    
    Printf("Current Reading Address %d, Readback = 0x%x\n", u1Addr, u1ReadBack);

    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
}



void vDrvAmsWrite1BCmd(UINT8 u1Addr, UINT8 u1Data)
{
    // 0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((u1Addr << 1) | LDM_AS3693_WRITE_IDX);
    u1SpiCmdArray[2] = u1Data;

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   
}

BOOL _bDrvAmsReadCmd(void)
{
    UINT8 u1ReadBack;
    
    if(u1ReadFlg == SV_TRUE)
    {
        if(vDrvLcdimSpiWaitReady(10000000)==SV_FAIL)
        {
            LOG(4, "[SPI] Read Procedure Fail, Wait For Next Vsync\n");

            return SV_FAIL;
        }
        
        u1ReadBack = IO32ReadFldAlign(LED_RBUF, READ_BUF0);    

        // AMS spec for fault handling.
        vDrvAmsSetFaultState(_u4FaultBlkIdx, u1ReadBack);

        ++_u4FaultBlkIdx;
        
        _u4FaultBlkIdx %= 4;
    
        vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
        
        u1ReadFlg = SV_FALSE;       
    }   

    return SV_SUCCESS;            
}

void vDrvAmsBatchReadCmd(void)
{
    // if hw is sending data, avoid r/w command to SRAM
    if(u1DrvLcdimSpiRWProtect() == SV_FALSE)
    {
        return;
    }    

    if(_bDrvAmsReadCmd() == SV_FAIL)
    {
        return;
    }
    
    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);  
    vIO32WriteFldAlign(LED_NOB2, 2, RBYTE_LEN);     
    vIO32WriteFldAlign(LED_NOB2, 2, RBACK_LEN);         

    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)(((LDM_AS3693_FAULT_DETECT + _u4FaultBlkIdx) << 1) | LDM_AS3693_READ_IDX);

    vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, 2);        

    // set flag, read in next vsync
    u1ReadFlg = SV_TRUE;
}

void vDrvAmsBatchWriteCmd(void)
{
#ifndef CC_MTK_LOADER      
    UINT16 u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
    UINT16 i;    
    UINT8 u1DimGain = vDrvLcdimSpiGetGlobalDimming();
    UINT16 u2TempBuf;    
    static UINT8 u1Buffer[LCDIM_MAX_BLK_NUM];

    // previous step is read function
    // ( note : spi read command will take effect in next vsync, hence need to be handled in write command section )
    if(_bDrvAmsReadCmd() == SV_FAIL)
    {
        return;
    }
    
    vDrvGetLcDimDimmingResult(u1Buffer); //move local dimming data to buffer

    // 0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_HTIME1 << 1) | LDM_AS3693_WRITE_IDX);

    // 1. set data body (16 bit per block, but each block use 9 bit to represent data actually)
    for(i=0; i < u2DrvLcdimSpiGetBlockNum(); i++)
    {
        // Cobra local dimming V2 supports only up to 200 blocks, fix klock warning
        if(i >= 200)
        {
            i = 199;
        }                
        if(!pParam->u1ScanningOnoff)
        {
            if(u1DriverOrder[i] >= 200)    //fix klocwork warning
            {
                u1DriverOrder[i] = 199;
            }             
            u1SpiCmdArray[2+u1DriverOrder[i]*2] =   (((u2IO32Read2B(ADAP_LCDIM_SPI_SEP) >> u1DriverOrder[i]) & 0x1) == 1) ? 
                                                    0 : (UINT8)(CLIP(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i])>>15), 0, 0xFF));

            if(u1SpiCmdArray[2+u1DriverOrder[i]*2] < u2DimmingLowBound)
            {
                u1SpiCmdArray[2+u1DriverOrder[i]*2] = u2DimmingLowBound;
            }

            u1SpiCmdArray[2+u1DriverOrder[i]*2+1] = 0;
            u1DimProtect[u1DriverOrder[i]] = (u1SpiCmdArray[2+u1DriverOrder[i]*2] < DIM_PROTECT) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            if(u1DriverOrder[i] >= 200)    //fix klocwork warning
            {
                u1DriverOrder[i] = 199;
            }             

            // when scanning supported, means pwm invert function is enabled, hence need to re-invert data using software
            u2TempBuf = 0x1ff - CLIP((UINT16)(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i])>>15) << 1), 0, 0x1FF); // 20130411 Ben, by MengHu's mail. Fix HTime out of range issue. 
#ifdef CC_SCAN_BACKLIGHT

            if(u2TempBuf > (0x1FF - (u2DimmingLowBound<<1)))
            {
                u2TempBuf = (0x1FF - (u2DimmingLowBound<<1));
            }
             
            if((u2TempBuf > 0x1FC) && (u2DimmingLowBound == 0))
            {
                u2TempBuf = 0x1FC; // If input is 0 && lower bound = 0, directly set it to 0     // 20130411 Ben, by MengHu's mail. Change minimum of HTime.
            }
#endif
            u1SpiCmdArray[2+u1DriverOrder[i]*2] = (((u2IO32Read2B(ADAP_LCDIM_SPI_SEP) >> u1DriverOrder[i]) & 0x1) == 1) ? 0xFF : (UINT8)(u2TempBuf & 0xff);      
            u1SpiCmdArray[2+u1DriverOrder[i]*2+1] = (((u2IO32Read2B(ADAP_LCDIM_SPI_SEP) >> u1DriverOrder[i]) & 0x1) == 1) ? 1 : (UINT8)((u2TempBuf>>8) & 0xff);
            u1DimProtect[u1DriverOrder[i]] = (u2TempBuf > (0x208 - DIM_PROTECT)) ? SV_TRUE : SV_FALSE;
        }
    }        
    vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, u2DataLen);        

    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_DIM_LOG) && (u4SPILogCnt++%90==0))
    {
        for (i=0; i < u2DataLen; i++)
        {
            Printf("0x%x ", u1SpiCmdArray[i]);
        }
        Printf("\n");
    }
#endif      
}

void vDrvAmsLightUpCmd(void)
{
    //-------------------------------------
    // Customization Part For Different Panel Design
    //-------------------------------------
    UINT16 u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
    UINT16 i;
    UINT8 u1InitLevel = 0xFF;
    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
    
    //-------------------------------------
    // 1. turn on all current regulator
    //-------------------------------------    
    // 1.0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_CUR_REGULATOR << 1) | LDM_AS3693_WRITE_IDX);

    // 1.1. set data body
    if(u2DrvLcdimSpiGetBlockNum() == 16)
    {
        u1SpiCmdArray[2] = 0xFF;
        u1SpiCmdArray[3] = 0xFF;
    }
    else if(u2DrvLcdimSpiGetBlockNum() == 10)
    {
        u1SpiCmdArray[2] = 0xFF;
        u1SpiCmdArray[3] = 0x03;
    }
    else if(u2DrvLcdimSpiGetBlockNum() == 9)
    {
        u1SpiCmdArray[2] = 0xFF;
        u1SpiCmdArray[3] = 0x01;
    }
    else if(u2DrvLcdimSpiGetBlockNum() == 8)
    {
        u1SpiCmdArray[2] = 0xFF;
        u1SpiCmdArray[3] = 0x00;
    }
#ifdef CC_SCAN_BACKLIGHT	
	else if(u2DrvLcdimSpiGetBlockNum() == 7)
	{
		u1SpiCmdArray[2] = 0x7F;
		u1SpiCmdArray[3] = 0x00;
	}
#endif	
    else if(u2DrvLcdimSpiGetBlockNum() == 6)
    {
        u1SpiCmdArray[2] = 0x3F;
        u1SpiCmdArray[3] = 0x00;
    }
#ifdef CC_SCAN_BACKLIGHT	
    else if(u2DrvLcdimSpiGetBlockNum() == 5)	
    {
        u1SpiCmdArray[2] = 0x1F;
        u1SpiCmdArray[3] = 0x00;
    }
#endif	
    else
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0x00;
        LOG(0, "Current Block Fail\n");
    }
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 4);   
    
    //-------------------------------------
    // 3. turn off feedbacks
    //-------------------------------------    

    // 3.1. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_FEEDBACK1 << 1) | LDM_AS3693_WRITE_IDX);

    // 3.2. set data body 
    if(u2DrvLcdimSpiGetBlockNum() == 10)
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0x00;
        u1SpiCmdArray[4] = 0xF0;
        u1SpiCmdArray[5] = 0xFF;        
    }
    else if(u2DrvLcdimSpiGetBlockNum() == 9)
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0x00;
        u1SpiCmdArray[4] = 0xFC;
        u1SpiCmdArray[5] = 0xFF;        
    }
    else if(u2DrvLcdimSpiGetBlockNum() == 8)
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0x00;
        u1SpiCmdArray[4] = 0xFF;
        u1SpiCmdArray[5] = 0xFF;        
    }
#ifdef CC_SCAN_BACKLIGHT	
	else if(u2DrvLcdimSpiGetBlockNum() == 7)
	{
		u1SpiCmdArray[2] = 0x00;
		u1SpiCmdArray[3] = 0xC0;
		u1SpiCmdArray[4] = 0xFF;
		u1SpiCmdArray[5] = 0xFF;		
	}
#endif	
    else if(u2DrvLcdimSpiGetBlockNum() == 6)
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0xF0;
        u1SpiCmdArray[4] = 0xFF;
        u1SpiCmdArray[5] = 0xFF;        
    }
#ifdef CC_SCAN_BACKLIGHT	
    else if(u2DrvLcdimSpiGetBlockNum() == 5)	
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0xFC;
        u1SpiCmdArray[4] = 0xFF;
        u1SpiCmdArray[5] = 0xFF;        
    }
#endif	
    else
    {
        u1SpiCmdArray[2] = 0x00;
        u1SpiCmdArray[3] = 0x00;
        u1SpiCmdArray[4] = 0x00;
        u1SpiCmdArray[5] = 0x00;   
    }
        
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 6);           
    //-------------------------------------
    // 4. turn on open / short detection
    //-------------------------------------    
    // 4.1. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_FBK_CONFIG1 << 1) | LDM_AS3693_WRITE_IDX);
    u1SpiCmdArray[2] = 0x0D;                      // enable open / short detection   
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   
    
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_FBK_CONFIG2 << 1) | LDM_AS3693_WRITE_IDX);
    u1SpiCmdArray[2] = 0x01;                      // enable open / short detection   
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   

    vDrvAmsSetScanning(pParam->u1ScanningTblIdx);

    //-------------------------------------
    // 2. set all dimming to full
    //-------------------------------------    

    // 2.0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_HTIME1 << 1) | LDM_AS3693_WRITE_IDX);

    // 2.1. set data body (16 bit per block)
    for(i=0; i < u2DrvLcdimSpiGetBlockNum(); i++)
    {
        if(pParam->u1ScanningOnoff)        
        {
            u1SpiCmdArray[2+i*2] = (UINT8)(((UINT16)(0xFF - u1InitLevel) << 1) & 0xFF);
            u1SpiCmdArray[2+i*2+1] = (UINT8)(((UINT16)(0xFF - u1InitLevel) << 1) >> 8);
        }
        else
        {
            u1SpiCmdArray[2+i*2] = u1InitLevel;
            u1SpiCmdArray[2+i*2+1] = 0;
        }
    }        

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

    vDrvSetPWM(SrcPWM2, (BSP_GetDomainClock(SRC_BUS_CLK)/255) / 63000, 0x80, 0xFF); 

    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);
}

void vDrvAmsLogOffCmd(void)
{
    UINT16 u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
    UINT16 i;

    if(u2DataLen > MAX_SPI_CMD_SIZE)
    {
        u2DataLen = MAX_SPI_CMD_SIZE;    // fix klockwork
    }

    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    

    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
    
    // 1. set all block dimming to 0
    // 1.0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_HTIME1 << 1) | LDM_AS3693_WRITE_IDX);

    // 1.1. set data body (16 bit per block)
    for(i=1; i < u2DrvLcdimSpiGetBlockNum(); i++)
    {
        u1SpiCmdArray[2+i*2] = 0;
        u1SpiCmdArray[2+i*2+1] = 0;
    }        

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen); 

    // 2. turn off current regulators    
    // 2.0. set header (device id and register address)
    u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
    u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_CUR_REGULATOR << 1) | LDM_AS3693_WRITE_IDX);  

    // 2.1. set data body
    u1SpiCmdArray[2] = 0x00;
    u1SpiCmdArray[3] = 0x00;
    
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 4); 

    LOG(0, "SPI Init [LogOff]\n");
}

//====================================================================================
//                 AMS Spec Implementation (Customized Behavior)
//====================================================================================

void vDrvAmsSetFaultState(UINT8 u1BlkIdx, UINT8 u1FaultStatus)
{
    UINT8 u1Blk = 0;

    if(u1BlkIdx > 3)
    {
        u1BlkIdx = 3;     // fix klockwork
    }    
    
#ifndef CC_MTK_LOADER      
    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_MANUAL_DELAY))
    {
        u1BlkIdx = IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_BLK);
        u1FaultStatus = IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_FAULT);
        Printf("u1FaultStatus %d = %d", u1BlkIdx, u1FaultStatus);
    }
#endif
    
    for(u1Blk = 0; u1Blk < 4; u1Blk++)
    {
        if (u1DimProtect[u1BlkIdx*4+u1Blk] == SV_FALSE)
        {
            if(((u1FaultStatus>>(u1Blk*2)) & 0x3) == E_SPI_SHORT_STATUS)
            {
                if(u2FaultState[u1BlkIdx*4+u1Blk] == E_SPI_NORMAL_STATUS)
                {
                    u2FaultState[u1BlkIdx*4+u1Blk] = E_SPI_UNCERTAIN_SHORT_STATUS;
                }
                else
                {
                    u2FaultState[u1BlkIdx*4+u1Blk] += 1;
                }
            }
            else
            {
                u2FaultState[u1BlkIdx*4+u1Blk] = ((u1FaultStatus>>(u1Blk*2)) & 0x3);
            }

            if((u2FaultState[u1BlkIdx*4+u1Blk] >= E_SPI_SHORT_TOLERANCE) || (u2FaultState[u1BlkIdx*4+u1Blk] == E_SPI_OPEN_STATUS))
            {
                u2BadBlock |= (1 << (u1BlkIdx*4+u1Blk));
            }
        }
    }
}

UINT8 u1DrvAmsGetFaultState(void)
{
    UINT8 u1Blk = 0;

    // return open status first
    for(u1Blk = 0; u1Blk < u2DrvLcdimSpiGetBlockNum(); u1Blk++)
    {
        if(u1Blk > 15)
        {
            u1Blk = 15;     // fix klockwork
        }
        
        if(u2FaultState[u1Blk] == E_SPI_OPEN_STATUS)
        {
            return E_SPI_OPEN_STATUS;
        }
    }

    // if any block short over than 60 frame, mark it as real short
    for(u1Blk = 0; u1Blk < u2DrvLcdimSpiGetBlockNum(); u1Blk++)
    {      
        if(u1Blk > 15)
        {
            u1Blk = 15;     // fix klockwork
        }
        
        if(u2FaultState[u1Blk] >= E_SPI_SHORT_TOLERANCE)
        {
            LOG(0, "[%d] SPI u2FaultState = %d\n", u1Blk, u2FaultState[u1Blk]);               
            return E_SPI_SHORT_STATUS;
        }
    }    

    // mark uncertain short block
    for(u1Blk = 0; u1Blk < u2DrvLcdimSpiGetBlockNum(); u1Blk++)
    {
        if(u1Blk > 15)
        {
            u1Blk = 15;     // fix klockwork
        }
        
        if((u2FaultState[u1Blk] >= E_SPI_UNCERTAIN_SHORT_STATUS) && (u2FaultState[u1Blk] < E_SPI_SHORT_TOLERANCE))
        {
            return E_SPI_UNCERTAIN_SHORT_STATUS;
        }
    }    

    return E_SPI_NORMAL_STATUS;
}

void vDrvAmsRepairCmd(void)
{
    UINT8 i;
    UINT16 u2RepairMask = 0x0;
    UINT8 u1FaultState = u1DrvAmsGetFaultState();

    for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
    {
        u2RepairMask |= (1 << i);
    }

    u2BadBlock &= u2RepairMask;

    if((u1FaultState == E_SPI_OPEN_STATUS)||(u1FaultState == E_SPI_SHORT_STATUS))
    {
        u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
        u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_CUR_REGULATOR << 1) | LDM_AS3693_WRITE_IDX);  

        u1SpiCmdArray[2] = (UINT8)(u2BadBlock & 0xFF);
        u1SpiCmdArray[3] = (UINT8)((u2BadBlock >> 8) & 0xFF);

        LOG(0, "Repair Open/Short 0x%x 0x%x\n", u1SpiCmdArray[2], u1SpiCmdArray[3]);
    
        vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, 4); 
        vDrvLcdimSpiWaitReady(1000000);            
    }

    if(u1FaultState == E_SPI_UNCERTAIN_SHORT_STATUS)
    {
        // uncertain state, bypass
    }
}

UINT16 u2DrvGetTotalBadBlocks(void)
{
    UINT16 i, u2BadCount = 0;
    for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
    {
        if(((u2BadBlock >> i) & 0x1) == 1)
        {
            u2BadCount++;
        }
    }
    return u2BadCount;
}

// For Delay Order Customization
void vDrvAmsSetDelayOrder(UINT8 *u1CustDelayOrder)
{
    UINT8 i;
    
    for(i = 0; i < 16; i++)
    {
        u1AMSDelayOrder[i] = u1CustDelayOrder[i];
    }    
}

// Customization Part For Customer Spec (5 Dimming Area)
void vDrvAmsSetScanning(UINT8 u1ScanningIdx)
{

#if 0
    UINT16 u2AMSDelayTime[3][5] = {
                                    {128, 248, 312, 370, 468},      // for scanning model
                                    {222, 279, 414, 485,  60},      // for non-scanning model
                                    {468, 370, 312, 248, 128}                 // for upside-down scanning model                                
                                  };

    UINT8 u1AMSDelaySetting16Blk[16] = {0, 0, 1, 2, 2, 3, 4, 4, 4, 4, 3, 2, 2, 1, 0, 0};
    UINT8 u1AMSDelaySetting10Blk[10] = {0, 1, 2, 3, 4, 4, 3, 2, 1, 0};
    UINT8 u1AMSDelaySetting9Blk[9] = {0, 0, 1, 2, 2, 3, 3, 4, 4};
    UINT8 u1AMSDelaySetting8Blk[8] = {0, 0, 1, 2, 2, 3, 4, 4};
#ifdef CC_SCAN_BACKLIGHT	
	UINT8 u1AMSDelaySetting7Blk[7] = {0, 0, 1, 2, 2, 3, 4};
#endif
    UINT8 u1AMSDelaySetting6Blk[6] = {0, 1, 2, 3, 4, 4};
#ifdef CC_SCAN_BACKLIGHT	
    UINT8 u1AMSDelaySetting5Blk[5] = {0, 1, 2, 3, 4};	
#endif
    UINT16 u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
    UINT16 i;

		
#ifndef CC_MTK_LOADER      
	if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_MANUAL_DELAY))
	{
		u1ScanningIdx = 0;
		u2AMSDelayTime[0][0] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_00);  
		u2AMSDelayTime[0][1] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_01);  
		u2AMSDelayTime[0][2] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_02);  
		u2AMSDelayTime[0][3] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_03);  
		u2AMSDelayTime[0][4] = IO32ReadFldAlign(SPI_SCANNING_DELAY_2, SPI_SCANNING_DELAY_04);
   }
#endif
#else
#ifdef CC_SCAN_BACKLIGHT
    UINT32 u4PanelIndex;
#endif
    UINT16 u2AMSDelayTime[3][3] = {
									  {  6, 127, 277},		// for scanning model
									  {222, 279, 414},		// for non-scanning model
									  {277, 127,   6}		// for upside-down scanning model								 
                                  };
  
#ifdef CC_SCAN_BACKLIGHT 
    UINT16 u2AMSDelayTimeE500iB1_TC_128[3][3] = {
                                                    {304, 176,  48},	   // for scanning model
                                                    {222, 279, 414},	   // for non-scanning model
                                                    { 48, 176, 304}	       // for upside-down scanning model								
                                                };
                                            
    UINT16 u2AMSDelayTimeE500iB1_TB_96[3][3] = {
                                                    {320, 176,  48},	   // for scanning model
                                                    {222, 279, 414},	   // for non-scanning model
                                                    { 48, 176, 320}	       // for upside-down scanning model								
                                               };
#endif

    UINT8 u1AMSDelaySetting16Blk[16] = {0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0};
    UINT8 u1AMSDelaySetting10Blk[10] = {0, 1, 1, 2, 2, 2, 2, 1, 1, 0};
    UINT8 u1AMSDelaySetting9Blk[9] = {0, 0, 0, 1, 1, 1, 2, 2, 2};
    UINT8 u1AMSDelaySetting8Blk[8] = {0, 0, 1, 1, 1, 2, 2, 2};
#ifdef CC_SCAN_BACKLIGHT	
	UINT8 u1AMSDelaySetting7Blk[7] = {0, 0, 1, 1, 1, 2, 2};	
#endif
    UINT8 u1AMSDelaySetting6Blk[6] = {0, 0, 1, 1, 2, 2};
#ifdef CC_SCAN_BACKLIGHT	
    UINT8 u1AMSDelaySetting5Blk[5] = {0, 1, 1, 2, 2};	
#endif
    UINT16 u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
    UINT16 i;

#ifdef CC_SCAN_BACKLIGHT
	u4PanelIndex = GetCurrentPanelIndex(); 
    switch (u4PanelIndex)
    {
        case PANEL_TPV_50_TPT500J1_HVN04_S17TC:
        { 
            x_memcpy(u2AMSDelayTime,	u2AMSDelayTimeE500iB1_TC_128, sizeof( u2AMSDelayTimeE500iB1_TC_128));
            break;
        }
        case PANEL_TPV_50_TPT500J1_HVN04_S17TB:
		case PANEL_TPV_50_TPT500J1_LE8_SC2A:	// Ben 20140313
        { 
            x_memcpy(u2AMSDelayTime,	u2AMSDelayTimeE500iB1_TB_96, sizeof( u2AMSDelayTimeE500iB1_TB_96));
            break;
        }
            
        default:								
         	break;						
    }
#endif

#ifndef	CC_DISABLE_UART_UPGRADE

#ifndef CC_MTK_LOADER      /* for use NCS tool adjust delay time*/
	if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_MANUAL_DELAY))
	{
		u1ScanningIdx = 0;
		u2AMSDelayTime[0][0] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_00);  
		u2AMSDelayTime[0][1] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_01);  
		u2AMSDelayTime[0][2] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_02);  
		#if 0
		u2AMSDelayTime[0][3] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_03);  	/* should be ignore for 3 zone */
		u2AMSDelayTime[0][4] = IO32ReadFldAlign(SPI_SCANNING_DELAY_2, SPI_SCANNING_DELAY_04);	/* should be ignore for 3 zone */
		#endif  
		vIO32WriteFldAlign(SPI_SCANNING_DELAY_0,u2AMSDelayTime[0][0],SPI_SCANNING_DELAY_00);  
	    vIO32WriteFldAlign(SPI_SCANNING_DELAY_0,u2AMSDelayTime[0][1], SPI_SCANNING_DELAY_01);  
		vIO32WriteFldAlign(SPI_SCANNING_DELAY_1,u2AMSDelayTime[0][2],SPI_SCANNING_DELAY_02);
	 }
#endif

#endif

#endif



    //-------------------------------------
    // 0. Set Scanning Backlight
    //-------------------------------------        
    // hsync (pwm) clk set to 30kHz
    // 256 (pwm resolution) x 120 (frame rate) = 30800
    // vDrvSetPWM(SrcPWM2, (BSP_GetDomainClock(SRC_BUS_CLK)/255) / 30720, 0x80, 0xFF); 
    // set dimming resolution
    // 20110525 todo : move scanning backlight setting to customization part
    if(pParam->u1ScanningOnoff)
    {
        //vDrvSetPWM(HSYNC_SRC, (BSP_GetDomainClock(SRC_BUS_CLK)/255) / 63000, 0x80, 0xFF);  //Larry 2013 1214 follow MengHu's suggestion
        
        // set dimming resolution from 8 bit to 9 bit, resolution is 520(0x208) instead of 511(0x1FF)
        u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
        u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_PERIOD_LSB << 1) | LDM_AS3693_WRITE_IDX);
        u1SpiCmdArray[2] = 0xF6;	//for SPI duty jitter
        u1SpiCmdArray[3] = 0x1;

        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 4);      

        // 0.1. set scanning parameters
        u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
        u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_MODE << 1) | LDM_AS3693_WRITE_IDX);

        u1SpiCmdArray[2] = 0x10;    // 0x10;  // For Scan Backlight Function, AMS PWM should be set to 0x10
                            	    // BIT[1:0]: PWM Mode: 00 Sync mode, 01 Async mode
                                	// BIT[2]: Clock Source: 0 Ext clock, 1 Internal 500kHz
                            	    // BIT[3]: VSync Invert: 0 active high, 1 active low
                                	// BIT[4]: PWMINVERT: 0: normal, 1: inverted        

        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);

        // 0.2 set scanning delay
        u2DataLen = 2 + u2DrvLcdimSpiGetBlockNum()*2;
        
        u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
        u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_DELAY1 << 1) | LDM_AS3693_WRITE_IDX);

        if(u2DrvLcdimSpiGetBlockNum() == 16)
        {
            for(i=0; i < 16; i++)
            {
                if(u1AMSDelaySetting16Blk[i] > 5)
                {
                    u1AMSDelaySetting16Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting16Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting16Blk[i]]>>8)&0xFF);                                                           // delay time MSB of channel[i]     
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }
        else if(u2DrvLcdimSpiGetBlockNum() == 10)
        {
            for(i=0; i < 10; i++)
            {
                if(u1AMSDelaySetting10Blk[i] > 5)
                {
                    u1AMSDelaySetting10Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting10Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting10Blk[i]]>>8)&0xFF);   
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }	
        else if(u2DrvLcdimSpiGetBlockNum() == 9)
        {
            for(i=0; i < 9; i++)
            {
                if(u1AMSDelaySetting9Blk[i] > 5)	
                {
                    u1AMSDelaySetting9Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting9Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting9Blk[i]]>>8)&0xFF);   
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }	
        else if(u2DrvLcdimSpiGetBlockNum() == 8)
        {
            for(i=0; i < 8; i++)
            {
                if(u1AMSDelaySetting8Blk[i] > 5)	
                {
                    u1AMSDelaySetting8Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting8Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting8Blk[i]]>>8)&0xFF);   
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }	
#ifdef CC_SCAN_BACKLIGHT	
		else if(u2DrvLcdimSpiGetBlockNum() == 7)
		{
			for(i=0; i < 7; i++)
			{
				if(u1AMSDelaySetting7Blk[i] > 5)	
				{
					u1AMSDelaySetting7Blk[i] = 5;
				}

				if(u1ScanningIdx > 2)
				{
					u1ScanningIdx = 2;
				}
				
				u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting7Blk[i]]&0xFF);  // delay time LSB of channel[i]
				u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting7Blk[i]]>>8)&0xFF);   
			}
			vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);	
		}	
#endif		
        else if(u2DrvLcdimSpiGetBlockNum() == 6)
        {
            for(i=0; i < 6; i++)
            {
                if(u1AMSDelaySetting6Blk[i] > 5)	
                {
                    u1AMSDelaySetting6Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting6Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting6Blk[i]]>>8)&0xFF);   
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }	
#ifdef CC_SCAN_BACKLIGHT	
        else if(u2DrvLcdimSpiGetBlockNum() == 5)	
        {
            for(i=0; i < 5; i++)
            {
                if(u1AMSDelaySetting5Blk[i] > 5)	
                {
                    u1AMSDelaySetting5Blk[i] = 5;
                }

                if(u1ScanningIdx > 2)
                {
                    u1ScanningIdx = 2;
                }
                
                u1SpiCmdArray[2 + i*2] =  (UINT8)(u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting5Blk[i]]&0xFF);  // delay time LSB of channel[i]
                u1SpiCmdArray[2 + i*2 + 1] = (UINT8)((u2AMSDelayTime[u1ScanningIdx][u1AMSDelaySetting5Blk[i]]>>8)&0xFF);   
            }
            vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
        }	
#endif		
        else
        {
            LOG(0, "Block Number Error\n");
        }    
    }    
    else
    {
        u1SpiCmdArray[0] = LDM_AS3693_ADDR1;
        u1SpiCmdArray[1] = (UINT8)((LDM_AS3693_PWM_PERIOD_LSB << 1) | LDM_AS3693_WRITE_IDX);
        u1SpiCmdArray[2] = 0xFF;

        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   
        LOG(0, "Not Support Scanning\n");
    }     
}

void vDrvAmsSetScanningCmd(UINT8 u1ScanningIdx)
{
    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  

    vDrvAmsSetScanning(u1ScanningIdx);
    
    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);        
}

void vDrvLcdimSetLowBound(UINT16 u2Bound) 
{
    u2DimmingLowBound = u2Bound;
}

