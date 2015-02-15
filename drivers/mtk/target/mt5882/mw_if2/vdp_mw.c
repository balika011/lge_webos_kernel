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
 * $RCSfile: vdp_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdp_mw.c
 *  Brief of file vdp_mw.c.
 *  Details of file vdp_mw.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

//#include "x_debug.h"
#define DEFINE_IS_LOG	VDP_IsLog
#include "mwif2_debug.h"

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "drv_comp_id.h"
#include "drv_name.h"
#include "d_drv_cust_util.h"
#include "u_drv_cust.h"
#include "x_vid_plane.h"
#include "x_assert.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_avc.h"
#include "vdec_mw.h"

#include "x_mm_common.h"
LINT_EXT_HEADER_END
#include "mtvdecex_mwif.h"
#include "mtdrvcust.h"
#include "mtpmx.h"
#include "mtvdo.h"
#include "mtvdo_mwif.h"
#include "tvsrc_mw.h"
#include "mtb2r.h"
#include "mtb2r_mwif.h"
#include "mtstc.h"
#include "mtswdmx.h"
#include "mtvdec.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define CC_DISP_FMT_ENUM_SIZE (VID_PLA_DISP_FMT_MAX)

#define VID_QA_MAX_SRC_TYPES       16

#ifndef ES_ID_UNKNOWN
#define ES_ID_UNKNOWN 0xFF
#endif

#define ES0  0

#ifndef CC_53XX_SWDMX_V2
#define CC_53XX_SWDMX_V2
#endif

#ifndef ES_ID_MAX
#if defined(CC_3D_MM_DS_SUPPORT) || defined(CC_FBM_TWO_FBP)
#define ES_ID_MAX 0x2
#else
#define ES_ID_MAX 0x1
#endif
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/** Brief of MW_VDP_T
 *  Details of MW_VDP_T (optional).
 */
typedef struct
{
    UCHAR ucConnect;
    UCHAR ucEnable;                                             
    UCHAR ucDtv;
    UCHAR ucBlank;

    /* for Super Freeze */
    UCHAR ucMemoMode;
    UCHAR ucPmxCompId;


    VID_PLA_MODE_T eMode;                                
    VID_PLA_DISP_FMT_T eFmt;
    DRV_TYPE_T  e_type;
    TV_DEC_TIMING_TYPE_T e_timing;

    DRV_CUSTOM_RESOLUTION_INFO_T rSrcRes;
    DRV_CUSTOM_RESOLUTION_INFO_T rOutRes;

    VID_PLA_VID_REGION_T rXlatOutput;           // display region converted from aspect ratio
    VID_PLA_VID_REGION_T rFullOutput;            // display region setting from mw
    VID_PLA_VID_REGION_T rFinalOutput;           // final display region set to driver (the combination of rXlatOutput and rFullOutput)
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    VID_PLA_VID_REGION_T rTrueFullOutput;
#endif
    VID_PLA_VID_REGION_T rXlatSource;           // source region converted from aspect ratio
    VID_PLA_VID_REGION_T rFullSource;            // source region setting from mw
    VID_PLA_VID_REGION_T rFinalSource;          // final source region set to driver (the combination of rXlatSource and rFullSource)

    UCHAR ucEsId;
    UCHAR ucEs2Id;          // to remember the second Es Id (MVC).
    UINT16 u2SrcCompId;
    UINT16 u2Src2CompId;    // to remember the second Src Comp Id (MVC).
    UINT16 u2TvdCompId;
    UINT32 u4FrameRate;
    UINT32 u4Interlace;
    VID_PLA_QV_INFO_T rQvInfo;
    VID_PLA_LB_DETECT_CONFIG_T rLBConfig;

    VID_PLA_SPLIT_SCRN_DEMO_T eDemo;

    VOID* pvEosTag;
    x_vid_pla_nfy_fct pfEosNfy;
#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
    VOID* pvATEosTag;
    x_vid_pla_nfy_fct pfAtEosNfy;     
#endif

#ifdef CC_3D_MM_DS_SUPPORT
    BOOL fgFrameSeq;
#endif    
    /*added by multiscreen interaction lianming.lin 2012-03-16*/
    x_vid_pla_src_nfy_fct   pfVdpSrcChg;
    VOID*                   pv_nfy_tag;
} MW_VDP_T;

/* add for CR 212789 */
typedef struct _MW_VDP_DISC_PARA_T
{
    DRV_COMP_ID_T    t_comp_id;
    DRV_DISC_TYPE_T  e_disc_type;
    DRV_COMP_ID_T    t_disc_info;
    SIZE_T           z_disc_info_len;
} MW_VDP_DISC_PARA_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define VID_QA_TBL_IS_END( pt_vid_qa_tbl )                 \
    ( pt_vid_qa_tbl->e_src_type == DRVT_UNKNOWN &&         \
      pt_vid_qa_tbl->pt_min_max_dft_lst == NULL )

#define VID_QA_TBL_EX_IS_END( pt_vid_qa_tbl )                     \
    ( pt_vid_qa_tbl->e_src_type == DRVT_UNKNOWN &&                \
      pt_vid_qa_tbl->e_pic_mode == VID_PLA_PIC_MODE_DEFAULT &&    \
      pt_vid_qa_tbl->pt_min_max_dft_lst == NULL )

#define VID_MIN_MAX_DFT_IS_END( pt_min_max_dft )                 \
    ( pt_min_max_dft->e_vid_qa_type == VID_QA_TYPE_UNKNOWN &&    \
      pt_min_max_dft->i4_min == 0 &&                             \
      pt_min_max_dft->i4_max == 0 &&                             \
      pt_min_max_dft->i4_default == 0 )

#define VERIFY_VDP_ID(id)                                   \
          do {                                              \
            if (id >= VDP_NS) { return; }     \
          } while (0)


#define MAX_ATOMIC_CMD_NUM 20
//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
EXTERN void MW_MPV_PlayDone_NOTIFY(UINT16 u2CompId);
EXTERN void MW_MPV_NOTIFY(UINT16 u2CompId);
EXTERN UCHAR MW_MPV_CompIdtoEsId(UINT16 u2CompId);
EXTERN BOOL avc_scart_is_from_tuner_src(DRV_TYPE_T e_type, UINT16 ui2_comp_id, DRV_TYPE_T* pe_src_type, UINT16* pui2_src_comp_id );
EXTERN MTVDECEX_SRC_T VdoSrcMap2Mtal(UINT8 u1Src);
//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

static INT32 _VdpConnect(DRV_COMP_ID_T*   pt_comp_id,
                        DRV_CONN_TYPE_T  e_conn_type,
                        const VOID*      pv_conn_info,
                        SIZE_T           z_conn_info_len,
                        VOID*            pv_tag,
                        x_rm_nfy_fct     pf_nfy);

static INT32 _VdpDisconnect(DRV_COMP_ID_T*   pt_comp_id,
                        DRV_DISC_TYPE_T  e_disc_type,
                        const VOID*      pv_disc_info,
                        SIZE_T           z_disc_info_len);

static INT32 _VdpGet(DRV_COMP_ID_T*  pt_comp_id,
                    DRV_GET_TYPE_T  e_get_type,
                    VOID*           pv_get_info,
                    SIZE_T*         pz_get_info_len);

static INT32 _VdpSet(DRV_COMP_ID_T*  pt_comp_id,
                    DRV_SET_TYPE_T  e_set_type,
                    const VOID*     pv_set_info,
                    SIZE_T          z_set_info_len);

static UINT32 _SetInput(UCHAR ucVdpId);

static UINT32 _VdpSetEnable(UCHAR ucVdpId);

static MTVDO_ARG_TYPE_T _VdpMwGetType2ArgType(UINT32 u4GetType);

static MTVDO_ARG_TYPE_T _VdpMwSetType2ArgType(UINT32 u4SetType);

static void _VdpPlayDoneNotify(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2);

static void _VdpAfdChangeNotify(UINT32 u4VdpId, UINT32 u4Afd,FBM_ASPECT_RATIO_T* prAspectRatio);

static UINT32 _VdpSourceChangeNotify(UCHAR ucVdpId, MTVDO_SOURCE_CHG_INFO_T rVdpSrcInfo);

static void _VdpResolutionChangeNotify(UCHAR ucVdpId);

static void _VdpBitRateChangeNotify(UCHAR ucVdpId);

static UINT32 _VdpXlatDispFmtCallBackMw(UCHAR ucVdpId);

static void _VdpMwCalculateFinalRegion(VID_PLA_VID_REGION_T *pXlatRegion, VID_PLA_VID_REGION_T *pFullRegion, VID_PLA_VID_REGION_T *pFinalRegion);

static MT_RESULT_T _VdpMwSetFinalSourceRegion(UCHAR ucVdpId, VID_PLA_VID_REGION_T *pRegion);

static MT_RESULT_T _VdpMwSetFinalOutputRegion(UCHAR ucVdpId, VID_PLA_VID_REGION_T *pRegion);

static DRVP_TYPE_T _VdpDrvCustParse(const DRV_CUSTOM_T* pt_drv_cust, VOID* pv_tag);

static DRV_CUSTOM_COLOR_SYS_TYPE_T _VdpGetDrvCustColorSys(UCHAR ucVdpId);

static DRV_CUST_INPUT_SRC_TYPE_T _VdpGetDrvCustInputSrcType(UCHAR ucVdpId, DRV_TYPE_T e_type);

static BOOL _VdpUpdateOverscan(UCHAR ucVdpId, UINT32* pui4_top, UINT32* pui4_bottom, UINT32* pui4_left, UINT32* pui4_right);

static void _VdpUpdateDispFmt(UCHAR ucVdpId);
#ifdef CC_53XX_SWDMX_V2
static void _VdpEosCallback(UINT32 u4VdpId, MTB2R_COND_T eCond, BOOL fgFastForward,UINT32 u4AttachedSrcId );
#else
static void _VdpEosCallback(UINT32 u4VdpId, MTB2R_COND_T eCond, BOOL fgFastForward);
#endif
static BOOL _VdpParseMinMaxDftTbl(UINT8 ui1_src_idx, VID_QA_MIN_MAX_DFT_T* pt_min_max_dft_tbl );

static INT32   _VdpSetDispRegion(UCHAR ucVdpId, const VOID* pv_set_info);
static INT32   _VdpSetDispFormat(UCHAR ucVdpId, const VOID* pv_set_info);
static INT32   _VdpSetOverScan(UCHAR ucVdpId, const VOID* pv_set_info);

void _MW_VDP_WSS625_CB(UCHAR ucVdpId, UINT16 u2Data);
void _MW_VDP_WSS525_CB(UCHAR ucVdpId, UINT16 u2Data);
void _MW_VDP_SCART_PIN_8_CB(UCHAR ucVdpId, DRV_CUSTOM_SCART_PIN_8_T t_scart_pin_8);
extern UINT8 MWGetScartPin8Status(UINT8 u1Path);

void MW_VDP_CallbackTrigger(UCHAR ucVdpId);
UCHAR MW_VDP_MEMO_MODE_GET(UCHAR ucVdpId);

extern UINT8 bHDMIInputType(void);

UINT8 bApiFlashPqUpdateQtyDft(void);

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static MW_VDP_T _rMwVdpDefault =
{
    0, 0, 
    0, 0,
    0, PMX_COMP_NS, 
    VID_PLA_NORMAL,
    VID_PLA_DISP_FMT_NORMAL,
    DRVT_UNKNOWN,
    TV_DEC_TIMING_UNKNOWN,
    { 0, 0 },
    { 0, 0 },
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    #ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    #endif
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},
    {0, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT},

    ES_ID_UNKNOWN,              // ucEsId
    ES_ID_UNKNOWN,              // ucEs2Id
    (UINT16)MPV_COMP_NS,        // u2SrcCompId
    (UINT16)MPV_COMP_NS,        // u2Src2CompId
    (UINT16)TVD_COMP_NS,
     0, 0,
    {VID_PLA_QV_INP_NULL, VID_PLA_QV_INP_NULL, VID_PLA_QV_INP_NULL, VID_PLA_QV_INP_NULL},
    {0, 0, 0, 0, 0},

    VID_PLA_SPLIT_SCRN_DEMO_OFF,
    NULL,
    0,

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
    NULL,
    0
#endif
};

MW_VDP_T _arMwVdp[VDP_COMP_NS]; 

static DRV_CUSTOM_VID_SRC_INFO_T _arMwVidSrcInfo[VDP_COMP_NS];


#ifdef CC_3D_MM_DS_SUPPORT
UCHAR _arMwComp3dInfo[MPV_COMP_NS];
#endif


static d_vid_pla_xlat_disp_fmt_ex_fct _pfXlatDispFmtExFct[CC_DISP_FMT_ENUM_SIZE];

static d_vid_pla_get_current_disp_fmt_fct _pfGetCurrentDispFmtFct;

static d_vid_pla_overscan_fct _pfOverscanFct;
static d_vid_pla_source_region_fct _pfVideoSrcRegionFct;
static d_vid_pla_Output_region_fct _pfVideoOutRegionFct;

//After discuss with AP, the function is not used now.
static d_vid_pla_update_overscan_tbl_fct _pfUpdateOverscanTbl;

static d_vid_pla_vid_qa_tbl_query_fct _pfVidQaTblQueryFct;

static BOOL fgVDPForceUnmute = NULL_HANDLE;

static BOOL fgVDPForceFreeze = FALSE;
static BOOL _fgVDPSuperFreezeDisc = FALSE;
static MW_VDP_DISC_PARA_T _rVDPDiscPara;

static UINT16 _ui2_pal_secam_wss = 0;

#ifdef CC_HYBRID_MODE
static BOOL fgEnableMwCb = FALSE;           
#else
static BOOL fgEnableMwCb = TRUE;
#endif

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
static HANDLE_T h_semaphore = NULL_HANDLE;
#endif
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
static INT32 _VdpDispFmtLockInit(VOID)
{
    INT32 i4_res = 0;
    i4_res = x_sema_create(& h_semaphore,
                           X_SEMA_TYPE_MUTEX,
                           X_SEMA_STATE_UNLOCK);
    
    return i4_res;
}

static INT32 _VdpDispFmtLock(VOID)
{
    INT32 i4_res = 0;
    
    i4_res = x_sema_lock(h_semaphore,
                         X_SEMA_OPTION_WAIT);
    
    return i4_res;
}

static INT32 _VdpDispFmtUnLock(VOID)
{
    INT32 i4_res = 0;
    i4_res = x_sema_unlock(h_semaphore);
    
    return i4_res;
}
#endif
UINT8 u1Mw_SetVideoSrc(UINT8 ucVdpId, UINT8 u1Src)
{
    BOOL ret;
    /*lint -save -e644 */
    UINT8  u1MonSrc=SV_VS_MAX;
    DRV_TYPE_T t_e_type;
    UINT16  ui2_comp_id;
    UINT16 u2MainCombiId;
    DRV_ID_T t_main_mon_comp;
    MTVDECEX_SRC_T eSrc, eMonSrc=MTVDECEX_SRC_MAX;
    COMP_INFO_FOR_CHANNEL_T *rChnPtr;

    /*lint restore */
    /* Combi related behavior here */
    ret = FALSE;

    if(u1Src != SV_VS_MAX)
    {        
        if(rGetVSrcCompFromIdx((UINT32)u1Src, 0, &t_e_type, &ui2_comp_id) == 0)
        {
            if(u1Src != SV_VS_MAX)
            {
                ret = avc_combi_is_a_combi_comp(t_e_type, ui2_comp_id, &u2MainCombiId);
            }
        }
    }


    rChnPtr = (ucVdpId == VDP_1)?(&_rMChannelCompInfo):(&_rPChannelCompInfo) ;

    if(ret)
    {
        /* Get Monitored Component */
        UNUSED(avc_combi_get_monitor_comp(u2MainCombiId, &t_main_mon_comp));

        /* translate monitored comp to nptv source */
        u1MonSrc = rGetVSrcMap(t_main_mon_comp.e_type, t_main_mon_comp.ui2_id);
        eMonSrc = VdoSrcMap2Mtal(u1MonSrc);            
		rChnPtr->t_mon_id = t_main_mon_comp  ;
		//Printf("SetMonitorSrc %d %d\n", rChnPtr->t_mon_id.e_type, rChnPtr->t_mon_id.ui2_id);
    }
	else
	{
		rChnPtr->t_mon_id.e_type = DRVT_UNKNOWN;
		rChnPtr->t_mon_id.ui2_id = 0xffff;
	}

    //Set Moinitor Src
    
    if(MTVDECEX_SetMonitorSrc((MTVDO_PATH_T)ucVdpId, eMonSrc) == MTR_NOT_OK)    
    {
        return FALSE;
    }	
    eSrc = VdoSrcMap2Mtal(u1Src); 		
    //Set Main/Sub src
    if(MTVDECEX_SetVideoSrc((MTVDO_PATH_T)ucVdpId, eSrc) == MTR_NOT_OK)    
    {    	
        return FALSE;
    }
    else
    {    	
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpConnect.
 */
//-----------------------------------------------------------------------------
static INT32 _VdpConnect(DRV_COMP_ID_T*   pt_comp_id,
                        DRV_CONN_TYPE_T  e_conn_type,
                        const VOID*      pv_conn_info,
                        SIZE_T           z_conn_info_len,
                        VOID*            pv_tag,
                        x_rm_nfy_fct     pf_nfy)
{
    LINT_SUPPRESS_BRACE(818)

    DRV_COMP_ID_T* prMpvCompReg;
    UCHAR ucVdpId;
    UCHAR ucPortId;
    UCHAR ucEsId;

    UINT16 u2SrcCompId;
    

    UNUSED(z_conn_info_len);
    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    /* check time shift super freeze status */
    if (fgVDPForceFreeze)
    {
        _fgVDPSuperFreezeDisc = FALSE;
        return (RMR_OK);
    }

    // Check component type and id
    if (pt_comp_id == NULL)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->e_type != DRVT_VID_PLANE)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    ucVdpId = (UCHAR) pt_comp_id->ui2_id;

    if (ucVdpId >= VDP_NS)
    {
        return RMR_DRV_INV_CONN_INFO;
    }
    if (ucVdpId > (VDP_COMP_NS + VDP_COMP_1))
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->b_sel_out_port)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    ucPortId = (UCHAR) pt_comp_id->u.ui1_inp_port;

    if (ucPortId >= VDP_MAX_INPORT_NS)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    prMpvCompReg = (DRV_COMP_ID_T*)pv_conn_info;

    if (prMpvCompReg == NULL)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    if (prMpvCompReg->e_type == DRVT_VID_DEC)
    {
        if (prMpvCompReg->ui2_id > (UCHAR)MPV_COMP_NS)
        {
           return (RMR_DRV_INV_CONN_INFO);
        }

        // handle connect
    //    ucEsId = MW_MpvCompIdtoEsId(prMpvCompReg->ui2_id);        // prMpvCompReg->ui2_id to ucEsId

        _arMwVdp[ucVdpId].e_type = prMpvCompReg->e_type;
        _arMwVdp[ucVdpId].e_timing = TV_DEC_TIMING_VIDEO;
        // In MVC playback, MW connect:
        // 1. VDP0 -> VDEC0 -> SWDMX0 -> Bagt0
        // 2. VDP0 -> VDEC1 -> SWDMX0 -> Bagt0
        u2SrcCompId = prMpvCompReg->ui2_id;
        if((_arMwVdp[ucVdpId].u2SrcCompId != MPV_COMP_NS) &&
           (_arMwVdp[ucVdpId].u2SrcCompId != u2SrcCompId))
        {
            if(_arMwVdp[ucVdpId].u2SrcCompId != MPV_COMP_NS)
            {
                LOG(0, "%s(L:%d), error, u2SrcCompId is already connected\n", __FUNCTION__, __LINE__);
            }
            _arMwVdp[ucVdpId].u2Src2CompId = u2SrcCompId;
        }
        else
        {
            _arMwVdp[ucVdpId].u2SrcCompId = u2SrcCompId;
        }
        ucEsId = MW_MPV_CompIdtoEsId(u2SrcCompId);
        if((_arMwVdp[ucVdpId].ucEsId != ES_ID_UNKNOWN) &&
           (_arMwVdp[ucVdpId].ucEsId != ucEsId))
        {
            if(_arMwVdp[ucVdpId].ucEs2Id != ES_ID_UNKNOWN)
            {
                LOG(0, "%s(L:%d), error, ucEs2Id is already connected\n", __FUNCTION__, __LINE__);
            }
            _arMwVdp[ucVdpId].ucEs2Id = ucEsId;
        }
        else
        {
        	_arMwVdp[ucVdpId].ucEsId = ucEsId;      
        }  

#if 0 // this code is move after u1Mw_SetVideoSrc because u1Mw_SetVideoSrc will set incorrect input
        if (_SetInput(ucVdpId) != VDP_SET_OK)
        {
           return (RMR_DRV_INV_CONN_INFO);
        }
#endif

#ifdef CC_SECOND_B2R_SUPPORT
        if(ucVdpId == 0)
        {
            if (u1Mw_SetVideoSrc(ucVdpId, SV_VS_DT1) == 0)
            {
                return (RMR_DRV_INV_CONN_INFO);
            }
        }
        else
        {
            if (u1Mw_SetVideoSrc(ucVdpId, SV_VS_DT2) == 0)
            {
                return (RMR_DRV_INV_CONN_INFO);
            }        
        }
#else

        // HD B2R.
        // if SKYPE is not defined, we only have 1 B2R, always connect to B2R1
        if (u1Mw_SetVideoSrc(ucVdpId, SV_VS_DT1) == 0)
        {
            return (RMR_DRV_INV_CONN_INFO);
        }
        #endif

#if 1 // this code is move after u1Mw_SetVideoSrc because u1Mw_SetVideoSrc will set incorrect input
        if (_SetInput(ucVdpId) != VDP_SET_OK)
        {
           return (RMR_DRV_INV_CONN_INFO);
        }
#endif

        _arMwVdp[ucVdpId].ucDtv = 1;
    }
    else
    if (prMpvCompReg->e_type == DRVT_TV_DEC)
    {
        DRV_TYPE_T e_type, e_src_type;
        UINT16 ui2_id, ui2_src_id;
        TV_DEC_TIMING_TYPE_T e_timing;
        // According to Alec, LH, Martin, 20070423
        DRV_CUSTOM_SCART_PIN_8_T t_scart_pin_8;

        _arMwVdp[ucVdpId].u2TvdCompId = prMpvCompReg->ui2_id;
        TvdNfyVdpResChgRegister(prMpvCompReg->ui2_id, ucVdpId);

        TvdGetCurrInputSrcInfo(prMpvCompReg->ui2_id, &e_type, &ui2_id, &e_timing);

        /* If source is SCART, should check if the real source is TUNER */
        if ( e_type == DRVT_AVC_SCART &&
             avc_scart_is_from_tuner_src( e_type, ui2_id, &e_src_type, &ui2_src_id ) )
        {
            _arMwVdp[ucVdpId].e_type = e_src_type;
            _arMwVdp[ucVdpId].e_timing = e_timing;
        }
        else
        {
            _arMwVdp[ucVdpId].e_type = e_type;
            _arMwVdp[ucVdpId].e_timing = e_timing;
        }

        _arMwVdp[ucVdpId].ucDtv = 0;

        _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_UNKNOWN;
        _VdpXlatDispFmtCallBackMw(ucVdpId);

        t_scart_pin_8 = (DRV_CUSTOM_SCART_PIN_8_T) MWGetScartPin8Status(ucVdpId);
	if((UINT32)t_scart_pin_8 != (UINT32)AVC_SCART_PIN_8_UNKNOWN)
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_PAL_SECAM_WSS_SCART;
            _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = t_scart_pin_8;
            _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = 0;
            _ui2_pal_secam_wss = 0;
            _MW_VDP_SCART_PIN_8_CB(ucVdpId, t_scart_pin_8);
        }

    }
    else
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    return (RMR_OK);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpDisconnect.
 */
