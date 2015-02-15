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
 * $RCSfile: drv_nr_int.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// Inclusions
////////////////////////////////////////////////////////////////////////////////
#define _DRV_NR_INT_C_

#include "vdo_misc.h"
#include "video_def.h"
#include "hw_vdoin.h"
#include "hw_nr.h"
#include "hw_di.h"
#include "hw_di_int.h"
#include "hw_sw.h"
#include "nr_debug.h"
#include "drv_nr.h"
#include "drv_di_int.h"
#include "drv_bnr_int.h"
#include "drv_nr_int.h"

#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"

EXTERN void vDrvDISetInkColor(UINT32 u4Idx, UINT32 u4Color);
EXTERN void vDrvNRSemaUnlock(void);

#define NX_WIDTH 1920
#define NX_HEIGHT 1080

#define NX_PACK_SIZE (1920>>2)
#define NX_VBLK_LS_SIZE (NX_WIDTH/4)
#define NX_BLKPOS_SIZE 120

////////////////////////////////////////////////////////////////////////////////
// Variables for driver
////////////////////////////////////////////////////////////////////////////////

static NR_NM_STA_T _rNmSta;

static UINT32 _u4EnMsk = 0;
static UINT8 _u1Init = 0;
static volatile UINT8 _u1NrProcFlg = 0;
static UINT8 _u1DotCrawlLevel = 0;


// variables for BNR SRAM R/W
static UINT8 _u1LRFreeRunCnt = 0;

// VBLK
static UINT16 _u2VBlkPxCnt   = 0;
static UINT16 _u2VBlkPxCntR  = 0;
static UINT32* _u4VBlkLineSum  = 0;
static UINT32* _u4VBlkLineSumR = 0;

static UINT32 _u4BLKPos[NX_BLKPOS_SIZE] = {0};

#define _SFTX(x) (((x)&0xF)*2)
#define RD_VBLK(x) ((_u4BLKPos[((x)>>4)] >> _SFTX(x)) & 0x3)
#define RD_HBLK(y) 0
#define WR_VBLK(x, v) _u4BLKPos[(x)>>4] = (_u4BLKPos[(x)>>4] & (~(0x3 << _SFTX(x)))) | ((v) << _SFTX(x))
#define WR_HBLK(y, v)

#define V_LS(x) (((*((UINT32*)_u4VBlkLineSum+((x)>>2))) >> (((x) & 0x3)*8)) & 0xFF)
#define H_LS(y) 0
#define V_LS_R(x) (((*((UINT32*)_u4VBlkLineSumR+((x)>>2))) >> (((x) & 0x3)*8)) & 0xFF)
#define H_LS_R(y) 0

////////////////////////////////////////////////////////////////////////////////
// NR utilities
////////////////////////////////////////////////////////////////////////////////
UINT32 u4DrvNRUtilIIR(NR_IIR_T* rIIR, UINT8 u1Res, UINT32 u4Val)
{
#if 1
    u4Val <<= u1Res;
    rIIR->u4Sta = LERP(rIIR->u4Sta, u4Val, rIIR->u1Wei, u1Res);
    return (rIIR->u4Sta + W_ROUND(u1Res)) >> u1Res;
#else
    UINT32 u4StaPre = rIIR->u4Sta;
    UINT32 u4StaNew;
    u4StaNew = LERP(u4StaPre, u4Val, rIIR->u1Wei, 8);
    if ((u4StaNew == u4StaPre) && (u4StaPre != u4Val))
    {
        rIIR->u1Err = MIN((UINT32)255, (UINT32)rIIR->u1Err + 256 - rIIR->u1Wei);
    }
    else
    {
        rIIR->u1Err = 0;
    }
    
    if (rIIR->u1Err >= 128)
    {
        rIIR->u1Err = 0;
        u4StaNew = (u4Val > u4StaNew) ? (u4StaNew+1) :
            ((u4Val < u4StaNew) ? (MAX(u4StaNew,1)-1) : u4StaNew);
    }

    rIIR->u4Sta = u4StaNew;
    return u4StaNew;
#endif
}


////////////////////////////////////////////////////////////////////////////////
// NR drivers
////////////////////////////////////////////////////////////////////////////////
static inline UINT32 _u4GetCurrLineCnt(void)
{
    return NR_R(MCVP_KC_2D, OUT_VDE_CNT);
}

UINT32 u4GetEnMsk(void)
{
    return _u4EnMsk;
}

static inline UINT32 *_pu4VBlkLSPtr(void)
{
    return ((_u1LRFreeRunCnt) ? _u4VBlkLineSumR : _u4VBlkLineSum);
}

UINT16 u2DrvNXNRGetVBlkPxCnt(BOOL fgR)
{
    return fgR ? _u2VBlkPxCntR : _u2VBlkPxCnt;
}

UINT16 u2DrvNXNRGetVBlkLineSum(BOOL fgR, UINT32 u4Pos)
{
    u4Pos = MIN(u4Pos, 1920);
    return fgR ? V_LS_R(u4Pos) : V_LS(u4Pos);
}

UINT16 u2DrvNXNRGetHBlkLineSum(BOOL fgR, UINT32 u4Pos)
{
#ifndef CC_MT5881
    u4Pos = MIN(u4Pos, 1080);
    return fgR ? H_LS_R(u4Pos) : H_LS(u4Pos);
#else
    return 0;
#endif
}

void vDrvNXNRResetPos(void)
{
    x_memset(_u4BLKPos, 0, sizeof(UINT32)*NX_BLKPOS_SIZE);
}

void vDrvNXNRWritePos(BOOL u1IsH, UINT32 u4Pos, UINT8 u1OnOff)
{
    if (u1IsH == 0)
    {
        WR_VBLK(MIN(u4Pos, 1919), (u1OnOff?3:0));
    }
    else
    {
        WR_HBLK(MIN(u4Pos, 1079), (u1OnOff?3:0));
    }
}

void vDrvNXNRWritePosLR(BOOL u1IsR, UINT32 u4Pos, UINT8 u1OnOff)
{
    UINT32 u4OnOff;
    UINT32 u4Msk;

    u4Pos = MIN(u4Pos, 1919);
    u4OnOff = RD_VBLK(u4Pos);
    u4Msk = u1IsR ? 2 : 1;
    u4OnOff = (u4OnOff & (~u4Msk)) | (u1OnOff?u4Msk:0);

    WR_VBLK(u4Pos, u4OnOff);
}

UINT8 u1DrvNXNRReadPos(BOOL u1IsH, UINT32 u4Pos)
{
    if (u1IsH == 0)
    {
        return RD_VBLK(MIN(u4Pos, 1919));
    }
    else
    {
        return RD_HBLK(MIN(u4Pos, 1079));
    }
}

