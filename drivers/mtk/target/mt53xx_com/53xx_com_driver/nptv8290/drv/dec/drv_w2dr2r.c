#include "general.h"
#include "x_hal_io.h"
#include "hw_sys.h"
#include "hw_w2dr2r.h"
#include "drv_w2dr2r.h"
#include "x_timer.h"
#include "vga_table.h"
#include "hw_hdmi.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
#include "x_debug.h"
#include "drv_dvi.h"
#include "fbm_drvif.h"
#include "srm_drvif.h"
#include "drv_dvi.h"
#include "drv_display.h"
#include "vdo_misc.h"
#include "hw_sys.h"
#include "hw_di.h"
#ifdef CC_HDMI_2_0_SUPPORT
#include "drv_hdmi2.h"
#include "drv_dvi2.h"
#endif
#ifdef CC_HDMI_2_0_SUPPORT
extern DviStatus_t DviSta[2];
extern E_HDMI_SWITCH_NUM eActiveHdmiPort;
#endif

//#include "panel.h"
//void vW2DMchTrigger(void) line 304 need double confirm

/*
108M = 0x6c 000 00
FbmAddrBase
every frame:size 36M = 0x2400000
every Y ,U, V size 36M/3 = 0xc00000 (0x00000~0xc00000,0xc00000~0x1800000,0x1800000~0x2400000)

							For R2R:
(buffer 1)   chA: FbmAddrBase                       chB: FbmAddrBase + 0xc00000,                    chC:FbmAddrBase + 0x1800000
(buffer 2)   chA: FbmAddrBase + 0x2400000   chB: FbmAddrBase + 0x2400000 + 0xc00000, chC:FbmAddrBase + 0x2400000 + 0x1800000
(buffer 2)   chA: FbmAddrBase + 0x4800000   chB: FbmAddrBase + 0x4800000 + 0xc00000, chC:FbmAddrBase + 0x4800000 + 0x1800000
							For W2D:
if 4K2K input,need L and R
(buffer 1)   chA_L: FbmAddrBase                       chB_L: FbmAddrBase + 0xc00000,                    chC_L:FbmAddrBase + 0x1800000
(buffer 2)   chA_L: FbmAddrBase + 0x2400000   chB_L: FbmAddrBase + 0x2400000 + 0xc00000, chC_L:FbmAddrBase + 0x2400000 + 0x1800000
(buffer 2)   chA_L: FbmAddrBase + 0x4800000   chB_L: FbmAddrBase + 0x4800000 + 0xc00000, chC_L:FbmAddrBase + 0x4800000 + 0x1800000

8bit:halfLineOffset = (1920/(128/8) =120,120X(128/8) = 1920byte  
10bit: halfLineOffset = (1920/(128/10) =160,160X(128/8) = 2560byte  
16bit: halfLineOffset = (1920/(128/16) =240,240X(128/8) = 3840byte  

(buffer 1)   chA_R: FbmAddrBase + halfLineOffset      chB_R: FbmAddrBase + 0xc00000,                    chC_R:FbmAddrBase + 0x1800000
(buffer 2)   chA_R: FbmAddrBase + 0x2400000   chB_R: FbmAddrBase + 0x2400000 + 0xc00000, chC_R:FbmAddrBase + 0x2400000 + 0x1800000
(buffer 2)   chA_R: FbmAddrBase + 0x4800000   chB_R: FbmAddrBase + 0x4800000 + 0xc00000, chC_R:FbmAddrBase + 0x4800000 + 0x1800000

other timing: need L only
(buffer 1)   chA_L: FbmAddrBase                       chB: FbmAddrBase + 0xc00000,                    chC:FbmAddrBase + 0x1800000
(buffer 2)   chA: FbmAddrBase + 0x2400000       chB: FbmAddrBase + 0x2400000 + 0xc00000, chC:FbmAddrBase + 0x2400000 + 0x1800000
(buffer 2)   chA: FbmAddrBase + 0x4800000   chB: FbmAddrBase + 0x4800000 + 0xc00000, chC:FbmAddrBase + 0x4800000 + 0x1800000

*/


extern UINT8 _bDviTiming;
extern VGAMODE  VGATIMING_TABLE[];

FBM_POOL_T rW2DR2RInfo;
FBM_POOL_T rW2DR2RInfoCHc;   //if use  FBM channel C, use this variable

R2R_CONFI r2r_configures;
W2D_CONFI w2d_configures;

/*for video txture feature*/
W2D_VIDEO_TXT_CONFI w2d_confi_videoTexture;
R2R_VIDEO_TXT_CONFI r2r_confi_videoTexture;

static x_os_isr_fct _pfnR2rIsr = NULL; 

// w2d r2r test by thread, only use when do test, use interrupt 
static HANDLE_T phR2RThreadHdl= NULL_HANDLE;
static HANDLE_T phW2DThreadHdl= NULL_HANDLE;
UINT8 r2rThreadRun = 0;
UINT8 w2dThreadRun = 0;
UINT8 fgW2dR2rAddrSame = FALSE;

UINT8 fgHDMIW2DR2REnable = FALSE; 
UINT8 fgUseChC = TRUE;
BOOL  fgR2RVB1Enable = TRUE;
UINT8 fgW2dForceLeftAndRight = FALSE;
UINT8 fgForceUseW2D = FALSE;
UINT8 fg2k2ktoOmux = FALSE;

UINT32 u4LoseSyncCnt = 0;

#ifdef W2D_R2R_TEST
UINT32 r2rThreadCnt = 0;
UINT32 w2dIntCnt = 0;
#endif

HAL_TIME_T rW2DTimeTmp;
HAL_TIME_T rR2RTimeTmp;

HAL_TIME_T rW2DIntTimeStep;
HAL_TIME_T rR2RIntTimeStep;
HAL_TIME_T rW2DChangeAddrTimeStep;
HAL_TIME_T rR2RChangeAddrTimeStep;





UINT8 w2dWriteFrameIndex = 0;
UINT8 r2rReadFrameIndex = 2;
UINT8 needFrameDelayCnt = 2;
UINT8 u1W2dWriteFrameDoneCnt = 0;
UINT8 u1R2rReadFrameDoneCnt = 1; 

UINT32 u4R2rModechangeCnt = 10;

#define R2R_FRAME_MAX_CNT 12
UINT8 u1AvailableFrameCnt = 3;
UINT8 fgFieldInfo[R2R_FRAME_MAX_CNT];
UINT8 fieldCtrl = 1;
UINT8 DiFieldCtrl = 1;

HDMI_TIMING_T hdmiVicTimingSpec[256] =
{
	//interlace timing
	{2200,1125,1920,88,44,148,540,2,5,15,60,5,1},
	{1716,525,1440,38,124,114,240,4,3,15,60,6,1},
	{1716,525,1440,38,124,114,240,4,3,15,60,7,1},
	{3432,525,2880,76,248,228,240,4,3,15,60,10,1},
	{3432,525,2880,76,248,228,240,4,3,15,60,11,1},
	{2640,1125,1920,528,44,148,540,2,5,15,50,20,1},
	{1728,625,1440,24,126,138,288,2,3,19,50,21,1},
	{1728,625,1440,24,126,138,288,2,3,19,50,22,1},
	{3456,625,2880,48,252,276,288,2,3,19,50,25,1},
	{3456,625,2880,48,252,276,288,2,3,19,50,26,1},
	{2304,1250,1920,32,168,184,540,23,5,57,50,39,1},
};
/*
HDMI_TIMING_T Vic6and7TimingSpec =
{1716,525,1440,38,124,114,240,4,3,15,60,6,1};
HDMI_TIMING_T Vic10and11TimingSpec =
{3432,525,2880,76,248,228,240,4,3,15,60,10,1};
HDMI_TIMING_T Vic21and22TimingSpec =
{1728,625,1440,24,126,138,288,2,3,19,50,21,1};
HDMI_TIMING_T Vic25and26TimingSpec =
{3456,625,2880,48,252,276,288,2,3,19,50,25,1};
HDMI_TIMING_T Vic5TimingSpec =
{2200,1125,1920,88,44,148,540,2,5,15,60,5,1};
HDMI_TIMING_T Vic20TimingSpec =
{2640,1125,1920,528,44,148,540,2,5,15,50,20,1};
HDMI_TIMING_T Vic39TimingSpec =
{2304,1250,1920,32,168,184,540,23,5,57,50,39,1};
*/
UINT8 getIndexAccordingVic(UINT8 vicCode)
{
	UINT8 i;
	for(i=0;i<100;i++)
	{
		if(vicCode == hdmiVicTimingSpec[i].vicCode)
			return i;
	}
	return 0xff;
}
R2R_FRAME_ADDR r2rReadFrameAddr[R2R_FRAME_MAX_CNT] = 
{
	{0x00200000,0x00300000,0x00400000},
	{0x00280000,0x00380000,0x00480000},
	{0x00500000,0x00600000,0x00700000}
};
W2D_FRAME_ADDR w2dWriteFrameAddr[R2R_FRAME_MAX_CNT] = 
{
	{0x00200000,0x00300000,0x00400000,0x00200000,0x00300000,0x00400000},
	{0x00200000,0x00300000,0x00400000,0x00200000,0x00300000,0x00400000},
	{0x00200000,0x00300000,0x00400000,0x00200000,0x00300000,0x00400000}		
};

extern INT32 x_os_drv_reg_isr_ex(UINT16 ui2_vec_id,
                                 x_os_isr_fct   pf_isr,
                                 x_os_isr_fct   *ppf_old_isr);

UINT32 u4W2DGetBase(BOOL fgLeft)
{
	if(fgLeft)
	{
		return 0x00;
	}
	else
	{
		return 0x40;
	}
}
void vW2DEnable(BOOL fgLeft)
{
	UINT32 u4RegBase;
	u4RegBase = u4W2DGetBase(fgLeft);		
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + u4RegBase,1,BGenvdoin_on);
}

void vW2DDisable(BOOL fgLeft)
{
	UINT32 u4RegBase;
	u4RegBase = u4W2DGetBase(fgLeft);		
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + u4RegBase,0,BGenvdoin_on);
}
void vW2DSetPackMode(UINT8 u1PackMode)
{
	switch(u1PackMode)
	{
		case 8:
			vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,pack_mode);
			vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,0,pack_mode);
			break;
		case 10:
			vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,pack_mode);
			vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,pack_mode);
			break;
		case 16:
			vIO32WriteFldAlign(W2D_LW2D_ENABLE,2,pack_mode);
			vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,2,pack_mode);
			break;
		default:			
			vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,pack_mode);
			vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,0,pack_mode);
			break;
	}
}
void vW2DSetLeftIntFunc(UINT8 u1IsIntMerge)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,u1IsIntMerge?1:0,c_int_new);
}
void vW2DSetRightIntFunc(UINT8 u1IsIntMerge)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,u1IsIntMerge?1:0,c_int_new);
}
void vW2DEnableLeftInt(UINT8 u1Enable)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,u1Enable?0:1,c_int_dis); // 1:disable interrupt; 0: enable interrupt output
}
void vW2DEnableRightInt(UINT8 u1Enable)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,u1Enable?0:1,c_int_dis);
}
void vW2DSetLeftIntMSK(UINT8 u1Enable)
{
	vIO32WriteFldAlign(SYS_03,u1Enable?0:1,INT_MASK_W2D_LEFT);
}
void vW2DSetRightIntMSK(UINT8 u1Enable)
{
	vIO32WriteFldAlign(SYS_03,u1Enable?0:1,INT_MASK_W2D_RIGHT);
}
UINT8 u1IsVdoInW2DLeftIntSet(void)
{
	if(IO32ReadFldAlign(INT_COLLECT,INTR_W2D_LEFT))
		return 1;
	return 0;
}
UINT8 u1IsVdoInW2DRightIntSet(void)
{
	if(IO32ReadFldAlign(INT_COLLECT,INTR_W2D_RIGHT))
		return 1;
	return 0;
}
void vW2DClrLeftIntSta(void)
{	
	vIO32WriteFldAlign(W2D_MON_01,1,interrupt);
}
void vW2DClrRightIntSta(void)
{
	vIO32WriteFldAlign(W2D_MON_01 + 0x40,1,interrupt);	
}
BOOL vW2DGetLeftIntSta(void)
{	
	return IO32ReadFldAlign(W2D_MON_01,interrupt)?1:0;
}
BOOL vW2DGetRightIntSta(void)
{
	return IO32ReadFldAlign(W2D_MON_01 + 0x40,interrupt)?1:0;	
}

void vW2DClrLeftInt(void)
{
	vW2DClrLeftIntSta();
}
void vW2DClrRightInt(void)
{
	vW2DClrRightIntSta();
}