//-----------------------------------------------------------------------------
static INT32 _VdpDisconnect(DRV_COMP_ID_T*   pt_comp_id,
                        DRV_DISC_TYPE_T  e_disc_type,
                        const VOID*      pv_disc_info,
                        SIZE_T           z_disc_info_len)
{
    LINT_SUPPRESS_BRACE(818)

    DRV_COMP_ID_T* prMpvCompReg;
    UCHAR ucVdpId;
    UCHAR ucPortId;

    UNUSED(e_disc_type);
    UNUSED(pv_disc_info);
    UNUSED(z_disc_info_len);

    /* check time shift super freeze status */
    if (fgVDPForceFreeze)
    {
        /* record disconnect parameter for CR 212789 */
        _fgVDPSuperFreezeDisc = TRUE;
        _rVDPDiscPara.t_comp_id = *pt_comp_id;
        _rVDPDiscPara.e_disc_type = e_disc_type;
        _rVDPDiscPara.t_disc_info = *(DRV_COMP_ID_T*)pv_disc_info;
        _rVDPDiscPara.z_disc_info_len = z_disc_info_len;
        return (RMR_OK);
    }
    else
    {
        _fgVDPSuperFreezeDisc = FALSE;
    }

    // Check component type and id
    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    if (pt_comp_id->e_type != DRVT_VID_PLANE)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    ucVdpId = (UCHAR) pt_comp_id->ui2_id;

    if (ucVdpId > (VDP_COMP_NS + VDP_COMP_1))
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    if (pt_comp_id->b_sel_out_port)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    ucPortId = (UCHAR) pt_comp_id->u.ui1_inp_port;

    if (ucPortId >= VDP_MAX_INPORT_NS)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    // handle disconnect
    if (_arMwVdp[ucVdpId].u2TvdCompId < TVD_COMP_NS)
    {
        TvdNfyVdpResChgRegister(_arMwVdp[ucVdpId].u2TvdCompId, VDP_COMP_NS);
    }
    _arMwVdp[ucVdpId].u2TvdCompId = (UINT16) TVD_COMP_NS;
    _arMwVdp[ucVdpId].u2SrcCompId = (UINT16) MPV_COMP_NS;

    // actually, it should use SrcCompId to clean correct connection
    _arMwVdp[ucVdpId].u2Src2CompId = (UINT16) MPV_COMP_NS;
    _arMwVdp[ucVdpId].ucEs2Id = ES_ID_UNKNOWN;

    _arMwVdp[ucVdpId].ucEsId = ES_ID_UNKNOWN;
    _arMwVdp[ucVdpId].e_type = DRVT_UNKNOWN;

    _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_UNKNOWN;
    _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = 0;
    _ui2_pal_secam_wss = 0;
    _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = DRV_CUSTOM_SCART_PIN_8_UNKNOWN;

	x_memset(&_arMwVdp[ucVdpId].rXlatSource, 0, sizeof(_arMwVdp[ucVdpId].rXlatSource));
	x_memset(&_arMwVdp[ucVdpId].rXlatOutput, 0, sizeof(_arMwVdp[ucVdpId].rXlatOutput));
	x_memset(&_arMwVdp[ucVdpId].rFullOutput, 0, sizeof(_arMwVdp[ucVdpId].rFullOutput));

	_arMwVdp[ucVdpId].rXlatSource.ui4_width = 10000;
	_arMwVdp[ucVdpId].rXlatSource.ui4_height = 10000;
	_arMwVdp[ucVdpId].rXlatOutput.ui4_width = 10000;
	_arMwVdp[ucVdpId].rXlatOutput.ui4_height = 10000;
	_arMwVdp[ucVdpId].rFullOutput.ui4_width = 10000;
	_arMwVdp[ucVdpId].rFullOutput.ui4_height = 10000;
	
    if (_SetInput(ucVdpId) != VDP_SET_OK)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    prMpvCompReg = (DRV_COMP_ID_T*)pv_disc_info;

    if (prMpvCompReg == NULL)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }

    if (prMpvCompReg->e_type == DRVT_VID_DEC)
    {
        if(u1Mw_SetVideoSrc(ucVdpId, SV_VS_MAX) == 0)
        {
            return (RMR_DRV_INV_CONN_INFO);
        }
        if (prMpvCompReg->ui2_id > (UCHAR)MPV_COMP_NS)
        {
            LOG(0, "disconnect with wrong MPV Comp Id(%d)\n", prMpvCompReg->ui2_id);
        }
    }

    return (RMR_OK);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpGet.
 */
