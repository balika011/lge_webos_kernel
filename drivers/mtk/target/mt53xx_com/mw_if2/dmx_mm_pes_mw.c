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
 * $RCSfile: dmx_mm_pes_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_pes_mw.c
 *  Demux driver - middleware interface of PES
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mtdmx_mwif.h"

#include "dmx_mw.h"
#include "dmx_mm_if.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "drv_comp_id.h"
#include "drv_name.h"

#include "x_common.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_demux.h"
#include "x_assert.h"

#include "x_debug.h"

LINT_EXT_HEADER_END

#undef LOG
#define LOG(a, ...)

EXTERN BOOL _MwDmxMM_SetSubtitleStrmID(UINT32 u4StrmID);
EXTERN BOOL _MwDmxMM_EnableSubtitle(void);
EXTERN BOOL _MwDmxMM_DisableSubtitle(void);
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define DMX_MM_PES_INDEX            25

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
/// Index that middleware pass to driver
//static UINT8 _u1MMPESIdx = 0;

static DEMUX_MM_PES_FMT_T eFmt;

/// Notification callback functions
static DEMUX_NFY_INFO_T _arMMNotifyInfo[DMX_MM_STRUCT_NUM];

/// State word for locking interrupt
static CRIT_STATE_T _rDmxMMLock;

/// Is interrupt locking? For making sure Lock()/Unlock() are not nested
static BOOL _fgDmxMMLocking = FALSE;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** _DMXMMLock
 *  Enter demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _DMXMMLock(void)
{
    _rDmxMMLock = x_crit_start();

    ASSERT(!_fgDmxMMLocking);
    _fgDmxMMLocking = TRUE;

    UNUSED(_fgDmxMMLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** _DMXMMUnlock
 *  Leave demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _DMXMMUnlock(void)
{
    ASSERT(_fgDmxMMLocking);
    _fgDmxMMLocking = FALSE;

    x_crit_end(_rDmxMMLock);

    UNUSED(_fgDmxMMLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** _MwDmxMMOnPesNotify
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMOnPesNotify(UINT8 u1Pidx, UINT32 u4Data, const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;
    MTDMX_NOTIFY_INFO_PES_T* prNotifyInfo;
    UINT32 u4DataAddr, u4Data1;
    UINT8 u1SerialNumber;
    BOOL fgKeepData;

    if(u1Pidx >= DMX_MM_STRUCT_NUM)
    {
        return FALSE;
    }

    _DMXMMLock();
    rInfo = _arMMNotifyInfo[u1Pidx];
    _DMXMMUnlock();
/*
    if(DMX_MM_PES_INDEX != u1Pidx)
    {
        LOG(3, "Pidx not correct, %d, %d\n", DMX_MM_PES_INDEX, u1Pidx);
        return FALSE;
    }
*/
    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No PES handler registered\n", u1Pidx);
        return FALSE;
    }

    prNotifyInfo = (MTDMX_NOTIFY_INFO_PES_T*)u4Data;
    ASSERT(prNotifyInfo != NULL);

    u4DataAddr = prNotifyInfo->u4DataAddr;
    u1SerialNumber = prNotifyInfo->u1SerialNumber;
    ASSERT(u1SerialNumber <= DMX_MAX_SERIAL_NUM);
    u4Data1 = u4DataAddr;
    LOG(6,"%s ,Send(addr=0x%x, size=0x%x) to mw.\r\n",__FUNCTION__,u4DataAddr,prNotifyInfo->u4DataSize);
    fgKeepData = rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_DATA_REC,
        u4Data1, prNotifyInfo->u4DataSize, (UINT32)u1SerialNumber
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        );

    UNUSED(pvTag);

    return fgKeepData;
}


//-----------------------------------------------------------------------------
/** _MwDmxMMOnPesExtraTimeNotify
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMOnPesExtraTimeNotify(UINT8 u1Pidx, UINT32 u4Data, const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;
    MTDMX_NOTIFY_INFO_PES_TIME_T* prNotifyInfo;
    UINT32 u4DataAddr, u4Data1;
    UINT8 u1SerialNumber;
    BOOL fgKeepData;
    DEMUX_MKV_PACK_T rMkvPack;

    if(u1Pidx >= DMX_MM_STRUCT_NUM)
    {
        return FALSE;
    }

    _DMXMMLock();
    rInfo = _arMMNotifyInfo[u1Pidx];
    _DMXMMUnlock();
/*
    if(DMX_MM_PES_INDEX != u1Pidx)
    {
        LOG(3, "Pidx not correct, %d, %d\n", DMX_MM_PES_INDEX, u1Pidx);
        return FALSE;
    }
*/
    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No PES handler registered\n", u1Pidx);
        return FALSE;
    }

    prNotifyInfo = (MTDMX_NOTIFY_INFO_PES_TIME_T*)u4Data;
    ASSERT(prNotifyInfo != NULL);

    u4DataAddr = prNotifyInfo->u4DataAddr;
    u1SerialNumber = prNotifyInfo->u1SerialNumber;
    ASSERT(u1SerialNumber <= DMX_MAX_SERIAL_NUM);
    u4Data1 = u4DataAddr;

    rMkvPack.ui4_data_1 = u4Data1;
    rMkvPack.ui4_data_2 = prNotifyInfo->u4DataSize;
    rMkvPack.ui4_data_3 = (UINT32)u1SerialNumber;
    rMkvPack.ui4_pts = prNotifyInfo->u4Pts;
    rMkvPack.ui4_duration = prNotifyInfo->u4Dts;
	LOG(6,"%s ,Send(addr=0x%x, size=0x%x,pts=0x%x,dtx=0x%x) to mw.\r\n",__FUNCTION__,u4Data1,prNotifyInfo->u4DataSize,
	 	                                                                 prNotifyInfo->u4Pts,prNotifyInfo->u4Dts);
    fgKeepData = rInfo.pf_demux_nfy(rInfo.pv_tag, DEMUX_COND_DATA_REC, (UINT32)&rMkvPack, 0, 0
                            #ifndef MW_CLASSIC_DMX_NFY
                                    , 0, 0, 0
                            #endif
                                    );

    UNUSED(pvTag);

    return fgKeepData;
}


