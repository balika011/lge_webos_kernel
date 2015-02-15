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
/************************************************************************* 
* 
* Filename: 
* --------- 
* 
* Description: 
* ------------ 
*    
* Author: 
* ------- 
*    JaShun Wan
* 
*************************************************************************/

#include "pi_dvbt2_if.h"
#include "PD_Def_dbg.h"  // Note that fcADD_DIAG_INFO defined in PD_Def_dbg.h, This file should be placed in the first position.
#include "pi_def_dvbt2.h"
#include "TDIS.h"
#include "pi_demod.h"
#include "pd_tuner_type.h"
#include "drvcust_if.h"
#include "x_gpio.h"
#include "com_defs.h"
#include "pd_API.h"


#include "fe_tuner_common_if.h"
#include "eeprom_if.h"
#include "pd_dvbt2_if.h"
#include "x_stl_lib.h"

#include "tuner_interface_if.h"
#include "ctrl_bus.h"
#include "dmx_if.h"




//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define DVBT_MON_POLL_INTRVL           10  // 10ms
#define DVBT2_MON_POLL_INTRVL          10  // 10ms
#define DVBT_T2_MON_POLL_INTRVL          10  // 10ms
//#define DVBT_LOCK_TIME_OUT             500  // 500ms
//#define DVBT2_LOCK_TIME_OUT            500  // 500ms
//#define DVBT_T2_LOCK_TIME_OUT            500  // 500ms
#define DVBT_LOCK_TIME_OUT             3000  // 500ms
#define DVBT2_LOCK_TIME_OUT            3000  // 500ms
#define DVBT_T2_LOCK_TIME_OUT            3500//6000  //Mingsheng 120727  6000->3000; 
#define DVBT_LOCK_TIME_OUT_CNT         (DVBT_LOCK_TIME_OUT/DVBT_MON_POLL_INTRVL)
#define DVBT2_LOCK_TIME_OUT_CNT        (DVBT2_LOCK_TIME_OUT/DVBT2_MON_POLL_INTRVL)
#define DVBT_T2_LOCK_TIME_OUT_CNT        (DVBT_T2_LOCK_TIME_OUT/DVBT_T2_MON_POLL_INTRVL)

#define PATCH_DVBT2_NO_SIGNAL_UNRECOVER  0
#define PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_BER  5
#define PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_THRHOLD  10
#define PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_HOST  5

#define FTT_ENABLE 0
//#if FTT_ENABLE
#define FFT_2K                  0
#define FFT_8K                  1
#define FFT_4K                  2
#define FFT_1K                  3
#define FFT_16K                 4
#define FFT_32K                 5
#define FFT_8K_2                6
#define FFT_32K_2               7

#define FTT_LF_FREQ_LB                   0x25
#define FTT_LF_FREQ_LB_DEFAULT           0x47
//#endif

#if  fcTUNER_TYPE == cALPS_TDAG4B03A
#define DVBT_TUNER_CONN_TIME_THREHOLD  750                          //add 700ms , TDAG4B03A AGC speed too slow
#else
#define DVBT_TUNER_CONN_TIME_THREHOLD  140
#endif
#define DVBT_DEMOD_CONN_TIME_THREHOLD  300

#define DVBT2_DEMOD_CONN_TIME_THREHOLD 300


#define mcIsDVBT2() (psDvbt2DemodCtx->u1DVBSystem)

DEMOD_CTX_T2*    psDvbt2DemodCtx;

static DVBT2_PD_CTX_T *pPdCtx;

static TUNER_DEVICE_CTX_T*    pDvbtTunerDeviceCtx = NULL;

static x_break_fct      MTDVBT2isBreak;
static x_break_fct      MTDVBTisBreak;

// for Qtest
static U16  u2ToolxInfoUpdateRate = 1000;
//static U8   ucToolxInfoUpdateLvl  = 1;
//static U8   fgToolxInfoUpdate = FALSE;
static U8   fgPec = FALSE;
static U8   fgPec_dtd_y = FALSE;
static U16  u2AccEsr = 0;
static U32  u4AccTotPkt = 0;
static U32  u4AccErrPkt = 0;
static U32  u4AccCorBit = 0;
static U32  u4AccPer = 0;


static U32  u4AccTotFrm = 0;
static U32  u4AccErrBitBCH = 0;
static U32  u4AccCorBitBCH = 0;
static U32  u4AccLDPCBER = 0;    // E-9

static U32 u4TolBCHPkt2 = 0;	//toolx qtest
static U32 u4PostLDPCErrorBit2 = 0;
static U32  u4AccPstLDPCBER = 0;
static U32 u4AccTolBCHPkt2 = 0;	//toolx
static U32 u4AccPostLDPCErrorBit2 = 0;


static U32  u4AccPostViterbiBER = 0;
static U32  u4AccPostViterbiBER_Cnt = 0;
static U16  u2KeepRefreshTime = 0;

//#if (fcTUNER_TYPE == cDVBT_TUNER_ALL)
static UINT8 ucTunerType;
//#endif

// t_escape_mon_suspend move to pdCtx
//static MUTEX_T  t_escape_mon_suspend;
static BOOL     fgMonitorThreadRunning;
static THREAD_T t_monitor_thread;
//static BOOL     fgIsDisconnected = TRUE;
static BOOL     fgBreakMonitor = TRUE;
static BOOL     fg1stMonitorRun = FALSE;

#if FTT_ENABLE
//#if 0
#if defined(CC_MT5136)
static BOOL gInitDVBT2Bit = TRUE;
static UINT8 FTTModifyCnt = 0;
//static UINT8 FTTModifyThrhold = 20;  //25s
UINT8 FTTModifyThrholdCLI = 20;
UINT8 FTT_LF_FREQ_LB_CLI = 0x25;
#endif

#else
UINT8 FTTModifyThrholdCLI = 20;
UINT8 FTT_LF_FREQ_LB_CLI = 0x25;

#endif

//static BOOL     fgT2Pd_Disconnect= TRUE;
#ifdef EU_AUTO_ATP
 UINT8 gATP_TEST_ITEM_BIT = 0 ;
 UINT8 gATPFailBitMsk =0;
#endif

UCHAR DVBT2syncConf ;
UINT32 u32DelayTime = 0;
UINT32 u32PLP_ID_DelayTime = 0;//delay for IQ test
UINT8  u1Get_PLP_ID = 0;
UCHAR	ucDVBT2syncConfThr = 3;


#if DEF_T2_1P7M
UINT8  bw_test = MT5136T2_BW_8MHz;
#endif


static void PD_DVBT2_Register_LTDIS_Fct(PTD_CTX ptTDCtx);

//extern TUNER_COMMONAPI_FCT_TBL_T *pTuner_CommonAPI_Fct;
/*
#if (fcTUNER_TYPE != cDVBT_TUNER_ALL)
extern void Tuner_CommonAPI_Register_Fct(TUNER_COMMONAPI_FCT_TBL_T *pcom_Fct);
#else
extern void Tuner_CommonAPI_Register_Fct(TUNER_COMMONAPI_FCT_TBL_T *pcom_Fct, U8 tunerType);
#endif
*/
#define  DVBT_SYNC_PER_THRESHOLD  99000
#define  DVBT2_SYNC_PER_THRESHOLD  99000

/***********************************************************************/
#ifdef fcADD_DIAG_INFO
INT32 DVBT2_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        DRV_CUSTOM_TUNER_SET_TYPE_T     e_tuner_set_type,
                        VOID*                           pv_set_info,
                        SIZE_T                          z_size)
{
    INT32   retSts = RMR_OK;

    mcSHOW_DBG_MSG4(("DVBT_TunerDisc pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerDisc pPdCtx->hHalLock\n"));

    switch(e_tuner_set_type)
    {
        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_OPEN:
            // Do initialization here
            break;

        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_CLOSE:
            // Do cleanup here
            break;

        default:
            retSts = RMR_INV_SET;
            break;
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerDisc unlock pPdCtx->hHalLock\n"));
    return retSts;
}

#define DRV_CUSTOM_TUNER_DIAG_ITEMS         16

INT32 DVBT2_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        DRV_CUSTOM_TUNER_GET_TYPE_T     e_tuner_get_type,
                        VOID*                           pv_get_info,
                        SIZE_T*                         pz_size)
{
    return 0;
}
#endif

STATIC INT8 Tuner_Connect(U32 Freq, U8 sawbw, U8 AutoSearch)
{
    PARAM_SETFREQ_T SetFreqPara;
    INT8 fgRet = 0;
	INT16 s2NullPtr;

    //ITuner_OP(ITunerGetCtx(), itSetSawBw, sawbw, NULL);
    ITuner_OP(ITunerGetCtx(), itSetSawBw, sawbw, &s2NullPtr);
   
    SetFreqPara.Freq = Freq;
//    SetFreqPara.Modulation = MOD_DVBT;  // mark by iven
    SetFreqPara.Modulation = MOD_DVBT2;
    SetFreqPara.fgAutoSearch = AutoSearch;  

	mcSHOW_USER_MSG(("Start Tuner_SetFreq(%d)", Freq));
    if((ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara)) != 0) {
        mcSHOW_USER_MSG(("Failed on Tuner_SetFreq(%d)", Freq));
        fgRet = 1;
    }

    return fgRet;
}