void vDrvNXNRCopyLineSumTool(void)
{
    static UINT8 _u1Cnt = 0;

    _u1Cnt = _SWR(NR_CTRL_00, EN_TRIG_COPY_LS) && !_SWR(NR_BLK_STA_03, NR_BLK_READ_FAIL) ? _u1Cnt + 1 : 0;

    if (_u1Cnt >= 6)
    {
        x_memcpy((UINT32*)NR_VBLK_START, _SWR(NR_FWBNR_00, NR_FWBNR_READ_RIGHT) ? _u4VBlkLineSumR : _u4VBlkLineSum, sizeof(UINT32)*NX_VBLK_LS_SIZE);
	#ifndef CC_MT5881
        x_memcpy((UINT32*)NR_HBLK_START, _SWR(NR_FWBNR_00, NR_FWBNR_READ_RIGHT) ? _u4HBlkLineSumR : _u4HBlkLineSum, sizeof(UINT32)*NX_HBLK_LS_SIZE);    
	#endif
        _SWW(NR_CTRL_00, 0, EN_TRIG_COPY_LS);
    }
}


void vDrvMCNRSetWeiTbl(E_3DNR_IIR_TBL eIIRTbl, const UINT8 arTbl[8])
{
    switch (eIIRTbl)
    {
    case MCNR_IIR_TBL_MA:
        NR_WM(MCNR_0A, 
            P_Fld(arTbl[0], MCNR_TBL_MA_0)|P_Fld(arTbl[1], MCNR_TBL_MA_1)|P_Fld(arTbl[2], MCNR_TBL_MA_2)|
            P_Fld(arTbl[3], MCNR_TBL_MA_3)|P_Fld(arTbl[4], MCNR_TBL_MA_4));
        NR_WM(MCNR_0B,
            P_Fld(arTbl[5], MCNR_TBL_MA_5)|P_Fld(arTbl[6], MCNR_TBL_MA_6)|P_Fld(arTbl[7], MCNR_TBL_MA_7));
        break;
    case MCNR_IIR_TBL_MC:     
        NR_WM(MCNR_0B, P_Fld(arTbl[0], MCNR_TBL_MC_0)|P_Fld(arTbl[1], MCNR_TBL_MC_1));
        NR_WM(MCNR_0C,
            P_Fld(arTbl[2], MCNR_TBL_MC_2)|P_Fld(arTbl[3], MCNR_TBL_MC_3)|P_Fld(arTbl[4], MCNR_TBL_MC_4)|P_Fld(arTbl[5], MCNR_TBL_MC_5)|
            P_Fld(arTbl[6], MCNR_TBL_MC_6));
        NR_WM(MCNR_0D, P_Fld(arTbl[7], MCNR_TBL_MC_7));
        break;
    case MCNR_IIR_TBL_C:
        NR_WM(MCNR_0D,
            P_Fld(arTbl[0], MCNR_TBL_C_0)|P_Fld(arTbl[1], MCNR_TBL_C_1)|P_Fld(arTbl[2], MCNR_TBL_C_2)|P_Fld(arTbl[3], MCNR_TBL_C_3));
        NR_WM(MCNR_0E, 
            P_Fld(arTbl[4], MCNR_TBL_C_4)|P_Fld(arTbl[5], MCNR_TBL_C_5)|P_Fld(arTbl[6], MCNR_TBL_C_6)|P_Fld(arTbl[7], MCNR_TBL_C_7));
        break;
    }
}

void vDrvMCNRSetTblThd(const UINT8 arTbl[8])
{
    NR_WM(MCNR_2E,
        P_Fld(arTbl[0], MCNR_TBL_THD1)|P_Fld(arTbl[1], MCNR_TBL_THD2)|
        P_Fld(arTbl[2], MCNR_TBL_THD3)|P_Fld(arTbl[3], MCNR_TBL_THD4));
    NR_WM(MCNR_2F,
        P_Fld(arTbl[4], MCNR_TBL_THD5)|P_Fld(arTbl[5], MCNR_TBL_THD6)|
        P_Fld(arTbl[6], MCNR_TBL_THD7)|P_Fld(arTbl[7], MCNR_TBL_THD8));
}

void vDrvMCNRSetTblThdFixed(UINT8 u1EndPtVal)
{ 
    NR_WM(MCNR_2E,
        P_Fld(((u1EndPtVal+4)>>3), MCNR_TBL_THD1)|P_Fld(((u1EndPtVal*2+4)>>3), MCNR_TBL_THD2)|
        P_Fld(((u1EndPtVal*3+4)>>3), MCNR_TBL_THD3)|P_Fld(((u1EndPtVal*4+4)>>3), MCNR_TBL_THD4));
    NR_WM(MCNR_2F,
        P_Fld(((u1EndPtVal*5+4)>>3), MCNR_TBL_THD5)|P_Fld(((u1EndPtVal*6+4)>>3), MCNR_TBL_THD6)|
        P_Fld(((u1EndPtVal*7+4)>>3), MCNR_TBL_THD7)|P_Fld(u1EndPtVal, MCNR_TBL_THD8));
}

void vDrvNRSetNMDebugInk(UINT32 u4Enable)
{
    vDrvDISetDebugOSDModule(u4Enable ? E_OSD_DBG_NR : E_OSD_DBG_NONE);
    vDrvDISetInkColor(0, 0x89A95310);
    vDrvDISetInkColor(1, 0xDCBB8546);
    vDrvDISetInkColor(2, 0x89AADCC0);
    vDrvDISetInkColor(3, 0x11235566);
    vDrvDISetInkColor(4, 0x23134570);
    vDrvDISetInkColor(5, 0x98744455);
    NR_WM(MCVP_KC_21, P_Fld((u4Enable ? 4 : 0)|((_rNrPrm.u2Width >= 960)<<3), OSD_MODE)|P_Fld(5, OSD_Y_POS)|P_Fld(5, OSD_X_POS));
}

void vDrvNXNRTrigToMoveData(void)
{
    NR_W(NXNR_14, 1, NX_SRAM_AUTO_MOV_TRG);
    NR_W(NXNR_14, 0, NX_SRAM_AUTO_MOV_TRG);
}

