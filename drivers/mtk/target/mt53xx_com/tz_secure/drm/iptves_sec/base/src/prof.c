/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2007,2008,2009 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "prof_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"

#if defined(ENABLE_PROFILING_TIME)

COMPONENT(BASE);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_INDENT_STEP   2

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/
static u_int32_t l_indent = 0;

/*============================================================================
 * Local Functions
 *==========================================================================*/
u_int32_t
prof_l_GetIndent(void)
{
    return l_indent;
}

void
prof_l_IncIndent(void)
{
    l_indent += L_INDENT_STEP;
}

void
prof_l_DecIndent(void)
{
    l_indent -= L_INDENT_STEP;
}

#endif /* defined(ENABLE_PROFILING_TIME) */

#if defined(ENABLE_PROFILING_SPACE)

COMPONENT(BASE);

#define L_MAGICSTRINGLEN 16
#define L_MAGICSTRING "MAGICmagicMaGiC!"
#if !defined(PROF_MAXSTACKSIZE)
#  define PROF_MAXSTACKSIZE (768*1024)
#endif

#define L_PROF_HEADERSIZE 8
#define L_PROF_ALLIGN 16

#if 0
static prof_l_memstat_t l_max[PROF_L_INDEX_MAX] = { {0},{0} };
static prof_l_memstat_t l_min[PROF_L_INDEX_MAX] = { {0},{0} };
static prof_l_memstat_t l_cur[PROF_L_INDEX_MAX] = { {0},{0} };
#endif
static char *l_stacktop=0, *l_stackbtm=0;

int
prof_l_SpaceStart(prof_l_context_t *io_context)
{
    char buf[PROF_MAXSTACKSIZE];
    size_t i, l;

    exh_InitOk();

    for (i = 0; i < PROF_MAXSTACKSIZE; i += L_MAGICSTRINGLEN) {
        l = PROF_MAXSTACKSIZE-i;
        if ( l > L_MAGICSTRINGLEN ) l = L_MAGICSTRINGLEN;
        exh_Check(base_Memcpy(buf+i, l, L_MAGICSTRING, l));
    }
    l_stacktop = l_stackbtm = buf+PROF_MAXSTACKSIZE;
#if 0
    exh_Check(base_Memcpy(io_context->bk_max, sizeof(l_max), l_max, sizeof(l_max)));
    exh_Check(base_Memcpy(io_context->bk_min, sizeof(l_min), l_min, sizeof(l_min)));
    exh_Check(base_Memcpy(l_max, sizeof(l_max), l_cur, sizeof(l_max)));
    exh_Check(base_Memcpy(l_min, sizeof(l_min), l_cur, sizeof(l_min)));
#endif

exh_CLEANUP:
    exh_Return();
}



int
prof_l_SpaceStack()
{
    char a;
    if ( l_stackbtm > &a ) l_stackbtm = &a;
    return RET_OK;
}

int
prof_l_SpaceStop(prof_l_context_t *io_context)
{
    char buf[PROF_MAXSTACKSIZE];
    int i, j;
    int32_t a, b;

    for (i = 0; i < PROF_MAXSTACKSIZE; i ++) {
        j = i % L_MAGICSTRINGLEN;
        if ( buf[i] != L_MAGICSTRING[j] ) break;
    }
    a = PROF_MAXSTACKSIZE - i;
    b = l_stacktop - l_stackbtm;
    io_context->sz_stack = (a > b && a < PROF_MAXSTACKSIZE) ? a : b;
#if 0
    exh_Check(base_Memcpy(io_context->sz_min, sizeof(l_min), l_min, sizeof(l_min)));
    exh_Check(base_Memcpy(io_context->sz_max, sizeof(l_max), l_max, sizeof(l_max)));
    for (i = 0; i < PROF_L_INDEX_MAX; i ++) {
        if ( l_min[i].size  > io_context->bk_min[i].size  )
            l_min[i].size  = io_context->bk_min[i].size;
        if ( l_min[i].count > io_context->bk_min[i].count )
            l_min[i].count = io_context->bk_min[i].count;
        if ( l_min[i].esize > io_context->bk_min[i].esize )
            l_min[i].esize = io_context->bk_min[i].esize;
        if ( l_max[i].size  > io_context->bk_max[i].size  )
            l_max[i].size  = io_context->bk_max[i].size;
        if ( l_max[i].count > io_context->bk_max[i].count )
            l_max[i].count = io_context->bk_max[i].count;
        if ( l_max[i].esize > io_context->bk_max[i].esize )
            l_max[i].esize = io_context->bk_max[i].esize;
    }
#endif
    return RET_OK;
}

