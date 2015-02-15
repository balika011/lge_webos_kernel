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

#ifndef __MT53_FB__
#define __MT53_FB__
#include "gfx_common_if.h"

#ifdef __KERNEL__
#include "panel.h"
#endif /* ifdef __KERNEL__ */

#define FB_INIT_BY_AP 1
#define FB_DBG 0
#define FB_PROF 0
#define USE_FBM_PREALLOCATION 1

#if FB_PROF
#undef FB_DBG
#define FB_DBG 1
#endif

#ifndef CC_B2R44K2K_SUPPORT
#if defined(CC_SUPPORT_4K2K) || defined(CC_SUPPORT_FULL_3D_PHOTO)
#if ((!defined(ANDROID)) && (!defined(CC_ANDROID_TWO_WORLDS)))
#define CC_B2R44K2K_SUPPORT
#endif
#endif
#endif


#define FBIO_RECTFILL 0x4620
#define FBIO_COPYAREA 0x4621
#define FBIO_IMAGEBLIT 0x4622
#define FBIO_REGISTERBUFFER 0x4623
#define FBIO_UNREGISTERBUFFER 0x4624
#define FBIO_RECTFILLROP 0x4625
#define FBIO_COPYAREAROP 0x4626
#define FBIO_SET 0x4627
#define FBIO_PALETTE 0x4628
#define FBIO_GETIMAGEBUFFER 0x4629
#define FBIO_IMAGEBLITROP 0x462A
#define FBIO_FONTRENDER 0x462B
#define FBIO_SETBLITOPT 0x462c
//new function
#define FBIO_DRAWLINE 0x462D
#define FBIO_DRAWLINEROP 0x462E
#define FBIO_DRAWRECT 0x462F
#define FBIO_DRAWRECTROP 0x4632
#define FBIO_DRAWTEXT 0x4631
#define FBIO_GETIMAGEBUFFER2 0x4633
#define FBIO_HAL_FLUSH_INVALII_DATE_DCACHE 0x4634


// For user mode cmdque
#define FBIO_GFX_FLUSH 0x4640
#define FBIO_GFX_WAIT 0x4641

#define FBIO_GET_PLANE_ORDER_ARRAY 0x4642
#define FBIO_SET_PLANE_ORDER_ARRAY 0x4643
#define FBIO_SET_OSD_PLANE_ORDER_ARRAY 0x4644
#define FBIO_GET 0x4645

// For DirectFB & FB share lock
#define FBIO_GFX_LOCK 0x4646
#define FBIO_GFX_UNLOCK 0x4647
#define FBIO_GFX_STRETCH 0x4648
#define FBIO_WAITLOGO 0x4649
#define FBIO_GFX_WAIT_VDP 0x464A
#define FBIO_IMGRZ_STRETCH_ALCOM 0x464B
#define FBIO_SET_OSD_3DPRLINESHIFT 0x464F

#define ROP_CLEAR       0
#define ROP_DST_IN      1
#define ROP_DST_OUT     2
#define ROP_DST_OVER    3
#define ROP_SRC         4
#define ROP_SRC_IN      5
#define ROP_SRC_OUT     6
#define ROP_SRC_OVER    7
#define ROP_DST         8
#define ROP_SRC_ATOP    9
#define ROP_DST_ATOP    10
#define ROP_XOR_GFX     11

#define FBIO_INIT 0x4630
#define FBIO_GETTIME 0x4631

#define FBIO_GET_SBSOUTPUT 0x464C
#define FBIO_GET_TBBOUTPUT 0x464D
#define FBIO_GET_3DOSD_SHIFT 0x464E

#define FBIO_VGFX_STRETCH_ALCOM       0x4650
#define FBIO_VGFX_SCALER      0x4651

#define FBIO_VDP_INIT 0x4660
#define FBIO_VDP_FLIP 0x4661
#define FBIO_VDP_SET 0x4662
#define FBIO_VDP_ROTATE 0x4663
#define FBIO_VDP_GETBUFFER 0x4664
#define FBIO_VDP_GET_CROP_RECT 0x4665
#define FBIO_VDP_SET_CROP_RECT 0x4666
#define FBIO_VDP_STRETCH_BLIT   0x4667
#define FBIO_FBM_INFO           0x4668
#define FBIO_CRSR_INFO          0x4669
#define FBIO_B2R44K2K_GETBUFFER 0x466A
#define FBIO_B2R_STRETCH_BLIT   0x466B

