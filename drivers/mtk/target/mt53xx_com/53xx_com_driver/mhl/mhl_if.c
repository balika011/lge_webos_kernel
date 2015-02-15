#include "hw_vdoin.h"
#include "sv_const.h"
#include "source_table.h"
#include "drv_video.h"
#include "drv_hdmi.h"
#include "x_printf.h"
#include "sif_if.h"
#include "hw_hdmi.h"
#include "drv_hdmi.h"
#include "x_timer.h"
#include "util.h"
#include "x_gpio.h"
#include "x_debug.h"
#include "eeprom_if.h"
#include "hw_pdwnc.h"
#include "ir_if.h"

#ifdef SYS_MHL_SUPPORT
#include "mhl_if.h"
#if defined (CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#ifndef CC_MHL_HDMI_SWITCH
#ifdef CC_MT5882
#include "drv_cbus_cpi_if.h"
#else
#include "drv_cbus_if.h"
#endif
extern UINT8  _bIsMhlDevice;
#endif
#endif
// PCB configuration
#define MHL_PORT_NUM 1
#ifdef CC_MHL_HDMI_SWITCH
#define MHL_PORT_ID 2
#define MHL_SWITCH_SEQUENCE_OFF 0
#define MHL_SWITCH_SEQUENCE_ON 1
#else
#define MHL_PORT_ID 1
#endif
#define MHL_GPIO_RST 208
#define MHL_GPIO_WAKEUP 204
#define MHL_GPIO_INT 0xffffffff
#define SIL_SIF_BUS 4 // PDWNC1
#define SIL_SIF_RATE 0x100
#define SIL_SIF_ADR_NUM 1
#ifdef CC_MHL_HDMI_SWITCH
#ifndef CC_MHL_INT
static HANDLE_T _HDMISwitchMainTimer;
#endif
#endif
HANDLE_T _MHLRapPollingTimer;
HANDLE_T _MHLRapPollingStartTimer;
HANDLE_T _MHLRapkTimer;
#define SIL_TIMER_PERIOD  10     // ms
#define SIL_LOOP_COUNTER  (100/SIL_TIMER_PERIOD)     // 100 ms
#if defined(CC_MHL_HDMI_SWITCH)
#define SIL_OCP_FREE_CNT  (500/SIL_TIMER_PERIOD)    // 500ms
#define SIL_OCP_DELAY_CNT  (30/SIL_TIMER_PERIOD)    // 10 ms
#else
#define SIL_OCP_FREE_CNT  (490/SIL_TIMER_PERIOD)    // 500ms
#define SIL_OCP_DELAY_CNT  (10/SIL_TIMER_PERIOD)    // 10 ms
#endif
#define SIL_OCP_TIMEOUT_CNT  (1000/SIL_TIMER_PERIOD)    // 2s

#define SIL_INIT_TIMEOUT (20000/SIL_TIMER_PERIOD)   // 20 seconds
#define SIL_REINIT_CNT (1000/SIL_TIMER_PERIOD)   // 1 second

#define SIL_LOOP_MONITOR_CNT  (10000/SIL_TIMER_PERIOD)  // 1s


#if 0  // for test only
#define SIL_INI_DELAY (700/SIL_TIMER_PERIOD)  // 600ms + 100 ms VBUS rst
#define SIL_INI_1ST_VBUS_RST (350/SIL_TIMER_PERIOD)
#define SIL_INI_VBUS_RST_FREE  (SIL_INI_DELAY > SIL_INI_1ST_VBUS_RST ?  (SIL_INI_DELAY - SIL_INI_1ST_VBUS_RST ) : 0)
#else
#define SIL_INI_DELAY (0)
#define SIL_INI_1ST_VBUS_RST (0)
#define SIL_INI_VBUS_RST_FREE  (0)
#endif
#ifdef CC_MHL_HDMI_SWITCH
static HANDLE_T _hHDMIswitchThread;
static HANDLE_T _hHDMIswitchSema = NULL_HANDLE;
#endif
HAL_TIME_T  timerStartTime;
HAL_TIME_T  currentTime;
HAL_TIME_T  deltaTime;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
//UINT32 u4MhlArg;
UINT32 u4MhlMscbArg;//MSC Callback arg
UINT32 u4MhlSpcbArg;//Scratchpad Callback arg
UINT32 u4MhlHpcbArg;//HotPlug Callback arg
UINT32 u4MhlBmcbArg;//CBUS Mode Callback arg
UINT32 u4MhlOCcbArg;//Over current Callback arg

static x_cbus_drv_nfy_msc_subcommand rCbusMscHandler;
static x_cbus_drv_nfy_scratchpad_data rCbusScratchpadHandler;
static x_cbus_drv_nfy_hot_plug rCbusHotPlugHandler;
static x_cbus_drv_nfy_cbus_mode rCbusModeHandler;
static x_cbus_drv_nfy_vbus_overcurrent rCbusOverCurrentHandler;
#endif
static x_cbus_drv_nfy_data_arrival rCbusRxCbHandler;
static x_cbus_drv_nfy_tx_result rCbusTxCbHandler;
static x_cbus_drv_nfy_rapk rCbusRapkCbHandler;
// ocp
x_vbus_drv_nfy_ocp_status rMhlOcpCbHandler;
#ifdef CC_MHL_HDMI_SWITCH
static UINT8 u1MhlInit = 0;
#endif
UINT32 u2RapPollingCnt=0;
UINT32 u2StartpollingCnt=0;
UINT8  u1wait5sensFlag=0;
BOOL _IsReceiveOsdName = FALSE;
UINT8 u1WaitRapKFlag=0;
UINT8 IsReiceiveContentOnFlag=0;
UINT8 IsReiceiveContentOffFlag=0;
UINT8 _bNoAckCnt;

UINT8 u1SourceStatus = MHL_RAP_NOERROR;
UINT8 u1RapOnOff = 1;

extern UINT8 buf1[16];

#define KEY_SELECT 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_MENU 9
#define KEY_EXIT 0xd

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)

static const RCP_KEY_CODE_T _at_rcp_key_code[] = {        
        {KEY_SELECT, BTN_SELECT},
        
        {KEY_UP, BTN_CURSOR_UP},
        
        {KEY_DOWN, BTN_CURSOR_DOWN},
        
        {KEY_LEFT, BTN_CURSOR_LEFT},
        
        {KEY_RIGHT, BTN_CURSOR_RIGHT},
        
        {KEY_MENU, BTN_MENU},
        
        {KEY_EXIT, BTN_EXIT},
        };
#endif

void vMhlSetPanelPower(UINT8 val);
UINT8 u1MhlGetPanelPower(void);
void vMhlSetRst(void);
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
DRV_MHL_NTF_CTRL_T drvMhlNtfCtrl = {TRUE,TRUE};
BOOL fgStandbyPowerCharge = TRUE;
BOOL drvMhlNtfEnable(void)
{
	return drvMhlNtfCtrl.enAbleAllNtf?1:0;
}
BOOL drvMhlNtfRapEnable(void)
{
	return drvMhlNtfCtrl.rapNeedNtf?1:0;
}
BOOL drvMhlStandbyNeedCharge(void)
{
	return fgStandbyPowerCharge;
}
#endif

#ifdef CC_MHL_HDMI_SWITCH

// SIF Function
BOOL Sif_WriteBytes(UINT8 DeviceID,UINT8 Offset,UINT8 *pData,UINT8 ByteCount)
{
	
	INT32 i4ReturnValue;

	i4ReturnValue = SIF_X_Write(SIL_SIF_BUS, SIL_SIF_RATE, DeviceID, SIL_SIF_ADR_NUM ,Offset, pData, ByteCount);

	if (i4ReturnValue > 0)
	{
		return SIL_OK;
	}
	else
	{
	//printf("Sil1292 write fail\n");		   
		return SIL_FAIL;
	}
}

BOOL Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount)
{
    INT32 i4ReturnValue;
    i4ReturnValue = SIF_X_Read(SIL_SIF_BUS,SIL_SIF_RATE,DeviceID,SIL_SIF_ADR_NUM, Offset, pData, ByteCount);

    if (i4ReturnValue>0)
    {
   	  return SIL_OK;
    }
    else
    {   		
    	  //printf("Sil1292 read fail\n");
	  return SIL_FAIL;
    }
}

UINT32 Timer_Get(void)
{
	UINT32 timer;
	HAL_TIME_T CurrentTime;
	HAL_GetTime( &CurrentTime);

	timer = CurrentTime.u4Seconds *1000 + currentTime.u4Micros;
	return timer;
}

// timer
BOOL Timer_Set(UINT8 Index, UINT32 MilliSecond)
{

	HAL_GetTime( &timerStartTime);

  	return (SIL_OK);
}

UINT32 u4Timer_Get(UINT8 Index)
{
  	return (((1000000 * deltaTime.u4Seconds) + deltaTime.u4Micros));
}

BOOL Timer_Expired(UINT8 Index)
{
	HAL_GetTime( &currentTime );  
	HAL_GetDeltaTime( &deltaTime, &timerStartTime, &currentTime );

    /* Return the difference in milliseconds.   */
    //elapsedMs = (UINT32)((1000000 * deltaTime.u4Seconds) + deltaTime.u4Micros )> (timeout * 1000)); 
	if(((1000000 * deltaTime.u4Seconds) + deltaTime.u4Micros))
		return SIL_OK;
	else
   		return SIL_FAIL;
}

void Delay1ms(UINT32 ms)
{
  vUtDelay1ms(ms);
}

void Delay1us(UINT32 us)
{
  vUtDelay2us((us > 1 ? us>>1 : us));
}
#endif

#if  defined(CC_MHL_HDMI_SWITCH)
// hardwaare reset
BOOL ChipRst(UINT8 Polarity, UINT32 MicroSecond)
{
  INT32 val;
  val = (INT32)!Polarity;
  GPIO_Output(MHL_GPIO_RST, &val);
  Delay1ms(MicroSecond);
  val = (INT32)Polarity;
  GPIO_Output(MHL_GPIO_RST, &val);
  
  return (SIL_OK);
}

void vReset_High(void)
{
   INT32 val=1;
   GPIO_Output(MHL_GPIO_RST, &val);
}

void vReset_Low(void)
{
   INT32 val=0;
   GPIO_Output(MHL_GPIO_RST, &val);
}

#elif  defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL MhlRst(UINT8 Polarity, UINT32 MicroSecond)
{
 
  return (SIL_OK);
}
#endif


