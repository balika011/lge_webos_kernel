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
/*-----------------------------------------------------------------------------
 * $RCSfile: jdsample.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/11 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 30a68c0f9438000cf72b2a6891c1f1a7 $ Asa
 *
 * Description:
 *         This file contains bilinear resample functions
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "djpeg_sw_api.h"

#ifdef HW_SCALER
    #include "res_mngr/rm.h"
    #include "res_mngr/x_rm_dev_types.h"
    #include "graphic/_gl.h"
#endif

#define MW_IMG_JPG_ARGB_OUTPUT
/* For MT538x, the default output color mode is AYUV */
#ifndef MW_IMG_JPG_ARGB_OUTPUT
    #undef HW_YCBCR
#endif /* HW_YCBCR */

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define SCALEBITS 16        /* speediest right-shift on some machines */
#define ONE_HALF  ((INT32) 1 << (SCALEBITS-1))
#define FIX1(x)    ((INT32) ((x) * (1L<<SCALEBITS) + 0.5))
#define PUT_2B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
    array[offset+1] = (char) (((value) >> 8) & 0xFF))
    #define PUT_4B(array,offset,value)  \
    (array[offset] = (char) ((value) & 0xFF), \
    array[offset+1] = (char) (((value) >> 8) & 0xFF), \
    array[offset+2] = (char) (((value) >> 16) & 0xFF), \
    array[offset+3] = (char) (((value) >> 24) & 0xFF))
#define SCALEBITS 16        /* speediest right-shift on some machines */
#define RIGHT_SHIFT(x,shft) ((x) >> (shft))

#define RANGE_LIMIT(result, x) \
{ \
    if (!(x & 0xff00))  \
        (result) = (UINT8)(x); \
    else {\
        if (x >0)\
            (result) = MAXSAMPLE; \
           else\
        (result) = 0;\
           }\
}
/* color composition macro */
#define COLOR_RGB565(r, g, b)        (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3))
#define COLOR_ARGB1555(a, r, g, b)   (((a != 0) ?  0x8000 : 0) | ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3))
#define COLOR_ARGB4444(a, r, g, b)   (((a & 0xf0) << 8) | ((r & 0xf0) << 4) | (g & 0xf0) | ( (b & 0xf0) >> 4))
#define COLOR_ARGB8888(a, r, g, b)   ((a << 24) | (r<< 16) | (g << 8) | b)
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
#ifdef HW_SCALER
UINT8       *_pui1_y_buf = NULL, *_pui1_cb_buf = NULL, *_pui1_cr_buf = NULL;
UINT8       *_pui1_dst_y_buf = NULL, *_pui1_dst_cb_buf = NULL, *_pui1_dst_cr_buf = NULL;
HANDLE_T    _h_y_surf = NULL_HANDLE, _h_cb_surf = NULL_HANDLE, _h_cr_surf = NULL_HANDLE;
HANDLE_T    _h_dst_y_surf = NULL_HANDLE, _h_dst_cb_surf = NULL_HANDLE, _h_dst_cr_surf = NULL_HANDLE;

UINT32      _ui4_y_pitch, _ui4_cb_pitch, _ui4_cr_pitch;
UINT32      _ui4_dst_y_pitch, _ui4_dst_cb_pitch, _ui4_dst_cr_pitch;

DRV_SET_TYPE_T  _e_gfx_op;

#ifdef HW_YCBCR
UINT8       *_pui1_cbcr_buf = NULL;
HANDLE_T    _h_cbcr_surf = NULL_HANDLE;

UINT32      _ui4_cbcr_pitch;

#endif /* HW_YCBCR */

#endif

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
#ifdef HW_SCALER
static VOID _bitblt_buffer(
    UINT8           *pui1_dst,
    UINT8           *pui1_src,
    INT32           i4_dst_x,
    INT32           i4_dst_y,
    UINT32          ui4_width,
    UINT32          ui4_height,
    UINT32          ui4_dst_pitch,
    UINT32          ui4_src_pitch);

static VOID _prepare_input_buffer(
    UINT8           *pui1_dst,
    UINT8           *pui1_src,
    UINT8           *pui1_next_src,
    UINT32          ui4_width,
    UINT32          ui4_height,
    UINT32          ui4_dst_pitch,
    UINT32          ui4_src_pitch);

static VOID _scale_buffer(
    UINT8           *pui1_dst,
    UINT8           *pui1_src,
    UINT32          ui4_dst_width,
    UINT32          ui4_dst_height,
    INT32           i4_src_x,
    INT32           i4_src_y,
    UINT32          ui4_src_width,
    UINT32          ui4_src_height,
    UINT32          ui4_dst_pitch,
    UINT32          ui4_src_pitch,
    DRV_SET_TYPE_T  e_gfx_op);

static UINT8 *_create_surface_buffer(
    GL_HSURFACE_T   *ph_surf,
    UINT32          ui4_width,
    UINT32          ui4_height,
    UINT32          *ui4_pitch);

static VOID _flush_buffer(
    HANDLE_T        *ph_surf);

VOID hw_set_scaler_mode(
    DRV_SET_TYPE_T  e_gfx_op);

static VOID _release_surface(
    HANDLE_T        *ph_surf);

extern INT32 _flush_cmd(UINT8 ui1_num, const HANDLE_T* ph_signal, UINT32 ui4_flag);

#ifdef HW_YCBCR
static VOID _merge_cbcr_buf(
    UINT8   *pui1_dst_cbcr_buf,
    UINT32  ui4_dst_cbcr_pitch,
    UINT32  ui4_dst_width,
    UINT32  ui4_dst_height,
    UINT8   *pui1_cb_buf,
    UINT32  ui4_cb_pitch,
    UINT8   *pui1_cr_buf,
    UINT32  ui4_cr_pitch);

static VOID _color_convert(
    UINT8           *pui1_dst_buf,
    UINT32          ui4_dst_pitch,
    INT32           i4_dst_x,
    INT32           i4_dst_y,
    GL_COLORMODE_T  e_dst_colormode,
    UINT8           *pui1_y_buf,
    UINT32          ui4_y_pitch,
    INT32           i4_src_x,
    INT32           i4_src_y,
    UINT8           *pui1_cbcr_buf,
    UINT32          ui4_cbcr_pitch,
    UINT32          ui4_width,
    UINT32          ui4_height);

#endif /* HW_YCBCR */

#endif /* HW_SCALER */

/*-----------------------------------------------------------------------------
 * Name:  vBuild_ycc_rgb_table
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vBuild_ycc_rgb_table (INT16 * Cr_r_tab, INT16 * Cb_b_tab, INT32 * Cr_g_tab, INT32 * Cb_g_tab)
{
#if !defined(HW_YCBCR) && defined(MW_IMG_JPG_ARGB_OUTPUT)
    INT16 i;
    INT32 x;

    for (i = 0, x = -CENTERSAMPLE; i <= MAXSAMPLE; i++, x++)
    {
        /* i is the actual input pixel value, in the range 0..MAXJSAMPLE */
        /* The Cb or Cr value we are thinking of is x = i - CENTERJSAMPLE */
        /* Cr=>R value is nearest int to 1.40200 * x */
        Cr_r_tab[i] = (INT16) RIGHT_SHIFT (FIX1 (1.40200) * x + ONE_HALF, SCALEBITS);
        /* Cb=>B value is nearest int to 1.77200 * x */
        Cb_b_tab[i] = (INT16) RIGHT_SHIFT (FIX1 (1.77200) * x + ONE_HALF, SCALEBITS);
        /* Cr=>G value is scaled-up -0.71414 * x */
        Cr_g_tab[i] = (-FIX1 (0.71414)) * x;
        /* Cb=>G value is scaled-up -0.34414 * x */
        /* We also add in ONE_HALF so that need not do it in inner loop */
        Cb_g_tab[i] = (-FIX1 (0.34414)) * x + ONE_HALF;
    }
#endif
}

