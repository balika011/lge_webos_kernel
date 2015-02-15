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

#ifdef CC_PROJECT_FOR_SONY
#include "eeprom_if.h"  //jk.xie
#endif /* CC_PROJECT_FOR_SONY */

#if defined(SONYSAT_TUNERMODULE_PE231)
// For PE231 tuner module
#define SONYSAT_TUNER_EXT_REF
#define SONYSAT_TUNER_LNA_ENABLE
#elif defined(SONYSAT_TUNERMODULE_ZE221)
// For ZE221 tuner module
#elif defined(SONYSAT_TUNERMODULE_PE221)
// For PE221 tuner module
#define SONYSAT_TUNER_EXT_REF
#else
#error Please set tuner module type.
#endif

#if defined(SONYSAT_LNBC_MPS_MP8126)
// For MPS_MP8126 LNBC
#include "mps_mp8126.h"
#elif defined(SONYSAT_LNBC_ST_LNBH29)
// For ST_LNBH29 LNBC
#include "st_lnbh29.h"
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
// For ALLEGRO_A8304 LNBC
#include "allegro_a8304.h"
#else
#error Please set lnbc module type.
#endif

// for Sony CXD2854DVBSS2 driver
#include "sonysat_i2c_MTK.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_integ_dvbs_s2.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_tuner_horus3a.h"
#include "sonysat_horus3a.h"
#include "sonysat_tuner_horus3.h"
#include "sonysat_horus3.h"
#include "sonysat_lnbc.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_integ_sat_device_ctrl.h"
#include "sonysat_math.h"

#define SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK

#define SCAN_RANGE_SINGLE_TIME     1 //scan range is 1M
#define Scan_Center_Freq_Step_100KHz        100
#define VENDOR_DRIVER_VER            "CXD2854 (DVB-S/S2) v0.04 (2014-03-18) _Xiaohong Modified for Sakura SCT"

#define SONYSAT_DVBS_NOT_RESET_DEMOD

#ifdef CC_PROJECT_FOR_SONY
#define eep_Non_Canal_Digital_Address_1     0x3162      // EEPROM Address for DS_PNC_OFF 1 byte
#define eep_Non_Canal_Digital_Address_2     0x3163      // EEPROM Address for DS_UCP_PLLSWOFF 1 byte
#define eep_Non_Canal_Digital_Address_3     0x3164      // EEPROM Address for DS_K_FINE2_G1G2 1 byte
#define eep_Non_Canal_Digital_Address_4     0x3165      // EEPROM Address for DS_Q_FINE2_G1G2 1 byte
#define eep_Non_Canal_Digital_Address_5     0x3166      // EEPROM Address for DS_8_FINE2_G1G2 1 byte
#define eep_Non_Canal_Digital_Address_6     0x3167      // EEPROM Address for DS_G1_RANGE 1 byte
#define eep_Non_Canal_Digital_Address_7     0x3168      //Flag for reset all register to 0x00 0x00 0x7e 0xce 0x9e 0x00
#endif /* CC_PROJECT_FOR_SONY */

static UINT16 bs_stop_freq = 2150; // In MHz
static UINT16 bs_start_freq = 950; // In MHz

#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
static UINT32 g_unicable_p = 0;
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Import extern variable
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
    mcDBG_MSG_L4((" [ _Lock_%s-%d ]\r\n", __FUNCTION__, __LINE__)); \
    x_sema_lock(sema, X_SEMA_OPTION_WAIT);                           \
    mcDBG_MSG_L4((" [ _Excape_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
}
#define mcSEMA_UNLOCK_MTFE(sema)                                     \
{                                                                    \
    mcDBG_MSG_L4((" [ _Unlock_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
    x_sema_unlock(sema);                                             \
}

typedef struct _sonysat_sqi_table_t{
    uint8_t sqi;   /* 0 - 100 */
    int32_t value;
} sonysat_sqi_table_t;
//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
static DVBS_PD_CTX_T *pPdCtxDVBS;
static TUNER_DEVICE_CTX_T*    pDVBSTunerDeviceCtx = NULL;
static volatile UINT8 do_blindscan = 0;
static volatile UINT8 Monitor_waiting_semaphore_flag = BOOL_FALSE;
static UINT32 ui4_data_forMW = 0;

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
static HANDLE_T  AutoScan_callback;
//static DVBS2_CHANNEL_T Current_channel;/*during blindscan process,record the currnet try_lock channel freq_info,or record the start_freq*/
static UINT8 g_channel_locked_flag=1;   //default is true;
static DISEQC_BUS_CMD_T g_diseqc_cmd;
static UINT8 g_set_flag=0;
static UINT8 g_ui1_mount_status = 0;
static UINT8 g_PreUserBandIdx = 0xff;
static UINT16 g_UnicableDelay = 0;//in seconds
#define Debug_time

//-----------------------------------------------------------------------------
// static variables (Sony specific)
//-----------------------------------------------------------------------------
typedef struct _sonysat_demod_ctx_t // Sony CXD2854DVBSS2 Driver Specific
{
    // Instances to control CXD2854 driver
    sonysat_demod_t demod;
    sonysat_integ_t integ;
    sonysat_tuner_sat_t tuner;
    sonysat_lnbc_t lnbc;
    sonysat_i2c_t i2c;
     sonysat_i2c_t i2cGw;//I2C GateAway
    sonysat_horus3a_t horus3a;
    sonysat_horus3_t horus3;
#ifdef SONYSAT_LNBC_MPS_MP8126
    mps_mp8126_t mp8126;
#endif

    // For saving current tuned signal information
    sonysat_dvbs_s2_tune_param_t tunedSignal;
    // For saving current blindscan progress information
    uint8_t blindscanProgress;

    // Data for unicable
    sonysat_integ_singlecable_data_t singlecableData;

    // For normal tune break
    x_break_fct breakFct; // Break function pointer to use in stopwatch function
    void* breakFctArg;    // Argument for break function
#ifdef CC_PROJECT_FOR_SONY
    uint8_t PreValue_non_CanalDigital[6];   //Global pre value for non_CanalDigital value
    uint8_t CurValue_non_CanalDigital[6];   //Global current value for non_CanalDigital value
#endif /* CC_PROJECT_FOR_SONY */
} sonysat_demod_ctx_t;

static sonysat_demod_ctx_t g_sonysat_demod_ctx;

#ifdef CC_PROJECT_FOR_SONY
static BOOL is_canal_digital_ts;        //jk.xie
#endif /* CC_PROJECT_FOR_SONY */

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static const char* sonysat_FormatResult(sonysat_result_t result);
static  INT32 DVBS_AutoScanCallback(UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec);
static sonysat_singlecable_bank_t get_bank(TUNER_UNICABLE_BANK_T e_bank);
static sonysat_singlecable_ub_slot_t get_userband(TUNER_USER_BAND_T e_userband);
EXTERN INT32 Tuner_Notify(DRV_COND_T e_nfy_cond,UINT32 u4Msg);
EXTERN VOID  DVBS_TunerConnect(void);
extern  UINT8       u1DbgLevel; //add by ray.wang 20120614

#ifndef CC_PROJECT_FOR_SONY
#define Unicable_collision
#ifdef Unicable_collision
EXTERN UINT32 PDWNC_ReadServoADCChannelValue (UINT32 u4Channel);
static BOOL BlindScan_MidWare_Break (void);
static void Unicable_ChgChnDelay (UINT8 UserBandIdx);
static UINT16 DVBS_GetUnicableDelay (void);

/*
 * Check Unicable state
 * Return 1:Free, 0:Busy
 */
UINT8 sonysat_DVBS_GetUnicableState (void)
{
    UINT8 u1Ret=0;
    UINT8 uCount =0;
    UINT16 u2Delay = 0;
    UINT32 busy_Value=150;
    mcDBG_MSG_L2(("Get_Unicable_state\n"));

    /* Repeat time */
    while (uCount < 5)
    {
        /* Busy flag */
        u1Ret = 0;
        if (do_blindscan && BlindScan_MidWare_Break())
        {
            mcDBG_MSG_L2(("[BlindScan]Break 1!\n"));
            break;
        }
        Unicable_ChgChnDelay (pPdCtxDVBS->t_UniCab_info.e_user_band);  // For P calculate
        mcDBG_MSG_L2(("PDWNC_ReadServoADCChannelValue=%d\n",PDWNC_ReadServoADCChannelValue(4)));
        if (PDWNC_ReadServoADCChannelValue(4) > busy_Value) //  check unicable state( busy or free)
        {
            mcDBG_MSG_L2(("[BlindScan]Delay1!\n"));
            u2Delay = DVBS_GetUnicableDelay ();    // Get P value
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
#endif /* (NOT) CC_PROJECT_FOR_SONY */

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

static void DVBS_OduChannelChg(TUNER_UNICABLE_CC_T *pUniCabinfo)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_diseqc_message_t message;
    sonysat_singlecable_ub_slot_t userband;
    sonysat_singlecable_bank_t bank;
    UINT32 centerFreqKHz = 0;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    mcDBG_MSG_L2(("[%s]\n",__FUNCTION__));
    mcDBG_MSG_L2(("Ft=%dMHz,Fo=%dMHz,Fu=%dMHz,UB=%d,Bank=%d\n",\
    pUniCabinfo->ui4_transponder_freq,pUniCabinfo->ui2_lnb_freq,pUniCabinfo->ui2_band_freq,pUniCabinfo->e_user_band,pUniCabinfo->e_bank));
    //S=TUNER_CODING_STEP_SIZE in u_tuner.h
    if(pUniCabinfo->ui4_transponder_freq==0)
    {
        mcDBG_MSG_L2(("Ft is 0MHz,do not do channel change\n"));
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

static void DVBS_OduPowerOff(DVBS_PD_CTX_T * pdCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_diseqc_message_t message;
    sonysat_singlecable_ub_slot_t userband;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    uint8_t tslock = 0;
    int retrycnt = 0;
    int checkcnt = 0;

#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    BOOL b_P10=0;
#endif
    mcDBG_MSG_L2(("[%s]UB=%d\n",__FUNCTION__,pdCtx->t_UniCab_info.e_user_band));

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
        if(g_unicable_p == 0)
        {
            g_unicable_p = 0x3FF&(((userband+1)<<6)|((userband+1)<<1)); // P = p[9:0], P[4:1] = userband+1, P[9:6] = userband+1,
        }
        //calc P10
        if(((g_unicable_p&0x80)>>7)^(g_unicable_p&0x1))
        {
            b_P10 = 1;//P10 = 1
        }
        else
        {
            b_P10 = 0;
        }
        //shift p
        g_unicable_p >>=1;
        g_unicable_p |= (b_P10<<9); //Add P10
        mcDBG_MSG_L2(("ODU power Off g_unicable_p = %d\n",g_unicable_p));
    }

    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pInteg->pDemod, &tslock);
    if(sonysat_result == SONYSAT_RESULT_OK){
        if(tslock == 1){
            mcDBG_MSG_L2(("TS lock, goto check ODU power off status.\n"));//change 7--> 4
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
                    if(((g_unicable_p&0x80)>>7)^(g_unicable_p&0x1))
                    {
                        b_P10 = 1;//P10 = 1
                    }
                    else
                    {
                        b_P10 = 0;
                    }
                    if(retrycnt == 0)
                    {
                        g_unicable_p = 0x1FF&(((userband+1)<<5)|(userband+1)); // P = p[9:0], P[3:0] = userband+1, P[8:5] = userband+1,
                    }
                    else
                    {
                        //shift p
                        g_unicable_p >>=1;
                    }
                    g_unicable_p |= (b_P10<<9); //Add P10
                    mcDBG_MSG_L2(("TS still lock.(%d).Delay(0x%x-%d)ms\n", retrycnt, g_unicable_p, (g_unicable_p*1000)/1024));
                    SONYSAT_SLEEP((g_unicable_p*1000)/1024);
                }
            }
            if(tslock == 0){
                mcDBG_MSG_L2(("Stopped TS(%d - %d)\n", retrycnt, checkcnt));
            } else {
                mcDBG_MSG_L2(("Give up!! TS can't be stopped.\n"));
            }
        } else {
            mcDBG_MSG_L2(("TS is NOT locked\n"));
            //Send ODU_PowerOFF command
            sonysat_result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &message);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_ERR(("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand : %s\n", sonysat_FormatResult(sonysat_result)));
            }
        }
    } else if((sonysat_result == SONYSAT_RESULT_ERROR_SW_STATE) && (pInteg->pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S)){
        mcDBG_MSG_L2(("NOT tuned\n"));
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

static void DVBS_TunerDiscSCIF(DVBS_PD_CTX_T * pdCtx)
{
    //TODO_Unicable:Please add your implemention to ensure the specified user band of the SCIF is powered off.We need it to avoid the traffic conflict
    //Here is just a simple example without any confirmation
    DVBS_OduPowerOff(pdCtx);
}

static BOOL DVBS_ValidateMwPara(TUNER_SAT_DIG_TUNE_INFO_T * pt_tuner_info,TUNER_CONNECTION_MODE_T e_conn_mode)
{
    UINT32 ui4_start_freq,ui4_end_freq,ui4_freq;
    BOOL fgRet=TRUE;

    switch(e_conn_mode)
    {
        case CONN_MODE_SCAN:
            ui4_start_freq=pt_tuner_info->ui4_start_freq;
            ui4_end_freq=pt_tuner_info->ui4_end_freq;
            if((ui4_start_freq>=ui4_end_freq) || (ui4_start_freq<940) || (ui4_end_freq>2160))
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
                if((ui4_start_freq>=ui4_end_freq) || ui4_start_freq<940 || ui4_end_freq>2160 || pUniCabinfo->ui2_band_freq<950 || pUniCabinfo->ui2_band_freq>2150)
                {
                    mcDBG_MSG_ERR(("frequency start %d MHz end %dMHz band freq %dMHz,out of range [940,2160]MHz.\n",ui4_start_freq,ui4_end_freq,pUniCabinfo->ui2_band_freq));
                    fgRet=FALSE;
                }
            }
            break;

        case CONN_MODE_UNICABLE_SURF:
        case CONN_MODE_UNKNOWN://channel change
        case CONN_MODE_SURF://preset scan ,single tp scan
            ui4_freq=pt_tuner_info->ui4_freq;
            if(ui4_freq<940 || ui4_freq>2160)
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

static void Unicable_ChgChnDelay(UINT8 UserBandIdx)
{
    UINT16 bNewMSB;;

    if(UserBandIdx != g_PreUserBandIdx)
    {
        g_PreUserBandIdx = UserBandIdx;
        g_UnicableDelay = 0;
        UserBandIdx++;
        g_UnicableDelay |= UserBandIdx;
        g_UnicableDelay |= (UserBandIdx << 5);
    }
    else
    {
        //new p10 = p8 xor p1
        bNewMSB = ((g_UnicableDelay << 2) ^ (g_UnicableDelay << 9)) & 0x0200;
        g_UnicableDelay = (g_UnicableDelay >> 1);
        g_UnicableDelay |= bNewMSB;
    }
}
static UINT16 DVBS_GetUnicableDelay(void)/// return ms
{
    UINT16 u2Delay=0;
    mcDBG_MSG_L4(("current unicable monitorprocess delay is %d ms\n",((g_UnicableDelay*1000)/1024) ));
    if(g_UnicableDelay)
    {
        u2Delay=((UINT32)(g_UnicableDelay*1000))/1024;// P*1000 sec to ms
    }
    else
    {
        u2Delay=500;//if delay is 0,set as 500ms for loop forever;
    }
    if(u2Delay<500)u2Delay=500;//d121202_Haibo:Limit the delay to 500ms to release CPU resource to other thread
    return u2Delay;
}

static void Demod_GPIO_Reset(void)
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
    GPIO_SetOut(u4ResetGpionum,1);
    mcDELAY_MS(500);
    mcDBG_MSG_L2(("Reset demod  via %d\n",u4ResetGpionum));
#else
    mcDBG_MSG_L2(("Do not need reset DVBS DEMOD"));
#endif
}

static BOOL BlindScan_MidWare_Break(void)
{
    return pPdCtxDVBS->fgBreakMonitor;
}

//-----------------------------------------------------------------------------
// Sony specific
//-----------------------------------------------------------------------------
// This function is used in DtdCommand function
STATIC UINT32 sonysat_strtoul(const char* str)
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

static UINT32 sonysat_GetBER(void)
{
    UINT32 ber = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
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
            mcDBG_MSG_L1(("-OK S BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("-NG S BER\n"));
        }
        break;
    case SONYSAT_DTV_SYSTEM_DVBS2:
        sonysat_result = sonysat_demod_dvbs2_monitor_PreBCHBER(pDemod, (uint32_t*)&ber); /* 1e9 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 5000) / 10000;
            mcDBG_MSG_L1(("-OK S2 BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("-NG S2 BER\n"));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L1(("-NG def BER\n"));
        break;
    }

    return ber;
}

static UINT32 sonysat_GetPreBER(void)
{
    UINT32 ber = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
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
            mcDBG_MSG_L1(("-OK S BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("-NG S BER\n"));
        }
        break;
    case SONYSAT_DTV_SYSTEM_DVBS2:
        sonysat_result = sonysat_demod_dvbs2_monitor_PreLDPCBER(pDemod, (uint32_t*)&ber); /* 1e7 */
        if(sonysat_result == SONYSAT_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L1(("-OK S2 BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L1(("-NG S2 BER\n"));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L1(("-NG def BER\n"));
        break;
    }

    return ber;
}

static UINT32 sonysat_GetPER(void)
{
    uint32_t per = 0;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;

    sonysat_result = sonysat_demod_dvbs_s2_monitor_PER (pDemod, &per);

    if(sonysat_result == SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("PER (1e6) : %d\n", per));
    }else{
        mcDBG_MSG_L2(("Error in sonysat_demod_dvbs_s2_monitor_PER (%s)\n", sonysat_FormatResult(sonysat_result)));
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
    //uint8_t quality = 0;
    //uint32_t s2ber = 0;
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

static UINT16 sonysat_GetUEC(VOID)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
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
static UINT8 sonysat_GetSSI(VOID)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
    uint32_t ifagcOut = 0;
    int32_t agcLevel = 0;
    int32_t i4Temp=0;
    INT32 RSSI = 0;
    UINT8 tsLock = 0;

    sonysat_result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &tsLock);
    if(sonysat_result == SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("TS Lock : %d\n", tsLock));
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
    mcDBG_MSG_L1(("RSSI is %d\n", RSSI));
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
    mcDBG_MSG_L1(("SSI is %d\n", i4Temp));
    return (uint8_t)i4Temp;
}

static void DVBS_TunerDemodClock(UINT8 demod_clock)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
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

#ifdef CC_PROJECT_FOR_SONY
void sonysat_set_eeprom_non_canaldigital(UINT8 *pValue)
{
    UINT32 u4Ofst[6] ={0};
    u4Ofst[0] = eep_Non_Canal_Digital_Address_1;
    u4Ofst[1] = eep_Non_Canal_Digital_Address_2;
    u4Ofst[2] = eep_Non_Canal_Digital_Address_3;
    u4Ofst[3] = eep_Non_Canal_Digital_Address_4;
    u4Ofst[4] = eep_Non_Canal_Digital_Address_5;
    u4Ofst[5] = eep_Non_Canal_Digital_Address_6;
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[0], (UINT32)&pValue[0], 1));
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[1], (UINT32)&pValue[1], 1));
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[2], (UINT32)&pValue[2], 1));
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[3], (UINT32)&pValue[3], 1));
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[4], (UINT32)&pValue[4], 1));
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst[5], (UINT32)&pValue[5], 1));
}

