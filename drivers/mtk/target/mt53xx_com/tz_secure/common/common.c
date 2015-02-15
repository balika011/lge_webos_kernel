/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tzst.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_chipreg.h"
#include "x_rand.h"

#ifdef swap
#undef swap
#endif

EXTERN UINT32 PA_LOAD_BASE;
extern UINT32 TZ_DRAM_START;
extern UINT32 TZ_DRAM_END;

static unsigned long ul_s1 = 0xAB12CE7AUL;
static unsigned long ul_s2 = 0x12DACED1UL;
static unsigned long ul_s3 = 0xD131CD43UL;
static unsigned char b_SecureID_Init = 0;

//--------------------------------------------------------------
// static functions
//--------------------------------------------------------------
// memory range checking
int _chk_range_in_secure(void *start, unsigned int size)
{
    
    //DMSG("[Trustzone][%s] TZ_DRAM_START = 0x%08x, TZ_DRAM_END = 0x%08x, start = 0x%08x, size = %d.\n", __func__, TZ_DRAM_START, TZ_DRAM_END, start, size);
#ifndef CC_ENABLE_TVP_DEBUG

    if (size <= TZ_DRAM_END - TZ_DRAM_START) { // Target range should be smaller than TZ range
        if ((TZ_DRAM_START <= (UINT32)start) && ((UINT32)start + size <= TZ_DRAM_END)) {
            return TRUE;
        }
    } 

    return FALSE;
#else
    return TRUE;
#endif
}

int _chk_range_in_normal(void *start, unsigned int size)
{
#ifndef CC_ENABLE_TVP_DEBUG
    //DMSG("[Trustzone][%s] TZ_DRAM_START = 0x%08x, TZ_DRAM_END = 0x%08x, start = 0x%08x, size = %d.\n", __func__, TZ_DRAM_START, TZ_DRAM_END, start, size);

    if (size >= TZ_DRAM_END - TZ_DRAM_START) { // Target range is bigger than TZ range
        if (((UINT32)start <= TZ_DRAM_START) && (TZ_DRAM_START <= (UINT32)start + size)) {
            return FALSE;
        }
        if (((UINT32)start <= TZ_DRAM_END) && (TZ_DRAM_END <= (UINT32)start + size)) {
            return FALSE;
        }
    } else { // Target range is smaller than TZ range
        if ((TZ_DRAM_START <= (UINT32)start) && ((UINT32)start <= TZ_DRAM_END)) {
            return FALSE;
        }
        if ((TZ_DRAM_START <= (UINT32)start + size) && ((UINT32)start + size <= TZ_DRAM_END)) {
            return FALSE;
        }
    }
#endif
    return TRUE;
}

//--------------------------------------------------------------
// normal functions
//--------------------------------------------------------------
// Use timer2 as random number generator
int rand(void)
{
    if(b_SecureID_Init == 0)
    {
        ul_s3 = BIM_READ32(REG_RW_TIMER2_LOW);
        BIM_WRITE32(0x688, 0x883);
        BIM_WRITE32(0x688, 0x1404);
        ul_s3 = ul_s3 ^ BIM_READ32(0x67c);
        ul_s1 = (((ul_s1 & 4294967294UL) << 12) ^ (((ul_s1 << 13) ^ ul_s1) >> 19) ^ BIM_READ32(0x684));
        ul_s2 = (((ul_s2 & 4294967288UL) <<  4) ^ (((ul_s2 <<  2) ^ ul_s2) >> 25) ^ BIM_READ32(0x678));
        ul_s3 = (((ul_s3 & 4294967280UL) << 17) ^ (((ul_s3 <<  3) ^ ul_s3) >> 11) ^ BIM_READ32(0x680));
        b_SecureID_Init = 1;
    }

    ul_s3 = BIM_READ32(REG_RW_TIMER2_LOW);
    ul_s1 = (((ul_s1 & 4294967294UL) << 12) ^ (((ul_s1 << 13) ^ ul_s1) >> 19));
    ul_s2 = (((ul_s2 & 4294967288UL) <<  4) ^ (((ul_s2 <<  2) ^ ul_s2) >> 25));
    ul_s3 = (((ul_s3 & 4294967280UL) << 17) ^ (((ul_s3 <<  3) ^ ul_s3) >> 11));

    return (int)((ul_s1 ^ ul_s2 ^ ul_s3) & RAND_MAX);
}

void TzGetRandomBytes(UINT8 *pu1_buf, UINT32 u4_len)
{
    UINT32 u4_i = 0;

    for (u4_i = 0; u4_i < u4_len; u4_i++)
    {
        *(UINT8 *)(pu1_buf + u4_i) = (UINT8)(rand() & 0xff);
    }
}


typedef struct
{
    int     (*cmp)(const void*, const void*);
    void    (*swap)(char*, char*, long);
    long    es;
} Sort;

void swapb(char *i, char *j, long es)
{
    char c;

    do {
        c = *i;
        *i++ = *j;
        *j++ = c;
        es--;
    } while(es != 0);
}

void swapi(char *ii, char *ij, long es)
{
    long *i, *j, c;

    i = (long*)ii;
    j = (long*)ij;
    
    do {
        c = *i;
        *i++ = *j;
        *j++ = c;
        es -= sizeof(long);
    } while(es != 0);
}

static char* pivot(char *a, long n, Sort *p)
{
    long j;
    char *pi, *pj, *pk;

    j = n/6 * p->es;
    pi = a + j;     /* 1/6 */
    j += j;
    pj = pi + j;    /* 1/2 */
    pk = pj + j;    /* 5/6 */
    
    if (p->cmp(pi, pj) < 0) 
    {
        if (p->cmp(pi, pk) < 0) 
        {
            if (p->cmp(pj, pk) < 0)
                return pj;
            
            return pk;
        }
        
        return pi;
    }
    
    if (p->cmp(pj, pk) < 0) 
    {
        if (p->cmp(pi, pk) < 0)
            return pi;
        
        return pk;
    }
    
    return pj;
}

void qsorts(char *a, long n, Sort *p)
{
    long j, es;
    char *pi, *pj, *pn;

    es = p->es;
    while (n > 1) 
    {
        if (n > 10) 
        {
            pi = pivot(a, n, p);
        } 
        else
            pi = a + (n>>1)*es;

        p->swap(a, pi, es);
        pi = a;
        pn = a + n*es;
        pj = pn;
        
        for (;;) 
        {
            do
                pi += es;
            while(pi < pn && p->cmp(pi, a) < 0);
            
            do
                pj -= es;
            while (pj > a && p->cmp(pj, a) > 0);
            
            if (pj < pi)
                break;
            
            p->swap(pi, pj, es);
        }
        
        p->swap(a, pj, es);
        j = (pj - a) / es;

        n = n-j-1;
        if (j >= n) 
        {
            qsorts(a, j, p);
            a += (j+1)*es;
        } 
        else 
        {
            qsorts(a + (j+1)*es, n, p);
            n = j;
        }
    }
}

void TZ_SWD_BufferStart(UINT32 u4Addr, UINT32 u4Size)
{
    if (u4Addr == 0 || u4Size == 0)
    {
        return;
    }
    
    if (((u4Addr & TZ_CACHE_LINE_SIZE_MSK) != 0) || (((u4Size & TZ_CACHE_LINE_SIZE_MSK) != 0)))
    {
        while(1);
    }
    
    HalInvalidateDCacheMultipleLine(u4Addr, u4Size);
}

void TZ_SWD_BufferEnd(UINT32 u4Addr, UINT32 u4Size)
{
    if (u4Addr == 0 || u4Size == 0)
    {
        return;
    }
    
    if (((u4Addr & TZ_CACHE_LINE_SIZE_MSK) != 0) || (((u4Size & TZ_CACHE_LINE_SIZE_MSK) != 0)))
    {
        while(1);
    }
    
    HalFlushInvalidateDCacheMultipleLine(u4Addr, u4Size);
}

void TZ_SWD_FlushDCache(UINT32 u4Addr, UINT32 u4Size)
{
    HalFlushDCacheMultipleLine(u4Addr, u4Size);
}

void TZ_SWD_InvDCache(UINT32 u4Addr, UINT32 u4Size)
{
    HalInvalidateDCacheMultipleLine(u4Addr, u4Size);
}

void TZ_SWD_FlushInvDCache(UINT32 u4Addr, UINT32 u4Size)
{
    HalFlushInvalidateDCacheMultipleLine(u4Addr, u4Size);
}

#ifdef CC_TVP_SUPPORT

#include "x_dram.h"

extern unsigned long sec_fbm_size ;
extern unsigned long sec_mjc_size ;
#define SECURE_FBM_SIZE (sec_fbm_size)
#define SECURE_MJC_SIZE (sec_mjc_size)


static UINT32 _u4TvpProtFlags = 0;

void BIM_Trustzone_DRAM_Protection_TVP(UINT32 base, UINT32 size)
{
    UINT32 u4DramSc;
    
    TZ_DRAM_START = base;
    TZ_DRAM_END   = base + size;  

    DMSG("[TZ SFBM]TZ dram: start=0x%08X, end=0x%08X\n", base, base+size);
	
    // 64-bytes alignment for mustang bim
    UNUSED(u4DramSc);
    if((TZ_DRAM_START & 0x3f) != 0 || (TZ_DRAM_END & 0x3f) != 0)
    {
        tz_printf("TZ dram protection range should be 64-bytes aligned. Invalid range 0x%08x~0x%08x\n", TZ_DRAM_START, TZ_DRAM_END); 
    }

    BIM_WRITE32(0x10, TZ_DRAM_START);   //DRAM_SC_BEGIN
    BIM_WRITE32(0x14, TZ_DRAM_END);   //DRAM_SC_END
    BIM_WRITE32(0x8, BIM_READ32(0x8) | (1 << 4));  // SECURE_MAP
}


extern UINT32 sec_fbm_start;
extern UINT32 sec_fbm_end;
extern UINT32 sec_heap_start;
extern UINT32 sec_heap_end;
extern UINT32 _u4Debug;

UINT32 GetSecureFBMStart(void)
{
	return PA_LOAD_BASE - SECURE_FBM_SIZE;;
}

UINT32 GetSecureFBMEnd(void)
{
	return PA_LOAD_BASE; 
}

extern void EnableSeucreMID(void);
extern void DisableSecureMID(void);

static UINT32 fgIRQRestore;

BOOL TZ_TVP_QUERY_ENABLED(void)
{
	if(_u4TvpProtFlags!=0)
		return TRUE;
	return FALSE;
}

BOOL TZ_TVP_Protection(UINT32 u4ProtFlag)
{
	if(!u4ProtFlag)
	{
		DMSG("[TZ WARNING]protection flag cannot be zero\n");
        return FALSE;
	}

    if (!_u4TvpProtFlags)
    {
    	if(_u4Debug != 3)
    	{
        	sec_fbm_start = PA_LOAD_BASE - SECURE_FBM_SIZE;
        	sec_fbm_end = PA_LOAD_BASE + TZ_DRAM_SC_SIZE + SECURE_MJC_SIZE;
    	}
		else
		{
			tz_printf("debug mode 3\n");
			tz_printf("sec_fbm_start = %x,sec_fbm_end = %x\n",sec_fbm_start,sec_fbm_end);
		}
        sec_heap_start = PA_LOAD_BASE;
        sec_heap_end = PA_LOAD_BASE + TZ_DRAM_SC_SIZE;

        if(BIM_IsIrqEnabled(VECTOR_DRAMC)){
            BIM_DisableIrq(VECTOR_DRAMC);
            fgIRQRestore |=0x1;
        }
        if(BIM_IsIrqEnabled(VECTOR_DRAMC_CHB)){
            BIM_DisableIrq(VECTOR_DRAMC_CHB);
            fgIRQRestore |=0x1<<1;            
        }
        if(BIM_IsIrqEnabled(VECTOR_DRAMC_C)){
            BIM_DisableIrq(VECTOR_DRAMC_C);
            fgIRQRestore |=0x1<<2;            
        }  

        DMSG("sec_fbm_start = %x,sec_fbm_end = %x\n",sec_fbm_start,sec_fbm_end);
        BIM_Trustzone_DRAM_Protection_TVP(sec_fbm_start,sec_fbm_end-sec_fbm_start);
        EnableSeucreMID();
    }

    _u4TvpProtFlags |= u4ProtFlag;

	return TRUE;
}

BOOL TZ_TVP_UnProtection(UINT32 u4ProtFlag)
{
    if(!u4ProtFlag)
	{
		DMSG("[TZ WARNING]un-protection flag cannot be zero\n");
        return FALSE;
	}

    _u4TvpProtFlags &= ~u4ProtFlag;

    if (!_u4TvpProtFlags)
    {        
        BIM_Trustzone_DRAM_Protection_TVP(PA_LOAD_BASE,TZ_DRAM_SC_SIZE);
	    DisableSecureMID();
	    DMSG("sec_fbm_start = %x,sec_fbm_end = %x\n",PA_LOAD_BASE,PA_LOAD_BASE+TZ_DRAM_SC_SIZE);
	    
        //All Secure Buffer Already be release
        if(fgIRQRestore&(0x1<<0)){
            BIM_ClearIrq(VECTOR_DRAMC);
            BIM_EnableIrq(VECTOR_DRAMC);
            fgIRQRestore &= ~(fgIRQRestore&(0x1<<0));
        }
        if(fgIRQRestore&(0x1<<1)){
            BIM_ClearIrq(VECTOR_DRAMC_CHB);            
            BIM_EnableIrq(VECTOR_DRAMC_CHB);
            fgIRQRestore &= ~(fgIRQRestore&(0x1<<1));           
        }    
        if(fgIRQRestore&(0x1<<2)){
            BIM_ClearIrq(VECTOR_DRAMC_C);            
            BIM_EnableIrq(VECTOR_DRAMC_C);
            fgIRQRestore &= ~(fgIRQRestore&(0x1<<2));           
        }          
    }

	return TRUE;
}
#else
BOOL TZ_TVP_QUERY_ENABLED(void)
{
	return FALSE;
}

BOOL TZ_TVP_Protection(UINT32 u4ProtFlag)
{
	Printf("[TZ WARNING]TVP_SUPPORT is not enabled\n");
	return FALSE;
}

BOOL TZ_TVP_UnProtection(UINT32 u4ProtFlag)
{
	Printf("[TZ WARNING]TVP_SUPPORT is not enabled\n");
	return FALSE;
}
#endif

// TEE new memcpy/memmove
void* memcpy_n2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_secure((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_secure(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memcpy */
    return __aeabi_memcpy(dst, src, n);

}

void* memcpy_n2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_secure((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_normal(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memcpy */
    return __aeabi_memcpy(dst, src, n);

}

void* memcpy_s2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_normal((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_secure(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memcpy */
    return __aeabi_memcpy(dst, src, n);

}

void* memcpy_s2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_normal((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_normal(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memcpy */
    return __aeabi_memcpy(dst, src, n);

}

void* memcpy(void* dest,const void* src,unsigned int n)
{
#ifdef CC_TEE_FORCE_MEM_CHK
    return memcpy_s2s_chk(dest, src, n, NULL);
#else
    return __aeabi_memcpy(dest, src, n);
#endif
}

void* memmove_n2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_secure((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_secure(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memmove */
    return __aeabi_memmove(dst, src, n);

}

void* memmove_n2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_secure((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_normal(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memmove */
    return __aeabi_memmove(dst, src, n);

}

void* memmove_s2n_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_normal((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_secure(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memmove */
    return __aeabi_memmove(dst, src, n);

}

void* memmove_s2s_chk(void* dst, const void* src, unsigned int n, cust_chk* pfn_chk)
{
#ifndef CC_ENABLE_TVP_DEBUG
    int ret = 0;

    DMSG("[Trustzone][%s] dst = 0x%08x, src = 0x%08x, n = %d.\n", __func__, dst, src, n);

    /* Check src range */
    if (_chk_range_in_normal((void*)src, n)) {
        DMSG("[Trustzone][%s] src range check fail.\n", __func__);
        while (1);
    }

    /* Check dst range */
    if (_chk_range_in_normal(dst, n)) {
        DMSG("[Trustzone][%s] dst range check fail.\n", __func__);
        while (1);
    }

    /* Customize check */
    if (pfn_chk != NULL) {
        ret = (*pfn_chk)(dst, src, n); 
        if (ret != 0) {
            DMSG("[Trustzone][%s] customize check fail, ret = %d.\n", __func__, ret);
            while (1);
        }
    }
#endif
    /* Perform memmove */
    return __aeabi_memmove(dst, src, n);

}

void* memmove(void* dest,const void* src,unsigned int n)
{
#ifdef CC_TEE_FORCE_MEM_CHK
    return memmove_s2s_chk(dest, src, n, NULL);
#else
    return __aeabi_memmove(dest, src, n);
#endif

}

