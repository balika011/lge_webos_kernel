#include "x_typedef.h"
#include "x_os.h"
#include "typedef.h"

extern void vHDMIMHL_Init(void);
extern void vMhl_MainLoop(void);


extern void fgSil_NTF_CBUS_Connected(void);
extern void fgSil_NTF_CBUS_Disconnected(void);
extern void fgSil_NTF_3D_Communicated(void);

extern void fgSil_NTF_VBUS_High(void); 
extern void fgSil_NTF_VBUS_Low(void); 


extern BOOL fgSil_Sif_WriteBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount);
extern BOOL fgSil_Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount);
extern void SendRcpKey(UINT8 key);







