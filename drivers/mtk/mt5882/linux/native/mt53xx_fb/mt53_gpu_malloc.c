//==========================================================================
//
//      MT53_GPU_dlmalloc.c
//
//      Port of Doug Lea's malloc implementation
//
//==========================================================================

//----------------------------------------------------------------------------

/*
  A version of malloc/free/realloc written by Doug Lea and released to the
  public domain.  Send questions/comments/complaints/performance data
  to dl at cs.oswego.edu

* VERSION 2.6.6  Sun Mar  5 19:10:03 2000  Doug Lea  (dl at gee)

   Note: There may be an updated version of this malloc obtainable at
           ftp://g.oswego.edu/pub/misc/malloc.c
         Check before installing!

* Why use this malloc?

  This is not the fastest, most space-conserving, most portable, or
  most tunable malloc ever written. However it is among the fastest
  while also being among the most space-conserving, portable and tunable.
  Consistent balance across these factors results in a good general-purpose
  allocator. For a high-level description, see
     http://g.oswego.edu/dl/html/malloc.html

* Synopsis of public routines

  (Much fuller descriptions are contained in the program documentation below.)

[ these have of course been renamed in the eCos port ]a

  malloc(size_t n);
     Return a pointer to a newly allocated chunk of at least n bytes, or null
     if no space is available.
  free(Void_t* p);
     Release the chunk of memory pointed to by p, or no effect if p is null.
  realloc(Void_t* p, size_t n);
     Return a pointer to a chunk of size n that contains the same data
     as does chunk p up to the minimum of (n, p's size) bytes, or null
     if no space is available. The returned pointer may or may not be
     the same as p. If p is null, equivalent to malloc. realloc of
     zero bytes calls free(p)

* Vital statistics:

  Alignment:                            8-byte
       8 byte alignment is currently hardwired into the design.  This
       seems to suffice for all current machines and C compilers.

  Assumed pointer representation:       4 or 8 bytes
       Code for 8-byte pointers is untested by me but has worked
       reliably by Wolfram Gloger, who contributed most of the
       changes supporting this.

  Assumed size_t  representation:       4 or 8 bytes
       Note that size_t is allowed to be 4 bytes even if pointers are 8.

  Minimum overhead per allocated chunk: 4 or 8 bytes
       Each malloced chunk has a hidden overhead of 4 bytes holding size
       and status information.

  Minimum allocated size: 4-byte ptrs:  16 bytes    (including 4 overhead)
                          8-byte ptrs:  24/32 bytes (including, 4/8 overhead)

       When a chunk is freed, 12 (for 4byte ptrs) or 20 (for 8 byte
       ptrs but 4 byte size) or 24 (for 8/8) additional bytes are
       needed; 4 (8) for a trailing size field
       and 8 (16) bytes for free list pointers. Thus, the minimum
       allocatable size is 16/24/32 bytes.

       Even a request for zero bytes (i.e., malloc(0)) returns a
       pointer to something of the minimum allocatable size.

  Maximum allocated size: 4-byte size_t: 2^31 -  8 bytes
                          8-byte size_t: 2^63 - 16 bytes

       It is assumed that (possibly signed) size_t bit values suffice to
       represent chunk sizes. `Possibly signed' is due to the fact
       that `size_t' may be defined on a system as either a signed or
       an unsigned type. To be conservative, values that would appear
       as negative numbers are avoided.
       Requests for sizes with a negative sign bit when the request
       size is treaded as a long will return null.

  Maximum overhead wastage per allocated chunk: normally 15 bytes

       Alignnment demands, plus the minimum allocatable size restriction
       make the normal worst-case wastage 15 bytes (i.e., up to 15
       more bytes will be allocated than were requested in malloc), with
       one exception: because requests for zero bytes allocate non-zero space,
       the worst case wastage for a request of zero bytes is 24 bytes.

* Limitations

    Here are some features that are NOT currently supported

    * No user-definable hooks for callbacks and the like.
    * No automated mechanism for fully checking that all accesses
      to malloced memory stay within their bounds.
    * No support for compaction.

* Synopsis of compile-time options:

    People have reported using previous versions of this malloc on all
    versions of Unix, sometimes by tweaking some of the defines
    below. It has been tested most extensively on Solaris and
    Linux. It is also reported to work on WIN32 platforms.
    People have also reported adapting this malloc for use in
    stand-alone embedded systems.

    The implementation is in straight, hand-tuned ANSI C.  Among other
    consequences, it uses a lot of macros.  Because of this, to be at
    all usable, this code should be compiled using an optimizing compiler
    (for example gcc -O2) that can simplify expressions and control
    paths.

  MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG      (default: NOT defined)
     Define to enable debugging. Adds fairly extensive assertion-based
     checking to help track down memory errors, but noticeably slows down
     execution.
  MALLOC_LOCK		   (default: NOT defined)
  MALLOC_UNLOCK		   (default: NOT defined)
     Define these to C expressions which are run to lock and unlock
     the malloc data structures.  Calls may be nested; that is,
     MALLOC_LOCK may be called more than once before the corresponding
     MALLOC_UNLOCK calls.  MALLOC_LOCK must avoid waiting for a lock
     that it already holds.
  MT53_MALLOC_ALIGNMENT          (default: NOT defined)
     Define this to 16 if you need 16 byte alignment instead of 8 byte alignment
     which is the normal default.
  SIZE_T_SMALLER_THAN_LONG (default: NOT defined)
     Define this when the platform you are compiling has
     sizeof(long) > sizeof(size_t).
     The option causes some extra code to be generated to handle operations
     that use size_t operands and have long results.
  MT53_INTERNAL_SIZE_T           (default: size_t)
     Define to a 32-bit type (probably `unsigned int') if you are on a
     64-bit machine, yet do not want or need to allow malloc requests of
     greater than 2^31 to be handled. This saves space, especially for
     very small chunks.

*/

//----------------------------------------------------------------------------


/* Preliminaries */

#include <stddef.h>                    // for size_t
//#include "gpu_malloc.h"
#include "mt53_dlmalloc.h"
//#include<stdlib.h>

//extern void* memcpy(void *, const void *, size_t);
//extern void* memset(void *, int, size_t);

#include "x_os.h"


// type definitions
//
#ifndef uint32

#define uint8							unsigned char
#define uint16							unsigned short
#define uint32							unsigned int
#define int8							char
#define int16							short
#define int32							int

#endif	// uint32

/*
    Debugging:

    Because freed chunks may be overwritten with link fields, this
    malloc will often die when freed memory is overwritten by user
    programs.  This can be very effective (albeit in an annoying way)
    in helping track down dangling pointers.

    If you compile with MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG enabled, a
    number of assertion checks are
    enabled that will catch more memory errors. You probably won't be
    able to make much sense of the actual assertion errors, but they
    should help you locate incorrectly overwritten memory.  The
    checking is fairly extensive, and will slow down execution
    noticeably. Calling get_status() with DEBUG set will
    attempt to check every allocated and free chunk in the
    course of computing the summmaries.

    Setting MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG may also be helpful if you
    are trying to modify this code. The assertions in the check routines
    spell out in more detail the assumptions and invariants underlying
    the algorithms.

*/

#ifndef ASSERT
	#ifdef _DEBUG_
		#include <assert.h>
		#define ASSERT(x)				assert((x))
	#else
		#define ASSERT(x)
	#endif	// _DEBUG_
#endif	// ASSERT

/*
   Define MALLOC_LOCK and MALLOC_UNLOCK to C expressions to run to
   lock and unlock the malloc data structures.  MALLOC_LOCK may be
   called recursively.
 */

//#include <pthread.h>

static HANDLE_T _hGpuMallocSema;



/*
inline void _MLOCK(void)
{
	pthread_mutex_lock(&_lock);
}

inline void _MUNLOCK(void)
{
	pthread_mutex_unlock(&_lock);
}

#define MALLOC_LOCK						_MLOCK()
#define MALLOC_UNLOCK					_MUNLOCK()
*/


#define MALLOC_LOCK						MT53_GPU_LockMalloc();
#define MALLOC_UNLOCK					MT53_GPU_UnLockMalloc();

#ifndef MALLOC_LOCK
#define MALLOC_LOCK
#endif

#ifndef MALLOC_UNLOCK
#define MALLOC_UNLOCK
#endif

/*
  MT53_INTERNAL_SIZE_T is the word-size used for internal bookkeeping
  of chunk sizes. On a 64-bit machine, you can reduce malloc
  overhead by defining MT53_INTERNAL_SIZE_T to be a 32 bit `unsigned int'
  at the expense of not being able to handle requests greater than
  2^31. This limitation is hardly ever a concern; you are encouraged
  to set this. However, the default version is the same as size_t.
*/

