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


#ifndef _KMEM_H_
#define _KMEM_H_


#include <linux/ioctl.h>
#include "u_handle2.h"

//fix89 #define DEV_KMEM        "/dev/kmem2"

#define IOCTL_KMEM_SET_MMAP_HANDLES     _IOW('k', 0, void *)
#define IOCTL_KMEM_GET_SIZE             _IOR('k', 1, size_t)
#define IOCTL_KMEM_GET_PAGE_OFFSET      _IOR('k', 2, size_t)
#define IOCTL_KMEM_ALIGNED_FREE         _IOW('k', 3, HANDLE_T)
#define IOCTL_KMEM_ALIGNED_ALLOC        _IOWR('k', 4, KMEM_ALIGNED_ALLOC_T)
#define IOCTL_KMEM_SLICED_ALIGNED_ALLOC _IOWR('k', 5, KMEM_SLICED_ALIGNED_ALLOC_T)
#define IOCTL_KMEM_GET_MMAP_HANDLES     _IOR('k', 6, void *)
/* add for map phy_addr to get chip info */
#define IOCTL_KMEM_GET_CHIP_INFO        _IOR('k', 7, void *)
/* add end for map phy_addr to get chip info */


typedef struct
{
    int n;
    HANDLE_T *handles;
} KMEM_MMAP_INFO_T;

typedef union
{
    struct
    {
        SIZE_T z_size;
        UINT32 u4Align;
    } in;
    struct
    {
        HANDLE_T handle;
    } out;
} KMEM_ALIGNED_ALLOC_T;

/* add for get chip info */
typedef struct
{
    HANDLE_T handle;
} KMEM_HANDLE_T;
/* add end for get chip info */


typedef union
{
    struct
    {
        UINT32 z_size;
        SIZE_T z_size_element;
        UINT32 u4Align;
    } in;
    struct
    {
        HANDLE_T handle;
    } out;
} KMEM_SLICED_ALIGNED_ALLOC_T;


#endif /* _KMEM_H_ */

