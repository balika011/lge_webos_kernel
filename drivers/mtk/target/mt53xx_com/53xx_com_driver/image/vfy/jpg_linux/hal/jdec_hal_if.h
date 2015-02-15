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
/*
* \par Project
*    MT8520
*
* \par Description
*    Jpeg Decoder HAL interface definition.
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

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#ifndef __JDEC_HAL_IF_H
#define __JDEC_HAL_IF_H

//#include "jdec_com_common.h"
#ifdef MT8520
#include "x_common.h"
#endif
#include "jdec_com_jfif.h"
#include "drv_config.h"
//#include "chip_ver.h"

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
#include "x_iommu.h"
#endif


#define JDEC_HAL_MT8550 1 
#ifdef __cplusplus
extern "C" {
#endif

/*! \name Video Decoder HAL MPEG Interface
* @{
*/
#define JDEC_HW_LOG_ENABLE 0

#if  CONFIG_ENABLE_IOMMU
#define JDEC_IO_MMU_TEST  0
#else
#define JDEC_IO_MMU_TEST    0
#endif

#define JDEC_QTABLE_RISC_MODE 1
#define  JDEC_HAL_SUPPORT_MB  1
#define  JDEC_MM_MODE_ENABLE 1

#if JDEC_IO_MMU_TEST
 extern void *IOMMU_get_fragment_vmem(UINT32 u4Size);
  #define  GGT_get_vmem    x_alloc_vmem
#endif


#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
typedef enum {
    JDEC_HAL_DEC_MODE_NONE,
    JDEC_HAL_DEC_MODE_BASELINE_PIC,                     ///< Jpeg baseline picture, decode whole picture
    JDEC_HAL_DEC_MODE_BASELINE_MCU,                     ///< Jpeg baseline picture, decode MCU row
    JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW,                 ///< Jpeg baseline picture, decode MCU
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
    JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT,
    #endif
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT,    ///< Jpeg progressive picture, decode MCU milti-collect
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE,           ///< Jpeg progressive picture, decode MCU enhance decoding
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT, ///Jpeg progressive MCU-Row Mode
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE,
    JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT,
    JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE,
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS ///Jpeg progressive MCU-Row Mode   
} JDEC_HAL_DEC_MODE_T;
#else
typedef enum {
    JDEC_HAL_DEC_MODE_NONE,
    JDEC_HAL_DEC_MODE_BASELINE_PIC,                     ///< Jpeg baseline picture, decode whole picture
    JDEC_HAL_DEC_MODE_BASELINE_MCU,                     ///< Jpeg baseline picture, decode MCU row
    JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW,                 ///< Jpeg baseline picture, decode MCU
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT,    ///< Jpeg progressive picture, decode MCU milti-collect
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE           ///< Jpeg progressive picture, decode MCU enhance decoding
} JDEC_HAL_DEC_MODE_T;
#endif


typedef struct {
    UINT32 u4InFifoSa;                       ///< Jpeg input Fifo memory start address
    UINT32 u4InFifoSz;                       ///< Jpeg input Fifo memory size
    UINT32 u4RdPoint;                     ///< Jpeg input Fifo Read Point
    UINT32 u4WrPoint;                     ///< Jpeg input Fifo Write Point
} JDEC_HAL_INPUT_INFO_T;


typedef struct {
    UINT32 u4OutYBufSa;                     ///< Jpeg output color buffer Y start address
    UINT32 u4OutCbBufSa;                    ///< Jpeg output color buffer Cb start address
    UINT32 u4OutCrBufSa;                    ///< Jpeg output color buffer Cr start address
    UINT32 u4OutYBufPitch;                  ///< Jpeg output Y color buffer pitch
    UINT32 u4OutCbBufPitch;                 ///< Jpeg output Cb color buffer pitch
    UINT32 u4OutCrBufPitch;                 ///< Jpeg output Cr color buffer pitch
    UINT32 u4OutBank0YBufSa;                ///< Jpeg output Bank0 color buffer Y start address
    UINT32 u4OutBank0CbBufSa;               ///< Jpeg output Bank0 color buffer Cb start address
    UINT32 u4OutBank0CrBufSa;               ///< Jpeg output Bank0 color buffer Cr start address
    UINT32 u4OutBank1YBufSa;                ///< Jpeg output Bank1 color buffer Y start address
    UINT32 u4OutBank1CbBufSa;               ///< Jpeg output Bank1 color buffer Cb start address
    UINT32 u4OutBank1CrBufSa;               ///< Jpeg output Bank1 color buffer Cr start address
 #if JDEC_MM_MODE_ENABLE
    UINT32 u4ToalMCURow;
    UINT32 u4DecodedMCURow;
    UINT32 ui4McuRowNumber ;// the decoding mcu row cnt for MM mode each time.
#endif	
} JDEC_HAL_OUTPUT_INFO_T;


typedef struct {
    JDEC_JFIF_SOF_T *prSofInfo;             ///< [IN] Jpeg SOF information
    JDEC_JFIF_DQT_T *prQTblInfo;            ///< [IN] Jpeg dqt information
    JDEC_JFIF_DHT_T *prHuffTblInfo;         ///< [IN] Jpeg dht information
    UINT32 u4RestartInterval;               ///< [IN] The Interval between Restart Marker.
} JDEC_HAL_PIC_INFO_T;


typedef struct {
    UINT32 u4BitsLeftRead;
    UINT32 u4BufPtrRead;
    UINT32 u4ShiftMag;
    UINT32 u4MkrRm2;
    UINT32 u4JpegBufRead;
    UINT32 u4RestartMkr;
    UINT32 u4LastDcValue0;
    UINT32 u4LastDcValue1;
    UINT32 u4LastDcValue2;
    UINT32 u4CurrMCUNum;
    UINT32 u4ReadAddrBytes;
    UINT32 u4ReadAddrBits;
} JDEC_HAL_HW_STATUS_T;


/// Initialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Init(
    UINT32 u4JDecID,                                     ///< [IN] Jpeg decoder hardware ID
    UINT32 u4HwId
);


/// Uninitialize Jpeg decoder HAL
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID,                                     ///< [IN] Jpeg decoder hardware ID
    BOOL  fgWait
);
#else
INT32 i4JDEC_HAL_Uninit(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
);
#endif

/// Set Jpeg input picture information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Pic_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_PIC_INFO_T *prPicInfo                      ///< [IN] Jpeg input picture information
);


/// Set Huffman Table information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_HuffmanTable_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_DHT_T *prHuffTblInfo                      ///< [IN] Huffman Table information
);


/// Set Jpeg decoding mode
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Decoding_Mode(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_DEC_MODE_T eDecodingMode                   ///< [IN] Jpeg decoding mode
);


/// Get Jpeg decoding mode supporting status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Decoding_Mode_Support_Status(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_DEC_MODE_T eDecodingMode                   ///< [IN] Jpeg decoding mode
);


/// Set Jpeg scan information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Scan_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_SOS_T *prScanInfo                    ///< [IN] Input Jpeg scan information
);


/// Set Jpeg input information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Input_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_INPUT_INFO_T *prInputInfo                  ///< [IN] Input Jpeg input information
);


/// Set Jpeg output information
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
INT32 i4JDEC_HAL_Set_Output_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo                ///< [IN] Input Jpeg output information
);
#else
INT32 i4JDEC_HAL_Set_Output_Info(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,                ///< [IN] Input Jpeg output information
    JDEC_HAL_OUTPUT_INFO_T *prOutputInfo1
);

#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
INT32 i4JDEC_HAL_Set_Burst_Mode(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_JFIF_SOF_T  *prSofInfo,                ///< [IN] Input Jpeg sof information
    BOOL fgBurstEn                                     ///< [IN] Input burst enable
);
#endif

/// Trigger Jpeg HAL decoding
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Decode(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
);


/// Get Jpeg HAL decode status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Decode_Status(
    UINT32 u4JDecID                                     ///< [IN] Jpeg decoder hardware ID
);

#if JDEC_MM_MODE_ENABLE
INT32 i4JDEC_HAL_Update_Output_Status(UINT32 u4JDecID);
JDEC_HAL_OUTPUT_INFO_T *prJDEC_HAL_Get_Output_Status(UINT32 u4JDecID);
#endif
/// Set Jpeg VLD read pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Read_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4Addr                                       ///< [IN] Read pointer address (bytes)
);


/// Get Jpeg VLD read pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_Read_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 *pu4AddrBytes,                               ///< [IN] Read pointer address (bytes)
    UINT32 *pu4AddrBits                                 ///< [IN] Read pointer address (bytes)
);


/// Set Jpeg VLD write pointer
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_Write_Pointer(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    UINT32 u4Addr                                       ///< [IN] Write pointer address (bytes)
);


/// Get Jpeg hardware status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Get_HW_Status(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    JDEC_HAL_HW_STATUS_T *prHwStatus                    ///< [OUT] Hardware status
);


/// Set Jpeg hardware status
/// \return If return value < 0, it's failed. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Set_HW_Status(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    const JDEC_HAL_HW_STATUS_T *prHwStatus              ///< [IN] Hardware status
);

/// Get Jpeg error status
/// \return If return 0, means no error ,else return error type
INT32 i4JDEC_HAL_GET_Error_Type(
    UINT32 u4JDecID                                   ///< [IN]   Jpeg decoder hardware ID
);

void i4JDEC_HAL_PrintChkSum(UINT32 u4JDecID);
void vJDEC_HAL_StopPicModeDec(
    UINT32 u4JDecID);

INT32 i4JDEC_HAL_GET_STOP_STATUS(
    UINT32 u4JDecID
    );

/// Get Jdec decoding finish or not
/// \return If return value < 0, it's means not finish. Please reference hal_jdec_errcode.h.
INT32 i4JDEC_HAL_Decode_Finish(
    UINT32 u4JDecID,                                    ///< [IN] Jpeg decoder hardware ID
    BOOL fgLastScan,                                    ///< [IN] The current scan is last scan or not
    UINT32* pu4CurrentLastScanNum                       ///< [IN][OUT] Current decode last scan number
);

/// Set IRQ
/// \return If return TRUE, it means the irq has been set
BOOL fgJDEC_HAL_SetIrq(
    void);

/// disable IRQ
/// \return If return TRUE, it means the irq has been disable
BOOL fgJDEC_HAL_DisIrq(
    void);

UINT32 u4JDEC_HAL_Get_VLD_WPtr(UINT32 u4JDecID);
UINT32 u4JDEC_HAL_Get_VLD_RPtr(UINT32 u4JDecID);
extern BOOL fgDisableErrConceal ;
extern void *x_hw_mem_alloc( UINT32 u4Size, UINT32 u4_alignment,BOOL fgSupportMMU);
extern void x_hw_mem_free(void* ptr, BOOL fgSupportMMU);

#define DEBUG_MMMMM_ON  0
#if DEBUG_MMMMM_ON
extern  UINT32 *_pi4JpegWindow[14];
extern  UINT32  _u4WindowNum[14];
extern  UINT32  _ui4ScanNumber;
#endif
//
/*! @} */
#ifdef __cplusplus
}
#endif

#endif //__JDEC_HAL_ERRCODE_H