//-----------------------------------------------------------------------------
static INT32 _VdpGet(DRV_COMP_ID_T*  pt_comp_id,
                    DRV_GET_TYPE_T  e_get_type,
                    VOID*           pv_get_info,
                    SIZE_T*         pz_get_info_len)
{
    LINT_SUPPRESS_BRACE(818)

    UCHAR ucVdpId;
    INT32 i4ReturnValue;

    // Check component type and id
    if (pt_comp_id == NULL)
    {
       return (RMR_DRV_INV_GET_INFO);
    }

    if (pt_comp_id->e_type != DRVT_VID_PLANE)
    {
       return (RMR_DRV_INV_GET_INFO);
    }

    ucVdpId = (UCHAR) pt_comp_id->ui2_id;

    if (ucVdpId > (VDP_COMP_NS + VDP_COMP_1))
    {
       return (RMR_DRV_INV_GET_INFO);
    }

    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_GET_INFO);
    }

    // Check get_info
    if (pv_get_info == NULL)
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    if (pz_get_info_len == NULL)
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    i4ReturnValue = RMR_OK;

    // Perform get opertion
    switch (e_get_type)
    {
        case VID_PLA_GET_TYPE_3D_MJC_ENABLE:
        {
            UINT8* peArg;
            peArg = (UINT8*) pv_get_info;

            if (*pz_get_info_len < sizeof(UINT8))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UINT8);
                MTVDO_GetCapability(ucVdpId, MTVDO_MJC3D_CAP, (UCHAR *)peArg);
            }
        }
        break;
        
        case VID_PLA_GET_TYPE_CTRL:
        {
            VID_PLA_CTRL_T* peArg;
            peArg = (VID_PLA_CTRL_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_CTRL_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_CTRL_T);
                if (_arMwVdp[ucVdpId].ucEnable)
                {
                    *peArg = VID_PLA_CTRL_ENABLE;
                }
                else
                {
                    *peArg = VID_PLA_CTRL_DISABLE;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_MODE:
        {
            VID_PLA_MODE_T* peArg;
            peArg = (VID_PLA_MODE_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_MODE_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_MODE_T);
                *peArg = (VID_PLA_MODE_T) _arMwVdp[ucVdpId].eMode;
            }
        }
        break;

        case VID_PLA_GET_TYPE_BG:
        {
            UINT32 u4Bg=0;
            VID_PLA_BG_COLOR_T* prArg;
            prArg = (VID_PLA_BG_COLOR_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_BG_COLOR_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_BG_COLOR_T);
                if(MTVDO_GetBg((MTVDO_PATH_T)ucVdpId, &u4Bg) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui1_r = (UINT8)((u4Bg >> 16) & 0xFF);
                    prArg->ui1_g = (UINT8)((u4Bg >> 8) & 0xFF);
                    prArg->ui1_b = (UINT8)(u4Bg & 0xFF);
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_DISP_REGION:
        {
            VID_PLA_VID_REGION_T* prArg;

            prArg = (VID_PLA_VID_REGION_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_VID_REGION_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_VID_REGION_T);
                *prArg = _arMwVdp[ucVdpId].rFullOutput;
            }
        }
        break;

        case VID_PLA_GET_TYPE_SRC_REGION:
        {
            VID_PLA_VID_REGION_T* prArg;
            prArg = (VID_PLA_VID_REGION_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_VID_REGION_T))
            {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_VID_REGION_T);
                *prArg = _arMwVdp[ucVdpId].rFullSource;
            }
        }
        break;

        case VID_PLA_GET_TYPE_FINAL_DISP_REGION:
        {
            VID_PLA_VID_REGION_T* prArg;
            prArg = (VID_PLA_VID_REGION_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_VID_REGION_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_VID_REGION_T);
                VDP_DBG_MSG("get final disp region\n");
                PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalOutput);
                *prArg = _arMwVdp[ucVdpId].rFinalOutput;
            }
        }
        break;

        case VID_PLA_GET_TYPE_FINAL_SRC_REGION:
        {
            VID_PLA_VID_REGION_T* prArg;
            prArg = (VID_PLA_VID_REGION_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_VID_REGION_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_VID_REGION_T);
                *prArg = _arMwVdp[ucVdpId].rFinalSource;
            }
        }
        break;

        case VID_PLA_GET_TYPE_OVER_SCAN_CLIPPER:
        {
            VID_PLA_OVER_SCAN_CLIPPER_T* prArg;
            MTVDO_OVERSCAN_REGION_T rOverScan;

            prArg = (VID_PLA_OVER_SCAN_CLIPPER_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_OVER_SCAN_CLIPPER_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_OVER_SCAN_CLIPPER_T);

                x_memset(&rOverScan,0,sizeof(MTVDO_OVERSCAN_REGION_T));
                if(MTVDO_GetOverScan((MTVDO_PATH_T)ucVdpId, (MTVDO_OVERSCAN_REGION_T *)&rOverScan) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui4_top = (UINT32) rOverScan.u4Top;
                    prArg->ui4_bottom = (UINT32) rOverScan.u4Bottom;
                    prArg->ui4_left = (UINT32) rOverScan.u4Left;
                    prArg->ui4_right = (UINT32) rOverScan.u4Right;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_OVERSCAN_ENABLE:
        {
            BOOL* pfgArg;

            pfgArg = (BOOL*) pv_get_info;

            if (*pz_get_info_len < sizeof(BOOL))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                UCHAR ucSupported=0;
                
                *pz_get_info_len = sizeof(BOOL);
                MTVDO_GetCapability(ucVdpId, MTVDO_OVERSCAN_CAP, (UCHAR *) &ucSupported);
                *pfgArg = (BOOL) ucSupported;
            }
        }
        break;
        case VID_PLA_GET_TYPE_ASP_RATIO_ENABLE:   
		{
			BOOL* pfgArg;
		
			pfgArg = (BOOL*) pv_get_info;
		
			if (*pz_get_info_len < sizeof(BOOL))
			{
				i4ReturnValue = RMR_DRV_INV_GET_INFO;
			}
			else
			{
			        UCHAR ucSupported=0;
				*pz_get_info_len = sizeof(BOOL);
				MTVDO_GetCapability(ucVdpId, MTVDO_ASP_RATIO_CAP, (UCHAR *) &ucSupported);
                            //*pfgArg = VDP_IsSupportAspectRatio(ucVdpId);
                            *pfgArg = ucSupported;
			}
		}
		break;   
            
        case VID_PLA_GET_TYPE_NON_LINEAR_SCALING:
        {
            BOOL* pfgArg;
            pfgArg = (BOOL*) pv_get_info;

            if (*pz_get_info_len < sizeof(BOOL))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                UCHAR ucSupported=0;
                *pz_get_info_len = sizeof(BOOL);
                MTVDO_GetCapability(ucVdpId, MTVDO_NONLINEAR_SCALING_CAP, (UCHAR *) &ucSupported);
                *pfgArg = (BOOL) ucSupported;
            }
        }
        break;

        case VID_PLA_GET_TYPE_3D_PANEL_TYPE:
        {
            VID_PLA_3D_PANEL_TYPE_T* prArg;
            prArg = (VID_PLA_3D_PANEL_TYPE_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_3D_PANEL_TYPE_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                UINT32 u4PanelType=0 ;
                
                *pz_get_info_len = sizeof(VID_PLA_3D_PANEL_TYPE_T);
                MTPMX_Get_Panel_Type(&u4PanelType);                
                *prArg = ((VID_PLA_3D_PANEL_TYPE_T) (u4PanelType));
            }
        }
        break;
        case VID_PLA_GET_TYPE_3D_FMT_CAPABILITY:
        {
            UINT32* prArg;
            prArg = (UINT32*) pv_get_info;

            if (*pz_get_info_len < sizeof(UINT32))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UINT32);
                MTVDO_Get3DFmtCap(ucVdpId, (UINT32 *)prArg);
                LOG(2,"[MW_3D_GET] Get3DFmtCap Path = %d, Arg = %d\n",ucVdpId,*(UINT32 *)prArg);
            }
        }
        break;
        case VID_PLA_GET_TYPE_3D_CTRL_CAPABILITY:
        {
            UINT32* prArg;
            prArg = (UINT32*) pv_get_info;

            if (*pz_get_info_len < sizeof(UINT32))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UINT32);
                MTVDO_Get3DCtrlCap(ucVdpId, (UINT32 *)prArg);
                LOG(2,"[MW_3D_GET] 3DCtrlCap Path = %d, Arg = %d\n",ucVdpId,*(UINT32 *)prArg);
            }
        }
        break;
        case VID_PLA_GET_TYPE_3D_NAVI_TAG:
        {
            UINT32* prArg;
            prArg = (UINT32*) pv_get_info;

            if (*pz_get_info_len < sizeof(UINT32))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {            
                UINT32 u4Tmp = (UINT32) MTVDO_TAG3D_2D ;
                *pz_get_info_len = sizeof(UINT32);
                MTVDO_GetTDTVGetNaviTag3D(ucVdpId, &u4Tmp);
                switch(u4Tmp) 
                {                
                    case MTVDO_TAG3D_2D:
                        *prArg = TV_DEC_TAG3D_2D;
                        break;
                    case MTVDO_TAG3D_MVC:
                        *prArg = TV_DEC_TAG3D_MVC;
                        break;
                    case MTVDO_TAG3D_FP:
                        *prArg = TV_DEC_TAG3D_FP;
                        break;
                    case MTVDO_TAG3D_FS:
                        *prArg = TV_DEC_TAG3D_FS;
                        break;
                    case MTVDO_TAG3D_TB:
                        *prArg = TV_DEC_TAG3D_TB;
                        break;
                    case MTVDO_TAG3D_SBS:
                        *prArg = TV_DEC_TAG3D_SBS;
                        break;
                    case MTVDO_TAG3D_REALD:
                        *prArg = TV_DEC_TAG3D_REALD;
                        break;
                    case MTVDO_TAG3D_SENSIO:
                        *prArg = TV_DEC_TAG3D_SENSIO;
                        break;
                    case MTVDO_TAG3D_LI:
                        *prArg = TV_DEC_TAG3D_LA;
                        break;
                    case MTVDO_TAG3D_TTDO:
                        *prArg = TV_DEC_TAG3D_TTDO;
                        break;
                    case MTVDO_TAG3D_NOT_SUPPORT:
                        *prArg = TV_DEC_TAG3D_NOT_SUPPORT;
                        break;
                    default:
                        return TV_DEC_TAG3D_2D;
                }
                LOG(2,"[MW_3D_GET] 3D_NAVI_TAG Arg = %d\n",*(UINT32 *)prArg);
            }
        }
        break;   
        case VID_PLA_GET_TYPE_3D_NAVI_AUTO_CHG_EN:      
            {
                UINT32* pu4Arg;
                pu4Arg = (UINT32*) pv_get_info;

                if (*pz_get_info_len < sizeof(UINT32))
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    *pz_get_info_len = sizeof(UINT32);

                    MTVDO_Get3DNavMode((UINT32 *) pu4Arg);
                    LOG(2,"[MW_3D_GET] 3DNavMode Arg = %d\n",*(UINT32 *)pu4Arg);
                }                
                break; 
            }
        case VID_PLA_GET_TYPE_3D_IMAGE_SAFETY_EN:    
            {
                UINT32* pu4Arg;
                pu4Arg = (UINT32*) pv_get_info;

                if (*pz_get_info_len < sizeof(UINT32))
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    *pz_get_info_len = sizeof(UINT32);

                    MTVDO_Get3DImgSafetyLvl((UINT32*)pu4Arg);
                    LOG(2,"[MW_3D_GET] 3DImgSafetyLvl Arg = %d\n",*(UINT32 *)pu4Arg);
                }                
                break;             
            }
      
        case VID_PLA_GET_TYPE_CAPABILITY:
        {
            UINT32* pu4Arg;
            pu4Arg = (UINT32*) pv_get_info;

            if (*pz_get_info_len < sizeof(UINT32))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UINT32);

                *pu4Arg = (VID_PLA_CAP_MODE_NORMAL | VID_PLA_CAP_MODE_BLANK |
                VID_PLA_CAP_BG | VID_PLA_CAP_DISP_REGION |
                VID_PLA_CAP_SRC_REGION | VID_PLA_CAP_BLENDING |
                VID_PLA_CAP_BRIGHTNESS | VID_PLA_CAP_CONTRAST |
                VID_PLA_CAP_HUE | VID_PLA_CAP_SATURATION |
                VID_PLA_CAP_CTI | VID_PLA_CAP_SHARPNESS |
                VID_PLA_CAP_ETI | VID_PLA_CAP_NR | VID_PLA_CAP_BLACK_LVL_EXT |
                VID_PLA_CAP_FLESH_TONE | VID_PLA_CAP_LUMA |
                VID_PLA_CAP_WHITE_PEAK_LMT |
                VID_PLA_CAP_OVER_SCAN_CLIPPER | VID_PLA_CAP_COLOR_GAIN |
                VID_PLA_CAP_COLOR_OFFSET | VID_PLA_CAP_DISP_FMT |
                VID_PLA_CAP_FORCE_UNMUTE_ON_NO_SIG |
                VID_PLA_CAP_H_POS | VID_PLA_CAP_V_POS );
            }
        }
        break;

        case VID_PLA_GET_TYPE_BLENDING:  // andrew ?? who use this
        case VID_PLA_GET_TYPE_BRIGHTNESS:
        case VID_PLA_GET_TYPE_CONTRAST:
        case VID_PLA_GET_TYPE_HUE:
        case VID_PLA_GET_TYPE_SATURATION:
        case VID_PLA_GET_TYPE_CTI:
        case VID_PLA_GET_TYPE_ETI:
        case VID_PLA_GET_TYPE_SHARPNESS:
        case VID_PLA_GET_TYPE_COLOR_SUPPRESS:
        case VID_PLA_GET_TYPE_NR:
        case VID_PLA_GET_TYPE_BLACK_LVL_EXT:
        case VID_PLA_GET_TYPE_WHITE_PEAK_LMT:
        case VID_PLA_GET_TYPE_FLESH_TONE:
        case VID_PLA_GET_TYPE_LUMA:
        case VID_PLA_GET_TYPE_3D_NR:
        case VID_PLA_GET_TYPE_WHITE_STRETCH:
        case VID_PLA_GET_TYPE_BLACK_STRETCH:
        case VID_PLA_GET_TYPE_LCDIM:    
        case VID_PLA_GET_TYPE_3D_MODE:
        case VID_PLA_GET_TYPE_3D_LR_SWITCH:     
        case VID_PLA_GET_TYPE_3D_PARALLAX:       
        case VID_PLA_GET_TYPE_3D_FLD_DEPTH:      
        case VID_PLA_GET_TYPE_3D_TO_2D: 
        case VID_PLA_GET_TYPE_3D_FPR: 
        case VID_PLA_GET_TYPE_3D_PROTRUDE_SCR:                       
        case VID_PLA_GET_TYPE_3D_DISTANCE_TV:                       
        case VID_PLA_GET_TYPE_3D_OSD_DEPTH:          
        {
            MTVDO_ARG_TYPE_T ucArgType;
            INT16 i2Val=0;
            UCHAR* pucArg;
            pucArg = (UCHAR*) pv_get_info;

            if (*pz_get_info_len < sizeof(UCHAR))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UCHAR);
                ucArgType = _VdpMwGetType2ArgType(e_get_type);

                if (MTVDO_GetArg(ucVdpId, ucArgType, &i2Val) != MTR_OK)    
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    *pucArg = (UCHAR)i2Val;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_REGION_CAPABILITY:
        {
            VID_PLA_VID_REGION_CAPABILITY_T* prArg;
            prArg = (VID_PLA_VID_REGION_CAPABILITY_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_VID_REGION_CAPABILITY_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_VID_REGION_CAPABILITY_T);

                switch (prArg->e_get_type)
                {
                    case VID_PLA_GET_TYPE_DISP_REGION:
                    {
                        UCHAR ucSupported=0;
                        MTVDO_GetCapability(ucVdpId, MTVDO_DOTBYDOT_CAP, (UCHAR *) &ucSupported);
                        if (ucSupported && (ucVdpId == VDP_1))
                        {
                            prArg->b_is_enable = 1;
                        }
                        else
                        {
                            prArg->b_is_enable = 0;
                        }

                        prArg->ui4_height_max = VDP_MAX_REGION_HEIGHT;
			   			prArg->ui4_height_min = 1000;
                        prArg->ui4_width_max = VDP_MAX_REGION_WIDTH;
			   			prArg->ui4_width_min = 1000;
                        prArg->ui4_x_max = 0;
                        prArg->ui4_x_min = 0;
                        prArg->ui4_y_max = 0;
                        prArg->ui4_y_min = 0;
                    }
                    break;

                    case VID_PLA_GET_TYPE_SRC_REGION:
                    {
                        UCHAR ucSupported=0, ucEnable=0;
                        
                        MTVDO_GetCapability(ucVdpId, MTVDO_DOTBYDOT_CAP, (UCHAR *) &ucSupported);
                        if (ucSupported && (ucVdpId == VDP_1))
                        {
                            prArg->b_is_enable = 1;
                        }
                        else
                        {
                            prArg->b_is_enable = 0;
                        }

                        MTVDO_GetScalerDispMode((UCHAR *) &ucEnable);
                        if (ucEnable == FALSE) // non-scaler dispmode
                        {
                            UINT32 u4MinVal=0 , u4MinHieght=0;
                            MTVDO_GetMinSrcRegionWidth(&u4MinVal);
                            MTVDO_GetMinSrcRegionHeight(&u4MinHieght);
                            
                                {
                                    #if defined(SUPPORT_TV3D_OVERSCAN)
                                    UINT32 u4Tmp ;                                    
                                    MTVDO_Get3DDoneByMJC(&u4Tmp);

                                    if(u4Tmp == TRUE)
                                    {
                                        //For zoom limitation.
                                        prArg->ui4_height_max = VDP_MAX_REGION_HEIGHT;
                                        prArg->ui4_height_min = 6000;
                                    }
                                    else
                                    #endif
                                    {
                                        prArg->ui4_height_max = VDP_MAX_REGION_HEIGHT;
                                        prArg->ui4_height_min = u4MinHieght;
                                    }
                                }

                            prArg->ui4_width_max = VDP_MAX_REGION_WIDTH;
                            if (_arMwVdp[ucVdpId].rXlatSource.ui4_width)
                            {
                                prArg->ui4_width_min = u4MinVal * VDP_MAX_REGION_WIDTH/_arMwVdp[ucVdpId].rXlatSource.ui4_width;
                            }
                            else
                            {
                                prArg->ui4_width_min = u4MinVal;
                            }

                                if (prArg->ui4_width_min >= VDP_MAX_REGION_WIDTH)
                                {
                                    prArg->ui4_width_min = VDP_MAX_REGION_WIDTH;
                                }

                            prArg->ui4_x_max = 0;
                            prArg->ui4_x_min = 0;
                            prArg->ui4_y_max = 0;
                            prArg->ui4_y_min = 0;
                        }
                        else // Scaler display mode
                        {
                            prArg->ui4_height_max = VDP_MAX_REGION_HEIGHT;
                            prArg->ui4_height_min = VDP_MAX_REGION_HEIGHT;
                            prArg->ui4_width_max = VDP_MAX_REGION_WIDTH;
                            prArg->ui4_width_min = VDP_MAX_REGION_WIDTH;
                            prArg->ui4_x_max = 0;
                            prArg->ui4_x_min = 0;
                            prArg->ui4_y_max = 0;
                            prArg->ui4_y_min = 0;
                        }
                    }
                    break;
                    
                    default:
                        break;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_MIN_MAX:
        {
            INT16 i2Min=0, i2Max=0;
            VID_PLA_MIN_MAX_INFO_T* prArg;
            BOOL bRet = TRUE;

            prArg = (VID_PLA_MIN_MAX_INFO_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_MIN_MAX_INFO_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_MIN_MAX_INFO_T);
                switch (prArg->e_get_type)
                {
                    case VID_PLA_GET_TYPE_BLENDING:
                    {
                        i2Min = 0;
                        i2Max = 255;
                        bRet = TRUE;
                    }
                    break;
                    
                    case VID_PLA_GET_TYPE_H_POSITION:
                    {
                        MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;
                        
                        MTVDECEX_GetPorchTune(ucVdpId, &eTuneType);
                        if(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_VGA)
                        {
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MIN, (UINT16*)&i2Min);
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MAX, (UINT16*)&i2Max);
                        }
                        else if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
                        {
                            i2Min = 0;
                            i2Max = 64;
                        }
                        else
                        {
                            UINT16 def=0 ;
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_DEFAULT, &def);
                            i2Min = (def>32)? def-32:0;
                            i2Max = (def>32)? def+32:def*2;
                        }
                        bRet = TRUE;
                    }
                    break;
                    
                    case VID_PLA_GET_TYPE_V_POSITION:
                    {
                        MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;
                        
                        MTVDECEX_GetPorchTune(ucVdpId, &eTuneType);
                        if(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_VGA)
                        {
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_MIN, (UINT16 *)&i2Min);
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_MAX, (UINT16 *)&i2Max);
                        }
                        else if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
                        {
                            i2Min = 0;
                            i2Max = 32;
                        }
                        else
                        {
                            UINT16 def=0 ;
                            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_DEFAULT, &def);
                            i2Min = (def>16)? def-16:0;
                            i2Max = (def>16)? def+16:def*2;
                        }
                        bRet = TRUE;
                    }
                    break;
                    
                    default:
                    {
                        MTVDO_UI_RANGE_T rUiRange;                            
                        MTVDO_ARG_TYPE_T ucArgType = _VdpMwGetType2ArgType(prArg->e_get_type);
                        
                        x_memset(&rUiRange, 0, sizeof(MTVDO_UI_RANGE_T));
                        bRet = (MTVDO_GetPeUiRangeParam(ucVdpId, ucArgType, &rUiRange) == MTR_OK) ? TRUE : FALSE;
                        i2Min = rUiRange.i4Min;
                        i2Max = rUiRange.i4Max;        
                    }
                    break;
                }

                if (bRet != TRUE)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui4_min_value = (UINT32)i2Min;
                    prArg->ui4_max_value = (UINT32)i2Max;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_MAX:
        {
            MTVDO_ARG_TYPE_T ucArgType;
            VID_PLA_MAX_INFO_T* prArg = (VID_PLA_MAX_INFO_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_MAX_INFO_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                MTVDO_UI_RANGE_T rUiRange;
                
                x_memset(&rUiRange, 0, sizeof(MTVDO_UI_RANGE_T));
                *pz_get_info_len = sizeof(VID_PLA_MAX_INFO_T);
                ucArgType = _VdpMwGetType2ArgType(prArg->e_get_type);

                if (MTVDO_GetPeUiRangeParam(ucVdpId, ucArgType, &rUiRange) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui4_max_value = (UINT32)rUiRange.i4Max;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_COLOR_GAIN:
        {
            VID_PLA_COLOR_GAIN_T* prArg = (VID_PLA_COLOR_GAIN_T*) pv_get_info;
            INT16 i2Gain=0;

            if (*pz_get_info_len < sizeof(VID_PLA_COLOR_GAIN_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_COLOR_GAIN_T);

                // Get R Gain
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_R_GAIN, &i2Gain) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_r_gain = (UINT16)i2Gain;
                }

                // Get G Gain
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_G_GAIN, &i2Gain) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_g_gain = (UINT16)i2Gain;
                }

                // Get B Gain
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_B_GAIN, &i2Gain) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_b_gain = (UINT16)i2Gain;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_COLOR_GAIN_MAX:
        case VID_PLA_GET_TYPE_COLOR_OFFSET_MAX:
        {
            MTVDO_UI_RANGE_T rUiRange;
            MTVDO_ARG_TYPE_T ucArgType;
            VID_PLA_COLOR_GAIN_T* prArg;

            prArg = (VID_PLA_COLOR_GAIN_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_COLOR_GAIN_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_COLOR_GAIN_T);
                ucArgType = _VdpMwGetType2ArgType(e_get_type);

                x_memset(&rUiRange, 0, sizeof(MTVDO_UI_RANGE_T));
                
                if (MTVDO_GetPeUiRangeParam(ucVdpId, ucArgType, &rUiRange) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_r_gain = (UINT16)rUiRange.i4Max;
                    prArg->ui2_g_gain = (UINT16)rUiRange.i4Max;
                    prArg->ui2_b_gain = (UINT16)rUiRange.i4Max;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_COLOR_GAIN_MIN:
        case VID_PLA_GET_TYPE_COLOR_OFFSET_MIN:
        {
            MTVDO_UI_RANGE_T rUiRange;
            MTVDO_ARG_TYPE_T ucArgType;
            VID_PLA_COLOR_GAIN_T* prArg;

            x_memset(&rUiRange, 0, sizeof(MTVDO_UI_RANGE_T));
            
            prArg = (VID_PLA_COLOR_GAIN_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_COLOR_GAIN_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_COLOR_GAIN_T);
                ucArgType = _VdpMwGetType2ArgType(e_get_type);

                if (MTVDO_GetPeUiRangeParam(ucVdpId, ucArgType, &rUiRange) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_r_gain = (UINT16)rUiRange.i4Min;
                    prArg->ui2_g_gain = (UINT16)rUiRange.i4Min;
                    prArg->ui2_b_gain = (UINT16)rUiRange.i4Min;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_COLOR_OFFSET:
        {
            VID_PLA_COLOR_GAIN_T* prArg;
            INT16 i2Offset=0;

            prArg = (VID_PLA_COLOR_GAIN_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_COLOR_GAIN_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_COLOR_GAIN_T);

                // Get R Offset
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_R_OFFSET, &i2Offset) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_r_gain = (UINT16)i2Offset;
                }

                // Get G Offset
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_G_OFFSET, &i2Offset) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_g_gain = (UINT16)i2Offset;
                }

                // Get B Offset
                if (MTVDO_GetArg(ucVdpId, MTVDO_ARG_B_OFFSET, &i2Offset) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
                else
                {
                    prArg->ui2_b_gain = (UINT16)i2Offset;
                }
            }
        }
        break;
        case VID_PLA_GET_TYPE_DISP_FMT:
        {
            VID_PLA_DISP_FMT_T* peArg;
            peArg = (VID_PLA_DISP_FMT_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_DISP_FMT_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_DISP_FMT_T);
                //MTAL need to remember the eFmt for each source , Adam  <-- should be fine to keep original code
                *peArg = _arMwVdp[ucVdpId].eFmt;
            }
        }
        break;

        case VID_PLA_GET_TYPE_LETTER_BOX_DETECT:
        {
            UCHAR* pucArg;
            UCHAR ucEnable=0 ;
            pucArg = (UCHAR*) pv_get_info;
            MTVDO_GetLBoxEnable((MTVDO_PATH_T)ucVdpId, &ucEnable);

            if (*pz_get_info_len < sizeof(UCHAR))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UCHAR);
                //*pucArg = _arMwVdp[ucVdpId].ucLbEnable;
                *pucArg = ucEnable ;
            }
        }
        break;

        case VID_PLA_GET_TYPE_DYNAMIC_SCALING:
        {
            UINT16* pu2Arg;
            UINT32 u4Delay=0;
            pu2Arg = (UINT16*) pv_get_info;
            MTVDO_GetDynamicScale((MTVDO_PATH_T)ucVdpId, &u4Delay);

            if (*pz_get_info_len < sizeof(UINT16))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(UINT16);
                *pu2Arg = u4Delay ;
            }
        }
        break;

        case VID_PLA_GET_TYPE_SPLIT_SCRN_DEMO:
        {
            VID_PLA_SPLIT_SCRN_DEMO_T* peArg;
            peArg = (VID_PLA_SPLIT_SCRN_DEMO_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_SPLIT_SCRN_DEMO_T))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_SPLIT_SCRN_DEMO_T);
                *peArg = _arMwVdp[ucVdpId].eDemo;   
            }
        }
        break;

        case VID_PLA_GET_TYPE_FILM_MODE:
        {
            UCHAR* pucArg;
            INT16 u1FilmOnOff=0;

            pucArg = (UCHAR*) pv_get_info;
            MTVDO_GetArg(ucVdpId, MTVDO_ARG_DI_FILM_MODE, &u1FilmOnOff);

            if (*pz_get_info_len < sizeof(BOOL))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(BOOL);
                *pucArg = u1FilmOnOff;
            }
        }
        break;

        case VID_PLA_GET_TYPE_H_POSITION:
        {
            UINT16 pos=0,min_off=0,max=0,min=0,def=0;

            MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;
            MTVDECEX_GetPorchTune(ucVdpId, &eTuneType);

            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_DEFAULT, &def);
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MAX, &max);
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MIN, &min);
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_CURRENT, &pos);

            if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
            {
                *(UINT16*)pv_get_info=(UINT16)MAX(32+pos-def,0);
            }
            else
            {
                if(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_VGA)
                {
                    min_off=pos-min;
                    pos=max-min_off;
                }
                else
                {
                    min_off=pos-def;
                    pos=def-min_off;
                }
                if (pos<min)
                {
                    pos=min;
                }
                if (pos>max)
                {
                    pos=max;
                }
                *(UINT16*)pv_get_info = (UINT16)pos;
            }
            *pz_get_info_len = sizeof(UINT16);
        }
        break;
        
        case VID_PLA_GET_TYPE_V_POSITION:
        {
            MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;
            MTVDECEX_GetPorchTune((MTVDO_PATH_T)ucVdpId, &eTuneType);

            if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
            {
                UINT16 def=0,pos=0;
                MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_DEFAULT, &def);
                MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_CURRENT, &pos);
                *(UINT16*)pv_get_info = (UINT16)MAX(16+pos-def,0);
            }
            else
            {
                MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_CURRENT, (UINT16 *) pv_get_info);
            }
            *pz_get_info_len = sizeof(UINT16);
        }
        break;

        case VID_PLA_GET_TYPE_FORCE_UNMUTE_ON_NO_SIG:
        {
            if ( !pz_get_info_len || *pz_get_info_len < sizeof(BOOL) ||
                 !pv_get_info )
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
                break;
            }
            *(BOOL*)pv_get_info = fgVDPForceUnmute;
        }
        break;

    case VID_PLA_GET_TYPE_HDMI_MODE:
        {
            MTVDECEX_HDMI_COLOR_MODE_T eHdmiColorMode=MTVDECEX_HDMI_MODE_AUTO ;
            MTVDECEX_HDMI_GetColorMode(ucVdpId, &eHdmiColorMode);
            switch(eHdmiColorMode)
            {
                case MTVDECEX_HDMI_MODE_GRAPHIC:
                    *(UINT8*)pv_get_info = VID_PLA_HDMI_MODE_GRAPHIC ;
                    break;
                case MTVDECEX_HDMI_MODE_VIDEO:
                    *(UINT8*)pv_get_info = VID_PLA_HDMI_MODE_VIDEO ;
                    break;
                case MTVDECEX_HDMI_MODE_AUTO:
                default:
                    *(UINT8*)pv_get_info = VID_PLA_HDMI_MODE_AUTO ;
                    break;
            }
            *pz_get_info_len = sizeof(UINT8);
        }
        break;
    case VID_PLA_GET_TYPE_ENHANCE:
    case VID_PLA_GET_TYPE_QV_INP:
    case VID_PLA_GET_TYPE_LB_DETECT_CONFIG:
    {
        VID_PLA_LB_DETECT_CONFIG_T* peArg;
        peArg = (VID_PLA_LB_DETECT_CONFIG_T*) pv_get_info;

        if (*pz_get_info_len < sizeof(VID_PLA_LB_DETECT_CONFIG_T))
        {
            i4ReturnValue = RMR_DRV_INV_GET_INFO;
        }
        else
        {
            *pz_get_info_len = sizeof(VID_PLA_LB_DETECT_CONFIG_T);
            *peArg = _arMwVdp[ucVdpId].rLBConfig;
        }
    }
    break;
    
    case VID_PLA_GET_TYPE_MODE_CAPABILITY:
    {
        UCHAR ucSupported=0;
        switch( *(VID_PLA_MODE_T*)pv_get_info )
        {
            case VID_PLA_NORMAL:
            case VID_PLA_BLANK:
            case VID_PLA_QUAD_VIDEO:
            case VID_PLA_DEINT:
            default:
                break;
            case VID_PLA_FREEZE:

                MTVDO_GetCapability(ucVdpId, MTVDO_FREEZABLE_CAP, (UCHAR *) &ucSupported);

                if(ucSupported == FALSE)
                {
                    i4ReturnValue = RMR_DRV_GET_FAILED;
                }
                break;
        }
    }
    break;

        case VID_PLA_GET_TYPE_PICTURE_INFO:
        {
            VID_PLA_PICTURE_INFO_T* prArg;
            MTB2R_PIC_INFO_T rVdpPicInfo;
            #ifdef ENABLE_MULTIMEDIA
            UINT32 u4Tmp=0;
            UINT64 u8Tmp=0;
            #endif

            prArg = (VID_PLA_PICTURE_INFO_T*) pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PLA_PICTURE_INFO_T))
            {
                i4ReturnValue = RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PLA_PICTURE_INFO_T);

                x_memset(&rVdpPicInfo,0,sizeof(MTB2R_PIC_INFO_T));
                if (MTB2R_GetPicInfo(ucVdpId, &rVdpPicInfo) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                    #ifdef ENABLE_MULTIMEDIA
#ifdef CC_53XX_SWDMX_V2
                    if(MTSWDMX_GetInfo(MTSWDMX_PRIMARY_SOURCE, MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#else
                    if(MTSWDMX_GetInfo(MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#endif
                    {
                        if((u4Tmp == (UINT32)MTSWDMX_FMT_H264_VIDEO_ES) ||
                           (u4Tmp == (UINT32)MTSWDMX_FMT_MPEG2_VIDEO_ES) ||
                           (u4Tmp == (UINT32)MTSWDMX_FMT_MPEG4_VIDEO_ES) ||
                           (u4Tmp == (UINT32)MTSWDMX_FMT_VC1_ES))
                        {
#ifdef CC_53XX_SWDMX_V2
                            if(MTSWDMX_GetInfo(MTSWDMX_PRIMARY_SOURCE, MTSWDMX_GET_CURRENT_POSITION, (UINT32*)&u8Tmp, 0, 0) == MTR_OK)
#else
                            if(MTSWDMX_GetInfo(MTSWDMX_GET_CURRENT_POSITION, (UINT32*)&u8Tmp, 0, 0) == MTR_OK)
#endif
                            {
                                prArg->ui8_offset = u8Tmp;
                                i4ReturnValue = RMR_OK;
                            }
                        }
                    }
                    #endif
                }
                else
                {
                    prArg->ui8_offset = rVdpPicInfo.u8OffsetDisp;
                }

                if (prArg->e_type == VID_PLA_PIC_INFO_TYPE_PICTURE)
                {
                    prArg->u.ui8_pts = (UINT64)rVdpPicInfo.u4Pts;
                    #ifdef ENABLE_MULTIMEDIA
#ifdef CC_53XX_SWDMX_V2
                    if(MTSWDMX_GetInfo(MTSWDMX_PRIMARY_SOURCE, MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#else
                    if(MTSWDMX_GetInfo(MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#endif
                    {
                        if(u4Tmp == (UINT32)MTSWDMX_FMT_MPEG2_TS_TIME_SHIFT)
                        {
                            prArg->u.ui8_pts = (UINT64)rVdpPicInfo.u4TickNum;
                        }
                    }
                    #endif
                }
                else if (prArg->e_type == VID_PLA_PIC_INFO_TYPE_GOP)
                {
                    prArg->u.t_time_code.ui1_hours = rVdpPicInfo.ucHours;
                    prArg->u.t_time_code.ui1_minutes = rVdpPicInfo.ucMinutes;
                    prArg->u.t_time_code.ui1_seconds = rVdpPicInfo.ucSeconds;
                    prArg->u.t_time_code.ui1_pictures = rVdpPicInfo.ucPictures;
                    prArg->u.t_time_code.i4_total_dur_adj = (INT32)rVdpPicInfo.u4TotlaTimeOffset;
                }
                else
                {
                    i4ReturnValue = RMR_DRV_INV_GET_INFO;
                }
            }
        }
        break;

        case VID_PLA_GET_TYPE_PROGRESS_SEQ:
        {
            VID_PROGRESS_SEQ_INFO_T* prArg;
            MTVDEC_HDR_INFO_T r_hdr_info;
            prArg = (VID_PROGRESS_SEQ_INFO_T*)pv_get_info;

            if (*pz_get_info_len < sizeof(VID_PROGRESS_SEQ_INFO_T))
            {
                i4ReturnValue = RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = sizeof(VID_PROGRESS_SEQ_INFO_T);

                //if (MTB2R_GetPicInfo(ucVdpId, &rVdpPicInfo) != VDP_SET_OK)
                x_memset(&r_hdr_info,0,sizeof(MTVDEC_HDR_INFO_T));
                if (MTVDEC_GetInfo(ES0, &r_hdr_info) == MTR_OK)
                {     
                    prArg->ui4_progress_seq = (UINT32)r_hdr_info.fgProgressiveSeq;
                }
            }
        }
        break;

#ifdef CC_FLIP_MIRROR_SUPPORT
        case VID_PLA_GET_TYPE_FLIP:
        {
            UINT32 u4Status=0;    
            if (MTVDO_GetMirrorFlip(0, &u4Status) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            *(BOOL*)pv_get_info = (BOOL)u4Status;
        }
        break;

        case VID_PLA_GET_TYPE_MIRROR:
        {
            UINT32 u4Status=0;    
            if (MTVDO_GetMirrorFlip(1, &u4Status) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            *(BOOL*)pv_get_info = (BOOL)u4Status;
        }
        break;
#endif
        case VID_PLA_GET_TYPE_DRAMINF:
            {
                    CAP_INPUT_T* peArg;
                    DRAM_INFO_T draminf ;
                    peArg = (CAP_INPUT_T*) pv_get_info;

                    if (*pz_get_info_len < sizeof(CAP_INPUT_T))
                    {
                        i4ReturnValue = RMR_DRV_INV_GET_INFO;
                    }
                    else
                    {
                        *pz_get_info_len = sizeof(CAP_INPUT_T);
                         //VDP_GetDramInf(peArg->ui1_video_path,&draminf);
                        if ( MTVDO_GetDramInfo((MTVDO_PATH_T)peArg->ui1_video_path,(MTVDO_DRAM_INFO_T *)& draminf) != MTR_OK)
                        {
                            i4ReturnValue = RMR_DRV_INV_GET_INFO;
                            break;
                        }
                        peArg->ui1_CfgFlipMirror = (draminf.scaler_inf.dram_from >> 4);
                        peArg->ui1_data_src = draminf.scaler_inf.dram_from & 0x0F;
                        if(peArg->ui1_data_src == 3)               //data from scaler
                        {
                            peArg->ui1_out_mode = 1; //only progressive
                            peArg->ui4_src_width = draminf.scaler_inf.PictureWidth;
                            peArg->ui4_src_height = draminf.scaler_inf.PictureHeight;
                            peArg->ui4_src_pitch = draminf.scaler_inf.DramPitch;                            
                            peArg->ui4_u_start_lsb_add = draminf.scaler_inf.ULsbAddr;
                            peArg->ui4_u_start_msb_add = draminf.scaler_inf.UMsbAddr;
                            peArg->ui4_y_start_lsb_add = draminf.scaler_inf.YLsbAddr;
                            peArg->ui4_y_start_msb_add = draminf.scaler_inf.YMsbAddr;
                            peArg->ui4_v_start_lsb_add = draminf.scaler_inf.VLsbAddr;
                            peArg->ui4_v_start_msb_add = draminf.scaler_inf.VMsbAddr;

                            peArg->ui1_block_mode = draminf.scaler_inf.BlockMode;
                            peArg->ui1_bit_res = draminf.scaler_inf.u1BitRes;
                            peArg->ui1_color_mode = draminf.scaler_inf.u1ColorMode;

                            peArg->ui1_y_frame_num = draminf.scaler_inf.FrameNum;
                            peArg->ui1_c_frame_num = draminf.scaler_inf.FrameNum;
                            
							peArg->ui1_CfgFlipMirror = draminf.scaler_inf.u1CfgFlipMirror;
                            peArg->ui1_DramFlipMirror = draminf.scaler_inf.u1DramFlipMirror;
                            peArg->b_dram_snake_map = draminf.scaler_inf.bDramSnakeMapping;
                        }
                        else if(peArg->ui1_data_src == 1)          //data from B2R
                        {
                            peArg->ui1_out_mode = draminf.B2R_inf.u1OutMode;
                            peArg->ui4_src_pitch = draminf.B2R_inf.u4Pitch;
                            peArg->ui4_src_height = draminf.B2R_inf.u4Height;
                            peArg->ui4_src_width = draminf.B2R_inf.u4With;
                            peArg->ui4_y_start_msb_add = draminf.B2R_inf.u4YStart;
                            peArg->ui4_u_start_msb_add = draminf.B2R_inf.u4CStart;
                        }
                        else if (peArg->ui1_data_src == 2)     //data from DI
                        {
                            peArg->ui1_out_mode = draminf.DI_inf.u1OutMode;
                            peArg->ui4_src_width = draminf.DI_inf.u2Width;
                            peArg->ui4_src_height = draminf.DI_inf.u2Height;
                            peArg->ui4_src_pitch = draminf.DI_inf.u2DramPitch;
                            peArg->ui4_u_start_lsb_add = draminf.DI_inf.u4UStartLSBAddr;
                            peArg->ui4_u_start_msb_add = draminf.DI_inf.u4UStartMSBAddr;
                            peArg->ui4_y_start_lsb_add = draminf.DI_inf.u4YStartLSBAddr;
                            peArg->ui4_y_start_msb_add = draminf.DI_inf.u4YStartMSBAddr;
                            peArg->ui4_v_start_lsb_add = draminf.DI_inf.u4VStartLSBAddr;
                            peArg->ui4_v_start_msb_add = draminf.DI_inf.u4VStartMSBAddr;
                            peArg->ui1_block_mode = draminf.DI_inf.u1BlockMode;
                            peArg->ui1_bit_res = draminf.DI_inf.u1BitRes;
                            peArg->ui1_color_mode = draminf.DI_inf.u1ColorMode;
                            peArg->ui1_AF_Bottom = draminf.DI_inf.u1AFBottom;
                            peArg->ui1_AF_YUVIndex = draminf.DI_inf.u1AFYUVIndex;
                            peArg->ui1_y_frame_num = draminf.DI_inf.u1YFrameNum;
                            peArg->ui1_c_frame_num = draminf.DI_inf.u1CFrameNUm;
                            peArg->ui1_CfgFlipMirror = draminf.DI_inf.u1CfgFlipMirror;
                            peArg->ui1_DramFlipMirror = draminf.DI_inf.u1DramFlipMirror;
                            peArg->b_dram_snake_map = draminf.DI_inf.bDramSnakeMapping;
                        }
                    }
                }
            break;
        
        case VID_PLA_GET_TYPE_DELAY_TIME:
        {
            UINT32 u4Delay=0;

                MTVDO_GetCmdDelay(MTVDO_MAIN, &u4Delay);

            *(UINT32*)pv_get_info = u4Delay;
        }
        break;
        case VID_PLA_GET_TYPE_VIDEO_FRAME_DELAY:
        {
            INT32* peArg;
            peArg = (INT32*) pv_get_info;

            if (*pz_get_info_len < sizeof(INT32))
            {
                i4ReturnValue = RMR_DRV_INV_GET_INFO;
            }
            else
            {
                *pz_get_info_len = sizeof(INT32);
                MTVDO_GetFrameDelay(ucVdpId, peArg);
            }
        }
        break;
        default:
            i4ReturnValue = RMR_DRV_INV_GET_INFO;
        break;
    }

    return (i4ReturnValue);
}

static INT32   _VdpSetDispRegion(UCHAR ucVdpId, const VOID* pv_set_info)
{
    VID_PLA_VID_REGION_T* prArg;
    INT32 i4ReturnValue = RMR_OK;

    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_SET_INFO);
    }
    VDP_DBG_MSG("vdp set disp region\n");
    prArg = (VID_PLA_VID_REGION_T*) pv_set_info;

    _arMwVdp[ucVdpId].rFullOutput = *prArg;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                    _arMwVdp[ucVdpId].rTrueFullOutput = *prArg;
#endif

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    if ((_arMwVdp[ucVdpId].ucDtv != 0) && (_arMwVdp[ucVdpId].e_type != DRVT_UNKNOWN))
    {
        // Callback again to update correct display region.
        if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
        {
            if(_VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }

            if(_VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
        }
        else
        {
            if (_arMwVdp[ucVdpId].eFmt != VID_PLA_DISP_FMT_NORMAL)
            {
                _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
            }
            else
            {
                _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rTrueFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
            }

            if(_VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
        }

    }
    else
    {
        // Final = Translate + Full
        if (_arMwVdp[ucVdpId].eFmt != VID_PLA_DISP_FMT_NORMAL)
        {
            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
        }
        else
        {
            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rTrueFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
        }
        
        PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalOutput);

        if(_VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput) != MTR_OK)
        {
            i4ReturnValue = RMR_DRV_INV_SET_INFO;
        }
            
    }
#else
    // Final = Translate + Full
    _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
#if 0//To debug the ap or sys_cfg outregion for aspectratio issue 
    Printf("%s(L%d) rXlatSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n",__FUNCTION__,__LINE__, 
        _arMwVdp[ucVdpId].rXlatOutput.ui4_x,
        _arMwVdp[ucVdpId].rXlatOutput.ui4_y,
        _arMwVdp[ucVdpId].rXlatOutput.ui4_width,
        _arMwVdp[ucVdpId].rXlatOutput.ui4_height);
    Printf("%s(L%d) rFullSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n",__FUNCTION__,__LINE__,
        _arMwVdp[ucVdpId].rFullOutput.ui4_x,
        _arMwVdp[ucVdpId].rFullOutput.ui4_y,
        _arMwVdp[ucVdpId].rFullOutput.ui4_width,
        _arMwVdp[ucVdpId].rFullOutput.ui4_height);	
    Printf("%s(L%d) rFinalSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n",__FUNCTION__,__LINE__,
        _arMwVdp[ucVdpId].rFinalOutput.ui4_x,
        _arMwVdp[ucVdpId].rFinalOutput.ui4_y,
        _arMwVdp[ucVdpId].rFinalOutput.ui4_width,
        _arMwVdp[ucVdpId].rFinalOutput.ui4_height);
#endif
    PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rXlatOutput);
    PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFullOutput);
    PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalOutput);

    if(_VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput) != MTR_OK)
    {
        i4ReturnValue = RMR_DRV_INV_SET_INFO;
    }
#endif
    return i4ReturnValue ;
}

