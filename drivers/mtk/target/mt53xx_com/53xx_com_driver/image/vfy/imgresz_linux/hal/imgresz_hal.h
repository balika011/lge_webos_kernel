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




#ifndef _IMGRESZ_HAL_H_
#define _IMGRESZ_HAL_H_


#include "x_typedef.h"
#include "imgresz_hal_if.h"

//#define IO_BASE_ADDRESS                     0x70000000L
#define IMGRESZ0_REG_OFFSET                 (IO_VIRT + 0x20000)
#define IMGRESZ1_REG_OFFSET                 (IO_VIRT + 0x6C000)
#define IMGRESZ2_REG_OFFSET                 (IO_VIRT + 0x6B000)

#define DRAM1_REG_OFFSET                    (IO_VIRT + 0x6200)
#define DRAM2_REG_OFFSET                    (IO_VIRT + 0x307000)

#define HW_IMGRESZ_NUM 3
#define WT_REG_OFFSET                           0x40804

/// Image resizer register map
#define CONFIG_CHIP_VER_MT8561 8561


#define RW_IMG_RESZ_START 0x000
  /*! \name  RW_IMG_RESZ_START bit value description
  * \anchor RW_IMG_RESZ_START bit value description
  * bit[0]: (1'b0) Trigger image resizer start doing resizing \n
  * bit[1]: (1'b0) Enable image resizer \n
  * bit[2]: (1'b0) Software reset. While issued, the internal registers will return to their initial value \n
  * @{
  */
  ///bit[0] = 0: Stable state before/after activating image resizer
  #define IMG_RESZ_UNACTIVATE           ((unsigned)0x0 << 0)
  ///bit[0] = 1: Activate image resizer (pulse signal)
  #define IMG_RESZ_ACTIVATE             ((unsigned)0x1 << 0)
  ///bit[1] = 0: Image resizer disable
  #define IMG_RESZ_DISABLE              ((unsigned)0x0 << 1)
  ///bit[1] = 1: Image resizer enable
  #define IMG_RESZ_ENABLE               ((unsigned)0x1 << 1)
  ///bit[2:3] = 0: Software reset off
  #define IMG_RESZ_SW_RESET_OFF         ((unsigned)0x0 << 2)
  ///bit[2:3] = 1: Software reset on
  #define IMG_RESZ_SW_RESET_ON          ((unsigned)0x3 << 2)
  ///bit[4] = 1: Check sum clear
  #define IMG_RESZ_MMU_RESET_ON          ((unsigned)0x3 << 20)

  
  #define IMG_RESZ_CHK_SUM_CLR          ((unsigned)0x1 << 4)
  #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
   ///bit[5] = 1: DMA software reset
  #define IMG_RESZ_DMA_SW_RST          ((unsigned)0x1 << 5)
  #define IMG_RESZ_WAIT_WR_DONE      ((unsigned)0x1 << 6)
  #endif
  #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
   ///bit[5] = 1: DMA software reset
  #define IMG_RESZ_MMU_ENABLE          ((unsigned)0x1 << 22)
  #endif
  //#ifdef IMGRESZ_HAL_MT8550
  #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
  ///bit[7] = 1: Turn On Interrupt
  #define IMG_RESZ_INT_ON               ((unsigned)0x1 << 7)
  ///bit[8:9] = ///------------------------------TBD
  #define IMG_RESZ_REGISTER_RESET_ON         ((unsigned)0x3 << 8)
  #endif
  //#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
  #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
  ///bit[16] = 1:DRAM read burst enable
  #define IMG_RESZ_RD_BURST_ON          ((unsigned)0x1 << 16)
  #endif
   #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    ///bit[18] = 1:Safe mode
  #define IMG_RESZ_DMA_SAFE_MODE                            ((unsigned)0x1 << 18)
  #endif
  /*! @} */

#define RW_IMG_RESZ_TYPE 0x004
  /*! \name  RW_IMG_RESZ_TYPE bit value description
  * \anchor RW_IMG_RESZ_TYPE bit value description
  * bit[0]: (1'b1) Jpeg_mode: jpeg mode(1) / video mode (0) \n
  * bit[2]: (1'b0) Top_field: top field(1) / bottom field(0) \n
  * bit[3]: (1'b0) Interlace: interlace mode(1) / frame mode(0) \n
  * bit[4]: (1'b1) raster_scan_in: raster scan in(1) / block based in(0) \n
  * bit[16]: (1'b0) raster_scan_out: raster scan out(1) / block based out(0) \n
  * bit[17]: (1'b0) output mode: 422(1) / 420(0) \n
  * bit[19:18]: (2'b0) CbCr component padding: Nopadding(00)/ Cb=80(10) / Cr=80(01) / CbCr=80(11) \n
  * @{
  */
  ///bit[0] = 0: Video mode
  #define IMG_RESZ_VIDEO_MODE                   ((unsigned)0x0 << 0)
  ///bit[0] = 1: Jpeg mode
  #define IMG_RESZ_JPEG_MODE                    ((unsigned)0x1 << 0)
  ///bit[1] = 0: Video mode
  #define IMG_RESZ_SEL_VID_MODE                 ((unsigned)0x0 << 1)
  ///bit[1] = 1: OSD mode
  #define IMG_RESZ_SEL_OSD_MODE                 ((unsigned)0x1 << 1)
  ///bit[2] = 0: bottom field
  #define IMG_RESZ_INTERLACE_BOTTOM_FIELD       ((unsigned)0x0 << 2)
  ///bit[2] = 1: top field
  #define IMG_RESZ_INTERLACE_TOP_FIELD          ((unsigned)0x1 << 2)
  ///bit[3] = 0: frame
  #define IMG_RESZ_FRAME                        ((unsigned)0x0 << 3)
  ///bit[3] = 0: frame
  #define IMG_RESZ_FIELD                        ((unsigned)0x1 << 3)
  ///bit[4] = 0: block based in
  #define IMG_RESZ_BLOCK_BASED_IN               ((unsigned)0x0 << 4)
  ///bit[4] = 1: raster scan in
  #define IMG_RESZ_RASTER_SCAN_IN               ((unsigned)0x1 << 4)
  ///bit[5] = 0: OSD normal mode
  #define IMG_RESZ_OSD_NORMAL_MODE              ((unsigned)0x0 << 5)
  ///bit[5] = 1: OSD partial mode
  #define IMG_RESZ_OSD_PARTIAL_MODE             ((unsigned)0x1 << 5)
  ///bit[8] = 0: bilinear filter
  #define IMG_RESZ_BILINEAR_FILTER              ((unsigned)0x0 << 8)
  ///bit[8] = 1: vertical 4 tap filter
  #define IMG_RESZ_V_4_TAP_FILTER               ((unsigned)0x1 << 8)
  //about alpha composition
  #define IMG_RESZ_FIX4_ENABLE                  ((unsigned)0x1 << 12)
  ///bit[16] = 0: block based out
  #define IMG_RESZ_BLOCK_BASED_OUT              ((unsigned)0x0 << 16)
  ///bit[16] = 1: raster scan out
  #define IMG_RESZ_RASTER_SCAN_OUT              ((unsigned)0x1 << 16)
  ///bit[17:18] = 0: output 420 mode
  #define IMG_RESZ_420_OUT                      ((unsigned)0x0 << 17)
  ///bit[17:18] = 1: output 422 mode
  #define IMG_RESZ_422_OUT                      ((unsigned)0x1 << 17)
  ///bit[17:18] = 2: output 444 mode
  #define IMG_RESZ_444_OUT                      ((unsigned)0x2 << 17)
  ///bit[19:20] = 0x0: CbCr NoPadding
  #define IMG_RESZ_CBCR_NO_PADDING              ((unsigned)0x0 << 19)
  ///bit[19:20] = 0x1: Cr Padding
  #define IMG_RESZ_CR_PADDING                   ((unsigned)0x1 << 19)
  ///bit[19:20] = 0x2: Cb Padding
  #define IMG_RESZ_CB_PADDING                   ((unsigned)0x2 << 19)
  ///bit[19:18] = 0x3: CbCr Padding
  #define IMG_RESZ_CBCR_PADDING                 ((unsigned)0x3 << 19)
  ///bit[21] = 0x0: Cb Cr Don't Swap
  #define IMG_RESZ_NOTCBCRSWAP                  ((unsigned)0x0 << 21)
  ///bit[21] = 0x1: Cb Cr Swap
  #define IMG_RESZ_CBCRSWAP                     ((unsigned)0x1 << 21)
  ///bit[24] = 0x0: video to video
  #define IMG_RESZ_V2V                          ((unsigned)0x0 << 24)
  ///bit[24] = 0x1: video to OSD(AYCbCr)
  #define IMG_RESZ_V2OSD                        ((unsigned)0x1 << 24)
  /*! @} */

