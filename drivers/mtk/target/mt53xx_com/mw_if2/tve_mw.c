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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tve_mw.c,v $
 * $Revision: #1 $
 *
 * Description:
 *         Video Eecoder middleware driver API
 *---------------------------------------------------------------------------*/

#ifdef CC_SUPPORT_TVE
/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "general.h"
#include "video_def.h"
#include "vdo_misc.h"
#include "drv_vbi.h"
#include "drv_scart.h"
#include "drvcust_if.h"
#include "source_table.h"
#include "source_select.h"

#include "x_rm_dev_types.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_tv_enc.h"

#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_hamming_code.h"
#include "d_drv_cust_util.h"
#include "tvsrc_mw.h"
#include "avc_combi_mw.h"

#include "b2r_if.h"
#include "vdp_if.h"
#include "tve_if.h"
#include "srm_drvif.h"
//#include "../tve/tve_debug.h"
#define DEFINE_IS_LOG	TVE_IsLog
#include "mwif2_debug.h"

/* For Cache and DRAM consistence */
//#include "x_hal_926.h"

/* For number of component */
#include "drv_comp_id.h"

/* For VBI data bypass */
#include "drv_vbi_if.h"
#include "mttve_mwif.h"
#include "vdec_mw.h"

#include "mtvdo.h"
#include "mtb2r.h"
#include "mtb2r_mwif.h"
#include "mtdrvcust.h"
/*-----------------------------------------------------------------------------
                    Macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/* constants */
#define DRVN_TV_ENC    "tvenc"

/* macros */
#ifdef DEBUG

//#define TVE_DBG_MSG( m ...)  {LOG(5, m);}
#define TVE_DBG_MSG( m ...)  {printf(m);}


#else

#define TVE_DBG_MSG( m ...)  {LOG(2, m);}

#endif

#define DAC_1 0
#define DAC_2 1

/*-----------------------------------------------------------------------------
                    structures
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    Global Data declaration
 ----------------------------------------------------------------------------*/
static TveUsage_t _rTveUsed = {0xf, 0x0};
static BOOL _afgScartWithDtvSource[TVENC_COMP_NS];
static UINT16 _u2VidDec_id = 0;
static UINT8 _au1ScartOutSource[TVENC_COMP_NS];
static BOOL _afgTveMwEnabled[TVENC_COMP_NS];
static UINT8 _abTveMwConnected[TVENC_COMP_NS];
extern COMP_INFO_FOR_CHANNEL_T _rMChannelCompInfo;
extern COMP_INFO_FOR_CHANNEL_T _rPChannelCompInfo;
#ifdef CC_SUPPORT_MONITOR_OUT_TVE	
static BOOL _bBypassMonOut = TRUE;
#else
static BOOL _bBypassMonOut = FALSE;
#endif

/*-----------------------------------------------------------------------------
                    constants declaration
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    function prototypes
 ----------------------------------------------------------------------------*/
extern BOOL _VdpTest(UINT16 Dec_id);   //modify this for tve test

EXTERN void MW_MPV_NOTIFY(UINT16 u2CompId);
EXTERN UINT8 u1Mw_SetVideoSrc(UINT8 ucVdpId, UINT8 u1Src);


static INT32 _TveMwConnect(DRV_COMP_ID_T * pt_comp_id,
                           DRV_CONN_TYPE_T e_conn_type,
                           const VOID * pv_conn_info,
                           SIZE_T z_conn_info_len,
                           VOID * pv_tag, x_rm_nfy_fct pf_nfy);

static INT32 _TveMwDisconnect(DRV_COMP_ID_T * pt_comp_id,
                              DRV_DISC_TYPE_T e_disc_type,
                              const VOID * pv_disc_info,
                              SIZE_T z_disc_info_len);

static INT32 _TveMwGet(DRV_COMP_ID_T * pt_comp_id,
                       DRV_GET_TYPE_T e_get_type,
                       VOID * pv_get_info, SIZE_T * pz_get_info_len);

static INT32 _TveMwSet(DRV_COMP_ID_T * pt_comp_id,
                       DRV_SET_TYPE_T e_set_type,
                       const VOID * pv_set_info, SIZE_T z_set_info_len);

static void _TveMwNotifySigChg(UINT8 bPath, UINT8 u1SigState);
static void _TveMwEnableBypassMonOut(BOOL bBypMonOut);