void vW2DSetLAddr(UINT32 u4ChAaddr,UINT32 u4ChBAddr, UINT32 u4ChCAddr)
{
	vIO32Write4B(0xf0022c18,u4ChAaddr);
	vIO32Write4B(0xf0022c1c,u4ChBAddr);					
	vIO32Write4B(0xf0022c20,u4ChCAddr);	
}
void vW2DSetRAddr(UINT32 u4ChAaddr,UINT32 u4ChBAddr, UINT32 u4ChCAddr)
{
	vIO32Write4B(0xf0022c58,u4ChAaddr);
	vIO32Write4B(0xf0022c5c,u4ChBAddr);					
	vIO32Write4B(0xf0022c60,u4ChCAddr);	
}
void vW2DSetFrameAddr(W2D_FRAME_ADDR arW2dWriteFrameAddr)
{
	vW2DSetLAddr(arW2dWriteFrameAddr.u4FrameLYaddr,arW2dWriteFrameAddr.u4FrameLUaddr,arW2dWriteFrameAddr.u4FrameLVaddr);
	vW2DSetRAddr(arW2dWriteFrameAddr.u4FrameRYaddr,arW2dWriteFrameAddr.u4FrameRUaddr,arW2dWriteFrameAddr.u4FrameRVaddr);
}
void vW2DCutEnalbe(BOOL fgCutEn,BOOL fgCutLeftAndRight)
{

	vIO32WriteFldAlign(W2D_FIELD_GEN,fgCutEn?1:0,c_decut_en);
	vIO32WriteFldAlign(W2D_FIELD_GEN + 0x40,fgCutEn?1:0,c_decut_en);

//cut length config
	if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
	{
		vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,c_separate_en);
		vIO32WriteFldAlign(W2D_FIELD_GEN,127,c_decut_start);
		if(fgCutLeftAndRight)
		{
			vIO32WriteFldAlign(W2D_FIELD_GEN,1920,c_decut_len);
		}
		else  //cut only left
		{
			vIO32WriteFldAlign(W2D_FIELD_GEN,3840,c_decut_len);
		}
		vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,c_separate_en);
		vIO32WriteFldAlign(W2D_FIELD_GEN + 0x40,0,c_decut_start);
		vIO32WriteFldAlign(W2D_FIELD_GEN + 0x40,0x77F,c_decut_len);

	}
	else
	{
		vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,c_separate_en);
		vIO32WriteFldAlign(W2D_FIELD_GEN,127,c_decut_start);
		if(fgCutLeftAndRight)
		{
			vIO32WriteFldAlign(W2D_FIELD_GEN,1920,c_decut_len);
		}
		else  //cut only left
		{
			vIO32WriteFldAlign(W2D_FIELD_GEN,3840,c_decut_len);
		}
		vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,c_separate_en);
		vIO32WriteFldAlign(W2D_FIELD_GEN + 0x40,0,c_decut_start);
		vIO32WriteFldAlign(W2D_FIELD_GEN + 0x40,1920,c_decut_len);

	}

}
void vW2DSetPitch(UINT32 u4HdeWidth,UINT8 u1BitNum, UINT8 u1W2DLeftAndRight) //pitch is used to jump to next line.
{
	UINT32 u4Pitch;
	if(u1BitNum == 0)
	{
		u1BitNum = 8;
	}
	//cut function not available, don't need change to 1920.	
	u4Pitch = u4HdeWidth / (128/u1BitNum);
	/*
	if(u1W2DLeftAndRight)  // when use left and right w2d simulationly, the pitch value will double. For example, 4K2K to 2X2K2k
	{
		u4Pitch = u4Pitch << 1;
	}
	*/
	vIO32WriteFldAlign(W2D_PITCH_CTL,u4Pitch,c_hsize);	
	vIO32WriteFldAlign(W2D_PITCH_CTL,u4Pitch,y_hszie);	
	
	vIO32WriteFldAlign(W2D_PITCH_CTL + 0x40,u4Pitch,c_hsize);	
	vIO32WriteFldAlign(W2D_PITCH_CTL + 0x40,u4Pitch,y_hszie);	

}
void vW2DSetVeticalLine(UINT32 u4VerticalLine)
{
	
	vIO32WriteFldAlign(W2D_VCNT,u4VerticalLine-1,bgyfldnum);	
	vIO32WriteFldAlign(W2D_VCNT,u4VerticalLine-1,bgcfldnum);
	vIO32WriteFldAlign(W2D_VACTIVE_GEN,u4VerticalLine,c_vlen);	
	
	vIO32WriteFldAlign(W2D_VCNT + 0x40,u4VerticalLine-1,bgyfldnum);	
	vIO32WriteFldAlign(W2D_VCNT + 0x40,u4VerticalLine-1,bgcfldnum);		
	vIO32WriteFldAlign(W2D_VACTIVE_GEN + 0x40,u4VerticalLine,c_vlen);	
}
void vW2DSet422Mode(BOOL fgEnable)
{	
	vIO32WriteFldAlign(W2D_L_YC2YUV,fgEnable?1:0,c_w2d_upsample);
	if(fgEnable)
	{
        vIO32WriteFldAlign(W2D_L_YC2YUV,1,c_w2d_l_bnd_hen);	    
        vIO32WriteFldAlign(W2D_R_YC2YUV,1,c_w2d_r_bnd_hen);	    
	}
}
void vW2DSet422Hbound(UINT32 u4Width)
{
	vIO32WriteFldAlign(W2D_L_YC2YUV,u4Width,c_w2d_l_bnd_h);			
	vIO32WriteFldAlign(W2D_R_YC2YUV,u4Width,c_w2d_r_bnd_h);		
}
void vW2DMchTrigger(void)
{ 
// this usage is not recommanded. use LatchMode first.  need confirm
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,mchg_trigger);			
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,mchg_trigger);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,mchg_trigger);			
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,0,mchg_trigger);
}
void vW2DSetAddrLatchMode(UINT8 u1NextFrameActive)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,u1NextFrameActive?1:0,reg_addr_latch_sel);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,u1NextFrameActive?1:0,reg_addr_latch_sel);
}
void vW2DInit(void)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,c_separate_en);	//separate write request.
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,sram_ena_sel); 
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,rst_b);	
	vIO32WriteFldAlign(W2D_LW2D_ENABLE ,0,reg_addr_latch_sel); // not  use address next frame active
	
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,c_separate_en);	
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,sram_ena_sel);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE +0x40,1,rst_b);	
	vIO32WriteFldAlign(W2D_LW2D_ENABLE +0x40,0,reg_addr_latch_sel);   //not  use address next frame active

	vIO32Write4B(0xf0022c04,0x0017a0e1);	   //threshold change to 16
	vIO32Write4B(0xf0022c04 + 0x40,0x0017a0e1);
	
	vIO32Write4B(0xf0022c10,0x0000ffff);   // add bandwidth protect
	vIO32Write4B(0xf0022c10 + 0x40,0x0000ffff); // add bandwidth protect
	//disable w2d 
	vW2DDisable(0);
	vW2DDisable(1);
	vW2DSetRightIntMSK(FALSE);
	vW2DEnableRightInt(FALSE);
	vW2DSetLeftIntMSK(TRUE);
	vW2DEnableLeftInt(TRUE);

		
}


void vSetW2DControl(W2D_CONFI *w2dConfigure,UINT8 u1DviTiming)
{
//control point is width height leftRight bitNum cutEnable
	UINT8 u1DeepColorSta;
	UINT8 u1IsHdmi422;
#ifdef CC_HDMI_2_0_SUPPORT
	UINT16 u2InputWidth = 0;
#endif

#ifdef CC_HDMI_2_0_SUPPORT
	if(u1GetMinHDMI20Port()<=eActiveHdmiPort)
	{
		if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
		{
			if(W2D_NO_NEED != vDviTimingNeedW2d_V2())
			{
				w2dConfigure->u4Width = 3840;
				w2dConfigure->u4Height = 2160;
				if(W2D_USE_LEFT_AND_RIGHT == vDviTimingNeedW2d_V2())
				{
					w2dConfigure->u4W2DLeftAndRight = 1;
				}
				else
				{
					w2dConfigure->u4W2DLeftAndRight = 0;
				}
			}
		}

		if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
		{
			if(fgHDMI2Get420Mode(eActiveHdmiPort))
			{
				u2InputWidth = (DviSta[0]._wDviWidth) * 2;
			}
			else
			{
				u2InputWidth = DviSta[0]._wDviWidth;
			}
			if(u2InputWidth >= 4000)
			{
				w2dConfigure->u1CutEnable = TRUE;
			}
			else
			{
				w2dConfigure->u1CutEnable = FALSE;
			}
		}
	}
	else
#endif
	{
		w2dConfigure->u4Width = Get_VGAMODE_IPH_WID(u1DviTiming);
		if((Get_VGAMODE_IPH_WID(u1DviTiming) == 3840) && ((wHDMIResoWidth() >= 4000)))
		{
			w2dConfigure->u1CutEnable = TRUE;
		}
		else
		{
			w2dConfigure->u1CutEnable = FALSE;
		}

		if(fgHDMIinterlaced())
		{
			w2dConfigure->u4Height = Get_VGAMODE_IPV_LEN(u1DviTiming)/2;
		}
		else
		{
			w2dConfigure->u4Height = Get_VGAMODE_IPV_LEN(u1DviTiming);
		}
	}

	if(!IsW2dWritingTxture())
	{	
		//if((u1IO32Read1B(AVIRX1_0 + u4ActiveHdmiRegBase) & 0x60) == 0x20)
#ifdef CC_HDMI_2_0_SUPPORT
		if(u1GetMinHDMI20Port()<=eActiveHdmiPort)
		{
			u1IsHdmi422 = bHDMI2422Input(eActiveHdmiPort)?1:0;

		}
		else			
#endif
		{
			if((u1IO32Read1B(AVIRX1_0 + u4ActiveHdmiRegBase) & 0x60) == 0x20)
			{
				u1IsHdmi422 = 1;
			}
			else
			{
				u1IsHdmi422 = 0;
			}
		}	
		w2dConfigure->u1IsHdmi422 = u1IsHdmi422;
#ifdef CC_HDMI_2_0_SUPPORT
	if(u1GetMinHDMI20Port()<=eActiveHdmiPort)
	{
		u1DeepColorSta = bHDMI2GetColorDepth(eActiveHdmiPort);
	}
	else
#endif
	{
		u1DeepColorSta = bHDMIDeepColorStatus();
	}
		switch(u1DeepColorSta)
		{
			case 0: 
				w2dConfigure->u1BitNum = 8;
				break;
			case 1: 
				w2dConfigure->u1BitNum = 10;
				break;
			case 2: 
				//w2dConfigure->u1BitNum = 12;	  // there is no 12bit mode in w2d and r2r ,use 10 bit instead
				w2dConfigure->u1BitNum = 10;
				break;
			case 3: 
				w2dConfigure->u1BitNum = 10;
				break;	
			default:
				w2dConfigure->u1BitNum = 8;
				break;
		}
		/*
		if((vDviTimingNeedW2d(u1DviTiming)==W2D_USE_LEFT_AND_RIGHT)|| fgW2dForceLeftAndRight)
		{
			w2dConfigure->u4W2DLeftAndRight = 1;
		}
		else
		{
			w2dConfigure->u4W2DLeftAndRight = 0;
		}
		*/
		w2d_configures.u1W2DIntGoodCnt = 0;
		w2d_configures.u1W2DStatusGoodFlg = FALSE;
	}
	else
	{
		UINT32 hdmiRegBase = u4GetHdmiRegBase(w2d_confi_videoTexture.hdmiPortId);
		if((Get_VGAMODE_IPH_WID(u1DviTiming) == 3840) && ((wHDMIResoWidthByPort(w2d_confi_videoTexture.hdmiPortId) >= 4000)))
		{
			w2dConfigure->u4Width = 4096;
		}
		if((Get_VGAMODE_IPH_WID(u1DviTiming) == 1920) && ((wHDMIResoWidthByPort(w2d_confi_videoTexture.hdmiPortId) >= 2000)))
		{
			w2dConfigure->u4Width = 2048;
		}		
		
		w2dConfigure->u4Height = Get_VGAMODE_IPV_LEN(u1DviTiming);
		if((u1IO32Read1B(AVIRX1_0 + hdmiRegBase) & 0x60) == 0x20)
		{
			u1IsHdmi422 = 1;
		}
		else
		{
			u1IsHdmi422 = 0;
		}		
		w2dConfigure->u1IsHdmi422 = u1IsHdmi422;
		w2dConfigure->u1BitNum = 8;
		if((vDviTimingNeedW2d(u1DviTiming)==W2D_USE_LEFT_AND_RIGHT)|| fgW2dForceLeftAndRight)
		{
			w2dConfigure->u4W2DLeftAndRight = 1;
		}
		else
		{
			w2dConfigure->u4W2DLeftAndRight = 0;
		}
		w2d_configures.u1W2DIntGoodCnt = 0;
		w2d_configures.u1W2DStatusGoodFlg = FALSE;
	}
}
void vConfiW2D(W2D_CONFI w2dConfigure)
{					
	vW2DSetPackMode(w2dConfigure.u1BitNum);					
	vW2DSetVeticalLine(w2dConfigure.u4Height);
	vW2DSetPitch(w2dConfigure.u4Width,w2dConfigure.u1BitNum,w2dConfigure.u4W2DLeftAndRight);
	
	if(w2dConfigure.u1CutEnable)
	{
		vW2DCutEnalbe(TRUE,w2dConfigure.u4W2DLeftAndRight);
	}
	else
	{
		vW2DCutEnalbe(FALSE,w2dConfigure.u4W2DLeftAndRight);
	}
	if(w2dConfigure.u1IsHdmi422)
	{
		vW2DSet422Mode(TRUE);
		vW2DSet422Hbound(w2dConfigure.u4Width);
	}
	else
	{
		vW2DSet422Mode(FALSE);
	}
	
	if(w2dConfigure.u4W2DLeftAndRight)
	{
		vW2DSetLeftIntFunc(1);   //use merge int here ,when left and right int occur ,int occur
        vW2DSetRightIntFunc(0);
	}
	else
	{			
		vW2DSetLeftIntFunc(0);
		vW2DSetRightIntFunc(0);
	}
}

