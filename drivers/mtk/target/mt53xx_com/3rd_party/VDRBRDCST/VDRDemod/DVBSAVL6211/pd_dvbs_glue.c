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
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_DVBS_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_DVBS_glue.c
 *  DVBS glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "u_tuner.h"
#include "x_os.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_dvbs_glue.h"
#include "tunerDebug.h"
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "pd_dvbs_if.h"
#include "x_gpio.h"
#include "vdr_PD_Def.h"
#include "x_rm.h"
#include "drvcust_if.h"
#include "lnb_control.h"

#include "AVL_DVBS2_Porting.h"
#include "x_gpio.h"
#include "avl.h"
#include "avl_dvbsx.h"

#define Scan_Center_Freq_Step_100KHz		100
#define DVBS_Vendor_Driver_Ver "2013-04-07 SDK 1.0.27 patch 1.0.29	"

static UINT8 g_u1RetryCnt=5;//default retry 5 times
static UINT16 bs_stop_freq=2150;		//The stop rf frequency, 2100M
static UINT16  bs_start_freq=950;			//The start rf frequency, 965M
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Import extern variable
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
					   
//-----------------------------------------------------------------------------
// Export definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define mcSEMA_LOCK_MTFE(sema)                                       \
{                                                                    \
    mcDBG_MSG_L4((" [ _Lock_%s-%d ]\r\n", __FUNCTION__, __LINE__)); \
    x_sema_lock(sema, X_SEMA_OPTION_WAIT);                           \
    mcDBG_MSG_L4((" [ _Excape_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
}
#define mcSEMA_UNLOCK_MTFE(sema)                                     \
{                                                                    \
    mcDBG_MSG_L4((" [ _Unlock_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
    x_sema_unlock(sema);                                             \
}


#define LNB_POWER_13 0x1
#define LNB_POWER_18 0x2
#define LNB_POWER_0 0x0
#define LNB_POWER_USER_SET_OFF 0x1f
#define LNB_POWER_USER_SET_NORMAL 0x2f
#define LNB_POWER_USER_SET_PLUS 0x3f

#define Debug_time
#define C_DVBS_SEND_ODU_POWEROFF_RETRY_CNT  g_u1RetryCnt
//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
static DVBS_PD_CTX_T *pPdCtxDVBS;
static DVBS_PD_CTX_T tPdCtxDVBS;

static TUNER_DEVICE_CTX_T*    pDVBSTunerDeviceCtx = NULL;
static UINT8 do_blindscan=0;
static UINT8 Monitor_waiting_semaphore_flag=BOOL_FALSE;
static UINT32 ui4_data_forMW=0;
static TUNER_POLARIZATION_T E_POL=POL_UNKNOWN;
//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
static HANDLE_T  AutoScan_callback;
static DVBS2_CHANNEL_T Current_channel;/*during blindscan process,record the currnet try_lock channel freq_info,or record the start_freq*/
static UINT8 g_channle_locked_flag=1;   //default is true;
static DISEQC_BUS_CMD_T g_diseqc_cmd;
static DVBS2_CHANNEL_T channels_Temp[64];
static UINT8 user_set_flag=LNB_POWER_USER_SET_PLUS;
static UINT8 g_PreUserBandIdx = 0xff;
static UINT16 g_UnicableDelay = 0;//in seconds
static UCHAR g_u1EnEarlyBreak=1;//enable demod early break when connecting
static UINT8 g_repeat=2;//add Diseqc repeat,2 is repeat,1 is not repeate 

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static  INT32 DVBS_AutoScanCallback(UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec);
EXTERN INT32 Tuner_Notify(DRV_COND_T e_nfy_cond,UINT32 u4Msg);
static void vEnEarlyBreak(UCHAR u1En)
{
   g_u1EnEarlyBreak=u1En;
   mcDBG_MSG_L2(("Early break flag:%d\n",g_u1EnEarlyBreak));
}

static TUNER_CONNECTION_MODE_T Util_GetConnMode(DVBS_PD_CTX_T * pdCtx,TUNER_CONNECTION_MODE_T e_mode)
{
  TUNER_CONNECTION_MODE_T e_ret=e_mode;
   switch(e_mode)
   {
      case CONN_MODE_SCAN:
	  {
	  	 if(pdCtx->fgUniCab)
	     {
	        e_ret=CONN_MODE_UNICABLE_SCAN;
	     }  
	  }
	  break;
	  case CONN_MODE_SURF:
	  case CONN_MODE_UNKNOWN:
	  {
	   	 if(pdCtx->fgUniCab)
	     {
	        e_ret=CONN_MODE_UNICABLE_SURF;
	     }  
	  }
      break;
	  default:
       mcDBG_MSG_L1(("[%s]Unknown conn mode %d\n",__FUNCTION__,e_mode));
       break;	   
   }
   return e_ret;
}

static BOOL DVBS_ValidateMwPara(TUNER_SAT_DIG_TUNE_INFO_T * pt_tuner_info,TUNER_CONNECTION_MODE_T e_conn_mode)
{
	UINT32 ui4_start_freq,ui4_end_freq,ui4_freq;
	BOOL fgRet=TRUE;

	switch(e_conn_mode)
	{
		case CONN_MODE_SCAN:
			ui4_start_freq=pt_tuner_info->ui4_start_freq;
			ui4_end_freq=pt_tuner_info->ui4_end_freq;
			if((ui4_start_freq>=ui4_end_freq) || (ui4_start_freq<940) || (ui4_end_freq>2160))
			{
				mcDBG_MSG_ERR(("frequency start %d MHz end %dMHz,out of range [940,2160]MHz.\n",ui4_start_freq,ui4_end_freq));
				fgRet=FALSE;
			}
			break;
			
		case CONN_MODE_UNICABLE_SCAN:
			{
				TUNER_UNICABLE_CC_T *pUniCabinfo=&(pPdCtxDVBS->t_UniCab_info); 
				ui4_start_freq=pt_tuner_info->ui4_start_freq;
				ui4_end_freq=	pt_tuner_info->ui4_end_freq;
				if((ui4_start_freq>=ui4_end_freq) || ui4_start_freq<940 || ui4_end_freq>2160 || pUniCabinfo->ui2_band_freq<950 || pUniCabinfo->ui2_band_freq>2150) 
				{
					mcDBG_MSG_ERR(("frequency start %d MHz end %dMHz band freq %dMHz,out of range [940,2160]MHz.\n",ui4_start_freq,ui4_end_freq,pUniCabinfo->ui2_band_freq));
					fgRet=FALSE;
				}
			}
			break;
		
		case CONN_MODE_UNICABLE_SURF:
		case CONN_MODE_UNKNOWN://channel change
		case CONN_MODE_SURF://preset scan ,single tp scan 
			ui4_freq=pt_tuner_info->ui4_freq;
			if(ui4_freq<940 || ui4_freq>2160)
			{
				mcDBG_MSG_ERR(("frequency %d MHz,out of range [940,2160]MHz.\n",ui4_freq));
				fgRet=FALSE;
			}
			break;
			
		default:
			mcDBG_MSG_ERR(("unknown connection mode %d.\n",e_conn_mode));
			fgRet=FALSE;
			break;
	}
	
	return fgRet;
}

static void Unicable_ChgChnDelay(UINT8 UserBandIdx)
{
	UINT16 bNewMSB;;

	if(UserBandIdx != g_PreUserBandIdx)
	{
		g_PreUserBandIdx = UserBandIdx;
		g_UnicableDelay = 0;
		UserBandIdx++;
		g_UnicableDelay |= UserBandIdx;
		g_UnicableDelay |= (UserBandIdx << 5);
	}
	else
	{
		//new p10 = p8 xor p1
		bNewMSB = ((g_UnicableDelay << 2) ^ (g_UnicableDelay << 9)) & 0x0200;
		g_UnicableDelay = (g_UnicableDelay >> 1);
		g_UnicableDelay |= bNewMSB;
	}
}
static UINT16 DVBS_GetUnicableDelay(void)/// return ms
{
    UINT16 u2Delay=0;
	mcDBG_MSG_L4(("current unicable monitorprocess delay is %d ms\n",((g_UnicableDelay*1000)/1024) ));
	if(g_UnicableDelay)
	{
   		u2Delay=((UINT32)(g_UnicableDelay*1000))/1024;// P*1000 sec to ms 
	}
	else
	{
		u2Delay=500;//if delay is 0,set as 500ms for loop forever;
	}
	if(u2Delay<500)u2Delay=500;//d121202_Haibo:Limit the delay to 500ms to release CPU resource to other thread
	return u2Delay;
}
static void DVBS_OduChannelChg(TUNER_UNICABLE_CC_T *pUniCabinfo)
{
	UINT8 command[5];	
	
	mcDBG_MSG_L2(("[%s]\n",__FUNCTION__));
	mcDBG_MSG_L2(("Ft=%dMHz,Fo=%dMHz,Fu=%dMHz,UB=%d,Bank=%d\n",\
	pUniCabinfo->ui4_transponder_freq,pUniCabinfo->ui2_lnb_freq,pUniCabinfo->ui2_band_freq,pUniCabinfo->e_user_band,pUniCabinfo->e_bank));
	//S=TUNER_CODING_STEP_SIZE in u_tuner.h
    if(pUniCabinfo->ui4_transponder_freq==0)
   {
        mcDBG_MSG_L2(("Ft is 0MHz,do not do channel change\n"));
		return;
   }
	
	//TODO:Please add your implemention to control SCIF
	AVL_DVBS2_Unicable_ODU_Channel_Change(command, pUniCabinfo);
    
	Unicable_ChgChnDelay(pUniCabinfo->e_user_band);
	
	AVL_DVBS2_LNB_Control_14or18V(0);
	mcDELAY_MS(5);//
	AVL_DVBS2_SendDiseqcCmmd(command, 5);
	mcDELAY_MS(10);//
	AVL_DVBS2_LNB_Control_14or18V(1);
	
	
}
static void DVBS_OduChannelChg_Blindscan(UINT32 nScanCenter)
{
	UINT8 command[5];	
	TUNER_UNICABLE_CC_T UniCabinfo;
	//mcDBG_MSG_L2(("[%s] center  freq:%d Mhz\n",__FUNCTION__,nScanCenter));
	
	UniCabinfo.ui4_transponder_freq=pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq+nScanCenter;/*暂时特殊处理*/
	UniCabinfo.ui2_lnb_freq=pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq;
	UniCabinfo.ui2_band_freq=pPdCtxDVBS->t_UniCab_info.ui2_band_freq;
	UniCabinfo.e_user_band=pPdCtxDVBS->t_UniCab_info.e_user_band;
	UniCabinfo.e_bank=pPdCtxDVBS->t_UniCab_info.e_bank;
	
	AVL_DVBS2_Unicable_ODU_Channel_Change(command, &UniCabinfo);

	AVL_DVBS2_LNB_Control_14or18V(0);
	mcDELAY_MS(5);//
	AVL_DVBS2_SendDiseqcCmmd(command, 5);
	mcDELAY_MS(10);//
	AVL_DVBS2_LNB_Control_14or18V(1);
	
	
}
static void DVBS_OduPowerOff(DVBS_PD_CTX_T * pdCtx)
{

	UINT8 command[5];	
    UINT32 u4TickStart=x_os_get_sys_tick();	
	mcDBG_MSG_L2(("[%s]UB=%d\n",__FUNCTION__,pdCtx->t_UniCab_info.e_user_band));
	//TODO:Please add your implemention to control SCIF
	AVL_DVBS2_Unicable_ODU_PowerOff(command, pdCtx->t_UniCab_info.e_user_band);
	AVL_DVBS2_LNB_Control_14or18V(0);
	mcDELAY_MS(5);//
	AVL_DVBS2_SendDiseqcCmmd(command, 5);
	mcDELAY_MS(10);//
	AVL_DVBS2_LNB_Control_14or18V(1); 
	mcDBG_MSG_L2(("[%s] time consume is %d ms\n",__FUNCTION__,(x_os_get_sys_tick()-u4TickStart)*x_os_get_sys_tick_period()));
}
static void DVBS_TunerDiscSCIF(DVBS_PD_CTX_T * pdCtx)
{
    UINT8 u1ReTryCnt=0;
	UINT32 u4Tick;
	UINT8 u1ChkTPLCnt=0;
	UINT32 u4TickStart=x_os_get_sys_tick();
	BOOL fgPreLockSts=AVL_DVBS2_CheckTP_IsLocked();//d120822_Haibo:Check current demod lock status
	
	while(u1ReTryCnt<C_DVBS_SEND_ODU_POWEROFF_RETRY_CNT)
	{
	  u4Tick=x_os_get_sys_tick();
	  u1ChkTPLCnt=0;
	  mcDBG_MSG_L2(("Tick Cnt=%d Delay=%dms\n",u4Tick,(u4Tick%10)+1));
	  
	  x_thread_delay((u4Tick%10)+1);
	  
	  DVBS_OduPowerOff(pdCtx);
	  //d120721_Haibo:if the previous channel is locked,then check if  it is off indeed by lock status.if not,need retry
	  if(fgPreLockSts)//previous lock
	  {
	     while(u1ChkTPLCnt<7)//total check time is 350ms.we measure the time from DVBS_OduPowerOff to it is off indeed,it is about 290ms.So we delay 350ms maximum to speed the check time in normal case
		 {
		   x_thread_delay(50);
		   u1ChkTPLCnt++;
		   if(!AVL_DVBS2_CheckTP_IsLocked())//now is unlock
		  { 
		   break;
		  }
		 }
		if(u1ChkTPLCnt!=7)break;
	  }
	  else
	  {
	       break;
	  }
	  u1ReTryCnt++;
	}
	if(u1ReTryCnt==C_DVBS_SEND_ODU_POWEROFF_RETRY_CNT)
	{
	   mcDBG_MSG_L1(("fail to %s!!!\n",__FUNCTION__));
	}
	mcDBG_MSG_L2(("Finish @%s after try %d times \n",__FUNCTION__,u1ReTryCnt+1));
	mcDBG_MSG_L2(("Time consume is %dms\n",(x_os_get_sys_tick()-u4TickStart)*x_os_get_sys_tick_period()));
}
static void Demod_GPIO_Reset(void)
{	
    UINT32 u4ResetGpionum;

	if(DRVCUST_OptQuery(eDvbsDemodResetGpio,&u4ResetGpionum)==0)
	{
	   if(u4ResetGpionum!=0xffffffff)
	     {
          GPIO_SetOut(u4ResetGpionum, 0);
	      mcDELAY_MS(50);
	      //fgSet = 1;
	      GPIO_SetOut(u4ResetGpionum,1);
	      mcDELAY_MS(250);
	      mcDBG_MSG_L2(("Reset demod  via %d\n",u4ResetGpionum));
	      }  
        else
	    mcDBG_MSG_L2(("Do not Reset DVBS demod AVL6211\n"));
	}
	else
		mcDBG_MSG_L2(("Do not Reset DVBS demod AVL6211\n"));
}

static void LNB_POWER_USER_SET(UINT8  Temp)
{
  //  AVL_uchar Temp2;
    user_set_flag=Temp;
}
static void LNB_Control(UINT8 Temp)
{
	 switch(Temp)
	 	{
	 	case LNB_POWER_0:
			AVL_DVBS2_LNB_Power_OnOff(0);
			break;
		case LNB_POWER_13:
			if(user_set_flag == LNB_POWER_USER_SET_NORMAL )
				AVL_DVBS2_LNB_Control_14or18V(1); //14V
			else if(user_set_flag == LNB_POWER_USER_SET_PLUS)
				AVL_DVBS2_LNB_Control_15or19V(1); //15V
			else
			{
				mcDBG_MSG_L2(("[LNB_Control] user flag is OFF!\n"));
			}
			break;	
		case LNB_POWER_18:
			if(user_set_flag == LNB_POWER_USER_SET_NORMAL )
				AVL_DVBS2_LNB_Control_14or18V(0);///18
			else if(user_set_flag == LNB_POWER_USER_SET_PLUS)
				AVL_DVBS2_LNB_Control_15or19V(0);//19
			else
			{
				mcDBG_MSG_L2(("[LNB_Control] user flag is OFF!\n"));
			}
			break;
		default:
			mcDBG_MSG_L2(("[LNB_Control]  not support!\n"));
			return;
	 	}

}
static BOOL BlindScan_MidWare_Break(void)
{
	return pPdCtxDVBS->fgBreakMonitor;
}

static void AVL_ResendDiseqcCmmd(DISEQC_BUS_CMD_T *diseqc_cmd)
{
    UINT32 i=0;
	UINT32 j=0;
	    if(((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)||(diseqc_cmd->ui1_tone_burst==1)||(diseqc_cmd->ui1_tone_burst==0))
	    {
	    	mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd] stop 22k \n"));
	        AVL_DVBS2_22K_OFF();
	    }
	//mcDELAY_MS(20);
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_POLARIZATION_MASK) != 0 )
		{//apply you code
			if(diseqc_cmd->ui1_polarity_13v_18v == 1)
			{
				mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]set 18V\n"));
				LNB_Control(LNB_POWER_18);
			}
			else
			{
				mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd] set 13V\n"));
				LNB_Control(LNB_POWER_13);
			}
			mcDELAY_MS(20);
   		}

					
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)
			{//apply you code
				for(i=0;i<g_repeat;i++)
				{
					if(i==1)
					{
						diseqc_cmd->aui1_cmd[0]|=0x01;
					}
				    else
			        {
                        diseqc_cmd->aui1_cmd[0]&=0xFE;
			        }
				    AVL_DVBS2_SendDiseqcCmmd(diseqc_cmd->aui1_cmd, diseqc_cmd->ui1_cmd_len);
				    for(j=0;j<diseqc_cmd->ui1_cmd_len;j++)
				    {
						mcDBG_MSG_L2(("0x%02x  ",diseqc_cmd->aui1_cmd[j]));
					}
				    mcDELAY_MS(110);
				 }
				mcDBG_MSG_L2(("After set cmd,delay 110ms,delay>15ms\n"));
			}

			
		if((diseqc_cmd->ui4_control_mask &DISEQC_CMD_CTRL_TONE_BURST_MASK) != 0)
		{//apply you code
			if(1==diseqc_cmd->ui1_tone_burst)
	    		{
		    		mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]b_tone_burst 1 \n"));
				AVL_DVBS2_Select_Tone(1);
	    		}
			else if(0==diseqc_cmd->ui1_tone_burst)
			{
				mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]b_tone_burst 0 \n"));
				AVL_DVBS2_Select_Tone(0);
			}
            else
            {
                mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]b_tone_burst off \n"));
            }
			mcDELAY_MS(20);
	   	 }
			
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) != 0)
	 	{//apply you code
			if(diseqc_cmd->b_22k)
		    	{
		    		mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]set 22k on.\n"));
				AVL_DVBS2_22K_ON();
		    	}
			else
			{
				mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd]set 22k off.\n"));
				AVL_DVBS2_22K_OFF();
			}	
			//mcDELAY_MS(20);
	    	}	
}