static void MonitorProcess(VOID *pvArg)
{
    U32    loop = 0;
    U8     ucAcqCnt = 0xFF, ucRstCnt = 0xFF, ucArCoef = 0xFF;
    U8     aucData[cMAX_READ_NUM + 1];
    INT32 i4Nbch_normal [] = {32400, 38880, 43200, 48600, 51840, 54000};
    INT32 i4Nbch_short [] = {7200, 9720, 10800, 11880, 12600, 13320};   
//    INT32 i4Nbch_normalE9 [] = {30864, 25720, 23148, 20576, 19290, 18519};    // E9/i4Nbch_normal
//    INT32 i4Nbch_shortE9 [] = {138889, 102881, 92593, 84175, 79365, 75075};   

    U8    uDataPLPEFC = 0, uPLPCR =0;
    INT32 u4BCH_DataBit = 0, u4temp = 0;
//	U8    ucData = 0;
	U8    ucData2 = 0;
	#if FTT_ENABLE
//	 UCHAR TEMP = 0;
	#endif
	
    #if PATCH_DVBT2_NO_SIGNAL_UNRECOVER  
	    STATIC UINT16 u2rec_pkt_last = 0;
	    STATIC UINT16 u2drop_pkt_last = 0;
	    UINT16 u2rec_pkt_diff = 0, u2drop_pkt_diff = 0;
	    INT32  s4dmx_err_rate = 0;
	    UINT16 u2temp = 0;
        STATIC U8     ucSecCnt = 0;
	#endif

	
	//INT32  Post_LDPC_BER = 0;


    mcSHOW_DBG_MSG(("MonitorProcess start\n"));
    while (fgMonitorThreadRunning)
    {
        if (fgBreakMonitor){
            // After mcMUTEX_LOCK(pPdCtx->t_escape_mon_suspend), monitor thread will be suspended until
            // TunerAcq or TunerClose invoked.
            mcSHOW_DBG_MSG(("Entering DVBT2 mon_suspend\n"));
            mcMUTEX_LOCK(pPdCtx->t_escape_mon_suspend);
            mcSHOW_DBG_MSG(("Escape DVBT2 mon_suspend\n"));
        }
        else{
            // mutex lock
            mcSHOW_DBG_MSG4(("MonitorProcess pPdCtx->hHalLock\n"));
            mcMUTEX_LOCK(pPdCtx->hHalLock);
            mcSHOW_DBG_MSG4(("escape MonitorProcess pPdCtx->hHalLock\n"));

            mcSHOW_DBG_MSG4(("Invoke DVBT2_Monitor\n"));
            // ***** DVB-T2 Info Update ***** //
            if (!fg1stMonitorRun)
                mcDELAY_MS(100);

           mcSHOW_DBG_MSG(("mcIsDVBT2() = %d\n", mcIsDVBT2()));                    
            DVBT2_GetReg(psDvbt2DemodCtx, 0xe40, &ucData2, 1);
            mcSHOW_DBG_MSG(("0xe40 is 0x%0x\n", ucData2));      
            mcSHOW_DBG_MSG(("psDvbt2DemodCtx->u1DVBSystem is %d\n", psDvbt2DemodCtx->u1DVBSystem));
			mcSHOW_DBG_MSG(("psDvbt2DemodCtx->ucResetCnt is %d\n", psDvbt2DemodCtx->ucResetCnt));


            if(mcIsDVBT2())
            {
                DVBT2_UpdateInfo(psDvbt2DemodCtx);
				//DVBT2_GetPstLDPCBER(psDvbt2DemodCtx);
				//DVBT2_GetSQI3(psDvbt2DemodCtx);
				//#if FTT_ENABLE
				mcSHOW_DBG_MSG(("[MonitorProcess]not change 0x145c\n"));
				#if 0
				if (gInitDVBT2Bit == TRUE)
				{
					if(psDvbt2DemodCtx->fgT2TsLock)
					{   
					    FTTModifyCnt ++;
						mcSHOW_DBG_MSG(("[MonitorProcess]1 FTTModifyCnt is %d\n", FTTModifyCnt));
						if(FTTModifyCnt > FTTModifyThrholdCLI)//FTTModifyThrhold)
						{
							TEMP = FTT_LF_FREQ_LB_DEFAULT;
				   			DVBT2_SetReg(psDvbt2DemodCtx, 0x145c, &TEMP,1);
							mcSHOW_DBG_MSG(("[MonitorProcess]FTT patch over, rollback normal setting\n"));
							gInitDVBT2Bit = FALSE;
						}
					}
					else
					{
						FTTModifyCnt = 0;
						mcSHOW_DBG_MSG(("[MonitorProcess]FTTModifyCnt reset to 0\n"));
					}
				}
				else
				{
					mcSHOW_DBG_MSG(("[MonitorProcess]gInitDVBT2Bit is %d\n", gInitDVBT2Bit));
				}
				#endif



			mcSHOW_DBG_MSG(("##################  START  ##############\n"));
				
				DVBT2_GetReg(psDvbt2DemodCtx, 0x145c, &ucData2, 1);
				mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x145c = 0x%02x\n",ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x135B, &ucData2, 1);
			mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x135B = 0x%02x\n",ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1821, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1821 is 0x%0x\n", ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1822, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1822 is 0x%0x\n", ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1881, &ucData2, 1);
			mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@ucComPLPID 0x1881 is 0x%0x\n", ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1882, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@ucDataPLPID 0x1882 is 0x%0x\n", ucData2));			
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1875, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1875 is 0x%0x\n", ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1876, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1876 is 0x%0x\n", ucData2));			
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1877, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1877 is 0x%0x\n", ucData2));
			DVBT2_GetReg(psDvbt2DemodCtx, 0x1880, &ucData2, 1);
            mcSHOW_DBG_MSG(("[MonitorProcess]@@@@@@@@@@0x1880 is 0x%0x\n", ucData2));

			mcSHOW_DBG_MSG(("SNR = %d/10 dB\n", DVBT2_GetSignalSNR(psDvbt2DemodCtx)));
				
	            mcSHOW_DBG_MSG(("Post-LDPC BER = %d * 10^-9\n",  DVBT2_GetPstLDPCBER(psDvbt2DemodCtx)));
	            mcSHOW_DBG_MSG(("Post-LDPC ErrBit = %d \n", psDvbt2DemodCtx ->u4PreBCHErrorBit2));
	            mcSHOW_DBG_MSG(("Post-BCH FER = %d * 10^-5\n", DVBT2_GetPstBCHFER(psDvbt2DemodCtx)));
	            mcSHOW_DBG_MSG(("Post-BCH ErrBit = %d \n", psDvbt2DemodCtx ->u4PstBCHErrorBit2));
				mcSHOW_USER_MSG(("TsLock = %x\n", DVBT2_GetTsLock(psDvbt2DemodCtx)));
			mcSHOW_DBG_MSG(("#################  END   ###############\n"));
				
				#if 0
				mcSHOW_DBG_MSG(("================DVBT2 information start========\n"));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1400, &ucData, 1);
                mcSHOW_DBG_MSG(("[Monitor]0x1400 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1492, &ucData, 1);
                mcSHOW_DBG_MSG(("[Monitor]0x1492 is 0x%0x\n", ucData));
                DVBT2_GetReg(psDvbt2DemodCtx, 0x1493, &ucData, 1);
                mcSHOW_DBG_MSG(("[Monitor]0x1493 is 0x%0x\n", ucData));
                DVBT2_GetReg(psDvbt2DemodCtx, 0x13CB, &ucData, 1);
                mcSHOW_DBG_MSG(("[MonitorProcess]0x13CB S2 bit3~6 is 0x%0x\n", ucData&0x78));
                mcSHOW_DBG_MSG(("[MonitorProcess]0x13CB S1 bit0~2 is 0x%0x\n", ucData&0x07));
                mcSHOW_DBG_MSG(("SNR = %d/10 dB\n", DVBT2_GetSignalSNR(psDvbt2DemodCtx)));
				Post_LDPC_BER = DVBT2_GetPstLDPCBER(psDvbt2DemodCtx);
	            mcSHOW_DBG_MSG(("Post-LDPC BER = %d * 10^-9\n", Post_LDPC_BER));
	            mcSHOW_DBG_MSG(("Post-LDPC ErrBit = %d \n", psDvbt2DemodCtx ->u4PreBCHErrorBit2));
	            mcSHOW_DBG_MSG(("Post-BCH FER = %d * 10^-5\n", DVBT2_GetPstBCHFER(psDvbt2DemodCtx)));
	            mcSHOW_DBG_MSG(("Post-BCH ErrBit = %d \n", psDvbt2DemodCtx ->u4PstBCHErrorBit2));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1805, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1805 is 0x%0x\n", ucData));
			
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1335, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1335 is 0x%0x\n", ucData));			
				DVBT2_GetReg(psDvbt2DemodCtx, 0x138b, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x138b is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x130b, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x130b is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x130c, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x130c is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x130d, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x130d is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x130e, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x130e is 0x%0x\n", ucData));
	            DVBT2_GetReg(psDvbt2DemodCtx, 0x1468, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1468 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1469, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1469 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x131c, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x131c is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x131d, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x131d is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x131e, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x131e is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x131f, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x131f is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1334, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1334 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1335, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1335 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1354, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1354 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1355, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1355 is 0x%0x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1366, &ucData, 1);
	            mcSHOW_DBG_MSG(("[Monitor]0x1366 is 0x%0x\n", ucData));
				
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1330, &ucData, 1);
                mcSHOW_DBG_MSG(("[Monitor]0x1330 is 0x%0x\n", ucData));
                DVBT2_GetReg(psDvbt2DemodCtx, 0x145c, &ucData,1);
		        mcSHOW_DBG_MSG(("[MonitorProcess]reg 145c is 0x%x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1834, &ucData,1);
		        mcSHOW_DBG_MSG(("[MonitorProcess]reg 1834 is 0x%x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1835, &ucData,1);
		        mcSHOW_DBG_MSG(("[MonitorProcess]reg 1835 is 0x%x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1840, &ucData,1);
		        mcSHOW_DBG_MSG(("[MonitorProcess]reg 1840 is 0x%x\n", ucData));
				DVBT2_GetReg(psDvbt2DemodCtx, 0x1841, &ucData,1);
		        mcSHOW_DBG_MSG(("[MonitorProcess]reg 1841 is 0x%x\n", ucData));
				#endif

				// ***********************patch for signal not recover start************************
				#if PATCH_DVBT2_NO_SIGNAL_UNRECOVER  
					
					//u2rec_pkt_last = (UINT16)DMX_GetReceivedPacketNumber();
	                //u2drop_pkt_last = (UINT16)DMX_GetDroppedPacketNumber();
	                //if((psDemodCtx->fgT2TsLock) && (Post_LDPC_BER == 0))
	                if((psDvbt2DemodCtx->fgT2TsLock))//&& (Post_LDPC_BER == 0))
	                {   

					    ucSecCnt++; 

						if (ucSecCnt> PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_BER)
						{   
						    mcSHOW_DBG_MSG(("MonitorProcess: ucSecCnt >3 "));
							u2temp = (UINT16)DMX_GetReceivedPacketNumber();
							mcSHOW_DBG_MSG(("MonitorProcess: Monitor Demux Receive Pkt = %d\n", u2temp));
			                if (u2temp >= u2rec_pkt_last)
			                {
			                    u2rec_pkt_diff = u2temp - u2rec_pkt_last;
			                }
			                else
			                {
			                    u2rec_pkt_diff = 65535 - u2rec_pkt_last + u2temp;
			                }
			                u2rec_pkt_last = u2temp;
			                mcSHOW_DBG_MSG(("MonitorProcess: Monitor Demux Receive Pkt diff = %d\n", u2rec_pkt_diff));

			                u2temp = (UINT16)DMX_GetDroppedPacketNumber();
							mcSHOW_DBG_MSG(("MonitorProcess: Monitor Demux Drop Pkt = %d\n", u2temp));
			                if (u2temp >= u2drop_pkt_last)
			                {
			                    u2drop_pkt_diff = u2temp - u2drop_pkt_last;
			                }
			                else
			                {
			                    u2drop_pkt_diff = 65535 - u2drop_pkt_last + u2temp;
			                }
							u2drop_pkt_last = u2temp;

							
			                mcSHOW_DBG_MSG(("MonitorProcess: Monitor Demux Drop Pkt diff = %d\n", u2drop_pkt_diff));
			               
						    if ((u2drop_pkt_diff==0) && (u2rec_pkt_diff==0))
						    {  
						       ucSecCnt--;
							   //s4dmx_err_rate = -1;
						    }
						    else
							   s4dmx_err_rate += (((INT32)u2drop_pkt_diff * 100000) / ((INT32)u2drop_pkt_diff+(INT32)u2rec_pkt_diff ));
							mcSHOW_DBG_MSG(("MonitorProcess: Monitor s4dmx_err_rate = %d, ucSecCnt = %d \n", s4dmx_err_rate, ucSecCnt));

							if(ucSecCnt>PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_THRHOLD)
							{	

								if ((s4dmx_err_rate/(PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_THRHOLD-PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_BER))>PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_HOST)
								{
                                    mcSHOW_DBG_MSG(("MonitorProcess: Notify FW to reset demod \n"));
									DVBT2_Patch_Demod_Rst(psDvbt2DemodCtx, TRUE);
									
								}

								ucSecCnt = PATCH_DVBT2_NO_SIGNAL_UNRECOVER_CNT_BER;
								u2temp = 0;
								u2rec_pkt_diff = 0;
								u2drop_pkt_diff = 0;
								u2rec_pkt_last = 0;
								u2drop_pkt_last = 0;
								s4dmx_err_rate = 0;
								
							}
 
						}
						else
						{
							mcSHOW_DBG_MSG(("MonitorProcess: ucSecCnt = %d\n, no condition to access patch\n", ucSecCnt));
						}
	                }
					else
					{
						ucSecCnt = 0;
						u2temp = 0;
						u2rec_pkt_diff = 0;
						u2drop_pkt_diff = 0;
						u2rec_pkt_last = 0;
						u2drop_pkt_last = 0;
						s4dmx_err_rate = 0;
						mcSHOW_DBG_MSG(("MonitorProcess: TS unlock, and BER =/= 0\n"));
					}
				#endif  //#if PATCH_DVBT2_NO_SIGNAL_UNRECOVER 
				// ***********************patch for signal not recover End************************

            
                mcSHOW_DBG_MSG(("================DVBT2 information end========\n"));
                mcSHOW_DBG_MSG2(("Updata T2 Parameters\n"));                 
            }
            else
            {
                DVBT_UpdateInfo(psDvbt2DemodCtx);
                mcSHOW_DBG_MSG2(("Updata T Parameters\n"));                 
            }
            
            fg1stMonitorRun = TRUE;  // Ken, 090202, check Monitor Process actually execute, for best mux

            // ***** Qtest Info Update ***** //
            if ((fgPec)||(fgPec_dtd_y))
            {
                if(mcIsDVBT2())
                {
                    u4AccTotFrm += psDvbt2DemodCtx->u4BCHTotalPkt2;
                    u4AccErrBitBCH += psDvbt2DemodCtx->u4PstBCHErrorBit2;
                    u4AccCorBitBCH += psDvbt2DemodCtx->u4PstBCHCorBit2;

					DVBT2_GetPstLDPCBERSQI(psDvbt2DemodCtx);// toolx
					u4AccTolBCHPkt2=psDvbt2DemodCtx->u4AveTotalPkt2;
					u4AccPostLDPCErrorBit2=psDvbt2DemodCtx->u4AveErrorBit2;
					u4TolBCHPkt2 =psDvbt2DemodCtx->u4BCHTotalPkt2;
					u4PostLDPCErrorBit2 =psDvbt2DemodCtx->u4PreBCHErrorBit2;
					
                    if(psDvbt2DemodCtx ->u1NumPLP == 1)  // when single PLP, data PLP is PLP 0
                    {
                        uDataPLPEFC = psDvbt2DemodCtx->u1PLP0FEC;
                        uPLPCR = psDvbt2DemodCtx->u1PLP0CR;
                    }
                    else if(psDvbt2DemodCtx ->u1NumPLP > 1)    // when multiPLP, data PLP is PLP 1 adn common PLP is PLP 0
                    {
                        uDataPLPEFC = psDvbt2DemodCtx->u1PLP1FEC; 
                        uPLPCR = psDvbt2DemodCtx->u1PLP1CR;
                    }
                    else  // PLP number is 0
                    {
                         mcSHOW_USER_MSG(("u1NumPLP UNKNOW \n"));
                    }

                    if(uDataPLPEFC == 0){
                        u4BCH_DataBit = i4Nbch_short[uPLPCR];            	       
                    }
                    else if(uDataPLPEFC == 1){
                        u4BCH_DataBit = i4Nbch_normal[uPLPCR];            	       
                    }
                    else{
                        mcSHOW_USER_MSG(("uDataPLPEFC UNKNOW \n"));      
                    }
                    
                    if(u4BCH_DataBit == 0){
                        mcSHOW_USER_MSG(("u4BCH_DataBit Invalid \n"));
                    }
                    else{
                        u4temp = 1000000000/(u4BCH_DataBit);                  
                    }

                    if(u4AccTotFrm == 0){
                        mcSHOW_USER_MSG(("u4AccTotFrm = 0 \n"));                 
                    }
                    else{
                        u4AccLDPCBER = ((u4AccErrBitBCH +u4AccCorBitBCH) *u4temp )/u4AccTotFrm ; 						
						mcSHOW_DBG_MSG(("[MonitorProcess]u4AccTotFrm=%d,u4AccErrBitBCH=%d,u4AccErrBitBCH=%d,u4AccLDPCBER=%d\n",u4AccTotFrm,u4AccErrBitBCH,u4AccErrBitBCH,u4AccLDPCBER));
						//#if T2MINCN// toolx
						u4AccPstLDPCBER = u4temp*u4PostLDPCErrorBit2/u4TolBCHPkt2;
						mcSHOW_USER_MSG(("[MonitorProcess]u4AccPstLDPCBER=%d,u4temp=%d,u4PostLDPCErrorBit2=%d,u4TolBCHPkt2=%d\n",u4AccPstLDPCBER,u4temp,u4PostLDPCErrorBit2,u4TolBCHPkt2));
						//#endif                   
                    }
                    
                    // get rst & acq count
                    if(fgPec) 
                    {
                        mcSHOW_USER_MSG(("<%05dE-9><%05dE-9><%02d/%02d>\t(%05d/%05d, %d) <%d>\n",  
                            u4AccLDPCBER, DVBT2_GetPstLDPCBER(psDvbt2DemodCtx), 
                            psDvbt2DemodCtx->u4LDPC_Cur2, psDvbt2DemodCtx->u4LDPC_AVGIter2,
                            (u4AccErrBitBCH+u4AccCorBitBCH), u4AccTotFrm, u4BCH_DataBit, psDvbt2DemodCtx->u4PecObserveTime));                                
                        
                    }
                    else    //(fgPec_dtd_y)
                    {
                        psDvbt2DemodCtx->u2PecRefreshTime = psDvbt2DemodCtx->u2PecRefreshTime - 1; 
                        psDvbt2DemodCtx->u4PecObserveTime = psDvbt2DemodCtx->u4PecObserveTime - 1;

                        if(psDvbt2DemodCtx->u2PecRefreshTime == 0)
                        {
                            psDvbt2DemodCtx->u2PecRefreshTime = u2KeepRefreshTime;  
                                
                            mcSHOW_USER_MSG(("<%05dE-9><%05dE-9><%02d/%02d>\t(%05d/%05d, %d) <%d>\n",  
                                u4AccLDPCBER, DVBT2_GetPstLDPCBER(psDvbt2DemodCtx), 
                                psDvbt2DemodCtx->u4LDPC_Cur2, psDvbt2DemodCtx->u4LDPC_AVGIter2,
                                (u4AccErrBitBCH+u4AccCorBitBCH), u4AccTotFrm, u4BCH_DataBit, psDvbt2DemodCtx->u4PecObserveTime));                                
                        }
                            
                        if(psDvbt2DemodCtx->u4PecObserveTime == 0)
                        {
                            fgPec_dtd_y = FALSE;
                            mcSHOW_USER_MSG(("<END>\n"));
                        }                                 
                    }
                 #ifndef __MODEL_slt__
                    // print info to Toolx
//              mcPRINTF("start.pec");
                if(fgPec) 
                {
					#if 1
					DVBT2_GetPstLDPCBERSQI(psDvbt2DemodCtx);
                    mcPRINTF("start.log");//9              
            
                    mcPRINTF("%02d%01d",  LOG_TAG_FFT,  psDvbt2DemodCtx->u1Fft);//3//
                    mcPRINTF("%02d%01d",  LOG_TAG_GI,   psDvbt2DemodCtx->u1MD_GI);//3
                    //mcPRINTF("%02d%01d",  LOG_TAG_CR,   psDvbt2DemodCtx->u1CR);//3
                    if (psDvbt2DemodCtx->u1NumPLP == 1)   
					{
						mcPRINTF("%02d%01d",  LOG_TAG_CR,   psDvbt2DemodCtx->u1PLP0CR);//3
					}
					else if(psDvbt2DemodCtx->u1NumPLP > 1)
					{
						mcPRINTF("%02d%01d",  LOG_TAG_CR,   psDvbt2DemodCtx->u1PLP1CR);//3
					}
                    mcPRINTF("%02d%02d",  LOG_TAG_SNR,  DVBT2_GetSignalSNR(psDvbt2DemodCtx)/10);
                    
                    mcPRINTF("%02d%06d",  LOG_TAG_PEC_PER,     u4AccLDPCBER);//21
                    mcPRINTF("%02d%05d",  LOG_TAG_PEC_ESR,     u4BCH_DataBit);//05 
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_TOT, u4TolBCHPkt2);//07 39 
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_ERR, u4PostLDPCErrorBit2);//06 51
					#if 0
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_COR_BIT, DVBT2_GetTsLock(psDvbt2DemodCtx));
                    mcPRINTF("%02d%03d",  LOG_TAG_ACQ_CNT,     ucAcqCnt);
                    mcPRINTF("%02d%03d",  LOG_TAG_RST_CNT,     ucRstCnt);
                    mcPRINTF("%02d%03d",  LOG_TAG_AR_COEF,     ucArCoef);
					#else					
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_COR_BIT, u4AccPostLDPCErrorBit2);//63
                    mcPRINTF("%02d%010d",  LOG_TAG_ACQ_CNT,     u4AccTolBCHPkt2);//75
                    mcPRINTF("%03d",  999);
                    //mcPRINTF("%02d%03d",  LOG_TAG_AR_COEF,     ucArCoef);
					#endif
                    
  //              mcPRINTF("stop.pec\r\n");
                    mcPRINTF("stop.log\r\n");
  
  					#else
					u4temp = 1000000000/(11880);
					u4AccPstLDPCBER = u4temp*92497/3195281;
					mcPRINTF("start.log");//9              
            
                    mcPRINTF("%02d%01d",  LOG_TAG_FFT,  7);//3//
                    mcPRINTF("%02d%01d",  LOG_TAG_GI,   6);//3
                    //mcPRINTF("%02d%01d",  LOG_TAG_CR,   psDvbt2DemodCtx->u1CR);//3
                  
                    if (psDvbt2DemodCtx->u1NumPLP == 1)   
					{
						mcPRINTF("%02d%01d",  LOG_TAG_CR,   5);//3
					}
					else if(psDvbt2DemodCtx->u1NumPLP > 1)
					{
						mcPRINTF("%02d%01d",  LOG_TAG_CR,   5);//3
					}
					
                    mcPRINTF("%02d%02d",  LOG_TAG_SNR,  99);
                    
                    mcPRINTF("%02d%06d",  LOG_TAG_PEC_PER,     654321);//21
                    mcPRINTF("%02d%05d",  LOG_TAG_PEC_ESR,     11880);
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_TOT, 3195281);//39
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_ERR, 92497);
                    mcPRINTF("%02d%010d", LOG_TAG_PEC_COR_BIT, u4AccPstLDPCBER);
                    mcPRINTF("%02d%03d",  LOG_TAG_ACQ_CNT,     012);
                    mcPRINTF("%02d%03d",  LOG_TAG_RST_CNT,     013);
                    mcPRINTF("%02d%03d",  LOG_TAG_AR_COEF,     014);
                    
  //              mcPRINTF("stop.pec\r\n");
                    mcPRINTF("stop.log\r\n");
  				#endif
				
				#if 0
  					mcSHOW_USER_MSG(("[MonitorProcess]u4BCH_DataBit ,%d\n",  u4BCH_DataBit));
					mcSHOW_USER_MSG(("[MonitorProcess]u4TolBCHPkt2 ,%d\n",  u4TolBCHPkt2));
                    mcSHOW_USER_MSG(("[MonitorProcess]u4PostLDPCErrorBit2 ,%d\n",  u4PostLDPCErrorBit2));					
                    mcSHOW_USER_MSG(("[MonitorProcess]u4AccPstLDPCBER ,%d\n",  u4AccPstLDPCBER));
				#endif
                }
                else
                {
                    psDvbt2DemodCtx->u2PecRefreshTime = psDvbt2DemodCtx->u2PecRefreshTime - 1; 
                    psDvbt2DemodCtx->u4PecObserveTime = psDvbt2DemodCtx->u4PecObserveTime - 1;

                    if(psDvbt2DemodCtx->u2PecRefreshTime == 0)
                    {
                        psDvbt2DemodCtx->u2PecRefreshTime = u2KeepRefreshTime;
                        //                        mcSHOW_USER_MSG(("\n************************\n"));
                        mcSHOW_USER_MSG(("SNR = %03d/10\t",  DVBT_GetSignalSNR(psDvbt2DemodCtx)));
                        mcSHOW_USER_MSG(("SQI = %03d\t",  DVBT_GetSQI(psDvbt2DemodCtx)));
                        mcSHOW_USER_MSG(("Total_Pkt = %010d\t",  u4AccTotPkt));
                        mcSHOW_USER_MSG(("Error_Pkt = %010d\t",  u4AccErrPkt));
                        mcSHOW_USER_MSG(("Reset_Cnt = %03d\t",  ucRstCnt));
                        mcSHOW_USER_MSG(("ReAcq_Cnt = %03d\t",  ucAcqCnt));
                        mcSHOW_USER_MSG(("Acc_Per = %06d\t", u4AccPer));
                        mcSHOW_USER_MSG(("Acc_PostVBER = %06dE-5\t", (u4AccPostViterbiBER/u4AccPostViterbiBER_Cnt)));
                        mcSHOW_USER_MSG(("PEC_Sec = %05d\n",  psDvbt2DemodCtx->u4PecObserveTime));                        
                        //                        mcSHOW_USER_MSG(("\n************************\n"));
                    }

                    if(psDvbt2DemodCtx->u4PecObserveTime == 0)
                    {
                        fgPec_dtd_y = FALSE;
                    }
                }
            #else
                mcSHOW_USER_MSG(("\n************************\n"));
                mcSHOW_USER_MSG(("SNR = %03d/10\n",  DVBT_GetSignalSNR(psDvbt2DemodCtx)));
                mcSHOW_USER_MSG(("Total_Pkt = %010d\n",  u4AccTotPkt));
                mcSHOW_USER_MSG(("Error_Pkt = %010d\n",  u4AccErrPkt));
                mcSHOW_USER_MSG(("Reset_Cnt = %03d\n",  ucRstCnt));
                mcSHOW_USER_MSG(("Acc_Per = %06d", u4AccPer));
                mcSHOW_USER_MSG(("\n************************\n"));
                //lei_code_dB110601,add for SLT building warning;
                mcSHOW_DBG_MSG4(("Acq_Cnt = %03d\n",  ucAcqCnt));
                mcSHOW_DBG_MSG4(("u2KeepRefreshTime = %03d\n",  u2KeepRefreshTime));
            #endif                                                  
                    
                }
                else  // for DVBT
                {
                    u4AccTotPkt += psDvbt2DemodCtx->u2TotalPkt;
                    u4AccErrPkt += psDvbt2DemodCtx->u2ErrorPkt;
                    u4AccCorBit += psDvbt2DemodCtx->u4CorrBit;

                    u4AccPostViterbiBER += DVBT_GetPostVBer(psDvbt2DemodCtx);
                    u4AccPostViterbiBER_Cnt = u4AccPostViterbiBER_Cnt + 1;

                    if ((psDvbt2DemodCtx->u2TotalPkt==0) || (psDvbt2DemodCtx->u2ErrorPkt!=0))
                        u2AccEsr++;

                    if (u4AccTotPkt == 0)
                        u4AccPer = 200000;
                    else
                    {
                        if (u4AccErrPkt < 42950) // (0xFFFFFFFF/100000)
                            u4AccPer = (u4AccErrPkt*100000/u4AccTotPkt);
                        else if (u4AccErrPkt < 429490)
                            u4AccPer = (u4AccErrPkt*10000/u4AccTotPkt)*10;
                        else if (u4AccErrPkt < 4294900)
                            u4AccPer = (u4AccErrPkt*1000/u4AccTotPkt)*100;
                        else if (u4AccErrPkt < 42949000)
                            u4AccPer = (u4AccErrPkt*100/u4AccTotPkt)*1000;
                        else
                            u4AccPer = (u4AccErrPkt*10/u4AccTotPkt)*10000;
                    }


                    // get rst & acq count
                    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_GET_INFO5;

                    if (mcHOST_CMD(psDvbt2DemodCtx, aucData, ccOFST_DEMOD_INFO5_SIZE, 1))
                    {
                        ucAcqCnt = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO5_ACQ_COUNT);
                        ucRstCnt = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO5_RESET_COUNT);
                    }
                                
                    // get AR coef
                    DVBT_GetReg(psDvbt2DemodCtx, 0x176, &ucArCoef, 1);
                    

                    #ifndef __MODEL_slt__
                    // print info to Toolx
//              mcPRINTF("start.pec");
                        if(fgPec) 
                        {
                            mcPRINTF("start.log");
                            
                    
                            mcPRINTF("%02d%01d",  LOG_TAG_FFT,  psDvbt2DemodCtx->u1Fft);
                            mcPRINTF("%02d%01d",  LOG_TAG_GI,   psDvbt2DemodCtx->u1GI);
                            mcPRINTF("%02d%01d",  LOG_TAG_CR,   psDvbt2DemodCtx->u1CR);
                            mcPRINTF("%02d%02d",  LOG_TAG_SNR,  DVBT_GetSignalSNR(psDvbt2DemodCtx)/10);
                            
                            mcPRINTF("%02d%06d",  LOG_TAG_PEC_PER,     u4AccPer);
                            mcPRINTF("%02d%05d",  LOG_TAG_PEC_ESR,     u2AccEsr);
                            mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_TOT, u4AccTotPkt);
                            mcPRINTF("%02d%010d", LOG_TAG_PEC_PKT_ERR, u4AccErrPkt);
                            mcPRINTF("%02d%010d", LOG_TAG_PEC_COR_BIT, u4AccCorBit);
                            mcPRINTF("%02d%03d",  LOG_TAG_ACQ_CNT,     ucAcqCnt);
                            mcPRINTF("%02d%03d",  LOG_TAG_RST_CNT,     ucRstCnt);
                            mcPRINTF("%02d%03d",  LOG_TAG_AR_COEF,     ucArCoef);
                            
          //              mcPRINTF("stop.pec\r\n");
                            mcPRINTF("stop.log\r\n");
                        }
                        else
                        {
                            psDvbt2DemodCtx->u2PecRefreshTime = psDvbt2DemodCtx->u2PecRefreshTime - 1; 
                            psDvbt2DemodCtx->u4PecObserveTime = psDvbt2DemodCtx->u4PecObserveTime - 1;

                            if(psDvbt2DemodCtx->u2PecRefreshTime == 0)
                            {
                                psDvbt2DemodCtx->u2PecRefreshTime = u2KeepRefreshTime;
                                //                        mcSHOW_USER_MSG(("\n************************\n"));
                                mcSHOW_USER_MSG(("SNR = %03d/10\t",  DVBT_GetSignalSNR(psDvbt2DemodCtx)));
                                mcSHOW_USER_MSG(("SQI = %03d\t",  DVBT_GetSQI(psDvbt2DemodCtx)));
                                mcSHOW_USER_MSG(("Total_Pkt = %010d\t",  u4AccTotPkt));
                                mcSHOW_USER_MSG(("Error_Pkt = %010d\t",  u4AccErrPkt));
                                mcSHOW_USER_MSG(("Reset_Cnt = %03d\t",  ucRstCnt));
                                mcSHOW_USER_MSG(("ReAcq_Cnt = %03d\t",  ucAcqCnt));
                                mcSHOW_USER_MSG(("Acc_Per = %06d\t", u4AccPer));
                                mcSHOW_USER_MSG(("Acc_PostVBER = %06dE-5\t", (u4AccPostViterbiBER/u4AccPostViterbiBER_Cnt)));
                                mcSHOW_USER_MSG(("PEC_Sec = %05d\n",  psDvbt2DemodCtx->u4PecObserveTime));                        
                                //                        mcSHOW_USER_MSG(("\n************************\n"));
                            }

                            if(psDvbt2DemodCtx->u4PecObserveTime == 0)
                            {
                                fgPec_dtd_y = FALSE;
                            }
                        }
                    #else
                        mcSHOW_USER_MSG(("\n************************\n"));
                        mcSHOW_USER_MSG(("SNR = %03d/10\n",  DVBT_GetSignalSNR(psDvbt2DemodCtx)));
                        mcSHOW_USER_MSG(("Total_Pkt = %010d\n",  u4AccTotPkt));
                        mcSHOW_USER_MSG(("Error_Pkt = %010d\n",  u4AccErrPkt));
                        mcSHOW_USER_MSG(("Reset_Cnt = %03d\n",  ucRstCnt));
                        mcSHOW_USER_MSG(("Acc_Per = %06d", u4AccPer));
                        mcSHOW_USER_MSG(("\n************************\n"));
                        //lei_code_dB110601,add for SLT building warning;
                        mcSHOW_DBG_MSG4(("Acq_Cnt = %03d\n",  ucAcqCnt));
                        mcSHOW_DBG_MSG4(("u2KeepRefreshTime = %03d\n",  u2KeepRefreshTime));
                    #endif                
                }
            }

            // mutex unlock
            mcMUTEX_UNLOCK(pPdCtx->hHalLock);
            mcSHOW_DBG_MSG4(("escape  MonitorProcess unlock pPdCtx->hHalLock\n"));

            // Sleep 
            mcDELAY_MS(u2ToolxInfoUpdateRate); 
            loop ++;
        }
    }
    
    t_monitor_thread = (THREAD_T)NULL;
    mcSHOW_DBG_MSG(("MonitorProcess exit!\n"));
    mcTHREAD_DESTROY();
}

