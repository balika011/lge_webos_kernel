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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_h264hw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_h264hw.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
#include "vdec_h264hw.h"
#include "vdec_h264util.h"
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"

#ifdef CC_DRIVER_DEMO
static BOOL _bVdecLogSimPatAVC = FALSE;
#endif
#ifdef AVC_MV_BUF_REDUCE
static BOOL _bIsMVReduce = FALSE;
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define AVC_WAIT_HDR_PARSE_TIME 5   //5 ms

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8 _u1BarrelShifterNum;
static UINT32 _u4VFIFOSize = 0;
#ifdef AVC_HW_TIMER
static HANDLE_T _hHWParseSema = NULL;
#endif
static UINT32 B0List[100] = {0};
static UINT32 B1List[100] = {0};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL _fgIsVLDFetchOk(UINT32 u4VDecID);
static BOOL _fgIsVLDFetchOk2(UINT32 u4VDecID);

static BOOL _fgWaitVldFetchOk(UINT32 u4VDecID);
static BOOL _fgWaitVldFetchOk2(UINT32 u4VDecID);
static UINT32 _AVCVLDGetBitS(UINT32 u4VDecID, UINT32 u4ShiftBit);//, BYTE bBarrelNum)

static UINT32 _dVLDSramRp(UINT32 u4VDecID);
static UINT32 _dVLDSramWp(UINT32 u4VDecID);
static UINT32 _dVLDDramRp(UINT32 u4VDecID);

// *********************************************************************
// Function : void vVDecWriteAVCMV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
// Description : write MV register
// Parameter : u4VDecID: vld id, u4Addr: offset, u4Val: value
// Return    : none
// *********************************************************************
void vVDecWriteAVCMV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (0 == u4VDecID)
    {
        vWriteReg(AVC_MV_BASE0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, AVC_MV_BASE0, u4Addr, u4Val);
    }
    else if (1 == u4VDecID)
    {
        vWriteReg(AVC_MV_BASE1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, AVC_MV_BASE1, u4Addr, u4Val);
    }
    else
    {
        //lae do nothing
    }
}


// *********************************************************************
// Function : void vVDecWriteAVCVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
// Description : write AVC VLD register
// Parameter : u4VDecID: vld id, u4Addr: offset, u4Val: value
// Return    : none
// *********************************************************************
void vVDecWriteAVCVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    if (u4VDecID == 0)
    {
        vWriteReg(AVC_VLD_BASE0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, AVC_VLD_BASE0, u4Addr, u4Val);
    }
    else if(u4VDecID == 1)
    {
        vWriteReg(AVC_VLD_BASE1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, AVC_VLD_BASE1, u4Addr, u4Val);
    }
    else if(u4VDecID == 2)
    {
        vWriteReg(LAE_AVCVLD_BASE0 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_AVCVLD_BASE0, u4Addr, u4Val);
    }
    else
    {
        vWriteReg(LAE_AVCVLD_BASE1 + u4Addr, u4Val);
        vVDecSimDumpW(u4VDecID, LAE_AVCVLD_BASE1, u4Addr, u4Val);
    }
}


// *********************************************************************
// Function : UINT32 u4VDecReadAVCVLD(UINT32 u4VDecID, UINT32 u4Addr)
// Description : read AVC VLD register
// Parameter : u4VDecID: vld id, u4Addr: offset
// Return    : the value of register
// *********************************************************************
UINT32 u4VDecReadAVCVLD(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    
    if (u4VDecID == 0)
    {
        u4Val = u4ReadReg(AVC_VLD_BASE0 + u4Addr);
        vVDecSimDumpR(u4VDecID, AVC_VLD_BASE0, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 1)
    {   
        u4Val = u4ReadReg(AVC_VLD_BASE1 + u4Addr);
        vVDecSimDumpR(u4VDecID, AVC_VLD_BASE1, u4Addr, u4Val);
        return u4Val;
    }
    else if(u4VDecID == 2)
    {   
        u4Val = u4ReadReg(LAE_AVCVLD_BASE0 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_AVCVLD_BASE0, u4Addr, u4Val);
        return u4Val;
    }
    else
    {   
        u4Val = u4ReadReg(LAE_AVCVLD_BASE1 + u4Addr);
        vVDecSimDumpR(u4VDecID, LAE_AVCVLD_BASE1, u4Addr, u4Val);
        return u4Val;
    }
//    return 0;
}


// *********************************************************************
// Function : UINT32 u4VDecReadAVCMV(UINT32 u4VDecID, UINT32 u4Addr)
// Description : read MV register
// Parameter : u4VDecID: vld id, u4Addr: offset
// Return    : the value of register
// *********************************************************************
UINT32 u4VDecReadAVCMV(UINT32 u4VDecID, UINT32 u4Addr)
{
    UINT32 u4Val = 0;
    #if (VDEC_MCORE_NOT_SUPPORT)
    u4VDecID = 0;
    #endif
    
    if (0 == u4VDecID)
    {
        u4Val = (u4ReadReg(AVC_MV_BASE0 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, AVC_MV_BASE0, u4Addr, u4Val);
        return u4Val;
    }
    else if (1 == u4VDecID)
    {
        u4Val = (u4ReadReg(AVC_MV_BASE1 + u4Addr)); 
        vVDecSimDumpR(u4VDecID, AVC_MV_BASE1, u4Addr, u4Val);
        return u4Val;
    }
    else
    {
        //lae do nothing
    }
    return 0;
}


// *********************************************************************
// Function : BOOL fgIsVLDFetchOk(void)
// Description : Check if VLD fetch is done
// Parameter : None
// Return    : TRUE: VLD fetch OK, FALSE: not OK
// *********************************************************************
static BOOL _fgIsVLDFetchOk(UINT32 u4VDecID)
{
    if((u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
    {
        return FALSE;
    }
    return TRUE;
}


// *********************************************************************
// Function : BOOL fgIsVLDFetchOk2(void)
// Description : Check if VLD fetch is done
// Parameter : None
// Return    : TRUE: VLD fetch OK, FALSE: not OK
// *********************************************************************
static BOOL _fgIsVLDFetchOk2(UINT32 u4VDecID)
{
    if((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_FETCHOK) & VLD_FETCH_OK) == 0)
    {
        return FALSE;
    }
    return TRUE;
}


// *********************************************************************
// Function : BOOL fgWaitVldFetchOk(void)
// Description :
// Parameter :
// Return    : TRUE: Init Barrel Shifter OK, FALSE: Time Out
// *********************************************************************
static BOOL _fgWaitVldFetchOk(UINT32 u4VDecID)
{
    UINT32 u4Cnt;

    if(!_fgIsVLDFetchOk(u4VDecID))
    {
        u4Cnt = 0;
        while(!_fgIsVLDFetchOk(u4VDecID))
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                return FALSE;
            }
        }
    }
    #ifdef WAIT_SRAM_STABLE
        u4Cnt = 0;
        #ifdef IS_POST_MT5368
        if(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
        {
            while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & S_BITS_PROC_NOP)==0)
            {
                u4Cnt++;
                if(u4Cnt >= WAIT_THRD)
                {
                    LOG(2, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                        u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR),
                        u4VDecReadVLD(u4VDecID, RO_VLD_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK));
                    return FALSE;
                }    
            }        
        }
        #else
        while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(2, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                    u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR),
                    u4VDecReadVLD(u4VDecID, RO_VLD_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK));
                return FALSE;
            }    
        }        
        #endif    
    #endif
    
    return TRUE;
}


// *********************************************************************
// Function : BOOL fgWaitVldFetchOk(void)
// Description :
// Parameter :
// Return    : TRUE: Init Barrel Shifter OK, FALSE: Time Out
// *********************************************************************
static BOOL _fgWaitVldFetchOk2(UINT32 u4VDecID)
{
    UINT32 u4Cnt;

    if(!_fgIsVLDFetchOk2(u4VDecID))
    {
        u4Cnt = 0;
        while(!_fgIsVLDFetchOk2(u4VDecID))
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                return FALSE;
            }
        }
    }
    #ifdef WAIT_SRAM_STABLE
    u4Cnt = 0;
    #ifdef IS_POST_MT5368
    if(u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & PROCESS_FLAG)
    {
        while((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & S_BITS_PROC_NOP)==0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(2, "//===2SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                    u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_2ND_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VRPTR),
                    u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_FETCHOK));
                return FALSE;
            }    
        }        
    }
    #else

    while((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & AA_FIT_TARGET_D)==0)
    {
        u4Cnt++;
        if(u4Cnt >= WAIT_THRD)
        {
            LOG(2, "//===2SRAM !Stable 0x4F4=0x%x, Rptr 0x4B0=0x%x, 0x4FC=0x%x, 0x4EC=0x%x, 0x4E8=0x%x\n", 
                u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_2ND_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VRPTR),
                u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_FETCHOK));
            return FALSE;
        }    
    }
    //LOG(6, "//2WaitFetchOK Rptr 0x4B0=0x%x, 0x4FC=0x%x, 0x4EC=0x%x, 0x4E8=0x%x\n", 
    //    VLD_READ32(RW_VLD_2ND_RPTR), VLD_READ32(RO_VLD_2ND_VRPTR),
    //    VLD_READ32(RO_VLD_2ND_VBAR), VLD_READ32(RO_VLD_2ND_FETCHOK));
    #endif
    #endif
    
    return TRUE;
}


// *********************************************************************
// Function : DWRD dAVCVLDGetBitS(DWRD dShiftBit)
// Description : Get Bitstream from VLD barrel shifter
// Parameter : dShiftBit: Bits to shift (0-32)
// Return    : barrel shifter
// *********************************************************************
static UINT32 _AVCVLDGetBitS(UINT32 u4VDecID, UINT32 u4ShiftBit)//, BYTE bBarrelNum)
{
    UINT32 u4RegVal, u4Rptr, u4Wptr;

    u4Rptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR);
    u4Wptr = u4VDecReadVLD(u4VDecID, RO_VLD_VWPTR);

    ASSERT(_u4VFIFOSize > 0);
    if(AVAILABLE_SZ(u4Rptr, u4Wptr, _u4VFIFOSize) <= MIN_LEN)
    {
        LOG(3, "GetBitS Rptr==Wptr\n");
        x_thread_delay(1);
        return 0;
    }
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        u4RegVal = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_BARL + (u4ShiftBit << 2));
    }
    else        
    #endif
    {
        u4RegVal = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL + (u4ShiftBit << 2));
    }

    return u4RegVal;
}


// *********************************************************************
// Function : DWRD dVLDSramRp(void)
// Description : Get VLD SRAM Read Ptr
// Parameter : None
// Return    : VLD SRAM Read PTR
// *********************************************************************
static UINT32 _dVLDSramRp(UINT32 u4VDecID)
{
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return(u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VBAR) & 0x3f);
    }  
    else        
    #endif
    {
        #ifdef IS_POST_MT5363
        UINT32 u4Reg = 0;
        u4Reg = u4VDecReadVLD(u4VDecID, RO_VLD_VBAR);
        return (((u4Reg & 0xf) * 4) + ((u4Reg >> 24) & 0x3));   
        
        #else
        return(u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0x3f);
        #endif
    }
}


