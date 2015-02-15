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
 * $RCSfile: panel.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file panel.h
 *  This file includes panel definitions.
 */

#ifndef PANEL_H
#define PANEL_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_typedef.h"
#include "drvcust_if.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// this file was place on each customer's directory
#include "panel_cust.h"

#define PANEL_DEFAULT 0
#define PANEL_INVALID 0xff

#ifndef PANEL_SELECT    /* NO CHANGE */
#define PANEL_SELECT PANEL_DEFAULT /* DO NOT CHANGE THIS SETTING!!! */
#endif /* PANEL_SELECT */


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

// display mode
#define DISP_MODE_OFF 0
#define DISP_MODE_ON 1
    #define DISP_MODE_USE DISP_MODE_OFF

// no tube in 537x
#define DISPLAY_LCD 0
#define DISPLAY_TUBE 1
    #define USE_DISPLAY_TYPE DISPLAY_LCD

// no interlace mode in 537x
#define PROGRESSIVE_MODE 0
#define INTERLACE_MODE 1
    #define INTERLACE_ENABLE PROGRESSIVE_MODE

// scpos
#define SRGB_EN_OFF 0
#define SRGB_EN_ON 1
    #define SRGB_FUNCTION_EN SRGB_EN_OFF
#define DEN_NO_ACIVE 0
#define DEN_ACIVE 1
    #define DEN_TYPE DEN_NO_ACIVE
#define DEN_HIGH 0
#define DEN_LOW 1
    #define DEN_POLARITY DEN_HIGH
#define FIELD_HIGH 0
#define FIELD_LOW 1
    #define FIELD_POLARITY FIELD_HIGH
#define HSYNC_HIGH 0
#define HSYNC_LOW 1
    #define HSYNC_POLARITY HSYNC_HIGH
#define VSYNC_HIGH 0
#define VSYNC_LOW 1
    #define VSYNC_POLARITY VSYNC_HIGH
#define RB_CHANNEL_INVERSE_OFF 0
#define RB_CHANNEL_INVERSE_ON 1
    #define RB_CHANNEL_INVERSE RB_CHANNEL_INVERSE_OFF
#define BIT_ORDER_INVERSE_OFF 0
#define BIT_ORDER_INVERSE_ON 1
    #define BIT_ORDER_INVERSE BIT_ORDER_INVERSE_OFF

// dump flag
#define PANEL_DUMP_CURRENT 0
#define PANEL_DUMP_ALL 1
#define PANEL_DUMP_TITLE 2


// luis@2007/10/4, no more use
#if 0
    // lvds
    #define LVDS_OUT_OFF 0
    #define LVDS_OUT_ON 1
        #define LVDS_OUT_USE LVDS_OUT_ON
    #define LVDS_RESERVED_LOW 0
    #define LVDS_RESERVED_HIGH 1
    #define LVDS_RESERVED_FIELD 2
        #define LVDS_RESERVED_TYPE LVDS_RESERVED_LOW

    // dac
    #define DAC_OUT_OFF 0
    #define DAC_OUT_ON 1
        #define DAC_OUT_USE DAC_OUT_ON
    #define DA_CLOCK_NORMAL 0
    #define DA_CLOCK_INVERSE 1
        #define DA_CLOCK_POARITY DA_CLOCK_NORMAL
    #define DACCLK_DELAY_SEL 0

    // svm
    #define SVM_DISABLE 0
    #define SVM_ENABLE 1
        #define SUPPORT_SVM SVM_DISABLE

    // ttl
    #define TTL_OUT_OFF 0
    #define TTL_OUT_ON 1
        #define TTL_OUT_USE TTL_OUT_OFF
    #define TTL_CLOCK_NORMAL 0
    #define TTL_CLOCK_INVERSE 1
        #define TTL_CLOCK_POARITY TTL_CLOCK_NORMAL
    #define TTLCLK_DELAY_SEL 0
    #define TTL_DRIVING_SEL 0x1

    // dither
    #define NO_DITHER 0 //no dither
    #define DITHER_1210 1 //12-bit dither to 10-bit
    #define DITHER_128 2 //12-bit dither to 8-bit
    #define DITHER_126 3 //12-bit dither to 6-bit
        #define DITHER_MODE DITHER_128//NO_DITHER
    #define ROUND_OFF 0
    #define ROUND_ON 1
        #define ROUND_FUNCTION_EN ROUND_ON//ROUND_OFF
    #define DITHER_ROUND_OFF 0
    #define DITHER_ROUND_ON 1
        #define DITHER_ROUND_EN DITHER_ROUND_OFF
    #define ERROR_DITHER_OFF 0
    #define ERROR_DITHER_ON 1
        #define ERROR_DITHER_EN ERROR_DITHER_ON//ERROR_DITHER_OFF
    #define FPHASE_VALUE 0x00
