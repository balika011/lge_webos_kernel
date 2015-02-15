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
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_mjc_dsr.c,v $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#ifdef _C_MODEL_
#include "MJC_top.h"
#include "drv_mjc_dsr.h"
#else
#include "typedef.h"
#include "x_hal_io.h"
#include "hw_mjc.h"
#include "hw_sw.h"
#include "mjc_debug.h"
#include "drv_mjc_dsr.h"
#endif

/*----------------------------------------------------------------------------*
 * Definitions
 *----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(x) ((x>0) ? (x) : (-(x)))
#endif


/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/

MJC_DSR_STA_T rMJCDsrSta;
BND_FG_STA_T rBndFgSta;

UINT32 u4Width= 1920;
UINT32 u4Height= 1080;

INT8 _i1DSROfst = 0;
UINT8 _u1PtFrmCnt = 0;
UINT16  u2B1T[3][2], u2B2T[3][2], u2B1C[3][2], u2B2C[3][2];
UINT16  u2R1T[5][2], u2R2T[5][2], u2R1C[5][2], u2R2C[5][2];
UINT32 u4TSMT[2];    // take side - motion tracking
UINT32 u4MBMT[2];    // move back - motion tracking
UINT32 u4TSHR[2];    // take side - halo reduction
UINT32 u4MBHR[2];    // move back - halo reduction

/*----------------------------------------------------------------------------*
 * C model only section
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Functions
 *----------------------------------------------------------------------------*/
void vDrvMJCDsrInit(void)
{ 
    vIO32WriteFldAlign(MJC_DSR_CTRL_00, 1, MJC_DSR_60_120_SEL); // 

    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_PYEOFST_EN); // switch
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 0, MJC_DSR_PYEOFST_FZ); // force zero
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_MVPAD_EN); // adaptive padding, should be turned off
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 0, MJC_DSR_MVPAD_FZ); // use old adaptive padding
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 0, MJC_DSR_BNDFG_EN); // boundary forgr
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_TSMT_EN);
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_MBMT_EN);
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_TSHR_EN);
    vIO32WriteFldAlign(MJC_DSR_CTRL_01, 1, MJC_DSR_MBHR_EN);

    vIO32WriteFldAlign(MJC_DSR_CTRL_02, 30, MJC_DSR_P_FRM_CNT_BND); // print log every 30 frames
    vIO32WriteFldAlign(MJC_DSR_CTRL_02, 0, MJC_DSR_PRINTF_EN); 
    vIO32WriteFldAlign(MJC_DSR_CTRL_02, 0, MJC_DSR_VCODE_SEL_1);
    vIO32WriteFldAlign(MJC_DSR_CTRL_02, 0, MJC_DSR_VCODE_SEL_2);

    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 8, MJC_DSR_OSD_H_CT); // position
    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 90, MJC_DSR_OSD_V_CT); //
    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 6, MJC_DSR_OSD_WD);
    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 6, MJC_DSR_OSD_HT);
    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 8, MJC_DSR_OSD_V_OFST);
    vIO32WriteFldAlign(MJC_DSR_CTRL_03, 0, MJC_DSR_OSD_DBG_EN);

    vIO32WriteFldAlign(MJC_DSR_CTRL_04, 256, MJC_DSR_EVAL_THR1);
    vIO32WriteFldAlign(MJC_DSR_CTRL_04, 64, MJC_DSR_EVAL_THR2);
    vIO32WriteFldAlign(MJC_DSR_CTRL_05, 64, MJC_DSR_EVAL_THR3);
    vIO32WriteFldAlign(MJC_DSR_CTRL_05, 48, MJC_DSR_EVAL_THR4);	
}

