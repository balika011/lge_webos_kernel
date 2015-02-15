/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file		hal_venc_debug.h
 *
 *  Header file for venc_debug.
 *
 *  @author   	Jaeseop So(jaeseop.so.lge.com)
 *  @version  	1.0
 *  @date     	2012.05.31
 *  @see		
 */


#ifndef _VENC_DEBUG_H_
#define _VENC_DEBUG_H_
/*-----------------------------------------------------------------------------
	Control Constants
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    File Inclusions
-----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#include "logm_kadp.h"

/*-----------------------------------------------------------------------------
	Constant Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Macro Definitions
-----------------------------------------------------------------------------*/
#ifndef MIN
   #define MIN(x, y) ( ((x)<(y))?(x):(y) )
#endif

#define UNLIKE(x)           unlikely(x)
#define LIKE(x)             likely(x)


#define VENC_COLOR_NONE		"\x1b[0m"
#define VENC_COLOR_RED		"\x1b[1;31m"
#define VENC_COLOR_GREEN	"\x1b[1;32m"
#define VENC_COLOR_YELLOW	"\x1b[1;33m"
#define VENC_COLOR_BLUE		"\x1b[1;34m"
#define VENC_COLOR_PURPLE	"\x1b[1;35m"
#define VENC_COLOR_CYAN		"\x1b[1;36m"

#if 0
    //0 ~ 31 bit
#define VENC_NORMAL             (0x01 << 0)
#define VENC_ERROR              (0x01 << 1)
#define VENC_INFO               (0x01 << 2)
#define VENC_MONITOR            (0x01 << 3)
#define VENC_VENC_DN            (0x01 << 4)
#define VENC_VENC_PTR           (0x01 << 5)
#define VENC_TRACE				(0x01 << 6)
#else
enum {
	VENC_ERROR    = (0x01 << LX_LOGM_LEVEL_ERROR),
	VENC_WARN     = (0x01 << LX_LOGM_LEVEL_WARNING),
	VENC_NOTI     = (0x01 << LX_LOGM_LEVEL_NOTI),
	VENC_INFO     = (0x01 << LX_LOGM_LEVEL_INFO),
	VENC_NORMAL   = (0x01 << LX_LOGM_LEVEL_DEBUG),
	VENC_TRACE    = (0x01 << LX_LOGM_LEVEL_TRACE),
	VENC_MONITOR  = (0x01 << (LX_LOGM_LEVEL_TRACE+1)),
	VENC_VENC_DN  = (0x01 << (LX_LOGM_LEVEL_TRACE+2)),
	VENC_VENC_PTR = (0x01 << (LX_LOGM_LEVEL_TRACE+3))
};
#endif

#if 0
#define HAL_VENC_DEBUG(fmt, args...)				DEBUG_VENC_PRINT(VENC_NORMAL, VENC_COLOR_BLUE		"[  VENC_TRACE]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_ERROR(fmt, args...)				DEBUG_VENC_PRINT(VENC_ERROR, VENC_COLOR_RED			"[  VENC_ERROR]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_WARM(fmt, args...)				    DEBUG_VENC_PRINT(VENC_ERROR, VENC_COLOR_RED			"[   VENC_WARN]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_INFO(fmt, args...)					DEBUG_VENC_PRINT(VENC_INFO, VENC_COLOR_PURPLE		"[   VENC_INFO]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define VENC_DBG_MONITOR(fmt, args...)				DEBUG_VENC_PRINT(VENC_MONITOR, VENC_COLOR_PURPLE	"[VENC_MONITOR]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_DN(fmt, args...)				DEBUG_VENC_PRINT(VENC_VENC_DN, VENC_COLOR_CYAN		"[ VENC_VES_DN]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_PTR(fmt, args...)			DEBUG_VENC_PRINT(VENC_VENC_PTR, VENC_COLOR_CYAN		"[VENC_VES_PTR]" VENC_COLOR_NONE fmt "\n\r", ## args)
#define HAL_VENC_TRACE(fmt, args...)				DEBUG_VENC_PRINT(VENC_TRACE, VENC_COLOR_YELLOW 		"[  VENC_TRACE]" VENC_COLOR_NONE fmt "\n\r", ## args)
#endif

