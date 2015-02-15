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

#ifndef X_IOMMU_8580_H
#define X_IOMMU_8580_H

//============================================================================
// Include files
//============================================================================
//#include "x_hal_ic.h"
#include "x_typedef.h"
#include "drv_config.h"

//============================================================================
// Macros for register read/write
//============================================================================
#define IOMMU_READ32(offset, bank)          IO_READ32(IOMMU_BASE+bank, offset)
#define IOMMU_WRITE32(offset, bank, value)  IO_WRITE32(IOMMU_BASE+bank, offset, (value))
#define IOMMU_CHKSUM_READ32(offset)         IO_READ32(IO_BASE, offset)
#define IOMMU_CHKSUM_WRITE32(offset, value) IO_WRITE32(IO_BASE, offset, (value))
//============================================================================
// Macros for memory read/write
//============================================================================
#define WRITEMEM8(Address, Value)           *(volatile UINT8 *)(Address) = Value
#define READMEM8(Address)                   *(volatile UINT8 *)(Address)

#define WRITEMEM16(Address, Value)          *(volatile UINT16 *)(Address) = Value
#define READMEM16(Address)                  *(volatile UINT16 *)(Address)

#define WRITEMEM32(Address, Value)          *(volatile UINT32 *)(Address) = Value
#define READMEM32(Address)                  *(volatile UINT32 *)(Address)

//============================================================================
// IOMMU Registers
//============================================================================
//  #define IOMMU_BASE                                      (IO_BASE + 0x5E000)
#define REG_RW_IOMMU_CFG0                   0x000           // basic setting
#define REG_RW_IOMMU_CFG1                   0x004           // page table index
#define REG_RW_IOMMU_CFG2                   0x008           // agnet_0~1 setting
#define REG_RW_IOMMU_CFG3                   0x00C           // agnet_2~3 setting
#define REG_RW_IOMMU_CFG4                   0x010           // interrupt, monitor and debug
#define REG_RW_IOMMU_CFG5                   0x014           // perfomance meter
#define REG_RW_IOMMU_CFG6                   0x018           // monitor result
#define REG_RW_IOMMU_CFG7                   0x01C           // monitor result
#define REG_RW_IOMMU_CFG8                   0x020           // monitor result
// special setting *********************
#define REG_RW_IOMMU_CFG9                   0x024           // over read protection
#define REG_RW_IOMMU_CFGA                   0x028           // over read protection
#define REG_RW_IOMMU_CFGB                   0x02C           // over read protection
#define REG_RW_IOMMU_CFGC                   0x030           // over read protection
#define REG_RW_IOMMU_CFGD                   0x034           // over read protection
// special setting &&&&&&&&&&&&&&&&&&&&&

#define IOMMU_GCPU                          0x0000
#define IOMMU_GFX                           0x1040
#define IOMMU_PNG                           0x1080
#define IOMMU_JPG                           0x10C0
#define IOMMU_RESZ                          0x1100
#define IOMMU_RLE                           0x1140
#define IOMMU_GIF                           0x1180

#define IOMMU_GFX_OTHER                     0x2040          // for OverRead used only
#define IOMMU_GFX_COMPRESSION               0x3040          // for OverRead used only

#define MON_START                           0
#define MON_DATA                            1
#define CHK_SUM_START                       0
#define CHK_SUM_RESULT                      1

#define REG_IOMMU_CFG0_DEF                  0x000000FE

#define REG_IOMMU_CFG4_EN                   0x0420090A
#define REG_IOMMU_CFG4_CLR                  0x04200905
#define REG_IOMMU_CFG4_MUTE                 0x04200900
#define REG_IOMMU_CFG4_RST_TLB              0x06200905
#define REG_IOMMU_CFG4_RST_ALL              0x07200905
#define REG_IOMMU_CFG4_DEBUG                0x0420010A

#define REG_IOMMU_CHK_SUM_RESET             0x40840         //
#define REG_IOMMU_CHK_SUM_SEL               0x40844         //
  #define AG_GFX                            0x0
  #define AG_GIF                            0x1
  #define AG_JPG                            0x2
  #define AG_PNG                            0x3
  #define AG_RESZ                           0x4
  #define AG_RLE                            0x5
#define REG_IOMMU_IADR_WRITE                0x40848         //
#define REG_IOMMU_IADR_READ                 0x4084C         //
#define REG_IOMMU_MADR_WRITE                0x40850         //
#define REG_IOMMU_MADR_READ                 0x40854         //
#define REG_IOMMU_IWDAT3                    0x40858         //
#define REG_IOMMU_IWDAT2                    0x4085C         //
#define REG_IOMMU_IWDAT1                    0x40860         //
#define REG_IOMMU_IWDAT0                    0x40864         //
#define REG_IOMMU_MWDAT3                    0x40868         //
#define REG_IOMMU_MWDAT2                    0x4086C         //
#define REG_IOMMU_MWDAT1                    0x40870         //
#define REG_IOMMU_MWDAT0                    0x40874         //

//============================================================================
// Public functions
//============================================================================
EXTERN UINT32 u4HalGetTTB0(VOID);
EXTERN UINT32 u4HalGetTTB1(VOID);
EXTERN UINT32 u4HalGetTTB(UINT32 ui4_mem_ptr);
EXTERN VOID vIOMMU_Performance(UINT32 ui4_type, UINT32 ui4_step);
EXTERN VOID vIOMMU_CheckSum(UINT32 ui4_step, UINT32 ui4_ag, UINT32 ui4_id);

#endif  // X_IOMMU_8580_H
