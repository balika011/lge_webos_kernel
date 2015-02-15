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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: widget_gfx.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget_gfx.c
 *  This header file declares public function prototypes of widget.
 */

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_demux.h"
//#include "x_hal_5371.h"
#include "x_hal_926.h"

#include "osd_if.h"
#include "gfx_if.h"
#include "ir_if.h"
#include "drv_fnt.h"
LINT_EXT_HEADER_END

#include "widget.h"
#include "widget_gfx.h"

#include "widget_debug.h"

#include "x_drv_if.h"
#include "fnt_if.h"


#define USE_FONT_LIB 1
#define TRUNCATE_LENGTH 16
#define OSDPLANE1 0
#define OSDPLANE2 1
#define OSDPLANE3 2

// Semaphores
static PEN_HANDLE hPen;

//static FNT_QPEN_T* prQPen;

static HANDLE_T hGfxSema;

#ifdef USE_RGB8_CLUT
static UINT32 _au4Pal[256] =
{
    0xD0113366,
    0xD0223377,
    0xD0EEAA22,
    0xD0000000, 
    0xD0224488,
    0xD0FFFFFF,
    0xD0CC9900,
    0xD0FFCC55,
    0xD0BB9955,
    0xD0FFEEDD,
    0xD0EECC99,
    0xD0FFEEAA,
    0xD0AA8833,
    0xD0335599,
    0xD0000011,
    0xD0112266,
    0xD0334488,
    0xD0DD9911,
    0xD0EEAA11,
    0xD0DDAA11,
    0xD0CC8800,
    0xD0CC9911,
    0xD077BBFF,
    0xD066AAEE,
    0xD0223366,
    0xD00066CC,
    0xD05599EE,
    0xD0112255,
    0xD0EEDDBB,
    0xD066BBFF,
    0xD088CCFF,
    0xD0001122,
    0xD04499FF,
    0xD0886611,
    0xD02288FF,
    0xD0112244,
    0xD0DDAA22,
    0xD0FFFF00,
    0xD055AAFF,
    0xD0333333,
    0xD03399FF,
    0xD01166CC,
    0xD099CCFF,
    0xD0DDEEFF,
    0xD04488DD,
    0xD0444444,
    0xD0BB8800,
    0xD01188FF,
    0xD055AAEE,
    0xD0775500,
    0xD0EEFFFF,
    0xD04499EE,
    0xD02288EE,
    0xD0222222,
    0xD0555555,
    0xD088AADD,
    0xD0BBBBBB,
    0xD03388FF,
    0xD0111111,
    0xD0666666,
    0xD088BBEE,
    0xD0000022,
    0xD044AAFF,
    0xD0CCCCCC,
    0xD0888888,
    0xD0EEEEEE,
    0xD0001111,
    0xD02299EE,
    0xD0001133,
    0xD0DDDDDD,
    0xD07799FF,
    0xD0999999,
    0xD099DDFF,
    0xD077CCFF,
    0xD0444455,
    0xD0AAAAAA,
    0xD0775511,
    0xD0777777,
    0xD0CCCCDD,
    0xD0EEEEFF,
    0xD0AACCFF,
    0xD05599DD,
    0xD03377DD,
    0xD03399EE,
    0xD0335588,
    0xD0111133,
    0xD066AAFF,
    0xD077AAEE,
    0xD02277CC,
    0xD0997711,
    0xD0112233,
    0xD06699EE,
    0xD0888899,
    0xD0446699,
    0xD04477AA,
    0xD02299FF,
    0xD04499DD,
    0xD077BBEE,
    0xD0CC9933,
    0xD0DDDDEE,
    0xD02277DD,
    0xD06699DD,
    0xD0FFFF11,
    0xD0997733,
    0xD088BBFF,
    0xD0FFCC00,
    0xD05588DD,
    0xD0776611,
    0xD04477DD,
    0xD0AACCEE,
    0xD0CCEEFF,
    0xD077AACC,
    0xD0114488,
    0xD02266CC,
    0xD0111122,
    0xD01155AA,
    0xD0EEEE22,
    0xD0333344,
    0xD0DD9922,
    0xD088BB99,
    0xD0DDDD33,
    0xD03377CC,
    0xD0110000,
    0xD0FFCC66,
    0xD0113355,
    0xD06688BB,
    0xD055BBFF,
    0xD0223355,
    0xD0EEBB44,
    0xD01177DD,
    0xD03377BB,
    0xD0002244,
    0xD02277EE,
    0xD0555566,
    0xD0003366,
    0xD03388EE,
    0xD099BBEE,
    0xD03388DD,
    0xD02288DD,
    0xD0DDCCDD,
    0xD0115599,
    0xD0AABBCC,
    0xD0002255,
    0xD04488EE,
    0xD0334477,
    0xD09999AA,
    0xD0889999,
    0xD088AAEE,
    0xD0002233,
    0xD0996622,
    0xD0444411,
    0xD0225599,
    0xD0776622,
    0xD0996611,
    0xD0EEBB55,
    0xD0FFDDAA,
    0xD0221100,
    0xD0DD9933,
    0xD06699CC,
    0xD0EEAA33,
    0xD0554422,
    0xD0AADDFF,
    0xD02266AA,
    0xD077DD77,
    0xD0AA8822,
    0xD0BBCCDD,
    0xD0DDBB66,
    0xD04488BB,
    0xD0886622,
    0xD0AA8844,
    0xD01155BB,
    0xD0445555,
    0xD0446688,
    0xD03366FF,
    0xD0CC8811,
    0xD0224455,
    0xD0AACC66,
    0xD066AABB,
    0xD0BBCC55,
    0xD0AAAABB,
    0xD0331100,
    0xD066AAAA,
    0xD033AAFF,
    0xD0225577,
    0xD06699BB,
    0xD02255EE,
    0xD05577DD,
    0xD05599CC,
    0xD0224477,
    0xD0BBCCFF,
    0xD03388CC,
    0xD0DDEEEE,
    0xD01133AA,
    0xD0CCDDEE,
    0xD099BBCC,
    0xD0AACCDD,
    0xD0DDFFFF,
    0xD0AABBBB,
    0xD0FFBB00,
    0xD0113377,
    0xD0BBDDEE,
    0xD05588BB,
    0xD0CCBBCC,
    0xD0BBCC66,
    0xD01177EE,
    0xD0003355,
    0xD0001155,
    0xD0114466,
    0xD0777788,
    0xD01166BB,
    0xD0000033,
    0xD0114477,
    0xD0115588,
    0xD02266DD,
    0xD0222233,
    0xD0EE9911,
    0xD0FFBB33,
    0xD0001166,
    0xD0BBBB88,
    0xD07799CC,
    0xD07799BB,
    0xD0CC6611,
    0xD0EECC22,
    0xD066BB66,
    0xD088EE88,
    0xD0778899,
    0xD099BBDD,
    0xD0AA5500,
    0xD0BB6611,
    0xD0552200,
    0xD0663300,
    0xD066AADD,
    0xD0334455,
    0xD03366EE,
    0xD0FFAA00,
    0xD0441100,
    0xD03366DD,
    0xD05599FF,
    0xD04488CC,
    0xD0CCDDFF,
    0xD0002266,
    0xD03366AA,
    0xD0223333,
    0xD0CCDDDD,
    0xD08899AA,
    0xD0BBBBCC,
    0xD06688AA,
    0xD0778888,
    0xD0667777,
    0xD0111144,
    0xD0112222,
    0xD0003377,
    0xD0FFAA22,
    0xD0FFBB44,
    0x00000000,
    0xD0FF9900,
};
#endif


