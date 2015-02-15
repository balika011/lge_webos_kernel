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
/*
 * Hardware specific code is based on MT53xx driver
 * Framebuffer framework code is based on documents on net
 *
 */


//---------------------------------------------------------------------------
// Header files
//---------------------------------------------------------------------------
#include <stddef.h>                    // for size_t    
#include "mt53_gal.h"
#include "mt53_dlmalloc.h"
#include "x_os.h"
#include "x_printf.h"
#include "gfx_if.h"

#include "pmx_drvif.h"
//#include "osd_drvif.h"
//#include "osd_if.h"

#include "fbm_drvif.h"

#include "drvcust_if.h"
#include "x_hal_arm.h"

#ifdef CC_SUPPORT_TVE_OSD_OUT
#include "tve_ctrl.h"
#endif
#include "panel.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <mach/mt53xx_linux.h>

#ifndef ASSERT
	#ifdef _DEBUG_
		#include <assert.h>
		#define ASSERT(x)				assert((x))
	#else
		#define ASSERT(x)
	#endif	// _DEBUG_
#endif	// ASSERT

UINT32 _u4Posd0Rgn0, _u4Posd0Rgn1;
UINT32 _u4Posd1Rgn0, _u4Posd1Rgn1;
static UINT32 _u4Posd0List0, _u4Posd0List1;
static UINT32 _u4Posd1List0, _u4Posd1List1;
static UINT32 _u4Posd2Rgn, _u4Posd2List;
static UINT32 u4PdpPosX = 0;
static UINT32 u4PdpPosY = 0;
static void  *u4LastDLFreePtr = NULL;
static UINT8 _mt3dMode = 0;
static UINT8 _mt3dModeApplied = 0;
static UINT8 _mt3dType[2] = {MT_GAL_3D_DISABLE, MT_GAL_3D_DISABLE};
static DEFINE_TIMER(debug_timer, NULL, 0, (unsigned long)NULL);

#define MT_GAL_STEREO_UI_WIDTH 1280
#define MT_GAL_STEREO_UI_HEIGHT 720
#define MT_GAL_STEREO_LCD_WIDTH 640
#define MT_GAL_STEREO_LCD_HEIGT 540
#define MT_GAL_STEREO_HDPDP_WIDTH 640
#define MT_GAL_STEREO_HDPDP_HEIGHT 720
#define MT_GAL_LCD_PANEL_WIDTH 1920
#define MT_GAL_LCD_PANEL_HEIGHT 1080
#define MT_GAL_HD_PDP_PANEL_WIDTH 1024
#define MT_GAL_HD_PDP_PANEL_HEIGHT 768

#define MT_GAL_STEREO_OSD_SCALER OSD_SCALER_3
#define MT_GAL_STEREO_OSD OSD_PLANE_3
#define MT_GAL_CURSOR_IMPROVE

#define MT_GAL_CURSOR_WIDTH 256
#define MT_GAL_CURSOR_HEIGHT 256

#define MT_GAL_SWAP(a, b, type)		do {\
									type tmp; \
									tmp = b, b = a,	a = tmp; \
									} while(0)


#if 0
typedef struct
{
	UINT32					offset;				/**< bytes offset from the start of graphic memory */
	UINT32					virtualAddress;   /**< virtual address of surface */
	UINT32					physicalAddress;  /**< physical address of surface */
	UINT16					pitch;				/**< pitch: length of horizontal line */
	UINT16					bpp;				/**< bits per pixel */
	UINT16					width;				/**< width of surface */
	UINT16					height;				/**< height of surface */
	MT_GAL_PIXEL_FORMAT_T	pixelFormat;		/**< pixel format of surface */
	MT_GAL_PALETTE_INFO_T	paletteInfo;		/**< palette information, this field is used when the surface's pixel format is based on indexed color. */
	signed int					id;					/**< surface identifier */
	signed int 					pid;				/**< request process id */
	UINT32					property;			/**< reserved for future use */
} MT_GAL_SURFACE_INFO_T;
#endif

typedef struct _mt_gal_surface
{
	struct list_head list_item;
	MT_GAL_SURFACE_INFO_T* surfaceInfo;
	UINT32		offset;
	UINT32		addr; //virtual address where surface is allocated.
	UINT32		size; // the size of allocated surface.
	UINT16		layer;
	UINT16		reference_count;
	signed int	id;
	int			sockId; /** socket id */
	pid_t		taskId; /** calling process id */
	UINT32		reserved_to_die;

#if defined(CC_LG_SNAP_SHOT)
	void *		suspend_pages;
	UINT32		page_order;
	size_t		gfx_prev_size;
	size_t		gfx_size;
#endif
} MT_GAL_SURFACE_T;

typedef struct _mt_gal_plane
{
	OSD_PLANE_T		plane_id;
	OSD_SCALER_T	scaler_id;
	BOOL			enable;
	BOOL			stereo_enable;
	MT_GAL_3D_MODE			stereo_mode;
	MT_GAL_RECT_T 			rect_in;
	MT_GAL_RECT_T 			rect_out;
	MT_GAL_PLANE_FORMAT_T	format;
	MT_GAL_SURFACE_T		*linked_surface;

	/* region info. */
	UINT32 *pregion_front, *pregion_list_front;
	UINT32 *pregion_back , *pregion_list_back;
	UINT32  flip_count;
	
	void	(*pfn_swap)(struct _mt_gal_plane * plane, UINT32 physical_of_back_surface);	
	
} MT_GAL_PLANE_T;

LIST_HEAD(mt_surface_list);
static MT_GAL_PLANE_T mt_plane_array[OSD_PLANE_MAX_NUM];
static MT_GAL_SURFACE_POOL_INFO_T mt_rSurfacePool;
static signed int mt_surface_id = 0;

static DEFINE_MUTEX(mutex_multiproc);
static DEFINE_MUTEX(mutex_surface);
static BOOL _SURFACE_MANAGE_INIT = false;

UINT32 u4SurfaceVirAddress = 0;
UINT32 u4SurfacePhyAddress = 0;

static UINT32 _fb_0_saved_smem_start;
static MT_GAL_SURFACE_INFO_T MTGAL_rPDPTmpBufSurfaceInfo;

static MT_GAL_SURFACE_INFO_T MTGAL_rTmpBufSurfaceInfo;
static MT_GAL_SURFACE_INFO_T MTGAL_rTmpBufSurfaceInfo2;
static BOOL MTGAL__fgTmpBufInited = FALSE;


#define MT53_GAL_PMX_MAX_NUM 5
static UINT32 _au4PMXOrderArray[PMX_MAX_INPORT_NS] =
{
	PMX_OSD1,
	PMX_OSD2,
	PMX_PIP,
	PMX_MAIN,
	PMX_OSD3
};

#define SURFACE_LOCK()					_MT_GAL_LockSurface(__func__, __LINE__)
#define SURFACE_UNLOCK()				_MT_GAL_UnLockSurface()

static inline int _MT_GAL_LockSurface(const char * func, const UINT32 line)
{
	return mutex_lock_interruptible(&mutex_surface);
}

static inline void _MT_GAL_UnLockSurface(void)
{
	mutex_unlock(&mutex_surface);
}

static inline unsigned int _MT_Check_Pixelformat_Valid(MT_GAL_PIXEL_FORMAT_T pixelFormat)
{
	switch (pixelFormat)
	{
	case MT_GAL_PIXEL_FORMAT_ARGB:
	case MT_GAL_PIXEL_FORMAT_LUT8:
	case MT_GAL_PIXEL_FORMAT_ARGB1555:
	case MT_GAL_PIXEL_FORMAT_RGB16:
	case MT_GAL_PIXEL_FORMAT_ARGB4444:
	case MT_GAL_PIXEL_FORMAT_A8:
		return TRUE;
	default:
		return FALSE;
	}
}

static inline UINT32 _MT_PixelFormat2Bpp(MT_GAL_PIXEL_FORMAT_T rFormat)
{
    switch (rFormat)
    {
	case MT_GAL_PIXEL_FORMAT_ARGB:
		return 4;
	case MT_GAL_PIXEL_FORMAT_LUT8:
		return 1;
	case MT_GAL_PIXEL_FORMAT_ARGB1555:
	case MT_GAL_PIXEL_FORMAT_RGB16:
	case MT_GAL_PIXEL_FORMAT_ARGB4444:
		return 2;
	default:
		return 0;
    }
}

static OSD_COLOR_MODE_T _MT_GALColorMode2OSDColorMode(MT_GAL_PIXEL_FORMAT_T rFormat)
{
    OSD_COLOR_MODE_T rOsdCm;
    switch (rFormat)
    {
	case MT_GAL_PIXEL_FORMAT_ARGB:
		rOsdCm = OSD_CM_ARGB8888_DIRECT32;
		break;
	case MT_GAL_PIXEL_FORMAT_LUT8:
		rOsdCm = OSD_CM_RGB_CLUT8;
		break;
	case MT_GAL_PIXEL_FORMAT_ARGB1555:
		rOsdCm = OSD_CM_ARGB1555_DIRECT16;
		break;
	case MT_GAL_PIXEL_FORMAT_RGB16:
		rOsdCm = OSD_CM_RGB565_DIRECT16;
		break;
	case MT_GAL_PIXEL_FORMAT_ARGB4444:
		rOsdCm = OSD_CM_ARGB4444_DIRECT16;
		break;
	case MT_GAL_PIXEL_FORMAT_A8:
		rOsdCm = OSD_CM_RESERVED_0;
		break;
	case MT_GAL_PIXEL_FORMAT_MAX:
		rOsdCm = OSD_CM_RESERVED_0;
		break;
	default:
		rOsdCm = OSD_CM_ARGB8888_DIRECT32;
		break;
    }
    return rOsdCm;
}