static S32 PD_StartMonitorEngine(void)
{
    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not 
    // directly use pv_arg. Therefore, if we want thread to share the same 
    // context as caller, we need set pv_arg as "pointer of context".
    fgMonitorThreadRunning = TRUE;
    if (mcTHREAD_CREATE(&t_monitor_thread,
            "DVBT_MonThread",
            cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
            MonitorProcess, sizeof(void*), NULL) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSHOW_DBG_MSG(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

static S32 PD_StopMonitorEngine(void)
{
//    fgMonitorThreadRunning = FALSE;  //must set to FALSE before unlock sema t_escape_mon_suspend
    while (t_monitor_thread){
        mcDELAY_MS(10); // in ms
    }
    mcSHOW_DBG_MSG(("DVBT PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}
static void setTunerTypeToEeprom(UINT8 u1TunerType){
    UINT8 u1Val1 = 0;
    UINT8 u1Val2 = 0;
    UINT8 u1Val3 = 0;
    UINT32 u4Ofst0,u4Ofst1,u4Ofst2;

    u4Ofst0 = DRVCUST_OptGet(eTunerTypeOfst0);  //read TUNER EEPROM drvcust offset address
    u4Ofst1 = DRVCUST_OptGet(eTunerTypeOfst1);
    u4Ofst2 = DRVCUST_OptGet(eTunerTypeOfst2);

    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst0, (UINT32)(void *)&u1Val1, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst1, (UINT32)(void *)&u1Val2, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst2, (UINT32)(void *)&u1Val3, 1));
    mcSHOW_USER_MSG(("read TunerType from EEPROM, addr0x%x=0x%x, addr0x%x=0x%x, addr0x%x=0x%x\r\n",u4Ofst0,u1Val1,u4Ofst1,u1Val2,u4Ofst2,u1Val3));

    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst0, (UINT32) &u1TunerType, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst0, (UINT32)(void *)&u1Val1, 1));

    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst1, (UINT32) &u1TunerType, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst1, (UINT32)(void *)&u1Val2, 1));

    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst2, (UINT32) &u1TunerType, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst2, (UINT32)(void *)&u1Val3, 1));
    mcSHOW_USER_MSG(("Modify TunerType in EEPROM, addr0x%x=0x%x, addr0x%x=0x%x, addr0x%x=0x%x\r\n",u4Ofst0,u1Val1,u4Ofst1,u1Val2,u4Ofst2,u1Val3));

    //VERIFY(0 == EEPROM_Write((UINT64)0x145, (UINT32)(void *)&u1Val, 1));
    //VERIFY(0 == EEPROM_Write((UINT64)0x4de, (UINT32)(void *)&u1Val, 1));
    //VERIFY(0 == EEPROM_Write((UINT64)0x7f6, (UINT32)(void *)&u1Val, 1));
}
//------------------------------------------------------------------------- 
/*
 *  checkValidTunerType
 *  check whether tuner Type is valid
 *  @param  u1TestTunerType   TunerType for test
 *  @retval TRUE    valid tuner type   
 *  @retval FALSE    invalid tuner type
 */
//-------------------------------------------------------------------------
#ifndef I2C_TUNER_ALL
static BOOL checkValidTunerType(UINT8 u1TestTunerType){
//todo: need refine the list
    if( (u1TestTunerType > cDVBT_TUNER_TYPE_START) && (u1TestTunerType <= cITUNER_TYPE_END) ){
        return TRUE;
    }
    else{
        return FALSE;
    }
}
#endif
//------------------------------------------------------------------------- 
/*
 *  checkTunerTypeFromEeprom
 *  1. Use current 5133_All_P_analog configuration
 *  2. tuner id exist in three additional EEPROM address for check
 *    0x145
 *    0x4DE
 *    0x7F6
 *  3. Judgement:
 *  Tuner ID invalid, ex. 0xff -> Use the default Tuner, 
 *  3 address are with the same valid Tuner ID -> Trust the tuner ID
 *  2 address are with the same valid Tuner ID -> Trust the tuner ID
 *  All address are with different Tuner ID -> Use the default Tuner
 *  @param  void
 *  @retval tunerid    return tuner id depend on the matched judgement rule
 */
//-------------------------------------------------------------------------
static UINT8 checkTunerTypeFromEeprom(void)
{
#ifdef I2C_TUNER_ALL

    UINT8 *pucBuffer =0;
    UINT16 ucByteCount =1;

    if(ICtrlBus_I2cTunerRead(270, 0xc0, pucBuffer, ucByteCount) == 0)
    {
        mcSHOW_USER_MSG(("--tuner I2C Addr = C0->Open tuner DT70WI------------\n"));
        return 0x62;
    }
    else if(ICtrlBus_I2cTunerRead(270, 0xc2, pucBuffer, ucByteCount) == 0)
    {
        mcSHOW_USER_MSG(("--tuner I2C Addr = C2->Open tuner ENV57S------------\n"));
        return 0x5d;
    }
    else
    {
        mcSHOW_USER_MSG(("--defaut I2C Addr = C2->Open tuner ENV57S------------\n"));
        return 0x5d;
    }
#else

    UINT8 u1Val1 = 0;
    UINT8 u1Val2 = 0;
    UINT8 u1Val3 = 0;
    UINT8 u1DeftType;
    UINT32 u4Ofst0,u4Ofst1,u4Ofst2,u4Default;

    u4Ofst0 = DRVCUST_OptGet(eTunerTypeOfst0);
    u4Ofst1 = DRVCUST_OptGet(eTunerTypeOfst1);
    u4Ofst2 = DRVCUST_OptGet(eTunerTypeOfst2);
    u4Default = DRVCUST_OptGet(eTunerTypeDefault);
    mcSHOW_USER_MSG(("ofst0=0x%x, ofst1=0x%x, ofst2=0x%x, def=0x%x\r\n",u4Ofst0, u4Ofst1, u4Ofst2, u4Default));

    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst0, (UINT32)(void *)&u1Val1, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst1, (UINT32)(void *)&u1Val2, 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst2, (UINT32)(void *)&u1Val3, 1));
    mcSHOW_USER_MSG(("read TunerType from EEPROM, addr0x%x=0x%x, addr0x%x=0x%x, addr0x%x=0x%x\r\n",u4Ofst0,u1Val1,u4Ofst1,u1Val2,u4Ofst2,u1Val3));

    // Ken, 081111
    if((u4Default < cDVBT_TUNER_TYPE_START) || (u4Default > cDVBT_VALID_TYPE_END))
        u1DeftType = cPANASONIC_ENV57S; //liuqu,20090716
    else
        u1DeftType = (U8) u4Default;

#if 0
    if(u4Default ==0 ){ //u4Default value is get from drv_cust setting. drv_cust no need to know exactly tunerType value
        u1DeftType = cPANASONIC_ENG37E; 
    }
    else{ //drv_cust can change EFAULT_TUNER_TYPE_DEFAULT from 0 to other value, here can mapping default tunertype to others
        u1DeftType = cSAMSUNG_DTOS40A;
    }
#endif

    /*if two of tuner types in eeprom are the same and are valid tuner type, then trust this vaule, other condition return default type*/
    if ((checkValidTunerType(u1Val1) == TRUE) && (u1Val1==u1Val2) ){
        return u1Val1;
    }
    else if((checkValidTunerType(u1Val1) == TRUE) && (u1Val1==u1Val3)){
        return u1Val1;
    }
    else if((checkValidTunerType(u1Val2) == TRUE) && (u1Val2==u1Val3)){
        return u1Val2;
    }
    else{
        setTunerTypeToEeprom(u1DeftType);   // Ken, recommend from CS/Terrence
        //return default tuner type
        return u1DeftType;
    }
#endif
}

//this API leverage from 5112, it export to analog pd code to get tuner id from EEPROM
//Jackson todo: arch. need refine
//UINT8 u1GetTunerType(DEMOD_CTX_T2 *psDemodCtx, UINT8 u1TunerType, BOOL fgAnaTunerType)
UINT8 u1GetTunerType(VOID)
{
    //return checkTunerTypeFromEeprom();
    mcSHOW_DBG_MSG(("TunerType(0x%x) from digital PD\r\n", ucTunerType));
    return (ucTunerType);
}

/***********************************************************************/
char *DVBT2_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
	CHAR    *szRetValue;

    mcSHOW_DBG_MSG4(("DVBT2_TunerGetVer pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetVer pPdCtx->hHalLock\n"));

	szRetValue = DVBT2_GetVer(psDvbt2DemodCtx);

	mcMUTEX_UNLOCK(pPdCtx->hHalLock);
	mcSHOW_DBG_MSG4(("DVBT2_TunerGetVer unlock pPdCtx->hHalLock\n"));

    return (szRetValue);    
}

/***********************************************************************/
void DVBT2_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, 
                            UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSHOW_DBG_MSG4(("DVBT_TunerSetRegSetting pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerSetRegSetting pPdCtx->hHalLock\n"));

    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerSetRegSetting unlock pPdCtx->hHalLock\n"));

}

void DVBT2_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG4(("DVBT_TunerShowRegSetting pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerShowRegSetting pPdCtx->hHalLock\n"));

    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerShowRegSetting unlock pPdCtx->hHalLock\n"));

    
}

void DVBT2_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG4(("DVBT_TunerTestI2C pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerTestI2C pPdCtx->hHalLock\n"));
    
    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerTestI2C unlock pPdCtx->hHalLock\n"));    
}

/**********************************************************************/

/**********************************************************************/
S32 DVBT2_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    S32 i4Ret = 0;
    mcSHOW_DBG_MSG4(("DVBT_TunerNimTest pPdCtx->hHalLock\n"));    
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerNimTest pPdCtx->hHalLock\n"));    

    //add your implementation here
    #ifdef CC_CAB_DEMOD_TUNER_BYPASS
    DigTunerBypassI2C(TRUE);
    #endif

    i4Ret = Tuner_Connect(666000, MT5391_BW_8MHz, FALSE);

    #ifdef CC_CAB_DEMOD_TUNER_BYPASS
    DigTunerBypassI2C(FALSE);
    #endif

    mcSHOW_DRVERR_MSG(("DVBT_TunerNimTest : %d (0:OK, 1:fail)\n", i4Ret));
    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerNimTest unlock pPdCtx->hHalLock\n"));  


    return i4Ret;
}

/**********************************************************************/
DVBT2_PD_CTX_T *DVBT_GetPdCtx(void)
{
    return pPdCtx;
}

/**********************************************************************/
STATIC VOID vResetDemod(UINT16 u2DelayTime)
{
	//UINT32 u4RstPinNum = 57;  //mingsheng 12/06/04
//   UINT32 u4RstPinNum = 30;  //mingsheng 12/06/04
#if defined(CC_MT5881)
   UINT32 u4RstPinNum = 9; 
#elif defined(CC_MT5398)
	UINT32 u4RstPinNum = 65; 
#else 
   UINT32 u4RstPinNum = 30; 
#endif
    // Config GPIO to reset demod
    mcPRINTF("vReset MT5136 Demod\n");
    if (DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum) == 0)
   {
        GPIO_SetOut(u4RstPinNum, 0);
        mcDELAY_MS(u2DelayTime);
        GPIO_SetOut(u4RstPinNum, 1);
        mcDELAY_MS(20); // Delay 20 ms for CR[DTV00030459]. Or it will init fail in rel image
        mcPRINTF(" Reset MT5136 Demod GPIO=0x%x, 0->1, %dms\r\n",u4RstPinNum, u2DelayTime);
   }
	else
	{
		mcPRINTF("No define demod reset\n");
	}
}



/**********************************************************************/
S32 DVBT2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, 
                     PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    #if defined(CC_MT5136)
	UINT8                   u1Val = 0 ;
	//UINT32                  u4Ofst = 0;
	//UINT8                   u1BrdSrcVal1 = 0;
	//UINT32                  u4BrdSrcOfst0 = 0;
	#endif
	
#ifdef __MODEL_slt__  // iven temp mark
    UINT32 u4RegValue;
