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
 * $RCSfile: drv_adcset.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
//#ifdef CC_MT5365_V2
//#include "drv_adcset_V2.c"
//#else
#define _ADCSET_C_

#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif
#include "general.h"
#include "hw_vdoin.h"
#include "drv_adcset.h"
#include "drv_vdoclk.h"
#include "drv_tdc.h"
#include "hw_tvd.h"
#include "api_eep.h"
#include "eepdef.h"
#include "source_table.h"
#include "source_select.h"
//#include "x_pdwnc.h"
#include "video_def.h"
#include "hw_hdtv.h"
#include "debug.h"
#include "hw_async.h"
#include "util.h"
#include "x_printf.h"
#ifndef CC_COPLAT_MT82
#include "x_os.h"
#endif
#include "drv_async.h"
#include "x_timer.h"
#include "nptv_debug.h"
#include "x_typedef.h"
#include "x_hal_5381.h"
#if SUPPORT_SCART
#include "drv_scart.h"
#endif
#include "hw_dvi.h"
#include "drvcust_if.h"
#include "drv_autocolor.h"
#ifndef CC_COPLAT_MT82
#include "eeprom_if.h"
#endif
#include "hw_ckgen.h"
#include "x_ckgen.h"
#include "hw_pdwnc.h"
#include "drv_hdtv.h"
#include "drv_scpos.h"
#include "hw_scslicer.h"
#include "x_hal_5381.h"

#ifdef __MODEL_slt__
#include "x_drv_cli.h"
#endif

EXTERN IC_VERSION_T BSP_GetIcVersion(void);

#ifdef CC_COPLAT_MT82
// cyj.todo.20090115
BOOL fgApiEepromWriteByte(UINT16 wAddr, UINT8 bData)
{
        return TRUE;
}
#endif

UINT32 bINT_BST_DET;
UINT8 _bPGAGainOffsetHistory[4];
UINT8 _bEFUSE_AUTOCOLOR_READY;
UINT8 _bcurrent_use_adc;

#ifdef __MODEL_slt__
BOOL fgCVBSFail = FALSE;
#endif

static struct
{
	UINT8 AP_SliceLevelManuSet;
	UINT8 AP_SVTL_ManuValue;
	UINT8 AP_SVTH_ManuValue;
       UINT8 AP_SVTLH_ManuValue;
} rSlicerConfig={0,0,0,0};

extern UINT8 code bVGADefaultGainTABLE_100[4][3];
#ifdef AUTOCOLOR_NEW_METHOD
extern UINT16 code bVGADefaultGainTABLE_100_DIGITAL[4][3];
extern UINT16 code bVGADefaultOffsetTABLE_100_DIGITAL[4][3];
extern UINT16 code wColorBlankValueNew[4][3];
#endif
extern UINT8 code bVGADefaultOffsetTABLE_100[4][3];
extern UINT8 code bVGADefaultGainTABLE_120[4][3];
extern UINT8 code bVGADefaultOffsetTABLE_120[4][3];
extern UINT8 code bVGADefaultGainTABLE_75_100[4][3];
extern UINT8 code bVGADefaultOffsetTABLE_75_100[4][3];
extern UINT16 code wOnChipColorGainTable[4][3];
extern UINT16 code wOnChipColorGainTable_75[4][3];
extern UINT16 code wOnChipColorGainTable_Temp[4][3];
extern INT16 code bEFuseCompensation[4][3];
extern void vDrvGeneralAutoColorGainSetToNextType(UINT16* ColorGainTablePtr, UINT8 bOrgType, UINT8 bNextType);
extern UINT8 bDvMeasureDataSel(void);
//////////////////////////////////////////////////////////////////////

// Firmware Flow Description
//////////////////////////////////////////////////////////////////////
// vDrvVideoHwInit
// => vDrvADCDefaultSetting
//	=> vDrvTVADCDefaultSetting()
//	=> vDrvHDTVADCDefaultSetting()
//
// vApiVideoInit
// => vDrvADCOffsetCal
//	=> vDrvCvbsAdcCal
//		=> vDrvTVADC0CH1ClampOnBottomOffsetCal();
//		=> vDrvTVADC0CH0ClampOnBlankOffsetCal();
//		=> vDrvAAFCalibration();
//			=> vDrvCalibrationEnable
//			=> vDrvCalibrationDisable
//			=> vDrvInputOffsetCal
//			=> wDrvOutputOffsetCal
//			=> vDrvDFE
//	=> vDrvHDTVADCOffsetCal
//
// bApiVideoMainSubSrc
// => Bypass Control
// 	vDrvBypPD
//	vDrvAvByp
//	vDrvSVdoByp
// => AD Setting
//	vDrvSetTVADC
//	vDrvAvMux (source_table.h)
//	vDrvCvbsAdcPD
//	vDrvSetHDTVADC
//	vDrvSetHDTVMux
//	vDrvAllHDADCPow
// => SCART Canal Plus

//////////////////////////////////////////////////////////////////////
// Private Function
//////////////////////////////////////////////////////////////////////

// => vDrvADCDefaultSetting
/**
 * @brief Cvbs ADC Setting
 *
 * @param bVDOINPUT bVDOINPUT is the input types, like CVBS\SVDO
 */
void vDrvTVADCDefaultSetting(void)
{
//    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_MIX_PWD);//
//    vIO32WriteFldAlign(REG_CVBS_CFG1, 0, RG_MIXGAIN);//
//    vIO32WriteFldAlign(REG_CVBS_CFG1, 1, RG_MIXINPUT);//
   // vIO32WriteFldAlign(VFE_E0, 0, RG_BTM_ENA); /*add for V2 */
    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_BOTTOM_EN); /*add for V2 */
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CTCLAMP_CTMA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CTCLAMP_CTMB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SOURCE_ENA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SOURCE_ENB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SINK_ENA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SINK_ENB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0x1f, RG_CLAMP_SOURCEA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 9, RG_CLAMP_SOURCEB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 5, RG_CLAMP_SINKA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 5, RG_CLAMP_SINKB);//
   // vIO32WriteFldAlign(VFE_E1, 5, RG_CLAMP_SINK); /*add for V2 */
    vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_BLANK_EN);//
    //vIO32WriteFldAlign(VFE_E1, 1, RG_UPDNB);//
    //vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELA);//
    //vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHA_MIDDLE_EN);//
    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHB_MIDDLE_EN);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_DC_EN);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_DC_EN);//

//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0, RG_UPDN_2);//
    //vIO32WriteFldAlign(VFE_E1, 1, RG_UPDNB);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0, RG_VAGSELA_2);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 1, RG_C_SELA_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0, RG_C_SELB_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0, RG_VAGSELB_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 1, RG_BTM_EN_2);

    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_HPCLAMPA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_HPCLAMPB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CLAMPC_TSTA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CLAMPC_TSTB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTCLAMPA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTCLAMPB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTBUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTBUFB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_INMUX_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGAA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGAB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_AAFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_AAFB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBS108M_EN);
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0x0D, RG_CVBSADC2_REV1);//Setting CHB gain
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNA_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNB_PWD);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_PGABUFNA_PWD_2);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_PGABUFNB_PWD_2);//

    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGABUFPA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGABUFPB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BUFB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTA_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_OFFCURA_PWD); /*add for V2 */
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_OFFCURB_PWD);  /*add for V2 */
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_SHIFTA_PWD_2);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_SHIFTB_PWD_2);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_OFFCURA_PWD_2); /*add for V2 */
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_OFFCURB_PWD_2);  /*add for V2 */
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_SHIFT2A_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_SHIFT2B_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFB);//

    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E3, 3, RG_AAF_BWA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 3, RG_AAF_BWB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_RS_AAFA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_RS_AAFB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTINA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTINB);//

    vIO32WriteFldAlign(REG_CVBS_CFG0, 0x01, RG_AISEL);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_FE_SELA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_FE_SELB);//
    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_AISELR);//
    vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_LPF_BW ); /*add for V2 */
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0, RG_AISELR_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 3, RG_AAF_BW_2); /*add for V2 */

    //vIO32WriteFldAlign(VFE_E5, 1, RG_MIX_10U);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_MIX_60U_EN);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_MIX_SHORT);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG1, 2, RG_MIXVOCM_SEL);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_BUF_MONEN);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_VCM_MONEN);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 0, RG_BIAS_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_40U_PGAOFFA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_40U_PGAOFFB);//
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_30U_BUFA);//
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_30U_BUFB);//
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_20U_CLAMPA);//
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E4, 1, RG_20U_CLAMPB);//

     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 1, RG_20U_PGAOFFA);//
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 1, RG_20U_PGAOFFB);//
//    vIO32WriteFldAlign(REG_CVBS_CFG8, 1, RG_20U_MIXOFF);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 0x0f, RG_SIFGAINA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 0x0f, RG_SIFGAINB);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 0, RG_SIF_ENA);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E5, 0, RG_SIF_ENB);//
//    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CINRA_EN);//
//    vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CINRB_EN);//
    vIO32WriteFldAlign(REG_CVBS_CFG1, 0x0, RG_CVBS_REV);//
//    #ifdef CC_MT5389
//	vIO32WriteFldAlign(REG_CVBS_CFG2, 0x1, RG_GCOUPLE_ENB);//
//    #endif
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_SEL_CKPLL);// CY 1->0
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_SEL_CKPLL);// CY 1->0
//    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_CVBSADC_CKCTRL);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBSADC_SEL_CKPLL_2);// CY 1->0
 //   vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBSADC_CKCTRL_2);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_BG_PD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E6, 0, RG_CVBSADC_VRSEL1); //
//    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_CVBSADC_BIASSEL);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//
//    vIO32WriteFldAlign(REG_CVBS_CFG3, 3, RG_CVBSADC_CHSEL);//

//    vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_CVBSADC_BIASSEL_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBSADC_PWD_2);//
//    vIO32WriteFldAlign(REG_CVBS_CFG7, 3, RG_CVBSADC_CHSEL_2);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_50U_CLAMP);//
    //vIO32WriteFldAlign(VFE_E4, 2, RG_50U_CLAMPA);// kal 30uA->40uA
    //vIO32WriteFldAlign(VFE_E4, 2, RG_50U_CLAMPB);//
    #ifdef CC_SUPPORT_SIF_BYPASS
     vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSPGA_CHAB_DIS);//Enable CHB PGA manual gain
    #else
     vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSPGA_CHAB_DIS);//disable CHB PGA manual gain
    #endif
    vIO32WriteFldAlign(VSRC_07, 0x00, VSRC_INV_AIDX); //[SWAP AIDX] could be "00" or "11" in MT5381
    vIO32WriteFldAlign(VSRC_07, 0x01, VSRC_SWAP_AD01); //in MT537x, [SWAP AIDX] = "01" and maybe "10"

//SIFP
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0x40, RG_AISEL_2);//CHC=SIFP
//    vIO32WriteFldAlign(REG_CVBS_CFG7, 0x0, RG_INMUX_PWD_2);
//    vIO32WriteFldAlign(REG_CVBS_CFG7, 0x0, RG_PGABUFNA_PWD_2);
   // vIO32WriteFldAlign(VFE_E1, 0x07, RG_CALCLAMPB);
    //set PGA Gain
   vIO32WriteFldAlign(VSRC_01, 0x01, VSRC_MUX1);
   vIO32WriteFldAlign(VSRC_00, 0x04, TVD3D_MPGAC);

//    vIO32WriteFldAlign(REG_CVBS_CFG5, 0x11, RG_OFFCURB_2);
//    vIO32WriteFldAlign(REG_CVBS_CFG5, 0x0D, RG_PGAGAINB_2);
//    vIO32WriteFldAlign(REG_CVBS_CFG7, 0x01, RG_SHIFTA_PWD_2);
    //AAF Gain
//    vIO32WriteFldAlign(REG_CVBS_CFG4, 0x00, RG_VAGSELA_2);
//   vIO32WriteFldAlign(REG_CVBS_CFG6,  0x01, RG_SIFA_EN_2);
    //TVD_CLK and TVD_CLK54
    //vIO32Write1BMsk(REG_OCLK, 0xc0, TVD_CLK_SEL); //reversed index;the same with MT537x and MT538x
    //vIO32Write1BMsk(REG_OCLK, 0x20, TVD_CK54_REV); //reversed;the same with MT537x and MT538x
    // 5365 mark vIO32WriteFldAlign(CKGEN_AFECFG4, 1, FLD_RG_SYS_CVBS_SEL);//kal 108MHz
    vIO32WriteFldAlign(CKGEN_VPCLK_STOP, 0, FLD_TVD_CK54_STOP);//kal
    vIO32WriteFldAlign(CKGEN_VPCLK_STOP, 0, FLD_TVD3D_STOP);//kal
    vIO32WriteFldMulti(CKGEN_VPCLK_STOP,P_Fld(0,FLD_PIP_CLK_STOP)|P_Fld(0,FLD_MAIN_CLK_STOP));
    vIO32WriteFldAlign(SYS_05, 0, TVD3D_ADC_SEL);
}

//////////////////////////////////////////////////////////////////////
// Public Function
//////////////////////////////////////////////////////////////////////
// => vDrvVideoHwInit
/**
 * @brief ADC Default Setting
 *
 * @param None
 */
void vDrvADCDefaultSetting(void)
{
	vDrvTVADCDefaultSetting();
	vDrvHDTVADCDefaultSetting();
}

// => vApiVideoInit
/**
 * @brief ADC Offset calibration
 *
 * @param None
 */
void vDrvADCOffsetCal(void)
{
    //UINT8 bEEPROMDATA0,bLoopCnt;
	//CVBS ADC Calibration
	#ifndef CC_MTK_LOADER
     x_os_drv_set_timestamp("vDrvADCOffsetCal begin");
    #endif
	vDrvCvbsAdcCal();

    vIO32WriteFldAlign(HDFE_00, 0xFF, AD1_OFFSET);  //set digital offset =1 for CH 1
    vIO32WriteFldAlign(HDFE_00, 0xFF, AD2_OFFSET);	 //set digital offset =1 for CH 2
    vIO32WriteFldAlign(HDFE_00, 0xFF, AD3_OFFSET);  //set digital offset =1 for CH 3

    #ifndef CC_MTK_LOADER
     x_os_drv_set_timestamp("vDrvADCOffsetCal end");
    #endif
}

// => vDrvSetTVADC
/**
 * @brief SVDO enable
 *
 * @param None
 */
void vDrvSVdoEnable(void)
{
	// Enable SVDO
	vIO32WriteFldAlign(VSRC_07, 1, VSRC_SVID);
	vIO32WriteFldAlign(SECAM_01, 0, DOT_REDUCE);
   #if defined(CC_SUPPORT_TVE)||defined(CC_SUPPORT_TVE_82xx)
	//for SCART out
    vIO32WriteFldAlign(SVF_03, 0x01, SVF_YCMIX_SEL);
    vIO32WriteFldAlign(SVF_03, 0x01, SVF_MOFC_EN); // for SECAM HW modification
   #endif
}

// => vDrvSetTVADC
/**
 * @brief SVDO Disable
 *
 * @param None
 */
void vDrvSVdoDisable(void)
{
	vIO32WriteFldAlign(VSRC_07, 0, VSRC_SVID);
	vIO32WriteFldAlign(SECAM_01, 1, DOT_REDUCE);
   #if defined(CC_SUPPORT_TVE)||defined(CC_SUPPORT_TVE_82xx)
	//for SCART out
    vIO32WriteFldAlign(SVF_03, 0x00, SVF_YCMIX_SEL);
    vIO32WriteFldAlign(SVF_03, 0x00, SVF_MOFC_EN);
   #endif
}

// => bApiVideoMainSubSrc
/**
 * @brief AV/SVDO BYPASS Power down
 * @param None
 */
void vDrvBypPD(void)
{
 //   vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_MIX_PWD);// //power down mixer
 //   vIO32WriteFldAlign(REG_CVBS_CFG1, 1, RG_MIXINPUT);// CH A
}
// => bApiVideoMainSubSrc
/**
 * @brief AV Byopass enable
 * @param None
 */
void vDrvAvByp(void)
{
    //Use mixer 0 for AV bypass
  //  vIO32WriteFldAlign(REG_CVBS_CFG1, 1, RG_MIXINPUT);//   //input ChA
 //   vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_MIX_PWD);//      // Power on  Mixer0
//    vIO32WriteFldAlign(REG_CVBS_CFG1, 5, RG_MIXGAIN);//      //MIX0 gain = 1.65X
    /*mark for V2*/ //vIO32WriteFldMulti(VFE_E4, P_Fld(1,RG_MIX_60U_EN)|P_Fld(0,RG_MIX_SHORT));//
    //vIO32WriteFldAlign(VFE_E5, 1, RG_MIX_10U);//      //MIX0 gain = 1.65X
}

void vDrvSVdoByp(void)  //MT5365 can not support A+B channel
{

    //Use mixer 0 for AV bypass
    //vIO32WriteFldAlign(REG_CVBS_CFG1, 3, RG_MIXINPUT);
    //vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_MIX_PWD);//      // Power on  Mixer0
    //vIO32WriteFldAlign(REG_CVBS_CFG1, 5, RG_MIXGAIN);//      //MIX0 gain = 1.33X
    //vIO32WriteFldMulti(VFE_E5, P_Fld(1,RG_MIX_10U));//
    /*mark for V2*/ //vIO32WriteFldMulti(VFE_E4, P_Fld(1,RG_MIX_60U_EN)|P_Fld(0,RG_MIX_SHORT));//

}

static struct
{
    UINT8 AP_SrcMuxCtrl;
	UINT8 AP_SrcMuxVal;
	UINT8 AP_GainCtrl;
	UINT8 AP_GainVal;
} rChBConfig={0,0,0,0};

/**
 * @brief bDrvSetTVDCHBGain(void)
 * @param  void
 * @retval UINT8 manual analog gain code
 */
void vDrvSetTVDCHBSrcMux(UINT8 value)
{
	rChBConfig.AP_SrcMuxCtrl=1;
	rChBConfig.AP_SrcMuxVal=value;
	vIO32WriteFldAlign(VSRC_01, value, VSRC_MUX1);	//Set ChB Gain code from HDTV_04
	if(rChBConfig.AP_GainCtrl)
	{
		vIO32WriteFldAlign(HDTV_04, rChBConfig.AP_GainVal, HDTV_AGC);	//Set ChB gain="10001"==>1.496x;0418_007
	}
}

/**
 * @brief bDrvSetTVDCHBGain(void)
 * @param  void
 * @retval UINT8 manual analog gain code
 */
void vDrvSetTVDCHBGain(UINT8 value)
{
	rChBConfig.AP_GainCtrl=1;
	rChBConfig.AP_GainVal=value;
	if(rChBConfig.AP_SrcMuxCtrl)
	{
		vIO32WriteFldAlign(HDTV_04, value, HDTV_AGC);	//Set ChB gain="10001"==>1.496x;0418_007
	}
}

// => bApiVideoMainSubSrc
/**
 * @brief Cvbs ADC Setting
 *
 * @param bVDOINPUT bVDOINPUT is the input types, like CVBS\SVDO
 */
void vDrvSetTVADC(UINT8 vsrc)
{

	switch (VSS_MAJOR(vsrc))
	{
		case VSS_ATD:
			/* enable internal ATV demod setting here */
			//Printf(">>>>>>enter set VSS_ATD\n");
			vTvd3dATDInit(SV_ON);
#ifdef CC_SOURCE_AUTO_DETECT
			//LOG(0,"=========source detect:settvadc:don't pd\n");
#else
			vDrvCvbsVfePD();
#endif

            vIO32WriteFldMulti(CKGEN_VPCLK_CFG,P_Fld(2,FLD_TVD_CLK_SEL)|P_Fld(1,FLD_ADC_C54M_SEL)|P_Fld(0,FLD_TVD_CK54_SEL));

			vIO32WriteFldAlign(DFE_03, 0x0c, AGC2_MODE);
			vDrvSVdoDisable();							//SVDO Disable
			vIO32WriteFldAlign(VSRC_07, SV_ON, ATD_ENABLE);	//set ATD path for TVD
			break;

		case VSS_ATV:
			vTvd3dATDInit(SV_OFF);
			vDrvCvbsVfePWON();
			vIO32WriteFldAlign(VSRC_01, 2, VSRC_MUX1);	//Set ChB Gain code from HDTV_04
			vIO32WriteFldAlign(HDTV_04, 0x11, HDTV_AGC);	//Set ChB gain="10001"==>1.496x;0418_007
                   /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0x20, RG_CLAMP_SOURCEB);//
                   /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_CLAMP_SINK_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_CLAMP_SOURCE_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
			vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHB_MIDDLE_EN);//
			//vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELB);//
			//vIO32WriteFldAlign(VFE_E1, 0, RG_UPDNB);//
			vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_SHIFT2B_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFB);//

			// Printf(">>>>>>enter set ATV ADC\n");
			/*************************/
			//setting of ADC, ADC clock and TVD clock;0523_2007
    			vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
    			vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
    			vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
    			vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//
            //SWAP ChA/ChB
	        vIO32WriteFldAlign(VSRC_07, 0x00, VSRC_INV_AIDX);	//[SWAP AIDX] could be "00" or "11"
	        vIO32WriteFldAlign(VSRC_07, 0x01, VSRC_SWAP_AD01); //SWAP ChA and ChB, for 5381 , by hua
            vIO32WriteFldMulti(CKGEN_VPCLK_CFG,P_Fld(2,FLD_TVD_CLK_SEL)|P_Fld(0,FLD_ADC_C54M_SEL)|
                           P_Fld(0,FLD_TVD_CK54_SEL));

			vIO32WriteFldAlign(DFE_03, 0x08, AGC2_MODE);
			/*************************/

			vDrvSVdoDisable();							//SVDO Disable
			//vDrvAvByp();								//Set CVBS Bypass
			vIO32WriteFldAlign(VSRC_07, SV_OFF, ATD_ENABLE);	//disable ATD path for TVD
			#ifdef CC_SUPPORT_SIF_BYPASS
				vDrvSetSIFADC();
		  #endif
			break;

		case VSS_CVBS:
			//P_C0~P_C3, Composite signal, set Ch B PGA Manual gain to minimal to prevent signal out of range
			vTvd3dATDInit(SV_OFF);
			vDrvCvbsVfePWON();
			//vIO32WriteFldAlign(VFE_20, 2, RG_GMUXSEL);////mt5382p, but 5391 no ,ground mux selection
			vIO32WriteFldAlign(VSRC_01, 2, VSRC_MUX1);	//Set ChB Gain code from HDTV_04
            #ifdef CC_SUPPORT_PIPELINE
            if(VSS_MAJOR(_fVFEAVDSourceMainNew) == VSS_SCART) //set gain of CHB from TVD when SCART
            #else
            if((_bMainICIn == P_FB0) || (_bSubICIn == P_FB0) || (_bMainICIn == P_FB1) || (_bSubICIn == P_FB1)) //set gain of CHB from TVD when SCART
            #endif
            
			{
                vIO32WriteFldAlign(VSRC_01, 0, VSRC_MUX1);	//Set ChB Gain code from TVD
				//vIO32WriteFldAlign(HDTV_04, 0x0, HDTV_AGC);	//gain=1.0x
				//LogS("Enter P_FB");
			}
			else
			{
				//vIO32WriteFldAlign(HDTV_04, 0x1F, HDTV_AGC);	//gain=1.5x
				vIO32WriteFldAlign(HDTV_04, 0x11, HDTV_AGC);	//Set ChB gain="10001"==>1.496x;0418_007
			}
			//vIO32WriteFldAlign(HDTV_04, 0x11, HDTV_AGC);	//Set ChB gain="10001"==>1.496x;0418_007
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0x20, RG_CLAMP_SOURCEB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_CLAMP_SINK_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_CLAMP_SOURCE_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
			vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHB_MIDDLE_EN);//
			//vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELB);//
			//vIO32WriteFldAlign(VFE_E1, 0, RG_UPDNB);//
			vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_SHIFT2B_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFB);//
            #if SUPPORT_SCART
            