INT32 MW_TveInit(void)
{
#ifndef CC_DRIVER_PROGRAM
    INT32 i4Ret;
    UINT32 u4Flags;
    UINT16 u2Idx;
    DRV_COMP_REG_T tTveCompReg;
    DRV_COMP_FCT_TBL_T tTveFctTbl;

    for(u2Idx=0;u2Idx<TVENC_COMP_NS;u2Idx++)
    {
        CHAR s_name[11];
        /* Setup component ID */
        tTveCompReg.e_type = DRVT_TV_ENC;
        tTveCompReg.e_id_type = ID_TYPE_IND;
        tTveCompReg.u.t_ind.ui2_id = u2Idx;
        tTveCompReg.u.t_ind.ui1_port = ALL_PORTS;
        tTveCompReg.u.t_ind.pv_tag = NULL;
    
        /* Registration Flags */
        u4Flags = 0;
    
        /* Setup component function table */
        tTveFctTbl.pf_rm_connect = _TveMwConnect;
        tTveFctTbl.pf_rm_disconnect = _TveMwDisconnect;
        tTveFctTbl.pf_rm_get = _TveMwGet;
        tTveFctTbl.pf_rm_set = _TveMwSet;
    
        x_snprintf( s_name, sizeof(s_name),"%s%d", DRVN_TV_ENC, u2Idx);
    
        /* Register component with the Resource Manager database */
        i4Ret = x_rm_reg_comp(&tTveCompReg,
                              1, 
                              1, 
                              s_name, 
                              u4Flags, 
                              &tTveFctTbl, 
                              NULL, 
                              0);
    
        if (i4Ret != RMR_OK)
        {
            TVE_DBG_MSG("MW_TveInit(): x_rm_reg_comp failed [%d]\n", i4Ret);
            return i4Ret;
        }

        _au1ScartOutSource[u2Idx] = SV_VS_MAX;
        _afgScartWithDtvSource[u2Idx] = FALSE;
        _afgTveMwEnabled[u2Idx] = FALSE;
        _abTveMwConnected[u2Idx] = 0;
    }

    MTTVE_Init();
    /* fixme: set tve enabled for bypass mode normally (not to powerdown dac) */
    MTTVE_SetEnable(MTTVE_1, TRUE); 
    MTTVE_SetEnable(MTTVE_1, FALSE); 

    MTTVE_RegSigChgNfyCb(_TveMwNotifySigChg);   //modify this for TVE test
	_TveMwEnableBypassMonOut(_bBypassMonOut);

    return (RMR_OK);
#else
    return 0;
#endif
}

static INT32 _TveMwConnect(DRV_COMP_ID_T * pt_comp_id,
                           DRV_CONN_TYPE_T e_conn_type,
                           const VOID * pv_conn_info,
                           SIZE_T z_conn_info_len,
                           VOID * pv_tag, x_rm_nfy_fct pf_nfy)
{

   
#ifndef CC_DRIVER_PROGRAM

    DRV_COMP_ID_T *prVSrcCompReg;

    // Check component type and id
    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->e_type != DRVT_TV_ENC)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->ui2_id >= TVENC_COMP_NS)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    // ????
    if (pt_comp_id->b_sel_out_port != FALSE)    // must select to input port
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    prVSrcCompReg = (DRV_COMP_ID_T *) pv_conn_info;

    TVE_DBG_MSG("*** Connect TVE%d to %u tick=%u***\n",
                 pt_comp_id->ui2_id,
                 prVSrcCompReg->e_type,
                 x_os_get_sys_tick());

    if(prVSrcCompReg->e_type == DRVT_VID_DEC) /* DTV source */
    {
        _afgScartWithDtvSource[pt_comp_id->ui2_id] = TRUE;
        _u2VidDec_id = prVSrcCompReg->ui2_id;
    }
    else if(prVSrcCompReg->e_type == DRVT_TV_DEC) 
    {
        _afgScartWithDtvSource[pt_comp_id->ui2_id] = FALSE;
    }

    _rTveUsed.TveReferenceCnt++;
    _abTveMwConnected[pt_comp_id->ui2_id] = _rTveUsed.TveReferenceCnt;

    return (RMR_OK);
#else
    return 0;
#endif

}

