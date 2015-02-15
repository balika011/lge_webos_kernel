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


#ifndef _HW_JDEC_H_
#define _HW_JDEC_H_

#include "jdec_hal_if.h"
#include "x_typedef.h"
#include "jdec_com_common.h"
#include "x_common.h"
//#include "chip_ver.h"
#include "jdec_drv.h"

#define HW_JDEC_VLD_NUM 1
#define HW_JDEC_MC_NUM 1

//JPEG decoding register address define
#define RO_VLD_BARL      0x00
#define RW_VLD_PROC             0x8C
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    #define VLD_SSCBIT              ((unsigned)0x1 << 16)
    #define VLD_SSCBYTE             ((unsigned)0x1 << 17)
    #define VLD_ABRT                ((unsigned)0x1 << 18)
    #define VLD_PSUP                ((unsigned)0x1 << 19)
    #define VLD_INIFET              ((unsigned)0x1 << 20)
    #define VLD_PDHW                ((unsigned)0x1 << 22)
    #define VLD_INIBR               ((unsigned)0x1 << 23)
    #define VLD_PERRCON             ((unsigned)0x1 << 24)
    #define VLD_RTERR               ((unsigned)0x1 << 25)
    #define VLD_EOFR                ((unsigned)0x1 << 26)
    #define VLD_IGBCL               ((unsigned)0x1 << 27)
    #define VLD_DECTOP              ((unsigned)0x1 << 29)
    #define VLD_DECBTM              ((unsigned)0x2 << 29)
    #define VLD_FPRBS               ((unsigned)0x3 << 29)
    #define VLD_B21EN               ((unsigned)0x1 << 31)
#endif    
    #define VLD_INIFET              ((unsigned)0x1 << 20)
    #define VLD_INIBR               ((unsigned)0x1 << 23)
    #define VLD_EOFR                ((unsigned)0x1 << 26)
    
#define RW_VLD_TABLIM           0xA0
#define RW_VLD_TABLNIM          0xA4
#define RW_VLD_RPTR             0xB0
#define RW_VLD_VSTART           0xB4
#define RW_VLD_VEND             0xB8
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
#define RW_VLD_RDY_SWTICH   0xC0
   #define READY_TO_RISC         (0x1 << 17)
   #define READY_TO_RISC_1       (0x1 << 18)
   #define READY_TO_RISC_2       (0x1 << 20)
#endif
//#define RW_VLD_SSCS             0xC0
#define RW_VLD_PWRSAVE          0xC4
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
#define RW_VLD_PDNFINE          0xC8
     #define RW_PDN_BASE_CLK_OFF         ((unsigned)0x1 << 0)
     #define RW_PDN_PROG_CLK_OFF         ((unsigned)0x1 << 1)
#endif
#define RW_VLD_AMSR             0xD0
    #define VLD_CPFE_EN          ((unsigned)0x1 << 17)

	   #define VLD_INIT_VALID_FLG       ((unsigned)0x1 << 3)
#define RO_VLD_FETCHOK          0xE8
    #define VLD_FETCH_OK            ((unsigned)0x1 << 0)
    #define VLD_DRAM_REQ_FIN        ((unsigned)0x1 << 2)
    #define VLD_INIT_VALID_FLG       ((unsigned)0x1 << 3)
#define RW_VLD_BS_RW_POINT      0xEC
#define RO_VIDEO_BUFFER_CTRL    0xF4
#define RO_VLD_HW_WRITE_POINT   0xF8
#define RW_VLD_HW_READ_POINT    0xFC
#define RW_VLD_WAITT            0x100
#define WO_VLD_SW_RESET         0x108
    #define VLD_SW_RESET_CLEAR       0x0
    #define VLD_SW_RESET_RESET       0x1
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
    #define VLD_LARB_RESET       ((unsigned)0x1 << 4)
    #define VLD_ASYNC_FIFO_RST  ((unsigned)0x1 << 8)
	#define VLD_IOMMU_RST  ((unsigned)0x1 << 12)	
    #endif
#define RW_VLD_SW_WP     0x110
  #define VLD_SW_WP_EN         (0x1 << 1)
  #define VLD_LATCH_WP_EN         (0x1 << 2)
#define VLD_BS_SUM              0x114  
#define WO_VLD_JPG_BITS_TBL     0x118
#define RW_VLD_JPG_HUFF_TBL     0x11C
	#define VLD_AC_HUFF_FLAG		0x80000000
