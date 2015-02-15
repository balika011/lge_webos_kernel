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
 * $RCSfile: pd_DVBS_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_DVBS_glue.c
 *  DVBS glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifndef SONYSAT_DUMMY_DEBUG
#include "u_tuner.h"
#include "x_os.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_dvbs_glue.h"
#include "tunerDebug.h"
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "pd_dvbs_cxd2854_if.h"
#include "x_gpio.h"
#include "vdr_PD_Def.h"
#include "x_rm.h"
#include "drvcust_if.h"
#include "x_gpio.h"
#else // SONYSAT_DUMMY_DEBUG
#include "MTKdef.h"
#include "pd_dvbs_glue.h"
#include "pd_dvbs_cxd2854_if.h"
#endif // SONYSAT_DUMMY_DEBUG
#ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
#include "eeprom_if.h"
#endif

#if defined(SONYSAT_TUNERMODULE_PE231)
// For PE231 tuner module
#define SONYSAT_TUNER_EXT_REF
#define SONYSAT_TUNER_LNA_ENABLE
#elif defined(SONYSAT_TUNERMODULE_ZE221)
// For ZE221 tuner module
#elif defined(SONYSAT_TUNERMODULE_PE221)
// For PE221 tuner module
#define SONYSAT_TUNER_EXT_REF
#elif defined(SONYSAT_TUNERMODULE_HELENE)
// For HELENE tuner module
#else
#error Please set tuner module type.
#endif

// for Sony CXD2854DVBSS2 driver
#include "sonysat_common.h"
#include "sonysat_i2c_MTK.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_integ_dvbs_s2.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_tuner_horus3a.h"
#include "sonysat_tuner_helene_MTK.h"
#include "sonysat_horus3a.h"
#include "sonysat_tuner_horus3.h"
#include "sonysat_horus3.h"
#include "sonysat_lnbc.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_integ_sat_device_ctrl.h"
#include "sonysat_math.h"
#include "mps_mp8126.h"
#include "st_lnbh29.h"
#include "allegro_a8304.h"
#include "allegro_a8303_1.h"
#include "cust_common_interface.h"

#define SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK

#define SCAN_RANGE_SINGLE_TIME     1 //scan range is 1M
#define Scan_Center_Freq_Step_100KHz        100
#define VENDOR_DRIVER_VER            "CXD2854 (DVB-S/S2) v0.13 (2014-07-31)"

#define SONYSAT_DVBS_NOT_RESET_DEMOD

#define SONYSAT_DEMOD_IIC_ADDRESS_0 0xD8
#define SONYSAT_DEMOD_IIC_ADDRESS_1 0xDA

#ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
#define addr_base_CANAL_DIGITAL     0x3162
#define addr_length_CANAL_DIGITAL   15
/*
#define addr_DS_PNC_OFF                 0x3162
#define addr_DS_UCP_PLLSWOFF            0x3163
#define addr_DS_K_FINE2_G1G2            0x3164
#define addr_DS_Q_FINE2_G1G2            0x3165
#define addr_DS_8_FINE2_G1G2            0x3166
#define addr_DS_G1_RANGE                0x3167
#define addr_DVB_DEF1                   0x3168
#define addr_DVB_DEF2                   0x3169
#define addr_DS_SC_ADDR                 0x316A
#define addr_DS_TSFORM                  0x316B
#define addr_DS_JOGEN                   0x316C
#define addr_SINGLE_CABLE               0x316D
#define addr_CANAL_CNR_THRES            0x316E
#define addr_CANAL_PHASE                0x316F
#define addr_POL_CAM_TSCKPER            0x3170
*/
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Export definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define mcSEMA_LOCK_MTFE(sema)                                       \
{                                                                    \
    mcDBG_MSG_L4((" [ _Lock_%s-%d ]\r\n", __FUNCTION__, __LINE__));  \
    x_sema_lock(sema, X_SEMA_OPTION_WAIT);                           \
    mcDBG_MSG_L4((" [ _Escape_%s-%d ]\r\n", __FUNCTION__, __LINE__));\
}
#define mcSEMA_UNLOCK_MTFE(sema)                                     \
{                                                                    \
    mcDBG_MSG_L4((" [ _Unlock_%s-%d ]\r\n", __FUNCTION__, __LINE__));\
    x_sema_unlock(sema);                                             \
}

typedef struct _sonysat_sqi_table_t{
    uint8_t sqi;   /* 0 - 100 */
    int32_t value;
} sonysat_sqi_table_t;

#define Debug_time

//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// static variables (Sony specific)
//-----------------------------------------------------------------------------
typedef struct _sonysat_demod_ctx_t // Sony CXD2854DVBSS2 Driver Specific
{
    HANDLE_T AutoScan_callback;
    UINT16 bs_stop_freq;    // In MHz
    UINT16 bs_start_freq;   // In MHz
    volatile UINT8 do_blindscan;
    volatile UINT8 Monitor_waiting_semaphore_flag;
    UINT32 ui4_data_forMW;

    UINT8 channel_locked_flag;  // default is true;
    DISEQC_BUS_CMD_T diseqc_cmd;
    UINT32 unicable_p;
    UINT8 set_flag;
    UINT8 ui1_mount_status;
    UINT8 PreUserBandIdx;
    UINT16 UnicableDelay;//in seconds

    DVBS_PD_CTX_T * pPdCtxDVBS;
    TUNER_DEVICE_CTX_T * pDVBSTunerDeviceCtx;

    // Instances to control CXD2854 driver
    sonysat_demod_t demod;
    sonysat_integ_t integ;
    sonysat_tuner_sat_t tuner;
    sonysat_lnbc_t lnbc;
    sonysat_i2c_t i2c;
    sonysat_i2c_t i2cTuner;
    sonysat_horus3a_t horus3a;
    sonysat_horus3_t horus3;
    sonysat_helene_mtk_t helene;
    mps_mp8126_t mp8126;

    // For saving current tuned signal information
    sonysat_dvbs_s2_tune_param_t tunedSignal;

    // For saving current blindscan progress information
    uint8_t blindscanProgress;

    // Data for unicable
    sonysat_integ_singlecable_data_t singlecableData;

    // For normal tune break
    x_break_fct breakFct; // Break function pointer to use in stopwatch function
    void* breakFctArg;    // Argument for break function
    UINT32 u4Uec;
    #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
    BOOL is_canal_digital_ts;
    #endif
} sonysat_demod_ctx_t;


/* Sat Tuner Control Instance */
#define SATTUNERCTX_NUM 2
static sonysat_demod_ctx_t g_sonysat_demod_ctx[SATTUNERCTX_NUM]; /* 0: Main, 1: Sub */

/* Sony Product Information */
#ifdef CC_PROJECT_FOR_SONY
static SONYPRODUCT_TYPEINFO g_sonyproduct_typeinfo;
#endif
/* BlindScan debug log flag */

#define SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM    (UINT32)0x01
#define SONYSAT_BLINDSCAN_LOG_CANDIDATE         (UINT32)0x02
static UINT32 g_sonysat_blindscan_log_flag = 0;

#ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
static uint8_t Value_non_CanalDigital[addr_length_CANAL_DIGITAL];   //non-Canal Digital value in eep
static UINT8 DVB_DEF2 = 0;
static BOOL b_already_read_eep = FALSE;
#endif

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static const char* sonysat_FormatResult(sonysat_result_t result);
static  INT32 DVBS_AutoScanCallback(UINT8 tunerID, UINT8 signal, UINT8 polar, UINT32 freq, UINT32 sym, UINT8 fec);
static sonysat_singlecable_bank_t get_bank(TUNER_UNICABLE_BANK_T e_bank);
static sonysat_singlecable_ub_slot_t get_userband(TUNER_USER_BAND_T e_userband);
static void DVBS_TunerConnect(UINT8 tunerID);

extern INT32 Tuner_Notify(DRV_COND_T e_nfy_cond, UINT32 u4Msg);
extern UINT8 u1DbgLevel; //add by ray.wang 20120614

#if defined(CC_PROJECT_FOR_SONY) || defined(SONYSAT_TUNERMODULE_HELENE)
extern INT16 HELENE_Satellite_SetRefOut(UINT8 tunerID, UINT8 setVal);
#endif

#ifndef CC_PROJECT_FOR_SONY
#define Unicable_collision
#ifdef Unicable_collision
EXTERN UINT32 PDWNC_ReadServoADCChannelValue (UINT32 u4Channel);
static BOOL BlindScan_MidWare_Break (UINT8 tunerID);
static void Unicable_ChgChnDelay (UINT8 tunerID, UINT8 UserBandIdx);
static UINT16 DVBS_GetUnicableDelay (UINT8 tunerID);

/*
 * Check Unicable state
 * Return 1:Free, 0:Busy
 */
UINT8 sonysat_DVBS_GetUnicableState (sonysat_integ_t * pInteg)
{
    UINT8 u1Ret=0;
    UINT8 uCount =0;
    UINT16 u2Delay = 0;
    UINT32 busy_Value=150;
    UINT8 tunerID = 0;
    sonysat_demod_ctx_t * pDemodCtx = NULL;
    DVBS_PD_CTX_T * pPdCtxDVBS = NULL;

    if (&g_sonysat_demod_ctx[1].integ == pInteg) {
        tunerID = 1;
    } else {
        tunerID = 0;
    }
    
    pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcDBG_MSG_L2(("CXD2854[%d] : %s\n", tunerID, __FUNCTION__));

    /* Repeat time */
    while (uCount < 5)
    {
        /* Busy flag */
        u1Ret = 0;
        if (pDemodCtx->do_blindscan && BlindScan_MidWare_Break(tunerID))
        {
            mcDBG_MSG_L2(("[BlindScan]Break 1!\n"));
            break;
        }
        Unicable_ChgChnDelay (tunerID, pPdCtxDVBS->t_UniCab_info.e_user_band);  // For P calculate
        mcDBG_MSG_L2(("PDWNC_ReadServoADCChannelValue=%d\n",PDWNC_ReadServoADCChannelValue(4)));
        if (PDWNC_ReadServoADCChannelValue(4) > busy_Value) //  check unicable state( busy or free)
        {
            mcDBG_MSG_L2(("[BlindScan]Delay1!\n"));
            u2Delay = DVBS_GetUnicableDelay (tunerID);    // Get P value
            SONYSAT_SLEEP(u2Delay);
            uCount++;
        } else {
            /* Free */
            u1Ret = 1;
            break;
        }
    }
    return (u1Ret);
}
#endif /* Unicable_collision */
#endif /* ifndef CC_PROJECT_FOR_SONY */

static TUNER_CONNECTION_MODE_T Util_GetConnMode(DVBS_PD_CTX_T * pdCtx,TUNER_CONNECTION_MODE_T e_mode)
{
    TUNER_CONNECTION_MODE_T e_ret=e_mode;
    switch(e_mode)
    {
        case CONN_MODE_SCAN:
        {
            if(pdCtx->fgUniCab)
            {
                e_ret=CONN_MODE_UNICABLE_SCAN;
            }
        }
        break;
        case CONN_MODE_SURF:
        case CONN_MODE_UNKNOWN:
        {
            if(pdCtx->fgUniCab)
            {
                e_ret=CONN_MODE_UNICABLE_SURF;
            }
        }
        break;
        default:
            mcDBG_MSG_L2(("[%s]Unknown conn mode %d\n",__FUNCTION__,e_mode));
        break;
    }
    return e_ret;
}

static void DVBS_OduChannelChg(UINT8 tunerID, TUNER_UNICABLE_CC_T *pUniCabinfo)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_diseqc_message_t message;
    sonysat_singlecable_ub_slot_t userband;
    sonysat_singlecable_bank_t bank;
    UINT32 centerFreqKHz = 0;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t *pInteg = &pDemodCtx->integ;

    mcDBG_MSG_L2(("CXD2854[%d] : %s\n", tunerID, __FUNCTION__));

    mcDBG_MSG_L2(("CXD2854[%d] : Ft=%dMHz,Fo=%dMHz,Fu=%dMHz,UB=%d,Bank=%d\n",\
    tunerID, pUniCabinfo->ui4_transponder_freq,pUniCabinfo->ui2_lnb_freq,pUniCabinfo->ui2_band_freq,pUniCabinfo->e_user_band,pUniCabinfo->e_bank));
    //S=TUNER_CODING_STEP_SIZE in u_tuner.h
    if(pUniCabinfo->ui4_transponder_freq==0)
    {
        mcDBG_MSG_L2(("CXD2854[%d] : Ft is 0MHz,do not do channel change\n", tunerID));
        return;
    }

    userband = get_userband(pUniCabinfo->e_user_band);
    bank = get_bank(pUniCabinfo->e_bank);

    if(pUniCabinfo->ui4_transponder_freq > pUniCabinfo->ui2_lnb_freq){
        centerFreqKHz = (UINT32)(pUniCabinfo->ui4_transponder_freq - pUniCabinfo->ui2_lnb_freq) * 1000;
    } else {
        centerFreqKHz = (UINT32)(pUniCabinfo->ui2_lnb_freq - pUniCabinfo->ui4_transponder_freq) * 1000;
    }
    //Create ODU_Channel_change command
    sonysat_result = sonysat_singlecable_command_ODU_Channel_change(&message,
        SONYSAT_SINGLECABLE_ADDRESS_ALL_DEVICES,
        userband,
        pUniCabinfo->ui2_band_freq * 1000,
        bank,
        centerFreqKHz);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_ERR(("sonysat_singlecable_command_ODU_Channel_change : %s\n", sonysat_FormatResult(sonysat_result)));
    }
#ifdef SONYSAT_LNBC_MPS_MP8126
    sonysat_result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, SONYSAT_LNBC_VOLTAGE_AUTO);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at pLnbc->SetVoltage(AUTO)\n"));
    }
    sonysat_result = sonysat_demod_sat_device_ctrl_TXENSetting (pInteg->pDemod, SONYSAT_TXEN_MODE_NORMAL, 13, 13);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at sonysat_demod_sat_device_ctrl_TXENSetting()\n"));
    }
#endif // SONYSAT_LNBC_MPS_MP8126
    //Send ODU_Channel_change command
    sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
    }
    return;
}

static void DVBS_OduPowerOff(UINT8 tunerID, DVBS_PD_CTX_T * pdCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_diseqc_message_t message;
    sonysat_singlecable_ub_slot_t userband;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t *pInteg = &pDemodCtx->integ;
    uint8_t tslock = 0;
    int retrycnt = 0;
    int checkcnt = 0;

#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    BOOL b_P10=0;
#endif

    mcDBG_MSG_L2(("CXD2854[%d] : %s UB=%d\n", tunerID, __FUNCTION__, pdCtx->t_UniCab_info.e_user_band));

    userband = get_userband(pdCtx->t_UniCab_info.e_user_band);

    //Create ODU_PowerOFF command
    sonysat_result = sonysat_singlecable_command_ODU_PowerOFF(&message,
        SONYSAT_SINGLECABLE_ADDRESS_ALL_DEVICES,
        userband);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_ERR(("sonysat_singlecable_command_ODU_PowerOFF : %s\n", sonysat_FormatResult(sonysat_result)));
    }

#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    {
        if(pDemodCtx->unicable_p == 0)
        {
            pDemodCtx->unicable_p = 0x3FF&(((userband+1)<<6)|((userband+1)<<1)); // P = p[9:0], P[4:1] = userband+1, P[9:6] = userband+1,
        }
        //calc P10
        if(((pDemodCtx->unicable_p & 0x80)>>7)^(pDemodCtx->unicable_p & 0x1))
        {
            b_P10 = 1;//P10 = 1
        }
        else
        {
            b_P10 = 0;
        }
        //shift p
        pDemodCtx->unicable_p >>= 1;
        pDemodCtx->unicable_p |= (b_P10<<9); //Add P10
        mcDBG_MSG_L2(("CXD2854[%d] : ODU power Off g_unicable_p = %d\n", tunerID, pDemodCtx->unicable_p));
    }

    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pInteg->pDemod, &tslock);
    if(sonysat_result == SONYSAT_RESULT_OK){
        if(tslock == 1){
            mcDBG_MSG_L2(("CXD2854[%d] : TS lock, goto check ODU power off status.\n", tunerID));//change 7--> 4
            for(retrycnt = 0; retrycnt < 4; retrycnt++){
#ifdef SONYSAT_LNBC_MPS_MP8126
                sonysat_result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, SONYSAT_LNBC_VOLTAGE_AUTO);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at pLnbc->SetVoltage(AUTO)\n"));
                }
#endif // SONYSAT_LNBC_MPS_MP8126
                //Send ODU_PowerOFF command
                sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
                }
                /* Check 40times with 10ms interval (= max 400ms) */
                if(retrycnt == 0)
                {
                    for(checkcnt = 0; checkcnt < 40; checkcnt++){
                        SONYSAT_SLEEP(10);
                        sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pInteg->pDemod, &tslock);
                        if(sonysat_result != SONYSAT_RESULT_OK){
                            mcDBG_MSG_ERR(("Error at sonysat_demod_dvbs_s2_monitor_SyncStat:%d (%s)\n", checkcnt, sonysat_FormatResult(sonysat_result)));
                            return;
                        }
                        if(tslock == 0){
                            break;
                        }
                    }
                }
                else
                {
                    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pInteg->pDemod, &tslock);
                    if(sonysat_result != SONYSAT_RESULT_OK){
                        mcDBG_MSG_ERR(("Error at sonysat_demod_dvbs_s2_monitor_SyncStat:%d (%s)\n", checkcnt, sonysat_FormatResult(sonysat_result)));
                        return;
                    }
                }

                if(tslock == 0){
                    break;
                }

                //For Traffic collision management Rules
                {
                    //calc P10
                    if(((pDemodCtx->unicable_p & 0x80)>>7)^(pDemodCtx->unicable_p & 0x1))
                    {
                        b_P10 = 1;//P10 = 1
                    }
                    else
                    {
                        b_P10 = 0;
                    }
                    if(retrycnt == 0)
                    {
                        pDemodCtx->unicable_p = 0x1FF&(((userband+1)<<5)|(userband+1)); // P = p[9:0], P[3:0] = userband+1, P[8:5] = userband+1,
                    }
                    else
                    {
                        //shift p
                        pDemodCtx->unicable_p >>= 1;
                    }
                    pDemodCtx->unicable_p |= (b_P10<<9); //Add P10
                    mcDBG_MSG_L2(("CXD2854[%d] : TS still lock.(%d).Delay(0x%x-%d)ms\n", tunerID, retrycnt, pDemodCtx->unicable_p, ((pDemodCtx->unicable_p) * 1000)/1024));
                    SONYSAT_SLEEP(((pDemodCtx->unicable_p) * 1000)/1024);
                }
            }
            if(tslock == 0){
                mcDBG_MSG_L2(("CXD2854[%d] : Stopped TS(%d - %d)\n", tunerID, retrycnt, checkcnt));
            } else {
                mcDBG_MSG_L2(("CXD2854[%d] : Give up!! TS can't be stopped.\n", tunerID));
            }
        } else {
            mcDBG_MSG_L2(("CXD2854[%d] : TS is NOT locked\n", tunerID));
            //Send ODU_PowerOFF command
            sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
            }
        }
    } else if((sonysat_result == SONYSAT_RESULT_ERROR_SW_STATE) && (pInteg->pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S)){
        mcDBG_MSG_L2(("CXD2854[%d] : NOT tuned\n", tunerID));
        //Send ODU_PowerOFF command
        sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
        }
    } else {
        /* Error */
        mcDBG_MSG_ERR(("Error at 1st sonysat_demod_dvbs_s2_monitor_SyncStat (%s)\n", sonysat_FormatResult(sonysat_result)));
        return;
    }
#else
#ifdef SONYSAT_LNBC_MPS_MP8126
    sonysat_result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, SONYSAT_LNBC_VOLTAGE_AUTO);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at pLnbc->SetVoltage(AUTO)\n"));
    }
#endif // SONYSAT_LNBC_MPS_MP8126
    //Send ODU_PowerOFF command
    sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
    }
#endif
}

static void DVBS_TunerDiscSCIF(UINT8 tunerID, DVBS_PD_CTX_T * pdCtx)
{
    //TODO_Unicable:Please add your implemention to ensure the specified user band of the SCIF is powered off.We need it to avoid the traffic conflict
    //Here is just a simple example without any confirmation
    DVBS_OduPowerOff(tunerID, pdCtx);
}

static BOOL DVBS_ValidateMwPara(UINT8 tunerID, TUNER_SAT_DIG_TUNE_INFO_T * pt_tuner_info,TUNER_CONNECTION_MODE_T e_conn_mode)
{
    UINT32 ui4_start_freq;
    UINT32 ui4_end_freq;
    UINT32 ui4_freq;
    BOOL fgRet = TRUE;
    DVBS_PD_CTX_T * pPdCtxDVBS = g_sonysat_demod_ctx[tunerID].pPdCtxDVBS;

    switch(e_conn_mode)
    {
        case CONN_MODE_SCAN:
            ui4_start_freq = pt_tuner_info->ui4_start_freq;
            ui4_end_freq = pt_tuner_info->ui4_end_freq;
            if((ui4_start_freq >= ui4_end_freq) || (ui4_start_freq < 940) || (ui4_end_freq > 2160))
            {
                mcDBG_MSG_ERR(("frequency start %d MHz end %dMHz,out of range [940,2160]MHz.\n",ui4_start_freq,ui4_end_freq));
                fgRet=FALSE;
            }
            break;

        case CONN_MODE_UNICABLE_SCAN:
            {
                TUNER_UNICABLE_CC_T *pUniCabinfo = &(pPdCtxDVBS->t_UniCab_info);
                ui4_start_freq = pt_tuner_info->ui4_start_freq;
                ui4_end_freq = pt_tuner_info->ui4_end_freq;
                if((ui4_start_freq >= ui4_end_freq) || (ui4_start_freq < 940) || (ui4_end_freq > 2160) || (pUniCabinfo->ui2_band_freq < 950) || (pUniCabinfo->ui2_band_freq > 2150))
                {
                    mcDBG_MSG_ERR(("frequency start %d MHz end %dMHz band freq %dMHz,out of range [940,2160]MHz.\n",ui4_start_freq,ui4_end_freq,pUniCabinfo->ui2_band_freq));
                    fgRet=FALSE;
                }
            }
            break;

        case CONN_MODE_UNICABLE_SURF:
        case CONN_MODE_UNKNOWN://channel change
        case CONN_MODE_SURF://preset scan ,single tp scan
            ui4_freq = pt_tuner_info->ui4_freq;
            if((ui4_freq < 940) || (ui4_freq > 2160))
            {
                mcDBG_MSG_ERR(("frequency %d MHz,out of range [940,2160]MHz.\n",ui4_freq));
                fgRet=FALSE;
            }
            break;

        default:
            mcDBG_MSG_ERR(("unknown connection mode %d.\n",e_conn_mode));
            fgRet=FALSE;
            break;
    }

    return fgRet;
}

#ifdef Unicable_collision
static void Unicable_ChgChnDelay(UINT8 tunerID, UINT8 UserBandIdx)
{
    UINT16 bNewMSB;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];

    if(UserBandIdx != pDemodCtx->PreUserBandIdx)
    {
        pDemodCtx->PreUserBandIdx = UserBandIdx;
        pDemodCtx->UnicableDelay = 0;
        UserBandIdx++;
        pDemodCtx->UnicableDelay |= UserBandIdx;
        pDemodCtx->UnicableDelay |= (UserBandIdx << 5);
    }
    else
    {
        //new p10 = p8 xor p1
        bNewMSB = ((pDemodCtx->UnicableDelay << 2) ^ (pDemodCtx->UnicableDelay << 9)) & 0x0200;
        pDemodCtx->UnicableDelay = (pDemodCtx->UnicableDelay >> 1);
        pDemodCtx->UnicableDelay |= bNewMSB;
    }
}
static UINT16 DVBS_GetUnicableDelay(UINT8 tunerID)/// return ms
{
    UINT16 u2Delay = 0;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];

    mcDBG_MSG_L4(("CXD2854[%d] : current unicable monitorprocess delay is %d ms\n", tunerID, ((pDemodCtx->UnicableDelay * 1000)/1024)));
    if(pDemodCtx->UnicableDelay)
    {
        u2Delay=((UINT32)(pDemodCtx->UnicableDelay * 1000))/1024;// P*1000 sec to ms
    }
    else
    {
        u2Delay = 500;//if delay is 0,set as 500ms for loop forever;
    }
    if(u2Delay < 500) u2Delay = 500;//d121202_Haibo:Limit the delay to 500ms to release CPU resource to other thread
    return u2Delay;
}
#endif

static void Demod_GPIO_Reset(UINT8 tunerID)
{
#ifndef SONYSAT_DVBS_NOT_RESET_DEMOD
    UINT32 u4ResetGpionum;

    //INT32 fgSet = 0;
    if(DRVCUST_OptQuery(eDemodResetGpio,&u4ResetGpionum)!=0)
    {
        u4ResetGpionum=201;//d111212_Haibo:default is gpio 201
    }
    GPIO_SetOut(u4ResetGpionum, 0);
    mcDELAY_MS(50);
    //fgSet = 1;
    GPIO_SetOut(u4ResetGpionum, 1);
    mcDELAY_MS(500);
    mcDBG_MSG_L2(("CXD2854[%d] : Reset demod via GPIO-%d\n", tunerID, u4ResetGpionum));
#else
    mcDBG_MSG_L2(("CXD2854[%d] : Do not need reset DVBS DEMOD", tunerID));
#endif
}

static BOOL BlindScan_MidWare_Break(UINT8 tunerID)
{
    DVBS_PD_CTX_T * pPdCtxDVBS = g_sonysat_demod_ctx[tunerID].pPdCtxDVBS;
    return pPdCtxDVBS->fgBreakMonitor;
}

//-----------------------------------------------------------------------------
// Sony specific
//-----------------------------------------------------------------------------
// This function is used in DtdCommand function
static UINT32 sonysat_strtoul(const char* str)
{
    const char* ptr = str;
    UINT32 value = 0;

    if(!str){
        return 0;
    }
    if((x_strlen(str) > 2) && (str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))){
        // Hex
        for(ptr = &(str[2]); *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value <<= 4; value += (UINT32)(*ptr - '0');
            }else if(('a' <= *ptr) && (*ptr <= 'f')){
                value <<= 4; value += (UINT32)(*ptr - 'a' + 10);
            }else if(('A' <= *ptr) && (*ptr <= 'F')){
                value <<= 4; value += (UINT32)(*ptr - 'A' + 10);
            }else{
                break;
            }
        }
    }else{
        // Decimal
        for(ptr = str; *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value *= 10; value += (UINT32)(*ptr - '0');
            }else{
                break;
            }
        }
    }

    return value;
}

static const char* sonysat_FormatResult(sonysat_result_t result)
{
    switch(result){
    case SONYSAT_RESULT_OK:
        return "OK";
    case SONYSAT_RESULT_ERROR_ARG:
        return "ERROR_ARG";
    case SONYSAT_RESULT_ERROR_I2C:
        return "ERROR_I2C";
    case SONYSAT_RESULT_ERROR_SW_STATE:
        return "ERROR_SW_STATE";
    case SONYSAT_RESULT_ERROR_HW_STATE:
        return "ERROR_HW_STATE";
    case SONYSAT_RESULT_ERROR_TIMEOUT:
        return "ERROR_TIMEOUT";
    case SONYSAT_RESULT_ERROR_UNLOCK:
        return "ERROR_UNLOCK";
    case SONYSAT_RESULT_ERROR_RANGE:
        return "ERROR_RANGE";
    case SONYSAT_RESULT_ERROR_NOSUPPORT:
        return "ERROR_NOSUPPORT";
    case SONYSAT_RESULT_ERROR_CANCEL:
        return "ERROR_CANCEL";
    case SONYSAT_RESULT_ERROR_OTHER:
        return "ERROR_OTHER";
    case SONYSAT_RESULT_ERROR_OVERFLOW:
        return "ERROR_OVERFLOW";
    case SONYSAT_RESULT_OK_CONFIRM:
        return "OK_CONFIRM";
    default:
        return "Unknown";
    }
}

