/** @addtogroup base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2009,2010,2011 Sony Corporation.
 *
 */

#ifndef _MUTEX_UTIL_H_
#define _MUTEX_UTIL_H_

NP_BEGIN_EXTERN_C

/* Since there is no linux process lock the tests only run for windows */
#if defined(_WIN32)

#if defined(_WIN32)
#include "windows.h"

#define TEST_THREAD_HANDLE              HANDLE
#define TEST_SLEEP(x)                   Sleep(x)
#define TEST_THREAD_WAIT(id)            WaitForSingleObject((id), INFINITE)
#define TEST_THREAD_CLOSE(id)           CloseHandle(id)

#elif defined(__linux__)
#include <unistd.h>
#include "pthread.h"

#define TEST_THREAD_HANDLE              pthread_t
#define TEST_SLEEP(x)                   sleep(((x)/100))
#define TEST_THREAD_WAIT(id)            pthread_join((id), NULL)
#define TEST_THREAD_CLOSE(id)
#else
#error "Unknown OS to run the multithread tests against"
#endif

#endif

/* Debug logging control */

#if 0
#define TEST_DEBUG(x) printf(x)
#define TEST_DEBUG1(x,a) printf(x,a)
#define TEST_DEBUG2(x,a,b) printf(x,a,b)
#define TEST_DEBUG3(x,a,b,c) printf(x,a,b,c)
#define TEST_DEBUG4(x,a,b,c,d) printf(x,a,b,c,d)
#else
#define TEST_DEBUG(x)
#define TEST_DEBUG1(x,a)
#define TEST_DEBUG2(x,a,b)
#define TEST_DEBUG3(x,a,b,c)
#define TEST_DEBUG4(x,a,b,c,d)
#endif

/*
 * Temporary file to hold the shared data to protect
 */
#define MUTEX_SHARED_FILENAME "mutex_test.file"

/*
 *
 */
#define MUTEX_TEST_PROC_CNTRL_FILE "tmp.proc"

/*
 * ID of the process lock used to protect the shared data
 */
#define MUTEX_PROC_ID         "proc_mutex"

/* Util function to read from the shared file
 *
 * @param in_data data string read from the file
 * @param in_data_sz size of the string read in (should always be 3 bytes)
 *
 * @return standard return code
 */
retcode_t
mutexUtils_rdSharedData(char *in_data, int32_t *in_data_sz);

/* Util function to write to the shared file
 *
 * @param in_data data string to write to the file
 * @param in_data_sz size of the string to write out (should always be 3 bytes)
 *
 * @return standard return code
 */
retcode_t
mutexUtils_wrSharedData(char *in_data, int32_t in_data_sz);

#if defined(_WIN32)

/*
 * Util function for continuously accessing the shared data
 *
 * @param in_thread_id user supplied id for the thread written to the file
 * @param in_thread_sz size of the string (should always be 1 for now)
 *
 * @return nothing
 */
void
mutexUtils_runThread(char in_thread_id);

/*
 * Util function to spawn a thread that continuously monitors a shared file
 *
 * @param in_id character used as an identifer for the spawned thread
 *
 * @return a void handle to the thread control structure
 */
int32_t
mutexUtils_spawnThread(char *in_id, TEST_THREAD_HANDLE *out_handle);

#endif

NP_END_EXTERN_C

#endif /* _MUTEX_UTIL_H_ */
/** @} */