#ifdef CC_SUPPORT_PIPELINE
            if((VSS_MAJOR((_fVFEAVDSourceMainNew) == VSS_SCART))&&(bINT_BST_DET == 1)) //set gain of CHB from TVD when SCART
#else
            if (((_bMainICIn == P_FB0) || (_bSubICIn == P_FB0) || (_bMainICIn == P_FB1) || (_bSubICIn == P_FB1))  && (bINT_BST_DET == 1)) //internal path
#endif
			{
			    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SINK_ENB);//
			    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SOURCE_ENB);//
                vIO32WriteFldAlign(REG_CVBS_CFG3, 1,  RG_SHIFTB_PWD);//
                //LogS("bINT_BST_DET==1 and TVD_BST at P_FB");
            }
            #endif

			// Printf(">>>>>>enter set cvbs ADC\n");
			/*************************/
			//setting of ADC, ADC clock and TVD clock;0523_2007
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//

            //SWAP ChA/ChB
	        vIO32WriteFldAlign(VSRC_07, 0x00, VSRC_INV_AIDX);	//[SWAP AIDX] could be "00" or "11"
	        vIO32WriteFldAlign(VSRC_07, 0x01, VSRC_SWAP_AD01); //SWAP ChA and ChB, for 5381 , by hua
            vIO32WriteFldMulti(CKGEN_VPCLK_CFG,P_Fld(2,FLD_TVD_CLK_SEL)|P_Fld(0,FLD_ADC_C54M_SEL)|
                            P_Fld(0,FLD_TVD_CK54_SEL));

			vIO32WriteFldAlign(DFE_03, 0x08, AGC2_MODE);
			/*************************/

			vDrvSVdoDisable();							//SVDO Disable
			//vDrvAvByp();								//Set CVBS Bypass
			vIO32WriteFldAlign(VSRC_07, SV_OFF, ATD_ENABLE);	//disable ATD path for TVD
			break;

		case VSS_SV:
			//P_S0~P_S2, S-Video
			vTvd3dATDInit(SV_OFF);
			vDrvCvbsVfePWON();
#ifdef CC_SUPPORT_PIPELINE
			if((_fVFEAVDMainICPin == P_FB0) || (_fVFEAVDSubICPin == P_FB0))
#else
            if((_bMainICIn==P_FB0)||(_bSubICIn==P_FB0)) //kal cheng for MT5335 SCART Y-C
#endif
            {
                vIO32WriteFldAlign(VSRC_01, 2, VSRC_MUX1);  //select AGC0B ~ from HDTV
                vIO32WriteFldAlign(HDTV_04, 8, HDTV_AGC); //Set PGA Manual gain
            }
            else
            {
		        vIO32WriteFldAlign(VSRC_01, 0, VSRC_MUX1);	//set AGC from TVD3D for chroma;0418_2007
			    //vIO32WriteFldAlign(HDTV_04, 8, HDTV_AGC);	//Set PGA Manual gain = 1.5 to prevent chroma cutoff
            }
	  		/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 9, RG_CLAMP_SOURCEB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SINK_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SOURCE_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CTCLAMP_CTMB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
			vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_CHB_MIDDLE_EN);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 1, RG_HPCLAMPB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CLAMPC_TSTB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 7, RG_CALCLAMPB);//
			//vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_VAGSELB);//
			//vIO32WriteFldAlign(VFE_E1, 0, RG_UPDNB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 5, RG_CLAMP_SINKB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BUFB_PWD);//
			vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_SHIFTB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_SHIFT2B_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFB);//
		      /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_AAFB_PWD);//
			vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGABUFPB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTCLAMPB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTBUFB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGAB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTINB);//
			vIO32WriteFldAlign(REG_CVBS_CFG1, 0, RG_OFFCURB);//
			//Printf(">>>>>>enter set SV ADC\n");
			/*************************/
			//setting of ADC, ADC clock and TVD clock;0523_2007
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
		    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//
            //SWAP ChA/ChB
	        vIO32WriteFldAlign(VSRC_07, 0x00, VSRC_INV_AIDX);	//[SWAP AIDX] could be "00" or "11"
	        vIO32WriteFldAlign(VSRC_07, 0x01, VSRC_SWAP_AD01); //SWAP ChA and ChB, for 5381 , by hua
            vIO32WriteFldMulti(CKGEN_VPCLK_CFG,P_Fld(2,FLD_TVD_CLK_SEL)|P_Fld(0,FLD_ADC_C54M_SEL)|
                            P_Fld(0,FLD_TVD_CK54_SEL));

			vIO32WriteFldAlign(DFE_03, 0x08, AGC2_MODE);
			/*************************/
			vDrvSVdoEnable();							//Enable SVDO
			//vDrvSVdoByp();								//Enable SVDO bypass
			vIO32WriteFldAlign(VSRC_07, SV_OFF, ATD_ENABLE);	//disable ATD path for TVD
			break;

		case VSS_SCART:
			//SCART Setting
			vTvd3dATDInit(SV_OFF);
			vDrvCvbsVfePWON();
			/*************************/
			//setting of ADC, ADC clock and TVD clock;0523_2007
			//Clock setting
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0x20, RG_CLAMP_SOURCEB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SINK_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_CLAMP_SOURCE_ENB);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0, RG_CALPGAB);//
			vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHB_MIDDLE_EN);//
			//vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELB);//
			//vIO32WriteFldAlign(VFE_E1, 0, RG_UPDNB);//
			vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTB_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_SHIFT2B_PWD);//
			/*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BP_PGABUFB);//
            vIO32WriteFldMulti(CKGEN_VPCLK_CFG,P_Fld(2,FLD_TVD_CLK_SEL)|P_Fld(0,FLD_ADC_C54M_SEL)|
                     P_Fld(0,FLD_TVD_CK54_SEL));

            vIO32WriteFldAlign(DFE_03, 0x08, AGC2_MODE);
			/*************************/

			vIO32WriteFldAlign(VSRC_07, SV_OFF, ATD_ENABLE);	//disable ATD path for TVD
			break;

		default:
			assert(0);
	}
}

#if CLI_CVBS
void vDrvCVBSPGACheck(void)
{//FLUKE white pattern video 50%
    UINT8 i;
    UINT16 wPY_LV;

    vIO32WriteFldAlign(DFE_03, 0x0E, AGC2_MODE); // A/D code fix
    vIO32WriteFldAlign(DFE_03, 0x07, AGC2_PK_MODE); // no peak limit

    for(i=0;i<=0x78;i+=8)
    {
        vIO32WriteFldAlign(DFE_07, i, AGC2_MANUAL_ACODE); // set A code
        vUtDelay1ms(100);
        wPY_LV = IO32ReadFldAlign(DFE_STA_03, PY_LV);
        //Printf("CVBS PGA Check acode=%2x, wPY_LV = %4d\n", i, wPY_LV );
        Printf(" \n");
        Printf("CVBS PGA Check acode= %2x, wPY_LV = %3d\n", i, wPY_LV );

    }

    vIO32WriteFldAlign(DFE_07, 0x7f, AGC2_MANUAL_ACODE); // set A code
    vUtDelay1ms(1);
    wPY_LV = IO32ReadFldAlign(DFE_STA_03, PY_LV);
    //Printf("CVBS PGA Check acode=7f, wPY_LV = %4d\n", wPY_LV);
    Printf("CVBS PGA Check acode= 7f, wPY_LV = %3d\n", wPY_LV);

    vIO32WriteFldAlign(DFE_03, 0x08, AGC2_MODE);
    vIO32WriteFldAlign(DFE_03, 0x0F, AGC2_PK_MODE);

}

void vDrvTargetBlankCheck(void)
{//FLUKE white pattern pure AV

    UINT16 wTargetBlank;
    UINT16 wBlank_LV;

    for(wTargetBlank=0;wTargetBlank<=1023;wTargetBlank++)
    {
        vIO32WriteFldAlign(DFE_0A, wTargetBlank, CLAMP_TARGET_BLANK_LV); // set target blank
        vUtDelay1ms(50);
        wBlank_LV = IO32ReadFldAlign(DFE_STA_01, BLANK_LV);
        //Printf("TBCheck wTB= %4d,wBL= %4d\n", wTargetBlank, wBlank_LV );
        Printf(" \n");
        Printf("TBCheck wTB= %4d ,wBL= %4d\n", wTargetBlank, wBlank_LV );
    }
    wBlank_LV = 256;
    vIO32WriteFldAlign(DFE_0A, wBlank_LV, CLAMP_TARGET_BLANK_LV);
}

void vDrvIOOffsetBlankCheck(void)
{//FLUKE white pattern pure AV
    UINT8 bCALCLAMP,bOFFCUR;

    for(bCALCLAMP=0;bCALCLAMP<=0x0f;bCALCLAMP++)
        for(bOFFCUR=0;bOFFCUR<=0x1f;bOFFCUR++)
        {
            /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, bCALCLAMP, RG_CALCLAMPA);//
            vIO32WriteFldAlign(REG_CVBS_CFG1, bOFFCUR, RG_OFFCURA);//
            vUtDelay1ms(50);
            //Printf("IOOBCheck I= %2d, J= %2d, wBL= %4d\n", bCALCLAMP,bOFFCUR, IO32ReadFldAlign(DFE_STA_01, BLANK_LV) );
            Printf(" \n");
            Printf("IOOBCheck I= %2d ,J= %2d ,wBL= %4d\n", bCALCLAMP,bOFFCUR, IO32ReadFldAlign(DFE_STA_01, BLANK_LV) );
        }
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, 0x07, RG_CALCLAMPA);// //default value
    vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_OFFCURA);//     //default value
}

void vDrvPeakLimitCheck(UINT8 PYC)
{//FLUKE white pattern pure AV

    UINT16 PYLIM;
    UINT16 PYCLIM;
    UINT8 BYCLIM;

    switch(PYC)
    {
        case 1://Peak Y
            vIO32WriteFldAlign(DFE_03, 0x0C, AGC2_PK_MODE);
            for(PYLIM=0x0f; PYLIM<0x1000;PYLIM+=16)
            {
                vIO32WriteFldAlign(DFE_04, PYLIM, AGC2_PY_LIM);
                vUtDelay1ms(50);
                //Printf("PKCheck PYLIM= %4d, PYLV= %4d\n", PYLIM, IO32ReadFldAlign(DFE_STA_03, PY_LV));
                Printf(" \n");
                Printf("PKCheck PYLIM= %4d ,PYLV= %4d\n", PYLIM, IO32ReadFldAlign(DFE_STA_03, PY_LV));
            }
            vIO32WriteFldAlign(DFE_04, 0x890, AGC2_PY_LIM);//recovery
        break;


        case 2://Peak YC
            vIO32WriteFldAlign(DFE_03, 0x0a, AGC2_PK_MODE);
            for(PYCLIM=0x00; PYCLIM<0x1000;PYCLIM+=16)
            {
                vIO32WriteFldAlign(DFE_04, PYCLIM, AGC2_PYC_LIM);
                vUtDelay1ms(50);
                //Printf("PKCheck PYCLIM= %4d, PYCLV= %4d\n", PYCLIM, IO32ReadFldAlign(DFE_STA_03, PYC_LV));
                Printf(" \n");
                Printf("PKCheck PYCLIM= %4d ,PYCLV= %4d\n", PYCLIM, IO32ReadFldAlign(DFE_STA_03, PYC_LV));
            }
            vIO32WriteFldAlign(DFE_04, 0xF80, AGC2_PYC_LIM);//recovery
        break;


        case 3://Bottom YC
                vIO32WriteFldAlign(DFE_03, 0x09, AGC2_PK_MODE);
                for(BYCLIM=0x00; BYCLIM<8;BYCLIM++)
                {
                    vIO32WriteFldAlign(DFE_04, BYCLIM, AGC2_BYC_LIM);
                    vUtDelay1ms(50);
                    //Printf("PKCheck BYCLIM= %4d, BYCLV= %4d\n", BYCLIM, IO32ReadFldAlign(DFE_STA_02, BYC_LV));
                    Printf(" \n");
                    Printf("PKCheck BYCLIM= %4d ,BYCLV= %4d\n", BYCLIM, IO32ReadFldAlign(DFE_STA_02, BYC_LV));
                }
                vIO32WriteFldAlign(DFE_04, 0x05, AGC2_BYC_LIM);//recovery
        break;


        default:
        break;

    }
    vIO32WriteFldAlign(DFE_03, 0x0F, AGC2_PK_MODE);//recovery

}

void vDrvSVFTest(void)
{//plug AV and SV
    UINT8 bVpres2, bVpres3;

    //接上AV1, SV1端子,並送CVBS signal
    //0x20022790 Bit[0] = 1 (SV.SY = SY0?ChB)
    vIO32WriteFldAlign(REG_CVBS_CFG0,(IO32ReadFldAlign(REG_CVBS_CFG0, RG_AISELR)|0x001), RG_AISELR);//

    //0x20022780 Bit[16] = 1(CT mode)
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0,1, RG_CLAMP_SOURCE_ENB);//

    //0x20022784 Bit[21] = 0(CT mode)
    //vIO32WriteFldAlign(VFE_E1,0, RG_UPDNB);//

    //Set  0x 20022638 Bit[31] = "1"
    vIO32WriteFldAlign(SVF_00,1, SVF_EN);

    //Set  0x20022638 Bit[30] = "0" [Vpres 2]
    vIO32WriteFldAlign(SVF_00,0, SVF_VPRES_SEL);
    vUtDelay1ms(500);

    //Read 0x20022080 Bit[13] ==> 需為"1"
    bVpres2 = IO32ReadFldAlign(STA_CDET_00, VPRES_SVF);

    //Set  0x20022638 Bit[30] = "1" [Vpres 3]
    vIO32WriteFldAlign(SVF_00,1, SVF_VPRES_SEL);
    vUtDelay1ms(500);

    //Read：0x20022080 Bit[13] ==> 需為"1"
    bVpres3 = IO32ReadFldAlign(STA_CDET_00, VPRES_SVF);

    Printf("vDrvSVFTest bVpres2 = %x, bVpres3 = %x\n", bVpres2, bVpres3);

}

#endif

/**
 * @brief power down VGA
 *
 * @param None
 */
void vDrvVGAPD(void)
{
	vIO32WriteFldAlign(PDWNC_VGACFG0, 0x1, FLD_RG_VMUX_PWD);
	//vIO32WriteFldAlign(PDWNC_VGACFG6, 0x1, FLD_RG_SOGY_ADC_PWD);
	vIO32WriteFldAlign(PDWNC_VGACFG0, 0x0, FLD_RG_AUTOBIAS_EN);//20100415, sc hwu suggestion
	vIO32WriteFldAlign(PDWNC_PDMISC, 0x1, FLD_VGA_SYNC_DIS);//20100415, chiahsien liu suggestion
}

/**
 * @brief power on VGA
 *
 * @param None
 */
void vDrvVGAPWON(void)
{

	vIO32WriteFldAlign(PDWNC_VGACFG0, 0x0, FLD_RG_VMUX_PWD);
	//vIO32WriteFldAlign(PDWNC_VGACFG6, 0x0, FLD_RG_SOGY_ADC_PWD);
	vIO32WriteFldAlign(PDWNC_VGACFG0, 0x1, FLD_RG_AUTOBIAS_EN);//20100415, sc hwu suggestion
	vIO32WriteFldAlign(PDWNC_PDMISC, 0x0, FLD_VGA_SYNC_DIS);//20100415, chiahsien liu suggestion
}

/**
 * @brief Set SIF ADC for tuner bypass
 *
 * @param None
 */
#ifdef CC_SUPPORT_SIF_BYPASS
BOOL vDrvSetSIFADC(void)
{
    UINT32 CHA =0 ;
    UINT32 i =0 ;
    BOOL bUseChD;

	for(i=0;i<portmap_table_num; i++)
	{
		if((portmap_table[i].vs_src==SV_VS_ATV1))
		{
			CHA=(portmap_table[i].av_mux&0x0F);
			CHA=dAvMuxMask(CHA,0);
      break;
		}
	  }
	  if ((IO32ReadFldAlign(REG_CVBS_CFG0, Fld(32, 0, AC_FULLDW))&CHA)>=1)
	{
          vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_PGABUFNB_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG3, 1,RG_SHIFTB_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG3, 1,RG_OFFCURB_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBS108M_EN);//disable 108M to VGA.Scart SV C use CVBS CHB
          bUseChD = FALSE;
	}
	else
	{
  //        if(CHA > 0xFFFFFF)
          	vIO32WriteFldAlign(REG_CVBS_CFG0, (CHA>>24), RG_AISELR);//write to 23:16  CHB
  //        else
  //        	vIO32WriteFldAlign(REG_CVBS_CFG4, (CHA>>16)|0x40, RG_AISEL_2);//write 31:24
          vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBS108M_EN);//enable 108M to VGA.Scart SV C use VGA AD
          vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNB_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG3, 0,RG_SHIFTB_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG3, 0,RG_OFFCURB_PWD);
          bUseChD = TRUE;
       }
	vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_SIF_TO_IFDEMO);
	vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHA_CVBS0P);
	vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHB_CVBS0P);
       return bUseChD;
}
void vScartSVInRFE(UINT8 status)
	{
		if(status)
			{
				if(IO32ReadFldAlign(REG_CVBS_CFG2, RG_CVBS108M_EN)==0)
				{
         				vIO32WriteFldAlign(SYS_06, 0, MANUAL_SCGAIN_EN);//Auto gain
          				vIO32WriteFldAlign(VSRC_09_0, 1, CVBS_SC_SEL);//enable VGA_R
         				vIO32WriteFldAlign(OMUX_VDOIN_CLK_SEL, 1, FLD_VDOIN_CLK_SEL_9);//enable CVBS2_CLK108
             				vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBS108M_EN);//enable 108M to VGA.Scart SV C use VGA AD
	     				vIO32WriteFldMulti(REG_VGA_Normal_CFG8,P_Fld(1,RG_CLKOSEL_3)|P_Fld(0,RG_CLKOSEL_2)|P_Fld(0,RG_CLKOSEL_1));
	     				vIO32WriteFldAlign(REG_VGA_Normal_CFG7,0,RG_VGAADC3_CKSEL);
             				vIO32WriteFldAlign(PDWNC_VGACFG0, 0x00, FLD_RG_SC_SCART );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG3,1,RG_CLAMP_GATE_3 );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG3,1,RG_CLAMP_MIDDLE_3 );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG0,1,RG_VDC_P_EN );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG0,1,RG_VDC_N_EN );
				}
			}
		else
			{
				if(IO32ReadFldAlign(REG_CVBS_CFG2, RG_CVBS108M_EN)==1)
				{
         				vIO32WriteFldAlign(SYS_06, 1, MANUAL_SCGAIN_EN);//disable Auto gain
          				vIO32WriteFldAlign(VSRC_09_0, 0, CVBS_SC_SEL);//disable VGA_R
         				vIO32WriteFldAlign(OMUX_VDOIN_CLK_SEL, 0, FLD_VDOIN_CLK_SEL_9);//disable CVBS2_CLK108
        				vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBS108M_EN);//enable 108M to VGA.Scart SV C use VGA AD
					vIO32WriteFldMulti(REG_VGA_Normal_CFG8,P_Fld(0,RG_CLKOSEL_3)|P_Fld(0,RG_CLKOSEL_2)|P_Fld(1,RG_CLKOSEL_1));
					vIO32WriteFldAlign(REG_VGA_Normal_CFG7,1,RG_VGAADC3_CKSEL);
             				// vIO32WriteFldAlign(PDWNC_VGACFG0, 0x01, FLD_RG_SC_SCART );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG3,0,RG_CLAMP_MIDDLE_3 );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0,RG_VDC_P_EN );
        				vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0,RG_VDC_N_EN );
				}
			}

}

#endif
// => bApiVideoMainSubSrc
/**
 * @brief power down Cvbs ADC
 *
 * @param None
 */
