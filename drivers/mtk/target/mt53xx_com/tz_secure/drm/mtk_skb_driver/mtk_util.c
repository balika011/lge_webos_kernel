

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "tomcrypt.h"
#include "mtk_util.h"
#include "x_os.h"
#include "x_assert.h"
#include "tzst.h"

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
struct MTK_DataBuffer {
    MTK_Boolean buffer_is_local;
    MTK_Byte*   buffer;
    MTK_Size    buffer_size;
    MTK_Size    data_size;
};

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define MTK_DATA_BUFFER_EXTRA_GROW_SPACE 256

/*----------------------------------------------------------------------
|   MTK_DataBuffer_Create
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_Create(MTK_Size size, MTK_DataBuffer** buffer)
{
	if(NULL == buffer)
	{
		return MTK_FAILURE;
	}


    /* allocate the object */
    *buffer = MTK_AllocateZeroMemory(sizeof(MTK_DataBuffer));
    if (*buffer == NULL) return MTK_ERROR_OUT_OF_MEMORY;

    /* construct the object */
    (*buffer)->buffer_is_local = MTK_TRUE;

    /* allocate the buffer */
    if (size) {
        (*buffer)->buffer_size = size;
        (*buffer)->buffer = MTK_AllocateMemory(size);
        if ((*buffer)->buffer == NULL) {
            MTK_FreeMemory((void*)(*buffer));
            return MTK_ERROR_OUT_OF_MEMORY;
        }
    }
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_Clone
+---------------------------------------------------------------------*/
MTK_Result
MTK_DataBuffer_Clone(const MTK_DataBuffer* self, MTK_DataBuffer** clone)
{

	if(NULL == self || NULL == clone)
	{
		return MTK_FAILURE;
	}


    /* create a clone with a buffer of the same size */
    MTK_CHECK(MTK_DataBuffer_Create(self->data_size, clone));

    /* copy the data */
    MTK_CHECK(MTK_DataBuffer_SetData(*clone, 
                                     self->buffer, 
                                     self->data_size));
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_Destroy
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_Destroy(MTK_DataBuffer* self)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}


    /* free the buffer */
    if (self->buffer_is_local) MTK_FreeMemory((void*)self->buffer);

    /* free the object */
    MTK_FreeMemory((void*)self);

    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_ReallocateBuffer
