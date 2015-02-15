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
#include "drv_scaler_drvif.h"
#include "vdp_display.h"
#include "vdp_if.h"
#include "scpos_debug.h"
#include "drv_css.h"
#include "drv_uhd.h"
#include "x_bim.h"

#if defined(CC_SUPPORT_UHD)
#define  PQPATH_DLY    			350   //>=317 //got the value from test



UHD_INFO_T sUhdSysInfo;
UHD_INFO_T sRtUhdSysInfo;

UINT8 fgRstGfxOsd=0;
EXTERN 	UINT8 fgUhdSetdata;
EXTERN UINT8 fgUhdPowerOn;
UINT8 fgUhdSetScPipdata=OFF;

EXTERN void vR2RSetVde(UINT32 u4VdeStart,UINT32 u4VdeEnd,UINT32 u4VdeEvenStart,UINT32 u4VdeEvenEnd);
EXTERN void vScpipInitPscTargetline(UINT16 u2PscTgtLine);


void vUhdSwInit(void)
{
    //default set to PQ path
    sUhdSysInfo.u1SrcSelect=vPQ_PATH;
	sUhdSysInfo.u4InputWidth=1920;
	sUhdSysInfo.u4InputHeight=2160;

    //output width height should be fixed
	sUhdSysInfo.u4OutWidth=PANEL_GetPanelWidth();
	sUhdSysInfo.u4OutHeight=PANEL_GetPanelHeight();
	sRtUhdSysInfo.u4OutWidth= sUhdSysInfo.u4OutWidth;
	sRtUhdSysInfo.u4OutHeight=sUhdSysInfo.u4OutHeight;
}

void vUHD_VdoModeChgDone(void)
{
	//if(fgIs4k2kNativeTimingOut(SV_VP_MAIN))
	if(bGetPlaneEnable(VDP_BYPASS))
	{
		sRtUhdSysInfo.u1SrcSelect=vBYPASS_PATH;
		sRtUhdSysInfo.u4InputWidth=  wDrvVideoInputWidth(SV_VP_MAIN);;
		sRtUhdSysInfo.u4InputHeight = wDrvVideoInputHeight(SV_VP_MAIN); 
		//to do:
		  //input timing: 2160x1440
	}	 
	else
	{
		sRtUhdSysInfo.u1SrcSelect=vPQ_PATH;
		sRtUhdSysInfo.u4InputWidth= 1920; //u4GetPlaneWidth(VDP_PQPATH);   
		sRtUhdSysInfo.u4InputHeight = 2160; //u4GetPlaneHeight(VDP_PQPATH);  //no signal become 1080 ??? 
	}
	sRtUhdSysInfo.u1OutFrameRate=vDrvGetLCDFreq();
	fgRstGfxOsd=1;
		 
	LOG(2,"vUHD_VdoModeChgDone\n"); 
	LOG(2,"input path = %d\n",sRtUhdSysInfo.u1SrcSelect);
	LOG(2,"input Width/height = %d x %d\n",sRtUhdSysInfo.u4InputWidth,sRtUhdSysInfo.u4InputHeight);
}

UINT8 fgUhdIsModeChanged(void)
{
    UINT8 ret = SV_FALSE;

	if (x_memcmp((const void *)&sUhdSysInfo,(const void *)&sRtUhdSysInfo,sizeof(UHD_INFO_T)) )
	{
	   LOG(2,"Uhd mode chg!\n");
	   LOG(2,"UHD update path change from %s --> %s\n",sUhdSysInfo.u1SrcSelect == 0? "PQ_PATH":"BYPASS_PATH",sRtUhdSysInfo.u1SrcSelect == 0? "PQ_PATH":"BYPASS_PATH");
	   LOG(2,"UHD upadte Width/height change from %d x %d  --> %d x %d\n",sUhdSysInfo.u4InputWidth,sUhdSysInfo.u4InputHeight,sRtUhdSysInfo.u4InputWidth,sRtUhdSysInfo.u4InputHeight);
	   x_memcpy((void *)&sUhdSysInfo,(void *)&sRtUhdSysInfo, sizeof(UHD_INFO_T));
	   
	   ret = SV_TRUE;
	   //to do: Mute UHD~
	}      
    return ret;
}

void vHdmiR2rToUhd(void)
{
	vUhdRefSyncDly(650);
	vUhdSetVmskStartOffset(0x1ffff); //voffset = -1;
	switch( sUhdSysInfo.u1OutFrameRate )
	{
		case 48:
			vUhdSetOsdDly(0xf8,0x0d);
			break;
		case 50:
			vUhdSetOsdDly(0x1d4,0x0d);
			break;
	    default:
		case 60:	
		  vUhdSetOsdDly(0x544,0x0d);
		break;
	}	
}

