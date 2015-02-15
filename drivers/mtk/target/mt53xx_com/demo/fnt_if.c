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
#include "x_typedef.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_hal_926.h"
#include "x_printf.h"

#include "drv_fnt.h"
#include "fnt_if.h"

static FNT_LIB_DATA_T _rFtLibData;

static void _fntConstructFonts(void)
{
	_rFtLibData.bHinted		= 1;
	_rFtLibData.bAntialias	= 1;
	_rFtLibData.bUseSBits	= 1;
	_rFtLibData.bLowPrec	= 0;
	_rFtLibData.bAutoHint	= 1;
	_rFtLibData.bLcdMode	= 0;
}

static void _fntDestructFonts(void)
{
	UINT32 u4Face;

	// destroy cache
	if (_rFtLibData.ftCacheManager)
	{
		FTC_Manager_Done( _rFtLibData.ftCacheManager );
	}
	// destroy faces
	for (u4Face = 0; u4Face < N_FACE_TOTAL; u4Face++)
	{
		if (!_rFtLibData.arFace[u4Face].fgCacheControl && _rFtLibData.arFace[u4Face].ftFace)
		{
			FT_Done_Face( _rFtLibData.arFace[u4Face].ftFace );
		}
	}
	// destroy lib
	if (_rFtLibData.ftLib)
	{
		FT_Done_FreeType(_rFtLibData.ftLib);
	}
	x_memset(&_rFtLibData, 0, sizeof(FNT_LIB_DATA_T));
	_fntConstructFonts();
}

static FT_Error _fntFtcFaceRequester(
	FTC_FaceID	face_id,
	FT_Library	library,
	FT_Pointer	request_data,
	FT_Face		*aface)
{
	FNT_FACE_DATA_T *prFaceData;
	UINT32			u4FaceId;

	prFaceData = (FNT_FACE_DATA_T*)request_data;
	u4FaceId = (UINT32)face_id;
	*aface = prFaceData[u4FaceId].ftFace;
	prFaceData[u4FaceId].fgCacheControl = TRUE;

	UNUSED(library);

	return (FT_Error)0;
}

static void _fntDrawChar_8		(FNT_QPEN_T *prPen)
{
	//bitblt
	UINT8 *pu1Dst, *pu1DstLine;
	UINT8 *pu1Src, *pu1SrcLine;
	//src bmp
	FNT_BITMAP_T *prSrcBmp;
	//common part
	INT32 i4cx, i4cy, i4H, i4W;

	prSrcBmp = &prPen->rFontBitmap;
	i4cx = prPen->x + prSrcBmp->i4Left;
	i4cy = prPen->y + prPen->u4PtSize - prSrcBmp->i4Top;// + prSrcBmp->ftBBox.yMin;

	pu1DstLine = (UINT8*)prPen->pvBitmap;
	pu1DstLine += i4cy * prPen->u4Pitch + i4cx;
	pu1SrcLine = (UINT8*)prSrcBmp->pvBuffer;

	for (i4H = 0; i4H < prSrcBmp->i4Height; i4H++)
	{
		pu1Dst = pu1DstLine;
		pu1Src = pu1SrcLine;
		for (i4W = 0; i4W < prSrcBmp->i4Width; i4W++)
		{
			if (*pu1Src)
			{
				*pu1Dst = prPen->u4Color; //*pu1Src;
			}
			pu1Dst++;
			pu1Src++;
		}
		pu1DstLine += prPen->u4Pitch;
		pu1SrcLine += prSrcBmp->i4Pitch;
	}
}

