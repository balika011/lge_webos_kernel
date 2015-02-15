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
 * $RCSfile: piana_f05wt.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file piana_f05wt.c
 *  Analog PI driver of Samsung F05WT combo tuner
 */

#define  __DRV_TUNER_C__

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "PI_ATuner.h"
//#include "sif_nptv_if.h"
//#include "drv_tuner.h"
//#include "tvsys_info.h"
//#include "PI_Def.h"
//#include "pi_anaeu_if.h"
#include "pd_anaeu_glue.h"
#include "piana_f05wt.h"
#include "sif_if.h"
//#include "PD_ATuner_dbg.h"
//#include "PI_ATuner_dbg.h"
//#include "PD_Def.h"
#include "tuner_interface_if.h"
//#include"ctrl_bus.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define TUNER_PART_POR                 0x80  // 0: Successful reading, 1: Power On reset.
#define TUNER_PART_FL                  0x40  // 0: PLL lock, 1: PLL unlock.
#define IF_PART_PONR                   0x01  // 0: Successful reading, 1: Power On reset.
#define IF_PART_AFC                    0x1E  // These four bits is used to monitor AFC Status.
#define IF_PART_FMIFL                  0x20  // 0: FM IF level low, 1: FM IF level High
#define IF_PART_VIFL                   0x40  // 0: Video IF Level Low, 1: Video IF Level High.
#define IF_PART_AFCWIN                 0x80  // 0: VIF outside AFC Window, 1: VIF inside AFC Window.
#define TUNER_I2C_RETRY     10
#define fcIGNORE_TVD_LOCK       1
#define fcNOT_CHECK_TVD         1           // Not check TVD for Europe System
#define TUNER_SCAN_DEBUG        0


//add by liuqu,20080710
#define VIF_LOCK_RANGE_DEBUG    0

#if fcTIMING_MEASURE
#define fcTIMEOUT_THRESHOLD_SET_FREQ            30   // Dirver ATP criterion, timeout for set freq
#define fcTIMEOUT_THRESHOLD_GET_SCAN_STEP       50   // Driver ATP criterion, timeout for get scan step
#define fcSEARCHNEARBY_TOLERANCE_TIME           10   // Driver ATP criterion, tolerance time for searchnearby (SW delay+overhead)
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
/***********************************************************
  fine tune step:       IF_PART_AFCWIN & IF_PART_VIFL are both ON
  check step:           IF_PART_AFCWIN is ON, but IF_PART_VIFL is OFF
  snow step:            IF_PART_AFCWIN is OFF
***********************************************************/
STATIC const UINT8 SCAN_STEP[4][3] =
{
  /* { fine tune step, check step, snow step}  */
  { SCAN_SMALL_STEP, SCAN_MIDDLE_STEP, SCAN_MIDDLE_STEP},   // Auto Scan Steps
  { SCAN_TINY_STEP,  SCAN_TINY_STEP, SCAN_SMALL_STEP},      // Auto Fine Tune Steps
  { SCAN_SMALL_STEP, SCAN_SMALL_STEP, SCAN_MIDDLE_STEP},    // Semi Scan Steps
  { SCAN_SMALL_STEP, SCAN_SMALL_STEP, SCAN_MIDDLE_STEP}     // Uni-type Auto Scan Steps
};

STATIC const UINT8 TunerPartAddr[1] = { TUNER_ONE_ADDR_TUNER};
STATIC const UINT8 IFPartAddr[1] = { TUNER_ONE_ADDR_IF};

STATIC UINT32  au4TvSysGroup[] =
{
    (TV_SYS_MASK_B | TV_SYS_MASK_G),
    (TV_SYS_MASK_D | TV_SYS_MASK_K),
    (TV_SYS_MASK_I),
    (TV_SYS_MASK_L),
    (TV_SYS_MASK_L_PRIME),
    (TV_SYS_MASK_M)
};

STATIC TUNER_ANA_ATTRIBUTE_T sTunerAnaAttribute =
{
    45150000,
    873225000,
    (TV_FINE_TUNE_THRESHOLD / 2) * 50000
};

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
EXTERN S16 Tuner_SetIFDemod(VOID* pDigiTunerCtx, U8 u1SubSysIdx);
EXTERN UINT8 u1CheckVifLoop;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

/************************************************************************
*  fgIFPartRead
*  I2C Read programming function to get VIF status register.
*  @param  bAddr : VIF slave address.
*  @param  *pbData : Output data content pointer.
*  @retval  TRUE : Successful.
*  @retval  FALSE : Failure.
************************************************************************/
static BOOL fgIFPartRead(UINT8 bAddr, UINT8 *pbData)
{
    UINT8 i;

    for (i = 0; i < TUNER_I2C_RETRY; i++)
    {
        //if ((fgHwI2CDataReadWithNoSubAddr(bAddr, 1, pbData, TUNER_I2C_OPTION)) == SV_TRUE)
        //if(SIF_Read_Only(0xf00, bAddr, pbData, 1) != 0) // 070308, LC Chien, slow down I2C speed, 27M/0x200 ~= 50k
        if(SIF_TunerReadNoSubAddr(0xf00, bAddr, pbData, 1) > 0)
        {
           return TRUE;
        }
    }
    mcSHOW_HW_MSG(("IF Part Read fail!\n"));
    return FALSE;
}

