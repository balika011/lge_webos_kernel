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
 * $RCSfile: widget.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget.c
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

#include "ir_if.h"
#include "osd_if.h"
//#include "drv_gfx.h"
#include "gfx_if.h"

LINT_EXT_HEADER_END
 
#include "widget.h"
#include "widget_gfx.h"
#include "widget_event.h"

#include "widget_debug.h"

#define _MAX_TITLE_LENGTH 256 //enlength to 256 for MM long filename
#define WIDGET_DEFAULT_NUM_OF_ITEM_PAGE 9

//static WIDGET_OSDINFO_T rWidgetOSDInfo[OSD_PLANE_MAX_NUM];


// ARGB8888
static WIDGET_FONT_PROPERTY_T rDefaultFontActive =
{
	0xffffffff, 0xffffff00, 100, 20
};
// 0xff0000ff blue
static WIDGET_FONT_PROPERTY_T rDefaultFontInactive = 
{
	0xffffffff, 0xff0000ff, 100, 20 
};

static WIDGET_FONT_PROPERTY_T rDefaultFontDisable =
{
	0xc0c0c0ff, 0x808080ff, 100 , 20
};

static WIDGET_FONT_PROPERTY_T rDefaultFontActiveItem =
{
	0xffffffff, 0xffffff00, 150, 16
};
static WIDGET_FONT_PROPERTY_T rDefaultFontInactiveItem = 
{
	0xffffffff, 0xff0000ff, 150, 16
};
static WIDGET_FONT_PROPERTY_T rDefaultFontDisableItem =
{
	0xc0c0c0ff, 0x808080ff, 150 , 16
};

static WIDGET_FONT_PROPERTY_T rDefaultFontValueItem =
{
	0xffff, 0xffc0c0c0, 16 * 3 , 16
};

// ARGB4444
#ifdef USE_RGB8_CLUT
static WIDGET_FONT_PROPERTY_T rDefaultFontActive16 =
{
	255, //0xff90, 
	1, //0xf237, 
	100,
	32
};
// 0xff0000ff blue
static WIDGET_FONT_PROPERTY_T rDefaultFontInactive16 = 
{
	5, //0xffff, 
	1, //0xf237, 
	100, 
	32 
};

static WIDGET_FONT_PROPERTY_T rDefaultFontDisable16 =
{
	63, //0xfccc, 
	64, //0xf888, 
	100 , 
	32
};

static WIDGET_FONT_PROPERTY_T rDefaultFontActiveItem16 =
{
	255, //0xff90, 
	1, //0xf237, 
	500, 
	24
};
static WIDGET_FONT_PROPERTY_T rDefaultFontInactiveItem16 = 
{
	5, //0xffff, 
	1, //0xf237, 
	500, 
	24
};
static WIDGET_FONT_PROPERTY_T rDefaultFontDisableItem16 =
{
	63, //0xfccc, 
	64, //0xf888, 
	500 , 
	24
};

static WIDGET_FONT_PROPERTY_T rDefaultFontValueItem16 =
{
	255, //0xff90, 
	1, //0xf237, 
	24 * 4 , 
	24
};
#else
static WIDGET_FONT_PROPERTY_T rDefaultFontActive16 =
{
	0xff90, 0xf237, 100, 32
};
// 0xff0000ff blue
static WIDGET_FONT_PROPERTY_T rDefaultFontInactive16 = 
{
	0xffff, 0xf237, 100, 32 
};

static WIDGET_FONT_PROPERTY_T rDefaultFontDisable16 =
{
	0xfccc, 0xf888, 100 , 32
};

static WIDGET_FONT_PROPERTY_T rDefaultFontActiveItem16 =
{
	0xff90, 0xf237, 500, 24
};
static WIDGET_FONT_PROPERTY_T rDefaultFontInactiveItem16 = 
{
	0xffff, 0xf237, 500, 24
};
static WIDGET_FONT_PROPERTY_T rDefaultFontDisableItem16 =
{
	0xfccc, 0xf888, 500 , 24
};

static WIDGET_FONT_PROPERTY_T rDefaultFontValueItem16 =
{
	0xff90, 0xf237, 24 * 4 , 24
};
#endif

WIDGET_OSDINFO_T _arWidgetInfo[3];

/* Public interfaces */
INT32 WIDGET_Init(WIDGET_OSDINFO_T rWidgetOSDInfo[]);

