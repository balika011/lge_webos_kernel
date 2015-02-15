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
 * $RCSfile: mtgfx.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file mtgfx.h
 *  This header file declares exported APIs of GFX module.
*/
#ifndef MT_GFX_H
#define MT_GFX_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mttype.h"


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/** The common bitblt field
*/
#define MTGFX_COMMON_BITBLT_FIELDS \
    VOID*           pvSrc;         \
    INT32           i4SrcX;       \
    INT32           i4SrcY;       \
    UINT32          u4SrcPitch;  \
    UINT8           eSrcCm;       \
                                    \
    VOID*           pvDst;         \
    INT32           i4DstX;       \
    INT32           i4DstY;       \
    UINT32          u4DstPitch;  \
    UINT8           eDstCm;       \
                                    \
    UINT32          u4Width;      \
    UINT32          u4Height;

/**The color mode component
*/
typedef enum
{
    MTGFX_CM_YCBCR_CLUT2,
    MTGFX_CM_YCBCR_CLUT4,
    MTGFx_CM_YCBCR_CLUT8,
    MTGFX_CM_RESERVED_0,
    MTGFX_CM_CBYCRY422_DIRECT16,
    MTGFX_CM_YCBYCR422_DIRECT16,
    MTGFX_CM_AYCBCR8888_DIRECT32,
    MTGFX_CM_RESERVED_1,
    MTGFX_CM_RGB_CLUT2,
    MTGFX_CM_RGB_CLUT4,
    MTGFX_CM_RGB_CLUT8,
    MTGFX_CM_RGB565_DIRECT16,
    MTGFX_CM_ARGB1555_DIRECT16,
    MTGFX_CM_ARGB4444_DIRECT16,
    MTGFX_CM_ARGB8888_DIRECT32,
    MTGFX_CM_RESERVED_2,
} MTGFX_COLORMODE_T;

/**The type of alpha composition operation
*/
typedef enum
{
    MTGFX_AC_CLEAR = 0,
    MTGFX_AC_DST_IN,
    MTGFX_AC_DST_OUT,
    MTGFX_AC_DST_OVER,
    MTGFX_AC_SRC,
    MTGFX_AC_SRC_IN,
    MTGFX_AC_SRC_OUT,
    MTGFX_AC_SRC_OVER,
    MTGFX_AC_DST,
    MTGFX_AC_SRC_ATOP,
    MTGFX_AC_DST_ATOP,
	MTGFX_AC_XOR
} MTGFX_PD_RULE_T;

/**The type of ROP operation
*/
typedef enum
{
    MTGFX_ROP_NOT_SRC = 0,
    MTGFX_ROP_NOT_DST,
    MTGFX_ROP_SRC_XOR_DST,
    MTGFX_ROP_SRC_XNOR_DST,
    MTGFX_ROP_SRC_AND_DST,
    MTGFX_ROP_NOT_SRC_AND_DST,
    MTGFX_ROP_SRC_AND_NOT_DST,
    MTGFX_ROP_NOT_SRC_AND_NOT_DST,
    MTGFX_ROP_SRC_OR_DST,
    MTGFX_ROP_NOT_SRC_OR_DST,
    MTGFX_ROP_SRC_OR_NOT_DST,
    MTGFX_ROP_NOT_SRC_OR_NOT_DST,
    MTGFX_ROP_MULTI
} MTGFX_ROP_TYPE_T;

/** Video standard mode
*/
typedef enum
{
    MTGFX_VSTD_BT601 = 0,
    MTGFX_VSTD_BT709
} MTGFX_YCBCR_SYSTEM_T;


/** YCbCr format
*/
typedef enum
{
    MTGFX_YCBCR_420_MB  = 0,
    MTGFX_YCBCR_420_LNR = 1,
    MTGFX_YCBCR_422_LNR = 2       /* MT538x,539x only */
} MTGFX_YCBCR_FORMAT_T;
/** YCbCr format converstion type,to mark the kind of source and dst color format
*/
typedef enum
{
    MTGFX_YCbCr420              = 0,
    MTGFX_RGB,
    MTGFX_ARGB888,             
    MTGFX_YCbCrBlockToRaster_2Buf,
    MTGFX_YCbCrBlockToRaster_3Buf
} MTGFX_COLORMODE_CONVERSION_T;

