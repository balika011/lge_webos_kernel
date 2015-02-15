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
 * $RCSfile: srm_dram.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file srm_dram.c
 *  Brief of file srm_dram.c.
 *  Details of file srm_dram.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "srm_drvif.h"
#include "srm_debug.h"
#include "drv_common.h"
#include "x_hal_5381.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_assert.h"
#include "drvcust_if.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define SRM_DRAM_MONITOR_IDLE 500 // Take a rest between each monitor


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/**
 ** Brief of HAL_DRAMC_T
 */
typedef struct
{
    UINT32 u4Reserved                       : 8;
    UINT32 u4Bmgp1ag                        : 5;
    UINT32                                  : 2;
    UINT32 fgBmbp1                          : 1;
    UINT32 u4Bmgp2ag                        : 3;
    UINT32 fgBmbp2                          : 1;
    UINT32 u4Bmgp3ag                        : 3;
    UINT32 fgBmbp3                          : 1;
    UINT32 fgReadOnly                       : 1;
    UINT32 fgWriteOnly                      : 1;
    UINT32                                  : 2;
    UINT32 fgFreeze                         : 1;
    UINT32 fgDynPrioEn                      : 1;
    UINT32 fgReqAle                         : 1;
    UINT32 fgPreAct                         : 1;
LINT_SUPPRESS_NEXT_EXPRESSION(950)
} __attribute__ ((packed))  HAL_DRAMC_T;


//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

UINT32 u4Agendid=0;
UINT32 u4Groupid=0;
static UINT32 _u4DramMeasureModule = SRM_DRV_TOTAL;
static HANDLE_T _hMonitor = NULL_HANDLE;
static BOOL _fgSrmDramInit = FALSE;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of _SrmDramMonitor.
 */
//-----------------------------------------------------------------------------

static VOID _SrmDramMonitor(VOID* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
#if 0
        UINT32 u4Result;
        UINT32 u4DelayClock;
        UINT64 u8Result;

        u4DelayClock = (BSP_GetDomainClock(CAL_SRC_DMPLL) >> 1) * 2;

        u4Result = SRM_DramMonitor(_u4DramMeasureModule, u4DelayClock, 2000);

        u8Result = (UINT64)u4Result;
        u8Result *= 1000;
        u8Result /= u4DelayClock;

        u4Result = (UINT32) u8Result;

        SRM_SendEvent(SRM_DRV_DRAM, SRM_DRV_TOTAL, u4Result, 0);
#else
        UNUSED(_u4DramMeasureModule);
#endif

        // Take a rest between each monitor
        x_thread_delay(SRM_DRAM_MONITOR_IDLE);
    }
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of SRM_Init.
 */
//-----------------------------------------------------------------------------

