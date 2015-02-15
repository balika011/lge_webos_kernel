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
 * $RCSfile: drv_ldmspi_lepower_1887.c,v $
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

extern UINT8 u1SpiCmdArray[MAX_SPI_CMD_SIZE];
extern UINT8 u1DriverOrder[200];
extern UINT8 u1BlockDimGain[256]; 
extern UINT8 u1PanelDelay[200];
extern PANEL_LOCAL_DIMMING_ATTRIBUTE_T *pParam;

//====================================================================================
//                        Function Prototype For Command Set
//====================================================================================
sLdmSpiCommandSet sLePower1887SpiFunc = 
{
    SPI_NON_DEF_CMD_0_PARAM,            //  void (*vSpiBatchReadCmd)(void);
    vDrvLePower1887BatchWriteCmd,           //  void (*vSpiBatchWriteCmd)(void);
    vDrvLePower1887LightUpCmd,              //  void (*vSpiLightUpCmd)(void);
    vDrvLePower1887LogOffCmd,               //	void (*vSpiLogOffCmd)(void);
    SPI_NON_DEF_CMD_0_PARAM,	        //	void (*vSpiRepairCmd)(void);	
    vDrvLePower1887Read1BCmd,	        //	void (*vSpiRead1BCmd)(UINT8);
    vDrvLePower1887Write1BCmd,              //	void (*vSpiWrite1BCmd)(UINT8, UINT8);	
    vDrvLePower1887SetScanningCmd,          //    void (*vSpiSetScanningCmd)(UINT8);	
    vDrvLePower1887MultiDevRead1BCmd,            //  void (*vSpiMultiDevRead1BCmd)(UINT8, UINT8);
    vDrvLePower1887MultiDevWrite1BCmd          // void (*vSpiMultiDevWrite1BCmd)(UINT8, UINT8, UINT8);
};

//====================================================================================
//                     LePower1887 Spec Implementation (General Behavior)
//====================================================================================

void vDrvLePower1887Read1BCmd(UINT8 u1Addr)
{
    UINT32 u4ReadBack;

    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);
    vIO32WriteFldAlign(LED_NOB2, 2, RBYTE_LEN);
    vIO32WriteFldAlign(LED_NOB2, 2, RBACK_LEN);

    u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_LP1887_READ_IDX << 7));

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

void vDrvLePower1887MultiDevRead1BCmd(UINT8 u1DevID, UINT8 u1Addr)
{
    UINT32 u4ReadBack, u2DataLen;
    u2DataLen = 2 + (pParam->u1DevNum - 1);

    vIO32WriteFldAlign(LED_CON, 2, OP_MODE);  
    vIO32WriteFldAlign(LED_NOB2, u2DataLen, RBYTE_LEN);     
    vIO32WriteFldAlign(LED_NOB2, 2, RBACK_LEN);

    u1SpiCmdArray[0] = (UINT8)(u1DevID | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_LP1887_READ_IDX << 7));

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

void vDrvLePower1887Write1BCmd(UINT8 u1Addr, UINT8 u1Data)
{
    u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_SINGLE_BYTE<<6));
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_LP1887_WRITE_IDX << 7));
    u1SpiCmdArray[2] = u1Data;

    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 3);   

    LOG(1, "[SPI] Send command: 0x%x 0x%x 0x%x\n", u1SpiCmdArray[0], u1SpiCmdArray[1], u1SpiCmdArray[2]);
}

void vDrvLePower1887MultiDevWrite1BCmd(UINT8 u1DevID, UINT8 u1Addr, UINT8 u1Data)
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
    if (u1DevID == LDM_LP1887_BCAST_SAME)
    {
        // Write same value to same addr for all device
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_BCAST_SAME | (LDM_LP1887_MULTI_DEV<<7) | (LDM_LP1887_SINGLE_BYTE<<6));
        u2DataLen = 3 + (pParam->u1DevNum - 1);
    }
    else
    {
        // Write value to one device
        u1SpiCmdArray[0] = (UINT8)(u1DevID | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_SINGLE_BYTE<<6));
        u2DataLen = 3 + (u1DevID - 1);
    }

    // 3. Set addr and data
    u1SpiCmdArray[1] = (UINT8)(u1Addr | (LDM_LP1887_WRITE_IDX << 7));
    u1SpiCmdArray[2] = u1Data;

    // 4. Send SPI command
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

    // 5. Command log
    Printf("[SPI] Send command: ");
    for (u4Index = 0; u4Index < u2DataLen; u4Index++)
    {
        Printf("0x%x ", u1SpiCmdArray[u4Index]);
    }
    Printf("\n");
}

