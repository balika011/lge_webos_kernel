/***************************************************************************************/
/* FILE: Si2151_L0_TV_Chassis.h                                                        */
/*                                                                                     */
/*                                                                                     */
/*                                                                                     */
/***************************************************************************************/
#ifndef SI2151_L0_TV_CHASSIS_H
#define SI2151_L0_TV_CHASSIS_H

typedef struct Si2151_L0_Context {
    unsigned char   address;
    int             indexSize;
} Si2151_L0_Context;

unsigned int     Si2151_system_time              (void); 
int              Si2151_system_wait              (int time_ms);
void	         Si2151_L0_Init                  (Si2151_L0_Context *pContext);
int              Si2151_L0_SetAddress            (Si2151_L0_Context* i2c, unsigned int add, int addSize);
int		         Si2151_L0_ReadCommandBytes      (Si2151_L0_Context* i2c, int iNbBytes, unsigned char *pucDataBuffer);
int		         Si2151_L0_WriteCommandBytes     (Si2151_L0_Context* i2c, int iNbBytes, unsigned char *pucDataBuffer);
int              Si2151_L0_ReadBytes             (Si2151_L0_Context* i2c, unsigned int iI2CIndex, int iNbBytes, unsigned char *pucDataBuffer);
int              Si2151_L0_WriteBytes            (Si2151_L0_Context* i2c, unsigned int iI2CIndex, int iNbBytes, unsigned char *pucDataBuffer);
#endif /* SI2151_L0_TV_CHASSIS_H */