#define RW_VLD_JPG_Q_TBL        0x120
#define RW_VLD_JPG_LAHUFF_IDX   0x124
#define RW_VLD_JPG_LAHUFF_DATA  0x128
#define RW_VLD_JPG_PRG_MISC1    0x12C
#define RW_VLD_JPG_BLK_DC_TBL   0x130
#define RW_VLD_JPG_BLK_AC_TBL   0x134
#define RW_VLD_JPG_DC_AC_NEEDED 0x138
#define RW_VLD_JPG_MEMBERSHIP   0x13C
#define RW_VLD_JPG_START        0x140
	#define VLD_DECODE_MCU_CLEAR            0
	#define VLD_DECODE_MCU		            1
#define RW_VLD_JPG_SWITCH       0x144
    #define VLD_INT_ENABLE           (0x1 << 4)
    #define VLD_INT_DISABLE          (0x0 << 4)
#define RW_VLD_JPG_FAST_DEC     0x148
	#define VLD_CLEAN_INTERNAL_BUFFER		0
	#define VLD_KEEP_INTERNAL_BUFFER        1
#define RW_VLD_JPG_RESTART      0x14C
	#define VLD_MCU_RESTART			0x1
	#define VLD_RES_DC_PRED			0x101
	#define VLD_RES_MKR_FND			0x10101
    #define VLD_JPG_STOP_EN         (0x1 << 24)
    #define VLD_JPG_STOP_RDY        (0x1 << 25)
#define RW_VLD_JPG_PRG_MODE     0x150
    #define PROG_MODE_LAST_SCAN     (1<<3)
    #define DECODE_MODE_PROG        (1<<8)
    #define JPG_DC_REFINE 1
    #define JPG_AC_FIRST 2
    #define JPG_AC_REFINE 3
    #define JPG_DC_FIRST 4
    #define WR_IDCT_DATA 8
    #define WR_VLD_COEFF 0
    #define EOB_AUTO_FILL           (1<<12)
    #define COEF_DEF_INPUT          (1<<13)
#define RW_VLD_JPG_PRG_NWB      0x154
	#define VLD_PROG_BUFF_ALL		0
	#define VLD_PROG_BUFF_NZ_ONLY	1
#define RW_VLD_JPG_AH_AL        0x158
#define RW_VLD_JPG_SS_SE        0x15C
#define RW_VLD_JPG_NZ_HIST      0x160
#define RW_VLD_JPG_MCU_COUNT    0x164
#define RW_VLD_JPG_WR_EOB_RUN   0x168
#define RW_VLD_JPG_RD_ADD0      0x16C
#define RW_VLD_JPG_RD_ADD1      0x170
#define RW_VLD_JPG_RD_ADD2      0x174
#define RW_VLD_JPG_WR_ADD0      0x178
#define RW_VLD_JPG_WR_ADD1      0x17C
#define RW_VLD_JPG_WR_ADD2      0x180
#define RW_VLD_JPG_X_IN_MCU     0x184
#define RW_VLD_JPG_Y_IN_MCU     0x188
#define RW_VLD_JPG_IDCT_WIDTH01 0x18C
#define RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0   0x190
#define RW_VLD_JPG_COEFF_WIDTH12    0x194
#define RW_VLD_JPG_RMK_AND_FINISH   0x198
	#define VLD_MCU_DECODE_FINISH           1
	#define VLD_RECEIVE_MARKER              (0x1<<16)
#define RW_VLD_JPG_CLR_ST_MRK   0x19C
#define RO_VLD_JPG_RD_EOB_RUN   0x1A0
#define RW_VLD_JPEG_MARK_REG    0x1A8
#define RO_VLD_JPG_BITS_LEFT    0x1AC
#define RO_VLD_JPG_INTERNAL_BUF 0x1B0
#define RW_VLD_JPG_MCU_COEF_SIZE 0x1C0
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
#define RW_VLD_JPG_BST_OUT_MODE  0x1DC  
    #define JPG_BST_FORMAT_SEL_422           (0x1<<13) //note 8580 is bit 17, viper/cobra is 13.....please pay attention to this.
    #define JPG_ROW_BST_OUT_ON                  (0x1<<16)
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
     #define JPG_SWAP_CBCR_SEL                     (0x1<<22)
     #define JPG_VIDEO_FORMAT_PADDING      (0x1<<21)
     #define JPG_VIDEO_FORMAT_OUTPUT        (0x1<<20)
    #endif
#endif