static void _fntDrawChar_16		(FNT_QPEN_T *prPen)
{
	//bitblt
	UINT16 *pu2Dst, *pu2DstLine;
	UINT8 *pu1Src, *pu1SrcLine;
	//src bmp
	FNT_BITMAP_T *prSrcBmp;
	//common part
	INT32 i4cx, i4cy, i4H, i4W;
	//pen color
	UINT32 u4ca, u4cr, u4cg, u4cb;
	UINT16 u2Color;

	prSrcBmp = &prPen->rFontBitmap;
	i4cx = prPen->x + prSrcBmp->i4Left;
	i4cy = prPen->y + prPen->u4PtSize - prSrcBmp->i4Top;// + prSrcBmp->ftBBox.yMin;

	/*
	Printf("bbox : %d %d %d %d\n",
		prSrcBmp->ftBBox.xMin,
		prSrcBmp->ftBBox.xMax,
		prSrcBmp->ftBBox.yMin,
		prSrcBmp->ftBBox.yMax
		);
	*/


	pu2DstLine = (UINT16*)prPen->pvBitmap;
	pu2DstLine += i4cy * (prPen->u4Pitch >> 1) + i4cx;
	pu1SrcLine = (UINT8*)prSrcBmp->pvBuffer;

	u4ca = prPen->u4Color >> 24;
	u4cr = (prPen->u4Color >> 16) & 0xff;
	u4cg = (prPen->u4Color >> 8) & 0xff;
	u4cb = prPen->u4Color & 0xff;
	// compose an 565 color (16bits)
	u2Color = ((u4cr >> 3) << 11) | ((u4cg >> 2) << 5) | (u4cb >> 3);

	for (i4H = 0; i4H < prSrcBmp->i4Height; i4H++)
	{
		pu2Dst = pu2DstLine;
		pu1Src = pu1SrcLine;
		for (i4W = 0; i4W < prSrcBmp->i4Width; i4W++)
		{
			if (*pu1Src)
			{
				if (*pu1Src == 0xff)
				{
					// alpha 255 == 1.0
					*pu2Dst = u2Color;
				}
				else if (*pu1Src == 0)
				{
					// alpha 0 == 0.0
					// skip this
				}
				else
				{
					UINT32 u4R, u4G, u4B;
					UINT32 u4SrcA, u4DstA;
					u4SrcA = (UINT32) *pu1Src;
					if (*pu2Dst)
					{
						// get 565 components
						u4R = (*pu2Dst) >> 11;
						u4R = (u4R << 3) | (u4R >> 2);	// repeat msb bits, in 5351 definition
						u4G = (*pu2Dst & 0x07e0) >> 5;
						u4G = (u4G << 2) | (u4G >> 4);
						u4B = (*pu2Dst & 0x001f);
						u4B = (u4B << 3) | (u4B >> 2);
						// mix with pen color component
						u4DstA = 0x100 - u4SrcA;
						u4R = ((u4cr * u4SrcA) + (u4R * u4DstA)) >> 8;
						u4G = ((u4cg * u4SrcA) + (u4G * u4DstA)) >> 8;
						u4B = ((u4cb * u4SrcA) + (u4B * u4DstA)) >> 8;
					}
					else
					{
						u4R = (u4cr * u4SrcA) >> 8;
						u4G = (u4cg * u4SrcA) >> 8;
						u4B = (u4cb * u4SrcA) >> 8;
					}
					// fill back to dst
					*pu2Dst = ((u4R >> 3) << 11) | ((u4G >> 2) << 5) | (u4B >> 3);
				}
			}
			pu2Dst++;
			pu1Src++;
		}
		pu2DstLine += (prPen->u4Pitch >> 1);
		pu1SrcLine += prSrcBmp->i4Pitch;
	}

	UNUSED(u4ca);
}

