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

#ifndef _X_DRVAPI_TUNER_C_
#define _X_DRVAPI_TUNER_C_

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/

#include "PI_Def_dbg.h"
#include "t_rm.h"
#include "drvapi_tuner.h"
#include "x_rm_dev_types.h"
#include "x_drvapi_tuner.h" /* Antonio 02/14/05 */
#include "x_sys_name.h"
#include "UtilAPI.h"
#include "x_pinmux.h"
#include "nptv_drvif.h"
#include "pd_remap_glue.h"

#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif

//lei130925
#ifdef CC_DEMOD_FASTACQ
#include "eepdef.h"
#include "source_table.h"
void vApiEepromDmaRead(UINT16 wPrm, void *pDst, UINT16 wLen); 
void vApiEepromDmaWrite(UINT16 wPrm, void *pSrc, UINT16 wLen);
static void FaStateSet(STATE_INFO_T* ptStateInfo, TUNER_STATE_T x);
static void FaStateChange(STATE_INFO_T* ptStateInfo, TUNER_STATE_T os, TUNER_STATE_T ns);
#endif

extern int  snprintf(char *s, size_t n, const char * format, ...);

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    variables definitions
 ----------------------------------------------------------------------------*/
GENERIC_LIST_ENTRY      TunerCtxList;
BOOL                    TunerInited = 0;

/*---------------
*   Removing global variable bDetachMW & bTunerOpenFailure to TDMgr.c
*   to solve involving x_rm_reg_comp() in drvapi_tuner.c when doing driver build.
*   reference CR: DTV00009782
-----------------*/
//BOOL                    bDetachMW = 0;  /*remove to TDMgr.c, solve drvapi_tuner.c cal*/
//BOOL                    bTunerOpenFailure = 0; /*remove to TDMgr.c*/
extern BOOL             bDetachMW;
extern BOOL             bTunerOpenFailure;

#ifdef CC_SUPPORT_STR
extern STATE_INFO_T*           g_ptStateInfo;
#endif

THREAD_T                t_init_thread = (THREAD_T)NULL;
char                    init_thread_name[16];
SEMA_T                  t_init_sync = (SEMA_T)NULL;
BOOL                    TDD_Ready = FALSE;

UINT16                  u2AcqTime;

static UCHAR            fStayInConnectedState=0;
UINT32                  gInit_Start_Time;

#ifndef CC_DUAL_TUNER_SUPPORT //dual tuner do not need this 
MUTEX_T                 t_tuner_scan_lock = (MUTEX_T)NULL;
#endif

#if TUNER_DTV_NOTIFY_ENABLE
DRV_COMP_ID_T*  gPt_comp_id = NULL;/* Add the value for nofity MW when DTV Fine Tuner Freq */
#endif

#ifndef CC_DUAL_TUNER_SUPPORT
static UINT8    t_fe_status = TUNER_COND_STATUS_UNKNOWN;
#endif

#ifdef CC_DEMOD_FASTACQ
enum
{	DEMOD_FASTBOOT_ENABLE,
	DEMOD_FASTBOOT_TunerID,
	DEMOD_FASTBOOT_E_CONN_TYPE,
	DEMOD_FASTBOOT_Freq_0,
	DEMOD_FASTBOOT_Freq_1,
	DEMOD_FASTBOOT_Freq_2,
	DEMOD_FASTBOOT_E_MOD,
	DEMOD_FASTBOOT_PARAM_1,
	DEMOD_FASTBOOT_PARAM_2,
	DEMOD_FASTBOOT_PARAM_3,
	DEMOD_FASTBOOT_PARAM_4,
	#ifdef CC_ATD_SUPPORT_FASTACQ
	DEMOD_FASTBOOT_PARAM_5,
	DEMOD_FASTBOOT_PARAM_6,
	DEMOD_FASTBOOT_PARAM_7,
	DEMOD_FASTBOOT_PARAM_8,
	DEMOD_FASTBOOT_PARAM_9,
	DEMOD_FASTBOOT_PARAM_10,
	#endif
	#ifdef CC_DVBS_SUPPORT_FASTACQ
    TUNER_PRETUNE_SAT_CONN_CONFIG,
    TUNER_PRETUNE_SAT_DISEQC_PARAM_1,//DISEQC_CONTROL_MASK,TRANSPONDER_FREQ_1
    TUNER_PRETUNE_SAT_DISEQC_PARAM_2,//DISEQC_CMD,TRANSPONDER_FREQ_2
    TUNER_PRETUNE_SAT_DISEQC_PARAM_3,//LNB_FREQ_1
    TUNER_PRETUNE_SAT_DISEQC_PARAM_4,//LNB_FREQ_2
    TUNER_PRETUNE_SAT_DISEQC_PARAM_5,//USER_BAND and BANK
	#endif
	DEMOD_FASTBOOT_NUM
};

static UINT8 VAR_FASTBOOT_NUM = 0;
static BOOL FgBootupWithChannel = FALSE;
static BOOL u1FastAcqMode = TRUE;
static UINT32 FastAcq_ui4_freq = 666000;//KHz
#ifdef CC_ATD_SUPPORT_FASTACQ
static UINT32 FastAcq_ui4_freq_bound = 666000;//KHz
static UINT32 FastAcq_ui4_tv_sys_mask = 0;//KHz
#endif
static UINT16 FastAcq_symbol_rate = 6875;

static UINT8 FA_W_Conn_Infor[DEMOD_FASTBOOT_NUM];
#endif
#ifdef CC_DVBS_SUPPORT_FASTACQ
FASTACQ_STATE_INFO_T   t_PreTune_Info; 
static BOOL FastAcq_CheckDiseqccmd(DISEQC_BUS_CMD_T *pCmd);
#endif

TUNERHAL_NTY fptIniThreadNfy = NULL;
TUNERHAL_NTY1 fptTunerProcNfy = NULL;

#ifdef CC_DUAL_TUNER_SUPPORT
#define MAX_MUTUAL_EXCL_COMP_REG_NUM            10
#else
#define MAX_MUTUAL_EXCL_COMP_REG_NUM            8
#endif
static DRV_COMP_REG_T   MutualExclCompReg_T[MAX_MUTUAL_EXCL_COMP_REG_NUM];

DRV_COMP_FCT_TBL_T t_tuner_fct_tbl =
{
    TunerConnect,
    TunerDisconnect,
    TunerGet,
    TunerSet
};

/*-----------------------------------------------------------------------------
                    functions definitions
 ----------------------------------------------------------------------------*/
S32 GetTunerCtx(DRV_COMP_ID_T* pt_comp_id, TUNER_CTX_T** pptTunerCtx,
                STATE_INFO_T** pptStateInfo)
{
    TUNER_CTX_T*    ptTunerCtx;
    STATE_INFO_T*   ptStateInfo=NULL;
	if(NULL==pt_comp_id)//d120104_Haibo:To avoid linux kernel crash due to NULL pointer parameter was transferred
	{
	  mcSHOW_DBG_MSG(("[%s]comp_id is NULL\n",__FUNCTION__));
	  return (DRVAPI_TUNER_ERROR);  
	}
    ptTunerCtx = (TUNER_CTX_T*)QueueGetHead(&TunerCtxList);
    // Find the corresponding TunerCtx
    while (ptTunerCtx)
    {
        if (ptTunerCtx->TunerID == pt_comp_id->ui2_id)
        {
            // The corresponding TunerCtx exists.
            break;
        }
        ptTunerCtx = (TUNER_CTX_T*)QueueGetNext(&TunerCtxList, &ptTunerCtx->Link);
    }
    *pptTunerCtx = ptTunerCtx;

    // Find the corresponding StateInfo
    if (ptTunerCtx)
    {
        ptStateInfo = (STATE_INFO_T*)QueueGetHead(&ptTunerCtx->StateInfoList);
        while(ptStateInfo)
        {
            if (ptStateInfo->t_tuner_comp_id.e_type == pt_comp_id->e_type)
            {
                // The corresponding StateInfo exists.
                break;
            }
            ptStateInfo = (STATE_INFO_T*)QueueGetNext(&ptTunerCtx->StateInfoList, &ptStateInfo->Link);
        }
    }
    *pptStateInfo = ptStateInfo;

    if (ptTunerCtx && ptStateInfo)
    {
        return (DRVAPI_TUNER_OK);
    }
    else
    {
        return (DRVAPI_TUNER_ERROR);
    }
}

void TunerStayInConnectedState(UCHAR sw)
{
    fStayInConnectedState = sw;
}

#ifdef CC_MT5371
//---------------------------------------------------------------------
/** _SetResetPeripheralPin()
 *  @param i4Val 0 to reset, 1 to back.
 */
//---------------------------------------------------------------------
static void _SetResetPeripheralPin(INT32 i4Val)
{
    GPIO_Output(0, &i4Val);
}

//-----------------------------------------------------------------------------
/** _BspSerIsMT5371() to check it's MT5371 UART or not.
 *  @retval TRUE    It's running on MT5371 UART.
 *  @retval FALSE   It's not running on MT5371 UART, assume it's MT5351.
 */
//-----------------------------------------------------------------------------
static BOOL _fgIsMT5371 = TRUE;
#define TEST_REGEST 0x2000c110
static BOOL _BspSerIsMT5371()
{
    UINT32 u4BufCtlReg;
    static BOOL   fgInit = FALSE;
    volatile UINT32* pu4TestReg = (volatile UINT32*)TEST_REGEST;

    if (!fgInit) // Do the MT5371 register test and update result to _fgIsMT5371
    {
        u4BufCtlReg = *pu4TestReg;

        // Write pattern to the register
        *pu4TestReg = (*pu4TestReg & 0xffff00ff) | 0xAA00;

        // Read back and do checking
        if ((*pu4TestReg & 0x0000ff00) == 0xAA00)
        {
            _fgIsMT5371 = FALSE;
        }

        // Restore the original setting
        *pu4TestReg = u4BufCtlReg;

        fgInit = TRUE;
    }

    return _fgIsMT5371;
}

void vHwResetDemod(void)
{
    if (_BspSerIsMT5371())
    {
        mcSHOW_USER_CRI_MSG(("#### Reset Tuner ####\n"));
        // Start the peripheral reseting.
        _SetResetPeripheralPin(0);
        // delay 100 ms.
        mcDELAY_MS(100);
        // Finish the peripheral reseting.
        _SetResetPeripheralPin(1);
    }
}
#endif

void InitThreadProc(VOID *pvArg)
{
	#ifdef CC_DEMOD_FASTACQ
	BOOL TVSource = FALSE;
	#endif

    mcSHOW_DRVAPI_MSG(("InitThreadProc is running!\n"));

	#ifdef CC_DEMOD_FASTACQ
	TVSource= TunerFastCheckSource();//Lei20131018 get the Input source information ASAP.
	#endif

    // Check if Tuner Device Drivers are ready
    TunerSetMwIntf(cTURNKEY_IF);
    if (TunerOpen() < DRVAPI_TUNER_OK)
    {
#ifdef CC_MT5371
        mcSHOW_USER_CRI_MSG(("#### TunerOpen failed, please check power module and I2C bus! ####\n"));
        vHwResetDemod();
        if (_fgIsMT5371)
        {
            mcSHOW_USER_CRI_MSG(("#### Re-open Again ####\n"));
            if (TunerOpen() < (S32)DRVAPI_TUNER_OK)
            {
                mcSHOW_USER_CRI_MSG(("#### TunerOpen failed, please check power module and I2C bus! ####\n"));
                mcSHOW_USER_CRI_MSG(("#### Enter Dummy mode ####\n"));
                bTunerOpenFailure = TRUE;
            }
        }
        else
        {
            mcSHOW_USER_CRI_MSG(("#### Enter Dummy mode ####\n"));
            bTunerOpenFailure = TRUE;
        }
#else
        mcSHOW_USER_CRI_MSG(("#### Enter Dummy mode ####\n"));
        bTunerOpenFailure = TRUE;
#endif
    }

	#ifdef CC_DEMOD_FASTACQ
	if(TVSource == TRUE)//TV source
	{
		if(TunerFastAcq()!=TRUE)
			mcSHOW_USER_CRI_MSG(("#### TunerFastAcq Fail  ####\n"));
	}
	#endif

    TDD_Ready = TRUE;
    mcSEMA_UNLOCK(t_init_sync);
    t_init_thread = (THREAD_T)NULL;
    mcSHOW_DRVAPI_MSG(("InitThreadProc exit!\n"));
    mcSHOW_DRVAPI_MSG(("InitThreadProc time = %d ms\n",
            (INT16)((os_get_sys_tick()-gInit_Start_Time)*x_os_drv_get_tick_period())));

    mcTHREAD_DESTROY();
}

/*
This function convert modulation type.
*/
TUNER_MODULATION_T TunerMod(S32 qam_size)
{
    TUNER_MODULATION_T tuner_mod;

    switch (qam_size)
    {
        case 4:
            tuner_mod = MOD_QPSK;
            break;
        case 5:
            tuner_mod = MOD_QAM_4_NR;
            break;
        case 8:
            tuner_mod = MOD_VSB_8;
            break;
        case 16:
            tuner_mod = MOD_QAM_16;
            break;
        case 32:
            tuner_mod = MOD_QAM_32;
            break;
        case 64:
            tuner_mod = MOD_QAM_64;
            break;
        case 128:
            tuner_mod = MOD_QAM_128;
            break;
        case 256:
            tuner_mod = MOD_QAM_256;
            break;
        case 999:
            tuner_mod = MOD_UNKNOWN;/*auto QAM*/
            break;
        default:
            tuner_mod = MOD_QAM_64;
            break;
    }
    return (tuner_mod);
}

/***********************************************************************/
/*
This function provide tuner break condition for acquisition.
*/
 S32 TunerBreak(void* pArg)
{
    DRV_COMP_ID_T*      pt_comp_id;
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;
    
    //TunerAcq in TUNER_STATE_CONNECTED state. - mtk40109 2009-07-21
    //Return from NIM to MW.
    if (!pArg)
    {
        return (FALSE);
    }
    
    pt_comp_id = (DRV_COMP_ID_T*)pArg;
    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
    {
        return (TRUE);
    }
    if (ptStateInfo == NULL)
    {
        return (TRUE);
    }

    if (ptStateInfo->e_tuner_state != TUNER_STATE_CONNECTING)
    {
        mcSHOW_DRVAPI_MSG(("\nTunerBreak: @state(%d)\n", ptStateInfo->e_tuner_state));
        return (TRUE);
    }
    
    return (FALSE);
}

