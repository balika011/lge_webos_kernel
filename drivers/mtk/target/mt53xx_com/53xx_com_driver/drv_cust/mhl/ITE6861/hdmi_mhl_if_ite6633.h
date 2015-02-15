#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
//#include "typedef.h"
#include "IT66332_Customer/IT66332.h"



extern void vHDMIMHL_Init(void);


extern void vMhl_MainLoop(void);


extern iTE_u1 i2c_read_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8 *p_data,iTE_u8 device);

extern iTE_u1 i2c_write_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8 *p_data,iTE_u8 device);

extern void Delay_ms(iTE_u16 Delay_Cnt);


extern void SendRcpKey(UINT8 key);


#endif
#endif


