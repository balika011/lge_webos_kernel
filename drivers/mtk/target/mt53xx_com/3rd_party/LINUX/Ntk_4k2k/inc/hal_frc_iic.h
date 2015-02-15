
/*! 
********************************************************************************
*  Copyright (c) Novatek Microelectronics Corp., Ltd. All Rights Reserved. 
*  \file    hal_frc.h
*  \brief 
*  \version ver0.2 
*  \chip    NT72314
******************************************************************************** 
*/
//#include "Def_types.h"

#ifdef  __cplusplus
extern "C" {
#endif

void OS_DelayMS(u16 u16DelayMS);	

bool HAL_FRC_ReadReg(u32 u32Reg, u32* u32Value);

bool HAL_FRC_WriteReg(u32 u32Reg, u32 u32Value);

bool HAL_FRC_WriteBurst(u32 u32Addr, unsigned char *data, u32 len);

bool HAL_FRC_Unlock(void);

#ifdef  __cplusplus
}
#endif
