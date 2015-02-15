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
 * $RCSfile: drv_od.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif

#ifdef CC_COPLAT_MT82
#include "util.h"
#endif

#include "general.h"
#include "hw_ckgen.h"
#include "hw_tcon.h"
#include "hw_lvds.h"
#include "hw_mlvds.h"
#include "hw_ycproc.h"
#include "drv_tcon.h"
#include "nptv_debug.h"
#include "fbm_drvif.h"
#include "x_timer.h"
#include "x_bim.h"
#include "panel.h"
#include "hw_od.h"
#include "drv_od.h"
#include "drvcust_if.h"
#include "pe_if.h"
#include "vdo_misc.h"
#include "drv_display.h"

/*************************************************************************  
* Constant/Configure Definitions
*************************************************************************/
#define OD_STRESS_TEST 1

#ifdef CC_SUPPORT_OD
    #define OD_ALPHA_SITE_TEST 0
#else
    #define OD_ALPHA_SITE_TEST 0
#endif

/*************************************************************************  
* Type definitions
*************************************************************************/
typedef struct
{
    UINT8 u1Flg;
    UINT32 u4TestNum;
    UINT32 u4ErrCnt;
    UINT32 u4Table;
    UINT32 u4LogCnt;
    UINT32 u4Delay;
    UINT32 u4TestMode;
} OD_LOAD_DATA;

typedef struct
{
	UINT32 u4Enable;
	UINT32 u4SrcWidth;
	UINT32 u4SrcHeight;
	UINT32 u4DstWidth;
	UINT32 u4DstHeight;
} OSD_SCALERINFO;

/*************************************************************************  
* Macro definitions
*************************************************************************/
#define OD_REGTBL_END   0xffffffff

#define OD_PANEL_WIDTH     (((PANEL_GetPanelWidth()) + 3) & 0xFFFFFFFC)
#define OD_PANEL_HEIGHT    (((PANEL_GetPanelHeight()) + 1) & 0xFFFFFFFE)  

#define OD_IDLE_FLG (1<<0)
#define IS_OD_IDLE (OdParam.u1Flg & OD_IDLE_FLG)
#define OD_READ_FLG (1<<1)
#define IS_OD_READ (OdParam.u1Flg & OD_READ_FLG)
#define OD_WRITE_OD_FLG (1<<2)
#define IS_OD_WRITE_OD (OdParam.u1Flg & OD_WRITE_OD_FLG)
#define OD_WRITE_FB_FLG (1<<3)
#define IS_OD_WRITE_FB (OdParam.u1Flg & OD_WRITE_FB_FLG)

#define OD_TABLE_VSYNC 0

#define OD_INT_BASE 26
#define FRAME_END   (1)               //[0]: frame protect frame end
#define FRAME_SYNC  (1<<1)              // [1]: frame sync set by DET8B_SYNC_POS
#define STATE_MACHINE_ERR   (1<<2)   //[2]: state machine error
#define FRAME_START     (1<<3)           //[3]: frame protect frame start
#define OD_TABLE_DMA_DONE     (1<<4)  // [4]: table DMA done

// TBD
#define IS_PANEL_PDP_OD 0

#ifdef CC_SUPPORT_STR
#define  vDrvOdReset(x)          (vIO32WriteFldAlign(CKGEN_BLOCK_RST_CFG1, x , FLD_OD_RST))
#define  vDrvOdSetClock(x)       (vIO32WriteFldAlign(CKGEN_BLOCK_CKEN_CFG1, x , FLD_OD_CKEN))
#endif
/*************************************************************************  
* Imported functions (optional)
*************************************************************************/

/*************************************************************************  
* Imported variables (optional)
*************************************************************************/

/*************************************************************************  
* Global/Static variables
*************************************************************************/

static OD_LOAD_DATA OdParam;
static UINT8 u1ODState;


#if  OD_STRESS_TEST 
static UINT8 OD_Q_Table[OD_TBL_L_SIZE];
static UINT8 FB_Q_Table[FB_TBL_SIZE];
#endif
UINT8 _u1DMAState =0;

OD_PARAM_T _ODParam;
UINT32 u4OdReg12, u4OdReg39, u4OdReg02;
UINT8 _u1ODDumpEnd =0;
EXTERN BOOL fgIsDitherInited;

UINT8 OD_Table41[OD_TBL_L_SIZE]=
{
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF,
};

#if 1
UINT8 OD_Table_test[FB_TBL_SIZE]=
/*	0	16	32	48	64	80	96	112	128	144	160	176	192	208	224	240	255*/
{
0x00, 0x1D, 0x30, 0x3B, 0x4A, 0x57, 0x5C, 0x64, 0x6B, 0x75, 0x7A, 0x81, 0x87, 0x8E, 0x93, 0x9B, 0xA0,
0x00, 0x08, 0x18, 0x2F, 0x39, 0x40, 0x4F, 0x59, 0x62, 0x6A, 0x72, 0x78, 0x7F, 0x86, 0x8C, 0x93, 0x9A,
0x00, 0x05, 0x10, 0x1B, 0x2D, 0x39, 0x42, 0x4D, 0x56, 0x60, 0x67, 0x70, 0x78, 0x80, 0x88, 0x8D, 0x93,
0x00, 0x08, 0x0E, 0x18, 0x20, 0x2D, 0x36, 0x40, 0x4E, 0x56, 0x5F, 0x67, 0x70, 0x75, 0x7D, 0x84, 0x8D,
0x00, 0x05, 0x09, 0x18, 0x20, 0x28, 0x35, 0x3E, 0x4A, 0x53, 0x5C, 0x63, 0x6D, 0x74, 0x7C, 0x84, 0x8B,
0x00, 0x06, 0x0A, 0x17, 0x20, 0x28, 0x30, 0x3C, 0x48, 0x51, 0x59, 0x63, 0x6A, 0x72, 0x7A, 0x82, 0x8A,
0x00, 0x05, 0x0A, 0x15, 0x1B, 0x28, 0x30, 0x38, 0x41, 0x4A, 0x55, 0x5E, 0x66, 0x6D, 0x77, 0x7D, 0x86,
0x00, 0x06, 0x0B, 0x12, 0x19, 0x23, 0x30, 0x38, 0x40, 0x48, 0x54, 0x5C, 0x65, 0x6D, 0x75, 0x7E, 0x85,
0x00, 0x06, 0x0A, 0x11, 0x17, 0x23, 0x2C, 0x38, 0x40, 0x48, 0x51, 0x5A, 0x64, 0x6D, 0x74, 0x7C, 0x85,
0x00, 0x05, 0x09, 0x10, 0x1A, 0x21, 0x2E, 0x38, 0x40, 0x48, 0x50, 0x58, 0x63, 0x6B, 0x73, 0x7C, 0x84,
0x00, 0x06, 0x0A, 0x10, 0x14, 0x1F, 0x27, 0x31, 0x3C, 0x48, 0x50, 0x58, 0x60, 0x6A, 0x73, 0x7B, 0x83,
0x00, 0x06, 0x0A, 0x0F, 0x14, 0x1E, 0x28, 0x32, 0x3B, 0x48, 0x50, 0x58, 0x60, 0x68, 0x73, 0x7B, 0x83,
0x00, 0x05, 0x0A, 0x0F, 0x15, 0x20, 0x28, 0x32, 0x3C, 0x45, 0x4E, 0x58, 0x60, 0x68, 0x71, 0x79, 0x82,
0x00, 0x06, 0x0A, 0x0F, 0x13, 0x1D, 0x25, 0x2F, 0x39, 0x42, 0x4C, 0x58, 0x60, 0x68, 0x70, 0x78, 0x81,
0x00, 0x06, 0x0A, 0x0E, 0x13, 0x1D, 0x25, 0x2E, 0x38, 0x42, 0x4C, 0x54, 0x60, 0x68, 0x70, 0x78, 0x80,
0x00, 0x05, 0x0A, 0x0E, 0x12, 0x1D, 0x25, 0x2D, 0x38, 0x42, 0x4C, 0x55, 0x5D, 0x68, 0x70, 0x78, 0x80,
0x00, 0x06, 0x09, 0x0E, 0x13, 0x1C, 0x23, 0x2C, 0x36, 0x41, 0x4A, 0x54, 0x5E, 0x67, 0x70, 0x78, 0x80,
};
#endif

