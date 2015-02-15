/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
/*! \file drv_imgresz_errcode.h
*
* \par Project
*    MT8520
*
* \par Description
*    The error code definitions of gcpu module.
*
* \par Author_Name
*    CK Hu
*
* \par Last_Changed
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

#include "x_typedef.h"

#ifndef _DRV_IMGRESZ_ERRCODE_H_
#define _DRV_IMGRESZ_ERRCODE_H_

#define DRL_MODULE_IMGRESZ                      0x73L   ///< Image resizer


#define IMGRESZ_OKCODE(modid, okid)           \
  ((INT32)                             \
    ((INT32)(0x00000000) |             \
     (INT32)((modid & 0x7f) << 24) |   \
     (INT32)(okid & 0xffff))           \
  )


#define IMGRESZ_ERRCODE(modid, errid)         \
  ((INT32)                             \
    ((INT32)(0x80000000) |             \
     (INT32)((modid & 0x7f) << 24) |   \
     (INT32)(errid & 0xffff))          \
  )


#define IMGRESZ_DRV_UOKCODE(group, okcode)  \
    (                                       \
        IMGRESZ_OKCODE(DRL_MODULE_IMGRESZ,         \
            ((UINT32)((group & 0xff) << 8) | \
            (UINT32)(okcode & 0xff)))        \
    )


#define IMGRESZ_DRV_UERRCODE(group, errcode)\
    (                                       \
        IMGRESZ_ERRCODE(DRL_MODULE_IMGRESZ,        \
            ((UINT32)((group & 0xff) << 8) | \
            (UINT32)(errcode & 0xff)))       \
    )


#define IMGRESZ_DRV_GROUP_GENERAL         0


#define S_IMGRESZ_DRV_OK                      IMGRESZ_DRV_UOKCODE(IMGRESZ_DRV_GROUP_GENERAL,0x00)
#define S_IMGRESZ_DRV_RESIZE_FINISH           IMGRESZ_DRV_UOKCODE(IMGRESZ_DRV_GROUP_GENERAL,0x01)
#define S_IMGRESZ_DRV_RESIZE_STOP             IMGRESZ_DRV_UOKCODE(IMGRESZ_DRV_GROUP_GENERAL,0x02)
#define S_IMGRESZ_DRV_RESIZE_PIC_MODE_READY   IMGRESZ_DRV_UOKCODE(IMGRESZ_DRV_GROUP_GENERAL,0x03)

#define E_IMGRESZ_DRV_FAIL                    IMGRESZ_DRV_UERRCODE(IMGRESZ_DRV_GROUP_GENERAL,0x00)
#define E_IMGRESZ_DRV_LOCK_FAIL               IMGRESZ_DRV_UERRCODE(IMGRESZ_DRV_GROUP_GENERAL,0x01)
#define E_IMGRESZ_DRV_RESIZE_TIMEOUT          IMGRESZ_DRV_UERRCODE(IMGRESZ_DRV_GROUP_GENERAL,0x02)


#endif // #ifndef _DRV_IMGRESZ_ERRCODE_H_