+---------------------------------------------------------------------*/
static MTK_Result
MTK_DataBuffer_ReallocateBuffer(MTK_DataBuffer* self, MTK_Size size)
{
    MTK_Byte* new_buffer;

	if(NULL == self)
	{
		return MTK_FAILURE;
	}



    /* check that the existing data fits */
    if (self->data_size > size) return MTK_ERROR_INVALID_PARAMETERS;

    /* allocate a new buffer */
    new_buffer = (MTK_Byte*)MTK_AllocateMemory(size);
    if (new_buffer == NULL) return MTK_ERROR_OUT_OF_MEMORY;

    /* copy the contents of the previous buffer, if any */
    if (self->buffer && self->data_size) {
        MTK_CopyMemory(new_buffer, self->buffer, self->data_size);
    }

    /* destroy the previous buffer */
    MTK_FreeMemory((void*)self->buffer);

    /* use the new buffer */
    self->buffer = new_buffer;
    self->buffer_size = size;

    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_SetBuffer
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_SetBuffer(MTK_DataBuffer* self,
                         MTK_Byte*       buffer, 
                         MTK_Size        buffer_size)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}



    if (self->buffer_is_local) {
        /* destroy the local buffer */
        MTK_FreeMemory((void*)self->buffer);
    }

    /* we're now using an external buffer */
    self->buffer_is_local = MTK_FALSE;
    self->buffer = buffer;
    self->buffer_size = buffer_size;
    self->data_size = 0;

    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_SetBufferSize
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_SetBufferSize(MTK_DataBuffer* self,
                             MTK_Size        buffer_size)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}

    if (self->buffer_is_local) {
        return MTK_DataBuffer_ReallocateBuffer(self, buffer_size);
    } else {
        /* cannot change an external buffer */
        return MTK_ERROR_NOT_SUPPORTED; 
    }
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_Reserve
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_Reserve(MTK_DataBuffer* self, MTK_Size size)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}


    if (self->buffer_size >= size) return MTK_SUCCESS;
    
    /* try doubling the size */
    {
        MTK_Size new_size = self->buffer_size*2;
        if (new_size < size) new_size = size + MTK_DATA_BUFFER_EXTRA_GROW_SPACE;
        return MTK_DataBuffer_SetBufferSize(self, new_size);
    }
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_GetBufferSize
+---------------------------------------------------------------------*/
MTK_Size   
MTK_DataBuffer_GetBufferSize(const MTK_DataBuffer* self)
{
	if(NULL == self)
	{
		return 0;
	}


    return self->buffer_size;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_GetData
+---------------------------------------------------------------------*/
const MTK_Byte*  
MTK_DataBuffer_GetData(const MTK_DataBuffer* self)
{

	if(NULL == self)
	{
		return NULL;
	}


    return self->buffer;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_UseData
+---------------------------------------------------------------------*/
MTK_Byte* 
MTK_DataBuffer_UseData(MTK_DataBuffer* self)
{

	if(NULL == self)
	{
		return NULL;
	}

    return self->buffer;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_GetDataSize
+---------------------------------------------------------------------*/
MTK_Size   
MTK_DataBuffer_GetDataSize(const MTK_DataBuffer* self)
{
	if(NULL == self)
	{
		return 0;
	}


    return self->data_size;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_SetDataSize
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_SetDataSize(MTK_DataBuffer* self, MTK_Size size)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}


    if (size > self->buffer_size) {
        /* the buffer is too small, we need to reallocate it */
        if (self->buffer_is_local) {
            MTK_CHECK(MTK_DataBuffer_ReallocateBuffer(self, size));
        } else { 
            /* we cannot reallocate an external buffer */
            return MTK_ERROR_NOT_SUPPORTED;
        }
    }
    self->data_size = size;

    return MTK_SUCCESS;
}

909-2934-29-0923-5-0;;jsajdlj3kl52j#
	#endif
/*----------------------------------------------------------------------
|   MTK_DataBuffer_SetData
+---------------------------------------------------------------------*/
MTK_Result 
MTK_DataBuffer_SetData(MTK_DataBuffer* self, 
                       const MTK_Byte* data,
                       MTK_Size        data_size)
{

	if(NULL == self)
	{
		return MTK_FAILURE;
	}



    if (data_size > self->buffer_size) {
        if (self->buffer_is_local) {
            MTK_CHECK(MTK_DataBuffer_ReallocateBuffer(self, data_size));
        } else {
            return MTK_ERROR_OUT_OF_RESOURCES;
        }
    }
    MTK_CopyMemory(self->buffer, data, data_size);
    self->data_size = data_size;

    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_Equals
+---------------------------------------------------------------------*/
MTK_Boolean
MTK_DataBuffer_Equals(const MTK_DataBuffer* self, 
                      const MTK_DataBuffer* other)
{
    /* true if both are NULL */
    if (self == NULL && other == NULL) return MTK_TRUE;

    /* not true if one of them is NULL */
    if (self == NULL || other == NULL) return MTK_FALSE;

    /* check that the sizes are the same */
    if (self->data_size != other->data_size) return MTK_FALSE;

    /* now compare the data */
    return MTK_MemoryEqual(self->buffer, 
                           other->buffer, 
                           self->data_size);
}

/*----------------------------------------------------------------------
|   MTK_DataBuffer_AppendData
+---------------------------------------------------------------------*/
MTK_Result
MTK_DataBuffer_AppendData(MTK_DataBuffer*   self,
                          const MTK_Byte*   data,
                          MTK_Size          data_size)
{
    MTK_Size   new_data_size;

	if(NULL == self || NULL == data)
	{
		return MTK_FAILURE;
	}

	new_data_size = self->data_size + data_size;

	
    /* reserve the space and copy the appended data */
    MTK_CHECK(MTK_DataBuffer_Reserve(self, new_data_size));
    MTK_CopyMemory(self->buffer + self->data_size, data, data_size);
    self->data_size = new_data_size;
    
    return MTK_SUCCESS;
}


void* MTK_AllocateMemory(unsigned int size)
{

    return x_mem_alloc (size);
}

void* MTK_AllocateZeroMemory(unsigned int size)
{
    return x_mem_calloc(1, size);
}

void MTK_FreeMemory(void* pointer)
{
	if(NULL != pointer)
	{
    	x_mem_free(pointer);
	}
    
}

int MTK_MemoryEqual(const void* s1, const void* s2, unsigned long n)
{
	if(NULL == s1 || NULL == s2)
	{
		return MTK_FALSE;
	}

    return (x_memcmp((s1), (s2), (n)) == 0) ;
}

void MTK_CopyMemory(void* dest, const void* src, MTK_Size size)
{
	if(NULL != dest  && NULL != src)
	{
    	//x_memcpy(dest, src, size);
    	memcpy_s2s_chk(dest, src, size, NULL);
	}

}


/*
	make sure buffer big engouh
*/
void MTK_BytesFromInt32Be(unsigned char* buffer, MTK_UInt32 value)
{
	if(NULL == buffer)
	{
		return;
	}

    buffer[0] = (unsigned char)(value>>24) & 0xFF;
    buffer[1] = (unsigned char)(value>>16) & 0xFF;
    buffer[2] = (unsigned char)(value>> 8) & 0xFF;
    buffer[3] = (unsigned char)(value    ) & 0xFF;
}

MTK_UInt32 MTKBytesToInt32Be(const unsigned char* buffer)
{

	if(NULL == buffer)
	{
		return 0;
	}


    return 
        ( ((MTK_UInt32)buffer[0])<<24 ) |
        ( ((MTK_UInt32)buffer[1])<<16 ) |
        ( ((MTK_UInt32)buffer[2])<<8  ) |
        ( ((MTK_UInt32)buffer[3])     );
}

MTK_UInt32 MTK_BytesToInt32Be(const unsigned char* buffer)
{


	if(NULL == buffer)
	{
		return 0;
	}


    return 
        ( ((MTK_UInt32)buffer[0])<<24 ) |
        ( ((MTK_UInt32)buffer[1])<<16 ) |
        ( ((MTK_UInt32)buffer[2])<<8  ) |
        ( ((MTK_UInt32)buffer[3])     );
}

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
struct MTK_Mutex {
    void* mutex;
};

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/


/*----------------------------------------------------------------------
|   MTK_GetCurrentThreadId
+---------------------------------------------------------------------*/
MTK_ThreadId MTK_GetCurrentThreadId(void)
{
    return 0;
}

static HANDLE_T _hApiSem = NULL_HANDLE;

/*----------------------------------------------------------------------
|   MTK_Mutex_Create
+---------------------------------------------------------------------*/
MTK_Result MTK_Mutex_Create(MTK_Mutex** mutex)
{
    static BOOL _fgInit = FALSE;

    if (!_fgInit)
    {
        VERIFY(x_sema_create(&_hApiSem, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _fgInit = TRUE;
    }
    
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_Mutex_LockAutoCreate
+---------------------------------------------------------------------*/
MTK_Result MTK_Mutex_LockAutoCreate(MTK_Mutex** mutex)
{
    static BOOL _fgInit = FALSE;

    if (!_fgInit)
    {
        VERIFY(x_sema_create(&_hApiSem, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _fgInit = TRUE;
    }

    VERIFY(x_sema_lock(_hApiSem, X_SEMA_OPTION_WAIT) == OSR_OK);
        
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_Mutex_Lock
+---------------------------------------------------------------------*/
MTK_Result MTK_Mutex_Lock(MTK_Mutex* mutex)
{
    VERIFY(x_sema_lock(_hApiSem, X_SEMA_OPTION_WAIT) == OSR_OK);
    
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_Mutex_Unlock
+---------------------------------------------------------------------*/
MTK_Result MTK_Mutex_Unlock(MTK_Mutex* mutex)
{
    VERIFY(x_sema_unlock(_hApiSem) == OSR_OK);
    
    return MTK_SUCCESS;
}

/*----------------------------------------------------------------------
|   MTK_Mutex_Destroy
+---------------------------------------------------------------------*/
MTK_Result
MTK_Mutex_Destroy(MTK_Mutex* mutex)
{
    return MTK_SUCCESS;
}

#ifdef swap
#undef swap
#endif

typedef
struct
{
        int     (*cmp)(const void*, const void*);
        void    (*swap)(char*, char*, long);
        long    es;
} Sort;

static  void
swapb(char *i, char *j, long es)
{
        char c;

		if(NULL == i || NULL == j)
		{
			return;
		}

        do {
                c = *i;
                *i++ = *j;
                *j++ = c;
                es--;
        } while(es != 0);

}

static  void
swapi(char *ii, char *ij, long es)
{
        long *i, *j, c;

		if(NULL == ii || NULL == ij)
		{
			return;
		}


        i = (long*)ii;
        j = (long*)ij;
        do {
                c = *i;
                *i++ = *j;
                *j++ = c;
                es -= sizeof(long);
        } while(es != 0);
}

static  char*
pivot(char *a, long n, Sort *p)
{
        long j;
        char *pi, *pj, *pk;


		if(NULL == a || NULL == n || NULL == p)
		{
			return;
		}



        j = n/6 * p->es;
        pi = a + j;     /* 1/6 */
        j += j;
        pj = pi + j;    /* 1/2 */
        pk = pj + j;    /* 5/6 */
        if(p->cmp(pi, pj) < 0) {
                if(p->cmp(pi, pk) < 0) {
                        if(p->cmp(pj, pk) < 0)
                                return pj;
                        return pk;
                }
                return pi;
        }
        if(p->cmp(pj, pk) < 0) {
                if(p->cmp(pi, pk) < 0)
                        return pi;
                return pk;
        }
        return pj;
}

static  void
qsorts(char *a, long n, Sort *p)
{
        long j, es;
        char *pi, *pj, *pn;


		if(NULL == a || NULL == n || NULL == p)
		{
			return;
		}


        es = p->es;
        while(n > 1) {
                if(n > 10) {
                        pi = pivot(a, n, p);
                } else
                        pi = a + (n>>1)*es;

                p->swap(a, pi, es);
                pi = a;
                pn = a + n*es;
                pj = pn;
                for(;;) {
                        do
                                pi += es;
                        while(pi < pn && p->cmp(pi, a) < 0);
                        do
                                pj -= es;
                        while(pj > a && p->cmp(pj, a) > 0);
                        if(pj < pi)
                                break;
                        p->swap(pi, pj, es);
                }
                p->swap(a, pj, es);
                j = (pj - a) / es;

                n = n-j-1;
                if(j >= n) {
                        qsorts(a, j, p);
                        a += (j+1)*es;
                } else {
                        qsorts(a + (j+1)*es, n, p);
                        n = j;
                }
        }
}

void
MTK_QSORT(void *va, long n, long es, int (*cmp)(const void*, const void*))
{
        Sort s;


		if(NULL == va || NULL == n || NULL == cmp)
		{
			return;
		}



        s.cmp = cmp;
        s.es = es;
        s.swap = swapi;
        if(((long)va | es) % sizeof(long))
                s.swap = swapb;
        qsorts((char*)va, n, &s);
}

void *tom_malloc(size_t size)
{
    return x_mem_alloc (size);
}

void *tom_realloc(void *ptr, size_t size)
{
    void *pNew;

	
    pNew = x_mem_alloc(size);

	if(NULL == pNew || NULL == ptr)
	{
		return;
	}
	
    x_memcpy(pNew, ptr, size);
	
    x_mem_free(ptr);

    return pNew;
}

void *tom_calloc(size_t nelem, size_t size)
{
    return x_mem_calloc (nelem, size);
}


void tom_free(void *ptr)
{
	if(NULL != tom_free)
  	{
  		x_mem_free(ptr);
	}
    
}

void *tom_memset(void *s, int c, size_t n)
{
	if(NULL == s)
	{
		return NULL;
	}

    return x_memset (s, c, n);
}
    

void *tom_memcpy(void *s1, const void *s2, size_t n)
{
	if(NULL == s1 || NULL == s2)
	{
		return NULL;
	}


    return x_memcpy(s1, s2, n);
}

int tom_memcmp(const void *s1, const void *s2, size_t n)
{

	if(NULL == s1 || NULL == s2)
	{
		return MTK_FAILURE;
	}

    return x_memcmp(s1, s2, n);
}

int tom_strcmp(const char *s1, const char *s2)
{
	if(NULL == s1 || NULL == s2)
	{
		return MTK_FAILURE;
	}


    return x_strcmp(s1, s2);
}
