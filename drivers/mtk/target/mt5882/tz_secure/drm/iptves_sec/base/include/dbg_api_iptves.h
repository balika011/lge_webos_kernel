/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2004,2005,2006,2007,2008,2009 Sony Corporation
 */
#ifndef DBG_API_IPTVES_H
#define DBG_API_IPTVES_H

/*==========================================================================*/
/**
 * @file
 * Debug Support Definition and API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

#if BUILD_LEVEL > 0 && !defined(ENABLE_PROFILING_TIME) && !defined(ENABLE_PROFILING_SPACE)

#  define ENABLE_DEBUG_FUNCTION
#  define COMPONENT(c)  static dbg_cmp_t l_cmp = { "DEBUG_" #c, -1 }
#  define EXECUTE(f,n)                                      \
    do {                                                    \
        if (l_cmp.level < 0) {                              \
            l_cmp.level = dbg_l_GetDebugLevel(l_cmp.cname); \
        }                                                   \
        if (l_cmp.level >= (n)) {                           \
            f;                                              \
        }                                                   \
    } while (0)
#  define EXECUTE2(f,n)                                      \
			do {													\
				if (l_cmp.level < 0) {								\
					l_cmp.level = dbg_l_GetDebugLevel(l_cmp.cname); \
				}													\
				if (l_cmp.level >= (n)) {							\
					dbg_append();												\
					f;												\
				}													\
			} while (0)
#else
#  define COMPONENT(c)
#  define EXECUTE(f,n)
#endif

/*---------------------------------------------------------------------------*/
/**
 * @brief macros for debugging messages
 *
 * @verbatim
 * usage: ({n} is message level)
 *   MSG{n}((format, args...)); // used when displaying message
 *   MSGI{n}((format, args...)); // used when displaying message with indent
 *   DUMP{n}(len, buf); // used at hexadecimal dump
 *   BDUMP{n}(len, buf); // used at hexadecimal dump (for short form)
 *   PRINT{n}(len, buf); // used at character dump
 * message level:
 *   0: display information always (used only for sample code)
 *   1: display error information
 *   2: display serious debug information
 *   3: display verbose debug information
 * The format and args follow the same manner as ddprintf about MSG and MSGI.
 * Note: format and args should be surrounded by duplicated `('s and ')'s.
 * @endverbatim
 */
/*---------------------------------------------------------------------------*/

#define MSG0(x)         dprintf x
#define MSG1(x)         EXECUTE(dprintf x, 1)
#define MSG2(x)         EXECUTE(dprintf x, 2)
#define MSG3(x)         EXECUTE(dprintf x, 3)

#define MSGI0(x)        dbg_append(); dprintf x
#define MSGI1(x)        EXECUTE2(dprintf x, 1)
#define MSGI2(x)        EXECUTE2(dprintf x, 2)
#define MSGI3(x)        EXECUTE2(dprintf x, 3)


#define DUMP0(l,b)      dbg_l_Dump(l,b)
#define DUMP1(l,b)      EXECUTE(dbg_l_Dump(l,b), 1)
#define DUMP2(l,b)      EXECUTE(dbg_l_Dump(l,b), 2)
#define DUMP3(l,b)      EXECUTE(dbg_l_Dump(l,b), 3)

#define BDUMP0(l,b)     dbg_l_ByteDump(l,b)
#define BDUMP1(l,b)     EXECUTE(dbg_l_ByteDump(l,b), 1)
#define BDUMP2(l,b)     EXECUTE(dbg_l_ByteDump(l,b), 2)
#define BDUMP3(l,b)     EXECUTE(dbg_l_ByteDump(l,b), 3)

#define PRINT0(l,b)     dbg_l_Print(l,b)
#define PRINT1(l,b)     EXECUTE(dbg_l_Print(l,b), 1)
#define PRINT2(l,b)     EXECUTE(dbg_l_Print(l,b), 2)
#define PRINT3(l,b)     EXECUTE(dbg_l_Print(l,b), 3)

#define RUN0(x)         x
#define RUN1(x)         EXECUTE(x, 1)
#define RUN2(x)         EXECUTE(x, 2)
#define RUN3(x)         EXECUTE(x, 3)


/*---------------------------------------------------------------------------*/
/**
 * @brief macros for debugging messages at function I/O
 *
 * @verbatim
 * usage:
 *   MSGENT();  // used when announcing the bigging of functions
 *   MSGEXT(r); // used when announcing the end of functions
 * Output function name and increment/decrement indent of message.
 * @endverbatim
 */
/*---------------------------------------------------------------------------*/

#if defined(ENABLE_PROFILING_TIME)
#  define MSGENT()
#  define MSGEXT(r)
#elif defined(ENABLE_PROFILING_SPACE)
#include "prof_api_iptves.h"
#  define MSGENT()      PROF_SPACESTACK()
#  define MSGEXT(r)
#else