#define RW_VLD_MM_ROW_SIZE_COMP0         0x1E4  
#define RW_VLD_MM_ROW_SIZE_COMP1         0x1E8
#define RW_VLD_MM_ROW_SIZE_COMP2         0x1EC

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
#define RW_MBSTART_DCAC_SWITCH  0x1F0
#endif

#define RW_VLD_STA_JPG_DEC_MB_ROW           0x2A8
    #define JPG_ROW_DEC_FIRST_ROW               0x1
    #define JPG_ROW_DEC_NEXT_ROW                0x100
#define RW_VLD_JPG_MB_ROW_DEC_SWITCH        0x2AC
    #define JPG_ROW_DEC_WR_ADDR                 0x100
    #define JPG_ROW_DEC_WR_BANK1_ADDR           0x200
    #define JPG_ROW_DEC_MCU_LEVEL               0x0
    #define JPG_ROW_DEC_MCU_ROW_LEVEL           0x1
    #define JPG_ROW_DEC_PIC_LEVEL               0x3
    #define JPG_ROW_DEC_SCAN_LEVEL              0x2
#define VLDREG_JPEG_BS_DDRAM_IN_CHKSUM 0x224
#define VLDREG_JPEG_INPUT_CHKSUM           0x228
#define VLDREG_JPEG_HUFFDEC_CHKSUM       0x22c
#define VLDREG_JPEG_VLD_OUT_CHKSUM       0x230
#define VLDREG_JPEG_IDCT_OUT_CHKSUM      0x234
#define VLDREG_JPEG_WINTF_CHKSUM           0x238
#define VLDREG_JPEG_COEFF_DRAM_IN_CHKSUM    0x23c
#define VLDREG_JPEG_NZ_IN_CHKSUM            0x240
#define VLDREG_JPEG_NZ_OUT_CHKSUM         0x244
    
#define  RW_IQ_TABLE_INDEX                           0x260
#define  RW_IQ_TABLE_DATA_IN                       0x264
#define RW_VLD_JPG_ROW_DEC_COMP0_ADDR       0x2B0
#define RW_VLD_JPG_ROW_DEC_COMP1_ADDR       0x2B4
#define RW_VLD_JPG_ROW_DEC_COMP2_ADDR       0x2B8
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8555)
#define RW_VLD_JPG_ROW_DEC_COMP0_ADDR_OFST  0x2BC
#define RW_VLD_JPG_ROW_DEC_COMP1_ADDR_OFST  0x2C0
#define RW_VLD_JPG_ROW_DEC_COMP2_ADDR_OFST  0x2C4
#endif
#if (CONFIG_CHIP_VER_CURR >  CONFIG_CHIP_VER_MT8560)
#define RW_VLD_JPG_PIC_PIX_WIDTH  0x2C4
#endif
#define RW_VLD_JPG_PIC_SIZE                 0x2C8
    #define JPG_ROW_DEC_WIDTH_SHIFT             0x10
    #define JPG_ROW_DEC_HEIGHT_SHIFT            0x0
#define RW_VLD_JPG_ROW_PIC_DEC_FINISH       0x2CC
    #define JPG_ROW_DEC_PIC_FINISH              (0x1 << 8)
    #define JPG_ROW_DEC_MCU_ROW_FINISH          (0x1 << 0)
#define RW_VLD_JPG_RESTART_INTERVAL         0x2D0
    #define RST_ENABLE                          (0x1 << 16)
#define RW_VLD_JPG_LOAD_INTER_DATA_TRIGGER  0x2D4
#define RW_VLD_JPG_LOAD_INTER_DATA1         0x2D8
#define RW_VLD_JPG_LOAD_INTER_DATA2         0x2DC
#define RW_VLD_JPG_LOAD_INTER_DATA3         0x2E0
#define RW_VLD_JPG_LOAD_INTER_DATA4         0x2E4
#define RW_VLD_JPG_LOAD_INTER_DATA5         0x2E8
#define RW_VLD_JPG_SRAM_CLEAR               0x2EC
#define RW_VLD_JPG_COEF_PITCH_0             0x2EC
#define RW_VLD_JPG_COEF_PITCH_1             0x2F0
#define RW_VLD_JPG_COEF_PITCH_2             0x2F4
#define RW_VLD_JPG_DEC_BREAK                0x31c
#define RW_VLD_JPG_DEC_STOP                 0x324
#define RW_VLD_JPG_NEW_BASE                 0x3C4
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
      #define NEW_BASELINE_ENABLE             0x701
      #define NEW_BASELINE_DISABLE            0x701  
      #define NEW_PROGRESSIVE_MULTI_COLLECT   0x701
      #define NEW_PROGRESSIVE_MULTI_COLLECT_MULTI_PASS 0x11
      #define NEW_PROGRESSIVE_ENHANCE         0x700	