/* -------------------------------------------------------------------------- */
void vDrvMJCDsrGetSta(void)
{
#ifdef _C_MODEL_
    UINT32 i;
    

    for (i=0;i<7;i++) {
        rMJCDsrSta.u2MVYBlkHistG1[i] = MJC_common::csDSRSTA.a_sta_dsr_blk_mvy_hist_g1[i];
        rMJCDsrSta.u2MVYBlkHistG2[i] = MJC_common::csDSRSTA.a_sta_dsr_blk_mvy_hist_g2[i];
    }
    for (i=0;i<11;i++) {
        rMJCDsrSta.u1MVYRgnHistG1[i] = MJC_common::csDSRSTA.a_sta_dsr_rgn_mvy_hist_g1[i];
        rMJCDsrSta.u1MVYRgnHistG2[i] = MJC_common::csDSRSTA.a_sta_dsr_rgn_mvy_hist_g2[i];
    }
    for (i=0;i<9;i++) {
        rMJCDsrSta.i2PPCRgnMVX[i] = MJC_common::csDSRSTA.a_sta_ppc_rgn_mv[i].x>>2;
        rMJCDsrSta.i1PPCRgnMVY[i] = MJC_common::csDSRSTA.a_sta_ppc_rgn_mv[i].y>>2;
    }
    rMJCDsrSta.i2PPCGlbMVX = MJC_common::csDSRSTA.sta_ppc_glb_mv.x>>2;
    rMJCDsrSta.i1PPCGlbMVY = MJC_common::csDSRSTA.sta_ppc_glb_mv.y>>2;
    rMJCDsrSta.u2PPCGmvBlkCnt = MJC_common::csDSRSTA.sta_ppc_gmv_blk_cnt;

    rMJCDsrSta.i1ME2GlbMVY = MJC_common::csFWGSTA.gmv_bi.y>>2;
    rMJCDsrSta.i2ME2GlbMVX = MJC_common::csFWGSTA.gmv_bi.x>>2;
#else
    UINT32 i,k;
    UINT32 it,ic; // t: target side, c: competitive side
    UINT32 u4RGain; // TBD: need modify for HPR/FPR

    rMJCDsrSta.u2MVYBlkHistG1[0] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_00, MJC_STA_DSR_BLK_MVY_HIST_G1_0); // Group 1 for motion tracking
    rMJCDsrSta.u2MVYBlkHistG1[1] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_00, MJC_STA_DSR_BLK_MVY_HIST_G1_1);
    rMJCDsrSta.u2MVYBlkHistG1[2] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_01, MJC_STA_DSR_BLK_MVY_HIST_G1_2);
    rMJCDsrSta.u2MVYBlkHistG1[3] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_01, MJC_STA_DSR_BLK_MVY_HIST_G1_3);
    rMJCDsrSta.u2MVYBlkHistG1[4] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_02, MJC_STA_DSR_BLK_MVY_HIST_G1_4);
    rMJCDsrSta.u2MVYBlkHistG1[5] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_02, MJC_STA_DSR_BLK_MVY_HIST_G1_5);
    rMJCDsrSta.u2MVYBlkHistG1[6] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_03, MJC_STA_DSR_BLK_MVY_HIST_G1_6);
    rMJCDsrSta.u2MVYBlkHistG2[0] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_03, MJC_STA_DSR_BLK_MVY_HIST_G2_0); // g2 more precise, for halo reduction
    rMJCDsrSta.u2MVYBlkHistG2[1] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_04, MJC_STA_DSR_BLK_MVY_HIST_G2_1);
    rMJCDsrSta.u2MVYBlkHistG2[2] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_04, MJC_STA_DSR_BLK_MVY_HIST_G2_2);
    rMJCDsrSta.u2MVYBlkHistG2[3] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_05, MJC_STA_DSR_BLK_MVY_HIST_G2_3);
    rMJCDsrSta.u2MVYBlkHistG2[4] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_05, MJC_STA_DSR_BLK_MVY_HIST_G2_4);
    rMJCDsrSta.u2MVYBlkHistG2[5] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_06, MJC_STA_DSR_BLK_MVY_HIST_G2_5);
    rMJCDsrSta.u2MVYBlkHistG2[6] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_06, MJC_STA_DSR_BLK_MVY_HIST_G2_6);

    rMJCDsrSta.u1MVYRgnHistG1[0] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_07, MJC_STA_DSR_RGN_MVY_HIST_G1_0);
    rMJCDsrSta.u1MVYRgnHistG1[1] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_07, MJC_STA_DSR_RGN_MVY_HIST_G1_1);
    rMJCDsrSta.u1MVYRgnHistG1[2] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_07, MJC_STA_DSR_RGN_MVY_HIST_G1_2);
    rMJCDsrSta.u1MVYRgnHistG1[3] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_07, MJC_STA_DSR_RGN_MVY_HIST_G1_3);
    rMJCDsrSta.u1MVYRgnHistG1[4] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_08, MJC_STA_DSR_RGN_MVY_HIST_G1_4);
    rMJCDsrSta.u1MVYRgnHistG1[5] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_08, MJC_STA_DSR_RGN_MVY_HIST_G1_5);
    rMJCDsrSta.u1MVYRgnHistG1[6] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_08, MJC_STA_DSR_RGN_MVY_HIST_G1_6);
    rMJCDsrSta.u1MVYRgnHistG1[7] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_08, MJC_STA_DSR_RGN_MVY_HIST_G1_7);
    rMJCDsrSta.u1MVYRgnHistG1[8] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_09, MJC_STA_DSR_RGN_MVY_HIST_G1_8);
    rMJCDsrSta.u1MVYRgnHistG1[9] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_09, MJC_STA_DSR_RGN_MVY_HIST_G1_9);
    rMJCDsrSta.u1MVYRgnHistG1[10] = IO32ReadFldAlign(MJC_ME_MVY_HIST_09, MJC_STA_DSR_RGN_MVY_HIST_G1_10);

    rMJCDsrSta.u1MVYRgnHistG2[0] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_10, MJC_STA_DSR_RGN_MVY_HIST_G2_0);
    rMJCDsrSta.u1MVYRgnHistG2[1] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_10, MJC_STA_DSR_RGN_MVY_HIST_G2_1);
    rMJCDsrSta.u1MVYRgnHistG2[2] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_10, MJC_STA_DSR_RGN_MVY_HIST_G2_2);
    rMJCDsrSta.u1MVYRgnHistG2[3] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_10, MJC_STA_DSR_RGN_MVY_HIST_G2_3);
    rMJCDsrSta.u1MVYRgnHistG2[4] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_11, MJC_STA_DSR_RGN_MVY_HIST_G2_4);
    rMJCDsrSta.u1MVYRgnHistG2[5] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_11, MJC_STA_DSR_RGN_MVY_HIST_G2_5);
    rMJCDsrSta.u1MVYRgnHistG2[6] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_11, MJC_STA_DSR_RGN_MVY_HIST_G2_6);
    rMJCDsrSta.u1MVYRgnHistG2[7] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_11, MJC_STA_DSR_RGN_MVY_HIST_G2_7);
    rMJCDsrSta.u1MVYRgnHistG2[8] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_12, MJC_STA_DSR_RGN_MVY_HIST_G2_8);
    rMJCDsrSta.u1MVYRgnHistG2[9] =  IO32ReadFldAlign(MJC_ME_MVY_HIST_12, MJC_STA_DSR_RGN_MVY_HIST_G2_9);
    rMJCDsrSta.u1MVYRgnHistG2[10] = IO32ReadFldAlign(MJC_ME_MVY_HIST_12, MJC_STA_DSR_RGN_MVY_HIST_G2_10);

    rMJCDsrSta.i2PPCRgnMVX[0] = (IO32ReadFldAlign(MJC_PPC_GMV_0, MJC_STA_PPC_RGN_MV_0) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[1] = (IO32ReadFldAlign(MJC_PPC_GMV_0, MJC_STA_PPC_RGN_MV_1) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[2] = (IO32ReadFldAlign(MJC_PPC_GMV_1, MJC_STA_PPC_RGN_MV_2) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[3] = (IO32ReadFldAlign(MJC_PPC_GMV_1, MJC_STA_PPC_RGN_MV_3) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[4] = (IO32ReadFldAlign(MJC_PPC_GMV_2, MJC_STA_PPC_RGN_MV_4) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[5] = (IO32ReadFldAlign(MJC_PPC_GMV_2, MJC_STA_PPC_RGN_MV_5) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[6] = (IO32ReadFldAlign(MJC_PPC_GMV_3, MJC_STA_PPC_RGN_MV_6) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[7] = (IO32ReadFldAlign(MJC_PPC_GMV_3, MJC_STA_PPC_RGN_MV_7) & 0xFF80) >> 7;
    rMJCDsrSta.i2PPCRgnMVX[8] = (IO32ReadFldAlign(MJC_PPC_GMV_4, MJC_STA_PPC_RGN_MV_8) & 0xFF80) >> 7;
    rMJCDsrSta.i1PPCRgnMVY[0] =  IO32ReadFldAlign(MJC_PPC_GMV_0, MJC_STA_PPC_RGN_MV_0) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[1] =  IO32ReadFldAlign(MJC_PPC_GMV_0, MJC_STA_PPC_RGN_MV_1) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[2] =  IO32ReadFldAlign(MJC_PPC_GMV_1, MJC_STA_PPC_RGN_MV_2) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[3] =  IO32ReadFldAlign(MJC_PPC_GMV_1, MJC_STA_PPC_RGN_MV_3) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[4] =  IO32ReadFldAlign(MJC_PPC_GMV_2, MJC_STA_PPC_RGN_MV_4) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[5] =  IO32ReadFldAlign(MJC_PPC_GMV_2, MJC_STA_PPC_RGN_MV_5) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[6] =  IO32ReadFldAlign(MJC_PPC_GMV_3, MJC_STA_PPC_RGN_MV_6) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[7] =  IO32ReadFldAlign(MJC_PPC_GMV_3, MJC_STA_PPC_RGN_MV_7) & 0x7F;
    rMJCDsrSta.i1PPCRgnMVY[8] =  IO32ReadFldAlign(MJC_PPC_GMV_4, MJC_STA_PPC_RGN_MV_8) & 0x7F;

    rMJCDsrSta.i2PPCGlbMVX = IO32ReadFldAlign(MJC_PPC_GMV_4, MJC_STA_PPC_GLB_MV) & 0xFF80;
    rMJCDsrSta.i2PPCGlbMVX = (rMJCDsrSta.i2PPCGlbMVX)>> 7;
    rMJCDsrSta.i1PPCGlbMVY =  IO32ReadFldAlign(MJC_PPC_GMV_4, MJC_STA_PPC_GLB_MV) & 0x7F;
    rMJCDsrSta.i1PPCGlbMVY = rMJCDsrSta.i1PPCGlbMVY | ((rMJCDsrSta.i1PPCGlbMVY&0x40)<<1);   //???
    rMJCDsrSta.u2PPCGmvBlkCnt = IO32ReadFldAlign(MJC_PPC_GMV_5, MJC_STA_PPC_GMV_BLK_CNT);

    rMJCDsrSta.i1ME2GlbMVY = IO32ReadFldAlign(MJC_STA_GMV_BI, MJC_ME2_STA_GMV_Y_FIX);   //center to frame half pixel
    rMJCDsrSta.i2ME2GlbMVX = IO32ReadFldAlign(MJC_STA_GMV_BI, MJC_ME2_STA_GMV_X);   //center to frame half pixel
    rMJCDsrSta.i2ME2GlbMVX = (rMJCDsrSta.i2ME2GlbMVX & 0x100) ? (-(((~rMJCDsrSta.i2ME2GlbMVX) + 1) & 0x1ff)) : rMJCDsrSta.i2ME2GlbMVX;
    rMJCDsrSta.i1ME2GlbMVY = rMJCDsrSta.i1ME2GlbMVY>>1;
    rMJCDsrSta.i2ME2GlbMVX = rMJCDsrSta.i2ME2GlbMVX>>1;

    vIO32WriteFldAlign(0xF0036F00, 1, Fld( 1, 28, AC_MSKDW));   //???

    rBndFgSta.u1FmCntU[0] =  IO32ReadFldAlign(MJC_BND_FG_U0, MJC_STA_BND_FG_FMCNT_U0); // For padding
    rBndFgSta.u1FmCntU[1] =  IO32ReadFldAlign(MJC_BND_FG_U1, MJC_STA_BND_FG_FMCNT_U1);
    rBndFgSta.u1FmCntU[2] =  IO32ReadFldAlign(MJC_BND_FG_U2, MJC_STA_BND_FG_FMCNT_U2);
    rBndFgSta.u1FmCntD[0] =  IO32ReadFldAlign(MJC_BND_FG_D0, MJC_STA_BND_FG_FMCNT_D0);
    rBndFgSta.u1FmCntD[1] =  IO32ReadFldAlign(MJC_BND_FG_D1, MJC_STA_BND_FG_FMCNT_D1);
    rBndFgSta.u1FmCntD[2] =  IO32ReadFldAlign(MJC_BND_FG_D2, MJC_STA_BND_FG_FMCNT_D2);

    vIO32WriteFldAlign(0xF0036F00, 0, Fld( 1, 28, AC_MSKDW));

#if 1
    // u4gain, PPC area 128*128
    u4RGain = ((128*128)>>6);	// TBD: need modify for HPR/FPR 
    for (k=0;k<2;k++) //k=0 move down; k=1 move up
    {
        for (i=0;i<3;i++) 
        {
            it = (k==0) ? i : (6-i); 
            ic = (k==0) ? (6-i) : i;
            
            u2B1T[i][k] = rMJCDsrSta.u2MVYBlkHistG1[it]; 
            u2B2T[i][k] = rMJCDsrSta.u2MVYBlkHistG2[it];
            u2B1C[i][k] = rMJCDsrSta.u2MVYBlkHistG1[ic]; 
            u2B2C[i][k] = rMJCDsrSta.u2MVYBlkHistG2[ic];
        }
        for (i=0;i<5;i++) 
        {
            it = (k==0) ? i : (10-i); 
            ic = (k==0) ? (10-i) : i;
            u2R1T[i][k] = rMJCDsrSta.u1MVYRgnHistG1[it]*u4RGain; 
            u2R2T[i][k] = rMJCDsrSta.u1MVYRgnHistG2[it]*u4RGain;
            u2R1C[i][k] = rMJCDsrSta.u1MVYRgnHistG1[ic]*u4RGain; 
            u2R2C[i][k] = rMJCDsrSta.u1MVYRgnHistG2[ic]*u4RGain;
        }
    }
    #endif

#endif
}

/* -------------------------------------------------------------------------- */

void vDrvDSRTSMT(void)
{
    UINT32 u4BlkCntThr;			// TBD: need adaptive according to frame size
    UINT32 k;


    INT32 i4MD1en=0; // tracking block size large than 1/4 of picture size
    
    u4BlkCntThr = IO32ReadFldAlign(MJC_DSR_CTRL_04, MJC_DSR_EVAL_THR1);			// TBD: need adaptive according to frame size
   
    for (k=0;k<2;k++) //k=0 move down; k=1 move up
    {
        // take side - motion tracking
        u4TSMT[k]=0;
        if (((u2B1T[0][k]+u2B1T[1][k])>((u2B1C[0][k]+u2B1C[1][k])*2))&&
        	((u2R1T[0][k]+u2R1T[1][k]+u2R1T[2][k]+u2R1T[3][k])>(u2R1C[0][k]+u2R1C[1][k]+u2R1C[2][k]+u2R1C[3][k]))&&
        	((u2B1T[0][k]+u2B1T[1][k])>u4BlkCntThr))	// C12 > TH1
        {
        	
            if (((u2B1T[0][k]+u2B1T[1][k])>((u4Width*u4Height>>6)>>2))|| // 1/4 blocks 
            ((u2R1T[0][k]+u2R1T[1][k]+u2R1T[2][k]+u2R1T[3][k])>((u4Width*u4Height>>6)>>2))) 
            {
                i4MD1en = 1;
            }
        	
            if (i4MD1en) 
            {
                if (((u2R1T[0][k]+u2R1T[1][k])>(u2B1C[0][k]+u2B1C[1][k]))&&
                ((u2R1T[0][k]+u2R1T[1][k])>(u2R1C[0][k]+u2R1C[1][k]+u2R1C[2][k]+u2R1C[3][k]))&&
                ((u2R1T[0][k])>u4BlkCntThr)) // C23 > TH2
                {
                    u4TSMT[k]=2;
                }
                else if (((u2R1T[0][k]+u2R1T[1][k])>(u2B1C[0][k]))&&
                            ((u2R1T[0][k]+u2R1T[1][k])>(u2R1C[0][k]+u2R1C[1][k]+u2R1C[2][k]))&&
                            ((u2R1T[0][k]+u2R1T[1][k])>u4BlkCntThr)) 
                {
                    u4TSMT[k]=1;
                }
                else if (((u2B1C[0][k]+u2B1C[1][k])<(u4BlkCntThr/2))&&
                        	((u2B1T[0][k])>(u2B1C[0][k]+u2B1C[1][k]))&&
                        	((u2B1T[0][k])>u4BlkCntThr))
                {
                    u4TSMT[k]=1;
                }
            }
        }
    }
}

void vDrvDSRMBMT(void)
{

    UINT32 k;
    UINT32 u2EvalThr2;

    for (k=0;k<2;k++) //k=0 move down; k=1 move up
    {
        u4MBMT[k]=0;
        u2EvalThr2 = IO32ReadFldAlign(MJC_DSR_CTRL_04, MJC_DSR_EVAL_THR2);
        
        if ((k==0) ? (_i1DSROfst<0) : (_i1DSROfst>0)) 
        {
            if (((u2B1C[0][k])>((u2B1T[0][k]+u2B1T[1][k]+u2B1T[2][k])*1+u2EvalThr2))&& // TH10
            	((u2R1C[0][k]+u2R1C[1][k])>(u2R1T[0][k]+u2R1T[1][k]+u2R1T[2][k]+u2R1T[3][k])))	
            {
        		u4MBMT[k]=1;
            }
            
            /*
            if (((u2B1C[0]+u2B1C[1])>((u2B1T[0]+u2B1T[1])*1+u2EvalThr2))&&
            	((u2R1C[0]+u2R1C[1]+u2R1C[2]+u2R1C[3])>(u2R1T[0]+u2R1T[1]+u2R1T[2]+u2R1T[3]))&&
            	1)	{
            		u4MBMT[k]=1;
            }	
            */
        }
    }
}

void vDrvDSRTSHR(void)
{
    UINT32 k;

    INT32 i4MD1en, i4MD2en;
    UINT32 u2EvalThr3 = IO32ReadFldAlign(MJC_DSR_CTRL_05, MJC_DSR_EVAL_THR3);
    UINT32 u2EvalThr2 = IO32ReadFldAlign(MJC_DSR_CTRL_04, MJC_DSR_EVAL_THR2);
    
    i4MD1en = 0;
    i4MD2en = 0;

    for (k=0;k<2;k++) //k=0 move down; k=1 move up
    {
        u4TSHR[k]=0;
        if (((u2B2T[0][k]+u2B2T[1][k])>((u2B2C[0][k]+u2B2C[1][k])*2))&&
        ((u2R2T[0][k]+u2R2T[1][k]+u2R2T[2][k]+u2R2T[3][k])>=(u2R2C[0][k]+u2R2C[1][k]+u2R2C[2][k]+u2R2C[3][k]))&&
        ((u2B2T[0][k])>u2EvalThr3)&&
        ((u2B2T[0][k]+u2B2T[1][k])>((u2B2C[0][k]+u2B2C[1][k])+u2EvalThr2))&&
        ((u2B1C[0][k]+u2B1C[1][k])<u2EvalThr3))	
        {

            // me3 background halo reduction 
            // me2 forward / backward

            if (((u2B2T[0][k]+u2B2T[1][k])>((u4Width*u4Height>>6)>>1))||
                ((u2R2T[0][k]+u2R2T[1][k]+u2R2T[2][k]+u2R2T[3][k])>((u4Width*u4Height>>6)>>1)))
            {
                i4MD1en = 1;
            }

            // me3 foreground halo reduction (special for lg_windoshutter)
            if ((ABS(rMJCDsrSta.i2PPCGlbMVX)<4)&&(ABS(rMJCDsrSta.i2ME2GlbMVX)<4)&&(ABS(rMJCDsrSta.i1ME2GlbMVY)<8)&&
                (rMJCDsrSta.u2PPCGmvBlkCnt > (((u4Width*u4Height>>6)*2)>>2))) 
            {
                i4MD2en=1;
            }

            if (i4MD1en || i4MD2en) 
            {
                if (((u2B1C[0][k]+u2B1C[1][k]+u2B1C[2][k])<u2EvalThr3)&&
                ((u2B2T[0][k]+u2B2T[1][k]+u2B2T[2][k])>((u2B2C[0][k]+u2B2C[1][k]+u2B2C[2][k])*2))) 
                {
                    u4TSHR[k]= i4MD2en ? 2 : 1;
                }
                else
                {
                    u4TSHR[k]=1;
                }
            }
        }
    }
}

void vDrvDSRMBHR(void)
{

    UINT32 k;
    UINT16 u2EvalThr2 = IO32ReadFldAlign(MJC_DSR_CTRL_04, MJC_DSR_EVAL_THR2);
    UINT16 u2EvalThr4 = IO32ReadFldAlign(MJC_DSR_CTRL_05, MJC_DSR_EVAL_THR4);

    for (k=0;k<2;k++) //k=0 move down; k=1 move up
    {
        u4MBHR[k]=0;
        if ((k==0) ? (_i1DSROfst<0) : (_i1DSROfst>0)) 
        {

            //if (((u2B2C[0]+u2B2C[1])>(u2B2T[0]+u2B2T[1]))) {
            if (((u2B2C[0][k])>(u2B2T[0][k]+u2B2T[1][k]+u2B2T[2][k])+u2EvalThr2)) // TH11
            {
                u4MBHR[k]=1;
            }
            else if (((u2B2T[0][k]+u2B2T[1][k]+u2B2T[2][k])<u2EvalThr4)) // TH12
            {
                u4MBHR[k]=1;
            }
            
            /*
            if (((u2B2T[0]+u2B2T[1])<64)&&((u2R2T[0]+u2R2T[1]+u2R2T[2]+u2R2T[3])<=256)) 
            {
                if (((u2B2C[0]+u2B2C[1])>64)) 
                {
                	    u4MBHR[k]=1;
                }
            }
            */

        }
    }    
}
// ISR main loop
void vDrvMJCDsrFrmStr(void) 
{
    UINT32 k;
    UINT32 u4MEMCDen;
    UINT32 u4MEMCNum;

#if 1
     // sw register
    //UINT8 u1VarRst = 1;
    UINT8 u160or120Sel = 1;
    UINT8 u1DSRPyeofstEn = 1;
    UINT8 u1DSRPyeofstFz = 0;	// force zero
    UINT8 u1DSRMVPadEn = 1;
    UINT8 u1DSRMVPadFz = 0;
    UINT8 u1DSRBndFGEn = 0;
    UINT8 u1DSRTSmtEn = 1;
    UINT8 u1DSRMBmtEn = 1;
    UINT8 u1DSRTShrEn = 1;
    UINT8 u1DSRMBhrEn = 1;

    UINT8 u1PtFrmCntBnd = 30;
    UINT8 u1PrintfEn = 0;
    UINT8 u1VcodeSel1 = 0;
    UINT8 u1VcodeSel2 = 0;

    UINT8 u1OsdHCt = 8; // *8
    UINT8 u1OsdVCt = 90; // *4
    UINT8 u1OsdWD = 6; // *4
    UINT8 u1OsdHT = 6; // *4
    UINT8 u1OsdVOfst = 8; // *4
    UINT8 u1OsdDbgEn = 0;
#endif    
    // general variable
    UINT32 u4PairFirstD1; 

    INT8 i1MVYRefBnd = 0;
    INT8 i1AbsME2GMVY =0;
    INT8 i1PadHt = 0;
    INT8 i1PadFndFGDetEnU = 0;
    INT8 i1PadFndFGDetEnD = 0;
    INT8 i1PadBndFGFlgU = 0;
    INT8 i1PadBndFGFlgD = 0;
    
    INT8 i1OfstBound = 4;

    INT32 i4PPCGlbMVY;				
    INT32 i4ME2GlbMVY;
    UINT16 u2OSDBndLf,u2OSDBndRt,u2OSDBndUp,u2OSDBndDn;
    UINT8 u1OSDIntpMode;
    UINT16 u2VcodeVal=0;
    UINT8  u1Vcode1=0,u1Vcode2=0,u1Vcode3=0,u1Vcode4=0;
    
#ifdef _C_MODEL_
	u4Width=MJC_common::Width;
	u4Height=MJC_common::Height;
#else
    u4MEMCDen = IO32ReadFldAlign(MJC_STA_FM_02, MJC_STA_MEMC_DEN_HW);
    u4MEMCNum = IO32ReadFldAlign(MJC_STA_FM_02, MJC_STA_MEMC_NUM_HW);
#endif

   

#ifdef _C_MODEL_
#else
    //test = IO32ReadFldAlign(0xF0036F20, Fld( 8,  0, AC_MSKDW));
    //vIO32WriteFldAlign(0xF0036F00, 0, Fld( 1, 28, AC_MSKDW));
    
    // HW status
    vDrvMJCDsrGetSta();
#if 0
    u1VarRst = IO32ReadFldAlign(MJC_DSR_CTRL_00, MJC_DSR_VAR_RST);
	
    if (u1VarRst==1) 
    {
        vIO32WriteFldAlign(MJC_DSR_CTRL_00, u160or120Sel, MJC_DSR_60_120_SEL);

        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRPyeofstEn, MJC_DSR_PYEOFST_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRPyeofstFz, MJC_DSR_PYEOFST_FZ);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRMVPadEn, MJC_DSR_MVPAD_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRMVPadFz, MJC_DSR_MVPAD_FZ);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRBndFGEn, MJC_DSR_BNDFG_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRTSmtEn, MJC_DSR_TSMT_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRMBmtEn, MJC_DSR_MBMT_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRTShrEn, MJC_DSR_TSHR_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_01, u1DSRMBhrEn, MJC_DSR_MBHR_EN);

        vIO32WriteFldAlign(MJC_DSR_CTRL_02, u1PtFrmCntBnd, MJC_DSR_P_FRM_CNT_BND);
        vIO32WriteFldAlign(MJC_DSR_CTRL_02, u1PrintfEn, MJC_DSR_PRINTF_EN);
        vIO32WriteFldAlign(MJC_DSR_CTRL_02, u1VcodeSel1, MJC_DSR_VCODE_SEL_1);
        vIO32WriteFldAlign(MJC_DSR_CTRL_02, u1VcodeSel2, MJC_DSR_VCODE_SEL_2);

        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdHCt, MJC_DSR_OSD_H_CT);
        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdVCt, MJC_DSR_OSD_V_CT);
        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdWD, MJC_DSR_OSD_WD);
        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdHT, MJC_DSR_OSD_HT);
        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdVOfst, MJC_DSR_OSD_V_OFST);
        vIO32WriteFldAlign(MJC_DSR_CTRL_03, u1OsdDbgEn, MJC_DSR_OSD_DBG_EN);

        vIO32WriteFldAlign(MJC_DSR_CTRL_04, u2EvalThr1, MJC_DSR_EVAL_THR1);
        vIO32WriteFldAlign(MJC_DSR_CTRL_04, u2EvalThr2, MJC_DSR_EVAL_THR2);
        vIO32WriteFldAlign(MJC_DSR_CTRL_05, u2EvalThr3, MJC_DSR_EVAL_THR3);
        vIO32WriteFldAlign(MJC_DSR_CTRL_05, u2EvalThr4, MJC_DSR_EVAL_THR4);		
    }
    else 