void vMpegB2rToUhd(void)
{
	vUhdRefSyncDly(sUhdSysInfo.u4PanelHtotal>>1);
	vUhd_InitB2rSlaveMode(0x1);		
	//vB2RSetVporch(2245,2248);
	switch( sUhdSysInfo.u1OutFrameRate )
	{		
		case 48:
			vUhdSetOsdDly(0xf8,0x0d);					
		break;
		case 50:
			vUhdSetOsdDly(0x1d4,0x0d);
		break;	
		default:			
		case 60:
			vUhdSetOsdDly(0x544,0x0d);
		break;
	}
}



//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
EXTERN UINT32 u4DrvTDTVIs3DPhoto(TDTV_3D_CROP_INTO_T* prCropInfo);


void vUpdateUhdP2S(void)
{
    UINT8 fgSrcByPath;	
   // TDTV_3D_CROP_INTO_T ePhotoCrop;
	
	if ( sUhdSysInfo.u1SrcSelect == vPQ_PATH )
	{
        vUhdRefSyncDly(350); 
		vUhdP2SEnable(0);
		vUhdSetVmskStartOffset(0x1ffff); //voffset =0;
		switch( sUhdSysInfo.u1OutFrameRate )
		{
			case 48:
				vUhdSetOsdDly(0xf8,0x0d);			   
			break;	

			case 50: //if input frame_rate=60
				vUhdSetOsdDly(0x1d4,0x0d);
			break;
		    default:
		    case 60:		//if input frame_rate=50
			  vUhdSetOsdDly(0x544,0x0d);
			break;  
		}
	}
	else   //bypass path
	{
		vUhdP2SEnable(1);			
		fgSrcByPath=bGetVideoDecType(VDP_1);		
		if ( fgSrcByPath == SV_VD_DVI )
		{
		    vHdmiR2rToUhd();
		}
		else  //signal come from b2r
		{			
		    vMpegB2rToUhd();
		}
	}
}

void vUpdateHorizontalSc(void)
{
    vUhdSetScaling(sUhdSysInfo.u4InputWidth,sUhdSysInfo.u4InputHeight, sUhdSysInfo.u4OutWidth,sUhdSysInfo.u4OutHeight);
	LOG(2,"UHD horizontal_sc input %d x %d output %d x %d\n",sUhdSysInfo.u4InputWidth,sUhdSysInfo.u4InputHeight, sUhdSysInfo.u4OutWidth,sUhdSysInfo.u4OutHeight);
}

void vUpdatePanelTimeGen(void)
{

   vUhdSetOutputPanelTiming(sUhdSysInfo.u4PanelHtotal,sUhdSysInfo.u4PanelVtotal, \
							sUhdSysInfo.u4PanelWidth,sUhdSysInfo.u4PanelHeight,  \
						    sUhdSysInfo.u4PanelH_BP, sUhdSysInfo.u4PanelH_FP,    \
						    sUhdSysInfo.u4PanelV_BP,sUhdSysInfo.u4PanelV_FP);




   LOG(0,"UHD vUpdatePanelTimeGen ~~~ \n");

   if ( fgUhdPowerOn != ON)
   {
	   fgUhdPowerOn=ON; 	
	   vScpipRefSetData(ON);	//set ref tg OFF

	   //vScpipSetData(1);	
	   vScpipGfxSetData(ON);
	   vUhdSetData(ON); 	 //set UHD TG enable
	   HAL_Delay_us(30000);  //wait to vsync let IMPORT effect. 	   
	   vResetUhdTimingGen(); //reset all TG
   }
   else
	   fgUhdSetdata=ON;


   //osd merge mode
   vUhdSetOsdMergeHtotal(sUhdSysInfo.u4PanelHtotal);
						    
}