//UINT8 PCID_Table[PCID_TBL_SIZE];


const REGTBL_T CODE  OD_INIT[]=
{
    {OD_REG00, 0x02040201, 0xffffffff},
    {OD_REG01, 0x00071401, 0x000000ff},
    {OD_REG02, 0x00000400, 0xffffffff},
    {OD_REG03, 0x00E0FF04, 0xffffffff},
    //{OD_REG06, 0x00000000, 0xffffffff}, //dram address
    //{OD_REG27, 0x000000a0, 0x000000ff}, //dram address
    //{OD_REG08, 0x04380780, 0xffffffff}, //frame protect width and heigh
    //{OD_REG09, 0x000A0048, 0xffffffff},
    //{OD_REG10, 0xffffffff, 0xffffffff},
    //{OD_REG11, 0x00081388, 0xffffffff},
    
    {OD_REG30, 0x00010026, 0xffffffff}, //set cr rate ==0x42
    //{OD_REG32, 0x04055600, 0xffffffff},
    {OD_REG33, 0x00021000, 0x003fffff},
    {OD_REG34, 0x0674A0E6, 0xffffffff},
    {OD_REG35, 0x000622E0, 0xffffffff},//0x00051170
    {OD_REG36, 0xE0849C40, 0xffffffff},//DET8B_POS=4
    {OD_REG37, 0x05CFDE08, 0xffffffff},//0x04E6D998
    {OD_REG38, 0x011F1013, 0xffffffff},//line size=0xf8
    {OD_REG39, 0x00200000, 0x00ff0000},//dram_crc_cnt=0x20
    {OD_REG40, 0x20000610, 0xffffffff},//enable GM
    {OD_REG41, 0x001E02D0, 0xffffffff},
    {OD_REG42, 0x00327C7C, 0xffffffff},
    {OD_REG43, 0x00180000, 0xffffffff}, //disable film mode detection
    {OD_REG44, 0x006400C8, 0xffffffff},
    {OD_REG45, 0x00210032, 0xfffffffc},//pcid_alig_sel=1
    {OD_REG46, 0x4E00023F, 0xffffffff},
    {OD_REG47, 0xC306B16A, 0xffffffff}, //pre_bw=3
    {OD_REG48, 0x10240408, 0xffffffff},  //???00210408???
    {OD_REG49, 0x05C00000, 0x07C00000},//new_fp 
    {OD_REG51, 0x21A1B800, 0xffffffff},//dump burst
    {OD_REG52, 0x60000044, 0xe00000ff},//DUMP_WFF_FULL_CONF=3
    {OD_REG53, 0x2FFF3E00, 0xffffffff},
    {OD_REG54, 0x80039000, 0xffffffff}, //new 3x3
    {OD_REG57, 0x352835CA, 0xffffffff}, //skip color thr 
    {OD_REG62, 0x00014438, 0xffffffff}, //pattern gen
    {OD_REG63, 0x27800898, 0xffffffff},
    {OD_REG64, 0x00438465, 0xffffffff},
    {OD_REG65, 0x01180001, 0xffffffff},
    {OD_REG66, 0x0002D000, 0xffffffff},
    {OD_REG67, 0x3FFFFFFF, 0xffffffff},
    {OD_REG68, 0x00000000, 0xffffffff},
    {OD_REG69, 0x000200C0, 0xffffffff},
    //{OD_REG78, 0x04000000, 0x0C000000}, //force_1st_frame_end

    {REGTBL_END, 0x00000000, 0x00000000}
};

/**************************************************************************  
 * Function Members
 *************************************************************************/

void vDrvODOnOff(UINT8 u1OnOff)
{
    _ODParam.u1ODCurrentONOFF = u1OnOff;
    
    if (u1OnOff)
    {
        vIO32WriteFldAlign(OD_REG12, 1, RG_OD_START);
        //vIO32WriteFldAlign(OD_REG39, 0, WDRAM_DIS);
        //vIO32WriteFldAlign(OD_REG39, 0, RDRAM_DIS);
    }
    else
    {
        vIO32WriteFldAlign(OD_REG12, 0, RG_OD_START);
        //vIO32WriteFldAlign(OD_REG39, 1, WDRAM_DIS);
        //vIO32WriteFldAlign(OD_REG39, 1, RDRAM_DIS);
    }
    fgIsDitherInited = SV_FALSE;
}

void vDrvPCIDOnOff(UINT8 u1OnOff)
{
#ifndef CC_MT5881
    vIO32WriteFldAlign(OD_REG45, u1OnOff, OD_PCID_EN);
    vIO32WriteFldAlign(OD_REG45, !u1OnOff, OD_PCID_BYPASS);
#endif
}

void vDrvFBOnOff(UINT8 u1OnOff)
{
#ifndef CC_MT5881
    vIO32WriteFldAlign(OD_REG02, !u1OnOff, FBT_BYPASS);
#endif
}

void vDrvODBypass(UINT8 u1OnOff)
{
    vIO32WriteFldAlign(OD_REG54, u1OnOff, BYPASS_ALL_SYNC_V);
}


void vDrvODPatternGen(UINT8 u1OnOff)
{
    if (u1OnOff)
    {
        vIO32WriteFldAlign(OD_REG62, 1, RG_2CH_PTGEN_START);
        vIO32WriteFldAlign(OD_REG62, (u1OnOff-1), RG_2CH_PTGEN_TYPE);
    }
    else
    {
        vIO32WriteFldAlign(OD_REG62, 0, RG_2CH_PTGEN_START);
        vIO32WriteFldAlign(OD_REG62, 4, RG_2CH_PTGEN_TYPE);
    }
}