#endif
    {
        u160or120Sel = IO32ReadFldAlign(MJC_DSR_CTRL_00, MJC_DSR_60_120_SEL);

        u1DSRPyeofstFz = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_PYEOFST_FZ);
        u1DSRPyeofstEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_PYEOFST_EN);
        u1DSRMVPadEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_MVPAD_EN);
        u1DSRMVPadFz = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_MVPAD_FZ);
        u1DSRBndFGEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_BNDFG_EN);
        u1DSRTSmtEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_TSMT_EN);
        u1DSRMBmtEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_MBMT_EN);
        u1DSRTShrEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_TSHR_EN);
        u1DSRMBhrEn = IO32ReadFldAlign(MJC_DSR_CTRL_01, MJC_DSR_MBHR_EN);

        u1PtFrmCntBnd = IO32ReadFldAlign(MJC_DSR_CTRL_02, MJC_DSR_P_FRM_CNT_BND);
        u1PrintfEn = IO32ReadFldAlign(MJC_DSR_CTRL_02, MJC_DSR_PRINTF_EN);
        u1VcodeSel1 = IO32ReadFldAlign(MJC_DSR_CTRL_02, MJC_DSR_VCODE_SEL_1);
        u1VcodeSel2 = IO32ReadFldAlign(MJC_DSR_CTRL_02, MJC_DSR_VCODE_SEL_2);

        u1OsdHCt = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_H_CT);
        u1OsdVCt = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_V_CT);
        u1OsdWD = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_WD);
        u1OsdHT = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_HT);
        u1OsdVOfst = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_V_OFST);
        u1OsdDbgEn = IO32ReadFldAlign(MJC_DSR_CTRL_03, MJC_DSR_OSD_DBG_EN);

    }
