#include "x_debug.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "../../sony_drv_inc/htl_vol_if.h"
#include "htl_vol.h"
#include "eeprom_if.h"
#include "drvcust_if.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_dbg.h"

/* Definitions */
/*#define HTLVOL_DEBUG*/

#ifdef HTLVOL_DEBUG
#define HTLVOL_DBG(...) x_dbg_stmt(__VA_ARGS__)
#else
#define HTLVOL_DBG(...)
#endif

#define ON 1
#define OFF 0
#define HTLVOL_STEPTIMER_MAX 250
#define HTLVOL_STEPTIMER_MIN 1

/* Static Variables */
/* ui1HtlVolMode:
   0 for OFF, and 1 for ON
   Defaults to OFF
*/
static UINT8 ui1HtlVolMode = OFF;

/* ui1HtlVolInitVol:
   Bath Volume initial volume level
*/
static UINT8 ui1HtlVolInitVol = 0;

/* ui1HtlVolLvl:
   Bath Volume's volume level
*/
static UINT8 ui1HtlVolLvl = 0;


/* ui1HtlVolInitVol:
   Bath Volume step timer
*/
static UINT8 ui1HtlVolStepTimer = 0;

/* Static Functions */

/* Public Functions */
BOOL HTLVOL_Init(VOID)
{
    UINT8 aui1Buf[4]={0,0,0,0};

    /* Read bath speaker volume parameters from EEPROM */
    EEPROM_Read((UINT64)HTL_VOL_EEPROM_HTLVOL_MODEL, (UINT32)aui1Buf, 4);
    if(HTL_VOL_EEPROM_CHKSUM != aui1Buf[3]) /* Invalid value, we'll write the default into eeprom then */
    {
        UINT8 aui1WriteBuf[4] = { 0x00, 0x0a, 0x0a, HTL_VOL_EEPROM_CHKSUM };
        EEPROM_Write((UINT64)HTL_VOL_EEPROM_HTLVOL_MODEL, (UINT32)aui1WriteBuf, 4);
        ui1HtlVolMode = OFF;
        /* Even hotel volume is disabled, these data are still needed by CODE2 */
        ui1HtlVolInitVol = aui1WriteBuf[1];
        ui1HtlVolStepTimer  = aui1WriteBuf[2];
        ui1HtlVolLvl = ui1HtlVolInitVol;
    }
    else
    {
        ui1HtlVolMode = ((ON == aui1Buf[0]) ? ON : OFF);
        /* Even hotel volume is disabled, these data are still needed by CODE2 */
        ui1HtlVolInitVol = aui1Buf[1];
        ui1HtlVolStepTimer  = aui1Buf[2];
        ui1HtlVolLvl = ui1HtlVolInitVol;
    }
    
    /* fail safe check: Check if timer step is in the range just like CODE2*/
    if (ui1HtlVolStepTimer < HTLVOL_STEPTIMER_MIN || ui1HtlVolStepTimer > HTLVOL_STEPTIMER_MAX)
    {
        UINT8 aui1WriteBuf[1];
        
        if (ui1HtlVolStepTimer < HTLVOL_STEPTIMER_MIN)
        {
            ui1HtlVolStepTimer = HTLVOL_STEPTIMER_MIN;
        }
        
        if (ui1HtlVolStepTimer > HTLVOL_STEPTIMER_MAX)
        {
            ui1HtlVolStepTimer = HTLVOL_STEPTIMER_MAX;
        }
        
        aui1WriteBuf[0] = ui1HtlVolStepTimer;
        EEPROM_Write((UINT64)HTL_VOL_EEPROM_HTLVOL_STEPTIMER, (UINT32)aui1WriteBuf, 1); /* write the default into eeprom then */
    }
    
    return TRUE;
}

BOOL HTLVOL_GetMode(UINT8 *pui1Mode)
{
    *pui1Mode = ui1HtlVolMode;
    return TRUE;
}

BOOL HTLVOL_GetInitVol(UINT8 *pui1InitVol)
{
    *pui1InitVol = ui1HtlVolInitVol;
    return TRUE;
}

BOOL HTLVOL_GetVolLvl(UINT8 *pui1VolLvl)
{
    *pui1VolLvl = ui1HtlVolLvl;
    return TRUE;
}

BOOL HTLVOL_GetStepTimer(UINT8 *pui1StepTimer)
{
    *pui1StepTimer = ui1HtlVolStepTimer;
    return TRUE;
}

BOOL HTLVOL_SetMode(UINT8 *pui1Mode)
{
    UINT8 aui1WriteBuf[1];
    ui1HtlVolMode = *pui1Mode;
    aui1WriteBuf[0] = ui1HtlVolMode;
    EEPROM_Write((UINT64)HTL_VOL_EEPROM_HTLVOL_MODEL, (UINT32)aui1WriteBuf, 1);
    return TRUE;
}

BOOL HTLVOL_SetInitVol(UINT8 *pui1InitVol)
{
    UINT8 aui1WriteBuf[1];
    ui1HtlVolInitVol = *pui1InitVol;
    aui1WriteBuf[0] = ui1HtlVolInitVol;
    EEPROM_Write((UINT64)HTL_VOL_EEPROM_HTLVOL_INITVOL, (UINT32)aui1WriteBuf, 1);
    return TRUE;
}

BOOL HTLVOL_SetVolLvl(UINT8 *pui1VolLvl)
{
    ui1HtlVolLvl = *pui1VolLvl;
    return TRUE;
}

BOOL HTLVOL_SetStepTimer(UINT8 *pui1StepTimer)
{
    UINT8 aui1WriteBuf[1];
    ui1HtlVolStepTimer = *pui1StepTimer;
    aui1WriteBuf[0] = ui1HtlVolStepTimer;
    EEPROM_Write((UINT64)HTL_VOL_EEPROM_HTLVOL_STEPTIMER, (UINT32)aui1WriteBuf, 1);
    return TRUE;
}