// *********************************************************************
// Function : DWRD dVLDSramWp(void)
// Description : Get VLD SRAM Write Ptr
// Parameter : None
// Return    : VLD SRAM Write PTR
// *********************************************************************
static UINT32 _dVLDSramWp(UINT32 u4VDecID)
{
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VBAR) >> 16) & 0x3f);
    }
    else        
    #endif
    {
        #ifdef IS_POST_MT5363
        UINT32 u4Reg = 0;
        u4Reg = u4VDecReadVLD(u4VDecID, RO_VLD_VBAR);
        return (((u4Reg >> 8) & 0xf) * 4);
        
        #else
        return((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) >> 16) & 0x3f);
        #endif
    }
}  


// *********************************************************************
// Function : DWRD dVLDDramRp(void)
// Description : Get VLD DRAM Read Ptr
// Parameter : None
// Return    : VLD DRAM Read PTR
// *********************************************************************
static UINT32 _dVLDDramRp(UINT32 u4VDecID)
{
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VRPTR);// & 0xffffff);
    }
    else
    #endif
    {
        return u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR);// & 0xffffff);
    }
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

// *********************************************************************
// Function : void vVLDHWReset(void)
// Description : Reset Video decode HW
// Parameter : None
// Return    : None
// *********************************************************************
void _VDEC_VLDHWReset(BOOL fgIsUFOMode, BOOL fgIsDualCoreMode, UINT32 u4VDecID)
{
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, if(`VDEC_PROCESS_FLAG == 1) wait(`VDEC_BITS_PROC_NOP == 1);\n", u4VDecID);
#endif
    #ifdef WAIT_SRAM_STABLE
    #ifdef IS_POST_MT5368
    #else
    UINT32 u4Cnt = 0;
    while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
    {
        u4Cnt++;
        if(u4Cnt >= WAIT_THRD)
        {
            LOG(2, "WaitSramStable Fail\n");
            break;
        }
    }
    //to avoid reset when HW still access DRAM
    vVDecWriteMC(u4VDecID, RW_MC_PP_DRAM_ACCESS, ACCESS_DISABLE);
    #endif
    #endif

    //sw reset
    #ifdef IS_POST_MT5368
    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
    {       
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_LAE_CORE,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_LAE_CORE) | (1 << (22 + u4VDecID)));
	}
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0x101);
    _VDEC_CkgenSel(u4VDecID, VDEC_FMT_H264, 0);
    #ifdef CC_VDEC_DYN_POWER_SAVING
    //vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_SPEC, PDN_SPEC_H264);
    vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_SPEC, 0x0/*PDN_SPEC_H264*/);
    if (fgIsDualCoreMode)
    {
        vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE, 0x0/*0x11E20180)*/);
        vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE2, 0x0/*0x20*/);
    }
    else
    {
        vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE, 0x0/*PDN_MODULE_H264*/);
        //vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE, PDN_MODULE_H264);
        #if (defined(VDEC_IS_POST_MT5882))
        vVDecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE2, 0x0/*PDN_MODULE2_H264*/);
        //DecWriteMISC(u4VDecID, RW_VDEC_MISC_PDN_CON_MODULE2, PDN_MODULE2_H264);
        #endif
    }
    if (fgIsUFOMode)
    {
        if (fgIsDualCoreMode)
        {
            vVDecWriteMCORETOP(u4VDecID, RW_VDEC_MCORE_PDN_CON_MODULE, 0/*0x20000*/);
        }
        else if (VDEC_VLD0 == u4VDecID)
        {
            vVDecWriteMCORETOP(u4VDecID, RW_VDEC_MCORE_PDN_CON_MODULE, 0/*0x1030102*/);
        }
        else if (VDEC_VLD1 == u4VDecID)
        {
            vVDecWriteMCORETOP(u4VDecID, RW_VDEC_MCORE_PDN_CON_MODULE, 0/*0x1030101*/);
        }
        else 
        {
            vVDecWriteMCORETOP(u4VDecID, RW_VDEC_MCORE_PDN_CON_MODULE, 0/*0x1030100*/);
        }
    }
    #endif
    vVDecWriteMISC(u4VDecID, VDEC_SYS_CLK_SEL, CLK_FOR_H264);
    _VDEC_SysClkSel(u4VDecID, VDEC_FMT_H264, 0);
    #else //IS_POST_MT5368
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 1);
    #endif
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);  
    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
    {               
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_LAE_CORE,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_LAE_CORE) & (~(1 << (22 + u4VDecID))));
    }
    #ifdef IS_POST_MT5368
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_TIMEOUT_THD, AVLD_TIMEOUT_THD);
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_MC_BUSY_THD, MC_BUSY_THD);
    #else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_TIMEOUT_THD, AVLD_TIMEOUT_THD);
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_MC_BUSY_THD, MC_BUSY_THD);
    #endif
    vVDecWriteMC(u4VDecID, RW_MC_PP_DRAM_ACCESS, ACCESS_ENABLE);
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, _VDEC_VLDHWReset End\n", u4VDecID);
#endif
}


// *********************************************************************
// Function : void vVLDHWReset(void)
// Description : Reset Video decode HW
// Parameter : None
// Return    : None
// *********************************************************************
void _VDEC_VLDHWReset2(UINT32 u4VDecID,BOOL bIsInit)
{
    #ifdef WAIT_SRAM_STABLE
    if(!bIsInit)
    {    
        while((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & AA_FIT_TARGET_D)==0){}
    }
    #endif   
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, (UINT32)(1<<8));
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
    //LOG(6, "//2SRST Rptr 0x4B0=0x%x, 0x4FC=0x%x\n", VLD_READ32(RW_VLD_2ND_RPTR), VLD_READ32(RO_VLD_2ND_VRPTR));    
}


// *********************************************************************
// Function : void vResetVLD1(DWRD dwRDPtrAddr)
// Description : Reset VLD1
// Parameter : u4RDPtrAddr, u4BsFifo, u4PredSA
// Return    : None
// *********************************************************************
BOOL _VDEC_ResetVLD1(UINT32 u4VDecID, UINT32 u4RDPtrAddr, UINT32 u4WPtrAddr, VDEC_H264_INFO_T* prVdec)
{  
    UINT32 u4AvalSz;     
    //VLD_WRITE32(RW_VLD_RDY_SWTICH, (UINT32)READY_TO_RISC);
#ifdef AVC_HW_TIMER
    if(_hHWParseSema == NULL)
    {
        VERIFY(x_sema_create(&_hHWParseSema, X_SEMA_TYPE_BINARY,
            X_SEMA_STATE_LOCK) == OSR_OK);
    }
#endif
    if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
    {
        vVDecWriteVLD(u4VDecID, VLD_REINIT_FETCH_MODE, (u4VDecReadVLD(u4VDecID,VLD_REINIT_FETCH_MODE) | (1 << 20)));
        vVDecWriteBS2(u4VDecID, BS_REINIT_FETCH_MODE, (u4VDecReadBS2(u4VDecID,BS_REINIT_FETCH_MODE) | (1 << 20)));
    }
    #ifdef IS_POST_MT5368
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_MDEC_TIMEOUT_INT_SWITCH, MDEC_INT_SWITCH_EN);
    #endif
    
#if (defined (VDEC_IS_POST_MT5882))
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, AVC_EN | AVC_RDY_WITH_CNT | AVC_RDY_CNT_THD | AVLD_MEM_PAUSE_MOD_EN |  
            AVC_SUM6_APPEND_INV |AVC_NOT_CHK_DATA_VALID | AVC_RBSP_CHK_INV | AVC_ERR_BYPASS | AVC_ERR_CONCEALMENT | 
            (1 << 29) | (1 << 25) | (1 << 26));
#else
    if(prVdec->rDecParam.u1ECMode > 0)  //frame decoding
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, (UINT32)(AVC_DEC_EN | AVC_RDY_WITH_CNT | 
            AVC_RDY_CNT_THD | AVLD_MEM_PAUSE_MOD_EN | 
            AVC_SUM6_APPEND_INV |AVC_NOT_CHK_DATA_VALID | AVC_RBSP_CHK_INV | AVC_READ_FLAG_CHK_INV |
            AVC_DEC_CYCLE_EN | 
            AVC_DEC_ERROR_SWITCH_ON | AVC_DEC_ERROR_ONLY1MC_START | AVC_DEC_ERROR_SKIP_PIC_END |
            AVC_RBSP_SWITCH |AVC_NON_SPEC_SWITCH|AVC_NOT_CHK_DATA_NSPEC)
            );
        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0x8e9fc001);
    }
    else                       //header parsing
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, (UINT32)(AVC_DEC_EN | AVC_RDY_WITH_CNT | 
            AVC_RDY_CNT_THD | AVLD_MEM_PAUSE_MOD_EN | 
            AVC_SUM6_APPEND_INV |AVC_NOT_CHK_DATA_VALID | AVC_RBSP_CHK_INV | AVC_READ_FLAG_CHK_INV |
            AVC_DEC_CYCLE_EN | 
            AVC_DEC_ERROR_SWITCH_OFF | AVC_DEC_ERROR_ONLY1MC_START | AVC_DEC_ERROR_SKIP_PIC_END |
            AVC_RBSP_SWITCH |AVC_NON_SPEC_SWITCH|AVC_NOT_CHK_DATA_NSPEC)
            );
        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0x8e9fc001);
    }
#endif
    
#if (defined (VDEC_IS_POST_MT5882))
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RM03R, ((u4VDecReadAVCVLD(u4VDecID,  RW_AVLD_RM03R)) & 0xFFFFFFDF)| MEM_PAUSE_EN);  
#else //(defined (CC_MT5890))
#ifdef MEM_PAUSE_SUPPORT
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV, (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) & 0xFFFF1FFC) | MEM_PAUSE_EN);
#else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV, (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) & 0xFFFF1FFC) | MEM_PAUSE_DISABLE);
#endif
    
#ifndef NON_SPEC_CTRL1_EN
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) | NON_SPEC_SHIFT_EN);
#endif

#ifdef IS_POST_MT5363
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) | NEIGHBOR_WRITE_DISABLE);
#endif   
#endif
    //error concealment
#if (defined (VDEC_IS_POST_MT5882))   
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERC_CTRL, 0x101);
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERC_DED_ERR_TYPE, 0x204008);
#else //(defined (CC_MT5890))
#ifdef ERROR_CCM_EN
    if((prVdec->rDecParam.u1ECMode & 0x1) == 0)
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERC_CTRL, ERC_DISABLE);
    }
    else
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERC_CTRL, ((prVdec->rDecParam.u1ECMode & 0xC)<<6) | (prVdec->rDecParam.u1ECMode & 0x3));
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERC_DED_ERR_TYPE, ((prVdec->rDecParam.u4ECType & DISABLE_FIND_STARTCODE_ERR) /*& ERR_MASK*/));
    }