void vDrvCvbsAdcPD(void)
{
   #if PD_ADC_ENABLE
    #ifndef __MODEL_slt__
     /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_BG_PD);
    #endif
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_PWD_12 );//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_PWD_33);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_PWD_12);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_PWD_33);//
   #endif
}

// => bApiVideoMainSubSrc
/**
 * @brief power on Cvbs ADC
 *
 * @param None
 */
void vDrvCvbsAdcPWON(void)
{
  #if PD_ADC_ENABLE
   #ifndef __MODEL_slt__
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_BG_PD);
   #endif
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//
  #endif
}

// => bApiVideoMainSubSrc
/**
 * @brief power down Cvbs ADC
 *
 * @param None
 */
void vDrvCvbsVfePD(void)
{
   #if PD_ADC_ENABLE
 //   vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_MIX_PWD);//
 //   vIO32WriteFldAlign(REG_CVBS_CFG1, 1, RG_MIXINPUT);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_INMUX_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_PGAA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_PGABUFPA_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_PGABUFNA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_AAFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_CTCLAMPA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_BUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_CTBUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_PGAB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_PGABUFPB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_PGABUFNB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_AAFB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_CTCLAMPB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_BUFB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 1, RG_CTBUFB_PWD);//

    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_SHIFTA_PWD);    /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_SHIFTB_PWD);    /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_OFFCURA_PWD); /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_OFFCURB_PWD); /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_CVBS_PWD);       /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSPGA_PWD_12);       /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_CLAMP_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_INMUX_PWD);//
 //  #ifdef CC_SUPPORT_SIF_BYPASS
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_PGABUFNB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0,RG_SHIFTB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0,RG_OFFCURB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBS_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CLAMP_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_INMUX_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBSADC_PWD_2);
//   #else
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_PGABUFNB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1,RG_SHIFTB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1,RG_OFFCURB_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_CVBS_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_CLAMP_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_INMUX_PWD_2);
//          vIO32WriteFldAlign(REG_CVBS_CFG7, 1, RG_CVBSADC_PWD_2);
//   #endif
   #ifndef __MODEL_slt__
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 1, RG_BG_PD);
   #endif
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_PWD_12 );//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC1_PWD_33);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_PWD_12);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 1, RG_CVBSADC2_PWD_33);//
    vIO32WriteFldAlign(CKGEN_VPCLK_STOP, 1, FLD_TVD3D_STOP);
   #endif
}

// => bApiVideoMainSubSrc
/**
 * @brief power on Cvbs ADC
 *
 * @param None
 */
void vDrvCvbsVfePWON(void)
{
   #if PD_ADC_ENABLE
   // vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_MIX_PWD);
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_INMUX_PWD);//
    /*mark for V2*/ // vIO32WriteFldAlign(VFE_E2, 0, RG_PGAA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGABUFPA_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_AAFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTCLAMPA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTBUFA_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGAB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_PGABUFPB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_AAFB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTCLAMPB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_BUFB_PWD);//
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E2, 0, RG_CTBUFB_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTA_PWD);    /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTB_PWD);    /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_OFFCURA_PWD); /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_OFFCURB_PWD); /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_CVBS_PWD);       /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSPGA_PWD_12);       /*add for V2*/
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_CLAMP_PWD);//
    vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_INMUX_PWD);//

 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_PGABUFNB_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0,RG_SHIFTB_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0,RG_OFFCURB_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBS_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CLAMP_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_INMUX_PWD_2);
 //         vIO32WriteFldAlign(REG_CVBS_CFG7, 0, RG_CVBSADC_PWD_2);
   #ifndef __MODEL_slt__
    /*mark for V2*/ //vIO32WriteFldAlign(VFE_E0, 0, RG_BG_PD);
   #endif
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_12 );//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC1_PWD_33);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_12);//
    vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBSADC2_PWD_33);//
    vIO32WriteFldAlign(CKGEN_VPCLK_STOP, 0, FLD_TVD3D_STOP);
   #endif
}


//////////////////////////////////////////////////////////////////////
// Private Function
//////////////////////////////////////////////////////////////////////

// => vDrvADCDefaultSetting		=>5371, OK, defaulting setting by ping
/**
 * @brief The 3 ADCs for HDTV/VGA default setting
 *
 * necessary default settings of high speed ADCs
 */
void vDrvHDTVADCDefaultSetting(void)
{
	UINT8 bCh;
	UINT8 bTmpvalue,bTmpResult;
	#ifndef CC_MTK_LOADER
     x_os_drv_set_timestamp("vDrvHDTVADCDefaultSetting begin");
    #endif
   #if EEPROM_READY
    _bEEPROM_ready = 1;
   #else
    _bEEPROM_ready = 0;
   #endif
     if (DRVCUST_OptGet(eEFuseAutocolor))
     {
            bTmpResult=0;
            for(bCh=0; bCh<3; bCh++)
            {
            	if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>24)==0)
            	{
                 bTmpvalue=((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>(8*bCh));
                 if((bTmpvalue>=0x80) && (bTmpvalue<=0xEE))
                 {
                      bTmpResult |= (0x01 << bCh);
                 }
            }
            	else
            	{
            	   bTmpvalue=((IO32ReadFldAlign(0xf0008660, AC_FULLDW))>>(8*(bCh+1)));
                 if((bTmpvalue>=0x80) && (bTmpvalue<=0xEE))
                 {
                      bTmpResult |= (0x01 << bCh);
                 }

            	}
            }
            _bEFUSE_AUTOCOLOR_READY = ((bTmpResult & 0x07) != 0x07) ? 0 : 1;

            #ifdef __MODEL_slt__
            if(_bEFUSE_AUTOCOLOR_READY==0)
            {
              LOG(0,"==== E Fuse Gain fail(empty or wrong) !!!!  ===\n");
              CLI_Parser("pmx.p 1");
            }
            #endif
     }
     else
     {
          _bEFUSE_AUTOCOLOR_READY=0;
     }
     if (_bEEPROM_ready==1)
     {
         if(bApiEepromReadByte(EEP_ADC_AUTOCOLOR_OVER_RULE_EFUSE)==0xAA)
         {
            _bEFUSE_AUTOCOLOR_READY=0;
         }
     }
   #if EnableSDDSCalibration
    _bSDDSCaliState = 0;
   #endif
    //vIO32WriteFldMulti(PDWNC_SOGYCFG,P_Fld(6,FLD_PD_SOGY_VTL)|P_Fld(1,FLD_SELECT)|P_Fld(0,FLD_PD_SOGY_CAL)|P_Fld(9,FLD_PD_SOGY_VTH));
    vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(8,FLD_RG_SYNC1_VTL)|P_Fld(8,FLD_RG_SYNC1_VTH));
    //vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0,FLD_RG_VMUX_PWD)|P_Fld(1,FLD_RG_DESPIKE)|P_Fld(1,FLD_RG_RGB_EN));  //5360 Michael checked
    vIO32WriteFldMulti(PDWNC_VGACFG0,P_Fld(0,FLD_RG_VMUX_PWD)|P_Fld(1,FLD_RG_DESPIKE)|P_Fld(1,FLD_RG_AUTOBIAS_EN));  //Mt5365/95 kal checked
    vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld( 0, FLD_RG_SYNC_DESPK_EN)|P_Fld(0,FLD_RG_DIG_TST_EN));  //5360 Michael checked
    vIO32WriteFldMulti(PDWNC_VGACFG3,P_Fld( 0, FLD_RG_SOGY_SORS_PWD)|P_Fld(0, FLD_RG_SOGY_SINK_PWD));//power on
    vIO32WriteFldAlign(ASYNC_0E, 2, AS_C_MAXMIN_FILTER);
    //vIO32WriteFldAlign(REG_VGA_Normal_CFG0, 1, SELF_EN); //5365 mark
          	vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBS108M_EN);//disable 108M to VGA.Scart SV C use VGA AD

    vIO32WriteFldMulti(VFE_00, P_Fld(0,ABIST_VGA_DATA_SEL)|P_Fld(0,ABIST_VGA_DOWN_SAMPLE));
    vIO32WriteFldMulti(REG_VGA_Normal_CFG0, P_Fld(0,RG_VDC_N_EN )|P_Fld(0,RG_SHORT_FEO )|P_Fld(0,RG_ADCTEST_EN ));
vIO32WriteFldMulti(REG_VGA_Normal_CFG8,P_Fld(0,RG_CLKOSEL_3)|P_Fld(0,RG_CLKOSEL_2)|P_Fld(1,RG_CLKOSEL_1)|P_Fld(0,RG_RGB_REV ));
         vIO32WriteFldMulti(REG_VGA_Normal_CFG4, P_Fld(2,RG_VGAADC1_IGBIAS)|P_Fld(2,RG_VGAADC2_IGBIAS)|P_Fld(2,RG_VGAADC3_IGBIAS))
   //vUtDelay1ms(2);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 0,FLD_RG_OFFCUR);
    #ifdef CC_MT5399
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1,FLD_RG_PSAV_REV_BG);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1,FLD_RG_PSAV_REV_INPUT_PORT);
    #else
    vIO32WriteFldAlign(PDWNC_VGACFG2, 0,FLD_RG_PSAV_REV_BG);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1,FLD_RG_PSAV_REV_INPUT_PORT);
    #endif
    
	for (bCh=0; bCh<3; bCh++)
	{
		//vIO32WriteFldAlign(REG_VGA_Normal_CFG1+16*bCh, 0x00000A0A, REG_VGA_Normal_CFG1_All);   //REG_VGA_Normal_CFG1, REG_VGA_Normal_CFG5, REG_VGA_Normal_CFG9 //20090323 37.5uA->62.5uA for SAG
        vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),1,RG_CLAMP_GATE_1 );
         vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),0,RG_FE_1_PWD );
         vIO32WriteFldMulti(REG_VGA_Normal_CFG1+(4*bCh),P_Fld(1,RG_CLAMP_GATE_1 )|P_Fld(0,RG_FE_1_PWD )|P_Fld(3,RG_COPBIAS_1 )|P_Fld(0,RG_COP_1_PWD ));
	 vIO32WriteFldMulti(REG_VGA_Normal_CFG5+(4*bCh),P_Fld(1,RG_VGAADC1_CKSEL )|P_Fld(0,RG_VGAADC1_DIV_SEL)
                                                                              |P_Fld(0,RG_VGAADC1_CORE_PWD)|P_Fld(0,RG_VGAADC1_CK_PWD )
                                                                              |P_Fld(0,RG_VGAADC1_PHSEL )|P_Fld(1,RG_VGAADC1_DC_EN )
                                                                              |P_Fld(0,RG_VGAADC1_VSEL_EN )|P_Fld(0,RG_VGAADC1_VSEL )
                                                                              |P_Fld(1,RG_VGAADC1_LSF_EN )|P_Fld(0,RG_VGAADC1_REV0 )
                                                                              |P_Fld(255,RG_VGAADC1_REV1 ));

	}

   // vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, TRGSEL);
vIO32WriteFldMulti(REG_VGA_Normal_CFG4,P_Fld(0,RG_VGAADC_MON_SEL)|P_Fld(0,RG_VGAADC1_IO_PWD)|P_Fld(0,RG_VGAADC2_IO_PWD)|P_Fld(0,RG_VGAADC3_IO_PWD));
    vIO32WriteFldMulti(REG_VGA_Normal_CFG0, P_Fld(0,RG_VREFGEN4FE_PWD )|P_Fld(0,RG_VDC_P_EN )|P_Fld(0,RG_RESSEL )
    	                                                                                                               |P_Fld(7,RG_CAP_EN )|P_Fld(1,RG_FE_OFFSET_N ));


     	vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0x04,RG_ADCVREFP );
   #if Phase3Channel
    vIO32WriteFldMulti(REG_VGA_Normal_CFG8, P_Fld(1,RG_CLKINV_EN )|P_Fld(1,RG_RELATCH_EN ));
   #else
    vIO32WriteFldMulti(REG_VGA_Normal_CFG8, P_Fld(0,RG_CLKINV_EN )|P_Fld(0,RG_RELATCH_EN ));
   #endif

   #if 1
    // Slove VGA ADC no data
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, CLR);vUtDelay1ms(1);
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 1, CLR);vUtDelay1ms(1);
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, CLR);vUtDelay1ms(1);
    // Slove VGA ADC no data
   #endif


 //   vIO32WriteFldAlign(HDFE_00, 0x07, CLAMP_MID_EN);

    vDrvClkInit(); //init for adcpll clk

    for(bCh=0; bCh<4; bCh++ )   // to clear the control variables of loading PGA gain and offset initial values.
    {
        _bPGAGainOffsetHistory[bCh]=0;
        _bOffsetCaliDone[bCh]=0;
    }

#ifndef CC_MTK_LOADER
     x_os_drv_set_timestamp("vDrvHDTVADCDefaultSetting end");
#endif
}

//CKGEN_WRITE32(0x150, (CKGEN_READ32(0x150) & 0xFFFFFFEF));// power on ADCPLL2
//vIO32WriteFldAlign(CKGEN_ADCPLLCFG0, 0, FLD_ADCPLL2_PWD); // 5360 NEED TO CHECK

//-- Michael : ------------------------------------------------------------------------------------------
//--    1. Set the default gain and offset value to EEPROM if the system never do any gain and offset calibration --------
//--    2. These values can be used for vDrvSetHDTVADC()                                      --------
//--    3. We can save 4 EEPROM location and simplify the program flow of  the vDrvSetHDTVADC --------
//--    4. The 4 EEPROM locations are                                                         --------
//--        EEP_VIDEO_AUTO_COLOR_VGA_DONE                                                     --------
//--        EEP_VIDEO_AUTO_COLOR_HDTV_DONE                                                    --------
//--        EEP_VIDEO_AUTO_COLOR_SCART_DONE                                                   --------
//--        EEP_VIDEO_AUTO_COLOR_VGA_COMP_DONE                                                --------
//----------------------------------------------------------------------------------------------------

PRIVATE void vDrvEFUSEAutoColorGainSetToNextType(UINT8 bOrgType, UINT8 bNextType)
{
    UINT8 bOrder;
    UINT16 wDen,wNum;
    UINT32 dGain,bGain;

    for(bOrder=0; bOrder<3; bOrder++)
    {
        dGain = _bAutoColorGain[bOrgType][bOrder];
        if(DRVCUST_OptGet(eVFE_HW_075ohm))
       {
        wNum  = wOnChipColorGainTable_75[bOrgType][bOrder];
        wDen  = wOnChipColorGainTable_75[bNextType][bOrder];
       }
        else
        {
        wNum  = wOnChipColorGainTable[bOrgType][bOrder];
        wDen  = wOnChipColorGainTable[bNextType][bOrder];
        }
      //  dGain = ((0xff + dGain)* wDen) - (0xff * wNum);
     //   bGain = (dGain + (wNum>>1)) /wNum ;  //round off
        bGain = dGain *wDen/wNum;
        _bAutoColorGain[bNextType][bOrder] = bGain;
    }
}


void vSetPGAGainOffsetInitVal(UINT8 bType)
{
    UINT8 bCh,i,j;
  #if EEPROM_READY
   #if EEPROM_USAGE_debug
    HAL_TIME_T rHDTVADCTime;
    HAL_GetTime(&rHDTVADCTime);
    DBG_Printf(VGA_Debug,"vDrvHDTVADCDefaultSetting start time=%d  %d\n",rHDTVADCTime.u4Seconds, rHDTVADCTime.u4Micros);
   #endif
  #endif

#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif

    if(_bPGAGainOffsetHistory[bType]==0) //This means the gain and offset were never given values.
    {
        DBG_Printf(VGA_Debug,"To set the PGA gain and offset \n");
        _bPGAGainOffsetHistory[bType]=1;
        if (_bEFUSE_AUTOCOLOR_READY==1)//kalcheng   eEFuseAutocolor
        {
            _bAutoColorHistory[bType] = SV_TRUE;
			DBG_Printf(VGA_Debug,"===use efuse autocolor\n");
            //////////////////////////////  1. Read Offset From code table   ////////////////////////////////////////////
            for(bCh=0; bCh<3; bCh++)
            {
            #ifdef AUTOCOLOR_NEW_METHOD
                _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_100_DIGITAL[bType][bCh];
                _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh];
          //      _bDigSignBit = (bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh]>>8) & 0xff;
           //     _bInitDigSignBit=_bDigSignBit;
         //	vIO32WriteFldAlign(HDFE_00, _bDigSignBit, SIGN_OFFSET1*bCh);
         //	vIO32WriteFldAlign(HDFE_00, 0, SIGN_OFFSET1*bCh);
            #else
                if(DRVCUST_OptGet(eVFE_HW_075ohm))
                {
                    _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_75_100[bType][bCh];
                }
                else
                {
                    if(DRVCUST_OptGet(eYPBPR_ADC_SUPPORT_120))
                    {
                        _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_120[bType][bCh];
                    }
                    else
                    {
                        _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_100[bType][bCh];
                    }
                }
                #endif
            }
            //////////////////////////////  2. Read Gain From E Fuse   ///////////////////////////////////////////////

            for(bCh=0; bCh<3; bCh++)
            {  //compel Read VGA E Fuse address 0xf0008674

               if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>24)==0)
            	{
                _bAutoColorGain[1][bCh] = ((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>(8*bCh))<<8;//<<8 change to 16bit //+bEFuseCompensation[bCh];
            	}
		else
            	{
                _bAutoColorGain[1][bCh] = ((IO32ReadFldAlign(0xf0008660, AC_FULLDW))>>(8*(bCh+1)))<<8;//<<8 change to 16bit  //+bEFuseCompensation[bCh];
            	}
                _bAutoColorGain[3][bCh] = _bAutoColorGain[1][bCh]; //kaltst add
            }

            _bcurrent_use_adc=1;  //efuese
            /////////////////////////////   3. Transfer to  YPbPr, SCART, VGA_YPbPr  ///////////////////////////////////

            if(DRVCUST_OptGet(eVFE_HW_SPECIAL_TARGET))
            {
                for(i=0;i<4;i++)
                    for(j=0;j<3;j++)
                    {
                            if(DRVCUST_OptGet(eVFE_HW_075ohm))
                        wOnChipColorGainTable_Temp[i][j] = wOnChipColorGainTable_75[i][j];
                            else
                        wOnChipColorGainTable_Temp[i][j] = wOnChipColorGainTable[i][j];
   	                }
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA_COMPOENT,INT_VGA);//_bType 3 maps to 1
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA,INT_HDTV);//_bType 1 maps to 0
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA,INT_SCART);//_bType 1 maps to 2
            }
            else
            {
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_HDTV);
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_SCART);
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_VGA_COMPOENT);
            }
            //////////////////////////////  4. add compensation gain ///////////////////////////////////////////////
            //for CLI command n.vga.ebgain 0 or 1 read; kal add
            for(i=0;i<4;i++)
   	            for(j=0;j<3;j++)
                {
                   _bAutoColorGain[i][j] = _bAutoColorGain[i][j] + bEFuseCompensation[i][j];
                   _bAutoColorGain_for_EFuse[i][j] = _bAutoColorGain[i][j];
   	            }
            /////////////////////////////   5. Check Embedded gain  ///////////////////////////////////

            #ifdef __MODEL_slt__
			if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))& 0x00FFFFFF)==0x00000000)
		    {
		        vDrvSetOSTGOutPattern(1);
		   	    DBG_Printf(VGA_Debug,"==== E Fuse Auto color fail(empty) !!!!  ===\n");
		    }
            #endif
    }
    else if (_bEEPROM_ready==1)
    {
       #if EEPROM_USAGE_debug
        DBG_Printf(VGA_Debug,"=== EEPROM is READY, _bEEPROM_ready=%u\n",_bEEPROM_ready );
       #endif
        if (bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_HDTV_DONE+bType) != SV_TRUE) //never do auto color
        {
          _bAutoColorHistory[bType] = SV_FALSE;
            #if EEPROM_USAGE_debug
            DBG_Printf(VGA_Debug,"=== EEPROM is not done, Type=%u, _bAutoColorHistory=%u\n",bType,_bAutoColorHistory[bType] );
            #endif
            for(bCh=0; bCh<3; bCh++)
            {
            #ifdef AUTOCOLOR_NEW_METHOD
                bTmpHighByte = (bVGADefaultGainTABLE_100_DIGITAL[bType][bCh]>>8) & 0xff;
                bTmpLowByte =  bVGADefaultGainTABLE_100_DIGITAL[bType][bCh] & 0xff;
                             if(bType==INT_SCART)
                            	{
                 UNUSED(fgApiEepromWriteByte((EEP_ADC_DIG_GAIN_SCART0_LOW_BYTE + bCh) , bTmpLowByte));
                 UNUSED(fgApiEepromWriteByte((EEP_ADC_DIG_GAIN_SCART0_HIGH_BYTE +bCh) , bTmpHighByte));

                            	}
                            else
                            	{
              UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV0_LOW_BYTE + (bType*6)+(2*bCh), bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV0_HIGH_BYTE + (bType*6)+(2*bCh), bTmpHighByte));
                            	}

                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_START + (bType*3)+bCh, bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh]));

                _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_100_DIGITAL[bType][bCh];
                _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh];
         //       _bDigSignBit = (bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh]>>8) & 0xff;
         //        _bInitDigSignBit=_bDigSignBit;
        //	vIO32WriteFldAlign(HDFE_00, _bDigSignBit, SIGN_OFFSET1*bCh);
        // 	vIO32WriteFldAlign(HDFE_00, 0, SIGN_OFFSET1*bCh);
            #else
                if(DRVCUST_OptGet(eVFE_HW_075ohm))
                {
                    UNUSED(fgApiEepromWriteByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh), bVGADefaultGainTABLE_75_100[bType][bCh]));
                    UNUSED(fgApiEepromWriteByte((EEP_ADC_OFFSET_START + (bType*3) + bCh), bVGADefaultOffsetTABLE_75_100[bType][bCh]));
                    _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_75_100[bType][bCh];
                    _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_75_100[bType][bCh];
                }
                else
                {
                    if(DRVCUST_OptGet(eYPBPR_ADC_SUPPORT_120))
                    {
                        UNUSED(fgApiEepromWriteByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh), bVGADefaultGainTABLE_120[bType][bCh]));
                        UNUSED(fgApiEepromWriteByte((EEP_ADC_OFFSET_START + (bType*3) + bCh), bVGADefaultOffsetTABLE_120[bType][bCh]));
                        _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_120[bType][bCh];
                        _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_120[bType][bCh];
                    }
                    else
                    {
                        UNUSED(fgApiEepromWriteByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh), bVGADefaultGainTABLE_100[bType][bCh]));
                        UNUSED(fgApiEepromWriteByte((EEP_ADC_OFFSET_START + (bType*3) + bCh), bVGADefaultOffsetTABLE_100[bType][bCh]));
                        _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_100[bType][bCh];
                        _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_100[bType][bCh];
                    }
                }
            #endif
            }
            _bcurrent_use_adc=3; //Default
        }
        else
        {
           _bAutoColorHistory[bType] = SV_TRUE;
           #if EEPROM_USAGE_debug
            DBG_Printf(VGA_Debug,"=== EEPROM is done, Type=%u, _bAutoColorHistory=%u\n",bType,_bAutoColorHistory[bType] );
           #endif
        #ifdef AUTOCOLOR_NEW_METHOD
            for(bCh=0; bCh<3; bCh++)
            {
                            	if(bType==INT_SCART)
                            	{
                 				bTmpLowByte =  bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART0_LOW_BYTE + bCh) ;
                 				bTmpHighByte=  bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART0_HIGH_BYTE +bCh);

                            	}
                            else
                            	{
                bTmpLowByte= bApiEepromReadByte((EEP_ADC_DIG_GAIN_HDTV0_LOW_BYTE + (bType*6) + (2*bCh)));
                bTmpHighByte= bApiEepromReadByte((EEP_ADC_DIG_GAIN_HDTV0_HIGH_BYTE + (bType*6) + (2*bCh)));
                            	}
                _bAutoColorGain[bType][bCh]=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;

                bTmpHighByte= bApiEepromReadByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh));
                bTmpLowByte= bApiEepromReadByte((EEP_ADC_OFFSET_START + (bType*3) + bCh));
                _bAutoColorOffset[bType][bCh]= (((UINT16)bTmpHighByte<<8)&0xFF00) |(UINT16) bTmpLowByte;
            }
        #else
            for(bCh=0; bCh<3; bCh++)
            {
              _bAutoColorGain[bType][bCh]= bApiEepromReadByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh));
                _bAutoColorOffset[bType][bCh]= bApiEepromReadByte((EEP_ADC_OFFSET_START + (bType*3) + bCh));
            }
        #endif
             _bcurrent_use_adc=2;  //eeprom
        }
    }
    else
    {

       DBG_Printf(VGA_Debug,"=== EEPROM is NOT READY, _bEEPROM_ready=%u\n",_bEEPROM_ready );

        for(bCh=0; bCh<3; bCh++)
        {
        #ifdef AUTOCOLOR_NEW_METHOD
                 bTmpHighByte = (bVGADefaultGainTABLE_100_DIGITAL[bType][bCh]>>8) & 0xff;
          _bAutoColorGain[bType][bCh] = bVGADefaultGainTABLE_100_DIGITAL[bType][bCh];
            _bAutoColorOffset[bType][bCh]= bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh];
         //       _bDigSignBit = (bVGADefaultOffsetTABLE_100_DIGITAL[bType][bCh]>>8) & 0xff;
         //        _bInitDigSignBit=_bDigSignBit;
        //	vIO32WriteFldAlign(HDFE_00, _bDigSignBit, SIGN_OFFSET1*bCh);
       // 	vIO32WriteFldAlign(HDFE_00, 0, SIGN_OFFSET1*bCh);
        #else
            if(DRVCUST_OptGet(eVFE_HW_075ohm))
            {
                _bAutoColorGain[bType][bCh]    = bVGADefaultGainTABLE_75_100[bType][bCh];
                _bAutoColorOffset[bType][bCh]  = bVGADefaultOffsetTABLE_75_100[bType][bCh];
            }
            else
            {
                if(DRVCUST_OptGet(eYPBPR_ADC_SUPPORT_120))
                {
                    _bAutoColorGain[bType][bCh]    = bVGADefaultGainTABLE_120[bType][bCh];
                    _bAutoColorOffset[bType][bCh]  = bVGADefaultOffsetTABLE_120[bType][bCh];
                }
                else
                {
                    _bAutoColorGain[bType][bCh]    = bVGADefaultGainTABLE_100[bType][bCh];
                    _bAutoColorOffset[bType][bCh]  = bVGADefaultOffsetTABLE_100[bType][bCh];
                }
            }
        #endif
        }
            _bcurrent_use_adc=3; //Default
        _bAutoColorHistory[bType] = SV_FALSE;
    }

      #if EEPROM_READY
       #if EEPROM_USAGE_debug
        HAL_GetTime(&rHDTVADCTime);
        DBG_Printf(VGA_Debug,"vDrvHDTVADCDefaultSetting end time=%d  %d\n",rHDTVADCTime.u4Seconds, rHDTVADCTime.u4Micros);

        UINT8 bTmpEEPROMdata[3];
        for(bCh=0; bCh<3; bCh++)
        {
            bTmpEEPROMdata[bCh] = bApiEepromReadByte((EEP_VIDEO_AUTO_COLOR_START + (bType*3) + bCh));
        }
        DBG_Printf(VGA_Debug,"after Type=%u  ,Gain1  =0x%x ,Gain2  =0x%x ,Gain3  =0x%x \n",bType,bTmpEEPROMdata[0],bTmpEEPROMdata[1],bTmpEEPROMdata[2]);
       #endif
      #endif
    }
    else
    {
        DBG_Printf(VGA_Debug,"The PGA gain and offset were given\n");
    }

    #if EEPROM_USAGE_debug
    DBG_Printf(VGA_Debug,"Type   =%u, autocolor ready=%u\n",bType,_bAutoColorHistory[bType]);
    DBG_Printf(VGA_Debug,"Gain1  =0x%x ,Gain2  =0x%x ,Gain3  =0x%x \n",_bAutoColorGain[bType][0],_bAutoColorGain[bType][1],_bAutoColorGain[bType][2]);
    DBG_Printf(VGA_Debug,"Offset1=0x%x ,Offset2=0x%x ,Offset3=0x%x \n",_bAutoColorOffset[bType][0],_bAutoColorOffset[bType][1],_bAutoColorOffset[bType][2]);
    #endif


}