static INT32 _VdpSetDispFormat(UCHAR ucVdpId, const VOID* pv_set_info)
{
LINT_SUPPRESS_BRACE(549)
    
    static UINT8 bOldFmt[VDP_COMP_NS] ={0xff, 0xff};
    UINT8 bMuteTime =0;
    VID_PLA_DISP_FMT_T eArg;
	INT32 i4ReturnValue = (INT32)RMR_OK;
	
    eArg = (VID_PLA_DISP_FMT_T) (UINT32) pv_set_info;

    if ((ucVdpId >= VDP_COMP_NS) || (!fgEnableMwCb))
    {
       return (RMR_DRV_INV_SET_INFO);
    }

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    _VdpDispFmtLock();
#endif

    _arMwVdp[ucVdpId].eFmt = eArg;

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    _VdpDispFmtUnLock();
#endif

    VDP_DBG_MSG("_VdpSetDispFormat %d\n", _arMwVdp[ucVdpId].eFmt);

    bMuteTime = MTDRVCUST_OptGet(eFlagMuteDispFmtChg);
    if (bMuteTime &&(bOldFmt[ucVdpId] != _arMwVdp[ucVdpId].eFmt))
    {
                MTVDO_SetVideoMute((MTVDO_PATH_T)ucVdpId, MTVDO_DISP_FMT_MUTE, bMuteTime, TRUE);
            bOldFmt[ucVdpId] = _arMwVdp[ucVdpId].eFmt;

            MTVDO_UpdateCmdDelay((MTVDO_PATH_T)ucVdpId);
    }
                if ((eArg == VID_PLA_DISP_FMT_NON_LINEAR_ZOOM) ||
                    (eArg == VID_PLA_DISP_FMT_NLZ_CUSTOM_0) ||
                    (eArg == VID_PLA_DISP_FMT_NLZ_CUSTOM_1) ||
                    (eArg == VID_PLA_DISP_FMT_NLZ_CUSTOM_2) ||
                    (eArg == VID_PLA_DISP_FMT_NLZ_CUSTOM_3))
                {
                    // turn off 121
                    if(MTVDO_SetDotByDot((MTVDO_PATH_T)ucVdpId, 0) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                    // turn on non-linear
                    if(MTVDO_SetNonLinearMode((MTVDO_PATH_T)ucVdpId, MTVDO_NONLINEAR_ON, NULL) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
                else
                if (eArg == VID_PLA_DISP_FMT_DOT_BY_DOT)
                {
                    // turn off non-linear
                    if(MTVDO_SetNonLinearMode((MTVDO_PATH_T)ucVdpId, MTVDO_NONLINEAR_OFF, NULL) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // turn on 121
                    if(MTVDO_SetDotByDot((MTVDO_PATH_T)ucVdpId, 1) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
                else
                {
                    // turn off 121
                    if(MTVDO_SetDotByDot((MTVDO_PATH_T)ucVdpId, 0) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // turn off non-linear
                    if(MTVDO_SetNonLinearMode((MTVDO_PATH_T)ucVdpId, MTVDO_NONLINEAR_OFF, NULL) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }

                if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
                {
                    if(_VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    if(_VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
                // DRV_CUSTOM_VID_SRC_INFO_TYPE_INPUT_SRC will update the disp format in sys build
                // But the return value src/out region will not be set.
                if (_pfXlatDispFmtExFct[(UINT32)_arMwVdp[ucVdpId].eFmt] != NULL)
                {
                    VID_PLA_VID_REGION_T rSrcReg;
                    VID_PLA_VID_REGION_T rDispReg;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                    VID_PLA_VID_REGION_T rUpdateDispReg;
#endif
                    DRV_CUSTOM_VID_SRC_INFO_T rVidSrcInfo;

                    rVidSrcInfo.ui2_vdp_id = (UINT16)ucVdpId;
                    rVidSrcInfo.e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_INPUT_SRC;
                    rVidSrcInfo.u.t_input_src.e_input_source = _VdpGetDrvCustInputSrcType(ucVdpId, _arMwVdp[ucVdpId].e_type);
                    if (rVidSrcInfo.u.t_input_src.e_input_source == DRV_CUST_INPUT_SRC_TYPE_MULTIMEDIA)
                    {
                        // sys not ready for MM input source.
                        // once sys is ready for this input source, you can remove this.
                        rVidSrcInfo.u.t_input_src.e_input_source = DRV_CUST_INPUT_SRC_TYPE_DTV;
                    }
                    rVidSrcInfo.u.t_input_src.ui2_id = _arMwVdp[ucVdpId].u2SrcCompId;
                    rVidSrcInfo.u.t_input_src.e_timing_type = _arMwVdp[ucVdpId].e_timing;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                    rUpdateDispReg = _arMwVdp[ucVdpId].rTrueFullOutput;
#endif

                    if (rVidSrcInfo.u.t_input_src.e_input_source != DRV_CUST_INPUT_SRC_TYPE_UNKNOWN)
                    {
                        _pfXlatDispFmtExFct[(UINT32)_arMwVdp[ucVdpId].eFmt](
                            &_arMwVdp[ucVdpId].rSrcRes,
                            &_arMwVdp[ucVdpId].rOutRes,
                            &rVidSrcInfo,
                            &rSrcReg,
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                            &rDispReg,
                            &rUpdateDispReg);
#else
                            &rDispReg);
#endif

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                        _arMwVdp[ucVdpId].rFullOutput = rUpdateDispReg;
#endif
                    }
                }
                return  i4ReturnValue ;

}


static INT32 _VdpSetOverScan(UCHAR ucVdpId, const VOID* pv_set_info)
{
    VID_PLA_OVER_SCAN_CLIPPER_T* prArg;
    MTVDO_OVERSCAN_REGION_T rOverScan;
    INT32 i4ReturnValue = RMR_OK;


    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_SET_INFO);
    }

    if (pv_set_info == NULL)
    {
        i4ReturnValue = RMR_DRV_INV_SET_INFO;
    }
    else
    {
        prArg = (VID_PLA_OVER_SCAN_CLIPPER_T*) pv_set_info;

        rOverScan.u4Top = (UINT32) prArg->ui4_top;
        rOverScan.u4Bottom = (UINT32) prArg->ui4_bottom;
        rOverScan.u4Left = (UINT32) prArg->ui4_left;
        rOverScan.u4Right = (UINT32) prArg->ui4_right;
		PrintfOverscan("_VdpSetOverScan1 %d %d %d %d\n", rOverScan.u4Top, rOverScan.u4Bottom, rOverScan.u4Left,rOverScan.u4Right);
		LOG(2,"%s(L%d) _VdpSetOverScan1  %d %d %d %d\n",__FUNCTION__,__LINE__,rOverScan.u4Top, rOverScan.u4Bottom, rOverScan.u4Left,rOverScan.u4Right);

#if 1
        //removed by MTAL porting
        //After discuss with AP, the function is not used now.
        if (_pfUpdateOverscanTbl != NULL)
        {
            DRV_CUST_INPUT_SRC_TYPE_T eSrcType;
            DRV_CUSTOM_TIMING_INFO_T rTimingInfo;

            eSrcType = _VdpGetDrvCustInputSrcType(ucVdpId, _arMwVdp[ucVdpId].e_type);
            /*check MM mode*/
                  
            rTimingInfo.ui2_width = _arMwVdp[ucVdpId].rSrcRes.ui2_width;
            rTimingInfo.ui2_height = _arMwVdp[ucVdpId].rSrcRes.ui2_height;
            rTimingInfo.ui4_fram_rate = _arMwVdp[ucVdpId].u4FrameRate;
            rTimingInfo.e_color_sys = _VdpGetDrvCustColorSys(ucVdpId);
            rTimingInfo.e_timing_type = _arMwVdp[ucVdpId].e_timing;

            if (_arMwVdp[ucVdpId].u4Interlace)
            {
                rTimingInfo.e_scan_mode = DRV_CUSTOM_TIMING_SCAN_MODE_INTERLACE;
            }
            else
            {
                rTimingInfo.e_scan_mode = DRV_CUSTOM_TIMING_SCAN_MODE_PROGRESSIVE;
            }
            _pfUpdateOverscanTbl(
                ucVdpId,
                eSrcType,
                &rTimingInfo,
                (UINT16)rOverScan.u4Top,
                (UINT16)rOverScan.u4Bottom,
                (UINT16)rOverScan.u4Left,
                (UINT16)rOverScan.u4Right
                );
        }
#endif
		
		LOG(2,"%s(L%d) _VdpSetOverScan2  %d %d %d %d\n",__FUNCTION__,__LINE__,rOverScan.u4Top, rOverScan.u4Bottom, rOverScan.u4Left,rOverScan.u4Right);
		PrintfOverscan("_VdpSetOverScan2  %d %d %d %d\n", rOverScan.u4Top, rOverScan.u4Bottom, rOverScan.u4Left,rOverScan.u4Right);
        if(MTVDO_SetOverScan((MTVDO_PATH_T)ucVdpId, &rOverScan) != MTR_OK)
        {
            // if video plane is NOT connect (switch), it will return fail.
            // ignore it!
            // i4ReturnValue = RMR_DRV_INV_SET_INFO;
        }
    }
    return i4ReturnValue;
}


//-----------------------------------------------------------------------------
/** Brief of _VdpSet.
 */
//-----------------------------------------------------------------------------
static INT32 _VdpSet(DRV_COMP_ID_T*  pt_comp_id,
                    DRV_SET_TYPE_T  e_set_type,
                    const VOID*     pv_set_info,
                    SIZE_T          z_set_info_len)
{
LINT_SUPPRESS_BRACE(818)

    UCHAR ucVdpId;
    INT32 i4ReturnValue;

    // Check component type and id
    if (pt_comp_id == NULL)
    {
       return (RMR_DRV_INV_SET_INFO);
    }

    if (pt_comp_id->e_type != DRVT_VID_PLANE)
    {
       return (RMR_DRV_INV_SET_INFO);
    }

    ucVdpId = (UCHAR) pt_comp_id->ui2_id;
    if (ucVdpId > (VDP_COMP_NS + VDP_COMP_1))
    {
       return (RMR_DRV_INV_SET_INFO);
    }

    if (ucVdpId >= VDP_COMP_NS)
    {
       return (RMR_DRV_INV_SET_INFO);
    }

    // Check set_info
    if (z_set_info_len == 0)
    {
        return (RMR_DRV_INV_SET_INFO);
    }

    /* This is better one to allow PQ settings */
    if (fgVDPForceFreeze)
    {
        switch (e_set_type)
        {
            case VID_PLA_SET_TYPE_CTRL:
            case VID_PLA_SET_TYPE_MODE:
                return RMR_OK;

            default:
                break;
        }
    }

    i4ReturnValue = RMR_OK;
    //VERIFY(x_sema_lock(_hMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    VDP_SET_DBG_MSG("VDP(%d) SET(%d) ARG(%x)\n",ucVdpId,  (UINT32) (e_set_type ), (UINT32) pv_set_info);

    // Perform set opertion
    switch (e_set_type)
    {
        case VID_PLA_SET_TYPE_BRIGHTNESS:
        case VID_PLA_SET_TYPE_CONTRAST:
        case VID_PLA_SET_TYPE_HUE:
        case VID_PLA_SET_TYPE_SATURATION:
        case VID_PLA_SET_TYPE_SHARPNESS:
        case VID_PLA_SET_TYPE_BLENDING:
        case VID_PLA_SET_TYPE_CTI:
        case VID_PLA_SET_TYPE_ETI:
        case VID_PLA_SET_TYPE_COLOR_SUPPRESS:
        case VID_PLA_SET_TYPE_NR:
        case VID_PLA_SET_TYPE_BLACK_LVL_EXT:
        case VID_PLA_SET_TYPE_WHITE_PEAK_LMT:
        case VID_PLA_SET_TYPE_FLESH_TONE:
        case VID_PLA_SET_TYPE_LUMA:
        case VID_PLA_SET_TYPE_3D_NR:
        case VID_PLA_SET_TYPE_SUPER_RESOLUTION:     
        case VID_PLA_SET_TYPE_DI_FILM_MODE:
        case VID_PLA_SET_TYPE_DI_MA:
        case VID_PLA_SET_TYPE_DI_EDGE_PRESERVE:
        case VID_PLA_SET_TYPE_MJC_EFFECT:
        case VID_PLA_SET_TYPE_MJC_MODE:
        case VID_PLA_SET_TYPE_MJC_DEMO:
        case VID_PLA_SET_TYPE_XVYCC:
        case VID_PLA_SET_TYPE_WCG:
        case VID_PLA_SET_TYPE_BLUE_STRETCH:
        case VID_PLA_SET_TYPE_WHITE_STRETCH:
        case VID_PLA_SET_TYPE_BLACK_STRETCH:
        case VID_PLA_SET_TYPE_MPEG_NR:
        case VID_PLA_SET_TYPE_LCDIM:
        case VID_PLA_SET_TYPE_3D_MODE:
        case VID_PLA_SET_TYPE_3D_LR_SWITCH:
        case VID_PLA_SET_TYPE_3D_FLD_DEPTH:
        case VID_PLA_SET_TYPE_3D_TO_2D:                
        case VID_PLA_SET_TYPE_3D_FPR:                       
        case VID_PLA_SET_TYPE_3D_PROTRUDE_SCR:           
        case VID_PLA_SET_TYPE_3D_DISTANCE_TV:           
        case VID_PLA_SET_TYPE_3D_OSD_DEPTH:            
        {
            LINT_SUPPRESS_BRACE(507)
            MTVDO_ARG_TYPE_T ucArgType;
            UCHAR ucArg;

            ucArg = (UCHAR) (UINT32) pv_set_info;
            ucArgType = _VdpMwSetType2ArgType(e_set_type);

            LOG(2,"[MW_3D_SET]  Path = %d, ArgType = %d, Arg = %d\n",ucVdpId,ucArgType,(INT16)ucArg);

            if (MTVDO_SetArg(ucVdpId, ucArgType, (INT16)ucArg) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
        }
        break;

        case VID_PLA_SET_TYPE_MJC_BYPASSWINDOW:
        {
        	VID_PLA_MJC_BYPASS_WINDOW_INFO_T* prBypassArg;
        	MTVDO_REGION_T sMJCBypassWinInfo;
        	
        	if (pv_set_info == NULL)
              {
                  i4ReturnValue = RMR_DRV_INV_SET_INFO;
              }
              else
              {
              	prBypassArg = (VID_PLA_MJC_BYPASS_WINDOW_INFO_T*) pv_set_info;
              	sMJCBypassWinInfo.u4X= prBypassArg->u4Start_X;
              	sMJCBypassWinInfo.u4Y= prBypassArg->u4Start_Y;
              	sMJCBypassWinInfo.u4Width= prBypassArg->u4Width;
              	sMJCBypassWinInfo.u4Height= prBypassArg->u4Height;
              	MTVDO_SetMJCBypassWindow(prBypassArg->u1WinID,sMJCBypassWinInfo);
        		break;
              }
        	break;
        }
        
        case VID_PLA_SET_TYPE_3D_NAVI_AUTO_CHG_EN:
        {
            UINT32 uNaviUI =(UINT32)pv_set_info; 

            LOG(2,"[MW_3D_SET] Set UI Navi Mode = %d\n",(UINT16)uNaviUI);
            
            MTVDO_SetTDTVNaviMode((UINT16)uNaviUI);
            break;
        }

        case VID_PLA_SET_TYPE_3D_IMAGE_SAFETY_EN:
        {
            UINT8 u1Level = (UINT8) (UINT32)pv_set_info; 
            
            LOG(2,"[MW_3D_SET] Set IMAGE_SAFETY = %d\n",u1Level);
            
            MTVDO_Set3DImgSafetyLvl(u1Level);
            break;
        }      
        case VID_PLA_SET_TYPE_GAME_MODE:
        {
            UINT8 u1OnOff;
            u1OnOff = (UINT8) (UINT32) pv_set_info;
            MTVDO_SetGameMode(ucVdpId, u1OnOff);
            break;
        }
        #if defined(CC_DOWN_BW_WITH_MENU)
        case VID_PLA_SET_TYPE_DOWN_BW_MODE:
        {
            UINT8 u1OnOff;
            u1OnOff = (UINT8) (UINT32) pv_set_info;
            MTVDO_SetDownBwMode(MTVDO_MAIN, u1OnOff);
            break;
        }
        #endif
        case VID_PLA_SET_TYPE_CTRL:
        {
            LINT_SUPPRESS_BRACE(549)

            VID_PLA_CTRL_T eArg;
            eArg = (VID_PLA_CTRL_T) (UINT32) pv_set_info;

            if (eArg == VID_PLA_CTRL_ENABLE)
            {
                _arMwVdp[ucVdpId].ucEnable = 1;
                if (_VdpSetEnable(ucVdpId) != VDP_SET_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
            }
            else
            if (eArg == VID_PLA_CTRL_DISABLE)
            {
                _arMwVdp[ucVdpId].ucEnable = 0;

                _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_UNKNOWN;
                _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = 0;
                _ui2_pal_secam_wss = 0;
                _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = DRV_CUSTOM_SCART_PIN_8_UNKNOWN;

                if (_VdpSetEnable(ucVdpId) != VDP_SET_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
            }
            else
            {
                UCHAR ucConnect;

                ucConnect = _arMwVdp[ucVdpId].ucConnect;
                _arMwVdp[ucVdpId] = _rMwVdpDefault;
                _arMwVdp[ucVdpId].ucConnect = ucConnect;
                MTVDO_Reset((MTVDO_PATH_T)ucVdpId);
            }
        }
        break;

        case VID_PLA_SET_TYPE_MODE:
        {
            LINT_SUPPRESS_BRACE(549)

            VID_PLA_MODE_T eArg;
            eArg = (VID_PLA_MODE_T) (UINT32) pv_set_info;


            if ((_arMwVdp[ucVdpId].eMode == VID_PLA_FREEZE) &&
                (eArg != VID_PLA_FREEZE))
            {
                if(MTVDO_SetFreeze((MTVDO_PATH_T)ucVdpId, 0) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
            }

            if (eArg == VID_PLA_FREEZE)
            {
                if(MTVDO_SetFreeze((MTVDO_PATH_T)ucVdpId, 1) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }

                if(MTVDO_SetMode((MTVDO_PATH_T)ucVdpId,VDP_MODE_DEINT) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
            }
            else
            if(MTVDO_SetMode((MTVDO_PATH_T)ucVdpId, (UCHAR)eArg) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            else
            {
            }

            _arMwVdp[ucVdpId].eMode = eArg;
        }
        break;
        case VID_PLA_SET_TYPE_SRM_MODE:
            {
                VID_PLA_SRM_MODE_T eSrmMode;
                {
                    eSrmMode = (VID_PLA_SRM_MODE_T)(UINT32)(pv_set_info);
                    if(eSrmMode == VID_PLA_SRM_MODE_OFF)
                    {
                        MTVDO_SetSrmMode((MTVDO_PATH_T)ucVdpId, MTVDO_MM_MODE_OFF);
                    }
                    else if(eSrmMode == VID_PLA_SRM_MODE_VIDEO)
                    {
                        MTVDO_SetSrmMode((MTVDO_PATH_T)ucVdpId, MTVDO_MM_VIDEO_MODE);
                    }
                    else if(eSrmMode == VID_PLA_SRM_MODE_THUMBNAIL)
                    {
                        MTVDO_SetSrmMode((MTVDO_PATH_T)ucVdpId, MTVDO_MM_THUMBNAIL_MODE);
                    }
                 }
            }
            break;
        case VID_PLA_SET_TYPE_BG:
            {
                UINT32 u4Bg;
                VID_PLA_BG_COLOR_T* prArg;

                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    prArg = (VID_PLA_BG_COLOR_T*) pv_set_info;

                    u4Bg = ((((UINT32)prArg->ui1_r) << 16) + (((UINT32)prArg->ui1_g) << 8) + ((UINT32)prArg->ui1_b));

                    if(MTVDO_SetBg(ucVdpId, u4Bg) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
            }
            break;

        case VID_PLA_SET_TYPE_DISP_REGION:
            {
                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    i4ReturnValue = _VdpSetDispRegion(ucVdpId, pv_set_info);
                }
            }
            break;

        case VID_PLA_SET_TYPE_SRC_REGION:
            {
                VID_PLA_VID_REGION_T* prArg;

                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    VDP_DBG_MSG("vdp set src region\n");
                    prArg = (VID_PLA_VID_REGION_T*) pv_set_info;

                    _arMwVdp[ucVdpId].rFullSource = *prArg;

                    // Final = Translate + Full
                    _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatSource, &_arMwVdp[ucVdpId].rFullSource, &_arMwVdp[ucVdpId].rFinalSource);

                    PrintfSrcRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalSource);
					
					LOG(2,"%s(L%d) vdp set src region  %d %d %d %d\n",__FUNCTION__,__LINE__,
						_arMwVdp[ucVdpId].rXlatSource.ui4_x, 
						_arMwVdp[ucVdpId].rXlatSource.ui4_y, 
						_arMwVdp[ucVdpId].rXlatSource.ui4_width,
						_arMwVdp[ucVdpId].rXlatSource.ui4_height);
					
					LOG(2,"%s(L%d) vdp set src region  %d %d %d %d\n",__FUNCTION__,__LINE__,
						_arMwVdp[ucVdpId].rFullSource.ui4_x, 
						_arMwVdp[ucVdpId].rFullSource.ui4_y, 
						_arMwVdp[ucVdpId].rFullSource.ui4_width,
						_arMwVdp[ucVdpId].rFullSource.ui4_height);
					
					LOG(2,"%s(L%d) vdp set src region  %d %d %d %d\n",__FUNCTION__,__LINE__,
						_arMwVdp[ucVdpId].rFinalSource.ui4_x, 
						_arMwVdp[ucVdpId].rFinalSource.ui4_y, 
						_arMwVdp[ucVdpId].rFinalSource.ui4_width,
						_arMwVdp[ucVdpId].rFinalSource.ui4_height);
					
                    if(_VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
            }
            break;

        case VID_PLA_SET_TYPE_OVER_SCAN_CLIPPER:
            {

                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    i4ReturnValue = _VdpSetOverScan(ucVdpId, pv_set_info);
                }
            }
            break;

        case VID_PLA_SET_TYPE_PIC_MODE:
        {
            if(MTDRVCUST_OptGet(eFlagFlashPqEnableQtySmartPic))
            {                
                MTVDO_SetPQ(ucVdpId, VDO_PQ_PICTURE_MODE, ((UINT8 *)(UINT32) &pv_set_info), sizeof(UINT8));

            }
        }
        break;

        case VID_PLA_SET_TYPE_COLOR_GAIN:
            {
                VID_PLA_COLOR_GAIN_T* prArg;
                VDP_COLOR_GAIN_T rGain;

                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    prArg = (VID_PLA_COLOR_GAIN_T*) pv_set_info;

                    rGain.ucRgain = (UINT16) prArg->ui2_r_gain;
                    rGain.ucGgain = (UINT16) prArg->ui2_g_gain;
                    rGain.ucBgain = (UINT16) prArg->ui2_b_gain;

                    // R Gain
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_R_GAIN, (INT16)rGain.ucRgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // G Gain
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_G_GAIN, (INT16)rGain.ucGgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // B Gain
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_B_GAIN, (INT16)rGain.ucBgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                }
            }
            break;

        case VID_PLA_SET_TYPE_COLOR_OFFSET:
            {
                VID_PLA_COLOR_GAIN_T* prArg;
                VDP_COLOR_GAIN_T rOffset;

                if (pv_set_info == NULL)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                else
                {
                    prArg = (VID_PLA_COLOR_GAIN_T*) pv_set_info;

                    rOffset.ucRgain = (UINT16) prArg->ui2_r_gain;
                    rOffset.ucGgain = (UINT16) prArg->ui2_g_gain;
                    rOffset.ucBgain = (UINT16) prArg->ui2_b_gain;

                    // R Offset
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_R_OFFSET, (INT16)rOffset.ucRgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // MTVDO_ Offset
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_G_OFFSET, (INT16)rOffset.ucGgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }

                    // B Offset
                    if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_B_OFFSET, (INT16)rOffset.ucBgain) != MTR_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
            }
            break;

        case VID_PLA_SET_TYPE_DISP_FMT:
        {
            i4ReturnValue = _VdpSetDispFormat(ucVdpId, pv_set_info);
        }
        break;


        case VID_PLA_SET_TYPE_ATOMIC_CMD_GRP:
        {

            if (pv_set_info == NULL)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            else
            {
                VID_PLA_ATOMIC_CMD_T *prAtmoicCmd;
                UINT8 u1CmdNum=0 ;

                prAtmoicCmd = (VID_PLA_ATOMIC_CMD_T *) pv_set_info ;
                do
                {
                    switch(prAtmoicCmd-> e_drv_set_type)
                    {
                        case VID_PLA_SET_TYPE_DISP_FMT:
                        {
                            i4ReturnValue = _VdpSetDispFormat(ucVdpId, prAtmoicCmd->u.pv_data);
                        }
                        break;

                        case VID_PLA_SET_TYPE_DISP_REGION:
                        {
                            i4ReturnValue = _VdpSetDispRegion(ucVdpId, prAtmoicCmd->u.pv_data);
                        }
                        break;

                        case VID_PLA_SET_TYPE_OVER_SCAN_CLIPPER:
                        {
                            i4ReturnValue = _VdpSetOverScan(ucVdpId, prAtmoicCmd->u.pv_data);
                        }
                        break;

                        case  VID_PLA_ATOMIC_CMD_CODE_END:
                        //command end
                        break;

                        default:
                            i4ReturnValue = RMR_DRV_INV_SET_INFO;

                    }
                    prAtmoicCmd++ ;
                    if(++u1CmdNum > MAX_ATOMIC_CMD_NUM)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                        break;
                    }
                }while(prAtmoicCmd-> e_drv_set_type != VID_PLA_ATOMIC_CMD_CODE_END);
            }
        }
        break;

        case VID_PLA_SET_TYPE_LETTER_BOX_DETECT:
        {
            LINT_SUPPRESS_BRACE(507)

                UCHAR ucLb;

                ucLb = (UCHAR) (UINT32) pv_set_info;


                if(MTVDO_SetLBoxEnable((MTVDO_PATH_T)ucVdpId, ucLb) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
        }
        break;

        case VID_PLA_SET_TYPE_DYNAMIC_SCALING:
        {
            LINT_SUPPRESS_BRACE(507)

                UINT16 u2Delay;

                u2Delay = (UINT16) (UINT32) pv_set_info;


                if(MTVDO_SetDynamicScale((MTVDO_PATH_T)ucVdpId, u2Delay) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
         }
        break;

        case VID_PLA_SET_TYPE_SPLIT_SCRN_DEMO:
            {
LINT_SUPPRESS_BRACE(507)

                MTVDO_ARG_TYPE_T ucArgType;
                UCHAR ucArg;

                _arMwVdp[ucVdpId].eDemo = (VID_PLA_SPLIT_SCRN_DEMO_T) (UINT32) pv_set_info;

                ucArg = (UCHAR) (UINT32) pv_set_info;
                ucArgType = _VdpMwSetType2ArgType(e_set_type);

                if (MTVDO_SetArg(ucVdpId, ucArgType, (INT16)ucArg) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
             }
            break;

          case VID_PLA_SET_TYPE_FILM_MODE:
             {
                BOOL b32OnOff = (BOOL) (UINT32) pv_set_info;

                if (MTVDO_SetArg(ucVdpId, MTVDO_ARG_DI_FILM_MODE, (UINT8)b32OnOff) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
             }
            break;

        case VID_PLA_SET_TYPE_H_POSITION:
        {
        INT32 pos=0,min_off=0,max=0,min=0,def=0;
        MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;

        MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_DEFAULT, (UINT16 *)&def);
        MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MAX, (UINT16 *)&max);
        MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_MIN, (UINT16 *)&min);

        MTVDECEX_GetPorchTune((MTVDO_PATH_T)ucVdpId, &eTuneType);

        if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
        {
               pos=(32-(UINT32)pv_set_info)*2+def;
               if(pos > max)
                   pos = max ;
               if(pos < min)
                   pos = min ;
                MTVDECEX_SetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_CURRENT, pos);
        }
        else
        {
            if (rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_VGA)
            {
                min_off=(INT32)pv_set_info-min;
                pos=max-min_off;
            }
            else
            {
                min_off=(INT32)pv_set_info-def;
                pos=def-min_off;
            }
                MTVDECEX_SetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_CURRENT, pos);
                MTVDECEX_SetScposPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_HPORCH_CURRENT, 1000);// reset scpos porch
                
            }
    }
            break;

        case VID_PLA_SET_TYPE_V_POSITION:
        {
        MTVDECEX_PORCHTUNE_TYPE_T eTuneType=MTVDECEX_PORCHTUNE_DEC;

        MTVDECEX_GetPorchTune((MTVDO_PATH_T)ucVdpId, &eTuneType);

        if (eTuneType == MTVDECEX_PORCHTUNE_SCPOS)//for DTV
        {
            UINT16 def = 0, u2TargetPorch = 0, max = 0, min = 0;
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_DEFAULT, &def);
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_MAX, (UINT16 *)&max);
            MTVDECEX_GetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_MIN, (UINT16 *)&min);

            u2TargetPorch=(UINT16)(INT32)pv_set_info - 16 + def;
            if(u2TargetPorch > max)
            {
                u2TargetPorch = max;
            }
            if(u2TargetPorch < min)
            {
                u2TargetPorch = min;
            }
            
            MTVDECEX_SetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_CURRENT, u2TargetPorch);
        }
        else
        {
            MTVDECEX_SetPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_CURRENT, (UINT16)(INT32)pv_set_info);
            MTVDECEX_SetScposPorch((MTVDO_PATH_T)ucVdpId, MTVDECEX_VPORCH_CURRENT, 1000);// reset scpos porch
        }
            break;
        }
        case VID_PLA_SET_TYPE_FORCE_UNMUTE_ON_NO_SIG:
            if ( z_set_info_len != sizeof(BOOL) )
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
                break;
            }
            if ( (BOOL)(UINT32)pv_set_info != TRUE &&
                 (BOOL)(UINT32)pv_set_info != FALSE )
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
                break;
            }
            MTVDO_SetCondForceUnmute((MTVDO_PATH_T)ucVdpId, MTVDO_NO_SIGNAL_UNMUTE, (BOOL)(UINT32)pv_set_info);
            fgVDPForceUnmute = (BOOL)(UINT32)pv_set_info;
            break;

        case VID_PLA_SET_TYPE_HDMI_MODE:
            {
             MTVDECEX_HDMI_COLOR_MODE_T u1NewHdmiColorMode;
             static MTVDECEX_HDMI_COLOR_MODE_T u1OldHdmiColorMode=MTVDECEX_HDMI_MODE_AUTO;
             
             switch((UINT32)pv_set_info)
             {
                case VID_PLA_HDMI_MODE_GRAPHIC:
                    u1NewHdmiColorMode = MTVDECEX_HDMI_MODE_GRAPHIC ;
                break;
                case VID_PLA_HDMI_MODE_VIDEO:
                    u1NewHdmiColorMode = MTVDECEX_HDMI_MODE_VIDEO ;
                break;
                case VID_PLA_HDMI_MODE_AUTO:
                default:
                    u1NewHdmiColorMode = MTVDECEX_HDMI_MODE_AUTO ;
                break;
             }
                   
            if(u1NewHdmiColorMode != u1OldHdmiColorMode)
            {
                u1OldHdmiColorMode = u1NewHdmiColorMode ;
                MTVDECEX_HDMI_SetColorMode((MTVDO_PATH_T)ucVdpId,  u1NewHdmiColorMode);
            }         
         }
            break;
        /*begin added for MSI , lianming.lin 2012-03-16*/
		case VID_PLA_SET_TYPE_SRC_NFY_FCT:
			{
				VID_PLA_SRC_NFY_INFO_T* prArg;

				prArg = (VID_PLA_SRC_NFY_INFO_T*) pv_set_info;
				if ( !prArg )
				{
					i4ReturnValue = RMR_DRV_INV_SET_INFO;
				}
				else
				{
					_arMwVdp[ucVdpId].pfVdpSrcChg = prArg->pf_vid_pla_src_nfy;
					_arMwVdp[ucVdpId].pv_nfy_tag = prArg->pv_tag;
				}
				
			}
			break;            
        case VID_PLA_SET_TYPE_PRE_DOWN:
			{
				UINT32   u4Factor;
		
				u4Factor = ( UINT32 )pv_set_info;
                MTVDO_SetDSFactor(ucVdpId, u4Factor);				
			}
			break;
        case VID_PLA_SET_TYPE_V_PRE_DOWN:
			{
				UINT32   u4Factor;
		
				u4Factor = ( UINT32 )pv_set_info;
                MTVDO_SetVerticalDSFactor(ucVdpId, u4Factor);				
			}
			break;
		case VID_PLA_SET_TYPE_DISPLAY_CTRL:
			{
				BOOL	 fgOnOff;
                UINT32      u4DispOffModule;
                VID_DISP_OFF_INFO_T* prArg;
                
                prArg = (VID_DISP_OFF_INFO_T*)pv_set_info;
				fgOnOff = (BOOL) prArg->b_ctrl;
                u4DispOffModule = (UINT32) prArg->eDispOffModule;
                
				MTVDO_SetDisplayOff_Ext(ucVdpId, u4DispOffModule,fgOnOff);
			}
			break;
		case VID_PLA_SET_TYPE_SUB_SRC_CLONE:
		    {
		        BOOL	 fgOnOff;
				fgOnOff = (BOOL) (UINT32)pv_set_info;
				MTVDO_SetSubCloneMode(fgOnOff);
		    }
		    break;		
        case VID_PLA_SET_TYPE_ENHANCE:
        case VID_PLA_SET_TYPE_QV_INP:
        case VID_PLA_SET_TYPE_LB_DETECT_CONFIG:
        {
            VID_PLA_LB_DETECT_CONFIG_T* prArg;

            UINT32 u4Time;
            UINT32 u4Bound;
            prArg = (VID_PLA_LB_DETECT_CONFIG_T*) pv_set_info;
            u4Time = (UINT32) prArg->ui4_time_threshold;
            u4Bound = (UINT32) prArg->ui4_bounds_left;
            _arMwVdp[ucVdpId].rLBConfig.ui4_bounds_left = u4Bound;
            _arMwVdp[ucVdpId].rLBConfig.ui4_bounds_right = u4Bound;
            _arMwVdp[ucVdpId].rLBConfig.ui4_bounds_top= u4Bound;
            _arMwVdp[ucVdpId].rLBConfig.ui4_bounds_bottom= u4Bound;
            _arMwVdp[ucVdpId].rLBConfig.ui4_time_threshold = u4Time;

            if(MTVDO_SetLBoxConfig((MTVDO_PATH_T)ucVdpId, u4Time, u4Bound)  != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
        }
            break;

        case VID_PLA_SET_TYPE_NFY_FCT:
            {
                VID_PLA_NFY_INFO_T* prArg;

                prArg = (VID_PLA_NFY_INFO_T*) pv_set_info;

                _arMwVdp[ucVdpId].pfEosNfy = prArg->pf_vid_pla_nfy;
                _arMwVdp[ucVdpId].pvEosTag = prArg->pv_tag;

                MTVDO_RegCbFunc(VDO_CB_FUNC_MM_COND_IND, (UINT32)_VdpEosCallback, 0, 0);
            }
            break;

        case VID_PLA_SET_TYPE_SUPER_BLANK:
            {
                VID_PLA_SUPER_BLANK_T *rSuperBlank;
                UINT32 u4Bg;

                rSuperBlank = (VID_PLA_SUPER_BLANK_T*)pv_set_info;

                if(rSuperBlank->b_enable != FALSE)
                {
                    _arMwVdp[ucVdpId].ucBlank = 1;
                    if (_VdpSetEnable(ucVdpId) != VDP_SET_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                    else
                    {
                        MTVDO_SetMode((MTVDO_PATH_T)ucVdpId,VDO_MODE_BG);

                        _arMwVdp[ucVdpId].rFullOutput.ui4_x = rSuperBlank->t_disp_region.ui4_x;
                        _arMwVdp[ucVdpId].rFullOutput.ui4_y = rSuperBlank->t_disp_region.ui4_y;
                        _arMwVdp[ucVdpId].rFullOutput.ui4_width = rSuperBlank->t_disp_region.ui4_width;
                        _arMwVdp[ucVdpId].rFullOutput.ui4_height = rSuperBlank->t_disp_region.ui4_height;

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
                        if (_arMwVdp[ucVdpId].eFmt != VID_PLA_DISP_FMT_NORMAL)
                        {
                            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
                        }
                        else
                        {
                            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rTrueFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
                        }
#else
                        _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
#endif
                  
                        _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);

                        u4Bg = ((((UINT32)rSuperBlank->t_bg_color.ui1_r) << 16) + (((UINT32)rSuperBlank->t_bg_color.ui1_g) << 8) + ((UINT32)rSuperBlank->t_bg_color.ui1_b));
                        MTVDO_SetBg(ucVdpId, u4Bg);
                    }
                }
                else
                {
                    MTVDO_SetMode((MTVDO_PATH_T)ucVdpId,VDO_MODE_NORMAL);
                    _arMwVdp[ucVdpId].ucBlank = 0;
                    if (_VdpSetEnable(ucVdpId) != VDP_SET_OK)
                    {
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    }
                }
            }
            break;

        case VID_PLA_SET_TYPE_COLORSPACE:
        {
            UINT8 u1Mode;
            
            switch ((UINT32)pv_set_info)
            {
                case VID_PLA_COLORSPACE_FORCE_RGB:
                    u1Mode = ENUM_COLORSPACE_FORCE_RGB;
                    break;
                case VID_PLA_COLORSPACE_FORCE_YCBCR:
                    u1Mode = ENUM_COLORSPACE_FORCE_YCBCR;
                    break;
                case VID_PLA_COLORSPACE_AUTO:
                default:
                    u1Mode = ENUM_COLORSPACE_AUTO;
                    break;
            }
            
            MTVDO_SetPQ(ucVdpId, VDO_PQ_VGA_COLOR_SPACE_SET, &u1Mode, sizeof(UINT8));
            break;
        }
        break;
#ifdef CC_FLIP_MIRROR_SUPPORT
        case VID_PLA_SET_TYPE_FLIP:
        {
            UINT32 u4OnOff;    
            u4OnOff = (UINT32)pv_set_info;
            if (MTVDO_SetMirrorFlip(0, u4OnOff) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            
        }
        break;

        case VID_PLA_SET_TYPE_MIRROR:
        {
            UINT32 u4OnOff;    
            u4OnOff = (UINT32)pv_set_info;
            if (MTVDO_SetMirrorFlip(1, u4OnOff) != MTR_OK)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }

        }
        break;
		case VID_PLA_SET_TYPE_MIRROR_B2R:
		{
			BOOL fgOn;    
            fgOn = (BOOL)(UINT32)pv_set_info;
			if (MTB2R_SetMirror(ucVdpId, fgOn) != MTR_OK)
			{
				i4ReturnValue = RMR_DRV_INV_SET_INFO;
			}
		}
		break;
#endif
#ifdef DIVX_PLUS_CER
#ifdef CC_B2R_CHAPTER_CHG
        case VID_PLA_SET_TYPE_CHAP_INFO:
        {
            MTB2R_CHAPTER_T       rChapter;
            VID_PLA_VID_CHAP_T  *prChapInfo = NULL ;

            x_memset(&rChapter,0,sizeof(rChapter));

            if(NULL == pv_set_info)
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
                break;
            }
            prChapInfo = (VID_PLA_VID_CHAP_T *)pv_set_info;

            LOG(0," PLA_SET_TYPE_CHAP_INFO \n");

            rChapter.fgIsPending     = prChapInfo->b_seek;
            rChapter.u8StartPts      = prChapInfo->ui8_start_time;
            rChapter.u8EndPts        = prChapInfo->ui8_end_time;
            rChapter.fgFirstSetChapter = prChapInfo->b_first;
            if(MTR_OK != MTB2R_SetChpterEnable(ucVdpId, &rChapter))
            {
                i4ReturnValue = RMR_DRV_INV_SET_INFO;
            }
            break;
        }  
#endif
#endif

#ifdef SYS_DTV_FRZ_CHG_SUPPORT
        case VID_PLA_SET_TYPE_DTV_FREEZE:
            {
                BOOL	 fgOnOff;
                fgOnOff = (BOOL) pv_set_info;
                #define FOREVER_MUTE            0xFFFFFFFF
                if(MTVDO_SetFreeze((MTVDO_PATH_T)ucVdpId,fgOnOff) != MTR_OK)
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
                if(fgOnOff==0&&_arMwVdp[ucVdpId].eMode==VID_PLA_BLANK)
                {
                     MTVDO_SetVideoMute((MTVDO_PATH_T)ucVdpId, MTVDO_AP_MUTE, FOREVER_MUTE, FALSE);
                }
            }
            break;
#endif

        case VID_PLA_SET_TYPE_SUPER_FREEZE:
            {
                VID_PLA_SUPER_FREEZE_T* ptSuperFreeze;

                ptSuperFreeze = (VID_PLA_SUPER_FREEZE_T*)pv_set_info;
                if (ptSuperFreeze->e_type == VID_PLA_SUPER_FREEZE_TYPE_FORCE)
                {
                    /* for time shift super freeze */
                    if(ptSuperFreeze->b_enable)
                    {
                        fgVDPForceFreeze = TRUE;
                    }
                    else
                    {
                        fgVDPForceFreeze = FALSE;

                        /* disconnect first by recorded disconnect parameter for CR 212789 */
                        /* otherwise actual connect can't notify resolution change to mw */
                        if (_fgVDPSuperFreezeDisc)
                        {
                            _VdpDisconnect(&_rVDPDiscPara.t_comp_id,
                                           _rVDPDiscPara.e_disc_type,
                                           (const VOID*)&_rVDPDiscPara.t_disc_info,
                                           _rVDPDiscPara.z_disc_info_len);
                            return (i4ReturnValue);
                        }
                    }
                    /*todo: need set to vdp driver */
                }
                else if(ptSuperFreeze->e_type == VID_PLA_SUPER_FREEZE_TYPE_ATVPVR)
                {
                    MTVDO_SetCondForceUnmute(MTVDO_MAIN, MTVDO_ATV_PVR_UNMUTE, (BOOL)ptSuperFreeze->b_enable);
                    MTVDO_SetCondForceUnmute(MTVDO_SUB , MTVDO_ATV_PVR_UNMUTE, (BOOL)ptSuperFreeze->b_enable);
                    MTVDO_SetFreeze(MTVDO_MAIN, (BOOL)ptSuperFreeze->b_enable);
                 
                }
                else
                {
                    i4ReturnValue = RMR_DRV_INV_SET_INFO;
                }
            }
            break;

        case VID_PLA_SET_TYPE_FORCE_UNMUTE:
            {
                VID_PLA_FORCE_UNMUTE_T *ptForceUmute;
                MTVDO_FORCE_UNMUTE_T eUnmuteType ;

                ptForceUmute = (VID_PLA_FORCE_UNMUTE_T*)pv_set_info;
                 switch(ptForceUmute->e_force_unmute_type)
                 {
                    case VID_PLA_FORCE_UNMUTE_TYPE_ON_NO_SIG:
                        eUnmuteType = MTVDO_NO_SIGNAL_UNMUTE;
                    break;
                    case VID_PLA_FORCE_UNMUTE_TYPE_ATV_CHG_CH:
                        eUnmuteType = MTVDO_ATV_CHG_CH_UNMUTE;
                    break;
                    case VID_PLA_FORCE_UNMUTE_TYPE_ATV_PVR:
                        eUnmuteType = MTVDO_ATV_PVR_UNMUTE;
                    break;    
                    default:
                        i4ReturnValue = RMR_DRV_INV_SET_INFO;
                    break;
                 }
                if(i4ReturnValue != RMR_DRV_INV_SET_INFO)
                {
                    MTVDO_SetCondForceUnmute((MTVDO_PATH_T)ucVdpId, eUnmuteType, (BOOL)(ptForceUmute->b_enable));
                }
            }
            break; 
        default:
            i4ReturnValue = RMR_DRV_INV_SET_INFO;
            break;
    }


    return (i4ReturnValue);
}

