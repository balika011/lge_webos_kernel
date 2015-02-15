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
#ifndef _VDEC_HAL_IF_COMMON_H_
#define _VDEC_HAL_IF_COMMON_H_

#include "vdec_hal_errcode.h"
#include "vdec_hw_common.h"

#define START_CODE 0x00000001
#define ParseNonVCL_WithRisc 1

#define DEBUG_INFO 0
#define CACHE_BUFF_SIZE 32
#define MIN_NALU_SIZE 1

typedef struct _RBSP_BitStream_{
    UCHAR *pStartBuff;
    UCHAR *pEndBuff;
    UINT32 u4Cache;
    UINT32 u4Cache1;
    UINT32 u4BitLeft;
    UINT32 u4BitLeft1;
    UINT32 u4ByteLeft;
    UINT32 u4BitAccumulated;
    UINT32 u4BitTotalBuffBitCount;
    BOOL   bError;
//    FILE *pfLogFile;
}RBSP_BitStream, *pRBSP_BitStream;

INT32 initRBSPBitStream(pRBSP_BitStream pRBSP_BitStreamInst, UCHAR *pBuffer, UINT32 u4BuffSize);
INT32 destroyRBSPBitStream(pRBSP_BitStream *ppRBSP_BitStreamInst);
INT32 RBSPGetBits(pRBSP_BitStream pRBSP_BitStreamInst, UINT32 u4BitCount);
UINT32 RBSPGetUE(pRBSP_BitStream pRBSP_BitStreamInst);
INT32 RBSPGetSE(pRBSP_BitStream pRBSP_BitStreamInst);
INT32 RBSPGetBitOffset(pRBSP_BitStream pRBSP_BitStreamInst);
BOOL RBSPMoreData(pRBSP_BitStream pRBSP_BitStreamInst);

/*! \name Video Decoder HAL Common Private Function
* @{
*/
#if (VDEC_SUPPORT_IOMMU)
    extern UINT32 u4HalGetTTB1(void);
#endif

typedef void  (* vDecEndCallback)(void *pvUserPrivData);


/// This function turns on video decoder HAL
/// \return If return value < 0, it's failed. Please reference drv_vdec_errcode.h.
INT32 i4VDEC_HAL_Common_Init(
    UINT32 u4ChipID            ///< [IN] Chip ID
);

#if (!CONFIG_DRV_FPGA_BOARD)
void vVDEC_HAL_CLK_Set(UINT32 u4CodeType);
#endif

void vVDec_HAL_CRC_Enable(UINT32 u4VDecID, BOOL fgCRCType);

/// Get video decoder hardware resource
/// \return None
void u4VDEC_HAL_Common_GetHWResourceNum(
    UINT32 *pu4BSNum,                                ///< [OUT] Pointer to barrel shifter number of every VLD
    UINT32 *pu4VLDNum                               ///< [OUT] Pointer to VLD number
);


/// Turn on or turn off VLD power
/// \return None
void vDEC_HAL_COMMON_SetVLDPower(
    UINT32 u4VDecID,                                            ///< [IN] Video decoder hardware ID
    BOOL fgOn                                                      ///< [IN] Turn on or off VLD
);

//
/*! @} */

INT32 i4VDEC_HAL_Dram_Busy (
    UINT32 u4ChipID,            ///< [IN] Chip ID
    UINT32 u4StartAddr, UINT32 u4Offset
);

INT32 i4VDEC_HAL_Dram_Busy_Off(
    UINT32 u4ChipID,            ///< [IN] Chip ID
    UINT32 u4StartAddr, UINT32 u4Offset
);

#if BANDWIDTH_MEASURE
void vVDEC_HAL_BANDWID_MEAS_DumpReg(UINT32 u4VDecID);
#endif
#if PARA8_BEHAVIOR_TEST
void vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(UINT32 u4VDecID);
#endif
#if VDEC_SUPPORT_IOMMU
extern void vDEC_HAL_COMMON_PrintfMMUStatus(void);
extern void vDEC_HAL_COMMON_IOMMUInit(void);
extern void vDEC_HAL_COMMON_MMUFlush(void);
#endif
#endif //#ifndef _HAL_VDEC_COMMON_IF_H_