#endif


#ifdef _C_MODEL_
	u4PairFirstD1 = MJC_common::csDSRFWIF.pair_first_d1; 
#else
    u4PairFirstD1 = 0; 
	if (u160or120Sel==1) 
	{
		if ((u4MEMCNum==1)||(u4MEMCNum==3)) 
		{
		    u4PairFirstD1 = 1; 
		}
	}
	else 
	{
		if ((u4MEMCNum==2)||(u4MEMCNum==3)) 
		{
		    u4PairFirstD1 = 1; 
		}
	}
#endif
    LOG(6, "u4PairFirstD1 = %d\n", u4PairFirstD1);

#if 1   //need to move to original padding function
	// adaptive mv padding
    i1MVYRefBnd = (24+ABS(_i1DSROfst)*4)/2;
    i1AbsME2GMVY = ABS(rMJCDsrSta.i1ME2GlbMVY);
    i1PadHt = 0;
    
    if (i1AbsME2GMVY >= i1MVYRefBnd) 
    {
        i1PadHt = (i1AbsME2GMVY>>3)+3;
    }
    i1PadHt = (i1PadHt>6) ? 6 : i1PadHt;


    if ((i1PadHt>0)&&(u1DSRBndFGEn==1)) // not use
    {
        i1PadFndFGDetEnU = 1;
        i1PadFndFGDetEnD = 1;
    }


    if (i1PadFndFGDetEnU) 
    {
        if ((rBndFgSta.u1FmCntU[0]>=32)||(rBndFgSta.u1FmCntU[1]>=32)||(rBndFgSta.u1FmCntU[2]>=32)) 
        {
            i1PadBndFGFlgU = 1;
        }
    }
    if (i1PadFndFGDetEnD) 
        {
        if ((rBndFgSta.u1FmCntD[0]>=32)||(rBndFgSta.u1FmCntD[1]>=32)||(rBndFgSta.u1FmCntD[2]>=32)) 
        {
            i1PadBndFGFlgD = 1;
        }
    }
