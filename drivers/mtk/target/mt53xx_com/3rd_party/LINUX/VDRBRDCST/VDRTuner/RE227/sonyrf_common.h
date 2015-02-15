/*------------------------------------------------------------------------------
  Copyright 2009-2012 Sony Corporation

  Last Updated  : 2012/03/14
  File Revision : 1.0.2.0
------------------------------------------------------------------------------*/

#ifndef SONYRF_COMMON_H
#define SONYRF_COMMON_H

/* Type definitions. */
/* <PORTING> Please comment out if conflicted */
#ifdef __linux__ /* For MTK */
#include <linux/types.h>
#else /* For MTK */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

/* <PORTING> Sleep function define (Please modify as you like) */
/*
#ifdef _WINDOWS
#include <windows.h>
#define SONYRF_SLEEP(n) Sleep(n)
#endif
*/

/* For MTK */
#include "x_os.h"
#define SONYRF_SLEEP(n) x_thread_delay(n)

#ifndef SONYRF_SLEEP
#error SONYRF_SLEEP(n) is not defined.
#endif

/* <PORTING> Floating point enable */
/* #define SONYRF_FP_ENABLE */

/* <PORTING> Trace function enable */
#define SONYRF_TRACE_ENABLE
/* <PORTING> Enable if I2C related function trace is necessary */
/* #define SONYRF_TRACE_I2C_ENABLE */

/* <PORTING> Macro to specify unused argument and suppress compiler warnings. */
#define SONYRF_ARG_UNUSED(arg) ((arg) = (arg))

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/
/* Return codes */
typedef enum {
    SONYRF_RESULT_OK,
    SONYRF_RESULT_ERROR_ARG,      /* Invalid argument (maybe software bug) */
    SONYRF_RESULT_ERROR_I2C,      /* I2C communication error */
    SONYRF_RESULT_ERROR_SW_STATE, /* Invalid software state */
    SONYRF_RESULT_ERROR_HW_STATE, /* Invalid hardware state */
    SONYRF_RESULT_ERROR_TIMEOUT,  /* Timeout occured */
    SONYRF_RESULT_ERROR_UNLOCK,   /* Failed to lock */
    SONYRF_RESULT_ERROR_RANGE,    /* Out of range */
    SONYRF_RESULT_ERROR_NOSUPPORT,/* Not supported for current device */
    SONYRF_RESULT_ERROR_CANCEL,   /* The operation is canceled */
    SONYRF_RESULT_ERROR_OTHER
} sonyrf_result_t;

/*------------------------------------------------------------------------------
  Common functions
------------------------------------------------------------------------------*/
int32_t sonyrf_Convert2SComplement(uint32_t value, uint32_t bitlen);
uint32_t sonyrf_BitSplitFromByteArray(uint8_t *pArray, uint32_t startBit, uint32_t bitNum);

/*------------------------------------------------------------------------------
  Trace
------------------------------------------------------------------------------*/
#ifdef SONYRF_TRACE_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
/* In sample application, these functions are implemented in main.c */
void sonyrf_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sonyrf_trace_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum);
#define SONYRF_TRACE_ENTER(func) sonyrf_trace_log_enter((func), __FILE__, __LINE__)
#define SONYRF_TRACE_RETURN(result) do{sonyrf_trace_log_return((result), __FILE__, __LINE__); return (result);}while(0)
#else /* SONYRF_TRACE_ENABLE */
#define SONYRF_TRACE_ENTER(func)
#define SONYRF_TRACE_RETURN(result) return(result)
#define SONYRF_TRACE_I2C_ENTER(func)
#define SONYRF_TRACE_I2C_RETURN(result) return(result)
#endif /* SONYRF_TRACE_ENABLE */


#ifdef SONYRF_TRACE_I2C_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sonyrf_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sonyrf_trace_i2c_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum);
#define SONYRF_TRACE_I2C_ENTER(func) sonyrf_trace_i2c_log_enter((func), __FILE__, __LINE__)
#define SONYRF_TRACE_I2C_RETURN(result) do{sonyrf_trace_i2c_log_return((result), __FILE__, __LINE__); return (result);}while(0)
#else /* SONYRF_TRACE_I2C_ENABLE */
#define SONYRF_TRACE_I2C_ENTER(func)
#define SONYRF_TRACE_I2C_RETURN(result) return(result)
#endif /* SONYRF_TRACE_I2C_ENABLE */

#endif /* SONYRF_COMMON_H */