/* Obsolete
static UINT8* apu1OSDBuffer[OSD_PLANE_MAX_NUM];
static UINT8* apu1OSDBackstage[OSD_PLANE_MAX_NUM];
static UINT32 au4OSDList[OSD_PLANE_MAX_NUM];
*/

/* Public interface */
INT32 WIDGET_ClearScreen(const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawString(const CHAR* szText, UINT32 u4X, UINT32 u4Y, const WIDGET_FONT_PROPERTY_T* prFont, const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_Fill(const WIDGET_RECT_PROPERTY_T* prRect, const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawBorder(const WIDGET_RECT_PROPERTY_T* prRect, UINT32 u4BorderW, const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawMenu(WIDGET_MENU_T* prMenu, const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawMenuItem(const WIDGET_MENUITEM_T* prItem, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected);
INT32 WIDGET_DrawMenuList(const WIDGET_MENULIST_T* prList, const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawMenuTitle(const WIDGET_MENU_T* prMenu, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected);
INT32 WIDGET_DrawMenuItemRangedValue(const WIDGET_MENUITEM_RANGEDVALUE_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected);
INT32 WIDGET_DrawMenuItemBinaryValue(const WIDGET_MENUITEM_BINARYVALUE_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected);
INT32 WIDGET_DrawMenuItemAction(const WIDGET_MENUITEM_ACTION_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected);
INT32 WIDGET_DrawMenuItemOption(const WIDGET_MENUITEM_OPTION_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected);
INT32 WIDGET_ClearMenuItem(const WIDGET_MENUITEM_T* prItem, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected);
INT32 WIDGET_ShowMenuList(const WIDGET_MENULIST_T* prList);

/* Interfaces between widget src files */
INT32 _WIDGET_InitOSDBuffer(WIDGET_OSDINFO_T* rWidgetOSD);

/* Private functions */
INT32 WIDGET_Flush(const WIDGET_OSDINFO_T* prOSD);
INT32 WIDGET_DrawBitmap(const WIDGET_BITMAP_T* prBmp, UINT32 u4DestX, UINT32 u4DestY, const WIDGET_OSDINFO_T* prOSD);

static UINT32 _GetBitsPerPixel(UINT32 u4DrvColorMode);
static UINT32 _GetBytesPerPixel(UINT32 u4ColorMode);
/*static UINT32 _ARGB32ToARGB16(UINT32 u4Color);


static UINT32 _ARGB32ToARGB16(UINT32 u4Color)
{
    u4Color = (u4Color >> 28) + 
    	(((u4Color >> 20) & 0xf) << 4) +
    	(((u4Color >> 12) & 0xf) << 8) +
    	((u4Color & 0xf) << 12);
    return u4Color;	
}
*/

static UINT32 _ARGB16ToARGB32(UINT32 u4Color)
{
    UINT32 u4Ret = 
    	((u4Color >> 12) << 28) |
    	((u4Color >> 12) << 24) |
    	(((u4Color >> 8) & 0xf) << 20) | 
    	(((u4Color >> 8) & 0xf) << 16) | 
    	(((u4Color >> 4) & 0xf) << 12) | 
    	(((u4Color >> 4) & 0xf) << 8) |
    	((u4Color & 0xf) << 4) | 
    	(u4Color & 0xf) ;
    //LOG(5, "ARGB4444 %d to ARGB8888 %d\n", u4Color, u4Ret);

    // FORCE ALPHA = 0xF
    u4Ret |= 0xff;
    
    return u4Ret;	
}

static UINT32 _GetBitsPerPixel(UINT32 u4DrvColorMode)
{
	switch ((OSD_COLOR_MODE_T)u4DrvColorMode)
	{
	case DRV_CM_YBR_2:
	case DRV_CM_RGB_2:
		return 2;

	case DRV_CM_YBR_4:
	case DRV_CM_RGB_4:
		return 4;

	case DRV_CM_YBR_8:
	case DRV_CM_RGB_8:
		return 8;

	case DRV_CM_BRY_422:
	case DRV_CM_YBR_422:
	case DRV_CM_RGB_565:
	case DRV_CM_ARGB_1555:
	case DRV_CM_ARGB_4444:
		return 16;

	case DRV_CM_AYBR_8888:
	case DRV_CM_ARGB_8888:
	default:
		return 32;
	}
}

static UINT32 _GetBytesPerPixel(UINT32 u4ColorMode)
{
	return (((_GetBitsPerPixel(u4ColorMode) -1) / 8) + 1);
}

INT32 _WIDGET_CreateOSDBuffer(WIDGET_OSDINFO_T* prWidgetOSD)
{
    UINT32 hRegion;
    //INT32 i4Flag;
    UINT32 u4Pitch = 0;
    DRV_OSD_RGN_T rOsd;
    VERIFY(prWidgetOSD != NULL);  
    
    u4Pitch = _GetBytesPerPixel(prWidgetOSD->u4ColorMode) * prWidgetOSD->u4W;    
    prWidgetOSD->apu1OSDBuffer = (UINT8*)DRV_SYS_AllocAlignedDmaMemory(prWidgetOSD->u4H * u4Pitch, WIDGET_OSDBUFFER_ALIGNMENT);
    prWidgetOSD->apu1OSDBackstage = (UINT8*)DRV_SYS_AllocAlignedDmaMemory(prWidgetOSD->u4H * u4Pitch, WIDGET_OSDBUFFER_ALIGNMENT);
    rOsd.pu1FrameBaseAddr = prWidgetOSD->apu1OSDBuffer;
    rOsd.u4Left = prWidgetOSD->u4X;
    rOsd.u4Top = prWidgetOSD->u4Y;
    rOsd.u4Width = prWidgetOSD->u4W;
    rOsd.u4Height = prWidgetOSD->u4H;
    rOsd.u4ScaleWidth = 0; //prWidgetOSD->u4W;
    rOsd.u4ScaleHeight = 0; //prWidgetOSD->u4H;
    rOsd.rColorMode = (DRV_COLOR_MODE_T)prWidgetOSD->u4ColorMode;
    rOsd.u4Pitch = u4Pitch;
    rOsd.u1PlaID = prWidgetOSD->u4Plane;

	#ifdef USE_RGB8_CLUT    
    rOsd.pu4Palette = (UINT32*)DRV_SYS_AllocAlignedDmaMemory(sizeof(_au4Pal), WIDGET_OSDBUFFER_ALIGNMENT);    
    x_memcpy(rOsd.pu4Palette, _au4Pal, sizeof(_au4Pal));
	#endif
    DRV_OSD_RGN_Create(&rOsd, &hRegion);    

    prWidgetOSD->u4Region = hRegion;
    
	#if defined(CC_MT5389)||defined(CC_MT5396)||defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)||defined(CC_MT5881)
	DRV_OSD_SetPlaneSwitchOrder((UINT32)OSDPLANE1,(UINT32)OSDPLANE2,(UINT32)OSDPLANE3);
	#else
	DRV_OSD_SetPlaneOrder(TRUE);
	#endif
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_DrawString(const CHAR* szText, UINT32 u4X, UINT32 u4Y, const WIDGET_FONT_PROPERTY_T* prFont, const WIDGET_OSDINFO_T* prOSD)
{
    UINT32 u4BgColor;
    UINT32 u4BgWidth;
    UINT32 u4FontSize;
    UINT32 u4BytesPP;
    INT32 i4Ret;

    DRV_GFX_FILL_T rFill;
	
    VERIFY(prOSD != NULL);
    VERIFY(prFont != NULL);
    u4BgColor = prFont->u4BgColor;
    u4BgWidth = prFont->u4BgWidth;
    u4FontSize = prFont->u4FontSize;
    u4BytesPP = _GetBytesPerPixel(prOSD->u4ColorMode);	
    VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT)==OSR_OK);

    if (szText==NULL)
    {

        rFill.u2DstX = u4X;
        rFill.u2DstY = u4Y + (u4FontSize>>3);
        rFill.u2Width = u4BgWidth;
        rFill.u2Height = u4FontSize;
#ifdef USE_RGB8_CLUT        
        rFill.u4Color = 0x254;
#else
        rFill.u4Color = 0x0;
#endif
        rFill.u2DstPitch = u4BytesPP * prOSD->u4W;        
        rFill.pu1DstBaseAddr = prOSD->apu1OSDBackstage;
        	
        DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
        DRV_GFX_Flush();

    }
    else
    {
        if (u4BgWidth>0)
        {
            rFill.u2DstX = u4X;
            rFill.u2DstY = u4Y + (u4FontSize>>3);
            rFill.u2Width = u4BgWidth;
            rFill.u2Height = u4FontSize;
            rFill.u4Color = u4BgColor;
            rFill.u2DstPitch = u4BytesPP * prOSD->u4W;        
            rFill.pu1DstBaseAddr = prOSD->apu1OSDBackstage;

            DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
            DRV_GFX_Flush();
        
        }
        

#ifdef USE_RGB8_CLUT
        FNT_SetPaper(hPen, (void *)(prOSD->apu1OSDBackstage), u4BytesPP, u4BytesPP * prOSD->u4W, 1);
        FNT_SetSize(hPen, u4FontSize);
        FNT_SetColor(hPen, prFont->u4FrColor);
        FNT_SetLocation(hPen, (INT32)u4X+1, (INT32)u4Y-1);
        i4Ret = FNT_DrawString(hPen, szText);             
#else
#ifdef USE_FONT_LIB
        FNT_SetPaper(hPen, (void *)(prOSD->apu1OSDBackstage), u4BytesPP, u4BytesPP * prOSD->u4W, 4);
        FNT_SetSize(hPen, u4FontSize);
        FNT_SetColor(hPen, _ARGB16ToARGB32(prFont->u4FrColor));           
        FNT_SetLocation(hPen, (INT32)u4X+1, (INT32)u4Y-1);
        i4Ret = FNT_DrawString(hPen, szText);             
#else
        {
            UINT32 i, j;
            UINT32 u4Strlen = x_strlen(szText);
            UINT32 u4Pitch = u4BytesPP * prOSD->u4W;
            UINT8* pu1Dest = prOSD->apu1OSDBackstage;
            UINT8* pu1Font;
            for(i = 0; i < u4Strlen; i++)
            {
                pu1Dest = (u4Y + 4) * u4Pitch + (u4X + 14 * i) * u4BytesPP  + prOSD->apu1OSDBackstage;
                pu1Font = (UINT8*)NAV_LookupFont(szText[i]);                
                for(j = 0; j < 23; j++)
                {
                    x_memcpy(pu1Dest, pu1Font, 28);
                    pu1Dest += u4Pitch;
                    pu1Font += 48;
                }
            }
            HalFlushDCache();
        }
#endif // use font lib
#endif // use RGB8 CLUT
        		        	          

    }
    
    VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);

    UNUSED(i4Ret);
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_Flush(const WIDGET_OSDINFO_T* prOSD)
{
	UINT32 u4Pitch;
	INT32 i4Ret;
       DRV_GFX_BLIT_T rBlit;
  
	
	VERIFY(prOSD != NULL);
	u4Pitch = _GetBytesPerPixel(prOSD->u4ColorMode) * prOSD->u4W;
	VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT)==OSR_OK);

       rBlit.pu1DstBaseAddr = prOSD->apu1OSDBuffer;
       rBlit.pu1SrcBaseAddr = prOSD->apu1OSDBackstage;
       rBlit.u2DstPitch = u4Pitch;
       rBlit.u2SrcPitch = u4Pitch;
       rBlit.u2DstX = 0;
       rBlit.u2DstY = 0;
       rBlit.u2Width = prOSD->u4W;
       rBlit.u2Height = prOSD->u4H;
       rBlit.u2SrcX = 0;
       rBlit.u2SrcY = 0;

       DRV_GFX_BitBlt((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rBlit);
       DRV_GFX_Flush();
       
	VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);

	UNUSED(i4Ret);
	
	return (INT32)E_WIDGET_MSG_OK;
}



INT32 WIDGET_ClearScreen(const WIDGET_OSDINFO_T* prOSD)
{
	WIDGET_RECT_PROPERTY_T rRect;
	VERIFY(prOSD != NULL);
	rRect.u4X = 0;
	rRect.u4Y = 0;
	rRect.u4W = prOSD->u4W;
	rRect.u4H = prOSD->u4H;
#ifdef USE_RGB8_CLUT	
	rRect.u4Color = 254;
#else
	rRect.u4Color = 0;
#endif
	VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);
	return (INT32)E_WIDGET_MSG_OK;
}

//extern WIDGET_BITMAP_T arBmpTbl[];
// Draw the menu title
INT32 WIDGET_DrawMenuTitle(const WIDGET_MENU_T* prMenu, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected)
{
	INT32 i4Ret;
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	WIDGET_FONT_PROPERTY_T* prFontDisable;
	VERIFY(prOSD != NULL);
	VERIFY(prMenu != NULL);
	prFontActive = prMenu->prFontActive;
	prFontInactive = prMenu->prFontInactive;
	prFontDisable = prMenu->prFontDisable;

	if(u4Selected == 1)
	{
		if((prOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32) || prOSD->fgNoTitleBmp)
		{
			i4Ret = WIDGET_DrawString(prMenu->szTitle, prMenu->u4TitleX, prMenu->u4TitleY, prFontActive, prOSD);
		}
		else
		{
			i4Ret = WIDGET_DrawBitmap(prMenu->prBmp, prMenu->u4TitleX, prMenu->u4TitleY, prOSD);
		}
	}
	else if(u4Selected == 0)
	{
		if((prOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32) || prOSD->fgNoTitleBmp)
		{
			i4Ret = WIDGET_DrawString(prMenu->szTitle, prMenu->u4TitleX, prMenu->u4TitleY, prFontInactive, prOSD);
		}
		else
		{
			i4Ret = WIDGET_DrawBitmap(prMenu->prBmpInactive, prMenu->u4TitleX, prMenu->u4TitleY, prOSD);
		}
	}
	else if(u4Selected == 2)
	{
		if((prOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32) || prOSD->fgNoTitleBmp)
		{
			i4Ret = WIDGET_DrawString(prMenu->szTitle, prMenu->u4TitleX, prMenu->u4TitleY, prFontDisable, prOSD);
		}
		else
		{
			i4Ret = WIDGET_DrawBitmap(prMenu->prBmpDisable, prMenu->u4TitleX, prMenu->u4TitleY, prOSD);
		}
	}
	else
	{
		return (INT32)E_WIDGET_MSG_INVLIAD_ARG;
	}

	UNUSED(i4Ret);
	
	return (INT32)E_WIDGET_MSG_OK;
}

// Draw the menu, excluding menu title
INT32 WIDGET_DrawMenu(WIDGET_MENU_T* prMenu, const WIDGET_OSDINFO_T* prOSD)
{
	INT32 i4Ret;
	WIDGET_MENUITEM_T* prHeadItem;
	WIDGET_MENUITEM_T* prCurrItem;
	WIDGET_MENUITEM_T* prPtr;
	WIDGET_RECT_PROPERTY_T rRect;

	//WIDGET_BITMAP_T *prBmp = prMenu->prParentList->prBmp;
	VERIFY(prOSD != NULL);
	VERIFY(prMenu != NULL);


	if(prOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
	{
		rRect.u4X = prMenu->u4X;
		rRect.u4Y = prMenu->u4Y;
		rRect.u4W = prMenu->u4W;
		rRect.u4H = prMenu->u4H;
		rRect.u4Color = prMenu->u4BgColor;
		VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rRect.u4Color = prMenu->u4BorderColor;
		VERIFY(WIDGET_DrawBorder(&rRect, prMenu->u4BorderW, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}	
	else
	{
		//WIDGET_DrawBitmap(prOSD->prList->prBmp, 0, 0, prOSD);
	}
	
        			
	{
		UINT32 i = 0;	

              WIDGET_MENUITEM_T* prTemp;

        	prHeadItem = prMenu->prPageStart;
        	prCurrItem = prMenu->prCurr;
        	
        	prPtr = prMenu->prPageStart;       

            // Set inital position corresponding to menu
            prPtr->u4X = (prMenu->u4W / 3) + 50;
            prPtr->u4Y = prMenu->u4H / 12;
            prPtr->u4W = prMenu->u4W - 80;
            prPtr->u4H = prMenu->u4H / 12;

		
        	if(prPtr == prCurrItem)
        	{
        		//_WIDGET_DrawMenuItem(prPtr, prOSD, 1);
        		i4Ret = prPtr->pfActivate(prPtr, prOSD, 1);
        	}
        	else
        	{
        		//_WIDGET_DrawMenuItem(prPtr, prOSD, 0);
        		i4Ret = prPtr->pfActivate(prPtr, prOSD, 0);
        	}	

              prTemp = prPtr;

        	for(prPtr = prHeadItem->prNext; prPtr != prHeadItem; prPtr = prPtr->prNext)
        	{	
        	       // Stop drawing if exceed page or last item
	        	if(++i >= prMenu->u4NumOfItemPage || prPtr->u4Index == 0)
	        	{
	        	    //prMenu->prPageStart = prPtr;
	        	    break;
	        	}
	        	
         
                     // Set inital position corresponding to menu
                     prPtr->u4X = (prMenu->u4W / 3) + 50;
                     prPtr->u4Y = prTemp->u4Y + (prMenu->u4H / 12);
                     prPtr->u4W = prMenu->u4W - 80;
                     prPtr->u4H = prMenu->u4H / 12;
	        	
        		if(prPtr == prCurrItem)
        		{
        			//_WIDGET_DrawMenuItem(prPtr, prOSD, 1);
        			i4Ret = prPtr->pfActivate(prPtr, prOSD, 1);
        		}
        		else
        		{
        			//_WIDGET_DrawMenuItem(prPtr, prOSD, 0);
        			i4Ret = prPtr->pfActivate(prPtr, prOSD, 0);
        		}	


                     prTemp = prPtr;
        		
        	}
	}
	UNUSED(i4Ret);
	return (INT32)E_WIDGET_MSG_OK;	
}

// Draw the specified menu item
INT32 WIDGET_DrawMenuItem(const WIDGET_MENUITEM_T* prItem, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected)
{
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	
	VERIFY(prItem != NULL);
	VERIFY(prOSD != NULL);
	prFontActive = prItem->prFontActive;
	prFontInactive = prItem->prFontInactive;
	if(u4Selected)
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X, prItem->u4Y, prFontActive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	else
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X, prItem->u4Y, prFontInactive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	return (INT32)E_WIDGET_MSG_OK;	
}

INT32 WIDGET_DrawMenuItemBinaryValue(const WIDGET_MENUITEM_BINARYVALUE_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected)
{
	UINT32 u4XOffset;
	CHAR szBuf[20];
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	WIDGET_FONT_PROPERTY_T* prFontValue;
	WIDGET_RECT_PROPERTY_T rRect;
	WIDGET_FONT_PROPERTY_T rFont;
	VERIFY(prItem != NULL);
	VERIFY(prOSD != NULL);

	prFontActive = prItem->prFontActive;
	prFontInactive = prItem->prFontInactive;
	prFontValue = prItem->prFontValue;
	VERIFY(WIDGET_ClearMenuItem((const WIDGET_MENUITEM_T*)((void*)prItem), prOSD, u4Selected) == (INT32)E_WIDGET_MSG_OK);
	
	u4XOffset = prItem->u4X + (prItem->prFontActive->u4FontSize * 12);
	
	if(prItem->fgOn)
	{
		x_sprintf(szBuf, "ON");
	}
	else
	{
		x_sprintf(szBuf, "OFF");
	}
	
	if(u4Selected)
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontActive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rRect.u4X = prItem->u4X;
		rRect.u4Y = prItem->u4Y;
		rRect.u4W = prItem->u4W;
		rRect.u4H = prItem->u4H;
		rRect.u4Color = prItem->prFontActive->u4FrColor;
		VERIFY(WIDGET_DrawBorder(&rRect, 2, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontActive->u4FrColor;
		rFont.u4BgColor = prFontActive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(szBuf, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}
	else
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontInactive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontInactive->u4FrColor;
		rFont.u4BgColor = prFontInactive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(szBuf, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}
	VERIFY(WIDGET_DrawString(szBuf, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	return (INT32)E_WIDGET_MSG_OK;	
	
}
INT32 WIDGET_DrawMenuItemRangedValue(const WIDGET_MENUITEM_RANGEDVALUE_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected)
{
	UINT32 u4XOffset;
	CHAR szBuf[20];
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	WIDGET_FONT_PROPERTY_T* prFontValue;
	WIDGET_RECT_PROPERTY_T rRect;
	WIDGET_FONT_PROPERTY_T rFont;

	VERIFY(prItem != NULL);
	VERIFY(prOSD != NULL);	
	prFontActive = prItem->prFontActive;
	prFontInactive = prItem->prFontInactive;
	prFontValue = prItem->prFontValue;
	VERIFY(WIDGET_ClearMenuItem((const WIDGET_MENUITEM_T*)((void*)prItem), prOSD, u4Selected) == (INT32)E_WIDGET_MSG_OK);
	
	u4XOffset = prItem->u4X + (prItem->prFontActive->u4FontSize * 12);
	
	x_sprintf(szBuf, "%d", prItem->i4Value);
	
	if(u4Selected)
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontActive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rRect.u4X = prItem->u4X;
		rRect.u4Y = prItem->u4Y;
		rRect.u4W = prItem->u4W;
		rRect.u4H = prItem->u4H;
		rRect.u4Color = prItem->prFontActive->u4FrColor;
		VERIFY(WIDGET_DrawBorder(&rRect, 2, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}
	else
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontInactive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	
	// Draw the value bar of this item
	if(prOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
	{
		rRect.u4X = u4XOffset;
		rRect.u4Y = prItem->u4Y + (prItem->prFontActive->u4FontSize /4);
		rRect.u4W = (UINT32)(((prItem->i4Value - prItem->i4Min) * 100) / (prItem->i4Max - prItem->i4Min));
		rRect.u4H = prItem->prFontActive->u4FontSize / 2;
		rRect.u4Color = prItem->prFontActive->u4FrColor;
		VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rRect.u4W = 100;
		VERIFY(WIDGET_DrawBorder(&rRect, 1, prOSD) == (INT32)E_WIDGET_MSG_OK);
		VERIFY(WIDGET_DrawString(szBuf, u4XOffset + (rRect.u4W / 2), prItem->u4Y + (rRect.u4H + 5), prFontValue, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	else
	{
		if(u4Selected)
		{
			rRect.u4X = u4XOffset;
			rRect.u4Y = prItem->u4Y + (prItem->prFontActive->u4FontSize /4);
			rRect.u4W = (UINT32)(((prItem->i4Value - prItem->i4Min) * 100) / (prItem->i4Max - prItem->i4Min));
			rRect.u4H = prItem->prFontActive->u4FontSize / 2;
			rRect.u4Color = prFontValue->u4FrColor;
			VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);
			rRect.u4W = 100;
			VERIFY(WIDGET_DrawBorder(&rRect, 1, prOSD) == (INT32)E_WIDGET_MSG_OK);

			rFont.u4BgColor = prFontActive->u4BgColor;
			rFont.u4FrColor = prFontActive->u4FrColor;
			rFont.u4BgWidth = 4 * prFontActive->u4FontSize;
			rFont.u4FontSize = prFontActive->u4FontSize;
			VERIFY(WIDGET_DrawString(szBuf, u4XOffset + rRect.u4W + 10, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		}
		else
		{
			rRect.u4X = u4XOffset;
			rRect.u4Y = prItem->u4Y + (prItem->prFontInactive->u4FontSize /4);
			rRect.u4W = (UINT32)(((prItem->i4Value -prItem->i4Min) * 100) / (prItem->i4Max - prItem->i4Min));
			rRect.u4H = prItem->prFontInactive->u4FontSize / 2;
			rRect.u4Color = prFontValue->u4FrColor;
			VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);
			rRect.u4W = 100;
			VERIFY(WIDGET_DrawBorder(&rRect, 1, prOSD) == (INT32)E_WIDGET_MSG_OK);

			rFont.u4BgColor = prFontInactive->u4BgColor;
			rFont.u4FrColor = prFontInactive->u4FrColor;
			rFont.u4BgWidth = 4 * prFontInactive->u4FontSize;
			rFont.u4FontSize = prFontInactive->u4FontSize;
			VERIFY(WIDGET_DrawString(szBuf, u4XOffset + (rRect.u4W + 10), prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		}
	}
	return (INT32)E_WIDGET_MSG_OK;
}


INT32 WIDGET_DrawMenuItemAction(const WIDGET_MENUITEM_ACTION_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected)
{
	UINT32 u4XOffset;
	CHAR szBuf[20] = "Press 'Enter'";
	CHAR szBufTmp[37] = "Press 'Enter'";
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	WIDGET_FONT_PROPERTY_T* prFontValue;
	WIDGET_RECT_PROPERTY_T rRect;
	WIDGET_FONT_PROPERTY_T rFont;
	VERIFY(prItem != NULL);
	VERIFY(prOSD != NULL);
	prFontActive = prItem->prFontActive;
	prFontInactive = prItem->prFontInactive;
	prFontValue = prItem->prFontValue;

	VERIFY(WIDGET_ClearMenuItem((const WIDGET_MENUITEM_T*)((void*)prItem), prOSD, u4Selected) == (INT32)E_WIDGET_MSG_OK);

	u4XOffset = prItem->u4X + (prItem->prFontActive->u4FontSize * 12);

	if(x_strlen(prItem->szTitle) > TRUNCATE_LENGTH)
	{
		x_strncpy(szBufTmp, prItem->szTitle, TRUNCATE_LENGTH);
	}
	else
	{
	    x_strcpy(szBufTmp, prItem->szTitle);
	}
		
	if(u4Selected)
	{
		VERIFY(WIDGET_DrawString(szBufTmp, prItem->u4X + 20, prItem->u4Y, prFontActive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rRect.u4X = prItem->u4X;
		rRect.u4Y = prItem->u4Y;
		rRect.u4W = prItem->u4W;
		rRect.u4H = prItem->u4H;
		rRect.u4Color = prItem->prFontActive->u4FrColor;
		VERIFY(WIDGET_DrawBorder(&rRect, 2, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontActive->u4FrColor;
		rFont.u4BgColor = prFontActive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(szBuf, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	else
	{
		VERIFY(WIDGET_DrawString(szBufTmp, prItem->u4X + 20, prItem->u4Y, prFontInactive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontInactive->u4FrColor;
		rFont.u4BgColor = prFontInactive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(szBuf, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);	
	}
	
	return (INT32)E_WIDGET_MSG_OK;
}


INT32 WIDGET_DrawMenuItemOption(const WIDGET_MENUITEM_OPTION_T * prItem, const WIDGET_OSDINFO_T * prOSD, UINT32 u4Selected)
{
	UINT32 u4XOffset;
	WIDGET_FONT_PROPERTY_T* prFontActive;
	WIDGET_FONT_PROPERTY_T* prFontInactive;
	WIDGET_FONT_PROPERTY_T* prFontValue;
	WIDGET_RECT_PROPERTY_T rRect;
	WIDGET_STRINGLIST_ELEMENT_T* prCurr;
	WIDGET_FONT_PROPERTY_T rFont;
	VERIFY(prItem != NULL);
	VERIFY(prOSD != NULL);
	prFontActive = prItem->prFontActive;
	prFontInactive = prItem->prFontInactive;
	prFontValue = prItem->prFontValue;
	prCurr = prItem->prStrList->prCurr;

	if(prCurr == NULL)
	{
		return (INT32)E_WIDGET_MSG_NULL;
	}
	VERIFY(WIDGET_ClearMenuItem((const WIDGET_MENUITEM_T*)((void*)prItem), prOSD, u4Selected) == (INT32)E_WIDGET_MSG_OK);
	
	u4XOffset = prItem->u4X + (prItem->prFontActive->u4FontSize * 12);
		
	if(u4Selected)
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontActive, prOSD) == (INT32)E_WIDGET_MSG_OK);	
		rRect.u4X = prItem->u4X;
		rRect.u4Y = prItem->u4Y;
		rRect.u4W = prItem->u4W;
		rRect.u4H = prItem->u4H;
		rRect.u4Color = prItem->prFontActive->u4FrColor;
		VERIFY(WIDGET_DrawBorder(&rRect, 2, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontActive->u4FrColor;
		rFont.u4BgColor = prFontActive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(prCurr->szStr, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}
	else
	{
		VERIFY(WIDGET_DrawString(prItem->szTitle, prItem->u4X + 20, prItem->u4Y, prFontInactive, prOSD) == (INT32)E_WIDGET_MSG_OK);
		rFont.u4BgWidth = prFontValue->u4BgWidth;
		rFont.u4FrColor = prFontInactive->u4FrColor;
		rFont.u4BgColor = prFontInactive->u4BgColor;
		rFont.u4FontSize = prFontValue->u4FontSize;
		VERIFY(WIDGET_DrawString(prCurr->szStr, u4XOffset, prItem->u4Y, &rFont, prOSD) == (INT32)E_WIDGET_MSG_OK);
	}
		
	return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_ClearMenuItem(const WIDGET_MENUITEM_T* prItem, const WIDGET_OSDINFO_T* prOSD, UINT32 u4Selected)
{
	WIDGET_RECT_PROPERTY_T rRect;
	UNUSED(u4Selected);
	VERIFY(prItem != NULL);
	rRect.u4X = prItem->u4X;
	rRect.u4Y = prItem->u4Y;
	rRect.u4W = prItem->u4W;
	rRect.u4H = prItem->u4H;
	rRect.u4Color = prItem->prParentMenu->u4BgColor;
	
	VERIFY(WIDGET_Fill(&rRect, prOSD) == (INT32)E_WIDGET_MSG_OK);

	return (INT32)E_WIDGET_MSG_OK;

}	
// Draw the osd menu list - redraw all
INT32 WIDGET_DrawMenuList(const WIDGET_MENULIST_T* prList, const WIDGET_OSDINFO_T* prOSD)
{
	WIDGET_MENU_T* prHeadMenu;
	WIDGET_MENU_T* prCurrMenu;
	WIDGET_MENU_T* prPtr;
	

	if( (prList == NULL) || (prOSD == NULL) )
	{
		return (INT32)E_WIDGET_MSG_NULL;
	}	
	prHeadMenu = prList->prHead;
	prCurrMenu = prList->prCurr;
	if(prHeadMenu == NULL)
	{
		return (INT32)E_WIDGET_MSG_NULL;
	}
	// Draw the menu background 

       VERIFY(WIDGET_DrawBitmap(prList->prBmp, 0, 0, prOSD) == (INT32)E_WIDGET_MSG_OK);

	if(prHeadMenu->fgEnable)
	{
		if(prHeadMenu == prCurrMenu)
		{
			VERIFY(WIDGET_DrawMenuTitle(prHeadMenu, prOSD, 1) == (INT32)E_WIDGET_MSG_OK);
		}
		else
		{
			VERIFY(WIDGET_DrawMenuTitle(prHeadMenu, prOSD, 0) == (INT32)E_WIDGET_MSG_OK);
		}
	}
	else
	{
		// Hide disable menu's title
		//WIDGET_DrawMenuTitle(prHeadMenu, prOSD, 2);
	}	
	prPtr = prHeadMenu->prNext;
	for(; prPtr != prHeadMenu; prPtr = prPtr->prNext)
	{
		// Draw the menu title if the menu must be presented
		if(prPtr->fgEnable)
		{
			if(prPtr == prCurrMenu)
			{
				VERIFY(WIDGET_DrawMenuTitle(prPtr, prOSD, 1) == (INT32)E_WIDGET_MSG_OK);
			}
			else
			{
				VERIFY(WIDGET_DrawMenuTitle(prPtr, prOSD, 0) == (INT32)E_WIDGET_MSG_OK);
			}
		}
		else
		{
			// Hide disable menu's title
			//WIDGET_DrawMenuTitle(prPtr, prOSD, 2);
		}
		
	}
	// Draw currently active menu
	VERIFY(WIDGET_DrawMenu(prCurrMenu, prOSD) == (INT32)E_WIDGET_MSG_OK);
	
	return (INT32)E_WIDGET_MSG_OK;
}


INT32 WIDGET_DrawBitmap(const WIDGET_BITMAP_T* prBmp, UINT32 u4DestX, UINT32 u4DestY, const WIDGET_OSDINFO_T* prOSD)
{
	INT32 i4Ret;
       DRV_GFX_BLIT_T rBlit;
	
	VERIFY(prOSD != NULL);
	if(prBmp == NULL)
	{
		return (INT32)E_WIDGET_MSG_NULL;
	}
	VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT)==OSR_OK);

       rBlit.pu1DstBaseAddr = prOSD->apu1OSDBackstage;
       rBlit.pu1SrcBaseAddr = prBmp->pu1Address;
       rBlit.u2DstPitch = prOSD->u4W * _GetBytesPerPixel(prOSD->u4ColorMode);
       rBlit.u2SrcPitch = prBmp->u4PitchSize;
       rBlit.u2DstX = u4DestX;
       rBlit.u2DstY = u4DestY;
       rBlit.u2Width = prBmp->u4Width;
       rBlit.u2Height = prBmp->u4Height;
       rBlit.u2SrcX = 0;
       rBlit.u2SrcY = 0;
       DRV_GFX_BitBlt((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rBlit);
       DRV_GFX_Flush();

	VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);

	UNUSED(i4Ret);
	return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_DrawBitmapScale(const WIDGET_BITMAP_T* prBmp, UINT32 u4DestX, UINT32 u4DestY, UINT32 u4DestW, UINT32 u4DestH, const WIDGET_OSDINFO_T* prOSD)
{
	INT32 i4Ret;
    DRV_GFX_STRETCHBLIT_T rBlit;
	
	VERIFY(prOSD != NULL);
	if(prBmp == NULL)
	{
		return (INT32)E_WIDGET_MSG_NULL;
	}
	VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT)==OSR_OK);

       rBlit.pu1DstBaseAddr = prOSD->apu1OSDBackstage;
       rBlit.pu1SrcBaseAddr = prBmp->pu1Address;
       rBlit.u2DstPitch = prOSD->u4W * _GetBytesPerPixel(prOSD->u4ColorMode);
       rBlit.u2SrcPitch = prBmp->u4PitchSize;
       rBlit.u2DstX = u4DestX;
       rBlit.u2DstY = u4DestY;
       rBlit.u2SrcWidth = prBmp->u4Width;
       rBlit.u2SrcHeight = prBmp->u4Height;
       rBlit.u2DstWidth = u4DestW;
       rBlit.u2DstHeight = u4DestH;       
       rBlit.u2SrcX = 0;
       rBlit.u2SrcY = 0;
       DRV_GFX_StretchBlt((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rBlit);
       DRV_GFX_Flush();

	VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);

	UNUSED(i4Ret);
	return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_Init(WIDGET_OSDINFO_T arWidgetOSDInfo[], UINT32 u4NumRegion)
{
    UINT32 i = 0;
    INT32 i4Ret;
    VERIFY(arWidgetOSDInfo != NULL);
    //i4Ret = OSD_Init();
    //DRV_GFX_Init();

// Move to later
    /*
    FNT_Init();
    i4Ret = FNT_NewQpen(&hPen);

    FNT_SetColor(hPen, 175);
    
    FNT_SetFont(hPen, 1);
   */
   
    VERIFY(x_sema_create(&hGfxSema, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
    LOG(5, "Sema created: %d\n", hGfxSema);

    for(i = 0; i < u4NumRegion; i++)
    {
        VERIFY(_WIDGET_CreateOSDBuffer(&arWidgetOSDInfo[i]) == (INT32)E_WIDGET_MSG_OK);
    }

    //DRV_OSD_SetPlaneAlpha(prWidgetOSDInfo->u4Plane, 0xD0);
    
    UNUSED(i4Ret);
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_Fill(const WIDGET_RECT_PROPERTY_T* prRect, const WIDGET_OSDINFO_T* prOSD)
{
	INT32 i4Ret;
	UINT32 u4X;
	UINT32 u4Y;
	UINT32 u4W;
	UINT32 u4H;
	UINT32 u4Color;
	UINT32 u4Pitch;
       DRV_GFX_FILL_T rFill;
	
	VERIFY(prRect != NULL);
	VERIFY(prOSD != NULL);
	u4X = prRect->u4X;
	u4Y = prRect->u4Y;
	u4W = prRect->u4W;
	u4H = prRect->u4H;
	u4Color = prRect->u4Color;
	u4Pitch = _GetBytesPerPixel(prOSD->u4ColorMode) * prOSD->u4W;
	
	if((u4W == 0) || (u4H == 0))
	{
		return (INT32)E_WIDGET_MSG_OK;
	}
	VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT)==OSR_OK);
	rFill.pu1DstBaseAddr = prOSD->apu1OSDBackstage;
	rFill.u2DstPitch = u4Pitch;
	rFill.u2DstX = u4X;
	rFill.u2DstY = u4Y;
	rFill.u2Width = u4W;
	rFill.u2Height = u4H;
	rFill.u4Color = u4Color;
       DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
       DRV_GFX_Flush();
	
	VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);
   
	UNUSED(i4Ret);	
	
	return (INT32)E_WIDGET_MSG_OK;
	
}

INT32 WIDGET_DrawBorder(const WIDGET_RECT_PROPERTY_T* prRect, UINT32 u4BorderW, const WIDGET_OSDINFO_T* prOSD)
{
	INT32 i4Ret;
	UINT32 u4X;
	UINT32 u4Y;
	UINT32 u4W;
	UINT32 u4H;
	UINT32 u4Color;
	UINT32 u4Pitch;
       DRV_GFX_FILL_T rFill;

	VERIFY(prRect != NULL);
	VERIFY(prOSD != NULL);

	u4X = prRect->u4X;
	u4Y = prRect->u4Y;
	u4W = prRect->u4W;
	u4H = prRect->u4H;
	u4Color = prRect->u4Color;
	u4Pitch = _GetBytesPerPixel(prOSD->u4ColorMode) * prOSD->u4W;

	VERIFY(x_sema_lock(hGfxSema, X_SEMA_OPTION_WAIT) == OSR_OK);
       rFill.pu1DstBaseAddr = prOSD->apu1OSDBackstage;
	rFill.u2DstPitch = u4Pitch;
	rFill.u4Color = u4Color;
	rFill.u2DstX = u4X;
	rFill.u2DstY = u4Y;
	rFill.u2Width = u4W;
	rFill.u2Height = u4BorderW;
       DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
	rFill.u2DstX = u4X;
	rFill.u2DstY = u4Y + u4H - u4BorderW;
	rFill.u2Width = u4W;
	rFill.u2Height = u4BorderW;       
       DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
	rFill.u2DstX = u4X;
	rFill.u2DstY = u4Y;
	rFill.u2Width = u4BorderW;
	rFill.u2Height = u4H;       
       DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);
	rFill.u2DstX = u4X + u4W - u4BorderW;
	rFill.u2DstY = u4Y;
	rFill.u2Width = u4BorderW;
	rFill.u2Height = u4H;       
       DRV_GFX_Fill((DRV_COLOR_MODE_T)prOSD->u4ColorMode, &rFill);       
       DRV_GFX_Flush();
	VERIFY(x_sema_unlock(hGfxSema)==OSR_OK);

	UNUSED(i4Ret);

	return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_ShowMenuList(const WIDGET_MENULIST_T* prList)
{
	VERIFY(prList != NULL);
	
	VERIFY(WIDGET_DrawMenuList(prList, prList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
	VERIFY(WIDGET_Flush(prList->prParentOSD) == (INT32)E_WIDGET_MSG_OK);
	
	return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_InitFont(void)
{
    FNT_Init();
    FNT_NewQpen(&hPen);
    //prQPen = (FNT_QPEN_T*)hPen;

    FNT_SetColor(hPen, 0xffff);
    
    FNT_SetFont(hPen, 2);

    return (INT32)E_WIDGET_MSG_OK;
}