void vDrvLePower1887BatchWriteCmd(void)
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
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * (u2DrvLcdimSpiGetBlockNum() + u4DummyChNum);
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX << 7));
    
        // 1. set data body
        for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
        {
            i = MIN(i, LCDIM_MAX_BLK_NUM-1);
            u2TempBuf = (((u2IO32Read2B(ADAP_LCDIM_SPI_SEP) >> u1DriverOrder[i]) & 0x1) == 1) ? 
                                0 : (UINT8)(CLIP(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i]+32767)>>15), 0, 0xFF));

            u2TempBuf = (u2TempBuf == 255) ? 2047 : CLIP(u2TempBuf*8, 0, 2047);
            if(pParam->u1ScanningOnoff)
            {
                u2TempBuf = 2047 - u2TempBuf;
            }
            u1SpiCmdArray[3+u1DriverOrder[i]*2] = u2TempBuf & 0xFF;
            u1SpiCmdArray[3+u1DriverOrder[i]*2+1] = (u2TempBuf >> 8) & 0xFF;
        }
    }
    else
    {
        u2DataLen = 3 + (u2DrvLcdimSpiGetBlockNum() + u4DummyChNum) * 2 + (pParam->u1DevNum - 1);

        // 0. set header (device id and register address)
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_BCAST_DIFF | (LDM_LP1887_MULTI_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX << 7));
    
        // 1. set data body (16 bit per block, but each block use 9 bit to represent data actually)
        for(i = 0; i < u2DrvLcdimSpiGetBlockNum(); i++)
        {
            i = MIN(i, LCDIM_MAX_BLK_NUM-1);
            u4SepCtrl = (i < 32) ? u4IO32Read4B(ADAP_LCDIM_SPI_SEP) : u4IO32Read4B(ADAP_LCDIM_SPI_SEP_1);
            u2TempBuf = (((u4SepCtrl >> u1DriverOrder[i]) & 0x1) == 1) ? 
                                0 : (UINT8)(CLIP(((UINT32)((UINT32)u1Buffer[i]*(UINT32)u1DimGain*(UINT32)u1BlockDimGain[i]+32767)>>15), 0, 0xFF));

            u2TempBuf = (u2TempBuf == 255) ? 2047 : CLIP(u2TempBuf*8, 0, 2047);
            if(pParam->u1ScanningOnoff)
            {
                u2TempBuf = 2047 - u2TempBuf;
            }
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

void vDrvLePower1887LightUpCmd(void)
{
    UINT16 u2DataLen, i;
    UINT16 u2BlockNum = MIN(u2DrvLcdimSpiGetBlockNum(), LCDIM_MAX_BLK_NUM-1);

    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  

    if (pParam->u1DevNum == 1)
    {
        // Single device light-up process
        // 1. Set trimming register for LP1887, not to set for LP1889
        if (pParam->u1DevChNum == 8)
        {
            vDrvLePower1887Write1BCmd(LDM_LP1887_TRIMMING, 0x82);
        }

        // 2. Set PWM Period
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 0x2;     // 2 byte command: LSB + MSB
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_PERIOD_LSB | (LDM_LP1887_WRITE_IDX << 7));
        u1SpiCmdArray[3] = 0xFF;
        u1SpiCmdArray[4] = 0x7;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, 5);   

        // 3. Set all dimming to full
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX << 7));

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
                u1SpiCmdArray[3+i*2+1] = 0x7;
            }
        }        
        u2DataLen = 3 + u2BlockNum * 2;
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

        // 4. Set PWM control mode
        if(pParam->u1ScanningOnoff)
        {
            vDrvLePower1887Write1BCmd(LDM_LP1887_PWM_MODE, 0x14);
        }
        else
        {
            vDrvLePower1887Write1BCmd(LDM_LP1887_PWM_MODE, 0x4);
        }

        // 5. Set PLL control mode
        vDrvLePower1887Write1BCmd(LDM_LP1887_PLL_MODE, 0x0);

    }
    else
    {
        // Multi device light-up process
        // 1. Set trimming register for LP1887, not to set for LP1889
        if (pParam->u1DevChNum == 8)
        {
            vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_TRIMMING, 0x82);
        }

        // 2. Set PWM Period
        vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_PWM_PERIOD_LSB, 0xFF);
        vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_PWM_PERIOD_MSB, 0x07);

        // 3. Set all dimming to full
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_BCAST_SAME | (LDM_LP1887_MULTI_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX<<7));

        for(i=0; i < pParam->u1DevChNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x7;
            }
        }        
        u2DataLen = 3 + pParam->u1DevChNum * 2 + (pParam->u1DevNum - 1);
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   

        // 4. Set PWM control mode
        if(pParam->u1ScanningOnoff)
        {
            vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_PWM_MODE, 0x14);
        }
        else
        {
            vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_PWM_MODE, 0x4);
        }

        // 5. Set PLL control mode
        vDrvLePower1887MultiDevWrite1BCmd(LDM_LP1887_BCAST_SAME, LDM_LP1887_PLL_MODE, 0x0);

    }

    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);
    vIO32WriteFldAlign(LED_CON, SV_ON, HW_TRIG_EN);  
}