#  define MSGENT()      EXECUTE(dbg_l_MsgEnter(__FUNCTION__), 2)
#  define MSGEXT(r)     EXECUTE(dbg_l_MsgExit(__FUNCTION__,r), 2)

#endif

/*---------------------------------------------------------------------------*/
/**
 * @brief macros for debugging messages at function I/O
 *
 * @verbatim
 * usage:
 *   IPTV_ERRLOG(err); // Push err on error stack
 * @endverbatim
 */
/*---------------------------------------------------------------------------*/

#ifndef WRITE_IPTV_ERRLOG
#define WRITE_IPTV_ERRLOG
#endif

#if defined(WRITE_IPTV_ERRLOG)
#  define IPTV_ERRLOG(e,s)           dbg_l_WriteLog(e,s,__FILE__,__FUNCTION__,__LINE__)
#  define CLEARLOG()            dbg_l_ClearErrStack()
#  define PREPARESAVELOG()      int l_dbg_stackdepth
#  define SAVELOG()             l_dbg_stackdepth=dbg_l_SaveErrStack()
#  define RESTORELOG()          dbg_l_RestoreErrStack(l_dbg_stackdepth)
#else
#  define IPTV_ERRLOG(e,s)
#  define CLEARLOG()
#  define PREPARESAVELOG()
#  define SAVELOG()
#  define RESTORELOG()
#endif

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    const char *cname;
    int         level;
} dbg_cmp_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * @brief Output message with indent
 *
 * @param [in] in_fmt           message format
 */
extern void
dbg_l_MsgWithIndent(const char *in_fmt, ...);

/**
 * @brief Output message without indent
 *
 * @param [in] in_fmt           message format
 */
extern void
dbg_l_Msg(const char           *in_fmt, ...);

/**
 * @brief Output function name and increment indent
 *
 * @param [in] in_fname         function name
 */
extern void
dbg_l_MsgEnter(const char      *in_fname);

/**
 * @brief Output function name and decrement indent
 *
 * @param [in] in_fname         function name
 * @param [in] in_ret           return value
 */
extern void
dbg_l_MsgExit(const char       *in_fname,
              u_int32_t         in_ret);

/**
 * @brief Hexadecimal dump for binary data
 *
 * @param [in] in_len           buffer size
 * @param [in] in_buf           buffer address
 */
extern void
dbg_l_Dump(po_size_t            in_len,
           const u_int8_t      *in_buf);

/**
 * @brief Hexadecimal dump for binary data (for short form)
 *
 * @param [in] in_len           buffer size
 * @param [in] in_buf           buffer address
 */
extern void
dbg_l_ByteDump(po_size_t        in_len,
               const u_int8_t  *in_buf);

/**
 * @brief Character dump for binary data
 *
 * @param [in] in_len           buffer size
 * @param [in] in_buf           buffer address
 */
extern void
dbg_l_Print(po_size_t           in_len,
            const u_int8_t     *in_buf);

/**
 * @brief Write error information to error stack
 *
 * @param [in] in_err           error code
 * @param [in] in_ename         error name
 * @param [in] in_fname         file name
 * @param [in] in_func          function name
 * @param [in] in_line          line number
 *
 * @retval in_err
 */
extern u_int32_t
dbg_l_WriteLog(u_int32_t        in_err,
               const char      *in_ename,
               const char      *in_fname,
               const char      *in_func,
               int              in_line);

/**
 * @brief Return debug level of in_cname
 *
 * @retval RET_OK
 */
extern u_int32_t
dbg_l_GetDebugLevel(const char *in_cname);

/**
 * @brief Clear error stack
 *
 * @retval RET_OK
 */
extern int
dbg_l_ClearErrStack(void);

extern int
dbg_l_SaveErrStack(void);

extern int
dbg_l_RestoreErrStack(int       in_depth);



extern int iptv_tee_log_enable;
extern void DumpHex(char *name, const unsigned char *data, int len);






#if !defined(IPTVES_PUBLISH_BUILD)
/**
 * @brief Initialize debug environment
 *
 * @retval RET_OK
 */
extern int
dbg_Init(void);

/**
 * @brief Output error stack
 *
 * @retval RET_OK
 */
extern int
dbg_PrintErrStack(void);

/**
 * @brief Finalize debug environment
 *
 * @retval RET_OK
 */
extern int
dbg_Fin(void);





#else /* IPTVES_PUBLISH_BUILD */

#define dbg_Init()
#define dbg_PrintErrStack()
#define dbg_Fin()

#endif /* !IPTVES_PUBLISH_BUILD */

#endif /* DBG_API_H */