void vDrvODReset(void)
{
    vIO32WriteFldAlign(OD_REG01, 1, OD_RST);
    vIO32WriteFldAlign(OD_REG01, 0, OD_RST);
}

void vDrvRegLoadODTable(void)
{
    OdParam.u1Flg |= OD_WRITE_OD_FLG;
}

void vDrvRegLoadFBTable(void)
{
    OdParam.u1Flg |= OD_WRITE_FB_FLG;
}

void vDrvODClearDramCRC(void)
{
    vIO32WriteFldAlign(OD_REG38, 1, DRAM_CRC_CLR);
    vIO32WriteFldAlign(OD_REG38, 0, DRAM_CRC_CLR);
}

void vDrvODWriteTable(UINT8 TableSel, UINT8 ColorSel, UINT8* pTable)
{
    UINT32 i, u4TblSize;
    UINT32 u4RegFld;
    UINT8 u1ODBypass =IO32ReadFldAlign(OD_REG02, ODT_BYPASS);
    UINT8 u1FBBypass =IO32ReadFldAlign(OD_REG02, FBT_BYPASS);
    UINT8 u1PCIDBypass =IO32ReadFldAlign(OD_REG45, OD_PCID_BYPASS);

    if (TableSel == FB_TABLE_17)
    {
        return;
    }

    if (ColorSel > 3)
    {
        return;
    }

    if (TableSel == FB_TABLE_17)
    {
        u4RegFld = TABLE_RW_SEL_FB_BGR;
    }
    else if (TableSel == PCID_TABLE_17)
    {
        u4RegFld = TABLE_RW_SEL_PCID_BGR;
    }
    else
    {
        u4RegFld = TABLE_RW_SEL_OD_BGR;
    }
    
    vIO32WriteFldAlign(OD_REG02, 1, ODT_BYPASS);
    vIO32WriteFldAlign(OD_REG02, 1, FBT_BYPASS);
    vIO32WriteFldAlign(OD_REG45, 0, OD_PCID_EN);
    vIO32WriteFldAlign(OD_REG45, 1, OD_PCID_BYPASS);
    vIO32WriteFldAlign(OD_REG04, 1, TABLE_ONLY_W_ADR_INC);	
    vIO32WriteFldAlign(OD_REG04, 0, ADDR_YX);
    vIO32WriteFldAlign(OD_REG04, 1<<ColorSel, u4RegFld);	

	if (ColorSel == 3)
	{
		vIO32WriteFldAlign(OD_REG04, 7, u4RegFld);
	}
	else
	{
		vIO32WriteFldAlign(OD_REG04, 1<<ColorSel, u4RegFld);
	}

	switch (TableSel)
	{	
		case OD_TABLE_33:
			u4TblSize = OD_TBL_M_SIZE;
			vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_41);
			vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_17);
			break;
		case OD_TABLE_41_MODE1:
		case OD_TABLE_41_MODE2:
			u4TblSize = OD_TBL_L_SIZE;
			vIO32WriteFldAlign(OD_REG32, 1, OD_IDX_41);
			vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_17);
			break;
		case FB_TABLE_17:
			u4TblSize = FB_TBL_SIZE;
			break;
		case OD_TABLE_17:
			u4TblSize = OD_TBL_S_SIZE;
			vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_41);
			vIO32WriteFldAlign(OD_REG32, 1, OD_IDX_17);
			break;
		case PCID_TABLE_17:
			u4TblSize = PCID_TBL_SIZE;
			break;	
		default:
			return;
	}		

	
    for (i=0; i<u4TblSize; i++)
    {
        vIO32WriteFldAlign(OD_REG05, *(pTable+i), TABLE_RW_DATA);
    }

    vIO32WriteFldAlign(OD_REG04, 0, OD_FLD_ALL);
    vIO32WriteFldAlign(OD_REG02, u1ODBypass, ODT_BYPASS);
    vIO32WriteFldAlign(OD_REG02, u1FBBypass, FBT_BYPASS);
    vIO32WriteFldAlign(OD_REG45, !u1PCIDBypass, OD_PCID_EN);
    vIO32WriteFldAlign(OD_REG45, u1PCIDBypass, OD_PCID_BYPASS);
}

UINT8 u1DrvODReadTable(UINT8 TableSel, UINT8 ColorSel, UINT8* pTable)
{
    UINT32 i, u4TblVal, u4TblSize, u4ErrCnt = 0;
    UINT32 u4RegFld;
    UINT8 u1ODBypass =IO32ReadFldAlign(OD_REG02, ODT_BYPASS);
    UINT8 u1FBBypass =IO32ReadFldAlign(OD_REG02, FBT_BYPASS);
    UINT8 u1PCIDBypass =IO32ReadFldAlign(OD_REG45, OD_PCID_BYPASS);

    if (TableSel == FB_TABLE_17)
    {
        return 0;
    }


    if (ColorSel > 2)
    {
        return 1;
    }
    
    if (TableSel == FB_TABLE_17)
    {
        u4RegFld = TABLE_RW_SEL_FB_BGR;
    }
    else if (TableSel == PCID_TABLE_17)
    {
        u4RegFld = TABLE_RW_SEL_PCID_BGR;
    }
    else
    {
        u4RegFld = TABLE_RW_SEL_OD_BGR;
    }


      //OdParam.u1Flg = OD_READ_FLG;    
    //vDrvODWaitIdle();	
    vIO32WriteFldAlign(OD_REG02, 1, ODT_BYPASS);
    vIO32WriteFldAlign(OD_REG02, 1, FBT_BYPASS);
    vIO32WriteFldAlign(OD_REG04, 0, TABLE_ONLY_W_ADR_INC);	
    vIO32WriteFldAlign(OD_REG04, 0, ADDR_YX);
    vIO32WriteFldAlign(OD_REG04, 1<<ColorSel, u4RegFld);	

    switch (TableSel)
    {	
        case OD_TABLE_33:
            u4TblSize = OD_TBL_M_SIZE;
            vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_41);
            vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_17);
            break;
        case OD_TABLE_41_MODE1:
        case OD_TABLE_41_MODE2:
            u4TblSize = OD_TBL_L_SIZE;
            vIO32WriteFldAlign(OD_REG32, 1, OD_IDX_41);
            vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_17);
            break;
        case FB_TABLE_17:
            u4TblSize = FB_TBL_SIZE;
            break;
        case OD_TABLE_17:
            u4TblSize = OD_TBL_S_SIZE;
            vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_41);
            vIO32WriteFldAlign(OD_REG32, 1, OD_IDX_17);
            break;
        case PCID_TABLE_17:
		u4TblSize = PCID_TBL_SIZE;
		break;
        default:
            return 0;
    }		

    for (i=0; i<u4TblSize; i++)
    {
        u4TblVal = IO32ReadFldAlign(OD_REG05, TABLE_RW_DATA);

        if (*(pTable+i) != u4TblVal)
        {
            if (++u4ErrCnt < OdParam.u4LogCnt)
            {
                Printf("\nTable[%02x]   %02x  <=>  %02x ", i, *(pTable+i), u4TblVal);    
            }
            else
            {
                break;
            }	        
        }	    
    }

    vIO32WriteFldAlign(OD_REG04, 0, OD_FLD_ALL);
    vIO32WriteFldAlign(OD_REG02, u1ODBypass, ODT_BYPASS);
    vIO32WriteFldAlign(OD_REG02, u1FBBypass, FBT_BYPASS);
    vIO32WriteFldAlign(OD_REG45, !u1PCIDBypass, OD_PCID_EN);
    vIO32WriteFldAlign(OD_REG45, u1PCIDBypass, OD_PCID_BYPASS);
    return u4ErrCnt;
}