void vDrvLePower1887LogOffCmd(void)
{
    UINT16 u2BlockNum = u2DrvLcdimSpiGetBlockNum();
    UINT16 u2DataLen = MIN(MAX_SPI_CMD_SIZE, 3 + u2BlockNum * 2);
    UINT16 i;

    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);    
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  

    if (pParam->u1DevNum == 1)
    {
        // 1. Set all dimming to zero
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX << 7));

        // 1.1. set data body (16 bit per block)
        for(i=1; i < u2BlockNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x7;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
        }        
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen); 
    }
    else
    {
        // 1. Set all dimming to zero
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_BCAST_SAME | (LDM_LP1887_MULTI_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_PWM_HTIME1 | (LDM_LP1887_WRITE_IDX<<7));

        for(i=0; i < pParam->u1DevChNum; i++)
        {
            if(pParam->u1ScanningOnoff)
            {
                u1SpiCmdArray[3+i*2] = 0xFF;
                u1SpiCmdArray[3+i*2+1] = 0x7;
            }
            else
            {
                u1SpiCmdArray[3+i*2] = 0;
                u1SpiCmdArray[3+i*2+1] = 0;
            }
        }        
        u2DataLen = 3 + pParam->u1DevChNum * 2 + (pParam->u1DevNum - 1);
        vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
    }
    
    LOG(0, "SPI LogOff\n");
}

void vDrvLePower1887SetScanning(UINT8 u1ScanningIdx)
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
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_ADDR1 | (LDM_LP1887_SINGLE_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * u2BlockNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_DELAY1 | (LDM_LP1887_WRITE_IDX << 7));
    }
    else
    {
        u2DataLen = 3 + u2BlockNum * 2 + (pParam->u1DevNum - 1);
        u1SpiCmdArray[0] = (UINT8)(LDM_LP1887_BCAST_DIFF | (LDM_LP1887_MULTI_DEV<<7) | (LDM_LP1887_MULTI_BYTE<<6));
        u1SpiCmdArray[1] = 2 * pParam->u1DevChNum;
        u1SpiCmdArray[2] = (UINT8)(LDM_LP1887_DELAY1 | (LDM_LP1887_WRITE_IDX << 7));
    }

    for(i=0; i < u2BlockNum; i++)
    {
        u2Temp = MIN(u1PanelDelay[i] * 8, 0x7FF);
        u1SpiCmdArray[3+i*2] = u2Temp & 0xFF;
        u1SpiCmdArray[3+i*2+1] = u2Temp >> 8;
    }        

    u2DataLen = MIN(u2DataLen, MAX_SPI_CMD_SIZE);
    vDrvLcdimSpiSendDataSWMode(u1SpiCmdArray, u2DataLen);   
}

void vDrvLePower1887SetScanningCmd(UINT8 u1ScanningIdx)
{
    vDrvLcdimSpiSetLoopAllOnOff(SV_OFF);
    vIO32WriteFldAlign(LED_CON, 1, OP_MODE);  
    vDrvLePower1887SetScanning(u1ScanningIdx);
    vDrvLcdimSpiSetLoopAllOnOff(SV_ON);
}