/************************************************************************
*  fgTunerPartRead
*  I2C Read programming function to get tuner PLL status register.
*  @param  bAddr : PLL slave address.
*  @param  *pbData : Output data content pointer.
*  @retval  TRUE : Successful.
*  @retval  FALSE : Failure.
************************************************************************/
STATIC BOOL fgTunerPartRead(UINT8 bAddr, UINT8 *pbData)
{
    UINT8 i;

//    mcDELAY_MS(2);     //delay 2ms for PLL lock check

    for(i=0;  i<TUNER_I2C_RETRY; i++)
    {
        //if((fgHwI2CDataReadWithNoSubAddr(bAddr, 1, pbData, TUNER_I2C_OPTION)) == SV_TRUE)
        //if((SIF_NIMReadNoSubAddr(0x100, bAddr, pbData, 1)) > 0) // new api
        if(SIF_TunerReadNoSubAddr(0x100, bAddr, pbData, 1) > 0)
        {
        //    mcSHOW_DBG_MSG(("fgTunerPartRead: %02X=%02X\n", bAddr, *pbData));
            return TRUE;
        }
    }
    mcSHOW_HW_MSG(("fgTunerPartRead: Fail\n"));
    return FALSE;
}

/*******************************************************************************************
*  fgGetCurIFStatus
*  Get one byte of VIF status register value
*  @param  bAddr : VIF slave address.
*  @param  *pbData : Output data content pointer.
*  @param  bMask : To get these bits to expect, 0 Not expect, 1, expect.
*  @retval  TRUE : Successful.
*  @retval  FALSE : Failure.
********************************************************************************************/
STATIC BOOL fgGetCurIFStatus(UINT8 bAddr, UINT8 *pbData, UINT8 bMask)
{
  UINT8 bTimeout =  5;
  BOOL fgReturn = FALSE;
  UINT8 bCntThreshold = 2;

  UINT8 pbDataOld;

  //*pbData = 0;
  pbDataOld = 0;
  if ((TRUE == fgIFPartRead(bAddr, pbData)) && ((*pbData & IF_PART_PONR) != IF_PART_PONR))
    pbDataOld = *pbData;

  do
  {
    *pbData = 0;
    // After a successful reading of the status register
    if  ((TRUE == fgIFPartRead(bAddr, pbData)) && ((*pbData & IF_PART_PONR) != IF_PART_PONR) && (*pbData == pbDataOld))
    {
      // VIFL lock
      if(((*pbData & IF_PART_VIFL) == IF_PART_VIFL) || ((*pbData & IF_PART_FMIFL) == IF_PART_FMIFL))
      {
        fgReturn = TRUE;
        break;
      }
      else  // VIFL not lock
      {
        bCntThreshold--;
      }
    }
    else     // The PONR is not ready or IFPartRead fail
    {
      pbDataOld = *pbData;
      bTimeout--;
    }
  }
  while (bTimeout != 0 && bCntThreshold != 0);

  *pbData &= bMask;

  if (!bCntThreshold)   // Check 2 times to make sure VIFL is really not locked.
  {
    fgReturn = TRUE;
  }

  return fgReturn;
}

/*******************************************************************************************
*  fgGetCurTunerStatus
*  Get one byte of tuner PLL status register value
*  @param  bAddr : PLL IC slave address.
*  @param  *pbData : Output data content pointer.
*  @retval  TRUE : Successful.
*  @retval  FALSE : Failure.
********************************************************************************************/
STATIC BOOL fgGetCurTunerStatus(UINT8 bAddr, UINT8 *pbData)
{
  UINT8 bCntThreshold;

  bCntThreshold = 20;//255;
  do
  {
    mcDELAY_MS(10);
    *pbData = 0;
    if (TRUE == fgTunerPartRead(bAddr, pbData))
    {
      if (((*pbData & TUNER_PART_POR)!=TUNER_PART_POR)&&((*pbData & TUNER_PART_FL) == TUNER_PART_FL))
      {
        break;
      }
    }
    bCntThreshold --;
  }
  while (bCntThreshold != 0);

  return ((bCntThreshold != 0) ? TRUE : FALSE);
}

/***************************************************************************
*  fgDrvTunerCheckFreqLock
*  Check if tuner PLL is locked or not.
*  @param :  void.
*  @retval : TRUE : PLL is locked.
*  @retval : FALSE : PLL is not locked yet.
****************************************************************************/
STATIC BOOL fgDrvTunerCheckFreqLock(void)
{
  UINT8 bHWStatus;
  return (fgGetCurTunerStatus(TunerPartAddr[0], &bHWStatus));
}

/*****************************************************************************
*  bDrvMonitorAFCStatus
*  Calculate AFC frequency offset. 
*  @param  bAFCBits : VIF status register value.
*  @retval UINT8 : AFC frequency offset.
******************************************************************************/
STATIC UINT8 bDrvMonitorAFCStatus(UINT8 bAFCBits)
{
  return ((bAFCBits & 0x10) ? (7-((bAFCBits&0x0F)>>1)) : (bAFCBits&0x0F)>>1);
}