//-----------------------------------------------------------------------------
/** Brief of _SetInput.
 */
//-----------------------------------------------------------------------------
static UINT32 _SetInput(UCHAR ucVdpId)
{
    UCHAR ucEsId;
#ifdef CC_3D_MM_DS_SUPPORT    
    if(_arMwComp3dInfo[_arMwVdp[0].u2SrcCompId] || _arMwComp3dInfo[_arMwVdp[1].u2SrcCompId])
    {
        ucEsId =0;
    }
    else
#endif  
	{
    ucEsId = _arMwVdp[ucVdpId].ucEsId;
	}

    if (MTB2R_SetInput(ucVdpId, ucEsId, 0) != MTR_OK)
    {
        return VDP_SET_ERROR;
    }
    return VDP_SET_OK;
}

//-----------------------------------------------------------------------------
/** Brief of _VdpSetEnable.
 */
//-----------------------------------------------------------------------------
static UINT32 _VdpSetEnable(UCHAR ucVdpId)
{
    if (_arMwVdp[ucVdpId].ucConnect &&
        _arMwVdp[ucVdpId].ucEnable)
    {
        if(MTVDO_SetEnable(ucVdpId, 1) != MTR_OK)
        {
            return VDP_SET_ERROR;
        }
		
		LOG(2,"%s(L%d) _VdpSetEnable %d\n",__FUNCTION__,__LINE__);
    }
    else if ((_arMwVdp[ucVdpId].ucConnect == 0) ||
             (_arMwVdp[ucVdpId].ucEnable == 0))
    {
        if(MTVDO_SetEnable(ucVdpId, 0) != MTR_OK)
        {
            return VDP_SET_ERROR;
        }		
		LOG(2,"%s(L%d) _VdpSetDisable %d\n",__FUNCTION__,__LINE__);
    }

    return VDP_SET_OK;
}