static BOOL sonysat_GetTsFmt(sonysat_demod_t *pDemod, TS_FMT_T *pTsfmt)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    UINT8 data = 0;
    
    if((pDemod == NULL) || (pDemod->pI2c == NULL) ||(pDemod->pI2c->WriteOneRegister == NULL) ||(pDemod->pI2c->ReadRegister == NULL))
    {
        mcDBG_MSG_L4(("sonysat_GetTsFmt NULL pointer!!\n"));
        return 0;
    }

    // Set Bank 0
    sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
    if(sonysat_result != SONYSAT_RESULT_OK){ return 0; }

    // OSERIALEN (Bank:00h, Addr:C4h, Bit[7])
    sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return 0; }
    pTsfmt->SerOrPar = (data & 0x80) ? SP_SERIAL : SP_PARALLEL;

    // OSERCKMODE[1:0] (Bank:00h, Addr:C4h, Bit[1:0]
    pTsfmt->fgGatedClock = ((data & 0x03) == 0x03) ? BOOL_TRUE : BOOL_FALSE;

    // OWFMT_CKINV (Bank:00h, Addr:C5h, Bit[0])
    sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC5, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return 0; }
    pTsfmt->ClockPolarity = (data & 0x01) ? POL_HIGH_ACTIVE : POL_LOW_ACTIVE;

    // OTSDTDISABLE (Bank:00h, Addr:C3h, Bit[0])
    sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return 0; }
    pTsfmt->fgTristate = (data & 0x01) ? BOOL_FALSE : BOOL_TRUE ;

    pTsfmt->u4TsClockInKHz = 82000;

    pTsfmt->fgExternalDemod = BOOL_TRUE;

    return 1;
}