WIDGET_MENULIST_T* WIDGET_NewMenuList(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp);
WIDGET_MENU_T* WIDGET_NewMenu(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp, WIDGET_BITMAP_T* prBmpInactive, WIDGET_BITMAP_T* prBmpDisable);
WIDGET_MENUITEM_RANGEDVALUE_T* WIDGET_NewMenuItem_RangedValue(const CHAR* szTitle, INT32 i4InitValue, INT32 i4Min, INT32 i4Max, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp);
WIDGET_MENUITEM_BINARYVALUE_T* WIDGET_NewMenuItem_BinaryValue(const CHAR* szTitle, BOOL fgInitValue, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp);
WIDGET_MENUITEM_OPTION_T* WIDGET_NewMenuItem_Option(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp);
WIDGET_MENUITEM_ACTION_T* WIDGET_NewMenuItem_Action(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp);
WIDGET_STRINGLIST_T* WIDGET_NewStringList(VOID);
WIDGET_STRINGLIST_ELEMENT_T* WIDGET_NewStringListElement(const CHAR* szStr);
WIDGET_LIST_T* WIDGET_NewList(VOID);
WIDGET_LIST_ELEMENT_T* WIDGET_NewListElement(const CHAR* szStr);
INT32 WIDGET_AddMenu(WIDGET_MENULIST_T* prList, WIDGET_MENU_T* prMenu);
INT32 WIDGET_AddMenuItem(WIDGET_MENU_T* prMenu, WIDGET_MENUITEM_T* prItem);
INT32 WIDGET_AddMenuList(WIDGET_OSDINFO_T* prOSD, WIDGET_MENULIST_T* prList);
INT32 WIDGET_SetOp(WIDGET_MENUITEM_T* prItem, INT32 (*pfOp)(void*));
INT32 WIDGET_SetAction(
	WIDGET_MENUITEM_T* prItem,
	BOOL (*pfHandler)(UINT32, void*), 
	INT32 (*pfOp)(void*), 
	INT32 (*pfActivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32),
	INT32 (*pfDeactivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32));
WIDGET_LIST_ELEMENT_T* WIDGET_SearchListElement(const CHAR* szStr, const WIDGET_LIST_T* prList);
INT32 WIDGET_RemoveLastListElement(WIDGET_LIST_T* prList);
INT32 WIDGET_RemoveLastMenuItem(WIDGET_MENU_T* prMenu);
INT32 WIDGET_RemoveLastStringListElement(WIDGET_STRINGLIST_T* prList);
INT32 WIDGET_MenuItemSetTitle(const WIDGET_MENUITEM_T* prItem, const CHAR* szTitle);
WIDGET_MENUITEM_T* WIDGET_LookupMenuItem(const WIDGET_MENULIST_T* prList, const CHAR* szMenu, const CHAR* szItem);
WIDGET_MENU_T* WIDGET_LookupMenu(const WIDGET_MENULIST_T* prList, const CHAR* szMenu);
INT32 WIDGET_FreeStringListElement(WIDGET_STRINGLIST_ELEMENT_T* prEle);
INT32 WIDGET_FreeMenuItem(WIDGET_MENUITEM_T* prItem);
INT32 WIDGET_FreeListElement(WIDGET_LIST_ELEMENT_T* prEle);
INT32 WIDGET_AddStringList(WIDGET_MENUITEM_OPTION_T* prItem, WIDGET_STRINGLIST_T* prList);
INT32 WIDGET_AddListElement(WIDGET_LIST_T* prList, const CHAR* szStr, VOID* pvContent);
INT32 WIDGET_AddStringListElement(WIDGET_STRINGLIST_T* prList, const CHAR* szStr);


INT32 _InitMenuList(WIDGET_MENULIST_T* prList);
INT32 _InitMenu(WIDGET_MENU_T* prMenu);
INT32 _InitMenuItem(WIDGET_MENUITEM_T* prItem);
INT32 _InitMenuItem_RangedValue(WIDGET_MENUITEM_RANGEDVALUE_T* prItem);
INT32 _InitMenuItem_BinaryValue(WIDGET_MENUITEM_BINARYVALUE_T* prItem);
INT32 _InitMenuItem_Option(WIDGET_MENUITEM_OPTION_T* prItem);
INT32 _InitMenuItem_Action(WIDGET_MENUITEM_ACTION_T* prItem);
INT32 _InitList(WIDGET_LIST_T* prList);
INT32 _InitListElement(WIDGET_LIST_ELEMENT_T* prEle);
INT32 _InitStringList(WIDGET_STRINGLIST_T* prList);
INT32 _InitStringListElement(WIDGET_STRINGLIST_ELEMENT_T* prEle);
void _Dummy(void);



