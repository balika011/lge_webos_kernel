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
 * $RCSfile: io_mtgfx.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtgfx.c
 *  This header file declares exported APIs of GFX module.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtgfx.h"
#include "gfx_if.h"
#include "gfx_drvif.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "fbm_drvif.h"
#include <linux/mtal_ioctl.h>
#include <linux/dma-mapping.h>
#include "drvcust_if.h"
#include "x_hal_arm.h"
#include "osd_drvif.h"

#include <linux/version.h>

#ifdef CC_SUPPORT_TVE_OSD_OUT
#include "tve_ctrl.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
extern void GFX_SetCharCM(BOOL flag,UINT32 u4SrcCM);

extern void GFX_ComposeLoopImprove(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX, 
                                                UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar, 
                                                    UINT32 u4OpCode, UINT32 u4RectSrc);

static MT_RESULT_T _MTGFX_GFX_MMU_Set_Enable(unsigned long arg);

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define MTGFX_FILL_T_PRINT(func,p) \
    do{\
        if(IS_MTGFX_LOG_ENABLE)\
        {\
        printf(" - %s.(INT32)pvDst = %d,",  func,(INT32)((p)->pvDst));\
        printf("%s.i4DstX = %d,",       func,(p)->i4DstX);\
        printf("%s.i4DstY = %d,",       func,(p)->i4DstY);\
        printf("%s.u4DstPitch = %d,",   func,(p)->u4DstPitch);\
        printf("%s.eDstCm = %d,",       func,(p)->eDstCm);\
        printf("%s.u4Width = %d,",      func,(p)->u4Width);\
        printf("%s.u4Height = %d,",     func,(p)->u4Height);\
        printf("%s.u4Color = %d\n",     func,(p)->u4Color);\
        }\
    }while(0)


#define COMMON_BITBLT_FIELDS_PRINT(func,p)\
    do{\
        if(IS_MTGFX_LOG_ENABLE)\
        {\
        printf(" - %s.(INT32)pvSrc = %d,",  func,(INT32)((p)->pvSrc));\
        printf("%s.i4SrcX = %d,",       func,(p)->i4SrcX);\
        printf("%s.i4SrcY = %d,",       func,(p)->i4SrcY);\
        printf("%s.u4SrcPitch = %d,",   func,(p)->u4SrcPitch);\
        printf("%s.eSrcCm = %d,",       func,(p)->eSrcCm);\
        printf("%s.(INT32)pvDst = %d,", func,(INT32)((p)->pvDst));\
        printf("%s.i4DstX = %d,",       func,(p)->i4DstX);\
        printf("%s.i4DstY = %d\n",      func,(p)->i4DstY);\
        printf("%s.u4DstPitch = %d,",   func,(p)->u4DstPitch);\
        printf("%s.eDstCm = %d,",       func,(p)->eDstCm);\
        printf("%s.u4Width = %d,",      func,(p)->u4Width);\
        printf("%s.u4Height = %d",      func,(p)->u4Height);\
        }\
    }while(0)
    