#define FBIO_GFX_FLUSH_DCACHE   0x4700
#define FBIO_MMU_GETPGD                             0x4670               
#define FBIO_MMU_PINMEM                             0x4671               
#define FBIO_MMU_UNPINMEM                           0x4672           
#define FBIO_MMU_MAPSINGLE                          0x4673             
#define FBIO_MMU_UNMAPSINGLE                        0x4674

#define FBIO_SET_UIMODE                             0x4800
#define FBIO_SET_OSD_OPACITY              0x4801
#define  FBIO_FBHWC_BLIT          0x4802
#define  FBIO_FBHWC_COMPOSE          0x4803
#define  FBIO_FBHWC_SKIP_BLIT          0x4804
#define  FBIO_FBHWC_CLOSE          0x4805
#define  FBIO_FBHWC_LOCK_GFX       0x4806
#define  FBIO_FBHWC_UNLOCK_GFX     0x4807
#define  FBIO_FBHWC_FLUSH_GFX      0x4808
#define FBIO_CHECK_IS4K2K_PANEL              	 	    0x4809
#define  FBIO_CHECK_WAIT_FOR_VSYNC 0x480A

#define MT53FB_SET_X 0
#define MT53FB_SET_Y 1
#define MT53FB_SET_W 2
#define MT53FB_SET_H 3
#define MT53FB_SET_VIRT_W 4
#define MT53FB_SET_VIRT_H 5 
#define MT53FB_SET_OFFSET_X 6
#define MT53FB_SET_OFFSET_Y 7
#define MT53FB_SET_BPP 8
#define MT53FB_SET_CM 9
#define MT53FB_SET_VISIBLE 10
#define MT53FB_SET_BASE 11
#define MT53FB_SET_PITCH 12
#define MT53FB_SET_ORDER 13
#define MT53FB_SET_OPACITY 14
#define MT53FB_SET_MIXSEL 15
#define MT53FB_SET_COLORKEYEN 16
#define MT53FB_SET_COLORKEY 17
#define MT53FB_SET_PLANE_ID 18
#define MT53FB_SET_WAIT_VSYNC 19
#define MT53FB_SET_WAITFORSYNC 20
#define MT53FB_SET_BUFFERMODE 21


#define MT53FB_SET_MASK_X (1u << MT53FB_SET_X)
#define MT53FB_SET_MASK_Y (1u << MT53FB_SET_Y)
#define MT53FB_SET_MASK_W (1u << MT53FB_SET_W)
#define MT53FB_SET_MASK_H (1u << MT53FB_SET_H)
#define MT53FB_SET_MASK_VIRT_W (1u << MT53FB_SET_VIRT_W)
#define MT53FB_SET_MASK_VIRT_H (1u << MT53FB_SET_VIRT_H)
#define MT53FB_SET_MASK_OFFSET_X (1u << MT53FB_SET_OFFSET_X)
#define MT53FB_SET_MASK_OFFSET_Y (1u << MT53FB_SET_OFFSET_Y)
#define MT53FB_SET_MASK_BPP (1u << MT53FB_SET_BPP)
#define MT53FB_SET_MASK_CM (1u << MT53FB_SET_CM)
#define MT53FB_SET_MASK_VISIBLE (1u << MT53FB_SET_VISIBLE)
#define MT53FB_SET_MASK_BASE (1u << MT53FB_SET_BASE)
#define MT53FB_SET_MASK_PITCH (1u << MT53FB_SET_PITCH)
#define MT53FB_SET_MASK_ORDER (1u << MT53FB_SET_ORDER)
#define MT53FB_SET_MASK_OPACITY (1u << MT53FB_SET_OPACITY)
#define MT53FB_SET_MASK_MIXSEL (1u << MT53FB_SET_MIXSEL)
#define MT53FB_SET_MASK_COLORKEY (1u << MT53FB_SET_COLORKEY)
#define MT53FB_SET_MASK_COLORKEYEN (1u << MT53FB_SET_COLORKEYEN)
#define MT53FB_SET_MASK_PLANE_ID (1u << MT53FB_SET_PLANE_ID)
#define MT53FB_SET_MASK_WAIT_VSYNC (1u << MT53FB_SET_WAIT_VSYNC)
#define MT53FB_SET_MASK_WAITFORSYNC (1u << MT53FB_SET_WAITFORSYNC)
#define MT53FB_SET_MASK_BUFFERMODE  (1u << MT53FB_SET_BUFFERMODE)