/***************************************************************************
*  bDrvTunerGetScanStep
*  Calculate next scan step according to the scan type, direction and current VIF status.
*  @param bScanType : AUTO_SCAN,  AUTO_FINE_TUNE,  SEMI_SCAN, UNI_AUTO_SCAN.
*  @param bDirection : SCAN_DIRECTION_DEC, SCAN_DIRECTION_IN.C
*  @param *pbData : Content pointer to save current VIF status.
*  @retval  UINT8 : Next scan step. Defined in SCAN_STEP[][] according to differnt scan type.
****************************************************************************/
STATIC UINT8 bDrvTunerGetScanStep(UINT8 bScanType, UINT8 bDirection, UINT8 *pbData)
{
  UINT8 bIFStatus, bStep;

    
#if VIF_LOCK_RANGE_DEBUG
  static UINT8 bStepPre =0x08, bStepMask;
  mcSHOW_DBG_MSG(("bStepPre =%d\n",bStepPre));
#endif

  fgGetCurIFStatus(IFPartAddr[0],
                    &bIFStatus,
                    IF_PART_AFCWIN | IF_PART_AFC | IF_PART_VIFL);
  
  *pbData = bIFStatus;
  
  mcSHOW_DBG_MSG1(("Step AFC status: %x\n", bIFStatus));

  if ( ((bIFStatus & IF_PART_AFCWIN) == IF_PART_AFCWIN) )
  {
     if (bDrvMonitorAFCStatus(bIFStatus) < 0x07)
     {
         if ((!((bIFStatus & IF_PART_VIFL) == IF_PART_VIFL) )&& GetVIFLBitCheckEnable())
         {
             bStep =  SCAN_STEP[bScanType][1];
         }
         else
         {
             if (bDrvMonitorAFCStatus(bIFStatus) <= 1)
             {// Current Freq is very close to the nominal VIF, don't tune the freq anymore.
                 return 0;
             }
             else
             { // Current Freq is near the nominal VIF; judge bStep from AFC status
                 bStep = (bDrvMonitorAFCStatus((bIFStatus))/2);
                 if ((bIFStatus & IF_PART_AFC) & 0x10)
                 {
                    return (0x80 | bStep);
                 }
                 else
                 {
                    return (bStep);
                 }
             }
         }
     }
     else
     {
        if ((bIFStatus & IF_PART_VIFL) == IF_PART_VIFL)
        {
           bStep =  SCAN_STEP[bScanType][0];   // fine tune step
        }
        else  // Sometimes the AFCWIN will be set even no VIFL inside AFCWIN
        {
           bStep =  SCAN_STEP[bScanType][1];    // check step
        }
     }
  }
  else  // IF_PART_AFCWIN not active
  {
     bStep = SCAN_STEP[bScanType][2];   // snow step
  }

    
  //add by liuqu,20080701
#if VIF_LOCK_RANGE_DEBUG
  bStepMask = bStep & 0x7f;
  if ((bStepMask == 0x04)&&(bStepPre ==0x08))
  {
     mcSHOW_DBG_MSG1(("start to enter AFCWIN!!liuqu\n"));
  }
  else if((bStepMask <0x04)&&(bStepPre ==0x04))
  {
     mcSHOW_DBG_MSG1(("start to enter AFC,liuqu,bstep =%d!!\n",bStep));
  }
  else if((bStepMask <0x04)&&(bStepPre ==0x08))
  {
     mcSHOW_DBG_MSG1(("start to enter AFC from 0x08,liuqu,bstep =%d!!\n",bStep));
  }
#endif

  if (bDirection == SCAN_DIRECTION_DEC)
     bStep = 0x80 | bStep;

#if VIF_LOCK_RANGE_DEBUG  
  bStepPre = bStep & 0x7f;
#endif

  return bStep;
}

//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

/*****************************************************************************
*  F05WT_AnaTunerGetVer
*  Get analog tuner model. 
*  @param  void.
*  @retval : Analog tuner model.
******************************************************************************/
UINT8 *F05WT_AnaTunerGetVer(void){
    return ("analog F05WT");
}

/*****************************************************************************
*  F05WT_AnaTunerInit
*  Initialize analog PI driver context varibables. 
*  @param  *psAtvPiCtx : pointer use to point to struct ATV_PI_CTX_T. 
*  @retval  TRUE : Successful.
*  @retval  FALSE : Failure.
******************************************************************************/
BOOL F05WT_AnaTunerInit(ATV_PI_CTX_T *psAtvPiCtx)
{
//ATV_PI_CTX_T *pF05WT_AnaTuner = NULL;
ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;

//    pF05WT_AnaTuner = (ATV_PI_CTX_T *) mcALLOC_MEM(sizeof(ATV_PI_CTX_T));
    if (pF05WT_AnaTuner == NULL)
        return FALSE;


   // pF05WT_AnaTuner->pDigiTunerCtx = GetDigiDvbtTunerCtx();

//    pFQD1236_AnaTuner->u1DEMOD_I2c_Addr = IFDEMOD_I2c_Addr;
//    pFQD1236_AnaTuner->u1RF_I2cAddr = RF_I2c_Addr;
    pF05WT_AnaTuner->u1IF_I2cAddr = TUNER_ONE_ADDR_IF;
    pF05WT_AnaTuner->u1RF_I2cAddr = TUNER_ONE_ADDR_TUNER;
    pF05WT_AnaTuner->u2IF_Freq = F05WT_ANA_PIC_IF;
    pF05WT_AnaTuner->u2LO_Step = F05WT_ANA_FREQ_STEP;
//    pF05WT_AnaTuner->u4Frequency = F05WT_ANA_INIT_FREQ;
    pF05WT_AnaTuner->u1DEMOD_state = F05WT_ANA_UNLOCK;
    pF05WT_AnaTuner->u1AnaMode = MOD_ANA_CABLE;
    pF05WT_AnaTuner->u1VIFLock = 0;
//    pFQD1236_AnaTuner->aSW_Ver_HW_Ver = FQD1236_ANA_TUNER_VER;

//    pF05WT_AnaTuner->sAtvSubSysCtx.ucTvSysGroupNum = sizeof(au4TvSysGroup) / sizeof(UINT32);
    pF05WT_AnaTuner->ucTvSysGroupNum = sizeof(au4TvSysGroup) / sizeof(UINT32);
    pF05WT_AnaTuner->pu4TvSysGroup = au4TvSysGroup;
    pF05WT_AnaTuner->psTunerAnaAttr = &sTunerAnaAttribute;

//    *ppsAtvPiCtx = pF05WT_AnaTuner;

    return TRUE;
    
}