#endif
#endif
    vVDecWriteAVCMV(u4VDecID, RW_AMV_EC_SETTING, AVOID_HANG/* | SKIP_MODE3*/);
    if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, 0);
    }
    else
    {
        #if (defined (VDEC_IS_POST_MT5882))   
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, (~(AVLD_MB_END_CHK |AVLD_4BLOCKS_SKIP_CHK))/*ERR_MASK_MM*/);
        #else
        if(prVdec->rDecParam.fgIsMMPlay || prVdec->rDecParam.fgUseMMErrMask)
        {
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, ERR_MASK_MM);
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ERR_MASK, ERR_MASK);
        }  
        #endif
    }
    vVDecWriteMC(u4VDecID, RW_MC_OPBUF, 6);    
    // Set for VLD boundary check 0x5000000
    _VDEC_VLDSetVFifo(u4VDecID, PHYSICAL(prVdec->rPesInfo.u4FifoStart), PHYSICAL(prVdec->rPesInfo.u4FifoEnd));
    if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
    {
        vVDecSetBS2VFIFO(0, u4VDecID, PHYSICAL(prVdec->rPesInfo.u4FifoStart), PHYSICAL(prVdec->rPesInfo.u4FifoEnd));
    }
    #ifdef IS_POST_MT5368
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_PRED_ADDR, PHYSICAL(prVdec->rDecParam.u4PredSA));    //buffer for VLD, 32*1024    
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_PRED_MODE, u4VDecReadVLDTOP(u4VDecID, RW_AVLD_PRED_MODE) | PRED_SRAM_USED);
    #else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PRED_ADDR, PHYSICAL(prVdec->rDecParam.u4PredSA) | PRED_SRAM_USED);    //buffer for VLD, 32*1024    
    #endif

    if(u4WPtrAddr > u4RDPtrAddr)
    {
        u4AvalSz = u4WPtrAddr - u4RDPtrAddr;
    }
    else
    {
        u4AvalSz = (prVdec->rPesInfo.u4FifoEnd - u4RDPtrAddr) + (u4WPtrAddr - prVdec->rPesInfo.u4FifoStart);
    }
    if(u4AvalSz > ((prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart) - (WPTR_OFFSET<<1)))
    {
        //LOG(2, "Fifo Data full\n");
        u4WPtrAddr -= WPTR_OFFSET;
    }
    // set write ptr    
    _VDEC_SetVldWPtr(u4VDecID, u4WPtrAddr, prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd);
    if(!_VDEC_VLDInitBarrelShifter(u4VDecID, u4RDPtrAddr))
    {
        LOG(2, "Barrel Shifter Init failed\n");
        return FALSE;
    }
    return TRUE;
}


// *********************************************************************
// Function : void vResetVLD2(DWRD dwRDPtrAddr)
// Description : Reset VLD2
// Parameter : None
// Return    : None
// *********************************************************************
BOOL _VDEC_ResetVLD2(UINT32 u4VDecID, UINT32 u4RDPtrAddr, UINT32 u4WPtrAddr,
    UINT32 u4BsFifoStart, UINT32 u4BsFifoEnd)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_RDY_SWTICH, (UINT32)READY_TO_RISC);
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_2ND_BARL_CTRL, AVLD_2ND_BARL_EN);    
    _VDEC_VLDSetVFifo2(u4VDecID, PHYSICAL(u4BsFifoStart), PHYSICAL(u4BsFifoEnd));
    /*u4Temp = 32 - (_VDEC_AVCVLDShiftBits() % 32);
    // at list trailing bit
    if(u4Temp)
    {
        u4Temp = _VDEC_AVCGetBitStream(u4Temp);
    }*/
    // set write ptr
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_WPTR, u4WPtrAddr);    //set wptr infinity

    if(!_VDEC_VLDInitBarrelShifter2(u4VDecID, u4RDPtrAddr))
    {
        LOG(2, "Barrel Shifter 2 Init failed\n");
        return FALSE;
    }  
    return TRUE;
}


// *********************************************************************
// Function : vVLDSetVFifo(DWRD dSa, DWRD dEa)
// Description : Set VLD VFifo Sa, Ea
// Parameter : dSa: Absolute Dram Address of VFifo Start
//             dEa: Absolute Dram Address of VFifo End
// Return    : None
// *********************************************************************
void _VDEC_VLDSetVFifo(UINT32 u4VDecID, UINT32 u4Sa, UINT32 u4Ea)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_VSTART, u4Sa >> 6);
    vVDecWriteVLD(u4VDecID, RW_VLD_VEND, u4Ea >> 6);
    _u4VFIFOSize = u4Ea - u4Sa;
}


// *********************************************************************
// Function : vVLDSetVFifo(DWRD dSa, DWRD dEa)
// Description : Set VLD VFifo Sa, Ea
// Parameter : dSa: Absolute Dram Address of VFifo Start
//             dEa: Absolute Dram Address of VFifo End
// Return    : None
// *********************************************************************
void _VDEC_VLDSetVFifo2(UINT32 u4VDecID, UINT32 u4Sa, UINT32 u4Ea)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_VSTART, u4Sa >> 6);
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_VEND, u4Ea >> 6);
}


// *********************************************************************
// Function : BOOL fgVLDInitBarrelShifter(DWRD dwPtr)
// Description : Init HW Barrel Shifter
// Parameter : dwPtr: Physical DRAM Start Address to fill Barrel Shifter
// Return    : TRUE: Initial Success, Fail: Initial Fail
// *********************************************************************
BOOL _VDEC_VLDInitBarrelShifter(UINT32 u4VDecID, UINT32 u4Ptr)
{
    UINT32 u4RPtr;
    UINT32 u4TimeOutCnt = 0;
    
    _u1BarrelShifterNum = BARREL1;
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, i4VDEC_HAL_H264_InitBarrelShifter Start\n", u4VDecID);
#endif
    vVDecWriteVLD(u4VDecID, RW_VLD_RPTR, u4Ptr);
    if(u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR) != (u4Ptr - (u4Ptr % RPTR_ALIGN)))
    {
        LOG(2, "RPTR moved before INIFET\n");
        return FALSE;
    }

#ifdef IS_POST_MT5368
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0x100);
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);
#endif
    // start to fetch data &  Reset AVLD_SUM
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RESET_SUM, AVLD_RESET_SUM_ON);
    
#ifndef IS_POST_MT5368
    vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) | 0x101);
    vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) |0x301);
    vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) |0x101);
#endif

    vVDecWriteVLD(u4VDecID, RW_VLD_PROC, VLD_INIFET);
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, _fgWaitVldFetchOk Start\n", u4VDecID);
#endif
    if(!_fgWaitVldFetchOk(u4VDecID))
    {
        LOG(2, "!HwRptr 0x%x, Rptr 0x%x\n", _VDEC_VldRPtr(u4VDecID), u4Ptr);
        return FALSE;
    }
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, _fgWaitVldFetchOk End\n", u4VDecID);
#endif

    vVDecWriteVLD(u4VDecID, RW_VLD_PROC, VLD_INIBR);

#ifndef IS_POST_MT5368
    UINT32 u4Cnt = 0;
    #if (defined (VDEC_IS_POST_MT5882))    //#ifdef MEM_PAUSE_SUPPORT

    #ifdef IS_POST_MT5363
    while((((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0xf) * 4) + 
        ((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) >> 24) & 0x3)) !=5)
    #else
    while((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0x3f) !=5)
    #endif
    {
        u4Cnt++;
        if(u4Cnt >= WAIT_THRD)
        {
            LOG(2, "FetchOK-WaitSramStable Fail\n");
            break;
        }    
    }
    #else

    #ifdef IS_POST_MT5363
    while((((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0xf) * 4) + 
        ((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) >> 24) & 0x3)) !=4)    
    #else
    while((u4VDecReadVLD(u4VDecID, RO_VLD_VBAR) & 0x3f) !=4)
    #endif    
    {
        u4Cnt++;
        if(u4Cnt >= WAIT_THRD)
        {
            LOG(2, "FetchOK-WaitSramStable Fail\n");
            break;
        }      
    }
    #endif
#endif
    
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RESET_SUM, AVLD_RESET_SUM_OFF);  

    //LOG(9, "//InWin 0x%x\n", VLD_READ32(RO_VLD_BARL));
    while(_VDEC_VldRPtr(u4VDecID) < u4Ptr)
    {
        UNUSED(_VDEC_AVCGetBitStream(u4VDecID, 8));
        if(u4TimeOutCnt++ > WAIT_THRD)
        {
            LOG(2, "//Init Rptr fail\n");
            break;
        }
    }

    if((u4RPtr = _VDEC_VldRPtr(u4VDecID)) != u4Ptr)
    {
        LOG(2, "BarrelShifter init u4Ptr 0x%x failed 0x%x\n", u4Ptr, u4RPtr);
    }
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, i4VDEC_HAL_H264_InitBarrelShifter End\n", u4VDecID);
#endif
    UNUSED(u4RPtr);
    return TRUE;
}


// *********************************************************************
// Function : BOOL fgVLDInitBarrelShifter2(DWRD dwPtr)
// Description : Init HW Barrel Shifter
// Parameter : dwPtr: Physical DRAM Start Address to fill Barrel Shifter
// Return    : TRUE: Initial Success, Fail: Initial Fail
// *********************************************************************
BOOL _VDEC_VLDInitBarrelShifter2(UINT32 u4VDecID, UINT32 u4Ptr)
{
    UINT32 u4Remainder;
    
    _u1BarrelShifterNum = BARREL2;

    u4Remainder = u4Ptr % 16; //VLD read ptr 16-byte aligned

    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_RPTR, u4Ptr);
    if(u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VRPTR) != (u4Ptr - u4Remainder))
    {
        LOG(2, "2RPTR moved before INIFET\n");
        //return FALSE;
    }
    // start to fetch data    
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_ASYNC, 0x101);
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_ASYNC, 0x301);
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_ASYNC, 0x101);

    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_PROC, VLD_INIFET);
    if(!_fgWaitVldFetchOk2(u4VDecID))
    {
        return FALSE;
    }
    vVDecWriteVLD(u4VDecID, RW_VLD_2ND_PROC, VLD_INIBR);
    while(_VDEC_VldRPtr(u4VDecID) < u4Ptr)
    {
        UNUSED(_VDEC_AVCGetBitStream(u4VDecID, 8));
    }  
    return TRUE;
}


// *********************************************************************
// Function : DWRD dwGetBitStream(DWRD dwShiftBit)
// Description : get 32 bits from shifter
// Parameter : bShiftBit: if 0, just get 32 bits from shifter
//                        others, 1. remove bShiftBit bits
//                                2. get 32 bits from shifter
// Return    : 32 bits value
// *********************************************************************
UINT32 _VDEC_AVCGetBitStream(UINT32 u4VDecID, UINT32 u4ShiftBit)
{
    UINT32 u4RegVal;
    //_dwBitCount += dwShiftBit;
    u4RegVal = _AVCVLDGetBitS(u4VDecID, u4ShiftBit);
    return u4RegVal;
}


// *********************************************************************
// Function : DWRD dwGetBitStream(DWRD dwShiftBit)
// Description : get 32 bits from shifter
// Parameter : bShiftBit: if 0, just get 32 bits from shifter
//                        others, 1. get 32 bits from the current position
//                                2. remove bShiftBit bits
// Return    : 32 bits value
// *********************************************************************
UINT32 _VDEC_AVCGetShtBitStream(UINT32 u4VDecID, UINT32 u4ShiftBit)
{
    UINT32 u4RegVal1;
    //_dwBitCount += dwShiftBit;
    u4RegVal1 = _AVCVLDGetBitS(u4VDecID, 0);
    UNUSED(_AVCVLDGetBitS(u4VDecID, u4ShiftBit));
    return(u4RegVal1);
}