#ifndef MT53_INTERNAL_SIZE_T
#define MT53_INTERNAL_SIZE_T size_t
#endif

/*
  Following is needed on implementations whereby long > size_t.
  The problem is caused because the code performs subtractions of
  size_t values and stores the result in long values.  In the case
  where long > size_t and the first value is actually less than
  the second value, the resultant value is positive.  For example,
  (long)(x - y) where x = 0 and y is 1 ends up being 0x00000000FFFFFFFF
  which is 2*31 - 1 instead of 0xFFFFFFFFFFFFFFFF.  This is due to the
  fact that assignment from unsigned to signed won't sign extend.
*/

#ifdef SIZE_T_SMALLER_THAN_LONG
#define mt53_long_sub_size_t(x, y) ( (x < y) ? -((long)(y - x)) : (x - y) );
#else
#define mt53_long_sub_size_t(x, y) ( (long)(x - y) )
#endif

/*
#define ICEOSD_MEMALLOC_ALLOCATOR
*/

#ifdef ICEOSD_MEMALLOC_ALLOCATOR

#include <string.h>                    // memcpy, memset

/* The following macros are only invoked with (2n+1)-multiples of
   MT53_INTERNAL_SIZE_T units, with a positive integer n. This is exploited
   for fast inline execution when n is small. */

#define MT53_MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  MT53_INTERNAL_SIZE_T mzsz = (nbytes);                                        \
  if(mzsz <= 9*sizeof(mzsz)) {                                                \
    MT53_INTERNAL_SIZE_T* mz = (MT53_INTERNAL_SIZE_T*) (charp);                 \
    if(mzsz >= 5*sizeof(mzsz)) {     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
      if(mzsz >= 7*sizeof(mzsz)) {   *mz++ = 0;                               \
                                     *mz++ = 0;                               \
        if(mzsz >= 9*sizeof(mzsz)) { *mz++ = 0;                               \
                                     *mz++ = 0; }}}                           \
                                     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
                                     *mz   = 0;                               \
  } else x_memset((charp), 0, mzsz);                                            \
} while(0)

#define MT53_MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  MT53_INTERNAL_SIZE_T mcsz = (nbytes);                                        \
  if(mcsz <= 9*sizeof(mcsz)) {                                                \
    MT53_INTERNAL_SIZE_T* mcsrc = (MT53_INTERNAL_SIZE_T*) (src);                \
    MT53_INTERNAL_SIZE_T* mcdst = (MT53_INTERNAL_SIZE_T*) (dest);               \
    if(mcsz >= 5*sizeof(mcsz)) {     *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
      if(mcsz >= 7*sizeof(mcsz)) {   *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
        if(mcsz >= 9*sizeof(mcsz)) { *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++; }}}                 \
                                     *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
                                     *mcdst   = *mcsrc  ;                     \
  } else x_memcpy(dest, src, mcsz);                                             \
} while(0)

#else /* !ICEOSD_MEMALLOC_ALLOCATOR */

/* Use Duff's device for good zeroing/copying performance. */

#define MT53_MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  MT53_INTERNAL_SIZE_T* mzp = (MT53_INTERNAL_SIZE_T*)(charp);                   \
  long mctmp = (nbytes)/sizeof(MT53_INTERNAL_SIZE_T), mcn;                     \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mzp++ = 0;                                             \
    case 7:           *mzp++ = 0;                                             \
    case 6:           *mzp++ = 0;                                             \
    case 5:           *mzp++ = 0;                                             \
    case 4:           *mzp++ = 0;                                             \
    case 3:           *mzp++ = 0;                                             \
    case 2:           *mzp++ = 0;                                             \
    case 1:           *mzp++ = 0; if(mcn <= 0) break; mcn--; }                \
  }                                                                           \
} while(0)

#define MT53_MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  MT53_INTERNAL_SIZE_T* mcsrc = (MT53_INTERNAL_SIZE_T*) ((unsigned long)src);                    \
  MT53_INTERNAL_SIZE_T* mcdst = (MT53_INTERNAL_SIZE_T*) ((unsigned long)dest);                   \
  long mctmp = (nbytes)/sizeof(MT53_INTERNAL_SIZE_T), mcn;                     \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mcdst++ = *mcsrc++;                                    \
    case 7:           *mcdst++ = *mcsrc++;                                    \
    case 6:           *mcdst++ = *mcsrc++;                                    \
    case 5:           *mcdst++ = *mcsrc++;                                    \
    case 4:           *mcdst++ = *mcsrc++;                                    \
    case 3:           *mcdst++ = *mcsrc++;                                    \
    case 2:           *mcdst++ = *mcsrc++;                                    \
    case 1:           *mcdst++ = *mcsrc++; if(mcn <= 0) break; mcn--; }       \
  }                                                                           \
} while(0)

#endif	// ICEOSD_MEMALLOC_ALLOCATOR


//----------------------------------------------------------------------------

/*
   mt53_malloc_chunk details:

    (The following includes lightly edited explanations by Colin Plumb.)

    Chunks of memory are maintained using a `boundary tag' method as
    described in e.g., Knuth or Standish.  (See the paper by Paul
    Wilson ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps for a
    survey of such techniques.)  Sizes of free chunks are stored both
    in the front of each chunk and at the end.  This makes
    consolidating fragmented chunks into bigger chunks very fast.  The
    size fields also hold bits representing whether chunks are free or
    in use.

    An allocated chunk looks like this:


    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk, if allocated            | |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             User data starts here...                          .
            .                                                               .
            .             (malloc_usable_space() bytes)                     .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of chunk                                     |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


    Where "chunk" is the front of the chunk for the purpose of most of
    the malloc code, but "mem" is the pointer that is returned to the
    user.  "Nextchunk" is the beginning of the next contiguous chunk.

    Chunks always begin on even word boundries, so the mem portion
    (which is returned to the user) is also on an even word boundary, and
    thus double-word aligned.

    Free chunks are stored in circular doubly-linked lists, and look like this:

    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk                            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Forward pointer to next chunk in list             |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Back pointer to previous chunk in list            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Unused space (may be 0 bytes long)                .
            .                                                               .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    The P (MT53_PREV_INUSE) bit, stored in the unused low-order bit of the
    chunk size (which is always a multiple of two words), is an in-use
    bit for the *previous* chunk.  If that bit is *clear*, then the
    word before the current chunk size contains the previous chunk
    size, and can be used to find the front of the previous chunk.
    (The very first chunk allocated always has this bit set,
    preventing access to non-existent (or non-owned) memory.)

    Note that the `foot' of the current chunk is actually represented
    as the prev_size of the NEXT chunk. (This makes it easier to
    deal with alignments etc).

    The exception to all this is the special chunk `top', which doesn't
    bother using the trailing size field since there is no next
    contiguous chunk that would have to index off it. (After
    initialization, `top' is forced to always exist. )

    Available chunks are kept in any of several places (all declared below):

    * `av': An array of chunks serving as bin headers for consolidated
       chunks. Each bin is doubly linked.  The bins are approximately
       proportionally (log) spaced.  There are a lot of these bins
       (128). This may look excessive, but works very well in
       practice.  All procedures maintain the invariant that no
       consolidated chunk physically borders another one. Chunks in
       bins are kept in size order, with ties going to the
       approximately least recently used chunk.

       The chunks in each bin are maintained in decreasing sorted order by
       size.  This is irrelevant for the small bins, which all contain
       the same-sized chunks, but facilitates best-fit allocation for
       larger chunks. (These lists are just sequential. Keeping them in
       order almost never requires enough traversal to warrant using
       fancier ordered data structures.)  Chunks of the same size are
       linked with the most recently freed at the front, and allocations
       are taken from the back.  This results in LRU or FIFO allocation
       order, which tends to give each chunk an equal opportunity to be
       consolidated with adjacent freed chunks, resulting in larger free
       chunks and less fragmentation.

    * `top': The top-most available chunk (i.e., the one bordering the
       end of available memory) is treated specially. It is never
       included in any bin, is used only if no other chunk is
       available.

    * `last_remainder': A bin holding only the remainder of the
       most recently split (non-top) chunk. This bin is checked
       before other non-fitting chunks, so as to provide better
       locality for runs of sequentially allocated chunks.

*/

/*
  This struct declaration is misleading (but accurate and necessary).
  It declares a "view" into memory allowing access to necessary
  fields at known offsets from a given base. See explanation below.
*/