#if OD_STRESS_TEST
void vDrvODChangeTable(UINT32 seed)
{
	UINT32 i;

	for(i=0; i<OD_TBL_L_SIZE; i++)
	{
		OD_Q_Table[i] = (OD_Q_Table[i]+seed+i)&0xFF;
	}
	
	for(i=0; i<FB_TBL_SIZE; i++)
	{
		FB_Q_Table[i] = (FB_Q_Table[i]+seed+i)&0xFF;
	}
}

UINT8 u1DrvODStressLoadODTable(void)
{
    UINT8 ret = 0;

    if ((OdParam.u4Table == 0)||(OdParam.u4Table == 2))  
    {
        vDrvODWriteTable(OD_TABLE_33, OD_ALL, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_33, OD_RED, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_33, OD_GREEN, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_33, OD_BLUE, OD_Q_Table);
    }

    if ((OdParam.u4Table == 0)||(OdParam.u4Table == 1))  
    {
        vDrvODWriteTable(OD_TABLE_17, OD_ALL, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_17, OD_RED, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_17, OD_GREEN, OD_Q_Table);
        ret |= u1DrvODReadTable(OD_TABLE_17, OD_BLUE, OD_Q_Table);
    }	
    return ret;
}

UINT8 u1DrvODStressLoadFBTable(void)
{	
    UINT8 ret = 0;
    return ret;
}

UINT8 u1DrvODStressDMA(void)
{
    UINT8 ret = 0;

    if (OdParam.u4Table == 1)  
    {
        if (_u1DMAState ==OD_DMA_READY)
        {
            _u1DMAState =OD_DMA_WRITE;
            vDrvODSetDMA(OD_TABLE_17, OD_Q_Table, sizeof(OD_Q_Table));
        } 
        
        if (_u1DMAState ==OD_DMA_WRITE_DONE)
        {
            ret |= u1DrvODReadTable(OD_TABLE_17, OD_RED, OD_Q_Table);
            ret |= u1DrvODReadTable(OD_TABLE_17, OD_GREEN, OD_Q_Table);
            ret |= u1DrvODReadTable(OD_TABLE_17, OD_BLUE, OD_Q_Table);
            _u1DMAState = OD_DMA_READY;
            vDrvODChangeTable(OdParam.u4TestNum);
            OdParam.u4TestNum--;
        }
    }
    else
    {
        if (_u1DMAState ==OD_DMA_READY)
        {
            _u1DMAState =OD_DMA_WRITE;
            vDrvODSetDMA(OD_TABLE_33, OD_Q_Table,sizeof(OD_Q_Table));
        } 
        
        if (_u1DMAState ==OD_DMA_WRITE_DONE)
        {
            ret |= u1DrvODReadTable(OD_TABLE_33, OD_RED, OD_Q_Table);
            ret |= u1DrvODReadTable(OD_TABLE_33, OD_GREEN, OD_Q_Table);
            ret |= u1DrvODReadTable(OD_TABLE_33, OD_BLUE, OD_Q_Table);
            _u1DMAState = OD_DMA_READY;
            OdParam.u4TestNum--;
        }
    }

    return ret;
}

#endif

void vDrvODStressTest(UINT32 u4Mode, UINT32 u4Num, UINT32 u4Table, UINT32 u4LogCnt, UINT32 u4Delay)
{
    OdParam.u4TestMode = u4Mode;
    OdParam.u4TestNum = u4Num;
    OdParam.u4LogCnt = u4LogCnt;
    OdParam.u4Table = u4Table;
    OdParam.u4Delay = u4Delay;
    OdParam.u4ErrCnt = 0;
    Printf("\nOD test Start :  %d delay %d\n", OdParam.u4TestNum, OdParam.u4Delay);
}


 
/**************************************************************************  
 * Function Members
 *************************************************************************/
/**
 * @brief vDrvLoadODTable
 * Load OD table
 * @param  void
 * @retval void
 */
void vDrvODLoadODTable(void)
{
    LOG(0, "Load OD table\n");
    //Write OD table
    vDrvODWriteTable(_ODParam.u1ODTblSizeMode, OD_ALL, OD_Table);
    //Check OD table
    u1DrvODReadTable(_ODParam.u1ODTblSizeMode, OD_RED, OD_Table);
    u1DrvODReadTable(_ODParam.u1ODTblSizeMode, OD_GREEN, OD_Table);
    u1DrvODReadTable(_ODParam.u1ODTblSizeMode, OD_BLUE, OD_Table);
}

/**
 * @brief vDrvLoadODTable
 * Load FB table
 * @param  void
 * @retval void
 */
void vDrvODLoadFBTable(void)
{	
    LOG(0, "Load FB table\n");	   
}

void vDrvODLoadPCIDTable(void)
{	
    LOG(0, "Load PCID table\n");	
    
    //Write FB table
    vDrvODWriteTable(PCID_TABLE_17, OD_ALL, PCID_Table);
    //Check FB table
    u1DrvODReadTable(PCID_TABLE_17, OD_RED, PCID_Table);
    u1DrvODReadTable(PCID_TABLE_17, OD_GREEN, PCID_Table);
    u1DrvODReadTable(PCID_TABLE_17, OD_BLUE, PCID_Table);	 
}

void vDrvODSetBufAddress(void)
{
    FBM_POOL_T* prPool;
    UINT32 u4ODAddress;
    UINT32 u4ODDramSize;
    
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_OD, NULL);
    u4ODAddress = prPool->u4Addr+63;
    u4ODDramSize =  prPool->u4Size;
    
    vIO32WriteFldAlign(OD_REG06, u4ODAddress>>4, RG_BASE_ADR);
    vIO32WriteFldAlign(OD_REG28, (u4ODAddress+u4ODDramSize-OD_DMA_SIZE)>>4, RG_TABLE_DMA_ADR_ST);
	#ifndef CC_MT5881
    vIO32WriteFldAlign(OD_REG_LT_04, (u4ODAddress+u4ODDramSize-OD_DMA_SIZE-LOCAL_OD_DMA_SIZE)>>4, LT_BASE_ADDR);
	#endif
	vIO32WriteFldAlign(OD_REG56, (u4ODAddress+u4ODDramSize)>>4, DRAM_UPBOUND);
    vIO32WriteFldAlign(OD_REG56, 1, DRAM_PROT);

}

extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);

void vDrvODSetDMA(UINT8 TableSel, UINT8* pTable, UINT32 u4Size)
{
    UINT8* pBuf = (UINT8 *)VIRTUAL((IO32ReadFldAlign(OD_REG28,RG_TABLE_DMA_ADR_ST))<<4);

    
    //HalFlushInvalidateDCacheMultipleLine((UINT32)(pBuf), OD_DMA_SIZE);
    x_memcpy(pBuf, pTable, u4Size);
    HalFlushInvalidateDCacheMultipleLine((UINT32)(pBuf), OD_DMA_SIZE);
    
    if (TableSel == OD_TABLE_17)
    {
        vIO32WriteFldAlign(OD_REG32, 1, OD_IDX_17);
        vIO32WriteFldAlign(OD_REG29, OD_DMA_SIZE_17, RG_ODT_SIZE);
    }
    else    //OD table 33x33
    {
        vIO32WriteFldAlign(OD_REG32, 0, OD_IDX_17);
        vIO32WriteFldAlign(OD_REG29, OD_DMA_SIZE_33, RG_ODT_SIZE);
    }
    
    vIO32WriteFldAlign(OD_REG29, 0, RG_TABLE_RGB_SEQ);
    vIO32WriteFldAlign(OD_REG29, 1, RG_TABLE_DMA_DONE_CLR);
    vIO32WriteFldAlign(OD_REG29, 0, RG_TABLE_DMA_DONE_CLR);
    vIO32WriteFldAlign(OD_REG29, 1, RG_TABLE_DMA_EN);

}

/**
 * @brief vDrvFrameProtectInit(void)
 * Frame Protection Init
 * @param  void
 * @retval void
 */
void vDrvFrameProtectionInit(void)
{
//    vIO32WriteFldAlign(OD_REG49, SV_ON, NEW_FP);  //NEW_FP moved
    vIO32WriteFldAlign(OD_REG08, OD_PANEL_WIDTH, OD_H_ACTIVE);
    vIO32WriteFldAlign(OD_REG32, OD_PANEL_WIDTH, OD_DE_WIDTH);
    vIO32WriteFldAlign(OD_REG08, OD_PANEL_HEIGHT, OD_V_ACTIVE);    
    
    vIO32WriteFldAlign(OD_REG53, 0xFFB, FRAME_ERR_CON);     //don't care v blank
    vIO32WriteFldAlign(OD_REG09, 0x1E, RG_H_BLANK);     //h_blank  = htotal - h_active
    vIO32WriteFldAlign(OD_REG09, 0xA, RG_H_OFFSET);     //tolerrance       
    
    vIO32WriteFldAlign(OD_REG10, 0xFFF, RG_H_BLANK_MAX);            
    vIO32WriteFldAlign(OD_REG10, 0x3FFFF, RG_V_BLANK_MAX);    //pixel-based counter
    
    vIO32WriteFldAlign(OD_REG11, 0xB000, RG_V_BLANK);    //v_blank  = vtotal - v_active    
    vIO32WriteFldAlign(OD_REG11, 2, RG_FRAME_SET);      

}



void vDrvODParam(void)
{
    UINT32 u4GMV_width;
    UINT32 u4Linesize;
    UINT32 u4EH_Htotal;
    UINT32 u4EH_Vtotal;
    UINT32 u4EH_DE;
    UINT32 u4EH_VDE;
    UINT32 u4HSync_start;
    UINT32 u4HSync_width;
    UINT32 u4VSync_start;
    UINT32 u4VSync_width;
    
    //set gmv detection width
    u4GMV_width = OD_PANEL_WIDTH/6;
    vIO32WriteFldAlign(OD_REG40, (u4GMV_width*1)>>4, GM_R0_CENTER);
    vIO32WriteFldAlign(OD_REG40, (u4GMV_width*2)>>4, GM_R1_CENTER);
    vIO32WriteFldAlign(OD_REG41, (u4GMV_width*3)>>4, GM_R2_CENTER);
    vIO32WriteFldAlign(OD_REG41, (u4GMV_width*4)>>4, GM_R3_CENTER);
    vIO32WriteFldAlign(OD_REG42, (u4GMV_width*5)>>4, GM_R4_CENTER);
    
    vIO32WriteFldAlign(OD_REG43, 12>>2,GM_V_ST);
    vIO32WriteFldAlign(OD_REG43, (OD_PANEL_HEIGHT-12)>>2, GM_V_END);
    vIO32WriteFldAlign(OD_REG42, (100*OD_PANEL_HEIGHT)/1080, GM_LGMIN_DIFF);
    vIO32WriteFldAlign(OD_REG44, (400*OD_PANEL_HEIGHT)/1080, GM_LMIN_THR);
    vIO32WriteFldAlign(OD_REG44, (200*OD_PANEL_HEIGHT)/1080, GM_GMIN_THR);

    
    //set compression ratio
    vIO32WriteFldAlign(OD_REG30, 0x26,MANU_CPR);
     // set line size
    u4Linesize = ((OD_PANEL_WIDTH*38)>>9) +2 ;  // linesize = ( h active/4* manual CR )/128
    vIO32WriteFldAlign(OD_REG47, 2, PRE_BW);    //vIO32WriteFldAlign(OD_REG47, 3, PRE_BW);


   vIO32WriteFldAlign(OD_REG34, 0xF, ODT_SB_TH0);
   vIO32WriteFldAlign(OD_REG34, 0x10,ODT_SB_TH1);
   vIO32WriteFldAlign(OD_REG34, 0x11,ODT_SB_TH2);
   vIO32WriteFldAlign(OD_REG34, 0x12,ODT_SB_TH3);
   vIO32WriteFldAlign(OD_REG47, 0x13, ODT_SB_TH4);
   vIO32WriteFldAlign(OD_REG47, 0x14,ODT_SB_TH5);
   vIO32WriteFldAlign(OD_REG47, 0x15,ODT_SB_TH6);
   vIO32WriteFldAlign(OD_REG47, 0x16,ODT_SB_TH7);

    vIO32WriteFldAlign(OD_REG38, u4Linesize, LINE_SIZE);
    
    //set auto 8bit parameters
    if (OD_PANEL_WIDTH >1900)
    {
        vIO32WriteFldAlign(OD_REG35, 140000, DET8B_DC_NUM);
        vIO32WriteFldAlign(OD_REG36, 40000, DET8B_BTC_NUM);
        vIO32WriteFldAlign(OD_REG37, (1900000>>4) ,DET8B_BIT_MGN);
    }
    else
    {
        vIO32WriteFldAlign(OD_REG35, 70000, DET8B_DC_NUM);
        vIO32WriteFldAlign(OD_REG36, 20000, DET8B_BTC_NUM);
        vIO32WriteFldAlign(OD_REG37, (950000>>4) ,DET8B_BIT_MGN);
    }
    
    //set auto Y5 mode thr
    vIO32WriteFldAlign(OD_REG46, 0x4E00, AUTO_Y5_NUM);
    vIO32WriteFldAlign(OD_REG53, 0x4E00, AUTO_Y5_NUM_1);

    //set OD threshold
    vIO32WriteFldAlign(OD_REG01, 10, MOTION_THR);
    vIO32WriteFldAlign(OD_REG48, 8, ODT_INDIFF_TH);
    vIO32WriteFldAlign(OD_REG02, 1, FBT_BYPASS);

    //set dump param
    vIO32WriteFldAlign(OD_REG51, 0, DUMP_STLINE);
    vIO32WriteFldAlign(OD_REG51, (OD_PANEL_HEIGHT-1), DUMP_ENDLINE);
    
    //set compression param
    vIO32WriteFldAlign(OD_REG77, 0xfc, RC_U_RATIO);
    vIO32WriteFldAlign(OD_REG78, 0xfc, RC_U_RATIO_FIRST2);
    vIO32WriteFldAlign(OD_REG77, 0x68, RC_L_RATIO);
    vIO32WriteFldAlign(OD_REG78, 0x68, RC_L_RATIO_FIRST2);
    vIO32WriteFldAlign(OD_REG76, 0x3, CHG_Q_FREQ);
    vIO32WriteFldAlign(OD_REG76, 0x2, CURR_Q_UV);
    vIO32WriteFldAlign(OD_REG76, 0x2, CURR_Q_BYPASS);
    vIO32WriteFldAlign(OD_REG77, 0x8, IP_SAD_TH); 

     //set pattern gen paramter
     u4EH_Htotal = wDrvGetOutputHTotal();
     u4EH_DE =wDrvGetOutputHActive();
     u4EH_Vtotal = wDrvGetOutputVTotal();
     u4EH_VDE = wDrvGetOutputVActive();
     u4HSync_width = wDrvGetOutputHTotal()-wDrvGetOutputHActive()-wDrvGetHsyncFp()-wDrvGetHsyncBp();
     u4HSync_start = u4HSync_width+ wDrvGetHsyncBp();
     u4VSync_width = wDrvGetOutputVTotal()-wDrvGetOutputVActive()-wDrvGetVsyncFp()-wDrvGetVsyncBp();
     u4VSync_start = u4VSync_width+ wDrvGetVsyncBp();
     
    vIO32WriteFldAlign(OD_REG63, u4EH_Htotal, RG_2CH_PTGEN_H_TOTAL);
    vIO32WriteFldAlign(OD_REG63, u4EH_DE, RG_2CH_PTGEN_H_ACTIVE);
    vIO32WriteFldAlign(OD_REG64, u4EH_Vtotal, RG_2CH_PTGEN_V_TOTAL);
    vIO32WriteFldAlign(OD_REG64, u4EH_VDE, RG_2CH_PTGEN_V_ACTIVE);
    vIO32WriteFldAlign(OD_REG65, u4HSync_width, RG_2CH_PTGEN_H_WIDTH);
    vIO32WriteFldAlign(OD_REG65, u4HSync_start, RG_2CH_PTGEN_H_START);
    vIO32WriteFldAlign(OD_REG66, u4VSync_width, RG_2CH_PTGEN_V_WIDTH);
    vIO32WriteFldAlign(OD_REG66, u4VSync_start, RG_2CH_PTGEN_V_START);
    
}

