#ifndef HTL_SI_H
#define HTL_SI_H

#include "x_typedef.h"

/** HTLSI_Init
 *  Initialize the hotel SI module.
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_Init(VOID);

/** HTLSI_GetOnlineMode
 *  Query the hotel SI module's online mode: 0 OFF, 1 ON
 *  @param  pbOnlineMode would be stored the module's online mode
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_GetOnlineMode(UINT8 *pui1OnlineMode);


/** HTLSI_SetOnlineMode
 *  Set the hotel SI module's online mode: 0 OFF, 1 ON
 *  @param  pbOnlineMode Would be stored the module's running mode
 *  @retval FALSE FAIL
 *          TRUE SUCCESSFUL
 */
BOOL HTLSI_SetOnlineMode(UINT8 *pui1OnlineMode);


#endif /* HTL_SI_H */