/*****************************************************************************
*  F05WT_AnaTunerClose
*  Clear analog PI driver context varibables. 
*  @param  *psAtvPiCtx : pointer use to point to struct ATV_PI_CTX_T. 
*  @retval  void.
******************************************************************************/
void F05WT_AnaTunerClose(ATV_PI_CTX_T *psAtvPiCtx)
{
//    mcFREE_MEM(psAtvPiCtx);
}

/***************************************************************************
*  F05WT_TunerSetSystem
*  Set TV system into VIF.
*  @param  *psAtvPiCtx : pointer use to point to struct ATV_PI_CTX_T. 
*  @param  u1SubSysIdx : TV system index.
*  @retval  TRUE : I2C programming successful.
*  @retval  FALSE : I2C programming fail.
****************************************************************************/
BOOL F05WT_TunerSetSystem(ATV_PI_CTX_T *psAtvPiCtx, UINT8 u1SubSysIdx)
//BOOL fgDrvTunerSetSystem(UINT8 bSubSysIdx)
{
    BOOL fgReturn;
	ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;

    //jackson mark
    if (u1SubSysIdx >= SYS_SUB_TOTAL)
        //u1SubSysIdx = SYS_SUB_NTSC_M;
    {
	mcSHOW_DBG_MSG(("!!!warning-u1SubSysIdx is set=%x,out of rang\n",u1SubSysIdx));
	}
	mcSHOW_DBG_MSG(("xxxSubSysIdx is set=%x\n",u1SubSysIdx));
    //jackson add
   
    //Jackson add
 pF05WT_AnaTuner->u1SubSysIdx = u1SubSysIdx;

     ITuner_OP(ITunerGetCtx(), itSetIfDemod, (u1SubSysIdx+MOD_ANA_TYPE_BEGIN), &fgReturn);

    return fgReturn;
}

/***************************************************************************
*  F05WT_AnaTunerSetFreq
*  Set frequency into PLL.
*  @param  *psAtvPiCtx : Pointer use to point to struct ATV_PI_CTX_T. 
*  @param  wFreqInKHz : Frequency in KHz unit.
*  @param  u1Mode : Current connection mode. (scan, change channel, fine-tune)
*  @retval  TRUE : I2C programming successful.
*  @retval  FALSE : I2C programming fail.
****************************************************************************/
BOOL F05WT_AnaTunerSetFreq(ATV_PI_CTX_T *psAtvPiCtx, UINT32 wFreqInKHz, UINT8 u1Mode)
{

  ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;
  //pF05WT_AnaTuner->u1AnaMode = u1AnaMode;
//  pF05WT_AnaTuner->u4Frequency = wFreqInKHz;
  PARAM_SETFREQ_T SetFreqPara ;
  SetFreqPara.Freq = wFreqInKHz;
  SetFreqPara.Modulation =pF05WT_AnaTuner->u1SubSysIdx;
  SetFreqPara.fgAutoSearch = 0;



  BOOL fgReturn;
  
  if(u1Mode == ATV_CONN_MODE_CHAN_CHANGE)
  {
  	fgReturn = ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara);
  }
  else
  {  SetFreqPara.fgAutoSearch = 1;
  	fgReturn = ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara);
  }

  return fgReturn;
}

