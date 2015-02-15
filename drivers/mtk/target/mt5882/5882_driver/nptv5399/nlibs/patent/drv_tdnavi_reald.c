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
 * $RCSfile: drv_mjc_int.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/

#include "x_assert.h"
#include "hw_tdnavi.h"

#include "drv_tdtv_feature.h"
#include "drv_tdnavi.h"
#include "vdo_misc.h"
#include "sv_const.h"


/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
 
void vDrvRealDSetDetectMode(UINT8 u1VdpId, UINT8 u1DetMode);
UINT8 u1DrvRealDGetDetectMode(UINT8 u1VdpId);
void vDrvRealDSetInDiffThr(UINT8 u1VdpId, UINT8 u1InThr);
void vDrvRealDSetOutDiffThr(UINT8 u1VdpId, UINT8 u1OutThr);
UINT16 u2DrvRealDGetQuaSta(UINT8 u1VdpId);
void u2DrvRealDQuery(void);


static const REGTBL_T REALD_INIT[] =
{
    {PRE_REALD_MAIN_00, 0x18618618, 0xFFFFFFFF},
    {PRE_REALD_MAIN_01, 0x61861868, 0xFFFFFFFF},
    {PRE_REALD_MAIN_02, 0x61861861, 0xFFFFFFFF},
    {PRE_REALD_MAIN_03, 0x86186100, 0xFFFFFFC0},
    {PRE_REALD_MAIN_04, 0x374B0040, 0xFFFFC171},
    {PRE_REALD_PIP_00, 0x18618618, 0xFFFFFFFF},
    {PRE_REALD_PIP_01, 0x61861868, 0xFFFFFFFF},
    {PRE_REALD_PIP_02, 0x61861861, 0xFFFFFFFF},
    {PRE_REALD_PIP_03, 0x86186100, 0xFFFFFFC0},
    {PRE_REALD_PIP_04, 0x374B0040, 0xFFFFC171},
    {REGTBL_END, 0x00000000, 0x00000000}
};

UINT8 g_u1RealDInDiffThr[2] = {75, 75};
UINT8 g_u1RealDOutDiffThr[2] = {55, 55};
UINT8 g_u1RealDInFrmThr[2] = {15, 15};
UINT8 g_u1RealDOutFrmThr[2] = {3, 3};
UINT8 g_u1RealDInFrmCnt[2] = {0, 0};
UINT8 g_u1RealDOutFrmCnt[2] = {0, 0};
UINT16 g_u2RealDInQuaThr[2] = {1800, 1800}; // depend on Input Width 
UINT16 g_u2RealDOutQuaThr[2] = {1800, 1800}; // depend on Input Width
UINT16 g_u2RealDInQuaThrRatio = 200; // 200 means 200/256
UINT16 g_u2RealDOutQuaThrRatio = 200; // 200 means 200/256

UINT8 g_u1RealDDetCurr[2] = {0, 0};
UINT8 g_u1RealDAdaptEn[2] = {1, 1};

UINT16 g_u2RealDPrint = 100;
UINT8 g_u1RealDTrigger = 0;


void vDrvRealDSetSourceMode(UINT8 u1VdpId)
{
    UINT8 u1RealDSrcMode, u1SrcTiming;
    UINT32 u4Width;
    
    u1SrcTiming = bDrvVideoGetTiming(u1VdpId);

    switch (u1SrcTiming)
    {
    case MODE_720p_24:
    case MODE_720p_25:    
    case MODE_720p_30:        
    case MODE_720p_50:
    case MODE_720p_60:
    	u1RealDSrcMode = 0;
		break;
    case MODE_1080i_48:
    case MODE_1080i_50:
    case MODE_1080i:
    	u1RealDSrcMode = 1;
		break;
    case MODE_1080p_24:
    case MODE_1080p_25:
    case MODE_1080p_30:
    case MODE_1080p_50:
    case MODE_1080p_60:
    	u1RealDSrcMode = 2;
        break;
    default:
    	u1RealDSrcMode = 3;
        break;
    
    }

	// Check 2 line, 60 segment/line, each segment delete left/right 5 pixels
    u4Width = wDrvVideoInputWidth(u1VdpId);
    g_u2RealDInQuaThr[u1VdpId]  = ((u4Width-600)*2*((UINT32)g_u2RealDInQuaThrRatio)) >> 8;
   	g_u2RealDOutQuaThr[u1VdpId] = ((u4Width-600)*2*((UINT32)g_u2RealDOutQuaThrRatio)) >> 8;


	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_REALD_MAIN_03, u1RealDSrcMode, R_UIINSRCMODE_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		vIO32WriteFldAlign(PRE_REALD_PIP_03, u1RealDSrcMode, R_UIINSRCMODE_P);
	}
}