#define MT53FB_GET_PANEL_SIZE 0
#define MT53FB_GET_GFX_DATA   1
#define MT53FB_GET_DFB_LAYER0_SIZE 2


/* This is for middleware private gl operations.*/
#define FBIO_MW_OP  0x5000
#define FBIO_MW_SET_LANGUAGE  0x5001
#define FBIO_MW_GET_LANGUAGE  0x5002
#define FBIO_MW_SET_LANGUAGE_MODE_CHG 0x5003
#define FBIO_MW_GET_LANGUAGE_MODE_CHG 0x5004

#define FBIO_DRV_ACTION         0x5555
#define FBIO_AP_MOD             0x5556
#define FBIO_GET_FBM            0x6000
#define FBIO_WAIT_VSYNC         0x6001

#define FBIOSET_HWCURSOR_SETTING 0x6002
#define FBIO_PLA_SET            0x6004

#define FBIO_GET_TMP_BUF        0x6005
#define FBIO_GET_TMP_BUF_SIZE   0x6006
#define FBIO_LOCK_TMP_BUF       0x6007
#define FBIO_UNLOCK_TMP_BUF     0x6008

#define FBIO_SET_SIMPLE_CMD     0x6009

#define FBIO_AP_MOD_VUDU        0x1

#define ENABLE_FB_LOG

#ifdef LINUX_TURNKEY_SOLUTION /* MHF Linux: CHUN for res*/
#define MAX_X_RES       1280
#define MAX_Y_RES       720
#else
#define MAX_X_RES       960
#define MAX_Y_RES       540
#endif
#define MAX_PIXEL_BYTES 4

#define PMX_X_OFFSET    ((TV_WIDTH - MAX_X_RES) >> 1)
#define PMX_Y_OFFSET     ((TV_HEIGHT - MAX_Y_RES) >> 1)

/* video dram size */
#define MAX_SIZE (MAX_X_RES * MAX_Y_RES * MAX_PIXEL_BYTES * 2)

#ifdef ENABLE_FB_LOG
#define FB_LOG(msg) fprint(stderr, "[MT53_FB] %s", msg);
#else
#define FB_LOG(msg)
#endif

union palette
{
    unsigned int color;
    unsigned char blue, green, red, alpha;
};

typedef enum _E_OAD_BUF
{
    E_OAD_BUF_ONE,
    E_OAD_BUF_TWO,
    E_OAD_BUF_MAX,
}E_OAD_BUF;

typedef enum
{
    E_GFXLIB_SRC_X_ERR = 1000,
    E_GFXLIB_SRC_Y_ERR,
    E_GFXLIB_DST_X_ERR,
    E_GFXLIB_DST_Y_ERR,
    E_GFXLIB_WIDTH_ERR,
    E_GFXLIB_HEIGHT_ERR,
    E_GFXLIB_SRC_PITCH_ERR,
    E_GFXLIB_DST_PITCH_ERR,
    E_GFXLIB_SRC_ADDR_ERR,
    E_GFXLIB_DST_ADDR_ERR,
    E_GFXLIB_ROP_ERR,
    E_GFXLIB_PALE_ADDR_ERR,
    E_GFXLIB_STRETCH_ERR,

    E_GFXLIB_SET_X_ERR,
    E_GFXLIB_SET_Y_ERR,
    E_GFXLIB_SET_WIDTH_ERR,
    E_GFXLIB_SET_HEIGHT_ERR,
    E_GFXLIB_SET_PITCH_ERR,
    E_GFXLIB_SET_BPP_ERR,
    E_GFXLIB_SET_ADDR_ERR,
    E_GFXLIB_SET_COLORMODE_ERR,
    E_GFXLIB_SET_ENABLE_ERR,
    E_GFXLIB_SET_ChangeResolution_ERR,

    E_GFXLIB_ERR_MAX
} E_GFXLIB_T;


//---------------------------------------------------------------------------
// HW Cursor Setting
//---------------------------------------------------------------------------


#define HWCURSOR_SET_REGION      0
#define HWCURSOR_SET_POSITION    1
#define HWCURSOR_SET_ALPHA       2
#define HWCURSOR_GET_PANELSIZE   3


