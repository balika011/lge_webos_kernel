/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */


#ifndef VDPOMX_DRV_IF_H
#define VDPOMX_DRV_IF_H

/*=============================================================================
 *                              Include Files
 *===========================================================================*/
#include <x_typedef.h>
#include <linux/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
 *                              Definition
 *===========================================================================*/
/**Video plane region (source/output).
*/
typedef struct
{
    UINT32 u4X;        	///< start position in x direction (u4X/10000 = start pixel/total pixel)
    UINT32 u4Y;        	///< start position in y direction (u4Y/10000 = start pixel/total pixel)
    UINT32 u4Width;  	///< width in percentage (u4Width/10000 = region width/total width)
    UINT32 u4Height; 	///< height in percentage (u4Width/10000 = region height/total width)
}VDPOMX_REGION_T;

/**Video plane overscan region.
*/
typedef struct
{
    UINT32 u4Top;      	///<overscan percentage on top side (u4Top%)
    UINT32 u4Bottom;	///<overscan percentage on top side (u4Bottom%)
    UINT32 u4Left;     	///<overscan percentage on top side (u4Left%)
    UINT32 u4Right;   	///<overscan percentage on top side (u4Right%)
}VDPOMX_OVERSCAN_REGION_T;

typedef struct
{
    UCHAR  ucChId ;
    VDPOMX_REGION_T rRegion ;
}VDPOMX_SET_REGION_PARM_T;

typedef struct
{
    UCHAR  ucChId ;
    BOOL    fgOnOff ;
}VDPOMX_SET_CHANNEL_PARM_T;

typedef struct
{
    UCHAR ucChId;
    UINT32 u4Addr;
}VDPOMX_SET_DISPLAY_BUF_PARM_T;

typedef struct
{
    UCHAR ucChId;
    BOOL fgFree;
}VDPOMX_GET_CHANNEL_FREE_PARM_T;

typedef struct
{
    UCHAR ucChId;
    UINT32 u4Buf;
    BOOL fgLocked;
}VDPOMX_GET_BUF_LOCKED_PARM_T;

typedef struct
{
    UCHAR ucChId;
    UINT32 u4AudioHwSync;
}VDPOMX_TUNNEL_CONNECT_AUD_PARM_T;

typedef struct
{
    UCHAR ucChId;
    UCHAR ucVdecId;
    UCHAR ucPort;
}VDPOMX_CONNECT_VDEC_PARM_T;

enum DISP_CHANNEL
{
    VDP_DISP_CHANNEL_0,
    VDP_DISP_CHANNEL_1,
    VDP_DISP_CHANNEL_NS
};

#define VDPOMX_DEVICE "/dev/vdpomx"

#define VDPOMX_IOC_MAGIC    'k'                                                                                
#define VDPOMX_SET_OUT_REGION              _IOWR(VDPOMX_IOC_MAGIC, 0, long)
#define VDPOMX_SET_SRC_REGION              _IOWR(VDPOMX_IOC_MAGIC, 1, long)
#define VDPOMX_SET_OUT_CHANNEL             _IOWR(VDPOMX_IOC_MAGIC, 2, long)                                    
#define VDPOMX_SET_DISPLAY_BUF             _IOWR(VDPOMX_IOC_MAGIC, 3, long)                                    
#define VDPOMX_GET_CHANNEL_FREE            _IOWR(VDPOMX_IOC_MAGIC, 4, long)
#define VDPOMX_GET_BUF_LOCKED              _IOWR(VDPOMX_IOC_MAGIC, 5, long)
//#define VDPOMX_TUNNEL_CONNECT_AUDIO        _IOWR(VDPOMX_IOC_MAGIC, 6, long)
//#define VDPOMX_CONNECT_VDEC                _IOWR(VDPOMX_IOC_MAGIC, 7, long)
#define VDPOMX_SRM_SETTVMODE               _IOWR(VDPOMX_IOC_MAGIC, 8, long)


#define VDPOMX_OK                0
#define VDPOMX_ERR            (-1)

#ifdef __KERNEL__
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("vdpomx_ioctl argument error\n");     \
        return -1;                                          \
    }                                                           

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("vdpomx_ioctl argument error\n");      \
        return -1;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)             \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(type)))                        \
        {                                                       \
            printk("vdpomx_ioctl argument error\n");      \
            return -1;                                          \
        }       
#endif


/*=============================================================================
 *                             Function Declaration
 *===========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* VDPOMX_DRV_IF_H */