static void _fntDrawChar_15		(FNT_QPEN_T *prPen)
{
	//bitblt
	UINT16 *pu2Dst, *pu2DstLine;
	UINT8 *pu1Src, *pu1SrcLine;
	//src bmp
	FNT_BITMAP_T *prSrcBmp;
	//common part
	INT32 i4cx, i4cy, i4H, i4W;
	//pen color
	UINT32 u4ca, u4cr, u4cg, u4cb;
	UINT16 u2Color;

	prSrcBmp = &prPen->rFontBitmap;
	i4cx = prPen->x + prSrcBmp->i4Left;
	i4cy = prPen->y + prPen->u4PtSize - prSrcBmp->i4Top;// + prSrcBmp->ftBBox.yMin;

	pu2DstLine = (UINT16*)prPen->pvBitmap;
	pu2DstLine += i4cy * (prPen->u4Pitch >> 1) + i4cx;
	pu1SrcLine = (UINT8*)prSrcBmp->pvBuffer;

	u4ca = prPen->u4Color >> 24;
	u4cr = (prPen->u4Color >> 16) & 0xff;
	u4cg = (prPen->u4Color >> 8) & 0xff;
	u4cb = prPen->u4Color & 0xff;
	// compose an 555 color (15bits), alpha bit set to 1
	u2Color = 0x8000 | ((u4cr >> 3) << 10) | ((u4cg >> 3) << 5) | (u4cb >> 3);

	for (i4H = 0; i4H < prSrcBmp->i4Height; i4H++)
	{
		pu2Dst = pu2DstLine;
		pu1Src = pu1SrcLine;
		for (i4W = 0; i4W < prSrcBmp->i4Width; i4W++)
		{
			// luis@2005/03/22, apply the same alogorithm with _fntDrawChar_24 to get better effect
			UINT32 u4SrcA, u4DstA;
			u4SrcA = (UINT32)*pu1Src;

			if (u4SrcA == 255)
			{
				*pu2Dst = u2Color;
			}
			else if (u4SrcA)
			{
				UINT32 u4R, u4G, u4B;
				if (*pu2Dst)
				{
					// get 555 components
					u4R = (*pu2Dst & 0x7c00) >> 10;
					u4R = (u4R << 3) | (u4R >> 2);	// repeat msb bits, in 5351 definition
					u4G = (*pu2Dst & 0x03e0) >> 5;
					u4G = (u4G << 3) | (u4G >> 2);
					u4B = (*pu2Dst & 0x001f);
					u4B = (u4B << 3) | (u4B >> 2);
					// mix with pen color component
					u4DstA = 0x100 - u4SrcA;
					u4R = ((u4cr * u4SrcA) + (u4R * u4DstA)) >> 8;
					u4G = ((u4cg * u4SrcA) + (u4G * u4DstA)) >> 8;
					u4B = ((u4cb * u4SrcA) + (u4B * u4DstA)) >> 8;
				}
				else
				{
					u4R = (u4cr * u4SrcA) >> 8;
					u4G = (u4cg * u4SrcA) >> 8;
					u4B = (u4cb * u4SrcA) >> 8;
				}
				// fill back to dst
				*pu2Dst = 0x8000 | ((u4R >> 3) << 10) | ((u4G >> 3) << 5) | (u4B >> 3);
			}
			else
			{
				// do nothing
			}

			// old algorithm
/*			if (*pu1Src)
			{
				if (*pu1Src > 0x7f)
				{
					// alpha 1.0
					*pu2Dst = u2Color;
				}
				else
				{
					// alpha 0.0, but still fill color
					// just for testing
					// *pu2Dst = u2Color & 0x7fff;
					*pu2Dst = 0x8000;
				}
			}*/

			pu2Dst++;
			pu1Src++;
		}
		pu2DstLine += (prPen->u4Pitch >> 1);
		pu1SrcLine += prSrcBmp->i4Pitch;
	}
	UNUSED(u4ca);
}