typedef struct HWCursorSetting
{
	 unsigned long u4Op;

	 union
	 {
		 struct
		 {
			 unsigned int width;
			 unsigned int height;
			 unsigned int pitch;
			 unsigned int base;
		 }set_region;

		 struct
		 {
			 unsigned int pos_x;
			 unsigned int pos_y;
		 }set_position;

		 struct
		 {
			 unsigned int alpha;
		 }set_alpha;

		 struct
		 {
		     unsigned int out_width;
			 unsigned int out_height;
		 }get_panelsize;		 
	 }u;
	 struct
	 {
	     unsigned int posX;
	     unsigned int posY;
	     unsigned int width;
	     unsigned int height;
	 }viewport;
}HWCursorSetting;



#define MW_GL_OP_FILL_RECT ((unsigned long)0x1)
#define MW_GL_OP_SET_PLANE ((unsigned long)0x2)
#define MW_GL_OP_GET_FBM_BITMAP_INFO ((unsigned long)0x3)
#define MW_GL_OP_ENABLE_SCALER          ((unsigned long)0x4)
#define MW_GL_OP_RELEASE_FBG            ((unsigned long)0x5)
#define MW_GL_OP_SET_B2R_4K2K           ((unsigned long)0x6)
#define MW_GL_OP_SET_B2R_FHD            ((unsigned long)0x7)
#define MW_GL_OP_SET_B2R_3DPARM         ((unsigned long)0x8)
#define MW_GL_OP_GET_B2R_INFO           ((unsigned long)0x9)

typedef struct mw_gl_op_t_
{
    unsigned long u4Op;
    union
    {
        struct
        {
            unsigned e_mode;
            void* pv_bits;
            unsigned ui4_pitch;
            int i4_x;
            int i4_y;
            unsigned ui4_width;
            unsigned ui4_height;
            unsigned ui4_color;
        } fill_rect;
        struct
        {
            int i4_idx;
            int i4_enable;
        } set_plane;
        struct
        {
            unsigned int ui4_size;
            void* pv_bits;
        } get_fbm_bitmap_info;
        struct
        {
            unsigned int ui4_id;
            unsigned int ui4_enable;
            unsigned int ui4_width;
            unsigned int ui4_height;
        } enable_scaler;
		struct
		{
			unsigned int u4Is3DMode;	
			unsigned int u4CropBottomOffset;
			unsigned int u4CropTopOffset;
			unsigned int u4CropLeftOffset;
			unsigned int u4CropRightOffset;		
		}b2r_3dparm;
        struct
        {
            unsigned int u4YCOffset;
        }b2r_info;
    } u;
} mw_gl_op_t;

struct mt53fb_vdp_init
{
    unsigned int width;
    unsigned int height;
};

struct mt53fb_vdp_flip
{
    unsigned char ucFbgId;
    unsigned int ui4_pitch;
    unsigned char* pu1SrcYAddr;
    unsigned char* pu1SrcCAddr;
    unsigned char fgIs3DMode;
    unsigned int u4CropBottomOffset;
    unsigned int u4CropTopOffset;
    unsigned int u4CropLeftOffset;
    unsigned int u4CropRightOffset;
};

struct mt53fb_vdp_set
{
    unsigned char ucEnable;
};

typedef struct _mt53fb_vdp_crop_rect
{
    unsigned int ui4_layer;
    unsigned int ui4_3d_mode;
    unsigned int crop_left_offset; /* pixel width */
    unsigned int crop_right_offset; /* pixel height */
    unsigned int crop_top_offset; /* pixel width */
    unsigned int crop_bottom_offset; /* pixel height */
} mt53fb_vdp_crop_rect;

typedef enum
{
    MT53_LAYER_OSD1, 
    MT53_LAYER_OSD2, 
    MT53_LAYER_OSD3, 
    MT53_LAYER_VDP1,
    MT53_LAYER_CURSOR,
    MT53_LAYER_VDP2,
    MT53_LAYER_CONTAINER,
    MT53_LAYER_MAX
} MT53LayerID;

//---------------------------------------------------------------------------
// Macro
//---------------------------------------------------------------------------
#define PITCH_VALID(p) (((p & 0xF)==0)?1:0)
#define ADDR_VALID(p, fbp) (((p>=fbp->fix.smem_start)&&(p<fbp->fix.smem_start+fbp->fix.smem_len))?1:0)