//-----------------------------------------------------------------------------
/** Brief of _VdpMwGetType2ArgType.
 */
//-----------------------------------------------------------------------------
static MTVDO_ARG_TYPE_T _VdpMwGetType2ArgType(UINT32 u4GetType)
{
    switch (u4GetType)
    {
        case VID_PLA_GET_TYPE_BRIGHTNESS:  
            return MTVDO_ARG_BRIGHTNESS;

        case VID_PLA_GET_TYPE_CONTRAST:    
            return MTVDO_ARG_CONTRAST;

        case VID_PLA_GET_TYPE_HUE:       
            return MTVDO_ARG_HUE;

        case VID_PLA_GET_TYPE_SATURATION: 
            return MTVDO_ARG_SATURATION;

        case VID_PLA_GET_TYPE_CTI:       
            return MTVDO_ARG_CTI;

        case VID_PLA_GET_TYPE_SHARPNESS:   
            return MTVDO_ARG_SHARPNESS;

        case VID_PLA_GET_TYPE_COLOR_GAIN:   
        case VID_PLA_GET_TYPE_COLOR_GAIN_MIN:   
        case VID_PLA_GET_TYPE_COLOR_GAIN_MAX:   
            return MTVDO_ARG_R_GAIN;

        case VID_PLA_GET_TYPE_COLOR_OFFSET: 
        case VID_PLA_GET_TYPE_COLOR_OFFSET_MIN: 
        case VID_PLA_GET_TYPE_COLOR_OFFSET_MAX: 
            return MTVDO_ARG_R_OFFSET;

        case VID_PLA_GET_TYPE_NR:           
            return MTVDO_ARG_NR;

        case VID_PLA_GET_TYPE_WHITE_PEAK_LMT:   
            return MTVDO_ARG_WHITE_PEAK_LMT;

        case VID_PLA_GET_TYPE_FLESH_TONE:  
            return MTVDO_ARG_SCE;

        case VID_PLA_GET_TYPE_LUMA:        
            return MTVDO_ARG_ADAPTIVE_LUMA;
            
        case VID_PLA_GET_TYPE_BLACK_STRETCH:
            
            return MTVDO_ARG_BLACK_STRETCH;
            
        case VID_PLA_GET_TYPE_WHITE_STRETCH:
            return MTVDO_ARG_WHITE_STRETCH;

        case VID_PLA_GET_TYPE_3D_NR:        
            return MTVDO_ARG_3D_NR;

        case VID_PLA_GET_TYPE_SPLIT_SCRN_DEMO:
            return MTVDO_ARG_DEMO;

        case VID_PLA_GET_TYPE_LCDIM:
            return MTVDO_ARG_LCDIM;                

        case VID_PLA_GET_TYPE_3D_MODE:
            return MTVDO_ARG_TDTV_PACKING;       
            
        case VID_PLA_GET_TYPE_3D_LR_SWITCH:
            return MTVDO_ARG_TDTV_INVERSE;
            
        case VID_PLA_GET_TYPE_3D_FLD_DEPTH:
            return MTVDO_ARG_TDTV_DEPTH;        
            
        case VID_PLA_GET_TYPE_3D_TO_2D:             
            return MTVDO_ARG_TDTV_3D_TO_2D;

        case VID_PLA_GET_TYPE_3D_FPR:             
            return MTVDO_ARG_TDTV_FPR;   
            
        case VID_PLA_GET_TYPE_3D_PROTRUDE_SCR:           
            return MTVDO_ARG_TDTV_PROTRUDE;
            
        case VID_PLA_GET_TYPE_3D_DISTANCE_TV:           
            return MTVDO_ARG_TDTV_DISTANCE;
            
        case VID_PLA_GET_TYPE_3D_OSD_DEPTH:           
            return MTVDO_ARG_TDTV_OSD_DEPTH;      
        default:
            return MTVDO_ARG_NS;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpMwSetType2ArgType.
 */
//-----------------------------------------------------------------------------
static MTVDO_ARG_TYPE_T _VdpMwSetType2ArgType(UINT32 u4SetType)
{
    switch (u4SetType)
    {
        case VID_PLA_SET_TYPE_BRIGHTNESS:  
            return MTVDO_ARG_BRIGHTNESS;

        case VID_PLA_SET_TYPE_CONTRAST:    
            return MTVDO_ARG_CONTRAST;

        case VID_PLA_SET_TYPE_HUE:         
            return MTVDO_ARG_HUE;

        case VID_PLA_SET_TYPE_SATURATION:   
            return MTVDO_ARG_SATURATION;

        case VID_PLA_SET_TYPE_CTI:          
            return MTVDO_ARG_CTI;

        case VID_PLA_SET_TYPE_SHARPNESS:   
            return MTVDO_ARG_SHARPNESS;

        case VID_PLA_SET_TYPE_NR:           
            return MTVDO_ARG_NR;

        case VID_PLA_SET_TYPE_FLESH_TONE:  
            return MTVDO_ARG_SCE;

        case VID_PLA_SET_TYPE_LUMA:        
            return MTVDO_ARG_ADAPTIVE_LUMA;
            
        case VID_PLA_SET_TYPE_BLACK_STRETCH:
            return MTVDO_ARG_BLACK_STRETCH;
            
        case VID_PLA_SET_TYPE_WHITE_STRETCH:
            return MTVDO_ARG_WHITE_STRETCH;

        case VID_PLA_SET_TYPE_WHITE_PEAK_LMT:  
            return MTVDO_ARG_WHITE_PEAK_LMT;

        case VID_PLA_SET_TYPE_3D_NR:       
            return MTVDO_ARG_3D_NR;

        case VID_PLA_SET_TYPE_SUPER_RESOLUTION:   
            return MTVDO_ARG_SUPERRESOLUTION_ON_OFF;

        case VID_PLA_SET_TYPE_SPLIT_SCRN_DEMO:        
            return MTVDO_ARG_DEMO;

        case VID_PLA_SET_TYPE_DI_FILM_MODE:
            return MTVDO_ARG_DI_FILM_MODE;

        case VID_PLA_SET_TYPE_DI_MA:
            return MTVDO_ARG_DI_MA;

        case VID_PLA_SET_TYPE_DI_EDGE_PRESERVE:
            return MTVDO_ARG_DI_EDGE_PRESERVE;

        case VID_PLA_SET_TYPE_MJC_MODE:
            return MTVDO_ARG_MJC_MODE;

        case VID_PLA_SET_TYPE_MJC_EFFECT:
            return MTVDO_ARG_MJC_EFFECT;

        case VID_PLA_SET_TYPE_MJC_DEMO:
            return MTVDO_ARG_MJC_DEMO;

        case VID_PLA_SET_TYPE_XVYCC:
            return MTVDO_ARG_XVYCC;

        case VID_PLA_SET_TYPE_WCG:
            return MTVDO_ARG_WCG;

        case VID_PLA_SET_TYPE_GAME_MODE:
            return MTVDO_ARG_GAME_MODE;

        case VID_PLA_SET_TYPE_BLUE_STRETCH:
            return MTVDO_ARG_BLUE_STRETCH;

        case VID_PLA_SET_TYPE_MPEG_NR:
            return MTVDO_ARG_MPEG_NR;

        case VID_PLA_SET_TYPE_LCDIM:
            return MTVDO_ARG_LCDIM;

        case VID_PLA_SET_TYPE_3D_MODE:
            return MTVDO_ARG_TDTV_PACKING;       
            
        case VID_PLA_SET_TYPE_3D_LR_SWITCH:
            return MTVDO_ARG_TDTV_INVERSE;
            
        case VID_PLA_SET_TYPE_3D_FLD_DEPTH:
            return MTVDO_ARG_TDTV_DEPTH;        
            
        case VID_PLA_SET_TYPE_3D_TO_2D:             
            return MTVDO_ARG_TDTV_3D_TO_2D;

        case VID_PLA_SET_TYPE_3D_FPR:             
            return MTVDO_ARG_TDTV_FPR;     
        
       case VID_PLA_SET_TYPE_3D_PROTRUDE_SCR:           
           return MTVDO_ARG_TDTV_PROTRUDE;
           
       case VID_PLA_SET_TYPE_3D_DISTANCE_TV:           
           return MTVDO_ARG_TDTV_DISTANCE;
           
       case VID_PLA_SET_TYPE_3D_OSD_DEPTH:           
           return MTVDO_ARG_TDTV_OSD_DEPTH;         
           
        default:
            return MTVDO_ARG_NS;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpMwVidQaType2UiVqItem.
 */
//-----------------------------------------------------------------------------
static MTVDO_ARG_TYPE_T _VdpMwVidQaType2UiVqItem(VID_QA_TYPE_T eVidQaType)
{
    switch (eVidQaType)
    {
        case VID_QA_TYPE_BRIGHTNESS:    
            return MTVDO_ARG_BRIGHTNESS;

        case VID_QA_TYPE_CONTRAST:      
            return MTVDO_ARG_CONTRAST;

        case VID_QA_TYPE_HUE:           
            return MTVDO_ARG_HUE;

        case VID_QA_TYPE_SATURATION:    
            return MTVDO_ARG_SATURATION;

        case VID_QA_TYPE_CTI:          
            return MTVDO_ARG_CTI;

        case VID_QA_TYPE_SHARPNESS:     
            return MTVDO_ARG_SHARPNESS;

        case VID_QA_TYPE_LTI:    
            return MTVDO_ARG_LTI;

        case VID_QA_TYPE_COLOR_SUPPRESS:
            return MTVDO_ARG_BRIGHTNESS;    
            
        case VID_QA_TYPE_COLOR_GAIN:    
            return MTVDO_ARG_R_GAIN;

        case VID_QA_TYPE_COLOR_OFFSET:    
            return MTVDO_ARG_R_OFFSET;

        case VID_QA_TYPE_NR:            
            return MTVDO_ARG_NR;

        case VID_QA_TYPE_GAMMA:  
            return MTVDO_ARG_GAMMA;

        case VID_QA_TYPE_WHITE_PEAK_LMT:
            return MTVDO_ARG_WHITE_PEAK_LMT;

        case VID_QA_TYPE_FLESH_TONE:   
            return MTVDO_ARG_SCE;

        case VID_QA_TYPE_LUMA:         
            return MTVDO_ARG_ADAPTIVE_LUMA;

        case VID_QA_TYPE_3D_NR:        
            return MTVDO_ARG_3D_NR;

        case VID_QA_TYPE_DEMO:
            return MTVDO_ARG_DEMO;

        case VID_QA_TYPE_DI_FILM_MODE:
            return MTVDO_ARG_DI_FILM_MODE;

        case VID_QA_TYPE_DI_MA:
            return MTVDO_ARG_DI_MA;

        case VID_QA_TYPE_DI_EDGE_PRESERVE:
            return MTVDO_ARG_DI_EDGE_PRESERVE;

        case VID_QA_TYPE_MJC_MODE:
            return MTVDO_ARG_MJC_MODE;

        case VID_QA_TYPE_MJC_EFFECT:
            return MTVDO_ARG_MJC_EFFECT;

        case VID_QA_TYPE_MJC_DEMO:
            return MTVDO_ARG_MJC_DEMO;

        case VID_QA_TYPE_XVYCC:
            return MTVDO_ARG_XVYCC;

        case VID_QA_TYPE_WCG:
            return MTVDO_ARG_WCG;

        case VID_QA_TYPE_GAME_MODE:
            return MTVDO_ARG_GAME_MODE;

        case VID_QA_TYPE_BLUE_STRETCH:   
            return MTVDO_ARG_BLUE_STRETCH;

        case VID_QA_TYPE_MPEG_NR:        
            return MTVDO_ARG_MPEG_NR;

        case VID_QA_TYPE_BLACK_STRETCH:   
            return MTVDO_ARG_BLACK_STRETCH;

        case VID_QA_TYPE_WHITE_STRETCH:   
            return MTVDO_ARG_WHITE_STRETCH;

        default:
            Printf("VID_QA_TYPE Unknow %d\n", eVidQaType);
            ASSERT(0);
            return MTVDO_ARG_NS;
    }
}

static BOOL _VdpUpdateOverscan(UCHAR ucVdpId, UINT32* pui4_top, UINT32* pui4_bottom, UINT32* pui4_left, UINT32* pui4_right)
{
    if ((_pfOverscanFct != NULL) && fgEnableMwCb)
    {
        UINT16 u2Up;
        UINT16 u2Down;
        UINT16 u2Left;
        UINT16 u2Right;
        DRV_CUST_INPUT_SRC_TYPE_T eSrcType;
        DRV_CUSTOM_TIMING_INFO_T rTimingInfo;

        if (ucVdpId >= VDP_NS)
        {
            return FALSE;
        }

        eSrcType = _VdpGetDrvCustInputSrcType(ucVdpId, _arMwVdp[ucVdpId].e_type);

        rTimingInfo.ui2_width = _arMwVdp[ucVdpId].rSrcRes.ui2_width;
        rTimingInfo.ui2_height = _arMwVdp[ucVdpId].rSrcRes.ui2_height;
        rTimingInfo.ui4_fram_rate = _arMwVdp[ucVdpId].u4FrameRate;
        rTimingInfo.e_color_sys = _VdpGetDrvCustColorSys(ucVdpId);
        rTimingInfo.e_timing_type = _arMwVdp[ucVdpId].e_timing;

        if (_arMwVdp[ucVdpId].u4Interlace)
        {
            rTimingInfo.e_scan_mode = DRV_CUSTOM_TIMING_SCAN_MODE_INTERLACE;
        }
        else
        {
            rTimingInfo.e_scan_mode = DRV_CUSTOM_TIMING_SCAN_MODE_PROGRESSIVE;
        }

        u2Up = (UINT16)*pui4_top;
        u2Down = (UINT16)*pui4_bottom;
        u2Left = (UINT16)*pui4_left;
        u2Right = (UINT16)*pui4_right;
        _pfOverscanFct(
            ucVdpId,
            eSrcType,
            &rTimingInfo,
            &u2Up,
            &u2Down,
            &u2Left,
            &u2Right
        );

        if (*pui4_top != u2Up ||
            *pui4_bottom != u2Down ||
            *pui4_left != u2Left ||
            *pui4_right != u2Right)
        {
			PrintfOverscan("_VdpUpdateOverscan %d %d %d %d\n", u2Up, u2Down, u2Left, u2Right);
            *pui4_top = u2Up;
            *pui4_bottom = u2Down;
            *pui4_left = u2Left;
            *pui4_right = u2Right;

            return TRUE;
        }
    }

    return FALSE;
}

static void _VdpUpdateDispFmt(UCHAR ucVdpId)
{
    if ((_pfGetCurrentDispFmtFct != NULL) && fgEnableMwCb)
    {
        VID_PLA_DISP_FMT_T e_fmt;
        DRV_CUSTOM_VID_SRC_INFO_T rVidSrcInfo;

        rVidSrcInfo.ui2_vdp_id = (UINT16)ucVdpId;
        rVidSrcInfo.e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_INPUT_SRC;
        rVidSrcInfo.u.t_input_src.e_input_source = _VdpGetDrvCustInputSrcType(ucVdpId, _arMwVdp[ucVdpId].e_type);
        if (rVidSrcInfo.u.t_input_src.e_input_source == DRV_CUST_INPUT_SRC_TYPE_MULTIMEDIA)
        {
            // sys not ready for MM input source.
            // once sys is ready for this input source, you can remove this.
            rVidSrcInfo.u.t_input_src.e_input_source = DRV_CUST_INPUT_SRC_TYPE_DTV;
        }
        rVidSrcInfo.u.t_input_src.ui2_id = _arMwVdp[ucVdpId].u2SrcCompId;  // ucPortId=?
        rVidSrcInfo.u.t_input_src.e_timing_type = _arMwVdp[ucVdpId].e_timing;

        e_fmt = _arMwVdp[ucVdpId].eFmt;

        // The major objective of _VdpUpdateDispFmt() is to update _arMwVdp[ucVdpId].eFmt.
        // _pfXlatDispFmtExFct callback will use eFmt as index to query display format table in sys_build.
        // Since vdp_mw.c is in user space already, it's no difference to call _pfGetCurrentDispFmtFct to get eFmt from keeping old eFmt in vdp_mw.c.
        // Remove _pfGetCurrentDispFmtFct can just simply the call flow.      
        //Adam, MTAL porting removed this callback and use local variable to remember the eFmt for each source.
        _pfGetCurrentDispFmtFct(
            &_arMwVdp[ucVdpId].rSrcRes,
            &_arMwVdp[ucVdpId].rOutRes,
            &rVidSrcInfo,
            &e_fmt);

        if (_arMwVdp[ucVdpId].eFmt != e_fmt)
        {
            VDP_DBG_MSG("FMT CHANGE (%d) to (%d) VidSrcInfo (%d, %d, %d)\n",
                _arMwVdp[ucVdpId].eFmt,
                e_fmt,
                rVidSrcInfo.u.t_input_src.e_input_source,
                rVidSrcInfo.u.t_input_src.ui2_id,
                rVidSrcInfo.u.t_input_src.e_timing_type);

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            _VdpDispFmtLock();
#endif
            _arMwVdp[ucVdpId].eFmt = e_fmt;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            _VdpDispFmtUnLock();
#endif
        }
    }
}


static TV_DEC_TIMING_TYPE_T  _bVDPGetTimingType(UINT8 u1Path)
{
    TV_DEC_TIMING_TYPE_T      e_timing_type ;
    MTVDECEX_SIG_STATUS            eSignalStatus=MTVDECEX_VDO_UNKNOWN;

    MTVDECEX_GetVideoStatus(u1Path, &eSignalStatus);
    if ( eSignalStatus == MTVDECEX_VDO_NOSUPPORT)
    {
        e_timing_type = TV_DEC_TIMING_NOT_SUPPORT;
    }
    else
    {
        BOOL pfgPcTiming=FALSE;
        MTVDECEX_IsPCTiming(u1Path, &pfgPcTiming);
        e_timing_type = ( (pfgPcTiming)?
                                       TV_DEC_TIMING_GRAPHIC:
                                       TV_DEC_TIMING_VIDEO );
    }

    return e_timing_type ;
}

static void _VdpResolutionChangeNotify(UCHAR ucVdpId)
{
	if (_arMwVdp[ucVdpId].e_type == DRVT_VID_DEC &&
            _arMwVdp[ucVdpId].u2SrcCompId != (UINT16)MPV_COMP_NS)
    {
        MW_MPV_NOTIFY(_arMwVdp[ucVdpId].u2SrcCompId);
    }
}

static void _VdpBitRateChangeNotify(UCHAR ucVdpId)
{
    if (ucVdpId >= VDP_NS)
    {
        return;
    }
    _arMwVdp[ucVdpId].pfEosNfy(_arMwVdp[ucVdpId].pvEosTag, VID_PLA_COND_BIT_RATE_CHG, 0, 0);
}

static void _VdpUnmuteStatusNotify(UINT32 u4VdpId, UINT32 u4Arg1)
{
    //u4VdpId: 0 -> main path, 1 -> sub path
    //u4Arg1:  1 -> video all unmute (check is better)
    if (u4VdpId > 1)
    {
        Printf("{%s, %d} return reason: unsupport u4VdpId: %u \n", __FUNCTION__, __LINE__, u4VdpId);
        return;
    }

    if (NULL == _arMwVdp[u4VdpId].pfEosNfy)
    {
        Printf("{%s, %d} return reason: NULL == _arMwVdp[%u].pfEosNfy \n", __FUNCTION__, __LINE__, u4VdpId);
        return;
    }

    if (1 == u4Arg1)
    {
        Printf("{%s, %d} Notify VID_PLA_COND_MUTE_ALL_FINISHED to SM !!! u4VdpId %u, u4Arg1 %u \n", __FUNCTION__, __LINE__, u4VdpId, u4Arg1);
        _arMwVdp[u4VdpId].pfEosNfy(_arMwVdp[u4VdpId].pvEosTag, VID_PLA_COND_MUTE_ALL_FINISHED, 0, 0);
    }
    else
    {
        Printf("{%s, %d} u4VdpId %u. Strange u4Arg1 %u. Don't notify to SM \n", __FUNCTION__, __LINE__, u4VdpId, u4Arg1);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpSourceChangeNotify.
 */
//-----------------------------------------------------------------------------
//static UINT32 _VdpSourceChangeNotify(UCHAR ucVdpId, VDP_SOURCE_INFO_T rVdpSrcInfo)
static UINT32 _VdpSourceChangeNotify(UCHAR ucVdpId, MTVDO_SOURCE_CHG_INFO_T rVdpSrcInfo)
{
    BOOL fgRetTrue = 0;

    if (ucVdpId >= VDP_NS)
    {
        return 0;
    }
    VDP_DBG_MSG("_VdpSourceChangeNotify\n");

	/*For MSI added by lianming.lin 2012-03-16*/
	if ( _arMwVdp[ucVdpId].pfVdpSrcChg )
	{
		_arMwVdp[ucVdpId].pfVdpSrcChg(_arMwVdp[ucVdpId].pv_nfy_tag,
		                              ( VID_PLA_SRC_INFO_T *)&rVdpSrcInfo, 0 );
	}


    _arMwVdp[ucVdpId].rSrcRes.ui2_width = (UINT16)rVdpSrcInfo.u4SrcWidth;
    _arMwVdp[ucVdpId].rSrcRes.ui2_height = (UINT16)rVdpSrcInfo.u4SrcHeight;
    _arMwVdp[ucVdpId].rOutRes.ui2_width = (UINT16)rVdpSrcInfo.u4OutWidth;
    _arMwVdp[ucVdpId].rOutRes.ui2_height = (UINT16)rVdpSrcInfo.u4OutHeight;
    _arMwVdp[ucVdpId].u4FrameRate = rVdpSrcInfo.u4FrameRate;
    _arMwVdp[ucVdpId].u4Interlace = rVdpSrcInfo.u4Interlace;
    _arMwVdp[ucVdpId].e_timing = _bVDPGetTimingType(ucVdpId);

    if (_arMwVdp[ucVdpId].e_type == DRVT_UNKNOWN ||
        (_arMwVdp[ucVdpId].e_timing != TV_DEC_TIMING_VIDEO &&
         _arMwVdp[ucVdpId].e_timing != TV_DEC_TIMING_GRAPHIC))
    {
        //_arMwVdp[ucVdpId].ucSrcChgNfy = 1;
        Printf("(%s)(%d) e_type(%d) e_timing(%d) ASSERT!\n", __FUNCTION__, __LINE__,
                _arMwVdp[ucVdpId].e_type,
                _arMwVdp[ucVdpId].e_timing);
        ASSERT(0);
        return 0;
    }
    else
    {
        MTVDO_OVERSCAN_REGION_T rOverScan;

        MTVDO_GetOverScan((MTVDO_PATH_T)ucVdpId, (MTVDO_OVERSCAN_REGION_T *)&rOverScan);
        if((fgRetTrue |=_VdpUpdateOverscan(ucVdpId,
                                        &(rOverScan.u4Top),
                                        &(rOverScan.u4Bottom),
                                        &(rOverScan.u4Left),
                                        &(rOverScan.u4Right))))
        {
            MTVDO_SetOverScan((MTVDO_PATH_T)ucVdpId, &rOverScan);
        }
        _VdpUpdateDispFmt(ucVdpId);
    }

    if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
    {

        _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

        _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);

        fgRetTrue = TRUE;
    }


    if (_arMwVdp[ucVdpId].e_type == DRVT_VID_DEC &&
        _arMwVdp[ucVdpId].u2SrcCompId != (UINT16)MPV_COMP_NS)
    {
        VDP_DBG_MSG("MPV notify in _VdpSourceChangeNotify\n");
        MW_MPV_NOTIFY(_arMwVdp[ucVdpId].u2SrcCompId);
			
    }

    if (fgRetTrue)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

BOOL _VdpTest(UINT16 Dec_id)
{

    if(!((_arMwVdp[VDP_1].e_type == DRVT_VID_DEC) &&
                  (_arMwVdp[VDP_1].u2SrcCompId == (UINT16)Dec_id)))
    {
         return TRUE;
	}
	else
	{
         return FALSE;
	}
    return FALSE;
}

void _VdpDriverSrcRegionChgNotify(UCHAR ucVdpId, VDP_REGION_T rVdpSrcRegion)
{
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4Width;
    UINT32 u4Height;

   if ((_pfVideoSrcRegionFct != NULL) && fgEnableMwCb)
    {
        if (ucVdpId >= VDP_NS)
        {
            return;
        }


        u4X = rVdpSrcRegion.u4X;
        u4Y = rVdpSrcRegion.u4Y;
        u4Width = rVdpSrcRegion.u4Width;
        u4Height = rVdpSrcRegion.u4Height;
        _pfVideoSrcRegionFct(
            ucVdpId,
            u4X,
            u4Y,
            u4Width,
            u4Height);
    }
}
void _VdpDriverOutputRegionChgNotify(UCHAR ucVdpId, VDP_REGION_T rVdpOutRegion)
{  
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4Width;
    UINT32 u4Height;

   if ((_pfVideoOutRegionFct != NULL) && fgEnableMwCb)
    {
        if (ucVdpId >= VDP_NS)
        {
            return;
        }


        u4X = rVdpOutRegion.u4X;
        u4Y = rVdpOutRegion.u4Y;
        u4Width = rVdpOutRegion.u4Width;
        u4Height = rVdpOutRegion.u4Height;
        _pfVideoOutRegionFct(
            ucVdpId,
            u4X,
            u4Y,
            u4Width,
            u4Height);
    }
}

static void _VdpPlayDoneNotify(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2)
{
    MW_MPV_PlayDone_NOTIFY(_arMwVdp[u4VdpId].u2SrcCompId);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpAfdChangeNotify.
 */
//-----------------------------------------------------------------------------
static void _VdpAfdChangeNotify(UINT32 u4VdpId, UINT32 u4Afd, FBM_ASPECT_RATIO_T* prAspectRatio)
{
    UNUSED(u4Afd);
    UNUSED(prAspectRatio);

    VDP_DBG_MSG("_VdpAfdChangeNotify\n");
    if (_VdpXlatDispFmtCallBackMw(u4VdpId) != 0)
    {

        _VdpMwSetFinalOutputRegion((UCHAR)u4VdpId, &_arMwVdp[u4VdpId].rFinalOutput);
        _VdpMwSetFinalSourceRegion((UCHAR)u4VdpId, &_arMwVdp[u4VdpId].rFinalSource);
    }


    if (_arMwVdp[u4VdpId].e_type == DRVT_VID_DEC &&
        _arMwVdp[u4VdpId].u2SrcCompId != (UINT16)MPV_COMP_NS)
    {
        MW_MPV_NOTIFY(_arMwVdp[u4VdpId].u2SrcCompId);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpXlatDispFmtCallBackMw.
 */
//-----------------------------------------------------------------------------
static UINT32 _VdpXlatDispFmtCallBackMw(UCHAR ucVdpId)
{
    if (ucVdpId >= VDP_NS)
    {
        return 0;
    }

    if (_arMwVdp[ucVdpId].e_type == DRVT_UNKNOWN)
    {
        Printf("(%s)(%d) e_type(%d) VDP_MW is disconnect!\n", __FUNCTION__, __LINE__,
                _arMwVdp[ucVdpId].e_type);
        return 0;
    }

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    _VdpDispFmtLock();
#endif
    
#ifndef CC_VDP_FORCE_NO_MW_CB
    if ((_pfXlatDispFmtExFct[(UINT32)_arMwVdp[ucVdpId].eFmt] != NULL) && fgEnableMwCb)
    {
        VID_PLA_VID_REGION_T rSrcReg;
        VID_PLA_VID_REGION_T rDispReg;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
        VID_PLA_VID_REGION_T rUpdateDispReg;
#endif
        DRV_CUSTOM_VID_SRC_INFO_T rVidSrcInfo;
        MTVDEC_HDR_INFO_T rVdecHdrInfo;

        x_memset(&rVdecHdrInfo,0,sizeof(MTVDEC_HDR_INFO_T));
        MTVDEC_GetInfo(_arMwVdp[ucVdpId].ucEsId,&rVdecHdrInfo);
        
        MTVDO_ASPECTRATIO_T rAspectRatio;
        x_memset(&rAspectRatio,0,sizeof(MTVDO_ASPECTRATIO_T));
        MTVDO_GetAspectRatio(ucVdpId,&rAspectRatio);

        if (_arMwVdp[ucVdpId].ucDtv != 0)
        {
            rVidSrcInfo.ui2_vdp_id = (UINT16)ucVdpId;
            rVidSrcInfo.e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_MPEG;
            rVidSrcInfo.u.t_mpeg.ui1_afd = rAspectRatio.u4Afd;
            MTPMX_PANEL_GetResolution(&rVidSrcInfo.ui4_panel_width, &rVidSrcInfo.ui4_panel_height);
            rVidSrcInfo.u.t_mpeg.b_is_cropping = rVdecHdrInfo.fgCropping;
            rVidSrcInfo.u.t_mpeg.u4_cropping_x = rVdecHdrInfo.u4CropX;
            rVidSrcInfo.u.t_mpeg.u4_cropping_y = rVdecHdrInfo.u4CropY;
            rVidSrcInfo.u.t_mpeg.u4_cropping_width = rVdecHdrInfo.u4CropWidth;
            rVidSrcInfo.u.t_mpeg.u4_cropping_height = rVdecHdrInfo.u4CropHeight;
            rVidSrcInfo.u.t_mpeg.e_input_source = _VdpGetDrvCustInputSrcType(ucVdpId, _arMwVdp[ucVdpId].e_type);
            rVidSrcInfo.u.t_mpeg.b_is_display_aspect_ratio = TRUE; //Output  asp ratio w,h is DAR always. 
            
            if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_4_3)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 4;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 3;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_4_3;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_16_9)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 16;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 9;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_16_9;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_221_1)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 221;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 100;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_2_21_1;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_10_11)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 10*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 11*rVdecHdrInfo.u2Height;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_10_11;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_40_33)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 40*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 33*rVdecHdrInfo.u2Height;            
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_40_33;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_16_11)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 16*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 11*rVdecHdrInfo.u2Height;            
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_16_11;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_12_11)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 12*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 11*rVdecHdrInfo.u2Height;            
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_12_11;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_3_2)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 3*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 2*rVdecHdrInfo.u2Height;            
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_3_2;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_1_1)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 1*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 1*rVdecHdrInfo.u2Height;            
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_1_1;
            }
            else if(rVdecHdrInfo.eAspectRatio == MTVDEC_ASPECT_RATIO_TRANSMIT)
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = rVdecHdrInfo.u4DarWidth;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = rVdecHdrInfo.u4DarHeight;
                rVidSrcInfo.u.t_mpeg.b_is_display_aspect_ratio = !rVdecHdrInfo.fgPixelAspectRatio;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_USR;
            }
            else
            {
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_w = 1*rVdecHdrInfo.u2Width;
                rVidSrcInfo.u.t_mpeg.u4_aspect_ratio_h = 1*rVdecHdrInfo.u2Height;
                rVidSrcInfo.u.t_mpeg.b_is_display_aspect_ratio = FALSE;
                rVidSrcInfo.u.t_mpeg.e_asp_ratio = VID_DEC_SRC_ASPECT_RATIO_PXL_1_1;
            }	
        }
        else
        {
            rVidSrcInfo = _arMwVidSrcInfo[ucVdpId];
        }
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
        rUpdateDispReg = _arMwVdp[ucVdpId].rTrueFullOutput;