static UINT32 sonysat_GetBER(UINT8 tunerID)
{
    UINT32 ber = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    sonysat_dtv_system_t currentSystem;

    currentSystem = pDemod->system;
    if(currentSystem == SONYSAT_DTV_SYSTEM_ANY){
        sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &currentSystem);
        if(sonysat_result != SONYSAT_RESULT_OK){
            currentSystem = SONYSAT_DTV_SYSTEM_UNKNOWN;
        }
    }

    /* unit is 1e-5 */
    switch(currentSystem){
    case SONYSAT_DTV_SYSTEM_DVBS:
        sonysat_result = sonysat_demod_dvbs_monitor_PreRSBER(pDemod, (uint32_t*)&ber); /* 1e7 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L1(("CXD2854[%d] : OK S BER\n", tunerID));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("CXD2854[%d] : NG S BER\n", tunerID));
        }
        break;
    case SONYSAT_DTV_SYSTEM_DVBS2:
        sonysat_result = sonysat_demod_dvbs2_monitor_PreBCHBER(pDemod, (uint32_t*)&ber); /* 1e9 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 5000) / 10000;
            mcDBG_MSG_L1(("CXD2854[%d] : OK S2 BER\n", tunerID));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("CXD2854[%d] : NG S2 BER\n", tunerID));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L1(("CXD2854[%d] : NG def BER\n", tunerID));
        break;
    }

    return ber;
}

static UINT32 sonysat_GetPreBER(UINT8 tunerID)
{
    UINT32 ber = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    sonysat_dtv_system_t currentSystem;

    currentSystem = pDemod->system;
    if(currentSystem == SONYSAT_DTV_SYSTEM_ANY){
        sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &currentSystem);
        if(sonysat_result != SONYSAT_RESULT_OK){
            currentSystem = SONYSAT_DTV_SYSTEM_UNKNOWN;
        }
    }

    /* unit is 1e-5 */
    switch(currentSystem){
    case SONYSAT_DTV_SYSTEM_DVBS:
        sonysat_result = sonysat_demod_dvbs_monitor_PreViterbiBER(pDemod, (uint32_t*)&ber); /* 1e7 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L1(("CXD2854[%d] : OK S PreBER\n", tunerID));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("CXD2854[%d] : NG S PreBER\n", tunerID));
        }
        break;
    case SONYSAT_DTV_SYSTEM_DVBS2:
        sonysat_result = sonysat_demod_dvbs2_monitor_PreLDPCBER(pDemod, (uint32_t*)&ber); /* 1e7 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L1(("CXD2854[%d] : OK S2 PreBER\n", tunerID));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("CXD2854[%d] : NG S2 PreBER\n", tunerID));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L1(("CXD2854[%d] : NG def PreBER\n", tunerID));
        break;
    }

    return ber;
}

static UINT32 sonysat_GetPER(UINT8 tunerID)
{
    uint32_t per = 0;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;

    sonysat_result = sonysat_demod_dvbs_s2_monitor_PER (pDemod, &per);

    if(sonysat_result == SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("CXD2854[%d] : PER (1e6) : %d\n", tunerID, per));
    }else{
        mcDBG_MSG_L2(("CXD2854[%d] : PER (%s)\n", tunerID, sonysat_FormatResult(sonysat_result)));
    }

    return per;
}

#ifndef CC_PROJECT_FOR_SONY
static sonysat_result_t s_monitor_Quality (sonysat_demod_t * pDemod,
                                        uint8_t * pQuality)
{
    uint32_t ber = 0;
    int32_t cnr = 0;
    int32_t cnNordig = 0;
    int32_t cnRel = 0;
    int32_t berSQI = 0;
    sonysat_dvbs_coderate_t codeRate = SONYSAT_DVBS_CODERATE_1_2;
    int32_t tmp = 0;

    static const int32_t cnrNordigP1dB1000[] = {
        /* 1/2, 2/3, 3/4, 5/6, 7/8 */
        3800, 5600, 6700, 7700, 8400
    };

    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("s_monitor_Quality");

    /*
     * SQI (Signal Quality Indicator) given by:
     * SQI = (((C/Nrel - 3) / 10) + 1) * BER_SQI
     * BER_SQI = 20*log10(1/BER) - 40
     *
     * Re-arrange for ease of calculation:
     * BER_SQI = 20 * (log10(1) - log10(BER)) - 40
     *
     * If BER in units of 1e-7
     * BER_SQI = 20 * (log10(1) - (log10(BER) - log10(1e7)) - 40
     *
     * BER_SQI = log10(1e7) - 40 - log10(BER)
     * BER_SQI = -33 - 20*log10 (BER)
     */

    /* Get Pre-RS (Post-Viterbi) BER. */
    result = sonysat_demod_dvbs_monitor_PreRSBER (pDemod, &ber);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Get CNR value. */
    result = sonysat_demod_dvbs_s2_monitor_CNR (pDemod, &cnr);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Get Code rate */
    result = sonysat_demod_dvbs_monitor_CodeRate (pDemod, &codeRate);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if ((int32_t)codeRate > (sizeof(cnrNordigP1dB1000)/sizeof(cnrNordigP1dB1000[0]))){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
    } else {
        cnNordig = cnrNordigP1dB1000[(uint8_t)codeRate];
    }

    /* Calculate. */

    /* Handle 0%. */
    cnRel = cnr - cnNordig;

    if (cnRel < -7 * 1000) {
        *pQuality = 0;
        SONYSAT_TRACE_RETURN (result);
    }

    /* Calculate BER_SQI and clip. BER_SQI (1000x) */
    if (ber <= 1) {
        berSQI = 100 * 1000;
    }
    else {
        berSQI = (int32_t) (10 * sonysat_math_log10 (ber));
        berSQI = 20 * (7 * 1000 - (berSQI)) - 40 * 1000;
        if (berSQI < 20000) {
            berSQI = 0;
        }
    }

    /* Round up for rounding errors. */
    if (cnRel >= 3 * 1000) {
        *pQuality = (uint8_t) ((berSQI + 500) / 1000);
    }
    else {
        tmp = (((cnRel - (3 * 1000)) / 10) + 1000);
        *pQuality = (uint8_t) (((tmp * berSQI) + (5 * 1000 * 1000 / 10)) / (1000 * 1000)) & 0xFF;
    }

    if (*pQuality > 100) {
        *pQuality = 100;
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t s2_monitor_Quality (sonysat_demod_t * pDemod,
                                         uint8_t * pQuality)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_dvbs2_plscode_t plsCode;
    int32_t cnr = 0;
    int32_t cnrNordig = 0;
    int32_t cnrRel = 0;
    uint32_t ber = 0;
    uint32_t berSQI = 0;

    static const int32_t cnrNordigP1dB1000[2][11] = {
    /*   1/4,   1/3,    2/5,   1/2,    3/5,   2/3,   3/4,   4/5,   5/6,   8/9,  9/10 */
        {-1400,  -200,  700,  2000,   3200,  4100,  5000,  5700,  6200,  7200,  7400}, /* QPSK  */
        {    0,     0,    0,     0,   6500,  7600,  8900,     0, 10400, 11700, 12000}, /* 8PSK  */
    };

    SONYSAT_TRACE_ENTER ("s2_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Get Pre BCH BER. */
    result = sonysat_demod_dvbs2_monitor_PreBCHBER (pDemod, &ber);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

     /* Get SNR */
    result = sonysat_demod_dvbs_s2_monitor_CNR (pDemod, &cnr);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    result = sonysat_demod_dvbs2_monitor_PLSCode (pDemod, &plsCode);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* BER_SQI Calculated from:
     * if (Pre-BCH BER > 10^-4)            BER_SQI = 0
     * if (10^-4 <= Pre-BCH BER <= 10^-7)  BER_SQI = 100/15 = 6.667
     * if (Pre-BCH BER < 10^-7)            BER_SQI = 100/6  = 16.667
     *
     * Note : Pre-BCH BER is scaled by 10^9
     */
    if (ber > 100000) {
        berSQI = 0;
    }
    else if (ber >= 100) {
        berSQI = 6667;
    }
    else {
        berSQI = 16667;
    }

    cnrNordig = cnrNordigP1dB1000[(uint8_t)(plsCode.modulation)][(uint8_t)(plsCode.codeRate)];

    /* C/Nrel = C/Nrec - C/Nnordigp1 */
    cnrRel = cnr - cnrNordig;

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -3dB)         SQI = 0
     * if (-3dB <= CNrel <= 3dB)  SQI = (C/Nrel + 3) * BER_SQI
     * if (CNrel > 3dB)           SQI = 100
     */
    if (cnrRel < -3000) {
        *pQuality = 0;
    }
    else if (cnrRel <= 3000) {
        /* snrRel and berSQI scaled by 10^3 so divide by 10^6 */
        uint32_t tempSQI = (((cnrRel + 3000) * berSQI) + 500000) / 1000000;
        /* Clip value to 100% */
        *pQuality = (tempSQI > 100)? 100 : (uint8_t) tempSQI;
    }
    else {
        *pQuality = 100;
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t s_s2_monitor_Quality (sonysat_demod_t * pDemod,
                                           uint8_t * pQuality)
{
    sonysat_dtv_system_t dtvSystem = SONYSAT_DTV_SYSTEM_UNKNOWN;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("s_s2_monitor_Quality");

    if ((!pDemod) || (!pQuality)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(pDemod->system)
    {
    case SONYSAT_DTV_SYSTEM_ANY:
        result = sonysat_demod_dvbs_s2_monitor_System (pDemod, &dtvSystem);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if ((dtvSystem != SONYSAT_DTV_SYSTEM_DVBS) && (dtvSystem != SONYSAT_DTV_SYSTEM_DVBS2)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
        break;

    case SONYSAT_DTV_SYSTEM_DVBS:
    case SONYSAT_DTV_SYSTEM_DVBS2:
        dtvSystem = pDemod->system;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (dtvSystem == SONYSAT_DTV_SYSTEM_DVBS) {
        result = s_monitor_Quality (pDemod, pQuality);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN(result);
        }
    } else {
        result = s2_monitor_Quality (pDemod, pQuality);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN(result);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
#else /* CC_PROJECT_FOR_SONY */
static int32_t linearInterpolation (int32_t x, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    if (x0 == x1) {
        return y0;
    }
    if (y0 == y1) {
        return y0;
    }
    if (x == x0) {
        return y0;
    }
    if (x == x1) {
        return y1;
    }

    if (x0 < x1) {
        int32_t tmp = y0 * (x1 - x) + y1 * (x - x0);
        if (tmp > 0) {
            return (tmp + (x1 - x0) / 2) / (x1 - x0);
        } else {
            return (tmp - (x1 - x0) / 2) / (x1 - x0);
        }
    } else {
        int32_t tmp = y0 * (x - x1) + y1 * (x0 - x);
        if (tmp > 0) {
            return (tmp + (x0 - x1) / 2) / (x0 - x1);
        } else {
            return (tmp - (x0 - x1) / 2) / (x0 - x1);
        }
    }
}

static uint8_t calculate_SQI2 (const sonysat_sqi_table_t * pTable, int32_t length, int32_t value)
{
    uint8_t isAscend = 0;
    int32_t index = 0;

    if ((!pTable) || (length < 2)) {
        return 0;
    }

    /* Check the order of the data */
    if (pTable->value < (pTable + 1)->value) {
        isAscend = 1;
    } else {
        isAscend = 0;
    }

    if (isAscend) {
        if (value < pTable->value) {
            return pTable->sqi;
        }
    } else {
        if (value > pTable->value) {
            return pTable->sqi;
        }
    }
    for (index = 1; index < length; index++) {
        if (((isAscend) && (value < (pTable + index)->value)) ||
            ((!isAscend) && (value > (pTable + index)->value))) {
            return (uint8_t)linearInterpolation (value, (pTable + index)->value, (pTable + index)->sqi, (pTable + index - 1)->value, (pTable + index - 1)->sqi);
        }
    }
    /* return the last SQI in table */
    return (pTable + length - 1)->sqi;
}

static sonysat_result_t s_monitor_Quality2 (sonysat_demod_t * pDemod,
                                        uint8_t * pQuality)
{
    uint32_t ber = 0;

    sonysat_result_t result = SONYSAT_RESULT_OK;
    static const sonysat_sqi_table_t sqi_dvbs[] = {
        {   0, 70001},
        {   9, 60000},
        {  19, 50000},
        {  29, 40000},
        {  39, 36000},
        {  49, 10000},
        {  59,  6000},
        {  69,  4000},
        {  79,  2000},
        {  89,   800},
        {  99,   300},
        { 100,   299}
    };

    SONYSAT_TRACE_ENTER ("s_monitor_Quality2");

    if ((!pDemod) || (!pQuality)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Get Pre-RS (Post-Viterbi) BER. */
    result = sonysat_demod_dvbs_monitor_PreRSBER (pDemod, &ber);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* SQI Calculation */
    *pQuality = calculate_SQI2 (sqi_dvbs, sizeof(sqi_dvbs)/sizeof(sqi_dvbs[0]), (int32_t)ber);

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t s2_monitor_Quality2 (sonysat_demod_t * pDemod,
                                         uint8_t * pQuality)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_dvbs2_plscode_t plsCode;
    int32_t cnr = 0;
    const sonysat_sqi_table_t * pSqiTable;
    int32_t sqiTableLength = 0;

    static const sonysat_sqi_table_t sqi_qpsk_1_2[] = {
        { 10,  1100},
        { 20,  1101},
        { 30,  2201},
        { 40,  3301},
        { 50,  3801},
        { 60,  4301},
        { 70,  4801},
        { 80,  5301},
        { 90,  6301},
        {100,  7301}
    };

    static const sonysat_sqi_table_t sqi_qpsk_3_5[] = {
        { 10,  2500},
        { 20,  2501},
        { 30,  3501},
        { 40,  4501},
        { 50,  5001},
        { 60,  5501},
        { 70,  6001},
        { 80,  6501},
        { 90,  7501},
        {100,  8501}
    };

    static const sonysat_sqi_table_t sqi_qpsk_2_3[]  = {
        { 10,  3400},
        { 20,  3401},
        { 30,  4401},
        { 40,  5401},
        { 50,  5901},
        { 60,  6401},
        { 70,  6901},
        { 80,  7401},
        { 90,  8401},
        {100,  9401}
    };

    static const sonysat_sqi_table_t sqi_qpsk_3_4[]  = {
        { 10,  4400},
        { 20,  4401},
        { 30,  5401},
        { 40,  6401},
        { 50,  6901},
        { 60,  7401},
        { 70,  7901},
        { 80,  8401},
        { 90,  9401},
        {100, 10401}
    };

    static const sonysat_sqi_table_t sqi_qpsk_4_5[]  = {
        { 10,  5000},
        { 20,  5001},
        { 30,  6001},
        { 40,  7001},
        { 50,  7501},
        { 60,  8001},
        { 70,  8501},
        { 80,  9001},
        { 90, 10001},
        {100, 11001}
    };

    static const sonysat_sqi_table_t sqi_qpsk_5_6[]  = {
        { 10,  5500},
        { 20,  5501},
        { 30,  6501},
        { 40,  7501},
        { 50,  8001},
        { 60,  8501},
        { 70,  9001},
        { 80,  9501},
        { 90, 10501},
        {100, 11501}
    };

    static const sonysat_sqi_table_t sqi_qpsk_8_9[]  = {
        { 10,  6600},
        { 20,  6601},
        { 30,  7601},
        { 40,  8601},
        { 50,  9101},
        { 60,  9601},
        { 70, 10101},
        { 80, 10601},
        { 90, 11601},
        {100, 12601}
    };

    static const sonysat_sqi_table_t sqi_qpsk_9_10[] = {
        { 10,  6900},
        { 20,  6901},
        { 30,  7901},
        { 40,  8901},
        { 50,  9401},
        { 60,  9901},
        { 70, 10401},
        { 80, 10901},
        { 90, 11901},
        {100, 12901}
    };

    static const sonysat_sqi_table_t sqi_8psk_3_5[]  = {
        { 10,  5900},
        { 20,  5901},
        { 30,  6601},
        { 40,  8001},
        { 50,  8501},
        { 60,  9001},
        { 70, 10001},
        { 80, 11501},
        { 90, 12501},
        {100, 13501}
    };

    static const sonysat_sqi_table_t sqi_8psk_2_3[]  = {
        { 10,  7300},
        { 20,  7301},
        { 30,  8001},
        { 40,  8901},
        { 50, 10001},
        { 60, 11001},
        { 70, 11501},
        { 80, 12001},
        { 90, 13001},
        {100, 14001}
    };

    static const sonysat_sqi_table_t sqi_8psk_3_4[]  = {
        { 10,  8700},
        { 20,  8701},
        { 30,  9501},
        { 40, 10801},
        { 50, 11501},
        { 60, 12001},
        { 70, 12501},
        { 80, 13001},
        { 90, 14001},
        {100, 15001}
    };

    static const sonysat_sqi_table_t sqi_8psk_5_6[]  = {
        { 10, 10500},
        { 20, 10501},
        { 30, 11001},
        { 40, 12001},
        { 50, 12501},
        { 60, 13001},
        { 70, 13501},
        { 80, 14001},
        { 90, 15001},
        {100, 16001}
    };

    static const sonysat_sqi_table_t sqi_8psk_8_9[]  = {
        { 10, 11400},
        { 20, 11401},
        { 30, 12101},
        { 40, 12801},
        { 50, 13501},
        { 60, 14001},
        { 70, 14501},
        { 80, 15001},
        { 90, 16001},
        {100, 17001}
    };

    static const sonysat_sqi_table_t sqi_8psk_9_10[] = {
        { 10, 11700},
        { 20, 11701},
        { 30, 12001},
        { 40, 12801},
        { 50, 13501},
        { 60, 14001},
        { 70, 14501},
        { 80, 15001},
        { 90, 16001},
        {100, 17001}
    };

    SONYSAT_TRACE_ENTER ("s2_monitor_Quality2");

    if ((!pDemod) || (!pQuality)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

     /* Get SNR */
    result = sonysat_demod_dvbs_s2_monitor_CNR (pDemod, &cnr);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    result = sonysat_demod_dvbs2_monitor_PLSCode (pDemod, &plsCode);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* SQI Calculation */
    if (plsCode.modulation == SONYSAT_DVBS2_MODULATION_QPSK) {
        switch (plsCode.codeRate) {
        case SONYSAT_DVBS2_CODERATE_1_2:
            pSqiTable = sqi_qpsk_1_2;
            sqiTableLength = sizeof(sqi_qpsk_1_2)/sizeof(sqi_qpsk_1_2[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_3_5:
            pSqiTable = sqi_qpsk_3_5;
            sqiTableLength = sizeof(sqi_qpsk_3_5)/sizeof(sqi_qpsk_3_5[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_2_3:
            pSqiTable = sqi_qpsk_2_3;
            sqiTableLength = sizeof(sqi_qpsk_2_3)/sizeof(sqi_qpsk_2_3[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_3_4:
            pSqiTable = sqi_qpsk_3_4;
            sqiTableLength = sizeof(sqi_qpsk_3_4)/sizeof(sqi_qpsk_3_4[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_4_5:
            pSqiTable = sqi_qpsk_4_5;
            sqiTableLength = sizeof(sqi_qpsk_4_5)/sizeof(sqi_qpsk_4_5[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_5_6:
            pSqiTable = sqi_qpsk_5_6;
            sqiTableLength = sizeof(sqi_qpsk_5_6)/sizeof(sqi_qpsk_5_6[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_8_9:
            pSqiTable = sqi_qpsk_8_9;
            sqiTableLength = sizeof(sqi_qpsk_8_9)/sizeof(sqi_qpsk_8_9[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_9_10:
            pSqiTable = sqi_qpsk_9_10;
            sqiTableLength = sizeof(sqi_qpsk_9_10)/sizeof(sqi_qpsk_9_10[0]);
            break;
        default:
            *pQuality = 0;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
    } else if (plsCode.modulation == SONYSAT_DVBS2_MODULATION_8PSK) {
        switch (plsCode.codeRate) {
        case SONYSAT_DVBS2_CODERATE_3_5:
            pSqiTable = sqi_8psk_3_5;
            sqiTableLength = sizeof(sqi_8psk_3_5)/sizeof(sqi_8psk_3_5[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_2_3:
            pSqiTable = sqi_8psk_2_3;
            sqiTableLength = sizeof(sqi_8psk_2_3)/sizeof(sqi_8psk_2_3[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_3_4:
            pSqiTable = sqi_8psk_3_4;
            sqiTableLength = sizeof(sqi_8psk_3_4)/sizeof(sqi_8psk_3_4[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_5_6:
            pSqiTable = sqi_8psk_5_6;
            sqiTableLength = sizeof(sqi_8psk_5_6)/sizeof(sqi_8psk_5_6[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_8_9:
            pSqiTable = sqi_8psk_8_9;
            sqiTableLength = sizeof(sqi_8psk_8_9)/sizeof(sqi_8psk_8_9[0]);
            break;
        case SONYSAT_DVBS2_CODERATE_9_10:
            pSqiTable = sqi_8psk_9_10;
            sqiTableLength = sizeof(sqi_8psk_9_10)/sizeof(sqi_8psk_9_10[0]);
            break;
        default:
            *pQuality = 0;
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
    } else {
        *pQuality = 0;
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
    }

    *pQuality = calculate_SQI2 (pSqiTable, sqiTableLength, cnr);

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t s_s2_monitor_Quality (sonysat_demod_t * pDemod,
                                            uint8_t * pQuality)
{
    sonysat_dtv_system_t dtvSystem = SONYSAT_DTV_SYSTEM_UNKNOWN;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("s_s2_monitor_Quality");

    if ((!pDemod) || (!pQuality)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(pDemod->system)
    {
    case SONYSAT_DTV_SYSTEM_ANY:
        result = sonysat_demod_dvbs_s2_monitor_System (pDemod, &dtvSystem);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if ((dtvSystem != SONYSAT_DTV_SYSTEM_DVBS) && (dtvSystem != SONYSAT_DTV_SYSTEM_DVBS2)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
        break;

    case SONYSAT_DTV_SYSTEM_DVBS:
    case SONYSAT_DTV_SYSTEM_DVBS2:
        dtvSystem = pDemod->system;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (dtvSystem == SONYSAT_DTV_SYSTEM_DVBS) {
        result = s_monitor_Quality2 (pDemod, pQuality);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN(result);
        }
    } else {
        result = s2_monitor_Quality2 (pDemod, pQuality);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN(result);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
#endif /* (NOT) CC_PROJECT_FOR_SONY */

#if 0
static UINT16 sonysat_GetUEC(UINT8 tunerID)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    UINT32 pen = 0;
    sonysat_dtv_system_t dtvSystem;
    uint32_t symbolRateSps;
    uint32_t symbolRateKSps;
    uint32_t bitRateKbps;
    uint32_t per;

    sonysat_result = sonysat_demod_dvbs_s2_monitor_System (pDemod, &dtvSystem);
    if (sonysat_result != SONYSAT_RESULT_OK) {
        /* Error */
        mcDBG_MSG_L2(("Error at monitor_System\n"));
        return 0;
    }
    sonysat_result = sonysat_demod_dvbs_s2_monitor_SymbolRate (pDemod, &symbolRateSps);
    if (sonysat_result != SONYSAT_RESULT_OK) {
        /* Error */
        mcDBG_MSG_L2(("Error at monitor_SymbolRate\n"));
        return 0;
    }
    symbolRateKSps = (symbolRateSps + 500) / 1000;
    sonysat_result = sonysat_demod_dvbs_s2_monitor_PER (pDemod, &per);
    if (sonysat_result != SONYSAT_RESULT_OK) {
        /* Error */
        mcDBG_MSG_L2(("Error at monitor_PER\n"));
        return 0;
    }
    if (per == 0) {
        /* No packet error */
        pen = 0;
    } else {
        if (dtvSystem == SONYSAT_DTV_SYSTEM_DVBS) {
            sonysat_dvbs_coderate_t codeRate;
            uint32_t numerator;
            uint32_t denominator;
            sonysat_result = sonysat_demod_dvbs_monitor_CodeRate (pDemod, &codeRate);
            if (sonysat_result != SONYSAT_RESULT_OK) {
                /* Error */
                mcDBG_MSG_L2(("Error at monitor_CodeRate\n"));
                return 0;
            }
            switch (codeRate) {
            case SONYSAT_DVBS_CODERATE_1_2:
                numerator = 1;
                denominator = 2;
                break;
            case SONYSAT_DVBS_CODERATE_2_3:
                numerator = 2;
                denominator = 3;
                break;
            case SONYSAT_DVBS_CODERATE_3_4:
                numerator = 3;
                denominator = 4;
                break;
            case SONYSAT_DVBS_CODERATE_5_6:
                numerator = 5;
                denominator = 6;
                break;
            case SONYSAT_DVBS_CODERATE_7_8:
                numerator = 7;
                denominator = 8;
                break;
            default:
                /* Error */
                mcDBG_MSG_L2(("CodeRate is invalid\n"));
                return 0;
            }
            if (symbolRateKSps >= 60000) {
                /* Error */
                mcDBG_MSG_L2(("SymbolRate is invalid\n"));
                return 0;
            } else {
                /*
                 * Bit rate = SR[MSps] * 2 * CodeRate * 188 / 204
                 *          = SR[MSps] *     CodeRate * 188 / 102
                 *          = SR[MSps] *     CodeRate *  94 /  51
                 */
                bitRateKbps = ((symbolRateKSps * numerator * 94) + (51 * denominator / 2)) / (51 * denominator);
            }
        } else if (dtvSystem == SONYSAT_DTV_SYSTEM_DVBS2) {
            sonysat_dvbs2_plscode_t plsCode;
            uint32_t kbch;
            sonysat_result = sonysat_demod_dvbs2_monitor_PLSCode (pDemod, &plsCode);
            if (sonysat_result != SONYSAT_RESULT_OK) {
                /* Error */
                mcDBG_MSG_L2(("Error at monitor_PLSCode\n"));
                return 0;
            }
            switch (plsCode.codeRate) {
            case SONYSAT_DVBS2_CODERATE_1_2:
                kbch = 32208;
                break;
            case SONYSAT_DVBS2_CODERATE_3_5:
                kbch = 38688;
                break;
            case SONYSAT_DVBS2_CODERATE_2_3:
                kbch = 43040;
                break;
            case SONYSAT_DVBS2_CODERATE_3_4:
                kbch = 48408;
                break;
            case SONYSAT_DVBS2_CODERATE_4_5:
                kbch = 51648;
                break;
            case SONYSAT_DVBS2_CODERATE_5_6:
                kbch = 53840;
                break;
            case SONYSAT_DVBS2_CODERATE_8_9:
                kbch = 57472;
                break;
            case SONYSAT_DVBS2_CODERATE_9_10:
                kbch = 58192;
                break;
            default:
                /* Error */
                mcDBG_MSG_L2(("CodeRate is invalid\n"));
                return 0;
            }
            if (plsCode.modulation == SONYSAT_DVBS2_MODULATION_8PSK) {
                if (plsCode.isPilotOn) {
                    bitRateKbps = ((symbolRateKSps * (kbch - 80)) + 11097) / 22194;
                } else {
                    bitRateKbps = ((symbolRateKSps * (kbch - 80)) + 10845) / 21690;
                }
            } else if (plsCode.modulation == SONYSAT_DVBS2_MODULATION_QPSK) {
                if (plsCode.isPilotOn) {
                    bitRateKbps = ((symbolRateKSps * (kbch - 80)) + 16641) / 33282;
                } else {
                    bitRateKbps = ((symbolRateKSps * (kbch - 80)) + 16245) / 32490;
                }
            } else {
                /* Error */
                mcDBG_MSG_L2(("Modulation is invalid\n"));
                return 0;
            }
        } else {
            /* Error */
            mcDBG_MSG_L2(("System is invalid\n"));
            return 0;
        }
        /*
         * PacketErrorNumber = (bitRate) / (packetSize) * (PER)
         *                   = (bitRateKbps * 1000) / (188 * 8) * (per / 1000000)
         *                   =  bitRateKbps / (188 * 8) * per / 1000
         *                   = (bitRateKbps * per) / (188 * 8 * 1000)
         *                   = (bitRateKbps * per) / (1504000)
         *                   = (bitRateKbps * per) / (5^3 * 2^8 * 47))
         *
         * 920 <= bitRateKbps <= 96000
         *   0 <=     per     <= 1000000
         */
        {
            uint32_t div = 1504000;
            while (per > ((0xFFFFFFFF - (div - 1)) / bitRateKbps)) {
                per = (per + 2) / 5;
                div = (div + 2) / 5;
            }
            pen = ((bitRateKbps * per) + (div - 1)) / div;
        }
    }
    /* Clip to 9999 */
    if (pen > 9999) {
        pen = 9999;
    }
    return (UINT16)pen;
}
#endif

static UINT8 sonysat_GetSSI(UINT8 tunerID)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_demod_t * pDemod = &pDemodCtx->demod;
    sonysat_tuner_sat_t * pTuner = &pDemodCtx->tuner;
    uint32_t ifagcOut = 0;
    int32_t agcLevel = 0;
    int32_t i4Temp=0;
    INT32 RSSI = 0;
    UINT8 tsLock = 0;

    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
    if(sonysat_result == SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("CXD2854[%d] : TS Lock : %d\n", tunerID, tsLock));
    }else{
        tsLock = 0;
    }

    if(tsLock == 0)
    {
        return 0;
    }

    //signal level.update it as i4Temp
    sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifagcOut);
    if(sonysat_result == SONYSAT_RESULT_OK){
        sonysat_result = pTuner->AGCLevel2AGCdB (pTuner, ifagcOut, &agcLevel);
        /* agcLevel value is dB x 100. */
        //RSSI = (INT32)(((agcLevel + 50) / 100) * (-1));
        RSSI = (INT32)(agcLevel  * (-1));
    }else{
        RSSI = 0;
    }
    mcDBG_MSG_L1(("CXD2854[%d] : RSSI is %d\n", tunerID, RSSI));
    if(RSSI<=-8000){
        i4Temp=0;
    }
    else if(RSSI <= -7900){
        i4Temp=(RSSI + 8000)/10;
    }
    else if(RSSI <= -7800){
        i4Temp=10 + (RSSI + 7900)/10;
    }
    else if((RSSI>-7800)&&(RSSI<=-7300)){
        i4Temp=20+(RSSI+7800)/50;
    }
    else if((RSSI>=-7300)&&(RSSI<-6900)){
        i4Temp=30+(RSSI+7300)/40;
    }
    else if((RSSI>=-6900)&&(RSSI<-6400)){
        i4Temp=40+(RSSI+6900)/50;
    }
    else if((RSSI>=-6400)&&(RSSI<-6000)){
        i4Temp=50+(RSSI+6400)/40;
    }
    else if((RSSI>=-6000)&&(RSSI<-5500)){
        i4Temp=60+(RSSI+6000)/50;
    }
    else if((RSSI>=-5500)&&(RSSI<-5100)){
        i4Temp=70+(RSSI+5500)/40;
    }
    else if((RSSI>=-5100)&&(RSSI<-4500)){
        i4Temp=80+(RSSI+5100)/40;
    }
    else if((RSSI>=-4500)&&(RSSI<-4000)){
        i4Temp=90+(RSSI+4500)/50;
    }
    else{
        i4Temp=100;
    }
    mcDBG_MSG_L1(("CXD2854[%d] : SSI is %d\n", tunerID, i4Temp));
    return (uint8_t)i4Temp;
}

static void DVBS_TunerDemodClock(UINT8 tunerID, UINT8 demod_clock)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    if(demod_clock==0)
    {
        sonysat_demod_Shutdown(pDemod);
    }
    else
    {
        sonysat_demod_SleepS(pDemod);
    }
}

static sonysat_singlecable_bank_t get_bank(TUNER_UNICABLE_BANK_T e_bank)
{
    sonysat_singlecable_bank_t sonysat_bank;
    switch(e_bank)
    {
    case BANK_POS_A_L_BAND_VERT_POL:
    default:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_0;
        break;
    case BANK_POS_A_H_BAND_VERT_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_1;
        break;
    case BANK_POS_A_L_BAND_HOR_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_2;
        break;
    case BANK_POS_A_H_BAND_HOR_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_3;
        break;
    case BANK_POS_B_L_BAND_VERT_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_4;
        break;
    case BANK_POS_B_H_BAND_VERT_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_5;
        break;
    case BANK_POS_B_L_BAND_HOR_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_6;
        break;
    case BANK_POS_B_H_BAND_HOR_POL:
        sonysat_bank = SONYSAT_SINGLECABLE_BANK_7;
        break;
    }
    return sonysat_bank;
}

static sonysat_singlecable_ub_slot_t get_userband(TUNER_USER_BAND_T e_userband)
{
    sonysat_singlecable_ub_slot_t sonysat_userband;
    switch(e_userband)
    {
    case USER_BAND_1:
    default:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_1;
        break;
    case USER_BAND_2:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_2;
        break;
    case USER_BAND_3:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_3;
        break;
    case USER_BAND_4:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_4;
        break;
    case USER_BAND_5:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_5;
        break;
    case USER_BAND_6:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_6;
        break;
    case USER_BAND_7:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_7;
        break;
    case USER_BAND_8:
        sonysat_userband = SONYSAT_SINGLECABLE_UB_8;
        break;
    }
    return sonysat_userband;
}

static sonysat_result_t transmitDummyUnicableCommand(sonysat_diseqc_message_t * pMessage)
{
    // do nothing
    return SONYSAT_RESULT_OK;
}
#ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
void sonysat_set_eeprom_non_canaldigital(UINT8 tunerID, UINT8 *pValue)
{
    UINT64 u4Ofst[addr_length_CANAL_DIGITAL] ={0};
    UINT8 i = 0;
    
    mcDBG_MSG_L2(("sonysat_set_eeprom_non_canaldigital[%d]\n", tunerID));    
    for(i=0; i<addr_length_CANAL_DIGITAL; i++){
        if(i==0){
            u4Ofst[0] = addr_base_CANAL_DIGITAL;            
            VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[0], (UINT32)&pValue[0], 1));
        }else{
            u4Ofst[i] =u4Ofst[i-1] + 1; 
            VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[i], (UINT32)&pValue[i], 1));
        }
    }
}

void sonysat_get_eeprom_non_canaldigital(UINT8 tunerID, UINT8 *pValue)
{
    UINT64 u4Ofst[addr_length_CANAL_DIGITAL] ={0};
    UINT8 i = 0;

    for(i=0; i<addr_length_CANAL_DIGITAL; i++){
        if(i==0){
            u4Ofst[0] = addr_base_CANAL_DIGITAL;            
            VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[0], (UINT32)(void *)&pValue[0], 1));
        }else{
            u4Ofst[i] =u4Ofst[i-1] + 1; 
            VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[i], (UINT32)(void *)&pValue[i], 1));
        }
        mcDBG_MSG_L2(("sonysat_get_eeprom_non_canaldigital pValue[%d] = 0x%x\n", i, pValue[i]));
    }
    mcDBG_MSG_L2(("sonysat_get_eeprom_non_canaldigital[%d]\n", tunerID));
}

void sonysat_set_flag_non_canaldigital(UINT8 tunerID,UINT8 u1_flag)
{
    UINT64 u4Ofst =addr_base_CANAL_DIGITAL + addr_length_CANAL_DIGITAL;
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst, (UINT32) &u1_flag, 1));
    mcDBG_MSG_L2(("sonysat_set_flag_non_canaldigital[%d] u1_flag = 0x%x\n", tunerID,u1_flag));
}

void sonysat_get_flag_non_canaldigital(UINT8 tunerID, UINT8 u1_flag)
{
    UINT64 u4Ofst =addr_base_CANAL_DIGITAL + addr_length_CANAL_DIGITAL;
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&u1_flag, 1));
    mcDBG_MSG_L2(("sonysat_get_flag_non_canaldigital[%d] u1_flag = 0x%x\n", tunerID, u1_flag));
}

static sonysat_result_t sonysat_set_demod_non_canaldigital(UINT8 tunerID, UINT8 *pParameter)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    UINT8 u1_Temp = pParameter[5];
    
    SONYSAT_TRACE_ENTER("sonysat_set_demod_non_canaldigital");
    mcDBG_MSG_L2(("b_is_canal_digital = %d\n DVB_DEF2 = %d \n", pDemodCtx->is_canal_digital_ts, DVB_DEF2));
    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, (((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) ? 0x01 : pParameter[0])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA8 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA8) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB4, (((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) ? 0x01 : pParameter[1])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA6 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA6) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6C, (((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) ? 0x7C : pParameter[2])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, (((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) ? 0x7C : pParameter[3])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, (((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) ? 0x7C : pParameter[4])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA6 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA6) != SONYSAT_RESULT_OK){
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x73, ((((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) || (u1_Temp!=0x00)) ? 0x7C : 0xCE)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA8 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA8) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, ((((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) || (u1_Temp!=0x00)) ? 0x73 : 0x93)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4C, ((((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) || (u1_Temp!=0x00)) ? 0x73 : 0xC3)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x64, ((((pDemodCtx->is_canal_digital_ts)&&(DVB_DEF2==0)) || (u1_Temp!=0x00)) ? 0x73 : 0xA3)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    //For other special setting-----------------------------BEGIN----------------------------------//
    /* Set SLV-T Bank : 0xA7 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA7) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE9, pParameter[12]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, pParameter[12]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA6 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA6) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBD, (pParameter[13] ? 0x1E : 0xF4)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA8 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA8) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4D, (pParameter[13] ? 0xAD : 0xDD)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6B, pParameter[10]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x76, pParameter[10]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    //For other special setting-----------------------------END----------------------------------//
    return sonysat_result;
}

void sonysat_init_eeprom_non_canaldigital(UINT8 tunerID)
{
    UINT8 pDftVal[addr_length_CANAL_DIGITAL] = {0x00, 0x00, 0x7E, 0xCE, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0B, 0x00, 0x02};
    UINT8 pValue[addr_length_CANAL_DIGITAL] = {0};
    UINT8 u1_initialed_flag = 0xFF;
    
    sonysat_get_flag_non_canaldigital(tunerID,u1_initialed_flag);
    if(u1_initialed_flag != 0xAA){ //EEPROM is not initialed.
        sonysat_set_eeprom_non_canaldigital(tunerID,pDftVal); //Initial EEPROM with default value
        sonysat_set_demod_non_canaldigital(tunerID,pDftVal); //Set demod registers with default value
        sonysat_set_flag_non_canaldigital(tunerID,0xAA); //After initial EEPROM, set flag = 0xAA
        DVB_DEF2 = 0x00;
    }else{
        sonysat_get_eeprom_non_canaldigital(tunerID,pValue); //Get value from EEPROM
        sonysat_set_demod_non_canaldigital(tunerID,pValue); //Set value to DEMOD
        DVB_DEF2 = pValue[7];
    }
}
#endif

#ifdef CC_PROJECT_FOR_SONY
static sonysat_result_t sonysat_set_ts_maxfreq(UINT8 tunerID, DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    uint8_t ui1_ts_freq = 0;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;

    if(pParameter->t_ts_maxfreq.b_setvalid == TRUE)
    {
        ui1_ts_freq = (uint8_t)pParameter->t_ts_maxfreq.ui1_ts_maxfreq;
        if(ui1_ts_freq > 3)
        {
            ui1_ts_freq = 0;
        }

        ui1_ts_freq += 8;
        mcDBG_MSG_L1(("CXD2854[%d] : Set TS Max freq: %d\n", tunerID, ui1_ts_freq));
        sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_ERR(("Error in WriteOneRegister(%s)\n", sonysat_FormatResult(sonysat_result)));
        }
        sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, ui1_ts_freq);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_ERR(("Error in WriteOneRegister(%s)\n", sonysat_FormatResult(sonysat_result)));
        }
    }

    return sonysat_result;
}
#endif

//-----------------------------------------------------------------------------
// Stopwatch function implementation necessary for integ code (Sony specific)
//-----------------------------------------------------------------------------
sonysat_result_t sonysat_stopwatch_start (sonysat_stopwatch_t * pStopwatch)
{
    SONYSAT_TRACE_ENTER("sonysat_stopwatch_start");

    if (!pStopwatch) {
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = x_os_get_sys_tick() * x_os_get_sys_tick_period();

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_stopwatch_sleep (sonysat_stopwatch_t * pStopwatch, uint32_t ms)
{
    SONYSAT_TRACE_ENTER("sonysat_stopwatch_sleep");
    if (!pStopwatch) {
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    SONYSAT_ARG_UNUSED(*pStopwatch);
    SONYSAT_SLEEP (ms);

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_stopwatch_elapsed (sonysat_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
    SONYSAT_TRACE_ENTER("sonysat_stopwatch_elapsed");

    if (!pStopwatch || !pElapsed) {
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    *pElapsed = x_os_get_sys_tick() * x_os_get_sys_tick_period() - pStopwatch->startTime;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

//-----------------------------------------------------------------------------
// DiSEqC command handling (Sony specific)
//-----------------------------------------------------------------------------
static BOOL sonysat_DiseqcCommand(UINT8 tunerID, DISEQC_BUS_CMD_T *pCmd, uint8_t repeatCount)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t *pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t *pInteg = &pDemodCtx->integ;
    sonysat_lnbc_t *pLnbc = &pDemodCtx->lnbc;
    DVBS_PD_CTX_T *pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    int32_t lnbcConfigVoltageLow = 0;
    int32_t lnbcConfigVoltageHigh = 0;
#ifdef Debug_time
    HAL_TIME_T TimeStart,TimeEnd,TimeDst;
    HAL_GetTime(&TimeStart);
#endif

    mcDBG_MSG_L2(("CXD2854[%d] : %s\n", tunerID, __FUNCTION__));

    // For testing without LNBC
    if(pInteg->pLnbc == NULL){
        mcDBG_MSG_L1(("No LNBC!\n"));
        return 1;
    }

    DVBS_TunerConnect(tunerID);

    if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK){
        /* For UniCable sequence */
        pPdCtxDVBS->fgUniCab = TRUE; /* Set unicable flag */
        #ifdef CC_DEMOD_NON_DVBS_LNBC_OUTPUT_0V
        if((pLnbc) && (pLnbc->WakeUp)){
            if(pLnbc->state == SONYSAT_LNBC_STATE_SLEEP){
                pLnbc->WakeUp(pLnbc);
                mcDBG_MSG_L2(("[Unicable][%d] ......WakeUp LNBC......\n", tunerID));
            }
        }
        #endif
        x_memcpy(&pPdCtxDVBS->t_UniCab_info, &pCmd->t_unicable_info, sizeof(TUNER_UNICABLE_CC_T));
        DVBS_OduChannelChg(tunerID, &pCmd->t_unicable_info);
    } else {
        /* Not UniCable sequence */
        pPdCtxDVBS->fgUniCab = FALSE; /* Reset unicable flag */
        if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_LNB_POWER_MASK){

            switch(pCmd->ui1_lnb_power){
            case 0: // Power Off
                mcDBG_MSG_L1(("CXD2854[%d] : Set LNBC Power Off\n", tunerID));
                sonysat_result = pLnbc->Sleep(pLnbc);
                break;
            case 1: // Normal
                mcDBG_MSG_L1(("CXD2854[%d] : Set LNBC 13V/18V\n", tunerID));
#if defined(SONYSAT_LNBC_MPS_MP8126)
                lnbcConfigVoltageLow = MPS_MP8126_CONFIG_VOLTAGE_LOW_13V;
                lnbcConfigVoltageHigh = MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V;
#elif defined(SONYSAT_LNBC_ST_LNBH29)
                lnbcConfigVoltageLow = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V;
                lnbcConfigVoltageHigh = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
                lnbcConfigVoltageLow = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8303_1)
                lnbcConfigVoltageLow = ALLEGRO_A8303_1_CONFIG_VOLTAGE_LOW_13_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8303_1_CONFIG_VOLTAGE_HIGH_18_667V;
#endif
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE, lnbcConfigVoltageLow);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(LOW_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE, lnbcConfigVoltageHigh);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(HIGH_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                break;
            case 2: // Plus
                mcDBG_MSG_L1(("CXD2854[%d] : Set LNBC 14V/19V\n", tunerID));
#if defined(SONYSAT_LNBC_MPS_MP8126)
                lnbcConfigVoltageLow = MPS_MP8126_CONFIG_VOLTAGE_LOW_14V;
                lnbcConfigVoltageHigh = MPS_MP8126_CONFIG_VOLTAGE_HIGH_19V;
#elif defined(SONYSAT_LNBC_ST_LNBH29)
                lnbcConfigVoltageLow = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_667V;
                lnbcConfigVoltageHigh = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_817V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
                lnbcConfigVoltageLow = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_14_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_19_667V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8303_1)
                lnbcConfigVoltageLow = ALLEGRO_A8303_1_CONFIG_VOLTAGE_LOW_14_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8303_1_CONFIG_VOLTAGE_HIGH_19_667V;
#endif
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE, lnbcConfigVoltageLow);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(LOW_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE, lnbcConfigVoltageHigh);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(HIGH_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                break;
            case 3: // power on to match MW
#if defined(SONYSAT_LNBC_MPS_MP8126)
                lnbcConfigVoltageLow = MPS_MP8126_CONFIG_VOLTAGE_LOW_13V;
                lnbcConfigVoltageHigh = MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V;
#elif defined(SONYSAT_LNBC_ST_LNBH29)
                lnbcConfigVoltageLow = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V;
                lnbcConfigVoltageHigh = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
                lnbcConfigVoltageLow = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8303_1)
                lnbcConfigVoltageLow = ALLEGRO_A8303_1_CONFIG_VOLTAGE_LOW_13_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8303_1_CONFIG_VOLTAGE_HIGH_18_667V;
#endif
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE, lnbcConfigVoltageLow);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(LOW_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                sonysat_result = pLnbc->SetConfig (pLnbc, SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE, lnbcConfigVoltageHigh);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pLnbc->SetConfig(HIGH_VOLTAGE) (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
                break;
            default:
                mcDBG_MSG_L1(("set lnb power : not supported (%d)\n", pCmd->ui1_lnb_power));
                return 0;
            }

            if((pCmd->ui1_lnb_power == 1) || (pCmd->ui1_lnb_power == 2)||(pCmd->ui1_lnb_power == 3)){
                mcDBG_MSG_L1(("CXD2854[%d] : Set LNBC Power On\n", tunerID));
                if(pLnbc->state == SONYSAT_LNBC_STATE_SLEEP){
                    // Need to wakeup!!
                    sonysat_result = pLnbc->WakeUp(pLnbc);
                    mcDBG_MSG_L1(("CXD2854[%d] : Waiting for DiSEqC switch stabilization.\n", tunerID));
                    mcDELAY_MS(500); // TODO: Is it OK for MTK ?
                }
            }
        }

        /*
          CXD2854 has DiSEqC hardware and it automatically do ALL DiSEqC sequence.
          (Stop continuous tone -> Voltage control -> Send DiSEqC command -> Tone burst -> Wait)
        */
        if((pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE)
            && (pCmd->ui4_control_mask & (DISEQC_CMD_CTRL_POLARIZATION_MASK | DISEQC_CMD_CTRL_FULL_CMD_MASK
            | DISEQC_CMD_CTRL_TONE_BURST_MASK | DISEQC_CMD_CTRL_22K_MASK))){
            uint8_t isVoltageHigh = 0;
            sonysat_toneburst_mode_t toneburst_mode = SONYSAT_TONEBURST_MODE_OFF;
            uint8_t isToneOn = 0;
            uint8_t isDiSEqC = 0;
            sonysat_diseqc_message_t diseqcCommand;

            if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_POLARIZATION_MASK){
                if(pCmd->ui1_polarity_13v_18v)
                {
                    mcDBG_MSG_L1(("CXD2854[%d] : Set 18V\n", tunerID));
                    isVoltageHigh = 1;
                }
                else
                {
                    mcDBG_MSG_L1(("CXD2854[%d] : Set 13V\n", tunerID));
                    isVoltageHigh = 0;
                }
            }else{
                // Use previous voltage.
                isVoltageHigh = pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH ? 1 : 0;
            }

            if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK){
                // DiSEqC 1.X command
                UINT8 cmdlength = 0;
                INT32 i = 0;

                mcDBG_MSG_L1(("CXD2854[%d] : DiSEqC command exist\n", tunerID));
                isDiSEqC = 1;

                cmdlength = pCmd->ui1_cmd_len;
                if(cmdlength > SONYSAT_DISEQC_MESSAGE_LENGTH){
                    cmdlength = SONYSAT_DISEQC_MESSAGE_LENGTH;
                }

                if(cmdlength == 0){
                    mcDBG_MSG_L1(("CXD2854[%d] : DiSEqC command enabled but command data is none.\n", tunerID));
                    isDiSEqC = 0;
                }else{
                    mcDBG_MSG_L1(("CXD2854[%d] : ", tunerID));
                    diseqcCommand.length = cmdlength;
                    for(i = 0; i < cmdlength; i++){
                        diseqcCommand.data[i] = pCmd->aui1_cmd[i];
                        mcDBG_MSG_L1(("0x%02x  ", pCmd->aui1_cmd[i]));
                    }
                }
            }else{
                // Disable DiSEqC command
                mcDBG_MSG_L1(("CXD2854[%d] : No DiSEqC command\n", tunerID));
                isDiSEqC = 0;
            }

            if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_TONE_BURST_MASK){
                if(1==pCmd->ui1_tone_burst) {
                    mcDBG_MSG_L1(("CXD2854[%d] : Tone burst 1\n", tunerID));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_B;
                } else if(0==pCmd->ui1_tone_burst) {
                    mcDBG_MSG_L1(("CXD2854[%d] : Tone burst 0\n", tunerID));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_A;
                } else {
                    mcDBG_MSG_L1(("CXD2854[%d] : Tone burst off\n", tunerID));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_OFF;
                }
            }
            if((pCmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) && (pCmd->b_22k)){
                mcDBG_MSG_L1(("CXD2854[%d] : 22k tone on\n", tunerID));
                isToneOn = 1;
            }else{
                mcDBG_MSG_L1(("CXD2854[%d] : 22k tone off\n", tunerID));
                isToneOn = 0;
            }

            //repeatCount = pCmd->ui1_repeat_cmd; // Not used for now
#ifdef SONYSAT_LNBC_MPS_MP8126
            if(pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO){
                // Change TXEN for Unicable to GPO for DiSEqC
                sonysat_result = pInteg->pLnbc->SetVoltage(pInteg->pLnbc, (isVoltageHigh ? SONYSAT_LNBC_VOLTAGE_HIGH : SONYSAT_LNBC_VOLTAGE_LOW));
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in pInteg->pLnbc->SetVoltage (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
            }
#endif // SONYSAT_LNBC_MPS_MP8126

            if(isDiSEqC){
                // DiSEqC command
                sonysat_result = sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand(pInteg, isVoltageHigh,
                    toneburst_mode, isToneOn, &diseqcCommand, repeatCount, NULL, 0);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
            }else if(toneburst_mode != SONYSAT_TONEBURST_MODE_OFF){
                // Toneburst only
                sonysat_result = sonysat_integ_sat_device_ctrl_TransmitToneBurst(pInteg, isVoltageHigh,
                    toneburst_mode, isToneOn);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in sonysat_integ_sat_device_ctrl_TransmitToneBurst (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
            }else{
                // Set Voltage and tone
                sonysat_result = sonysat_integ_sat_device_ctrl_SetVoltageTone(pInteg, isVoltageHigh, isToneOn);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("Error in sonysat_integ_sat_device_ctrl_SetVoltageTone (%s)\n", sonysat_FormatResult(sonysat_result)));
                    return 0;
                }
            }
        }
    }

#ifdef Debug_time
    HAL_GetTime(&TimeEnd);
    HAL_GetDeltaTime(&TimeDst,&TimeStart, &TimeEnd);
    mcDBG_MSG_L1(("CXD2854[%d] : [sonysat_HandleDiseqcCommand] time:%ld.\n", tunerID, TimeDst.u4Micros));
#endif

    return 1; // Success
}

static BOOL sonysat_HandleDiseqcCommand(UINT8 tunerID, DISEQC_BUS_CMD_T *pCmd)
{
    return sonysat_DiseqcCommand(tunerID, pCmd, 2);
}

static BOOL sonysat_HandleResetCommand(UINT8 tunerID, DISEQC_BUS_CMD_T *pCmd)
{
    return sonysat_DiseqcCommand(tunerID, pCmd, 1);
}

static void Vendor_ResendDiseqcCmmd(UINT8 tunerID, DISEQC_BUS_CMD_T *diseqc_cmd)
{
    sonysat_HandleDiseqcCommand(tunerID, diseqc_cmd);
}

//-----------------------------------------------------------------------------
// Blindscan implementation (Sony specific)
//-----------------------------------------------------------------------------
static void sonysat_blindscan_callback (sonysat_integ_t * pInteg,
                                     sonysat_integ_dvbs_s2_blindscan_result_t * pResult)
{
    UINT8 voltage = POL_UNKNOWN;
    UINT8 tunerID = 0;
    sonysat_demod_ctx_t * pDemodCtx = NULL;
    // This callback function is called from Sony internal driver code.
    if(!pInteg || !pResult){
        return;
    }

    if (pInteg == &g_sonysat_demod_ctx[0].integ) {
        // Main tuner
        tunerID = 0;
    } else {
        // Sub tuner
        tunerID = 1;
    }
    pDemodCtx = &g_sonysat_demod_ctx[tunerID];

    if(pResult->eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_DETECT){
        // DVB-S/S2 signal was found.
        mcDBG_MSG_L2(("CXD2854[%d] : ========== Signal Found!! ==========\n", tunerID));
        if(pResult->tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS){
            mcDBG_MSG_L2(("CXD2854[%d] : System : DVB-S\n", tunerID));
        }else if(pResult->tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS2){
            mcDBG_MSG_L2(("CXD2854[%d] : System : DVB-S2\n", tunerID));
        }else{
            mcDBG_MSG_L2(("CXD2854[%d] : System : Unknown\n", tunerID));
        }
        mcDBG_MSG_L2(("CXD2854[%d] : Frequency (kHz) : %d\n", tunerID, pResult->tuneParam.centerFreqKHz));
        mcDBG_MSG_L2(("CXD2854[%d] : Symbol rate (ksps) : %d\n", tunerID, pResult->tuneParam.symbolRateKSps));

        // Save signal information to global.
        x_memcpy(&pDemodCtx->tunedSignal, &pResult->tuneParam, sizeof(sonysat_dvbs_s2_tune_param_t));
        // Callback
#ifndef SONYSAT_DUMMY_DEBUG
        if(pInteg->pLnbc != NULL)
        {
            voltage = (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH ? POL_LIN_HORIZONTAL : POL_LIN_VERTICAL);
        }
        DVBS_AutoScanCallback(tunerID,
            1,
            voltage,
            pResult->tuneParam.centerFreqKHz,
            pResult->tuneParam.symbolRateKSps,
            FEC_I_UNKNOWN);
#endif
    }else if(pResult->eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_PROGRESS){
        // Progress was updated.
        mcDBG_MSG_L2(("CXD2854[%d] : Blindscan progress : %d\n", tunerID, pResult->progress));
        pDemodCtx->blindscanProgress = pResult->progress;

        // Progress update callback
#ifndef SONYSAT_DUMMY_DEBUG
        DVBS_AutoScanCallback(tunerID, 0, 0, 0, 0, 0);
#endif
    }

    if(BlindScan_MidWare_Break(tunerID)){
        sonysat_integ_Cancel(pInteg); // Set cancel flag to exit blindscan.
    }
}

#ifdef CC_PROJECT_FOR_SONY
extern void sony_cxd2854_Semalock(UINT8 u1TunerID,BOOL lock);

static uint8_t sonysat_GetLnbShort(UINT8 tunerID)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t * pInteg = &pDemodCtx->integ;
    sonysat_lnbc_t * pLnbc = &pDemodCtx->lnbc;
    UINT8 ocp = 0;
    UINT8 tsd = 0;
    UINT8 u1OverLoad=0;

    if(pInteg->pLnbc == NULL){
        return 0; //Error Return 0
    }
    if(pLnbc->state == SONYSAT_LNBC_STATE_SLEEP){
        return 0;
    }
    if(pLnbc->state == SONYSAT_LNBC_STATE_OVERLOAD){
        u1OverLoad = 1; //Overload return 1
    }else{
        sony_cxd2854_Semalock(tunerID,1);
        if(pLnbc->MonitorOverload){
            if(SONYSAT_RESULT_OK != pLnbc->MonitorOverload(pLnbc,&ocp)){
                ocp = 0;//previous value
            }
        }
        if(pLnbc->MonitorThermalShutdown){
            if(SONYSAT_RESULT_OK != pLnbc->MonitorThermalShutdown(pLnbc,&tsd))
            {
                tsd = 0;//previous value
            }
        }
        /* bit1 : tsd, bit : ocp */
        u1OverLoad = ((tsd << 1) & 0x02) | (ocp & 0x01);
        if(u1OverLoad){
            mcDBG_MSG_L2(("CXD2854[%d] LNB Overload Power Off\n", tunerID));
            pLnbc->Sleep(pLnbc);
            pLnbc->state = SONYSAT_LNBC_STATE_OVERLOAD;
        }
        sony_cxd2854_Semalock(tunerID,0);
    }
    mcDBG_MSG_L2(("CXD2854[%d] LNB Overload state = %d\n", tunerID, u1OverLoad));
    return (u1OverLoad);
}
#endif /* CC_PROJECT_FOR_SONY */


#ifndef SONYSAT_DUMMY_DEBUG
static VOID Vendor_BlindScan(UINT8 tunerID)
#else
VOID Vendor_BlindScan(UINT8 tunerID) // ONLY for debug
#endif
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t * pInteg = &pDemodCtx->integ;
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_dvbs_s2_blindscan_seq_t * pSeq;
    sonysat_integ_dvbs_s2_blindscan_param_t blindscan_param;

    // sonysat_demod_dvbs_s2_blindscan_seq_t size is big, so allocate in heap.
    pSeq = (sonysat_demod_dvbs_s2_blindscan_seq_t*) x_mem_alloc_virtual(sizeof(sonysat_demod_dvbs_s2_blindscan_seq_t));
    if(pSeq == NULL){
        // Oops... memory allocation failed...
        mcDBG_MSG_L2(("[Vendor_BlindScan] Error : Memory allocation for blindscan was failed!!\n"));
        return;
    }

    // Reset cancel flag
    sonysat_atomic_set (&(pInteg->cancel), 0);

    // Parameter initialization.
    blindscan_param.minFreqKHz = pDemodCtx->bs_start_freq * 1000; // kHz -> MHz
    blindscan_param.maxFreqKHz = pDemodCtx->bs_stop_freq * 1000; // kHz -> MHz
    blindscan_param.minSymbolRateKSps = 1000;  // From 1Msps
    blindscan_param.maxSymbolRateKSps = 45000; // To 45Msps

    if(Util_GetConnMode(pPdCtxDVBS, CONN_MODE_SCAN) == CONN_MODE_UNICABLE_SCAN){
        // Unicable
        sonysat_result = sonysat_integ_sat_device_ctrl_DisableSinglecable(pInteg);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("[Vendor_BlindScan] Error at sonysat_integ_sat_device_ctrl_DisableSinglecable\n"));
        }
        sonysat_result = sonysat_integ_sat_device_ctrl_EnableSinglecable(pInteg, &pDemodCtx->singlecableData, NULL);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("[Vendor_BlindScan] Error : sonysat_integ_sat_device_ctrl_EnableSinglecable.\n"));
        }
        sonysat_result = sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams(pInteg,
            SONYSAT_SINGLECABLE_ADDRESS_ALL_DEVICES,
            get_bank(pPdCtxDVBS->t_UniCab_info.e_bank),
            get_userband(pPdCtxDVBS->t_UniCab_info.e_user_band),
            (UINT32)pPdCtxDVBS->t_UniCab_info.ui2_band_freq * 1000,
            0,
            0);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("[Vendor_BlindScan] Error : sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams.\n"));
        }
    } else {
        sonysat_result = sonysat_integ_sat_device_ctrl_DisableSinglecable(pInteg);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("[Vendor_BlindScan] Error at sonysat_integ_sat_device_ctrl_DisableSinglecable\n"));
        }
    }

    sonysat_result = sonysat_integ_dvbs_s2_BlindScan(pInteg, pSeq, &blindscan_param, sonysat_blindscan_callback);
    if(sonysat_result == SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("CXD2854[%d] : [Vendor_BlindScan] Blindscan success.\n", tunerID));
    }else{
        mcDBG_MSG_L2(("CXD2854[%d] : [Vendor_BlindScan] Blindscan failed. (%s)\n", tunerID, sonysat_FormatResult(sonysat_result)));
    }

    // Debug information for memory management
    mcDBG_MSG_L2(("CXD2854[%d] : (Blindscan info) Max Used Count : %d\n", tunerID, pSeq->commonParams.storage.maxUsedCount));
    mcDBG_MSG_L2(("CXD2854[%d] : (Blindscan info) Current Used Count : %d\n", tunerID, pSeq->commonParams.storage.currentUsedCount));
    mcDBG_MSG_L2(("CXD2854[%d] : (Blindscan info) Max Used Power Count : %d\n", tunerID, pSeq->commonParams.storage.maxUsedPowerCount));

    mcDBG_MSG_L1(("CXD2854[%d] : [Vendor_BlindScan] end.!\n", tunerID));
    // Release allocated memory
    x_mem_free(pSeq);
    // Force to go to Sleep state
    sonysat_integ_SleepS(pInteg);
    return;
}

//-------------------------------------------------------------------------
/**
 *  This API Notify MW scan status,do sema lock wait MW continue CMD
 *  @param   UINT8 signal       signal status:0 unlock,1 lock
 *  @param   UINT8 polar       polar type
 *  @param   UINT32 freq    locked TP freq
 *  @param   UINT32 sym locked TP sym
 *  @param   UINT8 fec        Code rate
 */
//-------------------------------------------------------------------------
static INT32 DVBS_AutoScanCallback(UINT8 tunerID, UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    //mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    if(BlindScan_MidWare_Break(tunerID))
    {
        mcDBG_MSG_L1(("[DVBS_AutoScanCallback] MD has been broken....\n"));
        //mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
        return 0;
    }

    while(1)
    {
        if(x_sema_lock_timeout(pDemodCtx->AutoScan_callback, 10) == OSR_OK)
        {
            mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_AutoScanCallback] reset AutoScan_callback...\n", tunerID));
            continue;
        }
        else
        {
            break;
        }
    }
    if (signal==1)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_AutoScanCallback] TP_Locked\n", tunerID));
        Tuner_Notify(DRV_COND_STATUS, TUNER_COND_STATUS_GOT_SIGNAL);
    }
    else if(signal==0)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_AutoScanCallback] Next Band Request\n", tunerID));
        Tuner_Notify(DRV_COND_STATUS, TUNER_COND_STATUS_LOST_SIGNAL);
    }
    else if(signal==2)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_AutoScanCallback]  AUTOSCAN_END\n", tunerID));
        Tuner_Notify(DRV_COND_STATUS, TUNER_COND_STATUS_AUTOSCAN_END);   //at the end of blind scan
    }

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    pDemodCtx->Monitor_waiting_semaphore_flag = BOOL_TRUE;
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    //x_sema_lock_timeout(AutoScan_callback,8000);//liuyuan debug use
    mcSEMA_LOCK_MTFE(pDemodCtx->AutoScan_callback);
    mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_AutoScanCallback]  Got Semphore..\n", tunerID));
    pDemodCtx->Monitor_waiting_semaphore_flag = BOOL_FALSE;

    return 0;
}