// *********************************************************************
// Function : DWRD dwGetRealShtBitStream(DWRD dwShiftBit)
// Description : get dwShiftBit bits from shifter
// Parameter : bShiftBit: if 0, just get 32 bits from shifter
//                        others, 1. get 32 bits from the current position
//                                   2. remove bShiftBit bits
// Return    : 32 bits value
// *********************************************************************
UINT32 _VDEC_AVCGetRealShtBitStream(UINT32 u4VDecID, UINT32 u4ShiftBit)
{
    UINT32 u4RegVal1;
    //_dwBitCount += dwShiftBit;
    u4RegVal1 = _AVCVLDGetBitS(u4VDecID, 0);
    UNUSED(_AVCVLDGetBitS(u4VDecID, u4ShiftBit)); 
    return(u4RegVal1 >> (32 - u4ShiftBit));
}


VOID _VDEC_AVCYC(UINT32 u4VDecID, VDEC_DISP_PIC_INFO_T* prDispInfo)
{   
    int i,j;  
    VDEC_DISP_PIC_INFO_T* prDispInfoForCrc= prDispInfo;
    for(i=0;i<4;i++)
    {
        prDispInfoForCrc->u4Ycrc[i] = IO_READ32(IO_VIRT + 0x3000,(2+i)*4);
    }
    for(j=0;j<4;j++)
    {
        prDispInfoForCrc->u4Ccrc[j] = IO_READ32(IO_VIRT + 0x3000,(6+j)*4);
    }
    // 0:3 CRC check, 4:7 MC input check, 8:9 MC check, 10 MC Dram check
    prDispInfoForCrc->u4Ycrc[4] = u4VDecReadMC(u4VDecID, 378 << 2);
    prDispInfoForCrc->u4Ycrc[5] = u4VDecReadMC(u4VDecID, 379 << 2);
    prDispInfoForCrc->u4Ycrc[6] = u4VDecReadMC(u4VDecID, 380 << 2);
    prDispInfoForCrc->u4Ycrc[7] = u4VDecReadMC(u4VDecID, 383 << 2);
    prDispInfoForCrc->u4Ycrc[8] = u4VDecReadMC(u4VDecID, 381 << 2);
    prDispInfoForCrc->u4Ycrc[9] = u4VDecReadMC(u4VDecID, 382 << 2);
    prDispInfoForCrc->u4Ycrc[10] = u4VDecReadMC(u4VDecID, 390 << 2);
}


// *********************************************************************
// Function : BOOL fgGetBitStreamFlg(void)
// Description : get 1 bits from shifter
// Parameter : 1. remove bShiftBit bits
//                   2. get 1 bit from shifter
// Return    : 32 bits value
// *********************************************************************
BOOL _VDEC_AVCGetBitStreamFlg(UINT32 u4VDecID)
{
    UINT32 u4RegVal;
    u4RegVal = _VDEC_AVCGetShtBitStream(u4VDecID, 1);
    return((u4RegVal >> 31));
}


// *********************************************************************
// Function    : DWRD dwUeCodeNum(BYTE bBarrelNum)
// Description : Ue
// Parameter   : 
// Return      : 
// *********************************************************************
UINT32 _VDEC_UeCodeNum(UINT32 u4VDecID)
{
    UINT32 u4Rptr, u4Wptr;
    
    u4Rptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR);
    u4Wptr = u4VDecReadVLD(u4VDecID, RO_VLD_VWPTR);
    
    ASSERT(_u4VFIFOSize > 0);
    if(AVAILABLE_SZ(u4Rptr, u4Wptr, _u4VFIFOSize) <= MIN_LEN)
    {
        LOG(3, "Ue Rptr==Wptr\n");
        x_thread_delay(1);
        return 0;
    }
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_UE);
    }
    else
    #endif
    {
        return u4VDecReadAVCVLD(u4VDecID, RO_AVLD_UE);
    }
}


// *********************************************************************
// Function    : INT iSeCodeNum(BYTE bBarrelNum)
// Description : Ue
// Parameter   : 
// Return      : 
// *********************************************************************
INT32 _VDEC_SeCodeNum(UINT32 u4VDecID)
{
    UINT32 u4Rptr, u4Wptr;
    
    u4Rptr = u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR);
    u4Wptr = u4VDecReadVLD(u4VDecID, RO_VLD_VWPTR);
    
    ASSERT(_u4VFIFOSize > 0);
    if(AVAILABLE_SZ(u4Rptr, u4Wptr, _u4VFIFOSize) <= MIN_LEN)
    {
        LOG(3, "Se Rptr==Wptr\n");
        x_thread_delay(1);
        return 0;
    }
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return((INT32)u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_SE));
    }  
    else        
    #endif
    {
        return((INT32)u4VDecReadAVCVLD(u4VDecID, RO_AVLD_SE));
    }
}


// *********************************************************************
// Function    : DWRD vTrailingBits()
// Description : Ue
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_TrailingBits(UINT32 u4VDecID)
{
    UINT32 u4Temp;

    u4Temp = 8 - (_VDEC_AVCVLDShiftBits(u4VDecID) % 8);
    // at list trailing bit
    if(u4Temp < 8)
    {
        u4Temp = _VDEC_AVCGetBitStream(u4VDecID, u4Temp);
    }
}


// *********************************************************************
// Function    : DWRD vSetSPSAVLD()
// Description : Set SPS related registers
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_SetSPSAVLD(UINT32 u4VDecID, UINT32 u4SPSInfo, UINT32 u4Width, UINT32 u4Height)
{
#ifdef VDEC_SIM_DUMP
       LOG(0, "[H264]%d, _VDEC_SetSPSAVLD Start\n", u4VDecID);
#endif
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SPS, u4SPSInfo);
    #ifdef IS_POST_MT5368
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_PIC_SIZE, (u4Height << 16) | u4Width);
    #else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PIC_SIZE, (u4Height << 8) | u4Width);
    #endif
#ifdef VDEC_SIM_DUMP
       LOG(0, "[H264]%d, _VDEC_SetSPSAVLD End\n", u4VDecID);
#endif
}


// *********************************************************************
// Function    : DWRD vSetPPSAVLD()
// Description : Set SPS related registers
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_SetPPSAVLD(UINT32 u4VDecID, UINT32 u4PPSInfo1, UINT32 u4PPSInfo2)
{ 
#ifdef VDEC_SIM_DUMP
   LOG(0, "[H264]%d, _VDEC_SetPPSAVLD Start\n", u4VDecID);
#endif
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PPS_1 , u4PPSInfo1);  
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PPS_2 , u4PPSInfo2);
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, _VDEC_SetPPSAVLD End\n", u4VDecID);
#endif

}


// *********************************************************************
// Function    : DWRD vSetPPSAVLD1()
// Description : Set SPS related registers
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_SetSHDRAVLD1(UINT32 u4VDecID, UINT32 u4SHDRInfo, UINT32 u4FirstMBInSlice)
{
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, _VDEC_SetSHDRAVLD1 Start\n", u4VDecID);
#endif

    #ifdef IS_POST_MT5368
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1, u4SHDRInfo & 0xFFFFE000);  
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1_2, u4FirstMBInSlice & 0x1FFFF);   
#if (defined (VDEC_IS_POST_MT5882))    
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1_2, (u4VDecReadAVCVLD(u4VDecID,RW_AVLD_SHDR_1_2) & 0xffff0000));  
#endif
    #else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_1, u4SHDRInfo);  
    UNUSED(u4FirstMBInSlice);
    #endif
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, _VDEC_SetSHDRAVLD1 End\n", u4VDecID);
#endif

}


// *********************************************************************
// Function    : DWRD vSetPPSAVLD2()
// Description : Set SPS related registers
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_SetSHDRAVLD2(UINT32 u4VDecID, UINT32 u4SHDRInfo)
{
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, _VDEC_SetSHDRAVLD2 Start\n", u4VDecID);
#endif
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_2, u4SHDRInfo);
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]%d, _VDEC_SetSHDRAVLD2 End\n", u4VDecID);
#endif
}


// *********************************************************************
// Function : void vMCGetMbXY(DWRD *pdX, DWRD *pdY)
// Description : MC Get Last Decoded Macro Block Position
// Parameter : pdX, pdY: Pointer to X, Y
// Return    : X, Y position for MB, store in pdX, pdY
// *********************************************************************
void _VDEC_MCGetMbXY(UINT32 u4VDecID, UINT32 *pdX, UINT32 *pdY)
{   
    ASSERT(pdX!=NULL);
    ASSERT(pdY!=NULL);

    *pdX = u4VDecReadMC(u4VDecID, RO_MC_MBX);
    *pdY = u4VDecReadMC(u4VDecID, RO_MC_MBY);
}


// *********************************************************************
// Function : void vWriteScalingList(DWRD dwIdx, dwData)
// Description : 
// Parameter : 
// Return    : 
// *********************************************************************
void _VDEC_WriteScalingList(UINT32 u4VDecID, UINT32 u4Idx, const CHAR* pcSlicePtr)
{
    INT32 i;
    UINT32 u4Temp;

#if (defined (VDEC_IS_POST_MT5882))
    #ifdef VDEC_SIM_DUMP
    LOG(0, "%d, ==========2 pix mode===========\n", u4VDecID);
    #endif
    vVDecWritePP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN, (vVDecReadPP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN)| 0x1));
    if (u4Idx < 6)
    {
        for (i=0; i<16; i=i+2)
        {
            u4Temp = ((UCHAR)pcSlicePtr[i+1] << 8) + (UCHAR)pcSlicePtr[i];
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, (u4Idx << 4) + i);
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
            //vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + (u4Idx << 4) + i);
        }
    }
    else
    {
        for (i=0; i<64; i=i+2)
        {
            u4Temp = ((UCHAR)pcSlicePtr[i+1] << 8) + (UCHAR)pcSlicePtr[i];
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 96 + ((u4Idx - 6) << 6) + i);
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
            //vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + (u4Idx << 4) + i);
        }
    }
    vVDecWritePP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN, (vVDecReadPP(u4VDecID, AVC_PP_IQ_SRAM_CONTROL_EN)& 0xfffffffe));
#else //(defined (CC_MT5890))
    #ifdef VDEC_SIM_DUMP
    LOG(0, "%d, ==========1 pix mode===========\n", u4VDecID);
    #endif
    if (u4Idx < 6)
    {
        u4Idx = (u4Idx << 4);
        for(i=0; i<4; i++)
        {
            // add 16 for every list
            u4Temp = (((UCHAR)pcSlicePtr[i<<2]) << 24) + (((UCHAR)pcSlicePtr[(i<<2) + 1]) << 16) +(((UCHAR)pcSlicePtr[(i<<2) + 2]) << 8) + (((UCHAR)pcSlicePtr[(i<<2) + 3]));
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x200 + u4Idx + (i << 2));
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + u4Idx + (i << 2));
        }
    }
    else
    {
        u4Idx = (u4Idx == 6) ? (u4Idx << 4) : ((u4Idx + 3) << 4); // 6=>16*6   7=>16*6+64(equal to 16*7+48)
        for (i=0; i<16; i++)
        {
            // add 64 for every list
            u4Temp = (((UCHAR)pcSlicePtr[i<<2]) << 24) + (((UCHAR)pcSlicePtr[(i<<2) + 1]) << 16) +(((UCHAR)pcSlicePtr[(i<<2) + 2]) << 8) + (((UCHAR)pcSlicePtr[(i<<2) + 3]));
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x200 + u4Idx + (i << 2));
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_DATA, u4Temp);
            vVDecWriteVLD(u4VDecID, RW_VLD_SCL_ADDR, 0x100 + u4Idx + (i << 2));
        }
    }
