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
 * $RCSfile: tuner_interface.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file tuner_interface.c
 *  Implement primitive tuner driver interface.
 */

#include "PI_Def_dbg.h"
#include "drvapi_tuner.h"
//#include "tunerDebug.h"

//#ifdef CC_CUSTOMIZE_DVBDEMOD_TYPE_SONY
//demod type list definition
#define DEMOD_TYPE_TC   0x74
#define DEMOD_TYPE_T2   0x75
#define DEMOD_TYPE_S2   0x03
#define DEMOD_TYPE_T2S2 0x76
#define DEMOD_TYPE_ISDBT 0x77
#define DEMOD_TYPE_DTMB 0x78
#define DEMOD_TYPE_DTD 0x79
#define DEMOD_TYPE_T2_C2   0x7A
#define DEMOD_TYPE_T2S2_C2 0x7B
#define DEMOD_TYPE_ISDBT_C2 0x7C
#define DEMOD_TYPE_T2S2_2K 0x7D
#define DEMOD_TYPE_T2S2_C2_2K 0x7E


#define DEMOD_TYPE_CUSTOM_OFFSET1 0x6CD
#define DEMOD_TYPE_CUSTOM_OFFSET2 0x6CE
#define DEMOD_TYPE_CUSTOM_OFFSET3 0x6CF


static BOOL     b_DemodType_init = FALSE;
static UINT8    ui1_DemodType = 0;

