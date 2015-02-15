#include "x_typedef.h"
#include "x_os.h"
#include "typedef.h"

#define MS_BOOL BOOL
#define MS_U8   UINT8
#define MS_U16  UINT16
#define MS_U32  UINT32

#define U8   UINT8
#define U16  UINT16

#define SIL_OK 1
#define SIL_FAIL 0

#define SIL_TRUE  TRUE
#define SIL_FALSE FALSE


#ifndef MHL_IIC_BUS
#define MHL_IIC_BUS    0
#endif

#ifndef MHL_IIC_RATE
#define MHL_IIC_RATE 0x100
#endif

#define MHL_ELAND_COMMON 0xC8
#define MHL_ELAND_CONFIG 0xD0


extern void vHDMIMHL_Init(void);
extern void vMhl_MainLoop(void);


extern BOOL mhl_IIC_ReadBytes(U16 u16BusNumSlaveID, U8 u8AddrNum, U8* paddr, U16 u16size, U8* pu8data);
extern BOOL mhl_IIC_WriteBytes(U16 u16BusNumSlaveID, U8 u8addrcount, U8* pu8addr, U16 u16size, U8* pu8data);

extern UINT32 MHL_Timer_Get(void);
extern void msAPI_Timer_Delayms(UINT32 ms);
extern void MHL_Reset_High(void);
extern void MHL_Reset_Low(void);
extern BOOL vGetMHLCableDetect(void);
extern BOOL vGetMHLCbusConnect(void);

extern void Delay_ms(UINT32 Delay_Cnt);


extern void SendRcpKey(UINT8 key);

extern MS_BOOL mapi_mhl_SendRCP_PressRelease_Cmd(MS_U8 u8RCPKey);