#if 0
/*-----------------------------------------------------------------------------
 * Name:  blit_D565_0_pixel_sample_phase1
 *
 * Description: bitl D565 0 rotation pixel phase 1 function, get the common part of blit_D565_0_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D565_0_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D565_0_pixel_sample_phase2
 *
 * Description: bitl D565 0 rotation pixel phase 2 function, do D565 rotation degree 0 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D565_0_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y + y) * pt_lock_info->at_subplane[0].ui4_pitch +
         ((pt_src_dst_rect->ui4_dst_x + x) * sizeof(UINT16)));
    */

    pui2_Img = (UINT16*) (
                (UINT8*)(pui1_phase1_data) +
                y * ui4_pitch +
                x * sizeof(UINT16)
                );

    *pui2_Img = (UINT16) COLOR_RGB565(R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D565_90_pixel_sample_phase1
 *
 * Description: bitl D565 90 rotation pixel phase 1 function, get the common part of blit_D565_90_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D565_90_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - 1/*zero base*/) * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D565_90_pixel_sample_phase2
 *
 * Description: bitl D565 90 rotation pixel phase 2 function, do D565 rotation degree 90 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D565_90_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
     pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        ((pt_src_dst_rect->ui4_dst_y + x) * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - y - 1) * sizeof(UINT16));
    */                                                          /*zero base*/

    pui2_Img = (UINT16*) (
                (UINT8*)(pui1_phase1_data) +
                x * ui4_pitch -
                y * sizeof(UINT16)
                );

    *pui2_Img = (UINT16) COLOR_RGB565(R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D565_180_pixel_sample_phase1
 *
 * Description: bitl D565 180 rotation pixel phase 1 function, get the common part of blit_D565_180_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D565_180_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return  (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - 1) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - 1) * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D565_180_pixel_sample_phase2
 *
 * Description: bitl D565 180 rotation pixel phase 2 function, do D565 rotation degree 180 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D565_180_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - y - 1) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - x - 1) * sizeof(UINT16));
    */                                                      /*zero base*/
    pui2_Img = (UINT16*) (
            (UINT8*)(pui1_phase1_data) -
            y * ui4_pitch -
            x * sizeof(UINT16)
            );

    *pui2_Img = (UINT16) COLOR_RGB565(R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D565_270_pixel_sample_phase1
 *
 * Description: bitl D565 270 rotation pixel phase 1 function, get the common part of blit_D565_270_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D565_270_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return  (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - 1) * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D565_270_pixel_sample_phase2
 *
 * Description: bitl D565 270 rotation pixel phase 2 function, do D565 rotation degree 270 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D565_270_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - x - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        ((pt_src_dst_rect->ui4_dst_x + y) * sizeof(UINT16)));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) -
        x * ui4_pitch +
        y * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_RGB565(R, G, B);
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_0_pixel_sample_phase1
 *
 * Description: bitl D1555 0 rotation pixel phase 1 function, get the common part of blit_D1555_0_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D1555_0_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
         );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_0_pixel_sample_phase2
 *
 * Description: bitl D1555 0 rotation pixel phase 2 function, do D1555 rotation degree 0 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D1555_0_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y + y) * pt_lock_info->at_subplane[0].ui4_pitch +
         ((pt_src_dst_rect->ui4_dst_x + x) * sizeof(UINT16)));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) +
        y * ui4_pitch +
        x * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB1555(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_90_pixel_sample_phase1
 *
 * Description: bitl D1555 90 rotation pixel phase 1 function, get the common part of blit_D1555_90_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D1555_90_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return  (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - 1/*zero base*/) * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_90_pixel_sample_phase2
 *
 * Description: bitl D1555 90 rotation pixel phase 2 function, do D1555 rotation degree 90 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D1555_90_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        ((pt_src_dst_rect->ui4_dst_y + x) * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - y - 1//zero base) * sizeof(UINT16));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) +
        x * ui4_pitch -
        y * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB1555(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_180_pixel_sample_phase1
 *
 * Description: bitl D1555 180 rotation pixel phase 1 function, get the common part of blit_D1555_180_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D1555_180_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return  ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - 1) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x -1) * sizeof(UINT16));
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_180_pixel_sample_phase2
 *
 * Description: bitl D1555 180 rotation pixel phase 2 function, do D1555 rotation degree 180 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D1555_180_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - y - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - x -1//zero base) * sizeof(UINT16));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) -
        y * ui4_pitch -
        x * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB1555(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_270_pixel_sample_phase1
 *
 * Description: bitl D1555 270 rotation pixel phase 1 function, get the common part of blit_D1555_270_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D1555_270_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return   (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - 1) * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D1555_270_pixel_sample_phase2
 *
 * Description: bitl D1555 270 rotation pixel phase 2 function, do D1555 rotation degree 270 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D1555_270_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - x - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        ((pt_src_dst_rect->ui4_dst_x + y) * sizeof(UINT16)));
    */

    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) -
        x * ui4_pitch +
        y * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB1555(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_0_pixel_sample_phase1
 *
 * Description: bitl D4444 0 rotation pixel phase 1 function, get the common part of blit_D4444_0_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D4444_0_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
         );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_0_pixel_sample_phase2
 *
 * Description: bitl D4444 0 rotation pixel phase 2 function, do D4444 rotation degree 0 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D4444_0_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y + y) * pt_lock_info->at_subplane[0].ui4_pitch +
         ((pt_src_dst_rect->ui4_dst_x + x) * sizeof(UINT16)));
    */

    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) +
        y * ui4_pitch +
        x * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB4444(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_90_pixel_sample_phase1
 *
 * Description: bitl D4444 90 rotation pixel phase 1 function, get the common part of blit_D4444_90_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D4444_90_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - 1) * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_90_pixel_sample_phase2
 *
 * Description: bitl D4444 90 rotation pixel phase 2 function, do D4444 rotation degree 90 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D4444_90_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        ((pt_src_dst_rect->ui4_dst_y + x) * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - y - 1//zero base) * sizeof(UINT16));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) +
        x * ui4_pitch -
        y * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB4444(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_180_pixel_sample_phase1
 *
 * Description: bitl D4444 180 rotation pixel phase 1 function, get the common part of blit_D4444_180_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D4444_180_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - 1/*zero base*/) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - 1/*zero base*/) * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_180_pixel_sample_phase2
 *
 * Description: bitl D4444 180 rotation pixel phase 2 function, do D4444 rotation degree 180 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D4444_180_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - y - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - x - 1//zero base) * sizeof(UINT16));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) -
        y * ui4_pitch -
        x * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB4444(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_270_pixel_sample_phase1
 *
 * Description: bitl D4444 270 rotation pixel phase 1 function, get the common part of blit_D4444_270_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D4444_270_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - 1/*zero base*/) * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT16)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D4444_270_pixel_sample_phase2
 *
 * Description: bitl D4444 270 rotation pixel phase 2 function, do D4444 rotation degree 270 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D4444_270_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT16* pui2_Img;

    /*
    pui2_Img = (UINT16*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - x - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        ((y + pt_src_dst_rect->ui4_dst_x) * sizeof(UINT16)));
    */
    pui2_Img = (UINT16*) (
        (UINT8*)(pui1_phase1_data) -
        x * ui4_pitch +
        y * sizeof(UINT16)
        );

    *pui2_Img = (UINT16) COLOR_ARGB4444(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_0_pixel_sample_phase1
 *
 * Description: bitl D8888 0 rotation pixel phase 1 function, get the common part of blit_D8888_0_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D8888_0_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT32)
         );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_0_pixel_sample_phase2
 *
 * Description: bitl D8888 0 rotation pixel phase 2 function, do D8888 rotation degree 0 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D8888_0_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT32* pui4_Img;

    /*
    pui4_Img = (UINT32*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (pt_src_dst_rect->ui4_dst_y + y) * pt_lock_info->at_subplane[0].ui4_pitch +
         ((pt_src_dst_rect->ui4_dst_x + x) * sizeof(UINT32)));
    */
    pui4_Img = (UINT32*) (
        (UINT8*)(pui1_phase1_data) +
        y * ui4_pitch +
        x * sizeof(UINT32)
        );

    *pui4_Img =  COLOR_ARGB8888(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_90_pixel_sample_phase1
 *
 * Description: bitl D8888 90 rotation pixel phase 1 function, get the common part of blit_D8888_90_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D8888_90_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        pt_src_dst_rect->ui4_dst_y * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - 1/*zero base*/) * sizeof(UINT32)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_90_pixel_sample_phase2
 *
 * Description: bitl D8888 90 rotation pixel phase 2 function, do D8888 rotation degree 90 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D8888_90_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT32* pui4_Img;

    /*
    pui4_Img = (UINT32*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        ((x + pt_src_dst_rect->ui4_dst_y) * pt_lock_info->at_subplane[0].ui4_pitch) +
        (display_image_height + pt_src_dst_rect->ui4_dst_x - y - 1//zero base) * sizeof(UINT32));
    */
    pui4_Img = (UINT32*) (
        (UINT8*)(pui1_phase1_data) +
        x * ui4_pitch -
        y * sizeof(UINT32)
        );

    *pui4_Img =  COLOR_ARGB8888(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_180_pixel_sample_phase1
 *
 * Description: bitl D8888 180 rotation pixel phase 1 function, get the common part of blit_D8888_180_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D8888_180_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - 1/*zero base*/) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - 1/*zero base*/) * sizeof(UINT32)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_180_pixel_sample_phase2
 *
 * Description: bitl D8888 180 rotation pixel phase 2 function, do D8888 rotation degree 180 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D8888_180_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT32* pui4_Img;

    /*
    pui4_Img = (UINT32*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_height + pt_src_dst_rect->ui4_dst_y - y - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        (display_image_width + pt_src_dst_rect->ui4_dst_x - x - 1//zero base) * sizeof(UINT32));
    */
    pui4_Img = (UINT32*) (
        (UINT8*)(pui1_phase1_data) -
        y * ui4_pitch -
        x * sizeof(UINT32)
        );

    *pui4_Img =  COLOR_ARGB8888(0XFF, R, G, B);
}
/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_270_pixel_sample_phase1
 *
 * Description: bitl D8888 270 rotation pixel phase 1 function, get the common part of blit_D8888_270_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT8* blit_D8888_270_pixel_sample_phase1(SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, GL_LOCK_INFO_T* pt_lock_info)
{
    return (
        (UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - 1/*zero base*/) * pt_lock_info->at_subplane[0].ui4_pitch +
        pt_src_dst_rect->ui4_dst_x * sizeof(UINT32)
        );
}

/*-----------------------------------------------------------------------------
 * Name:  blit_D8888_270_pixel_sample_phase2
 *
 * Description: bitl D8888 270 rotation pixel phase 2 function, do D8888 rotation degree 270 x, y , R, G, B specific.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void blit_D8888_270_pixel_sample_phase2(UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data)
{
    UINT32* pui4_Img;

    /*
    pui4_Img = (UINT32*) ((UINT8*)(pt_lock_info->at_subplane[0].pv_bits) +
        (display_image_width + pt_src_dst_rect->ui4_dst_y - x - 1//zero base) * pt_lock_info->at_subplane[0].ui4_pitch +
        ((pt_src_dst_rect->ui4_dst_x + y) * sizeof(UINT32)));
    */
    pui4_Img = (UINT32*) (
        (UINT8*)(pui1_phase1_data) -
        x * ui4_pitch +
        y * sizeof(UINT32)
        );

    *pui4_Img =  COLOR_ARGB8888(0XFF, R, G, B);
}
#endif
/*-----------------------------------------------------------------------------
 * Name:  vSet_resampling_fun_pt
 *
 * Description: set resampling function pointer
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
#if 0
void vSet_resampling_fun_pt(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
#if 0
    IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return ;
    }

    if (pt_info->ui2_num_components == 3)
    {
        if ((pt_jpg_obj->ui4_h_pixel_ratio >= 2048) /* scale down and ratio > = 2*/
            && (pt_jpg_obj->ui4_v_pixel_ratio >= 2048))
        {
            pt_jpg_obj->pf_resampling_fct = (resampling_fct) nearest_YCBCR_resampling;
        }
        else
        {
        #ifdef HW_SCALER
            pt_jpg_obj->pf_resampling_fct = (resampling_fct) nearest_YCBCR_resampling;
        #else
            pt_jpg_obj->pf_resampling_fct =  (resampling_fct) bilinear_YCBCR_resampling;
        #endif
        }
    }
    else if (pt_info->ui2_num_components == 1)
    {
         if ((pt_jpg_obj->ui4_h_pixel_ratio >= 2048) /* scale down and ratio > = 2*/
            && (pt_jpg_obj->ui4_v_pixel_ratio >= 2048))
        {
            pt_jpg_obj->pf_resampling_fct = (resampling_fct) nearest_Gray_resampling;
        }
         else
        {
        #ifdef HW_SCALER
            pt_jpg_obj->pf_resampling_fct = (resampling_fct) nearest_Gray_resampling;
        #else
            pt_jpg_obj->pf_resampling_fct = (resampling_fct) bilinear_Gray_resampling;
        #endif
        }
    }