void vR2RSetHtoalAndVtotal(UINT32 u4Htotal,UINT32 u4Vtotal)
{	
	if(fgIsR2REnableBypass())
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG00,u4Htotal>>1,c_r2r_hsync_total); 
	}
	else
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG00,u4Htotal,c_r2r_hsync_total); 
	}
	vIO32WriteFldAlign(R2R_C_R2R_REG00,u4Vtotal,c_r2r_vsync_total);	
}

void vR2RSetHsyncWidth(UINT32 u4HsyncWidth)
{	
	if(fgIsR2REnableBypass())
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG01,u4HsyncWidth>>1,c_r2r_hsync_width); 
	}
	else
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG01,u4HsyncWidth,c_r2r_hsync_width); 
	}

}
void vR2RSetHdewAndVdew(UINT32 u4HdeWidth,UINT32 u4VdeWidth)
{	
	if(fgIsR2REnableBypass())
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG03,u4HdeWidth>>1,c_r2r_hdew);	
	}
	else
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG03,u4HdeWidth,c_r2r_hdew);	
	}
	vIO32WriteFldAlign(R2R_C_R2R_REG03,u4VdeWidth,c_r2r_vdew);	
}
void vR2RSetVsync(UINT32 u4VsyncStart,UINT32 u4VsyncEnd,UINT32 u4VsyncEvenStart,UINT32 u4VsyncEvenEnd)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG04,u4VsyncStart,c_r2r_vsync_odd_start);	
	vIO32WriteFldAlign(R2R_C_R2R_REG04,u4VsyncEnd,c_r2r_vsync_odd_end);		
	vIO32WriteFldAlign(R2R_C_R2R_REG05,u4VsyncEvenStart,c_r2r_vsync_even_start);	
	vIO32WriteFldAlign(R2R_C_R2R_REG05,u4VsyncEvenEnd,c_r2r_vsync_even_end);		
}

void vR2RSetHde(UINT32 u4HdeStart,UINT32 u4HdeEnd)
{
	if(fgIsR2REnableBypass())
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG08,(u4HdeStart>>1) + 1,c_r2r_hde_start);	
		vIO32WriteFldAlign(R2R_C_R2R_REG08,((u4HdeStart>>1) + 1) + (u4HdeEnd - u4HdeStart +1)/2 -1,c_r2r_hde_end);					
	}
	else
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG08,u4HdeStart + 1,c_r2r_hde_start);	
		vIO32WriteFldAlign(R2R_C_R2R_REG08,u4HdeEnd + 1,c_r2r_hde_end);				
	}
}
void vR2RSetVde(UINT32 u4VdeStart,UINT32 u4VdeEnd,UINT32 u4VdeEvenStart,UINT32 u4VdeEvenEnd)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG09,u4VdeStart,c_r2r_vde_odd_start);	
	vIO32WriteFldAlign(R2R_C_R2R_REG09,u4VdeEnd,c_r2r_vde_odd_end);		
	vIO32WriteFldAlign(R2R_C_R2R_REG10,u4VdeEvenStart,c_r2r_vde_even_start);	
	vIO32WriteFldAlign(R2R_C_R2R_REG10,u4VdeEvenEnd,c_r2r_vde_even_end);			
}

void vR2RSetPitch(UINT32 u4HdeWidth,UINT8 u1BitNum)
{
	UINT32 u4Pitch;
	
	if(u1BitNum == 0)
	{
		u1BitNum = 8;
	}
	u4Pitch = u4HdeWidth / (128/u1BitNum);
	vIO32WriteFldAlign(R2R_C_R2R_REG14,u4Pitch,c_r2r_hdew_pitch);	
}
void vR2RSetWordMax(UINT32 u4HdeWidth,UINT8 u1BitNum)
{	
	UINT32 u4WordMax;
	
	if(u1BitNum == 0)
	{
		u1BitNum = 8;
	}
	u4WordMax = u4HdeWidth / (128/u1BitNum);
	vIO32WriteFldAlign(R2R_C_R2R_REG13,u4WordMax,c_r2r_word_ymax);	
	vIO32WriteFldAlign(R2R_C_R2R_REG13,u4WordMax,c_r2r_word_umax);	
	vIO32WriteFldAlign(R2R_C_R2R_REG14,u4WordMax,c_r2r_word_vmax);	

}
void vR2RSetADDR(UINT32 u4ChA,UINT32 u4ChB,UINT32 u4ChC)
{
	vIO32Write4B(R2R_C_R2R_REG16,u4ChA);
	vIO32Write4B(R2R_C_R2R_REG18,u4ChB);
	vIO32Write4B(R2R_C_R2R_REG20,u4ChC);
}
void vR2RSetBottomADDR(UINT32 u4ChA,UINT32 u4ChB,UINT32 u4ChC)
{
	vIO32Write4B(R2R_C_R2R_REG17,u4ChA);
	vIO32Write4B(R2R_C_R2R_REG19,u4ChB);
	vIO32Write4B(R2R_C_R2R_REG21,u4ChC);
}
void vR2RSetBottomFrameAddr(R2R_FRAME_ADDR arFrameAddr)
{
	vR2RSetBottomADDR(arFrameAddr.u4FrameYaddr,arFrameAddr.u4FrameUaddr,arFrameAddr.u4FrameVaddr);
}
void vR2RSetFrameAddr(R2R_FRAME_ADDR arFrameAddr)
{
	vR2RSetADDR(arFrameAddr.u4FrameYaddr,arFrameAddr.u4FrameUaddr,arFrameAddr.u4FrameVaddr);
}
void vR2REnableWriteBack(BOOL fgEnable)  //write back function is only for FPGA emulation
{
	vIO32WriteFldAlign(R2R_C_R2R_REG00,(fgEnable)?1:0,c_r2r_write_back);	
}
void vR2RSetPacketMode(UINT8 u1BitNum)
{
	switch(u1BitNum)
	{
		case 8:
			{
				vIO32WriteFldAlign(R2R_C_R2R_REG15,0,c_r2r_deep_mode);	
				vIO32WriteFldAlign(R2R_C_R2R_REG15,0,c_10bit_enable);	
			}
			break;
		case 10:
			{
				vIO32WriteFldAlign(R2R_C_R2R_REG15,0,c_r2r_deep_mode);	
				vIO32WriteFldAlign(R2R_C_R2R_REG15,1,c_10bit_enable);					
			}
			break;
		case 16:
			{
				vIO32WriteFldAlign(R2R_C_R2R_REG15,1,c_r2r_deep_mode);	
				vIO32WriteFldAlign(R2R_C_R2R_REG15,0,c_10bit_enable);									
			}
			break;
	}
}
void vR2RSetBypassMode(BOOL fgEnable)   //bypass mode is vb1 mode
{
	vIO32WriteFldAlign(R2R_C_R2R_REG15,(fgEnable)?1:0,c_r2r_bypass_mode);		
}
BOOL vR2RIsR2RInt(void)
{
	UINT8 u1IsR2RIntSet;
	u1IsR2RIntSet = IO32ReadFldAlign(R2R_C_R2R_REG13,c_r2r_int);
	return (u1IsR2RIntSet)?1:0;
}
void vR2RClrR2RInt(void)
{	
	vIO32WriteFldAlign(R2R_C_R2R_REG13,1,c_r2r_int);	
}
void vR2RIntEnable(BOOL fgEnable)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable?0:1,c_r2r_int_mask);		
}
void vR2REnableDram(BOOL fgEnable)	
{
	vIO32WriteFldAlign(R2R_C_R2R_REG14,fgEnable?1:0,c_r2r_enable_dram);		
}
void vR2REnableTiming(BOOL fgEnable)	
{
	vIO32WriteFldAlign(R2R_C_R2R_REG15,fgEnable?1:0,c_r2r_enable_timing);		
}
BOOL fgIsR2REnableBypass(void)
{	
	BOOL fgIsVB1Enable = FALSE;
	if(IO32ReadFldAlign(R2R_C_R2R_REG15,c_r2r_bypass_mode))
		{
			fgIsVB1Enable = TRUE;
		}
	return fgIsVB1Enable;			
}