/***************************************************************************
*  F05WT_bDrvTunerCheckVIFLock
*  Check if programmed frequency has strong VIF signal nearby.
*  @param  *psAtvPiCtx : Pointer use to point to struct ATV_PI_CTX_T. 
*  @retval  2 : Current frequency is very close to the nominal VIF.
*  @retval  1 : Current frequency is is near the nominal VIF.
*  @retval  0 : Current frequency is not close to the nominal VIF yet.
****************************************************************************/
UINT8 F05WT_bDrvTunerCheckVIFLock(ATV_PI_CTX_T *psAtvPiCtx)
{
  UINT8 bIFStatus, bAFCStatus;

  fgGetCurIFStatus(IFPartAddr[0],
                    &bIFStatus,
                    IF_PART_AFCWIN | IF_PART_AFC | IF_PART_VIFL);

  mcSHOW_DBG_MSG(("Check VIF AFC status: %x\n", bIFStatus));

  if (((bIFStatus & IF_PART_AFCWIN) == IF_PART_AFCWIN))
  {
    bAFCStatus = bDrvMonitorAFCStatus(bIFStatus);
    //mcSHOW_DBG_MSG(("AFC status: %x\n", bAFCStatus));

    if (bAFCStatus <= 2)
    {
      //mcSHOW_USER_MSG(("AFC status : return 2\n")); //BS;060731
      // Current Freq is very close to the nominal VIF, don't tune the freq anymore.
      return 2;
    }
    else if (bAFCStatus < 0x07)
    {
      // Current Freq is near the nominal VIF.
      return 1;
    }
  }

  // IF_PART_AFCWIN not active or (bAFCStatus == 7)
  return 0;
}

/***************************************************************************
*  F05WT_AnaTunerSearchNearbyFreq
*  Find the VIF signal between the given start and end frequency.
*  @param  *psAtvPiCtx : Pointer use to point to struct ATV_PI_CTX_T. 
*  @param  *wFreqInKHz : Pointer of the start frequency (KHz).
*  @param  _BreakFct : Function pointer of the break function.
*  @param  *pvArg : Poniter of the break function parameter.
*  @param  u4FreqBDKHz : Search end frequency (KHz). 
*  @retval  bit0 : 1:Valid VIF signal is found. 0:Valid VIF signal is not found.
*  @retval  bit2 : break happen or not
****************************************************************************/
UINT8 F05WT_AnaTunerSearchNearbyFreq(ATV_PI_CTX_T *psAtvPiCtx, UINT32 *wFreqInKHz,
                                       x_break_fct _BreakFct, void *pvArg, UINT32 u4FreqBDKHz)
{
ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;
//UINT8 u1AnaMode = pF05WT_AnaTuner->u1AnaMode;
pF05WT_AnaTuner->isBreak = _BreakFct;
//UINT16 wDRTmp;
UINT32 wFRTmp;
UINT8 bStep, bStep_pre, bScanState, bIFStatus;
UINT8   ii;
UINT8   ucVIFLock = 0;
UINT8    fgScanDir;
UINT8 bATVStatus=0;
//UINT32 u4Freq;
//UINT8 ucStateCount;
bStep_pre = 0; 
//ucStateCount = 0;

#if fcTIMING_MEASURE
UINT32 u4TickStart, u4TickDuration;
//UINT8 ucLoopCnt = 0;
#endif

#if !fcNOT_CHECK_TVD
BOOL    fgTVDLock;
#endif

#if fcTIMING_MEASURE
    //u4SearchNearbyTime = fcSEARCHNEARBY_TOLERANCE_TIME;
    u4TickStart = mcGET_SYS_TICK();
#endif
    //wDRTmp = *wFreqDR;
    // Justin, 070830, make sure wFRTmp is in 50k Hz base
    wFRTmp = (*wFreqInKHz *1000) / 50000;
    wFRTmp *= (50000 /1000);

    mcSHOW_DBG_MSG(("MOMO LOG: SearchNearBy: Freq = %d kHz, FreqBD = %d kHz \n", wFRTmp, u4FreqBDKHz));


    fgScanDir = (*wFreqInKHz > u4FreqBDKHz)?SCAN_DIRECTION_DEC:SCAN_DIRECTION_INC;
    
//    for (ii = 0; ii < (200 / (cCHECK_VIF_DELAY * 10)); ii ++)
    for (ii = 0; ii < u1CheckVifLoop; ii ++)
    {
        mcDELAY_MS(10 * cCHECK_VIF_DELAY);
        ucVIFLock = F05WT_bDrvTunerCheckVIFLock(pF05WT_AnaTuner);
        if (ucVIFLock >= 2)
            break;    
    }
    
#if fcTIMING_MEASURE
    u4TickDuration = (mcGET_SYS_TICK() - u4TickStart) * mcGET_TICK_PERIOD();
    mcSHOW_DBG_MSG4(("[SearchNearby] 1st check VIF lock=%3u ms\n", u4TickDuration));
    //u4SearchNearbyTime = u4SearchNearbyTime + u4TickDuration;
#endif

#if !fcNOT_CHECK_TVD
    fgTVDLock = AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME);
#endif
#if fcTIMING_MEASURE
    #if !fcNOT_CHECK_TVD
    mcSHOW_DBG_MSG4((", %d, %d\n", ucVIFLock, fgTVDLock));
    #else
    mcSHOW_DBG_MSG4(("[SearchNearby] VIF Lock = %d\n", ucVIFLock));
    #endif
#endif
  //LogS("bDrvTunerSearchNearbyFreq");

  // Initial check to see if curent freq need full range AFT or not
#if !fcIGNORE_TVD_LOCK && !fcNOT_CHECK_TVD
    //if ((bScanType == eTUNER_UNI_AUTO_SCAN) && (ucVIFLock >= 2) && (TRUE == fgTVDLock))
    if ((ucVIFLock >= 2) && (TRUE == fgTVDLock))
#else
    //if ((bScanType == eTUNER_UNI_AUTO_SCAN) && (ucVIFLock >= 2))
    if (ucVIFLock >= 2)
