/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2004,2005,2006,2007,2008,2009 Sony Corporation
 */
#ifndef PROF_API_IPTVES_H
#define PROF_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/**
 * @file
 * Profiling (Time Performance measurement) Library
 *
 * require: base
 */
/*===========================================================================*/

#if defined(ENABLE_PROFILING_TIME)

#if defined(_WIN32) /* Windows */

#include <windows.h>
typedef struct {
    const char *title;
    LARGE_INTEGER ts,te,td,tf;
} prof_l_context_t;

#define PROF_L_TIMEINIT(c) prof_l_context_t c
#define PROF_L_TIMESTART(c,t) do {              \
        prof_l_IncIndent();                     \
        (c).title=(t);                          \
        QueryPerformanceCounter(&((c).ts));     \
    } while (0)
#define PROF_L_TIMESTOP(c) do {                 \
        QueryPerformanceCounter(&((c).te));     \
        prof_l_DecIndent();                     \
    } while (0)
#define PROF_L_TIMEDIFF(c) do {                                         \
        QueryPerformanceFrequency(&((c).tf));                           \
        (c).td.QuadPart = (c).te.QuadPart-(c).ts.QuadPart;              \
        (c).td.QuadPart = (c).td.QuadPart * 1000000 / (c).tf.QuadPart;  \
    } while (0)
#define PROF_L_TIMEPRINT(c) do {                                        \
        u_int32_t _i;                                                   \
        PROF_L_TIMEDIFF(c);                                             \
        MSG0(("***\n"));                                                \
        MSG0(("*** Profile(elapsed time):"));                           \
        for (_i = 0; _i < prof_l_GetIndent(); _i++) MSG0((" "));        \
        MSG0(("%s = %d.%06d [sec]\n", (c).title, (int)(c).td.QuadPart/1000000, (int)(c).td.QuadPart%1000000)); \
        MSG0(("***\n"));                                                \
    } while (0)
#define PROF_L_TIMEGETSEC(x,c) do{PROF_L_TIMEDIFF(c);(x)=(int)(((c).td.QuadPart)/1000000);}while(0)
#define PROF_L_TIMEGETUSEC(x,c) do{PROF_L_TIMEDIFF(c);(x)=(int)(((c).td.QuadPart)%1000000);}while(0)

#else /* linux */

#include <sys/time.h>
typedef struct {
    const char *title;
    struct timeval ts, te, td;
} prof_l_context_t;

#define PROF_L_TIMEINIT(c) prof_l_context_t c
#define PROF_L_TIMESTART(c,t) do {              \
        prof_l_IncIndent();                     \
        (c).title=(t);                          \
        gettimeofday(&((c).ts), 0);             \
    } while (0)
#define PROF_L_TIMESTOP(c) do {                 \
        gettimeofday(&((c).te), 0);             \
        prof_l_DecIndent();                     \
    } while (0)
#define PROF_L_TIMEDIFF(c) do {                                         \
        (c).td.tv_sec = (c).te.tv_sec - (c).ts.tv_sec;                  \
        if ( ((c).td.tv_usec = (c).te.tv_usec - (c).ts.tv_usec) < 0 ) { \
            (c).td.tv_usec += 1000000;                                  \
            (c).td.tv_sec --;                                           \
        }                                                               \
    } while (0)
#define PROF_L_TIMEPRINT(c) do {                                        \
        u_int32_t _i;                                                   \
        PROF_L_TIMEDIFF(c);                                             \
        MSG0(("***\n"));                                                \
        MSG0(("*** Profile(elapsed time):"));                           \
        for (_i = 0; _i < prof_l_GetIndent(); _i++) MSG0((" "));        \
        MSG0(("%s = %d.%06d [sec]\n", (c).title, (c).td.tv_sec, (c).td.tv_usec)); \
        MSG0(("***\n"));                                                \
    } while (0)
#define PROF_L_TIMEGETSEC(x,c) do{PROF_L_TIMEDIFF(c);(x)=(c).td.tv_sec;}while(0)
#define PROF_L_TIMEGETUSEC(x,c) do{PROF_L_TIMEDIFF(c);(x)=(c).td.tv_usec;}while(0)

#endif

extern u_int32_t prof_l_GetIndent(void);
extern void      prof_l_IncIndent(void);
extern void      prof_l_DecIndent(void);

#endif /* ENABLE_PROFILING_TIME */