#define RW_IMG_RESZ_JPG_MODE 0x008
  /*! \name  RW_IMG_RESZ_JPG_MODE bit value description
  * \anchor RW_IMG_RESZ_JPG_MODE bit value description
  * bit[31]: (1'b0) Select how many linw in color buffer \n
  * bit[18]: (1'b0) preload: preload DRAM data before writing \n
  * bit[16:14]: (3'b111) ycbcr: record the existence of each color. 3'b100: y, 3'b010: cb, 3'b001: cr \n
  * bit[13]: (1'b1) first_bl: the first block line of the jpeg image \n
  * bit[12]: (1'b0) last_bl: the last block line of the jpeg image \n
  * bit[9:8]: (2'b01) jpg_v_factor_y_s1: number of lines in y color buffer. 2'b00: 8, 2'b01: 16 \n
  * bit[5:4]: (2'b00) jpg_v_factor_cb_s1: number of lines in cb color buffer. 2'b00: 8, 2'b01: 16 \n
  * bit[1:0]: (2'b00) jpg_v_factor_cr_s1: number of lines in cr color buffer. 2'b00: 8, 2'b01: 16 \n
  * @{
  */
  ///bit[31] = 0: number of lines in a color buffer is determined
  #define IMG_RESZ_LINES_ASSIGNED_NOT_DIRECTLY          ((unsigned)0x0 << 31)
  ///bit[31] = 1: number of lines in a color buffer is aggisned directly
  #define IMG_RESZ_LINES_ASSIGNED_DIRECTLY              ((unsigned)0x1 << 31)
  ///bit[18] = 0: Normal case (no pre-load)
  #define IMG_RESZ_NO_PRELOAD_DRAM_DATA                 ((unsigned)0x0 << 18)
  ///bit[18] = 1: Preload DRAM data before writing
  #define IMG_RESZ_PRELOAD_DRAM_DATA                    ((unsigned)0x1 << 18)
  ///bit[16:14] = 1: record the existence cr color
  #define IMG_RESZ_RECORD_CR                            ((unsigned)0x1 << 14)
  ///bit[16:14] = 2: record the existence cb color
  #define IMG_RESZ_RECORD_CB                            ((unsigned)0x2 << 14)
  ///bit[16:14] = 4: record the existence y color
  #define IMG_RESZ_RECORD_Y                             ((unsigned)0x4 << 14)
  ///bit[13] = 1: Not first block line decode
  #define IMG_RESZ_NOT_FIRST_BLOCK_LINE                 ((unsigned)0x0 << 13)
  ///bit[13] = 1: First block line decode
  #define IMG_RESZ_FIRST_BLOCK_LINE                     ((unsigned)0x1 << 13)
  ///bit[12] = 1: Not last block line decode
  #define IMG_RESZ_NOT_LAST_BLOCK_LINE                  ((unsigned)0x0 << 12)
  ///bit[12] = 1: last block line decode
  #define IMG_RESZ_LAST_BLOCK_LINE                      ((unsigned)0x1 << 12)
  ///bit[11:10], Y color buffer H Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_YH_SHIFT                   10
  ///bit[11:10], Y color buffer V Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_YV_SHIFT                   8
  ///bit[11:10], Cb color buffer H Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_CbH_SHIFT                  6
  ///bit[11:10], Cb color buffer V Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_CbV_SHIFT                  4
  ///bit[11:10], Cr color buffer H Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_CrH_SHIFT                  2
  ///bit[11:10], Cr color buffer V Factor (factor - 1)
  #define IMG_RESZ_JPEG_MODE_CrV_SHIFT                  0

  /*! @} */

#define RW_IMG_RESZ_MEM_IF_MODE 0x00C
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)

  /*! \name  RW_IMG_RESZ_MEM_IF_MODE bit value description
  * \anchor RW_IMG_RESZ_MEM_IF_MODE bit value description
  * bit[8]: (1'b0) Reset address register: Normal(0), Reset(1) \n
  * bit[7:4]: (4'h8) burst_limit: DRAM request burst limit (1/2/4/8) \n
  * bit[3:2]: (2'b0) out_addr_swap: output address swap mode (0/1/2) \n
  * bit[1:0]: (2'b0) in_addr_swap: input address swap mode (0/1/2) \n
  * @{
  */
  ///bit[2:0] = 0: input address swap mode 0
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_0              ((unsigned)0x0 << 0)
  ///bit[2:0] = 1: input address swap mode 1
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_1              ((unsigned)0x1 << 0)
  ///bit[2:0] = 2: input address swap mode 2
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_2              ((unsigned)0x2 << 0)
  ///bit[2:0] = 4: input address swap mode 4
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_3              ((unsigned)0x3 << 0)
  ///bit[2:0] = 4: input address swap mode 4
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_4              ((unsigned)0x4 << 0)
  ///bit[2:0] = 5: input address swap mode 5
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_5              ((unsigned)0x5 << 0)
  ///bit[2:0] = 6: input address swap mode 6
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_6              ((unsigned)0x6 << 0)  
  ///bit[2:0] = 7: input address swap mode 7
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_7              ((unsigned)0x7 << 0)  
  ///bit[6:4] = 0: output address swap mode 0
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_0             ((unsigned)0x0 << 4)
  ///bit[6:4] = 1: output address swap mode 1
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_1             ((unsigned)0x1 << 4)
  ///bit[6:4] = 2: output address swap mode 2
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_2             ((unsigned)0x2 << 4)
  ///bit[6:4] = 0: output address swap mode 3
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_3             ((unsigned)0x3 << 4)
  ///bit[6:4] = 0: output address swap mode 4
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_4             ((unsigned)0x4 << 4)
  ///bit[6:4] = 1: output address swap mode 5
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_5             ((unsigned)0x5 << 4)
  ///bit[6:4] = 2: output address swap mode 6
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_6             ((unsigned)0x6 << 4)
  ///bit[6:4] = 2: output address swap mode 7
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_7             ((unsigned)0x7 << 4)
  ///bit[12:8] = 1: DRAM request burst limit 1
  #define IMG_RESZ_DRAM_BURST_LIMIT_1               ((unsigned)0x1 << 8)
  ///bit[12:8] = 2: DRAM request burst limit 2
  #define IMG_RESZ_DRAM_BURST_LIMIT_2               ((unsigned)0x2 << 8)
  ///bit[12:8] = 2: DRAM request burst limit 2
  #define IMG_RESZ_DRAM_BURST_LIMIT_3               ((unsigned)0x3 << 8)
  ///bit[12:8] = 4: DRAM request burst limit 4
  #define IMG_RESZ_DRAM_BURST_LIMIT_4               ((unsigned)0x4 << 8)
  ///bit[12:8] = 8: DRAM request burst limit 8
  #define IMG_RESZ_DRAM_BURST_LIMIT_8               ((unsigned)0x8 << 8)
  ///bit[12:8] = 16: DRAM request burst limit 16
  #define IMG_RESZ_DRAM_BURST_LIMIT_16               ((unsigned)0x10 << 8)
  ///bit[8] = 0: Reset addres register, normal
  #define IMG_RESZ_RESET_ADDR_NORMAL                ((unsigned)0x0 << 8)
  ///bit[8] = 1: Reset addres register, with a new block line
  #define IMG_RESZ_RESET_ADDR_NEW_BLOCK_LINE        ((unsigned)0x1 << 8)

  ///bit[12] = 1: Reset the output address
  #define IMG_RESZ_REST_ADDRESS                     ((unsigned)0x1 << 20)
  
  ///bit[16] = 1: Keep issue DRAM request
  #define IMG_RESZ_KEEP_DRAM_READ_REQUEST           ((unsigned)0x1 << 16)
  /*! @} */
#else

//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

  ///bit[2:0] = 0: input address swap mode 0
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_0              ((unsigned)0x0 << 0)
  ///bit[2:0] = 1: input address swap mode 1
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_1              ((unsigned)0x1 << 0)
  ///bit[2:0] = 2: input address swap mode 2
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_2              ((unsigned)0x2 << 0)
  ///bit[2:0] = 4: input address swap mode 4
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_4              ((unsigned)0x4 << 0)
  ///bit[2:0] = 5: input address swap mode 5
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_5              ((unsigned)0x5 << 0)
  ///bit[2:0] = 6: input address swap mode 6
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_6              ((unsigned)0x6 << 0)  
  ///bit[2:0] = 7: input address swap mode 7
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_7              ((unsigned)0x7 << 0)  
  ///bit[6:4] = 0: output address swap mode 0
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_0             ((unsigned)0x0 << 4)
  ///bit[6:4] = 1: output address swap mode 1
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_1             ((unsigned)0x1 << 4)
  ///bit[6:4] = 2: output address swap mode 2
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_2             ((unsigned)0x2 << 4)
  ///bit[6:4] = 0: output address swap mode 3
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_3             ((unsigned)0x3 << 4)
  ///bit[6:4] = 0: output address swap mode 4
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_4             ((unsigned)0x4 << 4)
  ///bit[6:4] = 1: output address swap mode 5
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_5             ((unsigned)0x5 << 4)
  ///bit[6:4] = 2: output address swap mode 6
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_6             ((unsigned)0x6 << 4)
  ///bit[6:4] = 2: output address swap mode 7
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_7             ((unsigned)0x7 << 4)
  ///bit[11:8] = 1: DRAM request burst limit 1
  #define IMG_RESZ_DRAM_BURST_LIMIT_1               ((unsigned)0x1 << 8)
  ///bit[11:8] = 2: DRAM request burst limit 2
  #define IMG_RESZ_DRAM_BURST_LIMIT_2               ((unsigned)0x2 << 8)
  ///bit[11:8] = 4: DRAM request burst limit 4
  #define IMG_RESZ_DRAM_BURST_LIMIT_4               ((unsigned)0x4 << 8)
  ///bit[11:8] = 8: DRAM request burst limit 8
  #define IMG_RESZ_DRAM_BURST_LIMIT_8               ((unsigned)0x8 << 8)
  ///bit[8] = 0: Reset addres register, normal
  #define IMG_RESZ_RESET_ADDR_NORMAL                ((unsigned)0x0 << 8)
  ///bit[8] = 1: Reset addres register, with a new block line
  #define IMG_RESZ_RESET_ADDR_NEW_BLOCK_LINE        ((unsigned)0x1 << 8)

  ///bit[12] = 1: Reset the output address
  #define IMG_RESZ_REST_ADDRESS                     ((unsigned)0x1 << 12)
  
  ///bit[16] = 1: Keep issue DRAM request
  #define IMG_RESZ_KEEP_DRAM_READ_REQUEST           ((unsigned)0x1 << 16)
  /*! @} */