static INT32 _MT_allocate_temp_buffer(void)
{
	if(MTGAL__fgTmpBufInited)
		return  true;

 	x_memset(&MTGAL_rTmpBufSurfaceInfo , 0, sizeof(MT_GAL_SURFACE_INFO_T));
	x_memset(&MTGAL_rTmpBufSurfaceInfo2, 0, sizeof(MT_GAL_SURFACE_INFO_T));
	
	if(MT_GAL_CreateSurface(1280, 720, MT_GAL_PIXEL_FORMAT_ARGB, &MTGAL_rTmpBufSurfaceInfo) != 0)
        return false;
	if(MT_GAL_CreateSurface(1280, 720, MT_GAL_PIXEL_FORMAT_ARGB, &MTGAL_rTmpBufSurfaceInfo2) != 0)
	{
		MT_GAL_DestroySurface(&MTGAL_rTmpBufSurfaceInfo);
        return false;
	}
	x_memset(&MTGAL_rPDPTmpBufSurfaceInfo, 0, sizeof(MT_GAL_SURFACE_INFO_T));
	
#if defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5882)
	if(IS_PANEL_2D_N_3D_L12R12)
	{
		struct fb_info * pfbinfo = registered_fb[0];
		if(MT_GAL_CreateSurface(1280, 2048, MT_GAL_PIXEL_FORMAT_ARGB, &MTGAL_rPDPTmpBufSurfaceInfo) != 0)
		{
			printf("%s: failed to allocate the size of 1280x2048 32bits.\n");
			return false;
		}

		_fb_0_saved_smem_start = pfbinfo->fix.smem_start;
		pfbinfo->fix.smem_start = MTGAL_rPDPTmpBufSurfaceInfo.physicalAddress;
	}
#endif

    MTGAL__fgTmpBufInited = TRUE;
    return true;
}

static int _MT_compare_from_offset(MT_GAL_SURFACE_T *pitem, UINT32 compare_value)
{
	return pitem->surfaceInfo->offset == compare_value;
}
static MT_GAL_SURFACE_T * _MT_find_surface( int (* pfn_compare)(MT_GAL_SURFACE_T *pitem, UINT32 compare_value), UINT32 compare_value )
{
	MT_GAL_SURFACE_T *mt_surface, *mt_selection = NULL;
	
	if(SURFACE_LOCK() < 0)
		return NULL;
	else
	{
		list_for_each_entry(mt_surface, &mt_surface_list, list_item)
		{
			if (pfn_compare(mt_surface, compare_value))
			{
				mt_selection = mt_surface;
				break;
			}
		}
	}
	SURFACE_UNLOCK();
	return mt_selection;
}
static MT_GAL_PLANE_T * _MT_find_plane(UINT32 plane)
{
	if (plane >= OSD_PLANE_MAX_NUM)
		return NULL;
	return &mt_plane_array[plane];
}

static inline UINT32 _MT_u4Offset2Virtual(UINT32 addr)
{
	//return (addr + mt_rSurfacePool.virtualAddr);
	 return (addr + u4SurfaceVirAddress);
}

static inline UINT32 _MT_u416BAlignWidth(UINT32 width)
{
	return ((width + 0xf) & (~0xf));
}
	
static inline UINT32 _MT_u416BAlignPitch(UINT32 width, UINT32 bpp)
{
#if 1//def HW_DECODER
		return ((width * bpp + 0xf)&(~0xf));
#else
		return (u416BAlignWidth(width) * bpp);
#endif
}

static inline UINT32 _MT_u4Offset2Physical(UINT32 addr)
{
	return (addr + u4SurfacePhyAddress);
}

static inline UINT32 _MT_Osd2PosdID(UINT32 osdID)
{
	if(osdID > 2) 
	{
		printf("posd used error \n");
		return 0;
	}
	return (osdID - 1);
}

static void _MT_output_surface(MT_GAL_SURFACE_T *pSurface)
{
	MT_GAL_SURFACE_INFO_T *pSurfaceInfo = pSurface->surfaceInfo;
	
	printk(" ** surface id:%d ** \n", pSurface->id);
	printk("\t- size  : 0x%08x\n", pSurface->size);
	printk("\t- vaddr : 0x%08x\n", pSurface->addr);
	printk("\t- reference_count: %d\n", pSurface->reference_count);
	printk("\t- layer : %d\n", pSurface->layer);
	printk("\t- taskId: %d\n", pSurface->taskId);
	printk("\t- reserved_to_die: %d\n", pSurface->reserved_to_die);

	if(pSurfaceInfo)
	{
		printk("\t\t ** surface info ** \n");
		printk("\t\t- width : %d\n", pSurfaceInfo->width);
		printk("\t\t- height: %d\n", pSurfaceInfo->height);
		printk("\t\t- bpp   : %d\n", pSurfaceInfo->bpp);
		printk("\t\t- offset: 0x%08x\n", pSurfaceInfo->offset);
		printk("\t\t- paddr : %d\n", pSurfaceInfo->physicalAddress);
		printk("\t\t- vaddr : %d\n", pSurfaceInfo->virtualAddress);
		printk("\t\t- palette: 0x%08x\n", (uint32_t)pSurfaceInfo->paletteInfo.pPalette);
		printk("\t\t- palette length: %d\n", pSurfaceInfo->paletteInfo.length);
	}
	else
		printk("\t- no surface info\n");
}

static void _MT_output_all_surfaces_in_list(void)
{
	MT_GAL_SURFACE_T *mt_surface;

	if (list_empty(&mt_surface_list))
	{
		printk(" ** there is no surface in list **\n");
		return;
	}

#if 0
	printk(" ** there is some of surfaces in list **\n");
#else
	if (SURFACE_LOCK() < 0)
		return;
	else
	{
		list_for_each_entry(mt_surface, &mt_surface_list, list_item)
		{
			_MT_output_surface(mt_surface);
			printk("\n");
		}
	}
	SURFACE_UNLOCK();
#endif
}

static void _MT_add_reference(MT_GAL_SURFACE_T* mt_surface)
{
	if (!mt_surface || mt_surface->reserved_to_die)
		return;
	mt_surface->reference_count++;
}

static void _MT_del_reference(MT_GAL_SURFACE_T* mt_surface)
{
	MT_GAL_SURFACE_INFO_T *mt_surface_info = (mt_surface) ? mt_surface->surfaceInfo : NULL;
	void *memory_surface = NULL;
	
	if (!mt_surface)
		return;

	if (SURFACE_LOCK() < 0)
		return;
	else
	{
		if (mt_surface->reference_count > 1)
		{
			mt_surface->reference_count--;
			SURFACE_UNLOCK();
			return;
		}
		list_del(&mt_surface->list_item);
	}
	SURFACE_UNLOCK();

	//dlfree((void*)(mt_hSurfaces[i].offset + mt_rSurfacePool.virtualAddress));/*[weiwei_xxxxxxxxx]*/
	//MT53_GPU_dlfree((void*)(mt_hSurfaces[i].offset + mt_rSurfacePool.virtualAddr));
	//printk("MT_GAL_DestroySurface   mt_hSurfaces[i].offset = 0x%8x \n",mt_hSurfaces[i].offset);
	//printk("MT_GAL_DestroySurface  u4SurfaceVirAddress = 0x%8x \n",u4SurfaceVirAddress);	
	
	memory_surface = (void *)_MT_u4Offset2Virtual(mt_surface_info->offset - mt_surface_info->aligned_offset);	
	MT53_GPU_dlfree(memory_surface);
	u4LastDLFreePtr = memory_surface;

	if ((mt_surface_info->pixelFormat == MT_GAL_PIXEL_FORMAT_LUT8) && ((mt_surface_info->paletteInfo.pPalette) != 0))
	{
		//printf("mt_hSurfaces[i].surfaceInfo->paletteInfo.pPalette = 0x%8x\n",mt_hSurfaces[i].surfaceInfo->paletteInfo.pPalette);
		//printf("_MT_u4Offset2Virtual = 0x%8x\n",(void*)_MT_u4Offset2Virtual((UINT32)(mt_hSurfaces[i].surfaceInfo->paletteInfo.pPalette)));
		MT53_GPU_dlfree((void*)_MT_u4Offset2Virtual((UINT32)(mt_surface_info->paletteInfo.pPalette)));/*[weiwei_xxxxxxxxx]*/
		mt_surface_info->paletteInfo.pPalette = 0;
	}	
	x_mem_free(mt_surface_info);
	x_mem_free(mt_surface);
}