struct mt53_malloc_chunk {

  MT53_INTERNAL_SIZE_T      prev_size;  /* Size of previous chunk (if free).  */
  MT53_INTERNAL_SIZE_T      size;       /* Size in bytes, including overhead. */

  struct mt53_malloc_chunk* fd;         /* double links -- used only if free. */
  struct mt53_malloc_chunk* bk;
};

typedef struct mt53_malloc_chunk* mt53_mchunkptr;

/*  sizes, alignments */

#define MT53_SIZE_SZ                (sizeof(MT53_INTERNAL_SIZE_T))
#ifndef MT53_MALLOC_ALIGNMENT
#define MT53_MALLOC_ALIGN           8
//#define MT53_MALLOC_ALIGNMENT       (MT53_SIZE_SZ + MT53_SIZE_SZ)
#define MT53_MALLOC_ALIGNMENT       (MT53_MALLOC_ALIGN + MT53_MALLOC_ALIGN)
#else
#define MT53_MALLOC_ALIGN           MT53_MALLOC_ALIGNMENT
#endif
#define MT53_MALLOC_ALIGN_MASK      (MT53_MALLOC_ALIGNMENT - 1)
#define MT53_MINSIZE                (sizeof(struct mt53_malloc_chunk))

/* conversion from malloc headers to user pointers, and back */

#define mt53_chunk2mem(p)   ((uint8*)((char*)(p) + 2*MT53_SIZE_SZ))
//#define mt53_mem2chunk(mem) ((mt53_mchunkptr)((char*)(mem) - 2*MT53_SIZE_SZ))
#define mt53_mem2chunk(mem) ((mt53_mchunkptr)(((unsigned long)(mem)) - 2*MT53_SIZE_SZ))

/* pad request bytes into a usable size */

#define mt53_request2size(req) \
 (((long)((req) + (MT53_SIZE_SZ + MT53_MALLOC_ALIGN_MASK)) < \
  (long)(MT53_MINSIZE + MT53_MALLOC_ALIGN_MASK)) ? ((MT53_MINSIZE + MT53_MALLOC_ALIGN_MASK) & ~(MT53_MALLOC_ALIGN_MASK)) : \
   (((req) + (MT53_SIZE_SZ + MT53_MALLOC_ALIGN_MASK)) & ~(MT53_MALLOC_ALIGN_MASK)))

/* Check if m has acceptable alignment */

#define mt53_aligned_OK(m)    (((unsigned long)((m)) & (MT53_MALLOC_ALIGN_MASK)) == 0)


/*
  Physical chunk operations
*/


/* size field is or'ed with MT53_PREV_INUSE when previous adjacent chunk in use */

#define MT53_PREV_INUSE 0x1

/* Bits to mask off when extracting size */

#define MT53_SIZE_BITS (MT53_PREV_INUSE)


/* Ptr to next physical mt53_malloc_chunk. */

//#define mt53_next_chunk(p) ((mt53_mchunkptr)( ((char*)(p)) + ((p)->size & ~MT53_PREV_INUSE) ))
#define mt53_next_chunk(p) ((mt53_mchunkptr)( ((unsigned long)(p)) + ((p)->size & ~MT53_PREV_INUSE) ))

/* Ptr to previous physical mt53_malloc_chunk */

//#define mt53_prev_chunk(p)
//   ((mt53_mchunkptr)( ((char*)(p)) - ((p)->prev_size) ))
#define mt53_prev_chunk(p)\
   ((mt53_mchunkptr)( ((unsigned long)(p)) - ((p)->prev_size) ))

/* Treat space at ptr + offset as a chunk */

//#define mt53_chunk_at_offset(p, s)  ((mt53_mchunkptr)(((char*)(p)) + (s)))
#define mt53_chunk_at_offset(p, s)  ((mt53_mchunkptr)(((unsigned long)(p)) + (s)))

/*
  Dealing with use bits
*/

/* extract p's mt53_inuse bit */

//#define mt53_inuse(p)
//((((mt53_mchunkptr)(((char*)(p))+((p)->size & ~MT53_PREV_INUSE)))->size) & MT53_PREV_INUSE)
#define mt53_inuse(p)\
((((mt53_mchunkptr)(((unsigned long)(p))+((p)->size & ~MT53_PREV_INUSE)))->size) & MT53_PREV_INUSE)

/* extract mt53_inuse bit of previous chunk */

#define mt53_prev_inuse(p)  ((p)->size & MT53_PREV_INUSE)

/* set/clear chunk as in use without otherwise disturbing */

//#define mt53_set_inuse(p)
//((mt53_mchunkptr)(((char*)(p)) + ((p)->size & ~MT53_PREV_INUSE)))->size |= MT53_PREV_INUSE
#define mt53_set_inuse(p)\
((mt53_mchunkptr)(((unsigned long)(p)) + ((p)->size & ~MT53_PREV_INUSE)))->size |= MT53_PREV_INUSE

//#define mt53_clear_inuse(p)
//((mt53_mchunkptr)(((char*)(p)) + ((p)->size & ~MT53_PREV_INUSE)))->size &= ~(MT53_PREV_INUSE)
#define mt53_clear_inuse(p)\
((mt53_mchunkptr)(((unsigned long)(p)) + ((p)->size & ~MT53_PREV_INUSE)))->size &= ~(MT53_PREV_INUSE)

/* check/set/clear inuse bits in known places */

//#define mt53_inuse_bit_at_offset(p, s)
// (((mt53_mchunkptr)(((char*)(p)) + (s)))->size & MT53_PREV_INUSE)
#define mt53_inuse_bit_at_offset(p, s)\
 (((mt53_mchunkptr)(((unsigned long)(p)) + (s)))->size & MT53_PREV_INUSE)

//#define mt53_set_inuse_bit_at_offset(p, s)
// (((mt53_mchunkptr)(((char*)(p)) + (s)))->size |= MT53_PREV_INUSE)
#define mt53_set_inuse_bit_at_offset(p, s)\
 (((mt53_mchunkptr)(((unsigned long)(p)) + (s)))->size |= MT53_PREV_INUSE)

//#define mt53_clear_inuse_bit_at_offset(p, s)
// (((mt53_mchunkptr)(((char*)(p)) + (s)))->size &= ~(MT53_PREV_INUSE))
#define mt53_clear_inuse_bit_at_offset(p, s)\
 (((mt53_mchunkptr)(((unsigned long)(p)) + (s)))->size &= ~(MT53_PREV_INUSE))


/*
  Dealing with size fields
*/

/* Get size, ignoring use bits */

#define mt53_chunksize(p)          ((p)->size & ~(MT53_SIZE_BITS))

/* Set size at head, without disturbing its use bit */

#define mt53_set_head_size(p, s)   ((p)->size = (((p)->size & MT53_PREV_INUSE) | (s)))

/* Set size/use ignoring previous bits in header */

#define mt53_set_head(p, s)        ((p)->size = (s))

/* Set size at footer (only when chunk is not in use) */

//#define mt53_set_foot(p, s)   (((mt53_mchunkptr)((char*)(p) + (s)))->prev_size = (s))
#define mt53_set_foot(p, s)   (((mt53_mchunkptr)((unsigned long)(p) + (s)))->prev_size = (s))


//----------------------------------------------------------------------------

/*
   Bins

    The bins, `av_' are an array of pairs of pointers serving as the
    heads of (initially empty) doubly-linked lists of chunks, laid out
    in a way so that each pair can be treated as if it were in a
    mt53_malloc_chunk. (This way, the fd/bk offsets for linking bin heads
    and chunks are the same).

    Bins for sizes < 512 bytes contain chunks of all the same size, spaced
    8 bytes apart. Larger bins are approximately logarithmically
    spaced. (See the table below.) The `av_' array is never mentioned
    directly in the code, but instead via bin access macros.

    Bin layout:

    64 bins of size       8
    32 bins of size      64
    16 bins of size     512
     8 bins of size    4096
     4 bins of size   32768
     2 bins of size  262144
     1 bin  of size what's left

    There is actually a little bit of slop in the numbers in bin_index
    for the sake of speed. This makes no difference elsewhere.

    The special chunks `top' and `last_remainder' get their own bins,
    (this is implemented via yet more trickery with the av_ array),
    although `top' is never properly linked to its bin since it is
    always handled specially.

*/

typedef struct mt53_malloc_chunk* mt53_mbinptr;

/* access macros */

