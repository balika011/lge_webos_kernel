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


#include "vdo_misc.h"
#include "sv_const.h"
#include "vdp_if.h"
#include "mute_if.h"
#include "vdo_rm.h"

#include "hw_tdnavi.h"
#include "drv_tdtv_feature.h"
#include "drv_tdnavi.h"
#include "drv_di.h"
#include "drv_di_int.h"

#define SCAN_PWM_START (250/4) //Delay Depend on Panel Raising , Falling Speed

enum
{
    TTT_MODE_FS = 0,
    TTT_MODE_SBS,
    TTT_MODE_TB,
    TTT_MODE_LI,
    TTT_MODE_MAX
};

enum
{
	TTT_FS_RIGHT = 0,
    TTT_FS_LEFT 
};

enum
{
    TTT_SBS_RIGHT = 0,
    TTT_SBS_LEFT      
};

enum
{
	TTT_TB_BOTTOM = 0,
    TTT_TB_TOP    
};

enum
{
    TTT_LI_ODD= 0,   
    TTT_LI_EVEN      
};

typedef enum{
    CB_SRC_CB   =0,
    CB_SRC_VI   =1,
    CB_SRC_FS   =2,
}CB_SRC;


typedef struct {
    UINT8 u1OnOff;
    UINT16 u2SrcWidth;
    UINT16 u2SrcHeight;
    UINT8 u1X2Mode;    // 0x0:X1 mode; 0x1:X2 mode
    UINT8 u1SrcMode;    // 0x0:SBS; 0x1;FS
    UINT8 u1FSMode;
}sDQConfig;

typedef struct {
    UINT8 u1OnOff;
    UINT16 u2SrcWidth;
    CB_SRC eSrc;
}sCBConfig;

static const REGTBL_T TTT_INIT[] =
{
    {PRE_3D22D_MAIN_00, 0x78043800, 0xFFFFFFF7},
    {PRE_3D22D_MAIN_01, 0x78043800, 0xFFFFFF1A},
    {PRE_3D22D_PIP_00,  0x78043800, 0xFFFFFFF7},
    {PRE_3D22D_PIP_01,  0x78043800, 0xFFFFFF1A},
    {REGTBL_END, 0x00000000, 0x00000000}
};

TD22D_INFO gMain3DTo2DInfo, gPip3DTo2DInfo;

static UINT8 fgFldInv[VDP_NS];

UINT8 u1DrvTTTSetMode(UINT8 u1VdpId, UINT8 u1TD22DEn, UINT8 u1ModeSel, UINT8 u1MaskSel)
{
	UINT16 u2Width, u2Height;
    UINT8  u1IsIntr;
    VRM_INFO_T* inRes;

    inRes = VRMGetVRMInfo(u1VdpId, VRM_MODULE_TTT);
    	
    u1IsIntr = inRes->u2ScanMode;
    u2Width  = inRes->u2Width;
    u2Height = inRes->u2Height;

	u2Height = u1IsIntr ? (u2Height >> 1) : u2Height;
	if (u1VdpId == SV_VP_MAIN)
	{
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, 0, R_B3D22D_EN_M); // before setting 3D-to-2D parameter, disable 3D-to-2D 
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, u2Width, R_UIIMGWIDTH_M);
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, u2Height, R_UIIMGHEIGHT_M);
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, u1ModeSel, R_UI3DMODESEL_M);
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, u1MaskSel, R_BMASKSEL_M);
		vIO32WriteFldAlign(PRE_3D22D_MAIN_00, u1TD22DEn, R_B3D22D_EN_M); // after setting 3D-to-2D parameter, enable 3D-to-2D 

        if(gMain3DTo2DInfo.u1TD22DEn != u1TD22DEn)
        {
            _vDrvVideoSetMute(MUTE_MODULE_SCPOS_TV3D, u1VdpId, 6, TRUE);
        }
        
		gMain3DTo2DInfo.u2Width = u2Width;
		gMain3DTo2DInfo.u2Height = u2Height;
		gMain3DTo2DInfo.u1IsIntr = u1IsIntr;
		gMain3DTo2DInfo.u1TD22DEn = u1TD22DEn;
		gMain3DTo2DInfo.u1ModeSel = u1ModeSel;
		gMain3DTo2DInfo.u1MaskSel = u1MaskSel;       
	}
	else
	{
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, 0, R_B3D22D_EN_P); // before setting 3D-to-2D parameter, disable 3D-to-2D 
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, u2Width, R_UIIMGWIDTH_P);
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, u2Height, R_UIIMGHEIGHT_P);
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, u1ModeSel, R_UI3DMODESEL_P);
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, u1MaskSel, R_BMASKSEL_P);
		vIO32WriteFldAlign(PRE_3D22D_PIP_00, u1TD22DEn, R_B3D22D_EN_P); // after setting 3D-to-2D parameter, enable 3D-to-2D 
        
		gPip3DTo2DInfo.u2Width = u2Width;
		gPip3DTo2DInfo.u2Height = u2Height;
		gPip3DTo2DInfo.u1IsIntr = u1IsIntr;
		gPip3DTo2DInfo.u1TD22DEn = u1TD22DEn;
		gPip3DTo2DInfo.u1ModeSel = u1ModeSel;
		gPip3DTo2DInfo.u1MaskSel = u1MaskSel;
	}		

    if(((u1ModeSel == TTT_MODE_TB)&&(u1MaskSel == TTT_TB_TOP) && (SV_ON == u1TD22DEn))                                        
        ||((u1ModeSel == TTT_MODE_FS)&&(u1MaskSel == TTT_FS_LEFT) && (SV_ON == u1TD22DEn))
        )
    {
        fgFldInv[u1VdpId]=SV_ON;
    }
    else
    {
        fgFldInv[u1VdpId]=SV_OFF;
    }

    _vDrvDIFieldInverse();
	return 1;
}