static void _MT_swap_region(MT_GAL_PLANE_T* pmt_plane, UINT32 physical_of_back_surface)
{
	OSD_PLA_FlipTo(pmt_plane->plane_id, *pmt_plane->pregion_list_back);

	pmt_plane->flip_count += 1;
	MT_GAL_SWAP(pmt_plane->pregion_list_front, pmt_plane->pregion_list_back, UINT32*);
	MT_GAL_SWAP(pmt_plane->pregion_front, pmt_plane->pregion_back, UINT32*);

#if 1
	/* make sure back buffer address has been swapped. it does not need at the cursor plane. */
	{
		BOOL fgEnable;
		OSD_PLA_GetEnable(pmt_plane->plane_id, &fgEnable);
		if ((fgEnable) &&
			(pmt_plane->plane_id == OSD_PLANE_2 || pmt_plane->plane_id == OSD_PLANE_3))
		{
			UINT32 u4Offset = ((pmt_plane->plane_id + 1) << 8) + 0x24;
			//u64 limited_jiffies = get_jiffies_64() + 5*HZ; /* up to 5s */
			while(1)
			{
				volatile UINT32 u4Val = IO_READ32(0xF0021000, u4Offset) << 4;
				if ( (u4Val == 0) ||
					((u4Val & 0xFFFFFFul) == (physical_of_back_surface & 0xFFFFFFul)) )
					break;
				/*if (time_after(jiffies, limited_jiffies)) {
					printf("%s: delay time to real swapping has been over.\n", __func__);
					break;
				}*/
				schedule();
			}
		}
	}
#endif
}

static void _MT_set_display_region(const MT_GAL_PLANE_T* pmt_plane, UINT32 u4Region, BOOL fgUpdateScaler)
{
	UINT32 u4OsdMirror = 0;
	UINT32 u4OsdFlip = 0;
	UINT32 u4Type = 0;
	UINT32 u4PanelWidth, u4PanelHeight;
    MT_GAL_SURFACE_T *pSurface = pmt_plane->linked_surface;
    MT_GAL_SURFACE_INFO_T* pSurfaceInfo = (pSurface) ? pSurface->surfaceInfo : NULL;
    MT_GAL_RECT_T srcRect, dstRect;
    const MT_GAL_RECT_T* pSrcRect = (const MT_GAL_RECT_T*)&pmt_plane->rect_in;
    const MT_GAL_RECT_T* pDstRect = (const MT_GAL_RECT_T*)&pmt_plane->rect_out;

	if (pSrcRect->w == 0 || pSrcRect->h == 0)
	{
		srcRect = *pSrcRect;
		if (pSurfaceInfo)
		{
			printk(KERN_ERR "%s: scaler's rect will be 0. (src_w:%d and src_h:%d), and no surface linked to the plane:%d\n",
					__func__, pSrcRect->w, pSrcRect->h, pmt_plane->plane_id);
			return;
		}
		srcRect.w = pSurfaceInfo->width;
		srcRect.h = pSurfaceInfo->height;
		pSrcRect = (const MT_GAL_RECT_T *)&srcRect;
	}

	OSD_RGN_Get(u4Region, OSD_RGN_H_MIRROR, &u4OsdMirror);
	OSD_RGN_Get(u4Region, OSD_RGN_V_FLIP,   &u4OsdFlip);

	if (u4OsdMirror && u4OsdFlip)
		u4Type = 4;
	else if (u4OsdFlip)
		u4Type = 2;
	else if (u4OsdMirror)
		u4Type = 1;
	else
		u4Type = 0;

	u4PanelWidth  = OSD_BASE_GetDisplayWidth();
	u4PanelHeight = OSD_BASE_GetDisplayHeight();

	if (pDstRect->w == 0 || pDstRect->h == 0)
	{
		dstRect = *pDstRect;
		dstRect.w = (int)u4PanelWidth;
		dstRect.h = (int)u4PanelHeight;
		pDstRect = (const MT_GAL_RECT_T *)&dstRect;
	}

	switch(u4Type)
	{
	case 0:
		OSD_RGN_Set(u4Region, OSD_RGN_POS_X,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_POS_Y,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_H, pSrcRect->x);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_V, pSrcRect->y);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_H,  pSrcRect->h);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_H,  pSrcRect->h);

		OSD_BASE_SetOsdPosition(pmt_plane->plane_id, pDstRect->x, pDstRect->y);
		if (fgUpdateScaler)
			OSD_SC_Scale(pmt_plane->scaler_id, 1, pSrcRect->w, pSrcRect->h, pDstRect->w, pDstRect->h);
		break;
	case 1: /* h-mirror mode */
		OSD_RGN_Set(u4Region, OSD_RGN_POS_X,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_POS_Y,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_H, pSrcRect->x);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_V, pSrcRect->y);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_H,  pSrcRect->h);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_H,  pSrcRect->h);
		
		if (u4PanelWidth > pDstRect->w + pDstRect->x)
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, u4PanelWidth - pDstRect->w - pDstRect->x, pDstRect->y);
		else
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, 0, pDstRect->y);
		if (fgUpdateScaler)
			OSD_SC_Scale(pmt_plane->scaler_id, 1, pSrcRect->w, pSrcRect->h, pDstRect->w, pDstRect->h);
		break;
	case 2: /* v-flip mode */
		OSD_RGN_Set(u4Region, OSD_RGN_POS_X,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_POS_Y,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_H, pSrcRect->x);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_V, pSrcRect->y);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_H,  pSrcRect->h);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_H,  pSrcRect->h);
		
		if (u4PanelHeight > pDstRect->h + pDstRect->y)
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, pDstRect->x, u4PanelHeight- pDstRect->h - pDstRect->y);
		else
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, pDstRect->x, 0);
		if (fgUpdateScaler)
			OSD_SC_Scale(pmt_plane->scaler_id, 1, pSrcRect->w, pSrcRect->h, pDstRect->w, pDstRect->h);
		break;
	case 3: /* h-mirror and v-flip mode */
		OSD_RGN_Set(u4Region, OSD_RGN_POS_X,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_POS_Y,  0);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_H, pSrcRect->x);
		OSD_RGN_Set(u4Region, OSD_RGN_CLIP_V, pSrcRect->y);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_BMP_H,  pSrcRect->h);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_W,  pSrcRect->w);
		OSD_RGN_Set(u4Region, OSD_RGN_OUT_H,  pSrcRect->h);

		if ((u4PanelHeight > pDstRect->h + pDstRect->y) && (u4PanelWidth > pDstRect->w + pDstRect->x))
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, u4PanelWidth - pDstRect->w - pDstRect->x, u4PanelHeight - pDstRect->h - pDstRect->y);
		else if (u4PanelWidth > pDstRect->w + pDstRect->x)
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, u4PanelWidth - pDstRect->w - pDstRect->x, pDstRect->y);
		else if (u4PanelHeight > pDstRect->h + pDstRect->y)
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, pDstRect->x, u4PanelHeight - pDstRect->h - pDstRect->y);
		else
			OSD_BASE_SetOsdPosition(pmt_plane->plane_id, 0, 0);
		if (fgUpdateScaler)
			OSD_SC_Scale(pmt_plane->scaler_id, 1, pSrcRect->w, pSrcRect->h, pDstRect->w, pDstRect->h);
		break;
	}
}

static void _MT_debug_timer_handler(unsigned long data)
{
	_MT_output_all_surfaces_in_list();

	/* reset the expires for next timer intr */
	debug_timer.expires = get_jiffies_64() + (10*HZ);  // 10s
	add_timer(&debug_timer);
}

///////////////////////////////////////////////////////////////////////////////////

void MT_GAL_SetSurfacePool(MT_GAL_SURFACE_POOL_INFO_T *u4SurfacePool)
{
	mt_rSurfacePool.byteOffsetAlign= u4SurfacePool->byteOffsetAlign;
	mt_rSurfacePool.bytePitchAlign = u4SurfacePool->bytePitchAlign;
	mt_rSurfacePool.physicalAddr = u4SurfacePool->physicalAddr;
	mt_rSurfacePool.size = u4SurfacePool->size;
	mt_rSurfacePool.startOffset = u4SurfacePool->startOffset;
	mt_rSurfacePool.virtualAddr = u4SurfacePool->virtualAddr;
} // mem_get_status()

MTGAL_RESULT_T MT_GAL_SyncSurface(MT_GAL_SURFACE_INFO_T *u4SurfacePool)
{
	MT_GAL_SURFACE_INFO_T* mt_surface_info = NULL;

	mt_surface_info = MT_GAL_GetSurfaceFromOffset(u4SurfacePool->offset);
	if(mt_surface_info == NULL)
		return MTGAL_INVALID_PARAM;
	
	mt_surface_info->bpp	= u4SurfacePool->bpp;
	mt_surface_info->height	= u4SurfacePool->height;
	mt_surface_info->id		= u4SurfacePool->id;
	mt_surface_info->offset	= u4SurfacePool->offset;
	mt_surface_info->paletteInfo.length		= u4SurfacePool->paletteInfo.length;
	mt_surface_info->paletteInfo.pPalette	= u4SurfacePool->paletteInfo.pPalette;
	mt_surface_info->physicalAddress		= u4SurfacePool->physicalAddress;
	mt_surface_info->pid	= u4SurfacePool->pid;
	mt_surface_info->pitch	= u4SurfacePool->pitch;
	mt_surface_info->pixelFormat	= u4SurfacePool->pixelFormat;
	mt_surface_info->property		= u4SurfacePool->property;
	mt_surface_info->virtualAddress = u4SurfacePool->virtualAddress;
	mt_surface_info->width	= u4SurfacePool->width;
	return MTGAL_OK;
} // mem_get_status()