void vR2RReset(void)
{	
	vIO32WriteFldAlign(R2R_C_R2R_REG15,1,c_r2r_rst_0);
	vUtDelay2us(10);
	vIO32WriteFldAlign(R2R_C_R2R_REG15,0,c_r2r_rst_0);		
}
void vR2RRoundMidEn(BOOL fgEnable)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable?1:0,c_r2r_y_mid_en);
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable?1:0,c_r2r_u_mid_en);
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable?1:0,c_r2r_v_mid_en);
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable?1:0,c_r2r_round_mid_en);
}
void vR2RLatchEnable(BOOL fgEnable)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG13,fgEnable,c_r2r_yuv_addr_latch_en);
}
void vR2RInit(void)
{	
	vR2RRoundMidEn(FALSE);  //enable middle round en
	vR2REnableDram(FALSE);
	vR2REnableTiming(FALSE);
	vR2RLatchEnable(TRUE);
	vIO32WriteFldAlign(R2R_C_R2R_REG01,0x0,c_r2r_padding_y);
	vIO32WriteFldAlign(R2R_C_R2R_REG02,0x0,c_r2r_padding_u);
	vIO32WriteFldAlign(R2R_C_R2R_REG02,0x0,c_r2r_padding_v);
	vR2RReset();
}
void vR2RRegisterIsr(void)
{	
	vIO32WriteFldAlign(0xf000804c,0,Fld(1,25,AC_MSKB3));  //disable r2r isr
	vIO32WriteFldAlign(0xf0008058,1,Fld(1,25,AC_MSKB3));  //clr r2r status
	if(x_os_drv_reg_isr(VECTOR_R2R, _vHdmiR2rISR, &_pfnR2rIsr) == OSR_OK)
	{
		LOG(0,"R2R register ISR success\n");
		vIO32WriteFldAlign(0xf000804c,1,Fld(1,25,AC_MSKB3));  //enable r2r isr
	}
	else
	{
		LOG(0,"R2R register ISR fail\n");
	}
	
}
void vSetR2RControl(R2R_CONFI *r2rConfigure,UINT8 dviTiming)
{
	UINT8 hdmiTimingIndex = 0xff;
	UINT8 vicCode;
#ifdef CC_HDMI_2_0_SUPPORT
	if(u1GetMinHDMI20Port()<=eActiveHdmiPort)
	{
		if(W2D_NO_NEED != vDviTimingNeedW2d_V2()) //hdmi2.0 timing
		{
			dviTiming = 99;
		}
	}
#endif
	r2rConfigure->u1VB1Enable = fgR2RVB1Enable;   //default to use vb1 ,not capture mode.
	if((Get_VGAMODE_IVTOTAL(dviTiming) < 2000) &&(r2rConfigure->u1VB1Enable))
	{
		r2rConfigure->u1CaptureEnable = TRUE;
		LOG(0,"r2r capture mode enable!\n");
		dviTiming = 99;    //need to change to MODE_3840 for  easy read
	}
	else
	{
		r2rConfigure->u1CaptureEnable = FALSE;
	}
#ifdef CC_HDMI_2_0_SUPPORT
	r2rConfigure->fgR2rInterlaced =	bDvi2Interlace(0);
#else
	r2rConfigure->fgR2rInterlaced = fgHDMIinterlaced();
#endif
	//if((dviTiming == 100) || (dviTiming == 103) || (dviTiming == 104))   //MODE_4096
	if(u1TimingNeedCutByR2R(dviTiming))
	{
		r2rConfigure->u1R2r4096to3840 = FALSE;
		dviTiming = 99;  //MODE_3840
	} 
	else
	{
		r2rConfigure->u1R2r4096to3840 = FALSE;
	}
	if(r2rConfigure->fgR2rInterlaced)
	{

		vicCode = u1HdmiGetActivePortTimingVic();
		hdmiTimingIndex = getIndexAccordingVic(vicCode);
	}
	if(hdmiTimingIndex!=0xff)
	{
		r2rConfigure->u4Htotal = hdmiVicTimingSpec[hdmiTimingIndex].u4Htotal;
		r2rConfigure->u4Width = hdmiVicTimingSpec[hdmiTimingIndex].u4Hde;
		r2rConfigure->u4HdeStart = hdmiVicTimingSpec[hdmiTimingIndex].u4HsyncWidth + hdmiVicTimingSpec[hdmiTimingIndex].u4HsyncbackPorch + 1;
		r2rConfigure->u4HsyncWidth = hdmiVicTimingSpec[hdmiTimingIndex].u4HsyncWidth;
	}
	else
	{
		r2rConfigure->u4Htotal = Get_VGAMODE_IHTOTAL(dviTiming);
		r2rConfigure->u4Width = Get_VGAMODE_IPH_WID(dviTiming);
		r2rConfigure->u4HdeStart = Get_VGAMODE_IPH_STA(dviTiming);
		r2rConfigure->u4HsyncWidth = Get_VGAMODE_IPH_SYNCW(dviTiming);
	}
	if((w2d_configures.u4W2DLeftAndRight) && (dviTiming != 99))
	{		
		r2rConfigure->u4Htotal = ((r2rConfigure->u4Htotal)<<1);
		r2rConfigure->u4Width = ((r2rConfigure->u4Width)<<1);
		r2rConfigure->u4HsyncWidth = ((r2rConfigure->u4HsyncWidth)<<1);
		r2rConfigure->u4HdeStart = ((r2rConfigure->u4HdeStart)<<1);
	}
	
	r2rConfigure->u4HdeEnd = r2rConfigure->u4Width + r2rConfigure->u4HdeStart - 1;
	if(hdmiTimingIndex!=0xff)
	{
		r2rConfigure->u4Vtotal = hdmiVicTimingSpec[hdmiTimingIndex].u4Vtotal;
		r2rConfigure->u4Height = hdmiVicTimingSpec[hdmiTimingIndex].u4Vde;
	}
	else
	{
		r2rConfigure->u4Vtotal = Get_VGAMODE_IVTOTAL(dviTiming);	
		r2rConfigure->u4Height = Get_VGAMODE_IPV_LEN(dviTiming);
	}
	r2rConfigure->u4VsyncStart = 1;
	r2rConfigure->u4VsyncEnd = 0x14; //need confirm.
	r2rConfigure->u4VsyncEvenStart = r2rConfigure->u4Vtotal-1;	
	r2rConfigure->u4VsyncEvenEnd = r2rConfigure->u4Vtotal + 1 + 0x14;

	
	if(r2rConfigure->fgR2rInterlaced)
	{
		if(hdmiTimingIndex!=0xff)
		{
			r2rConfigure->u4VdeStart = hdmiVicTimingSpec[hdmiTimingIndex].u4VsyncWidth + hdmiVicTimingSpec[hdmiTimingIndex].u4VsyncbackPorch + 1;
			r2rConfigure->u4VdeEnd = r2rConfigure->u4VdeStart + r2rConfigure->u4Height -1;
			r2rConfigure->u4VdeEvenStart = r2rConfigure->u4VdeEnd + hdmiVicTimingSpec[hdmiTimingIndex].u4VsyncbackPorch + hdmiVicTimingSpec[hdmiTimingIndex].u4VsyncWidth + hdmiVicTimingSpec[hdmiTimingIndex].u4VsyncFrontPorch + 1 + 1;
			r2rConfigure->u4VdeEvenEnd = r2rConfigure->u4VdeEvenStart +  hdmiVicTimingSpec[hdmiTimingIndex].u4Vde - 1;

		}
		else
		{
			r2rConfigure->u4VdeStart = Get_VGAMODE_IPV_STA(dviTiming) + 1;
			r2rConfigure->u4VdeEnd = r2rConfigure->u4VdeStart + r2rConfigure->u4Height/2 -1;
			r2rConfigure->u4VdeEvenEnd = r2rConfigure->u4Vtotal - 2;
			r2rConfigure->u4VdeEvenStart = r2rConfigure->u4VdeEvenEnd - r2rConfigure->u4Height/2 + 1;
		}
	}
	else
	{
		r2rConfigure->u4VdeStart = Get_VGAMODE_IPV_STA(dviTiming) + 1;
		r2rConfigure->u4VdeEnd = r2rConfigure->u4VdeStart + r2rConfigure->u4Height -1;
	}
	r2rConfigure->u1BitNum = w2d_configures.u1BitNum;
	{
		HDMI_3D_INFOFRAME *info;
		
    	info = API_HDMI_3D_Get_InfoFrame();
		if ((info->HDMI_3D_Enable == 1) && (info->HDMI_3D_Structure == HDMI_3D_Structure_FramePacking))
        {
        	UINT32 u4TwoDHeight;
        	r2rConfigure->fgR2r3DFp = TRUE;
			u4TwoDHeight = (r2rConfigure->u4Height - (r2rConfigure->u4Vtotal - r2rConfigure->u4Height))/2;
			r2rConfigure->u43DFpVdeStart = r2rConfigure->u4VdeStart + u4TwoDHeight;
			r2rConfigure->u43DFpVdeEnd = r2rConfigure->u4VdeEnd - u4TwoDHeight;
			r2rConfigure->u43DFpVsyncStart = r2rConfigure->u4VsyncStart + r2rConfigure->u4Vtotal;
			r2rConfigure->u43DFpVsyncEnd = r2rConfigure->u43DFpVsyncStart + (0x14-1);
        }
		else
		{
        	r2rConfigure->fgR2r3DFp = FALSE;
		}
	}
	r2rConfigure->u4R2rMuteCnt = u4R2rModechangeCnt;  //default is 10 counter to mute video
}
void vConfiR2R(R2R_CONFI r2rConfigure)
{

	vR2RSetBypassMode(r2rConfigure.u1VB1Enable);
	vR2RSetHtoalAndVtotal(r2rConfigure.u4Htotal,r2rConfigure.u4Vtotal);
	vR2RSetHsyncWidth(r2rConfigure.u4HsyncWidth);
	if(r2rConfigure.u1CaptureEnable)
	{
		vR2RSetHdewAndVdew(w2d_configures.u4Width,w2d_configures.u4Height);
	}
	else
	{	
		if(r2rConfigure.fgR2rInterlaced)
		{
			vR2RSetHdewAndVdew(r2rConfigure.u4Width,r2rConfigure.u4Height);
		}
		else
		{
			vR2RSetHdewAndVdew(r2rConfigure.u4Width,r2rConfigure.u4Height);
		}
	}
	vR2RSetVsync(r2rConfigure.u4VsyncStart,r2rConfigure.u4VsyncEnd,r2rConfigure.u4VsyncEvenStart,r2rConfigure.u4VsyncEvenEnd);
	vR2RSetHde(r2rConfigure.u4HdeStart,r2rConfigure.u4HdeEnd);
	vR2RSetVde(r2rConfigure.u4VdeStart,r2rConfigure.u4VdeEnd,r2rConfigure.u4VdeEvenStart,r2rConfigure.u4VdeEvenEnd);
	vIO32WriteFldAlign(R2R_C_R2R_REG14,	r2rConfigure.fgR2rInterlaced?0:1,c_r2r_prgs);
	if(r2rConfigure.u1R2r4096to3840)
	{
		vR2RSetPitch(4096,r2rConfigure.u1BitNum);		//when 4096 in but r2r 3084 out, pitch need to set 4096 Because dram pitch is 4096.
	}
	else
	{	
		vR2RSetPitch(r2rConfigure.u4Width,r2rConfigure.u1BitNum);
	}
	vR2RSetWordMax(r2rConfigure.u4Width,r2rConfigure.u1BitNum);
	vR2RSetPacketMode(r2rConfigure.u1BitNum);
	vR2rConfigFptoFs(r2rConfigure.fgR2r3DFp,r2rConfigure.u43DFpVsyncStart,r2rConfigure.u43DFpVsyncEnd,r2rConfigure.u43DFpVdeStart,r2rConfigure.u43DFpVdeEnd);
	if(r2rConfigure.u4R2rMuteCnt > 0)
	{
		vR2RSetPatternEn(1,0,0,0);
	}
}
void vHDMIW2DR2RSetting(UINT8 u1PortL,UINT8 u1PortR)
{
	vW2dSetVideoTextureControl(w2d_confi_videoTexture.hdmiPortId,0);
	vSetW2DControl(&w2d_configures,_bDviTiming);
	vConfiW2D(w2d_configures);
	vSetR2RControl(&r2r_configures,_bDviTiming);
	vConfiR2R(r2r_configures);
#if defined(CC_MT5890)
#ifdef CC_SUPPORT_4KBE
	vR2rconfigVb1Display(r2r_configures);   //config display vb1 control bit
#endif
#endif

#ifdef CC_SUPPORT_BOSTON_SWITCH
		if(w2d_configures.u4W2DLeftAndRight)
		{
			vW2DLeftSetInputDataAndClk(u1PortR);  //set w2d L :data and clk
			vW2DRightSetInputDataAndClk(u1PortL); //set w2d R: data and clk
		}
		else
		{
			vW2DLeftSetInputDataAndClk(u1PortL);  //set w2d L :data and clk
			vW2DRightSetInputDataAndClk(u1PortR); //set w2d R: data and clk 	
		}
#else
		vW2DLeftSetInputDataAndClk(u1PortL);  //set w2d L :data and clk
		vW2DRightSetInputDataAndClk(u1PortR); //set w2d R: data and clk
#endif
	
	if(rW2DR2RInfo.u4Size == 0)
	{
		vW2dR2rFbmInit();
	}
	vConfigW2DR2RAddr(rW2DR2RInfo.u4Addr,rW2DR2RInfoCHc.u4Addr,w2d_configures.u4Width,w2d_configures.u1BitNum);
	vW2dR2rReadWriteSyncInit();
	vR2RSetFrameAddr(r2rReadFrameAddr[r2rReadFrameIndex]);  //init to read frame 2
	vW2DSetFrameAddr(w2dWriteFrameAddr[w2dWriteFrameIndex]); //init to write frame 0
	if(fgR2RVB1Enable)
	{
		vR2RtoVB1Path();
		vR2RSelVB1Oclk();
	}
	else
	{
		vR2RSelPortTiming(u1PortL);
		vR2RSetOmuxOutput(1,1);
		vR2RSetOmuxOutput(0,1);	
	}
	//vR2RSelMainCLK();  // main clk should not be r2r if non hdmi source is selected.
	vW2dR2rConfiColorFormatTrans();
	vW2dReset();
	vR2RReset();
	if(w2d_configures.u4W2DLeftAndRight)
	{
		vW2DEnable(0);
		vW2DEnable(1);
	}
	else
	{
		vW2DEnable(1);
		vW2DDisable(0);		
	}
	//vR2rEnable(1);
}
void vHdmiW2dIntHandle(void)
{
	//clear w2d int status
	static UINT32 preVtotal;
	UINT32 newVtotal;
	HAL_TIME_T rTtmp;
	HAL_GetTime(&rTtmp);
	HAL_GetDeltaTime(&rW2DIntTimeStep, &rW2DTimeTmp, &rTtmp);
	rW2DTimeTmp = rTtmp;
	if(w2dThreadRun == 2)
	LOG(0,"W2d int time step: %d \n",1000000 * rW2DIntTimeStep.u4Seconds + rW2DIntTimeStep.u4Micros);
#ifdef W2D_R2R_TEST	
	w2dIntCnt ++;
#endif

	newVtotal =  wHDMIVTotal();
	if(newVtotal > preVtotal)
	{
		fgFieldInfo[w2dWriteFrameIndex] = 1;
	}
	else
	{
		fgFieldInfo[w2dWriteFrameIndex] = 0;
	}	
	preVtotal = newVtotal;
	vW2DClrLeftInt();
	vW2DClrRightInt();
	{
		w2d_configures.u1W2DIntMonitorCnt = 10;  // have int ,reset to 60.
		if(w2d_configures.u1W2DIntGoodCnt < 10)
		{
			w2d_configures.u1W2DIntGoodCnt ++;
		}
		else
		{
			w2d_configures.u1W2DStatusGoodFlg = 1;
		}
	}
	
	if(IsW2dWritingTxture())
	{
		u4W2dWriteNextFrame();
	}
	if(!IsW2dWritingTxture())
	{
		/*
		if(u1R2rReadFrameDoneCnt >0)
		{
			u4W2dWriteNextFrame();
			u1R2rReadFrameDoneCnt --;		
		}
		else
		{
			//do noting but wait
		}
		if(u1W2dWriteFrameDoneCnt < 2)
		{
			u1W2dWriteFrameDoneCnt ++;
		}
		*/
		u4W2dWriteNextFrame();
	}
}
void vHdmiR2rIntHandle(void)
{
#ifdef W2D_R2R_TEST
	r2rThreadCnt ++;
#endif
	HAL_TIME_T rTtmp;
	HAL_GetTime(&rTtmp);
	HAL_GetDeltaTime(&rR2RIntTimeStep, &rR2RTimeTmp, &rTtmp);
	rR2RTimeTmp = rTtmp;
	vR2RClrR2RInt();
	if(r2rThreadRun == 2)
	{
		Printf("r2r int time step: %d \n",1000000 * rR2RIntTimeStep.u4Seconds + rR2RIntTimeStep.u4Micros);
	}
	if((r2r_configures.u4R2rMuteCnt>0) && (w2d_configures.u1W2DStatusGoodFlg == TRUE))
	{
		r2r_configures.u4R2rMuteCnt --;
		if(r2r_configures.u4R2rMuteCnt == 0)
		{
			vR2RSetPatternEn(0,0,0,0);
		}
	}
	if(r2r_confi_videoTexture.ReadingTxtCnt>0)
	{
		if(!r2r_confi_videoTexture.fgR2rsettingdone)
		{
			vR2rConfiVideoTexture(r2r_confi_videoTexture);
			r2r_confi_videoTexture.fgR2rsettingdone = TRUE;
		}
		r2r_confi_videoTexture.ReadingTxtCnt --;
		if(r2r_confi_videoTexture.ReadingTxtCnt == 0)
		{
			//restore prevoius r2r settings
			r2r_confi_videoTexture.fgR2rsettingdone = FALSE;
			vConfiR2R(r2r_configures);
		}
	}
	if(r2r_confi_videoTexture.ReadingTxtCnt == 0)
	{	/*
		if(u1W2dWriteFrameDoneCnt>0)
		{
			u4R2rReadNextFrame();
			u1W2dWriteFrameDoneCnt--;	
		}
		else
		{
			
		}
		if(u1R2rReadFrameDoneCnt<2)
		{
			u1R2rReadFrameDoneCnt ++;
		}
		*/
		u4R2rReadNextFrame();
	}
}