#if defined(ENABLE_PROFILING_SPACE)
#define PROF_L_INDEX_MAX 2
typedef struct {
    int32_t size;
    int32_t count;
    int32_t esize; /* estimated size */
} prof_l_memstat_t;
typedef struct {
    const char *title;
    int32_t sz_stack;
#if 0
    prof_l_memstat_t sz_max[PROF_L_INDEX_MAX];
    prof_l_memstat_t sz_min[PROF_L_INDEX_MAX];
    prof_l_memstat_t bk_max[PROF_L_INDEX_MAX];
    prof_l_memstat_t bk_min[PROF_L_INDEX_MAX];
#endif
} prof_l_context_t;
extern int prof_l_SpaceStart(prof_l_context_t *io_context);
extern int prof_l_SpaceStop(prof_l_context_t *io_context);
extern int prof_l_SpacePrint(prof_l_context_t *io_context);
extern int prof_l_SpaceStack(void);
extern int prof_l_SpaceAlloc(int idx, int32_t in_sz);
extern int prof_l_SpaceFree(int idx, int32_t in_sz);
extern void prof_l_SpaceCurrentStatus(int idx);
#define PROF_L_SPACEINIT(c) prof_l_context_t c
#define PROF_L_SPACESTART(c,t) do{(c).title=(t);prof_l_SpaceStart(&c);}while(0)

#endif

/*===========================================================================*/
/** @brief
 *
 * PROF_INIT() should be placed at the end of variable declaration part in a function.
 * The argument 't' of PROF_START() should be read-only constant string.
 * PROF_FIN() should be placed just before return of the function.
 */
/*===========================================================================*/
#if defined(ENABLE_PROFILING_TIME)
#define PROF_INIT() PROF_L_TIMEINIT(prof_l_context)
#define PROF_TIMESTART(t) PROF_L_TIMESTART(prof_l_context,t)
#define PROF_TIMESTOP() PROF_L_TIMESTOP(prof_l_context)
#define PROF_TIMEPRINT() PROF_L_TIMEPRINT(prof_l_context)
#define PROF_TIMEGETSEC(x) PROF_L_TIMEGETSEC(x,prof_l_context)
#define PROF_TIMEGETUSEC(x) PROF_L_TIMEGETUSEC(x,prof_l_context)
#define PROF_SPACEALLOC(idx,sz)
#define PROF_SPACEFREE(idx,sz)
#define PROF_SPACEPRINT()
#define PROF_SPACESTACK()
#define PROF_SPACESTATUS(idx)
#define PROF_START(t) PROF_TIMESTART(t)
#define PROF_STOP() PROF_TIMESTOP()
#define PROF_PRINT() PROF_TIMEPRINT()
#define PROF_FIN()
#elif defined(ENABLE_PROFILING_SPACE)
#define PROF_INIT() PROF_L_SPACEINIT(prof_l_context)
#define PROF_TIMESTART(t)
#define PROF_TIMESTOP()
#define PROF_TIMEPRINT()
#define PROF_TIMEGETSEC(x)
#define PROF_TIMEGETUSEC(x)
#define PROF_SPACEALLOC(idx,sz) prof_l_SpaceAlloc(idx,sz)
#define PROF_SPACEFREE(idx,sz) prof_l_SpaceFree(idx,sz)
#define PROF_SPACEPRINT() prof_l_SpacePrint(&prof_l_context)
#define PROF_SPACESTACK() prof_l_SpaceStack()
#define PROF_SPACESTATUS(idx) prof_l_SpaceCurrentStatus(idx)
#define PROF_START(t) PROF_L_SPACESTART(prof_l_context,t)
#define PROF_STOP() prof_l_SpaceStop(&prof_l_context)
#define PROF_PRINT() PROF_SPACEPRINT()
#define PROF_FIN()
#else
#define PROF_INIT()
#define PROF_TIMESTART(t)
#define PROF_TIMESTOP()
#define PROF_TIMEPRINT()
#define PROF_TIMEGETSEC(x)
#define PROF_TIMEGETUSEC(x)
#define PROF_SPACEALLOC(idx,sz)
#define PROF_SPACEFREE(idx,sz)
#define PROF_SPACEPRINT()
#define PROF_SPACESTACK()
#define PROF_SPACESTATUS(idx)
#define PROF_START(t)
#define PROF_STOP()
#define PROF_PRINT()
#define PROF_FIN()
#endif

#ifdef __cplusplus
}
#endif

#endif /* PROF_API_H */
/** @} */