#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("COPY_TO_USER_ARG mtal_ioctl_gfx argument error\n");      \
        return MTR_ERR_INV;                                          \
    } 
 
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)             \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))                        \
    {                                                       \
        printk("COPY_FROM_USER_ARG mtal_ioctl_gfx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \
    
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))                            \
    {                                                       \
        printk("USR_SPACE_ACCESS_VALIDATE_ARG mtal_ioctl_gfx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("USR_SPACE_ACCESS_VALIDATE_1ARG mtal_ioctl_gfx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("COPY_FROM_USER_1ARG mtal_ioctl_gfx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("USR_SPACE_ACCESS_VALIDATE_2ARG mtal_ioctl_gfx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("COPY_FROM_USER_2ARG mtal_ioctl_gfx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))                            \
    {                                                       \
        printk("USR_SPACE_ACCESS_VALIDATE_3ARG mtal_ioctl_gfx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))                        \
    {                                                       \
        printk("COPY_FROM_USER_3ARG mtal_ioctl_gfx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define NULL_PT_CHECK(pt)                   \
    {                                       \
        if ((void*)(pt) == NULL)            \
        {                                   \
            return MTR_NOT_OK;      \
        }                                   \
    }

#define XYWH_CHECK(VX,VY,VW,VH)             \
    {                                       \
        if (((INT32)(VX) < 0)     ||        \
            ((INT32)(VY) < 0)     ||        \
            ((INT32)(VW) <= 0)    ||        \
            ((INT32)(VH) <= 0))             \
        {                                   \
            return MTR_NOT_OK;      \
        }                                   \
    }

#define COLOR_CHECK(color)                  \
    {                                       \
        if (((INT32)(color) != (INT32)(MTGFX_CM_AYCBCR8888_DIRECT32))    &&        \
            ((INT32)(color) != (INT32)(MTGFX_CM_RGB565_DIRECT16))        &&        \
            ((INT32)(color) != (INT32)(MTGFX_CM_ARGB1555_DIRECT16))      &&        \
            ((INT32)(color) != (INT32)(MTGFX_CM_ARGB4444_DIRECT16))      &&        \
            ((INT32)(color) != (INT32)(MTGFX_CM_ARGB8888_DIRECT32)))               \
        {                                   \
            return MTR_NOT_OK;      \
        }                                   \
    }

// Turn on each gfx operation internal flush (for debug use)
#if defined(GFX_DBG_EACH_OP_FLUSH)
    #define GFX_DBG_FLUSH()      GFX_Flush()
#else
    #define GFX_DBG_FLUSH()
#endif

//-------------------------------------------------------------------------
/** _GfxGetRopCode
 *  Get gfx driver ROP code
 *
 */
//-------------------------------------------------------------------------
static UINT32 _GfxGetRopCode(MTGFX_ROP_TYPE_T eMwRopType)
{
    switch (eMwRopType)
    {
    case MTGFX_ROP_NOT_SRC:
        return (UINT32)E_ROP_NOT_SRC;
        
    case MTGFX_ROP_NOT_DST:
        return (UINT32)E_ROP_NOT_DST;
        
    case MTGFX_ROP_SRC_XOR_DST:
        return (UINT32)E_ROP_SRC_XOR_DST;
        
    case MTGFX_ROP_SRC_XNOR_DST:
        return (UINT32)E_ROP_SRC_XNOR_DST;
        
    case MTGFX_ROP_SRC_AND_DST:
        return (UINT32)E_ROP_SRC_AND_DST;
        
    case MTGFX_ROP_NOT_SRC_AND_DST:
        return (UINT32)E_ROP_NOT_SRC_AND_DST;
        
    case MTGFX_ROP_SRC_AND_NOT_DST:
        return (UINT32)E_ROP_SRC_AND_NOT_DST;
    
    case MTGFX_ROP_NOT_SRC_AND_NOT_DST:
        return (UINT32)E_ROP_NOT_SRC_AND_NOT_DST;
        
    case MTGFX_ROP_SRC_OR_DST:
        return (UINT32)E_ROP_SRC_OR_DST;
        
    case MTGFX_ROP_NOT_SRC_OR_DST:
        return (UINT32)E_ROP_NOT_SRC_OR_DST;
        
    case MTGFX_ROP_SRC_OR_NOT_DST:
        return (UINT32)E_ROP_SRC_OR_NOT_DST;
        
    case MTGFX_ROP_NOT_SRC_OR_NOT_DST:
        return (UINT32)E_ROP_NOT_SRC_OR_NOT_DST;
     
    case MTGFX_ROP_MULTI:
        return (UINT32)E_ROP_MULTI;
    default:
        return (UINT32)E_ROP_MAX;
    }
}

//-------------------------------------------------------------------------
/** _GfxGetFontMode
 *  Get gfx driver font mode
 *
 */
//-------------------------------------------------------------------------
static UINT32 _GfxGetFontMode(MTGFX_COLORMODE_T rMwColorMode)
{
    switch (rMwColorMode)
    {
    case MTGFX_CM_YCBCR_CLUT2:
    case MTGFX_CM_RGB_CLUT2:
        return (UINT32)E_BMP_CM_2BIT;
        
    case MTGFX_CM_YCBCR_CLUT4:
    case MTGFX_CM_RGB_CLUT4:
        return (UINT32)E_BMP_CM_4BIT;
        
    case MTGFx_CM_YCBCR_CLUT8:
    case MTGFX_CM_RGB_CLUT8:
        return (UINT32)E_BMP_CM_8BIT;

    default:
        return (UINT32)E_BMP_CM_1BIT;
    }
}
static UINT32 _GfxGetSrcThirdMode(MTGFX_COLORMODE_T rMwColorMode)
{
    switch (rMwColorMode)
    {
    case MTGFX_CM_YCBCR_CLUT2:
    case MTGFX_CM_RGB_CLUT2:
        return (UINT32)E_BMP_CM_2BIT;
        
    case MTGFX_CM_YCBCR_CLUT4:
    case MTGFX_CM_RGB_CLUT4:
        return (UINT32)E_BMP_CM_4BIT;
        
    case MTGFx_CM_YCBCR_CLUT8:
    case MTGFX_CM_RGB_CLUT8:
        return (UINT32)E_BMP_CM_8BIT;

    case MTGFX_CM_RGB565_DIRECT16:
    case MTGFX_CM_ARGB1555_DIRECT16:
    case MTGFX_CM_ARGB4444_DIRECT16:
    case MTGFX_CM_ARGB8888_DIRECT32:
    case MTGFX_CM_CBYCRY422_DIRECT16:
    case MTGFX_CM_YCBYCR422_DIRECT16:
    case MTGFX_CM_AYCBCR8888_DIRECT32:
        return (UINT32)E_BMP_CM_DIRECT;
    default:
        return (UINT32)E_BMP_CM_1BIT;
    }
}



//-------------------------------------------------------------------------
/** _GfxGetYcFormat
 *  Get gfx driver YC format
 *
 */
//-------------------------------------------------------------------------
static UINT32 _GfxGetYcFormat(MTGFX_YCBCR_FORMAT_T eMwYbrFormat)
{
    IC_VERSION_T eVer;
    eVer = BSP_GetIcVersion();

    switch (eMwYbrFormat)
    {
    case MTGFX_YCBCR_420_MB :
        return (UINT32)E_YCFMT_420MB;

    case MTGFX_YCBCR_420_LNR :
        return (UINT32)E_YCFMT_420LINEAR;

    case MTGFX_YCBCR_422_LNR  :
        return (UINT32)E_YCFMT_422LINEAR;
        
    default:
        return (UINT32)E_YCFMT_RES_2;
    }
    
}

//-------------------------------------------------------------------------
/** _GfxOpModeCheck
 *  Check graphics operation mode
 *
 */
//-------------------------------------------------------------------------
    
static INT32 _GfxOpModeCheck(UINT32 u4Op, UINT32 u4Src, UINT32 u4Dst)
{
    // rule:0 - index to direct color mode
    if (u4Op == (UINT32)OP_IDX_TO_DIR_BITBLT)
    {
        if (((u4Src == (UINT32)MTGFX_CM_YCBCR_CLUT2) ||
             (u4Src == (UINT32)MTGFX_CM_YCBCR_CLUT4) ||
             (u4Src == (UINT32)MTGFx_CM_YCBCR_CLUT8) ||
             (u4Src == (UINT32)MTGFX_CM_RGB_CLUT2)   ||
             (u4Src == (UINT32)MTGFX_CM_RGB_CLUT4)   ||
             (u4Src == (UINT32)MTGFX_CM_RGB_CLUT8))     &&
            ((u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)      ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)    ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)    ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)    ||
             (u4Dst == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32)
             ))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }

    // rule:1 - do NOT support clut2 and clut4      (SRC)
    if ((u4Src == (UINT32)MTGFX_CM_YCBCR_CLUT2)   ||
        (u4Src == (UINT32)MTGFX_CM_YCBCR_CLUT4)   ||
        (u4Src == (UINT32)MTGFX_CM_RGB_CLUT2)     ||
        (u4Src == (UINT32)MTGFX_CM_RGB_CLUT4))
    {
        return (INT32)MTR_NOT_OK;
    }

    // rule:1 - do NOT support clut2 and clut4      (DST)
    if ((u4Dst == (UINT32)MTGFX_CM_YCBCR_CLUT2)   ||
        (u4Dst == (UINT32)MTGFX_CM_YCBCR_CLUT4)   ||
        (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT2)     ||
        (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT4))
    {
        return (INT32)MTR_NOT_OK;
    }
    
    // rule:2 - do NOT support YCbYCr or CbYCrY mode    (SRC, DST)
    if ((u4Src == (UINT32)MTGFX_CM_CBYCRY422_DIRECT16)  ||
        (u4Src == (UINT32)MTGFX_CM_YCBYCR422_DIRECT16)  ||
        (u4Dst == (UINT32)MTGFX_CM_CBYCRY422_DIRECT16)  ||
        (u4Dst == (UINT32)MTGFX_CM_YCBYCR422_DIRECT16))
    {
        return (INT32)MTR_NOT_OK;
    }

    // rule:3 - normal bitblt modes
    if (u4Op == (UINT32)OP_BITBLT)
    {
        if (((u4Src == (UINT32)MTGFX_CM_RGB565_DIRECT16)     ||
             (u4Src == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)   ||
             (u4Src == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)   ||
             (u4Src == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)   ||
             (u4Src == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32))  &&
            ((u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)     ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)   ||
             (u4Src == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32)))
        {
            return (INT32)MTR_OK;
        }
        
        if ((u4Src == u4Dst)    && 
            ((u4Dst == (UINT32)MTGFx_CM_YCBCR_CLUT8) || 
            (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT8)))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;

    }

    // rule:4 - the mode without source
    if ((u4Op == (UINT32)OP_RECT_FILL)  ||
        (u4Op == (UINT32)OP_DRAW_HLINE) ||
        (u4Op == (UINT32)OP_DRAW_VLINE) ||
        (u4Op == (UINT32)OP_GRAD_FILL))
    {
        if ((u4Dst == (UINT32)MTGFx_CM_YCBCR_CLUT8) ||
            (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT8)   ||
            (u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)     ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)   ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)   ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)   ||
            (u4Dst == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32))
        {
            if ((u4Op == (UINT32)OP_GRAD_FILL) &&
                ((u4Dst == (UINT32)MTGFx_CM_YCBCR_CLUT8) || 
                 (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT8)))
            {
                return (INT32)MTR_NOT_OK;
            }

            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }

    // rule:5 - dma mode
    if (u4Op == (UINT32)OP_DMA)
    {
        if ((u4Dst == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32)   || 
            (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }

    // rule:6 - ybr2rgb mode
    if (u4Op == (UINT32)OP_YCBCR_TO_RGB)
    {
        if (((u4Src == (UINT32)MTGFx_CM_YCBCR_CLUT8) ||
             (u4Src == (UINT32)MTGFX_CM_RGB_CLUT8))    &&
            ((u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)     ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }
    
    // rule:7 - alphamap mode
    if (u4Op == (UINT32)OP_ALPHA_MAP_BITBLT)
    {
        if (((u4Src == (UINT32)MTGFx_CM_YCBCR_CLUT8) ||
             (u4Src == (UINT32)MTGFX_CM_RGB_CLUT8))    &&
            ((u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)   ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }
    
    // rule:8 - all other mode
    if (u4Src == u4Dst)
    {
        if ((u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)   ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16) ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16) ||
            (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32) ||
            (u4Dst == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32))
        {
            return (INT32)MTR_OK;
        }

        if ((u4Dst == (UINT32)MTGFx_CM_YCBCR_CLUT8) ||
            (u4Dst == (UINT32)MTGFX_CM_RGB_CLUT8))
        {
            if ((u4Op == (UINT32)OP_STRETCH_BITBLT)     ||
                (u4Op == (UINT32)OP_BITBLT)             ||
                (u4Op == (UINT32)OP_LINE_HORI_TO_VERT)  ||
                (u4Op == (UINT32)OP_ROP_BITBLT)         //||
                /*(u4Op == (UINT32)OP_SC_SCALING)*/)
            {
                return (INT32)MTR_OK;
            }
        }
    }
    if(u4Op == OP_ALPHA_COMPOSE_LOOP)
    {
      if ((u4Src == (UINT32)MTGFX_CM_RGB_CLUT8) &&
            ((u4Dst == (UINT32)MTGFX_CM_RGB565_DIRECT16)      ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB1555_DIRECT16)    ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB4444_DIRECT16)    ||
             (u4Dst == (UINT32)MTGFX_CM_ARGB8888_DIRECT32)    ||
             (u4Dst == (UINT32)MTGFX_CM_AYCBCR8888_DIRECT32)
             ))
        {
            return (INT32)MTR_OK;
        }

        return (INT32)MTR_NOT_OK;
    }
    return (INT32)MTR_NOT_OK;
}

//-------------------------------------------------------------------------
/** _GfxGetColorMode
 *  Get gfx driver color mode
 */
//-------------------------------------------------------------------------
static UINT32 _GfxGetColorMode(MTGFX_COLORMODE_T rMwColorMode)
{
    switch (rMwColorMode)
    {
    case MTGFx_CM_YCBCR_CLUT8:
        return (UINT32)CM_YCbCr_CLUT8;
        
    case MTGFX_CM_CBYCRY422_DIRECT16:
        return (UINT32)CM_CbYCrY422_DIRECT16;
        
    case MTGFX_CM_YCBYCR422_DIRECT16:
        return (UINT32)CM_YCbYCr422_DIRECT16;
        
    case MTGFX_CM_AYCBCR8888_DIRECT32:
        return (UINT32)CM_ARGB8888_DIRECT32;    // workaround
        
    case MTGFX_CM_RGB_CLUT8:
        return (UINT32)CM_RGB_CLUT8;
        
    case MTGFX_CM_RGB565_DIRECT16:
        return (UINT32)CM_RGB565_DIRECT16;
        
    case MTGFX_CM_ARGB1555_DIRECT16:
        return (UINT32)CM_ARGB1555_DIRECT16;
        
    case MTGFX_CM_ARGB4444_DIRECT16:
        return (UINT32)CM_ARGB4444_DIRECT16;
        
    case MTGFX_CM_ARGB8888_DIRECT32:
        return (UINT32)CM_ARGB8888_DIRECT32;

    default:
        return (UINT32)MTGFX_CM_ARGB8888_DIRECT32;
    }
}

//-------------------------------------------------------------------------
/** _GfxGetIMGRZColor
 *  Get imgrz driver color mode
 */
//-------------------------------------------------------------------------
static UINT32 _GfxGetIMGRZColor(MTGFX_COLORMODE_T rMwColorMode)
{
    switch (rMwColorMode)
    {        
    case MTGFX_CM_AYCBCR8888_DIRECT32:
        return (UINT32)E_RZ_OSD_DIR_CM_AYCbCr8888;    // workaround

    case MTGFX_CM_RGB565_DIRECT16:
        return (UINT32)E_RZ_OSD_DIR_CM_RGB565;
        
    case MTGFX_CM_ARGB1555_DIRECT16:
        return (UINT32)E_RZ_OSD_DIR_CM_ARGB1555;
        
    case MTGFX_CM_ARGB4444_DIRECT16:
        return (UINT32)E_RZ_OSD_DIR_CM_ARGB4444;
        
    case MTGFX_CM_ARGB8888_DIRECT32:
        return (UINT32)E_RZ_OSD_DIR_CM_ARGB8888;

    default:
        return (UINT32)E_RZ_OSD_DIR_CM_AYCbCr8888;
    }
}

/**Reset and Init GFX module.
*@param VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTGFX_Reset(void)
{
    MTGFX_PRINT(" - void\n");

    //add driver implementation code here
    GFX_Init();
    return MTR_OK;
}
/**Wait GFX module to do operation
*@param i4Timeout            Specify timeout inverval,timeout unit is ms, set -1 as forever waiting
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*@retval MTR_ERR_TIMEOUT     Timeout
*/
static MT_RESULT_T _MTGFX_Wait(unsigned long arg)
{       
    INT32 i4Timeout;
    INT32 rArg;
    INT32* prArg;

    prArg = (INT32*)arg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(prArg);
    COPY_FROM_USER_1ARG(prArg, rArg);

    i4Timeout = rArg;

    MTGFX_PRINT(" - kernel mode i4Timeout = %d\n",i4Timeout);

    //add driver implementation code here
    GFX_Lock();
    GFX_Wait(); 
    GFX_Unlock();    
    return MTR_OK;
}
/**Force GFX module to do operation
*@param                                VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Flush(void)
{
    MTGFX_PRINT(" - void\n");

    //add driver implementation code here
    GFX_Lock();
	
	//#ifdef CC_MT5882
	HalFlushInvalidateDCache();
	//#endif
	
    GFX_Flush();
    GFX_Unlock();
    return MTR_OK;
}

/**Force GFX module to do operation
*@param                                VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Lock(unsigned long arg)
{
    MT_RESULT_T     r_set = MTR_OK;
    
    GFX_Lock();
    
    if(arg)
    {
        r_set = _MTGFX_GFX_MMU_Set_Enable(arg);
    }
    
    return r_set;
}

/**Force GFX module to do operation
*@param                                VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_UnLock(void)
{
    GFX_Unlock();
    return MTR_OK;
}


/**Perform GFX rectangle fill operation
*@param  prFill                       GFX rectangle operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_FillRect(unsigned long arg)
{
    MTGFX_FILL_T rFill;
    MTGFX_FILL_T* prFill;
    UINT32 ux, uy;
    
    prFill = (MTGFX_FILL_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prFill,MTGFX_FILL_T);
    COPY_FROM_USER_ARG(prFill,rFill,MTGFX_FILL_T);

    MTGFX_FILL_T_PRINT("rFill",&rFill);

    prFill = &rFill;
    //add driver implementation code here

    NULL_PT_CHECK(prFill);
    XYWH_CHECK(prFill->i4DstX, prFill->i4DstY, prFill->u4Width, 
        prFill->u4Height);

    if (_GfxOpModeCheck((UINT32)OP_RECT_FILL, 
            (UINT32)prFill->eDstCm, (UINT32)prFill->eDstCm))
    {
        return MTR_NOT_OK;
    }

    ux = (UINT32)prFill->i4DstX;
    uy = (UINT32)prFill->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prFill->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prFill->t_mmu);
    }
    
    GFX_SetDst((UINT8 *)prFill->pvDst, 
        _GfxGetColorMode(prFill->eDstCm), prFill->u4DstPitch);
        
    GFX_SetColor(prFill->u4Color);

    GFX_Fill(ux, uy, prFill->u4Width, prFill->u4Height);

    GFX_DBG_FLUSH();
    
    GFX_Unlock();
    return MTR_OK;
}

/**Perform GFX draw horizontal line operation
*@param  prFill                       GFX draw line operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_DrawHLine(unsigned long arg)
{
    MTGFX_FILL_T rFill;
    MTGFX_FILL_T* prFill;
    UINT32 ux, uy;
    prFill = (MTGFX_FILL_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prFill,MTGFX_FILL_T);
    COPY_FROM_USER_ARG(prFill,rFill,MTGFX_FILL_T);

    MTGFX_FILL_T_PRINT("rFill",&rFill);

    //add driver implementation code here

    prFill = &rFill;
    
    NULL_PT_CHECK(prFill);
    XYWH_CHECK(prFill->i4DstX, prFill->i4DstY, prFill->u4Width, 
        prFill->u4Height);
    if(_GfxOpModeCheck((UINT32)OP_DRAW_HLINE, 
            (UINT32)prFill->eDstCm, (UINT32)prFill->eDstCm))
    {
        return MTR_NOT_OK;
    }

    ux = (UINT32)prFill->i4DstX;
    uy = (UINT32)prFill->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prFill->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prFill->t_mmu);
    }

    GFX_SetDst((UINT8 *)prFill->pvDst, 
        _GfxGetColorMode(prFill->eDstCm), prFill->u4DstPitch);

    GFX_SetColor(prFill->u4Color);

    GFX_HLine(ux, uy, prFill->u4Width);

    GFX_DBG_FLUSH();
    
    GFX_Unlock();
    return MTR_OK;
}
/**Perform GFX draw vertical line operation
*@param  prFill                       GFX draw line operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_DrawVLine(unsigned long arg)
{
    MTGFX_FILL_T rFill;
    MTGFX_FILL_T* prFill;
    UINT32 ux, uy;
    prFill = (MTGFX_FILL_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prFill,MTGFX_FILL_T);
    COPY_FROM_USER_ARG(prFill,rFill,MTGFX_FILL_T);

    MTGFX_FILL_T_PRINT("rFill",&rFill);

    //add driver implementation code here
    prFill = &rFill;
    NULL_PT_CHECK(prFill);
    XYWH_CHECK(prFill->i4DstX, prFill->i4DstY, 
        prFill->u4Width, prFill->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_DRAW_VLINE, 
            (UINT32)prFill->eDstCm, (UINT32)prFill->eDstCm))
    {
        return MTR_NOT_OK;
    }

    ux = (UINT32)prFill->i4DstX;
    uy = (UINT32)prFill->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prFill->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prFill->t_mmu);
    }

    GFX_SetDst((UINT8 *)prFill->pvDst, 
        _GfxGetColorMode(prFill->eDstCm), prFill->u4DstPitch);

    GFX_SetColor(prFill->u4Color);

    GFX_VLine(ux, uy, prFill->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}

/**Perform GFX normal bitblt operation
*@param  prBitblt                    GFX bitblt operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Bitblt(unsigned long arg)
{
    MTGFX_BITBLT_T rFill;
    MTGFX_BITBLT_T* prBitblt;
    UINT32 usx, udx, usy, udy;
    prBitblt = (MTGFX_BITBLT_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prBitblt,MTGFX_BITBLT_T);
    COPY_FROM_USER_ARG(prBitblt,rFill,MTGFX_BITBLT_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill",&rFill);
    MTGFX_PRINT(",u1Alpha = %d\n",(&rFill)->u1Alpha);

    //add driver implementation code here
    prBitblt = &rFill;

    NULL_PT_CHECK(prBitblt);
    XYWH_CHECK(prBitblt->i4SrcX, prBitblt->i4SrcY, 
        prBitblt->u4Width, prBitblt->u4Height);
    XYWH_CHECK(prBitblt->i4DstX, prBitblt->i4DstY, 
        prBitblt->u4Width, prBitblt->u4Height);
    
    if(_GfxOpModeCheck((UINT32)OP_BITBLT, 
            (UINT32)prBitblt->eSrcCm, (UINT32)prBitblt->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prBitblt->i4SrcX;
    usy = (UINT32)prBitblt->i4SrcY;
    udx = (UINT32)prBitblt->i4DstX;
    udy = (UINT32)prBitblt->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prBitblt->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prBitblt->t_mmu);
    }

    GFX_SetSrc((UINT8 *)prBitblt->pvSrc, 
        _GfxGetColorMode(prBitblt->eSrcCm), prBitblt->u4SrcPitch);

    GFX_SetDst((UINT8 *)prBitblt->pvDst, 
        _GfxGetColorMode(prBitblt->eDstCm), prBitblt->u4DstPitch);

    GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

    GFX_SetAlpha(prBitblt->u1Alpha);

    GFX_BitBlt(usx, usy, udx, udy, prBitblt->u4Width, prBitblt->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}
/**Perform GFX transparent bitblt operation
*@param  prBitblt                    GFX transparent bitblt operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_TransparentBitblt(unsigned long arg)
{
    MTGFX_TRANSPARENT_BITBLT_T rFill;
    MTGFX_TRANSPARENT_BITBLT_T* prBitblt;
    UINT32 usx, udx, usy, udy;
    prBitblt = (MTGFX_TRANSPARENT_BITBLT_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prBitblt,MTGFX_TRANSPARENT_BITBLT_T);
    COPY_FROM_USER_ARG(prBitblt,rFill,MTGFX_TRANSPARENT_BITBLT_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill",&rFill);
    MTGFX_PRINT(",bInverseMasking = %d,u4ColorSpaceMin = %d,u4ColorSpaceMax = %d\n",
        (&rFill)->bInverseMasking,(&rFill)->u4ColorSpaceMin,(&rFill)->u4ColorSpaceMax);

    //add driver implementation code here
    prBitblt = &rFill;
    
    NULL_PT_CHECK(prBitblt);
    XYWH_CHECK(prBitblt->i4SrcX, prBitblt->i4SrcY, 
        prBitblt->u4Width, prBitblt->u4Height);
    XYWH_CHECK(prBitblt->i4DstX, prBitblt->i4DstY, 
        prBitblt->u4Width, prBitblt->u4Height);

    // aditional mode check due to hw
    if (prBitblt->eSrcCm != prBitblt->eDstCm)
    {
        return MTR_NOT_OK;
    }

    if(_GfxOpModeCheck((UINT32)OP_BITBLT, 
            (UINT32)prBitblt->eSrcCm, (UINT32)prBitblt->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prBitblt->i4SrcX;
    usy = (UINT32)prBitblt->i4SrcY;
    udx = (UINT32)prBitblt->i4DstX;
    udy = (UINT32)prBitblt->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prBitblt->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prBitblt->t_mmu);
    }

    GFX_SetSrc((UINT8*)prBitblt->pvSrc, 
        _GfxGetColorMode(prBitblt->eSrcCm), prBitblt->u4SrcPitch);

    GFX_SetDst((UINT8*)prBitblt->pvDst, 
        _GfxGetColorMode(prBitblt->eDstCm), prBitblt->u4DstPitch);

    GFX_SetBltOpt(
        ((D_GFXFLAG_TRANSPARENT) | 
         (prBitblt->bSrcOrDstKey ? D_GFXFLAG_KEYSDSEL : 0) |
         (prBitblt->bInverseMasking ? D_GFXFLAG_KEYNOT : 0)),
        prBitblt->u4ColorSpaceMin, prBitblt->u4ColorSpaceMax);

    GFX_BitBlt(usx, usy, udx, udy, 
        prBitblt->u4Width, prBitblt->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}
/**Perform transparent fill operation
*@param  prBitblt                    GFX transparent fill operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_TransparentFill(unsigned long arg)
{
    MTGFX_TRANSPARENT_FILL_T rFill;
    MTGFX_TRANSPARENT_FILL_T* prFill;
    UINT32 usx, udx, usy, udy;
    prFill = (MTGFX_TRANSPARENT_FILL_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prFill,MTGFX_TRANSPARENT_FILL_T);
    COPY_FROM_USER_ARG(prFill,rFill,MTGFX_TRANSPARENT_FILL_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",bInverseMasking = %d,u4ColorSpaceMin = %d,u4ColorSpaceMax = %d,u4FillColor = %d\n",
        (&rFill)->bInverseMasking,(&rFill)->u4ColorSpaceMin,(&rFill)->u4ColorSpaceMax,(&rFill)->u4FillColor);

    //add driver implementation code here
    prFill = &rFill;
    
    NULL_PT_CHECK(prFill);
    XYWH_CHECK(prFill->i4SrcX, prFill->i4SrcY, 
        prFill->u4Width, prFill->u4Height);
    XYWH_CHECK(prFill->i4DstX, prFill->i4DstY, 
        prFill->u4Width, prFill->u4Height);
        
    // aditional mode check due to hw
    if (prFill->eSrcCm != prFill->eDstCm)
    {
        return MTR_NOT_OK;
    }

    if(_GfxOpModeCheck((UINT32)OP_BITBLT, 
            (UINT32)prFill->eSrcCm, (UINT32)prFill->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prFill->i4SrcX;
    usy = (UINT32)prFill->i4SrcY;
    udx = (UINT32)prFill->i4DstX;
    udy = (UINT32)prFill->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prFill->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prFill->t_mmu);
    }

    GFX_SetSrc((UINT8*)prFill->pvSrc, _GfxGetColorMode(prFill->eSrcCm),
        prFill->u4SrcPitch);

    GFX_SetDst((UINT8*)prFill->pvDst, _GfxGetColorMode(prFill->eDstCm),
        prFill->u4DstPitch);

    GFX_SetBltOpt(
        ((D_GFXFLAG_COLORCHANGE) | 
         (prFill->bInverseMasking ? D_GFXFLAG_KEYNOT : 0)),
        prFill->u4ColorSpaceMin, prFill->u4ColorSpaceMax);

    GFX_SetColor(prFill->u4FillColor);

    GFX_BitBlt(usx, usy, udx, udy, prFill->u4Width, prFill->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();

    return MTR_OK;
}
/**Perform GFX alpha blending operation
*@param  prBitblt                    GFX alhpa blending operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_AlphaBlending(unsigned long arg)
{
    MTGFX_ALPHA_BLEND_T rFill;
    MTGFX_ALPHA_BLEND_T* prBitblt;
    UINT32 usx, usy, udx, udy;
    
    prBitblt = (MTGFX_ALPHA_BLEND_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prBitblt,MTGFX_ALPHA_BLEND_T);
    COPY_FROM_USER_ARG(prBitblt,rFill,MTGFX_ALPHA_BLEND_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",u1Alpha = %d\n",(&rFill)->u1Alpha);

    //add driver implementation code here
    prBitblt = &rFill;
    
    NULL_PT_CHECK(prBitblt);
    XYWH_CHECK(prBitblt->i4SrcX, prBitblt->i4SrcY, 
        prBitblt->u4Width, prBitblt->u4Height);
    XYWH_CHECK(prBitblt->i4DstX, prBitblt->i4DstY, 
        prBitblt->u4Width, prBitblt->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_ALPHA_BLEND, 
            (UINT32)prBitblt->eSrcCm, (UINT32)prBitblt->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prBitblt->i4SrcX;
    usy = (UINT32)prBitblt->i4SrcY;
    udx = (UINT32)prBitblt->i4DstX;
    udy = (UINT32)prBitblt->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prBitblt->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prBitblt->t_mmu);
    }

    GFX_SetSrc((UINT8*)prBitblt->pvSrc, 
        _GfxGetColorMode(prBitblt->eSrcCm), prBitblt->u4SrcPitch);

    GFX_SetDst((UINT8*)prBitblt->pvDst, 
        _GfxGetColorMode(prBitblt->eDstCm), prBitblt->u4DstPitch);

    GFX_SetAlpha((UINT32)prBitblt->u1Alpha);

    GFX_Blend(usx, usy, udx, udy, prBitblt->u4Width, prBitblt->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}
/**Perform GFX YCbCr to RGB operation
*@param  prYbr2Rgb                 GFX YCbCR to RGB operation structure
*@retval MTR_OK                      Success.
*@retval MTR_NOT_OK               Fail.
*/
static MT_RESULT_T _MTGFX_Ybr2Rgb(unsigned long arg)
{
    MTGFX_YCBCR_TO_RGB_T rFill;
    MTGFX_YCBCR_TO_RGB_T* prYbr2Rgb;
    UINT32 udx, udy;
    UINT32 u4YcFmt;
    prYbr2Rgb = (MTGFX_YCBCR_TO_RGB_T*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(prYbr2Rgb,MTGFX_YCBCR_TO_RGB_T);
    COPY_FROM_USER_ARG(prYbr2Rgb,rFill,MTGFX_YCBCR_TO_RGB_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",(INT32)pvCbCr = %d,ui4CbCrPitch = %d,eYcbcrFormat = %d,eYcbcrSystem = %d\n",
        (INT32)((&rFill)->pvCbCr),(&rFill)->ui4CbCrPitch,(&rFill)->eYcbcrFormat,(&rFill)->eYcbcrSystem);

    //add driver implementation code here
    prYbr2Rgb = &rFill;

    NULL_PT_CHECK(prYbr2Rgb);
    NULL_PT_CHECK(prYbr2Rgb->pvSrc);
    NULL_PT_CHECK(prYbr2Rgb->pvDst);
    NULL_PT_CHECK(prYbr2Rgb->pvCbCr);
    XYWH_CHECK(prYbr2Rgb->i4SrcX, prYbr2Rgb->i4SrcY, 
        prYbr2Rgb->u4Width, prYbr2Rgb->u4Height);
    XYWH_CHECK(prYbr2Rgb->i4DstX, prYbr2Rgb->i4DstY, 
        prYbr2Rgb->u4Width, prYbr2Rgb->u4Height);
    
    if(_GfxOpModeCheck((UINT32)OP_YCBCR_TO_RGB, 
            (UINT32)prYbr2Rgb->eSrcCm, (UINT32)prYbr2Rgb->eDstCm))
    {
        return MTR_NOT_OK;
    }
    
    u4YcFmt = _GfxGetYcFormat(prYbr2Rgb->eYcbcrFormat);
    
    // check valid YC format
    if (u4YcFmt == (UINT32)E_YCFMT_RES_2)
    {
        return MTR_NOT_OK;
    }

    // check valid pitch
    if (u4YcFmt == (UINT32)E_YCFMT_420MB)
    {
        if ((prYbr2Rgb->ui4CbCrPitch << 1) != prYbr2Rgb->u4SrcPitch)
        {
            // return MTR_NOT_OK;
        }
    }
    else    // 420 or 422 linear
    {
        if (prYbr2Rgb->ui4CbCrPitch != prYbr2Rgb->u4SrcPitch)
        {
            // return MTR_NOT_OK;
        }
    }
    
    udx = (UINT32)prYbr2Rgb->i4DstX;
    udy = (UINT32)prYbr2Rgb->i4DstY;
    
    GFX_Lock();
    GFX_Flush_Invalid_DCache();
    
    if(prYbr2Rgb->t_mmu.u4_init&&prYbr2Rgb->t_mmu.u4_enable)
    {
        // GFX_MMU_Set_Enable(0x1);
        GFX_MMU_Set_Cfg(&prYbr2Rgb->t_mmu);
    }
    
    GFX_Set_SrcDst((UINT8*)prYbr2Rgb->pvSrc,
                  prYbr2Rgb->u4SrcPitch,
                  (UINT8*)prYbr2Rgb->pvCbCr,
                  prYbr2Rgb->ui4CbCrPitch,
                  (UINT8*)prYbr2Rgb->pvDst,
                  prYbr2Rgb->u4DstPitch,
                  _GfxGetColorMode(prYbr2Rgb->eDstCm),
                  udx,udy,
                  prYbr2Rgb->u4Width,
                  prYbr2Rgb->u4Height);
    GFX_Set_ColConvInfo(u4YcFmt,
                        (UINT32)E_SWAP_BLOCK,
                        (UINT32)prYbr2Rgb->eYcbcrSystem,
                        (UINT32)E_VSYS_VID,
                        0,
                        (UINT32)prYbr2Rgb->u1Alpha);
    GFX_Flush();
    GFX_MMU_Set_Enable(0);
    GFX_Unlock();
    return MTR_OK;
}
/**Perform GFX color format conversion  operation
*@param  prConversion              color format conversion operation structure
*@retval MTR_OK                      Success.
*@retval MTR_NOT_OK               Fail.
*/
static MT_RESULT_T _MTGFX_ColorFormatConversion(unsigned long arg)
{

    MTGFX_YCBCR_FORMAT_CONVERSION_T rFill;
    MTGFX_YCBCR_FORMAT_CONVERSION_T* prConversion;
    prConversion = (MTGFX_YCBCR_FORMAT_CONVERSION_T*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(prConversion,MTGFX_YCBCR_FORMAT_CONVERSION_T);
    COPY_FROM_USER_ARG(prConversion,rFill,MTGFX_YCBCR_FORMAT_CONVERSION_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);

    //add driver implementation code here
    prConversion = &rFill;

    NULL_PT_CHECK(prConversion);
    NULL_PT_CHECK(prConversion->pvSrc);
    NULL_PT_CHECK(prConversion->pvDst);
    NULL_PT_CHECK(prConversion->pvCbCr);
    XYWH_CHECK(prConversion->i4SrcX, prConversion->i4SrcY, 
        prConversion->u4Width, prConversion->u4Height);
    XYWH_CHECK(prConversion->i4DstX, prConversion->i4DstY, 
        prConversion->u4Width, prConversion->u4Height);

    GFX_Lock();
    GFX_Flush_Invalid_DCache();
    if(((UINT32)prConversion->pvSrc)&0xFFF)
    {
        printf("Source Y address isn't 4096 align !\n");
        return MTR_NOT_OK;
    }
    if(((UINT32)prConversion->pvCbCr)&0x7FF)
    {
        printf("Source CbCr address isn't 2048 align !\n");
        return MTR_NOT_OK;
    }
    if((prConversion->t_mmu.u4_init)&&(prConversion->t_mmu.u4_enable)
            &&(prConversion->dstColorformat!=MTGFX_YCbCrBlockToRaster_3Buf))
    {
        GFX_MMU_Set_Cfg(&prConversion->t_mmu);
    }
  
    switch(prConversion->srcColorformat)
    {
        case MTGFX_YCbCr420:
        {
            switch(prConversion->dstColorformat)
            {
                case MTGFX_ARGB888:
                    GFX_YCbCr420ToARGB32((UINT8*)prConversion->pvSrc,
                                         prConversion->u4SrcPitch,
                                         (UINT8*)prConversion->pvCbCr,
                                         prConversion->ui4CbCrPitch,
                                         (UINT8*)prConversion->pvDst,
                                         prConversion->u4Width,
                                         prConversion->u4Height,
                                         prConversion->u1Alpha,
                                         prConversion->u4FieldPic);
                    break;
                case MTGFX_RGB:
                    break;
                case MTGFX_YCbCrBlockToRaster_2Buf :
                    GFX_YCbCrBlockToRaster_2Buf((UINT8*)prConversion->pvSrc,
                                         prConversion->u4SrcPitch,
                                         (UINT8*)prConversion->pvCbCr,
                                         prConversion->ui4CbCrPitch,
                                         (UINT8*)prConversion->pvDst,
                                         prConversion->u4Width,
                                         prConversion->u4Height);
                    break;
                case MTGFX_YCbCrBlockToRaster_3Buf :
                    GFX_YCbCrBlockToRaster_3Buf((UINT8*)prConversion->pvSrc,
                                         prConversion->u4SrcPitch,
                                         (UINT8*)prConversion->pvCbCr,
                                         prConversion->ui4CbCrPitch,
                                         (UINT8*)prConversion->pvDst,
                                         prConversion->u4Width,
                                         prConversion->u4Height,
                                         (void *)&prConversion->t_mmu);
                    
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    GFX_Flush();
    GFX_MMU_Set_Enable(0);
    GFX_Unlock();
    return MTR_OK;
}

/**Perform GFX alpha map operation
*@param  prAlphaMap                 GFX alpha map operation structure
*@retval MTR_OK                       Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTGFX_AlphaMapBitblt(unsigned long arg)
{
    MTGFX_ALPHAMAP_BITBLT_T rFill;
    MTGFX_ALPHAMAP_BITBLT_T* prAlphaMap;
    UINT32 usx, usy, udx, udy;
    UINT32 u4SrcCM;

    prAlphaMap = (MTGFX_ALPHAMAP_BITBLT_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prAlphaMap,MTGFX_ALPHAMAP_BITBLT_T);
    COPY_FROM_USER_ARG(prAlphaMap,rFill,MTGFX_ALPHAMAP_BITBLT_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);

    //add driver implementation code here
    prAlphaMap = &rFill;


    NULL_PT_CHECK(prAlphaMap);
    XYWH_CHECK(prAlphaMap->i4SrcX, prAlphaMap->i4SrcY, 
        prAlphaMap->u4Width, prAlphaMap->u4Height);
    XYWH_CHECK(prAlphaMap->i4DstX, prAlphaMap->i4DstY, 
        prAlphaMap->u4Width, prAlphaMap->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_ALPHA_MAP_BITBLT, 
            (UINT32)prAlphaMap->eSrcCm, (UINT32)prAlphaMap->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prAlphaMap->i4SrcX;
    usy = (UINT32)prAlphaMap->i4SrcY;
    udx = (UINT32)prAlphaMap->i4DstX;
    udy = (UINT32)prAlphaMap->i4DstY;

    // CM_YCbCr_CLUT8 changed to CM_RGB_CLUT8
    u4SrcCM = _GfxGetColorMode(prAlphaMap->eSrcCm);
    if (u4SrcCM == (UINT32)CM_YCbCr_CLUT8)
    {
        u4SrcCM = (UINT32)CM_RGB_CLUT8;
    }
    
    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prAlphaMap->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prAlphaMap->t_mmu);
    }

    GFX_SetSrc((UINT8 *)prAlphaMap->pvSrc, 
        u4SrcCM, prAlphaMap->u4SrcPitch);

    GFX_SetDst((UINT8 *)prAlphaMap->pvDst, 
        _GfxGetColorMode(prAlphaMap->eDstCm), prAlphaMap->u4DstPitch);

    GFX_AlphaMapBitBlt(usx, usy, udx, udy, prAlphaMap->u4Width, 
        prAlphaMap->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();

    return MTR_OK;
}
/**Perform GFX Rop bitblt operation
*@param  prRop                      GFX Rop bitblt operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_RopBitblt(unsigned long arg)
{
    MTGFX_ROP_BITBLT_T rFill;
    MTGFX_ROP_BITBLT_T* prRop;
    UINT32 usx, udx, usy, udy;
    UINT32 u4RopCode;

    prRop = (MTGFX_ROP_BITBLT_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prRop,MTGFX_ROP_BITBLT_T);
    COPY_FROM_USER_ARG(prRop,rFill,MTGFX_ROP_BITBLT_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",eRopType = %d\n",(&rFill)->eRopType);

    //add driver implementation code here
    prRop = &rFill;

    NULL_PT_CHECK(prRop);
    XYWH_CHECK(prRop->i4SrcX, prRop->i4SrcY, 
        prRop->u4Width, prRop->u4Height);
    XYWH_CHECK(prRop->i4DstX, prRop->i4DstY, 
        prRop->u4Width, prRop->u4Height);
    
    if(_GfxOpModeCheck((UINT32)OP_ROP_BITBLT, 
            (UINT32)prRop->eSrcCm, (UINT32)prRop->eDstCm))
    {
        return MTR_NOT_OK;
    }

    u4RopCode = _GfxGetRopCode(prRop->eRopType);
    if(u4RopCode == (UINT32)E_ROP_MAX) 
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prRop->i4SrcX;
    usy = (UINT32)prRop->i4SrcY;
    udx = (UINT32)prRop->i4DstX;
    udy = (UINT32)prRop->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prRop->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prRop->t_mmu);
    }


    if(u4RopCode == E_ROP_MULTI)
    {
        GFX_SetColor(prRop->u4Color);
    }

    GFX_SetSrc((UINT8 *)prRop->pvSrc, 
        _GfxGetColorMode(prRop->eSrcCm), prRop->u4SrcPitch);

    GFX_SetDst((UINT8 *)prRop->pvDst, 
        _GfxGetColorMode(prRop->eDstCm), prRop->u4DstPitch);

    GFX_SetRopOpt(u4RopCode);

    GFX_RopBitBlt(usx, usy, udx, udy, prRop->u4Width, prRop->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();

    return MTR_OK;
}
/**Perform GFX alpha composition operation
*@param  prRop                      GFX alpha composition operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_AlphaComposition(unsigned long arg)
{
    MTGFX_ALPHA_COMPOSITION_T rFill;
    MTGFX_ALPHA_COMPOSITION_T* prAcomp;
    UINT32 usx, usy, udx, udy, uar, upd, u4w, u4h;
    UINT32 u4RectSrcFlag;
    UINT32 u4SrcColorMode;
    UINT32 u4ThirdColorMode;
    UINT32 u4MsbLeftFlag;
    prAcomp = (MTGFX_ALPHA_COMPOSITION_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prAcomp,MTGFX_ALPHA_COMPOSITION_T);
    COPY_FROM_USER_ARG(prAcomp,rFill,MTGFX_ALPHA_COMPOSITION_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",u1Alpha = %d,ePDRule = %d,bRectSrcOption = %d,u4Color = %d\n",
        (&rFill)->u1Alpha,(&rFill)->ePDRule,(&rFill)->bRectSrcOption,(&rFill)->u4Color);

    //add driver implementation code here
    prAcomp = &rFill;
    
    NULL_PT_CHECK(prAcomp);
    XYWH_CHECK(prAcomp->i4SrcX, prAcomp->i4SrcY, 
        prAcomp->u4Width, prAcomp->u4Height);
    XYWH_CHECK(prAcomp->i4DstX, prAcomp->i4DstY, 
        prAcomp->u4Width, prAcomp->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_ALPHA_COMPOSE_LOOP, 
            (UINT32)prAcomp->eSrcCm, (UINT32)prAcomp->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prAcomp->i4SrcX;
    usy = (UINT32)prAcomp->i4SrcY;
    udx = (UINT32)prAcomp->i4DstX;
    udy = (UINT32)prAcomp->i4DstY;
    uar = (UINT32)prAcomp->u1Alpha;
    upd = (UINT32)prAcomp->ePDRule;
    
    u4w = prAcomp->u4Width;
    u4h = prAcomp->u4Height;
    u4RectSrcFlag = (UINT32)prAcomp->bRectSrcOption;
    GFX_Lock();
    if(GFX_INIT_MMU_VAL == prAcomp->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prAcomp->t_mmu);
    }
    u4SrcColorMode = _GfxGetSrcThirdMode(prAcomp->eSrcCm);
    u4ThirdColorMode=_GfxGetSrcThirdMode(prAcomp->u4ThirdColorMode);
    if ((u4SrcColorMode == (UINT32)E_BMP_CM_1BIT)||(u4ThirdColorMode==(UINT32)E_BMP_CM_1BIT))
    {
		GFX_Unlock();
        return MTR_NOT_OK;
    }
    if (prAcomp->eByteAligned == MTGFX_PALETTE_MSB)
    {
        u4MsbLeftFlag = (UINT32)E_IDX2DIR_MSB_LEFT_ON;
    }
    else
    {
        u4MsbLeftFlag = (UINT32)E_IDX2DIR_MSB_LEFT_OFF;
    }
    if(u4SrcColorMode==(UINT32)E_BMP_CM_2BIT
        ||u4SrcColorMode==(UINT32)E_BMP_CM_4BIT
        ||u4SrcColorMode==(UINT32)E_BMP_CM_8BIT)
    {
        GFX_SetCharCM(TRUE, u4SrcColorMode);
        GFX_SetIdx2DirOpt((UINT8 *) prAcomp->pvSrcPallete, u4MsbLeftFlag, 0);
    }
    
    if(prAcomp->fgThirdEn)
    {
        _GFX_SetThird((UINT8 *) prAcomp->pvThird, 
                _GfxGetColorMode(prAcomp->u4ThirdColorMode), prAcomp->u4ThirdPitch);
        if(u4ThirdColorMode==(UINT32)E_BMP_CM_2BIT
        ||u4ThirdColorMode==(UINT32)E_BMP_CM_4BIT
        ||u4ThirdColorMode==(UINT32)E_BMP_CM_8BIT)
        {
            GFX_SetCharCM(FALSE, u4ThirdColorMode);
            _GFX_SetThirdPallete((UINT8 *) prAcomp->pvThirdPallete, u4MsbLeftFlag, 0);
        }
        
    }
	GFX_SetColor(prAcomp->u4Color);
	GFX_SetBltOpt(0, 0, 0);
	if(!prAcomp->bRectSrcOption)
	{
		GFX_SetSrc((UINT8*)prAcomp->pvSrc, 
				_GfxGetColorMode(prAcomp->eSrcCm), prAcomp->u4SrcPitch);
	}
	GFX_SetSrc((UINT8*)prAcomp->pvSrc, 
			_GfxGetColorMode(prAcomp->eSrcCm), prAcomp->u4SrcPitch);
	GFX_SetDst((UINT8 *) prAcomp->pvDst, 
			_GfxGetColorMode(prAcomp->eDstCm), prAcomp->u4DstPitch);
	GFX_SetAlpha(prAcomp->u1Alpha);
	GFX_SetMaxAlphaEn(1);
	// After MT5395, we support pre-multiply
	GFX_SetPremult((UINT32)prAcomp->fgPremultSrc, (UINT32)prAcomp->fgPremultDst, (UINT32)prAcomp->fgPremultDstWr, 1);
	//GFX_GeneralAlphaCom(u4w, u4h, uar, upd, u4RectSrcFlag, prAcomp->fgThirdEn, 0, FALSE);
	GFX_ComposeLoop(usx, usy, udx, udy, u4w, u4h, uar, upd, u4RectSrcFlag);
	GFX_Flush();
	GFX_Unlock();
    return MTR_OK;
}


static MT_RESULT_T _MTGFX_AlphaCompositionImprove(unsigned long arg)
{
    MTGFX_ALPHA_COMPOSITION_IMPROVE_T rFill;
    MTGFX_ALPHA_COMPOSITION_IMPROVE_T* prAcomp;
    UINT32 usx, usy, udx, udy, uar, upd, u4w, u4h;
    UINT32 u4RectSrcFlag;
    
    prAcomp = (MTGFX_ALPHA_COMPOSITION_IMPROVE_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prAcomp,MTGFX_ALPHA_COMPOSITION_IMPROVE_T);
    COPY_FROM_USER_ARG(prAcomp,rFill,MTGFX_ALPHA_COMPOSITION_IMPROVE_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",u1Alpha = %d,ePDRule = %d,bRectSrcOption = %d,u4Color = %d\n",
        (&rFill)->u1Alpha,(&rFill)->ePDRule,(&rFill)->bRectSrcOption,(&rFill)->u4Color);

    //add driver implementation code here
    prAcomp = &rFill;
    
    NULL_PT_CHECK(prAcomp);
    XYWH_CHECK(prAcomp->i4SrcX, prAcomp->i4SrcY, 
        prAcomp->u4Width, prAcomp->u4Height);
    XYWH_CHECK(prAcomp->i4DstX, prAcomp->i4DstY, 
        prAcomp->u4Width, prAcomp->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_ALPHA_COMPOSE_LOOP, 
            (UINT32)prAcomp->eSrcCm, (UINT32)prAcomp->eDstCm))
    {
        return MTR_NOT_OK;
    }

    usx = (UINT32)prAcomp->i4SrcX;
    usy = (UINT32)prAcomp->i4SrcY;
    udx = (UINT32)prAcomp->i4DstX;
    udy = (UINT32)prAcomp->i4DstY;
    uar = (UINT32)prAcomp->u1Alpha;
    upd = (UINT32)prAcomp->ePDRule;
    
    u4w = prAcomp->u4Width;
    u4h = prAcomp->u4Height;
    
    u4RectSrcFlag = (UINT32)prAcomp->bRectSrcOption;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prAcomp->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prAcomp->t_mmu);
    }
      
    GFX_SetColor(prAcomp->u4Color);
    GFX_SetSrc((UINT8*)prAcomp->pvSrc, 
        _GfxGetColorMode(prAcomp->eSrcCm), prAcomp->u4SrcPitch);

    GFX_SetDst((UINT8*)prAcomp->pvDst, 
        _GfxGetColorMode(prAcomp->eDstCm), prAcomp->u4DstPitch);

    if(prAcomp->eSrcCm == MTGFX_CM_RGB_CLUT8)   
    {
     GFX_SetCharCM(TRUE,E_BMP_CM_8BIT);
     GFX_SetIdx2DirOpt(prAcomp->pPalette,0,0);
    }
    // After MT5395, we support pre-multiply
    GFX_SetPremult((UINT32)prAcomp->fgPremultSrc, (UINT32)prAcomp->fgPremultDst, (UINT32)prAcomp->fgPremultDstWr, 1);

    GFX_ComposeLoopImprove(usx, usy, udx, udy, u4w, u4h, uar, upd, u4RectSrcFlag);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}



/**Perform GFX index to direct color mode operation
*@param  prRop                      GFX index to direct color mode operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Idx2DirBitblt(unsigned long arg)
{
    MTGFX_PLTBLT_T rFill;
    MTGFX_PLTBLT_T* prPale;
    UINT32 udx, udy;
    UINT32 u4MsbLeftFlag;
    UINT32 u4FontMode;
    
    prPale = (MTGFX_PLTBLT_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prPale,MTGFX_PLTBLT_T);
    COPY_FROM_USER_ARG(prPale,rFill,MTGFX_PLTBLT_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",eByteAligned = %d,pPalette = %d\n",(&rFill)->eByteAligned,(&rFill)->pPalette);

    //add driver implementation code here
    prPale = &rFill;

    NULL_PT_CHECK(prPale);
    XYWH_CHECK(prPale->i4SrcX, prPale->i4SrcY, 
        prPale->u4Width, prPale->u4Height);
    XYWH_CHECK(prPale->i4DstX, prPale->i4DstY, 
        prPale->u4Width, prPale->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_IDX_TO_DIR_BITBLT, 
            (UINT32)prPale->eSrcCm, (UINT32)prPale->eDstCm))
    {
        return MTR_NOT_OK;
    }

    u4FontMode = _GfxGetFontMode(prPale->eSrcCm);
    if (u4FontMode == (UINT32)E_BMP_CM_1BIT)
    {
        return MTR_NOT_OK;
    }
    
    udx = (UINT32)prPale->i4DstX;
    udy = (UINT32)prPale->i4DstY;

    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prPale->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prPale->t_mmu);
    }

    GFX_SetIdx2DirSrc((UINT8*)prPale->pvSrc, u4FontMode);

    GFX_SetDst((UINT8*)prPale->pvDst, 
        _GfxGetColorMode(prPale->eDstCm), prPale->u4DstPitch);

    if (prPale->eByteAligned == MTGFX_PALETTE_MSB)
    {
        u4MsbLeftFlag = (UINT32)E_IDX2DIR_MSB_LEFT_ON;
    }
    else
    {
        u4MsbLeftFlag = (UINT32)E_IDX2DIR_MSB_LEFT_OFF;
    }
    
    GFX_SetIdx2DirOpt((UINT8 *)prPale->pPalette, u4MsbLeftFlag, 0);

    GFX_Idx2DirBitBlt(udx, udy, prPale->u4Width, prPale->u4Height);

    GFX_DBG_FLUSH();

    GFX_Unlock();
    return MTR_OK;
}
/**Perform GFX rotation operation
*@param  prRotate                  GFX rotation operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Rotate90(unsigned long arg)
{
    MTGFX_ROTATE_T rFill;
    MTGFX_ROTATE_T* prRotate;
    INT32 i4Sx, i4Sy, i4Dx, i4Dy;
    UINT32 i;
    
    prRotate = (MTGFX_ROTATE_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prRotate,MTGFX_ROTATE_T);
    COPY_FROM_USER_ARG(prRotate,rFill,MTGFX_ROTATE_T);

    COMMON_BITBLT_FIELDS_PRINT("rFill", &rFill);
    MTGFX_PRINT(",eDirection = %d\n",(&rFill)->eDirection);

    //add driver implementation code here
    prRotate = &rFill;
    
    NULL_PT_CHECK(prRotate);
    XYWH_CHECK(prRotate->i4SrcX, prRotate->i4SrcY, 
        prRotate->u4Width, prRotate->u4Height);
    XYWH_CHECK(prRotate->i4DstX, prRotate->i4DstY, 
        prRotate->u4Width, prRotate->u4Height);

    if(_GfxOpModeCheck((UINT32)OP_LINE_HORI_TO_VERT, 
            (UINT32)prRotate->eSrcCm, (UINT32)prRotate->eDstCm))
    {
        return MTR_NOT_OK;
    }
    
    i4Sx = prRotate->i4SrcX;
    i4Sy = prRotate->i4SrcY;
    i4Dx = prRotate->i4DstX;
    i4Dy = prRotate->i4DstY;
    
    GFX_Lock();
    
    if(GFX_INIT_MMU_VAL == prRotate->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prRotate->t_mmu);
    }

    GFX_SetSrc((UINT8 *)prRotate->pvSrc, 
        _GfxGetColorMode(prRotate->eSrcCm), prRotate->u4SrcPitch);

    GFX_SetDst((UINT8 *)prRotate->pvDst, 
        _GfxGetColorMode(prRotate->eDstCm), prRotate->u4DstPitch);

    if (prRotate->eDirection == MTGFX_C_CLOCKWISE)
    {
        // rotated by 90" counter-clock-wise
        GFX_SetHoriToVertLineOpt(1);

        for(i = 0; i < prRotate->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prRotate->u4Width);

            i4Sy++;
            i4Dx++;
        } // ~for
    }
    else
    {
        // rotated by 90" clock-wise
        GFX_SetHoriToVertLineOpt(0);

        for(i = 0; i < prRotate->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prRotate->u4Width);

            i4Sy++;
            i4Dx--;
        } // ~for
    }

    // flush and wait
    GFX_Flush();
    GFX_Wait();

    GFX_Unlock();

    return MTR_OK;
}

/**Perform GFX rotation operation
*@param  prRotate                  GFX rotation operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_GetBufferProperties(unsigned long arg)
{
	extern UINT32 fb_osd_get_fb_addr(void);
	extern UINT32 fb_osd_get_fb_size(void);	
    MTGFX_BUFPROP_T rProp;
    UINT32 u4Tmp;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTGFX_BUFPROP_T);

    // Ensure page alignment
    VERIFY(PAGE_ALIGN(fb_osd_get_fb_size() & ~PAGE_MASK) == 0);      
    VERIFY(PAGE_ALIGN(DRVCUST_OptGet(eDirectFBMemSize) & ~PAGE_MASK) == 0);      
    
    //rProp.u4Size = DRVCUST_OptGet(eFBMemSize);
	rProp.u4Size = fb_osd_get_fb_size() -DRVCUST_OptGet(eGfxFBPartitionSize);
    rProp.u4Addr = fb_osd_get_fb_addr();    

	u4Tmp = rProp.u4Addr;
	rProp.u4Addr = (rProp.u4Addr + PAGE_SIZE - 1) & PAGE_MASK;
	rProp.u4Size -= (rProp.u4Addr - u4Tmp);
	rProp.u4Size &= PAGE_MASK;

    COPY_TO_USER_ARG(arg, rProp,MTGFX_BUFPROP_T);

    return MTR_OK;

}

/**Perform GFX rotation operation
*@param  prRotate                  GFX rotation operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_GetBufferPartition(unsigned long arg)
{
	extern UINT32 fb_osd_get_fb_addr(void);
	extern UINT32 fb_osd_get_fb_size(void);	
    MTGFX_BUFPROP_T rProp;
	UINT32 u4Start, u4Size, u4Tmp;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTGFX_BUFPROP_T);

    // Ensure page alignment
    VERIFY(PAGE_ALIGN(fb_osd_get_fb_size() & ~PAGE_MASK) == 0);   
    VERIFY(PAGE_ALIGN(DRVCUST_OptGet(eDirectFBMemSize) & ~PAGE_MASK) == 0);      
    
    //rProp.u4Size = DRVCUST_OptGet(eGfxFBPartitionSize) ;
    //rProp.u4Addr = BSP_GetFbmMemAddr() - (DRVCUST_OptGet(eFBMemSize) + DRVCUST_OptGet(eDirectFBMemSize))+DRVCUST_OptGet(eGfxFBPartitionSize); 
    //rProp.u4Addr = BSP_GetFbmMemAddr() - DRVCUST_OptGet(eGfxFBPartitionSize) - DRVCUST_OptGet(eDirectFBMemSize);
	u4Start = fb_osd_get_fb_addr();
	u4Size = fb_osd_get_fb_size();

    u4Tmp = u4Start;
	u4Start = (u4Start + PAGE_SIZE - 1) & PAGE_MASK;
	u4Size -= (u4Start - u4Tmp);
	u4Size &= PAGE_MASK;

    rProp.u4Size = DRVCUST_OptGet(eGfxFBPartitionSize);
    rProp.u4Addr = (u4Start + u4Size - rProp.u4Size) & PAGE_MASK;
	
    COPY_TO_USER_ARG(arg, rProp,MTGFX_BUFPROP_T);

    return MTR_OK;

}

/**Perform GFX rotation operation
*@param  prRotate                  GFX rotation operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_Get_FBM_Buffer(unsigned long arg)
{
    MTGFX_BUFPROP_T rProp;
    FBM_POOL_T* prFbmPool;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTGFX_BUFPROP_T);

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_SUBTITLE);
    
    rProp.u4Size = prFbmPool->u4Size;
    rProp.u4Addr = prFbmPool->u4Addr;
    
    rProp.u4Addr &= PAGE_MASK;
    rProp.u4Size &= PAGE_MASK;
    
    // printk("_MTGFX_Get_FBM_Buffer: [0x%x,0x%x] \n",rProp.u4Addr,rProp.u4Size);
    
    COPY_TO_USER_ARG(arg, rProp,MTGFX_BUFPROP_T);

    return MTR_OK;
}

#ifdef CC_SUPPORT_TVE_OSD_OUT

/**Perform scaling rotation operation
*@param  prRGB2YBR                 GFX RGB 2 YCbCr operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/

static MT_RESULT_T _MTGFX_Rgb2Ybr(unsigned long arg)
{
    MTGFX_RGB_TO_YCBCR_T rRGB2YBR;
    MTGFX_RGB_TO_YCBCR_T* prRGB2YBR;
    UINT8* pu1SrcAddr =NULL;
    UINT32* pu4PaletteTable =NULL;
    UINT8* pu1DstAddr = NULL;
    UINT32 u4SrcX,u4SrcY,u4UpdateWidth,u4UpdateHeight;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4Idx,i,j;
    UINT32 u4FbmAddr;
    BOOL fgPartialUpdate;
    
    
    FBM_POOL_T* prFbmPool;
    MTGFX_PALLETE_TABLE_T rPalleteTable;
    
    MTGFX_DATA_BUFFER_T rDataBuffer;
    
    prRGB2YBR = (MTGFX_RGB_TO_YCBCR_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prRGB2YBR,MTGFX_RGB_TO_YCBCR_T);
    COPY_FROM_USER_ARG(prRGB2YBR,rRGB2YBR,MTGFX_RGB_TO_YCBCR_T);
    COPY_FROM_USER_ARG((prRGB2YBR->pvPaletteTable),rPalleteTable,MTGFX_PALLETE_TABLE_T);    

    //to be done
    u4Width = prRGB2YBR->u4Width;
    u4Height = prRGB2YBR->u4Height;

    u4SrcX = prRGB2YBR->u4SrcX;
    u4SrcY = prRGB2YBR->u4SrcY;
    u4UpdateWidth = prRGB2YBR->u4UpdateWidth;
    u4UpdateHeight = prRGB2YBR->u4UpdateHeight;

    if ((u4SrcX==0) && (u4SrcY==0) && (u4UpdateWidth == u4Width) && (u4UpdateHeight == u4Height))
        fgPartialUpdate = FALSE;
    else
        fgPartialUpdate = TRUE;
        
    pu1SrcAddr = VIRTUAL(prRGB2YBR->pvSrcAddr);
    pu4PaletteTable = VIRTUAL(&(rPalleteTable.u4PalleteTable[0]));//VIRTUAL(prRGB2YBR->pvPaletteTable);

    prFbmPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_SCPOS_PIP, NULL);
    
    if (fgPartialUpdate)
        u4FbmAddr= prFbmPool->u4Addr+ vApiTVEGetOSDBufOffset(SV_TVE_OSD_DISPLAY_BUF);           
    else
        u4FbmAddr= prFbmPool->u4Addr+ vApiTVEGetOSDBufOffset(SV_TVE_OSD_WORKING_BUF);
       
    pu1DstAddr = VIRTUAL((UINT8*)u4FbmAddr);
    ASSERT(prFbmPool != NULL);
    ASSERT(u4FbmAddr != NULL);

    for(i=u4SrcY ; i<(u4SrcY+u4UpdateHeight) ; i++) 
    {
        COPY_FROM_USER_ARG((prRGB2YBR->pvSrcAddr)+(i*u4Width),rDataBuffer,MTGFX_DATA_BUFFER_T);

        for(j=u4SrcX ; j<(u4SrcX+u4UpdateWidth) ; j++)
        {
            INT32 R,G,B;
            INT32 Y,Cb,Cr;
            UINT32 ARGB;
            UINT32 YAddr,CbAddr,CrAddr;
            UINT8* index = VIRTUAL(&rDataBuffer.DataBuffer[0]);
            UINT32 u4ChromaAddr;
               //UINT32 u4CrBaseAddr;

            u4Idx = i*u4Width + j;  
            ARGB = pu4PaletteTable[index[j]];

            B = ARGB & 0x000000FF;
            G = (ARGB>>8) & 0x000000FF;
            R = (ARGB>>16) & 0x000000FF;
                
            Y = ((77*R) + (150*G) + (29*B))/256;
            Cb = ((-44*R) - (87*G) + (131*B))/256 + 128;
            if (Cb >255) Cb = 255;
            if (Cb <0) Cb = 0;                      
            Cr = ((131*R) - (110*G) - (21*B))/256 + 128;
            if (Cr >255) Cr = 255;
            if (Cr <0) Cr = 0;                      
            
            YAddr = 16*(u4Idx/8)+15 - 2*(u4Idx%8);
            u4ChromaAddr = YAddr-1;
            
            pu1DstAddr[YAddr] = (UINT8)Y;
            if(j%2==0)
            pu1DstAddr[u4ChromaAddr] = (UINT8)Cb;
            else
                pu1DstAddr[u4ChromaAddr] = (UINT8)Cr;
        }
        
    }

    if(!fgPartialUpdate)
        vApiTVEUpdateOSDBufOffset(u4FbmAddr);
    
    return MTR_OK;
}
#endif

/**Perform scaling rotation operation
*@param  prScaler                  GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
#ifdef GFX_SPEED_OPTION_1
#define IMGRZ2_TEST
#endif

#ifdef CC_GPU_SUPPORT
//#define IMGRZ_IMPROVE_TEST
#endif
/**Force GFX module to do operation
*@param                                VOID
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_GFX_MMU_Set_Enable(unsigned long arg)
{
    MTGFX_MMU_SET_T     r_set;
    MTGFX_MMU_SET_T*    pr_set = (MTGFX_MMU_SET_T*)arg;
    
    COPY_FROM_USER_ARG(pr_set,r_set,MTGFX_MMU_SET_T);
    
    //add driver implementation code here
    GFX_MMU_Set_Cfg(&r_set);
    return MTR_OK;
}

static MT_RESULT_T _MTGFX_Scaler(unsigned long arg)
{
    RZ_OSD_SCL_PARAM_SET_T rSclParam;
    MTGFX_SCALER_T rScaler;
    MTGFX_SCALER_T *prScaler;
    #ifdef IMGRZ2_TEST
    RZ_OSD_SCL_PARAM_SET_T rSclParam2;
    #endif

    prScaler = (MTGFX_SCALER_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prScaler, MTGFX_SCALER_T);
    COPY_FROM_USER_ARG(prScaler, rScaler, MTGFX_SCALER_T);

    prScaler = &rScaler;
    
    NULL_PT_CHECK(prScaler);
    XYWH_CHECK(prScaler->i4SrcX, prScaler->i4SrcY, 
        prScaler->u4Width, prScaler->u4Height);
    XYWH_CHECK(prScaler->i4DstX, prScaler->i4DstY, 
        prScaler->u4DstWidth, prScaler->u4DstHeight);

    COLOR_CHECK(prScaler->eSrcCm);
    COLOR_CHECK(prScaler->eDstCm);

    x_memset((void *)&rSclParam, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T)); 
    #ifdef IMGRZ2_TEST
    x_memset((void *)&rSclParam2, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T)); 
    #endif
    
    rSclParam.u4SrcBase     = (UINT32) prScaler->pvSrc;
    rSclParam.u4IsSrcDirCM  = (UINT32) 1;
    rSclParam.u4SrcCM       = (UINT32) _GfxGetIMGRZColor(prScaler->eSrcCm);
    rSclParam.u4SrcBufLen   = (UINT32) D_RZ_ALIGN16(prScaler->u4SrcPitch);
    rSclParam.u4SrcHOffset  = (UINT32) prScaler->i4SrcX;
    rSclParam.u4SrcVOffset  = (UINT32) prScaler->i4SrcY;
    rSclParam.u4SrcW        = (UINT32) prScaler->u4Width;
    rSclParam.u4SrcH        = (UINT32) prScaler->u4Height;

    rSclParam.u4TgBase      = (UINT32) prScaler->pvDst;
    rSclParam.u4TgHOffset   = (UINT32) prScaler->i4DstX;
    rSclParam.u4TgVOffset   = (UINT32) prScaler->i4DstY;
    rSclParam.u4TgW         = (UINT32) prScaler->u4DstWidth;
    rSclParam.u4TgH         = (UINT32) prScaler->u4DstHeight;
    rSclParam.u4TgCM        = (UINT32) _GfxGetIMGRZColor(prScaler->eDstCm);
    rSclParam.u4TgBufLen    = (UINT32) D_RZ_ALIGN16(prScaler->u4DstPitch);
    rSclParam.u4RzIOMMU_MMUTableAddr = HalCpuGetPhyPgd();
#ifdef IMGRZ_IMPROVE_TEST
 if(rSclParam.u4TgHOffset%4 !=0)
    {
      rSclParam.fgOSDTurboMode = 0;
    }
 else
    {
     rSclParam.fgOSDTurboMode = 1;
    }
#endif
 #ifdef IMGRZ2_TEST
    x_memcpy((void *)&rSclParam2, (void *)&rSclParam, sizeof(RZ_OSD_SCL_PARAM_SET_T));  
    rSclParam.u4PartialUpdate = TRUE;

    //relative coordinate
    rSclParam.u4PartialUpdate_ClipTgx = 0;
    rSclParam.u4PartialUpdate_ClipTgy = 0;      
    rSclParam.u4PartialUpdate_ClipTgw = rSclParam.u4TgW/2;
    rSclParam.u4PartialUpdate_ClipTgh = rSclParam.u4TgH;
    
    //_IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_OSDMD);
   // _IMGRZ_Scale(&rSclParam);

    rSclParam2.u4PartialUpdate = TRUE;

    //relative coordinate
    rSclParam2.u4PartialUpdate_ClipTgx = rSclParam2.u4TgW/2;
    rSclParam2.u4PartialUpdate_ClipTgy = 0;     
    rSclParam2.u4PartialUpdate_ClipTgw = rSclParam2.u4TgW - (rSclParam2.u4TgW/2);
    rSclParam2.u4PartialUpdate_ClipTgh = rSclParam.u4TgH;
    rSclParam2.u4TgHOffset = (rSclParam2.u4TgW/2);
    
    _IMGRZ_ReInit2();   
    IMGRZ_Lock();
    IMGRZ_ReInit();
    if((GFX_INIT_MMU_VAL == prScaler->t_mmu.u4_init)&&(prScaler->t_mmu.u4_enable))
    {
        IMGRZ_Cfg_MMU_IO((BOOL)1,(BOOL)1);
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_OSDMD);
    IMGRZ_Scale(&rSclParam);
    _IMGRZ_SetScaleOpt2(E_RZ_INOUT_TYPE_OSDMD);
    _IMGRZ_Scale2(&rSclParam2);   
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();
 #else
   #ifdef IMGRZ_IMPROVE_TEST
    IMGRZ_Lock_Ex(E_SECND_RESIZER);
    IMGRZ_ReInit_Ex(E_SECND_RESIZER);
    if((GFX_INIT_MMU_VAL == prScaler->t_mmu.u4_init)&&(prScaler->t_mmu.u4_enable))
    {
        IMGRZ_Cfg_MMU_IO_Ex((UINT32)E_SECND_RESIZER,(BOOL)1,(BOOL)1);
    }
    IMGRZ_SetScaleOpt_Ex(E_SECND_RESIZER, E_RZ_INOUT_TYPE_OSDMD);
    IMGRZ_Scale_Ex(E_SECND_RESIZER, &rSclParam);
    IMGRZ_Flush_Ex(E_SECND_RESIZER);
    IMGRZ_Wait_Ex(E_SECND_RESIZER);
    IMGRZ_Unlock_Ex(E_SECND_RESIZER);
    #else
    IMGRZ_Lock();
    IMGRZ_ReInit();
    if((GFX_INIT_MMU_VAL == prScaler->t_mmu.u4_init)&&(prScaler->t_mmu.u4_enable))
    {
        IMGRZ_Cfg_MMU_IO((BOOL)1,(BOOL)1);
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_OSDMD);
    IMGRZ_Scale(&rSclParam);
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();
#endif
#endif
    return MTR_OK;
}

#ifdef CC_GPU_SUPPORT//#ifdef IMGRZ_IMPROVE_TEST
static MT_RESULT_T _MTGFX_ScalerComposition(unsigned long arg)
{

    RZ_OSD_SCL_PARAM_SET_T rSclParam;
    MTGFX_STRETCH_ALPHA_COMPOSITION_T rScaler;
    MTGFX_STRETCH_ALPHA_COMPOSITION_T *prScaler;
 
    prScaler = (MTGFX_STRETCH_ALPHA_COMPOSITION_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prScaler, MTGFX_STRETCH_ALPHA_COMPOSITION_T);
    COPY_FROM_USER_ARG(prScaler, rScaler, MTGFX_STRETCH_ALPHA_COMPOSITION_T);

    prScaler = &rScaler;
    
    NULL_PT_CHECK(prScaler);
    XYWH_CHECK(prScaler->i4SrcX, prScaler->i4SrcY, 
        prScaler->u4Width, prScaler->u4Height);
    XYWH_CHECK(prScaler->i4DstX, prScaler->i4DstY, 
        prScaler->u4DstWidth, prScaler->u4DstHeight);

     x_memset((void *)&rSclParam, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T)); 
    //COLOR_CHECK(prScaler->eSrcCm);
    //COLOR_CHECK(prScaler->eDstCm);
        rSclParam.u4SrcBase     = (UINT32) prScaler->pvSrc;
        rSclParam.u4IsSrcDirCM  = (UINT32) 1;
        rSclParam.u4SrcCM       = (UINT32) _GfxGetIMGRZColor(prScaler->eSrcCm);
        rSclParam.u4SrcBufLen   = (UINT32) D_RZ_ALIGN16(prScaler->u4SrcPitch);
        rSclParam.u4SrcHOffset  = (UINT32) prScaler->i4SrcX;
        rSclParam.u4SrcVOffset  = (UINT32) prScaler->i4SrcY;
        rSclParam.u4SrcW        = (UINT32) prScaler->u4Width;
        rSclParam.u4SrcH        = (UINT32) prScaler->u4Height;
    
        rSclParam.u4TgBase      = (UINT32) prScaler->pvDst;
        rSclParam.u4TgHOffset   = (UINT32) prScaler->i4DstX;
        rSclParam.u4TgVOffset   = (UINT32) prScaler->i4DstY;
        rSclParam.u4TgW         = (UINT32) prScaler->u4DstWidth;
        rSclParam.u4TgH         = (UINT32) prScaler->u4DstHeight;
        rSclParam.u4TgCM        = (UINT32) _GfxGetIMGRZColor(prScaler->eDstCm);
        rSclParam.u4TgBufLen    = (UINT32) D_RZ_ALIGN16(prScaler->u4DstPitch);
      #ifdef IMGRZ_IMPROVE_TEST
        rSclParam.fgOSDTurboMode = 1;
      #endif
    rSclParam.fgAlphaComposition = 1;
    rSclParam.u4AlphaComposition_Mode = (UINT32)rScaler.ePDRule;
    rSclParam.u4AlphaValue = (UINT32)rScaler.u1Alpha;
    rSclParam.u4PremultSrcR = (UINT32)rScaler.fgPremultSrc;
    rSclParam.u4PremultDstR = (UINT32)rScaler.fgPremultDst;
    rSclParam.u4PremultDstW = (UINT32)rScaler.fgPremultDstWr;

    #ifdef IMGRZ_IMPROVE_TEST
    IMGRZ_Lock_Ex(E_SECND_RESIZER);
    IMGRZ_ReInit_Ex(E_SECND_RESIZER);
    IMGRZ_SetScaleOpt_Ex(E_SECND_RESIZER, E_RZ_INOUT_TYPE_OSDMD);
    IMGRZ_Scale_Ex(E_SECND_RESIZER, &rSclParam);
    IMGRZ_Flush_Ex(E_SECND_RESIZER);
    IMGRZ_Wait_Ex(E_SECND_RESIZER);
    IMGRZ_Unlock_Ex(E_SECND_RESIZER);
    #endif

	return MTR_OK;
}
#endif
/**Perform scaling rotation operation
*@param  prScaler                  GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTVGFX_Scaler(unsigned long arg)
{
    MTVGFX_PARAM_SET_T* pr_vgfx_sclr;
    MTVGFX_PARAM_SET_T  r_vgfx_sclr;     

    pr_vgfx_sclr = (MTVGFX_PARAM_SET_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pr_vgfx_sclr, MTVGFX_PARAM_SET_T);
    COPY_FROM_USER_ARG(pr_vgfx_sclr, r_vgfx_sclr, MTVGFX_PARAM_SET_T);

    pr_vgfx_sclr = &r_vgfx_sclr;

    // _VGfx_SetVGfxScaler(( VGFX_SCALER_PARAM_SET_T *)pr_vgfx_sclr);

    return MTR_OK;
}

/**Perform scaling rotation operation
*@param  prScaler                  GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTVGFX_FlushDCache(unsigned long arg)
{
    MTGFX_CACHE_DATA* pt_this;
    MTGFX_CACHE_DATA  t_this;     

    pt_this = (MTGFX_CACHE_DATA*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pt_this, MTGFX_CACHE_DATA);
    COPY_FROM_USER_ARG(pt_this, t_this, MTGFX_CACHE_DATA);

    pt_this = &t_this;
    
    // printf("_MTVGFX_FlushDCache[0x%x,0x%x,0x%x]\n",pt_this->u4_fg,pt_this->u4_addr,pt_this->u4_size);
    
    if(pt_this->u4_fg & MTGFX_FLUSH_DCACHE)
    {
        HalFlushDCacheMultipleLine(pt_this->u4_addr,pt_this->u4_size);
    }

    if(pt_this->u4_fg & MTGFX_INVALID_DCACHE)
    {
        HalInvalidateDCacheMultipleLine(pt_this->u4_addr,pt_this->u4_size);
    }

    return MTR_OK;
}

static VOID _IMG_Resize(MTGFX_IMG_REGION_T  *prSrc, MTGFX_IMG_REGION_T *prDst, UINT32 u4IsVDO, UINT32 u4CM)
{
    RZ_OSD_SCL_PARAM_SET_T rScaleParam;
    RZ_VDO_SCL_PARAM_SET_T rScaleParamV;

    IMGRZ_Lock();
    IMGRZ_ReInit();
    
    if (!u4IsVDO)
    {   
         x_memset(&rScaleParam,0,sizeof(RZ_OSD_SCL_PARAM_SET_T));
         
        rScaleParam.u4SrcBase = (UINT32)prSrc->pu1Addr;
        rScaleParam.u4IsSrcDirCM = 1;
        rScaleParam.u4SrcCM = u4CM;        
        rScaleParam.u4SrcHOffset = prSrc->u4StartX;//0;
        rScaleParam.u4SrcVOffset = prSrc->u4StartY;//0;
        rScaleParam.u4SrcW = prSrc->u4Width;
        rScaleParam.u4SrcH = prSrc->u4Height;
        rScaleParam.u4TgBase = (UINT32)prDst->pu1Addr;
        rScaleParam.u4TgCM = rScaleParam.u4SrcCM;
        /*if (u4CM <= E_RZ_OSD_DIR_CM_ARGB4444)               //16bpp
        {
            rScaleParam.u4SrcBufLen = prSrc->u4Width << 1;
            rScaleParam.u4TgBufLen = prDst->u4Width << 1;
        }
        else                                                //24bpp
        {
            rScaleParam.u4SrcBufLen = prSrc->u4Width << 2;
            rScaleParam.u4TgBufLen = prDst->u4Width << 2;
        } */       
        rScaleParam.u4SrcBufLen = prSrc->u4Pitch;
        rScaleParam.u4TgBufLen = prDst->u4Pitch;
        rScaleParam.u4TgHOffset = prDst->u4StartX;//0;
        rScaleParam.u4TgVOffset = prDst->u4StartY;//0;
        rScaleParam.u4TgW = prDst->u4Width;
        rScaleParam.u4TgH = prDst->u4Height;
        IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_OSDMD);
        IMGRZ_Scale((void *)(&rScaleParam));   
    }
    else
    {
        
        UINT32  u4DestAddr = (UINT32)prDst->pu1Addr + prDst->u4StartX + prDst->u4StartY * prDst->u4Pitch;
        UINT32  u4SrcAddr  = (UINT32)prSrc->pu1Addr + prSrc->u4StartX + prSrc->u4StartY * prSrc->u4Pitch;
        
        HalFlushInvalidateDCacheMultipleLine(u4DestAddr, prDst->u4Pitch * prDst->u4Height);
        HalFlushInvalidateDCacheMultipleLine(u4DestAddr + prDst->u4CbCrOffset, prDst->u4Pitch*prDst->u4Height);
        HalFlushInvalidateDCacheMultipleLine(u4SrcAddr,  prSrc->u4Pitch * prSrc->u4Height);
        HalFlushInvalidateDCacheMultipleLine(u4SrcAddr + prSrc->u4CbCrOffset, prSrc->u4Pitch * prSrc->u4Height);        
		
         x_memset(&rScaleParamV,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));    
        rScaleParamV.u4IsRsIn = 1;        
        rScaleParamV.u4InMode = u4CM;
        rScaleParamV.u4YSrcBase = gfx_va_to_pa((UINT32)prSrc->pu1Addr);
        rScaleParamV.u4YSrcBufLen = prSrc->u4Pitch;//prSrc->u4Width;
        rScaleParamV.u4CSrcBufLen = prSrc->u4Pitch;//prSrc->u4Width;
        rScaleParamV.u4YSrcHOffset = prSrc->u4StartX;//0;
        rScaleParamV.u4YSrcVOffset = prSrc->u4StartY;//0;
        rScaleParamV.u4YSrcW = prSrc->u4Width;
        rScaleParamV.u4YSrcH = prSrc->u4Height;
        rScaleParamV.u4CSrcBase = gfx_va_to_pa((UINT32)prSrc->pu1Addr + prSrc->u4CbCrOffset);
        rScaleParamV.u4CSrcHOffset = prSrc->u4StartX >> 1;//0;
        rScaleParamV.u4CSrcVOffset = prSrc->u4StartY >> (1 - u4CM);//0;
        rScaleParamV.u4IsRsOut = 1;
        rScaleParamV.u4OutMode = u4CM;
        rScaleParamV.u4IsVdo2Osd = 0;     
        rScaleParamV.u4YTgBase = gfx_va_to_pa((UINT32)prDst->pu1Addr);       
        rScaleParamV.u4YTgCM = u4CM;
        rScaleParamV.u4YTgBufLen = prDst->u4Pitch;//prDst->u4Width;
        rScaleParamV.u4CTgBufLen = prDst->u4Pitch;//prDst->u4Width;
        rScaleParamV.u4YTgHOffset = ((prDst->u4StartX >> 1) << 1);//0;
        rScaleParamV.u4YTgVOffset = prDst->u4StartY;//0;
        rScaleParamV.u4YTgW = prDst->u4Width;
        rScaleParamV.u4YTgH = prDst->u4Height;
        rScaleParamV.u4CTgBase = gfx_va_to_pa((UINT32)prDst->pu1Addr + prDst->u4CbCrOffset);
        if (u4CM == E_RZ_VDO_OUTMD_420)
        {
            rScaleParamV.u4CSrcW = prSrc->u4Width >> 1;
            rScaleParamV.u4CSrcH = prSrc->u4Height >> 1;
            rScaleParamV.u4CTgW = prDst->u4Width >> 1;
            rScaleParamV.u4CTgH = prDst->u4Height >> 1;
        }
        else                        //422
        {
            rScaleParamV.u4CSrcW = prSrc->u4Width >> 1;
            rScaleParamV.u4CSrcH = prSrc->u4Height;
            rScaleParamV.u4CTgW = prDst->u4Width >> 1;
            rScaleParamV.u4CTgH = prDst->u4Height;
        }
        rScaleParamV.u4SrcSwap = 0;
        rScaleParamV.u4OutSwap = 0;
        
        IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_VDOMD);
        IMGRZ_Scale((void *)(&rScaleParamV));   
    }    
    IMGRZ_Flush();
    IMGRZ_Wait();    
    IMGRZ_Unlock();
    
}



static MT_RESULT_T _MTGFX_VDOResize(unsigned long arg)
{
    MTGFX_IMG_REGION_T   *prSrc,  *prDst, rSrc, rDst;
    UINT32                u4CM;
    MTAL_IOCTL_3ARG_T     rArg;
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    prSrc = (MTGFX_IMG_REGION_T*) rArg.ai4Arg[0];
    prDst = (MTGFX_IMG_REGION_T*) rArg.ai4Arg[1];
    u4CM  = (UINT32) rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prSrc, MTGFX_IMG_REGION_T);
    COPY_FROM_USER_ARG(prSrc, rSrc, MTGFX_IMG_REGION_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(prDst, MTGFX_IMG_REGION_T);
    COPY_FROM_USER_ARG(prDst, rDst, MTGFX_IMG_REGION_T);

#if 0
    printf("src:\n"
    "pu1Addr = 0x%x \n"
    "u4CbCrOffset = %d \n"
    "u4Depth = %d \n"
    "u4Pitch = %d \n"
    "u4TotalLine = %d \n"
    "u4StartX = %d \n"
    "u4StartY = %d \n"
    "u4Width = %d \n"
    "u4Height = %d \n"
    "ucPicStruct = %d \n"
    "u4DecRatio = %d \n", 
    rSrc.pu1Addr, rSrc.u4CbCrOffset, rSrc.u4Depth, rSrc.u4Pitch, rSrc.u4TotalLine, rSrc.u4StartX, rSrc.u4StartY,
    rSrc.u4Width, rSrc.u4Height, rSrc.ucPicStruct, rSrc.u4DecRatio);

    printf("dst:\n"
    "pu1Addr = 0x%x \n"
    "u4CbCrOffset = %d \n"
    "u4Depth = %d \n"
    "u4Pitch = %d \n"
    "u4TotalLine = %d \n"
    "u4StartX = %d \n"
    "u4StartY = %d \n"
    "u4Width = %d \n"
    "u4Height = %d \n"
    "ucPicStruct = %d \n"
    "u4DecRatio = %d \n", 
    rDst.pu1Addr, rDst.u4CbCrOffset, rDst.u4Depth, rDst.u4Pitch, rDst.u4TotalLine, rDst.u4StartX, rDst.u4StartY,
    rDst.u4Width, rDst.u4Height, rDst.ucPicStruct, rDst.u4DecRatio);    
#endif
    _IMG_Resize(&rSrc, &rDst, 1, u4CM);

    return MTR_OK;

}


/**Perform GFX stretch blit operation
*@param  prScaler                  GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_StretchBitblt(unsigned long arg)
{
    MTGFX_SCALER_T rScaler;
    MTGFX_SCALER_T *prScaler;
 
    prScaler = (MTGFX_SCALER_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prScaler, MTGFX_SCALER_T);
    COPY_FROM_USER_ARG(prScaler, rScaler, MTGFX_SCALER_T);

    prScaler = &rScaler;
    
    NULL_PT_CHECK(prScaler);
    XYWH_CHECK(prScaler->i4SrcX, prScaler->i4SrcY, 
        prScaler->u4Width, prScaler->u4Height);
    XYWH_CHECK(prScaler->i4DstX, prScaler->i4DstY, 
        prScaler->u4DstWidth, prScaler->u4DstHeight);

    //COLOR_CHECK(prScaler->eSrcCm);
    //COLOR_CHECK(prScaler->eDstCm);

    GFX_Lock();
    if(GFX_INIT_MMU_VAL == prScaler->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prScaler->t_mmu);
    }
    GFX_SetSrc((UINT8 *)rScaler.pvSrc, _GfxGetColorMode(rScaler.eSrcCm), rScaler.u4SrcPitch);
    GFX_SetDst((UINT8 *)rScaler.pvDst, _GfxGetColorMode(rScaler.eDstCm), rScaler.u4DstPitch);
    GFX_SetBltOpt(0, 0, 0);
    GFX_StretchBlt(rScaler.i4SrcX,rScaler.i4SrcY,rScaler.u4Width,rScaler.u4Height,
                         rScaler.i4DstX,rScaler.i4DstY,rScaler.u4DstWidth,rScaler.u4DstHeight);
    GFX_DBG_FLUSH();
    GFX_Unlock();
    
    return MTR_OK;
}

/**Perform GFX stretch blit operation
*@param  prScaler                  GFX scaling operation structure
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK             Fail.
*/
static MT_RESULT_T _MTGFX_StretchAlphaComposition(unsigned long arg)
{
    MTGFX_STRETCH_ALPHA_COMPOSITION_T rScaler;
    MTGFX_STRETCH_ALPHA_COMPOSITION_T *prScaler;
 
    prScaler = (MTGFX_STRETCH_ALPHA_COMPOSITION_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prScaler, MTGFX_STRETCH_ALPHA_COMPOSITION_T);
    COPY_FROM_USER_ARG(prScaler, rScaler, MTGFX_STRETCH_ALPHA_COMPOSITION_T);

    prScaler = &rScaler;
    
    NULL_PT_CHECK(prScaler);
    XYWH_CHECK(prScaler->i4SrcX, prScaler->i4SrcY, 
        prScaler->u4Width, prScaler->u4Height);
    XYWH_CHECK(prScaler->i4DstX, prScaler->i4DstY, 
        prScaler->u4DstWidth, prScaler->u4DstHeight);

    //COLOR_CHECK(prScaler->eSrcCm);
    //COLOR_CHECK(prScaler->eDstCm);

    GFX_Lock();
    if(GFX_INIT_MMU_VAL == prScaler->t_mmu.u4_init)
    {
        GFX_MMU_Set_Cfg(&prScaler->t_mmu);
    }
    GFX_SetSrc((UINT8 *)rScaler.pvSrc, _GfxGetColorMode(rScaler.eSrcCm), rScaler.u4SrcPitch);
    GFX_SetDst((UINT8 *)rScaler.pvDst, _GfxGetColorMode(rScaler.eDstCm), rScaler.u4DstPitch);
    GFX_SetBltOpt(0, 0, 0);
    GFX_SetPremult((UINT32)prScaler->fgPremultSrc, (UINT32)prScaler->fgPremultDst, (UINT32)prScaler->fgPremultDst, 1);
    GFX_StretchAlphaCom( rScaler.i4SrcX,rScaler.i4SrcY,rScaler.u4Width,rScaler.u4Height,
                         rScaler.i4DstX,rScaler.i4DstY,rScaler.u4DstWidth,rScaler.u4DstHeight, 
                                        rScaler.u1Alpha, rScaler.ePDRule);
    
    GFX_DBG_FLUSH();
    GFX_Unlock();
    
    return MTR_OK;
}

static MT_RESULT_T _MTGFX_SetFlipMirror(unsigned long arg)
{
    BOOL fgMirrorEn, fgFlipEn;
    MTAL_IOCTL_2ARG_T rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    fgMirrorEn = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgFlipEn = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT("  - fgMirrorEn = %d, fgFlipEn = %d\n", (int)fgMirrorEn, (int)fgFlipEn);

    // wait to sync to 98
    #if 0//def CC_MT5398
        vSetGFXFlipMirror(fgMirrorEn, fgFlipEn);
    #else
        return MTR_OK;
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTGFX_CompressBlt(unsigned long arg)
{
    MTGFX_COMPRESS_T rFill;
    MTGFX_COMPRESS_T* prCompress;
    prCompress = (MTGFX_COMPRESS_T*)arg;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prCompress,MTGFX_COMPRESS_T);
    COPY_FROM_USER_ARG(prCompress,rFill,MTGFX_COMPRESS_T);

    //add driver implementation code here
    prCompress = &rFill;
    NULL_PT_CHECK(prCompress);
    XYWH_CHECK(prCompress->u4SrcX, prCompress->u4SrcX, 
        prCompress->u4Width, prCompress->u4Height);
    MTGFX_PRINT("IO src=0x%x, dst=0x%x, pitch=%d, u4SrcX=%d, u4SrcY=%d, u4Width=%d, u4Height=%d\n",
            prCompress->u4Src, prCompress->u4Dst, prCompress->u4Pitch, 
            prCompress->u4SrcX, prCompress->u4SrcY, prCompress->u4Width, prCompress->u4Height);

    GFX_Lock();
    if(GFX_INIT_MMU_VAL == prCompress->t_mmu.u4_init)
    {
        prCompress->t_mmu.u4_cmprss = OP_NEW_COMPRESS_BITBLT;
        GFX_MMU_Set_Cfg(&prCompress->t_mmu);
    }
    
    GFX_NewCompressBlt((UINT8 *)prCompress->u4Src, prCompress->u4SrcX, prCompress->u4SrcY, prCompress->eCm, 
    prCompress->u4Pitch, (UINT8 *)prCompress->u4Dst, prCompress->u4Width, prCompress->u4Height, prCompress->u4Height,
    1, 0, 0xFFFFFFFF, 0, 0, 0,(void *)&(prCompress->t_mmu));
    GFX_Flush();
    GFX_Wait();    
    GFX_Unlock();        
    return MTR_OK;
}
extern UINT32 _au4MwUiStatus;

static MT_RESULT_T _MTGFX_Set3DSbs(unsigned long arg)
{
#if defined(CC_CONFIG_OSD_SBS)
     GFX_3D_DATA_T r3DData;
     GFX_3D_DATA_T* pr3dData;
     //UINT32 eColorMode; 

     pr3dData = (GFX_3D_DATA_T*)arg;
     USR_SPACE_ACCESS_VALIDATE_ARG(pr3dData,GFX_3D_DATA_T);
    COPY_FROM_USER_ARG(pr3dData,r3DData,GFX_3D_DATA_T);
    pr3dData = &r3DData;
    NULL_PT_CHECK(pr3dData);
    XYWH_CHECK(pr3dData->i4SrcX, pr3dData->i4SrcY, 
        pr3dData->u4Width, pr3dData->u4Height);
    XYWH_CHECK(pr3dData->i4DstX, pr3dData->i4DstY, 
        pr3dData->u4Width, pr3dData->u4Height);

    //eColorMode = _MTOSD_GetDrvColorMode(pr3dData->e_dst_cm);
    if((_au4MwUiStatus == 1) || (_au4MwUiStatus ==3))
    {
      pr3dData->e_mode = GFX_3D_OSD_SBS;
    }
    else
    {
       pr3dData->e_mode = GFX_3D_OSD_NOTMAL;
    }

    MTGFX_PRINT(" -  mode =%d, pr3dData->ui4_bmp_width =%d, pr3dData->ui4_bmp_height=%d, pr3dData->u4DstPitch=%d,  \
        pr3dData->pvDst =0x%x, eColorMode =%d, pr3dData->pvSrc = 0x%x\n",  pr3dData->e_mode, pr3dData->ui4_bmp_width,
        pr3dData->ui4_bmp_height, pr3dData->u4DstPitch,
        pr3dData->pvDst, pr3dData->eDstCm, pr3dData->pvSrc);
    OSD_PLA_Set3DDFB(0, pr3dData->e_mode, 0, pr3dData->ui4_bmp_width, pr3dData->ui4_bmp_height, pr3dData->u4DstPitch, 
                           (UINT32)pr3dData->pvDst, pr3dData->eDstCm, pr3dData->pvSrc);
    
#endif
    return MTR_OK;
}

int mtal_ioctl_mtgfx(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    switch (cmd) 
    {
        case MTAL_IO_GFX_RESET:
            return _MTGFX_Reset();
        case MTAL_IO_GFX_WAIT:
            return _MTGFX_Wait(arg);
        case MTAL_IO_GFX_FLUSH:
            return _MTGFX_Flush();
        case MTAL_IO_GFX_LOCK:
            return _MTGFX_Lock(arg);
        case MTAL_IO_GFX_UNLOCK:
            return _MTGFX_UnLock();
        case MTAL_IO_GFX_FILLRECT:
            return _MTGFX_FillRect(arg);
        case MTAL_IO_GFX_DRAWHLINE:
            return _MTGFX_DrawHLine(arg);
        case MTAL_IO_GFX_DRAWVLINE:
            return _MTGFX_DrawVLine(arg);
        case MTAL_IO_GFX_BITBLT:
            return _MTGFX_Bitblt(arg);
        case MTAL_IO_GFX_TRANSPARENTBITBLT:
            return _MTGFX_TransparentBitblt(arg);
        case MTAL_IO_GFX_TRANSPARENTFILL:
            return _MTGFX_TransparentFill(arg);
        case MTAL_IO_GFX_ALPHABLENDING:
            return _MTGFX_AlphaBlending(arg);
        case MTAL_IO_GFX_YBR2RGB:
            return _MTGFX_Ybr2Rgb(arg);
        case MTAL_IO_GFX_ALPHAMAPBITBLT:
            return _MTGFX_AlphaMapBitblt(arg);
        case MTAL_IO_GFX_COLORCONVERSION:
    		return _MTGFX_ColorFormatConversion(arg);
        case MTAL_IO_GFX_ROPBITBLT:
            return _MTGFX_RopBitblt(arg);
        case MTAL_IO_GFX_ALPHACOMPOSITION:
            return _MTGFX_AlphaComposition(arg);
        case MTAL_IO_GFX_IDX2DIRBITBLT:
            return _MTGFX_Idx2DirBitblt(arg);
        case MTAL_IO_GFX_ROTATE90:
            return _MTGFX_Rotate90(arg);
        case MTAL_IO_GFX_GETBUFFERPROPERTIES:
            return _MTGFX_GetBufferProperties(arg); 
        case MTAL_IO_GFX_GETBUFFERPATITION:
            return _MTGFX_GetBufferPartition(arg);  
        case MTAL_IO_GFX_GET_FBM:
            return _MTGFX_Get_FBM_Buffer(arg);
        case MTAL_IO_GFX_SCALER:
            return _MTGFX_Scaler(arg);
#ifdef CC_SUPPORT_TVE_OSD_OUT           
        case MTAL_IO_GFX_RGB2YBR:
            return _MTGFX_Rgb2Ybr(arg);
#endif          
        case MTAL_IO_GFX_STRETCHBITBLT:
                return _MTGFX_StretchBitblt(arg);
        case MTAL_IO_GFX_STRETCHALPHACOMPOSITION:
                return _MTGFX_StretchAlphaComposition(arg);
        case MTAL_IO_GFX_ALPHACOMPOSITION_IMPROVE:
            return _MTGFX_AlphaCompositionImprove(arg);
        case MTAL_IO_VGFX_SCALER:
            return _MTVGFX_Scaler(arg);
        case MTAL_IO_GFX_VDORESIZE:
            return _MTGFX_VDOResize(arg);
        case MTAL_IO_GFX_FLUSH_DCAHE:
            return _MTVGFX_FlushDCache(arg);
        case MTAL_IO_GFX_SET_MMU:
            return _MTGFX_GFX_MMU_Set_Enable(arg);  
        case MTAL_IO_GFX_SET_FLIPMIRROR:
            return _MTGFX_SetFlipMirror(arg);
        case MTAL_IO_GFX_COMPRESSBLIT:
            return _MTGFX_CompressBlt(arg);
#ifdef CC_GPU_SUPPORT
        case MTAL_IO_GFX_SCALERCOMPOSITION:
            return _MTGFX_ScalerComposition(arg);
#endif
        case MTAL_IO_GFX_SET_SBS:
                    return _MTGFX_Set3DSbs(arg);

        default:
                return MTR_NOT_SUPPORTED;
    }
    return MTR_OK;
}

static UINT32 CheckPTE(struct mm_struct* mm, unsigned long address, UINT32* pgd_o, UINT32* ptep_o)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep, pte;
    spinlock_t *ptl = (spinlock_t *)NULL;

    /* check PTE */
    pgd = pgd_offset(mm, address);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
    {
        GFX_PRINT("CheckPTE 1\n");
        goto no_page_table;
    }

#if 0
    HalFlushDCacheMultipleLine((UINT32)pgd, 4);
#else
    *pgd_o = (UINT32)pgd;
#endif

    pud = pud_offset(pgd, address);
    if (pud_none(*pud))
    {
        GFX_PRINT("CheckPTE 2\n");
        goto no_page_table;
    }

    if (unlikely(pud_bad(*pud)))
    {
        GFX_PRINT("CheckPTE 3\n");
        goto no_page_table;
    }

    pmd = pmd_offset(pud, address);
    if (pmd_none(*pmd))
    {
        GFX_PRINT("CheckPTE 4\n");
        goto no_page_table;
    }
    
    if (unlikely(pmd_bad(*pmd)))
    {
        GFX_PRINT("CheckPTE 5\n");
        goto no_page_table;
    }

    ptep = pte_offset_map_lock(mm, pmd, address, &ptl);
    pte = *ptep;
    if (!pte_present(pte))
    {
        GFX_PRINT("CheckPTE 6\n");
        goto no_page;
    }
#if 0
    HalFlushDCacheMultipleLine((UINT32)ptep, 4);
    HalFlushDCacheMultipleLine((UINT32)ptep-0x800, 4);
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
    *ptep_o = (UINT32)ptep+0x800;
#else
    *ptep_o = (UINT32)ptep-0x800;
#endif
#endif

    pte_unmap_unlock(ptep, ptl);

    return TRUE;

no_page:
    pte_unmap_unlock(ptep, ptl);
no_page_table:

    return FALSE;
}