UINT32 vDrvTTTIsFldInv(UINT32 u1VdpId)
{
    if(u4DrvTDTVIsDualFPI())
    {
        u1VdpId=SV_VP_MAIN;
    }
    return fgFldInv[u1VdpId];
}

static inline void vDrvTTTInit(void)
{
    vDrvLoadRegTbl(TTT_INIT);
    u1DrvTTTSetMode(SV_VP_MAIN, SV_OFF, TTT_MODE_FS, TTT_FS_LEFT);
    u1DrvTTTSetMode(SV_VP_PIP, SV_OFF, TTT_MODE_FS, TTT_FS_LEFT);
}

static inline void vDrvTTTCfg(UINT32 bPath)
{
    UINT32 u4TTT = u4DrvTDTVTTTModeQuery(bPath);
    E_TD_IN eTTDIn=TD_TTT_IN(u4TTT);
    E_TD_OUT eTTDOut=TD_TTT_OUT(u4TTT);
    E_TDTV_UI_3D_2_2D e322 =  eDrvTDTVGet3D22D(bPath);

    UINT32 u4OnOff,u4ModelSel,u4MskSel;
    static UINT32 u4PreModelSel[]={TTT_MODE_MAX, TTT_MODE_MAX};
    static UINT32 u4PreMskSel[]={E_TDTV_UI_3D_2_2D_OFF, E_TDTV_UI_3D_2_2D_OFF}; 
    static UINT32 u4PreOnOff[]={0, 0}; 

    eTTDIn = (E_TD_IN)((UINT32)eTTDIn & ~0x1);

    if(E_TD_OUT_3D_2_2D==eTTDOut)
    {    
        u4OnOff=SV_ON;
        switch(eTTDIn)
        {        
            case E_TD_IN_FS_I:
                u4ModelSel=TTT_MODE_FS;
                u4MskSel= E_TDTV_UI_3D_2_2D_L == e322
                    ? TTT_FS_LEFT : TTT_FS_RIGHT;
                break;
            case E_TD_IN_SBS_I:
            case E_TD_IN_SS_I:
            case E_TD_IN_RD_I:
                u4ModelSel=TTT_MODE_SBS;
                u4MskSel= E_TDTV_UI_3D_2_2D_L == e322
                    ? TTT_SBS_LEFT : TTT_SBS_RIGHT;
                break;            
            case E_TD_IN_TB_I:
                u4ModelSel=TTT_MODE_TB;
                u4MskSel= E_TDTV_UI_3D_2_2D_L == e322          
                    ? TTT_TB_TOP : TTT_TB_BOTTOM;
                break;
            case E_TD_IN_LI_I:
                u4ModelSel=TTT_MODE_LI;
                u4MskSel= E_TDTV_UI_3D_2_2D_L == e322       
                    ? TTT_LI_EVEN : TTT_LI_ODD;
                break;            
            default:
                u4ModelSel=TTT_MODE_FS;
                u4MskSel= E_TDTV_UI_3D_2_2D_L == e322        
                    ? TTT_FS_LEFT : TTT_FS_RIGHT;
        }
    }
    else
    {
        u4OnOff=SV_OFF;
        u4ModelSel=TTT_MODE_FS;
        u4MskSel=TTT_FS_LEFT;
    }

    if (u4PreModelSel[bPath] != u4ModelSel || 
        u4PreMskSel[bPath] != u4MskSel ||
        u4PreOnOff[bPath] != u4OnOff)
    {
        u4PreModelSel[bPath] = u4ModelSel;
        u4PreMskSel[bPath] = u4MskSel;
        u4PreOnOff[bPath] = u4OnOff;
        vVRMSetEventFlg(bPath, VRM_EVENT_BY_VDO_MODCHG);
    }
    u1DrvTTTSetMode(bPath, u4OnOff , u4ModelSel, u4MskSel);
}

