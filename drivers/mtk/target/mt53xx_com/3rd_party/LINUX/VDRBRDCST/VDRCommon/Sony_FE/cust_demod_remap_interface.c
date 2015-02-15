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
 * $RCSfile: cust_demod_remap_interface.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file cust_common_interface.c
 *  Implement common interface for Sony frontend.
 */
 
#ifdef CC_DEMOD_REMAP_ENABLE   //This is defined in drv_opt/xxx/xxxx.def

#include "PI_Def_dbg.h"
#include "drvapi_tuner.h"
#include "cust_common_interface.h"

#define AEP_PA_T2S2_1_TUNER  0x00
#define AEP_PA_T2S2_2_TUNER  0x01
#define PA_PH_T2_1_TUNER     0x02
#define TW_1_TUNER           0x03
#define CH_HK_1_TUNER        0x04
#define UC_MX_1_TUNER        0x05
#define LA_1_TUNER           0x06
#define LA_2_TUNER           0x07
#define JP_STD_2_TUNER       0x08
#define JP_SKP_2_TUNER       0x09
#define AEP_STD_1_TUNER      0x0a   //Only for Salvia

static SONYPRODUCT_TYPEINFO g_product_typeinfo;

#ifdef CC_SONY_DTMB_MODEL   //This define is from Makefile in current folder
extern INT32 DTMB_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern S32 DVBC_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);

INT32 REMAPDTMB_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if(g_product_typeinfo.modelVariationID == CH_HK_1_TUNER)
    {
        DTMB_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBC_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if((g_product_typeinfo.modelVariationID == CH_HK_1_TUNER) && (g_product_typeinfo.CISlot == 0x01))
    {
        DVBC_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}
#else   //For Sakura & Salvia EU remap
extern INT32 DVBT2_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern INT32 DVBT2_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern INT32 DVBS_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern INT32 DVBS_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern S32 DVBC_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx);
extern S32 DVBT_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo,  PTD_SPECIFIC_CTX* pptTDSpecificCtx);

INT32 REMAPDVBT_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    
    #ifdef CC_SOEM_BOARD    //For Salvia
    if(g_product_typeinfo.modelVariationID == AEP_STD_1_TUNER)
    {
        DVBT_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx); //MTK internal DVBT demodulator
    }
    #endif
    if((g_product_typeinfo.modelVariationID == AEP_PA_T2S2_1_TUNER)||(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER)   \
        ||(g_product_typeinfo.modelVariationID == PA_PH_T2_1_TUNER))
    {
        DVBT2_C1_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBT_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER)
    {
        DVBT2_C2_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBC_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if((g_product_typeinfo.modelVariationID == AEP_PA_T2S2_1_TUNER)||(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER)   \
        ||(g_product_typeinfo.modelVariationID == AEP_STD_1_TUNER)||(g_product_typeinfo.modelVariationID == PA_PH_T2_1_TUNER))
    {
        #ifdef CC_SOEM_BOARD    //For Salvia
        DVBC_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx); //MTK internal DVBC demodulator
        #else
        DVBT2_C1_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
        #endif
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBC_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER)
    {
        DVBT2_C2_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBS_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if((g_product_typeinfo.modelVariationID == AEP_PA_T2S2_1_TUNER)||(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER))
    {
        DVBS_C1_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBS_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    VDRBRDCST_GetSonyProductTypeInfo(&g_product_typeinfo);
    if(g_product_typeinfo.modelVariationID == AEP_PA_T2S2_2_TUNER)
    {
        DVBS_C2_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}
#endif
#endif

