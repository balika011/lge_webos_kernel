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
 * $RCSfile: pd_glue_dvbt2.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_glue_dvbt2.c
 *  dvbt2 glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#ifndef SONY_DUMMY_DEBUG // For dummy debug by Sony
#ifdef INTERNAL_3RD_DEMOD
#include "TDIS.h"
#include "pd_tuner_type.h"
#include "queue.h"
#include "TDMgr.h"
#include "drvapi_tuner.h"
#include "UtilAPI.h"
#include "pod_if.h"
#include "ctrl_bus.h"
#else
#include "TDIS_header.h"
//#include "vdr_PD_Def.h"
#include "tunerDebug.h"
#include "pi_demod_common_def.h"

#endif
#include "pd_dvbt2_if.h"
#include "pi_dvbt_if.h"
//#include "pi_def_dvbt.h"

#include "u_drv_cust.h"
#include "u_tuner.h"
#include "u_handle.h"

#include "x_os.h"
#include "x_stl_lib.h"
#include "x_gpio.h"
#include "x_rm.h"
#include "x_tuner.h"


#include "tuner_if.h"
#include "tuner_interface_if.h"

#include "drvcust_if.h"
#include "eeprom_if.h"
#include "i2c_api.h"

#else // SONY_DUMMY_DEBUG
#include "MTKdef.h" // MTK dummy definition
#include "pd_dvbt2_if.h"
#endif // SONY_DUMMY_DEBUG

// for Sony CXD2854 generation driver
#include "sony_i2c_MTK.h"
#include "sony_demod.h"
#include "sony_demod_dvbt.h"
#include "sony_demod_dvbt2.h"
#include "sony_demod_dvbc.h"
#include "sony_demod_dvbc2.h"
#include "sony_demod_dvbt_monitor.h"
#include "sony_demod_dvbt2_monitor.h"
#include "sony_demod_dvbc_monitor.h"
#include "sony_demod_dvbc2_monitor.h"
#include "sony_integ_dvbt_t2.h"
#include "sony_integ_dvbc.h"
#include "sony_integ_dvbc2.h"
#include "sony_math.h"

#include "sony_mtk_glue_common.h"

#define EMB_L1_DRIVER_CORE          /* empty  */
#define SLAB_WINAPI                 /* empty  */
#define EMB_L1_COMMON_DEMOD_API     /* empty  */
#define EMB_L1_MULTISTANDARD_API    /* empty  */
#define EMB_L1_DVB_C_DEMOD_API      /* empty  */
//#define WINAPI                      /* empty  */ by SONY

//#define  TSERR_ENABLE /*for TSer/GPIO select 20111221*/
#define PD_NO_USE_REMAP    /*wenming 12-2-23: PD_NO_USE_REMAP must be defined for DVBT2_TunerGetAttribute2 in use...*/
#define DEFAULT_DVBT_DELTA_FREQ 714000  /*wenming 12-2-23: DVBT default delta freq is 714000 for 8M(EU) DVBT*/
#define SONY_CXD2854_DVBTT2CC2_DRIVER_VERSION_STR "Sony CXD2854 (DVB-T/T2/C/C2) v0.02 2014-03-07"

#ifdef INTERNAL_3RD_DEMOD
#define mcDBG_MSG_ERR(_x_)      mcSHOW_USER_MSG(_x_)
#define mcDBG_MSG_INFO(_x_)     mcSHOW_USER_MSG(_x_)
#define mcDBG_MSG_L1(_x_)   mcSHOW_USER_MSG(_x_)
#define mcDBG_MSG_L2(_x_)   mcSHOW_USER_MSG(_x_)
#define mcDBG_MSG_L4(_x_)   mcSHOW_DBG_MSG4(_x_)
#endif

//#define SONY_DEMOD_IIC_ADDRESS 0xDA

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define WAIT_SQI_STABLE_TIMEOUT       2500  //SQI is correct after delay 2500ms

//-----------------------------------------------------------------------------
// Configurations
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

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static DVBT2_PD_CTX_T *pPdCtxDVBT2;
static BOOL fgHWReset = FALSE; /*wenming 12-3-24: HW reset flag*/
static BOOL fgChanScanforGetAtt = FALSE;
static BOOL fgDemodInited = FALSE;

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Sony specific
//-----------------------------------------------------------------------------
static BOOL sony_GetTsFmt(sony_demod_t *pDemod, TS_FMT_T *pTsfmt)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    UINT8 data = 0;

    // Set Bank 0
    sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
    if(sony_result != SONY_RESULT_OK){ return 0; }

    // OSERIALEN (Bank:00h, Addr:C4h, Bit[7])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->SerOrPar = (data & 0x80) ? SP_SERIAL : SP_PARALLEL;

    // OSERCKMODE[1:0] (Bank:00h, Addr:C4h, Bit[1:0]
    pTsfmt->fgGatedClock = ((data & 0x03) == 0x03) ? BOOL_TRUE : BOOL_FALSE;

    // OWFMT_CKINV (Bank:00h, Addr:C5h, Bit[0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC5, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->ClockPolarity = (data & 0x01) ? POL_HIGH_ACTIVE : POL_LOW_ACTIVE;

    // OTSDTDISABLE (Bank:00h, Addr:C3h, Bit[0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->fgTristate = (data & 0x01) ? BOOL_FALSE : BOOL_TRUE ;

    pTsfmt->u4TsClockInKHz = 82000;

    pTsfmt->fgExternalDemod = BOOL_TRUE;

    return 1;
}

/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/

/***********************************************************************/
/*              Private (static) Function Prototypes                   */
/***********************************************************************/
#ifdef CC_PROJECT_FOR_SONY
STATIC void DVBT2_TunerGetModuInfor(void* pzAttribute)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    TUNER_TER_DIG_TUNE_STATUS_T *pAttribute = (TUNER_TER_DIG_TUNE_STATUS_T *) pzAttribute;
    sony_dvbt_tpsinfo_t tpsinfo;

    if(pAttribute==NULL)
    {
        return;
    }

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    //add driver implementation code here
    x_memset(pAttribute, 0, sizeof(TUNER_TER_DIG_TUNE_STATUS_T));

    if((pDemod->system == SONY_DTV_SYSTEM_DVBT)||(pDemod->system == SONY_DTV_SYSTEM_DVBT2))
    {
        // BandWidth -----------------------------------------------------
        switch(pDemod->bandwidth){
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBT2_1_7MHZ
            case SONY_DTV_BW_1_7_MHZ:
            pAttribute->e_bandwidth = BW_1_7_MHz; break;
        #endif
            //case SONY_DTV_BW_5_MHZ:
            //    _pSignal->e_bandwidth = BW_5_MHz; break;
            case SONY_DTV_BW_6_MHZ:
            pAttribute->e_bandwidth = BW_6_MHz; break;
            case SONY_DTV_BW_7_MHZ:
            pAttribute->e_bandwidth = BW_7_MHz; break;
            case SONY_DTV_BW_8_MHZ:
            pAttribute->e_bandwidth = BW_8_MHz; break;
            default:
            pAttribute->e_bandwidth = BW_UNKNOWN; break;
        }
    }

    if(pDemod->system == SONY_DTV_SYSTEM_DVBT)
    {
        sony_result = sony_demod_dvbt_monitor_TPSInfo(pDemod, &tpsinfo);
        if(sony_result == SONY_RESULT_OK)
        {
            mcDBG_MSG_L1(("Guard : "));
            switch(tpsinfo.guard){
            case SONY_DVBT_GUARD_1_32:
                pAttribute->e_guard_int = GUARD_INT_1_32;
                mcDBG_MSG_L1(("1/32\n"));
                break;
            case SONY_DVBT_GUARD_1_16:
                pAttribute->e_guard_int = GUARD_INT_1_16;
                mcDBG_MSG_L1(("1/16\n"));
                break;
            case SONY_DVBT_GUARD_1_8:
                pAttribute->e_guard_int = GUARD_INT_1_8;
                mcDBG_MSG_L1(("1/8\n"));
                break;
            case SONY_DVBT_GUARD_1_4:
                pAttribute->e_guard_int = GUARD_INT_1_4;
                mcDBG_MSG_L1(("1/4\n"));
                break;
            default:
                pAttribute->e_guard_int = GUARD_INT_UNKNOWN;
                mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.guard));
            break;
            }

            mcDBG_MSG_L1(("Mode : "));
            switch(tpsinfo.mode){
            case SONY_DVBT_MODE_2K:
                pAttribute->e_trans_mode = TRANS_MODE_2_K;
                mcDBG_MSG_L1(("2k mode\n"));
                break;
            case SONY_DVBT_MODE_8K:
                pAttribute->e_trans_mode = TRANS_MODE_8_K;
                mcDBG_MSG_L1(("8k mode\n"));
                break;
            default:
                pAttribute->e_trans_mode = TRANS_MODE_UNDEFINED;
                mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.mode));
                break;
            }
        }
    }
    else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2)
    {
        sony_dvbt2_ofdm_t ofdmInfo;
        sony_dvbt2_plp_t PLPInfo;
        sony_dvbt2_l1pre_t l1Pre; //add for L1pre 20111226
        sony_dvbt2_l1post_t L1Post; //add for L1post 20111226

        sony_result = sony_demod_dvbt2_monitor_Ofdm(pDemod, &ofdmInfo);
        if(sony_result == SONY_RESULT_OK)
        {
            mcDBG_MSG_L1(("DVB-T2 OFDM Information ----\n"));
            if (ofdmInfo.bwExt==0)    //BW extend==Normal
            {
                mcDBG_MSG_L1(("Mode : "));
                switch(ofdmInfo.mode){
                case SONY_DVBT2_M2K:
                    pAttribute->e_t2_fft=CFFT_2K_N;
                    mcDBG_MSG_L1(("2K-N\n")); break;
                case SONY_DVBT2_M8K:
                    pAttribute->e_t2_fft=CFFT_8K_N;
                    mcDBG_MSG_L1(("8K-N\n")); break;
                case SONY_DVBT2_M4K:
                    pAttribute->e_t2_fft=CFFT_4K_N;
                    mcDBG_MSG_L1(("4K-N\n")); break;
                case SONY_DVBT2_M1K:
                    pAttribute->e_t2_fft=CFFT_1K_N;
                    mcDBG_MSG_L1(("1K-N\n")); break;
                case SONY_DVBT2_M16K:
                    pAttribute->e_t2_fft=CFFT_16K_N;
                    mcDBG_MSG_L1(("16K-N\n")); break;
                case SONY_DVBT2_M32K:
                    pAttribute->e_t2_fft=CFFT_32K_N;
                    mcDBG_MSG_L1(("32K-N\n")); break;
                default:
                    pAttribute->e_t2_fft=CFFT_XK_UNKNOWN;
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }
            }
            else if(ofdmInfo.bwExt==1) //BW extend==Extended
            {
                switch(ofdmInfo.mode){
                case SONY_DVBT2_M2K:
                    pAttribute->e_t2_fft=CFFT_2K_E;
                    mcDBG_MSG_L1(("2K-E\n")); break;
                case SONY_DVBT2_M8K:
                    pAttribute->e_t2_fft=CFFT_8K_E;
                    mcDBG_MSG_L1(("8K-E\n")); break;
                case SONY_DVBT2_M4K:
                    pAttribute->e_t2_fft=CFFT_4K_E;
                    mcDBG_MSG_L1(("4K-E\n")); break;
                case SONY_DVBT2_M1K:
                    pAttribute->e_t2_fft=CFFT_1K_E;
                    mcDBG_MSG_L1(("1K-E\n")); break;
                case SONY_DVBT2_M16K:
                    pAttribute->e_t2_fft=CFFT_16K_E;
                    mcDBG_MSG_L1(("16K-E\n")); break;
                case SONY_DVBT2_M32K:
                    pAttribute->e_t2_fft=CFFT_32K_E;
                    mcDBG_MSG_L1(("32K-E\n")); break;
                default:
                    pAttribute->e_t2_fft=CFFT_XK_UNKNOWN;
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }
            }//end for carrier type recognize 20111226

            mcDBG_MSG_L1(("GI : "));

            switch(ofdmInfo.gi){
            case SONY_DVBT2_G1_32:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_1_32;
                mcDBG_MSG_L1(("1/32\n")); break;
            case SONY_DVBT2_G1_16:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_1_16;
                mcDBG_MSG_L1(("1/16\n")); break;
            case SONY_DVBT2_G1_8:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_1_8;
                mcDBG_MSG_L1(("1/8\n")); break;
            case SONY_DVBT2_G1_4:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_1_4;
                mcDBG_MSG_L1(("1/4\n")); break;
            case SONY_DVBT2_G1_128:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_1_128;
                mcDBG_MSG_L1(("1/128\n")); break;
            case SONY_DVBT2_G19_128:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_19_128;
                mcDBG_MSG_L1(("19/128\n")); break;
            case SONY_DVBT2_G19_256:
                pAttribute->e_t2_guard_int = GUARD_INT_T2_19_256;
                mcDBG_MSG_L1(("19/256\n")); break;
            default:
                mcDBG_MSG_L1(("Unknown\n")); break;
            }

            mcDBG_MSG_L1(("Mixed : "));
            if(ofdmInfo.mixed)
            {
                mcDBG_MSG_L1(("Mixed\n"));
            }
            else
            {
                mcDBG_MSG_L1(("Not mixed\n"));
            }
         
            if(ofdmInfo.isMiso){
                pAttribute->eTransmisson=MISO;
                mcDBG_MSG_L1(("S1 : MISO\n"));
            }else{
                pAttribute->eTransmisson=SISO;
                mcDBG_MSG_L1(("S1 : SISO\n"));
            }

            mcDBG_MSG_L1(("PP : "));
            switch(ofdmInfo.pp){
            case SONY_DVBT2_PP1:
                pAttribute->ePilotPattern=PP1;
                mcDBG_MSG_L1(("PP1\n")); break;
            case SONY_DVBT2_PP2:
                pAttribute->ePilotPattern=PP2;
                mcDBG_MSG_L1(("PP2\n")); break;
            case SONY_DVBT2_PP3:
                pAttribute->ePilotPattern=PP3;
                mcDBG_MSG_L1(("PP3\n")); break;
            case SONY_DVBT2_PP4:
                pAttribute->ePilotPattern=PP4;
                mcDBG_MSG_L1(("PP4\n")); break;
            case SONY_DVBT2_PP5:
                pAttribute->ePilotPattern=PP5;
                mcDBG_MSG_L1(("PP5\n")); break;
            case SONY_DVBT2_PP6:
                pAttribute->ePilotPattern=PP6;
                mcDBG_MSG_L1(("PP6\n")); break;
            case SONY_DVBT2_PP7:
                pAttribute->ePilotPattern=PP7;
                mcDBG_MSG_L1(("PP7\n")); break;
            case SONY_DVBT2_PP8:
                pAttribute->ePilotPattern=PP8;
                mcDBG_MSG_L1(("PP8\n")); break;
            default:
                pAttribute->ePilotPattern=PPX_UNKNOWN;
                mcDBG_MSG_L1(("Unknown\n")); break;
            }
         
            mcDBG_MSG_L1(("BWExt : %u\n", ofdmInfo.bwExt));

            mcDBG_MSG_L1(("PAPR : "));
            switch(ofdmInfo.papr){
            case SONY_DVBT2_PAPR_0:
                pAttribute->ePapr=PAPR_NONE;
                mcDBG_MSG_L1(("None\n")); break;
            case SONY_DVBT2_PAPR_1:
                pAttribute->ePapr=PAPR_ACE;
                mcDBG_MSG_L1(("ACE\n")); break;
            case SONY_DVBT2_PAPR_2:
                pAttribute->ePapr=PAPR_TR;
                mcDBG_MSG_L1(("TR\n")); break;
            case SONY_DVBT2_PAPR_3:
                pAttribute->ePapr=PAPR_BOTH;
                mcDBG_MSG_L1(("ACE and TR\n")); break;
            default:
                pAttribute->ePapr=PAPR_UNKNOWN;
                mcDBG_MSG_L1(("Unknown\n")); break;
            }
         
            mcDBG_MSG_L1(("Num of Symbols : %u\n", ofdmInfo.numSymbols));
        }
        else
        {        
            mcDBG_MSG_L1(("Error in monitorT2_Ofdm\n"));
        }

        sony_result = sony_demod_dvbt2_monitor_ActivePLP(pDemod, SONY_DVBT2_PLP_DATA, &PLPInfo);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L1(("DVB-T2 PLP Information ----\n"));
            mcDBG_MSG_L1(("ID : %u\n", PLPInfo.id));
            pAttribute->ui1_CurrPlpId=PLPInfo.id;
            mcDBG_MSG_L1(("Type : "));
            switch(PLPInfo.type){
            case SONY_DVBT2_PLP_TYPE_COMMON:
                mcDBG_MSG_L1(("Common\n")); break;
            case SONY_DVBT2_PLP_TYPE_DATA1:
                mcDBG_MSG_L1(("Data Type 1\n")); break;
            case SONY_DVBT2_PLP_TYPE_DATA2:
                mcDBG_MSG_L1(("Data Type 2\n")); break;
            default:
                mcDBG_MSG_L1(("Unknown\n")); break;
            }

            mcDBG_MSG_L1(("Payload : "));
            switch(PLPInfo.payload){
            case SONY_DVBT2_PLP_PAYLOAD_GFPS:
                mcDBG_MSG_L1(("GFPS\n")); break;
            case SONY_DVBT2_PLP_PAYLOAD_GCS:
                mcDBG_MSG_L1(("GCS\n")); break;
            case SONY_DVBT2_PLP_PAYLOAD_GSE:
                mcDBG_MSG_L1(("GSE\n")); break;
            case SONY_DVBT2_PLP_PAYLOAD_TS:
                mcDBG_MSG_L1(("TS\n")); break;
            default:
                mcDBG_MSG_L1(("Unknown\n")); break;
            }

            mcDBG_MSG_L1(("FF : %u\n", PLPInfo.ff));
            mcDBG_MSG_L1(("First RF Index : %u\n", PLPInfo.firstRfIdx));
            mcDBG_MSG_L1(("First Frame Index : %u\n", PLPInfo.firstFrmIdx));
            mcDBG_MSG_L1(("Group ID : %u\n", PLPInfo.groupId));

            mcDBG_MSG_L1(("Constellation : "));
            //add for modulation mode recognize 20111226
            if(PLPInfo.rot==0)    //rotate not used
            {
                switch(PLPInfo.constell){
                case SONY_DVBT2_QPSK:
                    pAttribute->eModulation=MD_QPSK;
                    mcDBG_MSG_L1(("QPSK\n")); break;
                case SONY_DVBT2_QAM16:
                    pAttribute->eModulation=MD_16QAM;
                    mcDBG_MSG_L1(("16QAM\n")); break;
                case SONY_DVBT2_QAM64:
                    pAttribute->eModulation=MD_64QAM;
                    mcDBG_MSG_L1(("64QAM\n")); break;
                case SONY_DVBT2_QAM256:
                    pAttribute->eModulation=MD_256QAM;
                    mcDBG_MSG_L1(("256QAM\n")); break;
                default:
                    pAttribute->eModulation=MD_UNKNOWN;
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }
            }
            else if(PLPInfo.rot==1)//rotate used
            {
                switch(PLPInfo.constell){
                case SONY_DVBT2_QPSK:
                    pAttribute->eModulation=MD_QPSK_R;
                    mcDBG_MSG_L1(("QPSK-R\n")); break;
                case SONY_DVBT2_QAM16:
                    pAttribute->eModulation=MD_16QAM_R;
                    mcDBG_MSG_L1(("16QAM-R\n")); break;
                case SONY_DVBT2_QAM64:
                    pAttribute->eModulation=MD_64QAM_R;
                    mcDBG_MSG_L1(("64QAM-R\n")); break;
                case SONY_DVBT2_QAM256:
                    pAttribute->eModulation=MD_256QAM_R;
                    mcDBG_MSG_L1(("256QAM-R\n")); break;
                default:
                    pAttribute->eModulation=MD_UNKNOWN;
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }     
            }
            //end for modulation mode recognize 20111226
            mcDBG_MSG_L1(("Code Rate : "));
            switch(PLPInfo.plpCr){
            case SONY_DVBT2_R1_2:
                mcDBG_MSG_L1(("1/2\n")); break;
            case SONY_DVBT2_R3_5:
                mcDBG_MSG_L1(("3/5\n")); break;
            case SONY_DVBT2_R2_3:
                mcDBG_MSG_L1(("2/3\n")); break;
            case SONY_DVBT2_R3_4:
                mcDBG_MSG_L1(("3/4\n")); break;
            case SONY_DVBT2_R4_5:
                mcDBG_MSG_L1(("4/5\n")); break;
            case SONY_DVBT2_R5_6:
                mcDBG_MSG_L1(("5/6\n")); break;
            default:
                mcDBG_MSG_L1(("Unknown\n")); break;
            }

            mcDBG_MSG_L1(("Rotation : %s\n", PLPInfo.rot ? "Used" : "Not Used"));

            mcDBG_MSG_L1(("FEC Type : "));
            switch(PLPInfo.fec){
            case SONY_DVBT2_FEC_LDPC_16K:
                pAttribute->eFecMode=LDPC_SHORT;
                mcDBG_MSG_L1(("16K LDPC(SHORT)\n")); break;
            case SONY_DVBT2_FEC_LDPC_64K:
                pAttribute->eFecMode=LDPC_NORMAL;
                mcDBG_MSG_L1(("64K LDPC(NORMAL)\n")); break;
            default:
                pAttribute->eFecMode=LDPC_UNKNOWN;
                mcDBG_MSG_L1(("Unknown\n")); break;
            }

            mcDBG_MSG_L1(("Max Num of PLP Blocks : %u\n", PLPInfo.numBlocksMax));
            mcDBG_MSG_L1(("Frame Interval : %u\n", PLPInfo.frmInt));
            mcDBG_MSG_L1(("Time IL Length : %u\n", PLPInfo.tilLen));
            mcDBG_MSG_L1(("Time IL Type : %u\n", PLPInfo.tilType));
            mcDBG_MSG_L1(("In-Band Flag : %u\n", PLPInfo.inBandFlag));
        }
        else
        {
            mcDBG_MSG_L1(("Error in monitorT2_ActivePLP\n"));
        }
        //Add for L1Pre 20111226
        sony_result = sony_demod_dvbt2_monitor_L1Pre (pDemod, &l1Pre);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L1(("DVB-T2 L1Pre Information ----\n"));
            mcDBG_MSG_L1(("L1 modulation:  "));      
            switch(l1Pre.mod){
            case SONY_DVBT2_L1POST_BPSK:
                pAttribute->eL1Modulation=L1_BPSK;
                mcDBG_MSG_L1(("SONY_DVBT2_L1_BPSK\n")); break;
            case SONY_DVBT2_L1POST_QPSK:
                pAttribute->eL1Modulation=L1_QPSK;
                mcDBG_MSG_L1(("SONY_DVBT2_L1_QPSK\n")); break;
            case SONY_DVBT2_L1POST_QAM16:
                pAttribute->eL1Modulation=L1_16QAM;
                mcDBG_MSG_L1(("SONY_DVBT2_L1_QAM16\n")); break;
            case SONY_DVBT2_L1POST_QAM64:
                pAttribute->eL1Modulation=L1_64QAM;
                mcDBG_MSG_L1(("SONY_DVBT2_L1_QAM64\n")); break;
            default:
                pAttribute->eL1Modulation=L1_UNKNOWN;
                mcDBG_MSG_L1(("Unknown\n")); break;     
            }
        }
        else
        {
            mcDBG_MSG_L1(("Error in monitorT2_L1Pre\n"));
        }
        //end for L1Pre 20111226
        //Add for L1Post 20111226
 
        sony_result = sony_demod_dvbt2_monitor_L1Post(pDemod,&L1Post);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L1(("The frequency in Hz : %u\n", L1Post.freq));     
            mcDBG_MSG_L1((" FEF type : %u\n", L1Post.fefType));  
            mcDBG_MSG_L1((" FEF length : %u\n", L1Post.fefLength));
            if(L1Post.fefLength)
                pAttribute->eFef=FEF_YES;
            else
                pAttribute->eFef=FEF_NO;
            mcDBG_MSG_L1((" FEF interval : %u\n", L1Post.fefInterval));  
             
        }
        else
        {
            mcDBG_MSG_L1(("Error in monitorT2_L1Post\n"));
        }
 
        //end for L1Post 20111226
        // List PLP IDs
        {
            UINT8 aPLPId[255];
            UINT8 nNumPLPs = 0;
            UINT32 i = 0;
            sony_result = sony_demod_dvbt2_monitor_DataPLPs(pDemod, aPLPId, &nNumPLPs);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("DVB-T2 PLP ID List ----\n"));
                mcDBG_MSG_L1(("Num of PLPs : %u\n", nNumPLPs));
                pAttribute->ui1_plp_num=nNumPLPs;
                for(i=0; i<nNumPLPs; i++){
                    mcDBG_MSG_L1(("PLP(%u) : 0x%02x\n", i+1, aPLPId[i]));
                }
            }
            else
            {
                mcDBG_MSG_L1(("Error in monitorT2_DataPLPs\n"));
            }
        }
    }
    return;
}
#endif // CC_PROJECT_FOR_SONY

