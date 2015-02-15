/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised 
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and 
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2839 $
 * ----------------------------------------------------------------------------
 */

#ifndef TZST_H
#define TZST_H

/* ============================================================================
    Includes
============================================================================ */
#include "x_typedef.h"
#include "x_printf.h"
#include "drvcust_if.h"
#include "c_model.h"

#include "tz.h"
#include "tz_macro.h"


/* ===========================================================================
    Physical Addresses
=========================================================================== */
#define TZ_DRAM_SC_SIZE         (TRUSTZONE_MEM_SIZE)
#define PA_CODE_BASE            (TRUSTZONE_CODE_BASE)
#define PA_STACK_LEN            (STACK_SIZE*N_THREADS)

#ifdef CC_SVP_SUPPORT
#if defined(CC_SUPPORT_4K2K)
    #define PA_VFIFO_LEN (FBM_DMX_SIZE_TZ+FBM_DMX2_SIZE_TZ)
#elif defined(CC_FBM_INT_TWO_FBP) || defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)
    #ifdef CC_SUPPORT_5_SEC_PTS_PCR_OFFSET
        #define PA_VFIFO_LEN        (0x2000000)
    #elif CC_FIX_AV_BAD_INTER
        #define PA_VFIFO_LEN        (0x1600000)
    #elif CC_3D_MM_DMX_ADD_2MB
        #ifdef CC_SSIF_SUPPORT
            #define PA_VFIFO_LEN        (0x1100000)
        #else
            #define PA_VFIFO_LEN        (0xC00000)            
        #endif
    #else
        #ifdef CC_SSIF_SUPPORT
            #define PA_VFIFO_LEN        (0xE00000)
        #else
	    #if defined(CC_MT5880)||defined(CC_MT5565)||defined(CC_MT5398)
	    	#define PA_VFIFO_LEN        (0xC00000)
	    #else
            	#define PA_VFIFO_LEN        (0xE00000)
            #endif
        #endif
    #endif
#else
    #ifdef CC_SUPPORT_5_SEC_PTS_PCR_OFFSET
        #define PA_VFIFO_LEN        (0x1000000)
    #elif CC_FIX_AV_BAD_INTER
        #define PA_VFIFO_LEN        (0xB00000)
    #else        
        #define PA_VFIFO_LEN        (0x500000)            
    #endif
#endif
#else /* not CC_SVP_SUPPORT */
#define PA_VFIFO_LEN                (0)
#endif

#ifdef NOLOG
#define DMSG(...)
#define Printf(...)
#else
#define DMSG(...) dprintf("[%s] %s() line%d\n", __FILE__, __FUNCTION__, __LINE__); dprintf(__VA_ARGS__);
#define Printf(...) dprintf(__VA_ARGS__);
#endif

#define TZ_TVP_PROT_FLAG_DMX_SET_KEY    (1u << 0)
#define TZ_TVP_PROT_FLAG_DRM_SET_KEY    (1u << 1)
#define TZ_TVP_PROT_FLAG_SEC_BUF_ALC   (1u << 2)

/* ============================================================================
    Function prototypes
============================================================================ */

extern void enableDebug(UINT32 args);
extern int dprintf(const char *format, ...);
extern INT32 drv_s_dram_MemUtil_Init(void);
extern BOOL HalInitMMU(UINT32 u4Addr);
extern BOOL HalResumeMMU(UINT32 u4Addr);
extern void HalInvalidateTLB(void);
//extern int Printf(const char *format, ...);
extern void HalInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void HalFlushDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void TzGetRandomBytes(UINT8 *pu1_buf, UINT32 u4_len);
extern void TZ_SWD_BufferStart(UINT32 u4Addr, UINT32 u4Size);
extern void TZ_SWD_BufferEnd(UINT32 u4Addr, UINT32 u4Size);
extern void TZ_SWD_FlushInvDCache(UINT32 u4Addr, UINT32 u4Size);
extern void TZ_SWD_InvDCache(UINT32 u4Addr, UINT32 u4Size);
extern void* malloc(unsigned int u4Size);
extern void free(void *p);
extern void* realloc(void* pv_mem, unsigned int  z_new_size);
extern void* calloc(unsigned int a, unsigned int b);
extern void* memmove(void* dest,const void* src,unsigned int n);
extern void* memcpy(void* dest,const void* src,unsigned int n);
extern void* memset(void *s, int c, unsigned int n);
extern int memcmp(const void *s1, const void *s2, unsigned int n);
//extern int strcmp(const char *s1, const char *s2);

// TEE new memcpy/memmove
typedef int (*cust_chk)(void* dst, const void* src, unsigned int n);
extern void* memcpy_n2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memcpy_n2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memcpy_s2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memcpy_s2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memmove_n2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memmove_n2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memmove_s2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);
extern void* memmove_s2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk);

extern BOOL TZ_TVP_Protection(UINT32 u4ProtFlag);
extern BOOL TZ_TVP_UnProtection(UINT32 u4ProtFlag);
extern BOOL TZ_TVP_QUERY_ENABLED(void);
/* ----------------------------------------------------------------------------
 * Description:
 *     This is the Secure World entry point when the Normal World invokes any
 *     SMC; the monitor returns directly to this function which must then 
 *     route the request accordingly.
 *
 * Arguments:
 *     prArgs - the contents of the registers passed in using the low-level
 *         connectionless protocol.
 *
 * Return:
 *     TZAPI returns should be used to enable simpler client implementation.
 *     
 *     This return value is passed directly back to the Normal World by the 
 *     monitor.
 * ------------------------------------------------------------------------- */
UINT32 SvcMgr_Main(TZ_SMC_ARG_T const *prArgs);

#endif  /* TZST_H */