static void _fntDrawChar_12		(FNT_QPEN_T *prPen)
{
	//bitblt
	UINT16 *pu2Dst, *pu2DstLine;
	UINT8 *pu1Src, *pu1SrcLine;
	//src bmp
	FNT_BITMAP_T *prSrcBmp;
	//common part
	INT32 i4cx, i4cy, i4H, i4W;
	//pen color
	UINT32 u4ca, u4cr, u4cg, u4cb;
	UINT16 u2Color;

	prSrcBmp = &prPen->rFontBitmap;
	i4cx = prPen->x + prSrcBmp->i4Left;
	i4cy = prPen->y + prPen->u4PtSize - prSrcBmp->i4Top;// + prSrcBmp->ftBBox.yMin;

	pu2DstLine = (UINT16*)prPen->pvBitmap;
	pu2DstLine += i4cy * (prPen->u4Pitch >> 1) + i4cx;
	pu1SrcLine = (UINT8*)prSrcBmp->pvBuffer;

	u4ca = prPen->u4Color >> 24;
	u4cr = (prPen->u4Color >> 16) & 0xff;
	u4cg = (prPen->u4Color >> 8) & 0xff;
	u4cb = prPen->u4Color & 0xff;
	// compose an 444 color (12bits), alpha bit set to 0
	//u2Color = ((u4cr >> 4) << 8) | (u4cg & 0xf0) | (u4cb >> 4);
	u2Color = 0xf000 | ((u4cr >> 4) << 8) | (u4cg & 0xf0) | (u4cb >> 4);

	for (i4H = 0; i4H < prSrcBmp->i4Height; i4H++)
	{
		pu2Dst = pu2DstLine;
		pu1Src = pu1SrcLine;
		for (i4W = 0; i4W < prSrcBmp->i4Width; i4W++)
		{
			// apply the same alogorithm with _fntDrawChar_24 to get better effect
			UINT32 u4SrcA, u4DstA;
			u4SrcA = (UINT32)*pu1Src;

			if (u4SrcA == 255)
			{
				*pu2Dst = u2Color;
			}
			else if (u4SrcA)
			{
				UINT32 u4R, u4G, u4B;
				if (*pu2Dst)
				{
					// get 444 components
					u4R = (*pu2Dst & 0x0f00) >> 8;
					u4R = (u4R << 4) | u4R;	// repeat msb bits, in 5351 definition
					u4G = (*pu2Dst & 0x00f0) >> 4;
					u4G = (u4G << 4) | u4G;
					u4B = (*pu2Dst & 0x000f);
					u4B = (u4B << 4) | u4B;
					// mix with pen color component
					u4DstA = 0x100 - u4SrcA;
					u4R = ((u4cr * u4SrcA) + (u4R * u4DstA)) >> 8;
					u4G = ((u4cg * u4SrcA) + (u4G * u4DstA)) >> 8;
					u4B = ((u4cb * u4SrcA) + (u4B * u4DstA)) >> 8;
				}
				else
				{
					u4R = (u4cr * u4SrcA) >> 8;
					u4G = (u4cg * u4SrcA) >> 8;
					u4B = (u4cb * u4SrcA) >> 8;
				}
				// fill back to dst
				*pu2Dst = 0xf000 | ((u4R >> 4) << 8) | ((u4G >> 4) << 4) | (u4B >> 4);
			}

/*			if (*pu1Src)
			{
				*pu2Dst = u2Color | ((*pu1Src >> 4) << 12);
			}*/
			pu2Dst++;
			pu1Src++;
		}
		pu2DstLine += (prPen->u4Pitch >> 1);
		pu1SrcLine += prSrcBmp->i4Pitch;
	}
	UNUSED(u4ca);
}

