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
#include "pd_dvbs_templ_if.h"
#include "x_gpio.h"
#include "vdr_PD_Def.h"
#include "x_rm.h"
#include "drvcust_if.h"

#define SCAN_RANGE_SINGLE_TIME     1 //scan range is 1M
#define VENDOR_DRIVER_VER            "2012-11-16\nVer1.1"
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

//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
static DVBS_PD_CTX_T *pPdCtxDVBS;
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
#define Debug_time
//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static  INT32 DVBS_AutoScanCallback(UINT8 signal,UINT8 polar,UINT32 freq,UINT32 sym,UINT8 fec);
EXTERN INT32 Tuner_Notify(DRV_COND_T e_nfy_cond,UINT32 u4Msg);

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

static void DVBS_OduChannelChg(TUNER_UNICABLE_CC_T *pUniCabinfo)
{
	mcDBG_MSG_L2(("[%s]\n",__FUNCTION__));
	mcDBG_MSG_L2(("Ft=%dMHz,Fo=%dMHz,Fu=%dMHz,UB=%d,Bank=%d\n",\
	pUniCabinfo->ui4_transponder_freq,pUniCabinfo->ui2_lnb_freq,pUniCabinfo->ui2_band_freq,pUniCabinfo->e_user_band,pUniCabinfo->e_bank));
	//S=TUNER_CODING_STEP_SIZE in u_tuner.h
    if(pUniCabinfo->ui4_transponder_freq==0)
   {
        mcDBG_MSG_L2(("Ft is 0MHz,do not do channel change\n"));
		return;
   }
	//TODO_Unicable:Please add your implemention to control SCIF
}
static void DVBS_OduPowerOff(DVBS_PD_CTX_T * pdCtx)
{
	mcDBG_MSG_L2(("[%s]UB=%d\n",__FUNCTION__,pdCtx->t_UniCab_info.e_user_band));
	//TODO_Unicable:Please add your implemention to power off  SCIF
}
static void DVBS_TunerDiscSCIF(DVBS_PD_CTX_T * pdCtx)
{
   //TODO_Unicable:Please add your implemention to ensure the specified user band of the SCIF is powered off.We need it to avoid the traffic conflict 
   //Here is just a simple example without any confirmation 
   DVBS_OduPowerOff(pdCtx);
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

static void Demod_GPIO_Reset(void)
{	

	UINT32 u4ResetGpionum;

	//INT32 fgSet = 0;
	if(DRVCUST_OptQuery(eDemodResetGpio,&u4ResetGpionum)!=0)
	{
	u4ResetGpionum=201;//d111212_Haibo:default is gpio 201
	}
	GPIO_SetOut(u4ResetGpionum, 0);
	mcDELAY_MS(50);
	//fgSet = 1;
	GPIO_SetOut(u4ResetGpionum,1);
	mcDELAY_MS(500);
	mcDBG_MSG_L2(("Reset demod  via %d\n",u4ResetGpionum));
}

#define LNB_POWER_13 0x1
#define LNB_POWER_18 0x2
#define LNB_POWER_0 0x0
#define LNB_POWER_USER_SET_OFF 0x1f
#define LNB_POWER_USER_SET_NORMAL 0x2f
#define LNB_POWER_USER_SET_PLUS 0x3f
static UINT8 user_set_flag=LNB_POWER_USER_SET_PLUS;
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
			//power off LWB
			break;
		case LNB_POWER_13:
			if(user_set_flag == LNB_POWER_USER_SET_NORMAL )
			{
   			//14V
			}
			else if(user_set_flag == LNB_POWER_USER_SET_PLUS)
			{
  			//15V
			}
			else
			{
				mcDBG_MSG_L2(("[LNB_Control] user flag is OFF!\n"));
			}
			break;	
		case LNB_POWER_18:
			if(user_set_flag == LNB_POWER_USER_SET_NORMAL )
			{
			  //18
			}
			else if(user_set_flag == LNB_POWER_USER_SET_PLUS)
			{
  			//19
			}
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

static void Vendor_ResendDiseqcCmmd(DISEQC_BUS_CMD_T *diseqc_cmd)
{
	mcDBG_MSG_L2(("[%s] stop 22k \n",__FUNCTION__));
	//22K OFF.demod driver implement
	//mcDELAY_MS(20);
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_POLARIZATION_MASK) != 0 )
		{//apply you code
			if(diseqc_cmd->ui1_polarity_13v_18v == 1)
			{
				mcDBG_MSG_L2(("set 18V\n"));
				LNB_Control(LNB_POWER_18);
			}
			else
			{
				mcDBG_MSG_L2((" set 13V\n"));
				LNB_Control(LNB_POWER_13);
			}
			mcDELAY_MS(20);
   		}

					
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_FULL_CMD_MASK) != 0)
		{//apply you code
			
			mcDELAY_MS(20);
			mcDBG_MSG_L2(("After set cmd,delay 20ms,delay>15ms\n"));
    		}
			
		if((diseqc_cmd->ui4_control_mask &DISEQC_CMD_CTRL_TONE_BURST_MASK) != 0)
		{//apply you code
			if(diseqc_cmd->b_tone_burst)
	    		{
		    		mcDBG_MSG_L2(("b_tone_burst 1 \n"));
				    //demod driver implement
	    		}
			else
			{
				mcDBG_MSG_L2(("b_tone_burst 0 \n"));
				 //demod driver implement
			}
			mcDELAY_MS(20);
	   	 }
			
		if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) != 0)
	 	{//apply you code
			if(diseqc_cmd->b_22k)
		    	{
		    		mcDBG_MSG_L2(("set 22k on.\n"));
				 //demod driver implement
		    	}
			else
			{
				mcDBG_MSG_L2(("set 22k off.\n"));
				//demod driver implement
			}	
			//mcDELAY_MS(20);
	    	}	
		
}