void _vHdmiR2rISR(UINT16 u2Vector)
{	
    if(u2Vector == VECTOR_R2R)
    {    
	    //clear r2r int status
	    if(vR2RIsR2RInt())
    	{
    		vHdmiR2rIntHandle();
    	}
		VERIFY(BIM_ClearIrq(VECTOR_R2R));
    }
}


static void _r2r_Thread( void * pvArgs )
{
	UNUSED(pvArgs);
	while(1)
		{
			
			if(vR2RIsR2RInt())
			{
				if(r2rThreadRun)
				{
					vHdmiR2rIntHandle();
				}
			}
			x_thread_delay(5);
		}
}
static void _w2d_Thread( void * pvArgs )
{
	UNUSED(pvArgs);
	while(1)
		{
			if(w2dThreadRun)
			{
				if(w2dThreadRun == 1)
				{
					LOG(0,"w2d Thread running\n");
				}
				vHdmiW2dIntHandle();
			}
			x_thread_delay(10);
		}
}

void vHDMICreateR2rThread(void)
{
	VERIFY(x_thread_create(&phR2RThreadHdl, "R2RThread", 1024,  60, _r2r_Thread, 0, NULL) == OSR_OK);
	LOG(0,"creat R2r Thread OK\n");		

}
void vHDMICreateW2dThread(void)
{
	VERIFY(x_thread_create(&phW2DThreadHdl, "W2DThread", 1024,  60, _w2d_Thread, 0, NULL) == OSR_OK);
	LOG(0,"creat W2d Thread OK\n");		
}
void vSetR2RControlCmd(UINT8 u1timingVic,UINT8 u1BitNum,UINT8 u1vb1Enable)
{

	r2r_configures.u4Htotal = Get_VGAMODE_IHTOTAL(u1timingVic);
	r2r_configures.u4Vtotal = Get_VGAMODE_IVTOTAL(u1timingVic);	
	r2r_configures.u1CaptureEnable =(r2r_configures.u4Vtotal < 2000)? TRUE : FALSE;
	r2r_configures.fgR2r3DFp = (r2r_configures.u4Vtotal > 3000)? TRUE : FALSE;
	r2r_configures.u4Height = Get_VGAMODE_IPV_LEN(u1timingVic);
	r2r_configures.u4Width = Get_VGAMODE_IPH_WID(u1timingVic);
	r2r_configures.u4HsyncWidth = Get_VGAMODE_IPH_SYNCW(u1timingVic);
	r2r_configures.u4HdeStart = Get_VGAMODE_IPH_STA(u1timingVic);
	r2r_configures.u4HdeEnd = r2r_configures.u4Width + r2r_configures.u4HdeStart - 1;
	r2r_configures.u4VsyncStart = 1;
	r2r_configures.u4VsyncEnd = 0x14; //need confirm.
	r2r_configures.u4VdeStart = Get_VGAMODE_IPV_STA(u1timingVic) + 1;
	r2r_configures.u4VdeEnd = r2r_configures.u4VdeStart + r2r_configures.u4Height - 1;
	r2r_configures.u1BitNum = u1BitNum;
	r2r_configures.u1VB1Enable = u1vb1Enable;
	vConfiR2R(r2r_configures);
}

void vSetW2dControlCmd(UINT32 u4Height,UINT32 u4Width,UINT8 u1BitNum, UINT8 u1Is422,UINT8 u1LeftAndRight)
{
	w2d_configures.u4Height = u4Height;
	w2d_configures.u4Width= u4Width;
	w2d_configures.u1BitNum= u1BitNum;
	w2d_configures.u1IsHdmi422= u1Is422;
	w2d_configures.u4W2DLeftAndRight= u1LeftAndRight;
	vConfiW2D(w2d_configures);
}