#define MT53FB_LOG              (0x0)

#define  DFB_GFX_FLUSH_INVALID_DCACHE           0x1
#define  DFB_GFX_INVALID_DCACHE                 0x2
#define  DFB_GFX_FLUSH_DCACHE                   0x4

typedef struct _MT53_PLA_SET
{
    unsigned int u4Plane;

	unsigned int u4Enable;
}MT53_PLA_SET;


typedef struct _EX_DeviceData
{
    MT53_CACHE_DATA cache;
    GFX_MMU_T mmu;
} EX_DeviceData;

struct mt53fb_DrawLine
{
    unsigned short u2X1;
    unsigned short u2Y1;
    unsigned short u2X2;
    unsigned short u2Y2;
    unsigned short u2DstPitch;
    unsigned int u4color;
    unsigned char* pu1DstBaseAddr;
    unsigned int u4Bpp;
};

struct mt53fb_DrawLineRop
{
    unsigned short u2X1;
    unsigned short u2Y1;
    unsigned short u2X2;
    unsigned short u2Y2;
    unsigned short u2DstPitch;
    unsigned int u4color;
    unsigned int u4Rop;
    unsigned short u2Alpha;
    unsigned char* pu1DstBaseAddr;
    unsigned int u4Bpp;
};

struct mt53fb_DrawRect
{
    unsigned short u2DstX;
    unsigned short u2DstY;
    unsigned short u2DstPitch;
    // Rectangular's width and height
    unsigned short u2Width;
    unsigned short u2Height;
    // Destiniation paper
    unsigned char* pu1DstBaseAddr;
    // Color
    unsigned int u4Color;
    unsigned int u4Bpp;
    unsigned int u4BoarderWidth;
};

struct mt53fb_DrawRectRop
{
    unsigned short u2DstX;
    unsigned short u2DstY;
    unsigned short u2DstPitch;
    // Rectangular's width and height
    unsigned short u2Width;
    unsigned short u2Height;
    // Destiniation paper
    unsigned char* pu1DstBaseAddr;
    // Color
    unsigned int u4Color;
    unsigned int u4Alpha;
    unsigned int u4Rop;
    unsigned int u4Bpp;
    unsigned int u4BoarderWidth;
};

struct mt53fb_DrawText
{
    unsigned short u2X;
    unsigned short u2Y;
    unsigned short u2Width;
    unsigned short u2Height;
    unsigned short u2DstPitch;
    unsigned char* pTextArray;
    unsigned char* pu1DstBaseAddr;
    unsigned int u4Color;
    unsigned int u4Alpha;
    unsigned int u4Bpp;
};

struct mt53fb_imageblit
{
    unsigned short u2SrcX;
    unsigned short u2SrcY;
    unsigned short u2SrcPitch;
    unsigned short u2DstX;
    unsigned short u2DstY;
    unsigned short u2DstPitch;
    // Blitting area's width and height
    unsigned short u2Width;
    unsigned short u2Height;
    // Source paper
    unsigned char* pu1SrcBaseAddr;
    // Destination paper
    unsigned char* pu1DstBaseAddr;

    int i4SrcBpp;
    int i4DstBpp;

};

struct mt53fb_imageblitrop
{
    unsigned short u2SrcX;
    unsigned short u2SrcY;
    unsigned short u2SrcPitch;
    unsigned short u2DstX;
    unsigned short u2DstY;
    unsigned short u2DstPitch;
    // Blitting area's width and height
    unsigned short u2Width;
    unsigned short u2Height;
    // Source paper
    unsigned char* pu1SrcBaseAddr;
    // Destination paper
    unsigned char* pu1DstBaseAddr;

    int i4Alpha;
    int i4Rop;

    int i4SrcBpp;
    int i4DstBpp;

};

struct mt53fb_fillrect
{
    unsigned int u2DstX;
    unsigned int u2DstY;
    unsigned int u2DstPitch;
    // Rectangular's width and height
    unsigned int u2Width;
    unsigned int u2Height;
    // Destiniation paper
    unsigned char* pu1DstBaseAddr;
    // Color
    unsigned int i4Color;

    unsigned int i4Bpp;
    GFX_MMU_T mmu;
};

