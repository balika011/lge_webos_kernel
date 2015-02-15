/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: cust_common_interface.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file cust_common_interface.c
 *  Implement common interface for Sony frontend.
 */

#include "PI_Def_dbg.h"
#include "drvapi_tuner.h"
#include "tunerDebug.h"
#include "cust_common_interface.h"

#ifdef CC_SOEM_BOARD    //For Salvia
#define DEMOD_TYPE_TC   0x0A
//#define DEMOD_TYPE_T2   0x02
//#define DEMOD_TYPE_T2S2 0x00

#define DEMOD_TYPE_CUSTOM_OFFSET1 0x6CD
#define DEMOD_TYPE_CUSTOM_OFFSET2 0x6CE
#define DEMOD_TYPE_CUSTOM_OFFSET3 0x6CF

void VDRBRDCST_GetSonyProductTypeInfo(SONYPRODUCT_TYPEINFO *pSonyProductTypeInfo)
{
    #if defined(CC_AEP_PA_T2S2_1_TUNER) ||defined(CC_AEP_PA_T2S2_2_TUNER)||defined(CC_PA_PH_T2_1_TUNER)||defined(CC_AEP_STD_1_TUNER)
    UINT8  u1Val[3]={0};
    UINT32  u4Ofst[3];
    UINT8  u1Default = DEMOD_TYPE_TC;
    
    u4Ofst[0] = DEMOD_TYPE_CUSTOM_OFFSET1;
    u4Ofst[1] = DEMOD_TYPE_CUSTOM_OFFSET2;
    u4Ofst[2] = DEMOD_TYPE_CUSTOM_OFFSET3;
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[0], (UINT32)(void *)&u1Val[0], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[1], (UINT32)(void *)&u1Val[1], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[2], (UINT32)(void *)&u1Val[2], 1));    
    if (u1Val[0]==u1Val[1]){
        pSonyProductTypeInfo->modelVariationID = u1Val[0];
    }
    else if(u1Val[0]==u1Val[2]){
        pSonyProductTypeInfo->modelVariationID = u1Val[0];
    }
    else if(u1Val[1]==u1Val[2]){
        pSonyProductTypeInfo->modelVariationID = u1Val[1];
    }
    else{
        pSonyProductTypeInfo->modelVariationID = u1Default;
    }
    #endif

    #ifdef CC_TW_1_TUNER    // These types should be defined in drv_opt/sony/xxx.def
        pSonyProductTypeInfo->modelVariationID = 0x03;
    #endif
    
    #ifdef CC_CH_HK_1_TUNER
    pSonyProductTypeInfo->modelVariationID = 0x04;
    #endif
    
    #ifdef CC_UC_MX_1_TUNER
    pSonyProductTypeInfo->modelVariationID = 0x05;
    #endif
    
    #ifdef CC_LA_1_TUNER
    pSonyProductTypeInfo->modelVariationID = 0x06;
    #endif
    
    #ifdef CC_LA_2_TUNER
    pSonyProductTypeInfo->modelVariationID = 0x07;
    #endif
    
    #ifdef CC_JP_STD_2_TUNER
    pSonyProductTypeInfo->modelVariationID = 0x08;
    #endif
    
    #ifdef CC_JP_SKP_2_TUNER 
	pSonyProductTypeInfo->modelVariationID = 0x09;
    #endif
    
    #ifdef CC_PAA_1_TUNER 
	pSonyProductTypeInfo->modelVariationID = 0x0a;
    #endif

    #ifdef CC_SALVIA_SCT_SUPPORT
    pSonyProductTypeInfo->eBoardVer = BV_SCT;
    #else
    pSonyProductTypeInfo->eBoardVer = BV_CVT;
    #endif
    pSonyProductTypeInfo->eProjectID = 0x01;
}

#else   //for Sakura

#define MVID_EEPROM_ADDRESS 0x10011
//#define BV_EEPROM_ADDRESS   0x10012
#define CI_SLOT_ADDRESS     0x10086

void VDRBRDCST_GetSonyProductTypeInfo(SONYPRODUCT_TYPEINFO *pSonyProductTypeInfo)
{
    UINT8  u1ValMVID = 0xff;
    //UINT8 u1ValBV = 0xff;
    UINT8 u1ValCISlot = 0xff;

    VERIFY(0 == EEPROM_Read(MVID_EEPROM_ADDRESS, (UINT32)(void *)&u1ValMVID, 1));
    //VERIFY(0 == EEPROM_Read(BV_EEPROM_ADDRESS, (UINT32)(void *)&u1ValBV, 1));
    VERIFY(0 == EEPROM_Read(CI_SLOT_ADDRESS, (UINT32)(void *)&u1ValCISlot, 1));
    #if 0
    if(u1ValBV > BV_INVALID)
    {
        pSonyProductTypeInfo->eBoardVer = BV_INVALID;
    }
    else
    {
        pSonyProductTypeInfo->eBoardVer = u1ValBV;
    }
    #endif
    
    #ifdef CC_SAKURA_CVT_SUPPORT
    pSonyProductTypeInfo->eBoardVer = BV_CVT;
    #else
    pSonyProductTypeInfo->eBoardVer = BV_SCT;
    #endif
    pSonyProductTypeInfo->modelVariationID = u1ValMVID;
    pSonyProductTypeInfo->CISlot = u1ValCISlot;
    pSonyProductTypeInfo->eProjectID = 0x00;
    return;
}
#endif