UINT8 u1DrvNXNRReadBlkLineSum(void)
{
    static UINT32 _u4ReadCnt = 0;
    UINT32 u4Ret = 2;
    UINT32 u4VBlkDataSize = MIN((_rNrPrm.u2Width>>2), NX_VBLK_LS_SIZE);
    UINT32 u4PacketSize = NX_PACK_SIZE;
    UINT16* pu2VBlkPxCnt = ((_u1LRFreeRunCnt) ? &_u2VBlkPxCntR : &_u2VBlkPxCnt);
    UINT32* pu4VBlkLineSum = _pu4VBlkLSPtr();
    UINT32* pu4VBlkAddr = (UINT32*) pu4VBlkLineSum + _u4ReadCnt;

    if (pu4VBlkLineSum == NULL || !u1DrvNRGetBlockMeterSupport() || u4VBlkDataSize == 0)
        return 0;

    _SWW(NR_BLK_STA_02, u4VBlkDataSize, NR_BLK_STA_VBLK_DATA_SIZE);
    if (_rNrPrm.u1Ver)
    {
        UINT32 u4Tmp = NR_R4B(NXNR_13);
        *pu2VBlkPxCnt = R_Fld(u4Tmp, NX_STA_VBLK_PIX_CNT);
        _SWW_4B(NR_BLK_STA_11, u4Tmp);
    }

    NR_W(NXNR_14, 1, NX_CPU_SEL_SRAM);
    NR_W(NXNR_14, 1, NX_SRAM_AUTO_INC_ADDR);
    NR_W(NXNR_15, _u4ReadCnt, NX_INIT_SRAM_ADDR);

    if ((_u4ReadCnt + NX_PACK_SIZE) >= u4VBlkDataSize)
    {
        u4PacketSize = (u4VBlkDataSize < _u4ReadCnt) ? 0 : (u4VBlkDataSize - _u4ReadCnt);
        _u4ReadCnt = 0;
        u4Ret = 1;
    }
    else
    {
        _u4ReadCnt += NX_PACK_SIZE;
    }

    while (u4PacketSize--)
    {
        *pu4VBlkAddr++ = NR_R4B(NXNR_24);
    }

    NR_W(NXNR_14, 0, NX_CPU_SEL_SRAM);

    return u4Ret;
}

UINT8 u1DrvNXNRWriteBlkPos(void)
{
    UINT32 i = 0;

    if (_u4BLKPos == NULL)
        return 0;
    
    NR_W(NXNR_14, 1, NX_CPU_SEL_SRAM);
    NR_W(NXNR_15, 0, NX_INIT_SRAM_ADDR);
    NR_W(NXNR_14, 1, NX_SRAM_AUTO_INC_ADDR);

    for (i = 0; i < NX_BLKPOS_SIZE; i++)
    {
        NR_W4B(NXNR_27, _u4BLKPos[i]);
    }

    NR_W(NXNR_14, 0, NX_CPU_SEL_SRAM);
    
    return 1;
}

static void _vDrvNXNRDeterminePos(void)
{
    UINT32 i = 0;
    UINT32 u4Width = _rNrPrm.u2Width;
    UINT32 u4Thr = _SWR(NR_BK_METER_02, NR_BK_THR);

    for (i = 1; i < u4Width-1; i++)
    {
        if ((V_LS(i) > u4Thr) && (V_LS(i) >= V_LS(i-1)) && (V_LS(i) >= V_LS(i+1)))
        {
            WR_VBLK(i, 3);
        }
        else
        {
            WR_VBLK(i, 0);
        }
    }
}

static void _vDrvMCNRReadSADHist(void)
{
#ifdef _WA_FHD_P_SADHIST_
    static UINT8 _u1Tag = 0;
#endif
    UINT8 i = 0;
    UINT32 u4Addr;
    UINT32 u4TmpVal;

    // FHD SAD Hist WA
#ifdef _WA_FHD_P_SADHIST_
    u4TmpVal = NR_R(MCNR_18, SAD_HIST_CNT_STAT);
	if (_rNrPrm.u1IsFhdP)
	{
		if (u4TmpVal == 0)
		{
			if (NR_R(MCNR_28, MCNR_ME_FHD_P))
			{
				NR_W(MCNR_28, 0, MCNR_ME_FHD_P);
				_u1Tag = 1;
			}			
		}
		else 
		{		
			if (NR_R(MCNR_28, MCNR_ME_FHD_P) == 0 && _u1Tag)
			{
				NR_W(MCNR_28, 1, MCNR_ME_FHD_P);
				_u1Tag = 0;
			}
		}
	}
#endif

    for (u4Addr = MCNR_19; u4Addr < MCNR_25; u4Addr += 4)
    {
        u4TmpVal = NR_R4B(u4Addr);
        _rNmSta.arSADHistCurr[i]   = R_Fld(u4TmpVal, SAD_HIST00_STAT);
        _rNmSta.arSADHistCurr[i+1] = R_Fld(u4TmpVal, SAD_HIST01_STAT);
        i += 2;        
    }

    _rNmSta.arSADHistCurr[SAD_HIST_LAST] = NR_R(MCNR_25, SAD_HIST24_STAT);
    _rNmSta.u4SADHistCntCurr = NR_R(MCNR_18, SAD_HIST_CNT_STAT);
}

static UINT8 _u1DrvNRStatusSel(UINT8 u1Sel)
{
    UINT8 u1Ret = 0;
    switch (u1Sel)
    {
    case 0:
        u1Ret = _SWR(NR_NMSTA_0E, NR_NM_STA_NL);
        break;
    case 1:
        u1Ret = _SWR(NR_NMSTA_0E, NR_NM_STA_CNTD);
        break;
    case 2:
        u1Ret = _SWR(NR_NMSTA_0E, NR_NM_STA_PEAK);
        break;
    case 3:
        u1Ret = _SWR(NR_NMSTA_0E, NR_NM_STA_FIRPEAK);
        break;
    case 4:
        u1Ret = _SWR(NR_NMSTA_0E, NR_NM_STA_PEAKMO);
        break;
    case 5:
        u1Ret = _SWR(NR_NMSTA_17, NR_NM_STA_FATTAIL_RTO);
        break;
    case 6:
        u1Ret = _SWR(NR_NMSTA_18, NR_NM_STA_MOFBK_RTO);
        break;
    case 7:
        u1Ret = NR_R(NXNR_00, NX_VBLK_WINDOW_MODE);
        break;
    
    }

    return u1Ret;
}

static void _vDrvMCNRUpdateOSDInk(void)
{
    UINT8 u1DbgSel0 = _SWR(NR_NM_DBG_00, NM_DBG_SEL_0);
    UINT8 u1DbgSel1 = _SWR(NR_NM_DBG_00, NM_DBG_SEL_1);
    UINT8 u1DbgSel2 = _SWR(NR_NM_DBG_00, NM_DBG_SEL_2);
    UINT8 u1DbgSel3 = _SWR(NR_NM_DBG_00, NM_DBG_SEL_3);
    
    UINT32 u4RegVal = _u1DrvNRStatusSel(u1DbgSel0)|((UINT32)_u1DrvNRStatusSel(u1DbgSel1)<<8)|
        ((UINT32)_u1DrvNRStatusSel(u1DbgSel2)<<16)|((UINT32)_u1DrvNRStatusSel(u1DbgSel3)<<24);

    _SWW_4B(NR_NMSTA_0D, u4RegVal);
    vDrvDISetDebugOSD(E_OSD_DBG_NR, u4RegVal);

    if (_rNmSta.u1ScnChg && _EN(EN_LOG_SCNCHG))
        LOG(3, "Scene Change: Peak = %d, HOMO = %d, TH = %d\n",
        _SWR(NR_NMSTA_0E, NR_NM_STA_PEAK), _rNmSta.u4PreHomoSum, _rNmSta.u4HomoSumDiffTh);
}