MTGAL_RESULT_T MT_GAL_MultiProcLock(void)
{
	mutex_lock(&mutex_multiproc);
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_MultiProcUnLock(void)
{
	mutex_unlock(&mutex_multiproc);
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_MallocToOffset(INT32 u4Length,INT32 * offset )
{
	UINT32* Alloc_Addr = NULL;

	Alloc_Addr = (UINT32*)MT53_GPU_dlmalloc(u4Length);
	if (Alloc_Addr == NULL)
	{
		printk("(yjdbg) dlmalloc fail\n");
		return MTGAL_NOT_OK;
	}

	*offset = (UINT32)Alloc_Addr - u4SurfaceVirAddress;
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_CreateSurface(UINT16 width, UINT16 height, int pixelFormat, MT_GAL_SURFACE_INFO_T *pSurfaceInfo)
{
	MT_GAL_SURFACE_T *new_mt_surface;
	MT_GAL_SURFACE_INFO_T *new_mt_surface_info;
	
	UINT32* memory_surface = NULL;
	UINT32* memory_palette = NULL;
	UINT32  memory_surface_offset, memory_palette_offset = 0;
	UINT32  u4Length = 0;
	UINT32	u4Order = 0;
			
	mt_mempool_status st;
	UINT32 mtkFree = 0; //KBytes

	if (!_MT_Check_Pixelformat_Valid(pixelFormat))
		return MTGAL_INVALID_PARAM;
	u4Length = _MT_u416BAlignWidth(width) * height * _MT_PixelFormat2Bpp(pixelFormat);
	u4Length += PAGE_SIZE;

	//Check available Videomemory
	//dlmem_get_status(GAL_MEMPOOL_STAT_TOTALFREE, &st);/*[weiwei_xxxxxxxxx]*/	
	MT53_GPU_dlmem_get_status(MT53_GPU_MEMPOOL_STAT_TOTALFREE, &st);
	mtkFree = (st.totalfree) >> 10;/*[weiwei_xxxxxxxxx]*/
	//dbgprint("^r^ (mtkFree=%d KB), (mtkFree=%d MB)", mtkFree, (UINT32)(mtkFree/1024.0) );

#ifndef CC_MT5882
	if(mtkFree < 4096) //smaller than 4MB
	{
		printk("Not enough videomemory (mtkFree=%d KB). [Warn] Reserved size=4096KB)\n", mtkFree);
		return MTGAL_OUT_OF_MEMORY; //All members of 'pSurfaceInfo' must be zero.
	}
#endif

	memory_surface = (UINT32*)MT53_GPU_dlmalloc(u4Length);
	if (memory_surface == NULL)
	{
		printk("dlmalloc fail for surface\n");
		return MTGAL_OUT_OF_MEMORY;
	}

    if (u4LastDLFreePtr == (void *)memory_surface)
    {
		/* flush dcache when memory_surface is allocated with same addresss right after dlfree. */
        HalFlushInvalidateDCache();
        u4LastDLFreePtr = 0;
    }

	//newSurfaceInfo.offset = (UINT32)memory_surface - mt_rSurfacePool.virtualAddr;
	memory_surface_offset = (UINT32)memory_surface - u4SurfaceVirAddress;
	
	if(pixelFormat == MT_GAL_PIXEL_FORMAT_LUT8)
	{
	 	memory_palette = (UINT32 *)MT53_GPU_dlmalloc(sizeof(UINT32)*(256+4));
		if( memory_palette == NULL )
		{
			printk("dlmalloc fail for palette\n");
			MT53_GPU_dlfree((void*)_MT_u4Offset2Virtual(memory_surface_offset));
			return MTGAL_OUT_OF_MEMORY;
		}
		memory_palette_offset = (UINT32)memory_palette - u4SurfaceVirAddress;
	}

	new_mt_surface		= (MT_GAL_SURFACE_T *)x_mem_alloc(sizeof(MT_GAL_SURFACE_T));
	x_memset(new_mt_surface, 0, sizeof(MT_GAL_SURFACE_T));
	new_mt_surface_info = (MT_GAL_SURFACE_INFO_T *)x_mem_alloc(sizeof(MT_GAL_SURFACE_INFO_T));
	x_memset(new_mt_surface_info, 0, sizeof(MT_GAL_SURFACE_INFO_T));
	//x_memset(memory_surface, 0, u4Length);  // does it need to clear ?
	//HalFlushInvalidateDCache();

#if 1//def HW_DECODER
	new_mt_surface_info->pitch = _MT_u416BAlignPitch(width, _MT_PixelFormat2Bpp(pixelFormat));
#else
	new_mt_surface_info->pitch = _MT_u416BAlignPitch(width)* _MT_PixelFormat2Bpp(pixelFormat);
#endif
	new_mt_surface_info->aligned_offset = PAGE_SIZE - (_MT_u4Offset2Physical(memory_surface_offset) & (PAGE_SIZE-1));
	new_mt_surface_info->offset = memory_surface_offset + new_mt_surface_info->aligned_offset;
	new_mt_surface_info->bpp = _MT_PixelFormat2Bpp(pixelFormat) << 3;
	new_mt_surface_info->width = width;
	new_mt_surface_info->height = height;
	new_mt_surface_info->pixelFormat = pixelFormat;
	new_mt_surface_info->physicalAddress = _MT_u4Offset2Physical(new_mt_surface_info->offset);
	if(pixelFormat == MT_GAL_PIXEL_FORMAT_LUT8)
	{
		new_mt_surface_info->paletteInfo.pPalette = (void*)memory_palette_offset;
		new_mt_surface_info->paletteInfo.length = MT_PAL_LENGTH;
	}
	else
		new_mt_surface_info->paletteInfo.pPalette = 0;
	new_mt_surface_info->id = mt_surface_id;

	new_mt_surface->surfaceInfo = new_mt_surface_info;
	new_mt_surface->layer	= 0;
	new_mt_surface->addr	= (UINT32)memory_surface;
	new_mt_surface->size	= new_mt_surface_info->pitch * height;
	new_mt_surface->reference_count = 1;
	new_mt_surface->id		= mt_surface_id;
	new_mt_surface->sockId	= -1; /* sockId is assigend by RPC4DTV interface */
	new_mt_surface->taskId	= current->tgid;

	if (SURFACE_LOCK() < 0)
	{
		if (pixelFormat == MT_GAL_PIXEL_FORMAT_LUT8)
			MT53_GPU_dlfree((void*)_MT_u4Offset2Virtual(memory_palette_offset));
		MT53_GPU_dlfree((void*)_MT_u4Offset2Virtual(memory_surface_offset));		
		x_mem_free(new_mt_surface_info);
		x_mem_free(new_mt_surface);
		return MTGAL_NOT_OK;
	}
	else
	{
		list_add_tail(&new_mt_surface->list_item, &mt_surface_list);
		mt_surface_id = (mt_surface_id+1)%INT_MAX;
	}
	SURFACE_UNLOCK();

	//printk("[weiwei][MT_GAL_CreateSurface] new_mt_surface_info end :u4PhyAddr=0x%x u4VirtAddr= 0x%x pSurfaceInfo->offset=0x%x\n",
	//			new_mt_surface_info->physicalAddress,
	//			new_mt_surface_info->virtualAddress,
	//			new_mt_surface_info->offset);
	x_memcpy(pSurfaceInfo, new_mt_surface_info, sizeof(MT_GAL_SURFACE_INFO_T));

#if defined(CC_LG_SNAP_SHOT)
	/* calculation page order for suspend */
	new_mt_surface->suspend_pages = NULL;
	new_mt_surface->page_order = (UINT32)-1;
	for(u4Order = 0; u4Order <= MAX_ORDER; ++u4Order)
	{
		UINT32 suspend_page_size = (1 << u4Order) << PAGE_SHIFT;
		if (new_mt_surface->size <= suspend_page_size)
		{
			new_mt_surface->page_order = u4Order;
			break;
		}
	}
#endif /* CC_LG_SNAP_SHOT */
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_DestroySurface(MT_GAL_SURFACE_INFO_T *pSurfaceInfo)
{
	//gal layer has not used id in GAL_GX_SURFACE_INFO_T, so we don't need check this id in here
	MT_GAL_SURFACE_T *mt_surface = _MT_find_surface(_MT_compare_from_offset, pSurfaceInfo->offset);	

	if (!mt_surface)
	{
		printk("^r^ Not found: offset=0x%08x in %s()\n", pSurfaceInfo->offset, __func__);/*[weiwei_xxxxxxxxx]*/
		return MTGAL_NOT_OK;
	}

	mt_surface->reserved_to_die = 1;
	_MT_del_reference(mt_surface);
	return MTGAL_OK;
}

MT_GAL_SURFACE_INFO_T * MT_GAL_GetSurfaceFromOffset(unsigned int offset)
{
	MT_GAL_SURFACE_T *mt_surface = _MT_find_surface(_MT_compare_from_offset, offset);

	if (!mt_surface)
	{
		printk("Unknown surface! in %s(offset:0x%x)\n", __func__, offset);
		return NULL;
	}
	return mt_surface->surfaceInfo;
}

extern signed int _MTIMAGE_InitHWResouce(void);
int MT_GAL_GALInit(unsigned int u4PhyAddr, unsigned int u4VirAddr, unsigned int u4Size)
{
	INT32 rVal = 1;
	MT_GAL_SURFACE_POOL_INFO_T u4SurfacePool;

	UINT32 u4PanelW, u4PanelH;
	UINT32 u4BmpWidth, u4BmpHeight, u4BmpPitch, u4DispX, u4DispY, u4DispW, u4DispH;
	VOID * pvBitmap=NULL;
	MT_GAL_PIXEL_FORMAT_T u4ColorMode;
	UINT32 start, size;

	//printf("[weiwei][MT_GAL_GALInit]u4PhyAddr=0x%8x u4VirAddr=0x%8x  u4Size=0x%8x\n",u4PhyAddr, u4VirAddr,u4Size);
	
	if( !_SURFACE_MANAGE_INIT)
	{		
		//rBuffer.u4Size = DRVCUST_OptGet(eFBMemSize) -DRVCUST_OptGet(eGfxFBPartitionSize) ;
		//rBuffer.u4Addr = BSP_GetFbmMemAddr() - DRVCUST_OptGet(eFBMemSize) - DRVCUST_OptGet(eDirectFBMemSize);
		OSD_Init();
		GFX_Init();		
		//u4Virtualaddr = VIRTUAL(rBuffer.u4Addr );
		//rVal = MT53_GPU_dlmem_init(u4Virtualaddr,rBuffer.u4Size);

		mt_plane_array[0].plane_id = OSD_PLANE_1;
			mt_plane_array[0].pregion_front		= &_u4Posd2Rgn;
			mt_plane_array[0].pregion_back		= &_u4Posd2Rgn;
			mt_plane_array[0].pregion_list_front= &_u4Posd2List;
			mt_plane_array[0].pregion_list_back	= &_u4Posd2List;
			mt_plane_array[0].scaler_id 		= OSD_SCALER_1;
			mt_plane_array[0].flip_count		= 0;
			mt_plane_array[0].pfn_swap			= _MT_swap_region;
		mt_plane_array[1].plane_id = OSD_PLANE_2;
			mt_plane_array[1].pregion_front		= &_u4Posd0Rgn0;
			mt_plane_array[1].pregion_back		= &_u4Posd0Rgn1;
			mt_plane_array[1].pregion_list_front= &_u4Posd0List0;
			mt_plane_array[1].pregion_list_back	= &_u4Posd0List1;
			mt_plane_array[1].scaler_id			= OSD_SCALER_2;
			mt_plane_array[1].flip_count		= 0;
			mt_plane_array[1].pfn_swap			= _MT_swap_region;
		mt_plane_array[2].plane_id = OSD_PLANE_3;
			mt_plane_array[2].pregion_front		= &_u4Posd1Rgn0;
			mt_plane_array[2].pregion_back		= &_u4Posd1Rgn1;
			mt_plane_array[2].pregion_list_front= &_u4Posd1List0;
			mt_plane_array[2].pregion_list_back	= &_u4Posd1List1;
			mt_plane_array[2].scaler_id			= OSD_SCALER_3;
			mt_plane_array[2].flip_count		= 0;
			mt_plane_array[2].pfn_swap			= _MT_swap_region;
		
		//printf("[weiwei]1111 [MT_GAL_GALInit]u4VirAddr=0x%x, u4SurfaceVirAddress=0x%x, u4Size=0x%x 00 \n",u4VirAddr, u4SurfaceVirAddress, u4Size);
		
		mt53xx_get_reserved_area((unsigned long *)&start, (unsigned long *)&size);
		if (start && u4PhyAddr >= start && (u4PhyAddr + u4Size) <= (start + size))
		{
			u4SurfaceVirAddress = (UINT32)phys_to_virt((unsigned long)u4PhyAddr);
		}
		else
		{
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
				u4SurfaceVirAddress = (UINT32)ioremap_cached((int)u4PhyAddr, u4Size);
		#else
				u4SurfaceVirAddress = __ioremap((int)u4PhyAddr, u4Size, L_PTE_BUFFERABLE | L_PTE_CACHEABLE);
		#endif
		}
		//printf("[weiwei]2222 [MT_GAL_GALInit]u4VirAddr=0x%x, u4SurfaceVirAddress=0x%x, u4Size=0x%x \n",u4VirAddr, u4SurfaceVirAddress, u4Size);
		u4SurfacePhyAddress = u4PhyAddr;
		
		rVal = MT53_GPU_dlmem_init((void *)u4SurfaceVirAddress,u4Size);
		

		//printf("[weiwei][MT_GAL_GALInit]u4VirAddr=0x%x, u4SurfaceVirAddress=0x%x, u4Size=0x%x 00 \n",u4VirAddr, u4SurfaceVirAddress, u4Size);

		if(!rVal)
		{
		  printf("[weiwei][MT_GAL_GALInit]u4PhyAddr=0x%x, u4SurfaceVirAddress=0x%x, u4Size=0x%x 00 \n",u4PhyAddr, u4SurfaceVirAddress, u4Size);	
	          return MTGAL_NOT_OK;
		}

		u4SurfacePool.byteOffsetAlign = 16;
		u4SurfacePool.bytePitchAlign = 16;
		u4SurfacePool.physicalAddr = u4PhyAddr;
		u4SurfacePool.size = u4Size;
		u4SurfacePool.startOffset = 0;		
		u4SurfacePool.virtualAddr = u4VirAddr;
		
		MT_GAL_SetSurfacePool(&u4SurfacePool);
		_MT_allocate_temp_buffer();
		PMX_SetPlaneOrderArray(_au4PMXOrderArray);

		u4PanelW = MT_GAL_LCD_PANEL_WIDTH;//OSD_BASE_GetDisplayWidth();
    	       u4PanelH = MT_GAL_LCD_PANEL_HEIGHT;//OSD_BASE_GetDisplayHeight();
		
		u4BmpWidth = u4PanelW;
		u4BmpHeight = u4PanelH;
		u4BmpPitch =u4BmpWidth*4;
		u4DispX = 0;
		u4DispY= 0;
		u4DispW = u4PanelW;
		u4DispH = u4PanelH;
		pvBitmap = (void *)u4SurfacePool.physicalAddr;
		u4ColorMode = _MT_GALColorMode2OSDColorMode(MT_GAL_PIXEL_FORMAT_ARGB);

       // for(u4Plane = OSD_PLANE_1; u4Plane <= OSD_PLANE_3; u4Plane++)
        {
        	OSD_RGN_Create(&_u4Posd0Rgn0,u4BmpWidth,u4BmpHeight,pvBitmap,u4ColorMode,u4BmpPitch,u4DispX,u4DispY,u4DispW,u4DispH);
		OSD_RGN_Create(&_u4Posd0Rgn1,u4BmpWidth,u4BmpHeight,pvBitmap,u4ColorMode,u4BmpPitch,u4DispX,u4DispY,u4DispW,u4DispH);
        	
        	OSD_RGN_Set(_u4Posd0Rgn0, OSD_RGN_BIG_ENDIAN, 0);
		OSD_RGN_Set(_u4Posd0Rgn1, OSD_RGN_BIG_ENDIAN, 0);	
        	OSD_RGN_LIST_Create(&_u4Posd0List0);
		OSD_RGN_LIST_Create(&_u4Posd0List1);
		/* Set default alpha to bitsel */
        	OSD_RGN_Set(_u4Posd0Rgn0,OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
		OSD_RGN_Set(_u4Posd0Rgn1,OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
        	OSD_RGN_Insert(_u4Posd0Rgn0,_u4Posd0List0);
		OSD_RGN_Insert(_u4Posd0Rgn1,_u4Posd0List1);
        	OSD_PLA_FlipTo(OSD_PLANE_2,_u4Posd0List0);
        	OSD_SC_Scale(OSD_SCALER_2, 1, u4DispW, u4DispH, u4DispW,u4DispH);
        	/* Enable OSD plane*/
        	OSD_PLA_Enable(OSD_PLANE_2, FALSE);
        }
	    {
        	OSD_RGN_Create(&_u4Posd1Rgn0,u4BmpWidth,u4BmpHeight,pvBitmap,u4ColorMode,u4BmpPitch,u4DispX,u4DispY,u4DispW,u4DispH);
		OSD_RGN_Create(&_u4Posd1Rgn1,u4BmpWidth,u4BmpHeight,pvBitmap,u4ColorMode,u4BmpPitch,u4DispX,u4DispY,u4DispW,u4DispH);
        	
        	OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BIG_ENDIAN, 0);
		OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BIG_ENDIAN, 0);
        	OSD_RGN_LIST_Create(&_u4Posd1List0);
		OSD_RGN_LIST_Create(&_u4Posd1List1);
		/* Set default alpha to bitsel */
        	OSD_RGN_Set(_u4Posd1Rgn0,OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
		OSD_RGN_Set(_u4Posd1Rgn1,OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
        	OSD_RGN_Insert(_u4Posd1Rgn0,_u4Posd1List0);
		OSD_RGN_Insert(_u4Posd1Rgn1,_u4Posd1List1);
        	OSD_PLA_FlipTo(OSD_PLANE_3,_u4Posd1List0);
        	OSD_SC_Scale(OSD_SCALER_3, 1, u4DispW, u4DispH, u4DispW,u4DispH);
        	/* Enable OSD plane*/
        	OSD_PLA_Enable(OSD_PLANE_3, FALSE);
        }
	#ifdef MT_GAL_CURSOR_IMPROVE
	{
        	OSD_RGN_Create(&_u4Posd2Rgn,MT_GAL_CURSOR_WIDTH,MT_GAL_CURSOR_HEIGHT,pvBitmap,u4ColorMode,MT_GAL_CURSOR_WIDTH*4, 0, 0,MT_GAL_CURSOR_WIDTH,MT_GAL_CURSOR_HEIGHT);
		
        	OSD_RGN_Set(_u4Posd2Rgn, OSD_RGN_BIG_ENDIAN, 0);
		OSD_RGN_LIST_Create(&_u4Posd2List);
		/* Set default alpha to bitsel */
        	OSD_RGN_Set(_u4Posd2Rgn,OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
		OSD_RGN_Insert(_u4Posd2Rgn,_u4Posd2List);
        	OSD_PLA_FlipTo(OSD_PLANE_1,_u4Posd2List);
        	OSD_SC_Scale(OSD_SCALER_1, 1, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight(), OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
        	/* Enable OSD plane*/
        	OSD_PLA_Enable(OSD_PLANE_1, FALSE);
        }
	#endif
        _MTIMAGE_InitHWResouce();
		_SURFACE_MANAGE_INIT = true;

		debug_timer.expires = get_jiffies_64() + (10*HZ);  // 10s
		debug_timer.function = _MT_debug_timer_handler;
		//add_timer(&debug_timer);  // for debugging
	}
	return MTGAL_OK;
}


MTGAL_RESULT_T MT_GAL_GetTempBuffer(INT32 *pSurfaceInfo, INT32 *pSurfaceInfo2)
{
	if(!MTGAL__fgTmpBufInited)
	{
		printf("ERROR: MT_GAL_GetTempBuffer >> temp buffer havenot been alloc\n");
		return MTGAL_NOT_OK;
	}
	*pSurfaceInfo = MTGAL_rTmpBufSurfaceInfo.offset;
	*pSurfaceInfo2 = MTGAL_rTmpBufSurfaceInfo2.offset;
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_GetTempSurface(MT_GAL_SURFACE_INFO_T *pSurfaceInfo, MT_GAL_SURFACE_INFO_T *pSurfaceInfo2)
{
	if(!MTGAL__fgTmpBufInited)
	{
		printf("ERROR: MT_GAL_GetTempSurface >> temp buffer havenot been alloc\n");
		return MTGAL_NOT_OK;
	}
	*pSurfaceInfo = MTGAL_rTmpBufSurfaceInfo;
	*pSurfaceInfo2 = MTGAL_rTmpBufSurfaceInfo2;
	return MTGAL_OK;
}

MTGAL_RESULT_T MT_GAL_SetFrameSurface(UINT32 u4Plane, MT_GAL_SURFACE_INFO_T *pSurfaceInfo)
{
	MT_GAL_SURFACE_T *mt_old_surface;
	MT_GAL_SURFACE_T *mt_new_surface = _MT_find_surface(_MT_compare_from_offset, pSurfaceInfo->offset);
	MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
	BOOL fgSwitch = 0, u4TemRgn = 0;

	if (!mt_plane)
	{
		printk("%s: no plane related to the param.(plane:%d)\n", __func__, u4Plane);
		return MTGAL_INVALID_PARAM;
	}
	mt_old_surface = mt_plane->linked_surface;

	if (mt_old_surface != mt_new_surface)
	{
		mt_plane->linked_surface = mt_new_surface;

		_MT_add_reference(mt_new_surface);
		_MT_del_reference(mt_old_surface);
	}
	u4TemRgn = *mt_plane->pregion_back;

#if 1
	if (_mt3dMode == MT_GAL_3DPR_MODE)
	{
		if(!_mt3dModeApplied)
		{	
		   //reserve
			_mt3dModeApplied = 1;
		}				
	}
	else  if (_mt3dMode == MT_GAL_3DPDP_MODE)
	{
		if(!_mt3dModeApplied)
		{		
			//reserve
			_mt3dModeApplied = 1;
		}					
	}
	else
	{
		if(!_mt3dModeApplied)
		{	
			//reserve	
			OSD_RGN_Set(u4TemRgn, OSD_RGN_BMP_PITCH, pSurfaceInfo->pitch);
			//_mt3dModeApplied = 1;
		}
	}
#endif

	if(u4Plane == OSD_PLANE_3 && _mt3dMode == MT_GAL_3DPDP_MODE)
	{
		UINT32 _fb_buf[2];
		struct fb_info * pfbinfo = registered_fb[0];

		if(num_registered_fb == 0)
		{
			printk("%s: there is no memory to use in fb0\n");
			return MTGAL_NOT_OK;
		}

		_fb_buf[0] = pfbinfo->fix.smem_start;
		if(OSD_BASE_GetDisplayWidth() == MT_GAL_HD_PDP_PANEL_WIDTH )
			_fb_buf[1] = pfbinfo->fix.smem_start + (pfbinfo->fix.line_length * 768);
		else
			_fb_buf[1] = pfbinfo->fix.smem_start + (pfbinfo->fix.line_length * 1024);

	#if 0
		{
			/* stretchblit by memcpy */
			UINT32 _dst_fb_buf = ( fgSwitch)?_fb_buf[0]:_fb_buf[1];
			UINT32 _src_fb_buf = _MT_u4Offset2Physical(pSurfaceInfo->offset);

			UINT32 h, scale_h = (OSD_BASE_GetDisplayWidth() == MT_GAL_HD_PDP_PANEL_WIDTH ) ? 768 : 1023;
			void * virt_dst = (UINT32)ioremap_cached((int)_dst_fb_buf, 1280*scale_h*4);
			void * virt_src = (UINT32)ioremap_cached((int)_src_fb_buf, 1280*720*4);

			smp_mb();
			for(h = 0; h < scale_h; ++h)
			{
				int voffset = (720*h)/scale_h;
				x_memcpy((int *)virt_dst + 1280*h, (int *)virt_src + 1280*voffset, 1280*4);
			}

			iounmap(virt_dst);
			iounmap(virt_src);
		}
	#else
		{
			/* stretchblit by gfx h/w */
			UINT32 scale_h = (OSD_BASE_GetDisplayWidth() == MT_GAL_HD_PDP_PANEL_WIDTH ) ? 768 : 1023;
			UINT32 _dst_fb_buf = ( fgSwitch)?_fb_buf[0]:_fb_buf[1];
			UINT32 _src_fb_buf = _MT_u4Offset2Physical(pSurfaceInfo->offset);

			smp_mb();
			GFX_Lock();
			{
				GFX_SetSrc((UINT8 *)_src_fb_buf, CM_ARGB8888_DIRECT32, 1280*4);
				GFX_SetDst((UINT8 *)_dst_fb_buf, CM_ARGB8888_DIRECT32, 1280*4);
				GFX_SetBltOpt(0, 0, 0);
				GFX_StretchBlt(0, 0, 1280, 720, 0, 0, 1280, scale_h);

				GFX_Flush();
				GFX_Wait();
			}
			GFX_Unlock();
		}
	#endif
		OSD_RGN_Set(u4TemRgn, OSD_RGN_BMP_ADDR, (fgSwitch)?_fb_buf[0]:_fb_buf[1]);
	}
	else
	{
		//printf("MT_GAL_SetFrameSurface address = 0x%x\n", _MT_u4Offset2Physical(pSurfaceInfo->offset));
		OSD_RGN_Set(u4TemRgn, OSD_RGN_BMP_ADDR, _MT_u4Offset2Physical(pSurfaceInfo->offset));
		if(_mt3dType[_MT_Osd2PosdID(u4Plane)] == MT_GAL_3D_DISABLE)  //disable Stereo3d to change the frameb pitch
		{
			OSD_RGN_Set(u4TemRgn, OSD_RGN_BMP_PITCH, pSurfaceInfo->pitch);
		}
	}

	if(pSurfaceInfo->pixelFormat != 7)  // this is for YUV color mode useful,
	{
		OSD_RGN_Set(u4TemRgn, OSD_RGN_COLORMODE, _MT_GALColorMode2OSDColorMode(pSurfaceInfo->pixelFormat));
	}
	if(_MT_GALColorMode2OSDColorMode(pSurfaceInfo->pixelFormat) == OSD_CM_RGB_CLUT8)
	{
		OSD_RGN_Set(u4TemRgn, OSD_RGN_PAL_ADDR, _MT_u4Offset2Physical((UINT32)pSurfaceInfo->paletteInfo.pPalette));
		OSD_RGN_Set(u4TemRgn, OSD_RGN_PAL_LEN, 2);
		OSD_RGN_Set(u4TemRgn, OSD_RGN_PAL_LOAD, 1);
	}

	mt_plane->pfn_swap(mt_plane, _MT_u4Offset2Physical(pSurfaceInfo->offset));	
	return MTGAL_OK;
}


MTGAL_RESULT_T MT_GAL_Set3DMode(UINT32 u4Plane, UINT32 u4Enable, UINT32 u4Mode)
{
	MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
	UINT32 region_front;
	UINT32 region_back;

	if (!mt_plane)
	{
		printk("%s: no plane related to the param.(plane:%d)\n", __func__, u4Plane);
		return MTGAL_INVALID_PARAM;
	}

	region_front = *mt_plane->pregion_front;
	region_back  = *mt_plane->pregion_back;

	if(u4Enable)
	{
		_mt3dType[_MT_Osd2PosdID(u4Plane)] = u4Mode;
	}
	else
	{
		u4Mode = _mt3dType[_MT_Osd2PosdID(u4Plane)];
		_mt3dType[_MT_Osd2PosdID(u4Plane)] = MT_GAL_3D_DISABLE;
	}
	
	switch(u4Mode)
	{
	case MT_GAL_2D_TO_3D:
		//OSD_PLA_GetAllRegion(u4Plane, &u4TemRgn1, &u4TempRgn2);
		if(u4Enable)
		{
			OSD_BASE_SetPlaneLineShift(mt_plane->plane_id, u4Enable, MT_GAL_2DTO3D_SHIFT_VALUE);
			OSD_RGN_Set(region_front, OSD_RGN_STEP_V, 0x2000);
			OSD_RGN_Set(region_back , OSD_RGN_STEP_V, 0x2000);
		}
		else
		{
			OSD_BASE_SetPlaneLineShift(mt_plane->plane_id, u4Enable, 0);
			OSD_RGN_Set(region_front, OSD_RGN_STEP_V, 0x1000);
			OSD_RGN_Set(region_back , OSD_RGN_STEP_V, 0x1000);
		}
		break;
	case MT_GAL_S3D:
#if defined(CC_MT5398)||defined(CC_MT5399)||defined(CC_MT5880)||defined(CC_MT5860)
		if(u4Plane == OSD_PLANE_2) 
		{
			printk("posd0 can not use S3D Mode\n");
			return MTGAL_OK;
		}
		if(u4Enable)
		{
			if (IS_PANEL_L12R12)
			{
				MT_GAL_SetStereoMode(MT_GAL_3DPDP_MODE);
			}
			else
			{
				MT_GAL_SetStereoMode(MT_GAL_3DPR_MODE);
			}
		}
		else
		{
			MT_GAL_SetStereoMode(MT_GAL_NORMAL_MODE);
		}
#endif
		break;
	default:
		break;        
	}
	return MTGAL_OK;
}

void MT_GAL_SetStereoMode(UINT32 u4Mode)
{
	INT32 i4Ret;
	UINT32 u4TempPanelWidth = 0;
	UINT32 u4TempPanelHeight = 0;
	u4TempPanelWidth = OSD_BASE_GetDisplayWidth();
	u4TempPanelHeight = OSD_BASE_GetDisplayHeight();

	//printf("MT_GAL_SetStereoMode u4TempPanelWidth =%d, u4TempPanelHeight=%d\n", u4TempPanelWidth,u4TempPanelHeight);
	if (u4Mode == MT_GAL_3DPR_MODE)//AP draw 640x540L+640x540R
	{
		// PR mode buffer 0
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_X, 0);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_Y, 0);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_PITCH, 640 * 4);                
		//i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);   
		// PR mode buffer 1
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_W, 640);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_H, 1080);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_X, 0);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_Y, 0);
		i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_PITCH, 640 * 4);                    
		//i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);      
		
		OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 640, MT_GAL_LCD_PANEL_HEIGHT,
		MT_GAL_LCD_PANEL_WIDTH, MT_GAL_LCD_PANEL_HEIGHT);      
	}
	else if(u4Mode == MT_GAL_3DPDP_MODE) //AP draw 640x720L+640x720R
	{
		int yshift = 0;
#if defined(CC_MT5398) || defined(CC_MT5399)
		if(IS_PANEL_2D_N_3D_L12R12 && IS_PANEL_L12R12)
			yshift = 1;
#endif
		if(u4TempPanelWidth == MT_GAL_HD_PDP_PANEL_WIDTH )
		{
			OSD_BASE_GetOsdPosition(MT_GAL_STEREO_OSD, &u4PdpPosX, &u4PdpPosY);
//			OSD_BASE_SetOsdPosition(MT_GAL_STEREO_OSD,0, (1536 - 720*2)/2 >> yshift ); //set the game locate in the panel cent, panel is 1024 x  1536
			// PR mode buffer 0
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_PITCH, 640 * 4);                
			// i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);   
			// PR mode buffer 1
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_H, 768*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_PITCH, 640 * 4);                    
			//i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);      

			OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 640, 1536, u4TempPanelWidth, 1536 >> 1);  
		}
		else
		{
			OSD_BASE_GetOsdPosition(MT_GAL_STEREO_OSD, &u4PdpPosX, &u4PdpPosY);
//			OSD_BASE_SetOsdPosition(MT_GAL_STEREO_OSD,0, (2160 - 720*2)/2 >> yshift ); //set the game locate in the panel cent, panel is 1920x2160
			OSD_BASE_SetOsdPosition(MT_GAL_STEREO_OSD,0, (2160 - 1023*2)/2 >> yshift ); //set the game locate in the panel cent, panel is 1920x2160
			// PR mode buffer 0
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_PITCH, 640 * 4);                
			//i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);   
			// PR mode buffer 1
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_W, 640);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_H, 1023*2);//720*2);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_PITCH, 640 * 4);                    
			// i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);      

			OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 640, 2046, u4TempPanelWidth, 2046 >> 1);      
		}
	} 	
	else
	{
		if(u4TempPanelWidth == MT_GAL_HD_PDP_PANEL_WIDTH )
		{
			OSD_BASE_SetOsdPosition(MT_GAL_STEREO_OSD,u4PdpPosX, u4PdpPosY ); //restore the default position
			 // PR mode buffer 0
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_PITCH, 1280 * 4);                
			//Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);   
			// PR mode buffer 1
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_PITCH, 1280 * 4);                    
			//Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);      
			
			OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 1280, 720, u4TempPanelWidth, u4TempPanelHeight>>1);  
		}
		else
		{
			OSD_BASE_SetOsdPosition(MT_GAL_STEREO_OSD, u4PdpPosX, u4PdpPosY ); //set the game locate in the panel cent, panel is 1920x2160
			// PR mode buffer 0
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_DISP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_OUT_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_PITCH, 1280 * 4);                
			//Ret = OSD_RGN_Set(_u4Posd1Rgn0, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);   
			// PR mode buffer 1
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_DISP_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_W, 1280);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_OUT_H, 720);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_X, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_POS_Y, 0);
			i4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_PITCH, 1280 * 4);                    
			//4Ret = OSD_RGN_Set(_u4Posd1Rgn1, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);      
			
			if (IS_PANEL_L12R12)
			{
				OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 1280, 720, u4TempPanelWidth, u4TempPanelHeight>>1); 
			}
			else
			{
				OSD_SC_Scale(MT_GAL_STEREO_OSD_SCALER, TRUE, 1280, 720, u4TempPanelWidth, u4TempPanelHeight); 
			}
		}
	}
   
	_mt3dMode = u4Mode;
	printf("_mtk_fb_1_ioctl set 3D mode 1111 = %d\n", _mt3dMode);

	// Indicates 3D mode is not yet applied
	_mt3dModeApplied = 0;

}