struct mt53fb_flush_cache
{
    unsigned int pu1DstSize;
    unsigned char* pu1DstBaseAddr;
};
struct mt53fb_fillrectrop
{
    unsigned short u2DstX;
    unsigned short u2DstY;
    unsigned short u2DstPitch;
    // Rectangular's width and height
    unsigned short u2Width;
    unsigned short u2Height;
    // Destiniation paper
    unsigned char* pu1DstBaseAddr;
    // Color
    int i4Color;

    int i4Alpha;
    int i4Rop;

    int i4Bpp;
};

struct mt53fb_copyarearop
{
    unsigned int dx;
    unsigned int dy;
    unsigned int width;
    unsigned int height;
    unsigned int sx;
    unsigned int sy;
    unsigned int rop;
    unsigned int alpha;
};

struct mt53fb_setting
{
    unsigned int u4Pitch;
    unsigned int u1CM;
    unsigned int u2X;
    unsigned int u2Y;
    unsigned int u2W;
    unsigned int u2H;
    unsigned int u2OffsetX;
    unsigned int u2OffsetY;
    unsigned int fgVisible;
    unsigned int u4Base;
    unsigned int u4Order;
    unsigned int u4Opacity;
    unsigned int u4MixSel;
    unsigned int u4ColorKey;
    unsigned int u4ColorKeyEn;
    unsigned int u4OsdPlaneID;
	unsigned int u4BufferMode;
    GFX_MMU_T mmu;
};




struct mt53fb_compress
{
    unsigned int u4Src;
    unsigned int u4Dst;
    unsigned int eCm;
    unsigned int u4Pitch;
    unsigned int u4SrcX;
    unsigned int u4SrcY;
    unsigned int u4Width;
    unsigned int u4Height;
    unsigned int u4DstX;
    unsigned int u4DstY;
    unsigned int u4_blt_fg;
    GFX_MMU_T mmu;
};

#define MT53FB_VDP_ROTATE_90 0 
#define MT53FB_VDP_ROTATE_270 1
#define MT53FB_VDP_ROTATE_180 2

struct mt53fb_vdp_rotate
{
    unsigned int u4Src;
    unsigned int u4Dst;
    unsigned int eSrcCm;
    unsigned int eDstCm;
    unsigned int u4SrcPitch;
    unsigned int u4DstPitch;
    unsigned int u4SrcX;
    unsigned int u4SrcY;
    unsigned int u4Width;
    unsigned int u4Height;
    unsigned int u4DstX;
    unsigned int u4DstY;
    unsigned int u4RotateOp;
    unsigned int u4SrcCbCrOffset;
    unsigned int u4DstCbCrOffset;
};

typedef struct _mt53fb_vdp_stretch_blit_t
{
    unsigned int src1_buffer_Y_addr;
    unsigned int src1_buffer_C_addr;
    unsigned int src1_buffer_Y_width;
    unsigned int src1_buffer_Y_height;
    unsigned int src1_buffer_Y_pitch;
    unsigned int src2_buffer_Y_addr;
    unsigned int src2_buffer_C_addr;
    unsigned int dest_buffer_Y_addr;
    unsigned int dest_buffer_C_addr;
    unsigned int dest_buffer_Y_width;
    unsigned int dest_buffer_Y_height;
    unsigned int dest_buffer_Y_pitch;
    unsigned int composition_level;
    
    unsigned int src_clip_x;
    unsigned int src_clip_y;
    unsigned int src_clip_w;
    unsigned int src_clip_h;

    unsigned int dest_clip_x;
    unsigned int dest_clip_y;
    unsigned int dest_clip_w;
    unsigned int dest_clip_h;
} mt53fb_vdp_stretch_blit;

struct mt53fb_set
{
    struct mt53fb_setting rSetting;
    unsigned int mask;
};

struct mt53fb_get
{
    unsigned int get_type;
    unsigned int get_size;
    unsigned int *get_data;
};

struct mt53fb_palette
{
    unsigned int plane_id;
    unsigned int palette[256];
};

typedef struct _IOMMU_PIN_RANGE_T
{
    unsigned long start;
    unsigned long size;
    unsigned long pageArray;
    unsigned long nrPages;
} IOMMU_PIN_RANGE_T;

//new feature


struct mt53fb_imagebuffer
{
    unsigned int u4Size;
    unsigned int u4VirtAddr;
    unsigned int u4PhyAddr;
};

//20101227
struct mt53fb_3dprlineshift
{
    unsigned int enable;
    unsigned int shiftvalue;
};