#endif
    {
    #if TUNER_SCAN_DEBUG
        LogS("1. Very close!");
    #endif
    //    bScanState = TUNER_VIF_LOCK_STATE;
      mcSHOW_DBG_MSG(("MOMO LOG: SearchNearBy: RETURN0: Freq = %d \n", *wFreqInKHz));

        return TRUE;
    }
    else
    {
        if (*wFreqInKHz == u4FreqBDKHz)
        {
            wFRTmp -= TV_FINE_TUNE_THRESHOLD_KHZ / 2;
        }
        bScanState = TUNER_PLL_LOCK_STATE;
    }

  while (1)
  {

        if (pF05WT_AnaTuner->isBreak(pvArg))
        {
            mcSHOW_DBG_MSG(("[ATuner] SearchNearby.BreakFct\n"));
            mcSET_BIT(bATVStatus, cpANA_LOCK_STS_BREAK);
            return bATVStatus;
            //return (FALSE);
        }

	// remove 5ms delay after checking VIF lock for shorten scan time, Justin 080708
    // mcDELAY_MS(1);

    //#if fcTIMING_MEASURE
    //    mcSHOW_DBG_MSG(("[ATuner] SearchNearby.1=%3u ms\n",
    //                    (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
    //#endif
    
    switch(bScanState)
    {
      case TUNER_PLL_LOCK_STATE:
        // Check if current freq is outside TV_FINE_TUNE_THRESHOLD window
        if ((*wFreqInKHz == u4FreqBDKHz) && (wFRTmp > (*wFreqInKHz + TV_FINE_TUNE_THRESHOLD_KHZ/2)))
        {
        #if TUNER_SCAN_DEBUG
          LogS("4. Hit upper bound !");
        #endif
           return (FALSE);
        }

        if ((*wFreqInKHz != u4FreqBDKHz) && (wFRTmp > u4FreqBDKHz) && (fgScanDir == SCAN_DIRECTION_INC) )
        {
        #if TUNER_SCAN_DEBUG
            LogS("4. Hit upper bound !");
        #endif
            *wFreqInKHz = wFRTmp;  //return final frequency, LC Chien, 070115
            return (FALSE);
        }

        if ((*wFreqInKHz != u4FreqBDKHz) && (wFRTmp < u4FreqBDKHz) && (fgScanDir == SCAN_DIRECTION_DEC) )
        {
        #if TUNER_SCAN_DEBUG
            LogS("5. Hit lower bound !");
        #endif
            *wFreqInKHz = wFRTmp;  //return final frequency, LC Chien, 070115
            return (FALSE);
        }

          mcSHOW_DBG_MSG(("MOMO LOG: SearchNearBy: PLL_LOCK_STATE: Freq = %d\n", wFRTmp));

#if fcTIMING_MEASURE
        ucSearchNearbyLoopCnt++;
        u4TickStart = mcGET_SYS_TICK();
#endif        
        F05WT_AnaTunerSetFreq(psAtvPiCtx, wFRTmp, ATV_CONN_MODE_CHAN_SCAN);

        if (fgDrvTunerCheckFreqLock())
        {
          bScanState = TUNER_VIF_LOCK_STATE;

        }
        else
        {
          // Assume Tuner Device have not been inspected, AutoFine Behavior should be cont.
          mcSHOW_DBG_MSG(("PLL unlock!\n"));

          if(fgScanDir == SCAN_DIRECTION_INC)
          {
              wFRTmp += 50;
          }
          else
          {
              wFRTmp -= 50;
          }
        }
#if fcTIMING_MEASURE
        u4TickDuration = (mcGET_SYS_TICK() - u4TickStart) * mcGET_TICK_PERIOD();
        /*u4SearchNearbyTime = u4SearchNearbyTime + u4TickDuration;
        if (u4TickDuration > fcTIMEOUT_THRESHOLD_SET_FREQ)
        {
           mcSHOW_DBG_MSG(("[SearchNearby] Driver ATP Fail : Set Freq time (%ums) too long > %ums\n", 
           	                                u4TickDuration, fcTIMEOUT_THRESHOLD_SET_FREQ));
        }*/
        mcSHOW_DBG_MSG4(("[SearchNearby] The %u-th set freq and time is %3u ms\n",
                                             ucSearchNearbyLoopCnt, u4TickDuration));
#endif
            //#if fcTIMING_MEASURE
            //    mcSHOW_DBG_MSG(("[ATuner] SearchNearby.PLL=%3u ms / %d\n",
             //            (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD(),
             //            (INT16) (wFRTmp - *wFreqInKHz)));
            //#endif
        break;

      case TUNER_VIF_LOCK_STATE:
#if fcTIMING_MEASURE
        u4TickStart = mcGET_SYS_TICK();
#endif        
// remove 5ms delay after checking VIF lock for shorten scan time, Justin 080708
      	mcDELAY_MS(5);  
        
        bStep = bDrvTunerGetScanStep(eTUNER_UNI_AUTO_SCAN, fgScanDir, &bIFStatus);
        
#if fcTIMING_MEASURE
        u4TickDuration = (mcGET_SYS_TICK() - u4TickStart) * mcGET_TICK_PERIOD();
        /*u4SearchNearbyTime = u4SearchNearbyTime + u4TickDuration;
        if (u4TickDuration > fcTIMEOUT_THRESHOLD_GET_SCAN_STEP)
        {
           mcSHOW_DBG_MSG(("[SearchNearby] Driver ATP Fail : Get Scan Step time (%ums) too long > %ums\n", 
           	                                u4TickDuration, fcTIMEOUT_THRESHOLD_GET_SCAN_STEP));
        }*/
        mcSHOW_DBG_MSG4(("[SearchNearby] The %u-th get scan step=%3u ms\n",
                                              ucSearchNearbyLoopCnt, u4TickDuration));
#endif
        
        
        //if ((pF05WT_AnaTuner->u1SubSysIdx == 4) && (fgScanDir == SCAN_DIRECTION_INC))  //L' special case
        if ((fgScanDir == SCAN_DIRECTION_INC))
        {        	        	
        	if((bStep == 0x04) && (bStep_pre == 0x08) && ((bIFStatus & 0x1E) == 0x10))
                {
                	bStep = bStep | 0x80;
                }
        }
        //else if((pF05WT_AnaTuner->u1SubSysIdx <= 3) && (fgScanDir == SCAN_DIRECTION_DEC))  //non-L' special case
        else
        {
        	if((bStep == 0x84) && (bStep_pre == 0x88) && ((bIFStatus & 0x1E) == 0x0E))
                {
                	bStep = bStep & 0x7F;
                }
        }
        
        bStep_pre = bStep;
        
            //#if fcTIMING_MEASURE
            //    mcSHOW_DBG_MSG(("[ATuner] SearchNearby.Step=%02X\n", bStep));
            //#endif
        if (bStep != 0)
        {

            if ((bStep & 0x80) == 0x80) {
                bStep = bStep & 0x7F;
                switch(bStep)
                {
                case 1:
                    wFRTmp -= 50;
                    break;
                case 2:
                    wFRTmp -= 100;//125; 125 would be truncated to 100
                    break;
                case 3:
                    wFRTmp -= 150;
                    break;
                case 4:
                    wFRTmp -= GetJumpSmallStep(pF05WT_AnaTuner->pDigiTunerCtx);
                    break;
                case 8:
                    if (pF05WT_AnaTuner->u1SubSysIdx ==SYS_SUB_SECAM_L1)
                        wFRTmp -= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_INC);
                    else
                        wFRTmp -= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_DEC);;
                    break;
                default:
                    break;
                }
            }
            else {
                bStep = bStep & 0x7F;
                switch(bStep)
                {
                case 1:
                    wFRTmp += 50;
                    break;
                case 2:
                    wFRTmp += 100;//125; 125 would be truncated to 100
                    break;  
                case 3:
                    wFRTmp += 150;
                    break;
                case 4:
                    wFRTmp += GetJumpSmallStep(pF05WT_AnaTuner->pDigiTunerCtx);
                    break;
                case 8:
                        if(pF05WT_AnaTuner->u1SubSysIdx ==SYS_SUB_SECAM_L1)
                        {
                           wFRTmp+= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_DEC);
                        }
                        else
                        {
                           wFRTmp += GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_INC);
                        }
                    break;
                default:
                    break;
                }
            }

        #if TUNER_SCAN_DEBUG
       LogSW("2. Try again:",wFRTmp);
        #endif

          bScanState = TUNER_PLL_LOCK_STATE;
          //ucStateCount++;
        }
		
			
		/* liuyuan 20081202 update new  scan algroithm	
			
        if (bStep != 0)
        {

            if ((bStep & 0x80) == 0x80) {
                bStep = bStep & 0x7F;
                switch(bStep)
                {
                case 1:
                    wFRTmp -= 50;
                    break;
                case 2:
                    wFRTmp -= 100;//125; 125 would be truncated to 100
                    break;
                case 3:
                    wFRTmp -= 150;
                    break;
                case 4:
                    wFRTmp -= GetJumpSmallStep(pF05WT_AnaTuner->pDigiTunerCtx);
                    break;
                case 8:
                    if (fgScanDir == SCAN_DIRECTION_INC)
                        wFRTmp -= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_INC);
                    else
                        wFRTmp -= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_DEC);;
                    break;
                default:
                    break;
                }
            }
            else {
                bStep = bStep & 0x7F;
                switch(bStep)
                {
                case 1:
                    wFRTmp += 50;
                    break;
                case 2:
                    wFRTmp += 100;//125; 125 would be truncated to 100
                    break;  
                case 3:
                    wFRTmp += 150;
                    break;
                case 4:
                    wFRTmp += GetJumpSmallStep(pF05WT_AnaTuner->pDigiTunerCtx);
                    break;
                case 8:
                    if (fgScanDir == SCAN_DIRECTION_INC)
                   	{
                        if(pF05WT_AnaTuner->u1SubSysIdx ==SYS_SUB_SECAM_L1)
                        {
                          wFRTmp+= GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_DEC);
                        }
                        else
                        {
                           wFRTmp += GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_INC);
                        }
                    }
                    else
                        wFRTmp += GetJumpMiddleStep(pF05WT_AnaTuner->pDigiTunerCtx,SCAN_DIRECTION_DEC);;
                    break;
                default:
                    break;
                }
            }

        #if TUNER_SCAN_DEBUG
       LogSW("2. Try again:",wFRTmp);
        #endif

          bScanState = TUNER_PLL_LOCK_STATE;
          //ucStateCount++;
        }
*/        
		else
        {
        #if TUNER_SCAN_DEBUG
       LogSW("3. Got VIF:",wFRTmp);
        #endif

          *wFreqInKHz = wFRTmp;
          
        #if fcTIMING_MEASURE
        //    mcSHOW_DBG_MSG(("[ATuner] SearchNearby.VIF=%3u ms\n",
        //                    (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
                mcSHOW_DBG_MSG(("Got VIF: %d\n", wFRTmp));
        #endif
        #if fcNOT_CHECK_TVD
            return (TRUE);              // BS;060801;for MW auto search
        #else
            if(AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME) == TRUE)
            {
                #if fcTIMING_MEASURE
                        mcSHOW_DBG_MSG(("[ATuner] SearchNearby.TVDy=%3u ms\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
                #endif
                return TRUE;
            }
            else
            {
                #if fcTIMING_MEASURE
                        mcSHOW_DBG_MSG(("[ATuner] SearchNearby.TVDx=%3u ms\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
                #endif
                #if !fcIGNORE_TVD_LOCK
                        return FALSE;
                #else
                        return TRUE;
                #endif
            }
        #endif
        }
        break;
    }
  }
//  return FALSE;
}