INT32 _InitMenuList(WIDGET_MENULIST_T* prList)
{
    if(prList == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prList->u4Size = 0;
    prList->prCurr = NULL;
    prList->prHead = NULL;
    prList->prTail = NULL;
    if(prList->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
    {
        prList->prFontActive = &rDefaultFontActive;
        prList->prFontInactive = &rDefaultFontInactive;
    }
    else
    {
        prList->prFontActive = &rDefaultFontActive16;
        prList->prFontInactive = &rDefaultFontInactive16;
    }
    prList->fgEnable = TRUE;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 _InitMenu(WIDGET_MENU_T* prMenu)
{
    if(prMenu == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }	
    prMenu->u4Size = 0;
    prMenu->prCurr = NULL;
    prMenu->prHead = NULL;
    prMenu->prTail = NULL;
    prMenu->prNext = NULL;
    prMenu->prPrev = NULL;
    if(prMenu->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
    {
        prMenu->prFontActive = &rDefaultFontActive;
        prMenu->prFontInactive = &rDefaultFontInactive;
        prMenu->prFontDisable = &rDefaultFontDisable;
        prMenu->u4BorderColor = 0xff808080;
        prMenu->u4BgColor = 0xffc0c0c0;
    }
    else
    {
        prMenu->prFontActive = &rDefaultFontActive16;
        prMenu->prFontInactive = &rDefaultFontInactive16;
        prMenu->prFontDisable = &rDefaultFontDisable16;
#ifdef USE_RGB8_CLUT        
        prMenu->u4BorderColor = 64; //0xf888;
        prMenu->u4BgColor = 1; //0xf237;
#else
        prMenu->u4BorderColor = 0xf888;
        prMenu->u4BgColor = 0xf237;
#endif
    }
    prMenu->u4BorderW = 5;
    prMenu->fgEnable = TRUE;

    prMenu->u4NumOfItem = 0;
    prMenu->prPageStart = NULL;
    prMenu->u4NumOfItemPage = WIDGET_DEFAULT_NUM_OF_ITEM_PAGE;
    	
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 _InitMenuItem(WIDGET_MENUITEM_T* prItem)
{
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }	
    prItem->u4Size = 0;
    prItem->prNext = NULL;
    prItem->prPrev = NULL;
    if(prItem->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
    {
        prItem->prFontActive = &rDefaultFontActiveItem;
        prItem->prFontInactive = &rDefaultFontInactiveItem;
        prItem->prFontValue = &rDefaultFontValueItem;
        prItem->u4BorderColor = 0xff808080;
        prItem->u4BgColor = 0xffc0c0c0;
    }
    else
    {
        prItem->prFontActive = &rDefaultFontActiveItem16;
        prItem->prFontInactive = &rDefaultFontInactiveItem16;
        prItem->prFontValue = &rDefaultFontValueItem16;
#ifdef USE_RGB8_CLUT                
        prItem->u4BorderColor = 64; //0xf888;
        prItem->u4BgColor = 1; //0xf237;
#else
        prItem->u4BorderColor = 0xf888;
        prItem->u4BgColor = 0xf237;
#endif
    }
    prItem->u4BorderW = 3;
    prItem->fgEnable = TRUE;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 _InitMenuItem_RangedValue(WIDGET_MENUITEM_RANGEDVALUE_T* prItem)
{
    INT32 i4Ret;
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    i4Ret = _InitMenuItem((WIDGET_MENUITEM_T*)((void*)prItem));
    /*
    prItem->pfHandler = _WIDGET_IRHandler_MenuItem_RangedValue;
    prItem->pfActivate = _WIDGET_DrawMenuItemRangedValue;
    prItem->pfDeactivate = _WIDGET_DrawMenuItemRangedValue;
    */
    prItem->pfHandler = NULL;
    prItem->pfActivate = NULL;
    prItem->pfDeactivate = NULL;
    prItem->pfOp = NULL;
    /*
    prItem->u4Max = 255;
    prItem->u4Min = 0;
    prItem->u4Value = 100;
    */
    return i4Ret;
}

INT32 _InitMenuItem_BinaryValue(WIDGET_MENUITEM_BINARYVALUE_T* prItem)
{
    INT32 i4Ret;
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    i4Ret = _InitMenuItem((WIDGET_MENUITEM_T*)((void*)prItem));
    /*
    prItem->pfHandler = _WIDGET_IRHandler_MenuItem_BinaryValue;
    prItem->pfActivate = _WIDGET_DrawMenuItemBinaryValue;
    prItem->pfDeactivate = _WIDGET_DrawMenuItemBinaryValue;
    */
    prItem->pfHandler = NULL;
    prItem->pfActivate = NULL;
    prItem->pfDeactivate = NULL;	
    prItem->pfOp = NULL;
    //prItem->fgOn = FALSE;
    return i4Ret;
}


INT32 _InitMenuItem_Action(WIDGET_MENUITEM_ACTION_T* prItem)
{
    INT32 i4Ret;
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }	
    i4Ret = _InitMenuItem((WIDGET_MENUITEM_T*)((void*)prItem));
    /*
    prItem->pfHandler = _WIDGET_IRHandler_MenuItem_Action;
    prItem->pfActivate = _WIDGET_DrawMenuItemAction;
    prItem->pfDeactivate = _WIDGET_DrawMenuItemAction;
    */
    prItem->pfHandler = NULL;
    prItem->pfActivate = NULL;
    prItem->pfDeactivate = NULL;	
    prItem->pfOp = NULL;
    return i4Ret;
}

INT32 _InitMenuItem_Option(WIDGET_MENUITEM_OPTION_T* prItem)
{
    INT32 i4Ret;
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    i4Ret = _InitMenuItem((WIDGET_MENUITEM_T*)((void*)prItem));
    /*
    prItem->pfHandler = _WIDGET_IRHandler_MenuItem_Option;
    prItem->pfActivate = _WIDGET_DrawMenuItemOption;
    prItem->pfDeactivate = _WIDGET_DrawMenuItemOption;
    */
    prItem->pfHandler = NULL;
    prItem->pfActivate = NULL;
    prItem->pfDeactivate = NULL;	
    prItem->pfOp = NULL;
    prItem->prStrList = NULL;
    return i4Ret;
}

INT32 _InitList(WIDGET_LIST_T* prList)
{
    if(prList == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prList->prCurr = NULL;
    prList->prHead = NULL;
    prList->prTail = NULL;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 _InitListElement(WIDGET_LIST_ELEMENT_T* prEle)
{
    if(prEle == NULL)
    {
        return (INT32)E_WIDGET_MSG_OK;
    }
    prEle->prNext = NULL;
    prEle->prPrev = NULL;
    return (INT32)E_WIDGET_MSG_OK;
}


INT32 _InitStringList(WIDGET_STRINGLIST_T* prList)
{
    if(prList == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prList->prCurr = NULL;
    prList->prHead = NULL;
    prList->prTail = NULL;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 _InitStringListElement(WIDGET_STRINGLIST_ELEMENT_T* prEle)
{
    if(prEle == NULL)
    {
        return (INT32)E_WIDGET_MSG_OK;
    }
    prEle->prNext = NULL;
    prEle->prPrev = NULL;
    return (INT32)E_WIDGET_MSG_OK;
}

WIDGET_MENULIST_T* WIDGET_NewMenuList(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp)
{
    WIDGET_MENULIST_T* prNew;
    if(szTitle == NULL)
    {
        return NULL;
    }
    if( (prNew = (WIDGET_MENULIST_T*)x_mem_alloc(sizeof(WIDGET_MENULIST_T))) == NULL)
    {
        return NULL;
    }
    LOG(6, "Allocate at 0x%x\n", prNew);
    LOG(6, "STR: %s, LEN: %d\n", szTitle, x_strlen(szTitle));
    if( ( prNew->szTitle = (CHAR*)x_mem_alloc(x_strlen(szTitle) + 1) ) == NULL)
    {
        x_mem_free(prNew);	
        return NULL;
    }
    if(x_strcpy(prNew->szTitle, szTitle) == NULL)
    {
        VERIFY(0);
    }
    prNew->u4ColorMode = u4ColorMode;
    prNew->prBmp = prBmp;
    if(_InitMenuList(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_LIST_ELEMENT_T* WIDGET_NewListElement(const CHAR* szStr)
{
    WIDGET_LIST_ELEMENT_T* prNew;
    if(szStr == NULL)
    {
        return NULL;
    }
    if( (prNew = (WIDGET_LIST_ELEMENT_T*)x_mem_alloc(sizeof(WIDGET_LIST_ELEMENT_T))) == NULL)
    {
        return NULL;
    }	
    if( (prNew->szStr = (CHAR*)x_mem_alloc(x_strlen(szStr) + 1)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    VERIFY(x_strcpy(prNew->szStr, szStr) != NULL);
    if(_InitListElement(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szStr);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_LIST_ELEMENT_T* WIDGET_SearchListElement(const CHAR* szStr, const WIDGET_LIST_T* prList)
{
    WIDGET_LIST_ELEMENT_T* prEle;
    VERIFY(prList != NULL);
    prEle = prList->prHead;
    if(prEle == NULL)
    {
        return NULL;
    }
    if(x_strcmp(prEle->szStr, szStr) == 0)
    {
        return prEle;
    }
    for(prEle = prEle->prNext; prEle != prList->prHead; prEle = prEle->prNext)
    {
        if(x_strcmp(prEle->szStr, szStr) == 0)
        {
            return prEle;
        }
    }
    return NULL;
}

WIDGET_STRINGLIST_ELEMENT_T* WIDGET_NewStringListElement(const CHAR* szStr)
{
    WIDGET_STRINGLIST_ELEMENT_T* prNew;
    if(szStr == NULL)
    {
        return NULL;
    }
    if( (prNew = (WIDGET_STRINGLIST_ELEMENT_T*)x_mem_alloc(sizeof(WIDGET_STRINGLIST_ELEMENT_T))) == NULL)
    {
        return NULL;
    }	
    if( (prNew->szStr = (CHAR*)x_mem_alloc(x_strlen(szStr) + 1)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    VERIFY(x_strcpy(prNew->szStr, szStr) != NULL);
    if(_InitStringListElement(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szStr);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_LIST_T* WIDGET_NewList(VOID)
{
    WIDGET_LIST_T* prNew;
    if( (prNew = (WIDGET_LIST_T*)x_mem_alloc(sizeof(WIDGET_LIST_T))) == NULL)
    {
        return NULL;
    }
    if(_InitList(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_STRINGLIST_T* WIDGET_NewStringList(VOID)
{
    WIDGET_STRINGLIST_T* prNew;
    if( (prNew = (WIDGET_STRINGLIST_T*)x_mem_alloc(sizeof(WIDGET_STRINGLIST_T))) == NULL)
    {
        return NULL;
    }
    if(_InitStringList(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_MENU_T* WIDGET_NewMenu(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp, WIDGET_BITMAP_T* prBmpInactive, WIDGET_BITMAP_T* prBmpDisable)
{
    WIDGET_MENU_T* prNew;
    if(szTitle == NULL)
    {
        return NULL;
    }	
    if( (prNew = (WIDGET_MENU_T*)x_mem_alloc(sizeof(WIDGET_MENU_T))) == NULL)
    {
        return NULL;
    }	
    if( (prNew->szTitle = (CHAR*)x_mem_alloc(x_strlen(szTitle) + 1)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    VERIFY(x_strcpy(prNew->szTitle, szTitle) != NULL);
    prNew->u4ColorMode = u4ColorMode;
    prNew->prBmp = prBmp;
    prNew->prBmpInactive = prBmpInactive;
    prNew->prBmpDisable = prBmpDisable;
    
    if(_InitMenu(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

INT32 WIDGET_SetAction(
	WIDGET_MENUITEM_T* prItem,
	BOOL (*pfHandler)(UINT32, void*), 
	INT32 (*pfOp)(void*), 
	INT32 (*pfActivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32),
	INT32 (*pfDeactivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32))
{
    VERIFY(prItem != NULL);
    prItem->pfHandler = pfHandler;
    prItem->pfActivate = pfActivate;
    prItem->pfDeactivate = pfDeactivate;
    prItem->pfOp = pfOp;
    return (INT32)E_WIDGET_MSG_OK;
}

WIDGET_MENUITEM_RANGEDVALUE_T* WIDGET_NewMenuItem_RangedValue(const CHAR* szTitle, INT32 i4InitValue, INT32 i4Min, INT32 i4Max, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp)
{
    WIDGET_MENUITEM_RANGEDVALUE_T* prNew;
    UNUSED(prBmp);
    if(szTitle == NULL)
    {
        return NULL;
    }	
    if( (prNew = (WIDGET_MENUITEM_RANGEDVALUE_T*)x_mem_alloc(sizeof(WIDGET_MENUITEM_RANGEDVALUE_T))) == NULL)
    {
        return NULL;
    }	
    //if( (prNew->szTitle = (CHAR*)BSP_AllocDmaMemory(x_strlen(szTitle)+1)) == NULL)
    if( (prNew->szTitle = (CHAR*)x_mem_alloc(_MAX_TITLE_LENGTH)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    //x_strcpy(prNew->szTitle, szTitle);
    VERIFY(x_strncpy(prNew->szTitle, szTitle, _MAX_TITLE_LENGTH) != NULL);
    prNew->u4ColorMode = u4ColorMode;
    if(_InitMenuItem_RangedValue(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    prNew->i4Max = i4Max;
    prNew->i4Min = i4Min;
    prNew->i4Value = i4InitValue;
    /* */
    return prNew;
}

WIDGET_MENUITEM_BINARYVALUE_T* WIDGET_NewMenuItem_BinaryValue(const CHAR* szTitle, BOOL fgInitValue, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp)
{
    WIDGET_MENUITEM_BINARYVALUE_T* prNew;
    UNUSED(prBmp);
    
    if(szTitle == NULL)
    {
        return NULL;
    }	
    if( (prNew = (WIDGET_MENUITEM_BINARYVALUE_T*)x_mem_alloc(sizeof(WIDGET_MENUITEM_BINARYVALUE_T))) == NULL)
    {
        return NULL;
    }
    //if( (prNew->szTitle = (CHAR*)BSP_AllocDmaMemory(x_strlen(szTitle)+1)) == NULL)
    if( (prNew->szTitle = (CHAR*)x_mem_alloc(_MAX_TITLE_LENGTH)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    //x_strcpy(prNew->szTitle, szTitle);
    VERIFY(x_strncpy(prNew->szTitle, szTitle, _MAX_TITLE_LENGTH) != NULL);
    prNew->u4ColorMode = u4ColorMode;
    if(_InitMenuItem_BinaryValue(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    prNew->fgOn = fgInitValue;
    /* */
    return prNew;
}

WIDGET_MENUITEM_ACTION_T* WIDGET_NewMenuItem_Action(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp)
{
    WIDGET_MENUITEM_ACTION_T* prNew;
    UNUSED(prBmp);
    
    if(szTitle == NULL)
    {
        return NULL;
    }	
    if( (prNew = (WIDGET_MENUITEM_ACTION_T*)x_mem_alloc(sizeof(WIDGET_MENUITEM_ACTION_T))) == NULL)
    {
        return NULL;
    }	
    //if( (prNew->szTitle = (CHAR*)BSP_AllocDmaMemory(x_strlen(szTitle)+1)) == NULL)
    if( (prNew->szTitle = (CHAR*)x_mem_alloc(_MAX_TITLE_LENGTH)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    //x_strcpy(prNew->szTitle, szTitle);
    VERIFY(x_strncpy(prNew->szTitle, szTitle, _MAX_TITLE_LENGTH) != NULL);
    prNew->u4ColorMode = u4ColorMode;
    if(_InitMenuItem_Action(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

WIDGET_MENUITEM_OPTION_T* WIDGET_NewMenuItem_Option(const CHAR* szTitle, UINT32 u4ColorMode, WIDGET_BITMAP_T* prBmp)
{
    WIDGET_MENUITEM_OPTION_T* prNew;
    UNUSED(prBmp);
    
    if(szTitle == NULL)
    {
        return NULL;
    }	
    if( (prNew = (WIDGET_MENUITEM_OPTION_T*)x_mem_alloc(sizeof(WIDGET_MENUITEM_OPTION_T))) == NULL)
    {
        return NULL;
    }	
    //if( (prNew->szTitle = (CHAR*)BSP_AllocDmaMemory(x_strlen(szTitle)+1)) == NULL)
    if( (prNew->szTitle = (CHAR*)x_mem_alloc(_MAX_TITLE_LENGTH)) == NULL)
    {
        x_mem_free(prNew);
        return NULL;
    }
    //x_strcpy(prNew->szTitle, szTitle);
    VERIFY(x_strncpy(prNew->szTitle, szTitle, _MAX_TITLE_LENGTH) != NULL);
    prNew->u4ColorMode = u4ColorMode;
    if(_InitMenuItem_Option(prNew) != (INT32)E_WIDGET_MSG_OK)
    {
        x_mem_free(prNew->szTitle);
        x_mem_free(prNew);
        return NULL;
    }
    /* */
    return prNew;
}

INT32 WIDGET_AddStringListElement(WIDGET_STRINGLIST_T* prList, const CHAR* szStr)
{
    WIDGET_STRINGLIST_ELEMENT_T *prEle, *prTemp;

    VERIFY(prList != NULL);
    
    if(szStr == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prEle = WIDGET_NewStringListElement(szStr);
    
    // First element to add
    if(prList->prHead == NULL)
    {
        prList->prHead = prEle;
        prList->prTail = prEle;
        prList->prCurr = prEle;
        prList->prHead->prNext = prList->prHead;
        prList->prHead->prPrev = prList->prHead;
    }
    else
    {
        prTemp = prList->prTail;
        // Old tail's next points to new element
        prList->prTail->prNext = prEle;
        
        // Update newly added element's prev and next pointers
        prList->prTail = prEle;
        prList->prTail->prNext = prList->prHead;
        prList->prTail->prPrev = prTemp;
        
        // Head's prev pointer points to new element
        prList->prHead->prPrev = prList->prTail;
    }
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_RemoveLastStringListElement(WIDGET_STRINGLIST_T* prList)
{
    WIDGET_STRINGLIST_ELEMENT_T* prTemp;
    if(prList == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
        // only 1 node
    if (prList->prHead == prList->prTail)
    {
        prList->prCurr = NULL;
        prList->prHead = NULL;
        prList->prTail = NULL;
    }
    else // more than 1 node
    {
        prTemp = prList->prTail->prPrev;
        prTemp->prNext = NULL;
        prList->prTail = prTemp;
        prList->prHead->prPrev = prList->prTail;
    }
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_FreeStringListElement(WIDGET_STRINGLIST_ELEMENT_T* prEle)
{
    if(prEle != NULL)
    {
        x_mem_free(prEle->szStr);
        x_mem_free(prEle);
        return (INT32)E_WIDGET_MSG_OK;
    }
    return (INT32)E_WIDGET_MSG_NULL;
}

INT32 WIDGET_AddListElement(WIDGET_LIST_T* prList, const CHAR* szStr, VOID* pvContent)
{
    WIDGET_LIST_ELEMENT_T *prEle, *prTemp;
    VERIFY(prList != NULL);
    if(szStr == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prEle = WIDGET_NewListElement(szStr);
    prEle->pvContent = pvContent;
    // First element to add
    if(prList->prHead == NULL)
    {
        prList->prHead = prEle;
        prList->prTail = prEle;
        prList->prCurr = prEle;
        prList->prHead->prNext = prList->prHead;
        prList->prHead->prPrev = prList->prHead;
    }
    else
    {
        prTemp = prList->prTail;
        // Old tail's next points to new element
        prList->prTail->prNext = prEle;
        
        // Update newly added element's prev and next pointers
        prList->prTail = prEle;
        prList->prTail->prNext = prList->prHead;
        prList->prTail->prPrev = prTemp;
        
        // Head's prev pointer points to new element
        prList->prHead->prPrev = prList->prTail;
    }
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_RemoveLastListElement(WIDGET_LIST_T* prList)
{
    WIDGET_LIST_ELEMENT_T* prTemp;
    if(prList == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
        // only 1 node
    if (prList->prHead == prList->prTail)
    {
        prList->prCurr = NULL;
        prList->prHead = NULL;
        prList->prTail = NULL;
    }
    else // more than 1 node
    {
        prTemp = prList->prTail->prPrev;
        prTemp->prNext = NULL;
        prList->prTail = prTemp;
        prList->prHead->prPrev = prList->prTail;
    }
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_FreeListElement(WIDGET_LIST_ELEMENT_T* prEle)
{
    if(prEle != NULL)
    {
        x_mem_free(prEle->szStr);
        x_mem_free(prEle);
        return (INT32)E_WIDGET_MSG_OK;
    }
    return (INT32)E_WIDGET_MSG_NULL;
}

INT32 WIDGET_AddStringList(WIDGET_MENUITEM_OPTION_T* prItem, WIDGET_STRINGLIST_T* prList)
{
    if( (prItem == NULL) || (prList == NULL) )
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prItem->prStrList = prList;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_AddMenuList(WIDGET_OSDINFO_T* prOSD, WIDGET_MENULIST_T* prList)
{
    if( (prOSD == NULL) || (prList == NULL) )
    {
    	return (INT32)E_WIDGET_MSG_NULL;
    }
    prOSD->prList = prList;
    prList->prParentOSD = prOSD;
    prList->u4X = 0;
    prList->u4Y = 0;
    prList->u4W = prOSD->u4W;
    prList->u4H = prOSD->u4H;
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_AddMenu(WIDGET_MENULIST_T* prList, WIDGET_MENU_T* prMenu)
{
    WIDGET_MENU_T* prTemp;
    //UINT32 u4PrevX;

    if( (prMenu == NULL) || (prList == NULL) )
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    if(prList->prHead == NULL)
    {
    
        if(prList->prParentOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
        {
            // Set title position		
            prMenu->u4TitleX = 0;
            prMenu->u4TitleY = 0;
            // Set inital position corresponding to menutab
            prMenu->u4X = 0;
            prMenu->u4Y = prList->u4H / 8;
            prMenu->u4W = prList->u4W;
            prMenu->u4H = prList->u4H - (prList->u4H / 8);
        }
        else
        {	// Set title position		
            prMenu->u4TitleX = 10;
            prMenu->u4TitleY = 10;
            // Set inital position corresponding to menutab
            prMenu->u4X = prList->u4W / 4;
            prMenu->u4Y = prList->u4H / 12;
            prMenu->u4W = prList->u4W - (prList->u4W / 4);
            prMenu->u4H = prList->u4H;
        }
        
        prList->prCurr = prMenu;
        prList->prHead = prMenu;
        prList->prTail = prMenu;
        prList->prHead->prNext = prList->prHead;
        prList->prHead->prPrev = prList->prHead;       		
    }
    else
    {
        prTemp = prList->prTail;
        
        if(prList->prParentOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
        {
            // Set title position 
            prMenu->u4TitleX = prList->prTail->u4TitleX + (prList->u4W / 5);
            prMenu->u4TitleY = 0;
            // Set inital position corresponding to menutab
            prMenu->u4X = 0;
            prMenu->u4Y = prList->u4H / 8;
            prMenu->u4W = prList->u4W;
            prMenu->u4H = prList->u4H;
        }
        else
        {
            // Set title position 
            prMenu->u4TitleX = 10;
            prMenu->u4TitleY = prList->prTail->u4TitleY + (prList->u4H / 7);
            // Set inital position corresponding to menutab
            prMenu->u4X = prList->u4W / 4;
            prMenu->u4Y = prList->u4H / 12;
            prMenu->u4W = prList->u4W - (prList->u4W / 4);
            prMenu->u4H = prList->u4H;
        }
    	
        // Old tail's next points to new element
        prList->prTail->prNext = prMenu;
        
        // Update newly added element's prev and next pointers
        prList->prTail = prMenu;
        prList->prTail->prNext = prList->prHead;
        prList->prTail->prPrev = prTemp;
        
        // Head's prev pointer points to new element
        prList->prHead->prPrev = prList->prTail;
    }
    // Back-points to its container
    prMenu->prParentList = prList;
    
    LOG(6, "Add Menu %s at: (%d, %d) whose size: (%d * %d)\n", 
    	prMenu->szTitle, prMenu->u4X, prMenu->u4Y, prMenu->u4W, prMenu->u4H);
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_AddMenuItem(WIDGET_MENU_T* prMenu, WIDGET_MENUITEM_T* prItem)
{
    WIDGET_MENUITEM_T* prTemp;
    UINT32 u4BorderW;
    //UINT32 u4BorderColor;
    VERIFY(prMenu != NULL);
    u4BorderW = prMenu->u4BorderW;
    //u4BorderColor = prMenu->u4BorderColor;
    if(prItem == NULL)
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    if(prMenu->prHead == NULL)
    {
        if(prMenu->prParentList->prParentOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
        {
            // Set inital position corresponding to menu
            prItem->u4X = u4BorderW + (prMenu->u4W / 8);
            prItem->u4Y = u4BorderW + ((prMenu->u4H / 8) * 2);
            prItem->u4W = (prMenu->u4W - ((2 * prMenu->u4W) /8)) - (2 * prMenu->u4BorderW);
            prItem->u4H = prMenu->u4H / 8;
        
        }
        else
        {
            // Set inital position corresponding to menu
            prItem->u4X = (prMenu->u4W / 3) + 50;
            prItem->u4Y = prMenu->u4H / 12;
            prItem->u4W = prMenu->u4W - 80;
            prItem->u4H = prMenu->u4H / 12;
        			
        }
        prMenu->prCurr = prItem;
        prMenu->prHead = prItem;
        prMenu->prTail = prItem;
        prMenu->prHead->prNext = prMenu->prHead; 
        prMenu->prHead->prPrev = prMenu->prHead;

        prItem->u4Index = 0;
        prMenu->u4NumOfItem = 1;
        prMenu->prPageStart = prItem;
        
    }
    else
    {
        prTemp = prMenu->prTail;
        
        if(prMenu->prParentList->prParentOSD->u4ColorMode == (UINT32)CM_ARGB8888_DIRECT32)
        {
            // Set inital position corresponding to menu
            prItem->u4X = u4BorderW + (prMenu->u4W / 8);
            prItem->u4Y = prTemp->u4Y + (prMenu->u4H / 8);
            prItem->u4W = (prMenu->u4W - (prMenu->u4W /8)) - (2 * prMenu->u4BorderW);
            prItem->u4H = prMenu->u4H / 8;
        }
        else
        {
            // Set inital position corresponding to menu
            prItem->u4X = (prMenu->u4W / 3) + 50;
            prItem->u4Y = prTemp->u4Y + (prMenu->u4H / 12);
            prItem->u4W = prMenu->u4W - 80;
            prItem->u4H = prMenu->u4H / 12;
        }
        
        // Old tail's next points to new element
        prMenu->prTail->prNext = prItem;
        
        // Update newly added element's prev and next pointers
        prMenu->prTail = prItem;
        prMenu->prTail->prNext = prMenu->prHead;
        prMenu->prTail->prPrev = prTemp;
        
        // Head's prev pointer points to new element
        prMenu->prHead->prPrev = prMenu->prTail;

        prItem->u4Index = prMenu->u4NumOfItem++;
        
    }
    // Back-points to its container
    prItem->prParentMenu = prMenu;
    
    LOG(6, "Add MenuItem %s at: (%d, %d) whose size: (%d * %d)\n", 
    	prItem->szTitle, prItem->u4X, prItem->u4Y, prItem->u4W, prItem->u4H);
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_SetOp(WIDGET_MENUITEM_T* prItem, INT32 (*pfOp)(void*))
{
    if( (prItem == NULL) || (pfOp == NULL) )
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    prItem->pfOp = pfOp;
    return (INT32)E_WIDGET_MSG_OK;
}

WIDGET_MENU_T* WIDGET_LookupMenu(const WIDGET_MENULIST_T* prList, const CHAR* szMenu)
{
    WIDGET_MENU_T* prMenu;
    //WIDGET_MENUITEM_T* prItem;
    VERIFY(prList != NULL);
    VERIFY(szMenu != NULL);
    prMenu = prList->prHead;
    if(x_strcmp(prMenu->szTitle, szMenu) == 0)
    {
        return prMenu;
    }
    // menu list has more than 1 menus
    for(prMenu = prMenu->prNext; prMenu != prList->prHead; prMenu = prMenu->prNext)
    {
        if(x_strcmp(prMenu->szTitle, szMenu) == 0)
        {
            return prMenu;
        }
    }
    return NULL;	
}


WIDGET_MENUITEM_T* WIDGET_LookupMenuItem(const WIDGET_MENULIST_T* prList, const CHAR* szMenu, const CHAR* szItem)
{
    WIDGET_MENU_T* prMenu;
    WIDGET_MENUITEM_T* prItem;
    VERIFY(prList != NULL);
    prMenu = prList->prHead;
    if(x_strcmp(prMenu->szTitle, szMenu) == 0)
    {
        prItem = prMenu->prHead;
        if(x_strcmp(prItem->szTitle, szItem) == 0)
        {
            return prItem;		
        }
        // menu has more than 1 items
        for(prItem = prItem->prNext; prItem != prMenu->prHead; prItem = prItem->prNext)
        {
            if(x_strcmp(prItem->szTitle, szItem) == 0)
            {
                return prItem;
            }
        }
    
    }
    // menu list has more than 1 menus
    for(prMenu = prMenu->prNext; prMenu != prList->prHead; prMenu = prMenu->prNext)
    {
        if(x_strcmp(prMenu->szTitle, szMenu) == 0)
        {
            prItem = prMenu->prHead;
            if(x_strcmp(prItem->szTitle, szItem) == 0)
            {
                return prItem;		
            }
            
            // menu has more than 1 items
            for(prItem = prItem->prNext; prItem != prMenu->prHead; prItem = prItem->prNext)
            {
                if(x_strcmp(prItem->szTitle, szItem) == 0)
                {
            	      return prItem;
                }
            }
        }
    }
    return NULL;	
}

// add by ytchen----------------------------------------------------------------
INT32 WIDGET_RemoveLastMenuItem(WIDGET_MENU_T* prMenu)
{
    WIDGET_MENUITEM_T* prTemp;
    if( (prMenu == NULL) || (prMenu->prHead == NULL) || (prMenu->prTail == NULL) )
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    
    // only 1 node
    if (prMenu->prHead == prMenu->prTail)
    {
        prMenu->prCurr = NULL;
        prMenu->prHead = NULL;
        prMenu->prTail = NULL;

        prMenu->prPageStart = NULL;
        prMenu->u4NumOfItem = 0;        
    }
    else // more than 1 node
    {
        prTemp = prMenu->prTail->prPrev;
        prTemp->prNext = NULL;
        prMenu->prTail = prTemp;
        prMenu->prHead->prPrev = prMenu->prTail;

        prMenu->prPageStart = prMenu->prHead;
        prMenu->u4NumOfItem--;
        
    }
    
    return (INT32)E_WIDGET_MSG_OK;
}

INT32 WIDGET_FreeMenuItem(WIDGET_MENUITEM_T* prItem)
{
    if(prItem != NULL)
    {
        x_mem_free((void*)prItem->szTitle);
        x_mem_free((void*)prItem);
        return (INT32)E_WIDGET_MSG_OK;
    }
    return (INT32)E_WIDGET_MSG_NULL;
}

INT32 WIDGET_MenuItemSetTitle(const WIDGET_MENUITEM_T* prItem, const CHAR* szTitle)
{
    //CHAR* szTemp;
    if ( (prItem == NULL) || (szTitle == NULL) )
    {
        return (INT32)E_WIDGET_MSG_NULL;
    }
    
    // to enlarge title buf if not enough
    /*    if (x_strlen(szTitle) + 1 > x_strlen(prItem->szTitle))
    {
        BSP_FreeDmaMemory(prItem->szTitle);
        szTemp = (CHAR*)BSP_AllocDmaMemory(x_strlen(szTitle) + 1);
        if (szTemp == NULL)
        {
            return E_WIDGET_MSG_OUT_OF_MEMORY;
        }
    
        BSP_FreeDmaMemory(prItem->szTitle);
        prItem->szTitle = szTemp;
    }*/
    VERIFY(x_strncpy(prItem->szTitle, szTitle, _MAX_TITLE_LENGTH) != NULL);
    
    return (INT32)E_WIDGET_MSG_OK;
}


void _Dummy(void)
{
	UNUSED(rDefaultFontDisableItem);
	UNUSED(rDefaultFontDisableItem16);
}