static inline void vDrvTDBLCfg(void)
{
#if SUPPORT_3D_BL_CTRL      
    if(IO32ReadFldAlign(TDTV_FW_00, BL_MLOOP))
    {    
        if((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && IS_LVDS_DISP_3D_SHUTTER)
        {        
            vApiSetScanningPWM(wDrvGetOutputHTotal(), wDrvGetOutputVTotal()
                , SCAN_PWM_START,  wDrvGetOutputVTotal() - PANEL_GetPanelHeight());
        }
    }
#endif
}

static inline void vDrvDQCSetDQ(sDQConfig *sConfig)
{
    UINT32 u4PSCANFull,u4PSCANHalf,u4NRFull;
    UINT32 u4Height = sConfig->u2SrcHeight;

    u4Height = bDrvVideoIsSrcInterlace(SV_VP_MAIN) ? u4Height>>1 : u4Height;
    
    vIO32WriteFldAlign(DQ_00, !sConfig->u1OnOff, DQ_BYPASS);
    vIO32WriteFldAlign(DQ_09, sConfig->u2SrcWidth, SRCIMG_WIDTH);
    vIO32WriteFldAlign(DQ_09, u4Height, SRCIMG_HEIGHT);
    vIO32WriteFldAlign(DQ_00, sConfig->u1SrcMode, FS_SS_SEL);
    vIO32WriteFldAlign(DQ_00, SV_OFF, DQ_DBUF);
    
    if (sConfig->u1X2Mode)
    {
        vIO32WriteFldAlign(DQ_00, 0x1, DQ_LUMA_X2_MODE);
        vIO32WriteFldAlign(DQ_00, 0x1, DQ_CHROMA_X2_MODE);
    }
    else
    {
        vIO32WriteFldAlign(DQ_00, 0x0, DQ_LUMA_X2_MODE);
        vIO32WriteFldAlign(DQ_00, 0x1, DQ_CHROMA_X2_MODE);
    }


    vIO32WriteFldAlign(DQ_00, (sConfig->u1FSMode), FS_SS_SEL);

    // TODO: SRAM MODE
    if((bDrvVideoIsSrcInterlace(SV_VP_MAIN))
        ||(sConfig->u1X2Mode))
    {
        u4PSCANFull=SV_OFF;
        u4PSCANHalf=SV_OFF;
        u4NRFull=SV_ON;
    }
    else
    {
        u4PSCANFull=SV_ON;
        u4PSCANHalf=SV_OFF;
        u4NRFull=SV_OFF;
    }

    
    vIO32WriteFldAlign(DQ_00, u4PSCANFull, SRAM_PSCAN_FULL);
    vIO32WriteFldAlign(DQ_00, u4PSCANHalf, SRAM_PSCAN_HALF);
    vIO32WriteFldAlign(DQ_00, u4NRFull   , SRAM_NR_FULL);    
}

static inline void vDrvDQCSetCB(sCBConfig *sConfig)
{
    UINT32 u4Val = 0;
    u4Val = ((CB_SRC_FS==sConfig->eSrc)<< 14) | ((CB_SRC_VI==sConfig->eSrc)<< 13) | ((!sConfig->u1OnOff) << 12) | (sConfig->u2SrcWidth);
    vIO32Write4B(CB_00, u4Val);
    vIO32Write4B(CB_01, 0);    
}

static inline void vDrvDQCInit(void)
{
    vIO32WriteFldAlign(DQ_00, 0x1, DQ_BYPASS);
    vIO32WriteFldAlign(CB_00, 0x1, CB_BYPASS);
    //DQ always output 444    
    vIO32WriteFldAlign(DQ_00, 0x1, DQ_444_422_SEL); 
}

static inline void vDrvDQCCfg(void)
{
    UINT32 u4DQC= u4DrvTDTVDQCModeQuery();
    E_TD_IN eDQCIn= TD_DQC_IN(u4DQC);
    E_TD_OUT eDQCOut= TD_DQC_OUT(u4DQC);
    UINT8 fgIs2XMode = TD_DQC_2X(u4DQC);

    sDQConfig sDQCfg;
    sCBConfig sCBCfg;

    VRM_INFO_T in;
    VRM_INFO_T out;

    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_CB, &in, &out);

    sDQCfg.u2SrcWidth = out.u2Width;
    sDQCfg.u2SrcHeight = out.u2Height;

    sCBCfg.u2SrcWidth = in.u2Width;
 
    sDQCfg.u1FSMode = SV_OFF;

    if(E_TD_OUT_3D_FS==eDQCOut)
    {
        sDQCfg.u1FSMode=SV_ON;

        switch(eDQCIn)
        {
            case E_TD_IN_SS_I:
            case E_TD_IN_SS_P:
                //DQ
                sDQCfg.u1OnOff=SV_ON;
                sDQCfg.u1X2Mode=fgIs2XMode;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_OFF;
                sCBCfg.eSrc = CB_SRC_CB;
                break;
            default:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_OFF;
                sCBCfg.eSrc = CB_SRC_CB;              
        }
    }
    else if(E_TD_OUT_3D_SBS==eDQCOut)
    {
        switch(eDQCIn)
        {
            case E_TD_IN_FS_I:
            case E_TD_IN_FS_P:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_ON;
                sCBCfg.eSrc = CB_SRC_FS;
                break;        
            case E_TD_IN_SS_I:
            case E_TD_IN_SS_P:
                //DQ
                sDQCfg.u1OnOff=SV_ON;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_OFF;
                sCBCfg.eSrc = CB_SRC_CB;
                break;
            case E_TD_IN_CB_I:
            case E_TD_IN_CB_P:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=SV_OFF;
                //CB
                sCBCfg.u1OnOff=SV_ON;
                sCBCfg.eSrc = CB_SRC_CB;                
                break;
            case E_TD_IN_DA_I:
            case E_TD_IN_DA_P:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_ON;
                sCBCfg.eSrc = CB_SRC_VI;                
                break;
            case E_TD_IN_LI_I:
            case E_TD_IN_LI_P:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_OFF;
                sCBCfg.eSrc = CB_SRC_CB;                
                break;
            default:
                //DQ
                sDQCfg.u1OnOff=SV_OFF;
                sDQCfg.u1X2Mode=SV_OFF;
                sDQCfg.u1SrcMode=0;
                //CB
                sCBCfg.u1OnOff=SV_OFF;
                sCBCfg.eSrc = CB_SRC_CB;              
        }
    }
    else
    {
        sDQCfg.u1OnOff=SV_OFF;
        sDQCfg.u1X2Mode=SV_OFF;
        sDQCfg.u1SrcMode=0;
        
        sCBCfg.u1OnOff=SV_OFF;
        sCBCfg.eSrc = CB_SRC_CB;

    }

    vDrvDQCSetDQ(&sDQCfg);
    vDrvDQCSetCB(&sCBCfg);
}



void vDrvTDTVMiscInit(void)
{
    vDrvTTTInit();
    vDrvDQCInit();
}

void vDrvTDTVMiscCfg(void)
{
    u1DrvRealDMaskOnOff(SV_VP_MAIN, (TDTV_TAG3D_REALD==u4DrvTDTVGetTag3D(SV_VP_MAIN)));
    u1DrvRealDMaskOnOff(SV_VP_PIP, (TDTV_TAG3D_REALD==u4DrvTDTVGetTag3D(SV_VP_PIP)));
    
    vDrvTTTCfg(SV_VP_MAIN);
    vDrvTTTCfg(SV_VP_PIP);

    vDrvTDBLCfg();  
    vDrvDQCCfg();    
}