void vDrvNRPrintNMStatus(void)
{
//    LOG(3, "Peak Bin = %d\n", _rNmSta.u1PeakBin);
//    LOG(3, "Peak Bin MO = %d\n", _rNmSta.u1PeakBinMo);
//    LOG(3, "Centroid = %d\n", _rNmSta.u1Centroid);
    LOG(3, "----------\n");
//    LOG(3, "(MvApprove) = (%d)\n", _rNmSta.u1MvApprove);
    LOG(3, "----------\n");        
//    LOG(3, "NoiseLevel, Valid, FBCnt = %d, %d, %d\n", _rNmSta.u1NoiseLevel, _rNmSta.u1ValidNoiseLevel, _rNmSta.u1FbCnt);
//    LOG(3, (_rNmSta.u1FbkSta) ? "FbkSta = E_NR_FB_MCNR\n" : "FbkSta = E_NR_FB_MANR\n");
//    LOG(3, "FbkStep = %d\n", _rNmSta.u1FbkStep);
    LOG(3, "----------\n");
//    LOG(3, "GMV (Valid, X, Y) = (%d, %d, %d)\n", _rNmSta.u1GmvValid, _rNmSta.u1GmvX, _rNmSta.u1GmvY);
//    LOG(3, "SNR THM = %d\n", _rSnrTh.u2SnrThm);
}

static void _vScnChgForceMA(void)
{
    static UINT8 _u1PrevOnOff = 0xFF;
    UINT8 u1OnOff = _SWR(NR_CTRL_01, EN_FORCE_MA_SC);

    if (u1OnOff)
    {
        if (_rNmSta.u1ScnChgCnt == _SWR(NR_NM_07, NM_SC_HIST_SKIP))
        {    	
            NR_WM(MCNR_11, P_Fld(1, MCNR_FORCE_ALPHA_MC_EN)|P_Fld(0, MCNR_FORCE_ALPHA_MC));
        }
        else if (_rNmSta.u1ScnChgCnt == _SWR(NR_NM_07, NM_SC_HIST_SKIP) - 1)
        {		
            NR_WM(MCNR_11, P_Fld(0, MCNR_FORCE_ALPHA_MC_EN)|P_Fld(0, MCNR_FORCE_ALPHA_MC));
        }
    }
    else if (u1OnOff != _u1PrevOnOff)
    {
        NR_WM(MCNR_11, P_Fld(0, MCNR_FORCE_ALPHA_MC_EN)|P_Fld(0, MCNR_FORCE_ALPHA_MC));
    }

    _u1PrevOnOff = u1OnOff;
}

static void _vPatch8Bit(void)
{
    static UINT8 _u1PrevOnOff = 0xFF;
    UINT8 u1OnOff = _SWR(NR_CTRL_01, EN_PATCH_8BIT);
    
    // patch for 8bit
    if (u1OnOff || _u1PrevOnOff)
    {
        UINT32 u4Pause = u1OnOff ? (_rNmSta.u1Pause?1:0) : 0;
        NR_WM(MCNR_11,
            P_Fld(u4Pause, MCNR_FORCE_MA_WEI_Y_EN)|
            P_Fld(u4Pause, MCNR_FORCE_MC_WEI_Y_EN)|
            P_Fld(u4Pause, MCNR_FORCE_C_WEI_EN));
    }

    _u1PrevOnOff = u1OnOff;
}

static BOOL _fgIsFrequentlyScnChg(BOOL fgScnChg)
{
    static UINT8 _u1State = 0;
    static UINT8 _u1AccCnt = 0;
    static UINT8 _u1FrmCnt = 0;
    BOOL fgRet = 0;

    switch (_u1State)
    {
    case 0:
        fgRet = 0;
        if (fgScnChg)
        {
            _u1AccCnt = 1;
            _u1FrmCnt = 0;
            _u1State = 1;
        }
        break;
    case 1:
        fgRet = 0;
        if (fgScnChg)
        {
            _u1AccCnt ++;
        }
        _u1FrmCnt ++;

        if (_u1FrmCnt >= 20)
        {
            if (_u1AccCnt >= 6)
            {
                _u1AccCnt = 0;
                _u1FrmCnt = 0;
                _u1State = 2;
                fgRet = 1;
            }
            else
            {
                _u1State = 0;
            }
        }
        break;
    case 2:
        fgRet = 1;
        _u1FrmCnt = fgScnChg ? 0 : _u1FrmCnt+1;
        if (_u1FrmCnt >= (16 + _SWR(NR_NM_07, NM_SC_HIST_SKIP)))
        {
            _u1State = 0;
            fgRet = 0;
        }
        break;
    }

    //LOG(5, "_u1State = %d, Freq ScnChg = %d, _u1FrmCnt = %d, _u1AccCnt = %d\n", _u1State, fgRet, _u1FrmCnt, _u1AccCnt);
    
    return fgRet;
}

static void _vScnChgCnt(void)
{
    UINT8 u1ScSkip = 0;

    if (_rNmSta.u1ScnChg)
    {
        u1ScSkip = _SWR(NR_NM_07, NM_SC_HIST_SKIP);
    }
    else
    {
        u1ScSkip = (_rNmSta.u1ScnChgCnt > 1) ? (_rNmSta.u1ScnChgCnt - 1) : _rNmSta.u1ScnChgCnt;
    }

    _rNmSta.u1ScnChgCnt = u1ScSkip;
}

static void _vNLScnChgCnt(void)
{
    UINT8 _u1ScSkip = 0;

    if (_rNmSta.u1ScnChgCnt == 1)
    {
        _u1ScSkip = 16;
        _rNmSta.u1ScnChgCnt = 0;
    }
    else
    {
        _u1ScSkip = _rNmSta.u1NLScnChgCnt ? _rNmSta.u1NLScnChgCnt - 1 : 0;
    }

    _rNmSta.u1NLScnChgCnt = _u1ScSkip;
}

static UINT8 _u1DrvNRPerfectStill(void)
{
    static UINT8 _u1Idx = 0;
    static UINT8 _u1Cnt = 0;
    static UINT32 _u4PrevCrc[2] = {0};
    
    UINT32 u4Crc = NR_R(MCVP_KC_28, OUT_CRC_OUT_V);

    if (u4Crc == _u4PrevCrc[_u1Idx])
    {
        if (_u1Cnt < 20)
            _u1Cnt ++;
    }
    else
    {
        _u1Cnt = 0;
    }

    _u4PrevCrc[_u1Idx] = u4Crc;

    _u1Idx = (_u1Idx + 1) & 0x1;

    return _u1Cnt;
}

