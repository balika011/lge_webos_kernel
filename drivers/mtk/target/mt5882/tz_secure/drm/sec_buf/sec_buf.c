
#include "tzst.h"
#include "sec_buf.h"
#include "x_os.h"
#include "x_typedef.h"
#include "tzst.h"

#define true 1
#define false 0

#define MAX_SECTION_NR 50


typedef struct
{
    UINT32 u4Addr;
    UINT32 u4Size;        //Size to be alloc
    UINT32 session_id;
    UINT32 u4Flag;
    UINT32 u4Used;
    UINT32 u4Empty;
}SEC_BUF;

SEC_BUF sec_list[MAX_SECTION_NR];

#define VDEC_ALIGN 0x8000

#define SEC_ELE_STR(idx) (sec_list[(idx)].u4Addr)
#define SEC_ELE_END(idx) (sec_list[(idx)].u4Addr + sec_list[(idx)].u4Size)


static UINT32 sec_base;//base addr of secure buffer
static UINT32 sec_size;//maximun size of secure buffer
static UINT32 sec_next;//next allocate addr


extern unsigned long sec_drm_addr;
extern unsigned long sec_drm_size;

static UINT8 session_next;

static inline void TZ_CACHE_STR(void* addr , size_t size) 
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferStart((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

static inline void TZ_CACHE_END(void* addr , size_t size)
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferEnd((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

BOOL TZ_SEC_BUF_OPEN_SESSION(void *arg, UINT32 u4Size)
{
#if 0
    UINT32 i,sid=0;
    UINT32 *session_id = (UINT32 *)arg;
    BOOL session_found = false;

    while(session_found == true)
    {
        sid = session_next++;
        session_found = true;
        //check if session id used, skip
        for( i =0 ; i< MAX_SECTION_NR ; ++i)
        {
            if((sec_list[i].session_id == sid)&&(sec_list[i].u4Size!=0))
                session_found = false;
        }
    }

    if(session_found)
    {
        *session_id = sid;
    }
#endif
    return true;
}

static void sec_buf_init()
{
    static unsigned char sec_init = 0;

    if(sec_init == 0)
    {
#if defined(CC_TVP_SUPPORT)
        sec_base = (sec_drm_addr + VDEC_ALIGN -1) & ~(VDEC_ALIGN -1);
        sec_size =  sec_drm_size - (sec_base - sec_drm_addr);
        sec_next = sec_base;
#endif        
        sec_init = 1;
    }
}

char sec_buf_verify(UINT32 u4Addr , UINT32 u4Size)
{
    if((sec_base <= u4Addr)&&(sec_base + sec_size >= u4Addr + u4Size))
        return true;

    return false;
}

BOOL TZ_SEC_BUF_CLOSE_SESSION(void *arg, UINT32 u4Size)
{
#if 0
    UINT32 sid = *(UINT32 *)arg;
    UINT32 i;
    for( i =0 ; i< MAX_SECTION_NR ; ++i)
    {
        if(sec_list[i].session_id == sid)
        {
            sec_list[i].u4Size = 0; 
        }
    }
#endif    
    return true;
}

BOOL TZ_SEC_BUF_CPB_MOVE_DATA(void *arg, UINT32 u4ArgSize){
    TZ_DRM_SEC_BUF_CPB_MOVEDATA_T *rArg = (TZ_DRM_SEC_BUF_CPB_MOVEDATA_T*)arg;


    UINT32 i;
    UINT32 u4Size,u4Des;

    TZ_CACHE_STR(rArg->pu4Size, sizeof(UINT32));
    TZ_CACHE_STR(rArg->pu4Des, sizeof(UINT32));    

    u4Size = *rArg->pu4Size;
    u4Des = *rArg->pu4Des;
    //Find The Allocated Region

    *rArg->pu4Size =0;

    for(i=0;i< MAX_SECTION_NR;++i)
    {
        if((sec_list[i].u4Size != 0)
                &&(sec_list[i].u4Addr <= u4Des)
                &&((sec_list[i].u4Addr + sec_list[i].u4Size)> u4Des))
        {
            break;
        }
    }

    if(i==MAX_SECTION_NR)
        goto final;
    
    //Source is inside CPB
    if(sec_buf_verify(rArg->u4Src , u4Size)==false)
        goto final;


    //Cyclic Buffer
    if(u4Des + u4Size > sec_list[i].u4Addr + sec_list[i].u4Size)
        u4Des = sec_list[i].u4Addr;
    
    //Copy Size inside Range
    if(u4Des + u4Size > sec_list[i].u4Addr + sec_list[i].u4Size)
        goto final;

    //RD / WR Point check
    if((u4Des<rArg->u4RDP)&&(u4Des + u4Size >= rArg->u4RDP))
        goto final;

    HalInvalidateDCacheMultipleLine(u4Des, u4Size);
    memcpy_s2s_chk(u4Des , rArg->u4Src, u4Size, NULL);
    HalFlushInvalidateDCacheMultipleLine(u4Des, u4Size);

    *rArg->pu4Des =  u4Des;
    *rArg->pu4Size = u4Size;

final:

    TZ_CACHE_END(rArg->pu4Des, sizeof(UINT32));    
    TZ_CACHE_END(rArg->pu4Size, sizeof(UINT32));
    return true;
}

BOOL TZ_SEC_BUF_CPB_GET_DATA(void *arg, UINT32 u4ArgSize){
    TZ_DRM_SEC_BUF_CPB_GETDATA_T *rArg = (TZ_DRM_SEC_BUF_CPB_GETDATA_T*)arg;
#ifdef DEBUG_ONLY_FUNCTION
    UINT32 u4SecureSrc;
    UINT8 *pu4NormalDes;
    BOOL ret = false;

    TZ_CACHE_STR(rArg->pu4NormalDes, rArg->len);
    
    if(sec_buf_verify(rArg->u4SecureSrc, rArg->len)==false)
        goto final;
    memcpy_s2n_chk(rArg->pu4NormalDes, rArg->u4SecureSrc,  rArg->len, NULL);
    ret = true;
    
final:
    TZ_CACHE_END(rArg->pu4NormalDes, rArg->len);
    return ret;
#else
    return false;
#endif
}

BOOL TZ_SEC_BUF_ALLOC(void *arg, UINT32 u4Size)
{
    TZ_DRM_SEC_BUF_ALLOC_T *rArg = (TZ_DRM_SEC_BUF_ALLOC_T *)arg;
    UINT32 i;
    UINT32 alloc_addr; 
    BOOL ret = false;

    //Alloc Size is always VDEC_ALIGNED
    UINT32 allocSize = (rArg->u4Size + VDEC_ALIGN -1) & ~(VDEC_ALIGN -1);

    TZ_CACHE_STR(rArg->u4Handle, sizeof(UINT32));
    *rArg->u4Handle =0;

    sec_buf_init();

    alloc_addr= sec_next; 

    if((alloc_addr - sec_base + allocSize >  sec_size)) // Size is not enough
    {
        //Search for Lowest Addr Being used
        for(i=0;i< MAX_SECTION_NR;++i)
        {
            if((sec_list[i].u4Size != 0)&&(sec_list[i].u4Addr < alloc_addr))
            {
                alloc_addr = sec_list[i].u4Addr;
            }
        }

        if( sec_base + rArg->u4Size < alloc_addr )
        {
            alloc_addr = sec_base;
        }
        else
        {
            //Both Upper and Lower Addr not enough for allocation
            goto final;
        }
    }

    //Find an empty list item to put the allocated buffer
    for(i=0;i< MAX_SECTION_NR;++i)
    {
        if(sec_list[i].u4Size == 0)
        {
            sec_list[i].session_id = rArg->u4Session;

            sec_list[i].u4Addr = alloc_addr;
            sec_list[i].u4Used = alloc_addr;
            sec_list[i].u4Empty = alloc_addr;

            sec_list[i].u4Size = allocSize;
            sec_list[i].u4Flag = rArg->u4Flag;
            *rArg->u4Handle = sec_list[i].u4Addr;
            break;
        }
    }

    if(i == MAX_SECTION_NR)
        goto final;

    TZ_TVP_Protection(TZ_TVP_PROT_FLAG_SEC_BUF_ALC);
    sec_next = alloc_addr + allocSize;
    ret = true;


final:

    TZ_CACHE_END(rArg->u4Handle, sizeof(UINT32));
    return ret;
}

BOOL TZ_SEC_BUF_FREE(void *arg, UINT32 u4Size)
{
    UINT32 i;
    BOOL ret =false;
    for(i=0;i< MAX_SECTION_NR;++i)
    {
        if((sec_list[i].u4Addr == *(UINT32 *)arg)&&(sec_list[i].u4Size !=0))
        {
            sec_list[i].u4Size = 0;
            ret =  true;
        }
    }


    if(ret==true)
    {
        for(i=0;i< MAX_SECTION_NR;++i)
        {
            if(sec_list[i].u4Size !=0)
            {
                break;
            }
        }

        if(i==MAX_SECTION_NR)
        {
            TZ_TVP_UnProtection(TZ_TVP_PROT_FLAG_SEC_BUF_ALC);
            memset(sec_base , 0 , sec_size);
        }
    }
    return ret;
}

BOOL TZ_SEC_BUF_FRAGMENT_ALLOC(void *arg, UINT32 u4Size)
{
#if 0
    TZ_DRM_SEC_BUF_FRAGMENT_ALLOC_T *rArg = (TZ_DRM_SEC_BUF_FRAGMENT_ALLOC_T *)arg;
    BOOL ret = false;
    int i;

    TZ_CACHE_STR(rArg->u4Fragment, sizeof(UINT32));

    for(i =0; i< MAX_SECTION_NR;++i)
    {
        if((sec_list[i].u4Addr == rArg->u4Handle) && ( (SEC_BUF_FLAG_IS_CYCLIC_BUFFER && sec_list[i].u4Flag) !=0))
        {
            break;
        }

    }

    if(i == MAX_SECTION_NR)
        goto final;

    if(sec_list[i].u4Empty <= sec_list[i].u4Used)
    {
        if(sec_list[i].u4Addr + sec_list[i].u4Size > sec_list[i].u4Used + rArg->u4Size )
        {
            *rArg->u4Fragment = sec_list[i].u4Used;
            sec_list[i].u4Used += rArg->u4Size;
            ret = true;
        }
        else if(sec_list[i].u4Empty > sec_list[i].u4Addr + rArg->u4Size)
        {
            *rArg->u4Fragment = sec_list[i].u4Addr;
            sec_list[i].u4Used = sec_list[i].u4Addr + rArg->u4Size;
            ret = true;
        }
    }
    else
    {
        if (sec_list[i].u4Empty > sec_list[i].u4Used + rArg->u4Size )
        {
            *rArg->u4Fragment = sec_list[i].u4Used;
            sec_list[i].u4Used += rArg->u4Size;
            ret = true;
        }
    }

final:

    TZ_CACHE_END(rArg->u4Fragment, sizeof(UINT32));
    return ret;
#endif
    return true;
}


BOOL TZ_SEC_BUF_FRAGMENT_FREE(void *arg, UINT32 u4Size)
{
#if 0
    TZ_DRM_SEC_BUF_FRAGMENT_FREE_T *rArg = (TZ_DRM_SEC_BUF_FRAGMENT_FREE_T *)arg;
    BOOL ret = false;
    int i;

    for(i =0; i< MAX_SECTION_NR;++i)
    {
        if((sec_list[i].u4Addr == rArg->u4Handle) && ((SEC_BUF_FLAG_IS_CYCLIC_BUFFER && sec_list[i].u4Flag) !=0))
        {
            break;
        }
    }

    if(i == MAX_SECTION_NR)
        goto final;

    if(rArg->u4Fragment <= SEC_ELE_STR(i) 
            &&((rArg->u4Fragment + rArg->u4Size < SEC_ELE_END(i))))
    {
        sec_list[i].u4Empty = rArg->u4Fragment + rArg->u4Size;
        ret = true;
    }

final:

    return ret;
#endif
    return true;
}


BOOL TZ_SEC_BUF_FLUSH(void *arg, UINT32 u4Size)
{
    UINT32 i;
    sec_buf_init();
    for(i=0;i< MAX_SECTION_NR;++i)
    {
        sec_list[i].u4Size = 0;
    }
    sec_next = sec_base;
    return true;
}

BOOL TZ_SEC_BUF_VERIFY(void *arg, UINT32 u4Size)
{
    SEC_BUF *buf = (SEC_BUF *)arg;

    sec_buf_init();
    return sec_buf_verify(buf->u4Addr , buf->u4Size);
}