static void _fntDrawChar_24		(FNT_QPEN_T *prPen)
{
	//bitblt
	UINT32 *pu4Dst, *pu4DstLine;
	UINT8 *pu1Src, *pu1SrcLine;
	//src bmp
	FNT_BITMAP_T *prSrcBmp;
	//common part
	INT32 i4cx, i4cy, i4H, i4W;
	//pen color
	UINT32 u4ca, u4cr, u4cg, u4cb;
	UINT32 u4Color;

	prSrcBmp = &prPen->rFontBitmap;
	i4cx = prPen->x + prSrcBmp->i4Left;
	i4cy = prPen->y + prPen->u4PtSize - prSrcBmp->i4Top;// + prSrcBmp->ftBBox.yMin;

	pu4DstLine = (UINT32*)prPen->pvBitmap;
	pu4DstLine += i4cy * (prPen->u4Pitch >> 2) + i4cx;
	pu1SrcLine = (UINT8*)prSrcBmp->pvBuffer;

	// compose an 888 color (24bits), alpha bit set to 0
	u4Color = prPen->u4Color & 0x00ffffff;
	u4Color |= 0xff000000;
	//u4ca = u4Color >> 24;
	u4cr = (u4Color >> 16) & 0xff;
	u4cg = (u4Color >> 8) & 0xff;
	u4cb = u4Color & 0xff;

	for (i4H = 0; i4H < prSrcBmp->i4Height; i4H++)
	{
		pu4Dst = pu4DstLine;
		pu1Src = pu1SrcLine;
		for (i4W = 0; i4W < prSrcBmp->i4Width; i4W++)
		{
			UINT32 u4R, u4G, u4B;
			UINT32 u4SrcA, u4DstA;
			u4SrcA = (UINT32)*pu1Src;

			if (u4SrcA == 255)
			{
				*pu4Dst = u4Color;
			}
			else if (u4SrcA)
			{
				if (*pu4Dst)
				{
					u4DstA = 256 - u4SrcA;
					u4R = (*pu4Dst >> 16) & 0xff;
					u4G = (*pu4Dst >> 8) & 0xff;
					u4B = *pu4Dst & 0xff;
					u4R = (u4R * u4DstA + u4cr * u4SrcA) >> 8;
					u4G = (u4G * u4DstA + u4cg * u4SrcA) >> 8;
					u4B = (u4B * u4DstA + u4cb * u4SrcA) >> 8;
					*pu4Dst = 0xff000000 | (u4R << 16) | (u4G << 8) | u4B;
				}
				else
				{
					// blend color with alpha
					u4R = (u4cr * u4SrcA) >> 8;
					u4G = (u4cg * u4SrcA) >> 8;
					u4B = (u4cb * u4SrcA) >> 8;
					*pu4Dst = 0xff000000 | (u4R << 16) | (u4G << 8) | u4B;
				}
			}
			else
			{
				// do nothing
			}
			pu4Dst++;
			pu1Src++;
		}
		pu4DstLine += (prPen->u4Pitch >> 2);
		pu1SrcLine += prSrcBmp->i4Pitch;
	}
	UNUSED(u4ca);
}

static FT_Error _fntGetFtBitmap(FNT_QPEN_T *prPen, UINT32 u4GlyphIndex)
{
	FT_Glyph		ftGlyph;
	FNT_BITMAP_T *prBitmap;

	if (prPen == NULL)
	{
		return -1;
	}

	// output structure
	prBitmap = &prPen->rFontBitmap;

	// get glyph, for bbox
	FTC_ImageCache_Lookup(
			_rFtLibData.ftImageCache,
			&prPen->ftcImageTypeRec,
			u4GlyphIndex,
			&ftGlyph,
			NULL);

	FT_Glyph_Get_CBox(
			ftGlyph,
			ft_glyph_bbox_pixels,
			&prBitmap->ftBBox);

	if (prPen->u4PtSize < D_FNT_BITMAP_MAX)
	{
		FTC_SBit	ftcSBit;
		// get directly from bitmap cache
		FTC_SBitCache_Lookup(
				_rFtLibData.ftSBitCache,
				&prPen->ftcImageTypeRec,
				u4GlyphIndex,
				&ftcSBit,
				NULL);
		prBitmap->i4Height		= (INT32)ftcSBit->height;
		prBitmap->i4Width		= (INT32)ftcSBit->width;
		prBitmap->i4Pitch		= (INT32)ftcSBit->pitch;
		prBitmap->pvBuffer		= (void *)ftcSBit->buffer;
		prBitmap->u4Mode		= ftcSBit->format == FT_PIXEL_MODE_GRAY ? 0 : 1;
		prBitmap->i4Left		= (INT32)ftcSBit->left;
		prBitmap->i4Top			= (INT32)ftcSBit->top;
		prBitmap->vecAdvance.x	= (FT_Pos)ftcSBit->xadvance;
		prBitmap->vecAdvance.y	= (FT_Pos)ftcSBit->yadvance;
	}
	else
	{
		FT_BitmapGlyph	ftBitmapGlyph;
		FT_Bitmap		*pftBitmap;

		// get from glyph image cache, then render to bitmap

		FT_Glyph_To_Bitmap(
				&ftGlyph,
				FT_RENDER_MODE_NORMAL,
				NULL,
				0);

		ftBitmapGlyph = (FT_BitmapGlyph)ftGlyph;
		pftBitmap = &ftBitmapGlyph->bitmap;

		prBitmap->i4Height		= (INT32)pftBitmap->rows;
		prBitmap->i4Width		= (INT32)pftBitmap->width;
		prBitmap->i4Pitch		= (INT32)pftBitmap->pitch;
		prBitmap->pvBuffer		= (void *)pftBitmap->buffer;
		prBitmap->u4Mode		= pftBitmap->pixel_mode == FT_PIXEL_MODE_GRAY ? 0 : 1;
		prBitmap->i4Left		= (INT32)ftBitmapGlyph->left;
		prBitmap->i4Top			= (INT32)ftBitmapGlyph->top;
		prBitmap->vecAdvance.x	= (FT_Pos)((ftGlyph->advance.x + 0x8000) >> 16);
		prBitmap->vecAdvance.y	= (FT_Pos)((ftGlyph->advance.y + 0x8000) >> 16);
	}

	return 0;
}