#endif
}


// *********************************************************************
// Function : void Reordering(void)
// Description : 
// Parameter : 
// Return    : 
// *********************************************************************
BOOL _VDEC_AVCReordering(UINT32 u4VDecID)
{
    UINT32 u4Cnt, u4RegData;
  
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_RPL_REORD, 1);
    u4Cnt = 0;
    while(1)
    {
        //if(u4Cnt == 100)
        {
            if((u4RegData = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_RPL_REORD)) > 0)
            {
                if((u4RegData & 0x2) > 0)
                {
                    LOG(7, "Error! _VDEC_AVCReordering timeout!\n");
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else if(u4Cnt == 100)
            {
                LOG(7, "\nError! _VDEC_AVCReordering not ready!!!!!!!!!!\n");
            #ifdef AVC_HW_TIMER
                //wait until timeout
                UNUSED(x_sema_lock_timeout(_hHWParseSema, 5) != OSR_OK);
            #else
                return FALSE;
            #endif
            }
            else if(u4Cnt == 101)
            {
                //wait timer timeout (5ms) and still not ready
                return FALSE;
            }
        }
        //else
        {
            u4Cnt ++;
        }
    }
    //return FALSE;
}


// *********************************************************************
// Function : void _VDEC_PredWeightTable(void)
// Description : 
// Parameter : 
// Return    : 
// *********************************************************************
BOOL _VDEC_PredWeightTable(UINT32 u4VDecID)
{
    UINT32 u4Cnt, u4RegData;
    
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_WEIGHT_PRED_TBL, 1);
    u4Cnt = 0;
    while(1)
    {
        //if(u4Cnt == 100)
        {
            if((u4RegData = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_WEIGHT_PRED_TBL)) > 0)
            {
                if((u4RegData & 0x2) > 0)
                {
                    LOG(2, "Error! _VDEC_PredWeightTable timeout!\n");
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else if(u4Cnt == 100)
            {
                LOG(2, "\nError! _VDEC_PredWeightTable not ready!!!!!!!!!!\n");
            #ifdef AVC_HW_TIMER
                //wait until timeout
                UNUSED(x_sema_lock_timeout(_hHWParseSema, 5) != OSR_OK);
            #else
                return FALSE;
            #endif
            }
            else if(u4Cnt == 101)
            {
                //wait timer timeout (5ms) and still not ready
                return FALSE;
            }
        }
        //else
        {
            u4Cnt ++;
        }
    }
    //return FALSE;
}


// *********************************************************************
// Function : DWRD dwAVCVLDShiftBits(void)
// Description : Get AVCVLD shift bits %64
// Parameter : None
// Return    : VLD Sum
// *********************************************************************
UINT32 _VDEC_AVCVLDShiftBits(UINT32 u4VDecID)
{
    #ifdef BARREL2_SUPPORT  
    if(_u1BarrelShifterNum == BARREL2)
    {
        return (u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_CTRL) & 0x3F);
    }
    else
    #endif
    {
        return ((u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) >> 16) & 0x3F);
    }
    
}


// *********************************************************************
// Function : DWRD dwVldRPtr(void)
// Description : Get VLD Current Dram pointer
// Parameter : None
// Return    : VLD Sum
// *********************************************************************
UINT32 _VDEC_VldRPtr(UINT32 u4VDecID)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
    UINT32 u4Bits, u4VStart, u4VEnd;
    UINT32 u4Cnt = 0;

    #ifdef WAIT_SRAM_STABLE
    #ifdef IS_POST_MT5368
    if(_u1BarrelShifterNum == BARREL2)
    {
        if(u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & PROCESS_FLAG)
        {
            while((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & S_BITS_PROC_NOP)==0)
            {
                u4Cnt++;
                if(u4Cnt >= WAIT_THRD)
                {
                    LOG(2, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                        u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_2ND_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VRPTR),
                        u4VDecReadVLD(u4VDecID, RO_VLD_2ND_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_2ND_FETCHOK));
                    return FALSE;
                }    
            }        
        }
    }
    else
    {
        if(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
        {
            while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & S_BITS_PROC_NOP)==0)
            {
                u4Cnt++;
                if(u4Cnt >= WAIT_THRD)
                {
                    LOG(2, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                        u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4VDecID, RW_VLD_RPTR), u4VDecReadVLD(u4VDecID, RO_VLD_VRPTR),
                        u4VDecReadVLD(u4VDecID, RO_VLD_VBAR), u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK));
                    return FALSE;
                }    
            }        
        }
    }
    #else

    if(_u1BarrelShifterNum == BARREL2)
    {
        while((u4VDecReadVLD(u4VDecID, RO_VLD_2ND_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(2, "Rptr-WaitSramStable Fail\n");
                break;
            }        
        }
    }
    else
    {
        while((u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(2, "Rptr-WaitSramStable Fail\n");
                break;
            }        
        }
    }
    #endif
    #endif
    
    u4DramRptr = _dVLDDramRp(u4VDecID);
    u4SramRptr = _dVLDSramRp(u4VDecID);
    u4SramWptr = _dVLDSramWp(u4VDecID);
    if(u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
    }

    u4Bits = _VDEC_AVCVLDShiftBits(u4VDecID);

    if(_u1BarrelShifterNum == BARREL2)
    {
        u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 4) * 4)) + (u4Bits / 8);
    }
    else
    {
        #if (defined (VDEC_IS_POST_MT5882))    //#ifdef MEM_PAUSE_SUPPORT
        u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 5) * 4)) + (u4Bits / 8);
        #else
        u4ByteAddr = (u4DramRptr - ((u4SramDataSz + 4) * 4)) + (u4Bits / 8);
        #endif
    }
    #ifdef IS_POST_MT5368
    u4ByteAddr -= 4;
    #endif
    
    //ringbuffer check
    if(_u1BarrelShifterNum == BARREL2)
    {
        u4VStart = u4VDecReadVLD(u4VDecID, RW_VLD_2ND_VSTART)<<6;
        u4VEnd = u4VDecReadVLD(u4VDecID, RW_VLD_2ND_VEND)<<6;
    }
    else
    {
        u4VStart = u4VDecReadVLD(u4VDecID, RW_VLD_VSTART)<<6;
        u4VEnd = u4VDecReadVLD(u4VDecID, RW_VLD_VEND)<<6;    
    }
    if(u4ByteAddr < u4VStart)
    {
        u4ByteAddr = u4VEnd - (u4VStart - u4ByteAddr);
    }
    if(u4ByteAddr > u4VEnd)
    {
        u4ByteAddr = u4VStart + (u4ByteAddr - u4VEnd);
    }
    
    u4Bits &= 0x7;

    return u4ByteAddr;
}


void _VDEC_SetVldWPtr(UINT32 u4VDecID, UINT32 u4WPtr, UINT32 u4VStart, UINT32 u4VEnd)
{
    UINT32 u4WPtrAlign;

    u4WPtrAlign = ((u4WPtr + RPTR_ALIGN - 1) & (~((UINT32)RPTR_ALIGN - 1))) + WPTR_OFFSET;
    /*if(_u1BarrelShifterNum == BARREL2)
    {
        u4VStart = VLD_READ32(RW_VLD_2ND_VSTART)<<6;
        u4VEnd = VLD_READ32(RW_VLD_2ND_VEND)<<6;
    }
    else
    {
        u4VStart = VLD_READ32(RW_VLD_VSTART)<<6;
        u4VEnd = VLD_READ32(RW_VLD_VEND)<<6;    
    }*/
    if(u4WPtrAlign < u4VStart)
    {
        u4WPtrAlign = u4VEnd - (u4VStart - u4WPtrAlign);
    }
    if(u4WPtrAlign > u4VEnd)
    {
        u4WPtrAlign = u4VStart + (u4WPtrAlign - u4VEnd);
    }
    #ifdef CC_DRIVER_DEMO
    if(!_bVdecLogSimPatAVC)
    #endif
    {
        #ifndef IS_POST_MT5368
        vVDecWriteVLD(u4VDecID, RW_VLD_ASYNC, u4VDecReadVLD(u4VDecID, RW_VLD_ASYNC) | WPTR_SETBY_RISC_EN);
        #endif       
        #if defined(CC_MT5360B) || defined(CC_MT5392B) || defined (IS_POST_MT5363)
        vVDecWriteVLD(u4VDecID, WO_VLD_WPTR, u4WPtrAlign);
        #else
        vVDecWriteVLD(u4VDecID, WO_VLD_WPTR, (u4WPtrAlign << WPTR_LSH_BITS) | (VLD_EWPL | VLD_RISC_WR_EN));    //set wptr infinity
        #endif
    }    
}