#endif
	
    mcSHOW_DBG_MSG(("DVBT2_TunerOpen : Start!\n")); 

    pDvbtTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    //Create DVBT2 PD Context
    pPdCtx = (DVBT2_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBT2_PD_CTX_T));
    if (pPdCtx == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    mcSHOW_DBG_MSG(("DVBT2_TunerOpen : PD Context PASS!\n")); 
    
    // Init Disconnect Status
    pPdCtx->fgDisStatus = FALSE;
    *pptTDSpecificCtx = pPdCtx;

    if (mcSEMA_CREATE(&pPdCtx->hHalLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("DVBT2_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : SEMA PASS!\n")); 
    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);

    // Register PD API to TDMgr
    PD_DVBT2_Register_LTDIS_Fct(ptTDCtx);
    
#ifdef __MODEL_slt__  // iven temp mark
#if defined(CC_MT5396)  
    mcSHOW_DRVERR_MSG(("Reset Demod\n"));

    u4RegValue = mcRISC_Reg(0xf0000000, 0xd270);
    u4RegValue = (u4RegValue & 0xFFFFCFFF) | 0x00000000;
    mcRISC_Reg(0xf0000000, 0xd270) = u4RegValue;

    u4RegValue = mcRISC_Reg(0xf0000000, 0xd270);
    u4RegValue = (u4RegValue & 0xFFFFCFFF) | 0x00003000;
    mcRISC_Reg(0xf0000000, 0xd270) = u4RegValue;    
#elif defined(CC_MT5368)
    mcSHOW_DRVERR_MSG(("Reset Demod\n"));
    u4RegValue = mcRISC_Reg(0xf0000000, 0xd270);
    u4RegValue = (u4RegValue & 0xFF9FFFFF) | 0x00000000;
    mcRISC_Reg(0xf0000000, 0xd270) = u4RegValue;
    
    u4RegValue = mcRISC_Reg(0xf0000000, 0xd270);
    u4RegValue = (u4RegValue & 0xFF9FFFFF) | 0x00600000;
    mcRISC_Reg(0xf0000000, 0xd270) = u4RegValue;    
#endif
#endif

    psDvbt2DemodCtx = DVBT2_DemodCtxCreate();
    if (psDvbt2DemodCtx == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    //mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : Demod Context PASS!\n")); 
    
    psDvbt2DemodCtx->psTunerCtx = ITunerGetCtx();

    //link demod ctx to pd ctx
    pPdCtx->pDemodCtx = psDvbt2DemodCtx;

#if defined(CC_MT5135) || (defined(CC_MT5136) && !defined(CC_MT5880) ||(defined(CC_MT5136) && !defined(CC_MT5881))||(defined(CC_MT5136) && !defined(CC_MT5398)))//LL
    SPIBus_Init();
#endif
    //mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : SPI Init PASS!\n")); 

	//#if defined(CC_MT5136)
	#if 0
	//Read from eeprom
	u4BrdSrcOfst0 = DRVCUST_OptGet(eBrdcstSrcOfst0);
	mcSHOW_USER_MSG(("u4BrdSrcOfst0=0x%x, \r\n",u4BrdSrcOfst0)); 
	VERIFY(0 == EEPROM_Read((UINT64)u4BrdSrcOfst0, (UINT32)(void *)&u1BrdSrcVal1, 1));
	mcSHOW_USER_MSG(("read bradsrc from EEPROM, addr0x%x=0x%x\r\n",u4BrdSrcOfst0,u1BrdSrcVal1));

	//write eeprom
	u1BrdSrcVal1 = u1BrdSrcVal1+2;
	VERIFY(0 == EEPROM_Write((UINT64)u4BrdSrcOfst0, (UINT32) &u1BrdSrcVal1, 1));
	mcSHOW_USER_MSG(("write bradsrc to EEPROM, addr0x%x=0x%x\r\n",u4BrdSrcOfst0,u1BrdSrcVal1));
	
	#endif
	
	// RF Tuner registertration
	ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);
    mcSHOW_USER_MSG(("Register DVBT2 Tunertype = %d\n",((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType)); 

	// Tuner init
    ITuner_Init(ITunerGetCtx());  
    
    //Do demod init
	vResetDemod(100);

	if (DVBT2_Initialize(psDvbt2DemodCtx, DEFAULT_I2C_ADDR, TRUE))
	{
        mcSHOW_USER_MSG(("DVBT2_Initialize failed\n"));
        return (DRVAPI_TUNER_ERROR);
    }
        #if defined(CC_MT5136)
		//Mingsheng test bradcust type
		//u4Ofst = 0x51e;
		//u4Ofst = DRVCUST_OptGet(eBrdcstSrcOfst0);
		//VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&u1Val, 1));
		//mcSHOW_DRVERR_MSG(("[MS debug]DVBT2 TunerOpen EEPROM_Read 0x%0x is 0x%0x!\n",u4Ofst, u1Val));
		u1Val = DVBT2_DVBT_GetEEPromBrdcstType(psDvbt2DemodCtx);

    if (u1Val == cDEMOD_DVBT2_MODE)
    {
		    //Set TS to Tri-state for CI CAM compatibility after Power on 
		DVBT2_SetTsPathMode(psDvbt2DemodCtx, FALSE);

		mcSHOW_DBG_MSG(("DVBT2 Disconnect @ TunerOpen!!\n"));
		DVBT2_DisConnect(psDvbt2DemodCtx);   
    }
	else 
	{
		DVBT2_WO_FW_DisConnect(psDvbt2DemodCtx);
		mcSHOW_DBG_MSG(("DVBT2 DVBT2_WO_FW_DisConnect @ TunerOpen!!\n"));
	}
      #else
//mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : Demod Init PASS!\n")); 

    //Set TS to Tri-state for CI CAM compatibility after Power on 
    DVBT2_SetTsPathMode(psDvbt2DemodCtx, FALSE);

    mcSHOW_DBG_MSG(("DVBT2 Disconnect @ TunerOpen!!\n"));
    DVBT2_DisConnect(psDvbt2DemodCtx);   // add by Ken, default disconnect demod for TS_CLK EMI issue, 090114

    	#endif
    if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (mcSEMA_CREATE(&pPdCtx->t_escape_mon_suspend, X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcSHOW_DRVERR_MSG(("DVBT2_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : (mcSEMA_CREATE) PASS!\n")); 
        
        // Start Monitoring Thread
        fgBreakMonitor = TRUE;
        PD_StartMonitorEngine();
    }

    mcSHOW_USER_CRI_MSG(("DVBT2_TunerOpen : PASS!\n"));   

    return (DRVAPI_TUNER_OK);
}

S32 DVBT2_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
#ifdef CC_SUPPORT_STR
    ITuner_OP(ITunerGetCtx(), itSetTunerInit, (UINT8)FALSE, NULL);
#endif

    if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        fgBreakMonitor = TRUE; //FALSE;lei131118 for fastboot refine
        fgMonitorThreadRunning = FALSE;  //must set to FALSE before unlock sema t_escape_mon_suspend
        mcMUTEX_UNLOCK(pPdCtx->t_escape_mon_suspend);
        PD_StopMonitorEngine();
        mcMUTEX_DESTROY(pPdCtx->t_escape_mon_suspend);
    }

    DVBT2_DemodCtxDestroy(psDvbt2DemodCtx);

    mcMUTEX_DESTROY(pPdCtx->hHalLock);

	mcFREE_MEM(pPdCtx);//lei131220 for fastboot refine;
    
    return (DRVAPI_TUNER_OK);
}

S16 DVBT2_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                        DRV_CONN_TYPE_T e_conn_type)
{
    S16 s2_ret = FALSE;
    mcMUTEX_LOCK(pPdCtx->hHalLock);

#if 0 // This flow will cause info can't be switched with T/T2 changing
    if(mcIsDVBT2())
    {
        // report tuner unsync if TS unlocked or high PER 
        vDVBT2GetStatus(psDvbt2DemodCtx);
        if (DVBT2_GetTsLock(psDvbt2DemodCtx) == TRUE)
        {
            // in SDAL, TunerGetSync() is used in ch scan => don't use PER to drop ch
            // and also PER is not updated in SDAL
            //#ifndef CC_SDAL
            if (pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
                if (DVBT2_GetPstBCHFER(psDvbt2DemodCtx) < DVBT2_SYNC_PER_THRESHOLD)
            //#endif
                s2_ret = TRUE;
        }
    }
    else
    {
        // report tuner unsync if TS unlocked or high PER 
        vDVBTGetStatus(psDvbt2DemodCtx);
        if (DVBT_GetTsLock(psDvbt2DemodCtx) == TRUE)
        {
            // in SDAL, TunerGetSync() is used in ch scan => don't use PER to drop ch
            // and also PER is not updated in SDAL
            //#ifndef CC_SDAL
            if (pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
                if (DVBT_GetPER(psDvbt2DemodCtx) < DVBT_SYNC_PER_THRESHOLD)
            //#endif
                s2_ret = TRUE;
        }
    }
#else
	// report tuner unsync if TS unlocked or high PER 
	vDVBT2GetStatus(psDvbt2DemodCtx);
	if (DVBT2_GetTsLock(psDvbt2DemodCtx) == TRUE)
	{
		// in SDAL, TunerGetSync() is used in ch scan => don't use PER to drop ch
		// and also PER is not updated in SDAL
		if (pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
			if (DVBT2_GetPstBCHFER(psDvbt2DemodCtx) < DVBT2_SYNC_PER_THRESHOLD)
				s2_ret = TRUE;
	}

	// report tuner unsync if TS unlocked or high PER 
	vDVBTGetStatus(psDvbt2DemodCtx);
	if (DVBT_GetTsLock(psDvbt2DemodCtx) == TRUE)
	{
		// in SDAL, TunerGetSync() is used in ch scan => don't use PER to drop ch
		// and also PER is not updated in SDAL
		if (pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
			if (DVBT_GetPER(psDvbt2DemodCtx) < DVBT_SYNC_PER_THRESHOLD)
				s2_ret = TRUE;
	}

#endif

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    return s2_ret;
}

void DVBT2_SetTSInterface(BOOL fgParallel)
{
    mcSHOW_DBG_MSG4((" DVBT_SetTSInterface pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" escape DVBT_SetTSInterface pPdCtx->hHalLock\n"));

    DVBT2_SetTsOutput(psDvbt2DemodCtx, fgParallel);
    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" DVBT_SetTSInterface unlock pPdCtx->hHalLock\n"));
}

UINT8 DVBT2_SetTSPath(BOOL fgEnable)
{
    UINT8 u1Ret = 0;

    mcSHOW_DBG_MSG4((" DVBT_SetTSPath pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" escape DVBT_SetTSPath pPdCtx->hHalLock\n"));

    u1Ret = DVBT2_SetTsPathMode(psDvbt2DemodCtx, fgEnable);
    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" DVBT_SetTSPath unlock pPdCtx->hHalLock\n"));

    return u1Ret;
}

UINT8 DVBT2_FirstPlpIdGet(UINT8 ucNUMPlp)
{
	UINT8 u1LoopIdx    = 0;
	UINT8 u1LoopCnt, ucDataPLPAvaliable, aPLPId, u1PLPIdCheckLoop, u1PLPIdCheckIdx, u1Temp;
	UINT8 a4PLPId[4];

	// Check if T2 is MPLP
	if (ucNUMPlp > 1)
	{
		// Check all of 256 PLPID
		for (u1LoopCnt = 0; u1LoopCnt < 64; u1LoopCnt++)
		{
			// To get the data PLPID
			ucDataPLPAvaliable = vDVBT2GetPLPInfo(psDvbt2DemodCtx, a4PLPId, (U8)(u1LoopIdx * 4));

			u1PLPIdCheckIdx = 0;
			for (u1PLPIdCheckLoop = 0; u1PLPIdCheckLoop < 4; u1PLPIdCheckLoop++)
			{					
				// Check how many the data PLP is
				u1Temp = (ucDataPLPAvaliable >> u1PLPIdCheckIdx) & 0x1;
				if (u1Temp) 
				{
					aPLPId = a4PLPId[u1PLPIdCheckIdx];
					mcSHOW_DBG_MSG4(("[MPLP]First Data PLP ID = %d\n", aPLPId));
					return aPLPId;
				}

				u1PLPIdCheckIdx++;
			}

			u1LoopIdx++;
		}
	}

	return 0;
}



BOOL DVBT2_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type, 
                    void* pv_conn_info, 
                    SIZE_T z_conn_info_len, 
                    x_break_fct _BreakFct,
                    void* pvArg)
{
    TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = 
                    (TUNER_TER_DIG_TUNE_INFO_T *) pv_conn_info;
    U32     freq;
    UINT8   bw, plpID;
    U16     ii = 0;
    UINT8   fgChannelScan = FALSE;
    BOOL    fgRetSts = FALSE;
    BOOL    fgConnSts = TRUE;
    UINT8   u1DvbtDemodState = DVBT_DEMOD_STATE_UNKNOWN;
    UINT8   u1Dvbt2DemodState = DVBT2_DEMOD_STATE_UNKNOWN;    
    //UCHAR   ucData = 0x3C;   // for best mux
    UINT32  u4TickCnt, u4ConnTime;
	#if DEF_T2_1P7M
	UINT8 bw_tuner;
	#endif

	#if FTT_ENABLE
	UCHAR TEMP = 0;
	FTTModifyCnt = 0;
	#else
//	UCHAR   TEMP = 0x25;
	#endif

	mcSHOW_DBG_MSG((" [DVBT2_TunerAcq] %%%%%%%%%%%%%%%% current DataID=%d\n",pt_tuner_info->ui1_plp_id));

    mcSHOW_DBG_MSG4((" DVBT2_TunerAcq pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" escape DVBT2_TunerAcq pPdCtx->hHalLock\n"));

    u1DvbtDemodState = DVBT_DEMOD_STATE_UNKNOWN;
    u1Dvbt2DemodState = DVBT2_DEMOD_STATE_UNKNOWN;    

    MTDVBT2isBreak = _BreakFct;
	MTDVBTisBreak  = _BreakFct;

    u4TickCnt = os_get_sys_tick(); 
	#if 0 // LL test for db10-2
	#if  (defined(CC_MT5136) && defined (CC_MT5881))
psDvbt2DemodCtx->ucTsIfFormat = 0x88; // LL 0x8c to 0x8 for 5136+5881  //changed from 0x0C to 0x8C due to MT5360B, and no impact to MT539x and MT5360A
    psDvbt2DemodCtx->ucTsIfCtrl = 0x00;
    psDvbt2DemodCtx->ucTsIfRate = 0x07;
	DVBT_SetTsIf(psDvbt2DemodCtx, psDvbt2DemodCtx->ucTsIfFormat, psDvbt2DemodCtx->ucTsIfCtrl, psDvbt2DemodCtx->ucTsIfRate);// LL test
	DVBT2_SetTsIf(psDvbt2DemodCtx, psDvbt2DemodCtx->ucTsIfFormat, psDvbt2DemodCtx->ucTsIfCtrl, psDvbt2DemodCtx->ucTsIfRate);// LL test
	mcSHOW_USER_CRI_MSG(("[DVBT2_TunerAcq]DVBT_SetTsI  DVBT2_SetTsIff\n"));
#endif
#endif
	
    // Channel Scan Flag
    if(pt_tuner_info->e_mod == MOD_UNKNOWN){  
        //ucData = 0;
        fgChannelScan = TRUE;
    }
    
    // Force to turn off AR when channel scan for BestMux selection
//    DVBT2_SetArCoef(psDvbt2DemodCtx, ucData);
//    mcSHOW_USER_MSG(("//DVBT2_SetArCoef(psDvbt2DemodCtx, ucData);\n"));

    // If monitor thread is running, pause it.
    fgBreakMonitor = TRUE;  

    freq = pt_tuner_info->ui4_freq / 1000;

    switch(pt_tuner_info->e_bandwidth) 
    {
        case BW_6_MHz:
            mcSHOW_DBG_MSG(("Set BW = 6 MHz\n"));
            bw = MT5136T2_BW_6MHz;
        break;
        case BW_7_MHz:
            mcSHOW_DBG_MSG(("Set BW = 7 MHz\n"));
            bw = MT5136T2_BW_7MHz;
        break;
        case BW_8_MHz:
        default:
            mcSHOW_DBG_MSG(("Set BW = 8 MHz\n"));
            bw = MT5136T2_BW_8MHz;
        break;
    }

	#if DEF_T2_1P7M
	bw = bw_test;
	if( bw == MT5136T2_BW_1p7MHz )
		{ 
		bw_tuner = MT5136T2_BW_6MHz;
		mcSHOW_USER_MSG(("bw_tuner = 6MHz\n"));
		}
	else
		{ bw_tuner = bw;
		mcSHOW_USER_MSG(("bw_tuner = %d\n",bw));
	}
	#endif
	
	#if (defined(CC_MT5136) && defined (CC_MT5398))
	mcSHOW_USER_MSG(("[DVBT2_TunerAcq]defined(CC_MT5136) && defined (CC_MT5398)\n"));
	#endif
#if DVBT2_FROM_PHILIPS
if( bw == MT5136T2_BW_7MHz )
{
	if( (freq==198500) || (freq==212500) || (freq==226500))
	{
	DVBT2_GetReg(psDvbt2DemodCtx, 0x09, &ucTemp,1);// 1000 for B
	ucTemp = ucTemp & 0x0F;
	ucTemp = 0xB0  | ucTemp;
	DVBT2_SetReg(psDvbt2DemodCtx, 0x09, &ucTemp,1);
	//DVBT2_SetReg(psDvbt2DemodCtx, 0x08, &ucTemp,1);
	mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]set 0x08 = 0x%02x\n",ucTemp));
	}
}
else
{
		DVBT2_GetReg(psDvbt2DemodCtx, 0x09, &ucTemp,1);//500 for A
    		ucTemp = ucTemp & 0x0F;
		ucTemp = 0xA0  | ucTemp;
		DVBT2_SetReg(psDvbt2DemodCtx, 0x09, &ucTemp,1);
		//DVBT2_SetReg(psDvbt2DemodCtx, 0x08, &ucTemp,1);
		mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]set 0x08 = 0x%02x\n",ucTemp));
}
#endif
		// Set Tuner Frequency	 
#ifdef fcTUNE_NOT_START
		if (psDvbt2DemodCtx->u1TunerAcqType != ACQ_DEMOD_ONLY)	 // Do not SetFreq when do demod-only acq
#endif
		{			   
			mcSHOW_DBG_MSG(("_Freq1 = %d kHz, set hier = %d, \n", freq, pt_tuner_info->e_hierarchy_priority));
	
	
			DigTunerBypassI2C(TRUE);
			
			#if DEF_T2_1P7M
			if (Tuner_Connect(freq, bw_tuner, fgChannelScan))
			#else
			if (Tuner_Connect(freq, bw, fgChannelScan))
			#endif
				{
				mcSHOW_DBG_MSG(("Failed on Tuner_SetFreq(%d) \n", freq));
				fgConnSts = FALSE;
				
			#if 1 //lei added for CI PLUS 1.3 freq=0 connected Ok issue
				if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
				{
					fgBreakMonitor = FALSE;
					mcMUTEX_UNLOCK(pPdCtx->t_escape_mon_suspend);
				}
	
				/* Init Disconnect Status */
				pPdCtx->fgDisStatus = FALSE;
	
				mcSHOW_DBG_MSG4((" DVBT2_TunerAcq pPdCtx->hHalLock Unlock\n"));
				mcMUTEX_UNLOCK(pPdCtx->hHalLock);
				mcSHOW_DBG_MSG4((" escape DVBT2_TunerAcq pPdCtx->hHalLock : Unlock\n"));
	
				mcSHOW_DBG_MSG1(("Tuner_Connect return error, TunerAcq directly return\n"));
	   
				return fgRetSts;
				
			#endif
			}
				
			//if (ITuner_OP(ITunerGetCtx(), itSetIF, NULL, NULL)== ITUNER_OK)
			//{
			//	  DVBT_SetIF(psDvbt2DemodCtx);
			//}
	
			DigTunerBypassI2C(FALSE);
	
    mcSHOW_DBG_MSG4(("//DVBT2_Connect_HWConfig(psDvbt2DemodCtx);\n"));
//    DVBT2_Connect_HWConfig(psDvbt2DemodCtx);    
    
    mcSHOW_DBG_MSG4(("DVBT2_ChipInit(psDvbt2DemodCtx);\n"));

    DVBT2_ChipInit(psDvbt2DemodCtx);
		
    // Do FW switch to redownload fw depend on current modulation type.
    mcSHOW_DBG_MSG4(("DVBT2_FwSwitch(psDvbt2DemodCtx);\n"));   
	mcSHOW_DBG_MSG4(("Bandwidth = %d\n", bw));
	psDvbt2DemodCtx->u1ChBw = bw;


    DVBT2_FwSwitch(psDvbt2DemodCtx); /* For ReMap Use */	

	//DVBT2_DVBT_SetEEPromBrdcstType(psDvbt2DemodCtx);
	        DVBT_SetTsOutput(psDvbt2DemodCtx, FALSE);   
			DVBT2_SetTsOutput(psDvbt2DemodCtx, FALSE);
    // 090326, Split DVBT_Connect into SetNormalMode and SetBw for IFAGC startup in advance, JC
#ifdef fcTUNE_NOT_START
    if (psDvbt2DemodCtx->u1TunerAcqType != TUNE_FREQ_ONLY)   // Do not set Demod when do TUNE_FREQ_ONLY 
#endif
    {    
        DVBT2_DriverStop(psDvbt2DemodCtx);
        //mcDELAY_MS(10);
        
        //mcSHOW_USER_MSG(("DVBT2_SetNormalMode(psDvbt2DemodCtx);\n"));
        if (DVBT2_SetNormalMode(psDvbt2DemodCtx)) {
            mcSHOW_DBG_MSG(("Failed on DVBT2_SetNormalMode()\n"));
            fgConnSts = FALSE;
        }
        
        //mcSHOW_USER_MSG(("DVBT_SetNormalMode(psDvbt2DemodCtx);\n"));        
        if (DVBT_SetNormalMode(psDvbt2DemodCtx)) {
            mcSHOW_DBG_MSG(("Failed on DVBT_SetNormalMode()\n"));
            fgConnSts = FALSE;
        }        

    }
		    

#ifdef  EU_AUTO_ATP
        u4ConnTime = (os_get_sys_tick()-u4TickCnt) * x_os_drv_get_tick_period();
        mcSHOW_DBG_MSG(("DVBT tuner connection time = %dms\n", u4ConnTime));
        if ((u4ConnTime > DVBT_TUNER_CONN_TIME_THREHOLD) && (fgChannelScan))
		{
            mcSHOW_DBG_MSG(("EU Driver ATP Fail: DVBT tuner connection time = %dms, too long!!\n", u4ConnTime));
        }

        mcSHOW_DRVAPI_MSG(("ATPFailBitMskBefore =%d\n",gATPFailBitMsk));
        gATPFailBitMsk |=0X04 ;// DVBT ATP Fail !
        mcSHOW_DRVAPI_MSG(("ATPFailBitMskAfter =%x\n",gATPFailBitMsk));
#endif  

    }

    {
        #ifdef fcTUNE_NOT_START
//        if (psDvbt2DemodCtx->u1TunerAcqType != TUNE_FREQ_ONLY)   // Do not set Demod when do TUNE_FREQ_ONLY 
        #endif
        {  
            u4TickCnt = os_get_sys_tick();        
            // SLD setting 
            // ==== set AGCCurve ====        
            DVBT_SetIfAgcCurve(psDvbt2DemodCtx);        
        
            // ==== set IF ====
            DVBT_SetIF(psDvbt2DemodCtx);  
        
            // T/T2 common
            // ==== set BW ====
            DVBT_SetBW(psDvbt2DemodCtx, bw);  //ok   

            // T Only        
            // ==== set Hier ====        
            mcSHOW_DBG_MSG4(("set hier = %d, \n", pt_tuner_info->e_hierarchy_priority));
            if (pt_tuner_info->e_hierarchy_priority == HIR_PRIORITY_LOW)
                DVBT_SetHpLp(psDvbt2DemodCtx, EN_LP_DECODE); // set to LP 
            else
            	DVBT_SetHpLp(psDvbt2DemodCtx, EN_HP_DECODE); // set to HP        

               
            // Driver Start
           //DVBT_DriverStart(psDvbt2DemodCtx, REACQ_AUTO, FAST_SYNC_AUTO);  //ok
           DVBT2_DriverStart(psDvbt2DemodCtx, REACQ_AUTO, FAST_SYNC_AUTO);  


            if (fgConnSts) 
            {
                // polling lock loop
                for (ii = 0; ii < DVBT_T2_LOCK_TIME_OUT_CNT; ii++)
                {   
					psDvbt2DemodCtx->u1DVBSystem = 0; // Set flag as DVBT
					
					// get demod sync state
                    // Get T/T2 Sync
                    if(u1DvbtDemodState != DVBT_DEMOD_STATE_UNLOCK)                  
                    {
                    	u1DvbtDemodState = DVBT_GetSync(psDvbt2DemodCtx);
                    	mcSHOW_DBG_MSG(("ii =%d u1DvbtDemodState = %d\n", ii, u1DvbtDemodState));                              
                    }
					else
					{
						mcSHOW_DBG_MSG(("ii =%d u1DvbtDemodState = %d\n", ii, u1DvbtDemodState)); 
					}

					if(u1Dvbt2DemodState != DVBT2_DEMOD_STATE_UNLOCK) 
					{
                    	u1Dvbt2DemodState = DVBT2_GetSync(psDvbt2DemodCtx);
						if(u1Dvbt2DemodState ==  DVBT2_DEMOD_STATE_LOCK)
						{
							//if (DVBT2syncConf == 1)
							mcSHOW_DBG_MSG(("[DVBT2_TunerAcq] L1 CRC must=%d \n",ucDVBT2syncConfThr));
							if (DVBT2syncConf == ucDVBT2syncConfThr)	
							{
								#if FTT_ENABLE
								//if (gInitDVBT2Bit &&((psDvbt2DemodCtx->u1P1D_Fft == FFT_32K) || (psDvbt2DemodCtx->u1P1D_Fft ==FFT_32K_2)))
								if ((psDvbt2DemodCtx->u1P1D_Fft == FFT_32K) || (psDvbt2DemodCtx->u1P1D_Fft ==FFT_32K_2))
							{
                                	//TEMP = FTT_LF_FREQ_LB;
                                	TEMP = FTT_LF_FREQ_LB_CLI;
		   							DVBT2_SetReg(psDvbt2DemodCtx, 0x145c, &TEMP,1);
									
									mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]@@@@@@@@@@@@@@If 32K ,ALWAYS change the FFT trcking,set 0x145c=0x%02x\n",TEMP));
									mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]@@@@@@@@@@@@@@1st in dvbt2 turnacq , change the FFT trcking,set 0x145c=0x%02x\n",TEMP));
									//gInitDVBT2Bit = False;
                                }
								else
								{
									gInitDVBT2Bit = FALSE;
									TEMP = 0x47;
									DVBT2_SetReg(psDvbt2DemodCtx, 0x145c, &TEMP,1);
									mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]@@@@@@@@@@@@@@If not 32k,set FTT to default  , ,set 0x145c=0x%02x\n",TEMP));
									mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]1st in dvbt2 turnacq without 32k, sp no need ftt patch"));
								}
								#endif
							    mcSHOW_DBG_MSG(("ii =%d Resetcnt is %d u1Dvbt2DemodState = %d\n, and DVBT2syncConf is %d\n", ii, psDvbt2DemodCtx->ucResetCnt, u1Dvbt2DemodState, DVBT2syncConf)); 
							}
							else
							{    
							    DVBT2syncConf++;
								u1Dvbt2DemodState =  DVBT2_DEMOD_STATE_UNKNOWN;
								mcSHOW_DBG_MSG(("ii =%d Resetcnt is %d u1Dvbt2DemodState change to %d\n, and DVBT2syncConf is %d\n", ii, psDvbt2DemodCtx->ucResetCnt, u1Dvbt2DemodState, DVBT2syncConf));
							}
						}
						else 
						{
							DVBT2syncConf = 0;
							mcSHOW_DBG_MSG(("ii =%d Resetcnt is %d u1Dvbt2DemodState is %d, and DVBT2syncConf is %d\n", ii, psDvbt2DemodCtx->ucResetCnt, u1Dvbt2DemodState, DVBT2syncConf));
							
						}
					}
					else 
					{
						DVBT2syncConf = 0;
						mcSHOW_DBG_MSG(("ii =%d Resetcnt is %d u1Dvbt2DemodState is %d, and DVBT2syncConf is %d\n", ii, psDvbt2DemodCtx->ucResetCnt, u1Dvbt2DemodState, DVBT2syncConf));
					}
					
                    // if DVBT & DVBT2 both Unlock, break
                    if((u1DvbtDemodState == DVBT_DEMOD_STATE_UNLOCK) && (u1Dvbt2DemodState == DVBT2_DEMOD_STATE_UNLOCK)) 
                    {
                        mcSHOW_DBG_MSG(("ii =%d TPS unlock! DVB-T & DVB-T2 not exist!\n",ii));                    
                        fgRetSts = FALSE;
                        break;                    
                    }
                    // if DVBT lock
                    else if(u1DvbtDemodState == DVBT_DEMOD_STATE_LOCK)
                    {
                        fgRetSts = TRUE;
						u4ConnTime = (os_get_sys_tick()-u4TickCnt) * x_os_drv_get_tick_period();
                        mcSHOW_DBG_MSG(("DVBT Demod lock cost time = %dms\n", u4ConnTime));
       
                        psDvbt2DemodCtx->u1DVBSystem = 0;
						mcSHOW_DBG_MSG(("ii =%d DVBT is locked!\n",ii)); 
						DVBT_UpdateSimpleInfo(psDvbt2DemodCtx);   // Mingsheng, for context update, best mux, 120727
          
                        break;                                        
                    }
                    // if DVBT2 lock
                    else if(u1Dvbt2DemodState == DVBT2_DEMOD_STATE_LOCK)
                    {
                        fgRetSts = TRUE;
						u4ConnTime = (os_get_sys_tick()-u4TickCnt) * x_os_drv_get_tick_period();
                        mcSHOW_DBG_MSG(("DVBT2 Demod lock cost time = %dms\n", u4ConnTime));          
                        psDvbt2DemodCtx->u1DVBSystem = 1;     
						mcSHOW_DBG_MSG(("ii =%d DVBT2 L1 CRC locked!\n",ii)); 
						DVBT2_UpdateSimpleInfo(psDvbt2DemodCtx);   // Mingsheng, for context update, best mux, 120727
  
                        break;                                        
                    } 
#if 1 // 20120715 Hauze : It will break the T/T2 channel search so early when do the channel search first time
                    //break function
                    if ((MTDVBT2isBreak(pvArg)) ||(MTDVBTisBreak(pvArg)))
                    {
                        mcSHOW_USER_MSG(("break DVBT_T2_TunerAcq()!"));
                        fgRetSts = FALSE;
                        break;
                    }
#endif					
                    // delay for a while                    
                    mcDELAY_MS(DVBT_T2_MON_POLL_INTRVL);
                }   
				
                // if time-out
                if(ii >= DVBT_T2_LOCK_TIME_OUT_CNT)
                {
                    mcSHOW_DBG_MSG(("Time-Out : DVBT TPS unlock and DVBT2 L1 CRC fail!\n"));
                }
                else // check Demod lock time only in non-time-out case
                {
                    u4ConnTime = (os_get_sys_tick()-u4TickCnt) * x_os_drv_get_tick_period();
                    mcSHOW_DBG_MSG(("DVBT_Tx Demod connection time = %dms\n", u4ConnTime));
                    if(u1DvbtDemodState == DVBT_DEMOD_STATE_LOCK)
                    {
                        if ((u4ConnTime > DVBT_DEMOD_CONN_TIME_THREHOLD) && (fgChannelScan))
                        {
                            mcSHOW_DBG_MSG(("EU Driver ATP Fail: DVBT demod connection time = %dms, too long!!\n", u4ConnTime));
                        }                    
                    }
                    if(u1Dvbt2DemodState == DVBT2_DEMOD_STATE_LOCK)
                    {
                        if ((u4ConnTime > DVBT2_DEMOD_CONN_TIME_THREHOLD) && (fgChannelScan))
                        {
                            mcSHOW_DBG_MSG((" EU Driver ATP Fail: DVBT2 demod connection time = %dms, too long!!\n", u4ConnTime));
                        }                    
                    }                    
                }
                
            }
			
      	
            // update contex for best mux, 120419
            //DVBT2_UpdateInfo(psDvbt2DemodCtx);   // Pluto, for context update, best mux, 120419            
            //DVBT_UpdateInfo(psDvbt2DemodCtx);   // Ken, for context update, best mux, 081217 

           if (fgRetSts)
            {
                if(u1DvbtDemodState == DVBT_DEMOD_STATE_LOCK)
                {
                    mcSHOW_DBG_MSG(("DVBT : TPS lock, freq = %d kHz, lock time = %d ms, SNR = %d/10 dB, Hier = %d (0: unknown, 1: none, 2:hp, 3:lp)\n", 
                    	freq, ii*DVBT_MON_POLL_INTRVL, DVBT_GetSignalSNR(psDvbt2DemodCtx), DVBT_GetHier(psDvbt2DemodCtx)));
                    
                    mcSHOW_DBG_MSG4((" DVBT_SetTsOutput(psDvbt2DemodCtx, TRUE); \n"));                         
                    DVBT_SetTsOutput(psDvbt2DemodCtx, TRUE);          
                }

                if(u1Dvbt2DemodState == DVBT2_DEMOD_STATE_LOCK)
                {
                    mcSHOW_DBG_MSG(("DVBT2 : TS lock, freq = %d kHz, lock time = %d ms, SNR = %d/10 dB\n", 
                    	freq, ii*DVBT2_MON_POLL_INTRVL, DVBT2_GetSignalSNR(psDvbt2DemodCtx)));  
                    
                    mcSHOW_DBG_MSG4((" DVBT2_SetTsOutput(psDvbt2DemodCtx, TRUE);     \n"));    
					    
            		//Set Data PLP ID
					plpID = pt_tuner_info->ui1_plp_id;   //use the ID mw gives
					mcSHOW_DBG_MSG((" DVBT2 PLP ID : %d\n", plpID));  // ok
					mcSHOW_DBG_MSG((" DVBT2 PLP Num : %d\n", psDvbt2DemodCtx->u1NumPLP));
				        if (plpID == 0) plpID = DVBT2_FirstPlpIdGet(psDvbt2DemodCtx->u1NumPLP); // Get the first data PLP ID
                    mcSHOW_DBG_MSG((" Real PLP ID%d\n", plpID));  // ok

					if (psDvbt2DemodCtx->u1NumPLP > 1)
					{
					    mcSHOW_DBG_MSG(("MS debug]DVBT2 u1NumPLP >1\n"));
                
						DVBT2_SetTsPLPCombine(psDvbt2DemodCtx, 
							  				0    , // Common PLP ID for demod only
							  				plpID, // Data PLP ID
							  				TRUE , // Common PLP at path1 and data PLP at path2
							  				FALSE, // No need to enable PID selection
							  				TRUE , // Demod platform is ASIC
							  				TRUE   // Demod with SOC
							  	 			);
						//DVBT2_DemodReset(psDvbt2DemodCtx); // Reset T/T2 demod
						vDVBT2FECReset(psDvbt2DemodCtx, 0);
						//mcSHOW_USER_MSG(("[dvbt2_tuneracq] delay %dms if PLPnum>1\n",u32DelayTime));
						//mcDELAY_MS(u32DelayTime);  //Mingsheng 120815;  Data plp stable
						//mcDELAY_MS(30);  //Mingsheng 120815;  Data plp stable
						//20130527 debug ;Add 1s delay
	                     mcDELAY_MS(1000);
					}

					// Set TS output format
                    DVBT2_SetTsOutput(psDvbt2DemodCtx, TRUE);                    
                 }
      
            
            }                    
        }
        
    }
