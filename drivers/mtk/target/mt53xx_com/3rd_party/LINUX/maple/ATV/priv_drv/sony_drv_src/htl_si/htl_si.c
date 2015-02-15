#include "x_debug.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "../../sony_drv_inc/htl_si_if.h"
#include "htl_si.h"
#include "eeprom_if.h"
#include "drvcust_if.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_dbg.h"

/* Definitions */
/*#define HTLVOL_DEBUG*/

#ifdef HTLSI_DEBUG
#define HTLSI_DBG(...) x_dbg_stmt(__VA_ARGS__)
#else
#define HTLSI_DBG(...)
#endif


/* Static Variables */
static UINT8 ui1HtlSiOnlineMode = 0x00;
/* Static Functions */

/* Public Functions */

/** HTLSI_Init
 *  Initialize the hotel SI module.
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_Init(VOID)
{
    return TRUE;
}

/** HTLSI_GetOnlineMode
 *  Query the hotel SI module's online mode: 0 OFF, 1 ON
 *  @param  pbOnlineMode would be stored the module's online mode
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_GetOnlineMode(UINT8 *pui1OnlineMode)
{
    *pui1OnlineMode = ui1HtlSiOnlineMode;
    return TRUE;
}


/** HTLSI_SetOnlineMode
 *  Set the hotel SI module's online mode: 0 OFF, 1 ON
 *  @param  pbOnlineMode Would be stored the module's running mode
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_SetOnlineMode(UINT8 *pui1OnlineMode)
{
   ui1HtlSiOnlineMode = *pui1OnlineMode;   
    return TRUE;
}