void FNT_Init(void)
{
	// open all faces
	// new ftc_manager

	UINT8 **aau1FontsData;
	UINT8 *pu1FontData;
	UINT32 u4Face;
	FT_Face ftFace;
	FT_Error ftRet;

	_fntDestructFonts();

	// create ft_lib
	if (FT_Init_FreeType(&_rFtLibData.ftLib))
	{
		// failed
		_fntDestructFonts();
		return;
	}

	aau1FontsData = FNT_GetFontData();

	// open all font faces
	pu1FontData = aau1FontsData[0];
	u4Face = 0;
	ftRet = FT_New_Memory_Face(
		_rFtLibData.ftLib,
		pu1FontData + 4,
		(pu1FontData[0] << 24) | (pu1FontData[1] << 16) | (pu1FontData[2] << 8) | (pu1FontData[3]),
		u4Face,
		&ftFace);
	_rFtLibData.arFace[0].eFont = TTF_ARIAL;
	_rFtLibData.arFace[0].u4Face = u4Face;
	_rFtLibData.arFace[0].ftFace = ftFace;
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}

	pu1FontData = aau1FontsData[1];
	u4Face = 0;
	ftRet = FT_New_Memory_Face(
		_rFtLibData.ftLib,
		pu1FontData + 4,
		(pu1FontData[0] << 24) | (pu1FontData[1] << 16) | (pu1FontData[2] << 8) | (pu1FontData[3]),
		u4Face,
		&ftFace);
	_rFtLibData.arFace[1].eFont = TTF_COPP;
	_rFtLibData.arFace[1].u4Face = u4Face;
	_rFtLibData.arFace[1].ftFace = ftFace;
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}
	// cache manager
	ftRet = FTC_Manager_New(
		_rFtLibData.ftLib,
		0,0,0,	//max_faces, max_size, max_bytes
		(FTC_Face_Requester)_fntFtcFaceRequester,
		(FT_Pointer)_rFtLibData.arFace,
		&_rFtLibData.ftCacheManager );
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}
	// sbit cache
	ftRet = FTC_SBitCache_New(_rFtLibData.ftCacheManager, &_rFtLibData.ftSBitCache);
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}
	// image cache
	ftRet = FTC_ImageCache_New(_rFtLibData.ftCacheManager, &_rFtLibData.ftImageCache);
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}
	// cmap cache
	ftRet = FTC_CMapCache_New(_rFtLibData.ftCacheManager, &_rFtLibData.ftCMapCache);
	if (ftRet)
	{
		_fntDestructFonts();
		return;
	}
}

