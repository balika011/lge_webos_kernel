#ifndef __H_TYPE_C

#define __H_TYPE_C

//#include "stdio.h"
//#include "stdlib.h"
#include "x_os.h"
//#include <linux/types.h>


//memcpy
//#include <linux/string.h>


//#include <linux/vmalloc.h>

//#ifndef malloc 
//#define malloc vmalloc
//#endif
//
//#ifndef free
//#define free vfree
//#endif


#define free tom_free
#define malloc tom_malloc
//#define memcpy memcpy

//typedef unsigned int size_t;

#ifdef CC_SECURE_WORLD
#define strncmp x_strncmp
extern int x_strncmp(const char *ps_s1, const char *ps_s2, size_t z_l);

typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;
typedef unsigned char bool;

typedef long long int64_t;

static const uint8_t true  = 1;
static const uint8_t false = 0;

#define NULL 0
#endif

typedef uint32_t SessionId;

uint32_t net_to_host(uint32_t net_uint32);
int64_t htonll64(int64_t x);

#endif