/*
This function set tuner state unconditionally.
*/
static void TunerStateSet(STATE_INFO_T* ptStateInfo, TUNER_STATE_T x)
{
    mcMUTEX_LOCK(ptStateInfo->t_tuner_state_lock);
    ptStateInfo->e_tuner_state = x;
    mcMUTEX_UNLOCK(ptStateInfo->t_tuner_state_lock);
    mcSHOW_DRVAPI_MSG(("%s TunerStateSet: %d\n", ptStateInfo->comp_name, x));
}

/*
This function change tuner state conditionally.
*/
static void TunerStateChange(STATE_INFO_T* ptStateInfo, TUNER_STATE_T os, TUNER_STATE_T ns)
{
    mcMUTEX_LOCK(ptStateInfo->t_tuner_state_lock);
    if (ptStateInfo->e_tuner_state == os)
    {
        ptStateInfo->e_tuner_state = ns;
        mcMUTEX_UNLOCK(ptStateInfo->t_tuner_state_lock);
        mcSHOW_DRVAPI_MSG(("%s TunerStateChang: %d -> %d\n",ptStateInfo->comp_name, os, ns));
    }
    else
    {
        mcMUTEX_UNLOCK(ptStateInfo->t_tuner_state_lock);
    }
}

VOID pf_dummy_nfy (DRV_COMP_ID_T*  pt_comp_id,
                   DRV_COND_T      e_nfy_cond,
                   VOID*           pv_tag,
                   UINT32          ui4_data)
{
    mcSHOW_DRVAPI_MSG(("pf_dummy_nfy is called\n"));
    return;
}