//#define mt53_bin_at(i)      ((mt53_mbinptr)((char*)&(av_[2*(i) + 2]) - 2*MT53_SIZE_SZ))
//#define mt53_next_bin(b)    ((mt53_mbinptr)((char*)(b) + 2 * sizeof(mt53_mbinptr)))
//#define mt53_prev_bin(b)    ((mt53_mbinptr)((char*)(b) - 2 * sizeof(mt53_mbinptr)))
#define mt53_bin_at(i)      ((mt53_mbinptr)((unsigned long)&(av_[2*(i) + 2]) - 2*MT53_SIZE_SZ))
#define mt53_next_bin(b)    ((mt53_mbinptr)((unsigned long)(b) + 2 * sizeof(mt53_mbinptr)))
#define mt53_prev_bin(b)    ((mt53_mbinptr)((unsigned long)(b) - 2 * sizeof(mt53_mbinptr)))

/*
   The first 2 bins are never indexed. The corresponding av_ cells are instead
   used for bookkeeping. This is not to save space, but to simplify
   indexing, maintain locality, and avoid some initialization tests.
*/

#define mt53_top            (mt53_bin_at(0)->fd)   /* The topmost chunk */
#define mt53_last_remainder (mt53_bin_at(1))       /* remainder from last split */


/* Helper macro to initialize bins */

#define MT53_IAV(i)  mt53_bin_at(i), mt53_bin_at(i)

// number of bins - but changing this alone will not change the number of
// bins!
#define MT53_MEMALLOC_ALLOCATOR_DLMALLOC_NAV 128

static mt53_mbinptr av_[MT53_MEMALLOC_ALLOCATOR_DLMALLOC_NAV * 2 + 2] = {
 0, 0,
 MT53_IAV(0),   MT53_IAV(1),   MT53_IAV(2),   MT53_IAV(3),   MT53_IAV(4),   MT53_IAV(5),   MT53_IAV(6),   MT53_IAV(7),
 MT53_IAV(8),   MT53_IAV(9),   MT53_IAV(10),  MT53_IAV(11),  MT53_IAV(12),  MT53_IAV(13),  MT53_IAV(14),  MT53_IAV(15),
 MT53_IAV(16),  MT53_IAV(17),  MT53_IAV(18),  MT53_IAV(19),  MT53_IAV(20),  MT53_IAV(21),  MT53_IAV(22),  MT53_IAV(23),
 MT53_IAV(24),  MT53_IAV(25),  MT53_IAV(26),  MT53_IAV(27),  MT53_IAV(28),  MT53_IAV(29),  MT53_IAV(30),  MT53_IAV(31),
 MT53_IAV(32),  MT53_IAV(33),  MT53_IAV(34),  MT53_IAV(35),  MT53_IAV(36),  MT53_IAV(37),  MT53_IAV(38),  MT53_IAV(39),
 MT53_IAV(40),  MT53_IAV(41),  MT53_IAV(42),  MT53_IAV(43),  MT53_IAV(44),  MT53_IAV(45),  MT53_IAV(46),  MT53_IAV(47),
 MT53_IAV(48),  MT53_IAV(49),  MT53_IAV(50),  MT53_IAV(51),  MT53_IAV(52),  MT53_IAV(53),  MT53_IAV(54),  MT53_IAV(55),
 MT53_IAV(56),  MT53_IAV(57),  MT53_IAV(58),  MT53_IAV(59),  MT53_IAV(60),  MT53_IAV(61),  MT53_IAV(62),  MT53_IAV(63),
 MT53_IAV(64),  MT53_IAV(65),  MT53_IAV(66),  MT53_IAV(67),  MT53_IAV(68),  MT53_IAV(69),  MT53_IAV(70),  MT53_IAV(71),
 MT53_IAV(72),  MT53_IAV(73),  MT53_IAV(74),  MT53_IAV(75),  MT53_IAV(76),  MT53_IAV(77),  MT53_IAV(78),  MT53_IAV(79),
 MT53_IAV(80),  MT53_IAV(81),  MT53_IAV(82),  MT53_IAV(83),  MT53_IAV(84),  MT53_IAV(85),  MT53_IAV(86),  MT53_IAV(87),
 MT53_IAV(88),  MT53_IAV(89),  MT53_IAV(90),  MT53_IAV(91),  MT53_IAV(92),  MT53_IAV(93),  MT53_IAV(94),  MT53_IAV(95),
 MT53_IAV(96),  MT53_IAV(97),  MT53_IAV(98),  MT53_IAV(99),  MT53_IAV(100), MT53_IAV(101), MT53_IAV(102), MT53_IAV(103),
 MT53_IAV(104), MT53_IAV(105), MT53_IAV(106), MT53_IAV(107), MT53_IAV(108), MT53_IAV(109), MT53_IAV(110), MT53_IAV(111),
 MT53_IAV(112), MT53_IAV(113), MT53_IAV(114), MT53_IAV(115), MT53_IAV(116), MT53_IAV(117), MT53_IAV(118), MT53_IAV(119),
 MT53_IAV(120), MT53_IAV(121), MT53_IAV(122), MT53_IAV(123), MT53_IAV(124), MT53_IAV(125), MT53_IAV(126), MT53_IAV(127)
};

/* field-extraction macros */

#define mt53_first(b) ((b)->fd)
#define mt53_last(b)  ((b)->bk)

/*
  Indexing into bins
*/

#define mt53_bin_index(sz)                                                          \
(((((unsigned long)(sz)) >> 9) ==    0) ?       (((unsigned long)(sz)) >>  3): \
 ((((unsigned long)(sz)) >> 9) <=    4) ?  56 + (((unsigned long)(sz)) >>  6): \
 ((((unsigned long)(sz)) >> 9) <=   20) ?  91 + (((unsigned long)(sz)) >>  9): \
 ((((unsigned long)(sz)) >> 9) <=   84) ? 110 + (((unsigned long)(sz)) >> 12): \
 ((((unsigned long)(sz)) >> 9) <=  340) ? 119 + (((unsigned long)(sz)) >> 15): \
 ((((unsigned long)(sz)) >> 9) <= 1364) ? 124 + (((unsigned long)(sz)) >> 18): \
                                          126)
/*
  bins for chunks < 512 are all spaced MT53_SMALLBIN_WIDTH bytes apart, and hold
  identically sized chunks. This is exploited in malloc.
*/

#define MT53_MAX_SMALLBIN_SIZE   512
#define MT53_SMALLBIN_WIDTH        8
#define MT53_SMALLBIN_WIDTH_BITS   3
#define MT53_MAX_SMALLBIN        (MT53_MAX_SMALLBIN_SIZE / MT53_SMALLBIN_WIDTH) - 1

#define mt53_smallbin_index(sz)  (((unsigned long)(sz)) >> MT53_SMALLBIN_WIDTH_BITS)

/*
   Requests are `small' if both the corresponding and the next bin are small
*/

#define mt53_is_small_request(nb) (nb < MT53_MAX_SMALLBIN_SIZE - MT53_SMALLBIN_WIDTH)

/*
    To help compensate for the large number of bins, a one-level index
    structure is used for bin-by-bin searching.  `mt53_binblocks' is a
    one-word bitvector recording whether groups of BINBLOCKWIDTH bins
    have any (possibly) non-empty bins, so they can be skipped over
    all at once during during traversals. The bits are NOT always
    cleared as soon as all bins in a block are empty, but instead only
    when all are noticed to be empty during traversal in malloc.
*/

#define MT53_BINBLOCKWIDTH     4   /* bins per block */

#define mt53_binblocks      (mt53_bin_at(0)->size) /* bitvector of nonempty blocks */

/* bin<->block macros */

#define mt53_idx2binblock(ix)    ((unsigned long)1 << (ix / MT53_BINBLOCKWIDTH))
#define mt53_mark_binblock(ii)   (mt53_binblocks |= mt53_idx2binblock(ii))
#define mt53_clear_binblock(ii)  (mt53_binblocks &= ~(mt53_idx2binblock(ii)))

//----------------------------------------------------------------------------
//
// malloc states
//
//----------------------------------------------------------------------------

// The first value returned from sbrk
static uint8* arenabase = NULL;

// The total memory in the pool
static int32 arenasize = 0;

//----------------------------------------------------------------------------

/*
  Debugging support
*/

void MT53_GPU_LockMalloc(void)
{
#if !defined(CC_MTK_LOADER)    
    ASSERT(x_sema_lock(_hGpuMallocSema, X_SEMA_OPTION_WAIT) ==
               OSR_OK);      
#endif               
}


void MT53_GPU_UnLockMalloc(void)
{
#if !defined(CC_MTK_LOADER)    
    ASSERT(x_sema_unlock(_hGpuMallocSema) == OSR_OK);
#endif    
}