VOID SRM_DramInit(VOID)
{
    if (_fgSrmDramInit == FALSE)
    {
        HANDLE_T hThread;
        // create semaphore
        VERIFY(x_sema_create(&_hMonitor, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

        // create thread
        if (x_thread_create(&hThread, "DRAM_M", 1024, (LOG_PRINT_PRIORITY - 1),
            _SrmDramMonitor, 0, NULL) != OSR_OK)
        {
            ASSERT(0);
        }

        _fgSrmDramInit = TRUE;
    }
}


//-----------------------------------------------------------------------------
/** Brief of SRM_DramGetID.
 */
//-----------------------------------------------------------------------------

VOID SRM_DramGetID(UINT32 *u4AgendId,UINT32 *u4GroupId)
{
	//Printf("u4Agendid=%d,  u4Groupid=%d\n",u4Agendid,u4Groupid);
	*u4AgendId=u4Agendid;
	*u4GroupId=u4Groupid;
}
/** Brief of SRM_DramMonitor.
 */
//-----------------------------------------------------------------------------
#ifdef CC_MT5398
UINT32 SRM_DramMonitor(UINT32 u4Channel, UINT32 u4Mod, UINT32 u4DelayClock, UINT32 u4MonitorDelay)
{
    UINT32 u4AgendId;
    UINT32 u4GroupId;
    UINT32 u4Result;
    UINT32 u4Total;
    UINT32 u4Base;
    volatile HAL_DRAMC_T* _prDramcReg; 
    HAL_DRAMC_T rDramcReg;

    if (u4Channel == 0)
    {
        u4Base =  DRAM_BASE;   
    }
    else
    {
        u4Base = DRAM_CHB_BASE;
    }
    _prDramcReg = (HAL_DRAMC_T*) (u4Base + 0x180);

    if (_fgSrmDramInit == FALSE)
    {
        LOG(1, "SRM not init\n");
        return 0;
    }

    VERIFY(x_sema_lock(_hMonitor, X_SEMA_OPTION_WAIT) == OSR_OK);

    u4Total = 0;

    switch (u4Mod)
    {
        case SRM_DRV_TOTAL:
        u4Total = 1;
        u4GroupId = 1;
        u4AgendId = 1;
        break;

        // group1 --------------------------------------------------------------

        case SRM_DRV_AUD:
        u4AgendId = 0;
        u4GroupId = 1;
        break;

        case SRM_DRV_DMX:   // demux, pvr
        u4AgendId = 1;
        u4GroupId = 1;
        break;

        case SRM_DRV_VDOIN:    // 3d-comb, vbi, teltext, TVE
        u4AgendId = 2;
        u4GroupId = 1;
        break;

        case SRM_DRV_OSD:
        u4AgendId = 3;
        u4GroupId = 1;
        break;

        case SRM_DRV_MDDI:  // nr, pscan, pscan_pip
        u4AgendId = 4;
        u4GroupId = 1;
        break;

        case SRM_DRV_B2R:
        u4AgendId = 5;
        u4GroupId = 1;
        break;

        case SRM_DRV_CPU:
        u4AgendId = 6;
        u4GroupId = 1;
        break;

        case SRM_DRV_SCPOS:
        u4AgendId = 7;
        u4GroupId = 1;
        break;

        case SRM_DRV_VDEC_MC:  // vdec_mc
        u4AgendId = 8;
        u4GroupId = 1;
        break;

        case SRM_DRV_VLD1_JPEG:  // cabac,avc_mv,vld1,vld2,mpv_pred,jpeg
        u4AgendId = 9;
        u4GroupId = 1;
        break;
        
        case SRM_DRV_3D_GFX:  // 3D graphic
        #ifdef CC_MT5398
		u4AgendId = 10;
		#else
        u4AgendId = 13;//MID_AGENT_G3D // python
        #endif
        u4GroupId = 1;
        break;

        case SRM_DRV_GFX_HI:
        case SRM_DRV_IMGRZ:
        case SRM_DRV_JPEGDEC:   // JPEG
        u4AgendId = 11;
        u4GroupId = 1;
        break;

        case SRM_DRV_VENC:
		#ifdef CC_MT5398
		u4AgendId = 12;
		#else
        u4AgendId = 14;//MID_AGENT_VENC
        #endif
        u4GroupId = 1;
        break;

        // MJC
        case SRM_DRV_MJC:    
        u4AgendId = 13;
        u4GroupId = 1;
        break;
        case SRM_DRV_MJC_OUT:    
        u4AgendId = 14;
        u4GroupId = 1;
        break;
        
        case SRM_DRV_PR_GEN:
        u4AgendId = 5;
        u4GroupId = 1;
        break;

        case SRM_DRV_DSP_1:
        u4AgendId = 15;
        u4GroupId = 1;
        break;
        
        // group2 --------------------------------------------------------------

        case SRM_DRV_USB:
        u4AgendId = 0;
        u4GroupId = 2;
        break;

        case SRM_DRV_ETH:   // Ethernet
        u4AgendId = 1;
        u4GroupId = 2;
        break;

        case SRM_DRV_DDI:   // Demux_pull, ddi
        u4AgendId = 2;
        u4GroupId = 2;
        break;
        
        case SRM_DRV_DSP_2:
        u4AgendId = 4;
        u4GroupId = 2;
        break;
        
        case SRM_DRV_FCI:
        u4AgendId = 5;
        u4GroupId = 2;
        break;
        
        // group3 --------------------------------------------------------------

        case SRM_DRV_GFX_LO:
        u4AgendId = 0;
        u4GroupId = 3;
        break;
        
        case SRM_DRV_TCM:
        u4AgendId = 1;
        u4GroupId = 3;
        break;
        
        case SRM_DRV_UART:
        u4AgendId = 3;
        u4GroupId = 3;
        break;

        default:
        u4Total = 1;
        u4GroupId = 1;
        u4AgendId = 1;
        break;
    }

	u4Agendid = u4AgendId;
	u4Groupid = u4GroupId;

    IO_REG32(u4Base, 0x18C) = u4DelayClock;

    rDramcReg = *_prDramcReg;
    rDramcReg.fgBmbp1 = 0;
    rDramcReg.fgBmbp2 = 0;
    rDramcReg.fgBmbp3 = 0;
    if (u4Total != 0)
    {
        // All agent, including group 1, 2, and 3
        rDramcReg.u4Bmgp1ag = 0x1F;
    }
    else
    if (u4GroupId == 1)
    {
        rDramcReg.u4Bmgp1ag = u4AgendId;
    }
    else
    if (u4GroupId == 2)
    {
        rDramcReg.u4Bmgp2ag = u4AgendId;
    }
    else
    if (u4GroupId == 3)
    {
        rDramcReg.u4Bmgp3ag = u4AgendId;
    }
    *_prDramcReg = rDramcReg;
    if (u4GroupId == 1)
    {
        rDramcReg.fgBmbp1 = 1;
    }
    else
    if (u4GroupId == 2)
    {
        rDramcReg.fgBmbp2 = 1;
    }
    else
    if (u4GroupId == 3)
    {
        rDramcReg.fgBmbp3 = 1;
    }

    rDramcReg.fgReqAle = 0;
    *_prDramcReg = rDramcReg;
#ifndef VDEC_TIME_PROFILE
    while (IO_REG32(u4Base, 0x19C) != IO_REG32(u4Base, 0x18C))
    {
        //x_thread_delay(1000);
        x_thread_delay(u4MonitorDelay);
    }
#endif
    u4Result = 0;
    if (u4GroupId == 1)
    {
        u4Result = IO_REG32(u4Base, 0x190);
    }
    else
    if (u4GroupId == 2)
    {
        u4Result = IO_REG32(u4Base, 0x194);
    }
    else
    if (u4GroupId == 3)
    {
        u4Result = IO_REG32(u4Base, 0x198);
    }

    VERIFY(x_sema_unlock(_hMonitor) == OSR_OK);

    return u4Result;
}

#else
UINT32 SRM_DramMonitor(UINT32 u4Channel, UINT32 u4Mod, UINT32 u4DelayClock, UINT32 u4MonitorDelay)
{
    UINT32 u4AgendId;
    UINT32 u4GroupId;
    UINT32 u4Result;
    UINT32 u4Total;
    UINT32 u4Base;
    volatile HAL_DRAMC_T* _prDramcReg; 
    HAL_DRAMC_T rDramcReg;

    if (u4Channel == 0)
    {
        u4Base =  DRAM_BASE;   
    }
    else
    {
        u4Base = DRAM_CHB_BASE;
    }
    _prDramcReg = (HAL_DRAMC_T*) (u4Base + 0x180);

    if (_fgSrmDramInit == FALSE)
    {
        LOG(1, "SRM not init\n");
        return 0;
    }

    VERIFY(x_sema_lock(_hMonitor, X_SEMA_OPTION_WAIT) == OSR_OK);

    u4Total = 0;

    switch (u4Mod)
    {
        case SRM_DRV_TOTAL:
        u4Total = 1;
        u4GroupId = 1;
        u4AgendId = 1;
        break;

        // group1 --------------------------------------------------------------

        case SRM_DRV_AUD:
        u4AgendId = 0;
        u4GroupId = 1;
        break;

        case SRM_DRV_DMX:   // demux, pvr
        u4AgendId = 1;
        u4GroupId = 1;
        break;

        case SRM_DRV_VDOIN:    // 3d-comb, vbi, teltext, TVE
        u4AgendId = 2;
        u4GroupId = 1;
        break;

        case SRM_DRV_OSD:
        u4AgendId = 3;
        u4GroupId = 1;
        break;

        case SRM_DRV_MDDI:  // nr, pscan, pscan_pip
        u4AgendId = 4;
        u4GroupId = 1;
        break;

        case SRM_DRV_B2R:
        u4AgendId = 5;
        u4GroupId = 1;
        break;

        case SRM_DRV_CPU:
        u4AgendId = 6;
        u4GroupId = 1;
        break;

        case SRM_DRV_SCPOS:
        u4AgendId = 7;
        u4GroupId = 1;
        break;

        case SRM_DRV_VDEC_MC:  // vdec_mc
        u4AgendId = 8;
        u4GroupId = 1;
        break;

        case SRM_DRV_VLD1_JPEG:  // cabac,avc_mv,vld1,vld2,mpv_pred,jpeg
        u4AgendId = 9;
        u4GroupId = 1;
        break;
        
        case SRM_DRV_DSP:  // dsp
		u4AgendId = 10;
        u4GroupId = 1;
        break;

        case SRM_DRV_GFX_HI:
        case SRM_DRV_IMGRZ:
        case SRM_DRV_JPEGDEC:   // JPEG
        u4AgendId = 11;
        u4GroupId = 1;
        break;

        case SRM_DRV_MMU:
		u4AgendId = 12;
        u4GroupId = 1;
        break;

        // MJC
        case SRM_DRV_G3D:    
        u4AgendId = 13;
        u4GroupId = 1;
        break;
        case SRM_DRV_VENC:    
        u4AgendId = 14;
        u4GroupId = 1;
        break;
        
        case SRM_DRV_TEST0:
        u4AgendId = 15;
        u4GroupId = 1;
        break;

        
        // group2 --------------------------------------------------------------

        case SRM_DRV_USB:
        u4AgendId = 0;
        u4GroupId = 2;
        break;

        case SRM_DRV_ETH:   // Ethernet
        u4AgendId = 1;
        u4GroupId = 2;
        break;

        case SRM_DRV_DDI:   // Demux_pull, ddi
        u4AgendId = 2;
        u4GroupId = 2;
        break;
        
        case SRM_DRV_GDMA:
        u4AgendId = 3;
        u4GroupId = 2;
        break;
		
        case SRM_DRV_DEMOD:
        u4AgendId = 4;
        u4GroupId = 2;
        break;
        
        case SRM_DRV_FCI:
        u4AgendId = 5;
        u4GroupId = 2;
        break;
        
        case SRM_DRV_TEST1:
        u4AgendId = 6;
        u4GroupId = 2;
        break;
        // group3 --------------------------------------------------------------

        case SRM_DRV_GFX_LO:
        u4AgendId = 0;
        u4GroupId = 3;
        break;
        
        case SRM_DRV_TCM:
        u4AgendId = 1;
        u4GroupId = 3;
        break;
        
        case SRM_DRV_UART:
        u4AgendId = 3;
        u4GroupId = 3;
        break;
		
        case SRM_DRV_NFI:
        u4AgendId = 4;
        u4GroupId = 3;
        break;
		
        case SRM_DRV_TEST2:
        u4AgendId = 5;
        u4GroupId = 3;
        break;

        default:
        VERIFY(x_sema_unlock(_hMonitor) == OSR_OK);
        return 0xFFFFFFFF;
#if 0
        u4Total = 1;
        u4GroupId = 1;
        u4AgendId = 1;
        break;
#endif
    }
	u4Agendid = u4AgendId;
	u4Groupid = u4GroupId;


    IO_REG32(u4Base, 0x18C) = u4DelayClock;

    rDramcReg = *_prDramcReg;
    rDramcReg.fgBmbp1 = 0;
    rDramcReg.fgBmbp2 = 0;
    rDramcReg.fgBmbp3 = 0;
    if (u4Total != 0)
    {
        // All agent, including group 1, 2, and 3
        rDramcReg.u4Bmgp1ag = 0x1F;
    }
    else
    if (u4GroupId == 1)
    {
        rDramcReg.u4Bmgp1ag = u4AgendId;
    }
    else
    if (u4GroupId == 2)
    {
        rDramcReg.u4Bmgp2ag = u4AgendId;
    }
    else
    if (u4GroupId == 3)
    {
        rDramcReg.u4Bmgp3ag = u4AgendId;
    }
    *_prDramcReg = rDramcReg;
    if (u4GroupId == 1)
    {
        rDramcReg.fgBmbp1 = 1;
    }
    else
    if (u4GroupId == 2)
    {
        rDramcReg.fgBmbp2 = 1;
    }
    else
    if (u4GroupId == 3)
    {
        rDramcReg.fgBmbp3 = 1;
    }

    rDramcReg.fgReqAle = 0;
    *_prDramcReg = rDramcReg;
#ifndef VDEC_TIME_PROFILE
    while (IO_REG32(u4Base, 0x19C) != IO_REG32(u4Base, 0x18C))
    {
        //x_thread_delay(1000);
        x_thread_delay(u4MonitorDelay);
    }
#endif
    u4Result = 0;
    if (u4GroupId == 1)
    {
        u4Result = IO_REG32(u4Base, 0x190);
    }
    else
    if (u4GroupId == 2)
    {
        u4Result = IO_REG32(u4Base, 0x194);
    }
    else
    if (u4GroupId == 3)
    {
        u4Result = IO_REG32(u4Base, 0x198);
    }

    VERIFY(x_sema_unlock(_hMonitor) == OSR_OK);

    return u4Result;
}

#endif