MTGAL_RESULT_T	MT_GAL_ResetFrameSurface(UINT32 u4Plane)
{
	if (u4Plane == 0xfffffffful)
	{
		UINT32 i;
		for(i = 0; i < OSD_PLANE_MAX_NUM; ++i)
		{
			MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
			if (!mt_plane)
				return MTGAL_INVALID_PARAM;
			else
			{
				MT_GAL_SURFACE_INFO_T *mt_surface_info;
				MT_GAL_SURFACE_T *mt_surface = mt_plane->linked_surface;
				if (!mt_surface)
					return MTGAL_OK;
				mt_surface_info = mt_surface->surfaceInfo;

				/* ... */
			}
		}
		// trigger
	}
	else
	{
		MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
		if (!mt_plane)
			return MTGAL_INVALID_PARAM;
		else		
		{
			MT_GAL_SURFACE_INFO_T *mt_surface_info;
			MT_GAL_SURFACE_T *mt_surface = mt_plane->linked_surface;
			if (!mt_surface)
				return MTGAL_OK;
			mt_surface_info = mt_surface->surfaceInfo;

			/* ... */	

			// trigger
		}
	}
	return MTGAL_OK;
}

MTGAL_RESULT_T	MT_GAL_GetFrameSurfaceAttributes(UINT32 u4Plane, MT_GAL_PLANE_ATTRIBUTE_T *attributes, INT32 *values)
{
	MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
	MT_GAL_SURFACE_T *mt_surface;
	MT_GAL_SURFACE_INFO_T *mt_surface_info = NULL;
	UINT32 loop = 10; // safe-guard about infinite loop.

	if (!mt_plane)
	{
		printk("%s: no plane related to the param.(plane:%d)\n", __func__, u4Plane);
		return MTGAL_INVALID_PARAM;
	}
	
	mt_surface = mt_plane->linked_surface;	
	if (mt_surface)
		mt_surface_info = mt_surface->surfaceInfo;

	while(loop--)
	{
		switch(*attributes++)
		{
		case MT_GAL_PLANE_ATTRIBUTE_NULL:
			loop = 0;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_RECT_IN:
			values[0] = mt_plane->rect_in.x;
			values[1] = mt_plane->rect_in.y;
			values[2] = mt_plane->rect_in.w;
			values[3] = mt_plane->rect_in.h;
			values += 4;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_RECT_OUT:
			values[0] = mt_plane->rect_out.x;
			values[1] = mt_plane->rect_out.y;
			values[2] = mt_plane->rect_out.w;
			values[3] = mt_plane->rect_out.h;
			values += 4;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_STEREO:
			values[0] = mt_plane->stereo_enable;
			values[1] = mt_plane->stereo_mode;
			values += 2;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_SURFACE:		
			if (!mt_surface || !mt_surface_info)
			{
				printk("%s: there is no surface info linked to this plane.(plane:%d)\n", __func__, u4Plane);
				return MTGAL_NOT_OK;
			}		
			x_memcpy(values, mt_surface_info, sizeof(MT_GAL_SURFACE_INFO_T));
			values += sizeof(MT_GAL_SURFACE_INFO_T)/sizeof(UINT32);
			break;
		default:
			return MTGAL_INVALID_PARAM;
		}
	}
	return MTGAL_OK;
}