void sonysat_get_eeprom_non_canaldigital(UINT8   *pValue)
{
    UINT32  u4Ofst[6] ={0};
    u4Ofst[0] = eep_Non_Canal_Digital_Address_1;
    u4Ofst[1] = eep_Non_Canal_Digital_Address_2;
    u4Ofst[2] = eep_Non_Canal_Digital_Address_3;
    u4Ofst[3] = eep_Non_Canal_Digital_Address_4;
    u4Ofst[4] = eep_Non_Canal_Digital_Address_5;
    u4Ofst[5] = eep_Non_Canal_Digital_Address_6;
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[0], (UINT32)(void *)&pValue[0], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[1], (UINT32)(void *)&pValue[1], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[2], (UINT32)(void *)&pValue[2], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[3], (UINT32)(void *)&pValue[3], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[4], (UINT32)(void *)&pValue[4], 1));
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst[5], (UINT32)(void *)&pValue[5], 1));
}

void sonysat_set_flag_non_canaldigital(UINT8 u1_flag)
{
    UINT32  u4Ofst =eep_Non_Canal_Digital_Address_7;
    VERIFY(0 == EEPROM_Write((UINT64)u4Ofst, (UINT32) &u1_flag, 1));
}

UINT8 sonysat_get_flag_non_canaldigital(void)
{
    UINT32  u4Ofst =eep_Non_Canal_Digital_Address_7;
    UINT8   ui_Value = 0;
    VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&ui_Value, 1));
    return ui_Value;
}

static sonysat_result_t sonysat_set_demod_non_canaldigital(UINT8 *pParameter)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    UINT8 u1_Temp = pParameter[5];

    SONYSAT_TRACE_ENTER("sonysat_set_demod_non_canaldigital");
    mcDBG_MSG_L2(("b_is_canal_digital : %d\n", is_canal_digital_ts));
    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, ((is_canal_digital_ts == TRUE) ? 0x01 : pParameter[0])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA8 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA8) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB4, ((is_canal_digital_ts == TRUE) ? 0x01 : pParameter[1])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA6 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA6) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6C, ((is_canal_digital_ts == TRUE) ? 0x7C : pParameter[2])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, ((is_canal_digital_ts == TRUE) ? 0x7C : pParameter[3])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, ((is_canal_digital_ts == TRUE) ? 0x7C : pParameter[4])) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA6 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA6) != SONYSAT_RESULT_OK){
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x73, (((is_canal_digital_ts == TRUE) || (u1_Temp != 0x00)) ? 0x7C : 0xCE)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA8 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA8) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, (((is_canal_digital_ts == TRUE) || (u1_Temp != 0x00)) ? 0x73 : 0x93)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4C, (((is_canal_digital_ts == TRUE) || (u1_Temp != 0x00)) ? 0x73 : 0xC3)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x64, (((is_canal_digital_ts == TRUE) || (u1_Temp != 0x00)) ? 0x73 : 0xA3)) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    return sonysat_result;
}