BOOL MTAL_SetPGAGainOffsetInitVal(void)
{
    UINT8 bInputType,bLoopCnt;
	#ifdef CC_SUPPORT_PIPELINE
	switch (u1ADCConnentSrcPort)
	#else
    switch (_bMainICIn)    //need video path to set the PGA again 
    #endif
    {
        case P_YP0:   //HDTV
        case P_YP1:
            bInputType = 0;
            break;
        case P_VGA:
            bInputType = 1;
            break;
        case P_FB0:   //SCART0
        case P_FB1:   //SCART1
            bInputType = 2;
            break;
        case P_VGACOMP:
          	bInputType = 3;
            break;
        default:
            bInputType= 0xFF;
            break;
    }
    DBG_Printf(VGA_Debug,"SetPGAGainOffsetInitVal: input=%d\n",bInputType);
    if(bInputType != 0xFF)
    {
        _bPGAGainOffsetHistory[bInputType]=0;
        vSetPGAGainOffsetInitVal(bInputType);
        for(bLoopCnt=0; bLoopCnt<3; bLoopCnt++)
        {
     #ifdef AUTOCOLOR_NEW_METHOD
		vDrvSetHDADCDigitalOffset(bLoopCnt,_bAutoColorOffset[bInputType][bLoopCnt]);
		bDrvSetHDADCGain_Digital(bLoopCnt,_bAutoColorGain[bInputType][bLoopCnt]);
     #else
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(0x10*bLoopCnt), _bAutoColorGain[bInputType][bLoopCnt], PGAGAIN_1);      //PGAGAIN_1,PGAGAIN_2,PGAGAIN_3
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(0x10*bLoopCnt), _bAutoColorOffset[bInputType][bLoopCnt], PGAOFFSET_1);  //PGAOFFSET_1,PGAOFFSET_2,PGAOFFSET_3
     #endif
         }
       return TRUE;
    }
    else
        return FALSE;
}
//////////////////////////////////////////////////////////////////////
// Public Function
//////////////////////////////////////////////////////////////////////


// => bApiVideoMainSubSrc
/**
 * @brief Set the HDTV ADCs setting by different input types
 *
 * Set the HDTV ADCs by different input types, like set the clamp methods, correct PGA offset and gain settings
 * @param bVDOINPUT bVDOINPUT is the input types, like VGA/SCART/HDTV
 */
void vDrvSetHDTVADC(UINT8 bVDOINPUT)
{
    UINT8 bInputType,bLoopCnt;

    vIO32WriteFldMulti(CKGEN_VPCLK_STOP,P_Fld(0,FLD_PIP_CLK_STOP)|P_Fld(0,FLD_MAIN_CLK_STOP)
                                          |P_Fld(0,FLD_VGA_CLK_STOP)|P_Fld(0,FLD_HDTV_CLK_STOP));//
    vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_CVBS_EN)|P_Fld(0,FLD_RG_BYPS_SYNCPROSR)|P_Fld(0,FLD_RG_BYPS_SOGYPGA));  // Clamp level 0xE8884C58 // 0xE8884D58
   vIO32WriteFldMulti(PDWNC_VGACFG3, P_Fld(1,FLD_RG_CLAMPREFSEL)| P_Fld(0x1B,FLD_RG_SOGY_SINK)|P_Fld(0xB,FLD_RG_SOGY_SOURCE)
   									|P_Fld(0x3,FLD_RG_SOGY_RGAIN )|P_Fld(0x3,FLD_RG_SOGY_BW ));
   	// vIO32WriteFldAlign(SyncSlice_Control_01, 0x96,VTLH_SEL);
    vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_SYNC_TSTSEL)|P_Fld(0,FLD_RG_SYNC_DESPK_EN)|P_Fld(0,FLD_RG_SYNC_PWD ));  // [SA7_Nick] 080520 revised, power down despike  // vIO32WriteFldAlign(PDWNC_VGACFG2, 0x88801011,PDWNC_VGACFG2_All);  //5360 Michael checked
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0x01, FLD_RG_FB_AB_EN );
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_SHIFT_PWD );  //power down SOGY sink clamp
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_OFFCUR_PWD); //power down SOGY source clamp
     vIO32WriteFldAlign(HDFE_03, 512, AD1_GAIN_BIAS);   //5881 fix 512
     vIO32WriteFldAlign(HDFE_03, 512, AD2_GAIN_BIAS);   //5881 fix 512
     vIO32WriteFldAlign(HDFE_03, 512 , AD3_GAIN_BIAS);   //5881 fix 512
//vDrvVoDDDSRST(1);


    //Set clamp control mode
    for(bLoopCnt=0; bLoopCnt<3; bLoopCnt++)
    {
        //vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(bLoopCnt<<4), 0, DCLAMP_EN_1);      //5365 mark //disable digital clamp of CH 1/2/3
        //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bLoopCnt<<4), 0, CLAMP_SOURCE_OFF_1);
        //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bLoopCnt<<4), 0, CLAMP_SINK_OFF_1);
    }
    vIO32WriteFldAlign(HDFE_07, 0x03, vga_filter_byps);

    // 1. Enable Mapping of Channel 1
    vIO32WriteFldAlign(HDFE_02, 0x0F, IDX_CHANNEL_EN); 	//[SA7_Michael] enable mapping table for CH1,2,3
    switch (bVDOINPUT)
    {
        case P_YP0:   //HDTV
        case P_YP1:
            vIO32WriteFldAlign(REG_VGA_Normal_CFG1, 0, RG_CLAMP_MIDDLE_1);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG2, 1, RG_CLAMP_MIDDLE_2);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 1, RG_CLAMP_MIDDLE_3);
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(8,FLD_RG_SYNC1_VTL)|P_Fld(8,FLD_RG_SYNC1_VTH));//SOY
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0,FLD_RG_SYNC2_VTH)|P_Fld(0,FLD_RG_SYNC2_VTL));//MON
            vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_VSYNC_EN)|P_Fld(0,FLD_RG_HSYNC_EN)|P_Fld(1,FLD_RG_BYPS_SYNCPROSR));
            bInputType = 0;
        break;
        case P_VGA:
            vIO32WriteFldAlign(REG_VGA_Normal_CFG1, 0, RG_CLAMP_MIDDLE_1);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG2, 0, RG_CLAMP_MIDDLE_2);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, RG_CLAMP_MIDDLE_3);
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(4,FLD_RG_SYNC1_VTH)|P_Fld(2,FLD_RG_SYNC1_VTL));//HSYNC
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(4,FLD_RG_SYNC2_VTH)|P_Fld(2,FLD_RG_SYNC2_VTL)); //VSYNC
            vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(1,FLD_RG_VSYNC_EN)|P_Fld(1,FLD_RG_HSYNC_EN)|P_Fld(0,FLD_RG_BYPS_SYNCPROSR));
            bInputType = 1;
        break;

        case P_FB0:   //SCART0
        case P_FB1:   //SCART1
            vIO32WriteFldAlign(REG_VGA_Normal_CFG1, 0, RG_CLAMP_MIDDLE_1);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG2, 0, RG_CLAMP_MIDDLE_2);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, RG_CLAMP_MIDDLE_3);
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0x08,FLD_RG_SYNC1_VTL)|P_Fld(0x08,FLD_RG_SYNC1_VTH));//CVBS2SYNC
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(2,FLD_RG_SYNC2_VTH)|P_Fld(2,FLD_RG_SYNC2_VTL)); //FB
            bInputType = 2;
        break;

        case P_VGACOMP:
        	vIO32WriteFldAlign(REG_VGA_Normal_CFG1, 1, RG_CLAMP_MIDDLE_1);
              vIO32WriteFldAlign(REG_VGA_Normal_CFG2, 0, RG_CLAMP_MIDDLE_2);
          	vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 1, RG_CLAMP_MIDDLE_3);
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(8,FLD_RG_SYNC1_VTL)|P_Fld(8,FLD_RG_SYNC1_VTH));//SOY
            vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0,FLD_RG_SYNC2_VTH)|P_Fld(0,FLD_RG_SYNC2_VTL)); //MON
              vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_VSYNC_EN)|P_Fld(0,FLD_RG_HSYNC_EN)|P_Fld(0,FLD_RG_BYPS_SYNCPROSR));
        	bInputType = 3;
            break;

        default:
            bInputType= 0xFF;
        break;
    }

#ifdef CC_SUPPORT_SIF_BYPASS
     		vScartSVInRFE(DISABLE);
#endif
    //Load PGA gain and offset

    if(bInputType != 0xFF)
    {
        if(bInputType==INT_VGA)
        {
            vSetPGAGainOffsetInitVal(INT_SCART);  //We use VGA signal to simulate SCART signal to do auto color. So we have to set the related variables.
        }
        vSetPGAGainOffsetInitVal(bInputType);



     #ifdef AUTOCOLOR_NEW_METHOD
            for(bLoopCnt=0; bLoopCnt<3; bLoopCnt++)
           {
            		//vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(0x10*bLoopCnt), 0, PGAGAIN_1);      //PGAGAIN_1,PGAGAIN_2,PGAGAIN_3
            		vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bLoopCnt), 0x01, RG_FE_OFFSET_P1 );  //PPGAOFFSET_2,PGAOFFSET_3
		vDrvSetHDADCDigitalOffset(bLoopCnt,_bAutoColorOffset[bInputType][bLoopCnt]);
		bDrvSetHDADCGain_Digital(bLoopCnt,_bAutoColorGain[bInputType][bLoopCnt]);
       }
    #else
    vIO32WriteFldAlign(HDFE_00, 0x00, AD1_OFFSET);  //set digital offset =1 for CH 1
    vIO32WriteFldAlign(HDFE_00, 0x00, AD2_OFFSET);	 //set digital offset =1 for CH 2
    vIO32WriteFldAlign(HDFE_00, 0x00, AD3_OFFSET);  //set digital offset =1 for CH 3
    // 2. Set the Mapping Gain fot ADC1~3
    vIO32WriteFldAlign(HDFE_01, 0x8000, AD1_GAIN);
    vIO32WriteFldAlign(HDFE_01, 0x8000, AD2_GAIN);
    vIO32WriteFldAlign(HDFE_02, 0x8000, AD3_GAIN);
       // for(bLoopCnt=0; bLoopCnt<3; bLoopCnt++)
       // {
        //    vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(0x10*bLoopCnt), _bAutoColorGain[bInputType][bLoopCnt], PGAGAIN_1);      //PGAGAIN_1,PGAGAIN_2,PGAGAIN_3
        //    vIO32WriteFldAlign(REG_VGA_Normal_CFG2+(0x10*bLoopCnt), _bAutoColorOffset[bInputType][bLoopCnt], PGAOFFSET_1);  //PGAOFFSET_1,PGAOFFSET_2,PGAOFFSET_3
        // }
    #endif
        vDrvHDTVMeasureSetting(UPDATE_PER_1_FIELD);
        vDrvHDTVClampMethodDefaultSetting(bInputType);
    }

    //Set RGB front clamp-control and LPF
    if ((bVDOINPUT == (UINT8)P_FB0)||(bVDOINPUT == (UINT8)P_FB1))
    {
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_TVD3D, VSRC_MUX9);
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_TVD3D, VSRC_MUX8);
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_TVD3D, VSRC_MUX7);
    }
    else
    {
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_HDTV, VSRC_MUX9);
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_HDTV, VSRC_MUX8);
        vIO32WriteFldAlign(VSRC_01, CLAMP_OF_HDTV, VSRC_MUX7);
    }
}

// => bApiVideoMainSubSrc
/**
 * @brief Set the HDTV ADCs input muxes
 *
 * Set the HDTV ADCs input muxes by different input types
 * @param bVDOINPUT bVDOINPUT is the input types, like VGA/SCART/HDTV
 */

void vDrvSetHDTVMux(UINT8 bVDOINPUT)
{
    //UINT8 bCh;
   #if 1
    // Solve VGA ADC no data
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, CLR);vUtDelay1ms(1);
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 1, CLR);vUtDelay1ms(1);
	//vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, CLR);vUtDelay1ms(1);
    // Solve VGA ADC no data
   #endif
    switch (bVDOINPUT)
    {
        case P_YP1:
        //case P_FB0:
        //case P_FB1:
            #ifdef CC_ONE_LAYER_PCB
            vIO32WriteFldAlign(PDWNC_VGACFG0, VGAMUX_RGB_EN, FLD_RGBHDTV01_EN);		//Set RGB Mux RGB
            vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_SOY1_SOY0_EN);	                            //Disable SOY0, disable SOY1
            vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_RG_SOG_EN);	                                   //Enable SOG
            #else
            vIO32WriteFldAlign(PDWNC_VGACFG0, VGAMUX_HDTV1_EN, FLD_RGBHDTV01_EN);	//Set RGB Mux YPbPb1
            vIO32WriteFldAlign(PDWNC_VGACFG1, 2, FLD_SOY1_SOY0_EN);	                            //Enable SOY1, disable SOY0
            vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOG_EN);	                                   //disable  SOG
            #endif
        break;
        case P_YP0:
            vIO32WriteFldAlign(PDWNC_VGACFG0, VGAMUX_HDTV0_EN, FLD_RGBHDTV01_EN);	//Set RGB Mux YPbPb0
            vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_SOY1_SOY0_EN);	                            //Enable SOY0, disable SOY1
            vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOG_EN);	                                   //disable  SOG
        break;
        case P_VGA:
        case P_VGACOMP:
           #ifdef CC_ONE_LAYER_PCB
            vIO32WriteFldAlign(PDWNC_VGACFG0, VGAMUX_HDTV1_EN, FLD_RGBHDTV01_EN);	//Set RGB Mux YPbPb1
            vIO32WriteFldAlign(PDWNC_VGACFG1, 2, FLD_SOY1_SOY0_EN);	                            //Enable SOY1, disable SOY0
            vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOG_EN);	                                   //disable  SOG
            #else
            vIO32WriteFldAlign(PDWNC_VGACFG0, VGAMUX_RGB_EN, FLD_RGBHDTV01_EN);		//Set RGB Mux RGB
            vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_SOY1_SOY0_EN);	                            //Disable SOY0, disable SOY1
            vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_RG_SOG_EN);	                                   //Enable SOG
            #endif
        break;
        default:
        assert(0);
    }
}

//to control the SOY0, SOY1, SOGY ; 1:enable, 0:disable
void vDrvSOY0EN(UINT8 bEn)
{
    if (bEn == 1) //Enable SOY0
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_RG_SOY0_EN);
    }
    else //Disable SOY1
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOY0_EN);
    }
}

void vDrvSOY1EN(UINT8 bEn)
{
    if (bEn == 1) //Enable SOY1
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_RG_SOY1_EN);
    }
    else //Disable SOY1
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOY1_EN);
    }
}

void vDrvSOGEN(UINT8 bEn)
{
    if (bEn == 1) //Enable SOG
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 1, FLD_RG_SOG_EN);
    }
    else //Disable SOG
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0, FLD_RG_SOG_EN);
    }
}


// => bApiVideoMainSubSrc
/**
 * @brief Set all the HDTV ADCs on / off
 *
 * Set all the HDTV ADCs on / off
 * @param bPow bPow is the on / off selection (SV_TRUE, SV_FALSE)
 */
