

#ifndef __MT53_SIMPLE_H__
#define __MT53_SIMPLE_H__

#ifdef __KERNEL__
#include "x_typedef.h"
#else
//typedef unsigned int UINT32;
#endif
typedef struct SIMPLE_CMD_T SIMPLE_CMD_T;
typedef struct SIMPLE_BUFFER_T SIMPLE_BUFFER_T;
typedef struct VDP_BUFFER_T VDP_BUFFER_T;
typedef struct VDPC_INFO_T VDPC_INFO_T;

#ifndef ASSERT
	#ifdef _DEBUG_
		#include <assert.h>
		#define ASSERT(x)				assert((x))
	#else
		#define ASSERT(x)
	#endif	// _DEBUG_
#endif	// ASSERT
#ifndef UN_USED
#define UN_USED(X) (void)X
#endif

struct SIMPLE_CMD_T
{
    UINT32 u4Cmd;
	UINT32 u4Arg;
};

struct SIMPLE_BUFFER_T
{
    UINT32 u4Addr;
	UINT32 u4Size;
};

struct VDP_BUFFER_T
{
    SIMPLE_BUFFER_T rBuffer;
	UINT32 u4Width;
	UINT32 u4Height;
	UINT32 u4Pitch;
	UINT32 u4ColorMode;
};

struct VDPC_INFO_T
{
    VDP_BUFFER_T rSrcBuffer;
	UINT32 u4SrcClipX;
	UINT32 u4SrcClipY;
	UINT32 u4SrcClipWidth;
	UINT32 u4SrcClipHeight;
    UINT32 u4DisplayX;
	UINT32 u4DisplayY;
	UINT32 u4DisplayWidth;
	UINT32 u4DisplayHeight;
	UINT32 u4ChannelID;//0:main 1:sub
	BOOL fgRun;
};

/* ioctls
   0x53 is 'S'								*/
#define SIMPLE_CONSUME_CMD_START         0x5300
#define SIMPLE_ADD_SIMPLEBUFFER	         0x5300
#define SIMPLE_REMOVE_SIMPLEBUFFER       0x5301
#define SIMPLE_WAIT_SIMPLEBUFFER_REMOVE  0x5302
#define SIMPLE_POLL_CONSUME              0x5303
#define SIMPLE_CONSUME_CMD_END           0x530f
#define SIMPLE_VDP_CMD_START             0x5310
#define SIMPLE_VDP_CONVERT               0x5310
#define SIMPLE_VDP_STOP_CVT              0x5311
#define SIMPLE_VDP_CMD_END               0x531f
#define SIMPLE_TEST_IO                   0x53aa

//extern VOID Mt53Simple_InitDevice(VOID);
#ifdef __KERNEL__
extern INT32 mt53simple_init(void);
extern INT32 mt53simple_ioctl(UINT32 u4Arg);
#endif

#endif