#if defined(CC_MHL_HDMI_SWITCH)
UINT8 bMHLDeviceConnect=0;
void NTF_CBUS_Connected(void)
{
	bMHLDeviceConnect=1;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	printf("______connected______\n");
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 bMHLDeviceConnect=0;
void Mhl_NTF_CBUS_Connected(void)
{
	bMHLDeviceConnect=1;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	//printf("______connected______\n");
}
#endif

#if defined(CC_MHL_HDMI_SWITCH)
void NTF_CBUS_Disconnected(void)
{
	bMHLDeviceConnect=0;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	printf("______disconnected______\n");
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
void Mhl_NTF_CBUS_Disconnected(void)
{
	bMHLDeviceConnect=0;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	//printf("______disconnected______\n");
}

#endif

void NTF_3D_Communicated(void)
{
	
}


#if 0
UINT8 bMHLDeviceState(UINT8 port)
{
	if(_bIsMhlDeviceConnect  ==1)
	{
		return 1;
	}
	
	if((u1MhlGetCableStatus(1) ==0) || (_bIsMhlDeviceConnect == 0))
	{
		return 0;
	}

	return 0;
}
#endif

#ifdef CC_MHL_HDMI_SWITCH
UINT8 _IsMhlDevice(void)
{
   return fgSil_NTF_CBUS_STATUS();//(((rSilStatus.MhlHdmiDevice & 0x11) == 0x11) ? 1 : 0);
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 _IsMhlDevice(void)
{
   return _bIsMhlDevice;
   //return _bIsMhlDeviceConnect;
   //return 0;
}
#endif
static UINT8 u1OcpFlag[MHL_PORT_NUM] = {0};
static UINT8 u1OcpTimeout[MHL_PORT_NUM] = {0};
static UINT8 u1OcpReleaseCnt[MHL_PORT_NUM] = {0};
static UINT8 u1OcpStatusCurrent[MHL_PORT_NUM] = {0xaa};

#if 1//def CC_SII1292
static UINT8 u1OcpDebunCnt[MHL_PORT_NUM] = {0};
static UINT8 u1OcpStatusOld[MHL_PORT_NUM] = {0xaa};
//static UINT8 u1MHLPlugCurrent[MHL_PORT_NUM] = {0xaa};
//static UINT8 u1MHLPlugOld[MHL_PORT_NUM] = {0xaa};
static UINT8 u1TimeOutFlag=0;
static UINT8 u1VbusFlag =0;//0,disable,1,enable

static MHL_OC_STATUS mhl_oc_status;
void vOcpHandler(void)
{
    UINT8 i = 0;
    UINT8 OcpStatus;
	OcpStatus = (UINT8)Mhl_GetOCPStatus(1);
    for (i = 0 ; i < MHL_PORT_NUM ; i++) {
    	
    	 UNUSED(u1OcpDebunCnt[i]);
		 Mhl_NFYOCPStatus(1,OcpStatus);

        if (u1OcpStatusCurrent[i] != u1OcpStatusOld[i])
		{
#if 1
                u1OcpStatusOld[i] = u1OcpStatusCurrent[i];
                // turn Vbus off
                if (u1OcpReleaseCnt[i] == 0) 
				{
					
					 MhlSetVBus(u1OcpStatusOld[i] ? 0:1);
                     printf("####6,vbus off,%d..................\n",(u1OcpStatusOld[i] ? 0:1));
                     u1VbusFlag=0;
                     mhl_oc_status.MhlOCStatus = u1OcpStatusOld[i] ? 1:0;
					 mhl_oc_status.MhlDeviceConnect = 0;//bMHLDeviceState(1)? 1:0;not used for soem
					 u1OcpFlag[i] = (u1OcpStatusOld[i] ? 1:0);
					// set OCP timeout
		            if (u1OcpFlag[i] == 1) 
					{
		            	u1OcpTimeout[i] = SIL_OCP_TIMEOUT_CNT;
						printf("#####5,u1OcpTimeout=%d............\n",u1OcpTimeout[i]);
						u1TimeOutFlag =0;
		            } 
					else 
		            {
		            	u1OcpTimeout[i] = 0;
		            }

                    if (rMhlOcpCbHandler) 
					{
                        // notify to ap
                        rMhlOcpCbHandler(i, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
                        printf("#####3:driver notify,oc status:%d,OCP Flag=%d..................\n",u1OcpStatusOld[i],u1OcpFlag[i]);
                        
                    }
				
				#ifdef SYS_ANDROID_TV_TIF_SUPPORT
					vCbus_NotifyOverCurrentResult(1,mhl_oc_status.MhlOCStatus,(void *)u4MhlOCcbArg);
				#endif
                }  
#else
            // de-bouncing
            if (u1OcpStatusCurrent[i] == fgSil_GetOCPStatus(i)) {
            	  u1OcpDebunCnt[i]++;
            } else if (u1OcpDebunCnt[i] ) {
            	  u1OcpDebunCnt[i]--;
            }
            
            // 
            if ( u1OcpDebunCnt[i] >= 2) {
            	  u1OcpStatusOld[i] = u1OcpStatusCurrent[i];
                if (rMhlOcpCbHandler) {
                    // notify to ap
                    rMhlOcpCbHandler(i, u1OcpStatusOld[i] ? 0:1);
                }
                // turn Vbus on/off
                fgSil_SetVBus(u1OcpStatusOld[i] ? 1:0);
            }
#endif            
     }
	  
        // turn Vbus off
    if (u1OcpReleaseCnt[i]) 
	{
//            u1OcpReleaseCnt[i]--;
        if (--u1OcpReleaseCnt[i] == 0) 
		{

			MhlSetVBus(u1OcpStatusOld[i] ? 0:1);
			u1VbusFlag=1;
			printf("####7,vbus off,%d..................\n",(u1OcpStatusOld[i] ? 0:1));
			printf("#####2,u1VbusFlag=%d..................\n",u1VbusFlag);
        }
    }

     // ocp timeout count
     if (u1OcpTimeout[i])
	 {  	
     	u1OcpTimeout[i]--;
		if(u1OcpTimeout[i] == 0)
		{
			printf("#####4,u1TimeOutFlag=%d..................\n",u1TimeOutFlag);
			u1TimeOutFlag =1;
		}
    }
	 
	if((u1TimeOutFlag == 1) && (u1VbusFlag == 1))
	{
		
#ifdef CBUS_OC_ADIN_USING
		u1OcpFlag[i] = (OcpStatus ? 1:0);
#else
		u1OcpFlag[i] = (u1CbusGetOcpStatus(1) ? 1:0);
#endif
		u1VbusFlag =0;
        u1TimeOutFlag =0;
#ifdef CBUS_OC_ADIN_USING
		mhl_oc_status.MhlOCStatus = OcpStatus ? 1:0;
#else
		mhl_oc_status.MhlOCStatus = u1CbusGetOcpStatus(1) ? 1:0;
#endif
		mhl_oc_status.MhlDeviceConnect = 0;//bMHLDeviceState(1)? 1:0;
		
	#ifdef SYS_ANDROID_TV_TIF_SUPPORT
			vCbus_NotifyOverCurrentResult(1,mhl_oc_status.MhlOCStatus,(void *)u4MhlOCcbArg);
	#endif
		if (rMhlOcpCbHandler)
		{
			// notify to ap
			rMhlOcpCbHandler(i, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
			printf("#####1:driver notify,oc status:%d,OCP Flag=%d..................\n",u1OcpStatusOld[i],u1OcpFlag[i]);
		}
	}


    }
    
}
#endif


UINT8 uIsHaveAckFlag=0;
UINT8 uRapkFlag=0;
UINT32 _u4DevieAdapterID;
UINT8 u1RapPollingFlag=0;
extern UINT8 buf1[16];

void vMHLRapkTimer(HANDLE_T hHandle,VOID* pTag)
{
	//no ack
	UNUSED(hHandle);
    UNUSED(pTag);
	if(u1RapPollingFlag ==1)
	{
		u1RapPollingFlag =0;
	}
	printf("___time out___\n");
	//x_timer_stop(_MHLRapkTimer);
	if(rCbusRapkCbHandler)
	{
	   rCbusRapkCbHandler(0xf);
	}
}

static UINT8 u1POLLINGCnt = 0;
UINT8 u1_RapPolling;
void vMHLRapPollingTimer(HANDLE_T hHandle,VOID* pTag)
{
   UNUSED(hHandle);
   UNUSED(pTag);
  
   u1POLLINGCnt ++;
   if(u1POLLINGCnt >= 15)
   {
       u1POLLINGCnt =0;
  // printf("___vIsSonyMHLDevice=%d,u1RapOnOf=%d,bMHLDeviceState(1)=%d_______\n",vIsSonyMHLDevice(),u1RapOnOff,bMHLDeviceState(1));
//   if(vIsSonyMHLDevice()&&(u1RapOnOff ==1) &&(bMHLDeviceState(1)))//when bravia sync control is on,and cbus connect,send rap polling to sony device
   {
		if(x_timer_create(&_MHLRapkTimer) != OSR_OK)
		{
		   //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   return;
		}

			//Mhl_RapKeys(0);//rap polling
			u1RapPollingFlag =1;
			u1_RapPolling =1;
			if(x_timer_start(_MHLRapkTimer, 1000, X_TIMER_FLAG_ONCE,vMHLRapkTimer, NULL) != OSR_OK)
		{
		   printf("Start HDMI switch timer fail !\n");
		   return;
		}
				
	}
	  #if 0
		else
		{
		    
			x_memset(buf1, 0, sizeof(buf1));
			_IsReceiveOsdName=FALSE;
			_bNoAckCnt=0;

			u1ClearMHLDeviceAdapterID();
			//printf("[mhl]no mhl cable connect\n");
		}
	 #endif
   }
}

void vMHLRapPollingStartTimer(HANDLE_T hHandle,VOID* pTag)
{
    UNUSED(hHandle);
    UNUSED(pTag);
	if(x_timer_create(&_MHLRapkTimer) != OSR_OK)
	{
	   //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
	   printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
	   return;
	}

	//Mhl_RapKeys(0);//rap polling
	u1_RapPolling =1;
	u1RapPollingFlag =1;
	
	if(x_timer_start(_MHLRapkTimer, 500, X_TIMER_FLAG_ONCE,vMHLRapkTimer, NULL) != OSR_OK)
	{
	   printf("Start HDMI switch timer fail !\n");
	   return;
	}

	if (x_timer_create(&_MHLRapPollingTimer) != OSR_OK)
	{
	   //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
	   printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
	   return;
	}
	u1POLLINGCnt=0;
	if (x_timer_start( _MHLRapPollingTimer, 1000, X_TIMER_FLAG_REPEAT, \
		   vMHLRapPollingTimer, NULL) != OSR_OK)
	{
	   printf("Start HDMI switch timer fail !\n");
	   return;
	}
}
#ifdef CC_MHL_HDMI_SWITCH
HAL_TIME_T  timerStartTime;
HAL_TIME_T  currentTime;
HAL_TIME_T  deltaTime;

void vHDMISwitchMainLoopTimer(HANDLE_T hHandle, VOID* pTag)
{
    UNUSED(hHandle);
    UNUSED(pTag);

    /* When called, we release the HDMI semaphore to allow  */
    /* the _HDMI_SwitchThread to run.                       */

   x_sema_unlock(_hHDMIswitchSema);
}                         

void vIntEn(INT32 En)
{
	GPIO_Intrq(MHL_GPIO_INT,&En);
}
void vMhlIrq(INT32 i4Gpio, BOOL fgStatus)
{
   // UNUSED(i4Gpio);
    INT32 IntEnbable=0;
    UNUSED(fgStatus);
	VERIFY(i4Gpio == MHL_GPIO_INT);
	VERIFY(x_sema_unlock(_hHDMIswitchSema) == OSR_OK);
	GPIO_Intrq(i4Gpio,&IntEnbable);
}

static UINT32 u4LoopCnt = 0;    
static UINT32 u4InitialDelay = 0;   
extern void vMhl_MainLoop(void);
extern void vHDMIMHL_Init(void);
#ifdef SYS_MHL_SUPPORT
#if defined(CC_MHL_HDMI_SWITCH)
//static UINT8 mhlinitflg = 0;
extern UINT8 u1mhlswitchthrd;
static void vSiiOcpDetect(void)
{
    UINT8 i = 0;
	fgSil_GetOCPStatus(0);
    
    for (i = 0 ; i < MHL_PORT_NUM ; i++) {
    	
    	 UNUSED(u1OcpDebunCnt[i]);
    	    
        if (u1OcpStatusCurrent[i] != u1OcpStatusOld[i]) {
                u1OcpStatusOld[i] = u1OcpStatusCurrent[i];
                // turn Vbus off
                if (u1OcpReleaseCnt[i] == 0) {
                    fgSil_SetVBus(u1OcpStatusOld[i] ? 1:0);
                     //printf("vbus off..................\n");
                 mhl_oc_status.MhlOCStatus = u1OcpStatusOld[i] ? 0:1;
				 mhl_oc_status.MhlDeviceConnect = 0;//bMHLDeviceState(1)? 1:0;not used for soem
                    if (rMhlOcpCbHandler) {
                        // notify to ap
                        rMhlOcpCbHandler(i,mhl_oc_status);
                        u1OcpFlag[i] = (u1OcpStatusOld[i] ? 0:1);
                    }

                }  
        }

        // turn Vbus off
        if (u1OcpReleaseCnt[i]) {
//            u1OcpReleaseCnt[i]--;
            if (--u1OcpReleaseCnt[i] == 0) {
                fgSil_SetVBus(u1OcpStatusOld[i] ? 1:0);
                //printf("vbus ctrl..................%d\n",u1OcpStatusOld[i]);
				mhl_oc_status.MhlOCStatus = u1OcpStatusOld[i] ? 0:1;
				mhl_oc_status.MhlDeviceConnect = 0;//bMHLDeviceState(1)? 1:0;not used for soem
                if (rMhlOcpCbHandler) {
                    // notify to ap
                    rMhlOcpCbHandler(i, mhl_oc_status);
                    u1OcpFlag[i] = (u1OcpStatusOld[i] ? 0:1);
                }

                // set OCP timeout
                if (u1OcpFlag[i] == 1) {
                	u1OcpTimeout[i] = SIL_OCP_TIMEOUT_CNT;
                } else {
                	u1OcpTimeout[i] = 0;
                }

            }
        }

         // ocp timeout count
         if (u1OcpTimeout[i]) {
         	
         	u1OcpTimeout[i]--;
         	if (u1OcpTimeout[i] == 0) {
      		    //printf(" ocp timeout ...\n");
                  u1OcpStatusCurrent[i] =  0x40;   // FAULT PIN normal high
       	    	u1OcpFlag[i] = 0;
				mhl_oc_status.MhlOCStatus = 0;
				mhl_oc_status.MhlDeviceConnect = 0;//bMHLDeviceState(1)? 1:0;not used for soem
                  if (rMhlOcpCbHandler) {
                      // notify to ap, clear ocp status
                      rMhlOcpCbHandler(i,mhl_oc_status);
                  }       	    
            }
        }

    }
    u1TimeOutFlag = 0;
	u1VbusFlag = 0;
}

static VOID _HDMI_SwitchThread(VOID)
{
	UINT8 pdata[3] = {0};
#ifdef CC_MHL_INT
	INT32 IntEnbable=1;
#endif
	HDMI_SIL_FW_VER_T rSilVersion;
	HDMI_SIL_FW_VER_T *prSilVersion;
	
	HDMI_SIL_FW_VER_T rSilFwVer;
	HDMI_SIL_FW_VER_T *prSilFwVer;
	
	HDMI_SIL_STATUS_T rSilStatus;  
	HDMI_SIL_STATUS_T *prSilStatus;
	
	prSilVersion = &rSilVersion;
	prSilFwVer = &rSilFwVer;
	prSilStatus = &rSilStatus;
	
	UNUSED(pdata);
	UNUSED(prSilVersion);
	UNUSED(prSilFwVer);
	UNUSED(prSilStatus);
#if 0
	if(u1MhlInit == 0)
	{
		u1MhlInit =1;
		vHDMIMHL_Init();
		u4LoopCnt=0;
	}
#endif
#ifdef CC_MHL_INT
	 //u4Gpio = MHL_GPIO_INT;
    VERIFY(GPIO_Reg(MHL_GPIO_INT, GPIO_TYPE_INTR_LEVEL_LOW, vMhlIrq) >= 0);
	GPIO_Intrq(MHL_GPIO_INT,&IntEnbable);

    while (1)
    {
        /* Wait for HDMI timer to release the semaphore,    */
        /* then call the device event monitor.  Leave the   */
        /* semaphore locked so that we will start the wait  */
        /* again when we're done.                           */

       VERIFY( x_sema_lock( _hHDMIswitchSema, X_SEMA_OPTION_WAIT) == OSR_OK);
	   vMhl_MainLoop();
    }
#else
	while (1)
    {
        /* Wait for HDMI timer to release the semaphore,    */
        /* then call the device event monitor.  Leave the   */
        /* semaphore locked so that we will start the wait  */
        /* again when we're done.                           */

       VERIFY( x_sema_lock( _hHDMIswitchSema, X_SEMA_OPTION_WAIT) == OSR_OK);
		if (u1MhlGetPanelPower() == MHL_SWITCH_SEQUENCE_OFF) 
		{
				// do nothing
				//printf("_____Panel power:%d________\n",u1MhlGetPanelPower());
		}
		else if(u1MhlInit == 0) 
		{
			LOG(0,"MHL SWITCH BEGIN INITIAL\n");
			if ((u4LoopCnt % SIL_REINIT_CNT == 0) && (u4LoopCnt < SIL_INIT_TIMEOUT)) 
			{

				if ( fgSil_Sif_ReadBytes(SIL_DEV_PAGE_0, 0, pdata, 1) == SIL_OK) 
				{	

					//vSil_Delay1ms(545);
					
					vfgSiI_MhlSetState(1,1);
					vHDMIMHL_Init();

					// get chip ID
					fgSil_Get_Chip_ID(prSilVersion);

					// firmware version
					fgSil_Get_FW_Version(prSilFwVer);
					LOG(0,"fgSil_Get_FW_Version %d.%d.%d \n",prSilFwVer->major,prSilFwVer->second, prSilFwVer->third);

					// status
					fgSil_GetStatus(SIL_PORT_1,prSilStatus);	

					u4LoopCnt = 0;

					u1MhlInit = 1;
					fgSil_Set_3D_State(FALSE);//Not support MHL 3D in this porj.
					LOG(0," mhl bridge init ok.... %d\n", u4LoopCnt);
				} 
				else 
				{
					LOG(0, " mhl bridge rst fail %d....\n",u4LoopCnt);	  
					ChipRst(0,10);
				}

			}

			if (u4LoopCnt < SIL_INIT_TIMEOUT + 1)
			{
				u4LoopCnt++;
			}

		}
		else
		{// every 100ms
			u4LoopCnt++;
			if (( u4LoopCnt % SIL_LOOP_COUNTER) == 0)
			{
				vMhl_MainLoop();
			}
			// ocp handler
			vSiiOcpDetect();
		}
	}	
#endif
}    
#endif
#endif
#endif    

/* 
@brief : When TV power on, FW will call this function once. 
*/     
void vMhlInit(void)
{
#if defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
  vCbusInit();
#elif defined (CC_MHL_HDMI_SWITCH)
  UINT8 i = 0;

  if (u1MhlInit == 1) {
  	printf("mhl bridge re-inited !!! \n");
  }
  
     u4LoopCnt = 0;
     u1MhlInit = 0;

     
     for ( i = 0 ; i < sizeof(u1OcpStatusCurrent) ; i++) {
         u1OcpStatusCurrent[i] = 0;
     }     
     for ( i = 0 ; i < sizeof(u1OcpStatusOld) ; i++) {
         u1OcpStatusOld[i] = 0;
     }
     for ( i = 0 ; i < sizeof(u1OcpReleaseCnt) ; i++) {
         u1OcpReleaseCnt[i] = 0;
     }
     for ( i = 0 ; i < sizeof(u1OcpDebunCnt) ; i++) {
         u1OcpDebunCnt[i] = 0;
     }
     for ( i = 0 ; i < sizeof(u1OcpFlag) ; i++) {
         u1OcpFlag[i] = 0;
     }

	   // GPIO_Input(MHL_GPIO_WAKEUP);
     //fgSil_ChipRst(1,10);

     u4InitialDelay = SIL_INI_DELAY;

      // 10 ms delay after 1292 HW reset
      //vSil_Delay1ms(10);
#endif      
}
#ifdef SYS_MHL_SUPPORT
#if defined(CC_MHL_HDMI_SWITCH)
void vMhlGo(void)
{

  if (u1MhlInit == 1) {
  	printf("mhl bridge re-inited !!! \n");
  }
  
   // plan to run all mhl staff are running on it's own thread.

    /* Create HDMI switch timer semaphore.  */
    VERIFY(x_sema_create(&_hHDMIswitchSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);

    /* Create HDMI Control thread.   */

    VERIFY(x_thread_create(&_hHDMIswitchThread, "HDMI_switch_thread", 2048, 200,
        (x_os_thread_main_fct)_HDMI_SwitchThread, 0, NULL) == OSR_OK);

#ifndef CC_MHL_INT

	   /* Create HDMI switch timer semaphore.  */

    if (x_timer_create(&_HDMISwitchMainTimer) != OSR_OK)
    {
        //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
        printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
        return;
    }

    /* Start the 100ms timer.  Every time this timer fires, the */
    /* vHDMISwitchMainLoopTimer() function is called.           */

    if (x_timer_start( _HDMISwitchMainTimer, SIL_TIMER_PERIOD, X_TIMER_FLAG_REPEAT, \
        vHDMISwitchMainLoopTimer, NULL) != OSR_OK)
    {
        printf("Start HDMI switch timer fail !\n");
        return;
    }
#endif

}
#endif
#endif
#if defined(CC_MHL_HDMI_SWITCH)
extern void SendRcpKey(UINT8 key);
UINT8 bSendRcpKeys(UINT8 Key, UINT8 event)
{
    SendRcpKey(Key);
     
     printf("fgSil_RcpKeys 0x%x, event: %d\n", Key, event);
     return 1;
}
#elif defined(CC_MT5399) ||defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 bSendRcpKeys(UINT8 Key, UINT8 event)
{
   vCbusSendRcpKeys(Key, event);
   return TRUE;
}
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
void vSendMscSubMsg(CBUS_MSC_MSG_COMMAND_T *msg)
{
	vCbusSendMsg(msg->opcode,msg->data);
}
void vSendScratchPadData(CBUS_MSG_DESCRIPTION_T *msg)
{
	vCbusRequestSendSratchPad(msg);
}

#endif
#endif

void vCbus_SetTxCallback(x_cbus_drv_nfy_tx_result nfyfp)
{
    rCbusTxCbHandler = nfyfp;
}

void vCbus_SetRxCallback(x_cbus_drv_nfy_data_arrival nfyfp)
{
    rCbusRxCbHandler = nfyfp;
}

void vCbus_NotifyTxResult(UINT8 result, void *tag)
{
    if (rCbusTxCbHandler)
    {
        rCbusTxCbHandler(result, tag);
    }
}

void vCbus_NotifyRxResult(CBUS_MSG_DESCRIPTION_T *msg)
{
    if (rCbusRxCbHandler)
    {
        rCbusRxCbHandler(msg);
    }
}

void vCbus_SetRAPKCallback(x_cbus_drv_nfy_rapk nfyfp)
{
   rCbusRapkCbHandler = nfyfp;
}
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
void vCbus_SetMscCallback(x_cbus_drv_nfy_msc_subcommand nfyFp,void *arg)
{
	rCbusMscHandler = nfyFp;
	u4MhlMscbArg = (UINT32)arg;
}

void vCbus_SetScratchPadCallback(x_cbus_drv_nfy_scratchpad_data nfyFp,void *arg)
{
	rCbusScratchpadHandler = nfyFp;
	u4MhlSpcbArg = (UINT32)arg;
}

void vCbus_SetHotPlugCallback(x_cbus_drv_nfy_hot_plug nfyFp,void *arg)
{
	rCbusHotPlugHandler = nfyFp;
	u4MhlHpcbArg = (UINT32)arg;
}

void vCbus_SetCbusModeCallback(x_cbus_drv_nfy_cbus_mode nfyFp,void *arg)
{
	rCbusModeHandler = nfyFp;
	u4MhlBmcbArg = (UINT32)arg;
}

void vCbus_SetOverCurrentCallback(x_cbus_drv_nfy_vbus_overcurrent nfyFp,void *arg)
{
	rCbusOverCurrentHandler = nfyFp;
	u4MhlOCcbArg = (UINT32)arg;
}


void vCbus_NotifyMscResult(UINT8 u1Port,UINT8 u1Opcode,UINT8 data,void *arg)
{
	if(drvMhlNtfEnable())
	{
	    if (rCbusMscHandler)
	    {  
	    	if((!drvMhlNtfRapEnable()) && ((u1Opcode == MHL_MSC_MSG_RAP) || (u1Opcode == MHL_MSC_MSG_RAPK)))
    		{
    			return;
    		}
	        rCbusMscHandler(u1Port,u1Opcode,data,arg);
	    }
	}
}

void vCbus_NotifyScratchPadResult(CBUS_MSG_DESCRIPTION_T *msg)
{
	if(drvMhlNtfEnable())
	{
		if (rCbusScratchpadHandler)
		{
			rCbusScratchpadHandler(msg);
		}
	}
}

void vCbus_NotifyHotPlugResult(UINT8 u1Port,UINT8 connect, void *arg)
{
	if(drvMhlNtfEnable())
	{
	    if (rCbusHotPlugHandler)
	    {
	        rCbusHotPlugHandler(u1Port,connect,arg);
	    }
	}
}

void vCbus_NotifyCbusModeResult(UINT8 u1Port,UINT8 mode, void *arg)
{
	if(drvMhlNtfEnable())
	{
	    if (rCbusModeHandler)
	    {
	        rCbusModeHandler(u1Port,mode,arg);
	    }
	}
}
void vCbus_NotifyOverCurrentResult(UINT8 u1Port,UINT8 ocStatus,void *arg)
{
	if(drvMhlNtfEnable())
	{
	    if (rCbusOverCurrentHandler)
	    {
	        rCbusOverCurrentHandler(u1Port,ocStatus,arg);
	    }
	}
}
#endif
#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NFYRcp(UINT8 key)
{
    printf(">>> nfy RCP key 0x%2x\n", key);
    return(SIL_OK);
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NFYRcp(UINT8 key)
{
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)

    CBUS_MSG_DESCRIPTION_T arMsg;


    UINT8 u1KeyCodeTableSize = (sizeof(_at_rcp_key_code))/(sizeof(_at_rcp_key_code[0]));
    UINT8 i = 0;
    UINT32 u4Addr = 0;
    UINT32 u4Val = 0;


    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x10;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
	
	LOG(0,">>> nfy RCP key Received 0x%2x\n",key);
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
		for(i=0; i<u1KeyCodeTableSize; i++)
		{
		   if(key == _at_rcp_key_code[i].u1_key_code)
		   {			  
			  if ((u4Addr = DRVCUST_InitGet(eEepromNHLRCPOffset)) != 0x00)
			  {
			    u4Val = 1;
//	            EEPROM_Write(u4Addr, (UINT32)&u4Val, 1);
				IRRX_SendMtkIr(_at_rcp_key_code[i].u4_key_code);
				LOG(0,"%s:Send to Ir Done!",__FUNCTION__);
				return SIL_OK;
			  }
           }
        }
    }
#endif
    return(SIL_OK);
}

BOOL Mhl_NFYRcpK(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x11;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
	vCbus_NotifyMscResult(1,arMsg.Opcode,key,(void *)u4MhlMscbArg);
#endif
		vCbus_NotifyRxResult(&arMsg);
    }
    printf(">>> nfy RCPK key 0x%2x\n", key);
    return(SIL_OK);
}

BOOL Mhl_NFYRcpE(UINT8 u1ErrorCode)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x12;
    arMsg.Size = 1;
    arMsg.Data[0] = u1ErrorCode;
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT    
		vCbus_NotifyMscResult(1,arMsg.Opcode,u1ErrorCode,(void *)u4MhlMscbArg);
#endif
		vCbus_NotifyRxResult(&arMsg);
    }
    printf(">>> nfy RCPE error code 0x%2x\n", u1ErrorCode);
    return(SIL_OK);
}

#endif
#if defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NFYUcp(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x30;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT  
	vCbus_NotifyMscResult(1,arMsg.Opcode,key,(void *)u4MhlMscbArg);
#endif
		vCbus_NotifyRxResult(&arMsg);
    }
    printf(">>> nfy UCP key 0x%2x\n", key);
    return(SIL_OK);
}