#else
  /*! \name  RW_IMG_RESZ_MEM_IF_MODE bit value description
  * \anchor RW_IMG_RESZ_MEM_IF_MODE bit value description
  * bit[8]: (1'b0) Reset address register: Normal(0), Reset(1) \n
  * bit[7:4]: (4'h8) burst_limit: DRAM request burst limit (1/2/4/8) \n
  * bit[3:2]: (2'b0) out_addr_swap: output address swap mode (0/1/2) \n
  * bit[1:0]: (2'b0) in_addr_swap: input address swap mode (0/1/2) \n
  * @{
  */
  ///bit[1:0] = 0: input address swap mode 0
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_0              ((unsigned)0x0 << 0)
  ///bit[1:0] = 1: input address swap mode 1
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_1              ((unsigned)0x1 << 0)
  ///bit[1:0] = 2: input address swap mode 2
  #define IMG_RESZ_IN_ADDR_SWAP_MODE_2              ((unsigned)0x2 << 0)
  ///bit[3:2] = 0: output address swap mode 0
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_0             ((unsigned)0x0 << 2)
  ///bit[3:2] = 1: output address swap mode 1
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_1             ((unsigned)0x1 << 2)
  ///bit[3:2] = 2: output address swap mode 2
  #define IMG_RESZ_OUT_ADDR_SWAP_MODE_2             ((unsigned)0x2 << 2)
  ///bit[7:4] = 1: DRAM request burst limit 1
  #define IMG_RESZ_DRAM_BURST_LIMIT_1               ((unsigned)0x1 << 4)
  ///bit[7:4] = 2: DRAM request burst limit 2
  #define IMG_RESZ_DRAM_BURST_LIMIT_2               ((unsigned)0x2 << 4)
  ///bit[7:4] = 4: DRAM request burst limit 4
  #define IMG_RESZ_DRAM_BURST_LIMIT_4               ((unsigned)0x4 << 4)
  ///bit[7:4] = 8: DRAM request burst limit 8
  #define IMG_RESZ_DRAM_BURST_LIMIT_8               ((unsigned)0x8 << 4)
  ///bit[8] = 0: Reset addres register, normal
  #define IMG_RESZ_RESET_ADDR_NORMAL                ((unsigned)0x0 << 8)
  ///bit[8] = 1: Reset addres register, with a new block line
  #define IMG_RESZ_RESET_ADDR_NEW_BLOCK_LINE        ((unsigned)0x1 << 8)
  
  ///bit[16] = 1: Keep issue DRAM request
  #define IMG_RESZ_KEEP_DRAM_READ_REQUEST           ((unsigned)0x1 << 16)
  
#endif
#endif
  

  /*! @} */
  
#define RW_IMG_RESZ_SRC_BUF_LEN 0x010
  /*! \name  RW_IMG_RESZ_SRC_BUF_LEN bit value description
  * \anchor RW_IMG_RESZ_SRC_BUF_LEN bit value description
  * bit[11:0]: (12'd0) src_buf_len (unit:ow). In jpeg mode, color buffer length. In video mode, source buffer length.\n
  * @{
  */

  /*! @} */

#define RW_IMG_RESZ_INTERFACE_SWITCH 0x014
  /*! \name  RW_IMG_RESZ_INTERFACE_SWITCH bit value description
  * \anchor RW_IMG_RESZ_INTERFACE_SWITCH bit value description
  * bit[0]: (0'h0) jpeg interface on for tracking with jpeg decode hardware
  * @{
  */
  ///bit[0] = 0: off for tracking with jpeg decode hardware
  #define IMG_RESZ_NOT_TRACKING_WITH_JPG_HW         ((unsigned)0x0 << 0)
  ///bit[1] = 1: on for tracking with jpeg decode hardware
  #define IMG_RESZ_TRACKING_WITH_JPG_HW             ((unsigned)0x1 << 0)
  /*! @} */

#define RW_IMG_RESZ_TGT_BUF_LEN 0x018
  /*! \name  RW_IMG_RESZ_TGT_BUF_LEN bit value description
  * \anchor RW_IMG_RESZ_TGT_BUF_LEN bit value description
  * bit[31]: (1'b1) extend_16: use 16 bits to store a component value in temp line buffer.\n
  * bit[28:24]: (5'd23) lbuffer_max: line buffer size (unit:ow)(SRAM). \n
  * bit[9:0]: (0'h0) tgt_buf_len (unit:ow). target buffer length.\n
  * @{
  */
  ///bit[31] = 0: not extend 16 bits
  #define IMG_RESZ_BOUND_EXTEND_16_OFF              ((unsigned)0x0 << 31)
  ///bit[31] = 1: extend 16 bits
  #define IMG_RESZ_BOUND_EXTEND_16_ON               ((unsigned)0x1 << 31)

  #define IMG_RESZ_LINE_BUFFER_LEN_SHIFT            24
  #define IMG_RESZ_TGT_BUFFER_LEN_SHIFT             0
  /*! @} */

#define RW_IMG_RESZ_SRC_Y_ADDR_BASE1 0x01C
  /*! \name  RW_IMG_RESZ_SRC_Y_ADDR_BASE1 bit value description
  * \anchor RW_IMG_RESZ_SRC_Y_ADDR_BASE1 bit value description
  * bit[25:0]: (26'h0) ybuf_addr_base1(unit:0w). \n
  *                           In jpeg mode, base address of current y color buffer. \n
  *                           In video mode, y component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_Y_ADDR_BASE2 0x020
  /*! \name  RW_IMG_RESZ_SRC_Y_ADDR_BASE2 bit value description
  * \anchor RW_IMG_RESZ_SRC_Y_ADDR_BASE2 bit value description
  * bit[25:0]: (26'h0) ybuf_addr_base2(unit:ow). \n
  *                           In jpeg mode, base address of current y color buffer. \n
  *                           In video mode, y component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_CB_ADDR_BASE1 0x024
  /*! \name  RW_IMG_RESZ_SRC_CB_ADDR_BASE1 bit value description
  * \anchor RW_IMG_RESZ_SRC_CB_ADDR_BASE1 bit value description
  * bit[25:0]: (26'h0) cbbuf_addr_base1(unit:ow). \n
  *                           In jpeg mode, base address of current cb color buffer. \n
  *                           In video mode, cb component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_CB_ADDR_BASE2 0x028
  /*! \name  RW_IMG_RESZ_SRC_CB_ADDR_BASE2 bit value description
  * \anchor RW_IMG_RESZ_SRC_CB_ADDR_BASE2 bit value description
  * bit[25:0]: (26'h0) cbbuf_addr_base2(unit:ow). \n
  *                           In jpeg mode, base address of current cb color buffer. \n
  *                           In video mode, cb component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_CR_ADDR_BASE1 0x02C
  /*! \name  RW_IMG_RESZ_SRC_CR_ADDR_BASE1 bit value description
  * \anchor RW_IMG_RESZ_SRC_CR_ADDR_BASE1 bit value description
  * bit[25:0]: (26'h0) crbuf_addr_base1(unit:ow). \n
  *                           In jpeg mode, base address of current cr color buffer. \n
  *                           In video mode, cr component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_CR_ADDR_BASE2 0x030
  /*! \name  RW_IMG_RESZ_SRC_CR_ADDR_BASE2 bit value description
  * \anchor RW_IMG_RESZ_SRC_CR_ADDR_BASE2 bit value description
  * bit[25:0]: (26'h0) crbuf_addr_base2(unit:ow). \n
  *                           In jpeg mode, base address of current cr color buffer. \n
  *                           In video mode, cr component source base address. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_TGT_Y_ADDR_BASE 0x034
  /*! \name  RW_IMG_RESZ_TGT_Y_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_TGT_Y_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) tgt_y_addr_base1: y component target base address(unit:ow). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_TGT_C_ADDR_BASE 0x038
  /*! \name  RW_IMG_RESZ_TGT_C_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_TGT_C_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) tgt_c_addr_base1: c component target base address(unit:ow). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_SIZE_Y 0x040
  /*! \name  RW_IMG_RESZ_SRC_SIZE_Y bit value description
  * \anchor RW_IMG_RESZ_SRC_SIZE_Y bit value description
  * bit[31:16]: (16'h0) src_width_y: y component source width. \n
  * bit[15:0]: (16'h0) src_height_y: y component source height. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_SIZE_CB 0x044
  /*! \name  RW_IMG_RESZ_SRC_SIZE_CB bit value description
  * \anchor RW_IMG_RESZ_SRC_SIZE_CB bit value description
  * bit[31:16]: (16'h0) src_width_cb. \n
  *                             In jpeg mode, cb component source width. \n
  *                             In video mode, c component source width. \n
  * bit[15:0]: (16'h0) src_height_cb. \n
  *                             In jpeg mode, cb component source height. \n
  *                             In video mode, c component source height. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_SIZE_CR 0x048
  /*! \name  RW_IMG_RESZ_SRC_SIZE_CR bit value description
  * \anchor RW_IMG_RESZ_SRC_SIZE_CR bit value description
  * only supported in jpeg mode
  * bit[31:16]: (16'h0) src_width_cr: cr component source width. \n
  * bit[15:0]: (16'h0) src_height_cr: cr component source height. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_TGT_SIZE 0x04C
  /*! \name  RW_IMG_RESZ_TGT_SIZE bit value description
  * \anchor RW_IMG_RESZ_TGT_SIZE bit value description
  * bit[23:12]: (12'h0) tgt_width: target width. \n
  * bit[11:0]: (12'h0) tgt_height: target height. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_OFFSET_Y 0x54
  /*! \name  RW_IMG_RESZ_SRC_OFFSET_Y bit value description
  * \anchor RW_IMG_RESZ_SRC_OFFSET_Y bit value description
  * only support in video mode
  * bit[23:12]: (12'h0) rd_h_offset_y: y component horizontal reading offset(unit:pixel). \n
  * bit[11:0]: (12'h0) rd_v_offset_y: y component vertical reading offset(unit:pixel). \n
  * @{
  */