static BOOL SetBreakMonitor(DVBS_PD_CTX_T *psPdCtx)
{
    UINT8 u1Cnt=0;

    psPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psPdCtx->fgPIMoniStatus){  //PI really doing PI Monitor
        while(psPdCtx->fgPIMoniStatus){
            //wait PI Monitor really finish break
            if(u1Cnt>250){  //in case of deadlock
                break;
            }
            u1Cnt++;
            mcDBG_MSG_L1(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n", u1Cnt));
            //  mcDELAY_MS(10);
            x_thread_delay((UINT32)(10));
        }
    }

    return TRUE;
}
static void ResumeMonitor(DVBS_PD_CTX_T *psPdCtx)
{
    mcDBG_MSG_L1(("[ResumeMonitor] is call\r\n"));
    psPdCtx->fgBreakMonitor = FALSE;
    mcSEMA_UNLOCK_MTFE(psPdCtx->t_escape_mon_suspend);
}
/************************************************************************
*  MonitorProcess
*  Monitor thread process function.
*  @param  void.
*  @retval  void.
************************************************************************/
static void MonitorProcess(VOID *pvArg)
{
    UINT8 uNcount=0;
    UINT8 uNcount_2=0;
    UINT8 tunerID = 0;
    DVBS_PD_CTX_T * psPdCtx = *(DVBS_PD_CTX_T **) pvArg;
    sonysat_demod_ctx_t * pDemodCtx = NULL;
    #ifdef SONYSAT_LNBC_ALLEGRO_A8303_1
    sonysat_lnbc_t * pLnbc = NULL;
    UINT8 uNcount_3 = 0;
    UINT8 u1Disable = 0;
    #endif

    if (psPdCtx == g_sonysat_demod_ctx[1].pPdCtxDVBS){
        tunerID = 1;
        pDemodCtx = &g_sonysat_demod_ctx[1];
    } else {
        tunerID = 0;
        pDemodCtx = &g_sonysat_demod_ctx[0];
    }

    mcDBG_MSG_L2(("CXD2854[%d] : MonitorProcess start!\n", tunerID));

    while (psPdCtx->fgMonitorThreadRunning)
    {
        if (psPdCtx->fgBreakMonitor){
            mcDBG_MSG_L1(("CXD2854[%d] : Entering DVBS mon_suspend\n", tunerID));
            mcSEMA_LOCK_MTFE(psPdCtx->t_escape_mon_suspend);
            mcDBG_MSG_L1(("CXD2854[%d] : Escape DVBS mon_suspend\n", tunerID));
        }else{
            if(pDemodCtx->do_blindscan == 1){
                psPdCtx->fgPIMoniStatus = TRUE;
                // update the frequency before blind scan
                pDemodCtx->tunedSignal.centerFreqKHz = (pDemodCtx->bs_start_freq * 1000);
                Vendor_BlindScan(tunerID);
                psPdCtx->fgPIMoniStatus = FALSE;
                pDemodCtx->do_blindscan = 0;
                // Here, demod is in sleep state.
                // update the final frequency before notify end
                pDemodCtx->tunedSignal.centerFreqKHz = (pDemodCtx->bs_stop_freq * 1000) - 500;
                pDemodCtx->tunedSignal.symbolRateKSps = 0;
                DVBS_AutoScanCallback(tunerID, 2, 0, 0, 0, 0);
            }else{
                mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
                /*UEC*/
                if(uNcount_2 == 1){
                    UINT32 per = 0;
                    UINT64 temp = 0;
                    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
                    sonysat_demod_t * pDemod = &pDemodCtx->demod;
                    uNcount_2 = 0;
                    sonysat_result = sonysat_demod_dvbs_s2_monitor_PER(pDemod, &per);
                    if(sonysat_result == SONYSAT_RESULT_OK){
                        temp = (UINT64)(pDemodCtx->u4Uec) + (UINT64)per;
                        if(temp > 0xffffffff){
                            temp = 0xffffffff;
                            pDemodCtx->u4Uec = (UINT32)temp;
                            mcDBG_MSG_L2(("UEC(max) : %lu\n", pDemodCtx->u4Uec));
                        }else{
                            pDemodCtx->u4Uec = (UINT32)temp;
                            mcDBG_MSG_L2(("UEC : %lu\n", pDemodCtx->u4Uec));
                        }
                    }else{
                        mcDBG_MSG_L2(("Error in sonysat_demod_dvbs_s2_monitor_PER (%s)\n", sonysat_FormatResult(sonysat_result)));
                    }
                }else{
                    uNcount_2 ++;
                }
                /*LNBC monitor for A8303_1*/
                #ifdef SONYSAT_LNBC_ALLEGRO_A8303_1
                if(uNcount_3 == 2){
                    uNcount_3 = 0;
                    pLnbc = &pDemodCtx->lnbc;
                    if((pLnbc)&&(pLnbc->MonitorDisable)&&(pLnbc->Restart)){
                        if(SONYSAT_RESULT_OK == pLnbc->MonitorDisable(pLnbc,&u1Disable)){
                            if(u1Disable){
                                if(pLnbc->state != SONYSAT_LNBC_STATE_SLEEP){
                                    pLnbc->Restart(pLnbc);
                                }
                            }
                        }
                    }
                }else{
                    uNcount_3 ++;
                }
                #endif
                /*DiSEqC command*/

                if(pDemodCtx->set_flag == 1){
                   uNcount=0;
                   pDemodCtx->set_flag = 0;
                }
                if(pDemodCtx->channel_locked_flag==0){
                    uNcount++;
                    if(uNcount>=3){
                        uNcount=0;
                        Vendor_ResendDiseqcCmmd(tunerID, &pDemodCtx->diseqc_cmd);
                    }
                }else{
                    uNcount=0;
                }
                mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock);
            }
            x_thread_delay((UINT32)(500));
        }
    }

    psPdCtx->t_monitor_thread = 0;
    mcDBG_MSG_L1(("CXD2854[%d] : MonitorProcess exit!\n", tunerID));
    x_thread_exit();
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static INT32 PD_StartMonitorEngine(DVBS_PD_CTX_T *psPdCtx)
{
    UINT8 tunerID;
    if(psPdCtx == g_sonysat_demod_ctx[1].pPdCtxDVBS)
    {
        tunerID = 1;
    }
    else
    {
        tunerID = 0;
    }
    psPdCtx->fgMonitorThreadRunning = TRUE;
    if(tunerID == 0) //Two threads are created
    {
        if(x_thread_create(&psPdCtx->t_monitor_thread,
            "DVBS_Monitor0",
            cTHREAD_STACK_SIZE*10,cTHREAD_PRIORITY,
            MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
        {
            mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
    }
    else
    {
        if(x_thread_create(&psPdCtx->t_monitor_thread,
            "DVBS_Monitor1",
            cTHREAD_STACK_SIZE*10,cTHREAD_PRIORITY,
            MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
        {
            mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
    }
    mcDBG_MSG_L1(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  PD_StopMonitorEngine
*  Function which stops monitor thread engine.
*  @param  void.
*  @retval  DRVAPI_TUNER_OK : Stop monitor thread successfully.
************************************************************************/
static INT32 PD_StopMonitorEngine(DVBS_PD_CTX_T *psPdCtx)
{
    while (psPdCtx->t_monitor_thread){
        x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_L2(("DVBS PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

#ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
void DVBS_Set_Demod_non_CanalDigital(UINT8 tunerID)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    UINT8 pValue[addr_length_CANAL_DIGITAL] = {0};
    UINT8 i=0;
    
    if(!pDemodCtx->is_canal_digital_ts){ //non Canal Digital
        for(i=0; i<addr_length_CANAL_DIGITAL;i++){
            pValue[i] = Value_non_CanalDigital[i];
        }
    }
    sonysat_set_demod_non_canaldigital(tunerID,pValue);
}
#endif

//-------------------------------------------------------------------------
/**
 *  This API do connect tuner
 *  @retval  void
 */
//-------------------------------------------------------------------------
static void DVBS_TunerConnect(UINT8 tunerID)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    if(pPdCtxDVBS->fgDisStatus == FALSE)
    {
        /* Has connected, return */
        mcDBG_MSG_L2(("CXD2854[%d] : DVBS has connected!\n", tunerID));
        return;
    }
    //add demod driver implementation code here
    {
        UINT8 data = 0;
        sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
        sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;

        if((pDemod == NULL) || (pDemod->pI2c == NULL) || (pDemod->pI2c->WriteOneRegister == NULL) ||(pDemod->pI2c->ReadRegister == NULL)){
            mcDBG_MSG_L4(("DVBS_TunerConnect NULL pointer!!\n"));
            return;
        }

#if defined(CC_PROJECT_FOR_SONY) || defined(SONYSAT_TUNERMODULE_HELENE)
        // REFOUT enable
        HELENE_Satellite_SetRefOut(tunerID, 1);
#endif

        sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_ERR(("Error in WriteOneRegister\n", sonysat_FormatResult(sonysat_result)));
        }

        sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, &data, 1);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_ERR(("Error in ReadRegister\n", sonysat_FormatResult(sonysat_result)));
        }

        if(data == 0x01){
            /* Demod HW is now in Shutdown state. */
            mcDBG_MSG_L1(("CXD2854[%d] : Return back from Shutdown.\n", tunerID));
            sonysat_result = sonysat_demod_ReinitializeS(pDemod);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("Error in sonysat_demod_ReinitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }else{
            sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_ERR(("Error in WriteOneRegister(%s)\n", sonysat_FormatResult(sonysat_result)));
            }
            sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, &data, 1);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_ERR(("Error in ReadRegister(%s)\n", sonysat_FormatResult(sonysat_result)));
            }

            if((data != 0x0A) && (data != 0x0B)){
                //Need to reinitialize
                mcDBG_MSG_L2(("CXD2854[%d] : Switch to DVB satellite.\n", tunerID));
                sonysat_result = sonysat_demod_ReinitializeS(pDemod);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_ERR(("Error in sonysat_demod_ReinitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
                }
            }
        }
    }

    /* set disconnect status */
    pPdCtxDVBS->fgDisStatus = FALSE;
}

//-------------------------------------------------------------------------
/**
 *  This API implement close tuner. It should performs
 *  1. free all allocated resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
static INT32 DVBS_TunerClose(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));

    pPdCtxDVBS->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtxDVBS->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
    PD_StopMonitorEngine(pPdCtxDVBS);  // stop monitor engine
    x_sema_delete(pPdCtxDVBS->t_escape_mon_suspend);

    //add driver implementation code here
    x_sema_delete(pPdCtxDVBS->hHalLock);
    x_mem_free(pPdCtxDVBS);
    //x_mem_free(psDemodCtx);
    pDemodCtx->ui1_mount_status = 0x00;
    //To Do If you have allocated any resource in TunerOpen(), release them here.

    return (DRVAPI_TUNER_OK);
}

//-------------------------------------------------------------------------
/**
 *  This API do channel acuqusition.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @param   pv_conn_info       middleware tuner info structure, refer u_tuner.h
 *  @param   z_conn_info_len    length of pv_conn_info
 *  @param   _BreakFct          TunerHAL callback API for Vendor_TunerAcq to check whether need to break TunerAcq
 *  @param   *pvArg             pass this parameter to _BreakFct when calling _BreakFct
 *  @retval  1                  TPS LOCK
 *  @retval  0                  TPS UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DVBS_TunerAcq(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;//=(TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
    BOOL fgChannelScan = FALSE;
    BOOL fgLock = FALSE;
    UINT32 freqMHz = 0;
    UINT32 symbolRateKsps = 0;
    UINT16 uiLockStatus = 0;
    TUNER_CONNECTION_MODE_T e_conn_mode=CONN_MODE_UNKNOWN;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t *pInteg = &pDemodCtx->integ;
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    TUNER_DEVICE_CTX_T * pDVBSTunerDeviceCtx = pDemodCtx->pDVBSTunerDeviceCtx;

    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));
    if(pv_conn_info != NULL)
    {
        pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
    }
    else
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_TunerAcq] pv_conn_info is Err.\n", tunerID));
        return (fgLock);
    }
    #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
    pDemodCtx->is_canal_digital_ts = pt_tuner_info->b_canal_digital_ts;
    #endif
    
    //set break function
    e_conn_mode=Util_GetConnMode(pPdCtxDVBS, pt_tuner_info->e_conn_mode);
    if(!DVBS_ValidateMwPara(tunerID, pt_tuner_info, e_conn_mode))
    {
       mcDBG_MSG_ERR(("CXD2854[%d] : Invalid parameter!!\n", tunerID));
       return fgLock;
    }
    pPdCtxDVBS->isBreak = _BreakFct;
    //mutex lock
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Pause monitor thread
    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        mcDBG_MSG_L2(("CXD2854[%d] : Pause monitor process!!\n", tunerID));
        SetBreakMonitor(pPdCtxDVBS);
    }

    DVBS_TunerConnect(tunerID);

    #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT    
    if(!b_already_read_eep){
        sonysat_get_eeprom_non_canaldigital(tunerID, Value_non_CanalDigital);
        b_already_read_eep = TRUE;
    }
    DVBS_Set_Demod_non_CanalDigital(tunerID); 
    #endif

    //Parse parameter from upper layer
    freqMHz = pt_tuner_info->ui4_freq; // in MHz
    symbolRateKsps = pt_tuner_info->ui4_sym_rate; // in ksps

    // Save break function pointer to global
    pDemodCtx->breakFct = _BreakFct;
    pDemodCtx->breakFctArg = pvArg;

    //get conncetion mode
    switch(e_conn_mode)
    {
    case CONN_MODE_SCAN://this mode is for blindscan
    case CONN_MODE_UNICABLE_SCAN:
        {
            mcDBG_MSG_L1(("CXD2854[%d] : Start to BlindScan. Scan range is [%d, %d] MHz !\n", tunerID, pt_tuner_info->ui4_start_freq, pt_tuner_info->ui4_end_freq));
                        /*before AVL_BlindScan()function finished First runnings scan ,MD will call DVBS_TunerGetSync for blindscan start freq.*/
            pDemodCtx->bs_start_freq = (UINT16)(pt_tuner_info->ui4_start_freq); // in MHz
            pDemodCtx->bs_stop_freq = (UINT16)(pt_tuner_info->ui4_end_freq);    // in MHz
            pDemodCtx->do_blindscan = 1;
        }
        break;
    case CONN_MODE_UNKNOWN://channel change
    case CONN_MODE_SURF://preset scan ,single tp scan
    case CONN_MODE_UNICABLE_SURF://unicable
        {
            //call you channel change driver here
            // Sony specific tuning parameter struct
            sonysat_dvbs_s2_tune_param_t tuneParam;

            tuneParam.centerFreqKHz = freqMHz * 1000;
            tuneParam.symbolRateKSps = symbolRateKsps;
            tuneParam.system = SONYSAT_DTV_SYSTEM_ANY; // DVB-S/S2 auto detection

            // Clear cancel flag
            sonysat_atomic_set (&(pInteg->cancel), 0);

            if(CONN_MODE_UNICABLE_SURF==e_conn_mode){
                // Unicable
                sonysat_result = sonysat_integ_sat_device_ctrl_DisableSinglecable(pInteg);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at sonysat_integ_sat_device_ctrl_DisableSinglecable\n"));
                }
                sonysat_result = sonysat_integ_sat_device_ctrl_EnableSinglecable(pInteg,
                    &pDemodCtx->singlecableData,
                    transmitDummyUnicableCommand);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at sonysat_integ_sat_device_ctrl_EnableSinglecable\n"));
                }
                sonysat_result = sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams(pInteg,
                    SONYSAT_SINGLECABLE_ADDRESS_ALL_DEVICES,
                    get_bank(pPdCtxDVBS->t_UniCab_info.e_bank),
                    get_userband(pPdCtxDVBS->t_UniCab_info.e_user_band),
                    (UINT32)pPdCtxDVBS->t_UniCab_info.ui2_band_freq * 1000,
                    0,
                    0);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error : sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams.\n"));
                }
                // Overwrite frequency by unicable information.
                if(pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq > pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq){
                    tuneParam.centerFreqKHz = (UINT32)(pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq - pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq) * 1000;
                } else {
                    tuneParam.centerFreqKHz = (UINT32)(pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq - pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq) * 1000;
                }
            } else {
                // Normal
                sonysat_result = sonysat_integ_sat_device_ctrl_DisableSinglecable(pInteg);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error at sonysat_integ_sat_device_ctrl_DisableSinglecable\n"));
                }
            }

            if(tuneParam.symbolRateKSps == 0){
                //demod auto symbol rate detect case
                sonysat_demod_dvbs_s2_tune_srs_seq_t *pSeq;
                sonysat_integ_dvbs_s2_tune_srs_param_t tune_srs_param;

                mcDBG_MSG_L2(("CXD2854[%d] : [DVBS_TunerAcq] Start auto symbol rate detect tune.\n", tunerID));
                mcDBG_MSG_L2(("CXD2854[%d] : Frequency : %d kHz\n", tunerID, freqMHz * 1000));

                // sonysat_demod_dvbs_s2_blindscan_seq_t size is big, so allocate in heap.
                pSeq = (sonysat_demod_dvbs_s2_tune_srs_seq_t*) x_mem_alloc(sizeof(sonysat_demod_dvbs_s2_tune_srs_seq_t));
                if(pSeq == NULL){
                    // Oops... memory allocation failed...
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error : Memory allocation for auto symbol rate tune was failed!!\n"));
                    uiLockStatus = 0;
                    goto ErrorExit;
                }

                tune_srs_param.centerFreqKHz = tuneParam.centerFreqKHz;
                tune_srs_param.freqRange = SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_FREQ_RANGE_5MHz;

                sonysat_result = sonysat_integ_dvbs_s2_TuneSRS(pInteg, pSeq, &tune_srs_param, &tuneParam);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("CXD2854[%d] : [DVBS_TunerAcq] Auto symbol rate tune success.\n", tunerID));
                    if(tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS){
                        mcDBG_MSG_L2(("CXD2854[%d] : System : DVB-S\n", tunerID));
                    }else if(tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS2){
                        mcDBG_MSG_L2(("CXD2854[%d] : System : DVB-S2\n", tunerID));
                    }else{
                        mcDBG_MSG_L2(("CXD2854[%d] : System : Unknown\n", tunerID));
                    }
                    mcDBG_MSG_L2(("CXD2854[%d] : Frequency (kHz) : %d\n", tunerID, tuneParam.centerFreqKHz));
                    mcDBG_MSG_L2(("CXD2854[%d] : Symbol rate (ksps) : %d\n", tunerID, tuneParam.symbolRateKSps));
                    uiLockStatus = 1;
                }else{
                    mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_TunerAcq] Auto symbol rate tune failed. (%s)\n", tunerID, sonysat_FormatResult(sonysat_result)));
                    uiLockStatus = 0;
                }

                // Debug information for auto symbol rate tune
                mcDBG_MSG_L2(("CXD2854[%d] : (Auto symbol rate tune info) Max Used Count : %d\n", tunerID, pSeq->commonParams.storage.maxUsedCount));
                mcDBG_MSG_L2(("CXD2854[%d] : (Auto symbol rate tune info) Current Used Count : %d\n", tunerID, pSeq->commonParams.storage.currentUsedCount));
                mcDBG_MSG_L2(("CXD2854[%d] : (Auto symbol rate tune info) Max Used Power Count : %d\n", tunerID, pSeq->commonParams.storage.maxUsedPowerCount));

                x_mem_free(pSeq); // Release memory
            }else{
                // Normal tuning
                mcDBG_MSG_L2(("CXD2854[%d] : [DVBS_TunerAcq] Start normal tune.\n", tunerID));
                mcDBG_MSG_L2(("CXD2854[%d] : Frequency : %d kHz, Symbol rate %d ksps\n", tunerID, freqMHz * 1000, symbolRateKsps));

                sonysat_result = sonysat_integ_dvbs_s2_Tune(pInteg, &tuneParam);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("CXD2854[%d] : [DVBS_TunerAcq] Normal tune success.\n", tunerID));
                    uiLockStatus = 1;
                }else{
                    mcDBG_MSG_L2(("CXD2854[%d] : [DVBS_TunerAcq] Normal tune failed. (%s)\n", tunerID, sonysat_FormatResult(sonysat_result)));
                    uiLockStatus = 0;
                }
            }

            // Save signal information to global.
            x_memcpy(&pDemodCtx->tunedSignal, &tuneParam, sizeof(sonysat_dvbs_s2_tune_param_t));

            break;
        }
    default:
        mcDBG_MSG_ERR(("CXD2854[%d] : Invalid mode to DVBS_TunerAcq!\n", tunerID));
        break;
    }

    if((CONN_MODE_SCAN==e_conn_mode) || (CONN_MODE_UNICABLE_SCAN==e_conn_mode))
    {
        uiLockStatus = 1;//scan mode should return LOCK Status.
    }