#endif

	// ========================
        // take side
        vDrvDSRTSMT();

        // move back - motion tracking
        vDrvDSRMBMT();
    
        // take side - halo reduction
        vDrvDSRTSHR();

        // move back - halo reduction
        vDrvDSRMBHR();

	
	for (k=0;k<2;k++) 
	{
		if (u1DSRTSmtEn==0) u4TSMT[k] = 0;
		if (u1DSRTShrEn==0) u4TSHR[k] = 0;
		if (u1DSRMBmtEn==0) u4MBMT[k] = 0;
		if (u1DSRMBhrEn==0) u4MBHR[k] = 0;
	}


    
    if (u4PairFirstD1) 
    {
        // TSMT
        if ((u4TSMT[0]>0)&&(u4TSMT[1]==0)) 
        {
            _i1DSROfst = _i1DSROfst-u4TSMT[0];
        }
        else if ((u4TSMT[1]>0)&&(u4TSMT[0]==0)) 
        {
            _i1DSROfst = _i1DSROfst+u4TSMT[1];
        }
        // MBMT
        else if ((u4MBMT[0]==1)&&(u4MBMT[1]==0)) 
        {
            _i1DSROfst = _i1DSROfst+1;
        }
        else if ((u4MBMT[0]==0)&&(u4MBMT[1]==1)) 
        {
            _i1DSROfst = _i1DSROfst-1;
        }
	  // TSHR
        else if ((u4TSHR[0]>0)&&(u4TSHR[1]==0)) 
        {
            //if (abs(_i1DSROfst)<=3) // ???
            {
                i1OfstBound = 3;
            }
		_i1DSROfst = _i1DSROfst-u4TSHR[0];
        }
        else if ((u4TSHR[1]>0)&&(u4TSHR[0]==0)) 
        {
            //if (abs(_i1DSROfst)<=3) 
            {
                i1OfstBound = 3;
            }    
            _i1DSROfst = _i1DSROfst+u4TSHR[1];
        }
        // MBHR
        else if ((u4MBHR[0]==1)&&(u4MBHR[1]==0)) 
        {
            _i1DSROfst = _i1DSROfst+1;
        }
        else if ((u4MBHR[0]==0)&&(u4MBHR[1]==1)) 
        {
            _i1DSROfst = _i1DSROfst-1;
        }
    }

	// clipping
	if (_i1DSROfst>=i1OfstBound) 
	{
	    _i1DSROfst = i1OfstBound;
	}    
	else if (_i1DSROfst<=-i1OfstBound)
	{
	     _i1DSROfst = -i1OfstBound;
	}    

    // histogram analyze
	
	
	
	/*
    // use ppc_gmv only
    if (rMJCDsrSta.i1PPCGlbMVY>=16) {
        _i1DSROfst = ((rMJCDsrSta.i1PPCGlbMVY-16)>>3)+1;
    }
    else if (rMJCDsrSta.i1PPCGlbMVY<=-16) {
        _i1DSROfst = ((rMJCDsrSta.i1PPCGlbMVY+16)>>3)-1;
    }
	else {
        _i1DSROfst = 0;
	
	*/