/*
This function implements component connect function.
*/
//static INT32 TunerConnect(DRV_COMP_ID_T*   pt_comp_id,
INT32 TunerConnect(DRV_COMP_ID_T*   pt_comp_id,
			     DRV_CONN_TYPE_T  e_conn_type,
                          const VOID*      pv_conn_info,
                          SIZE_T           z_conn_info_len,
                          VOID*            pv_tag,
                          x_rm_nfy_fct     pf_nfy)
{
    TUNER_CTX_T*            ptTunerCtx;
    STATE_INFO_T*           ptStateInfo;
    UINT32                  Freq=0;
    UINT32                  SymbolRate=0;
    TUNER_MODULATION_T      e_mod=MOD_UNKNOWN;
    BOOL                    SpecInv=0;
#ifdef CC_DVBS_SUPPORT_FASTACQ
    FASTACQ_STATE_INFO_T*	pt_PreTune_Info = &t_PreTune_Info;
    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;
    UINT8 ui1_temp;
#endif
    mcSHOW_DRVAPI_MSG(("[MWCmd]TunerConnect()\n"));
#ifdef CC_SUPPORT_STR
	if(TRUE == Get_STR_Mode())
		{
			mcSHOW_USER_CRI_MSG(("[ERROR]Tuner is in the Resuming Process, TunerConncet will be show no signal\n"));
		}
#endif
    if (bDetachMW)
    {
        mcSHOW_DRVAPI_MSG(("TunerDriver Detach MW\n"));
        return (RMR_OK);
    }

    // Service will be provided ONLY when tuner is opened successfully.
    if (!TDD_Ready)
    {
        mcSEMA_LOCK(t_init_sync);
        mcSEMA_UNLOCK(t_init_sync);
    }

    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    if (ptStateInfo == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    if (ptStateInfo->e_conn_type != e_conn_type)
    {
        mcSHOW_DRVERR_MSG(("TunerConnect: RMR_DRV_INV_CONN_INFO!\n"));
        return (RMR_DRV_INV_CONN_INFO);
    }
    
    /* verify the request */
    if ((pt_comp_id->e_type != DRVT_TUNER_CAB_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_DIG_OOB_RX) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_DIG)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
        mcSHOW_DRVERR_MSG(("TunerConnect: RMR_DRV_INV_CONN_INFO!\n"));
        return (RMR_DRV_INV_CONN_INFO);
    }
    
    if ((pf_nfy == NULL) && (!ptStateInfo->b_disable_nfy))
    {
        mcSHOW_DRVERR_MSG(("TunerConnect: RMR_DRV_INV_CONN_INFO!\n"));
        return (RMR_DRV_INV_CONN_INFO);
    }
    
    /* verify connect info */
    if (pv_conn_info == NULL)
    {
        mcSHOW_DRVERR_MSG(("TunerConnect: RMR_DRV_INV_CONN_INFO!\n"));
        return (RMR_DRV_INV_CONN_INFO);
    }

#ifdef CC_SUPPORT_STR
    g_ptStateInfo = ptStateInfo;
#endif

#ifndef CC_DUAL_TUNER_SUPPORT
    t_fe_status = TUNER_COND_STATUS_UNKNOWN;
#else
    ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_UNKNOWN;
#endif

    switch (e_conn_type)
    {
        case TUNER_CONN_TYPE_TER_DIG:
            ptStateInfo->b_disable_nfy = ((TUNER_TER_DIG_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_CAB_DIG:
            ptStateInfo->b_disable_nfy = ((TUNER_CAB_DIG_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_SAT_DIG:
            ptStateInfo->b_disable_nfy = ((TUNER_SAT_DIG_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_CAB_DIG_OOB_RX:
            ptStateInfo->b_disable_nfy = ((TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_TER_ANA:
            ptStateInfo->b_disable_nfy = ((TUNER_TER_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_CAB_ANA:
            ptStateInfo->b_disable_nfy = ((TUNER_CAB_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_SAT_ANA:
            ptStateInfo->b_disable_nfy = ((TUNER_SAT_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_CAB_ANA_SCART_OUT:
            ptStateInfo->b_disable_nfy = ((TUNER_CAB_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_TER_ANA_SCART_OUT:
            ptStateInfo->b_disable_nfy = ((TUNER_CAB_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        case TUNER_CONN_TYPE_SAT_ANA_SCART_OUT:
            ptStateInfo->b_disable_nfy = ((TUNER_SAT_ANA_TUNE_INFO_T*)pv_conn_info)->b_no_disc_nfy;
            break;
        default:
            mcSHOW_DRVERR_MSG(("ERROR: Unknown e_conn_type\n"));
            break;
    }

    if ((pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
        if (Util_TranslateConnectInfo(e_conn_type, (VOID*)pv_conn_info, z_conn_info_len,
                    &Freq, &SymbolRate, &e_mod, &SpecInv)!=RMR_OK)
        {
            return (RMR_DRV_INV_CONN_INFO);
        }

        /* Don't set tuner if RF freq not changed */
		mcSHOW_DRVERR_MSG(("ptStateInfo->e_tuner_state = %d\n", ptStateInfo->e_tuner_state));
		mcSHOW_DRVERR_MSG(("TunerGetSync(ptTunerCtx->TunerID, e_conn_type) = %d\n", TunerGetSync(ptTunerCtx->TunerID, e_conn_type)));
		mcSHOW_DRVERR_MSG(("ptStateInfo->t_tuner_info.ui4_freq = %d, Freq = %d\n", ptStateInfo->t_tuner_info.ui4_freq, Freq));
		mcSHOW_DRVERR_MSG(("ptStateInfo->t_tuner_info.ui4_sym_rate = %d, SymbolRate = %d\n", ptStateInfo->t_tuner_info.ui4_sym_rate, SymbolRate));
		mcSHOW_DRVERR_MSG(("ptStateInfo->t_tuner_info.e_mod = %d, e_mod = %d\n", ptStateInfo->t_tuner_info.e_mod, e_mod));
		mcSHOW_DRVERR_MSG(("ptStateInfo->e_conn_type = %d\n", ptStateInfo->e_conn_type));
		mcSHOW_DRVERR_MSG(("ptStateInfo->OobSpecInv = %d, SpecInv = %d\n", ptStateInfo->OobSpecInv, SpecInv));
		
        if ((ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED) &&
            (TunerGetSync(ptTunerCtx->TunerID, e_conn_type)) &&
            (ptStateInfo->t_tuner_info.ui4_freq == Freq) &&
            (ptStateInfo->t_tuner_info.ui4_sym_rate == SymbolRate) &&
            (ptStateInfo->t_tuner_info.e_mod == e_mod))
        {
            ptStateInfo->pf_tuner_nfy = pf_nfy;
            ptStateInfo->pv_tuner_nfy_tag = pv_tag;
            //++ Save conn_info
            x_memcpy(&ptStateInfo->t_conn_info, pv_conn_info, z_conn_info_len);
            ptStateInfo->z_conn_info_len = z_conn_info_len;
            //--
            TunerStateSet(ptStateInfo, TUNER_STATE_CONNECTED);
			//lei131016 fix CR DTV00554282
			#ifdef CC_DUAL_TUNER_SUPPORT
			ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_GOT_SIGNAL;
			#else
			t_fe_status = TUNER_COND_STATUS_GOT_SIGNAL;
			#endif
			#ifdef CC_DEMOD_FASTACQ
			FaStateSet(ptStateInfo, TUNER_STATE_CONNECTED);
			#endif
			
            /* always report connected */
            if (pf_nfy != NULL) // Klocwork error (NPD.CHECK.MIGHT)
            {
                pf_nfy(&ptStateInfo->t_tuner_comp_id,
                    DRV_COND_CONNECTED,
                    pv_tag,
                    RM_CONN_REASON_AS_REQUESTED);
                pf_nfy(&ptStateInfo->t_tuner_comp_id,
                    DRV_COND_STATUS,
                    pv_tag,
                    TUNER_COND_STATUS_GOT_SIGNAL);
            }
            return (RMR_OK);
        }
    }
    
    if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTING)
    {
        // According to Driver-Tuner document,
        // (Sec: 2.3.2  Multiple connect requests)
        // "It is important to note that when connection requests are
        // discarded the tuner driver does not have to notify the
        // Middleware layer."
        ptStateInfo->pf_tuner_nfy = (x_rm_nfy_fct)pf_dummy_nfy;
    }

    /* signal TunerProcess to be TUNER_STATE_DISCONNECTED */
    TunerStateSet(ptStateInfo, TUNER_STATE_DISCONNECTED);
    mcSEMA_UNLOCK(ptStateInfo->t_tuner_sync);
    mcSHOW_DRVLOCK_MSG(("\t====== .LOCK_disconnected\n"));
    mcSEMA_LOCK(ptStateInfo->t_tuner_disconnected);
    mcSHOW_DRVLOCK_MSG(("\t====== .LOCK2_disconnected\n"));
	#ifdef CC_DEMOD_FASTACQ
	FaStateSet(ptStateInfo, TUNER_STATE_DISCONNECTED);
    mcSEMA_UNLOCK(ptStateInfo->t_fa_sync);
    mcSEMA_LOCK(ptStateInfo->t_fa_disconnected);
	#endif

    // TunerProcess is ready to process this new request
    ptStateInfo->pf_tuner_nfy = pf_nfy;
    ptStateInfo->pv_tuner_nfy_tag = pv_tag;
    //++ Save conn_info
    x_memcpy(&ptStateInfo->t_conn_info, pv_conn_info, z_conn_info_len);
    ptStateInfo->z_conn_info_len = z_conn_info_len;
    //--

    if ((pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
//#ifndef DTMB_MANUAL_DEMOD_DETECT
        /* change tuner parameters */
        ptStateInfo->t_tuner_info.ui4_freq     = Freq;
        ptStateInfo->t_tuner_info.ui4_sym_rate = SymbolRate;
        ptStateInfo->t_tuner_info.e_mod        = e_mod;
        ptStateInfo->OobSpecInv = SpecInv;
//#else

//#endif
    }

	#ifdef CC_DEMOD_FASTACQ
	
		FA_W_Conn_Infor[DEMOD_FASTBOOT_TunerID]     = mcLOW_BYTE(ptTunerCtx->TunerID);
		FA_W_Conn_Infor[DEMOD_FASTBOOT_E_CONN_TYPE] = ptStateInfo->e_conn_type- TUNER_CONN_TYPE(0);
		
		switch(ptStateInfo->e_conn_type)
		{
		    case TUNER_CONN_TYPE_TER_DIG:
				FastAcq_ui4_freq = ptStateInfo->t_conn_info.t_ter_tuner_info.ui4_freq/1000;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_2]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq>>16));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_1]     = mcHIGH_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_0]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_E_MOD]      = ptStateInfo->t_conn_info.t_ter_tuner_info.e_mod;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_1]    = ptStateInfo->t_conn_info.t_ter_tuner_info.e_bandwidth;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_2]    = ptStateInfo->t_conn_info.t_ter_tuner_info.e_hierarchy_priority;			
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_3]    = ptStateInfo->t_conn_info.t_ter_tuner_info.e_conn_mode;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_4]    = ptStateInfo->t_conn_info.t_ter_tuner_info.ui1_plp_id;
				VAR_FASTBOOT_NUM = 11;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0xA5;//enable
	            break;
	        case TUNER_CONN_TYPE_CAB_DIG:
				FastAcq_ui4_freq = ptStateInfo->t_conn_info.t_cab_tuner_info.ui4_freq/1000;
				FastAcq_symbol_rate = ptStateInfo->t_conn_info.t_cab_tuner_info.ui4_sym_rate/1000;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_2]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq>>16));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_1]     = mcHIGH_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_0]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
				FA_W_Conn_Infor[DEMOD_FASTBOOT_E_MOD]      = ptStateInfo->t_conn_info.t_cab_tuner_info.e_mod;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_1]    = mcLOW_BYTE(FastAcq_symbol_rate);
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_2]    = mcHIGH_BYTE(FastAcq_symbol_rate);
				FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_3]    = ptStateInfo->t_conn_info.t_cab_tuner_info.e_conn_mode;			
				VAR_FASTBOOT_NUM = 10;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0xA5;//enable
	            break;
            #ifdef CC_DVBS_SUPPORT_FASTACQ
			case TUNER_CONN_TYPE_SAT_DIG:
				pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;				

				FastAcq_ui4_freq = pt_tuner_info->ui4_freq/1000;
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_2]	   = mcLOW_BYTE((U16)(FastAcq_ui4_freq>>16));
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_1]	   = mcHIGH_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_0]	   = mcLOW_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_E_MOD]	   = pt_tuner_info->e_mod;
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_1]    = pt_tuner_info->e_conn_mode;
                FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_2]    = mcGET_FIELD(pt_tuner_info->ui4_data, mcMASK(8), 0);
                FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_3]    = mcGET_FIELD(pt_tuner_info->ui4_data, mcMASK(16), 8);
               if(pt_PreTune_Info->t_diseqc_cmd.ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK)
               {
                //CONN_CONFIG
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_CONN_CONFIG] = 0x20;
                //TRANSPONDER_FREQ_1
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_1] = mcGET_FIELD(pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui4_transponder_freq, mcMASK(8), 0);
                //TRANSPONDER_FREQ_2
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_2] = mcGET_FIELD(pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui4_transponder_freq, mcMASK(16), 8);
                //LNB_FREQ_1
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_3] = mcGET_FIELD(pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_lnb_freq, mcMASK(8), 0);
                //LNB_FREQ_2
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_4] = mcGET_FIELD(pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_lnb_freq, mcMASK(16), 8);
                /*  Addr                  Bit                Name
                         * -------------------------------------------------------
                         *  CONN_CONFIG   [3:0]             e_bank
                         *  CONN_CONFIG   [7:4]             e_user_band
                         */
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_5] = (pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_user_band<<4) || (pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_bank);
				VAR_FASTBOOT_NUM = 15;

			   }
              else
               {
                //DISEQC_CONTROL_MASK
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_1] = pt_PreTune_Info->t_diseqc_cmd.ui4_control_mask;
                //DISEQC_CMD
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_2] = pt_PreTune_Info->t_diseqc_cmd.aui1_cmd[3];
                //CONN_CONFIG
                /*  Addr                  Bit                Name
                         * -------------------------------------------------------
                         *  CONN_CONFIG   [1:0]          ui1_lnb_power
                         *  CONN_CONFIG   [2]             ui1_polarity_13v_18v
                         *  CONN_CONFIG   [3]             b_22k
                         *  CONN_CONFIG   [4]             ui1_tone_burst
                         *  CONN_CONFIG   [5]             e_conn_mode
                         *  CONN_CONFIG   [6]             b_canal_digital_ts
                         */
                ui1_temp = pt_PreTune_Info->t_diseqc_cmd.ui1_lnb_power;
                ui1_temp |= (pt_PreTune_Info->t_diseqc_cmd.ui1_polarity_13v_18v << 2);
                ui1_temp |= (pt_PreTune_Info->t_diseqc_cmd.b_22k << 3);
                ui1_temp |= (pt_PreTune_Info->t_diseqc_cmd.ui1_tone_burst << 4);
                ui1_temp |= (pt_tuner_info->b_canal_digital_ts << 6);
                FA_W_Conn_Infor[TUNER_PRETUNE_SAT_CONN_CONFIG] = ui1_temp;
				VAR_FASTBOOT_NUM = 12;
               }		    
			    FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0xA5;//enable	
			    break;
            #endif
			
			#ifdef CC_ATD_SUPPORT_FASTACQ
		        case TUNER_CONN_TYPE_TER_ANA:
				case TUNER_CONN_TYPE_TER_ANA_SCART_OUT:
					FastAcq_ui4_freq = ptStateInfo->t_conn_info.t_tana_tuner_info.ui4_freq/1000;
					FastAcq_ui4_freq_bound = ptStateInfo->t_conn_info.t_tana_tuner_info.ui4_freq_bound/1000;
					FastAcq_ui4_tv_sys_mask = ptStateInfo->t_conn_info.t_tana_tuner_info.ui4_tv_sys_mask;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_2]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_1]     = mcHIGH_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_0]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_E_MOD]      = ptStateInfo->t_conn_info.t_tana_tuner_info.e_mod;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_1]    = ptStateInfo->t_conn_info.t_tana_tuner_info.b_fine_tune;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_2]    = mcHIGH_BYTE(ptStateInfo->t_conn_info.t_tana_tuner_info.i2_fine_tune_offset);
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_3]    = mcLOW_BYTE(ptStateInfo->t_conn_info.t_tana_tuner_info.i2_fine_tune_offset);
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_4]    = mcLOW_BYTE((U16)(FastAcq_ui4_freq_bound>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_5]    = mcHIGH_BYTE((U16)(FastAcq_ui4_freq_bound & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_6]    = mcLOW_BYTE((U16)(FastAcq_ui4_freq_bound & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_7]    = mcHIGH_BYTE((U16)(FastAcq_ui4_tv_sys_mask>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_8]    = mcLOW_BYTE((U16)(FastAcq_ui4_tv_sys_mask>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_9]    = mcHIGH_BYTE((U16)(FastAcq_ui4_tv_sys_mask & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_10]   = mcLOW_BYTE((U16)(FastAcq_ui4_tv_sys_mask & 0xFFFF));
					VAR_FASTBOOT_NUM = 17;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0xA5;//enable
					break;
		        case TUNER_CONN_TYPE_CAB_ANA:
					FastAcq_ui4_freq = ptStateInfo->t_conn_info.t_cana_tuner_info.ui4_freq/1000;
					FastAcq_ui4_freq_bound = ptStateInfo->t_conn_info.t_cana_tuner_info.ui4_freq_bound/1000;
					FastAcq_ui4_tv_sys_mask = ptStateInfo->t_conn_info.t_cana_tuner_info.ui4_tv_sys_mask;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_2]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_1]     = mcHIGH_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_Freq_0]     = mcLOW_BYTE((U16)(FastAcq_ui4_freq & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_E_MOD]      = ptStateInfo->t_conn_info.t_cana_tuner_info.e_mod;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_1]    = ptStateInfo->t_conn_info.t_cana_tuner_info.b_fine_tune;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_2]    = mcHIGH_BYTE(ptStateInfo->t_conn_info.t_cana_tuner_info.i2_fine_tune_offset);
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_3]    = mcLOW_BYTE(ptStateInfo->t_conn_info.t_cana_tuner_info.i2_fine_tune_offset);
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_4]    = mcLOW_BYTE((U16)(FastAcq_ui4_freq_bound>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_5]    = mcHIGH_BYTE((U16)(FastAcq_ui4_freq_bound & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_6]    = mcLOW_BYTE((U16)(FastAcq_ui4_freq_bound & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_7]    = mcHIGH_BYTE((U16)(FastAcq_ui4_tv_sys_mask>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_8]    = mcLOW_BYTE((U16)(FastAcq_ui4_tv_sys_mask>>16));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_9]    = mcHIGH_BYTE((U16)(FastAcq_ui4_tv_sys_mask & 0xFFFF));
					FA_W_Conn_Infor[DEMOD_FASTBOOT_PARAM_10]   = mcLOW_BYTE((U16)(FastAcq_ui4_tv_sys_mask & 0xFFFF));
					VAR_FASTBOOT_NUM = 17;
					FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0xA5;//enable
	            	break;
#endif

	        //case TUNER_CONN_TYPE_SAT_DIG:
	        //case TUNER_CONN_TYPE_CAB_DIG_OOB_RX:
			//case TUNER_CONN_TYPE_SAT_ANA:
	        //case TUNER_CONN_TYPE_CAB_ANA_SCART_OUT:	
	        //case TUNER_CONN_TYPE_SAT_ANA_SCART_OUT:
	        default:
				VAR_FASTBOOT_NUM = 1;
				FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0x00;//disable
	            mcSHOW_DRVAPIOK_MSG(("ERROR: Unknown e_conn_type\n"));
	            break;
				
    }
	#endif

    /* signal tuner to start a new acquisition */
    TunerStateSet(ptStateInfo, TUNER_STATE_CONNECTING);
    mcSHOW_DRVLOCK_MSG(("\t====== .UNLOCK_connecting\n"));
    mcSEMA_UNLOCK(ptStateInfo->t_tuner_connecting);

	#ifdef CC_DEMOD_FASTACQ
    FaStateSet(ptStateInfo, TUNER_STATE_CONNECTING);
    mcSEMA_UNLOCK(ptStateInfo->t_fa_connecting);
	#endif
	
    mcSHOW_DRVAPIOK_MSG(("TunerConnect: RMR_OK!\n"));
    return (RMR_ASYNC_NFY);
}

/*
This function implements component disconnect function.
*/
//static INT32 TunerDisconnect(DRV_COMP_ID_T*   pt_comp_id,
INT32 TunerDisconnect(DRV_COMP_ID_T*   pt_comp_id,
                             DRV_DISC_TYPE_T  e_disc_type,
                             const VOID*      pv_disc_info,
                             SIZE_T           z_disc_info_len)
{
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;

    mcSHOW_DRVAPI_MSG(("[MWCmd]TunerDisconnect()\n"));
    if (bDetachMW)
    {
        mcSHOW_DRVAPI_MSG(("TunerDriver Detach MW\n"));
        return (RMR_OK);
    }
    
    // Service will be provided ONLY when tuner is opened successfully.
    if (!TDD_Ready)
    {
        mcSEMA_LOCK(t_init_sync);
        mcSEMA_UNLOCK(t_init_sync);
    }

    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    if (ptStateInfo == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    /* verify the request */
    if (((pt_comp_id->e_type != DRVT_TUNER_CAB_DIG) &&
         (pt_comp_id->e_type != DRVT_TUNER_CAB_DIG_OOB_RX) &&
         (pt_comp_id->e_type != DRVT_TUNER_TER_DIG) &&
         (pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
         (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
         (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&     
         (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
         (pt_comp_id->e_type != DRVT_TUNER_SAT_DIG)&&
         (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA)&&
         (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
         ))
    {
        mcSHOW_DRVERR_MSG(("TunerDisconnect: RMR_DRV_INV_DISC_INFO!\n"));
        return (RMR_DRV_INV_DISC_INFO);
    }

    if (ptStateInfo->e_tuner_state != TUNER_STATE_DISCONNECTED)
    { /* change tuner state */
        TunerStateSet(ptStateInfo, TUNER_STATE_DISCONNECTING);
		#if 0
		//Lei120927 for Tuner_disconnect time unstable performance issue;
		mcSEMA_UNLOCK(ptStateInfo->t_tuner_sync);
    mcSEMA_LOCK(ptStateInfo->t_tuner_disconnecting); /*bug fix CR: DTV00139714*/
		#else
		//Lei140109 Fix CR:DTV00573027
		mcSEMA_LOCK(ptStateInfo->t_tuner_disconnecting); /*bug fix CR: DTV00139714*/
		mcSEMA_UNLOCK(ptStateInfo->t_tuner_sync);
		#endif

    		
		#ifdef CC_DEMOD_FASTACQ
		VAR_FASTBOOT_NUM = 1;
		FA_W_Conn_Infor[DEMOD_FASTBOOT_ENABLE]	   = 0x00;//disable
		FaStateSet(ptStateInfo, TUNER_STATE_DISCONNECTING);
		mcSEMA_UNLOCK(ptStateInfo->t_fa_sync);
        mcSEMA_LOCK(ptStateInfo->t_fa_disconnecting);
		#endif

    }
    else
    { /*MW wanna do TunerDisconnect even tuner sate is disconnect*/
        TunerDisc(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
    }

    mcSHOW_DRVAPIOK_MSG(("TunerDisconnect: RMR_OK!\n"));
    return (RMR_OK); /*bug fix CR: DTV00139714*/
}

/*
This function implements component get function.
*/
//static INT32 TunerGet(DRV_COMP_ID_T*  pt_comp_id,
INT32 TunerGet(DRV_COMP_ID_T*  pt_comp_id,
                      DRV_GET_TYPE_T  e_get_type,
                      VOID*           pv_get_info,
                      SIZE_T*         pz_get_info_len)
{
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;
	

    mcSHOW_DRVAPI_MSG(("[MWCmd]TunerGet()\n"));
    if (bDetachMW)
    {
        mcSHOW_DRVAPI_MSG(("TunerDriver Detach MW\n"));
        return (RMR_OK);
    }

    // Service will be provided ONLY when tuner is opened successfully.
    if (!TDD_Ready)
    {
        mcSEMA_LOCK(t_init_sync);
        mcSEMA_UNLOCK(t_init_sync);
    }

    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    if (ptStateInfo == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    /* verify the request */
    if ((pt_comp_id->e_type != DRVT_TUNER_CAB_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_DIG_OOB_RX) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_DIG)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
        mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_INV_GET_INFO!\n"));
        return (RMR_DRV_INV_GET_INFO);
    }

#ifdef CC_SUPPORT_STR
    g_ptStateInfo = ptStateInfo;
#endif

    /* supported types */
    switch (e_get_type)
    {
        case TUNER_GET_TYPE_TER_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_TER_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_TER_ANA_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_TER_ANA,
                                    (SIGNAL*)pv_get_info);
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_CAB_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_CAB_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_CAB_ANA_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_ANA, (SIGNAL*)pv_get_info);
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SAT_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_SAT_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_SAT_ANA_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_SAT_ANA, (SIGNAL*)pv_get_info);
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_TER_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_TER_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_TER_DIG_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *) pv_get_info;

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_TER_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
					pt_tuner_info->e_dvb_t_t2_mode	= t_signal.u1DVBT_T2_indicator;  //add mode get!!
					if (pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_T2)
						pt_tuner_info->ui1_plp_id = t_signal.ui1_plp_id;   //update plp id.

					mcSHOW_DRVAPI_MSG(("[MWInfo] pt_tuner_info->ui4_freq = %d\n", pt_tuner_info->ui4_freq));
					mcSHOW_DRVAPI_MSG(("[MWInfo] pt_tuner_info->e_mod = %d\n", pt_tuner_info->e_mod));
					mcSHOW_DRVAPI_MSG(("[MWInfo] pt_tuner_info->e_dvb_t_t2_mode = %d\n", pt_tuner_info->e_dvb_t_t2_mode));
					mcSHOW_DRVAPI_MSG(("[MWInfo] pt_tuner_info->ui1_plp_id = %d\n", pt_tuner_info->ui1_plp_id));
					
						
#ifndef DTMB_MANUAL_DEMOD_DETECT
                    pt_tuner_info->e_bandwidth  = (TUNER_BANDWIDTH_T) t_signal.e_bandwidth;
                    pt_tuner_info->e_hierarchy_priority = (TUNER_HIERARCHY_PRIORITY_T) t_signal.e_hierarchy_priority;
#else
                    pt_tuner_info->e_guard_int = (TUNER_GUARD_INTERVAL_T) t_signal.u1GI;
                    pt_tuner_info->e_timer_int = (TUNER_TIMER_INTERLEAVING_T) t_signal.u1TimDeInterl;
                    pt_tuner_info->e_FecRate = (TUNER_CODRRATE_T) t_signal.u1FecRate;
                    pt_tuner_info->e_CarrierMode = (TUNER_CARRIER_MODE_T) t_signal.u1CarrierMode;
                    pt_tuner_info->e_PnNumber = (TUNER_PN_SEQ_T) t_signal.u1PnNumber;
#endif               
                    mcSHOW_DRVAPI_MSG(("[MWInfo] pt_tuner_info->e_hierarchy_priority = %d\n", pt_tuner_info->e_hierarchy_priority));
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_CAB_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_CAB_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_CAB_DIG_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_CAB_DIG_TUNE_INFO_T *) pv_get_info;

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->ui4_sym_rate = t_signal.SymbolRate * 1000;   /* sps */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SAT_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_SAT_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_SAT_DIG_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
#if 0
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *) pv_get_info;

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_SAT_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency;    /* Hz */
                    pt_tuner_info->ui4_sym_rate = t_signal.SymbolRate;   /* sps */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
                }
#else
				return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_SAT_DIG));
#endif
			}
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGet: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            //break;
        }
        case TUNER_GET_TYPE_CAB_DIG_OOB_RX:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_get_info_len < sizeof(TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T))
                { /* not enough space */
                    *pz_get_info_len = sizeof(TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                    mcSHOW_DRVERR_MSG(("TunerGetOob: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                    return (RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T *pt_tuner_info = (TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T *) pv_get_info;

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_DIG_OOB_RX, &t_signal);
                    pt_tuner_info->ui4_freq      = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->ui4_data_rate = t_signal.SymbolRate * 1000;
                    pt_tuner_info->e_mod         = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
                }
            }
            else
            { /* if not connected */
                mcSHOW_DRVAPI_MSG(("TunerGetOob: RMR_DRV_NO_GET_INFO!\n"));
                return (RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SIGNAL_LEVEL:
        {
            if (*pz_get_info_len < sizeof(UINT8))
            { /* not enough space */
                *pz_get_info_len = sizeof(UINT8);
                mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                *(UINT8 *)pv_get_info = TunerGetSignalLevel(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
            }
            mcSHOW_DRVAPI_MSG(("%s Signal level: (%d/100)\n", ptStateInfo->comp_name, *(UINT8 *)pv_get_info));
            break;
        }
        case TUNER_GET_TYPE_STATE:
        {
            if (*pz_get_info_len < sizeof(TUNER_STATE_INFO_T))
            { /* not enough space */
                *pz_get_info_len = sizeof(TUNER_STATE_INFO_T);
                mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                TUNER_STATE_INFO_T *pt_tuner_state;

                pt_tuner_state = (TUNER_STATE_INFO_T *)pv_get_info;
                pt_tuner_state->e_state = ptStateInfo->e_tuner_state;
                if ((pt_comp_id->e_type == DRVT_TUNER_CAB_ANA) ||
                    (pt_comp_id->e_type == DRVT_TUNER_TER_ANA) ||
                    (pt_comp_id->e_type == DRVT_TUNER_SAT_ANA))
                {
                    pt_tuner_state->b_dig_signal = (FALSE);
                }
                else
                {
                    pt_tuner_state->b_dig_signal = (TRUE);
                }
            }
            mcSHOW_DRVAPI_MSG(("%s Tuner state: (%d)\n",ptStateInfo->comp_name, ptStateInfo->e_tuner_state));
            mcSHOW_DRVAPI_MSG(("%s Current HW sync state: (%d)\n",ptStateInfo->comp_name,
                        TunerGetSync(ptTunerCtx->TunerID, ptStateInfo->e_conn_type)));
            break;
        }
        case TUNER_GET_TYPE_NO_DISC_NFY:
        {
            if (*pz_get_info_len < sizeof(BOOL))
            { /* not enough space */
                *pz_get_info_len = sizeof(BOOL);
                mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                *(BOOL *)pv_get_info = ptStateInfo->b_disable_nfy;
            }
            break;
        }
        case TUNER_GET_TYPE_FE_STATUS:
        {
            if (*pz_get_info_len < sizeof(UINT8))            
            { /* not enough space */
                *pz_get_info_len = sizeof(UINT8);
                mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            } 
            else 
            { /* get info */
            #ifndef CC_DUAL_TUNER_SUPPORT 
                *(UINT8 *)pv_get_info = t_fe_status;
            #else //temp mark for dual tuner
                *(UINT8 *)pv_get_info = ptStateInfo->dual_tuner_state;
            #endif
            }            
            break;
        }
        case TUNER_GET_TYPE_DBM_SIGNAL_LEVEL:
        {
            if (*pz_get_info_len < sizeof(INT16))
            { /* not enough space */
                *pz_get_info_len = sizeof(INT16);
                mcSHOW_DRVERR_MSG(("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                *(INT16 *)pv_get_info = TunerGetSignalLeveldBm(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
            }
            mcSHOW_DRVAPI_MSG(("%s (%d dBm)\n",ptStateInfo->comp_name, *(INT16 *) pv_get_info / 10));
            break;
        }

        case TUNER_GET_TYPE_ANA_ATTRIBUTE:
        case TUNER_GET_TYPE_TV_SYS_GROUP:
        case TUNER_GET_TYPE_TV_SYS_MASK:
        {
            if (pt_comp_id->e_type==DRVT_TUNER_TER_ANA)
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_TER_ANA));
            }
            else if (pt_comp_id->e_type==DRVT_TUNER_CAB_ANA)
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_CAB_ANA));
            }
            break;
        }

        case TUNER_GET_TYPE_BER:
        case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        case TUNER_GET_TYPE_RAW_SIGNAL_LEVEL:
        case TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE:
        case TUNER_GET_TYPE_BER_TOLERANCE:
