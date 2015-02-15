/**
 * @file si_drv_cra_api.h
 *
 * @brief Chip Register Access Driver
 *
 *****************************************************************************/

#ifndef SI_DRV_CRA_API_H
#define SI_DRV_CRA_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public type definitions ***********************************************/

typedef uint16_t      SiiDrvCraAddr_t;
typedef uint16_t      SiiDrvCraSize_t;
typedef uint8_t       SiiDrvCraIdx_t;

/***** public functions ******************************************************/

void                SiiDrvCraCreate( uint8_t devId );
void                SiiDrvCraDelete( uint8_t devId );

void                SiiDrvCraWrite8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t val );
uint8_t             SiiDrvCraRead8( uint8_t devId, SiiDrvCraAddr_t addr );
void                SiiDrvCraBitsSet8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t mask, bool_t bSet );
void                SiiDrvCraBitsMod8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t mask, uint8_t val );

void                SiiDrvCraWrite16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t val );
uint16_t            SiiDrvCraRead16( uint8_t devId, SiiDrvCraAddr_t addr );
void                SiiDrvCraBitsSet16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t mask, bool_t bSet );
void                SiiDrvCraBitsMod16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t mask, uint16_t val );

void                SiiDrvCraWrite24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t val );
uint32_t            SiiDrvCraRead24( uint8_t devId, SiiDrvCraAddr_t addr );
void                SiiDrvCraBitsSet24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, bool_t bSet );
void                SiiDrvCraBitsMod24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, uint32_t val );

void                SiiDrvCraWrite32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t val );
uint32_t            SiiDrvCraRead32( uint8_t devId, SiiDrvCraAddr_t addr );
void                SiiDrvCraBitsSet32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, bool_t bSet );
void                SiiDrvCraBitsMod32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, uint32_t val );

void                SiiDrvCraFifoWrite8( uint8_t devId, SiiDrvCraAddr_t addr, const uint8_t* pData, SiiDrvCraSize_t size );
void                SiiDrvCraFifoRead8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t* pData, SiiDrvCraSize_t size );
void                SiiDrvCraBlockWrite8( uint8_t devId, SiiDrvCraAddr_t addr, const uint8_t* pData, SiiDrvCraSize_t size );
void                SiiDrvCraBlockRead8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t* pData, SiiDrvCraSize_t size );

#endif /* SI_DRV_CRA_API_H */