static void _vDrvNRCalcScnChgHomoSum(void)
{
    static UINT8 _u1PauseState = 0;
    UINT32 u4Gain = _SWR(NR_NM_0E, NM_SCNCHG_DET_TH_GAIN);
    UINT32 u4Max = _SWR(NR_NM_0E, NM_SCNCHG_DET_TH_MAX);
    UINT32 u4Min = _SWR(NR_NM_0C, NM_SCNCHG_DET_TH_MIN);

    UINT32 u4Diff = AbsDiff(_rNmSta.u4PreHomoSum, _rNmSta.u4HomoSum);

    UINT32 u4Th = (_rNmSta.u4PreHomoSum * u4Gain) >> 2;

    UINT8 u1PerfectStillCnt = _u1DrvNRPerfectStill();
    
    u4Th = MAX(MIN(u4Th, u4Max), u4Min);

    // scene change detection
    _SWW(NR_NMSTA_0F, ((u4Diff > u4Th) ? 1 : 0), NR_NM_HOMO_SCNCHG);
    _SWW(NR_NMSTA_10, u4Th, NR_NM_HOMO_SCNHG_TH);

    // patch for 8bit remain
    if (10 == NR_R(MCNR_01, MCNR_RND_PRECISION))
    {
        switch (_u1PauseState)
        {
        case 0:
            if ((_rNmSta.u4MoPixCnt == 0 && _rNmSta.u4PreMoPixCnt != 0 &&
                _rNmSta.u4HomoSum == 0 && _rNmSta.u4PreHomoSum != 0) ||
                u1PerfectStillCnt == 10)
            {
                _u1PauseState = 1;
            }
            break;
        case 1:
            if (_rNmSta.u4MoPixCnt == 0 && _rNmSta.u4PreMoPixCnt == 0 &&
                _rNmSta.u4HomoSum == 0 && _rNmSta.u4PreHomoSum == 0)
            {
                _rNmSta.u1Pause ++;
                if (_rNmSta.u1Pause >= 4)
                    _u1PauseState = 2;
            }
            else
            {
                _rNmSta.u1Pause = 0;
                _u1PauseState = 0;
            }
            break;
        case 2:
            if (_rNmSta.u1Pause)
            {
                _rNmSta.u1Pause --;
            }
            if (_rNmSta.u1Pause == 0 || _rNmSta.u4MoPixCnt || _rNmSta.u4HomoSum)
            {
                _rNmSta.u1Pause = 0;
                _u1PauseState = 0;
            }
            break;
        }        
    }
    else
    {
        _rNmSta.u1Pause = 0;
        _u1PauseState = 0;
    }

    _rNmSta.u4PreMoPixCnt = _rNmSta.u4MoPixCnt;
    _rNmSta.u4PreHomoSum = _rNmSta.u4HomoSum;
    _rNmSta.u4HomoSumDiffTh = u4Th;
}

#if 0
static UINT8 _u1DrvNRCalcForceScnChg(void)
{
    static UINT8 _u1PrevNL = 0xFF;
    static UINT8 _u1Cnt = 0;

    UINT8 u1CurrNL = _rNmSta.u1NoiseLevel; // _u1DrvNRGetNLForQ();
    UINT8 u1Ret = 0;

    if ((u1CurrNL == _u1PrevNL) /*&& (u1CurrNL != _rNmSta.u1NoiseLevel)*/)
    {
        if (_u1Cnt < 0xFF)
            _u1Cnt ++;
    }
    else
    {
        _u1Cnt = 0;
    }

    if (_u1Cnt > 240)
    {
        _u1Cnt = 0;
        //u1Ret = 1;
    }

    _u1PrevNL = (UINT8) u1CurrNL;

    return u1Ret;
}
#endif