#endif


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

extern void LoadPanelIndex(void);
extern void WritePanelIndexToEeprom(UINT32 u4PanelIndex);
extern void SelectPanel(UINT32 u4Index);
extern CHAR* GetCurrentPanelName(void);
extern UINT32 GetCurrentPanelIndex(void);
extern CHAR* GetPanelName(UINT32 u4Index);
extern void DumpPanelAttribute(UINT32 u4Flag);
extern UINT32 IsLvdsMsbSwOn(void);
extern UINT32 IsPwmLowPanelBright(void);
extern UINT32 PANEL_GetBacklightHigh(void);
extern UINT32 PANEL_GetBacklightMiddle(void);
extern UINT32 PANEL_GetBacklightLow(void);
extern UINT32 PANEL_TranslateEepromToFlashIndex(UINT32 u4EepromIndex);
extern UINT32 PANEL_GetEepromPanelIndex(void);
extern void PANEL_SetPixelShift(BOOL fgEnable, INT32 i4HPixelShift, INT32 i4VPixelShift);

extern UINT32 PANEL_GetPanelWidth(void);
extern UINT32 PANEL_GetPanelHeight(void);
extern UINT32 PANEL_GetPixelClkMax(void);
extern UINT32 PANEL_GetPixelClk60Hz(void);
extern UINT32 PANEL_GetPixelClk50Hz(void);
extern UINT32 PANEL_GetPixelClkMin(void);
extern UINT32 PANEL_GetHTotalMax(void);
extern UINT32 PANEL_GetHTotal60Hz(void);
extern UINT32 PANEL_GetHTotal50Hz(void);
extern UINT32 PANEL_GetHTotalMin(void);
extern UINT32 PANEL_GetHSyncLen60Hz(void);
extern UINT32 PANEL_GetHSyncLen50Hz(void);
extern UINT32 PANEL_GetVTotalMax(void);
extern UINT32 PANEL_GetVTotal60Hz(void);
extern UINT32 PANEL_GetVTotal50Hz(void);
extern UINT32 PANEL_GetVTotalMin(void);
extern UINT32 PANEL_GetVClkMax(void);
extern UINT32 PANEL_GetVClkMin(void);
extern UINT32 PANEL_GetHSyncWidth(void);
extern UINT32 PANEL_GetVSyncWidth(void);
extern UINT32 PANEL_GetHPosition60Hz(void);
extern UINT32 PANEL_GetHPosition50Hz(void);
extern UINT32 PANEL_GetVPosition(void);
extern UINT32 PANEL_GetControlWord(void);
extern UINT32 PANEL_GetBacklightHighHwPwm(void);
extern UINT32 PANEL_GetBacklightMiddleHwPwm(void);
extern UINT32 PANEL_GetBacklightLowHwPwm(void);
extern UINT32 PANEL_GetDimmingFrequency60Hz(void);
extern UINT32 PANEL_GetDimmingFrequency50Hz(void);
extern UINT32 PANEL_GetLvdsOnDalay(void);
extern UINT32 PANEL_GetBacklightOnDelay(void);
extern UINT32 PANEL_GetBacklightOffDelay(void);
extern UINT32 PANEL_GetLvdsOffDalay(void);
extern UINT32 PANEL_GetMaxOverscan(void);