BOOL Mhl_NFYUcpK(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x31;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT  
		vCbus_NotifyMscResult(1,arMsg.Opcode,key,(void *)u4MhlMscbArg);
#endif
		vCbus_NotifyRxResult(&arMsg);
    }
    printf(">>> nfy UCPK key 0x%2x\n", key);
    return(SIL_OK);
}

BOOL Mhl_NFYUcpE(UINT8 u1ErrorCode)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x32;
    arMsg.Size = 1;
    arMsg.Data[0] = u1ErrorCode;
    if(eHDMICurrSwitch == HDMI_SWITCH_1)
    {
#ifdef SYS_ANDROID_TV_TIF_SUPPORT 
		vCbus_NotifyMscResult(1,arMsg.Opcode,u1ErrorCode,(void *)u4MhlMscbArg);
#endif
		vCbus_NotifyRxResult(&arMsg);
    }
    printf(">>> nfy RCPE error code 0x%2x\n", u1ErrorCode);
    return(SIL_OK);
}
#endif
#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NFYRAP(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
	
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x20;
    arMsg.Size = 1;
    arMsg.Data[0] = key;

	if(key == 0x11)
	{
		x_memset(buf1, 0, sizeof(buf1));
		IsReiceiveContentOffFlag=1;
	}

	if(key == 0x10)
	{
		IsReiceiveContentOnFlag=1;
	}
	
	if(((key == 0x11) && (eHDMICurrSwitch == HDMI_SWITCH_1)) || (key == 0x10) )
	{
		vCbus_NotifyRxResult(&arMsg);
	}

    printf(">>> nfyrap 0x%2x ..... \n", key);
   
    return(SIL_OK);
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NFYRAP(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
	
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x20;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
	
#ifdef SYS_ANDROID_TV_TIF_SUPPORT  
	vCbus_NotifyMscResult(1,arMsg.Opcode,key,(void *)u4MhlMscbArg);
#endif
    vCbus_NotifyRxResult(&arMsg);

    printf(">>> nfyrap 0x%2x ..... \n", key);
   
    return(SIL_OK);
}
#endif
#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NFYRAPK(UINT8 key)
{

   printf(">>> nfy RAPK 0x%2x\n", key);
   uIsHaveAckFlag++;
   uRapkFlag=key;
#if 0
   switch(key)
   {
      case 0x00:
	  	uRapkFlag=0;
	  	//return SIL_OK;
		break;
	  default:
	  	uRapkFlag=0;
		break;
	  	//return SIL_FAIL;
   }
#endif
   //x_sema_unlock(_hMHLRAPKSema);

   return SIL_OK;//(uRapkFlag ? SIL_OK : SIL_FAIL);
}
#elif defined(CC_MT5399)|| defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NFYRAPK(UINT8 key)
{
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
	CBUS_MSG_DESCRIPTION_T arMsg;
#endif
  printf(">>> nfy RAPK 0x%2x\n", key);

  if(vIsSonyMHLDevice()) 
  {
       if(u1RapPollingFlag ==1)
	   {
	   		u1RapPollingFlag =0;
			x_timer_stop(_MHLRapkTimer);
	   }
	   uIsHaveAckFlag++;
	   uRapkFlag=key;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT  
	  vCbus_NotifyMscResult(1,arMsg.Opcode,key,(void *)u4MhlMscbArg);
#endif
	   if(rCbusRapkCbHandler)
	   {
	   	  rCbusRapkCbHandler(key);
	   }
  }
	
	   return SIL_OK;//(uRapkFlag ? SIL_OK : SIL_FAIL);
}
#endif