static VOID AVL_BlindScan(void)
{
	UCHAR m_New_Temp=0;
	UCHAR i=0,j=0;
	UINT32 ret=0;
	UINT16 WaitLoop=0;
       BOOL Lockflag=0;
       UINT16 Scan_Step;  
 	UINT32 freq_temp=0;
	UINT16  nFreqCenter_100KHZ=0;

	mcDBG_MSG_L2(("[AVL_BlindScan] start...............!\n"));	
	Scan_Step = (10*(bs_stop_freq-bs_start_freq))/Scan_Center_Freq_Step_100KHz;
	
	for(i=0;i<Scan_Step;i++)
	{	
		if(BlindScan_MidWare_Break())
		{
			mcDBG_MSG_L2(("[AVL_BlindScan]Break 2!\n"));
			 break;
		}		
		nFreqCenter_100KHZ=10*(bs_start_freq)+(i+1)*Scan_Center_Freq_Step_100KHz;
		if(nFreqCenter_100KHZ > (bs_stop_freq*10-300) )
		{
			  break;///end
		}
		m_New_Temp=0;
		x_memset(channels_Temp,0,sizeof(channels_Temp));
		
		if(pPdCtxDVBS->fgUniCab == TRUE)
		{
			DVBS_OduChannelChg_Blindscan(nFreqCenter_100KHZ/10);
			ret= AVL_DVBS2_Unicable_BlindScanAction((pPdCtxDVBS->t_UniCab_info.ui2_band_freq)*10,nFreqCenter_100KHZ,channels_Temp,&m_New_Temp);
		}
		else
		{
			ret= AVL_DVBS2_BlindScanAction(nFreqCenter_100KHZ,channels_Temp,&m_New_Temp);
		}
		   
		if(ret!=DRVAPI_TUNER_OK )
		{
			mcDBG_MSG_L2(("[AVL_BlindScan]  Break ,Err!\n"));
			break;
		}
		if(BlindScan_MidWare_Break())
		{
			mcDBG_MSG_L2(("[AVL_BlindScan]Break 3!\n"));
			 break;
		}			
		for(j=0;j<m_New_Temp;j++)
		{
			Lockflag=0;
			if(pPdCtxDVBS->fgUniCab == TRUE)
			{
				mcDBG_MSG_L2(("[AVL_BlindScan] Unicable lock to :%dMhz!\n",channels_Temp[j].m_uiFrequency_kHz/1000));
				DVBS_OduChannelChg_Blindscan(channels_Temp[j].m_uiFrequency_kHz/1000);
				ret=AVL_DVBS2_LcokChannel((pPdCtxDVBS->t_UniCab_info.ui2_band_freq)*10, channels_Temp[j].m_uiSymbolRate_Hz,pPdCtxDVBS->fgUniCab);	
			}
			else
			{
				ret=AVL_DVBS2_LcokChannel(channels_Temp[j].m_uiFrequency_kHz/100, channels_Temp[j].m_uiSymbolRate_Hz,pPdCtxDVBS->fgUniCab);
			}
			if(ret==DRVAPI_TUNER_OK)
			{
				WaitLoop=AVL_DVBS2_Get_MaxLoop();
				while(WaitLoop--)
				{
					x_thread_delay(10);
					Lockflag=AVL_DVBS2_CheckTP_IsLocked();
					if(BlindScan_MidWare_Break())
					{
						mcDBG_MSG_L2(("[AVL_BlindScan]Break 4!\n"));
						break;
					}	
				}
			}	
			if(BlindScan_MidWare_Break())
			{
				mcDBG_MSG_L2(("[AVL_BlindScan]Break 5!\n"));
				break;
			}		
			if(Lockflag)		
			{	
				Current_channel.m_uiFrequency_kHz=channels_Temp[j].m_uiFrequency_kHz;
				Current_channel.m_uiSymbolRate_Hz=channels_Temp[j].m_uiSymbolRate_Hz;	
				Current_channel.m_fec=FEC_I_UNKNOWN;  			
				DVBS_AutoScanCallback(1, 0, 0,0,0);	
			}						
		}				
		freq_temp=bs_start_freq*1000+(i*Scan_Center_Freq_Step_100KHz)*100;
		if(freq_temp>Current_channel.m_uiFrequency_kHz)/*notiffy freq must be bigger than before */
		{
			Current_channel.m_uiFrequency_kHz=freq_temp;//bs_start_freq*1000+(i*Scan_Center_Freq_Step_100KHz)*100;
			Current_channel.m_uiSymbolRate_Hz=0;			
			Current_channel.m_fec=FEC_I_UNKNOWN;										
			DVBS_AutoScanCallback(0, 0, 0,0,0);
		}
	}

	mcDBG_MSG_L2(("[AVL_BlindScan] end.!\n"));		
	return ;
}
static UCHAR AVL_JudgeSameChannel(UINT32 a_Khz,UINT32 b_Khz,UINT32 b_symKs)
{
   UINT32 offset=0;
    if(a_Khz>=b_Khz)
		offset=a_Khz-b_Khz;
	else
		offset=b_Khz-a_Khz;
	if(((offset<=(1000+500)) && (b_symKs<=10*1000)) || // d121129_Haibo:When detected symbol rate is not gt 10M KS/s,tolerant offset is 1.5MHz
      ((offset<=(3000+500)) && (b_symKs>10*1000)))// d121129_Haibo:When detected symbol rate is gt 10M KS/s,tolerant offset is 3.5MHz	  
	  {
	    return 1;
	  }
	else
	  {
    	return 0;
	  }
}
static VOID AVL_SymbolRate_AutoDetec(DVBS2_CHANNEL_T *stchannel)
{
	UCHAR m_New_Temp=0,ii=0;
	UINT32 ret=0;

	x_memset(channels_Temp,0,sizeof(channels_Temp));
	
	m_New_Temp=0;
	stchannel->m_uiSymbolRate_Hz=0;
	AVL_DVBS2_BlindScanInit();
	ret= AVL_DVBS2_BlindScanAction(stchannel->m_uiFrequency_kHz/100,channels_Temp,&m_New_Temp);
	if(ret!=DRVAPI_TUNER_OK )
	{
		mcDBG_MSG_L2(("[AVL_SymbolRate_AutoDetec]  Err!\n"));
		return;
	}
	for(ii=0;ii<m_New_Temp;ii++)
	{
		if(AVL_JudgeSameChannel(stchannel->m_uiFrequency_kHz,channels_Temp[ii].m_uiFrequency_kHz,channels_Temp[ii].m_uiSymbolRate_Hz/1000))
		{
			stchannel->m_uiSymbolRate_Hz=channels_Temp[ii].m_uiSymbolRate_Hz;
			mcDBG_MSG_L2(("[AVL_SymbolRate_AutoDetec]  Find the symbol:%d Khz!\n",channels_Temp[ii].m_uiSymbolRate_Hz/1000));
			return;
		}
	}	
	mcDBG_MSG_L2(("[AVL_SymbolRate_AutoDetec]  Not Find the symbol !\n"));
}