INT32 FNT_NewQpen(PEN_HANDLE *phPen)
{
	FNT_QPEN_T *prQuillPen;

	ASSERT(phPen != NULL);

	if (_rFtLibData.ftLib == NULL)
	{
		FNT_Init();
		if (_rFtLibData.ftLib == NULL)
		return -E_FNT_ERR_INIT;
	}

	prQuillPen = (FNT_QPEN_T*)x_mem_alloc(sizeof(FNT_QPEN_T));
	if (prQuillPen == NULL)
	{
		return -E_FNT_ERR_ALLOC;
	}
	x_memset(prQuillPen, 0, sizeof(FNT_QPEN_T));

	prQuillPen->u4PtSize		= 24;
	prQuillPen->u4Color			= 0xffffffff;
	prQuillPen->pvBitmap		= NULL;
	prQuillPen->u4BytePerPix	= 4;
	prQuillPen->u4Pitch			= 0;
	prQuillPen->u4AlphaBits		= 8;
	*phPen = (void *)prQuillPen;

	return E_FNT_OK;
}

void FNT_DeleteQpen(PEN_HANDLE hPen)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		x_mem_free(prPen);
	}
}

void FNT_SetSize(PEN_HANDLE hPen, UINT32 u4Height)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		prPen->u4PtSize = u4Height;

		prPen->ftcImageTypeRec.face_id		= (FTC_FaceID)prPen->u4Font;
		prPen->ftcImageTypeRec.width		= (FT_Int) prPen->u4PtSize;
		prPen->ftcImageTypeRec.height		= (FT_Int) prPen->u4PtSize;
		prPen->ftcImageTypeRec.flags		= FT_LOAD_DEFAULT;
	}
}

void FNT_SetColor(PEN_HANDLE hPen, UINT32 u4Color)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		prPen->u4Color = u4Color;
	}
}

void FNT_SetFont(PEN_HANDLE hPen, UINT32 u4Font)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		if (u4Font < N_FACE_TOTAL)
		{
			prPen->u4Font = u4Font;
		}
		else
		{
			prPen->u4Font = 0;
		}

		prPen->ftcImageTypeRec.face_id		= (FTC_FaceID)prPen->u4Font;
		prPen->ftcImageTypeRec.width		= (FT_Int) prPen->u4PtSize;
		prPen->ftcImageTypeRec.height		= (FT_Int) prPen->u4PtSize;
		prPen->ftcImageTypeRec.flags		= FT_LOAD_DEFAULT;
	}
}

void FNT_SetLocation(PEN_HANDLE hPen, INT32 x, INT32 y)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		prPen->x = x;
		prPen->y = y;
	}
}

void FNT_SetPaper(PEN_HANDLE hPen, void *pvBitmap, UINT32 u4BytePerPix, UINT32 u4Pitch, UINT32 u4AlphaBits)
{
	FNT_QPEN_T *prPen;
	prPen = (FNT_QPEN_T *)hPen;

	if (prPen)
	{
		prPen->pvBitmap = pvBitmap;
		prPen->u4BytePerPix = u4BytePerPix;
		prPen->u4Pitch = u4Pitch;
		prPen->u4AlphaBits = u4AlphaBits;

		switch (u4BytePerPix)
		{
		case 1:
			prPen->pfnQPenDraw = _fntDrawChar_8;
			break;
		case 2:
			switch (u4AlphaBits)
			{
			case 0:
				prPen->pfnQPenDraw = _fntDrawChar_16;
				break;
			case 1:
				prPen->pfnQPenDraw = _fntDrawChar_15;
				break;
			case 4:
			default:
				prPen->pfnQPenDraw = _fntDrawChar_12;
				break;
			}
			break;
		case 4:
		default:
			prPen->pfnQPenDraw = _fntDrawChar_24;
			break;
		}

	}
}