#ifdef MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG

/*
  These routines make a number of assertions about the states
  of data structures that should be true at all times. If any
  are not true, it's very likely that a user program has somehow
  trashed memory. (It's also possible that there is a coding error
  in malloc. In which case, please report it!)
*/

static void
MT53_GPU_do_check_chunk( mt53_mchunkptr p )
{
  MT53_INTERNAL_SIZE_T sz = p->size & ~MT53_PREV_INUSE;

  /* Check for legal address ... */
  ASSERT((uint8 *)p >= arenabase);
  if (p != mt53_top)
    ASSERT((uint8 *)p + sz <= (uint8 *)mt53_top);
  else
    ASSERT((uint8 *)p + sz <= arenabase + arenasize);

} // MT53_GPU_do_check_chunk()


static void
MT53_GPU_do_check_free_chunk(mt53_mchunkptr p)
{
  MT53_INTERNAL_SIZE_T sz = p->size & ~MT53_PREV_INUSE;
  mt53_mchunkptr next = mt53_chunk_at_offset(p, sz);

  MT53_GPU_do_check_chunk(p);

  /* Check whether it claims to be free ... */
  ASSERT(!mt53_inuse(p));

  /* Unless a special marker, must have OK fields */
  if ((long)sz >= (long)MT53_MINSIZE)
  {
    ASSERT((sz & MT53_MALLOC_ALIGN_MASK) == 0);
    ASSERT(mt53_aligned_OK(mt53_chunk2mem(p)));
    /* ... matching footer field */
    ASSERT(next->prev_size == sz);
    /* ... and is fully consolidated */
    ASSERT(mt53_prev_inuse(p));
    ASSERT (next == mt53_top || mt53_inuse(next));

    /* ... and has minimally sane links */
    ASSERT(p->fd->bk == p);
    ASSERT(p->bk->fd == p);
  }
  else /* markers are always of size MT53_SIZE_SZ */
    ASSERT(sz == MT53_SIZE_SZ);
} // MT53_GPU_do_check_free_chunk()

static void
MT53_GPU_do_check_inuse_chunk(mt53_mchunkptr p)
{
  mt53_mchunkptr next = mt53_next_chunk(p);
  MT53_GPU_do_check_chunk(p);

  /* Check whether it claims to be in use ... */
  ASSERT(mt53_inuse(p));

  /* ... and is surrounded by OK chunks.
    Since more things can be checked with free chunks than mt53_inuse ones,
    if an mt53_inuse chunk borders them and debug is on, it's worth doing them.
  */
  if (!mt53_prev_inuse(p))
  {
    mt53_mchunkptr prv = mt53_prev_chunk(p);
    ASSERT(mt53_next_chunk(prv) == p);
    MT53_GPU_do_check_free_chunk(prv);
  }
  if (next == mt53_top)
  {
    ASSERT(mt53_prev_inuse(next));
    ASSERT(mt53_chunksize(next) >= MT53_MINSIZE);
  }
  else if (!mt53_inuse(next))
    MT53_GPU_do_check_free_chunk(next);

} // MT53_GPU_do_check_inuse_chunk(

static void
MT53_GPU_do_check_malloced_chunk(mt53_mchunkptr p, MT53_INTERNAL_SIZE_T s)
{
  MT53_INTERNAL_SIZE_T sz = p->size & ~MT53_PREV_INUSE;
  long room = mt53_long_sub_size_t(sz, s);

  MT53_GPU_do_check_inuse_chunk(p);

  /* Legal size ... */
  ASSERT((long)sz >= (long)MT53_MINSIZE);
  ASSERT((sz & MT53_MALLOC_ALIGN_MASK) == 0);
  ASSERT(room >= 0);
  ASSERT(room < (long)MT53_MINSIZE);

  /* ... and alignment */
  ASSERT(mt53_aligned_OK(mt53_chunk2mem(p)));


  /* ... and was allocated at front of an available chunk */
  ASSERT(mt53_prev_inuse(p));

} // MT53_GPU_do_check_malloced_chunk(


#define mt53_check_free_chunk(P)  MT53_GPU_do_check_free_chunk(P)
#define mt53_check_inuse_chunk(P) MT53_GPU_do_check_inuse_chunk(P)
#define mt53_check_chunk(P) MT53_GPU_do_check_chunk(P)
#define mt53_check_malloced_chunk(P,N) MT53_GPU_do_check_malloced_chunk(P,N)
#else
#define mt53_check_free_chunk(P)
#define mt53_check_inuse_chunk(P)
#define mt53_check_chunk(P)
#define mt53_check_malloced_chunk(P,N)
#endif


//----------------------------------------------------------------------------

/*
  Macro-based internal utilities
*/


/*
  Linking chunks in bin lists.
  Call these only with variables, not arbitrary expressions, as arguments.
*/

/*
  Place chunk p of size s in its bin, in size order,
  putting it ahead of others of same size.
*/


#define mt53_frontlink(P, S, IDX, BK, FD)                                          \
{                                                                             \
  if (S < MT53_MAX_SMALLBIN_SIZE)                                                  \
  {                                                                           \
    IDX = mt53_smallbin_index(S);                                                  \
    mt53_mark_binblock(IDX);                                                       \
    BK = mt53_bin_at(IDX);                                                         \
    FD = BK->fd;                                                              \
    P->bk = BK;                                                               \
    P->fd = FD;                                                               \
    FD->bk = BK->fd = P;                                                      \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    IDX = mt53_bin_index(S);                                                       \
    BK = mt53_bin_at(IDX);                                                         \
    FD = BK->fd;                                                              \
    if (FD == BK) mt53_mark_binblock(IDX);                                         \
    else                                                                      \
    {                                                                         \
      while (FD != BK && S < mt53_chunksize(FD)) FD = FD->fd;                      \
      BK = FD->bk;                                                            \
    }                                                                         \
    P->bk = BK;                                                               \
    P->fd = FD;                                                               \
    FD->bk = BK->fd = P;                                                      \
  }                                                                           \
}


/* take a chunk off a list */

#define mt53_unlink(P, BK, FD)                                                     \
{                                                                             \
  BK = P->bk;                                                                 \
  FD = P->fd;                                                                 \
  FD->bk = BK;                                                                \
  BK->fd = FD;                                                                \
}                                                                             \

/* Place p as the last remainder */

#define mt53_link_last_remainder(P)                                                \
{                                                                             \
  mt53_last_remainder->fd = mt53_last_remainder->bk =  P;                               \
  P->fd = P->bk = mt53_last_remainder;                                             \
}

/* Clear the mt53_last_remainder bin */

#define mt53_clear_last_remainder \
  (mt53_last_remainder->fd = mt53_last_remainder->bk = mt53_last_remainder)


//----------------------------------------------------------------------------

int MT53_GPU_dlmem_init(void *base, int32 size)
{
    uint32 front_misalign;
    int32 correction;

	static int instances = 0;

	//printk("[weiwei][MT53_GPU_dlmem_init] base=0x%8x size =0x%8x\n",base, size);
    if ( ++instances > 1 )
    {
    	// init multiple times
    	ASSERT(0);
    	return 0;
	}
	//printk("[weiwei][MT53_GPU_dlmem_init] base=%d size = %d\n",base,size);

    arenabase = (uint8*)base;
    arenasize = size;       


    front_misalign = (uint32)mt53_chunk2mem(base) & MT53_MALLOC_ALIGN_MASK;

    if ( front_misalign > 0 ) {
        correction = (MT53_MALLOC_ALIGNMENT) - front_misalign;
    } else {
        correction = 0;
    }

    // too small to be useful?
    if ( correction + 2*MT53_MALLOC_ALIGNMENT > (unsigned) size )
        // help catch errors. Don't fail now.
        arenabase = NULL;
    else {
        //mt53_top = (mt53_mchunkptr)((uint8*)base + correction);
mt53_top = (mt53_mchunkptr)((unsigned long)base + correction);
        mt53_set_head(mt53_top, arenasize | MT53_PREV_INUSE);
    }

    // Create mutex	
    if (x_sema_create
            (&_hGpuMallocSema, X_SEMA_TYPE_BINARY,
             X_SEMA_STATE_LOCK) != OSR_OK)
    {
            ASSERT(0);
    }

	if (x_sema_create
            (&_hGpuMallocSema, X_SEMA_TYPE_BINARY,
             X_SEMA_STATE_UNLOCK) != OSR_OK)
    {
            ASSERT(0);
    }

    return 1;
}	// mem_init