/** Video standard mode
*/
typedef enum 
{
    MT_E_VSTD_BT601 = 0,
    MT_E_VSTD_BT709
} MTGFX_VIDSTD_T;

/** Byte alignment setting for palette
*/
typedef enum
{
    MTGFX_PALETTE_MSB = 0,
    MTGFX_PALETTE_LSB = 1,
} MTGFX_BYTE_ALIGNED_T;

/** The direction of rotation
*/
typedef enum
{
    MTGFX_CLOCKWISE   = 0,
    MTGFX_C_CLOCKWISE = 1
} MTGFX_ROTATE_FLAG_T;

/** the gfx alpha composition mode
*/
typedef enum 
{
    MT_E_AC_CLEAR = 0,
    MT_E_AC_DST_IN,
    MT_E_AC_DST_OUT,
    MT_E_AC_DST_OVER,
    MT_E_AC_SRC,
    MT_E_AC_SRC_IN,
    MT_E_AC_SRC_OUT,
    MT_E_AC_SRC_OVER,
    MT_E_AC_DST,
    MT_E_AC_SRC_ATOP,
    MT_E_AC_DST_ATOP,
    MT_E_AC_XOR,
    MT_E_AC_NONE,
    MT_E_AC_ADD,
} MTGFX_AC_MODE_T;

/**alpha edge mode for VGFX
*/
typedef enum 
{
    MT_VGX_ALPHA_EDGE_NEAREST,   // argb nearest neighbor
    MT_VGX_ALPHA_EDGE_ZERO,   // a=0, rgb nearest neighbor
    MT_VGX_ALPHA_EDGE_NORMAL,  //normal 
    MT_VGX_ALPHA_EDGE_MAX
} MTVGFX_ALPHA_EDGE_MD;              

/**filter mode for VGFX
*/
typedef enum 
{
    MT_VGFX_FILTER_2TAP,   
    MT_VGFX_FILTER_4TAP,  
} MTVGFX_FILTER_T;   

/**teh option of VGFX
*/
typedef enum 
{
    MT_u4RefEn = 0 ,
    MT_u4AlphaCompEn = 2 ,
    MT_u4AntiAliasing = 4,             //anti-alising          0x300_OK//en
    MT_u4FilterEn = 8,
} MTVGFX_OPTION_ENUM;

#define GFX_INIT_MMU_VAL        (1)

// #define  MTGFX_FLUSH_INVALID_DCACHE           0x1
#define  MTGFX_INVALID_DCACHE                 0x2
#define  MTGFX_FLUSH_DCACHE                   0x4

/** The structure contains of alphacomposition for VGFX
*/

typedef struct
{
    unsigned long u4_init;                     // set: mmu reg to init value
    unsigned long u4_enable;                   // para: of mmu enable/disable  if do init.
    unsigned long u4_cmprss;                   // set:compress to dst
    unsigned long u4_src_rw_mmu;               // set:src mmu enable/disable
    unsigned long u4_dst_rw_mmu;               // set:dst mmu enable/disable
    unsigned long u4_vgfx_ord;                 // set:vgfx overread 
    unsigned long u4_vgfx_slva;                // para:  vgfx src last valid address
    unsigned long u4_pgt;                      // set: pate table address    
}MTGFX_MMU_SET_T;

/** The structure contains YCBCR to RGB conversion operation setting information
 */
typedef struct
{
    UINT8* pu1Addr;                             ///< [IN] start address
    UINT32 u4CbCrOffset;                            ///< [IN] address offset for Y/CbCr seperate mode
    UINT32 u4Depth;                             ///< [IN] number of bits per pixel  for Y component.
    UINT32 u4Pitch;                             ///< [IN] pitch
    UINT32 u4TotalLine;                         ///< [IN] total height
    UINT32 u4StartX;                                ///< [IN] start point X
    UINT32 u4StartY;                                ///< [IN] start point Y
    UINT32 u4Width;                             ///< [IN] region width
    UINT32 u4Height;                                ///< [IN] region height
    UCHAR  ucPicStruct;                            //1, TOP_FLD_PIC   2,BTM_FLD_PIC  3,FRM_PIC
    UINT32 u4DecRatio;
    MTGFX_MMU_SET_T t_mmu;
}MTGFX_IMG_REGION_T;
    