#define RW_IMG_RESZ_SRC_OFFSET_CB 0x058
  /*! \name  RW_IMG_RESZ_SRC_OFFSET_CB bit value description
  * \anchor RW_IMG_RESZ_SRC_OFFSET_CB bit value description
  * only support in video mode
  * bit[23:12]: (12'h0) rd_h_offset_cb: cb component horizontal reading offset(unit:pixel). \n
  * bit[11:0]: (12'h0) rd_v_offset_cb: cb component vertical reading offset(unit:pixel). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_SRC_OFFSET_CR 0x05C
  /*! \name  RW_IMG_RESZ_SRC_OFFSET_CR bit value description
  * \anchor RW_IMG_RESZ_SRC_OFFSET_CR bit value description
  * only support in video mode
  * bit[23:12]: (12'h0) rd_h_offset_cr: cr component horizontal reading offset(unit:pixel). \n
  * bit[11:0]: (12'h0) rd_v_offset_cr: cr component vertical reading offset(unit:pixel). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_TGT_OFFSET 0x060
  /*! \name  RW_IMG_RESZ_TGT_OFFSET bit value description
  * \anchor RW_IMG_RESZ_TGT_OFFSET bit value description
  * bit[23:12]: (12'h0) wr_h_offset:  component horizontal writing offset(unit:pixel). \n
  * bit[11:0]: (12'h0) wr_v_offset:  component vertical writing offset. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_H8TAPS_SCL_Y 0x064
  /*! \name  RW_IMG_RESZ_H8TAPS_SCL_Y bit value description
  * \anchor RW_IMG_RESZ_H8TAPS_SCL_Y bit value description
  * bit[31:19]: (13'h0) h8taps_scl_offset_y: horizontal 8taps filter scaling offset, y. \n
  * bit[18:0]: (19'h800) h8taps_scl_factor_y: horizontal 8taps filter scaling factor,y. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_H8TAPS_SCL_CB 0x068
  /*! \name  RW_IMG_RESZ_H8TAPS_SCL_CB bit value description
  * \anchor RW_IMG_RESZ_H8TAPS_SCL_CB bit value description
  * bit[31:19]: (13'h0) h8taps_scl_offset_cb \n
  *                             In jpeg mode, horizontal 8taps filter scaling offset, cb. \n
  *                             In video mode, horizontal 8taps filter scaling offset, c. \n
  * bit[18:0]: (19'h800) h8taps_scl_factor_cb. \n
  *                             In jpeg mode, horizontal 8taps filter scaling factor, cb. \n
  *                             In video mode, horizontal 8taps filter scaling factor, c. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_H8TAPS_SCL_CR 0x06C
  /*! \name  RW_IMG_RESZ_H8TAPS_SCL_CR bit value description
  * \anchor RW_IMG_RESZ_H8TAPS_SCL_CR bit value description
  * only supported in jpeg mode
  * bit[31:19]: (13'h0) h8taps_scl_offset_cr: horizontal 8taps filter scaling offset, cr. \n
  * bit[18:0]: (19'h800) h8taps_scl_factor_cr: horizontal 8taps filter scaling factor, cr. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_HSA_SCL_Y 0x070
  /*! \name  RW_IMG_RESZ_HSA_SCL_Y bit value description
  * \anchor RW_IMG_RESZ_HSA_SCL_Y bit value description
  * bit[22:12]: (11'h0) hsa_scl_offset_y: horizontal source accumulator scaling offset, y. \n
  * bit[11:0]: (12'h800) hsa_scl_factor_y: horizontal source accumulator scaling factor,y. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_HSA_SCL_CB 0x074
  /*! \name  RW_IMG_RESZ_HSA_SCL_CB bit value description
  * \anchor RW_IMG_RESZ_HSA_SCL_CB bit value description
  * bit[22:12]: (11'h0) hsa_scl_offset_cb \n
  *                             In jpeg mode, horizontal source accumulator scaling offset, cb. \n
  *                             In video mode, horizontal source accumulator scaling offset, c. \n
  * bit[11:0]: (12'h800) hsa_scl_factor_cb. \n
  *                             In jpeg mode, horizontal source accumulator scaling factor, cb. \n
  *                             In video mode, horizontal source accumulator scaling factor, c. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_HSA_SCL_CR 0x078
  /*! \name  RW_IMG_RESZ_HSA_SCL_CR bit value description
  * \anchor RW_IMG_RESZ_HSA_SCL_CR bit value description
  * only supported in jpeg mode
  * bit[22:12]: (11'h0) hsa_scl_offset_cr: horizontal  source accumulator scaling offset, cr. \n
  * bit[11:0]: (12'h800) hsa_scl_factor_cr: horizontal  source accumulator scaling factor, cr. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_V_SCL_Y 0x07C
  /*! \name  RW_IMG_RESZ_V_SCL_Y bit value description
  * \anchor RW_IMG_RESZ_V_SCL_Y bit value description
  * bit[23:12]: (12'h0) v_scl_offset_y: vertical bilinear filter scaling offset, y. \n
  * bit[11]: (1'b1) v_scl_up_y: scaling up(1)/scaling down(0), y. \n
  * bit[10:0]: (11'h800) v_scl_factor_y: vertical bilinear filter scaling factor,y. \n
  * @{
  */
  ///bit[11] = 0: Down scaling
  #define IMG_RESZ_Y_VERTICAL_DOWN_SCALING      ((unsigned)0x0 << 0)
  ///bit[11] = 1: Up scaling
  #define IMG_RESZ_Y_VERTICAL_UP_SCALING        ((unsigned)0x1 << 0)
  /*! @} */

#define RW_IMG_RESZ_V_SCL_CB 0x080
  /*! \name  RW_IMG_RESZ_V_SCL_CB bit value description
  * \anchor RW_IMG_RESZ_V_SCL_CB bit value description
  * bit[23:12]: (12'h0) v_scl_offset_cb \n
  *                             In jpeg mode, vertical bilinear filterscaling offset, cb. \n
  *                             In video mode, vertical bilinear filter scaling offset, c. \n
  * bit[11]: (1'b1) v_scl_up_cb \n
  *                      In jpeg mode, scaling up(1)/scaling down(0), cb. \n
  *                      In video mode, scaling up(1)/scaling down(0), c. \n
  * bit[10:0]: (11'h800) v_scl_factor_cb. \n
  *                             In jpeg mode, vertical bilinear filter scaling factor, cb. \n
  *                             In video mode, vertical bilinear filter scaling factor, c. \n
  * @{
  */
  ///bit[11] = 0: Down scaling
  #define IMG_RESZ_CB_VERTICAL_DOWN_SCALING      ((unsigned)0x0 << 0)
  ///bit[11] = 1: Up scaling
  #define IMG_RESZ_CB_VERTICAL_UP_SCALING        ((unsigned)0x1 << 0)
  /*! @} */

#define RW_IMG_RESZ_V_SCL_CR 0x084
  /*! \name  RW_IMG_RESZ_V_SCL_CR bit value description
  * \anchor RW_IMG_RESZ_V_SCL_CR bit value description
  * only supported in jpeg mode
  * bit[23:12]: (12'h0) v_scl_offset_cr: vertical bilinear filterscaling offset, cr. \n
  * bit[11]: (1'b1) v_scl_up_cr: scaling up(1)/scaling down(0), cr. \n
  * bit[10:0]: (11'h800) v_scl_factor_cr: vertical bilinear filter scaling factor, cr. \n
  * @{
  */
  ///bit[11] = 0: Down scaling
  #define IMG_RESZ_CR_VERTICAL_DOWN_SCALING      ((unsigned)0x0 << 0)
  ///bit[11] = 1: Up scaling
  #define IMG_RESZ_CR_VERTICAL_UP_SCALING        ((unsigned)0x1 << 0)
  /*! @} */

#define RW_IMG_RESZ_V4TAPS_SCL_Y 0x088
  /*! \name  RW_IMG_RESZ_V4TAPS_SCL_Y bit value description
  * \anchor RW_IMG_RESZ_V4TAPS_SCL_Y bit value description
  * only supported in video mode
  * bit[30:19]: (12'h0) v4taps_scl_offset_y: vertical 4taps filter scaling offset, y. \n
  * bit[18:0]: (19'h800) v4taps_scl_factor_y: vertical 4taps filter scaling factor, y. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_V4TAPS_SCL_CB 0x08C
  /*! \name  RW_IMG_RESZ_V4TAPS_SCL_CB bit value description
  * \anchor RW_IMG_RESZ_V4TAPS_SCL_CB bit value description
  * only supported in video mode
  * bit[30:19]: (12'h0) v4taps_scl_offset_cb: vertical 4taps filter scaling offset, c. \n
  * bit[18:0]: (19'h800) v4taps_scl_factor_cb: vertical 4taps filter scaling factor, c. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_V4TAPS_SCL_CR 0x090
  /*! \name  RW_IMG_RESZ_V4TAPS_SCL_CR bit value description
  * \anchor RW_IMG_RESZ_V4TAPS_SCL_CR bit value description
  * only supported in video mode
  * bit[30:19]: (12'h0) v4taps_scl_offset_cr: vertical 4taps filter scaling offset, c. \n
  * bit[18:0]: (19'h800) v4taps_scl_factor_cr: vertical 4taps filter scaling factor, c. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_TMP_ADDR_BASE 0x094
  /*! \name  RW_IMG_RESZ_TMP_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_TMP_ADDR_BASE bit value description
  * only supported in jpeg mode
  * bit[25:0]: (25'h0) tmp_addr_base (unit:ow): temporary buffer base adress. \n
  * @{
  */
  /*! @} */