void vDrvODDramOverflowReset(void)
{
    vIO32WriteFldAlign(OD_REG47, 1, WOV_CLR);
    vIO32WriteFldAlign(OD_REG47, 1, ROV_CLR);
    vIO32WriteFldAlign(OD_REG47, 0, WOV_CLR);
    vIO32WriteFldAlign(OD_REG47, 0, ROV_CLR);
}

UINT32 u4DrvODDramOverflow(void)
{    
    if (IO32ReadFldAlign(OD_REG52, R_UNDERFLOW) == 1)
    {
        return 1;
    }
    else if (IO32ReadFldAlign(OD_REG52, W_UNDERFLOW) == 1)
    {
        return 2;
    }    
    else 
    {
        return 0;
    }    
}

void vDrvODMloop(void)
{
    static UINT8 _DumpCnt=0;
#if OD_STRESS_TEST
    static UINT32 u1FrmCnt = 0;
#endif

#ifdef CC_SUPPORT_OD

    if (IO32ReadFldAlign(OD_STA06,TABLE_DMA_DONE) == 1)
    {
        vIO32WriteFldAlign(OD_REG29, 0, RG_TABLE_DMA_EN);
        vIO32WriteFldAlign(OD_REG29, 1, RG_TABLE_DMA_DONE_CLR);
        vIO32WriteFldAlign(OD_REG29, 0, RG_TABLE_DMA_DONE_CLR);
        _u1DMAState =OD_DMA_WRITE_DONE;
        LOG(0,"OD Table DMA Done\n");
    }
#endif
if (_u1ODDumpEnd)
{
    if (_DumpCnt ==1)
    {
        vIO32Write4B(OD_REG02, u4OdReg02);
        _DumpCnt=0;
        _u1ODDumpEnd=0;
    }
    else
    {
        _DumpCnt++;
    }    
}
#if OD_STRESS_TEST
    
    if (OdParam.u4TestNum)
    {       
        if (++u1FrmCnt > OdParam.u4Delay)
        {        
        	if (OdParam.u4TestNum%100 == 0)
        	{
            	Printf("OD test %d\n", OdParam.u4TestNum);
        	}
			else
			{
				Printf(".");
			}

            if (OdParam.u4TestMode==0)
            {
                vDrvODChangeTable(OdParam.u4TestNum);
                OdParam.u4TestNum--;
                if (u1DrvODStressLoadODTable() || u1DrvODStressLoadFBTable()) 
                {
                    OdParam.u4ErrCnt++;
                }  
            }

            if (OdParam.u4TestMode==1)
            {
                if (u1DrvODStressDMA()) 
                {
                    OdParam.u4ErrCnt++;
                }  
            }
            
            if (OdParam.u4TestNum == 0)
            {
            	Printf("\nOD test Error :  %d\n ", OdParam.u4ErrCnt);
        	}    

            u1FrmCnt = 0;
        }
    }
#endif 
}

void vDrvODISR(UINT16 u2Vector)
{
    //ASSERT(u2Vector == VECTOR_OD);
    SYS_MEASURE_TIME_START(SV_OD_TIMER);     // measure the time used by OD in VSYNC

    if (IO32ReadFldAlign(OD_STA05, STA_IFM) & (FRAME_END<<OD_INT_BASE))
    {
        vApiApplyVideoEvent(PEQTY_APPLY_TIME_OD_VSYNC_ISR);  

        if (IS_OD_WRITE_OD)
        {
            vDrvODWriteTable(_ODParam.u1ODTblSizeMode, OD_ALL, OD_Table);
            OdParam.u1Flg &= ~OD_WRITE_OD_FLG; 
            LOG(0, "OD table write OK\n");
        }
        else if (IS_OD_WRITE_FB) 
        {
            vDrvODWriteTable(FB_TABLE_17, OD_ALL, FB_Table);
            OdParam.u1Flg &= ~OD_WRITE_FB_FLG; 
            LOG(0, "FB table write OK\n");
        }
#if 0
        TIME_StopMeasure(0);
        TIME_PrintElapsedTime(0);
        TIME_StartMeasure(0);
#endif
        vIO32WriteFldAlign(OD_REG54, FRAME_END, OD_STA_INT_CLR);
        vIO32WriteFldAlign(OD_REG54, 0, OD_STA_INT_CLR);        
    }
    SYS_MEASURE_TIME_END(SV_OD_TIMER);
    SYS_STORE_TIME_INFO_TO_REG(SV_OD_TIMER);           
}