/** The structure contains YCBCR to RGB conversion operation setting information
*/
typedef struct
{
	VOID*			pvY;		  
	VOID*			pvCbCr; 						 
	VOID*			pvDst;	
	UINT32			u4Pitch;										 
	UINT32			u4Width;	  
	UINT32			u4Height;	
	UINT32			u4DstCm;
    UINT32			u4YPitch;
    UINT32			u4CbCrPitch;
    UINT32			u4DstPitch;
    UINT32          u4FieldPic;
} MTGFX_YCBCR_420MB_TO_RGB_T;

/** The structure contains YCBCR Block to ARGB8888,YCBCR raster 2bufer,YCBCR raster 3buffer operation
    setting information
*/
typedef struct
{
	UINT8 *pu1SrcY;                              //source Y address,should 4086 align
    UINT32 u4SrcYPitch;                          //source Y pitch
    UINT8 *pu1SrcCbCr;                           //source CbCr address,should 2048 align
	UINT32 u4SrcCbCrPitch;                       //source CbCr pitch
	UINT8 * u4DstAdress;                         //target address
    UINT32 u4DstPitch;
	UINT32 u4SrcWidth;                           //Source Width is the same with target height
	UINT32 u4SrcHeight;                          //Source Height is the same with target height
	UINT32 u4AlphaValue;                    
	UINT32 u4FieldPic;                           //Should be set to 1 or 0	
    BOOL fgLockDst;                              //If Dst Address need lock
    MTGFX_COLORMODE_CONVERSION_T dstColorformat; // ARGB888 or Ycbcr 2buffer raster or Ycbcr 3buffer raster
} MTGFX_YCBCR_COLOR_FORMAT_CONVERSION_T;

/**The structure contains ALPACOMP set
*/
typedef struct _MTVGFX_ALPHACOMP_SET_T
{
	UINT32 u4AlphaCompAr;
	UINT32 u4PreSrcRd;
    UINT32 u4PreDstRd;
    UINT32 u4PreDstWt;
	BOOL fgAlphaCompNormal; 
	MTVGFX_ALPHA_EDGE_MD _AlphaEdgeMd;
	MTGFX_AC_MODE_T u4AlphaCompOpCode;
} MTVGFX_ALPHACOMP_SET_T;

/** The structure contains of WB for VGFX
*/
typedef struct _MTVGFX_WB_SET_T
{
	UINT32 u4BaseAddr;
	UINT32 u4ColrMode; 
	UINT32 u4WBWidth;
	UINT32 u4WBHeight;
	UINT32 u4GlAr;
	UINT32 u4Pitch;
} MTVGFX_WB_SET_T;

/** The structure contains of QUADRILATERAL for VGFX
*/
typedef struct _MTVGFX_QUADRILATERAL_SET_T
{
	UINT32 x1;
	UINT32 y1; 
	UINT32 x2;
	UINT32 y2; 
	UINT32 x3;
	UINT32 y3; 
	UINT32 x4;
	UINT32 y4; 
} MTVGFX_QUADRILATERAL_SET_T;

/** The structure contains of Image for VGFX
*/
typedef struct _MTVGFX_IMGAGE_SET_T
{
	UINT32 u4BaseAddr;
	UINT32 u4ColrMode; 
	UINT32 u4GlAr;
	UINT32 u4Pitch;
} MTVGFX_IMGAGE_SET_T;

/** The structure contains of Texture for VGFX
*/
typedef struct _MTVGFX_TEXTURE_SET_T
{
	UINT32 u4StartX;
	UINT32 u4StartY;
	UINT32 u4EndX;
	UINT32 u4EndY;
} MTVGFX_TEXTURE_SET_T;

/** The structure contains of BoundingBOx for VGFX
*/
typedef struct _MTVGFX_BB_SET_T
{
	UINT32 u4StartX;
	UINT32 u4StartY;
	UINT32 u4Width;
	UINT32 u4Height;
} MTVGFX_BB_SET_T;