//#ifndef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)

#define RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF 0x098
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Y : the line number in Y color buffer. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF 0x09C
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Cb : the line number in Cb color buffer. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF 0x0A0
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Cr : the line number in Cr color buffer. \n
  * @{
  */
  /*! @} */

#else
#define RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE 0x098
  /*! \name  RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) pld_y_addr_base1: y component pre-load base address(unit:ow). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE 0x09C
  /*! \name  RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) pld_c_addr_base1: c component pre-load base address(unit:ow). \n
  * @{
  */
  /*! @} */
  
#endif
//#ifndef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
#define RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE 0x0A4
  /*! \name  RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) pld_y_addr_base1: y component pre-load base address(unit:ow). \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE 0x0A8
  /*! \name  RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE bit value description
  * \anchor RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE bit value description
  * bit[25:0]: (26'h0) pld_c_addr_base1: c component pre-load base address(unit:ow). \n
  * @{
  */
  /*! @} */

#endif
//#ifdef IMGRESZ_HAL_MT8550 // Added By Youlin.Pei For MT8550 Emulation! 
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

#define RW_IMG_RESZ_H8TAP_OFSET_Y       0x0C8

#define RW_IMG_RESZ_H8TAP_OFSET_CB      0x0CC

#define RW_IMG_RESZ_H8TAP_OFSET_CR      0x0D0

#define RW_IMG_RESZ_V4TAP_OFSET_Y       0x0D4

#define RW_IMG_RESZ_V4TAP_OFSET_C       0x0D8
#endif

#define RO_IMG_RESZ_CHECK_SUM_REG 0x0E4
  /*! \name  RW_IMG_RESZ_CHECK_SUM_REG bit value description
  * \anchor RW_IMG_RESZ_CHECK_SUM_REG bit value description
  * bit[31:0]: (32'h0) Check sum data. \n
  * @{
  */
  /*! @} */

#define RO_IMG_RESZ_INTERFACE_MONITOR_REG 0x0EC
  /*! \name  RO_IMG_RESZ_INTERFACE_MONITOR_REG bit value description
  * \anchor RO_IMG_RESZ_INTERFACE_MONITOR_REG bit value description
  * @{
  */
  /*! @} */

#define RO_IMG_RESZ_FSM_MONITOR_REG 0x0F0
  /*! \name  RO_IMG_RESZ_FSM_MONITOR_REG bit value description
  * \anchor RO_IMG_RESZ_FSM_MONITOR_REG bit value description
  * @{
  */
  /*! @} */

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
#define WORK_COUNTER                    ((unsigned)0x1F << 18)
#endif

#define RO_IMG_RESZ_STATUS_MONITOR_REG 0x0F4
  /*! \name  RO_IMG_RESZ_STATUS_MONITOR_REG bit value description
  * \anchor RO_IMG_RESZ_STATUS_MONITOR_REG bit value description
  * @{
  */
  /*! @} */

#define RO_IMG_RESZ_DATA_MONITOR_REG 0x0F8
  /*! \name  RO_IMG_RESZ_DATA_MONITOR_REG bit value description
  * \anchor RO_IMG_RESZ_DATA_MONITOR_REG bit value description
  * @{
  */
  /*! @} */

#define RO_IMG_RESZ_DONE 0x0FC
  /*! \name  RW_IMG_RESZ_DONE bit value description
  * \anchor RW_IMG_RESZ_DONE bit value description
  * bit[0]: done_pic: finish one picture. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_MODE_SETTING 0x100
  /*! \name  RO_IMG_RESZ_OSD_MODE_SETTING bit value description
  * \anchor RO_IMG_RESZ_OSD_MODE_SETTING bit value description
  * bit[0]: OSD direct mode selection, index mode(0), direct mode(1). \n
  * bit[1:2]: OSD input type mode. \n
  * bit[3]: OSD bit extension repeating, off(0), on(1). \n
  * bit[4:5]: OSD scaling type for dealing with alpha change. \n
  * bit[8:10]: OSD output type sclection. \n
  * bit[11]: OSD switch the position of Cb and Cr, off(0), on(1). \n
  * bit[12:15]: OSD 8-level alpha blending. \n
  * bit[16:23]: OSD user defined alpha value. \n
  * @{
  */
  ///bit[0] = 0: osd index mode
  #define IMG_RESZ_OSD_INDEX_MODE           ((unsigned)0x0 << 0)
  ///bit[0] = 1: osd direct mode
  #define IMG_RESZ_OSD_DIRECT_MODE          ((unsigned)0x1 << 0)
  ///bit[1:2] = 1: osd direct mode
  #define IMG_RESZ_OSD_INDEX_2BPP           ((unsigned)0x1 << 1)
  ///bit[1:2] = 2: osd direct mode
  #define IMG_RESZ_OSD_INDEX_4BPP           ((unsigned)0x2 << 1)
  ///bit[1:2] = 3: osd direct mode
  #define IMG_RESZ_OSD_INDEX_8BPP           ((unsigned)0x3 << 1)
  ///bit[1:2] = 0: osd direct mode
  #define IMG_RESZ_OSD_DIRECT_RGB565        ((unsigned)0x0 << 1)
  ///bit[1:2] = 1: osd direct mode
  #define IMG_RESZ_OSD_DIRECT_ARGB1555      ((unsigned)0x1 << 1)
  ///bit[1:2] = 2: osd direct mode
  #define IMG_RESZ_OSD_DIRECT_ARGB4444      ((unsigned)0x2 << 1)
  ///bit[1:2] = 3: osd direct mode
  #define IMG_RESZ_OSD_DIRECT_ARGB8888      ((unsigned)0x3 << 1)
  
 #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
  ///bit[3] = 0: osd no repeating
  #define IMG_RESZ_OSD_NO_REPEATING         ((unsigned)0x0 << 3)
  ///bit[3] = 1: osd reapeating
  #define IMG_RESZ_OSD_REPEATING            ((unsigned)0x1 << 3)
  ///bit[4:5] = 0: osd scale type for alpha, normal
  #define IMG_RESZ_OSD_ALPHA_SCALE_NORMAL   ((unsigned)0x0 << 4)
  ///bit[4:5] = 1: osd scale type for alpha, refer left
  #define IMG_RESZ_OSD_ALPHA_SCALE_REF_LEFT ((unsigned)0x1 << 4)
  ///bit[4:5] = 2: osd scale type for alpha, refer nearest
  #define IMG_RESZ_OSD_ALPHA_SCALE_REF_NEAREST  ((unsigned)0x2 << 4)
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
     ///bit[7] = 1: bilinear mode for luma key/ video boundary in 4 & 8-tap filters
     #define IMG_RESZ_OSD_ALPHA_BILINEAR_BOUNDARY  ((unsigned)0x1 << 7)
     ///bit[6] = 1: only distinguish alpha
     #define IMG_RESZ_OSD_ONLY_DISTINGUISH_ALPHA   ((unsigned)0x1 << 6)
#endif  
  ///bit[8:9] = 0: osd output type
  #define IMG_RESZ_OSD_OUTPUT_RGB565        ((unsigned)0x0 << 8)
  ///bit[8:9] = 1: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB1555      ((unsigned)0x1 << 8)
  ///bit[8:9] = 2: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB4444      ((unsigned)0x2 << 8)
  ///bit[8:9] = 3: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB8888      ((unsigned)0x3 << 8)
  ///bit[11] = 0: osd no switch cb, cr
  #define IMG_RESZ_OSD_NOT_SWITCH_CBCR      ((unsigned)0x0 << 11)
  ///bit[11] = 1: osd switch cb, cr
  #define IMG_RESZ_OSD_SWITCH_CBCR          ((unsigned)0x1 << 11)
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)  
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
  #define IMG_RESZ_A_BLEND_SHIFT            24
#else
  #define IMG_RESZ_A_BLEND_SHIFT            12
#endif  

#else
  ///bit[4] = 0: osd no repeating
  #define IMG_RESZ_OSD_NO_REPEATING         ((unsigned)0x0 << 4)
  ///bit[4] = 1: osd reapeating
  #define IMG_RESZ_OSD_REPEATING            ((unsigned)0x1 << 4)
  ///bit[6:5] = 0: osd scale type for alpha, normal
  #define IMG_RESZ_OSD_ALPHA_SCALE_NORMAL   ((unsigned)0x0 << 5)
  ///bit[6:5] = 1: osd scale type for alpha, refer left
  #define IMG_RESZ_OSD_ALPHA_SCALE_REF_LEFT ((unsigned)0x1 << 5)
  ///bit[6:5] = 2: osd scale type for alpha, refer nearest
  #define IMG_RESZ_OSD_ALPHA_SCALE_REF_NEAREST  ((unsigned)0x2 << 5)
   ///bit[8] = 1: bilinear mode for luma key/ video boundary in 4 & 8-tap filters
   #define IMG_RESZ_OSD_ALPHA_BILINEAR_BOUNDARY  ((unsigned)0x1 << 8)
   ///bit[7] = 1: only distinguish alpha
   #define IMG_RESZ_OSD_ONLY_DISTINGUISH_ALPHA   ((unsigned)0x1 << 7)
  ///bit[10:9] = 0: osd output type
  #define IMG_RESZ_OSD_OUTPUT_RGB565        ((unsigned)0x0 << 9)
  ///bit[10:9] = 1: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB1555      ((unsigned)0x1 << 9)
  ///bit[10:9] = 2: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB4444      ((unsigned)0x2 << 9)
  ///bit[10:9] = 3: osd output type
  #define IMG_RESZ_OSD_OUTPUT_ARGB8888      ((unsigned)0x3 << 9)
  ///bit[12] = 0: osd no switch cb, cr
  #define IMG_RESZ_OSD_NOT_SWITCH_CBCR      ((unsigned)0x0 << 12)
  ///bit[12] = 1: osd switch cb, cr
  #define IMG_RESZ_OSD_SWITCH_CBCR          ((unsigned)0x1 << 12)  
  ///bit[14] = 0 ARGB don't -> RGBA
  #define IMG_RESZ_OSD_NOT_ARGB_RGBA        ((unsigned)0x0 << 14)
  ///bit[14] = 1: ARGB -> RGBA
  #define IMG_RESZ_OSD_ARGB_RGBA            ((unsigned)0x1 << 14)
  #define IMG_RESZ_A_BLEND_SHIFT            24
