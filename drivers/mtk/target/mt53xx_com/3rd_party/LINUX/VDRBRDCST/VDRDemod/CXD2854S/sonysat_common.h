/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#ifndef SONYSAT_COMMON_H
#define SONYSAT_COMMON_H

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
#define SONYSAT_SLEEP(n) Sleep(n)
#endif
*/

/* For MTK */
#include "x_os.h"
#define SONYSAT_SLEEP(n) x_thread_delay(n)

#ifndef SONYSAT_SLEEP
#error SONYSAT_SLEEP(n) is not defined. This macro must be ported to your platform.
#endif

/* <PORTING> Trace function enable */
#define SONYSAT_TRACE_ENABLE
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_ENABLE

/* <PORTING> Enable if I2C related function trace is necessary */
/* #define SONYSAT_TRACE_I2C_ENABLE */

/* <PORTING> Macro to specify unused argument and suppress compiler warnings. */
#define SONYSAT_ARG_UNUSED(arg) ((arg) = (arg))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/* Return codes */
typedef enum {
    SONYSAT_RESULT_OK,              /**< Function was successfully actioned */
    SONYSAT_RESULT_ERROR_ARG,       /**< Invalid argument (maybe software bug) */
    SONYSAT_RESULT_ERROR_I2C,       /**< I2C communication error */
    SONYSAT_RESULT_ERROR_SW_STATE,  /**< Invalid software state */
    SONYSAT_RESULT_ERROR_HW_STATE,  /**< Invalid hardware state */
    SONYSAT_RESULT_ERROR_TIMEOUT,   /**< Timeout occurred */
    SONYSAT_RESULT_ERROR_UNLOCK,    /**< Failed to lock */
    SONYSAT_RESULT_ERROR_RANGE,     /**< Out of range */
    SONYSAT_RESULT_ERROR_NOSUPPORT, /**< Not supported for current device */
    SONYSAT_RESULT_ERROR_CANCEL,    /**< The operation is cancelled */
    SONYSAT_RESULT_ERROR_OTHER,     /**< Unspecified error */
    SONYSAT_RESULT_ERROR_OVERFLOW,  /**< Memory overflow */
    SONYSAT_RESULT_OK_CONFIRM       /**< Tune was successful, but confirm parameters */
} sonysat_result_t;

/*------------------------------------------------------------------------------
  Common functions
------------------------------------------------------------------------------*/
int32_t sonysat_Convert2SComplement(uint32_t value, uint32_t bitlen);
uint32_t sonysat_BitSplitFromByteArray(uint8_t *pArray, uint32_t startBit, uint32_t bitNum);
/*------------------------------------------------------------------------------
  Trace
------------------------------------------------------------------------------*/
/*
 Disables MS compiler warning (__pragma(warning(disable:4127))
 with do { } while (0);
*/
#define SONYSAT_MACRO_MULTILINE_BEGIN  do {
#if ((defined _MSC_VER) && (_MSC_VER >= 1300))
#define SONYSAT_MACRO_MULTILINE_END \
        __pragma(warning(push)) \
        __pragma(warning(disable:4127)) \
        } while(0) \
        __pragma(warning(pop))
#else
#define SONYSAT_MACRO_MULTILINE_END } while(0)
#endif


#ifdef SONYSAT_TRACE_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sonysat_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sonysat_trace_log_return(sonysat_result_t result, const char* filename, unsigned int linenum);
#define SONYSAT_TRACE_ENTER(func) sonysat_trace_log_enter((func), __FILE__, __LINE__)
#define SONYSAT_TRACE_RETURN(result) \
    SONYSAT_MACRO_MULTILINE_BEGIN \
        sonysat_trace_log_return((result), __FILE__, __LINE__); \
        return (result); \
    SONYSAT_MACRO_MULTILINE_END
#else /* SONYSAT_TRACE_ENABLE */
#define SONYSAT_TRACE_ENTER(func)
#define SONYSAT_TRACE_RETURN(result) return(result)
#define SONYSAT_TRACE_I2C_ENTER(func)
#define SONYSAT_TRACE_I2C_RETURN(result) return(result)
#endif /* SONYSAT_TRACE_ENABLE */


#ifdef SONYSAT_TRACE_I2C_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sonysat_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sonysat_trace_i2c_log_return(sonysat_result_t result, const char* filename, unsigned int linenum);
#define SONYSAT_TRACE_I2C_ENTER(func) sonysat_trace_i2c_log_enter((func), __FILE__, __LINE__)
#define SONYSAT_TRACE_I2C_RETURN(result) \
    SONYSAT_MACRO_MULTILINE_BEGIN \
        sonysat_trace_i2c_log_return((result), __FILE__, __LINE__); \
        return (result); \
    SONYSAT_MACRO_MULTILINE_END
#else /* SONYSAT_TRACE_I2C_ENABLE */
#define SONYSAT_TRACE_I2C_ENTER(func)
#define SONYSAT_TRACE_I2C_RETURN(result) return(result)
#endif /* SONYSAT_TRACE_I2C_ENABLE */


/*------------------------------------------------------------------------------
  Multi-threaded defines
 ------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Defines for basic atomic operations for cancellation. 
*/
typedef struct sonysat_atomic_t {
    /**
     @brief Underlying counter.
    */
    volatile int counter;
} sonysat_atomic_t;
#define sonysat_atomic_set(a,i) ((a)->counter = i)                 /**< Set counter atomically. */
#define sonysat_atomic_read(a) ((a)->counter)                      /**< Get counter atomically. */

/*------------------------------------------------------------------------------
  Stopwatch struct and functions definitions
------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Stopwatch structure to measure accurate time.
*/
typedef struct sonysat_stopwatch_t {
    /**
     @brief Underlying start time.
    */
    uint32_t startTime;

} sonysat_stopwatch_t;

/**
 @brief Start the stopwatch.
        
 @param pStopwatch The stopwatch instance.

 @return SONYSAT_RESULT_OK is successful.
*/
sonysat_result_t sonysat_stopwatch_start (sonysat_stopwatch_t * pStopwatch);

/**
 @brief Pause for a specified period of time.
        
 @param pStopwatch The stopwatch instance.
 
 @param ms  The time in milliseconds to sleep.

 @return SONYSAT_RESULT_OK is successful.
*/
sonysat_result_t sonysat_stopwatch_sleep (sonysat_stopwatch_t * pStopwatch, uint32_t ms);

/**
 @brief Returns the elapsed time (ms) since the stopwatch was started.
        
 @param pStopwatch The stopwatch instance.

 @param The elapsed time in milliseconds.

 @return SONYSAT_RESULT_OK is successful.
 
*/
sonysat_result_t sonysat_stopwatch_elapsed (sonysat_stopwatch_t * pStopwatch, uint32_t* pElapsed);


#endif /* SONYSAT_COMMON_H */