/** The structure contains of Rect for VGFX
*/
typedef struct _MTVGFX_RECT_SET_T
{
	UINT32 u4StartX;
	UINT32 u4StartY;
	UINT32 u4Width;
	UINT32 u4Height;
} MTVGFX_RECT_SET_T;

/** The structure contains of Reflect for VGFX
*/
typedef struct _MTVGFX_REFLECT_SET_T
{
	UINT32 u4RefOnly;
	UINT32 u4TextTureHeight;
	UINT32 u4RefRatioInit;
	UINT32 u4RatioLineStep;
	UINT32 u4RefAAEn;
} MTVGFX_REFLECT_SET_T;
/** This structure has no usefull now
*/
typedef struct _MTVGFX_MATRIX_T
{
	UINT32          u4_a;
    UINT32          u4_b;
    UINT32          u4_c;
    UINT32          u4_d;
    UINT32          u4_e;
    UINT32          u4_f;
    UINT32          u4_g;
    UINT32          u4_h;
    UINT32          u4_i;
}MTVGFX_MATRIX_T;
/** The structure contains scaling rotation operation setting information
*/
typedef struct _MTVGFX_PARAM_SET_T
{
	UINT32 _u4Option;

	MTVGFX_WB_SET_T _WBType;
	MTVGFX_IMGAGE_SET_T _IMAGType;
	MTVGFX_BB_SET_T _BBType;
	MTVGFX_MATRIX_T _SetMartix;

    MTVGFX_QUADRILATERAL_SET_T _SrcQuard;
    MTVGFX_QUADRILATERAL_SET_T _DstQuard;
    
    MTVGFX_REFLECT_SET_T _SetReflect;
    MTVGFX_ALPHACOMP_SET_T _SetAlphaComp;
    MTVGFX_FILTER_T _FilterType;
    MTGFX_MMU_SET_T       t_mmu;
} MTVGFX_PARAM_SET_T;

/** The structure contains bitblt operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    UINT8       u1Alpha;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_BITBLT_T;

/** The structure contains bitblt operation setting information
*/
typedef struct _MTGFX_CACHE_DATA
{
    unsigned int u4_addr;
    unsigned int u4_phy;
    unsigned int u4_size;
    unsigned int u4_fg;
} MTGFX_CACHE_DATA;

/** The structure contains rectangle fill operation setting information
*/
typedef struct
{
    VOID*           pvDst;
    INT32           i4DstX;
    INT32           i4DstY;
    UINT32          u4DstPitch;
    UINT8           eDstCm;
    UINT32          u4Width;
    UINT32          u4Height;
    UINT32          u4Color;
	MTGFX_MMU_SET_T t_mmu;
} MTGFX_FILL_T;

/** The structure contains transparent bitblt opreation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    BOOL        bInverseMasking;
    UINT32      u4ColorSpaceMin;
    UINT32      u4ColorSpaceMax;
    BOOL        bSrcOrDstKey;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_TRANSPARENT_BITBLT_T;

/** The structure contains transparent fill opreation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    BOOL        bInverseMasking;
    UINT32      u4ColorSpaceMin;
    UINT32      u4ColorSpaceMax;
    UINT32      u4FillColor;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_TRANSPARENT_FILL_T;

/** The structure contains alpha blending operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    UINT8       u1Alpha; // new=(src*alpha + dst*(255-alpha))/255
    MTGFX_MMU_SET_T       t_mmu;
} MTGFX_ALPHA_BLEND_T;

/** The structure contains alpha map operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_ALPHAMAP_BITBLT_T;

/** The structure contains YCBCR to RGB conversion operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    VOID                *pvCbCr;
    UINT32              ui4CbCrPitch;
    UINT8               u1Alpha;
    MTGFX_YCBCR_FORMAT_T  eYcbcrFormat; // 420 or 422
    MTGFX_YCBCR_SYSTEM_T  eYcbcrSystem; // 601 or 709
    MTGFX_MMU_SET_T       t_mmu;
} MTGFX_YCBCR_TO_RGB_T;

/** The sturcture contains YCBCR block to ARGB888,YCBCR raster 2bufffer,YCBCR raster 3buffer conversion 
    operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    VOID                *pvCbCr;
    UINT32              ui4CbCrPitch;
    UINT8               u1Alpha;
    UINT32                u4FieldPic;
    UINT32                u4ColorMode;
    MTGFX_YCBCR_FORMAT_T  eYcbcrFormat; // 420 or 422
    MTGFX_YCBCR_SYSTEM_T  eYcbcrSystem; // 601 or 709
    MTGFX_COLORMODE_CONVERSION_T srcColorformat;
    MTGFX_COLORMODE_CONVERSION_T dstColorformat;
    MTGFX_MMU_SET_T       t_mmu;
} MTGFX_YCBCR_FORMAT_CONVERSION_T;


/** The structure contains ROP operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS
    MTGFX_ROP_TYPE_T      eRopType;
    UINT32          u4Color;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_ROP_BITBLT_T;

/** The structure contains alpha composition operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    UINT32           u1Alpha;
    MTGFX_PD_RULE_T   ePDRule;
    BOOL            bRectSrcOption;
    UINT32          u4Color;
    // After MT5395, we support pre-multiply
    BOOL fgPremultSrc;
    BOOL fgPremultDst;
    BOOL fgPremultDstWr;
    //Add for Third buffer function
    BOOL            fgThirdEn;
    VOID*           pvThird;
    VOID*           pvThirdPallete;
    VOID*           pvSrcPallete;
    UINT32          u4ThirdColorMode;
    UINT32          u4ThirdPitch;
    MTGFX_BYTE_ALIGNED_T  eByteAligned;
	MTGFX_MMU_SET_T 	  t_mmu;
} MTGFX_ALPHA_COMPOSITION_T;
/** The structure contains GFX alpha composition operation setting information
*/

typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    UINT8           u1Alpha;
    MTGFX_PD_RULE_T   ePDRule;
    BOOL            bRectSrcOption;
    UINT32          u4Color;
    // After MT5395, we support pre-multiply
    BOOL fgPremultSrc;
    BOOL fgPremultDst;
	BOOL fgPremultDstWr;
    VOID*      pPalette;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_ALPHA_COMPOSITION_IMPROVE_T;


/** The structure contains alpha composition operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    UINT32 u4DstWidth;
    UINT32 u4DstHeight;
    UINT8           u1Alpha;
    MTGFX_PD_RULE_T   ePDRule;
    BOOL            bRectSrcOption;
    UINT32          u4Color;
    // After MT5395, we support pre-multiply
    BOOL fgPremultSrc;
    BOOL fgPremultDst;
    BOOL fgPremultDstWr;
	MTGFX_MMU_SET_T 	  t_mmu;

} MTGFX_STRETCH_ALPHA_COMPOSITION_T;

/** The structure contains alpha composition operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    MTGFX_BYTE_ALIGNED_T  eByteAligned;
    VOID*      pPalette;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_PLTBLT_T;

/** The structure contains rotation operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    MTGFX_ROTATE_FLAG_T eDirection;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_ROTATE_T;
/** The structure contains GFX compress bitblt operation setting information
*/
typedef struct 
{
    UINT32 u4Src;
    UINT32 u4Dst;
    UINT32 eCm; 
    UINT32 u4Pitch;
    UINT32 u4SrcX;
    UINT32 u4SrcY;
    UINT32 u4Width;    
    UINT32 u4Height;        
    UINT32 u4DstX;       
    UINT32 u4DstY;  
	MTGFX_MMU_SET_T       t_mmu;
}MTGFX_COMPRESS_T;


/** The structure contains scaling operation setting information
*/
typedef struct
{
    MTGFX_COMMON_BITBLT_FIELDS

    UINT32 u4DstWidth;
    UINT32 u4DstHeight;
	MTGFX_MMU_SET_T       t_mmu;
} MTGFX_SCALER_T;

typedef enum _GFX_3D_MODE_T
{
    GFX_3D_OSD_SBS = 0,
    GFX_3D_OSD_TB    = 1,
    GFX_3D_OSD_NOTMAL    = 2
} GFX_3D_MODE_T;


typedef struct _GFX_3D_DATA_T
{
    MTGFX_COMMON_BITBLT_FIELDS

    GFX_3D_MODE_T   e_mode;
    UINT32              ui4_bmp_width;
    UINT32              ui4_bmp_height;
} GFX_3D_DATA_T;