static INT32 _TveMwDisconnect(DRV_COMP_ID_T * pt_comp_id,
                              DRV_DISC_TYPE_T e_disc_type,
                              const VOID * pv_disc_info,
                              SIZE_T z_disc_info_len)
{
    DRV_COMP_ID_T *prVSrcCompReg;

#ifndef CC_DRIVER_PROGRAM

    INT32 i4RetCode = RMR_OK;

    prVSrcCompReg = (DRV_COMP_ID_T *) pv_disc_info;
    if (!prVSrcCompReg)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    // Check component type and id
    if (!pt_comp_id)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->e_type != DRVT_TV_ENC)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->ui2_id >= TVENC_COMP_NS)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    TVE_DBG_MSG("*** Disconnect TVE%d tick=%u***\n",
                 pt_comp_id->ui2_id, x_os_get_sys_tick());

    if (pt_comp_id->b_sel_out_port != FALSE)
    {
        i4RetCode = RMR_DRV_INV_DISC_INFO;
    }

    _abTveMwConnected[pt_comp_id->ui2_id] = 0;
    if(_afgScartWithDtvSource[pt_comp_id->ui2_id] == FALSE)
    {
        _au1ScartOutSource[pt_comp_id->ui2_id] = SV_VS_MAX;
        MTTVE_SetScartOutCtrl((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), &_au1ScartOutSource[pt_comp_id->ui2_id], &_rTveUsed);
    }

    _rTveUsed.TveReferenceCnt--;
    return (i4RetCode);
#else
    return 0;
#endif
}