#define HAL_VENC_PRINT(fmt, args...)				printf(fmt, ##args)

extern SINT32 g_hal_venc_logm_fd;
extern UINT32 g_hal_venc_log_level;
#define HAL_VENC_GET_INDEX( val, index )	((val) & (1<<(index)))
#define HAL_VENC_LOGM 						"hal-venc"
#define HAL_VENC_ERROR(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_ERROR, 	fmt, ##args)
#define HAL_VENC_WARN(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_WARNING,  fmt, ##args)
#define HAL_VENC_NOTI(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_NOTI, 	fmt, ##args)
#define HAL_VENC_INFO(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_INFO, 	fmt, ##args)
#define HAL_VENC_DEBUG(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_DEBUG, 	fmt, ##args)
#define HAL_VENC_TRACE(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_TRACE, 	fmt, ##args)
#define HAL_VENC_MONITOR(fmt,args...)		KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_TRACE+1, 	fmt, ##args)
#define HAL_VENC_DN(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_TRACE+2, 	fmt, ##args)
#define HAL_VENC_PTR(fmt,args...)			KADP_LOGM_PRINT(g_hal_venc_logm_fd, LX_LOGM_LEVEL_TRACE+3, 	fmt, ##args)

#define HAL_VENC_Sleep(msec)				KADP_OSA_SleepMsec(msec)
//#define HAL_VENC_ReadMsTicks()						KADP_OSA_ReadMsTicks();

/**
 *	@def __CHECK_IF_ERROR
 *
 *	General purpose error check routine.
 *	This macro will help you to write error check code with one line.
 */
#ifndef	__CHECK_IF_ERROR
#define	__CHECK_IF_ERROR(__checker,__if_output,__if_action,fmt,args...)	\
							do { 								\
								if (unlikely(__checker)) { 				\
									__if_output(fmt,##args);	\
									__if_action;				\
								}								\
							} while(0)
#endif

#ifndef	__CHECK_IF_ELSE_ERROR
#define	__CHECK_IF_ELSE_ERROR(__checker,__if_output,__if_action,__else_action,fmt,args...)	\
							do { 								\
								if (__checker) { 				\
									__if_output(fmt,##args);	\
									__if_action;				\
								}								\
								else {							\
									__else_action;				\
								}								\
							} while(0)
#endif

#define VENC_CHECK_CODE(__checker,__if_action,fmt,args...)   \
							__CHECK_IF_ERROR(__checker, HAL_VENC_ERROR, __if_action , fmt, ##args )


#define VENC_CHECK_PARAM(x) { \
__CHECK_IF_ERROR(x == NULL, HAL_VENC_ERROR, return NOT_OK , "[%s] [%u] parameter is NULL!\n", __FUNCTION__, __LINE__);\
	}

#define VENC_CHECK_RET(x, __if_action) { \
__CHECK_IF_ERROR(x != 0, HAL_VENC_ERROR, __if_action, "[%s] Error! [E = %d] Line:%d\n", __FUNCTION__, x, __LINE__);\
	}


#define OSA_CLOSE(x)    do{                     \
                            if(x > -1){         \
                               OSA_Close(x);    \
                               x = -1;          \
                            }                   \
                        } while(0)

#define OSA_FREE(x)     do{                     \
                            if(x != NULL){      \
                                free(x);        \
                                x = NULL;       \
                            }                   \
                        }while(0)

#define OSA_fclose(x)   do{                     \
                            if(x != NULL){         \
                               fclose(x);    \
                               x = NULL;          \
                            }                   \
                        } while(0)


#define OSA_fprintf(fd, fmt, args...)    do{                                \
                                            if(fd != NULL){                 \
                                                fprintf(fd, fmt, ##args);   \
                                            }                               \
                                        }while(0)

/*-----------------------------------------------------------------------------
    Type Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Function Prototype Declaration 
-----------------------------------------------------------------------------*/
extern void DEBUG_VENC_Menu(void);

/*-----------------------------------------------------------------------------
	Extern Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Includes
-----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif


#endif


//EOF