static void _vDrvMCNRReadHWStatus(void)
{
    UINT8 u1AbsX, u1AbsY, u1ForceChg;

    if (!_u1Init)
        return;

    _rNmSta.u1FlfbStatus = NR_R(MCVP_CS_29, B_FLFB_STATUS);
    _rNmSta.u1CompositeStatus = NR_R(MCVP_CS_29, COMPOSITE_STATUS);
    _rNmSta.u2Nzc = NR_R(MCVP_CS_29, CS_NON_ZERO_CNT);
    _rNmSta.u1GmvValid = NR_R(MCVP_CS_28, GMV_VALID);
    _rNmSta.u1GmvRatio = NR_R(MCNR_18, MCNR_GMV_RATIO);
    _rNmSta.u1FavorCSState = _SWR(PSCAN_FWCS_02, FAVOR_CS_STATE);
    _rNmSta.u4HomoSum = NR_R(MCVP_FUSION_13, IF_PD_SUM_HOMO_UP) + NR_R(MCVP_FUSION_15, IF_PD_SUM_HOMO_DN);
    _rNmSta.u4StaDetail = NR_R(MCVP_CS_1C, STA_DETAIL);
    _rNmSta.u4EdgeCnt = NR_R(MCVP_CS_18, STA_EDGE_CNT);
    _rNmSta.u2HistMaxCnt = NR_R(MCVP_CS_27, STA_MAX_CNT1);
    _rNmSta.u4TmprDiffSum = NR_R(MCNR_2A, SUM_TMPRL_DIFF_STAT);
    _rNmSta.u4TmprDiffCnt = NR_R(MCNR_2B, TMPRL_DIFF_CNT_STAT);
    _rNmSta.u4MoPixCnt = NR_R(MCVP_STATUS_0F, OLD_FRAME_MO_Q_STATUS);
    _rNmSta.u4HistDiffAcc = NR_R(MCVP_CS_2A, HIST_DIFF_ACC);

    _vDrvNRCalcScnChgHomoSum();
    u1ForceChg = 0; //_u1DrvNRCalcForceScnChg();
    
    if (_SWR(NR_NM_10, NM_SCNCHG_DET_FORCE_OFF))
    {
        _rNmSta.u1ScnChg = 0;
    }
    else
    {
        BOOL fgScnChg = (_SWR(NR_NM_0E, NM_SCNCHG_DET_EN) ? 
            _SWR(NR_NMSTA_0F, NR_NM_HOMO_SCNCHG) : NR_R(MCVP_CS_29, SCENE_CHANGE_STATUS));
        BOOL fgIsFreqScnChg = _fgIsFrequentlyScnChg(fgScnChg);
        fgScnChg = fgScnChg && !fgIsFreqScnChg;
        _rNmSta.u1ScnChg = fgScnChg || u1ForceChg;
    }

    u1AbsX = NR_R(MCVP_CS_28, GMV_MVX);
    u1AbsY = NR_R(MCVP_CS_28, GMV_MVY);

    u1AbsX = (u1AbsX & 0x10) ? (((~u1AbsX)&0x1F)+1) : u1AbsX;
    u1AbsY = (u1AbsY & 0x8) ? (((~u1AbsY)&0xF)+1) : u1AbsY;

    _rNmSta.u1GmvX = u1AbsX;
    _rNmSta.u1GmvY = u1AbsY;

    _vScnChgCnt();

    _SWW(NR_NMSTA_0F, _rNmSta.u1FavorCSState, NR_NM_STA_DI_FAVOR_CS_LVL);

    _vDrvMCNRReadSADHist();
}
void vDrvNR3DNRDitherInit(void)
{
    NR_WM(MCNR_3E, P_Fld(0, MCNR_DITHER_EN)|P_Fld(0, MCNR_DITHER_BYPASS));
    NR_W(MCNR_3E, 0, MCNR_DITHER_FPHASE_EN);
    NR_W(MCNR_3E, 27, MCNR_DITHER_FPHASE);
    NR_W(MCNR_3E, 1, MCNR_DITHER_FPHASE_CTRL);
    NR_W(MCNR_3E, 2, MCNR_DITHER_FPHASE_SEL);
    NR_W(MCNR_3E, 4, MCNR_DITHER_FPHASE_BIT);
    NR_W(MCNR_3E, 1, MCNR_DITHER_TABLE_EN);
    NR_W(MCNR_3E, 1, MCNR_DITHER_SUBSAMPLE_MODE);
    NR_W(MCNR_3E, 0, MCNR_DITHER_INK_EN);
    NR_W(MCNR_3E, 1, MCNR_DITHER_PHASE_R);
}
void vDrvNRMCVPDitherInit(void)
{
    NR_WM(MCVP_KC_3E, P_Fld(0, DI_MCNR_DITHER_EN)|P_Fld(0, DI_MCNR_DITHER_BYPASS)|P_Fld(0, DI_MCNR_ROUND_EN));
    NR_W(MCVP_KC_3E, 0, DI_MCNR_DITHER_FPHASE_EN);
    NR_W(MCVP_KC_3E, 27, DI_MCNR_DITHER_FPHASE);
    NR_W(MCVP_KC_3E, 1, DI_MCNR_DITHER_FPHASE_CTRL);
    NR_W(MCVP_KC_3E, 2, DI_MCNR_DITHER_FPHASE_SEL);
    NR_W(MCVP_KC_3E, 4, DI_MCNR_DITHER_FPHASE_BIT);
    NR_W(MCVP_KC_3E, 1, DI_MCNR_DITHER_TABLE_EN);
    NR_W(MCVP_KC_3E, 1, DI_MCNR_DITHER_SUBSAMPLE_MODE);
    NR_W(MCVP_KC_3E, 0, DI_MCNR_DITHER_INK_EN);
    NR_W(MCVP_KC_3E, 1, DI_MCNR_DITHER_FPHASE_R);
}
void vDrvNRPQInit(void)
{
    x_memset(&_rNmSta, 0, sizeof(NR_NM_STA_T));

    _u4VBlkLineSum  = (UINT32*) x_mem_alloc(sizeof(UINT32)*NX_VBLK_LS_SIZE);
    _u4VBlkLineSumR = (UINT32*) x_mem_alloc(sizeof(UINT32)*NX_VBLK_LS_SIZE);

    if (_u4VBlkLineSum == NULL || _u4VBlkLineSumR == NULL)
    {
        LOG(0, "Memory allocation for block meter failed.\n");
    }
    else
    {
        x_memset(_u4VBlkLineSum,  0, sizeof(UINT32)*NX_VBLK_LS_SIZE);
        x_memset(_u4VBlkLineSumR, 0, sizeof(UINT32)*NX_VBLK_LS_SIZE);
    }
    
    NR_WM(MCNR_00, P_Fld(1, MCNR_FORCE_SCENE_CHANGE_EN)|P_Fld(0, MCNR_FORCE_SCENE_CHANGE));
    NR_W(MCNR_00, 2, MCNR_ACDC_MODE);
    NR_W(MCNR_01, 0, MCNR_DCNR_ALPHA);
    
    NR_W(MCNR_17, 1, MCNR_VALID_NOISE_LEVEL); //always set valid noise level:let mcnr_mc_noise_add_wei_nl_gain work.
    NR_W(NXNR_04, 4, NX_SNR_GAIN_Y);
    _SWW_4B(NR_CTRL_00, 0x80010030);
    
    _SWW_4B(NR_NM_00, 0x00AC8120);
    _SWW_4B(NR_NM_03, 0x0149A810);  // clip noise level at 8 (0x20)
    _SWW_4B(NR_NM_04, 0x000026FF);

    _SWW_4B(NR_PQ_07, 0x318C6047);

    // MA Table
    _SWW(NR_PQ_00, 32, NR_TBL_MA_0);
    _SWW(NR_PQ_00, 40, NR_TBL_MA_1);
    _SWW(NR_PQ_00, 40, NR_TBL_MA_2);
    _SWW(NR_PQ_00, 32, NR_TBL_MA_3);
    _SWW(NR_PQ_00, 20, NR_TBL_MA_4);
    _SWW(NR_PQ_01, 15, NR_TBL_MA_5);
    _SWW(NR_PQ_01, 10, NR_TBL_MA_6);
    _SWW(NR_PQ_01,  5, NR_TBL_MA_7);
    // MA Table End
    _SWW(NR_PQ_03, 36, NR_TBL_MA_E_0);
    _SWW(NR_PQ_03, 48, NR_TBL_MA_E_1);
    _SWW(NR_PQ_03, 44, NR_TBL_MA_E_2);
    _SWW(NR_PQ_03, 30, NR_TBL_MA_E_3);
    _SWW(NR_PQ_04, 22, NR_TBL_MA_E_4);
    _SWW(NR_PQ_04, 16, NR_TBL_MA_E_5);
    _SWW(NR_PQ_04, 12, NR_TBL_MA_E_6);
    _SWW(NR_PQ_04,  5, NR_TBL_MA_E_7);
    // MA C Table
    _SWW(NR_PQ_0C, 32, NR_TBL_C_0);
    _SWW(NR_PQ_0C, 40, NR_TBL_C_1);
    _SWW(NR_PQ_0C, 40, NR_TBL_C_2);
    _SWW(NR_PQ_0C, 32, NR_TBL_C_3);
    _SWW(NR_PQ_0C, 20, NR_TBL_C_4);
    _SWW(NR_PQ_0D, 15, NR_TBL_C_5);
    _SWW(NR_PQ_0D, 10, NR_TBL_C_6);
    _SWW(NR_PQ_0D,  5, NR_TBL_C_7);
    // MAX Wei
    _SWW(NR_PQ_05, 23, NR_IIR_MAX_WEI);
    _SWW(NR_PQ_05,  0, NR_IIR_MAX_WEI_E);
    // MA Thd
    _SWW(NR_PQ_0E,  4, NR_TBL_THD_1);
    _SWW(NR_PQ_0E,  8, NR_TBL_THD_2);
    _SWW(NR_PQ_0E, 12, NR_TBL_THD_3);
    _SWW(NR_PQ_0E, 20, NR_TBL_THD_4);
    _SWW(NR_PQ_0F, 25, NR_TBL_THD_5);
    _SWW(NR_PQ_0F, 30, NR_TBL_THD_6);
    _SWW(NR_PQ_0F, 40, NR_TBL_THD_7);
    _SWW(NR_PQ_0F, 60, NR_TBL_THD_8);
    // MA Thd End
    _SWW(NR_PQ_10,  8, NR_TBL_THD_E_1);
    _SWW(NR_PQ_10, 16, NR_TBL_THD_E_2);
    _SWW(NR_PQ_10, 28, NR_TBL_THD_E_3);
    _SWW(NR_PQ_10, 40, NR_TBL_THD_E_4);
    _SWW(NR_PQ_11, 50, NR_TBL_THD_E_5);
    _SWW(NR_PQ_11, 60, NR_TBL_THD_E_6);
    _SWW(NR_PQ_11, 80, NR_TBL_THD_E_7);
    _SWW(NR_PQ_11,100, NR_TBL_THD_E_8);
	//rnr
    _SWW(NR_PQ_17,0x200, RNR_AGG_THM_OFST);
    // SNR Gain
    _SWW(NR_PQ_06,  0, SNR_MAX_RTO_LVL_0);
    _SWW(NR_PQ_06,  2, SNR_MAX_RTO_LVL_1);
    _SWW(NR_PQ_06,  4, SNR_MAX_RTO_LVL_2);
    _SWW(NR_PQ_06,  8, SNR_MAX_RTO_LVL_3);
    _SWW(NR_PQ_06, 12, SNR_MAX_RTO_LVL_4);
    _SWW(NR_PQ_0B, 16, SNR_MAX_RTO_LVL_5);
    _SWW(NR_PQ_0B, 20, SNR_MAX_RTO_LVL_6);
    _SWW(NR_PQ_0B, 24, SNR_MAX_RTO_LVL_7);
    _SWW(NR_PQ_0B, 28, SNR_MAX_RTO_LVL_8);
    _SWW(NR_PQ_0B, 32, SNR_MAX_RTO_LVL_9);
    // Remapping
    _SWW(NR_PQ_12,  8, NR_NL_THD_MIN);
    _SWW(NR_PQ_12, 32, NR_NL_THD_MAX);
    _SWW(NR_PQ_12,  8, SNR_NL_THD_MIN);
    _SWW(NR_PQ_12, 16, SNR_NL_THD_MAX);

    _SWW(NR_NM_05, 2300, SNR_THM_GAIN_A);
    _SWW(NR_NM_05, 75743, SNR_THM_GAIN_B);
    _SWW(NR_NM_06, (-908984), SNR_THM_OFST_C);
    _SWW(NR_NM_06, 1, NM_SC_NL_MODE);
        
	_SWW(NR_NM_07, 1, SNR_THM_SEL);
    _SWW(NR_NM_07, 20, NM_SC_HIST_SKIP);

    _SWW(NR_NM_0C, 0x3E00, CLEAN_DF_LVL_PDSUM_TH);

    _SWW(NR_NM_0E, 1200000, NM_SCNCHG_DET_TH_MAX);
    _SWW(NR_NM_0E, 10, NM_SCNCHG_DET_TH_GAIN);
    _SWW(NR_NM_0C, 10000, NM_SCNCHG_DET_TH_MIN);

    _SWW(NR_NM_0F, 0x10, NM_ANTI_NL_GAIN_THD_LO);
    _SWW(NR_NM_0F, 0x32, NM_ANTI_NL_GAIN_THD_HI);
    _SWW(NR_NM_0F, 0x3F, NM_ANTI_NL_GAIN_MAX);
    _SWW(NR_NM_0F, 0x0, NM_ANTI_NL_GAIN_EN);

    _SWW(NR_BK_METER_00, 7, NR_BK_GOOD_THD);
    _SWW(NR_BK_METER_00, 8, NR_BK_LOCAL_MIN_RTO);
    _SWW(NR_BK_METER_00, 5, NR_BK_LOCAL_MIN_THD);
    _SWW(NR_BK_METER_00, 20, NR_BK_GOOD_AVG_MIN_DEN);
    _SWW(NR_BK_METER_00, 235, NR_BK_FNL_IIR);

    _SWW(NR_NM_03,   0, UPDATE_NL_SC);
    _SWW(NR_NM_0F,   0, NM_ADJ_BY_FATTAIL);
    _SWW(NR_NM_10,   0, NM_ADJ_BY_MOFBK);  // change to 0, because of some noisy pattern's noise level not correct .
    _SWW(NR_NM_10,  16, NM_ADJ_BY_MOFBK_OFST);
    _SWW(NR_NM_0F,   1, NM_STABLIZER_IIR_EN);
    _SWW(NR_NM_10, 248, NM_STABLIZER_IIR);
    _SWW(NR_CTRL_01, 1, EN_FORCE_MA_SC);
        
    _SWW(NR_CTRL_00, 1, EN_SLOW_ERFB);
    _SWW(NR_NM_0D, 4, NM_ERFB_INC);
    _SWW(NR_NM_0D, -4, NM_ERFB_DEC);
    _SWW(NR_NM_0E, 2, NM_ERFB_INC_OFST);
    _SWW(NR_NM_03, 1, ERFB_MODE);

    _SWW(NR_NM_0F, 8, NM_TDF_STA_IIR);
    
/*
    _SWW(NR_NM_03, 0, UPDATE_NL_SC);
    _SWW(NR_CTRL_00, 0, EN_SLOW_ERFB);
    _SWW(NR_NM_0D, 8, NM_ERFB_INC);
    _SWW(NR_NM_0D, -8, NM_ERFB_DEC);
    _SWW(NR_NM_0E, 1, NM_ERFB_INC_OFST);
*/
    //_SWW(NR_PQ_13, 1, RNR_TH_ADAP);
    //_SWW(NR_PQ_13, 1, BNR_RNR_ADAP);
    _SWW(NR_PQ_13, 0x28/*0x3E*/, BNR_RNR_TH);
    _SWW(NR_PQ_13, 0x3F/*28*/, RNR_MAX_TH);

    _SWW(NR_PQ_14, 20, SNR_NL_GAIN_MIN);
    _SWW(NR_PQ_14, 32, SNR_NL_GAIN_MAX);

    _SWW(NR_FWBNR_00, 1, NR_FWBNR_LOC_WRITE);

    vDrvNRSet3DNRAutoStrength(0);
    NR_FW_NM_Init();
    NR_FW_ANR_Init();
    BNR_FW_Initialize(1920, 1080);
        
    _u1Init = 1;
}