#if 0
    else
    {
        mcSHOW_USER_CRI_MSG((" DVBT2 ACQ UnKnown Mode\n"));
    }    
#endif

    if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        //++ Escape monitor suspend
        fgBreakMonitor = FALSE;
        mcMUTEX_UNLOCK(pPdCtx->t_escape_mon_suspend);
    }

    /* Init Disconnect Status */
    pPdCtx->fgDisStatus = FALSE;
    
#if 1 //rongmin20130121
        if(!mcIsDVBT2())
        {
            DVBT_UpdateInfo(psDvbt2DemodCtx);
            mcSHOW_DBG_MSG(("*************************DVBT acq updata dvbt info**********************************\n"));
        #if 0
            mcSHOW_DBG_MSG(("*************************DVBT acq start**********************************\n"));
            mcSHOW_USER_MSG(("PostV BER = %d * 10^-7\n", DVBT_GetPostVBer(psDvbt2DemodCtx)));
            mcSHOW_USER_MSG(("SQI = %d\n", DVBT_GetSQI(psDvbt2DemodCtx)));
            ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &temp4);
            mcSHOW_USER_MSG(("SSI = %d\n", temp4));
            mcSHOW_DBG_MSG(("Update T Parameters\n"));
            mcSHOW_DBG_MSG(("**************************DVBT acq end***************************\n"));
        #endif
        }
#endif

    mcSHOW_DBG_MSG4((" DVBT2_TunerAcq pPdCtx->hHalLock Unlock\n"));
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4((" escape DVBT2_TunerAcq pPdCtx->hHalLock : Unlock\n"));  
   
    return fgRetSts;
}

/**********************************************************************/
void DVBT2_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                    DRV_CONN_TYPE_T e_conn_type, 
                    SIGNAL *_pSignal)
{
    UINT8 u1BW = 2; //Mingsheng 120627 fix build warning
    UINT8 u1DvbtDemodState = DVBT_DEMOD_STATE_UNKNOWN; 
	UINT8 u1Dvbt2DemodState =DVBT_DEMOD_STATE_UNKNOWN;
    
    mcSHOW_DBG_MSG4(("DVBT_TunerGetSignal pPdCtx->hHalLock\n"));                
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerGetSignal pPdCtx->hHalLock\n"));                

    // removed, tuner HAL already did the conversion
    //_pSignal->Frequency *= 1000; // convert from kHz to Hz 
    _pSignal->SymbolRate = 0;
	_pSignal->u1DVBT_T2_indicator = TUNER_DVB_AUTO_T_T2;

	u1DvbtDemodState = DVBT_GetSync(psDvbt2DemodCtx);
    u1Dvbt2DemodState = DVBT2_GetSync(psDvbt2DemodCtx);
	mcSHOW_DBG_MSG(("[DVBT_TunerGetSignal] %% enter DVBT_TunerGetSignal,DvbtDemodState=%d,Dvbt2DemodState=%d\n",u1DvbtDemodState,u1Dvbt2DemodState)); 

    // if DVBT & DVBT2 both Unlock, break
    if((u1DvbtDemodState == DVBT_DEMOD_STATE_UNLOCK) && (u1Dvbt2DemodState == DVBT2_DEMOD_STATE_UNLOCK)) 
    {
    	_pSignal->Frequency  = (S32)(psDvbt2DemodCtx->psTunerCtx->u4RF_Freq);
		
    	// DVBT
    	_pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        _pSignal->sHierInfo.eAlpha = ALPHA_0;
        _pSignal->e_hierarchy_priority = DVBT_HIR_PRIORITY_UNKNOWN;

		// DVBT2
		_pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        _pSignal->ui1_plp_id = 0;
			
    	mcSHOW_DBG_MSG(("DVB-T TPS unlock! & DVB-T2 not exist!\n"));                                    
    }
    // if DVBT lock
    else if(u1DvbtDemodState == DVBT_DEMOD_STATE_LOCK)
    {
        vDVBTGetPara(psDvbt2DemodCtx); // update Hier, Hp/Lp, BW
        u1BW = DVBT_GetBW(psDvbt2DemodCtx);    
		_pSignal->u1DVBT_T2_indicator = TUNER_DVB_T;
		switch (psDvbt2DemodCtx->u1Mod)
		{
			case 0: _pSignal->QAMSize = 4;  break;  // QPSK
			case 1: _pSignal->QAMSize = 16; break;  // 16QAM
			case 2: _pSignal->QAMSize = 64; break;  // 64QAM
			default: _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
		}

		_pSignal->e_hierarchy_priority = DVBT_GetHier(psDvbt2DemodCtx);
    	_pSignal->sHierInfo.eAlpha = (ALPHA_INFO_T)DVBT_GetHierMode(psDvbt2DemodCtx);
    	_pSignal->sHierInfo.eInDepth = (INTERLEAVE_INFO_T)DVBT_GetIndepth(psDvbt2DemodCtx);
		_pSignal->Frequency  = (S32)(psDvbt2DemodCtx->psTunerCtx->u4RF_Freq - (DVBT_GetFreqOffset_kHz(psDvbt2DemodCtx)>>8));

		mcSHOW_DBG_MSG(("DVBT_TunerGetSignal: PRIORITY = %d (1:None, 2:HP, 3:LP)\n", _pSignal->e_hierarchy_priority));
    	mcSHOW_DBG_MSG4(("DVBT_TunerGetSignal: Alpha = %d (3: a=4)\n", _pSignal->sHierInfo.eAlpha));
    	mcSHOW_DBG_MSG4(("DVBT_TunerGetSignal: Indepth = %d \n", _pSignal->sHierInfo.eInDepth));

		switch (u1BW)
		{
			case 0:  _pSignal->e_bandwidth = BW_6_MHz;	 break;
			case 1:  _pSignal->e_bandwidth = BW_7_MHz;	 break;
			case 2:  _pSignal->e_bandwidth = BW_8_MHz;	 break;
			case 3:
			default: _pSignal->e_bandwidth = BW_UNKNOWN; break;
		}
		
    }
    // if DVBT2 lock
    else if(u1Dvbt2DemodState == DVBT2_DEMOD_STATE_LOCK)
    {
		mcSHOW_DBG_MSG(("[DVBT_TunerGetSignal] %% T2 lock \n"));
    	vDVBT2GetPara(psDvbt2DemodCtx); // update Hier, Hp/Lp, BW
        u1BW = DVBT2_GetBW(psDvbt2DemodCtx);
		_pSignal->u1DVBT_T2_indicator = TUNER_DVB_T2;
		_pSignal->Frequency  = (S32)(psDvbt2DemodCtx->psTunerCtx->u4RF_Freq);
		_pSignal->ui1_plp_id = psDvbt2DemodCtx->u1PLP1ID;
		_pSignal->e_hierarchy_priority = DVBT_HIR_PRIORITY_NONE;
		mcSHOW_DBG_MSG(("[DVBT_TunerGetSignal]psDvbt2DemodCtx->u1PLP1ID = %d,u1DVBT_T2_indicator=%d\n", psDvbt2DemodCtx->u1PLP1ID,_pSignal->u1DVBT_T2_indicator));

		switch (psDvbt2DemodCtx->u1Mod)
		{
			case 0: _pSignal->QAMSize = 4;   break;  // QPSK
			case 1: _pSignal->QAMSize = 16;  break;  // 16QAM
			case 2: _pSignal->QAMSize = 64;  break;  // 64QAM
			case 3: _pSignal->QAMSize = 256; break;  // 256QAM
			default: _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
		}

		switch (u1BW)
		{
			case 0:  _pSignal->e_bandwidth = BW_6_MHz;	 break;
			case 1:  _pSignal->e_bandwidth = BW_7_MHz;	 break;
			case 2:  _pSignal->e_bandwidth = BW_8_MHz;	 break;
			case 3:
			default: _pSignal->e_bandwidth = BW_UNKNOWN; break;
		}

		mcSHOW_DBG_MSG(("DVBT2_TunerGetSignal\n"));
		mcSHOW_DBG_MSG(("_pSignal->ui1_plp_id = %d\n", _pSignal->ui1_plp_id));
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerGetSignal unlock pPdCtx->hHalLock\n"));                    
}

/**********************************************************************/
U8   DVBT2_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                                DRV_CONN_TYPE_T e_conn_type)
{
    U8 u1_ret;
    INT32 s4Temp = 0;
    INT16 s2Temp;

    mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalLevel pPdCtx->hHalLock\n"));                
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetSignalLevel pPdCtx->hHalLock\n"));                

	DigTunerBypassI2C(TRUE);
	if((mcIsDVBT2()))
    {
    	s2Temp = ITuner_OP( ITunerGetCtx(), itGetSSIIndex_T2,0, &s4Temp);
	}
	else
	{
    	s2Temp = ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &s4Temp);
	}
	DigTunerBypassI2C(FALSE);
	
    if (s2Temp == ITUNER_OK)   // If SSI is ready, use it for signal level
    {
        u1_ret = (U8) s4Temp;
        mcSHOW_DBG_MSG(("DVBT2_TunerGetSignalLevel (SSI): %d\n", s4Temp));
    }
    else
    {
//        u1_ret = DVBT2_GetSignalLevel(psDvbt2DemodCtx);
        u1_ret = DVBT_GetSignalLevel(psDvbt2DemodCtx);
        mcSHOW_DBG_MSG(("DVBT2_TunerGetSignalLevel (IFAGC): %d\n", u1_ret));
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalLevel unlock pPdCtx->hHalLock\n"));
    
    return u1_ret;
}

/**********************************************************************/
S16 DVBT2_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                                    DRV_CONN_TYPE_T e_conn_type)
{
    mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalLeveldBm pPdCtx->hHalLock\n"));                
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetSignalLeveldBm pPdCtx->hHalLock\n"));                



    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalLeveldBm unlock pPdCtx->hHalLock\n"));
    return 0;
}

/**********************************************************************/
UINT16  DVBT2_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                                    DRV_CONN_TYPE_T e_conn_type)
{
    
    UINT8 SNR;
    
    mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalSNR pPdCtx->hHalLock\n"));                   
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetSignalSNR pPdCtx->hHalLock\n"));                    

     if(mcIsDVBT2())
     {
         SNR = DVBT2_GetSnrInfo(psDvbt2DemodCtx);     
     }
     else
     {
         SNR = DVBT_GetSnrInfo(psDvbt2DemodCtx);     
     }

    
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerGetSignalSNR unlock pPdCtx->hHalLock\n"));

//#ifdef CC_SDAL
    if(pDvbtTunerDeviceCtx->u1MWIntf == cSDAL_IF)
    {
        return (UINT16)((SNR * 10) >> 3);
    }
//#else
    else
    {
        return (UINT16)(SNR >> 3);
    }
//#endif
}

UINT16  DVBT2_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                                    DRV_CONN_TYPE_T e_conn_type)  // return value = PER*5*10^4
{    
    //INT32 u4_ret;
#ifdef __MODEL_slt__
    if(mcIsDVBT2())
    {
        if (u4AccLDPCBER >= 655340)
        {
            return 655340;
        }
        else
        {
 // because original u4AccLDPCBER is E-9 and SLT test criterion is 20, /10 to normalize to E-8, so that the SLT criterion is 20E-8
            return (UINT16) (u4AccLDPCBER/10);  
        }           
    }
    else    // DVBT
    {
        if (u4AccPer >= 50000)
        {
            return 50000;
        }
        else
        {
            return (UINT16) (u4AccPer);
        }       
    }

    
#else
    INT32 u4_ret;//lei_code_dB110601,for slt build warning
    if(mcIsDVBT2())
    {
        mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalPER_IsT2 pPdCtx->hHalLock\n"));                
        mcMUTEX_LOCK(pPdCtx->hHalLock);
        mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetSignalPER_IsT2 pPdCtx->hHalLock\n"));  
        
        u4_ret = DVBT2_GetPstBCHFER(psDvbt2DemodCtx);        

        mcMUTEX_UNLOCK(pPdCtx->hHalLock);
        mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalPER_IsT2 unlock pPdCtx->hHalLock\n")); 

        if ( u4_ret == -1 )
        {
            return 50000;
        }
        else
        {
            return (UINT16)(u4_ret >> 1);
        }        
    }
    else
    {
        mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalPER_IsT pPdCtx->hHalLock\n"));                
        mcMUTEX_LOCK(pPdCtx->hHalLock);
        mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetSignalPER_IsT pPdCtx->hHalLock\n"));                
        
        // Justin, move to update update Demod context to thread
        //DVBT_GetInfo(psDvbt2DemodCtx);
        u4_ret = DVBT_GetPER(psDvbt2DemodCtx);  


        mcMUTEX_UNLOCK(pPdCtx->hHalLock);
        mcSHOW_DBG_MSG4(("DVBT2_TunerGetSignalPER_IsT unlock pPdCtx->hHalLock\n")); 
    
        if ( u4_ret == -1 )
        {
            return 50000;
        }
        else
        {
            return (UINT16)(u4_ret >> 1);
        }
    
    }
#endif
}

/**********************************************************************/
void DVBT2_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                            MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcSHOW_DBG_MSG4(("DVBT_TunerSetMpgFmt pPdCtx->hHalLock\n"));                    
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerSetMpgFmt pPdCtx->hHalLock\n"));                        

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerSetMpgFmt unlock pPdCtx->hHalLock\n")); 

}

