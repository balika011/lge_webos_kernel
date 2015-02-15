/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2004, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $RCSfile: be_boot_if.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *---------------------------------------------------------------------------*/
#ifndef _BE_BOOT_IF_H_
#define _BE_BOOT_IF_H_

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "loader_if.h"
#include "x_ldr_env.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/

typedef enum _BEBOOTSTATUS
{
    eState_BEBootInit= 0,
    eState_BEBootWaitOutput,
	eState_BEBootRisingVx1Output,
	eState_BEBootFallingVx1Output,
	eState_BEBootErrorHandle,
	eState_BEBootWaitUartRdy,
	eState_BEBootFail, 
    eState_BEBootFinish   
} BE_BOOT_STATUS_T;

typedef enum _BEBOOTFLASHPARTITIONIDX
{
    eFlashPartID_DDRI= 0,
	eFlashPartID_MLFW,
	eFlashPartID_LDAT,	
	eFlashPartID_MAFW,	
	eFlashPartID_DFW1,
	eFlashPartID_DFW2,
	eFlashPartID_PDAT,
	eFlashPartID_BDAT,
	eFlashPartID_NDAT,
	eFlashPartID_FDAT,
	eFlashPartID_UDAT,
	eFlashPartID_BDIX,
	eFlashPartID_RESV_2,
	eFlashPartID_RESV_3
} BE_BOOT_FLASH_IDX_T;

typedef struct _BeFlashInfo
{
    UINT32    u4PartId;
	UINT32    u4Size;
} BE_BOOT_FLASH_INFO_T;

typedef enum _BEFRAME_HEADER_MODE
{
    eIFrame_Header_mode= 0,
    eFFrame_Header_mode,		
	eDFrame_Header_mode
} BE_FRAME_HEADER_MODE_T;

typedef struct _BeIFrameHeader
{
    UINT32    u4Key;
	UINT32    u4HeaderSize;
	UINT32    u4FrameSize;
	UINT32    u4FrameNo;
	UINT32    u4TotalFrameNo;
	UINT32    u4TotalPacketNo;
	UINT16    u2HeaderCRC;
} BE_I_FRAME_HEADER_T;

typedef struct _BeIFrameFooter
{
    UINT32    u4FootKey;
} BE_I_FRAME_FOOTER_T;

typedef struct _BeIFrame
{
    BE_I_FRAME_HEADER_T rIFrameHeaderInfo;
	BE_I_FRAME_FOOTER_T rIFrameFooterInfo;
} BE_I_FRAME_T;

typedef struct _BeFFrameHeader
{
    UINT32    u4Key;
	UINT32    u4HeaderSize;
	UINT32    u4FrameSize;
	UINT32    u4FrameNo;
	UINT32    u4TotalFrameNo;
	UINT32    u4TotalPacketNo;
	UINT16    u2HeaderCRC;
	UINT8     u1AuxRegister; //bit01:Boot mode / bit23:panel boot request / bit45:Logo on/off / bit67:normal boot/phase2	
} BE_F_FRAME_HEADER_T;

typedef struct _BeFFrameFooter
{
    UINT32    u4FootKey;
} BE_F_FRAME_FOOTER_T;

typedef struct _BeFFrame
{
    BE_F_FRAME_HEADER_T rFFrameHeaderInfo;
	BE_F_FRAME_FOOTER_T rFFrameFooterInfo;
} BE_F_FRAME_T;

typedef struct _BeDFrameHeader
{
    UINT32    u4Key;
	UINT32    u4HeaderSize;
	UINT32    u4FrameSize;
	UINT32    u4FrameNo;
	UINT32    u4TotalFrameNo;
	UINT32    u4TotalPacketNo;
	UINT16    u2HeaderCRC;
	UINT8     u1AuxRegister; //bit01:Boot mode / bit23:panel boot request / bit45:Logo on/off / bit67:normal boot/phase2	
} BE_D_FRAME_HEADER_T;

typedef struct _BeDFrameFooter
{
    UINT32    u4FootKey;
} BE_D_FRAME_FOOTER_T;

typedef struct _BeDFrame
{
    BE_D_FRAME_HEADER_T rDFrameHeaderInfo;
	BE_D_FRAME_FOOTER_T rDFrameFooterInfo;
} BE_D_FRAME_T;

typedef struct _BeFrameInfo
{
	BE_FRAME_HEADER_MODE_T rFrameMode;
    UINT8 FrameHeader[128];
	UINT8 FrameFooter[16];
	UINT8 rFrameContainer[7];
	UINT8  u1State;
} BE_FRAME_INFO_T;

typedef struct _BeFbmInfo
{
    UINT32 ui4DisplayFBMAddr;
	UINT32 ui4DisplayFBMSize;
	UINT8  ui1DisplayMode;
	UINT32 ui4DisplayAreaWidth;
	UINT32 ui4DisplayAreaHeight;
	UINT32 ui4MaxBootTime;
} BE_FBM_INFO_T;

typedef struct _BeOutputFrmInfo
{
	UINT8  u1State;
	UINT8  u1FrameNum;
} BE_OUTPUT_FRM_INFO_T;

typedef struct _BeOsdOrder
{
    UINT32 ui4Alpha;
	UINT32 ui4Red;
	UINT32 ui4Green;
	UINT32 ui4Blue;
} BE_OSD_ORDER_T;

#ifdef CC_MTK_LOADER
typedef struct _BeDelPartInfo
{
    UINT8 ui1PartitionId;
	UINT32 ui4PacketFlag;
} BE_DEL_PART_INFO_T;
#endif

#ifndef CC_MTK_LOADER
typedef struct _BeFbmRegionInfo
{
    UINT32 u4RegionIdx;
	UINT32 u4RegionListId;
	UINT32 u4Pitch;
} BE_FBM_REGION_INFO_T;
#endif

#define BE_IC_SUPPORT_ADDR               (0xA020)
#define BE_IC_OSD_PIXEL_SHIFT_POLARITY   (0xA030)
#define BE_IC_OSD_PIXEL_SHIFT            (0xA031)
#define BE_IC_PANEL_ID_ADDR              (0xA050)

/*-----------------------------------------------------------------------------
                    function declarations
-----------------------------------------------------------------------------*/
/* Customer specific APIs */
//-----------------------------------------------------------------------------
/** LDR_BE_Init(): Do necessary setup settings for BEIC data transferring
 *  This function is going to setup boot settings, such as GPIO_Init() and register call back
 *
 *  @param  prLdrData       The loader global data structure pointer.
 *  @retval 0               Success.
 *  @retval otherwise       Failed.
 */
//-----------------------------------------------------------------------------
extern UINT32 LDR_BE_Init(LDR_DATA_T *prLdrData);
extern UINT32 LDR_BEBootSeq(LDR_DATA_T *prLdrData);
extern UINT32 LDR_BEFrameDisplay(UINT32 u4PartitionIdx, UINT32 u4Offset);
extern BOOL LDR_BE_Support(void);
extern VOID LDR_BE_Reset(void);
extern VOID LDR_BE_Erase_Del_Partition(void);
#ifndef CC_MTK_LOADER
extern UINT32 BEBoot_pm_suspend(void);
extern UINT32 BEBoot_pm_resume(void);
#endif
#endif /* _BE_BOOT_IF_H_ */