//----------------------------------------------------------------------------

/* Main public routines */

/*
  Malloc Algorithm:

    The requested size is first converted into a usable form, `nb'.
    This currently means to add 4 bytes overhead plus possibly more to
    obtain 8-byte alignment and/or to obtain a size of at least
    MT53_MINSIZE (currently 16 bytes), the smallest allocatable size.
    (All fits are considered `exact' if they are within MT53_MINSIZE bytes.)

    From there, the first successful of the following steps is taken:

      1. The bin corresponding to the request size is scanned, and if
         a chunk of exactly the right size is found, it is taken.

      2. The most recently remaindered chunk is used if it is big
         enough.  This is a form of (roving) first fit, used only in
         the absence of exact fits. Runs of consecutive requests use
         the remainder of the chunk used for the previous such request
         whenever possible. This limited use of a first-fit style
         allocation strategy tends to give contiguous chunks
         coextensive lifetimes, which improves locality and can reduce
         fragmentation in the long run.

      3. Other bins are scanned in increasing size order, using a
         chunk big enough to fulfill the request, and splitting off
         any remainder.  This search is strictly by best-fit; i.e.,
         the smallest (with ties going to approximately the least
         recently used) chunk that fits is selected.

      4. If large enough, the chunk bordering the end of memory
         (`top') is split off. (This use of `top' is in accord with
         the best-fit search rule.  In effect, `top' is treated as
         larger (and thus less well fitting) than any other available
         chunk since it can be extended to be as large as necessary
         (up to system limitations).

      All allocations are made from the the `lowest' part of any found
      chunk. (The implementation invariant is that mt53_prev_inuse is
      always true of any allocated chunk; i.e., that each allocated
      chunk borders either a previously allocated and still in-use chunk,
      or the base of its memory arena.)

*/

void*
MT53_GPU_dlmalloc( int32 bytes )
{
  mt53_mchunkptr victim;                  /* inspected/selected chunk */
  MT53_INTERNAL_SIZE_T victim_size;       /* its size */
  int       idx;                     /* index for bin traversal */
  mt53_mbinptr   bin;                     /* associated bin */
  mt53_mchunkptr remainder;               /* remainder from a split */
  long      remainder_size;          /* its size */
  int       remainder_index;         /* its bin index */
  unsigned long block;               /* block traverser bit */
  int       startidx;                /* first bin of a traversed block */
  mt53_mchunkptr fwd;                     /* misc temp for linking */
  mt53_mchunkptr bck;                     /* misc temp for linking */
  mt53_mbinptr q;                         /* misc temp */

  MT53_INTERNAL_SIZE_T nb;

  /*  Allow uninitialised (zero sized) heaps because they could exist as a
   *  quirk of the MLT setup where a dynamically sized heap is at the top of
   *  memory. */
   //printk("[weiwei][MT53_GPU_dlmalloc]  111\n");
  if (NULL==arenabase) return NULL;
  //printk("[weiwei][MT53_GPU_dlmalloc]  222\n");

  if ((long)bytes < 0) return 0;
  //printk("[weiwei][MT53_GPU_dlmalloc]  333\n");

  nb = mt53_request2size(bytes);  /* padded request size; */

  MALLOC_LOCK;

  /* Check for exact match in a bin */

  if (mt53_is_small_request(nb))  /* Faster version for small requests */
  {
    idx = mt53_smallbin_index(nb);

    /* No traversal or size check necessary for small bins.  */

    q = mt53_bin_at(idx);
    victim = mt53_last(q);

#if MT53_MALLOC_ALIGN != 16
    /* Also scan the next one, since it would have a remainder < MT53_MINSIZE */
    if (victim == q)
    {
      q = mt53_next_bin(q);
      victim = mt53_last(q);
    }
#endif
    if (victim != q)
    {
      victim_size = mt53_chunksize(victim);
      mt53_unlink(victim, bck, fwd);
      mt53_set_inuse_bit_at_offset(victim, victim_size);
      mt53_check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
	  //printk("[weiwei][MT53_GPU_dlmalloc]  444\n");
      return (void*)mt53_chunk2mem(victim);
    }

    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = mt53_bin_index(nb);
    bin = mt53_bin_at(idx);

    for (victim = mt53_last(bin); victim != bin; victim = victim->bk)
    {
      victim_size = mt53_chunksize(victim);
      remainder_size = mt53_long_sub_size_t(victim_size, nb);

      if (remainder_size >= (long)MT53_MINSIZE) /* too big */
      {
        --idx; /* adjust to rescan below after checking last remainder */
        break;
      }

      else if (remainder_size >= 0) /* exact fit */
      {
        mt53_unlink(victim, bck, fwd);
        mt53_set_inuse_bit_at_offset(victim, victim_size);
        mt53_check_malloced_chunk(victim, nb);
        MALLOC_UNLOCK;
		//printk("[weiwei][MT53_GPU_dlmalloc]  555\n");
        return (void*)mt53_chunk2mem(victim);
      }
    }

    ++idx;

  }

  /* Try to use the last split-off remainder */

  if ( (victim = mt53_last_remainder->fd) != mt53_last_remainder)
  {
    victim_size = mt53_chunksize(victim);
    remainder_size = mt53_long_sub_size_t(victim_size, nb);

    if (remainder_size >= (long)MT53_MINSIZE) /* re-split */
    {
      remainder = mt53_chunk_at_offset(victim, nb);
      mt53_set_head(victim, nb | MT53_PREV_INUSE);
      mt53_link_last_remainder(remainder);
      mt53_set_head(remainder, remainder_size | MT53_PREV_INUSE);
      mt53_set_foot(remainder, remainder_size);
      mt53_check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
	  //printk("[weiwei][MT53_GPU_dlmalloc]  666\n");
      return (void*)mt53_chunk2mem(victim);
    }

    mt53_clear_last_remainder;

    if (remainder_size >= 0)  /* exhaust */
    {
      mt53_set_inuse_bit_at_offset(victim, victim_size);
      mt53_check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
	  //printk("[weiwei][MT53_GPU_dlmalloc]  666\n");
      return (void*)mt53_chunk2mem(victim);
    }

    /* Else place in bin */

    mt53_frontlink(victim, victim_size, remainder_index, bck, fwd);
  }

  /*
     If there are any possibly nonempty big-enough blocks,
     search for best fitting chunk by scanning bins in blockwidth units.
  */

  if ( (block = mt53_idx2binblock(idx)) <= mt53_binblocks)
  {

    /* Get to the first marked block */

    if ( (block & mt53_binblocks) == 0)
    {
      /* force to an even block boundary */
      idx = (idx & ~(MT53_BINBLOCKWIDTH - 1)) + MT53_BINBLOCKWIDTH;
      block <<= 1;
      while ((block & mt53_binblocks) == 0)
      {
        idx += MT53_BINBLOCKWIDTH;
        block <<= 1;
      }
    }

    /* For each possibly nonempty block ... */
    for (;;)
    {
      startidx = idx;          /* (track incomplete blocks) */
      q = bin = mt53_bin_at(idx);

      /* For each bin in this block ... */
      do
      {
        /* Find and use first big enough chunk ... */

        for (victim = mt53_last(bin); victim != bin; victim = victim->bk)
        {
          victim_size = mt53_chunksize(victim);
          remainder_size = mt53_long_sub_size_t(victim_size, nb);

          if (remainder_size >= (long)MT53_MINSIZE) /* split */
          {
            remainder = mt53_chunk_at_offset(victim, nb);
            mt53_set_head(victim, nb | MT53_PREV_INUSE);
            mt53_unlink(victim, bck, fwd);
            mt53_link_last_remainder(remainder);
            mt53_set_head(remainder, remainder_size | MT53_PREV_INUSE);
            mt53_set_foot(remainder, remainder_size);
            mt53_check_malloced_chunk(victim, nb);
            MALLOC_UNLOCK;
			//printk("[weiwei][MT53_GPU_dlmalloc]  777\n");
            return (void*)mt53_chunk2mem(victim);
          }

          else if (remainder_size >= 0)  /* take */
          {
            mt53_set_inuse_bit_at_offset(victim, victim_size);
            mt53_unlink(victim, bck, fwd);
            mt53_check_malloced_chunk(victim, nb);
            MALLOC_UNLOCK;
			//printk("[weiwei][MT53_GPU_dlmalloc]  888\n");
            return (void*)mt53_chunk2mem(victim);
          }

        }

       bin = mt53_next_bin(bin);

#if MT53_MALLOC_ALIGN == 16
       if (idx < MT53_MAX_SMALLBIN)
         {
           bin = mt53_next_bin(bin);
           ++idx;
         }
#endif
      } while ((++idx & (MT53_BINBLOCKWIDTH - 1)) != 0);

      /* Clear out the block bit. */

      do   /* Possibly backtrack to try to clear a partial block */
      {
        if ((startidx & (MT53_BINBLOCKWIDTH - 1)) == 0)
        {
          mt53_binblocks &= ~block;
          break;
        }
        --startidx;
       q = mt53_prev_bin(q);
      } while (mt53_first(q) == q);

      /* Get to the next possibly nonempty block */

      if ( (block <<= 1) <= mt53_binblocks && (block != 0) )
      {
        while ((block & mt53_binblocks) == 0)
        {
          idx += MT53_BINBLOCKWIDTH;
          block <<= 1;
        }
      }
      else
        break;
    }
  }


  /* Try to use top chunk */

  /* Require that there be a remainder, ensuring top always exists  */
  remainder_size = mt53_long_sub_size_t(mt53_chunksize(mt53_top), nb);
  if (mt53_chunksize(mt53_top) < nb || remainder_size < (long)MT53_MINSIZE)
  {
      //diag_printk("mt53_chunksize(mt53_top)=%ld, nb=%d, remainder=%ld\n", mt53_chunksize(mt53_top),
      //            nb, remainder_size);
      MALLOC_UNLOCK;
	  //printk("[weiwei][MT53_GPU_dlmalloc]  999\n");
      return NULL; /* propagate failure */
  }

  victim = mt53_top;
  mt53_set_head(victim, nb | MT53_PREV_INUSE);
  mt53_top = mt53_chunk_at_offset(victim, nb);
  mt53_set_head(mt53_top, remainder_size | MT53_PREV_INUSE);
  mt53_check_malloced_chunk(victim, nb);
  MALLOC_UNLOCK;
  //printk("[weiwei][MT53_GPU_dlmalloc]  aaa\n");
  return (void*)mt53_chunk2mem(victim);

} // malloc()