//-------------------------------------------------------------------------
/**
 *  This API Notify MW scan status,do sema lock wait MW continue CMD
 *  @param   UINT8 signal       signal status:0 unlock,1 lock
 *  @param   UINT8 polar       polar type
 *  @param   UINT32 freq	locked TP freq
 *  @param   UINT32 sym	locked TP sym
 *  @param   UINT8 fec        Code rate
 */
//-------------------------------------------------------------------------
static  INT32 DVBS_AutoScanCallback(UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec)
{
	//mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	if(BlindScan_MidWare_Break())
	{
		mcDBG_MSG_L2(("[DVBS_AutoScanCallback] MD has been broken....\n"));
		//mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
		return 0;
	}
       while(1)
	{
		if(x_sema_lock_timeout(AutoScan_callback,10)==OSR_OK)
		{
			mcDBG_MSG_L2(("[DVBS_AutoScanCallback] reset AutoScan_callback...\n"));
			continue;
		}
		else
		{
			break;
		}
	}
	if (signal==1)	
	{
		mcDBG_MSG_L2(("[DVBS_AutoScanCallback] TP_Locked\n"));
		Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_GOT_SIGNAL);	
	}
	else if(signal==0)
	{
		mcDBG_MSG_L2(("[DVBS_AutoScanCallback] Next Band Request\n"));
		Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_LOST_SIGNAL);
	}   
	else if(signal==2)
	{
		mcDBG_MSG_L2(("[DVBS_AutoScanCallback]  AUTOSCAN_END\n"));
		Tuner_Notify(DRV_COND_STATUS,TUNER_COND_STATUS_AUTOSCAN_END);	//at the end of blind scan
	}
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);	
	Monitor_waiting_semaphore_flag=BOOL_TRUE;
	
        mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	//x_sema_lock_timeout(AutoScan_callback,8000);//liuyuan debug use
	mcSEMA_LOCK_MTFE(AutoScan_callback);
	mcDBG_MSG_L2(("[DVBS_AutoScanCallback]  Got Semphore..\n"));
	Monitor_waiting_semaphore_flag=BOOL_FALSE;		
	return 0;
}