/** The structure contains VGX color mode information
*/
typedef enum 
{
    MT_VGX_CM_YCBCR_CLUT2,
    MT_VGX_CM_YCBCR_CLUT4,
    MT_VGX_CM_YCBCR_CLUT8,
    MT_VGX_CM_RESERVED_0,
    MT_VGX_CM_CBYCRY422_DIRECT16,
    MT_VGX_CM_YCBYCR422_DIRECT16,
    MT_VGX_CM_AYCBCR8888_DIRECT32,
    MT_VGX_CM_RESERVED_1,
    MT_VGX_CM_RGB_CLUT2,
    MT_VGX_CM_RGB_CLUT4,
    MT_VGX_CM_RGB_CLUT8,
    MT_VGX_CM_RGB565_DIRECT16,
    MT_VGX_CM_ARGB1555_DIRECT16,
    MT_VGX_CM_ARGB4444_DIRECT16,
    MT_VGX_CM_ARGB8888_DIRECT32,
    MT_VGX_CM_ALPHA_BIT
} MT_VGX_COLOR_MODE_T;
/** The structure contains buffer information
*/
typedef struct
{
    UINT32 u4Addr;
    UINT32 u4Size;
    UINT32 u4PhyAddr;
    UINT32 u4VirtAddr;
}MTGFX_GETBUF_T;
/** This structure used for get a buffer
*/
typedef struct
{
    UINT32 u4Addr;
    UINT32 u4Size;
}MTGFX_BUFPROP_T;
/** The structure contains RGB format converte to YCBCR format information
*/
typedef struct
{
     VOID   *pvSrcAddr;
     VOID   *pvPaletteTable;
     UINT32 u4Width;
     UINT32 u4Height;
     UINT32 u4SrcX;
     UINT32 u4SrcY;
     UINT32 u4UpdateWidth;
     UINT32 u4UpdateHeight;
     UINT8  eSrcCm;
	 MTGFX_MMU_SET_T       t_mmu;
} MTGFX_RGB_TO_YCBCR_T;
/** used for scaling rotation operation,to get pallete table information
*/
typedef struct
{
	UINT32 u4PalleteTable[256];
}MTGFX_PALLETE_TABLE_T;

/** used for scaling rotation operation,to get rgb data buffer
*/
typedef struct
{
	UINT32 DataBuffer[720];
}MTGFX_DATA_BUFFER_T;

/** used for DMA mapping
*/
typedef enum
{
    MTIOMMU_DMA_BIDIRECTION = 0,
    MTIOMMU_DMA_TO_DEVICE   = 1,
    MTIOMMU_DMA_FROM_DEVICE = 2
} MTIOMMU_DMA_DIRECTION_T;

/**The structure contains io mmu DMA mapping informaion
*/
typedef struct
{
    UINT32 start;
    UINT32 size;
    MTIOMMU_DMA_DIRECTION_T direction;
    UINT32 dma_addr;
    UINT32 pageArray;
} MTIOMMU_DMA_RANGE_T;

/**Used for mapping user space address to physical memory
*/
typedef struct
{
    UINT32 start;
    UINT32 size;
    UINT32 pageArray;
    UINT32 nrPages;
} MTIOMMU_PIN_RANGE_T;
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


/**Get OSD buffer
*@param prGetBuf                   result of MTGFX_GetBuffer
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_GetBuffer(MTGFX_GETBUF_T* prGetBuf);
EXTERN MT_RESULT_T MTGFX_Release(void);

/**Get OSD bufferPartition
*@param prGetBuf                   result of MTGFX_GetBufferPartition
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_GetBufferPartition(MTGFX_GETBUF_T* prGetBuf);
EXTERN MT_RESULT_T MTGFX_PartitionRelease(void);

/**Reset and Init GFX module.
*@param VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_Reset (VOID);

/**Wait GFX module to do operation
*@param i4Timeout		     Specify timeout inverval,timeout unit is ms, set -1 as forever waiting
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*@retval MTR_ERR_TIMEOUT     Timeout
*/
EXTERN MT_RESULT_T MTGFX_Wait (INT32 i4Timeout);