#endif
}
/*-----------------------------------------------------------------------------
 * Name:  vSet_blit_fun_pt
 *
 * Description: set which is blit pixel function blit_D565_pixel_sample/blit_D1555_pixel_sample/blit_D4444_pixel_sample/blit_D8888_pixel_sample
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vSet_blit_fun_pt(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, IMG_ROTATE_OP_TYPE_T e_ratate_type)
{
#if 0
    switch (pt_lock_info->e_clrmode)
    {
        case GL_COLORMODE_RGB_D565:
            switch (e_ratate_type)
            {
                case IMG_ROTATE_CLOCKWS_0_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D565_0_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D565_0_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_90_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D565_90_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D565_90_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_180_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D565_180_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D565_180_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_270_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D565_270_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D565_270_pixel_sample_phase2;
                    break;
            }
            break;
        case GL_COLORMODE_ARGB_D1555:
            switch (e_ratate_type)
            {
                case IMG_ROTATE_CLOCKWS_0_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D1555_0_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D1555_0_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_90_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D1555_90_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D1555_90_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_180_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D1555_180_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D1555_180_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_270_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D1555_270_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D1555_270_pixel_sample_phase2;
                    break;
            }
            break;
        case GL_COLORMODE_ARGB_D4444:
            switch (e_ratate_type)
            {
                case IMG_ROTATE_CLOCKWS_0_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D4444_0_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D4444_0_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_90_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D4444_90_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D4444_90_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_180_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D4444_180_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D4444_180_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_270_OP_TYPE:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D4444_270_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D4444_270_pixel_sample_phase2;
                    break;
            }

            break;
        case GL_COLORMODE_ARGB_D8888:
        case GL_COLORMODE_AYUV_D8888:
            switch (e_ratate_type)
            {
                case IMG_ROTATE_CLOCKWS_0_OP_TYPE:
                case IMG_ROTATE_CLOCKWS_0_OP_TYPE_WITH_FLIP:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D8888_0_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D8888_0_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_90_OP_TYPE:
                case IMG_ROTATE_CLOCKWS_90_OP_TYPE_WITH_FLIP:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D8888_90_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D8888_90_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_180_OP_TYPE:
                case IMG_ROTATE_CLOCKWS_180_OP_TYPE_WITH_FLIP:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D8888_180_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D8888_180_pixel_sample_phase2;
                    break;
                case IMG_ROTATE_CLOCKWS_270_OP_TYPE:
                case IMG_ROTATE_CLOCKWS_270_OP_TYPE_WITH_FLIP:
                    pt_jpg_obj->pui1_blit_fct_phase1_data = blit_D8888_270_pixel_sample_phase1(pt_src_dst_rect, display_image_width, display_image_height, pt_lock_info);
                    pt_jpg_obj->pf_blit_phase2_fct = blit_D8888_270_pixel_sample_phase2;
                    break;
            }
            break;
        default:
            return;
    }
#endif
}
#endif
#if 0
/*-----------------------------------------------------------------------------
 * Name:  nearest_Gray_resampling
 *
 * Description: resample the image to desired size by nearest point
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void nearest_Gray_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal)
{
#ifdef HW_SCALER
    register IMG_BASIC_INFO_T* pt_info = NULL;
    blit_pixel_fct pf_blit_phase2_fct;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
    UINT32 src_y;
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_v_pixel_ratio;
    UINT32 ui4_Y_color_buf_width;
    register UINT32 y, x;
    UINT32 scale_up_src_y_pos;

    UINT32 src_y_pos_quotient;
    UINT32 imcu_row_y_index;
    register INT16 i2_R, i2_G, i2_B;
    INT16 i2_Y;

    UINT32      ui4_dst_y_width, ui4_dst_y_height;
    UINT32      ui4_src_y_width, ui4_src_y_height;
    UINT8       *pui1_y_start;

    UINT8		ui1_scale_factor, ui1_y_index_offset;

    UINT8       *pui1_src_buffer;
    UINT32      ui4_src_pitch;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
    {
        return;
    }

    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;


    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_y = pt_src_dst_rect->ui4_src_y;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];

    ui4_dst_y_width  = ui4_display_image_width;
    ui4_dst_y_height = (org_comp_Y_image_height_sub_1 + 1) / v_samp_factor_X_DCTSIZE;
    ui4_dst_y_height = (ui4_display_image_height + (ui4_dst_y_height - 1)) / ui4_dst_y_height;

    ui4_src_y_width  = org_comp_Y_image_width_sub_1 + 1;
    ui4_src_y_height = v_samp_factor_X_DCTSIZE;

    if (ui4_dst_y_height == 1)
    {
        ui1_scale_factor = 2;
        ui1_y_index_offset = 0;
    }
    else
    {
        ui1_scale_factor = 1;
        ui1_y_index_offset = 1;
    }

    /* first time to decode this image */
    if (pt_jpg_obj->ui2_output_image_lines == 0)
    {
        /* allocate tmp buffer */
        if (_h_dst_y_surf != NULL_HANDLE)
        {
            x_gl_surface_unlock(_h_dst_y_surf, NULL);

            if (ui4_dst_y_height == 1)
            {
                x_gl_surface_unlock(_h_y_surf, NULL);
            }

            x_gl_obj_free(_h_dst_y_surf);

            if (ui4_dst_y_height == 1)
            {
                x_gl_obj_free(_h_y_surf);
            }

            _h_dst_y_surf = NULL_HANDLE;

            if (ui4_dst_y_height == 1)
            {
                _h_y_surf = NULL_HANDLE;
            }
        }

        if (_h_dst_y_surf == NULL_HANDLE)
        {
            _pui1_dst_y_buf  = _create_surface_buffer(&_h_dst_y_surf, ui4_dst_y_width, ui4_dst_y_height * ui1_scale_factor, &_ui4_dst_y_pitch);

            if (ui4_dst_y_height == 1)
            {
                _pui1_y_buf  = _create_surface_buffer(&_h_y_surf, ui4_src_y_width, ui4_src_y_height * ui1_scale_factor, &_ui4_y_pitch);
            }
        }
    }

    if (ui4_dst_y_height == 1)
    {
        /* prepare input buffers */
        /* Y */
        _prepare_input_buffer(
            _pui1_y_buf,
            pui1_y_start_addr,
            pui1_next_y_start_addr,
            ui4_src_y_width,
            ui4_src_y_height,
            _ui4_y_pitch,
            ui4_Y_color_buf_width);
    }

    /* scale */
    /* Y */
    if (ui4_dst_y_height == 1)
    {
        pui1_src_buffer = _pui1_y_buf;
        ui4_src_pitch = _ui4_y_pitch;
    }
    else
    {
        pui1_src_buffer = pui1_y_start_addr;
        ui4_src_pitch = ui4_Y_color_buf_width;
    }
    _scale_buffer(
        _pui1_dst_y_buf,
        pui1_src_buffer,
        ui4_dst_y_width,
        ui4_dst_y_height * ui1_scale_factor,
        0,
        0,
        ui4_src_y_width,
        ui4_src_y_height * ui1_scale_factor,
        _ui4_dst_y_pitch,
        ui4_src_pitch,
        _e_gfx_op);

    _flush_buffer(&h_surf_signal);

    /* color convert and write to output buffer */
    pui1_y_start  = _pui1_dst_y_buf;

    for (y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;
        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
        {
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;
        }

        src_y_pos_quotient = src_y_pos_quotient + src_y;

        /* get y position for imcu row*/
        imcu_row_y_index = src_y_pos_quotient - resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index + (UINT32)ui1_y_index_offset >= v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
        {
            /* we have done all the resampling lines of one component in this iMCU */
            goto FIN;
        }

        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            i2_Y  = *(pui1_y_start  + x);

            /* Range-limiting is essential due to noise introduced by DCT losses */
            i2_R = i2_Y;
            i2_G = i2_Y;
            i2_B = i2_Y;

            if (!((i2_R|i2_G|i2_B) & 0xff00))
            {
                /* blit RGB of pixel value */
                pf_blit_phase2_fct(
                    (pt_src_dst_rect->b_flip != TRUE)? x: ui4_display_image_width - 1 - x,
                    y,
                    (UINT8)i2_R,
                    (UINT8)i2_G,
                    (UINT8)i2_B,
                    ui4_pitch,
                    pui1_blit_fct_phase1_data);
            }
            else
            {
                RANGE_LIMIT(i2_R, i2_R);
                RANGE_LIMIT(i2_G, i2_G);
                RANGE_LIMIT(i2_B, i2_B);
                /* blit RGB of pixel value */
                pf_blit_phase2_fct(
                    (pt_src_dst_rect->b_flip != TRUE)? x: ui4_display_image_width - 1 - x,
                    y,
                    (UINT8)i2_R,
                    (UINT8)i2_G,
                    (UINT8)i2_B,
                    ui4_pitch,
                    pui1_blit_fct_phase1_data);
            }
        }

        pt_jpg_obj->ui2_output_image_lines++;

        pui1_y_start  += _ui4_dst_y_pitch;
    }
#else
    register IMG_BASIC_INFO_T* pt_info = NULL;
    blit_pixel_fct pf_blit_phase2_fct;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_h_pixel_ratio, ui4_v_pixel_ratio;

    UINT32 src_x;
    UINT32 src_y;
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;

    UINT32 ui4_Y_color_buf_width;
    KEEP_RESAMPLE_X_PARAMETERS* pt_keep_x_param = NULL;
    register UINT32 y, x;
    UINT32 next_x_index;
    UINT32 scale_up_src_x_pos;
    UINT32 scale_up_src_x_pos_remainder;
    UINT32 src_x_pos_quotient;
    UINT32 scale_up_src_y_pos;
    UINT32 src_y_pos_quotient;
    UINT32 scale_up_src_y_pos_remainder;
    UINT32 imcu_row_y_index;
    UINT8 *color_buf_y_start_addr[MAX_COMPS_IN_SCAN];
    register INT16 i2_Y;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
    {
        return;
    }

    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;

    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;

    ui4_h_pixel_ratio = pt_jpg_obj->ui4_h_pixel_ratio;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_x = pt_src_dst_rect->ui4_src_x;
    src_y = pt_src_dst_rect->ui4_src_y;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];


    /* caculate x parameters first and store it for future used*/
    if (pt_jpg_obj->pt_keep_x_params == NULL)
    {
        /* allocate space to store x parameters */
        pt_keep_x_param = x_mem_alloc(sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        if (pt_keep_x_param == NULL)
        {
        	pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        	return;
        }
        x_memset(pt_keep_x_param, 0, sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        pt_jpg_obj->pt_keep_x_params = pt_keep_x_param;
        /* caculate x parameters first*/
        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            scale_up_src_x_pos = (x * ui4_h_pixel_ratio);
            src_x_pos_quotient = scale_up_src_x_pos >> UP_SCALE_Q;
            scale_up_src_x_pos_remainder = scale_up_src_x_pos & UP_SCALE_R;

            /* correct src_x_pos_quotient out of right boundary */
            if (src_x_pos_quotient > org_comp_Y_image_width_sub_1)
            {
                src_x_pos_quotient = org_comp_Y_image_width_sub_1;
            }

            if (scale_up_src_x_pos_remainder & 0x200)/*scale_up_src_x_pos_remainder>=512*/
            {
                /* correct next_x_index out of right boundary */
                if (src_x_pos_quotient >= org_comp_Y_image_width_sub_1)
                {
                    next_x_index = src_x_pos_quotient ;
                }
                else
                {
                    next_x_index = src_x_pos_quotient + 1;
                }
                next_x_index = next_x_index + src_x;
                pt_keep_x_param[x].src_x_pos_quotient_Y = next_x_index;
            }
            else
            {
                src_x_pos_quotient = src_x_pos_quotient + src_x;
                pt_keep_x_param[x].src_x_pos_quotient_Y = src_x_pos_quotient;
            }

        }
    }
    else
    {
        pt_keep_x_param = pt_jpg_obj->pt_keep_x_params;
    }

    for (y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;
        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
        {
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;
        }

        src_y_pos_quotient = src_y_pos_quotient + src_y;
        /* get y position for imcu row*/

        imcu_row_y_index = src_y_pos_quotient - resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index >= v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
        {
            /* we have done all the resampling lines of one component in this iMCU */
            goto FIN;
        }

        /* get remainder part of desination y pos */
        scale_up_src_y_pos_remainder = scale_up_src_y_pos & UP_SCALE_R;

         if ((scale_up_src_y_pos_remainder & 0x200) /*scale_up_src_y_pos_remainder>=512*/
            && (src_y_pos_quotient < org_comp_Y_image_height_sub_1))
        {
            if ( (imcu_row_y_index + 1) >= v_samp_factor_X_DCTSIZE )
            {
                /* use another color buffer's first line data */
                color_buf_y_start_addr[Y_INDEX] = pui1_next_y_start_addr;
            }
            else
            {
                color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + (imcu_row_y_index+1) * ui4_Y_color_buf_width;
            }
        }
        else
        {
            /* calculate color buffer (y) start address */
            color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + imcu_row_y_index * ui4_Y_color_buf_width;
        }

        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            /* get the nearest neighboring pixels in original image */
            /* pix1 pix2                                        */
            /*    O                                           */
            /* pix3 pix4                                        */
            i2_Y = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);

            /*blit RGB of pixel value*/
            if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
            {
                pf_blit_phase2_fct(x, y, i2_Y, i2_Y, i2_Y, ui4_pitch, pui1_blit_fct_phase1_data);
            }
            else if (pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
            {
                pf_blit_phase2_fct(x, y, i2_Y, 127, 127, ui4_pitch, pui1_blit_fct_phase1_data);
            }
        }
        /************************************************************************/
        /*            end resample a whole row                                  */
        /************************************************************************/
        pt_jpg_obj->ui2_output_image_lines++;
    }
#endif /* HW_SCALER */

FIN:
    /* increases pt_jpg_obj->ui2_num_of_resampling by 1 */
    pt_jpg_obj->ui2_num_of_resampling = pt_jpg_obj->ui2_num_of_resampling + 1;

} /* bilinear_YCBCR_resampling() function  */

/*-----------------------------------------------------------------------------
 * Name:  nearest_YCBCR_resampling
 *
 * Description: resample the image to desired size by nearest point
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void nearest_YCBCR_resampling(
    IMG_JPG_OBJ_T           *pt_jpg_obj,
    SRC_DST_RECT_T          *pt_src_dst_rect,
    IMG_ROTATE_OP_TYPE_T    e_ratate_type,
    GL_LOCK_INFO_T          *pt_lock_info,
    UINT8                   *pui1_y_start_addr,
    UINT8                   *pui1_cb_start_addr,
    UINT8                   *pui1_cr_start_addr,
    UINT8                   *pui1_next_y_start_addr,
    UINT8                   *pui1_next_cb_start_addr,
    UINT8                   *pui1_next_cr_start_addr,
    HANDLE_T                h_surf_signal)
{
#ifdef HW_SCALER
    register IMG_BASIC_INFO_T* pt_info = NULL;
    blit_pixel_fct pf_blit_phase2_fct;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
#if !defined(HW_YCBCR) && defined(MW_IMG_JPG_ARGB_OUTPUT)
    INT16* pi2_Cr_r_tab;
    INT16* pi2_Cb_b_tab;
    INT32* pi4_Cr_g_tab;
    INT32* pi4_Cb_g_tab;
#endif /* !defined(HW_YCBCR) && defined(MW_IMG_JPG_ARGB_OUTPUT) */
    UINT32 src_y;
    UINT32 ui4_max_v_samp_factor;
    UINT32 ui4_max_h_samp_factor;
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_v_pixel_ratio;
    UINT32 ui4_Y_color_buf_width, ui4_Cb_color_buf_width, ui4_Cr_color_buf_width;
    register UINT32 y;
