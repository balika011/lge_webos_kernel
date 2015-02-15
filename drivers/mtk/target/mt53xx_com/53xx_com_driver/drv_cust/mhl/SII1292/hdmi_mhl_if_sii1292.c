#include "drv_sil_if.h"


void vHDMIMHL_Init(void)
{
    fgSil_Init();
}


void vMhl_MainLoop(void)
{
	vSil_MainLoop();
}



#ifdef CECNFY5VSTATUS
extern void _Cec_Notify5VStatus(UINT8 u1Port, UINT8 u15vStatus);
#endif

void fgSil_NTF_CBUS_Connected(void)
{
	u1fgSilCbusStatus = SIL_CBUS_CONNECTED;
#ifdef CECNFY5VSTATUS
	_Cec_Notify5VStatus(3,1);//MHL SOURCE 
#endif

}
void fgSil_NTF_CBUS_Disconnected(void)
{
	u1fgSilCbusStatus = SIL_CBUS_DISCONECTED;
#ifdef CECNFY5VSTATUS
	_Cec_Notify5VStatus(3,0);//MHL SOURCE 
#endif
	
}
void fgSil_NTF_3D_Communicated(void)
{
	
}

void fgSil_NTF_VBUS_High(void)
{
	
}
void fgSil_NTF_VBUS_Low(void)
{
	
}

extern BOOL Sif_WriteBytes(UINT8 DeviceID,UINT8 Offset,UINT8 *pData,UINT8 ByteCount);
extern BOOL Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount);
extern void Delay1ms(UINT32 ms);
extern void Delay1us(UINT32 us);

BOOL fgSil_Sif_WriteBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount)
{
	return Sif_WriteBytes(DeviceID,Offset,pData,ByteCount);

}

BOOL fgSil_Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount)
{
	return Sif_ReadBytes(DeviceID,Offset,pData,ByteCount);
}

void SendRcpKey(UINT8 key)
{
	fgSil_RcpKeys(key);
}

void vSil_Delay1ms(UINT32 ms)   // Delay in millisecond
{
	Delay1ms(ms);
}
void vSil_Delay1us(UINT32 us)   // Delay in microsecond
{
	Delay1us(us);
}





