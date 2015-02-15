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
 * $RCSfile: d_ATV_cust.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *---------------------------------------------------------------------------*/

#ifndef _D_ATV_CUST_H_
#define _D_ATV_CUST_H_

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "u_common.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/
	
#define DRV_CUSTOM_ATV_MODEL_NON_MOTION_FLOW        (0)
#define DRV_CUSTOM_ATV_MODEL_MOTION_FLOW            (1 << 0)
#define DRV_CUSTOM_ATV_MODEL_NON_RGB_SENSOR         (0 << 1)
#define DRV_CUSTOM_ATV_MODEL_RGB_SENSOR             (1 << 1)
#define DRV_CUSTOM_ATV_MODEL_NON_FLIGHT_SENSOR      (0 << 2)
#define DRV_CUSTOM_ATV_MODEL_FLIGHT_SENSOR          (1 << 2)
#define DRV_CUSTOM_ATV_MODEL_NON_FM_RADIO           (0 << 3)
#define DRV_CUSTOM_ATV_MODEL_FM_RADIO               (1 << 3)
#define DRV_CUSTOM_ATV_MODEL_HDMIX2                 (0 << 4)
#define DRV_CUSTOM_ATV_MODEL_HDMIX4                 (1 << 4)

typedef enum
{
    DRV_CUSTOM_CUST_SPEC_TYPE_BEGIN = 0,

    DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_PATH,
    DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_PICTURE_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_CINE_MOTION_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_GRAPHIC_AREA,
    DRV_CUSTOM_CUST_SPEC_TYPE_DCC,
    DRV_CUSTOM_CUST_SPEC_TYPE_MASK_AREA,
    DRV_CUSTOM_CUST_SPEC_TYPE_INPUT_MODE,
	DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_LED_MODE,
    
    DRV_CUSTOM_CUST_SPEC_TYPE_MMP_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_MMP_OSD_STATE,
    DRV_CUSTOM_CUST_SPEC_TYPE_OSD_TYPE,
    DRV_CUSTOM_CUST_SPEC_TYPE_FM_RADIO_STATE,
	  
    DRV_CUSTOM_CUST_SPEC_GET_TAG_VERSION,

    DRV_CUSTOM_CUST_SPEC_TYPE_END
} DRV_CUSTOM_CUST_SPEC_TYPE_T;

typedef enum {
	D_DEMO_OFF = 0,
	D_BRAVIA_ENGINE = 1,
	D_MOTION_FLOW = 3,
	NUM_OF_D_DEMO_MODE
} D_DEMO_MODE;

typedef enum {
	D_FORMAT_422 = 0,
	D_FORMAT_444,
	NUM_OF_D_PATH
} D_PATH;

typedef enum {
	D_MOTION_OFF = 0,
	D_MOTION_STANDARD,
	D_MOTION_HIGH,
	NUM_OF_D_MOTIONFLOW_MODE
} D_MOTIONFLOW_MODE;

typedef enum {
	D_PICTURE_VIVID = 0,
	D_PICTURE_STANDARD,
	D_PICTURE_CUSTOM,
	D_PICTURE_CINEMA1,
	D_PICTURE_CINEMA2,
	D_PICTURE_GAME,
	D_PICTURE_TEXT,
	D_PICTURE_SPORTS,
	D_PICTURE_PHOTO_VIVID,
	D_PICTURE_PHOTO_STANDARD,
	D_PICTURE_PHOTO_CUSTOM,
	D_PICTURE_PHOTO_ORIGINAL,
	NUM_OF_D_PICTURE_MODE
} D_PICTURE_MODE;

typedef enum {
	D_CINE_MOTION_OFF = 0,
	D_CINE_MOTION_AUTO2,
	D_CINE_MOTION_AUTO1,
	NUM_OF_D_CINE_MOTION
} D_CINE_MOTION_MODE;

typedef struct _SMicroMaskArea
{
    UINT8 u1Index;
    UINT16 u2Width;
    UINT16 u2Height;
    UINT16 u2X;
    UINT16 u2Y;
} D_SMicroMaskArea;