VOID sonysat_init_eeprom_non_canaldigital(void)
{
    UINT8 pPreVal[6] = {0x00, 0x00, 0x7E, 0xCE, 0x9E, 0x00};
    UINT8 pValue[6] = {0};
    if(sonysat_get_flag_non_canaldigital() != 0xAA)     //Registers have been written to Demod
    {
        sonysat_set_demod_non_canaldigital(pPreVal);
        sonysat_set_eeprom_non_canaldigital(pPreVal);
        //sonysat_set_flag_non_canaldigital(0x55);    //0x55 means Registers are default values
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[0] = 0x00;
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[1] = 0x00;
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[2] = 0x7E;
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[3] = 0xCE;
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[4] = 0x9E;
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[5] = 0x00;

        g_sonysat_demod_ctx.CurValue_non_CanalDigital[0] = 0x00;
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[1] = 0x00;
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[2] = 0x7E;
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[3] = 0xCE;
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[4] = 0x9E;
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[5] = 0x00;
    }
    else
    {
        sonysat_get_eeprom_non_canaldigital(pValue);
        sonysat_set_demod_non_canaldigital(pValue);
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[0] = pValue[0];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[1] = pValue[1];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[2] = pValue[2];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[3] = pValue[3];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[4] = pValue[4];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[5] = pValue[5];

        g_sonysat_demod_ctx.CurValue_non_CanalDigital[0] = pValue[0];
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[1] = pValue[1];
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[2] = pValue[2];
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[3] = pValue[3];
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[4] = pValue[4];
        g_sonysat_demod_ctx.CurValue_non_CanalDigital[5] = pValue[5];
    }
}
sonysat_result_t sonysat_set_ts_maxfreq(DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    uint8_t ui1_ts_freq = 0;

    if(pParameter->t_ts_maxfreq.b_setvalid == TRUE)
    {
        ui1_ts_freq = (uint8_t)pParameter->t_ts_maxfreq.ui1_ts_maxfreq;
        if(ui1_ts_freq > 3)
        {
            ui1_ts_freq = 0;
        }

        ui1_ts_freq += 8;
        mcDBG_MSG_L1(("Set TS Max freq: %d\n",ui1_ts_freq));
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
#endif /* CC_PROJECT_FOR_SONY */

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

    /* Check break from MTK uppder layer here!! */
    if(do_blindscan){
        // Blind scan case
        if(BlindScan_MidWare_Break()){
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_CANCEL);
        }
    }else if(g_sonysat_demod_ctx.breakFct){
        if(g_sonysat_demod_ctx.breakFct(g_sonysat_demod_ctx.breakFctArg)){
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_CANCEL);
        }
    }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

//-----------------------------------------------------------------------------
// DiSEqC command handling (Sony specific)
//-----------------------------------------------------------------------------
static BOOL sonysat_DiseqcCommand(DISEQC_BUS_CMD_T *pCmd, uint8_t repeatCount)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    int32_t lnbcConfigVoltageLow = 0;
    int32_t lnbcConfigVoltageHigh = 0;
#ifdef Debug_time
    HAL_TIME_T TimeStart,TimeEnd,TimeDst;
    HAL_GetTime(&TimeStart);