void vR2RSetPatternEn(BOOL fgPatternEn, UINT32 u4YPadding, UINT32 u4UPadding, UINT32 u4VPadding)
{
	if(fgPatternEn)
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG13,0,c_r2r_pattern_en);	
		vIO32WriteFldAlign(R2R_C_R2R_REG01,u4YPadding,c_r2r_padding_y);
		vIO32WriteFldAlign(R2R_C_R2R_REG02,u4UPadding,c_r2r_padding_u);
		vIO32WriteFldAlign(R2R_C_R2R_REG02,u4UPadding,c_r2r_padding_v);
	}
	else
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG13,0,c_r2r_pattern_en);	
	}
}
void vW2DLeftSetInputDataAndClk(UINT8 u1HdmiPortL)
{
	UINT8 u1Temp; //port 1,3'b001  port2 3'b010 port3 3'b100 port4 3'b000
	u1Temp = 1<<(u1HdmiPortL-1);
	u1Temp = u1Temp & 0x7;
	vIO32WriteFldAlign(W2D_DATA_CLK,u1Temp,w2d_l_clk_sel);
	vIO32WriteFldAlign(W2D_DATA_CLK,u1Temp,w2d_l_data_sel);
}
void vW2DRightSetInputDataAndClk(UINT8 u1HdmiPortR)
{
	UINT8 u1Temp; //port 1,3'b001  port2 3'b010 port3 3'b100 port4 3'b000
	u1Temp = 1<<(u1HdmiPortR-1);
	u1Temp = u1Temp & 0x7;
	vIO32WriteFldAlign(W2D_DATA_CLK,u1Temp,w2d_r_clk_sel);
	vIO32WriteFldAlign(W2D_DATA_CLK,u1Temp,w2d_r_data_sel);
}
void vR2RSetOmuxOutput(UINT8 u1MainSub,UINT8 u1OutputEn)
{
	if(u1MainSub == 1)
	{
		vIO32WriteFldAlign(R2R_OMUX_SEL,u1OutputEn?1:0,r2r_sub_omux_sel_bit);
	}
	else if(u1MainSub == 0)
	{
		vIO32WriteFldAlign(R2R_OMUX_SEL,u1OutputEn?1:0,r2r_main_omux_sel_bit);
	}
}
UINT32 u4GetW2dHalfLineSize(UINT32 u4Hactive, UINT8 u1BitNum)
{
	if(u1BitNum == 0)
	{
		u1BitNum = 8;
	}
	return ((u4Hactive / (128/u1BitNum)) * (128/8));
}
void vConfigW2DR2RAddr(UINT32 u4BaseAddr,UINT32 u4BaseAddr2,UINT32 u4Width,UINT8 u1BitNum)
{
	UINT32 u4HalfLineSize;
	UINT8 i;
	UINT32 oneFrameSize = 0xc00000;
	//UINT32 u4W2dLYaddrBase,u4W2dLUaddrBase,u4W2dLVaddrBase;
	//UINT32 u4W2dRYaddrBase,u4W2dRUaddrBase,u4W2dRVaddrBase;
	u4HalfLineSize = u4GetW2dHalfLineSize(1920,u1BitNum);
	if(0)//FHD
	{
		oneFrameSize = 0x300000;
		u1AvailableFrameCnt = 12;
		needFrameDelayCnt = 6;
	}
	else//UHD
	{
		oneFrameSize = 0xc00000;
		u1AvailableFrameCnt = 3;
		needFrameDelayCnt = 1;
	}
	//u4W2dLYaddrBase = u4BaseAddr;
	for(i=0;i<u1AvailableFrameCnt;i++)
	{
//#ifdef USE_FBM_CHC_FOR_W2D_Y
		if(fgUseChC)
		{	
			w2dWriteFrameAddr[i].u4FrameLYaddr = u4BaseAddr2 + i*oneFrameSize;
			w2dWriteFrameAddr[i].u4FrameLUaddr = u4BaseAddr  + i*oneFrameSize*2;
			w2dWriteFrameAddr[i].u4FrameLVaddr = u4BaseAddr + oneFrameSize + i*oneFrameSize*2;
		}
		//#else
		else
		{
				w2dWriteFrameAddr[i].u4FrameLYaddr = u4BaseAddr + i*0x2400000;
				w2dWriteFrameAddr[i].u4FrameLUaddr = u4BaseAddr + 0xc00000 + i*0x2400000;
				w2dWriteFrameAddr[i].u4FrameLVaddr = u4BaseAddr + 0xc00000*2 + i*0x2400000;
		}//#endif
		
		r2rReadFrameAddr[i].u4FrameYaddr = w2dWriteFrameAddr[i].u4FrameLYaddr >> 4;
		r2rReadFrameAddr[i].u4FrameUaddr = w2dWriteFrameAddr[i].u4FrameLUaddr >> 4;
		r2rReadFrameAddr[i].u4FrameVaddr = w2dWriteFrameAddr[i].u4FrameLVaddr >> 4;
		if(r2r_configures.u1R2r4096to3840)
		{
			r2rReadFrameAddr[i].u4FrameYaddr += (((128/(128/u1BitNum))*16)>>4);  //12bit will be error?
			r2rReadFrameAddr[i].u4FrameUaddr += (((128/(128/u1BitNum))*16)>>4);  //12bit will be error?
			r2rReadFrameAddr[i].u4FrameVaddr += (((128/(128/u1BitNum))*16)>>4);  //12bit will be error?
		}
		w2dWriteFrameAddr[i].u4FrameRYaddr = (w2dWriteFrameAddr[i].u4FrameLYaddr + u4HalfLineSize) >> 4;
		w2dWriteFrameAddr[i].u4FrameRUaddr = (w2dWriteFrameAddr[i].u4FrameLUaddr + u4HalfLineSize) >> 4;
		w2dWriteFrameAddr[i].u4FrameRVaddr = (w2dWriteFrameAddr[i].u4FrameLVaddr + u4HalfLineSize) >> 4;

		
		w2dWriteFrameAddr[i].u4FrameLYaddr = w2dWriteFrameAddr[i].u4FrameLYaddr >> 4;
		w2dWriteFrameAddr[i].u4FrameLUaddr = w2dWriteFrameAddr[i].u4FrameLUaddr >> 4;
		w2dWriteFrameAddr[i].u4FrameLVaddr = w2dWriteFrameAddr[i].u4FrameLVaddr >> 4;
	}
}
void vW2dR2rFbmInit(void)
{
	rW2DR2RInfo = *FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_W2D_R2R, NULL); 
	rW2DR2RInfoCHc = *FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_W2D_R2R_Y, NULL); 
}
void vW2dR2rInit(void)
{
	vW2DInit();
	vR2RInit();
	vR2RRegisterIsr();
	vW2dR2rFbmInit();
	vR2rRegisterDisplayCB();
}
void vDisableW2dR2r(void)
{
	vW2DDisable(0);
	vW2DDisable(1);
	vR2REnableDram(0);
	vR2REnableTiming(0);
}
void vEnableW2dR2r(void)
{
	if(w2d_configures.u4W2DLeftAndRight)
	{
		vW2DEnable(0);
		vW2DEnable(1);
	}
	else
	{
		vW2DEnable(1);
		vW2DDisable(0);		
	}
	vR2REnableDram(1);
	vR2REnableTiming(1);
}
void vW2dR2rPrintStatus(void)
{
	UINT8 i;
	UINT32 u4FrameSize;
	LOG(0,"w2d config:\n");
	LOG(0,"w2d_configures.u1BitNum : %d\n",w2d_configures.u1BitNum);
	LOG(0,"w2d_configures.u1IsHdmi422 : %d\n",w2d_configures.u1IsHdmi422);
	LOG(0,"w2d_configures.u4Height : %d\n",w2d_configures.u4Height);
	LOG(0,"w2d_configures.u4Width : %d\n",w2d_configures.u4Width);
	LOG(0,"w2d_configures.u4W2DLeftAndRight : %d\n",w2d_configures.u4W2DLeftAndRight);

	LOG(0,"r2r config:\n");
	LOG(0,"r2r_configures.u1BitNum : %d\n",r2r_configures.u1BitNum);
	LOG(0,"r2r_configures.u1VB1Enable : %d\n",r2r_configures.u1VB1Enable);
	LOG(0,"r2r_configures.u4HdeStart : %d\n",r2r_configures.u4HdeStart);
	LOG(0,"r2r_configures.u4HdeEnd : %d\n",r2r_configures.u4HdeEnd);
	LOG(0,"r2r_configures.u4Height : %d\n",r2r_configures.u4Height);
	LOG(0,"r2r_configures.u4HsyncWidth : %d\n",r2r_configures.u4HsyncWidth);
	LOG(0,"r2r_configures.u4Htotal : %d\n",r2r_configures.u4Htotal);
	LOG(0,"r2r_configures.u4VdeStart : %d\n",r2r_configures.u4VdeStart);
	LOG(0,"r2r_configures.u4VdeEnd : %d\n",r2r_configures.u4VdeEnd);
	LOG(0,"r2r_configures.u4VsyncStart : %d\n",r2r_configures.u4VsyncStart);
	LOG(0,"r2r_configures.u4VsyncEnd : %d\n",r2r_configures.u4VsyncEnd);
	LOG(0,"r2r_configures.u4Vtotal : %d\n",r2r_configures.u4Vtotal);
	LOG(0,"r2r_configures.u4Width : %d\n",r2r_configures.u4Width);
	LOG(0,"r2r_configures.u1CaptureEnable : %d\n",r2r_configures.u1CaptureEnable);
	LOG(0,"r2r_configures.u1R2r4096to3840 : %d\n",r2r_configures.u1R2r4096to3840);
	LOG(0,"r2r_configures.fgR2r3DFp : %d\n",r2r_configures.fgR2r3DFp);
	LOG(0,"r2r_configures.u43DFpVsyncStart : %d\n",r2r_configures.u43DFpVsyncStart);
	LOG(0,"r2r_configures.u43DFpVsyncEnd : %d\n",r2r_configures.u43DFpVsyncEnd);
	LOG(0,"r2r_configures.u43DFpVdeStart : %d\n",r2r_configures.u43DFpVdeStart);
	LOG(0,"r2r_configures.u43DFpVdeEnd : %d\n",r2r_configures.u43DFpVdeEnd);




	LOG(0,"rW2DR2RInfo :\n");	
	LOG(0,"rW2DR2RInfo.fgAlloced : %d\n",rW2DR2RInfo.fgAlloced);
	LOG(0,"rW2DR2RInfo.u4Addr : %x\n",rW2DR2RInfo.u4Addr);
	LOG(0,"rW2DR2RInfo.u4Size : %x\n",rW2DR2RInfo.u4Size);
	LOG(0,"rW2DR2RInfo.u4Height : %d\n",rW2DR2RInfo.u4Height);
	LOG(0,"rW2DR2RInfo.u4Inherit : %d\n",rW2DR2RInfo.u4Inherit);
	LOG(0,"rW2DR2RInfo.u4Mode : %d\n",rW2DR2RInfo.u4Mode);
	LOG(0,"rW2DR2RInfo.u4Width : %d\n",rW2DR2RInfo.u4Width);
	

	LOG(0,"w2d int step: %d\n",rW2DIntTimeStep.u4Seconds*1000000+rW2DIntTimeStep.u4Micros);
	LOG(0,"r2r int step: %d\n",rR2RIntTimeStep.u4Seconds*1000000+rR2RIntTimeStep.u4Micros);
	LOG(0,"rW2DChangeAddrTimeStep: %d\n",rW2DChangeAddrTimeStep.u4Seconds*1000000+rW2DChangeAddrTimeStep.u4Micros);
	LOG(0,"rR2RChangeAddrTimeStep: %d\n",rR2RChangeAddrTimeStep.u4Seconds*1000000+rR2RChangeAddrTimeStep.u4Micros);
	u4FrameSize = u4W2dFrameSize(w2d_configures.u4Height,w2d_configures.u4Width,w2d_configures.u1BitNum);
	if(w2d_confi_videoTexture.txtureModeEnable)
	{	
		u4FrameSize = u4W2dFrameSize(w2d_confi_videoTexture.eW2dConfi.u4Height,w2d_confi_videoTexture.eW2dConfi.u4Width,w2d_confi_videoTexture.eW2dConfi.u1BitNum);		
	}
	LOG(0,"Frame Size: %x  \n",u4FrameSize);
	LOG(0,"w2d r2r Frame buffer info\n");
	for(i=0;i<3;i++)
	{
		LOG(0,"w2d Frame buffer num %d\n",i);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameLYaddr: %x ~ %x\n",i,w2dWriteFrameAddr[i].u4FrameLYaddr, (u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameLYaddr);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameLUaddr: %x ~ %x \n",i,w2dWriteFrameAddr[i].u4FrameLUaddr,(u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameLUaddr);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameLVaddr: %x ~ %x \n",i,w2dWriteFrameAddr[i].u4FrameLVaddr,(u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameLVaddr);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameRYaddr: %x ~ %x \n",i,w2dWriteFrameAddr[i].u4FrameRYaddr,(u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameRYaddr);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameRUaddr: %x ~ %x \n",i,w2dWriteFrameAddr[i].u4FrameRUaddr,(u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameRUaddr);
		LOG(0,"w2dWriteFrameAddr[%d].u4FrameRVaddr: %x ~ %x \n",i,w2dWriteFrameAddr[i].u4FrameRVaddr,(u4FrameSize>>4) + w2dWriteFrameAddr[i].u4FrameRVaddr);
		
		LOG(0,"r2r Frame buffer num %d\n",i);
		LOG(0,"r2rReadFrameAddr[%d].u4FrameYaddr: %x ~ %x \n",i,r2rReadFrameAddr[i].u4FrameYaddr,(u4FrameSize>>4) + r2rReadFrameAddr[i].u4FrameYaddr);
		LOG(0,"r2rReadFrameAddr[%d].u4FrameUaddr: %x ~ %x \n",i,r2rReadFrameAddr[i].u4FrameUaddr,(u4FrameSize>>4) + r2rReadFrameAddr[i].u4FrameUaddr);
		LOG(0,"r2rReadFrameAddr[%d].u4FrameVaddr: %x ~ %x \n",i,r2rReadFrameAddr[i].u4FrameVaddr,(u4FrameSize>>4) + r2rReadFrameAddr[i].u4FrameVaddr);
	}

	LOG(0,"w2d write video txture info:  \n");
	
	LOG(0,"w2d_confi_videoTexture.txtureModeEnable : %d \n",w2d_confi_videoTexture.txtureModeEnable);
	if(w2d_confi_videoTexture.txtureModeEnable)
	{
		LOG(0,"w2d_confi_videoTexture.hdmiPortId : %d \n",w2d_confi_videoTexture.hdmiPortId);
		LOG(0,"w2d_confi_videoTexture.timingStable : %d \n",w2d_confi_videoTexture.timingStable);
		LOG(0,"w2d_confi_videoTexture.timingStableCnt : %d \n",w2d_confi_videoTexture.timingStableCnt);
		LOG(0,"w2d_confi_videoTexture.eW2dConfi.u1BitNum : %d\n",w2d_confi_videoTexture.eW2dConfi.u1BitNum);
		LOG(0,"w2d_confi_videoTexture.eW2dConfi.u1IsHdmi422 : %d\n",w2d_confi_videoTexture.eW2dConfi.u1IsHdmi422);
		LOG(0,"w2d_confi_videoTexture.eW2dConfi.u4Height : %d\n",w2d_confi_videoTexture.eW2dConfi.u4Height);
		LOG(0,"w2d_confi_videoTexture.eW2dConfi.u4Width : %d\n",w2d_confi_videoTexture.eW2dConfi.u4Width);
		LOG(0,"w2d_confi_videoTexture.eW2dConfi.u4W2DLeftAndRight : %d\n",w2d_confi_videoTexture.eW2dConfi.u4W2DLeftAndRight);

	}
	LOG(0,"w2d r2r HW info:  \n");
	
	LOG(0,"w2d enable L : %d \n",IO32ReadFldAlign(W2D_LW2D_ENABLE ,BGenvdoin_on));
	LOG(0,"w2d enable R : %d \n",IO32ReadFldAlign(W2D_LW2D_ENABLE + 0x40 ,BGenvdoin_on));
	LOG(0,"r2r enable dram : %d \n",IO32ReadFldAlign(R2R_C_R2R_REG14 ,c_r2r_enable_dram));
	LOG(0,"r2r enable timing : %d \n",IO32ReadFldAlign(R2R_C_R2R_REG15 ,c_r2r_enable_timing));

	LOG(0,"w2dWriteFrameIndex : %d\n",w2dWriteFrameIndex);
	LOG(0,"r2rReadFrameIndex : %d\n",r2rReadFrameIndex);
}
void vR2RSelVB1Oclk(void)
{	
	vIO32WriteFldAlign(R2R_SEL_CLOCK,1,OCLK_SEL);   //14
	vIO32WriteFldAlign(R2R_SEL_CLOCK,1,OCLK_R2RCLK_SEL); // 15
	vIO32WriteFldAlign(R2R_CLOCK_EN,0,R2R_CLK_EN);	// 0d100 12
}
void vR2RSelMainCLK(void)
{
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,0,MAIN_CLK);
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,0,CCH_CLK);
}
void vCCIRSelMainCLK(void)
{
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,1,MAIN_CLK);
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,1,CCH_CLK);
}
void vR2RSelSubCLK(void)
{
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,0,PIP_CLK);
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,0,CCH_CLK_PIP);
}
void vCCIRSelSubCLK(void)
{
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,1,MAIN_CLK);
	vIO32WriteFldAlign(R2R_MAIN_PIP_CLK_SEL,1,CCH_CLK);
}

void vR2RSelPortTiming(UINT8 u1Port)
{
	vIO32WriteFldAlign(R2R_SEL_CLOCK,(u1Port - 1),R2RCLK_SEL);	// 12  13
	vIO32WriteFldAlign(R2R_SEL_CLOCK,0,OCLK_R2RCLK_SEL); // 15 
	vIO32WriteFldAlign(R2R_CLOCK_EN,0,R2R_CLK_EN);	// 0d100 12
}