ErrorExit:

    // Clear break function pointer
    pDemodCtx->breakFct = NULL;
    pDemodCtx->breakFctArg = NULL;

    pDemodCtx->ui4_data_forMW = pt_tuner_info->ui4_data;
    if(uiLockStatus)
    {
        pDemodCtx->channel_locked_flag = 1;
        fgLock=1;
    }
    else
    {
        pDemodCtx->channel_locked_flag = 0;
        fgLock=0;
    }
    pDemodCtx->u4Uec = 0;

    //-------------------------------------------------------------------------------------
    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg) && (!fgChannelScan))
        {
            mcDBG_MSG_L2(("CXD2854[%d] : Enable monitor process!!\n", tunerID));
            ResumeMonitor(pPdCtxDVBS);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
        }
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    // TODO return correct lock status
    return (fgLock);    //if lock return 1, unlock return 0
}

//-------------------------------------------------------------------------
/**
 *  This API check lock status
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  1                  LOCK
 *  @retval  0                  UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DVBS_TunerGetSync(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t * pDemod = &pDemodCtx->demod;
    BOOL fgLock = FALSE;

    if(pDemodCtx->do_blindscan == 1)
    {
        mcDBG_MSG_L4(("CXD2854[%d] : [%s] doing blindscan.\n", tunerID, __FUNCTION__));
        return TRUE;
    }
    mcDBG_MSG_L4(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO Get demod lock status from demod driver and update it as variable fgLock

    {
        UINT8 tsLock = 0;

        sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
        if(sonysat_result == SONYSAT_RESULT_OK){
            if(tsLock){
                fgLock = 1;
            }else{
                fgLock = 0;
            }
        }else{
            fgLock = 0;
        }
    }

    if(fgLock)
    {
        //mcDBG_MSG_L1(" %s ,lock\n",__FUNCTION__);
        pDemodCtx->channel_locked_flag = 1;
    }
    else
    {
        //mcDBG_MSG_L1(" %s ,unlock\n",__FUNCTION__);
        pDemodCtx->channel_locked_flag = 0;
    }
    //END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return fgLock;
}

//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  void
 */
//-------------------------------------------------------------------------
/*
****************
After DVBS_TunerDisc being called,there will be no TUNER_SET_TYPE_SCAN_CONTINUE command .
So the Monitor_waiting_semaphore_flag is important.......
**********************
*/
static void DVBS_TunerDisc(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx[tunerID].integ;

    mcDBG_MSG_L1(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    //TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered

    pPdCtxDVBS->fgDisStatus = TRUE;

    if(pDemodCtx->Monitor_waiting_semaphore_flag == BOOL_TRUE)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_TunerDisc]  111 set to stop Blindscan .\n", tunerID));
        x_sema_unlock(pDemodCtx->AutoScan_callback);
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
//END
    SetBreakMonitor(pPdCtxDVBS);

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    if(pDemodCtx->Monitor_waiting_semaphore_flag == BOOL_TRUE)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [DVBS_TunerDisc]  222 set to stop Blindscan .\n", tunerID));
        x_sema_unlock(pDemodCtx->AutoScan_callback);
    }

    if(pPdCtxDVBS->fgUniCab){
        DVBS_TunerDiscSCIF(tunerID, pPdCtxDVBS);
    }

    sonysat_integ_SleepS(pInteg);

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get modulation information.
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  Outputs:
 *  @param   *_pSignal          Modulation info. structure to be filled.
 *  @retval  void
 */
//-------------------------------------------------------------------------
static void DVBS_TunerGetSignal(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t *pDemod = &pDemodCtx->demod;
    sonysat_dtv_system_t system;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //apply signal code here

    if(_pSignal)
    {
        x_memset(_pSignal, 0, sizeof(SIGNAL));
        //return demod locked frequency(MHz) and symbol rate
        _pSignal->Frequency = (pDemodCtx->tunedSignal.centerFreqKHz + 500) / 1000;

        // Get current system.
        sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &system);
        if(sonysat_result != SONYSAT_RESULT_OK){
            system = SONYSAT_DTV_SYSTEM_UNKNOWN;
        }

        // Symbol rate
        {
            uint32_t symbolRateSps = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_SymbolRate(pDemod, &symbolRateSps);
            if(sonysat_result == SONYSAT_RESULT_OK){
                _pSignal->SymbolRate = symbolRateSps;
            }else{
                _pSignal->SymbolRate = 0;
            }
        }

        // QAM size
        if(system == SONYSAT_DTV_SYSTEM_DVBS){
            _pSignal->QAMSize = (UINT32)MOD_QPSK;
        }else if(system == SONYSAT_DTV_SYSTEM_DVBS2){
            sonysat_dvbs2_plscode_t plscode;

            sonysat_result = sonysat_demod_dvbs2_monitor_PLSCode(pDemod, &plscode);
            if(sonysat_result == SONYSAT_RESULT_OK){
                switch(plscode.modulation){
                case SONYSAT_DVBS2_MODULATION_QPSK:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_QPSK; break;
                case SONYSAT_DVBS2_MODULATION_8PSK:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_8PSK; break;
                case SONYSAT_DVBS2_MODULATION_16APSK:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_16APSK; break; // MOD_APSK_16 is not defined.
                case SONYSAT_DVBS2_MODULATION_32APSK:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_32APSK; break; // MOD_APSK_32 is not defined.
                case SONYSAT_DVBS2_MODULATION_DUMMY_PLFRAME:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_INVALID; break; // Unknown
                case SONYSAT_DVBS_CODERATE_INVALID:
                default:
                    _pSignal->QAMSize = (UINT32)TUNER_MODULATION_INVALID; break; // Unknown
                }
            }else{
                _pSignal->QAMSize = (UINT32)TUNER_MODULATION_INVALID;
            }
        }else{
            _pSignal->QAMSize = (UINT32)TUNER_MODULATION_INVALID;
        }
    }
    else
    {
        mcDBG_MSG_L1(("[DVBS_TunerGetSignal] _pSignal is Err .\n"));
    }
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in terms of percentage.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 DVBS_TunerGetSignalLevel(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    UINT8 u1Ret=0;

    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO Read If_AGC control word and mapping it to signal level(0~100)
    //update it as u1Ret
    u1Ret = sonysat_GetSSI(tunerID);

    mcDBG_MSG_L2(("CXD2854[%d] : SSI : %d\n", tunerID, u1Ret));
    //END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    //example of return
    return (u1Ret);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in multiples of (0.1dBm).
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0 ~ -900           signal level
 */
//-------------------------------------------------------------------------
static INT16 DVBS_TunerGetSignalLeveldBm(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT16 i2Ret=0;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //ToDo:demod driver implemented
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return i2Ret;
}

static UINT32 DVBS_TunerGetSignalPER(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t * pDemod = &pDemodCtx->demod;

    if(pDemodCtx->do_blindscan == 1)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [%s] doing blindscan.\n", tunerID, __FUNCTION__));
        return 0;
    }
    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    // PER unit is 1e-5
    sonysat_result = sonysat_demod_dvbs_s2_monitor_PER(pDemod, (uint32_t*)&u4_ret);
    if(sonysat_result != SONYSAT_RESULT_OK){
        u4_ret = 100000; // Error Rate = 100%
    }else{
        // NOTE: sonysat_demod_dvbx_monitor_PER returns 1e6 unit value.
        u4_ret = (u4_ret + 5) / 10;
    }

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return u4_ret;//modify return value
}

static UINT16 DVBS_TunerGetSignalSNR(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret=0;
    int32_t cnr = 0;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t * pDemod = &pDemodCtx->demod;

    if(pDemodCtx->do_blindscan == 1)
    {
        mcDBG_MSG_L1(("CXD2854[%d] : [%s] doing blindscan.\n", tunerID, __FUNCTION__));
        return 0;
    }
    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO Get SNR(signal-noise ratio) from demod driver
    //update it as u2_ret

    sonysat_result = sonysat_demod_dvbs_s2_monitor_CNR(pDemod, &cnr);
    if(sonysat_result != SONYSAT_RESULT_OK){
        cnr = 0;
    }

    u2_ret = (UINT16)((cnr + 500)/1000); // rounding

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return u2_ret;//modify return value
}

