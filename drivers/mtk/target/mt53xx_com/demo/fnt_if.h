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
#if ! defined(FNT_IF_H)
#define FNT_IF_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#define D_FNT_BITMAP_MAX 48

typedef enum _ENUM_TTF_FONT_T
{
	TTF_ARIAL,
	TTF_COPP,
	TTF_MAX_FONT
} ENUM_TTF_FONT_T;

#define N_FACE_IN_ARIAL		1
#define N_FACE_IN_COPP		1

#define N_FACE_TOTAL		(N_FACE_IN_ARIAL + N_FACE_IN_COPP)

typedef struct _FNT_FACE_DATA_T
{
	ENUM_TTF_FONT_T	eFont;
	UINT32			u4Face;
	FT_Face			ftFace;
	BOOL			fgCacheControl;
} FNT_FACE_DATA_T;


typedef struct _FNT_LIB_DATA_T
{
	BOOL			fgFntInit;
	UINT32			u4FntHeight;
	UINT32			u4Color;
	/* freetype related data */
	FT_Library		ftLib;
	FTC_Manager		ftCacheManager;
	FTC_ImageCache	ftImageCache;
	FTC_SBitCache	ftSBitCache;
	FTC_CMapCache	ftCMapCache;
	/* font faces */
	FNT_FACE_DATA_T	arFace[N_FACE_TOTAL];
	/* other data */
	INT32			bHinted;
	INT32			bAntialias;
	INT32			bUseSBits;
	INT32			bLowPrec;
	INT32			bAutoHint;
	INT32			bLcdMode;
	INT32			i4Dpi;
} FNT_LIB_DATA_T;

typedef struct _FNT_BITMAP_T
{
	INT32 i4Height;
	INT32 i4Width;
	INT32 i4Pitch;
	INT32 i4Left;
	INT32 i4Top;

	FT_Vector vecAdvance;
	FT_BBox ftBBox;

	UINT32 u4Mode;
	void *pvBuffer;
} FNT_BITMAP_T;

typedef struct _FNT_QPEN_T
{
	UINT32			u4PtSize;
	INT32			x, y;
	UINT32			u4Color;
	UINT32			u4Font;
	FTC_ImageTypeRec	ftcImageTypeRec;
	INT32			i4CMapIndex;
	void			*pvBitmap;
	UINT32			u4BytePerPix;
	UINT32			u4Pitch;
	UINT32			u4AlphaBits;
	void			(*pfnQPenDraw)(struct _FNT_QPEN_T *pQPen);
	FNT_BITMAP_T	rFontBitmap;
} FNT_QPEN_T;


#endif // FNT_IF_H
