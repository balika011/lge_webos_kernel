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

#include "hw_pdwnc.h"

#include "mhl_if.h"

// PCB configuration
#define MHL_PORT_NUM 1
#ifdef FIJI_PCB_SCT
#define MHL_PORT_ID 4
#else
#define MHL_PORT_ID 2
#endif
#define MHL_GPIO_RST 208
#define MHL_GPIO_VBUS 205
#define MHL_GPIO_WAKEUP 204
#define MHL_GPIO_INT 0xffffffff
#define SIL_SIF_BUS 4 // PDWNC1
#define SIL_SIF_RATE 0x100
#define SIL_SIF_ADR_NUM 1

static HANDLE_T _HDMISwitchMainTimer;
HANDLE_T _MHLRapPollingTimer;
#define SIL_TIMER_PERIOD  10     // ms
#define SIL_LOOP_COUNTER  (100/SIL_TIMER_PERIOD)     // 100 ms
#define SIL_OCP_FREE_CNT  (500/SIL_TIMER_PERIOD)    // 500ms
#define SIL_OCP_DELAY_CNT  (10/SIL_TIMER_PERIOD)    // 10 ms
#define SIL_OCP_TIMEOUT_CNT  (2000/SIL_TIMER_PERIOD)    // 2s

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

static HANDLE_T _hHDMIswitchThread;
static HANDLE_T _hHDMIswitchSema = NULL_HANDLE;

HAL_TIME_T  timerStartTime;
HAL_TIME_T  currentTime;
HAL_TIME_T  deltaTime;

static x_cbus_drv_nfy_data_arrival rCbusRxCbHandler;
static x_cbus_drv_nfy_tx_result rCbusTxCbHandler;
// ocp
x_vbus_drv_nfy_ocp_status rMhlOcpCbHandler;

static UINT8 u1MhlInit = 0;

MHL_3D_VIC_Structuration_T p3D_VIC_Struct={0};
MHL_3D_DTD_Structuration_T p3D_DTD_Struct={0};
UINT8 uIsSupport3D;


UINT8 IsReiceiveContentOnFlag=0;
UINT8 IsReiceiveContentOffFlag=0;
//extern UINT8 buf1[16];
UINT8 buf1[16];
void vMhlSetPanelPower(UINT8 val);
UINT8 u1MhlGetPanelPower(void);
void vMhlSetRst(void);

// SIF Function
BOOL fgSil_Sif_WriteBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount)
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