#endif

        _pfXlatDispFmtExFct[(UINT32)_arMwVdp[ucVdpId].eFmt](
            &_arMwVdp[ucVdpId].rSrcRes,
            &_arMwVdp[ucVdpId].rOutRes,
            &rVidSrcInfo,
            &rSrcReg,
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            &rDispReg,
            &rUpdateDispReg);
#else
            &rDispReg);
#endif

        if ((rSrcReg.ui4_x != _arMwVdp[ucVdpId].rXlatSource.ui4_x) ||
            (rSrcReg.ui4_y != _arMwVdp[ucVdpId].rXlatSource.ui4_y) ||
            (rSrcReg.ui4_width != _arMwVdp[ucVdpId].rXlatSource.ui4_width) ||
            (rSrcReg.ui4_height != _arMwVdp[ucVdpId].rXlatSource.ui4_height) ||
            (rDispReg.ui4_x != _arMwVdp[ucVdpId].rXlatOutput.ui4_x) ||
            (rDispReg.ui4_y != _arMwVdp[ucVdpId].rXlatOutput.ui4_y) ||
            (rDispReg.ui4_width != _arMwVdp[ucVdpId].rXlatOutput.ui4_width) ||
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            (rDispReg.ui4_height != _arMwVdp[ucVdpId].rXlatOutput.ui4_height) ||
            (rUpdateDispReg.ui4_x != _arMwVdp[ucVdpId].rFullOutput.ui4_x) ||
            (rUpdateDispReg.ui4_y != _arMwVdp[ucVdpId].rFullOutput.ui4_y) ||
            (rUpdateDispReg.ui4_width != _arMwVdp[ucVdpId].rFullOutput.ui4_width) ||
            (rUpdateDispReg.ui4_height != _arMwVdp[ucVdpId].rFullOutput.ui4_height))
#else
            (rDispReg.ui4_height != _arMwVdp[ucVdpId].rXlatOutput.ui4_height))
#endif
        {
            // New Source/Output Region

            _arMwVdp[ucVdpId].rXlatSource = rSrcReg;
            _arMwVdp[ucVdpId].rXlatOutput = rDispReg;
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            _arMwVdp[ucVdpId].rFullOutput = rUpdateDispReg;
#endif

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            if (_arMwVdp[ucVdpId].eFmt != VID_PLA_DISP_FMT_NORMAL)
            {
                _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
            }
            else
            {
                _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rTrueFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
            }
#else
            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatOutput, &_arMwVdp[ucVdpId].rFullOutput, &_arMwVdp[ucVdpId].rFinalOutput);
#endif
            _VdpMwCalculateFinalRegion(&_arMwVdp[ucVdpId].rXlatSource, &_arMwVdp[ucVdpId].rFullSource, &_arMwVdp[ucVdpId].rFinalSource);

#if 0//To debug the ap or sys_cfg outregion for aspectratio issue 
            //_arMwVdp[ucVdpId].rXlatOutput is from AP->vdp_set->dst region
            //_arMwVdp[ucVdpId].rFullOutput is from AP->vdp_set->picture aspectratio
            //_arMwVdp[ucVdpId].rFinalOutput is from _VdpMwCalculateFinalRegion
            Printf("_arMwVdp[ucVdpId].eFmt(%d) VID_PLA_DISP_FMT_NORMAL(%d).\n", _arMwVdp[ucVdpId].eFmt, VID_PLA_DISP_FMT_NORMAL);
            Printf("rFinalOutput ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rFinalOutput.ui4_x,_arMwVdp[ucVdpId].rFinalOutput.ui4_y,
                                _arMwVdp[ucVdpId].rFinalOutput.ui4_width,_arMwVdp[ucVdpId].rFinalOutput.ui4_height);
            Printf("rFullOutput ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rFullOutput.ui4_x,_arMwVdp[ucVdpId].rFullOutput.ui4_y,
                                _arMwVdp[ucVdpId].rFullOutput.ui4_width,_arMwVdp[ucVdpId].rFullOutput.ui4_height);
            Printf("rXlatOutput ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rXlatOutput.ui4_x,_arMwVdp[ucVdpId].rXlatOutput.ui4_y,
                                _arMwVdp[ucVdpId].rXlatOutput.ui4_width,_arMwVdp[ucVdpId].rXlatOutput.ui4_height);
            Printf("rFinalSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rFinalSource.ui4_x,_arMwVdp[ucVdpId].rFinalSource.ui4_y,
                                _arMwVdp[ucVdpId].rFinalSource.ui4_width,_arMwVdp[ucVdpId].rFinalSource.ui4_height);
            Printf("rFullSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rFullSource.ui4_x,_arMwVdp[ucVdpId].rFullSource.ui4_y,
                                _arMwVdp[ucVdpId].rFullSource.ui4_width,_arMwVdp[ucVdpId].rFullSource.ui4_height);
            Printf("rXlatSource ui4_x; ui4_y; ui4_width; ui4_height(%d, %d, %d, %d)\n", _arMwVdp[ucVdpId].rXlatSource.ui4_x,_arMwVdp[ucVdpId].rXlatSource.ui4_y,
                                _arMwVdp[ucVdpId].rXlatSource.ui4_width,_arMwVdp[ucVdpId].rXlatSource.ui4_height);
#endif
            PrintfSrcRegion(ucVdpId, _arMwVdp[ucVdpId].rXlatSource);
            PrintfSrcRegion(ucVdpId, _arMwVdp[ucVdpId].rFullSource);
            PrintfSrcRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalSource);
            
            PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rXlatOutput);
            PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFullOutput);
            PrintfOutRegion(ucVdpId, _arMwVdp[ucVdpId].rFinalOutput);
            
#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
            _VdpDispFmtUnLock();
#endif

            return 1;
        }
    }
#endif

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    _VdpDispFmtUnLock();
#endif

    return 0;
}

static void _VdpMwCalculateFinalRegion(VID_PLA_VID_REGION_T *pXlatRegion, VID_PLA_VID_REGION_T *pFullRegion, VID_PLA_VID_REGION_T *pFinalRegion)
{
    // X
    pFinalRegion->ui4_x = pFullRegion->ui4_x + ((pXlatRegion->ui4_x * pFullRegion->ui4_width) / VDP_MAX_REGION_WIDTH);

    // Y
    pFinalRegion->ui4_y = pFullRegion->ui4_y + ((pXlatRegion->ui4_y * pFullRegion->ui4_height) / VDP_MAX_REGION_HEIGHT);

    // Width
    pFinalRegion->ui4_width = (pXlatRegion->ui4_width * pFullRegion->ui4_width) / VDP_MAX_REGION_WIDTH;

    // Height
    pFinalRegion->ui4_height = (pXlatRegion->ui4_height * pFullRegion->ui4_height) / VDP_MAX_REGION_HEIGHT;
}


static MT_RESULT_T _VdpMwSetFinalSourceRegion(UCHAR ucVdpId, VID_PLA_VID_REGION_T *pRegion)
{
    MTVDO_REGION_T rSrcRegion;

    rSrcRegion.u4X = (UINT32) pRegion->ui4_x;
    rSrcRegion.u4Y = (UINT32) pRegion->ui4_y;
    rSrcRegion.u4Width = (UINT32) pRegion->ui4_width;
    rSrcRegion.u4Height = (UINT32) pRegion->ui4_height;

    return MTVDO_SetSrcRegion((MTVDO_PATH_T)ucVdpId, &rSrcRegion);
}

static MT_RESULT_T _VdpMwSetFinalOutputRegion(UCHAR ucVdpId, VID_PLA_VID_REGION_T *pRegion)
{
    MTVDO_REGION_T rOutRegion;
    
    rOutRegion.u4X = (UINT32) pRegion->ui4_x;
    rOutRegion.u4Y = (UINT32) pRegion->ui4_y;
    rOutRegion.u4Width = (UINT32) pRegion->ui4_width;
    rOutRegion.u4Height = (UINT32) pRegion->ui4_height;
    
    return MTVDO_SetOutRegion((MTVDO_PATH_T)ucVdpId, &rOutRegion);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpGetSrcTypeIdx.
 */
//-----------------------------------------------------------------------------
static UINT8 _VdpGetSrcTypeIdx( DRV_TYPE_T  e_drv_type )
{
    switch( e_drv_type )
    {
    case DRVT_AVC_COMP_VIDEO:
        return (UINT8)EVDP_VIS_AVC_COMP_VIDEO;

    case DRVT_AVC_S_VIDEO:
        return (UINT8)EVDP_VIS_AVC_S_VIDEO;

    case DRVT_AVC_Y_PB_PR:
        return (UINT8)EVDP_VIS_AVC_Y_PB_PR;

    case DRVT_AVC_VGA:
        return (UINT8)EVDP_VIS_AVC_VGA;

    case DRVT_AVC_SCART:
        return (UINT8)EVDP_VIS_AVC_SCART;

    case DRVT_AVC_DVI:
        return (UINT8)EVDP_VIS_AVC_DVI;

    case DRVT_AVC_HDMI:
        return (UINT8)EVDP_VIS_AVC_HDMI;

    case DRVT_TUNER_SAT_DIG:
    case DRVT_TUNER_CAB_DIG:
    case DRVT_TUNER_TER_DIG:
        return (UINT8)EVDP_VIS_TUNER_DIGITAL;

    case DRVT_TUNER_SAT_ANA:
    case DRVT_TUNER_CAB_ANA:
    case DRVT_TUNER_TER_ANA:
        return (UINT8)EVDP_VIS_TUNER_ANALOG;

    default:
        return (UINT8)EVDP_VIS_MAX;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpParseMinMaxDftTbl.
 */
//-----------------------------------------------------------------------------
static BOOL _VdpParseMinMaxDftTbl(
    UINT8                    ui1_src_idx,
    VID_QA_MIN_MAX_DFT_T*    pt_min_max_dft_tbl )
{
LINT_EXT_HEADER_BEGIN

    MTVDO_ARG_TYPE_T eUiVqItem;
    MTVDO_UI_RANGE_T rUiVqMinMaxDft;
    VID_QA_MIN_MAX_DFT_T *pt_iter = pt_min_max_dft_tbl;

    if ( !pt_min_max_dft_tbl )
    {
        return FALSE;
    }

    while( !VID_MIN_MAX_DFT_IS_END(pt_iter) )
    {
        if ( pt_iter->e_vid_qa_type >= VID_QA_TYPE_MAX ||
             pt_iter->e_vid_qa_type <= VID_QA_TYPE_UNKNOWN )
        {
            return FALSE;
        }

        // set UI min, max and dft values to driver
        eUiVqItem = _VdpMwVidQaType2UiVqItem(pt_iter->e_vid_qa_type);
        if (eUiVqItem >= MTVDO_ARG_NS)
        {
            return FALSE;
        }

        rUiVqMinMaxDft.i4Min = pt_iter->i4_min;
        rUiVqMinMaxDft.i4Max = pt_iter->i4_max;
        rUiVqMinMaxDft.i4Dft = pt_iter->i4_default;

        switch (pt_iter->e_vid_qa_type)
        {
            case VID_QA_TYPE_COLOR_GAIN:
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_R_GAIN, &rUiVqMinMaxDft);                        
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_G_GAIN, &rUiVqMinMaxDft);        
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_B_GAIN, &rUiVqMinMaxDft);                        
                break;
            case VID_QA_TYPE_COLOR_OFFSET:
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_R_OFFSET, &rUiVqMinMaxDft);                        
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_G_OFFSET, &rUiVqMinMaxDft);        
                MTVDO_SetPeUiRangeParam(ui1_src_idx, MTVDO_ARG_B_OFFSET, &rUiVqMinMaxDft);                        
                break;
            default:
                MTVDO_SetPeUiRangeParam(ui1_src_idx, eUiVqItem, &rUiVqMinMaxDft);                
        }

        pt_iter++;
    }

    return TRUE;
LINT_EXT_HEADER_END
}

//-----------------------------------------------------------------------------
/** Brief of _VdpParseVidQaTbl.
 */
//-----------------------------------------------------------------------------
static BOOL _VdpParseVidQaTbl( VID_QA_TBL_T*    pt_vid_qa_tbl )
{
LINT_EXT_HEADER_BEGIN
    VID_QA_TBL_T*    pt_iter = pt_vid_qa_tbl;

    if ( !pt_vid_qa_tbl )
    {
        return FALSE;
    }

    while( !VID_QA_TBL_IS_END(pt_iter) )
    {
        UINT8  ui1_idx = _VdpGetSrcTypeIdx( pt_iter->e_src_type );

        if (ui1_idx >= (UINT8)EVDP_VIS_MAX)
        {
            return FALSE;
        }
        
        if (!_VdpParseMinMaxDftTbl(ui1_idx, pt_vid_qa_tbl->pt_min_max_dft_lst))
        {
            return FALSE;
        }

        pt_iter++;
    }

    return TRUE;
LINT_EXT_HEADER_END
}

//-----------------------------------------------------------------------------
/** Brief of _VdpDrvCustParse.
 */
//-----------------------------------------------------------------------------
static DRVP_TYPE_T _VdpDrvCustParse(
    const DRV_CUSTOM_T*     pt_drv_cust,
    VOID*                   pv_tag )
{
LINT_EXT_HEADER_BEGIN
    UNUSED(pv_tag);

    if (pt_drv_cust == NULL)
    {
        return DRVP_ERROR;
    }

    if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_XLAT_DISP_FMT_EX)
    {
        DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_EX_INFO_T* prFmtInfo;

        if (pt_drv_cust->ui2_id != MAX_COMP_ID)
        {
            return DRVP_ERROR;
        }

        if (pt_drv_cust->pv_arg == NULL)
        {
            return DRVP_ERROR;
        }

        prFmtInfo = (DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_EX_INFO_T*) (pt_drv_cust->pv_arg);

        if(((UINT32) prFmtInfo->e_disp_fmt) >= CC_DISP_FMT_ENUM_SIZE)
        {
            return DRVP_ERROR;
        }
        _pfXlatDispFmtExFct[(UINT32) prFmtInfo->e_disp_fmt] = prFmtInfo->pf_xlat_disp_fmt;
    }
    else
    if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_XLAT_DISP_FMT)
    {
        #if 0
        DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_INFO_T* prFmtInfo;

        if (pt_drv_cust->ui2_id != MAX_COMP_ID)
        {
            return DRVP_ERROR;
        }

        if (pt_drv_cust->pv_arg == NULL)
        {
            return DRVP_ERROR;
        }

        prFmtInfo = (DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_INFO_T*) (pt_drv_cust->pv_arg);

        _pfXlatDispFmtFct[(UINT32) prFmtInfo->e_disp_fmt] = prFmtInfo->pf_xlat_disp_fmt;
        #else
        return DRVP_ERROR;
        #endif
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_VID_QA_MIN_MAX_DFT)
    {
        BOOL    b_success;

        b_success = _VdpParseVidQaTbl( (VID_QA_TBL_T*)(pt_drv_cust->pv_arg) );
        if ( !b_success )
        {
            return DRVP_ERROR;
        }
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_GET_CURRENT_DISP_FMT)
    {
        // unnecessary to get display format from sys_build. remove the callback function.
        //_pfGetCurrentDispFmtFct = (d_vid_pla_get_current_disp_fmt_fct) pt_drv_cust->pv_arg;
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_OVERSCAN_CB)
    {
        _pfOverscanFct = (d_vid_pla_overscan_fct) pt_drv_cust->pv_arg;
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_UPDATE_OVERSCAN_TBL)
    {
        _pfUpdateOverscanTbl = (d_vid_pla_update_overscan_tbl_fct) pt_drv_cust->pv_arg;
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_VID_QA_TBL_QUERY_CB)
    {
        _pfVidQaTblQueryFct = (d_vid_pla_vid_qa_tbl_query_fct) pt_drv_cust->pv_arg;
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_SRC_REGION_CHG_CB)
    {
        _pfVideoSrcRegionFct = (d_vid_pla_source_region_fct) pt_drv_cust->pv_arg;
    }
    else if (pt_drv_cust->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_OUT_REGION_CHG_CB)
    {
        _pfVideoOutRegionFct = (d_vid_pla_Output_region_fct) pt_drv_cust->pv_arg;
    }
    else
    {
        return DRVP_ERROR;
    }

    return DRVP_NEXT;