#ifdef TUNER_ISDBT_EWS    //if ISDBT then EWS enable          
		case TUNER_GET_TYPE_EWS_STATUS:
        {
            if ((pt_comp_id->e_type==DRVT_TUNER_TER_ANA) || (pt_comp_id->e_type==DRVT_TUNER_TER_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_TER_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_CAB_ANA) || (pt_comp_id->e_type==DRVT_TUNER_CAB_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_CAB_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_SAT_ANA) || (pt_comp_id->e_type==DRVT_TUNER_SAT_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_SAT_DIG));
            }
            else
            {
                return (TunerGetAttribute(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len));
            }
        }
#endif        		
        default:
        {
            if ((pt_comp_id->e_type==DRVT_TUNER_TER_ANA) || (pt_comp_id->e_type==DRVT_TUNER_TER_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_TER_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_CAB_ANA) || (pt_comp_id->e_type==DRVT_TUNER_CAB_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_CAB_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_SAT_ANA) || (pt_comp_id->e_type==DRVT_TUNER_SAT_DIG))
            {
                return (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len, TUNER_CONN_TYPE_SAT_DIG));
            }
            else
            {
                return (TunerGetAttribute(ptTunerCtx->TunerID, e_get_type, pv_get_info, pz_get_info_len));
            }
        }
    }

    mcSHOW_DRVAPIOK_MSG(("TunerGet: RMR_OK!\n"));
    return (RMR_OK);
}

/*
This function implements component set function.
*/
//static INT32 TunerSet(DRV_COMP_ID_T*  pt_comp_id,
INT32 TunerSet(DRV_COMP_ID_T*  pt_comp_id,
                      DRV_SET_TYPE_T  e_set_type,
                      const VOID*     pv_set_info,
                      SIZE_T          z_set_info_len)
{
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;
    INT32               retStatus;

    mcSHOW_DRVAPI_MSG(("[MWCmd]TunerSet()\n"));
    if (bDetachMW)
    {
        mcSHOW_DRVAPI_MSG(("TunerDriver Detach MW\n"));
        return (RMR_OK);
    }

    // Service will be provided ONLY when tuner is opened successfully.
    if (!TDD_Ready)
    {
        mcSEMA_LOCK(t_init_sync);
        mcSEMA_UNLOCK(t_init_sync);
    }

    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo) != DRVAPI_TUNER_OK)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    if (ptStateInfo == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    /* verify the request */
    if ((pt_comp_id->e_type != DRVT_TUNER_CAB_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_DIG_OOB_RX) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_DIG)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
        mcSHOW_DRVERR_MSG(("TunerSet: RMR_DRV_INV_SET_INFO!\n"));
        return (RMR_DRV_INV_SET_INFO);
    }
#ifndef DTD_INCLUDE
    SetRemapConnType(pt_comp_id->ui2_id,ptStateInfo->e_conn_type);
#endif
    /* supported types */
    switch (e_set_type)
    {
        case TUNER_SET_TYPE_NO_DISC_NFY:
        {
            if (z_set_info_len < sizeof(BOOL))
            { /* not enough space */
                z_set_info_len = sizeof(BOOL);
                mcSHOW_DRVERR_MSG(("TunerSet: RMR_DRV_NOT_ENOUGH_SPACE!\n"));
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* set info */
                ptStateInfo->b_disable_nfy = (BOOL)(S32)pv_set_info;
            }
            break;
        }
        case TUNER_SET_TYPE_CI_CARD_TYPE:
        {
            retStatus = TunerSetAttribute(ptTunerCtx->TunerID, e_set_type, pv_set_info, z_set_info_len);
            if (retStatus != RMR_OK)
            {
                return retStatus;
            }
            break;
        }
		
#ifdef CC_DVBS_SUPPORT_FASTACQ
        case TUNER_SET_TYPE_DISEQC_CMD:
        {
			DISEQC_BUS_CMD_T *pCmd = (DISEQC_BUS_CMD_T*) pv_set_info; 
			if(FastAcq_CheckDiseqccmd(pCmd) == TRUE)
			{
				return (RMR_OK);
			}
			// Save signal information to global.
			x_memcpy(&t_PreTune_Info.t_diseqc_cmd, pCmd, sizeof(DISEQC_BUS_CMD_T));
			return (TunerSetAttribute(ptTunerCtx->TunerID, e_set_type, pv_set_info, z_set_info_len));
		}
#endif

        default:
        {
            // Ii is more reasonable for pd_glue to handle this
            return (TunerSetAttribute(ptTunerCtx->TunerID, e_set_type, pv_set_info, z_set_info_len));
        }
    }

    /* there is nothing to be set with digital cable tuner */
    mcSHOW_DRVAPIOK_MSG(("TunerSet: RMR_OK!\n"));
    return (RMR_OK);
}

/*
This function implements the tuner state machine process and never return.
*/
VOID TunerProcess(VOID *pvArg)
{
    STATE_INFO_T*   ptStateInfo = *(PSTATE_INFO_T*)pvArg;
    TUNER_CTX_T*    ptTunerCtx = (TUNER_CTX_T*)ptStateInfo->ptParentTunerCtx;
    UCHAR           uc_unluck;
    TIME_TICK_T     t0_unlock, t1_unlock;
    TIME_DIFF_T     t_diff;
    S16             sync_sts = 0;
    UINT32          u4TickCnt;
    BOOL            fgRetSts = FALSE;
    BOOL            b_notified = FALSE;
#ifdef GETSYNCTIME_MODIFY
    BOOL b_enable_getsync = TRUE;
    UINT16 unlock_cnt = 0;
#endif
#ifdef TUNER_ISDBT_EWS    //if ISDBT then EWS enable 
    EWS_INFO        ewsInfo = {FALSE, 0};
    SIZE_T          ewsInfoLen;
#endif
    BOOL            bPrevDetachMW = 0;


    mcSHOW_DRVAPI_MSG(("TunerProcess %s is running!\n", ptStateInfo->thread_name));
    ptStateInfo->e_tuner_state = TUNER_STATE_DISCONNECTED;
    uc_unluck = 0;

    while (ptStateInfo->fgTunerThreadRunning)
    {
        //While Detach MW,  pause the TunerProcess. - mtk40109 2009-07-15
        if(bDetachMW)
        {
            mcDELAY_MS(cMAX_RESET_TIME);
            bPrevDetachMW = 1;
            continue;
        }

        switch (ptStateInfo->e_tuner_state)
        {
            case TUNER_STATE_DISCONNECTED:
            {
                b_notified = FALSE;

            #ifndef CC_DUAL_TUNER_SUPPORT
                mcMUTEX_UNLOCK(t_tuner_scan_lock);
            #endif
                mcSHOW_DRVLOCK_MSG(("\t****** .UNLOCK_disconnected\n"));
                mcSEMA_UNLOCK(ptStateInfo->t_tuner_disconnected);
                mcSHOW_DRVLOCK_MSG(("\t****** LOCK_connecting\n"));

                // Waiting for t_tuner_connecting
                mcSEMA_LOCK(ptStateInfo->t_tuner_connecting);
                mcSHOW_DRVLOCK_MSG(("\t****** LOCK2_connecting\n"));
                break;
            }
            
            case TUNER_STATE_CONNECTING:
            {
                b_notified = FALSE;
            #ifdef GETSYNCTIME_MODIFY
                b_enable_getsync = TRUE;
                unlock_cnt = cLOCK_GETSYNC_TIME;
            #endif
                /* open a new acquisition */
                u4TickCnt = os_get_sys_tick();

				#ifdef TIME_MEASUREMENT
				TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "TunerAcq_S");
				#endif

            #if TUNER_DTV_NOTIFY_ENABLE
                gPt_comp_id = &ptStateInfo->t_tuner_comp_id;/* Add the value for nofity MW when DTV Fine Tuner Freq */
            #endif
            #ifndef CC_DUAL_TUNER_SUPPORT
                mcMUTEX_LOCK(t_tuner_scan_lock);
            #endif
				#ifdef CC_DEMOD_FASTACQ
				if((Get_FgBootupWithChannel() == TRUE) && (u1FastAcqMode == TRUE))
				{
					u1FastAcqMode = FALSE;
					fgRetSts = (BOOL)TunerGetSync(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
					mcSHOW_USER_CRI_MSG(("[FASTACQ]Fast Acq, no need to TunerAcq in TunerProcess!\n"));
				}
				else
				#endif
				{

                fgRetSts = TunerAcq(ptTunerCtx->TunerID, ptStateInfo->e_conn_type,
                                    (void*)&ptStateInfo->t_conn_info,
                                    ptStateInfo->z_conn_info_len,
                                    TunerBreak, &ptStateInfo->t_tuner_comp_id);

				}

				#ifdef TIME_MEASUREMENT
				TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "TunerAcq_E");
				#endif

                if (fgRetSts) 
                {/* lock */
                    mcSHOW_DRVAPI_MSG(("%s: Connected in TUNER_STATE_CONNECTING\n", ptStateInfo->comp_name));
                    uc_unluck = 0;
                    TunerStateChange(ptStateInfo, TUNER_STATE_CONNECTING, TUNER_STATE_CONNECTED);
                 #ifndef CC_DUAL_TUNER_SUPPORT
                    t_fe_status = TUNER_COND_STATUS_GOT_SIGNAL;
                 #else
                    ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_GOT_SIGNAL;
                 #endif
					
                    /* always report connected */
                    ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                            DRV_COND_CONNECTED,
                            ptStateInfo->pv_tuner_nfy_tag,
                            RM_CONN_REASON_AS_REQUESTED);
                    
                    if(TunerNotifyEnable(ptTunerCtx->TunerID) == TRUE)/* Enable/Disable Notify Got Signal to ResMgr*/
                    {
                        ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                DRV_COND_STATUS,
                                ptStateInfo->pv_tuner_nfy_tag,
                                TUNER_COND_STATUS_GOT_SIGNAL);
                    }

#ifdef TUNER_ISDBT_EWS    //if ISDBT then EWS enable
                    //! EWS notification
                    TunerGetAttribute(ptTunerCtx->TunerID, TUNER_GET_SPECIAL_TYPE_EWS_STATUS, (void*)&ewsInfo, (SIZE_T*)&ewsInfoLen);
                    if (ewsInfo.isNotify)
                    {//! EWS triggerred
                        if (ewsInfo.val)
                        { //! EWS ON
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_STATUS,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_COND_STATUS_EWS_ON);
                        }
                        else
                        {//! EWS OFF
                        
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_STATUS,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_COND_STATUS_EWS_OFF);
                        }
                    }