#ifdef _C_MODEL_
    if (u1DSRPyeofstEn==1) MJC_common::c_dsr_pye_ofst = _i1DSROfst;

	if (u1DSRMVPadEn==1) {
		if(i1PadHt>0) {
			MJC_common::c_mv_csr.c_mv_pad_up_en = 1;
			MJC_common::c_mv_csr.c_mv_pad_dn_en = 1;
			MJC_common::c_mv_csr.c_mv_pad_up_ht = i1PadHt;
			MJC_common::c_mv_csr.c_mv_pad_dn_ht = i1PadHt;	
		}
		else {
			MJC_common::c_mv_csr.c_mv_pad_up_en = 0;
			MJC_common::c_mv_csr.c_mv_pad_dn_en = 0;
			MJC_common::c_mv_csr.c_mv_pad_up_ht = 0;
			MJC_common::c_mv_csr.c_mv_pad_dn_ht = 0;	
		}
	}
#else
    if (u1DSRPyeofstEn==1) 
    {
        _i1DSROfst = (u1DSRPyeofstFz==1) ? 0 : _i1DSROfst;
        vIO32WriteFldAlign(MJC_FM_17, _i1DSROfst, MJC_DSR_PYE_OFST);
    }	
	
    if (u1DSRMVPadEn==1) 
    {
        i1PadHt = (u1DSRMVPadFz==1) ? 0 : i1PadHt;

        if(i1PadFndFGDetEnU>0) 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 1, MJC_MV_PAD_BND_FG_DET_UP_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, (i1PadHt+1), MJC_MV_PAD_UP_HT_FG);
        }
        else 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 0, MJC_MV_PAD_BND_FG_DET_UP_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, 0, MJC_MV_PAD_UP_HT_FG);
        }
        if(i1PadFndFGDetEnD>0) 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 1, MJC_MV_PAD_BND_FG_DET_DN_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, (i1PadHt+1), MJC_MV_PAD_DN_HT_FG);
        }
        else 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 0, MJC_MV_PAD_BND_FG_DET_DN_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, 0, MJC_MV_PAD_DN_HT_FG);
        }

        if((i1PadHt>0)&&(i1PadBndFGFlgU==0)) 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 1, MJC_MV_PAD_UP_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, i1PadHt, MJC_MV_PAD_UP_HT);
        }
        else 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 0, MJC_MV_PAD_UP_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, 0, MJC_MV_PAD_UP_HT);
        }
        if((i1PadHt>0)&&(i1PadBndFGFlgD==0)) 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 1, MJC_MV_PAD_DN_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, i1PadHt, MJC_MV_PAD_DN_HT);
        }
        else 
        {
            vIO32WriteFldAlign(MJC_CA_PAD, 0, MJC_MV_PAD_DN_EN);
            vIO32WriteFldAlign(MJC_MV_PAD, 0, MJC_MV_PAD_DN_HT);
        }
        
    }