static BOOL SetBreakMonitor(DVBS_PD_CTX_T *psPdCtx)
{
    UINT8 u1Cnt=0;
    
    psPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psPdCtx->fgPIMoniStatus){  //PI really doing PI Monitor
        while(psPdCtx->fgPIMoniStatus){ 
            //wait PI Monitor really finish break 
            if(u1Cnt>250){  //in case of deadlock
                break;   
            }
            u1Cnt++;
            mcDBG_MSG_L2(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
          //  mcDELAY_MS(10); 
        x_thread_delay((UINT32)(10));
        }            
    }
    
    return TRUE; 
}
static void ResumeMonitor(DVBS_PD_CTX_T *psPdCtx)
{
   mcDBG_MSG_L2(("[ResumeMonitor] is call\r\n"));
    psPdCtx->fgBreakMonitor = FALSE;
   mcSEMA_UNLOCK_MTFE(psPdCtx->t_escape_mon_suspend);
} 
/************************************************************************
*  MonitorProcess
*  Monitor thread process function.
*  @param  void.
*  @retval  void.
************************************************************************/
static void MonitorProcess(VOID *pvArg)
{
        UINT8 uNcount=0;
	DVBS_PD_CTX_T *psPdCtx = *(DVBS_PD_CTX_T **) pvArg;
	mcDBG_MSG_L2(("MonitorProcess start!\n"));
        
    while (psPdCtx->fgMonitorThreadRunning)
    {

      //  Snr = 0;
    	if (psPdCtx->fgBreakMonitor)
    	{
    		// After mcMUTEX_LOCK(t_escape_mon_suspend), monitor thread will be suspended until
    		// TunerAcq or TunerClose invoked.
            mcDBG_MSG_L2(("Entering DVBS mon_suspend\n"));
            mcSEMA_LOCK_MTFE(psPdCtx->t_escape_mon_suspend);
    		//mcMUTEX_LOCK(psPdCtx->t_escape_mon_suspend);
    	  	mcDBG_MSG_L2(("Escape DVBS mon_suspend\n"));
    	}
    	else
	{
		// mutex lock for I2C access
		//mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
		//psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
		//! [TODO:PI] put your monitor process code here
		if(do_blindscan == 1)
		{
			psPdCtx->fgPIMoniStatus = TRUE;
			AVL_DVBS2_BlindScanInit();
			AVL_BlindScan();
			psPdCtx->fgPIMoniStatus = FALSE;	
			do_blindscan = 0;
			Current_channel.m_uiFrequency_kHz=(bs_stop_freq)*1000;
			Current_channel.m_uiSymbolRate_Hz=0;			
			Current_channel.m_fec=FEC_I_UNKNOWN;				
			DVBS_AutoScanCallback(2, 0, 0,0,0);
		}
		else
		{
			     mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);

		        if(g_channle_locked_flag==0)
				{
					if(pPdCtxDVBS->fgUniCab ==FALSE)
					{
						uNcount++;
						if(uNcount>=3)
						{
							uNcount=0;
							AVL_ResendDiseqcCmmd(&g_diseqc_cmd);
						}
					}
					else
					{
					  	mcDBG_MSG_L2(("Unicable command resend...\n"));
						DVBS_OduChannelChg(&(psPdCtx->t_UniCab_info));
					}
				}
				else
				{
					uNcount=0;
				}
		     		mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock); 	
		}
		if(pPdCtxDVBS->fgUniCab == FALSE)
			x_thread_delay((UINT32)(500)); 
		else
			x_thread_delay(DVBS_GetUnicableDelay());// In unicable mod ,do not use the constant 500ms delay
		
	}
}
    
    //psPdCtx->t_monitor_thread = NULL;
    psPdCtx->t_monitor_thread = 0;
    mcDBG_MSG_L2(("MonitorProcess exit!\n"));
   x_thread_exit();
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static INT32 PD_StartMonitorEngine(DVBS_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgMonitorThreadRunning = TRUE;
   if(x_thread_create(&psPdCtx->t_monitor_thread,
   	          "DVBS_MonitorThread",
   	          cTHREAD_STACK_SIZE*10,cTHREAD_PRIORITY,
   	          MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
    {
        mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcDBG_MSG_L2(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  PD_StopMonitorEngine
*  Function which stops monitor thread engine.
*  @param  void.
*  @retval  DRVAPI_TUNER_OK : Stop monitor thread successfully.
************************************************************************/
static INT32 PD_StopMonitorEngine(void)
{
    while (pPdCtxDVBS->t_monitor_thread){
      x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_L1(("DVBS PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

//-------------------------------------------------------------------------
/**
 *  This API do connect tuner
 *  @retval  void
 */
//-------------------------------------------------------------------------
VOID  DVBS_TunerConnect(void)
{
	if(pPdCtxDVBS->fgDisStatus == FALSE)
	{
		/* Has connected, return */
		mcDBG_MSG_L2(("DVBS Has connect!\n"));
		return;
	}
    //add driver implementation code here
    mcDBG_MSG_L2(("[DVBS_TunerConnect] Enable TS Output!\n"));
	AVL_DVBS2_Enable_TS();

	/* set disconnect status */
	pPdCtxDVBS->fgDisStatus = FALSE;
}

//-------------------------------------------------------------------------
/**
 *  This API implement close tuner. It should performs
 *  1. free all allocated resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
static INT32 DVBS_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
     
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtxDVBS->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtxDVBS->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
     mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    x_sema_delete(pPdCtxDVBS->t_escape_mon_suspend); 

    //add driver implementation code here
    x_sema_delete(pPdCtxDVBS->hHalLock);
    x_mem_free(pPdCtxDVBS);
    //x_mem_free(psDemodCtx);

    //To Do If you have allocated any resource in TunerOpen(), release them here.

    return (DRVAPI_TUNER_OK);
}
//-------------------------------------------------------------------------
/**
 *  This API do channel acuqusition.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @param   pv_conn_info       middleware tuner info structure, refer u_tuner.h
 *  @param   z_conn_info_len    length of pv_conn_info
 *  @param   _BreakFct          TunerHAL callback API for Vendor_TunerAcq to check whether need to break TunerAcq
 *  @param   *pvArg             pass this parameter to _BreakFct when calling _BreakFct
 *  @retval  1                  TPS LOCK
 *  @retval  0                  TPS UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DVBS_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
	TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;//=(TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
	BOOL fgChannelScan=FALSE;	
	BOOL fgLock=FALSE;
	UINT32 freq=0;
	UINT32 symbolrate=0;	
	UINT16 uiLockStatus=0;
	UINT32 ret=0;
	UINT16 WaitLoop=0;
	DVBS2_CHANNEL_T stchannel;
	TUNER_CONNECTION_MODE_T e_conn_mode=CONN_MODE_UNKNOWN;
	
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));
	if(pv_conn_info!=NULL)
	{
		pt_tuner_info=(TUNER_SAT_DIG_TUNE_INFO_T *) pv_conn_info;
	}
	else
	{
		mcDBG_MSG_L2(("[DVBS_TunerAcq] pv_conn_info is Err.\n"));
		return (fgLock);   
	}
//set break function
	pPdCtxDVBS->isBreak=_BreakFct;
//mutex lock
	//get conncetion mode
	e_conn_mode=Util_GetConnMode(pPdCtxDVBS,pt_tuner_info->e_conn_mode);
	if(!DVBS_ValidateMwPara(pt_tuner_info,e_conn_mode))
	{
       mcDBG_MSG_ERR(("invalid parameter!!\n"));
       return fgLock;
	}
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Pause monitor thread
	if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{
		mcDBG_MSG_L2(("Pause monitor process!!\n"));
		SetBreakMonitor(pPdCtxDVBS);
	}
	//Parse parameter from upper layer


	//----------------------------------------------------------------Availink added
	DVBS_TunerConnect();	
	
	freq=pt_tuner_info->ui4_freq;
	symbolrate=(pt_tuner_info->ui4_sym_rate)*1000;
//	mcDBG_MSG_L2(("[DVBS_TunerAcq]  ui4_sym_rate:% !\n",pt_tuner_info->ui4_sym_rate));
 //get conncetion mode
        
 
	switch(e_conn_mode)
	{
		case CONN_MODE_SCAN://this mode is for blindscan 
				{
					mcDBG_MSG_L2(("Start to BlindScan  ,Set start frequency value[%d,%d]MHz !\n",pt_tuner_info->ui4_start_freq,pt_tuner_info->ui4_end_freq));
	                            /*before AVL_BlindScan()function finished First runnings scan ,MD will call DVBS_TunerGetSync for blindscan start freq.*/				
					bs_start_freq=pt_tuner_info->ui4_start_freq;
					bs_stop_freq=	pt_tuner_info->ui4_end_freq;	
					Current_channel.m_uiFrequency_kHz=1000*bs_start_freq;
					Current_channel.m_uiSymbolRate_Hz=0;
					Current_channel.m_fec=FEC_I_UNKNOWN;				
					do_blindscan=1;
				}
				break;
		case CONN_MODE_UNICABLE_SCAN:
				{
				   TUNER_UNICABLE_CC_T *pUniCabinfo=&(pPdCtxDVBS->t_UniCab_info); 
                   mcDBG_MSG_L2(("[%s]Start to UNICABLE BlindScan  ,Set start frequency value[%d,%d]MHz !\n",__FUNCTION__,pt_tuner_info->ui4_start_freq,pt_tuner_info->ui4_end_freq)); 
				   mcDBG_MSG_L2(("F_ub=%dMHz,UB=%d,bank=%d\n",\
				   pUniCabinfo->ui2_band_freq,pUniCabinfo->e_user_band,pUniCabinfo->e_bank
				   ));
				   //here is sample
				   //TODO:Please add your implemention to do unicable blind scan
				   	bs_start_freq=pt_tuner_info->ui4_start_freq;
					bs_stop_freq=	pt_tuner_info->ui4_end_freq;	
					Current_channel.m_uiFrequency_kHz=1000*bs_start_freq;
					Current_channel.m_uiSymbolRate_Hz=0;
					Current_channel.m_fec=FEC_I_UNKNOWN;				
					do_blindscan=1;
				   //Add your code here to do unicable blind scan
				}
				break;
		case CONN_MODE_UNICABLE_SURF://d120718_Haibo:When unicable,the ui4_freq is band freq	
				mcDBG_MSG_L2(("Unicable tuning.Band freq is %dMHz\n",freq));	        
		case CONN_MODE_UNKNOWN://channel change
		case CONN_MODE_SURF://preset scan ,single tp scan
				{//call you channel change driver here
					        if(symbolrate == 0 )
				        	{
					            stchannel.m_uiSymbolRate_Hz=0;
						    stchannel.m_uiFrequency_kHz=freq*1000;
				        	   AVL_SymbolRate_AutoDetec(&stchannel);
					           if(stchannel.m_uiSymbolRate_Hz>0)
					           	{					           	
                                                          symbolrate= stchannel.m_uiSymbolRate_Hz;
					           	}
							  else
						   	{
							   	break;
						   	}
							   
				        	}
							if(pPdCtxDVBS->fgUniCab==TRUE)
							{
								mcDBG_MSG_L2(("transponder & LNB freq is %d and %d\n",pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq,pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq));
								Current_channel.m_uiFrequency_kHz=(UINT32)AVL_abs((UINT32)((pPdCtxDVBS->t_UniCab_info.ui4_transponder_freq)-(UINT32)(pPdCtxDVBS->t_UniCab_info.ui2_lnb_freq))*1000);  
								mcDBG_MSG_L2(("current freq is %d\n",Current_channel.m_uiFrequency_kHz));
							}
							else					
								Current_channel.m_uiFrequency_kHz=freq*1000;

					Current_channel.m_uiSymbolRate_Hz=symbolrate;
					Current_channel.m_fec=FEC_I_UNKNOWN;
					ret=AVL_DVBS2_LcokChannel(freq*10, symbolrate,pPdCtxDVBS->fgUniCab);
					if(ret==DRVAPI_TUNER_OK)
					{
						WaitLoop=AVL_DVBS2_Get_MaxLoop();
						while(WaitLoop--)
						{
							x_thread_delay(10);
							uiLockStatus=AVL_DVBS2_CheckTP_IsLocked();
							if(g_u1EnEarlyBreak && uiLockStatus)
							{
							   break;
							}
							if(pPdCtxDVBS->isBreak(pvArg))
							{
								mcDBG_MSG_L2(("Channel lock Break 1!\n"));
								break;
							}	
						}
					}
				
					break;
				}
		default:
				mcDBG_MSG_ERR(("Invalid mode to DVBS_TunerAcq!\n"));
				break;
	}

	if(pt_tuner_info->e_conn_mode == CONN_MODE_SCAN || pt_tuner_info->e_conn_mode == CONN_MODE_UNICABLE_SCAN)
	{
		uiLockStatus=1;//scand mode should return LOCK Status.
	}
	  
	if(uiLockStatus)
	{
	   mcDBG_MSG_L2(("demod lock!\n"));
                ui4_data_forMW = pt_tuner_info->ui4_data;
		g_channle_locked_flag=1;
		fgLock=1;
	}
	else
	{
	    mcDBG_MSG_L2(("demod unlock!\n"));
		g_channle_locked_flag=0;
		fgLock=0;
	}

	//-------------------------------------------------------------------------------------

	if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{
	    if (!_BreakFct(pvArg) && (!fgChannelScan))
	    {
    	        mcDBG_MSG_L2(("Enable monitor process!!\n"));
	        ResumeMonitor(pPdCtxDVBS);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
	    }
	}

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	// TODO return correct lock status
	return (fgLock);    //if lock return 1, unlock return 0
	}
//-------------------------------------------------------------------------
/**
 *  This API check lock status
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  1                  LOCK
 *  @retval  0                  UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL DVBS_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	BOOL fgLock=FALSE;

	if(do_blindscan == 1)
	{
		mcDBG_MSG_L4(("[%s] doing blindscan.\n",__FUNCTION__));
		return TRUE;
	}	
	mcDBG_MSG_L4((" %s \n",__FUNCTION__));
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//TODO Get demod lock status from demod driver and update it as variable fgLock
	fgLock=AVL_DVBS2_CheckTP_IsLocked();

	if(fgLock)
	{
		//mcDBG_MSG_L2(" %s ,lock\n",__FUNCTION__);
		g_channle_locked_flag=1;
	}
	else
	{
		//mcDBG_MSG_L2(" %s ,unlock\n",__FUNCTION__);
		g_channle_locked_flag=0;
	}
	//END
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return fgLock;
}

//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  void
 */
//-------------------------------------------------------------------------
/*
****************
After DVBS_TunerDisc being called,there will be no TUNER_SET_TYPE_SCAN_CONTINUE command .
So the Monitor_waiting_semaphore_flag is important.......
**********************
*/
static VOID  DVBS_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));  
    if(pPdCtxDVBS->fgUniCab)//d120721_Haibo:power off SCIF  and confirm it is done indeed
	{  
	   DVBS_TunerDiscSCIF(pPdCtxDVBS);
	}
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

	//TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered
	pPdCtxDVBS->fgDisStatus = TRUE;
	AVL_DVBS2_Disable_TS();
	if(Monitor_waiting_semaphore_flag == BOOL_TRUE)
	{
		mcDBG_MSG_L2(("[DVBS_TunerDisc]  111 set to stop Blindscan .\n"));
		x_sema_unlock(AutoScan_callback);
	}
	
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);	
//END     
	SetBreakMonitor(pPdCtxDVBS);

	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	if(Monitor_waiting_semaphore_flag == BOOL_TRUE)
	{
		mcDBG_MSG_L2(("[DVBS_TunerDisc]  222 set to stop Blindscan .\n"));
		x_sema_unlock(AutoScan_callback);
	}
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);		
	
}

//-------------------------------------------------------------------------
/**
 *  This API get modulation information.
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  Outputs:
 *  @param   *_pSignal          Modulation info. structure to be filled.
 *  @retval  void
 */
//-------------------------------------------------------------------------
static void DVBS_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    	//apply signal code here
    	if(_pSignal)
	{
		_pSignal->Frequency  = Current_channel.m_uiFrequency_kHz/1000;  	
		_pSignal->SymbolRate = Current_channel.m_uiSymbolRate_Hz/1000;
	}
	else
	{
		mcDBG_MSG_ERR(("[DVBS_TunerGetSignal] _pSignal is Err .\n"));
	}
	
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in terms of percentage.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 DVBS_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{

	UINT8 u1Ret=0;
	
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//TODO Read If_AGC control word and mapping it to signal level(0~100)
	
	u1Ret=AVL_DVBS2_Get_SignalLevelPercent();
	//END
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	//example of return
	return (u1Ret);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in multiples of (0.1dBm).
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0 ~ -900           signal level
 */
//-------------------------------------------------------------------------
static INT16 DVBS_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	INT16 i2Ret=0;
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//-----------------------------------------added by Availink
	i2Ret=(INT16)AVL_DVBS2_Get_SignalLevelPercent();
	//-----------------------------------------------------------------------------
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return i2Ret;
}

static UINT32 DVBS_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	UINT32 u4_ret=100000;//unit is 1e-5

	if(do_blindscan == 1)
	{
		mcDBG_MSG_L2(("[%s] doing blindscan.\n",__FUNCTION__));
		return 0;
	}		
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));
	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-5

	AVL_DVBS2_Get_PER(&u4_ret);//return unit is 1e-9;

	//END
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return u4_ret;//modify return value
}
static UINT16 DVBS_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	UINT16 u2_ret=0;
	UINT32 u4_ret=0;
	if(do_blindscan == 1)
	{
		mcDBG_MSG_L2(("[%s] doing blindscan.\n",__FUNCTION__));
		return 0;
	}	
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	 
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//TODO Get SNR(signal-noise ratio) from demod driver
	AVL_DVBS2_Get_SNR(&u4_ret);
	u2_ret=(UINT16)u4_ret;
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return u2_ret;//modify return value
}