#endif
					
                }
                else
                {/* out of lock */  
                    mcSHOW_DRVAPI_MSG(("%s: Can't sync in TUNER_STATE_CONNECTING\n", ptStateInfo->comp_name));
                    if (!fStayInConnectedState)
                    {
                        if (!ptStateInfo->b_disable_nfy)
                        {
                            mcSHOW_DBG_MSG(("[TUNER_STATE_CONNECTING] b_disable_nfy = FALSE\n"));
                            TunerStateChange(ptStateInfo, TUNER_STATE_CONNECTING, TUNER_STATE_DISCONNECTED);
                        #ifndef CC_DUAL_TUNER_SUPPORT
                            t_fe_status = TUNER_COND_STATUS_LOST_SIGNAL;
                        #else
                            ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_LOST_SIGNAL;
                        #endif
                            
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_DISCONNECTED,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_DISC_REASON_SIGNAL_LOSS);
                        }
                        else
                        {
                            mcSHOW_DBG_MSG(("[TUNER_STATE_CONNECTING] b_disable_nfy = TRUE\n"));
                            TunerStateChange(ptStateInfo, TUNER_STATE_CONNECTING, TUNER_STATE_CONNECTED);
                            
                        #ifndef CC_DUAL_TUNER_SUPPORT
                            t_fe_status = TUNER_COND_STATUS_LOST_SIGNAL;
                        #else
                            ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_LOST_SIGNAL;
                        #endif
                            
                            /* always report connected */
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_CONNECTED,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    RM_CONN_REASON_AS_REQUESTED);
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_STATUS,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_COND_STATUS_LOST_SIGNAL);

                            b_notified = TRUE;
                        }
                    }
                }

                u4TickCnt = os_get_sys_tick()-u4TickCnt;
                u2AcqTime = u4TickCnt * x_os_drv_get_tick_period();
                mcSHOW_DRVAPI_MSG(("TunerID%d Connection time = %d ms\n",ptTunerCtx->TunerID, u2AcqTime));
                break;
            }
            
            case TUNER_STATE_CONNECTED:
            {
                //While nim.dm 0,  resume the TunerProcess. - mtk40109 2009-07-21
                //Sync the PI state with HAL state by calling TunerAcq.
                // The last parameter must be NULL to guarantee TunerBreak return FALSE.
                if(bDetachMW == 0 && bPrevDetachMW == 1)
                {
                    TunerAcq(ptTunerCtx->TunerID, ptStateInfo->e_conn_type,
                            (void*)&ptStateInfo->t_conn_info,
                            ptStateInfo->z_conn_info_len,
                            TunerBreak, NULL);
                }
                
                if( (ptStateInfo->e_conn_type == TUNER_CONN_TYPE_SAT_DIG) &&
                     (TunerNotifyEnable(ptTunerCtx->TunerID) == FALSE) )
                {
                    mcSEMA_LOCK_TIMEOUT(ptStateInfo->t_tuner_sync, cUNCLOCK_POLLING_TIME);
                    break;
                }
				
            #ifdef GETSYNCTIME_MODIFY
                if( (b_enable_getsync == TRUE && unlock_cnt>=cLOCK_GETSYNC_TIME) ||
                     unlock_cnt >= cUNLOCK_GETSYNC_TIME)
                 {
                    //mcSHOW_DRVAPI_MSG(("GetSync is running!\n"));
                    unlock_cnt=0;
                    sync_sts = TunerGetSync(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
                 }
                else
                    unlock_cnt++;
            #else
                    sync_sts = TunerGetSync(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
            #endif
				
                if (sync_sts)
                { /* lock & minitor */
                #ifndef CC_DUAL_TUNER_SUPPORT
                    t_fe_status = TUNER_COND_STATUS_GOT_SIGNAL;
                #else
                    ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_GOT_SIGNAL;
                #endif
                #ifdef GETSYNCTIME_MODIFY
                    b_enable_getsync = TRUE;
                #endif
                    if (TRUE == b_notified)
                    {
                        ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                DRV_COND_STATUS,
                                ptStateInfo->pv_tuner_nfy_tag,
                                TUNER_COND_STATUS_GOT_SIGNAL);
                            
                        b_notified = FALSE;
                    }
#ifdef TUNER_ISDBT_EWS    //if ISDBT then EWS enable  
                    //! EWS notification
                #ifdef GETSYNCTIME_MODIFY
                    if(unlock_cnt%2 == 0)
                    {
                #endif
                      TunerGetAttribute(ptTunerCtx->TunerID, TUNER_GET_SPECIAL_TYPE_EWS_STATUS, (void*)&ewsInfo, (SIZE_T*)&ewsInfoLen);
                      if (ewsInfo.isNotify)
                      {//! EWS triggerred
                      
                        if (ewsInfo.val)
                        {//! EWS ON
                        	
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_STATUS,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_COND_STATUS_EWS_ON);
                        }
                        else
                        {//! EWS OFF
                       
                            ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                    DRV_COND_STATUS,
                                    ptStateInfo->pv_tuner_nfy_tag,
                                    TUNER_COND_STATUS_EWS_OFF);
                        }
                      }
                #ifdef GETSYNCTIME_MODIFY
                   }
                #endif
#endif				

                    uc_unluck = 0;
                    mcSEMA_LOCK_TIMEOUT(ptStateInfo->t_tuner_sync, cUNCLOCK_POLLING_TIME);
                }
                else
                { /* out of lock */
                    if (uc_unluck == 0)
                    {
                        mcGET_SYS_TIME(t0_unlock);
                    }                    
                    uc_unluck++;
                 #ifdef GETSYNCTIME_MODIFY
                    b_enable_getsync = FALSE;
                 #endif
                    mcGET_SYS_TIME(t1_unlock);
                    mcGET_DIFF_TIME(t_diff, t0_unlock, t1_unlock);
                    
                    if (mcCONV_SYS_TIME(t_diff) >= cUNLOCK_TIME_THRD)  // To fix bug:DTV00213128
                    //if (uc_unluck >= cUNLOCK_CNT_THRD)
                    {
                        mcSHOW_DRVAPI_MSG(("%s: Loss sync in TUNER_STATE_CONNECTED, try to sync %d times\n", ptStateInfo->comp_name, uc_unluck));
                        uc_unluck = 0;

                        if (fStayInConnectedState)
                        {
                            TunerStateChange(ptStateInfo, TUNER_STATE_CONNECTED, TUNER_STATE_CONNECTING);
                        }
                        else
                        {
                            if (!ptStateInfo->b_disable_nfy)
                            {
                                mcSHOW_DRVERR_MSG(("[TUNER_STATE_CONNECTED] b_disable_nfy = FALSE\n"));
                                TunerStateChange(ptStateInfo, TUNER_STATE_CONNECTED, TUNER_STATE_DISCONNECTED);
                            #ifndef CC_DUAL_TUNER_SUPPORT
                                t_fe_status = TUNER_COND_STATUS_LOST_SIGNAL;
                            #else
                                ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_LOST_SIGNAL;
                            #endif
                                ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                        DRV_COND_DISCONNECTED,
                                        ptStateInfo->pv_tuner_nfy_tag,
                                        TUNER_DISC_REASON_SIGNAL_LOSS);
                            }
                            else
                            {
                                if(TunerNotifyEnable(ptTunerCtx->TunerID) == TRUE)/* Enable/Disable Notify Lost Signal to ResMgr*/
                                {
                                    if (FALSE == b_notified)
                                    {
                                        mcSHOW_DRVERR_MSG(("[TUNER_STATE_CONNECTED] b_disable_nfy = TRUE\n"));
                                    #ifndef CC_DUAL_TUNER_SUPPORT
                                        t_fe_status = TUNER_COND_STATUS_LOST_SIGNAL;
                                    #else
                                        ptStateInfo->dual_tuner_state = TUNER_COND_STATUS_LOST_SIGNAL;
                                    #endif
                                        ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                                                DRV_COND_STATUS,
                                                ptStateInfo->pv_tuner_nfy_tag,
                                                TUNER_COND_STATUS_LOST_SIGNAL);
                                        
                                        b_notified = TRUE;
                                    }
                                }
                            }
                        }
                    }
                    mcDELAY_MS(cLOCK_POLLING_TIME);
                }
                break;
            }

            case TUNER_STATE_DISCONNECTING:
            {
                mcSHOW_DRVAPI_MSG(("%s: Got disconnection command\n", ptStateInfo->comp_name));

                TunerStateChange(ptStateInfo, TUNER_STATE_DISCONNECTING, TUNER_STATE_DISCONNECTED);
                
                mcSEMA_UNLOCK(ptStateInfo->t_tuner_disconnecting);/*bug fix CR: DTV00139714*/
                
                /*Modify for LTDIS protection: Move TunerDisc() from TunerDisconnect() to here. Let tunerbreak callback function can work correctly*/
                TunerDisc(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
                

                if (!ptStateInfo->b_disable_nfy)
                {
                    mcSHOW_DRVERR_MSG(("[TUNER_STATE_DISCONNECTING] b_disable_nfy = FALSE\n"));

                    ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                            DRV_COND_DISCONNECTED,
                            ptStateInfo->pv_tuner_nfy_tag,
                            RM_DISC_REASON_AS_REQUESTED);
                }
                else
                {
                    mcSHOW_DRVERR_MSG(("[TUNER_STATE_DISCONNECTING] b_disable_nfy = TRUE\n"));

                    ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                            DRV_COND_STATUS,
                            ptStateInfo->pv_tuner_nfy_tag,
                            TUNER_COND_STATUS_LOST_SIGNAL);
                }
                break;
            }
        } /* switch */

        bPrevDetachMW = 0;
        mcCHK_STATE();
    } /* while */

    ptStateInfo->t_tuner_thread = (THREAD_T)NULL;
    mcSHOW_DRVAPI_MSG(("TunerProcess %s exit!\n", ptStateInfo->thread_name));
    mcTHREAD_DESTROY();
}