// *********************************************************************
// Function    : void vVDecSetPRefPicListReg(void)
// Description : Set B Ref Pic List related registers
// Parameter   : None
// Return      : None
// Note  :JM decode_POC
// *********************************************************************
void _VDEC_SetPRefPicListReg(UINT32 u4VDecID, VDEC_REF_PIC_LIST_T rRefPicList, VDEC_FBUF_INFO_T* prFBufInfoPtr, 
    UINT32 u4FBufInfo, UINT32 u4ListIdx)
{
    UINT8 u1FBufIdx, u1FBufIdxReg;
    UINT8 u1Fld;  
    UINT8 u1RefPicIdx;  
    UINT8 u1RegIdx;
    UINT32 u4Temp;
    BOOL fgLRefPic;
    VDEC_FBUF_INFO_T* prFBufInfo;
    UINT8 u1ViewIdx;
    BOOL fgIsMVC;
    BOOL fgIsInterView;
    
    ASSERT(prFBufInfoPtr!=NULL);
    
    u1Fld = (UINT8)(u4FBufInfo & 0xff);  
    u1RefPicIdx = (UINT8)((u4FBufInfo >> 8) & 0xff);  
    u1RegIdx = (UINT8)((u4FBufInfo >> 16) & 0xff);  

    if(u1RefPicIdx >= 32)
    {
        ASSERT(u1RefPicIdx < 32);   //size of au4FBufIdx
        return;
    }
    u1FBufIdx = (UINT8)(rRefPicList.au4FBufIdx[u1RefPicIdx] & 0xFF);
    u1ViewIdx = (UINT8)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 8) & 0xFF);
    fgIsInterView = (BOOL)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 16) & 0x1);
    fgIsMVC = (BOOL)(rRefPicList.au4FBufIdx[u1RefPicIdx] >> 24);
    prFBufInfo = &prFBufInfoPtr[u1ViewIdx * MAX_FRAME_NUM];
    
    fgLRefPic = ((u4ListIdx > 3) && (!fgIsInterView))? TRUE: FALSE;
    
    //set RW_AMV_P_REF_PARA
    if(fgIsInterView)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else if(u1ViewIdx > 0)
    {
        u1FBufIdxReg = (u1FBufIdx << 1) + 1;
    }
    else if(fgIsMVC)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else
    {
        u1FBufIdxReg = u1FBufIdx;
    }
    if(u1Fld == FRAME)
    {    
        //bRegIdx = bRefPicIdx;
        if (u1RegIdx == 0)
        {
            prFBufInfo[u1FBufIdx].u4PList0Reg = prFBufInfo[u1FBufIdx].u4YAddr;
        }
        #if (defined (CC_MT5890))
        if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
        {
            vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
            vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
        }
        #endif
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4YAddr);
        #if (defined (VDEC_IS_POST_MT5882))
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
        #endif
        if(fgIsMVC)
        {
            if(fgIsInterView)
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));        
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (fgLRefPic? 
                                (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));        
            }
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + (fgLRefPic? 
                            (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));        
        }
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= 
                        prFBufInfo[u1FBufIdx].i4BFldPOC) && 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);    
        u4Temp |= (fgLRefPic << 20);    
        u4Temp |= ((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara;
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx<<3)), u4Temp);

        #ifdef REF_TRACE
        LOG(2, "PList RegIdx=%d, YAddr=0x%x, (-), FrmNum(%d), LongTermIdx(%d), POC(%d), 0x%x\n", 
        u1RegIdx, PHYSICAL(prFBufInfo[u1FBufIdx].u4YAddr), prFBufInfo[u1FBufIdx].u4FrameNum, 
        prFBufInfo[u1FBufIdx].u4LongTermFrameIdx, prFBufInfo[u1FBufIdx].i4POC, u4Temp);
        #endif        
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= 
                        prFBufInfo[u1FBufIdx].i4BFldPOC) && 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);    
        u4Temp |= (fgLRefPic << 20);    
        u4Temp |= (UINT32)((((((UINT32)u1FBufIdxReg)<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx<<3) + 4), u4Temp);
    }
    else
    { 
        if(u1Fld == TOP_FIELD)
        {
            if (u1RegIdx == 0)
            {
                prFBufInfo[u1FBufIdx].u4PList0Reg = prFBufInfo[u1FBufIdx].u4YAddr;
            }
            #if (defined (CC_MT5890))
            if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
            {
                vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
            }
            #endif
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);            
            #if (defined (VDEC_IS_POST_MT5882))
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
            #endif
            if(fgIsMVC)
            {
                if(fgIsInterView)
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (fgLRefPic? 
                                (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                }
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + (fgLRefPic? 
                            (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
            }      
            //dwTemp = (((_ptFBufInfo[bFBufIdx].iTFldPOC <= _ptFBufInfo[bFBufIdx].iBFldPOC) && (_ptFBufInfo[bFBufIdx].iTFldPOC != 0x7fffffff))? 1: 0) << 21;    
            u4Temp = (fgLRefPic << 20);      
            u4Temp |= ((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara;
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx<<2)), u4Temp);            
            
            #ifdef REF_TRACE
            LOG(2, "PList RegIdx=%d, YAddr=0x%x, (-), FrmNum(%d), LongTermIdx(%d), POC(%d), 0x%x\n", 
            u1RegIdx, PHYSICAL(prFBufInfo[u1FBufIdx].u4YAddr), prFBufInfo[u1FBufIdx].u4FrameNum, 
            prFBufInfo[u1FBufIdx].u4LongTermFrameIdx, prFBufInfo[u1FBufIdx].i4POC, u4Temp);
            #endif

        }
        else
        {
            if (u1RegIdx == 0)
            {
                prFBufInfo[u1FBufIdx].u4PList0Reg = prFBufInfo[u1FBufIdx].u4YAddr;
            }
            #if (defined (CC_MT5890))
            if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
            {
                vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
            }
            #endif
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);          
            #if (defined (VDEC_IS_POST_MT5882))
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
            #endif
            if(fgIsMVC)
            {
                if(fgIsInterView)
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) + (fgLRefPic? 
                                (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                }
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_P0_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + (fgLRefPic? 
                            (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
            }      
            //dwTemp = (((_ptFBufInfo[bFBufIdx].iTFldPOC <= _ptFBufInfo[bFBufIdx].iBFldPOC) && (_ptFBufInfo[bFBufIdx].iTFldPOC != 0x7fffffff))? 1: 0) << 21;    
            u4Temp = (fgLRefPic << 20);      
            u4Temp |= ((((UINT32)u1FBufIdxReg<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara;
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx<<2)), u4Temp);
      
            vVDecWriteMC(u4VDecID, RW_MC_P_LIST0_FLD, 
                (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_P_LIST0_FLD) | (0x1 << u1RegIdx)));
            #ifdef REF_TRACE
            LOG(2, "PList RegIdx=%d, YAddr=0x%x, (-), FrmNum(%d), LongTermIdx(%d), POC(%d), 0x%x\n", 
            u1RegIdx, PHYSICAL(prFBufInfo[u1FBufIdx].u4YAddr), prFBufInfo[u1FBufIdx].u4FrameNum, 
            prFBufInfo[u1FBufIdx].u4LongTermFrameIdx, prFBufInfo[u1FBufIdx].i4POC, u4Temp);
            #endif
        }     
    }
}


// *********************************************************************
// Function    : void vVDecSetBRefPicListReg(void)
// Description : Set B Ref Pic List related registers
// Parameter   : None
// Return      : None
// Note  :JM decode_POC
// *********************************************************************
BOOL _VDEC_LAE_SetBRefPicListReg(UINT32 u4VDecID, VDEC_REF_PIC_LIST_T rRefPicList, VDEC_REF_PIC_LIST_T rRefPicList1, 
    VDEC_FBUF_INFO_T* prFBufInfoPtr, UINT32 u4FBufInfo, UINT32 u4ListIdx)
{
    UINT8 u1FBufIdx, u1FBufIdxReg;
    UINT8 u1Fld;  
    UINT8 u1RefPicIdx;  
    UINT8 u1RegIdx;
    UINT32 u4Temp;
    BOOL fgIsMVC;
    BOOL fgLRefPic, fgInterViewNonRef; 
    const VDEC_FBUF_INFO_T* prFBufInfo;
    UINT8 u1ViewIdx;
    BOOL fgIsInterView;
    
    ASSERT(prFBufInfoPtr!=NULL);

    u1Fld = (UINT8)(u4FBufInfo & 0xff);  
    u1RefPicIdx = (UINT8)((u4FBufInfo >> 8) & 0xff);  
    u1RegIdx = (UINT8)((u4FBufInfo >> 16) & 0xff);     

    if(u1RefPicIdx >= 32)
    {
        ASSERT(u1RefPicIdx < 32);   //size of au4FBufIdx
        return FALSE;
    }
    u1FBufIdx = (UINT8)(rRefPicList.au4FBufIdx[u1RefPicIdx] & 0xFF);
    u1ViewIdx = (UINT8)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 8) & 0xFF);
    fgIsInterView = (BOOL)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 16) & 0x1);
    fgIsMVC = (BOOL)(rRefPicList.au4FBufIdx[u1RefPicIdx] >> 24);    
    prFBufInfo = &prFBufInfoPtr[u1ViewIdx * MAX_FRAME_NUM];

    fgLRefPic = ((u4ListIdx > 3) && (!fgIsInterView))? TRUE: FALSE;
    fgInterViewNonRef = fgLRefPic;
    
    //set RW_AMV_P_REF_PARA
    if(fgIsInterView)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else if(u1ViewIdx > 0)
    {
        u1FBufIdxReg = (u1FBufIdx << 1) + 1;
    }
    else if(fgIsMVC)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else
    {
        u1FBufIdxReg = u1FBufIdx;
    }

    if (u1Fld == FRAME)
    {
        if(fgIsMVC)
        {
            if(fgIsInterView)
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
            }
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2), 
                (UINT32)((fgLRefPic << 19) + 
                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
        }
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                    && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgLRefPic << 20);
        u4Temp += ((((UINT32)u1FBufIdxReg) << 1) << 22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                    (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff);
        B0List[(u1RegIdx << 3)] = u4Temp;
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgLRefPic << 20);
        u4Temp += (((((UINT32)u1FBufIdxReg) << 1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff);
        B0List[(u1RegIdx << 3) + 4] = u4Temp;

        // B_1
        if(u4ListIdx < 4)
        {
            u4ListIdx += 2;
            u1FBufIdx = (UINT8)rRefPicList1.au4FBufIdx[u1RefPicIdx];   
        }
        else     // long term
        {
            u1FBufIdx = (UINT8)rRefPicList.au4FBufIdx[u1RefPicIdx];
        }
        //set RW_AMV_P_REF_PARA
        if(fgIsInterView)
        {
            u1FBufIdxReg = u1FBufIdx << 1;
        }
        else if(u1ViewIdx > 0)
        {
            u1FBufIdxReg = (u1FBufIdx << 1) + 1;
        }
        else if(fgIsMVC)
        {
            u1FBufIdxReg = u1FBufIdx << 1;
        }
        else
        {
            u1FBufIdxReg = u1FBufIdx;
        }
        if(fgIsMVC)
        {
            if(fgIsInterView)
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
            }
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
        }
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
        u4Temp += ((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff);
        B1List[(u1RegIdx << 3)] = u4Temp;
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
        u4Temp += (((((UINT32)u1FBufIdxReg)<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff);
        B1List[(u1RegIdx << 3) + 4] = u4Temp;
    }
    else
    {
        // B_0
        if((u4ListIdx == 0) || (u4ListIdx == 1) || (u4ListIdx == 4) || (u4ListIdx == 5))
        {    
            if (u1Fld == TOP_FIELD)
            {  
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                    }
                }
                else
                {                    
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                        (UINT32)((fgLRefPic << 19) + 
                                        (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                        (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                }
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff));
                u4Temp |= (fgLRefPic << 20);
                B0List[(u1RegIdx << 2)] = u4Temp;
            }
            else
            {    
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                        (UINT32)((fgLRefPic << 19) + 
                                        (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                        (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                }
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff));
                u4Temp |= (fgLRefPic << 20);
                B0List[(u1RegIdx << 2)] = u4Temp;      
            }
        }
        // B_1  
        if((u4ListIdx == 2) || (u4ListIdx == 3) || (u4ListIdx == 4) || (u4ListIdx == 5))
        {    
            if (u1Fld == TOP_FIELD)
            {
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));                        
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                            (UINT32)((fgLRefPic << 19) + 
                                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                            (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                }
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg <<1 )<<22) + prFBufInfo[u1FBufIdx].u4TFldPara) + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff));
                u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
                B1List[(u1RegIdx << 2)] = u4Temp;
            }
            else
            {     
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                            (UINT32)((fgLRefPic << 19) + 
                                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                            (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                }
                u4Temp = (UINT32)((((((UINT32)u1FBufIdxReg<<1) + 1) << 22) + prFBufInfo[u1FBufIdx].u4BFldPara) + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff));
                u4Temp |= (fgInterViewNonRef << 20);
                B1List[(u1RegIdx <<2 )] = u4Temp;
            }
        }
    }
    if (B0List[(u1RegIdx<<2)] != B1List[(u1RegIdx<<2)])
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