static void DVBS_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	 
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

	AVL_DVBS2_Set_TS_ParallelMode();

	AVL_DVBS2_Enable_TS();
	//----------------------------------------------------------------------		
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static char *DVBS_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return DVBS_Vendor_Driver_Ver;
}

static INT32 DVBS_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

	//-------------------------------------------------

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return 0;
}
static void DVBS_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}
static void DVBS_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
   //TODO you can add command for debug use here
   //END
    UCHAR   ucCmdId = 0;  
    UINT32 ret=0;	
	UINT8 u1lnbpowerset=0;
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t a: [Freq_KHz] [SymbolRate_S]   Lock specified channel\n"));   
        mcDBG_MSG_L1(("\t b: Check Lock Status\n"));
		mcDBG_MSG_L1(("\t g: set lnb power\n"));
		mcDBG_MSG_L1(("\t j: xx[FreqMHz] xx[SymbolRateKbs].\n"));
    }	
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);
        
    switch (ucCmdId)
    {
    case 'a':
	    	mcDBG_MSG_L2(("b_tone_burst 1 \n"));
			 if (i4Argc >2)
			 {
                  UINT32 u4Freq=(UINT32)StrToInt(aszArgv[1]);
				  UINT32 u4SymbolRate=(UINT32)StrToInt(aszArgv[2]);
                  Current_channel.m_uiFrequency_kHz=u4Freq;
				  Current_channel.m_uiSymbolRate_Hz=u4SymbolRate;
					Current_channel.m_fec=FEC_I_UNKNOWN;
					ret=AVL_DVBS2_LcokChannel(u4Freq*10, u4SymbolRate,pPdCtxDVBS->fgUniCab);
					if(ret!=DRVAPI_TUNER_OK)
					{

                        mcDBG_MSG_ERR(("Lock channel fail\n"));
					}
			 }
		break;
    case 'b':
	    	mcDBG_MSG_L2(("b_tone_burst 0\n"));
			mcDBG_MSG_L1(("Lock status=%d\n",AVL_DVBS2_CheckTP_IsLocked()));
		break;	
    case 'c':
	Demod_GPIO_Reset();
	
	AVL_DVBS2_Demod_Init();

		break;	
    case 'd':
			Demod_GPIO_Reset();
	
		
		break;	
    case 'e':    
    		AVL_DVBS2_Demod_Init();
        break;
    case 'f':    
    	AVL_DVBS2_LNB_Power_OnOff(0);
        break;
    case 'g':  
		{
			if (i4Argc > 1)
			{
		        u1lnbpowerset = *((UINT8 *) aszArgv[1]);
				switch(u1lnbpowerset)
					{
					case '1':
						{
						mcDBG_MSG_L1(("set lnb power as 14v.\n"));
						AVL_DVBS2_LNB_Control_14or18V(1);
						break;
						}
					case '2':
						{
						mcDBG_MSG_L1(("set lnb power as 18v.\n"));
						AVL_DVBS2_LNB_Control_14or18V(0);
						break;
						}
					case '3':
						{
						mcDBG_MSG_L1(("set lnb power as 15v.\n"));
						AVL_DVBS2_LNB_Control_15or19V(1);
						break;
						}
					case '4':
						{
						mcDBG_MSG_L1(("set lnb power as 19v.\n"));
						AVL_DVBS2_LNB_Control_15or19V(0);
						break;
						}
					default:
						mcDBG_MSG_L1(("set lnb power need to input para.\n"));
					break;
					}
			}
			else
			{
    			mcDBG_MSG_L1(("nim.dtd g 1:set lnb power 14v.\n"));
    			mcDBG_MSG_L1(("nim.dtd g 2:set lnb power 18v.\n"));
    			mcDBG_MSG_L1(("nim.dtd g 3:set lnb power 15v.\n"));
    			mcDBG_MSG_L1(("nim.dtd g 4:set lnb power 19v.\n"));
			}
    	}
        break;
    case 'h':    
    			mcDBG_MSG_L1(("set 22k on.\n"));
    	AVL_DVBS2_22K_ON();
        break;		
	case 'i':
		mcDBG_MSG_L1(("set 22k off.\n"));
		AVL_DVBS2_22K_OFF();
		break;
	case 'j':
			{
				UINT16 WaitLoop=0;
				BOOL uiLockStatus=0;
				UINT32 u4FreqMHz=1450;
				UINT32 u4SymbolRateKbs=30000;
				if(i4Argc > 2)
				{
					u4FreqMHz=(UINT32)StrToInt(aszArgv[1]);
					u4SymbolRateKbs=(UINT32)StrToInt(aszArgv[2]);
						
					mcDBG_MSG_L1(("current Freq is %dMhz , SymbolRate is %d kbs.\n",u4FreqMHz,u4SymbolRateKbs));
					
					ret=AVL_DVBS2_LcokChannel(u4FreqMHz*10, u4SymbolRateKbs*1000,pPdCtxDVBS->fgUniCab);
					if(ret==DRVAPI_TUNER_OK)
					{
						WaitLoop=AVL_DVBS2_Get_MaxLoop();
						while(WaitLoop--)
						{
							x_thread_delay(10);
							uiLockStatus=AVL_DVBS2_CheckTP_IsLocked();
						}
						if(uiLockStatus)
							{
							mcDBG_MSG_L1(("channel lock.\n"));
							}
						else
							{
							mcDBG_MSG_L1(("channel unlock.\n"));
							}
					}
				}
				else
				{
				mcDBG_MSG_L1(("nim.dtd j xx[FreqMHz] xx[SymbolRateKbs].\n"));
				}
			}
		break;
	case 'k':
		{
			DVBS2_CHANNEL_T stchannel;
			stchannel.m_uiSymbolRate_Hz=0;
			stchannel.m_uiFrequency_kHz=1450*1000;
			AVL_SymbolRate_AutoDetec(&stchannel);
			}
		break;
	case 'l':
			{
			DVBS2_CHANNEL_T stchannel;
			stchannel.m_uiSymbolRate_Hz=0;
			stchannel.m_uiFrequency_kHz=1452*1000;
			AVL_SymbolRate_AutoDetec(&stchannel);
			}	
		break;
	case 'm':
	     {
		   UINT32 u4Start;
		   DVBS_OduPowerOff(pPdCtxDVBS);
		   u4Start=x_os_get_sys_tick();
			while(1)
			{
		     if(!AVL_DVBS2_CheckTP_IsLocked())break;;
			}
			mcDBG_MSG_L1(("Stable time is %d ms\n",(x_os_get_sys_tick()-u4Start)*x_os_get_sys_tick_period()));
	     DVBS_TunerDiscSCIF(pPdCtxDVBS);
		 }
     break;
	 case 'n':
	   if (i4Argc >1)
	   {
            g_u1RetryCnt=(UINT8)StrToInt(aszArgv[1]);
			mcDBG_MSG_L1(("g_u1RetryCnt is %d\n",g_u1RetryCnt));
	   }		  
		break;
    default:
  /*      mcSHOW_USER_MSG(("Cmd: \n"));
        mcSHOW_USER_MSG(("\t SetChannel  [Freq (MHz)] [symbrate(kbps)]: set channel\n"));
		mcSHOW_USER_MSG(("\t blindscan  [sFreq (MHz)] [eFreq (MHz)]  : blind scan\n"));
		mcSHOW_USER_MSG(("\t s           : status\n"));
		mcSHOW_USER_MSG(("\t ts [x]\n\t    'p'-parallel TS;\n\t    's'-serial TS;\n\t    'o'-open TS output;\n\t    'c'-close TS output;\n"));
		mcSHOW_USER_MSG(("\t Dint        : demod driver init\n"));
        mcSHOW_USER_MSG(("\t checkreg    : dump demod all regsiter\n"));
        mcSHOW_USER_MSG(("\t diseqc_op [work mode][diseqc cmd][cmd length]:set diseqc 1x cmd\n"));
		mcSHOW_USER_MSG(("\t setH/V [0/1] :0--set Horizontal  1--set Vertical\n"));
		mcSHOW_USER_MSG(("\t scancontinue \n"));//liuyuan debug use
		mcSHOW_USER_MSG(("\t Disc        : End blindscan\n"));
		mcSHOW_USER_MSG(("\t w  [reg_addr][lenth][data]: write demod reg\n"));
		mcSHOW_USER_MSG(("\t r  [reg_addr][lenth]: read demod reg\n"));
		mcSHOW_USER_MSG(("\t iqswap  [0/1]:swap IQ signal\n"));*/

        break;
    }    

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static void DVBS_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBS_GetTsFmt
 *  Get demod TS output format.
 *  @param  psDemodCtx  Pointer of context created by DVBT_DemodCtxCreate.
 *  @retval   
 */