#ifndef HW_YCBCR
    register UINT32 x;
#endif /* HW_YCBCR */
    UINT32 scale_up_src_y_pos;

    UINT32 src_y_pos_quotient;
    INT32  imcu_row_y_index;
#ifndef HW_YCBCR
    register INT16 i2_R, i2_G, i2_B;
    INT16 i2_Y, i2_CR, i2_CB;
#endif /* HW_YCBCR */

    UINT32      ui4_dst_y_width, ui4_dst_y_height, ui4_up_height, ui4_down_height;
    UINT32      ui4_dst_cb_width, ui4_dst_cb_height;
    UINT32      ui4_dst_cr_width, ui4_dst_cr_height;
    UINT32      ui4_src_y_width, ui4_src_y_height;
    UINT32      ui4_src_cb_width, ui4_src_cb_height;
    UINT32      ui4_src_cr_width, ui4_src_cr_height;
#ifndef HW_YCBCR
    UINT8       *pui1_y_start, *pui1_cb_start, *pui1_cr_start;
    UINT8       *pui1_y_index, *pui1_cb_index, *pui1_cr_index;
#endif /* HW_YCBCR */

    /*UINT8		ui1_scale_factor, ui1_y_index_offset;*/

    UINT8       *pui1_src_buffer;
    UINT32      ui4_src_pitch;

#ifndef HW_YCBCR
    UINT32      *pui4_dst_start, *pui4_dst_index;
#endif /* HW_YCBCR */

#ifdef HW_YCBCR
    UINT32      ui4_cbcr_width, ui4_cbcr_height;
#endif /* HW_YCBCR */

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
    {
        return;
    }

#if !defined(HW_YCBCR) && defined(MW_IMG_JPG_ARGB_OUTPUT)
    pi2_Cr_r_tab =  pt_jpg_obj->ai2_Cr_r_tab;
    pi2_Cb_b_tab =  pt_jpg_obj->ai2_Cb_b_tab;

    pi4_Cr_g_tab =  pt_jpg_obj->ai4_Cr_g_tab;
    pi4_Cb_g_tab =  pt_jpg_obj->ai4_Cb_g_tab;
#endif /* !defined(HW_YCBCR) && defined(MW_IMG_JPG_ARGB_OUTPUT) */

    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;


    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_y = pt_src_dst_rect->ui4_src_y;

    ui4_max_v_samp_factor = pt_info->ui1_max_v_samp_factor;
    ui4_max_h_samp_factor = pt_info->ui1_max_h_samp_factor;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];
    ui4_Cb_color_buf_width = pt_info->ui4_pic_width_in_buf[CB_INDEX];
    ui4_Cr_color_buf_width = pt_info->ui4_pic_width_in_buf[CR_INDEX];

    ui4_dst_y_width  = ui4_display_image_width;

    if (pt_jpg_obj->ui2_output_image_lines == 0)
    {
        /* get the output height of the 1st MCU row */
        for (ui4_up_height= 0, y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++, ui4_up_height++)
        {
            /* get double type destination y pos */
            scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

            /* convert to integer part of destination y pos */
            src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;

            /* prevent out side of display height*/
            if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
            {
                src_y_pos_quotient = org_comp_Y_image_height_sub_1;
            }

            src_y_pos_quotient = src_y_pos_quotient + src_y;

            /* get y position for imcu row*/
            imcu_row_y_index = (INT32)src_y_pos_quotient - (INT32)resamp_num_X_v_samp_X_DCTSIZE;

            if (imcu_row_y_index + 1 >= (INT32)v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
            {
                break;
            }
        }
    }
    else
    {
        ui4_up_height = pt_jpg_obj->ui4_prev_output_lines;
    }

    /* get the output height of the 2nd MCU row */
    for (ui4_down_height = 0, y = pt_jpg_obj->ui2_output_image_lines + ui4_up_height; y < ui4_display_image_height; y++, ui4_down_height++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;

        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
        {
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;
        }

        src_y_pos_quotient = src_y_pos_quotient + src_y;

        /* get y position for imcu row*/
        imcu_row_y_index = (INT32)src_y_pos_quotient - (INT32)resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index + 1 >= (INT32)v_samp_factor_X_DCTSIZE * 2)
        {
            break;
        }
    }

    if (pt_jpg_obj->ui4_total_mcu_rows == pt_jpg_obj->ui2_num_of_resampling + 1)
    {
        ui4_down_height = ui4_up_height;
    }

    ui4_dst_y_height = ui4_up_height + ui4_down_height;

    pt_jpg_obj->ui4_prev_output_lines = ui4_down_height;

    if (ui4_dst_y_height == 0)
    {
        goto FIN;
    }

    ui4_dst_cb_width  = ui4_dst_y_width;
    ui4_dst_cb_height = ui4_dst_y_height;

    ui4_dst_cr_width  = ui4_dst_y_width;
    ui4_dst_cr_height = ui4_dst_y_height;

    ui4_src_y_width  = org_comp_Y_image_width_sub_1 + 1;
    ui4_src_y_height = v_samp_factor_X_DCTSIZE * 2;

    ui4_src_cb_width  = ui4_src_y_width / ui4_max_h_samp_factor;
    ui4_src_cb_height = (ui4_max_v_samp_factor == 1)? v_samp_factor_X_DCTSIZE * 2: v_samp_factor_X_DCTSIZE;

    ui4_src_cr_width  = ui4_src_y_width / ui4_max_h_samp_factor;
    ui4_src_cr_height = (ui4_max_v_samp_factor == 1)? v_samp_factor_X_DCTSIZE * 2: v_samp_factor_X_DCTSIZE;

#ifdef HW_YCBCR
    if (ui4_max_h_samp_factor == 1 && ui4_max_v_samp_factor == 1)
    {
        ui4_cbcr_width  = ui4_dst_y_width;
        ui4_cbcr_height = (ui4_dst_y_height + 1) / 2;
    }
    /* 4: 2: 2 (V) */
    else if (ui4_max_h_samp_factor == 2 && ui4_max_v_samp_factor == 1)
    {
        ui4_cbcr_width  = ui4_dst_y_width;
        ui4_cbcr_height = (ui4_dst_y_height + 1) / 2;
    }
    /* 4: 2: 2 (H) */
    else if (ui4_max_h_samp_factor == 1 && ui4_max_v_samp_factor == 2)
    {
        ui4_cbcr_width  = ui4_dst_y_width;
        ui4_cbcr_height = (ui4_dst_y_height + 1) / 2;
    }
    /* 4: 1: 1 */
    else
    {
        ui4_cbcr_width  = ui4_dst_y_width;
        ui4_cbcr_height = (ui4_dst_y_height + 1) / 2;
    }
#endif /* HW_YCBCR */

    /* first time to decode this image */
    if (pt_jpg_obj->ui2_output_image_lines == 0)
    {
        /* allocate tmp buffer */
        if (_h_dst_y_surf != NULL_HANDLE)
        {
            _release_surface(&_h_dst_y_surf);
            _release_surface(&_h_dst_cb_surf);
            _release_surface(&_h_dst_cr_surf);

            _release_surface(&_h_y_surf);
            _release_surface(&_h_cb_surf);
            _release_surface(&_h_cr_surf);

        #ifdef HW_YCBCR
            _release_surface(&_h_cbcr_surf);
        #endif
        }

        if (_h_dst_y_surf == NULL_HANDLE)
        {
            _pui1_dst_y_buf  = _create_surface_buffer(&_h_dst_y_surf, ui4_dst_y_width, ui4_dst_y_height * 3 / 2, &_ui4_dst_y_pitch);
            _pui1_dst_cb_buf = _create_surface_buffer(&_h_dst_cb_surf, ui4_dst_cb_width, ui4_dst_cb_height * 3 / 2, &_ui4_dst_cb_pitch);
            _pui1_dst_cr_buf = _create_surface_buffer(&_h_dst_cr_surf, ui4_dst_cr_width, ui4_dst_cr_height * 3 / 2, &_ui4_dst_cr_pitch);
        #ifdef HW_YCBCR
            _pui1_cbcr_buf   = _create_surface_buffer(&_h_cbcr_surf, ui4_cbcr_width, ui4_cbcr_height * 3 / 2, &_ui4_cbcr_pitch);
        #endif

            _pui1_y_buf      = _create_surface_buffer(&_h_y_surf, ui4_src_y_width, ui4_src_y_height, &_ui4_y_pitch);
            _pui1_cb_buf     = _create_surface_buffer(&_h_cb_surf, ui4_src_cb_width, ui4_src_cb_height, &_ui4_cb_pitch);
            _pui1_cr_buf     = _create_surface_buffer(&_h_cr_surf, ui4_src_cr_width, ui4_src_cr_height, &_ui4_cr_pitch);
        }
    }

    /* prepare input buffers */
    /* Y */
    _prepare_input_buffer(
        _pui1_y_buf,
        pui1_y_start_addr,
        pui1_next_y_start_addr,
        ui4_src_y_width,
        ui4_src_y_height / 2,
        _ui4_y_pitch,
        ui4_Y_color_buf_width);
    /* Cb */
    _prepare_input_buffer(
        _pui1_cb_buf,
        pui1_cb_start_addr,
        pui1_next_cb_start_addr,
        ui4_src_cb_width,
        ui4_src_cb_height / 2,
        _ui4_cb_pitch,
        ui4_Cb_color_buf_width);
    /* Cr */
    _prepare_input_buffer(
        _pui1_cr_buf,
        pui1_cr_start_addr,
        pui1_next_cr_start_addr,
        ui4_src_cr_width,
        ui4_src_cr_height / 2,
        _ui4_cr_pitch,
        ui4_Cr_color_buf_width);

#ifdef HW_YCBCR
    /* 4: 4: 4 */
    if (ui4_max_h_samp_factor == 1 && ui4_max_v_samp_factor == 1)
    {
        ui4_dst_cb_width  = (ui4_dst_cb_width  + 1) / 2;
        ui4_dst_cb_height = (ui4_dst_cb_height + 1) / 2;

        ui4_dst_cr_width  = (ui4_dst_cr_width  + 1) / 2;
        ui4_dst_cr_height = (ui4_dst_cr_height + 1) / 2;
    }
    /* 4: 2: 2 (V) */
    else if (ui4_max_h_samp_factor == 2 && ui4_max_v_samp_factor == 1)
    {
        ui4_dst_cb_width  = (ui4_dst_cb_width  + 1) / 2;
        ui4_dst_cb_height = (ui4_dst_cb_height + 1) / 2;

        ui4_dst_cr_width  = (ui4_dst_cr_width  + 1) / 2;
        ui4_dst_cr_height = (ui4_dst_cr_height + 1) / 2;
    }
    /* 4: 2: 2 (H) */
    else if (ui4_max_h_samp_factor == 1 && ui4_max_v_samp_factor == 2)
    {
        ui4_dst_cb_width  = (ui4_dst_cb_width  + 1) / 2;
        ui4_dst_cb_height = (ui4_dst_cb_height + 1) / 2;

        ui4_dst_cr_width  = (ui4_dst_cr_width  + 1) / 2;
        ui4_dst_cr_height = (ui4_dst_cr_height + 1) / 2;
    }
    /* 4: 1: 1 */
    else
    {
        ui4_dst_cb_width  = (ui4_dst_cb_width  + 1) / 2;
        ui4_dst_cb_height = (ui4_dst_cb_height + 1) / 2;

        ui4_dst_cr_width  = (ui4_dst_cr_width  + 1) / 2;
        ui4_dst_cr_height = (ui4_dst_cr_height + 1) / 2;
    }
