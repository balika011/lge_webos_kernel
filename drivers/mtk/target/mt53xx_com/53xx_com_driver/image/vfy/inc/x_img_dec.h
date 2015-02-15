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
/*----------------------------------------------------------------------------*
 * $RCSfile: x_img_dec.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $ 2007/08/27
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: Vincent Hsu $
 * $MD5HEX:  $
 *
 * Description: 
 *         This file exports all set & get types commands and related structures 
 *         for the image decoder driver.
 *---------------------------------------------------------------------------*/

#ifndef _X_IMG_DEC_H_
#define _X_IMG_DEC_H_


/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_gfx.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "drv_config.h"
//#include "x_drv_cb.h"
//#include "sys_config.h"
//#include "u_img_mem_def.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/*! \name Get Operations
* @{
*/
#define IMG_GET_TYPE_PROGRESS                       \
    (RM_GET_TYPE_LAST_ENTRY + ((DRV_GET_TYPE_T) 1))
#define IMG_GET_TYPE_MEMORY                       \
    (RM_GET_TYPE_LAST_ENTRY + ((DRV_GET_TYPE_T) 2))

#define IMG_GET_TYPE_FREE_MEMORY                       \
    (RM_GET_TYPE_LAST_ENTRY + ((DRV_GET_TYPE_T) 3))

    
/*! @} */
    
/*! \name Set Operations
* @{
*/
#define IMG_SET_TYPE_FRM_START                      \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 1))
    
#define IMG_SET_TYPE_DECODE                         \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 2))
    
#define IMG_SET_TYPE_STOP                           \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 3))
    
#define IMG_SET_TYPE_BUF_FILLED                     \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 4))
    
#define IMG_SET_TYPE_FRM_END                        \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 5))

#define IMG_SET_TYPE_DIRECT_DECODE                  \
    (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 6))
/*! @} */

/* Driver error codes */
#define IMG_DRV_HW_ERROR        (-1)
#define IMG_DRV_IO_ERROR        (-2)
#define IMG_DRV_NOT_SUPPORT     (-3)

/* MW buffer-filling return codes */
#define IMG_MW_FILL_OK          ( 0)
#define IMG_MW_FILL_EOF         (-1)
#define IMG_MW_FILL_ERROR       (-2)
/* notification states */
typedef enum
{
    IMG_NFY_FILL_BUF,
    IMG_NFY_FINISHED,
    IMG_NFY_ERROR,
    IMG_NFY_STOP_DONE,
    IMG_NFY_ALREADY_STOPPED
} IMG_NFY_STATE_T;

/* data passed with IMG_NFY_FILL_BUF */
typedef struct
{
    UINT32          ui4_trsn_id;        /* transaction id */
    VOID            *pv_start_add;      /* starting address */
    UINT32          ui4_required_len;   /* required length */
    BOOL            b_reset_pos;        /* position-resetting flag */
    UINT32          ui4_position;       /* the position to be resettd */
#if CONFIG_SYS_MEM_PHASE3
    UINT32          u4Offset;           /* for memory phase III, handle replace address*/
#endif
} IMG_FILL_BUF_T;


#if UNIFORM_DRV_CALLBACK
typedef struct
{
    UINT32 ui4_img_id;
    IMG_NFY_STATE_T e_state;
    union
    {
        void *pv_cache;
        IMG_FILL_BUF_T rFillBuf;
    }rInfo;
}IMG_NFY_PARAM_T;

#else
/* callback function */
typedef VOID (*IMG_NFY_FCT_T)
(
    UINT32          ui4_img_id,         /* the image id which causes this notification */
    VOID            *pv_tag,            /* tag passed to the callback function */
    VOID            *pv_data,           /* data passed with this notification */
    IMG_NFY_STATE_T e_state);           /* notification state */
#endif

/* rotation information */
typedef enum
{
    IMG_ROTATE_NONE             = 0,    /* no rotation */
    IMG_ROTATE_CW_90            = 1,    /* clockwise  90 degrees  */
    IMG_ROTATE_CW_180           = 2,    /* clockwise 180 degrees  */
    IMG_ROTATE_CW_270           = 3,    /* clockwise 270 degrees  */
    IMG_ROTATE_NONE_WITH_FLIP   = 4,    /* no rotation, with flip */
    IMG_ROTATE_CW_90_WITH_FLIP  = 5,    /* clockwise  90 degrees, with flip */
    IMG_ROTATE_CW_180_WITH_FLIP = 6,    /* clockwise 180 degrees, with flip */
    IMG_ROTATE_CW_270_WITH_FLIP = 7     /* clockwise 270 degrees, with flip */
} IMG_ROTATE_T;

/* decode quality factor */
typedef enum
{
    IMG_QUALITY_FACTOR_NORMAL = 0,
    IMG_QUALITY_FACTOR_LOW,
    IMG_QUALITY_FACTOR_FAST
} IMG_QUALITY_FACTOR_T;

