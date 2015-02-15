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
 * $RCSfile: nav.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget.h
 *  This header file declares public function prototypes of widget.
 */

#ifndef NAV_H
#define NAV_H

//#define CC_NAV_DOLBY_TEST 1

#define NAV_ATV_AUDIOSTOP_WORKAROUND 0
#define NAV_HDMI_AUDIO_OK 1

#define NAV_NSTOMAX 0

#define NAV_PIP_DTV_CVBS1 0

#define NAV_BACKDOOR_DTVSCAN 1

#define NAV_BLACKMUTE_AP 1

#define NAV_MENU_MAX_NUM 5

#define NAV_BORDER_SCALING_FACTOR 4

#define NAV_NOR_MAGIC "MT5332"

typedef enum _E_NAV_MENUITEM_TYPE {
	E_NAV_MENUITEM_TYPE_NONE,
	E_NAV_MENUITEM_TYPE_RANGEDVALUE,
	E_NAV_MENUITEM_TYPE_BINARYVALUE,
	E_NAV_MENUITEM_TYPE_ACTION,
	E_NAV_MENUITEM_TYPE_OPTION,
	E_NAV_MENUITEM_TYPE_LEAF
} E_NAV_MENUITEM_TYPE;

typedef enum _E_NAV_MSG {
	E_NAV_MSG_OK,
	E_NAV_MSG_NOT_SUPPORT,
	E_NAV_MSG_PARTIAL_OK,
	E_NAV_MSG_CHANNEL_NOT_SCANNED,
	E_NAV_MSG_HAS_INIT,
    	E_NAV_MSG_WRONG_STATE,
	E_NAV_MSG_ERROR
} E_NAV_MSG;

typedef struct _NAV_OPTION_T {
	// Input src enable list
	BOOL fgEnableATV;
	BOOL fgEnableCVBS1;
	BOOL fgEnableCVBS2;
	BOOL fgEnableCVBS3;
	BOOL fgEnableS1;
	BOOL fgEnableS2;
	BOOL fgEnableS3;
	BOOL fgEnableYPbPr1;
	BOOL fgEnableYPbPr2;
	BOOL fgEnableHDMI;
	BOOL fgEnableHDMI2;
	BOOL fgEnableVGA;
	
	BOOL fgEnableDTV;
	BOOL fgEnableI1394;

	// Audio enable
	BOOL fgEnableAudio; 

	// OSD configuration
	BOOL fgEnableOSD32;

	BOOL fgDTVFullScan;

       UINT32 u4PanelId;
	
} NAV_OPTION_T; 

typedef struct _NAV_MENUITEM_T NAV_MENUITEM_T;
struct _NAV_MENUITEM_T {
	CHAR szTitle[20];
	E_NAV_MENUITEM_TYPE eItemType;
	NAV_MENUITEM_T* prSubItem;
	UINT32 u4NumSubItem;
	INT32 (*pfOp)(void*);
	INT32 i4Value;
	INT32 i4Min;
	INT32 i4Max;
};

typedef struct _NAV_MENUTITLE_T NAV_MENUTITLE_T;
struct _NAV_MENUTITLE_T
{
	const CHAR* szTitle;
	NAV_MENUITEM_T* prItem;
} ;

typedef struct _NAV_CONF_T {
	BOOL u4MenuOn[NAV_MENU_MAX_NUM];
	UINT32 u4Path;
} NAV_CONF_T;

typedef struct _NAV_VIDEOPATH_T {
	UINT32 u4Src;
	DRV_VDO_WINDOW_T rVDPRegion;
	UINT32 u4Volume;
	UINT32 u4Brightness;
	UINT32 u4Contrast;
	UINT32 u4Hue;
	UINT32 u4Saturation;
	UINT32 u4Sharpness;
	UINT32 u4ResX;
	UINT32 u4ResY;
	BOOL fgProgressive;
	BOOL fgValid;
} NAV_VIDEOPATH_T;



EXTERN NAV_OPTION_T rNAVOption;
EXTERN WIDGET_OSDINFO_T arWidgetOSD[];
EXTERN WIDGET_LIST_T* prInputSrcList;
EXTERN WIDGET_LIST_T* prInputSrcListPIP;
EXTERN WIDGET_BITMAP_T arBmpTbl[];

EXTERN BOOL fgInfoOSDOn;

EXTERN UINT32 u4PmxX;
EXTERN UINT32 u4PmxY;
EXTERN UINT32 u4PmxW;
EXTERN UINT32 u4PmxH;

EXTERN void _ADAC_Enable(BOOL fgEnalbe);
EXTERN WIDGET_BITMAP_T* NAV_LookupBmp(const CHAR* szStr);
EXTERN INT32 NAV_InitHandlers(void);

#endif