#else
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
        #define NEW_BASELINE_ENABLE             0x7FF
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        #define NEW_BASELINE_ENABLE             0x7BE 
#endif
        #define NEW_BASELINE_DISABLE            0x707
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        #define NEW_PROGRESSIVE_MULTI_COLLECT   0x79F
        #define NEW_PROGRESSIVE_ENHANCE         0x79E
#endif
#endif
#define RW_VLD_JPG_HSRAM_INIT               0x3C8
    #define SRAM_CLOCK_DISABLE                   0x0
    #define SRAM_CLOCK_ENABLE                    0x1
    #define FILL_ENABLE                          (0x1 << 1)
    #define FILL_DISABLE                         (0x0 << 1)
#define RW_JPG_ERROR_DECTECT                0x3CC
    
#define RW_VLD_JPG_ERROR_CONCEAL            0x3D0
    #define EC_BEC_LEN_EN                        (0x1 << 0) // baseline error code length
    #define EC_GT64_EN                           (0x1 << 1)
    #define EC_RSTMAK_EN                         (0x1 << 4)
#define RW_VLD_JPG_FILL_HSRAM               0x3D4
#define RW_VLD_JPG_HUFF_VAL                 0x3D8
    #define DC_TABLE                            (0 << 12)
    #define AC_TABLE                            (1 << 12)
#define RW_VLD_JPG_ERROR_STATUS             0x3DC


// MC register define
#define RW_MC_ADDRSWAP                      0x90
#define RW_MC_PS_DRAM_MODE                  0x1C8//read/write Dram address mode






typedef struct {
    UINT32 u4MCUx;
    UINT32 u4MCUy;
    UINT32 u4MCUWidth;                          ///< MCU number per MCU row
    UINT32 u4MCUHeight;                         ///< Total MCU row number
    UINT32 u4CombMCUWidth;
    UINT32 u4CombMCUHeight;
    UINT32 au4MCUWidth[3];
    UINT32 au4MCUHeight[3];
    UINT32 u4YBufSa;
    UINT32 u4CbBufSa;
    UINT32 u4CrBufSa;
    UINT32 u4YCoeffBufSa;
    UINT32 u4CbCoeffBufSa;
    UINT32 u4CrCoeffBufSa;
    BOOL fgAddrModified;                  ///< Output address is modified.
    UINT32 u4AddrModifiedMCUy;            ///< The MCU Y when output address modified.
} JDEC_HAL_OUTPUT_ADDR_T;


extern void vHwJdecVLDPowerOn(UINT32 u4HwId,BOOL fgPowerOn);
extern void vHwJdecHwReset(UINT32 u4HwId);
extern void vHwJdecHwCheckDram(UINT32 u4HwId);
extern void vHwJdecSRAMClear(UINT32 u4HwId);
extern void vHwJdecSetVLDWaitTimeoutValue(UINT32 u4HwId,UINT32 u4TimeoutValue);
extern void vHwJdecSetDecodeModeProgressive(UINT32 u4HwId);
extern void vHwJdecSetDecodeModeBaseline(UINT32 u4HwId);
extern void vHwJdecSetAhAl(UINT32 u4HwId,UINT32 Ah,UINT32 Al);
extern void vHwJdecSetSsSe(UINT32 u4HwId,UINT32 Ss,UINT32 Se);
extern void vHwJdecSetDcAc(UINT32 u4HwId,UINT32 Ah,UINT32 Ss);
extern void vHwJdecSetLastScan(UINT32 u4HwId,BOOL fgLastScan);
extern void vHwJdecCleanInterBuf(UINT32 u4HwId,BOOL fgClean);
extern void vHwJdecSetNoneZeroHistoryBuf(UINT32 u4HwId,UINT32 u4NoneZeroHistoryBuf);
extern void vHwJdecSetOutputNoneZeroHistoryBufOnly(UINT32 u4HwId,BOOL fgNzBufOnly);
#if JDEC_MM_MODE_ENABLE
extern void vHwJdecSeMCURowNum(UINT32 u4HwId,UINT32 u4StartRow, UINT32 u4EndRow);
extern void vHwJDecSetMMpassMode(UINT32 u4HwId);
extern INT32 i4HwJdecSetMMPassPitchPerRow(UINT32 u4HwId,UINT32 u4Comp0PitchPerRow,UINT32 u4Comp1PitchPerRow,UINT32 u4Comp2PitchPerRow,
	                                                      JDEC_JFIF_SOS_T *prScanInfo, BOOL fgLastScan);