void vDrvODMode(UINT8 u1Mode)
{
    if (u1Mode & OD_ONOFF)
    {
        LOG(0, " OD ON\n");
        vDrvODOnOff(SV_ON);
        vDrvODBypass(SV_OFF);

        if (u1Mode & FB_ONOFF)
        {
            LOG(0, " FB ON\n");
            vIO32WriteFldAlign(OD_REG02, 0, FBT_BYPASS);
        }
        else
        {
            LOG(0, "FB OFF\n");
            vIO32WriteFldAlign(OD_REG02, 1, FBT_BYPASS);
        }
    }
    else // OD_OFF_NORMAL
    {
        LOG(0, "OD OFF\n");

        vDrvODOnOff(SV_OFF);
        vDrvODBypass(SV_ON);
        vIO32WriteFldAlign(OD_REG02, 1, FBT_BYPASS);
    }

    if (u1Mode & PCID_ONOFF)
    {
        LOG(0, " PCID ON\n");
        vIO32WriteFldAlign(OD_REG45, 0, OD_PCID_BYPASS);
        vIO32WriteFldAlign(OD_REG45, 1, OD_PCID_EN);
    }
    else
    {
        LOG(0, " PCID OFF\n");
        vIO32WriteFldAlign(OD_REG45, 1, OD_PCID_BYPASS);
        vIO32WriteFldAlign(OD_REG45, 0, OD_PCID_EN);
    }    
}

void vDrvODLoadConfig(void)
{
    if (bApiFlashPqUpdateOD() ==SV_TRUE)
    {
  
        LOG(0, "[OD] : FlashPQ OD OK\n");
        _ODParam.u1ODFlashPQEnable = 1;
    }
    else 
    {
        LOG(0, "[OD] : FlashPQ OD Not Ready\n");
         _ODParam.u1ODFlashPQEnable = 0;
         _ODParam.u1ODTblSizeMode = OD_TABLE_CONFIG;
         
        if (IS_OD_ENABLE == OD_ON)
        {
            _ODParam.u1OD_ON = SV_TRUE;
   
            _ODParam.u1FB_ON = SV_FALSE;
				
            _ODParam.u1PCID_ON = SV_FALSE;
        }
        else
        {
            _ODParam.u1OD_ON = SV_FALSE;
            _ODParam.u1FB_ON = SV_FALSE;
            _ODParam.u1PCID_ON = SV_FALSE;
        }
    }
}

void vDrvODSetTable(void)
{
     if (_ODParam.u1OD_ON)
    {
        vDrvODLoadODTable();
        LOG(0, "Set OD Table\n");
    		
        if (_ODParam.u1FB_ON)	
        {
            vDrvODLoadFBTable();
            LOG(0, "Set FB Table\n");
        }
        else
        {
            LOG(0, "NOT Support FB Table\n");
        }    
    }
    else
    {
        //load a OD default table even though OD is disable
        vDrvODLoadODTable();
        LOG(0, "NOT Support OD Table\n");
    }
    
    if (_ODParam.u1PCID_ON)	
    {
        vDrvODLoadPCIDTable();
        LOG(0, "Set PCID Table\n");
    }
    else
    {
        LOG(0, "NOT Support PCID Table\n");
    }
    
    vDrvODMode(_ODParam.u1OD_ON|(_ODParam.u1FB_ON<<1)|(_ODParam.u1PCID_ON<<2));

}
void vDrvODSetPatternGen(UINT8 u1OnOff, UINT8 u1Type)
{
    vIO32WriteFldAlign(OD_REG62, u1OnOff, RG_2CH_PTGEN_START);
    vIO32WriteFldAlign(OD_REG62, u1Type, RG_2CH_PTGEN_TYPE);
}

void vDrvODCheckEnable(void)
{

}

void vDrvODInitISR(void)
{
    static BOOL u1ODISRInited = SV_FALSE;

    x_os_isr_fct    pfnOldIsr;

    if (!u1ODISRInited)
    {
        if (x_reg_isr(VECTOR_OD, vDrvODISR, &pfnOldIsr) != OSR_OK)
        {
            LOG(0, "Error: Fail to register OD ISR!\n");
        }
        else
        {
             LOG(0, "Success to register OD ISR!\n");
        }

        u1ODISRInited = SV_TRUE;
    }
    
    vIO32WriteFldAlign(OD_REG54, FRAME_END, OD_INT_MASK); //enable frame end int
    LOG(0, "OD ISR registered!\n");
    
}



/**
 * @brief vDrvOdInit
 * Init OD driver
 * @param  void
 * @retval void
 */
void vDrvODInit(void)
{
#ifdef CC_SUPPORT_OD
    UINT32 i;
#endif

    vErrorHandleInit();
    vDrvLoadRegTbl(OD_INIT); 
    vDrvODSetBufAddress();
    vDrvFrameProtectionInit();

    // OD on/off align to vsync
    vIO32WriteFldAlign(OD_REG53, 1, OD_START_SYNC_V);

    vDrvODParam();
   
#ifdef CC_SUPPORT_OD
    LOG(0, "[OD] : FBM is  ready for OD\n");
    vDrvODLoadConfig();
    #if OD_TABLE_VSYNC
        vApiRegisterVideoEvent(PE_EVENT_OD, SV_VP_MAIN, SV_ON);
    #else
        vDrvODSetTable(); 
    #endif    
        vDrvODInitISR();
#else
    LOG(0, "[OD] : FBM is NOT ready for OD\n");
    vDrvODMode(0);
#endif    
    
#if OD_ALPHA_SITE_TEST
        LOG(0, "Force OD to test \n");
        vDrvODMode(SV_ON);
        vDrvODLoadODTable();
        vDrvODLoadFBTable();
#endif
  
    fgIsDitherInited = SV_FALSE;
}