typedef struct B2R44K2K_Buffer
{
	unsigned char m_Fd;
	unsigned int  m_YAddr;
	unsigned int  m_CAddr;
	unsigned int  m_VAddr;
	unsigned int  m_Size;
	unsigned int  m_State;
}B2R44K2K_Buffer;

#define B2R_BUFFER_NUM           4

#define B2R_BUFFER_MASK          ((1<<B2R_BUFFER_NUM)-1)

/*==========================================*/
/*VGFX*/
typedef enum
{
    MT53_E_AC_CLEAR = 0,
    MT53_E_AC_DST_IN,
    MT53_E_AC_DST_OUT,
    MT53_E_AC_DST_OVER,
    MT53_E_AC_SRC,
    MT53_E_AC_SRC_IN,
    MT53_E_AC_SRC_OUT,
    MT53_E_AC_SRC_OVER,
    MT53_E_AC_DST,
    MT53_E_AC_SRC_ATOP,
    MT53_E_AC_DST_ATOP,
    MT53_E_AC_XOR,
    MT53_E_AC_NONE,
    MT53_E_AC_ADD,
} MT53_GFX_AC_MODE_T;

typedef enum
{
    MT53_VGX_ALPHA_EDGE_NEAREST, // argb nearest neighbor
    MT53_VGX_ALPHA_EDGE_ZERO, // a=0, rgb nearest neighbor
    MT53_VGX_ALPHA_EDGE_NORMAL, //normal
    MT53_VGX_ALPHA_EDGE_MAX
} MT53_ALPHA_EDGE_MD;

typedef enum
{
    MT53_VGFX_FILTER_2TAP, MT53_VGFX_FILTER_4TAP,
} MT53_VGFX_FILTER_T;

typedef struct _MT53_VGFX_ALPHACOMP_SET_T
{
    unsigned long u4AlphaCompEn;
    unsigned long u4AlphaCompAr;
    unsigned long u4PreSrcRd;
    unsigned long u4PreDstRd;
    unsigned long u4PreDstWt;
    unsigned long fgAlphaCompNormal;
    MT53_ALPHA_EDGE_MD _AlphaEdgeMd;
    MT53_GFX_AC_MODE_T u4AlphaCompOpCode;
} MT53_VGFX_ALPHACOMP_SET_T;

typedef struct _MT53_VGFX_WB_SET_T
{
    unsigned long u4BaseAddr;
    unsigned long u4ColrMode;
    unsigned long u4WBWidth;
    unsigned long u4WBHeight;
    unsigned long u4GlAr;
    unsigned long u4Pitch;
} MT53_VGFX_WB_SET_T;

typedef struct _MT53_VGFX_QUADRILATERAL_SET_T
{
    unsigned long x1;
    unsigned long y1;
    unsigned long x2;
    unsigned long y2;
    unsigned long x3;
    unsigned long y3;
    unsigned long x4;
    unsigned long y4;
} MT53_VGFX_QUADRILATERAL_SET_T;

typedef struct _MT53_VGFX_IMGAGE_SET_T
{
    unsigned long u4BaseAddr;
    unsigned long u4ColrMode;
    unsigned long u4GlAr;
    unsigned long u4Pitch;
} MT53_VGFX_IMGAGE_SET_T;

typedef struct _MT53_VGFX_TEXTURE_SET_T
{
    unsigned long u4StartX;
    unsigned long u4StartY;
    unsigned long u4EndX;
    unsigned long u4EndY;
} MT53_VGFX_TEXTURE_SET_T;

typedef struct _MT53_VGFX_BB_SET_T
{
    unsigned long u4StartX;
    unsigned long u4StartY;
    unsigned long u4Width;
    unsigned long u4Height;
} MT53_VGFX_BB_SET_T;

typedef struct _MT53_VGFX_RECT_SET_T
{
    unsigned long u4StartX;
    unsigned long u4StartY;
    unsigned long u4Width;
    unsigned long u4Height;
} MT53_VGFX_RECT_SET_T;

typedef struct _MT53_VGFX_REFLECT_SET_T
{
    unsigned long u4RefEn;
    unsigned long u4RefOnly;
    unsigned long u4TextTureHeight;
    unsigned long u4RefRatioInit;
    unsigned long u4RatioLineStep;
    unsigned long u4RefAAEn;
} MT53_VGFX_REFLECT_SET_T;