static int mxx_map_user_pages(MTIOMMU_PIN_RANGE_T* pinRange)
{
    int nr_pages;
    unsigned long first,last;
    struct page **pages;
    int res, j;
    unsigned long uaddr, size;
    unsigned long start_chk_addr, end_chk_addr, addr;
    UINT32 pgd_p=0, ptep_p=0, old_pgd_p=0, x_pte=0, sz_pte=0;

    uaddr = pinRange->start;
    size = pinRange->size;

    first = (uaddr & PAGE_MASK) >> PAGE_SHIFT;
    last  = ((uaddr + size) & PAGE_MASK) >> PAGE_SHIFT;

    // nr_pages = last-first+1;
    
    if(0x0 == ((uaddr+size)%0x1000))
    {
        nr_pages = last-first;
    }
    else
    {
        nr_pages = last-first+1;
    }
    
    #if 1
    if((pages = kmalloc(nr_pages * sizeof(*pages), GFP_KERNEL)) == NULL)
    {
        return -ENOMEM;
    }
    
    pinRange->pageArray = (UINT32)pages;

    /* Try to fault in all of the necessary pages */
    down_read(&current->mm->mmap_sem);
    /* rw==READ means read from drive, write into memory area */
    res = get_user_pages(
            current,
            current->mm,
            uaddr,
            nr_pages,
            1, /* todo: write */
            0, /* don't force */
            pages,
            NULL);
    up_read(&current->mm->mmap_sem);

    pinRange->nrPages = res;
    /* Errors and no page mapped should return here */
    //if (res < nr_pages)
        //goto out_unmap;
    #endif
    /* checking the pte directly */
    start_chk_addr = uaddr;
    end_chk_addr = uaddr + size-1;
    addr = start_chk_addr;

    while (1)
    {
        if ((addr < start_chk_addr) || (addr > end_chk_addr))
        {
            if (sz_pte != 0)
            {
                HalFlushDCacheMultipleLine((UINT32)x_pte, sz_pte);
            }
            break;
        }

        if(CheckPTE(current->mm, addr, &pgd_p, &ptep_p) != TRUE)
        {
            printk("<5> [yjdbg] _MTMMU_PinMem: (start: 0x%x, size: 0x%x)\n", pinRange->start, pinRange->size);
            printk("<5> check PTE at address 0x%x: fail\n", addr);
        }

        if (pgd_p != old_pgd_p)
        {
            HalFlushDCacheMultipleLine((UINT32)pgd_p, 4);
            old_pgd_p = pgd_p;
        }
        if (ptep_p != x_pte+sz_pte)
        {
            if (sz_pte != 0)
            {
            HalFlushDCacheMultipleLine((UINT32)x_pte, sz_pte);
            }
            x_pte = ptep_p;
            sz_pte = 4;
        }
        else
        {
            sz_pte += 4;
        }

        addr += 0x1000;
    }

    //printk("<0> (yjdbg) mxx_map_user_pages success: (start: 0x%x, size: 0x%x, #pages: 0x%x)\n", pinRange->start, pinRange->size, pinRange->nrPages);
    if (res < nr_pages)
        goto out_unmap;

    return nr_pages;
#if 1
out_unmap:
    //printk("<5> get_user_pages fail\n");
    if (res > 0) {
        for (j=0; j < res; j++)
            put_page(pages[j]);
        res = 0;
    }
    kfree(pages);
    return res;
#endif
}
int gfx_mxx_map_user_pages(unsigned long arg)
{
    return mxx_map_user_pages((MTIOMMU_PIN_RANGE_T*)arg);
}

