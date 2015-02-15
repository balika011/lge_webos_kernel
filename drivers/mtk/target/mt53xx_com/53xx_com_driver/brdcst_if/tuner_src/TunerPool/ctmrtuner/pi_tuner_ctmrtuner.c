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
 * $RCSfile: pi_tuner_ctmrtuner.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_ctmrtuner.c
 *  Tuner control for CTMRTUNER(Customer dummy tuner)
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tuner_interface_if.h"
#include "PI_Def.h"
#include "pi_demod_atd.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//---------------------------------static  variable--------------------------------
#define PGA_GAIN_SECAM 0x0a
#define PGA_GAIN_PAL 0x03
#define PGA_GAIN_NTSC 0x04
//
/** CTMRTUNER_TunerInit
 *  Tuner initialzation for CTMRTUNER.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
VOID CTMRTUNER_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{ 
    pTCtx->fgRFTuner=FALSE;//Legacy tuner
	pTCtx->u2LO_Step=1000;  //*1Khz
	pTCtx->u1AtdPatchSwitch = (MASK_PATCH_CR|MASK_PATCH_SENS|MASK_PATCH_DRO); 
    mcSHOW_USER_MSG(("Customer Dummy Tuner Init\n"));
}

//----------------------------------------------------------------------------- 
/** CTMRTUNER_TunerSetFreq
 *  Set Tuner PLL for CTMRTUNER to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval   0   success
 *                1   fail   out of range 
 *               -1  fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 CTMRTUNER_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
    mcSHOW_DBG_MSG(("Customer dummy tuner set freq\n")); 

    return (0);    //alway return success
}

//----------------------------------------------------------------------------- 
/**  CTMRTUNER_TunerGetStatus
 *  Set Tuner PLL staus for CTMRTUNER
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
STATIC BOOL CTMRTUNER_TunerGetStatus(ITUNER_CTX_T *pTCtx)
{
    return TRUE;    // return tuner lock status
}

//----------------------------------------------------------------------------- 
/** CTMRTUNER_TunerGetStatus
 *  Set Tuner PLL staus for CTMRTUNER
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
STATIC CHAR *CTMRTUNER_TunerGetVer(VOID)
{
    return ("Customer Dummy Tuner V0.0");
}

//----------------------------------------------------------------------------- 
/** CTMRTUNER_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 CTMRTUNER_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{         
    switch(eOperation)
    {
    case itGetStatus:
        {
            BOOL * Retval=(BOOL *)pInOutVal;
            *Retval=CTMRTUNER_TunerGetStatus(pTCtx);
        }
        break;
    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
            *RetStr=CTMRTUNER_TunerGetVer();
        }
        break;	
    case itSetIF:
        break;	
    case itSetSawBw:
        pTCtx->specific_member.eu_ctx.m_SAW_BW = SetVal;
        break;	
    case itGetPGAIdx:
		{
   		  UINT8 u1SubSysIdx=SetVal;
		  if((MOD_SECAM_L==u1SubSysIdx) ||(MOD_SECAM_L1==u1SubSysIdx))
		  {
		   *(UINT8*)pInOutVal=PGA_GAIN_SECAM;
		  }
		  else if (MOD_NTSC_M==u1SubSysIdx)
		  {
		    *(UINT8*)pInOutVal=PGA_GAIN_NTSC;
		  }
		  else
		  {
		   *(UINT8*)pInOutVal=PGA_GAIN_PAL;
		  }
		}
		break;
    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