#if TUNER_DTV_NOTIFY_ENABLE
/* Add the value for nofity MW when DTV Fine Tuner Freq */
INT32 Tuner_Notify(DRV_COND_T e_nfy_cond, UINT32 u4Msg)
{
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;

    mcSHOW_DRVAPI_MSG(("[HALCmd]Tuner_Notify()\n"));    
    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(gPt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    if (ptStateInfo == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    ptStateInfo->pf_tuner_nfy(&ptStateInfo->t_tuner_comp_id,
                            e_nfy_cond,
                            ptStateInfo->pv_tuner_nfy_tag,
                            u4Msg);
    return (RMR_OK);
}
#endif

/*
This function cancels tuner thread and close tuner I2C device.
*/
INT32 Tuner_Kill(void)
{
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo;

    if (!TunerInited)
    {
        mcSHOW_DRVERR_MSG(("Tuner had NOT been initialized.\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    //++ Make sure init_thread has been terminated
    while (t_init_thread)
    {
        mcDELAY_MS(10);    // in ms
    }
    
    if (t_init_sync)
    {
        mcSEMA_DESTROY(t_init_sync);
    }
    //--

#ifndef CC_DUAL_TUNER_SUPPORT
    if (t_tuner_scan_lock)
    {
        mcMUTEX_DESTROY(t_tuner_scan_lock);
    }
#endif
    while (QueueGetHead(&TunerCtxList))
    {
        // Remove from TunerCtxList to prevent future possible calls of
        // exported APIs.
        ptTunerCtx = (TUNER_CTX_T*)QueuePopHead(&TunerCtxList);

        // Make sure no exported API is under-process
        if (ptTunerCtx->RefSema)
        {
            while (ptTunerCtx->RefCnt)
            {
                mcDELAY_MS(10);    // in ms
            }
        }

        // Destroy RefSema
        if (ptTunerCtx->RefSema)
        {
            mcSEMA_DESTROY(ptTunerCtx->RefSema);
        }

        // Clean up all StateInfos
        while (QueueGetHead(&ptTunerCtx->StateInfoList))
        {
            ptStateInfo = (STATE_INFO_T*)QueuePopHead(&ptTunerCtx->StateInfoList);

            // Terminate thread
            ptStateInfo->fgTunerThreadRunning = FALSE;
            /* Force Thread leave idle state, therefore can change to exit state */
            mcSEMA_UNLOCK(ptStateInfo->t_tuner_connecting);
            mcSEMA_UNLOCK(ptStateInfo->t_tuner_sync);

            while (ptStateInfo->t_tuner_thread)
            {
                mcDELAY_MS(10);    // in ms
            }

            // Destroy all semaphores in StateInfo
            if (ptStateInfo->t_tuner_state_lock)
            {
                mcMUTEX_DESTROY(ptStateInfo->t_tuner_state_lock);
            }
            if (ptStateInfo->t_tuner_disconnected)
            {
                mcSEMA_DESTROY(ptStateInfo->t_tuner_disconnected);
            }
            if (ptStateInfo->t_tuner_disconnecting)
            {
                mcSEMA_DESTROY(ptStateInfo->t_tuner_disconnecting);
            }
            if (ptStateInfo->t_tuner_connecting)
            {
                mcSEMA_DESTROY(ptStateInfo->t_tuner_connecting);
            }
            if (ptStateInfo->t_tuner_sync)
            {
                mcSEMA_DESTROY(ptStateInfo->t_tuner_sync);
            }
			#ifdef CC_DEMOD_FASTACQ
				// Terminate thread
	            ptStateInfo->fgfaThreadRunning = FALSE;
	            /* Force Thread leave idle state, therefore can change to exit state */
	            mcSEMA_UNLOCK(ptStateInfo->t_fa_connecting);
	            mcSEMA_UNLOCK(ptStateInfo->t_fa_sync);

	            while (ptStateInfo->t_fa_thread)
	            {
	                mcDELAY_MS(10);    // in ms
	            }

	            // Destroy all semaphores in StateInfo
	            if (ptStateInfo->t_fa_state_lock)
	            {
	                mcMUTEX_DESTROY(ptStateInfo->t_fa_state_lock);
	            }
	            if (ptStateInfo->t_fa_disconnected)
	            {
	                mcSEMA_DESTROY(ptStateInfo->t_fa_disconnected);
	            }
	            if (ptStateInfo->t_fa_disconnecting)
	            {
	                mcSEMA_DESTROY(ptStateInfo->t_fa_disconnecting);
	            }
	            if (ptStateInfo->t_fa_connecting)
	            {
	                mcSEMA_DESTROY(ptStateInfo->t_fa_connecting);
	            }
	            if (ptStateInfo->t_fa_sync)
	            {
	                mcSEMA_DESTROY(ptStateInfo->t_fa_sync);
	            }
			#endif

            // Free StateInfo
            x_mem_free(ptStateInfo);
        } // End of while (QueueGetHead(&ptTunerCtx->StateInfoList))

        // Free TunerCtx
        x_mem_free(ptTunerCtx);
    } // End of while(QueueGetHead(&TunerCtxList))

    TunerClose();
    
    TunerInited = 0;
    mcSHOW_DRVAPI_MSG(("Tuner_Kill: DRVAPI_TUNER_OK!\n"));

    return (DRVAPI_TUNER_OK);
}

/*
 * Description: Initialize state info list and create thread.
 * @weile 2012.8.7
*/
INT32 _InitializeStateInfo(TUNER_CTX_T* ptTunerCtx, STATE_INFO_T* ptStateInfo,
                            DRV_TYPE_T DrvType)
{
    ptStateInfo->ptParentTunerCtx = (VOID*)ptTunerCtx;
    ptStateInfo->t_tuner_comp_id.e_type = DrvType;
    ptStateInfo->t_tuner_comp_id.ui2_id = ptTunerCtx->TunerID;
    ptStateInfo->t_tuner_comp_reg.e_type = DrvType;
    ptStateInfo->t_tuner_comp_reg.e_id_type = ID_TYPE_IND;
    ptStateInfo->t_tuner_comp_reg.u.t_ind.ui2_id = ptTunerCtx->TunerID;

    switch(DrvType)
    {
        case DRVT_TUNER_SAT_DIG:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_SAT_DIG;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_SAT", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_SAT_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_CAB_DIG:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_CAB_DIG;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_CAB", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_CAB_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_TER_DIG:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_TER_DIG;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_TER", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_TER_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_SAT_ANA:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_SAT_ANA;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_SAT_ANA", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_SAT_ANA_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_TER_ANA:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_TER_ANA;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_TER_ANA", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_TER_ANA_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_CAB_ANA:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_CAB_ANA;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_CAB_ANA", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_CAB_ANA_Thread", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_CAB_DIG_OOB_RX:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_CAB_DIG_OOB_RX;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_OOBRX", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_ORX_Thread", ptTunerCtx->TunerID);
            break;
#ifdef TUNER_SUPPORT_SCART_OUT
        case DRVT_TUNER_SAT_ANA_SCART_OUT:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_SAT_ANA_SCART_OUT;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_SAT_SCRT", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_SAT_SCRT_Trd", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_CAB_ANA_SCART_OUT:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_CAB_ANA_SCART_OUT;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_CAB_SCRT", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_CAB_SCRT_Trd", ptTunerCtx->TunerID);
            break;
        case DRVT_TUNER_TER_ANA_SCART_OUT:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_TER_ANA_SCART_OUT;
            snprintf(ptStateInfo->comp_name, sizeof(ptStateInfo->comp_name), "Comp_%d_TER_SCRT", ptTunerCtx->TunerID);
            snprintf(ptStateInfo->thread_name, sizeof(ptStateInfo->thread_name), "TU%d_TER_SCRT_Trd", ptTunerCtx->TunerID);
            break;
#endif
        default:
            ptStateInfo->e_conn_type = TUNER_CONN_TYPE_TER_DIG;
            break;
    }

    ptStateInfo->fgTunerThreadRunning = TRUE;
    if (mcMUTEX_CREATE(&ptStateInfo->t_tuner_state_lock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcMUTEX_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    mcMUTEX_UNLOCK(ptStateInfo->t_tuner_state_lock);

    if (mcSEMA_CREATE(&ptStateInfo->t_tuner_disconnected, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    if (mcSEMA_CREATE(&ptStateInfo->t_tuner_disconnecting, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    if (mcSEMA_CREATE(&ptStateInfo->t_tuner_connecting, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    if (mcSEMA_CREATE(&ptStateInfo->t_tuner_sync, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    
#if 0//remove for Daniel test
    if (mcSEMA_CREATE(&ptStateInfo->t_sync_tunerprocess_acq_and_mwtask, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
#endif
    // Ready for creating Thread
    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not
    // directly use pv_arg. Therefore, if we want thread to share the same
    // context as caller, we need set pv_arg as "pointer of context".

    //EC_MW_IF ToDo

    if (mcTHREAD_CREATE(&(ptStateInfo->t_tuner_thread),
            ptStateInfo->thread_name,
            cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
            TunerProcess, sizeof(void*), (void*)&ptStateInfo) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    
	#ifdef CC_DEMOD_FASTACQ
		switch(DrvType)
	    {
	        case DRVT_TUNER_SAT_DIG:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_SAT_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_CAB_DIG:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_CAB_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_TER_DIG:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_TER_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_SAT_ANA:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_SAT_ANA_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_TER_ANA:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_TER_ANA_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_CAB_ANA:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_CAB_ANA_Thread", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_CAB_DIG_OOB_RX:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_ORX_Thread", ptTunerCtx->TunerID);
	            break;
		#ifdef TUNER_SUPPORT_SCART_OUT
	        case DRVT_TUNER_SAT_ANA_SCART_OUT:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_SAT_SCRT_Trd", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_CAB_ANA_SCART_OUT:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_CAB_SCRT_Trd", ptTunerCtx->TunerID);
	            break;
	        case DRVT_TUNER_TER_ANA_SCART_OUT:
	            snprintf(ptStateInfo->fa_thread_name, sizeof(ptStateInfo->fa_thread_name), "FA_TU%d_TER_SCRT_Trd", ptTunerCtx->TunerID);
	            break;
		#endif
	        default:
	            break;
	    }
		ptStateInfo->fgfaThreadRunning = TRUE;
	    if (mcMUTEX_CREATE(&ptStateInfo->t_fa_state_lock, X_SEMA_STATE_LOCK) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("TunerInit (mcMUTEX_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }

	    mcMUTEX_UNLOCK(ptStateInfo->t_fa_state_lock);

	    if (mcSEMA_CREATE(&ptStateInfo->t_fa_disconnected, X_SEMA_STATE_LOCK) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }

	    if (mcSEMA_CREATE(&ptStateInfo->t_fa_disconnecting, X_SEMA_STATE_LOCK) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }

	    if (mcSEMA_CREATE(&ptStateInfo->t_fa_connecting, X_SEMA_STATE_LOCK) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }

	    if (mcSEMA_CREATE(&ptStateInfo->t_fa_sync, X_SEMA_STATE_LOCK) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }
		if (mcTHREAD_CREATE(&(ptStateInfo->t_fa_thread),
	            ptStateInfo->fa_thread_name,
	            cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
	            FastAcqWEepProcess, sizeof(void*), (void*)&ptStateInfo) != OSR_OK)
	    {
	        mcSHOW_DRVERR_MSG(("FastAcqWEepProcess (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
	        return (DRVAPI_TUNER_ERROR);
	    }
	#endif

    
    if (fptIniThreadNfy != NULL)
    {
        if (fptIniThreadNfy(ptTunerCtx->TunerID, DrvType ) != OSR_OK)
        {
            printf(" -IO: DRVAPI_TUNER_ERROR!\n");
            return (DRVAPI_TUNER_ERROR);
        }
        printf(" -IO: fptIniThreadNfy trigger\n");
    }

    return (DRVAPI_TUNER_OK);
}
    

/*
 * Description: move Tuner Init to TunerHAL.
 * @weile 2012.8.7
*/
INT32 _Tuner_Init(void)
{
    INT32               res;
    UCHAR               i;
    TUNER_CONFIG_TBL_T  tTunerCfgTbl;
    TUNER_CTX_T*        ptTunerCtx;
    STATE_INFO_T*       ptStateInfo_SAT=NULL;
    STATE_INFO_T*       ptStateInfo_TER=NULL;
    STATE_INFO_T*       ptStateInfo_CAB=NULL;
    STATE_INFO_T*       ptStateInfo_OOBRx;
    STATE_INFO_T*       ptStateInfo_SAT_ANA=NULL;
    STATE_INFO_T*       ptStateInfo_TER_ANA=NULL;
    STATE_INFO_T*       ptStateInfo_CAB_ANA=NULL;

#ifdef TUNER_SUPPORT_SCART_OUT
    STATE_INFO_T*       ptStateInfo_SAT_ANA_SCART_OUT=NULL;
    STATE_INFO_T*       ptStateInfo_TER_ANA_SCART_OUT=NULL;
    STATE_INFO_T*       ptStateInfo_CAB_ANA_SCART_OUT=NULL;
#endif

    UCHAR               index = 0;

    if (TunerInited)
    {
        mcSHOW_DRVERR_MSG(("Tuner had been initialized \n"));
        return (DRVAPI_TUNER_ERROR);
    }

    //gInit_Start_Time = os_get_sys_tick();
    gInit_Start_Time = 0;

    mcSHOW_DRVAPI_MSG(("TunerInit...... \n"));
        
    // Create Thread for TunerOpen to save overall boot time.
    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not
    // directly use pv_arg. Therefore, if we want thread to share the same
    // context as caller, we need set pv_arg as "pointer of context".
    TDD_Ready = FALSE;

    if (mcSEMA_CREATE(&t_init_sync, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("t_init_sync (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
        goto TUNER_INIT_ERROR_HANDLER;
    }

    snprintf(init_thread_name, sizeof(init_thread_name), "InitThread");

#ifndef CC_DUAL_TUNER_SUPPORT
    if (mcMUTEX_CREATE(&t_tuner_scan_lock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("TunerInit (mcMUTEX_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    mcMUTEX_UNLOCK(t_tuner_scan_lock);
#endif
    
    if (mcTHREAD_CREATE(&t_init_thread,
                        init_thread_name,
                        cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
                        InitThreadProc, sizeof(void*), (void*)NULL) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("InitThreadProc (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        goto TUNER_INIT_ERROR_HANDLER;
    }

    res = TDIS_RetrieveTunerCfgTbl(&tTunerCfgTbl);
    if ((res != DRVAPI_TUNER_OK) || (tTunerCfgTbl.TunerNum > MAX_TUNER_NUM))
    {
        goto TUNER_INIT_ERROR_HANDLER;
    }

    // Initialize TunerCtxList
    QueueInitList(&TunerCtxList);

    // Allocate/Initialize resources for each Tuner Device
    for (i = 0; i < tTunerCfgTbl.TunerNum; i++)
    {
        /**************************************************************/
        /*  Allocate TunerCtx and Link to TunerCtxList                */
        /**************************************************************/
        ptTunerCtx = (TUNER_CTX_T*)x_mem_calloc(1, sizeof(TUNER_CTX_T));
        if (ptTunerCtx == NULL)
        {
            goto TUNER_INIT_ERROR_HANDLER;;
        }
        // Link TunerCtx to TunerCtxList
        QueuePutTail(&TunerCtxList, &ptTunerCtx->Link);

        /*************************************************************/
        /*  Initialize TunerCtx                                      */
        /*************************************************************/
        // Save TunerID in TunerCtx
        ptTunerCtx->TunerID = tTunerCfgTbl.tTunerCfg[i].TunerID;
        // Save Tuner's CfgInfo in TunerCtx
        x_memcpy(&ptTunerCtx->tTunerCfg, &tTunerCfgTbl.tTunerCfg[i],
            sizeof(TUNER_CONFIG_T));

        // Initialize RefSema
        if (mcSEMA_CREATE(&ptTunerCtx->RefSema, X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcSHOW_DRVERR_MSG(("TunerInit (mcSEMA_CREATE): DRVAPI_TUNER_ERROR!\n"));
            goto TUNER_INIT_ERROR_HANDLER;
        }
        mcSEMA_UNLOCK(ptTunerCtx->RefSema);

        // Initialize StateInfoList
        QueueInitList(&ptTunerCtx->StateInfoList);

        /****************************************************************/
        /*  Allocate all necessary StateInfos, Link to StateInfoList,   */
        /*  Create thread and register to RM.                           */
        /****************************************************************/
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA)
        {
            ptStateInfo_SAT_ANA = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_SAT_ANA == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_SAT_ANA->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_SAT_ANA, DRVT_TUNER_SAT_ANA)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            	
            MutualExclCompReg_T[index++] = ptStateInfo_SAT_ANA->t_tuner_comp_reg;
        }
            
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA)
        {
            ptStateInfo_TER_ANA = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_TER_ANA == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_TER_ANA->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_TER_ANA, DRVT_TUNER_TER_ANA)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
            MutualExclCompReg_T[index++] = ptStateInfo_TER_ANA->t_tuner_comp_reg;
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA)
        {
            ptStateInfo_CAB_ANA = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_CAB_ANA == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_CAB_ANA->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_CAB_ANA, DRVT_TUNER_CAB_ANA)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
            MutualExclCompReg_T[index++] = ptStateInfo_CAB_ANA->t_tuner_comp_reg;
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA)            
            )
        {
            if (fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_TER_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER_ANA->t_tuner_comp_reg.e_type , ptStateInfo_CAB_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB_ANA->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }


        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA)
            )
        {
            if (fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT_ANA->t_tuner_comp_reg.e_type , ptStateInfo_CAB_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB_ANA->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA)
            )
        {
            if (fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT_ANA->t_tuner_comp_reg.e_type , ptStateInfo_TER_ANA->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER_ANA->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }
        
#ifdef TUNER_SUPPORT_SCART_OUT
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA_SCART_OUT)
        {
            ptStateInfo_SAT_ANA_SCART_OUT = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_SAT_ANA_SCART_OUT == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_SAT_ANA_SCART_OUT->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_SAT_ANA_SCART_OUT, DRVT_TUNER_SAT_ANA_SCART_OUT)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA_SCART_OUT)
        {
            ptStateInfo_TER_ANA_SCART_OUT = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_TER_ANA_SCART_OUT == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_TER_ANA_SCART_OUT->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_TER_ANA_SCART_OUT, DRVT_TUNER_TER_ANA_SCART_OUT)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA_SCART_OUT)
        {
            ptStateInfo_CAB_ANA_SCART_OUT = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_CAB_ANA_SCART_OUT == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_CAB_ANA_SCART_OUT->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_CAB_ANA_SCART_OUT, DRVT_TUNER_CAB_ANA_SCART_OUT)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
             
        }
        
        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA_SCART_OUT)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA_SCART_OUT)
            )
        {
            if(fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_TER_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER_ANA_SCART_OUT->t_tuner_comp_reg.e_type , ptStateInfo_CAB_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB_ANA_SCART_OUT->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA_SCART_OUT)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_ANA_SCART_OUT)
            )
        {
            if(fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT_ANA_SCART_OUT->t_tuner_comp_reg.e_type , ptStateInfo_CAB_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB_ANA_SCART_OUT->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_ANA_SCART_OUT)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_ANA_SCART_OUT)
            )
        {
            if(fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT_ANA_SCART_OUT->t_tuner_comp_reg.e_type , ptStateInfo_TER_ANA_SCART_OUT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER_ANA_SCART_OUT->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }
#endif

        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_DIG)
        {
            ptStateInfo_SAT = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_SAT == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_SAT->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_SAT, DRVT_TUNER_SAT_DIG)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
            MutualExclCompReg_T[index++] = ptStateInfo_SAT->t_tuner_comp_reg;
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_DIG)
        {
            ptStateInfo_TER = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_TER == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_TER->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_TER, DRVT_TUNER_TER_DIG)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
            MutualExclCompReg_T[index++] = ptStateInfo_TER->t_tuner_comp_reg;
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_DIG)
        {
            ptStateInfo_CAB = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_CAB == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_CAB->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_CAB, DRVT_TUNER_CAB_DIG)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
        #ifdef CC_DUAL_TUNER_SUPPORT
            if (index >= 10)
        #else
            if (index >= 8)
        #endif
            {
                mcSHOW_DRVERR_MSG(("Array index overflow!"));
                goto TUNER_INIT_ERROR_HANDLER;
            }
            
            MutualExclCompReg_T[index++] = ptStateInfo_CAB->t_tuner_comp_reg;
        }
        
        if (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_DIG_OOB_RX)
        {
            ptStateInfo_OOBRx = (STATE_INFO_T*)x_mem_calloc(1, sizeof(STATE_INFO_T));
            if (ptStateInfo_OOBRx == NULL)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
            // Link to StateInfoList
            QueuePutTail(&ptTunerCtx->StateInfoList, &ptStateInfo_OOBRx->Link);
            // Initialize StateInfo
            if (_InitializeStateInfo(ptTunerCtx, ptStateInfo_OOBRx, DRVT_TUNER_CAB_DIG_OOB_RX)
                != DRVAPI_TUNER_OK)
            {
                goto TUNER_INIT_ERROR_HANDLER;
            }
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_DIG)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_DIG)
            )
        {
            if(fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_TER->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER->t_tuner_comp_reg.e_type , ptStateInfo_CAB->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }

        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_DIG)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_CAB_DIG)
            )
        {
            if(fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT->t_tuner_comp_reg.e_type , ptStateInfo_CAB->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_CAB->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }
        
        if ((ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_SAT_DIG)
            &&
            (ptTunerCtx->tTunerCfg.i2TunerCapability & cWITH_DRVT_TUNER_TER_DIG)
            )
        {
            if (fptTunerProcNfy != NULL)
            {
                if (fptTunerProcNfy(ptStateInfo_SAT->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_SAT->t_tuner_comp_reg.e_type , ptStateInfo_TER->t_tuner_comp_reg.u.t_ind.ui2_id, ptStateInfo_TER->t_tuner_comp_reg.e_type)!= OSR_OK)
                {
                    mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure!\n"));
                    goto TUNER_INIT_ERROR_HANDLER;
                }
            }
        }
    } // end of for-loop

    ASSERT(index <= MAX_MUTUAL_EXCL_COMP_REG_NUM);
    if (tTunerCfgTbl.MutualExcl)
    {
        if(fptTunerProcNfy != NULL)
           {
               if (fptTunerProcNfy(MutualExclCompReg_T[0].u.t_ind.ui2_id, MutualExclCompReg_T[0].e_type , MutualExclCompReg_T[1].u.t_ind.ui2_id, MutualExclCompReg_T[1].e_type)!= OSR_OK)
               {
                   mcSHOW_DRVERR_MSG(("x_rm_reg_comp_excl_list failure! - MutualExclCompReg_T\n"));
                   ASSERT(0);
                   goto TUNER_INIT_ERROR_HANDLER;
               }
           }
    }

    TunerInited = 1;
    mcSHOW_DRVAPI_MSG(("TunerInit OK \n"));
    // mcSHOW_DRVAPI_MSG(("Tuner_Init API time = %d ms\n",
    // (os_get_sys_tick()-gInit_Start_Time)*x_os_drv_get_tick_period()));
    return (DRVAPI_TUNER_OK);

    TUNER_INIT_ERROR_HANDLER:
        mcSHOW_DRVERR_MSG(("TunerInit failure !! \n"));
        printf("TunerInit fail\n");
        return (DRVAPI_TUNER_ERROR);   
}

#ifndef CC_DUAL_TUNER_SUPPORT //mtk40739 when use dual tuner
VOID fe_status_set(TUNER_COND_STATUS_T fe_status)
{
    t_fe_status = fe_status;
}

UINT8 fe_status_get(VOID)
{
    return t_fe_status;
}
#else
VOID fe_status_set(DRV_COMP_ID_T*   pt_comp_id,TUNER_COND_STATUS_T fe_status)
{
	   TUNER_CTX_T*			ptTunerCtx;
	   STATE_INFO_T*		ptStateInfo;

	// Get corresponding TunerCtx and StateInfo
	if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
	{
		return;
	}
	if (ptStateInfo == NULL)
	{
		return;
	}

    ptStateInfo->dual_tuner_state = fe_status;
}
UINT16 fe_status_get(DRV_COMP_ID_T*   pt_comp_id)
{
	TUNER_CTX_T*	   ptTunerCtx;
	STATE_INFO_T* 	   ptStateInfo;
	
	   // Get corresponding TunerCtx and StateInfo
	   if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo)!= DRVAPI_TUNER_OK)
	   {
		   return DRVAPI_TUNER_ERROR;
	   }
	   if (ptStateInfo == NULL)
	   {
		   return DRVAPI_TUNER_ERROR;
	   }
	
    return  ptStateInfo->dual_tuner_state;
}
#endif

#ifdef CC_DEMOD_FASTACQ
/*
This function set tuner state unconditionally.
*/
static void FaStateSet(STATE_INFO_T* ptStateInfo, TUNER_STATE_T x)
{
    mcMUTEX_LOCK(ptStateInfo->t_fa_state_lock);
    ptStateInfo->e_fa_state = x;
    mcMUTEX_UNLOCK(ptStateInfo->t_fa_state_lock);
    mcSHOW_DRVAPI_MSG(("%s FaStateSet: %d\n", ptStateInfo->comp_name, x));
}

/*
This function change tuner state conditionally.
*/
static void FaStateChange(STATE_INFO_T* ptStateInfo, TUNER_STATE_T os, TUNER_STATE_T ns)
{
    mcMUTEX_LOCK(ptStateInfo->t_fa_state_lock);
    if (ptStateInfo->e_fa_state == os)
    {
        ptStateInfo->e_fa_state = ns;
        mcMUTEX_UNLOCK(ptStateInfo->t_fa_state_lock);
        mcSHOW_DRVAPI_MSG(("%s FaStateChang: %d -> %d\n",ptStateInfo->comp_name, os, ns));
    }
    else
    {
        mcMUTEX_UNLOCK(ptStateInfo->t_fa_state_lock);
    }
}

/*
 * Description: Tuner Fast Acq Write EEPROM thread  function
 * @Lei20130927
*/
VOID FastAcqWEepProcess(VOID *pvArg)
{
    STATE_INFO_T*   ptStateInfo = *(PSTATE_INFO_T*)pvArg;
	
	mcSHOW_DRVAPI_MSG(("TunerProcess %s is running!\n", ptStateInfo->fa_thread_name));
	ptStateInfo->e_fa_state = TUNER_STATE_DISCONNECTED;

    while (ptStateInfo->fgfaThreadRunning)
    {
        switch (ptStateInfo->e_fa_state)
		{
            case TUNER_STATE_DISCONNECTED:
            {
                mcSEMA_UNLOCK(ptStateInfo->t_fa_disconnected);   
                mcSEMA_LOCK(ptStateInfo->t_fa_connecting);
                break;
			}
            
            case TUNER_STATE_CONNECTING:
			{						
				vApiEepromDmaWrite(EEP_DEMOD_FASTBOOT_ENABLE, FA_W_Conn_Infor, VAR_FASTBOOT_NUM);
                FaStateChange(ptStateInfo, TUNER_STATE_CONNECTING, TUNER_STATE_CONNECTED);
				break;
            }
            case TUNER_STATE_CONNECTED:
			{
                mcSEMA_LOCK_TIMEOUT(ptStateInfo->t_fa_sync, cUNCLOCK_POLLING_TIME);
                break;
			}

            case TUNER_STATE_DISCONNECTING:
            {				
                FaStateChange(ptStateInfo, TUNER_STATE_DISCONNECTING, TUNER_STATE_DISCONNECTED);                
                mcSEMA_UNLOCK(ptStateInfo->t_fa_disconnecting);                
                vApiEepromDmaWrite(EEP_DEMOD_FASTBOOT_ENABLE, FA_W_Conn_Infor, VAR_FASTBOOT_NUM);                
                break;
			}
        } /* switch */

    } /* while */
	
    ptStateInfo->t_fa_thread = (THREAD_T)NULL;
    mcSHOW_DRVAPI_MSG(("TunerProcess %s exit!\n", ptStateInfo->fa_thread_name));
    mcTHREAD_DESTROY();	
}

/*
 * Description: Tuner Fast Acq API Breaker function
 * @Lei20130829
*/
INT32 _TunerFastAcq_Breaker(void)
{
    return FALSE;
}

/*
 * Description: get the flag:Is Boot up with channel? from the EEPROM
 * @Lei20130829
*/
BOOL Get_FgBootupWithChannel(void)
{
	return FgBootupWithChannel;
}

/*
 * Description: Format the Connect information
 * @Lei20130829
*/
static BOOL TunerFastAcq_FormatConnectInfo(UINT16 TunerId,DRV_CONN_TYPE_T e_conn_type, void *ptConnInfo,
    UINT32 u4FreqK, UINT8 _e_mod, UINT8 Conn_Infor[DEMOD_FASTBOOT_NUM])
{	
#ifdef CC_ATD_SUPPORT_FASTACQ
    UINT32 u4FreqK_B = 0;
#endif
#ifdef CC_DVBS_SUPPORT_FASTACQ
    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;
    FASTACQ_STATE_INFO_T*   pt_PreTune_Info = &t_PreTune_Info;
    UINT8 ui1_temp;
    INT32 i4_ret = 0;
#endif


	if(TunerId > MTK_DVBT_0) return FALSE;

    switch (e_conn_type)
    {
    case TUNER_CONN_TYPE_TER_DIG:
        {
            TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info;
            pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *)ptConnInfo;

            pt_tuner_info->ui4_freq = u4FreqK * 1000;
            pt_tuner_info->e_mod = _e_mod;
			pt_tuner_info->e_bandwidth           = Conn_Infor[DEMOD_FASTBOOT_PARAM_1];
			pt_tuner_info->e_hierarchy_priority  = Conn_Infor[DEMOD_FASTBOOT_PARAM_2];
			pt_tuner_info->e_conn_mode           = Conn_Infor[DEMOD_FASTBOOT_PARAM_3];
			pt_tuner_info->ui1_plp_id            = Conn_Infor[DEMOD_FASTBOOT_PARAM_4];
            break;
        }

    case TUNER_CONN_TYPE_CAB_DIG:
        {
            TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info;
            pt_tuner_info = (TUNER_CAB_DIG_TUNE_INFO_T *)ptConnInfo;

            pt_tuner_info->ui4_freq = u4FreqK * 1000;
			pt_tuner_info->e_mod = _e_mod;
            pt_tuner_info->ui4_sym_rate = mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_2], Conn_Infor[DEMOD_FASTBOOT_PARAM_1], 8, 8) * 1000;
			pt_tuner_info->e_conn_mode  = Conn_Infor[DEMOD_FASTBOOT_PARAM_3];
            break;
        }
#ifdef CC_DVBS_SUPPORT_FASTACQ
	case TUNER_CONN_TYPE_SAT_DIG:
		{			
            pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *)ptConnInfo;
						
            pt_tuner_info->ui4_freq = u4FreqK * 1000;
			pt_tuner_info->e_mod = _e_mod;
			pt_tuner_info->e_conn_mode			 = Conn_Infor[DEMOD_FASTBOOT_PARAM_1];
			pt_tuner_info->ui4_data              = (UINT32)mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_2], Conn_Infor[DEMOD_FASTBOOT_PARAM_3], 8, 8);
			/*  Addr                  Bit                Name
                     * -------------------------------------------------------
                     *  CONN_CONFIG   [1:0]          ui1_lnb_power
                     *  CONN_CONFIG   [2]             ui1_polarity_13v_18v
                     *  CONN_CONFIG   [3]             b_22k
                     *  CONN_CONFIG   [4]             ui1_tone_burst
                     *  CONN_CONFIG   [5]             e_conn_mode
                     *  CONN_CONFIG   [6]             b_canal_digital_ts
                     */
            ui1_temp = Conn_Infor[TUNER_PRETUNE_SAT_CONN_CONFIG];
            pt_PreTune_Info->t_conn_info.t_sat_tuner_info.b_canal_digital_ts = mcGET_FIELD(ui1_temp, mcMASK(7), 6);
            if(mcGET_FIELD(ui1_temp, mcMASK(6), 5) == 0)
            {
                pt_PreTune_Info->t_conn_info.t_sat_tuner_info.e_conn_mode = CONN_MODE_SURF;
                pt_PreTune_Info->t_diseqc_cmd.ui1_lnb_power = mcGET_FIELD(ui1_temp, mcMASK(3), 0);
                pt_PreTune_Info->t_diseqc_cmd.ui1_polarity_13v_18v = mcGET_FIELD(ui1_temp, mcMASK(3), 2);
                pt_PreTune_Info->t_diseqc_cmd.b_22k = mcGET_FIELD(ui1_temp, mcMASK(4), 3);
                pt_PreTune_Info->t_diseqc_cmd.ui1_tone_burst = mcGET_FIELD(ui1_temp, mcMASK(5), 4);
                pt_PreTune_Info->t_diseqc_cmd.aui1_cmd[0] = 0xE0;
                pt_PreTune_Info->t_diseqc_cmd.aui1_cmd[1] = 0x10;
                pt_PreTune_Info->t_diseqc_cmd.aui1_cmd[2] = 0x38;
                pt_PreTune_Info->t_diseqc_cmd.aui1_cmd[3] = Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_2];;
                pt_PreTune_Info->t_diseqc_cmd.ui1_cmd_len = 4;
                pt_PreTune_Info->t_diseqc_cmd.ui4_control_mask = Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_1];
            }
            else
            {
                pt_PreTune_Info->t_conn_info.t_sat_tuner_info.e_conn_mode = CONN_MODE_UNICABLE_SURF;
                pt_PreTune_Info->t_diseqc_cmd.ui4_control_mask = DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK;
                /*  Addr                  Bit                Name
                         * -------------------------------------------------------
                         *  CONN_CONFIG   [3:0]             e_bank
                         *  CONN_CONFIG   [7:4]             e_user_band
                         */
                ui1_temp = Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_5]; 
                pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_bank = mcGET_FIELD(ui1_temp, mcMASK(4), 0);
                pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_user_band = mcGET_FIELD(ui1_temp, mcMASK(8), 4);
                pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui4_transponder_freq = mcUNSIGN_2BYTE(Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_2], Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_1], 8, 8);
                pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_lnb_freq = mcUNSIGN_2BYTE(Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_4], Conn_Infor[TUNER_PRETUNE_SAT_DISEQC_PARAM_3], 8, 8);
                pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_band_freq = pt_PreTune_Info->t_conn_info.t_sat_tuner_info.ui4_freq;
            }
		   i4_ret = TunerSetAttribute(pt_PreTune_Info->TunerID, TUNER_SET_TYPE_DISEQC_CMD, (void *) &pt_PreTune_Info->t_diseqc_cmd, sizeof(DISEQC_BUS_CMD_T));
           if(DRVAPI_TUNER_OK == i4_ret)
           {
                pt_PreTune_Info->b_diseqccmd_ready = TRUE;
           }
           if(pt_PreTune_Info->t_conn_info.t_sat_tuner_info.e_conn_mode == CONN_MODE_UNICABLE_SURF)
           {
                mcDELAY_MS(50);
           }      
			break;
		}