void vDrvNRDNRWAProc(void)
{
    UINT32 u4DotLevelSum =  NR_R(MCNR_3C,DNR0_DOT_LEVEL_SUM_STATUS);
    UINT32 u4DotLevelCntTH =  NR_R(MCNR_3A,DNR0_DOT_LEVEL_CNT_TH);

    if (_SWR(NR_PQ_0A,  NR_DNR_WA_EN) == 0)
    {
        return;
    }

    if (u4DotLevelSum > u4DotLevelCntTH)
    {
        if (_u1DotCrawlLevel<8)
        {
            _u1DotCrawlLevel++;
        }
    }
    else
    {
        if(_u1DotCrawlLevel>0)
        {
            _u1DotCrawlLevel--;
        }
    }

    _SWW(NR_PQ_0A, _u1DotCrawlLevel, NR_DotCrawl_Level);

    if (!_u1DotCrawlLevel)
    {
        NR_W(MCNR_39, 0, DNR0_EN);	
    }
    else
    {
        NR_W(MCNR_39, 1, DNR0_EN);	
    }	

}

void NR_FW_SetState(E_NR_FW_STATE eState)
{
    _rNmSta.eNrFwState = eState;
}

void vDrvNRISR(void)
{
    UINT32 u4Start;
    UINT32 u4End;

    if (!_u1Init)
        return;

    u4Start = _u4GetCurrLineCnt();
    _u1NrProcFlg = 0;

    if (bDrvVideoSignalStatus(SV_VP_MAIN) != (UINT8)SV_VDO_STABLE)
        _rNmSta.eNrFwState = NR_FW_STOP;

    if (_rNmSta.eNrFwState == NR_FW_VSYNC)
    {
        NR_FW_ANR_WriteReg();
        _vDrvMCNRUpdateOSDInk();

        _vDrvMCNRReadHWStatus();
        _vScnChgForceMA();
        _vPatch8Bit();

        if (_rNmSta.eNrFwState != NR_FW_STOP)
            _rNmSta.eNrFwState = NR_FW_ACTIVE;
    }
	
    u4End = _u4GetCurrLineCnt();

    _SWWM(NR_NMSTA_12, P_Fld(u4Start, NR_VSYNC_PROC_START)|P_Fld(u4End, NR_VSYNC_PROC_END));
}