void vR2RCCIRSelCLK(UINT8 u1R2R)
{
	if(u1R2R)
	{
		vR2RSelMainCLK();
		vR2RSelSubCLK();
		vR2RSetOmuxOutput(0,1);
		vR2RSetOmuxOutput(1,1);
	}
	else
	{
		vCCIRSelMainCLK();
		vCCIRSelSubCLK();
		vR2RSetOmuxOutput(0,0);
		vR2RSetOmuxOutput(1,0);
	}
}
UINT32 u4W2dFrameSize(UINT32 height, UINT32 width, UINT8 bitNum)
{
	UINT32 u4FrameSize;
	if(bitNum == 0) bitNum = 8;
	u4FrameSize = (width / (128/bitNum)) * 16 * height;
	return u4FrameSize;
}
void u4R2rReadNextFrame(void)
{	
	HAL_TIME_T rTtmp, rDelta;
	HAL_GetTime(&rTtmp);
	HAL_GetDeltaTime(&rDelta, &rR2RChangeAddrTimeStep, &rTtmp);
	rR2RChangeAddrTimeStep = rTtmp;
	
	if(r2rThreadRun == 5)
	{
		Printf("r2r change time step: %d \n",1000000 * rDelta.u4Seconds + rDelta.u4Micros);
	}
	/*
	if(r2rReadFrameIndex < 2)
	{
		r2rReadFrameIndex ++;
	}
	else
	{
		r2rReadFrameIndex = 0;
	}
	*/
	if(needFrameDelayCnt >= u1AvailableFrameCnt-1)
	{
		needFrameDelayCnt = u1AvailableFrameCnt - 2;
	}
	if(w2dWriteFrameIndex >= needFrameDelayCnt)
	{
		r2rReadFrameIndex = w2dWriteFrameIndex - needFrameDelayCnt;
	}
	else
	{
		r2rReadFrameIndex = w2dWriteFrameIndex + u1AvailableFrameCnt - needFrameDelayCnt;
	}
	if(fgHDMIinterlaced())
	{
		
		if(r2rThreadRun == 9)
		{
			printf("Omux : %d\n",getOmuxOutputField());
		}
		if(getOmuxOutputField() == DiFieldCtrl)
		{
			if(fgFieldInfo[r2rReadFrameIndex] == fieldCtrl)
			{
			}
			else
			{
				if((r2rReadFrameIndex+1) >= u1AvailableFrameCnt)
				{
					r2rReadFrameIndex =0;
				}
				else
				{
					r2rReadFrameIndex++;
				}
				if(r2rThreadRun == 8)
				printf("automatically frame delay fine tune \n");
			}
			vR2RSetFrameAddr(r2rReadFrameAddr[r2rReadFrameIndex]);	
			if((r2rReadFrameIndex+1) >= u1AvailableFrameCnt)
			{
				vR2RSetBottomFrameAddr(r2rReadFrameAddr[0]);			
			}
			else
			{
				vR2RSetBottomFrameAddr(r2rReadFrameAddr[r2rReadFrameIndex + 1]);			
			}

		}
	}
	else
	{
		vR2RSetFrameAddr(r2rReadFrameAddr[r2rReadFrameIndex]);	
	}
}
void u4W2dWriteNextFrame(void)
{	
	HAL_TIME_T rTtmp, rDelta;
	HAL_GetTime(&rTtmp);
	HAL_GetDeltaTime(&rDelta, &rW2DChangeAddrTimeStep, &rTtmp);
	rW2DChangeAddrTimeStep = rTtmp;
	
	if(w2dThreadRun == 5)
	{
		Printf("w2d change time step: %d \n",1000000 * rDelta.u4Seconds + rDelta.u4Micros);
	}
	/*
	if(w2dWriteFrameIndex < 2)
	{
		w2dWriteFrameIndex ++;
	}
	else
	{
		w2dWriteFrameIndex = 0;
	}*/
	if((++w2dWriteFrameIndex) == u1AvailableFrameCnt)
	{
		w2dWriteFrameIndex = 0;
	}
	vW2DSetFrameAddr(w2dWriteFrameAddr[w2dWriteFrameIndex]);
}
void vW2dR2rUseSameFrame(UINT8 fgSame)
{
	fgW2dR2rAddrSame = fgSame;
}
void vW2dForceLR(UINT8 fgLR)
{
	fgW2dForceLeftAndRight = fgLR;
}
void vR2RtoVB1Path(void)
{
	//vIO32WriteFldAlign(0xf0026064,0,Fld(1,8,AC_MSKB1));// 0 r2r ;1 b2r
	vSetVB1SrcType(0);
}
W2D_CONFI_TYPE vDviTimingNeedW2d(UINT8 u1Dvitiming)
{
	if((u1Dvitiming>=97)&&(u1Dvitiming<=102))
		{

			if((u1Dvitiming>=101))
				{
					return W2D_USE_LEFT_AND_RIGHT;
				}
			else
				{
					return W2D_USE_LEFT_ONLY;
				}
		}
	else
		{
			if(fgForceUseW2D)
				{
					return W2D_USE_LEFT_ONLY;
				}
			return W2D_NO_NEED;
		}
}
#ifdef CC_HDMI_2_0_SUPPORT
W2D_CONFI_TYPE vDviTimingNeedW2d_V2(void)
{
	if(bDvi23DPacketVaild(0)) //TODO: How to deal with Sub source?
	{
		return W2D_NO_NEED; //TODO: Does 3D timing need W2D???
	}
	else
	{
		if(wDvi2InputHeight(0) < 2000) //TODO: How to deal with Sub source?
		{
			return W2D_NO_NEED;
		}
		else
		{
			if(fgHDMI2GetDuleMode(eActiveHdmiPort)) //>3G bps timing and YC420 8bit(3G bps)
			{
				return W2D_USE_LEFT_AND_RIGHT;
			}
			else // <=3G bps timing(4k2k <30Hz, 8 bit)
			{
				return W2D_USE_LEFT_ONLY;
			}
		}
	}
}
#endif
void vW2dR2rReadWriteSyncInit(void)
{
	u1W2dWriteFrameDoneCnt = 0;
	u1R2rReadFrameDoneCnt = 1;
	r2rReadFrameIndex = 2;
	w2dWriteFrameIndex = 0;
}
void vR2RSetCapureMode(UINT8 fgEnable)
{
	r2r_configures.u1CaptureEnable = fgEnable;
}
void vW2d2K2KtoOmux(UINT8 fgEnable)
{
	fg2k2ktoOmux = fgEnable;
}
void vW2dSetFifoThreshold(UINT8 threshold,UINT8 thresholdY)
{
	vIO32WriteFldAlign(W2D_LW2D_MODE,thresholdY,Bgthreshold);
	vIO32WriteFldAlign(W2D_LW2D_MODE,threshold,V_Bgthreshold);
	vIO32WriteFldAlign(W2D_LW2D_MODE + 0x40,thresholdY,Bgthreshold);
	vIO32WriteFldAlign(W2D_LW2D_MODE + 0x40,threshold,V_Bgthreshold);
}
void vW2dStatusReset(void)
{
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,rst_b);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,0,rst_b);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,rst_b);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,rst_b);
}
void vW2dChangeFifoTh(UINT8 threshold,UINT8 thresholdY)
{
	vW2DDisable(1);
	vW2DDisable(0);
	vW2dSetFifoThreshold(threshold,thresholdY);
	vW2dStatusReset();
	vW2DEnable(1);
	vW2DEnable(0);
}
void vW2dUseChannelC(UINT8 u1Use)
{
	fgUseChC = u1Use;
}
void vW2dSetVideoTextureControl(UINT8 port,UINT8 enable)
{
	if(fgHDMIW2DR2REnable)
	{
		LOG(0,"w2d in use , vdotxture not available,return.\n");		
		w2d_confi_videoTexture.txtureModeEnable = FALSE;
		return;
	}
	w2d_confi_videoTexture.txtureModeEnable = enable;
	w2d_confi_videoTexture.hdmiPortId = port;
	if(enable == 0)
	{
		vW2DDisable(0);
		vW2DDisable(1);
	}
}
void vW2dConfiVideoTexture(W2D_VIDEO_TXT_CONFI ew2dVideoTxt)
{
	UINT32 u4ChALaddr;
	UINT32 u4ChBLaddr;
	UINT32 u4ChCLaddr;
	UINT32 u4ChARaddr;
	UINT32 u4ChBRaddr;
	UINT32 u4ChCRaddr;
	UINT32 u4HalfLineSize;
	u4ChALaddr = ew2dVideoTxt.u4VideoTxtAddr;
	u4ChBLaddr = ew2dVideoTxt.u4VideoTxtAddr + 0xc00000;
	u4ChCLaddr = ew2dVideoTxt.u4VideoTxtAddr + 0xc00000*2;
	u4HalfLineSize = u4GetW2dHalfLineSize(ew2dVideoTxt.eW2dConfi.u4Width,ew2dVideoTxt.eW2dConfi.u1BitNum);
	u4ChARaddr = u4ChALaddr + u4HalfLineSize;
	u4ChBRaddr = u4ChBLaddr + u4HalfLineSize;
	u4ChCRaddr = u4ChCLaddr + u4HalfLineSize;
	u4ChALaddr = (u4ChALaddr>>4);
	u4ChBLaddr = (u4ChBLaddr>>4);
	u4ChCLaddr = (u4ChCLaddr>>4);
	u4ChARaddr = (u4ChARaddr>>4);
	u4ChBRaddr = (u4ChBRaddr>>4);
	u4ChCRaddr = (u4ChCRaddr>>4);	
	vConfiW2D(ew2dVideoTxt.eW2dConfi);
	vW2DSetLAddr(u4ChALaddr,u4ChBLaddr,u4ChCLaddr);
	vW2DSetRAddr(u4ChARaddr,u4ChBRaddr,u4ChCRaddr);
}