void vDrvAllHDADCPow(UINT8 bPow)
{
	UINT8 bCh;
	if (bPow == SV_TRUE)//Power on
	{
		if(IO32ReadFldAlign(REG_VGA_Normal_CFG0,RG_VREFGEN4FE_PWD )==1)
		{
       #if PD_ADC_ENABLE
        //vIO32WriteFldAlign(REG_VGA_Normal_CFG0, 1, SELF_EN); //5365 mark
	//	vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0,VBG_PWD);
	//	vIO32WriteFldAlign(REG_VGA_Normal_CFG4,0,ADCBIAS_PWD);
		vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0,RG_VREFGEN4FE_PWD );
       // vUtDelay1ms(2);
		for (bCh=0; bCh<3; bCh++)
		{
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4), 0, CLAMP_1_PWD); //5365 mark
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4), 0, CLAMP_SOURCE_OFF_1);
           // vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4), 0, CLAMP_SINK_OFF_1);
          //  vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4), 0, PGA_1_PWD);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh), 0, RG_VGAADC1_CORE_PWD);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh), 0, RG_VGAADC1_DIV_SEL );
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh), 0, RG_VGAADC1_CK_PWD);
           vIO32WriteFldMulti(REG_VGA_Normal_CFG1+(4*bCh),P_Fld(0,RG_FE_1_PWD )|P_Fld(0,RG_COP_1_PWD ));
		}
	vIO32WriteFldMulti(REG_VGA_Normal_CFG4,P_Fld(0,RG_VGAADC1_IO_PWD)|P_Fld(0,RG_VGAADC2_IO_PWD)|P_Fld(0,RG_VGAADC3_IO_PWD));
        vUtDelay1ms(2);
        //vIO32WriteFldAlign(REG_VGA_Normal_CFG0, 0, SELF_EN); //5365 mark
       #endif
    vIO32WriteFldAlign(HDTV_03, 1, HDTV_EN);
    vDrvAllADCPLLPow(SV_TRUE);
	}
	}
	else if (bPow == SV_FALSE)//Power down
	{
		if(IO32ReadFldAlign(REG_VGA_Normal_CFG0,RG_VREFGEN4FE_PWD )==0)
		{
       #if PD_ADC_ENABLE
		//vIO32WriteFldAlign(REG_VGA_Normal_CFG0,1,VBG_PWD);
		//vIO32WriteFldAlign(REG_VGA_Normal_CFG4,1,ADCBIAS_PWD);
		vIO32WriteFldAlign(REG_VGA_Normal_CFG0,1,RG_VREFGEN4FE_PWD );
		for (bCh=0; bCh<3; bCh++)
		{
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG1,1,CLAMP_1_PWD); //5365 mark
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4),1,CLAMP_SOURCE_OFF_1);
            //vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4),1,CLAMP_SINK_OFF_1);
           // vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(bCh<<4),1,PGA_1_PWD);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh),1,RG_VGAADC1_CORE_PWD);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh), 1, RG_VGAADC1_DIV_SEL);
            vIO32WriteFldAlign(REG_VGA_Normal_CFG5+(4*bCh), 1, RG_VGAADC1_CK_PWD);
           vIO32WriteFldMulti(REG_VGA_Normal_CFG1+(4*bCh),P_Fld(1,RG_FE_1_PWD )|P_Fld(1,RG_COP_1_PWD ));
		}
	vIO32WriteFldMulti(REG_VGA_Normal_CFG4,P_Fld(1,RG_VGAADC1_IO_PWD)|P_Fld(1,RG_VGAADC2_IO_PWD)|P_Fld(1,RG_VGAADC3_IO_PWD));
       #endif
    vIO32WriteFldAlign(HDTV_03, 0, HDTV_EN);
    vDrvAllADCPLLPow(SV_FALSE);
	}
}
}

// => _PdwncSetupVGA
/**
 * @brief Set all the vDrvSYNCSlicerWakeup on / off
 *
 * Set all the vDrvSYNCSlicerWakeup on / off
 * @param bPow bPow is the on / off selection (SV_TRUE, SV_FALSE)
 */
void vDrvSYNCSlicerWakeup(UINT8 bPow)
{
	if (bPow == SV_TRUE) // for Wakeup setting
	{
#if 0//!defined(CC_MT5399) && !defined(CC_MT5881)//beforce remy
        vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(1,FLD_RG_HSYNC_EN));
#ifdef CC_SOURCE_AUTO_DETECT
    	vIO32WriteFldAlign(PDWNC_VGACFG1, 1,FLD_RG_VSYNC_EN);
#else
        vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(1,FLD_RG_VSYNC_EN)|P_Fld(1,FLD_RG_SOG_EN));
#endif
#endif

		vIO32WriteFldMulti(PDWNC_VGACFG3, P_Fld(0, FLD_RG_SOGY_SORS_PWD)|P_Fld(0,FLD_RG_SOGY_SINK_PWD) );
        DBG_Printf(VGA_Debug,"[SA7]for Wakeup setting \n");
	}
	else if (bPow == SV_FALSE)//Power down setting
    {
  		//vIO32WriteFldMulti(PDWNC_SOGYCFG,P_Fld(0,FLD_SELECT));
        vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_HSYNC_EN));
        vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0,FLD_RG_VSYNC_EN)|P_Fld(1,FLD_RG_SOG_EN));
     vIO32WriteFldMulti(PDWNC_VGACFG3,P_Fld (1   , FLD_RG_SOGY_SORS_PWD)|P_Fld(1,FLD_RG_SOGY_SINK_PWD) );
    //vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_SHIFT_PWD );
    //vIO32WriteFldAlign(PDWNC_VGACFG2, 0   , FLD_RG_OFFCUR_PWD);
        DBG_Printf(VGA_Debug,"[SA7]for Power down setting \n");
	}
}



//New function for manual gain/offset adjustment

/**
 * @brief  save RGB_offset to EEPROM
 *
 * @param None
 */
void vCust_R_Offset_Update(UINT8 bOffset)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);        //MC20080417_1 ***
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorOffset[bType][2] = bOffset<<1;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bOffset>>8) & 0xff;
                //bTmpLowByte =  bOffset& 0xff;

				bTmpHighByte = bOffset && 0x80;
                bTmpLowByte =  (bOffset<<1)&0xff;
        if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP2 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP2_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP2 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP2_HIGH_BYTE , bTmpHighByte));
            }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP2 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP2_HIGH_BYTE , bTmpHighByte));
            }
                //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?  EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG10, _bAutoColorOffset[bType][2], PGAOFFSET_3);
    		vIO32WriteFldAlign(HDFE_00, _bAutoColorOffset[bType][2], AD3_OFFSET);	 //set digital offset =1 for CH 2
        }

    }
}
void vCust_G_Offset_Update(UINT8 bOffset)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);        //MC20080417_1 ***
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorOffset[bType][1] = bOffset<<1;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bOffset>>8) & 0xff;
                //bTmpLowByte =  bOffset& 0xff;

				bTmpHighByte = bOffset && 0x80;
                bTmpLowByte =  (bOffset<<1)&0xff;
        if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP1 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP1_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP1 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP1_HIGH_BYTE , bTmpHighByte));
            }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP1 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP1_HIGH_BYTE , bTmpHighByte));
            }
            //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?       EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG6, _bAutoColorOffset[bType][1], PGAOFFSET_2);
   		vIO32WriteFldAlign(HDFE_00, _bAutoColorOffset[bType][1], AD2_OFFSET);	 //set digital offset =1 for CH 2
        }
    }
}

void vCust_B_Offset_Update(UINT8 bOffset)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);        //MC20080417_1 ***
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorOffset[bType][0] = bOffset<<1;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bOffset>>8) & 0xff;
                //bTmpLowByte =  bOffset& 0xff;

				bTmpHighByte = bOffset && 0x80;
                bTmpLowByte =  (bOffset<<1)&0xff;
        if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP0 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_SCART_CLAMP0_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP0 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_VGA_CLAMP0_HIGH_BYTE , bTmpHighByte));
            }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP0 , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_OFFSET_HDTV_CLAMP0_HIGH_BYTE , bTmpHighByte));
            }
            //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?       EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG2, _bAutoColorOffset[bType][0], PGAOFFSET_1);
    		vIO32WriteFldAlign(HDFE_00, _bAutoColorOffset[bType][0], AD1_OFFSET);  //set digital offset =1 for CH 1
        }
    }
}



/**
 * @brief  save RGB_Gain to EEPROM
 *
 * @param None
 */
void vCust_R_Gain_Update(UINT8 bGain)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorGain[bType][2]=bGain<<8;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bGain>>8) & 0xff;
                //bTmpLowByte =  bGain& 0xff;

				bTmpHighByte = bGain & 0xff;
                bTmpLowByte =  0;
       if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART2_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART2_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA2_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA2_HIGH_BYTE , bTmpHighByte));
           }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV2_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV2_HIGH_BYTE , bTmpHighByte));
            }
            //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?        EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG10, _bAutoColorGain[bType][2], PGAGAIN_3);
     		vIO32WriteFldAlign(HDFE_02, _bAutoColorGain[bType][2], AD3_GAIN);
       }
    }
}

void vCust_G_Gain_Update(UINT8 bGain)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorGain[bType][1]=bGain<<8;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bGain>>8) & 0xff;
                //bTmpLowByte =  bGain& 0xff;

				bTmpHighByte = bGain & 0xff;
                bTmpLowByte =  0;
        if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART1_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART1_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA1_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA1_HIGH_BYTE , bTmpHighByte));
            }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV1_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV1_HIGH_BYTE , bTmpHighByte));
            }
            //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?        EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG6, _bAutoColorGain[bType][1], PGAGAIN_2);
    		vIO32WriteFldAlign(HDFE_01, _bAutoColorGain[bType][1], AD2_GAIN);
        }
    }
}

void vCust_B_Gain_Update(UINT8 bGain)
{
    UINT8 bIntputType,bType;
#ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
#endif
    bIntputType   = bGetICInputType(SV_VP_MAIN);
    bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
    if(bType!=P_FA && bType<4)
    {
        _bAutoColorGain[bType][0]=bGain<<8;
        if(_bEEPROM_ready==1)
        {
                //bTmpHighByte = (bGain>>8) & 0xff;
                //bTmpLowByte =  bGain& 0xff;

				bTmpHighByte = bGain & 0xff;
                bTmpLowByte =  0;
        if((bType==INT_SCART) && (fgIsCLKLock()))
        {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART0_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_SCART0_HIGH_BYTE , bTmpHighByte));
        }
        else
        {
            if ( fgIsMainVga()||fgIsPipVga() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA0_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_VGA0_HIGH_BYTE , bTmpHighByte));
            }
            else if ( fgIsMainYPbPr()||fgIsPipYPbPr() )
            {
                 UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV0_LOW_BYTE , bTmpLowByte));
                UNUSED(fgApiEepromWriteByte(EEP_ADC_DIG_GAIN_HDTV0_HIGH_BYTE , bTmpHighByte));
            }
            //UNUSED(fgApiEepromWriteByte((fgIsMainVga()||fgIsPipVga()) ?        EEP_VIDEO_AUTO_COLOR_VGA_DONE : EEP_VIDEO_AUTO_COLOR_HDTV_DONE, SV_TRUE));
        }
    }
        else
        {
      		//vIO32WriteFldAlign(REG_VGA_Normal_CFG2, _bAutoColorGain[bType][0], PGAGAIN_1);
    		vIO32WriteFldAlign(HDFE_01, _bAutoColorGain[bType][0], AD1_GAIN);
        }
    }
}
void vCust_Gain_OFFSet_Update(void)
{
    		UINT8 bIntputType,bType;
			#ifdef CC_SUPPORT_PIPELINE
			bIntputType =u1ADCConnentSrcPort;
			#else
    		bIntputType   = bGetICInputType(SV_VP_MAIN);
			#endif
    		bType  = bDrvOnChipGetVFESignalType(bIntputType);  //to get Signal Type
            _bEFUSE_AUTOCOLOR_READY=0;
            if(bType<4)
            {
            	_bPGAGainOffsetHistory[bType]=1;
            }
             _bcurrent_use_adc=2;  //external ADC
             vDrvEnableBlankLevelAdjust();
			 Printf("vDrvEnableBlankLevelAdjust\n");

}

void vCust_EBGain_OverRule_Update(UINT8 bNum) //kaloverrule
{
	UINT8 bCh;
	UINT8 bTmpvalue,bTmpResult;
	DBG_Printf(VGA_Debug,"vCust_EBGain_OverRule_Update (%x)\n", bNum);

    UNUSED(fgApiEepromWriteByte(EEP_ADC_AUTOCOLOR_OVER_RULE_EFUSE, bNum));
	if(bNum ==0xAA)
	{
		_bcurrent_use_adc=2;  //external ADC
         if(bApiEepromReadByte(EEP_ADC_AUTOCOLOR_OVER_RULE_EFUSE)==0xAA)//OTP to external ADC
         {
   		#if EEPROM_READY
    			_bEEPROM_ready = 1;
   		#else
    			_bEEPROM_ready = 0;
   		#endif
            _bEFUSE_AUTOCOLOR_READY=0;
         }
	}
     else//external ADC to OTP
         {
     		if (DRVCUST_OptGet(eEFuseAutocolor))
     		{
            	bTmpResult=0;
            	for(bCh=0; bCh<3; bCh++)
	            {
	            	if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>24)==0)
	            	{
	                 		bTmpvalue=((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>(8*bCh));
	                 		if((bTmpvalue>=0x80) && (bTmpvalue<=0xEE))
	                 		{
	                      		bTmpResult |= (0x01 << bCh);
	                 		}
	            	}
	            	else
	            	{
	            	   bTmpvalue=((IO32ReadFldAlign(0xf0008660, AC_FULLDW))>>(8*(bCh+1)));
	                 	if((bTmpvalue>=0x80) && (bTmpvalue<=0xEE))
	                 	{
	                      	bTmpResult |= (0x01 << bCh);
	                 	}

	            	}
	            }
           		_bEFUSE_AUTOCOLOR_READY = ((bTmpResult & 0x07) != 0x07) ? 0 : 1;
				_bcurrent_use_adc=1;  //Efuse
     		}
     		else
     		{
            _bEFUSE_AUTOCOLOR_READY=0;//external ADC
 			_bcurrent_use_adc=3;  //default
            }
			MTAL_SetPGAGainOffsetInitVal();
			vDrvEnableBlankLevelAdjust();

         }

}

UINT8 vCust_EBGain_OverRule_Read(void) //kaloverrule
{
    return (UINT8)bApiEepromReadByte(EEP_ADC_AUTOCOLOR_OVER_RULE_EFUSE);
}

void vCust_Embedded_Gain(UINT8 bType) //kaloverrule
{
    UINT8 bCh;
    UINT8 _bEmbeddedGain[3]={0,0,0};
      if(bType==0)
      {
	    for(bCh=0; bCh<3; bCh++)
		{
		  _bEmbeddedGain[bCh] = _bAutoColorGain_for_EFuse[0][bCh]>>8;//YPbPr
		}
		LOG(0,"After YPbPr transformation\n");
      }
      else if(bType==1)
      {
		for(bCh=0; bCh<3; bCh++)
		{
		  _bEmbeddedGain[bCh] = _bAutoColorGain_for_EFuse[1][bCh]>>8;//VGA
		}
		LOG(0,"After VGA transformation\n");
      }
      else if(bType==2)
      {
		for(bCh=0; bCh<3; bCh++)
		{
		  _bEmbeddedGain[bCh] = _bAutoColorGain_for_EFuse[2][bCh]>>8;//SCART
		}
		LOG(0,"After SCART transformation\n");
      }

	LOG(0,"The Embedded B_Gain=0x%x, G_Gain=0x%x, R_Gain=0x%x\n",_bEmbeddedGain[0],_bEmbeddedGain[1],_bEmbeddedGain[2]);
}
UINT8 vCust_Get_Embedded_Gain(UINT8 Type,UINT8 Ch) //kaloverrule
{
      UINT8 _bEmbeddedGain[3]={0,0,0};
      
	    {
		  _bEmbeddedGain[Ch] = _bAutoColorGain_for_EFuse[Type][Ch]>>8;//YPbPr
		}
		
      

	LOG(0,"The Embedded  Type %d ch %d Gain=0x%x \n",Type,Ch, _bEmbeddedGain[Ch]);
	return _bEmbeddedGain[Ch];
}


//SOGY slicer Vth setting by customer
void vCust_SOGY_VTH_Set(UINT32 dwLevel)
{
    dwLevel = (dwLevel << 24);
 //   vIO32WriteFldAlign(SyncSlice_Control_01, dwLevel,VTH_SEL);
        vIO32WriteFldAlign(PDWNC_VGACFG4,dwLevel,FLD_RG_SYNC1_VTH);
}
//SOGY slicer Vtl setting by customer
void vCust_SOGY_VTL_Set(UINT32 dwLevel)
{
    dwLevel= (dwLevel << 28);
    //vIO32WriteFldAlign(SyncSlice_Control_01, dwLevel,VTL_SEL);

    vIO32WriteFldAlign(PDWNC_VGACFG4,dwLevel,FLD_RG_SYNC1_VTL);

}

void MTAL_SOGY_VTH_Set(UINT8 bLevel)
{
	rSlicerConfig.AP_SliceLevelManuSet=1;
	if(bLevel>0x0f)
	{
	   bLevel = 0x0f;
	}
	rSlicerConfig.AP_SVTH_ManuValue=bLevel;
	rSlicerConfig.AP_SVTLH_ManuValue=(bLevel) |(rSlicerConfig.AP_SVTL_ManuValue<<4);
      //vIO32WriteFldAlign(SyncSlice_Control_01, bLevel,VTH_SEL);
      vIO32WriteFldAlign(PDWNC_VGACFG4,bLevel,FLD_RG_SYNC1_VTH);
}

void MTAL_SOGY_VTL_Set(UINT8 bLevel)
{
	rSlicerConfig.AP_SliceLevelManuSet=1;
	if(bLevel>0x0f)
	{
	   bLevel = 0x0f;
	}
	rSlicerConfig.AP_SVTL_ManuValue=bLevel;
	rSlicerConfig.AP_SVTLH_ManuValue=(bLevel<<4) |(rSlicerConfig.AP_SVTH_ManuValue);
      //vIO32WriteFldAlign(SyncSlice_Control_01, bLevel,VTL_SEL);

      vIO32WriteFldAlign(PDWNC_VGACFG4,bLevel,FLD_RG_SYNC1_VTL);
}

UINT8 vCust_Current_Use_ADC_Read(UINT8 bType)
{
	return _bcurrent_use_adc;    //  1 efuse  2 eeprom  3 Default
}

/**
 * @brief  Read RGB_offset
 *
 * @param None
 */
UINT8 vCust_R_Offset_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD3_OFFSET)>>1);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD3_OFFSET)>>1);
#endif
}

UINT8 vCust_G_Offset_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD2_OFFSET)>>1);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD2_OFFSET)>>1);
#endif
}

UINT8 vCust_B_Offset_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD1_OFFSET)>>1);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_00, AD1_OFFSET)>>1);
#endif
}

/**
 * @brief  Read RGB_Gain
 *
 * @param None
 */
UINT8 vCust_R_Gain_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_02, AD3_GAIN)>>8);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_02, AD3_GAIN)>>8);
#endif
}

UINT8 vCust_G_Gain_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_01, AD2_GAIN)>>8);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_01, AD2_GAIN)>>8);
#endif
}

UINT8 vCust_B_Gain_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
	return (UINT8)(IO32ReadFldAlign(HDFE_01, AD1_GAIN)>>8);
#else
	return (UINT8)(IO32ReadFldAlign(HDFE_01, AD1_GAIN)>>8);
#endif
}

void vCust_AutoColor_EEPROM_Enable(UINT8 bEnable)
{
    if(bEnable==0)
    {
      _bEEPROM_ready=0;
    }
    else
    {
      _bEEPROM_ready=1;
    }
}

UINT8 vCust_B_MAX_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
       return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_VGA_MAX_CH0));
	}
	else if(fgIsMainYPbPr() || fgIsPipYPbPr())
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_HDTV_MAX_CH0));
	}
       return 0;
#endif
}

UINT8 vCust_G_MAX_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
       return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_VGA_MAX_CH1));
	}
	else if(fgIsMainYPbPr() || fgIsPipYPbPr())
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_HDTV_MAX_CH1));
	}
       return 0;
#endif
}

UINT8 vCust_R_MAX_Read(void)
{
#ifdef AUTOCOLOR_NEW_METHOD
       return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_VGA_MAX_CH2));
	}
	else
	{
	  return (UINT8)(bApiEepromReadByte(EEP_ADC_HDTV_MAX_CH2));
	}
#endif
}

//Mike modify ADC setting 20080704
void vCust_R_MAX_Update(UINT8 value)
{
#ifdef AUTOCOLOR_NEW_METHOD
      // return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_VGA_MAX_CH2,value));
	}
	else if(fgIsMainYPbPr() || fgIsPipYPbPr())
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_HDTV_MAX_CH2,value));
	}
#endif
}

void vCust_G_MAX_Update(UINT8 value)
{
#ifdef AUTOCOLOR_NEW_METHOD
       //return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_VGA_MAX_CH1,value));
	}
	else if(fgIsMainYPbPr() || fgIsPipYPbPr())
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_HDTV_MAX_CH1,value));
	}
#endif
}

void vCust_B_MAX_Update(UINT8 value)
{
#ifdef AUTOCOLOR_NEW_METHOD
      // return 0;
#else
	if ( fgIsMainVga()||fgIsPipVga() )
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_VGA_MAX_CH0,value));
	}
	else
	{
	  UNUSED(fgApiEepromWriteByte(EEP_ADC_HDTV_MAX_CH0,value));
	}
#endif
}

UINT16 vCust_B_Blank_Read(void)
{
    UINT32 dwSumADCRawData;
    UINT16 wData_v1v2;
    UINT8 bLoopCnt;
    dwSumADCRawData = 0;
    if(bDvMeasureDataSel()==1)
    {
        dwSumADCRawData = bDrvGetBlankVal(0);   //to read the blank level of Y, Pb, Pr respective
        wData_v1v2 = (dwSumADCRawData)<<3; // 10 bits to 13 bits = 8bit sum 32 times
    }
    else
    {
        for(bLoopCnt = 0; bLoopCnt<32; bLoopCnt++)
        {
            dwSumADCRawData += bDrvGetBlankVal(0);   //to read the blank level of Y, Pb, Pr respective
            vUtDelay2us(50);
        }
        wData_v1v2 = (dwSumADCRawData+2)>>2; // 10 bits to 8 bits // +8) >> 4; // 12 bits to 8 bits
    }
    wData_v1v2 = wData_v1v2 >>3;  //10 BIT resolution

    return (wData_v1v2);
}