#endif

    /* scale */
    /* Y */
    pui1_src_buffer = _pui1_y_buf;
    ui4_src_pitch = _ui4_y_pitch;

    _scale_buffer(
        _pui1_dst_y_buf,
        pui1_src_buffer,
        ui4_dst_y_width,
        ui4_dst_y_height,
        0,
        0,
        ui4_src_y_width,
        ui4_src_y_height,
        _ui4_dst_y_pitch,
        ui4_src_pitch,
        _e_gfx_op);

    /* Cb */
    pui1_src_buffer = _pui1_cb_buf;
    ui4_src_pitch = _ui4_cb_pitch;

    _scale_buffer(
        _pui1_dst_cb_buf,
        pui1_src_buffer,
        ui4_dst_cb_width,
        ui4_dst_cb_height,
        0,
        0,
        ui4_src_cb_width,
        ui4_src_cb_height,
        _ui4_dst_cb_pitch,
        ui4_src_pitch,
        _e_gfx_op);

    pui1_src_buffer = _pui1_cr_buf;
    ui4_src_pitch = _ui4_cr_pitch;

    /* Cr */
    _scale_buffer(
        _pui1_dst_cr_buf,
        pui1_src_buffer,
        ui4_dst_cr_width,
        ui4_dst_cr_height,
        0,
        0,
        ui4_src_cr_width,
        ui4_src_cr_height,
        _ui4_dst_cr_pitch,
        ui4_src_pitch,
        _e_gfx_op);

#ifdef HW_YCBCR
    _flush_buffer(&h_surf_signal);

    _merge_cbcr_buf(
        _pui1_cbcr_buf,
        _ui4_cbcr_pitch,
        ui4_cbcr_width / 2,
        ui4_cbcr_height,
        _pui1_dst_cb_buf,
        _ui4_dst_cb_pitch,
        _pui1_dst_cr_buf,
        _ui4_dst_cr_pitch);

    _color_convert(
        pui1_blit_fct_phase1_data,
        ui4_pitch,
        0,
        pt_jpg_obj->ui2_output_image_lines,
        pt_lock_info->e_clrmode,
        _pui1_dst_y_buf,
        _ui4_dst_y_pitch,
        0,
        0,
        _pui1_cbcr_buf,
        _ui4_cbcr_pitch,
        ui4_dst_y_width,
        ((ui4_up_height & 0x01) == 1 && pt_jpg_obj->ui2_output_image_lines + ui4_up_height + 1 < ui4_display_image_height)?
            ui4_up_height + 1:
            ui4_up_height);

    pt_jpg_obj->ui2_output_image_lines += ui4_up_height;

    _flush_buffer(&h_surf_signal);
#else
    _flush_buffer(&h_surf_signal);

    /* color convert and write to output buffer */
    pui1_y_start  = _pui1_dst_y_buf  + 0 * _ui4_dst_y_pitch;
    pui1_cb_start = _pui1_dst_cb_buf + 0 * _ui4_dst_cb_pitch;
    pui1_cr_start = _pui1_dst_cr_buf + 0 * _ui4_dst_cr_pitch;

    pui4_dst_start = (UINT32 *)(pui1_blit_fct_phase1_data + pt_jpg_obj->ui2_output_image_lines * ui4_pitch);

    for (imcu_row_y_index = 0, y = pt_jpg_obj->ui2_output_image_lines; imcu_row_y_index < ui4_up_height; y++, imcu_row_y_index++)
    {
        pui1_y_index   = pui1_y_start   - 1;
        pui1_cb_index  = pui1_cb_start  - 1;
        pui1_cr_index  = pui1_cr_start  - 1;
        pui4_dst_index = pui4_dst_start - 1;

        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            /* Removing this loop body will save 0.55 sec for the cat image (2.57 -> 2.02)
             *
             * The time consumed by this loop body is propotional to the total displaying pixels.
             * Therefore, for any full-screen displayed image, this loop body consumes about 0.55 sec.
             *
             */
            i2_Y  = *(++pui1_y_index);
            i2_CB = *(++pui1_cb_index);
            i2_CR = *(++pui1_cr_index);

            if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
            {
                /* Range-limiting is essential due to noise introduced by DCT losses */
                i2_R = i2_Y + pi2_Cr_r_tab[i2_CR];
                i2_G = i2_Y + ((INT16) RIGHT_SHIFT (pi4_Cb_g_tab[i2_CB] + pi4_Cr_g_tab[i2_CR], SCALEBITS));
                i2_B = i2_Y + pi2_Cb_b_tab[i2_CB];

                if ((i2_R | i2_G | i2_B) & 0xff00)
                {
                    RANGE_LIMIT(i2_R, i2_R);
                    RANGE_LIMIT(i2_G, i2_G);
                    RANGE_LIMIT(i2_B, i2_B);
                }
            }
            else if (pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
            {
                i2_R = i2_Y;
                i2_G = i2_CB;
                i2_B = i2_CR;
            }

            /* blit RGB of pixel value */
        #if 0
            if (e_ratate_type == IMG_ROTATE_CLOCKWS_0_OP_TYPE)
            {
                /* ASSUME D8888 only */
                *(++pui4_dst_index) = COLOR_ARGB8888(0XFF, i2_R, i2_G, i2_B);
            }
            else
        #endif
            {
                pf_blit_phase2_fct(
                    (pt_src_dst_rect->b_flip != TRUE)? x: ui4_display_image_width - 1 - x,
                    y,
                    (UINT8)i2_R,
                    (UINT8)i2_G,
                    (UINT8)i2_B,
                    ui4_pitch,
                    pui1_blit_fct_phase1_data);
            }
        }

        pt_jpg_obj->ui2_output_image_lines++;

        pui1_y_start  += _ui4_dst_y_pitch;
        pui1_cb_start += _ui4_dst_cb_pitch;
        pui1_cr_start += _ui4_dst_cr_pitch;

        if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
            pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
        {
            pui4_dst_start += ui4_pitch / sizeof(UINT32);
        }
        else if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                 pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                 pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
        {
            pui4_dst_start += ui4_pitch / sizeof(UINT16);
        }
    }
#endif /* HW_YCBCR */

#else
    register IMG_BASIC_INFO_T* pt_info = NULL;
    blit_pixel_fct pf_blit_phase2_fct;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
#ifdef MW_IMG_JPG_ARGB_OUTPUT
    INT16* pi2_Cr_r_tab;
    INT16* pi2_Cb_b_tab;
    INT32* pi4_Cr_g_tab;
    INT32* pi4_Cb_g_tab;
#endif
    UINT32 src_x;
    UINT32 src_y;
    UINT32 ui4_max_v_samp_factor;
    UINT32 ui4_max_h_samp_factor;
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_h_pixel_ratio, ui4_v_pixel_ratio;
    UINT32 ui4_Y_color_buf_width, ui4_Cb_color_buf_width, ui4_Cr_color_buf_width;
    KEEP_RESAMPLE_X_PARAMETERS* pt_keep_x_param = NULL;
    register UINT32 y, x;
    UINT32 next_x_index;
    UINT32 scale_up_src_x_pos;
    UINT32 scale_up_src_x_pos_remainder;
    UINT32 src_x_pos_quotient;
    UINT32 scale_up_src_y_pos;

    UINT32 src_y_pos_quotient;
    UINT32 scale_up_src_y_pos_remainder;
    UINT32 imcu_row_y_index;
    UINT32 ui4_tmp;
    UINT8 *color_buf_y_start_addr[MAX_COMPS_IN_SCAN];
    register INT16 i2_R = 0, i2_G = 0, i2_B = 0;
    INT16 i2_Y, i2_CR, i2_CB;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
    {
        return;
    }

#ifdef MW_IMG_JPG_ARGB_OUTPUT
    pi2_Cr_r_tab =  pt_jpg_obj->ai2_Cr_r_tab;
    pi2_Cb_b_tab =  pt_jpg_obj->ai2_Cb_b_tab;

    pi4_Cr_g_tab =  pt_jpg_obj->ai4_Cr_g_tab;
    pi4_Cb_g_tab =  pt_jpg_obj->ai4_Cb_g_tab;