BOOL IsRevRapK(void)
{
   return uRapkFlag;

}
#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NFYWriteBurstData(UINT8 key, UINT8 *pdata, UINT8 size)
{
   UINT8 i = 0;
#ifdef CECNFY5VSTATUS
	   if(_IsReceiveOsdName == TRUE)
	   {
		   _Cec_Notify5VStatus(3,1);//MHL SOURCE 
	   }
#endif
    printf(">>> nfy scratchpad key 0x%2x\n", key);
   for (i = 0 ; i < 16 && i < size ; i++)
   {
       printf("0x%2x, ", pdata[i]);
	   buf1[i]=pdata[i];
   }
   printf("\n");
   return(SIL_OK);
}

BOOL Mhl_SendWriteBurstRequest(void)//notify MSC:: MSC_WRITE_BURST = 0x6c ...etc.
{
    //vCbusSendWriteBurstData(key, pdata, size);
   // vCbusWriteBurstRequest();
	return TRUE;
}

#elif defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 u1IsOSDNAMEReady=0;
#if 0//def CECNFY5VSTATUS
extern x_cec_drv_nfy_5v_status rCec5vHandler;
#endif
//UINT8  u1IsGetOsdName =0;
#if defined(CC_MT5399) || defined(CC_MT5881)
BOOL Mhl_NFYWriteBurstData(UINT8 key, UINT8* pdata, UINT8 size)
{
   UINT8 i = 0;

   CBUS_MSG_DESCRIPTION_T arMsg;
	
   arMsg.MscMsg = 0x6c;
   arMsg.Opcode = 0x00;
   arMsg.Size =(UINT8)size;
    printf(">>> nfy scratchpad key 0x%2x\n", key);
   for (i = 0 ; i < 16 && i < size ; i++)
   {
       printf("0x%2x, ", pdata[i]);
	   arMsg.Data[i] = (UINT8)pdata[i];
   }
   printf("\n");
   vCbus_NotifyRxResult(&arMsg);

   printf(">>> nfyrcp 0x%2x ..... \n", key);
   return(SIL_OK);
}
#elif defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NFYWriteBurstData(UINT8 key, UINT16* pdata, UINT16 size)
{
   UINT8 i = 0;

   CBUS_MSG_DESCRIPTION_T arMsg;
	
   arMsg.MscMsg = 0x6c;
   arMsg.Opcode = 0x00;
   arMsg.Size =(UINT8)size;
    printf(">>> nfy scratchpad key 0x%2x\n", key);
   for (i = 0 ; i < 16 && i < size ; i++)
   {
       printf("0x%2x, ", pdata[i]);
	   arMsg.Data[i] = (UINT8)pdata[i];
   }
   printf("\n");
#ifdef SYS_ANDROID_TV_TIF_SUPPORT  
	vCbus_NotifyScratchPadResult(&arMsg);
#endif
   vCbus_NotifyRxResult(&arMsg);

   printf(">>> nfyscratchpad data 0x%2x ..... \n", key);
   return(SIL_OK);
}

#endif
BOOL Mhl_SendWriteBurstRequest(void)//notify MSC:: MSC_WRITE_BURST = 0x6c ...etc.
{
    //vCbusSendWriteBurstData(key, pdata, size);
   // vCbusWriteBurstRequest();
	return TRUE;
}
#endif
void MHL_SetOCPCallback(x_vbus_drv_nfy_ocp_status nfyfp)
{
    rMhlOcpCbHandler = nfyfp;
}

#ifdef SYS_ANDROID_TV_TIF_SUPPORT
void Mhl_NotifyHotPlug(UINT8 u1Port,UINT8 u1Oc)
{
	vCbus_NotifyHotPlugResult(u1Port,u1Oc,(void *)u4MhlHpcbArg);
}
void Mhl_NotifyCbusMode(UINT8 u1Port,UINT8 u1CbusMode)
{
	vCbus_NotifyCbusModeResult(u1Port,u1CbusMode,(void *)u4MhlBmcbArg);
}
#else
void Mhl_NotifyHotPlug(UINT8 u1Port,UINT8 u1Oc)
{
	//vCbus_NotifyHotPlugResult(u1Port,u1Oc,(void *)u4MhlHpcbArg);
}
#endif

#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NFYOCPStatus(UINT8 port, UINT8 result)
{
   u1OcpStatusCurrent[0] =  result;
   if (result) {
   	if ((u1OcpReleaseCnt[0] > SIL_OCP_FREE_CNT) || (u1OcpReleaseCnt[0] == 0)) {
   	    u1OcpReleaseCnt[0] = SIL_OCP_FREE_CNT;
   	    // reset ocp timeout timer
   	    //u1OcpTimeout[0] = 0;
   	}
   } else {
       if ((u1OcpReleaseCnt[0] > SIL_OCP_DELAY_CNT) ||(u1OcpReleaseCnt[0] == 0)) {   
   	    u1OcpReleaseCnt[0] = SIL_OCP_DELAY_CNT;
       }
   }
   
   printf("Sil_NFYOCP port:%d, status: %d, relCnt \n", port, result, u1OcpReleaseCnt[0]);
   
   return(SIL_OK);
}

BOOL MhlSetVBus(UINT8 value)
{
	return TRUE;
}

#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 u1OCPNormalCNT=0;
UINT32 u1OCPControlCNT=0;
UINT8 u1OCPFaultCNT=0;
UINT8 u1OCPCurrent=0xaa;
UINT8 u1OCPOld=0xaa;
BOOL Mhl_NFYOCPStatus(UINT8 port, UINT8 result)
{
   if(result == 0)
   {
   		u1OCPNormalCNT++;
		u1OCPFaultCNT = 0;
		u1OCPControlCNT++;
   }else if(result == 1)
   {
   		u1OCPNormalCNT = 0;
		u1OCPFaultCNT ++;
   }

   if( ((u1OCPNormalCNT == 4) && (u1OCPControlCNT == 4)) || (u1OCPFaultCNT == 4))
   {
       if(u1OCPNormalCNT ==4)
       {
       		u1OCPNormalCNT =0;
       }

	   if(u1OCPFaultCNT ==4)
	   {
	   		u1OCPFaultCNT =0;
			u1OCPControlCNT =0;
	   }
	   
	   u1OcpStatusCurrent[0] =  result;
	   
	   if (result == 0)
	   {
		   	if ((u1OcpReleaseCnt[0] > SIL_OCP_FREE_CNT) || (u1OcpReleaseCnt[0] == 0)) 
			{
		   	    u1OcpReleaseCnt[0] = SIL_OCP_FREE_CNT;
		   	    // reset ocp timeout timer
		   	   // u1OcpTimeout[0] = 0;
		   	}
	   } 
	   else if(result == 1)
	   {
	       if ((u1OcpReleaseCnt[0] > SIL_OCP_DELAY_CNT) ||(u1OcpReleaseCnt[0] == 0)) 
		   {   
	   	    	u1OcpReleaseCnt[0] = 0;
	       }
	   }
   }
   
   LOG(1,"Sil_NFYOCP port:%d, status: %d, relCnt = %d\n", port, result, u1OcpReleaseCnt[0]);
   
   return(SIL_OK);
}

BOOL Mhl_GetOCPStatus(UINT8 port) // OCP: true, Non-OCP: false.
{
	
#ifdef CBUS_OC_ADIN_USING	
	UNUSED(port);
	return (u1CbusADINGetOcpStatus());
#else
	return (u1CbusGetOcpStatus(port));
#endif
   //return 0;
}
//Set PS_CTRL to 0 or 1. For control VBUS power out or not.
//Control Confition: If current is MHL mode and OCP(GPIO0) was happened event.
BOOL MhlSetVBus(UINT8 value)
{
	vCbusSetVbus(value);
	return TRUE;
}

VOID Mhl_NotifyOCP(UINT8 OCStatus,UINT8 DevConn)
{
	MHL_OC_STATUS mhl_oc_status;
	mhl_oc_status.MhlOCStatus = OCStatus;
	mhl_oc_status.MhlDeviceConnect = DevConn;//bMHLDeviceState(1)? 1:0;

	u1OcpFlag[0] = OCStatus;
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
	vCbus_NotifyOverCurrentResult(1,OCStatus,(void *)u4MhlOCcbArg);
#endif
	if (rMhlOcpCbHandler) {
	// notify to ap
	//printf("########:driver notify..................\n");
	rMhlOcpCbHandler(0, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
	
	}
}
#endif


#ifdef CC_MHL_HDMI_SWITCH
void fgSil_NTFDevCap(HDMI_SIL_DevCap_STATUS_T *pHDMI_SIL_DevCap_STATUS)
{
	_u4DevieAdapterID =  pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_H;
	_u4DevieAdapterID = ( _u4DevieAdapterID << 8) | pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_L;

	printf("______nfy DevCap,adapter id =0x%x_______\n",_u4DevieAdapterID);

	if(vIsSonyMHLDevice())
	{
        //wait 5sens
		if (x_timer_create(&_MHLRapPollingTimer) != OSR_OK)
		{
		   //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   return;
		}

		/* Start the 100ms timer.  Every time this timer fires, the */
		/* vHDMISwitchMainLoopTimer() function is called.		   */

		if (x_timer_start( _MHLRapPollingTimer, 1000, X_TIMER_FLAG_REPEAT, \
		   vMHLRapPollingTimer, NULL) != OSR_OK)
		{
		   printf("Start HDMI switch timer fail !\n");
		   return;
		}
		u2RapPollingCnt=0;	
		u1wait5sensFlag=0;
	}

}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
void Mhl_NTFDevCap(HDMI_SIL_DevCap_STATUS_T *pHDMI_SIL_DevCap_STATUS)
{
	_u4DevieAdapterID =  pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_H;
	_u4DevieAdapterID = ( _u4DevieAdapterID << 8) | pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_L;

	printf("______nfy DevCap,adapter id =0x%x_______\n",_u4DevieAdapterID);

	if(vIsSonyMHLDevice())
	{
        //wait 5sens
		if (x_timer_create(&_MHLRapPollingTimer) != OSR_OK)
		{
		   //LOG(5, "[Sony_HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   printf ("[HDMISwitchMainTimer] Create HDMI switch timer fail !\n");
		   return;
		}

		/* Start the 100ms timer.  Every time this timer fires, the */
		/* vHDMISwitchMainLoopTimer() function is called.		   */

		if (x_timer_start( _MHLRapPollingTimer, 1000, X_TIMER_FLAG_REPEAT, \
		   vMHLRapPollingTimer, NULL) != OSR_OK)
		{
		   printf("Start HDMI switch timer fail !\n");
		   return;
		}
		u2RapPollingCnt=0;	
		u1wait5sensFlag=0;
	}

}
#endif


#if defined(CC_MHL_HDMI_SWITCH)
BOOL fgSil_NTF_MSC_CMD(UINT8 port, UINT8 MscCmd, UINT8 OpCode, UINT8 Data)
{
#if 0 // for test only

   if ((MscCmd == 0x64) && (OpCode == 0x0) && (Data == 0x0))
   {


           vIO32Write1B(PDWNC_EDID_ADDR, 0);
           
   #if defined(MHL_PORT_MAPPING_2)
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_DE_CNT0);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_DE_CNT1);
           vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 3, FLD_DE_CNT0);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 3, FLD_DE_CNT1);

           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 1, FLD_EDID0_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 1, FLD_EDID1_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_EDID0_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_EDID1_DIS);
   #else

           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_DE_CNT2);
           vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 3, FLD_DE_CNT2);

           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 1, FLD_EDID2_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_EDID2_DIS);
           
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 1, FLD_EDID0_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 1, FLD_EDID1_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 1, FLD_EDID2_RST);           
           vIO32WriteFldAlign(PDWNC_EDID_DEV3, 1, FLD_EDID3_RST);
            vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_EDID0_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_EDID1_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_EDID2_RST);           
           vIO32WriteFldAlign(PDWNC_EDID_DEV3, 0, FLD_EDID3_RST);

           vIO32Write1B(PDWNC_EDID_ADDR, 0);
           
          // printf("sil nyf ddc dis_en 0x%x\n", IO32ReadFldAlign(PDWNC_EDID_DEV2,FLD_DE_CNT));           
   #endif
   }
   
#endif
   
   return(SIL_OK);
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_NTF_MSC_CMD(UINT8 port, UINT8 MscCmd, UINT8 OpCode, UINT8 Data)
{
    vCbusNtfMscCmd(port, MscCmd, OpCode, Data);
#if 0 // for test only

   if ((MscCmd == 0x64) && (OpCode == 0x0) && (Data == 0x0))
   {


           vIO32Write1B(PDWNC_EDID_ADDR, 0);
           
   #if defined(MHL_PORT_MAPPING_2)
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_DE_CNT0);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_DE_CNT1);
           vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 3, FLD_DE_CNT0);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 3, FLD_DE_CNT1);

           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 1, FLD_EDID0_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 1, FLD_EDID1_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_EDID0_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_EDID1_DIS);
   #else

           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_DE_CNT2);
           vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 3, FLD_DE_CNT2);

           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 1, FLD_EDID2_DIS);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_EDID2_DIS);
           
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 1, FLD_EDID0_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 1, FLD_EDID1_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 1, FLD_EDID2_RST);           
           vIO32WriteFldAlign(PDWNC_EDID_DEV3, 1, FLD_EDID3_RST);
            vSil_Delay1ms(1);
           vIO32WriteFldAlign(PDWNC_EDID_DEV0, 0, FLD_EDID0_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV1, 0, FLD_EDID1_RST);
           vIO32WriteFldAlign(PDWNC_EDID_DEV2, 0, FLD_EDID2_RST);           
           vIO32WriteFldAlign(PDWNC_EDID_DEV3, 0, FLD_EDID3_RST);

           vIO32Write1B(PDWNC_EDID_ADDR, 0);
           
          // printf("sil nyf ddc dis_en 0x%x\n", IO32ReadFldAlign(PDWNC_EDID_DEV2,FLD_DE_CNT));           
   #endif
   }
   