//-----------------------------------------------------------------------------
TS_FMT_T DVBS_GetTsFmt(void)
{
	//UINT8 ucData,ucData2;
	TS_FMT_T fgTsFmt;
	UCHAR SerOrParal,Clk_Polar,Ts_Tri;
	AVL_DVBS2_Get_TSMode(&SerOrParal,&Clk_Polar,&Ts_Tri);
	
	//follow is an example,pls apply you code
	//Parallel or Serial
	if(SerOrParal == 1)
		fgTsFmt.SerOrPar = SP_SERIAL;
	else
		fgTsFmt.SerOrPar = SP_PARALLEL;
	    
	//clock polarity;
	if(Clk_Polar == 0)
		fgTsFmt.ClockPolarity = POL_LOW_ACTIVE;     
	else
		fgTsFmt.ClockPolarity = POL_HIGH_ACTIVE;
	
	//GateClock -->continue clk or not?
	fgTsFmt.fgGatedClock = BOOL_FALSE;//only continue clk  mode

	//Tristate 
	if(Ts_Tri == 0 )
		fgTsFmt.fgTristate = BOOL_FALSE;
	else
		fgTsFmt.fgTristate = BOOL_TRUE;

	//TS clk = ???KHZ,calculate by parallel
	fgTsFmt.u4TsClockInKHz = AVL_DVBS2_Get_TSClock();
	
	//ExternalDemod
    fgTsFmt.fgExternalDemod = BOOL_TRUE;//external Demod or not

    return fgTsFmt;
}

