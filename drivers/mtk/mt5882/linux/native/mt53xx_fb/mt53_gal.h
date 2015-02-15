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

#if ! defined(__MT53_GAL__)
#include "osd_drvif.h"

#define MT_PAL_LENGTH 256
#define MT_GAL_2DTO3D_SHIFT_VALUE 16

typedef enum 
{
	MT_GAL_PIXEL_FORMAT_ARGB = 0,	/**< 32 bit ARGB (4 byte, alpha 8\@24, red 8\@16, green 8\@8, blue 8\@0) */
	MT_GAL_PIXEL_FORMAT_LUT8, 		/**< 8 bit LUT (8 bit color and alpha lookup from palette) */
	MT_GAL_PIXEL_FORMAT_ARGB1555, 	/**< 16 bit ARGB (2 byte, alpha 1\@15, red 5\@10, green 5\@5, blue 5\@0) */
	MT_GAL_PIXEL_FORMAT_RGB16, 	/**< 16 bit RGB (2 byte, red 5\@11, green 6\@5, blue 5\@0) */
	MT_GAL_PIXEL_FORMAT_ARGB4444, 	/**< 16 bit ARGB (2 byte, alpha 4\@12, red 4\@8, green 4\@4, blue 4\@0) */
	MT_GAL_PIXEL_FORMAT_A8, 		/**< 8 bit A (1 byte, alpha 8\@0) */
	MT_GAL_PIXEL_FORMAT_MAX		/**< Maximum number of MT_GAL_PIXEL_FORMAT_T */
} MT_GAL_PIXEL_FORMAT_T;

typedef enum
{
	MT_GAL_PLANE_ARGB_EX = 0x00000000,
	MT_GAL_PLANE_YUV_EX	= 0x00000001,
	MT_GAL_PLANE_MAX_EX	= 0x00000010,
} MT_GAL_PLANE_FORMAT_T;

typedef struct
{
	unsigned int	*pPalette;	/**< point to the palette arry */
	unsigned int	length;		/**< size of palette (max: 256) */
} MT_GAL_PALETTE_INFO_T;

typedef struct
{
	unsigned int			offset;				/**< bytes offset from the start of graphic memory */
	unsigned int			virtualAddress;   /**< virtual address of surface */
	unsigned int			physicalAddress;  /**< physical address of surface */
	unsigned short			pitch;				/**< pitch: length of horizontal line */
	unsigned short			bpp;				/**< bits per pixel */
	unsigned short			width;				/**< width of surface */
	unsigned short			height;				/**< height of surface */
	MT_GAL_PIXEL_FORMAT_T	pixelFormat;		/**< pixel format of surface */
	MT_GAL_PALETTE_INFO_T	paletteInfo;		/**< palette information, this field is used when the surface's pixel format is based on indexed color. */
	signed int				id;					/**< surface identifier */
	signed int 				pid;				/**< request process id */
	unsigned int			property;			/**< reserved for future use */
	unsigned int			aligned_offset;	
} MT_GAL_SURFACE_INFO_T;

typedef struct {
	unsigned int		physicalAddr;		/**< physical address of surface pool base */
	unsigned int		virtualAddr;		/**< virtual address of surface pool base */
	unsigned int		startOffset;		/**< the end of allocated layers (surface pool start with zero offset.) */
	unsigned int		size;				/**< size of surface pool (in bytes). It is not from startOffset but zero offset which means base point of surface pool. */
	unsigned short		byteOffsetAlign;	/**< bytes alignment constraint for the offset value of each surface allocation */
	unsigned short		bytePitchAlign;		/**< bytes alignment constraint for the surface's pitch value */
} MT_GAL_SURFACE_POOL_INFO_T;

typedef enum
{
   MTGAL_OK					=  0,   ///< Success
   MTGAL_NOT_OK				= -1,   ///< Not ok
   MTGAL_INVALID_PARAM		= -2,   ///< Not ok
   MTGAL_OUT_OF_MEMORY		= -3
} MTGAL_RESULT_T;

typedef struct
{
    unsigned int u4Addr;
    unsigned int u4Size;
} MT_GAL_BUFPROP_T;