int
prof_l_SpacePrint(prof_l_context_t *in_context)
{
#if 1
    MSG0(("***\n"));
    MSG0(("*** Profile(space):%s = %d [bytes]\n", in_context->title, in_context->sz_stack));
    MSG0(("***\n"));
#else
    int i;
    MSG0(("*** Profile(space):title=%s\n", in_context->title));
    for (i = 0; i < PROF_L_INDEX_MAX; i ++) {
        MSG0(("***   %d:s:%5d h:%6d/%4d/%6d~%6d/%4d/%6d d:%6d/%4d/%6d\n",
              i, in_context->sz_stack,
              in_context->sz_min[i].size, in_context->sz_min[i].count,
              in_context->sz_min[i].esize,
              in_context->sz_max[i].size, in_context->sz_max[i].count,
              in_context->sz_max[i].esize,
              in_context->sz_max[i].size -in_context->sz_min[i].size,
              in_context->sz_max[i].count-in_context->sz_min[i].count,
              in_context->sz_max[i].esize-in_context->sz_min[i].esize
              ));
    }
#endif
    return RET_OK;;
}

#if 0
static int32_t
l_Estimated(int32_t in_sz)
{
#if defined(NPOPT_USE_LIBC_MALLOC)
    return (in_sz+(L_PROF_HEADERSIZE+L_PROF_ALLIGN-1))&~(L_PROF_ALLIGN-1);
#else
    return ((in_sz+31)&~15);
#endif
}
#endif

int
prof_l_SpaceAlloc(int in_idx, int32_t in_sz)
{
#if 0
    l_cur[in_idx].size += in_sz;
    l_cur[in_idx].count ++;
    l_cur[in_idx].esize += l_Estimated(in_sz);
    if ( l_max[in_idx].size  < l_cur[in_idx].size  ) l_max[in_idx].size  = l_cur[in_idx].size;
    if ( l_max[in_idx].count < l_cur[in_idx].count ) l_max[in_idx].count = l_cur[in_idx].count;
    if ( l_max[in_idx].esize < l_cur[in_idx].esize ) l_max[in_idx].esize = l_cur[in_idx].esize;
#endif
    return RET_OK;
}

int
prof_l_SpaceFree(int in_idx, int32_t in_sz)
{
#if 0
    l_cur[in_idx].size -= in_sz;
    l_cur[in_idx].count --;
    l_cur[in_idx].esize -= l_Estimated(in_sz);
    if ( l_min[in_idx].size  > l_cur[in_idx].size  ) l_min[in_idx].size  = l_cur[in_idx].size;
    if ( l_min[in_idx].count > l_cur[in_idx].count ) l_min[in_idx].count = l_cur[in_idx].count;
    if ( l_min[in_idx].esize > l_cur[in_idx].esize ) l_min[in_idx].esize = l_cur[in_idx].esize;
#endif
    return RET_OK;
}

void
prof_l_SpaceCurrentStatus(int in_idx)
{
    MSG3(("PROF[idx:%d %d bytes/%d pts, estimated=%d bytes]\n",
          in_idx, l_cur[in_idx].size, l_cur[in_idx].count, l_cur[in_idx].esize));
}

#endif /* defined(ENABLE_PROFILING_SPACE) */