#endif
	
	#ifdef CC_ATD_SUPPORT_FASTACQ
	    case TUNER_CONN_TYPE_TER_ANA:
		case TUNER_CONN_TYPE_TER_ANA_SCART_OUT:
	        {
	            TUNER_TER_ANA_TUNE_INFO_T *pt_tuner_info;
	            pt_tuner_info = (TUNER_TER_ANA_TUNE_INFO_T *)ptConnInfo;
	            
				u4FreqK_B  = Conn_Infor[DEMOD_FASTBOOT_PARAM_6];
	    		u4FreqK_B += (UINT32)mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_4], Conn_Infor[DEMOD_FASTBOOT_PARAM_5], 8, 8) * 256;
				pt_tuner_info->ui4_freq = u4FreqK * 1000;
				pt_tuner_info->e_mod = _e_mod;
				pt_tuner_info->b_fine_tune         = Conn_Infor[DEMOD_FASTBOOT_PARAM_1];
				pt_tuner_info->i2_fine_tune_offset = (INT16)mcSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_2], Conn_Infor[DEMOD_FASTBOOT_PARAM_3], 8, 8);
	    		pt_tuner_info->ui4_freq_bound      = u4FreqK_B * 1000;
				pt_tuner_info->ui4_tv_sys_mask     = mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_9], Conn_Infor[DEMOD_FASTBOOT_PARAM_10], 8, 8);
				pt_tuner_info->ui4_tv_sys_mask    += (UINT32)mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_7], Conn_Infor[DEMOD_FASTBOOT_PARAM_8], 8, 8)*256*256;
	            break;
	        }

	    case TUNER_CONN_TYPE_CAB_ANA:
	        {
	            TUNER_CAB_ANA_TUNE_INFO_T *pt_tuner_info;
	            pt_tuner_info = (TUNER_CAB_ANA_TUNE_INFO_T *)ptConnInfo;

	            u4FreqK_B  = Conn_Infor[DEMOD_FASTBOOT_PARAM_6];
	    		u4FreqK_B += (UINT32)mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_4], Conn_Infor[DEMOD_FASTBOOT_PARAM_5], 8, 8) * 256;
	            pt_tuner_info->ui4_freq = u4FreqK * 1000;
				pt_tuner_info->e_mod = _e_mod;
				pt_tuner_info->b_fine_tune         = Conn_Infor[DEMOD_FASTBOOT_PARAM_1];
				pt_tuner_info->i2_fine_tune_offset = (INT16)mcSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_2], Conn_Infor[DEMOD_FASTBOOT_PARAM_3], 8, 8);
	    		pt_tuner_info->ui4_freq_bound      = u4FreqK_B * 1000;
				pt_tuner_info->ui4_tv_sys_mask     = mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_9], Conn_Infor[DEMOD_FASTBOOT_PARAM_10], 8, 8);
				pt_tuner_info->ui4_tv_sys_mask    += (UINT32)mcUNSIGN_2BYTE(Conn_Infor[DEMOD_FASTBOOT_PARAM_7], Conn_Infor[DEMOD_FASTBOOT_PARAM_8], 8, 8)*256*256;
	            break;
	        }
	#endif

    default:
        mcSHOW_USER_CRI_MSG(("Invalid CONN_TYPE!\n"));
        return FALSE;
    }

    return TRUE;
}
/*
 * Description: Check the Input Source is TV source??
 * @Lei20131018
*/
BOOL  TunerFastCheckSource(void) 
{
    BOOL i4Ret = TRUE;
	UINT8 Source = 0xFF;

	vApiEepromDmaRead(EEP_DEMOD_FASTBOOT_LASTSRC, &Source, 1);
	if((Source!= SV_VS_DTV1)&&(Source!= SV_VS_ATD1))//EU_DTV is 0x48,EU_ATV is 0x8;
	{
		mcSHOW_USER_CRI_MSG(("[FASTACQ]Boot Input Source is not TV.\n"));
		FgBootupWithChannel = FALSE;
		i4Ret = FALSE;		
	}
	return i4Ret;
}
#ifdef CC_DVBS_SUPPORT_FASTACQ
static BOOL FastAcq_CheckDiseqccmd(DISEQC_BUS_CMD_T *pCmd)
{
    FASTACQ_STATE_INFO_T*   pt_PreTune_Info = &t_PreTune_Info; 

    if(pt_PreTune_Info->b_diseqccmd_ready == FALSE)
    {
        return FALSE;
    }

    if(pCmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK)
    {
        if((pCmd->t_unicable_info.ui4_transponder_freq != pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui4_transponder_freq)
            ||(pCmd->t_unicable_info.e_bank != pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_bank)
            ||(pCmd->t_unicable_info.e_user_band != pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.e_user_band)
            ||(pCmd->t_unicable_info.ui2_band_freq != pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_band_freq)
            ||(pCmd->t_unicable_info.ui2_lnb_freq != pt_PreTune_Info->t_diseqc_cmd.t_unicable_info.ui2_lnb_freq))
        {
            return FALSE;
        }
    }
    else
    {
        if((pCmd->ui1_polarity_13v_18v != pt_PreTune_Info->t_diseqc_cmd.ui1_polarity_13v_18v)
            ||(pCmd->ui1_lnb_power!= pt_PreTune_Info->t_diseqc_cmd.ui1_lnb_power)
            ||(pCmd->b_22k!= pt_PreTune_Info->t_diseqc_cmd.b_22k))
        {
            return FALSE;
        }
    }
    pt_PreTune_Info->b_diseqccmd_ready = FALSE;
    mcSHOW_DRVAPI_MSG(("[PreTune]Have Pre Set Diseqc Cmd!\n"));
    return TRUE;
}
#endif