typedef struct
{
	int x, y;
	int w, h;
} MT_GAL_RECT_T;

typedef enum 
{
	MT_GAL_2D_TO_3D = 0,	/**2d->3d */
	MT_GAL_S3D, 		/**sterei 3d */
	MT_GAL_3D_DISABLE, 
	MT_GAL_MAX
} MT_GAL_3D_MODE;

typedef enum
{
  MT_GAL_NORMAL_MODE,
  MT_GAL_3DPR_MODE,
  MT_GAL_3DSG_MODE,
  MT_GAL_3DPDP_MODE, //PDP special case, 
  MT_GAL_MAX_MODE,
} MT_GAL_MODE_T;

typedef enum
{
	MT_GAL_PLANE_ATTRIBUTE_NULL,
	MT_GAL_PLANE_ATTRIBUTE_RECT_IN,   /* x, y, w, h */
	MT_GAL_PLANE_ATTRIBUTE_RECT_OUT,  /* x, y, w, h */
	MT_GAL_PLANE_ATTRIBUTE_STEREO,    /* on/off, mode */
	MT_GAL_PLANE_ATTRIBUTE_FORMAT,	  /* plane format, not surface format */
	MT_GAL_PLANE_ATTRIBUTE_SURFACE,
	MT_GAL_PLANE_ATTRIBUTE_RUN,
	MT_GAL_PLANE_ATTRIBUTE_MAX
} MT_GAL_PLANE_ATTRIBUTE_T;

extern void				MT_GAL_InitSurfaceSema(void);
extern void				MT_GAL_SetSurfacePool(MT_GAL_SURFACE_POOL_INFO_T *u4SurfacePool);
extern MTGAL_RESULT_T	MT_GAL_CreateSurface(unsigned short width, unsigned short height, int pixelFormat, MT_GAL_SURFACE_INFO_T *pSurfaceInfo);
extern MTGAL_RESULT_T	MT_GAL_DestroySurface(MT_GAL_SURFACE_INFO_T *pSurfaceInfo);
extern MT_GAL_SURFACE_INFO_T * MT_GAL_GetSurfaceFromOffset(unsigned int offset);
extern MTGAL_RESULT_T	MT_GAL_SyncSurface(MT_GAL_SURFACE_INFO_T *u4SurfacePool);
extern MTGAL_RESULT_T	MT_GAL_GALInit(unsigned int u4PhyAddr, unsigned int u4VirAddr, unsigned int u4Size);
extern MTGAL_RESULT_T	MT_GAL_GetTempBuffer(INT32 *pSurfaceInfo, INT32 *pSurfaceInfo2);
extern MTGAL_RESULT_T	MT_GAL_GetTempSurface(MT_GAL_SURFACE_INFO_T *pSurfaceInfo, MT_GAL_SURFACE_INFO_T *pSurfaceInfo2);
extern MTGAL_RESULT_T	MT_GAL_SetFrameSurface(UINT32 u4Plane, MT_GAL_SURFACE_INFO_T *pSurfaceInfo);
extern void				MT_GAL_InitUsrSaceSema(void);
extern MTGAL_RESULT_T	MT_GAL_MultiProcLock(void);
extern MTGAL_RESULT_T	MT_GAL_MultiProcUnLock(void);
extern MTGAL_RESULT_T	MT_GAL_MallocToOffset(INT32 u4Length,INT32 * offset );
extern MTGAL_RESULT_T	MT_GAL_Set3DMode(UINT32 u4Plane, UINT32 u4Enable, UINT32 u4Mode);
extern void				MT_GAL_SetStereoMode(UINT32 u4Mode);
extern MTGAL_RESULT_T	MT_GAL_ResetFrameSurface(UINT32 u4Plane);
extern MTGAL_RESULT_T	MT_GAL_GetFrameSurfaceAttributes(UINT32 u4Plane, MT_GAL_PLANE_ATTRIBUTE_T *attributes, INT32 *values);
extern MTGAL_RESULT_T	MT_GAL_SetFrameSurfaceAttributes(UINT32 u4Plane, MT_GAL_PLANE_ATTRIBUTE_T *attributes, const INT32 *values);

#endif

