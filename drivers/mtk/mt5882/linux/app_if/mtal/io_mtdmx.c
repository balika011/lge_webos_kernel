/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * $RCSfile: io_mtdmx.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: wen han $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mtdmx.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "dmx_if.h"
#include "gcpu_if.h"
#include "dmx_mm_if.h"
#include "fbm_drvif.h"
#include "mpv_drvif.h"
#include "vdec_drvif.h"
#include "aud_drvif.h"
#include "stc_drvif.h"
#include "cb_data.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>
#ifdef CC_TRUSTZONE_SUPPORT
#include "tz_if.h"
#endif
#include "x_assert.h"
#include "x_util.h"
#include "x_bim.h"
#include "dmx_drm_drvif.h"
#include "x_hal_arm.h"
#include "x_os.h"

#include "tz_wv_if.h"
#define WV_DEBUG_LOG (0)


extern int mtal_ioctl_mttsrec(unsigned int cmd, unsigned long arg);
#ifdef TIME_SHIFT_SUPPORT
extern int mtal_ioctl_mtdrec(unsigned int cmd, unsigned long arg);
#endif
extern MT_RESULT_T MTDMX_MWIF_Call(unsigned long arg);
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MAX_SECTION_SIZE            32768

#define MAX_PES_SIZE                65536

#define MTAL_DMX_MULTIPLE_INVALID_INSTANCE 0xFF
#define MTAL_DMX_MULTIPLE_INVALID_PIDX     0xFF
#define MTAL_DMX_MULTIPLE_INVALID_FILTERX  0xFF

#define MTAL_DMX_EXPECTED_SIZE              (0x500000)
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    BOOL fgValid;
    DMX_CONN_TYPE_T eConType;
    UINT8 u1VirtualPidx;
} MTAL_DMX_PID_INFO_T;

typedef struct
{
    BOOL fgValid;
    DMX_CONN_TYPE_T eConType;
    UINT8 u1VirtualPidx;
} MTAL_DMX_FILTER_INFO_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
extern BOOL _MTPVR_QueryPlayMode(UCHAR ucPvrId);
extern BOOL _MTPVR_VideoCallback(const DMX_PES_MSG_T* prPes);
extern BOOL _MTPVR_AudioCallback(const DMX_AUDIO_PES_T* prPes);

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define DATASIZE(rp, wp, size)      \
    (((rp) <= (wp)) ? ((wp) - (rp)) : (((wp) + (size)) - (rp)))


#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,   \
                       (size)))                             \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_7ARG(arg)                 \
		if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   sizeof(MTAL_IOCTL_7ARG_T)))				\
		{														\
			printk("mtal_ioctl_mtdmx access argument error\n");	 \
			return MTR_ERR_INV; 										 \
		}														\
	
#define COPY_FROM_USER_7ARG(ArgUsr, ArgKernel)              \
		if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,	\
						   sizeof(MTAL_IOCTL_7ARG_T)))			\
		{														\
			printk("mtal_ioctl_mtdmx copy argument error\n");	  \
			return MTR_ERR_INV; 										 \
		}	  \

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_9ARG(arg)                 \
		if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   sizeof(MTAL_IOCTL_9ARG_T)))				\
		{														\
			printk("mtal_ioctl_mtdmx access argument error\n");	 \
			return MTR_ERR_INV; 										 \
		}														\
	
#define COPY_FROM_USER_9ARG(ArgUsr, ArgKernel)              \
		if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,	\
						   sizeof(MTAL_IOCTL_9ARG_T)))			\
		{														\
			printk("mtal_ioctl_mtdmx copy argument error\n");	  \
			return MTR_ERR_INV; 										 \
		}	  \

#define USR_SPACE_ACCESS_VALIDATE_11ARG(arg)                 \
		if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
					   sizeof(MTAL_IOCTL_11ARG_T)))				\
		{														\
			printk("mtal_ioctl_mtdmx access argument error\n");	 \
			return MTR_ERR_INV; 										 \
		}														\
	
#define COPY_FROM_USER_11ARG(ArgUsr, ArgKernel)              \
		if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,	\
						   sizeof(MTAL_IOCTL_11ARG_T)))			\
		{														\
			printk("mtal_ioctl_mtdmx copy argument error\n");	  \
			return MTR_ERR_INV; 										 \
		}	  \

#define USR_SPACE_ACCESS_VALIDATE_12ARG(arg)                 \
				if (!access_ok(VERIFY_READ, (void __user *)arg, 		\
							   sizeof(MTAL_IOCTL_12ARG_T))) 			\
				{														\
					printk("mtal_ioctl_mtdmx access argument error\n");	 \
					return MTR_ERR_INV; 										 \
				}														\
			
#define COPY_FROM_USER_12ARG(ArgUsr, ArgKernel)              \
				if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,	\
								   sizeof(MTAL_IOCTL_12ARG_T))) 		\
				{														\
					printk("mtal_ioctl_mtdmx copy argument error\n");	  \
					return MTR_ERR_INV; 										 \
				}	  \


#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx access argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx copy argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#ifdef MTDMX_PRINT_SCHAR
#undef MTDMX_PRINT_SCHAR
#endif
#define MTDMX_PRINT_SCHAR(func,str)   do{\
                                            if(IS_MTDMX_LOG_ENABLE)\
                                            {\
                                                UINT8 i=0;\
                                                do\
                                                {\
                                                    printf("%s[%d] = %d, "func,i,str[i]);\
                                                }while(str[i++] != 0);\
                                                printf("\n");\
                                            }\
                                        }while(0)

#ifdef MTDMX_PRINT_CHAR
#undef MTDMX_PRINT_CHAR
#endif
#define MTDMX_PRINT_CHAR(func,str)   do{\
                                            if(IS_MTDMX_LOG_ENABLE)\
                                            {\
                                                UINT8 i=0;\
                                                do\
                                                {\
                                                    printf("%s[%d] = %d, "func,i,str[i]);\
                                                }while(str[i++] != 0);\
                                                printf("");\
                                            }\
                                        }while(0)

#ifdef MTDMX_PRINT_ARRAY
#undef MTDMX_PRINT_ARRAY
#endif
#define MTDMX_PRINT_ARRAY(func,typeinfo,str,length) do{\
                                                            if(IS_MTDMX_LOG_ENABLE)\
                                                            {\
                                                                UINT8 i=0;\
                                                                while(length > i)\
                                                                {\
                                                                    printf("%s%s[%d] = %d\n",func,typeinfo,i,str[i]);\
                                                                    i++;\
                                                                }\
                                                                printf("\n");\
                                                            }\
                                                        }while(0)

#ifdef MTDMX_PRINT_SEC
#undef MTDMX_PRINT_SEC
#endif
#define MTDMX_PRINT_SEC(func ,rsec)  do{\
                                            if(IS_MTDMX_LOG_ENABLE)\
                                            {printf("%s u1TableID = 0x%x,\n",func,(rsec).u1TableID);\
                                                printf("%s u2SecLen = 0x%x,\n",func,(rsec).u2SecLen);\
                                                  printf("%s u1VerNum = 0x%x,\n",func,(rsec).u1VerNum);\
                                                   printf("%s u4CRC = 0x%x\n",func,(rsec).u4CRC);\
                                                }\
                                        }while(0)

#ifdef MTDMX_PID_CNT
#undef MTDMX_PID_CNT
#endif
#define MTDMX_PID_CNT(func,rcnt) do{\
                                        if(IS_MTDMX_LOG_ENABLE)\
                                        {printf("%s u4PesCount = 0x%x,\n",func,(rcnt).u4PesCount);\
                                            printf("%s u4PicCount = 0x%x,\n",func,(rcnt).u4PicCount);\
                                               printf("%s u4SecCount = 0x%x\n",func,(rcnt).u4SecCount);\
                                            }\
                                    }while(0)


#ifdef MTDMX_PRINT_P_PID
#undef  MTDMX_PRINT_P_PID
#endif
#define MTDMX_PRINT_P_PID(str,rpid)    do{\
                                        if(IS_MTDMX_LOG_ENABLE)\
                                        {printf("%s fgEnable = %u,\n",str,(rpid).fgEnable);\
                                            printf("%s fgAllocateBuffer = %u,\n",str,(rpid).fgAllocateBuffer);\
                                              printf("%s u2Pid = 0x%x,\n",str,(rpid).u2Pid);\
                                               printf("%s u4BufAddr = 0x%x,\n",str,(rpid).u4BufAddr);\
                                                 printf("%s u4BufSize = 0x%x,\n",str,(rpid).u4BufSize);\
                                                  printf("%s ePidType = 0x%x,\n",str,(rpid).ePidType);\
                                                   printf("%s pvNotifyTag = 0x%x,\n",str,(UINT32)((rpid).pvNotifyTag));\
                                                    printf("%s pfnScramble = 0x%x,\n",str,(UINT32)(rpid).pfnScramble);\
                                                     printf("%s pvScrambleTag = 0x%x\n",str,(UINT32)((rpid).pvScrambleTag));\
                                            }\
                                        }while(0)

#ifdef MTDMX_PRINT_FTR
#undef MTDMX_PRINT_FTR
#endif
#define MTDMX_PRINT_FTR(func,rftr)    do{\
                                            if(IS_MTDMX_LOG_ENABLE)\
                                            {printf("%s fgEnable = %d,\n",func,(rftr).fgEnable);\
                                                printf("%s fgCheckCrc = %d,\n",func,(rftr).fgCheckCrc);\
                                                  printf("%s u1Pidx = 0x%x,\n",func,(rftr).u1Pidx);\
                                                   printf("%s u1Offset = 0x%x,\n",func,(rftr).u1Offset);\
                                                     MTDMX_PRINT_ARRAY(func,"au1Data", (rftr).au1Data, 16);\
                                                      MTDMX_PRINT_ARRAY(func,"au1Mask", (rftr).au1Mask, 16);\
                                                }\
                                        }while(0)


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

/// Temporary Section buffer
static UINT8 *_pu1SecBuf = NULL;
static HANDLE_T _hSecBufSem = NULL_HANDLE;

static BOOL _afgPidCreated[MTDMX_NUM_PID_INDEX];
static BOOL _afgSectionPid[MTDMX_NUM_PID_INDEX];


static BOOL _fgVdecEs1Used = FALSE;

/// Demux API critical
static BOOL _fgIoDmxLocking = FALSE;
static CRIT_STATE_T _rIoDmxLock;

// stored buffer pointer
static UINT32 u4PsiBufAddr;
static UINT32 u4PesBufAddr;

static BOOL fgFbmDmx1Used = FALSE;
static BOOL fgFbmDmx2Used = FALSE;
static FBM_POOL_T* prFbmPool = NULL;
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

MTAL_DMX_PID_INFO_T _arDmxInfo[DMX_NUM_PID_INDEX];
MTAL_DMX_FILTER_INFO_T _arDmxFilterInfo[DMX_NUM_FILTER_INDEX];

static BOOL _MTAL_DMX_VirtualFilterBimap(DMX_NOTIFY_INFO_PSI_T *prPSIInfo, MTDMX_NOTIFY_INFO_PSI_T *prPSINtyInfo);

//-----------------------------------------------------------------------------
/** Enter demux API critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _IoMtDmx_Lock(void)
{
    _rIoDmxLock = x_crit_start();

    ASSERT(!_fgIoDmxLocking);
    _fgIoDmxLocking = TRUE;

    UNUSED(_fgIoDmxLocking);          // Make Lint happy
}

//-----------------------------------------------------------------------------
/** Leave demux API critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------

static void _IoMtDmx_Unlock(void)
{
    ASSERT(_fgIoDmxLocking);
    _fgIoDmxLocking = FALSE;

    x_crit_end(_rIoDmxLock);

    UNUSED(_fgIoDmxLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL _MtDmxCallback(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
                           UINT32 u4Data, const void* pvNotifyTag)
{
    DMX_NOTIFY_INFO_PSI_T* prPSIInfo;
    DMX_NOTIFY_INFO_PES_T* prPESInfo;
    DMX_NOTIFY_INFO_RAW_TS_T* prRawTsInfo;
    DMX_MTAL_CB_T rCB = {0};/***L5L***/
    UINT8 u1VirPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    MTDMX_NOTIFY_INFO_PSI_T rPSINtyInfo;

    // Check if PID is really active
    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    _IoMtDmx_Lock();
    u1VirPidx = _arDmxInfo[u1Pidx].u1VirtualPidx;
    _IoMtDmx_Unlock();

    if (u1VirPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1VirPidx error\n");
        return FALSE;
    }
    else
    {
        rCB.u1Pidx = u1VirPidx;
    }
    rCB.type = eCode;
    //rCB.u1Pidx = u1Pidx;

    // Check notification code
    switch (eCode)
    {
    case DMX_NOTIFY_CODE_OVERFLOW:
        rCB.u4Data0 = u4Data;
        break;

    case DMX_NOTIFY_CODE_SCRAMBLE_STATE:
        rCB.u4Data0 = u4Data;
        break;
    case DMX_NOTIFY_CODE_RAW_SCRAMBLE_STATE:
        rCB.u4Data0 = u4Data;
        break;
    case DMX_NOTIFY_CODE_PSI:
        prPSIInfo = (DMX_NOTIFY_INFO_PSI_T*)u4Data;
        ASSERT(prPSIInfo != NULL);

        rCB.u4Data0 = prPSIInfo->u1SerialNumber;
        rCB.u4Data1 = prPSIInfo->u4SecAddr;
        rCB.u4Data2 = prPSIInfo->u4SecLen;
        // make klocwork happy
        //x_memcpy(&rCB.u4Data3, prPSIInfo->au4MatchBitmap, SEC_CNT * sizeof(UINT32));
        _MTAL_DMX_VirtualFilterBimap(prPSIInfo, &rPSINtyInfo);
        if (MTSEC_CNT >= 1)
        {
            rCB.u4Data3 = rPSINtyInfo.au4MatchBitmap[0];
        }
        if (MTSEC_CNT >= 2)
        {
            rCB.u4Data4 = rPSINtyInfo.au4MatchBitmap[1];
        }
        if (MTSEC_CNT >= 3)
        {
            rCB.u4Data5 = rPSINtyInfo.au4MatchBitmap[2];
        }
        if (MTSEC_CNT >= 4)
        {
            rCB.u4Data6 = rPSINtyInfo.au4MatchBitmap[3];
        }
        if (MTSEC_CNT >= 5)
        {
            rCB.u4Data7 = rPSINtyInfo.au4MatchBitmap[4];
        }
        if (MTSEC_CNT >= 6)
        {
            rCB.u4Data8 = rPSINtyInfo.au4MatchBitmap[5];
        }
        if (MTSEC_CNT >= 7)
        {
            rCB.u4Data9 = rPSINtyInfo.au4MatchBitmap[6];
        }
        if (MTSEC_CNT >= 8)
        {
            rCB.u4Data10 = rPSINtyInfo.au4MatchBitmap[7];
        }
        break;

    case DMX_NOTIFY_CODE_PES:
        prPESInfo = (DMX_NOTIFY_INFO_PES_T*)u4Data;
        ASSERT(prPESInfo != NULL);

        rCB.u4Data0 = prPESInfo->u1SerialNumber;
        rCB.u4Data1 = prPESInfo->u4DataAddr;
        rCB.u4Data2 = prPESInfo->u4DataSize;
		//some pvr playback,subtile not sync with A/V, this fix it
        rCB.u4Data3 = 0;
        #ifdef CC_DMX_DILIVER_AUDIO_PTS_FOR_PES_NOTIFY
        rCB.u4Data3 = prPESInfo->u4Pts;
        #endif
        break;

    case DMX_NOTIFY_CODE_AV_PES:
        prPESInfo = (DMX_NOTIFY_INFO_PES_T*)u4Data;
        ASSERT(prPESInfo != NULL);

        rCB.u4Data0 = prPESInfo->u1SerialNumber;
        rCB.u4Data1 = prPESInfo->u4DataAddr;
        rCB.u4Data2 = prPESInfo->u4DataSize;
        break;

    case DMX_NOTIFY_CODE_RAW_TS:
        prRawTsInfo = (DMX_NOTIFY_INFO_RAW_TS_T*)u4Data;
        ASSERT(prRawTsInfo != NULL);

        rCB.u4Data0 = prRawTsInfo->u1SerialNumber;
        rCB.u4Data1 = prRawTsInfo->u4DataAddr;
        rCB.u4Data2 = prRawTsInfo->u4DataSize;
        break;

    default:
        // Unexpected notification
        ASSERT(0);
        break;
    }

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_TRIGGER, sizeof (DMX_MTAL_CB_T), &rCB));

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwMPEGVideoCallback
 *  Callback handler of video PES
 *
 *  @param  prPes           PES message
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwMPEGVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rMpvPes;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(VDEC_PES_INFO_T));

    switch (prPes->u4FrameType)
    {
    case PIC_TYPE_I:
        rMpvPes.ucPicType = MPV_I_TYPE;
        break;

    case PIC_TYPE_P:
        rMpvPes.ucPicType = MPV_P_TYPE;
        break;

    case PIC_TYPE_B:
        rMpvPes.ucPicType = MPV_B_TYPE;
        break;

    default:
        return TRUE;
    }

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;
    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;
    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    rMpvPes.fgGop = prPes->fgGop;
    rMpvPes.fgEos = prPes->fgEOS;

    if (prPes->fgEOS)
    {
        MTDMX_PRINT("MPEG-2 VideoCallback EOS.\n");
    }

    return (VDEC_SendEs(&rMpvPes) == 1);
}


//-----------------------------------------------------------------------------
/** _MwH264VideoCallback
 *  Callback handler of video PES
 */
//-----------------------------------------------------------------------------
static BOOL _MwH264VideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T  rMpvPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(VDEC_PES_INFO_T));

    if (!H264_PIC_VALID_TYPE(prPes->u4FrameType))
    {
        return TRUE;
    }
    rMpvPes.ucPicType = prPes->au1PicInfo[0];
    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;

    u4Addr = prPes->u4FrameAddr + 4;
    if (u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rMpvPes.u4VldReadPtr = u4Addr;

    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    //rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    //rMpvPes.fgGop = prPes->fgGop;

    rMpvPes.fgEos = prPes->fgEOS;
    rMpvPes.u1SliceCount = prPes->u1SliceCount;

    if (prPes->fgEOS)
    {
        MTDMX_PRINT("h264 VideoCallback EOS.\n");
    }

    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}


//-----------------------------------------------------------------------------
/** _AVSVideoCallback
 *  Callback handler of video PES
 */
//-----------------------------------------------------------------------------
static BOOL _AVSVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T  rMpvPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(rMpvPes));

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prPes->au1PicInfo;
#else
    u4Addr = prPes->u4FrameAddr + 3;
    if (u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
#endif
    rMpvPes.ucPicType = *(UINT8*)VIRTUAL(u4Addr);

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;

    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;

    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    //rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    //rMpvPes.fgGop = prPes->fgGop;

    rMpvPes.fgEos = prPes->fgEOS;

    if (prPes->fgEOS)
    {
        MTDMX_PRINT("AVS VideoCallback EOS.\n");
    }

    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}


//-----------------------------------------------------------------------------
/** _MwDmxVideoCallback
 *  Callback handler of video PES
 *
 *  @param  prPes           PES message
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxVideoCallback(const DMX_PES_MSG_T* prPes)
{

    switch (prPes->eVideoType)
    {
    case DMX_VIDEO_MPEG:
        return _MwMPEGVideoCallback(prPes);

    case DMX_VIDEO_H264:
        return _MwH264VideoCallback(prPes);

    case DMX_VIDEO_AVS:
        return _AVSVideoCallback(prPes);

    default:
        return FALSE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
/** _MTDMX_SetDecoderEx
 */
//-----------------------------------------------------------------------------
void _MTDMX_SetDecoderEx(UINT8 u1InstId)
{
    DMX_DECODER_CALLBACKS_T rDecoderCallbacks;

    MTDMX_PRINT("dmx video, audio callback setting ------\n");

    // play 192 ts
#ifdef CC_DMX_A1
    if (_MTPVR_QueryPlayMode(0))
#endif
    {
        MTDMX_PRINT("============ callback for Play 192 ===========\n");
        rDecoderCallbacks.pfnSendVideoPes = _MwDmxVideoCallback;
        rDecoderCallbacks.pfnUpdateVideoWp = VDEC_SetWptr;
        rDecoderCallbacks.pfnSendAudioPes = AUD_SendAudioPes;
    }
#ifdef CC_DMX_A1
    else // play 188 ts
    {
        MTDMX_PRINT("============= callback for Play 188 ===========\n");
        rDecoderCallbacks.pfnSendVideoPes = _MTPVR_VideoCallback;
        rDecoderCallbacks.pfnUpdateVideoWp = VDEC_SetWptr;
        rDecoderCallbacks.pfnSendAudioPes = _MTPVR_AudioCallback;
    }
#endif

    DMX_MUL_SetDecoderCallbacks(u1InstId, &rDecoderCallbacks);
}


//-----------------------------------------------------------------------------
/** _MTAL_DMX_PhysicalPidx
 *
 *  @param  u1VirtualPidx          Virtual PID index
 *
 */
//-----------------------------------------------------------------------------
UINT8 _MTAL_DMX_PhysicalPidx(UINT8 u1VirtualPidx)
{
    UINT8 i;

    _IoMtDmx_Lock();
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        if ((_arDmxInfo[i].fgValid == TRUE) && (_arDmxInfo[i].u1VirtualPidx == u1VirtualPidx))
        {
            break;
        }
    }
    _IoMtDmx_Unlock();

    if (i >= DMX_NUM_PID_INDEX)
    {
        return MTAL_DMX_MULTIPLE_INVALID_PIDX;
    }

    return i;
}

//-----------------------------------------------------------------------------
/** _MTAL_DMX_PhysicalFilterx
 *
 *  @param  u1VirtualFilterx          Virtual Filter index
 *
 */
//-----------------------------------------------------------------------------
UINT8 _MTAL_DMX_PhysicalFilterx(UINT8 u1VirFilterx)
{
    UINT8 u1Count;
    
    _IoMtDmx_Lock();
    for (u1Count = 0; u1Count < DMX_NUM_FILTER_INDEX; u1Count++)
    {
        if (_arDmxFilterInfo[u1Count].fgValid && (_arDmxFilterInfo[u1Count].u1VirtualPidx == u1VirFilterx))
        {
            break;
        }
    }
    _IoMtDmx_Unlock();

    if (u1Count >= DMX_NUM_FILTER_INDEX)
    {
        return MTAL_DMX_MULTIPLE_INVALID_FILTERX;
    }
    return u1Count;
}

//-----------------------------------------------------------------------------
/** _MTAL_DMX_VirtualFilterBimap
 *
 *  @param  u1VirtualFilterx          Virtual Filter index
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTAL_DMX_VirtualFilterBimap(DMX_NOTIFY_INFO_PSI_T *prPSIInfo, MTDMX_NOTIFY_INFO_PSI_T *prPSINtyInfo)
{
    UINT8 u1Count;
    UINT8 u4VirFilterx = 0;

    if((prPSIInfo == NULL) || (prPSINtyInfo == NULL))
    {
        return FALSE;
    }

    x_memset((void*)prPSINtyInfo, 0, sizeof(MTDMX_NOTIFY_INFO_PSI_T)); 
    for (u1Count = 0; u1Count < DMX_NUM_FILTER_INDEX; u1Count++)
    {
        if(_arDmxFilterInfo[u1Count].fgValid)
        {
            if((prPSIInfo->au4MatchBitmap[MTSEC_NUM(u1Count)] & (1 << MTFILTERINDEX(u1Count))) != 0)
            {            
                u4VirFilterx = _arDmxFilterInfo[u1Count].u1VirtualPidx;
                if(u4VirFilterx < MTDMX_NUM_EXTEND_FILTER_INDEX)
                {
                    prPSINtyInfo->au4MatchBitmap[MTSEC_NUM(u4VirFilterx)] |= (1 << MTFILTERINDEX(u4VirFilterx));
                }
            }
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MTAL_DMX_FreePhyFilterx
 *
 *  @param  void
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTAL_DMX_InitPhyFilterx(void)
{
    UINT8 u1Count;
    
    _IoMtDmx_Lock();
    for (u1Count = 0; u1Count < DMX_NUM_FILTER_INDEX; u1Count++)
    {
        _arDmxFilterInfo[u1Count].fgValid = FALSE;
    }
    _IoMtDmx_Unlock();

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MTAL_DMX_GetPhyAvailableFilterx
 *
 *  @param  void
 *
 */
//-----------------------------------------------------------------------------
static UINT8 _MTAL_DMX_GetPhyAvailableFilterx(void)
{
    UINT8 u1Count;
    
    _IoMtDmx_Lock();
    for (u1Count = 0; u1Count < DMX_NUM_FILTER_INDEX; u1Count++)
    {
        if (!_arDmxFilterInfo[u1Count].fgValid)
        {
            break;
        }
    }
    if (u1Count >= DMX_NUM_FILTER_INDEX)
    {
        return MTAL_DMX_MULTIPLE_INVALID_FILTERX;
    }

    _arDmxFilterInfo[u1Count].fgValid = TRUE;
    _IoMtDmx_Unlock();

    return u1Count;
}

//-----------------------------------------------------------------------------
/** _MTAL_DMX_FreePhyFilterx
 *
 *  @param  u1PhyPidx   Physics Filter index
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTAL_DMX_FreePhyFilterx(UINT8 u1PhyFilterx)
{
    if(u1PhyFilterx >= DMX_NUM_FILTER_INDEX)
    {
        return FALSE;
    }
    
    _IoMtDmx_Lock();
    _arDmxFilterInfo[u1PhyFilterx].fgValid = FALSE;
    _IoMtDmx_Unlock();
    
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MTAL_DMX_SetVirtualFilterx
 *
 *  @param  u1PhyPidx   Physics PID index
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTAL_DMX_SetVirtualFilterx(UINT8 u1PhyFilterx, UINT8 u1VirFilterx)
{
    if(u1PhyFilterx >= DMX_NUM_FILTER_INDEX)
    {
        return FALSE;
    }
    
    _IoMtDmx_Lock();
    _arDmxFilterInfo[u1PhyFilterx].u1VirtualPidx = u1VirFilterx;
    _IoMtDmx_Unlock();
    
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MtdmxInitConntype
 *
 *  @param  eConnType           connection type
 *  @return  u1Inst             instance ID
 *
 */
//-----------------------------------------------------------------------------
static UINT8 _MtdmxInitConntype(DMX_CONN_TYPE_T eConnType)
{
    UINT8 u1Inst = 0;

    u1Inst = DMX_MUL_GetAvailableInst(eConnType);

    if (u1Inst == MTAL_DMX_MULTIPLE_INVALID_INSTANCE)
    {
        MTDMX_PRINT("Instances are all occupied.\n");
        return u1Inst;
    }

    //just for dtv case
    if (eConnType < DMX_CONN_TYPE_TUNER_MAX)
    {
        if (!DMX_MUL_SetInstType(u1Inst, DMX_IN_BROADCAST_TS))
        {
            MTDMX_PRINT("SetInstType fail.\n");
        }

        if (!DMX_MUL_SetTSIdx(u1Inst))
        {
            MTDMX_PRINT("SetTsIdx fail.\n");
        }

        //set dtv av decoder callback;
        _MTDMX_SetDecoderEx(u1Inst);
    }

    return u1Inst;
}

//-----------------------------------------------------------------------------
/*
 *  _MTDMX_DisConnectInfo
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ConnectInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTDMX_PID_INFO_T    rPidInfo;
    MTDMX_PID_INFO_T*   prPidInfo = NULL;
    UINT8 u1VirtualPidx;
    UINT8 u1PhysicalPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1Inst = 0;
    DMX_CONN_TYPE_T eConnType = DMX_CONN_TYPE_NONE;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1VirtualPidx = (UINT8)rArg.ai4Arg[0];
    prPidInfo = (MTDMX_PID_INFO_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prPidInfo, MTDMX_PID_INFO_T);
    COPY_FROM_USER_ARG(prPidInfo, rPidInfo, MTDMX_PID_INFO_T);

    MTDMX_PRINT(" _MTDMX_ConnectInfo u1Pidx = %d \n",u1VirtualPidx);

    if (u1VirtualPidx >= MTDMX_NUM_PID_INDEX)
    {
        return MTR_NOT_OK;
    }

    eConnType = (DMX_CONN_TYPE_T)rPidInfo.eConType;

    if (eConnType == DMX_CONN_TYPE_NONE)
    {
        return MTR_NOT_OK;
    }

    u1Inst = _MtdmxInitConntype(eConnType);

    if (u1Inst == MTAL_DMX_MULTIPLE_INVALID_INSTANCE)
    {
        MTDMX_PRINT("Instances are all occupied.\n");
        return MTR_NOT_OK;
    }

    u1PhysicalPidx = DMX_MUL_GetAvailablePidx(u1Inst);

    if (u1PhysicalPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Pidx are all occupied.\n");
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    _arDmxInfo[u1PhysicalPidx].fgValid = TRUE;
    _arDmxInfo[u1PhysicalPidx].eConType = (DMX_CONN_TYPE_T)rPidInfo.eConType;
    _arDmxInfo[u1PhysicalPidx].u1VirtualPidx = u1VirtualPidx;
    _IoMtDmx_Unlock();

    return MTR_OK;
}

/*
 *  _MTDMX_ConnectAvailablePidx
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ConnectAvailablePidx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTDMX_PID_INFO_T    rPidInfo;
    MTDMX_PID_INFO_T*   prPidInfo = NULL;
    UINT8 *pu1VirtualPidx;
    UINT8 u1VirtualPidx;
    UINT8 u1PhysicalPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1Inst = 0;
    DMX_CONN_TYPE_T eConnType = DMX_CONN_TYPE_NONE;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    prPidInfo = (MTDMX_PID_INFO_T*)rArg.ai4Arg[0];
    pu1VirtualPidx = (UINT8*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prPidInfo, MTDMX_PID_INFO_T);
    COPY_FROM_USER_ARG(prPidInfo, rPidInfo, MTDMX_PID_INFO_T);

    eConnType = (DMX_CONN_TYPE_T)rPidInfo.eConType;

    if (eConnType == DMX_CONN_TYPE_NONE)
    {
        return MTR_NOT_OK;
    }

    u1Inst = _MtdmxInitConntype(eConnType);

    if (u1Inst == MTAL_DMX_MULTIPLE_INVALID_INSTANCE)
    {
        MTDMX_PRINT("Instances are all occupied.\n");
        return MTR_NOT_OK;
    }

    u1PhysicalPidx = DMX_MUL_GetAvailablePidx(u1Inst);

    if (u1PhysicalPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Pidx are all occupied.\n");
        return MTR_NOT_OK;
    }

    u1VirtualPidx = u1PhysicalPidx + MTDMX_EXTEND_PID_START; 
    
    _IoMtDmx_Lock();
    _arDmxInfo[u1PhysicalPidx].fgValid = TRUE;
    _arDmxInfo[u1PhysicalPidx].eConType = (DMX_CONN_TYPE_T)rPidInfo.eConType;
    _arDmxInfo[u1PhysicalPidx].u1VirtualPidx = u1VirtualPidx;
    _IoMtDmx_Unlock();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1VirtualPidx, UINT8);
    COPY_TO_USER_ARG(pu1VirtualPidx, u1VirtualPidx, UINT8);

    MTDMX_PRINT(" -u1VirtualPidx = %d\n", u1VirtualPidx);
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  _MTDMX_DisConnectInfo
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_DisConnectInfo(unsigned long arg)
{
    INT32 Arg;
    UINT8 u1Pidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1VirtualPidx, i;
    BOOL fgTheLastPidxOfInst = FALSE;
    DMX_CONN_TYPE_T eConnType;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1VirtualPidx = (UINT8)Arg;

    MTDMX_PRINT(" -u1Pidx = %d \n",u1VirtualPidx);

    u1Pidx = _MTAL_DMX_PhysicalPidx(u1VirtualPidx);

    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        MTDMX_PRINT("Pidx are all occupied.\n");
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    // check if this is the last pidx of the inst
    eConnType = _arDmxInfo[u1Pidx].eConType;
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        if ((i != u1Pidx) && (_arDmxInfo[i].eConType == eConnType) &&
                (_arDmxInfo[i].fgValid == TRUE))
        {
            break;
        }
    }

    if (i == DMX_NUM_PID_INDEX)
    {
        fgTheLastPidxOfInst = TRUE;
    }

    _arDmxInfo[u1Pidx].fgValid = FALSE;
    _arDmxInfo[u1Pidx].eConType = DMX_CONN_TYPE_BUFAGENT_0;
    _arDmxInfo[u1Pidx].u1VirtualPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    _IoMtDmx_Unlock();

    UNUSED(DMX_MUL_FreePidx(u1Pidx));

    if (fgTheLastPidxOfInst)
    {
        //just for dtv case, swdmx will free mm inst
        if (eConnType < DMX_CONN_TYPE_TUNER_MAX)
        {
            UINT8 u1Inst;
            u1Inst = DMX_MUL_GetAvailableInst(eConnType);
            UNUSED(DMX_MUL_FreeInst(u1Inst));
        }
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTAL_DMX_IsES1Used
 *
 *  @param  u1VirtualPidx          Virtual PID index
 *
 */
