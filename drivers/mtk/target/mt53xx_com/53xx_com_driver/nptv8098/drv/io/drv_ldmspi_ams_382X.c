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
 * $RCSfile: drv_ldmspi_ams_382X.c,v $
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

extern UINT16 u2FaultState[16];
extern UINT16 u2BadBlock;
extern UINT32 _u4FaultBlkIdx;

extern UINT8 u1AMSDelayOrder[16];

extern UINT8 u1SpiCmdArray[MAX_SPI_CMD_SIZE];
extern UINT8 u1ReadFlg;
extern UINT8 u1DriverOrder[200];
extern UINT8 u1BlockDimGain[256]; 
extern UINT8 u1PanelDelay[200];

extern PANEL_LOCAL_DIMMING_ATTRIBUTE_T *pParam;

//====================================================================================
//                        Function Prototype For Command Set
//====================================================================================
sLdmSpiCommandSet sAms382XSpiFunc = 
{
	vDrvAms382XBatchReadCmd,            //  void (*vSpiBatchReadCmd)(void);
	vDrvAms382XBatchWriteCmd,           //  void (*vSpiBatchWriteCmd)(void);
    vDrvAms382XLightUpCmd,              //  void (*vSpiLightUpCmd)(void);
	vDrvAms382XLogOffCmd,               //	void (*vSpiLogOffCmd)(void);
	vDrvAms382XRepairCmd,	            //	void (*vSpiRepairCmd)(void);	
	vDrvAms382XRead1BCmd,	            //	void (*vSpiRead1BCmd)(UINT8);
	vDrvAms382XWrite1BCmd,               //	void (*vSpiWrite1BCmd)(UINT8, UINT8);	
	vDrvAms382XSetScanningCmd,           //    void (*vSpiSetScanningCmd)(UINT8);		
    vDrvAms382XMultiDevRead1BCmd,         //  void (*vSpiMultiDevRead1BCmd)(UINT8, UINT8);
    vDrvAms382XMultiDevWrite1BCmd         // void (*vSpiMultiDevWrite1BCmd)(UINT8, UINT8, UINT8);
};

//====================================================================================
//                     AMS Spec Implementation (General Behavior)
//====================================================================================

void vDrvAms382XRead1BCmd(UINT8 u1Addr)
{
    UINT32 u4ReadBack;

    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);  
    vIO32WriteFldAlign(LED_NOB2, 2, RBYTE_LEN);     
    vIO32WriteFldAlign(LED_NOB2, 2, RBACK_LEN);

    u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_AS382X_READ_IDX << 7));

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 2);    
    LOG(1, "[SPI] Send command: 0x%x 0x%x\n", u1SpiCmdArray[0], u1SpiCmdArray[1]);

    if(vDrvLcdimSpiWaitReady(10000000)==SV_FAIL)
    {
        Printf("[SPI] Reading Address %d Fail\n", u1Addr);
        vIO32WriteFldAlign(LED_CON, 1, OP_MODE);          
        return;
    }
    
    u4ReadBack = IO32ReadFldAlign(LED_RBUF, READ_BUF0) & 0xFF;
    Printf("[SPI] Current Reading Address 0x%x, Readback = 0x%x\n", u1Addr, u4ReadBack);

    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
}

void vDrvAms382XMultiDevRead1BCmd(UINT8 u1DevID, UINT8 u1Addr)
{
    UINT32 u4ReadBack, u2DataLen;
    u2DataLen = 2 + (pParam->u1DevNum - 1);

    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);  
    vIO32WriteFldAlign(LED_NOB2, u2DataLen, RBYTE_LEN);     
    vIO32WriteFldAlign(LED_NOB2, 2, RBACK_LEN);

    u1SpiCmdArray[0] = (UINT8)(u1DevID | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_AS382X_READ_IDX << 7));

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);    
    LOG(1, "[SPI] Send command: 0x%x 0x%x\n", u1SpiCmdArray[0], u1SpiCmdArray[1]);

    if(vDrvLcdimSpiWaitReady(10000000)==SV_FAIL)
    {
        Printf("[SPI] Reading Address %d Fail\n", u1Addr);
        vIO32WriteFldAlign(LED_CON, 1, OP_MODE);          
        return;
    }
    
    u4ReadBack = IO32ReadFldAlign(LED_RBUF, READ_BUF0) & 0xFF;
    Printf("[SPI] Current Reading Address 0x%x, Readback = 0x%x\n", u1Addr, u4ReadBack);

    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
}

