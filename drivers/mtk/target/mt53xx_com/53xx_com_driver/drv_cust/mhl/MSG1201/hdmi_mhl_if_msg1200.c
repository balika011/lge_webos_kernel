#include "sif_if.h"
#include "hdmi_mhl_if_msg1200.h"
#include "Eland/drvMHL_ELAND_st.h"

extern void devMHL_Initialization(void);
void vHDMIMHL_Init(void)
{
	devMHL_Initialization();
}

extern void devMHL_Handler(void);
void vMhl_MainLoop(void)
{
   // UNUSED(i4Gpio);
   // UNUSED(fgStatus);
	devMHL_Handler();
}

BOOL mhl_IIC_WriteBytes(U16 u16BusNumSlaveID, U8 u8addrcount, U8* pu8addr, U16 u16size, U8* pu8data)
{
	INT32 i4ReturnValue;  
	UINT32 u4Addr=0;
	if(u8addrcount == 1)
	{
		u4Addr = pu8addr[0];
	}
	else if(u8addrcount == 2)
	{
		u4Addr = pu8addr[0];
		u4Addr = (u4Addr<<8) | pu8addr[1];
	}
	
	i4ReturnValue = SIF_X_Write(MHL_IIC_BUS, MHL_IIC_RATE,u16BusNumSlaveID & 0x00ff, u8addrcount ,u4Addr, pu8data, u16size);
	

    if (i4ReturnValue > 0)
    {
    	 return SIL_OK;
    }
    else
    {
   	    //printf("msg1201 write fail\n");           
        return SIL_FAIL;
    }
}

BOOL mhl_IIC_ReadBytes(U16 u16BusNumSlaveID, U8 u8AddrNum, U8* paddr, U16 u16size, U8* pu8data)
{
	INT32 i4ReturnValue;
	UINT32 u4Addr=0;
	if(u8AddrNum == 1)
	{
		u4Addr = paddr[0];
	}
	else if(u8AddrNum == 2)
	{
		u4Addr = paddr[0];
		u4Addr = (u4Addr<<8) | paddr[1];
	}
	
    i4ReturnValue = SIF_X_Read(MHL_IIC_BUS,MHL_IIC_RATE, u16BusNumSlaveID & 0x00ff, u8AddrNum, u4Addr, pu8data, u16size);

    if (i4ReturnValue>0)
    {
   	  return SIL_OK;
    }
    else
    {   		
     // printf("msg1201 read fail\n");
	  return SIL_FAIL;
    }
}

#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
extern UINT32 Timer_Get(void);
extern void Delay1ms(UINT32 ms);
extern void vReset_High(void);
extern void vReset_Low(void);
#endif
#endif

UINT32 MHL_Timer_Get(void)
{
#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
	return Timer_Get();
#endif
#else
	return 0;
#endif
}

void msAPI_Timer_Delayms(UINT32 ms)
{
#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
	Delay1ms(ms);
#endif
#endif
}


void MHL_Reset_High(void)
{
#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
	vReset_High();
#endif
#endif
}

void MHL_Reset_Low(void)
{
#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH

	vReset_Low();
#endif
#endif
}


BOOL vGetMHLCableDetect(void)
{
	return GET_MHL_CABLE_PLUGGED();
}
BOOL vGetMHLCbusConnect(void)
{
	return GET_MHL_CBUS_CONNECT();
}

void SendRcpKey(UINT8 key)
{
	mapi_mhl_SendRCP_PressRelease_Cmd(key);
}