extern void PANEL_SetPanelWidth(UINT32 u4Value);
extern void PANEL_SetPanelHeight(UINT32 u4Value);
extern void PANEL_SetPixelClkMax(UINT32 u4Value);
extern void PANEL_SetPixelClk60Hz(UINT32 u4Value);
extern void PANEL_SetPixelClk50Hz(UINT32 u4Value);
extern void PANEL_SetPixelClkMin(UINT32 u4Value);
extern void PANEL_SetHTotalMax(UINT32 u4Value);
extern void PANEL_SetHTotal60Hz(UINT32 u4Value);
extern void PANEL_SetHTotal50Hz(UINT32 u4Value);
extern void PANEL_SetHTotalMin(UINT32 u4Value);
extern void PANEL_SetVTotalMax(UINT32 u4Value);
extern void PANEL_SetVTotal60Hz(UINT32 u4Value);
extern void PANEL_SetVTotal50Hz(UINT32 u4Value);
extern void PANEL_SetVTotalMin(UINT32 u4Value);
extern void PANEL_SetVClkMax(UINT32 u4Value);
extern void PANEL_SetVClkMin(UINT32 u4Value);
extern void PANEL_SetHSyncWidth(UINT32 u4Value);
extern void PANEL_SetVSyncWidth(UINT32 u4Value);
extern void PANEL_SetHPosition60Hz(UINT32 u4Value);
extern void PANEL_SetVPosition(UINT32 u4Value);
extern void PANEL_SetControlWord(UINT32 u4Value);
extern void PANEL_SetBacklightHighHwPwm(UINT32 u4Value);
extern void PANEL_SetBacklightMiddleHwPwm(UINT32 u4Value);
extern void PANEL_SetBacklightLowHwPwm(UINT32 u4Value);
extern void PANEL_SetDimmingFrequency60Hz(UINT32 u4Value);
extern void PANEL_SetDimmingFrequency50Hz(UINT32 u4Value);
extern void PANEL_SetLvdsOnDalay(UINT32 u4Value);
extern void PANEL_SetBacklightOnDelay(UINT32 u4Value);
extern void PANEL_SetBacklightOffDelay(UINT32 u4Value);
extern void PANEL_SetLvdsOffDalay(UINT32 u4Value);
extern void PANEL_SetMaxOverscan(UINT32 u4Value);
extern INT32 DRVCUST_PanelQuery(QUERY_TYPE_T eQryType, UINT32 *pu4Data);
//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

// replace these legacy global variable with function call

#define TV_WIDTH PANEL_GetPanelWidth()
#define TV_HEIGHT PANEL_GetPanelHeight()
#define wDISPLAY_WIDTH PANEL_GetPanelWidth()
#define wDISPLAY_HEIGHT PANEL_GetPanelHeight()

#define PIXEL_SHIFT_H_MAX 4
#define PIXEL_SHIFT_V_MAX 4

#define HLEN_TOTAL_TYPI PANEL_GetHTotal60Hz()
#define VLEN_TOTAL_TYPI PANEL_GetVTotal60Hz()
#define HSYNCLEN_TYPI PANEL_GetHSyncLen60Hz()
#define LEFT_MASK_TYPI 0
#define TOP_MASK_TYPI 0
#define RIGHT_MASK_TYPI (UINT16)(((HLEN_TOTAL_TYPI-HSYNCLEN_TYPI)-PANEL_GetPanelWidth())-LEFT_MASK_TYPI)
#define BOTTOM_MASK_TYPI (UINT16)((VLEN_TOTAL_TYPI-PANEL_GetPanelHeight())-TOP_MASK_TYPI)

