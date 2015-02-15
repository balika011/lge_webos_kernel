#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
#include "../../mhl/mhl_if.h"
#include "IT66332_Customer/IT66332.h"

void vHDMIMHL_Init(void)
{
	while(iTE_FALSE == IT66332_Init());
}

void vMhl_MainLoop(void)
{
   // UNUSED(i4Gpio);
   // UNUSED(fgStatus);
	IT66332_Irq();
}

iTE_u1 i2c_read_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8 *p_data,iTE_u8 device)
{
    BOOL bRet=0;

	bRet =Sif_ReadBytes(address,offset,p_data,byteno);
	if(bRet == SIL_OK)
		return 1;
	else
		return 0;
}

iTE_u1 i2c_write_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8 *p_data,iTE_u8 device)
{
    BOOL bRet=0;
	bRet =Sif_WriteBytes(address,offset,p_data,byteno);
	if(bRet == SIL_OK)
		return 1;
	else
		return 0;
}

#if 0
void Delay_ms(iTE_u16 Delay_Cnt)
{
	Delay1ms((UINT32)Delay_Cnt);
}
#endif

void Delay_ms(UINT32 Delay_Cnt)
{
	Delay1ms((UINT32)Delay_Cnt);
}


void SendRcpKey(UINT8 key)
{
	IT66332_MscRcpSend(key);
}
#endif
#endif