#endif

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    // For testing without LNBC
    if(pInteg->pLnbc == NULL){
        mcDBG_MSG_L1(("No LNBC!\n"));
        return 1;
    }

    DVBS_TunerConnect();

    if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK){
        /* For UniCable sequence */
        mcDBG_MSG_L2(("[%s]ODU_CHANNEL_CHANGE\n",__FUNCTION__));
        pPdCtxDVBS->fgUniCab = TRUE; /* Set unicable flag */
        x_memcpy(&pPdCtxDVBS->t_UniCab_info, &pCmd->t_unicable_info, sizeof(TUNER_UNICABLE_CC_T));
        DVBS_OduChannelChg(&pCmd->t_unicable_info);
    } else {
        /* Not UniCable sequence */
        pPdCtxDVBS->fgUniCab = FALSE; /* Reset unicable flag */
        if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_LNB_POWER_MASK){

            switch(pCmd->ui1_lnb_power){
            case 0: // Power Off
                mcDBG_MSG_L1(("set lnb power off\n"));
                sonysat_result = pLnbc->Sleep(pLnbc);
                break;
            case 1: // Normal
#if defined(SONYSAT_LNBC_MPS_MP8126)
                lnbcConfigVoltageLow = MPS_MP8126_CONFIG_VOLTAGE_LOW_13V;
                lnbcConfigVoltageHigh = MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V;
#elif defined(SONYSAT_LNBC_ST_LNBH29)
                lnbcConfigVoltageLow = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V;
                lnbcConfigVoltageHigh = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
                lnbcConfigVoltageLow = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V;
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
#if defined(SONYSAT_LNBC_MPS_MP8126)
                lnbcConfigVoltageLow = MPS_MP8126_CONFIG_VOLTAGE_LOW_14V;
                lnbcConfigVoltageHigh = MPS_MP8126_CONFIG_VOLTAGE_HIGH_19V;
#elif defined(SONYSAT_LNBC_ST_LNBH29)
                lnbcConfigVoltageLow = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_667V;
                lnbcConfigVoltageHigh = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_817V;
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
                lnbcConfigVoltageLow = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_14_333V;
                lnbcConfigVoltageHigh = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_19_667V;
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
                mcDBG_MSG_L1(("set lnb power on\n"));
                if(pLnbc->state == SONYSAT_LNBC_STATE_SLEEP){
                    // Need to wakeup!!
                    sonysat_result = pLnbc->WakeUp(pLnbc);
                    mcDBG_MSG_L1(("Waiting for DiSEqC switch stabilization.\n"));
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
                    mcDBG_MSG_L1(("set 18V\n"));
                    isVoltageHigh = 1;
                }
                else
                {
                    mcDBG_MSG_L1(("set 13V\n"));
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

                mcDBG_MSG_L1(("DiSEqC command exist\n"));
                isDiSEqC = 1;

                cmdlength = pCmd->ui1_cmd_len;
                if(cmdlength > SONYSAT_DISEQC_MESSAGE_LENGTH){
                    cmdlength = SONYSAT_DISEQC_MESSAGE_LENGTH;
                }

                if(cmdlength == 0){
                    mcDBG_MSG_L1(("DiSEqC command enabled but command data is none.\n"));
                    isDiSEqC = 0;
                }else{
                    diseqcCommand.length = cmdlength;
                    for(i = 0; i < cmdlength; i++){
                        diseqcCommand.data[i] = pCmd->aui1_cmd[i];
                        mcDBG_MSG_L1(("0x%02x  ", pCmd->aui1_cmd[i]));
                    }
                }
            }else{
                // Disable DiSEqC command
                mcDBG_MSG_L1(("No DiSEqC command\n"));
                isDiSEqC = 0;
            }

            if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_TONE_BURST_MASK){
                if(1==pCmd->ui1_tone_burst) {
                    mcDBG_MSG_L1(("Tone burst 1\n"));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_B;
                } else if(0==pCmd->ui1_tone_burst) {
                    mcDBG_MSG_L1(("Tone burst 0\n"));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_A;
                } else {
                    mcDBG_MSG_L1(("Tone burst off\n"));
                    toneburst_mode = SONYSAT_TONEBURST_MODE_OFF;
                }
            }
            if((pCmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) && (pCmd->b_22k)){
                mcDBG_MSG_L1(("22k tone on\n"));
                isToneOn = 1;
            }else{
                mcDBG_MSG_L1(("22k tone off\n"));
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
    mcDBG_MSG_L1(("[sonysat_HandleDiseqcCommand] time:%ld.\n", TimeDst.u4Micros));
#endif

    return 1; // Success
}

static BOOL sonysat_HandleDiseqcCommand(DISEQC_BUS_CMD_T *pCmd)
{
    return sonysat_DiseqcCommand(pCmd, 2);
}

static BOOL sonysat_HandleResetCommand(DISEQC_BUS_CMD_T *pCmd)
{
    return sonysat_DiseqcCommand(pCmd, 1);
}

static void Vendor_ResendDiseqcCmmd(DISEQC_BUS_CMD_T *diseqc_cmd)
{
    sonysat_HandleDiseqcCommand(diseqc_cmd);
}

//-----------------------------------------------------------------------------
// Blindscan implementation (Sony specific)
//-----------------------------------------------------------------------------
static void sonysat_blindscan_callback (sonysat_integ_t * pInteg,
                                     sonysat_integ_dvbs_s2_blindscan_result_t * pResult)
{
    UINT8 voltage = POL_UNKNOWN;
    // This callback function is called from Sony internal driver code.
    if(!pInteg || !pResult){
        return;
    }

    if(pResult->eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_DETECT){
        // DVB-S/S2 signal was found.
        mcDBG_MSG_L2(("========== Signal Found!! ==========\n"));
        if(pResult->tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS){
            mcDBG_MSG_L2(("System : DVB-S\n"));
        }else if(pResult->tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS2){
            mcDBG_MSG_L2(("System : DVB-S2\n"));
        }else{
            mcDBG_MSG_L2(("System : Unknown\n"));
        }
        mcDBG_MSG_L2(("Frequency (kHz) : %d\n", pResult->tuneParam.centerFreqKHz));
        mcDBG_MSG_L2(("Symbol rate (ksps) : %d\n", pResult->tuneParam.symbolRateKSps));

        // Save signal information to global.
        x_memcpy(&g_sonysat_demod_ctx.tunedSignal, &pResult->tuneParam, sizeof(sonysat_dvbs_s2_tune_param_t));
        // Callback
#ifndef SONYSAT_DUMMY_DEBUG
        if(pInteg->pLnbc != NULL)
        {
            voltage = (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH ? POL_LIN_HORIZONTAL : POL_LIN_VERTICAL);
        }
        DVBS_AutoScanCallback(1,
            voltage,
            pResult->tuneParam.centerFreqKHz,
            pResult->tuneParam.symbolRateKSps,
            FEC_I_UNKNOWN);
#endif
    }else if(pResult->eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_PROGRESS){
        // Progress was updated.
        mcDBG_MSG_L2(("Blindscan progress : %d\n", pResult->progress));
        g_sonysat_demod_ctx.blindscanProgress = pResult->progress;

        // Progress update callback
#ifndef SONYSAT_DUMMY_DEBUG
        DVBS_AutoScanCallback(0, 0, 0, 0, 0);
#endif
    }

    if(BlindScan_MidWare_Break()){
        sonysat_integ_Cancel(pInteg); // Set cancel flag to exit blindscan.
    }
}

#ifdef CC_PROJECT_FOR_SONY
extern void sony_cxd2837_Semalock(BOOL lock);

uint8_t sonysat_GetLnbShort(void)
{
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    UINT8 u1OverLoad=0;

    if(pInteg->pLnbc == NULL)
    {
        return 0; //Error Return 0
    }

    if(pLnbc->state == SONYSAT_LNBC_STATE_OVERLOAD)
    {
        u1OverLoad = 1; //Overload return 1
    }
    else
    {
        sony_cxd2837_Semalock(1);
        if(SONYSAT_RESULT_OK != pLnbc->MonitorOverload(pLnbc,&u1OverLoad))
        {
            u1OverLoad = 0;//previous value
        }
        if(u1OverLoad)
        {
            mcDBG_MSG_L4(("lnb overload power off\n"));
            pLnbc->Sleep(pLnbc);
            pLnbc->state = SONYSAT_LNBC_STATE_OVERLOAD;
        }
        sony_cxd2837_Semalock(0);
    }

    mcDBG_MSG_L4(("LNB over load state = %d\n", u1OverLoad));
    return (u1OverLoad);
}
#endif /* CC_PROJECT_FOR_SONY */


#ifndef SONYSAT_DUMMY_DEBUG
static VOID Vendor_BlindScan(void)
#else
VOID Vendor_BlindScan(void) // ONLY for debug
#endif
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_demod_dvbs_s2_blindscan_seq_t *pSeq;
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
    blindscan_param.minFreqKHz = bs_start_freq * 1000; // kHz -> MHz
    blindscan_param.maxFreqKHz = bs_stop_freq * 1000; // kHz -> MHz
    blindscan_param.minSymbolRateKSps = 1000;  // From 1Msps
    blindscan_param.maxSymbolRateKSps = 45000; // To 45Msps

    if(Util_GetConnMode(pPdCtxDVBS, CONN_MODE_SCAN) == CONN_MODE_UNICABLE_SCAN){
        // Unicable
        sonysat_result = sonysat_integ_sat_device_ctrl_DisableSinglecable(pInteg);
        if(sonysat_result != SONYSAT_RESULT_OK){
            mcDBG_MSG_L2(("[Vendor_BlindScan] Error at sonysat_integ_sat_device_ctrl_DisableSinglecable\n"));
        }
        sonysat_result = sonysat_integ_sat_device_ctrl_EnableSinglecable(pInteg, &g_sonysat_demod_ctx.singlecableData, NULL);
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
        mcDBG_MSG_L2(("[Vendor_BlindScan] Blindscan success.\n"));
    }else{
        mcDBG_MSG_L2(("[Vendor_BlindScan] Blindscan failed. (%s)\n", sonysat_FormatResult(sonysat_result)));
    }

    // Debug information for memory management
    mcDBG_MSG_L2(("(Blindscan info) Max Used Count : %d\n", pSeq->commonParams.storage.maxUsedCount));
    mcDBG_MSG_L2(("(Blindscan info) Current Used Count : %d\n", pSeq->commonParams.storage.currentUsedCount));
    mcDBG_MSG_L2(("(Blindscan info) Max Used Power Count : %d\n", pSeq->commonParams.storage.maxUsedPowerCount));

    mcDBG_MSG_L1(("[Vendor_BlindScan] end.!\n"));
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

static  INT32 DVBS_AutoScanCallback(UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec)
{
    //mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    if(BlindScan_MidWare_Break())
    {
        mcDBG_MSG_L1(("[DVBS_AutoScanCallback] MD has been broken....\n"));
        //mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
        return 0;
    }

    while(1)
    {
        if(x_sema_lock_timeout(AutoScan_callback,10)==OSR_OK)
        {
            mcDBG_MSG_L1(("[DVBS_AutoScanCallback] reset AutoScan_callback...\n"));
            continue;
        }
        else
        {
            break;
        }
    }
    if (signal==1)
    {
        mcDBG_MSG_L1(("[DVBS_AutoScanCallback] TP_Locked\n"));
        Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_GOT_SIGNAL);
    }
    else if(signal==0)
    {
        mcDBG_MSG_L1(("[DVBS_AutoScanCallback]  Next Band Request\n"));
        Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_LOST_SIGNAL);
    }
    else if(signal==2)
    {
        mcDBG_MSG_L1(("[DVBS_AutoScanCallback]  AUTOSCAN_END\n"));
        Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_AUTOSCAN_END);   //at the end of blind scan
    }

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    Monitor_waiting_semaphore_flag = BOOL_TRUE;
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    //x_sema_lock_timeout(AutoScan_callback,8000);//liuyuan debug use
    mcSEMA_LOCK_MTFE(AutoScan_callback);
    mcDBG_MSG_L1(("[DVBS_AutoScanCallback]  Got Semphore..\n"));
    Monitor_waiting_semaphore_flag = BOOL_FALSE;

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
            mcDBG_MSG_L1(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
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
    DVBS_PD_CTX_T *psPdCtx = *(DVBS_PD_CTX_T **) pvArg;
    mcDBG_MSG_L2(("MonitorProcess start!\n"));

    while (psPdCtx->fgMonitorThreadRunning)
    {
        //  Snr = 0;
        if (psPdCtx->fgBreakMonitor)
        {
            // After mcMUTEX_LOCK(t_escape_mon_suspend), monitor thread will be suspended until
            // TunerAcq or TunerClose invoked.
            mcDBG_MSG_L1(("Entering DVBS mon_suspend\n"));
            mcSEMA_LOCK_MTFE(psPdCtx->t_escape_mon_suspend);
            //mcMUTEX_LOCK(psPdCtx->t_escape_mon_suspend);
            mcDBG_MSG_L1(("Escape DVBS mon_suspend\n"));
        }
        else
        {
            // mutex lock for I2C access
            //mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
            //psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
            //! [TODO:PI] put your monitor process code here
            if(do_blindscan == 1)
            {
                psPdCtx->fgPIMoniStatus = TRUE;
                // update the frequency before blind scan
                g_sonysat_demod_ctx.tunedSignal.centerFreqKHz = (bs_start_freq * 1000);
                Vendor_BlindScan();
                psPdCtx->fgPIMoniStatus = FALSE;
                do_blindscan = 0;
                // Here, demod is in sleep state.
                // update the final frequency before notify end
                g_sonysat_demod_ctx.tunedSignal.centerFreqKHz = (bs_stop_freq * 1000) - 500;
                g_sonysat_demod_ctx.tunedSignal.symbolRateKSps = 0;
                DVBS_AutoScanCallback(2, 0, 0, 0, 0);
            }
            else
            {
                mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
                if(g_set_flag==1)
                {
                   uNcount=0;
                   g_set_flag=0;
                }
                if(g_channel_locked_flag==0)
                {
                    uNcount++;
                    if(uNcount>=3)
                    {
                        uNcount=0;
                        Vendor_ResendDiseqcCmmd(&g_diseqc_cmd);
                    }
                }
                else
                {
                    uNcount=0;
                }
                mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock);
            }
            x_thread_delay((UINT32)(500));
        }
    }

    //psPdCtx->t_monitor_thread = NULL;
    psPdCtx->t_monitor_thread = 0;
    mcDBG_MSG_L1(("MonitorProcess exit!\n"));
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
    psPdCtx->fgMonitorThreadRunning = TRUE;
    if(x_thread_create(&psPdCtx->t_monitor_thread,
        "DVBS_MonitorThread",
        cTHREAD_STACK_SIZE*10,cTHREAD_PRIORITY,
        MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
    {
        mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
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
static INT32 PD_StopMonitorEngine(void)
{
    while (pPdCtxDVBS->t_monitor_thread){
        x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_L2(("DVBS PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

#ifdef CC_PROJECT_FOR_SONY
/************************************************************************
*  DVBS_Set_Demod_non_CanalDigital
*  Set Demod for non_CanalDigital service.
************************************************************************/

VOID DVBS_Set_Demod_non_CanalDigital(void)
{
    UINT8 pValue[6] = {0};

    if((g_sonysat_demod_ctx.CurValue_non_CanalDigital[0] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[0]) &&
       (g_sonysat_demod_ctx.CurValue_non_CanalDigital[1] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[1]) &&
       (g_sonysat_demod_ctx.CurValue_non_CanalDigital[2] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[2]) &&
       (g_sonysat_demod_ctx.CurValue_non_CanalDigital[3] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[3]) &&
       (g_sonysat_demod_ctx.CurValue_non_CanalDigital[4] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[4]) &&
       (g_sonysat_demod_ctx.CurValue_non_CanalDigital[5] == g_sonysat_demod_ctx.PreValue_non_CanalDigital[5]))
    {
        return;
    }
    else
    {
        pValue[0] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[0];
        pValue[1] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[1];
        pValue[2] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[2];
        pValue[3] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[3];
        pValue[4] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[4];
        pValue[5] = g_sonysat_demod_ctx.CurValue_non_CanalDigital[5];
        sonysat_set_demod_non_canaldigital(pValue);
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[0] = pValue[0];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[1] = pValue[1];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[2] = pValue[2];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[3] = pValue[3];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[4] = pValue[4];
        g_sonysat_demod_ctx.PreValue_non_CanalDigital[5] = pValue[5];
    }
}
#endif /* CC_PROJECT_FOR_SONY */

//-------------------------------------------------------------------------
/**
 *  This API do connect tuner
 *  @retval  void
 */
//-------------------------------------------------------------------------
 VOID  DVBS_TunerConnect(void)
{
    if(pPdCtxDVBS->fgDisStatus == FALSE)
    {
        /* Has connected, return */
        mcDBG_MSG_L2(("DVBS Has connect!\n"));
        return;
    }
    //add demod driver implementation code here
    {
        UINT8 data = 0;
        sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
        sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;

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
            mcDBG_MSG_L2(("Switch to DVB satellite.\n"));
            sonysat_result = sonysat_demod_ReinitializeS(pDemod);
            if(sonysat_result != SONYSAT_RESULT_OK){
                mcDBG_MSG_ERR(("Error in sonysat_demod_ReinitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
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
static INT32 DVBS_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtxDVBS->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    x_sema_delete(pPdCtxDVBS->t_escape_mon_suspend);

    //add driver implementation code here
    x_sema_delete(pPdCtxDVBS->hHalLock);
    x_mem_free(pPdCtxDVBS);
    //x_mem_free(psDemodCtx);
    g_ui1_mount_status = 0x00;
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
static BOOL DVBS_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;//=(TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
    BOOL fgChannelScan = FALSE;
    BOOL fgLock = FALSE;
    UINT32 freqMHz = 0;
    UINT32 symbolRateKsps = 0;
    UINT16 uiLockStatus = 0;
    TUNER_CONNECTION_MODE_T e_conn_mode=CONN_MODE_UNKNOWN;

    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;


    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    if(pv_conn_info != NULL)
    {
        pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
    }
    else
    {
        mcDBG_MSG_L1(("[DVBS_TunerAcq] pv_conn_info is Err.\n"));
        return (fgLock);
    }

#ifdef CC_PROJECT_FOR_SONY
    is_canal_digital_ts = pt_tuner_info->b_canal_digital_ts;    //jk.xie
#endif /* CC_PROJECT_FOR_SONY */

    //set break function
    e_conn_mode=Util_GetConnMode(pPdCtxDVBS,pt_tuner_info->e_conn_mode);
    if(!DVBS_ValidateMwPara(pt_tuner_info,e_conn_mode))
    {
       mcDBG_MSG_ERR(("invalid parameter!!\n"));
       return fgLock;
    }
    pPdCtxDVBS->isBreak = _BreakFct;
    //mutex lock
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Pause monitor thread
    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        mcDBG_MSG_L2(("Pause monitor process!!\n"));
        SetBreakMonitor(pPdCtxDVBS);
    }

    DVBS_TunerConnect();

#ifdef CC_PROJECT_FOR_SONY
    DVBS_Set_Demod_non_CanalDigital();
#endif /* CC_PROJECT_FOR_SONY */

    //Parse parameter from upper layer
    freqMHz = pt_tuner_info->ui4_freq; // in MHz
    symbolRateKsps = pt_tuner_info->ui4_sym_rate; // in ksps

    // Save break function pointer to global
    g_sonysat_demod_ctx.breakFct = _BreakFct;
    g_sonysat_demod_ctx.breakFctArg = pvArg;

    //get conncetion mode
    switch(e_conn_mode)
    {
    case CONN_MODE_SCAN://this mode is for blindscan
    case CONN_MODE_UNICABLE_SCAN:
        {
            mcDBG_MSG_L1(("Start to BlindScan. Set start frequency value[%d, %d]MHz !\n",pt_tuner_info->ui4_start_freq,pt_tuner_info->ui4_end_freq));
                        /*before AVL_BlindScan()function finished First runnings scan ,MD will call DVBS_TunerGetSync for blindscan start freq.*/
            bs_start_freq = (UINT16)(pt_tuner_info->ui4_start_freq); // in MHz
            bs_stop_freq = (UINT16)(pt_tuner_info->ui4_end_freq);    // in MHz
            mcDBG_MSG_L1(("bs_stop_freq=%dMHz\n",bs_stop_freq));
            do_blindscan = 1;
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
                    &g_sonysat_demod_ctx.singlecableData,
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

                mcDBG_MSG_L2(("[DVBS_TunerAcq] Start auto symbol rate detect tune.\n"));
                mcDBG_MSG_L2(("Frequency : %d kHz\n", freqMHz * 1000));

                // sonysat_demod_dvbs_s2_blindscan_seq_t size is big, so allocate in heap.
                pSeq = (sonysat_demod_dvbs_s2_tune_srs_seq_t*) x_mem_alloc(sizeof(sonysat_demod_dvbs_s2_tune_srs_seq_t));
                if(pSeq == NULL){
                    // Oops... memory allocation failed...
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Error : Memory allocation for auto symbol rate tune was failed!!\n"));
                    uiLockStatus = 0;
                    goto ErrorExit;
                }

                tune_srs_param.centerFreqKHz = tuneParam.centerFreqKHz;
                tune_srs_param.freqRange = SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_FREQ_RANGE_10MHz;

                sonysat_result = sonysat_integ_dvbs_s2_TuneSRS(pInteg, pSeq, &tune_srs_param, &tuneParam);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Auto symbol rate tune success.\n"));
                    if(tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS){
                        mcDBG_MSG_L2(("System : DVB-S\n"));
                    }else if(tuneParam.system == SONYSAT_DTV_SYSTEM_DVBS2){
                        mcDBG_MSG_L2(("System : DVB-S2\n"));
                    }else{
                        mcDBG_MSG_L2(("System : Unknown\n"));
                    }
                    mcDBG_MSG_L2(("Frequency (kHz) : %d\n", tuneParam.centerFreqKHz));
                    mcDBG_MSG_L2(("Symbol rate (ksps) : %d\n", tuneParam.symbolRateKSps));
                    uiLockStatus = 1;
                }else{
                    mcDBG_MSG_L1(("[DVBS_TunerAcq] Auto symbol rate tune failed. (%s)\n", sonysat_FormatResult(sonysat_result)));
                    uiLockStatus = 0;
                }

                // Debug information for auto symbol rate tune
                mcDBG_MSG_L2(("(Auto symbol rate tune info) Max Used Count : %d\n", pSeq->commonParams.storage.maxUsedCount));
                mcDBG_MSG_L2(("(Auto symbol rate tune info) Current Used Count : %d\n", pSeq->commonParams.storage.currentUsedCount));
                mcDBG_MSG_L2(("(Auto symbol rate tune info) Max Used Power Count : %d\n", pSeq->commonParams.storage.maxUsedPowerCount));

                x_mem_free(pSeq); // Release memory
            }else{
                // Normal tuning
                mcDBG_MSG_L2(("[DVBS_TunerAcq] Start normal tune.\n"));
                mcDBG_MSG_L2(("Frequency : %d kHz, Symbol rate %d ksps\n", freqMHz * 1000, symbolRateKsps));

                sonysat_result = sonysat_integ_dvbs_s2_Tune(pInteg, &tuneParam);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Normal tune success.\n"));
                    uiLockStatus = 1;
                }else{
                    mcDBG_MSG_L2(("[DVBS_TunerAcq] Normal tune failed. (%s)\n", sonysat_FormatResult(sonysat_result)));
                    uiLockStatus = 0;
                }
            }

            // Save signal information to global.
            x_memcpy(&g_sonysat_demod_ctx.tunedSignal, &tuneParam, sizeof(sonysat_dvbs_s2_tune_param_t));

            break;
        }
    default:
        mcDBG_MSG_ERR(("Invalid mode to DVBS_TunerAcq!\n"));
        break;
    }

    if((CONN_MODE_SCAN==e_conn_mode) || (CONN_MODE_UNICABLE_SCAN==e_conn_mode))
    {
        uiLockStatus = 1;//scan mode should return LOCK Status.
    }

ErrorExit:

    // Clear break function pointer
    g_sonysat_demod_ctx.breakFct = NULL;
    g_sonysat_demod_ctx.breakFctArg = NULL;

    ui4_data_forMW = pt_tuner_info->ui4_data;
    if(uiLockStatus)
    {
        g_channel_locked_flag = 1;
        fgLock=1;
    }
    else
    {
        g_channel_locked_flag = 0;
        fgLock=0;
    }

    //-------------------------------------------------------------------------------------
    if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg) && (!fgChannelScan))
        {
            mcDBG_MSG_L2(("Enable monitor process!!\n"));
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
static BOOL DVBS_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    BOOL fgLock = FALSE;

    if(do_blindscan == 1)
    {
        mcDBG_MSG_L4(("[%s] doing blindscan.\n",__FUNCTION__));
        return TRUE;
    }
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
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
        g_channel_locked_flag = 1;
    }
    else
    {
        //mcDBG_MSG_L1(" %s ,unlock\n",__FUNCTION__);
        g_channel_locked_flag = 0;
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
static VOID  DVBS_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;

    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    //TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered

    pPdCtxDVBS->fgDisStatus = TRUE;

    if(Monitor_waiting_semaphore_flag == BOOL_TRUE)
    {
        mcDBG_MSG_L1(("[DVBS_TunerDisc]  111 set to stop Blindscan .\n"));
        x_sema_unlock(AutoScan_callback);
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
//END
    SetBreakMonitor(pPdCtxDVBS);

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    if(Monitor_waiting_semaphore_flag == BOOL_TRUE)
    {
        mcDBG_MSG_L1(("[DVBS_TunerDisc]  222 set to stop Blindscan .\n"));
        x_sema_unlock(AutoScan_callback);
    }

    if(pPdCtxDVBS->fgUniCab){
        DVBS_TunerDiscSCIF(pPdCtxDVBS);
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
static void DVBS_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_dtv_system_t system;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //apply signal code here

    if(_pSignal)
    {
        x_memset(_pSignal, 0, sizeof(SIGNAL));
        //return demod locked frequency(MHz) and symbol rate
        _pSignal->Frequency = (g_sonysat_demod_ctx.tunedSignal.centerFreqKHz + 500) / 1000;

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
static UINT8 DVBS_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{

    UINT8 u1Ret=0;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO Read If_AGC control word and mapping it to signal level(0~100)
    //update it as u1Ret
    u1Ret = sonysat_GetSSI();

    mcDBG_MSG_L2(("SSI : %d\n", u1Ret));
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
static INT16 DVBS_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT16 i2Ret=0;
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //ToDo:demod driver implemented
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return i2Ret;
}

static UINT32 DVBS_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret = 100000; // unit is 1e-5
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;

    if(do_blindscan == 1)
    {
        mcDBG_MSG_L1(("[%s] doing blindscan.\n",__FUNCTION__));
        return 0;
    }
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

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
static UINT16 DVBS_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret=0;
    int32_t cnr = 0;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;

    if(do_blindscan == 1)
    {
        mcDBG_MSG_L1(("[%s] doing blindscan.\n",__FUNCTION__));
        return 0;
    }
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

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

static void DVBS_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static char *DVBS_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    return (char *)(VENDOR_DRIVER_VER);
}

static INT32 DVBS_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    //-------------------------------------------------

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return 0;
}
static void DVBS_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}
static void DVBS_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
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
    }
    mcDBG_MSG_L1((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);

    switch (ucCmdId)
    {
    case 's':
        mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

        /* Current frequency and symbol rate */
        {
            mcDBG_MSG_L1(("Current Signal Frequency : %d.%03d\n",
                g_sonysat_demod_ctx.tunedSignal.centerFreqKHz / 1000, g_sonysat_demod_ctx.tunedSignal.centerFreqKHz % 1000));

            mcDBG_MSG_L1(("Tuner Tuned Frequency : %d.%03d\n",
                g_sonysat_demod_ctx.tuner.frequencyKHz / 1000, g_sonysat_demod_ctx.tuner.frequencyKHz % 1000));
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
            uint32_t ifagcOut = 0;
            int32_t agcLevel = 0;
            sonysat_result = sonysat_demod_dvbs_s2_monitor_IFAGCOut(pDemod, &ifagcOut);
            if(sonysat_result == SONYSAT_RESULT_OK){
                sonysat_result = pTuner->AGCLevel2AGCdB (pTuner, ifagcOut, &agcLevel);
                if(sonysat_result == SONYSAT_RESULT_OK){
                    mcDBG_MSG_L1(("RSSI (dBm) : %d\n", (INT8)(((agcLevel + 50) / 100) * (-1))));
                } else {
                    mcDBG_MSG_L1(("Error in pTuner->AGCLevel2AGCdB (%s)\n", sonysat_FormatResult(sonysat_result)));
                }
            }else{
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
        {
            UINT16 pen = sonysat_GetUEC();
            mcDBG_MSG_L1(("UEC : %d\n", pen));
        }

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
            /* Tuner Dump/Read/Write */
            const UINT8 tunerI2cAddress = 0xC0; /* Please fix it if tuner address is changed. */
            sonysat_i2c_t i2cTuner;

            /*sonysat_i2c_MTK_CreateI2c(&i2cTuner);*/
            /* If tuner is accessed by I2C Gateway */
            sonysat_i2c_MTK_CreateI2c(&i2cTuner);
//            sonysat_i2c_MTK_CreateI2cGw(&i2cTuner, pDemod->i2cAddressSLVT, 0x09);

            switch(aszArgv[0][1]){
            case 'd':
            default:
                {
                    UINT8 data[256];

                    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock); // Enter Critical Section
                    sonysat_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sonysat_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, 0x00, data, 256);
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
                    sonysat_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, subAddress, data, length);
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
                    sonysat_result = i2cTuner.WriteOneRegister(&i2cTuner, tunerI2cAddress, subAddress, value);
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
                UINT32 startFreqMHz = 0;
                UINT32 stopFreqMHz = 0;
                if(i4Argc < 3){
                    mcDBG_MSG_L1(("Error: start and stop frequency (MHz) are necessary.\n"));
                    return;
                }

                startFreqMHz = (UINT32)sonysat_strtoul(aszArgv[1]);
                stopFreqMHz = (UINT32)sonysat_strtoul(aszArgv[2]);

                bs_start_freq = startFreqMHz;
                bs_stop_freq = stopFreqMHz;

                if(pPdCtxDVBS->fgBreakMonitor)
                {
                    pPdCtxDVBS->fgBreakMonitor = FALSE;
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
                }
                do_blindscan = 1;
                mcDBG_MSG_L1(("Start blind scan.\n"));
            }
            break;
        case 'c':
            mcDBG_MSG_L1(("Continue to do blind scan...\n"));
            x_sema_unlock(AutoScan_callback);
            break;
        case 'q':
            pPdCtxDVBS->fgBreakMonitor=TRUE;
            mcDBG_MSG_L1(("Cancel blind scan\n"));
            break;
        default:
            mcDBG_MSG_L1(("Unknown command.\n"));
            break;
        }
        break;
    default:
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
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
static INT16 DVBS_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    TUNER_SAT_DIG_TUNE_INFO_T *pAttri0;
    TUNER_DIG_ATTRIBUTE_T *pAttri1;
    TUNER_SAT_DIG_LNB_STATUS_INFO_T *pAttri2;
    INT32 i4Temp = 0;

    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
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

    mcDBG_MSG_L4((" %s ,e_get_type:%d\n",__FUNCTION__,e_get_type));
    //add driver implementation code here
    switch (e_get_type)
    {
    case TUNER_GET_TYPE_SCAN_PROGRESS:
        {
            UINT8 u1ScanProg = g_sonysat_demod_ctx.blindscanProgress;//Please implement scan progres information
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
            pAttri0->ui4_freq = (g_sonysat_demod_ctx.tunedSignal.centerFreqKHz + 500) / 1000;
            pAttri0->ui4_sym_rate = g_sonysat_demod_ctx.tunedSignal.symbolRateKSps;
            sonysat_result = sonysat_demod_dvbs_s2_monitor_SymbolRate(pDemod, &symbolRateSps);
            if(sonysat_result == SONYSAT_RESULT_OK)
            {
                 pAttri0->u14_sym_rate_in_Sps = symbolRateSps;
            }
            else
            {
                mcDBG_MSG_L1(("Error in sonysat_demod_dvbs_s2_monitor_SymbolRate (%s)\n", sonysat_FormatResult(sonysat_result)));
            }
            if(g_channel_locked_flag == 1)
            {
                pAttri0->ui4_data = ui4_data_forMW;
            }
            else
            {
                pAttri0->ui4_data = 0;
            }
            pAttri0->e_pol = pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH ? POL_LIN_HORIZONTAL : POL_LIN_VERTICAL;
            mcDBG_MSG_L1(("[DVBS_TunerGetAttribute]  ui4_freq:%d KHz SymbolRate=%dS/s !\n",pAttri0->ui4_freq,pAttri0->u14_sym_rate_in_Sps));
        }
        break;
    case TUNER_GET_TYPE_BER:
        {
            mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //TODO Get BER from demod driver.Note the unit is 1e-5.update it as i4Temp
            {
                UINT32 ber = sonysat_GetBER();
                i4Temp = (INT32)ber;
            }
            mcDBG_MSG_L1(("BER is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PRE_VBER:
        {
            mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_PRE_VBER\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //TODO Get BER from demod driver.Note the unit is 1e-5.update it as i4Temp
            {
                UINT32 ber = sonysat_GetPreBER();
                i4Temp = (INT32)ber;
            }
            mcDBG_MSG_L1(("BER is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SSI:
        {
            //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_RSSI\r\n"));
            *pzAttributeLen = sizeof(INT32);
            //signal level.update it as i4Temp
            i4Temp = sonysat_GetSSI();
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
            mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_SQI\r\n"));
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
            mcDBG_MSG_L1(("SQI is %d\n",i4Temp));
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
            mcDBG_MSG_L1(("CNR is %d\n",i4Temp));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_LNB_STATUS:
        {
            UINT8 u1OverLoad=0;
            sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;

            if(NULL!=pLnbc->MonitorOverload)
            {
               if(SONYSAT_RESULT_OK!=pLnbc->MonitorOverload(pLnbc,&u1OverLoad))
               {
                u1OverLoad=0;//defaunlt value
               }
               pAttri2->b_short_status=u1OverLoad;
             }
            pAttri2->ui1_lnb_power=g_diseqc_cmd.ui1_lnb_power;
            pAttri2->ui1_polarity_13v_18v=g_diseqc_cmd.ui1_polarity_13v_18v;
            pAttri2->b_22k=g_diseqc_cmd.b_22k;

            mcDBG_MSG_L2(("LNB overload is %d,LNB power is=%d ,power polarity is %d,22K tone is %d\n",pAttri2->b_short_status,pAttri2->ui1_lnb_power,pAttri2->ui1_polarity_13v_18v,pAttri2->b_22k));
        }
        break;
    case TUNER_GET_TYPE_UEC:
        {
            UINT16 UEC = 0;
            mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_UEC\r\n"));
            *pzAttributeLen = sizeof(UINT16);
            UEC = sonysat_GetUEC ();
            mcDBG_MSG_L1(("UEC is %d\n", UEC));
            x_memcpy(pzAttribute, &UEC, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        {
            //mcDBG_MSG_L1(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
            pAttri1 = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
            //follow is an example,pls apply you code
            //Tuner frequency boundary
            pAttri1->ui4_lower_bound_freq = bs_start_freq;//950Hz
            pAttri1->ui4_upper_bound_freq = bs_stop_freq;//2150M
            // freq offset tolerence range in Hz (1M mapping to 100K)
            pAttri1->ui4_fine_tune_delta_freq = 3;  //3//
            pAttri1->b_drv_notify_progress_capability = TRUE;//To indicate if driver can support scan progress notify
            mcDBG_MSG_L2(("b_drv_notify_progress_capability=%d\n", pAttri1->b_drv_notify_progress_capability));
        }
        break;
    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T tsFmt;
            *pzAttributeLen = sizeof(TS_FMT_T);
            sonysat_GetTsFmt(pDemod, &tsFmt);
            x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_BER_UPPER:
        {
            UINT16 berUpper;
            UINT32 ber=sonysat_GetBER();
            berUpper = (UINT16)((ber>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_BER_LOWER:
        {
            UINT16 berLower;
            UINT32 ber=sonysat_GetBER();
            berLower = (UINT16)(ber&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berLower, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_UPPER:
        {
            UINT16 perUpper;
            UINT32 per=sonysat_GetPER();
            perUpper = (UINT16)((per>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_LOWER:
        {
            UINT16 perLower;
            UINT32 per=sonysat_GetPER();
            perLower = (UINT16)(per&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perLower, *pzAttributeLen);
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
static INT32 DVBS_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    UCHAR   ucData=0;// = *(UCHAR *)pvAttribute;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;

    if(pvAttribute != NULL)
    {
        ucData = *(UCHAR *)pvAttribute;
    }
    else if( e_set_type == TUNER_SET_TYPE_SCAN_CONTINUE)
    {
        mcDBG_MSG_L1(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
        x_sema_unlock(AutoScan_callback);
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
            x_sema_unlock(AutoScan_callback);
        }
        break;*/
    case TUNER_SET_TYPE_DISEQC_CMD:
        {
            DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pvAttribute;
            g_set_flag=1;
            g_channel_locked_flag = 1;//Reset the lock flag to TRUE.This is very important.
            x_memcpy(&g_diseqc_cmd, pCmd, sizeof(DISEQC_BUS_CMD_T));//availink :add for desiqc control ,20110210 lmh
            sonysat_HandleDiseqcCommand(pCmd);
        }
        break;
    case TUNER_SET_TYPE_RESET_CMD:
        {
            DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pvAttribute;
            g_set_flag=1;
            g_channel_locked_flag = 1;//Reset the lock flag to TRUE.This is very important.
            sonysat_HandleResetCommand(pCmd);
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
            DVBS_TunerDemodClock(data);
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

    default:
        mcDBG_MSG_L1(("unknown: set Attribute\r\n"));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return DRVAPI_TUNER_OK;
}
#ifdef fcADD_DIAG_INFO
static INT32 DVBS_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    INT32   retSts = RMR_OK;

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
            sonysat_set_ts_maxfreq(pParameter);

            /*Non-CanalDigital change demod setting*/
            sonysat_set_demod_non_canaldigital(pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
            sonysat_set_eeprom_non_canaldigital(pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
            sonysat_set_flag_non_canaldigital(0xAA);    //0xAA means User have re-written Registers.
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[0] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[0];
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[1] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[1];
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[2] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[2];
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[3] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[3];
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[4] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[4];
            g_sonysat_demod_ctx.CurValue_non_CanalDigital[5] = pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value[5];

            /*Other*/
        }
        break;
        case DRV_CUSTOM_TUNER_SET_TYPE_LNB_OVERLOAD_RESET:
        {
            DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_set_info;
            sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
            #ifdef CC_DEMOD_DVBS_LNB_SHORT_MONITOR
            /*Reset LNB short status*/
            if(pParameter->t_reset_lnbshort.b_setvalid == TRUE)
            {
                mcDBG_MSG_L1(("[DVBS] Set LNB short reset!\r\n"));
                if(pLnbc->state == SONYSAT_LNBC_STATE_OVERLOAD)
                {
                    mcDBG_MSG_L1(("LNB overload reset!\r\n"));
                    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;
                    Vendor_ResendDiseqcCmmd(&g_diseqc_cmd);
                }
            }
            #endif
            /*Other*/
        }
        break;

        default:
            mcDBG_MSG_L1(("unknown: set type = %d\r\n",e_tuner_set_type));
        break;
    }
#else
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used
#endif /* CC_PROJECT_FOR_SONY */

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

    return retSts;
}
static INT32 DVBS_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
#ifdef CC_PROJECT_FOR_SONY
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    switch (e_tuner_get_type) {
    case DRV_CUSTOM_TUNER_GET_TYPE_DEMOD_PARAMETER:
        {
            DRV_CUSTOM_DEMOD_PARAMETER_T   *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_get_info;
            sonysat_get_eeprom_non_canaldigital(pParameter->t_non_canal_digital_setting.ui1_non_canal_digital_value);
        }
        break;
    case DRV_CUSTOM_TUNER_GET_TYPE_LNB_OVERLOAD:
        {
            DRV_CUSTOM_DEMOD_PARAMETER_T  *pParameter = (DRV_CUSTOM_DEMOD_PARAMETER_T*) pv_get_info;
            x_memset(pParameter, 0, sizeof(DRV_CUSTOM_DEMOD_PARAMETER_T));
            pParameter->t_reset_lnbshort.ui1_overload = sonysat_GetLnbShort();
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
static void DVBS_TunerBypassI2C (PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;

    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO  Bypass i2c to tuner connected(bSwitchOn is TRUE) or disconnect(bSwitchOn is FALSE)

    sonysat_demod_I2cRepeaterEnable(pDemod, bSwitchOn ? 0x01 : 0x00);

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}
#if 1
BOOL DVBS_TunerNotifyEnable(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    BOOL TunerNotifyEnable;
    if(do_blindscan == 1)
        TunerNotifyEnable = FALSE;
    else
        TunerNotifyEnable =TRUE;

    return TunerNotifyEnable;
}
#endif
//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
void PD_DVBS_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose   = DVBS_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq     = DVBS_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = DVBS_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = DVBS_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBS_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBS_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBS_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBS_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = DVBS_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer  = DVBS_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = DVBS_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBS_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBS_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = DVBS_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = DVBS_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = DVBS_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = DVBS_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc    = DVBS_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DVBS_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DVBS_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= DVBS_TunerBypassI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNotifyEnable = DVBS_TunerNotifyEnable;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = DVBS_TunerSetDiagCmd;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DVBS_TunerGetDiagInfo;
#endif
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
INT32 DVBS_TunerOpen_c1_4K(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
    sonysat_horus3a_t *pHorus3a = &g_sonysat_demod_ctx.horus3a;
    sonysat_horus3_t *pHorus3 = &g_sonysat_demod_ctx.horus3;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    sonysat_i2c_t *pI2c = &g_sonysat_demod_ctx.i2c;
    sonysat_i2c_t *pI2cGw = &g_sonysat_demod_ctx.i2cGw;
    UINT8 data = 0;
    UINT8 demodSlave = 0xD0;
    UINT8 tunerSlave = 0xC6;
    UINT32 tunerFlags = 0;

    sonysat_demod_xtal_t demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;
    BOOL is_dsqout_pwm = FALSE;
    UINT8 is_iqinv = 0;

    BOOL ignore_lnbc = FALSE;
	
    ITUNER_CTX_T *psTunerCtx ;

#ifdef SONYSAT_DUMMY_DEBUG
    /* for dummy debug */
    demodSlave = 0xD8;
    tunerSlave = 0xC6;
    is_iqinv = 0; /* Bernabeu : 0, SYSEVA : 1 */
#else
#if defined(CC_SONYDEMOD_CXD2839)
    demodSlave = 0xC8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#else
    demodSlave = 0xD8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#endif
#endif

    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=demodSlave;

    // Set initial log level
    //u1DbgLevel=4;

    //create pdctx used in this file, you also can add any declaration you want in pdctx
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
    if (pPdCtxDVBS == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/

    pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
    mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
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
    PD_DVBS_Register_LTDIS_Fct(ptTDCtx);

    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
    Demod_GPIO_Reset();

    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set TS serial or parallel here!!
    is_parallel = FALSE;

    // NOTE: Please set DSQOUT output is PWM (Tone is generated by Demod)
    //                                or ENVELOPE (Tone is generated by LNBC)
#if defined(SONYSAT_LNBC_ST_LNBH29) || defined(SONYSAT_LNBC_ALLEGRO_A8304)
    // LNBH29 and A8304 only support ENVELOPE
    is_dsqout_pwm = FALSE;
#else
    is_dsqout_pwm = TRUE;
#endif

    // Sony specific initialization -------------------------------------------

    g_sonysat_demod_ctx.breakFct = NULL;
    g_sonysat_demod_ctx.breakFctArg = NULL;

    // I2C struct creation used in CXD2854 driver
    sonysat_result = sonysat_i2c_MTK_CreateI2c(pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    //I2C GateAway
    sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cGw, demodSlave, 0x09);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Enable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x01, 0x01);

#ifdef CC_PROJECT_FOR_SONY
    //----------------------------------
    //check mount status
    g_ui1_mount_status = 0x00;
    if(sonysat_result == SONYSAT_RESULT_OK)
    {
        g_ui1_mount_status = 0x01;
    }
/*
    if(ignore_lnbc != TRUE)
    {
        g_ui1_mount_status |= 0x02;
    }
  */
    //----------------------------------
#endif /* CC_PROJECT_FOR_SONY */

    if(sonysat_result != SONYSAT_RESULT_OK)
    {
        return DRVAPI_TUNER_ERROR;
    }

    // Read tuner product id
    sonysat_result = pI2c->ReadRegister (pI2c, tunerSlave, 0x7F, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Disable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x00, 0x01);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

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
        //sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2cGw, /* 16MHz Xtal */
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
	//I2C Gateaway
	//sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2cGw, /* 16MHz Xtal */
            tunerFlags,
            pHorus3);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    } else {
        mcDBG_MSG_ERR(("Error to access to tuner.\n"));
        return DRVAPI_TUNER_ERROR;
    }

    // LNB struct creation ----------------------------------------------------
#if defined(SONYSAT_LNBC_MPS_MP8126)
    ignore_lnbc = FALSE;
    sonysat_result = mps_mp8126_Create(pLnbc, pDemod, 1, 2, 0, &g_sonysat_demod_ctx.mp8126);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ST_LNBH29)
    ignore_lnbc = FALSE;
    sonysat_result = st_lnbh29_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
    ignore_lnbc = FALSE;
    sonysat_result = allegro_a8304_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif
//Check LNBC Monted status
{
		uint8_t tempData = 0x24;
		sonysat_result = pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &tempData, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN);
		if(sonysat_result != SONYSAT_RESULT_OK){
			ignore_lnbc = TRUE;
			mcDBG_MSG_L2(("Mount Error to write to LNBC. LNBC will be ignored.\n"));
		} else {
			
			ignore_lnbc = FALSE;
			mcDBG_MSG_L2(("Mount Success to write to LNBC.\n"));
		}
}

		  if(ignore_lnbc != TRUE)
			{
				g_ui1_mount_status |= 0x02;
			}
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
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, NULL);
    }else{
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, pLnbc);
    }
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2854(DVB-S/S2 part) Demod Initialization.\n"));

    // Sony demod initialization ----------------------------------------------
    sonysat_result = sonysat_integ_InitializeS(pInteg);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_integ_InitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
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

#ifdef CC_PROJECT_FOR_SONY
    sonysat_init_eeprom_non_canaldigital();
#endif /* CC_PROJECT_FOR_SONY */

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
#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    g_unicable_p = 0;
#endif
    return (DRVAPI_TUNER_OK);
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
INT32 DVBS_TunerOpen_c1_2K(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
    sonysat_horus3a_t *pHorus3a = &g_sonysat_demod_ctx.horus3a;
    sonysat_horus3_t *pHorus3 = &g_sonysat_demod_ctx.horus3;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    sonysat_i2c_t *pI2c = &g_sonysat_demod_ctx.i2c;
    sonysat_i2c_t *pI2cGw = &g_sonysat_demod_ctx.i2cGw;
    UINT8 data = 0;
    UINT8 demodSlave = 0xD0;
    UINT8 tunerSlave = 0xC6;
    UINT32 tunerFlags = 0;

    sonysat_demod_xtal_t demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;
    BOOL is_dsqout_pwm = FALSE;
    UINT8 is_iqinv = 0;

    BOOL ignore_lnbc = FALSE;
	
    ITUNER_CTX_T *psTunerCtx ;

#ifdef SONYSAT_DUMMY_DEBUG
    /* for dummy debug */
    demodSlave = 0xD8;
    tunerSlave = 0xC6;
    is_iqinv = 0; /* Bernabeu : 0, SYSEVA : 1 */
#else
#if defined(CC_SONYDEMOD_CXD2839)
    demodSlave = 0xC8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#else
    demodSlave = 0xD8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#endif
#endif

    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=demodSlave;

    // Set initial log level
    //u1DbgLevel=4;

    //create pdctx used in this file, you also can add any declaration you want in pdctx
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
    if (pPdCtxDVBS == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/

    pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
    mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
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
    PD_DVBS_Register_LTDIS_Fct(ptTDCtx);

    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
    Demod_GPIO_Reset();

    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set TS serial or parallel here!!
    is_parallel = FALSE;

    // NOTE: Please set DSQOUT output is PWM (Tone is generated by Demod)
    //                                or ENVELOPE (Tone is generated by LNBC)
#if defined(SONYSAT_LNBC_ST_LNBH29) || defined(SONYSAT_LNBC_ALLEGRO_A8304)
    // LNBH29 and A8304 only support ENVELOPE
    is_dsqout_pwm = FALSE;
#else
    is_dsqout_pwm = TRUE;
#endif

    // Sony specific initialization -------------------------------------------

    g_sonysat_demod_ctx.breakFct = NULL;
    g_sonysat_demod_ctx.breakFctArg = NULL;

    // I2C struct creation used in CXD2854 driver
    sonysat_result = sonysat_i2c_MTK_CreateI2c(pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    //I2C GateAway
    sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cGw, demodSlave, 0x09);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Enable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x01, 0x01);

#ifdef CC_PROJECT_FOR_SONY
    //----------------------------------
    //check mount status
    g_ui1_mount_status = 0x00;
    if(sonysat_result == SONYSAT_RESULT_OK)
    {
        g_ui1_mount_status = 0x01;
    }
/*
    if(ignore_lnbc != TRUE)
    {
        g_ui1_mount_status |= 0x02;
    }
  */
    //----------------------------------
#endif /* CC_PROJECT_FOR_SONY */

    if(sonysat_result != SONYSAT_RESULT_OK)
    {
        return DRVAPI_TUNER_ERROR;
    }

    // Read tuner product id
    sonysat_result = pI2c->ReadRegister (pI2c, tunerSlave, 0x7F, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Disable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x00, 0x01);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

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
        //tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
       // tunerFlags |= SONYSAT_HORUS3A_CONFIG_LNA_ENABLE;           /* LNA enable */
#endif
        //sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 27, tunerSlave, pI2cGw, /* 27MHz Xtal */
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
       // tunerFlags |= SONYSAT_HORUS3_CONFIG_EXT_REF;               /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
     //   tunerFlags |= SONYSAT_HORUS3_CONFIG_LNA_ENABLE;            /* LNA enable */
#endif
	//I2C Gateaway
	//sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3_Create(pTuner, 27, tunerSlave, pI2cGw, /* 27MHz Xtal */
            tunerFlags,
            pHorus3);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    } else {
        mcDBG_MSG_ERR(("Error to access to tuner.\n"));
        return DRVAPI_TUNER_ERROR;
    }

    // LNB struct creation ----------------------------------------------------
#if defined(SONYSAT_LNBC_MPS_MP8126)
    ignore_lnbc = FALSE;
    sonysat_result = mps_mp8126_Create(pLnbc, pDemod, 1, 2, 0, &g_sonysat_demod_ctx.mp8126);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ST_LNBH29)
    ignore_lnbc = FALSE;
    sonysat_result = st_lnbh29_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
    ignore_lnbc = FALSE;
    sonysat_result = allegro_a8304_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif
//Check LNBC Monted status
{
		uint8_t tempData = 0x24;
		sonysat_result = pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &tempData, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN);
		if(sonysat_result != SONYSAT_RESULT_OK){
			ignore_lnbc = TRUE;
			mcDBG_MSG_L2(("Mount Error to write to LNBC. LNBC will be ignored.\n"));
		} else {
			
			ignore_lnbc = FALSE;
			mcDBG_MSG_L2(("Mount Success to write to LNBC.\n"));
		}
}

		  if(ignore_lnbc != TRUE)
			{
				g_ui1_mount_status |= 0x02;
			}
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
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, NULL);
    }else{
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, pLnbc);
    }
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2854(DVB-S/S2 part) Demod Initialization.\n"));

    // Sony demod initialization ----------------------------------------------
    sonysat_result = sonysat_integ_InitializeS(pInteg);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_integ_InitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
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

#ifdef CC_PROJECT_FOR_SONY
    sonysat_init_eeprom_non_canaldigital();
#endif /* CC_PROJECT_FOR_SONY */

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
#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    g_unicable_p = 0;
#endif
    return (DRVAPI_TUNER_OK);
}
/////////////////////////////////////////////////
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
INT32 DVBS_TunerOpen_c2_4K(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
    sonysat_horus3a_t *pHorus3a = &g_sonysat_demod_ctx.horus3a;
    sonysat_horus3_t *pHorus3 = &g_sonysat_demod_ctx.horus3;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    sonysat_i2c_t *pI2c = &g_sonysat_demod_ctx.i2c;
    sonysat_i2c_t *pI2cGw = &g_sonysat_demod_ctx.i2cGw;
    UINT8 data = 0;
    UINT8 demodSlave = 0xD0;
    UINT8 tunerSlave = 0xC6;
    UINT32 tunerFlags = 0;

    sonysat_demod_xtal_t demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;
    BOOL is_dsqout_pwm = FALSE;
    UINT8 is_iqinv = 0;

    BOOL ignore_lnbc = FALSE;
	
    ITUNER_CTX_T *psTunerCtx ;

#ifdef SONYSAT_DUMMY_DEBUG
    /* for dummy debug */
    demodSlave = 0xDA;
    tunerSlave = 0xC6;
    is_iqinv = 0; /* Bernabeu : 0, SYSEVA : 1 */
#else
#if defined(CC_SONYDEMOD_CXD2839)
    demodSlave = 0xC8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#else
    demodSlave = 0xDA;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#endif
#endif

    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=demodSlave;

    // Set initial log level
    //u1DbgLevel=4;

    //create pdctx used in this file, you also can add any declaration you want in pdctx
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
    if (pPdCtxDVBS == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/

    pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
    mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
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
    PD_DVBS_Register_LTDIS_Fct(ptTDCtx);

    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
    Demod_GPIO_Reset();

    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set TS serial or parallel here!!
    is_parallel = FALSE;

    // NOTE: Please set DSQOUT output is PWM (Tone is generated by Demod)
    //                                or ENVELOPE (Tone is generated by LNBC)
#if defined(SONYSAT_LNBC_ST_LNBH29) || defined(SONYSAT_LNBC_ALLEGRO_A8304)
    // LNBH29 and A8304 only support ENVELOPE
    is_dsqout_pwm = FALSE;
#else
    is_dsqout_pwm = TRUE;
#endif

    // Sony specific initialization -------------------------------------------

    g_sonysat_demod_ctx.breakFct = NULL;
    g_sonysat_demod_ctx.breakFctArg = NULL;

    // I2C struct creation used in CXD2854 driver
    sonysat_result = sonysat_i2c_MTK_CreateI2c(pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    //I2C GateAway
    sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cGw, demodSlave, 0x09);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Enable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x01, 0x01);

#ifdef CC_PROJECT_FOR_SONY
    //----------------------------------
    //check mount status
    g_ui1_mount_status = 0x00;
    if(sonysat_result == SONYSAT_RESULT_OK)
    {
        g_ui1_mount_status = 0x01;
    }
/*
    if(ignore_lnbc != TRUE)
    {
        g_ui1_mount_status |= 0x02;
    }
  */
    //----------------------------------
#endif /* CC_PROJECT_FOR_SONY */

    if(sonysat_result != SONYSAT_RESULT_OK)
    {
        return DRVAPI_TUNER_ERROR;
    }

    // Read tuner product id
    sonysat_result = pI2c->ReadRegister (pI2c, tunerSlave, 0x7F, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Disable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x00, 0x01);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

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
        //sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2cGw, /* 16MHz Xtal */
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
	//I2C Gateaway
	//sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2cGw, /* 16MHz Xtal */
            tunerFlags,
            pHorus3);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    } else {
        mcDBG_MSG_ERR(("Error to access to tuner.\n"));
        return DRVAPI_TUNER_ERROR;
    }

    // LNB struct creation ----------------------------------------------------
#if defined(SONYSAT_LNBC_MPS_MP8126)
    ignore_lnbc = FALSE;
    sonysat_result = mps_mp8126_Create(pLnbc, pDemod, 1, 2, 0, &g_sonysat_demod_ctx.mp8126);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ST_LNBH29)
    ignore_lnbc = FALSE;
    sonysat_result = st_lnbh29_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
    ignore_lnbc = FALSE;
    sonysat_result = allegro_a8304_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif
//Check LNBC Monted status
{
		uint8_t tempData = 0x24;
		sonysat_result = pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &tempData, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN);
		if(sonysat_result != SONYSAT_RESULT_OK){
			ignore_lnbc = TRUE;
			mcDBG_MSG_L2(("Mount Error to write to LNBC. LNBC will be ignored.\n"));
		} else {
			ignore_lnbc = FALSE;
			mcDBG_MSG_L2(("Mount Success to write to LNBC.\n"));
		}
}

		  if(ignore_lnbc != TRUE)
			{
				g_ui1_mount_status |= 0x02;
			}
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
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, NULL);
    }else{
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, pLnbc);
    }
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2854(DVB-S/S2 part) Demod Initialization.\n"));

    // Sony demod initialization ----------------------------------------------
    sonysat_result = sonysat_integ_InitializeS(pInteg);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_integ_InitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
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

#ifdef CC_PROJECT_FOR_SONY
    sonysat_init_eeprom_non_canaldigital();
#endif /* CC_PROJECT_FOR_SONY */

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
#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    g_unicable_p = 0;
#endif
    return (DRVAPI_TUNER_OK);
}

/////////////////////////////////////////////////
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
INT32 DVBS_TunerOpen_c2_2K(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    sonysat_result_t sonysat_result = SONYSAT_RESULT_OK;
    sonysat_integ_t *pInteg = &g_sonysat_demod_ctx.integ;
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    sonysat_tuner_sat_t *pTuner = &g_sonysat_demod_ctx.tuner;
    sonysat_horus3a_t *pHorus3a = &g_sonysat_demod_ctx.horus3a;
    sonysat_horus3_t *pHorus3 = &g_sonysat_demod_ctx.horus3;
    sonysat_lnbc_t *pLnbc = &g_sonysat_demod_ctx.lnbc;
    sonysat_i2c_t *pI2c = &g_sonysat_demod_ctx.i2c;
    sonysat_i2c_t *pI2cGw = &g_sonysat_demod_ctx.i2cGw;
    UINT8 data = 0;
    UINT8 demodSlave = 0xD0;
    UINT8 tunerSlave = 0xC6;
    UINT32 tunerFlags = 0;

    sonysat_demod_xtal_t demod_xtal = SONYSAT_DEMOD_XTAL_24000KHz;
    BOOL is_parallel = FALSE;
    BOOL is_dsqout_pwm = FALSE;
    UINT8 is_iqinv = 0;

    BOOL ignore_lnbc = FALSE;
	
    ITUNER_CTX_T *psTunerCtx ;

#ifdef SONYSAT_DUMMY_DEBUG
    /* for dummy debug */
    demodSlave = 0xDA;
    tunerSlave = 0xC6;
    is_iqinv = 0; /* Bernabeu : 0, SYSEVA : 1 */
#else
#if defined(CC_SONYDEMOD_CXD2839)
    demodSlave = 0xC8;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#else
    demodSlave = 0xDA;
    tunerSlave = 0xC6;
    is_iqinv = 0;
#endif
#endif

    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=demodSlave;

    // Set initial log level
    //u1DbgLevel=4;

    //create pdctx used in this file, you also can add any declaration you want in pdctx
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
    if (pPdCtxDVBS == NULL)
    {
        mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/

    pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
    mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
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
    PD_DVBS_Register_LTDIS_Fct(ptTDCtx);

    //TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
    Demod_GPIO_Reset();

    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set TS serial or parallel here!!
    is_parallel = FALSE;

    // NOTE: Please set DSQOUT output is PWM (Tone is generated by Demod)
    //                                or ENVELOPE (Tone is generated by LNBC)
#if defined(SONYSAT_LNBC_ST_LNBH29) || defined(SONYSAT_LNBC_ALLEGRO_A8304)
    // LNBH29 and A8304 only support ENVELOPE
    is_dsqout_pwm = FALSE;
#else
    is_dsqout_pwm = TRUE;
#endif

    // Sony specific initialization -------------------------------------------

    g_sonysat_demod_ctx.breakFct = NULL;
    g_sonysat_demod_ctx.breakFctArg = NULL;

    // I2C struct creation used in CXD2854 driver
    sonysat_result = sonysat_i2c_MTK_CreateI2c(pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    //I2C GateAway
    sonysat_result = sonysat_i2c_MTK_CreateI2cGw(pI2cGw, demodSlave, 0x09);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Enable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x01, 0x01);

#ifdef CC_PROJECT_FOR_SONY
    //----------------------------------
    //check mount status
    g_ui1_mount_status = 0x00;
    if(sonysat_result == SONYSAT_RESULT_OK)
    {
        g_ui1_mount_status = 0x01;
    }
/*
    if(ignore_lnbc != TRUE)
    {
        g_ui1_mount_status |= 0x02;
    }
  */
    //----------------------------------
#endif /* CC_PROJECT_FOR_SONY */

    if(sonysat_result != SONYSAT_RESULT_OK)
    {
        return DRVAPI_TUNER_ERROR;
    }

    // Read tuner product id
    sonysat_result = pI2c->ReadRegister (pI2c, tunerSlave, 0x7F, &data, 1);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // Disable I2C repeater via demodulator.
    sonysat_result = sonysat_i2c_SetRegisterBits (pI2c, demodSlave + 4, 0x08, 0x00, 0x01);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

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
       // tunerFlags |= SONYSAT_HORUS3A_CONFIG_EXT_REF;              /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
       // tunerFlags |= SONYSAT_HORUS3A_CONFIG_LNA_ENABLE;           /* LNA enable */
#endif
        //sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3a_Create(pTuner, 27, tunerSlave, pI2cGw, /* 27MHz Xtal */
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
     //   tunerFlags |= SONYSAT_HORUS3_CONFIG_EXT_REF;               /* External reference */
#endif
#ifdef SONYSAT_TUNER_LNA_ENABLE
    //    tunerFlags |= SONYSAT_HORUS3_CONFIG_LNA_ENABLE;            /* LNA enable */
#endif
	//I2C Gateaway
	//sonysat_result = sonysat_tuner_horus3_Create(pTuner, 16, tunerSlave, pI2c, /* 16MHz Xtal */
        sonysat_result = sonysat_tuner_horus3_Create(pTuner, 27, tunerSlave, pI2cGw, /* 27MHz Xtal */
            tunerFlags,
            pHorus3);
        if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
    } else {
        mcDBG_MSG_ERR(("Error to access to tuner.\n"));
        return DRVAPI_TUNER_ERROR;
    }

    // LNB struct creation ----------------------------------------------------
#if defined(SONYSAT_LNBC_MPS_MP8126)
    ignore_lnbc = FALSE;
    sonysat_result = mps_mp8126_Create(pLnbc, pDemod, 1, 2, 0, &g_sonysat_demod_ctx.mp8126);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ST_LNBH29)
    ignore_lnbc = FALSE;
    sonysat_result = st_lnbh29_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#elif defined(SONYSAT_LNBC_ALLEGRO_A8304)
    ignore_lnbc = FALSE;
    sonysat_result = allegro_a8304_Create (pLnbc, 0x10, pI2c);
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }
#endif
//Check LNBC Monted status
{
		uint8_t tempData = 0x24;
		sonysat_result = pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &tempData, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN);
		if(sonysat_result != SONYSAT_RESULT_OK){
			ignore_lnbc = TRUE;
			mcDBG_MSG_L2(("Mount Error to write to LNBC. LNBC will be ignored.\n"));
		} else {
			ignore_lnbc = FALSE;
			mcDBG_MSG_L2(("Mount Success to write to LNBC.\n"));
		}
}

		  if(ignore_lnbc != TRUE)
			{
				g_ui1_mount_status |= 0x02;
			}
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
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, NULL);
    }else{
        sonysat_result = sonysat_integ_Create(pInteg, demod_xtal, demodSlave, pI2c, pDemod, pTuner, pLnbc);
    }
    if(sonysat_result != SONYSAT_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2854(DVB-S/S2 part) Demod Initialization.\n"));

    // Sony demod initialization ----------------------------------------------
    sonysat_result = sonysat_integ_InitializeS(pInteg);
    if(sonysat_result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L2(("Error in sonysat_integ_InitializeS (%s)\n", sonysat_FormatResult(sonysat_result)));
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

#ifdef CC_PROJECT_FOR_SONY
    sonysat_init_eeprom_non_canaldigital();
#endif /* CC_PROJECT_FOR_SONY */

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
#ifdef SONYSAT_DVBS_ENABLE_DETECTING_FALSE_LOCK
    g_unicable_p = 0;
#endif
    return (DRVAPI_TUNER_OK);
}

///////////////////////////////////////////////////////////////

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

#ifdef CC_PROJECT_FOR_SONY
UINT8 DVBS_DemodMountStatus(void)
{
    //mcDBG_MSG_L2(("DemodMountStatus %d \n",g_ui1_mount_status));
    sonysat_demod_t *pDemod = &g_sonysat_demod_ctx.demod;
    if(g_ui1_mount_status & 0x1)
    {
        mcDBG_MSG_L2(("Demod Chip ID = 0x%X \n",pDemod->chipId));
    }
    else
    {
        mcDBG_MSG_L2(("Demod Enable = 0\n"));
    }
    if(g_ui1_mount_status & 0x2)
    {
        mcDBG_MSG_L2(("LNB Enable = 1\n"));
    }
    else
    {
        mcDBG_MSG_L2(("LNB Enable = 0\n"));
    }

    return g_ui1_mount_status;
}
#endif /* CC_PROJECT_FOR_SONY */