#endif


#ifdef _C_MODEL_
    MJC_common::c_dsr_pye_ofst = _i1DSROfst;
	printf ("vDrvMJCDsrFrmStr: c_dsr_pye_ofst[%d]\n",MJC_common::c_dsr_pye_ofst);
	printf ("vDrvMJCDsrFrmStr: abs_gmv[%d],c_mv_pad_up_en[%d],up_ht[%d]\n",i1AbsME2GMVY,MJC_common::c_mv_csr.c_mv_pad_up_en,MJC_common::c_mv_csr.c_mv_pad_up_ht);
	printf ("vDrvMJCDsrFrmStr: u4TSMT(0,1)=[%d,%d]\n",u4TSMT[0],u4TSMT[1]);
	printf ("vDrvMJCDsrFrmStr: u4MBMT(0,1)=[%d,%d]\n",u4MBMT[0],u4MBMT[1]);
	printf ("vDrvMJCDsrFrmStr: u4TSHR(0,1)=[%d,%d]\n",u4TSHR[0],u4TSHR[1]);
	printf ("vDrvMJCDsrFrmStr: u4MBHR(0,1)=[%d,%d]\n",u4MBHR[0],u4MBHR[1]);
#else
	_u1PtFrmCnt = _u1PtFrmCnt+1;
	if (_u1PtFrmCnt > u1PtFrmCntBnd) {
		_u1PtFrmCnt = 0;
	}

	if ((u1PrintfEn&0x1)&&(_u1PtFrmCnt==0)) {
		LOG(3, "[%d,%d], DSROfst[%x] PadHt[%x],",u4MEMCNum,u4MEMCDen,_i1DSROfst,i1PadHt);
		LOG(3, "TSMT[%x,%x],MBMT[%x,%x],TSHR[%x,%x],MBHR[%x,%x],",
						u4TSMT[0],u4TSMT[1],u4MBMT[0],u4MBMT[1],u4TSHR[0],u4TSHR[1],u4MBHR[0],u4MBHR[1]);
		i4PPCGlbMVY = (rMJCDsrSta.i1PPCGlbMVY>0) ? (rMJCDsrSta.i1PPCGlbMVY) : -ABS(rMJCDsrSta.i1PPCGlbMVY);				
		i4ME2GlbMVY = (rMJCDsrSta.i1ME2GlbMVY>0) ? (rMJCDsrSta.i1ME2GlbMVY) : -ABS(rMJCDsrSta.i1ME2GlbMVY);				
		LOG(3, "GMVY,PPC[%d],ME2[%d],U[%d],D[%d]\n",i4PPCGlbMVY,i4ME2GlbMVY,i1PadBndFGFlgU,i1PadBndFGFlgD);
	}
