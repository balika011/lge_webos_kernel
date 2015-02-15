/****************************************************************************

	UTILITY PRIVATE HEADER (MISC)            

*****************************************************************************
*
* File:        utility.h
*
* Description: utility (misc) private header
*
* Author:      kl-lees
*
* History:     17 july 2009 - Creation
*
*****************************************************************************/

#ifndef _UTILITY_H
#define _UTILITY_H

#define CreateMutex(x)      VERIFY(x_sema_create(&x, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK)
#define LockMutex(x)        VERIFY(x_sema_lock(x, X_SEMA_OPTION_WAIT) == OSR_OK)
#define UnlockMutex(x)      VERIFY(x_sema_unlock(x) == OSR_OK)
#define DeleteMutex(x)      VERIFY(x_sema_delete(x) == OSR_OK)

#define CreateSemaphore(x)  VERIFY(x_sema_create(&x, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK)
#define WaitSemaphore(x)    VERIFY(x_sema_lock(x, X_SEMA_OPTION_WAIT) == OSR_OK)
#define SignalSemaphore(x)  VERIFY(x_sema_unlock(x) == OSR_OK)
#define DeleteSemaphore(x)  VERIFY(x_sema_delete(x) == OSR_OK)

/****************************************************************************
	FUNCTIONS
*****************************************************************************/
extern UINT16 vAPIGetDriverVersion(void);

#endif /* _UTILITY_H */


/*****************************************************************************
*   End of file
*****************************************************************************/

