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
 * $RCSfile: pd_tuner_helene.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pd_tuner_helene.c
 *  glue layer for SS  helene.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tuner_interface_if.h"
#include "x_typedef.h"
#include "tunerDebug.h"
//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------
// Tuner #0
EXTERN VOID ASCOT3HELENE_TunerInit_0(
    ITUNER_CTX_T *pTCtx
    );
EXTERN INT16 ASCOT3HELENE_TunerSetFreq_0(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T * param
    );
EXTERN INT16 ASCOT3HELENE_TunerOP_0(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal);

// Tuner #1
EXTERN VOID ASCOT3HELENE_TunerInit_1(
    ITUNER_CTX_T *pTCtx
    );
EXTERN INT16 ASCOT3HELENE_TunerSetFreq_1(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T * param
    );
EXTERN INT16 ASCOT3HELENE_TunerOP_1(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal);
//---------------------------------------------------------------------------
// static functions
//---------------------------------------------------------------------------
static VOID ASCOT3HELENE_TunerInit_Remap(ITUNER_CTX_T *pTCtx)
{
    UINT8 u1TunerCellID = pTCtx->u1TunerCellID;
    switch(u1TunerCellID)
    {
        case 0:
            ASCOT3HELENE_TunerInit_0(pTCtx);
            break;
        case 1:
            ASCOT3HELENE_TunerInit_1(pTCtx);
            break;
        default:
            mcDBG_MSG_L1(("%s error: Got tuner cell ID %d!\n",__FUNCTION__,u1TunerCellID));
            break;
    }
    return;
}

static INT16 ASCOT3HELENE_TunerSetFreq_Remap(ITUNER_CTX_T *pTCtx,PARAM_SETFREQ_T * param)
{
    UINT8 u1TunerCellID = pTCtx->u1TunerCellID;
    INT16 i2Ret = 0;
    switch(u1TunerCellID)
    {
        case 0:
            i2Ret = ASCOT3HELENE_TunerSetFreq_0(pTCtx,param);
            break;
        case 1:
            i2Ret = ASCOT3HELENE_TunerSetFreq_1(pTCtx,param);
            break;
        default:
        {
            i2Ret = -1;
            mcDBG_MSG_L1(("%s error: Got tuner cell ID %d!\n",__FUNCTION__,u1TunerCellID));
            break;
        }
    }
    return i2Ret;
}

static INT16 ASCOT3HELENE_TunerOP_Remap(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    UINT8 u1TunerCellID = pTCtx->u1TunerCellID;
    INT16 i2Ret = 0;
    switch(u1TunerCellID)
    {
        case 0:
            i2Ret = ASCOT3HELENE_TunerOP_0(pTCtx,eOperation,SetVal,pInOutVal);
            break;
        case 1:
            i2Ret = ASCOT3HELENE_TunerOP_1(pTCtx,eOperation,SetVal,pInOutVal);
            break;
        default:
        {
            i2Ret = -1;
            mcDBG_MSG_L1(("%s error: Got tuner cell ID %d!\n",__FUNCTION__,u1TunerCellID));
            break;
        }
    }
    return i2Ret;
}

//---------------------------------------------------------------------------
// static variables
//---------------------------------------------------------------------------

ITUNER_FCT_T piTunerFct_ASCOT3HELENE = {
    .Init    = ASCOT3HELENE_TunerInit_Remap,
    .SetFreq = ASCOT3HELENE_TunerSetFreq_Remap,
    .OP      = ASCOT3HELENE_TunerOP_Remap,
};