LINT_EXT_HEADER_END
}

static DRV_CUSTOM_COLOR_SYS_TYPE_T _VdpGetDrvCustColorSys(UCHAR ucVdpId)
{
    MTVDECEX_CS_T eColSys ;

    if ( _arMwVdp[ucVdpId].e_type == DRVT_UNKNOWN ||
         _arMwVdp[ucVdpId].ucDtv == 1 )
    {
        return DRV_CUSTOM_COLOR_SYS_UNKNOWN;
    }
    
    MTVDECEX_GetColorSystem(&eColSys);

    switch (eColSys)
    {
    case MTVDECEX_CS_PAL_N:
        return DRV_CUSTOM_COLOR_SYS_PAL_N;
    case MTVDECEX_CS_PAL:
        return DRV_CUSTOM_COLOR_SYS_PAL;
    case MTVDECEX_CS_PAL_M:
        return DRV_CUSTOM_COLOR_SYS_PAL_M;
    case MTVDECEX_CS_NTSC358:
        return DRV_CUSTOM_COLOR_SYS_NTSC;
    case MTVDECEX_CS_SECAM:
        return DRV_CUSTOM_COLOR_SYS_SECAM;
    case MTVDECEX_CS_NTSC443:
        return DRV_CUSTOM_COLOR_SYS_NTSC_443;
    case MTVDECEX_CS_PAL_60:
        return DRV_CUSTOM_COLOR_SYS_PAL_60;
    case MTVDECEX_CS_UNSTABLE:
    default:
        return DRV_CUSTOM_COLOR_SYS_UNKNOWN;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpGetDrvCustInputSrcType.
 */
//-----------------------------------------------------------------------------
static DRV_CUST_INPUT_SRC_TYPE_T _VdpGetDrvCustInputSrcType(UCHAR ucVdpId, DRV_TYPE_T e_type)
{
    switch (e_type)
    {
        case DRVT_AVC_COMP_VIDEO:
        case DRVT_AVC_S_VIDEO:
            return DRV_CUST_INPUT_SRC_TYPE_CVBS;

        case DRVT_AVC_Y_PB_PR:
            return DRV_CUST_INPUT_SRC_TYPE_YPBPR;

        case DRVT_AVC_VGA:
            return DRV_CUST_INPUT_SRC_TYPE_VGA;

        case DRVT_AVC_SCART:
            return DRV_CUST_INPUT_SRC_TYPE_SCART;

        case DRVT_AVC_DVI:
            return DRV_CUST_INPUT_SRC_TYPE_DVI;

        case DRVT_AVC_HDMI:
            return DRV_CUST_INPUT_SRC_TYPE_HDMI;

        case DRVT_VID_DEC:
#ifdef ENABLE_MULTIMEDIA
            /*check MM mode*/
            { 
                UINT8 u1PlayMode = (UINT8)(MTB2R_FBG_MM_MODE);
                MTB2R_GetPlayMode(ucVdpId,&u1PlayMode);
                
                if (u1PlayMode == (UINT8)MTB2R_FBG_MM_MODE)
                {
                    DRV_CUST_INPUT_SRC_TYPE_T eSrcType = DRV_CUST_INPUT_SRC_TYPE_MULTIMEDIA;

#ifdef TIME_SHIFT_SUPPORT
                    UINT32 u4Tmp=0;

#ifdef CC_53XX_SWDMX_V2
                    if(MTSWDMX_GetInfo(MTSWDMX_PRIMARY_SOURCE, MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#else
                    if(MTSWDMX_GetInfo(MTSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0) == MTR_OK)
#endif
                    {
                        if((u4Tmp == (UINT32)MTSWDMX_FMT_MPEG2_TS_TIME_SHIFT)||          //DTV00145200
                            (u4Tmp == (UINT32)MTSWDMX_FMT_MPEG2_TS_ENCRYPT_192))         //when pvr overscan is set as dtv

                        {

                            eSrcType = DRV_CUST_INPUT_SRC_TYPE_TIMESHIFT;
                        }
                    }
#endif
                    return eSrcType;
                }
            }
#endif
        case DRVT_TUNER_SAT_DIG:
        case DRVT_TUNER_CAB_DIG:
        case DRVT_TUNER_TER_DIG:
            return DRV_CUST_INPUT_SRC_TYPE_DTV;

        case DRVT_TUNER_SAT_ANA:
        case DRVT_TUNER_CAB_ANA:
        case DRVT_TUNER_TER_ANA:
            return DRV_CUST_INPUT_SRC_TYPE_ATV;

        default:
            return DRV_CUST_INPUT_SRC_TYPE_UNKNOWN;
    }
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
void _MW_VDP_HDMI_AVI_INFOFRAME_CB(UCHAR ucVdpId, DRV_CUSTOM_VID_SRC_HDMI_AVI_INFOFRAME_INFO_T u2Data)
{
    if (ucVdpId < VDP_COMP_NS)
    {

        if ((_arMwVidSrcInfo[ucVdpId].e_type != DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_AVI_INFOFRAME)
            ||(_arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_1 != u2Data.ui1_byte_1)
            ||(_arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_2 != u2Data.ui1_byte_2)
            ||(_arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_3 != u2Data.ui1_byte_3))
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_AVI_INFOFRAME;
            _arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_1 = u2Data.ui1_byte_1;
            _arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_2 = u2Data.ui1_byte_2;
            _arMwVidSrcInfo[ucVdpId].u.t_hdmi_avi_infoframe.ui1_byte_3 = u2Data.ui1_byte_3;

            if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
            {

                _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

                _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
            }
        }
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
void _MW_VDP_HDMI_AFD_CB(UCHAR ucVdpId, UINT16 u2Data)
{
    if (ucVdpId < VDP_COMP_NS)
    {

        if ((_arMwVidSrcInfo[ucVdpId].e_type != DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_AFD) ||
            (_arMwVidSrcInfo[ucVdpId].u.ui1_hdmi_afd != (UINT8)u2Data))
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_AFD;
            _arMwVidSrcInfo[ucVdpId].u.ui1_hdmi_afd = (UINT8)u2Data;

            if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
            {

                _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

                _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
            }
        }
    }
    else
    {
        ASSERT(0);
    }
}

void _MW_VDP_HDMI_CB(UCHAR ucVdpId, UINT16 u2Data)
{
    if (ucVdpId < VDP_COMP_NS)
    {

        if ((_arMwVidSrcInfo[ucVdpId].e_type != DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_ASP_RATIO) ||
            (_arMwVidSrcInfo[ucVdpId].u.e_hdmi_asp != (DRV_CUSTOM_HDMI_ASP_RATIO_T)u2Data))
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_HDMI_ASP_RATIO;
            _arMwVidSrcInfo[ucVdpId].u.e_hdmi_asp = (DRV_CUSTOM_HDMI_ASP_RATIO_T)u2Data;

            if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
            {

                _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

                _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
            }
        }

    }
    else
    {
        ASSERT(0);
    }
}

void _VdpDispSupportCallback(MTVDO_PATH_T eVdpId, UINT32 u4Status)
{
    UINT32 u4VdpId;

    u4VdpId = (UINT32)eVdpId;
    
    _arMwVdp[u4VdpId].pfEosNfy(_arMwVdp[u4VdpId].pvEosTag, VID_PLA_COND_DISP_MODE, u4Status, 0);
}

#ifdef CC_53XX_SWDMX_V2
void _VdpEosCallback(UINT32 u4VdpId, MTB2R_COND_T eCond, BOOL fgFastForward,UINT32 u4AttachedSrcId)
{
    UNUSED(u4AttachedSrcId);
 if(_arMwVdp[u4VdpId].pfEosNfy)
 {
         #ifndef    MULTIMEDIA_AUTOTEST_SUPPORT
             _arMwVdp[u4VdpId].pfEosNfy(_arMwVdp[u4VdpId].pvEosTag, (VID_PLA_COND_T)eCond, (UINT32)fgFastForward, 0);
         #else
             if(fgFastForward)       //only send eos notify to mw for  forward not for backward
             {
               _arMwVdp[u4VdpId].pfEosNfy(_arMwVdp[u4VdpId].pvEosTag, (VID_PLA_COND_T)eCond, (UINT32)fgFastForward, 0);
             }
         #endif 
    #ifdef MULTIMEDIA_AUTOTEST_SUPPORT 
        if(_arMwVdp[u4VdpId].pfAtEosNfy)
        {
           _arMwVdp[u4VdpId].pfAtEosNfy(_arMwVdp[u4VdpId].pvATEosTag,(VID_PLA_COND_T)eCond,1,0);
        }
   #endif
 }
}

#else
void _VdpEosCallback(UINT32 u4VdpId, MTB2R_COND_T eCond, BOOL fgFastForward)
{
    _arMwVdp[u4VdpId].pfEosNfy(_arMwVdp[u4VdpId].pvEosTag, (VID_PLA_COND_T)eCond, (UINT32)fgFastForward, 0);
}
#endif

void _MW_VDP_WSS525_CB(UCHAR ucVdpId, UINT16 u2Data)
{
    if (ucVdpId < VDP_COMP_NS)
    {
        if ((_arMwVidSrcInfo[ucVdpId].e_type != DRV_CUSTOM_VID_SRC_INFO_TYPE_NTSC_WSS) ||
            (_arMwVidSrcInfo[ucVdpId].u.ui4_ntsc_wss != u2Data))
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_NTSC_WSS;
            _arMwVidSrcInfo[ucVdpId].u.ui4_ntsc_wss = u2Data;
            _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = DRV_CUSTOM_SCART_PIN_8_UNKNOWN;
            VDP_DBG_MSG("_MW_VDP_WSS525_CB\n");

            if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
            {

                _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

                _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
            }
        }

    }
    else
    {
        ASSERT(0);
    }
}

void _MW_VDP_WSS625_CB(UCHAR ucVdpId, UINT16 u2Data)
{
    if (ucVdpId < VDP_COMP_NS)
    {
        if ((_arMwVidSrcInfo[ucVdpId].e_type != DRV_CUSTOM_VID_SRC_INFO_TYPE_PAL_SECAM_WSS_SCART) ||
            (_ui2_pal_secam_wss != u2Data))
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_PAL_SECAM_WSS_SCART;
            _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = u2Data;
            _ui2_pal_secam_wss = u2Data;
            _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = DRV_CUSTOM_SCART_PIN_8_UNKNOWN;
            VDP_DBG_MSG("_MW_VDP_WSS625_CB\n");

            if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
            {

                _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

                _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
            }
        }

    }
    else
    {
        ASSERT(0);
    }
}

void _MW_VDP_SCART_PIN_8_CB(UCHAR ucVdpId, DRV_CUSTOM_SCART_PIN_8_T t_scart_pin_8)
{
    if (ucVdpId < VDP_COMP_NS)
    {
        
        if(t_scart_pin_8 == DRV_CUSTOM_SCART_PIN_8_UNKNOWN && _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 == DRV_CUSTOM_SCART_PIN_8_UNKNOWN)
        {
           return;
        }

        if(t_scart_pin_8 != DRV_CUSTOM_SCART_PIN_8_UNKNOWN)
        {
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_PAL_SECAM_WSS_SCART;
        }
        else
        {
            // notify unknown for canal+ tuner case
            _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_UNKNOWN;
        }
        _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = t_scart_pin_8;
        _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = 0;
        VDP_DBG_MSG("_MW_VDP_SCART_PIN_8_CB\n");
        if (_VdpXlatDispFmtCallBackMw(ucVdpId) != 0)
        {

            _VdpMwSetFinalSourceRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalSource);

            _VdpMwSetFinalOutputRegion(ucVdpId, &_arMwVdp[ucVdpId].rFinalOutput);
        }
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of MW_VDP_Init.
 */
//-----------------------------------------------------------------------------
INT32 MW_VDP_Init( DRV_CUSTOM_DIR_T* pt_drv_cust_dir )
{
    LINT_SUPPRESS_BRACE(818)

    UCHAR ucVdpId;
    UINT32 u4Idx;
    INT32 i4_return;
    DRV_COMP_REG_T tVdpCompReg;
    DRV_COMP_FCT_TBL_T tVdpFctTbl;

    /* Check enum value */
    ASSERT((UINT32)VID_PLA_NORMAL == VDP_MODE_NORMAL);
    ASSERT((UINT32)VID_PLA_BLANK == VDP_MODE_BG);
    ASSERT((UINT32)VID_PLA_QUAD_VIDEO == VDP_MODE_QV);
    ASSERT((UINT32)VID_PLA_DEINT == VDP_MODE_DEINT);
    ASSERT((UINT32) VID_PLA_DISP_FMT_MAX <= CC_DISP_FMT_ENUM_SIZE );

    /* Init Local Variable */
    for (ucVdpId = 0; ucVdpId < VDP_COMP_NS; ucVdpId++)
    {
        _arMwVdp[ucVdpId] = _rMwVdpDefault;

        _arMwVidSrcInfo[ucVdpId].ui2_vdp_id = (UINT16)ucVdpId;
        _arMwVidSrcInfo[ucVdpId].e_type = DRV_CUSTOM_VID_SRC_INFO_TYPE_UNKNOWN;
        _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.ui2_pal_secam_wss = 0;
        _ui2_pal_secam_wss = 0;
        _arMwVidSrcInfo[ucVdpId].u.t_wss_scart.e_scart_pin_8 = DRV_CUSTOM_SCART_PIN_8_UNKNOWN;
    }

    for (u4Idx = 0; u4Idx < (UCHAR)MPV_COMP_NS; u4Idx++)
    {
#ifdef CC_3D_MM_DS_SUPPORT        
        _arMwComp3dInfo[u4Idx] = FALSE;
#endif        
    }

    /* Init */
    MTVDO_Init();

    // get semaphore instead of create semaphore, Upper Layer, DTV, & NPTV share one mutex to prevent deadlock
    //_hMutex = VDP_GetMutex();

#ifdef SYS_OVERRIDE_DISP_REGION_SUPPORT
    _VdpDispFmtLockInit();
#endif

    /* Source Change Notification */
    MTVDO_RegCbFunc(VDO_CB_FUNC_SRC_CHG_MW_IND, (UINT32)_VdpSourceChangeNotify, 0, 0);

    Printf("register src chg call back in vdp_if2\n");
    MTVDO_RegCbFunc(VDO_CB_FUNC_SRC_RANGE_CHG_IND, (UINT32)_VdpDriverSrcRegionChgNotify, 0, 0);
    MTVDO_RegCbFunc(VDO_CB_FUNC_OUT_RANGE_CHG_IND, (UINT32)_VdpDriverSrcRegionChgNotify, 0, 0);


    MTVDO_RegCbFunc(VDO_CB_FUNC_DISP_SUPPORT_IND, (UINT32)_VdpDispSupportCallback, 0, 0);
	MTVDO_RegCbFunc(VDO_CB_FUNC_RES_CHG_ING, (UINT32)_VdpResolutionChangeNotify, 0, 0);
    MTVDO_RegCbFunc(VDO_CB_FUNC_BIT_TATE_CHG_ING, (UINT32)_VdpBitRateChangeNotify, 0, 0);
    /* Afd Change Notification */
    MTVDO_RegCbFunc(VDO_CB_FUNC_AFD_IND, (UINT32)_VdpAfdChangeNotify, 0, 0);

    /* Play Done Notification */
    MTVDO_RegCbFunc(VDO_CB_FUNC_PLAY_DONE_IND, (UINT32)_VdpPlayDoneNotify, 0, 0);

    MTVDO_RegCbFunc(VDO_CB_FUNC_UNMUTE_IND, (UINT32)_VdpUnmuteStatusNotify, 0, 0);

    /* Setup component ID */
    tVdpCompReg.e_type = DRVT_VID_PLANE;
    tVdpCompReg.e_id_type = ID_TYPE_RANGE;
    tVdpCompReg.u.t_range.ui2_first_id = VDP_COMP_1;
    tVdpCompReg.u.t_range.ui2_delta_id = 1;
    tVdpCompReg.u.t_range.ui2_num_of_ids = VDP_COMP_NS;
    tVdpCompReg.u.t_range.pv_first_tag = NULL;
    tVdpCompReg.u.t_range.pv_delta_tag = NULL;
    tVdpCompReg.u.t_range.ui1_port = VDP_MAX_INPORT_NS;

    /* Setup component function table */
    tVdpFctTbl.pf_rm_connect = _VdpConnect;
    tVdpFctTbl.pf_rm_disconnect = _VdpDisconnect;
    tVdpFctTbl.pf_rm_get = _VdpGet;
    tVdpFctTbl.pf_rm_set = _VdpSet;

    /* Register component with the Resource Manager database */
    i4_return = x_rm_reg_comp(&tVdpCompReg,
                                VDP_MAX_INPORT_NS,
                                1,
                                DRVN_VID_PLANE,            //"VDP",
                                DRV_FLAG_SINGLE_CONN_ON_OUTPUT,
                                &tVdpFctTbl,
                                NULL,
                                0);
    if (i4_return != RMR_OK)
    {
        return i4_return;
    }

    /* Clean custom table */
    for (u4Idx = 0; u4Idx < CC_DISP_FMT_ENUM_SIZE; u4Idx++)
    {
        _pfXlatDispFmtExFct[u4Idx] = NULL;
    }
    _pfGetCurrentDispFmtFct = NULL;
    _pfOverscanFct = NULL;

    //After discuss with AP, the function is not used now.
    _pfUpdateOverscanTbl = NULL;

    _pfVidQaTblQueryFct = NULL;

    _pfVideoSrcRegionFct = NULL;
    _pfVideoOutRegionFct = NULL;


    /* Parse custom table */
    if (pt_drv_cust_dir != NULL)
    {
        if (d_drv_cust_parse(
            pt_drv_cust_dir->pt_custom,
            _VdpDrvCustParse,
            NULL ) != 0)
        {
            return (RMR_DRV_ERROR);
        }
    }    

    return (RMR_OK);
}

//-----------------------------------------------------------------------------
/** Brief of MW_VDP_SetConnect.
 */
//-----------------------------------------------------------------------------
void MW_VDP_SetConnect(UCHAR ucVdpId, UCHAR ucConnect)
{
    VERIFY_VDP_ID(ucVdpId);

    _arMwVdp[ucVdpId].ucConnect = ucConnect;

    VERIFY(_VdpSetEnable(ucVdpId) == VDP_SET_OK);
}

//-----------------------------------------------------------------------------
/** Brief of MW_VDP_Comp2EsInd.
 */
//-----------------------------------------------------------------------------
void MW_VDP_Comp2EsInd(UCHAR ucCompId, UCHAR ucEsId)
{
    if (ucCompId >= (UCHAR)MPV_COMP_NS)
    {
        ASSERT(ucCompId >= (UCHAR)MPV_COMP_NS);
        return;
    }

    //if (_arMwComp2EsId[ucCompId] != ucEsId)
    {
        UINT32 u4VdpId;

        //_arMwComp2EsId[ucCompId] = ucEsId;

        for (u4VdpId = 0; u4VdpId < VDP_COMP_NS; u4VdpId++)
        {
            if (_arMwVdp[u4VdpId].u2SrcCompId == (UINT16)ucCompId)
            {
                if(_arMwVdp[u4VdpId].ucEsId != ucEsId)
                {
                _arMwVdp[u4VdpId].ucEsId = ucEsId;
                VERIFY(_SetInput((UCHAR)u4VdpId) == VDP_SET_OK);
	            }
	        }
	    }
	}
}

#ifdef CC_3D_MM_DS_SUPPORT
void MW_VDP_Comp3DInfo(UCHAR ucCompId, BOOL fgFrameSeq)
{
    if (ucCompId >= (UCHAR)MPV_COMP_NS)
    {
        ASSERT(ucCompId >= (UCHAR)MPV_COMP_NS);
        return;
    }

    if (_arMwComp3dInfo[ucCompId] != fgFrameSeq)
    {
        _arMwComp3dInfo[ucCompId] = fgFrameSeq;
    }

    if(fgFrameSeq)
    {
        MTB2R_Set3DInfo(0,MTB2R_3D_FRAME_SEQUENTIAL,TRUE);
        MTB2R_Set3DInfo(1,MTB2R_3D_FRAME_SEQUENTIAL,TRUE);
        MTB2R_SetInput(0,0,0);
    }
}
#endif

//-----------------------------------------------------------------------------
/** Brief of MW_VDP_Comp2Vdp.
 */
//-----------------------------------------------------------------------------
UINT32 MW_VDP_Comp2Vdp(UCHAR ucCompId)
{
    UINT32 u4VdpId = VDP_NS;

    if (ucCompId >= (UCHAR)MPV_COMP_NS)
    {
        ASSERT(ucCompId >= (UCHAR)MPV_COMP_NS);
        return VDP_NS;
    }

    for (u4VdpId = 0; u4VdpId < VDP_COMP_NS; u4VdpId++)
    {
        if (_arMwVdp[u4VdpId].u2SrcCompId == (UINT16)ucCompId)
        {
            return u4VdpId;
        }
    }

    return VDP_NS;

}

UINT32 MW_VDP_Es2Vdp(UCHAR ucEsId)
{
    UINT32 u4VdpId = VDP_NS;

    if (ucEsId >= (UCHAR)ES_ID_MAX)
    {
        ASSERT(ucEsId >= (UCHAR)ES_ID_MAX);
        return VDP_NS;
    }

    for (u4VdpId = 0; u4VdpId < VDP_COMP_NS; u4VdpId++)
    {
		
        if (_arMwVdp[u4VdpId].ucEsId == ucEsId || _arMwVdp[u4VdpId].ucEs2Id == ucEsId)
        {
            return u4VdpId;
        }
    }

    return VDP_NS;

}


UCHAR MW_VDP_MEMO_MODE_GET(UCHAR ucVdpId)
{
    return (_arMwVdp[ucVdpId].ucMemoMode);
}

void MW_VDP_ENABLE_CB(BOOL fEnable)
{ 
    fgEnableMwCb = fEnable;
}

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
VOID MM_AUTOTEST_SET_VDP_CALLBACK(UCHAR ucVdpId,UINT32 pfAtEosNfy,VOID *pv_Tag)
{
    _arMwVdp[ucVdpId].pfAtEosNfy=(x_vid_pla_nfy_fct)pfAtEosNfy;
    _arMwVdp[ucVdpId].pvATEosTag=pv_Tag;
}
#endif