extern INT32 DVBT2_TunerOpen_c1(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DVBT2_TunerOpen_c2(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);

extern INT32 DVBT_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
//Xiaohong Add
extern INT32 Vendor_TunerOpen_c1(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 Vendor_TunerOpen_c2(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DTMB_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DTD_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);

//Xiaohong End
extern INT32 DVBC_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);

#ifdef SYS_DVBS_SUPPORT
extern INT32 DVBS_TunerOpen_c1_2K(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DVBS_TunerOpen_c2_2K(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DVBS_TunerOpen_c1_4K(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DVBS_TunerOpen_c2_4K(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
#endif

//------------------------------------------------------------------------- 
/*
 *  CheckDemodTypeFromEeprom
 *  @param  void
 *  @retval demodid
 */
//-------------------------------------------------------------------------
UINT8 CheckDemodTypeFromEeprom(VOID)
{
    UINT8  u1Val[3]={0};//initialize u1Val to fix Klocwork warning

    UINT8  u1ResultType;
    UINT32  u4Ofst[3];
    UINT8  u1Default;

    if(b_DemodType_init == TRUE)
    {
        return ui1_DemodType;
    }
    //u4Ofst = DRVCUST_OptGet(eDemodTypeOfst);
    /* Read three offset value and default Demod type from drv_cust */
    u4Ofst[0] = DEMOD_TYPE_CUSTOM_OFFSET1;//DRVCUST_OptGet(eDemodTypeCustomOffset1);
    u4Ofst[1] = DEMOD_TYPE_CUSTOM_OFFSET2;//DRVCUST_OptGet(eDemodTypeCustomOffset2);
    u4Ofst[2] = DEMOD_TYPE_CUSTOM_OFFSET3;//DRVCUST_OptGet(eDemodTypeCustomOffset3);
    u1Default = DEMOD_TYPE_TC;
    //mcDBG_MSG_INFO(("EEPROM(PH Demod): Ofst0=0x%x,Ofst1=0x%x,Ofst2=0x%x,def=0x%x\r\n",u4Ofst[0], u4Ofst[1], u4Ofst[2], u1Default));

    /* Read three Demod IDs from EEPROM offset */
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[0], (UINT32)(void *)&u1Val[0], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[1], (UINT32)(void *)&u1Val[1], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[2], (UINT32)(void *)&u1Val[2], 1));
    
    //mcDBG_MSG_INFO(("Read DemodType from EEPROM: addr(0x%x)=0x%x,addr(0x%x)=0x%x,addr(0x%x)=0x%x\r\n",u4Ofst[0],u1Val[0],u4Ofst[1],u1Val[1],u4Ofst[2],u1Val[2]));
    
    /* If two of Demod types in eeprom are the same and are valid tuner type, then trust this value, other condition return default type */
    if (u1Val[0]==u1Val[1]){
        u1ResultType = u1Val[0];
    }
    else if(u1Val[0]==u1Val[2]){
        u1ResultType = u1Val[0];
    }
    else if(u1Val[1]==u1Val[2]){
        u1ResultType = u1Val[1];
    }
    else{
        //Return default tuner type
        u1ResultType = u1Default;
    }
    //mcDBG_MSG_INFO(("u1ResultType=0x%x\r\n", u1ResultType));

    b_DemodType_init = TRUE;
    return u1ResultType;
}

INT32 REMAPDVBT_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ui1_DemodType = CheckDemodTypeFromEeprom();

    if((ui1_DemodType == DEMOD_TYPE_TC)||(ui1_DemodType == DEMOD_TYPE_S2))
    {
        DVBT_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if((ui1_DemodType == DEMOD_TYPE_T2)||(ui1_DemodType == DEMOD_TYPE_T2S2)||(ui1_DemodType == DEMOD_TYPE_T2S2_2K))
    {
        DVBT2_TunerOpen_c1(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if((ui1_DemodType == DEMOD_TYPE_T2_C2)||(ui1_DemodType == DEMOD_TYPE_T2S2_C2)||(ui1_DemodType == DEMOD_TYPE_T2S2_C2_2K))
    {
        DVBT2_TunerOpen_c2(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if(ui1_DemodType == DEMOD_TYPE_ISDBT)
    {
        Vendor_TunerOpen_c1(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }	
    else if(ui1_DemodType == DEMOD_TYPE_ISDBT_C2)
    {
        Vendor_TunerOpen_c2(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }	
    else if(ui1_DemodType == DEMOD_TYPE_DTMB)
    {
        DTMB_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if(ui1_DemodType == DEMOD_TYPE_DTD)
    {
        DTD_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBC_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //#if 1
    //DVBC_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
 //   #else
    ui1_DemodType = CheckDemodTypeFromEeprom();
    
    if((ui1_DemodType == DEMOD_TYPE_TC)||(ui1_DemodType == DEMOD_TYPE_S2)||(ui1_DemodType == DEMOD_TYPE_DTMB))
    {
        DVBC_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if((ui1_DemodType == DEMOD_TYPE_T2)||(ui1_DemodType == DEMOD_TYPE_T2S2)||(ui1_DemodType == DEMOD_TYPE_T2S2_2K))
    {
        DVBT2_TunerOpen_c1(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if((ui1_DemodType == DEMOD_TYPE_T2_C2)||(ui1_DemodType == DEMOD_TYPE_T2S2_C2)||(ui1_DemodType == DEMOD_TYPE_T2S2_C2_2K))
    {
        DVBT2_TunerOpen_c2(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
	
    else if(ui1_DemodType == DEMOD_TYPE_DTD)
    {
        DTD_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
  //  #endif
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBS_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //#ifdef SYS_DVBS_SUPPORT
    ui1_DemodType = CheckDemodTypeFromEeprom();

    if((ui1_DemodType == DEMOD_TYPE_S2)||(ui1_DemodType == DEMOD_TYPE_T2S2))
    {
        DVBS_TunerOpen_c1_4K(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
	
    else if(ui1_DemodType == DEMOD_TYPE_T2S2_C2)
    {
        DVBS_TunerOpen_c2_4K(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
	
    else if(ui1_DemodType == DEMOD_TYPE_T2S2_2K)
    {
        DVBS_TunerOpen_c1_2K(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    else if(ui1_DemodType == DEMOD_TYPE_T2S2_C2_2K)
    {
        DVBS_TunerOpen_c2_2K(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }

   // #endif
    return (DRVAPI_TUNER_OK);
}
//#endif