STATIC VOID vResetDemod(UINT16 u2DelayTime)
{
    UINT32 u4RstPinNum=201;

    // Config GPIO to reset demod
    mcDBG_MSG_L2(("vResetDemod\n"));
    if(!fgHWReset)
    {
        if (DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum) == 0)
        {
            GPIO_SetOut(u4RstPinNum, 0);
            x_thread_delay((UINT32)u2DelayTime);
            GPIO_SetOut(u4RstPinNum, 1);
            x_thread_delay((UINT32)20); // Delay 20 ms for CR[DTV00030459]. Or it will init fail in rel image
            mcDBG_MSG_L2(("Reset CXD2854 Demod GPIO=0x%x, 0->1, %dms\r\n",u4RstPinNum, u2DelayTime));
        }
    }
}

//-----------------------------------------------------------------------------
// DVB-C SQI/SSI (NOT FORMAL)
//-----------------------------------------------------------------------------
/* The following Quality monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation. */
static sony_result_t sony_demod_dvbc_monitor_Quality (sony_demod_t * pDemod, uint8_t * pQuality)
{
    uint32_t ber = 0;
    int32_t snr = 0;
    int32_t snRel = 0;
    int32_t berSQI = 0;
    sony_dvbc_constellation_t constellation;

    /* Nordig spec C/N (Es/No) minimum performance
     * Note: 32QAM isn't provided in the Nordig unified specification, so has been
     * Implemented based on interpolation and measurements.
     */
    static const int32_t cnrNordigdB1000[] = {
    /*  16QAM   32QAM   64QAM   128QAM  256QAM */
        20000,  23000,  26000,  29000,  32000  };

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_demod_dvbc_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_DTV_SYSTEM_DVBC) {
        /* Not DVB-C*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre-RS (Post-Viterbi) BER. */
    result = sony_demod_dvbc_monitor_PreRSBER (pDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR value. */
    result = sony_demod_dvbc_monitor_SNR (pDemod, &snr);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    /* Get Code rate */
    result = sony_demod_dvbc_monitor_QAM (pDemod, &constellation);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    /* Calculate. */
    snRel = snr - cnrNordigdB1000[(uint8_t)constellation];

    /* BER_SQI is calculated from:
     * if (BER > 10^-3)          : 0
     * if (10^-7 < BER <= 10^-3) : BER_SQI = 20*log10(1/BER) - 40
     * if (BER <= 10^-7)         : BER_SQI = 100
      */
    if (ber > 10000) {
        berSQI = 0;
    }
    else if (ber > 1) {
        /* BER_SQI = 20 * log10(1/BER) - 40
         * BER_SQI = 20 * (log10(1) - log10(BER)) - 40
         *
         * If BER in units of 1e-7
         * BER_SQI = 20 * (log10(1) - (log10(BER) - log10(1e7)) - 40
         *
         * BER_SQI = 20 * (log10(1e7) - log10(BER)) - 40
         * BER_SQI = 20 * (7 - log10 (BER)) - 40
         */
        berSQI = (int32_t) (10 * sony_math_log10 (ber));
        berSQI = 20 * (7 * 1000 - (berSQI)) - 40 * 1000;
    }
    else {
        berSQI = 100 * 1000;
    }

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -7dB)         : SQI = 0
     * if (-7dB <= C/Nrel < +3dB) : SQI = (((C/Nrel - 3) / 10) + 1) * BER_SQI
     * if (C/Nrel >= +3dB)        : SQI = BER_SQI
     */
    if (snRel < -7 * 1000) {
        *pQuality = 0;
    }
    else if (snRel < 3 * 1000) {
        int32_t tmpSQI = (((snRel - (3 * 1000)) / 10) + 1000);
        *pQuality = (uint8_t) (((tmpSQI * berSQI) + (1000000/2)) / (1000000)) & 0xFF;
    }
    else {
        *pQuality = (uint8_t) ((berSQI + 500) / 1000);
    }

    /* Clip to 100% */
    if (*pQuality > 100) {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

/* The following SSI monitor is provided as an example only as it is not based
 * on a formal specification. It is therefore advised that this is only used for
 * guidance, rather than an exact representation.
 *
 * NOTE : This function calls the integration layer RF Level monitor which is
 * HW implementation dependant, therefore the SSI level may be incorrect RF Level
 * is correctly characterised. */
static sony_result_t sony_integ_dvbc_monitor_SSI (sony_integ_t * pInteg, uint8_t * pSSI)
{
    sony_dvbc_constellation_t constellation;
    uint32_t symbolRate;
    int32_t prec;
    int32_t prel;
    int32_t pref;
    int32_t tempSSI = 0;
    int32_t noiseFigureDB1000;
    sony_result_t result = SONY_RESULT_OK;

    /* Nordig spec C/N (Es/No) minimum performance
     * Note: 32QAM isn't provided in the Nordig unified specification, so has been
     * Implemented based on interpolation and measurements.
     */
    static const int32_t cnrNordigdB1000[] = {
    /*  16QAM   32QAM   64QAM   128QAM  256QAM */
        20000,  23000,  26000,  29000,  32000  };

    SONY_TRACE_ENTER ("sony_integ_dvbc_monitor_SSI");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_DTV_SYSTEM_DVBC) {
        /* Not DVB-C*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level (for MTK) */
    result = sony_integ_monitor_RFLevel_MTK (pInteg, &prec);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Monitor constellation */
    result = sony_demod_dvbc_monitor_QAM (pInteg->pDemod, &constellation);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Monitor symbol rate */
    result = sony_demod_dvbc_monitor_SymbolRate(pInteg->pDemod, &symbolRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Modify this to suit the tuner noise figure specification value in dB * 1000 */
    noiseFigureDB1000 = 9000;

    /* Reference sensitivity limit is calcualted from:
     * RefLevel (dB) = (10 * Log(1.38*10^-23 * 290)) + 30 + NoiseFigure + (10 * Log(symbolRate)) + C/N_Offset
     *  - sony_math_log10(x)     = 100 * Log(x)
     *  - Log(1.38*10^-23 * 290) = -20.3977
     *
     * Therefore:
     * RefLevel (dB * 1000) = -203977 + 30000 + (1000 * NoiseFigure) + (100 * Log(symbolRate)) + (1000 * C/N_Offset)
     */
    pref = -203977 + 30000 + noiseFigureDB1000 + (100 * sony_math_log10(symbolRate* 1000)) + cnrNordigdB1000[(uint8_t)constellation];

    /* prel = prec - pref */
    prel = prec - pref;

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)              SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = 4 * prel + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    SONY_TRACE_RETURN (result);
}

//-----------------------------------------------------------------------------
// DVB-C2 SQI/SSI (NOT FORMAL)
//-----------------------------------------------------------------------------
/**
 @brief The following Quality monitor is provided as an example only as it is not based
        on a formal specification. It is therefore advised that this is only used for
        guidance, rather than an exact representation.

        For improved accuracy the C/N ref table values can be characterised on your own
        implementation.  Nordig specification values are currently used, but may not give
        an accurate representation of your specific hardware capabilities.

 @param pDemod The demodulator instance.
 @param pQuality The quality as a percentage (0-100).

 @return SONY_RESULT_OK if successful and pQuality valid.
*/
static sony_result_t sony_demod_dvbc2_monitor_Quality (sony_demod_t * pDemod, uint8_t * pQuality)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_dvbc2_plp_code_rate_t codeRate;
    sony_dvbc2_constellation_t constellation;
    uint32_t ber = 0;
    int32_t snr = 0;
    int32_t snrRel = 0;
    int32_t berSQI = 0;

    /* The list of DVB-C2 SNR reciever performance requirement values in dBx1000.
     * Entries equalling 0 indicate invalid code rate and constellation combination.
     */
    static const int32_t snrRefdB1000[6][7] = {
        /* RSVD1,  2/3,    3/4,    4/5,    5/6,    8/9,    9/10*/
        {  0,      0,      0,      0,      0,      0,      0},     /* RSVD1 */
        {  0,      0,      0,      12900,  0,      15000,  15000}, /* 16-QAM */
        {  0,      15600,  0,      18200,  0,      20700,  20700}, /* 64-QAM */
        {  0,      0,      22200,  0,      24300,  26500,  26500}, /* 256-QAM */
        {  0,      0,      27300,  0,      30300,  33600,  33600}, /* 1024-QAM */
        {  0,      0,      0,      0,      39900,  41800,  41800}  /* 4096-QAM */
    };

    SONY_TRACE_ENTER ("sony_demod_dvbc2_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_DTV_SYSTEM_DVBC2) {
        /* Not DVB-C2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get PLP Code Rate */
    result = sony_demod_dvbc2_monitor_CodeRate (pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &codeRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP constellation */
    result = sony_demod_dvbc2_monitor_QAM (pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &constellation);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get Pre-BCH (Post-LDPC) BER */
    result = sony_demod_dvbc2_monitor_PreBCHBER (pDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR value */
    result = sony_demod_dvbc2_monitor_SNR(pDemod, &snr);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN(result);
    }

    /* Ensure correct code rate and constellation values. */
    if ((codeRate >= SONY_DVBC2_R_RSVD2) || (constellation >= SONY_DVBC2_CONSTELLATION_RSVD2)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* BER_SQI Calculated from:
     * if (Pre-BCH BER > 10^-4)            BER_SQI = 0
     * if (10^-4 >= Pre-BCH BER >= 10^-7)  BER_SQI = 100/15 = 6.667
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

    /* C/Nrel = C/Nrec - C/Nnordigp1 */
    snrRel = snr - snrRefdB1000[constellation][codeRate];

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -3dB)         SQI = 0
     * if (-3dB <= CNrel <= 3dB)  SQI = (C/Nrel + 3) * BER_SQI
     * if (CNrel > 3dB)           SQI = 100
     */
    if (snrRel < -3000) {
        *pQuality = 0;
    }
    else if (snrRel <= 3000) {
        /* snrRel and berSQI scaled by 10^3 so divide by 10^6 */
        uint32_t tempSQI = (((snrRel + 3000) * berSQI) + 500000) / 1000000;
        /* Clip value to 100% */
        *pQuality = (tempSQI > 100)? 100 : (uint8_t) tempSQI;
    }
    else {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

/**
 @brief The following SSI monitor is provided as an example only as it is not based
        on a formal specification. It is therefore advised that this is only used for
        guidance, rather than an exact representation.

        For improved accuracy the Level ref table values can be characterised on your own
        implementation.  Proposed specification values are currently used, but may not give
        an accurate representation of your specific hardware capabilities.

 @note  This function calls the integration layer RF Level monitor which is
        HW implementation dependant, therefore the SSI level may be incorrect RF Level
        is correctly characterised.

 @param pInteg The driver object
 @param pSSI Signal Strength Indicator percentage.

 @return SONY_RESULT_OK if successful.
*/
static sony_result_t sony_integ_dvbc2_monitor_SSI (sony_integ_t * pInteg, uint8_t * pSSI)
{
    int32_t rfLevel;
    sony_dvbc2_plp_code_rate_t codeRate;
    sony_dvbc2_constellation_t constellation;
    int32_t prel;
    int32_t tempSSI = 0;
    sony_result_t result = SONY_RESULT_OK;

    static const int32_t rfLevelRefdBm1000[6][7] = {
        /* RSVD1,  2/3,    3/4,    4/5,    5/6,    8/9,    9/10*/
        {  0,      0,      0,      0,      0,      0,      0},      /* RSVD1 */
        {  0,      0,      0,      -84300, 0,      -82200, -82200}, /* 16-QAM */
        {  0,      -81600, 0,      -78900, 0,      -76500, -76500}, /* 64-QAM */
        {  0,      0,      -74900, 0,      -72800, -70600, -70600}, /* 256-QAM */
        {  0,      0,      -69800, 0,      -66800, -63600, -63600}, /* 1024-QAM */
        {  0,      0,      0,      0,      -57300, -55300, -55300}  /* 4096-QAM */
    };

    SONY_TRACE_ENTER ("sony_integ_dvbc2_monitor_SSI");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_DTV_SYSTEM_DVBC2) {
        /* Not DVB-C2*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level (for MTK) */
    result = sony_integ_monitor_RFLevel_MTK (pInteg, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP Code Rate */
    result = sony_demod_dvbc2_monitor_CodeRate (pInteg->pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &codeRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP constellation */
    result = sony_demod_dvbc2_monitor_QAM (pInteg->pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &constellation);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct code rate and constellation values. */
    if ((codeRate >= SONY_DVBC2_R_RSVD2) || (constellation >= SONY_DVBC2_CONSTELLATION_RSVD2)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* prel = prec - pref */
    prel = rfLevel - rfLevelRefdBm1000[constellation][codeRate];

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)              SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = 4 * prel + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    SONY_TRACE_RETURN (result);
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
static INT32 DVBT2_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    if(fgDemodInited == TRUE)
    {
        fgDemodInited = FALSE; 
    }
    else
    {
        mcDBG_MSG_L1(("CXD2854 (DVB) had been closed!\n"));
        return (DRVAPI_TUNER_OK);
    }
    
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    sony_integ_SleepT_C(pInteg);

    x_sema_delete(pPdCtxDVBT2->hHalLock);
    x_mem_free(pPdCtxDVBT2);

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
 *  @retval  1                  Demod Lock
 *  @retval  0                  Demod Unlock
 */
//-------------------------------------------------------------------------
static BOOL DVBT2_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    UINT8  fgChannelScan = FALSE;
    BOOL fgLock = 0;
    UINT32  u4TickCnt, u4ConnTime;

    // CXD2854 driver specific
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    mcDBG_MSG_L2((SONY_CXD2854_DVBTT2CC2_DRIVER_VERSION_STR "\n"));
    mcDBG_MSG_L2(("DVBT2_TunerAcq--------->>>>>>>>>>>>\n"));

    //MUTEX lock
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    // Save break function pointer/argument here to use in stopwatch function.
    // Set break function pointer to the member to call it from integ APIs.
    g_sony_demod_ctx.breakFct = _BreakFct;
    g_sony_demod_ctx.breakFctArg = pvArg;
    // Clear cancel flag
    sony_atomic_set (&(pInteg->cancel), 0);

    if(pPdCtxDVBT2->fgDisStatus == TRUE){
        /* This means that terrestrial driver was disconnected now. */
        /* Check register to check that re-initialization is necessary or not. */
        sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
        uint8_t data = 0;

        sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in WriteOneRegister\n", sony_FormatResult(sony_result)));
        }

        sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, &data, 1);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in ReadRegister\n", sony_FormatResult(sony_result)));
        }

        if(data == 0x01){
            /* Demod HW is now in Shutdown state. */
            mcDBG_MSG_L1(("Return back from Shutdown.\n"));
            sony_result = sony_demod_ReinitializeT_C(pDemod);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_ReinitializeT_C (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }
        }else{
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, &data, 1);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_ERR(("Error in ReadRegister\n", sony_FormatResult(sony_result)));
            }

            if((data != 0x01) && (data != 0x02) && (data != 0x04) && (data != 0x05) && (data != 0x06)){
                /* Demod HW is now in not DVB-T/T2/C/C2 and ISDB-T state. ReInitialize demodulator. */
                /* This case occurs in Terrestrial/Cable and Satellite combined demodulator. */
                mcDBG_MSG_L1(("Switch to Terrestrial/Cable.\n"));
                sony_result = sony_demod_ReinitializeT_C(pDemod);
                if(sony_result != SONY_RESULT_OK){
                    mcDBG_MSG_L1(("Error in sony_demod_ReinitializeT_C (%s)\n", sony_FormatResult(sony_result)));
                    fgLock = 0;
                    goto ErrorExit;
                }
            }
        }
    }

    u4TickCnt = x_os_get_sys_tick();

    if(e_conn_type == TUNER_CONN_TYPE_CAB_DIG){ // DVB-C/C2
        //wenming 2012-3-31, set pt_tuner_info by e_conn_type!!!
        TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_CAB_DIG_TUNE_INFO_T *) pv_conn_info;

        if(CONN_MODE_SCAN == pt_tuner_info->e_conn_mode)
        {
            fgChannelScan = TRUE;
            fgChanScanforGetAtt = TRUE;
        }
        else
        {
            fgChanScanforGetAtt = FALSE;
        }
        g_sony_demod_ctx.frequency = pt_tuner_info->ui4_freq/1000;

        // Try DVB-C tune
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
        if((pt_tuner_info->e_dvb_c_c2_mode == TUNER_DVB_C) || (pt_tuner_info->e_dvb_c_c2_mode == TUNER_DVB_AUTO_C_C2))
        #endif
        {
            sony_dvbc_tune_param_t tune_param; // tune param struct for DVB-C
            tune_param.centerFreqKHz = pt_tuner_info->ui4_freq/1000;
            tune_param.bandwidth = SONY_DTV_BW_8_MHZ;

            // Normal Tune : Disable DVB-C Early Unlock Detection.
            // Scan        : Enable DVB-C Early Unlock Detection.
            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_DVBC, fgChannelScan ? 0x01 : 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }

            mcDBG_MSG_L2(("Freq = %d\n", tune_param.centerFreqKHz));
            mcDBG_MSG_L2(("System = DVB-C\n"));
            mcDBG_MSG_L2(("Bandwidth = 8\n"));
            if(fgChannelScan){
                mcDBG_MSG_L2(("Channel Scan\n"));
            }else{
                mcDBG_MSG_L2(("Normal Tune\n"));
            }

            sony_result = sony_integ_dvbc_Tune(pInteg, &tune_param);
            switch(sony_result){
            case SONY_RESULT_OK:
                mcDBG_MSG_L2(("DVB-C TS Locked.\n"));
                fgLock = 1;
                break;
            case SONY_RESULT_ERROR_UNLOCK:
                mcDBG_MSG_L2(("DVB-C TS Unlocked.\n"));
                fgLock = 0;
                break;
            case SONY_RESULT_ERROR_TIMEOUT:
                mcDBG_MSG_L2(("DVB-C Wait TS Lock but Timeout.\n"));
                fgLock = 0;
                break;
            default:
                mcDBG_MSG_L1(("Error in sony_integ_dvbc_Tune (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                break;
            }
        }

        #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
        // Try DVB-C2 tune
        if((fgLock == 0) && ((pt_tuner_info->e_dvb_c_c2_mode == TUNER_DVB_C2) || (pt_tuner_info->e_dvb_c_c2_mode == TUNER_DVB_AUTO_C_C2))){
            g_sony_demod_ctx.c2DsId = 0;
            g_sony_demod_ctx.c2PlpId = 0;

            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_DVBC2, fgChannelScan ? 0x01 : 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }

            if(fgChannelScan){
                // For C2 scanning.
                // Tune specified frequency to get L1 part2 information and get tuning parameter of finding C2 system.
                // Following parameter will be used.
                //   pt_tuner_info->ui4_freq
                //   pt_tuner_info->e_bw
                // If C2 system is found, the information is stored to
                //   g_sony_demod_ctx.c2DsPlpList     -> MW can get by DVBT2_TunerGetAttribute2(TUNER_GET_TYPE_DVBC2_DS_PLP)
                //   g_sony_demod_ctx.c2NextFrequency -> MW can get by DVBT2_TunerGetAttribute2(TUNER_GET_TYPE_DVBC2_NEXT_FREQ)
                uint32_t frequencyKHz = 0;
                sony_dtv_bandwidth_t bandwidth = SONY_DTV_BW_8_MHZ;
                sony_dvbc2_tune_param_t * ptune_param = (sony_dvbc2_tune_param_t *)x_mem_calloc(256,sizeof(sony_dvbc2_tune_param_t));
                uint16_t num_tune_param = 0;
                uint32_t next_frequency = 0;
                int i = 0;

                x_memset(ptune_param, 0, sizeof(sony_dvbc2_tune_param_t)*256);
                frequencyKHz = (uint32_t)(pt_tuner_info->ui4_freq/1000);
                switch(pt_tuner_info->e_bw){
                case BW_6_MHz:
                    bandwidth = SONY_DTV_BW_6_MHZ; break;
                case BW_8_MHz:
                default:
                    bandwidth = SONY_DTV_BW_8_MHZ; break;
                }

                mcDBG_MSG_L2(("Freq = %d kHz\n", frequencyKHz));
                mcDBG_MSG_L2(("System = DVB-C2 (Scan)\n"));
                mcDBG_MSG_L2(("Bandwidth = %d\n", bandwidth));
                mcDBG_MSG_L2(("Channel Scan\n"));

                sony_result = sony_integ_dvbc2_GetChannelTuneParameters(pInteg, frequencyKHz, bandwidth,
                    ptune_param, &num_tune_param, &next_frequency);
                switch(sony_result){
                case SONY_RESULT_OK:
                    mcDBG_MSG_L2(("DVB-C2 Locked. (Signal Found)\n"));
                    fgLock = 1;

                    mcDBG_MSG_L2(("Number of Tune Params = %d\n", num_tune_param));
                    mcDBG_MSG_L2(("Next Freq = %d kHz\n", next_frequency));

                    g_sony_demod_ctx.c2DsPlpList.e_bw = pt_tuner_info->e_bw;
                    g_sony_demod_ctx.c2DsPlpList.ui2_curr_num = (UINT16)num_tune_param;
                    g_sony_demod_ctx.c2NextFrequency = (UINT32)(next_frequency * 1000);

                    for(i = 0; i < (UINT16)num_tune_param; i++){
                        mcDBG_MSG_L2(("[%d] Freq = %d kHz\n", i, ptune_param[i].c2TuningFrequencyKHz));
                        mcDBG_MSG_L2(("[%d] DS ID = %d\n", i, ptune_param[i].dataSliceId));
                        mcDBG_MSG_L2(("[%d] PLP ID = %d\n", i, ptune_param[i].dataPLPId));
                        mcDBG_MSG_L2(("[%d] IsDependentStaticDS = %d\n", i, ptune_param[i].isDependentStaticDS));

                        // Store the information to g_sony_demod_ctx
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui4_c2_tuning_freq = (UINT32)(ptune_param[i].c2TuningFrequencyKHz * 1000);
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui1_ds_id = (UINT8)ptune_param[i].dataSliceId;
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui1_plp_id = (UINT8)ptune_param[i].dataPLPId;
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].b_dependent_static_ds = (BOOL)ptune_param[i].isDependentStaticDS;
                    }
                    break;
                case SONY_RESULT_ERROR_UNLOCK:
                    mcDBG_MSG_L2(("DVB-C2 Unlocked. (Signal Not Found)\n"));
                    fgLock = 0;
                    g_sony_demod_ctx.c2NextFrequency = (UINT32)(next_frequency * 1000);
                    break;
                case SONY_RESULT_ERROR_TIMEOUT:
                    mcDBG_MSG_L2(("DVB-C2 Wait Lock but Timeout. (Signal Not Found)\n"));
                    fgLock = 0;
                    g_sony_demod_ctx.c2NextFrequency = (UINT32)(next_frequency * 1000);
                    break;
                default:
                    mcDBG_MSG_L1(("Error in sony_integ_dvbc2_GetChannelTuneParameters (%s)\n", sony_FormatResult(sony_result)));
                    fgLock = 0;
                    break;
                }
                x_mem_free(ptune_param);
            }else{
                // Normal C2 tuning.
                // Following parameter will be used.
                //   pt_tuner_info->ui4_freq
                //   pt_tuner_info->e_bw
                //   pt_tuner_info->ui1_ds_id
                //   pt_tuner_info->ui1_plp_id
                //   pt_tuner_info->b_dependent_static_ds
                sony_dvbc2_tune_param_t tune_param;
                tune_param.c2TuningFrequencyKHz = pt_tuner_info->ui4_freq/1000;
                switch(pt_tuner_info->e_bw){
                case BW_6_MHz:
                    tune_param.bandwidth = SONY_DTV_BW_6_MHZ; break;
                case BW_8_MHz:
                default:
                    tune_param.bandwidth = SONY_DTV_BW_8_MHZ; break;
                }
                tune_param.dataSliceId = (uint8_t)pt_tuner_info->ui1_ds_id;
                tune_param.dataPLPId = (uint8_t)pt_tuner_info->ui1_plp_id;
                tune_param.isDependentStaticDS = (uint8_t)pt_tuner_info->b_dependent_static_ds;
                tune_param.rfTuningFrequencyKHz = 0;

                mcDBG_MSG_L2(("Freq = %d kHz\n", tune_param.c2TuningFrequencyKHz));
                mcDBG_MSG_L2(("System = DVB-C2\n"));
                mcDBG_MSG_L2(("Bandwidth = %d\n", tune_param.bandwidth));
                mcDBG_MSG_L2(("DS ID = %d\n", tune_param.dataSliceId));
                mcDBG_MSG_L2(("PLP ID = %d\n", tune_param.dataPLPId));
                mcDBG_MSG_L2(("IsDependentStaticDS = %d\n", tune_param.isDependentStaticDS));
                mcDBG_MSG_L2(("Normal Tune\n"));

                // Store DS ID and PLP ID for DVBT2_TunerGetSignal
                g_sony_demod_ctx.c2DsId = (UINT8)tune_param.dataSliceId;
                g_sony_demod_ctx.c2PlpId = (UINT8)tune_param.dataPLPId;

                sony_result = sony_integ_dvbc2_Tune(pInteg, &tune_param);
                switch(sony_result){
                case SONY_RESULT_OK:
                    mcDBG_MSG_L2(("DVB-C2 TS Locked.\n"));
                    fgLock = 1;
                    break;
                case SONY_RESULT_ERROR_UNLOCK:
                    mcDBG_MSG_L2(("DVB-C2 TS Unlocked.\n"));
                    fgLock = 0;
                    break;
                case SONY_RESULT_ERROR_TIMEOUT:
                    mcDBG_MSG_L2(("DVB-C2 Wait TS Lock but Timeout.\n"));
                    fgLock = 0;
                    break;
                default:
                    mcDBG_MSG_L1(("Error in sony_integ_dvbc2_Tune (%s)\n", sony_FormatResult(sony_result)));
                    fgLock = 0;
                    break;
                }
            }
        }
        #endif
    }
    else if(e_conn_type == TUNER_CONN_TYPE_TER_DIG){ // DVB-T/T2
        //wenming 2012-3-31, set pt_tuner_info by e_conn_type!!!
        TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *) pv_conn_info;
        if(MOD_UNKNOWN == pt_tuner_info->e_mod) //wenming 2012-3-31: e_mod will be used to decide scan or not for DVBT, still need to discuss with MW if sync with DBVC method
        {
            fgChannelScan = TRUE;
            fgChanScanforGetAtt = TRUE;
        }
        else
        {
            fgChanScanforGetAtt = FALSE;
        }
        g_sony_demod_ctx.frequency = pt_tuner_info->ui4_freq/1000;

        // DVB-T BER period setting for stablizing SQI
        //  - Normal Tune: BER period is set to 16 for SQI stabilization.
        //  - Scan       : BER period is set to 14 for getting SQI quickly.
        sony_result = sony_demod_SetConfig(pInteg->pDemod, SONY_DEMOD_CONFIG_DVBT_BERN_PERIOD, fgChannelScan ? 14 : 16);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_SetConfig (%s).\n", sony_FormatResult(sony_result)));
            fgLock = 0;
            goto ErrorExit;
        }

        // Try DVB-T tune
        if((pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_T) || (pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_AUTO_T_T2)){
            sony_dvbt_tune_param_t tune_param; // tune param struct for DVB-T

            // Normal Tune : Wait TS Lock.
            // Scan        : Wait Demod Lock.
            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_DVBT, fgChannelScan ? 0x01 : 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }

            tune_param.centerFreqKHz = pt_tuner_info->ui4_freq/1000;
            switch(pt_tuner_info->e_bandwidth){
            case BW_6_MHz:
                tune_param.bandwidth = SONY_DTV_BW_6_MHZ; break;
            case BW_7_MHz:
                tune_param.bandwidth = SONY_DTV_BW_7_MHZ; break;
            case BW_8_MHz:
            default:
                tune_param.bandwidth = SONY_DTV_BW_8_MHZ; break;
            }
            if (pt_tuner_info->e_hierarchy_priority == HIR_PRIORITY_LOW) {
                tune_param.profile = SONY_DVBT_PROFILE_LP;
            } else {
                tune_param.profile = SONY_DVBT_PROFILE_HP;
            }

            mcDBG_MSG_L2(("Freq = %d\n", tune_param.centerFreqKHz));
            mcDBG_MSG_L2(("System = DVB-T\n"));
            mcDBG_MSG_L2(("Bandwidth = %d\n", tune_param.bandwidth));
            mcDBG_MSG_L2(("Profile = %d\n", tune_param.profile));
            if(fgChannelScan){
                mcDBG_MSG_L2(("Channel Scan\n"));
            }else{
                mcDBG_MSG_L2(("Normal Tune\n"));
            }

            sony_result = sony_integ_dvbt_Tune(pInteg, &tune_param);
            switch(sony_result){
            case SONY_RESULT_OK:
                mcDBG_MSG_L2(("DVB-T Locked.\n"));
                fgLock = 1;
                break;
            case SONY_RESULT_ERROR_UNLOCK:
                mcDBG_MSG_L2(("DVB-T Unlocked.\n"));
                fgLock = 0;
                break;
            case SONY_RESULT_ERROR_TIMEOUT:
                mcDBG_MSG_L2(("DVB-T Wait Lock but Timeout.\n"));
                fgLock = 0;
                break;
            default:
                mcDBG_MSG_L2(("Error in sony_integ_dvbt_Tune (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                break;
            }
        }

        // Try DVB-T2 tune
        if((fgLock == 0) && ((pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_T2) || (pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_AUTO_T_T2))){
            sony_dvbt2_tune_param_t tune_param; // tune param struct for DVB-T2

            // Normal Tune : Wait TS Lock.
            // Scan        : Wait Demod Lock.
            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_DVBT2, fgChannelScan ? 0x01 : 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }

            tune_param.centerFreqKHz = pt_tuner_info->ui4_freq/1000;
            switch(pt_tuner_info->e_bandwidth){
            #ifdef CC_SONY_DEMOD_SUPPORT_DVBT2_1_7MHZ
            case BW_1_7_MHz:
                tune_param.bandwidth = SONY_DTV_BW_1_7_MHZ; break;
            #endif
            case BW_6_MHz:
                tune_param.bandwidth = SONY_DTV_BW_6_MHZ; break;
            case BW_7_MHz:
                tune_param.bandwidth = SONY_DTV_BW_7_MHZ; break;
            case BW_8_MHz:
            default:
                tune_param.bandwidth = SONY_DTV_BW_8_MHZ; break;
            }
            tune_param.dataPlpId = pt_tuner_info->ui1_plp_id;
            tune_param.profile = SONY_DVBT2_PROFILE_BASE; // T2-base profile support.
            tune_param.tuneInfo = SONY_DEMOD_DVBT2_TUNE_INFO_OK; //wenming 2012-7-4: fix building error

            mcDBG_MSG_L2(("Freq = %d\n", tune_param.centerFreqKHz));
            mcDBG_MSG_L2(("System = DVB-T2\n"));
            mcDBG_MSG_L2(("Bandwidth = %d\n", tune_param.bandwidth));
            mcDBG_MSG_L2(("PLP ID = %d\n", tune_param.dataPlpId));
            if(fgChannelScan){
                mcDBG_MSG_L2(("Channel Scan\n"));
            }else{
                mcDBG_MSG_L2(("Normal Tune\n"));
            }

            sony_result = sony_integ_dvbt2_Tune(pInteg, &tune_param);
            switch(sony_result){
            case SONY_RESULT_OK:
                mcDBG_MSG_L2(("DVB-T2 Locked.\n"));
                fgLock = 1;
                break;
            case SONY_RESULT_ERROR_UNLOCK:
                mcDBG_MSG_L2(("DVB-T2 Unlocked.\n"));
                fgLock = 0;
                break;
            case SONY_RESULT_ERROR_TIMEOUT:
                mcDBG_MSG_L2(("DVB-T2 Wait Lock but Timeout.\n"));
                fgLock = 0;
                break;
            default:
                mcDBG_MSG_L2(("Error in sony_integ_dvbt2_Tune (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                break;
            }

            if(fgLock == 1){
                if(tune_param.tuneInfo == SONY_DEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID){
                    mcDBG_MSG_L2(("Warning: PLP ID is invalid.\n"));
                    if(!g_sony_demod_ctx.autoPLP){
                        fgLock = 0;
                    }
                }
            }
        }
    }else{
        mcDBG_MSG_L1(("Error TUNER CONN TYPE = %d!\n",e_conn_type));
        goto ErrorExit;
    }

    //Check tuner conncection time
    u4ConnTime = (x_os_get_sys_tick() - u4TickCnt) * x_os_get_sys_tick_period();
    mcDBG_MSG_L2(("DVBT2 acquisition time = %dms\n", u4ConnTime));

ErrorExit:
    // Clear break function pointer
    g_sony_demod_ctx.breakFct = NULL;
    g_sony_demod_ctx.breakFctArg = NULL;

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    /* Init Disconnect Status */
    pPdCtxDVBT2->fgDisStatus = FALSE;

    return fgLock; //if lock return 1, unlock return 0
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
#ifdef  INTERNAL_3RD_DEMOD
static INT16 DVBT2_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
#else
static BOOL DVBT2_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
#endif
{
    UINT16 u2Ret = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L4(("%s \n", __FUNCTION__));

    {
        uint8_t syncStat = 0;
        uint8_t tsLockStat = 0;
        uint8_t unlockDetect = 0;

        switch(pDemod->system){
        case SONY_DTV_SYSTEM_DVBT:
            sony_result = sony_demod_dvbt_monitor_SyncStat(pDemod, &syncStat, &tsLockStat, &unlockDetect);
            break;
        case SONY_DTV_SYSTEM_DVBT2:
            sony_result = sony_demod_dvbt2_monitor_SyncStat(pDemod, &syncStat, &tsLockStat, &unlockDetect);
            break;
        case SONY_DTV_SYSTEM_DVBC:
            sony_result = sony_demod_dvbc_monitor_SyncStat(pDemod, &syncStat, &tsLockStat, &unlockDetect);
            break;
        case SONY_DTV_SYSTEM_DVBC2:
            sony_result = sony_demod_dvbc2_monitor_SyncStat(pDemod, &syncStat, &tsLockStat, &unlockDetect);
            break;
        default:
            sony_result = SONY_RESULT_ERROR_HW_STATE; // temporaly
            break;
        }

        if((sony_result == SONY_RESULT_OK) && tsLockStat){
            u2Ret = 1;
        }else{
            u2Ret = 0;
        }
    }

    if((pDemod->system == SONY_DTV_SYSTEM_DVBT2) && (!g_sony_demod_ctx.autoPLP)){
        // Check PLP ID error
        UINT8 plpError = 0;
        mcDBG_MSG_L4(("PLP ID error check (while autoPLP == 0)\n"));
        sony_result = sony_demod_dvbt2_monitor_DataPLPError(pDemod, &plpError);
        if((sony_result != SONY_RESULT_OK) || plpError){
            u2Ret = 0;
            mcDBG_MSG_L4(("PLP ID error detected (while autoPLP == 0)\n"));
        }
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    #ifdef  INTERNAL_3RD_DEMOD
    return u2Ret;
    #else
    return (BOOL)u2Ret;
    #endif
}

//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  void
 */
//-------------------------------------------------------------------------
static VOID  DVBT2_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    if(pPdCtxDVBT2->fgDisStatus == TRUE)
    {
        /* Has Disconnected, return */
        mcDBG_MSG_L2(("DVBT2 Has Disconnect!\n"));
        return;
    }
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    sony_integ_SleepT_C(pInteg);

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    /* set disconnect status */
    pPdCtxDVBT2->fgDisStatus = TRUE;
}



/*<<<<<<<<< Need to do further implementation <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
static void DVBT2_TunerBypassI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)
{
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    sony_demod_I2cRepeaterEnable(pDemod, bSwitchOn ? 0x01 : 0x00);
    mcDBG_MSG_L2(("[CXD2854]Testlog:Now is set I2C bypass: %d!!\n", bSwitchOn));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
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
static void DVBT2_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    uint32_t symRate = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    _pSignal->u1DVBT_T2_indicator = TUNER_DVB_AUTO_T_T2;
    #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
    _pSignal->u1DVBC_C2_indicator = TUNER_DVB_AUTO_C_C2;
    #endif

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    //add driver implementation code here
    x_memset(_pSignal, 0, sizeof(SIGNAL));

    switch(pDemod->system){
    case SONY_DTV_SYSTEM_DVBT:
        mcDBG_MSG_L2(("system = DVB-T\n")); break;
    case SONY_DTV_SYSTEM_DVBT2:
        mcDBG_MSG_L2(("system = DVB-T2\n")); break;
    case SONY_DTV_SYSTEM_DVBC:
        mcDBG_MSG_L2(("system = DVB-C\n")); break;
    case SONY_DTV_SYSTEM_DVBC2:
        mcDBG_MSG_L2(("system = DVB-C2\n")); break;
    default:
        mcDBG_MSG_L2(("system = Unknown\n")); break;
    }

    // Frequency -----------------------------------------------------
    _pSignal->Frequency = g_sony_demod_ctx.frequency;

    if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
        int32_t i4Freqoffset = 0;//by Ray.WANG 20130606 for DVBC Offset compensation
        // DVB-C bandwidth is 8MHz
        _pSignal->e_bandwidth = BW_8_MHz;
        //Offset compensation---------------------------------------------
        //by Ray.WANG 20130606 (initially the issue from CVT and TCL)
        sony_result=sony_demod_dvbc_monitor_CarrierOffset( pDemod, &i4Freqoffset);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("DVBC frequency offset is %d !\n", i4Freqoffset));
            _pSignal->Frequency += i4Freqoffset;
        }

    }
    else
    {
        // BandWidth -----------------------------------------------------
        switch(pDemod->bandwidth){
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBT2_1_7MHZ
        case SONY_DTV_BW_1_7_MHZ:
            _pSignal->e_bandwidth = BW_1_7_MHz; break;
        #endif
        //case SONY_DTV_BW_5_MHZ:
        //    _pSignal->e_bandwidth = BW_5_MHz; break;
        case SONY_DTV_BW_6_MHZ:
            _pSignal->e_bandwidth = BW_6_MHz; break;
        case SONY_DTV_BW_7_MHZ:
            _pSignal->e_bandwidth = BW_7_MHz; break;
        case SONY_DTV_BW_8_MHZ:
            _pSignal->e_bandwidth = BW_8_MHz; break;
        default:
            _pSignal->e_bandwidth = BW_UNKNOWN; break;
        }
    }

    // QAMSize / Hierarchy / PLP -----------------------------------------------
    if(pDemod->system == SONY_DTV_SYSTEM_DVBT){
        sony_dvbt_tpsinfo_t tpsInfo;

        _pSignal->u1DVBT_T2_indicator = TUNER_DVB_T;

        sony_result = sony_demod_dvbt_monitor_TPSInfo(pDemod, &tpsInfo);
        if(sony_result == SONY_RESULT_OK){
            switch(tpsInfo.constellation){
            //wenming 2012-3-19: HAL will do remap by API TunerMod()
            case SONY_DVBT_CONSTELLATION_QPSK:
                _pSignal->QAMSize = 4; break;
            case SONY_DVBT_CONSTELLATION_16QAM:
                _pSignal->QAMSize = 16; break;
            case SONY_DVBT_CONSTELLATION_64QAM:
                _pSignal->QAMSize = 64; break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
            }

            switch(tpsInfo.hierarchy){
            case SONY_DVBT_HIERARCHY_NON:
            default:
                _pSignal->sHierInfo.eAlpha = ALPHA_0; break;
            case SONY_DVBT_HIERARCHY_1:
                _pSignal->sHierInfo.eAlpha = ALPHA_1; break;
            case SONY_DVBT_HIERARCHY_2:
                _pSignal->sHierInfo.eAlpha = ALPHA_2; break;
            case SONY_DVBT_HIERARCHY_4:
                _pSignal->sHierInfo.eAlpha = ALPHA_4; break;
            }

            if (tpsInfo.hierarchy == SONY_DVBT_HIERARCHY_NON) {
                _pSignal->e_hierarchy_priority = HIR_PRIORITY_NONE;
            } else {
                uint8_t data = 0;
                /* Set bank 10h */
                pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10);
                pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x67, &data, 1);
                _pSignal->e_hierarchy_priority = ((data & 0x01) == 0x01) ? HIR_PRIORITY_LOW : HIR_PRIORITY_HIGH;
                mcDBG_MSG_L2(("Driver get e_hierarchy_priority is %d\n", _pSignal->e_hierarchy_priority));
            }