void* MT53_GPU_dlcalloc(int32 nmemb, int32 size)
{
	int32 realsize = nmemb * size;

	void* ptr = MT53_GPU_dlmalloc(realsize);

    // Fill with 0's if non-NULL
    if (ptr != NULL)
        x_memset(ptr, 0, realsize );

	return ptr;
}

//----------------------------------------------------------------------------

/*
  free() algorithm :

    cases:

       1. free(NULL) has no effect.

       2. Chunks are consolidated as they arrive, and
          placed in corresponding bins. (This includes the case of
          consolidating with the current `last_remainder').
*/

int
MT53_GPU_dlfree( void* _mem )
{
  mt53_mchunkptr p;         /* chunk corresponding to mem */
  MT53_INTERNAL_SIZE_T hd;  /* its head field */
  MT53_INTERNAL_SIZE_T sz;  /* its size */
  int       idx;       /* its bin index */
  mt53_mchunkptr next;      /* next contiguous chunk */
  MT53_INTERNAL_SIZE_T nextsz; /* its size */
  MT53_INTERNAL_SIZE_T prevsz; /* size of previous contiguous chunk */
  mt53_mchunkptr bck;       /* misc temp for linking */
  mt53_mchunkptr fwd;       /* misc temp for linking */
  int       islr;      /* track whether merging with last_remainder */

  uint8* mem = (uint8*)_mem;

  if (mem == NULL)                              /* free(NULL) has no effect */
    return 0;

  MALLOC_LOCK;

  p = mt53_mem2chunk(mem);
  hd = p->size;

  mt53_check_inuse_chunk(p);

  sz = hd & ~MT53_PREV_INUSE;
  next = mt53_chunk_at_offset(p, sz);
  nextsz = mt53_chunksize(next);

  if (next == mt53_top)                            /* merge with mt53_top */
  {
    sz += nextsz;

    if (!(hd & MT53_PREV_INUSE))                    /* consolidate backward */
    {
      prevsz = p->prev_size;
      p = mt53_chunk_at_offset(p, -((long) prevsz));
      sz += prevsz;
      mt53_unlink(p, bck, fwd);
    }

    mt53_set_head(p, sz | MT53_PREV_INUSE);
    mt53_top = p;
    MALLOC_UNLOCK;
    return 1;
  }

  mt53_set_head(next, nextsz);                    /* clear inuse bit */

  islr = 0;

  if (!(hd & MT53_PREV_INUSE))                    /* consolidate backward */
  {
    prevsz = p->prev_size;
    p = mt53_chunk_at_offset(p, -((long) prevsz));
    sz += prevsz;

    if (p->fd == mt53_last_remainder)             /* keep as last_remainder */
      islr = 1;
    else
      mt53_unlink(p, bck, fwd);
  }

  if (!(mt53_inuse_bit_at_offset(next, nextsz)))   /* consolidate forward */
  {
    sz += nextsz;

    if (!islr && next->fd == mt53_last_remainder)  /* re-insert last_remainder */
    {
      islr = 1;
      mt53_link_last_remainder(p);
    }
    else
      mt53_unlink(next, bck, fwd);
  }


  mt53_set_head(p, sz | MT53_PREV_INUSE);
  mt53_set_foot(p, sz);
  if (!islr)
    mt53_frontlink(p, sz, idx, bck, fwd);

  MALLOC_UNLOCK;

  return 1;
} // free()

//----------------------------------------------------------------------------

// resize existing allocation, if oldsize is non-NULL, previous
// allocation size is placed into it. If previous size not available,
// it is set to 0. NB previous allocation size may have been rounded up.
// Occasionally the allocation can be adjusted *backwards* as well as,
// or instead of forwards, therefore the address of the resized
// allocation is returned, or NULL if no resizing was possible.
// Note that this differs from ::realloc() in that no attempt is
// made to call malloc() if resizing is not possible - that is left
// to higher layers. The data is copied from old to new though.
// The effects of alloc_ptr==NULL or newsize==0 are undefined


// DOCUMENTATION FROM ORIGINAL FILE:
// (some now irrelevant parts elided)
/*
  Realloc algorithm:

    If the reallocation is for additional space, and the
    chunk can be extended, it is, else a malloc-copy-free sequence is
    taken.  There are several different ways that a chunk could be
    extended. All are tried:

       * Extending forward into following adjacent free chunk.
       * Shifting backwards, joining preceding adjacent space
       * Both shifting backwards and extending forward.

    If the reallocation is for less space, and the new request is for
    a `small' (<512 bytes) size, then the newly unused space is lopped
    off and freed.

    The old unix realloc convention of allowing the last-free'd chunk
    to be used as an argument to realloc is no longer supported.
    I don't know of any programs still relying on this feature,
    and allowing it would also allow too many other incorrect
    usages of realloc to be sensible.
*/