static void DVBS_TunerSetMpgFmt(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcDBG_MSG_L2(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static char *DVBS_TunerGetVer(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcDBG_MSG_L1(("CXD2854[%d] : %s \n", tunerID, __FUNCTION__));
    return (char *)(VENDOR_DRIVER_VER);
}

static INT32 DVBS_TunerNimTest(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    //-------------------------------------------------

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return 0;
}

static void DVBS_TunerSetRegSetting(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerShowRegSetting(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerTestI2C(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerCommand(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDtdCommand(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_integ_t * pInteg = &pDemodCtx->integ;
    sonysat_demod_t * pDemod = &pDemodCtx->demod;
    sonysat_tuner_sat_t * pTuner = &pDemodCtx->tuner;
    sonysat_dtv_system_t currentSystem; // DVB-S or DVB-S2

    //TODO you can add command for debug use here

    UCHAR   ucCmdId = 0;
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t s                          Show signal, TS, DVB system status\n"));
        mcDBG_MSG_L1(("\t d [t|x] [Bank]             Demod register page dump\n"));
        mcDBG_MSG_L1(("\t r [t|x] [Bank] [RegAddr] [Num]    Read demod register\n"));
        mcDBG_MSG_L1(("\t w [t|x] [Bank] [RegAddr] [Value]  Write demod register\n"));
        mcDBG_MSG_L1(("\t ad                         Tuner register page dump\n"));
        mcDBG_MSG_L1(("\t ar [RegAddr] [Num]         Read tuner register\n"));
        mcDBG_MSG_L1(("\t aw [RegAddr] [Value]       Write tuner register\n"));
        mcDBG_MSG_L1(("\t bs [StFreqMHz] [SpFreqMHz] Start to do blind scan\n"));
        mcDBG_MSG_L1(("\t bc                         Continue to do blind scan\n"));
        mcDBG_MSG_L1(("\t bq                         Cancel blind scan\n"));
        mcDBG_MSG_L1(("\t bl [p|c]                   Toggle blind scan log (p:power spectrum, c:candidate)\n"));
        mcDBG_MSG_L1(("\t bp [0|1] [Num]             Change power spectrum mode (0:Off, 1:calc average)\n"));
        mcDBG_MSG_L1(("\t                            Num is average count.(2,4(recommended),8,16,32,64)\n"));
        mcDBG_MSG_L1(("\t bf [SRSps]                 Change LPF setting for power spectrum.\n"));
        mcDBG_MSG_L1(("\t ts                         To show demod TS lock status\n"));
        mcDBG_MSG_L1(("\t be                         To show pre RS or pre BCH BER\n"));
        mcDBG_MSG_L1(("\t ue                         To show UEC value\n"));
        mcDBG_MSG_L1(("\t lv [0/1/2/3/4]             Set LNBC Voltage: 0_off,1_13V,2_14V,3_18V,4_19V\n"));
        mcDBG_MSG_L1(("\t lt [0/1]                   Set LNBC Tone: 0_off,1_22K on\n"));
        mcDBG_MSG_L1(("\t ls                         To check LNBC overload status\n"));
    }
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);

    switch (ucCmdId)
    {
    case 's':
        mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

        /* Current frequency and symbol rate */
        {
            mcDBG_MSG_L1(("Current Signal Frequency : %d.%03d\n",
                g_sonysat_demod_ctx[tunerID].tunedSignal.centerFreqKHz / 1000, g_sonysat_demod_ctx[tunerID].tunedSignal.centerFreqKHz % 1000));

            mcDBG_MSG_L1(("Tuner Tuned Frequency : %d.%03d\n",
                pTuner->frequencyKHz / 1000, pTuner->frequencyKHz % 1000));
        }

        /* System */
        {
            sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &currentSystem);
            if(sonysat_result == SONYSAT_RESULT_OK){
                switch(currentSystem){
                case SONYSAT_DTV_SYSTEM_DVBS:
                    mcDBG_MSG_L1(("System : DVB-S\n")); break;
                case SONYSAT_DTV_SYSTEM_DVBS2:
                    mcDBG_MSG_L1(("System : DVB-S2\n")); break;
                default:
                    mcDBG_MSG_L1(("System : Unknown(%d)\n", currentSystem)); break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_System (%s)\n", sonysat_FormatResult(sonysat_result)));
                currentSystem = SONYSAT_DTV_SYSTEM_UNKNOWN;
            }
        }

        /* Demod Lock Status */
        {
            UINT8 tsLock = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("TS Lock : %d\n", tsLock));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_SyncStat (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* IFAGC */
        {
            uint32_t ifAgc = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifAgc);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("IFAGC (%%) : %d\n", ifAgc * 100 / 8191));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_IFAGCOut (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* Carrier Offset */
        {
            int32_t offset = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_CarrierOffset(pDemod, &offset);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("Carrier Offset (kHz) : %d\n", offset));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_CarrierOffset (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* Symbol Rate */
        {
            uint32_t symbolRateSps = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_SymbolRate(pDemod, &symbolRateSps);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("Symbol Rate (sps) : %d\n", symbolRateSps));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_SymbolRate (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* IQ Sense */
        {
            sonysat_demod_sat_iq_sense_t sense;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_IQSense(pDemod, &sense);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("IQ Sense : %d\n", sense));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbx_monitor_IQSense (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* CNR */
        {
            int32_t cnr = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_CNR(pDemod, &cnr);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("CNR (dB) : %d.%03d\n", cnr/1000U, cnr%1000U));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbx_monitor_CNR (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* BER/FER */
        if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS){
            uint32_t value = 0;

            sonysat_result = sonysat_demod_dvbs_monitor_PreViterbiBER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PreViterbi BER (1e7) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_PreViterbiBER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }

            sonysat_result = sonysat_demod_dvbs_monitor_PreRSBER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PreRS BER (1e7) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_PreRSBER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }else if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS2){
            uint32_t value = 0;

            sonysat_result = sonysat_demod_dvbs2_monitor_PreLDPCBER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PreLDPC BER (1e7) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PreLDPCBER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }

            sonysat_result = sonysat_demod_dvbs2_monitor_PreBCHBER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PreBCH BER (1e9) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PreBCHBER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }

            sonysat_result = sonysat_demod_dvbs2_monitor_PostBCHFER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PostBCH FER (1e6) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PostBCHFER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* PER */
        {
            uint32_t value = 0;

            sonysat_result = sonysat_demod_dvbs_s2_monitor_PER(pDemod, &value);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("PER (1e6) : %d\n", value));
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_PER (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* RSSI */
        {
            int32_t agcLevel = 0;
            sonysat_result = sonysat_integ_dvbs_s2_monitor_RFLevel(pInteg, &agcLevel);
            if(sonysat_result == SONYSAT_RESULT_OK){
                if (agcLevel > 0){
                    agcLevel += 500;
                } else {
                    agcLevel -= 500;
                }
                mcDBG_MSG_L1(("RSSI (dBm) : %d\n", agcLevel / 1000));
            } else {
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_IFAGCOut (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* Quality */
        {
            uint8_t quality = 0;
            sonysat_result = s_s2_monitor_Quality (pDemod, &quality);
            if (sonysat_result == SONYSAT_RESULT_OK) {
                mcDBG_MSG_L1(("Quality (%%) : %d\n", quality));
            } else {
                mcDBG_MSG_L1(("Error in s_s2_monitor_Quality (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* UEC (Uncorrected Error Count) */
        /*
        {
            UINT16 pen = sonysat_GetUEC(tunerID);
            mcDBG_MSG_L1(("UEC : %d\n", pen));
        }
        */

        /* Code Rate (DVB-S) */
        if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS){
            sonysat_dvbs_coderate_t coderate;

            sonysat_result = sonysat_demod_dvbs_monitor_CodeRate(pDemod, &coderate);
            if(sonysat_result == SONYSAT_RESULT_OK){
                switch(coderate){
                case SONYSAT_DVBS_CODERATE_1_2:
                    mcDBG_MSG_L1(("Code Rate : 1/2\n")); break;
                case SONYSAT_DVBS_CODERATE_2_3:
                    mcDBG_MSG_L1(("Code Rate : 2/3\n")); break;
                case SONYSAT_DVBS_CODERATE_3_4:
                    mcDBG_MSG_L1(("Code Rate : 3/4\n")); break;
                case SONYSAT_DVBS_CODERATE_5_6:
                    mcDBG_MSG_L1(("Code Rate : 5/6\n")); break;
                case SONYSAT_DVBS_CODERATE_7_8:
                    mcDBG_MSG_L1(("Code Rate : 7/8\n")); break;
                case SONYSAT_DVBS_CODERATE_INVALID:
                default:
                    mcDBG_MSG_L1(("Code Rate : Unknown(%d)\n", coderate)); break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_CodeRate (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        /* PLS Code (DVB-S2) */
        if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS2){
            sonysat_dvbs2_plscode_t plscode;

            sonysat_result = sonysat_demod_dvbs2_monitor_PLSCode(pDemod, &plscode);
            if(sonysat_result == SONYSAT_RESULT_OK){
                switch(plscode.modulation){
                case SONYSAT_DVBS2_MODULATION_QPSK:
                    mcDBG_MSG_L1(("Modulation : QPSK\n")); break;
                case SONYSAT_DVBS2_MODULATION_8PSK:
                    mcDBG_MSG_L1(("Modulation : 8PSK\n")); break;
                case SONYSAT_DVBS2_MODULATION_16APSK:
                    mcDBG_MSG_L1(("Modulation : 16APSK\n")); break;
                case SONYSAT_DVBS2_MODULATION_32APSK:
                    mcDBG_MSG_L1(("Modulation : 32APSK\n")); break;
                case SONYSAT_DVBS2_MODULATION_DUMMY_PLFRAME:
                    mcDBG_MSG_L1(("Modulation : Dummy PL Frame\n")); break;
                case SONYSAT_DVBS_CODERATE_INVALID:
                default:
                    mcDBG_MSG_L1(("Modulation : Unknown(%d)\n", plscode.modulation)); break;
                }

                switch(plscode.codeRate){
                case SONYSAT_DVBS2_CODERATE_1_4:
                    mcDBG_MSG_L1(("Code Rate : 1/4\n")); break;
                case SONYSAT_DVBS2_CODERATE_1_3:
                    mcDBG_MSG_L1(("Code Rate : 1/3\n")); break;
                case SONYSAT_DVBS2_CODERATE_2_5:
                    mcDBG_MSG_L1(("Code Rate : 2/5\n")); break;
                case SONYSAT_DVBS2_CODERATE_1_2:
                    mcDBG_MSG_L1(("Code Rate : 1/2\n")); break;
                case SONYSAT_DVBS2_CODERATE_3_5:
                    mcDBG_MSG_L1(("Code Rate : 3/5\n")); break;
                case SONYSAT_DVBS2_CODERATE_2_3:
                    mcDBG_MSG_L1(("Code Rate : 2/3\n")); break;
                case SONYSAT_DVBS2_CODERATE_3_4:
                    mcDBG_MSG_L1(("Code Rate : 3/4\n")); break;
                case SONYSAT_DVBS2_CODERATE_4_5:
                    mcDBG_MSG_L1(("Code Rate : 4/5\n")); break;
                case SONYSAT_DVBS2_CODERATE_5_6:
                    mcDBG_MSG_L1(("Code Rate : 5/6\n")); break;
                case SONYSAT_DVBS2_CODERATE_8_9:
                    mcDBG_MSG_L1(("Code Rate : 8/9\n")); break;
                case SONYSAT_DVBS2_CODERATE_9_10:
                    mcDBG_MSG_L1(("Code Rate : 9/10\n")); break;
                case SONYSAT_DVBS2_CODERATE_INVALID:
                default:
                    mcDBG_MSG_L1(("Code Rate : Unknown(%d)\n", plscode.codeRate)); break;
                }

                if(plscode.isShortFrame){
                    mcDBG_MSG_L1(("FEC Frame Size : Short\n"));
                }else{
                    mcDBG_MSG_L1(("FEC Frame Size : Normal\n"));
                }

                if(plscode.isPilotOn){
                    mcDBG_MSG_L1(("Pilot Mode : On\n"));
                }else{
                    mcDBG_MSG_L1(("Pilot Mode : Off\n"));
                }
            }else{
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PLSCode (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
        }

        if(pPdCtxDVBS->fgUniCab){
            mcDBG_MSG_L1(("Unicable mode\n"));
            switch(pPdCtxDVBS->t_UniCab_info.e_bank)
            {
            case BANK_POS_A_L_BAND_VERT_POL:
                mcDBG_MSG_L1((" Bank0 (PosA, LowBand,  Vertical)\n"));
                break;
            case BANK_POS_A_H_BAND_VERT_POL:
                mcDBG_MSG_L1((" Bank1 (PosA, HighBand, Vertical)\n"));
                break;
            case BANK_POS_A_L_BAND_HOR_POL:
                mcDBG_MSG_L1((" Bank2 (PosA, LowBand,  Horizontal)\n"));
                break;
            case BANK_POS_A_H_BAND_HOR_POL:
                mcDBG_MSG_L1((" Bank3 (PosA, HighBand, Vertical)\n"));
                break;
            case BANK_POS_B_L_BAND_VERT_POL:
                mcDBG_MSG_L1((" Bank0 (PosB, LowBand,  Vertical)\n"));
                break;
            case BANK_POS_B_H_BAND_VERT_POL:
                mcDBG_MSG_L1((" Bank1 (PosB, HighBand, Vertical)\n"));
                break;
            case BANK_POS_B_L_BAND_HOR_POL:
                mcDBG_MSG_L1((" Bank2 (PosB, LowBand,  Horizontal)\n"));
                break;
            case BANK_POS_B_H_BAND_HOR_POL:
                mcDBG_MSG_L1((" Bank3 (PosB, HighBand, Vertical)\n"));
                break;
            default:
                mcDBG_MSG_L1((" Error at Bank(%d)\n", pPdCtxDVBS->t_UniCab_info.e_bank));
                break;
            }
            switch(pPdCtxDVBS->t_UniCab_info.e_user_band)
            {
            case USER_BAND_1:
                mcDBG_MSG_L1((" UB1\n"));
                break;
            case USER_BAND_2:
                mcDBG_MSG_L1((" UB2\n"));
                break;
            case USER_BAND_3:
                mcDBG_MSG_L1((" UB3\n"));
                break;
            case USER_BAND_4:
                mcDBG_MSG_L1((" UB4\n"));
                break;
            case USER_BAND_5:
                mcDBG_MSG_L1((" UB5\n"));
                break;
            case USER_BAND_6:
                mcDBG_MSG_L1((" UB6\n"));
                break;
            case USER_BAND_7:
                mcDBG_MSG_L1((" UB7\n"));
                break;
            case USER_BAND_8:
                mcDBG_MSG_L1((" UB8\n"));
                break;
            default:
                mcDBG_MSG_L1((" Error at UB\n"));
                break;
            }
            mcDBG_MSG_L1((" UB frequency          : %4d MHz\n", pPdCtxDVBS->t_UniCab_info.ui2_band_freq));
            mcDBG_MSG_L1((" IF frequency          : %4d MHz\n", pPdCtxDVBS->t_UniCab_info.ui2_if_freq));
            mcDBG_MSG_L1((" LNB frequency         : %4d MHz\n", pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq));
            mcDBG_MSG_L1((" Transponder frequency : %4d MHz\n", pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq));
        } else {
            mcDBG_MSG_L1(("DiSEqC mode\n"));
        }
        mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
        break;
    /* Tuner Debug Command ----------------------------------------------------- */
    case 'a':
        {
            switch(aszArgv[0][1]){
            case 'd':
            default:
                {
                    UINT8 data[256];

                    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock); // Enter Critical Section
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sonysat_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x00, data, 256);
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock); // Leave Critical Section
                    if(sonysat_result != SONYSAT_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Register Dump Result. (Tuner)\n"));
                    mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
                    {
                        int i = 0;
                        int j = 0;
                        char dumpline[53];
                        char* ptr = dumpline;
                        const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                        dumpline[52] = '\0';
                        for(i=0; i<16; i++){
                            ptr = dumpline;
                            for(j=0; j<16; j++){
                                if(j == 0){
                                    *ptr++ = hexchar[(i*16) >> 4];
                                    *ptr++ = hexchar[(i*16) & 0x0F];
                                    *ptr++ = ' ';
                                    *ptr++ = '|';
                                }
                                *ptr++ = ' ';
                                *ptr++ = hexchar[data[(i*16)+j] >> 4];
                                *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                            }
                            mcDBG_MSG_L1(("%s\n", dumpline));
                        }
                    }
                    mcDBG_MSG_L1(("\n"));
                }
                break;
            case 'r':
                {
                    UINT8 data[256];
                    UINT8 subAddress = 0;
                    UINT32 length = 0;
                    UINT32 i = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No read register and length.\n"));
                        return;
                    }
                    subAddress = (UINT8)sonysat_strtoul(aszArgv[1]);
                    length = (UINT32)sonysat_strtoul(aszArgv[2]);
                    if(length > 256){
                        length = 256;
                    }
                    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sonysat_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, subAddress, data, length);
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
                    if(sonysat_result != SONYSAT_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    for(i=0; i<length; i++){
                        mcDBG_MSG_L1(("Tuner Register 0x%02X = 0x%02X\n", i + subAddress, data[i]));
                    }
                }
                break;
            case 'w':
                {
                    UINT8 subAddress = 0;
                    UINT8 value = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No write register and data.\n"));
                        return;
                    }
                    subAddress = (UINT8)sonysat_strtoul(aszArgv[1]);
                    value = (UINT8)sonysat_strtoul(aszArgv[2]);
                    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sonysat_result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, subAddress, value);
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
                    if(sonysat_result != SONYSAT_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Wrote Tuner Register 0x%02X = 0x%02X\n", subAddress, value));
                }
                break;
            }
            break;
        }
    /* Normal register access command -------------------------------------- */
    case 'd':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;

            if(i4Argc < 3){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and bank are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sonysat_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock); // Enter Critical Section
            sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
                return;
            }
            sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, 0x00, data, 256);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock); // Leave Critical Section
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Register Dump Result. (Slave:0x%02X, Bank:0x%02X)\n", i2cAddress, bank));
            mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
            {
                int i = 0;
                int j = 0;
                char dumpline[53];
                char* ptr = dumpline;
                const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                dumpline[52] = '\0';
                for(i=0; i<16; i++){
                    ptr = dumpline;
                    for(j=0; j<16; j++){
                        if(j == 0){
                            *ptr++ = hexchar[(i*16) >> 4];
                            *ptr++ = hexchar[(i*16) & 0x0F];
                            *ptr++ = ' ';
                            *ptr++ = '|';
                        }
                        *ptr++ = ' ';
                        *ptr++ = hexchar[data[(i*16)+j] >> 4];
                        *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                    }
                    mcDBG_MSG_L1(("%s\n", dumpline));
                }
            }
            mcDBG_MSG_L1(("\n"));
        }
        break;
    case 'r':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT32 length = 0;
            UINT32 i = 0;
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and subaddress, length are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sonysat_strtoul(aszArgv[2]);
            subAddress = (UINT8)sonysat_strtoul(aszArgv[3]);
            length = (UINT32)sonysat_strtoul(aszArgv[4]);
            if(length > 256){
                length = 256;
            }
            mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
            sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sonysat_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, subAddress, data, length);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            for(i=0; i<length; i++){
                mcDBG_MSG_L1(("Read Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, i + subAddress, data[i]));
            }
        }
        break;
    case 't':   //TS lock
        {
            switch(aszArgv[0][1])
            {
                case 's':
                default:
                {
                    UINT8 tsLock = 0;
                
                    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
                    if(sonysat_result == SONYSAT_RESULT_OK){
                        mcDBG_MSG_L1(("TS Lock : %d\n", tsLock));
                    }else{
                        mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_SyncStat (%s)\n", sonysat_FormatResult(sonysat_result)));
                    }
                }
            }
            break;
        }
        break;
    case 'w':
        {
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT8 value = 0;
            if(i4Argc < 4){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and subaddress, value are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sonysat_strtoul(aszArgv[2]);
            subAddress = (UINT8)sonysat_strtoul(aszArgv[3]);
            value = (UINT8)sonysat_strtoul(aszArgv[4]);
            mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
            sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sonysat_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, subAddress, value);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Wrote Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, subAddress, value));
        }
        break;
    // Blindscan command
    case 'b':
        switch(aszArgv[0][1]){
        case 's':
            {
                if(i4Argc < 3){
                    mcDBG_MSG_L1(("Error: start and stop frequency (MHz) are necessary.\n"));
                    return;
                }

                pDemodCtx->bs_start_freq = (UINT16)sonysat_strtoul(aszArgv[1]);
                pDemodCtx->bs_stop_freq = (UINT16)sonysat_strtoul(aszArgv[2]);

                if(pPdCtxDVBS->fgBreakMonitor)
                {
                    pPdCtxDVBS->fgBreakMonitor = FALSE;
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
                }
                pDemodCtx->do_blindscan = 1;
                mcDBG_MSG_L1(("Start blind scan.\n"));
            }
            break;
        case 'c':
            mcDBG_MSG_L1(("Continue to do blind scan...\n"));
            x_sema_unlock(pDemodCtx->AutoScan_callback);
            break;
        case 'e':
            {
                /* BER */
                sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &currentSystem);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS){
                        uint32_t value = 0;
                    
                        sonysat_result = sonysat_demod_dvbs_monitor_PreViterbiBER(pDemod, &value);
                        if(sonysat_result == SONYSAT_RESULT_OK){
                            mcDBG_MSG_L4(("DVBS PreViterbi BER (1e7) : %d\n", value));
                        }else{
                            mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_PreViterbiBER (%s)\n", sonysat_FormatResult(sonysat_result)));
                        }
                    
                        sonysat_result = sonysat_demod_dvbs_monitor_PreRSBER(pDemod, &value);
                        if(sonysat_result == SONYSAT_RESULT_OK){
                            mcDBG_MSG_L1(("DVBS PreRS BER (1e7) : %d\n", value));
                        }else{
                            mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_PreRSBER (%s)\n", sonysat_FormatResult(sonysat_result)));
                        }
                    }else if(currentSystem == SONYSAT_DTV_SYSTEM_DVBS2){
                        uint32_t value = 0;
                    
                        sonysat_result = sonysat_demod_dvbs2_monitor_PreLDPCBER(pDemod, &value);
                        if(sonysat_result == SONYSAT_RESULT_OK){
                            mcDBG_MSG_L4(("DVBS2 PreLDPC BER (1e7) : %d\n", value));
                        }else{
                            mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PreLDPCBER (%s)\n", sonysat_FormatResult(sonysat_result)));
                        }
                    
                        sonysat_result = sonysat_demod_dvbs2_monitor_PreBCHBER(pDemod, &value);
                        if(sonysat_result == SONYSAT_RESULT_OK){
                            mcDBG_MSG_L1(("DVBS2 PreBCH BER (1e9) : %d\n", value));
                        }else{
                            mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PreBCHBER (%s)\n", sonysat_FormatResult(sonysat_result)));
                        }
                    
                        sonysat_result = sonysat_demod_dvbs2_monitor_PostBCHFER(pDemod, &value);
                        if(sonysat_result == SONYSAT_RESULT_OK){
                            mcDBG_MSG_L4(("DVBS2 PostBCH FER (1e6) : %d\n", value));
                        }else{
                            mcDBG_MSG_L1(("Error in sonysat_demod_dvbs2_monitor_PostBCHFER (%s)\n", sonysat_FormatResult(sonysat_result)));
                        }
                    }
                }
            }
            break;
        case 'q':
            pPdCtxDVBS->fgBreakMonitor=TRUE;
            mcDBG_MSG_L1(("Cancel blind scan\n"));
            break;
        case 'l':
            {
                switch(aszArgv[1][0]){
                case 'p':
                    if (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM){
                        /* On -> Off */
                        g_sonysat_blindscan_log_flag &= ~SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM;
                    } else {
                        /* Off -> On */
                        g_sonysat_blindscan_log_flag |=  SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM;
                    }
                    break;
                case 'c':
                    if (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_CANDIDATE){
                        /* On -> Off */
                        g_sonysat_blindscan_log_flag &= ~SONYSAT_BLINDSCAN_LOG_CANDIDATE;
                    } else {
                        /* Off -> On */
                        g_sonysat_blindscan_log_flag |=  SONYSAT_BLINDSCAN_LOG_CANDIDATE;
                    }
                    break;
                default:
                    break;
                }
                mcDBG_MSG_L1(("BlindScan power spectrum log : %s\n", (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM) ? "On" : "Off"));
                mcDBG_MSG_L1(("BlindScan candidate      log : %s\n", (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_CANDIDATE)      ? "On" : "Off"));
            }
            break;
        case 'p':
            {
                int32_t count = 0;
                switch(aszArgv[1][0]){
                case '0':
                    sonysat_demod_SetConfig (&pDemodCtx->demod, SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_MODE,  0);
                    break;
                case '1':
                    count = StrToInt(aszArgv[2]);
                    switch (count) {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                    case 16:
                    case 32:
                    case 64:
                        sonysat_demod_SetConfig (&pDemodCtx->demod, SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_MODE, 1);
                        sonysat_demod_SetConfig (&pDemodCtx->demod, SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_COUNT, count);
                        break;
                    default:
                        mcDBG_MSG_L1(("Argument error\n"));
                        break;
                    }
                    break;
                default:
                    mcDBG_MSG_L1(("Argument error\n"));
                    break;
                }
            }
            break;
        case 'f':
            {
                pDemodCtx->tuner.symbolRateKSpsForSpectrum = (uint32_t)StrToInt(aszArgv[1]);
                mcDBG_MSG_L1(("Tuner LPF setting for PowerSpectrum is DVB-S %d KSps\n", pDemodCtx->tuner.symbolRateKSpsForSpectrum));
            }
            break;
        default:
            mcDBG_MSG_L1(("Unknown command.\n"));
            break;
        }
        break;
    case 'l':
        {
            switch(aszArgv[0][1])
            {
                case 'i':
                    {
                        sonysat_lnbc_t * pLNBC = &pDemodCtx->lnbc;
                        pLNBC->Initialize(pLNBC);
                        mcDBG_MSG_L1(("pLNBC->Initialize\n"));
                    }
                    break;
                case 'v':
                    {
                        DISEQC_BUS_CMD_T *pCmd = &pDemodCtx->diseqc_cmd;
                        pCmd->ui4_control_mask |= DISEQC_CMD_CTRL_LNB_POWER_MASK;
                        pCmd->ui4_control_mask |= DISEQC_CMD_CTRL_POLARIZATION_MASK;
                        switch(aszArgv[1][0])
                        {
                            case '0':   //off
                                pCmd->ui1_lnb_power = 0;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            case '1':   //13V
                                pCmd->ui1_lnb_power = 1;
                                pCmd->ui1_polarity_13v_18v = 0;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            case '2':   //14V
                                pCmd->ui1_lnb_power = 2;
                                pCmd->ui1_polarity_13v_18v = 0;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            case '3':   //18V
                                pCmd->ui1_lnb_power = 1;
                                pCmd->ui1_polarity_13v_18v = 1;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            case '4':   //19V
                                pCmd->ui1_lnb_power = 2;
                                pCmd->ui1_polarity_13v_18v = 1;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            default:
                                mcDBG_MSG_L1(("nim.dtd lv 0         Set LNBC Voltage = 0V\n"));
                                mcDBG_MSG_L1(("nim.dtd lv 1         Set LNBC Voltage = 13V\n"));
                                mcDBG_MSG_L1(("nim.dtd lv 2         Set LNBC Voltage = 14V\n"));
                                mcDBG_MSG_L1(("nim.dtd lv 3         Set LNBC Voltage = 18V\n"));
                                mcDBG_MSG_L1(("nim.dtd lv 4         Set LNBC Voltage = 19V\n"));
                                break;
                        }
                    }
                    break;
                case 't':
                    {
                        DISEQC_BUS_CMD_T *pCmd = &pDemodCtx->diseqc_cmd;
                        pCmd->ui4_control_mask |= DISEQC_CMD_CTRL_22K_MASK;
                        //pCmd->ui4_control_mask |= DISEQC_CMD_CTRL_LNB_POWER_MASK;
                        switch(aszArgv[1][0])
                        {
                            case '0':   //22k off
                                pCmd->b_22k = 0;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            case '1':   //22k on
                                pCmd->b_22k = 1;
                                sonysat_HandleDiseqcCommand(tunerID, pCmd);
                                break;
                            default:
                                mcDBG_MSG_L1(("nim.dtd lt 0         Set LNBC tone off\n"));
                                mcDBG_MSG_L1(("nim.dtd lt 1         Set LNBC tone 22k on\n"));
                                break;
                        }
                    }
                    break;
                case 's':
                    {
                        sonysat_lnbc_t * pLNBC = &pDemodCtx->lnbc;
                        if(pLNBC){
                            switch(pLNBC->state)
                            {
                                case SONYSAT_LNBC_STATE_SLEEP:
                                    mcDBG_MSG_L1(("LNBC is in SLEEP state\n"));
                                    break;
                                case SONYSAT_LNBC_STATE_ACTIVE:                                
                                    mcDBG_MSG_L1(("LNBC is in ACTIVE state\n"));
                                    break;
                                case SONYSAT_LNBC_STATE_OVERLOAD:
                                    mcDBG_MSG_L1(("LNBC is in OVERLOAD state, please take care!!!\n"));
                                    break;
                                case 0:
                                default:
                                    mcDBG_MSG_L1(("LNBC is in UNKNOWN state\n"));
                                    break;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        break;
    case 'u':
        {
            UINT32 Uec = pDemodCtx->u4Uec;
            switch(aszArgv[0][1])
            {
                case 'e':
                default:
                {
                    mcDBG_MSG_L1(("CXD2854[%d] : UEC = %lu\n", tunerID,Uec));
                }
                break;
                case 'c':
                {
                    pDemodCtx->u4Uec = 0;
                    mcDBG_MSG_L1(("CXD2854[%d] : Clean UEC\n", tunerID));
                }
                break;                    
            }
        }
        break;
    default:
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDetachI2C(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_DVBS
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_DVBS_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 DVBS_TunerGetAttribute(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    TUNER_SAT_DIG_TUNE_INFO_T *pAttri0;
    TUNER_DIG_ATTRIBUTE_T *pAttri1;
    TUNER_SAT_DIG_LNB_STATUS_INFO_T *pAttri2;
    INT32 i4Temp = 0;

    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t *pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T *pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t *pDemod = &pDemodCtx->demod;
    sonysat_lnbc_t *pLnbc = &pDemodCtx->lnbc;
    sonysat_dtv_system_t currentSystem; // DVB-S or DVB-S2
    uint8_t currentrolloff=0x00;
    sonysat_dvbs2_plscode_t plscode;
    sonysat_dvbs_coderate_t coderate;
    uint32_t symbolRateSps = 0;

    if(pzAttribute!=NULL)
    {
        pAttri0 = (TUNER_SAT_DIG_TUNE_INFO_T*)pzAttribute;
        pAttri2 = (TUNER_SAT_DIG_LNB_STATUS_INFO_T*)pzAttribute;
    }
    else
    {
        mcDBG_MSG_L1(("[DVBS_TunerGetAttribute] pzAttribute IS Err\n"));
        return DRVAPI_TUNER_OK;
    }

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    mcDBG_MSG_L4(("CXD2854[%d] : %s, e_get_type:%d\n", tunerID, __FUNCTION__, e_get_type));
    //add driver implementation code here
    switch (e_get_type)
    {
    case TUNER_GET_TYPE_SCAN_PROGRESS:
        {
            UINT8 u1ScanProg = pDemodCtx->blindscanProgress;//Please implement scan progres information
            *pzAttributeLen = sizeof(UINT8);
            x_memcpy(pzAttribute, &u1ScanProg, *pzAttributeLen);
            mcDBG_MSG_L2(("Scan progress is %d\n",u1ScanProg));
        }
        break;
    case TUNER_GET_TYPE_SIGNAL_LEVEL:
        {
            uint32_t ifagcOut = 0;
            //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //signal level.update it as i4Temp
            sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifagcOut);
            if(sonysat_result == SONYSAT_RESULT_OK){
                i4Temp = (INT32)(ifagcOut * 100 / 8191);
            }else{
                i4Temp = 0;
            }
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
     case TUNER_GET_TYPE_AGC:
         {
             uint32_t ifagcOut = 0;
             //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL\r\n"));
             *pzAttributeLen = sizeof(INT32);
             //signal level.update it as i4Temp
             sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifagcOut);
             if(sonysat_result == SONYSAT_RESULT_OK){
                 i4Temp=255-(ifagcOut>>5);
             }else{
                 i4Temp = 0;
             }
             mcDBG_MSG_L2(("IF AGC is %d\n",i4Temp));
             x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
         }
        break;
    case TUNER_GET_TYPE_KHZ_CFO:
        {
            int32_t offset = 0;
            sonysat_result = sonysat_demod_dvbs_s2_monitor_CarrierOffset(pDemod, &offset);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L2(("Carrier Offset (kHz) : %d\n", offset));
            }else{
                mcDBG_MSG_L2(("Error in sonysat_demod_dvbs_s2_monitor_CarrierOffset (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
            *pzAttributeLen = sizeof(INT32);
            x_memcpy(pzAttribute, &offset, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SAT_DIG:
        {   /*maybe return Last_program_channel*/
                //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_SAT_DIG:freq:%d\r\n",Current_channel.m_uiFrequency_kHz));
                //update channel information.
            sonysat_result = sonysat_demod_dvbs_s2_monitor_System(pDemod, &currentSystem);
            if(sonysat_result == SONYSAT_RESULT_OK)
            {
                switch(currentSystem)
                {
                    case SONYSAT_DTV_SYSTEM_DVBS:
                    {
                        pAttri0->e_dvb_s_s2_mode=TUNER_DVB_S;
                        mcDBG_MSG_L2(("System : DVB-S\n"));
                        break;
                    }
                    case SONYSAT_DTV_SYSTEM_DVBS2:
                    {
                        pAttri0->e_dvb_s_s2_mode=TUNER_DVB_S2;
                        mcDBG_MSG_L2(("System : DVB-S2\n"));
                        break;
                    }
                    default:
                    {
                        pAttri0->e_dvb_s_s2_mode=TUNER_DVB_AUTO_S_S2;
                        mcDBG_MSG_L2(("System : Unknown\n"));
                        break;
                    }
                }
            }
            else
            {
                mcDBG_MSG_L2(("Error in sonysat_demod_dvbs_s2_monitor_System : Unknown\n"));
                pAttri0->e_dvb_s_s2_mode=TUNER_DVB_AUTO_S_S2;
            }
            if  (currentSystem==SONYSAT_DTV_SYSTEM_DVBS2)
            {
                sonysat_result =sonysat_demod_dvbs2_monitor_Rolloff(pDemod,&currentrolloff);
                if(sonysat_result == SONYSAT_RESULT_OK){
                   switch(currentrolloff)
                   {
                      case 0:
                        pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_35;
                        mcDBG_MSG_L2(("S2 Roll off parameter is 0.35\n"));break;
                      case 1:
                        pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_25;
                        mcDBG_MSG_L2(("S2 Roll off parameter is 0.25\n"));break;
                      case 2:
                        pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_20;
                        mcDBG_MSG_L2(("S2 Roll off parameter is 0.20\n"));break;
                      case 3:
                        pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_RESERVE;
                        mcDBG_MSG_L2(("S2 Roll off parameter is Reserve\n"));break;
                      default:
                        break;
                   }
                }
                sonysat_result = sonysat_demod_dvbs2_monitor_PLSCode(pDemod, &plscode);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    switch(plscode.modulation){
                       case SONYSAT_DVBS2_MODULATION_QPSK:
                           pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_QPSK;
                           mcDBG_MSG_L2(("Modulation : QPSK\n")); break;
                       case SONYSAT_DVBS2_MODULATION_8PSK:
                           pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_8PSK;
                           mcDBG_MSG_L2(("Modulation : 8PSK\n")); break;
                       case SONYSAT_DVBS2_MODULATION_16APSK:
                           pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_16APSK;
                           mcDBG_MSG_L2(("Modulation : 16APSK\n")); break;
                       case SONYSAT_DVBS2_MODULATION_32APSK:
                           pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_32APSK;
                           mcDBG_MSG_L2(("Modulation : 32APSK\n")); break;
                       case SONYSAT_DVBS_CODERATE_INVALID:
                       default:
                           pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_INVALID;
                           mcDBG_MSG_L2(("Modulation : Unknown(%d)\n", plscode.modulation)); break;
                    }

                    switch(plscode.codeRate){
                       case SONYSAT_DVBS2_CODERATE_1_4:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_4;
                           mcDBG_MSG_L2(("Code Rate : 1/4\n")); break;
                       case SONYSAT_DVBS2_CODERATE_1_3:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_3;
                           mcDBG_MSG_L2(("Code Rate : 1/3\n")); break;
                       case SONYSAT_DVBS2_CODERATE_2_5:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_5;
                           mcDBG_MSG_L2(("Code Rate : 2/5\n")); break;
                       case SONYSAT_DVBS2_CODERATE_1_2:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_2;
                           mcDBG_MSG_L2(("Code Rate : 1/2\n")); break;
                       case SONYSAT_DVBS2_CODERATE_3_5:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_5;
                           mcDBG_MSG_L2(("Code Rate : 3/5\n")); break;
                       case SONYSAT_DVBS2_CODERATE_2_3:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_3;
                           mcDBG_MSG_L2(("Code Rate : 2/3\n")); break;
                       case SONYSAT_DVBS2_CODERATE_3_4:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_4;
                           mcDBG_MSG_L2(("Code Rate : 3/4\n")); break;
                       case SONYSAT_DVBS2_CODERATE_4_5:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_4_5;
                           mcDBG_MSG_L2(("Code Rate : 4/5\n")); break;
                       case SONYSAT_DVBS2_CODERATE_5_6:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_5_6;
                           mcDBG_MSG_L2(("Code Rate : 5/6\n")); break;
                       case SONYSAT_DVBS2_CODERATE_8_9:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_8_9;
                           mcDBG_MSG_L2(("Code Rate : 8/9\n")); break;
                       case SONYSAT_DVBS2_CODERATE_9_10:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_9_10;
                           mcDBG_MSG_L2(("Code Rate : 9/10\n")); break;
                       case SONYSAT_DVBS2_CODERATE_INVALID:
                       default:
                           pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
                           mcDBG_MSG_L2(("Code Rate : Unknown(%d)\n", plscode.codeRate)); break;
                    }
                    if(plscode.isPilotOn){
                           pAttri0->b_pilot_insert=1;
                           mcDBG_MSG_L2(("Pilot Mode : On\n"));
                    }else{
                            pAttri0->b_pilot_insert=0;
                           mcDBG_MSG_L2(("Pilot Mode : Off\n"));
                   }
               }
            }
            else if(currentSystem==SONYSAT_DTV_SYSTEM_DVBS)
            {
                pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_QPSK;
                pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_35;
                pAttri0->b_pilot_insert=0;
                sonysat_result = sonysat_demod_dvbs_monitor_CodeRate(pDemod, &coderate);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    switch(coderate){
                    case SONYSAT_DVBS_CODERATE_1_2:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_2;
                        mcDBG_MSG_L2(("Code Rate : 1/2\n")); break;
                    case SONYSAT_DVBS_CODERATE_2_3:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_3;
                        mcDBG_MSG_L2(("Code Rate : 2/3\n")); break;
                    case SONYSAT_DVBS_CODERATE_3_4:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_4;
                        mcDBG_MSG_L2(("Code Rate : 3/4\n")); break;
                    case SONYSAT_DVBS_CODERATE_5_6:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_5_6;
                        mcDBG_MSG_L2(("Code Rate : 5/6\n")); break;
                    case SONYSAT_DVBS_CODERATE_7_8:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_7_8;
                        mcDBG_MSG_L2(("Code Rate : 7/8\n")); break;
                    case SONYSAT_DVBS_CODERATE_INVALID:
                    default:
                        pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
                        mcDBG_MSG_L2(("Code Rate : Unknown(%d)\n", coderate)); break;
                    }
                }
                else{
                    mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_monitor_CodeRate (%s)\n", sonysat_FormatResult(sonysat_result)));
                }
            }
            else
            {
                 pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_RESERVE;
                 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
                 pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_INVALID;
                 pAttri0->b_pilot_insert=0;
                 mcDBG_MSG_L2(("not DVBS & DVBS2 system\n"));
            }
            pAttri0->e_fec_inner = FEC_I_UNKNOWN;
            pAttri0->ui4_freq = (pDemodCtx->tunedSignal.centerFreqKHz + 500) / 1000;
            pAttri0->ui4_sym_rate = pDemodCtx->tunedSignal.symbolRateKSps;
            sonysat_result = sonysat_demod_dvbs_s2_monitor_SymbolRate(pDemod, &symbolRateSps);
            if(sonysat_result == SONYSAT_RESULT_OK)
            {
                 pAttri0->u14_sym_rate_in_Sps = symbolRateSps;
            }
            else
            {
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_SymbolRate (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
            if(pDemodCtx->channel_locked_flag == 1)
            {
                pAttri0->ui4_data = pDemodCtx->ui4_data_forMW;
            }
            else
            {
                pAttri0->ui4_data = 0;
            }
            pAttri0->e_pol = pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH ? POL_LIN_HORIZONTAL : POL_LIN_VERTICAL;
            mcDBG_MSG_L1(("[DVBS_TunerGetAttribute]  ui4_freq:%d KHz SymbolRate=%dS/s !\n",pAttri0->ui4_freq,pAttri0->u14_sym_rate_in_Sps));
       //   mcDBG_MSG_L1(("[DVBS_TunerGetAttribute]  ui4_freq:%d KHz SymbolRate=%dS/s !\n",pAttri0->ui4_freq,pAttri0->ui4_sym_rate));
        }
        break;
    case TUNER_GET_TYPE_BER:
        {
            mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //TODO Get BER from demod driver.Note the unit is 1e-5.update it as i4Temp
            {
                UINT32 ber = sonysat_GetBER(tunerID);
                i4Temp = (INT32)ber;
            }
            mcDBG_MSG_L2(("BER is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PRE_VBER:
        {
            mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_PRE_VBER\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //TODO Get BER from demod driver.Note the unit is 1e-5.update it as i4Temp
            {
                UINT32 ber = sonysat_GetPreBER(tunerID);
                i4Temp = (INT32)ber;
            }
            mcDBG_MSG_L2(("BER is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SSI:
        {
            mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SSI\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //signal level.update it as i4Temp
            i4Temp = sonysat_GetSSI(tunerID);
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    #if 0  //bo 20140313
    case TUNER_GET_TYPE_RSSI:
        {
            uint32_t ifagcOut = 0;
            int32_t agcLevel = 0;
            INT8 i1Temp = 0;
            //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_RSSI\r\n"));
            *pzAttributeLen = sizeof(INT8);
            //signal level.update it as i4Temp
            sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifagcOut);
            if(sonysat_result == SONYSAT_RESULT_OK){
                sonysat_result = pTuner->AGCLevel2AGCdB (pTuner, ifagcOut, &agcLevel);
                /* agcLevel value is dB x 100. */
                i1Temp = (INT8)(((agcLevel + 50) / 100) * (-1));
            }else{
                i1Temp = 0;
            }
            mcDBG_MSG_L1(("RSSI is %d\n", i1Temp));
            x_memcpy(pzAttribute, &i1Temp, *pzAttributeLen);
        }
        break;
    #endif
    case TUNER_GET_TYPE_SQI:
        {
            mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SQI\r\n"));
            *pzAttributeLen = sizeof(INT32);
            {
                // NOTE: This is tentative implementation by SONY.
                uint8_t quality = 0;
                sonysat_result = s_s2_monitor_Quality(pDemod, &quality);
                if(sonysat_result != SONYSAT_RESULT_OK){
                    // Error
                    quality = 0;
                }

                i4Temp = (INT32)quality;
            }
            mcDBG_MSG_L2(("SQI is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_DBM_SNR:
        {
            int32_t cnr = 0;
            *pzAttributeLen = sizeof(INT32);
            sonysat_result = sonysat_demod_dvbs_s2_monitor_CNR(pDemod, &cnr);
            if(sonysat_result == SONYSAT_RESULT_OK){
                i4Temp = cnr/1000U;
            }else{
                i4Temp =0;
            }
            mcDBG_MSG_L2(("CNR is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_LNB_STATUS:
        {
            UINT8 ocp = 0;
            UINT8 tsd = 0;
            UINT8 u1OverLoad=0;
            sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx[tunerID].lnbc;

            if(NULL!=pLnbc->MonitorOverload)
            {
               if(SONYSAT_RESULT_OK!=pLnbc->MonitorOverload(pLnbc,&ocp))
               {
                u1OverLoad=0;//default value
               }
            }
            if(NULL!=pLnbc->MonitorThermalShutdown)
            {
               if(SONYSAT_RESULT_OK!=pLnbc->MonitorThermalShutdown(pLnbc,&tsd))
               {
                u1OverLoad=0;//default value
               }
            }
            /* bit1 : TSD, bit0 : OCP */
            u1OverLoad = ((tsd << 1) & 0x02) | (ocp & 0x01);
            pAttri2->b_short_status=u1OverLoad;
            pAttri2->ui1_lnb_power = pDemodCtx->diseqc_cmd.ui1_lnb_power;
            pAttri2->ui1_polarity_13v_18v = pDemodCtx->diseqc_cmd.ui1_polarity_13v_18v;
            pAttri2->b_22k = pDemodCtx->diseqc_cmd.b_22k;

            mcDBG_MSG_L2(("LNB overload is %d,LNB power is=%d ,power polarity is %d,22K tone is %d\n",pAttri2->b_short_status,pAttri2->ui1_lnb_power,pAttri2->ui1_polarity_13v_18v,pAttri2->b_22k));
        }
        break;
	case TUNER_GET_TYPE_LNB_SHORT_STATUS:
    	 {
    	  	UINT8 u1OverLoad=0;
    	  	u1OverLoad = sonysat_GetLnbShort(tunerID);
    	  	pAttri2->b_short_status=u1OverLoad;
    	  	mcDBG_MSG_L2(("Monitor LNB overload is %d\n",pAttri2->b_short_status));
    	 }
    	  break;
    case TUNER_GET_TYPE_UEC:
        {
            /*
            UINT16 UEC = 0;
            mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_UEC\r\n"));
            *pzAttributeLen = sizeof(UINT16);
            UEC = sonysat_GetUEC (tunerID);
            mcDBG_MSG_L1(("UEC is %d\n", UEC));
            x_memcpy(pzAttribute, &UEC, *pzAttributeLen);
            */
            mcDBG_MSG_L2(("e_get_type =TUNER_GET_TYPE_UEC\r\n"));
            *pzAttributeLen = sizeof(UINT32);
            x_memcpy(pzAttribute, &pDemodCtx->u4Uec, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        {
            //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
            pAttri1 = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
            //follow is an example,pls apply you code
            //Tuner frequency boundary
            pAttri1->ui4_lower_bound_freq = pDemodCtx->bs_start_freq;   // 950MHz
            pAttri1->ui4_upper_bound_freq = pDemodCtx->bs_stop_freq;    //2150MHz
            // freq offset tolerence range in Hz (1M mapping to 100K)
            pAttri1->ui4_fine_tune_delta_freq = 3;  //3//
            pAttri1->b_drv_notify_progress_capability = TRUE;//To indicate if driver can support scan progress notify
            mcDBG_MSG_L2(("b_drv_notify_progress_capability=%d\n", pAttri1->b_drv_notify_progress_capability));
        }
        break;
    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T tsFmt;
            if(pDemod != NULL)
            {
                *pzAttributeLen = sizeof(TS_FMT_T);
                sonysat_GetTsFmt(pDemod, &tsFmt);
                x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);                
            }
            else
            {
                mcDBG_MSG_L4(("Err: TUNER_GET_TYPE_TS_FMT fail, Null pointer\n"));
            }
        }
        break;
    case TUNER_GET_TYPE_BER_UPPER:
        {
            UINT16 berUpper;
            UINT32 ber=sonysat_GetBER(tunerID);
            berUpper = (UINT16)((ber>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_BER_LOWER:
        {
            UINT16 berLower;
            UINT32 ber=sonysat_GetBER(tunerID);
            berLower = (UINT16)(ber&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berLower, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_UPPER:
        {
            UINT16 perUpper;
            UINT32 per=sonysat_GetPER(tunerID);
            perUpper = (UINT16)((per>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_LOWER:
        {
            UINT16 perLower;
            UINT32 per=sonysat_GetPER(tunerID);
            perLower = (UINT16)(per&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perLower, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_TS_LOCK_STATUS:
        {
            UINT8 tsLock = 0;
            sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
            if(sonysat_result == SONYSAT_RESULT_OK){
                mcDBG_MSG_L2(("TS Lock : %d\n", tsLock));
            }else{
                mcDBG_MSG_L2(("Error in sonysat_demod_dvbs_s2_monitor_SyncStat (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
            *pzAttributeLen = sizeof(UINT8);
            x_memcpy(pzAttribute, &tsLock, *pzAttributeLen);
        }
        break;
    default:
        mcDBG_MSG_L4(("unknown: get Attribute %d\r \n",e_get_type));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return DRVAPI_TUNER_OK;
}

//-------------------------------------------------------------------------
/**
 *  This API set attribute to tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_set_type         tuner set type, check x_tuner.h set operation TUNER_SET_TYPE_DVBSX
 *  @param   *pvAttribute       set value.
 *  @param   zAttributeLen      length of *pvAttribute.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT32 DVBS_TunerSetAttribute(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    UCHAR   ucData=0;// = *(UCHAR *)pvAttribute;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
    sonysat_demod_t *pDemod = &pDemodCtx->demod;

    if(pvAttribute != NULL)
    {
        ucData = *(UCHAR *)pvAttribute;
    }
    else if( e_set_type == TUNER_SET_TYPE_SCAN_CONTINUE)
    {
        mcDBG_MSG_L1(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
        x_sema_unlock(pDemodCtx->AutoScan_callback);
        return DRVAPI_TUNER_OK;
    }
    else
    {
        mcDBG_MSG_L1(("[DVBS_TunerSetAttribute] pvAttribute is NULL & set type can not support...\n"));
        return DRVAPI_TUNER_OK;
    }
    
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    //add driver implementation code here

    switch (e_set_type)
    {
    /*
    case TUNER_SET_TYPE_SCAN_CONTINUE:
        {
            mcDBG_MSG_L1(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
            x_sema_unlock(pDemodCtx->AutoScan_callback);
        }
        break;
    */
    
    #ifdef CC_DEMOD_NON_DVBS_LNBC_OUTPUT_0V
    case TUNER_SET_TYPE_LNBC_SET_SLEEP:
        {
            sonysat_lnbc_t *pLnbc = &pDemodCtx->lnbc;
            if((pLnbc)&&(pLnbc->Sleep)){
                if(ucData == 0){
                    pLnbc->Sleep(pLnbc);
                    mcDBG_MSG_L1(("[DVBS_TunerSetAttribute][%d] ......set lnbc sleep......\n",tunerID));
                }
            }
        }
        break;
    #endif
    case TUNER_SET_TYPE_DISEQC_CMD:
        {
            DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pvAttribute;
            pDemodCtx->set_flag = 1;
            pDemodCtx->channel_locked_flag = 1;//Reset the lock flag to TRUE.This is very important.
            x_memcpy(&pDemodCtx->diseqc_cmd, pCmd, sizeof(DISEQC_BUS_CMD_T));//availink :add for desiqc control ,20110210 lmh
            sonysat_HandleDiseqcCommand(tunerID, pCmd);
        }
        break;
    case TUNER_SET_TYPE_RESET_CMD:
        {
            DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pvAttribute;
            pDemodCtx->set_flag = 1;
            pDemodCtx->channel_locked_flag = 1;//Reset the lock flag to TRUE.This is very important.
            sonysat_HandleResetCommand(tunerID, pCmd);
        }
        break;
    case TUNER_SET_TYPE_DEMOD_CLOCK:
        {
            UINT8 data = 0;
            if(ucData == 0){
                data = 0;
                mcDBG_MSG_L1(("[DVBS] Disable DVBS Demod clock\n"));
            }else {
                data = 1;
                mcDBG_MSG_L1(("[DVBS] Enable DVBS Demod clock\n"));
            }
            DVBS_TunerDemodClock(tunerID, data);
        }
        break;
    case TUNER_SET_TYPE_TS_TRISTATE:
        {
            UINT8 data = 0;
            if(ucData == BOOL_FALSE){
                mcDBG_MSG_L1(("[DVBS] Enable TS output\n"));
                // Enable TS output
                data = 0;
            }else if (ucData == BOOL_TRUE){
                mcDBG_MSG_L1(("[DVBS] Disable TS output\n"));
                // Disable TS output
                data = 1;
            }else{
                break;
            }
            // TS Output Enable (Bank:00h, Addr:C3h, Bit[0])
            // NOTE: This register is automatically controlled by the CXD2854DVBSS2 driver.
            //       Normally, this control is unnecessary from MTK upper layer.
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, data);
        }
        break;
    case TUNER_SET_TYPE_TS_OUTPUT:
        {
            UINT8 data = 0;
            if(ucData == BOOL_TRUE){
                mcDBG_MSG_L1(("[DVBS] Enable TS output\n"));
                // Enable TS output
                data = 0;
            }else if (ucData == BOOL_FALSE){
                mcDBG_MSG_L1(("[DVBS] Disable TS output\n"));
                // Disable TS output
                data = 1;
            }else{
                break;
            }
            // TS Output Enable (Bank:00h, Addr:C3h, Bit[0])
            // NOTE: This register is automatically controlled by the CXD2854DVBSS2 driver.
            // Normally, this control is unnecessary from MTK upper layer.
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, data);
        }
        break;

    case TUNER_SET_TYPE_TS_SP:
        /*
        if (ucData == SP_PARALLEL)
        {//apply you code
            mcDBG_MSG_L1(("[DVBS] SetTSInterface : parallel\n"));
        }
        else if (ucData == SP_SERIAL)
        {//apply you code
            mcDBG_MSG_L1(("[DVBS] SetTSInterface : serial\n"));
        }
        */
        mcDBG_MSG_L1(("e_set_type = TUNER_SET_TYPE_TS_SP : Not supported\r\n"));
        break;

    case TUNER_SET_TYPE_DEMOD_SHUTDOWN:
        {
            // Confirm that now this driver is disconnected state.
            if(pPdCtxDVBS->fgDisStatus != TRUE){
                mcDBG_MSG_L1(("CXD2854[%d] : ERROR!! SetAttribute(TUNER_SET_TYPE_DEMOD_SHUTDOWN) called but not disconnected!!\n", tunerID));
                break;
            }

            // Go shutdown state
            if(sonysat_demod_ForceShutdown(pDemod) != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("CXD2854[%d] : sonysat_demod_ForceShutdown failed!\n", tunerID));
                break;
            }

#if defined(CC_PROJECT_FOR_SONY) || defined(SONYSAT_TUNERMODULE_HELENE)
            // REFOUT disable
            HELENE_Satellite_SetRefOut(tunerID, 0);
#endif

            mcDBG_MSG_L2(("CXD2854[%d] : Demod is in Shutdown state.\n", tunerID));
        }
        break;

    default:
        mcDBG_MSG_L1(("unknown: set Attribute\r\n"));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return DRVAPI_TUNER_OK;
}

#ifdef fcADD_DIAG_INFO
static INT32 DVBS_TunerSetDiagCmd(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    INT32   retSts = RMR_OK;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

#ifdef CC_PROJECT_FOR_SONY
    if(pv_set_info == NULL)
    {
        mcDBG_MSG_L1(("[DVBS_TunerSetDiagCmd] pvAttribute is NULL & set type can not support...\n"));
        return DRVAPI_TUNER_OK;
    }

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    switch (e_tuner_set_type)
    {
        case DRV_CUSTOM_TUNER_SET_TYPE_DEMOD_PARAMETER:
        {
            DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_set_info;
            /*Set TS Max Freq*/
            sonysat_set_ts_maxfreq(tunerID, pParameter);

            #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
            /*Non-CanalDigital change demod setting*/
            sonysat_set_eeprom_non_canaldigital(tunerID, pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
            sonysat_set_flag_non_canaldigital(tunerID, 0xAA); //0xAA means EEPROM has been re-wrote
            sonysat_set_demod_non_canaldigital(tunerID, pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
            b_already_read_eep = 0;
            DVB_DEF2 = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[7];
            #endif
            
            /*Other*/
        }
        break;
        case DRV_CUSTOM_TUNER_SET_TYPE_LNB_OVERLOAD_RESET:
        {
            #ifdef CC_DEMOD_DVBS_LNB_SHORT_MONITOR
            DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_set_info;
            sonysat_lnbc_t *pLnbc = &pDemodCtx->lnbc;
            /*Reset LNB short status*/
            if(pParameter->t_reset_lnbshort.b_setvalid == TRUE)
            {
                mcDBG_MSG_L1(("[DVBS] Set LNB short reset!\r\n"));
                if(pLnbc->state == SONYSAT_LNBC_STATE_OVERLOAD)
                {
                    mcDBG_MSG_L1(("LNB overload reset!\r\n"));
                    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;
                    Vendor_ResendDiseqcCmmd(tunerID, &pDemodCtx->diseqc_cmd);
                }
            }
            #endif
            /*Other*/
        }
        break;
        case DRV_CUSTOM_TUNER_SET_TYPE_RESET_DISEQC_CMD:
        {
            DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pv_set_info;            
            pDemodCtx->set_flag = 1;
            pDemodCtx->channel_locked_flag = 1;
            sonysat_HandleResetCommand(tunerID, pCmd);
        }
        break;        

        default:
            mcDBG_MSG_L1(("unknown: set type = %d\r\n", e_tuner_set_type));
        break;
    }
#else
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used
#endif /* CC_PROJECT_FOR_SONY */

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return retSts;
}
static INT32 DVBS_TunerGetDiagInfo(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;
#ifdef CC_PROJECT_FOR_SONY
    if(pv_get_info == NULL)
    {
        mcDBG_MSG_L1(("[DVBS_TunerGetDiagInfo] pvAttribute is NULL & set type can not support...\n"));
        return DRVAPI_TUNER_OK;
    }
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    switch (e_tuner_get_type) {
    case DRV_CUSTOM_TUNER_GET_TYPE_DEMOD_PARAMETER:
        {
            #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
            DRV_CUSTOM_DEMOD_PARAMETER_T   *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_get_info; 
            sonysat_get_eeprom_non_canaldigital(tunerID, pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
            #endif
        }
        break;
    case DRV_CUSTOM_TUNER_GET_TYPE_LNB_OVERLOAD:
        {
            DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_get_info;
            x_memset(pParameter, 0, sizeof(DRV_CUSTOM_DEMOD_PARAMETER_T));
            pParameter->t_reset_lnbshort.ui1_overload = sonysat_GetLnbShort(tunerID);
        }
        break;
    default:
        mcDBG_MSG_L1(("unknown: get type = %d\r\n",e_tuner_get_type));
        break;
    }
#else
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used
#endif /* CC_PROJECT_FOR_SONY */

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return 0;
}
#endif
//-------------------------------------------------------------------------
/**
*  This API  control  the i2c bypass to tuner. When zap from digital to ananlog,we need this api to bypass i2c to tuner.
* Inputs:
* @param   ptTDSpecificCtx  Pointer to tuner driver context, there is no use for third party tuner.
* @param    bSwitchOn        TRUE(bypass i2c to tuner on).
                             FALSE(bypass i2c to tuner off)
* @Retval                    void
*/
//-------------------------------------------------------------------------
static void DVBS_TunerBypassI2C(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    DVBS_PD_CTX_T * pPdCtxDVBS = pDemodCtx->pPdCtxDVBS;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO  Bypass i2c to tuner connected(bSwitchOn is TRUE) or disconnect(bSwitchOn is FALSE)

    sonysat_demod_I2cRepeaterEnable(pDemod, bSwitchOn ? 0x01 : 0x00);

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static BOOL DVBS_TunerNotifyEnable(UINT8 tunerID, PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    BOOL TunerNotifyEnable;
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];

    if(pDemodCtx->do_blindscan == 1){
        TunerNotifyEnable = FALSE;
    } else {
        TunerNotifyEnable = TRUE;
    }

    return TunerNotifyEnable;
}

//-----------------------------------------------------------------------------
// APIs for making function table
//-----------------------------------------------------------------------------
// Tuner #0
static INT32 DVBS_TunerClose_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerClose(0, ptTDSpecificCtx);
}

static BOOL DVBS_TunerAcq_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    return DVBS_TunerAcq(0, ptTDSpecificCtx, e_conn_type, pv_conn_info, z_conn_info_len, _BreakFct, pvArg);
}

static BOOL DVBS_TunerGetSync_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSync(0, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerDisc_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    DVBS_TunerDisc(0, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerGetSignal_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    DVBS_TunerGetSignal(0, ptTDSpecificCtx, e_conn_type, _pSignal);
}

static UINT8 DVBS_TunerGetSignalLevel_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalLevel(0, ptTDSpecificCtx, e_conn_type);
}

static INT16 DVBS_TunerGetSignalLeveldBm_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalLeveldBm(0, ptTDSpecificCtx, e_conn_type);
}

static UINT32 DVBS_TunerGetSignalPER_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalPER(0, ptTDSpecificCtx, e_conn_type);
}

static UINT16 DVBS_TunerGetSignalSNR_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalSNR(0, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerSetMpgFmt_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    DVBS_TunerSetMpgFmt(0, ptTDSpecificCtx, pt_mpeg_fmt);
}

static char *DVBS_TunerGetVer_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerGetVer(0, ptTDSpecificCtx);
}

static INT32 DVBS_TunerNimTest_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerNimTest(0, ptTDSpecificCtx);
}

static void DVBS_TunerSetRegSetting_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    DVBS_TunerSetRegSetting(0, ptTDSpecificCtx, ucRegSet, ucRegAddr, ucRegValue);
}

static void DVBS_TunerShowRegSetting_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    DVBS_TunerShowRegSetting(0, ptTDSpecificCtx);
}

static void DVBS_TunerTestI2C_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    DVBS_TunerTestI2C(0, ptTDSpecificCtx);
}

static void DVBS_TunerCommand_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    DVBS_TunerCommand(0, ptTDSpecificCtx, i4Argc, aszArgv);
}

static void DVBS_TunerDtdCommand_0(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    DVBS_TunerDtdCommand(0, ptTDSpecificCtx, i4Argc, aszArgv);
}

static void DVBS_TunerDetachI2C_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    DVBS_TunerDetachI2C(0, ptTDSpecificCtx, bDetach);
}

static INT16 DVBS_TunerGetAttribute_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    return DVBS_TunerGetAttribute(0, ptTDSpecificCtx, e_get_type, pzAttribute, pzAttributeLen);
}

static INT32 DVBS_TunerSetAttribute_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    return DVBS_TunerSetAttribute(0, ptTDSpecificCtx, e_set_type, pvAttribute, zAttributeLen);
}

#ifdef fcADD_DIAG_INFO
static INT32 DVBS_TunerSetDiagCmd_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    return DVBS_TunerSetDiagCmd(0, ptTDSpecificCtx, e_tuner_set_type, pv_set_info, z_size);
}

static INT32 DVBS_TunerGetDiagInfo_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    return DVBS_TunerGetDiagInfo(0, ptTDSpecificCtx, e_tuner_get_type, pv_get_info, pz_size);
}
#endif

static void DVBS_TunerBypassI2C_0(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    DVBS_TunerBypassI2C(0, ptTDSpecificCtx, bSwitchOn);
}

static BOOL DVBS_TunerNotifyEnable_0(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerNotifyEnable(0, ptTDSpecificCtx);
}

// Tuner #1
static INT32 DVBS_TunerClose_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerClose(1, ptTDSpecificCtx);
}

static BOOL DVBS_TunerAcq_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    return DVBS_TunerAcq(1, ptTDSpecificCtx, e_conn_type, pv_conn_info, z_conn_info_len, _BreakFct, pvArg);
}

static BOOL DVBS_TunerGetSync_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSync(1, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerDisc_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    DVBS_TunerDisc(1, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerGetSignal_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    DVBS_TunerGetSignal(1, ptTDSpecificCtx, e_conn_type, _pSignal);
}

static UINT8 DVBS_TunerGetSignalLevel_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalLevel(1, ptTDSpecificCtx, e_conn_type);
}

static INT16 DVBS_TunerGetSignalLeveldBm_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalLeveldBm(1, ptTDSpecificCtx, e_conn_type);
}

static UINT32 DVBS_TunerGetSignalPER_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalPER(1, ptTDSpecificCtx, e_conn_type);
}

static UINT16 DVBS_TunerGetSignalSNR_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    return DVBS_TunerGetSignalSNR(1, ptTDSpecificCtx, e_conn_type);
}

static void DVBS_TunerSetMpgFmt_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    DVBS_TunerSetMpgFmt(1, ptTDSpecificCtx, pt_mpeg_fmt);
}

static char *DVBS_TunerGetVer_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerGetVer(1, ptTDSpecificCtx);
}

static INT32 DVBS_TunerNimTest_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerNimTest(1, ptTDSpecificCtx);
}

static void DVBS_TunerSetRegSetting_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    DVBS_TunerSetRegSetting(1, ptTDSpecificCtx, ucRegSet, ucRegAddr, ucRegValue);
}

static void DVBS_TunerShowRegSetting_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    DVBS_TunerShowRegSetting(0, ptTDSpecificCtx);
}

static void DVBS_TunerTestI2C_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    DVBS_TunerTestI2C(1, ptTDSpecificCtx);
}

static void DVBS_TunerCommand_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    DVBS_TunerCommand(1, ptTDSpecificCtx, i4Argc, aszArgv);
}

static void DVBS_TunerDtdCommand_1(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    DVBS_TunerDtdCommand(1, ptTDSpecificCtx, i4Argc, aszArgv);
}

static void DVBS_TunerDetachI2C_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    DVBS_TunerDetachI2C(1, ptTDSpecificCtx, bDetach);
}

static INT16 DVBS_TunerGetAttribute_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    return DVBS_TunerGetAttribute(1, ptTDSpecificCtx, e_get_type, pzAttribute, pzAttributeLen);
}

static INT32 DVBS_TunerSetAttribute_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    return DVBS_TunerSetAttribute(1, ptTDSpecificCtx, e_set_type, pvAttribute, zAttributeLen);
}

#ifdef fcADD_DIAG_INFO
static INT32 DVBS_TunerSetDiagCmd_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    return DVBS_TunerSetDiagCmd(1, ptTDSpecificCtx, e_tuner_set_type, pv_set_info, z_size);
}

static INT32 DVBS_TunerGetDiagInfo_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    return DVBS_TunerGetDiagInfo(1, ptTDSpecificCtx, e_tuner_get_type, pv_get_info, pz_size);
}
#endif

static void DVBS_TunerBypassI2C_1(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    DVBS_TunerBypassI2C(1, ptTDSpecificCtx, bSwitchOn);
}

static BOOL DVBS_TunerNotifyEnable_1(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    return DVBS_TunerNotifyEnable(1, ptTDSpecificCtx);
}

static void PD_DVBS_Register_LTDIS_Fct(UINT8 tunerID, PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T *pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;

    if (tunerID == 0) {
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose   = DVBS_TunerClose_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq     = DVBS_TunerAcq_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = DVBS_TunerGetSync_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = DVBS_TunerGetSignal_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBS_TunerGetSignalLevel_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBS_TunerGetSignalLeveldBm_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBS_TunerGetSignalPER_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBS_TunerGetSignalSNR_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = DVBS_TunerSetMpgFmt_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer  = DVBS_TunerGetVer_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = DVBS_TunerNimTest_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBS_TunerSetRegSetting_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBS_TunerShowRegSetting_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = DVBS_TunerTestI2C_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = DVBS_TunerCommand_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = DVBS_TunerDtdCommand_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = DVBS_TunerDetachI2C_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc    = DVBS_TunerDisc_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DVBS_TunerSetAttribute_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DVBS_TunerGetAttribute_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= DVBS_TunerBypassI2C_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNotifyEnable = DVBS_TunerNotifyEnable_0;
#ifdef fcADD_DIAG_INFO
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = DVBS_TunerSetDiagCmd_0;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DVBS_TunerGetDiagInfo_0;
#endif
    } else {
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose   = DVBS_TunerClose_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq     = DVBS_TunerAcq_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = DVBS_TunerGetSync_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = DVBS_TunerGetSignal_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBS_TunerGetSignalLevel_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBS_TunerGetSignalLeveldBm_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBS_TunerGetSignalPER_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBS_TunerGetSignalSNR_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = DVBS_TunerSetMpgFmt_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer  = DVBS_TunerGetVer_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = DVBS_TunerNimTest_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBS_TunerSetRegSetting_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBS_TunerShowRegSetting_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = DVBS_TunerTestI2C_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = DVBS_TunerCommand_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = DVBS_TunerDtdCommand_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = DVBS_TunerDetachI2C_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc    = DVBS_TunerDisc_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DVBS_TunerSetAttribute_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DVBS_TunerGetAttribute_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= DVBS_TunerBypassI2C_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNotifyEnable = DVBS_TunerNotifyEnable_1;
#ifdef fcADD_DIAG_INFO
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = DVBS_TunerSetDiagCmd_1;
        pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DVBS_TunerGetDiagInfo_1;
#endif
    }
}

//-------------------------------------------------------------------------
/**
 *  This API is driver entry point. It performs
 *  1. allocate and initialize resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDCtx            TunerHAL Context use for LTDIS API mapping
 *  @param   ptTunerCfgInfo     Tuner configuration information.
 *  @param   pptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
static INT32 DVBS_TunerOpen(UINT8 tunerID, PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_ctx_t *pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    sonysat_integ_t *pInteg = &pDemodCtx->integ;
    sonysat_demod_t *pDemod = &pDemodCtx->demod;
    sonysat_tuner_sat_t *pTuner = &pDemodCtx->tuner;
    sonysat_lnbc_t *pLnbc = &pDemodCtx->lnbc;
    sonysat_i2c_t *pI2c = &pDemodCtx->i2c;
#ifdef CC_PROJECT_FOR_SONY
    sonysat_i2c_t *pI2cTuner = &pDemodCtx->i2cTuner;
    sonysat_horus3a_t *pHorus3a = &pDemodCtx->horus3a;
    UINT8 data = 0;
    UINT32 tunerFlags = 0;
#else
    #ifndef SONYSAT_TUNERMODULE_HELENE
    sonysat_i2c_t *pI2cTuner = &pDemodCtx->i2cTuner;
    sonysat_horus3a_t *pHorus3a = &pDemodCtx->horus3a;
    sonysat_horus3_t *pHorus3 = &pDemodCtx->horus3;
    UINT8 data = 0;
    UINT32 tunerFlags = 0;
    #endif
#endif
    UINT8 i2cAddressDemod = 0xD8;
    UINT8 i2cAddressTuner = 0xC0;

    sonysat_demod_xtal_t demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;
    BOOL is_dsqout_pwm = FALSE;
    UINT8 is_iqinv = 0;

    BOOL ignore_lnbc = FALSE;

    DVBS_PD_CTX_T * pPdCtxDVBS = NULL;
    TUNER_DEVICE_CTX_T * pDVBSTunerDeviceCtx = NULL;

    pDemodCtx->unicable_p = 0;
    pDemodCtx->bs_stop_freq  = 2150;
    pDemodCtx->bs_start_freq =  950;
    pDemodCtx->do_blindscan = 0;
    pDemodCtx->Monitor_waiting_semaphore_flag = BOOL_FALSE;
    pDemodCtx->ui4_data_forMW = 0;
    pDemodCtx->channel_locked_flag = 1;
    pDemodCtx->set_flag = 0;
    pDemodCtx->ui1_mount_status = 0;
    pDemodCtx->PreUserBandIdx = 0xFF;
    pDemodCtx->UnicableDelay = 0;

    // Set initial log level
    //u1DbgLevel=4;

    //create pdctx used in this file, you also can add any declaration you want in pdctx
    mcDBG_MSG_L2(("CXD2854[%d] %s \n", tunerID, __FUNCTION__));

    pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
    if (pPdCtxDVBS == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/

    pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
    mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));

    // Store pointers to ctx.
    pDemodCtx->pPdCtxDVBS = pPdCtxDVBS;
    pDemodCtx->pDVBSTunerDeviceCtx = pDVBSTunerDeviceCtx;

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pDemodCtx->AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(AutoScan_callback) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }

    if (x_sema_create(&pPdCtxDVBS->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtxDVBS->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtxDVBS->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcDBG_MSG_ERR(("EN_DVBS_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
    }

    //register LTDIS APIs to HAL
    PD_DVBS_Register_LTDIS_Fct(tunerID, ptTDCtx);

    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
    Demod_GPIO_Reset(tunerID);

    // Sony specific initialization -------------------------------------------

    pDemodCtx->breakFct = NULL;
    pDemodCtx->breakFctArg = NULL;

#ifdef CC_PROJECT_FOR_SONY
    /* ========== Settings for Sony Project ========== */
    {
        // For FY15 Sony Project Configuration
        i2cAddressDemod = 0xD8;
        i2cAddressTuner = 0xC0;

        VDRBRDCST_GetSonyProductTypeInfo(&g_sonyproduct_typeinfo);

        mcDBG_MSG_L2(("CXD2854[%d] : Board Ver = %d, Model Variation ID = %d\n",
            tunerID, g_sonyproduct_typeinfo.eBoardVer, g_sonyproduct_typeinfo.modelVariationID));

        // Invalid Model Variation ID check
        switch((int) g_sonyproduct_typeinfo.modelVariationID){
        case 0x00: // AEP/PA-T2S2-1Tuner
        case 0x01: // AEP/PA-T2S2-2Tuner
        case 0x02: // PA/PH-T2-1Tuner
        case 0x06: // LA-1Tuner
        case 0x07: // LA-2Tuner
            // Artemis Model
            break;

        case 0x03: // TW-1Tuner
        case 0x04: // CH/HK-1Tuner
        case 0x05: // UC/MX-1Tuner
            // Non Artemis Model
            mcDBG_MSG_L2(("CXD2854[%d] : Warning : This model does not includes CXD2854. (%d)\n", tunerID, g_sonyproduct_typeinfo.modelVariationID));
            break;

        default:
            // Error!?
            mcDBG_MSG_L2(("CXD2854[%d] : Fatal Error!! Invalid Model Variation ID!! (%d)\n", tunerID, g_sonyproduct_typeinfo.modelVariationID));
            break;
        }

        // Demod I2C Connection
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // Salvia
            // I2C_D is used
            sonysat_i2c_MTK_CreateI2c(pI2c, SONYSAT_I2C_MTK_BUSID_D);
        }else{
            // Sakura
            // For SCT, I2C_D is used
            // For CVT or later, I2C_A is used for main tuner, and I2C_D is used for sub tuner.
            if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
                sonysat_i2c_MTK_CreateI2c(pI2c, SONYSAT_I2C_MTK_BUSID_D);
            }else{
                if(tunerID == 0){
                    sonysat_i2c_MTK_CreateI2c(pI2c, SONYSAT_I2C_MTK_BUSID_A);
                }else{
                    sonysat_i2c_MTK_CreateI2c(pI2c, SONYSAT_I2C_MTK_BUSID_D);
                }
            }
        }

        // Tuner I2C Connection (For tuner register dump)
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){
            // Salvia
            // I2C_D is used
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sonysat_i2c_MTK_CreateI2cGw(pI2cTuner, SONYSAT_DEMOD_IIC_ADDRESS_0, 0x09, SONYSAT_I2C_MTK_BUSID_D);
            #else
            sonysat_i2c_MTK_CreateI2c(pI2cTuner, SONYSAT_I2C_MTK_BUSID_D);
            #endif
        }else{
            // Sakura
            // Single, Dual main tuner, use I2C_A and direct access.
            // Dual sub tuner, use I2C_D. Use Repeater for SCT, and direct access for CVT or later.
            if(tunerID == 0){
                #ifdef CC_I2C_GATEWAY_SUPPORT
                sonysat_i2c_MTK_CreateI2cGw(pI2cTuner, SONYSAT_DEMOD_IIC_ADDRESS_0, 0x09, SONYSAT_I2C_MTK_BUSID_A);
                #else
                sonysat_i2c_MTK_CreateI2c(pI2cTuner, SONYSAT_I2C_MTK_BUSID_A);
                #endif
            }else{
                #ifdef CC_I2C_GATEWAY_SUPPORT
                sonysat_i2c_MTK_CreateI2cGw(pI2cTuner, SONYSAT_DEMOD_IIC_ADDRESS_1, 0x09, SONYSAT_I2C_MTK_BUSID_D);
                #else
                sonysat_i2c_MTK_CreateI2c(pI2cTuner, SONYSAT_I2C_MTK_BUSID_D);
                #endif
                // Note : I2C Repeater will be controlled by demod side.
            }
        }

        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            i2cAddressTuner = 0xC6;
        }else{
            if(tunerID == 0){
                i2cAddressTuner = 0xC0;
            }else{
                i2cAddressTuner = 0xC2;
            }
        }

        // Demod I2C Address
        if(tunerID == 0){
            i2cAddressDemod = SONYSAT_DEMOD_IIC_ADDRESS_0;
        }else{
            i2cAddressDemod = SONYSAT_DEMOD_IIC_ADDRESS_1;
        }

        // Check Demod Chip ID and Create
        {
            data = 0x00;
            sonysat_result = pI2c->WriteOneRegister(pI2c, i2cAddressDemod + 4, 0x00, 0x00);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("CXD2854[%d] : Chip ID Check Error!! (%s).\n", tunerID, sonysat_FormatResult(sonysat_result)));
                return DRVAPI_TUNER_ERROR;
            }

            sonysat_result = pI2c->ReadRegister(pI2c, i2cAddressDemod + 4, 0xFD, &data, 1);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L1(("CXD2854[%d] : Chip ID Check Error!! (%s).\n", tunerID, sonysat_FormatResult(sonysat_result)));
                return DRVAPI_TUNER_ERROR;
            }
            
            if(data == SONYSAT_DEMOD_CHIP_ID_CXD2854){
                // Integ and demod struct creation used in CXD2854 driver
                sonysat_result = sonysat_integ_Create(pInteg, SONYSAT_DEMOD_XTAL_24000KHz, i2cAddressDemod, pI2c, pDemod, pTuner, pLnbc);
                if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
            }else{
                // Non-CXD2854 case.
                sonysat_result = sonysat_integ_Create(pInteg, SONYSAT_DEMOD_XTAL_41000KHz, i2cAddressDemod, pI2c, pDemod, pTuner, pLnbc);
                if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                demod_xtal = SONYSAT_DEMOD_XTAL_41000KHz;
            }
        }

        // Create Tuner driver
        {
            if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
                // SCT = HORUS3A
                uint8_t tunerXtalMHz = 0;
                switch (g_sonyproduct_typeinfo.modelVariationID) {
                case 0x00:
                default:
                    if(g_sonyproduct_typeinfo.eProjectID == 0x01){ //Salvia
                        tunerFlags  = SONYSAT_HORUS3A_CONFIG_IQOUT_SINGLEEND;      /* IQ out : Single ended */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_REFOUT_OFF;           /* REFOUT off */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL; /* Do not stop xtal in power save state */
                        #ifdef SONYSAT_TUNER_EXT_REF
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
                        #endif
                        #ifdef SONYSAT_TUNER_LNA_ENABLE
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_LNA_ENABLE;           /* LNA enable */
                        #endif
                        tunerXtalMHz = 16;
                    }else{
                        // Single tuner model
                        tunerFlags  = SONYSAT_HORUS3A_CONFIG_IQOUT_SINGLEEND;      /* IQ out : Single ended */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_REFOUT_OFF;           /* REFOUT off */
                        //tunerFlags |= SONYSAT_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL; /* Do not stop xtal in power save state */
                        tunerXtalMHz = 27;
                    }
                    break;

                case 0x01:
                    // Double tuner model
                    if (tunerID == 0) {
                        tunerFlags  = SONYSAT_HORUS3A_CONFIG_IQOUT_SINGLEEND;      /* IQ out : Single ended */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_REFOUT_OFF;           /* REFOUT off */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL; /* Do not stop xtal in power save state */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
                        tunerXtalMHz = 16;
                    } else {
                        tunerFlags  = SONYSAT_HORUS3A_CONFIG_IQOUT_SINGLEEND;      /* IQ out : Single ended */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_REFOUT_OFF;           /* REFOUT off */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL; /* Do not stop xtal in power save state */
                        tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
                        tunerXtalMHz = 16;
                    }
                    break;
                }
                // Create HORUS3A driver
                sonysat_result = sonysat_tuner_horus3a_Create(pTuner,
                    tunerXtalMHz,
                    i2cAddressTuner,
                    pI2cTuner,
                    tunerFlags,
                    pHorus3a);
                if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                is_iqinv = 0; /* Todo: Check */
            } else {
                // CVT/MVT = HELENE
                // Create HELENE driver
                sonysat_result = sonysat_tuner_helene_MTK_Create(pTuner, tunerID, &pDemodCtx->helene);
                if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
                is_iqinv = 0;
                pTuner->pI2c = pI2cTuner;
                pTuner->i2cAddress = i2cAddressTuner;
                switch (g_sonyproduct_typeinfo.modelVariationID) {
                case 0x01:
                    /* Double tuner model */
                    pTuner->flags |= SONYSAT_HELENE_MTK_CONFIG_SAT_LNA_OFF;
                    break;
                case 0x00:
                default:
                    /* Single tuner model */
                    break;
                }
            }
        }

        // Create LNBC driver
        {
            uint8_t i2cAddressLNBC = 0x10;
            ignore_lnbc = FALSE;
            is_dsqout_pwm = FALSE;
            if (tunerID == 0) {
                i2cAddressLNBC = 0x10;
            } else {
                i2cAddressLNBC = 0x12;
            }
            sonysat_result = allegro_a8303_1_Create (pLnbc, i2cAddressLNBC, pI2c);
            if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
        }

        // I2C Repeater Setting
        // Tuner can be access directly except for SCT, Sub-Tuner case.
        pDemod->i2cRepeaterEnable = 0;
        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            if(g_sonyproduct_typeinfo.eProjectID == 0x01){ //Salvia
                #ifndef CC_I2C_GATEWAY_SUPPORT
                pDemod->i2cRepeaterEnable = 1;
                #endif
            }else{
                if(tunerID == 1){ // Sub
                    pDemod->i2cRepeaterEnable = 1;
                }
            }
        }
        //----------------------------------
        //check mount status
        pDemodCtx->ui1_mount_status = 0x01;
        if(ignore_lnbc != TRUE) {
            pDemodCtx->ui1_mount_status |= 0x02;
        }
        //----------------------------------

        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
	        is_parallel = 1; // Parallel Output Only
        }else{
		    is_parallel = 0; // Serial Output Only
        }
    }