// *********************************************************************
// Function    : void vVDecSetBRefPicListReg(void)
// Description : Set B Ref Pic List related registers
// Parameter   : None
// Return      : None
// Note  :JM decode_POC
// *********************************************************************
BOOL _VDEC_SetBRefPicListReg(UINT32 u4VDecID, VDEC_REF_PIC_LIST_T rRefPicList, VDEC_REF_PIC_LIST_T rRefPicList1, 
    VDEC_FBUF_INFO_T* prFBufInfoPtr, UINT32 u4FBufInfo, UINT32 u4ListIdx)
{
    UINT8 u1FBufIdx, u1FBufIdxReg;
    UINT8 u1Fld;  
    UINT8 u1RefPicIdx;  
    UINT8 u1RegIdx;
    UINT32 u4Temp;
    BOOL fgLRefPic, fgInterViewNonRef; 
    const VDEC_FBUF_INFO_T* prFBufInfo;
    UINT8 u1ViewIdx;
    BOOL fgIsMVC;
    BOOL fgIsInterView;
    
    ASSERT(prFBufInfoPtr!=NULL);

    u1Fld = (UINT8)(u4FBufInfo & 0xff);  
    u1RefPicIdx = (UINT8)((u4FBufInfo >> 8) & 0xff);  
    u1RegIdx = (UINT8)((u4FBufInfo >> 16) & 0xff);  
    
    if(u1RefPicIdx >= 32)
    {
        ASSERT(u1RefPicIdx < 32);   //size of au4FBufIdx
        return FALSE;
    }
    u1FBufIdx = (UINT8)(rRefPicList.au4FBufIdx[u1RefPicIdx] & 0xFF);
    u1ViewIdx = (UINT8)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 8) & 0xFF);
    fgIsInterView = (BOOL)((rRefPicList.au4FBufIdx[u1RefPicIdx] >> 16) & 0x1);
    fgIsMVC = (BOOL)(rRefPicList.au4FBufIdx[u1RefPicIdx] >> 24);    
    prFBufInfo = &prFBufInfoPtr[u1ViewIdx * MAX_FRAME_NUM];
    
    fgLRefPic = ((u4ListIdx > 3) && (!fgIsInterView))? TRUE: FALSE;
    fgInterViewNonRef = fgLRefPic;
    
    //set RW_AMV_P_REF_PARA
    if(fgIsInterView)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else if(u1ViewIdx > 0)
    {
        u1FBufIdxReg = (u1FBufIdx << 1) + 1;
    }
    else if(fgIsMVC)
    {
        u1FBufIdxReg = u1FBufIdx << 1;
    }
    else
    {
        u1FBufIdxReg = u1FBufIdx;
    }

    if(u1Fld == FRAME)
    {
        // B_0        
        #if (defined (CC_MT5890))
        if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
        {
            vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
            vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
        }
        #endif
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);
        #if (defined (VDEC_IS_POST_MT5882))
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
        #endif
        #ifdef REF_TRACE
        LOG(2, "BList0 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
        #endif
        
        if(fgIsMVC)
        {
            if(fgIsInterView)
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
            }
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
        }
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgLRefPic << 20);
        u4Temp += ((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx<<3)), u4Temp);

        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgLRefPic << 20);
        u4Temp += (((((UINT32)u1FBufIdxReg)<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx<<3) + 4), u4Temp);

        //B_1
        if(u4ListIdx < 4)
        {
            u4ListIdx += 2;
            u1FBufIdx = (UINT8)rRefPicList1.au4FBufIdx[u1RefPicIdx];   
        }
        else     // long term
        {
            u1FBufIdx = (UINT8)rRefPicList.au4FBufIdx[u1RefPicIdx];
        }
        //set RW_AMV_P_REF_PARA
        if(fgIsInterView)
        {
            u1FBufIdxReg = u1FBufIdx << 1;
        }
        else if(u1ViewIdx > 0)
        {
            u1FBufIdxReg = (u1FBufIdx << 1) + 1;
        }
        else if(fgIsMVC)
        {
            u1FBufIdxReg = u1FBufIdx << 1;
        }
        else
        {
            u1FBufIdxReg = u1FBufIdx;
        }       
    #if (defined (CC_MT5890))
        if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
        {
            vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
            vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
        }
    #endif
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);      
        #if (defined (VDEC_IS_POST_MT5882))
        vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
        #endif
        #ifdef REF_TRACE
        LOG(2, "BList1 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
        #endif
        
        if(fgIsMVC)
        {
            if(fgIsInterView)
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
            }
            else
            {
                vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
            }
        }
        else
        {
            vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4LongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4PicNum & 0x7ffff))));
        }      
        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
        u4Temp += ((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx<<3)), u4Temp);

        u4Temp = (UINT32)((((prFBufInfo[u1FBufIdx].i4TFldPOC <= prFBufInfo[u1FBufIdx].i4BFldPOC) 
                        && (prFBufInfo[u1FBufIdx].i4TFldPOC != 0x7fffffff))? 1: 0) << 21);
        u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
        u4Temp += (((((UINT32)u1FBufIdxReg)<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx<<3) + 4), u4Temp);

        u4Temp = PHYSICAL(prFBufInfo[u1FBufIdx].u4MVBufAddr);
        vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<3)), (UINT32)(u4Temp >> 4));
        #ifdef AVC_MV_BUF_REDUCE
        if(_bIsMVReduce)
        {
            #ifdef IS_POST_MT5368
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<3) + 4), 
                (UINT32)((u4Temp >> 4) + 2)); 
            #else
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<3) + 4), 
                (UINT32)((u4Temp >> 4) + 1)); 
            #endif
        }
        else
        #endif
        {
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<3) + 4), 
            (UINT32)((u4Temp >> 4) + 4));
        }
    }
    else
    {
        // B_0
        if((u4ListIdx == 0) || (u4ListIdx == 1) || (u4ListIdx == 4) || (u4ListIdx == 5))
        {      
            //u4Temp = (((_ptFBufInfo[bFBufIdx].iTFldPOC <= _ptFBufInfo[bFBufIdx].iBFldPOC) && (_ptFBufInfo[bFBufIdx].iTFldPOC != 0x7fffffff))? 1: 0) << 21;
            if(u1Fld == TOP_FIELD)
            {            
                #if (defined (CC_MT5890))
                if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
                {
                    vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                    vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
                }
                #endif
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);                
                #if (defined (VDEC_IS_POST_MT5882))
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
                #endif
                #ifdef REF_TRACE
                LOG(2, "BList0 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
                #endif
                
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                }        
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg)<<1)<<22) + prFBufInfo[u1FBufIdx].u4TFldPara + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff));
                u4Temp |= (fgLRefPic << 20);
                vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx<<2)), u4Temp);
            }
            else
            {                    
                #if (defined (CC_MT5890))
                if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
                {
                    vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                    vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
                }
                #endif
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);               
                #if (defined (VDEC_IS_POST_MT5882))
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
                #endif
                #ifdef REF_TRACE
                LOG(2, "BList0 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
                #endif
                
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B0_RPL + (u1RegIdx<<2)), 
                        (UINT32)((fgLRefPic << 19) + 
                        (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                        (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                }        
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg<<1) + 1)<<22) + prFBufInfo[u1FBufIdx].u4BFldPara + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff));
                u4Temp |= (fgLRefPic << 20);
                vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx<<2)), u4Temp);
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_FLD, 
                    (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_B_LIST0_FLD) | (0x1 << u1RegIdx)));        
            }
        }
        // B_1    
        if((u4ListIdx == 2) || (u4ListIdx == 3) || (u4ListIdx == 4) || (u4ListIdx == 5))
        {      
            //u4Temp = (((_ptFBufInfo[bFBufIdx].iTFldPOC <= _ptFBufInfo[bFBufIdx].iBFldPOC) && (_ptFBufInfo[bFBufIdx].iTFldPOC != 0x7fffffff))? 1: 0) << 21;
            if(u1Fld == TOP_FIELD)
            {
                #if (defined (CC_MT5890))
                if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
                {
                    vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                    vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
                }
                #endif
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);
                #if (defined (VDEC_IS_POST_MT5882))
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
                #endif
                #ifdef REF_TRACE
                LOG(2, "BList1 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
                #endif
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));                        
                    }
                    else
                    {                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4TFldLongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4TFldPicNum & 0x7ffff))));
                }
                u4Temp = PHYSICAL(prFBufInfo[u1FBufIdx].u4MVBufAddr);
                vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<2)), 
                    (UINT32)(u4Temp >> 4));      
        
                u4Temp = (UINT32)(((((UINT32)u1FBufIdxReg <<1 )<<22) + prFBufInfo[u1FBufIdx].u4TFldPara) + 
                        (prFBufInfo[u1FBufIdx].i4TFldPOC & 0x3ffff));
                u4Temp |= (fgInterViewNonRef << 20);//(fgLRefPic << 20);
                vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx<<2)), u4Temp);
            }
            else
            {
                #if (defined (CC_MT5890))
                if (prFBufInfo[u1FBufIdx].fgIsUFOMode)
                {
                    vVDecWriteMC(u4VDecID, RW_MC_Y_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4YLenOffset);
                    vVDecWriteMC(u4VDecID, RW_MC_C_LEN_OFFSET, prFBufInfo[u1FBufIdx].u4CLenOffset);
                }
                #endif
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1RegIdx<<2)), prFBufInfo[u1FBufIdx].u4YAddr);
                #if (defined (VDEC_IS_POST_MT5882))
                vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1_CADDR + (u1RegIdx <<2)), prFBufInfo[u1FBufIdx].u4CAddr);
                #endif
                #ifdef REF_TRACE
                LOG(2, "BList1 RegIdx=%d, YAddr=0x%x\n", u1RegIdx, prFBufInfo[u1FBufIdx].u4YAddr);
                #endif
                if(fgIsMVC)
                {
                    if(fgIsInterView)
                    {
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (prFBufInfo[u1FBufIdx].u4ViewIDFb & 0x7FFFF)));
                    }
                    else
                    {                                
                        vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                                (UINT32)((fgIsInterView << 21) + (fgLRefPic << 20) + (1 << 19) +
                                (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                                (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                    }
                }
                else
                {
                    vVDecWriteAVCVLD(u4VDecID, (UINT32)(RW_AVLD_REORD_B1_RPL + (u1RegIdx<<2)), 
                            (UINT32)((fgLRefPic << 19) + 
                            (fgLRefPic? (prFBufInfo[u1FBufIdx].i4BFldLongTermPicNum & 0x7ffff) : 
                            (prFBufInfo[u1FBufIdx].i4BFldPicNum & 0x7ffff))));
                }
                u4Temp = PHYSICAL(prFBufInfo[u1FBufIdx].u4MVBufAddr);
                #ifdef AVC_MV_BUF_REDUCE
                if(_bIsMVReduce)
                {
                    #ifdef IS_POST_MT5368
                    vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<2)), 
                        (UINT32)((u4Temp >> 4) + 2));
                    #else
                    vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<2)), 
                        (UINT32)((u4Temp >> 4) + 1));
                    #endif
                }
                else
                #endif
                {
                    vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_MV_BUF_ADDR + (u1RegIdx<<2)), 
                    (UINT32)((u4Temp >> 4) + 4));
                }
                u4Temp = (UINT32)((((((UINT32)u1FBufIdxReg<<1) + 1) << 22) + prFBufInfo[u1FBufIdx].u4BFldPara) + 
                        (prFBufInfo[u1FBufIdx].i4BFldPOC & 0x3ffff));
                u4Temp |= (fgInterViewNonRef << 20);
                vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx<<2)), u4Temp);

                vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_FLD, 
                    (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD) | (0x1 << u1RegIdx)));        
            }
        }
    }
    if (u4VDecReadAVCMV(u4VDecID, RW_AMV_B0_REF_PARA + (u1RegIdx<<2)) != u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + (u1RegIdx<<2)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#ifdef TEST_ONOFF_RUNTIME
static void  CheckLockStatus(void)
{
    VDEC_INFO_T *prVdecInfo;
    prVdecInfo = _VDEC_GetInfo();
    if(prVdecInfo->arVdecVldInfo[0].u4LockCount == 0)
    {
        LOG(0, "FixMe, can't access VLD/MC register before lock vld\n");
        ASSERT(0);
    }
    if(!prVdecInfo->arVdecVldInfo[0].fgPowerOn)
    {
        LOG(0, "FIXME: can't access VDEC register before power on\n");
        ASSERT(0);
    }
}
#endif

/*UINT32 _VDEC_IO_Read32(UINT32 u4Base, UINT32 u4Offset)
{
    #ifdef TEST_ONOFF_RUNTIME
    CheckLockStatus();
    #endif
    #ifdef CC_DRIVER_DEMO
    if(_bVdecLogSimPatAVC)
    {
        switch(u4Base)
        {
            case VLD_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_VLD + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);
                break;
            case MC_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_MC + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            case AVC_VLD_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_AVC_VLD + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            case AVC_MV_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_AVC_MV + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            case VLD_TOP_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_VLD_TOP + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            case PP_BASE:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_PP + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            case VDEC_MISC:
                LOG(0, "RISCRead(`VDEC_FULL_ADDR_MISC + 4*%d, 32'hx);\n", (u4Offset & 0xFFF)>>2);            
                break;
            default:
                break;
        }
        if((u4Offset != RW_AVLD_CTRL) &&
            (u4Offset != RO_AVLD_COMPLETE) &&
            (u4Offset != RO_AVLD_ERR_MESSAGE))
        {
            x_thread_delay(100);
        }
    }
    #endif
    return IO_READ32(u4Base, u4Offset);
}

void _VDEC_IO_Write32(UINT32 u4Base, UINT32 u4Offset, UINT32 u4Value)
{
    #ifdef TEST_ONOFF_RUNTIME
    CheckLockStatus();
    #endif
    #ifdef CC_DRIVER_DEMO
    if(_bVdecLogSimPatAVC)
    {
        switch(u4Base)
        {
            case VLD_BASE:
                if((u4Offset==RW_VLD_RPTR) || (u4Offset==RW_VLD_2ND_RPTR))
                {
                    LOG(0, "RISCWrite(`VDEC_FULL_ADDR_VLD + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, 
                        u4Value - (IO_READ32(u4Base, RW_VLD_VSTART) << 6));
                }
                else
                {
                    LOG(0, "RISCWrite(`VDEC_FULL_ADDR_VLD + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);
                    if((u4Offset==RW_VLD_PROC) || (u4Offset==RW_VLD_2ND_PROC))
                    {
                        LOG(0, "#200000;\n");
                    }
                }
                break;
            case MC_BASE:
                if(u4Offset==RW_MC_PP_Y_ADDR)
                {
                    LOG(0, "RISCWrite(`VDEC_FULL_ADDR_MC + 4*137, PP_OUT_Y_ADDR);\n");
                }
                else if(u4Offset==RW_MC_PP_C_ADDR)
                {
                    LOG(0, "RISCWrite(`VDEC_FULL_ADDR_MC + 4*138, PP_OUT_C_ADDR);\n");
                }
                else
                {
                    LOG(0, "RISCWrite(`VDEC_FULL_ADDR_MC + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);            
                }
                break;
            case AVC_VLD_BASE:
                LOG(0, "RISCWrite(`VDEC_FULL_ADDR_AVC_VLD + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);
                if(u4Offset==RW_AVLD_PROC)
                {
                    LOG(0, "//==============Finish simulation pattern===========\n");
                }
                break;
            case AVC_MV_BASE:
                LOG(0, "RISCWrite(`VDEC_FULL_ADDR_AVC_MV + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);            
                break;
            case VLD_TOP_BASE:
                LOG(0, "RISCWrite(`VDEC_FULL_ADDR_VLD_TOP + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);            
                break;
            case PP_BASE:
                LOG(0, "RISCWrite(`VDEC_FULL_ADDR_PP + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);            
                break;
            case VDEC_MISC:
                LOG(0, "RISCWrite(`VDEC_FULL_ADDR_MISC + 4*%d, 32'h%x);\n", (u4Offset & 0xFFF)>>2, u4Value);            
                break;
            default:
                break;
        }
        //cannot use delay in ISR
        if((u4Offset != RW_AVLD_CTRL) &&
            (u4Offset != RW_AVLD_COMPLETE_CLR) &&
            (u4Offset != RW_AVLD_TIMEOUT_CLR))
        {
            x_thread_delay(100);
        }
    }
    #endif
    IO_WRITE32(u4Base, u4Offset, u4Value);
}*/

void _VDEC_LogSimPat(UINT32 u4VDecID, BOOL bLogEn)
{
    #ifdef CC_DRIVER_DEMO
    _bVdecLogSimPatAVC = bLogEn;
    #else
    UNUSED(bLogEn);
    #endif
}

#ifdef AVC_MV_BUF_REDUCE
void _VDEC_IsMVReduce(BOOL bEn)
{
    _bIsMVReduce = bEn;
}
#endif


#ifdef MPV_DUMP_H264_DEC_REG
// *********************************************************************
// Function :void VDec_DumpH264Reg(UCHAR ucMpvId)
// Description : Set RW_MC_BREF
// Parameter : fgIVop: indicate the backward reference picture is an I-VOP
// Return    : None
// *********************************************************************
void VDec_DumpH264Reg(UCHAR ucMpvId)
{
    UINT32 i;
    if (VDEC_LAE0 == ucMpvId)
    {
        LOG(0, "===============LAE setting=================\n");
        LOG(0, "avc vld #%d = (0x%x)\n",0,u4VDecReadAVCVLD(2,(0*4)));
        for(i = 33; i < 142; i ++)
        {
            LOG(0, "avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(2,(i*4)));
        }
        for(i = 145; i < 512; i ++)
        {
            LOG(0, "avc vld #%d = (0x%x)\n",i,u4VDecReadAVCVLD(2,(i*4)));
        }
        for(i = 33; i < 256; i ++)
        {
            LOG(0, "vld #%d = (0x%x)\n",i,u4VDecReadVLD(2,(i*4)));
        }
        for (i = 0; i < 256; i++)
        {
            LOG(0, "mcore #%d = (0x%x)\n", i, u4VDecReadMCORETOP(2, (i*4)));
        }
        for (i = 0; i < 128; i++)
        {
            LOG(0, "vld top #%d = (0x%x)\n", i, u4VDecReadVLDTOP(2, (i*4)));
        }
    }
    else
    {
        LOG(0, "===============core0 setting=================\n");
        //LOG(0, "%d, avc vld #%d = (0x%x)\n",ucMpvId, 0,u4VDecReadAVCVLD(ucMpvId,(0*4)));
        for(i = 33; i < 142; i ++)
        {
            LOG(0, "%d, avc vld #%d = (0x%x)\n",ucMpvId, i,u4VDecReadAVCVLD(ucMpvId,(i*4)));
        }
        for(i = 145; i < 512; i ++)
        {
            LOG(0, "%d, avc vld #%d = (0x%x)\n",ucMpvId, i,u4VDecReadAVCVLD(ucMpvId,(i*4)));
        }
        for(i = 33; i < 256; i ++)
        {
            LOG(0, "%d, vld #%d = (0x%x)\n",ucMpvId, i,u4VDecReadVLD(ucMpvId,(i*4)));
        }
        for (i = 0; i < 256; i++)
        {
            LOG(0, "%d, mcore #%d = (0x%x)\n", ucMpvId, i, u4VDecReadMCORETOP(ucMpvId, (i*4)));
        }
        for (i = 0; i < 128; i++)
        {
            LOG(0, "%d, vld top #%d = (0x%x)\n", ucMpvId, i, u4VDecReadVLDTOP(ucMpvId, (i*4)));
        }
        for (i = 0; i < 1024; i++)
        {
            LOG(0, "%d, MC #%d = (0x%x)\n", ucMpvId, i, u4VDecReadMC(ucMpvId, (i*4)));
        }
        for (i = 0; i < 256; i++)
        {
            LOG(0, "%d, MV #%d = (0x%x)\n", ucMpvId, i, u4VDecReadAVCMV(ucMpvId, (i*4)));
        }
        for (i = 0; i < 1024; i++)
        {
            LOG(0, "%d, PP #%d = (0x%x)\n", ucMpvId, i, vVDecReadPP(ucMpvId, (i*4)));
        }
        for (i = 0; i < 128; i++)
        {
            LOG(0, "%d, MISC #%d = (0x%x)\n", ucMpvId, i, u4VDecReadMISC(ucMpvId, (i*4)));
        }
        
        LOG(0, "===============core1 setting=================\n");
        //LOG(0, "%d, avc vld #%d = (0x%x)\n",VDEC_VLD1,0,u4VDecReadAVCVLD(VDEC_VLD1,(0*4)));
        for(i = 33; i < 142; i ++)
        {
            LOG(0, "%d, avc vld #%d = (0x%x)\n",VDEC_VLD1,i,u4VDecReadAVCVLD(VDEC_VLD1,(i*4)));
        }
        for(i = 145; i < 512; i ++)
        {
            LOG(0, "%d, avc vld #%d = (0x%x)\n",VDEC_VLD1,i,u4VDecReadAVCVLD(VDEC_VLD1,(i*4)));
        }
        for(i = 33; i < 256; i ++)
        {
            LOG(0, "%d, vld #%d = (0x%x)\n",VDEC_VLD1,i,u4VDecReadVLD(VDEC_VLD1,(i*4)));
        }
        for (i = 0; i < 256; i++)
        {
            LOG(0, "%d, mcore #%d = (0x%x)\n", VDEC_VLD1,i, u4VDecReadMCORETOP(VDEC_VLD1, (i*4)));
        }
        for (i = 0; i < 128; i++)
        {
            LOG(0, "%d, vld top #%d = (0x%x)\n", VDEC_VLD1,i, u4VDecReadVLDTOP(VDEC_VLD1, (i*4)));
        }
        for (i = 0; i < 1024; i++)
        {
            LOG(0, "%d, MC #%d = (0x%x)\n", VDEC_VLD1,i, u4VDecReadMC(VDEC_VLD1, (i*4)));
        }
        for (i = 0; i < 256; i++)
        {
            LOG(0, "%d, MV #%d = (0x%x)\n", VDEC_VLD1,i, u4VDecReadAVCMV(VDEC_VLD1, (i*4)));
        }
        for (i = 0; i < 1024; i++)
        {
            LOG(0, "%d, PP #%d = (0x%x)\n", VDEC_VLD1,i, vVDecReadPP(VDEC_VLD1, (i*4)));
        }
        for (i = 0; i < 128; i++)
        {
            LOG(0, "%d, MISC #%d = (0x%x)\n", VDEC_VLD1,i, u4VDecReadMISC(VDEC_VLD1, (i*4)));
        }
    }
}
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