#ifdef CC_PROJECT_FOR_SONY
            if(_pSignal->e_hierarchy_priority==HIR_PRIORITY_LOW)
            {
                mcDBG_MSG_L2(("RateLP : "));
                switch(tpsInfo.rateLP){
                case SONY_DVBT_CODERATE_1_2:
                   _pSignal->u1FecRate =CODE_RATE_1_2;
                   mcDBG_MSG_L2(("1/2\n")); break;
                case SONY_DVBT_CODERATE_2_3:
                    _pSignal->u1FecRate =CODE_RATE_2_3;
                   mcDBG_MSG_L2(("2/3\n")); break;
                case SONY_DVBT_CODERATE_3_4:
                    _pSignal->u1FecRate =CODE_RATE_3_4;
                   mcDBG_MSG_L2(("3/4\n")); break;
                case SONY_DVBT_CODERATE_5_6:
                   _pSignal->u1FecRate =CODE_RATE_5_6;
                   mcDBG_MSG_L2(("5/6\n")); break;
                case SONY_DVBT_CODERATE_7_8:
                   _pSignal->u1FecRate =CODE_RATE_7_8;
                   mcDBG_MSG_L2(("7/8\n")); break;
                default:
                   _pSignal->u1FecRate =CODE_RATE_UNKOWN;
                   mcDBG_MSG_L2(("Unknown\n"));
                break;
                }
            }
            else
            {
                mcDBG_MSG_L2(("RateHP : "));
                switch(tpsInfo.rateHP){
                case SONY_DVBT_CODERATE_1_2:
                   _pSignal->u1FecRate =CODE_RATE_1_2;
                   mcDBG_MSG_L2(("1/2\n")); break;
                case SONY_DVBT_CODERATE_2_3:
                    _pSignal->u1FecRate =CODE_RATE_2_3;
                   mcDBG_MSG_L2(("2/3\n")); break;
                case SONY_DVBT_CODERATE_3_4:
                    _pSignal->u1FecRate =CODE_RATE_3_4;
                   mcDBG_MSG_L2(("3/4\n")); break;
                case SONY_DVBT_CODERATE_5_6:
                   _pSignal->u1FecRate =CODE_RATE_5_6;
                   mcDBG_MSG_L2(("5/6\n")); break;
                case SONY_DVBT_CODERATE_7_8:
                   _pSignal->u1FecRate =CODE_RATE_7_8;
                   mcDBG_MSG_L2(("7/8\n")); break;
                default:
                   _pSignal->u1FecRate =CODE_RATE_UNKOWN;
                   mcDBG_MSG_L2(("Unknown\n"));
                break;
                }
            }

            mcDBG_MSG_L2(("Guard : "));
            switch(tpsInfo.guard){
            case SONY_DVBT_GUARD_1_32:
                _pSignal->u1GI=GUARD_INT_T_1_32;
                mcDBG_MSG_L2(("1/32\n")); break;
            case SONY_DVBT_GUARD_1_16:
                _pSignal->u1GI=GUARD_INT_T_1_16;
                mcDBG_MSG_L2(("1/16\n")); break;
            case SONY_DVBT_GUARD_1_8:
                _pSignal->u1GI=GUARD_INT_T_1_8;
                mcDBG_MSG_L2(("1/8\n")); break;
            case SONY_DVBT_GUARD_1_4:
                _pSignal->u1GI=GUARD_INT_T_1_4;
                mcDBG_MSG_L2(("1/4\n")); break;
            default:
                _pSignal->u1GI=GUARD_INT_T_UNKNOWN;
                mcDBG_MSG_L2(("Unknown(%d)\n", (int)tpsInfo.guard));
                break;
            }

            mcDBG_MSG_L1(("Mode : "));
            switch(tpsInfo.mode){
            case SONY_DVBT_MODE_2K:
                _pSignal->u1DVBT_T2_FFT_mode=CFFT_MODE_T_2K;
                mcDBG_MSG_L1(("2k mode\n")); break;
            case SONY_DVBT_MODE_8K:
                _pSignal->u1DVBT_T2_FFT_mode=CFFT_MODE_T_8K;
                mcDBG_MSG_L1(("8k mode\n")); break;
            default:
                _pSignal->u1DVBT_T2_FFT_mode=CFFT_MODE_T_UNKOWN;
                mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsInfo.mode));
                break;
            }
