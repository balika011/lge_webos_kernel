
#include "Def_types.h"


extern UINT16 SIF_X_Write(UINT8 u1CHannelID, UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddrNum, UINT32 u4WordAddr,
                   UINT8 *pu1Buf, UINT16 u2ByteCnt);
extern UINT16 SIF_X_Read(UINT8 u1CHannelID, UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddrNum, UINT32 u4WordAddr,
                  UINT8 *pu1Buf, UINT16 u2ByteCnt);

/*-----------------------------------------------------------------------------*/
/* Extern Global Variables                                                     */
/*-----------------------------------------------------------------------------*/
// DON'T export global variable here. Please use function to access them
/*-----------------------------------------------------------------------------*/
/* Interface Function Prototype                                                */
/*-----------------------------------------------------------------------------*/
 bool Soc_Drv_IIC_Init(void)
 {
	  return TRUE;
 }

// functions
/*!
 * \fn bool DRV_IIC_Open
 * \brief
 *  Open IIC
 *
 * \param pstOpenParam  (Input) open parameter
 *
 * \return Operation result
 * \retval TRUE         Operation Success
 * \retval FALSE        Operation Failure
 *
 */
bool Soc_Drv_IIC_Open( void *pstOpenParam )
{
	 return TRUE;
}

/*!
 * \fn bool DRV_IIC_Close
 * \brief
 *  Close IIC control
 *
 * \param EN_KER_IIC_BUS enBus     (Input) Bus ID
 *
 * \return none
 *
 */
void Soc_Drv_IIC_Close( void *pstOpenParam )
{
	 return ;
}


/*!
 * \fn s8 DRV_IIC_Read
 * \brief
 *  IIC Read
 *
 * \param EN_KER_IIC_BUS enBus     (Input) Bus ID
 * \param EN_KER_IIC_CLOCK enClk     (Input) Bus Clock
* \param u8SlaveAddr              (Input) Slave address
 * \param u32SubAddr               (Input) Sub address
 * \param u8NumOfBytes             (Input) Number of bytes to read
 * \param pau8Buf                  (Input) Pointer to data buffer 
 *
 * \return FAIL = 0 / TRUE = 1 
 *
 */
s8 Soc_Drv_IIC_Read( u8 u8SlaveAddr, u8 enSubAddrMode, u8 u8SubAddr, u32 u32NumOfBytes, u8 *pau8Buf )
{
    UINT16 u2Ret;
    u2Ret = SIF_X_Read(2,0x100,u8SlaveAddr,1,u8SubAddr,pau8Buf,u32NumOfBytes);

    if(u2Ret)
        return TRUE;
    else
        return FALSE;
}

/*!
 * \fn s8 DRV_IIC_Write
 * \brief
 *  IIC Write 
 *
 * \param EN_KER_IIC_BUS enBus     (Input) Bus ID
  * \param EN_KER_IIC_CLOCK enClk     (Input) Bus Clock
* \param u8SlaveAddr              (Input) Slave address
 * \param u32SubAddr               (Input) Sub address
 * \param u8NumOfBytes             (Input) Number of bytes to write 
 * \param pau8Buf                  (Input) Pointer to data buffer 
 *
 * \return FAIL = 0 / TRUE = 1 
 *
 */
s8 Soc_Drv_IIC_Write( u8 u8SlaveAddr, u8 enSubAddrMode, u8 u8SubAddr, u32 u32NumOfBytes, u8 *pau8Buf )
{
    UINT16 u2Ret;
    u2Ret = SIF_X_Write(2,0x100,u8SlaveAddr,1,u8SubAddr,pau8Buf,u32NumOfBytes);

    if(u2Ret)
        return TRUE;
    else
        return FALSE;
}