#endif
   
   return(SIL_OK);
}


#endif



UINT8 u1MhlGetPortId(UINT8 ports)
{
    return (MHL_PORT_ID);
}

UINT8 u1MhlGetCableStatus(UINT8 ports)
{
#if defined(CC_MHL_HDMI_SWITCH)
    //need to do
    return 0;//(rSilStatus.RxCableDetected ? 1:0);
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
    return (u1GetCdsenseStatus());
#endif
	return 0;
}
BOOL vIsSonyMHLDevice(VOID)
{
	if((_u4DevieAdapterID == 0x0143) || (_u4DevieAdapterID == 0x03A7))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT32 u1GetMHLDeviceAdapterID(VOID)
{
	return _u4DevieAdapterID;
}
void u1ClearMHLDeviceAdapterID(VOID)
{
	_u4DevieAdapterID =0;
}

UINT8 u1MhlGetOcpStatus(UINT8 ports)
{
    return ((ports < MHL_PORT_NUM) ? u1OcpFlag[ports] : 0);
}



void vMhlSetRst(void)
{
    #if defined(CC_MHL_HDMI_SWITCH)
    ChipRst(1,1);
	#elif defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	MhlRst(1,1);
	#endif
}

#ifdef CC_MHL_HDMI_SWITCH
static UINT8 u1PanelPower = MHL_SWITCH_SEQUENCE_OFF;
static UINT8 _bflgMhlInit = 0;
HANDLE_T mhlVbusRstTimer = NULL_HANDLE;
#ifndef CC_MTK_LOADER
void vCbusResetTimerHdler(HANDLE_T ptTmHandle, VOID* pvTag)
{
	u1PanelPower = MHL_SWITCH_SEQUENCE_ON;
	if(_hHDMIswitchSema != NULL_HANDLE)
	{
		x_sema_unlock(_hHDMIswitchSema);
	}
}
#endif
void vMhlSetPanelPower(UINT8 val)
{
	UINT32 i = val;
    //u1PanelPower = val;
#ifndef CC_MTK_LOADER
    UNUSED(x_timer_create(&mhlVbusRstTimer));
#endif
	if(val == 1)
	{
	    if(_bflgMhlInit == 0)
	    {
			GPIO_Output(MHL_GPIO_RST, &i); //rst high
			fgSil_NTF_VBUS_Low();  //disable VBUS
			fgSil_Enable_Cbus(0); //diable CBus CBus High-Z
			fgSil_Enable_Termination(0); //Termination off
			//vSil_Delay1ms(10); //delay 20ms then Disable Vbus
			//fgSil_NTF_VBUS_Low();  //disable VBUS
			_bflgMhlInit = 1;
#ifndef CC_MTK_LOADER
			VERIFY(x_timer_start(mhlVbusRstTimer,550,X_TIMER_FLAG_ONCE,vCbusResetTimerHdler,NULL) == OSR_OK);
#endif
	    }			
		LOG(0,"mhl power on>>>>>>>>>>>>>>\n");
	}
	else if(val == 0)
	{
	   //	fgSil_Enable_Cbus(0);
		//fgSil_Enable_Termination(0);
		GPIO_Output(MHL_GPIO_RST, &i); //rst low
	   u1PanelPower = MHL_SWITCH_SEQUENCE_OFF;
		LOG(0,"mhl power off>>>>>>>>>>>>>>\n");
	}
}

UINT8 u1MhlGetPanelPower(void)
{
   //UNUSED(u1PanelPower);
  	return (u1PanelPower);  // for SOEM Model only, power on CBUS switch on/off timing issue.
}
#endif

void vMhlSetSourceStatus(UINT8 port, UINT8 status)
{
    u1SourceStatus = status;
	printf("_____u1SourceStatus=%d_________\n",status);
}


void vMhlSetRapOnOff(UINT8 port, UINT8 OnOff)
{
    u1RapOnOff = OnOff;
	printf("_____u1RapOnOff=%d______\n",u1RapOnOff);
}

#if defined(CC_MHL_HDMI_SWITCH)
static UINT8 u1Loops = 0;
UINT8 fgSil_GetSystemPortStatus(UINT8 port)
{
    UINT8 u1Sta = 0;
    UNUSED(u1Loops);
   // if (u1Loops++ % 50 == 0) {
   //     printf("fgSil_GetSystemPortStatus loops:%d, sta: %d\n", u1Loops, u1SourceStatus);
   // }
    /*
    #define RAP_NOERROR	   0x00
    #define RAP_UNRECOGNIZED 0x01
    #define RAP_UNSUPPORTED  0x02
    #define RAP_BUSY		   0x03
    */
    u1Sta = (u1RapOnOff ? u1SourceStatus : 0);

    switch (u1Sta)
    {
     case 0:
      return MHL_RAP_UNSUPPORTED;
     default:
      return MHL_RAP_NOERROR;
    };
}
#elif defined(CC_MT5399) || defined(CC_MT5881) || defined(CC_MT5890) || defined(CC_MT5882)
UINT8 Mhl_GetSystemPortStatus(UINT8 port)
{
    UINT8 u1Sta = 0;
    UNUSED(port);
   // if (u1Loops++ % 50 == 0) {
   //     printf("fgSil_GetSystemPortStatus loops:%d, sta: %d\n", u1Loops, u1SourceStatus);
   // }
    /*
    #define RAP_NOERROR	   0x00
    #define RAP_UNRECOGNIZED 0x01
    #define RAP_UNSUPPORTED  0x02
    #define RAP_BUSY		   0x03
    */
    u1Sta = (u1RapOnOff ? u1SourceStatus : 0);

    switch (u1Sta)
    {
     case 0:
      return MHL_RAP_UNSUPPORTED;
     default:
      return MHL_RAP_NOERROR;
    };
}
#endif

static UINT8 uRapContentOn=0;
void vMhlSetRapContentOn(UINT8 OnOff)
{
  uRapContentOn=OnOff;
}

UINT8 vGetMhlRapContentON(void)
{
    return uRapContentOn;
}
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_SendWriteBurstData(UINT8 key, UINT8 *pdata, UINT8 size)//notify MSC:: MSC_WRITE_BURST = 0x6c ...etc.
{
    vCbusSendWriteBurstData(key, pdata, size);
	return TRUE;
}
#endif
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL Mhl_RapKeys(UINT8 key)
{
    vCbusSendRapKeys(key);
	return TRUE;
}
#endif

//#ifndef CC_MHL_HDMI_SWITCH
#ifdef CC_MHL_3D_SUPPORT
//#if defined(CC_MT5399) || defined(CC_MT5890) 

MHL_3D_VIC_Structuration_T t3D_VIC_Struct={0};
MHL_3D_DTD_Structuration_T t3D_DTD_Struct={0};
UINT8 uIsSupport3D=0;
WRITE_BURST_VDO_3D_T tWriteBurstVic3D[8];
WRITE_BURST_VDO_3D_T tWriteBurstDtd3D[2];
UINT8 VicWriteburstCnt =0;
UINT8 DtdWriteburstCnt =0;

BOOL Mhl_3D_EDID_Configuration_VIC(MHL_3D_VIC_Structuration_T *p3D_VIC_Struct)
{
	//vCbus3DEdidConfigurationVicDtd(p3D_VIC_Struct,p3D_DTD_Struct);
   if(p3D_VIC_Struct->EDID_1st_VIC)
   {   
       LOG(0,"vic 1 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[0].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[0].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[0].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[0].SEQ=1;//seq
	   tWriteBurstVic3D[0].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[0].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[0].VDI_0_L= (p3D_VIC_Struct->EDID_1st_VIC_LR << 2) + (p3D_VIC_Struct->EDID_1st_VIC_TB << 1) + (p3D_VIC_Struct->EDID_1st_VIC_FS);                                          
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_2nd_VIC)
   {
        LOG(0,"vic 2 \n");
	   tWriteBurstVic3D[0].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[0].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[0].VDI_1_L= (p3D_VIC_Struct->EDID_2nd_VIC_LR << 2) + (p3D_VIC_Struct->EDID_2nd_VIC_TB << 1) + (p3D_VIC_Struct->EDID_2nd_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_3rd_VIC)
   {
       LOG(0,"vic 3 \n");
	   tWriteBurstVic3D[0].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[0].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[0].VDI_2_L= (p3D_VIC_Struct->EDID_3rd_VIC_LR << 2) + (p3D_VIC_Struct->EDID_3rd_VIC_TB << 1) + (p3D_VIC_Struct->EDID_3rd_VIC_FS);
       tWriteBurstVic3D[0].CHECK_SUM += tWriteBurstVic3D[0].VDI_2_H + tWriteBurstVic3D[0].VDI_2_L;

   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_4th_VIC)
   {
       LOG(0,"vic 4 \n");
	   tWriteBurstVic3D[0].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[0].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[0].VDI_3_L= (p3D_VIC_Struct->EDID_4th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_4th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_4th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_5th_VIC)
   {
       LOG(0,"vic 5 \n");
	   tWriteBurstVic3D[0].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[0].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[0].VDI_4_L= (p3D_VIC_Struct->EDID_5th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_5th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_5th_VIC_FS);
       tWriteBurstVic3D[0].CHECK_SUM += tWriteBurstVic3D[0].VDI_4_H + tWriteBurstVic3D[0].VDI_4_L;
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_6th_VIC)
   {
       LOG(0,"vic 6 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[1].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[1].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[1].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[1].SEQ=2;//seq
	   tWriteBurstVic3D[1].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[1].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[1].VDI_0_L= (p3D_VIC_Struct->EDID_6th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_6th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_6th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_7th_VIC)
   {
       LOG(0,"vic 7 \n");
	   tWriteBurstVic3D[1].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[1].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[1].VDI_1_L= (p3D_VIC_Struct->EDID_7th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_7th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_7th_VIC_FS);
       tWriteBurstVic3D[1].CHECK_SUM += tWriteBurstVic3D[1].VDI_1_H + tWriteBurstVic3D[1].VDI_1_H;
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_8th_VIC)
   {
       LOG(0,"vic 8 \n");
	   tWriteBurstVic3D[1].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[1].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[1].VDI_2_L= (p3D_VIC_Struct->EDID_8th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_8th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_8th_VIC_FS);
       tWriteBurstVic3D[1].CHECK_SUM += tWriteBurstVic3D[1].VDI_2_H + tWriteBurstVic3D[1].VDI_2_H;
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_9th_VIC)
   {
       LOG(0,"vic 9 \n");
	   tWriteBurstVic3D[1].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[1].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[1].VDI_3_L= (p3D_VIC_Struct->EDID_9th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_9th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_9th_VIC_FS);
       tWriteBurstVic3D[1].CHECK_SUM += tWriteBurstVic3D[1].VDI_3_H + tWriteBurstVic3D[1].VDI_3_H;
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_10th_VIC)
   {
       LOG(0,"vic 10 \n");
	   tWriteBurstVic3D[1].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[1].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[1].VDI_4_L= (p3D_VIC_Struct->EDID_10th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_10th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_10th_VIC_FS);
       tWriteBurstVic3D[1].CHECK_SUM += tWriteBurstVic3D[1].VDI_4_H + tWriteBurstVic3D[1].VDI_4_H;
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_11th_VIC)
   {
       LOG(0,"vic 11 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[2].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[2].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[2].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[2].SEQ=3;//seq
	   tWriteBurstVic3D[2].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[2].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[2].VDI_0_L= (p3D_VIC_Struct->EDID_11th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_11th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_11th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_12th_VIC)
   {
       LOG(0,"vic 12 \n");
	   tWriteBurstVic3D[2].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[2].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[2].VDI_1_L= (p3D_VIC_Struct->EDID_12th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_12th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_12th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_13th_VIC)
   {
       LOG(0,"vic 13 \n");
	   tWriteBurstVic3D[2].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[2].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[2].VDI_2_L= (p3D_VIC_Struct->EDID_13th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_13th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_13th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }   
   if(p3D_VIC_Struct->EDID_14th_VIC)
   {
       LOG(0,"vic 14 \n");
	   tWriteBurstVic3D[2].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[2].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[2].VDI_3_L= (p3D_VIC_Struct->EDID_14th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_14th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_14th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_15th_VIC)
   {
       LOG(0,"vic 15 \n");
	   tWriteBurstVic3D[2].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[2].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[2].VDI_4_L= (p3D_VIC_Struct->EDID_15th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_15th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_15th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_16th_VIC)
   {
       LOG(0,"vic 16 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[3].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[3].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[3].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[3].SEQ=4;//seq
	   tWriteBurstVic3D[3].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[3].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[3].VDI_0_L= (p3D_VIC_Struct->EDID_16th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_16th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_16th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_17th_VIC)
   {
       LOG(0,"vic 17 \n");
	   tWriteBurstVic3D[3].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[3].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[3].VDI_1_L= (p3D_VIC_Struct->EDID_17th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_17th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_17th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_18th_VIC)
   {
       LOG(0,"vic 18 \n");
	   tWriteBurstVic3D[3].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[3].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[3].VDI_2_L= (p3D_VIC_Struct->EDID_18th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_18th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_18th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   
   if(p3D_VIC_Struct->EDID_19th_VIC)
   {
       Printf("vic 19 \n");
	   tWriteBurstVic3D[3].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[3].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[3].VDI_3_L= (p3D_VIC_Struct->EDID_19th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_19th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_19th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }  
   if(p3D_VIC_Struct->EDID_20th_VIC)
   {
       LOG(6,"vic 20 \n");
	   tWriteBurstVic3D[3].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[3].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[3].VDI_4_L= (p3D_VIC_Struct->EDID_20th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_20th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_20th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_21th_VIC)
   {
       LOG(0,"vic 21 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[4].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[4].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[4].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[4].SEQ=5;//seq
	   tWriteBurstVic3D[4].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[4].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[4].VDI_0_L= (p3D_VIC_Struct->EDID_21th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_21th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_21th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_22th_VIC)
   {
       LOG(0,"vic 22 \n");
	   tWriteBurstVic3D[4].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[4].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[4].VDI_1_L= (p3D_VIC_Struct->EDID_22th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_22th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_22th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_23th_VIC)
   {
       LOG(0,"vic 23 \n");
	   tWriteBurstVic3D[4].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[4].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[4].VDI_2_L= (p3D_VIC_Struct->EDID_23th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_23th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_23th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_24th_VIC)
   {
       LOG(0,"vic 24 \n");
	   tWriteBurstVic3D[4].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[4].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[4].VDI_3_L= (p3D_VIC_Struct->EDID_24th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_24th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_24th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_25th_VIC)
   {
       LOG(0,"vic 25 \n");
	   tWriteBurstVic3D[4].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[4].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[4].VDI_4_L= (p3D_VIC_Struct->EDID_25th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_25th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_25th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   if(p3D_VIC_Struct->EDID_26th_VIC)
   {
       LOG(0,"vic 26 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[5].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[5].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[5].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[5].SEQ=6;//seq
	   tWriteBurstVic3D[5].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[5].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[5].VDI_0_L= (p3D_VIC_Struct->EDID_26th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_26th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_26th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_27th_VIC)
   {
       LOG(0,"vic 27 \n");
	   tWriteBurstVic3D[5].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[5].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[5].VDI_1_L= (p3D_VIC_Struct->EDID_27th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_27th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_27th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_28th_VIC)
   {
       LOG(0,"vic 28 \n");
	   tWriteBurstVic3D[5].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[5].VDI_2_H=0;//vdi2_h
	   tWriteBurstVic3D[5].VDI_2_L= (p3D_VIC_Struct->EDID_28th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_28th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_28th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }  
   if(p3D_VIC_Struct->EDID_29th_VIC)
   {
       LOG(0,"vic 29 \n");
	   tWriteBurstVic3D[5].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[5].VDI_3_H=0;//vdi3_h
	   tWriteBurstVic3D[5].VDI_3_L= (p3D_VIC_Struct->EDID_29th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_29th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_29th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_30th_VIC)
   {
       Printf("vic 30 \n");
	   tWriteBurstVic3D[5].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[5].VDI_4_H=0;//vdi4_h
	   tWriteBurstVic3D[5].VDI_4_L= (p3D_VIC_Struct->EDID_30th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_30th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_30th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }  
   if(p3D_VIC_Struct->EDID_31th_VIC)
   {
       LOG(0,"vic 31 \n");
       VicWriteburstCnt ++;
	   tWriteBurstVic3D[6].BURST_ID_H=0x00;//burst_id_h
	   tWriteBurstVic3D[6].BURST_ID_L=0x10;//burst_id_l
	   tWriteBurstVic3D[6].CHECK_SUM= 0;//check_sum
	   tWriteBurstVic3D[6].SEQ=7;//seq
	   tWriteBurstVic3D[6].NUM_ENT=1;//num_ent
	   tWriteBurstVic3D[6].VDI_0_H=0;//vdi0_h
	   tWriteBurstVic3D[6].VDI_0_L= (p3D_VIC_Struct->EDID_31th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_31th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_31th_VIC_FS);
   }
   else
   {
	   return TRUE;
   } 
   if(p3D_VIC_Struct->EDID_32th_VIC)
   {
       LOG(0,"vic 32 \n");
	   tWriteBurstVic3D[6].NUM_ENT++;//num_ent
	   tWriteBurstVic3D[6].VDI_1_H=0;//vdi1_h
	   tWriteBurstVic3D[6].VDI_1_L= (p3D_VIC_Struct->EDID_32th_VIC_LR << 2) + (p3D_VIC_Struct->EDID_32th_VIC_TB << 1) + (p3D_VIC_Struct->EDID_32th_VIC_FS);
   }
   else
   {
	   return TRUE;
   }
   return TRUE;

}

BOOL Mhl_3D_EDID_Configuration_DTD(MHL_3D_DTD_Structuration_T *p3D_DTD_Struct)
{
	if(p3D_DTD_Struct->EDID_1st_DTD)
	{
		LOG(0,"dtd 1 \n");
	    DtdWriteburstCnt = 1;
		tWriteBurstDtd3D[0].BURST_ID_H=0x00;//burst_id_h
		tWriteBurstDtd3D[0].BURST_ID_L=0x11;//burst_id_l
		tWriteBurstDtd3D[0].CHECK_SUM= 0;//check_sum
		tWriteBurstDtd3D[0].SEQ=1;//seq
		tWriteBurstDtd3D[0].NUM_ENT=1;//num_ent
		tWriteBurstDtd3D[0].VDI_0_H=0;//vdi0_h
		tWriteBurstDtd3D[0].VDI_0_L= (p3D_DTD_Struct->EDID_1st_DTD_LR << 2) + (p3D_DTD_Struct->EDID_1st_DTD_TB << 1) + (p3D_DTD_Struct->EDID_1st_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_2nd_DTD)
	{
		LOG(0,"dtd 2 \n");
		tWriteBurstDtd3D[0].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[0].VDI_1_H=0;//vdi1_h
		tWriteBurstDtd3D[0].VDI_1_L= (p3D_DTD_Struct->EDID_2nd_DTD_LR << 2) + (p3D_DTD_Struct->EDID_2nd_DTD_TB << 1) + (p3D_DTD_Struct->EDID_2nd_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_3rd_DTD)
	{
		LOG(0,"dtd 3 \n");
		tWriteBurstDtd3D[0].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[0].VDI_2_H=0;//vdi2_h
		tWriteBurstDtd3D[0].VDI_2_L= (p3D_DTD_Struct->EDID_3rd_DTD_LR << 2) + (p3D_DTD_Struct->EDID_3rd_DTD_TB << 1) + (p3D_DTD_Struct->EDID_3rd_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_4th_DTD)
	{
		LOG(0,"dtd 4 \n");
		tWriteBurstDtd3D[0].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[0].VDI_3_H=0;//vdi3_h
		tWriteBurstDtd3D[0].VDI_3_L= (p3D_DTD_Struct->EDID_4th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_4th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_4th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_5th_DTD)
	{
		LOG(0,"dtd 5 \n");
		tWriteBurstDtd3D[0].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[0].VDI_4_H=0;//vdi4_h
		tWriteBurstDtd3D[0].VDI_4_L= (p3D_DTD_Struct->EDID_5th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_5th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_5th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_6th_DTD)
	{
		LOG(0,"dtd 6 \n");
	    DtdWriteburstCnt ++;
		tWriteBurstDtd3D[1].BURST_ID_H=0x00;//burst_id_h
		tWriteBurstDtd3D[1].BURST_ID_L=0x11;//burst_id_l
		tWriteBurstDtd3D[1].CHECK_SUM= 0;//check_sum
		tWriteBurstDtd3D[1].SEQ=2;//seq
		tWriteBurstDtd3D[1].NUM_ENT=1;//num_ent
		tWriteBurstDtd3D[1].VDI_0_H=0;//vdi0_h
		tWriteBurstDtd3D[1].VDI_0_L= (p3D_DTD_Struct->EDID_6th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_6th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_6th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_7th_DTD)
	{
		LOG(0,"dtd 7 \n");
		tWriteBurstDtd3D[1].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[1].VDI_1_H=0;//vdi1_h
		tWriteBurstDtd3D[1].VDI_1_L= (p3D_DTD_Struct->EDID_7th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_7th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_7th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_8th_DTD)
	{
		LOG(6,"dtd 8 \n");
		tWriteBurstDtd3D[1].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[1].VDI_2_H=0;//vdi2_h
		tWriteBurstDtd3D[1].VDI_2_L= (p3D_DTD_Struct->EDID_8th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_8th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_8th_DTD_FS);
	}
/*	else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_9th_DTD)
	{
		LOG(6,"dtd 9 \n");
		tWriteBurstDtd3D[1].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[1].VDI_3_H=0;//vdi3_h
		tWriteBurstDtd3D[1].VDI_3_L= (p3D_DTD_Struct->EDID_9th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_9th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_9th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	if(p3D_DTD_Struct->EDID_10th_DTD)
	{
		LOG(6,"dtd 10 \n");
		tWriteBurstDtd3D[1].NUM_ENT++;//num_ent
		tWriteBurstDtd3D[1].VDI_4_H=0;//vdi4_h
		tWriteBurstDtd3D[1].VDI_4_L= (p3D_DTD_Struct->EDID_10th_DTD_LR << 2) + (p3D_DTD_Struct->EDID_10th_DTD_TB << 1) + (p3D_DTD_Struct->EDID_10th_DTD_FS);
	}
	/*else
	{
		return TRUE;
	}*/
	return TRUE;

}
BOOL Mhl_3D_EDID_Configuration_VIC_DTD(MHL_3D_VIC_Structuration_T *p3D_VIC_Struct,MHL_3D_DTD_Structuration_T *p3D_DTD_Struct)
{
	UINT8 i = 0;
	UINT8 u1VicTOTEnt = 0;
	UINT8 u1DtdTOTEnt = 0;
	
	Mhl_3D_EDID_Configuration_VIC(p3D_VIC_Struct);
	Mhl_3D_EDID_Configuration_DTD(p3D_DTD_Struct);
	for(i=0; i<VicWriteburstCnt; i++)
	{
	    u1VicTOTEnt += tWriteBurstVic3D[i].NUM_ENT; 
	}
	for(i=0; i<VicWriteburstCnt; i++)
	{
	    tWriteBurstVic3D[i].TOT_ENT = u1VicTOTEnt;
	    tWriteBurstVic3D[i].CHECK_SUM =  
	          tWriteBurstVic3D[i].BURST_ID_H + tWriteBurstVic3D[i].BURST_ID_L + 
	          tWriteBurstVic3D[i].TOT_ENT + tWriteBurstVic3D[i].SEQ +
	          tWriteBurstVic3D[i].NUM_ENT + tWriteBurstVic3D[i].VDI_0_H + 
	          tWriteBurstVic3D[i].VDI_0_L + tWriteBurstVic3D[i].VDI_1_H + 
	          tWriteBurstVic3D[i].VDI_1_L + tWriteBurstVic3D[i].VDI_2_H + 
	          tWriteBurstVic3D[i].VDI_2_L + tWriteBurstVic3D[i].VDI_3_H + 
	          tWriteBurstVic3D[i].VDI_3_L + tWriteBurstVic3D[i].VDI_4_H + tWriteBurstVic3D[i].VDI_4_L;
	    tWriteBurstVic3D[i].CHECK_SUM = 0xff - tWriteBurstVic3D[i].CHECK_SUM;   
	    tWriteBurstVic3D[i].CHECK_SUM += 1;
	}
	for(i=0; i<DtdWriteburstCnt; i++)
	{
	    u1DtdTOTEnt += tWriteBurstDtd3D[i].NUM_ENT; 
	}
	for(i=0; i<DtdWriteburstCnt; i++)
	{
	    tWriteBurstDtd3D[i].TOT_ENT = u1DtdTOTEnt;
	    tWriteBurstDtd3D[i].CHECK_SUM =  
	          tWriteBurstDtd3D[i].BURST_ID_H + tWriteBurstDtd3D[i].BURST_ID_L + 
	          tWriteBurstDtd3D[i].TOT_ENT + tWriteBurstDtd3D[i].SEQ +
	          tWriteBurstDtd3D[i].NUM_ENT + tWriteBurstDtd3D[i].VDI_0_H + 
	          tWriteBurstDtd3D[i].VDI_0_L + tWriteBurstDtd3D[i].VDI_1_H + 
	          tWriteBurstDtd3D[i].VDI_1_L + tWriteBurstDtd3D[i].VDI_2_H + 
	          tWriteBurstDtd3D[i].VDI_2_L + tWriteBurstDtd3D[i].VDI_3_H + 
	          tWriteBurstDtd3D[i].VDI_3_L + tWriteBurstDtd3D[i].VDI_4_H + tWriteBurstDtd3D[i].VDI_4_L;
	    tWriteBurstDtd3D[i].CHECK_SUM = 0xff - tWriteBurstDtd3D[i].CHECK_SUM;   
	    tWriteBurstDtd3D[i].CHECK_SUM += 1;
	}
	return TRUE;

}

void vMHLParsingEDIDForMHL3D(UINT8 *Edid_table)
{
  
   UINT8 *uEdidVSDBPtr=NULL,*uRdidSVDPtr=NULL;
   UINT8 VIC=0;
   UINT16 i=0;
   UINT8 uVSDBLen=0,DTDlength=0;
   UINT8 uTagCode;
   UINT8 uLength;
   UINT8 index=0;
   INT32 PixClk=0;
   UINT8 u1DTDOffSet=0;

   uEdidVSDBPtr=Edid_table;
   uRdidSVDPtr=Edid_table;
   	
   // for mhl is support 3d;
	for(i = 128; i< 254; i++)
	{

	    if ((Edid_table[i] == 0x3) && \
	    	(Edid_table[i+1] == 0xc) && \
	    	 (Edid_table[i+2] == 0x0))
	    {
	      uEdidVSDBPtr += (i-1);
	    }

	}

    uVSDBLen=(*uEdidVSDBPtr)& 0x1f;
	if(uVSDBLen <8)
	{
		uIsSupport3D=0;//not support 3D
	}
	else if(uVSDBLen >= 8)
	{
		//uEdidPtr=uEdidVSDBPtr;
		if(((*(uEdidVSDBPtr +8) & 0x3f)>>5) >0)
		{
			uIsSupport3D = *(uEdidVSDBPtr + 8 + ((*(uEdidVSDBPtr + 8)>>7)*2 + ((*(uEdidVSDBPtr+8) & 0x7f)>>6)*2) + ((*(uEdidVSDBPtr +8) & 0x3f)>>5))>>7;
			
		}
		else
		{
		    uIsSupport3D=0;
		}
	}
	LOG(0,"___vsdblen=%d,uIsSupport3D=%d_______\n",uVSDBLen,uIsSupport3D);

	//fgSil_Set_3D_State( uIsSupport3D ? 1 : 0 );


	//for mhl edid svd
   i=0x84;
   while(i < 255)
   {
   
		uTagCode = Edid_table[i] >> 5;
		uLength  = Edid_table[i] & 0x1f;
		if( uTagCode == 2)
		{
			uRdidSVDPtr=Edid_table + i;
			break;
		}
		else
		{
		    i += (uLength+1);
		}
		
    }
    LOG(0,"____svd uLength=0x%x______\n",uLength);
	for(index = 0; index < uLength; index++)
	{
	   VIC = *(uRdidSVDPtr + 1 + index) & 0x7f;
	   LOG(6,"____vic=0x%x______\n",VIC);
	   
	   switch(VIC)
	   {
	   	  case 16: //1080p/60
	   	  case 31: //1080p/50
	   	     if(index == 0)
	   	     {
	   	      t3D_VIC_Struct.EDID_1st_VIC=1;
			  t3D_VIC_Struct.EDID_1st_VIC_FS=0;
			  t3D_VIC_Struct.EDID_1st_VIC_LR=0;
			  t3D_VIC_Struct.EDID_1st_VIC_TB=0;
	   	     }
			 else if(index == 1)
			 {
			  
			  t3D_VIC_Struct.EDID_2nd_VIC=1;
			   t3D_VIC_Struct.EDID_2nd_VIC_FS=0;
			   t3D_VIC_Struct.EDID_2nd_VIC_LR=0;
			   t3D_VIC_Struct.EDID_2nd_VIC_TB=0;
			   
			 }
			 else if(index == 2)
			 {
			  
			   t3D_VIC_Struct.EDID_3rd_VIC=1;
			   t3D_VIC_Struct.EDID_3rd_VIC_FS=0;
			   t3D_VIC_Struct.EDID_3rd_VIC_LR=0;
			   t3D_VIC_Struct.EDID_3rd_VIC_TB=0;
			   
			 }
			 else if(index == 3)
			 {
			  t3D_VIC_Struct.EDID_4th_VIC=1;
			   t3D_VIC_Struct.EDID_4th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_4th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_4th_VIC_TB=0;
			   
			   
			 }
			 else if(index == 4)
			 {
			  t3D_VIC_Struct.EDID_5th_VIC=1;
			   t3D_VIC_Struct.EDID_5th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_5th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_5th_VIC_TB=0;
			  
			 }
			 else if(index == 5)
			 {
			   t3D_VIC_Struct.EDID_6th_VIC=1;
			   t3D_VIC_Struct.EDID_6th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_6th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_6th_VIC_TB=0;
			 }
			 else if(index == 6)
			 {
			    t3D_VIC_Struct.EDID_7th_VIC=1;
				t3D_VIC_Struct.EDID_7th_VIC_FS=0;
				t3D_VIC_Struct.EDID_7th_VIC_LR=0;
				t3D_VIC_Struct.EDID_7th_VIC_TB=0;	
			 }
			 else if(index == 7)
			 {
			  t3D_VIC_Struct.EDID_8th_VIC=1;
			   t3D_VIC_Struct.EDID_8th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_8th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_8th_VIC_TB=0;
			 }
			 else if(index == 8)
			 {
			  t3D_VIC_Struct.EDID_9th_VIC=1;
			   t3D_VIC_Struct.EDID_9th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_9th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_9th_VIC_TB=0;
			 
			 }
			 else if(index == 9)
			 {
			    t3D_VIC_Struct.EDID_10th_VIC=1;
			   t3D_VIC_Struct.EDID_10th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_10th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_10th_VIC_TB=0;
			 }
			 else if(index == 10)
			 {
			  t3D_VIC_Struct.EDID_11th_VIC=1;
			   t3D_VIC_Struct.EDID_11th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_11th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_11th_VIC_TB=0;
			  
			 }
			 else if(index == 11)
			 {
			  t3D_VIC_Struct.EDID_12th_VIC=1;
			   t3D_VIC_Struct.EDID_12th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_12th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_12th_VIC_TB=0;
			  
			   
			 }
			 else if(index == 12)
			 {
			 t3D_VIC_Struct.EDID_13th_VIC=1;
			   t3D_VIC_Struct.EDID_13th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_13th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_13th_VIC_TB=0;
			  
			 }
			 else if(index == 13)
			 {
			  t3D_VIC_Struct.EDID_14th_VIC=1;
			   t3D_VIC_Struct.EDID_14th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_14th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_14th_VIC_TB=0;
			  
			   
			 }
			 else if(index == 14)
			 {
			 t3D_VIC_Struct.EDID_15th_VIC=1;
			   t3D_VIC_Struct.EDID_15th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_15th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_15th_VIC_TB=0;
			  
			 }
			 else if(index == 15)
			 {
			  t3D_VIC_Struct.EDID_16th_VIC=1;
			   t3D_VIC_Struct.EDID_16th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_16th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_16th_VIC_TB=0;
			 }
			 else if(index == 16)
			 {
			 t3D_VIC_Struct.EDID_17th_VIC=1;
			   t3D_VIC_Struct.EDID_17th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_17th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_17th_VIC_TB=0;
			   
			 }
			 else if(index == 17)
			 {
			 t3D_VIC_Struct.EDID_18th_VIC=1;
			   t3D_VIC_Struct.EDID_18th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_18th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_18th_VIC_TB=0;
			  
			  
			 }
			 else if(index == 18)
			 {
			  t3D_VIC_Struct.EDID_19th_VIC=1;
			   t3D_VIC_Struct.EDID_19th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_19th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_19th_VIC_TB=0;
			   
			 }
			 else if(index == 19)
			 {
			 t3D_VIC_Struct.EDID_20th_VIC=1;
			   t3D_VIC_Struct.EDID_20th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_20th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_20th_VIC_TB=0;
			  
			  
			 }
			 else if(index == 20)
			 {
			  t3D_VIC_Struct.EDID_21th_VIC=1;
			   t3D_VIC_Struct.EDID_21th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_21th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_21th_VIC_TB=0;
			   
			 }
			 else if(index == 21)
			 {
			 t3D_VIC_Struct.EDID_22th_VIC=1;
			   t3D_VIC_Struct.EDID_22th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_22th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_22th_VIC_TB=0;
			  
			  
			 }
			 else if(index == 22)
			 {
			  t3D_VIC_Struct.EDID_23th_VIC=1;
			   t3D_VIC_Struct.EDID_23th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_23th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_23th_VIC_TB=0;
			   
			 }
			 else if(index == 23)
			 {
			 t3D_VIC_Struct.EDID_24th_VIC=1;
			   t3D_VIC_Struct.EDID_24th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_24th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_24th_VIC_TB=0;
			  
			   
			 }
			 else if(index == 24)
			 {
			 t3D_VIC_Struct.EDID_25th_VIC=1;
			   t3D_VIC_Struct.EDID_25th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_25th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_25th_VIC_TB=0;
			  
			 }
			 else if(index == 25)
			 {
			  t3D_VIC_Struct.EDID_26th_VIC=1;
			   t3D_VIC_Struct.EDID_26th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_26th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_26th_VIC_TB=0;
			 }
			 else if(index == 26)
			 {
			 t3D_VIC_Struct.EDID_27th_VIC=1;
			   t3D_VIC_Struct.EDID_27th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_27th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_27th_VIC_TB=0;
			  
			 }
			 else if(index == 27)
			 {
			  t3D_VIC_Struct.EDID_28th_VIC=1;
			   t3D_VIC_Struct.EDID_28th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_28th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_28th_VIC_TB=0;
			  
			   
			 }
			 else if(index == 28)
			 {
			 t3D_VIC_Struct.EDID_29th_VIC=1;
			   t3D_VIC_Struct.EDID_29th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_29th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_29th_VIC_TB=0;
			   
			 }
			 else if(index == 29)
			 {
			 t3D_VIC_Struct.EDID_30th_VIC=1;
			   t3D_VIC_Struct.EDID_30th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_30th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_30th_VIC_TB=0;
			  
			  
			 }
			 else if(index == 30)
			 {
			  t3D_VIC_Struct.EDID_31th_VIC=1;
			   t3D_VIC_Struct.EDID_31th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_31th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_31th_VIC_TB=0;
			  
			 }
			 else if(index == 31)
			 {
			  t3D_VIC_Struct.EDID_32th_VIC=1;
			   t3D_VIC_Struct.EDID_32th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_32th_VIC_LR=0;
			   t3D_VIC_Struct.EDID_32th_VIC_TB=0;
			 }
			 
			 break;
		  default:
		  	 if(index == 0)
	   	     {
	   	      t3D_VIC_Struct.EDID_1st_VIC=1;
			  t3D_VIC_Struct.EDID_1st_VIC_FS=1;
			  t3D_VIC_Struct.EDID_1st_VIC_LR=1;
			  t3D_VIC_Struct.EDID_1st_VIC_TB=1;
	   	     }
			 else if(index == 1)
			 {
			  
			  t3D_VIC_Struct.EDID_2nd_VIC=1;
			   t3D_VIC_Struct.EDID_2nd_VIC_FS=0;
			   t3D_VIC_Struct.EDID_2nd_VIC_LR=1;
			   t3D_VIC_Struct.EDID_2nd_VIC_TB=1;
			   
			 }
			 else if(index == 2)
			 {
			  
			   t3D_VIC_Struct.EDID_3rd_VIC=1;
			   t3D_VIC_Struct.EDID_3rd_VIC_FS=0;
			   t3D_VIC_Struct.EDID_3rd_VIC_LR=1;
			   t3D_VIC_Struct.EDID_3rd_VIC_TB=1;
			   
			 }
			 else if(index == 3)
			 {
			  t3D_VIC_Struct.EDID_4th_VIC=1;
			   t3D_VIC_Struct.EDID_4th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_4th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_4th_VIC_TB=1;
			   
			   
			 }
			 else if(index == 4)
			 {
			  t3D_VIC_Struct.EDID_5th_VIC=1;
			   t3D_VIC_Struct.EDID_5th_VIC_FS=1;
			   t3D_VIC_Struct.EDID_5th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_5th_VIC_TB=1;
			  
			 }
			 else if(index == 5)
			 {
			   t3D_VIC_Struct.EDID_6th_VIC=1;
			   t3D_VIC_Struct.EDID_6th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_6th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_6th_VIC_TB=1;
			 }
			 else if(index == 6)
			 {
			    t3D_VIC_Struct.EDID_7th_VIC=1;
				t3D_VIC_Struct.EDID_7th_VIC_FS=0;
				t3D_VIC_Struct.EDID_7th_VIC_LR=1;
				t3D_VIC_Struct.EDID_7th_VIC_TB=1;	
			 }
			 else if(index == 7)
			 {
			  t3D_VIC_Struct.EDID_8th_VIC=1;
			   t3D_VIC_Struct.EDID_8th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_8th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_8th_VIC_TB=1;
			 }
			 else if(index == 8)
			 {
			  t3D_VIC_Struct.EDID_9th_VIC=1;
			   t3D_VIC_Struct.EDID_9th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_9th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_9th_VIC_TB=1;
			 
			 }
			 else if(index == 9)
			 {
			    t3D_VIC_Struct.EDID_10th_VIC=1;
			   t3D_VIC_Struct.EDID_10th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_10th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_10th_VIC_TB=1;
			 }
			 else if(index == 10)
			 {
			  t3D_VIC_Struct.EDID_11th_VIC=1;
			   t3D_VIC_Struct.EDID_11th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_11th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_11th_VIC_TB=1;
			  
			 }
			 else if(index == 11)
			 {
			  t3D_VIC_Struct.EDID_12th_VIC=1;
			   t3D_VIC_Struct.EDID_12th_VIC_FS=1;
			   t3D_VIC_Struct.EDID_12th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_12th_VIC_TB=1;
			  
			   
			 }
			 else if(index == 12)
			 {
			 t3D_VIC_Struct.EDID_13th_VIC=1;
			   t3D_VIC_Struct.EDID_13th_VIC_FS=1;
			   t3D_VIC_Struct.EDID_13th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_13th_VIC_TB=1;
			  
			 }
			 else if(index == 13)
			 {
			  t3D_VIC_Struct.EDID_14th_VIC=1;
			   t3D_VIC_Struct.EDID_14th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_14th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_14th_VIC_TB=1;
			  
			   
			 }
			 else if(index == 14)
			 {
			 t3D_VIC_Struct.EDID_15th_VIC=1;
			   t3D_VIC_Struct.EDID_15th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_15th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_15th_VIC_TB=1;
			  
			 }
			 else if(index == 15)
			 {
			  t3D_VIC_Struct.EDID_16th_VIC=1;
			   t3D_VIC_Struct.EDID_16th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_16th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_16th_VIC_TB=1;
			 }
			 else if(index == 16)
			 {
			 t3D_VIC_Struct.EDID_17th_VIC=1;
			   t3D_VIC_Struct.EDID_17th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_17th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_17th_VIC_TB=1;
			   
			 }
			 else if(index == 17)
			 {
			 t3D_VIC_Struct.EDID_18th_VIC=1;
			   t3D_VIC_Struct.EDID_18th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_18th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_18th_VIC_TB=1;
			  
			  
			 }
			 else if(index == 18)
			 {
			  t3D_VIC_Struct.EDID_19th_VIC=1;
			   t3D_VIC_Struct.EDID_19th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_19th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_19th_VIC_TB=1;
			   
			 }
			 else if(index == 19)
			 {
			 t3D_VIC_Struct.EDID_20th_VIC=1;
			   t3D_VIC_Struct.EDID_20th_VIC_FS=1;
			   t3D_VIC_Struct.EDID_20th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_20th_VIC_TB=1;
			  
			  
			 }
			 else if(index == 20)
			 {
			  t3D_VIC_Struct.EDID_21th_VIC=1;
			   t3D_VIC_Struct.EDID_21th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_21th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_21th_VIC_TB=1;
			   
			 }
			 else if(index == 21)
			 {
			 t3D_VIC_Struct.EDID_22th_VIC=1;
			   t3D_VIC_Struct.EDID_22th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_22th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_22th_VIC_TB=1;
			  
			  
			 }
			 else if(index == 22)
			 {
			  t3D_VIC_Struct.EDID_23th_VIC=1;
			   t3D_VIC_Struct.EDID_23th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_23th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_23th_VIC_TB=1;
			   
			 }
			 else if(index == 23)
			 {
			 t3D_VIC_Struct.EDID_24th_VIC=1;
			   t3D_VIC_Struct.EDID_24th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_24th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_24th_VIC_TB=1;
			  
			   
			 }
			 else if(index == 24)
			 {
			 t3D_VIC_Struct.EDID_25th_VIC=1;
			   t3D_VIC_Struct.EDID_25th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_25th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_25th_VIC_TB=1;
			  
			 }
			 else if(index == 25)
			 {
			  t3D_VIC_Struct.EDID_26th_VIC=1;
			   t3D_VIC_Struct.EDID_26th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_26th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_26th_VIC_TB=1;
			 }
			 else if(index == 26)
			 {
			 t3D_VIC_Struct.EDID_27th_VIC=1;
			   t3D_VIC_Struct.EDID_27th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_27th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_27th_VIC_TB=1;
			  
			 }
			 else if(index == 27)
			 {
			  t3D_VIC_Struct.EDID_28th_VIC=1;
			   t3D_VIC_Struct.EDID_28th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_28th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_28th_VIC_TB=1;
			  
			   
			 }
			 else if(index == 28)
			 {
			 t3D_VIC_Struct.EDID_29th_VIC=1;
			   t3D_VIC_Struct.EDID_29th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_29th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_29th_VIC_TB=1;
			   
			 }
			 else if(index == 29)
			 {
			 t3D_VIC_Struct.EDID_30th_VIC=1;
			   t3D_VIC_Struct.EDID_30th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_30th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_30th_VIC_TB=1;
			  
			  
			 }
			 else if(index == 30)
			 {
			  t3D_VIC_Struct.EDID_31th_VIC=1;
			   t3D_VIC_Struct.EDID_31th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_31th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_31th_VIC_TB=1;
			  
			 }
			 else if(index == 31)
			 {
			  t3D_VIC_Struct.EDID_32th_VIC=1;
			   t3D_VIC_Struct.EDID_32th_VIC_FS=0;
			   t3D_VIC_Struct.EDID_32th_VIC_LR=1;
			   t3D_VIC_Struct.EDID_32th_VIC_TB=1;
			 }
			 
		  	break;
	   	}
	   		
	}
			  
   //for mhl edid dtd
   //block 0
   DTDlength=4;
   for(i=0;i<128;i++)
   {
     if ((Edid_table[i] == 0x00) && \
	    	(Edid_table[i+1] == 0x00) && \
	    	 (Edid_table[i+2] == 0x00) && \
	    	 (Edid_table[i+3] == 0xFC) && \
	    	 (Edid_table[i+4] == 0x00))
	    {
	      DTDlength -= 1;
		  
	    }
	  if ((Edid_table[i] == 0x00) && \
	    	(Edid_table[i+1] == 0x00) && \
	    	 (Edid_table[i+2] == 0x00) && \
	    	 (Edid_table[i+3] == 0xFD) && \
	    	 (Edid_table[i+4] == 0x00))
	    {
	      DTDlength -= 1;
		 
	    }
   }

   LOG(0,"_____DTDlength in block0=%d_______\n",DTDlength);
   
   #if 1
   //uEdidPtr=HDMIEDID_table;
   for(index=0;index < DTDlength;index++)
   {
     PixClk = Edid_table[0x36 + index*18 +1 ]<<8; //*(uEdidPtr + 0x36 +index*18 +1 ) << 8;
	 PixClk |= Edid_table[0x36 + index*18];//*(uEdidPtr + 0x36 +index*18);
	 LOG(0,"_____block0 PixClk=%d_______\n",PixClk);
	 if(PixClk >= 2500 && PixClk <= 7500)
	 {
	    if(index == 0)
	    {
			t3D_DTD_Struct.EDID_1st_DTD=1;
			t3D_DTD_Struct.EDID_1st_DTD_LR=1;
			t3D_DTD_Struct.EDID_1st_DTD_TB=1;
			t3D_DTD_Struct.EDID_1st_DTD_FS=0;
 
	    }
		else if(index == 1)
		{
			t3D_DTD_Struct.EDID_2nd_DTD=1;
			t3D_DTD_Struct.EDID_2nd_DTD_LR=1;
			t3D_DTD_Struct.EDID_2nd_DTD_TB=1;
			t3D_DTD_Struct.EDID_2nd_DTD_FS=0;

  
		}
		else if(index == 2)
        {
			t3D_DTD_Struct.EDID_3rd_DTD=1;
			t3D_DTD_Struct.EDID_3rd_DTD_LR=1;
			t3D_DTD_Struct.EDID_3rd_DTD_TB=1;
			t3D_DTD_Struct.EDID_3rd_DTD_FS=0;
  
        }
		else if(index == 3)
	    {
			t3D_DTD_Struct.EDID_4th_DTD=1;
			t3D_DTD_Struct.EDID_4th_DTD_LR=1;
			t3D_DTD_Struct.EDID_4th_DTD_TB=1;
			t3D_DTD_Struct.EDID_4th_DTD_FS=0;

	    }
	 }
	 else if(PixClk > 7500)
	 {
	    if(index == 0)
	    {
			t3D_DTD_Struct.EDID_1st_DTD=1;
			t3D_DTD_Struct.EDID_1st_DTD_LR=0;
			t3D_DTD_Struct.EDID_1st_DTD_TB=0;
			t3D_DTD_Struct.EDID_1st_DTD_FS=0;
	    }
		else if(index == 1)
		{
			 t3D_DTD_Struct.EDID_2nd_DTD=1;
			  t3D_DTD_Struct.EDID_2nd_DTD_LR=0;
			  t3D_DTD_Struct.EDID_2nd_DTD_TB=0;
			  t3D_DTD_Struct.EDID_2nd_DTD_FS=0;
		}
		else if(index == 2)
	    {
	       t3D_DTD_Struct.EDID_3rd_DTD=1;
			  t3D_DTD_Struct.EDID_3rd_DTD_LR=0;
			  t3D_DTD_Struct.EDID_3rd_DTD_TB=0;
			  t3D_DTD_Struct.EDID_3rd_DTD_FS=0;
	    }
		else if(index == 3)
	    {
	      t3D_DTD_Struct.EDID_4th_DTD=1;
		  t3D_DTD_Struct.EDID_4th_DTD_LR=0;
		  t3D_DTD_Struct.EDID_4th_DTD_TB=0;
		  t3D_DTD_Struct.EDID_4th_DTD_FS=0;
	    }
	 }
   }
   #endif

   //block1
  u1DTDOffSet=Edid_table[0x82];
  if(DTDlength == 2 )
  {
  	  for(index=0; index < 4; index++)
	  {
		PixClk = Edid_table[0x80 + u1DTDOffSet + index*18 +1] <<8;//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18 + 1 ) << 8;
		PixClk |= Edid_table[0x80 + u1DTDOffSet + index*18];//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18);
		LOG(0,"_____block1 PixClk=%d_______\n",PixClk);
		if(PixClk >= 2500 && PixClk <= 7500)
		{
		    if(index == 0)
		    {
		      t3D_DTD_Struct.EDID_3rd_DTD=1;
			  t3D_DTD_Struct.EDID_3rd_DTD_LR=1;
			  t3D_DTD_Struct.EDID_3rd_DTD_TB=1;
			  t3D_DTD_Struct.EDID_3rd_DTD_FS=0;
			 
		    }
			else if(index == 1)
			{
			    t3D_DTD_Struct.EDID_4th_DTD=1;
			  t3D_DTD_Struct.EDID_4th_DTD_LR=1;
			  t3D_DTD_Struct.EDID_4th_DTD_TB=1;
			  t3D_DTD_Struct.EDID_4th_DTD_FS=0;

	  
			}
			else if(index == 2)
	        {
	         t3D_DTD_Struct.EDID_5th_DTD=1;
			  t3D_DTD_Struct.EDID_5th_DTD_LR=1;
			  t3D_DTD_Struct.EDID_5th_DTD_TB=1;
			  t3D_DTD_Struct.EDID_5th_DTD_FS=0;
	        }
			else if(index == 3)
		    {
		      t3D_DTD_Struct.EDID_6th_DTD=1;
			  t3D_DTD_Struct.EDID_6th_DTD_LR=1;
			  t3D_DTD_Struct.EDID_6th_DTD_TB=1;
			  t3D_DTD_Struct.EDID_6th_DTD_FS=0;
		    }
		 }
		 else if(PixClk > 7500)
		 {
		    if(index == 0)
		    {
				t3D_DTD_Struct.EDID_3rd_DTD=1;
				t3D_DTD_Struct.EDID_3rd_DTD_LR=0;
				t3D_DTD_Struct.EDID_3rd_DTD_TB=0;
				t3D_DTD_Struct.EDID_3rd_DTD_FS=0;
		    }
			else if(index == 1)
			{
				t3D_DTD_Struct.EDID_4th_DTD=1;
				t3D_DTD_Struct.EDID_4th_DTD_LR=0;
				t3D_DTD_Struct.EDID_4th_DTD_TB=0;
				t3D_DTD_Struct.EDID_4th_DTD_FS=0;
			}
			else if(index == 2)
		    {
				t3D_DTD_Struct.EDID_5th_DTD=1;
				t3D_DTD_Struct.EDID_5th_DTD_LR=0;
				t3D_DTD_Struct.EDID_5th_DTD_TB=0;
				t3D_DTD_Struct.EDID_5th_DTD_FS=0;
		    }
			else if(index == 3)
		    {
				t3D_DTD_Struct.EDID_6th_DTD=1;
				t3D_DTD_Struct.EDID_6th_DTD_LR=0;
				t3D_DTD_Struct.EDID_6th_DTD_TB=0;
				t3D_DTD_Struct.EDID_6th_DTD_FS=0;
		    }
		 }
	  }
  	}
    else if(DTDlength == 3)
    {
    	for(index=0; index < 4; index++)
		{
			//PixClk = *(uEdidPtr + 0x80 + *(uEdidPtr + 0x82)+index*18 +1 ) << 8;
			//PixClk |= *(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) +index*18);
			PixClk = Edid_table[0x80 + u1DTDOffSet + index*18 +1] <<8;//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18 + 1 ) << 8;
			PixClk |= Edid_table[0x80 + u1DTDOffSet + index*18];//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18);
			LOG(0,"_____block1 PixClk=%d_______\n",PixClk);
			if(PixClk >= 2500 && PixClk <= 7500)
			{
			   
				if(index == 0)
				{
				  t3D_DTD_Struct.EDID_4th_DTD=1;
				  t3D_DTD_Struct.EDID_4th_DTD_LR=1;
				  t3D_DTD_Struct.EDID_4th_DTD_TB=1;
				  t3D_DTD_Struct.EDID_4th_DTD_FS=0;
				}
				else if(index == 1)
			    {
			     t3D_DTD_Struct.EDID_5th_DTD=1;
				  t3D_DTD_Struct.EDID_5th_DTD_LR=1;
				  t3D_DTD_Struct.EDID_5th_DTD_TB=1;
				  t3D_DTD_Struct.EDID_5th_DTD_FS=0;
			    }
				else if(index == 2)
			    {
			      t3D_DTD_Struct.EDID_6th_DTD=1;
				  t3D_DTD_Struct.EDID_6th_DTD_LR=1;
				  t3D_DTD_Struct.EDID_6th_DTD_TB=1;
				  t3D_DTD_Struct.EDID_6th_DTD_FS=0;
			    }
				else if(index == 3)
			    {
			      t3D_DTD_Struct.EDID_7th_DTD=1;
				  t3D_DTD_Struct.EDID_7th_DTD_LR=1;
				  t3D_DTD_Struct.EDID_7th_DTD_TB=1;
				  t3D_DTD_Struct.EDID_7th_DTD_FS=0;
				 
			    }
			 }
			 else if(PixClk > 7500)
			 {
			   if(index == 0)
				{
					t3D_DTD_Struct.EDID_4th_DTD=1;
					t3D_DTD_Struct.EDID_4th_DTD_LR=0;
					t3D_DTD_Struct.EDID_4th_DTD_TB=0;
					t3D_DTD_Struct.EDID_4th_DTD_FS=0;
				}
				else if(index == 1)
			    {
					t3D_DTD_Struct.EDID_5th_DTD=1;
					t3D_DTD_Struct.EDID_5th_DTD_LR=0;
					t3D_DTD_Struct.EDID_5th_DTD_TB=0;
					t3D_DTD_Struct.EDID_5th_DTD_FS=0;
			    }
				else if(index == 2)
			    {
					t3D_DTD_Struct.EDID_6th_DTD=1;
					t3D_DTD_Struct.EDID_6th_DTD_LR=0;
					t3D_DTD_Struct.EDID_6th_DTD_TB=0;
					t3D_DTD_Struct.EDID_6th_DTD_FS=0;
			    }
				else  if(index == 3)
			    {
					t3D_DTD_Struct.EDID_7th_DTD=1;
					t3D_DTD_Struct.EDID_7th_DTD_LR=0;
					t3D_DTD_Struct.EDID_7th_DTD_TB=0;
					t3D_DTD_Struct.EDID_7th_DTD_FS=0;
			    }
				
			 }
		}
    }
   
    Mhl_3D_EDID_Configuration_VIC_DTD(&t3D_VIC_Struct,&t3D_DTD_Struct);
 
}
#endif
//#endif
#ifdef SYS_ANDROID_TV_TIF_SUPPORT
UINT8 u1MhlSetOption(MHL_SET_OPTION_T eMhlSetOption)
{	
	switch(eMhlSetOption.flag)
	{
		case DRV_MHL_OPTION_ALLOW_INPUT_SWITCHING:
			drvMhlNtfCtrl.rapNeedNtf = eMhlSetOption.on?1:0;
			printf("DRV_MHL_OPTION_ALLOW_INPUT_SWITCHING: %s\n",eMhlSetOption.on?"on":"off");
			break;
		case DRV_MHL_OPTION_POWER_CHARGE:
			fgStandbyPowerCharge = eMhlSetOption.on?1:0;
			printf("DRV_MHL_OPTION_POWER_CHARGE: %s\n",eMhlSetOption.on?"on":"off");
			break;
		case DRV_MHL_OPTION_ENABLE_MHL:
			drvMhlNtfCtrl.enAbleAllNtf = eMhlSetOption.on?1:0;
			printf("DRV_MHL_OPTION_ENABLE_MHL: %s\n",eMhlSetOption.on?"on":"off");
			break;
		default:
			break;
	}
	return 1;
}
UINT8 u1CbusGetPeerMhlVersion(int *version)
{
	*version = _arMhlSrcDevCap[0x01];
	//*version = 0x20;
	return 1;
}
#endif

#endif