void DVBT2_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                        INT32 i4Argc, const CHAR** aszArgv)
{
    UCHAR   ucCmdId = 0, ucPar, ucData[256];
    U16     ii, jj, kk, u2RegAddr;
    UINT8   i4CurTop = 0;
    UINT8   ucData1;
    UINT8   temp[2];
    UINT32  temp4;
    UINT8   RegistWord[5]; 
    UINT8   InVal[4];
	PARAM_SETFREQ_T SetFreqPara;

	UINT8   ucCmdExt;
    SPECIFIC_MEMBER_EU_CTX* pEuTunerCtx = &(psDvbt2DemodCtx->psTunerCtx->specific_member.eu_ctx);
    
    if (i4Argc < 1)
    {
        mcSHOW_USER_MSG(("Cmd: \n"));
        mcSHOW_USER_MSG(("\t e [Register (UINT8)]: Set or Get 5 Register,The CLI input formats are: nim.dtd e or nim.atd e ##-##-##-##-##\n"));
        mcSHOW_USER_MSG(("\t t [Top]: SetTOP\n"));
        mcSHOW_USER_MSG(("\t d:Dump DVBT/T2 registers\n"));
        mcSHOW_USER_MSG(("\t s:show signal/ts/tps status\n"));

        mcSHOW_USER_MSG(("\t r [RegAddr] [Num]:   read  Demod register\n"));
        mcSHOW_USER_MSG(("\t w [RegAddr] [Value]: write Demod register\n")); 
        mcSHOW_USER_MSG(("\t v [1/0]:             Driver on/off\n"));
        mcSHOW_USER_MSG(("\t i : Tuner IF Parameters\n"));

        if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
        {
            mcSHOW_USER_MSG(("\t mp: Pause monitor thread\n"));
            mcSHOW_USER_MSG(("\t mr: Resume monitor thread\n"));
        }
    }

    mcSHOW_DBG_MSG4(("DVBT2_TunerDtdCommand pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("Escape DVBT2_TunerDtdCommand pPdCtx->hHalLock\n"));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);

    switch (ucCmdId)
    {
        case 'i':            
            InVal[0] = 0x88;
            InVal[1] = 0x13; //5000KHz = 0x1388
            InVal[2] = 148;
            InVal[3] = 27;

			DigTunerBypassI2C(TRUE);
            ITuner_OP(ITunerGetCtx(), itSetIF, 0, InVal);
			DigTunerBypassI2C(FALSE);
			
            mcSHOW_USER_MSG(("Setting Tuner IF Parameters\n"));
        break;
    
        case 's':
            // no monitor thread in MTAL/SDAL, do info update in nim.dtd s
            if (pDvbtTunerDeviceCtx->u1MWIntf == cMTAL_IF || pDvbtTunerDeviceCtx->u1MWIntf == cSDAL_IF)
            {
                if(mcIsDVBT2())
                {
                    DVBT2_UpdateInfo(psDvbt2DemodCtx);
                    mcSHOW_DBG_MSG(("Updata T2 Parameters\n"));                    
                }
                else
                {
                    DVBT_UpdateInfo(psDvbt2DemodCtx);
                    mcSHOW_DBG_MSG(("Updata T Parameters\n"));                        
                }
            }

            mcSHOW_USER_MSG(("=================================\n"));
            mcSHOW_USER_MSG(("Digital Frequency lower bound=%d Hz\n", pEuTunerCtx->m_s4FreqBoundLower));
            mcSHOW_USER_MSG(("Digital Frequency upper bound=%d Hz\n", pEuTunerCtx->m_s4FreqBoundUpper));
            mcSHOW_USER_MSG(("Analog Frequency lower bound=%d Hz\n", pEuTunerCtx->m_s4AnaFreqBoundLower));
            mcSHOW_USER_MSG(("Analog Frequency upper bound=%d Hz\n", pEuTunerCtx->m_s4AnaFreqBoundUpper));
            mcSHOW_USER_MSG(("=================================\n"));
            if(mcIsDVBT2())
            {
                switch (psDvbt2DemodCtx->u1L1MOD)
                {
                    case 0: mcSHOW_USER_MSG(("L1 QMode = BPSK\n"));     break;                
                    case 1: mcSHOW_USER_MSG(("L1 QMode = QPSK\n"));     break;
                    case 2: mcSHOW_USER_MSG(("L1 QMode = 16QAM\n"));    break;
                    case 3: mcSHOW_USER_MSG(("L1 QMode = 64QAM\n"));    break;                    
                    default: mcSHOW_USER_MSG(("L1 QMode = N/A QAM\n")); break;
                }
                
                switch (psDvbt2DemodCtx->u1Fft)
                {
                    case 0: mcSHOW_USER_MSG(("FFT =  2k\n"));           break;
                    case 1: mcSHOW_USER_MSG(("FFT =  8k\n"));           break;
                    case 2: mcSHOW_USER_MSG(("FFT =  4k\n"));           break;
                    case 3: mcSHOW_USER_MSG(("FFT =  1k\n"));           break;
                    case 4: mcSHOW_USER_MSG(("FFT = 16k\n"));           break;
                    case 5: mcSHOW_USER_MSG(("FFT = 32k\n"));           break;
                    case 6: mcSHOW_USER_MSG(("FFT =  8k(ALSO)\n"));     break;
                    case 7: mcSHOW_USER_MSG(("FFT = 32k(ALSO)\n"));     break;
                    default: mcSHOW_USER_MSG(("FFT = N/A\n"));          break;
                }

                switch (psDvbt2DemodCtx->u1MD_GI)
                {
                    case 0: mcSHOW_USER_MSG(("GI = 1/32\n")); break;
                    case 1: mcSHOW_USER_MSG(("GI = 1/16\n")); break;
                    case 2: mcSHOW_USER_MSG(("GI = 1/8\n"));  break;
                    case 3: mcSHOW_USER_MSG(("GI = 1/4\n"));  break;
                    case 4: mcSHOW_USER_MSG(("GI = 1/128\n"));  break;
                    case 5: mcSHOW_USER_MSG(("GI = 19/128\n")); break;
                    case 6: mcSHOW_USER_MSG(("GI = 19/256\n")); break;              
                    default: mcSHOW_USER_MSG(("GI = N/A\n")); break;
                }
                
                mcSHOW_USER_MSG(("u1NumPLP = %d\n", psDvbt2DemodCtx->u1NumPLP));                
        
                if (psDvbt2DemodCtx->u1NumPLP == 1)                     
	         	{
                    switch (psDvbt2DemodCtx->u1PLP0MOD)
                    {
                        case 0: mcSHOW_USER_MSG(("PLP_Data QMode = QPSK\n"));     break;
                        case 1: mcSHOW_USER_MSG(("PLP_Data QMode = 16QAM\n"));    break;
                        case 2: mcSHOW_USER_MSG(("PLP_Data QMode = 64QAM\n"));    break;
                        case 3: mcSHOW_USER_MSG(("PLP_Data QMode = 256QAM\n"));   break;                    
                        default: mcSHOW_USER_MSG(("PLP_Data QMode = N/A QAM\n")); break;
                    } 	         
	         	}
	         	else if (psDvbt2DemodCtx->u1NumPLP > 1)             
	         	{
                    switch (psDvbt2DemodCtx->u1PLP0MOD)
                    {
                        case 0: mcSHOW_USER_MSG(("PLP 0 QMode = QPSK\n"));     break;
                        case 1: mcSHOW_USER_MSG(("PLP 0 QMode = 16QAM\n"));    break;
                        case 2: mcSHOW_USER_MSG(("PLP 0 QMode = 64QAM\n"));    break;
                        case 3: mcSHOW_USER_MSG(("PLP 0 QMode = 256QAM\n"));   break;                    
                        default: mcSHOW_USER_MSG(("PLP 0 QMode = N/A QAM\n")); break;
                    } 	         	         
                    switch (psDvbt2DemodCtx->u1PLP1MOD)
                    { 
                        case 0: mcSHOW_USER_MSG(("PLP 1 QMode = QPSK\n"));     break;
                        case 1: mcSHOW_USER_MSG(("PLP 1 QMode = 16QAM\n"));    break;
                        case 2: mcSHOW_USER_MSG(("PLP 1 QMode = 64QAM\n"));    break;
                        case 3: mcSHOW_USER_MSG(("PLP 1 QMode = 256QAM\n"));   break;                     
                        default: mcSHOW_USER_MSG(("PLP 1 QMode = N/A QAM\n")); break;
                    }
                    
	         	}
	         
                
                // PLP1 parameters
	         	if (psDvbt2DemodCtx->u1NumPLP == 1)                     
	         	{
                    switch (psDvbt2DemodCtx->u1PLP0CR)
                   	{
                        case 0: mcSHOW_USER_MSG(("PLP_Data CR = 1/2\n"));  break;
                        case 1: mcSHOW_USER_MSG(("PLP_Data CR = 3/5\n"));  break;
                        case 2: mcSHOW_USER_MSG(("PLP_Data CR = 2/3\n"));  break;
                        case 3: mcSHOW_USER_MSG(("PLP_Data CR = 3/4\n"));  break;
                        case 4: mcSHOW_USER_MSG(("PLP_Data CR = 4/5\n"));  break;
                        case 5: mcSHOW_USER_MSG(("PLP_Data CR = 5/6\n"));  break;                    
                        default: mcSHOW_USER_MSG(("PLP_Data CR = N/A\n")); break;
                   }	         
	         	}
	         	else if(psDvbt2DemodCtx->u1NumPLP > 1)                     
	         	{
             		switch (psDvbt2DemodCtx->u1PLP0CR)
                   	{
                        case 0: mcSHOW_USER_MSG(("PLP 0 CR = 1/2\n"));  break;
                        case 1: mcSHOW_USER_MSG(("PLP 0 CR = 3/5\n"));  break;
                        case 2: mcSHOW_USER_MSG(("PLP 0 CR = 2/3\n"));  break;
                        case 3: mcSHOW_USER_MSG(("PLP 0 CR = 3/4\n"));  break;
                        case 4: mcSHOW_USER_MSG(("PLP 0 CR = 4/5\n"));  break;
                        case 5: mcSHOW_USER_MSG(("PLP 0 CR = 5/6\n"));  break;                    
                        default: mcSHOW_USER_MSG(("PLP 0 CR = N/A\n")); break;
                   	}	
					
                    switch (psDvbt2DemodCtx->u1PLP1CR)
                    {
                        case 0: mcSHOW_USER_MSG(("PLP 1 CR = 1/2\n"));  break;
                        case 1: mcSHOW_USER_MSG(("PLP 1 CR = 3/5\n"));  break;
                        case 2: mcSHOW_USER_MSG(("PLP 1 CR = 2/3\n"));  break;
                        case 3: mcSHOW_USER_MSG(("PLP 1 CR = 3/4\n"));  break;
                        case 4: mcSHOW_USER_MSG(("PLP 1 CR = 4/5\n"));  break;
                        case 5: mcSHOW_USER_MSG(("PLP 1 CR = 5/6\n"));  break;
                        default: mcSHOW_USER_MSG(("PLP 1 CR = N/A\n")); break;
                    } 	         
                    
	         	}

                
		  		 mcSHOW_USER_MSG(("Freq_Offset = %dkHz\n", DVBT2_GetFreqOffset_kHz(psDvbt2DemodCtx)));
//                		  mcSHOW_USER_MSG(("Strength = %3d\n", DVBT2_GetSignalLevel(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("Strength = %3d\n", DVBT_GetSignalLevel(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("SNR = %d/10 dB\n", DVBT2_GetSignalSNR(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("=================================\n"));         
                 mcSHOW_USER_MSG(("TsLock = %x\n", DVBT2_GetTsLock(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("=================================\n"));
                 mcSHOW_USER_MSG(("LDPC DataIte# (Cur/Avg) = (%d, %d) \n", psDvbt2DemodCtx ->u4LDPC_Cur2, psDvbt2DemodCtx ->u4LDPC_AVGIter2));
                
	          	if(psDvbt2DemodCtx->u1NumPLP > 1)                     
                     mcSHOW_USER_MSG(("LDPC ComIte# (Cur/Avg) = (%d, %d) \n", psDvbt2DemodCtx ->u4LDPC_Cur1, psDvbt2DemodCtx ->u4LDPC_AVGIter1));                                	         
	         
//                		  mcSHOW_USER_MSG(("PostV BER = %d * 10^-5\n", DVBT2_GetPostVBer(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("Post-LDPC BER = %d * 10^-9\n", DVBT2_GetPstLDPCBER(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("Post-LDPC ErrBit = %d \n", psDvbt2DemodCtx ->u4PreBCHErrorBit2));
                 mcSHOW_USER_MSG(("Post-BCH FER = %d * 10^-5\n", DVBT2_GetPstBCHFER(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("Post-BCH ErrBit = %d \n", psDvbt2DemodCtx ->u4PstBCHErrorBit2));                
                 mcSHOW_USER_MSG(("=================================\n"));

                 #if defined(CC_MT5135) || defined(CC_MT5396)|| defined(CC_MT5368)|| defined(CC_MT5398)||defined(CC_MT5880) ||defined(CC_MT5881)||defined(CC_MT5136) //EC_code_dB330: For MT5396DVBTC demod //LL
                     mcSHOW_USER_MSG(("SLD RFAGC CW = 0.%d\n", psDvbt2DemodCtx->s2SldPraRfagcCtrlWord/41));
                     mcSHOW_USER_MSG(("SLD IFAGC CW = 0.%d\n", psDvbt2DemodCtx->s2SldPraIfagcCtrlWord/41));
                     mcSHOW_USER_MSG(("SLD PRA Gain Idx = 0x%X\n", psDvbt2DemodCtx->ucSldPraGainIdx));
                     mcSHOW_USER_MSG(("SLD Target LV = %d\n", psDvbt2DemodCtx->s2SldPraOut));
                 #endif                
                 mcSHOW_USER_MSG(("=================================\n"));        
                 mcSHOW_USER_MSG(("SQI = %d\n", DVBT2_GetSQI(psDvbt2DemodCtx)));        

				 DigTunerBypassI2C(TRUE);
                 ITuner_OP( ITunerGetCtx(), itGetSSIIndex_T2, 0, &temp4);
				 DigTunerBypassI2C(FALSE);
				 
                 mcSHOW_USER_MSG(("SSI = %d\n", temp4));
				 temp4= 0;
				 
				 DigTunerBypassI2C(TRUE);
				 ITuner_OP( ITunerGetCtx(), itGetSignalLevel, 0, &temp4);
				 DigTunerBypassI2C(FALSE);
                 mcSHOW_USER_MSG(("Signal Level = %d dBuV\n", temp4));
                 mcSHOW_USER_MSG(("=================================\n"));

				 mcSHOW_USER_MSG(("Init TS BitRate from FW = %d\n", psDvbt2DemodCtx->u4InitTSBitRate));
				 mcSHOW_USER_MSG(("DJB output BitRate = %d\n", psDvbt2DemodCtx->u4DJB_TSBitRate));
				 mcSHOW_USER_MSG(("TSIF output BitRate = %d\n", psDvbt2DemodCtx->u4TSIFBitRate));
				 	
             }
             else
             { 
                 switch (psDvbt2DemodCtx->u1Mod)
                 {
                     case 0: mcSHOW_USER_MSG(("QamMode = QPSK\n"));      break;
                     case 1: mcSHOW_USER_MSG(("QamMode = 16QAM\n"));     break;
                     case 2: mcSHOW_USER_MSG(("QamMode = 64QAM\n"));     break;
                     default: mcSHOW_USER_MSG(("QamMode = N/A QAM\n"));  break;
                 }
				 
                 switch (psDvbt2DemodCtx->u1Fft)
                 {
                     case 0: mcSHOW_USER_MSG(("FFT = 2k\n"));            break;
                     case 1: mcSHOW_USER_MSG(("FFT = 8k\n"));            break;
                     case 2: mcSHOW_USER_MSG(("FFT = 4k\n"));            break;
                     default: mcSHOW_USER_MSG(("FFT = N/A\n"));          break;
                 }
				 
                 switch (psDvbt2DemodCtx->u1GI)
                 {
                     case 0: mcSHOW_USER_MSG(("GI = 1/32\n"));           break;
                     case 1: mcSHOW_USER_MSG(("GI = 1/16\n"));           break;
                     case 2: mcSHOW_USER_MSG(("GI = 1/8\n"));            break;
                     case 3: mcSHOW_USER_MSG(("GI = 1/4\n"));            break;
                     default: mcSHOW_USER_MSG(("GI = N/A\n"));           break;
                 }
				 
                 switch (psDvbt2DemodCtx->u1CR)
                 {
                     case 0: mcSHOW_USER_MSG(("CR = 1/2\n"));            break;
                     case 1: mcSHOW_USER_MSG(("CR = 2/3\n"));            break;
                     case 2: mcSHOW_USER_MSG(("CR = 3/4\n"));            break;
                     case 3: mcSHOW_USER_MSG(("CR = 5/6\n"));            break;
                     case 4: mcSHOW_USER_MSG(("CR = 7/8\n"));            break;
                     default: mcSHOW_USER_MSG(("CR = N/A\n"));           break;
                 }  
				 
                 switch (psDvbt2DemodCtx->u1Hier)
                 {
                     case 0: mcSHOW_USER_MSG(("Alpha = None\n"));        break;
                     case 1: mcSHOW_USER_MSG(("Alpha = a=1\n"));         break;
                     case 2: mcSHOW_USER_MSG(("Alpha = a=2\n"));         break;
                     case 3: mcSHOW_USER_MSG(("Alpha = a=4\n"));         break;
                     default: mcSHOW_USER_MSG(("Alpha = N/A\n"));        break;
                 }            
                
                 mcSHOW_USER_MSG(("CFO = %dkHz\n", psDvbt2DemodCtx->s2Ifo));                
                 mcSHOW_USER_MSG(("Freq_Offset = %dkHz\n", DVBT_GetFreqOffset_kHz(psDvbt2DemodCtx)/256));
                 mcSHOW_USER_MSG(("Strength = %3d\n", DVBT_GetSignalLevel(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("SNR = %d/10 dB\n", DVBT_GetSignalSNR(psDvbt2DemodCtx) ));
                 mcSHOW_USER_MSG(("=================================\n"));         
                 mcSHOW_USER_MSG(("TpsLock = %x\n", DVBT_GetTpsLock(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("TsLock = %x\n", DVBT_GetTsLock(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("=================================\n"));
                 //mcSHOW_USER_MSG(("PostV BER = %d * 10^-5\n", DVBT_GetPostVBer(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("PostV BER = %d * 10^-7\n", DVBT_GetPostVBer(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("PER = %d * 10^-5\n", DVBT_GetPER(psDvbt2DemodCtx)));
                 mcSHOW_USER_MSG(("=================================\n"));
            	 mcSHOW_USER_MSG(("SLD RFAGC CW = 0.%d\n", psDvbt2DemodCtx->s2SldPraRfAgc/41));
            	 mcSHOW_USER_MSG(("SLD IFAGC CW = 0.%d\n", psDvbt2DemodCtx->s2SldPraIfAgc/41));
            	 mcSHOW_USER_MSG(("SLD PRA Gain Idx = 0x%X\n", psDvbt2DemodCtx->ucSldPraGainIdx));
            	 mcSHOW_USER_MSG(("SLD Target LV = %d\n", psDvbt2DemodCtx->s2SldPraOut));
                 mcSHOW_USER_MSG(("SQI = %d\n", DVBT_GetSQI(psDvbt2DemodCtx)));

				 DigTunerBypassI2C(TRUE);
                 ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &temp4);      
				 DigTunerBypassI2C(FALSE);
				 
                 mcSHOW_USER_MSG(("SSI = %d\n", temp4));
                 mcSHOW_USER_MSG(("=================================\n"));
				 
				 mcSHOW_USER_MSG(("Demod Reset Num = %d\n", psDvbt2DemodCtx->u2DemodAcqNum));
             }
#ifdef EU_AUTO_ATP
            if (gATP_TEST_ITEM_BIT & 0x01)
            {
                mcSHOW_DBG_MSG(("##Start\n##Strength=%d\n##BER = %d\n##PER = %d\n##TPSLOCK = %d\n##TSLOCK = %d\n##End\n",
                DVBT_GetSignalLevel(psDvbt2DemodCtx),
                DVBT_GetPostVBer(psDvbt2DemodCtx),
                DVBT_GetPER(psDvbt2DemodCtx),
                DVBT_GetTpsLock(psDvbt2DemodCtx),
                DVBT_GetTsLock(psDvbt2DemodCtx)));
            }
            if (gATP_TEST_ITEM_BIT & 0x02)
            {
                if(DVBT_GetTsLock(psDvbt2DemodCtx))
                {
                    mcSHOW_DBG_MSG(("##Start\n##Video = ON\n##End\n" ));
                }
                else
                {
                    mcSHOW_DBG_MSG(("##Start\n##Video = OFF\n##End\n" ));
                }
            }
        break;

        case 'z':
            if (i4Argc > 1)
            {
                gATP_TEST_ITEM_BIT = StrToInt(aszArgv[1]);                           
            }
            mcSHOW_USER_MSG(("AUTO ATP debug level:%x\n",gATP_TEST_ITEM_BIT));          
#endif
        break;
      
        case 'd':        
            for (kk=0; kk<3; kk++)
            {
                if(DVBT_DumpRegisters(psDvbt2DemodCtx, ucData, kk, 8))
                {
                    mcSHOW_USER_MSG(("DVBT2 Register Page %d Dump Fail!\n", kk));
                }
                else
                {
                    mcSHOW_USER_MSG(("DVBT2 Register Page %d Dump Result:\n", kk));
                    mcSHOW_USER_MSG(("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
                    for (ii=0; ii<16; ii++)
                    {
                        mcSHOW_USER_MSG(("%02X ", ii));
                        for (jj=0; jj<16; jj++)
                        {
                            mcSHOW_USER_MSG(("%02x ", ucData[ii*16+jj]));
                        }
                        mcSHOW_USER_MSG(("\n"));
                    }
                }
            }
            break;

        case 'e':
            {
                if (i4Argc == 2)
                {
                    ii = 0;
                    jj = 0;
                    kk = 0;
                    while ( (((CHAR *) aszArgv[1])[ii] != '\0') &&( kk < 5))
                    {
                        while((((CHAR *) aszArgv[1])[ii] != '-') && (((CHAR *) aszArgv[1])[ii] != '\0')&& (jj < 2))
                        { 
                            temp[jj] = ((CHAR *) aszArgv[1])[ii];
                            ii++;
                            jj++;
                        }
                        if(jj > 2)
                        {
                            mcSHOW_USER_MSG(("error!One word's format is ## \n")); 
                            break;
                        }
                        if (kk >= 5)
                        {
                            mcSHOW_USER_MSG(("error! the format is ##-##-##-##-##,just 5 words \n")); 
                            break;
                        }

                        RegistWord[kk] = (U16)StrToHex((CHAR *)&temp,jj);
                        mcSHOW_USER_MSG(("the input word is 0x%x\n",RegistWord[kk])); 
                        kk++;
                        jj = 0;
                        ii++;
                    }

                    //ITuner_OP(psDvbt2DemodCtx->psTunerCtx, itSetReg,0, &RegistWord); 
                    DigTunerBypassI2C(TRUE);
					
                    if ((jj <= 2) &&( kk <= 5))
                    {
                        ITuner_OP( ITunerGetCtx(), itSetReg,0, &RegistWord); 
                        mcSHOW_USER_MSG(("Set Register complete!\n"));
                    }

					DigTunerBypassI2C(FALSE);
                }
                else if (i4Argc == 1)
                {     
                    for (ii = 0; ii < 5; ii++)
                        RegistWord[ii] = 0;
                    //ITuner_OP(psDvbt2DemodCtx->psTunerCtx, itGetReg,0, RegistWord); 

					DigTunerBypassI2C(TRUE);
                    ITuner_OP(ITunerGetCtx(), itGetReg,0, RegistWord); 
					DigTunerBypassI2C(FALSE);
					
                    for (ii = 0; ii < 5; ii++)
                    {
                        mcSHOW_USER_MSG(("the output word is 0x%x\n",RegistWord[ii]));  
                    }
                }
                else
                    mcSHOW_USER_MSG(("The CLI input formats are: nim.atd e or nim.atd e ##-##-##-##-## \n"));
            }
            break;

        case 't': 
            if (i4Argc ==2)
            {
            	DigTunerBypassI2C(TRUE);
				
                i4CurTop= (U8)StrToInt((CHAR *) aszArgv[1]);
                SetFreqPara.Freq = (psDvbt2DemodCtx->Frequency)/1000;
                SetFreqPara.Modulation = MOD_DVBT;
                SetFreqPara.fgAutoSearch =( psDvbt2DemodCtx->u1ChSearchMode & 0x00); //no autosearch,just set freq one time
                ITuner_OP(ITunerGetCtx(), itSetTop, i4CurTop, &ucData1);
                if((ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara)) != 0)
                {
                    mcSHOW_USER_MSG(("Failed on Tuner_SetFreq(%d)", SetFreqPara.Freq));
                    break;
                }
                mcSHOW_USER_MSG(("Set Top = 0x%x\n", i4CurTop));

				DigTunerBypassI2C(FALSE);
            }
            break;

        case 'f': 
            if (i4Argc ==2)
            {
            	DigTunerBypassI2C(TRUE);
				
                SetFreqPara.Freq = (U32)StrToInt(aszArgv[1]);
                mcSHOW_USER_MSG(("Tuner_SetFreq(%d)\n", SetFreqPara.Freq));                
                SetFreqPara.Modulation = MOD_DVBT2;
                if((ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara)) != 0)
                {
                    mcSHOW_USER_MSG(("Failed on Tuner_SetFreq(%d)\n", SetFreqPara.Freq));
                    break;
                }

				DigTunerBypassI2C(FALSE);
            }
            break;

        case 'a': 
            if (i4Argc == 2)
            {
            	DigTunerBypassI2C(TRUE);
				
                mcSHOW_USER_MSG(("Tuner Init\n"));
                ITuner_Init(ITunerGetCtx());
                SetFreqPara.Freq = (U32)StrToInt(aszArgv[1]);                
                mcSHOW_USER_MSG(("Tuner_SetFreq(%d)\n", SetFreqPara.Freq));
                SetFreqPara.Modulation = MOD_DVBT2;
                if((ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara)) != 0)
                {
                    mcSHOW_USER_MSG(("Failed on Tuner_SetFreq(%d)\n", SetFreqPara.Freq));
                    break;
                }

				DigTunerBypassI2C(FALSE);
            }
            break;            

        case 'r':
            if (i4Argc < 3)
                break;
                
            u2RegAddr = (U16)StrToInt(aszArgv[1]);
            ucPar = (U8)StrToInt(aszArgv[2]);
            if (DVBT2_GetReg(psDvbt2DemodCtx, u2RegAddr, ucData, ucPar))
            {
                mcSHOW_USER_MSG(("DVBT2 Get Register 0x%03X Fail!\n", u2RegAddr));
                break;
            }
            for (ii=0; ii<ucPar; ii++)
            {
                mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X\n", u2RegAddr+ii, ucData[ii]));
            }
            break;
	#if DEF_T2_1P7M
	case 'b':
	  	bw_test = (UINT8)StrToInt(aszArgv[1]);
		mcSHOW_USER_MSG(("nim.dtd b , set bw=%d\n", bw_test));
	break;
	#endif
	
	 case 'k':
        //UINT8   ucCmdExt;
        if(i4Argc == 1)
        {
             ucCmdExt = ((CHAR *) aszArgv[0])[1];
            if (ucCmdExt == 'v')
            {
                vDVBT2_GetAsicVer(psDvbt2DemodCtx);
            }
        }
		else if(i4Argc == 2)
        {
            //UINT8   ucCmdExt;
            ucCmdExt = ((CHAR *) aszArgv[0])[1];
            if(ucCmdExt == 'e')

            {
                UINT8  ucEarlyBreakCnt_Max;
                ucEarlyBreakCnt_Max = (U8)StrToInt(aszArgv[1]);
                mcSHOW_USER_CRI_MSG(("[DVBT2]DTD_CMD ucEarlyBreakCnt_Max=%d\n", ucEarlyBreakCnt_Max));
                DVBT2_SetEarlyBreak(psDvbt2DemodCtx, ucEarlyBreakCnt_Max);
            }
        }
		else if(i4Argc == 4)
			{
			#if 0
			psDvbt2DemodCtx->u1_wSnr  = (UINT8)StrToInt(aszArgv[1]);
			psDvbt2DemodCtx->u1_wBer= (UINT8)StrToInt(aszArgv[2]);
			psDvbt2DemodCtx->i2_topBer =(INT32)StrToInt(aszArgv[3]);
			mcSHOW_USER_CRI_MSG(("u1_wSnr=%d,u1_wBer=%d,i2_topBer=%d\n",psDvbt2DemodCtx->u1_wSnr,psDvbt2DemodCtx->u1_wBer,psDvbt2DemodCtx->i2_topBer));
			#endif
			//u32PLP_ID_DelayTime = (UINT32)StrToInt(aszArgv[1]);
			mcSHOW_USER_CRI_MSG(("[Curent]FTT_LF_FREQ_LB_CLI=%d,u32DelayTime=%d,FTTModifyThrholdCLI=%d\n",FTT_LF_FREQ_LB_CLI,u32DelayTime,FTTModifyThrholdCLI));
			FTT_LF_FREQ_LB_CLI = (UINT32)StrToInt(aszArgv[1]);
			//u32DelayTime = (UINT32)StrToInt(aszArgv[2]);
			ucDVBT2syncConfThr= (UINT32)StrToInt(aszArgv[2]);
			//u1Get_PLP_ID = (UINT32)StrToInt(aszArgv[3]);			
			FTTModifyThrholdCLI= (UINT32)StrToInt(aszArgv[3]);
			mcSHOW_USER_CRI_MSG(("[Afer CLI]FTT_LF_FREQ_LB_CLI=%d,u32DelayTime=%d,FTTModifyThrholdCLI=%d\n",FTT_LF_FREQ_LB_CLI,u32DelayTime,FTTModifyThrholdCLI));
			}
	  break;
	  
        case 'w':
            if (i4Argc < 3)
                break;

            u2RegAddr = (U16)StrToInt(aszArgv[1]);
            ucPar = (U8)StrToInt(aszArgv[2]);
            if (DVBT2_SetReg(psDvbt2DemodCtx, u2RegAddr, &ucPar, 1))
            {
                mcSHOW_USER_MSG(("DVBT2 Set Register 0x%03X Fail!\n", u2RegAddr));
                break;
            }
            mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X, OK!\n", u2RegAddr, ucPar));
            break;

        case 'v':
            if (i4Argc < 2)
                break;
            
            ucPar = StrToInt((CHAR *) aszArgv[1]);
            if (ucPar)
            {
                DVBT_DriverStart(psDvbt2DemodCtx, REACQ_AUTO, FAST_SYNC_AUTO);
                mcSHOW_USER_MSG(("Driver On!\n"));
            }
            else
            {
                DVBT_DriverStop(psDvbt2DemodCtx);
                mcSHOW_USER_MSG(("Driver Off!\n"));
            }
            break;

        case 'm':
            if(pDvbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDvbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
            {
                UINT8   ucCmdExt;

                ucCmdExt = ((CHAR *) aszArgv[0])[1];

                if (ucCmdExt == 'p')
                {
                    fgBreakMonitor = TRUE;
                }
                else if (ucCmdExt == 'r')
                {
                    fgBreakMonitor = FALSE;
                    mcMUTEX_UNLOCK(pPdCtx->t_escape_mon_suspend);
                }
            }
            break;

        case 'c':
            if (i4Argc < 2)
                break;

            ucPar = StrToInt((CHAR *) aszArgv[1]);
		
            if (ucPar)
            {
                DVBT2_Connect(psDvbt2DemodCtx, MT5391_BW_8MHz);
                mcSHOW_USER_MSG(("DVBT Connect!\n"));
            }
            else
            {
                DVBT2_DisConnect(psDvbt2DemodCtx);
                mcSHOW_USER_MSG(("DVBT Disconnect!\n"));
            }
			
	
            break;

        case 'y':    // stat PEC
            if (i4Argc > 1)
            {            
                psDvbt2DemodCtx->u4PecObserveTime = StrToInt(aszArgv[1]); // Input1: Observe time(sec)
                if (psDvbt2DemodCtx->u4PecObserveTime > 600)
                {
                    psDvbt2DemodCtx->u4PecObserveTime = 600;
                    mcSHOW_USER_MSG(("Max observation time is 600 sec!"));
                }

                psDvbt2DemodCtx->u2PecRefreshTime = StrToInt(aszArgv[2]); // Input2: Refresh time(sec)
                u2KeepRefreshTime = psDvbt2DemodCtx->u2PecRefreshTime;
                fgPec_dtd_y = TRUE;  
                mcSHOW_USER_MSG(("\n"));
                mcSHOW_USER_MSG(("u2PecRefreshCnt = %04d\n",  psDvbt2DemodCtx->u2PecRefreshTime));
                mcSHOW_USER_MSG(("u4PecObserveTime = %05d\n",  psDvbt2DemodCtx->u4PecObserveTime));
                
                if(mcIsDVBT2())
                {
                    DVBT2_UpdateInfo(psDvbt2DemodCtx);            
                    u4AccTotFrm = 0;                    
                    u4AccErrBitBCH = 0;
                    u4AccCorBitBCH = 0;
                    u4AccLDPCBER = 0;
                    mcSHOW_USER_MSG(("LDPC BER: <AccBER><CurBER><CurIter#/AvgIter#>(AccErrBit/AccTtFrm, BCH_Bitlen)<time left>\n"));
                 }
                else
                {
                    DVBT_UpdateInfo(psDvbt2DemodCtx);
                    u4AccTotPkt = 0;
                    u4AccErrPkt = 0;
                    u4AccCorBit = 0;
                    u2AccEsr = 0;
                    u4AccPer = 0;
                    u4AccPostViterbiBER = 0;
                    u4AccPostViterbiBER_Cnt = 0;                
                }
                
            }
            else
            {
                mcSHOW_USER_MSG(("\t y ObserveTime(sec) RefreshTime(sec): Start PEC \n"));
            }
            break;

        case 'z':   // stop PEC
            fgPec_dtd_y = FALSE;
            break;

        default:
            break;
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerDtdCommand unlock pPdCtx->hHalLock\n"));
}

/**********************************************************************/
void DVBT2_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, 
                        INT32 i4Argc, const CHAR** aszArgv)
{
    //UCHAR   ucCmdId = 0;
    UCHAR   ucArg1;
    UCHAR   ucCmdSize = 0;
    UINT8 tunerType;
    UINT8 ucData[256];

    mcSHOW_DBG_MSG4(("DVBTx_TunerCommand pPdCtx->hHalLock\n"));
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBTx_TunerCommand pPdCtx->hHalLock\n"));

    if (i4Argc > 0) {
        //ucCmdId = *((CHAR *) aszArgv[0]);
        ucCmdSize = x_strlen((CHAR *) aszArgv[0]);
    }

    switch (ucCmdSize) {
    case 1: // one-word cmd
        if (x_strcmp((CHAR *) aszArgv[0], "s") == 0) {
            if((fcTUNER_TYPE != cDVBT_TUNER_ALL) && (fcTUNER_TYPE != cDVBT_2K10_TUNERLST)){
                mcSHOW_USER_MSG(("Only build tuner all image can use this command\r\n"));
                break;
            }
            else{
                if(i4Argc <= 1 || i4Argc >=3 ){
                    mcSHOW_USER_MSG(("\t s [1/2/3/4/5]: Set TunerID to EEPROM (1: ENV57S, 2: FT2101, 3: TDTWS810D,4: FH2607, 5: Query TunerType in EEPROM)\n"));
                    break;
                }
            }
            mcSHOW_USER_MSG(("i4Argc=%d\r\n",i4Argc));
            ucArg1 = (U8)StrToInt(aszArgv[1]);
            mcSHOW_USER_MSG(("i4Argc=%x, ucArg1=%x\r\n",i4Argc,ucArg1));

            switch(ucArg1){
            case 1:
                mcSHOW_USER_MSG(("case 1, cPANASONIC_ENV57S=0x%x\r\n",cPANASONIC_ENV57S));
                setTunerTypeToEeprom(cPANASONIC_ENV57S);
                break;
            case 2:
                mcSHOW_USER_MSG(("case 2, cNXP_FT2101=0x%x\r\n",cNXP_FT2101));
                setTunerTypeToEeprom(cNXP_FT2101);
                break;
            case 3:
                mcSHOW_USER_MSG(("case 3, cLG_TDTWS810D=0x%x\r\n",cLG_TDTWS810D));
                setTunerTypeToEeprom(cLG_TDTWS810D);
                break;
            case 4:
                mcSHOW_USER_MSG(("case 4, cNuTune_FT2607=0x%x\r\n",cNuTune_FT2607));
                setTunerTypeToEeprom(cNuTune_FT2607);
                break;
            default:
                tunerType = checkTunerTypeFromEeprom();
                mcSHOW_USER_MSG(("tunerType decision(after check EEPROM)=0x%x\r\n",tunerType));
                break;
            }
        }
        break;

    case 2: // two-word cmd
        if (x_strcmp((CHAR *) aszArgv[0], "sh") == 0) {
            if (i4Argc > 1) 
            {
                if (StrToInt(aszArgv[1]))
                    DVBT_SetHpLp(psDvbt2DemodCtx, EN_HP_DECODE);//HP
                else
                    DVBT_SetHpLp(psDvbt2DemodCtx, EN_LP_DECODE);//LP
            }
            DVBT_GetReg(psDvbt2DemodCtx, MT5391_REG_TPS_REG_1, ucData, 1);
            mcSHOW_USER_MSG(("set hier = %d (1:HP, 0:LP)\n", !(ucData[0]&0x40) ));
            vDVBTGetPara(psDvbt2DemodCtx);
            mcSHOW_USER_MSG(("current hier = %d (1:none, 2:HP, 3:LP)\n", DVBT_GetHier(psDvbt2DemodCtx)));
            DVBT_DemodReset(psDvbt2DemodCtx); // reset to trigger MT5391_SetTsClkRate() in FW
        }
        else if (x_strcmp((CHAR *) aszArgv[0], "ar") == 0) {
            if (i4Argc > 1) 
            {
                if (StrToInt(aszArgv[1]) == 1)
                {
                    DVBT_SetArCoef(psDvbt2DemodCtx, 0x3C);   // Ar coefficiet ON
                    mcSHOW_USER_MSG(("AR Coefficient ON\n"));
                }
                else if ( StrToInt(aszArgv[1]) == 0)
                {
                    DVBT_SetArCoef(psDvbt2DemodCtx, 0x00);   // Ar coefficiet OFF
                    mcSHOW_USER_MSG(("AR Coefficient OFF\n"));
                }
            }
            else 
            {
                DVBT_GetArCoef(psDvbt2DemodCtx, &(ucData[0]));
                mcSHOW_USER_MSG(("AR Coefficient = 0x%x\n", ucData[0]));
            }
        }
        else if (x_strcmp((CHAR *) aszArgv[0], "tm") == 0)
        {
            if (i4Argc > 1)
            {
                if (StrToInt(aszArgv[1]) == 1)   // parallel
                {
                    DVBT_SetTsOutputMode(psDvbt2DemodCtx, TRUE);
                    mcSHOW_USER_MSG(("[DVBT] Set TS to parallel mode\n"));
                }
                else if ( StrToInt(aszArgv[1]) == 0)   // serial mode
                {
                    DVBT_SetTsOutputMode(psDvbt2DemodCtx, FALSE);
                    mcSHOW_USER_MSG(("[DVBT] Set TS to serial mode\n"));
                }
            }
        }
        else if (x_strcmp((CHAR *) aszArgv[0], "te") == 0) 
        {
            if (StrToInt(aszArgv[1]) == 1)   // pass-through
            {
                DVBT_SetTsPathMode(psDvbt2DemodCtx, TRUE);
                mcSHOW_USER_MSG(("[DVBT] Set TS path pass-through\n"));
            }
            else if ( StrToInt(aszArgv[1]) == 0)   // tri-state
            {
                DVBT_SetTsPathMode(psDvbt2DemodCtx, FALSE);
                mcSHOW_USER_MSG(("[DVBT] Set TS path tri-state\n"));
            }
        }
		else if (x_strcmp((CHAR *) aszArgv[0], "sp") == 0)
		{
			if (i4Argc > 1)
			{
				ucArg1 = (U8)StrToInt(aszArgv[1]);
				DVBT_SetPGAGainIndex(psDvbt2DemodCtx, ucArg1);
				mcSHOW_USER_MSG(("Set PGA Gain Index ucArg1=%d\r\n",ucArg1));

			}
		}
        break;
	case 3:
		if (x_strcmp((CHAR *) aszArgv[0], "snr") == 0) {
			if (i4Argc > 1)
			{
				psDvbt2DemodCtx->s2snr_cc = (U8)StrToInt(aszArgv[1]);
				mcSHOW_USER_MSG(("Set SNR = SNR-0.1*%d\r\n",psDvbt2DemodCtx->s2snr_cc));
			}
        }
        break;

    case 4:
        {
            SPECIFIC_MEMBER_EU_CTX* pEuTunerCtx = &(psDvbt2DemodCtx->psTunerCtx->specific_member.eu_ctx);
            if (x_strcmp((CHAR *) aszArgv[0], "stop") == 0) {
                if (i4Argc > 1) {
                    pEuTunerCtx->m_aucPara[0] = (U8)StrToHex((CHAR *) aszArgv[1], 2);
                }
                mcSHOW_USER_MSG(("TOP value = 0x%x\n", pEuTunerCtx->m_aucPara[0]));
            }
        }
        break;

    case 5:
        if (x_strcmp((CHAR *) aszArgv[0], "monon") == 0) 
        {
            if(mcIsDVBT2())
            {
                u4AccTotFrm = 0;                    
                u4AccErrBitBCH = 0;
                u4AccCorBitBCH = 0;
                u4AccLDPCBER = 0;
//                mcSHOW_USER_MSG(("LDPC BER: <AccBER><CurBER><CurIter#/AvgIter#>(AccErrBit/AccTtFrm, BCH_Bitlen)<time left>\n"));
            }        
            else
            {
                u4AccTotPkt = 0;
                u4AccErrPkt = 0;
                u4AccCorBit = 0;
                u2AccEsr    = 0;
                u4AccPer = 0;            
            }
            
            fgPec = TRUE;
        }
        break;

    case 6:
        if (x_strcmp((CHAR *) aszArgv[0], "monoff") == 0) {
            fgPec = FALSE;
        }
        break;

    case 7:
        if (x_strcmp((CHAR *) aszArgv[0], "dagcmon") == 0)
        {
            if (i4Argc == 4)
            {
                DVBT_SetDagcMon(psDvbt2DemodCtx, StrToInt(aszArgv[1]), StrToInt(aszArgv[2]), StrToInt(aszArgv[3]));
            }
        }
        break;

    case 8:
         if (x_strcmp((CHAR *) aszArgv[0], "checkreg") == 0) 
        {
            mcSHOW_USER_MSG(("DVBT basic register settings check: \n"));

            mcSHOW_USER_MSG(("DTD PLL CFG (0x2000d134) = 0x%x\tDTD PLL setting = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x134), (mcRISC_Reg(CKGEN_BASE, 0x134) & 0x00001F70)));
            mcSHOW_USER_MSG(("DTD PLL CFG (0x2000d12c) = 0x%x\tDTD PLL setting = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x12c), (mcRISC_Reg(CKGEN_BASE, 0x12c) & 0x04000000)));
            mcSHOW_USER_MSG(("DEMOD ADC (0x2000d1d0) = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x1d0)));
            mcSHOW_USER_MSG(("DEMOD ADC (0x2000d1d4) = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x1d4)));
            mcSHOW_USER_MSG(("DEMOD ADC (0x2000d21c) = 0x%x\tDEMOD ADC setting = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x21c), (mcRISC_Reg(CKGEN_BASE, 0x21c) & 0x00030003)));
            mcSHOW_USER_MSG(("PINMUX CFG (0x2000d400) = 0x%x\tPINMUX setting = 0x%x\n", mcRISC_Reg(CKGEN_BASE, 0x400), (mcRISC_Reg(CKGEN_BASE, 0x400) & 0x20000000)));
            DVBT_GetReg(psDvbt2DemodCtx, 0x93e, ucData, 1);
            mcSHOW_USER_MSG(("DVBT MODE (0x2002b93e) = 0x%x\tDVBT MODE setting = 0x%x\n", ucData[0], (ucData[0] & 0x01)));
            DVBT_GetReg(psDvbt2DemodCtx, 0x4a0, ucData, 4);
            mcSHOW_USER_MSG(("RISC WAIT TIME (0x2002b4a0) = 0x%x\n", (*((UINT32*)ucData))));

            DVBT_GetReg(psDvbt2DemodCtx, 0x00, ucData, 1);
            mcSHOW_USER_MSG(("uP CTRL REG= 0x%02x\n", ucData[0]));

            DVBT_GetReg(psDvbt2DemodCtx, 0x0D, ucData, 2);
            mcSHOW_USER_MSG(("uP TOGGLE= 0x%02x\n", ucData[0]));
            mcSHOW_USER_MSG(("uP RUN_TAG= 0x%02x\n", ucData[1]));

            DVBT_GetReg(psDvbt2DemodCtx, 0x1A0, ucData, 2);
            mcSHOW_USER_MSG(("IFAGC Target Level Set= 0x%02x%02x\n", ucData[1], ucData[0]));

            DVBT_GetReg(psDvbt2DemodCtx, 0x1D7, ucData, 2);
            mcSHOW_USER_MSG(("IFAGC Target Level Out= 0x%02x%02x\n", ucData[1], ucData[0]));
        }
        break;

    default:
        mcSHOW_USER_MSG(("Cmd: \n"));
        mcSHOW_USER_MSG(("\t s [1/2/3/4/5]: Set TunerID to EEPROM (1: ENV57S, 2: FT2101, 3: TDTWS810D,4: FH2607, 5: Query TunerType in EEPROM)\n"));
        mcSHOW_USER_MSG(("\t monon:    (period level) turn on monitor for Toolx info update\n"));
        mcSHOW_USER_MSG(("\t monoff:   turn off monitor for Toolx info update\n"));
        mcSHOW_USER_MSG(("\t pecon:    turn on  PEC\n"));
        mcSHOW_USER_MSG(("\t pecoff:   turn off PEC\n"));
        mcSHOW_USER_MSG(("\t sh:       set hierarchical (1: HP, 0: LP)\n"));
        mcSHOW_USER_MSG(("\t stop:     Set TOP\n"));  
        mcSHOW_USER_MSG(("\t checkreg:  Basic register settings check"));
        mcSHOW_USER_MSG(("\t ar [0/1]: AR Coefficient [Off/On]\n"));
        mcSHOW_USER_MSG(("\t te [0(disable)/1(enable)]      : ts enable\n"));
        mcSHOW_USER_MSG(("\t tm [0(serial)/1(parallel)]     : ts mode\n"));
        mcSHOW_USER_MSG(("\t dagcmon [0(off)/1(on)] [monTime(20)] [monConfTh(10)] : Set DAGC monitor parameters\n"));

        mcSHOW_USER_MSG(("\r\n"));
        mcSHOW_USER_MSG(("\r\n"));
        break;
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerCommand unlock pPdCtx->hHalLock\n")); 
}

/**********************************************************************/
void DVBT2_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    // No implement
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
}

void *GetDigiDvbtTunerCtx(void)
{
    return psDvbt2DemodCtx->psTunerCtx;
}

/**********************************************************************/
void  DVBT2_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    if(pPdCtx->fgDisStatus == TRUE)
    {
        /* Has Disconnected, return */
        mcSHOW_DBG_MSG(("DVBT2 Has Disconnect!\n"));
        return;
    }
    mcSHOW_DBG_MSG4(("DVBT2_TunerDisc pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("Escape DVBT2_TunerDisc pPdCtx->hHalLock\n"));

    fgBreakMonitor = TRUE;    
    
    DVBT2_DisConnect(psDvbt2DemodCtx);

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT2_TunerDisc unlock pPdCtx->hHalLock\n"));

    /* set disconnect status */
    pPdCtx->fgDisStatus = TRUE;    
}


INT16 DVBT2_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type, VOID *pAnazAttribute, SIZE_T* pzAnaAttributeLen)
{
    TUNER_DIG_ATTRIBUTE_T *pAttri;
	TUNER_DVBT2_PLP_LIST_INFO sPlpIdList;
	UINT8 u1Temp, u1LoopCnt, u1LoopIdx, u1PLPIdCheckLoop, u1PLPIdCheckIdx;
	UINT8 aPLPId[255] = {0};
	UINT8 a4PLPId[4] = {0};
    UINT8 ucDataPLPAvaliable, nNumDataPLPs, u1I;
    INT32 temp;
    UINT16 u2Temp;
    INT16 s2Temp;
    SPECIFIC_MEMBER_EU_CTX* pEuTunerCtx = &(psDvbt2DemodCtx->psTunerCtx->specific_member.eu_ctx);

    mcSHOW_DBG_MSG4(("DVBT2_TunerGetAttribute pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT2_TunerGetAttribute pPdCtx->hHalLock\n")); 
	mcSHOW_DBG_MSG(("[DVBT2_TunerGetAttribute]Para2 = %d\n",e_get_type)); 
    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER:
        //DVBT_UpdateInfo(psDvbt2DemodCtx);   // Ken, for context update, best mux, 081217
        // JC, 090202, if context not updated, update here, 090202
        if(mcIsDVBT2())
		{
	        *pzAnaAttributeLen = sizeof(INT32);
	        //temp = DVBT2_GetSQI(psDvbt2DemodCtx);
			temp = DVBT2_GetPstLDPCBER(psDvbt2DemodCtx);
			temp = temp/1000;
			mcSHOW_DBG_MSG(("[DVBT2_TunerGetAttribute]-->DVBT2_GetPstLDPCBER\n"));
	        x_memcpy(pAnazAttribute, &temp, *pzAnaAttributeLen);
		}
		else
		{
        if (!fg1stMonitorRun)
        {
            vDVBTGetInfo(psDvbt2DemodCtx);
            mcSHOW_DBG_MSG(("DVBT_TunerGetAttribute: TUNER2_GET_TYPE_BER not enough time to update\n"));
        }
        *pzAnaAttributeLen = sizeof(INT32);
        temp = DVBT_GetPostVBer(psDvbt2DemodCtx);
        x_memcpy(pAnazAttribute, &temp, *pzAnaAttributeLen);
		}
        break;

    case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        pAttri = (TUNER_DIG_ATTRIBUTE_T *) pAnazAttribute;
        pAttri->ui4_lower_bound_freq = pEuTunerCtx->m_s4FreqBoundLower;
        pAttri->ui4_upper_bound_freq = pEuTunerCtx->m_s4FreqBoundUpper;
        // freq offset tolerence range in Hz (for IFS = 4)
        switch (DVBT_GetBW(psDvbt2DemodCtx))
        {
            case 0:  pAttri->ui4_fine_tune_delta_freq = 536000; break; //6MHz
            case 1:  pAttri->ui4_fine_tune_delta_freq = 625000; break; //7MHz
            case 2:  pAttri->ui4_fine_tune_delta_freq = 714000; break; //8MHz
            case 3:  pAttri->ui4_fine_tune_delta_freq = 446000; break; //5MHz
            default: pAttri->ui4_fine_tune_delta_freq = 0;      break;
        }
        break;

    case TUNER_GET_TYPE_RAW_SIGNAL_LEVEL:
        // update m_SigLvScan
        temp = 0;

		DigTunerBypassI2C(TRUE);
        s2Temp = ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &temp);
		DigTunerBypassI2C(FALSE);
		
        if (s2Temp == ITUNER_OK)
        {
            mcSHOW_DBG_MSG(("TUNER_GET_TYPE_RAW_SIGNAL_LEVEL (SSI): %d-%d\n", temp, pEuTunerCtx->m_SigLvScan));
        }
        else
        {
            mcSHOW_DBG_MSG(("TUNER_GET_TYPE_RAW_SIGNAL_LEVEL (IFAGC): %d\n", pEuTunerCtx->m_SigLvScan));
        }
        *pzAnaAttributeLen = sizeof(S8);
        x_memcpy(pAnazAttribute, &(pEuTunerCtx->m_SigLvScan), *pzAnaAttributeLen);
        break;

    case TUNER_GET_TYPE_BER_TOLERANCE:
        *pzAnaAttributeLen = sizeof(INT32);
        x_memcpy(pAnazAttribute, &psDvbt2DemodCtx->i4BerTh, *pzAnaAttributeLen);
        break;

    case TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE:
        *pzAnaAttributeLen = sizeof(UINT8);
        x_memcpy(pAnazAttribute, &pEuTunerCtx->m_SigLvTh, *pzAnaAttributeLen);
        break;
    case TUNER_GET_TYPE_CELL_ID:
        *pzAnaAttributeLen = sizeof(UINT16);
        vDVBTGetInfo(psDvbt2DemodCtx);
        u2Temp = DVBT_GetCellId(psDvbt2DemodCtx);
        x_memcpy(pAnazAttribute, &u2Temp, *pzAnaAttributeLen);
        break;

    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T tsFmt;
            *pzAnaAttributeLen = sizeof(TS_FMT_T);
			#if 0 // LL test for db10-2
		if(mcIsDVBT2())
		{
            tsFmt = DVBT2_GetTsFmt(psDvbt2DemodCtx);
		}
		else
		{
			tsFmt = DVBT_GetTsFmt(psDvbt2DemodCtx);
		}
		#endif
            tsFmt = DVBT2_GetTsFmt(psDvbt2DemodCtx);
            x_memcpy(pAnazAttribute, &tsFmt, *pzAnaAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SQI:
		if(mcIsDVBT2())
		{
	        *pzAnaAttributeLen = sizeof(INT32);
	        temp = DVBT2_GetSQI3(psDvbt2DemodCtx);
			//#if DVBT2_FROM_PHILIPS
			//temp = DVBT2_GetSQI2(psDvbt2DemodCtx);
			//endif
	        x_memcpy(pAnazAttribute, &temp, *pzAnaAttributeLen);
		}
		else
		{
        	if (!fg1stMonitorRun)
        	{
            	vDVBTGetInfo(psDvbt2DemodCtx);
            	mcSHOW_DBG_MSG(("DVBT_TunerGetAttribute: TUNER_GET_TYPE_SQI not enough time to update\n"));
        	}
        	*pzAnaAttributeLen = sizeof(INT32);
        	temp = DVBT_GetSQI(psDvbt2DemodCtx);
        	x_memcpy(pAnazAttribute, &temp, *pzAnaAttributeLen);        
		}
        break;
    case TUNER_GET_TYPE_SSI:
        *pzAnaAttributeLen = sizeof(INT32);      
		#if 1
		if(mcIsDVBT2())
         	{
         		DigTunerBypassI2C(TRUE);
	            ITuner_OP( ITunerGetCtx(), itGetSSIIndex_T2,0, &temp);
			DigTunerBypassI2C(FALSE);
			mcSHOW_DBG_MSG(("DVBT2 SSI: %d\n",temp));
         	}
		else
		{
			DigTunerBypassI2C(TRUE);
	            ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &temp);
			DigTunerBypassI2C(FALSE);
			mcSHOW_DBG_MSG(("DVBT SSI: %d\n",temp));
		}
		#endif
		#if 0
		DigTunerBypassI2C(TRUE);
        ITuner_OP( ITunerGetCtx(), itGetSSIIndex,0, &temp);
		DigTunerBypassI2C(FALSE);		
        mcSHOW_DBG_MSG(("DVBT SSI: %d\n",temp));
		#endif
        x_memcpy(pAnazAttribute, &temp, *pzAnaAttributeLen);
        break;

	case TUNER_GET_TYPE_DVBT2_PLP:				
        *pzAnaAttributeLen = sizeof(TUNER_DVBT2_PLP_LIST_INFO);
		nNumDataPLPs = 0;
		u1LoopIdx    = 0;
        memset(&sPlpIdList, 0, sizeof(TUNER_DVBT2_PLP_LIST_INFO));
		mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]TUNER_GET_TYPE_DVBT2_PLP=%d\n",TUNER_GET_TYPE_DVBT2_PLP));
		// Check if T2 is MPLP
		if (psDvbt2DemodCtx->u1NumPLP > 1)
		{
			// Check all of 256 PLPID
			for (u1LoopCnt = 0; u1LoopCnt < 64; u1LoopCnt++)
			{
				// To get the data PLPID
				ucDataPLPAvaliable = vDVBT2GetPLPInfo(psDvbt2DemodCtx, a4PLPId, (U8)(u1LoopIdx * 4));

				u1PLPIdCheckIdx = 0;
				for (u1PLPIdCheckLoop = 0; u1PLPIdCheckLoop < 4; u1PLPIdCheckLoop++)
				{
					mcSHOW_DBG_MSG2(("[[TUNER_GET_TYPE_DVBT2_PLP]]ucDataPLPAvaliable = 0x%x\n", ucDataPLPAvaliable));
					
					// Check how many the data PLP is
					u1Temp = (ucDataPLPAvaliable >> u1PLPIdCheckIdx) & 0x1;
					if (u1Temp) 
					{
						aPLPId[nNumDataPLPs] = a4PLPId[u1PLPIdCheckIdx];
						mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]PLP(%u) : 0x%02x\n", nNumDataPLPs + 1, aPLPId[nNumDataPLPs]));
						nNumDataPLPs++; 
					}

					u1PLPIdCheckIdx++;
				}

				u1LoopIdx++;

				// If all of avaliable PLP is found, the parsing case could be terminated
				if (psDvbt2DemodCtx->u1NumPLP <= (u1LoopCnt * 4)) break;
			}
		}
		else
		{
			nNumDataPLPs = 1;
			aPLPId[0]    = psDvbt2DemodCtx->u1PLP1ID;
		}

		for(u1I=0; u1I < nNumDataPLPs; u1I++){
        	mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]PLP(%u) : 0x%02x\n", u1I+1, aPLPId[u1I]));
        }
		mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]PLP(%u) NumPLP : 0x%02x\n", u1I+1, psDvbt2DemodCtx->u1NumPLP));
		mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]PLP(%u) PLP ID: 0x%02x\n", u1I+1, aPLPId[0]));
		mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]PLP(%u) Data PLP num: 0x%02x\n", u1I+1, nNumDataPLPs));		
		
		// Clear PLP ID table
		for (u1LoopCnt = 0; u1LoopCnt < 255; u1LoopCnt++) sPlpIdList.ui1_PLPLIST[u1LoopCnt] = 0;
		
		sPlpIdList.ui1_current_count = psDvbt2DemodCtx->u1NumPLP;
		mcSHOW_DBG_MSG(("[TUNER_GET_TYPE_DVBT2_PLP]sPlpIdList.ui1_current_count = %d\n", sPlpIdList.ui1_current_count));
		x_memcpy(&(sPlpIdList.ui1_PLPLIST[0]), &aPLPId[0], nNumDataPLPs*sizeof(UINT8));  
        x_memcpy(pAnazAttribute, &sPlpIdList, *pzAnaAttributeLen);

		break;

    default:
        {
            mcSHOW_DBG_MSG4(("DVBT_TunerGetAttribute: RMR_DRV_INV_GET_INFO!, type=%d\n", e_get_type));
            //return (RMR_DRV_INV_GET_INFO);
            break;
        }
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerGetAttribute unlock pPdCtx->hHalLock\n")); 

    return (RMR_OK);
}


/***********************************************************************/
INT32 DVBT2_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_SET_TYPE_T  e_set_type,
                           const VOID *pvAnaAttribute,
                           SIZE_T zAnaAttributeLen)
{
    UCHAR ucData = *(UCHAR *)pvAnaAttribute;
    UCHAR *pucData = (UCHAR *)pvAnaAttribute;

    printf("===============\n DVBT2_TunerSetAttribute\n, e_set_type = %d\n", e_set_type);

    
    mcSHOW_DBG_MSG4(("DVBT_TunerSetAttribute pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_TunerSetAttribute pPdCtx->hHalLock\n"));

    switch (e_set_type)
    {
#ifdef fcTUNE_NOT_START
        case TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS:
            if (zAnaAttributeLen)
            {
            TUNER_ACQ_ACTION_T *peActionType = (TUNER_ACQ_ACTION_T *) pvAnaAttribute;

                mcSHOW_DBG_MSG(("%s: (%d, %X) eActionType=%d!\n", __FUNCTION__,
                                TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS,
                                TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS, *peActionType));
        printf("===============\n *peActionType_Old= %d\n", psDvbt2DemodCtx->u1TunerAcqType);                            
                psDvbt2DemodCtx->u1TunerAcqType = *peActionType;
        printf("===============\n *peActionType_New= %d\n", psDvbt2DemodCtx->u1TunerAcqType);                
            }
            break;
#endif

    case TUNER_SET_TYPE_TS_TRISTATE:
        if (ucData == BOOL_FALSE) {
            DVBT_SetTsPathMode(psDvbt2DemodCtx, TRUE);
        }
        else if (ucData == BOOL_TRUE) {
            DVBT_SetTsPathMode(psDvbt2DemodCtx, FALSE);
        }
        break;

    case TUNER_SET_TYPE_TS_SP:
        if (ucData == SP_PARALLEL) {
            DVBT_SetTsOutputMode(psDvbt2DemodCtx, TRUE);
        }
        else if (ucData == SP_SERIAL) {
            DVBT_SetTsOutputMode(psDvbt2DemodCtx, FALSE);
        }
        break;

    case TUNER_SET_TYPE_CI_CARD_TYPE:
        mcSHOW_DBG_MSG(("DVBT_TunerSetAttribute: TUNER_SET_TYPE_CI_CARD_TYPE\n"));
        if (pucData[0] == TUNER_CI_CARD_TYPE_1) {
            mcSHOW_DBG_MSG(("TUNER_SET_TYPE_CI_CARD_TYPE: TUNER_CI_CARD_TYPE_1, para = %d\n", pucData[1]));
            if (pucData[1] == TRUE) {
                vSetFwVarDVBT(psDvbt2DemodCtx, 0x6F, 0);
            }
            else if (pucData[1] == FALSE) {
                vSetFwVarDVBT(psDvbt2DemodCtx, 0x6F, 1);
            }
        }
        break;

    case TUNER_SET_TYPE_TS_OUTPUT:
        if (ucData == BOOL_TRUE) 
{
            DVBT2_SetTsOutput(psDvbt2DemodCtx, TRUE);
            DVBT_SetTsOutput(psDvbt2DemodCtx, TRUE);
        }
        else if (ucData == BOOL_FALSE) {
            DVBT2_SetTsOutput(psDvbt2DemodCtx, FALSE);
			DVBT_SetTsOutput(psDvbt2DemodCtx, FALSE);
        }
        break;

    default:
            mcSHOW_DBG_MSG(("DVBT_TunerSetAttribute: RMR_DRV_INV_SET_INFO!\n"));
            break;
    }

    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_TunerSetAttribute unlock pPdCtx->hHalLock\n")); 

    return (RMR_OK);
}

#if 0
/***********************************************************************/
void DVBT2_SetDmxExt(void)
{
    mcSHOW_DBG_MSG4(("DVBT_SetDmxExt pPdCtx->hHalLock\n")); 

    mcMUTEX_LOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape DVBT_SetDmxExt pPdCtx->hHalLock\n")); 
        
    mcMUTEX_UNLOCK(pPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("DVBT_SetDmxExt unlock pPdCtx->hHalLock\n")); 
}
#endif

/***********************************************************************/
void DVBT2_TunerBypassI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{   
    //UCHAR ucValue = 0;
    
    //mcMUTEX_LOCK(pPdCtx->hHalLock);   // no lock due to may be deadlock with TunerAcq

    DVBT2_TunerI2c(psDvbt2DemodCtx, bSwitchOn);
    
    //mcMUTEX_UNLOCK(pPdCtx->hHalLock);
}

/**********************************************************************/
void PD_DVBT2_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*    pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose = DVBT2_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq = DVBT2_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync = DVBT2_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal = DVBT2_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBT2_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBT2_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBT2_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBT2_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt = DVBT2_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer = DVBT2_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest = DVBT2_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBT2_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBT2_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C = DVBT2_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand = DVBT2_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand = DVBT2_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C = DVBT2_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc = DVBT2_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute = DVBT2_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute = DVBT2_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C = DVBT2_TunerBypassI2C;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd = DVBT2_TunerSetDiagCmd;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DVBT2_TunerGetDiagInfo;
#endif
}