#endif

    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;


    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;
    ui4_h_pixel_ratio = pt_jpg_obj->ui4_h_pixel_ratio;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_x = pt_src_dst_rect->ui4_src_x;
    src_y = pt_src_dst_rect->ui4_src_y;

    ui4_max_v_samp_factor = pt_info->ui1_max_v_samp_factor;
    ui4_max_h_samp_factor = pt_info->ui1_max_h_samp_factor;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];
    ui4_Cb_color_buf_width = pt_info->ui4_pic_width_in_buf[CB_INDEX];
    ui4_Cr_color_buf_width = pt_info->ui4_pic_width_in_buf[CR_INDEX];


    /* caculate x parameters first and store it for future used*/
    if (pt_jpg_obj->pt_keep_x_params == NULL)
    {
        /* allocate space to store x parameters */
        pt_keep_x_param = x_mem_alloc(sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        if (pt_keep_x_param == NULL)
        {
        	pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
        	return;
        }
        x_memset(pt_keep_x_param, 0, sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        pt_jpg_obj->pt_keep_x_params = pt_keep_x_param;
        /* caculate x parameters first*/
        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            scale_up_src_x_pos = (x * ui4_h_pixel_ratio);
            src_x_pos_quotient = scale_up_src_x_pos >> UP_SCALE_Q;
            scale_up_src_x_pos_remainder = scale_up_src_x_pos & UP_SCALE_R;

            /* correct src_x_pos_quotient out of right boundary */
            if (src_x_pos_quotient > org_comp_Y_image_width_sub_1)
            {
                src_x_pos_quotient = org_comp_Y_image_width_sub_1;
            }

            if (scale_up_src_x_pos_remainder & 0x200)/*scale_up_src_x_pos_remainder>=512*/
            {
                /* correct next_x_index out of right boundary */
                if (src_x_pos_quotient >= org_comp_Y_image_width_sub_1)
                {
                    next_x_index = src_x_pos_quotient + src_x;
                }
                else
                {
                    next_x_index = src_x_pos_quotient + 1 + src_x;
                }
                pt_keep_x_param[x].src_x_pos_quotient_Y = next_x_index;
                pt_keep_x_param[x].src_x_pos_quotient_CbCr = next_x_index / ui4_max_h_samp_factor;
            }
            else
            {
                src_x_pos_quotient = src_x_pos_quotient + src_x;
                pt_keep_x_param[x].src_x_pos_quotient_Y = src_x_pos_quotient;
                pt_keep_x_param[x].src_x_pos_quotient_CbCr = src_x_pos_quotient / ui4_max_h_samp_factor;
            }

        }
    }
    else
    {
        pt_keep_x_param = pt_jpg_obj->pt_keep_x_params;
    }

    for (y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;
        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
        {
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;
        }

        src_y_pos_quotient = src_y_pos_quotient + src_y;
        /* get y position for imcu row*/

        imcu_row_y_index = src_y_pos_quotient - resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index >= v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
        {
            /* we have done all the resampling lines of one component in this iMCU */
            goto FIN;
        }

        /* get remainder part of desination y pos */
        scale_up_src_y_pos_remainder = scale_up_src_y_pos & UP_SCALE_R;

         if ((scale_up_src_y_pos_remainder & 0x200) /*scale_up_src_y_pos_remainder>=512*/
            && (src_y_pos_quotient < org_comp_Y_image_height_sub_1))
        {
            if ( (imcu_row_y_index + 1) >= v_samp_factor_X_DCTSIZE )
            {
                /* use another color buffer's first line data */
                color_buf_y_start_addr[Y_INDEX] = pui1_next_y_start_addr;

                if (pt_info->ui1_max_v_samp_factor == 1)
                {
                    color_buf_y_start_addr[CB_INDEX] = pui1_next_cb_start_addr;
                    color_buf_y_start_addr[CR_INDEX] = pui1_next_cr_start_addr;
                }
                else
                {
                    ui4_tmp = ((imcu_row_y_index)>>1);
                    color_buf_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
                    color_buf_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
                }
            }
            else
            {
                color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + (imcu_row_y_index + 1) * ui4_Y_color_buf_width;

                if (pt_info->ui1_max_v_samp_factor == 1)
                {
                    ui4_tmp = (imcu_row_y_index / ui4_max_v_samp_factor) + 1;
                    color_buf_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
                    color_buf_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
                }
                else
                {
                    ui4_tmp = ((imcu_row_y_index)>>1);
                    color_buf_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
                    color_buf_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
                }
            }
        }
        else
        {
            /* calculate color buffer (y) start address */
            color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + imcu_row_y_index * ui4_Y_color_buf_width;

            ui4_tmp = imcu_row_y_index / ui4_max_v_samp_factor;
            color_buf_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
            color_buf_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
        }

        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            /* get the nearest neighboring pixels in original image */
            /* pix1 pix2                                        */
            /*    O                                           */
            /* pix3 pix4                                        */
            i2_Y = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);
            i2_CB = *(color_buf_y_start_addr[CB_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);
            i2_CR = *(color_buf_y_start_addr[CR_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);

            /* Range-limiting is essential due to noise introduced by DCT losses. */
            if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
            {
                i2_R = i2_Y + pi2_Cr_r_tab[i2_CR];
                i2_G = i2_Y + ((INT16) RIGHT_SHIFT (pi4_Cb_g_tab[i2_CB] + pi4_Cr_g_tab[i2_CR], SCALEBITS));
                i2_B = i2_Y + pi2_Cb_b_tab[i2_CB];
            }
            else if (pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
            {
                i2_R = i2_Y;
                i2_G = i2_CB;
                i2_B = i2_CR;
            }

            if (!((i2_R|i2_G|i2_B) & 0xff00))
            {
                /*blit RGB of pixel value*/
                pf_blit_phase2_fct(x, y, (UINT8)i2_R, (UINT8)i2_G, (UINT8)i2_B, ui4_pitch, pui1_blit_fct_phase1_data);
            }
            else
            {
                RANGE_LIMIT (i2_R, i2_R);
                RANGE_LIMIT (i2_G, i2_G);
                RANGE_LIMIT (i2_B, i2_B);
                /*blit RGB of pixel value*/
                pf_blit_phase2_fct(x, y, (UINT8)i2_R, (UINT8)i2_G, (UINT8)i2_B, ui4_pitch, pui1_blit_fct_phase1_data);
            }
        }
        /************************************************************************/
        /*            end resample a whole row                                  */
        /************************************************************************/
        pt_jpg_obj->ui2_output_image_lines++;
    }
#endif /* HW_SCALER */

FIN:
    /* increases pt_jpg_obj->ui2_num_of_resampling by 1 */
    pt_jpg_obj->ui2_num_of_resampling = pt_jpg_obj->ui2_num_of_resampling + 1;


} /* bilinear_YCBCR_resampling() function  */

/*-----------------------------------------------------------------------------
 * Name:  bilinear_YCBCR_resampling
 *
 * Description: bilinear interpolation to resample the image to desired size
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void bilinear_YCBCR_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal)
{
    UINT32 scale_up_src_y_pos;
    UINT32 src_y_pos_quotient;
    UINT32 scale_up_src_y_pos_remainder;
    UINT32 scale_up_src_x_pos;
    UINT32 src_x_pos_quotient;
    UINT32 scale_up_src_x_pos_remainder;
    UINT32 imcu_row_y_index;
    UINT32 next_x_index;
    register UINT32 y, x;
    UINT32 pix1[MAX_COMPS_IN_SCAN], pix2[MAX_COMPS_IN_SCAN], pix3[MAX_COMPS_IN_SCAN], pix4[MAX_COMPS_IN_SCAN];
    UINT8 *color_buf_y_start_addr[MAX_COMPS_IN_SCAN];
    UINT8 *color_buf_next_y_start_addr[MAX_COMPS_IN_SCAN];
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;
    UINT32 src_x;
    UINT32 src_y;
    UINT32 ui4_tmp;
    register INT16 i2_R = 0, i2_G = 0, i2_B = 0;
    INT16 i2_Y, i2_CR, i2_CB;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_h_pixel_ratio, ui4_v_pixel_ratio;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 ui4_max_v_samp_factor;
    UINT32 ui4_max_h_samp_factor;
    UINT32 ui4_Y_color_buf_width, ui4_Cb_color_buf_width, ui4_Cr_color_buf_width;
    KEEP_RESAMPLE_X_PARAMETERS* pt_keep_x_param = NULL;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
    blit_pixel_fct pf_blit_phase2_fct;
#ifdef MW_IMG_JPG_ARGB_OUTPUT
    INT16* pi2_Cr_r_tab;
    INT16* pi2_Cb_b_tab;
    INT32* pi4_Cr_g_tab;
    INT32* pi4_Cb_g_tab;
#endif

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
   {
        return;
    }

#ifdef MW_IMG_JPG_ARGB_OUTPUT
    pi2_Cr_r_tab =  pt_jpg_obj->ai2_Cr_r_tab;
    pi2_Cb_b_tab =  pt_jpg_obj->ai2_Cb_b_tab;

    pi4_Cr_g_tab =  pt_jpg_obj->ai4_Cr_g_tab;
    pi4_Cb_g_tab =  pt_jpg_obj->ai4_Cb_g_tab;
#endif

    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;

    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;
    ui4_h_pixel_ratio = pt_jpg_obj->ui4_h_pixel_ratio;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_x = pt_src_dst_rect->ui4_src_x;
    src_y = pt_src_dst_rect->ui4_src_y;

    ui4_max_v_samp_factor = pt_info->ui1_max_v_samp_factor;
    ui4_max_h_samp_factor = pt_info->ui1_max_h_samp_factor;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];
    ui4_Cb_color_buf_width = pt_info->ui4_pic_width_in_buf[CB_INDEX];
    ui4_Cr_color_buf_width = pt_info->ui4_pic_width_in_buf[CR_INDEX];

	/* caculate x parameters first and store it for future used*/
	if (pt_jpg_obj->pt_keep_x_params == NULL)
	{
		/* allocate space to store x parameters */
		pt_keep_x_param = x_mem_alloc(sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
		if (pt_keep_x_param == NULL)
		{
			pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
			return;
		}
		x_memset(pt_keep_x_param, 0, sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
		pt_jpg_obj->pt_keep_x_params = pt_keep_x_param;
		/* caculate x parameters first*/
            for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
            {
                scale_up_src_x_pos = (x * ui4_h_pixel_ratio);
                src_x_pos_quotient = scale_up_src_x_pos >> UP_SCALE_Q;
                scale_up_src_x_pos_remainder = scale_up_src_x_pos & UP_SCALE_R;

                if (src_x_pos_quotient >= org_comp_Y_image_width_sub_1)
                {
                    src_x_pos_quotient = org_comp_Y_image_width_sub_1;
                    next_x_index = src_x_pos_quotient ;
                }
                else
                {
                    next_x_index = src_x_pos_quotient + 1;
                }

                src_x_pos_quotient = src_x_pos_quotient + src_x;
                next_x_index = next_x_index + src_x;

                pt_keep_x_param[x].next_x_index_Y = next_x_index;
                pt_keep_x_param[x].next_x_index_CbCr = next_x_index / ui4_max_h_samp_factor;
                pt_keep_x_param[x].scale_up_src_x_pos_remainder = scale_up_src_x_pos_remainder;
                pt_keep_x_param[x].src_x_pos_quotient_Y = src_x_pos_quotient;
                pt_keep_x_param[x].src_x_pos_quotient_CbCr = src_x_pos_quotient / ui4_max_h_samp_factor;
            }
	}
	else
	{
		pt_keep_x_param = pt_jpg_obj->pt_keep_x_params;
	}

    for (y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;
        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;

        /* get remainder part of desination y pos */
        scale_up_src_y_pos_remainder = scale_up_src_y_pos & UP_SCALE_R;

        src_y_pos_quotient = src_y_pos_quotient + src_y;
        /* get y position for imcu row*/

        imcu_row_y_index = src_y_pos_quotient - resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index >= v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
        {
            /* we have done all the resampling lines of one component in this iMCU */
            goto FIN;
        }

        /* calculate color buffer (y) start address */
        color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + imcu_row_y_index * ui4_Y_color_buf_width;

        ui4_tmp = imcu_row_y_index / ui4_max_v_samp_factor;
        color_buf_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
        color_buf_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;

        /* calculate color buffer next row (y+1) start address */
        if (src_y_pos_quotient >= org_comp_Y_image_height_sub_1)
        {
            color_buf_next_y_start_addr[Y_INDEX] = color_buf_y_start_addr[Y_INDEX];
            color_buf_next_y_start_addr[CB_INDEX] = color_buf_y_start_addr[CB_INDEX];
            color_buf_next_y_start_addr[CR_INDEX] = color_buf_y_start_addr[CR_INDEX];
        }
        else
        {
            if ( (imcu_row_y_index + 1) >= v_samp_factor_X_DCTSIZE )
            {
                /* use another color buffer's first line data */
                color_buf_next_y_start_addr[Y_INDEX] = pui1_next_y_start_addr;
                if (pt_info->ui1_max_v_samp_factor == 1)
                {
                    color_buf_next_y_start_addr[CB_INDEX] = pui1_next_cb_start_addr;
                    color_buf_next_y_start_addr[CR_INDEX] = pui1_next_cr_start_addr;
                }
                else
                {
                    ui4_tmp = ((imcu_row_y_index)>>1);
                    color_buf_next_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
                    color_buf_next_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
                }
            }
            else
            {
                color_buf_next_y_start_addr[Y_INDEX] = color_buf_y_start_addr[Y_INDEX] + ui4_Y_color_buf_width;

                if (pt_info->ui1_max_v_samp_factor == 1)
                {
                    color_buf_next_y_start_addr[CB_INDEX] = color_buf_y_start_addr[CB_INDEX] + ui4_Cb_color_buf_width;
                    color_buf_next_y_start_addr[CR_INDEX] = color_buf_y_start_addr[CR_INDEX] + ui4_Cr_color_buf_width;
                }
                else
                {
                    ui4_tmp = ((imcu_row_y_index)>>1);
                    color_buf_next_y_start_addr[CB_INDEX] = pui1_cb_start_addr + ui4_tmp * ui4_Cb_color_buf_width;
                    color_buf_next_y_start_addr[CR_INDEX] = pui1_cr_start_addr + ui4_tmp * ui4_Cr_color_buf_width;
                }
            }
        }

        ui4_tmp = UP_SCALE - scale_up_src_y_pos_remainder;/* 1-a */
        /************************************************************************/
        /*            begin resample a whole row                                */
        /************************************************************************/
        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            /* get the four most neighboring pixels in original image */
            /* pix1 pix2                                        */
            /*    O                                           */
            /* pix3 pix4                                        */
            /* We should use pt_info->ui4_pic_width_in_buf instead of pt_jpg_obj->image_width to calculate pixel actual address */
            pix1[Y_INDEX] = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);
            pix3[Y_INDEX] = *(color_buf_next_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);
            pix2[Y_INDEX] = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].next_x_index_Y);
            pix4[Y_INDEX] = *(color_buf_next_y_start_addr[Y_INDEX] + pt_keep_x_param[x].next_x_index_Y);

            pix1[CB_INDEX] = *(color_buf_y_start_addr[CB_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);
            pix3[CB_INDEX] = *(color_buf_next_y_start_addr[CB_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);
            pix2[CB_INDEX] = *(color_buf_y_start_addr[CB_INDEX] + pt_keep_x_param[x].next_x_index_CbCr);
            pix4[CB_INDEX] = *(color_buf_next_y_start_addr[CB_INDEX] + pt_keep_x_param[x].next_x_index_CbCr);

            pix1[CR_INDEX] = *(color_buf_y_start_addr[CR_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);
            pix3[CR_INDEX] = *(color_buf_next_y_start_addr[CR_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_CbCr);
            pix2[CR_INDEX] = *(color_buf_y_start_addr[CR_INDEX] + pt_keep_x_param[x].next_x_index_CbCr);
            pix4[CR_INDEX] = *(color_buf_next_y_start_addr[CR_INDEX] + pt_keep_x_param[x].next_x_index_CbCr);

            /* (1-a)*(pixel1 + b*(pixel2-pixel1)) + a*(pixel3 + b*(pixel4-pixel3)) */
            /*
            Y1 = (UINT8)((UINT32) ( (UP_SCALE - scale_up_src_y_pos_remainder) *  ((pix1[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[Y_INDEX] - pix1[Y_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[Y_INDEX] - pix3[Y_INDEX]) ) ) >> (UP_SCALE_Q2));

            CB1 = (UINT8)((UINT32) ( (UP_SCALE - scale_up_src_y_pos_remainder) *  ((pix1[CB_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[CB_INDEX] - pix1[CB_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[CB_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[CB_INDEX] - pix3[CB_INDEX]) ) ) >> (UP_SCALE_Q2));

            CR1 = (UINT8)((UINT32) ( (UP_SCALE - scale_up_src_y_pos_remainder) *  ((pix1[CR_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[CR_INDEX] - pix1[CR_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[CR_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[CR_INDEX] - pix3[CR_INDEX]) ) ) >> (UP_SCALE_Q2));*/

            /* move to outside of x loop */
            /*ui4_tmp = UP_SCALE - scale_up_src_y_pos_remainder; */
            scale_up_src_x_pos_remainder = pt_keep_x_param[x].scale_up_src_x_pos_remainder;

            i2_Y = ((UINT32) ( ui4_tmp *  ((pix1[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[Y_INDEX] - pix1[Y_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[Y_INDEX] - pix3[Y_INDEX]) ) ) >> (UP_SCALE_Q2));

            i2_CB = ((UINT32) ( ui4_tmp *  ((pix1[CB_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[CB_INDEX] - pix1[CB_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[CB_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[CB_INDEX] - pix3[CB_INDEX]) ) ) >> (UP_SCALE_Q2));

            i2_CR = ((UINT32) ( ui4_tmp *  ((pix1[CR_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[CR_INDEX] - pix1[CR_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[CR_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[CR_INDEX] - pix3[CR_INDEX]) ) ) >> (UP_SCALE_Q2));

           /* Range-limiting is essential due to noise introduced by DCT losses. */
           if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
           {
               i2_R = i2_Y +pi2_Cr_r_tab[i2_CR];
               i2_G = i2_Y + ((INT16) RIGHT_SHIFT (pi4_Cb_g_tab[i2_CB] + pi4_Cr_g_tab[i2_CR], SCALEBITS));
               i2_B = i2_Y + pi2_Cb_b_tab[i2_CB];
           }
           else if (pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
           {
               i2_R = i2_Y;
               i2_G = i2_CB;
               i2_B = i2_CR;
           }

           if (!((i2_R|i2_G|i2_B) & 0xff00))
            {
                /*blit RGB of pixel value*/
                pf_blit_phase2_fct(x, y, (UINT8)i2_R, (UINT8)i2_G, (UINT8)i2_B, ui4_pitch, pui1_blit_fct_phase1_data);
            }
            else
            {
                RANGE_LIMIT (i2_R, i2_R);
                RANGE_LIMIT (i2_G, i2_G);
                RANGE_LIMIT (i2_B, i2_B);
                /*blit RGB of pixel value*/
                pf_blit_phase2_fct(x, y, (UINT8)i2_R, (UINT8)i2_G, (UINT8)i2_B, ui4_pitch, pui1_blit_fct_phase1_data);
            }
        } /* x for loop */
        /************************************************************************/
        /*            end resample a whole row                                  */
        /************************************************************************/
        pt_jpg_obj->ui2_output_image_lines++;
    }
FIN:
    /* increases pt_jpg_obj->ui2_num_of_resampling by 1 */
    pt_jpg_obj->ui2_num_of_resampling = pt_jpg_obj->ui2_num_of_resampling + 1;

} /* bilinear_YCBCR_resampling() function  */

/*-----------------------------------------------------------------------------
 * Name:  bilinear_Gray_resampling
 *
 * Description: bilinear interpolation to resample the image to desired size
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void bilinear_Gray_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal)
{
    UINT32 scale_up_src_y_pos;
    UINT32 src_y_pos_quotient;
    UINT32 scale_up_src_y_pos_remainder;
    UINT32 scale_up_src_x_pos;
    UINT32 src_x_pos_quotient;
    UINT32 scale_up_src_x_pos_remainder;
    UINT32 imcu_row_y_index;
    UINT32 next_x_index;
    register UINT32 y, x;
    UINT32 pix1[MAX_COMPS_IN_SCAN], pix2[MAX_COMPS_IN_SCAN], pix3[MAX_COMPS_IN_SCAN], pix4[MAX_COMPS_IN_SCAN];
    UINT8 *color_buf_y_start_addr[MAX_COMPS_IN_SCAN];
    UINT8 *color_buf_next_y_start_addr[MAX_COMPS_IN_SCAN];
    UINT32 org_comp_Y_image_width_sub_1, org_comp_Y_image_height_sub_1;
    UINT32 v_samp_factor_X_DCTSIZE, resamp_num_X_v_samp_X_DCTSIZE;
    UINT32 src_x;
    UINT32 src_y;
    UINT32 ui4_tmp;
    register INT16 Y1;
    register UINT32 ui4_display_image_width, ui4_display_image_height, ui4_h_pixel_ratio, ui4_v_pixel_ratio;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 ui4_Y_color_buf_width;
    KEEP_RESAMPLE_X_PARAMETERS* pt_keep_x_param = NULL;
    UINT32 ui4_pitch;
    UINT8* pui1_blit_fct_phase1_data;
    blit_pixel_fct pf_blit_phase2_fct;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pf_blit_phase2_fct = pt_jpg_obj->pf_blit_phase2_fct;
    if (pf_blit_phase2_fct == NULL)
    {
        return;
    }
    ui4_pitch = pt_lock_info->at_subplane[0].ui4_pitch;
    pui1_blit_fct_phase1_data = pt_jpg_obj->pui1_blit_fct_phase1_data;

    ui4_display_image_width = pt_jpg_obj->ui4_display_image_width;
    ui4_display_image_height = pt_jpg_obj->ui4_display_image_height;
    ui4_h_pixel_ratio = pt_jpg_obj->ui4_h_pixel_ratio;
    ui4_v_pixel_ratio = pt_jpg_obj->ui4_v_pixel_ratio;

    src_x = pt_src_dst_rect->ui4_src_x;
    src_y = pt_src_dst_rect->ui4_src_y;

    org_comp_Y_image_height_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1;
    org_comp_Y_image_width_sub_1 = pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1;
    v_samp_factor_X_DCTSIZE = pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE;

    resamp_num_X_v_samp_X_DCTSIZE = pt_jpg_obj->ui2_num_of_resampling * v_samp_factor_X_DCTSIZE;

    ui4_Y_color_buf_width = pt_info->ui4_pic_width_in_buf[Y_INDEX];

    /* caculate x parameters first and store it for future used*/
    if (pt_jpg_obj->pt_keep_x_params == NULL)
    {
        /* allocate space to store x parameters */
        pt_keep_x_param = x_mem_alloc(sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        if (pt_keep_x_param == NULL)
        {
            pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
            return;
        }
        x_memset(pt_keep_x_param, 0, sizeof(KEEP_RESAMPLE_X_PARAMETERS) * ui4_display_image_width);
        pt_jpg_obj->pt_keep_x_params = pt_keep_x_param;
        /* caculate x parameters first*/
        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            scale_up_src_x_pos = (x * ui4_h_pixel_ratio);
            src_x_pos_quotient = scale_up_src_x_pos >> UP_SCALE_Q;
            scale_up_src_x_pos_remainder = scale_up_src_x_pos & UP_SCALE_R;

            if (src_x_pos_quotient >= org_comp_Y_image_width_sub_1)
            {
                src_x_pos_quotient = org_comp_Y_image_width_sub_1;
                next_x_index = src_x_pos_quotient ;
            }
            else
            {
                next_x_index = src_x_pos_quotient + 1;
            }

            src_x_pos_quotient = src_x_pos_quotient + src_x;
            next_x_index = next_x_index + src_x;

            pt_keep_x_param[x].next_x_index_Y = next_x_index;
            pt_keep_x_param[x].scale_up_src_x_pos_remainder = scale_up_src_x_pos_remainder;
            pt_keep_x_param[x].src_x_pos_quotient_Y = src_x_pos_quotient;
        }
    }
    else
    {
        pt_keep_x_param = pt_jpg_obj->pt_keep_x_params;
    }

    for (y = pt_jpg_obj->ui2_output_image_lines; y < ui4_display_image_height; y++)
    {
        /* get double type destination y pos */
        scale_up_src_y_pos = (y * ui4_v_pixel_ratio);

        /* convert to integer part of destination y pos */
        src_y_pos_quotient = scale_up_src_y_pos >> UP_SCALE_Q;
        /* prevent out side of display height*/
        if (src_y_pos_quotient > org_comp_Y_image_height_sub_1)
            src_y_pos_quotient = org_comp_Y_image_height_sub_1;

        /* get remainder part of desination y pos */
        scale_up_src_y_pos_remainder = scale_up_src_y_pos & UP_SCALE_R;

        src_y_pos_quotient = src_y_pos_quotient + src_y;
        /* get y position for imcu row*/

        imcu_row_y_index = src_y_pos_quotient - resamp_num_X_v_samp_X_DCTSIZE;

        if (imcu_row_y_index >= v_samp_factor_X_DCTSIZE ) /* the below interpolation line don't exist in the current pt_jpg_obj->aapui1_color_buf */
        {
            /* we have done all the resampling lines of one component in this iMCU */
            goto FIN;
        }

        /* calculate color buffer (y) start address */
        color_buf_y_start_addr[Y_INDEX] = pui1_y_start_addr + imcu_row_y_index * ui4_Y_color_buf_width;

        /* calculate color buffer next row (y+1) start address */
        if (src_y_pos_quotient >= org_comp_Y_image_height_sub_1)
        {
            color_buf_next_y_start_addr[Y_INDEX] = color_buf_y_start_addr[Y_INDEX];
        }
        else
        {
            if ( (imcu_row_y_index + 1) >= v_samp_factor_X_DCTSIZE )
            {
                /* use another color buffer's first line data */
                color_buf_next_y_start_addr[Y_INDEX] = pui1_next_y_start_addr;
            }
            else
            {
                color_buf_next_y_start_addr[Y_INDEX] = color_buf_y_start_addr[Y_INDEX] + ui4_Y_color_buf_width;
            }
        }

        ui4_tmp = UP_SCALE - scale_up_src_y_pos_remainder;
        /************************************************************************/
        /*            begin resample a whole row                                */
        /************************************************************************/
        for (x = 0; x < (UINT32)ui4_display_image_width; x ++)
        {
            /* get the four most neighboring pixels in original image */
            /* pix1 pix2                                        */
            /*    O                                           */
            /* pix3 pix4                                        */
            /* We should use pt_info->ui4_pic_width_in_buf instead of pt_jpg_obj->image_width to calculate pixel actual address */
            pix1[Y_INDEX] = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);
            pix3[Y_INDEX] = *(color_buf_next_y_start_addr[Y_INDEX] + pt_keep_x_param[x].src_x_pos_quotient_Y);
            pix2[Y_INDEX] = *(color_buf_y_start_addr[Y_INDEX] + pt_keep_x_param[x].next_x_index_Y);
            pix4[Y_INDEX] = *(color_buf_next_y_start_addr[Y_INDEX] + pt_keep_x_param[x].next_x_index_Y);


            /* (1-a)*(pixel1 + b*(pixel2-pixel1)) + a*(pixel3 + b*(pixel4-pixel3)) */
            /*
            Y1 = (UINT8)((UINT32) ( (UP_SCALE - scale_up_src_y_pos_remainder) *  ((pix1[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[Y_INDEX] - pix1[Y_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[Y_INDEX] - pix3[Y_INDEX]) ) ) >> (UP_SCALE_Q2));
			*/
            /* move to outside of x loop */
            /*ui4_tmp = UP_SCALE - scale_up_src_y_pos_remainder; */
            scale_up_src_x_pos_remainder = pt_keep_x_param[x].scale_up_src_x_pos_remainder;

            Y1 = (UINT8)((UINT32) ( ui4_tmp *  ((pix1[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix2[Y_INDEX] - pix1[Y_INDEX]) ) +
                                   scale_up_src_y_pos_remainder * ((pix3[Y_INDEX] << UP_SCALE_Q) + scale_up_src_x_pos_remainder * (pix4[Y_INDEX] - pix3[Y_INDEX]) ) ) >> (UP_SCALE_Q2));

            if (pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D8888 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D4444 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_ARGB_D1555 ||
                pt_lock_info->e_clrmode == GL_COLORMODE_RGB_D565)
            {
                pf_blit_phase2_fct(x, y, Y1, Y1, Y1, ui4_pitch, pui1_blit_fct_phase1_data);
            }
            else if (pt_lock_info->e_clrmode == GL_COLORMODE_AYUV_D8888)
            {
                pf_blit_phase2_fct(x, y, Y1, 127, 127, ui4_pitch, pui1_blit_fct_phase1_data);
            }

        } /* x for loop */
        /************************************************************************/
        /*            end resample a whole row                                  */
        /************************************************************************/
        pt_jpg_obj->ui2_output_image_lines++;
    }
FIN:
    /* increases pt_jpg_obj->ui2_num_of_resampling by 1 */
    pt_jpg_obj->ui2_num_of_resampling = pt_jpg_obj->ui2_num_of_resampling + 1;

} /* bilinear_Gray_resampling() function  */
#endif

#ifdef HW_SCALER
static VOID _prepare_input_buffer(
    UINT8   *pui1_dst,
    UINT8   *pui1_src,
    UINT8   *pui1_next_src,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_dst_pitch,
    UINT32  ui4_src_pitch)
{
    /* start */
    _bitblt_buffer(pui1_dst, pui1_src, 0, 0, ui4_width, ui4_height, ui4_dst_pitch, ui4_src_pitch);
    /* next */
    _bitblt_buffer(pui1_dst, pui1_next_src, 0, (INT32)ui4_height, ui4_width, ui4_height, ui4_dst_pitch, ui4_src_pitch);
}

static VOID _bitblt_buffer(
    UINT8   *pui1_dst,
    UINT8   *pui1_src,
    INT32   i4_dst_x,
    INT32   i4_dst_y,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_dst_pitch,
    UINT32  ui4_src_pitch)
{
#if 1
    INT32           i4_ret;
    GFX_BITBLT_T    t_blt = {0};

    t_blt.pv_src   = pui1_src;
    t_blt.i4_src_x = 0;
    t_blt.i4_src_y = 0;
    t_blt.ui4_src_pitch = ui4_src_pitch;
    t_blt.e_src_cm = GFX_COLORMODE_ARGB_CLUT8;

    t_blt.pv_dst   = pui1_dst;
    t_blt.i4_dst_x = i4_dst_x;
    t_blt.i4_dst_y = i4_dst_y;
    t_blt.ui4_dst_pitch = ui4_dst_pitch;
    t_blt.e_dst_cm = GFX_COLORMODE_ARGB_CLUT8;

    t_blt.ui4_width  = ui4_width;
    t_blt.ui4_height = ui4_height;

    i4_ret = rm_set(
        gl_get_gpu(),
        FALSE,
        ANY_PORT_NUM,
        GFX_SET_TYPE_BITBLT,
        &t_blt,
        sizeof(GFX_BITBLT_T),
        NULL);

    if (i4_ret != RMR_OK)
    {
        IMG_ERROR(("{JPEG MW} _bitblt_buffer() = %d error.\n", i4_ret));
    }

#else
    UINT8   *pui1_start, *pui1_src_start;

    pui1_start = pui1_dst + i4_dst_y * ui4_dst_pitch;

    pui1_src_start = pui1_src;

    for (; ui4_height != 0; ui4_height--)
    {
        x_memcpy(pui1_start + i4_dst_x, pui1_src_start, ui4_width);

        pui1_start     += ui4_dst_pitch;
        pui1_src_start += ui4_src_pitch;
    }
#endif
}

static VOID _scale_buffer(
    UINT8           *pui1_dst,
    UINT8           *pui1_src,
    UINT32          ui4_dst_width,
    UINT32          ui4_dst_height,
    INT32           i4_src_x,
    INT32           i4_src_y,
    UINT32          ui4_src_width,
    UINT32          ui4_src_height,
    UINT32          ui4_dst_pitch,
    UINT32          ui4_src_pitch,
    DRV_SET_TYPE_T  e_gfx_op)
{
    INT32                   i4_ret;
    GFX_STRETCH_BITBLT_T    t_blt = {0};

    t_blt.pv_src         = pui1_src;
    t_blt.i4_src_x       = i4_src_x;
    t_blt.i4_src_y       = i4_src_y;
    t_blt.ui4_src_pitch  = ui4_src_pitch;
    t_blt.e_src_cm       = GFX_COLORMODE_ARGB_CLUT8;

    t_blt.pv_dst         = pui1_dst;
    t_blt.i4_dst_x       = 0;
    t_blt.i4_dst_y       = 0;
    t_blt.ui4_dst_pitch  = ui4_dst_pitch;
    t_blt.e_dst_cm       = GFX_COLORMODE_ARGB_CLUT8;

    t_blt.ui4_width      = ui4_src_width;
    t_blt.ui4_height     = ui4_src_height;

    t_blt.ui4_dst_width  = ui4_dst_width;
    t_blt.ui4_dst_height = ui4_dst_height;

    i4_ret = rm_set(
        gl_get_gpu(),
        FALSE,
        ANY_PORT_NUM,
        GFX_SET_TYPE_ADV_STRETCH_BITBLT,
        &t_blt,
        sizeof(GFX_STRETCH_BITBLT_T),
        NULL);

    if (i4_ret != RMR_OK)
    {
        IMG_ERROR(("{JPEG MW} _scale_buffer() = %d error.\n", i4_ret));
    }
}

static UINT8 *_create_surface_buffer(GL_HSURFACE_T *ph_surf, UINT32 ui4_width, UINT32 ui4_height, UINT32 *ui4_pitch)
{
    INT32               i4_ret;
    GL_SURFACE_DESC_T   t_desc;
    GL_LOCK_INFO_T      t_lock_info;

    t_desc.ui4_width  = ui4_width;
    t_desc.ui4_height = ui4_height;
    t_desc.h_clipper  = NULL_HANDLE;
    t_desc.h_palette  = NULL_HANDLE;
    t_desc.e_clrmode  = GL_COLORMODE_ARGB_CLUT8;

    i4_ret = x_gl_surface_create(&t_desc, ph_surf);
    if (i4_ret != GLR_OK)
    {
        IMG_ERROR(("{JPEG MW} _create_surface_buffer() = %d failed.\n", i4_ret));
        return NULL;
    }

    i4_ret = x_gl_surface_lock(*ph_surf, NULL, 0, &t_lock_info);
    if (i4_ret != GLR_OK)
    {
        IMG_ERROR(("{JPEG MW} _create_surface_buffer() = %d failed.\n", i4_ret));
        return NULL;
    }

    *ui4_pitch = t_lock_info.at_subplane[0].ui4_pitch;

    return (UINT8 *)t_lock_info.at_subplane[0].pv_bits;
}

static VOID _flush_buffer(HANDLE_T *ph_surf)
{
    INT32   i4_ret;

    i4_ret = _flush_cmd(1, ph_surf, GL_SYNC);
    if (i4_ret != GLR_OK)
    {
        IMG_ERROR(("{JPEG MW} _flush_buffer() = %d failed.\n", i4_ret));
    }
}

VOID hw_set_scaler_mode(DRV_SET_TYPE_T e_gfx_op)
{
    _e_gfx_op = e_gfx_op;
}

static VOID _release_surface(HANDLE_T *ph_surf)
{
    INT32   i4_ret;

    i4_ret = x_gl_surface_unlock(*ph_surf, NULL);

    if (i4_ret != GLR_OK)
    {
        IMG_ERROR(("x_gl_surface_unlock() = %d failed.\n", i4_ret));
    }

    i4_ret = x_gl_obj_free(*ph_surf);

    if (i4_ret != GLR_OK)
    {
        IMG_ERROR(("x_gl_surface_unlock() = %d failed.\n", i4_ret));
    }

    *ph_surf = NULL_HANDLE;
}

#ifdef HW_YCBCR
static VOID _merge_cbcr_buf(
    UINT8   *pui1_dst_cbcr_buf,
    UINT32  ui4_dst_cbcr_pitch,
    UINT32  ui4_dst_width,
    UINT32  ui4_dst_height,
    UINT8   *pui1_cb_buf,
    UINT32  ui4_cb_pitch,
    UINT8   *pui1_cr_buf,
    UINT32  ui4_cr_pitch)
{
    UINT32  ui4_x;
    UINT8   *pui1_cbcr_index;
    UINT8   *pui1_cb_index;
    UINT8   *pui1_cr_index;

    for ( ; ui4_dst_height != 0; ui4_dst_height--)
    {
        pui1_cbcr_index = pui1_dst_cbcr_buf - sizeof(UINT16);
        pui1_cb_index   = pui1_cb_buf       - sizeof(UINT8);
        pui1_cr_index   = pui1_cr_buf       - sizeof(UINT8);

        for (ui4_x = 0; ui4_x < ui4_dst_width; ui4_x++)
        {
            *(++((UINT16 *)pui1_cbcr_index)) = (UINT16)((*(++pui1_cr_index) << 8) | *(++pui1_cb_index));
        }

        pui1_dst_cbcr_buf += ui4_dst_cbcr_pitch;
        pui1_cb_buf       += ui4_cb_pitch;
        pui1_cr_buf       += ui4_cr_pitch;
    }
}

static VOID _color_convert(
    UINT8           *pui1_dst_buf,
    UINT32          ui4_dst_pitch,
    INT32           i4_dst_x,
    INT32           i4_dst_y,
    GL_COLORMODE_T  e_dst_colormode,
    UINT8           *pui1_y_buf,
    UINT32          ui4_y_pitch,
    INT32           i4_src_x,
    INT32           i4_src_y,
    UINT8           *pui1_cbcr_buf,
    UINT32          ui4_cbcr_pitch,
    UINT32          ui4_width,
    UINT32          ui4_height)
{
    INT32               i4_ret;
    GFX_YCBCR_TO_RGB_T  t_blt;

    t_blt.pv_src         = pui1_y_buf + i4_src_y * ui4_y_pitch + i4_src_x;
    t_blt.i4_src_x       = 0;
    t_blt.i4_src_y       = 0;
    t_blt.ui4_src_pitch  = ui4_y_pitch;
    t_blt.e_src_cm       = GFX_COLORMODE_AYCbCr_CLUT8;

    t_blt.pv_dst         = pui1_dst_buf;
    t_blt.i4_dst_x       = i4_dst_x;
    t_blt.i4_dst_y       = i4_dst_y;
    t_blt.ui4_dst_pitch  = ui4_dst_pitch;
    t_blt.e_dst_cm       = (GFX_COLORMODE_T)e_dst_colormode;

    t_blt.ui4_width      = ui4_width;
    t_blt.ui4_height     = ui4_height;

    t_blt.pv_cbcr        = pui1_cbcr_buf + (i4_src_y / 2) * ui4_cbcr_pitch + (i4_src_x / 2);
    t_blt.ui4_cbcr_pitch = ui4_cbcr_pitch;
    t_blt.e_ycbcr_format = GFX_YCBCR_420_LNR;

    t_blt.ui1_alpha      = 255;

    i4_ret = rm_set(
        gl_get_gpu(),
        FALSE,
        ANY_PORT_NUM,
        GFX_SET_TYPE_YCBCR_TO_RGB,
        &t_blt,
        sizeof(GFX_YCBCR_TO_RGB_T),
        NULL);

    if (i4_ret != RMR_OK)
    {
        IMG_ERROR(("{JPEG MW} _color_convert() = %d error.\n", i4_ret));
    }
}
#endif /* HW_YCBCR */

#endif /* HW_SCALER*/

