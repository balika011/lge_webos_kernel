/*------------------------------------------------------------------------------

 Copyright 2009, 2010, 2011 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/01/28
 Revision: 1.0.3.0

------------------------------------------------------------------------------*/

#ifndef RA217_TUNER_COMMON_H
#define RA217_TUNER_COMMON_H

/* Type definitions. */
/* <PORTING> Please comment out if conflicted */
/*typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;*/

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

/* <PORTING> Sleep function define (Please modify as you like) */
#ifdef _WINDOWS
#include <windows.h>
#define RA217_SLEEP(n) Sleep(n)
#endif

#include "x_os.h"
#include "x_typedef.h"
#define RA217_SLEEP(n) x_thread_delay(n)

#ifndef RA217_SLEEP
#error RA217_SLEEP(n) is not defined.
#endif

/* <PORTING> Floating point enable */
//#define RA217_FP_ENABLE

/* <PORTING> Trace function enable */
#define RA217_TRACE_ENABLE // NOTE: This can be disabled if you do not need debug message.
/* <PORTING> Enable if I2C related function trace is necessary */
/* #define RA217_TRACE_I2C_ENABLE */

/* <PORTING> Macro to specify unused argument and suppress compiler warnings. */
#define RA217_ARG_UNUSED(arg) ((arg) = (arg))

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/
/* Return codes */
typedef enum {
    RA217_RESULT_OK,
    RA217_RESULT_ERROR_ARG,      /* Invalid argument (maybe software bug) */
    RA217_RESULT_ERROR_I2C,      /* I2C communication error */
    RA217_RESULT_ERROR_SW_STATE, /* Invalid software state */
    RA217_RESULT_ERROR_HW_STATE, /* Invalid hardware state */
    RA217_RESULT_ERROR_TIMEOUT,  /* Timeout occured */
    RA217_RESULT_ERROR_UNLOCK,   /* Failed to lock */
    RA217_RESULT_ERROR_RANGE,    /* Out of range */
    RA217_RESULT_ERROR_NOSUPPORT,/* Not supported for current device */
    RA217_RESULT_ERROR_CANCEL,   /* The operation is canceled */
    RA217_RESULT_ERROR_OTHER
} ra217_result_t;

/*------------------------------------------------------------------------------
  Common functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Trace
------------------------------------------------------------------------------*/
#ifdef RA217_TRACE_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
/* In sample application, these functions are implemented in main.c */
void ra217_tuner_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void ra217_tuner_trace_log_return(ra217_result_t result, const char* filename, unsigned int linenum);
#define RA217_TRACE_ENTER(func) ra217_tuner_trace_log_enter((func), __FILE__, __LINE__)
#define RA217_TRACE_RETURN(result) do{ra217_tuner_trace_log_return((result), __FILE__, __LINE__); return (result);}while(0)
#else /* RA217_TRACE_ENABLE */
#define RA217_TRACE_ENTER(func)
#define RA217_TRACE_RETURN(result) return(result)
#define RA217_TRACE_I2C_ENTER(func)
#define RA217_TRACE_I2C_RETURN(result) return(result)
#endif /* RA217_TRACE_ENABLE */


#ifdef RA217_TRACE_I2C_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void ra217_tuner_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void ra217_tuner_trace_i2c_log_return(ra217_result_t result, const char* filename, unsigned int linenum);
#define RA217_TRACE_I2C_ENTER(func) ra217_tuner_trace_i2c_log_enter((func), __FILE__, __LINE__)
#define RA217_TRACE_I2C_RETURN(result) do{ra217_tuner_trace_i2c_log_return((result), __FILE__, __LINE__); return (result);}while(0)
#else /* RA217_TRACE_I2C_ENABLE */
#define RA217_TRACE_I2C_ENTER(func)
#define RA217_TRACE_I2C_RETURN(result) return(result)
#endif /* RA217_TRACE_I2C_ENABLE */

#endif /* RA217_COMMON_H */