#endif // CC_PROJECT_FOR_SONY

        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
            _pSignal->sHierInfo.eAlpha = ALPHA_0;
            _pSignal->e_hierarchy_priority = HIR_PRIORITY_UNKNOWN;
        }
    }else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2){
        sony_dvbt2_plp_t plpInfo;
        sony_dvbt2_l1pre_t l1Pre;
#ifdef CC_PROJECT_FOR_SONY
        sony_dvbt2_ofdm_t ofdmInfo;
#endif
        _pSignal->u1DVBT_T2_indicator = TUNER_DVB_T2;

        sony_result = sony_demod_dvbt2_monitor_ActivePLP(pDemod, SONY_DVBT2_PLP_DATA, &plpInfo);
        if(sony_result == SONY_RESULT_OK){
            switch(plpInfo.constell){
            //wenming 2012-3-19: HAL will do remap by API TunerMod()
            case SONY_DVBT2_QPSK:
                _pSignal->QAMSize = 4; break;
            case SONY_DVBT2_QAM16:
                _pSignal->QAMSize = 16; break;
            case SONY_DVBT2_QAM64:
                _pSignal->QAMSize = 64; break;
            case SONY_DVBT2_QAM256:
                _pSignal->QAMSize = 256; break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
            }

#ifdef CC_PROJECT_FOR_SONY
            mcDBG_MSG_L2(("Code Rate : "));
            switch(plpInfo.plpCr){
            case SONY_DVBT2_R1_2:
               _pSignal->u1FecRate =CODE_RATE_1_2;
               mcDBG_MSG_L2(("1/2\n")); break;
            case SONY_DVBT2_R3_5:
               _pSignal->u1FecRate =CODE_RATE_3_5;
               mcDBG_MSG_L2(("3/5\n")); break;
            case SONY_DVBT2_R2_3:
                _pSignal->u1FecRate =CODE_RATE_2_3;
               mcDBG_MSG_L2(("2/3\n")); break;
            case SONY_DVBT2_R3_4:
               _pSignal->u1FecRate =CODE_RATE_3_4;
               mcDBG_MSG_L2(("3/4\n")); break;
            case SONY_DVBT2_R4_5:
               _pSignal->u1FecRate =CODE_RATE_4_5;
               mcDBG_MSG_L2(("4/5\n")); break;
            case SONY_DVBT2_R5_6:
               _pSignal->u1FecRate =CODE_RATE_5_6;
               mcDBG_MSG_L2(("5/6\n")); break;
            default:
               _pSignal->u1FecRate =CODE_RATE_1_2;
               mcDBG_MSG_L2(("Unknown\n")); break;
            }
#endif // CC_PROJECT_FOR_SONY

            _pSignal->ui1_plp_id = plpInfo.id;
        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
            _pSignal->ui1_plp_id = 0;
        }
        //wenming 12-3-23: Add to get T2 system id
        sony_result = sony_demod_dvbt2_monitor_L1Pre (pDemod, &l1Pre);
        if(sony_result == SONY_RESULT_OK)
        {
            _pSignal->ui2_t2_sys_id = l1Pre.systemId;
        }

#ifdef CC_PROJECT_FOR_SONY
        sony_result = sony_demod_dvbt2_monitor_Ofdm(pDemod, &ofdmInfo);
        if(sony_result == SONY_RESULT_OK)
        {
            mcDBG_MSG_L2(("PP : "));
            switch(ofdmInfo.pp){
            case SONY_DVBT2_PP1:
               _pSignal->u1Piliot_Pattern=PP1; break;
            case SONY_DVBT2_PP2:
               _pSignal->u1Piliot_Pattern=PP2; break;
            case SONY_DVBT2_PP3:
               _pSignal->u1Piliot_Pattern=PP3; break;
            case SONY_DVBT2_PP4:
               _pSignal->u1Piliot_Pattern=PP4; break;
            case SONY_DVBT2_PP5:
               _pSignal->u1Piliot_Pattern=PP5; break;
            case SONY_DVBT2_PP6:
               _pSignal->u1Piliot_Pattern=PP6; break;
            case SONY_DVBT2_PP7:
                _pSignal->u1Piliot_Pattern=PP7; break;
            case SONY_DVBT2_PP8:
               _pSignal->u1Piliot_Pattern=PP8; break;
            default:
               _pSignal->u1Piliot_Pattern=PPX_UNKNOWN; break;
            }

            mcDBG_MSG_L1(("DVB-T2 OFDM Information ----\n"));
            if (ofdmInfo.bwExt==0)    //BW extend==Normal
            {
                mcDBG_MSG_L2(("Mode : "));
                switch(ofdmInfo.mode){
                case SONY_DVBT2_M2K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_2K_N;
                    mcDBG_MSG_L2(("2K-N\n")); break;
                case SONY_DVBT2_M8K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_8K_N;
                    mcDBG_MSG_L2(("8K-N\n")); break;
                case SONY_DVBT2_M4K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_4K_N;
                    mcDBG_MSG_L2(("4K-N\n")); break;
                case SONY_DVBT2_M1K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_1K_N;
                    mcDBG_MSG_L2(("1K-N\n")); break;
                case SONY_DVBT2_M16K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_16K_N;
                    mcDBG_MSG_L2(("16K-N\n")); break;
                case SONY_DVBT2_M32K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_32K_N;
                    mcDBG_MSG_L2(("32K-N\n")); break;
                default:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_XK_UNKNOWN;
                    mcDBG_MSG_L2(("Unknown\n")); break;
                }
            }
            else if(ofdmInfo.bwExt==1) //BW extend==Extended
            {
                switch(ofdmInfo.mode){
                case SONY_DVBT2_M2K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_2K_E;
                    mcDBG_MSG_L2(("2K-E\n")); break;
                case SONY_DVBT2_M8K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_8K_E;
                    mcDBG_MSG_L2(("8K-E\n")); break;
                case SONY_DVBT2_M4K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_4K_E;
                    mcDBG_MSG_L2(("4K-E\n")); break;
                case SONY_DVBT2_M1K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_1K_E;
                    mcDBG_MSG_L2(("1K-E\n")); break;
                case SONY_DVBT2_M16K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_16K_E;
                    mcDBG_MSG_L2(("16K-E\n")); break;
                case SONY_DVBT2_M32K:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_32K_E;
                    mcDBG_MSG_L2(("32K-E\n")); break;
                default:
                    _pSignal->u1DVBT_T2_FFT_mode=CFFT_XK_UNKNOWN;
                    mcDBG_MSG_L2(("Unknown\n")); break;
                }
            }//end for carrier type recognize 20111226

            mcDBG_MSG_L2(("GI : "));

            switch(ofdmInfo.gi){
            case SONY_DVBT2_G1_32:
                _pSignal->u1GI = GUARD_INT_T2_1_32;
                mcDBG_MSG_L2(("1/32\n")); break;
            case SONY_DVBT2_G1_16:
                 _pSignal->u1GI = GUARD_INT_T2_1_16;
                mcDBG_MSG_L2(("1/16\n")); break;
            case SONY_DVBT2_G1_8:
                 _pSignal->u1GI =GUARD_INT_T2_1_8;
                mcDBG_MSG_L2(("1/8\n")); break;
            case SONY_DVBT2_G1_4:
                 _pSignal->u1GI = GUARD_INT_T2_1_4;
                mcDBG_MSG_L2(("1/4\n")); break;
            case SONY_DVBT2_G1_128:
                 _pSignal->u1GI = GUARD_INT_T2_1_128;
                mcDBG_MSG_L2(("1/128\n")); break;
            case SONY_DVBT2_G19_128:
                 _pSignal->u1GI = GUARD_INT_T2_19_128;
                mcDBG_MSG_L2(("19/128\n")); break;
            case SONY_DVBT2_G19_256:
                 _pSignal->u1GI = GUARD_INT_T2_19_256;
                mcDBG_MSG_L2(("19/256\n")); break;
            default:
                _pSignal->u1GI = GUARD_INT_T2_UNKNOWN;
                mcDBG_MSG_L2(("Unknown\n")); break;
            }
        }
#endif // CC_PROJECT_FOR_SONY

    }
    else if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
        sony_dvbc_constellation_t constellation = SONY_DVBC_CONSTELLATION_16QAM;
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
        _pSignal->u1DVBC_C2_indicator = TUNER_DVB_C;
        #endif
        sony_result = sony_demod_dvbc_monitor_QAM(pDemod, &constellation);
        if(sony_result == SONY_RESULT_OK){
            switch(constellation){
            //wenming 2012-3-19: HAL will do remap by API TunerMod()
            case SONY_DVBC_CONSTELLATION_16QAM:
                _pSignal->QAMSize = 16; break;
            case SONY_DVBC_CONSTELLATION_32QAM:
                _pSignal->QAMSize = 32; break;
            case SONY_DVBC_CONSTELLATION_64QAM:
                _pSignal->QAMSize = 64; break;
            case SONY_DVBC_CONSTELLATION_128QAM:
                _pSignal->QAMSize = 128; break;
            case SONY_DVBC_CONSTELLATION_256QAM:
                _pSignal->QAMSize = 256; break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
            }
        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        }
        mcDBG_MSG_L2(("Driver get QAMSize is %d\n", _pSignal->QAMSize));

        sony_result = sony_demod_dvbc_monitor_SymbolRate(pDemod, &symRate);
        if(sony_result == SONY_RESULT_OK){
            //wenming 2012-3-20: symRate will *1000 in HAL...
            _pSignal->SymbolRate = symRate;
        }else{
            _pSignal->SymbolRate = 0;
        }
        mcDBG_MSG_L2(("Driver get SymbolRate is %d\n", _pSignal->SymbolRate));
    }
    else if(pDemod->system == SONY_DTV_SYSTEM_DVBC2){
        #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
        sony_dvbc2_constellation_t constellation = SONY_DVBC2_CONSTELLATION_UNKNOWN;

        _pSignal->u1DVBC_C2_indicator = TUNER_DVB_C2;

        sony_result = sony_demod_dvbc2_monitor_QAM(pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &constellation);
        if(sony_result == SONY_RESULT_OK){
            switch(constellation){
            case SONY_DVBC2_CONSTELLATION_16QAM:
                _pSignal->QAMSize = 16; break;
            case SONY_DVBC2_CONSTELLATION_64QAM:
                _pSignal->QAMSize = 64; break;
            case SONY_DVBC2_CONSTELLATION_256QAM:
                _pSignal->QAMSize = 256; break;
            case SONY_DVBC2_CONSTELLATION_1024QAM:
                _pSignal->QAMSize = 1024; break;
            case SONY_DVBC2_CONSTELLATION_4096QAM:
                _pSignal->QAMSize = 4096; break;
            default:
                _pSignal->QAMSize = (UINT32)MOD_UNKNOWN; break;
            }
        }else{
            _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
        }
        mcDBG_MSG_L2(("Driver get QAMSize is %d\n", _pSignal->QAMSize));

        _pSignal->ui1_ds_id = g_sony_demod_ctx.c2DsId;
        _pSignal->ui1_plp_id = g_sony_demod_ctx.c2PlpId;
        #endif
    }
    else{
        _pSignal->QAMSize = (UINT32)MOD_UNKNOWN;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in terms of percentage.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 DVBT2_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT8 ssi = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    // Get Nordig SSI
    switch(pInteg->pDemod->system){
    case SONY_DTV_SYSTEM_DVBT:
        sony_result = sony_integ_dvbt_monitor_SSI(pInteg, &ssi);
        break;
    case SONY_DTV_SYSTEM_DVBT2:
        sony_result = sony_integ_dvbt2_monitor_SSI(pInteg, &ssi);
        break;
    case SONY_DTV_SYSTEM_DVBC:
        sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi);
        break;
    case SONY_DTV_SYSTEM_DVBC2:
        sony_result = sony_integ_dvbc2_monitor_SSI(pInteg, &ssi);
        break;
    default:
        sony_result = SONY_RESULT_ERROR_NOSUPPORT;
        break;
    }

    if(sony_result != SONY_RESULT_OK){
        ssi = 0;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    return ssi;
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in multiples of (0.1dBm).
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0 ~ -900           signal level
 */
//-------------------------------------------------------------------------
static INT16 DVBT2_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT16 i2Ret = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;
    int32_t rfLevel = 0;

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    sony_result = sony_integ_monitor_RFLevel_MTK(pInteg, &rfLevel);
    if(sony_result == SONY_RESULT_OK){
        if(rfLevel >= 0){
            i2Ret = (INT16)((rfLevel + 50) / 100); // In 0.1dBm unit.
        }else{
            i2Ret = (INT16)((rfLevel - 50) / 100); // In 0.1dBm unit.
        }
    }else{
        i2Ret = 0;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    return i2Ret;
}

static UINT32 sony_GetPreBER(void)
{
    uint32_t ber = 100000; // unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    /* unit is 1e5 */
    switch(pDemod->system)
    {
    case SONY_DTV_SYSTEM_DVBT:
        sony_result = sony_demod_dvbt_monitor_PreViterbiBER(pDemod, &ber); /* 1e7 */
        mcDBG_MSG_L2(("T Pre Viterbi BER\n"));
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
        }else{
            ber = 100000;
        }
        break;
    case SONY_DTV_SYSTEM_DVBT2:
        sony_result = sony_demod_dvbt2_monitor_PreLDPCBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-G T2 Pre LDPC BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG T2 Pre LDPC BER\n"));
        }
        break;
    case SONY_DTV_SYSTEM_DVBC:
        sony_result = sony_demod_dvbc_monitor_PreRSBER(pDemod, &ber); /* 1e7 */
        mcDBG_MSG_L2(("C BER\n"));
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
        }else{
            ber = 100000;
        }
        break;
    case SONY_DTV_SYSTEM_DVBC2:
        sony_result = sony_demod_dvbc2_monitor_PreLDPCBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-G C2 Pre LDPC BER\n"));
        }else{
            ber = 100000;
            mcDBG_MSG_L2(("-NG C2 Pre LDPC BER\n"));
        }
        break;
    default:
        ber = 100000;
        mcDBG_MSG_L2(("NG def BER\n"));
        break;
    }

    return ber;
}

static UINT32 sony_GetPER(void)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    uint32_t per = 0;
    
    switch(pDemod->system){
    case SONY_DTV_SYSTEM_DVBT:
        sony_result = sony_demod_dvbt_monitor_PER(pDemod, &per); break;
    case SONY_DTV_SYSTEM_DVBT2:
        sony_result = sony_demod_dvbt2_monitor_PER(pDemod, &per); break;
    case SONY_DTV_SYSTEM_DVBC:
        sony_result = sony_demod_dvbc_monitor_PER(pDemod, &per); break;
    case SONY_DTV_SYSTEM_DVBC2:
        sony_result = sony_demod_dvbc2_monitor_PER(pDemod, &per); break;
    default:
        sony_result = SONY_RESULT_ERROR_SW_STATE; break;
    }
    if(sony_result == SONY_RESULT_OK){
        mcDBG_MSG_L1(("PER (1e6) : %d\n", per));
    }else{
        mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
    }

    return per;
}

#ifndef ERROR_BER_VALUE
#define ERROR_BER_VALUE 100000
#endif