UINT8 u1DrvRealDModeChange(UINT8 u1VdpId)
{
	if (u1VdpId >= SV_VP_NA)
	{
		return 0;
	}

	g_u1RealDInFrmCnt[u1VdpId] = 0;
	g_u1RealDOutFrmCnt[u1VdpId] = 0;
	g_u1RealDDetCurr[u1VdpId] = REALD_DETECT_2D;
	vDrvRealDSetDetectMode(u1VdpId, REALD_DETECT_2D);
	vDrvRealDSetSourceMode(u1VdpId);

	vDrvRealDSetInDiffThr(u1VdpId, g_u1RealDInDiffThr[u1VdpId]);
	vDrvRealDSetOutDiffThr(u1VdpId, g_u1RealDOutDiffThr[u1VdpId]);
	
	return 1;
}

void vDrvRealDInit(void)
{
    vDrvLoadRegTbl(REALD_INIT);
}


void vDrvRealDMainloop(UINT8 u1VdpId)
{
    UINT16 u2QuaSta;
    UINT8 u1DetectMode;
    u2QuaSta = u2DrvRealDGetQuaSta(u1VdpId);
    u1DetectMode = u1DrvRealDGetDetectMode(u1VdpId);

    if (u1VdpId >= SV_VP_NA)
    {
        return ;
    }

    if ((!u1VdpId) && g_u1RealDTrigger && (g_u2RealDPrint > 0))
    {
        LOG(0, "u1DetectMode = %d, u2QuaSta = %d\n", u1DetectMode, u2QuaSta);
        g_u2RealDPrint--;
    }


    if (g_u1RealDAdaptEn[u1VdpId])
    {
        if (REALD_DETECT_2D == g_u1RealDDetCurr[u1VdpId])
        {
            if (u2QuaSta > g_u2RealDInQuaThr[u1VdpId])
            {
                g_u1RealDInFrmCnt[u1VdpId]++;
            }
            else if (u2QuaSta < g_u2RealDInQuaThr[u1VdpId])
            {
                if (g_u1RealDInFrmCnt[u1VdpId] > 0)
                {
                    g_u1RealDInFrmCnt[u1VdpId]--;
                }
            }

            if (g_u1RealDInFrmCnt[u1VdpId] > g_u1RealDInFrmThr[u1VdpId])  // Status == RealD
            {
                g_u1RealDInFrmCnt[u1VdpId] = 0;
                g_u1RealDDetCurr[u1VdpId] = REALD_DETECT_RD;
                vDrvRealDSetDetectMode(u1VdpId, REALD_DETECT_RD);
            }
        }
        else if (REALD_DETECT_RD == g_u1RealDDetCurr[u1VdpId])
        {
            if (u2QuaSta > g_u2RealDOutQuaThr[u1VdpId])
            {
                g_u1RealDOutFrmCnt[u1VdpId]++;
            }
            else if (u2QuaSta < g_u2RealDOutQuaThr[u1VdpId])
            {
                if (g_u1RealDOutFrmCnt[u1VdpId] > 0)
                {
                    g_u1RealDOutFrmCnt[u1VdpId]--;
                }
            }

            if (g_u1RealDOutFrmCnt[u1VdpId] > g_u1RealDOutFrmThr[u1VdpId]) // Status == 2D
            {
                g_u1RealDOutFrmCnt[u1VdpId] = 0;
                g_u1RealDDetCurr[u1VdpId] = REALD_DETECT_2D;
                vDrvRealDSetDetectMode(u1VdpId, REALD_DETECT_2D);
            }
        }
    }
}