static INT32 _TveMwGet(DRV_COMP_ID_T * pt_comp_id,
                       DRV_GET_TYPE_T e_get_type,
                       VOID * pv_get_info, SIZE_T * pz_get_info_len)
{
#ifndef CC_DRIVER_PROGRAM

    UINT8 u1ColSys;
    INT32 i4_return;

    if (pt_comp_id->ui2_id >= TVENC_COMP_NS)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    switch (e_get_type)
    {
    case TV_ENC_GET_TYPE_CTRL:
        if (*pz_get_info_len >= sizeof(TV_ENC_CTRL_T))
        {
            if (_afgTveMwEnabled[pt_comp_id->ui2_id] != FALSE)
            {
                    *((TV_ENC_CTRL_T *) pv_get_info) = TV_ENC_CTRL_ENABLE;
            }
            else
            {
                    *((TV_ENC_CTRL_T *) pv_get_info) = TV_ENC_CTRL_DISABLE;
            }

            *pz_get_info_len = sizeof(TV_ENC_CTRL_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
        break;
    case TV_ENC_GET_TYPE_FMT:
        if (*pz_get_info_len >= sizeof(TV_ENC_FMT_T))
        {
            *((TV_ENC_FMT_T *) pv_get_info) = TV_ENC_FMT_CVBS;

            *pz_get_info_len = sizeof(TV_ENC_FMT_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
        break;

    case TV_ENC_GET_TYPE_FMT_CAP:
        if (*pz_get_info_len >= sizeof(TV_ENC_FMT_CAP_INFO_T))
        {
            *((TV_ENC_FMT_CAP_INFO_T *) pv_get_info) = TV_ENC_FMT_CAP_CVBS;

            *pz_get_info_len = sizeof(TV_ENC_FMT_CAP_INFO_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
        break;

    case TV_ENC_GET_TYPE_TV_TYPE:
    {
        UINT8 ucFmt = TVE_MAX_FMT;
        MTTVE_GetFmt(MTTVE_1, &ucFmt);
        if(ucFmt == TVE_FMT_480I)
        {
            u1ColSys = TV_ENC_TV_TYPE_NTSC_M;
        }
        else
        {
            u1ColSys = TV_ENC_TV_TYPE_PAL_B;
        }

        if (*pz_get_info_len >= sizeof(TV_ENC_TV_TYPE_INFO_T))
        {
            /* temporarily always return the PAL format */
            *((TV_ENC_TV_TYPE_INFO_T *) pv_get_info) = (TV_ENC_TV_TYPE_INFO_T)u1ColSys;

            *pz_get_info_len = sizeof(TV_ENC_TV_TYPE_INFO_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
    }
        break;

    case TV_ENC_GET_TYPE_MV:
        if (*pz_get_info_len >= sizeof(TV_ENC_MV_INFO_T))
        {
            /* temporarily always return OFF */
            ((TV_ENC_MV_INFO_T *) pv_get_info)->e_mv_type =
                TV_ENC_MV_TYPE_OFF;
            ((TV_ENC_MV_INFO_T *) pv_get_info)->u.e_aps_type =
                TV_ENC_MV_APS_TYPE_OFF;

            *pz_get_info_len = sizeof(TV_ENC_MV_INFO_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
        break;

    case TV_ENC_GET_TYPE_CCI:
        if (*pz_get_info_len >= sizeof(TV_ENC_CCI_TYPE_T))
        {
            /* temporarily always return no restrict */
            *((TV_ENC_CCI_TYPE_T *) pv_get_info) =
                TV_ENC_CCI_TYPE_NO_RESTRICT;

            *pz_get_info_len = sizeof(TV_ENC_CCI_TYPE_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
        }
        break;
    case TV_ENC_GET_TYPE_ENC_CAP:
        if (*pz_get_info_len >= sizeof(TV_ENC_ENC_CAP_INFO_T))
        {
            if (_abTveMwConnected[pt_comp_id->ui2_id] == 1) /* the first one */
            {
                *((TV_ENC_ENC_CAP_INFO_T*) pv_get_info) = TV_ENC_ENC_CAP_VBI;
            }
            else /* later */
            {
                *((TV_ENC_ENC_CAP_INFO_T*) pv_get_info) = 0;
            }
            *pz_get_info_len = sizeof(TV_ENC_ENC_CAP_INFO_T);
            i4_return = RMR_OK;
        }
        else
        {
            i4_return = RMR_DRV_NOT_ENOUGH_SPACE;
    }
    break;
    default:
        i4_return = RMR_DRV_INV_GET_INFO;
    }

    return (i4_return);

#else /* !CC_DRIVER_PROGRAM */
    return 0;
#endif /* !CC_DRIVER_PROGRAM */
}

static INT32 _TveMwSet(DRV_COMP_ID_T * pt_comp_id,
                       DRV_SET_TYPE_T e_set_type,
                       const VOID * pv_set_info, SIZE_T z_set_info_len)
{
#ifndef CC_DRIVER_PROGRAM

    INT32 i4RetCode = RMR_DRV_INV_SET_INFO;

    i4RetCode = RMR_OK;


    if (pt_comp_id->ui2_id >= TVENC_COMP_NS)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    //TVE_DBG_MSG("*** TVE%d Set_type %d tick=%u***\n",
    //             pt_comp_id->ui2_id,
    //             e_set_type,
    //             x_os_get_sys_tick());

    switch (e_set_type)
    {
    case TV_ENC_SET_TYPE_CTRL:
        /* ignore set_type_ctrl enable if disconnected */
        if( _abTveMwConnected[pt_comp_id->ui2_id] == 0 && 
            (TV_ENC_CTRL_T)(UINT32)pv_set_info == TV_ENC_CTRL_ENABLE)
        {
            break;
        }

        if((TV_ENC_CTRL_T)(UINT32)pv_set_info == TV_ENC_CTRL_ENABLE)
        {
            if (_afgTveMwEnabled[pt_comp_id->ui2_id] != FALSE) 
            {
                break;
            }
            _afgTveMwEnabled[pt_comp_id->ui2_id] = TRUE;

            if(_afgScartWithDtvSource[pt_comp_id->ui2_id] != FALSE)
            {
                _au1ScartOutSource[pt_comp_id->ui2_id]=SV_VS_DTV1;
            }

            MTTVE_SetScartOutCtrl((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), &_au1ScartOutSource[pt_comp_id->ui2_id], &_rTveUsed);

            TVE_DBG_MSG("** TVE%d Set_type_ctrl_enable src = 0x%x tick=%u***\n",
                             pt_comp_id->ui2_id,
                             _au1ScartOutSource[pt_comp_id->ui2_id],
                             x_os_get_sys_tick());

            if(((pt_comp_id->ui2_id==TV_ENC_SCART_1)?(0):(1))== _rTveUsed.TveIsUsedBy)
            {
                if(_bBypassMonOut == FALSE)
                {
                    MTVDO_SetEnable(MTVDO_SUB, TRUE);
                }
                MTTVE_SetEnable(MTTVE_1, TRUE); 
            }
        }
        else if((TV_ENC_CTRL_T)(UINT32)pv_set_info == TV_ENC_CTRL_DISABLE)
        {
            if(_afgTveMwEnabled[pt_comp_id->ui2_id] == FALSE)
            {
                break;
            }
            _afgTveMwEnabled[pt_comp_id->ui2_id] = FALSE;
            if(_afgScartWithDtvSource[pt_comp_id->ui2_id] != FALSE)
            {
                _au1ScartOutSource[pt_comp_id->ui2_id] = SV_VS_MAX;
                MTTVE_SetScartOutCtrl((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), &_au1ScartOutSource[pt_comp_id->ui2_id], &_rTveUsed);
            }
            if(((pt_comp_id->ui2_id==TV_ENC_SCART_1)?(0):(1)) == _rTveUsed.TveIsUsedBy)
            {
                MTTVE_SetEnable(MTTVE_1, FALSE);
                if(_bBypassMonOut == FALSE)
                {
                if(_rPChannelCompInfo.u2_e_type == DRVT_UNKNOWN) /* sub path is not utilized as pip/pop */
                {
                    MTVDO_SetEnable(MTVDO_SUB, FALSE);
                }

                if(_afgScartWithDtvSource[pt_comp_id->ui2_id] != FALSE)
                {
                    MTB2R_SetInput(MTVDO_SUB, 0xff, 0);
                }

                if(_rPChannelCompInfo.u2_e_type == DRVT_UNKNOWN) /* sub path is not utilized as pip/pop */
                {
                    u1Mw_SetVideoSrc(MTVDO_SUB, SV_VD_MAX);
	                }
                }
                MTTVE_SetOutputFormat(MTTVE_OFMT_NONE);
                _rTveUsed.TveIsUsedBy = 0xf;
            }
            MTTVE_SetDacMuteMask(((pt_comp_id->ui2_id==TV_ENC_SCART_1)?(0):(1)), MTTVE_DAC_MUTE_MASK_DRV, TRUE);
            TVE_DBG_MSG("** TVE%d Set_type_ctrl_disable tick=%u***\n",
                             pt_comp_id->ui2_id,
                             x_os_get_sys_tick());
        }
        else if((TV_ENC_CTRL_T)(UINT32)pv_set_info == TV_ENC_CTRL_RESET)
        {
                _afgTveMwEnabled[pt_comp_id->ui2_id] = FALSE;
                if(_afgScartWithDtvSource[pt_comp_id->ui2_id] != FALSE)
                {
                    _au1ScartOutSource[pt_comp_id->ui2_id] = SV_VS_MAX;
                }
                MTTVE_Reset(MTTVE_1);
                MTTVE_SetEnable(MTTVE_1, FALSE);
        }
        else
        {
            i4RetCode = RMR_DRV_INV_SET_INFO;
        }
        break;

    case TV_ENC_SET_TYPE_FMT:
        //only support CVBS in 539x/536x
        if((TV_ENC_FMT_T)(UINT32)pv_set_info == TV_ENC_FMT_CVBS)
        {
            i4RetCode = RMR_OK;
        }
        else
        {
            i4RetCode = RMR_DRV_INV_SET_INFO;
        }
        break;
    case TV_ENC_SET_TYPE_TV_TYPE:
        if((TV_ENC_TV_TYPE_INFO_T)(UINT32)pv_set_info == TV_ENC_TV_TYPE_NTSC_M)
        {
            MTTVE_VideoFmt((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_480i);
            i4RetCode = RMR_OK;
        }
        else if((TV_ENC_TV_TYPE_INFO_T)(UINT32)pv_set_info <= TV_ENC_TV_TYPE_PAL_D)
        {
            MTTVE_VideoFmt((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_576i);
            i4RetCode = RMR_OK;
        }
        else
        {
            i4RetCode = RMR_DRV_INV_SET_INFO;
        }
        break;

    case TV_ENC_SET_TYPE_VBI_CONTENT:
        break;

    case TV_ENC_SET_TYPE_MV:
        break;

    case TV_ENC_SET_TYPE_CCI:
        {
            UINT8 bType;

            if (pv_set_info == NULL)
            {
                i4RetCode = RMR_DRV_INV_SET_INFO;
            }
            else
            {
                bType = (UINT8)*((UINT32 *)pv_set_info);
                if(MTDRVCUST_OptGet(eTVESupportAFD))
                {
                    if(MTTVE_SetCgmsaValue(MTTVE_1, bType) != MTR_OK)
                    {
                        i4RetCode = RMR_DRV_INV_SET_INFO;
                    }
                }
            }
        }		
        break;

//    case TV_ENC_SET_TYPE_VBI_DATA:
//        break;

    case TV_ENC_SET_TYPE_ENABLE_EU_VBI_FIELD_DATA_GEN:
        {
        UINT8 fgEnable;

         //TVE_DBG_MSG("*** ENABLE_EU_VBI_FIELD_DATA_GEN%x tick=%u***\r\n",
         //        fgEnable, x_os_get_sys_tick());

                if (pv_set_info == NULL)
                {
                    i4RetCode = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    fgEnable = (UINT8)*((BOOL*)pv_set_info);
					
					#ifndef CC_SUPPORT_TVE_NO_INSERT_TTX
                    if(MTTVE_SetVbiEnable((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_VBI_DATA_TTX, fgEnable) != MTR_OK)
                    {
                        i4RetCode = RMR_DRV_INV_SET_INFO;
                    }
					#endif
                    if(MTDRVCUST_OptGet(eTVESupportAFD) == 0)
                    {
                        if(MTTVE_SetVbiEnable((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_VBI_DATA_WSS , fgEnable) != MTR_OK)
                        {
                            i4RetCode = RMR_DRV_INV_SET_INFO;
                        }
                    }
                    #if 0
                    if(TVE_SetCcEnable(TVE_1, fgEnable) != TVE_SET_OK)
                    {
                        i4RetCode = RMR_DRV_INV_SET_INFO;
                    }
                    #endif
#ifdef CC_TVE_INSERT_VPS//  for vps
                    #if 0
                    if(TVE_SetVPSEnable(TVE_1, fgEnable) != TVE_SET_OK)
                    {
                    	i4RetCode = RMR_DRV_INV_SET_INFO;
                    }
                    #endif
#endif
                }
        }
        break;

    case TV_ENC_SET_TYPE_EU_VBI_FIELD_DATA:
        {
        TV_ENC_EU_VBI_FIELD_DATA_T* _tv_enc_eu_vbi_field_data;
        MT_RESULT_T i4RetValue;
        
                if (pv_set_info == NULL)
                {
                    i4RetCode = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    #if 0 //#ifndef LINUX_TURNKEY_SOLUTION
                    _tv_enc_eu_vbi_field_data = (TV_ENC_EU_VBI_FIELD_DATA_T*) pv_set_info;
                    #else                    
                    _tv_enc_eu_vbi_field_data = (TV_ENC_EU_VBI_FIELD_DATA_T*)x_mmap_share_mem ((void *)pv_set_info, sizeof (TV_ENC_EU_VBI_FIELD_DATA_T));
                    if(_tv_enc_eu_vbi_field_data==NULL || _tv_enc_eu_vbi_field_data==MAP_FAILED)
                    {
                        break;
                    }
                    #endif
                    _tv_enc_eu_vbi_field_data->i4_result_ttx = TV_ENC_EU_VBI_DATA_RESULT_TTX_OK;
                    _tv_enc_eu_vbi_field_data->i4_result_vps = TV_ENC_EU_VBI_DATA_RESULT_VPS_OK;
                    _tv_enc_eu_vbi_field_data->i4_result_wss = TV_ENC_EU_VBI_DATA_RESULT_WSS_OK;
                    _tv_enc_eu_vbi_field_data->i4_result_cc = TV_ENC_EU_VBI_DATA_RESULT_CC_OK;

                    //TVE_DBG_MSG("*** _tv_enc_eu_vbi_field_data->ui4_flag=%x tick=%u***\r\n",
                    // _tv_enc_eu_vbi_field_data->ui4_flag, x_os_get_sys_tick());
                    
					#ifndef CC_SUPPORT_TVE_NO_INSERT_TTX
                    if(_tv_enc_eu_vbi_field_data->ui4_flag & TV_ENC_EU_VBI_DATA_FLAG_TTX)
                    {
                        i4RetValue = MTTVE_SendVbiData((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_VBI_DATA_TTX, (UINT8*)_tv_enc_eu_vbi_field_data->t_ttx.at_lines_data, _tv_enc_eu_vbi_field_data->t_ttx.ui4_num_line);
                        if(i4RetValue == MTR_NOT_OK)
                        {
                            _tv_enc_eu_vbi_field_data->i4_result_ttx = TV_ENC_EU_VBI_DATA_RESULT_TTX_ERROR;
                        }
                        else if(i4RetValue == MTR_TVE_SET_VBI_OVERFLOW)
                        {
                            _tv_enc_eu_vbi_field_data->i4_result_ttx = TV_ENC_EU_VBI_DATA_RESULT_TTX_OVERFLOW;
                        }
                    }
					#endif
                    if(_tv_enc_eu_vbi_field_data->ui4_flag & TV_ENC_EU_VBI_DATA_FLAG_VPS)
                    {
#ifdef CC_TVE_INSERT_VPS//  for vps

						if(MTTVE_SendVbiData((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_VBI_DATA_VPS, (UINT8*)&(_tv_enc_eu_vbi_field_data->t_vps.aui1_vps_data), 13) != MTR_OK)
#endif                    
                        _tv_enc_eu_vbi_field_data->i4_result_vps = TV_ENC_EU_VBI_DATA_RESULT_VPS_ERROR;
                    }

                    if(_tv_enc_eu_vbi_field_data->ui4_flag & TV_ENC_EU_VBI_DATA_FLAG_WSS)
                    {
                        if(MTDRVCUST_OptGet(eTVESupportAFD) == 0)
                        {
                            if(MTTVE_SendVbiData((pt_comp_id->ui2_id==0)?(TV_ENC_SCART_1):(TV_ENC_SCART_2), MTTVE_VBI_DATA_WSS, (UINT8*)&(_tv_enc_eu_vbi_field_data->t_wss.ui2_wss), 2) != MTR_OK)
                            {
                                _tv_enc_eu_vbi_field_data->i4_result_wss = TV_ENC_EU_VBI_DATA_RESULT_WSS_ERROR;
                            }
                        }
                    }
                    #if 0
                    if(_tv_enc_eu_vbi_field_data->ui4_flag & TV_ENC_EU_VBI_DATA_FLAG_CC)
                    {
                        if(TVE_SendCc(TVE_1, _tv_enc_eu_vbi_field_data->t_cc.ui1_field_parity, (const UCHAR*)&(_tv_enc_eu_vbi_field_data->t_cc.ui2_cc_data), _tv_enc_eu_vbi_field_data->ui8_pts) != TVE_SET_OK)
                        {
                            _tv_enc_eu_vbi_field_data->i4_result_cc = TV_ENC_EU_VBI_DATA_RESULT_CC_ERROR;
                        }
                    }
                    #endif
                    if (_tv_enc_eu_vbi_field_data)
                    {
                        x_munmap_share_mem (_tv_enc_eu_vbi_field_data, sizeof (TV_ENC_EU_VBI_FIELD_DATA_T));
                    }
                }
        }
       break;

    default:
        i4RetCode = RMR_DRV_INV_SET_INFO;
        break;
    }

    return i4RetCode;
#else
    return 0;
#endif
}


static void _TveMwNotifySigChg(UINT8 bPath, UINT8 u1SigState)
{
    if(bPath == VDP_2)
    {
        if(u1SigState == SV_VDO_STABLE)
        {
            if(_afgScartWithDtvSource[(TV_ENC_SCART_1==_rTveUsed.TveIsUsedBy)?(0):(1)] != FALSE)
            {
               if(_VdpTest(_u2VidDec_id))       //modify this for tve test
               {
                 MW_MPV_NOTIFY(_u2VidDec_id);  
			   }
            }
        }
    }
}


void _TveSetScartOutSource(UINT8 u1scart_id, UINT8 u1source)
{
    if(u1scart_id < TVENC_COMP_NS)
    {
        _au1ScartOutSource[u1scart_id] = u1source;
    }
    return;
}


UINT8 _TveGetScartOutSource(UINT8 u1scart_id)
{
    if(u1scart_id < TVENC_COMP_NS)
    {
        return _au1ScartOutSource[u1scart_id];
    }
    return SV_VS_MAX;
}

BOOL _fgIsTveUsed(UINT8 u1scart_id)
{
    return (_rTveUsed.TveIsUsedBy == u1scart_id);
}
void _TveMwEnableBypassMonOut(BOOL bBypMonOut)
{
	MTTVE_EnableBypassMonOut(MTTVE_1, bBypMonOut);
}
#endif