//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_DVBS
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_DVBS_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 DVBS_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{   
	TUNER_SAT_DIG_TUNE_INFO_T *pAttri0;
	TUNER_DIG_ATTRIBUTE_T *pAttri1;
	INT32 i4Temp = 0;
	INT16 iRFOff_100Khz=0;
	UINT8 CurrentSystem=0;
	struct AVL_DVBSx_SignalInfo SignalInfo; 	
   
	if(pzAttribute!=NULL)
	{
		pAttri0 = (TUNER_SAT_DIG_TUNE_INFO_T*)pzAttribute;
	}
	else
	{
		mcDBG_MSG_L2(("[DVBS_TunerGetAttribute] pzAttribute IS Err\n"));
		 return DRVAPI_TUNER_OK;
	}
	
	if((Current_channel.m_uiSymbolRate_Hz<5000000)&&(AVL_DVBS2_CheckTP_IsLocked()==FALSE)&&(e_get_type==TUNER_GET_TYPE_SQI))
	{
		mcDBG_MSG_L4(("[DVBS_TunerGetAttribute] Do not get SQI when sym <5M & demod unlock\n"));
		return DRVAPI_TUNER_OK;
	}
	
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);   
	 mcDBG_MSG_L4((" %s ,e_get_type:%d\n",__FUNCTION__,e_get_type));

    //add driver implementation code here
    switch (e_get_type)
    {
	    case TUNER_GET_TYPE_SIGNAL_LEVEL:
	        	{
				//mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SIGNAL_LEVEL\r\n"));
		       	*pzAttributeLen = sizeof(INT32);
				i4Temp =  AVL_DVBS2_Get_SignalLevelPercent();
		      	x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
	    		}
			break;
		case TUNER_GET_TYPE_SAT_DIG:
			{	/*maybe return Last_program_channel*/
			        //mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SAT_DIG:freq:%d\r\n",Current_channel.m_uiFrequency_kHz));
                UINT32 u4TunerFreq_KHz=Current_channel.m_uiFrequency_kHz; 
				if(AVL_DVBS2_CheckTP_IsLocked())
	       	 	{	
	       	 		AVL_DVBS2_Get_SignalOffSet(&iRFOff_100Khz);
					mcDBG_MSG_L2(("Get OFFSET=%d Khz\r\n",iRFOff_100Khz*100));
					u4TunerFreq_KHz=Current_channel.m_uiFrequency_kHz-iRFOff_100Khz*100;
					CurrentSystem=AVL_DVBS2_Get_SYSTEM();
					pAttri0->e_dvb_s_s2_mode=(TUNER_DVB_S_S2_MODE_T)CurrentSystem;
	       	 	}
				 else
			 	{
			 		CurrentSystem=TUNER_DVB_AUTO_S_S2;
					pAttri0->e_dvb_s_s2_mode=TUNER_DVB_AUTO_S_S2;
				 	mcDBG_MSG_L2(("[DVBS_TunerGetAttribute] tp is unlocked.unable to get offset\r\n"));
			 	}


				if(0 == AVL_DVBS2_Get_Signal_Info((VOID*)&SignalInfo))
				{
								
					if	(CurrentSystem==TUNER_DVB_S2)
						{
						  switch(SignalInfo.m_rolloff)
						  {
						      case AVL_DVBSx_RO_35:
							       pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_35;
								   mcDBG_MSG_L2(("S2 Roll off parameter is 0.35\n"));break;
							  case AVL_DVBSx_RO_25:
								   pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_25;
								   mcDBG_MSG_L2(("S2 Roll off parameter is 0.25\n"));break;
							  case AVL_DVBSx_RO_20:
								   pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_20;
								   mcDBG_MSG_L2(("S2 Roll off parameter is 0.20\n"));break;
							  default:
									break;
							}
									
							switch(SignalInfo.m_modulation)
						    {
							   case AVL_DVBSx_MM_QPSK:
									pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_QPSK;
								    mcDBG_MSG_L2(("Modulation : QPSK\n")); break;
							   case AVL_DVBSx_MM_8PSK:
									pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_8PSK;
								    mcDBG_MSG_L2(("Modulation : 8PSK\n")); break;
							   case AVL_DVBSx_MM_16APSK:
								    pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_16APSK;
									mcDBG_MSG_L2(("Modulation : 16APSK\n")); break;
							   case AVL_DVBSx_MM_32APSK:
								    pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_32APSK;
									mcDBG_MSG_L2(("Modulation : 32APSK\n")); break;
							   default:
									pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_INVALID;
								    mcDBG_MSG_L2(("Modulation : Unknown\n")); break;
							  }
					 
							 switch(SignalInfo.m_coderate)
							 {
							    case RX_DVBS2_1_4:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_4;
									 mcDBG_MSG_L2(("Code Rate : 1/4\n")); break;
								case RX_DVBS2_1_3:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_3;
									 mcDBG_MSG_L2(("Code Rate : 1/3\n")); break;
							    case RX_DVBS2_2_5:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_5;
									 mcDBG_MSG_L2(("Code Rate : 2/5\n")); break;
							    case RX_DVBS2_1_2:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_2;
									 mcDBG_MSG_L2(("Code Rate : 1/2\n")); break;
							    case RX_DVBS2_3_5:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_5;
									 mcDBG_MSG_L2(("Code Rate : 3/5\n")); break;
								case RX_DVBS2_2_3:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_3;
									 mcDBG_MSG_L2(("Code Rate : 2/3\n")); break;
								case RX_DVBS2_3_4:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_4;
									 mcDBG_MSG_L2(("Code Rate : 3/4\n")); break;
							    case RX_DVBS2_4_5:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_4_5;
									 mcDBG_MSG_L2(("Code Rate : 4/5\n")); break;
								case RX_DVBS2_5_6:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_5_6;
									 mcDBG_MSG_L2(("Code Rate : 5/6\n")); break;
								case RX_DVBS2_8_9:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_8_9;
									 mcDBG_MSG_L2(("Code Rate : 8/9\n")); break;
								case RX_DVBS2_9_10:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_9_10;
									 mcDBG_MSG_L2(("Code Rate : 9/10\n")); break;
							    default:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
									 mcDBG_MSG_L2(("Code Rate : Unknown(%d)\n", SignalInfo.m_coderate)); break;
							}
										
						     if(SignalInfo.m_pilot)
							 {
								 pAttri0->b_pilot_insert=1;
								 mcDBG_MSG_L2(("Pilot Mode : On\n"));
							 }
							 else
							 {
								 pAttri0->b_pilot_insert=0;
								 mcDBG_MSG_L2(("Pilot Mode : Off\n"));
							 }
						   }
								
					 else if(CurrentSystem==TUNER_DVB_S)
						   {
							   pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_QPSK;
							   pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_0_35;
							   pAttri0->b_pilot_insert=0;
					
							   switch(SignalInfo.m_coderate)
							   {
								case RX_DVBS2_1_2:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_1_2;
									 mcDBG_MSG_L2(("Code Rate : 1/2\n")); break;
								case RX_DVBS_2_3:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_2_3;
									 mcDBG_MSG_L2(("Code Rate : 2/3\n")); break;
								case RX_DVBS_3_4:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_3_4;
									 mcDBG_MSG_L2(("Code Rate : 3/4\n")); break;
								case RX_DVBS_5_6:
									 pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_5_6;
									 mcDBG_MSG_L2(("Code Rate : 5/6\n")); break;				
								case RX_DVBS_7_8:
									pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_7_8;
									mcDBG_MSG_L2(("Code Rate : 7/8\n")); break;
								default:
									pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
									mcDBG_MSG_L2(("Code Rate : Unknown(%d)\n", SignalInfo.m_coderate)); 
									break;
								}
						  
							}
					 else
						{
							pAttri0->e_dvb_s_s2_roll_off=TUNER_ROLLOFF_RESERVE;
							pAttri0->e_dvb_s_s2_coderate=TUNER_CODERATE_INVALID;
							pAttri0->e_dvb_s_s2_modulation=TUNER_MODULATION_INVALID;
							pAttri0->b_pilot_insert=0;
							mcDBG_MSG_L2(("not DVBS & DVBS2 system\n"));
						}
					}
			     pAttri0->e_fec_inner = Current_channel.m_fec;
			     pAttri0->ui4_freq = u4TunerFreq_KHz/1000;
				 pAttri0->ui4_sym_rate = Current_channel.m_uiSymbolRate_Hz/1000;
				 pAttri0->ui4_data = ui4_data_forMW;
				 pAttri0->e_pol = E_POL; 
                 mcDBG_MSG_L2(("[DVBS_TunerGetAttribute]  ui4_freq:%d MHz symbol Rate=%d KS/s DVBS system=%d(0:unknown,1:dvbs,2:dvbs2)!\n",pAttri0->ui4_freq,pAttri0->ui4_sym_rate,pAttri0->e_dvb_s_s2_mode));
			     
			}
			break;
		case TUNER_GET_TYPE_SQI:
			{
			       //mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
			        *pzAttributeLen = sizeof(INT32);
			        i4Temp = (INT32)AVL_DVBS2_Get_SignalQualityPercent(); //TODO Get BER from demod driver.Note the unit is 1e-5
			        x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
	        	}
			break;
	    case TUNER_GET_TYPE_DIG_ATTRIBUTE:   	
	        	{
				//mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_DIG_ATTRIBUTE\r\n"));
		        	pAttri1 = (TUNER_DIG_ATTRIBUTE_T *) pzAttribute;
		       	//follow is an example,pls apply you code
		       	//Tuner frequency boundary
		        	pAttri1->ui4_lower_bound_freq = bs_start_freq;//950Hz
		        	pAttri1->ui4_upper_bound_freq = bs_stop_freq;//2150M
		       	// freq offset tolerence range in Hz (1M mapping to 100K)
		       	 pAttri1->ui4_fine_tune_delta_freq = 3;  //3//
	    		}
	        	break;
		case TUNER_GET_TYPE_TS_FMT:
	        	{
	            		TS_FMT_T tsFmt;
				//mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_TS_FMT\r\n"));
	            		*pzAttributeLen = sizeof(TS_FMT_T);
	           		tsFmt = DVBS_GetTsFmt();
	            		x_memcpy(pzAttribute, &tsFmt, *pzAttributeLen);
	        	}
	        	break;
	     default:
	        		mcDBG_MSG_L4(("unknown: get Attribute %d\r \n",e_get_type));
	        		break;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
    return DRVAPI_TUNER_OK;
}
//-------------------------------------------------------------------------
/**
 *  This API set attribute to tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_set_type         tuner set type, check x_tuner.h set operation TUNER_SET_TYPE_DVBSX
 *  @param   *pvAttribute       set value.
 *  @param   zAttributeLen      length of *pvAttribute.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT32 DVBS_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
	UINT32 i=0;
	UINT32 j=0;
	UCHAR	ucData=0;// = *(UCHAR *)pvAttribute;  
	DISEQC_BUS_CMD_T *diseqc_cmd;// = (DISEQC_BUS_CMD_T*) pvAttribute; 
      static UCHAR nDiseqcReset_Flag=1;
#ifdef Debug_time	
	HAL_TIME_T TimeStart,TimeEnd,TimeDst;
       HAL_GetTime(&TimeStart);
#endif	
    //d120712_Haibo:To skip HAL sema check
	if( e_set_type == TUNER_SET_TYPE_SCAN_CONTINUE)
	{
		mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
		x_sema_unlock(AutoScan_callback);//d120712_Haibo:release the sema to continue to blind scan next band
		return DRVAPI_TUNER_OK;
	}

	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    //add driver implementation code here

	switch (e_set_type)
	{
		case TUNER_SET_TYPE_DISEQC_CMD:
		    diseqc_cmd = (DISEQC_BUS_CMD_T*) pvAttribute; 
			if(((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)||((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK) != 0)||(diseqc_cmd->ui1_tone_burst==1)||(diseqc_cmd->ui1_tone_burst==0))
			{
				mcDBG_MSG_L2(("[AVL_ResendDiseqcCmmd] stop 22k \n"));
				AVL_DVBS2_22K_OFF();
			}
			g_channle_locked_flag=1;//Reset the lock flag to TRUE.This is very important.
			x_memcpy(&g_diseqc_cmd,diseqc_cmd,sizeof(DISEQC_BUS_CMD_T));//availink :add for desiqc control ,20110210 lmh
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_LNB_POWER_MASK) != 0 )
			{
			    switch(diseqc_cmd->ui1_lnb_power)
			       	{
			       	case LNB_POWER_OFF:
						mcDBG_MSG_L2(("set lnb power off\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_OFF);
						LNB_Control(LNB_POWER_0);
						nDiseqcReset_Flag=1;
						break;
					case LNB_POWER_13V_18V:
						mcDBG_MSG_L2(("set lnb power normal\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_NORMAL);
						break;
					case LNB_POWER_14V_19V:
						mcDBG_MSG_L2(("set lnb power plus\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_PLUS);
						break;
					case LNB_POWER_ON:
						mcDBG_MSG_L2(("set lnb power on\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_NORMAL);
						break;
				       default:
					   	mcDBG_MSG_L2(("set lnb power not support...\n"));
					   	break;
			       	}
				//mcDELAY_MS(20);
				//mcDBG_MSG_L2(("After DISEQC_CMD_CTRL_LNB_POWER_MASK,delay 20ms,delay>15ms\n"));		   
			}
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_POLARIZATION_MASK) != 0 )
			{//apply you code
				if(diseqc_cmd->ui1_polarity_13v_18v == 1)
				{
					mcDBG_MSG_L2(("set 18V\n"));
					//A8293_Control(LNB_POWER_18);
					LNB_Control(LNB_POWER_18);
				}
				else
				{
					mcDBG_MSG_L2(("set 13V\n"));
					//A8293_Control(LNB_POWER_13);
					LNB_Control(LNB_POWER_13);
				}
				if(nDiseqcReset_Flag==1)
				{
					nDiseqcReset_Flag=0;
					mcDBG_MSG_L2(("Delay  130ms  for Diseqc Reset..\n"));
					mcDELAY_MS(130);		
				}
				else
				{
					mcDELAY_MS(20);
					mcDBG_MSG_L2(("After set H/V,delay 20ms,delay>15ms\n"));
				}
				
	    		}
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)
			{//apply you code
			    for(i=0;i<g_repeat;i++)
			    {
			      if(i==1)
			      {
				    diseqc_cmd->aui1_cmd[0]|=0x01;
			      }
				  else
			      {
                  diseqc_cmd->aui1_cmd[0]&=0xFE;
			      }
				  AVL_DVBS2_SendDiseqcCmmd(diseqc_cmd->aui1_cmd, diseqc_cmd->ui1_cmd_len);
				  for(j=0;j<diseqc_cmd->ui1_cmd_len;j++)
				  {
					mcDBG_MSG_L2(("0x%02x  ",diseqc_cmd->aui1_cmd[j]));
				  }
			      mcDELAY_MS(110);
			    }
				mcDBG_MSG_L2(("After set cmd,delay 110ms,delay>15ms\n"));
    		}
			
			if((diseqc_cmd->ui4_control_mask &DISEQC_CMD_CTRL_TONE_BURST_MASK) != 0)
			{//apply you code
				if(1==diseqc_cmd->ui1_tone_burst)
		    		{
			    		mcDBG_MSG_L2(("b_tone_burst 1 \n"));
						AVL_DVBS2_Select_Tone(1);
		    		}
				else if(0==diseqc_cmd->ui1_tone_burst)
				{
					mcDBG_MSG_L2(("b_tone_burst 0 \n"));
					AVL_DVBS2_Select_Tone(0);
				}
                else
                {
                     mcDBG_MSG_L2(("b_tone_burst off \n"));
                }
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set burst,delay 20ms,delay>15ms\n"));
		   	 }
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) != 0)
		 	{//apply you code
				if(diseqc_cmd->b_22k)
		    	{
		    		mcDBG_MSG_L2(("set 22k on.\n"));
					AVL_DVBS2_22K_ON();
		    	}
				else
				{
					mcDBG_MSG_L2(("set 22k off.\n"));
					AVL_DVBS2_22K_OFF();
				}	
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set 22k\n"));
		    }
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK) != 0)//d120712_Haibo:ODU channel change
			{
		                mcDBG_MSG_L2(("[%s]ODU_CHANNEL_CHANGE\n",__FUNCTION__));
				pPdCtxDVBS->fgUniCab=TRUE;
				x_memcpy(&(pPdCtxDVBS->t_UniCab_info),&(diseqc_cmd->t_unicable_info),sizeof(TUNER_UNICABLE_CC_T));
				DVBS_OduChannelChg(&(pPdCtxDVBS->t_UniCab_info));
			}
			else
			{
			    pPdCtxDVBS->fgUniCab=FALSE;//reset the flag
			}
#ifdef Debug_time
HAL_GetTime(&TimeEnd);
HAL_GetDeltaTime(&TimeDst,&TimeStart, &TimeEnd);
mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] time:%ld.\n",TimeDst.u4Micros));
#endif
			break;
	case TUNER_SET_TYPE_RESET_CMD:
		    diseqc_cmd = (DISEQC_BUS_CMD_T*) pvAttribute; 
			mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] stop 22k \n"));
			AVL_DVBS2_22K_OFF();
			g_channle_locked_flag=1;//Reset the lock flag to TRUE.This is very important.
			x_memcpy(&g_diseqc_cmd,diseqc_cmd,sizeof(DISEQC_BUS_CMD_T));//availink :add for desiqc control ,20110210 lmh
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_LNB_POWER_MASK) != 0 )
			{
			    switch(diseqc_cmd->ui1_lnb_power)
			       	{
			       	case LNB_POWER_OFF:
						mcDBG_MSG_L2(("set lnb power off\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_OFF);
						LNB_Control(LNB_POWER_0);
						nDiseqcReset_Flag=1;
						break;
					case LNB_POWER_13V_18V:
						mcDBG_MSG_L2(("set lnb power normal\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_NORMAL);
						break;
					case LNB_POWER_14V_19V:
						mcDBG_MSG_L2(("set lnb power plus\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_PLUS);
						break;
					case LNB_POWER_ON:
						mcDBG_MSG_L2(("set lnb power on\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_NORMAL);
						break;
				       default:
					   	mcDBG_MSG_L2(("set lnb power not support...\n"));
					   	break;
			       	}
				//mcDELAY_MS(20);
				//mcDBG_MSG_L2(("After DISEQC_CMD_CTRL_LNB_POWER_MASK,delay 20ms,delay>15ms\n"));		   
			}
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_POLARIZATION_MASK) != 0 )
			{//apply you code
				if(diseqc_cmd->ui1_polarity_13v_18v == 1)
				{
					mcDBG_MSG_L2(("set 18V\n"));
					//A8293_Control(LNB_POWER_18);
					LNB_Control(LNB_POWER_18);
				}
				else
				{
					mcDBG_MSG_L2(("set 13V\n"));
					//A8293_Control(LNB_POWER_13);
					LNB_Control(LNB_POWER_13);
				}
				if(nDiseqcReset_Flag==1)
				{
					nDiseqcReset_Flag=0;
					mcDBG_MSG_L2(("Delay  130ms  for Diseqc Reset..\n"));
					mcDELAY_MS(130);		
				}
				else
				{
					mcDELAY_MS(20);
					mcDBG_MSG_L2(("After set H/V,delay 20ms,delay>15ms\n"));
				}
				
	    		}
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)
			{//apply you code
				AVL_DVBS2_SendDiseqcCmmd(diseqc_cmd->aui1_cmd, diseqc_cmd->ui1_cmd_len);
				mcDELAY_MS(110);
				mcDBG_MSG_L2(("After set cmd,delay 110ms,delay>15ms\n"));
				for(i=0;i<diseqc_cmd->ui1_cmd_len;i++)
				{
					mcDBG_MSG_L2(("0x%02x  ",diseqc_cmd->aui1_cmd[i]));
				}
				mcDBG_MSG_L2(("\n"));
    		}
			
			if((diseqc_cmd->ui4_control_mask &DISEQC_CMD_CTRL_TONE_BURST_MASK) != 0)
			{//apply you code
				if(1==diseqc_cmd->ui1_tone_burst)
		    		{
			    		mcDBG_MSG_L2(("b_tone_burst 1 \n"));
						AVL_DVBS2_Select_Tone(1);
		    		}
				else if(0==diseqc_cmd->ui1_tone_burst)
				{
					mcDBG_MSG_L2(("b_tone_burst 0 \n"));
					AVL_DVBS2_Select_Tone(0);
				}
                else
                {
                     mcDBG_MSG_L2(("b_tone_burst off \n"));
                }
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set burst,delay 20ms,delay>15ms\n"));
		   	 }
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) != 0)
		 	{//apply you code
				if(diseqc_cmd->b_22k)
		    	{
		    		mcDBG_MSG_L2(("set 22k on.\n"));
					AVL_DVBS2_22K_ON();
		    	}
				else
				{
					mcDBG_MSG_L2(("set 22k off.\n"));
					AVL_DVBS2_22K_OFF();
				}	
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set 22k\n"));
		    }
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_UNICABLE_ODU_CH_CHANGE_MASK) != 0)//d120712_Haibo:ODU channel change
			{
		                mcDBG_MSG_L2(("[%s]ODU_CHANNEL_CHANGE\n",__FUNCTION__));
				pPdCtxDVBS->fgUniCab=TRUE;
				x_memcpy(&(pPdCtxDVBS->t_UniCab_info),&(diseqc_cmd->t_unicable_info),sizeof(TUNER_UNICABLE_CC_T));
				DVBS_OduChannelChg(&(pPdCtxDVBS->t_UniCab_info));
			}
			else
			{
			    pPdCtxDVBS->fgUniCab=FALSE;//reset the flag
			}
#ifdef Debug_time
HAL_GetTime(&TimeEnd);
HAL_GetDeltaTime(&TimeDst,&TimeStart, &TimeEnd);
mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] time:%ld.\n",TimeDst.u4Micros));
#endif
			break;	
	case TUNER_SET_TYPE_TS_TRISTATE:
		    ucData = *(UCHAR *)pvAttribute;  
			if (ucData == BOOL_FALSE) 
			{//apply you code
				mcDBG_MSG_L2(("[DVBS] SetTSPath : pass-through\n"));
				AVL_DVBS2_Enable_TS(); //open ts
				mcDBG_MSG_L2(("[DVBS]	Set TS output on  \n"));
			}
			
			if (ucData == BOOL_TRUE) 
			{//apply you code
				mcDBG_MSG_L2(("[DVBS] SetTSPath : tri-state\n"));
	              	AVL_DVBS2_Disable_TS();// close ts
				mcDBG_MSG_L2(("[DVBS]	Set TS output off  \n"));
			}
			break;
		
	case TUNER_SET_TYPE_TS_SP:
			ucData = *(UCHAR *)pvAttribute;  
			if (ucData == SP_PARALLEL) 
			{//apply you code
				 mcDBG_MSG_L2(("[DVBS] SetTSInterface : parallel\n"));
	        	}
			else if (ucData == SP_SERIAL) 
			{//apply you code
	        		mcDBG_MSG_L2(("[DVBS] SetTSInterface : serial\n"));
			}
	 		break;
	case TUNER_SET_TYPE_DEMOD_EARLY_BREAK_CTL:
			ucData = *(UCHAR *)pvAttribute; 
			vEnEarlyBreak(ucData);
			break;
    	default:
	        mcDBG_MSG_L2(("unknown: set Attribute\r\n"));
	        break;
    }

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return DRVAPI_TUNER_OK;
}
#ifdef fcADD_DIAG_INFO
static INT32 DVBS_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
	INT32   retSts = RMR_OK;
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return retSts;
}
static INT32 DVBS_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//Not used

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return 0;
}
#endif
//-------------------------------------------------------------------------
/**
*  This API  control  the i2c bypass to tuner. When zap from digital to ananlog,we need this api to bypass i2c to tuner. 
* Inputs:
* @param   ptTDSpecificCtx  Pointer to tuner driver context, there is no use for third party tuner.  
* @param    bSwitchOn        TRUE(bypass i2c to tuner on).
                             FALSE(bypass i2c to tuner off) 
* @Retval                    void
*/
//-------------------------------------------------------------------------
static void DVBS_TunerBypassI2C (PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)