/**Force GFX module to do operation
*@param                                VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_Flush (VOID);

/**Set GFX lock,and do io mmu setting
*@param pt_mmu           pointer of MTGFX_MMU_SET_T structure
*@retval MTR_OK          Success.
*@retval MTR_NOT_OK      Fail.
*/
EXTERN MT_RESULT_T MTGFX_Lock (const MTGFX_MMU_SET_T *pt_mmu);
EXTERN MT_RESULT_T MTGFX_Unlock (void);

/**Force GFX set mmu enable/disable
*@param pt_mmu           pointer of MTGFX_MMU_SET_T structure
*@retval MTR_OK          Success.
*@retval MTR_NOT_OK      Fail.
*/
EXTERN MT_RESULT_T MTGFX_Set_Mmu_Enable (const MTGFX_MMU_SET_T *pt_mmu);

/**Perform GFX rectangle fill operation
*@param  prFill                 pointer of GFX rectangle operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_FillRect(const MTGFX_FILL_T *prFill);

/**Perform GFX draw horizontal line operation
*@param  prFill                 pointer of GFX draw line operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_DrawHLine(const MTGFX_FILL_T *prFill);

/**Perform GFX draw vertical line operation
*@param  prFill                 GFX draw line operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_DrawVLine(const MTGFX_FILL_T *prFill);

/**Perform GFX normal bitblt operation
*@param  prBitblt               GFX bitblt operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_Bitblt(const MTGFX_BITBLT_T *prBitblt);

/**Perform GFX transparent bitblt operation
*@param  prBitblt               GFX transparent bitblt operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_TransparentBitblt(const MTGFX_TRANSPARENT_BITBLT_T *prBitblt);

/**Perform transparent fill operation
*@param  prBitblt               GFX transparent fill operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_TransparentFill(const MTGFX_TRANSPARENT_FILL_T *prFill);

/**Perform GFX alpha blending operation
*@param  prBitblt               GFX alhpa blending operation structure
*@retval MTR_OK                 Success.
*@retval MTR_NOT_OK             Fail.
*/
EXTERN MT_RESULT_T MTGFX_AlphaBlending(const MTGFX_ALPHA_BLEND_T *prBitblt);
/**Perform YCBCR to other format conversion operation,can translate YCbCr block 2buffer to ARGB8888,YCbCr raster 2buffer,
*YCbCr raster 3buffer,this function will be called by user,and it will call MTGFX_YcbcrFormatConversion()
*@param  prConversion             color format conversion operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_YcbcrFormatConversion(const MTGFX_YCBCR_COLOR_FORMAT_CONVERSION_T *prConversion);

/**Perform color format conversion operation,can translate YCbCr block 2buffer to ARGB8888,YCbCr raster 2buffer,
*YCbCr raster 3buffer,RGB565,ARGB4444,this function do the actual work of color format conversion,
*@param  prConversion             color format conversion operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_ColorFormatConversion(const MTGFX_YCBCR_FORMAT_CONVERSION_T *prConversion);

/**Perform GFX YCbCr to RGB operation,can do Ycbcr block 2buffer to ARGB8888,RGB565,ARGB4444,can support 
*earier board.
*@param  prYbr2Rgb                GFX YCbCR to RGB operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_Ybr2Rgb(const MTGFX_YCBCR_TO_RGB_T *prYbr2Rgb);

/**Perform GFX alpha map operation
*@param  prAlphaMap                GFX alpha map operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_AlphaMapBitblt(const MTGFX_ALPHAMAP_BITBLT_T *prAlphaMap);

/**Perform GFX Rop bitblt operation
*@param  prRop                     GFX Rop bitblt operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_RopBitblt(const MTGFX_ROP_BITBLT_T *prRop);

/**Perform GFX alpha composition operation
*@param  prRop                     GFX alpha composition operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_AlphaComposition(const MTGFX_ALPHA_COMPOSITION_T *prAcomp);

/**Perform GFX alpha composition operation
*@param  prAcomp                     GFX alpha composition operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_AlphaCompositionImprove(const MTGFX_ALPHA_COMPOSITION_IMPROVE_T *prAcomp);

/**Perform GFX index to direct color mode operation
*@param  prRop                     GFX index to direct color mode operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_Idx2DirBitblt(const MTGFX_PLTBLT_T *prPale);

/**Perform GFX rotation operation, the coordinate of up-left point, src(x, y), of the 
*source image will go to dst(x, y) after rotation
*@param  prRotate                  GFX rotation operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_Rotate90(const MTGFX_ROTATE_T *prRotate);

/**Perform GFX scaling operation
*@param  prScale                   GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_Scaler(const MTGFX_SCALER_T *prScaler);

/**Perform GFX stretch blit operation
*@param prScaler                   alpha composition operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_ScalerComposition(const MTGFX_STRETCH_ALPHA_COMPOSITION_T *prScaler);

/**Perform VGFX scaling operation
*@param  prScale                   VGFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTVGFX_Scaler(const MTVGFX_PARAM_SET_T *prScaler);

/**Perform GFX scaling operation
*@param  prRGB2Ybr                 GFX RGB 2 YCbCr operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_RGB2Ybr(const MTGFX_RGB_TO_YCBCR_T *prRGB2Ybr);

/**Perform GFX stretch blit operation
*@param  prScale                  GFX scaling operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_StretchBitblt(const MTGFX_SCALER_T *prScaler);

/**Perform GFX flush dcache operation
*@param  prScale                  GFX Dcache operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
EXTERN MT_RESULT_T MTGFX_Flush_Dcache(const MTGFX_CACHE_DATA *pt_this);

/**Perform GFX stretch alpha composition operation
*@param  prScale                  GFX scaling operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_StretchAlphaComposition(const MTGFX_STRETCH_ALPHA_COMPOSITION_T *prScaler);

/**Perform GFX compress bitblt operation
*@param prCompressBlit            GFX compress bitblt operation setting
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_CompressBlt(const MTGFX_COMPRESS_T *prCompressBlit);

/**Perform GFX get pageset physical address operation
*@param uaddr                     keep the physical address
*@retval MTR_OK                   Success
*/
MT_RESULT_T MTMMU_GetPGD_Addr(unsigned long* uaddr);