static VOID Vendor_BlindScan(void)
{
    UINT32 u4Freq=bs_start_freq;
    BOOL Lockflag=0;
    for(;u4Freq<=bs_stop_freq;u4Freq+=SCAN_RANGE_SINGLE_TIME)
	{
	 /*ToDo:Implement demod blind scan process
	 
	 */
	 if(Lockflag)//demod lock the signal		
	 {	
       DVBS_AutoScanCallback(1, 0, 0,0,0);	
	 }						
	 else										
	 {
      	DVBS_AutoScanCallback(0, 0, 0,0,0);
	 }
	 if(BlindScan_MidWare_Break())
	 {
			mcDBG_MSG_L2(("Break 3!\n"));
			 break;
	 }	
	}
		
     mcDBG_MSG_L2(("blind scan end.!\n"));		
	return ;
}
static void Vendor_UnicableBlindScan(void)
{
    UINT32 u4Freq=bs_start_freq;
    BOOL Lockflag=0;
	
    for(;u4Freq<=bs_stop_freq;u4Freq+=SCAN_RANGE_SINGLE_TIME)
	{
	 /*TODO_Unicable:Implement demod unicable blind scan process
	 
	 */
	 if(Lockflag)//demod lock the signal		
	 {	
       DVBS_AutoScanCallback(1, 0, 0,0,0);	
	 }						
	 else										
	 {
      	DVBS_AutoScanCallback(0, 0, 0,0,0);
	 }
	 if(BlindScan_MidWare_Break())
	 {
			mcDBG_MSG_L2(("Break 3!\n"));
			 break;
	 }	
	}	
     mcDBG_MSG_L2(("blind scan end.!\n"));	
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
		mcDBG_MSG_L2(("[DVBS_AutoScanCallback]  Next Band Request\n"));
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
			if(pPdCtxDVBS->fgUniCab ==FALSE)
			{
			  Vendor_BlindScan();
			}
			else
			{
			  Vendor_UnicableBlindScan();
			}
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
						Vendor_ResendDiseqcCmmd(&g_diseqc_cmd);
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
		x_thread_delay((UINT32)(500)); 
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
    mcDBG_MSG_L2(("[DVBS_TunerConnect] Enable TS Output!\n"));
	    //add demod driver implementation code here

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
	BOOL fgLock=FALSE;
	UINT32 freq=0;
	UINT32 symbolrate=0;	
	UINT16 uiLockStatus=0;
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
    //get conncetion mode
        
	e_conn_mode=Util_GetConnMode(pPdCtxDVBS,pt_tuner_info->e_conn_mode);

	if(!DVBS_ValidateMwPara(pt_tuner_info,e_conn_mode))
	{
       mcDBG_MSG_ERR(("invalid parameter!!\n"));
       return fgLock;
	}
	
//set break function
	pPdCtxDVBS->isBreak=_BreakFct;
//mutex lock
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
				   	bs_start_freq=pt_tuner_info->ui4_start_freq;
					bs_stop_freq=	pt_tuner_info->ui4_end_freq;	
					Current_channel.m_uiFrequency_kHz=1000*bs_start_freq;
					Current_channel.m_uiSymbolRate_Hz=0;
					Current_channel.m_fec=FEC_I_UNKNOWN;				
					do_blindscan=1;
				}
				break;	
		case CONN_MODE_UNICABLE_SURF://d120718_Haibo:When unicable,the ui4_freq is band freq	
				mcDBG_MSG_L2(("Unicable tuning.Band freq is %dMHz\n",freq));	        				
		case CONN_MODE_UNKNOWN://channel change
		case CONN_MODE_SURF://preset scan ,single tp scan
				{//call you channel change driver here
					        if(symbolrate == 0 )
				        	{
                               //demod auto symbol rate detect case
				        	}
	                  //demod driver implement
					  //lock demod to specified freq and check lock status
					  //please update uiLockStatus
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
        ui4_data_forMW = pt_tuner_info->ui4_data;
		g_channle_locked_flag=1;
		fgLock=1;
	}
	else
	{
		g_channle_locked_flag=0;
		fgLock=0;
	}

	//-------------------------------------------------------------------------------------

	if(pDVBSTunerDeviceCtx->u1MWIntf != cMTAL_IF && pDVBSTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{
	    if (!_BreakFct(pvArg))
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
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
   if(pPdCtxDVBS->fgUniCab)//d120721_Haibo:power off SCIF  and confirm it is done indeed
	{  
	   DVBS_TunerDiscSCIF(pPdCtxDVBS);
	}
	//TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered
	pPdCtxDVBS->fgDisStatus = TRUE;
	
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
	  //return demod locked frequency(MHz) and symbol rare
		_pSignal->Frequency  = 0;
		_pSignal->SymbolRate =0;//KS/s
	}
	else
	{
		mcDBG_MSG_L2(("[DVBS_TunerGetSignal] _pSignal is Err .\n"));
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
	//update it as u1Ret
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
	//ToDo:demod driver implemented
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return i2Ret;
}

static UINT32 DVBS_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	UINT32 u4_ret=100000;

	if(do_blindscan == 1)
	{
		mcDBG_MSG_L2(("[%s] doing blindscan.\n",__FUNCTION__));
		return 0;
	}		
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));
	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-9

	//END
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return u4_ret;//modify return value
}
static UINT16 DVBS_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
	UINT16 u2_ret=0;
	if(do_blindscan == 1)
	{
		mcDBG_MSG_L2(("[%s] doing blindscan.\n",__FUNCTION__));
		return 0;
	}	
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	 
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);
	//TODO Get SNR(signal-noise ratio) from demod driver
	//update it as u2_ret
	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);

	return u2_ret;//modify return value
}