typedef struct _MT53_VGFX_FILTER_SET_T
{
    unsigned long u4FilterEn;
    MT53_VGFX_FILTER_T _FilterType;
} MT53_VGFX_FILTER_SET_T;

typedef struct _MT53_VGFX_MATRIX_T
{
    unsigned long u4_a;
    unsigned long u4_b;
    unsigned long u4_c;
    unsigned long u4_d;
    unsigned long u4_e;
    unsigned long u4_f;
    unsigned long u4_g;
    unsigned long u4_h;
    unsigned long u4_i;
} MT53_VGFX_MATRIX_T;

typedef struct _MT53_VGFX_SCALER_PARAM_SET_T
{
    unsigned long u4AntiAliasing;
    MT53_VGFX_WB_SET_T _WBType;
    MT53_VGFX_IMGAGE_SET_T _IMAGType;
    MT53_VGFX_BB_SET_T _BBType;
    MT53_VGFX_MATRIX_T _SetMartix;

    MT53_VGFX_QUADRILATERAL_SET_T _SrcQuard;
    MT53_VGFX_QUADRILATERAL_SET_T _DstQuard;

    MT53_VGFX_REFLECT_SET_T _SetReflect;
    MT53_VGFX_ALPHACOMP_SET_T _SetAlphaComp;
    MT53_VGFX_FILTER_SET_T _FilterType;

} MT53_VGFX_SCALER_PARAM_SET_T;

typedef struct _mt53fb_b2r_stretch_blit_t
{
    unsigned int src_buffer_Y_addr;
    unsigned int src_buffer_C_addr;
    unsigned int src_buffer_Y_width;
    unsigned int src_buffer_Y_height;
    unsigned int src_buffer_Y_pitch;
    unsigned int dest_buffer_Y_addr;
    unsigned int dest_buffer_C_addr;
    unsigned int dest_buffer_Y_width;
    unsigned int dest_buffer_Y_height;
    unsigned int dest_buffer_Y_pitch;

    unsigned int src_clip_x;
    unsigned int src_clip_y;
    unsigned int src_clip_w;
    unsigned int src_clip_h;

    unsigned int dest_clip_x;
    unsigned int dest_clip_y;
    unsigned int dest_clip_w;
    unsigned int dest_clip_h;

} mt53fb_b2r_stretch_blit;

#ifdef __KERNEL__

struct iospace
{
    volatile unsigned int __iomem *vbase;
    unsigned int pbase;
    unsigned int len;
};

struct mt53fb_par
{
    struct device *dev;
    /* total dram size allocated when startup */
    unsigned int dram_size;
    /* single osd region memory base */
    struct
    {
        unsigned char __iomem *vbase;
        unsigned int pbase;
        unsigned int len;
    }mem;
    /* frame buffer hardware io space */
    struct iospace osdio;
    /* graphic accelerator engine io space */
    struct iospace gfxio;
    /* palette */
    union palette pal[256];
    unsigned int pseudo_palette[256];
};

typedef struct fbdev_blit
{
    unsigned int height;
    unsigned int width;
    unsigned int bytes_per_pixel;
    unsigned int src_addr;
    unsigned int src_x;
    unsigned int src_y;
    unsigned int src_pitch;
    unsigned int dst_addr;
    unsigned int dst_x;
    unsigned int dst_y;
    unsigned int dst_pitch;
	unsigned int blend; //0:blit 1:blend
}fbdev_blit;
extern INT32 mtfb_compress_enable(UINT32 u4_plane, UINT32 u4_enable);
extern INT32 mtfb_get_cmp_state(UINT32 u4_plane);
extern INT32 mtfb_borrow_cmp_buf(UINT32 u4_plane,UINT32 u4_plane_cmp);
extern UINT32 fb_osd_get_plane_basic_mem_idx(UINT32 u4_plane);
extern UINT32 fb_osd_get_plane_3d_mem_idx(UINT32 u4_plane);
extern UINT32 fb_osd_get_plane_dec_mem_idx(UINT32 u4_plane);
#else
extern void system_get_fbm_buf(struct mt53fb_imagebuffer *pt_fbm);
extern void system_get_tmp_buf(struct mt53fb_imagebuffer *pt_fbm);
extern void system_lock_tmp_buf(void);
extern void system_unlock_tmp_buf(void);

#endif

#endif /*__MT53_FB__*/