#endif
  /*! @} */

#define RW_IMG_RESZ_OSD_MD_CTRL 0x104
  /*! \name  RW_IMG_RESZ_OSD_MD_CTRL bit value description
  * \anchor RW_IMG_RESZ_OSD_MD_CTRL bit value description
  * bit[0]: access color table, write(0), read(1). \n
  * bit[1]: color pallet table enable. \n
  * @{
  */
  ///bit[0] = 0: osd write CPT
  #define IMG_RESZ_OSD_WR_CPT               ((unsigned)0x0 << 0)
  ///bit[0] = 0: osd read CPT
  #define IMG_RESZ_OSD_ED_CPT               ((unsigned)0x1 << 0)
  ///bit[0] = 0: osd CPT disable
  #define IMG_RESZ_OSD_CPT_DISABLE          ((unsigned)0x0 << 1)
  ///bit[0] = 0: osd CPT enable
  #define IMG_RESZ_OSD_CPT_ENABLE           ((unsigned)0x1 << 1)
  ///bit[28] = 1: osd ALU
  #define IMG_RESZ_OSD_ALU_ENABLE           ((unsigned)0x1 << 28)
  /*! @} */

#define RW_IMG_RESZ_OSD_ALPHA_TBL 0x108
  /*! \name  RW_IMG_RESZ_OSD_ALPHA_TBL bit value description
  * \anchor RW_IMG_RESZ_OSD_ALPHA_TBL bit value description
  * bit[31:0]: alpha table. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_COLOR_TRANSLATION0  0x10C
#define RW_IMG_RESZ_OSD_COLOR_TRANSLATION1  0x110
#define RW_IMG_RESZ_OSD_COLOR_TRANSLATION2  0x114
#define RW_IMG_RESZ_OSD_COLOR_TRANSLATION3  0x118
  /*! \name  RW_IMG_RESZ_OSD_COLOR_TRANSLATION bit value description
  * \anchor RW_IMG_RESZ_OSD_COLOR_TRANSLATION bit value description
  * bit[31:0]: color translation value. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_CPT_ADDR 0x11C
  /*! \name  RW_IMG_RESZ_OSD_CPT_ADDR bit value description
  * \anchor RW_IMG_RESZ_OSD_CPT_ADDR bit value description
  * bit[7:0]: CPT_WR_ADDR. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_CPT_DATA 0x120
  /*! \name  RW_IMG_RESZ_OSD_CPT_DATA bit value description
  * \anchor RW_IMG_RESZ_OSD_CPT_DATA bit value description
  * bit[21:0]: CPT_WR_DATA. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_H_COEF0     0x124
#define RW_IMG_RESZ_H_COEF1     0x128
#define RW_IMG_RESZ_H_COEF2     0x12C
#define RW_IMG_RESZ_H_COEF3     0x130
#define RW_IMG_RESZ_H_COEF4     0x134
#define RW_IMG_RESZ_H_COEF5     0x138
#define RW_IMG_RESZ_H_COEF6     0x13C
#define RW_IMG_RESZ_H_COEF7     0x140
#define RW_IMG_RESZ_H_COEF8     0x144
#define RW_IMG_RESZ_H_COEF9     0x148
#define RW_IMG_RESZ_H_COEF10    0x14C
#define RW_IMG_RESZ_H_COEF11    0x150
#define RW_IMG_RESZ_H_COEF12    0x154
#define RW_IMG_RESZ_H_COEF13    0x158
#define RW_IMG_RESZ_H_COEF14    0x15C
#define RW_IMG_RESZ_H_COEF15    0x160
#define RW_IMG_RESZ_H_COEF16    0x164
#define RW_IMG_RESZ_H_COEF17    0x168
  /*! \name  RW_IMG_RESZ_H_COEF bit value description
  * \anchor RW_IMG_RESZ_H_COEF bit value description
  * bit[31:0]: coefficients of horizontal 8-taps filter. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_V_COEF0     0x16C
#define RW_IMG_RESZ_V_COEF1     0x170
#define RW_IMG_RESZ_V_COEF2     0x174
#define RW_IMG_RESZ_V_COEF3     0x178
#define RW_IMG_RESZ_V_COEF4     0x17C
#define RW_IMG_RESZ_V_COEF5     0x180
#define RW_IMG_RESZ_V_COEF6     0x184
#define RW_IMG_RESZ_V_COEF7     0x188
#define RW_IMG_RESZ_V_COEF8     0x18C
  /*! \name  RW_IMG_RESZ_V_COEF bit value description
  * \anchor RW_IMG_RESZ_V_COEF bit value description
  * bit[31:0]: coefficients of vertical 4-taps filter. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_DITHER_SETTING 0x190
  /*! \name  RW_IMG_RESZ_OSD_DITHER_SETTING bit value description
  * \anchor RW_IMG_RESZ_OSD_DITHER_SETTING bit value description
  * bit[4:0]: frame phase. \n
  * bit[5]: LSB off setting, output 0(1). \n
  * bit[6]: rounding directly enable. \n
  * bit[7]: error dispersion enable. \n
  * bit[8]: run order dithering enable. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_OSD_CSC_SETTING 0x194
  /*! \name  RW_IMG_RESZ_OSD_CSC_SETTING bit value description
  * \anchor RW_IMG_RESZ_OSD_CSC_SETTING bit value description
  * bit[0]: color space conversion function enable. \n
  * bit[1]: input Y-16 before CSC. \n
  * bit[2]: input C-128 before CSC. \n
  * bit[3]: input Y+16 before CSC. \n
  * bit[4]: input C+128 before CSC. \n
  * @{
  */
  ///bit[0] = 1: color space conversion function enable.
  #define IMG_RESZ_OSD_CSC_ENABLE           ((unsigned)0x1 << 0)
  ///bit[1] = 1: input Y-16 before CSC.
  #define IMG_RESZ_OSD_CSC_YIN_D16          ((unsigned)0x1 << 1)
  ///bit[2] = 1: input C-128 before CSC.
  #define IMG_RESZ_OSD_CSC_CIN_D128         ((unsigned)0x1 << 2)
  ///bit[3] = 1: output Y+16 after CSC.
  #define IMG_RESZ_OSD_CSC_YOUT_A16         ((unsigned)0x1 << 3)
  ///bit[4] = 1: output C+128 after CSC.
  #define IMG_RESZ_OSD_CSC_COUT_A128        ((unsigned)0x1 << 4)

  /*! @} */

#define RW_IMG_RESZ_OSD_CSC_COEF11 0x198
#define RW_IMG_RESZ_OSD_CSC_COEF12 0x19C
#define RW_IMG_RESZ_OSD_CSC_COEF13 0x1A0
#define RW_IMG_RESZ_OSD_CSC_COEF21 0x1A4
#define RW_IMG_RESZ_OSD_CSC_COEF22 0x1A8
#define RW_IMG_RESZ_OSD_CSC_COEF23 0x1AC
#define RW_IMG_RESZ_OSD_CSC_COEF31 0x1B0
#define RW_IMG_RESZ_OSD_CSC_COEF32 0x1B4
#define RW_IMG_RESZ_OSD_CSC_COEF33 0x1B8
  /*! \name  RW_IMG_RESZ_OSD_CSC_COEF bit value description
  * \anchor RW_IMG_RESZ_OSD_CSC_COEF bit value description
  * bit[0:12]: color space conversion coefficient. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_LUMA_KEY 0x1BC
  /*! \name  RW_IMG_RESZ_OSD_LUMA_KEY bit value description
  * \anchor RW_IMG_RESZ_OSD_LUMA_KEY bit value description
  * bit[0]: enable supporting luma key. \n
  * bit[1:2]: scaling type for dearing with luma key boundary. \n
  * bit[8:15]: luma key. \n
  * @{
  */
  /// bit[0] = 1: enable supporting luma key.
  #define IMG_RESZ_LUMA_KEY_ENABLE              ((unsigned)0x1 << 0)
  ///bit[1:2] = 0: scale type for luma key, normal
  #define IMG_RESZ_LUMA_KEY_SCALE_NORMAL        ((unsigned)0x0 << 1)
  ///bit[1:2] = 1: scale type for luma key, refer left
  #define IMG_RESZ_LUMA_KEY_SCALE_REF_LEFT      ((unsigned)0x1 << 1)
  ///bit[1:2] = 2: scale type for luma key, refer nearest
  #define IMG_RESZ_LUMA_KEY_SCALE_REF_NEAREST   ((unsigned)0x2 << 1)
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
  ///bit[3] = 1: bilinear mode for luma key/ video boundary in 4 & 8-tap filters
  #define IMG_RESZ_LUMA_KEY_BILINEAR_BOUNDARY   ((unsigned)0x1 << 3)