extern  void vHwJdecHwResetMM(UINT32 u4HwId);

#endif
extern void vHwJdecSetInputBuffer(UINT32 u4HwId,UINT32 u4InBufSa,UINT32 u4InBufSz);
extern INT32 i4HwJdecSetBarrelShifterAddr(UINT32 u4HwId,UINT32 u4Addr,UINT32 u4Bits);
extern INT32 i4HwJdecSetVLDSwWritePointer(UINT32 u4HwId,BOOL fgEnable,UINT32 u4Addr);
extern void vHwJdecSetVLDBarrelShifterToJpegMode(UINT32 u4HwId,BOOL fgJpegMode);
extern void vHwJdecSetMiscReg(UINT32 u4HwId);
extern INT32 i4HwJdecCalculateOutputBufAddr(UINT32 u4HwId,UINT32 u4CurrMCUNum,
                                                   JDEC_HAL_OUTPUT_ADDR_T *prOutputAddr,
                                                   JDEC_JFIF_SOF_T *prSofInfo,
                                                   JDEC_JFIF_SOS_T *prScanInfo,
                                                   JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,
                                                   UINT32 u4OutCoefYBufSa,
                                                   UINT32 u4OutCoefCbBufSa,
                                                   UINT32 u4OutCoefCrBufSa,
                                                   BOOL fgReinit,
                                                   BOOL fgIsMultiCollectMode,
                                                   BOOL fgLastScan,
                                                   JDEC_HAL_DEC_MODE_T eDecodingMode
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
                                                   ,HANDLE_T hY
                                                   ,HANDLE_T hCb
                                                   ,HANDLE_T hCr
                                                   ,BOOL fgSlicedCoef
#endif

);
extern void vHwJdecSetOutputBufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr,
                                           JDEC_JFIF_SOS_T *prScanInfo);
extern void vHwJdecSetOutputCoefBufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr,
                                               JDEC_JFIF_SOS_T *prScanInfo);