static void mxx_unmap_user_pages(MTIOMMU_PIN_RANGE_T* pinRange)
{
    struct page **pages;
    int nr_pages, i;

    //printk("<0> (yjdbg) mxx_unmap_user_pages success: (start: 0x%x, size: 0x%x, #pages: 0x%x)\n", pinRange->start, pinRange->size, pinRange->nrPages);

    pages = (struct page**) pinRange->pageArray;
    nr_pages    = pinRange->nrPages;

    for(i=0; i < nr_pages; i++)
    {
        put_page(pages[i]);
    }
    
    kfree(pages);

    return;
}
void gfx_mxx_unmap_user_pages(unsigned long arg)
{
    return mxx_unmap_user_pages((MTIOMMU_PIN_RANGE_T*)arg);
}


int _MTMMU_PinMem(unsigned long arg)
{
    int res;
    MTIOMMU_PIN_RANGE_T* prPinRange;
    MTIOMMU_PIN_RANGE_T rPinRange;

    prPinRange = (MTIOMMU_PIN_RANGE_T*) arg;
   
    USR_SPACE_ACCESS_VALIDATE_ARG(prPinRange, MTIOMMU_PIN_RANGE_T);
    COPY_FROM_USER_ARG(prPinRange, rPinRange, MTIOMMU_PIN_RANGE_T);

    //printk("<0> [yjdbg] _MTMMU_PinMem: (start: 0x%x, size: 0x%x)\n", prPinRange->start, prPinRange->size);
    res = mxx_map_user_pages(&rPinRange);

    COPY_TO_USER_ARG(prPinRange,rPinRange,MTIOMMU_PIN_RANGE_T);
    
    // HalFlushInvalidateDCache();
    //GFX_Task_Set_Mmu_Fire(TRUE);

    if (res > 0)
    {        
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

int _MTMMU_UnPinMem(unsigned long arg)
{
    MTIOMMU_PIN_RANGE_T* prPinRange;
    MTIOMMU_PIN_RANGE_T rPinRange;

    prPinRange = (MTIOMMU_PIN_RANGE_T*) arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prPinRange, MTIOMMU_PIN_RANGE_T);
    COPY_FROM_USER_ARG(prPinRange, rPinRange, MTIOMMU_PIN_RANGE_T);

    //printk("<0> [yjdbg] _MTMMU_UnPinMem: (start: 0x%x, size: 0x%x)\n", prPinRange->start, prPinRange->size);
    mxx_unmap_user_pages(&rPinRange);

    COPY_TO_USER_ARG(prPinRange,rPinRange,MTIOMMU_PIN_RANGE_T);
    //GFX_Task_Set_Mmu_Fire(TRUE);

    return MTR_OK;
}

static enum dma_data_direction _MTMMU_DMA_DIR_TRANSLATION(MTIOMMU_DMA_DIRECTION_T dir)
{
    enum dma_data_direction res;

    switch (dir)
    {
        case MTIOMMU_DMA_BIDIRECTION:
            res = HAL_DMA_BIDIRECTION;
            break;
        case MTIOMMU_DMA_TO_DEVICE:
            res = HAL_DMA_TO_DEVICE;
            break;
        case MTIOMMU_DMA_FROM_DEVICE:
            res = HAL_DMA_FROM_DEVICE;
            break;
        default:
            ASSERT(0);
            res = HAL_DMA_BIDIRECTION;
    }

    return res;
}

MT_RESULT_T _MTMMU_DMA_MAPUNMAP_RANGE(UINT32 maponoff, MTIOMMU_DMA_RANGE_T* prDMARange, MTIOMMU_PIN_RANGE_T* prPinRange)
{
    enum dma_data_direction dir;
    struct page **pages;
    int nr_pages, end_page, i;
    UINT32 start, size, partStart, partSize;

    start = prDMARange->start;
    size = prDMARange->size;
    dir = _MTMMU_DMA_DIR_TRANSLATION(prDMARange->direction);
    //printk("<0> (yjdbg) _MTMMU_DMA_MAP_SINGLE: (dma start: 0x%x, dma size: 0x%x, dir: 0x%x)\n", prDMARange->start, prDMARange->size, prDMARange->direction);

    pages = (struct page**) prPinRange->pageArray;
    nr_pages = prPinRange->nrPages;

    ASSERT(prDMARange->start >= prPinRange->start && "DMA range is not under the pin range");
    ASSERT(((prDMARange->start+prDMARange->size) <= (prPinRange->start+prPinRange->size)) && "DMA range is not under the pin range");

    i = 0;
    i += (prDMARange->start >> PAGE_SHIFT) - (prPinRange->start >> PAGE_SHIFT);
    //printk("<0> (yjdbg) _MTMMU_DMA_MAP_SINGLE: (pin start: 0x%x, pin size: 0x%x)\n", prPinRange->start, prPinRange->size);
    //prDMARange->dma_addr = dma_map_single(NULL, (void*)prDMARange->start, prDMARange->size, dir); // need physical continuous address

    partStart = start;
    // the first page
    if (start & ~PAGE_MASK)
    {
        UINT32 sizeA, sizeB;

        partStart = start;
        sizeA = size;
        sizeB = PAGE_SIZE - (partStart & ~PAGE_MASK);
        if (sizeA <= sizeB)
        {
            partSize = sizeA;
            //printk("<0>(yjdbg)-1 virstart: 0x%x, phystart: 0x%x, size: 0x%x\n", partStart, ((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize);
            if (maponoff)
            {
                HalIOMMUDmaBufferStart((void*)partStart, (void*)((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize, dir);
            }
            else
            {
                HalIOMMUDmaBufferDone((void*)partStart, (void*)((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize, dir);
            }
            return MTR_OK;;
        }
        else
        {
            partSize = sizeB;
            //printk("<0>(yjdbg)-1 virstart: 0x%x, phystart: 0x%x, size: 0x%x\n", partStart, ((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize);
            if (maponoff)
            {
                HalIOMMUDmaBufferStart((void*)partStart, (void*)((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize, dir);
            }
            else
            {
                HalIOMMUDmaBufferDone((void*)partStart, (void*)((page_to_pfn(pages[i])<<PAGE_SHIFT) + (partStart & ~PAGE_MASK)), partSize, dir);
            }
            partStart += partSize;
        }
        i++;
    }

    end_page = ((start + size) >> PAGE_SHIFT) - (prPinRange->start >> PAGE_SHIFT) + 1;
    // pages in the middle
    for (; i < end_page-1; i++)
    {
        //dma_map_page(NULL, pages[i], 0, PAGE_SIZE, dir);
        //printk("<0>(yjdbg)-2 virstart: 0x%x, phystart: 0x%x, size: 0x%x\n", partStart, (page_to_pfn(pages[i])<<PAGE_SHIFT), PAGE_SIZE);
        if (maponoff)
        {
            HalIOMMUDmaBufferStart((void*)partStart, (void*)(page_to_pfn(pages[i])<<PAGE_SHIFT), PAGE_SIZE, dir);
        }
        else
        {
            HalIOMMUDmaBufferDone((void*)partStart, (void*)(page_to_pfn(pages[i])<<PAGE_SHIFT), PAGE_SIZE, dir);
        }
        partStart += PAGE_SIZE;
    }

    // the last page
    if (i < end_page)
    {
        partSize = start + size - partStart;
        //printk("<0>(yjdbg)-3 virstart: 0x%x, phystart: 0x%x, size: 0x%x\n", partStart, (page_to_pfn(pages[i])<<PAGE_SHIFT), partSize);
        if (maponoff)
        {
            HalIOMMUDmaBufferStart((void*)partStart, (void*)(page_to_pfn(pages[i])<<PAGE_SHIFT), partSize, dir);
        }
        else
        {
            HalIOMMUDmaBufferDone((void*)partStart, (void*)(page_to_pfn(pages[i])<<PAGE_SHIFT), partSize, dir);
        }
    }

    return MTR_OK;
}

int _MTMMU_DMA_MAP_SINGLE(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTIOMMU_DMA_RANGE_T* prDMARange;
    MTIOMMU_DMA_RANGE_T rDMARange;
    MTIOMMU_PIN_RANGE_T* prPinRange;
    MTIOMMU_PIN_RANGE_T rPinRange;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    prDMARange = (MTIOMMU_DMA_RANGE_T*)rArg.ai4Arg[0];
    prPinRange = (MTIOMMU_PIN_RANGE_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prDMARange, MTIOMMU_DMA_RANGE_T);
    COPY_FROM_USER_ARG(prDMARange, rDMARange, MTIOMMU_DMA_RANGE_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(prPinRange, MTIOMMU_PIN_RANGE_T);
    COPY_FROM_USER_ARG(prPinRange, rPinRange, MTIOMMU_PIN_RANGE_T);

    return _MTMMU_DMA_MAPUNMAP_RANGE(1, &rDMARange, &rPinRange);
}

int _MTMMU_DMA_UNMAP_SINGLE(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTIOMMU_DMA_RANGE_T* prDMARange;
    MTIOMMU_DMA_RANGE_T rDMARange;
    MTIOMMU_PIN_RANGE_T* prPinRange;
    MTIOMMU_PIN_RANGE_T rPinRange;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    prDMARange = (MTIOMMU_DMA_RANGE_T*)rArg.ai4Arg[0];
    prPinRange = (MTIOMMU_PIN_RANGE_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prDMARange, MTIOMMU_DMA_RANGE_T);
    COPY_FROM_USER_ARG(prDMARange, rDMARange, MTIOMMU_DMA_RANGE_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(prPinRange, MTIOMMU_PIN_RANGE_T);
    COPY_FROM_USER_ARG(prPinRange, rPinRange, MTIOMMU_PIN_RANGE_T);

    return _MTMMU_DMA_MAPUNMAP_RANGE(0, &rDMARange, &rPinRange);
}

int _MTMMU_GetPhyPgd(unsigned long arg)
{
    unsigned long addr;

    addr = HalCpuGetPhyPgd();

    USR_SPACE_ACCESS_VALIDATE_ARG(arg,unsigned long);
    COPY_TO_USER_ARG(arg, addr,unsigned long);

    return MTR_OK;
}

int mtal_ioctl_mtmmu(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int i4Ret = MTR_OK;

    switch (cmd) 
    {
        case MTAL_IO_MMU_GETPGD:
            return _MTMMU_GetPhyPgd(arg);
        case MTAL_IO_MMU_PINMEM:
            return _MTMMU_PinMem(arg);
            break;
        case MTAL_IO_MMU_UNPINMEM:
            return _MTMMU_UnPinMem(arg);
            break;
        case MTAL_IO_MMU_MAPSINGLE:
            return _MTMMU_DMA_MAP_SINGLE(arg);
            break;
        case MTAL_IO_MMU_UNMAPSINGLE:
            return _MTMMU_DMA_UNMAP_SINGLE(arg);
            break;
        default:
            return MTR_NOT_SUPPORTED;
    }

    return i4Ret;
}