#endif


// debug ink section
#ifdef _C_MODEL_
#else
		// OSD
						
		if (u1OsdDbgEn&0x1) {
			u2OSDBndLf = u1OsdHCt*8;
			u2OSDBndRt = u2OSDBndLf+u1OsdWD*4;
			u2OSDBndUp = (_i1DSROfst<0) ? (u1OsdVCt*4 + ABS(_i1DSROfst)*u1OsdVOfst*4) : (u1OsdVCt*4 - ABS(_i1DSROfst)*u1OsdVOfst*4);
			u2OSDBndDn = u2OSDBndUp+u1OsdHT*4;
		}
		else {
			u2OSDBndLf = 0; u2OSDBndRt = 0; u2OSDBndUp = 0; u2OSDBndDn = 0;
		}
						
		if (_i1DSROfst>0) { u1OSDIntpMode = 0x3F; }
		else if (_i1DSROfst<0) { u1OSDIntpMode = 0x3E; }
		else  { u1OSDIntpMode = 0x3D; }

		vIO32WriteFldAlign(MJC_MC_16, u1OSDIntpMode, MJC_MC_INTP_OSD);
		vIO32WriteFldAlign(MJC_MC_16, u2OSDBndRt, MJC_OSD_BOUND_RIGHT_0);
		vIO32WriteFldAlign(MJC_MC_16, u2OSDBndLf, MJC_OSD_BOUND_LEFT_0);
		vIO32WriteFldAlign(MJC_MC_17, u2OSDBndDn, MJC_OSD_BOUND_DOWN_0);
		vIO32WriteFldAlign(MJC_MC_17, u2OSDBndUp, MJC_OSD_BOUND_UP_0);

		// VCODE
		
		if ((u1VcodeSel1>0)||(u1VcodeSel2>0)) {
			if (u1VcodeSel1==1) {
				u1Vcode1 = (_i1DSROfst>0) ? _i1DSROfst : (8|(ABS(_i1DSROfst)));
				u1Vcode2 = i1PadHt;
			}
			if (u1VcodeSel2==1) {
				u1Vcode3 = (u4MBMT[1]<<3)|(u4MBMT[0]<<2)|(u4TSMT[1]<<1)|(u4TSMT[0]<<0);
				u1Vcode4 = (u4MBHR[1]<<3)|(u4MBHR[0]<<2)|(u4TSHR[1]<<1)|(u4TSHR[0]<<0);
			}
			u2VcodeVal=(u1Vcode4<<12)|(u1Vcode3<<8)|(u1Vcode2<<4)|(u1Vcode1<<0);

			//vIO32WriteFldAlign(MJC_MC_06,   1, MJC_VDB_MODU_SEL);
			//vIO32WriteFldAlign(MJC_MC_06,0x40, MJC_VDB_INFO_SEL);
			vIO32WriteFldAlign(MJC_MC_08,u2VcodeVal, MJC_VDB_CODE_VAL);
		}
		else {
			//vIO32WriteFldAlign(MJC_MC_06, 0, MJC_VDB_MODU_SEL);
			//vIO32WriteFldAlign(MJC_MC_06, 0, MJC_VDB_INFO_SEL);
		}
#endif

}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */

