/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#include "x_typedef.h"
#include "x_os.h"
#include "tzst.h"

EXTERN UINT32 PA_HEAP_START, PA_HEAP_END;

#if __LINUX_ARM_ARCH__ >= 7
#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")
#elif __LINUX_ARM_ARCH__ == 6
#define isb() __asm__ __volatile__ ("mcr p15, 0, %0, c7, c5,  4" : : "r" (0) : "memory")
#define dsb() __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define dmb() __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 5" : : "r" (0) : "memory")
#endif

#define TzCritStart(x)      \
    asm volatile (          \
    "mrs    %0, cpsr\n"     \
    "cpsid  if"             \
    : "=r" (x) : : "memory", "cc")

#define TzCritEnd(x)        \
    asm volatile (          \
    "msr    cpsr_c, %0"     \
    : : "r" (x) : "memory", "cc")

static inline void dsb_sev(void)
{
#if __LINUX_ARM_ARCH__ >= 7
    __asm__ __volatile__ (
    "dsb\n"
    "sev");
#elif __LINUX_ARM_ARCH__ == 6
    __asm__ __volatile__ (
    "mcr p15, 0, %0, c7, c10, 4\n" //DSB
    : : "r" (0));
#endif
}
    
#if __LINUX_ARM_ARCH__ >= 7
    #define smp_mb()    dmb()
    #define smp_rmb()   dmb()
    #define smp_wmb()   dmb()
#else
    #define barrier() __asm__ __volatile__("": : :"memory")
    #define smp_mb()    barrier()
    #define smp_rmb()   barrier()
    #define smp_wmb()   barrier()
#endif

typedef struct 
{
    volatile unsigned int lock;
} tz_spinlock_t;

typedef struct S_DRAM_HEADER_STRUCT
{
    struct S_DRAM_HEADER_STRUCT *next_memory;
    struct S_DRAM_HEADER_STRUCT *previous_memory;
    UINT32                       memory_free;
} S_DRAM_HEADER;

static S_DRAM_HEADER *s_dram_memory_list;
static tz_spinlock_t tz_s_dram_lock = {0};

static inline void tz_spin_lock(tz_spinlock_t *lock)
{
    unsigned long tmp;

    __asm__ __volatile__(
    "1:         ldrex    %0, [%1]\n"
    "teq        %0, #0\n"
    "strexeq    %0, %2, [%1]\n"
    "teqeq      %0, #0\n"
    "bne        1b"
    : "=&r" (tmp)
    : "r" (&lock->lock), "r" (1)
    : "cc");

    smp_mb();
}

static inline void tz_spin_unlock(tz_spinlock_t *lock)
{
    smp_mb();

    __asm__ __volatile__(
    "str    %1, [%0]\n"
    :
    : "r" (&lock->lock), "r" (0)
    : "cc");

    dsb_sev();
}


//-----------------------------------------------------------------------------
void* malloc(size_t u4Size)
{
    void *p = NULL;
    S_DRAM_HEADER *memory_ptr = s_dram_memory_list;
    int crit;
    UINT32 free_size;

    if (u4Size == 0)
    {
        return p;
    }
    
    u4Size = (u4Size + 3) & ~3;

    TzCritStart(crit);
    tz_spin_lock(&tz_s_dram_lock);
    
    do
    {
        if (memory_ptr->memory_free)
        {
            free_size = (UINT32)(memory_ptr->next_memory) - (UINT32)memory_ptr - sizeof(S_DRAM_HEADER);
            if (free_size >= u4Size)
            {
                if (free_size >= u4Size + sizeof(S_DRAM_HEADER) * 2)
                {
                    S_DRAM_HEADER *new_ptr = (S_DRAM_HEADER *)((UINT32)memory_ptr + u4Size + sizeof(S_DRAM_HEADER));
                    new_ptr->memory_free     = TRUE;
                    new_ptr->previous_memory = memory_ptr;
                    new_ptr->next_memory     = memory_ptr->next_memory;
                    new_ptr->next_memory->previous_memory = new_ptr;
                    memory_ptr->next_memory  = new_ptr;
                }
                
                memory_ptr->memory_free = FALSE;
                p = (void *)((UINT32)memory_ptr + sizeof(S_DRAM_HEADER));
                break;
            }
        }
        
        memory_ptr = memory_ptr->next_memory;
    } while (memory_ptr != s_dram_memory_list);
    
    tz_spin_unlock(&tz_s_dram_lock);
    TzCritEnd(crit);
    
    return p;
}

void free(void *p)
{
    S_DRAM_HEADER *header_ptr;
    int crit;

    if (p == NULL)
    {
        Printf("TZ Heap free out of range!\n");
        return;
    }
    
    if (!((UINT32)p >= PA_HEAP_START && (UINT32)p < PA_HEAP_END))
    {
        while (1);
    }

    TzCritStart(crit);
    tz_spin_lock(&tz_s_dram_lock);
    
    header_ptr = (S_DRAM_HEADER *)((UINT32)p - sizeof(S_DRAM_HEADER));
    header_ptr->memory_free = TRUE;

    if (header_ptr->previous_memory->memory_free)
    {
        header_ptr->previous_memory->next_memory = header_ptr->next_memory;
        header_ptr->next_memory->previous_memory = header_ptr->previous_memory;
        header_ptr = header_ptr->previous_memory;
    }
    
    if (header_ptr->next_memory->memory_free)
    {
        S_DRAM_HEADER *new_ptr = header_ptr->next_memory;
        new_ptr->next_memory->previous_memory = header_ptr;
        header_ptr->next_memory = new_ptr->next_memory;
    }
    
    tz_spin_unlock(&tz_s_dram_lock);
    TzCritEnd(crit);
}

static void *realloc_s_dram_mem(void *pv_mem, size_t z_new_size)
{
    void *pv_new;
    size_t z_size;
    S_DRAM_HEADER* pv_mem_temp;
    pv_mem_temp=(S_DRAM_HEADER *)((UINT32)pv_mem - sizeof(S_DRAM_HEADER));

    z_size =(UINT32)(pv_mem_temp->next_memory) - (UINT32)pv_mem_temp - sizeof(S_DRAM_HEADER);

    if (z_new_size <= z_size)
    {
        /* make use the existing pv_mem */
        return pv_mem;
    }
    
    pv_new = malloc(z_new_size);
    if (pv_new == NULL)
    {
        return ((void *)NULL);
    }

    /* copy data to new memory and put the memory handle to free bucket */
    memcpy(pv_new, pv_mem, z_size);
    free(pv_mem);
    return pv_new;
}

void* realloc(void* pv_mem, size_t z_new_size)
{
    VOID *pv_m;
    if ((pv_mem != NULL) && (((UINT32)pv_mem) & 0x3))
    {
        while (1);
    }
    
    if (pv_mem == NULL)
    {
        pv_m = malloc(z_new_size);
        return pv_m;
    }

    /* If z_new_size is 0, the call is equivalent to x_free_s_dram_mem(pv_new) */
    if (z_new_size == 0)
    {
        free(pv_mem);
        return NULL;
    }

    pv_m = realloc_s_dram_mem(pv_mem, z_new_size);
    return pv_m;
}

//NAUTILUS_FOR_LINUX
void* calloc(size_t a, size_t b)
{
    void *p = malloc(a*b);
    if (p)
    {
        memset(p, 0, a*b);
    }
    
    return p;
}

void* x_alloc_s_dram_mem(size_t u4Size)
{
    return (void *)x_mem_alloc(u4Size);
}

void x_free_s_dram_mem(void* p)
{
    x_mem_free(p);
}

void* x_realloc_s_dram_mem(void *p, size_t u4Size)
{
    return (void *)x_mem_realloc(p, u4Size);
}

void* x_calloc_s_dram_mem(size_t a, size_t b)
{
    return (void *)x_mem_calloc(a, b);
}

INT32 drv_s_dram_MemUtil_Init(void)
{
    s_dram_memory_list = (S_DRAM_HEADER *)(PA_HEAP_START);
    S_DRAM_HEADER *header_ptr = (S_DRAM_HEADER *)PA_HEAP_END - 1;

    s_dram_memory_list->next_memory     = header_ptr;
    s_dram_memory_list->previous_memory = header_ptr;
    s_dram_memory_list->memory_free     = TRUE;
    
    header_ptr->next_memory     = s_dram_memory_list;
    header_ptr->previous_memory = s_dram_memory_list;
    header_ptr->memory_free     = 0;
    
    tz_printf("TZ Heap: start=0x%08X, end=0x%08X\n", PA_HEAP_START, PA_HEAP_END);
    
    if (PA_HEAP_START >= PA_HEAP_END)
    {
        Printf("TZ Heap out of range!\n");
        while (1);
    }
    
    return 0;
}