#else
    /* ========== Settings for General Customer Projects ========== */
    {
#ifdef SONYSAT_DUMMY_DEBUG
        /* for dummy debug */
        i2cAddressDemod = 0xDA; /* 0xD8/0xDA */
        i2cAddressTuner = 0xC2; /* 0xC0/0xC2 */
        is_iqinv = 0; /* Bernabeu : 0, SYSEVA : 1 */
#else
#if defined(CC_SONYDEMOD_CXD2839)
        i2cAddressDemod = 0xC8;
        i2cAddressTuner = 0xC6;
        is_iqinv = 0;
#else
        i2cAddressDemod = 0xD8;
        i2cAddressTuner = 0xC6;
        is_iqinv = 0;
#endif
#endif

        // I2C struct creation used in CXD2854 driver
        sonysat_result = sonysat_i2c_MTK_CreateI2c(pI2c, SONYSAT_I2C_MTK_BUSID_D);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

#ifdef CC_SONYDEMOD_I2CGATEWAY
        // I2C struct creation for I2C Gateway in CXD2854 driver
        sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cTuner, i2cAddressDemod, 0x09, SONYSAT_I2C_MTK_BUSID_D);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif

#ifndef SONYSAT_TUNERMODULE_HELENE

#ifdef CC_SONYDEMOD_I2CGATEWAY
        // I2C struct creation for I2C Gateway in CXD2854 driver
        sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cTuner, i2cAddressDemod, 0x09, SONYSAT_I2C_MTK_BUSID_D);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif

#ifndef CC_SONYDEMOD_I2CGATEWAY
        // Enable I2C repeater via demodulator.
        sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, i2cAddressDemod + 4, 0x08, 0x01, 0x01);
#endif

        //----------------------------------
        //check mount status
        pDemodCtx->ui1_mount_status = 0x00;
        if(sonysat_result == SONYSAT_RESULT_OK) {
            pDemodCtx->ui1_mount_status = 0x01;
        }
        if(ignore_lnbc != TRUE) {
            pDemodCtx->ui1_mount_status |= 0x02;
        }
        //----------------------------------

        if(sonysat_result != SONYSAT_RESULT_OK) {
            return DRVAPI_TUNER_ERROR;
        }

        // Read tuner product id
        sonysat_result = pI2cTuner->ReadRegister (pI2cTuner, i2cAddressTuner, 0x7F, &data, 1);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

#ifndef CC_SONYDEMOD_I2CGATEWAY
        // Disable I2C repeater via demodulator.
        sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, i2cAddressDemod + 4, 0x08, 0x00, 0x01);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif

        //                                    | PE221 | PE231 | ZE221 |
        // -----------------------------------+-------+-------+-------+
        // SONYSAT_TUNER_EXT_REF              |   o   |   o   |   x   |
        // SONYSAT_TUNER_LNA_ENABLE           |   x   |   o   |   x   |
        // -----------------------------------+-------+-------+-------+
        if(data == 0x07){
            // ----------------------------------------
            // Tuner struct creation (HORUS3A)
            // ----------------------------------------
            mcDBG_MSG_L2(("Create struct for HORUS3A\n"));
            tunerFlags  = SONYSAT_HORUS3A_CONFIG_IQOUT_SINGLEEND;      /* IQ out : Single ended */
            tunerFlags |= SONYSAT_HORUS3A_CONFIG_REFOUT_OFF;           /* REFOUT off */
            tunerFlags |= SONYSAT_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL; /* Do not stop xtal in power save state */
#ifdef SONYSAT_TUNER_EXT_REF
            tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
            tunerFlags |= SONYSAT_HORUS3A_CONFIG_LNA_ENABLE;           /* LNA enable */
#endif

            sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16,  /* 16MHz Xtal */
                i2cAddressTuner,
                pI2cTuner,
                tunerFlags,
                pHorus3a);
            if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
        } else if((data == 0x51) || (data == 0x61)){
            // ----------------------------------------
            // Tuner struct creation (HORUS3)
            // ----------------------------------------
            mcDBG_MSG_L2(("Create struct for HORUS3\n"));
            tunerFlags  = SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND;       /* IQ out : Single ended */
            tunerFlags |= SONYSAT_HORUS3_CONFIG_REFOUT_OFF;            /* REFOUT off */
            tunerFlags |= SONYSAT_HORUS3_CONFIG_POWERSAVE_ENABLEXTAL;  /* Do not stop xtal in power save state */
#ifdef SONYSAT_TUNER_EXT_REF
            tunerFlags |= SONYSAT_HORUS3_CONFIG_EXT_REF;               /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
            tunerFlags |= SONYSAT_HORUS3_CONFIG_LNA_ENABLE;            /* LNA enable */
#endif

            sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16,   /* 16MHz Xtal */
                i2cAddressTuner,
                pI2cTuner,
                tunerFlags,
                pHorus3);
            if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
        } else {
            mcDBG_MSG_ERR(("Error to access to tuner.\n"));
            return DRVAPI_TUNER_ERROR;
        }

#else /* SONYSAT_TUNERMODULE_HELENE */
        sonysat_result = sonysat_tuner_helene_MTK_Create(pTuner, tunerID, &pDemodCtx->helene);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif /* SONYSAT_TUNERMODULE_HELENE */

    // LNB struct creation ----------------------------------------------------
#if defined(SONYSAT_LNBC_MPS_MP8126)
        ignore_lnbc = FALSE;
        sonysat_result = mps_mp8126_Create(pLnbc, pDemod, 1, 2, 0, &pDemodCtx->mp8126);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ST_LNBH29)
        ignore_lnbc = FALSE;
        sonysat_result = st_lnbh29_Create (pLnbc, 0x10, pI2c);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
        ignore_lnbc = FALSE;
        sonysat_result = allegro_a8304_Create (pLnbc, 0x10, pI2c);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8303_1)
        ignore_lnbc = FALSE;
        sonysat_result = allegro_a8303_1_Create (pLnbc, 0x10, pI2c);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif

        // NOTE: Please set correct demod xtal frequency here!!
        // SONYSAT_DEMOD_XTAL_20500KHz : 20.5MHz
        // SONYSAT_DEMOD_XTAL_24000KHz : 24MHz (default)
        // SONYSAT_DEMOD_XTAL_41000KHz : 41MHz
#if defined SONYSAT_DEMOD_CRYSTAL_41M
        demod_xtal = SONYSAT_DEMOD_XTAL_41000KHz;
        mcDBG_MSG_L2(("CXD2854: Using 41MHz crystal!!\n"));
#elif defined SONYSAT_DEMOD_CRYSTAL_20_5M
        demod_xtal = SONYSAT_DEMOD_XTAL_20500KHz;
        mcDBG_MSG_L2(("CXD2854: Using 20.5MHz crystal!!\n"));
#else // 24MHz
        demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
        mcDBG_MSG_L2(("CXD2854: Using 24MHz crystal!!\n"));
#endif

        // Integ and demod struct creation used in CXD2854 driver -----------------
        if(ignore_lnbc){
            // LNBC seems not exist, so configure without LNBC.
            sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, i2cAddressDemod, pI2c, pDemod, pTuner, NULL);
        }else{
            sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, i2cAddressDemod, pI2c, pDemod, pTuner, pLnbc);
        }
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    }

    // NOTE: Please set DSQOUT output is PWM (Tone is generated by Demod)
    //                                or ENVELOPE (Tone is generated by LNBC)
#if defined(SONYSAT_LNBC_ST_LNBH29) || defined(SONYSAT_LNBC_ALLEGRO_A8304) || defined(SONYSAT_LNBC_ALLEGRO_A8303_1)
    // LNBH29, A8304 and A8303_1 only support ENVELOPE
    is_dsqout_pwm = FALSE;
#else
    is_dsqout_pwm = TRUE;
#endif

#endif

    mcDBG_MSG_L2(("Starting CXD2854(DVB-S/S2 part) Demod Initialization.\n"));

    // Sony demod initialization ----------------------------------------------
    sonysat_result = sonysat_integ_InitializeS(pInteg);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_integ_InitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
        return DRVAPI_TUNER_ERROR;
    }

    // Setting for BlindScan
    sonysat_result = sonysat_demod_SetConfig (&pDemodCtx->demod, SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_MODE, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
        return DRVAPI_TUNER_ERROR;
    }
    sonysat_result = sonysat_demod_SetConfig (&pDemodCtx->demod, SONYSAT_DEMOD_CONFIG_DVBSS2_SPECTRUM_COUNT, 4);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
        return DRVAPI_TUNER_ERROR;
    }

    // IFAGC negative (for HORUS3 silicon tuner)
    sonysat_result = sonysat_demod_SetConfig(pDemod, SONYSAT_DEMOD_CONFIG_SAT_IFAGCNEG, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
        return DRVAPI_TUNER_ERROR;
    }

    // IQ sense normal (Depend on tuner and demod IQ line connection)
    sonysat_result = sonysat_demod_SetConfig(pDemod, SONYSAT_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV, is_iqinv);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
        return DRVAPI_TUNER_ERROR;
    }

    // NOTE: Following setting is necessary only for serial TS output.
    if(is_parallel){
        // It's hardware default value so additional setting is unnecessary.
    }else{
        // TS output format (Serial)
        sonysat_result = sonysat_demod_SetConfig(pDemod, SONYSAT_DEMOD_CONFIG_PARALLEL_SEL, 0);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
            return DRVAPI_TUNER_ERROR;
        }

        // TS output from TSDATA[0] (For Serial)
        sonysat_result = sonysat_demod_SetConfig(pDemod, SONYSAT_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("Error in sonysat_demod_SetConfig (%s)\n", sonysat_FormatResult(sonysat_result)));
            return DRVAPI_TUNER_ERROR;
        }
    }

    #ifdef CC_CANAL_DIGITAL_SPEC_SUPPORT
    b_already_read_eep = 0;
    sonysat_init_eeprom_non_canaldigital(tunerID); 
    #endif
    if(ignore_lnbc){
        /* LNBC doesn't exist. Do nothing. */
    } else {
        if(is_dsqout_pwm){
            sonysat_result = pLnbc->SetConfig(pLnbc, SONYSAT_LNBC_CONFIG_ID_TONE_INTERNAL, 0);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L2(("Error in pLnbc->SetConfig(TONE_INTERNAL(PWM)) (%s)\n", sonysat_FormatResult(sonysat_result)));
                return DRVAPI_TUNER_ERROR;
            }
            sonysat_result = sonysat_demod_sat_device_ctrl_DSQOUTSetting(pDemod, SONYSAT_DSQOUT_MODE_PWM, 22);
        }else{
            sonysat_result = pLnbc->SetConfig(pLnbc, SONYSAT_LNBC_CONFIG_ID_TONE_INTERNAL, 1);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_L2(("Error in pLnbc->SetConfig(TONE_INTERNAL(Envelope)) (%s)\n", sonysat_FormatResult(sonysat_result)));
                return DRVAPI_TUNER_ERROR;
            }
            sonysat_result = sonysat_demod_sat_device_ctrl_DSQOUTSetting(pDemod, SONYSAT_DSQOUT_MODE_ENVELOPE, 22);
        }
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("Error in sonysat_demod_sat_device_ctrl_DSQOUTSetting (%s)\n", sonysat_FormatResult(sonysat_result)));
            return DRVAPI_TUNER_ERROR;
        }
    }

    // Sony specific initialization (end) -------------------------------------

    // NOTE: DVB-S, S2, tuner control will be done in this driver.
    //       So, following code will not be necessary.
    //Regisiter tuner driver
    //ITunerRegistration(pDVBSTunerDeviceCtx->tunerType);
    //ITuner_Init(ITunerGetCtx());

    #ifdef CC_DEMOD_NON_DVBS_LNBC_OUTPUT_0V
    if((pLnbc)&&(pLnbc->Sleep))
    {
        pLnbc->Sleep(pLnbc);
        mcDBG_MSG_L1(("[DVBS_TunerOpen][%d] ......set lnbc sleep......\n",tunerID));
    }
    #endif
    
    pPdCtxDVBS->fgDisStatus = TRUE;
    
    //-----------------------------------------------------------------------------

    //END
    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
#if 0
        if (x_sema_create(&pPdCtxDVBS->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcDBG_MSG_ERR(("EN_DVBS_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
#endif
        pPdCtxDVBS->fgBreakMonitor = TRUE;  //init
        pPdCtxDVBS->fgPIMoniStatus = FALSE; //init
        PD_StartMonitorEngine(pPdCtxDVBS);  //start monitor engine
    }

    return (DRVAPI_TUNER_OK);
}

/* Function trace (For Debug) */
void sonysat_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONYSAT_ENTER : %s (%d)\n", funcname, linenum));
}

/* Function trace (For Debug */
void sonysat_trace_log_return(sonysat_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L4(("SONYSAT_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONYSAT_RETURN : (line %d)\n", linenum));
    }
}

/* Function trace for I2C functions (For Debug) */
void sonysat_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sonysat_trace_log_enter(funcname, filename, linenum);
}

/* Function trace for I2C functions (For Debug) */
void sonysat_trace_i2c_log_return(sonysat_result_t result, const char* filename, unsigned int linenum)
{
    sonysat_trace_log_return(result, filename, linenum);
}

sonysat_result_t sonysat_integ_CheckCancellation (sonysat_integ_t * pInteg)
{
    UINT8 tunerID = 0;
    sonysat_demod_ctx_t * pDemodCtx;

    SONYSAT_TRACE_ENTER ("sonysat_integ_CheckCancellation");

    /* Argument verification. */
    if (!pInteg) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if(pInteg == &(g_sonysat_demod_ctx[1].integ)){
        tunerID = 1;
    }else{
        tunerID = 0;
    }

    pDemodCtx = &g_sonysat_demod_ctx[tunerID];

    /* Check break from MTK uppder layer here!! */
    if(pDemodCtx->do_blindscan){
        // Blind scan case
        if(BlindScan_MidWare_Break(tunerID)){
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_CANCEL);
        }
    } else if(pDemodCtx->breakFct){
        if(pDemodCtx->breakFct(pDemodCtx->breakFctArg)){
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_CANCEL);
        }
    }

    /* Check the cancellation flag. */
    if (sonysat_atomic_read (&(pInteg->cancel)) != 0) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

UINT8 DVBS_DemodMountStatus(UINT8 tunerID)
{
    sonysat_demod_ctx_t * pDemodCtx = &g_sonysat_demod_ctx[tunerID];
    //mcDBG_MSG_L2(("DemodMountStatus %d \n",ui1_mount_status));
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx[tunerID].demod;
    if(pDemodCtx->ui1_mount_status & 0x1)
    {
        mcDBG_MSG_L2(("Demod Chip ID = 0x%X \n",pDemod->chipId));
    }
    else
    {
        mcDBG_MSG_L2(("Demod Enable = 0\n"));
    }
    if(pDemodCtx->ui1_mount_status & 0x2)
    {
        mcDBG_MSG_L2(("LNB Enable = 1\n"));
    }
    else
    {
        mcDBG_MSG_L2(("LNB Enable = 0\n"));
    }

    return pDemodCtx->ui1_mount_status;
}

void sonysat_demod_dvbs_s2_blindscan_trace_init (void)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_powerlist (char * name, sonysat_demod_dvbs_s2_blindscan_power_t * pList)
{
    if (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_POWER_SPECTRUM) {
        sonysat_demod_dvbs_s2_blindscan_power_t * pCurrent = pList;
        while (pCurrent) {
            mcDBG_MSG_L2(("%d, %d\n", pCurrent->freqKHz, pCurrent->power));
            pCurrent = pCurrent->pNext;
        }
    }
}

void sonysat_demod_dvbs_s2_blindscan_trace_candlist (char * name, sonysat_demod_dvbs_s2_blindscan_data_t * pList)
{
    if (g_sonysat_blindscan_log_flag & SONYSAT_BLINDSCAN_LOG_CANDIDATE) {
        sonysat_demod_dvbs_s2_blindscan_data_t * pCurrent = pList;
        while (pCurrent) {
            mcDBG_MSG_L2(("%d, %d (%d-%d)\n", pCurrent->data.candidate.centerFreqKHz, pCurrent->data.candidate.symbolRateKSps, pCurrent->data.candidate.minSymbolRateKSps, pCurrent->data.candidate.maxSymbolRateKSps));
            pCurrent = pCurrent->pNext;
        }
    }
}

void sonysat_demod_dvbs_s2_blindscan_trace_bt_start (uint32_t freqKHz, uint32_t candSR, uint32_t minCandSR, uint32_t maxCandSR, uint32_t minSR, uint32_t maxSR)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_bt_result (char * eventName)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_cs_tune (uint32_t freqKHz)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_cs_start (int32_t offsetFreqKHz)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_cs_result (uint32_t coarseSR)
{
}

void sonysat_demod_dvbs_s2_blindscan_trace_pm_power (int32_t offsetFreqKHz, int32_t power)
{
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
// Tuner #0
INT32 DVBS_C1_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    return DVBS_TunerOpen(0, ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
}

// Tuner #1
INT32 DVBS_C2_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    return DVBS_TunerOpen(1, ptTDCtx, ptTunerCfgInfo, pptTDSpecificCtx);
}