#endif
#if(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
  ///bit[4] = 1: luma key scaling for YUV at the same time mode
  #define IMG_RESZ_LUMA_KEY_YUV_MODE   ((unsigned)0x1 << 4)
#endif
  ///bit[15:8] = luma key.
  #define IMG_RESZ_LUMA_KEY_SHIFT               8
  
  /*! @} */

//#ifndef IMGRESZ_HAL_MT8550
//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

#define RW_IMG_RESZ_WT_SET 0x1C4
  /*! \name  RW_IMG_RESZ_WT_SET bit value description
  * \anchor RW_IMG_RESZ_WT_SET bit value description
  * bit[0]:	 rz_rgb_yuv_r   (read format) \n
  * bit[4]:	 rz_rgb_yuv_w   (write format) \n
  * bit[8]:  rz_wt_bypass_r (read bypass) \n
  * bit[11]: rz_wt_bypass_w (write bypass) \n
  * @{
  */

  ///bit[0] = 1: read format AYUV
  #define IMG_RESZ_WT_READ_FORMAT_AYUV      ((unsigned)0x1 << 0)
  ///bit[4] = 1: write format AYUV
  #define IMG_RESZ_WT_WRITE_FORMAT_AYUV     ((unsigned)0x1 << 4)
  ///bit[8] = 1: read bypass (disable read WT)
  #define IMG_RESZ_WT_READ_BYPASS           ((unsigned)0x1 << 8)
  ///bit[11] = 1: write bypass (disable write WT)
  #define IMG_RESZ_WT_WRITE_BYPASS          ((unsigned)0x1 << 12)
  /*! @} */
//#endif
#else
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)

#define RW_IMG_RESZ_WT_SET 0x1D4
  /*! \name  RW_IMG_RESZ_WT_SET bit value description
  * \anchor RW_IMG_RESZ_WT_SET bit value description
  * bit[0]:	 rz_rgb_yuv_r   (read format) \n
  * bit[4]:	 rz_rgb_yuv_w   (write format) \n
  * bit[8]:  rz_wt_bypass_r (read bypass) \n
  * bit[11]: rz_wt_bypass_w (write bypass) \n
  * @{
  */

  ///bit[0] = 1: read format AYUV
  #define IMG_RESZ_WT_READ_FORMAT_AYUV      ((unsigned)0x1 << 0)
  ///bit[4] = 1: write format AYUV
  #define IMG_RESZ_WT_WRITE_FORMAT_AYUV     ((unsigned)0x1 << 4)
  ///bit[8] = 1: read bypass (disable read WT)
  #define IMG_RESZ_WT_READ_BYPASS           ((unsigned)0x1 << 8)
  ///bit[11] = 1: write bypass (disable write WT)
  #define IMG_RESZ_WT_WRITE_BYPASS          ((unsigned)0x1 << 12)

#define RW_IMG_RESZ_RPR        0x1D0

  #define IMG_RESZ_RPR_FLAG_ON              ((unsigned)0x1 << 0)

  #define IMG_RESZ_URCRPR_ON                ((unsigned)0x1 << 1)

  #define IMG_RESZ_TRC_VDEC_EN              ((unsigned)0x1 << 8)

  #define IMG_RESZ_TRC_VDEC_INT             ((unsigned)0x1 << 9)

#define RW_IMG_RESZ_FLIP       0x1D8

  #define IMG_RESZ_OUT_FLIP_ON              ((unsigned)0x1 << 0)
#endif
#endif

//#ifndef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8550)

#define RW_IMG_RESZ_HW_STATUS_CPU_ASSIGN  0x1D8
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_Y   0x1DC
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_CB  0x1E0
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_CR  0x1E4
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_Y  0x1E8
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_CB 0x1EC
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_CR 0x1F0
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_Y  0x1F4
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CB 0x1F8
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CR 0x1FC
  /*! \name  RW_IMG_RESZ_HW_STATUS bit value description
  * \anchor RW_IMG_RESZ_HW_STATUS bit value description
  *
  * @{
  */
  /*! @} */
#else
#define RW_IMG_RESZ_HW_STATUS_CPU_ASSIGN  0x0A0
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_Y   0x0A4
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_CB  0x0A8
#define RW_IMG_RESZ_HW_STATUS_SRC_CNT_CR  0x0AC
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_Y  0x0B0
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_CB 0x0B4
#define RW_IMG_RESZ_HW_STATUS_V_OFFSET_CR 0x0B8
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_Y  0x0BC
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CB 0x0C0
#define RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CR 0x0C4
  /*! \name  RW_IMG_RESZ_HW_STATUS bit value description
  * \anchor RW_IMG_RESZ_HW_STATUS bit value description
  *
  * @{
  */
  /*! @} */
#endif

//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)