UINT16 vCust_G_Blank_Read(void)
{
    UINT32 dwSumADCRawData;
    UINT16 wData_v1v2;
    UINT8 bLoopCnt;
    dwSumADCRawData = 0;
    if(bDvMeasureDataSel()==1)
    {
        dwSumADCRawData = bDrvGetBlankVal(1);   //to read the blank level of Y, Pb, Pr respective
        wData_v1v2 = (dwSumADCRawData)<<3; // 10 bits to 13 bits = 8bit sum 32 times
    }
    else
    {
        for(bLoopCnt = 0; bLoopCnt<32; bLoopCnt++)
        {
            dwSumADCRawData += bDrvGetBlankVal(1);   //to read the blank level of Y, Pb, Pr respective
            vUtDelay2us(50);
        }
        wData_v1v2 = (dwSumADCRawData+2)>>2; // 10 bits to 8 bits // +8) >> 4; // 12 bits to 8 bits
    }
    wData_v1v2 = wData_v1v2 >>3;  //10 BIT resolution

    return (wData_v1v2);
}

UINT16 vCust_R_Blank_Read(void)
{
    UINT32 dwSumADCRawData;
    UINT16 wData_v1v2;
    UINT8 bLoopCnt;
    dwSumADCRawData = 0;
    if(bDvMeasureDataSel()==1)
    {
        dwSumADCRawData = bDrvGetBlankVal(2);   //to read the blank level of Y, Pb, Pr respective
        wData_v1v2 = (dwSumADCRawData)<<3; // 10 bits to 13 bits = 8bit sum 32 times
    }
    else
    {
        for(bLoopCnt = 0; bLoopCnt<32; bLoopCnt++)
        {
            dwSumADCRawData += bDrvGetBlankVal(2);   //to read the blank level of Y, Pb, Pr respective
            vUtDelay2us(50);
        }
        wData_v1v2 = (dwSumADCRawData+2)>>2; // 10 bits to 8 bits // +8) >> 4; // 12 bits to 8 bits
    }
    wData_v1v2 = wData_v1v2 >>3;  //10 BIT resolution

    return (wData_v1v2);
}

UINT16 vCust_B_Max_Read(void)
{
    return bDrvOnChipGetADCMaxMinValue(0,AS_PHASE_MAX_SEL);
}

UINT16 vCust_G_Max_Read(void)
{
    return bDrvOnChipGetADCMaxMinValue(1,AS_PHASE_MAX_SEL);;
}

UINT16 vCust_R_Max_Read(void)
{
    return bDrvOnChipGetADCMaxMinValue(2,AS_PHASE_MAX_SEL);;
}

void vCust_clear_eeprom(UINT8 bValue)
{
    int i;
    for(i=EEP_ADC_START; i<(EEP_VIDEO_AUTO_COLOR_END+1); i++)
    {
        UNUSED(fgApiEepromWriteByte(i ,bValue));
    }
}


#ifdef CC_COPLAT_MT82
typedef UINT32  HANDLE_T;
#define OSR_OK                    ((INT32)   0)
#endif

INT32 AD_AcquireControl(void)
{
	return OSR_OK;
}

INT32 AD_ReleaseControl(void)
{
	return OSR_OK;
}

UINT16 AD_QueryInputSource(void)
{
	// return format:  (PIP_IPS<<8)|MAIN_IPS
	//  *	current only return MAIN path for 5381
	//  *	going to support 5382p
	//

	switch (VSS_MAJOR(_bSrcMainNew))
	{
		case VSS_ATV:
			return IPS_ATV;
		case VSS_ATD:
			return IPS_ATD;
		case VSS_CVBS:
		case VSS_SV:
		case VSS_SCART:
			return IPS_CVBS_A_B; //use 2 channel
		case VSS_DTV:
			return IPS_DTV;
		case VSS_DTD:
			return IPS_DTD;
		default:
			return IPS_DTV;
	}

}


#if 1//SUPPORT_SCART
#define SCART_SOGY_BW 0		//0~3
#define SCART_SOGY_RGAIN 3	//0~3   // [SD3/SA7/nick] 20080407 revised MT5391 SCART RGB By External Path  // 1
#define SCART_FB_HIGH_THR 1	//0~7
#define SCART_FB_LOW_THR 1	//0~7
#define SCART_SOY_HIGH_THR 8//0~15
#define SCART_SOY_LOW_THR 8	//0~15

/*	Scart Setting for drv_scart.c		*/
// =>
/**
 * @brief Set SCART Video front end
 * Set SCART VFE
 * @param void
 */
void vScartVfeConnect(void)
{
    // 1. Select YC mixer for CVBS signal to sync slicer
    // 2. Set SYNC path
    // 3. Set FB path
    // 4. Power down sink and source clamp
    // 5. Set slicer BW,gain and VH,VL

    UINT32 ret_FB = 0;
   // UINT32 ret_SCART1_SC = 0;
  //  UINT32 ret_SCART2_SC = 0;
    UINT32 CHA =0 ;
    UINT32 i =0 ;
    // 1. Select YC mixer
 //   vIO32WriteFldMulti(REG_CVBS_CFG1, P_Fld(1,RG_MIXINPUT)|P_Fld(5,RG_MIXGAIN));//power on mixer 0
 //   vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_MIX_PWD);

    //vIO32WriteFldMulti(VFE_E0, P_Fld(0,RG_MIX1_PWD)|P_Fld(1,RG_MIX1_INPUT)|P_Fld(2,RG_MIX1_GAIN));//power on mixer 1

    //vIO32WriteFldMulti(VFE_E5, P_Fld(1,RG_MIX_10U));//
 //   vIO32WriteFldMulti(REG_CVBS_CFG1, /*mark for V2: P_Fld(1,RG_MIX_60U_EN) |P_Fld(0,RG_MIX_SHORT)|*/P_Fld(2,RG_MIXVOCM_SEL));//

    //vIO32WriteFldMulti(VFE_E5, P_Fld(1,RG_MIX1_10U)|P_Fld(0,RG_MIX1_60U_EN)|P_Fld(1,RG_MIX1_BEN)|P_Fld(2,RG_MIX0_SHORT));
    //vIO32WriteFldAlign(VFE_E7, 0x00, RG_CVBS2SYNCA_EN); CY //YC mixer0 NOT to SYNC slicer
    //vIO32WriteFldAlign(VFE_E7, 0x01, RG_CVBS2SYNCB_EN); CY //YC mixer1 to SYNC slicer
    // 2. Set SYNC path
    vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld(0, FLD_RG_SOY0_EN) | P_Fld(0, FLD_RG_SOY1_EN)| P_Fld(0, FLD_RG_SOG_EN)); //Disconnect SOY0.SOY1,SOG
    vIO32WriteFldAlign(PDWNC_VGACFG1, 1   , FLD_RG_CVBS_EN);
    vIO32WriteFldAlign(PDWNC_VGACFG1, 0   , FLD_RG_HSYNC_EN);
    vIO32WriteFldAlign(PDWNC_VGACFG1, 0x00, FLD_RG_FB_AB_EN );
    vIO32WriteFldAlign(REG_CVBS_CFG1, 0x10, RG_CVBS_REV);//
    // 3. Set FB path
#ifdef CC_SUPPORT_PIPELINE
	if((_fVFEAVDMainICPin == P_FB0) || (_fVFEAVDSubICPin == P_FB0))
#else
    if ((_bMainICIn == P_FB0) || (_bSubICIn == P_FB0))//SCART1
#endif

    {
        ret_FB = (UINT32)DRVCUST_OptGet(eSCART1FBInSelect);
    }
#ifdef CC_SUPPORT_PIPELINE
    else if ((_fVFEAVDMainICPin == P_FB1) || (_fVFEAVDSubICPin == P_FB1))//SCART2
#else
    else if ((_bMainICIn == P_FB1) || (_bSubICIn == P_FB1))//SCART2
#endif

    {
        ret_FB = (UINT32)DRVCUST_OptGet(eSCART2FBInSelect);
    }

    if (ret_FB == SCART_FB_IN_SOY0)
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0x04, FLD_RG_SCART_EN);
    }
    else if (ret_FB == SCART_FB_IN_SOY1)
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0x02, FLD_RG_SCART_EN);
    }
    else if (ret_FB == SCART_FB_IN_SOG)
    {
        vIO32WriteFldAlign(PDWNC_VGACFG1, 0x01, FLD_RG_SCART_EN);
    }
    // 4.  Power down sink and source clamp
    vIO32WriteFldAlign(PDWNC_VGACFG3, 1   , FLD_RG_SOGY_SINK_PWD);  //power down SOGY sink clamp
    vIO32WriteFldAlign(PDWNC_VGACFG3, 1   , FLD_RG_SOGY_SORS_PWD); //power down SOGY source clamp
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0x0   , FLD_RG_SOGY_SINK);      //set SOGY Sink current=0
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0x0   , FLD_RG_SOGY_SOURCE);   //set SOGY source current=0
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_SHIFT_PWD );  //power down SOGY sink clamp
    vIO32WriteFldAlign(PDWNC_VGACFG2, 0   , FLD_RG_OFFCUR_PWD); //power down SOGY source clamp
    vIO32WriteFldAlign(PDWNC_VGACFG2, 3   , FLD_RG_VREF_CVBS); //power down SOGY source clamp
    // 5. Set slicer BW,gain and VH,VL
    vIO32WriteFldMulti(SyncSlice_Control_00,P_Fld(0x1,RG_VT_EN)|P_Fld(0x0,RG_MASK)|P_Fld(0x0,RG_SEL)|P_Fld(0x0,RG_CAL));
    //vIO32WriteFldMulti(PDWNC_VGACFG6,P_Fld(0,FLD_RG_DIV2_EN)|P_Fld(0,FLD_RG_DIV3_EN)|P_Fld(0,FLD_RG_DIV5_EN)|P_Fld(0,FLD_RG_CKINV_SOGY_EN)|P_Fld(1,FLD_RG_CLKSEL_SOGY)|P_Fld(1,FLD_RG_SOGY_ADC_PWD));//use pixel clk
    //vIO32WriteFldAlign(SyncSlice_Control_01, 0xc,VTH_SEL);
    //vIO32WriteFldAlign(SyncSlice_Control_01, 0xb,VTL_SEL);
    vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0x08,FLD_RG_SYNC1_VTL)|P_Fld(0x08,FLD_RG_SYNC1_VTH));
    vIO32WriteFldMulti(PDWNC_VGACFG3, P_Fld(0x3   , FLD_RG_SOGY_RGAIN)|P_Fld(0x3  , FLD_RG_SOGY_BW));     //set PGA gain=6 for slicer   // 0x01 6x 0x00 3.5x gain = 3x 0x1

    #ifdef SCART_CLK_SOURCE_CVBS
    //vIO32WriteFldMulti(ASYNC_08, P_Fld(0,AS_MASK_SLICE_EN)|P_Fld(1,AS_VMASK1_OFF)|P_Fld(1,AS_VMASK2_OFF)|P_Fld(1,AS_VMASK3_OFF));
    //vIO32WriteFldAlign(CKGEN_AFECFG7, 0x1, FLD_RG_ADCPLL_H_CVBS_SEL);
    #else
    //vIO32WriteFldAlign(CKGEN_AFECFG7, 0x0, FLD_RG_ADCPLL_H_CVBS_SEL);
    #endif
    // Need to check
    //vIO32WriteFldAlign(SYS_00, 1, MAIN_DATA_SYNC_INV); // For MT5387 TVD change to 54MHz issue
    //vIO32WriteFldAlign(SYS_00, 1, PIP_DATA_SYNC_INV); // For MT5363 Sub PIP TVD change to 54MHz issue

	for(i=0;i<portmap_table_num; i++)
	{
		if((portmap_table[i].vs_src==SV_VS_SCART1))
		{
			CHA=(portmap_table[i].av_mux&0x0F);
			CHA=dAvMuxMask(CHA,0);
      break;
		}
	  }
	  if ((IO32ReadFldAlign(REG_CVBS_CFG0, Fld(32, 0, AC_FULLDW))&CHA)>=1)
	  bINT_BST_DET=1;
	  else
	  bINT_BST_DET=0;

    if(bINT_BST_DET==1) // Internal Path, set from source_table.c
    {
       //      vIO32WriteFldAlign(REG_CVBS_CFG0, 0x01, RG_CINRB_EN);//connect SC from VGA VFE to CVBS VFE in CVBS part
     #ifndef CC_SUPPORT_SIF_BYPASS
#ifdef CC_SUPPORT_PIPELINE
			if((_fVFEAVDMainICPin == P_FB0) || (_fVFEAVDSubICPin == P_FB0))
#else
            if((_bMainICIn == P_FB0) || (_bSubICIn == P_FB0))//SCART1
#endif
             {
                       vIO32WriteFldAlign(PDWNC_VGACFG0, 0x01, FLD_RG_SC_SCART );
             }
             #endif
                   vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));//disable HDTV burst detector
                   #if !TVD_NEW_BST_DET_TH
				   vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
		      #endif
	}
	else
	{
          vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));//disable HDTV burst detector
          #if !TVD_NEW_BST_DET_TH
	    vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
	  #endif
	}
}

// =>
/**
 * @brief Disconnect SCART Video front end setting
 * Set SCART
 * @param void
 */
void vScartVfeDisconnect(void)
{ //Michael Modify :20100308
	UINT8 bCh;
    // 1. power on SYNC slicer clamp circuit and set sink/source current
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0   , FLD_RG_SOGY_SINK_PWD);
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0   , FLD_RG_SOGY_SORS_PWD);
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0x1B   , FLD_RG_SOGY_SINK);//101010
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0xB   , FLD_RG_SOGY_SOURCE);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_SHIFT_PWD );  //power down SOGY sink clamp
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1   , FLD_RG_OFFCUR_PWD); //power down SOGY source clamp
    vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_CVBS_REV);//
    // 2. Set clamp BW and Gain
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0x3   , FLD_RG_SOGY_BW);      // BW=13.6MHz
    vIO32WriteFldAlign(PDWNC_VGACFG3, 0x3   , FLD_RG_SOGY_RGAIN );  //Gain=6
    // 3. Dsiconnect SCART Sync and FB path
    //vIO32WriteFldAlign(VFE_E7, 0x00, RG_CVBS2SYNCA_EN); CY //disconnect CVBS to slicer for RGB sync
    //vIO32WriteFldAlign(VFE_E7, 0x00, RG_CVBS2SYNCB_EN); CY //disconnect CVBS to slicer for RGB sync
    vIO32WriteFldAlign(PDWNC_VGACFG1, 0   , FLD_RG_CVBS_EN);
    vIO32WriteFldAlign(PDWNC_VGACFG1, 0x00, FLD_RG_SCART_EN);
    // 4. Disconnect SCART input path

    vIO32WriteFldAlign(PDWNC_VGACFG0, 0x00, FLD_RG_SC_SCART );
    vIO32WriteFldAlign(PDWNC_VGACFG1, 0x01, FLD_RG_FB_AB_EN );

//    vIO32WriteFldAlign(REG_CVBS_CFG0, 0x00, RG_CINRA_EN);// //disconnect C in R at CHB
//    vIO32WriteFldAlign(REG_CVBS_CFG0, 0x00, RG_CINRB_EN);// //disconnect C in R at CHB

     #ifdef CC_SUPPORT_SIF_BYPASS
     		vScartSVInRFE(DISABLE);
 	#endif
		for (bCh=0; bCh<3; bCh++)
		{
        		vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),1,RG_CLAMP_GATE_1 );
		}

    DBG_Printf(VGA_Debug,"Disconnect FB at VFE disconnect\n");
    //penggang 20090827 mark. for CR 	DTV00140033. eg:Change sub source will cause main ypbpr slicer modify here and
    //ypbpr image position move into right lightly.
    //	vIO32WriteFldAlign(PDWNC_VGACFG1, 0x8   , FLD_RG_SOGY_VHSEL); // 0x9 for Macrovision disc set 0x8 for YPbPr
    //	vIO32WriteFldAlign(PDWNC_VGACFG1, 0x5   , FLD_RG_SOGY_VLSEL); // 0x6 for Macrovision disc set 0x8 for YPbPr
    vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));//disable HDTV burst detector
   #if !TVD_NEW_BST_DET_TH
    vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
   #endif
    vDrvBypPD();
   if (bModeIndex == SEPERATESYNC)//kal 20091223 VG848 GaryBar INV==>20101125 VGA Seperate Sync when PIP(analog)
   {
     vIO32WriteFldAlign(PDWNC_VGACFG3, 1   , FLD_RG_SOGY_SINK_PWD);//FLD_RG_SOGY_SORS_PWD
   }
   else
   {
     vIO32WriteFldAlign(PDWNC_VGACFG3, 0   , FLD_RG_SOGY_SINK_PWD);//FLD_RG_SOGY_SORS_PWD
   }
}


// =>
/**
 * @brief Set Scart Mix mode
 * Set SCART
 * @param void
 */
void vScartMix(void)
{
	UINT8 bCh;
	if(bINT_BST_DET==1) //Internal Path
	{
	    //LogS("bINT_BST_DET == 1 at Mix");
		vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));
		for (bCh=0; bCh<3; bCh++)
		{
        		vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),1,RG_CLAMP_GATE_1 );
		}
	   #if !TVD_NEW_BST_DET_TH
	    vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
	   #endif
	    //LogS("bINT_BST_DET==1 and TVD_BST at Mix");
	}
	else //TVD Bst Detector & External Path
	{
	    vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));
	   #if !TVD_NEW_BST_DET_TH
	    vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
	   #endif
	    //LogS("bINT_BST_DET==0 at Mix");
	}
    //TVD ch1 clamp on bottom (clamping off); s-first mode
	vDrvSetTVADC(SV_VS_CVBS1);
	//mtk01140 20090902 remove to prevent call too early will get wrong DRAM address
    //vDrvTDCOnOff(SV_ON);
}

// =>
/**
 * @brief Set SCART SVDO
 * Set SCART
 * @param void
 */
void vScartSvConnect(void)
{
	#ifndef CC_SUPPORT_SIF_BYPASS
	UINT8 bCh;
	#endif
	vIO32WriteFldMulti(SVF_02, P_Fld(0,SVFBST_HDTV)|P_Fld(0,SVFBST_FIR));
   #if !TVD_NEW_BST_DET_TH
    vIO32WriteFldAlign(SVF_03, 0x8, SVF_BSTDET_TH);
   #endif
	if(bINT_BST_DET==1) //Internal Path
	{
     		#ifdef CC_SUPPORT_SIF_BYPASS
     			vScartSVInRFE(ENABLE);
     		#else
			for (bCh=0; bCh<3; bCh++)
			{
        			vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),0,RG_CLAMP_GATE_1 );
			}
     		#endif
     }
    //TVD ch1 clamp on middle; s-video mode
    vDrvSetTVADC(SV_VS_S1);
    //fix mixed mode to SV , tdc on problem.

    vDrvTDCOnOff(SV_OFF);


}
#endif


/********************************************************/
/***************  Adaptive Slicer  **********************/
/********************************************************/

#if ADAPTIVE_SLICER_TSET

static HAL_TIME_T cur_time[20];
UINT16 wAS_CV_totalMeasure(void);
UINT16 wASV_MONtotalMeasure(void);

/**
 * @brief void vResetVLen(void)
 * Reset Vlen counter when losing V sync for VGA
 * @param	None
 * @retval	None
 */
void vResetVLenSP0(void)
{
	vIO32WriteFldAlign(ASYNC_02, 0x1, AS_HLEN_VLEN_RESET);   // Set HVLEN_RESET
	vUtDelay2us(10);
	vIO32WriteFldAlign(ASYNC_02, 0x0, AS_HLEN_VLEN_RESET);  // Disable HVLEN_RESET
}

/**
 * @brief void vResetVLen(void)
 * Reset Vlen counter when losing V sync for VGA
 * @param	None
 * @retval	None
 */
void vResetVLenSP2(void)
{
	vIO32WriteFldAlign(ASYNC_02, 0x1, AS_HLEN_VLEN_RESET_SP2);   // Set HVLEN_RESET in SP2
	vUtDelay2us(10);
	vIO32WriteFldAlign(ASYNC_02, 0x0, AS_HLEN_VLEN_RESET_SP2);   // Disable HVLEN_RESET in SP2
}

/**
 * @brief Return Monitor Slicer  V-Length
 * @param void
 */
UINT16 wASV_MONtotalMeasure(void)
{
	UINT16 measure;
    measure = IO32ReadFldAlign(STA_SYNC2_01, AS2_V_LEN_S);//MT5360B has the same address with MT5360A
	return (measure);
}

/**
 * @brief Return composite sync V-Length in Main Slicer
 *
 * @param void
 */
UINT16 wAS_CV_totalMeasure(void)
{
	UINT16 measure;
	measure = IO32ReadFldAlign(STA_SYNC0_00, AS_V_LEN_S);
	return (measure);
}