void vDrvApplyUHDTiming(LCDTIMING_INFO_T *tLcdTiming)
{
	sRtUhdSysInfo.u4PanelHtotal=tLcdTiming->u4HTotal;
	sRtUhdSysInfo.u4PanelVtotal=tLcdTiming->u4VTotal;
	sRtUhdSysInfo.u4PanelWidth=tLcdTiming->u4HActive;
	sRtUhdSysInfo.u4PanelHeight=tLcdTiming->u4VActive;	
	sRtUhdSysInfo.u4PanelH_BP=tLcdTiming->u4HBP;	
	sRtUhdSysInfo.u4PanelH_FP=tLcdTiming->u4HFP;
	sRtUhdSysInfo.u4PanelV_BP=tLcdTiming->u4VBP;
	sRtUhdSysInfo.u4PanelV_FP=tLcdTiming->u4VFP;

    LOG(0,"UHD timggen= %d ,%d, %d, %d,,%d ,%d , %d, %d \n",tLcdTiming->u4HTotal, tLcdTiming->u4VTotal,tLcdTiming->u4HActive,tLcdTiming->u4VActive,tLcdTiming->u4HBP,\
    					tLcdTiming->u4HFP,tLcdTiming->u4VBP,tLcdTiming->u4VFP);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------


void vUhdClearISRstatus(UINT32 u4Sel)
{
    switch(u4Sel)
    {
		case ISR_UHD_OUT_VSYNC_FALLING:  // 1
			UHD_WRITE_FLD(UHD_INT0, 1, INT_UHD_OUT_VSYNC_F_CLR);	
			UHD_WRITE_FLD(UHD_INT0, 0, INT_UHD_OUT_VSYNC_F_CLR);	
		break;	
		case ISR_SC_OUT_VSYNC_FALLING:  // 8
			UHD_WRITE_FLD(UHD_INT0, 1, INT_SC_OUT_VSYNC_F_CLR);	
			UHD_WRITE_FLD(UHD_INT0, 0, INT_SC_OUT_VSYNC_F_CLR);	
		break;	
		default:
		break;
    }
}
//--------------------------------------------------------------------

UINT32 u4UhdGetISRstatus(UINT32 u4Sel)
{
    UINT32 u4status;
    switch(u4Sel)
    {
		case ISR_UHD_OUT_VSYNC_FALLING:  // 1
			u4status = (UINT32)SCPIP_READ_FLD(UHD_INT1, INT_STA_UHD_OUT_VSYNC_F);
		break;	
		case ISR_SC_OUT_VSYNC_FALLING:  //8
			u4status = (UINT32)SCPIP_READ_FLD(UHD_INT1, INT_STA_SC_OUT_VSYNC_F);
		break;	

    }
    return u4status;
}

//--------------------------------------------------------------------
UINT8 U1RestCnt;
static void _vDrvUhdISR(UINT16 u2Vector)
{
    //TDTV_3D_CROP_INTO_T ePhotoCrop;

    ASSERT(u2Vector == VECTOR_UHD);
	if(u4UhdGetISRstatus(ISR_UHD_OUT_VSYNC_FALLING)) // 1
	{
			LOG(7, "ISR_UHD_OUT_VSYNC_FALLING  %d\n", u4UhdGetOutVCnt());
			vUhdClearISRstatus(ISR_UHD_OUT_VSYNC_FALLING);	
	}

    if(u4UhdGetISRstatus(ISR_SC_OUT_VSYNC_FALLING)) // 8
    {
    		LOG(7, "ISR_SC_OUT_VSYNC_FALLING  %d\n", u4UhdGetOutVCnt());
    		vUhdClearISRstatus(ISR_SC_OUT_VSYNC_FALLING);	
    		if ( fgRstGfxOsd )
    		{
    			vUhdOsdMergeSoftRst(1);
    			fgRstGfxOsd=0;
				LOG( 7, "ISR--OSD mege reset\n");
    			vUhdOsdMergeSoftRst(0);
    		}
			if( fgUhdPowerOn == ON )
			{		
		      if (fgUhdSetdata == ON ) 
		      {
    			  fgUhdSetdata=OFF;
    			  LOG( 2, "ISR--TG setdata\n");
    			  vScpipGfxSetData(SV_ON);
    			  vScpipRefSetData(ON);
    			  vUhdSetData(1); 

				  if ( fgUhdSetScPipdata == ON )
				  {
					LOG( 2, "ISR--TG set fsc data\n");
					fgUhdSetScPipdata=OFF;
					vScpipSetData(1);
					//if(u4DrvTDTVIs3DPhoto(&ePhotoCrop))  //photo play back
					//	U1RestCnt=10;
					//else	
					   U1RestCnt=10;
					vScpipSetTimeGenEnable( E_FSC_TG, SV_OFF );
				  }   

				  if(IS_VB1_2SEC_ENABLE)
				  {
				  	vIO32WriteFldAlign(UHD_RESET, 1, S2P_SOFT_RST);
				  	vIO32WriteFldAlign(UHD_RESET, 0, S2P_SOFT_RST);
				  }			  
		      }

			  if ( U1RestCnt > 0 )
			  { 			 
				 U1RestCnt--;
				 if ( U1RestCnt == 0)
					 vScpipSetTimeGenEnable( E_FSC_TG, SV_ON );
			  }
			}
				
    }

}
//--------------------------------------------------------------------
void vUhdSetISREn(UINT32 u4Sel, UINT32 u4En)
{
	switch(u4Sel)
	{
	  case ISR_UHD_OUT_VSYNC_FALLING:   // 1
			  SCPIP_WRITE_FLD(UHD_INT0, u4En, INT_UHD_OUT_VSYNC_F_EN);	  
	  break;  
      case ISR_SC_OUT_VSYNC_FALLING:    // 8
		  SCPIP_WRITE_FLD(UHD_INT0, u4En, INT_SC_OUT_VSYNC_F_EN);	  
	  break;	  
	  default:
	  break;	
	}
}
//--------------------------------------------------------------------

//UHD isr register
static void _vDrvUhdInitISR(void)
{
    static UINT8 fgUhdIsrInitialized = SV_FALSE;
    x_os_isr_fct pfnUhdIsr;

    if (!fgUhdIsrInitialized)
    {
        if (x_reg_isr(VECTOR_UHD, _vDrvUhdISR, &pfnUhdIsr) != OSR_OK) // todo
        {
            LOG(0, "Error: fail to register SCPIP ISR!\n");
        }
        else
        {
            LOG(0, "Register SCPIP ISR OK!\n");
        }

        fgUhdIsrInitialized = SV_TRUE;
        LOG(2, "UHD ISR registered!\n");
    }
}



void vUhdSetISR(void)
{
	_vDrvUhdInitISR();
	vUhdSetISREn(ISR_SC_OUT_VSYNC_FALLING,SV_ON);   //turn UHD vsync int ON


	
}
//---------------------------------------------------------------------------------------------




void vUhdNormProc(void)
{
    if(fgUhdIsModeChanged()==SV_TRUE)
    {    
        vUpdateUhdP2S();
		vUpdateHorizontalSc();
		vUpdatePanelTimeGen();
    }
}

void vUhdQueryStatus(void)
{
	LOG(0,"==== UhdSysInfo =====\n ");
	LOG(0,"src %s Input WxH = %d x %d  Output WxH = %d x %d\n",sUhdSysInfo.u1SrcSelect == 0? "PQ_PATH":"BYPASS_PATH",sUhdSysInfo.u4InputWidth,sUhdSysInfo.u4InputHeight,\
		                                                       sUhdSysInfo.u4OutWidth,sUhdSysInfo.u4OutHeight);
	LOG(0,"Panel WxH = %d x %d   Panel HtxVt = %d x %d \n",sUhdSysInfo.u4PanelWidth,sUhdSysInfo.u4PanelHeight,sUhdSysInfo.u4PanelHtotal,sUhdSysInfo.u4PanelVtotal);
	LOG(0,"Panel HBxVB = %d x %d Panel HFxVF = %d x %d \n",sUhdSysInfo.u4PanelH_BP,sUhdSysInfo.u4PanelV_BP,sUhdSysInfo.u4PanelH_FP,sUhdSysInfo.u4PanelV_FP);
	LOG(0,"panel out frate=%d\n",sUhdSysInfo.u1OutFrameRate);

	LOG(0,"==== sRtUhdSysInfo =====\n ");		                                                       
	LOG(0,"src %s Input wxh = %d x %d  Output WxH = %d x %d\n",sRtUhdSysInfo.u1SrcSelect == 0? "PQ_PATH":"BYPASS_PATH",sRtUhdSysInfo.u4InputWidth,sRtUhdSysInfo.u4InputHeight,\
		                                                       sRtUhdSysInfo.u4OutWidth,sRtUhdSysInfo.u4OutHeight);
	LOG(0,"Panel WxH = %d x %d   Panel HtxVt = %d x %d \n",sRtUhdSysInfo.u4PanelWidth,sRtUhdSysInfo.u4PanelHeight,sRtUhdSysInfo.u4PanelHtotal,sRtUhdSysInfo.u4PanelVtotal);
	LOG(0,"Panel HBxVB = %d x %d Panel HFxVF = %d x %d \n",sRtUhdSysInfo.u4PanelH_BP,sRtUhdSysInfo.u4PanelV_BP,sRtUhdSysInfo.u4PanelH_FP,sRtUhdSysInfo.u4PanelV_FP);	

}
#endif  //#if defined(CC_SUPPORT_UHD)