#define RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF 0x1C4
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Y : the line number in Y color buffer. \n
  * @{
  */
  /*! @} */

#define RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF 0x1C8
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Cb : the line number in Cb color buffer. \n
  * @{
  */
  /*! @} */
  
#define RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF 0x1CC
  /*! \name  RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF bit value description
  * \anchor RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF bit value description
  * only supported in jpeg mode
  * bit[11:0]: (12'h0) Line_num_Cr : the line number in Cr color buffer. \n
  * @{
  */
  /*! @} */
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
#define RW_IMG_RESZ_VENC_SKIP 0x1EC
    #define RM_IMG_RESZ_VENC_SKIP_ON (1 << 0)
#endif
  
#endif


#define RW_IMG_RESZ_FLIP_FUNC 0x1D8

//bit[0] = 0: Close Flip function
#define RW_IMG_RESZ_FLIP_CLOSE   (0x0<<0)
//bit[0] = 1: OPEN Flip function
#define RW_IMG_RESZ_FLIP_OPEN   (0x1<<0)

#define RW_IMG_RESZ_ALPHA_COMPOSITION 0x200

//HW0 or HW2
#define RW_IMG_RESZ_UFO_CFG 0x200
  /// bit[0] = 1: enable supporting luma key.
  #define IMG_RESZ_UFO_R_ARGB0AG0TIM    // ((unsigned)0x1 << 0)
  #define IMG_RESZ_UFO_R_ARGB0AG1TIM    // ((unsigned)0x1 << 0)
  #define IMG_RESZ_UFO_R_ARGB0AG2TIM    // ((unsigned)0x1 << 0)
  #define IMG_RESZ_UFO_UFO_EN                 ((unsigned)0x1 << 12)
  #define IMG_RESZ_UFO_UFO_CFG_8BIT           ((unsigned)0x0 << 13)
  #define IMG_RESZ_UFO_UFO_CFG_10BIT_REORDER  ((unsigned)0x1 << 13)
  #define IMG_RESZ_UFO_UFO_CFG_10BIT_COMPACT  ((unsigned)0x3 << 13)
  #define IMG_RESZ_UFO_UFO_10BIT_BYPASS_EN    ((unsigned)0x1 << 15)
  #define IMG_RESZ_UFO_UFO_10BIT_BYPASS_DIS   ((unsigned)0x0 << 15)
  #define IMG_RESZ_UFO_UFO_16BIT_BYPASS_EN    ((unsigned)0x1 << 16)
  #define IMG_RESZ_UFO_UFO_16BIT_BYPASS_DIS   ((unsigned)0x0 << 16)

#define RW_IMG_RESZ_UFO_YBUF_LEN 0x22C
#define RW_IMG_RESZ_UFO_CBUF_LEN 0x230

typedef enum {
  FACTOR_0 = 0,               ///< factor 0
  FACTOR_0_0625,           ///< factor 0.0625
  FACTOR_0_125,             ///< factor 0.125
  FACTOR_0_25,               ///< factor 0.25
  FACTOR_0_5,                 ///< factor 0.5
  FACTOR_1,                     ///< factor 1
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)  
  FACTOR_RM,                   /// <for RM mode
#endif
  FACTOR_MAX                 ///< don't use it
}IMGRZ_SCALE_FACTOR;

typedef enum
{
    E_AC_OP_CLEAR = 0,
    E_AC_OP_DST_IN,
    E_AC_OP_DST_OUT,
    E_AC_OP_DST_OVER,
    E_AC_OP_SRC,
    E_AC_OP_SRC_IN,
    E_AC_OP_SRC_OUT,
    E_AC_OP_SRC_OVER,
    E_AC_OP_DST,
    E_AC_OP_SRC_ATOP,
    E_AC_OP_DST_ATOP,
    E_AC_OP_XOR,
    E_AC_OP_NONE,
    E_AC_OP_ADD = 13,
} IMGRESZ_AC_OP_MODE_T;

//#if defined(IMGRESZ_HAL_MT8550) 
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
extern void vHwImgIntResize(UINT32 u4HwId);//sun for turning on interrupt
extern void vHwImgReszWaitCountReset(UINT32 u4HwId);
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
extern void vHwImgRezeDMAReset(UINT32 u4HwId);
extern void vHwImgRezeSetWaitWRDone(UINT32 u4HwId);
#endif

extern void vHwImgRezeReset(UINT32 u4HwId);
extern void vHwImgReszEnable(UINT32 u4HwId);
extern void vHwImgReszDisable(UINT32 u4HwId);
extern void vHwImgReszResize(UINT32 u4HwId);
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
extern BOOL fgHwImgReszResizeWorking(UINT32 u4HwId);
extern BOOL fgHwImgReszInterruptExist(UINT32 u4HwId);
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
extern BOOL fgHwImgReszDMARstFinish(UINT32 u4HwId);
#endif

extern BOOL fgHwImgReszResizeFinish(UINT32 u4HwId);
extern INT32 i4HwImgReszSetResizeMode(UINT32 u4HwId,
                                      IMGRESZ_HAL_RESIZE_MODE_T eResizeMode,
                                      IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
extern INT32 i4HwImgReszSetResampleMethod(UINT32 u4HwId,
                                          IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod,
                                          IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod);
extern INT32 i4HwImgReszSetSrcBufFormat(UINT32 u4HwId,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
extern INT32 i4HwImgReszSetSrcBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2, UINT32 u4BufSA3);
extern INT32 i4HwImgReszSetSrcPrevRowBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2, UINT32 u4BufSA3);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
extern INT32 i4HwImgReszSetSrcBufWidth(UINT32 u4HwId,UINT32 u4BufWidth, IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
#else
extern INT32 i4HwImgReszSetSrcBufWidth(UINT32 u4HwId,UINT32 u4BufWidth);
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern INT32 i4HwImgReszSetSrcRowBufHeight(UINT32 u4HwId,UINT32 u4RowBufHeight,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat, BOOL fgRPRRacingModeEnable);
#else
extern INT32 i4HwImgReszSetSrcRowBufHeight(UINT32 u4HwId,UINT32 u4RowBufHeight,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
#endif
extern INT32 i4HwImgReszSetSrcImageWidthHeight(UINT32 u4HwId,UINT32 u4Width,UINT32 u4Height,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
extern INT32 i4HwImgReszSetSrcImageOffset(UINT32 u4HwId,UINT32 u4XOff,UINT32 u4YOff,
                                   IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                   UINT32 u4SrcHeight,UINT32 u4TgtHeight);
extern INT32 i4HwImgReszSetSrcFirstRow(UINT32 u4HwId,BOOL fgFirstRow);
extern INT32 i4HwImgReszSetSrcLastRow(UINT32 u4HwId,BOOL fgLastRow);
extern INT32 i4HwImgReszSetDestBufFormat(UINT32 u4HwId,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                                IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat);
extern INT32 i4HwImgReszSetDestBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2);
extern INT32 i4HwImgReszSetDestBufWidth(UINT32 u4HwId,UINT32 u4BufWidth);
extern INT32 i4HwImgReszSetDestImageWidthHeight(UINT32 u4HwId,UINT32 u4Width,UINT32 u4Height);
extern INT32 i4HwImgReszSetDestImageOffset(UINT32 u4HwId,UINT32 u4XOff,UINT32 u4YOff);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

extern INT32 i4HwImgReszSetTmpLineBufLen(UINT32 u4HwId,IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod,
                                  UINT32 u4SrcHeight,UINT32 u4DestHeight,UINT32 u4DestWidth,
                                  BOOL fgRPRMode,
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)                              
                                  BOOL fgOSDMode,
#endif                                  
                                  BOOL fgLumaKeyEnable);
#else
extern INT32 i4HwImgReszSetTmpLineBufLen(UINT32 u4HwId,IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod,
                                  UINT32 u4SrcHeight,UINT32 u4DestHeight,UINT32 u4DestWidth);

#endif

extern INT32 i4HwImgReszSetTempBufAddr(UINT32 u4HwId,UINT32 u4Addr);
extern INT32 i4HwImgReszSetJpegPicMode(UINT32 u4HwId,BOOL fgPicMode);
extern INT32 i4HwImgReszSetJpegPreloadMode(UINT32 u4HwId,BOOL fgPreloadMode);
extern INT32 i4HwImgReszSetJpegComponent(UINT32 u4HwId,BOOL fgYExist,BOOL fgCbExist,BOOL fgCrExist);
extern INT32 i4HwImgReszSetJpegComponentExt(UINT32 u4HwId,BOOL fgYExist,BOOL fgCbExist,BOOL fgCrExist,BOOL fgRPRRacingModeEnable);
extern INT32 i4HwImgReszSetCbCrExit(UINT32 u4HwId, BOOL fgCb, BOOL fgCr);
extern INT32 i4HwImgReszSetHScaleFactor(UINT32 u4HwId,UINT32 u4SrcWidth,UINT32 u4TgtWidth,
                                 IMGRESZ_HAL_RESAMPLE_METHOD_T eResampleMethod,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat);
#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
extern INT32 i4HwImgReszSetVScaleFactorWebpTap4(UINT32 u4HwId,
                                         UINT32 u4SrcHeight,
                                         UINT32 u4TgtHeight,
                                         UINT32 u4TgtStartY);
#endif                                 
extern INT32 i4HwImgReszSetVScaleFactor(UINT32 u4HwId,
                                 UINT32 u4SrcHeight,UINT32 u4TgtHeight,
                                 IMGRESZ_HAL_RESAMPLE_METHOD_T eResampleMethod,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat);
extern INT32 i4HwImgReszSetAlphaBlendingLevel(UINT32 u4HwId,UINT32 u4Alpha);
extern INT32 i4HwImgReszSetPreloadBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2);
extern INT32 i4HwImgReszSetAlphaChangeScalingType(UINT32 u4HwId,UINT32 u4ScalingType);
extern INT32 i4HwImgReszSetLumaKeyEnable(UINT32 u4HwId,BOOL fgEnable);
extern INT32 i4HwImgReszSetLumaKeyScalingType(UINT32 u4HwId,UINT32 u4ScalingType);
extern INT32 i4HwImgReszSetLumaKey(UINT32 u4HwId,UINT8 u1LumaKey);
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
extern INT32 i4HwImgReszSetAlphaChangeBilinearBoundary(UINT32 u4HwId,BOOL fgBilinearBoundary);
extern INT32 i4HwImgReszSetOnlyDistinguishAlpha(UINT32 u4HwId,BOOL fgOnlyDistinguishAlpha);
extern INT32 i4HwImgReszSetLumaKeyBilinearBoundary(UINT32 u4HwId,BOOL fgBilinearBoundary);
extern void vHwImgReszReadBurstLength(UINT32 u4HwId,BOOL fgEnable);
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
extern INT32 i4HwImgReszSetLumaKeyYUVMode(UINT32 u4HwId,BOOL fgYUVMode);
#endif
extern INT32 i4HwImgReszSetDramReqBurstLimit(UINT32 u4HwId,UINT32 u4Limit);
extern INT32 i4HwImgReszSetIndexBufColorTranslation(UINT32 u4HwId);
extern INT32 i4HwImgReszSetColorPalletTable(UINT32 u4HwId,UINT32 u4ColorPalletTableAddr);
extern INT32 i4HwImgReszGetHwStatus(UINT32 u4HwId,IMGRESZ_HAL_HW_STATUS_T *prHwStatus);
extern INT32 i4HwImgReszSetHwStatus(UINT32 u4HwId,IMGRESZ_HAL_HW_STATUS_T *prHwStatus);
extern INT32 i4HwImgReszResetDestPartialBuf(UINT32 u4HwId, BOOL fgReset);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern INT32 i4HwImgReszSetRPR(UINT32 u4HwId,BOOL fgRPRMode, BOOL fgRPRRacingModeEnable);
extern INT32 i4HwImgReszSetRPRHScaleFactor(UINT32 u4HwId,INT32 i4SrcWidth,INT32 i4TgtWidth); //sun new
extern INT32 i4HwImgReszSetRPRVScaleFactor(UINT32 u4HwId,INT32 i4SrcHeight,INT32 i4TgtHeight);//sun new
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561)
extern INT32 i4HwImgReszSetVENCSKIP(UINT32 u4HwId,BOOL fgSkip);
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
extern void vHwImgReszEnableMMU(UINT32 u4HwId);
extern void vHwImgReszDisableMMU(UINT32 u4HwId);
#endif
#endif
extern INT32 i4HwImgReszSetScale1to1Factor(UINT32 u4HwId,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
extern INT32 i4HwImgReszGetHwCRC(UINT32 u4HwId);
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561) 
 extern void  vHwImgReszSetPreloadBuf_iommu(UINT32 u4HwId);
#endif		
 extern void i4HwImgReszSetDstPitchEnable(UINT32 u4HwId,BOOL fgPitchEnable,UINT32 u4PitchByte,UINT32 u4BufAddr_Byte);
INT32 i4HwImgReszSetARGB2RGBA(UINT32 u4HwId, BOOL fgArgb2Rgba);
INT32 i4HwImgReszSetVideoCbCrSwap(UINT32 u4HwId, BOOL fgCbCrSwap);
INT32 i4HwImgReszSetAYUVCbCrSwap(UINT32 u4HwId, BOOL fgCbCrSwap);
INT32 i4HwImgReszSetOSDFLIP(UINT32 u4HwId, BOOL fgFlipOpen);
INT32 i4HwImgReszSetAYUVYONLY(UINT32 u4HwId, BOOL fgYOnly);
INT32 i4HwImgReszSetAlphaCompositionEnable(UINT32 u4HwId, BOOL fgEnable);
INT32 i4HwImgReszSetAlphaCompositionRange(UINT32 u4HwId, UINT32 i4AlphaComRange);
INT32 i4HwImgReszSetAlphaCompositionOpt(UINT32 u4HwId, UINT32 i4AlphaComOpt);
INT32 i4HwImgReszPerformaceGetCycle(UINT32 u4HwId);
INT32 i4HwImgReszSetAlphaComDebugEnable(UINT32 u4HwId, BOOL fgDebug);

INT32 i4HwImgReszUFO_Enable(UINT32 u4HwId,BOOL fgEnable);
INT32 i4HwImgReszUFO_Config(UINT32 u4HwId,IMGRESZ_UFO_CFG_TYPE eCfgType);
INT32 i4HwImgReszUFO_SizeBuf_Addr(UINT32 u4HwId, UINT32 u4YBufSZSA, UINT32 u4CBufSZSA);

#endif