//-----------------------------------------------------------------------------
BOOL _MTAL_DMX_IsES1Used(void)
{
    BOOL fgUsed = FALSE;

    _IoMtDmx_Lock();
    fgUsed = _fgVdecEs1Used;
    _IoMtDmx_Unlock();

    return fgUsed;
}

//-----------------------------------------------------------------------------
/** _MtdmxSetPid
 */
//-----------------------------------------------------------------------------
static BOOL _MtdmxSetPid(UINT8 u1Pidx, UINT32 u4Flags, const DMX_PID_T* prPid)
{
    DMX_PID_T rPid;

    x_memset((void *)&rPid, 0, sizeof(DMX_PID_T));

    rPid = *prPid;

    //if(u4Flags & DMX_PID_FLAG_TS_INDEX)
    //{
    //    rPid.u1TsIndex = DMX_GetFramerIndex();
    //}

    return DMX_SetPid(u1Pidx, u4Flags, &rPid);
}

//-----------------------------------------------------------------------------
/*
 *  Init demux driver
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_Init(unsigned long arg)
{
    UINT32 i;
    UINT8 u1Inst = MTAL_DMX_MULTIPLE_INVALID_INSTANCE;
    BOOL fgAllocateSecBuf = FALSE;

    MTDMX_PRINT(" - void\n");

    if (!DMX_Init())
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    for (i=0; i<MTDMX_NUM_PID_INDEX; i++)
    {
        _afgPidCreated[i] = FALSE;
        _afgSectionPid[i] = FALSE;
    }

    if (!_pu1SecBuf)
    {
        fgAllocateSecBuf = TRUE;
    }
    _IoMtDmx_Unlock();


	if (_hSecBufSem == NULL_HANDLE)
	{
		if (x_sema_create(&_hSecBufSem, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) != OSR_OK)
		{
			MTDMX_PRINT("create _hSecBufSem fail\n");
			return MTR_NOT_OK;
		}
	}

    if (fgAllocateSecBuf)
    {
        _pu1SecBuf = (UINT8 *)x_mem_alloc(MAX_SECTION_SIZE);

        if (!_pu1SecBuf)
        {
            MTDMX_PRINT("alloc temp section buffer error\n");
            return MTR_NOT_OK;
        }
    }

    /*Tuner 0 connection initializaiton for backward compability purpose.
      New MW will initial it by _MTDMX_ConnectInfo()*/
    u1Inst =  _MtdmxInitConntype(DMX_CONN_TYPE_TUNER_0);
    if (u1Inst == MTAL_DMX_MULTIPLE_INVALID_INSTANCE)
    {
        MTDMX_PRINT("Instances are all occupied.\n");
        return MTR_NOT_OK;
    }

    //Free Phy Filter
    _MTAL_DMX_InitPhyFilterx();
    
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Reset demux driver
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_Reset(unsigned long arg)
{
    MTDMX_PRINT(" - void\n");

    if (!DMX_Reset())
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Start demux
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_Start (unsigned long arg)
{
    MTDMX_PRINT(" - void\n");

    if (!DMX_Start())
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Stop demux
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_Stop(unsigned long arg)
{
    MTDMX_PRINT(" - void\n");

    if (!DMX_Stop())
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set Front end type
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetFrontEndEx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1TsIdx;
    DMX_FRONTEND_T eFrontEnd;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    eFrontEnd = (DMX_FRONTEND_T)rArg.ai4Arg[1];

    MTDMX_PRINT(" - Ts: %d eFE: %d\n", u1TsIdx, (UINT32)eFrontEnd);

    if (!DMX_SetFrontEndEx(u1TsIdx, eFrontEnd))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Set Video type by ts_idx
 */
//-----------------------------------------------------------------------------

static MT_RESULT_T _MTDMX_SetVideoTypeEx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1TsIdx;
    DMX_VIDEO_TYPE_T eType;

    UINT8 u1Instance = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    eType = (DMX_VIDEO_TYPE_T)rArg.ai4Arg[1];

    u1Instance = DMX_MUL_GetDtvInstFromTsIdx(u1TsIdx);

    if (!DMX_MUL_SetVideoType(u1Instance, eType))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

static MT_RESULT_T _DMX_SetVideoTypeByPidx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1VirtualPidx;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    DMX_VIDEO_TYPE_T eType;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1VirtualPidx = (UINT8)rArg.ai4Arg[0];
    eType = (DMX_VIDEO_TYPE_T)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1VirtualPidx);
    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_SetVideoTypeByPidx(u1PhyPidx, eType))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set  packet framer size by ts_idx
 */
//-----------------------------------------------------------------------------

static MT_RESULT_T _MTDMX_SetFramerPkt(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1TsIdx;
    UINT8 u1Size;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u1Size = (UINT8)rArg.ai4Arg[1];

    if (!DMX_SetPacketSizeEx(u1TsIdx, u1Size))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Set Video type
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetVideoType(unsigned long arg)
{
    DMX_VIDEO_TYPE_T eType;
    INT32 Arg;
    UINT8 u1Instance = 0;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    MTDMX_PRINT(" - eType: %lu\n", Arg);

    eType = (DMX_VIDEO_TYPE_T)Arg;


    u1Instance = DMX_MUL_GetDtvInstFromTsIdx(0); // this old API always use TS index 0


    if (!DMX_MUL_SetVideoType(u1Instance, eType))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set a PID
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure of the PES
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx ;
    UINT32 u4Flags ;
    MTDMX_PID_T* prPid =NULL;
    MTDMX_PID_T rPid;
    DMX_PID_T rDrvPid;
    //FBM_POOL_T* prFbmPool;
    UINT8 u4AudDeviceID;
    UINT32 au4BufStart[3], au4BufEnd[3];
    UINT32 u4VfifoAddr = 0, u4VfifoSize = 0;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    x_memset((void *)&rPid,0,sizeof(MTDMX_PID_T));
    x_memset((void *)&rDrvPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prPid = (MTDMX_PID_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prPid, MTDMX_PID_T);
    COPY_FROM_USER_ARG(prPid, rPid, MTDMX_PID_T);

    MTDMX_PRINT(" -u1Pidx = %u, u4Flags = 0x%x\n ", u1Pidx, u4Flags);
    MTDMX_PRINT_P_PID("rPid.", rPid);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);
    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

#if 1
    // Only enable/disable PID
    if (u4Flags == MTDMX_PID_FLAG_VALID)
    {
        rDrvPid.fgEnable = rPid.fgEnable;
        if (!_MtdmxSetPid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }

        return MTR_OK;
    }
#endif
    // Get from MW
    rDrvPid.fgEnable = rPid.fgEnable;
    rDrvPid.fgAllocateBuffer = rPid.fgAllocateBuffer;
    rDrvPid.u4BufAddr = rPid.u4BufAddr;
    rDrvPid.u4BufSize = rPid.u4BufSize;
    rDrvPid.u1DeviceId = 0;
    rDrvPid.ePidType = rPid.ePidType;
#ifdef CC_DMX_A1
    rDrvPid.u1ContainAD = (rPid.ePidType == MTDMX_PID_TYPE_ES_AUDIO) ? rPid.u1ContainAD : 0;
#else
    rDrvPid.u1ContainAD = 2;
#endif

    if (rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        // Allocate video buffer from FBM
        if((!fgFbmDmx1Used) && (!fgFbmDmx2Used))
        {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
            prFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
            prFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
            ASSERT(prFbmPool != NULL);
            if (!prFbmPool)
            {
                Printf("Fail to get VFIFO!\n");
                return MTR_NOT_OK;
            }
        }

        if(rPid.u1ChannelId == 0)
        {
        	_IoMtDmx_Lock();
            fgFbmDmx1Used = TRUE;
			_IoMtDmx_Unlock();
        }
        else
        {
        	_IoMtDmx_Lock();
            fgFbmDmx2Used = TRUE;
			_IoMtDmx_Unlock();
        }

        u4VfifoAddr = prFbmPool->u4Addr;
        u4VfifoSize = prFbmPool->u4Size;
        
        ASSERT(u4VfifoAddr != 0);
        if(u4VfifoSize < MTAL_DMX_EXPECTED_SIZE)
        {
            MTDMX_PRINT("DMX buffer size is too smaler: 0x%x!\n", u4VfifoSize);
        }

        // Setup buffer attributes
        rDrvPid.fgAllocateBuffer = FALSE;
#if defined(CC_DMX_A1) || defined(CC_DUAL_TUNER_SUPPORT)

        if (rPid.u1ChannelId == 0)
        {
            rDrvPid.u4BufAddr = u4VfifoAddr;
            rDrvPid.u4BufSize = u4VfifoSize / 2;
        }
        else
        {
            rDrvPid.u4BufAddr = u4VfifoAddr + (u4VfifoSize / 2);
            rDrvPid.u4BufSize = u4VfifoSize / 2;
            Printf("Free VDEC1 for DDI playing 3D content or background recording\n");

            _IoMtDmx_Lock();
            _fgVdecEs1Used = TRUE;
            _IoMtDmx_Unlock();

        }
#else
        rDrvPid.u4BufAddr = u4VfifoAddr;
        rDrvPid.u4BufSize = u4VfifoSize;
#endif
        Printf("Vdec ID=%d,VFifo Start=0x%08X,Vfifo BufferSize=0x%08X.\r\n",rPid.u1ChannelId,rDrvPid.u4BufAddr,
        rDrvPid.u4BufSize);
    }
    else if ((rPid.ePidType == MTDMX_PID_TYPE_ES_AUDIO) && (u4Flags & MTDMX_PID_FLAG_DEVICE_ID))
    {
#ifdef AUD_OLDFIFO_INTF
        if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                           &au4BufEnd[1]) != AUD_OK)
#else
        if ((AUD_GetAudFifo(0, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK) ||
                (AUD_GetAudFifo(1, &au4BufStart[1], &au4BufEnd[1]) != AUD_OK)
                || (AUD_GetAudFifo(2, &au4BufStart[2], &au4BufEnd[2]) != AUD_OK)
           )
#endif
        {
            return MTR_NOT_OK;
        }
        if ((au4BufStart[0] == 0) || (au4BufStart[1] == 0)
                ||  (au4BufStart[2] == 0)
           )
        {
            return MTR_NOT_OK;
        }

        rDrvPid.fgAllocateBuffer = FALSE;
        /*Get the Audio device id from mw*/
        u4AudDeviceID = rPid.u1DeviceId;

#if defined(CC_DMX_A1)
        if(u4AudDeviceID > 2)
        {
            MTDMX_PRINT("The audio device id is wrong %d\n ",rPid.u1DeviceId);
            return MTR_NOT_OK;
        }
#elif !defined(CC_MT5391_AUD_3_DECODER)
        if (u4AudDeviceID >= 2)
        {
            MTDMX_PRINT("The audio device id from mw is wrong %d\n ", rPid.u1DeviceId);
            return FALSE;
        }
#elif defined(CC_MT5391_AUD_MW_AUTO_CTRL)
        if (u4AudDeviceID == 0)
        {
            u4AudDeviceID = 1;
        }

#ifdef CC_DUAL_AUD_DEC_SUPPORT
        //if (u4AudDeviceID == AUD_DEC_SUB)
        {
            u4AudDeviceID = AUD_Check_Dual_DecId(u4AudDeviceID);    
        }
#endif


        if (u4AudDeviceID > 2)
        {
            MTDMX_PRINT("The audio device id from mw is wrong %d\n ",rPid.u1DeviceId);
            return FALSE;
        }
#endif
        rDrvPid.u4BufAddr = au4BufStart[u4AudDeviceID];
        rDrvPid.u4BufSize = au4BufEnd[u4AudDeviceID] - au4BufStart[u4AudDeviceID];
        rDrvPid.u1DeviceId = u4AudDeviceID;
    }

    if ((rPid.pfnNotify != NULL) && (u4Flags & MTDMX_PID_FLAG_CALLBACK))
    {
        rDrvPid.pfnNotify = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnNotify = NULL;
    }

    if ((rPid.pfnScramble != NULL) && (u4Flags & DMX_PID_FLAG_SCRAMBLE_STATE))
    {
        rDrvPid.pfnScramble = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnScramble = NULL;
    }

    rDrvPid.pvNotifyTag = NULL;
    rDrvPid.pvScrambleTag = NULL;
	rDrvPid.u1ChannelId = rPid.u1ChannelId;
    rDrvPid.u1TsIndex = rPid.u1TsIndex;
    u4Flags |= DMX_PID_FLAG_TS_INDEX;

    // Set by driver only
     if (u4Flags & MTDMX_PID_FLAG_PID)
    {
     rDrvPid.u2Pid = rPid.u2Pid;
     rDrvPid.fgPrimary = TRUE;
     rDrvPid.u1KeyIndex = 0x0;
     rDrvPid.u1SteerMode = DMX_STEER_TO_FTUP;
     rDrvPid.ePcrMode = DMX_PCR_MODE_NONE;
     rDrvPid.eDescMode = DMX_DESC_MODE_NONE;
     u4Flags |= (DMX_PID_FLAG_PRIMARY | DMX_PID_FLAG_KEY_INDEX | DMX_PID_FLAG_STEER | 
                    DMX_PID_FLAG_PCR | DMX_PID_FLAG_DESC_MODE);
    }

    // This is config for scramble check case
    if (u4Flags & MTDMX_PID_FLAG_DISABLE_OUTPUT)
    {
        rDrvPid.fgDisableFifoOutput = rPid.fgDisableOutput;
        u4Flags |= DMX_PID_FLAG_DISABLE_OUTPUT;
        if (!DMX_SetScrambleCheckPid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        if (!_MtdmxSetPid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }

#ifdef CC_KR_3DDTV_HARDCODE
    // set another h264 pid by hardcode
    if (rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        UINT8 u1SpecificPidx;
        u1SpecificPidx = 30;  //special case, how to avoid
        rDrvPid.u2Pid = 259;
        rDrvPid.u4BufAddr = u4VfifoAddr + (u4VfifoSize / 2);
        rDrvPid.u4BufSize = u4VfifoSize / 2;
        rDrvPid.u1ChannelId = 1;
        if (!_MtdmxSetPid(u1SpecificPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }
#endif

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set a PID
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure of the PES
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetFilePid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx ;
    UINT32 u4Flags ;
    MTDMX_PID_T* prPid =NULL;
    MTDMX_PID_T rPid;
    DMX_PID_T rDrvPid;
    //FBM_POOL_T* prFbmPool;
    UINT8 u4AudDeviceID;
    UINT32 au4BufStart[3], au4BufEnd[3];
    UINT32 u4VfifoAddr = 0, u4VfifoSize = 0;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    x_memset((void *)&rPid,0,sizeof(MTDMX_PID_T));
    x_memset((void *)&rDrvPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prPid = (MTDMX_PID_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prPid, MTDMX_PID_T);
    COPY_FROM_USER_ARG(prPid, rPid, MTDMX_PID_T);

    MTDMX_PRINT(" -u1Pidx = %u, u4Flags = 0x%x\n ", u1Pidx, u4Flags);
    MTDMX_PRINT_P_PID("rPid.", rPid);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

#if 1
    // Only enable/disable PID
    if (u4Flags == MTDMX_PID_FLAG_VALID)
    {
        rDrvPid.fgEnable = rPid.fgEnable;
        if (!DMX_SetFilePid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }

        return MTR_OK;
    }
#endif
    // Get from MW
    rDrvPid.fgEnable = rPid.fgEnable;
    rDrvPid.fgAllocateBuffer = rPid.fgAllocateBuffer;
    rDrvPid.u2Pid = rPid.u2Pid;
    rDrvPid.u4BufAddr = rPid.u4BufAddr;
    rDrvPid.u4BufSize = rPid.u4BufSize;
    rDrvPid.u1DeviceId = 0;
    rDrvPid.ePidType = rPid.ePidType;

    if (rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        // Allocate video buffer from FBM
        if((!fgFbmDmx1Used) && (!fgFbmDmx2Used))
        {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
            prFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
            prFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
            ASSERT(prFbmPool != NULL);
            if (!prFbmPool)
            {
                Printf("Fail to get VFIFO!\n");
                return MTR_NOT_OK;
            }
        }

        if(rPid.u1ChannelId == 0)
        {
        	_IoMtDmx_Lock();
            fgFbmDmx1Used = TRUE;
			_IoMtDmx_Unlock();
        }
        else
        {
        	_IoMtDmx_Lock();
            fgFbmDmx2Used = TRUE;
			_IoMtDmx_Unlock();
        }

        u4VfifoAddr = prFbmPool->u4Addr;
        u4VfifoSize = prFbmPool->u4Size;
        
        ASSERT(u4VfifoAddr != 0);
        if(u4VfifoSize < MTAL_DMX_EXPECTED_SIZE)
        {
            MTDMX_PRINT("DMX buffer size is too smaler: 0x%x!\n", u4VfifoSize);
        }

        // Setup buffer attributes
        rDrvPid.fgAllocateBuffer = FALSE;
#if defined(CC_DMX_A1) || defined(CC_DUAL_TUNER_SUPPORT)
        if (rPid.u1ChannelId == 0)
        {
            rDrvPid.u4BufAddr = u4VfifoAddr;
            rDrvPid.u4BufSize = u4VfifoSize / 2;
        }
        else
        {
            rDrvPid.u4BufAddr = u4VfifoAddr + (u4VfifoSize / 2);
            rDrvPid.u4BufSize = u4VfifoSize / 2;
            Printf("Free VDEC1 for DDI playing 3D content or background recording\n");

            _IoMtDmx_Lock();
            _fgVdecEs1Used = TRUE;
            _IoMtDmx_Unlock();

        }
#else
        rDrvPid.u4BufAddr = u4VfifoAddr;
        rDrvPid.u4BufSize = u4VfifoSize;
#endif
        //rDrvPid.u1DeviceId = rPid.u1DeviceId;
    }
    else if (rPid.ePidType == MTDMX_PID_TYPE_ES_AUDIO && (u4Flags & MTDMX_PID_FLAG_DEVICE_ID))
    {
#ifdef AUD_OLDFIFO_INTF
        if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                           &au4BufEnd[1]) != AUD_OK)
#else
        if ((AUD_GetAudFifo(0, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK) ||
                (AUD_GetAudFifo(1, &au4BufStart[1], &au4BufEnd[1]) != AUD_OK)
                || (AUD_GetAudFifo(2, &au4BufStart[2], &au4BufEnd[2]) != AUD_OK)
           )
#endif
        {
            return MTR_NOT_OK;
        }
        if ((au4BufStart[0] == 0) || (au4BufStart[1] == 0)
                ||  (au4BufStart[2] == 0)
           )
        {
            return MTR_NOT_OK;
        }

        rDrvPid.fgAllocateBuffer = FALSE;
        /*Get the Audio device id from mw*/
        u4AudDeviceID = rPid.u1DeviceId;

#ifndef CC_MT5391_AUD_3_DECODER
        if (u4AudDeviceID >= 2)
        {
            MTDMX_PRINT("The audio device id from mw is wrong %d\n ", rPid.u1DeviceId);
            return FALSE;
        }
#else
#ifdef CC_MT5391_AUD_MW_AUTO_CTRL
        if (u4AudDeviceID == 0)
        {
            u4AudDeviceID = 1;
        }
        
#ifdef CC_DUAL_AUD_DEC_SUPPORT
        //if (u4AudDeviceID == AUD_DEC_SUB)
        {
            u4AudDeviceID = AUD_Check_Dual_DecId(u4AudDeviceID);    
        }
#endif

        if (u4AudDeviceID > 2)
        {
            MTDMX_PRINT("The audio device id from mw is wrong %d\n ",rPid.u1DeviceId);
            return FALSE;
        }
#endif
#endif

        rDrvPid.u4BufAddr = au4BufStart[u4AudDeviceID];
        rDrvPid.u4BufSize = au4BufEnd[u4AudDeviceID] - au4BufStart[u4AudDeviceID];
        rDrvPid.u1DeviceId = u4AudDeviceID;
    }

    if ((rPid.pfnNotify != NULL) && (u4Flags & MTDMX_PID_FLAG_CALLBACK))
    {
        rDrvPid.pfnNotify = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnNotify = NULL;
    }

    if ((rPid.pfnScramble != NULL) && (u4Flags & DMX_PID_FLAG_SCRAMBLE_STATE))
    {
        rDrvPid.pfnScramble = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnScramble = NULL;
    }

    rDrvPid.pvNotifyTag = NULL;
    rDrvPid.pvScrambleTag = NULL;

    // Set by driver only
    rDrvPid.fgPrimary = TRUE;
    rDrvPid.u1TsIndex = rPid.u1TsIndex;
    rDrvPid.u1KeyIndex = 0x0;
    rDrvPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rDrvPid.ePcrMode = DMX_PCR_MODE_NONE;
    rDrvPid.eDescMode = DMX_DESC_MODE_NONE;
    rDrvPid.u1ChannelId = rPid.u1ChannelId;

    u4Flags |= (DMX_PID_FLAG_PRIMARY | DMX_PID_FLAG_TS_INDEX | DMX_PID_FLAG_KEY_INDEX |
                DMX_PID_FLAG_DEVICE_ID | DMX_PID_FLAG_STEER | DMX_PID_FLAG_PCR |
                DMX_PID_FLAG_DESC_MODE);

    // This is config for scramble check case
    if (u4Flags & MTDMX_PID_FLAG_DISABLE_OUTPUT)
    {
        rDrvPid.fgDisableFifoOutput = rPid.fgDisableOutput;
        u4Flags |= DMX_PID_FLAG_DISABLE_OUTPUT;
        if (!DMX_SetScrambleCheckPid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        if (!DMX_SetFilePid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }

#ifdef CC_KR_3DDTV_HARDCODE
    // set another h264 pid by hardcode
    if (rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        UINT8 u1SpecificPidx;
        u1SpecificPidx = 30;  //special case, how to avoid
        rDrvPid.u2Pid = 259;
        rDrvPid.u4BufAddr = u4VfifoAddr + (u4VfifoSize / 2);
        rDrvPid.u4BufSize = u4VfifoSize / 2;
        rDrvPid.u1ChannelId = 1;
        if (!DMX_SetFilePid(u1SpecificPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }
    }
#endif

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Get a PID
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure of the PES
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetPid (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx;
    UINT32 u4Flags;
    MTDMX_PID_T * prPid = NULL;
    MTDMX_PID_T rPid;
    DMX_PID_T rDrvPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    x_memset(&rDrvPid, 0, sizeof(rDrvPid));
    x_memset((void *)&rPid,0,sizeof(MTDMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prPid = (MTDMX_PID_T*)rArg.ai4Arg[2];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    //This is special for getpid, MW want traverse serch all pidx ,Then in most of
    //case u1PhyPidx is 0xff.
    //so in mach case, we return the right value to mw.
    //in not mach case, we return all zero back to mw. this will need glue layer judge enable status.
    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error, u1Pidx=0x%x\n", u1Pidx);
        //return MTR_NOT_OK;
    }
    else //match case
    {
        if (!DMX_GetPid(u1PhyPidx, u4Flags, &rDrvPid))
        {
            return MTR_NOT_OK;
        }

	    rPid.ePidType = rDrvPid.ePidType;
	    rPid.fgAllocateBuffer = rDrvPid.fgAllocateBuffer;
	    rPid.fgEnable = rDrvPid.fgEnable;
	    rPid.pfnNotify = (PFN_MTDMX_NOTIFY)rDrvPid.pfnNotify;
	    rPid.pfnScramble = (PFN_MTDMX_NOTIFY)rDrvPid.pfnScramble;
	    rPid.pvNotifyTag = rDrvPid.pvNotifyTag;
	    rPid.pvScrambleTag = rDrvPid.pvScrambleTag;
	    rPid.u2Pid = rDrvPid.u2Pid;
	    rPid.u4BufAddr = rDrvPid.u4BufAddr;
	    rPid.u4BufSize = rDrvPid.u4BufSize;
	    rPid.u1TsIndex = rDrvPid.u1TsIndex;
	    rPid.u1DeviceId = rDrvPid.u1DeviceId;
	    rPid.u1ChannelId = rDrvPid.u1ChannelId;
	    rPid.fgDisableOutput = rDrvPid.fgDisableFifoOutput;
		rPid.u1LocalTsIndex = rDrvPid.u1LocalTsIndex;

	    MTDMX_PRINT(" -u1PhyPidx = %u , u4Flags = 0x%x\n ", u1PhyPidx, u4Flags);// log for virtual address
	    MTDMX_PRINT_P_PID("rPid.", rPid);
	}

	USR_SPACE_ACCESS_VALIDATE_ARG(prPid, MTDMX_PID_T);
	COPY_TO_USER_ARG(prPid, rPid, MTDMX_PID_T);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Free a PID
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_FreePid(unsigned long arg)
{
    DMX_PID_T rDrvPid;
    INT32 Arg;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1VirtualPidx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1VirtualPidx = (UINT8)Arg;

    MTDMX_PRINT(" - u1Pidx = %d \n",u1VirtualPidx);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1VirtualPidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_GetPid(u1PhyPidx, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DEVICE_ID, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

#ifndef CC_DMX_A1
    if (rDrvPid.ePidType == DMX_PID_TYPE_ES_AUDIO)
    {
        DMX_TraceLog((UINT32)_MTDMX_FreePid, "_MTDMX_FreePid",
                     (UINT32)AUD_WaitDspFlush, "AUD_WaitDspFlush",
                     DMX_TRACE_FUNCTION_CALL);

        AUD_WaitDspFlush(rDrvPid.u1DeviceId);

        DMX_TraceLog((UINT32)_MTDMX_FreePid, "_MTDMX_FreePid",
                     (UINT32)AUD_WaitDspFlush, "AUD_WaitDspFlush",
                     DMX_TRACE_FUNCTION_RETURN);
    }
#endif

    _IoMtDmx_Lock();
    _afgPidCreated[u1PhyPidx] = FALSE;
    _afgSectionPid[u1PhyPidx] = FALSE;
    _IoMtDmx_Unlock();

    if (!DMX_FreePid(u1PhyPidx))
    {
        return MTR_NOT_OK;
    }

    if(rDrvPid.ePidType == (DMX_PID_TYPE_T)MTDMX_PID_TYPE_ES_VIDEO)
    {
        if(rDrvPid.u1ChannelId == 0)
        {
        	_IoMtDmx_Lock();
            fgFbmDmx1Used = FALSE;
			_IoMtDmx_Unlock();
        }
        else if(rDrvPid.u1ChannelId == 1)
        {
        	_IoMtDmx_Lock();
            fgFbmDmx2Used = FALSE;
			_IoMtDmx_Unlock();
        }

        if((!fgFbmDmx1Used) && (!fgFbmDmx2Used))
        {
			_IoMtDmx_Lock();
            UNUSED(FBM_Free(prFbmPool));
			_IoMtDmx_Unlock();
            prFbmPool = NULL;
        }
    }

#ifdef CC_DMX_A1
    if(rDrvPid.ePidType == (DMX_PID_TYPE_T)MTDMX_PID_TYPE_ES_VIDEO)
    {
        if (rDrvPid.u1ChannelId == 1)
        {
            _IoMtDmx_Lock();
            _fgVdecEs1Used = FALSE;
            _IoMtDmx_Unlock();
        }
    }
#endif

    //UNUSED(_MTAL_DMX_DisConnectInfo(u1VirtualPidx)); //disconnect information

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Find PID index for a given PID value
 *
 *  @param  u2Pid           PID
 *  @param  pu1PidIndex [out]
 *                          The PID index if found
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetPidIndex(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT16 u2Pid;
    UINT8 *pu1PidIndex = NULL;
    UINT8 u1PidIndex;
    UINT8 u1VirPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u2Pid = (UINT16)rArg.ai4Arg[0];
    pu1PidIndex = (UINT8*)rArg.ai4Arg[1];

    if (!DMX_GetPidIndex(u2Pid, &u1PidIndex))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    u1VirPidx = _arDmxInfo[u1PidIndex].u1VirtualPidx;
    _IoMtDmx_Unlock();

    if (u1VirPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1VirPidx error\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1PidIndex, UINT8);
    COPY_TO_USER_ARG(pu1PidIndex, u1VirPidx, UINT8);

    MTDMX_PRINT(" -u2Pid = 0x%x, u1PidIndex = %d\n",u2Pid,u1VirPidx);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Set a Video Pid
 *
 *  @param  u1PidIndex      PID index (0 - 31)
 *  @param  u2PidNum        Video PID number
 *  @param  fgEnable        Enable video pid or not
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetVideoPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx ;
    UINT16 u2PidNum;
    BOOL fgEnable;
    DMX_PID_T rPid;
    //FBM_POOL_T* prFbmPool;
    UINT32 u4VfifoAddr = 0, u4VfifoSize = 0;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    BOOL fgPidCreated = FALSE;
    UINT8 u1Instance;

    x_memset((void *)&rPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u2PidNum = (UINT16)rArg.ai4Arg[1];
    fgEnable = (BOOL)rArg.ai4Arg[2];

    MTDMX_PRINT(" -u1VitruePidx =%d, u2PidNum = 0x%x, fgEnable  =%d\n ",u1Pidx,u2PidNum,fgEnable);


    if (u1Pidx >= MTDMX_NUM_PID_INDEX)
    {
        return MTR_NOT_OK;
    }

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }



    // If callback is used by MM, setup callback again to make sure we have correct callback registration
    u1Instance = DMX_MUL_GetPidxInstId(u1PhyPidx);
    _MTDMX_SetDecoderEx(u1Instance);

    _IoMtDmx_Lock();
    fgPidCreated = _afgPidCreated[u1PhyPidx];
    _IoMtDmx_Unlock();

    if (fgPidCreated)
    {
        rPid.fgEnable = fgEnable;
        if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_VALID, &rPid))
        {
            return MTR_NOT_OK;
        }

        return MTR_OK;
    }

    // Allocate video buffer from FBM
    if((!fgFbmDmx1Used) && (!fgFbmDmx2Used))
    {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
        prFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
        prFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
        ASSERT(prFbmPool != NULL);
        if (!prFbmPool)
        {
            Printf("Fail to get VFIFO!\n");
            return MTR_NOT_OK;
        }
    }

    if(rPid.u1ChannelId == 0)
    {
      	_IoMtDmx_Lock();
        fgFbmDmx1Used = TRUE;
		_IoMtDmx_Unlock();
    }
    else
    {
       	_IoMtDmx_Lock();
        fgFbmDmx2Used = TRUE;
		_IoMtDmx_Unlock();
    }

    u4VfifoAddr = prFbmPool->u4Addr;
    u4VfifoSize = prFbmPool->u4Size;
    
    ASSERT(u4VfifoAddr != 0);
    if(u4VfifoSize < MTAL_DMX_EXPECTED_SIZE)
    {
        MTDMX_PRINT("DMX buffer size is too smaler: 0x%x!\n", u4VfifoSize);
    }

    // Setup buffer attributes
    rPid.fgAllocateBuffer = FALSE;
    rPid.u4BufAddr = u4VfifoAddr;
    rPid.u4BufSize = u4VfifoSize;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rPid.u1TsIndex = 0x0;
    rPid.pfnNotify = NULL;
    rPid.fgPrimary = TRUE;
    rPid.u2Pid = u2PidNum;
    rPid.fgEnable = fgEnable;
    rPid.u1ChannelId = 0x0;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    // Set by driver only
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = 0;
    rPid.u1KeyIndex = 0x0;
    rPid.u1DeviceId = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.eDescMode = DMX_DESC_MODE_NONE;

    if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_ALL, &rPid))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    _afgPidCreated[u1PhyPidx] = TRUE;
    _IoMtDmx_Unlock();

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Set a Audio Pid
 *
 *  @param  u1PidIndex      PID index (0 - 31)
 *  @param  u2PidNum        Video PID number
 *  @param  fgEnable        Enable audio pid or not
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetAudioPid (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx ;
    UINT16 u2PidNum;
    BOOL fgEnable;
    DMX_PID_T rPid;
    UINT32 au4BufStart[3], au4BufEnd[3];
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    BOOL fgPidCreated = FALSE;
    UINT8 u1Instance;
    x_memset((void *)&rPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u2PidNum = (UINT16)rArg.ai4Arg[1];
    fgEnable = (BOOL)rArg.ai4Arg[2];

    MTDMX_PRINT(" -u1ViturePidx =%d, u2PidNum = 0x%x, fgEnable  =%d\n ",u1Pidx,u2PidNum,fgEnable);

    if (u1Pidx >= MTDMX_NUM_PID_INDEX)
    {
        return MTR_NOT_OK;
    }

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }


	// If callback is used by MM, setup callback again to make sure we have correct callback registration
	 u1Instance = DMX_MUL_GetPidxInstId(u1PhyPidx);
	 _MTDMX_SetDecoderEx(u1Instance);
    _IoMtDmx_Lock();
    fgPidCreated = _afgPidCreated[u1PhyPidx];
    _IoMtDmx_Unlock();

    if (fgPidCreated)
    {
        rPid.fgEnable = fgEnable;
        if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_VALID, &rPid))
        {
            return MTR_NOT_OK;
        }

        return MTR_OK;
    }

#ifdef AUD_OLDFIFO_INTF
    if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                       &au4BufEnd[1]) != AUD_OK)
#else
    if ((AUD_GetAudFifo(0, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK) ||
            (AUD_GetAudFifo(1, &au4BufStart[1], &au4BufEnd[1]) != AUD_OK)
            || (AUD_GetAudFifo(2, &au4BufStart[2], &au4BufEnd[2]) != AUD_OK)
       )
#endif
    {
        return MTR_NOT_OK;
    }

    if ((au4BufStart[0] == 0) || (au4BufStart[1] == 0)
            ||  (au4BufStart[2] == 0)
       )
    {
        return MTR_NOT_OK;
    }

    if (GetTargetCountry() == COUNTRY_US)
    {
        rPid.u1DeviceId = 0;        // Audio Device ID
    }
    else
    {
        rPid.u1DeviceId = 1;        // Audio Device ID
    }

    rPid.u4BufAddr = au4BufStart[rPid.u1DeviceId];
    rPid.u4BufSize = au4BufEnd[rPid.u1DeviceId] -
                     au4BufStart[rPid.u1DeviceId];
    rPid.fgAllocateBuffer = FALSE;

    rPid.ePidType = DMX_PID_TYPE_ES_AUDIO;
    rPid.u1TsIndex = 0x0;
    rPid.pfnNotify = NULL;
    rPid.fgPrimary = TRUE;
    rPid.u2Pid = u2PidNum;
    rPid.fgEnable = fgEnable;

    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    // Set by driver only
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = 0;
    rPid.u1KeyIndex = 0x0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.eDescMode = DMX_DESC_MODE_NONE;

    if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_ALL, &rPid))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    _afgPidCreated[u1PhyPidx] = TRUE;
    _IoMtDmx_Unlock();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set a Audio description Pid
 *
 *  @param  u1PidIndex      PID index (0 - 31)
 *  @param  u2PidNum        Video PID number
 *  @param  fgEnable        Enable audio pid or not
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetAudioDescPid (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx ;
    UINT16 u2PidNum;
    BOOL fgEnable;
    DMX_PID_T rPid;
    UINT32 au4BufStart[3], au4BufEnd[3];
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    BOOL fgPidCreated = FALSE;
    UINT8 u1Instance;

    x_memset((void *)&rPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u2PidNum = (UINT16)rArg.ai4Arg[1];
    fgEnable = (BOOL)rArg.ai4Arg[2];

    MTDMX_PRINT(" -u1VirtualPidx =%d, u2PidNum = 0x%x, fgEnable  =%d\n ",u1Pidx,u2PidNum,fgEnable);



    if (u1Pidx >= MTDMX_NUM_PID_INDEX)
    {
        return MTR_NOT_OK;
    }

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }


	// If callback is used by MM, setup callback again to make sure we have correct callback registration
    u1Instance = DMX_MUL_GetPidxInstId(u1PhyPidx);
	 _MTDMX_SetDecoderEx(u1Instance);
    _IoMtDmx_Lock();
    fgPidCreated = _afgPidCreated[fgPidCreated];
    _IoMtDmx_Unlock();

    if (fgPidCreated)
    {
        rPid.fgEnable = fgEnable;
        if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_VALID, &rPid))
        {
            return MTR_NOT_OK;
        }

        return MTR_OK;
    }

#ifdef AUD_OLDFIFO_INTF
    if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                       &au4BufEnd[1]) != AUD_OK)
#else
    if ((AUD_GetAudFifo(0, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK) ||
            (AUD_GetAudFifo(1, &au4BufStart[1], &au4BufEnd[1]) != AUD_OK)
            || (AUD_GetAudFifo(2, &au4BufStart[2], &au4BufEnd[2]) != AUD_OK)
       )
#endif
    {
        return MTR_NOT_OK;
    }
    if ((au4BufStart[0] == 0) || (au4BufStart[1] == 0)
            ||  (au4BufStart[2] == 0)
       )
    {
        return MTR_NOT_OK;
    }

    rPid.u1DeviceId = 2;        // Audio Description Device ID
    rPid.u4BufAddr = au4BufStart[rPid.u1DeviceId];
    rPid.u4BufSize = au4BufEnd[rPid.u1DeviceId] -
                     au4BufStart[rPid.u1DeviceId];
    rPid.fgAllocateBuffer = FALSE;

    rPid.ePidType = DMX_PID_TYPE_ES_AUDIO;
    rPid.u1TsIndex = 0x0;
    rPid.pfnNotify = NULL;
    rPid.fgPrimary = TRUE;
    rPid.u2Pid = u2PidNum;
    rPid.fgEnable = fgEnable;

    // Set by driver only
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = 0;
    rPid.u1KeyIndex = 0x0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.eDescMode = DMX_DESC_MODE_NONE;

    if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_ALL, &rPid))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    _afgPidCreated[u1PhyPidx] = TRUE;
    _IoMtDmx_Unlock();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set a PCR Pid to one PID index
 *
 *  @param  u1TsIdx         TS index (0 - 3)
 *  @param  u1PidIndex      PID index (0 - 31)
 *  @param  u1PidIndex      Stc set (0 - 1)
 *  @param  fgEnable        Enable PCR of the PID or not
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetPcrPidEx  (unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1TsIdx;
    UINT8 u1Pidx;
    UINT8 u1Inst;
    UINT8 u1StcSet;
    UINT16 u2PcrPid;
    DMX_PID_T rDrvPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u1Pidx = (UINT8)rArg.ai4Arg[1];
    u1StcSet = (UINT8)rArg.ai4Arg[2];
    u2PcrPid = (UINT16)rArg.ai4Arg[3];

    MTDMX_PRINT(" - u1VitrualPidx = %d, u2PcrPid = 0x%x, u1StcSet =  0x%x\n", u1Pidx, u2PcrPid, u1StcSet);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    u1Inst = DMX_MUL_GetPidxInstId(u1PhyPidx);
    u1TsIdx = DMX_MUL_GetTSIdx(u1Inst);

    //Printf("_MTDMX_SetPcrPidEx %d\n", u1TsIdx);
    if ((u1TsIdx >= DMX_FRAMER_COUNT) || (u1StcSet >= DMX_NUM_STC_NUM))
    {
        MTDMX_PRINT(" wrong parameter - u1TsIdx = %d, u1StcSet = 0x%x\n", u1TsIdx, u1StcSet);
        return MTR_NOT_OK;
    }

    x_memset((VOID*)&rDrvPid, 0, sizeof(DMX_PID_T));
    rDrvPid.fgEnable = TRUE;
    rDrvPid.fgAllocateBuffer = FALSE;
    rDrvPid.u2Pid = u2PcrPid;
    rDrvPid.ePidType = DMX_PID_TYPE_NONE;
    rDrvPid.fgPrimary = FALSE;
    rDrvPid.u1TsIndex = u1TsIdx;
    rDrvPid.ePcrMode = DMX_PCR_MODE_NEW;
    rDrvPid.eDescMode = DMX_DESC_MODE_NONE;
    rDrvPid.u1DeviceId = u1StcSet;

    if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_ALL & ~DMX_PID_FLAG_BUFFER, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

    if (DMX_SetStcSrc(u1PhyPidx, u1StcSet) != 0)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Set a PCR
 *
 *  @param  u1PidIndex      PID index (0 - 31)
 *  @param  fgEnable        Enable PCR of the PID or not
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetPcr  (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    BOOL fgEnable;
    DMX_PID_T rDrvPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1StcSet;

    x_memset((void *)&rDrvPid,0,sizeof(DMX_PID_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTDMX_PRINT(" - u1VirtualPidx = %d, fgEnable = %d\n",u1Pidx,fgEnable);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    rDrvPid.fgEnable = fgEnable;

    if (!_MtdmxSetPid(u1PhyPidx, DMX_PID_FLAG_VALID, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

    /* get STC Id from device ID */
    if (!DMX_GetPid(u1PhyPidx, DMX_PID_FLAG_DEVICE_ID, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

    u1StcSet = rDrvPid.u1DeviceId;

    if (fgEnable)
    {
        if (DMX_SetStcSrc(u1PhyPidx, u1StcSet) != 0)
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        if (DMX_SetStcSrc(u1PhyPidx, STC_SRC_NS) != 0)   // Set to other PCR to disable
        {
            return MTR_NOT_OK;
        }
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Setup a section filter
 *
 *  @param  u1FilterIndex   Filter index (0 - 31)
 *  @param  u4Flags         Function flags
 *  @param  prFilter        Filter structure
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetFilter (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1FilterIndex;
    UINT32 u4Flags;
    MTDMX_FILTER_T *prFilter =NULL;
    MTDMX_FILTER_T rFilter;
    DMX_FILTER_T rDrvFilter;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    x_memset(&rDrvFilter, 0, sizeof(rDrvFilter));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1FilterIndex = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prFilter = (MTDMX_FILTER_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFilter, MTDMX_FILTER_T);
    COPY_FROM_USER_ARG(prFilter, rFilter, MTDMX_FILTER_T);
    MTDMX_PRINT(" -u1FilterIndex = %u, u4Flags = 0x%x\n ",u1FilterIndex,u4Flags);
    MTDMX_PRINT_FTR("rFilter.", rFilter);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(rFilter.u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    // Driver here
    rDrvFilter.fgEnable = rFilter.fgEnable;
    rDrvFilter.fgCheckCrc = rFilter.fgCheckCrc;
    rDrvFilter.u1Pidx = u1PhyPidx;
    rDrvFilter.u1Offset = rFilter.u1Offset;
    rDrvFilter.au4Data[0] = (rFilter.au1Data[0] << 24) | (rFilter.au1Data[1] << 16) |
                            (rFilter.au1Data[2] << 8) | rFilter.au1Data[3];
    rDrvFilter.au4Data[1] = (rFilter.au1Data[4] << 24) | (rFilter.au1Data[5] << 16) |
                            (rFilter.au1Data[6] << 8) | rFilter.au1Data[7];
    rDrvFilter.au4Mask[0] = (rFilter.au1Mask[0] << 24) | (rFilter.au1Mask[1] << 16) |
                            (rFilter.au1Mask[2] << 8) | rFilter.au1Mask[3];
    rDrvFilter.au4Mask[1] = (rFilter.au1Mask[4] << 24) | (rFilter.au1Mask[5] << 16) |
                            (rFilter.au1Mask[6] << 8) | rFilter.au1Mask[7];


    //rDrvFilter.eMode = DMX_FILTER_MODE_GENERIC;
    rDrvFilter.eMode = DMX_FILTER_MODE_POS_NEG;

    u4Flags |= DMX_FILTER_FLAG_MODE;
    if (!DMX_SetFilter(u1FilterIndex, 0, u4Flags, &rDrvFilter))
    {
        return MTR_NOT_OK;
    }

    rDrvFilter.u1Offset = 1;
    rDrvFilter.au4Data[0] = (rFilter.au1Data[8] << 24) | (rFilter.au1Data[9] << 16) |
                            (rFilter.au1Data[10] << 8) | rFilter.au1Data[11];
    rDrvFilter.au4Data[1] = (rFilter.au1Data[12] << 24) | (rFilter.au1Data[13] << 16) |
                            (rFilter.au1Data[14] << 8) | rFilter.au1Data[15];
    rDrvFilter.au4Mask[0] = (rFilter.au1Mask[8] << 24) | (rFilter.au1Mask[9] << 16) |
                            (rFilter.au1Mask[10] << 8) | rFilter.au1Mask[11];
    rDrvFilter.au4Mask[1] = (rFilter.au1Mask[12] << 24) | (rFilter.au1Mask[13] << 16) |
                            (rFilter.au1Mask[14] << 8) | rFilter.au1Mask[15];
    if (!DMX_SetFilter(u1FilterIndex, 1, u4Flags, &rDrvFilter))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get a section filter
 *
 *  @param  u1FilterIndex   Filter index (0 - 31)
 *  @param  u4Flags         Function flags
 *  @param  prFilter        Filter structure
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetFilter (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1FilterIndex;
    UINT32 u4Flags;
    MTDMX_FILTER_T *prFilter = NULL;
    MTDMX_FILTER_T rFilter;
    DMX_FILTER_T rDrvFilter;
    UINT8 u1VirPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    x_memset(&rDrvFilter, 0, sizeof(rDrvFilter));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1FilterIndex = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prFilter = (MTDMX_FILTER_T*)rArg.ai4Arg[2];

    if (!DMX_GetFilter(u1FilterIndex, 0, u4Flags, &rDrvFilter))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    u1VirPidx = _arDmxInfo[rDrvFilter.u1Pidx].u1VirtualPidx;
    _IoMtDmx_Unlock();

    if (u1VirPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1VirPidx error\n");
    }


    rFilter.fgEnable = rDrvFilter.fgEnable;
    rFilter.fgCheckCrc = rDrvFilter.fgEnable;
    rFilter.u1Pidx = u1VirPidx;
    rFilter.u1Offset = rDrvFilter.u1Offset;
    rFilter.au1Data[0] = (UINT8)((rDrvFilter.au4Data[0] >> 24) & 0xFF);
    rFilter.au1Data[1] = (UINT8)((rDrvFilter.au4Data[0] >> 16) & 0xFF);
    rFilter.au1Data[2] = (UINT8)((rDrvFilter.au4Data[0] >> 8) & 0xFF);
    rFilter.au1Data[3] = (UINT8)(rDrvFilter.au4Data[0] & 0xFF);
    rFilter.au1Data[4] = (UINT8)((rDrvFilter.au4Data[1] >> 24) & 0xFF);
    rFilter.au1Data[5] = (UINT8)((rDrvFilter.au4Data[1] >> 16) & 0xFF);
    rFilter.au1Data[6] = (UINT8)((rDrvFilter.au4Data[1] >> 8) & 0xFF);
    rFilter.au1Data[7] = (UINT8)(rDrvFilter.au4Data[1] & 0xFF);
    rFilter.au1Mask[0] = (UINT8)((rDrvFilter.au4Mask[0] >> 24) & 0xFF);
    rFilter.au1Mask[1] = (UINT8)((rDrvFilter.au4Mask[0] >> 16) & 0xFF);
    rFilter.au1Mask[2] = (UINT8)((rDrvFilter.au4Mask[0] >> 8) & 0xFF);
    rFilter.au1Mask[3] = (UINT8)(rDrvFilter.au4Mask[0] & 0xFF);
    rFilter.au1Mask[4] = (UINT8)((rDrvFilter.au4Mask[1] >> 24) & 0xFF);
    rFilter.au1Mask[5] = (UINT8)((rDrvFilter.au4Mask[1] >> 16) & 0xFF);
    rFilter.au1Mask[6] = (UINT8)((rDrvFilter.au4Mask[1] >> 8) & 0xFF);
    rFilter.au1Mask[7] = (UINT8)(rDrvFilter.au4Mask[1] & 0xFF);

    if (!DMX_GetFilter(u1FilterIndex, 1, u4Flags, &rDrvFilter))
    {
        return MTR_NOT_OK;
    }
    rFilter.au1Data[8] = (UINT8)((rDrvFilter.au4Data[0] >> 24) & 0xFF);
    rFilter.au1Data[9] = (UINT8)((rDrvFilter.au4Data[0] >> 16) & 0xFF);
    rFilter.au1Data[10] = (UINT8)((rDrvFilter.au4Data[0] >> 8) & 0xFF);
    rFilter.au1Data[11] = (UINT8)(rDrvFilter.au4Data[0] & 0xFF);
    rFilter.au1Data[12] = (UINT8)((rDrvFilter.au4Data[1] >> 24) & 0xFF);
    rFilter.au1Data[13] = (UINT8)((rDrvFilter.au4Data[1] >> 16) & 0xFF);
    rFilter.au1Data[14] = (UINT8)((rDrvFilter.au4Data[1] >> 8) & 0xFF);
    rFilter.au1Data[15] = (UINT8)(rDrvFilter.au4Data[1] & 0xFF);
    rFilter.au1Mask[8] = (UINT8)((rDrvFilter.au4Mask[0] >> 24) & 0xFF);
    rFilter.au1Mask[9] = (UINT8)((rDrvFilter.au4Mask[0] >> 16) & 0xFF);
    rFilter.au1Mask[10] = (UINT8)((rDrvFilter.au4Mask[0] >> 8) & 0xFF);
    rFilter.au1Mask[11] = (UINT8)(rDrvFilter.au4Mask[0] & 0xFF);
    rFilter.au1Mask[12] = (UINT8)((rDrvFilter.au4Mask[1] >> 24) & 0xFF);
    rFilter.au1Mask[13] = (UINT8)((rDrvFilter.au4Mask[1] >> 16) & 0xFF);
    rFilter.au1Mask[14] = (UINT8)((rDrvFilter.au4Mask[1] >> 8) & 0xFF);
    rFilter.au1Mask[15] = (UINT8)(rDrvFilter.au4Mask[1] & 0xFF);

    USR_SPACE_ACCESS_VALIDATE_ARG(prFilter, MTDMX_FILTER_T);
    COPY_TO_USER_ARG(prFilter, rFilter, MTDMX_FILTER_T);

    MTDMX_PRINT(" -u1FilterIndex = %u, u4Flags = 0x%x\n ",u1FilterIndex,u4Flags);
    MTDMX_PRINT_FTR("rFilter.", rFilter);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Allocate section buffer pool
 *
 *  @param  u4Size          Buffer size
 *  @param  pu4Addr         Return kernel address
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_AllocateBuffer (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT32 u4Size;
    UINT32 *pu4Addr = NULL;
    UINT32 u4Addr;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4Size = (UINT32)rArg.ai4Arg[0];
    pu4Addr = (UINT32*)rArg.ai4Arg[1];

    u4Addr = (UINT32)x_mem_alloc(u4Size);
    if (u4Addr == 0)
    {
        return MTR_NOT_OK;
    }

    u4PsiBufAddr = u4Addr;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);

    MTDMX_PRINT(" - u4Size = 0x%x, u4Addr = 0x%x\n", u4Size, u4Addr);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Free a section buffer pool
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_FreeBuffer(unsigned long arg)
{
    UINT32 u4Addr;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u4Addr = (UINT32)Arg;

    if (u4Addr == 0)
    {
        return MTR_NOT_OK;
    }

    x_mem_free((VOID*)u4Addr);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Copy data from a section buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4FrameAddr     Frame address (Kernel address)
 *  @param  u4SkipSize      Skip size before copying
 *  @param  u4CopySize      Data size to copy
 *  @param  pu1Dest         Destination buffer (User address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_GetBuffer(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4FrameAddr;
    UINT32 u4SkipSize;
    UINT32 u4CopySize;
    UCHAR* pu1Dest =NULL;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4FrameAddr = (UINT32)rArg.ai4Arg[2];
    u4SkipSize = (UINT32)rArg.ai4Arg[3];
    u4CopySize = (UINT32)rArg.ai4Arg[4];
    pu1Dest = (UCHAR*)rArg.ai4Arg[5];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!_pu1SecBuf)
    {
        MTDMX_PRINT("PSI_GetBuffer: null section buffer\n");
        return MTR_NOT_OK;
    }

    if (u4CopySize > MAX_SECTION_SIZE)
    {
        MTDMX_PRINT("PSI_GetBuffer: invalid size %u\n", u4CopySize);
        return MTR_NOT_OK;
    }

	VERIFY(x_sema_lock(_hSecBufSem, X_SEMA_OPTION_WAIT) == OSR_OK);

    if (!DMX_PSI_GetBuffer(u1PhyPidx, u1SerialNumber, u4FrameAddr,
                          u4SkipSize, u4CopySize, _pu1SecBuf))
    {
		VERIFY(x_sema_unlock(_hSecBufSem) == OSR_OK);
        MTDMX_PRINT("DMX_PSI_GetBuffer fail.\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Dest, u4CopySize);
    COPY_TO_USER_ARG_SIZE(pu1Dest, _pu1SecBuf, u4CopySize);

	VERIFY(x_sema_unlock(_hSecBufSem) == OSR_OK);

    MTDMX_PRINT(" - u1PhyPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4FrameAddr = 0x%x\n"
                "u4SkipSize = 0x%x\n"
                "u4CopySize = 0x%x\n",
                u1PhyPidx,
                u1SerialNumber,
                u4FrameAddr,
                u4SkipSize,
                u4CopySize);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Unlock a section buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4Size          Unlocked data size
 *  @param  u4FrameAddr     Original frame address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_UnlockBuffer(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4Size;
    UINT32 u4FrameAddr;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];
    u4FrameAddr = (UINT32)rArg.ai4Arg[3];

    MTDMX_PRINT(" - u1VirtualPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4Size = 0x%x\n"
                "u4FrameAddr = 0x%x\n",
                u1Pidx,
                u1SerialNumber,
                u4Size,
                u4FrameAddr);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_PSI_UnlockBuffer(u1PhyPidx, u1SerialNumber, u4Size, u4FrameAddr))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Flush a section buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_FlushBuffer (unsigned long arg)
{
    UINT8 u1Pidx;
    INT32 Arg;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    MTDMX_PRINT(" - u1VirtualPidx = %d\n",u1Pidx);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
    }

    //add driver implementation code here
    if (!DMX_PSI_FlushBuffer(u1PhyPidx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get section header and CRC
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  prHeaderCRC     Section header/CRC structure
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PSI_GetBufferSectionHeader(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4FrameAddr;
    UINT32 u4SkipSize;
    UINT32 u4CopySize;
    MTDMX_SECTION_HEADER_T *prHeader = NULL;
    MTDMX_SECTION_HEADER_T rHeader;
    UINT32 u4Offset;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4FrameAddr = (UINT32)rArg.ai4Arg[2];
    u4SkipSize = (UINT32)rArg.ai4Arg[3];
    u4CopySize = (UINT32)rArg.ai4Arg[4];
    prHeader = (MTDMX_SECTION_HEADER_T *)rArg.ai4Arg[5];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!_pu1SecBuf)
    {
        MTDMX_PRINT("PSI_GetBufferSectionHeader: null section buffer\n");
        return MTR_NOT_OK;
    }

    if (u4CopySize > MAX_SECTION_SIZE)
    {
        MTDMX_PRINT("PSI_GetBufferSectionHeader: invalid size %u\n", u4CopySize);
        return MTR_NOT_OK;
    }

	VERIFY(x_sema_lock(_hSecBufSem, X_SEMA_OPTION_WAIT) == OSR_OK);

    if (!DMX_PSI_GetBuffer(u1PhyPidx, u1SerialNumber, u4FrameAddr,
                          u4SkipSize, u4CopySize, _pu1SecBuf))
    {
		VERIFY(x_sema_unlock(_hSecBufSem) == OSR_OK);
        return MTR_NOT_OK;
    }

    rHeader.u1TableID = _pu1SecBuf[0];
    rHeader.u2SecLen = ((_pu1SecBuf[1] & 0xf) << 8) | _pu1SecBuf[2];
    rHeader.u1VerNum = (_pu1SecBuf[5] & 0x3e) >> 1;
    u4Offset = (UINT32)rHeader.u2SecLen + 3;

    if (u4Offset > MAX_SECTION_SIZE)
    {
    	VERIFY(x_sema_unlock(_hSecBufSem) == OSR_OK);
        return MTR_NOT_OK;
    }

    rHeader.u4CRC = (UINT32)_pu1SecBuf[u4Offset - 1] |
                    ((UINT32)_pu1SecBuf[u4Offset - 2] << 8) |
                    ((UINT32)_pu1SecBuf[u4Offset - 3] << 16) |
                    ((UINT32)_pu1SecBuf[u4Offset - 4] << 24);

	VERIFY(x_sema_unlock(_hSecBufSem) == OSR_OK);

    USR_SPACE_ACCESS_VALIDATE_ARG(prHeader, MTDMX_SECTION_HEADER_T);
    COPY_TO_USER_ARG(prHeader, rHeader, MTDMX_SECTION_HEADER_T);

    MTDMX_PRINT(" - u1PhyPidx = %u\n"
                "u1SerialNumber = 0x%x\n",
                u1PhyPidx,
                u1SerialNumber);
    MTDMX_PRINT_SEC("rHeader.",rHeader);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Allocate PES buffer pool
 *
 *  @param  u4Size          Buffer size
 *  @param  pu4Addr         Return allocated buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PES_AllocateBuffer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT32 u4Size;
    UINT32 *pu4Addr = NULL;
    UINT32 u4Addr;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4Size = (UINT32)rArg.ai4Arg[0];
    pu4Addr = (UINT32*)rArg.ai4Arg[1];

    u4Addr = (UINT32)x_mem_alloc(u4Size);
    if (u4Addr == 0)
    {
        return MTR_NOT_OK;
    }

    u4PesBufAddr = u4Addr;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);

    MTDMX_PRINT(" - u4Size = 0x%x, u4Addr = 0x%x\n", u4Size, u4Addr);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Free a PES buffer pool
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PES_FreeBuffer(unsigned long arg)
{
    UINT32 u4Addr;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u4Addr =(UINT32)Arg;

    if (u4Addr == 0)
    {
        return MTR_NOT_OK;
    }

    x_mem_free((VOID*)u4Addr);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Unlock a PES buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4Size          Unlocked data size
 *  @param  u4FrameAddr     Original frame address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PES_UnlockBuffer(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4Size;
    UINT32 u4FrameAddr;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];
    u4FrameAddr = (UINT32)rArg.ai4Arg[3];

    MTDMX_PRINT(" - u1VirtualPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4Size = 0x%x\n"
                "u4FrameAddr = 0x%x\n",
                u1Pidx,
                u1SerialNumber,
                u4Size,
                u4FrameAddr);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_PES_UnlockBuffer(u1PhyPidx, u1SerialNumber, u4Size, u4FrameAddr))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Flush a PES buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PES_FlushBuffer(unsigned long arg)
{
    UINT8 u1Pidx;
    INT32 Arg;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    MTDMX_PRINT(" - u1VirtualPidx = %d\n",u1Pidx);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_PES_FlushBuffer(u1PhyPidx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Copy data from a PES buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4FrameAddr     Frame address (Kernel mode address)
 *  @param  u4SkipSize      Skip size before copying
 *  @param  u4CopySize      Data size to copy
 *  @param  pu1Dest         Destination buffer (User mode address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_PES_GetBuffer (unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4FrameAddr;
    UINT32 u4SkipSize;
    UINT32 u4CopySize;
    UCHAR* pu1Dest =NULL;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT32 u4Addr;
    UCHAR* pu1Tmp =NULL;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4FrameAddr = (UINT32)rArg.ai4Arg[2];
    u4SkipSize = (UINT32)rArg.ai4Arg[3];
    u4CopySize = (UINT32)rArg.ai4Arg[4];
    pu1Dest = (UCHAR*)rArg.ai4Arg[5];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    u4Addr = (UINT32)x_mem_alloc(u4CopySize);
    if (u4Addr == 0)
    {
        MTDMX_PRINT("alloc Pes temp buffer error\n");
        return MTR_NOT_OK;
    }
    pu1Tmp = (UINT8 *)u4Addr;

    if (!DMX_PES_GetBuffer(u1PhyPidx, u1SerialNumber, u4FrameAddr,
                           u4SkipSize, u4CopySize, pu1Tmp))
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Dest, u4CopySize);
    COPY_TO_USER_ARG_SIZE(pu1Dest, pu1Tmp, u4CopySize);

    x_mem_free((VOID*)u4Addr);

    MTDMX_PRINT(" - u1PhyPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4FrameAddr = 0x%x\n"
                "u4SkipSize = 0x%x\n"
                "u4CopySize = 0x%x\n",
                u1PhyPidx,
                u1SerialNumber,
                u4FrameAddr,
                u4SkipSize,
                u4CopySize);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Copy data from a raw TS buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4FrameAddr     Frame address (Kernel mode address)
 *  @param  u4SkipSize      Skip size before copying
 *  @param  u4CopySize      Data size to copy
 *  @param  pu1Dest         Destination buffer (User mode address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_RawTs_GetBuffer (unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4FrameAddr;
    UINT32 u4SkipSize;
    UINT32 u4CopySize;
    UCHAR* pu1Dest =NULL;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UCHAR* pu1Tmp =NULL;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4FrameAddr = (UINT32)rArg.ai4Arg[2];
    u4SkipSize = (UINT32)rArg.ai4Arg[3];
    u4CopySize = (UINT32)rArg.ai4Arg[4];
    pu1Dest = (UCHAR*)rArg.ai4Arg[5];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    pu1Tmp = (UCHAR *)x_mem_alloc(u4CopySize);
    if (!pu1Tmp)
    {
        MTDMX_PRINT("alloc RawTs temp buffer error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_RawTs_GetBuffer(u1PhyPidx, u1SerialNumber, u4FrameAddr,
                             u4SkipSize, u4CopySize, pu1Tmp))
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Dest, u4CopySize);
    COPY_TO_USER_ARG_SIZE(pu1Dest, pu1Tmp, u4CopySize);

    x_mem_free((VOID*)pu1Tmp);

    MTDMX_PRINT(" - u1PhyPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4FrameAddr = 0x%x\n"
                "u4SkipSize = 0x%x\n"
                "u4CopySize = 0x%x\n",
                u1PhyPidx,
                u1SerialNumber,
                u4FrameAddr,
                u4SkipSize,
                u4CopySize);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Unlock a raw TS buffer
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  u1SerialNumber  The serial number
 *  @param  u4Size          Unlocked data size
 *  @param  u4FrameAddr     Original frame address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_RawTs_UnlockBuffer(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1Pidx;
    UINT8 u1SerialNumber;
    UINT32 u4Size;
    UINT32 u4FrameAddr;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u1SerialNumber = (UINT8)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];
    u4FrameAddr = (UINT32)rArg.ai4Arg[3];

    MTDMX_PRINT(" - u1VirtualPidx = %u\n"
                "u1SerialNumber = 0x%x\n"
                "u4Size = 0x%x\n"
                "u4FrameAddr = 0x%x\n",
                u1Pidx,
                u1SerialNumber,
                u4Size,
                u4FrameAddr);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_RawTs_UnlockBuffer(u1PhyPidx, u1SerialNumber, u4Size, u4FrameAddr))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get the counter structure of a given pid index
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @param  prCounters [output]
 *                          Pointer to the PID counter structure
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetPidCounters (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx;
    MTDMX_PID_COUNTERS_T * prCounters =NULL;
    MTDMX_PID_COUNTERS_T rCounters;
    DMX_PID_COUNTERS_T rDrvCounters;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    prCounters = (MTDMX_PID_COUNTERS_T*)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_GetPidCounters(u1PhyPidx, &rDrvCounters))
    {
        return MTR_NOT_OK;
    }

    rCounters.u4PesCount = rDrvCounters.u4PesCount;
    rCounters.u4PicCount = rDrvCounters.u4PicCount;
    rCounters.u4SecCount = rDrvCounters.u4SecCount;

    USR_SPACE_ACCESS_VALIDATE_ARG(prCounters, MTDMX_PID_COUNTERS_T);
    COPY_TO_USER_ARG(prCounters, rCounters, MTDMX_PID_COUNTERS_T);

    MTDMX_PRINT(" -u1PhyPidx = %u ", u1PhyPidx);
    MTDMX_PID_CNT("rCounters.", rCounters);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTDMX_ResetPidCounters
 *  Reset counters of a given pid index
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ResetPidCounters (unsigned long arg)
{
    UINT8 u1Pidx;
    INT32 Arg;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    MTDMX_PRINT(" -u1VirtualPidx = %u \n",u1Pidx);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_ResetPidCounters(u1PhyPidx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get lock state
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetLockState(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1TsIdx;
    BOOL *pfgLock = NULL;
    BOOL fgLock;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    pfgLock = (BOOL*)rArg.ai4Arg[1];

    fgLock = DMX_GetLockState(u1TsIdx);

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgLock, BOOL);
    COPY_TO_USER_ARG(pfgLock, fgLock, BOOL);

    MTDMX_PRINT("-- Tsidx : %d, -fgLock = %d\n", u1TsIdx, fgLock);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get scramble state
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @return     The scramble state
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetScrambleState(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    MTDMX_SCRAMBLE_STATE_T* peScramble_State = NULL;
    MTDMX_SCRAMBLE_STATE_T eScramble_State;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    peScramble_State = (MTDMX_SCRAMBLE_STATE_T*)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    eScramble_State = (MTDMX_SCRAMBLE_STATE_T)DMX_GetScrambleState(u1PhyPidx);

    USR_SPACE_ACCESS_VALIDATE_ARG(peScramble_State, MTDMX_SCRAMBLE_STATE_T);
    COPY_TO_USER_ARG(peScramble_State, eScramble_State, MTDMX_SCRAMBLE_STATE_T);

    MTDMX_PRINT(" -u1PhyPidx = %d, eScramble_State = %d\n",u1PhyPidx,eScramble_State);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get raw scramble state
 *
 *  @param  u1Pidx          PID index, 0 ~ 31
 *  @return     The scramble state
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetRawScrambleState(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    MTDMX_SCRAMBLE_STATE_T* peScramble_State = NULL;
    MTDMX_SCRAMBLE_STATE_T eScramble_State;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    peScramble_State = (MTDMX_SCRAMBLE_STATE_T*)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    eScramble_State = (MTDMX_SCRAMBLE_STATE_T)DMX_GetRawScrambleState(u1PhyPidx);

    USR_SPACE_ACCESS_VALIDATE_ARG(peScramble_State, MTDMX_SCRAMBLE_STATE_T);
    COPY_TO_USER_ARG(peScramble_State, eScramble_State, MTDMX_SCRAMBLE_STATE_T);

    MTDMX_PRINT(" -u1PhyPidx = %d, RawScramble_State = %d\n",u1PhyPidx,eScramble_State);
    return MTR_OK;
}



//-----------------------------------------------------------------------------
/*
 *  Get current STC
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetCurSTC(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    STC_CLOCK rClk;
    UINT32 u4StcHi, u4StcLo;
    UINT32 *pu4StcHi, *pu4StcLo;
    UINT8 u1StcSet;
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1StcSet = (UINT8)rArg.ai4Arg[0];
    pu4StcHi = (UINT32*)rArg.ai4Arg[1];
    pu4StcLo = (UINT32*)rArg.ai4Arg[2];

    if (u1StcSet >= DMX_NUM_STC_NUM)
    {
        return MTR_NOT_OK;
    }

    if (STC_GetSrc(u1StcSet, &rClk) != 0)
    {
        return MTR_NOT_OK;
    }

    u4StcLo = rClk.u4Base;
    u4StcHi = (UINT32)rClk.ucBaseHi | ((UINT32)rClk.u2Ext << 1);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4StcHi, UINT32);
    COPY_TO_USER_ARG(pu4StcHi, u4StcHi, UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4StcLo, UINT32);
    COPY_TO_USER_ARG(pu4StcLo, u4StcLo, UINT32);

    return MTR_OK;
} 
        
static MT_RESULT_T _MTDMX_GetStcId(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1EsId, u1StcId = 0, u1TsIdx;
    UINT8* pu1StcId;
    INT32 i;
    DMX_PID_T rPid;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u1EsId = (UINT8)rArg.ai4Arg[1];
    pu1StcId = (UINT8*)rArg.ai4Arg[2];

    if (!pu1StcId)
    {
        return MTR_NOT_OK;
    }

    if (u1TsIdx == 0xFF)
    {
        for (i = 0; i < DMX_NUM_PID_INDEX; i++)
        {
            if (!DMX_GetPid(i, DMX_PID_FLAG_VALID | DMX_PID_FLAG_TS_INDEX | DMX_PID_FLAG_BUFFER, &rPid))
            {
                MTDMX_PRINT("GetPid error\n");
                return MTR_NOT_OK;
            }

            if (rPid.fgEnable && rPid.u1ChannelId == u1EsId)
            {
                u1TsIdx = rPid.u1TsIndex;
                break;
            }
        }

        if (i >= DMX_NUM_PID_INDEX)
        {
            MTDMX_PRINT("Not found VDEC%d\n", u1EsId);
            return MTR_NOT_OK;
        }
    }

    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        if (!DMX_GetPid(i, DMX_PID_FLAG_VALID | DMX_PID_FLAG_TS_INDEX | DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DEVICE_ID, &rPid))
        {
            MTDMX_PRINT("GetPid error\n");
            return MTR_NOT_OK;
        }

        if (rPid.fgEnable && rPid.u1TsIndex == u1TsIdx && rPid.ePidType == DMX_PID_TYPE_NONE)
        {
            u1StcId = rPid.u1DeviceId;
            break;
        }
    }
    
    if (i >= DMX_NUM_PID_INDEX)
    {
        MTDMX_PRINT("Not found PCR_PID on TS%d\n", u1TsIdx);
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1StcId, UINT8);
    COPY_TO_USER_ARG(pu1StcId, u1StcId, UINT8);

    MTDMX_PRINT("u1EsId = %d , u1StcId = %d.\n", u1EsId, u1StcId);
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Set generic section filter
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetGenericFilter(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 i, u1VirFilterx, u1PhyFilterx;
    UINT32 u4Flags;
    MTDMX_GENERIC_FILTER_T *prFilter =NULL;
    MTDMX_GENERIC_FILTER_T rFilter;
    DMX_FILTER_GENERIC_T rDrvFilter;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    #ifdef CC_MT5399
    DMX_PID_T rDrvPid;
    #endif

    x_memset(&rDrvFilter, 0, sizeof(rDrvFilter));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1VirFilterx = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prFilter = (MTDMX_GENERIC_FILTER_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFilter, MTDMX_GENERIC_FILTER_T);
    COPY_FROM_USER_ARG(prFilter, rFilter, MTDMX_GENERIC_FILTER_T);
    MTDMX_PRINT(" -u1FilterIndex = %u, u4Flags = 0x%x\n ",u1VirFilterx,u4Flags);
    MTDMX_PRINT_FTR("rFilter.", rFilter);

    //Virtual Filterx Convert to Physics
    u1PhyFilterx = _MTAL_DMX_PhysicalFilterx(u1VirFilterx);
    if(u1PhyFilterx == MTAL_DMX_MULTIPLE_INVALID_FILTERX)
    {
        MTDMX_PRINT("Get Phy Filterx error\n");
        return MTR_NOT_OK;
    }
    MTDMX_PRINT(" -u1PhyFilterx = %u\n ",u1PhyFilterx);

#ifdef CC_MT5399
    //find phypidx according vitual pidx
    u1PhyPidx = _MTAL_DMX_PhysicalPidx(rFilter.u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        rFilter.u1Pidx = u1PhyPidx;
    }

    //Only mt5399 need set tsindx
    x_memset(&rDrvPid, 0, sizeof(rDrvPid));

    //Get Ts index
    if (!DMX_GetPid(u1PhyPidx, DMX_PID_FLAG_TS_INDEX, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

    rDrvFilter.u1Tsidx = rDrvPid.u1TsIndex;
    //rvFilter.u1Tsidx = DMX_MUL_GetTsIdx(DMX_MUL_GetPidxInstId(rDrvFilter.u1Pidx));
#else
    //if don't set pidx ,so we need not set virtual and physical pidx
    if ((u4Flags & DMX_FILTER_FLAG_PIDX) != 0)
    {
        u1PhyPidx = _MTAL_DMX_PhysicalPidx(rFilter.u1Pidx);

        if(u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
        {
            MTDMX_PRINT("Get u1PhyPidx error\n");
            return MTR_NOT_OK;
        }
        else
        {
            rFilter.u1Pidx = u1PhyPidx ;
        }
    }
#endif

    // Driver here
    rDrvFilter.fgEnable = rFilter.fgEnable;
    rDrvFilter.fgCheckCrc = rFilter.fgCheckCrc;
    rDrvFilter.u1Pidx = rFilter.u1Pidx;
    rDrvFilter.u1Offset = rFilter.u1Offset;

    for (i=0; i<16; i++)
    {
        rDrvFilter.au1Data[i] = rFilter.au1Data[i];
        rDrvFilter.au1Mask[i] = rFilter.au1Mask[i];
        rDrvFilter.au1PosNeg[i] = rFilter.au1PosNeg[i];
    }

    if (!DMX_SetGenFilter(u1PhyFilterx, u4Flags, &rDrvFilter))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  Get generic section filter setting
 *
 *  @param  u1FilterIndex   Filter index (0 - 31)
 *  @param  u4Flags         Function flags
 *  @param  prFilter        Filter structure
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetGenFilter (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1VirFilterx, u1PhyFilterx, i;
    UINT32 u4Flags;
    MTDMX_GENERIC_FILTER_T *prFilter = NULL;
    MTDMX_GENERIC_FILTER_T rFilter;
    DMX_FILTER_GENERIC_T rDrvFilter;
    UINT8 u1VirPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;
    #ifdef CC_MT5399
    DMX_PID_T rDrvPid;
    #endif

    x_memset(&rFilter, 0, sizeof(rFilter));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    //Virtual Filterx Convert to Physics
    u1VirFilterx = (UINT8)rArg.ai4Arg[0];
    u1PhyFilterx = _MTAL_DMX_PhysicalFilterx(u1VirFilterx);
    if(u1PhyFilterx == MTAL_DMX_MULTIPLE_INVALID_FILTERX)
    {
        MTDMX_PRINT("Get Phy Filterx error\n");
        return MTR_NOT_OK;
    }
    MTDMX_PRINT(" -u1PhyFilterx = %u\n ",u1VirFilterx);
    
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prFilter = (MTDMX_GENERIC_FILTER_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFilter, MTDMX_GENERIC_FILTER_T);
    COPY_FROM_USER_ARG(prFilter, rFilter, MTDMX_GENERIC_FILTER_T);


    //find phy pidx according virtual pidx
    u1PhyPidx = _MTAL_DMX_PhysicalPidx(rFilter.u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        rDrvFilter.u1Pidx = u1PhyPidx ;
    }

    //Only mt5399 need set tsindx
    #ifdef CC_MT5399
    x_memset(&rDrvPid, 0, sizeof(rDrvPid));

    //Get Ts index
    if (!DMX_GetPid(u1PhyPidx, DMX_PID_FLAG_TS_INDEX, &rDrvPid))
    {
        return MTR_NOT_OK;
    }

    rDrvFilter.u1Tsidx = rDrvPid.u1TsIndex;
    #endif

    if (!DMX_GetGenFilter(u1PhyFilterx, u4Flags, (DMX_FILTER_GENERIC_T*)&rDrvFilter))
    {
        return MTR_NOT_OK;
    }

    _IoMtDmx_Lock();
    u1VirPidx = _arDmxInfo[rDrvFilter.u1Pidx].u1VirtualPidx;
    _IoMtDmx_Unlock();

    if (u1VirPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1VirPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        rFilter.u1Pidx = u1VirPidx;
    }

    //
    rFilter.fgEnable= rDrvFilter.fgEnable;
    rFilter.fgCheckCrc = rDrvFilter.fgCheckCrc;
    rFilter.u1Offset = rDrvFilter.u1Offset;

    for (i=0; i<16; i++)
    {
         rFilter.au1Data[i]= rDrvFilter.au1Data[i];
         rFilter.au1Mask[i] = rDrvFilter.au1Mask[i];
         rFilter.au1PosNeg[i] = rDrvFilter.au1PosNeg[i];
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prFilter, MTDMX_GENERIC_FILTER_T);
    COPY_TO_USER_ARG(prFilter, rFilter, MTDMX_GENERIC_FILTER_T);

    MTDMX_PRINT(" -u1PhyFilterx = %u, u4Flags = 0x%x\n ",u1PhyFilterx,u4Flags);
    MTDMX_PRINT_FTR("rFilter.", rFilter);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  _MTDMX_ConnectFilter
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ConnectFilter(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1VirFilterx;
    UINT8 u1PhyFilterx = MTAL_DMX_MULTIPLE_INVALID_FILTERX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1VirFilterx = (UINT8)rArg.ai4Arg[0];

    u1PhyFilterx = _MTAL_DMX_GetPhyAvailableFilterx();
    if (u1PhyFilterx == MTAL_DMX_MULTIPLE_INVALID_FILTERX)
    {
        MTDMX_PRINT("Filterx are all occupied.\n");
        return MTR_NOT_OK;
    }

    _MTAL_DMX_SetVirtualFilterx(u1PhyFilterx, u1VirFilterx);

    MTDMX_PRINT(" -u1PhyFilterx = %d, u1VirFilterx = %d\n", u1PhyFilterx, u1VirFilterx);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/*
 *  _MTDMX_ConnectAvailableFilterx
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ConnectAvailableFilterx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 *pu1VirFilterx;
    UINT8 u1VirFilterx;
    UINT8 u1PhyFilterx = MTAL_DMX_MULTIPLE_INVALID_FILTERX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    pu1VirFilterx = (UINT8*)rArg.ai4Arg[1];

    u1PhyFilterx = _MTAL_DMX_GetPhyAvailableFilterx();
    if (u1PhyFilterx == MTAL_DMX_MULTIPLE_INVALID_FILTERX)
    {
        MTDMX_PRINT("Filterx are all occupied.\n");
        return MTR_NOT_OK;
    }

    u1VirFilterx = u1PhyFilterx + MTDMX_EXTEND_FILTER_START;

    _MTAL_DMX_SetVirtualFilterx(u1PhyFilterx, u1VirFilterx);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1VirFilterx, UINT8);
    COPY_TO_USER_ARG(pu1VirFilterx, u1VirFilterx, UINT8);

    MTDMX_PRINT(" -u1PhyFilterx = %d, u1VirFilterx = %d\n", u1PhyFilterx, u1VirFilterx);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  _MTDMX_DisConnectFilter
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_DisConnectFilter(unsigned long arg)
{
    INT32 Arg;
    UINT8 u1VirFilterx, u1PhyFilterx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1VirFilterx = (UINT8)Arg;

    //Virtual Filterx Convert to Physics
    u1PhyFilterx = _MTAL_DMX_PhysicalFilterx(u1VirFilterx);
    if(u1PhyFilterx == MTAL_DMX_MULTIPLE_INVALID_FILTERX)
    {
        MTDMX_PRINT("Get Phy Filterx error\n");
        return MTR_NOT_OK;
    }
    
    _MTAL_DMX_FreePhyFilterx(u1PhyFilterx);

    MTDMX_PRINT(" -u1Filterx = %d \n",u1VirFilterx);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set DES desrambler key
 *
 *  @param  u1Index         The key index
 *  @param  fg3Des          TRUE: 3DES, FALSE: DES
 *  @param  fgEven          TRUE: even key, FALSE: odd key
 *  @param  au4Key          The key
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetDesKey(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1KeyIdx;
    BOOL fg3Des;
    BOOL fgEven;
    UINT32* pu4Key;
    UINT32 au4Key[6];

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1KeyIdx = (UINT8)rArg.ai4Arg[0];
    fg3Des = (BOOL)rArg.ai4Arg[1];
    fgEven = (BOOL)rArg.ai4Arg[2];
    pu4Key = (UINT32*)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Key, 24);
    COPY_FROM_USER_ARG_SIZE(pu4Key, au4Key, 24);

    MTDMX_PRINT(" -u1Index=%d, fg3Des=%d, fgEven=%d\n", u1KeyIdx, (int)fg3Des, (int)fgEven);

    if (!DMX_SetDesKey(u1KeyIdx, fg3Des, fgEven, au4Key))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** Set the initial vector of DES
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetDesOddEvenIV(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1Idx;
    UINT32 u4IvHi;
    UINT32 u4IvLo;
    BOOL fgEven;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    u4IvHi = (UINT32)rArg.ai4Arg[2];
    u4IvLo = (UINT32)rArg.ai4Arg[3];

    MTDMX_PRINT(" -u4IvHi=0x%x, u4IvLo=0x%x\n", (unsigned int)u4IvHi,
                (unsigned int)u4IvLo);

    DMX_SetDesIV(u1Idx, u4IvHi, u4IvLo, fgEven);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set RTB mode of DES
 *
 *  @param  eMode          MTDMX_DESC_RTB_MODE_T
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetRtbMode(unsigned long arg)
{
    UINT8 u1Idx;
    DMX_DESC_RTB_MODE_T eMode;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    eMode = (DMX_DESC_RTB_MODE_T)rArg.ai4Arg[1];

    MTDMX_PRINT(" - u1Pidx = %d \n", eMode);

    DMX_SetRtbMode(u1Idx, eMode);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set AES key len
 *
 *  @param  u4KeyLen        The key length
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetAesKeyLen(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1Idx;
    UINT32 u4KeyLen;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    u4KeyLen = (UINT32)rArg.ai4Arg[1];

    MTDMX_PRINT(" - KeyLen=%d\n", (int)u4KeyLen);

    DMX_SetAesKeyLen(u1Idx, u4KeyLen);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set AES key
 *
 *  @param  u1Index         The key index
 *  @param  fgEven          If fgEven=TRUE means set even key
 *  @param  au4Key          The key value
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetAesKey(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1KeyIdx;
    BOOL fgEven;
    UINT32* pu4Key;
    UINT32 au4Key[8];

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1KeyIdx = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    pu4Key = (UINT32*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Key, 32);
    COPY_FROM_USER_ARG_SIZE(pu4Key, au4Key, 32);

    MTDMX_PRINT(" - u1Index=%d, fgEven=%d\n", (int)u1KeyIdx, (int)fgEven);

    if (!DMX_SetAesKey(u1KeyIdx, fgEven, au4Key))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** Set the odd/even initial vector
 *
 *  @param  au4Iv           The initial vector
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetAesOddEvenIV(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T  rArg;
    UINT8 u1Idx;
    BOOL fgEven;
    UINT32* pu4Iv;
    UINT32 au4Iv[4];

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    pu4Iv = (UINT32*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Iv, 16);
    COPY_FROM_USER_ARG_SIZE(pu4Iv, au4Iv, 16);

    DMX_SetAesIV(u1Idx, au4Iv, fgEven);

    UNUSED(fgEven);
    UNUSED(au4Iv);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** Set the initial vector of AES
 *
 *  @param  u1Pidx           PID index value
 *  @param  eDescMode        Descramble mode
 *  @param  u1KyeIndex       Key index value
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetDescramblPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx;
    MTDMX_DESC_MODE_T eDescMode;
    UINT8 u1KeyIndex;
    DMX_PID_T rPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    eDescMode = (MTDMX_DESC_MODE_T)rArg.ai4Arg[1];
    u1KeyIndex = (UINT8)rArg.ai4Arg[2];

    MTDMX_PRINT(" - u1VirtualPidx=%d, eDescMode=%d, u1KeyIndex=%d\n",
                (int)u1Pidx, (int)eDescMode, (int)u1KeyIndex);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    rPid.eDescMode = (DMX_DESC_MODE_T)eDescMode;
    rPid.u1KeyIndex = u1KeyIndex;
    if (!DMX_SetPid(u1PhyPidx, DMX_PID_FLAG_DESC_MODE | DMX_PID_FLAG_KEY_INDEX, &rPid))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** Set the initial vector of AES
 *
 *  @param  u1Pidx           PID index value
 *  @param  eDescMode        Descramble mode
 *  @param  u1KyeIndex       Key index value
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetFileDescramblPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx;
    MTDMX_DESC_MODE_T eDescMode;
    UINT8 u1KeyIndex;
    DMX_PID_T rPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    eDescMode = (MTDMX_DESC_MODE_T)rArg.ai4Arg[1];
    u1KeyIndex = (UINT8)rArg.ai4Arg[2];

    MTDMX_PRINT(" - u1VirtualPidx=%d, eDescMode=%d, u1KeyIndex=%d\n",
                (int)u1Pidx, (int)eDescMode, (int)u1KeyIndex);

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    rPid.eDescMode = (DMX_DESC_MODE_T)eDescMode;
    rPid.u1KeyIndex = u1KeyIndex;
    if (!DMX_SetFilePid(u1PhyPidx, DMX_PID_FLAG_DESC_MODE | DMX_PID_FLAG_KEY_INDEX, &rPid))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetDVBKey
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetDVBKey(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1Index;
    BOOL fgEven;
    UINT32 *pu4Key;
    UINT32 au4Key[2];

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    pu4Key = (UINT32*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Key, 8);
    COPY_FROM_USER_ARG_SIZE(pu4Key, au4Key, 8);

    if (!DMX_SetDVBKey(u1Index, fgEven, au4Key))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set the initial vector of DVB
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetDVBIV(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT8 u1Idx;
    UINT32 u4IvHi;
    UINT32 u4IvLo;
    BOOL fgEven;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    u4IvHi = (UINT32)rArg.ai4Arg[2];
    u4IvLo = (UINT32)rArg.ai4Arg[3];

    MTDMX_PRINT(" -u4IvHi=0x%x, u4IvLo=0x%x\n", (unsigned int)u4IvHi,
                (unsigned int)u4IvLo);

    DMX_SetDVBIV(u1Idx, u4IvHi, u4IvLo, fgEven);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set crypto key
 *
 *  @param  u1KeyIdx        The key index
 *  @param  fgEven          If fgEven=TRUE means set even key
 *  @param  eKeyType        The key type 
 *  @param  pu1Key          Pointer to the key value
 *  @param  u4KeyLen        The key length 
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetCryptoKey(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T    rArg;
    UINT8 u1KeyIdx;
    BOOL fgEven;
    DMX_CRYPTO_KEY_TYPE_T eKeyType;
    UINT8 *pu1UserKey, *pu1Key;
    UINT32 u4KeyLen;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u1KeyIdx = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    eKeyType = (DMX_CRYPTO_KEY_TYPE_T)rArg.ai4Arg[2];
    pu1UserKey = (UINT8*)rArg.ai4Arg[3];
    u4KeyLen = (UINT32)rArg.ai4Arg[4]; 
    

    pu1Key = (UINT8 *)x_mem_alloc(u4KeyLen);
    if (!pu1Key)
    {
        MTDMX_PRINT("alloc key buffer error, size=%u\n", u4KeyLen);
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1UserKey, u4KeyLen);
    COPY_FROM_USER_ARG_SIZE(pu1UserKey, pu1Key, u4KeyLen);

    if (!DMX_SetCryptoKey(u1KeyIdx, fgEven, eKeyType, pu1Key, u4KeyLen))
    {
        return MTR_NOT_OK;
    }
				
    x_mem_free((VOID*)pu1Key);
    
    MTDMX_PRINT(" - u1KeyIdx=%d, fgEven=%d, eKeyType=%u, u4KeyLen=%u\n", 
                (int)u1KeyIdx, (int)fgEven, (UINT32)eKeyType, u4KeyLen);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get ES fifo data size
 *
 *  @param  u1Pidx          PID index, 0 ~ 79
 *  @param  pu4Size [out]
 *                          The ES fifo Size
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_Get_ESFifoSize(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx;
    UINT32* pu4Size;
    UINT32 u4Size;
    DMX_PID_T rPid;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    pu4Size = (UINT32*)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }

    if (!DMX_GetPid(u1PhyPidx, DMX_PID_FLAG_DATA_POINTERS | DMX_PID_FLAG_BUFFER, &rPid))
    {
        return MTR_NOT_OK;
    }

    u4Size = DATASIZE(rPid.u4Rp, rPid.u4Wp, rPid.u4BufSize);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, u4Size, UINT32);

    MTDMX_PRINT("u1PhyPidIndex = %d , Size = %d.\n",u1PhyPidx,u4Size);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  _MTDMX_GetPacketStatus
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetPacketStatus(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 u1Tsidx;
    UINT32 u4MsTime;
    UINT32 u4RcvCount, u4DropCount;
    UINT32 *pu4RcvCount, *pu4DropCount;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1Tsidx  = (UINT8)rArg.ai4Arg[0];
    u4MsTime = (UINT32)rArg.ai4Arg[1];
    pu4RcvCount = (UINT32*)rArg.ai4Arg[2];
    pu4DropCount = (UINT32*)rArg.ai4Arg[3];

    DMX_GetPacketStatus(u1Tsidx, u4MsTime, &u4RcvCount, &u4DropCount);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4RcvCount, UINT32);
    COPY_TO_USER_ARG(pu4RcvCount, u4RcvCount, UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4DropCount, UINT32);
    COPY_TO_USER_ARG(pu4DropCount, u4DropCount, UINT32);

    MTDMX_PRINT("Tsindex: %u, Period: %u ms, receive %u pkt, drop %u pkt\n",
                u1Tsidx, u4MsTime, u4RcvCount, u4DropCount);
    return MTR_OK;
}


#ifdef ENABLE_MULTIMEDIA
static MT_RESULT_T _MTDMX_AES_SetDecryptInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 i;
    UINT32 u4Flags;
    MTDMX_DRM_AES_T* prDrm =NULL;
    MTDMX_DRM_AES_T rDrm;
    DMX_MM_DRM_AES_T rDrvDrm;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4Flags = (UINT32)rArg.ai4Arg[0];
    prDrm = (MTDMX_DRM_AES_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prDrm, MTDMX_DRM_AES_T);
    COPY_FROM_USER_ARG(prDrm, rDrm, MTDMX_DRM_AES_T);

    rDrvDrm.fgCBC = rDrm.fgCBC;
    rDrvDrm.u4KeyBitLen = rDrm.u4KeyBitLen;
    rDrvDrm.u4Offset = rDrm.u4Offset;
    rDrvDrm.u4EncryLen = rDrm.u4EncryLen;
    rDrvDrm.fgEncrypt = rDrm.fgEncrypt;

    for (i=0; i <16; i++)
    {
        rDrvDrm.au1IV[i] = rDrm.au1IV[i];
    }

    for (i=0; i <32; i++)
    {
        rDrvDrm.au1Key[i] = rDrm.au1Key[i];
    }

    if (!DMX_AES_SetDecryptInfo(u4Flags, &rDrvDrm))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTDMX_AES_MoveBlockData(unsigned long arg)
{
    INT32    Arg;
    MTDMX_DRM_AES_BLKDATA_T* prBlkData =NULL;
    MTDMX_DRM_AES_BLKDATA_T rBlkData;
    //DMX_AES_BLKDATA_T rDrvBlkData;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    prBlkData = (MTDMX_DRM_AES_BLKDATA_T*)Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prBlkData, MTDMX_DRM_AES_BLKDATA_T);
    COPY_FROM_USER_ARG(prBlkData, rBlkData, MTDMX_DRM_AES_BLKDATA_T);

    if (!DMX_AES_MoveBlockData((DMX_AES_BLKDATA_T*)&rBlkData))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTDMX_AES_SetDecryptInfoEx(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 i;
    UINT32 u4Flags;
    MTDMX_DRM_AES_T* prDrm =NULL;
    MTDMX_DRM_AES_T rDrm;
    DMX_MM_DRM_AES_T rDrvDrm;
    UINT8 u1Idx;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Idx = (UINT8)rArg.ai4Arg[0];
    u4Flags = (UINT32)rArg.ai4Arg[1];
    prDrm = (MTDMX_DRM_AES_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prDrm, MTDMX_DRM_AES_T);
    COPY_FROM_USER_ARG(prDrm, rDrm, MTDMX_DRM_AES_T);

    rDrvDrm.fgCBC = rDrm.fgCBC;
    rDrvDrm.u4KeyBitLen = rDrm.u4KeyBitLen;
    rDrvDrm.u4Offset = rDrm.u4Offset;
    rDrvDrm.u4EncryLen = rDrm.u4EncryLen;
    rDrvDrm.fgEncrypt = rDrm.fgEncrypt;

    for (i=0; i <16; i++)
    {
        rDrvDrm.au1IV[i] = rDrm.au1IV[i];
    }

    for (i=0; i <32; i++)
    {
        rDrvDrm.au1Key[i] = rDrm.au1Key[i];
    }

    if (!DMX_AES_SetDecryptInfoEx(u1Idx, u4Flags, &rDrvDrm))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

static MT_RESULT_T _MTDMX_AES_AllocEngine(unsigned long arg)
{
    INT32    Arg;
    UINT8 u1Idx;
    UINT8 *pu1Idx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    pu1Idx = (UINT8 *)Arg;

    u1Idx = DMX_AES_AllocEngine();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Idx, UINT8);
    COPY_TO_USER_ARG(pu1Idx, u1Idx, UINT32);

    return MTR_OK;
}


static MT_RESULT_T _MTDMX_AES_FreeEngine(unsigned long arg)
{
    INT32    Arg;
    UINT8 u1Idx;
    //UINT8 *pu1Idx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Idx = (UINT8)Arg;

    if (!DMX_AES_FreeEngine(u1Idx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTDMX_AES_MoveBlockDataEx(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Idx;
    MTDMX_DRM_AES_BLKDATA_T* prBlkData =NULL;
    MTDMX_DRM_AES_BLKDATA_T rBlkData;
    //DMX_AES_BLKDATA_T rDrvBlkData;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Idx = (UINT32)rArg.ai4Arg[0];
    prBlkData = (MTDMX_DRM_AES_BLKDATA_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prBlkData, MTDMX_DRM_AES_BLKDATA_T);
    COPY_FROM_USER_ARG(prBlkData, rBlkData, MTDMX_DRM_AES_BLKDATA_T);

    if (!DMX_AES_MoveBlockDataEx(u1Idx, (DMX_AES_BLKDATA_T*)&rBlkData))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

#endif //ENABLE_MULTIMEDIA


static MT_RESULT_T _MTDMX_GCPU_Cmd(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T    rArg;
    UINT32 u4Handle;
    UINT32 u4Cmd;
    void* pvUserParam = NULL;
    void* pvParam = NULL;
    UINT32 u4Size;
    BOOL fgIommu;
    UINT8 au1Param[sizeof(TSDESC_PARAM_T)]; // the max size of GCPU parameter

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u4Handle = (UINT32)rArg.ai4Arg[0];
    u4Cmd = (UINT32)rArg.ai4Arg[1];
    pvUserParam = (void *)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];
    fgIommu = (BOOL)rArg.ai4Arg[4];

    if (u4Size > sizeof(TSDESC_PARAM_T))
    {
        MTDMX_PRINT("The parameter size: %d is over reserved size: %d\n", u4Size, (int)sizeof(TSDESC_PARAM_T));
        return MTR_NOT_OK;
    }

    pvParam = au1Param;
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pvUserParam, u4Size);
    COPY_FROM_USER_ARG_SIZE(pvUserParam, pvParam, u4Size);

    MTDMX_PRINT(" -u4Handle=%d, u4Cmd=%d, u4ParamSize=%d\n", u4Handle, (int)u4Cmd, (int)u4Size);

    if (GCPU_CmdEx(u4Handle, u4Cmd, pvParam, fgIommu) != S_GCPU_OK)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pvUserParam, u4Size);
    COPY_TO_USER_ARG_SIZE(pvUserParam, pvParam, u4Size);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** Set PID input source type
 *
 *  @param  u1Pidx           PID index value
 *  @param  eType            PID input source type
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetPidInputSource(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    MTDMX_PID_INSRC_T eType;
    DMX_PID_T rPid;
    DMX_SOURCE_T eSource;
    UINT8 u1PhyPidx = MTAL_DMX_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    eType = (MTDMX_PID_INSRC_T)rArg.ai4Arg[1];

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }


    if (eType == MTDMX_PID_INSRC_DTV)
    {
        eSource = DMX_SOURCE_DTV0;
    }
    else if ((eType == MTDMX_PID_INSRC_TIMESHIFT_PLAY) || (eType == MTDMX_PID_INSRC_MM))
    {
        eSource = DMX_SOURCE_TSFILE0;
    }
    else
    {
        return MTR_NOT_OK;
    }

    if (!DMX_SetPidEx(u1PhyPidx, DMX_PID_FLAG_NONE, &rPid, eSource))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Set the Key Index for CIplus which in TrustZone
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_ReloadSecureKey(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1DmxKeyIdx;
    UINT32 u4SecureKeyIdx;
    DMX_CRYPTO_KEY_TYPE_T eKeyType;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1DmxKeyIdx = (UINT8)rArg.ai4Arg[0];
    u4SecureKeyIdx = (UINT8)rArg.ai4Arg[1];
    eKeyType = (DMX_CRYPTO_KEY_TYPE_T)rArg.ai4Arg[2];

    if (!DMX_ReloadSecureKey(u1DmxKeyIdx, u4SecureKeyIdx, eKeyType))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

#ifdef CC_DMX_SUPPORT_MULTI2
//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2Config
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2Config(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    BOOL fgOfbFreeRun;
    UINT16 u2Iteration;
    UINT8 u1Index;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    fgOfbFreeRun = (BOOL)rArg.ai4Arg[1];
    u2Iteration = (UINT16)rArg.ai4Arg[2];

    DMX_SetMulti2Config(u1Index,fgOfbFreeRun, u2Iteration);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2Key
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2Key(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1Index;
    BOOL fgEven;
    UINT32 *pu4Key;
    UINT32 au4Key[2];

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    fgEven = (BOOL)rArg.ai4Arg[1];
    pu4Key = (UINT32*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Key, 8);
    COPY_FROM_USER_ARG_SIZE(pu4Key, au4Key, 8);

    if (!DMX_SetMulti2Key(u1Index, fgEven, au4Key))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2CbcIV
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2CbcIV(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4IvHi;
    UINT32 u4IvLo;
    UINT8 u1Index;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    u4IvHi = (UINT32)rArg.ai4Arg[1];
    u4IvLo = (UINT32)rArg.ai4Arg[2];

    DMX_SetMulti2CbcIV(u1Index,u4IvHi, u4IvLo);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2OfbIV
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2OfbIV(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4IvHi;
    UINT32 u4IvLo;
    UINT8 u1Index;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    u4IvHi = (UINT32)rArg.ai4Arg[1];
    u4IvLo = (UINT32)rArg.ai4Arg[2];

    DMX_SetMulti2OfbIV(u1Index,u4IvHi, u4IvLo);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2SysKey
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2SysKey(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 *pu4Key;
    UINT32 au4Key[8];
    UINT8 u1Index;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Index = (UINT8)rArg.ai4Arg[0];
    pu4Key = (UINT32*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu4Key, 32);
    COPY_FROM_USER_ARG_SIZE(pu4Key, au4Key, 32);

    DMX_SetMulti2SysKey(u1Index,au4Key);

    return MTR_OK;
}
#else
//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2Config
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2Config(unsigned long arg)
{
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2Key
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2Key(unsigned long arg)
{
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2CbcIV
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2CbcIV(unsigned long arg)
{
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2OfbIV
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2OfbIV(unsigned long arg)
{
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDMX_SetMulti2SysKey
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_SetMulti2SysKey(unsigned long arg)
{
    return MTR_OK;
}

#endif

static MT_RESULT_T _MTDRM_WVSetAssetKey(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 *pu1Key;
    UINT8 au1Key[16];
    UINT32 u4KeyLen;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Key = (UINT8*)rArg.ai4Arg[0];
    u4KeyLen = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Key, 16);
    COPY_FROM_USER_ARG_SIZE(pu1Key, au1Key, 16);

    if (!TZ_DRM_WV_SetAssetKey(au1Key, u4KeyLen))
    {
        return MTR_NOT_OK;
    }
#endif
    return MTR_OK;
}


static MT_RESULT_T _MTDRM_WVDeriveCW(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 *pu1Ecm;
    UINT32 *pu4Flags;
    UINT8 au1Ecm[32];
    UINT32 u4Flags = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Ecm = (UINT8 *)rArg.ai4Arg[0];
    pu4Flags = (UINT32 *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Ecm, 32);
    COPY_FROM_USER_ARG_SIZE(pu1Ecm, au1Ecm, 32);

    if (!TZ_DRM_WV_DeriveCW(au1Ecm, &u4Flags))
    {
        return MTR_NOT_OK;
    }

    //Printf("ECM in io_mtdmx: 0x%x\n", u4Flags);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Flags, UINT32);
    COPY_TO_USER_ARG(pu4Flags, u4Flags, UINT32);
#endif
    return MTR_OK;
}


static MT_RESULT_T _MTDRM_WVPassKeybox(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 *pu1Keybox;
    UINT8 au1Keybox[256];
    UINT32 u4Size;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Keybox = (UINT8*)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Keybox, 256);
    COPY_FROM_USER_ARG_SIZE(pu1Keybox, au1Keybox, 256);

    if (!TZ_DRM_WV_PassKeybox(au1Keybox, u4Size))
    {
        return MTR_NOT_OK;
    }
#endif

    return MTR_OK;
}

static MT_RESULT_T  _MTDRM_WVDataDecrypt(unsigned long arg)
{
    static BOOL fgInited = FALSE;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 *pOutputbuf, *pInputbuf, *pCounter;
    INT32 length;
    UINT8 au1Iv[16];

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pOutputbuf = (UINT8 *)rArg.ai4Arg[0];
    pInputbuf = (UINT8 *)rArg.ai4Arg[1];
    length = (INT32)rArg.ai4Arg[2];
    pCounter = (UINT8 *)rArg.ai4Arg[3];

    if (length % 16 != 0)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pCounter, sizeof(au1Iv));
    COPY_FROM_USER_ARG_SIZE(pCounter, au1Iv, sizeof(au1Iv));

    // alloc GCPU handler for HDCP
    if (!fgInited)
    {
        if (prGcpuHDCP == NULL)
        {
            prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
        }
        rParam.eMode = BLOCK_CIPHER_MODE_CBC;
        rParam.eKeyBitLen = KEY_BIT_LEN_128;
        prGcpuHDCP->u4AesKeyPtr = AES_WV_SECURE_KEY_PTR;

        fgInited = TRUE;
    }

    rParam.u4SrcStartAddr = (UINT32)pInputbuf;
    rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pInputbuf);
    rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pInputbuf + length));
    rParam.u4DstStartAddr = (UINT32)pOutputbuf;
    rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pOutputbuf);
    rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pOutputbuf + length));
    rParam.u4DatLen = length;
    rParam.fgEncrypt = FALSE;
    memcpy(rParam.au1Iv, au1Iv, sizeof(au1Iv));

    if (GCPU_Cmd((UINT32)prGcpuHDCP, GCPU_AES, &rParam) != S_GCPU_OK)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

///////////////////////////////////for hulu+dash+wv drm only begin//////////////
static MT_RESULT_T _MTDRM_WV_ValidateKeybox(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
	UINT32 *presult = NULL;
	UINT32 result = 0;
    INT32 Arg;

	//MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

	presult = (UINT32 *)Arg;
	
	if (!TZ_DRM_WV_ValidateKeybox(&result))
	{
		printf("TZ_DRM_WV_ValidateKeybox error\n");
		return MTR_NOT_OK;
	}
	
	USR_SPACE_ACCESS_VALIDATE_ARG(presult, UINT32);
	COPY_TO_USER_ARG(presult, result, UINT32);
		
	MTDMX_PRINT(" result=%d, *presult=%d \n",result,*presult);

#endif

    return MTR_OK;
}

#ifdef CC_TRUSTZONE_SUPPORT
static BOOL fgFirst = TRUE;
uint8_t* acipher_data = NULL;
static UINT32 decrypt_data_buffer = 80*1024;
#endif

static MT_RESULT_T _MTDRM_WV_DestroySession(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
	UINT32 asesscnt = 0;
    INT32 Arg;

	//MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

	asesscnt = (UINT32)Arg;
	//MTDMX_PRINT("asesscnt=0x%x\n",asesscnt);

    if (!TZ_DRM_WV_DestroySession((SessionContext *)asesscnt))
    {
    	printf("TZ_DRM_WV_DestroySession error\n");
        return MTR_NOT_OK;
    }

	if(!fgFirst)  
	{
		printf("_MTDRM_WV_DestroySession x_mem_free acipher_data \n");
		if(NULL != acipher_data)	
		{		
			x_mem_free((VOID*)acipher_data);
			acipher_data = NULL;
		}

		fgFirst = TRUE;
	}
#endif

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_WV_CreatSession(unsigned long arg)
{
	#ifdef CC_TRUSTZONE_SUPPORT
	UINT32 *psesscnt = NULL;
	UINT32 asesscnt = 0;
    INT32 Arg;

	//MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

	psesscnt = (UINT32 *)Arg;

    if (!TZ_DRM_WV_CreatSession(&asesscnt))
    {
    	printf("TZ_DRM_WV_CreatSession error\n");
        return MTR_NOT_OK;
    }
	
	USR_SPACE_ACCESS_VALIDATE_ARG(psesscnt, INT32);
	COPY_TO_USER_ARG(psesscnt, asesscnt, INT32);

	if(fgFirst)   
	{
		printf("_MTDRM_WV_CreatSession x_mem_alloc acipher_data 80k \n");

		acipher_data = (UINT8 *)x_mem_alloc(decrypt_data_buffer); //80k
		if (!acipher_data)
		{
			printf("_MTDRM_WV_CreatSession alloc cipher_data error\n");
			return MTR_NOT_OK;		
		}
		x_memset(acipher_data, 0, decrypt_data_buffer);			
		
		fgFirst = FALSE;
	}
		
#endif

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_WV_GenerateNonceSess(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT32 nonce = 0;
	UINT32 *pnonce = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
    pnonce = (UINT32 *)rArg.ai4Arg[1]; 

    if (!TZ_DRM_WV_GenerateNonceSess((SessionContext *)asesscnt, &nonce))
    {
    	printf("TZ_DRM_WV_GenerateNonceSess error\n");
        return MTR_NOT_OK;
    }
	
	USR_SPACE_ACCESS_VALIDATE_ARG(pnonce, UINT32);
    COPY_TO_USER_ARG(pnonce, nonce, UINT32);
	
	//MTDMX_PRINT(" nonce=0x%x, *pnonce=0x%x\n",nonce,*pnonce);
#endif

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_WV_GenerateDeriveKeys(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmaster_key, *pmac_context, *penc_context;
    UINT32 mst_cnt_len, mac_cnt_len, enc_cnt_len;
	WV_IOCTL_DERIVEKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_master_key = NULL;
	uint8_t* puil_cont_mac_context = NULL;
	uint8_t* puil_cont_enc_context = NULL;
	#if WV_DEBUG_LOG
	int i;
    #endif

	pmaster_key=pmac_context=penc_context = NULL;
	mst_cnt_len=mac_cnt_len=enc_cnt_len = 0;
	 
	MTDMX_PRINT(" Entry\n");
	
    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
  
	asesscnt = (UINT32)rArg.ai4Arg[0];
	pmaster_key = (UINT8*)rArg.ai4Arg[1];
    mst_cnt_len = (UINT32)rArg.ai4Arg[2];
	pmac_context = (UINT8*)rArg.ai4Arg[3];
    mac_cnt_len = (UINT32)rArg.ai4Arg[4];
	penc_context = (UINT8*)rArg.ai4Arg[5];
    enc_cnt_len = (UINT32)rArg.ai4Arg[6];

	u4BufVAddr = (WV_IOCTL_DERIVEKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_DERIVEKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_GenerateDeriveKeys Allocate WV_IOCTL_DERIVEKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(mst_cnt_len != 0)
	{
		puil_cont_master_key = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(mst_cnt_len, TZ_CACHE_LINE_SIZE));
		if(NULL == puil_cont_master_key)
		{
			printf("_MTDRM_WV_GenerateDeriveKeys alloc puil_cont_master_key fail\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmaster_key, mst_cnt_len);
		COPY_FROM_USER_ARG_SIZE(pmaster_key, puil_cont_master_key, mst_cnt_len);
	}
	
	if(mac_cnt_len != 0)
	{
		puil_cont_mac_context = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(mac_cnt_len, TZ_CACHE_LINE_SIZE));
		if(NULL == puil_cont_mac_context)
		{
			printf("_MTDRM_WV_GenerateDeriveKeys alloc mac_context error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmac_context, mac_cnt_len);
		COPY_FROM_USER_ARG_SIZE(pmac_context, puil_cont_mac_context, mac_cnt_len);
	}
	
	if(enc_cnt_len != 0)
	{
		puil_cont_enc_context = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(enc_cnt_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_enc_context)
		{
			printf("_MTDRM_WV_GenerateDeriveKeys alloc enc_context error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(penc_context, enc_cnt_len);
		COPY_FROM_USER_ARG_SIZE(penc_context, puil_cont_enc_context, enc_cnt_len);
	}

	
#if WV_DEBUG_LOG
	printf("mst_cnt_len = %d\n",mst_cnt_len);
	for(i = 0; i < mst_cnt_len; i++)
	{
	    printf("%d ", puil_cont_master_key[i]);
	}
	
	printf("\n");
	
    printf("mac_cnt_len = %d\n",mac_cnt_len);
	for(i = 0; i < mac_cnt_len; i++)
	{
	    printf("%d ", puil_cont_mac_context[i]);
	}
	printf("\n");
	
	printf("enc_cnt_len = %d\n",enc_cnt_len);
	for(i = 0; i < enc_cnt_len; i++)
	{
	    printf("%d ", puil_cont_enc_context[i]);
	}	
	printf("\n");
#endif

	u4BufVAddr->master_key = (uint8_t *)PHYSICAL((UINT32)puil_cont_master_key);
	u4BufVAddr->mst_cnt_len = mst_cnt_len;
	u4BufVAddr->mac_context = (uint8_t *)PHYSICAL((UINT32)puil_cont_mac_context);
	u4BufVAddr->mac_cnt_len = mac_cnt_len;
	u4BufVAddr->enc_context = (uint8_t *)PHYSICAL((UINT32)puil_cont_enc_context);
	u4BufVAddr->enc_cnt_len = enc_cnt_len;

	if(puil_cont_master_key)
		TZ_NWD_BufferStart((UINT32)puil_cont_master_key, TZ_CACHE_ALIGN(mst_cnt_len));
	if(puil_cont_mac_context)	
		TZ_NWD_BufferStart((UINT32)puil_cont_mac_context, TZ_CACHE_ALIGN(mac_cnt_len));
	if(puil_cont_enc_context)
		TZ_NWD_BufferStart((UINT32)puil_cont_enc_context, TZ_CACHE_ALIGN(enc_cnt_len));

	//printf("TZ_DRM_WV_GenerateDeriveKeys TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_GenerateDeriveKeys(u4BufVAddr, sizeof(WV_IOCTL_DERIVEKEY_T)))
    {
    	printf("TZ_DRM_WV_GenerateDeriveKeys error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_master_key)
		TZ_NWD_BufferEnd((UINT32)puil_cont_master_key, TZ_CACHE_ALIGN(mst_cnt_len));
	if(puil_cont_mac_context)
		TZ_NWD_BufferEnd((UINT32)puil_cont_mac_context, TZ_CACHE_ALIGN(mac_cnt_len));
	if(puil_cont_enc_context)
		TZ_NWD_BufferEnd((UINT32)puil_cont_enc_context, TZ_CACHE_ALIGN(enc_cnt_len));

	//printf("TZ_DRM_WV_GenerateDeriveKeys TZ_NWD_BufferEnd\n");

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_master_key)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_master_key));
	if(puil_cont_mac_context)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_mac_context));
	if(puil_cont_enc_context)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_enc_context));
#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_GenerateSignature(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

	#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_5ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmessage, *psignature;
	UINT32 message_length, signature_length;
	UINT32 *psignature_length = NULL;
	WV_IOCTL_SIG_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_message = NULL;
	uint8_t* puil_cont_signature = NULL;
	#if WV_DEBUG_LOG 
	int i = 0;
	#endif

	pmessage=psignature = NULL;
	message_length=signature_length = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    asesscnt = (UINT32)rArg.ai4Arg[0];
	pmessage = (UINT8*)rArg.ai4Arg[1];
    message_length = (UINT32)rArg.ai4Arg[2];
	psignature = (UINT8*)rArg.ai4Arg[3];
	psignature_length = (UINT32 *)rArg.ai4Arg[4];

	u4BufVAddr = (WV_IOCTL_SIG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_SIG_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_GenerateSignature Allocate WV_IOCTL_SIG_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(message_length != 0)
	{
		puil_cont_message = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(message_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_message)
		{
			printf("_MTDRM_WV_GenerateSignature alloc message error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmessage, message_length);
		COPY_FROM_USER_ARG_SIZE(pmessage, puil_cont_message, message_length);
	}

	signature_length = *psignature_length;
	if(signature_length !=0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_GenerateSignature alloc signature error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		x_memset(puil_cont_signature, 0, signature_length);
	}
	
	//MTDMX_PRINT("signature_length in = %d\n",signature_length);
#if WV_DEBUG_LOG
	printf("message_length = %d\n",message_length);
	for(i = 0; i < message_length; i++)
	{
	    printf("%d ", puil_cont_message[i]);
	}
	printf("\n");
#endif

	u4BufVAddr->message = (uint8_t *)PHYSICAL((UINT32)puil_cont_message);
	u4BufVAddr->message_length = message_length;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;

	if(puil_cont_message)
		TZ_NWD_BufferStart((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferStart((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_GenerateSignature TZ_NWD_BufferStart\n");
	
	if (!TZ_DRM_WV_GenerateSignature(u4BufVAddr, sizeof(WV_IOCTL_SIG_T)))
    {
    	printf("TZ_DRM_WV_GenerateSignature error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_message)
		TZ_NWD_BufferEnd((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_GenerateSignature TZ_NWD_BufferEnd\n");

	USR_SPACE_ACCESS_VALIDATE_ARG(psignature_length, UINT32);
    COPY_TO_USER_ARG(psignature_length, u4BufVAddr->signature_length, UINT32);

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, u4BufVAddr->signature_length);
    COPY_TO_USER_ARG_SIZE(psignature, puil_cont_signature, u4BufVAddr->signature_length);
	
	MTDMX_PRINT("signature_length out =%d\n",*psignature_length);

#if  WV_DEBUG_LOG 
	printf("signature = \n");
	for(i = 0; i < u4BufVAddr->signature_length; i++)
	{
	    printf("%x ", puil_cont_signature[i]);
	}
	printf("\n");
#endif

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_message)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_message));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));
#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_ValidateMessage(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_5ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmessage, *psignature;
	UINT32 message_length, signature_length;
	WV_IOCTL_SIG_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_message = NULL;
	uint8_t* puil_cont_signature = NULL;
	#if WV_DEBUG_LOG
	int i;
	#endif

	pmessage=psignature = NULL;
	message_length=signature_length = 0;
		
	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pmessage = (UINT8*)rArg.ai4Arg[1];
    message_length = (UINT32)rArg.ai4Arg[2];
	psignature = (UINT8*)rArg.ai4Arg[3];
	signature_length = (UINT32)rArg.ai4Arg[4];

	u4BufVAddr = (WV_IOCTL_SIG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_SIG_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_ValidateMessage Allocate WV_IOCTL_SIG_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(message_length != 0)
	{
		puil_cont_message = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(message_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_message)
		{
			printf("_MTDRM_WV_ValidateMessage alloc message error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmessage, message_length);
		COPY_FROM_USER_ARG_SIZE(pmessage, puil_cont_message, message_length);
	}

	if(signature_length != 0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_ValidateMessage alloc signature error\n");
			ret = MTR_NOT_OK;		
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, signature_length);
		COPY_FROM_USER_ARG_SIZE(psignature, puil_cont_signature, signature_length);
	}

#if WV_DEBUG_LOG
    printf("message_length = %d\n",message_length);
	for(i = 0; i < message_length; i++)
	{
	    printf("%d ", puil_cont_message[i]);
	}
	printf("\n");
	printf("signature_length = %d\n",signature_length);
	for(i = 0; i < signature_length; i++)
	{
	    printf("%d ", puil_cont_signature[i]);
	}
	printf("\n");
#endif
    
	u4BufVAddr->message = (uint8_t *)PHYSICAL((UINT32)puil_cont_message);
	u4BufVAddr->message_length = message_length;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;

	if(puil_cont_message)
		TZ_NWD_BufferStart((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferStart((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_ValidateMessage TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_ValidateMessage(u4BufVAddr, sizeof(WV_IOCTL_SIG_T)))
	{
    	printf("TZ_DRM_WV_ValidateMessage error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_message)
		TZ_NWD_BufferEnd((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_ValidateMessage TZ_NWD_BufferEnd\n");

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_message)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_message));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));
#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_InstallKey(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_11ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pkey_id, *pkey_data, *pkey_data_iv, *pkey_control, *pkey_control_iv;
	UINT32 key_id_len, key_data_len, key_data_iv_len, key_control_len, key_control_iv_len;
	WV_IOCTL_KEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_key_id = NULL;
	uint8_t* puil_cont_key_data = NULL;
	uint8_t* puil_cont_key_data_iv = NULL;
	uint8_t* puil_cont_key_control = NULL;
	uint8_t* puil_cont_key_control_iv = NULL;

	pkey_id=pkey_data=pkey_data_iv=pkey_control=pkey_control_iv = NULL;
	key_id_len=key_data_len=key_data_iv_len=key_control_len=key_control_iv_len= 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_11ARG(arg);
    COPY_FROM_USER_11ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pkey_id = (UINT8*)rArg.ai4Arg[1];
    key_id_len = (UINT32)rArg.ai4Arg[2];
	pkey_data = (UINT8*)rArg.ai4Arg[3];
	key_data_len = (UINT32)rArg.ai4Arg[4];
	pkey_data_iv = (UINT8*)rArg.ai4Arg[5];
    key_data_iv_len = (UINT32)rArg.ai4Arg[6];
	pkey_control = (UINT8*)rArg.ai4Arg[7];
	key_control_len = (UINT32)rArg.ai4Arg[8];
	pkey_control_iv = (UINT8*)rArg.ai4Arg[9];
    key_control_iv_len = (UINT32)rArg.ai4Arg[10];

	u4BufVAddr = (WV_IOCTL_KEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_KEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_InstallKey Allocate WV_IOCTL_KEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;
		
	if(key_id_len != 0)
	{
		puil_cont_key_id = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_id_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_id)
		{
			printf("_MTDRM_WV_InstallKey alloc key_id error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_id, key_id_len);
		COPY_FROM_USER_ARG_SIZE(pkey_id, puil_cont_key_id, key_id_len);
	}

	if(key_data_len != 0)
	{
		puil_cont_key_data = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_data_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_data)
		{
			printf("_MTDRM_WV_InstallKey alloc key_data error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_data, key_data_len);
		COPY_FROM_USER_ARG_SIZE(pkey_data, puil_cont_key_data, key_data_len);
	}

	if(key_data_iv_len != 0)
	{
		puil_cont_key_data_iv = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_data_iv_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_data_iv)
		{
			printf("_MTDRM_WV_InstallKey alloc key_data_iv error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_data_iv, key_data_iv_len);
		COPY_FROM_USER_ARG_SIZE(pkey_data_iv, puil_cont_key_data_iv, key_data_iv_len);
	}

	if(key_control_len != 0)
	{
		puil_cont_key_control = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_control_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_control)
		{
			printf("_MTDRM_WV_InstallKey alloc key_control error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_control, key_control_len);
		COPY_FROM_USER_ARG_SIZE(pkey_control, puil_cont_key_control, key_control_len);
	}

	if(key_control_iv_len != 0)
	{
		puil_cont_key_control_iv = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_control_iv_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_control_iv)
		{
			printf("_MTDRM_WV_InstallKey alloc key_control_iv error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_control_iv, key_control_iv_len);
		COPY_FROM_USER_ARG_SIZE(pkey_control_iv, puil_cont_key_control_iv, key_control_iv_len);
	}

	u4BufVAddr->key_id = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_id); 
	u4BufVAddr->key_id_length = key_id_len;
	u4BufVAddr->key_data = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_data);
	u4BufVAddr->key_data_length = key_data_len;
	u4BufVAddr->key_data_iv = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_data_iv); 
	u4BufVAddr->key_data_iv_length = key_data_iv_len;
	u4BufVAddr->key_control = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_control); 
	u4BufVAddr->key_control_length = key_control_len;
	u4BufVAddr->key_control_iv = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_control_iv); 
	u4BufVAddr->key_control_iv_length = key_control_iv_len;

	if(puil_cont_key_id)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));
	if(puil_cont_key_data)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_data, TZ_CACHE_ALIGN(key_data_len));
	if(puil_cont_key_data_iv)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_data_iv, TZ_CACHE_ALIGN(key_data_iv_len));
	if(puil_cont_key_control)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_control, TZ_CACHE_ALIGN(key_control_len));
	if(puil_cont_key_control_iv)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_control_iv, TZ_CACHE_ALIGN(key_control_iv_len));

	//printf("TZ_DRM_WV_InstallKey TZ_NWD_BufferStart\n");
	//printf("TZ_DRM_WV_InstallKey sizeof(WV_IOCTL_KEY_T)=%d, sizeof(u4BufVAddr) \n",sizeof(WV_IOCTL_KEY_T),sizeof(u4BufVAddr));

	if (!TZ_DRM_WV_InstallKey(u4BufVAddr, sizeof(WV_IOCTL_KEY_T)))
    {
    	printf("TZ_DRM_WV_InstallKey error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_key_id)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));	
	if(puil_cont_key_data)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_data, TZ_CACHE_ALIGN(key_data_len));	
	if(puil_cont_key_data_iv)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_data_iv, TZ_CACHE_ALIGN(key_data_iv_len));
	if(puil_cont_key_control)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_control, TZ_CACHE_ALIGN(key_control_len));	
	if(puil_cont_key_control_iv)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_control_iv, TZ_CACHE_ALIGN(key_control_iv_len));

	//printf("TZ_DRM_WV_InstallKey TZ_NWD_BufferEnd\n");

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_key_id)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_id));
	if(puil_cont_key_data)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_data));
	if(puil_cont_key_data_iv)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_data_iv));
	if(puil_cont_key_control)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_control));
	if(puil_cont_key_control_iv)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_control_iv));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_FlushNoncesSess(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
	UINT32 asesscnt = 0;
    INT32 Arg;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

	asesscnt = (UINT32)Arg;
	
    if (!TZ_DRM_WV_FlushNoncesSess((SessionContext *)asesscnt))
    {
    	printf("TZ_DRM_WV_FlushNoncesSess error\n");
        return MTR_NOT_OK;
    }
#endif

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_WV_UpdateMacKeys(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
	MTAL_IOCTL_5ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmac_keys, *piv;
	UINT32 mac_keys_len, iv_len;
	
	WV_IOCTL_MACKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_mac_keys = NULL;
	uint8_t* puil_cont_iv = NULL;

	pmac_keys=piv = NULL;
	mac_keys_len=iv_len = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pmac_keys = (UINT8*)rArg.ai4Arg[1];
    mac_keys_len = (UINT32)rArg.ai4Arg[2];
	piv = (UINT8*)rArg.ai4Arg[3];
	iv_len = (UINT32)rArg.ai4Arg[4];

	u4BufVAddr = (WV_IOCTL_MACKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_MACKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_UpdateMacKeys Allocate WV_IOCTL_MACKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(mac_keys_len != 0)
	{
		puil_cont_mac_keys = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(mac_keys_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_mac_keys)
		{
			printf("_MTDRM_WV_UpdateMacKeys alloc mac_keys error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmac_keys, mac_keys_len);
		COPY_FROM_USER_ARG_SIZE(pmac_keys, puil_cont_mac_keys, mac_keys_len);
	}

	if(iv_len != 0)
	{
		puil_cont_iv = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(iv_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_iv)
		{
			printf("_MTDRM_WV_UpdateMacKeys alloc iv error\n");
			ret = MTR_NOT_OK;		
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(piv, iv_len);
		COPY_FROM_USER_ARG_SIZE(piv, puil_cont_iv, iv_len);
	}

	u4BufVAddr->mac_keys = (uint8_t *)PHYSICAL((UINT32)puil_cont_mac_keys); 
	u4BufVAddr->mac_keys_length = mac_keys_len;
	u4BufVAddr->iv = (uint8_t *)PHYSICAL((UINT32)puil_cont_iv);
	u4BufVAddr->iv_length = iv_len;

	if(puil_cont_mac_keys)
		TZ_NWD_BufferStart((UINT32)puil_cont_mac_keys, TZ_CACHE_ALIGN(mac_keys_len));
	if(puil_cont_iv)
		TZ_NWD_BufferStart((UINT32)puil_cont_iv, TZ_CACHE_ALIGN(iv_len));

	//printf("TZ_DRM_WV_UpdateMacKeys TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_UpdateMacKeys(u4BufVAddr, sizeof(WV_IOCTL_MACKEY_T)))
    {
    	printf("TZ_DRM_WV_UpdateMacKeys error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_mac_keys)
		TZ_NWD_BufferEnd((UINT32)puil_cont_mac_keys, TZ_CACHE_ALIGN(mac_keys_len));
	if(puil_cont_iv)
		TZ_NWD_BufferEnd((UINT32)puil_cont_iv, TZ_CACHE_ALIGN(iv_len));	

	//printf("TZ_DRM_WV_UpdateMacKeys TZ_NWD_BufferEnd\n");

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_mac_keys)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_mac_keys));
	if(puil_cont_iv)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_iv));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_RefreshKey(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pkey_id,  *pkey_control, *pkey_control_iv;
	UINT32 key_id_len, key_control_len, key_control_iv_len;
	WV_IOCTL_KEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_key_id = NULL;
	uint8_t* puil_cont_key_control = NULL;
	uint8_t* puil_cont_key_control_iv = NULL;

	pkey_id=pkey_control=pkey_control_iv = NULL;
	key_id_len=key_control_len=key_control_iv_len =0 ;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pkey_id = (UINT8*)rArg.ai4Arg[1];
    key_id_len = (UINT32)rArg.ai4Arg[2];
	pkey_control = (UINT8*)rArg.ai4Arg[3];
	key_control_len = (UINT32)rArg.ai4Arg[4];
	pkey_control_iv = (UINT8*)rArg.ai4Arg[5];
    key_control_iv_len = (UINT32)rArg.ai4Arg[6];

	u4BufVAddr = (WV_IOCTL_KEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_KEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_RefreshKey Allocate WV_IOCTL_KEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;


	if(key_id_len != 0)
	{
		puil_cont_key_id = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_id_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_id)
		{
			printf("_MTDRM_WV_RefreshKey alloc key_id error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_id, key_id_len);
		COPY_FROM_USER_ARG_SIZE(pkey_id, puil_cont_key_id, key_id_len);
	}

	if(key_control_len != 0)
	{
		puil_cont_key_control = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_control_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_control)
		{
			printf("_MTDRM_WV_RefreshKey alloc key_control error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_control, key_control_len);
		COPY_FROM_USER_ARG_SIZE(pkey_control, puil_cont_key_control, key_control_len);
	}

	if(key_control_iv_len != 0)
	{
		puil_cont_key_control_iv = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_control_iv_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_control_iv)
		{
			printf("_MTDRM_WV_RefreshKey alloc key_control_iv error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pkey_control_iv, key_control_iv_len);
		COPY_FROM_USER_ARG_SIZE(pkey_control_iv, puil_cont_key_control_iv, key_control_iv_len);
	}

	u4BufVAddr->key_id = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_id); 
	u4BufVAddr->key_id_length = key_id_len;
	u4BufVAddr->key_control = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_control); 
	u4BufVAddr->key_control_length = key_control_len;
	u4BufVAddr->key_control_iv = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_control_iv); 
	u4BufVAddr->key_control_iv_length = key_control_iv_len;

	if(puil_cont_key_id)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));
	if(puil_cont_key_control)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_control, TZ_CACHE_ALIGN(key_control_len));
	if(puil_cont_key_control_iv)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_control_iv, TZ_CACHE_ALIGN(key_control_iv_len));
	
    //printf("TZ_DRM_WV_RefreshKey TZ_NWD_BufferStart\n");
	
	if (!TZ_DRM_WV_RefreshKey(u4BufVAddr, sizeof(WV_IOCTL_KEY_T)))
    {
    	printf("TZ_DRM_WV_RefreshKey error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_key_id)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));
	if(puil_cont_key_control)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_control, TZ_CACHE_ALIGN(key_control_len));
	if(puil_cont_key_control_iv)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_control_iv, TZ_CACHE_ALIGN(key_control_iv_len));
	
	//printf("TZ_DRM_WV_RefreshKey TZ_NWD_BufferEnd\n");

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_key_id)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_id));
	if(puil_cont_key_control)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_control));
	if(puil_cont_key_control_iv)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_control_iv));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_SelectContentKey(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_3ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *key_id = NULL;
	int key_id_len = 0;
	WV_IOCTL_CONTKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_key_id = NULL;
	
	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
    key_id = (UINT8 *)rArg.ai4Arg[1];
    key_id_len = (UINT32)rArg.ai4Arg[2];

	u4BufVAddr = (WV_IOCTL_CONTKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_CONTKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_SelectContentKey Allocate WV_IOCTL_CONTKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(key_id_len != 0)
	{
		puil_cont_key_id = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(key_id_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_key_id)
		{
			printf("_MTDRM_WV_SelectContentKey alloc key_id error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(key_id, key_id_len);
		COPY_FROM_USER_ARG_SIZE(key_id, puil_cont_key_id, key_id_len);
	}

	u4BufVAddr->key_id = (uint8_t *)PHYSICAL((UINT32)puil_cont_key_id); 
	u4BufVAddr->key_id_length = key_id_len;

	if(puil_cont_key_id)
		TZ_NWD_BufferStart((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));

	//printf("TZ_DRM_WV_SelectContentKey TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_SelectContentKey(u4BufVAddr, sizeof(WV_IOCTL_CONTKEY_T)))
    {
    	printf("TZ_DRM_WV_SelectContentKey error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_key_id)
		TZ_NWD_BufferEnd((UINT32)puil_cont_key_id, TZ_CACHE_ALIGN(key_id_len));

	//printf("TZ_DRM_WV_SelectContentKey TZ_NWD_BufferEnd\n");
s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_key_id)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_key_id));

#endif

    return ret;
}

#if 0
static MT_RESULT_T _MTDRM_WV_DecryptCTR(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_9ARG_T rArg;
	UINT32 asesscnt = 0;
	uint8_t* iv = NULL;
	uint8_t aiv[AES_FUN_BLK_LEN] = {0};
	size_t block_offset = 0;
	uint8_t* cipher_data = NULL;
	size_t cipher_data_length = 0;
	BOOL is_encrypted = 0;
	void* clear_data = NULL;
	
	BufferType buffer_type = 0;
	uint32_t curtime = 0;
	MT_RESULT_T ret = MTR_OK;
	
	unsigned short tmp_slot = 0;
	static BOOL fgInited = false;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;
	uint8_t* pSrcBuf = NULL;
	uint8_t* pDstBuf = NULL;
	size_t len = 0;
	

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_9ARG(arg);
    COPY_FROM_USER_9ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
	iv = (uint8_t*)rArg.ai4Arg[1];
	block_offset = (size_t)rArg.ai4Arg[2];
	cipher_data = (uint8_t*)rArg.ai4Arg[3];
	cipher_data_length = (size_t)rArg.ai4Arg[4];
	is_encrypted= (BOOL)rArg.ai4Arg[5];
	clear_data = (void*)rArg.ai4Arg[6];
	buffer_type = (BufferType)rArg.ai4Arg[7];
	curtime =(uint32_t)rArg.ai4Arg[8];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(iv, AES_FUN_BLK_LEN);
	COPY_FROM_USER_ARG_SIZE(iv, aiv, AES_FUN_BLK_LEN);

	//printf("cipher_data_length=%d,cipher_data addr=%p, clear_data addr=%p, block_offset=%d, is_encrypted=%d, buffer_type=%d, time=%d\n",cipher_data_length,cipher_data,clear_data,block_offset,is_encrypted,buffer_type,curtime);

	if (!is_encrypted)// If the data is clear, we do not need a current key selected.
	{
		MTDMX_PRINT(" the data is clear,do not need decrypt \n");
		x_memcpy((uint8_t*)(clear_data),cipher_data, cipher_data_length);
		return MTR_OK;
	} 
	else
	{

		if (cipher_data_length % 16 != 0)
    	{
    		printf("_MTDRM_WV_DecryptCTR(): error cipher_data_length % 16 != 0 \n");
        	return MTR_NOT_OK;
    	}
								
		if (!TZ_DRM_WV_CheckDecCTRContentKey((SessionContext *)asesscnt, curtime,buffer_type,&tmp_slot))
		{
			printf("TZ_DRM_WV_CheckDecCTRContentKey error\n");
			ret = MTR_NOT_OK;
			//goto s;
			return ret;
		}
		
		if (buffer_type == kBufferTypeDirect)
		{
			// For reference implementation, we quietly drop direct video.
			printf("[_MTDRM_WV_DecryptCTR(): kBufferTypeDirect]\n");			
		}	
		else if (buffer_type == kBufferTypeSecure)
		{
			// For reference implementation, we also quietly drop secure data.
			printf("[_MTDRM_WV_DecryptCTR(): kBufferTypeSecure]\n");			
		}
		else
		{	
			// alloc GCPU handler for HDCP
		    if (!fgInited)
		    {
		        if (prGcpuHDCP == NULL)
		        {
		            prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
		        }
		        rParam.eMode = BLOCK_CIPHER_MODE_CTR;
		        rParam.eKeyBitLen = KEY_BIT_LEN_128;
		        prGcpuHDCP->u4AesKeyPtr = tmp_slot;

		        fgInited = true;
		    }

			pSrcBuf = cipher_data ;
			pDstBuf = clear_data ;
			len = cipher_data_length ;

			rParam.u4SrcStartAddr = (UINT32)pSrcBuf;
			rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pSrcBuf);
			rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pSrcBuf + len));
			rParam.u4DstStartAddr = (UINT32)pDstBuf;
			rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pDstBuf);
			rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pDstBuf + len));
			rParam.u4DatLen = len;
			rParam.fgEncrypt = false;
			x_memcpy(rParam.au1Iv, aiv, sizeof(aiv));
				
			if (GCPU_Cmd((UINT32)prGcpuHDCP, GCPU_AES, &rParam) != S_GCPU_OK)
			{
				printf("_MTDRM_WV_DecryptCTR GCPU_Cmd GCPU_AES error\n");
				ret = MTR_NOT_OK;
				return ret;
			}
	
		}				
	
	}

#endif

    return ret;
}

#else

static MT_RESULT_T _MTDRM_WV_DecryptCTR(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_9ARG_T rArg;
	UINT32 asesscnt = 0;
	uint8_t* iv = NULL;
	uint8_t aiv[AES_FUN_BLK_LEN] = {0};
	size_t block_offset = 0;
	uint8_t* cipher_data = NULL;
	size_t cipher_data_length = 0;
	BOOL is_encrypted = 0;
	void* clear_data = NULL;
	BufferType buffer_type = 0;
	uint32_t curtime = 0;
	
	unsigned short tmp_slot = 0;
	static BOOL fgInited = false;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;
	uint8_t* pSrcBuf = NULL;
	uint8_t* pDstBuf = NULL;
	size_t len = 0;
	size_t n = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_9ARG(arg);
    COPY_FROM_USER_9ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
	iv = (uint8_t*)rArg.ai4Arg[1];
	block_offset = (size_t)rArg.ai4Arg[2];
	cipher_data = (uint8_t*)rArg.ai4Arg[3];
	cipher_data_length = (size_t)rArg.ai4Arg[4];
	is_encrypted= (BOOL)rArg.ai4Arg[5];
	clear_data = (void*)rArg.ai4Arg[6];
	buffer_type = (BufferType)rArg.ai4Arg[7];
	curtime =(uint32_t)rArg.ai4Arg[8];

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(iv, AES_FUN_BLK_LEN);
	COPY_FROM_USER_ARG_SIZE(iv, aiv, AES_FUN_BLK_LEN);

	//printf("_MTDRM_WV_DecryptCTR cipher_data_length=%d, block_offset=%d, is_encrypted=%d \n",cipher_data_length,block_offset,is_encrypted);

	if (!is_encrypted)// If the data is clear, we do not need a current key selected.
	{
		MTDMX_PRINT(" the data is clear,do not need decrypt \n");
		x_memcpy((uint8_t*)(clear_data),cipher_data, cipher_data_length);
		return MTR_OK;
	} 
	else
	{
		n = (cipher_data_length + block_offset) % 16;
				
		if(n == 0)
		{	
			len = cipher_data_length + block_offset;
		}
		else
		{
			len = cipher_data_length + block_offset + (16 - n);
		}
			
		//printf("_MTDRM_WV_DecryptCTR length=%d \n",len);

		if(decrypt_data_buffer < len)
		{
			printf("_MTDRM_WV_DecryptCTR x_mem_alloc acipher_data again, size=%d \n",len);
			if(NULL != acipher_data)
			{
				x_mem_free((VOID*)acipher_data);
				acipher_data = NULL;
			}
		
			acipher_data = (UINT8 *)x_mem_alloc(len);
			if (!acipher_data)
			{
				printf("_MTDRM_WV_DecryptCTR alloc cipher_data error\n");
				return MTR_NOT_OK;	
			}
			x_memset(acipher_data, 0, len);

			decrypt_data_buffer = len;
		}
		
		/*if(n == 0)
		{	
			USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(cipher_data, cipher_data_length);
			COPY_FROM_USER_ARG_SIZE(cipher_data, acipher_data, cipher_data_length);			
		}
		else
		{
			USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(cipher_data, cipher_data_length);
			
			if(block_offset == 0)
			{
				COPY_FROM_USER_ARG_SIZE(cipher_data, acipher_data, cipher_data_length);
			}
			else
			{
				COPY_FROM_USER_ARG_SIZE(cipher_data, acipher_data + (16 - n), cipher_data_length);	
			}
		}*/

		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(cipher_data, cipher_data_length);
		if(block_offset == 0)
		{
			COPY_FROM_USER_ARG_SIZE(cipher_data, acipher_data, cipher_data_length);
		}
		else
		{
			COPY_FROM_USER_ARG_SIZE(cipher_data, acipher_data + block_offset, cipher_data_length);	
		}
								
		if (!TZ_DRM_WV_CheckDecCTRContentKey((SessionContext *)asesscnt, curtime,buffer_type,&tmp_slot))
		{
			printf("TZ_DRM_WV_CheckDecCTRContentKey error\n");
			ret = MTR_NOT_OK;
			return ret;
		}
		
		if (buffer_type == kBufferTypeDirect)
		{
			// For reference implementation, we quietly drop direct video.
			printf("[_MTDRM_WV_DecryptCTR(): kBufferTypeDirect]\n");			
		}	
		else if (buffer_type == kBufferTypeSecure)
		{
			// For reference implementation, we also quietly drop secure data.
			printf("[_MTDRM_WV_DecryptCTR(): kBufferTypeSecure]\n");			
		}
		else
		{	
			// alloc GCPU handler for HDCP
		    if (!fgInited)
		    {
		        if (prGcpuHDCP == NULL)
		        {
		            prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
		        }
		        rParam.eMode = BLOCK_CIPHER_MODE_CTR;
		        rParam.eKeyBitLen = KEY_BIT_LEN_128;
		        prGcpuHDCP->u4AesKeyPtr = tmp_slot;

		        fgInited = true;
		    }

			pSrcBuf = acipher_data ;
			pDstBuf = acipher_data ;

			rParam.u4SrcStartAddr = (UINT32)pSrcBuf;
			rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pSrcBuf);
			rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pSrcBuf + len));
			rParam.u4DstStartAddr = (UINT32)pDstBuf;
			rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pDstBuf);
			rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(pDstBuf + len));
			rParam.u4DatLen = len;
			rParam.fgEncrypt = false;
			x_memcpy(rParam.au1Iv, aiv, sizeof(aiv));
				
			if (GCPU_Cmd((UINT32)prGcpuHDCP, GCPU_AES, &rParam) != S_GCPU_OK)
			{
				printf("_MTDRM_WV_DecryptCTR GCPU_Cmd GCPU_AES error\n");
				ret = MTR_NOT_OK;
				return ret;
			}
	
		}

		if(block_offset == 0)
		{
			USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(clear_data, cipher_data_length);
			COPY_TO_USER_ARG_SIZE(clear_data, acipher_data, cipher_data_length);
		}
		else
		{
			USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(clear_data, cipher_data_length);
			COPY_TO_USER_ARG_SIZE(clear_data, acipher_data + block_offset, cipher_data_length);
		}
	
	}
		
#endif

    return ret;
}
#endif

static MT_RESULT_T _MTDRM_WV_SaveKeybox(unsigned long arg)
{
#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 *pu1Keybox = NULL;
    UINT8 au1Keybox[128] = {0};
    UINT32 u4Size = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Keybox = (UINT8*)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Keybox, 128);
    COPY_FROM_USER_ARG_SIZE(pu1Keybox, au1Keybox, 128);

    if (!TZ_DRM_WV_SaveKeybox(au1Keybox, u4Size))
    {
    	printf("TZ_DRM_WV_SaveKeybox error\n");
        return MTR_NOT_OK;
    }
#endif

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_WV_GetRandomNum(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
	uint8_t * prandomData = NULL;
	//uint8_t * arandomData = NULL;
	size_t dataLength = 0;	
	UINT32 u4BufVAddr;
	
	#if WV_DEBUG_LOG
	int i;
	#endif

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
	
	prandomData = (uint8_t * )rArg.ai4Arg[0];
	dataLength = (size_t)rArg.ai4Arg[1];
	
	u4BufVAddr = (UINT32)VIRTUAL(BSP_AllocAlignedDmaMemory(dataLength, TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_GetRandomNum Allocate failed\n");
		return MTR_NOT_OK;
	} 

	if (!TZ_DRM_WV_GetRandomNum((void *)u4BufVAddr, dataLength))
	{
		printf("TZ_DRM_WV_GetRandomNum error\n");
		ret = MTR_NOT_OK;
		goto s;
	}

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prandomData, dataLength);
    COPY_TO_USER_ARG_SIZE(prandomData, (void *)u4BufVAddr, dataLength);

	//printf("dataLength =%d\n",dataLength);
#if WV_DEBUG_LOG
	printf("randomData = \n");
	for(i = 0; i < dataLength; i++)
	{
		printf("%d ", prandomData[i]);
	}
	printf("\n");
#endif

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_RewrapDeviceRSAKeySess(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_12ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmessage, *psignature, *penc_rsa_key, *pwrapped_rsa_key, *penc_rsa_key_iv;
	UINT32 message_length, signature_length,enc_rsa_key_length, wrapped_rsa_key_length;
	UINT32 *pnonce = NULL;
	UINT32 nonce = 0;
	UINT32 *pwrapped_rsa_key_length = NULL;
	UINT32 *presult = NULL;

	WV_IOCTL_RSAKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_message = NULL;
	uint8_t* puil_cont_signature = NULL;
	uint8_t* puil_cont_enc_rsa_key = NULL;
	uint8_t* puil_cont_enc_rsa_key_iv = NULL;
	uint8_t* puil_cont_wrapped_rsa_key = NULL;
	#if WV_DEBUG_LOG
	int i;
    #endif

	pmessage=psignature=penc_rsa_key=pwrapped_rsa_key=penc_rsa_key_iv = NULL;
	message_length=signature_length=enc_rsa_key_length=wrapped_rsa_key_length = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_12ARG(arg);
    COPY_FROM_USER_12ARG(arg, rArg);

    asesscnt = (UINT32)rArg.ai4Arg[0];
	pmessage = (UINT8*)rArg.ai4Arg[1];
    message_length = (UINT32)rArg.ai4Arg[2];
	psignature = (UINT8*)rArg.ai4Arg[3];
	signature_length = (UINT32)rArg.ai4Arg[4];
	pnonce = (UINT32 *)rArg.ai4Arg[5];
	penc_rsa_key = (UINT8*)rArg.ai4Arg[6];
    enc_rsa_key_length = (UINT32)rArg.ai4Arg[7];
	penc_rsa_key_iv = (UINT8*)rArg.ai4Arg[8];
    pwrapped_rsa_key = (UINT8*)rArg.ai4Arg[9];
	pwrapped_rsa_key_length = (UINT32 *)rArg.ai4Arg[10];
	presult = (UINT32 *)rArg.ai4Arg[11];

	u4BufVAddr = (WV_IOCTL_RSAKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_RSAKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_RewrapDeviceRSAKeySess Allocate WV_IOCTL_RSAKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(message_length != 0)
	{
		puil_cont_message = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(message_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_message)
		{
			printf("_MTDRM_WV_RewrapDeviceRSAKeySess alloc message error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmessage, message_length);
		COPY_FROM_USER_ARG_SIZE(pmessage, puil_cont_message, message_length);
	}

	if(signature_length != 0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_RewrapDeviceRSAKeySess alloc signature error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, signature_length);
		COPY_FROM_USER_ARG_SIZE(psignature, puil_cont_signature, signature_length);
	}

	USR_SPACE_ACCESS_VALIDATE_ARG(pnonce, UINT32);
	COPY_FROM_USER_ARG(pnonce, nonce, UINT32);

	if(enc_rsa_key_length != 0)
	{
		puil_cont_enc_rsa_key = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(enc_rsa_key_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_enc_rsa_key)
		{
			printf("_MTDRM_WV_RewrapDeviceRSAKeySess alloc enc_rsa_key error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(penc_rsa_key, enc_rsa_key_length);
	    COPY_FROM_USER_ARG_SIZE(penc_rsa_key, puil_cont_enc_rsa_key, enc_rsa_key_length);
	}

	puil_cont_enc_rsa_key_iv = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(KEY_IV_SIZE, TZ_CACHE_LINE_SIZE));
	if (NULL == puil_cont_enc_rsa_key_iv)
	{
		printf("_MTDRM_WV_RewrapDeviceRSAKeySess alloc enc_rsa_key_iv error\n");
		ret = MTR_NOT_OK;
		goto s;
	}
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(penc_rsa_key_iv, KEY_IV_SIZE);
    COPY_FROM_USER_ARG_SIZE(penc_rsa_key_iv, puil_cont_enc_rsa_key_iv, KEY_IV_SIZE);

	wrapped_rsa_key_length = *pwrapped_rsa_key_length;

	if(wrapped_rsa_key_length != 0)
	{
		puil_cont_wrapped_rsa_key = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(wrapped_rsa_key_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_wrapped_rsa_key)
		{
			printf("_MTDRM_WV_RewrapDeviceRSAKeySess alloc wrapped_rsa_key error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		x_memset(puil_cont_wrapped_rsa_key, 0, wrapped_rsa_key_length);
	}

	MTDMX_PRINT("message_length in = %d\n", message_length);
	MTDMX_PRINT("signature_length in = %d\n", signature_length);
	MTDMX_PRINT("enc_rsa_key_length in = %d\n", enc_rsa_key_length);
	MTDMX_PRINT("wrapped_rsa_key_length in = %d\n", wrapped_rsa_key_length);

	u4BufVAddr->message = (uint8_t *)PHYSICAL((UINT32)puil_cont_message);
	u4BufVAddr->message_length = message_length;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;
	u4BufVAddr->nonce = nonce;
	u4BufVAddr->enc_rsa_key = (uint8_t *)PHYSICAL((UINT32)puil_cont_enc_rsa_key);
	u4BufVAddr->enc_rsa_key_length = enc_rsa_key_length;
	u4BufVAddr->enc_rsa_key_iv = (uint8_t *)PHYSICAL((UINT32)puil_cont_enc_rsa_key_iv); 
	u4BufVAddr->wrapped_rsa_key = (uint8_t *)PHYSICAL((UINT32)puil_cont_wrapped_rsa_key); 
	u4BufVAddr->wrapped_rsa_key_length = wrapped_rsa_key_length;
	u4BufVAddr->result = 0;

	//printf("TZ_DRM_WV_RewrapDeviceRSAKeySess 8 \n");
	if(puil_cont_message)
		TZ_NWD_BufferStart((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferStart((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));
	if(puil_cont_enc_rsa_key)
		TZ_NWD_BufferStart((UINT32)puil_cont_enc_rsa_key, TZ_CACHE_ALIGN(enc_rsa_key_length));
	if(puil_cont_enc_rsa_key_iv)
		TZ_NWD_BufferStart((UINT32)puil_cont_enc_rsa_key_iv, TZ_CACHE_ALIGN(KEY_IV_SIZE));
	if(puil_cont_wrapped_rsa_key)
		TZ_NWD_BufferStart((UINT32)puil_cont_wrapped_rsa_key, TZ_CACHE_ALIGN(wrapped_rsa_key_length));

	//printf("TZ_DRM_WV_RewrapDeviceRSAKeySess TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_RewrapDeviceRSAKeySess(u4BufVAddr, sizeof(WV_IOCTL_RSAKEY_T)))
    {
    	printf("TZ_DRM_WV_RewrapDeviceRSAKeySess error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_message)
		TZ_NWD_BufferEnd((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));
	if(puil_cont_enc_rsa_key)
		TZ_NWD_BufferEnd((UINT32)puil_cont_enc_rsa_key, TZ_CACHE_ALIGN(enc_rsa_key_length));
	if(puil_cont_enc_rsa_key_iv)
		TZ_NWD_BufferEnd((UINT32)puil_cont_enc_rsa_key_iv, TZ_CACHE_ALIGN(KEY_IV_SIZE));
	if(puil_cont_wrapped_rsa_key)
		TZ_NWD_BufferEnd((UINT32)puil_cont_wrapped_rsa_key, TZ_CACHE_ALIGN(wrapped_rsa_key_length));

	//printf("TZ_DRM_WV_RewrapDeviceRSAKeySess TZ_NWD_BufferEnd\n");


	USR_SPACE_ACCESS_VALIDATE_ARG(pwrapped_rsa_key_length, UINT32);
	COPY_TO_USER_ARG(pwrapped_rsa_key_length, u4BufVAddr->wrapped_rsa_key_length, UINT32);

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pwrapped_rsa_key, u4BufVAddr->wrapped_rsa_key_length);
    COPY_TO_USER_ARG_SIZE(pwrapped_rsa_key, puil_cont_wrapped_rsa_key, u4BufVAddr->wrapped_rsa_key_length);

	USR_SPACE_ACCESS_VALIDATE_ARG(presult, UINT32);
	COPY_TO_USER_ARG(presult, u4BufVAddr->result, UINT32);

	MTDMX_PRINT("wrapped_rsa_key_length out = %d\n", *pwrapped_rsa_key_length);
	#if WV_DEBUG_LOG
	for(i = 0; i < *pwrapped_rsa_key_length; i++)
	{
	    printf("%x ", pwrapped_rsa_key[i]);
	}
	printf("\n");
	#endif
	MTDMX_PRINT("*presult=%d \n",*presult);

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_message)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_message));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));
	if(puil_cont_enc_rsa_key)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_enc_rsa_key));
	if(puil_cont_enc_rsa_key_iv)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_enc_rsa_key_iv));
	if(puil_cont_wrapped_rsa_key)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_wrapped_rsa_key));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_LoadDeviceRSAKeySessCtx(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_4ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pwrapped_rsa_key = NULL;
	UINT32 wrapped_rsa_key_length = 0;
	UINT32 *presult = NULL;
	WV_IOCTL_RSAKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_wrapped_rsa_key = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pwrapped_rsa_key = (UINT8*)rArg.ai4Arg[1];
    wrapped_rsa_key_length = (UINT32)rArg.ai4Arg[2];
	presult = (UINT32 *)rArg.ai4Arg[3];

	u4BufVAddr = (WV_IOCTL_RSAKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_RSAKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_LoadDeviceRSAKeySessCtx Allocate WV_IOCTL_RSAKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(wrapped_rsa_key_length != 0)
	{
		puil_cont_wrapped_rsa_key = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(wrapped_rsa_key_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_wrapped_rsa_key)
		{
			printf("_MTDRM_WV_LoadDeviceRSAKeySessCtx alloc wrapped_rsa_key error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pwrapped_rsa_key, wrapped_rsa_key_length);
		COPY_FROM_USER_ARG_SIZE(pwrapped_rsa_key, puil_cont_wrapped_rsa_key, wrapped_rsa_key_length);
	}
	
	u4BufVAddr->wrapped_rsa_key = (uint8_t *)PHYSICAL((UINT32)puil_cont_wrapped_rsa_key); 
	u4BufVAddr->wrapped_rsa_key_length = wrapped_rsa_key_length;
	u4BufVAddr->result = 0;

	if(puil_cont_wrapped_rsa_key)
		TZ_NWD_BufferStart((UINT32)puil_cont_wrapped_rsa_key, TZ_CACHE_ALIGN(wrapped_rsa_key_length));

	//printf("TZ_DRM_WV_LoadDeviceRSAKeySessCtx TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_LoadDeviceRSAKeySessCtx(u4BufVAddr, sizeof(WV_IOCTL_RSAKEY_T)))
    {
    	printf("TZ_DRM_WV_LoadDeviceRSAKeySessCtx error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_wrapped_rsa_key)
		TZ_NWD_BufferEnd((UINT32)puil_cont_wrapped_rsa_key, TZ_CACHE_ALIGN(wrapped_rsa_key_length));

	//printf("TZ_DRM_WV_LoadDeviceRSAKeySessCtx TZ_NWD_BufferEnd\n");

	USR_SPACE_ACCESS_VALIDATE_ARG(presult, UINT32);
	COPY_TO_USER_ARG(presult, u4BufVAddr->result, UINT32);
		
	MTDMX_PRINT("*presult=%d \n",*presult);

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_wrapped_rsa_key)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_wrapped_rsa_key));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_GenerateRSASignature(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_5ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *pmessage, *psignature;
	UINT32 message_length, signature_length;
	UINT32 *psignature_length = NULL;
	WV_IOCTL_SIG_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_message = NULL;
	uint8_t* puil_cont_signature = NULL;
	#if WV_DEBUG_LOG
	int i;
	#endif

	pmessage=psignature = NULL;
	message_length=signature_length= 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	pmessage = (UINT8*)rArg.ai4Arg[1];
    message_length = (UINT32)rArg.ai4Arg[2];
	psignature = (UINT8*)rArg.ai4Arg[3];
	psignature_length = (UINT32 *)rArg.ai4Arg[4];

	u4BufVAddr = (WV_IOCTL_SIG_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_SIG_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_GenerateRSASignature Allocate WV_IOCTL_SIG_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(message_length != 0)
	{
		puil_cont_message = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(message_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_message)
		{
			printf("_MTDRM_WV_GenerateRSASignature alloc message error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmessage, message_length);
		COPY_FROM_USER_ARG_SIZE(pmessage, puil_cont_message, message_length);
	}

	signature_length = *psignature_length;
	if(signature_length !=0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_GenerateRSASignature alloc signature error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		x_memset(puil_cont_signature, 0, signature_length);
	}

	MTDMX_PRINT("signature_length in = %d\n",signature_length);
#if WV_DEBUG_LOG
		printf("message_length = %d\n",message_length);
		for(i = 0; i < message_length; i++)
		{
			printf("%x ", puil_cont_message[i]);
		}
		printf("\n");
#endif

	u4BufVAddr->message = (uint8_t *)PHYSICAL((UINT32)puil_cont_message);
	u4BufVAddr->message_length = message_length;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;

	if(puil_cont_message)
		TZ_NWD_BufferStart((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferStart((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_GenerateRSASignature TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_GenerateRSASignature(u4BufVAddr, sizeof(WV_IOCTL_SIG_T)))
    {
    	printf("TZ_DRM_WV_GenerateRSASignature error\n");
        ret = MTR_NOT_OK;
		goto s;
    }

	if(puil_cont_message)
		TZ_NWD_BufferEnd((UINT32)puil_cont_message, TZ_CACHE_ALIGN(message_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	//printf("TZ_DRM_WV_GenerateRSASignature TZ_NWD_BufferEnd\n");

	USR_SPACE_ACCESS_VALIDATE_ARG(psignature_length, UINT32);
    COPY_TO_USER_ARG(psignature_length, u4BufVAddr->signature_length, UINT32);

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, u4BufVAddr->signature_length);
    COPY_TO_USER_ARG_SIZE(psignature, puil_cont_signature, u4BufVAddr->signature_length);
	
	MTDMX_PRINT("signature_length out =%d\n",*psignature_length);
#if WV_DEBUG_LOG
		printf("signature = \n");
		for(i = 0; i < *psignature_length; i++)
		{
			printf("%x ", psignature[i]);
		}
		printf("\n");
#endif

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_message)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_message));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_RSADeriveKeys(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8 *penc_session_key, *pmac_context, *penc_context;
	UINT32 enc_sesskey_len, mac_context_len, enc_context_len;
	
	WV_IOCTL_RSADERIVEKEY_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_enc_session_key = NULL;
	uint8_t* puil_cont_mac_context = NULL;
	uint8_t* puil_cont_enc_context = NULL;

	penc_session_key=pmac_context=penc_context = NULL;
	enc_sesskey_len=mac_context_len=enc_context_len = 0;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);

	asesscnt = (UINT32)rArg.ai4Arg[0];
	penc_session_key = (UINT8*)rArg.ai4Arg[1];
    enc_sesskey_len = (UINT32)rArg.ai4Arg[2];
	pmac_context = (UINT8*)rArg.ai4Arg[3];
	mac_context_len = (UINT32)rArg.ai4Arg[4];
	penc_context = (UINT8*)rArg.ai4Arg[5];
    enc_context_len = (UINT32)rArg.ai4Arg[6];

	u4BufVAddr = (WV_IOCTL_RSADERIVEKEY_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_RSADERIVEKEY_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_RSADeriveKeys Allocate WV_IOCTL_RSADERIVEKEY_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(enc_sesskey_len != 0)
	{
		puil_cont_enc_session_key = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(enc_sesskey_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_enc_session_key)
		{
			printf("_MTDRM_WV_RSADeriveKeys alloc enc_session_key error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(penc_session_key, enc_sesskey_len);
		COPY_FROM_USER_ARG_SIZE(penc_session_key, puil_cont_enc_session_key, enc_sesskey_len);
	}

	if(mac_context_len != 0)
	{
		puil_cont_mac_context = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(mac_context_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_mac_context)
		{
			printf("_MTDRM_WV_RSADeriveKeys alloc mac_context error\n");
			ret = MTR_NOT_OK;		
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pmac_context, mac_context_len);
	    COPY_FROM_USER_ARG_SIZE(pmac_context, puil_cont_mac_context, mac_context_len);
	}

	if(enc_context_len != 0)
	{
		puil_cont_enc_context = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(enc_context_len, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_enc_context)
		{
			printf("_MTDRM_WV_RSADeriveKeys alloc enc_context error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(penc_context, enc_context_len);
	    COPY_FROM_USER_ARG_SIZE(penc_context, puil_cont_enc_context, enc_context_len);
	}

	u4BufVAddr->enc_session_key = (uint8_t *)PHYSICAL((UINT32)puil_cont_enc_session_key);
	u4BufVAddr->enc_session_key_length = enc_sesskey_len;
	u4BufVAddr->mac_context = (uint8_t *)PHYSICAL((UINT32)puil_cont_mac_context);
	u4BufVAddr->mac_context_length = mac_context_len;
	u4BufVAddr->enc_context = (uint8_t *)PHYSICAL((UINT32)puil_cont_enc_context);
	u4BufVAddr->enc_context_length = enc_context_len;

	if(puil_cont_enc_session_key)
		TZ_NWD_BufferStart((UINT32)puil_cont_enc_session_key, TZ_CACHE_ALIGN(enc_sesskey_len));
	if(puil_cont_mac_context)
		TZ_NWD_BufferStart((UINT32)puil_cont_mac_context, TZ_CACHE_ALIGN(mac_context_len));
	if(puil_cont_enc_context)
		TZ_NWD_BufferStart((UINT32)puil_cont_enc_context, TZ_CACHE_ALIGN(enc_context_len));

	//printf("TZ_DRM_WV_RSADeriveKeys TZ_NWD_BufferStart\n");

	if (!TZ_DRM_WV_RSADeriveKeys(u4BufVAddr, sizeof(WV_IOCTL_RSADERIVEKEY_T)))
    {
    	printf("TZ_DRM_WV_RSADeriveKeys error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_enc_session_key)
		TZ_NWD_BufferEnd((UINT32)puil_cont_enc_session_key, TZ_CACHE_ALIGN(enc_sesskey_len));
	if(puil_cont_mac_context)
		TZ_NWD_BufferEnd((UINT32)puil_cont_mac_context, TZ_CACHE_ALIGN(mac_context_len));
	if(puil_cont_enc_context)
		TZ_NWD_BufferEnd((UINT32)puil_cont_enc_context, TZ_CACHE_ALIGN(enc_context_len));

	//printf("TZ_DRM_WV_RSADeriveKeys TZ_NWD_BufferEnd\n");

s:
	BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_enc_session_key)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_enc_session_key));
	if(puil_cont_mac_context)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_mac_context));
	if(puil_cont_enc_context)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_enc_context));	

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_Generic_Encrypt(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8* in_buffer, *ain_buffer;
	UINT32 buffer_length = 0;
	UINT8* iv = NULL;
	UINT8 aiv[AES_FUN_BLK_LEN] = {0};
	OEMCrypto_Algorithm algorithm = 0;
	UINT8* out_buffer, *aout_buffer;
	UINT32 curtime = 0;
	
	unsigned short tmp_slot = 0;
	static BOOL fgInited = false;
    static GCPU_STRUCT_T *prGcpuHDCP = NULL;
    static AES_PARAM_T rParam;

	in_buffer=ain_buffer = NULL;
	out_buffer=aout_buffer = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
    in_buffer = (UINT8*)rArg.ai4Arg[1];
    buffer_length = (UINT32)rArg.ai4Arg[2];
	iv = (UINT8*)rArg.ai4Arg[3];
	algorithm = (OEMCrypto_Algorithm)rArg.ai4Arg[4];
	out_buffer = (UINT8*)rArg.ai4Arg[5];
    curtime = (UINT32)rArg.ai4Arg[6];

	if(buffer_length != 0)
	{
		ain_buffer = (UINT8 *)x_mem_alloc(buffer_length);
		if (!ain_buffer)
		{
			printf("_MTDRM_WV_Generic_Encrypt alloc in_buffer error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_buffer, buffer_length);
	    COPY_FROM_USER_ARG_SIZE(in_buffer, ain_buffer, buffer_length);
	}

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(iv, AES_FUN_BLK_LEN);
    COPY_FROM_USER_ARG_SIZE(iv, aiv, AES_FUN_BLK_LEN);

	if(buffer_length != 0)
	{
		aout_buffer = (UINT8 *)x_mem_alloc(buffer_length);
		if (!aout_buffer)
		{
			printf("_MTDRM_WV_Generic_Encrypt alloc aout_buffer error\n");
			ret = MTR_NOT_OK;
			goto s;
		}
		x_memset(aout_buffer, 0, buffer_length);
	}
////////////////////////////////////////////////////////////////
	if ( buffer_length % AES_BLOCK_SIZE != 0 )
	{
		printf("[_MTDRM_WV_Generic_Encrypt(): buffers size bad.\n");
		ret = MTR_NOT_OK;
		goto s;
	}
		
	if ( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING )
	{
		printf("[_MTDRM_WV_Generic_Encrypt(): algorithm bad.\n");
		ret = MTR_NOT_OK;
		goto s;
	}

	if (!TZ_DRM_WV_CheckEncDecCBCContentKey((SessionContext *)asesscnt, curtime, 0, &tmp_slot))
	{
		printf("TZ_DRM_WV_CheckEncDecCBCContentKey error\n");
		ret = MTR_NOT_OK;
		goto s;
	}

	/*if (gcpu_invalidslot == tmp_slot)
	{
		printf("_MTDRM_WV_Generic_Encrypt  tmp_slot is error\n");
		ret = MTR_NOT_OK;
		goto s;
	}*/

	// alloc GCPU handler for HDCP
	if (!fgInited)
	{
		if (prGcpuHDCP == NULL)
		{
		 	prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
		}
		rParam.eMode = BLOCK_CIPHER_MODE_CBC;
		rParam.eKeyBitLen = KEY_BIT_LEN_128;
		prGcpuHDCP->u4AesKeyPtr = tmp_slot;

		fgInited = true;
	}

	rParam.u4SrcStartAddr = (UINT32)ain_buffer;
	rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)ain_buffer);
	rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(ain_buffer + buffer_length));
	rParam.u4DstStartAddr = (UINT32)aout_buffer;
	rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)aout_buffer);
	rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(aout_buffer + buffer_length));
	rParam.u4DatLen = buffer_length;
	rParam.fgEncrypt = true;
	x_memcpy(rParam.au1Iv, aiv, sizeof(aiv));

	if (GCPU_Cmd((UINT32)prGcpuHDCP, GCPU_AES, &rParam) != S_GCPU_OK)
	{
		printf("_MTDRM_WV_Generic_Encrypt GCPU_Cmd GCPU_AES error\n");
        ret = MTR_NOT_OK;
		goto s;
	}	
    
	/*if (!TZ_DRM_WV_Generic_Encrypt((SessionContext *)asesscnt, ain_buffer,buffer_length,aiv,algorithm,aout_buffer,curtime))
    {
    	printf("TZ_DRM_WV_Generic_Encrypt error\n");
        ret = MTR_NOT_OK;
		goto s;
    }*/
///////////////////////////////////////////////////////////////

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_buffer, buffer_length);
    COPY_TO_USER_ARG_SIZE(out_buffer, aout_buffer, buffer_length);

s:
	if(NULL != ain_buffer)
	{
		x_mem_free((VOID*)ain_buffer);
		ain_buffer = NULL;
	}
	if(NULL != aout_buffer)
	{
		x_mem_free((VOID*)aout_buffer);
		aout_buffer = NULL;
	}
#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_Generic_Decrypt(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8* in_buffer, *ain_buffer;
	UINT32 buffer_length = 0;
	UINT8* iv = NULL;
	UINT8 aiv[AES_FUN_BLK_LEN] = {0};
	OEMCrypto_Algorithm algorithm = 0;
	UINT8* out_buffer, *aout_buffer;
	UINT32 curtime = 0;
		
	unsigned short tmp_slot = 0;
	static BOOL fgInited = false;
	static GCPU_STRUCT_T *prGcpuHDCP = NULL;
	static AES_PARAM_T rParam;

	in_buffer=ain_buffer = NULL;
	out_buffer=aout_buffer = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
    in_buffer = (UINT8*)rArg.ai4Arg[1];
    buffer_length = (UINT32)rArg.ai4Arg[2];
	iv = (UINT8*)rArg.ai4Arg[3];
	algorithm = (OEMCrypto_Algorithm)rArg.ai4Arg[4];
	out_buffer = (UINT8*)rArg.ai4Arg[5];
    curtime = (UINT32)rArg.ai4Arg[6];

	if(buffer_length != 0)
	{
		ain_buffer = (UINT8 *)x_mem_alloc(buffer_length);
		if (!ain_buffer)
		{
			printf("_MTDRM_WV_Generic_Decrypt alloc in_buffer error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_buffer, buffer_length);
	    COPY_FROM_USER_ARG_SIZE(in_buffer, ain_buffer, buffer_length);
	}

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(iv, AES_FUN_BLK_LEN);
    COPY_FROM_USER_ARG_SIZE(iv, aiv, AES_FUN_BLK_LEN);

	if(buffer_length != 0)
	{
		aout_buffer = (UINT8 *)x_mem_alloc(buffer_length);
		if (!aout_buffer)
		{
			printf("_MTDRM_WV_Generic_Decrypt alloc aout_buffer error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		x_memset(aout_buffer, 0, buffer_length);
	}
////////////////////////////////////////////////
	if ( buffer_length % AES_BLOCK_SIZE != 0 )
	{
		printf("[_MTDRM_WV_Generic_Decrypt(): buffers size bad.\n");
		//return false;
		ret = MTR_NOT_OK;
		goto s;
	}
		
	if ( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING )
	{
		printf("[_MTDRM_WV_Generic_Decrypt(): algorithm bad.\n");
		//return false;
		ret = MTR_NOT_OK;
		goto s;
	}

	if (!TZ_DRM_WV_CheckEncDecCBCContentKey((SessionContext *)asesscnt, curtime, 1, &tmp_slot))
	{
		printf("TZ_DRM_WV_CheckEncDecCBCContentKey error\n");
		ret = MTR_NOT_OK;
		goto s;
	}

	/*if (gcpu_invalidslot == tmp_slot)
	{
		printf("_MTDRM_WV_Generic_Decrypt  tmp_slot is error\n");
		ret = MTR_NOT_OK;
		goto s;
	}*/

	// alloc GCPU handler for HDCP
	if (!fgInited)
	{
		if (prGcpuHDCP == NULL)
		{
		 	prGcpuHDCP = (GCPU_STRUCT_T *)GCPU_CreateHandle();
		}
		rParam.eMode = BLOCK_CIPHER_MODE_CBC;
		rParam.eKeyBitLen = KEY_BIT_LEN_128;
		prGcpuHDCP->u4AesKeyPtr = tmp_slot;

		fgInited = true;
	}

	rParam.u4SrcStartAddr = (UINT32)ain_buffer;
	rParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)ain_buffer);
	rParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)(ain_buffer + buffer_length));
	rParam.u4DstStartAddr = (UINT32)aout_buffer;
	rParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)aout_buffer);
	rParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)(aout_buffer + buffer_length));
	rParam.u4DatLen = buffer_length;
	rParam.fgEncrypt = false;
	x_memcpy(rParam.au1Iv, aiv, sizeof(aiv));

	if (GCPU_Cmd((UINT32)prGcpuHDCP, GCPU_AES, &rParam) != S_GCPU_OK)
	{
		printf("_MTDRM_WV_Generic_Decrypt GCPU_Cmd GCPU_AES error\n");
        ret = MTR_NOT_OK;
		goto s;
	}	

	/*if (!TZ_DRM_WV_Generic_Decrypt((SessionContext *)asesscnt, ain_buffer,buffer_length,aiv,algorithm,aout_buffer,curtime))
    {
    	printf("TZ_DRM_WV_Generic_Decrypt error\n");
        ret = MTR_NOT_OK;
		goto s;
    }*/
/////////////////////////////////////////////////

	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(out_buffer, buffer_length);
    COPY_TO_USER_ARG_SIZE(out_buffer, aout_buffer, buffer_length);

s:
	if(NULL != ain_buffer)
	{
		x_mem_free((VOID*)ain_buffer);
		ain_buffer = NULL;
	}
	if(NULL != aout_buffer)
	{
		x_mem_free((VOID*)aout_buffer);
		aout_buffer = NULL;
	}

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_Generic_Sign(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8* in_buffer;
	UINT32 buffer_length = 0;
	OEMCrypto_Algorithm algorithm = 0;
	UINT8* psignature;
	INT32 signature_length = 0;
	UINT32 *psignature_length = NULL;
	UINT32 curtime = 0;
	WV_IOCTL_GENERIC_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_in_buffer = NULL;
	uint8_t* puil_cont_signature = NULL;
	#if WV_DEBUG_LOG
	int i;
	#endif

	in_buffer = NULL;
	psignature = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
    in_buffer = (UINT8*)rArg.ai4Arg[1];
    buffer_length = (UINT32)rArg.ai4Arg[2];
	algorithm = (OEMCrypto_Algorithm)rArg.ai4Arg[3];
	psignature = (UINT8*)rArg.ai4Arg[4];
	psignature_length = (UINT32 *)rArg.ai4Arg[5];
    curtime = (UINT32)rArg.ai4Arg[6];

	u4BufVAddr = (WV_IOCTL_GENERIC_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_GENERIC_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_Generic_Sign Allocate WV_IOCTL_GENERIC_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(buffer_length != 0)
	{
		puil_cont_in_buffer = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(buffer_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_in_buffer)
		{
			printf("_MTDRM_WV_Generic_Sign alloc in_buffer error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_buffer, buffer_length);
	    COPY_FROM_USER_ARG_SIZE(in_buffer, puil_cont_in_buffer, buffer_length);
	}

	signature_length = *psignature_length;
	if(signature_length !=0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_Generic_Sign alloc asignature error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		x_memset(puil_cont_signature, 0, signature_length);
	}

	u4BufVAddr->in_buffer = (uint8_t *)PHYSICAL((UINT32)puil_cont_in_buffer);
	u4BufVAddr->in_buffer_length = buffer_length;
	u4BufVAddr->algorithm = algorithm;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;
	u4BufVAddr->u4Curtime = curtime;

	if(puil_cont_in_buffer)
		TZ_NWD_BufferStart((UINT32)puil_cont_in_buffer, TZ_CACHE_ALIGN(buffer_length));
	if(puil_cont_signature)
		TZ_NWD_BufferStart((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

    if (!TZ_DRM_WV_Generic_Sign(u4BufVAddr, sizeof(WV_IOCTL_GENERIC_T)))
    {
    	printf("TZ_DRM_WV_Generic_Sign error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_in_buffer)
		TZ_NWD_BufferEnd((UINT32)puil_cont_in_buffer, TZ_CACHE_ALIGN(buffer_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

	USR_SPACE_ACCESS_VALIDATE_ARG(psignature_length, UINT32);
    COPY_TO_USER_ARG(psignature_length, u4BufVAddr->signature_length, UINT32);
	
	USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, u4BufVAddr->signature_length);
    COPY_TO_USER_ARG_SIZE(psignature, puil_cont_signature, u4BufVAddr->signature_length);

	MTDMX_PRINT("signature_length out =%d\n",*psignature_length);
#if WV_DEBUG_LOG
	printf("signature = \n");
	for(i = 0; i < *psignature_length; i++)
	{
	    printf("%d ", psignature[i]);
	}
	printf("\n");
#endif

s:
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_in_buffer)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_in_buffer));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));

#endif

    return ret;
}

static MT_RESULT_T _MTDRM_WV_Generic_Verify(unsigned long arg)
{
	MT_RESULT_T ret = MTR_OK;

#ifdef CC_TRUSTZONE_SUPPORT
    MTAL_IOCTL_7ARG_T rArg;
	UINT32 asesscnt = 0;
	UINT8* in_buffer;
	UINT32 buffer_length = 0;
	OEMCrypto_Algorithm algorithm = 0;
	UINT8* psignature;
	INT32 signature_length = 0;
	UINT32 curtime = 0;
	WV_IOCTL_GENERIC_T *u4BufVAddr = NULL;
	uint8_t* puil_cont_in_buffer = NULL;
	uint8_t* puil_cont_signature = NULL;

	in_buffer = NULL;
	psignature = NULL;

	MTDMX_PRINT(" Entry\n");

    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
	
	asesscnt = (UINT32)rArg.ai4Arg[0];
    in_buffer = (UINT8*)rArg.ai4Arg[1];
    buffer_length = (UINT32)rArg.ai4Arg[2];
	algorithm = (OEMCrypto_Algorithm)rArg.ai4Arg[3];
	psignature = (UINT8*)rArg.ai4Arg[4];
	signature_length = (UINT32)rArg.ai4Arg[5];
    curtime = (UINT32)rArg.ai4Arg[6];

	
	u4BufVAddr = (WV_IOCTL_GENERIC_T *)VIRTUAL(BSP_AllocAlignedDmaMemory(sizeof(WV_IOCTL_GENERIC_T), TZ_CACHE_LINE_SIZE));
	if (!u4BufVAddr)
	{
		printf("_MTDRM_WV_Generic_Sign Allocate WV_IOCTL_GENERIC_T failed\n");
		return MTR_NOT_OK;
	} 
	u4BufVAddr->SessionContext = (UINT32)asesscnt;

	if(buffer_length != 0)
	{
		puil_cont_in_buffer = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(buffer_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_in_buffer)
		{
			printf("_MTDRM_WV_Generic_Verify alloc in_buffer error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
	    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(in_buffer, buffer_length);
	    COPY_FROM_USER_ARG_SIZE(in_buffer, puil_cont_in_buffer, buffer_length);
	}

	if(signature_length != 0)
	{
		puil_cont_signature = (uint8_t *)VIRTUAL(BSP_AllocAlignedDmaMemory(signature_length, TZ_CACHE_LINE_SIZE));
		if (NULL == puil_cont_signature)
		{
			printf("_MTDRM_WV_Generic_Verify alloc asignature error\n");
			ret = MTR_NOT_OK;	
			goto s;
		}
		USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(psignature, signature_length);
   	 	COPY_FROM_USER_ARG_SIZE(psignature, puil_cont_signature, signature_length);
	}
	
	u4BufVAddr->in_buffer = (uint8_t *)PHYSICAL((UINT32)puil_cont_in_buffer);
	u4BufVAddr->in_buffer_length = buffer_length;
	u4BufVAddr->algorithm = algorithm;
	u4BufVAddr->signature = (uint8_t *)PHYSICAL((UINT32)puil_cont_signature); 
	u4BufVAddr->signature_length = signature_length;
	u4BufVAddr->u4Curtime = curtime;

	if(puil_cont_in_buffer)
		TZ_NWD_BufferEnd((UINT32)puil_cont_in_buffer, TZ_CACHE_ALIGN(buffer_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));

    if (!TZ_DRM_WV_Generic_Verify(u4BufVAddr, sizeof(WV_IOCTL_GENERIC_T)))
    {
    	printf("TZ_DRM_WV_Generic_Verify error\n");
        ret = MTR_NOT_OK;
		goto s;
    }
	
	if(puil_cont_in_buffer)
		TZ_NWD_BufferEnd((UINT32)puil_cont_in_buffer, TZ_CACHE_ALIGN(buffer_length));
	if(puil_cont_signature)
		TZ_NWD_BufferEnd((UINT32)puil_cont_signature, TZ_CACHE_ALIGN(signature_length));
		
s:	
    BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)u4BufVAddr));
	if(puil_cont_in_buffer)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_in_buffer));
	if(puil_cont_signature)
		BSP_FreeAlignedDmaMemory(PHYSICAL((UINT32)puil_cont_signature));

#endif

    return ret;
}

///////////////////////////////////for hulu+dash+wv drm only end//////////////


#define DEVICE_DATA_LEN                    (128)
#define DM_KEY_LEN                         (16)
#define ULPK_LEN                           (32)
#define ROMKEY_LEN                         (32)


static MT_RESULT_T _MTDRM_BimGetDeviceData(unsigned long arg)
{
    return MTR_NOT_OK;
}

static MT_RESULT_T _MTDRM_BimGetDmKey(unsigned long arg)
{
    return MTR_NOT_OK;
}

static MT_RESULT_T _MTDRM_BimGetULPK(unsigned long arg)
{
    return MTR_NOT_OK;
}

static MT_RESULT_T _MTDRM_BimGetRomKey(unsigned long arg)
{
    return MTR_NOT_OK;
}

/**
Input parameter
    pu1KeyBuf : Output key buffer
    pu4OutSize : Output size pointer
Output
„« Success/Failure
Operation
    Read DivX DRM HW Secret key
**/
extern VOID DRV_DIVX_GetHWSecretKeyEx(UINT8 *pu1KeyBuf);
static MT_RESULT_T  _MTDRM_DivX_GetHWSecretKeyEx(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T    rArg;

    //DRV_DIVX_GetHWSecretKeyEx((UINT8 *)&rArg.ai4Arg[0]);
    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_TO_USER_ARG(arg, rArg,MTAL_IOCTL_8ARG_T);

    return MTR_OK;
}

static MT_RESULT_T _MTDMX_SetTsOut(unsigned long arg)
{
#ifdef CC_DMX_ENABLE_TSOUT
    DMX_TSOUT_PATH_T ePath;
    UINT8 u1TsIdx;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    ePath = (DMX_TSOUT_PATH_T)rArg.ai4Arg[1];

    if (!DMX_SetTsOut(u1TsIdx, ePath))
    {
        return MTR_NOT_OK;
    }
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTDMX_SetPTSDrift(unsigned long arg)
{
    UCHAR ucStcId;
    INT32 i4Drift;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucStcId = (UCHAR)rArg.ai4Arg[0];
    i4Drift = (INT32)rArg.ai4Arg[1];

    if (STC_SetPtsDrift(ucStcId, i4Drift) == STC_VALID)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

static MT_RESULT_T _MTDMX_GetHwInfo(unsigned long arg)
{
    MTDMX_HW_INFO_T *prHwInfo = NULL;
    MTDMX_HW_INFO_T rHwInfo;
    UINT32 u4Arg = 0;
    x_memset(&rHwInfo, 0, sizeof(rHwInfo));

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,u4Arg);

    prHwInfo = (MTDMX_HW_INFO_T *)u4Arg;

    rHwInfo.u1PbPidFilterNum = DMX_NUM_PID_INDEX;
    rHwInfo.u1PbSecFilterNum = DMX_NUM_FILTER_INDEX;
    rHwInfo.u1RecPidFilterNum = FVR_NUM_PID_INDEX;
    rHwInfo.u1CaKeyNum = DMX_DMEM_CA_KEY_NUM;
    rHwInfo.u1FramerNum = DMX_FRAMER_COUNT;
    rHwInfo.u1StcNum = DMX_NUM_STC_NUM;

    USR_SPACE_ACCESS_VALIDATE_ARG(prHwInfo, MTDMX_HW_INFO_T);
    COPY_TO_USER_ARG(prHwInfo, rHwInfo, MTDMX_HW_INFO_T);

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_DIVX_GetHwSecretKey0(unsigned long arg)
{
    INT32    Arg;
    UINT8 *pu1Buf;
    UINT8 au1Data[ROMKEY_LEN];

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    pu1Buf = (UINT8 *)Arg;

    HalFlushInvalidateDCacheMultipleLine((UINT32)au1Data, ROMKEY_LEN);

    #if defined(CC_TRUSTZONE_SUPPORT)
    if (!TZ_DRM_Divx_GetHwSecretKey0(au1Data))
    {
        return MTR_NOT_OK;
    }
    #else
    x_memcpy(pu1Buf, (void *)(0xf4003c00), ROMKEY_LEN);
    #endif

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Buf, ROMKEY_LEN);
    COPY_TO_USER_ARG_SIZE(pu1Buf, au1Data, ROMKEY_LEN);

    return MTR_OK;
}

static MT_RESULT_T _MTDRM_DIVX_GetHwSecretKey1(unsigned long arg)
{
    INT32    Arg;
    UINT8 *pu1Buf;
    UINT8 au1Data[ROMKEY_LEN];

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    pu1Buf = (UINT8 *)Arg;

    HalFlushInvalidateDCacheMultipleLine((UINT32)au1Data, ROMKEY_LEN);

    #if defined(CC_TRUSTZONE_SUPPORT)
    if (!TZ_DRM_Divx_GetHwSecretKey1(au1Data))
    {
        return MTR_NOT_OK;
    }
    #else
    x_memcpy(pu1Buf, (void *)(0xf4003c20), ROMKEY_LEN);
    #endif

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Buf, ROMKEY_LEN);
    COPY_TO_USER_ARG_SIZE(pu1Buf, au1Data, ROMKEY_LEN);

    return MTR_OK;
}

#ifdef CC_EIT_SECTION_FILTER
static MT_RESULT_T _MTDMX_SetEITSection(unsigned long arg)
{
    MTDMX_SECTION_KEY_SET_T rMtSetSection;
    SECTION_KEY_SET_T rSetSection;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg,MTDMX_SECTION_KEY_SET_T);
    COPY_FROM_USER_ARG(arg,rMtSetSection,MTDMX_SECTION_KEY_SET_T);

    rSetSection.eSetType = rMtSetSection.eSetType;
    rSetSection.fgSet = rMtSetSection.fgSet;
    rSetSection.tSecKey.e_key.ui2_onid = rMtSetSection.tSecKey.e_key.ui2_onid;
    rSetSection.tSecKey.e_key.ui2_svcid = rMtSetSection.tSecKey.e_key.ui2_svcid;
    rSetSection.tSecKey.e_key.ui2_tsid = rMtSetSection.tSecKey.e_key.ui2_tsid;
    rSetSection.tSecKey.ui1_tableid = rMtSetSection.tSecKey.ui1_tableid;
    rSetSection.tSecKey.ui1_version = rMtSetSection.tSecKey.ui1_version;
    rSetSection.tSecKey.ui1_secnum = rMtSetSection.tSecKey.ui1_secnum;

    if(!DMX_SetEITSection(&rSetSection))
    {
        return MTR_NOT_OK;
    }
    return MTR_OK;
}
#endif

//-----------------------------------------------------------------------------
/** Get DMXPID buff VIRTUAL addr and size
 *
 *  @param  prDmxPidInfo         The buff info
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDMX_GetDMXPIDInfo(unsigned long arg)
{
    MTDMX_DMXPID_INFO_T *prDmxPidInfo = NULL;
    MTDMX_DMXPID_INFO_T rDmxPidInfo;
    UINT32 u4Arg = 0;
    FBM_POOL_T* prFbmPool;
    
    x_memset(&rDmxPidInfo, 0, sizeof(rDmxPidInfo));

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,u4Arg);

    prDmxPidInfo = (MTDMX_DMXPID_INFO_T *)u4Arg;

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMXPID);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    rDmxPidInfo.u4Addr = VIRTUAL(prFbmPool->u4Addr&PAGE_MASK);
    rDmxPidInfo.u4Size = (prFbmPool->u4Size + (PAGE_SIZE - 1)) & PAGE_MASK;

    USR_SPACE_ACCESS_VALIDATE_ARG(prDmxPidInfo, MTDMX_DMXPID_INFO_T);
    COPY_TO_USER_ARG(prDmxPidInfo, rDmxPidInfo, MTDMX_DMXPID_INFO_T);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
int mtal_ioctl_mtdmx(struct inode *inode, struct file *file, unsigned int cmd,
                     unsigned long arg)
{
    INT32 i4Ret = MTR_OK;

#ifdef TIME_SHIFT_SUPPORT
    if ((i4Ret = mtal_ioctl_mtdrec(cmd, arg)) != MTR_NOT_SUPPORTED)
    {
        return i4Ret;
    }
#endif

    switch (cmd)
    {
    case MTAL_IO_DMX_INIT:
        i4Ret = _MTDMX_Init(arg);
        break;

    case MTAL_IO_DMX_RESET:
        i4Ret = _MTDMX_Reset(arg);
        break;

    case MTAL_IO_DMX_SETFRONTENDEX:
        i4Ret = _MTDMX_SetFrontEndEx(arg);
        break;

    case MTAL_IO_DMX_SETVIDEOTYPE:
        i4Ret = _MTDMX_SetVideoType(arg);
        break;
    case MTAL_IO_DMX_SETVIDEOTYPEEX:
        i4Ret = _MTDMX_SetVideoTypeEx(arg);
        break;
    case MTAL_IO_DMX_SETVIDEOTYPEBYPIDX:
        i4Ret = _DMX_SetVideoTypeByPidx(arg);
        break;
    case MTAL_IO_DMX_SETPKTSIZE:
		i4Ret = _MTDMX_SetFramerPkt(arg);
        break;
    case MTAL_IO_DMX_START:
        i4Ret = _MTDMX_Start(arg);
        break;

    case MTAL_IO_DMX_STOP:
        i4Ret = _MTDMX_Stop(arg);
        break;

    case MTAL_IO_DMX_SETPID:
        i4Ret = _MTDMX_SetPid(arg);
        break;

    case MTAL_IO_DMX_SETFILEPID:
        i4Ret = _MTDMX_SetFilePid(arg);
        break;

    case MTAL_IO_DMX_GETPID:
        i4Ret = _MTDMX_GetPid(arg);
        break;

    case MTAL_IO_DMX_FREEPID:
        i4Ret = _MTDMX_FreePid(arg);
        break;

    case MTAL_IO_DMX_GETPIDIDX:
        i4Ret = _MTDMX_GetPidIndex(arg);
        break;

    case MTAL_IO_DMX_GETESFIFO:
        i4Ret = _MTDMX_Get_ESFifoSize(arg);
        break;

    case MTAL_IO_DMX_GETPACKETSTATUS:
        i4Ret = _MTDMX_GetPacketStatus(arg);
        break;

    case MTAL_IO_DMX_SETVDEPID:
        i4Ret = _MTDMX_SetVideoPid(arg);
        break;

    case MTAL_IO_DMX_SETAUDPID:
        i4Ret = _MTDMX_SetAudioPid(arg);
        break;

    case MTAL_IO_DMX_SETAUDDESPID:
        i4Ret = _MTDMX_SetAudioDescPid(arg);
        break;

    case MTAL_IO_DMX_SETPCRPIDEX:
        i4Ret = _MTDMX_SetPcrPidEx(arg);
        break;

    case MTAL_IO_DMX_SETPCR:
        i4Ret = _MTDMX_SetPcr(arg);
        break;

    case MTAL_IO_DMX_SETFTR:
        i4Ret = _MTDMX_SetFilter(arg);
        break;

    case MTAL_IO_DMX_GETFTR:
        i4Ret = _MTDMX_GetFilter(arg);
        break;

    case MTAL_IO_DMX_PSI_ALCBFR:
        i4Ret = _MTDMX_PSI_AllocateBuffer(arg);
        break;

    case MTAL_IO_DMX_PSI_FREEBFR:
        i4Ret = _MTDMX_PSI_FreeBuffer(arg);
        break;

    case MTAL_IO_DMX_PSI_GETBFR:
        i4Ret = _MTDMX_PSI_GetBuffer(arg);
        break;

    case MTAL_IO_DMX_PSI_ULKBFR:
        i4Ret = _MTDMX_PSI_UnlockBuffer(arg);
        break;

    case MTAL_IO_DMX_PSI_FLSBFR:
        i4Ret = _MTDMX_PSI_FlushBuffer(arg);
        break;

    case MTAL_IO_DMX_PSI_GETBFRSECHEAD:
        i4Ret = _MTDMX_PSI_GetBufferSectionHeader(arg);
        break;

    case MTAL_IO_DMX_PES_ALKBFR:
        i4Ret = _MTDMX_PES_AllocateBuffer(arg);
        break;

    case MTAL_IO_DMX_PES_FREEBFR:
        i4Ret = _MTDMX_PES_FreeBuffer(arg);
        break;

    case MTAL_IO_DMX_PES_ULKBFR:
        i4Ret = _MTDMX_PES_UnlockBuffer(arg);
        break;

    case MTAL_IO_DMX_PES_FLSBFR:
        i4Ret = _MTDMX_PES_FlushBuffer(arg);
        break;

    case MTAL_IO_DMX_PES_GETBFR:
        i4Ret = _MTDMX_PES_GetBuffer(arg);
        break;

    case MTAL_IO_DMX_RAWTS_GETBFR:
        i4Ret = _MTDMX_RawTs_GetBuffer(arg);
        break;

    case MTAL_IO_DMX_RAWTS_ULKBFR:
        i4Ret = _MTDMX_RawTs_UnlockBuffer(arg);
        break;

    case MTAL_IO_DMX_GETPIDCNT:
        i4Ret = _MTDMX_GetPidCounters(arg);
        break;

    case MTAL_IO_DMX_RSTPIDCNT:
        i4Ret = _MTDMX_ResetPidCounters(arg);
        break;

    case MTAL_IO_DMX_GETLOKSTE:
        i4Ret = _MTDMX_GetLockState(arg);
        break;

    case MTAL_IO_DMX_GETSCRMSTE:
        i4Ret = _MTDMX_GetScrambleState(arg);
        break;

    case MTAL_IO_DMX_GETRAWSCRMSTE:
        i4Ret = _MTDMX_GetRawScrambleState(arg);
        break;

    case MTAL_IO_DMX_GETCURSTC:
        i4Ret = _MTDMX_GetCurSTC(arg);
        break;

    case MTAL_IO_DMX_SETGENERICFILTER:
        i4Ret = _MTDMX_SetGenericFilter(arg);
        break;

    case MTAL_IO_DMX_GETGENERICFILTER:
        i4Ret = _MTDMX_GetGenFilter(arg);
        break;

    case MTAL_IO_DMX_CONNECT_FILTER:
        i4Ret = _MTDMX_ConnectFilter(arg);
        break;
        
    case MTAL_IO_DMX_CONNECT_AVAILABLEFILTER:
        i4Ret = _MTDMX_ConnectAvailableFilterx(arg);
        break;
        
    case MTAL_IO_DMX_DISCONNECT_FILTER:
        i4Ret = _MTDMX_DisConnectFilter(arg);
        break;
        
    case MTAL_IO_DMX_SETDESKEY:
        i4Ret = _MTDMX_SetDesKey(arg);
        break;

    case MTAL_IO_DMX_SETDESEVENODDIV:
        i4Ret = _MTDMX_SetDesOddEvenIV(arg);
        break;

    case MTAL_IO_DMX_SETRTBMODE:
        i4Ret = _MTDMX_SetRtbMode(arg);
        break;

    case MTAL_IO_DMX_SETAESKEYLEN:
        i4Ret = _MTDMX_SetAesKeyLen(arg);
        break;

    case MTAL_IO_DMX_SETAESKEY:
        i4Ret = _MTDMX_SetAesKey(arg);
        break;

        //DVB key and IV
    case MTAL_IO_DMX_DVB_KEY:
        i4Ret = _MTDMX_SetDVBKey(arg);
        break;

    case MTAL_IO_DMX_DVB_IV:
        i4Ret = _MTDMX_SetDVBIV(arg);
        break;

    case MTAL_IO_DMX_SETCRYPTOKEY:
        i4Ret = _MTDMX_SetCryptoKey(arg);
        break;

    case MTAL_IO_DMX_SETAESEVENODDIV:
        i4Ret = _MTDMX_SetAesOddEvenIV(arg);
        break;

    case MTAL_IO_DMX_SETDESCPID:
        i4Ret = _MTDMX_SetDescramblPid(arg);
        break;

    case MTAL_IO_DMX_SETFILEDESCPID:
        i4Ret = _MTDMX_SetFileDescramblPid(arg);
        break;

#ifdef ENABLE_MULTIMEDIA
    case MTAL_IO_DMX_AES_SETDCRINFO:
        i4Ret = _MTDMX_AES_SetDecryptInfo(arg);
        break;
#endif

#ifdef ENABLE_MULTIMEDIA
    case MTAL_IO_DMX_AES_MVBLKDATA:
        i4Ret = _MTDMX_AES_MoveBlockData(arg);
        break;

    case MTAL_IO_DMX_AES_ALLOCENGINE:
        i4Ret = _MTDMX_AES_AllocEngine(arg);
        break;

    case MTAL_IO_DMX_AES_FREEENGINE:
        i4Ret = _MTDMX_AES_FreeEngine(arg);
        break;

    case MTAL_IO_DMX_AES_SETDCRINFOEX:
        i4Ret = _MTDMX_AES_SetDecryptInfoEx(arg);
        break;

    case MTAL_IO_DMX_AES_MVBLKDATAEX:
        i4Ret = _MTDMX_AES_MoveBlockDataEx(arg);
        break;
#endif

    case MTAL_IO_DMX_GCPU_CMD:
        i4Ret = _MTDMX_GCPU_Cmd(arg);
        break;

    case MTAL_IO_DMX_SETPID_INSRC:
        i4Ret = _MTDMX_SetPidInputSource(arg);
        break;

    case MTAL_IO_DMX_MULTI2_CFG:
        i4Ret = _MTDMX_SetMulti2Config(arg);
        break;

    case MTAL_IO_DMX_MULTI2_KEY:
        i4Ret = _MTDMX_SetMulti2Key(arg);
        break;

    case MTAL_IO_DMX_MULTI2_CBCIV:
        i4Ret = _MTDMX_SetMulti2CbcIV(arg);
        break;

    case MTAL_IO_DMX_MULTI2_OFBIV:
        i4Ret = _MTDMX_SetMulti2OfbIV(arg);
        break;

    case MTAL_IO_DMX_MULTI2_SYSKEY:
        i4Ret = _MTDMX_SetMulti2SysKey(arg);
        break;

    case MTAL_IO_DRM_WV_SET_ASSET_KEY:
        i4Ret = _MTDRM_WVSetAssetKey(arg);
        break;

    case MTAL_IO_DRM_WV_DERIVE_CW:
        i4Ret = _MTDRM_WVDeriveCW(arg);
        break;

    case MTAL_IO_DRM_WV_PASS_KEYBOX:
        i4Ret = _MTDRM_WVPassKeybox(arg);
        break;

    case MTAL_IO_DRM_WV_DECRYPTION:
        i4Ret = _MTDRM_WVDataDecrypt(arg);
        break;

    case MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY_EX:
        i4Ret = _MTDRM_DivX_GetHWSecretKeyEx(arg);
        break;

    case MTAL_IO_BIM_GET_DEVICE_DATA:
        i4Ret = _MTDRM_BimGetDeviceData(arg);
        break;

    case MTAL_IO_BIM_DM_GET_KEY:
        i4Ret = _MTDRM_BimGetDmKey(arg);
        break;

    case MTAL_IO_BIM_GET_ULPK:
        i4Ret = _MTDRM_BimGetULPK(arg);
        break;

    case MTAL_IO_BIM_GET_ROMKEY:
        i4Ret = _MTDRM_BimGetRomKey(arg);
        break;

    case MTAL_IO_DMX_SETTSOUT:
        i4Ret = _MTDMX_SetTsOut(arg);
        break;

    case MTAL_IO_DMX_SETPTSDRIFT:
        i4Ret = _MTDMX_SetPTSDrift(arg);
        break;

    case MTAL_IO_DMX_MWIF_CALL:
        i4Ret = MTDMX_MWIF_Call(arg);
        break;

    case MTAL_IO_DMX_CONNECT:
        i4Ret = _MTDMX_ConnectInfo(arg);
        break;

    case MTAL_IO_DMX_CONNECT_AVAILABLEPID:
        i4Ret = _MTDMX_ConnectAvailablePidx(arg);
        break;

    case MTAL_IO_DMX_DISCONNECT:
        i4Ret = _MTDMX_DisConnectInfo(arg);
        break;

    case MTAL_IO_DMX_GETHWINFO:
           i4Ret = _MTDMX_GetHwInfo(arg);
           break;

	case MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY0:
		i4Ret = _MTDRM_DIVX_GetHwSecretKey0(arg);
		break;
	case MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY1:
		i4Ret = _MTDRM_DIVX_GetHwSecretKey1(arg);
		break;

    case MTAL_IO_DMX_RELOADSECUREKEY:
        i4Ret = _MTDMX_ReloadSecureKey(arg);
        break;
        
    case MTAL_IO_DMX_GETSTCID:
        i4Ret = _MTDMX_GetStcId(arg);
        break;

#ifdef CC_EIT_SECTION_FILTER
    case MTAL_IO_DMX_SET_EITSECTION:
        i4Ret = _MTDMX_SetEITSection(arg);
        break;
#endif
///////////////////////////////////for hulu+dash+wv drm only begin//////////////		
	case MTAL_IO_DRM_WV_VALIDATE_KEYBOX:
		i4Ret = _MTDRM_WV_ValidateKeybox(arg);
		break;
		
	case MTAL_IO_DRM_WV_DESTROYSESSION:
		i4Ret = _MTDRM_WV_DestroySession(arg);
		break;	
		
	case MTAL_IO_DRM_WV_CREATESESSION:
		i4Ret = _MTDRM_WV_CreatSession(arg);
		break;

	case MTAL_IO_DRM_WV_GENERATE_NONCE:
		i4Ret = _MTDRM_WV_GenerateNonceSess(arg);
		break;

	case MTAL_IO_DRM_WV_GENERATE_DERIVEKEY:
		i4Ret = _MTDRM_WV_GenerateDeriveKeys(arg);
		break;

	case MTAL_IO_DRM_WV_GENERATE_SIGNATURE:
		i4Ret = _MTDRM_WV_GenerateSignature(arg);
		break;

	case MTAL_IO_DRM_WV_VALIDATEM_MESSAGE:
		i4Ret = _MTDRM_WV_ValidateMessage(arg);
		break;

	case MTAL_IO_DRM_WV_INSTALL_KEY:
		i4Ret = _MTDRM_WV_InstallKey(arg);
		break;

	case MTAL_IO_DRM_WV_FLUSH_NONCE:
		i4Ret = _MTDRM_WV_FlushNoncesSess(arg);
		break;

	case MTAL_IO_DRM_WV_UPDATE_MACKEY:
		i4Ret = _MTDRM_WV_UpdateMacKeys(arg);
		break;

	case MTAL_IO_DRM_WV_REFRESH_KEY:
		i4Ret = _MTDRM_WV_RefreshKey(arg);
		break;

	case MTAL_IO_DRM_WV_SELECT_CONTENT_KEY:
		i4Ret = _MTDRM_WV_SelectContentKey(arg);
		break;

	case MTAL_IO_DRM_WV_DECRYPT_CTR:
		i4Ret = _MTDRM_WV_DecryptCTR(arg);
		break;

	case MTAL_IO_DRM_WV_SAVE_KEYBOX:
		i4Ret = _MTDRM_WV_SaveKeybox(arg);
		break;

	case MTAL_IO_DRM_WV_GET_RANDOM_NUM:
		i4Ret = _MTDRM_WV_GetRandomNum(arg);
		break;

	case MTAL_IO_DRM_WV_REWRAP_RSAKEY:
		i4Ret = _MTDRM_WV_RewrapDeviceRSAKeySess(arg);
		break;

	case MTAL_IO_DRM_WV_LOAD_RSAKET:
		i4Ret = _MTDRM_WV_LoadDeviceRSAKeySessCtx(arg);
		break;

	case MTAL_IO_DRM_WV_GENERATE_RSASIGN:
		i4Ret = _MTDRM_WV_GenerateRSASignature(arg);
		break;

	case MTAL_IO_DRM_WV_RSA_DERIVEKEY:
		i4Ret = _MTDRM_WV_RSADeriveKeys(arg);
		break;

	case MTAL_IO_DRM_WV_GENERIC_ENC:
		i4Ret = _MTDRM_WV_Generic_Encrypt(arg);
		break;

	case MTAL_IO_DRM_WV_GENERIC_DEC:
		i4Ret = _MTDRM_WV_Generic_Decrypt(arg);
		break;

	case MTAL_IO_DRM_WV_GENERIC_SIGN:
		i4Ret = _MTDRM_WV_Generic_Sign(arg);
		break;

	case MTAL_IO_DRM_WV_GENERIC_VERIFY:
		i4Ret = _MTDRM_WV_Generic_Verify(arg);
		break;
///////////////////////////////////for hulu+dash+wv drm only end//////////////
    case MTAL_IO_DMX_GETDMXPIDINFO:
        i4Ret = _MTDMX_GetDMXPIDInfo(arg);
        break;
#ifdef TIME_SHIFT_SUPPORT
    default:
        i4Ret = mtal_ioctl_mttsrec(cmd, arg);
        break;
#endif

    }

    return i4Ret;
}


//-----------------------------------------------------------------------------
/** MTDMX_SetPCR
 */
//-----------------------------------------------------------------------------
BOOL MTDMX_SetPCR(UINT8 u1Pidx, UINT16 u2PidNum, MTDMX_PID_INSRC_T eInType, BOOL fgEnable)
{
    DMX_PID_T rPid;

    x_memset((VOID*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgAllocateBuffer = FALSE;
    rPid.u2Pid = u2PidNum;
    //rPid.u4BufAddr = 0x0;
    //rPid.u4BufSize = 0x0;
    rPid.ePidType = DMX_PID_TYPE_NONE;
    rPid.fgPrimary = FALSE;
    rPid.ePcrMode = DMX_PCR_MODE_NEW;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.fgEnable = fgEnable;

    if (eInType == MTDMX_PID_INSRC_DTV)
    {
        if (!_MtdmxSetPid(u1Pidx, DMX_PID_FLAG_ALL & (~DMX_PID_FLAG_BUFFER), &rPid))
        {
            return FALSE;
        }
    }
    else if (eInType == MTDMX_PID_INSRC_TIMESHIFT_PLAY)
    {
        rPid.u1TsIndex = 2;
        if (!DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL & (~DMX_PID_FLAG_BUFFER), &rPid))
        {
            return FALSE;
        }
    }
    else if (eInType == MTDMX_PID_INSRC_MM)
    {
        rPid.u1TsIndex = 0;
        if (!DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL & (~DMX_PID_FLAG_BUFFER), &rPid))
        {
            return FALSE;
        }
    }
    else
    {
        return  FALSE;
    }

    if (fgEnable)
    {
        if (DMX_SetStcSrc(u1Pidx, 0) != 0)   // Force use STC 0
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        if (DMX_SetStcSrc(u1Pidx, STC_SRC_NS) != 0)   // Set to other PCR to disable
        {
            return MTR_NOT_OK;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** MTDMX_SetSecondVideoPid
 */
//-----------------------------------------------------------------------------
BOOL MTDMX_SetSecondVideoPid(UINT8 u1Pidx, UINT32 u4Flags, MTDMX_PID_T* prPid)
{
    MTDMX_PID_T rPid;
    DMX_PID_T rDrvPid;
    //FBM_POOL_T* prFbmPool;
    UINT32 u4VfifoAddr = 0, u4VfifoSize = 0;

    rPid = *prPid;

    // Only enable/disable PID
    if (u4Flags == MTDMX_PID_FLAG_VALID)
    {
        rDrvPid.fgEnable = rPid.fgEnable;
        if (!_MtdmxSetPid(u1Pidx, u4Flags, &rDrvPid))
        {
            return FALSE;
        }

        return TRUE;
    }

    // Get from MW
    rDrvPid.fgEnable = rPid.fgEnable;
    rDrvPid.fgAllocateBuffer = rPid.fgAllocateBuffer;
    rDrvPid.u2Pid = rPid.u2Pid;
    rDrvPid.u4BufAddr = rPid.u4BufAddr;
    rDrvPid.u4BufSize = rPid.u4BufSize;
    rDrvPid.u1DeviceId = 0;
    rDrvPid.ePidType = rPid.ePidType;

    if (rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        // Allocate video buffer from FBM
        if((!fgFbmDmx1Used) && (!fgFbmDmx2Used))
        {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
            prFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
            prFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
            ASSERT(prFbmPool != NULL);
            if (!prFbmPool)
            {
                Printf("Fail to get VFIFO!\n");
                return MTR_NOT_OK;
            }
        }

        if(rPid.u1ChannelId == 0)
        {
        	_IoMtDmx_Lock();
            fgFbmDmx1Used = TRUE;
			_IoMtDmx_Unlock();
        }
        else
        {
        	_IoMtDmx_Lock();
            fgFbmDmx2Used = TRUE;
			_IoMtDmx_Unlock();
        }

        u4VfifoAddr = prFbmPool->u4Addr;
        u4VfifoSize = prFbmPool->u4Size;
        
        ASSERT(u4VfifoAddr != 0);
        if(u4VfifoSize < MTAL_DMX_EXPECTED_SIZE)
        {
            MTDMX_PRINT("DMX buffer size is too smaler: 0x%x!\n", u4VfifoSize);
        }

        // Setup buffer attributes
        rDrvPid.fgAllocateBuffer = FALSE;
        rDrvPid.u4BufAddr = u4VfifoAddr + (u4VfifoSize / 2);
        rDrvPid.u4BufSize = u4VfifoSize / 2;
        //rDrvPid.u1DeviceId = rPid.u1DeviceId;
    }

    if ((rPid.pfnNotify != NULL) && (u4Flags & MTDMX_PID_FLAG_CALLBACK))
    {
        rDrvPid.pfnNotify = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnNotify = NULL;
    }

    if ((rPid.pfnScramble != NULL) && (u4Flags & DMX_PID_FLAG_SCRAMBLE_STATE))
    {
        rDrvPid.pfnScramble = _MtDmxCallback;
    }
    else
    {
        rDrvPid.pfnScramble = NULL;
    }

    rDrvPid.pvNotifyTag = NULL;
    rDrvPid.pvScrambleTag = NULL;

    // Set by driver only
    rDrvPid.fgPrimary = TRUE;
    rDrvPid.u1TsIndex = rPid.u1TsIndex;
    rDrvPid.u1KeyIndex = 0x0;
    rDrvPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rDrvPid.ePcrMode = DMX_PCR_MODE_NONE;
    rDrvPid.eDescMode = DMX_DESC_MODE_NONE;
    rDrvPid.u1ChannelId = 0x1;

    u4Flags |= (DMX_PID_FLAG_PRIMARY | DMX_PID_FLAG_TS_INDEX | DMX_PID_FLAG_KEY_INDEX |
                DMX_PID_FLAG_DEVICE_ID | DMX_PID_FLAG_STEER | DMX_PID_FLAG_PCR |
                DMX_PID_FLAG_DESC_MODE | DMX_PID_FLAG_SCRAMBLE_STATE);

    if (!_MtdmxSetPid(u1Pidx, u4Flags, &rDrvPid))
    {
        return FALSE;
    }

    return TRUE;
}

VOID DmxGetCallbacks(DMX_DECODER_CALLBACKS_T *pDecoderCallbacks)
{
   pDecoderCallbacks->pfnSendVideoPes=_MwDmxVideoCallback;
   pDecoderCallbacks->pfnUpdateVideoWp = VDEC_SetWptr;
   pDecoderCallbacks->pfnSendAudioPes = AUD_SendAudioPes;
   return;
}