typedef enum
{
    IMG_JPG_DECODE_NORMAL = 0,
    IMG_JPG_DECODE_SAME_PIC,
} IMG_JPG_DECODE_FLAG_E;

typedef INT32 (*VDEC_JPEG_DRV_NOTIFY)(void *pvPrivData);

typedef struct
{
  VDEC_JPEG_DRV_NOTIFY pvCallBackFunc;
  void *pvPrivData;
} VDEC_JPEG_DRV_NOTIFY_CB_REG_T;


/* data passed with IMG_SET_TYPE_FRM_START */
typedef struct
{
    UINT32          ui4_img_id;         /* the image id decoded */
    VOID            *pv_img_buf;        /* image data */
    UINT32          ui4_img_size;       /* image size */
    VOID            *pv_aux_cache;      /* auxiliary cache data */
    #if UNIFORM_DRV_CALLBACK
    x_drv_cb_nfy_fct pf_func;
    #else
    IMG_NFY_FCT_T   pf_func;            /* callback function */
    #endif
    VOID            *pv_tag;            /* tag passed to the callback function */
    IMG_JPG_DECODE_FLAG_E          e_jpg_flag;     /* IMG_JPG_DECODE_FLAG_E */
    BOOL            fgVdecPath;
    void *          hJfifInst;
    VDEC_JPEG_DRV_NOTIFY_CB_REG_T* prVdecCallback;   
    UINT32         ui4_img_position;
    BOOL             fgKernelImgData;
} IMG_FRM_START_T;


/* data passed with IMG_SET_TYPE_BUF_FILLED */
typedef struct
{
    UINT32          ui4_trsn_id;        /* transaction id */
    INT32           i4_ret;             /* MW buffer-filling return codes */
    UINT32          ui4_filled_len;     /* filled length */
} IMG_BUF_FILLED_T;

/* data passed with IMG_SET_TYPE_DECODE */
typedef struct
{	  
    VOID            *pv_img_buf;        /* image data */
    VOID            *pv_img_buf2;       /* image data2 */
    UINT32          ui4_img_size;       /* image size */
    VOID            *pv_type_data;      /* type data */

    UINT32          ui4_src_x;          /* x offset in the source image in pixels */
    UINT32          ui4_src_y;          /* y offset in the source image in pixels */
    UINT32          ui4_src_width;      /* width to be decoded in pixels */
    UINT32          ui4_src_height;     /* height to be decoded in pixels */

    VOID            *pv_dst;            /* destination starting address */
    VOID            *pv_dst2;           /* additional dst starting address */
    UINT32          ui4_dst_x;          /* x offset in the destination in pixels */
    UINT32          ui4_dst_y;          /* y offset in the destination in pixels */
    UINT32          ui4_dst_width;      /* expected output width in pixels */
    UINT32          ui4_dst_height;     /* expected output height in pixels */
    UINT32          ui4_dst_pitch;      /* pitch of the destination image */
    UINT32          ui4_dst2_pitch;     /* pitch of the additional destination image */
    GFX_COLORMODE_T e_dst_cm;           /* destination color mode */
    
    IMG_ROTATE_T    e_rotate;           /* rotation option */
} IMG_DIRECT_DECODE_T;

typedef struct
{
    UINT32          ui4_img_id;         /* the image id decoded */
    VOID            *pv_type_data;      /* frame index */

    UINT32          ui4_src_x;          /* x offset in the source image in pixels */
    UINT32          ui4_src_y;          /* y offset in the source image in pixels */
    UINT32          ui4_src_width;      /* width to be decoded in pixels */
    UINT32          ui4_src_height;     /* height to be decoded in pixels */

    VOID            *pv_dst;            /* destination starting address */
    VOID            *pv_dst2;           /* additional dst starting address */
    UINT32          ui4_dst_x;          /* x offset in the destination in pixels */
    UINT32          ui4_dst_y;          /* y offset in the destination in pixels */
    UINT32          ui4_dst_width;      /* expected output width in pixels */
    UINT32          ui4_dst_height;     /* expected output height in pixels */
    UINT32          ui4_dst_pitch;      /* pitch of the destination image */
    UINT32          ui4_dst2_pitch;     /* pitch of the additional destination image */
    GFX_COLORMODE_T e_dst_cm;           /* destination color mode */
    
    IMG_ROTATE_T    e_rotate;           /* rotation option */
    IMG_QUALITY_FACTOR_T e_quality;     /* quality factor */ //[20081014] BDP00013841
    BOOL            b_compressed;       /* PNG WT*/
    BOOL            b_directdecodemode; /* for call png decode from user space directly */
} IMG_DECODE_T;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _X_IMG_DEC_H_ */