MTGAL_RESULT_T	MT_GAL_SetFrameSurfaceAttributes(UINT32 u4Plane, MT_GAL_PLANE_ATTRIBUTE_T *attributes, const INT32 *values)
{
	MT_GAL_PLANE_T *mt_plane = _MT_find_plane(u4Plane);
	MT_GAL_SURFACE_T * mt_new_surface = NULL;
	MT_GAL_SURFACE_INFO_T * mt_new_surface_info = NULL;
	UINT32 loop = 10; // safe-guard about infinite loop.

	if (!mt_plane)
	{
		printk("%s: no plane related to the param.(plane:%d)\n", __func__, u4Plane);
		return MTGAL_INVALID_PARAM;
	}

	while(loop--)
	{
		switch(*attributes++)
		{
		case MT_GAL_PLANE_ATTRIBUTE_NULL:
			loop = 0;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_RECT_IN:
			mt_plane->rect_in.x = values[0];
			mt_plane->rect_in.y = values[1];
			mt_plane->rect_in.w = values[2];
			mt_plane->rect_in.h = values[3];
			values += 4;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_RECT_OUT:
			mt_plane->rect_out.x = values[0];
			mt_plane->rect_out.y = values[1];
			mt_plane->rect_out.w = values[2];
			mt_plane->rect_out.h = values[3];
			values += 4;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_STEREO:
			mt_plane->stereo_enable	= values[0];
			mt_plane->stereo_mode	= values[1];
			values += 2;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_SURFACE:
			{				
				mt_new_surface = _MT_find_surface(_MT_compare_from_offset, values[0]);
				if(!mt_new_surface)
				{
					printk("%s: there is no surface to swap. we can not find it from the param.\n", __func__);
					printk("\t- offset of surface: 0x%08x\n", values[0]);
					return MTGAL_NOT_OK;
				}
				mt_new_surface_info = mt_new_surface->surfaceInfo;
			}
			values += 1;
			break;
		case MT_GAL_PLANE_ATTRIBUTE_RUN:
			{
				UINT32 region_front = *mt_plane->pregion_front;
				UINT32 region_back  = *mt_plane->pregion_back;
#if 0
				printk("MT_GAL_PLANE_ATTRIBUTE_RUN\n");
				printk(" - OSD_PLANE_1:%d, OSD_PLANE_2:%d, OSD_PLANE_3:%d\n", OSD_PLANE_1, OSD_PLANE_2, OSD_PLANE_3);
				printk(" - OSD_SCALER_1:%d, OSD_SCALER_2:%d, OSD_SCALER_3:%d\n", OSD_SCALER_1, OSD_SCALER_2, OSD_SCALER_3);
				printk(" - plane: %d\n", u4Plane);
				printk(" - plane_id: %d, scaler_id: %d\n", mt_plane->plane_id, mt_plane->scaler_id);
				printk(" - first_region_id: 0x%08x\n", first_region_id);
				printk(" - region1: 0x%08x\n", region_id1);
				printk(" - region2: 0x%08x\n", region_id2);
				
				printk(" - rect_in: %d, %d, %d, %d\n", mt_plane->rect_in.x, mt_plane->rect_in.y, mt_plane->rect_in.w, mt_plane->rect_in.h);
				printk(" - rect_out: %d, %d, %d, %d\n", mt_plane->rect_out.x, mt_plane->rect_out.y, mt_plane->rect_out.w, mt_plane->rect_out.h);
#endif
				if (mt_new_surface)
				{
					MT_GAL_SURFACE_T * mt_old_surface = mt_plane->linked_surface;

					/* if there exist the surface to swap... */
					if (mt_old_surface != mt_new_surface)
					{
						mt_plane->linked_surface = mt_new_surface;

						_MT_add_reference(mt_new_surface);
						_MT_del_reference(mt_old_surface);
					}

					if(mt_new_surface_info)
					{
						OSD_RGN_Set(region_back, OSD_RGN_BMP_ADDR, _MT_u4Offset2Physical(mt_new_surface_info->offset));
						OSD_RGN_Set(region_back, OSD_RGN_BMP_PITCH, mt_new_surface_info->pitch);

						if(mt_new_surface_info->pixelFormat != 7)  // this is for YUV color mode useful,
						{
							OSD_RGN_Set(region_back, OSD_RGN_COLORMODE, _MT_GALColorMode2OSDColorMode(mt_new_surface_info->pixelFormat));
						}

						if(mt_new_surface_info->pixelFormat == MT_GAL_PIXEL_FORMAT_LUT8)
						{
							OSD_RGN_Set(region_back, OSD_RGN_PAL_ADDR, _MT_u4Offset2Physical((UINT32)mt_new_surface_info->paletteInfo.pPalette));
							OSD_RGN_Set(region_back, OSD_RGN_PAL_LEN, 2);
							OSD_RGN_Set(region_back, OSD_RGN_PAL_LOAD, 1);
						}
					}
					else
						printk("%s: warning. mt_new_surface_info is NULL.\n");
				}
				else
				{
					mt_new_surface = mt_plane->linked_surface;
					if (mt_new_surface)
						mt_new_surface_info = mt_new_surface->surfaceInfo;
                }


				_MT_set_display_region(mt_plane, region_front, FALSE);
				_MT_set_display_region(mt_plane, region_back, TRUE);

				if (mt_new_surface_info)
					mt_plane->pfn_swap(mt_plane, _MT_u4Offset2Physical(mt_new_surface_info->offset));
				else
					printk(KERN_INFO "%s: no surface linked to the plane:%d\n", __func__, mt_plane->plane_id);
				
			}
			break;
		default:
			return MTGAL_INVALID_PARAM;
		}
	}
	return MTGAL_OK;
}