#define HLEN_TOTAL_50HZ PANEL_GetHTotal50Hz()
#define VLEN_TOTAL_50HZ PANEL_GetVTotal50Hz()
#define HSYNCLEN_50HZ PANEL_GetHSyncLen50Hz()
#define LEFT_MASK_50HZ 0
#define TOP_MASK_50HZ 0
#define RIGHT_MASK_50HZ (UINT16)(((HLEN_TOTAL_50HZ-HSYNCLEN_50HZ)-PANEL_GetPanelWidth())-LEFT_MASK_50HZ)
#define BOTTOM_MASK_50HZ (UINT16)((VLEN_TOTAL_50HZ-PANEL_GetPanelHeight())-TOP_MASK_50HZ)

#define HS_WIDTH PANEL_GetHSyncWidth()
#define VS_WIDTH PANEL_GetVSyncWidth()
#define H_POS_60HZ PANEL_GetHPosition60Hz()
#define H_POS_50HZ PANEL_GetHPosition50Hz()
#define V_POS PANEL_GetVPosition()

// panel limitation, for display mode use
#define DCLK_MIN PANEL_GetPixelClkMin()
#define DCLK_MAX PANEL_GetPixelClkMax()
#define DHS_TOTAL_MIN PANEL_GetHTotalMin()
#define DHS_TOTAL_MAX PANEL_GetHTotalMax()
#define DVS_TOTAL_MIN PANEL_GetVTotalMin()
#define DVS_TOTAL_MAX PANEL_GetVTotalMax()
#define DVS_FREQ_MIN PANEL_GetVClkMin()
#define DVS_FREQ_MAX PANEL_GetVClkMax()

// lvds
// msb switch
#define LVDS_JEIDA LVDS_MSB_SW_OFF
#define LVDS_NS LVDS_MSB_SW_ON
#define LVDS_MSB_SW_OFF 0 // {MSB 7,6,5,4,3,2,1,0 LSB}
#define LVDS_MSB_SW_ON 1 // {MSB 5,4,3,2,1,0,7,6 LSB}
    #define LVDS_DISP_MSB_SW IsLvdsMsbSwOn()

// odd/even pixel stitch
#define LVDS_ODD_SW_OFF 0 // even in A0~A3, odd in A4~A7
#define LVDS_ODD_SW_ON (1 << 1)
    #define LVDS_DISP_ODD_SW (PANEL_GetControlWord() & LVDS_ODD_SW_ON)

// lvds port
#define SINGLE_PORT 0
#define DUAL_PORT (1 << 2)
    #define LVDS_OUTPUT_PORT (PANEL_GetControlWord() & DUAL_PORT)

// dynamic sync frame rate, frame rate will stay 60Hz when WFB_MODE_OFF
/*
#define WFB_MODE_ON 0
#define WFB_MODE_OFF (1 << 3)
    #define WFB_LOCK_ENABLE WFB_MODE_ON(PANEL_GetControlWord() & WFB_MODE_OFF)
*/
// display bit
#define DISP_18BIT 0 // 18-bit RGB output
#define DISP_24BIT (1 << 4) // 24-bit RGB output
#define DISP_30BIT (2 << 4) // 30-bit RGB output
#define DISP_BIT_MSK (3 << 4)
    #define DISP_BIT (PANEL_GetControlWord() & DISP_BIT_MSK)

// PWM_HIGH_PANEL_BRIGHT : panel will be brighter when duty cycle is higher
#define PWM_HIGH_PANEL_BRIGHT 0
#define PWM_LOW_PANEL_BRIGHT (1 << 6)
    #define PANEL_BRIGHT_SETTING IsPwmLowPanelBright()

#define PANEL_BACKLIGHT_HIGH bApiGetBackLightTbl(BACKLIGHT_HIGH)
#define PANEL_BACKLIGHT_MIDDLE bApiGetBackLightTbl(BACKLIGHT_MIDDLE)
#define PANEL_BACKLIGHT_LOW bApiGetBackLightTbl(BACKLIGHT_LOW)

// reset panel when switch frame rate
#define WFB_PANEL_RESET_OFF 0
#define WFB_PANEL_RESET_ON (1 << 7)
    #define WFB_PANEL_RESET (PANEL_GetControlWord() & WFB_PANEL_RESET_ON)

#endif /* PANEL_H */