static UINT32 sony_GetBER(void)
{
    uint32_t ber = 100000; // unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    /* unit is 1e5 */
    switch(pDemod->system)
    {
    case SONY_DTV_SYSTEM_DVBT:
        sony_result = sony_demod_dvbt_monitor_PreRSBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-OK T BER\n"));
        }else{
            ber = ERROR_BER_VALUE;
            mcDBG_MSG_L2(("-NG T BER\n"));
        }
        break;
    case SONY_DTV_SYSTEM_DVBT2:
        sony_result = sony_demod_dvbt2_monitor_PreBCHBER(pDemod, &ber); /* 1e9 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 5000) / 10000;
            mcDBG_MSG_L2(("-OK T2 BER\n"));
        }else{
            ber = ERROR_BER_VALUE;
            mcDBG_MSG_L2(("-NG T2 BER\n"));
        }
        break;
    case SONY_DTV_SYSTEM_DVBC:
        sony_result = sony_demod_dvbc_monitor_PreRSBER(pDemod, &ber); /* 1e7 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 50) / 100;
            mcDBG_MSG_L2(("-OK C BER\n"));
        }else{
            ber = ERROR_BER_VALUE;
            mcDBG_MSG_L2(("-NG C BER\n"));
        }
        break;
    case SONY_DTV_SYSTEM_DVBC2:
        sony_result = sony_demod_dvbc2_monitor_PreBCHBER(pDemod, &ber); /* 1e9 */
        if(sony_result == SONY_RESULT_OK){
            ber = (ber + 5000) / 10000;
            mcDBG_MSG_L2(("-OK C2 BER\n"));
        }else{
            ber = ERROR_BER_VALUE;
            mcDBG_MSG_L2(("-NG C2 BER\n"));
        }
        break;
    default:
        ber = ERROR_BER_VALUE;
        mcDBG_MSG_L2(("-NG def BER\n"));
        break;
    }

    return ber;
}

#if 0 //Defined but not used warning to error...
static UINT32 DVBT2_TunerGetSignalBER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 ber = 0;

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    ber = sony_GetBER();

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    return ber;
}
#endif

#ifdef  INTERNAL_3RD_DEMOD
static UINT16 DVBT2_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
#else
UINT32 DVBT2_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
#endif
{
    uint32_t u4_ret = 100000; // unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    //sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    u4_ret = sony_GetPER();
    if(sony_result != SONY_RESULT_OK){
        u4_ret = 100000; // Error Rate = 100%
    }else{
        // NOTE: sony_demod_dvbx_monitor_PER returns 1e6 unit value.
        u4_ret = (u4_ret + 5) / 10;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
#ifdef  INTERNAL_3RD_DEMOD

    return (UINT16)u4_ret;//modify return value
#else
    return (UINT32)u4_ret;//modify return value

#endif
}

static UINT32 sony_GetUEC(VOID)
{
    uint32_t pen = 0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    if(pDemod->system == SONY_DTV_SYSTEM_DVBT){
        sony_result = sony_demod_dvbt_monitor_PacketErrorNumber(pDemod, &pen);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("UEC : %d\n", pen));
        }else{
            mcDBG_MSG_L2(("Error in monitorT_UEC\n"));
        }
    }else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2){
        sony_result = sony_demod_dvbt2_monitor_PacketErrorNumber(pDemod, &pen);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("UEC : %d\n", pen));
        }else{
            mcDBG_MSG_L2(("Error in monitorT2_UEC\n"));
        }
    }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
        sony_result = sony_demod_dvbc_monitor_PacketErrorNumber(pDemod, &pen);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("UEC : %d\n", pen));
        }else{
            mcDBG_MSG_L2(("Error in monitorC_UEC\n"));
        }
    }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC2){
        sony_result = sony_demod_dvbc2_monitor_PacketErrorNumber(pDemod, &pen);
        if(sony_result == SONY_RESULT_OK){
            mcDBG_MSG_L2(("UEC : %d\n", pen));
        }else{
            mcDBG_MSG_L2(("Error in monitorC2_UEC\n"));
        }
    }else{
        mcDBG_MSG_L1(("Invalid System\n"));
    }
    return pen;
}


static UINT16 DVBT2_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret = 0;
    int32_t snr = 0;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_result_t sony_result = SONY_RESULT_OK;

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    // NOTE: sony_demod_dvbx_monitor_SNR returns dB * 1000.
    switch(pDemod->system){
    case SONY_DTV_SYSTEM_DVBT:
        sony_result = sony_demod_dvbt_monitor_SNR(pDemod, &snr);
        break;
    case SONY_DTV_SYSTEM_DVBT2:
        sony_result = sony_demod_dvbt2_monitor_SNR(pDemod, &snr);
        break;
    case SONY_DTV_SYSTEM_DVBC:
        sony_result = sony_demod_dvbc_monitor_SNR(pDemod, &snr);
        break;
    case SONY_DTV_SYSTEM_DVBC2:
        sony_result = sony_demod_dvbc2_monitor_SNR(pDemod, &snr);
        break;
    default:
        snr = 0;
        break;
    }

    if(sony_result != SONY_RESULT_OK){
        snr = 0;
    }

    u2_ret = (UINT16)((snr + 500)/1000); // rounding

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    return u2_ret;
}