static void DVBS_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	 
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
}

static char *DVBS_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

	mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->hHalLock);
	return (char *)(VENDOR_DRIVER_VER);
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
    if (i4Argc < 1)
    {
	    mcDBG_MSG_L2(("\t a [Freq(MHz)] [Sym_Rate(KS/s)]:  show signal/ts/tps status\n"));              
	    mcDBG_MSG_L2(("\t b :  start to do blind scan\n"));   
        mcDBG_MSG_L2(("\t c :  continue to do blind scan\n"));
        mcDBG_MSG_L2(("\t d :  cancel blind scan(then type c)\n"));		
        mcDBG_MSG_L2(("\t s: show signal/ts/tps status\n"));              
    }	
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);
        
    switch (ucCmdId)
    {
    case 'b'://blind scan
       	if(pPdCtxDVBS->fgBreakMonitor)
		{
		 pPdCtxDVBS->fgBreakMonitor=FALSE;
		mcSEMA_UNLOCK_MTFE(pPdCtxDVBS->t_escape_mon_suspend);
		}
		do_blindscan=1;
		mcDBG_MSG_L2(("do blind scan.\n"));
        break;	
    case 'c':
		mcDBG_MSG_L2(("continue to do blind scan...\n"));
		x_sema_unlock(AutoScan_callback);
        break;
    case 'd':
		 pPdCtxDVBS->fgBreakMonitor=TRUE;
		 mcDBG_MSG_L2(("cancel blind scan\n"));
		break;	
    default:
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
	UCHAR SerOrParal=0,Clk_Polar=0,Ts_Tri=0;
	//demod get ts output format.update it as SerOrParal,Clk_Polar,Ts_Tri
	
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
	fgTsFmt.u4TsClockInKHz =0;
	
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
	
	if(pzAttribute!=NULL)
	{
		pAttri0 = (TUNER_SAT_DIG_TUNE_INFO_T*)pzAttribute;
	}
	else
	{
		mcDBG_MSG_L2(("[DVBS_TunerGetAttribute] pzAttribute IS Err\n"));
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
				//signal level.update it as i4Temp
		      		x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
	    		}
			break;
		case TUNER_GET_TYPE_SAT_DIG:
			{	/*maybe return Last_program_channel*/
			        //mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_SAT_DIG:freq:%d\r\n",Current_channel.m_uiFrequency_kHz));
                    //update channel information.
			        pAttri0->e_fec_inner = Current_channel.m_fec;
			        pAttri0->ui4_freq = Current_channel.m_uiFrequency_kHz/1000;
				 pAttri0->ui4_sym_rate = Current_channel.m_uiSymbolRate_Hz/1000;
				 pAttri0->ui4_data = ui4_data_forMW;
				 pAttri0->e_pol = E_POL; 
                 mcDBG_MSG_L2(("[DVBS_TunerGetAttribute]  ui4_freq:0x%x !\n",pAttri0->ui4_freq));
			}
			break;
		case TUNER_GET_TYPE_SQI:
			{
			       //mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_BER\r\n"));
			        *pzAttributeLen = sizeof(INT32);
                     //TODO Get BER from demod driver.Note the unit is 1e-5.update it as i4Temp
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
	UCHAR	ucData=0;// = *(UCHAR *)pvAttribute;  
	DISEQC_BUS_CMD_T *diseqc_cmd;// = (DISEQC_BUS_CMD_T*) pvAttribute; 
      static UCHAR nDiseqcReset_Flag=1;
#ifdef Debug_time	
	HAL_TIME_T TimeStart,TimeEnd,TimeDst;
       HAL_GetTime(&TimeStart);
#endif	

	if(pvAttribute!=NULL)
	{
		ucData = *(UCHAR *)pvAttribute;  
		diseqc_cmd = (DISEQC_BUS_CMD_T*) pvAttribute; 
	}
	else if( e_set_type == TUNER_SET_TYPE_SCAN_CONTINUE)
	{
				mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
				x_sema_unlock(AutoScan_callback);
				return DRVAPI_TUNER_OK;
	}
	else
	{
		mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] pvAttribute is NULL & set type can not support...\n"));
		return DRVAPI_TUNER_OK;	
	}
	
	mcSEMA_LOCK_MTFE(pPdCtxDVBS->hHalLock);

    //add driver implementation code here

	switch (e_set_type)
	{
		/*
		case TUNER_SET_TYPE_SCAN_CONTINUE:
			{
				mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] MonitorProcess  continue...\n"));
				x_sema_unlock(AutoScan_callback);
			}
			break;*/
		case TUNER_SET_TYPE_DISEQC_CMD:
			mcDBG_MSG_L2(("[DVBS_TunerSetAttribute] stop 22k \n"));
			//22k off.demod implement
			g_channle_locked_flag=1;//Reset the lock flag to TRUE.This is very important.
			x_memcpy(&g_diseqc_cmd,diseqc_cmd,sizeof(DISEQC_BUS_CMD_T));//availink :add for desiqc control ,20110210 lmh
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_LNB_POWER_MASK) != 0 )
			{
			       switch(diseqc_cmd->ui1_lnb_power)
			       	{
			       	case 2:
						mcDBG_MSG_L2(("set lnb power off\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_OFF);
						LNB_Control(LNB_POWER_0);
						nDiseqcReset_Flag=1;
						break;
					case 0:
						mcDBG_MSG_L2(("set lnb power normal\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_NORMAL);
						break;
					case 1:
						mcDBG_MSG_L2(("set lnb power plus\n"));
						LNB_POWER_USER_SET(LNB_POWER_USER_SET_PLUS);
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
					LNB_Control(LNB_POWER_18);
				}
				else
				{
					mcDBG_MSG_L2(("set 13V\n"));
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
				//send disc command.demod should implement
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set cmd,delay 20ms,delay>15ms\n"));
				for(i=0;i<diseqc_cmd->ui1_cmd_len;i++)
				{
					mcDBG_MSG_L2(("0x%02x  ",diseqc_cmd->aui1_cmd[i]));
				}
				mcDBG_MSG_L2(("\n"));
    		}
			
			if((diseqc_cmd->ui4_control_mask &DISEQC_CMD_CTRL_TONE_BURST_MASK) != 0)
			{//apply you code
				if(diseqc_cmd->b_tone_burst)
		    		{
			    		mcDBG_MSG_L2(("b_tone_burst 1 \n"));
						//demod implement
		    		}
				else
				{
					mcDBG_MSG_L2(("b_tone_burst 0 \n"));
					//demod implement
				}
				mcDELAY_MS(20);
				mcDBG_MSG_L2(("After set burst,delay 20ms,delay>15ms\n"));
		   	 }
			
			if((diseqc_cmd->ui4_control_mask & DISEQC_CMD_CTRL_22K_MASK) != 0)
		 	{//apply you code
				if(diseqc_cmd->b_22k)
		    	{
		    		mcDBG_MSG_L2(("set 22k on.\n"));
					//demod implement
		    	}
				else
				{
					mcDBG_MSG_L2(("set 22k off.\n"));
					//demod implement
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
			
			if (ucData == BOOL_FALSE) 
			{//apply you code
				mcDBG_MSG_L2(("[DVBS] SetTSPath : pass-through\n"));
				//open ts output.demod implement
				mcDBG_MSG_L2(("[DVBS]	Set TS output on  \n"));
			}
			
			if (ucData == BOOL_TRUE) 
			{//apply you code
				mcDBG_MSG_L2(("[DVBS] SetTSPath : tri-state\n"));
	             // close ts output.demod implement
				mcDBG_MSG_L2(("[DVBS]	Set TS output off  \n"));
			}
			break;
		
	case TUNER_SET_TYPE_TS_SP:
			if (ucData == SP_PARALLEL) 
			{//apply you code
				 mcDBG_MSG_L2(("[DVBS] SetTSInterface : parallel\n"));
	        	}
			else if (ucData == SP_SERIAL) 
			{//apply you code
	        		mcDBG_MSG_L2(("[DVBS] SetTSInterface : serial\n"));
			}
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

	pPdCtxDVBS = (DVBS_PD_CTX_T *) x_mem_calloc(1, sizeof(DVBS_PD_CTX_T));
	if (pPdCtxDVBS == NULL)
	{
		mcDBG_MSG_ERR(("DVB_PD_CTX_T malloc fail\n"));
		return (DRVAPI_TUNER_ERROR);
	}
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
	//Regisiter tuner driver
	ITunerRegistration(pDVBSTunerDeviceCtx->tunerType);
	ITuner_Init(ITunerGetCtx());
	
	//TODO Here do demod init.If demod init fail,return DRVAPI_TUNER_ERROR
	Demod_GPIO_Reset();
	//TODO 
	//init demod
    //power off LNB
	//demod standby 
	  	
      pPdCtxDVBS->fgDisStatus = TRUE;
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