/*
 * Description: Tuner Fast Acq without the MW call Tuner_Connect.
*  "CC_DEMOD_FASTACQ" or "CC_ATD_SUPPORT_FASTACQ" are defined in mt5880_xx_linux\mt5880_xx_linux.cfg
 * @Lei20130829
*/
BOOL  TunerFastAcq(void) 
{
    BOOL i4Ret = FALSE;

	TUNER_ACQ_ACTION_T	eActionType = ACQ_ACTION_DEFAULT;
	TUNER_ALL_INFO_T	tConnInfo;
	
	UINT8 FA_R_Conn_Infor[DEMOD_FASTBOOT_NUM]={0};//Fix warning	
	UINT16 _u1TunerId = MTK_DVBT_0;
	DRV_CONN_TYPE_T _e_conn_type =TUNER_CONN_TYPE_TER_DIG;
	UINT32 u4FreqK = 666000;
	UINT8 _e_mod   = 0;
	UINT8 ii  = 0;

	
	UINT8 u1FreqK_0,u1FreqK_1,u1FreqK_2;
	UINT32	u4TickCnt, u4ConnTime;

	u4TickCnt = os_get_sys_tick();

	vApiEepromDmaRead(EEP_DEMOD_FASTBOOT_ENABLE, FA_R_Conn_Infor, 1);
	if(FA_R_Conn_Infor[DEMOD_FASTBOOT_ENABLE]!= 0xA5)
	{
		mcSHOW_USER_CRI_MSG(("[FASTACQ]TunerFastAcq:Boot up without Channel.\n"));
		FgBootupWithChannel = FALSE;
		return i4Ret;
	}
	else
	{	
		mcSHOW_USER_CRI_MSG(("[FASTACQ]TunerFastAcq:Boot up with Channel.\n"));

		vApiEepromDmaRead(EEP_DEMOD_FASTBOOT_ENABLE, FA_R_Conn_Infor, DEMOD_FASTBOOT_NUM);

		_u1TunerId   = FA_R_Conn_Infor[DEMOD_FASTBOOT_TunerID];
		_e_conn_type = FA_R_Conn_Infor[DEMOD_FASTBOOT_E_CONN_TYPE]+ TUNER_CONN_TYPE(0);
		u1FreqK_2    = FA_R_Conn_Infor[DEMOD_FASTBOOT_Freq_2];
		u1FreqK_1    = FA_R_Conn_Infor[DEMOD_FASTBOOT_Freq_1];
		u1FreqK_0    = FA_R_Conn_Infor[DEMOD_FASTBOOT_Freq_0];	
		_e_mod       = FA_R_Conn_Infor[DEMOD_FASTBOOT_E_MOD];	

		u4FreqK =  u1FreqK_0;
	    u4FreqK += (UINT32)mcUNSIGN_2BYTE(u1FreqK_2, u1FreqK_1, 8, 8) * 256;

		mcSHOW_DRVERR_MSG(("[FASTACQ]TunerFastAcq:_u1TunerId = %d.\n",_u1TunerId));
		mcSHOW_DRVERR_MSG(("[FASTACQ]TunerFastAcq:_e_conn_type = %d.\n",_e_conn_type));
		mcSHOW_DRVERR_MSG(("[FASTACQ]TunerFastAcq:u4FreqK = %d.\n",u4FreqK));
		mcSHOW_DRVERR_MSG(("[FASTACQ]TunerFastAcq:_e_mod = %d.\n",_e_mod));
		for(ii=DEMOD_FASTBOOT_PARAM_1;ii< DEMOD_FASTBOOT_NUM;ii++)
			mcSHOW_DRVERR_MSG(("[FASTACQ]TunerFastAcq:Param_%d = %02X.\n",ii-DEMOD_FASTBOOT_PARAM_1+1,FA_R_Conn_Infor[ii]));
		
		if(TunerFastAcq_FormatConnectInfo(_u1TunerId,_e_conn_type, &tConnInfo, u4FreqK, _e_mod, FA_R_Conn_Infor)!= TRUE)
		{
			mcSHOW_USER_CRI_MSG(("[FASTACQ]TunerFastAcq:Connect infor ERROR!.\n"));
			FgBootupWithChannel = FALSE;
			return i4Ret;
		}
			
		TunerSetAttribute(_u1TunerId, TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS, (void *) &eActionType, sizeof(TUNER_ACQ_ACTION_T));

		i4Ret = TunerAcq(_u1TunerId, _e_conn_type, &tConnInfo, sizeof(TUNER_ALL_INFO_T), (x_break_fct)_TunerFastAcq_Breaker, NULL);

		FgBootupWithChannel = TRUE;

		u4ConnTime = (os_get_sys_tick()-u4TickCnt) * x_os_drv_get_tick_period();
		mcSHOW_DRVERR_MSG(("[FASTACQ]_FastAcqTunerThread Time = %dms\n", u4ConnTime));
		
	    return i4Ret;
	
	}

}

#endif
/***********************************************************************************/

#endif /* _X_DRVAPI_TUNER_C_ */