/***************************************************************************
*  F05WT_TunerGetSystem
*  Get current TV system setting.
*  @param  *psAtvPiCtx : pointer use to point to struct ATV_PI_CTX_T. 
*  @retval  UINT8 : Current TV system setting value.
****************************************************************************/
UINT8 F05WT_TunerGetSystem(ATV_PI_CTX_T *psAtvPiCtx)
{
ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;

    return pF05WT_AnaTuner->u1SubSysIdx;
}

/***************************************************************************
*  F05WT_MonitorAnaTuner
*  Monitor current VIF status. If frequency offset too large, do real time VIF search again.
*  @param  *psAtvPiCtx : Pointer use to point to struct ATV_PI_CTX_T. 
*  @param  *wFreqInKHz : Pointer of current frequency (KHz).
*  @param  _BreakFct : Function pointer of the break function.
*  @param  *pvArg : Poniter of the break function parameter.
*  @retval  TRUE : Current VIF is lock or valid VIF signal is found.
*  @retval  FALSE : VIF I2C fail or valid VIF signal is not found.
****************************************************************************/
UINT8 F05WT_MonitorAnaTuner(ATV_PI_CTX_T *psAtvPiCtx, UINT32 *wFreqInKHz,
                              x_break_fct _BreakFct, void *pvArg)
{
#ifdef fc_DISABLE_AFT

return 1;

#else

BOOL fgIFStatus;
UINT8 bIFStatus, bAFCStatus;
UINT8 bATVStatus;
UINT32 wFreqDR;

    ATV_PI_CTX_T *pF05WT_AnaTuner = psAtvPiCtx;
    pF05WT_AnaTuner->isBreak = _BreakFct;
            
    if ( *wFreqInKHz > 0 )
    {

        wFreqDR = *wFreqInKHz;
        mcSHOW_DBG_MSG2(("Monitor frequency = %d\n", *wFreqInKHz));
        
        DigTunerBypassI2C(TRUE);
        fgIFStatus = fgIFPartRead(IFPartAddr[0], &bIFStatus);
        DigTunerBypassI2C(FALSE);

        if(!fgIFStatus)
        {
            mcSHOW_DBG_MSG(("[ATuner] Monitor.I2CFail\n"));
            return (FALSE);
        }
        else
        {
            bAFCStatus = bDrvMonitorAFCStatus(bIFStatus);
        }

        if ( ((bIFStatus & IF_PART_AFCWIN) == IF_PART_AFCWIN) && (bAFCStatus < 0x07) )
        {
            mcSHOW_DBG_MSG2(("In range\n"));
            return (TRUE);
        }
        else
        {
            if ( pF05WT_AnaTuner->isBreak(pvArg))
            {
                mcSHOW_DBG_MSG(("[ATuner] Monitor.BreakFct\n"));
                return (FALSE);
            }

            mcSHOW_DBG_MSG(("VIF Out of range\n"));
            
            DigTunerBypassI2C(TRUE);
            F05WT_AnaTunerSetFreq(psAtvPiCtx, wFreqDR, ATV_CONN_MODE_CHAN_SCAN);
            bATVStatus =F05WT_AnaTunerSearchNearbyFreq(psAtvPiCtx, &wFreqDR, _BreakFct, pvArg, wFreqDR);                                                    
            DigTunerBypassI2C(FALSE);
            
            if( mcCHK_MASK(bATVStatus, cpANA_LOCK_STS_TVD) ) // if TVD locked, set freq to switch LNA, for F05WT
            {
            	DigTunerBypassI2C(TRUE);
            	F05WT_AnaTunerSetFreq(psAtvPiCtx, wFreqDR, ATV_CONN_MODE_CHAN_CHANGE);
            	DigTunerBypassI2C(FALSE);
            }
            
            return bATVStatus;
        }
    }
    else
    {        
        return (TRUE);
    }

#endif
}