//-----------------------------------------------------------------------------
/** _MwDmxMMOnOverflowNotify
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMOnOverflowNotify(UINT8 u1Pidx, UINT32 u4Data,
    const void* pvTag)
{
    DEMUX_NFY_INFO_T rInfo;

    if(u1Pidx >= DMX_MM_STRUCT_NUM)
    {
        return FALSE;
    }
/*
    if(DMX_MM_PES_INDEX != u1Pidx)
    {
        LOG(3, "Pidx not correct, %d, %d\n", DMX_MM_PES_INDEX, u1Pidx);
        return FALSE;
    }
*/
    _DMXMMLock();
    rInfo = _arMMNotifyInfo[u1Pidx];
    _DMXMMUnlock();

    if (rInfo.pf_demux_nfy == NULL)
    {
        LOG(3, "Pidx %u: No handler registered\n", u1Pidx);
        return FALSE;
    }
    LOG(6,"%s ,OverFlow to mw.\r\n",__FUNCTION__);
    UNUSED(rInfo.pf_demux_nfy(
        rInfo.pv_tag, DEMUX_COND_OVERFLOW, 0, 0, 0
    #ifndef MW_CLASSIC_DMX_NFY
        , 0, 0, 0
    #endif
        ));

    UNUSED(u4Data);
    UNUSED(pvTag);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxMMHandler
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MwDmxMMHandler(UINT8 u1Pidx, MTDMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvTag)
{
    MT_RESULT_T rRet = MTR_NOT_OK;

    switch (eCode)
    {
    case MTDMX_NOTIFY_CODE_PES:
        rRet = (_MwDmxMMOnPesNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    case MTDMX_NOTIFY_CODE_PES_TIME:
        rRet = (_MwDmxMMOnPesExtraTimeNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    case MTDMX_NOTIFY_CODE_OVERFLOW:
        rRet = (_MwDmxMMOnOverflowNotify(u1Pidx, u4Data, pvTag))?MTR_OK:MTR_NOT_OK;
        break;

    default:
        // Unknown notification code
        LOG(1, "Pidx %u: Unknown notification code %u\n", u1Pidx,
            (UINT32)eCode);
        ASSERT(0);
        break;
    }

    return rRet;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_SetMMCallback
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_SetMMCallback(UINT8 u1Pidx, const DEMUX_NFY_INFO_T* prNfyInfo)
{
    MTDMX_MM_T rMM;

    if ((u1Pidx >= DMX_MM_STRUCT_NUM) || (prNfyInfo == NULL))
    {
        return FALSE;
    }

    
    #ifdef HYBRID_DRIVER_BASE
	if(MTDMX_RegCbFunc()!= MTR_OK)
    {
        printf("RegCbFunc fail\n");
        return FALSE;
    }
    #endif

    // Set PSI callback function to our handler
    rMM.pfnCallback = _MwDmxMMHandler;

    if (MTDMX_MM_Set(u1Pidx, MTDMX_MM_FLAG_NOTIFY, &rMM) != MTR_OK)
    {
        return FALSE;
    }

    _DMXMMLock();
    _arMMNotifyInfo[u1Pidx].pf_demux_nfy = prNfyInfo->pf_demux_nfy;
    _arMMNotifyInfo[u1Pidx].pv_tag = prNfyInfo->pv_tag;
    _DMXMMUnlock();

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_MMResetCallback
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_MMResetCallback(UINT8 u1Pidx)
{
    MTDMX_MM_T rMM;

    if (u1Pidx >= DMX_MM_STRUCT_NUM)
    {
        return FALSE;
    }

    rMM.pfnCallback = NULL;
    if (MTDMX_MM_Set(u1Pidx, MTDMX_MM_FLAG_NOTIFY, &rMM) != MTR_OK)
    {
        return FALSE;
    }

    _DMXMMLock();
    _arMMNotifyInfo[u1Pidx].pf_demux_nfy = NULL;
    _arMMNotifyInfo[u1Pidx].pv_tag = NULL;
    _DMXMMUnlock();

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwDmxMMPesUnlockBuffer
 *  Unlock a PES buffer
 *
 *  @param  u1Pidx          PID index
 *  @param  prUnlockInfo    The unlock info
 *
 *  @retval TRUE	        Succeed
 *  @retval FALSE	        Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMPesUnlockBuffer(UINT8 u1Pidx,
    const DEMUX_UNLOCK_BUFFER_INFO_T* prUnlockInfo)
{
    UINT32 u4Size, u4SrcTag;
    UINT8 u1SerialNumber;

    ASSERT(prUnlockInfo != NULL);

    u4Size = (UINT32)prUnlockInfo->z_src_len;
    u4SrcTag = (UINT32)prUnlockInfo->pv_src_tag;
    u1SerialNumber = (UINT8)prUnlockInfo->ui1_serial_num;

    return (MTDMX_MM_UnlockBuffer(u1Pidx, u1SerialNumber, u4Size, u4SrcTag) == MTR_OK)?TRUE:FALSE;
}


//-----------------------------------------------------------------------------
/** _MwDmxMMPesReadBuffer
 *  Read a PES buffer
 *
 *  @param  u1Pidx          PID index
 *  @param  prReadInfo      The read info
 *  @param  pu4Size[output] The size actually read
 *
 *  @retval TRUE	        Succeed
 *  @retval FALSE	        Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxMMPesReadBuffer(UINT8 u1Pidx,
    const DEMUX_READ_BUFFER_INFO_T* prReadInfo, UINT32* pu4Size)
{
    UINT32 u4SrcTag;
    UINT8 u1SerialNumber;
    BOOL fgRet;

    ASSERT(prReadInfo != NULL);
    ASSERT(pu4Size != NULL);

    u4SrcTag = (UINT32)prReadInfo->pv_src_tag;
    u1SerialNumber = (UINT8)prReadInfo->ui1_serial_num;

    fgRet = (MTDMX_MM_GetBuffer(u1Pidx, u1SerialNumber, u4SrcTag,
        (UINT32)prReadInfo->z_skip_len, (UINT32)prReadInfo->z_copy_len,
        prReadInfo->pui1_dest) == MTR_OK)?TRUE:FALSE;

    if (fgRet)
    {
        *pu4Size = (UINT32)prReadInfo->z_copy_len;
    }

    return fgRet;
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Notes: record and set something on hardware(src, dest, type...)
 ----------------------------------------------------------------------------*/
static INT32 _MwDmx_MMPesConnect(
							const DRV_COMP_ID_T*    pt_comp_id,
							DRV_CONN_TYPE_T         e_conn_type,
							const VOID*             pv_conn_info,
							SIZE_T                  z_conn_info_len,
							VOID*                   pv_tag,
							x_rm_nfy_fct            pf_nfy)
{
    DRV_COMP_ID_T* prConnectComp = 0;
    MTDMX_MM_T rMM;
    UINT32 u4Flags;
    UINT8 u1Pidx, u1Inst;
    DMX_PID_TYPE_T eType;

	// 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
       	return (RMR_DRV_INV_CONN_INFO);
    }

    // 1.2 check id range
  	// u1Pidx = (UINT8)(pt_comp_id->ui2_id - DMX_MM_CONTAINER_ES_MEM_COMP_ID_START);
   	u1Pidx = (UINT8)(pt_comp_id->ui2_id);
  	if (u1Pidx >= DMX_NUM_PID_INDEX)
  	{
  	    return RMR_DRV_INV_CONN_INFO;
  	}

	if (e_conn_type != RM_CONN_TYPE_COMP_ID)
	{
       return RMR_DRV_INV_CONN_INFO;
	}

    // Check component type
	prConnectComp = (DRV_COMP_ID_T*)pv_conn_info;
    //_MW_DMX_ConnectInfo(u1Pidx, (UINT32)pt_comp_id->e_type, prConnectComp->e_type);

    if (!_MW_DMX_ConnectInfo(u1Pidx, (UINT32)pt_comp_id->e_type, prConnectComp->e_type, prConnectComp->ui2_id))
    {
		return RMR_DRV_CONN_FAILED;
	}

    if(u1Pidx == 0xff)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if(MTDMX_MUL_GetPidxInstId(u1Pidx, &u1Inst))
    {
        return RMR_DRV_CONN_FAILED;
    }

    if(MTDMX_MUL_SetSubtitlePidx(u1Inst, u1Pidx))
    {
        return RMR_DRV_CONN_FAILED;
    }

    x_memset(&rMM, 0, sizeof(rMM));

    if(MTDMX_MM_Get(u1Pidx, MTDMX_MM_FLAG_VALID, &rMM) != MTR_OK)
    {
        return RMR_DRV_CONN_FAILED;
    }

    if(rMM.fgEnable)
    {
        LOG(1, "MM PES PID already enable\n");
        return RMR_DRV_CONN_FAILED;
    }

    eFmt = (DEMUX_MM_PES_FMT_T)(UINT32)(((DRV_COMP_ID_T*)pv_conn_info)->pv_tag);

    if(eFmt == DMX_MM_PES_MKV)
    {
        eType = MTDMX_PID_TYPE_PES_TIME;
    }
    else if(eFmt == DMX_MM_PES_PS)
    {
        eType = MTDMX_PID_TYPE_PES_TIME;
        /*
        if(!DMX_MUL_SetPidxInstance(0, DMX_MM_PES_INDEX))
        {
            LOG(1, "PES PID already enable\n");
        }
        */
    }
    else
    {
        eType = MTDMX_PID_TYPE_PES;
    }

	// Set PES PID
    rMM.ePidType = eType;
    rMM.fgSearchStartCode = FALSE;

    u4Flags = (UINT32)(MTDMX_MM_FLAG_TYPE | MTDMX_MM_FLAG_SEARCH_START_CODE);

    if (MTDMX_MM_Set(u1Pidx, u4Flags, &rMM) != MTR_OK)
    {
        return RMR_DRV_CONN_FAILED;
    }

	//_u1MMPESIdx = u1Pidx;

    UNUSED(z_conn_info_len);
    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    return RMR_OK;
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:  just record, since it's hardwire
 ----------------------------------------------------------------------------*/
static INT32 _MwDmx_MMPesDisconnect(
							const DRV_COMP_ID_T*    pt_comp_id,
							DRV_DISC_TYPE_T         e_disc_type,
							const VOID*             pv_disc_info,
							SIZE_T                  z_disc_info_len)
{
    UINT8 u1Pidx;

	// 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
       	return (RMR_DRV_INV_DISC_INFO);
    }

    // 1.2 check id range
  	// u1Pidx = (UINT8)(pt_comp_id->ui2_id - DMX_MM_CONTAINER_ES_MEM_COMP_ID_START);
  	u1Pidx = (UINT8)(pt_comp_id->ui2_id);
  	if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
       return (RMR_DRV_INV_DISC_INFO);
    }
/*
  	if(u1Pidx != _u1MMPESIdx)
  	{
  	    return (RMR_DRV_INV_DISC_INFO);
  	}
*/

  	if(MTDMX_MM_Free(u1Pidx) != MTR_OK)
  	{
  	    return RMR_DRV_INV_DISC_INFO;
  	}

  	if(!_MW_DMX_MMResetCallback(u1Pidx))
  	{
  	    return RMR_DRV_INV_DISC_INFO;
  	}
/*
    if(eFmt == DMX_MM_PES_PS)
    {
        if(!DMX_MUL_FreePidx(DMX_MM_PES_INDEX))
        {
            LOG(1, "Failed to free PES PID\n");
        }
    }

    _u1MMPESIdx = 0;
*/
    //_MW_DMX_DisConnectInfo((UINT8)pt_comp_id->ui2_id);
    if (!_MW_DMX_DisConnectInfo(u1Pidx))
    {
		return RMR_DRV_DISC_FAILED;
	}

    UNUSED(e_disc_type);
    UNUSED(z_disc_info_len);

    return RMR_OK;
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:
 ----------------------------------------------------------------------------*/
static INT32 _MwDmx_MMPesSet(
					const DRV_COMP_ID_T*    pt_comp_id,
					DRV_SET_TYPE_T		    e_set_type,
					const VOID*			    pv_set_info,
					SIZE_T				    z_set_info_len)
{
    MTDMX_MM_T rMM;
    UINT8 u1Pidx;
    BOOL fgRet = FALSE;

	//1. Check para valid
    if ((NULL == pt_comp_id))
    {
       	return RMR_DRV_INV_SET_INFO;
    }

    //1.2 check id range
  	// u1Pidx = (UINT8)(pt_comp_id->ui2_id - DMX_MM_CONTAINER_ES_MEM_COMP_ID_START);
 	u1Pidx = (UINT8)(pt_comp_id->ui2_id);
  	if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
       return RMR_DRV_INV_SET_INFO;
    }
/*
  	if(u1Pidx != _u1MMPESIdx)
  	{
  	    LOG(1, "Pidx error, %d\n", u1Pidx);
  	    return RMR_DRV_INV_SET_INFO;
  	}
*/
    x_memset(&rMM, 0, sizeof(rMM));

    //set,
    switch (e_set_type)
    {
    case DEMUX_SET_TYPE_PID:
        // Not necessary
        break;

    case  DEMUX_SET_TYPE_STREAM_ID:
        fgRet = _MwDmxMM_SetSubtitleStrmID((UINT32)pv_set_info);
        if(eFmt == DMX_MM_PES_PS)
        {
            MTDMX_PS_SetStreamId(u1Pidx, (UINT8)(((UINT32)pv_set_info) >> 8),
                (UINT8)(UINT32)pv_set_info);
        }
        break;

    case DEMUX_SET_TYPE_CTRL:
        rMM.fgEnable = (BOOL)(UINT32)pv_set_info;
        fgRet = (MTDMX_MM_Set(u1Pidx, MTDMX_MM_FLAG_VALID, &rMM) == MTR_OK)?TRUE:FALSE;
        if(fgRet)
        {
            if(rMM.fgEnable)
            {
                fgRet = _MwDmxMM_EnableSubtitle();
            }
            else
            {
                fgRet = _MwDmxMM_DisableSubtitle();
            }
        }
        break;

    case DEMUX_SET_TYPE_NFY_FCT:
        fgRet = _MW_DMX_SetMMCallback(u1Pidx,
                                     (DEMUX_NFY_INFO_T*)pv_set_info);
        break;

    case DEMUX_SET_TYPE_ALLOC_BUFFER:
        fgRet = (MTDMX_MM_AllocateBuffer(u1Pidx,
                                     (UINT32)(SIZE_T)pv_set_info) == MTR_OK)?TRUE:FALSE;
        break;

    case DEMUX_SET_TYPE_FREE_BUFFER:
        fgRet = (MTDMX_MM_FreeBuffer(u1Pidx) == MTR_OK)?TRUE:FALSE;
        break;

    case DEMUX_SET_TYPE_UNLOCK_BUFFER:
        fgRet = _MwDmxMMPesUnlockBuffer(u1Pidx,
                        (DEMUX_UNLOCK_BUFFER_INFO_T*)pv_set_info);
        break;

    case DEMUX_SET_TYPE_FLUSH_BUFFER:
        fgRet = (MTDMX_MM_FlushBuffer(u1Pidx) == MTR_OK)?TRUE:FALSE;
        break;

    default:
        break;
    }

    UNUSED(z_set_info_len);

    return (fgRet ? RMR_OK : RMR_DRV_INV_SET_INFO);
}


/*-----------------------------------------------------------------------------
 * Name:
 *
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 * Note:
 ----------------------------------------------------------------------------*/
static INT32 _MwDmx_MMPesGet(
						const DRV_COMP_ID_T*    pt_comp_id,
						DRV_GET_TYPE_T		    e_get_type,
						VOID*				    pv_get_info,
						SIZE_T*				    pz_get_info_len)
{
    MTDMX_MM_T rMM;
    UINT32 u4Size = 0;
    UINT8 u1Pidx;
    BOOL fgRet = FALSE;

    x_memset(&rMM, 0, sizeof(rMM));

	// 1. Check para valid
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) ||
        (NULL == pz_get_info_len))
    {
       	return RMR_DRV_INV_GET_INFO;
    }

    // 1.2 check id range
  	// u1Pidx = (UINT8)(pt_comp_id->ui2_id - DMX_MM_CONTAINER_ES_MEM_COMP_ID_START);
  	u1Pidx = (UINT8)(pt_comp_id->ui2_id);
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
       return RMR_DRV_INV_GET_INFO;
    }
/*
  	if(u1Pidx != _u1MMPESIdx)
  	{
  	    LOG(1, "Pidx error, %d\n", u1Pidx);
  	    return RMR_DRV_INV_GET_INFO;
  	}
*/

    //get
    switch (e_get_type)
    {
    case DEMUX_GET_TYPE_PID:
        *(MPEG_2_PID_T*)pv_get_info = 0;            // Not necessary
        *pz_get_info_len = sizeof(MPEG_2_PID_T);
        fgRet = TRUE;
        break;

    case DEMUX_GET_TYPE_CTRL:
        // Query if a PID is enabled or not
        if (MTDMX_MM_Get(u1Pidx, MTDMX_MM_FLAG_VALID, &rMM) == MTR_OK)
        {
            *(BOOL*)pv_get_info = rMM.fgEnable;
            *pz_get_info_len = sizeof(BOOL);
            fgRet = TRUE;
        }
        break;

    case DEMUX_GET_TYPE_READ_BUFFER_NFY:
    case DEMUX_GET_TYPE_READ_BUFFER:
        if (_MwDmxMMPesReadBuffer(u1Pidx,
                    (DEMUX_READ_BUFFER_INFO_T*)pv_get_info, &u4Size))
        {
            *pz_get_info_len = (SIZE_T)u4Size;
            fgRet = TRUE;
        }
        break;

    default:
        break;
    }

    return (fgRet ? RMR_OK : RMR_DRV_INV_GET_INFO);
}


 /*-----------------------------------------------------------------------------
 * Name:
 *
 * Description: Registration
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 ----------------------------------------------------------------------------*/
BOOL _MW_DMX_MM_PES_Init(void)
{
    UINT32 u4i = 0, u4Flags;
	INT32                ret;
	DRV_COMP_REG_T       t_comp_id;
	DRV_COMP_FCT_TBL_T   t_comp_fct_tbl;
	ID_IND_T             at_id_ind[DMX_MM_CONTAINER_ES_MEM_COMP_NUM];

	/* register Section Memory component */
	t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwDmx_MMPesConnect;
	t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwDmx_MMPesDisconnect;
	t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwDmx_MMPesGet;
	t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwDmx_MMPesSet;

	t_comp_id.e_type = DRVT_DEMUX_CONTAINER_ES_MEMORY;
	t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = DMX_MM_CONTAINER_ES_MEM_COMP_NUM;
	t_comp_id.u.t_list.pt_list = at_id_ind;

	for ( u4i = 0; u4i < DMX_MM_CONTAINER_ES_MEM_COMP_NUM; u4i++ )
	{
		at_id_ind[u4i].ui2_id = (UINT16)(DMX_MM_CONTAINER_ES_MEM_COMP_ID_START + u4i);
		at_id_ind[u4i].pv_tag = NULL;
		at_id_ind[u4i].ui1_port = ALL_PORTS;
	}

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

	ret = x_rm_reg_comp(&t_comp_id,
	                    1,                                  /* num in port */
	                    1,                                  /* num out port */
	                    NULL,                               /* name */
	                    u4Flags,                            /* flag */
	                    &t_comp_fct_tbl,
	                    NULL,
	                    0);
	if ( ret != RMR_OK )
	{
		return FALSE;
	}

    return TRUE;
}