int VFErateXX(UINT8 low,UINT8 high,UINT16 vlen,UINT16 target)
{
	/* 272 & 271 */
	INT16 min,max;
	if ((vlen<200)||(vlen>2000))
		return 0;

//	if((low==8)&&(high==8))
//		DBG_Printf(VGA_Debug,"8888888888888:vlen=%d target=%d\n",vlen,target);

	if (bHdtvInterlace())
	{
		min=target-1;
		max=target+1;
	}
	else
	{
		min=target;
		max=target;
	}
	if (min<0)
	{
		min=0;
	}
	if (max>2000)
	{
		max=2000;
	}
	if ((vlen>=min)&&(vlen<=max))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


UINT8 VFEverC_slicer[16*16]={
                                0x00,
                            };

void vVFESLT_slicer_test_verC(void)
{
	int testok=1;
	int time_esplase;
	int i,j;

	for (i=0;i<16;i++)
		for (j=0;j<16;j++)
			VFEverC_slicer[16*i+j]=(i<<4)|j;

	LOG(1,"#SLT:[  ]  [#00] [#01] [#02] [#03] [#04] [#05] [#06] [#07] [#08] [#09] [#10] [#11] [#12] [#13] [#14] [#15]\n");
	for (i=0;i<sizeof(VFEverC_slicer);i++)
	{
		UINT16 r_vlen,target_len;
		HAL_GetTime(&cur_time[0]);

		if ((i%16)==0)
			LOG(1,"#SLT:[%2d] ",i/16);
             //vIO32WriteFldMulti(SyncSlice_Control_01, P_Fld((VFEverC_slicer[i]>>3), VTL_SEL)|P_Fld((VFEverC_slicer[i]), VTH_SEL));
             vIO32WriteFldMulti(PDWNC_VGACFG4,  P_Fld((VFEverC_slicer[i]>>3), FLD_RG_SYNC1_VTL)|P_Fld((VFEverC_slicer[i]), FLD_RG_SYNC1_VTH));
		vResetVLenSP0();
		do
		{
			HAL_GetTime(&cur_time[1]);
			time_esplase=((cur_time[1].u4Seconds-cur_time[0].u4Seconds)*1000000)+
			             (cur_time[1].u4Micros-cur_time[0].u4Micros);
		}
		while ( (time_esplase<50000));
		HAL_GetTime(&cur_time[0]);
		target_len=wAS_CV_totalMeasure();
		testok=1;
		do
		{
			r_vlen=wAS_CV_totalMeasure();
			testok=VFErateXX(VFEverC_slicer[i]>>4,VFEverC_slicer[i]&0xf,r_vlen,target_len);
			HAL_GetTime(&cur_time[1]);
			time_esplase=((cur_time[1].u4Seconds-cur_time[0].u4Seconds)*1000000)+
			             (cur_time[1].u4Micros-cur_time[0].u4Micros);
		}
		while ( (time_esplase<200000)&&(testok));
		if (testok)
			//LOG(3," [%x@%x]",VFEverC_slicer[i]>>4,VFEverC_slicer[i]&0xf);
			LOG(1," [%3x]",r_vlen);
		else
			LOG(1," /%3x/",r_vlen);

		if ((i%16)==15)
			LOG(1,"\n");
	}

	//*******************************************************************

#if 1
	//LOG(3,"#SLT: ---%d---\n",bSLT_port);
	LOG(1,"#SLT:[  ]  [#00] [#01] [#02] [#03] [#04] [#05] [#06] [#07] [#08] [#09] [#10] [#11] [#12] [#13] [#14] [#15]\n");
	for (i=0;i<sizeof(VFEverC_slicer);i++)
	{
		UINT16 r_vlen,target_len;
		HAL_GetTime(&cur_time[0]);
		if ((i%16)==0)
			LOG(1,"#SLT:[%2d] ",i/16);
		vIO32WriteFldMulti(PDWNC_VGACFG4, P_Fld((VFEverC_slicer[i]>>3), FLD_RG_SYNC2_VTL)|P_Fld((VFEverC_slicer[i]), FLD_RG_SYNC2_VTH));
		vResetVLenSP2();
		do
		{
			HAL_GetTime(&cur_time[1]);
			time_esplase=((cur_time[1].u4Seconds-cur_time[0].u4Seconds)*1000000)+
			             (cur_time[1].u4Micros-cur_time[0].u4Micros);
		}
		while ( (time_esplase<50000));
		HAL_GetTime(&cur_time[0]);
		target_len=wASV_MONtotalMeasure();
		testok=1;
		do
		{
			r_vlen=wASV_MONtotalMeasure();
			testok=VFErateXX(VFEverC_slicer[i]>>4,VFEverC_slicer[i]&0xf,r_vlen,target_len);
			HAL_GetTime(&cur_time[1]);
			time_esplase=((cur_time[1].u4Seconds-cur_time[0].u4Seconds)*1000000)+
			             (cur_time[1].u4Micros-cur_time[0].u4Micros);
		}
		while ( (time_esplase<200000)&&(testok));
		if (testok)
			LOG(1," [%3x]",r_vlen);
		else
			LOG(1," /%3x/",r_vlen);

		if ((i%16)==15)
			LOG(1,"\n");
	}
#endif
	//Change PGA setting
             //vIO32WriteFldMulti(SyncSlice_Control_01, P_Fld(8,  VTL_SEL)|P_Fld(8,  VTH_SEL));
             vIO32WriteFldMulti(PDWNC_VGACFG4, P_Fld(0x09, FLD_RG_SYNC1_VTL)|P_Fld(8, FLD_RG_SYNC1_VTH));
}
#endif

void vQuerySlicer(void)
{
   #ifndef CC_UP8032_ATV
    //DBG_Printf(VGA_Debug,"slicer Low/High %d/%d\n", IO32ReadFldAlign(SyncSlice_Control_01,  VTL_SEL), IO32ReadFldAlign(SyncSlice_Control_01,  VTH_SEL));
    DBG_Printf(VGA_Debug,"slicer Low/High %d/%d\n", IO32ReadFldAlign(PDWNC_VGACFG4,  FLD_RG_SYNC1_VTL), IO32ReadFldAlign(PDWNC_VGACFG4,  FLD_RG_SYNC1_VTH));
   #endif

}

#if SUPPORT_SET_SLICER
PRIVATE void vSetSlicer(UINT8 pair)//wu add
{
        //vIO32WriteFldAlign(SyncSlice_Control_01, pair,VTLH_SEL);
        //vIO32WriteFldAlign(PDWNC_SOGYCFG,pair,FLD_PD_SOGY_VTLH);
       vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld((pair & 0xF0) >>4,FLD_RG_SYNC1_VTL)|P_Fld((pair & 0x0F),FLD_RG_SYNC1_VTH));
}
void vSetSOGSlicer(UINT8 pair)//wu add
{
    vSetSlicer(pair);
}
#endif


#if ADAPTIVE_SLICER_ENABLE
UINT8 check_quaity_state;
#define SLICER_LV_LIST_NUM 5 // 4
#if ADAPTIVE_MONITOR_SLICER_MEASURE
//PRIVATE UINT8 code  MonSlicerSetting[] ={0x41, 0x41, 0xC9, 0xC9, 0x63}; // {0x9c, 0x14, 0x9C, 0x9C, 0x36};
PRIVATE UINT8 code  MonSlicerSetting[] ={0xAA, 0xAA, 0xAA, 0x43, 0x65}; // {0x9c, 0x14, 0x9C, 0x9C, 0x36};//HL
#endif

PRIVATE void vSetSlicer(UINT8 pair)
{
    if(bHdtvOpt05_AdaptiveSlicer)
    {
        //vIO32WriteFldAlign(SyncSlice_Control_01, pair,VTLH_SEL);
        //vIO32WriteFldAlign(PDWNC_SOGYCFG,pair,FLD_PD_SOGY_VTLH);
        vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld((pair & 0xF0) >>4,FLD_RG_SYNC1_VTL)|P_Fld((pair & 0x0F),FLD_RG_SYNC1_VTH));
        //DBG_Printf(VGA_Debug,"set slicer High/Low 0x%x\n", pair);
        DBG_Printf(VGA_Debug,"set slicer Low/High 0x%x\n", pair);
       #if ADAPTIVE_MONITOR_SLICER_MEASURE
        vSetMONSlicer_Matrix();
       #endif
    }
}

#if ADAPTIVE_MONITOR_SLICER_MEASURE
void vSetMONSlicer_Matrix(void)
{
    if (_bCurSlicerIdx>=SLICER_LV_LIST_NUM)
    {
        return;
    }
    vSetMONSlicer(MonSlicerSetting[_bCurSlicerIdx]);
}
#endif


UINT8 _bCurSlicerIdx=0 ;
#if ADAPTIVE_MONITOR_SLICER_MEASURE_ONLINE
UINT8 _bCurSlicerIdx_best=0 ;
#endif

void vSetDefaultSlicer(void)
{
    //DBG_Printf(VGA_Debug,"set slicer in DefaultSlicer\n");
    #if (defined(ADAPTIVE_SLICER_DEFAULT_SETTING) || defined(ADAPTIVE_MONITOR_SLICER_MEASURE))
    _bCurSlicerIdx = 0;
    #endif
    #if ADAPTIVE_MONITOR_SLICER_MEASURE_ONLINE
    _bCurSlicerIdx_best= 0;
    #endif
    vSetSlicer(0x88);  // 0x88
}

PRIVATE void check_quaity(UINT16 vlen,UINT8 *trylist, UINT8 trynr, UINT32 ms_delay, UINT32 ms_delay2, UINT8 h_prot)
{
    static	UINT8 i;
    static	INT32 bestdiff;
    static	HAL_TIME_T t_init;
    static	UINT8 bestone;
    static  INT32 ai4DiffScore[SLICER_LV_LIST_NUM];
    static  UINT8 bestlevel;
    UINT8 bTmpCnt;
    INT32 diff;
    HAL_TIME_T t_end;
    UINT32 time_esplase;

	if((!vlen)||(!trynr))
	{
		if(trylist)
			vSetSlicer(trylist[0]);
		check_quaity_state=10;
		return;
	}

	ms_delay=ms_delay*1000;
	ms_delay2=ms_delay2*1000;
	switch(check_quaity_state)
	{
		case 0:
			i=0;
			bestone=trylist[0];
			bestdiff=0xffff;
			check_quaity_state++;
			break;
		case 1:
                     //DBG_Printf(VGA_Debug,"set slicer in Fine tune\n");
			vSetSlicer(trylist[i]);
			vResetVLen();
			HAL_GetTime(&t_init);
			check_quaity_state++;
			break;
		case 2:
			HAL_GetTime(&t_end);
			time_esplase=((t_end.u4Seconds-t_init.u4Seconds)*1000000)+
			(t_end.u4Micros-t_init.u4Micros);
			if(time_esplase>=ms_delay)
			{
				HAL_GetTime(&t_init);
				check_quaity_state++;
			}
			break;
		case 3:
			HAL_GetTime(&t_end);
			diff=vlen-wASVtotalMeasure();
			if(diff<0) diff=-diff;

			ai4DiffScore[i] = diff;
			//DBG_Printf(VGA_Debug, "Slicer[%d] Diff = %x\n", i, diff);
			if(diff>=2)
			{
				if(diff<bestdiff)
				{
					bestone=trylist[i];
					bestdiff=diff;
					bestlevel=i;
				}
				i++;
				if(i<trynr)
				{
					check_quaity_state=1;
				}
				else
				{
					check_quaity_state=4;
				}
				break;
			}
			time_esplase=((t_end.u4Seconds-t_init.u4Seconds)*1000000)+
				(t_end.u4Micros-t_init.u4Micros);
			if(time_esplase>=ms_delay2)
			{
				bestone=trylist[i];
				bestlevel=i;
               #if ADAPTIVE_MONITOR_SLICER_MEASURE_ONLINE
                _bCurSlicerIdx_best=bestlevel;
               #endif
				check_quaity_state=4;
			}
			break;
		case 4:
                     //DBG_Printf(VGA_Debug, "Final_at_case 4_Slicer[%d], level = 0x%x,  Diff = %x\n", i, trylist[i],  ai4DiffScore[i]);
                     //DBG_Printf(VGA_Debug, "best_before_center_Slicer[%d], level = 0x%x,  Diff = %x\n", bestlevel, trylist[bestlevel],  ai4DiffScore[bestlevel]);
			for(bTmpCnt=0;bTmpCnt<i;bTmpCnt++)
			{
			    if(ai4DiffScore[bestlevel] >= ai4DiffScore[bTmpCnt])
			    {
			        /* bTmpCnt is better or equal to the bestone */
			        if(DIFF(bestlevel, trynr/2) > DIFF(bTmpCnt, trynr/2))
			        {
			            /* bTmpCnt is near center of the list */
			            bestlevel = bTmpCnt;
			            bestone = trylist[bTmpCnt];
			            //DBG_Printf(VGA_Debug,"Center_Slicer = 0x%x\n", bestone);
			        }
			    }
			}
                     //DBG_Printf(VGA_Debug,"final choose bestone in fine tune = 0x%x\n", bestone);
			vSetSlicer(bestone);

			check_quaity_state=10;
			break;
	}

}

typedef struct slicerSetting_s
{
    UINT8 bCoarse;
    UINT8 abVReduce[4];
}slicerSetting_t;


PRIVATE slicerSetting_t code rSlicerSetting[] =
{
    //0x96, {0x96,0xA7,0xB8,0xB9}, // 0x58, {0x58,0x69,0x79,0x89}, // 0xa8, {0x98,0xa8,0xb9,0xc9},
    //0xC9, {0xC9,0xDA,0xEB,0xFC}, // 0x79, {0x69,0x79,0x89,0x9A}, // 0xb9, {0xa8,0xb9,0xc9,0xca},
    //0x63, {0x63,0x74,0x85,0x96},
    //0x51, {0x51,0x61,0x72,0x83}, // 0x04, {0x04,0x15,0x26,0x37},  // 480i HV Reduce
    //0xEB, {0xEB,0xEC,0xED,0xFC}, // 0xCF, {0xCF,0xDF,0xCE,0xDE},  // 720p60 V Reduce
    //LH
    {0x88, {0x88,0x99,0x9A,0xAA}}, // 0x58, {0x58,0x69,0x79,0x89}, // 0xa8, {0x98,0xa8,0xb9,0xc9},
    {0x35, {0x35,0x36,0x47,0x58}}, // 0x04, {0x04,0x15,0x26,0x37},  // 480i HV Reduce
    {0x7B, {0x7B,0x8C,0x9D,0xAE}}, // 0x79, {0x69,0x79,0x89,0x9A}, // 0xb9, {0xa8,0xb9,0xc9,0xca},
    {0x56, {0x56,0x67,0x78,0x88}},
    {0xCE, {0xCE,0xDE,0xDF,0xEF}}, // 0xCF, {0xCF,0xDF,0xCE,0xDE},  // 720p60 V Reduce
    //{0x69, {0x69,0x7A,0x8B,0x9B}}, // 0x58, {0x58,0x69,0x79,0x89}, // 0xa8, {0x98,0xa8,0xb9,0xc9},
    //{0x9C, {0x9C,0xAD,0xBE,0xCF}}, // 0x79, {0x69,0x79,0x89,0x9A}, // 0xb9, {0xa8,0xb9,0xc9,0xca},
    //{0x36, {0x36,0x47,0x58,0x69}},
    //{0x15, {0x15,0x16,0x27,0x38}}, // 0x04, {0x04,0x15,0x26,0x37},  // 480i HV Reduce
   // {0xBE, {0xBE,0xCE,0xDE,0xCF}}, // 0xCF, {0xCF,0xDF,0xCE,0xDE},  // 720p60 V Reduce
};

//backup level
/*
    0x88, {0x78,0x88,0x98,0xa8},
    0x98, {0x88,0x98,0xa8,0xb9},
    0xa8, {0x98,0xa8,0xb9,0xc9},
    0xb9, {0xa8,0xb9,0xc9,0xca},
*/
/*
PRIVATE UINT8 SlicerCoarse[]={0x88,0x98,0xA8,0xB9};
PRIVATE UINT8 SlicerVReduce[][]={{0x88,0x98,0xA8,0xB9},
	                                                                               {0x88,0x98,0xA8,0xB9},
	                                                                               {0x88,0x98,0xA8,0xB9},
	                                                                               {0x88,0x98,0xA8,0xB9}
                                                                                 };*/
void  vSliceQuality536x(void)
{
    UINT8 *list;

    list = rSlicerSetting[(_bCurSlicerIdx)%(sizeof(rSlicerSetting)/sizeof(slicerSetting_t))].abVReduce;

    switch(_bHdtvTiming)
    {
        case MODE_525I:
        case MODE_525I_OVERSAMPLE:
            check_quaity(263,list,4,40,360,1);
        break;
        case MODE_625I:
        case MODE_625I_OVERSAMPLE:
            check_quaity(313,list,4,50,440,1);
        break;
        case MODE_480P:
        case MODE_480P_OVERSAMPLE:
            check_quaity(525,list,4,40,360,1);
        break;
        case MODE_576P:
        case MODE_576P_OVERSAMPLE:
            check_quaity(625,list,4,50,440,1);
        break;
        case MODE_720p_50:
            check_quaity(750,list,4,50,440,0);//0x2f3
        break;
        case MODE_720p_60:
            check_quaity(750,list,4,40,360,0);//0x2f3
        break;
        case MODE_1080i_50:
            check_quaity(563,list,4,50,440,0);//0x243
        break;
        case MODE_1080i:
            check_quaity(563,list,4,40,360,0);//0x243
        break;
        case MODE_1080p_50:
            check_quaity(1125,list,4,50,440,0);//0x46a
        break;
        case MODE_1080p_60:
            check_quaity(1125,list,4,40,360,0);//0x46a
        break;
        default:
        /*	MODE_1080i_48,
        MODE_1080p_24,
        MODE_1080p_25,
        MODE_1080p_30,  */
            check_quaity(0,list,4,50,110,0);
        break;

        case MODE_NOSIGNAL:
        case MODE_NODISPLAY:
        case MODE_NOSUPPORT:
        case MODE_WAIT:
        return;
    }
}


HAL_TIME_T rPreSliceTime, rDeltaSliceTime, _rCurSliceTime;
void vResetSliceTimer(void)
{
    if(bHdtvOpt05_AdaptiveSlicer)
    {
        HAL_GetTime(&rPreSliceTime);
        //DBG_Printf(VGA_Debug,"set slicer in ResetSliceTimer\n");
        //vSetDefaultSlicer();
        /* default level index */
        //_bCurSlicerIdx = 0;//reset slicer level to index 0 to match default setting
    }
}

void vNextSlicer(UINT16 wTimeout)
{
    if(bHdtvOpt05_AdaptiveSlicer)
    {
        UINT32 u4MilliSec;

        HAL_GetTime(&_rCurSliceTime);
        HAL_GetDeltaTime(&rDeltaSliceTime, &_rCurSliceTime, &rPreSliceTime);
        u4MilliSec = (rDeltaSliceTime.u4Seconds * 1000) + (rDeltaSliceTime.u4Micros / 1000);
        if(u4MilliSec >= wTimeout)
        {
            HAL_GetTime(&rPreSliceTime);
            _bCurSlicerIdx++;
           #if ADAPTIVE_MONITOR_SLICER_MEASURE
            if (_bCurSlicerIdx>=5)
            {
                _bCurSlicerIdx=0;
            }
           #endif
            //DBG_Printf(VGA_Debug,"set slicer in Coarse tune\n");
            vSetSlicer(rSlicerSetting[(_bCurSlicerIdx)%(sizeof(rSlicerSetting)/sizeof(slicerSetting_t))].bCoarse);

        }
    }
}


#endif

#if ADAPTIVE_MONITOR_SLICER_MEASURE
void vSetMONSlicer(UINT8 pair)
{
    if(bHdtvOpt05_AdaptiveSlicer)
    {
        vIO32WriteFldAlign(PDWNC_VGACFG4, pair, FLD_RG_SYNC2_VHLSEL);
        DBG_Printf(DBG_SLICER,"[SA7] set MON slicer High/Low = 0x%x \n",pair);
    }
}

#if ADAPTIVE_MONITOR_SLICER_MEASURE_DEBUG
UINT8 bReadMONSlicer(void)
{
    return(IO32ReadFldAlign(PDWNC_VGACFG4, FLD_RG_SYNC2_VHLSEL));
}

UINT8 bReadNewSlicer(void)
{
	UINT8 pair;
	pair=IO32ReadFldAlign(PDWNC_VGACFG4,FLD_RG_SYNC1_VTL)<<4 |IO32ReadFldAlign(PDWNC_VGACFG4,FLD_RG_SYNC1_VTH);
    //return(IO32ReadFldAlign(SyncSlice_Control_01,VTLH_SEL));
    //return(IO32ReadFldAlign(PDWNC_SOGYCFG,FLD_PD_SOGY_VTLH));
    return(pair);
}
#endif
#endif

//0403
UINT16 wDrvReadADCBlank(void) //read 32 bit???
{
     UINT16 wVAL;
     vIO32WriteFldAlign(DFE_19, 1, DFE_STATUS_CTRL);
     wVAL=u2IO32Read2B(DFE_STA_02);
     vIO32WriteFldAlign(DFE_19, 0, DFE_STATUS_CTRL);
     //wVAL=wReadVIN2B(DFE_STA_02, 1, 0);//moving average blk
     return wVAL;
}

//0403
UINT16 wDrvReadADCBlankAve(void)
{
             UINT16 i,wVAL;
             UINT32 dAVE=0;
             for (i=0; i<8; i++)
             {
                 vIO32WriteFldAlign(DFE_19, 1, DFE_STATUS_CTRL);
                    vUtDelay2us(25);
                 dAVE +=IO32ReadFldAlign(DFE_STA_01,BLANK_LV);
                 vIO32WriteFldAlign(DFE_19, 0, DFE_STATUS_CTRL);
             }
             wVAL=dAVE>>3;

             return wVAL;
}



//0403
void vDrvSetPGAMauGainAGC2(void)
{
     vIO32WriteFldAlign(DFE_19, 1, AGC_SEL);
     vIO32WriteFldAlign(DFE_03, 6, AGC2_MODE);
}

//0403
 void vDrvPGAMaxGainMauAGC2(void)
{
     vIO32Write1BMsk(DFE_07_2, 0x7f, 0x7f);
     vIO32Write2B(DFE_07, 0x0800);
}

//0403
void vDrvPGAMidGainMauAGC2(void)
{
     vIO32Write1BMsk(DFE_07_2, 0x3a, 0x7f);
     vIO32Write2B(DFE_07, 0x0800);
}