typedef enum {
	D_INPUT_RF = 0,
	D_INPUT_OTHER,
	NUM_OF_D_INPUT_MODE
} D_INPUT_MODE;

typedef enum
{
    D_LED_GENERIC_ON,
    D_LED_GENERIC_OFF,
} D_LED_GENERIC_STATE;

typedef enum
{
    D_LED_POWER_ON,
    D_LED_POWER_OFF
} D_LED_POWER_STATE;

typedef enum
{
    D_LED_STANDBY_ON,
    D_LED_STANDBY_OFF
} D_LED_STANDBY_STATE;

typedef enum
{
    D_LED_TIMER_RED,
    D_LED_TIMER_GREEN,
    D_LED_TIMER_ORANGE,
    D_LED_TIMER_OFF,
} D_LED_TIMER_STATE;

typedef enum
{
    D_LED_POWER = 0,
    D_LED_STANDBY,
    D_LED_TIMER,
    D_LED_TIMER_2,
    D_LED_TOTAL
} D_LED_TYPE;

typedef struct _ATVLedDisplay
{
    D_LED_TYPE eLedType;
    UINT32 eLedState;
} D_LED_DISPLAY;


typedef struct DRV_CUSTOM_TAG_VERSION_TYPE_T
{
    UINT8 ui1_tag_type;                      /* tag*/
    CHAR szVerStr[20];						//version string
}   DRV_CUSTOM_TAG_VERSION_TYPE_T;

typedef enum
{
	DRV_CUSTOM_TAG_TYPE_LOADER= 0,  //// loader by luis
	DRV_CUSTOM_TAG_TYPE_FIRMWARE,  // firmware by luis
	DRV_CUSTOM_TAG_TYPE_PQ ,	// PQ data by norman
	DRV_CUSTOM_TAG_TYPE_PANEL,	// panel data by cosh
	DRV_CUSTOM_TAG_TYPE_AQ ,	// AQ data by Andrew
	DRV_CUSTOM_TAG_TYPE_UPEEP , // uP nvm by Cosh
	DRV_CUSTOM_TAG_TYPE_UPIMG ,	// uP image by Cosh	
	DRV_CUSTOM_TAG_TYPE_NVRAM , // eeprom by Jifeng
	DRV_CUSTOM_TAG_TYPE_EDID,  	// EDID by CI
	DRV_CUSTOM_TAG_TYPE_MAX
}DRV_CUSTOM_TAG_TYPE_T;

typedef enum
{
    D_MMP_NONE = 0,
    D_MMP_PHOTO,
    D_MMP_MUSIC,
    D_MMP_MOVIE,
    NUM_OF_MMP_MODE
} D_MMP_MODE;

typedef enum
{
    D_MMP_OSD_NONE = 0,
    D_MMP_OSD_THUMBNAIL,
    D_MMP_OSD_DEVICE,
    D_MMP_OSD_PLAYBACK,
    NUM_OF_MMP_OSD_STATE
} D_MMP_OSD_STATE;

typedef enum
{
    D_MMP_OSD_NOT_FULL = 0,
    D_MMP_OSD_FULL,
    NUM_OF_OSD_TYPE
} D_MMP_OSD_TYPE;

typedef enum
{
    D_FM_RADIO_OFF = 0,
    D_FM_RADIO_ON,
    NUM_OF_FM_RADIO_STATE
} D_FM_RADIO_STATE;


/*-----------------------------------------------------------------------------
                    function declarations
-----------------------------------------------------------------------------*/

/* Customer specific APIs */
extern INT32 d_ATV_cust_spec_set (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_set_info,
    SIZE_T                         z_size,
    BOOL                           b_store
);

extern INT32 d_ATV_cust_spec_get (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_get_info,
    SIZE_T*                        pz_size
);

extern void ATVLEDCtrlFunc(
    D_LED_TYPE eLedType,
    UINT32 u4LedState
);

#endif /* _D_ATV_CUST_H_ */
