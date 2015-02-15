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
 * $RCSfile: widget.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget.h
 *  This header file declares public function prototypes of widget.
 */

#ifndef WIDGET_H
#define WIDGET_H

typedef struct _WIDGET_MENUITEM_T WIDGET_MENUITEM_T;
typedef struct _WIDGET_MENU_T WIDGET_MENU_T; 
typedef struct _WIDGET_MENULIST_T WIDGET_MENULIST_T;
typedef struct _WIDGET_OSDINFO_T WIDGET_OSDINFO_T;

typedef struct _WIDGET_BITMAP_T
{
    CHAR* szName;
    UINT8* pu1Address;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4PitchSize;
    UINT32 u4Size;
} WIDGET_BITMAP_T;

	
typedef struct _WIDGET_FONT_PROPERTY_T {
    UINT32 u4FrColor;
    UINT32 u4BgColor;
    UINT32 u4BgWidth;
    UINT32 u4FontSize;
} WIDGET_FONT_PROPERTY_T;

// Common definition of all WIDGET_MENUITEM_T type structure
#define WIDGET_MENUITEM_COMMON \
    UINT32 u4Size;\
    WIDGET_MENUITEM_T* prNext;\
    WIDGET_MENUITEM_T* prPrev;\
    CHAR* szTitle;\
    UINT32 u4X;\
    UINT32 u4Y;\
    UINT32 u4W;\
    UINT32 u4H;\
    UINT32 u4BorderW;\
    UINT32 u4BorderColor;\
    UINT32 u4BgColor;\
    WIDGET_FONT_PROPERTY_T* prFontActive;\
    WIDGET_FONT_PROPERTY_T* prFontInactive;\
    WIDGET_FONT_PROPERTY_T* prFontValue;\
    WIDGET_MENU_T* prParentMenu;\
    BOOL fgEnable;\
    BOOL (*pfHandler)(UINT32, void*);\
    INT32 (*pfOp)(void*);\
    INT32 (*pfActivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32);\
    INT32 (*pfDeactivate)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32);\
    UINT32 u4ColorMode;\
    void* pvCallbackTag;\
    UINT32 u4Index;


struct _WIDGET_MENUITEM_T {
    WIDGET_MENUITEM_COMMON
};

struct _WIDGET_MENU_T {
    UINT32 u4Size;
    // Pointers maintaining its doubly-linked menuitems
    WIDGET_MENUITEM_T* prHead;
    WIDGET_MENUITEM_T* prTail;
    WIDGET_MENUITEM_T* prCurr;

    WIDGET_MENUITEM_T* prPageStart;
    
    // Doublely-linked menu list
    WIDGET_MENU_T* prNext;
    WIDGET_MENU_T* prPrev;
    CHAR* szTitle;
    // Position
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4W;
    UINT32 u4H;
    UINT32 u4TitleX;
    UINT32 u4TitleY;
    
    UINT32 u4BorderW;
    UINT32 u4BorderColor;
    UINT32 u4BgColor;
    
    // Font properties when active, inactive, disable
    WIDGET_FONT_PROPERTY_T* prFontActive;
    WIDGET_FONT_PROPERTY_T* prFontInactive;
    WIDGET_FONT_PROPERTY_T* prFontDisable;
    // Back-pointer to its container
    WIDGET_MENULIST_T* prParentList;
    // Enable or not
    BOOL fgEnable;
    
    UINT32 u4ColorMode;
    WIDGET_BITMAP_T* prBmp;
    WIDGET_BITMAP_T* prBmpInactive;
    WIDGET_BITMAP_T* prBmpDisable;

    UINT32 u4NumOfItem;
    UINT32 u4NumOfItemPage;
};

struct _WIDGET_MENULIST_T {
    UINT32 u4Size;
    // Pointers maintaining its doubly-linked menus
    WIDGET_MENU_T* prHead;
    WIDGET_MENU_T* prTail;
    WIDGET_MENU_T* prCurr;
    CHAR* szTitle;
    // Position
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4W;
    UINT32 u4H;
    // Font properties when active, inactive, disable
    WIDGET_FONT_PROPERTY_T* prFontActive;
    WIDGET_FONT_PROPERTY_T* prFontInactive;
    WIDGET_FONT_PROPERTY_T* prFontDisable;
    // Back-pointer to its container
    WIDGET_OSDINFO_T* prParentOSD;
    // Enable or not
    BOOL fgEnable;
    
    UINT32 u4ColorMode;
    WIDGET_BITMAP_T* prBmp;
};

typedef struct _WIDGET_STRINGLIST_ELEMENT_T WIDGET_STRINGLIST_ELEMENT_T;
struct _WIDGET_STRINGLIST_ELEMENT_T {
    CHAR* szStr;
    WIDGET_STRINGLIST_ELEMENT_T* prNext;
    WIDGET_STRINGLIST_ELEMENT_T* prPrev;
}; 

typedef struct _WIDGET_STRINGLIST_T {
    WIDGET_STRINGLIST_ELEMENT_T* prCurr;
    WIDGET_STRINGLIST_ELEMENT_T* prHead;
    WIDGET_STRINGLIST_ELEMENT_T* prTail;
} WIDGET_STRINGLIST_T;

typedef struct _WIDGET_LIST_ELEMENT_T WIDGET_LIST_ELEMENT_T;
struct _WIDGET_LIST_ELEMENT_T {
    CHAR* szStr;
    WIDGET_LIST_ELEMENT_T* prNext;
    WIDGET_LIST_ELEMENT_T* prPrev;
    VOID* pvContent;
}; 

typedef struct _WIDGET_LIST_T {
    WIDGET_LIST_ELEMENT_T* prCurr;
    WIDGET_LIST_ELEMENT_T* prHead;
    WIDGET_LIST_ELEMENT_T* prTail;
} WIDGET_LIST_T;
	
typedef struct _WIDGET_MENUITEM_RANGEDVALUE_T {
    WIDGET_MENUITEM_COMMON
    INT32 i4Value;
    INT32 i4Max;
    INT32 i4Min;
} WIDGET_MENUITEM_RANGEDVALUE_T;

typedef struct _WIDGET_MENUITEM_BINARYVALUE_T {
    WIDGET_MENUITEM_COMMON
    BOOL fgOn;
} WIDGET_MENUITEM_BINARYVALUE_T;

typedef struct _WIDGET_MENUITEM_ACTION_T {
    WIDGET_MENUITEM_COMMON
} WIDGET_MENUITEM_ACTION_T;

typedef struct _WIDGET_MENUITEM_OPTION_T {
    WIDGET_MENUITEM_COMMON
    WIDGET_STRINGLIST_T* prStrList;
} WIDGET_MENUITEM_OPTION_T;


struct _WIDGET_OSDINFO_T {
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4W;
    UINT32 u4H;
    UINT32 u4ColorMode;
    UINT32 u4Plane;
    UINT8* apu1OSDBuffer;
    UINT8* apu1OSDBackstage;
    UINT32 au4OSDList;

    UINT32 u4Region;
    
    WIDGET_MENULIST_T* prList;
    BOOL fgEnable;
    
    BOOL fgNoTitleBmp;
};


typedef enum _E_WIDGET_MSG {
    E_WIDGET_MSG_OK,
    E_WIDGET_MSG_NULL,
    E_WIDGET_MSG_NOT_SUPPORT,
    E_WIDGET_MSG_INVLIAD_ARG,
    E_WIDGET_MSG_ERROR,
       E_WIDGET_MSG_OUT_OF_MEMORY
} E_WIDGET_MSG;


#endif //WIDGET_H