//0403
void vDrvPGAMinGainMauAGC2(void)
{
     vIO32Write1BMsk(DFE_07_2, 0x07, 0x7f);
     vIO32Write2B(DFE_07, 0x0800);
}

//0403
void vDrvSetPGAAutoGainAGC2(void)
{
    vIO32WriteFldAlign(DFE_19, 1, AGC_SEL);
    vIO32WriteFldAlign(DFE_03, 8, AGC2_MODE);
}

//0403
void vDrvChACalonBlankEnb(void)
{
         vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_BLANK_EN);//
        // vIO32WriteFldAlign(REG_CVBS_CFG0, 1, RG_VAGSELA);//
         vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_CHA_MIDDLE_EN);//
         vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTA_PWD);//
         vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_AISEL);//
         vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_AISELR);//
     vDrvSetPGAMauGainAGC2();//set manul gain mode in AGC2
}


//0403
void vDrvChACalonBlankDsb(void)
{
      //recovery the setting of chA in "vDrvTVADCDefaultSetting() ==> operation in clamping on blank"
         vIO32WriteFldAlign(REG_CVBS_CFG0, 0, RG_AISEL);//
         vIO32WriteFldAlign(REG_CVBS_CFG0, 0x0110, RG_AISELR);//
         vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_SHIFTA_PWD);////

     vDrvSetPGAAutoGainAGC2();//auto gain in AGC2
}


/*
 * @brief  Input Offset Calibration function
 * @param bCH, like CHA
 * @param bCLAMP, like CLAMP_ON_BLANK\CLAMP_ON_BOTTOM
 * @clamping on blank for CHA usually
 */
void vDrvInputOffsetCal(void)
{
       //UINT8 i=0;
	//UINT8 bCALCLAMP = 0x00; //initial value
	UINT8 bOFFCUR = 0x10; //initial value
	UINT16  iA=0, iB=0, iC=0, iD=0;
	//BOOL fgRet;
       UINT32 u4Delay = 5;//20 save time ;//for lint


	// **************************
	//Input offset Calibration START
	// **************************
	//bCALCLAMP=0x0f;//mt5360, 15->0, slope = positive
       /*mark for V2*/ //vIO32WriteFldAlign(VFE_E1, bCALCLAMP, RG_CALCLAMPA); //
       vIO32WriteFldAlign(REG_CVBS_CFG1, bOFFCUR, RG_OFFCURA);//


       #if 1 //new method of similar triangle to look for cross point .....kal
            vDrvPGAMinGainMauAGC2();
            vUtDelay1ms(u4Delay);
            iA= wDrvReadADCBlankAve();
            vUtDelay1ms(u4Delay);
            iB= wDrvReadADCBlankAve();

            vDrvPGAMaxGainMauAGC2();
            vUtDelay1ms(u4Delay);
            iC= wDrvReadADCBlankAve();
            vUtDelay1ms(u4Delay);
            iD= wDrvReadADCBlankAve();

            if((iA>iC)&&(iD>iB)&&(iB>=iA)&&(iD>iC))
            {
                //bCALCLAMP = (iD-iB)*15/(iA+iD-iC-iB);
            }
            else
            {
                //bCALCLAMP = 0x07;//default value

                #ifdef __MODEL_slt__
                fgCVBSFail = TRUE;
	         #endif
             }


       #endif

}
//0403
void vDrvChACalonblank(void)
{
    vDrvChACalonBlankEnb();
    vDrvDFE(256);
    vDrvChACalonBlankDsb();

}

//////////////////////////////////////////////////////////////////////
// Public Function
//////////////////////////////////////////////////////////////////////
void vDrvCvbsAdcCal(void)
{
          vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNA_PWD);
          vIO32WriteFldAlign(REG_CVBS_CFG3, 1, RG_PGABUFNB_PWD);
       vDrvChACalonblank();
          vIO32WriteFldAlign(REG_CVBS_CFG3, 0, RG_PGABUFNB_PWD);
}


void vDrvCVBSOffsetTest(void)
{
    //vIO32WriteFldAlign(VFE_E1, 0x07, RG_CALCLAMPA);//recover to default setting
    //vIO32WriteFldAlign(VFE_E1, 0x07, RG_CALCLAMPB);//recover to default setting
    //vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_OFFCURA);//recover to default setting
    //vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_OFFCURB);//recover to default setting
    //vDrvInputOffsetCal();
    //vDrvOutputOffsetCalNew(256);
}


/*
 * @brief Output Offset Calibration function
 * @param bCH, like CHA
 * @param bCLAMP, like CLAMP_ON_BLANK\CLAMP_ON_BOTTOM
 */

void vDrvOutputOffsetCalNew(UINT16 wCLAMP)
{
	UINT8 bOFFCUR = 0x00; //output initial center value
	UINT16  iA=0, iB=0;
       UINT32 u4Delay = 5;


	// **************************
	//Input offset Calibration START
	// **************************
       //new method of similar triangle to look for cross point .....kal
       vIO32WriteFldAlign(REG_CVBS_CFG1, 0x1F, RG_OFFCURA);//
       vUtDelay1ms(u4Delay);
       iA= wDrvReadADCBlankAve();
       vIO32WriteFldAlign(REG_CVBS_CFG1, 0x0F, RG_OFFCURA);//
       vUtDelay1ms(u4Delay);
       iB= wDrvReadADCBlankAve();

       if((iA>iB)&&(iA>wCLAMP)&&(wCLAMP>iB))
       {
           bOFFCUR = 30*(wCLAMP-iB)/(iA-iB);
           if(bOFFCUR>=15)
	    {
             bOFFCUR = bOFFCUR-15;
             bOFFCUR = (bOFFCUR | 0x10);
	    }
           else
           {
             bOFFCUR = 15-bOFFCUR;
           }
       }
       else
	{
           bOFFCUR = 0x00;//default value

           #ifdef __MODEL_slt__
            fgCVBSFail = TRUE;
	    #endif
	}

       vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_OFFCURA);////MT536X //recover to default setting
       vIO32WriteFldAlign(REG_CVBS_CFG1, 0x00, RG_OFFCURB);////MT536X //recover to default setting

}

//--------- To get the auto color gain and offset
/**
 * @brief  Read  3 channel Gain of VGA  from EEPROM
 *
 * @param None
 */
UINT16 vCust_EEPROM_VGA_B_Gain_Read(void)
{
    UINT16 bGain;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA0_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA0_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    		bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_VGA_GAIN0);
    #endif
    return (bGain);
}
UINT16 vCust_EEPROM_VGA_G_Gain_Read(void)
{
    UINT16 bGain;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA1_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA1_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    		bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_VGA_GAIN1);
    #endif
    return (bGain);
}
UINT16 vCust_EEPROM_VGA_R_Gain_Read(void)
{
    UINT16 bGain;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA2_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_VGA2_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    		bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_VGA_GAIN2);
     #endif
   return (bGain);
}
UINT16 vCust_EEPROM_VGA_B_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP0);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP0_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
   bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP0);
    #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_VGA_G_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP1);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP1_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP1);
     #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_VGA_R_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP2);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP2_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_VGA_CLAMP2);
     #endif
    return (bOffset);
}


/**
 * @brief  Read  3 channel Gain of COMPONENT  from EEPROM
 *
 * @param None
 */
UINT16 vCust_EEPROM_CMP_Y_Gain_Read(void)
{
    UINT16 bGain;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV0_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV0_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_HDTV_GAIN0);
    #endif
    return (bGain);
}
UINT16 vCust_EEPROM_CMP_PB_Gain_Read(void)
{
    UINT16 bGain;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV1_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV1_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_HDTV_GAIN1);
    #endif
    return (bGain);
}
UINT16 vCust_EEPROM_CMP_PR_Gain_Read(void)
{
    UINT16 bGain;
     #ifdef AUTOCOLOR_NEW_METHOD
   UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV2_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_HDTV2_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_HDTV_GAIN2);
    #endif
    return (bGain);
}

UINT16 vCust_EEPROM_CMP_Y_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP0);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP0_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP0);
     #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_CMP_PB_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP1);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP1_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP1);
     #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_CMP_PR_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP2);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP2_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_HDTV_CLAMP2);
     #endif
    return (bOffset);
}


/**
 * @brief  Read  RGB Gain of SCART from EEPROM
 *
 * @param None
 */
 UINT16 vCust_EEPROM_SCART_B_Gain_Read(void)
{
    UINT16 bGain;
     #ifdef AUTOCOLOR_NEW_METHOD
   UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART0_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART0_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_SCART_GAIN0);
   #endif
    return (bGain);
}
UINT16 vCust_EEPROM_SCART_G_Gain_Read(void)
{
    UINT16 bGain;
     #ifdef AUTOCOLOR_NEW_METHOD
   UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART1_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART1_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_SCART_GAIN1);
  #endif
    return (bGain);
}
UINT16 vCust_EEPROM_SCART_R_Gain_Read(void)
{
    UINT16 bGain;
     #ifdef AUTOCOLOR_NEW_METHOD
   UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART2_LOW_BYTE);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_DIG_GAIN_SCART2_HIGH_BYTE);
                bGain=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
    #else
    bGain = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_SCART_GAIN2);
 #endif
    return (bGain);
}
UINT16 vCust_EEPROM_SCART_B_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP0);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP0_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP0);
 #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_SCART_G_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP1);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP1_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP1);
  #endif
    return (bOffset);
}
UINT16 vCust_EEPROM_SCART_R_Offset_Read(void)
{
    UINT16 bOffset;
    #ifdef AUTOCOLOR_NEW_METHOD
    UINT8  bTmpLowByte,bTmpHighByte;
                bTmpLowByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP2);
                bTmpHighByte= bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP2_HIGH_BYTE);
                bOffset=  (((UINT16)bTmpHighByte<<8)&0xFF00)|(UINT16) bTmpLowByte;
     #else
    bOffset = bApiEepromReadByte(EEP_ADC_OFFSET_SCART_CLAMP2);
 #endif
    return (bOffset);
}


/**
 * @brief  Read  auto color status for VGA, HDTV, and SCART
 *
 * @param None
 */
UINT8 vCust_EEPROM_CMP_STATUS(void)
{
    UINT8 bStatus;
    bStatus = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_HDTV_DONE);
    return (bStatus);
}
UINT8 vCust_EEPROM_VGA_STATUS(void)
{
    UINT8 bStatus;
    bStatus = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_VGA_DONE);
    return (bStatus);
}
UINT8 vCust_EEPROM_SCART_STATUS(void)
{
    UINT8 bStatus;
    bStatus = bApiEepromReadByte(EEP_VIDEO_AUTO_COLOR_SCART_DONE);
    return (bStatus);
}

//===================================================================
//  VFE GPIO Debug Monitor
//===================================================================
#if VFE_GPIODEBUG
extern INT32 GPIO_Enable(INT32 i4GpioNum, INT32 *pfgSet);
extern INT32 GPIO_Output(INT32 i4GpioNum, INT32 *pfgSet);

void GPIO_H(UINT8 u1GpioNumIdx)
{
    INT32 i4Val, i4Set;

    ASSERT(u1GpioNumIdx<VFE_GPIONum);

    i4Val = 0;
    GPIO_Enable(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Val); // Change to input mode first.
    i4Set = 1;
    GPIO_Output(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Set);
    i4Val = 1;
    GPIO_Enable(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Val); // Change to output mode.
}

void GPIO_L(UINT8 u1GpioNumIdx)
{
    INT32 i4Val, i4Set;

    ASSERT(u1GpioNumIdx<VFE_GPIONum);

    i4Val = 0;
    GPIO_Enable(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Val); // Change to input mode first.
    i4Set = 0;
    GPIO_Output(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Set);
    i4Val = 1;
    GPIO_Enable(VFE_GPIOPINTbl[u1GpioNumIdx], &i4Val); // Change to output mode.
}
#endif /* VFE_GPIODEBUG */
//#endif /* endif  CC_MT5365_V2*/

#ifdef CC_SUPPORT_STR
extern UINT8 _bSCART1Status;
extern UINT8 _bSCART2Status;
void vDrvHDTVADCDefaultSetting_register(void)
{
	UINT8 bCh;
	
    //vIO32WriteFldMulti(PDWNC_SOGYCFG,P_Fld(6,FLD_PD_SOGY_VTL)|P_Fld(1,FLD_SELECT)|P_Fld(0,FLD_PD_SOGY_CAL)|P_Fld(9,FLD_PD_SOGY_VTH));
    vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(8,FLD_RG_SYNC1_VTL)|P_Fld(8,FLD_RG_SYNC1_VTH));
    //vIO32WriteFldMulti(PDWNC_VGACFG4,P_Fld(0,FLD_RG_VMUX_PWD)|P_Fld(1,FLD_RG_DESPIKE)|P_Fld(1,FLD_RG_RGB_EN));  //5360 Michael checked
    vIO32WriteFldMulti(PDWNC_VGACFG0,P_Fld(0,FLD_RG_VMUX_PWD)|P_Fld(1,FLD_RG_DESPIKE)|P_Fld(1,FLD_RG_AUTOBIAS_EN));  //Mt5365/95 kal checked
    vIO32WriteFldMulti(PDWNC_VGACFG1, P_Fld( 0, FLD_RG_SYNC_DESPK_EN)|P_Fld(0,FLD_RG_DIG_TST_EN));  //5360 Michael checked
    vIO32WriteFldMulti(PDWNC_VGACFG3,P_Fld( 0, FLD_RG_SOGY_SORS_PWD)|P_Fld(0, FLD_RG_SOGY_SINK_PWD));//power on
    vIO32WriteFldAlign(ASYNC_0E, 2, AS_C_MAXMIN_FILTER);
    //vIO32WriteFldAlign(REG_VGA_Normal_CFG0, 1, SELF_EN); //5365 mark
          	vIO32WriteFldAlign(REG_CVBS_CFG2, 0, RG_CVBS108M_EN);//disable 108M to VGA.Scart SV C use VGA AD

    vIO32WriteFldMulti(VFE_00, P_Fld(0,ABIST_VGA_DATA_SEL)|P_Fld(0,ABIST_VGA_DOWN_SAMPLE));
    vIO32WriteFldMulti(REG_VGA_Normal_CFG0, P_Fld(0,RG_VDC_N_EN )|P_Fld(0,RG_SHORT_FEO )|P_Fld(0,RG_ADCTEST_EN ));
	vIO32WriteFldMulti(REG_VGA_Normal_CFG8,P_Fld(0,RG_CLKOSEL_3)|P_Fld(0,RG_CLKOSEL_2)|P_Fld(1,RG_CLKOSEL_1)|P_Fld(0,RG_RGB_REV ));
         vIO32WriteFldMulti(REG_VGA_Normal_CFG4, P_Fld(2,RG_VGAADC1_IGBIAS)|P_Fld(2,RG_VGAADC2_IGBIAS)|P_Fld(2,RG_VGAADC3_IGBIAS))
   //vUtDelay1ms(2);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 0,FLD_RG_OFFCUR);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1,FLD_RG_PSAV_REV_BG);
    vIO32WriteFldAlign(PDWNC_VGACFG2, 1,FLD_RG_PSAV_REV_INPUT_PORT);
	for (bCh=0; bCh<3; bCh++)
	{
		//vIO32WriteFldAlign(REG_VGA_Normal_CFG1+16*bCh, 0x00000A0A, REG_VGA_Normal_CFG1_All);   //REG_VGA_Normal_CFG1, REG_VGA_Normal_CFG5, REG_VGA_Normal_CFG9 //20090323 37.5uA->62.5uA for SAG
        vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),1,RG_CLAMP_GATE_1 );
        vIO32WriteFldAlign(REG_VGA_Normal_CFG1+(4*bCh),0,RG_FE_1_PWD );
        vIO32WriteFldMulti(REG_VGA_Normal_CFG1+(4*bCh),P_Fld(1,RG_CLAMP_GATE_1 )|P_Fld(0,RG_FE_1_PWD )|P_Fld(3,RG_COPBIAS_1 )|P_Fld(0,RG_COP_1_PWD ));
	 	vIO32WriteFldMulti(REG_VGA_Normal_CFG5+(4*bCh),P_Fld(1,RG_VGAADC1_CKSEL )|P_Fld(0,RG_VGAADC1_DIV_SEL)
                                                                              |P_Fld(0,RG_VGAADC1_CORE_PWD)|P_Fld(0,RG_VGAADC1_CK_PWD )
                                                                              |P_Fld(0,RG_VGAADC1_PHSEL )|P_Fld(1,RG_VGAADC1_DC_EN )
                                                                              |P_Fld(0,RG_VGAADC1_VSEL_EN )|P_Fld(0,RG_VGAADC1_VSEL )
                                                                              |P_Fld(1,RG_VGAADC1_LSF_EN )|P_Fld(0,RG_VGAADC1_REV0 )
                                                                              |P_Fld(255,RG_VGAADC1_REV1 ));

	}

   // vIO32WriteFldAlign(REG_VGA_Normal_CFG3, 0, TRGSEL);
	vIO32WriteFldMulti(REG_VGA_Normal_CFG4,P_Fld(0,RG_VGAADC_MON_SEL)|P_Fld(0,RG_VGAADC1_IO_PWD)|P_Fld(0,RG_VGAADC2_IO_PWD)|P_Fld(0,RG_VGAADC3_IO_PWD));
    vIO32WriteFldMulti(REG_VGA_Normal_CFG0, P_Fld(0,RG_VREFGEN4FE_PWD )|P_Fld(0,RG_VDC_P_EN )|P_Fld(0,RG_RESSEL )|P_Fld(7,RG_CAP_EN )|P_Fld(1,RG_FE_OFFSET_N ));


    vIO32WriteFldAlign(REG_VGA_Normal_CFG0,0x04,RG_ADCVREFP );
   #if Phase3Channel
    vIO32WriteFldMulti(REG_VGA_Normal_CFG8, P_Fld(1,RG_CLKINV_EN )|P_Fld(1,RG_RELATCH_EN ));
   #else
    vIO32WriteFldMulti(REG_VGA_Normal_CFG8, P_Fld(0,RG_CLKINV_EN )|P_Fld(0,RG_RELATCH_EN ));
   #endif

    vDrvClkInit(); //init for adcpll clk

}


void ADC_pm_resume(void)//please call it after the one who call VOPLL_pm_resume() in the resume fuction, for sdds unlock issue
{
        UINT8 mCvbsSelREG;
        mCvbsSelREG=IO32ReadFldAlign(REG_CVBS_CFG0,RG_AISEL);
		LOG(3, "ADC resume\n");
		_bSCART1Status = 0;
		_bSCART2Status = 0;
		vDrvTVADCDefaultSetting();
        
        vIO32WriteFldAlign(REG_CVBS_CFG0, mCvbsSelREG, RG_AISEL);//
		//vDrvHDTVADCDefaultSetting();//because after resume,sys can not access eeprom,replace it using vDrvHDTVADCDefaultSetting_register;
		vDrvHDTVADCDefaultSetting_register();

}

#endif

void vGetPGAGainInitVal(void)
{
    UINT8 bCh,i,j;
	static UINT8 u1flagEfuseInit = SV_FALSE;
   
    if (u1flagEfuseInit)
		return;
    else  
        {
            u1flagEfuseInit = SV_TRUE;
			Printf("=== init OTP gain\n");
           /////////////////////////////   1. Check Embedded gain  ///////////////////////////////////

          
			if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))& 0x00FFFFFF)==0x00000000)
		    {
		        	Printf("==== OTP fail(empty) !!!!  ===\n");
					return;
		    }
            //////////////////////////////  2. Read Gain From E Fuse   ///////////////////////////////////////////////

            for(bCh=0; bCh<3; bCh++)
            {  //compel Read VGA E Fuse address 0xf0008674

               if(((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>24)==0)
            	{
                _bAutoColorGain[1][bCh] = ((IO32ReadFldAlign(0xf0008674, AC_FULLDW))>>(8*bCh))<<8;//<<8 change to 16bit //+bEFuseCompensation[bCh];
            	}
		else
            	{
                _bAutoColorGain[1][bCh] = ((IO32ReadFldAlign(0xf0008660, AC_FULLDW))>>(8*(bCh+1)))<<8;//<<8 change to 16bit  //+bEFuseCompensation[bCh];
            	}
                _bAutoColorGain[3][bCh] = _bAutoColorGain[1][bCh]; //kaltst add
            }

   
            /////////////////////////////   3. Transfer to  YPbPr, SCART, VGA_YPbPr  ///////////////////////////////////

            if(DRVCUST_OptGet(eVFE_HW_SPECIAL_TARGET))
            {
                for(i=0;i<4;i++)
                    for(j=0;j<3;j++)
                    {
                            if(DRVCUST_OptGet(eVFE_HW_075ohm))
                        wOnChipColorGainTable_Temp[i][j] = wOnChipColorGainTable_75[i][j];
                            else
                        wOnChipColorGainTable_Temp[i][j] = wOnChipColorGainTable[i][j];
   	                }
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA_COMPOENT,INT_VGA);//_bType 3 maps to 1
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA,INT_HDTV);//_bType 1 maps to 0
                    vDrvGeneralAutoColorGainSetToNextType((UINT16 *)wOnChipColorGainTable_Temp,INT_VGA,INT_SCART);//_bType 1 maps to 2
            }
            else
            {
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_HDTV);
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_SCART);
                vDrvEFUSEAutoColorGainSetToNextType(INT_VGA,INT_VGA_COMPOENT);
            }
            //////////////////////////////  4. add compensation gain ///////////////////////////////////////////////
            //for CLI command n.vga.ebgain 0 or 1 read; kal add
            for(i=0;i<4;i++)
   	            for(j=0;j<3;j++)
                {
                   _bAutoColorGain[i][j] = _bAutoColorGain[i][j] + bEFuseCompensation[i][j];
                   _bAutoColorGain_for_EFuse[i][j] = _bAutoColorGain[i][j];
   	            }

  


		}
	}
    