INT32 FNT_DrawString(PEN_HANDLE hPen, const CHAR *szString)
{
	FNT_QPEN_T *prPen;

	CHAR *pCh;
	FT_UInt32 u4GlyphIndex = 0;
	FT_UInt32 u4PrevIndex;
	FT_UInt32 u4CharCode;
	FT_Vector vecKern;

	FTC_ScalerRec ftcScalerRec;		// instance of a FTC_Scaler
	FT_Size ftSize;
	FT_Face ftFace;
	BOOL fgHasKerning;

	FT_Error ftRet;

	prPen = (FNT_QPEN_T *)hPen;

	if (prPen == NULL)
	{
		return -E_FNT_ERR_INIT;
	}

	if (szString == NULL)
	{
		return -E_FNT_ERR_NULL;
	}
	pCh = (CHAR *)szString;

	// lookup size
	if (!*pCh)
	{
		return E_FNT_OK;
	}

	ftcScalerRec.face_id	= (FTC_FaceID)prPen->u4Font;
	ftcScalerRec.width		= (FT_UInt)prPen->u4PtSize;					// auto widht
	ftcScalerRec.height		= (FT_UInt)prPen->u4PtSize;
	ftcScalerRec.pixel		= (FT_Int)TRUE;
	ftcScalerRec.x_res		= (FT_UInt)0;
	ftcScalerRec.y_res		= (FT_UInt)0;

	FTC_Manager_LookupSize(
		_rFtLibData.ftCacheManager,
		(FTC_Scaler)&ftcScalerRec,
		&ftSize);

	// get face
	/*
	FTC_Manager_LookupFace(
		_rFtLibData.ftCacheManager,
		(FT_FaceID)prPen->u4Font,
		&ftFace);
	*/

	ftFace = ftSize->face;
	// get kerning flag
	fgHasKerning = FT_HAS_KERNING( ftFace );

	while (*pCh)
	{
		u4CharCode = (FT_UInt32)*pCh;

		u4PrevIndex = u4GlyphIndex;

		if (u4CharCode != 0x20)
		{
			u4GlyphIndex = FTC_CMapCache_Lookup(
								_rFtLibData.ftCMapCache,
								(FTC_FaceID)prPen->u4Font,
								prPen->i4CMapIndex,
								u4CharCode);

			// if previous index not equal to zero, get kerning
			if (fgHasKerning)
			{
				if (u4PrevIndex)
				{
					FT_Get_Kerning(
						ftFace,
						u4PrevIndex,
						u4GlyphIndex,
						ft_kerning_default,	// get kerning distance aligned on pixel grid
						&vecKern);

					// vecKern.x is 26.6 fix point format
				}
				else
				{
					vecKern.x = 0;
					vecKern.y = 0;
				}
			}

			// get bitmap using sbits cache or image cache
			// and x advance, bounding box using image cache
			// already sized using cache manager size lookup
			ftRet = _fntGetFtBitmap(prPen, u4GlyphIndex);
			if (ftRet)
			{
				return ftRet;
			}

			// draw bitmap to quill pen buffer
			prPen->pfnQPenDraw(prPen);
			// advance vector is pixels, but kerning distance is 26.6
			prPen->x += prPen->rFontBitmap.vecAdvance.x + (vecKern.x >> 6);

//printf("vecAdvance.x=%d, vecAdvance.y=%d, vecKern.x=%d, vecKern.y=%d\n", prPen->rFontBitmap.vecAdvance.x, prPen->rFontBitmap.vecAdvance.y, vecKern.x, vecKern.y);
//printf("i4Top=%d, i4Height=%d, prSrcBmp->ftBBox.yMin=%d\n", prPen->rFontBitmap.i4Top, prPen->rFontBitmap.i4Height, prPen->rFontBitmap.ftBBox.yMin);
		}
		else
		{
			// blanc space
			u4GlyphIndex = 0;
			// advance pen a little
			prPen->x += (prPen->u4PtSize >> 2);
		}

		pCh++;
	}

	HalFlushDCache();
	return 0;
}