/**Perform GFX map user space address to physical address
*@param prPinRange                user space address
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
MT_RESULT_T MTMMU_Pin_UserMem(const MTIOMMU_PIN_RANGE_T *prPinRange);
MT_RESULT_T MTMMU_UnPin_UserMem(const MTIOMMU_PIN_RANGE_T *prPinRange);

/**Perform GFX map dma address
*@param prDMARange                dma range
*@param prPinRange                map range
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail
*/
MT_RESULT_T MTMMU_DMA_MapSingle(const MTIOMMU_DMA_RANGE_T* prDMARange, const MTIOMMU_PIN_RANGE_T* prPinRange);
MT_RESULT_T MTMMU_DMA_UnMapSingle(const MTIOMMU_DMA_RANGE_T* prDMARange, const MTIOMMU_PIN_RANGE_T* prPinRange);

/**Perform GFX YCbCr to RGB operation,this function call MTGFX_Ybr2Rgb()function to translate color format
*@param  prYbr2Rgb                GFX YCbCR to RGB operation structure
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_Ybr420MB2Rgb(const MTGFX_YCBCR_420MB_TO_RGB_T *prYbr2Rgb8888);

/**Perform GFX Set flip and mirror operation
*@param fgMirrorEn                TRUE:Enable mirror,FALSE:Disable mirror
*@param fgFlipEn                  TRUE:Enable flip,FALSE:Disable flip
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_SetFlipMirror(BOOL fgMirrorEn, BOOL fgFlipEn);

/**Perform GFX get OSD buffer operation
*@param prGetBuf                  keep the address.
*@retval MTR_OK                   Success.
*@retval MTR_NOT_OK               Fail.
*/
EXTERN MT_RESULT_T MTGFX_GetFbmBuffer(MTGFX_GETBUF_T* prGetBuf);
EXTERN MT_RESULT_T MTGFX_Fbm_Buffer_Release(void);

EXTERN MT_RESULT_T MTGFX_Set3DSbs(const GFX_3D_DATA_T *pr3dData);
EXTERN MT_RESULT_T MTGFX_VDOResize(MTGFX_IMG_REGION_T *prSrc, MTGFX_IMG_REGION_T *prDst, UINT32 u4CM);

#ifdef __cplusplus
}
#endif

#endif //MT_GFX_H