#if defined(CC_LG_SNAP_SHOT)
void GAL_pm_suspend(void)
{
	MT_GAL_SURFACE_T *mt_surface;

	list_for_each_entry(mt_surface, &mt_surface_list, list_item)
	{
		size_t *prev_size = (size_t *)_MT_u4Offset2Virtual(mt_surface->surfaceInfo->offset - mt_surface->surfaceInfo->aligned_offset) - 2;

		if (mt_surface->surfaceInfo->width <= 256 && mt_surface->surfaceInfo->height <= 256)
		{
#if 1
			mt_surface->suspend_pages = (void *)kmalloc(mt_surface->size, GFP_ATOMIC);
#else
			mt_surface->suspend_pages = (void *)__get_free_pages(GFP_ATOMIC, mt_surface->page_order);
#endif
			if (mt_surface->suspend_pages)
			{
				void *src_pixels = (void *)_MT_u4Offset2Virtual(mt_surface->surfaceInfo->offset);
				memcpy(mt_surface->suspend_pages, src_pixels, mt_surface->size);
			}
		}
		else
			mt_surface->suspend_pages = NULL;

		mt_surface->gfx_prev_size = *(prev_size + 0);
		mt_surface->gfx_size = *(prev_size + 1);
	}
}
EXPORT_SYMBOL(GAL_pm_suspend);

