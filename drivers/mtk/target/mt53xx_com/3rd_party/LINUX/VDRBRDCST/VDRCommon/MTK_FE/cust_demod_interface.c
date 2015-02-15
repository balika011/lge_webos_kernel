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

//demod type list definition
#define DEMOD_TYPE_T2 (0X01)
#define DEMOD_TYPE_S2 (0X01<<1)

#define DEMOD_TYPE_CXD2837      (DEMOD_TYPE_T2) //0x01
#define DEMOD_TYPE_CXD2842      (DEMOD_TYPE_T2|DEMOD_TYPE_S2)//0x03

static BOOL     b_DemodType_init = FALSE;
static UINT8    ui1_DemodType = 0;

extern INT32 DVBT2_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);
extern INT32 DVBS_TunerOpen(PTD_CTX ptTDCtx,
    TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* ptTDSpecificCtx);




//------------------------------------------------------------------------- 
/*
 *  CheckDemodTypeFromEeprom
 *  @param  void
 *  @retval demodid
 */
//-------------------------------------------------------------------------
UINT8 CheckDemodTypeFromEeprom(VOID)
{
    UINT8  u1Val = 0;
    UINT32 u4Ofst;

    if(b_DemodType_init == TRUE)
    {
        return ui1_DemodType;
    }
    /* Read three offset value and default demod type from drv_cust */
    u4Ofst = DRVCUST_OptGet(eDemodTypeOfst);
    mcSHOW_USER_CRI_MSG(("EEPROM(Demod): Ofst=0x%x\r\n",u4Ofst));

    /* Read three tuner IDs from EEPROM offset */
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&u1Val, 1));

    mcSHOW_USER_CRI_MSG(("Read DemodType from EEPROM: addr(0x%x)=0x%x\r\n",u4Ofst,u1Val));

    b_DemodType_init = TRUE;
    
    return u1Val;
}

INT32 REMAPDVBT_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ui1_DemodType = CheckDemodTypeFromEeprom();

    if((ui1_DemodType == DEMOD_TYPE_CXD2837)||(ui1_DemodType == DEMOD_TYPE_CXD2842))
    {
        DVBT2_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBC_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ui1_DemodType = CheckDemodTypeFromEeprom();
    
    if((ui1_DemodType == DEMOD_TYPE_CXD2837)||(ui1_DemodType == DEMOD_TYPE_CXD2842))
    {
        DVBT2_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

INT32 REMAPDVBS_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ui1_DemodType = CheckDemodTypeFromEeprom();

    if(ui1_DemodType == DEMOD_TYPE_CXD2842)
    {
        DVBS_TunerOpen(ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
    }
    return (DRVAPI_TUNER_OK);
}