void vDrvODQueryStatus(void)
{

    LOG(0, "OD bypass : %d\n", IO32ReadFldAlign(OD_REG54, BYPASS_ALL_SYNC_V));
    LOG(0, "OD Enable : %d\n", IO32ReadFldAlign(OD_REG12, RG_OD_START));
    LOG(0, "OD Ready : %d\n", IO32ReadFldAlign(OD_STA00, OD_RDY));
    LOG(0, "PCID_EN : %d\n", IO32ReadFldAlign(OD_REG45, OD_PCID_EN));
    LOG(0, "PCID bypass : %d\n", IO32ReadFldAlign(OD_REG45, OD_PCID_BYPASS));
    if (IO32ReadFldAlign(OD_REG02, FBT_BYPASS) == 1)
    {
        LOG(0, "FB table is bypass \n");
    }
    else
    {
        LOG(0, "FB table is enable \n");
    }

    if (IO32ReadFldAlign(OD_REG32, OD_IDX_17) == 0)
    {
        LOG(0, "OD table size is 33x33 \n");
    }
    else if (IO32ReadFldAlign(OD_REG32, OD_IDX_17) == 1)
    {
        LOG(0, "OD table size is 17x17 \n");
    }
    else
    {
        LOG(0, "OD table size setting ERROR \n");
    }

    LOG(0, "OD CONFIG ======================>\n");
    LOG(0, "\n u1ODFlashPQEnable : %d\n", _ODParam.u1ODFlashPQEnable);
    LOG(0, "u1OD_ON : %d\n", _ODParam.u1OD_ON);
    LOG(0, "u1FB_ON : %d\n", _ODParam.u1FB_ON);
    LOG(0, "u1PCID_ON : %d\n", _ODParam.u1PCID_ON);
    LOG(0, "u1ODTblSizeMode : %d\n", _ODParam.u1ODTblSizeMode);

}

void OD_OnOutputVSync(void)
{

}


void vDrvOdDumpInit(void)
{
    UINT32 u4Width, u4Height;    
    UINT32 u4DumpSize;    
    FBM_POOL_T* prPool;
    
    UINT32 u4ReqHeight = IO32ReadFldAlign(SWRW_07, DUMP_REQ_HEIGHT);  
    UINT32 u4StartH = IO32ReadFldAlign(SWRW_06, DUMP_CTRL_START_Y);
    UINT32 u4ShiftBit = 16;    

    LOG(2, "OD Dump init \n");
    
    // OD dump dram address and size    
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_TOOL, NULL);
    u4Width = PANEL_GetPanelWidth();
    u4Height = (IS_PANEL_L12R12) ? (PANEL_GetPanelHeight()>>1) : PANEL_GetPanelHeight();

    // not define request width height, use full size
    u4ReqHeight = (u4ReqHeight == 0) ? u4Height : u4ReqHeight;        

    // dump offset range protection
    u4StartH = (u4StartH > u4Height) ? u4Height : u4StartH;

    if ((u4StartH+u4ReqHeight) > u4Height)
    {
        u4ReqHeight = u4Height-u4StartH;
    }    
    
    u4DumpSize = ((u4Width*u4ReqHeight*4+(1<<u4ShiftBit))>>u4ShiftBit)<<u4ShiftBit;

    LOG(4, "OD Dump Start \n");    
    LOG(4, "Pos W 0  H %d  Length W %d  H %d  Size %d \n", 
        u4StartH, u4Width, u4ReqHeight, u4DumpSize);    
        
    if (u4DumpSize > prPool->u4Size)
    {
        LOG(0, "OD Dump buffer too small\n");
        vIO32WriteFldAlign(SWRW_04, DUMP_ENG_NONE, DUMP_CTRL_ENG_SEL);
        vIO32WriteFldAlign(SWRW_04, 0, DUMP_CTRL_STATE);
        return;
    }
    
    vIO32Write4B(SWRW_01, prPool->u4Addr);
    vIO32Write4B(SWRW_02, prPool->u4Size);
    vIO32WriteFldAlign(SWRW_03, u4Width, DUMP_CTRL_WIDTH);
    vIO32WriteFldAlign(SWRW_03, u4ReqHeight, DUMP_CTRL_HEIGHT);
    vIO32WriteFldAlign(SWRW_03, DUMP_FMT_RGB_444_10BIT, DUMP_CTRL_FORMAT);    
    vIO32WriteFldAlign(SWRW_04, u4DumpSize, DUMP_CTRL_SIZE); 

    //u4OdReg06 = u4IO32Read4B(OD_REG06);
    u4OdReg12 = u4IO32Read4B(OD_REG12);
	u4OdReg39 = u4IO32Read4B(OD_REG39);
    u4OdReg02 = u4IO32Read4B(OD_REG02);

    vIO32WriteFldAlign(OD_REG02, 1, ODT_BYPASS);   
	vIO32WriteFldAlign(OD_REG12, 1, RG_OD_START);
	vIO32WriteFldAlign(OD_REG39, 0, WDRAM_DIS);
	vIO32WriteFldAlign(OD_REG39, 0, RDRAM_DIS);      
    //vIO32WriteFldAlign(OD_REG06, prPool->u4Addr >> 4, RG_BASE_ADR);

	vIO32WriteFldAlign(OD_REG51, u4StartH, DUMP_STLINE);
	vIO32WriteFldAlign(OD_REG51, u4StartH+u4ReqHeight-1, DUMP_ENDLINE);             

	vIO32WriteFldAlign(OD_REG51, 0, DUMP_DRAM_EN);		
	vIO32WriteFldAlign(OD_REG51, 1, DUMP_ONCE);
	vIO32WriteFldAlign(OD_REG51, 3, DUMP_BURST_LEN);

    vIO32WriteFldAlign(OD_REG50, prPool->u4Addr >> u4ShiftBit, DUMP_STADR_A);	
    vIO32WriteFldAlign(OD_REG50, prPool->u4Addr >> u4ShiftBit, DUMP_STADR_B);	

    u1ODState = IO32ReadFldAlign(OD_REG12, RG_OD_START);
    // enable OD
    vIO32WriteFldAlign(OD_REG12,1, RG_OD_START);
    
    //Printf("OD Dump Init\n");
}

void vDrvOdDumpStart(void)
{    
      LOG(2, "OD Dump Start\n");
	vIO32WriteFldAlign(OD_REG51, 1, DUMP_DRAM_EN);  
}


void vDrvOdDumpEnd(void)
{    
    //Printf("OD Dump End \n");
    LOG(2, "OD Dump End \n");
    vIO32WriteFldAlign(OD_REG51, 0, DUMP_DRAM_EN);	 

    vIO32WriteFldAlign(OD_REG12,u1ODState, RG_OD_START);

    //vIO32Write4B(OD_REG06, u4OdReg06);
    vIO32Write4B(OD_REG12, u4OdReg12);
    vIO32Write4B(OD_REG39, u4OdReg39);
    //vIO32Write4B(OD_REG02, u4OdReg02);
    _u1ODDumpEnd=1;
}


#ifdef CC_SUPPORT_STR
//-----------------------------------------------------
//
// OD
// added for OD suspend/resume  function
//-----------------------------------------------------
void vDrvODSuspend(void)
{
    vDrvOdReset( OFF );   //reset_b =0 the function is going reset state
    vDrvOdSetClock( OFF );
}

void vDrvODResume(void)
{
    vDrvOdSetClock( ON );
	vDrvODInit();
    vDrvOdReset( ON );
}
#endif