void GAL_pm_resume(void)
{
	MT_GAL_SURFACE_T *mt_surface;
	int i;

	list_for_each_entry(mt_surface, &mt_surface_list, list_item)
	{
		size_t *prev_size = (size_t *)_MT_u4Offset2Virtual(mt_surface->surfaceInfo->offset - mt_surface->surfaceInfo->aligned_offset) - 2;

		if (mt_surface->suspend_pages)
		{
			void *dst_pixels = (void *)_MT_u4Offset2Virtual(mt_surface->surfaceInfo->offset);
			memcpy(dst_pixels, mt_surface->suspend_pages, mt_surface->size);

#if 1
			kfree(mt_surface->suspend_pages);
#else
			free_pages((unsigned long)mt_surface->suspend_pages, mt_surface->page_order);
#endif
			mt_surface->suspend_pages = NULL;
		}

		*(prev_size + 0) = mt_surface->gfx_prev_size;
		*(prev_size + 1) = mt_surface->gfx_size;
	}

	for(i = 1; i < 3; ++i)  /* except for cursor osd */
	{
		if(mt_plane_array[i].linked_surface)
		{
			mt_surface = mt_plane_array[i].linked_surface;
			memset((void *)_MT_u4Offset2Virtual(mt_surface->surfaceInfo->offset), 0, mt_surface->size);
		}
	}
}
EXPORT_SYMBOL(GAL_pm_resume);
#endif /* CC_LG_SNAP_SHOT */