static void DVBT2_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add your implementation here
    mcDBG_MSG_L1(("No implementation in DVBT2_TunerSetMpgFmt()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

static char *DVBT2_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    static char * version = SONY_CXD2854_DVBTT2CC2_DRIVER_VERSION_STR;

    return version;
}

static INT32 DVBT2_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    INT32 i4Ret = 0;
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    //add your implementation here

    mcDBG_MSG_L1(("No implementation in DVBT2_TunerNimTest()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    return i4Ret;
}
static void DVBT2_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add your implementation here
    mcDBG_MSG_L1(("No implementation in DVBT2_TunerSetRegSetting()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

static void DVBT2_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add your implementation here
    mcDBG_MSG_L1(("No implementation in DVBT2_TunerShowRegSetting()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

void DVBT2_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add your implementation here
    mcDBG_MSG_L1(("No implementation in DVBT2_TunerTestI2C()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

static void DVBT2_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    //UCHAR   ucCmdSize = 0;

    mcDBG_MSG_L4(("DVBT2_TunerCommand pPdCtx->hHalLock\n"));
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    mcDBG_MSG_L4(("escape DVBT2_TunerCommand pPdCtx->hHalLock\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    mcDBG_MSG_L4(("DVBT2_TunerCommand unlock pPdCtx->hHalLock\n"));
}

static void DVBT2_TunerDVBT2Command(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    //TODO you can add command for debug use here

    UCHAR ucCmdId = 0;
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t c                                 MTK Show checklist info\n"));
        mcDBG_MSG_L1(("\t g [freqKHz] [plpid] [bw](1,6,7,8) MTK CLI lock T2 signal\n"));
        mcDBG_MSG_L1(("\t vt [bw](6,8) [freqKHz] [dsid] [plpid] [isDSDS] MTK CLI lock C2 signal (tune)\n"));
        mcDBG_MSG_L1(("\t vs [bw](6,8) [freqKHz]            MTK CLI lock C2 signal (scan)\n"));
        mcDBG_MSG_L1(("\t s                                 Show signal, TS, DVB system status\n"));
        mcDBG_MSG_L1(("\t d [t|x] [Bank]                    Demod register page dump\n"));
        mcDBG_MSG_L1(("\t r [t|x] [Bank] [RegAddr] [Num]    Read demod register\n"));
        mcDBG_MSG_L1(("\t w [t|x] [Bank] [RegAddr] [Value]  Write demod register\n"));
        mcDBG_MSG_L1(("\t ad                                Tuner register page dump\n"));
        mcDBG_MSG_L1(("\t ar [RegAddr] [Num]                Read tuner register\n"));
        mcDBG_MSG_L1(("\t aw [RegAddr] [Value]              Write tuner register\n"));
        mcDBG_MSG_L1(("\t f [Freq]                          Set Freq (kHz)\n"));
        mcDBG_MSG_L1(("\t l                                 Make sony demod sleep\n"));
        mcDBG_MSG_L1(("\t p [d|a] [freqKHz]                 Change trap threshold\n"));
    }
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);

    switch (ucCmdId)
    {
#ifndef SONY_DUMMY_DEBUG
    case 'c':{
        /*********NO1 .check Demod rest config*********/
        UINT32 u4RstPinNum=0xFFFF;
        UINT32 u4DemodBusId=0xFFFF;
         if ( 0== DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum))
            {
            mcDBG_MSG_L2(("DRVCUST DEMOD rest GPIO=%d\n",u4RstPinNum));
            }
         else
            {mcDBG_MSG_L2(("!!!No config DRVCUST DEMOD rest GPIO=%d\n",u4RstPinNum));}
        /*********NO2 .check Demod&Tuner I2C  busID config*********/
        if (0==DRVCUST_OptQuery(eDemodI2cBusID,&u4DemodBusId))
            {
            mcDBG_MSG_L2(("Demod  I2C  busID =%d\n",u4DemodBusId));
            }
        else
            {mcDBG_MSG_L2(("!!!No config Demod I2C  busID =%d\n",u4DemodBusId));}
        /*********NO3 .check Demod crystal Freq config*********/
            #ifdef SONY_DEMOD_CRYSTAL_41M
            mcDBG_MSG_L2(("SONY_DEMOD_CRYSTAL_41M\n"));
            //#warning  "SONY_DEMOD_CRYSTAL_41M"
            #else
            mcDBG_MSG_L2(("SONY_DEMOD_CRYSTAL_20.5M\n"));
            //#warning  "SONY_DEMOD_CRYSTAL_20.5M"
            #endif
        /*********NO4 .check Demod TS output mode config*********/
            #ifdef SONY_DEMOD_TSOUTPUT_SERIAL
            //  2 for TS output mode
            mcDBG_MSG_L2(("SONY_DEMOD_TSOUTPUT:  SERIAL\n"));
            #else
            mcDBG_MSG_L2(("SONY_DEMOD_TSOUTPUT:  PARAlLEL\n"));
            #endif
        /*********NO5 .check system  config*********/
            #ifdef __KERNEL__
            mcDBG_MSG_L2(("System config:  LINUX\n"));
            #else
            mcDBG_MSG_L2(("System config:  ncls\n"));
            #endif
            /*********NO6 .check internal tuner_driver  config*********/
            #ifdef INTERNAL_3RD_DEMOD
            // 3 for internal tuner_src use
            mcDBG_MSG_L2(("INTERNAL_3RD_DEMOD\n"));
            #else
            mcDBG_MSG_L2(("VDRDEMOD_3RD_DEMOD\n"));
            #endif
            /*********NO7 .check i2c Demod Gateway config*********/
            #ifdef SONY_DEMOD
            // 4 for sony_demod bypass i2c Gateway which use 0xd8 0x09 i2c data to tuner
            mcDBG_MSG_L2(("SONY_DEMOD I2C Gateway\n"));
            #else
            mcDBG_MSG_L2(("I2C direct ACESS\n"));
            #endif
        }
        break;
#endif // SONY_DUMMY_DEBUG
    case 'g':
        {
            sony_dvbt2_tune_param_t tune_param; // tune param struct for DVB-T2
            BOOL fgLock = 0;
            uint32_t freqKHz = 666000;
            uint8_t plpID =0;
            uint8_t bw = 0;

            if (i4Argc < 4){
                mcDBG_MSG_L1(("Error: Argument is invalid. (g [freqKHz] [plpid] [bw](1,6,7,8))\n"));
                return;
            }

            g_sony_demod_ctx.breakFct = NULL;
            g_sony_demod_ctx.breakFctArg = NULL;
            sony_atomic_set (&(pInteg->cancel), 0);

            // For MTK, scan mode flag should be 0 except for DVB-C scanning. Set 0 here for safe.
            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_ANY, 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                return;
            }

            freqKHz = (uint32_t)sony_strtoul(aszArgv[1]);
            plpID = (uint8_t)sony_strtoul(aszArgv[2]);
            bw = (uint8_t)sony_strtoul(aszArgv[3]);

            tune_param.centerFreqKHz = freqKHz;
            switch(bw){
            case 1:
                tune_param.bandwidth = SONY_DTV_BW_1_7_MHZ; break;
            case 6:
                tune_param.bandwidth = SONY_DTV_BW_6_MHZ; break;
            case 7:
                tune_param.bandwidth = SONY_DTV_BW_7_MHZ; break;
            case 8:
            default:
                tune_param.bandwidth = SONY_DTV_BW_8_MHZ; break;
            }
            tune_param.dataPlpId = plpID;
            tune_param.tuneInfo = SONY_DEMOD_DVBT2_TUNE_INFO_OK; //wenming 2012-7-4: fix building error

            mcDBG_MSG_L1(("Freq = %d\n", tune_param.centerFreqKHz));
            mcDBG_MSG_L1(("System = DVB-T2\n"));
            mcDBG_MSG_L1(("Bandwidth = %d\n", tune_param.bandwidth));
            mcDBG_MSG_L1(("PLP ID = %d\n", tune_param.dataPlpId));

            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock); // Enter Critical Section
            sony_result = sony_integ_dvbt2_Tune(pInteg, &tune_param);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock); // Leave Critical Section
            switch(sony_result){
            case SONY_RESULT_OK:
                mcDBG_MSG_L1(("DVB-T2 TS Locked.\n"));
                fgLock = 1;
                break;
            case SONY_RESULT_ERROR_UNLOCK:
                mcDBG_MSG_L1(("DVB-T2 TS Unlocked.\n"));
                fgLock = 0;
                break;
            case SONY_RESULT_ERROR_TIMEOUT:
                mcDBG_MSG_L1(("DVB-T2 Wait TS Lock but Timeout.\n"));
                fgLock = 0;
                break;
            default:
                mcDBG_MSG_L1(("Error in sony_integ_dvbt2_Tune (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                break;
            }

            if(fgLock == 1){
                if(tune_param.tuneInfo == SONY_DEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID){
                    mcDBG_MSG_L1(("Warning: PLP ID is invalid.\n"));
                    if(!g_sony_demod_ctx.autoPLP){
                        fgLock = 0;
                    }
                }
            }
        }
        break;
#ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
    case 'v':
        {
            // Try C2 Tune/Scan by CLI

            g_sony_demod_ctx.breakFct = NULL;
            g_sony_demod_ctx.breakFctArg = NULL;
            sony_atomic_set (&(pInteg->cancel), 0);

            // For MTK, scan mode flag should be 0 except for DVB-C scanning. Set 0 here for safe.
            sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_ANY, 0x00);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
                return;
            }

            g_sony_demod_ctx.c2DsId = 0;
            g_sony_demod_ctx.c2PlpId = 0;

            if(aszArgv[0][1] == 's'){
                uint32_t frequencyKHz = 0;
                sony_dtv_bandwidth_t bandwidth = SONY_DTV_BW_8_MHZ;
                sony_dvbc2_tune_param_t * ptune_param  = (sony_dvbc2_tune_param_t *)x_mem_calloc(256,sizeof(sony_dvbc2_tune_param_t));
                uint16_t num_tune_param = 0;
                uint32_t next_frequency = 0;

                int i = 0;

                uint8_t bw = 0;

                x_memset(ptune_param, 0, sizeof(sony_dvbc2_tune_param_t)*256);

                if(i4Argc < 3){
                    mcDBG_MSG_L1(("Error: Argument is invalid. (vs [bw](6,8) [freqKHz])\n"));
                    return;
                }

                bw = (uint8_t)sony_strtoul(aszArgv[1]);
                frequencyKHz = (uint32_t)sony_strtoul(aszArgv[2]);

                switch(bw){
                case 6:
                    bandwidth = SONY_DTV_BW_6_MHZ; break;
                case 8:
                default:
                    bandwidth = SONY_DTV_BW_8_MHZ; break;
                }

                mcDBG_MSG_L1(("Freq = %d kHz\n", frequencyKHz));
                mcDBG_MSG_L1(("System = DVB-C2 (Scan)\n"));
                mcDBG_MSG_L1(("Bandwidth = %d\n", bandwidth));

                mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock); // Enter Critical Section

                sony_result = sony_integ_dvbc2_GetChannelTuneParameters(pInteg, frequencyKHz, bandwidth,
                    ptune_param, &num_tune_param, &next_frequency);

                mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock); // Leave Critical Section

                switch(sony_result){
                case SONY_RESULT_OK:
                    mcDBG_MSG_L1(("DVB-C2 Locked. (Signal Found)\n"));

                    mcDBG_MSG_L1(("Number of Tune Params = %d\n", num_tune_param));
                    mcDBG_MSG_L1(("Next Freq = %d kHz\n", next_frequency));

                    if(bandwidth == SONY_DTV_BW_6_MHZ){
                        g_sony_demod_ctx.c2DsPlpList.e_bw = BW_6_MHz;
                    }else{
                        g_sony_demod_ctx.c2DsPlpList.e_bw = BW_8_MHz;
                    }
                    g_sony_demod_ctx.c2DsPlpList.ui2_curr_num = (UINT16)num_tune_param;
                    g_sony_demod_ctx.c2NextFrequency = (UINT32)(next_frequency * 1000);

                    for(i = 0; i < (int)num_tune_param; i++){
                        mcDBG_MSG_L1(("[%d] Freq = %d kHz\n", i, ptune_param[i].c2TuningFrequencyKHz));
                        mcDBG_MSG_L1(("[%d] DS ID = %d\n", i, ptune_param[i].dataSliceId));
                        mcDBG_MSG_L1(("[%d] PLP ID = %d\n", i, ptune_param[i].dataPLPId));
                        mcDBG_MSG_L1(("[%d] IsDependentStaticDS = %d\n", i, ptune_param[i].isDependentStaticDS));

                        // Store the information to g_sony_demod_ctx
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui4_c2_tuning_freq = (UINT32)(ptune_param[i].c2TuningFrequencyKHz * 1000);
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui1_ds_id = (UINT8)ptune_param[i].dataSliceId;
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].ui1_plp_id = (UINT8)ptune_param[i].dataPLPId;
                        g_sony_demod_ctx.c2DsPlpList.t_plp_info[i].b_dependent_static_ds = (BOOL)ptune_param[i].isDependentStaticDS;
                    }
                    break;
                case SONY_RESULT_ERROR_UNLOCK:
                    mcDBG_MSG_L1(("DVB-C2 Unlocked. (Signal Not Found)\n"));
                    break;
                case SONY_RESULT_ERROR_TIMEOUT:
                    mcDBG_MSG_L1(("DVB-C2 Wait Lock but Timeout. (Signal Not Found)\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("Error in sony_integ_dvbc2_GetChannelTuneParameters (%s)\n", sony_FormatResult(sony_result)));
                    break;
                }
                x_mem_free(ptune_param);
            }else if(aszArgv[0][1] == 't'){
                // Tune
                sony_dvbc2_tune_param_t tune_param; // tune param struct for DVB-C2
                uint8_t bw = 0;

                if(i4Argc < 6){
                    mcDBG_MSG_L1(("Error: Argument is invalid. (vt [bw](6,8) [freqKHz] [dsid] [plpid] [isDSDS])\n"));
                    return;
                }

                bw = (uint8_t)sony_strtoul(aszArgv[1]);
                tune_param.c2TuningFrequencyKHz = (uint32_t)sony_strtoul(aszArgv[2]);
                tune_param.dataSliceId = (uint8_t)sony_strtoul(aszArgv[3]);
                tune_param.dataPLPId = (uint8_t)sony_strtoul(aszArgv[4]);
                tune_param.isDependentStaticDS = (uint8_t)sony_strtoul(aszArgv[5]);
                tune_param.rfTuningFrequencyKHz = 0;

                switch(bw){
                case 6:
                    tune_param.bandwidth = SONY_DTV_BW_6_MHZ; break;
                case 8:
                default:
                    tune_param.bandwidth = SONY_DTV_BW_8_MHZ; break;
                }

                mcDBG_MSG_L1(("Freq = %d kHz\n", tune_param.c2TuningFrequencyKHz));
                mcDBG_MSG_L1(("System = DVB-C2\n"));
                mcDBG_MSG_L1(("Bandwidth = %d\n", tune_param.bandwidth));
                mcDBG_MSG_L1(("DS ID = %d\n", tune_param.dataSliceId));
                mcDBG_MSG_L1(("PLP ID = %d\n", tune_param.dataPLPId));
                mcDBG_MSG_L1(("IsDependentStaticDS = %d\n", tune_param.isDependentStaticDS));

                // Store DS ID and PLP ID for DVBT2_TunerGetSignal
                g_sony_demod_ctx.c2DsId = (UINT8)tune_param.dataSliceId;
                g_sony_demod_ctx.c2PlpId = (UINT8)tune_param.dataPLPId;

                mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock); // Enter Critical Section

                sony_result = sony_integ_dvbc2_Tune(pInteg, &tune_param);

                mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock); // Leave Critical Section

                switch(sony_result){
                case SONY_RESULT_OK:
                    mcDBG_MSG_L1(("DVB-C2 TS Locked.\n"));
                    break;
                case SONY_RESULT_ERROR_UNLOCK:
                    mcDBG_MSG_L1(("DVB-C2 TS Unlocked.\n"));
                    break;
                case SONY_RESULT_ERROR_TIMEOUT:
                    mcDBG_MSG_L1(("DVB-C2 Wait TS Lock but Timeout.\n"));
                    break;
                default:
                    mcDBG_MSG_L1(("Error in sony_integ_dvbc2_Tune (%s)\n", sony_FormatResult(sony_result)));
                    break;
                }
            }else{
                // Unknown command.
            }
        }
        break;
#endif
    case 's':
        mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

        /* Demod Lock Status */
        if(pDemod->system == SONY_DTV_SYSTEM_DVBT){
            UINT8 syncStat = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_demod_dvbt_monitor_SyncStat(pDemod, &syncStat, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Sync State : %d\n", syncStat));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det : %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt_monitor_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2){
            UINT8 syncStat = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_demod_dvbt2_monitor_SyncStat(pDemod, &syncStat, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Sync State : %d\n", syncStat));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det : %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
            UINT8 arLock = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_demod_dvbc_monitor_SyncStat(pDemod, &arLock, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("AR Lock    : %d\n", arLock));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det : %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc_monitor_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC2){
            UINT8 syncStat = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_demod_dvbc2_monitor_SyncStat(pDemod, &syncStat, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Sync State : %d\n", syncStat));
                mcDBG_MSG_L1(("TS Lock    : %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det : %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }else{
            mcDBG_MSG_L1(("Invalid System\n"));
        }

        /* IFAGC */
        {
            uint32_t ifAgc = 0;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_IFAGCOut(pDemod, &ifAgc); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("IFAGC (%%)  : %d\n", 58 - ifAgc * 100 / 4095));
                mcDBG_MSG_L1(("IFAGC value : %d\n", ifAgc));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_IFAGCOut (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Carrier Offset */
        {
            int32_t offset = 0;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_CarrierOffset(pDemod, &offset);
                offset = (offset + 500) / 1000; break; // Hz -> kHz
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_CarrierOffset(pDemod, &offset);
                offset = (offset + 500) / 1000; break; // Hz -> kHz
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_CarrierOffset(pDemod, &offset); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_CarrierOffset(pDemod, &offset);
                offset = (offset + 500) / 1000; break; // Hz -> kHz
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Carrier Offset (kHz) : %d\n", offset));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_CarrierOffset (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* BER */
        if(pDemod->system == SONY_DTV_SYSTEM_DVBT){
            uint32_t ber = 0;
            sony_result = sony_demod_dvbt_monitor_PreViterbiBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreViterbi BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt_monitor_PreViterbiBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_dvbt_monitor_PreRSBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreRS BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2){
            uint32_t ber = 0;
            sony_result = sony_demod_dvbt2_monitor_PreLDPCBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreLDPC BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_PreLDPCBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_dvbt2_monitor_PreBCHBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreBCH BER (1e9) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_PreBCHBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
            uint32_t ber = 0;
            sony_result = sony_demod_dvbc_monitor_PreRSBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreRS BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC2){
            uint32_t ber = 0;
            sony_result = sony_demod_dvbc2_monitor_PreLDPCBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreLDPC BER (1e7) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_PreLDPCBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_dvbc2_monitor_PreBCHBER(pDemod, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PreBCH BER (1e9) : %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_PreBCHBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }else{
            mcDBG_MSG_L1(("Invalid System\n"));
        }

        /* PER */
        {
            uint32_t per = 0;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_PER(pDemod, &per); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_PER(pDemod, &per); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_PER(pDemod, &per); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_PER(pDemod, &per); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (1e6) : %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* Packet Error Number */
        {
            uint32_t packetErrorNum = 0;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_PacketErrorNumber(pDemod, &packetErrorNum); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_PacketErrorNumber(pDemod, &packetErrorNum); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_PacketErrorNumber(pDemod, &packetErrorNum); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_PacketErrorNumber(pDemod, &packetErrorNum); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (1sec) : %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* SNR */
        {
            int32_t snr = 0;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_SNR(pDemod, &snr); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SNR (dB) : %d.%03d\n", snr/1000U, snr%1000U));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* SQI */
        {
            uint8_t sqi = 0;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_Quality(pDemod, &sqi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SQI (%%) : %d\n", sqi));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_Quality (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* SSI */
        {
            uint8_t ssi = 0;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_integ_dvbt_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_integ_dvbt2_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_integ_dvbc2_monitor_SSI(pInteg, &ssi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SSI (%%) : %d\n", ssi));
            }else{
                mcDBG_MSG_L1(("Error in sony_integ_dvbx_monitor_SSI (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* TS rate */
        if((pDemod->system == SONY_DTV_SYSTEM_DVBT2) || (pDemod->system == SONY_DTV_SYSTEM_DVBC2)){
            uint32_t rate = 0;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_TsRate(pDemod, &rate); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_TsRate(pDemod, &rate); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("TS rate (kbps) : %d\n", rate));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbx_monitor_TsRate (%s)\n", sony_FormatResult(sony_result)));
            }
        }

        /* System Dependent Information */
        if(pDemod->system == SONY_DTV_SYSTEM_DVBT){
            sony_dvbt_tpsinfo_t tpsinfo;
            sony_result = sony_demod_dvbt_monitor_TPSInfo(pDemod, &tpsinfo);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("DVB-T TPS Information ----\n"));
                mcDBG_MSG_L1(("Constellation : "));
                switch(tpsinfo.constellation){
                case SONY_DVBT_CONSTELLATION_QPSK:
                    mcDBG_MSG_L1(("QPSK\n")); break;
                case SONY_DVBT_CONSTELLATION_16QAM:
                    mcDBG_MSG_L1(("16QAM\n")); break;
                case SONY_DVBT_CONSTELLATION_64QAM:
                    mcDBG_MSG_L1(("64QAM\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.constellation));
                    break;
                }

                mcDBG_MSG_L1(("Hierarchy : "));
                switch(tpsinfo.hierarchy){
                case SONY_DVBT_HIERARCHY_NON:
                    mcDBG_MSG_L1(("Non hierarchical\n")); break;
                case SONY_DVBT_HIERARCHY_1:
                    mcDBG_MSG_L1(("a=1\n")); break;
                case SONY_DVBT_HIERARCHY_2:
                    mcDBG_MSG_L1(("a=2\n")); break;
                case SONY_DVBT_HIERARCHY_4:
                    mcDBG_MSG_L1(("a=4\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.hierarchy));
                    break;
                }

                mcDBG_MSG_L1(("RateHP : "));
                switch(tpsinfo.rateHP){
                case SONY_DVBT_CODERATE_1_2:
                    mcDBG_MSG_L1(("1/2\n")); break;
                case SONY_DVBT_CODERATE_2_3:
                    mcDBG_MSG_L1(("2/3\n")); break;
                case SONY_DVBT_CODERATE_3_4:
                    mcDBG_MSG_L1(("3/4\n")); break;
                case SONY_DVBT_CODERATE_5_6:
                    mcDBG_MSG_L1(("5/6\n")); break;
                case SONY_DVBT_CODERATE_7_8:
                    mcDBG_MSG_L1(("7/8\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.rateHP));
                    break;
                }

                mcDBG_MSG_L1(("RateLP : "));
                switch(tpsinfo.rateLP){
                case SONY_DVBT_CODERATE_1_2:
                    mcDBG_MSG_L1(("1/2\n")); break;
                case SONY_DVBT_CODERATE_2_3:
                    mcDBG_MSG_L1(("2/3\n")); break;
                case SONY_DVBT_CODERATE_3_4:
                    mcDBG_MSG_L1(("3/4\n")); break;
                case SONY_DVBT_CODERATE_5_6:
                    mcDBG_MSG_L1(("5/6\n")); break;
                case SONY_DVBT_CODERATE_7_8:
                    mcDBG_MSG_L1(("7/8\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.rateLP));
                    break;
                }

                mcDBG_MSG_L1(("Guard : "));
                switch(tpsinfo.guard){
                case SONY_DVBT_GUARD_1_32:
                    mcDBG_MSG_L1(("1/32\n")); break;
                case SONY_DVBT_GUARD_1_16:
                    mcDBG_MSG_L1(("1/16\n")); break;
                case SONY_DVBT_GUARD_1_8:
                    mcDBG_MSG_L1(("1/8\n")); break;
                case SONY_DVBT_GUARD_1_4:
                    mcDBG_MSG_L1(("1/4\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.guard));
                    break;
                }

                mcDBG_MSG_L1(("Mode : "));
                switch(tpsinfo.mode){
                case SONY_DVBT_MODE_2K:
                    mcDBG_MSG_L1(("2k mode\n")); break;
                case SONY_DVBT_MODE_8K:
                    mcDBG_MSG_L1(("8k mode\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown(%d)\n", (int)tpsinfo.mode));
                    break;
                }

                mcDBG_MSG_L1(("Fnum : %02x\n", tpsinfo.fnum));
                mcDBG_MSG_L1(("LengthIndicator : %02x\n", tpsinfo.lengthIndicator));
                mcDBG_MSG_L1(("CellID : 0x%04X\n", tpsinfo.cellID));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt_monitor_TPSInfo (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2){
            sony_dvbt2_ofdm_t ofdmInfo;
            sony_dvbt2_plp_t PLPInfo;
            sony_dvbt2_l1pre_t l1Pre;   //add for L1pre 20120703
            sony_dvbt2_l1post_t L1Post; //add for L1post 20120703
            sony_result = sony_demod_dvbt2_monitor_Ofdm(pDemod, &ofdmInfo);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("DVB-T2 OFDM Information ----\n"));
                mcDBG_MSG_L1(("Mixed : "));
                if(ofdmInfo.mixed){
                    mcDBG_MSG_L1(("Mixed\n"));
                }else{
                    mcDBG_MSG_L1(("Not mixed\n"));
                }

                if(ofdmInfo.isMiso){
                    mcDBG_MSG_L1(("S1 : MISO\n"));
                }else{
                    mcDBG_MSG_L1(("S1 : SISO\n"));
                }

                mcDBG_MSG_L1(("Mode : "));
                //add for carrier type recognize 20111226
                if (ofdmInfo.bwExt==0)  //BW extend==Normal
                {
                    switch(ofdmInfo.mode){
                    case SONY_DVBT2_M2K:
                        mcDBG_MSG_L1(("2K-N\n")); break;
                    case SONY_DVBT2_M8K:
                        mcDBG_MSG_L1(("8K-N\n")); break;
                    case SONY_DVBT2_M4K:
                        mcDBG_MSG_L1(("4K-N\n")); break;
                    case SONY_DVBT2_M1K:
                        mcDBG_MSG_L1(("1K-N\n")); break;
                    case SONY_DVBT2_M16K:
                        mcDBG_MSG_L1(("16K-N\n")); break;
                    case SONY_DVBT2_M32K:
                        mcDBG_MSG_L1(("32K-N\n")); break;
                    default:
                        mcDBG_MSG_L1(("Unknown\n")); break;
                    }
                }
                else if(ofdmInfo.bwExt==1) //BW extend==Extended
                {
                    switch(ofdmInfo.mode){
                    case SONY_DVBT2_M2K:
                        mcDBG_MSG_L1(("2K-E\n")); break;
                    case SONY_DVBT2_M8K:
                        mcDBG_MSG_L1(("8K-E\n")); break;
                    case SONY_DVBT2_M4K:
                        mcDBG_MSG_L1(("4K-E\n")); break;
                    case SONY_DVBT2_M1K:
                        mcDBG_MSG_L1(("1K-E\n")); break;
                    case SONY_DVBT2_M16K:
                        mcDBG_MSG_L1(("16K-E\n")); break;
                    case SONY_DVBT2_M32K:
                        mcDBG_MSG_L1(("32K-E\n")); break;
                    default:
                        mcDBG_MSG_L1(("Unknown\n")); break;
                    }
                }//end for carrier type recognize 20111226

                mcDBG_MSG_L1(("GI : "));
                switch(ofdmInfo.gi){
                case SONY_DVBT2_G1_32:
                    mcDBG_MSG_L1(("1/32\n")); break;
                case SONY_DVBT2_G1_16:
                    mcDBG_MSG_L1(("1/16\n")); break;
                case SONY_DVBT2_G1_8:
                    mcDBG_MSG_L1(("1/8\n")); break;
                case SONY_DVBT2_G1_4:
                    mcDBG_MSG_L1(("1/4\n")); break;
                case SONY_DVBT2_G1_128:
                    mcDBG_MSG_L1(("1/128\n")); break;
                case SONY_DVBT2_G19_128:
                    mcDBG_MSG_L1(("19/128\n")); break;
                case SONY_DVBT2_G19_256:
                    mcDBG_MSG_L1(("19/256\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("PP : "));
                switch(ofdmInfo.pp){
                case SONY_DVBT2_PP1:
                    mcDBG_MSG_L1(("PP1\n")); break;
                case SONY_DVBT2_PP2:
                    mcDBG_MSG_L1(("PP2\n")); break;
                case SONY_DVBT2_PP3:
                    mcDBG_MSG_L1(("PP3\n")); break;
                case SONY_DVBT2_PP4:
                    mcDBG_MSG_L1(("PP4\n")); break;
                case SONY_DVBT2_PP5:
                    mcDBG_MSG_L1(("PP5\n")); break;
                case SONY_DVBT2_PP6:
                    mcDBG_MSG_L1(("PP6\n")); break;
                case SONY_DVBT2_PP7:
                    mcDBG_MSG_L1(("PP7\n")); break;
                case SONY_DVBT2_PP8:
                    mcDBG_MSG_L1(("PP8\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("BWExt : %u\n", ofdmInfo.bwExt));

                mcDBG_MSG_L1(("PAPR : "));
                {
                    sony_dvbt2_version_t version;
                    sony_result = sony_demod_dvbt2_monitor_Version(pDemod, &version);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("T2 Version Unknown\n"));
                    }else if(version == SONY_DVBT2_V111){
                        switch(ofdmInfo.papr){
                        case SONY_DVBT2_PAPR_0:
                            mcDBG_MSG_L1(("None\n")); break;
                        case SONY_DVBT2_PAPR_1:
                            mcDBG_MSG_L1(("ACE\n")); break;
                        case SONY_DVBT2_PAPR_2:
                            mcDBG_MSG_L1(("TR\n")); break;
                        case SONY_DVBT2_PAPR_3:
                            mcDBG_MSG_L1(("ACE and TR\n")); break;
                        default:
                            mcDBG_MSG_L1(("Unknown\n")); break;
                        }
                    }else{
                        switch(ofdmInfo.papr){
                        case SONY_DVBT2_PAPR_0:
                            mcDBG_MSG_L1(("L1-ACE and P2-TR\n")); break;
                        case SONY_DVBT2_PAPR_1:
                            mcDBG_MSG_L1(("L1-ACE and ACE\n")); break;
                        case SONY_DVBT2_PAPR_2:
                            mcDBG_MSG_L1(("L1-ACE and TR\n")); break;
                        case SONY_DVBT2_PAPR_3:
                            mcDBG_MSG_L1(("L1-ACE, ACE and TR\n")); break;
                        default:
                            mcDBG_MSG_L1(("Unknown\n")); break;
                        }
                    }
                }

                mcDBG_MSG_L1(("Num of Symbols : %u\n", ofdmInfo.numSymbols));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_Ofdm (%s)\n", sony_FormatResult(sony_result)));
            }

            sony_result = sony_demod_dvbt2_monitor_ActivePLP(pDemod, SONY_DVBT2_PLP_DATA, &PLPInfo);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("DVB-T2 PLP Information ----\n"));
                mcDBG_MSG_L1(("ID : %u\n", PLPInfo.id));

                mcDBG_MSG_L1(("Type : "));
                switch(PLPInfo.type){
                case SONY_DVBT2_PLP_TYPE_COMMON:
                    mcDBG_MSG_L1(("Common\n")); break;
                case SONY_DVBT2_PLP_TYPE_DATA1:
                    mcDBG_MSG_L1(("Data Type 1\n")); break;
                case SONY_DVBT2_PLP_TYPE_DATA2:
                    mcDBG_MSG_L1(("Data Type 2\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("Payload : "));
                switch(PLPInfo.payload){
                case SONY_DVBT2_PLP_PAYLOAD_GFPS:
                    mcDBG_MSG_L1(("GFPS\n")); break;
                case SONY_DVBT2_PLP_PAYLOAD_GCS:
                    mcDBG_MSG_L1(("GCS\n")); break;
                case SONY_DVBT2_PLP_PAYLOAD_GSE:
                    mcDBG_MSG_L1(("GSE\n")); break;
                case SONY_DVBT2_PLP_PAYLOAD_TS:
                    mcDBG_MSG_L1(("TS\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("FF : %u\n", PLPInfo.ff));
                mcDBG_MSG_L1(("First RF Index : %u\n", PLPInfo.firstRfIdx));
                mcDBG_MSG_L1(("First Frame Index : %u\n", PLPInfo.firstFrmIdx));
                mcDBG_MSG_L1(("Group ID : %u\n", PLPInfo.groupId));

                mcDBG_MSG_L1(("Constellation : "));


                //add for modulation mode recognize 20120703
                if(PLPInfo.rot==0)  //rotate not used
                {
                    switch(PLPInfo.constell){
                    case SONY_DVBT2_QPSK:
                        mcDBG_MSG_L1(("QPSK\n")); break;
                    case SONY_DVBT2_QAM16:
                        mcDBG_MSG_L1(("16QAM\n")); break;
                    case SONY_DVBT2_QAM64:
                        mcDBG_MSG_L1(("64QAM\n")); break;
                    case SONY_DVBT2_QAM256:
                        mcDBG_MSG_L1(("256QAM\n")); break;
                    default:
                        mcDBG_MSG_L1(("Unknown\n")); break;
                    }
                }
                else if(PLPInfo.rot==1)//rotate used
                {
                    switch(PLPInfo.constell){
                    case SONY_DVBT2_QPSK:
                        mcDBG_MSG_L1(("QPSK-R\n")); break;
                    case SONY_DVBT2_QAM16:
                        mcDBG_MSG_L1(("16QAM-R\n")); break;
                    case SONY_DVBT2_QAM64:
                        mcDBG_MSG_L1(("64QAM-R\n")); break;
                    case SONY_DVBT2_QAM256:
                        mcDBG_MSG_L1(("256QAM-R\n")); break;
                    default:
                        mcDBG_MSG_L1(("Unknown\n")); break;
                    }
                }
                //end for modulation mode recognize 20120703

                mcDBG_MSG_L1(("Code Rate : "));
                switch(PLPInfo.plpCr){
                case SONY_DVBT2_R1_2:
                    mcDBG_MSG_L1(("1/2\n")); break;
                case SONY_DVBT2_R3_5:
                    mcDBG_MSG_L1(("3/5\n")); break;
                case SONY_DVBT2_R2_3:
                    mcDBG_MSG_L1(("2/3\n")); break;
                case SONY_DVBT2_R3_4:
                    mcDBG_MSG_L1(("3/4\n")); break;
                case SONY_DVBT2_R4_5:
                    mcDBG_MSG_L1(("4/5\n")); break;
                case SONY_DVBT2_R5_6:
                    mcDBG_MSG_L1(("5/6\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("Rotation : %s\n", PLPInfo.rot ? "Used" : "Not Used"));

                mcDBG_MSG_L1(("FEC Type : "));
                switch(PLPInfo.fec){
                case SONY_DVBT2_FEC_LDPC_16K:
                    mcDBG_MSG_L1(("16K LDPC\n")); break;
                case SONY_DVBT2_FEC_LDPC_64K:
                    mcDBG_MSG_L1(("64K LDPC\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }

                mcDBG_MSG_L1(("Max Num of PLP Blocks : %u\n", PLPInfo.numBlocksMax));
                mcDBG_MSG_L1(("Frame Interval : %u\n", PLPInfo.frmInt));
                mcDBG_MSG_L1(("Time IL Length : %u\n", PLPInfo.tilLen));
                mcDBG_MSG_L1(("Time IL Type : %u\n", PLPInfo.tilType));
                mcDBG_MSG_L1(("In-Band Flag : %u\n", PLPInfo.inBandFlag));
                mcDBG_MSG_L1(("FEC header type(0: Normal, 1: HEM) : %u\n", PLPInfo.plpMode));//Add for FEC header 20120703
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_ActivePLP (%s)\n", sony_FormatResult(sony_result)));
            }

            //Add for L1Pre 20120703
            sony_result = sony_demod_dvbt2_monitor_L1Pre (pDemod, &l1Pre);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("DVB-T2 L1Pre Information ----\n"));
                mcDBG_MSG_L1(("System Id : %u\n", l1Pre.systemId));
                mcDBG_MSG_L1(("Network Id : %u\n", l1Pre.networkId));
                mcDBG_MSG_L1(("Cell Id : %u\n", l1Pre.cellId));
                //end T2 System Id, Network Id, Cell Id information 20120320
                mcDBG_MSG_L1(("L1 modulation:  "));
                switch(l1Pre.mod){
                case SONY_DVBT2_L1POST_BPSK:
                    mcDBG_MSG_L1(("SONY_DVBT2_L1_BPSK\n")); break;
                case SONY_DVBT2_L1POST_QPSK:
                    mcDBG_MSG_L1(("SONY_DVBT2_L1_QPSK\n")); break;
                case SONY_DVBT2_L1POST_QAM16:
                    mcDBG_MSG_L1(("SONY_DVBT2_L1_QAM16\n")); break;
                case SONY_DVBT2_L1POST_QAM64:
                    mcDBG_MSG_L1(("SONY_DVBT2_L1_QAM64\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }
            }else{
                mcDBG_MSG_L1(("Error in monitorT2_L1Pre\n"));
            }
            //end for L1Pre 20120703
            //Add for L1Post 20120703
            sony_result = sony_demod_dvbt2_monitor_L1Post(pDemod,&L1Post);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("The frequency in Hz : %u\n", L1Post.freq));
                mcDBG_MSG_L1((" FEF type : %u\n", L1Post.fefType));
                mcDBG_MSG_L1((" FEF length : %u\n", L1Post.fefLength));
                mcDBG_MSG_L1((" FEF interval : %u\n", L1Post.fefInterval));
            }else{
                mcDBG_MSG_L1(("Error in monitorT2_L1Post\n"));
            }
            //end for L1Post 20120703

            // List PLP IDs
            {
                UINT8 aPLPId[255];
                UINT8 nNumPLPs = 0;
                UINT32 i = 0;
                sony_result = sony_demod_dvbt2_monitor_DataPLPs(pDemod, aPLPId, &nNumPLPs);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L1(("DVB-T2 PLP ID List ----\n"));
                    mcDBG_MSG_L1(("Num of PLPs : %u\n", nNumPLPs));
                    for(i=0; i<nNumPLPs; i++){
                        mcDBG_MSG_L1(("PLP(%u) : 0x%02x\n", i+1, aPLPId[i]));
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_DataPLPs (%s)\n", sony_FormatResult(sony_result)));
                }
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC){
            sony_dvbc_constellation_t qam = SONY_DVBC_CONSTELLATION_16QAM;
            uint32_t symRate = 0;

            sony_result = sony_demod_dvbc_monitor_QAM(pDemod, &qam);
            mcDBG_MSG_L1(("Constellation : "));
            if(sony_result == SONY_RESULT_OK){
                switch(qam){
                case SONY_DVBC_CONSTELLATION_16QAM:
                    mcDBG_MSG_L1(("16 QAM\n")); break;
                case SONY_DVBC_CONSTELLATION_32QAM:
                    mcDBG_MSG_L1(("32 QAM\n")); break;
                case SONY_DVBC_CONSTELLATION_64QAM:
                    mcDBG_MSG_L1(("64 QAM\n")); break;
                case SONY_DVBC_CONSTELLATION_128QAM:
                    mcDBG_MSG_L1(("128 QAM\n")); break;
                case SONY_DVBC_CONSTELLATION_256QAM:
                    mcDBG_MSG_L1(("256 QAM\n")); break;
                default:
                    mcDBG_MSG_L1(("Unknown\n")); break;
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc_monitor_QAM (%s)\n", sony_FormatResult(sony_result)));
            }

            sony_result = sony_demod_dvbc_monitor_SymbolRate(pDemod, &symRate);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SymbolRate : %d kSymbol/sec\n", symRate));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_dvbc_monitor_SymbolRate (%s)\n", sony_FormatResult(sony_result)));
            }
        }else if(pDemod->system == SONY_DTV_SYSTEM_DVBC2){
            /* QAM */
            {
                sony_dvbc2_constellation_t qam = SONY_DVBC2_CONSTELLATION_16QAM;

                sony_result = sony_demod_dvbc2_monitor_QAM(pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &qam);
                if(sony_result == SONY_RESULT_OK){
                    switch(qam){
                    case SONY_DVBC2_CONSTELLATION_16QAM:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : 16 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_64QAM:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : 64 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_256QAM:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : 256 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_1024QAM:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : 1024 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_4096QAM:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : 4096 QAM\n")); break;
                    default:
                        mcDBG_MSG_L1(("Constellation (Data PLP) : Unknown\n")); break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_QAM (%s)\n", sony_FormatResult(sony_result)));
                }

                sony_result = sony_demod_dvbc2_monitor_QAM(pDemod, SONY_DVBC2_PLP_COMMON, &qam);
                if(sony_result == SONY_RESULT_OK){
                    switch(qam){
                    case SONY_DVBC2_CONSTELLATION_16QAM:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : 16 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_64QAM:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : 64 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_256QAM:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : 256 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_1024QAM:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : 1024 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_4096QAM:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : 4096 QAM\n")); break;
                    default:
                        mcDBG_MSG_L1(("Constellation (Common PLP) : Unknown\n")); break;
                    }
                }else if(sony_result == SONY_RESULT_ERROR_HW_STATE){
                    mcDBG_MSG_L1(("Constellation (Common PLP) : No Associated Common PLP\n"));
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_QAM (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* LDPC code length */
            {
                sony_dvbc2_ldpc_code_length_t codeLength;

                sony_result = sony_demod_dvbc2_monitor_LDPCCodeLength(pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &codeLength);
                if(sony_result == SONY_RESULT_OK){
                    switch(codeLength){
                    case SONY_DVBC2_LDPC_SHORT:
                        mcDBG_MSG_L1(("Code length (Data PLP) : 16K\n")); break;
                    case SONY_DVBC2_LDPC_NORMAL:
                        mcDBG_MSG_L1(("Code length (Data PLP) : 64K\n")); break;
                    default:
                        mcDBG_MSG_L1(("Code length (Data PLP) : Unknown\n")); break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_LDPCCodeLength (%s)\n", sony_FormatResult(sony_result)));
                }

                sony_result = sony_demod_dvbc2_monitor_LDPCCodeLength(pDemod, SONY_DVBC2_PLP_COMMON, &codeLength);
                if(sony_result == SONY_RESULT_OK){
                    switch(codeLength){
                    case SONY_DVBC2_LDPC_SHORT:
                        mcDBG_MSG_L1(("Code length (Common PLP) : 16K\n")); break;
                    case SONY_DVBC2_LDPC_NORMAL:
                        mcDBG_MSG_L1(("Code length (Common PLP) : 64K\n")); break;
                    default:
                        mcDBG_MSG_L1(("Code length (Common PLP) : Unknown\n")); break;
                    }
                }else if(sony_result == SONY_RESULT_ERROR_HW_STATE){
                    mcDBG_MSG_L1(("Code length (Common PLP) : No Associated Common PLP\n"));
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_LDPCCodeLength (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* Code rate */
            {
                sony_dvbc2_plp_code_rate_t codeRate;

                sony_result = sony_demod_dvbc2_monitor_CodeRate(pDemod, SONY_DVBC2_PLP_NORMAL_DATA, &codeRate);
                if(sony_result == SONY_RESULT_OK){
                    switch(codeRate){
                    case SONY_DVBC2_R2_3:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 2/3\n")); break;
                    case SONY_DVBC2_R3_4:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 3/4\n")); break;
                    case SONY_DVBC2_R4_5:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 4/5\n")); break;
                    case SONY_DVBC2_R5_6:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 5/6\n")); break;
                    case SONY_DVBC2_R8_9:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 8/9\n")); break;
                    case SONY_DVBC2_R9_10:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : 9/10\n")); break;
                    default:
                        mcDBG_MSG_L1(("Code rate (Data PLP) : Unknown\n")); break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_CodeRate (%s)\n", sony_FormatResult(sony_result)));
                }

                sony_result = sony_demod_dvbc2_monitor_CodeRate(pDemod, SONY_DVBC2_PLP_COMMON, &codeRate);
                if(sony_result == SONY_RESULT_OK){
                    switch(codeRate){
                    case SONY_DVBC2_R2_3:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 2/3\n")); break;
                    case SONY_DVBC2_R3_4:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 3/4\n")); break;
                    case SONY_DVBC2_R4_5:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 4/5\n")); break;
                    case SONY_DVBC2_R5_6:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 5/6\n")); break;
                    case SONY_DVBC2_R8_9:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 8/9\n")); break;
                    case SONY_DVBC2_R9_10:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : 9/10\n")); break;
                    default:
                        mcDBG_MSG_L1(("Code rate (Common PLP) : Unknown\n")); break;
                    }
                }else if(sony_result == SONY_RESULT_ERROR_HW_STATE){
                    mcDBG_MSG_L1(("Code rate (Common PLP) : No Associated Common PLP\n"));
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_CodeRate (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* Guard interval */
            {
                sony_dvbc2_l1_gi_t gi;

                sony_result = sony_demod_dvbc2_monitor_GI(pDemod, &gi);
                if(sony_result == SONY_RESULT_OK){
                    switch(gi){
                    case SONY_DVBC2_GI_1_64:
                        mcDBG_MSG_L1(("Guard interval : 1/64\n")); break;
                    case SONY_DVBC2_GI_1_128:
                        mcDBG_MSG_L1(("Guard interval : 1/128\n")); break;
                    default:
                        mcDBG_MSG_L1(("Guard interval : Unknown\n")); break;
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_GI (%s)\n", sony_FormatResult(sony_result)));
                }
            }

            /* L1 */
            {
                sony_dvbc2_l1_t l1Data;

                sony_result = sony_demod_dvbc2_monitor_L1Data(pDemod, &l1Data);
                if(sony_result == SONY_RESULT_OK){
                    mcDBG_MSG_L1(("== L1 Part2 Information ================\n"));
                    mcDBG_MSG_L1(("Network ID : 0x%04X\n", l1Data.networkID));
                    mcDBG_MSG_L1(("System ID : 0x%04X\n", l1Data.systemID));
                    mcDBG_MSG_L1(("Start frequency : %u\n", l1Data.startFrequency));
                    mcDBG_MSG_L1(("Bandwidth : %d\n", l1Data.bandwidth));

                    switch(l1Data.gi){
                    case SONY_DVBC2_GI_1_128:
                        mcDBG_MSG_L1(("Guard interval : 1/128\n"));
                        break;
                    case SONY_DVBC2_GI_1_64:
                        mcDBG_MSG_L1(("Guard interval : 1/64\n"));
                        break;
                    default:
                        mcDBG_MSG_L1(("Guard interval : Unknown\n"));
                        break;
                    }

                    mcDBG_MSG_L1(("Frame length : %d\n", l1Data.frameLength));
                    mcDBG_MSG_L1(("Change Counter L1 : %d\n", l1Data.changeCntL1));
                    mcDBG_MSG_L1(("Data slice num : %d\n", l1Data.numDs));
                    mcDBG_MSG_L1(("Notch num : %d\n", l1Data.numNotchs));
                    mcDBG_MSG_L1(("Data slice ID : %d\n", l1Data.dsID));
                    mcDBG_MSG_L1(("Data slice tuning position : %d\n", l1Data.dsTunePos));
                    mcDBG_MSG_L1(("Data slice offset (Left) : %d\n", l1Data.dsOffsetOnLeft));
                    mcDBG_MSG_L1(("Data slice offset (Right) : %d\n", l1Data.dsOffsetOnRight));

                    switch(l1Data.dsTiDepth){
                    case SONY_DVBC2_DS_TI_DEPTH_NONE:
                        mcDBG_MSG_L1(("Data slice TI depth : None\n")); break;
                    case SONY_DVBC2_DS_TI_DEPTH_4_OFDM_SYM:
                        mcDBG_MSG_L1(("Data slice TI depth : 4\n")); break;
                    case SONY_DVBC2_DS_TI_DEPTH_8_OFDM_SYM:
                        mcDBG_MSG_L1(("Data slice TI depth : 8\n")); break;
                    case SONY_DVBC2_DS_TI_DEPTH_16_OFDM_SYM:
                        mcDBG_MSG_L1(("Data slice TI depth : 16\n")); break;
                    default:
                        mcDBG_MSG_L1(("Data slice TI depth : Unknown\n")); break;
                    }

                    switch(l1Data.dsType){
                    case SONY_DVBC2_DS_TYPE_1:
                        mcDBG_MSG_L1(("Data slice type : Type 1\n")); break;
                    case SONY_DVBC2_DS_TYPE_2:
                        mcDBG_MSG_L1(("Data slice type : Type 2\n")); break;
                    default:
                        mcDBG_MSG_L1(("Data slice type : Unknown\n")); break;
                    }

                    switch(l1Data.fecHdrType){
                    case SONY_DVBC2_FEC_HEADER_TYPE_ROBUST:
                        mcDBG_MSG_L1(("FEC header type : Robust\n")); break;
                    case SONY_DVBC2_FEC_HEADER_TYPE_HIGH_EFFICIENCY:
                        mcDBG_MSG_L1(("FEC header type : High Efficiency\n")); break;
                    default:
                        mcDBG_MSG_L1(("FEC header type : Unknown\n")); break;
                    }

                    mcDBG_MSG_L1(("Data slice const conf : %s\n", l1Data.dsConstConf ? "Fixed" : "Variable"));
                    mcDBG_MSG_L1(("Data slice left notch : %d\n", l1Data.dsLeftNotch));
                    mcDBG_MSG_L1(("Data slice PLP num : %d\n", l1Data.dsNumPLP));
                    mcDBG_MSG_L1(("PLP ID : %d\n", l1Data.plpID));
                    mcDBG_MSG_L1(("PLP bundled : %d\n", l1Data.isPlpBundled));

                    switch(l1Data.plpType){
                    case SONY_DVBC2_PLP_COMMON:
                        mcDBG_MSG_L1(("PLP type : Common\n")); break;
                    case SONY_DVBC2_PLP_GROUPED_DATA:
                        mcDBG_MSG_L1(("PLP type : Grouped data\n")); break;
                    case SONY_DVBC2_PLP_NORMAL_DATA:
                        mcDBG_MSG_L1(("PLP type : Normal data\n")); break;
                    default:
                        mcDBG_MSG_L1(("PLP type : Unknown\n")); break;
                    }

                    switch(l1Data.plpPayloadType){
                    case SONY_DVBC2_PLP_PAYLOAD_TYPE_GFPS:
                        mcDBG_MSG_L1(("PLP payload type : GFPS\n")); break;
                    case SONY_DVBC2_PLP_PAYLOAD_TYPE_GCS:
                        mcDBG_MSG_L1(("PLP payload type : GCS\n")); break;
                    case SONY_DVBC2_PLP_PAYLOAD_TYPE_GSE:
                        mcDBG_MSG_L1(("PLP payload type : GSE\n")); break;
                    case SONY_DVBC2_PLP_PAYLOAD_TYPE_TS:
                        mcDBG_MSG_L1(("PLP payload type : TS\n")); break;
                    default:
                        mcDBG_MSG_L1(("PLP payload type : Unknown\n")); break;
                    }

                    mcDBG_MSG_L1(("PLP group ID : %d\n", l1Data.plpGroupID));
                    mcDBG_MSG_L1(("PLP start : %d\n", l1Data.plpStart));

                    switch(l1Data.plpFecType){
                    case SONY_DVBC2_LDPC_SHORT:
                        mcDBG_MSG_L1(("PLP FEC type : 16KLDPC\n")); break;
                    case SONY_DVBC2_LDPC_NORMAL:
                        mcDBG_MSG_L1(("PLP FEC type : 64KLDPC\n")); break;
                    default:
                        mcDBG_MSG_L1(("PLP FEC type : Unknown\n")); break;
                    }

                    switch(l1Data.plpMod){
                    case SONY_DVBC2_CONSTELLATION_16QAM:
                        mcDBG_MSG_L1(("PLP modulation : 16 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_64QAM:
                        mcDBG_MSG_L1(("PLP modulation : 64 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_256QAM:
                        mcDBG_MSG_L1(("PLP modulation : 256 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_1024QAM:
                        mcDBG_MSG_L1(("PLP modulation : 1024 QAM\n")); break;
                    case SONY_DVBC2_CONSTELLATION_4096QAM:
                        mcDBG_MSG_L1(("PLP modulation : 4096 QAM\n")); break;
                    default:
                        mcDBG_MSG_L1(("PLP modulation : Unknown\n")); break;
                    }

                    switch(l1Data.plpCod){
                    case SONY_DVBC2_R2_3:
                        mcDBG_MSG_L1(("PLP code rate : 2/3\n")); break;
                    case SONY_DVBC2_R3_4:
                        mcDBG_MSG_L1(("PLP code rate : 3/4\n")); break;
                    case SONY_DVBC2_R4_5:
                        mcDBG_MSG_L1(("PLP code rate : 4/5\n")); break;
                    case SONY_DVBC2_R5_6:
                        mcDBG_MSG_L1(("PLP code rate : 5/6\n")); break;
                    case SONY_DVBC2_R8_9: /* 5 */
                        if(l1Data.plpFecType == 0){
                            mcDBG_MSG_L1(("PLP code rate : 8/9\n")); break;
                        }else{
                            mcDBG_MSG_L1(("PLP code rate : 9/10\n")); break;
                        }
                    default:
                        mcDBG_MSG_L1(("PLP code rate : Unknown\n")); break;
                    }

                    mcDBG_MSG_L1(("PSISI reprocessing : %d\n"));
                    mcDBG_MSG_L1(("Transport stream ID : 0x%04X\n", l1Data.transportID));
                    mcDBG_MSG_L1(("Original network ID : 0x%04X\n", l1Data.origNetworkID));
                    mcDBG_MSG_L1(("Reserved tone : %d\n", l1Data.reservedTone));

                    if(l1Data.numNotchs > 0){
                        int i = 0;
                        for(i=0; i<l1Data.numNotchs; i++){
                            mcDBG_MSG_L1(("Notch [%d] start : %d\n", i, l1Data.notches[i].notchStart));
                            mcDBG_MSG_L1(("Notch [%d] width : %d\n", i, l1Data.notches[i].notchWidth));
                        }
                    }
                }else{
                    mcDBG_MSG_L1(("Error in sony_demod_dvbc2_monitor_L1Data (%s)\n", sony_FormatResult(sony_result)));
                }
            }
        }else{
            mcDBG_MSG_L1(("Invalid System\n"));
        }

        /* TS Format */
        /*
        {
            TS_FMT_T tsfmt;
            if(sony_GetTsFmt(pDemod, &tsfmt)){
                mcDBG_MSG_L1(("SerOrPar : %d\n", tsfmt.SerOrPar));
                mcDBG_MSG_L1(("ClockPolarity : %d\n", tsfmt.ClockPolarity));
                mcDBG_MSG_L1(("Tristate : %d\n", tsfmt.fgTristate));
                mcDBG_MSG_L1(("TsClock : %d\n", tsfmt.u4TsClockInKHz));
                mcDBG_MSG_L1(("GatedClock : %d\n", tsfmt.fgGatedClock));
                mcDBG_MSG_L1(("ExternalDemod : %d\n", tsfmt.fgExternalDemod));
            }else{
                mcDBG_MSG_L1(("Error in GetTsFmt\n"));
            }
        }
        */

        mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
        break;
    /* Tuner Debug Command ----------------------------------------------------- */
    case 'a':
        {
            /* Tuner Dump/Read/Write */
            const UINT8 tunerI2cAddress = 0xC0; /* Please fix it if tuner address is changed. */
            sony_i2c_t i2cTuner;

            /* If tuner is accessed by I2C Gateway */
	#ifndef CC_SONY_DISABLE_I2C_REPEATER
	              sony_i2c_MTK_CreateI2c(&i2cTuner);
	#else 
	             sony_i2c_MTK_CreateI2cGw(&i2cTuner, pDemod->i2cAddressSLVT, 0x09);
 	#endif
	
            switch(aszArgv[0][1]){
            case 'd':
            default:
                {
                    UINT8 data[256];

                    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock); // Enter Critical Section
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, 0x00, data, 256);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock); // Leave Critical Section
                    if(sony_result != SONY_RESULT_OK){
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
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    length = (UINT32)sony_strtoul(aszArgv[2]);
                    if(length > 256){
                        length = 256;
                    }
                    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, subAddress, data, length);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
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
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    value = (UINT8)sony_strtoul(aszArgv[2]);
                    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.WriteOneRegister(&i2cTuner, tunerI2cAddress, subAddress, value);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Wrote Tuner Register 0x%02X = 0x%02X\n", subAddress, value));
                }
                break;
            }
            break;
        }
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
            bank = (UINT8)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock); // Enter Critical Section
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
                return;
            }
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, 0x00, data, 256);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock); // Leave Critical Section
            if(sony_result != SONY_RESULT_OK){
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
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, length are necessary.\n"));
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
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            length = (UINT32)sony_strtoul(aszArgv[4]);
            if(length > 256){
                length = 256;
            }
            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, subAddress, data, length);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
            if(sony_result != SONY_RESULT_OK){
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
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, value are necessary.\n"));
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
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            value = (UINT8)sony_strtoul(aszArgv[4]);
            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, subAddress, value);
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Wrote Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, subAddress, value));
        }
        break;
    case 'f':
        {
            UINT32 u4Freq = 0;
            PARAM_SETFREQ_T param;

            if(i4Argc < 2){
                mcDBG_MSG_L1(("Error: No Frequency.\n"));
                return;
            }
            u4Freq = (UINT32)sony_strtoul(aszArgv[1]); // freq

            param.Freq = u4Freq;
            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                param.Modulation = MOD_DVBT; break;
            case SONY_DTV_SYSTEM_DVBT2:
                param.Modulation = MOD_DVBT2; break;
            case SONY_DTV_SYSTEM_DVBC:
                param.Modulation = MOD_DVBC; break;
            default:
                mcDBG_MSG_L1(("Error: Demod is not active.\n"));
                return;
            }
            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

            sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable

            if(ITuner_SetFreq(ITunerGetCtx(), &param))
            {
                mcDBG_MSG_L1(("Failed on Config tuner (%d KHz)\n", u4Freq));
            }
            else
            {
                mcDBG_MSG_L1(("Config tuner(%d KHz) OK!\n", u4Freq));
            }

            sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable

            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
        }
        break;
    case 'l':
        {
            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
            // TS Output Parallel/Serial
            sony_result = sony_integ_SleepT_C(pInteg);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error: Sleep Failed.\n"));
            }
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
        }
        break;
    case 'p':
        {
            UINT32 frequencyKHz = 0;
            UINT8 isAnalog = 0;

            switch(aszArgv[1][0]){
            case 'd':
            case 'D':
                isAnalog = 0;
                break;
            case 'a':
            case 'A':
                isAnalog = 1;
                break;
            default:
                mcDBG_MSG_L1(("Error: Digital/Analog ('d' or 'a') is necessary.\n"));
                return;
            }
            frequencyKHz = (UINT32)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
            sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
            if(ITuner_OP(ITunerGetCtx(), itSetTrapThresold, isAnalog, &frequencyKHz) == ITUNER_OK){
                if(isAnalog){
                    mcDBG_MSG_L1(("Analog trap threshold : %d kHz\n", frequencyKHz));
                }else{
                    mcDBG_MSG_L1(("Digital trap threshold : %d kHz\n", frequencyKHz));
                }
            }else{
                mcDBG_MSG_L1(("ITuner_OP failed.\n"));
            }
            sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
            mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
        }
        break;
    default:
        break;
    }
}

static void DVBT2_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add your implementation here
    mcDBG_MSG_L2(("No implementation in DVBT2_TunerDetachI2C()\n"));

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
}

#define C_T2_FREQ_DBOUND_UPPER   862*1000*1000     //Sony manual scan will upper to 862MHz and scan down
#define C_T2_FREQ_DBOUND_LOWER    47*1000*1000     //Sony manual scan will lower to 47MHz and scan up
//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_XXXXX
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_XXXXX_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 DVBT2_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    TUNER_DIG_ATTRIBUTE_T *pAttri;
    ITUNER_CTX_T *pTunerCtx;
    INT32 i4Temp = 0;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    pTunerCtx = ITunerGetCtx();

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L4(("%s \n", __FUNCTION__));

    //add driver implementation code here
    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
        {
            UINT32 ber = sony_GetBER();
            i4Temp = (INT32)ber;
        }
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
        pAttri = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
        //Tuner frequency boundary
        pAttri->ui4_lower_bound_freq = C_T2_FREQ_DBOUND_LOWER;
        pAttri->ui4_upper_bound_freq = C_T2_FREQ_DBOUND_UPPER;
        // freq offset tolerence range in Hz (1M mapping to 100K)
        pAttri->b_auto_qam_capability = TRUE;
        pAttri->b_auto_sr_capability = TRUE;
        switch (pDemod->system)
        {
        case SONY_DTV_SYSTEM_DVBC:
            pAttri->ui4_fine_tune_delta_freq = 400000;
            break;
        case SONY_DTV_SYSTEM_DVBT:
        case SONY_DTV_SYSTEM_DVBT2:
            switch (pDemod->bandwidth){
            case SONY_DTV_BW_6_MHZ:  pAttri->ui4_fine_tune_delta_freq = 536000; break; //6MHz
            case SONY_DTV_BW_7_MHZ:  pAttri->ui4_fine_tune_delta_freq = 625000; break; //7MHz
            case SONY_DTV_BW_8_MHZ:  pAttri->ui4_fine_tune_delta_freq = 714000; break; //8MHz
            default: pAttri->ui4_fine_tune_delta_freq = 0;      break;
            }
            break;
        default:
            break;
        }
        break;
    case TUNER_GET_TYPE_RAW_SIGNAL_LEVEL:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_RAW_SIGNAL_LEVEL\r\n"));
        *pzAttributeLen = sizeof(INT8);
        //i4Temp =DVBT2_GetIFAGC(pSonyDemod);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_BER_TOLERANCE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER_TOLERANCE\r\n"));
        *pzAttributeLen = sizeof(INT32);
        i4Temp = 430;  // copy from in-house DVBT
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE\r\n"));
        *pzAttributeLen = sizeof(UINT8);
        x_memcpy(pzAttribute, &(pTunerCtx->specific_member.eu_ctx.m_SigLvTh), *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T tsFmt;
            *pzAttributeLen = sizeof(TS_FMT_T);
            sony_GetTsFmt(pDemod, &tsFmt);
            x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);
        }
        break;
    default:
        mcDBG_MSG_L4(("unknown: e_get_type = %d\r\n", e_get_type));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    return DRVAPI_TUNER_OK;
}

//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_XXXXX
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_XXXXX_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 DVBT2_TunerGetAttribute2(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen, DRV_CONN_TYPE_T e_conn_type)
{
    TUNER_DIG_ATTRIBUTE_T *pAttri;
    ITUNER_CTX_T *pTunerCtx;
    INT32 i4Temp = 0;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    pTunerCtx = ITunerGetCtx();

     if ((e_get_type==TUNER_GET_TYPE_SQI)&&(fgChanScanforGetAtt==TRUE))
    {
        x_thread_delay((UINT32)WAIT_SQI_STABLE_TIMEOUT);
        mcDBG_MSG_L2(("Before get SQI delaytime = %d\r\n",WAIT_SQI_STABLE_TIMEOUT));
        fgChanScanforGetAtt=FALSE;
    }

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);

    mcDBG_MSG_L4(("%s \n", __FUNCTION__));

    //add driver implementation code here
    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
        {
            UINT32 ber = sony_GetBER();
            i4Temp = (INT32)ber;
        }
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_PRE_VBER:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_PRE_BER\r\n"));
        {
            UINT32 u4Preber = sony_GetPreBER();
            i4Temp = (INT32)u4Preber;
        }
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
        pAttri = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
        //Tuner frequency boundary
        pAttri->ui4_lower_bound_freq = C_T2_FREQ_DBOUND_LOWER;
        pAttri->ui4_upper_bound_freq = C_T2_FREQ_DBOUND_UPPER;
        // freq offset tolerence range in Hz (1M mapping to 100K)
        pAttri->b_auto_qam_capability = TRUE;
        pAttri->b_auto_sr_capability = TRUE;
        switch (e_conn_type)
        {
            case TUNER_CONN_TYPE_CAB_DIG:
               pAttri->ui4_fine_tune_delta_freq = 400000;
               mcDBG_MSG_L2(("delta_freq = %d\r\n",pAttri->ui4_fine_tune_delta_freq));
               break;
            case TUNER_CONN_TYPE_TER_DIG:
            {
                switch (pDemod->bandwidth)
                {
                    case SONY_DTV_BW_6_MHZ:  pAttri->ui4_fine_tune_delta_freq = 536000; break; //6MHz
                    case SONY_DTV_BW_7_MHZ:  pAttri->ui4_fine_tune_delta_freq = 625000; break; //7MHz
                    case SONY_DTV_BW_8_MHZ:  pAttri->ui4_fine_tune_delta_freq = 714000; break; //8MHz

                    default: pAttri->ui4_fine_tune_delta_freq = DEFAULT_DVBT_DELTA_FREQ;        break;
                }
                break;
            }
            default:
                break;
        }
        break;
    case TUNER_GET_TYPE_RAW_SIGNAL_LEVEL:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_RAW_SIGNAL_LEVEL\r\n"));
        *pzAttributeLen = sizeof(INT8);
        //i4Temp =DVBT2_GetIFAGC(pSonyDemod);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_BER_TOLERANCE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER_TOLERANCE\r\n"));
        *pzAttributeLen = sizeof(INT32);
        i4Temp = 430;  // copy from in-house DVBT
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE\r\n"));
        *pzAttributeLen = sizeof(UINT8);
        x_memcpy(pzAttribute, &(pTunerCtx->specific_member.eu_ctx.m_SigLvTh), *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T tsFmt;
            *pzAttributeLen = sizeof(TS_FMT_T);
            sony_GetTsFmt(pDemod, &tsFmt);
            x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_UEC:
        {
            UINT16 UEC = (UINT16)sony_GetUEC();
            mcDBG_MSG_L2(("e_get_type =TUNER_GET_TYPE_UEC\r\n"));
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &UEC, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SQI:
        *pzAttributeLen = sizeof(INT32);
        {
            UINT8 sqi = 0;
            sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_Quality(pDemod, &sqi); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_Quality(pDemod, &sqi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result != SONY_RESULT_OK){
                sqi = 0;
            }
            i4Temp = (INT32)sqi;
            mcDBG_MSG_L2(("SQI = %d\r\n", sqi));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_SIGNAL_LEVEL:
    case TUNER_GET_TYPE_SSI:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL(SSI)\r\n"));
        *pzAttributeLen = sizeof(INT32);
        {
            UINT8 ssi = 0;
            sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_integ_dvbt_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_integ_dvbt2_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_integ_dvbc_monitor_SSI(pInteg, &ssi); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_integ_dvbc2_monitor_SSI(pInteg, &ssi); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }

            if(sony_result != SONY_RESULT_OK){
                ssi = 0;
            }
            i4Temp = (INT32)ssi;
            mcDBG_MSG_L2(("SSI = %d\r\n", ssi));
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;

    //add by AEC 20120703
    case TUNER_GET_TYPE_AGC:
        *pzAttributeLen = sizeof(INT32);
        {
            uint32_t ifAgc = 0;
            sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_IFAGCOut(pDemod, &ifAgc); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_IFAGCOut(pDemod, &ifAgc); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            ifAgc=255-(ifAgc>>4);
            //ifAgc = 108 - ifAgc * 100 / 4095;
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("IFAGC (%%) : %d\n", ifAgc));
            }else{
                ifAgc = 0;
                mcDBG_MSG_L2(("Error in monitor_IFAGCOut\n"));
            }
            i4Temp = (INT32)ifAgc;
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        }
        break;

    case TUNER_GET_TYPE_DBM_SNR:
        *pzAttributeLen = sizeof(INT32);
        {
            int32_t snr = 0;
             sony_result_t sony_result = SONY_RESULT_OK;

            switch(pDemod->system){
            case SONY_DTV_SYSTEM_DVBT:
                sony_result = sony_demod_dvbt_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBT2:
                sony_result = sony_demod_dvbt2_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBC:
                sony_result = sony_demod_dvbc_monitor_SNR(pDemod, &snr); break;
            case SONY_DTV_SYSTEM_DVBC2:
                sony_result = sony_demod_dvbc2_monitor_SNR(pDemod, &snr); break;
            default:
                sony_result = SONY_RESULT_ERROR_SW_STATE; break;
            }
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("SNR (dB) : %d.%03d\n", snr/1000U, snr%1000U));
            }else{
                snr = 0;
                mcDBG_MSG_L2(("Error in monitor_SNR \n"));
            }
            snr = snr/100;
            x_memcpy(pzAttribute, &snr, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_DVBT2_PLP:
        {
            TUNER_DVBT2_PLP_LIST_INFO sPlpIdList;
            //DVBT2_PARAM mDVBT2_PARAM;
            UINT8 u1plpcnt=0;
            sony_result_t sony_result=SONY_RESULT_OK;
            UINT8 aPLPId[255];
            UINT8 nNumPLPs = 0;
            UINT32 i = 0;

            *pzAttributeLen = sizeof(TUNER_DVBT2_PLP_LIST_INFO);
            //mDVBT2_PARAM.eOP=GET_MPLP_LIST;
            //MTK_DVBT2_GetInfo(&mDVBT2_PARAM);
            //sPlpIdList.ui1_current_count=mDVBT2_PARAM.outplpNumb;//d110916_Haibo:PLPid table length

            sony_result = sony_demod_dvbt2_monitor_DataPLPs(pDemod, aPLPId, &nNumPLPs);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("DVB-T2 PLP ID List ----\n"));
                mcDBG_MSG_L2(("Num of PLPs : %u\n", nNumPLPs));
                for(i=0; i<nNumPLPs; i++){
                    mcDBG_MSG_L2(("PLP(%u) : 0x%02x\n", i+1, aPLPId[i]));
                }
            }else{
                mcDBG_MSG_L2(("Error in monitorT2_DataPLPs\n"));
                nNumPLPs=1;//default splp
            }
            sPlpIdList.ui1_current_count=nNumPLPs;//mDVBT2_PARAM.outplpdataNumb;//d110920_Haibo:Get data plp table length
            for(;u1plpcnt<sPlpIdList.ui1_current_count;u1plpcnt++)
            {
                mcDBG_MSG_L2(("index %d data plp id=%d\n",u1plpcnt,aPLPId[u1plpcnt]));
            }
            x_memcpy(&(sPlpIdList.ui1_PLPLIST[0]), &aPLPId[0], sPlpIdList.ui1_current_count*sizeof(UINT8));
            x_memcpy(pzAttribute, &sPlpIdList, *pzAttributeLen);
        }
        break;
        //end add by AEC 20120703
    case TUNER_GET_TYPE_CELL_ID:
        {
            sony_dvbt_tpsinfo_t tpsInfo;
            sony_dvbt2_l1pre_t l1Pre;
            sony_result_t sony_result = SONY_RESULT_OK;
            UINT16 u2Temp = 0;

            *pzAttributeLen = sizeof(UINT16);

            if(pDemod->system == SONY_DTV_SYSTEM_DVBT)
            {
                sony_result = sony_demod_dvbt_monitor_TPSInfo(pDemod, &tpsInfo);
                if(sony_result == SONY_RESULT_OK)
                {
                    u2Temp = (UINT16)tpsInfo.cellID;
                }
                else
                {
                    mcDBG_MSG_L1(("Error in dvb_demod_monitorT_TPSInfo (%s)\n", sony_FormatResult(sony_result)));
                }
            }
            else if(pDemod->system == SONY_DTV_SYSTEM_DVBT2)
            {
                sony_result = sony_demod_dvbt2_monitor_L1Pre (pDemod, &l1Pre);
                if(sony_result == SONY_RESULT_OK)
                {
                    u2Temp = (UINT16)l1Pre.cellId;
                }
                else
                {
                    mcDBG_MSG_L1(("Error in sony_demod_dvbt2_monitor_L1Pre (%s)\n", sony_FormatResult(sony_result)));
                }
            }
            mcDBG_MSG_L1(("CELL_ID u2Temp = %d\n",u2Temp));
            x_memcpy(pzAttribute, &u2Temp, *pzAttributeLen);
        }
        break;
    #ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
    case TUNER_GET_TYPE_DVBC2_DS_PLP_LIST:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DVBC2_DS_PLP\r\n"));
        *pzAttributeLen = sizeof(TUNER_DVBC2_DS_PLP_LIST);
        x_memcpy(pzAttribute, &g_sony_demod_ctx.c2DsPlpList, *pzAttributeLen);
        break;
    case TUNER_GET_TYPE_DVBC2_NEXT_FREQ:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DVBC2_NEXT_FREQ\r\n"));
        {
            UINT32 ui4Freq = g_sony_demod_ctx.c2NextFrequency;
            i4Temp = (INT32)ui4Freq;
        }
        *pzAttributeLen = sizeof(INT32);
        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
        break;
    #endif

#ifdef CC_PROJECT_FOR_SONY
    case TUNER_GET_TYPE_DIG_DEMOD_STATUS:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_DEMOD_STATUS\r\n"));
        {
            TUNER_TER_DIG_TUNE_STATUS_T  tuneStatus;
            *pzAttributeLen = sizeof(TUNER_TER_DIG_TUNE_STATUS_T);
            DVBT2_TunerGetModuInfor(&tuneStatus);
            x_memcpy(pzAttribute, &tuneStatus, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_BER_UPPER:
        {
            UINT16 berUpper;
            UINT32 ber=sony_GetBER();
            berUpper = (UINT16)((ber>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_BER_LOWER:
        {
            UINT16 berLower;
            UINT32 ber=sony_GetBER();
            berLower = (UINT16)(ber&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berLower, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_UPPER:
        {
            UINT16 perUpper;
            UINT32 per=sony_GetPER();
            perUpper = (UINT16)((per>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perUpper, *pzAttributeLen);
        }
        break;
    case TUNER_GET_TYPE_PER_LOWER:
        {
            UINT16 perLower;
            UINT32 per=sony_GetPER();
            perLower = (UINT16)(per&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perLower, *pzAttributeLen);
        }
        break;
#endif // CC_PROJECT_FOR_SONY

    default:
        mcDBG_MSG_L4(("unknown: e_get_type = %d\r\n", e_get_type));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    return DRVAPI_TUNER_OK;
}
//-------------------------------------------------------------------------
/**
 *  This API set attribute to tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_set_type         tuner set type, check x_tuner.h set operation TUNER_SET_TYPE_XXXX
 *  @param   *pvAttribute       set value.
 *  @param   zAttributeLen      length of *pvAttribute.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT32 DVBT2_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    UCHAR ucData = *(UCHAR *)pvAttribute;
    //UCHAR *pucData = (UCHAR *)pvAttribute;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    //add driver implementation code here
    mcDBG_MSG_L1(("111%s \n", __FUNCTION__));

    switch (e_set_type)
    {
    case TUNER_SET_TYPE_TS_TRISTATE:
        {
            UINT8 data = 0;
            if(ucData == BOOL_FALSE){
                mcDBG_MSG_L2(("[DVBT2] Enable TS output\n"));
                // Enable TS output
                data = 0;
            }else if (ucData == BOOL_TRUE){
                mcDBG_MSG_L2(("[DVBT2] Disable TS output\n"));
                // Disable TS output
                data = 1;
            }else{
                break;
            }
            // TS Output Enable (Bank:00h, Addr:C3h, Bit[0])
            // NOTE: This register is automatically controlled by the CXD2854 driver.
            //       Normally, this control is unnecessary from MTK upper layer.
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, data);
        }
        break;
    case TUNER_SET_TYPE_TS_SP:
        break;
    case TUNER_SET_TYPE_CI_CARD_TYPE:
        break;

#ifdef CC_PROJECT_FOR_SONY
    case TUNER_SET_TYPE_TS_GAP_LENGTH:
        {
            UINT8 data = 0;
            if(ucData>7)
            {
                ucData = 0x4; 
            }
            /* Set bank 00h */            
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
            pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x67, &data, 1);
            data = data&(ucData|0xF8);
            mcDBG_MSG_L2(("Set TS Gap Length: %d\n",data));
            pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xd6, ucData);
        }
        break;
#endif // CC_PROJECT_FOR_SONY

    /*
    case TUNER_SET_TYPE_AUTOPLP:
        // Save autoPLP mode to sony_demod_ctx_t struct
        if (ucData == BOOL_FALSE) {
            mcDBG_MSG_L2(("[DVBT2] disable auto plp\n"));
            g_sony_demod_ctx.autoPLP = 0;
        }
        else if (ucData == BOOL_TRUE) {
            mcDBG_MSG_L2(("[DVBT2] auto plp\n"));
            g_sony_demod_ctx.autoPLP = 1;
        }
        break;
    */
    default:
        mcDBG_MSG_L2(("unknown: set Attribute:%d\r\n",e_set_type));
        break;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);

    return DRVAPI_TUNER_OK;

}

//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
/*typedef INT16 (*LTDIS_TunerGetSync_fct) (PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                        DRV_CONN_TYPE_T e_conn_type);
INT16 DVBT2_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
*/

static void PD_DVBT2_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose   = DVBT2_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq     = DVBT2_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = DVBT2_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = DVBT2_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBT2_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBT2_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBT2_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBT2_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = DVBT2_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer  = DVBT2_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = DVBT2_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBT2_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBT2_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = DVBT2_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = DVBT2_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = DVBT2_TunerDVBT2Command;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = DVBT2_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc    = DVBT2_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DVBT2_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DVBT2_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C = DVBT2_TunerBypassI2C;
#if defined(PD_REMAP_TER_DIG) || defined(PD_REMAP_CAB_DIG) || defined(PD_REMAP_SAT_DIG) || defined(PD_NO_USE_REMAP)
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute2 =  DVBT2_TunerGetAttribute2;
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
INT32 DVBT2_TunerOpen_c2(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    INT32 retCode = 0;
    ITUNER_CTX_T *psTunerCtx ;
    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=0xDA;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    //register LTDIS APIs to HAL
    PD_DVBT2_Register_LTDIS_Fct(ptTDCtx);

    //Check Demod inited
    if (fgDemodInited){
        mcDBG_MSG_L1(("CXD2854 had been initialized!\n"));
        return (DRVAPI_TUNER_OK);
    }
    fgDemodInited = TRUE;
    
    //create pdctx used in this file, you also can add any declaration you want in pdctx
    pPdCtxDVBT2 = (DVBT2_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBT2_PD_CTX_T));
    if (pPdCtxDVBT2 == NULL)
    {
        mcDBG_MSG_L1(("DVB_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    /* Init Disconnect Status */
    pPdCtxDVBT2->fgDisStatus = TRUE;

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtxDVBT2->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_L1(("DVB x_sema_create(pPdCtxDVBT2->hHalLock) Fail!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    x_sema_unlock(pPdCtxDVBT2->hHalLock);

    // demod instance initialization (to do only once)
    // NOTE: If Initialization is done from ISDB-T side, this is unnecessary.

    vResetDemod(20);
    retCode = sony_mtk_glue_common_Initialize();
    if (retCode != DRVAPI_TUNER_OK) {
        return retCode;
    }

    return (DRVAPI_TUNER_OK);
}

#ifdef CC_DEMOD_DVBS_LNB_SHORT_MONITOR
static void sony_cxd2854_Semalock(BOOL lock)
{
    if(fgDemodInited == FALSE)
    {
        return;
    }
    
    if(lock == 1)
    {
        mcSEMA_LOCK_MTFE(pPdCtxDVBT2->hHalLock);
    }
    else
    {
        mcSEMA_UNLOCK_MTFE(pPdCtxDVBT2->hHalLock);
    }
}
#endif // CC_DEMOD_DVBS_LNB_SHORT_MONITOR