extern void vHwJdecSetReadWriteOutputBufMode(UINT32 u4HwId);
extern void vHwJdecInitScan(UINT32 u4HwId);
extern void vHwJdecRestart(UINT32 u4HwId);
extern void vHwJdecSetBlockAddrList(UINT32 u4HwId,UINT32 *au4XAddrList,UINT32 *au4YAddrList);
extern void vHwJdecSetCurrMCUNum(UINT32 u4HwId,UINT32 u4CurrMCUNum);
extern void vHwJdecDecodeMCU(UINT32 u4HwId);
extern BOOL fgHwJdecIsDecodeFinish(UINT32 u4HwId);
extern BOOL fgHwJdecIsFoundMarker(UINT32 u4HwId);
extern UINT32 u4HwJdecGetFoundMarker(UINT32 u4HwId);
extern INT32 i4HwJdecSetBlockPostition(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetHuffmanTable(UINT32 u4HwId,JDEC_JFIF_DHT_T *prHuffmanTable,
                              UINT8 *pu1HwDcHuffTbl,UINT8 *pu1HwAcHuffTbl);
extern INT32 i4HwJdecSetBlkAndDcAcTblList(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetBlkAndCompMembership(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetQuantTable(UINT32 u4HwId,JDEC_JFIF_DQT_T *prQTblInfo);
extern INT32 i4HwJdecSetCurrUsedQuantTable(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetPitchPerRow(UINT32 u4HwId,UINT32 u4Comp0PitchPerRow,UINT32 u4Comp1PitchPerRow,UINT32 u4Comp2PitchPerRow,JDEC_JFIF_SOS_T *prScanInfo,BOOL fgCoeffPitch, BOOL fgLastScan);
extern INT32 i4HwJdecSetCoefPitchPerRow(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo);
extern INT32 i4HwJdecSetDcAcNeededList(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetBlockCount(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetEOBRUN(UINT32 u4HwId,UINT32 u4EOBRUN);
extern UINT32 u4HwJdecGetEOBRUN(UINT32 u4HwId);
extern INT32 i4HwJdecProcRestart(UINT32 u4HwId);
extern void vHwJdecSetOutputBank0BufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr);
extern void vHwJdecSetOutputBank1BufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr);
extern void vHwJdecSetMCUWidth(UINT32 u4HwId,UINT32 u4Comp0MCUWidth,UINT32 u4Comp1MCUWidth,UINT32 u4Comp2MCUWidth);
extern void vHwJdecSetPictureSize(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo);
extern void vHwJdecSetRestartInterval(UINT32 u4HwId,UINT32 u4RestartInterval);
extern void vHwJdecSetDecodeMode(UINT32 u4HwId,JDEC_HAL_DEC_MODE_T eDecodeMode);
extern void vHwJdecDecodePicture(UINT32 u4HwId);
extern BOOL fgHwJdecIsDecodePictureFinish(UINT32 u4HwId);
extern void vHwJdecDecodeMCURow(UINT32 u4HwId,BOOL fgFirstRow);
extern BOOL fgHwJdecIsDecodeMCURowFinish(UINT32 u4HwId);
extern INT32 i4HwJdecGetHwStatus(UINT32 u4HwId,JDEC_HAL_HW_STATUS_T *prHwStatus);
extern INT32 i4HwJdecSetHwStatus(UINT32 u4HwId,JDEC_HAL_HW_STATUS_T *prHwStatus);
extern void vHwJdecSetNewBaseline(UINT32 u4HwId, BOOL fgEnable);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern void vHwJDecSetNewProgressive(UINT32 u4HwId, BOOL fgMultiEnable, BOOL fgEnhanceEnable);
#endif
extern void vHwJdecSetInterrupt(UINT32 u4HwId, BOOL fgEnable);
extern INT32 i4HwJdecGetErrorStatus(UINT32 u4HwId);
extern void vHwPrintChkSum(u4HwId);
extern INT32 i4JDECReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa, UINT32 u4VFIFOEa,UINT32 *pu4Bytes);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern void vJDECErrorConceal(UINT32 u4HwId,BOOL fgGT64En, BOOL fgRstMakEn,BOOL fgErrCodeLenEn);
#endif
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
void vJDECErrorConceal(UINT32 u4HwId,BOOL fgGT64En, BOOL fgRstMakEn);
#endif
extern void vJDECSTOPPicMode(UINT32 u4HwId);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
extern void vHwJdecSetBurstMode(UINT32 u4HwId, BOOL fgBrstEnable, BOOL fg422Enable);
#endif
#if (CONFIG_CHIP_VER_CURR >  CONFIG_CHIP_VER_MT8560)
extern INT32 i4HwJdecSetVideoOutMode(UINT32 u4HwId, BOOL fgPadding16Byte, BOOL fgVideoOutput,JDEC_JFIF_SOF_T *prSofInfo);
extern void vHwJdecSetPicturePixelSize (UINT32 u4HwId, UINT32 u4PicPixelSize);
#endif

extern BOOL fgJDECGetStopReady(UINT32 u4HwId);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern void vHwJdecSetPictureSizeForProg(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prSosInfo);
extern INT32 i4HwJdecSetCoefMCUSize(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo);
extern INT32 i4HwJdecSetCoefPitch(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo, JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
                                 ,JDEC_HAL_DEC_MODE_T eDecodingMode
#endif
    );
extern void vHwJdecForNewEnhance(UINT32 u4HwId,BOOL fgOpenEOB,BOOL fgOpenCoef);
#endif
extern UINT32 u4HwJdecGetVLDWritePtr(UINT32 u4HwId);
extern UINT32 u4HwJdecGetVLDReadPtr(UINT32 u4HwId);
extern void vJDECReleaseCPU(UINT32 u4HwId);

#define D_HUFF_LOOKAHEAD_BITS	6	/* # of bits of lookahead */
typedef struct
{
	/* Basic tables: (element [0] of each array is unused) */
	INT32	ai4Maxcode[18];
		/*
			largest code of length k (-1 if none)
			(maxcode[17] is a sentinel to ensure jpeg_huff_decode terminates)
		*/
	INT32	ai4Valoffset[17];
		/*
			huffval[] offset for codes of length k
			valoffset[k] = huffval[] index of 1st symbol of code length k,
			less the smallest code of length k; so given a code of length k,
			thecorresponding symbol is huffval[code + valoffset[k]]
		*/
	INT32	ai4LookNbits[1 << D_HUFF_LOOKAHEAD_BITS];
		/* # bits, 0 if too long */
	UINT8	au1LookSym[1 << D_HUFF_LOOKAHEAD_BITS];
		/* symbol, or unused */
} JDEC_HAL_DERIVED_HUFF_TABLE_T;


#endif