void*
MT53_GPU_dlresize_alloc( void* _oldmem, int32 bytes, int32 *poldsize )
{

  MT53_INTERNAL_SIZE_T nb;             /* padded request size */

  mt53_mchunkptr oldp;                 /* chunk corresponding to oldmem */
  MT53_INTERNAL_SIZE_T oldsize;        /* its size */

  mt53_mchunkptr newp;                 /* chunk to return */
  MT53_INTERNAL_SIZE_T newsize;        /* its size */
  uint8*   newmem;            /* corresponding user mem */

  mt53_mchunkptr next;                 /* next contiguous chunk after oldp */
  MT53_INTERNAL_SIZE_T nextsize;       /* its size */

  mt53_mchunkptr prev;                 /* previous contiguous chunk before oldp */
  MT53_INTERNAL_SIZE_T prevsize = 0;       /* its size */

  mt53_mchunkptr remainder;            /* holds split off extra space from newp */
  MT53_INTERNAL_SIZE_T remainder_size; /* its size */

  mt53_mchunkptr bck;                  /* misc temp for linking */
  mt53_mchunkptr fwd;                  /* misc temp for linking */

  uint8* oldmem = (uint8*)_oldmem;

  MALLOC_LOCK;

  newp    = oldp    = mt53_mem2chunk(oldmem);
  newsize = oldsize = mt53_chunksize(oldp);

  if (NULL != poldsize)
      *poldsize = oldsize - MT53_SIZE_SZ;

  nb = mt53_request2size(bytes);

  mt53_check_inuse_chunk(oldp);

  if ((long)(oldsize) < (long)(nb))
  {

    /* Try expanding forward */

    next = mt53_chunk_at_offset(oldp, oldsize);
    if (next == mt53_top || !mt53_inuse(next))
    {
      nextsize = mt53_chunksize(next);

      /* Forward into mt53_top only if a remainder */
      if (next == mt53_top)
      {
        if ((long)(nextsize + newsize) >= (long)(nb + MT53_MINSIZE))
        {
          newsize += nextsize;
          mt53_top = mt53_chunk_at_offset(oldp, nb);
          mt53_set_head(mt53_top, (newsize - nb) | MT53_PREV_INUSE);
          mt53_set_head_size(oldp, nb);
          MALLOC_UNLOCK;
          return (void*)mt53_chunk2mem(oldp);
        }
      }

      /* Forward into next chunk */
      else if (((long)(nextsize + newsize) >= (long)(nb)))
      {
        mt53_unlink(next, bck, fwd);
        newsize  += nextsize;
        goto split;
      }
    }
    else
    {
      next = 0;
      nextsize = 0;
    }

    /* Try shifting backwards. */

    if (!mt53_prev_inuse(oldp))
    {
      prev = mt53_prev_chunk(oldp);
	  
	  if(prev == 0)
	  {
	  	//dbgprint("^R^ prev == NULL %s %d\n",__func__,__LINE__);
	  }
	  if(prev != NULL)
	  {
	      prevsize = mt53_chunksize(prev);
	  }

      /* try forward + backward first to save a later consolidation */

      if (next != 0)
      {
        /* into mt53_top */
        if (next == mt53_top)
        {
          if ((long)(nextsize + prevsize + newsize) >= (long)(nb + MT53_MINSIZE))
          {
            mt53_unlink(prev, bck, fwd);
            newp = prev;
            newsize += prevsize + nextsize;
            newmem = mt53_chunk2mem(newp);
            MT53_MALLOC_COPY(newmem, oldmem, oldsize - MT53_SIZE_SZ);
            mt53_top = mt53_chunk_at_offset(newp, nb);
            mt53_set_head(mt53_top, (newsize - nb) | MT53_PREV_INUSE);
            mt53_set_head_size(newp, nb);
            MALLOC_UNLOCK;
            return (void*)newmem;
          }
        }

        /* into next chunk */
        else if (((long)(nextsize + prevsize + newsize) >= (long)(nb)))
        {
          mt53_unlink(next, bck, fwd);
          mt53_unlink(prev, bck, fwd);
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = mt53_chunk2mem(newp);
          MT53_MALLOC_COPY(newmem, oldmem, oldsize - MT53_SIZE_SZ);
          goto split;
        }
      }

      /* backward only */
      if (prev != NULL && (long)(prevsize + newsize) >= (long)nb)
      {
        mt53_unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
        newmem = mt53_chunk2mem(newp);
        MT53_MALLOC_COPY(newmem, oldmem, oldsize - MT53_SIZE_SZ);
        goto split;
      }
    }

    // couldn't resize the allocation any direction, so return failure
    MALLOC_UNLOCK;
    return NULL;
  }


 split:  /* split off extra room in old or expanded chunk */

  remainder_size = mt53_long_sub_size_t(newsize, nb);

  if (remainder_size >= (long)MT53_MINSIZE) /* split off remainder */
  {
    remainder = mt53_chunk_at_offset(newp, nb);
    mt53_set_head_size(newp, nb);
    mt53_set_head(remainder, remainder_size | MT53_PREV_INUSE);
    mt53_set_inuse_bit_at_offset(remainder, remainder_size);
    /* let free() deal with it */
    MT53_GPU_dlfree( mt53_chunk2mem(remainder) );
  }
  else
  {
    mt53_set_head_size(newp, newsize);
    mt53_set_inuse_bit_at_offset(newp, newsize);
  }

  mt53_check_inuse_chunk(newp);
  MALLOC_UNLOCK;
  return (void*)mt53_chunk2mem(newp);

} // resize_alloc()

void*
MT53_GPU_dlrealloc(void* ptr, int32 size)
{
	int32 oldsize;

	return MT53_GPU_dlresize_alloc(ptr, size, &oldsize);
}

//----------------------------------------------------------------------------

// Get memory pool status
// flags is a bitmask of requested fields to fill in. The flags are
// defined in common.hxx
void
MT53_GPU_dlmem_get_status(uint32 flags, struct MT53_GPU_mempool_status* status )
{
    status->arenabase       = (const uint8 *)-1;
    status->arenasize       = -1;
    status->freeblocks      = -1;
    status->totalallocated  = -1;
    status->totalfree       = -1;
    status->blocksize       = -1;
    status->maxfree         = -1;
    status->waiting         = (char)-1;
    status->origbase        = (const uint8 *)-1;
    status->origsize        = -1;
    status->maxoverhead     = (char)-1;

    if (0 != (flags & (MT53_GPU_MEMPOOL_STAT_FREEBLOCKS | MT53_GPU_MEMPOOL_STAT_TOTALFREE |
                     MT53_GPU_MEMPOOL_STAT_TOTALALLOCATED | MT53_GPU_MEMPOOL_STAT_MAXFREE)))
    {
        int i;
        mt53_mbinptr b;
        mt53_mchunkptr p;
        int32 chunksizep;
        int32 maxfree;
#ifdef MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG
        mt53_mchunkptr q;
#endif

        MT53_INTERNAL_SIZE_T avail = mt53_chunksize(mt53_top);
        int   navail = ((long)(avail) >= (long)MT53_MINSIZE)? 1 : 0;
        maxfree = avail;



        for (i = 1; i < MT53_MEMALLOC_ALLOCATOR_DLMALLOC_NAV; ++i) {
            b = mt53_bin_at(i);
            for (p = mt53_last(b); p != b; p = p->bk) {
#ifdef MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG
                mt53_check_free_chunk(p);
                for (q = mt53_next_chunk(p);
                     (q < mt53_top) && mt53_inuse(q) &&
                         (long)(mt53_chunksize(q)) >= (long)MT53_MINSIZE;
                     q = mt53_next_chunk(q))
                    mt53_check_inuse_chunk(q);
#endif
                chunksizep = mt53_chunksize(p);
                avail += chunksizep;
                if ( chunksizep > maxfree )
                    maxfree = chunksizep;
                navail++;
            }
        }

        if ( 0 != (flags & MT53_GPU_MEMPOOL_STAT_TOTALALLOCATED) )
            status->totalallocated = arenasize - avail;
        // as quick or quicker to just set most of these, rather than
        // test flag first
        status->totalfree = (avail & ~(MT53_MALLOC_ALIGN_MASK)) - MT53_SIZE_SZ - MT53_MINSIZE;
        ASSERT( ((avail + MT53_SIZE_SZ + MT53_MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)
            >= MT53_MINSIZE);                        // free mem negative!
        status->freeblocks = navail;
        status->maxfree = (maxfree & ~(MT53_MALLOC_ALIGN_MASK)) - MT53_SIZE_SZ - MT53_MINSIZE;
        //diag_printk("raw mf: %d, ret mf: %d\n", maxfree, status.maxfree);
        ASSERT( ((maxfree + MT53_SIZE_SZ + MT53_MALLOC_ALIGN_MASK) &
            ~MALLOC_ALIGN_MASK) >= MT53_MINSIZE);    // max free block size negative!
    } // if

    // as quick or quicker to just set most of these, rather than
    // test flag first
    status->arenabase = status->origbase = arenabase;
    status->arenasize = status->origsize = arenasize;
    status->maxoverhead = MT53_MINSIZE + MT53_MALLOC_ALIGNMENT;

} // mem_get_status()


/*

struct mallinfo
mallinfo( void )
{
    struct mallinfo ret = { 0 }; // initialize to all zeros
    struct MT53_GPU_mempool_status stat;

    mem_get_status( MEMPOOL_STAT_ARENASIZE |
                    MEMPOOL_STAT_FREEBLOCKS |
                    MEMPOOL_STAT_TOTALALLOCATED |
                    MEMPOOL_STAT_TOTALFREE |
                    MEMPOOL_STAT_MAXFREE, &stat );

    if ( stat.arenasize > 0 )
        ret.arena = stat.arenasize;

    if ( stat.freeblocks > 0 )
        ret.ordblks = stat.freeblocks;

    if ( stat.totalallocated > 0 )
        ret.uordblks = stat.totalallocated;

    if ( stat.totalfree > 0 )
        ret.fordblks = stat.totalfree;

    if ( stat.maxfree > 0 )
        ret.maxfree = stat.maxfree;

    return ret;
} // mallinfo()

*/

//----------------------------------------------------------------------------

// EOF dlmalloc.cxx