{
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
    //TODO  Bypass i2c to tuner connected(bSwitchOn is TRUE) or disconnect(bSwitchOn is FALSE)
	
	//END
    mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}
#if 1
BOOL DVBS_TunerNotifyEnable(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    BOOL TunerNotifyEnable;
    if(do_blindscan == 1)
	    TunerNotifyEnable = FALSE;
    else
	    TunerNotifyEnable =TRUE;

    return TunerNotifyEnable;
}
#endif
//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
void PD_DVBS_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*		pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
	
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose 	= DVBS_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq 	= DVBS_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync 	= DVBS_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal 	= DVBS_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = DVBS_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = DVBS_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = DVBS_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = DVBS_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt 	= DVBS_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer 	= DVBS_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest 	= DVBS_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = DVBS_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = DVBS_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C 	= DVBS_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand 	= DVBS_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand 	= DVBS_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C 	= DVBS_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc 	= DVBS_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= DVBS_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= DVBS_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= DVBS_TunerBypassI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNotifyEnable = DVBS_TunerNotifyEnable;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd	= DVBS_TunerSetDiagCmd;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = DVBS_TunerGetDiagInfo;
#endif
}
//-------------------------------------------------------------------------
/**
 *  This API is driver entry point. It performs
 *  1. allocate and initialize resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDCtx            TunerHAL Context use for LTDIS API mapping
 *  @param   ptTunerCfgInfo     Tuner configuration information.
 *  @param   pptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
INT32 DVBS_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //create pdctx used in this file, you also can add any declaration you want in pdctx		 
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));
	
	//pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
	pPdCtxDVBS=&tPdCtxDVBS;
	if (pPdCtxDVBS == NULL)
	{
		mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
		return (DRVAPI_TUNER_ERROR);
	}
	x_memset(pPdCtxDVBS, 0, sizeof(tPdCtxDVBS));
	
	*pptTDSpecificCtx = pPdCtxDVBS;/*There may be a question here.................................*/
	
	pDVBSTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;/*Err:Porting Guiding has not TUNER_DEVICE_CTX_T defined. */
	mcDBG_MSG_L2(("pDVBSTunerDeviceCtx->tunerType=0x%02x\n",pDVBSTunerDeviceCtx->tunerType));
#if 0
	//create demod ctx
	psDemodCtx = DVBS_DemodCtxCreate();
	if (psDemodCtx == NULL)
	{
		mcDBG_MSG_ERR(("DEMOD_CTX_T malloc fail\n"));
		return (DRVAPI_TUNER_ERROR);
	}
#endif
    //Create a semaphore for low level demod PI access protection
	if (x_sema_create(&AutoScan_callback, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
	{
		mcDBG_MSG_ERR(("%s:  x_sema_create(AutoScan_callback) Fail!\n",__FUNCTION__));
		return (DRVAPI_TUNER_ERROR);
	}
	
	if (x_sema_create(&pPdCtxDVBS->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
	{
		mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtxDVBS->hHalLock) Fail!\n",__FUNCTION__));
		return (DRVAPI_TUNER_ERROR);
	}
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	//register LTDIS APIs to HAL
	PD_DVBS_Register_LTDIS_Fct(ptTDCtx);
	//TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR

//---------------------------------	Availink added
	Demod_GPIO_Reset();
	
	AVL_DVBS2_Demod_Init();
	
      AVL_DVBS2_LNB_Power_OnOff(0);
	  	
      pPdCtxDVBS->fgDisStatus = TRUE;///lmh add 2011-9-15
	  AVL_DVBS2_Disable_TS();//d120823_Haibo:Disable TS output after demod init
//-----------------------------------------------------------------------------

	
	//END
	if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{
		if (x_sema_create(&pPdCtxDVBS->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
		{
			mcDBG_MSG_ERR(("EN_DVBS_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
			return (DRVAPI_TUNER_ERROR);
		}

		pPdCtxDVBS->fgBreakMonitor = TRUE;//init
		pPdCtxDVBS->fgPIMoniStatus = FALSE; //init
		PD_StartMonitorEngine(pPdCtxDVBS);  //start monitor engine
	}
	 
	return (DRVAPI_TUNER_OK);
	
}


