/*!
********************************************************************************
*  Copyright (c) Novatek Microelectronics Corp., Ltd. All Rights Reserved.
*  \file    def_type.h
*  \brief   header file for type definition
*  \project all
*  \chip    
********************************************************************************
*/
#ifndef _COM_FEC_TYPE_H
#define _COM_FEC_TYPE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "x_typedef.h"

/*-----------------------------------------------------------------------------*/
/* Including Files                                                             */
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/* Constant Definitions                                                        */
/*-----------------------------------------------------------------------------*/
#ifndef TRUE
#define TRUE        (1)                  /*!< Define TRUE 1 */
#endif

#ifndef ENABLE
#define ENABLE      (1)                  /*!< Define ENABLE 1 */
#endif

#ifndef FALSE
#define FALSE       (0)                  /*!< Define FALSE 0 */
#endif

#ifndef DISABLE
#define DISABLE     (0)                  /*!< Define DISABLE 0 */
#endif

#ifdef  __cplusplus
#ifndef NULL
#define NULL		 (0)
#endif
#else
#ifndef NULL
#define NULL        ((void *)(0))       /*!< Define NULL (void *)(0) */
#endif
#endif

#define INVALID_U32 ((unsigned long)(-1))
#define INVALID_U16 ((unsigned short)(-1))
#define INVALID_U8  ((unsigned char)(-1))
#define INVALID_HDL INVALID_U32
#define INVALID_PTS 0x8000000000000000LL

#ifndef  _Pack_Struct_
#define  _Pack_Struct_		__attribute__ ((__packed__))
#endif

/*-----------------------------------------------------------------------------*/
/* Types Declarations                                                          */
/*-----------------------------------------------------------------------------*/
#ifdef KERNEL_MODE_CODING
#include <linux/types.h>
#else
#ifndef u8
#define u8              UINT8
#endif
#ifndef u16
#define u16             UINT16
#endif
#ifndef u32
#define u32             UINT32
#endif
#ifndef u64
#define u64             (unsigned long long)
#endif
#ifndef s8
#define s8              INT8
#endif
#ifndef s16
#define s16              INT16
#endif
#ifndef s32
#define s32              INT32
#endif
#ifndef s64
#define s64              (__signed long long)
#endif
#ifndef bool
#ifndef  __cplusplus
#define bool            UINT8
#endif
#endif
#endif

#ifndef c8
#define c8              char
#endif
#ifndef cc8
#define cc8              (const char)
#endif
#ifndef c16
#define c16              (unsigned short)
#endif
#ifndef hdl
#define hdl              (unsigned long)
#endif
#ifndef bool8
#define bool8              UINT8
#endif
#ifndef vu8
#define vu8              (volatile unsigned char)
#endif
#ifndef vu16
#define vu16              (volatile unsigned short)
#endif
#ifndef vu32
#define vu32              (volatile unsigned int)
#endif
#ifndef vuint
#define vuint              (volatile unsigned int)
#endif
#ifndef ercode
#define ercode              (long)
#endif

typedef struct _ST_POS
{
	s32 s32X; /*!< horizontal position */
	s32 s32Y; /*!< vertical position */

} ST_POS;

typedef struct _ST_SIZE
{
	u32 u32W; /*!< horizontal size */
	u32 u32H; /*!< vertical size */

} ST_SIZE;

typedef struct _ST_RECT
{
	s32 s32X; /*!< horizontal position */
	s32 s32Y; /*!< vertical position */
	u32 u32W; /*!< horizontal size */
	u32 u32H; /*!< vertical size */
} ST_RECT;

typedef unsigned char          U8BIT;
typedef char                   S8BIT;
typedef unsigned short         U16BIT;
typedef signed short           S16BIT;
typedef unsigned int           U32BIT;
typedef signed int             S32BIT;
typedef unsigned char          BOOLEAN;  /* BOOLEAN as 1 byte */
#define PC_SIMULATION 0
#if (!PC_SIMULATION)
typedef unsigned long          HANDLE;
#endif
//typedef float                  FLOAT;
//typedef double                 DOUBLE;
typedef signed long            ER;

/*-----------------------------------------------------------------------------*/
/* Extern Global Variables                                                     */
/*-----------------------------------------------------------------------------*/
// DON'T export global variable here. Please use function to access them
/*-----------------------------------------------------------------------------*/
/* Interface Function Prototype                                                */
/*-----------------------------------------------------------------------------*/

#define TMP_UNUSED(expr) \
    do                   \
    {                    \
        (void)(expr);    \
    } while (0)

#define TMP_UNUSED2(expr1,expr2) \
    do                  \
    {                   \
        (void)(expr1);  \
        (void)(expr2);  \
    } while (0)

#define TMP_UNUSED3(expr1,expr2, expr3) \
    do                  \
    {                   \
        (void)(expr1);  \
        (void)(expr2);  \
        (void)(expr3);  \
    } while (0)

typedef enum
{
	FRC_OK						= 0,		//!<
	FRC_NOT_OK					= -1,	//!<
	FRC_NOT_SUPPORTED		= -2,	//!<
	FRC_PARAMETER_ERROR 	= -3	,	//!<
	FRC_DONT_CARE				= -4,
	FRC_ERR_SYS 			= -5	,	//!< operating system call failure 
	FRC_ERR_INV 			= -6	,	//!< invalid argument passed 
	FRC_ERR_TIMEDOUT			= -7	,	//!< timeout occured 
	FRC_ERR_DEFERRED			= -8	,	//!< action has been deferred 
	FRC_ERR_UNIMPLEM			= -9	,	//!< Unimplemented service 
	FRC_ERR_LOCKED			= -10,	//!< object locked 
	FRC_ERR_DELETED 		= -11,	//!< Object has been deleted 
	FRC_ERR_HEAP				= -12,	//!< Heap full 
	FRC_ERR_UNINIT				= -13,	//!< module is not initialised 
	FRC_ERR_UNIQUE			= -14,	//!< unique id needed 
	FRC_ERR_STATE				= -15,	//!< invalid state to invoke function 
	FRC_ERR_DEV 			= -16,	//!< no valid device exists 
	FRC_ERR_NOT_FOUND		= -17,	//!< Not found
	FRC_ERR_EOL 			= -18,	//!< a list traversal has ended 
	FRC_ERR_TERM				= -19,	//!< module has terminated 
	FRC_ERR_LIMIT				= -20,	//!< System limit was reached. 
	FRC_ERR_RESOURCE			= -21,	//!< insufficient resources for request 
	FRC_ERR_NOT_AVAIL			= -22,	//!< resources not available 
	FRC_ERR_BAD_DATA			= -23,	//!< Bad data in transport stream 
	FRC_ERR_INLINE_SUCCESS	= -24,
	FRC_ERR_DEFERRED_SUCCESS	= -25,
	FRC_ERR_IN_USE			= -26,	//!< Object is in use. 
	FRC_ERR_INTERMEDIATE		= -27,	//!< Intermediate return - successful but not complete 
	FRC_ERR_OVERRUN 		= -28,	//!< Buffer overrun occured (e.g. in filters). 
	FRC_ERR_ABORT				= -29,	//!< Request not supported. 
	FRC_ERR_NOTIMPLEMENTED	= -30,	//!< Request not implemented. 
	FRC_ERR_INVALID_HANDLE	= -31,
	FRC_ERR_NOMEM				= -32,	//!< No Memory 
	FRC_ERR_USER				= -99	//!< start of subsystem specific errors 
} FRC_Result;							//!< FRC Error code

#ifdef  __cplusplus
}
#endif

#endif /* _COM_DEF_TYPE_H */