BOOL fgSil_Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount)
{
    INT32 i4ReturnValue;
    i4ReturnValue = SIF_X_Read(SIL_SIF_BUS,SIL_SIF_RATE, DeviceID,  SIL_SIF_ADR_NUM, Offset, pData, ByteCount);

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

// timer
BOOL fgSil_Timer_Set(UINT8 Index, UINT32 MilliSecond)
{

	HAL_GetTime( &timerStartTime);

  return (SIL_OK);
}

UINT32 u4Sil_Timer_Get(UINT8 Index)
{
  return (((1000000 * deltaTime.u4Seconds) + deltaTime.u4Micros));
}

BOOL fgSil_Timer_Expired(UINT8 Index)
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

void vSil_Delay1ms(UINT32 ms)
{
  vUtDelay1ms(ms);
}

void vSil_Delay1us(UINT32 us)
{
  vUtDelay2us((us > 1 ? us>>1 : us));
}

// hardwaare reset
BOOL fgSil_ChipRst(UINT8 Polarity, UINT32 MicroSecond)
{
  INT32 val;
  val = (INT32)!Polarity;
  GPIO_Output(MHL_GPIO_RST, &val);
  vSil_Delay1ms(MicroSecond);
  val = (INT32)Polarity;
  GPIO_Output(MHL_GPIO_RST, &val);
  
  return (SIL_OK);
}

HDMI_SIL_STATUS_T rSilStatus;
void _CheckMhlStatus(UINT8 val)
{
   HDMI_SIL_STATUS_T rStatus;
   HDMI_SIL_FW_VER_T rSilFwVer;
     	
   fgSil_GetStatus(0, &rStatus);

   if (x_memcmp(&rSilStatus, &rStatus, sizeof(HDMI_SIL_STATUS_T)) != 0 || val == 1)
   {
   #if 1
       if (val == 1)
       {
// firmware version
       fgSil_Get_FW_Version(&rSilFwVer);

       printf("PCB configuration: SIF_BUS: %d, MHL PORT: %d, RST#:%d, WAKEUP%d\n",SIL_SIF_BUS,u1MhlGetPortId(1),MHL_GPIO_RST,MHL_GPIO_WAKEUP);
   #ifdef PsCtrl_High   
       printf("PS_CTRL# Polarity high\n");
   #else
       printf("PS_CTRL# Polarity low\n");
   #endif
   #ifdef DETECT_OCP
       printf("OCP enabled\n");
   #else
       printf("OCP disabled\n");
   #endif
       printf("fgSil_Get_FW_Version: %d.%d.%d \n",rSilFwVer.major,rSilFwVer.second, rSilFwVer.third);                
       printf("SupportEdid %d -> %d\n",rStatus.SupportEdid,rSilStatus.SupportEdid);
       printf("SupportHdcp %d -> %d\n",rStatus.SupportHdcp,rSilStatus.SupportHdcp);
       printf("SupportMhl %d -> %d\n",rStatus.SupportMhl,rSilStatus.SupportMhl);
       printf("HdcpKeyGood %d -> %d\n",rStatus.HdcpKeyGood,rSilStatus.HdcpKeyGood);
       printf("EdidKeyGood %d -> %d\n",rStatus.EdidKeyGood,rSilStatus.EdidKeyGood);
       printf("RxCableDetected %d -> %d\n",rStatus.RxCableDetected,rSilStatus.RxCableDetected);
       printf("RxSync %d -> %d\n",rStatus.RxSync,rSilStatus.RxSync);
       printf("RxStable %d -> %d\n",rStatus.RxStable,rSilStatus.RxStable);
       printf("MhlHdmiDevice %d -> %d\n",rStatus.MhlHdmiDevice,rSilStatus.MhlHdmiDevice);
       printf("HdmiMode %d -> %d\n",rStatus.HdmiMode,rSilStatus.HdmiMode);
       printf("TxCableDetected %d -> %d\n",rStatus.TxCableDetected,rSilStatus.TxCableDetected);
       printf("TxOutputing %d -> %d\n",rStatus.TxOutputing,rSilStatus.TxOutputing);
       printf("HdmiMode %d -> %d\n",rStatus.HdmiMode,rSilStatus.HdmiMode);
       }
   #else
       LOG(5,"Mhl status change\n");
       LOG(5,"SupportEdid %d -> %d\n",rStatus.SupportEdid,rSilStatus.SupportEdid);
       LOG(5,"SupportHdcp %d -> %d\n",rStatus.SupportHdcp,rSilStatus.SupportHdcp);
       LOG(5,"SupportMhl %d -> %d\n",rStatus.SupportMhl,rSilStatus.SupportMhl);
       LOG(5,"HdcpKeyGood %d -> %d\n",rStatus.HdcpKeyGood,rSilStatus.HdcpKeyGood);
       LOG(5,"EdidKeyGood %d -> %d\n",rStatus.EdidKeyGood,rSilStatus.EdidKeyGood);
       LOG(5,"RxCableDetected %d -> %d\n",rStatus.RxCableDetected,rSilStatus.RxCableDetected);
       LOG(5,"RxSync %d -> %d\n",rStatus.RxSync,rSilStatus.RxSync);
       LOG(5,"RxStable %d -> %d\n",rStatus.RxStable,rSilStatus.RxStable);
       LOG(5,"MhlHdmiDevice %d -> %d\n",rStatus.MhlHdmiDevice,rSilStatus.MhlHdmiDevice);
       LOG(5,"HdmiMode %d -> %d\n",rStatus.HdmiMode,rSilStatus.HdmiMode);
       LOG(5,"TxCableDetected %d -> %d\n",rStatus.TxCableDetected,rSilStatus.TxCableDetected);
       LOG(5,"TxOutputing %d -> %d\n",rStatus.TxOutputing,rSilStatus.TxOutputing);
       LOG(5,"HdmiMode %d -> %d\n",rStatus.HdmiMode,rSilStatus.HdmiMode);
   #endif
       x_memcpy(&rSilStatus, &rStatus, sizeof(HDMI_SIL_STATUS_T));
   }

}

UINT8 bMHLDeviceConnect=0;
void fgSil_NTF_CBUS_Connected(void)
{
	bMHLDeviceConnect=1;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	printf("______connected______\n");
}
void fgSil_NTF_CBUS_Disconnected(void)
{
	bMHLDeviceConnect=0;
	x_memset(buf1, 0, sizeof(buf1));
	IsReiceiveContentOnFlag=0;
	IsReiceiveContentOffFlag=0;
	//x_timer_stop(_MHLRapPollingTimer);
//	x_timer_stop(_MHLRapkTimer);
	
	printf("______disconnected______\n");
}
void fgSil_NTF_3D_Communicated(void)
{
	
}
UINT8 bMHLDeviceState(UINT8 port)
{
	if(bMHLDeviceConnect ==1)
	{
		return 1;
	}
	
	if((u1MhlGetCableStatus(1) ==0) || (bMHLDeviceConnect == 0))
	{
		return 0;
	}

	return 0;
}
UINT8 _IsMhlDevice(void)
{
   //return (u1MhlInit);
   return (((rSilStatus.MhlHdmiDevice & 0x11) == 0x11) ? 1 : 0);
}

static UINT8 u1OcpDebunCnt[MHL_PORT_NUM] = {0};
static UINT8 u1OcpReleaseCnt[MHL_PORT_NUM] = {0};
static UINT8 u1OcpTimeout[MHL_PORT_NUM] = {0};
static UINT8 u1OcpStatusCurrent[MHL_PORT_NUM] = {0xaa};
static UINT8 u1OcpStatusOld[MHL_PORT_NUM] = {0xaa};
static UINT8 u1OcpFlag[MHL_PORT_NUM] = {0};
static UINT8 u1MHLPlugCurrent[MHL_PORT_NUM] = {0xaa};
static UINT8 u1MHLPlugOld[MHL_PORT_NUM] = {0xaa};
static MHL_OC_STATUS mhl_oc_status;
static void vOcpHandler(void)
{
    UINT8 i = 0;
    
    for (i = 0 ; i < MHL_PORT_NUM ; i++) {
    	
    	 UNUSED(u1OcpDebunCnt[i]);
		 u1MHLPlugCurrent[i] = bMHLDeviceState(1);   
        if (u1OcpStatusCurrent[i] != u1OcpStatusOld[i]) {

#if 1
                u1OcpStatusOld[i] = u1OcpStatusCurrent[i];

                // turn Vbus off
                if (u1OcpReleaseCnt[i] == 0) {
                    fgSil_SetVBus(u1OcpStatusOld[i] ? 1:0);
                     //printf("vbus off..................\n");
                     mhl_oc_status.MhlOCStatus = u1OcpStatusOld[i] ? 0:1;
					 mhl_oc_status.MhlDeviceConnect = bMHLDeviceState(1)? 1:0;

                    if (rMhlOcpCbHandler) {
                        // notify to ap
                        rMhlOcpCbHandler(i, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
                        //printf("#####3:driver notify..................\n");
                        u1OcpFlag[i] = (u1OcpStatusOld[i] ? 0:1);
                    }

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

		if(u1MHLPlugCurrent[i] != u1MHLPlugOld[i])
		{
		     u1MHLPlugOld[i]   = u1MHLPlugCurrent[i];
			 mhl_oc_status.MhlOCStatus = 0;
			 mhl_oc_status.MhlDeviceConnect = u1MHLPlugOld[i];

            if (rMhlOcpCbHandler) {
                // notify to ap
                rMhlOcpCbHandler(i, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
                //printf("#####4:driver notify..................\n");
                u1OcpFlag[i] = (u1OcpStatusOld[i] ? 0:1);
            }
		}
        
        // turn Vbus off
        if (u1OcpReleaseCnt[i]) {
//            u1OcpReleaseCnt[i]--;
            if (--u1OcpReleaseCnt[i] == 0) {
                fgSil_SetVBus(u1OcpStatusOld[i] ? 1:0);
                printf("vbus ctrl..................%d\n",u1OcpStatusOld[i]);
				mhl_oc_status.MhlOCStatus = u1OcpStatusOld[i] ? 0:1;
			    mhl_oc_status.MhlDeviceConnect = bMHLDeviceState(1)? 1:0;

                if (rMhlOcpCbHandler) {
                    // notify to ap
                    rMhlOcpCbHandler(i, mhl_oc_status);//u1OcpStatusOld[i] ? 0:1);
                     //printf("#####1:driver notify..................\n");
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
      		    printf(" ocp timeout ...\n");
                  u1OcpStatusCurrent[i] =  0x40;   // FAULT PIN normal high
       	    u1OcpFlag[i] = 0;
			 mhl_oc_status.MhlOCStatus = 0;
		     mhl_oc_status.MhlDeviceConnect = bMHLDeviceState(1)? 1:0;
      	    
                  if (rMhlOcpCbHandler) {
                      // notify to ap, clear ocp status
	              rMhlOcpCbHandler(i, mhl_oc_status);
					 // printf("#####2:driver notify..................\n");
                  }       	    
            }
        }

    }
    
}

#ifdef RAP_OSD_NAME_ROLL_BACK

UINT8 uIsHaveAckFlag=0;
UINT8 uRapkFlag=0;
UINT32 _u4DevieAdapterID;
UINT8 u1IsNeedRemoveSonyDevice=1;
extern UINT8 buf1[16];

#endif


#ifndef RAP_OSD_NAME_ROLL_BACK

UINT8 uIsHaveAckFlag=0;
UINT8 uRapkFlag=0;
UINT32 _u4DevieAdapterID;
UINT8 u1IsNeedRemoveSonyDevice=1;
UINT32 u2RapPollingCnt=0;
UINT32 u2StartpollingCnt=0;
UINT8  u1wait5sensFlag=0;
BOOL _IsReceiveOsdName = FALSE;
UINT8 u1WaitRapKFlag=0;
UINT8 u1UnSupportCnt=0;
UINT8 _bNoAckCnt;
extern UINT8 buf1[16];
void vMHLRapPollingTimer(HANDLE_T hHandle,VOID* pTag)
{
   UNUSED(hHandle);
   UNUSED(pTag);
   UINT8 j=0;
  // _u4DevieAdapterID=0x0143;

   if(u1MhlGetCableStatus(1))
   {
      if(vIsSonyMHLDevice())
      {
		   u2StartpollingCnt++;
		   u2RapPollingCnt++;
		   if((u2RapPollingCnt %15 == 0)  || ((u2StartpollingCnt %5 ==0) && (u1wait5sensFlag == 0)) 
		   	  || (_bNoAckCnt >=1)|| (u1WaitRapKFlag==1) || ((u1wait5sensFlag==1) && (IsReiceiveContentOnFlag ==1)))
		   {  
		        if(u2RapPollingCnt %15 == 0)
		        {
		        	printf("______15s timer_______\n");
					u2RapPollingCnt=0;
		        }

				if((u2StartpollingCnt %5 ==0) && (u1wait5sensFlag == 0))
				{
					printf("______5s timer_______\n");
					u1wait5sensFlag=1;
					IsReiceiveContentOnFlag=0;
					//u2StartpollingCnt=1;
					u2RapPollingCnt=0;
				}

				if(_bNoAckCnt >=1)
				{
					printf("_____no ack cnt=%d_______\n",_bNoAckCnt);
				}

				if((u1wait5sensFlag==1) && (IsReiceiveContentOnFlag ==1))//from unlock to lock,start polling
				{
					IsReiceiveContentOnFlag=0;
					u1WaitRapKFlag=0;
					u2RapPollingCnt=0;
				}
				
				if(u1WaitRapKFlag ==1)
				{
					u1WaitRapKFlag =2;
				}
				if(u1WaitRapKFlag == 0)
				{
					fgSil_RapKeys(0);//rap polling
					//vUtDelay1s(1);  //wait 1s
					//uIsHaveAckFlag=0;
					u1WaitRapKFlag=1;
				}
				
				if(u1WaitRapKFlag == 1)
				{
					return;
				}
				if(u1WaitRapKFlag==2)
				{
				    u1WaitRapKFlag=0;
					if(uIsHaveAckFlag >= 1)//receive rapk
					{
					    uIsHaveAckFlag--;
						//u1IsNeedRemoveSonyDevice=0;
						_bNoAckCnt=0;
						//printf("[mhl]receive rapk from phone for rap polling\n");
						switch(IsRevRapK())
						{
							case 0x00://no error
								uRapkFlag=0;
								u1UnSupportCnt=0;
								u1IsNeedRemoveSonyDevice=0;
								printf("[mhl]receive rapk no error from phone for rap polling\n");
								if(_IsReceiveOsdName == FALSE)
								{
                                    if(_u4DevieAdapterID == 0x0143)
                                    {
                                    
										buf1[0]=0x01;
										buf1[1]=0x43;
										buf1[2]=0xA1;
                                    }
									
									if(_u4DevieAdapterID == 0x03A7)
									{
										buf1[0]=0x03;
										buf1[1]=0xA7;
										buf1[2]=0xA1;
									}

									x_memset(buf1+3, 0, sizeof(buf1)-3);
									
									for(j=0;j<16;j++)
									{
										printf("0x%x ",buf1[j]);
									}
									printf("\n");
									
									fgSil_SendWriteBurstData(1,buf1,16);//request osd name
									_IsReceiveOsdName=TRUE;
									//u1IsNeedRemoveSonyDevice=1;
									printf("[mhl]request osd name frome phone\n");
								}
								break;
							case 0x02://unspport
								uRapkFlag=0;
								u1UnSupportCnt++;
								if(u1UnSupportCnt >=2 ) //mobile lock
								{
									IsReiceiveContentOnFlag=0;
								}
								x_memset(buf1, 0, sizeof(buf1));
								u1IsNeedRemoveSonyDevice=1;
								break;
							case 0x03://busy
								x_memset(buf1, 0, sizeof(buf1));
								u1IsNeedRemoveSonyDevice=1;
								break;
							default:
								x_memset(buf1, 0, sizeof(buf1));
								u1IsNeedRemoveSonyDevice=1;
								break;
						}
						
					 }
					 else //no rapk
					 {
					 	uIsHaveAckFlag=0;
						u1WaitRapKFlag=0;
						_bNoAckCnt++;
					 	if(_bNoAckCnt > 3-1)//retry three times,always no ack,device connect off,notify ap for remove device from device list
					 	{
					 		_bNoAckCnt=0;
							uIsHaveAckFlag=0;
							uRapkFlag=0;
							u2RapPollingCnt=0;	
					        u1wait5sensFlag=1;
							_IsReceiveOsdName = FALSE;
							u1IsNeedRemoveSonyDevice=1;
							x_memset(buf1, 0, sizeof(buf1));
							u1ClearMHLDeviceAdapterID();
							printf("[mhl]no receive ack from phone for rap polling\n");
					 	}
					   
					 }
				}
		   }
      	}
   	}
	else
	{
		x_memset(buf1, 0, sizeof(buf1));
		_bNoAckCnt=0;
		uIsHaveAckFlag=0;
		uRapkFlag=0;
		u2RapPollingCnt=0;	
        u1wait5sensFlag=1;
		_IsReceiveOsdName = FALSE;
		u1IsNeedRemoveSonyDevice=1;
        u1ClearMHLDeviceAdapterID();
		printf("[mhl]no mhl cable connect\n");
	}

}
#endif
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

static UINT32 u4LoopCnt = 0;    
static UINT32 u4InitialDelay = 0;    
static UINT8 u1bgm_get_power_key = 0;
void bgm_get_power_key(void)
{
    u1bgm_get_power_key = 1;
}
extern BOOL pdwnc_is_semi_standby(void);

static VOID _HDMI_SwitchThread(VOID)
{
  UINT8 pdata[3] = {0};

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
  
    while (1)
    {
        /* Wait for HDMI timer to release the semaphore,    */
        /* then call the device event monitor.  Leave the   */
        /* semaphore locked so that we will start the wait  */
        /* again when we're done.                           */

        x_sema_lock( _hHDMIswitchSema, X_SEMA_OPTION_WAIT);

        if (pdwnc_is_semi_standby())
        {
            if (u1bgm_get_power_key)
            {
                //do nothing
            }
            else
            {
                continue;
            }
        }
        if (u1MhlGetPanelPower() == 0) 
        {
            // do nothing
        } 
        else if ( u4InitialDelay) 
        {
    	
            if (u4InitialDelay > SIL_INI_VBUS_RST_FREE) 
            {
                // 1: Hi-Z, 1: term off, 1: Vbus disable
                // 0: 1k, 0: term on, 0: vbus enable
                if (u4InitialDelay == SIL_INI_DELAY || (u4InitialDelay % 2 == 0)) 
                {
                    fgSil_MHLSignalControl(1, 1, 1);  // init VBUS off first
                }
            } 
            else if (u4InitialDelay <= SIL_INI_VBUS_RST_FREE) 
            {
                if (u4InitialDelay == SIL_INI_VBUS_RST_FREE || (u4InitialDelay % 2 == 0)) 
                {
                    fgSil_RecoveryMHLCable();  // connect Cable
                }
            }

            u4InitialDelay--;
       
        } 
        else if (u1MhlInit == 0) 
        {
            if ((u4LoopCnt % SIL_REINIT_CNT == 0) && (u4LoopCnt < SIL_INIT_TIMEOUT)) 
            {
        
                //fgSil_ChipRst(1,10);       
                //vSil_Delay1ms(10);   // 10 ms
              
                if ( fgSil_Sif_ReadBytes(SIL_DEV_PAGE_0, 0, pdata, 1) == SIL_OK) 
                {
  	  
                    fgSil_Set_MHL2_State(1,1);
                    fgSil_Set_3D_State( uIsSupport3D ? 1 : 0 );
                    SI_3D_EDID_Configuration_VIC_DTD(&p3D_VIC_Struct,&p3D_DTD_Struct);
                    fgSil_ChipRst(1,10);
                    fgSil_NTF_VBUS_High();
                    fgSil_Init();
        
                    // get chip ID
                    fgSil_Get_Chip_ID(prSilVersion);

                    // firmware version
                    fgSil_Get_FW_Version(prSilFwVer);
                    printf("fgSil_Get_FW_Version %d.%d.%d \n",prSilFwVer->major,prSilFwVer->second, prSilFwVer->third);

                    // status
                    fgSil_GetStatus(SIL_PORT_1,prSilStatus);    
                
                    u4LoopCnt = 0;
               
                    u1MhlInit = 1;
  	         
                    printf(" mhl bridge init ok.... %d\n", u4LoopCnt);
                } 
                else 
                {
                    printf(" mhl bridge rst fail %d....\n",u4LoopCnt);  	     
                    fgSil_ChipRst(1,5);
                }

            }

            if (u4LoopCnt < SIL_INIT_TIMEOUT + 1)
            {
                u4LoopCnt++;
            }

        } 
        else 
        { 

            // every 100ms
            u4LoopCnt++;
            if (( u4LoopCnt % SIL_LOOP_COUNTER) == 0) 
            {
                if ((u4LoopCnt%SIL_LOOP_MONITOR_CNT) == 0) 
                {
                    LOG(5,"MHL u4LoopCnt %d ,init: %d\n",u4LoopCnt, u1MhlInit);
                }
                vSil_MainLoop();
            }

            // ocp handler
            vOcpHandler();

        }
       
        
    }    

}

extern UINT8 HDMIEDID_table[256];
extern void vDrvLoadHdmiEdid2Table(void);
void vMHLParsingEDIDForMHL3D(UINT8 *Edid_table)
{
  
   UINT16 uEdidVSDBOffset=0,uEdidSVDOffset=0;
   UINT8 VIC=0;
   UINT16 i=0;
   UINT8 uVSDBLen=0,DTDlength=0;
   UINT8 uTagCode;
   UINT8 uLength;
   UINT8 index=0;
   INT32 PixClk=0;
   UINT8 u1DTDOffSet=0;
   UINT8 u1SupportLR=0;
   UINT8 u1SupportTB=0;
   UINT8 u1SupportFS=0;
   	
   // for mhl is support 3d;
	for(i=0x80; i< 0x100; i++)
	{

	    if ((Edid_table[i] == 0x3) && \
	    	(Edid_table[i+1] == 0xc) && \
	    	 (Edid_table[i+2] == 0x0))
	    {
	      uEdidVSDBOffset += (i-1);
		  
		  if( uEdidVSDBOffset < 255)
		  {
		  	//return;
		  }
		  else
		  {
		  		return;
		  }
	    }
	}

    uVSDBLen=Edid_table[uEdidVSDBOffset]& 0x1f;
	if(uVSDBLen < 8)
	{
		uIsSupport3D=0;//not support 3D
	}
	else if(uVSDBLen >= 8)
	{
		if(uEdidVSDBOffset + 8 < 255)
		{
			
		}
		else
		{
			return;
		}

		if(((uEdidVSDBOffset + 8 + (((Edid_table[uEdidVSDBOffset + 8]) >> 7)*2)) + 
        (((Edid_table[uEdidVSDBOffset + 8] & 0x7f) >>6)*2) + ((Edid_table[uEdidVSDBOffset + 8] & 0x3f)>>5)) < 255)
		{
			
		}
		else
		{
			return;
		}
		
		if(((Edid_table[uEdidVSDBOffset + 8] & 0x3f)>>5) > 0)
		{
			uIsSupport3D = Edid_table[((uEdidVSDBOffset + 8 + (((Edid_table[uEdidVSDBOffset + 8]) >> 7)*2)) + 
                                (((Edid_table[uEdidVSDBOffset + 8] & 0x7f) >>6)*2) + ((Edid_table[uEdidVSDBOffset + 8] & 0x3f)>>5)) ]>>7;
		}
		else
		{
		    uIsSupport3D=0;
		}
	}
	printf("___vsdblen=%d,uIsSupport3D=%d_______\n",uVSDBLen,uIsSupport3D);

	//for mhl edid svd
   i=0x84;
   while(i < 255)
   {
   
		uTagCode = Edid_table[i] >> 5;
		uLength  = Edid_table[i] & 0x1f;
		if( uTagCode == 2)
		{
			uEdidSVDOffset = i;
			break;
		}
		else
		{
		    i += (uLength+1);
		}
		
    }
    printf("____svd uLength=0x%x______\n",uLength);
	for(index = 0; index < uLength; index++)
	{
		if(uEdidSVDOffset + 1 + index < 255 )
		{
			
		}
		else
		{
			return;
		}

		VIC = Edid_table[uEdidSVDOffset + 1 + index] & 0x7f;
		//printf("____vic=0x%x______\n",VIC);

		switch(VIC)
		{
			case 32://1080p/24
			case 4: //720p/60
			case 19://720p/50
			case 5: //1080i/60
			case 20://1080i/50
				u1SupportLR=1;
				u1SupportTB=1;
				u1SupportFS=0;
				break;
		   case 34://1080p/30
		   		u1SupportLR=0;
				u1SupportTB=1;
				u1SupportFS=0;
				break;
			default:
				u1SupportLR=0;
				u1SupportTB=0;
				u1SupportFS=0;
				break;
		}

		printf("_____vic=%d,u1SupportLR=%d,u1SupportTB=%d_______\n",VIC,u1SupportLR,u1SupportTB);
		if(index == 0)
		{
			p3D_VIC_Struct.EDID_1st_VIC=1;
			p3D_VIC_Struct.EDID_1st_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_1st_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_1st_VIC_TB=u1SupportTB;
		 }
		else if(index == 1)
		{

			p3D_VIC_Struct.EDID_2nd_VIC=1;
			p3D_VIC_Struct.EDID_2nd_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_2nd_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_2nd_VIC_TB=u1SupportTB;

		}
		else if(index == 2)
		{

			p3D_VIC_Struct.EDID_3rd_VIC=1;
			p3D_VIC_Struct.EDID_3rd_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_3rd_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_3rd_VIC_TB=u1SupportTB;

		}
		else if(index == 3)
		{
			p3D_VIC_Struct.EDID_4th_VIC=1;
			p3D_VIC_Struct.EDID_4th_VIC_FS=0;
			p3D_VIC_Struct.EDID_4th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_4th_VIC_TB=u1SupportTB;


		}
		else if(index == 4)
		{
			p3D_VIC_Struct.EDID_5th_VIC=1;
			p3D_VIC_Struct.EDID_5th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_5th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_5th_VIC_TB=u1SupportTB;

		}
		else if(index == 5)
		{
			p3D_VIC_Struct.EDID_6th_VIC=1;
			p3D_VIC_Struct.EDID_6th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_6th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_6th_VIC_TB=u1SupportTB;
		}
		else if(index == 6)
		{
			p3D_VIC_Struct.EDID_7th_VIC=1;
			p3D_VIC_Struct.EDID_7th_VIC_FS=0;
			p3D_VIC_Struct.EDID_7th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_7th_VIC_TB=u1SupportTB;	
		}
		else if(index == 7)
		{
			p3D_VIC_Struct.EDID_8th_VIC=1;
			p3D_VIC_Struct.EDID_8th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_8th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_8th_VIC_TB=u1SupportTB;
		}
		else if(index == 8)
		{
			p3D_VIC_Struct.EDID_9th_VIC=1;
			p3D_VIC_Struct.EDID_9th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_9th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_9th_VIC_TB=u1SupportTB;

		}
		else if(index == 9)
		{
			p3D_VIC_Struct.EDID_10th_VIC=1;
			p3D_VIC_Struct.EDID_10th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_10th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_10th_VIC_TB=u1SupportTB;
		}
		else if(index == 10)
		{
			p3D_VIC_Struct.EDID_11th_VIC=1;
			p3D_VIC_Struct.EDID_11th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_11th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_11th_VIC_TB=u1SupportTB;

		}
		else if(index == 11)
		{
			p3D_VIC_Struct.EDID_12th_VIC=1;
			p3D_VIC_Struct.EDID_12th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_12th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_12th_VIC_TB=u1SupportTB;


		}
		else if(index == 12)
		{
			p3D_VIC_Struct.EDID_13th_VIC=1;
			p3D_VIC_Struct.EDID_13th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_13th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_13th_VIC_TB=u1SupportTB;

		}
		else if(index == 13)
		{
			p3D_VIC_Struct.EDID_14th_VIC=1;
			p3D_VIC_Struct.EDID_14th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_14th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_14th_VIC_TB=u1SupportTB;


		}
		else if(index == 14)
		{
			p3D_VIC_Struct.EDID_15th_VIC=1;
			p3D_VIC_Struct.EDID_15th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_15th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_15th_VIC_TB=u1SupportTB;

		}
		else if(index == 15)
		{
			p3D_VIC_Struct.EDID_16th_VIC=1;
			p3D_VIC_Struct.EDID_16th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_16th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_16th_VIC_TB=u1SupportTB;
		}
		else if(index == 16)
		{
			p3D_VIC_Struct.EDID_17th_VIC=1;
			p3D_VIC_Struct.EDID_17th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_17th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_17th_VIC_TB=u1SupportTB;

		}
		else if(index == 17)
		{
			p3D_VIC_Struct.EDID_18th_VIC=1;
			p3D_VIC_Struct.EDID_18th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_18th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_18th_VIC_TB=u1SupportTB;


		}
		else if(index == 18)
		{
			p3D_VIC_Struct.EDID_19th_VIC=1;
			p3D_VIC_Struct.EDID_19th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_19th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_19th_VIC_TB=u1SupportTB;

		}
		else if(index == 19)
		{
			p3D_VIC_Struct.EDID_20th_VIC=1;
			p3D_VIC_Struct.EDID_20th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_20th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_20th_VIC_TB=u1SupportTB;


		}
		else if(index == 20)
		{
			p3D_VIC_Struct.EDID_21th_VIC=1;
			p3D_VIC_Struct.EDID_21th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_21th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_21th_VIC_TB=u1SupportTB;

		}
		else if(index == 21)
		{
			p3D_VIC_Struct.EDID_22th_VIC=1;
			p3D_VIC_Struct.EDID_22th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_22th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_22th_VIC_TB=u1SupportTB;


		}
		else if(index == 22)
		{
			p3D_VIC_Struct.EDID_23th_VIC=1;
			p3D_VIC_Struct.EDID_23th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_23th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_23th_VIC_TB=u1SupportTB;

		}
		else if(index == 23)
		{
			p3D_VIC_Struct.EDID_24th_VIC=1;
			p3D_VIC_Struct.EDID_24th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_24th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_24th_VIC_TB=u1SupportTB;


		}
		else if(index == 24)
		{
			p3D_VIC_Struct.EDID_25th_VIC=1;
			p3D_VIC_Struct.EDID_25th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_25th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_25th_VIC_TB=u1SupportTB;

		}
		else if(index == 25)
		{
			p3D_VIC_Struct.EDID_26th_VIC=1;
			p3D_VIC_Struct.EDID_26th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_26th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_26th_VIC_TB=u1SupportTB;
		}
		else if(index == 26)
		{
			p3D_VIC_Struct.EDID_27th_VIC=1;
			p3D_VIC_Struct.EDID_27th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_27th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_27th_VIC_TB=u1SupportTB;

		}
		else if(index == 27)
		{
			p3D_VIC_Struct.EDID_28th_VIC=1;
			p3D_VIC_Struct.EDID_28th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_28th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_28th_VIC_TB=u1SupportTB;


		}
		else if(index == 28)
		{
			p3D_VIC_Struct.EDID_29th_VIC=1;
			p3D_VIC_Struct.EDID_29th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_29th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_29th_VIC_TB=u1SupportTB;

		}
		else if(index == 29)
		{
			p3D_VIC_Struct.EDID_30th_VIC=1;
			p3D_VIC_Struct.EDID_30th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_30th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_30th_VIC_TB=u1SupportTB;


		}
		else if(index == 30)
		{
			p3D_VIC_Struct.EDID_31th_VIC=1;
			p3D_VIC_Struct.EDID_31th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_31th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_31th_VIC_TB=u1SupportTB;

		}
		else if(index == 31)
		{
			p3D_VIC_Struct.EDID_32th_VIC=1;
			p3D_VIC_Struct.EDID_32th_VIC_FS=u1SupportFS;
			p3D_VIC_Struct.EDID_32th_VIC_LR=u1SupportLR;
			p3D_VIC_Struct.EDID_32th_VIC_TB=u1SupportTB;
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

   printf("_____DTDlength in block0=%d_______\n",DTDlength);
   
 
   for(index=0;index < DTDlength;index++)
   {
   	
		if(0x36 + index*18 +1 < 255 )
		{
			
		}
		else
		{
			return;
		}

		PixClk = Edid_table[0x36 + index*18 +1 ]<<8; //*(uEdidPtr + 0x36 +index*18 +1 ) << 8;
		PixClk |= Edid_table[0x36 + index*18];//*(uEdidPtr + 0x36 +index*18);


	if(PixClk >= 2700 && PixClk <= 14850)
	{
		if((PixClk == 7425))
		{
			u1SupportLR=1;
			u1SupportTB=1;
			u1SupportFS=0;
		}
		
		else if(PixClk > 0)
		{
			u1SupportLR=0;
			u1SupportTB=0;
			u1SupportFS=0;
		}
		
		
		printf("_____block0 PixClk=%d,u1SupportLR=%d,u1SupportTB=%d_______\n",PixClk,u1SupportLR,u1SupportTB);
		//printf("_______u1SupportLR=%d,u1SupportTB=%d",u1SupportLR,u1SupportTB);
		if(index == 0)
		{
			p3D_DTD_Struct.EDID_1st_DTD=1;
			p3D_DTD_Struct.EDID_1st_DTD_LR=u1SupportLR;
			p3D_DTD_Struct.EDID_1st_DTD_TB=u1SupportTB;
			p3D_DTD_Struct.EDID_1st_DTD_FS=u1SupportFS;

		}
		else if(index == 1)
		{
			p3D_DTD_Struct.EDID_2nd_DTD=1;
			p3D_DTD_Struct.EDID_2nd_DTD_LR=u1SupportLR;
			p3D_DTD_Struct.EDID_2nd_DTD_TB=u1SupportTB;
			p3D_DTD_Struct.EDID_2nd_DTD_FS=u1SupportFS;


		}
		else if(index == 2)
		{
			p3D_DTD_Struct.EDID_3rd_DTD=1;
			p3D_DTD_Struct.EDID_3rd_DTD_LR=u1SupportLR;
			p3D_DTD_Struct.EDID_3rd_DTD_TB=u1SupportTB;
			p3D_DTD_Struct.EDID_3rd_DTD_FS=0;

		}
		else if(index == 3)
		{
			p3D_DTD_Struct.EDID_4th_DTD=1;
			p3D_DTD_Struct.EDID_4th_DTD_LR=u1SupportLR;
			p3D_DTD_Struct.EDID_4th_DTD_TB=u1SupportTB;
			p3D_DTD_Struct.EDID_4th_DTD_FS=u1SupportFS;

		}
	 }
   }
	

   //block1
  u1DTDOffSet=Edid_table[0x82];
  if(DTDlength == 2 )
  {
  	  for(index=0; index < 4; index++)
	  {
			if(0x80 + u1DTDOffSet + index*18 +1 < 255)
			{
				
			}
			else
			{
				return;
			}

			PixClk = Edid_table[0x80 + u1DTDOffSet + index*18 +1] <<8;//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18 + 1 ) << 8;
			PixClk |= Edid_table[0x80 + u1DTDOffSet + index*18];//*(uEdidPtr + 0x80 + *(uEdidPtr + 0x82) + index*18);


		if(PixClk >= 2700 && PixClk <= 14850)
		{
			if((PixClk == 7425))
			{
				u1SupportLR=1;
				u1SupportTB=1;
				u1SupportFS=0;
			}
			else 
			{
				u1SupportLR=0;
				u1SupportTB=0;
				u1SupportFS=0;
			}
		

			printf("_____block1 PixClk=%d,u1SupportLR=%d,u1SupportTB=%d_______\n",PixClk,u1SupportLR,u1SupportTB);
			if(index == 0)
			{
				p3D_DTD_Struct.EDID_3rd_DTD=1;
				p3D_DTD_Struct.EDID_3rd_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_3rd_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_3rd_DTD_FS=u1SupportFS;

			}
			else if(index == 1)
			{
				p3D_DTD_Struct.EDID_4th_DTD=1;
				p3D_DTD_Struct.EDID_4th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_4th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_4th_DTD_FS=u1SupportFS;


			}
			else if(index == 2)
			{
				p3D_DTD_Struct.EDID_5th_DTD=1;
				p3D_DTD_Struct.EDID_5th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_5th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_5th_DTD_FS=u1SupportFS;
			}
			else if(index == 3)
			{
				p3D_DTD_Struct.EDID_6th_DTD=1;
				p3D_DTD_Struct.EDID_6th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_6th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_6th_DTD_FS=u1SupportFS;
			} 
	    }
  	  }
  	}
    else if(DTDlength == 3)
    {
    	for(index=0; index < 4; index++)
		{
			if(0x80 + u1DTDOffSet + index*18 +1 < 255)
			{
				
			}
			else
			{
				return;
			}
			PixClk = Edid_table[0x80 + u1DTDOffSet + index*18 +1] <<8;
			PixClk |= Edid_table[0x80 + u1DTDOffSet + index*18];


		if(PixClk >= 2700 && PixClk <= 14850)
	    {
			if((PixClk == 7425) )
			{
				u1SupportLR=1;
				u1SupportTB=1;
				u1SupportFS=0;
			}
			else if(PixClk > 0)
			{
				u1SupportLR=0;
				u1SupportTB=0;
				u1SupportFS=0;
			}
			

			printf("_____block1 PixClk=%d,u1SupportLR=%d,u1SupportTB=%d_______\n",PixClk,u1SupportLR,u1SupportTB);
			if(index == 0)
			{
				p3D_DTD_Struct.EDID_4th_DTD=1;
				p3D_DTD_Struct.EDID_4th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_4th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_4th_DTD_FS=u1SupportFS;
			}
			else if(index == 1)
			{
				p3D_DTD_Struct.EDID_5th_DTD=1;
				p3D_DTD_Struct.EDID_5th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_5th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_5th_DTD_FS=u1SupportFS;
			}
			else if(index == 2)
			{
				p3D_DTD_Struct.EDID_6th_DTD=1;
				p3D_DTD_Struct.EDID_6th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_6th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_6th_DTD_FS=u1SupportFS;
			}
			else if(index == 3)
			{
				p3D_DTD_Struct.EDID_7th_DTD=1;
				p3D_DTD_Struct.EDID_7th_DTD_LR=u1SupportLR;
				p3D_DTD_Struct.EDID_7th_DTD_TB=u1SupportTB;
				p3D_DTD_Struct.EDID_7th_DTD_FS=u1SupportFS;

			}	 
     	}
      }
 	}
}

    
void fgSil_Enable_CBUS(BOOL bEn)
{
	INT32 i4Ret=0;
	UINT8 u1ValTmp = 0;

	i4Ret= SIF_X_Read(4,0x100,0xD0,1,0x87,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____read error_____0xD0,0x87\n");
	}

	if(bEn)
	{
		u1ValTmp &= ~0x0C; //set to 1K ohm
	}
	else
	{
		u1ValTmp |= 0x0C;
	}

	i4Ret=SIF_X_Write(4,0x100,0xD0,1,0x87,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____write error_____0xD0,0x87\n");
	}

    i4Ret= SIF_X_Read(4,0x100,0xD0,1,0x76,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____read error_____0xD0,0x76\n");
	}

	if(bEn)
	{
		u1ValTmp &= ~0x0C; //set to 1K ohm
	}
	else
	{
		u1ValTmp |= 0x0C;
	}

	i4Ret=SIF_X_Write(4,0x100,0xD0,1,0x76,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____write error_____0xD0,0x76\n");
	}


	i4Ret= SIF_X_Read(4,0x100,0xD0,1,0x73,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____read error_____0xD0,0x73\n");
	}

	if(bEn)
	{
		u1ValTmp &= ~0x0C; //set to 1K ohm
	}
	else
	{
		u1ValTmp |= 0x0C;
	}

	i4Ret=SIF_X_Write(4,0x100,0xD0,1,0x73,&u1ValTmp,1);

	if(i4Ret == 0)
	{
		Printf("_____write error_____0xD0,0x73\n");
	}
}

/* 
@brief : When TV power on, FW will call this function once. 
*/     

void vMhlInit(void)
{

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

	    GPIO_Input(MHL_GPIO_WAKEUP);
     //fgSil_ChipRst(1,10);
     vDrvLoadHdmiEdid2Table();
	 vMHLParsingEDIDForMHL3D(HDMIEDID_table);

	 fgSil_Enable_CBUS(FALSE);
     u4InitialDelay = SIL_INI_DELAY;

      // 10 ms delay after 1292 HW reset
      vSil_Delay1ms(10);
}


void vMhlGo(void)
{

  if (u1MhlInit == 1) {
  	printf("mhl bridge re-inited !!! \n");
  }
  
   // plan to run all mhl staff are running on it's own thread.

    /* Create HDMI switch timer semaphore.  */
    x_sema_create(&_hHDMIswitchSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);

    /* Create HDMI Control thread.   */

    x_thread_create(&_hHDMIswitchThread, "HDMI_switch_thread", 1024, 200,
        (x_os_thread_main_fct)_HDMI_SwitchThread, 0, NULL);

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

}

UINT8 bSendRcpKeys(UINT8 Key, UINT8 event)
{
#if 1 // for testing
    CBUS_MSG_DESCRIPTION_T arMsg;
    UINT8 buf[16] = {0, 1, 2, 3, 4, 5 ,6 ,7 , 8, 9, 10, 11, 12, 13, 14, 15};
	
     if (Key == 0xff)
     {
         arMsg.MscMsg = 0x68;
         arMsg.Opcode = 0x20;
         arMsg.Size = 1;
         arMsg.Data[0] = event;
         vCbus_NotifyRxResult(&arMsg);
     } else if (Key == 0xfe) {
         fgSil_SetVBus(event);
     } else if (Key == 0xfd) {
         fgSil_SendWriteBurstData(event,buf,16);
     } else if (Key == 0xfc) {
        fgSil_RapKeys(0x10);
     } else if (Key == 0xfb) {
         printf("OCP status %d\n",fgSil_GetOCPStatus(0));
     }else if(Key == 0xfa){
//     	u1IsNeedRemoveSonyDevice = event;
     }else 
#endif
     {
         fgSil_RcpKeys(Key,event);
     }
     
     printf("fgSil_RcpKeys 0x%x, event: %d\n", Key, event);
     return 1;
}

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


BOOL fgSil_NFYRcp(UINT8 key)
{
    printf(">>> nfy RCP key 0x%2x\n", key);
    return(SIL_OK);
}


BOOL fgSil_NFYRAP(UINT8 key)
{
    CBUS_MSG_DESCRIPTION_T arMsg;
	
    arMsg.MscMsg = 0x68;
    arMsg.Opcode = 0x20;
    arMsg.Size = 1;
    arMsg.Data[0] = key;
	#ifndef RAP_OSD_NAME_ROLL_BACK
	if( (key == 0x11) && vIsSonyMHLDevice())//content off
	{
		fgSil_RapKeys(0);//rap polling
		uIsHaveAckFlag=0;
		vUtDelay1s(1);  //wait 1s
				
		if(uIsHaveAckFlag == 0)//device connect off,remove sony device from device list
		{
		    uIsHaveAckFlag=0;
			u1IsNeedRemoveSonyDevice=1;
			//.....
		}
		else
		{
			if(IsRevRapK())//need remove the sony mhl device list
			{
			  u1IsNeedRemoveSonyDevice=1;
			}
		}
	}else if((key == 0x10) && vIsSonyMHLDevice())
	{
		IsReiceiveContentOnFlag=1;
	}
	#endif
	if(((key == 0x11) && (eHDMICurrSwitch == HDMI_SWITCH_3)) || (key == 0x10) )
	{
		vCbus_NotifyRxResult(&arMsg);
	}

    printf(">>> nfyrap 0x%2x ..... \n", key);
   
    return(SIL_OK);
}

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

BOOL IsRevRapK(void)
{
   return uRapkFlag;
}
BOOL fgSil_NFYWriteBurstData(UINT8 key, UINT8 *pdata, UINT8 size)
{
   UINT8 i = 0;

    printf(">>> nfy scratchpad key 0x%2x\n", key);
   for (i = 0 ; i < 16 && i < size ; i++)
   {
       printf("0x%2x, ", pdata[i]);
	   buf1[i]=pdata[i];
   }
   u1IsNeedRemoveSonyDevice=0;
   printf("\n");
   return(SIL_OK);
}

void MHL_SetOCPCallback(x_vbus_drv_nfy_ocp_status nfyfp)
{
    rMhlOcpCbHandler = nfyfp;
}

BOOL fgSil_NFYOCPStatus(UINT8 port, UINT8 result)
{
   u1OcpStatusCurrent[0] =  result;
   if (result) {
   	if ((u1OcpReleaseCnt[0] > SIL_OCP_FREE_CNT) || (u1OcpReleaseCnt[0] == 0)) {
   	    u1OcpReleaseCnt[0] = SIL_OCP_FREE_CNT;
   	    // reset ocp timeout timer
   	    u1OcpTimeout[0] = 0;
   	}
   } else {
       if ((u1OcpReleaseCnt[0] > SIL_OCP_DELAY_CNT) ||(u1OcpReleaseCnt[0] == 0)) {   
   	    u1OcpReleaseCnt[0] = SIL_OCP_DELAY_CNT;
       }
   }
   
   printf("Sil_NFYOCP port:%d, status: %d, relCnt \n", port, result, u1OcpReleaseCnt[0]);
   
   return(SIL_OK);
}
void fgSil_NTFDevCap(HDMI_SIL_DevCap_STATUS_T *pHDMI_SIL_DevCap_STATUS)
{
	_u4DevieAdapterID =  pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_H;
	_u4DevieAdapterID = ( _u4DevieAdapterID << 8) | pHDMI_SIL_DevCap_STATUS->DevCapValue_ADOPTER_ID_L;

	printf("______nfy DevCap,adapter id =0x%x_______\n",_u4DevieAdapterID);
	#ifndef RAP_OSD_NAME_ROLL_BACK
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
	#endif
}

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


UINT8 u1MhlGetPortId(UINT8 ports)
{
    return (MHL_PORT_ID);
}

UINT8 u1MhlGetCableStatus(UINT8 ports)
{
    return (rSilStatus.RxCableDetected ? 1:0);
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

void u1ClearMHLDeviceAdapterID(VOID)
{
	_u4DevieAdapterID =0;
}

UINT8 u1MhlGetOcpStatus(UINT8 ports)
{
    return ((ports < MHL_PORT_NUM) ? u1OcpFlag[ports] : 0);
}


static UINT8 u1PanelPower = 0;

void vMhlSetRst(void)
{
    fgSil_ChipRst(1,1);
}

void vMhlSetPanelPower(UINT8 val)
{
    u1PanelPower = val;
    if (val) {
        vSil_Delay1ms(10);
        fgSil_MHLSignalControl(1, 1, 1);
    }
}

UINT8 u1MhlGetPanelPower(void)
{
   UNUSED(u1PanelPower);
   return (1);
   //return (u1PanelPower);  // for SOEM Model only, power on CBUS switch on/off timing issue.
}
	
UINT8 u1SourceStatus = MHL_RAP_NOERROR;
void vMhlSetSourceStatus(UINT8 port, UINT8 status)
{
    u1SourceStatus = status;
	printf("_____u1SourceStatus=%d_________\n",status);
}

UINT8 u1RapOnOff = 1;
void vMhlSetRapOnOff(UINT8 port, UINT8 OnOff)
{
    u1RapOnOff = OnOff;
	printf("_____u1RapOnOff=%d______\n",u1RapOnOff);
}

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
static UINT8 uRapContentOn=0;
void vMhlSetRapContentOn(UINT8 OnOff)
{
  uRapContentOn=OnOff;
}

UINT8 vGetMhlRapContentON(void)
{
    return uRapContentOn;
}
void fgSil_NTF_HWRST_1292(void)
{
	fgSil_ChipRst(1,1);
}

void fgSil_NTF_VBUS_High(void)
{
    INT32 val=0;
	GPIO_Output(MHL_GPIO_VBUS, &val);
}

void fgSil_NTF_VBUS_Low(void)
{
    INT32 val=1;
	GPIO_Output(MHL_GPIO_VBUS, &val);
}