void vR2rSetVideoTextureControl(UINT32 u4Addr,UINT8 u1BitNum,UINT32 u4ReadCnt)
{
	r2r_confi_videoTexture.eR2rconfi = r2r_configures;
	r2r_confi_videoTexture.u4VideoTxtAddr = u4Addr;
	r2r_confi_videoTexture.eR2rconfi.u1BitNum = u1BitNum;
	r2r_confi_videoTexture.ReadingTxtCnt = u4ReadCnt;
}
void vR2rConfiVideoTexture(R2R_VIDEO_TXT_CONFI er2rVideoTxt)
{
	UINT32 u4ChALaddr;
	UINT32 u4ChBLaddr;
	UINT32 u4ChCLaddr;
	u4ChALaddr = er2rVideoTxt.u4VideoTxtAddr;
	u4ChBLaddr = er2rVideoTxt.u4VideoTxtAddr + 0xc00000;
	u4ChCLaddr = er2rVideoTxt.u4VideoTxtAddr + 0xc00000*2;
	u4ChALaddr = (u4ChALaddr>>4);
	u4ChBLaddr = (u4ChBLaddr>>4);
	u4ChCLaddr = (u4ChCLaddr>>4);
	vConfiR2R(er2rVideoTxt.eR2rconfi);
	vR2RSetADDR(u4ChALaddr,u4ChBLaddr,u4ChCLaddr);
}
BOOL bR2rSelVB1Path(void)
{
	return (((fgHDMIW2DR2REnable) && (r2r_configures.u1VB1Enable))?1:0);
}
void vR2rConfigFptoFs(UINT8 fgFpInput,UINT32 u4VsyncStart,UINT32 u4VsyncEnd,UINT32 u4VdeStart,UINT32 v4VdeEnd)
{
	if(fgFpInput)
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG11,u4VdeStart,c_r2r_vde_odd_fp2fs_start);
		vIO32WriteFldAlign(R2R_C_R2R_REG11,v4VdeEnd,c_r2r_vde_odd_fp2fs_end);
		vIO32WriteFldAlign(R2R_C_R2R_REG06,u4VsyncStart,c_r2r_vsync_fp2fs_odd_start);
		vIO32WriteFldAlign(R2R_C_R2R_REG06,u4VsyncEnd,c_r2r_vsync_fp2fs_odd_end);
		vIO32WriteFldAlign(R2R_C_R2R_REG14,1,c_r2r_fp2fs_en);	
	}
	else
	{
		vIO32WriteFldAlign(R2R_C_R2R_REG14,0,c_r2r_fp2fs_en);	
	}
}
void vForR2r3D(UINT8 u1R2r3dInput)
{
	r2r_configures.fgR2r3DFp = u1R2r3dInput;
}
void vW2dWriteVdoTxture(UINT8 u1HdmiPort,UINT32 u4StartAddr,UINT8 u1BitNum)
{
	UINT8 bDviTiming;
	bDviTiming =  bDviStdTimingSearchByPort(1,u1HdmiPort);
	if(MODE_NOSUPPORT == bDviTiming)
	{
		LOG(0,"no stable timing input,writing texture stop \n");
		return;
	}
	vSetW2DControl((&(w2d_confi_videoTexture.eW2dConfi)),bDviTiming);
	w2d_confi_videoTexture.u4VideoTxtAddr = u4StartAddr;
	vW2dConfiVideoTexture(w2d_confi_videoTexture);
	w2d_confi_videoTexture.WritingTxtStatus = 1;
	
	vW2DLeftSetInputDataAndClk(u1HdmiPort);
	
	if(w2d_confi_videoTexture.eW2dConfi.u4W2DLeftAndRight)
	{
		vW2DEnable(0);
		vW2DEnable(1);
	}
	else
	{
		vW2DEnable(1);
		vW2DDisable(0);		
	}
	LOG(0,"start to writing video txture %d X %d\n",w2d_confi_videoTexture.eW2dConfi.u4Width,w2d_confi_videoTexture.eW2dConfi.u4Height);
	//wait interrupt to finish writing
}
BOOL IsW2dWritingTxture(void)
{
	return ((w2d_confi_videoTexture.txtureModeEnable)?1:0);
}
void vR2rEnable(UINT8 enable)
{
	vR2REnableDram(enable);
	vR2REnableTiming(enable);	
}
void vR2rSetMuteCnt(UINT32 muteCnt)
{
	u4R2rModechangeCnt = muteCnt;
}
UINT32 u4GetVdoTxtureHdmiPort(UINT32 u4RzId)
{
	return w2d_confi_videoTexture.hdmiPortId;
}
UINT8 u1R2rGetDumpFrameAddr(W2D_VDO_TXTURE_TYPE *pW2dVdoTxture)
{
	if((w2d_confi_videoTexture.timingStable) &&(w2d_confi_videoTexture.txtureModeEnable))
	{
		UINT8 frameBufferIndex;
		
		pW2dVdoTxture->port = w2d_confi_videoTexture.hdmiPortId;      
		switch(w2dWriteFrameIndex)
		{
			case 0: frameBufferIndex = 2;break;
			case 1: frameBufferIndex = 0;break;
			case 2: frameBufferIndex = 1;break;
			default:frameBufferIndex = 0;break;
		}
		pW2dVdoTxture->isRgb = bHDMIInputType();
		pW2dVdoTxture->Yaddr =  w2dWriteFrameAddr[frameBufferIndex].u4FrameLYaddr;
		pW2dVdoTxture->Uaddr =  w2dWriteFrameAddr[frameBufferIndex].u4FrameLUaddr;
		pW2dVdoTxture->Vaddr =  w2dWriteFrameAddr[frameBufferIndex].u4FrameLVaddr;
		pW2dVdoTxture->width =  w2d_confi_videoTexture.eW2dConfi.u4Width;
		pW2dVdoTxture->height = w2d_confi_videoTexture.eW2dConfi.u4Height;
		if(w2d_confi_videoTexture.eW2dConfi.u1BitNum == 0)
		{
			w2d_confi_videoTexture.eW2dConfi.u1BitNum = 8;
		}
		pW2dVdoTxture->pitch =  (pW2dVdoTxture->width)/(128/w2d_confi_videoTexture.eW2dConfi.u1BitNum);
		return 1;
	}
	else
	{
		LOG(0,"enable : %d , stable: %d\n",w2d_confi_videoTexture.txtureModeEnable,w2d_confi_videoTexture.timingStable);
		return 0;
	}
}
void vW2dTxtureModeLoop(void)
{
	if(w2d_confi_videoTexture.txtureModeEnable)
	{
		UINT8 bDviTiming;
		if(u1IsHdmiResStable((E_HDMI_SWITCH_NUM)w2d_confi_videoTexture.hdmiPortId))
		{
		bDviTiming =  bDviStdTimingSearchByPort(1,w2d_confi_videoTexture.hdmiPortId);
		if(bDviTiming == w2d_confi_videoTexture.dvitiming)
		{
			if(w2d_confi_videoTexture.timingStable == FALSE)
			{			
				w2d_confi_videoTexture.timingStableCnt++;
				if(w2d_confi_videoTexture.timingStableCnt >= 10)
				{
					w2d_confi_videoTexture.timingStable = TRUE;
					vSetW2DControl((&(w2d_confi_videoTexture.eW2dConfi)),bDviTiming);
					vConfiW2D(w2d_confi_videoTexture.eW2dConfi);
					vConfigW2DR2RAddr(rW2DR2RInfo.u4Addr,rW2DR2RInfoCHc.u4Addr,w2d_confi_videoTexture.eW2dConfi.u4Width,w2d_confi_videoTexture.eW2dConfi.u1BitNum);

					vW2DSetFrameAddr(w2dWriteFrameAddr[0]);
					vW2DLeftSetInputDataAndClk(w2d_confi_videoTexture.hdmiPortId);
		
					if(w2d_confi_videoTexture.eW2dConfi.u4W2DLeftAndRight)
					{
						vW2DEnable(0);
						vW2DEnable(1);
					}
					else
					{
						vW2DEnable(1);
						vW2DDisable(0); 	
					}
					LOG(0,"start to writing video txture %d X %d\n",w2d_confi_videoTexture.eW2dConfi.u4Width,w2d_confi_videoTexture.eW2dConfi.u4Height);
				}
			}
		}
		else
		{
			w2d_confi_videoTexture.timingStableCnt = 0;
			w2d_confi_videoTexture.timingStable = FALSE;
			LOG(0,"write video txture ,  timing not stable\n");
		}
		w2d_confi_videoTexture.dvitiming = bDviTiming;
		}
		else
		{
			LOG(3,"resolution not stable!\n");
			w2d_confi_videoTexture.timingStable = FALSE;
			w2d_confi_videoTexture.timingStableCnt = 0;
		}
	}
}
void vW2dR2rConfiColorFormatTrans(void)
{
	//vIO32WriteFldAlign(0xf0026068,bHDMIInputType()?0:1,Fld(1,31,AC_MSKB3));	
	UINT8 colorMetri601 = bHDMIClocrimetry();
	UINT8 rgbInput = bHDMIInputType();
#ifdef CC_HDMI_2_0_SUPPORT	
	if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
	{
		vSetVB1Color_Transf(bHDMI2InputType(eActiveHdmiPort)?0:1,4);

	}
	else
#endif
	{
		vSetVB1Color_Transf(rgbInput?0:1,colorMetri601?BT601_2_RGB:BT709_2_RGB);
	
	}
}
#if defined(CC_MT5890)
#ifdef CC_SUPPORT_4KBE
void vR2rconfigVb1Display(R2R_CONFI eR2rconfi)
{
	UINT8 colorRgb, color601;
	UINT8 frameRate;
	UINT8 hdmiRange;
	CONTROL_BIT_VB1_T eVb1control;
	eVb1control.u13DStructure = eR2rconfi.fgR2r3DFp;
	eVb1control.u1AspectRatio = 1;
	eVb1control.u1AspectType = 0;
	eVb1control.u1CodeType = 0;
	colorRgb = bHDMIInputType();
	color601 = bHDMIClocrimetry();
	if(colorRgb == 1)
	{
		eVb1control.u1ColorFormat = 3;
	}
	else if(color601 == 1)
	{
		eVb1control.u1ColorFormat = 1;
	}
	else
	{
		eVb1control.u1ColorFormat = 0;
	}
	eVb1control.u1CtrlBitValid = 1;
	frameRate = bHDMIRefreshRate();
	eVb1control.u1DoubleRF = ((frameRate<=30)?1:0);
	hdmiRange = bHDMIRgbRange();
	eVb1control.u1Range = ((hdmiRange == 1)?0:1);
	eVb1control.u1VideoPath = 1;
	eVb1control.u4CropHPosition = 0;
	eVb1control.u4CropHSize = eR2rconfi.u4Width;
	eVb1control.u4CropVPosition = 0;
	eVb1control.u4CropVSize = eR2rconfi.u4Height;
	eVb1control.u4PicHSize = eR2rconfi.u4Width;
	eVb1control.u4PicVSize = eR2rconfi.u4Height;
	eVb1control.u4SarHSize = 0;
	eVb1control.u4SarVSize = 0;
	vDrvUpdateVB1ControlBit(&eVb1control);
}
#endif
#endif
void vW2dReset(void)
{
	
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,0,rst_b);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,0,rst_b);
	vUtDelay2us(10);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE,1,rst_b);
	vIO32WriteFldAlign(W2D_LW2D_ENABLE + 0x40,1,rst_b);	
}
W2D_STA_MONITOR_T w2dStaMonitor[2];

void vW2dMonitorStaMachine(void)
{
	
	if(fgHDMIW2DR2REnable)
	{
		if(w2d_configures.u1W2DIntMonitorCnt-- == 0)
		{
			w2d_configures.u1W2DIntMonitorCnt = 10;	
			vW2DDisable(0);
			vW2DDisable(1);
			vUtDelay2us(10);
			vW2DEnable(1);
			vW2DEnable(0);
			vR2RSetPatternEn(1,0,0,0);
			r2r_configures.u4R2rMuteCnt = 10;
			
			printf("w2d int status wrong,reset \n");
		}
	}
}
void vW2dR2rErrorHandle(void)
{
	vW2dMonitorStaMachine();
}
void vR2rSetHActive(UINT16 hAcvite)
{
	UINT32 u4HdeEnd = r2r_configures.u4HdeStart + hAcvite - 1;
	if(u4HdeEnd!= r2r_configures.u4HdeEnd)
	{
		printf("conflict setting hdeEnd:%d,aplly r2r settign....................\n",u4HdeEnd);
		u4HdeEnd = r2r_configures.u4HdeEnd;
	}
	vR2RSetHde(r2r_configures.u4HdeStart,r2r_configures.u4HdeStart + hAcvite - 1);
}
void vR2rSetVActive(UINT16 vAcvite)
{
	UINT32 vdeEnd = r2r_configures.u4VdeStart + vAcvite - 1;
	if(vdeEnd!= r2r_configures.u4VdeEnd)
	{
		printf("conflict setting vdeEnd:%d,aplly r2r settign....................\n",vdeEnd);
		vdeEnd = r2r_configures.u4VdeEnd;
	}
	vR2RSetVde(r2r_configures.u4VdeStart,r2r_configures.u4VdeStart + vAcvite - 1,r2r_configures.u4VdeEvenStart,r2r_configures.u4VdeEvenStart + vAcvite - 1);
}

void vR2rSetHTotal(UINT16 hTotal)
{
	
	if(fgIsR2REnableBypass())
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG00,hTotal>>1,c_r2r_hsync_total); 
	}
	else
	{		
		vIO32WriteFldAlign(R2R_C_R2R_REG00,hTotal,c_r2r_hsync_total); 
	}	
}
UINT32 vR2rGetHTotal(void)
{
	UINT32 htotal;
	htotal = IO32ReadFldAlign(R2R_C_R2R_REG00,c_r2r_hsync_total);
	return htotal;
}

void vR2rSetVTotal(UINT16 vTotal)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG00,vTotal,c_r2r_vsync_total);	
}
UINT32 vR2rGetVTotal(void)
{
	UINT32 vtotal;
	vtotal = IO32ReadFldAlign(R2R_C_R2R_REG00,c_r2r_vsync_total);
	return vtotal;
}
UINT32 vR2rGetHActive(void)
{
	UINT32 hactive;
	hactive = IO32ReadFldAlign(R2R_C_R2R_REG03,c_r2r_hdew);
	return hactive;
}
UINT32 vR2rGetVActive(void)
{
	UINT32 vactive;
	vactive = IO32ReadFldAlign(R2R_C_R2R_REG03,c_r2r_vdew);
	return vactive;
}

void vR2rSetHfp(UINT16 hfp)
{
	
}
void vR2rSetHbp(UINT16 hbp)
{
	
}
void vR2rSetVfp(UINT16 vfp)
{
	
}

EXTERN UINT8 bSI_DISPLAY_DCLK_TYPE;

void vR2rSetVbp(UINT16 vbp)
{
#ifdef CC_SUPPORT_UHD
    UINT32  u4PnlVsyncWidth;
	u4PnlVsyncWidth=PANEL_GetVSyncWidth();
	r2r_configures.u4VdeStart=vbp+u4PnlVsyncWidth;  //for 48Hz
	
	r2r_configures.u4VdeEnd=(r2r_configures.u4VdeStart+2160);
	printf("vR2rSetVbp vR2rSetVbp  vde=%d, vsync=%d",vbp,u4PnlVsyncWidth);
	vR2RSetVde(r2r_configures.u4VdeStart,r2r_configures.u4VdeEnd,r2r_configures.u4VdeStart,r2r_configures.u4VdeEnd);
#endif	
}


void vR2rRegisterDisplayCB(void)
{
	E_TIMEGEN_PATH r2rTimeGenPath = E_TIMEGEN_PATH_BYPASS_HDMI;
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_H_ACTIVE,vR2rSetHActive);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_H_TOTAL, vR2rSetHTotal);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_H_FP, vR2rSetHfp);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_H_BP, vR2rSetHbp);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_V_ACTIVE, vR2rSetVActive);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_V_TOTAL,vR2rSetVTotal);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_V_FP, vR2rSetVfp);
	fpDrvRegisterDisplayTimingUpdateCB(r2rTimeGenPath,E_DISPLAY_TIMING_CB_V_BP, vR2rSetVbp);
}
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
UINT8 u1ApRequestVdoTxture = 0;
UINT8 u1ApiW2dTxtureModeSet(BOOL enable)
{	
	u1ApRequestVdoTxture = enable;
	if(enable == 0)
	{
		vW2dSetVideoTextureControl(w2d_confi_videoTexture.hdmiPortId,0)	
	}
}
UINT8 u1ApiW2dTxtureModeGet(void)
{
	return u1ApRequestVdoTxture?1:0;
}
#endif
UINT8 u1TimingNeedCutByR2R(UINT8 u1DviTiming)
{	
	if(((Get_VGAMODE_IPH_WID(u1DviTiming) == 3840) && (wHDMIResoWidth() >= 4000)) \
		|| ((Get_VGAMODE_IPH_WID(u1DviTiming) == 1920) && (wHDMIResoWidth() >= 2000)))
	{
		return 1;
	}
	return 0;
}
UINT8 getOmuxOutputField(void)
{
	return IO32ReadFldAlign(MCVP_KC_0D,IN_FLD);
}
void vUhdInitR2rSlaveMode(UINT8 bOnOff)
{
	vIO32WriteFldAlign(R2R_C_R2R_REG15,bOnOff,c_r2r_ref2r2r_vsync_pul_en);	
}