void vDrvNRVBlankProc(void)
{
    UINT32 u4StartLine = 0;
    UINT32 u4CurrLine = 0;
    UINT32 u4VBlkOK = 1;
    UINT32 u4WriteOK = 0;
    UINT32 u4ReadFail = 0;
    UINT32 u4VSkipLine = 8*NR_R(NXNR_00, NX_DEBLOCK_V_MASK);

    CRIT_STATE_T csState;

    if (!_u1Init)
        return;

    csState = x_crit_start();
    
    u4StartLine = _u4GetCurrLineCnt();

    _u1LRFreeRunCnt = 0; //!_u1LRFreeRunCnt;
    
#if 0
    if( (u4StartLine < NR_R(MCVP_KC_0B, OUT_ADJ_INT_LINE)) && (u4StartLine > u4VSkipLine) ||
        u4StartLine+5 > 1125 )
    {	   
        _SWWM(NR_BLK_STA_03,
            P_Fld(1, NR_BLK_READ_FAIL)| P_Fld(1, NR_BLK_READ_VHOLD)|
            P_Fld(u4StartLine, NR_BLK_READ_END)| P_Fld(u4StartLine, NR_BLK_READ_START));

        return;
    }
#endif
    //if (!_EN(EN_READ_NXNR_SRAM))
    {
        BNR_FW_VSyncProcess();

        #if 0
        u4VBlkOK = u1DrvNXNRReadBlkLineSum();

        if (_EN(EN_FW_BLK_POS))
        {
            u4WriteOK = u1DrvNXNRWriteBlkPos();
        }
        #endif
    }

    u4CurrLine = _u4GetCurrLineCnt();

    x_crit_end(csState);

    //u4ReadFail = (u4CurrLine < u4StartLine) || (u4VBlkOK == 0) || (!u4HBlkOK) ||
    //    (MIN(u4StartLine, u4CurrLine) < NR_R(MCVP_KC_0B, OUT_ADJ_INT_LINE));	
    u4ReadFail = !((u4CurrLine >= u4StartLine) || (u4CurrLine <= u4VSkipLine))
        || (u4VBlkOK == 0);
	
    _SWWM(NR_BLK_STA_03,
        P_Fld(u4ReadFail, NR_BLK_READ_FAIL)|P_Fld((u4VBlkOK == 2), NR_BLK_READ_VHOLD)|P_Fld(u4WriteOK, NR_BLK_POS_WRITE_OK)|
        P_Fld(u4CurrLine, NR_BLK_READ_END)|P_Fld(u4StartLine, NR_BLK_READ_START));

    vDrvNRSemaUnlock();
}

void vDrvNRNoiseLevelProc(void)
{
    UINT32 u4Start;
    UINT32 u4End;
    UINT32 u4Dirty;

    NR_NM_ANA_T rNmAna;

    if (!_u1Init)
        return;

    _u1NrProcFlg = 1;
    u4Start = _u4GetCurrLineCnt();

    //_u1LRFreeRunCnt = !_u1LRFreeRunCnt;

    if (_rNmSta.eNrFwState == NR_FW_ACTIVE /* && _u1LRFreeRunCnt*/)
    {
        _vNLScnChgCnt();

        //if (_rNmSta.u1ScnChg)
        //    BNR_FW_SetWindowMode(4);
        _u4EnMsk = _SWR_4B(NR_CTRL_00);
        
        NR_FW_NM_Process(&_rNmSta, &rNmAna);
        NR_FW_ANR_Process(&_rNmSta, &rNmAna);

        //_vDrvNXNRCalcBlkHist();
        //_vDrvNXNRCalcFinalStrength();
        if (_EN(EN_COPY_LINE_SUM) || _EN(EN_READ_NXNR_SRAM))
        {
            BNR_FW_Stop();
            if (_EN(EN_COPY_LINE_SUM))
                BNR_FW_Dbg();
        }
        else
        {
            BNR_FW_Resume();
        }

        if (_EN(EN_TEST_AUTO_BLK))
            _vDrvNXNRDeterminePos();

        if (_rNmSta.eNrFwState != NR_FW_STOP)
            _rNmSta.eNrFwState = NR_FW_VSYNC;
    }

    u4End = _u4GetCurrLineCnt();

    u4Dirty = (u4Start <= _SWR(NR_NMSTA_12, NR_VSYNC_PROC_END) || _u1NrProcFlg == 0);
    _u1NrProcFlg = 0;

    _SWWM(NR_NMSTA_11, P_Fld(u4Dirty, NR_PROC_DIRTY)|P_Fld(u4Start, NR_PROC_START)|P_Fld(u4End, NR_PROC_END));
}