void vDrvRealDSetDetectMode(UINT8 u1VdpId, UINT8 u1DetMode)
{
	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_REALD_MAIN_04, u1DetMode, R_BIN2D3DMODE_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		vIO32WriteFldAlign(PRE_REALD_PIP_04, u1DetMode, R_BIN2D3DMODE_P);
	}	
}


UINT8 u1DrvRealDGetDetectMode(UINT8 u1VdpId)
{
	UINT8 u1DetMode = 0;
	
	if (u1VdpId == SV_VP_MAIN)
	{
		u1DetMode = IO32ReadFldAlign(PRE_REALD_MAIN_04, R_BIN2D3DMODE_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		u1DetMode = IO32ReadFldAlign(PRE_REALD_PIP_04, R_BIN2D3DMODE_P);
	}	

	return u1DetMode;
}

void vDrvRealDSetInDiffThr(UINT8 u1VdpId, UINT8 u1InThr)
{
	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_REALD_MAIN_04, u1InThr, R_UIQUITHL_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		vIO32WriteFldAlign(PRE_REALD_PIP_04, u1InThr, R_UIQUITHL_P);
	}	
}

void vDrvRealDSetOutDiffThr(UINT8 u1VdpId, UINT8 u1OutThr)
{
	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_REALD_MAIN_04, u1OutThr, R_UIDEQUITHL_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		vIO32WriteFldAlign(PRE_REALD_PIP_04, u1OutThr, R_UIDEQUITHL_P);
	}	
}


UINT8 u1DrvRealDGetCurrDetType(UINT8 u1VdpId)
{
	return g_u1RealDDetCurr[u1VdpId];
}

void u1DrvRealDMaskOnOff(UINT8 u1VdpId, UINT8 u1OnOff)
{
	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_REALD_MAIN_04, u1OnOff, R_UIMASKENABLE_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		vIO32WriteFldAlign(PRE_REALD_PIP_04, u1OnOff, R_UIMASKENABLE_P);
	}		
}


UINT16 u2DrvRealDGetQuaSta(UINT8 u1VdpId)
{
	UINT16 u2QuaSta = 0;
	
	if (u1VdpId == SV_VP_MAIN)
	{
		u2QuaSta = IO32ReadFldAlign(STA_REALD_MAIN_00, FRAME_QUA_STA_M);
	}
	else if (u1VdpId == SV_VP_PIP)
	{
		u2QuaSta = IO32ReadFldAlign(STA_REALD_PIP_00, FRAME_QUA_STA_P);
	}	

	return u2QuaSta;
}

void u2DrvRealDQuery(void)
{
	UINT8 u1VdpId;

    for (u1VdpId = 0; u1VdpId < 2; u1VdpId++)
    {    
        printf("===================== VDP:%d========================\n",u1VdpId);   
        Printf("g_u1RealDInDiffThr ...... %d\n", g_u1RealDInDiffThr[u1VdpId]);
        Printf("g_u1RealDOutDiffThr ..... %d\n", g_u1RealDOutDiffThr[u1VdpId]);
        Printf("g_u1RealDInFrmThr ....... %d\n", g_u1RealDInFrmThr[u1VdpId]);
        Printf("g_u1RealDOutFrmThr ...... %d\n", g_u1RealDOutFrmThr[u1VdpId]);
        Printf("g_u2RealDInQuaThr ....... %d\n", g_u2RealDInQuaThr[u1VdpId]);
        Printf("g_u2RealDOutQuaThr ...... %d\n", g_u2RealDOutQuaThr[u1VdpId]);
        Printf("g_u2RealDInQuaThrRatio .. %d\n", g_u2RealDInQuaThrRatio);
        Printf("g_u2RealDOutQuaThrRatio . %d\n", g_u2RealDOutQuaThrRatio);
    }
}