void vDrvAms382XWrite1BCmd(UINT8 u1Addr, UINT8 u1Data)
{
    // 0. set header (device id and register address)
    u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_AS382X_WRITE_IDX << 7));
    u1SpiCmdArray[2] = u1Data;

    LOG(1, "[SPI] Send command: 0x%x 0x%x 0x%x\n", u1SpiCmdArray[0], u1SpiCmdArray[1], u1SpiCmdArray[2]);

#ifndef CC_MTK_LOADER
    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_HW_MODE))
    {
        vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, 3);   
    }
    else
#endif
    {
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   
    }
}

void vDrvAms382XMultiDevWrite1BCmd(UINT8 u1DevID, UINT8 u1Addr, UINT8 u1Data)
{
    UINT32 u4Index, u2DataLen;

    // 0. Clear command array
    for (u4Index = 0; u4Index < 10; u4Index++)
    {
        u1SpiCmdArray[u4Index] = 0;
    }

    // 1. Check  device ID
    if (u1DevID > pParam->u1DevNum)
    {
        Printf("[SPI] Error! Device ID is larger than device number.");
        return;
    }

    // 2. Set device ID
    if (u1DevID == LDM_AS382X_BCAST_SAME)
    {
        // Write same value to same addr for all device
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_BCAST_SAME | (LDM_AS382X_MULTI_DEV<<7) | (LDM_AS382X_SINGLE_BYTE<<6));
        u2DataLen = 3 + (pParam->u1DevNum - 1);
    }
    else
    {
        // Write value to one device
        u1SpiCmdArray[0] = (UINT8)(u1DevID | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_SINGLE_BYTE<<6));
        u2DataLen = 3 + (u1DevID - 1);
    }

    // 3. Set addr and data
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_AS382X_WRITE_IDX << 7));
    u1SpiCmdArray[2] = u1Data;

    // 4. Send SPI command
#ifndef CC_MTK_LOADER
    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_HW_MODE))
    {
        vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, u2DataLen);   
        //x_thread_delay(20);
    }
    else
#endif
    {
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
    }

    // 5. Command log
    LOG(1, "[SPI] Send command: ");
    for (u4Index = 0; u4Index < u2DataLen; u4Index++)
    {
        LOG(1, "0x%x ", u1SpiCmdArray[u4Index]);
    }
    LOG(1, "\n");
}

BOOL _bDrvAms382XReadCmd(void)
{
    return SV_SUCCESS;            
}

void vDrvAms382XBatchReadCmd(void)
{
 
}

void vDrvAms382XBatchWriteCmd(void)
{
#ifndef CC_MTK_LOADER
    UINT16 u2DataLen;
    UINT16 i;    
    UINT8 u1DimGain = vDrvLcdimSpiGetGlobalDimming();
    UINT16 u2TempBuf;    
    static UINT8 u1Buffer[LCDIM_MAX_BLK_NUM];
    static UINT32 u4DimCnt = 0;
    UINT32 u4DummyChNum = IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_DUMMY_CH_NUM);
    UINT32 u4SepCtrl;

    vDrvGetLcDimDimmingResult(u1Buffer); //move local dimming data to buffer
    
    if (pParam->u1DevNum == 1)
    {
        u2DataLen = 3 + (u2DrvLcdimSpiGetBlockNum() + u4DummyChNum)*2;

        // 0. set header (device id and register address)
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * (u2DrvLcdimSpiGetBlockNum() + u4DummyChNum);
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX << 7));
    
        // 1. set data body (16 bit per block, but each block use 9 bit to represent data actually)
        for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
        {
            i = MIN(i, LCDIM_MAX_BLK_NUM-1);
            u2TempBuf = (((u2IO32Read2B(ADAP_LCDIM_SPI_SEP) >> u1DriverOrder[i]) & 0x1) == 1) ? 
                                0 : (UINT8)(CLIP(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i]+32767)>>15), 0, 0xFF));

            u2TempBuf = (u2TempBuf == 255) ? 1023 : CLIP(u2TempBuf*4, 0, 1023);     
            if(pParam->u1ScanningOnoff)
            {
                u2TempBuf = 1023 - u2TempBuf;
            }
            u1SpiCmdArray[3+u1DriverOrder[i]*2] = u2TempBuf & 0xFF;
            u1SpiCmdArray[3+u1DriverOrder[i]*2+1] = (u2TempBuf >> 8) & 0xFF;
        }
    }
    else
    {
        u2DataLen = 3 + (u2DrvLcdimSpiGetBlockNum() + u4DummyChNum) * 2 + (pParam->u1DevNum - 1);

        // 0. set header (device id and register address)
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_BCAST_DIFF | (LDM_AS382X_MULTI_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX << 7));
    
        // 1. set data body (16 bit per block, but each block use 9 bit to represent data actually)
        for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
        {
            i = MIN(i, LCDIM_MAX_BLK_NUM-1);
            u4SepCtrl = (i < 32) ? u4IO32Read4B(ADAP_LCDIM_SPI_SEP) : u4IO32Read4B(ADAP_LCDIM_SPI_SEP_1);
            u2TempBuf = (((u4SepCtrl >> u1DriverOrder[i]) & 0x1) == 1) ? 
                                0 : (UINT8)(CLIP(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i]+32767)>>15), 0, 0xFF));

            u2TempBuf = (u2TempBuf == 255) ? 1023 : CLIP(u2TempBuf*4, 0, 1023);     
            if(pParam->u1ScanningOnoff)
            {
                u2TempBuf = 1023 - u2TempBuf;
            }// scanning backlight invert // okis @20140211
            u1SpiCmdArray[3+u1DriverOrder[i]*2] = u2TempBuf & 0xFF;
            u1SpiCmdArray[3+u1DriverOrder[i]*2+1] = (u2TempBuf >> 8) & 0xFF;
        }
    }

    u2DataLen = MIN(u2DataLen, MAX_SPI_CMD_SIZE);
    vDrvLcdimSpiSendDataHWMode(u1SpiCmdArray, u2DataLen);   

    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_DIM_LOG) && ((u4DimCnt++) % 120 == 0))
    {
        for(i = 0; i < u2DataLen; i++)
        {
            Printf("0x%2x ", u1SpiCmdArray[i]);
        }
        Printf("\n");
    }
#endif      
}

void vDrvAms382XLightUpCmd(void)
{
    UINT16 u2DataLen, u2RegulatorTurnOff, i, u2DevID;
    UINT16 u2BlockNum = MIN(u2DrvLcdimSpiGetBlockNum(), LCDIM_MAX_BLK_NUM-1);

    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  

    if (pParam->u1DevNum == 1)
    {
        // Single device light-up process
        // 1. Unlock AS382X
        vDrvAms382XWrite1BCmd(LDM_AS382X_UNLOCK, LDM_AS382X_UNLOCK_CMD);

        // 2. Set PWM Period
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 0x2;     // 2 byte command: LSB + MSB
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_PERIOD_LSB | (LDM_AS382X_WRITE_IDX << 7));
        u1SpiCmdArray[3] = 0xFF;    // PWM period = 1023, hsync freq=120x1023=122820Hz
        u1SpiCmdArray[4] = 0x3;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 5);   

        // 3. set all dimming to full
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX << 7));

        for(i=0; i < u2BlockNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x3;
            }
        }        
        u2DataLen = 3 + u2BlockNum * 2;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

        // 4. turn on all current regulator
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2;   // 2 byte command: Channe 1~8 + Channe 9~16
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_CUR_REGULATOR | (LDM_AS382X_WRITE_IDX << 7));

        u2RegulatorTurnOff = 0;
        for(i = 0; i < u2BlockNum; i++)
        {
            u2RegulatorTurnOff |= (1 << i);
        }

        u1SpiCmdArray[3] = u2RegulatorTurnOff & 0xFF;
        u1SpiCmdArray[4] = (u2RegulatorTurnOff >> 8) & 0xFF;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 5);

        // 5. Set PWM control mode
        if(pParam->u1ScanningOnoff)
        {
            vDrvAms382XWrite1BCmd(LDM_AS382X_PWM_MODE, 0x23);
        }
        else
        {
            vDrvAms382XWrite1BCmd(LDM_AS382X_PWM_MODE, 0x3);
        }

        // 6. Set double vsync frequency
#ifndef CC_MT5398
        vDrvLcdimSpiSetDoubleVsyncFreq(SV_TRUE);
#endif

        // 7. Set Feedback function
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2;   // 2 byte command: Channe 1~8 + Channe 9~16
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_FB_CUR | (LDM_AS382X_WRITE_IDX << 7));

        u2RegulatorTurnOff = 0;
        for(i = 0; i < u2BlockNum; i++)
        {
            u2RegulatorTurnOff |= (1 << i);
        }

        u1SpiCmdArray[3] = u2RegulatorTurnOff & 0xFF;
        u1SpiCmdArray[4] = (u2RegulatorTurnOff >> 8) & 0xFF;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 5);

        // 8. Set open/short protection function
        vDrvAms382XWrite1BCmd(LDM_AS382X_PROTECT, 0x87);    // bit 0: 1=auto off at open, bit 1: 1=enable open detection

        // 9. Lock AS382X
        vDrvAms382XWrite1BCmd(LDM_AS382X_UNLOCK, LDM_AS382X_LOCK_CMD);
    }
    else
    {
        // Multi device light-up process
        // 1. Unlock AS382X
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_UNLOCK, LDM_AS382X_UNLOCK_CMD);

        // 2. Set PWM Period
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_PWM_PERIOD_LSB, 0xFF); // PWM period = 0x3FF = 1023, hsync freq=120x1023=122820Hz
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_PWM_PERIOD_MSB, 0x03);

        // 3. Set all dimming to full
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_BCAST_SAME | (LDM_AS382X_MULTI_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX<<7));

        for(i=0; i < pParam->u1DevChNum; i++)
        {
            u1SpiCmdArray[3+i*2] = 0xFF;
            u1SpiCmdArray[3+i*2+1] = 0x3;
        }        
        u2DataLen = 3 + pParam->u1DevChNum * 2 + (pParam->u1DevNum - 1);
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

        // 4. Turn on all current regulator and set feedback function
        for (u2DevID = 1; u2DevID <= pParam->u1DevNum; u2DevID++)
        {
            u2RegulatorTurnOff = 0;
            if (u2BlockNum >= (pParam->u1DevChNum * u2DevID))
            {
                u2RegulatorTurnOff = 0xFFFF;
            }
            else
            {
                for(i = 0; i < (u2BlockNum - pParam->u1DevChNum * (u2DevID-1)); i++)
                {
                    u2RegulatorTurnOff |= (1 << i);
                }
            }
            vDrvAms382XMultiDevWrite1BCmd(u2DevID, LDM_AS382X_CUR_REGULATOR, u2RegulatorTurnOff & 0xFF);
            vDrvAms382XMultiDevWrite1BCmd(u2DevID, LDM_AS382X_CUR_REGULATOR+1, (u2RegulatorTurnOff >> 8) & 0xFF);
            vDrvAms382XMultiDevWrite1BCmd(u2DevID, LDM_AS382X_FB_CUR, u2RegulatorTurnOff & 0xFF);
            vDrvAms382XMultiDevWrite1BCmd(u2DevID, LDM_AS382X_FB_CUR+1, (u2RegulatorTurnOff >> 8) & 0xFF);
        }

        // 5. Set PWM control mode
        if(pParam->u1ScanningOnoff)
        {
            vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_PWM_MODE, 0x23);
        }
        else
        {
            vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_PWM_MODE, 0x3);
        }

        // 6. Set double vsync frequency
#ifndef CC_MT5398
        vDrvLcdimSpiSetDoubleVsyncFreq(SV_TRUE);
#endif
 
        // 7. Set open/short protection function
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_PROTECT, 0x87);    // bit 0: 1=auto off at open, bit 1: 1=enable open detection

        // 8. Lock AS382X
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_UNLOCK, LDM_AS382X_LOCK_CMD);
    }

    vDrvSetPWM(HSYNC_SRC, (BSP_GetDomainClock(SRC_BUS_CLK)/255) / 122880, 0x80, 0xFF); 
    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);
    vIO32WriteFldAlign(LED_CON, SV_ON, HW_TRIG_EN);  
}

void vDrvAms382XLogOffCmd(void)
{
    UINT16 u2BlockNum = u2DrvLcdimSpiGetBlockNum();
    UINT16 u2DataLen = 3 + u2BlockNum*2;
    UINT16 i;

    if(u2DataLen > MAX_SPI_CMD_SIZE)
    {
        u2DataLen = MAX_SPI_CMD_SIZE;    // fix klockwork
    }

    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  

    if (pParam->u1DevNum == 1)
    {
        // 0. Unlock AM382X
        vDrvAms382XWrite1BCmd(LDM_AS382X_UNLOCK, LDM_AS382X_UNLOCK_CMD);
        
        // 1. set all block dimming to 0
        // 1.0. set header (device id and register address)
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX << 7));

        // 1.1. set data body (16 bit per block)
        for(i=1; i < u2BlockNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x3;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
        }        

        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen); 

        // 2. turn off current regulators    
        // 2.0. set header (device id and register address)
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2;   // 2 byte command: Channe 1~8 + Channe 9~16
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_CUR_REGULATOR | (LDM_AS382X_WRITE_IDX << 7));

        // 2.1. set data body
        u1SpiCmdArray[3] = 0x00;
        u1SpiCmdArray[4] = 0x00;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 5);

        // 3. Lock AM382X
        vDrvAms382XWrite1BCmd(LDM_AS382X_UNLOCK, LDM_AS382X_LOCK_CMD);
    }
    else
    {
        // 1. Unlock AS382X
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_UNLOCK, LDM_AS382X_UNLOCK_CMD);

        // 2. Set all dimming to zero
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_BCAST_SAME | (LDM_AS382X_MULTI_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_PWM_HTIME1 | (LDM_AS382X_WRITE_IDX<<7));

        for(i=0; i < pParam->u1DevChNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x3;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
        }        
        u2DataLen = 3 + pParam->u1DevChNum * 2 + (pParam->u1DevNum - 1);
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

        // 3. Turn off all current regulator
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_CUR_REGULATOR, 0);
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_CUR_REGULATOR+1, 0);

        // 4. Lock AS382X
        vDrvAms382XMultiDevWrite1BCmd(LDM_AS382X_BCAST_SAME, LDM_AS382X_UNLOCK, LDM_AS382X_LOCK_CMD);
    }
    
    LOG(0, "SPI LogOff\n");
}

//====================================================================================
//                 AMS Spec Implementation (Customized Behavior)
//====================================================================================

void vDrvAms382XSetFaultState(UINT8 u1BlkIdx, UINT8 u1FaultStatus)
{

}

UINT8 u1DrvAms382XGetFaultState(void)
{
    return E_SPI_NORMAL_STATUS;
}

void vDrvAms382XRepairCmd(void)
{

}

void vDrvAms382XSetScanning(UINT8 u1ScanningIdx)
{
    UINT16 u2Temp, i, u2BlockNum, u2DataLen;

    u2BlockNum = MIN(u2DrvLcdimSpiGetBlockNum(), LCDIM_MAX_BLK_NUM-1);

#ifndef CC_MTK_LOADER
    // For maunal tuning delay time
    if (IO32ReadFldAlign(ADAP_LCDIM_SPI, SPI_MANUAL_DELAY))
    {
        u1PanelDelay[0] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_00);  
        u1PanelDelay[1] = IO32ReadFldAlign(SPI_SCANNING_DELAY_0, SPI_SCANNING_DELAY_01);  
        u1PanelDelay[2] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_02);  
        u1PanelDelay[3] = IO32ReadFldAlign(SPI_SCANNING_DELAY_1, SPI_SCANNING_DELAY_03);
        u1PanelDelay[4] = IO32ReadFldAlign(SPI_SCANNING_DELAY_2, SPI_SCANNING_DELAY_04);
        u1PanelDelay[5] = IO32ReadFldAlign(SPI_SCANNING_DELAY_2, SPI_SCANNING_DELAY_05);            
        u1PanelDelay[6] = IO32ReadFldAlign(SPI_SCANNING_DELAY_3, SPI_SCANNING_DELAY_06);
        u1PanelDelay[7] = IO32ReadFldAlign(SPI_SCANNING_DELAY_3, SPI_SCANNING_DELAY_07);            
        u1PanelDelay[8] = IO32ReadFldAlign(SPI_SCANNING_DELAY_4, SPI_SCANNING_DELAY_08);  
        u1PanelDelay[9] = IO32ReadFldAlign(SPI_SCANNING_DELAY_4, SPI_SCANNING_DELAY_09);  
        u1PanelDelay[10] = IO32ReadFldAlign(SPI_SCANNING_DELAY_5, SPI_SCANNING_DELAY_10);  
        u1PanelDelay[11] = IO32ReadFldAlign(SPI_SCANNING_DELAY_5, SPI_SCANNING_DELAY_11);
        u1PanelDelay[12] = IO32ReadFldAlign(SPI_SCANNING_DELAY_6, SPI_SCANNING_DELAY_12);
        u1PanelDelay[13] = IO32ReadFldAlign(SPI_SCANNING_DELAY_6, SPI_SCANNING_DELAY_13);            
        u1PanelDelay[14] = IO32ReadFldAlign(SPI_SCANNING_DELAY_7, SPI_SCANNING_DELAY_14);
        u1PanelDelay[15] = IO32ReadFldAlign(SPI_SCANNING_DELAY_7, SPI_SCANNING_DELAY_15);            

        u1PanelDelay[16] = IO32ReadFldAlign(SPI_SCANNING_DELAY_8, SPI_SCANNING_DELAY_16);  
        u1PanelDelay[17] = IO32ReadFldAlign(SPI_SCANNING_DELAY_8, SPI_SCANNING_DELAY_17);  
        u1PanelDelay[18] = IO32ReadFldAlign(SPI_SCANNING_DELAY_9, SPI_SCANNING_DELAY_18);  
        u1PanelDelay[19] = IO32ReadFldAlign(SPI_SCANNING_DELAY_9, SPI_SCANNING_DELAY_19);
        u1PanelDelay[20] = IO32ReadFldAlign(SPI_SCANNING_DELAY_A, SPI_SCANNING_DELAY_20);
        u1PanelDelay[21] = IO32ReadFldAlign(SPI_SCANNING_DELAY_A, SPI_SCANNING_DELAY_21);            
        u1PanelDelay[22] = IO32ReadFldAlign(SPI_SCANNING_DELAY_B, SPI_SCANNING_DELAY_22);
        u1PanelDelay[23] = IO32ReadFldAlign(SPI_SCANNING_DELAY_B, SPI_SCANNING_DELAY_23);            
        u1PanelDelay[24] = IO32ReadFldAlign(SPI_SCANNING_DELAY_C, SPI_SCANNING_DELAY_24);  
        u1PanelDelay[25] = IO32ReadFldAlign(SPI_SCANNING_DELAY_C, SPI_SCANNING_DELAY_25);  
        u1PanelDelay[26] = IO32ReadFldAlign(SPI_SCANNING_DELAY_D, SPI_SCANNING_DELAY_26);  
        u1PanelDelay[27] = IO32ReadFldAlign(SPI_SCANNING_DELAY_D, SPI_SCANNING_DELAY_27);
        u1PanelDelay[28] = IO32ReadFldAlign(SPI_SCANNING_DELAY_E, SPI_SCANNING_DELAY_28);
        u1PanelDelay[29] = IO32ReadFldAlign(SPI_SCANNING_DELAY_E, SPI_SCANNING_DELAY_29);
        u1PanelDelay[30] = IO32ReadFldAlign(SPI_SCANNING_DELAY_F, SPI_SCANNING_DELAY_30);
        u1PanelDelay[31] = IO32ReadFldAlign(SPI_SCANNING_DELAY_F, SPI_SCANNING_DELAY_31);
    }
#endif
    if (pParam->u1DevNum == 1)
    {
        u2DataLen = 3 + u2BlockNum * 2;
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_ADDR1 | (LDM_AS382X_SINGLE_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_DELAY1 | (LDM_AS382X_WRITE_IDX << 7));
    }
    else
    {
        u2DataLen = 3 + u2BlockNum * 2 + (pParam->u1DevNum - 1);
        u1SpiCmdArray[0] = (UINT8)(LDM_AS382X_BCAST_DIFF | (LDM_AS382X_MULTI_DEV<<7) | (LDM_AS382X_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_AS382X_DELAY1 | (LDM_AS382X_WRITE_IDX << 7));
    }

    for(i=0; i < u2BlockNum; i++)
    {
        u2Temp = MIN(u1PanelDelay[i] * 4, 0x3FF);
        u1SpiCmdArray[3+i*2] = u2Temp & 0xFF;
        u1SpiCmdArray[3+i*2+1] = u2Temp >> 8;
    }        

    u2DataLen = MIN(u2DataLen, MAX_SPI_CMD_SIZE);
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
}

void vDrvAms382XSetScanningCmd(UINT8 u1ScanningIdx)
{
    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
    vDrvAms382XSetScanning(u1ScanningIdx);
    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);
}

